foreach(required_var
        ABC_CORE_SOURCE_DIR
        ABC_CORE_BUILD_DIR
        ABC_CORE_CONFIG
        ABC_CORE_OUTPUT_DIR)
    if(NOT DEFINED ${required_var} OR "${${required_var}}" STREQUAL "")
        message(FATAL_ERROR "PackageAbcCore.cmake: missing ${required_var}")
    endif()
endforeach()

file(STRINGS "${ABC_CORE_SOURCE_DIR}/src/Core.cpp" ABC_CORE_VERSION_LINE
    REGEX "abc_core [0-9]+\\.[0-9]+\\.[0-9]+")
string(REGEX MATCH "abc_core ([0-9]+\\.[0-9]+\\.[0-9]+)" _ "${ABC_CORE_VERSION_LINE}")
set(ABC_CORE_VERSION "${CMAKE_MATCH_1}")
if(NOT ABC_CORE_VERSION)
    set(ABC_CORE_VERSION "0.0.0")
endif()

set(ABC_CORE_DLL "${ABC_CORE_BUILD_DIR}/${ABC_CORE_CONFIG}/AbcCore.dll")
if(NOT EXISTS "${ABC_CORE_DLL}")
    set(ABC_CORE_DLL "${ABC_CORE_BUILD_DIR}/AbcCore.dll")
endif()
if(NOT EXISTS "${ABC_CORE_DLL}")
    message(FATAL_ERROR "PackageAbcCore.cmake: AbcCore.dll not found under ${ABC_CORE_BUILD_DIR}")
endif()

file(MAKE_DIRECTORY "${ABC_CORE_OUTPUT_DIR}")
set(ABC_CORE_STAGE "${ABC_CORE_OUTPUT_DIR}/.stage/AbcCore-${ABC_CORE_VERSION}")
file(REMOVE_RECURSE "${ABC_CORE_STAGE}")
file(MAKE_DIRECTORY "${ABC_CORE_STAGE}/svencoop/metahook/dlls/AbcCore")
file(COPY "${ABC_CORE_DLL}" DESTINATION "${ABC_CORE_STAGE}/svencoop/metahook/dlls/AbcCore")
file(COPY "${ABC_CORE_SOURCE_DIR}/README.md" "${ABC_CORE_SOURCE_DIR}/LICENSE"
    DESTINATION "${ABC_CORE_STAGE}")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E chdir "${ABC_CORE_STAGE}"
            "${CMAKE_COMMAND}" -E tar cf
            "${ABC_CORE_OUTPUT_DIR}/AbcCore-${ABC_CORE_VERSION}.zip"
            --format=zip
            -- svencoop README.md LICENSE
    RESULT_VARIABLE ABC_CORE_ARCHIVE_RESULT)
if(NOT ABC_CORE_ARCHIVE_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to create AbcCore-${ABC_CORE_VERSION}.zip")
endif()
file(REMOVE_RECURSE "${ABC_CORE_STAGE}")

message(STATUS "Packaged AbcCore-${ABC_CORE_VERSION}.zip")
