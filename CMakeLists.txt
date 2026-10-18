cmake_minimum_required(VERSION 4.2)

project(ABCEnchance LANGUAGES CXX)

include(ExternalProject)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(CMAKE_GENERATOR_PLATFORM AND NOT CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
    message(FATAL_ERROR "The ABC modules are Win32/x86 only; configure with -A Win32")
endif()

if(CMAKE_SIZEOF_VOID_P AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The ABC modules are Win32/x86 only")
endif()

# AbcCore, AbcUi and AbcEssential are standalone CMake projects. Build every
# module in an isolated binary directory, then expose one aggregate target for
# the whole family. The source directories default to the submodules but can be
# overridden with -DABC_*_SOURCE_DIR for local, unpushed checkouts.
set(ABC_CORE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/AbcCore" CACHE PATH
    "AbcCore source tree")
set(ABC_ESSENTIAL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/AbcEssential" CACHE PATH
    "AbcEssential source tree")
set(ABC_UI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/AbcUi" CACHE PATH
    "AbcUi source tree")
foreach(source_var ABC_CORE_SOURCE_DIR ABC_ESSENTIAL_SOURCE_DIR ABC_UI_SOURCE_DIR)
    if(NOT EXISTS "${${source_var}}/CMakeLists.txt")
        message(FATAL_ERROR
            "${source_var} does not contain a CMake project: ${${source_var}}")
    endif()
endforeach()

set(ABC_MODULE_BUILD_DIR "${CMAKE_BINARY_DIR}/modules")

set(ABC_MODULE_GENERATOR_ARGS)
if(CMAKE_GENERATOR)
    list(APPEND ABC_MODULE_GENERATOR_ARGS -G "${CMAKE_GENERATOR}")
endif()
if(CMAKE_GENERATOR_PLATFORM)
    list(APPEND ABC_MODULE_GENERATOR_ARGS -A "${CMAKE_GENERATOR_PLATFORM}")
endif()
if(CMAKE_GENERATOR_TOOLSET)
    list(APPEND ABC_MODULE_GENERATOR_ARGS -T "${CMAKE_GENERATOR_TOOLSET}")
endif()
if(CMAKE_GENERATOR_INSTANCE)
    file(TO_CMAKE_PATH "${CMAKE_GENERATOR_INSTANCE}" ABC_MODULE_GENERATOR_INSTANCE)
    list(APPEND ABC_MODULE_GENERATOR_ARGS
        "-DCMAKE_GENERATOR_INSTANCE:STRING=${ABC_MODULE_GENERATOR_INSTANCE}")
endif()

set(ABC_MODULE_CMAKE_ARGS ${ABC_MODULE_GENERATOR_ARGS})
list(APPEND ABC_MODULE_CMAKE_ARGS
    "-DABC_CORE_DIR:PATH=${ABC_CORE_SOURCE_DIR}")
if(DEFINED ABC_GAME_DIR AND NOT ABC_GAME_DIR STREQUAL "")
    file(TO_CMAKE_PATH "${ABC_GAME_DIR}" ABC_MODULE_GAME_DIR)
    list(APPEND ABC_MODULE_CMAKE_ARGS "-DABC_GAME_DIR:STRING=${ABC_MODULE_GAME_DIR}")
endif()

if(CMAKE_CONFIGURATION_TYPES)
    set(ABC_MODULE_CONFIG "$<CONFIG>")
    set(ABC_MODULE_CONFIG_ARGS --config "$<CONFIG>")
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    endif()
    set(ABC_MODULE_CONFIG "${CMAKE_BUILD_TYPE}")
    set(ABC_MODULE_CONFIG_ARGS --config "${CMAKE_BUILD_TYPE}")
    list(APPEND ABC_MODULE_CMAKE_ARGS "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}")
endif()

function(abc_add_module module source_dir)
    set(module_source "${source_dir}")
    set(module_binary "${ABC_MODULE_BUILD_DIR}/${module}")

    ExternalProject_Add(${module}
        SOURCE_DIR "${module_source}"
        BINARY_DIR "${module_binary}"
        PREFIX "${module_binary}"
        STAMP_DIR "${module_binary}/stamp"
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND
            "${CMAKE_COMMAND}"
            -S "${module_source}"
            -B "${module_binary}"
            ${ABC_MODULE_CMAKE_ARGS}
        BUILD_COMMAND
            "${CMAKE_COMMAND}"
            --build "${module_binary}"
            ${ABC_MODULE_CONFIG_ARGS}
            --parallel
        INSTALL_COMMAND ""
        BUILD_ALWAYS ON
        USES_TERMINAL_CONFIGURE TRUE
        USES_TERMINAL_BUILD TRUE
        LOG_CONFIGURE TRUE
        LOG_BUILD TRUE
    )
endfunction()

abc_add_module(AbcCore "${ABC_CORE_SOURCE_DIR}")
abc_add_module(AbcEssential "${ABC_ESSENTIAL_SOURCE_DIR}")
abc_add_module(AbcUi "${ABC_UI_SOURCE_DIR}")

set(ABC_SHARED_GAMEDATA_DIR "${CMAKE_BINARY_DIR}/assets/svencoop/metahook/gamedata/abc")
add_custom_target(SharedGameData
    COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/build-shared-gamedata.py"
        --core-dir "${ABC_CORE_SOURCE_DIR}"
        --manifest "${CMAKE_CURRENT_SOURCE_DIR}/scripts/manifests/abc.json"
        --layout-dir "${ABC_UI_SOURCE_DIR}/assets/gamedata/abcui-layout"
        --output "${ABC_SHARED_GAMEDATA_DIR}"
        --temp-root "${CMAKE_BINARY_DIR}/gamedata-sync"
    COMMENT "Building the shared ABC gamedata catalog"
    VERBATIM)

add_custom_target(ABCEnchance ALL
    DEPENDS AbcCore AbcEssential AbcUi
    COMMENT "Building AbcCore, AbcEssential and AbcUi")

# Package the three modules into <build>/dist:
#   AbcCore-<version>.zip
#   AbcEssential-<version>.zip
#   AbcUi-<version>.zip
add_custom_target(dist
    COMMAND "${CMAKE_COMMAND}"
        "-DABC_CORE_SOURCE_DIR=${ABC_CORE_SOURCE_DIR}"
        "-DABC_CORE_BUILD_DIR=${ABC_MODULE_BUILD_DIR}/AbcCore"
        "-DABC_CORE_CONFIG=${ABC_MODULE_CONFIG}"
        "-DABC_CORE_OUTPUT_DIR=${CMAKE_BINARY_DIR}/dist"
        "-DABC_CORE_SHARED_GAMEDATA_DIR=${ABC_SHARED_GAMEDATA_DIR}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/PackageAbcCore.cmake"
    COMMAND "${CMAKE_COMMAND}"
        --build "${ABC_MODULE_BUILD_DIR}/AbcEssential"
        ${ABC_MODULE_CONFIG_ARGS}
        --target dist
    COMMAND "${CMAKE_COMMAND}"
        --build "${ABC_MODULE_BUILD_DIR}/AbcUi"
        ${ABC_MODULE_CONFIG_ARGS}
        --target dist
    COMMAND "${CMAKE_COMMAND}"
        "-DABC_MODULE_BUILD_DIR=${ABC_MODULE_BUILD_DIR}"
        "-DABC_OUTPUT_DIR=${CMAKE_BINARY_DIR}/dist"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CollectPackages.cmake"
    DEPENDS AbcCore AbcEssential AbcUi SharedGameData
    COMMENT "Packaging AbcCore, AbcEssential and AbcUi"
    VERBATIM)

# Deploy the built family into the game root. The individual module install
# targets are not used here because ExternalProject builds have their own
# install prefixes; this target copies the exact build outputs.
if(ABC_GAME_DIR)
    file(TO_CMAKE_PATH "${ABC_GAME_DIR}" ABC_DEPLOY_GAME_DIR)
    add_custom_target(deploy
        DEPENDS AbcCore AbcEssential AbcUi SharedGameData
        COMMAND "${CMAKE_COMMAND}" -E make_directory
            "${ABC_DEPLOY_GAME_DIR}/svencoop/metahook/dlls/AbcCore"
            "${ABC_DEPLOY_GAME_DIR}/svencoop/metahook/plugins"
            "${ABC_DEPLOY_GAME_DIR}/svencoop/metahook/gamedata/abc"
            "${ABC_DEPLOY_GAME_DIR}/svencoop/abc"
        COMMAND "${CMAKE_COMMAND}" -E copy
            "${ABC_MODULE_BUILD_DIR}/AbcCore/${ABC_MODULE_CONFIG}/AbcCore.dll"
            "${ABC_DEPLOY_GAME_DIR}/svencoop/metahook/dlls/AbcCore/"
        COMMAND "${CMAKE_COMMAND}" -E copy
            "${ABC_MODULE_BUILD_DIR}/AbcEssential/${ABC_MODULE_CONFIG}/AbcEssential.dll"
            "${ABC_DEPLOY_GAME_DIR}/svencoop/metahook/plugins/"
        COMMAND "${CMAKE_COMMAND}" -E copy
            "${ABC_MODULE_BUILD_DIR}/AbcUi/${ABC_MODULE_CONFIG}/AbcUi.dll"
            "${ABC_DEPLOY_GAME_DIR}/svencoop/metahook/plugins/"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${ABC_UI_SOURCE_DIR}/assets/abc/abcui"
            "${ABC_DEPLOY_GAME_DIR}/svencoop/abc/abcui"
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${ABC_SHARED_GAMEDATA_DIR}"
            "${ABC_DEPLOY_GAME_DIR}/svencoop/metahook/gamedata/abc"
        COMMENT "Deploying AbcCore.dll, AbcEssential.dll, AbcUi.dll and assets to ${ABC_GAME_DIR}"
        VERBATIM)
endif()
