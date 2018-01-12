# Designed to be included from an IDF app's CMakeLists.txt file
#
cmake_minimum_required(VERSION 3.5)

#
# Verify the project configured the environment correctly
#
if(NOT ( ${CMAKE_SYSTEM_NAME} STREQUAL "Generic" AND ${CMAKE_C_COMPILER} MATCHES xtensa))
  message(FATAL_ERROR "The parent project CMakeLists.txt file needs to set CMAKE_TOOLCHAIN_FILE "
    "before including this file. "
    "Update CMakeLists.txt to match the template project and delete CMakeCache.txt before "
    "re-running cmake.")
endif()

set(IDF_PATH $ENV{IDF_PATH})
if(NOT IDF_PATH)
  # Documentation says you should set IDF_PATH in your environment, but we
  # can infer it here if it's not set.
  set(IDF_PATH ${CMAKE_CURRENT_LIST_DIR})
endif()

# PROJECT_PATH has the path to the IDF project (top-level cmake directory)
#
# (cmake calls this CMAKE_SOURCE_DIR, keeping old name for compatibility.)
set(PROJECT_PATH "${CMAKE_SOURCE_DIR}")

#
# Load cmake modules
#
set(CMAKE_MODULE_PATH "${IDF_PATH}/tools/cmake" ${CMAKE_MODULE_PATH})
include(GetGitRevisionDescription)
include(utilities)
include(components)
include(kconfig)

#
# Configure optional variables
#
set_default(EXTRA_COMPONENT_DIRS "")

# Note: "main" is no longer a component...
#
set_default(COMPONENT_DIRS "${PROJECT_PATH}/components ${EXTRA_COMPONENT_DIRS} ${IDF_PATH}/components")
spaces2list(COMPONENT_DIRS)

# expand COMPONENT_DIRS variable into full paths to all components and their names
spaces2list(COMPONENTS)
find_all_components("${COMPONENT_DIRS}" "${COMPONENTS}" COMPONENT_PATHS COMPONENTS)
build_component_config()

# Include sdkconfig.cmake so rest of the build knows the configuration
include(${SDKCONFIG_CMAKE})

#
# Add some idf-wide definitions
#
add_definitions(-DESP_PLATFORM)
add_definitions(-DHAVE_CONFIG_H)

git_describe(GIT_REVISION)
add_definitions(-DIDF_VER=\"${GIT_REVISION}\")

add_compile_options("-I${CMAKE_BINARY_DIR}") # for sdkconfig.h

#
# Add components to the build
#
foreach(component ${COMPONENT_PATHS})
  get_filename_component(component_name ${component} NAME)
  add_subdirectory(${component} ${component_name})
endforeach()

finish_component_registration()

# Load the targets for the bootloader subproject
if(NOT BOOTLOADER_BUILD)
  include(bootloader_subproject)
endif()
