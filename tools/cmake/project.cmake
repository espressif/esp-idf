# Designed to be included from an IDF app's CMakeLists.txt file
#
cmake_minimum_required(VERSION 3.5)

# Set IDF_PATH, as nothing else will work without this
set(IDF_PATH "$ENV{IDF_PATH}")
if(NOT IDF_PATH)
  # Documentation says you should set IDF_PATH in your environment, but we
  # can infer it here if it's not set.
  set(IDF_PATH ${CMAKE_CURRENT_LIST_DIR})
  set($ENV{IDF_PATH} "${IDF_PATH}")
endif()

#
# Load cmake modules
#
set(CMAKE_MODULE_PATH "${IDF_PATH}/tools/cmake" "${IDF_PATH}/tools/cmake/third_party" ${CMAKE_MODULE_PATH})
include(GetGitRevisionDescription)
include(utilities)
include(components)
include(kconfig)
include(git_submodules)
include(idf_functions)

# Verify the environment is configured correctly
idf_verify_environment()

# Set global variables used by rest of the build
idf_set_global_variables()

# Search COMPONENT_DIRS for COMPONENTS, make a list of full paths to each component in COMPONENT_PATHS
components_find_all("${COMPONENT_DIRS}" "${COMPONENTS}" COMPONENT_PATHS COMPONENTS)

kconfig_set_variables()

kconfig_process_config()

# Include sdkconfig.cmake so rest of the build knows the configuration
include(${SDKCONFIG_CMAKE})

set(PYTHON "${CONFIG_PYTHON}")

# Add some idf-wide definitions
idf_set_global_compiler_options()

# Check git version (may trigger reruns of cmake)
# & set GIT_REVISION/IDF_VER variable
git_describe(GIT_REVISION)
add_definitions(-DIDF_VER=\"${GIT_REVISION}\")
git_submodule_check("${IDF_PATH}")


#
# Add components to the build
#
foreach(component ${COMPONENT_PATHS})
  get_filename_component(component_name ${component} NAME)
  add_subdirectory(${component} ${component_name})
endforeach()

components_finish_registration()

# Load the targets for the bootloader subproject
if(NOT BOOTLOADER_BUILD)
  include(bootloader_subproject)
endif()
