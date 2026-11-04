foreach(required_var ABC_MODULE_BUILD_DIR ABC_PLUGIN_MODULES ABC_OUTPUT_DIR)
    if(NOT DEFINED ${required_var} OR "${${required_var}}" STREQUAL "")
        message(FATAL_ERROR "CollectPackages.cmake: missing ${required_var}")
    endif()
endforeach()

file(MAKE_DIRECTORY "${ABC_OUTPUT_DIR}")
set(ABC_PACKAGES)
foreach(ABC_MODULE IN LISTS ABC_PLUGIN_MODULES)
    file(GLOB ABC_MODULE_PACKAGES
        "${ABC_MODULE_BUILD_DIR}/${ABC_MODULE}/dist/*.zip")
    if(NOT ABC_MODULE_PACKAGES)
        message(FATAL_ERROR
            "CollectPackages.cmake: ${ABC_MODULE} produced no package")
    endif()
    list(APPEND ABC_PACKAGES ${ABC_MODULE_PACKAGES})
endforeach()

file(COPY ${ABC_PACKAGES} DESTINATION "${ABC_OUTPUT_DIR}")

foreach(ABC_PACKAGE IN LISTS ABC_PACKAGES)
    get_filename_component(ABC_PACKAGE_NAME "${ABC_PACKAGE}" NAME)
    message(STATUS "Collected ${ABC_PACKAGE_NAME}")
endforeach()
