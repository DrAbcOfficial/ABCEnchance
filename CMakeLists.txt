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

# AbcUi and AbcEssential are standalone CMake projects, and each one vendors its
# own pinned AbcCore checkout under thirdparty/AbcCore. Adding all three with
# add_subdirectory() would define the abc_core target more than once. Build every
# module in an isolated binary directory instead, then expose one aggregate
# target for the whole family.
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
    "-DABC_CORE_DIR:PATH=${CMAKE_CURRENT_SOURCE_DIR}/AbcCore")
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

function(abc_add_module module)
    set(module_source "${CMAKE_CURRENT_SOURCE_DIR}/${module}")
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

abc_add_module(AbcCore)
abc_add_module(AbcEssential)
abc_add_module(AbcUi)

set(ABC_SHARED_GAMEDATA_DIR "${CMAKE_BINARY_DIR}/assets/svencoop/metahook/gamedata/abc")
add_custom_target(SharedGameData
    COMMAND "${Python3_EXECUTABLE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/build-shared-gamedata.py"
        --core-dir "${CMAKE_CURRENT_SOURCE_DIR}/AbcCore"
        --manifest "${CMAKE_CURRENT_SOURCE_DIR}/scripts/manifests/abc.json"
        --layout-dir "${CMAKE_CURRENT_SOURCE_DIR}/AbcUi/assets/gamedata/abcui-layout"
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
        "-DABC_CORE_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/AbcCore"
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
