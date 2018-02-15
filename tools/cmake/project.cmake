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

set_default(PYTHON "python")

# project
#
# This macro wraps the cmake 'project' command to add
# all of the IDF-specific functionality required
#
# Implementation Note: This macro wraps 'project' on purpose, because cmake has some
# backwards-compatible magic where if you don't call "project" in the top-level
# CMakeLists file, it will call it implicitly. However, the implict project will not
# have CMAKE_TOOLCHAIN_FILE set and therefore tries to create a native build project.
#
# Therefore, to keep all the IDF "build magic", the cleanest way is to keep the
# top-level "project" call but customize it to do what we want in the IDF build...
#
macro(project name)
  # Set global variables used by rest of the build
  idf_set_global_variables()

  # Search COMPONENT_DIRS for COMPONENTS, make a list of full paths to each component in COMPONENT_PATHS
  components_find_all("${COMPONENT_DIRS}" "${COMPONENTS}" COMPONENT_PATHS COMPONENTS)

  kconfig_set_variables()

  kconfig_process_config()

  # Include sdkconfig.cmake so rest of the build knows the configuration
  include(${SDKCONFIG_CMAKE})

  # Now the configuration is loaded, set the toolchain appropriately
  #
  # TODO: support more toolchains than just ESP32
  set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-esp32.cmake)

  # Declare the actual cmake-level project
  _project(${name} ASM C CXX)

  # Verify the environment is configured correctly
  idf_verify_environment()

  # Add some idf-wide definitions
  idf_set_global_compiler_options()

  # Check git version (may trigger reruns of cmake)
  # & set GIT_REVISION/IDF_VER variable
  git_describe(GIT_REVISION)
  add_definitions(-DIDF_VER=\"${GIT_REVISION}\")
  git_submodule_check("${IDF_PATH}")

  # Include any top-level project_include.cmake files from components
  foreach(component ${COMPONENT_PATHS})
    include_if_exists("${component}/project_include.cmake")
  endforeach()

  #
  # Add each component to the build as a library
  #
  foreach(component ${COMPONENT_PATHS})
    get_filename_component(component_name ${component} NAME)
    add_subdirectory(${component} ${component_name})
  endforeach()

  #
  # Add the app executable to the build (has name of PROJECT.elf)
  #
  idf_add_executable()

  # Write project description JSON file
  configure_file("${IDF_PATH}/tools/cmake/project_description.json.in"
    "${CMAKE_BINARY_DIR}/project_description.json")

  #
  # Finish component registration (add cross-dependencies, make
  # executable dependent on all components)
  #
  components_finish_registration()

endmacro(project)
