foreach(required_var ABC_MODULE_BUILD_DIR ABC_OUTPUT_DIR)
    if(NOT DEFINED ${required_var} OR "${${required_var}}" STREQUAL "")
        message(FATAL_ERROR "CollectPackages.cmake: missing ${required_var}")
    endif()
endforeach()

file(MAKE_DIRECTORY "${ABC_OUTPUT_DIR}")
file(GLOB ABC_PACKAGES
    "${ABC_MODULE_BUILD_DIR}/AbcEssential/dist/*.zip"
    "${ABC_MODULE_BUILD_DIR}/AbcUi/dist/*.zip")
if(NOT ABC_PACKAGES)
    message(FATAL_ERROR "CollectPackages.cmake: no plugin packages were produced")
endif()
file(COPY ${ABC_PACKAGES} DESTINATION "${ABC_OUTPUT_DIR}")

foreach(ABC_PACKAGE IN LISTS ABC_PACKAGES)
    get_filename_component(ABC_PACKAGE_NAME "${ABC_PACKAGE}" NAME)
    message(STATUS "Collected ${ABC_PACKAGE_NAME}")
endforeach()
