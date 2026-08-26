# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# The bootloader always uses the "bootloader_components" folder name
# as the project-local search path for bootloader components added to the bootloader build.
#
# Use BOOTLOADER_EXTRA_COMPONENT_DIRS when bootloader components already lives somewhere else,
# for example in a shared components directory or in a component reused by several projects.
# The path may point to a directory containing multiple components.
idf_build_set_property(BOOTLOADER_EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/extra_bootloader_components" APPEND)

# The path may also point directly to a single component when adding a wrapper directory
# just for the bootloader build would be unnecessary.
idf_build_set_property(BOOTLOADER_EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/extra_component4" APPEND)
idf_build_set_property(BOOTLOADER_EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/extra_component5" APPEND)

# A shared extra directory may contain components that are not needed in this bootloader build.
# Ignore them by component name, including direct single-component paths.
set(BOOTLOADER_IGNORE_EXTRA_COMPONENT extra_component3 extra_component5)

# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)

project(main)

# Check which extra bootloader components are included and which ones are ignored.
add_custom_target(check_bootloader_extra_components ALL
    COMMAND ${CMAKE_COMMAND}
        -DPROJECT_DESCRIPTION=${CMAKE_BINARY_DIR}/bootloader/project_description.json
        -DEXPECTED_COMPONENTS=extra_component1,extra_component2,extra_component4
        -DIGNORED_COMPONENTS=extra_component3,extra_component5
        -P ${CMAKE_CURRENT_LIST_DIR}/check_bootloader_component.cmake
    DEPENDS bootloader
    VERBATIM)
