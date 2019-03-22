#
# Load cmake modules
#

get_property(__idf_environment_set GLOBAL PROPERTY __IDF_ENVIRONMENT_SET)

if(NOT __idf_environment_set)

    # Set IDF_PATH, as nothing else will work without this.
    set(IDF_PATH "$ENV{IDF_PATH}")
    if(NOT IDF_PATH)
        # Documentation says you should set IDF_PATH in your environment, but we
        # can infer it relative to tools/cmake directory if it's not set.
        get_filename_component(IDF_PATH "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    endif()
    file(TO_CMAKE_PATH "${IDF_PATH}" IDF_PATH)
    set(ENV{IDF_PATH} ${IDF_PATH})

    set(CMAKE_MODULE_PATH
        "${IDF_PATH}/tools/cmake"
        "${IDF_PATH}/tools/cmake/third_party"
        ${CMAKE_MODULE_PATH})
    include(utilities)
    include(components)
    include(kconfig)
    include(targets)
    include(git_submodules)
    include(GetGitRevisionDescription)
    include(crosstool_version_check)
    include(ldgen)
    include(version)

    set_default(PYTHON "python")

    if(NOT PYTHON_DEPS_CHECKED AND NOT BOOTLOADER_BUILD)
        message(STATUS "Checking Python dependencies...")
        execute_process(COMMAND "${PYTHON}" "${IDF_PATH}/tools/check_python_dependencies.py"
            RESULT_VARIABLE result)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR "Some Python dependencies must be installed. Check above message for details.")
        endif()
    endif()

    idf_set_target()

    set_property(GLOBAL APPEND PROPERTY __IDF_COMPONENTS_PREFIX "idf_component")
    set_property(GLOBAL PROPERTY __IDF_ENVIRONMENT_SET 1)
endif()

macro(idf_set_variables)
    set_default(IDF_BUILD_ARTIFACTS OFF)

    if(IDF_BUILD_ARTIFACTS)
        if(NOT IDF_BUILD_ARTIFACTS_DIR OR NOT IDF_PROJECT_EXECUTABLE)
            message(FATAL_ERROR "IDF_BUILD_ARTIFACTS and IDF_PROJECT_EXECUTABLE needs to be specified \
                    if IDF_BUILD_ARTIFACTS is ON.")
        endif()
    endif()

    set_default(IDF_COMPONENT_DIRS "${IDF_EXTRA_COMPONENT_DIRS} ${IDF_PATH}/components")
    set_default(IDF_COMPONENTS "")
    set_default(IDF_COMPONENT_REQUIRES_COMMON "cxx ${IDF_TARGET} newlib freertos heap log soc")

    set(IDF_PROJECT_PATH "${CMAKE_SOURCE_DIR}")

    set(ESP_PLATFORM 1 CACHE BOOL INTERNAL)

    spaces2list(IDF_COMPONENT_DIRS)
    spaces2list(IDF_COMPONENTS)
    spaces2list(IDF_COMPONENT_REQUIRES_COMMON)
endmacro()

# Add all the IDF global compiler & preprocessor options
# (applied to all components). Some are config-dependent
#
# If you only want to set options for a particular component,
# don't call or edit this function. TODO DESCRIBE WHAT TO DO INSTEAD
#
function(idf_set_global_compile_options)
    # Temporary trick to support both gcc5 and gcc8 builds
    if(CMAKE_C_COMPILER_VERSION VERSION_EQUAL 5.2.0)
        set(GCC_NOT_5_2_0 0 CACHE STRING "GCC is 5.2.0 version")
    else()
        set(GCC_NOT_5_2_0 1 CACHE STRING "GCC is not 5.2.0 version")
    endif()
    list(APPEND compile_definitions "GCC_NOT_5_2_0=${GCC_NOT_5_2_0}")

    list(APPEND compile_definitions "ESP_PLATFORM" "HAVE_CONFIG_H")

    list(APPEND compile_options "${CMAKE_C_FLAGS}")
    list(APPEND c_compile_options "${CMAKE_C_FLAGS}")
    list(APPEND cxx_compile_options "${CMAKE_CXX_FLAGS}")

    if(CONFIG_OPTIMIZATION_LEVEL_RELEASE)
        list(APPEND compile_options "-Os")
    else()
        list(APPEND compile_options "-Og")
    endif()

    list(APPEND c_compile_options "-std=gnu99")
    list(APPEND cxx_compile_options "-std=gnu++11" "-fno-rtti")

    if(CONFIG_CXX_EXCEPTIONS)
        list(APPEND cxx_compile_options "-fexceptions")
    else()
        list(APPEND cxx_compile_options "-fno-exceptions")
    endif()

    # Default compiler configuration
    list(APPEND compile_options "-ffunction-sections"
                                "-fdata-sections"
                                "-fstrict-volatile-bitfields"
                                "-nostdlib")

    list(APPEND compile_options "-Wall"
                                "-Werror=all"
                                "-Wno-error=unused-function"
                                "-Wno-error=unused-but-set-variable"
                                "-Wno-error=unused-variable"
                                "-Wno-error=deprecated-declarations"
                                "-Wextra"
                                "-Wno-unused-parameter"
                                "-Wno-sign-compare")

    list(APPEND c_compile_options "-Wno-old-style-declaration")

    if(CONFIG_DISABLE_GCC8_WARNINGS)
        list(APPEND compile_options
            "-Wno-parentheses"
            "-Wno-sizeof-pointer-memaccess"
            "-Wno-clobbered"
        )

        # doesn't use GCC_NOT_5_2_0 because idf_set_global_variables was not called before
        if(NOT CMAKE_C_COMPILER_VERSION VERSION_EQUAL 5.2.0)
            list(APPEND compile_options
                "-Wno-format-overflow"
                "-Wno-stringop-truncation"
                "-Wno-misleading-indentation"
                "-Wno-cast-function-type"
                "-Wno-implicit-fallthrough"
                "-Wno-unused-const-variable"
                "-Wno-switch-unreachable"
                "-Wno-format-truncation"
                "-Wno-memset-elt-size"
                "-Wno-int-in-bool-context"
            )
        endif()
    endif()

    # Stack protection
    if(NOT BOOTLOADER_BUILD)
        if(CONFIG_STACK_CHECK_NORM)
            list(APPEND compile_options "-fstack-protector")
        elseif(CONFIG_STACK_CHECK_STRONG)
            list(APPEND compile_options "-fstack-protector-strong")
        elseif(CONFIG_STACK_CHECK_ALL)
            list(APPEND compile_options "-fstack-protector-all")
        endif()
    endif()

    if(CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED)
        list(APPEND compile_definitions "NDEBUG")
    endif()

    # Always generate debug symbols (even in Release mode, these don't
    # go into the final binary so have no impact on size)
    list(APPEND compile_options "-ggdb")

    # Use EXTRA_CFLAGS, EXTRA_CXXFLAGS and EXTRA_CPPFLAGS to add more priority options to the compiler
    # EXTRA_CPPFLAGS is used for both C and C++
    # Unlike environments' CFLAGS/CXXFLAGS/CPPFLAGS which work for both host and target build,
    # these works only for target build
    set(EXTRA_CFLAGS "$ENV{EXTRA_CFLAGS}")
    set(EXTRA_CXXFLAGS "$ENV{EXTRA_CXXFLAGS}")
    set(EXTRA_CPPFLAGS "$ENV{EXTRA_CPPFLAGS}")
    spaces2list(EXTRA_CFLAGS)
    spaces2list(EXTRA_CXXFLAGS)
    spaces2list(EXTRA_CPPFLAGS)
    list(APPEND c_compile_options ${EXTRA_CFLAGS})
    list(APPEND cxx_compile_options ${EXTRA_CXXFLAGS})
    list(APPEND compile_options ${EXTRA_CPPFLAGS})

    set_default(IDF_COMPILE_DEFINITIONS "${compile_definitions}")
    set_default(IDF_COMPILE_OPTIONS "${compile_options}")
    set_default(IDF_C_COMPILE_OPTIONS "${c_compile_options}")
    set_default(IDF_CXX_COMPILE_OPTIONS "${cxx_compile_options}")
    set_default(IDF_INCLUDE_DIRECTORIES "${CONFIG_DIR}")

    set(IDF_COMPILE_DEFINITIONS ${IDF_COMPILE_DEFINITIONS} PARENT_SCOPE)
    set(IDF_COMPILE_OPTIONS ${IDF_COMPILE_OPTIONS} PARENT_SCOPE)
    set(IDF_C_COMPILE_OPTIONS ${IDF_C_COMPILE_OPTIONS} PARENT_SCOPE)
    set(IDF_CXX_COMPILE_OPTIONS ${IDF_CXX_COMPILE_OPTIONS} PARENT_SCOPE)
    set(IDF_INCLUDE_DIRECTORIES ${CONFIG_DIR} PARENT_SCOPE)
endfunction()

# Verify the IDF environment is configured correctly (environment, toolchain, etc)
function(idf_verify_environment)
    if(NOT CMAKE_PROJECT_NAME)
        message(FATAL_ERROR "Internal error, IDF project.cmake should have set this variable already")
    endif()

    # Check toolchain is configured properly in cmake
    if(NOT ( ${CMAKE_SYSTEM_NAME} STREQUAL "Generic" AND ${CMAKE_C_COMPILER} MATCHES xtensa))
        message(FATAL_ERROR "Internal error, toolchain has not been set correctly by project "
            "(or an invalid CMakeCache.txt file has been generated somehow)")
    endif()

    #
    # Warn if the toolchain version doesn't match
    #
    # TODO: make these platform-specific for diff toolchains
    get_expected_ctng_version(expected_toolchain expected_gcc)
    gcc_version_check("${expected_gcc}")
    crosstool_version_check("${expected_toolchain}")
endfunction()

# idf_get_git_revision
#
# Set global IDF_VER to the git revision of ESP-IDF.
#
# Running git_describe() here automatically triggers rebuilds
# if the ESP-IDF git version changes
function(idf_get_git_revision)
    git_describe(IDF_VER_GIT "${IDF_PATH}")
    if(EXISTS "${IDF_PATH}/version.txt")
        file(STRINGS "${IDF_PATH}/version.txt" IDF_VER_T)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${IDF_PATH}/version.txt")
    else()
        set(IDF_VER_T ${IDF_VER_GIT})
    endif()
    # cut IDF_VER to required 32 characters.
    string(SUBSTRING "${IDF_VER_T}" 0 31 IDF_VER)
    message(STATUS "IDF_VER: ${IDF_VER}")
    add_definitions(-DIDF_VER=\"${IDF_VER}\")
    git_submodule_check("${IDF_PATH}")
    set(IDF_VER ${IDF_VER} PARENT_SCOPE)
endfunction()

# app_get_revision
#
# Set global PROJECT_VER
#
# If PROJECT_VER variable set in project CMakeLists.txt file, its value will be used.
# Else, if the _project_path/version.txt exists, its contents will be used as PROJECT_VER.
# Else, if the project is located inside a Git repository, the output of git describe will be used.
# Otherwise, PROJECT_VER will be "1".
function(app_get_revision _project_path)
    if(NOT DEFINED PROJECT_VER)
        if(EXISTS "${_project_path}/version.txt")
            file(STRINGS "${_project_path}/version.txt" PROJECT_VER)
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_project_path}/version.txt")
        else()
            git_describe(PROJECT_VER_GIT "${_project_path}")
            if(PROJECT_VER_GIT)
                set(PROJECT_VER ${PROJECT_VER_GIT})
            else()
                message(STATUS "Project is not inside a git repository, \
                        will not use 'git describe' to determine PROJECT_VER.")
                set(PROJECT_VER "1")
            endif()
        endif()
    endif()
    message(STATUS "Project version: ${PROJECT_VER}")
    set(PROJECT_VER ${PROJECT_VER} PARENT_SCOPE)
endfunction()

# idf_link_components
#
# Link library components to the target
function(idf_link_components target components)
    foreach(component ${components})
        component_get_target(component_target ${component})

        # Add each component library's link-time dependencies (which are otherwise ignored) to the executable
        # LINK_DEPENDS in order to trigger a re-link when needed (on Ninja/Makefile generators at least).
        # (maybe this should probably be something CMake does, but it doesn't do it...)
        if(TARGET ${component_target})
            get_target_property(type ${component_target} TYPE)
            get_target_property(imported ${component_target} IMPORTED)
            if(NOT imported)
                if(${type} STREQUAL STATIC_LIBRARY OR ${type} STREQUAL EXECUTABLE)
                    get_target_property(link_depends "${component_target}" LINK_DEPENDS)
                    if(link_depends)
                        set_property(TARGET ${target} APPEND PROPERTY LINK_DEPENDS "${link_depends}")
                    endif()
                endif()
            endif()

            if(${type} MATCHES .+_LIBRARY)
                list(APPEND libraries ${component_target})
            endif()
        endif()
    endforeach()

    if(libraries)
        # gc-sections is necessary for linking some IDF binary libraries
        # (and without it, IDF apps are much larger than they should be)
        target_link_libraries(${target} "-Wl,--gc-sections")
        target_link_libraries(${target} "-Wl,--start-group")
        target_link_libraries(${target} ${libraries})
        message(STATUS "Component libraries: ${IDF_COMPONENT_LIBRARIES}")
    endif()
endfunction()

# idf_import_components
#
# Adds ESP-IDF as a subdirectory to the current project and imports the components
macro(idf_import_components var idf_path build_path)
    #
    # Set variables that control the build configuration and the build itself
    #
    idf_set_variables()

    kconfig_set_variables()

    #
    # Generate a component dependencies file, enumerating components to be included in the build
    # as well as their dependencies.
    #
    execute_process(COMMAND "${CMAKE_COMMAND}"
        -D "COMPONENTS=${IDF_COMPONENTS}"
        -D "COMPONENT_REQUIRES_COMMON=${IDF_COMPONENT_REQUIRES_COMMON}"
        -D "EXCLUDE_COMPONENTS=${IDF_EXCLUDE_COMPONENTS}"
        -D "TEST_COMPONENTS=${IDF_TEST_COMPONENTS}"
        -D "TEST_EXCLUDE_COMPONENTS=${IDF_TEST_EXCLUDE_COMPONENTS}"
        -D "BUILD_TESTS=${IDF_BUILD_TESTS}"
        -D "DEPENDENCIES_FILE=${CMAKE_BINARY_DIR}/component_depends.cmake"
        -D "COMPONENT_DIRS=${IDF_COMPONENT_DIRS}"
        -D "BOOTLOADER_BUILD=${BOOTLOADER_BUILD}"
        -D "IDF_TARGET=${IDF_TARGET}"
        -D "IDF_PATH=${IDF_PATH}"
        -D "DEBUG=${DEBUG}"
        -P "${IDF_PATH}/tools/cmake/scripts/expand_requirements.cmake"
        WORKING_DIRECTORY "${PROJECT_PATH}"
        RESULT_VARIABLE expand_requirements_result)

    if(expand_requirements_result)
        message(FATAL_ERROR "Failed to expand component requirements")
    endif()

    include("${CMAKE_BINARY_DIR}/component_depends.cmake")

    #
    # We now have the following component-related variables:
    #
    # IDF_COMPONENTS is the list of initial components set by the user
    # (or empty to include all components in the build).
    # BUILD_COMPONENTS is the list of components to include in the build.
    # BUILD_COMPONENT_PATHS is the paths to all of these components, obtained from the component dependencies file.
    #
    # Print the list of found components and test components
    #
    string(REPLACE ";" " " BUILD_COMPONENTS_SPACES "${BUILD_COMPONENTS}")
    message(STATUS "Component names: ${BUILD_COMPONENTS_SPACES}")
    unset(BUILD_COMPONENTS_SPACES)
    message(STATUS "Component paths: ${BUILD_COMPONENT_PATHS}")

    # Print list of test components
    if(TESTS_ALL EQUAL 1 OR TEST_COMPONENTS)
        string(REPLACE ";" " " BUILD_TEST_COMPONENTS_SPACES "${BUILD_TEST_COMPONENTS}")
        message(STATUS "Test component names: ${BUILD_TEST_COMPONENTS_SPACES}")
        unset(BUILD_TEST_COMPONENTS_SPACES)
        message(STATUS "Test component paths: ${BUILD_TEST_COMPONENT_PATHS}")
    endif()

    # Generate project configuration
    kconfig_process_config()

    # Include sdkconfig.cmake so rest of the build knows the configuration
    include(${SDKCONFIG_CMAKE})

    # Verify the environment is configured correctly
    idf_verify_environment()

    # Check git revision (may trigger reruns of cmake)
    ##  sets IDF_VER to IDF git revision
    idf_get_git_revision()

    # Check that the targets set in cache, sdkconfig, and in environment all match
    idf_check_config_target()

    ## get PROJECT_VER
    if(NOT BOOTLOADER_BUILD)
        app_get_revision("${CMAKE_SOURCE_DIR}")
    endif()

    # Add some idf-wide definitions
    idf_set_global_compile_options()

    # generate compile_commands.json (needs to come after project)
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

    #
    # Setup variables for linker script generation
    #
    ldgen_set_variables()

    # Include any top-level project_include.cmake files from components
    foreach(component ${BUILD_COMPONENT_PATHS})
        set(COMPONENT_PATH "${component}")
        include_if_exists("${component}/project_include.cmake")
        unset(COMPONENT_PATH)
    endforeach()

    add_subdirectory(${idf_path} ${build_path})

    if(IDF_BUILD_ARTIFACTS)
        # Write project description JSON file
        make_json_list("${BUILD_COMPONENTS}" build_components_json)
        make_json_list("${BUILD_COMPONENT_PATHS}" build_component_paths_json)
        configure_file("${IDF_PATH}/tools/cmake/project_description.json.in"
            "${IDF_BUILD_ARTIFACTS_DIR}/project_description.json")
        unset(build_components_json)
        unset(build_component_paths_json)
    endif()

    ldgen_add_dependencies()

    set(${var} ${BUILD_COMPONENTS})
endmacro()
