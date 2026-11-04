# Merge every package archive produced by the dist target into one game
# archive. Packages and their contents are discovered, not listed by name:
# adding a plugin or module package needs no edit here or in the workflow.
foreach(required_var ABC_OUTPUT_DIR ABC_README ABC_BUNDLE_ARCHIVE
        ABC_PLUGIN_MODULES)
    if(NOT DEFINED ${required_var} OR "${${required_var}}" STREQUAL "")
        message(FATAL_ERROR "BundlePackages.cmake: missing ${required_var}")
    endif()
endforeach()

if(NOT EXISTS "${ABC_README}")
    message(FATAL_ERROR "BundlePackages.cmake: README not found at ${ABC_README}")
endif()

file(GLOB ABC_PACKAGE_ARCHIVES "${ABC_OUTPUT_DIR}/*.zip")
list(FILTER ABC_PACKAGE_ARCHIVES EXCLUDE REGEX "/ABCEnchance-[^/]*\\.zip$")
if(NOT ABC_PACKAGE_ARCHIVES)
    message(FATAL_ERROR
        "BundlePackages.cmake: no package archives in ${ABC_OUTPUT_DIR}")
endif()
list(SORT ABC_PACKAGE_ARCHIVES)

set(ABC_STAGE_DIR "${ABC_OUTPUT_DIR}/.bundle")
file(REMOVE_RECURSE "${ABC_STAGE_DIR}")
file(MAKE_DIRECTORY "${ABC_STAGE_DIR}")

foreach(ABC_ARCHIVE IN LISTS ABC_PACKAGE_ARCHIVES)
    get_filename_component(ABC_ARCHIVE_NAME "${ABC_ARCHIVE}" NAME)
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar tf "${ABC_ARCHIVE}"
        OUTPUT_VARIABLE ABC_ARCHIVE_ENTRIES
        ERROR_VARIABLE ABC_ARCHIVE_ERROR
        RESULT_VARIABLE ABC_ARCHIVE_RESULT)
    if(NOT ABC_ARCHIVE_RESULT EQUAL 0)
        message(FATAL_ERROR
            "BundlePackages.cmake: cannot list ${ABC_ARCHIVE_NAME}: ${ABC_ARCHIVE_ERROR}")
    endif()
    if(ABC_ARCHIVE_ENTRIES STREQUAL "")
        message(FATAL_ERROR
            "BundlePackages.cmake: ${ABC_ARCHIVE_NAME} is empty")
    endif()
    # Every package puts the game tree at its archive root so the release can
    # be extracted directly; README/LICENSE entries at the root are allowed.
    string(FIND "${ABC_ARCHIVE_ENTRIES}" "svencoop/" ABC_SVENCOOP_OFFSET)
    if(ABC_SVENCOOP_OFFSET EQUAL -1)
        message(FATAL_ERROR
            "BundlePackages.cmake: ${ABC_ARCHIVE_NAME} has no svencoop/ root")
    endif()
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar xf "${ABC_ARCHIVE}"
        WORKING_DIRECTORY "${ABC_STAGE_DIR}"
        ERROR_VARIABLE ABC_ARCHIVE_ERROR
        RESULT_VARIABLE ABC_ARCHIVE_RESULT)
    if(NOT ABC_ARCHIVE_RESULT EQUAL 0)
        message(FATAL_ERROR
            "BundlePackages.cmake: cannot extract ${ABC_ARCHIVE_NAME}: ${ABC_ARCHIVE_ERROR}")
    endif()
    message(STATUS "Bundled ${ABC_ARCHIVE_NAME}")
endforeach()

# Structural checks: discover the artifacts instead of naming the packages that
# are expected to provide them. A package that stops producing its payload
# fails here; a new plugin or module is accepted without touching this script.
file(GLOB_RECURSE ABC_PLUGIN_DLLS
    "${ABC_STAGE_DIR}/svencoop/metahook/plugins/*.dll")
if(NOT ABC_PLUGIN_DLLS)
    message(FATAL_ERROR
        "BundlePackages.cmake: merged package has no plugin DLL under "
        "svencoop/metahook/plugins")
endif()
list(LENGTH ABC_PLUGIN_MODULES ABC_PLUGIN_MODULE_COUNT)
list(LENGTH ABC_PLUGIN_DLLS ABC_PLUGIN_DLL_COUNT)
if(ABC_PLUGIN_DLL_COUNT LESS ABC_PLUGIN_MODULE_COUNT)
    message(FATAL_ERROR
        "BundlePackages.cmake: merged package has ${ABC_PLUGIN_DLL_COUNT} "
        "plugin DLL(s) for ${ABC_PLUGIN_MODULE_COUNT} plugin package(s)")
endif()
file(GLOB_RECURSE ABC_SHARED_DLLS
    "${ABC_STAGE_DIR}/svencoop/metahook/dlls/*.dll")
if(NOT ABC_SHARED_DLLS)
    message(FATAL_ERROR
        "BundlePackages.cmake: merged package has no shared DLL under "
        "svencoop/metahook/dlls")
endif()

set(ABC_GAMEDATA_INDEX
    "${ABC_STAGE_DIR}/svencoop/metahook/gamedata/abc/index.json")
if(NOT EXISTS "${ABC_GAMEDATA_INDEX}")
    message(FATAL_ERROR
        "BundlePackages.cmake: merged package has no gamedata catalog at "
        "svencoop/metahook/gamedata/abc/index.json")
endif()

set(ABC_MODULES_DIR "${ABC_STAGE_DIR}/svencoop/abc/abcui/modules")
file(GLOB ABC_MODULE_PACKAGES
    "${ABC_MODULES_DIR}/*.zip"
    "${ABC_MODULES_DIR}/*.disable")
if(NOT ABC_MODULE_PACKAGES)
    message(FATAL_ERROR
        "BundlePackages.cmake: merged package has no UI module package under "
        "svencoop/abc/abcui/modules")
endif()
list(SORT ABC_MODULE_PACKAGES)

set(ABC_MODULE_CHECK_DIR "${ABC_STAGE_DIR}/.module-check")
foreach(ABC_MODULE_PACKAGE IN LISTS ABC_MODULE_PACKAGES)
    get_filename_component(ABC_MODULE_NAME "${ABC_MODULE_PACKAGE}" NAME)
    file(REMOVE_RECURSE "${ABC_MODULE_CHECK_DIR}")
    file(MAKE_DIRECTORY "${ABC_MODULE_CHECK_DIR}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar xf "${ABC_MODULE_PACKAGE}"
        WORKING_DIRECTORY "${ABC_MODULE_CHECK_DIR}"
        ERROR_VARIABLE ABC_MODULE_ERROR
        RESULT_VARIABLE ABC_MODULE_RESULT)
    if(NOT ABC_MODULE_RESULT EQUAL 0)
        message(FATAL_ERROR
            "BundlePackages.cmake: cannot extract ${ABC_MODULE_NAME}: ${ABC_MODULE_ERROR}")
    endif()
    set(ABC_MODULE_MANIFEST "${ABC_MODULE_CHECK_DIR}/manifest.json")
    if(NOT EXISTS "${ABC_MODULE_MANIFEST}")
        message(FATAL_ERROR
            "BundlePackages.cmake: ${ABC_MODULE_NAME} has no manifest.json")
    endif()
    file(READ "${ABC_MODULE_MANIFEST}" ABC_MODULE_JSON)
    string(JSON ABC_MODULE_ID
        ERROR_VARIABLE ABC_MODULE_JSON_ERROR
        GET "${ABC_MODULE_JSON}" id)
    if(ABC_MODULE_JSON_ERROR OR ABC_MODULE_ID STREQUAL "")
        message(FATAL_ERROR
            "BundlePackages.cmake: ${ABC_MODULE_NAME} has no module id")
    endif()
    string(JSON ABC_MODULE_TYPE
        ERROR_VARIABLE ABC_MODULE_TYPE_ERROR
        GET "${ABC_MODULE_JSON}" type)
    if(ABC_MODULE_TYPE_ERROR)
        set(ABC_MODULE_TYPE "module")
    endif()
    if(NOT ABC_MODULE_TYPE STREQUAL "resources")
        string(JSON ABC_MODULE_ENTRY
            ERROR_VARIABLE ABC_MODULE_ENTRY_ERROR
            GET "${ABC_MODULE_JSON}" entry)
        if(ABC_MODULE_ENTRY_ERROR OR ABC_MODULE_ENTRY STREQUAL "")
            message(FATAL_ERROR
                "BundlePackages.cmake: ${ABC_MODULE_NAME} has no entry")
        endif()
        if(NOT EXISTS
                "${ABC_MODULE_CHECK_DIR}/${ABC_MODULE_ENTRY}")
            message(FATAL_ERROR
                "BundlePackages.cmake: ${ABC_MODULE_NAME} is missing its entry "
                "${ABC_MODULE_ENTRY}")
        endif()
    endif()
    message(STATUS "Validated module ${ABC_MODULE_ID} (${ABC_MODULE_NAME})")
endforeach()
file(REMOVE_RECURSE "${ABC_MODULE_CHECK_DIR}")

file(REMOVE "${ABC_STAGE_DIR}/README.md")
file(COPY_FILE "${ABC_README}" "${ABC_STAGE_DIR}/README.md")

file(REMOVE "${ABC_BUNDLE_ARCHIVE}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar cf "${ABC_BUNDLE_ARCHIVE}"
        --format=zip -- svencoop README.md
    WORKING_DIRECTORY "${ABC_STAGE_DIR}"
    ERROR_VARIABLE ABC_BUNDLE_ERROR
    RESULT_VARIABLE ABC_BUNDLE_RESULT)
if(NOT ABC_BUNDLE_RESULT EQUAL 0)
    message(FATAL_ERROR
        "BundlePackages.cmake: cannot create ${ABC_BUNDLE_ARCHIVE}: ${ABC_BUNDLE_ERROR}")
endif()

list(LENGTH ABC_PACKAGE_ARCHIVES ABC_PACKAGE_COUNT)
list(LENGTH ABC_MODULE_PACKAGES ABC_MODULE_COUNT)
file(REMOVE_RECURSE "${ABC_STAGE_DIR}")
message(STATUS
    "Bundled ${ABC_PACKAGE_COUNT} package(s) and ${ABC_MODULE_COUNT} UI "
    "module(s) into ${ABC_BUNDLE_ARCHIVE}")
