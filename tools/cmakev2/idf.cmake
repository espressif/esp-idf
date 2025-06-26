# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

cmake_minimum_required(VERSION 3.22)

# Update CMAKE_MODULE_PATH to ensure that other build system modules can be
# included.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}" ${CMAKE_MODULE_PATH})

include(component)
include(build)

#[[
   __init_idf_path()

   Determine the IDF_PATH value, either from the IDF_PATH environmental
   variable or based on the location of this file. Also check there is no
   inconsistency between the two.

   Set the IDF_PATH global variable, environment variable and build property.
#]]
function(__init_idf_path)
    get_filename_component(idf_path_infer "${CMAKE_CURRENT_LIST_DIR}/../.." REALPATH)

    if(NOT DEFINED ENV{IDF_PATH})
        idf_warn("IDF_PATH environment variable not found. "
                 "Setting IDF_PATH to '${idf_path_infer}'.")
        set(idf_path "${idf_path_infer}")
    else()
        get_filename_component(idf_path_env "$ENV{IDF_PATH}" REALPATH)
        if(NOT "${idf_path_env}" STREQUAL "${idf_path_infer}")
            idf_warn("IDF_PATH environment variable is different from inferred IDF_PATH. "
                     "Check if your project's top-level CMakeLists.txt includes the right "
                     "CMake files. Environment IDF_PATH will be used for the build: "
                     "'${idf_path_env}'")
        endif()
        set(idf_path "${idf_path_env}")
    endif()

    idf_build_set_property(IDF_PATH "${idf_path}")
    set(IDF_PATH ${idf_path} PARENT_SCOPE)
    set(ENV{IDF_PATH} ${idf_path})
endfunction()

#[[
   __init_components()

   Search for possible component directories categorized by their source, which
   could be ``idf_components``, ``project_extra_components``, or
   ``project_components``. Components added by the component manager are
   initialized later as ``project_managed_components`` after the component
   manager is called.

   The search respects the variables set by the user e.g. in the project's
   CMakeLists.txt file. These are maintained for backward compatibility.

   :COMPONENT_DIRS: If set, component directories are searched exclusively in
                    the paths provided in ``COMPONENT_DIRS``.
   :EXTRA_COMPONENT_DIRS: Includes extra paths to search if `COMPONENT_DIRS` is not specified.
   :EXTRA_COMPONENT_EXCLUDE_DIRS: List of paths to exclude from searching the
                                  component directories.

   Each component is initialized for every component directory found.
#]]
function(__init_components)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(prefix PREFIX)

    __get_component_paths(PATHS "${idf_path}/components"
                          OUTPUT idf_components)
    if(COMPONENT_DIRS)
        # The user explicitly stated the locations to search for components.
        # For backward compatibility, check that the paths in
        # COMPONENT_DIRS exist.
        __get_component_paths(PATHS ${COMPONENT_DIRS}
                              EXCLUDE_PATHS ${EXTRA_COMPONENT_EXCLUDE_DIRS}
                              SOURCE "COMPONENT_DIRS"
                              CHECK
                              OUTPUT project_components)
    else()
        __get_component_paths(PATHS "${CMAKE_CURRENT_SOURCE_DIR}/main"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/components"
                              EXCLUDE_PATHS ${EXTRA_COMPONENT_EXCLUDE_DIRS}
                              OUTPUT project_components)
        if(EXTRA_COMPONENT_DIRS)
            # For backward compatibility, check that the paths in
            # EXTRA_COMPONENT_DIRS exist.
            __get_component_paths(PATHS ${EXTRA_COMPONENT_DIRS}
                                  EXCLUDE_PATHS ${EXTRA_COMPONENT_EXCLUDE_DIRS}
                                  SOURCE "EXTRA_COMPONENT_DIRS"
                                  CHECK
                                  OUTPUT project_extra_components)
        endif()
    endif()

    foreach(path IN LISTS idf_components)
        __init_component(DIRECTORY "${path}"
                         PREFIX "${prefix}"
                         SOURCE "idf_components")
    endforeach()

    foreach(path IN LISTS project_components)
        __init_component(DIRECTORY "${path}"
                         PREFIX "${prefix}"
                         SOURCE "project_components")
    endforeach()

    foreach(path IN LISTS project_extra_components)
        __init_component(DIRECTORY "${path}"
                         PREFIX "${prefix}"
                         SOURCE "project_extra_components")
    endforeach()
endfunction()

#[[
   The idf_build_properties interface target is exclusively used to store
   information about global build properties and is not linked or used in any
   other way. This is created very early so that all the initialization
   functions can use it.

   List of build properties

   :IDF_PATH: Path to esp-idf directory.
   :PREFIX: Prefix used for component target names.
   :COMPONENTS_DISCOVERED: List of component names identified by the build
                           system. These components are initialized and can
                           have properties attached to them. However, they
                           are not necessarily included in the build through
                           add_subdirectory.
   :COMPONENT_INTERFACES: This is a list of component interface targets for
                          the components in ``COMPONENTS_DISCOVERED``. It is
                          used when searching for a component, such as by its
                          name, to set or retrieve the component's properties.

#]]
add_library(idf_build_properties INTERFACE)

# Set build system prefix for component targets.
idf_build_set_property(PREFIX "idf")

# Initialize IDF_PATH and set it as a global and environmental variable, as
# well as a build property.
__init_idf_path()

# Discover and initialize components.
__init_components()

#[[ TODO

Many of the following things are already implemented in PoC !38337, but they
need to be reviewed.

* Set IDF version.
* Set and check python interpreter.
* Set build target.
* Set the toolchain before invoking project().
* Enable ccache if requested and available.
* Generate initial sdkconfig for component manager.
* Call component manager with initial sdkconfig to download requested components
  and include the generated cmake.  See the generated
  ``build/managed_components_list.temp.cmake``. We probably need shim for
  ``idf_build_component`` calling ``__init_component``.
* Generate final sdkconfig and include its cmake version.

There may be additional steps. We should initialize everything necessary before
calling the ``project()`` function, as well as any global settings that cannot be
modified later.

#]]
