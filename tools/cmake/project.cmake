# Designed to be included from an IDF app's CMakeLists.txt file
cmake_minimum_required(VERSION 3.5)

# The mere inclusion of this CMake file sets up some interal build properties.
# These properties can be modified in between this inclusion the the idf_build_process
# call.
include(${CMAKE_CURRENT_LIST_DIR}/idf.cmake)

set(IDFTOOL ${PYTHON} "${IDF_PATH}/tools/idf.py")
# Internally, the Python interpreter is already set to 'python'. Re-set here
# to be absolutely sure.
set_default(PYTHON "python")
idf_build_set_property(PYTHON ${PYTHON})

# On processing, checking Python required modules can be turned off if it was
# already checked externally.
if(PYTHON_DEPS_CHECKED)
    idf_build_set_property(__CHECK_PYTHON 0)
endif()

# Initialize build target for this build using the environment variable or
# value passed externally.
__target_init()

#
# Get the project version from either a version file or the Git revision. This is passed
# to the idf_build_process call. Dependencies are also set here for when the version file
# changes (if it is used).
#
function(__project_get_revision var)
    set(_project_path "${CMAKE_CURRENT_LIST_DIR}")
    if(NOT DEFINED PROJECT_VER)
        if(EXISTS "${_project_path}/version.txt")
            file(STRINGS "${_project_path}/version.txt" PROJECT_VER)
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_project_path}/version.txt")
        else()
            git_describe(PROJECT_VER_GIT "${_project_path}")
            if(PROJECT_VER_GIT)
                set(PROJECT_VER ${PROJECT_VER_GIT})
            else()
                message(STATUS "Project is not inside a git repository, or git repository has no commits;"
                        " will not use 'git describe' to determine PROJECT_VER.")
                set(PROJECT_VER 1)
            endif()
        endif()
    endif()
    message(STATUS "Project version: ${PROJECT_VER}")
    set(${var} "${PROJECT_VER}" PARENT_SCOPE)
endfunction()

#
# Output the built components to the user. Generates files for invoking idf_monitor.py
# that doubles as an overview of some of the more important build properties.
#
function(__project_info test_components)
    idf_build_get_property(prefix __PREFIX)
    idf_build_get_property(_build_components BUILD_COMPONENTS)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(idf_path IDF_PATH)

    list(SORT _build_components)

    unset(build_components)
    unset(build_component_paths)

    foreach(build_component ${_build_components})
        __component_get_target(component_target "${build_component}")
        __component_get_property(_name ${component_target} COMPONENT_NAME)
        __component_get_property(_prefix ${component_target} __PREFIX)
        __component_get_property(_alias ${component_target} COMPONENT_ALIAS)
        __component_get_property(_dir ${component_target} COMPONENT_DIR)

        if(_alias IN_LIST test_components)
            list(APPEND test_component_paths ${_dir})
        else()
            if(_prefix STREQUAL prefix)
                set(component ${_name})
            else()
                set(component ${_alias})
            endif()
            list(APPEND build_components ${component})
            list(APPEND build_component_paths ${_dir})
        endif()
    endforeach()

    set(PROJECT_NAME ${CMAKE_PROJECT_NAME})
    idf_build_get_property(PROJECT_PATH PROJECT_DIR)
    idf_build_get_property(BUILD_DIR BUILD_DIR)
    idf_build_get_property(SDKCONFIG SDKCONFIG)
    idf_build_get_property(SDKCONFIG_DEFAULTS SDKCONFIG_DEFAULTS)
    idf_build_get_property(PROJECT_EXECUTABLE EXECUTABLE)
    set(PROJECT_BIN ${CMAKE_PROJECT_NAME}.bin)
    idf_build_get_property(IDF_VER IDF_VER)

    idf_build_get_property(sdkconfig_cmake SDKCONFIG_CMAKE)
    include(${sdkconfig_cmake})
    idf_build_get_property(COMPONENT_KCONFIGS KCONFIGS)
    idf_build_get_property(COMPONENT_KCONFIGS_PROJBUILD KCONFIG_PROJBUILDS)

    # Write project description JSON file
    idf_build_get_property(build_dir BUILD_DIR)
    make_json_list("${build_components};${test_components}" build_components_json)
    make_json_list("${build_component_paths};${test_component_paths}" build_component_paths_json)
    configure_file("${idf_path}/tools/cmake/project_description.json.in"
        "${build_dir}/project_description.json")

    # We now have the following component-related variables:
    #
    # build_components is the list of components to include in the build.
    # build_component_paths is the paths to all of these components, obtained from the component dependencies file.
    #
    # Print the list of found components and test components
    string(REPLACE ";" " " build_components "${build_components}")
    string(REPLACE ";" " " build_component_paths "${build_component_paths}")
    message(STATUS "Components: ${build_components}")
    message(STATUS "Component paths: ${build_component_paths}")

    if(test_components)
        string(REPLACE ";" " " test_components "${test_components}")
        string(REPLACE ";" " " test_component_paths "${test_component_paths}")
        message(STATUS "Test components: ${test_components}")
        message(STATUS "Test component paths: ${test_component_paths}")
    endif()
endfunction()

function(__project_init components_var test_components_var)
    # Use EXTRA_CFLAGS, EXTRA_CXXFLAGS and EXTRA_CPPFLAGS to add more priority options to the compiler
    # EXTRA_CPPFLAGS is used for both C and C++
    # Unlike environments' CFLAGS/CXXFLAGS/CPPFLAGS which work for both host and target build,
    # these works only for target build
    set(extra_cflags "$ENV{EXTRA_CFLAGS}")
    set(extra_cxxflags "$ENV{EXTRA_CXXFLAGS}")
    set(extra_cppflags "$ENV{EXTRA_CPPFLAGS}")

    spaces2list(extra_cflags)
    spaces2list(extra_cxxflags)
    spaces2list(extra_cppflags)

    idf_build_set_property(C_COMPILE_OPTIONS "${extra_cflags}" APPEND)
    idf_build_set_property(CXX_COMPILE_OPTIONS "${extra_cxxflags}" APPEND)
    idf_build_set_property(COMPILE_OPTIONS "${extra_cppflags}" APPEND)

    function(__project_component_dir component_dir)
        get_filename_component(component_dir "${component_dir}" ABSOLUTE)
        if(EXISTS ${component_dir}/CMakeLists.txt)
            idf_build_component(${component_dir})
        else()
            file(GLOB component_dirs ${component_dir}/*)
            foreach(component_dir ${component_dirs})
                if(EXISTS ${component_dir}/CMakeLists.txt)
                    get_filename_component(base_dir ${component_dir} NAME)
                    __component_dir_quick_check(is_component ${component_dir})
                    if(is_component)
                        idf_build_component(${component_dir})
                    endif()
                endif()
            endforeach()
        endif()
    endfunction()

    # Add component directories to the build, given the component filters, exclusions
    # extra directories, etc. passed from the root CMakeLists.txt.
    if(COMPONENT_DIRS)
        # User wants to fully override where components are pulled from.
        spaces2list(COMPONENT_DIRS)
        idf_build_set_property(__COMPONENT_TARGETS "")
        foreach(component_dir ${COMPONENT_DIRS})
            __project_component_dir(${component_dir})
        endforeach()
    else()
        # Look for components in the usual places: CMAKE_CURRENT_LIST_DIR/main,
        # CMAKE_CURRENT_LIST_DIR/components, and the extra component dirs
        if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/main")
            __project_component_dir("${CMAKE_CURRENT_LIST_DIR}/main")
        endif()

        __project_component_dir("${CMAKE_CURRENT_LIST_DIR}/components")

        spaces2list(EXTRA_COMPONENT_DIRS)
        foreach(component_dir ${EXTRA_COMPONENT_DIRS})
            __project_component_dir("${component_dir}")
        endforeach()
    endif()

    spaces2list(COMPONENTS)
    spaces2list(EXCLUDE_COMPONENTS)
    idf_build_get_property(component_targets __COMPONENT_TARGETS)
    foreach(component_target ${component_targets})
        __component_get_property(component_name ${component_target} COMPONENT_NAME)
        set(include 1)
        if(COMPONENTS AND NOT component_name IN_LIST COMPONENTS)
            set(include 0)
        endif()
        if(EXCLUDE_COMPONENTS AND component_name IN_LIST EXCLUDE_COMPONENTS)
            set(include 0)
        endif()
        if(include)
            list(APPEND components ${component_name})
        endif()
    endforeach()

    if(TESTS_ALL OR BUILD_TESTS OR TEST_COMPONENTS OR TEST_EXCLUDE_COMPONENTS)
        spaces2list(TEST_COMPONENTS)
        spaces2list(TEST_EXCLUDE_COMPONENTS)
        idf_build_get_property(component_targets __COMPONENT_TARGETS)
        foreach(component_target ${component_targets})
            __component_get_property(component_dir ${component_target} COMPONENT_DIR)
            __component_get_property(component_name ${component_target} COMPONENT_NAME)
            if(component_name IN_LIST components)
                set(include 1)
                if(TEST_COMPONENTS AND NOT component_name IN_LIST TEST_COMPONENTS)
                    set(include 0)
                endif()
                if(TEST_EXCLUDE_COMPONENTS AND component_name IN_LIST TEST_EXCLUDE_COMPONENTS)
                    set(include 0)
                endif()
                if(include AND EXISTS ${component_dir}/test)
                    __component_add(${component_dir}/test ${component_name})
                    list(APPEND test_components ${component_name}::test)
                endif()
            endif()
        endforeach()
    endif()

    set(${components_var} "${components}" PARENT_SCOPE)
    set(${test_components_var} "${test_components}" PARENT_SCOPE)
endfunction()

# Trick to temporarily redefine project(). When functions are overriden in CMake, the originals can still be accessed
# using an underscore prefixed function of the same name. The following lines make sure that __project  calls
# the original project(). See https://cmake.org/pipermail/cmake/2015-October/061751.html.
function(project)
endfunction()

function(_project)
endfunction()

macro(project project_name)
    # Initialize project, preparing COMPONENTS argument for idf_build_process()
    # call later using external COMPONENT_DIRS, COMPONENTS_DIRS, EXTRA_COMPONENTS_DIR,
    # EXTRA_COMPONENTS_DIRS, COMPONENTS, EXLUDE_COMPONENTS, TEST_COMPONENTS,
    # TEST_EXLUDE_COMPONENTS, TESTS_ALL, BUILD_TESTS
    __project_init(components test_components)

    __target_set_toolchain()

    if(CCACHE_ENABLE)
        find_program(CCACHE_FOUND ccache)
        if(CCACHE_FOUND)
            message(STATUS "ccache will be used for faster recompilation")
            set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        else()
            message(WARNING "enabled ccache in build but ccache program not found")
        endif()
    endif()

    # The actual call to project()
    __project(${project_name} C CXX ASM)

    # Generate compile_commands.json (needs to come after project call).
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

    # Since components can import third-party libraries, the original definition of project() should be restored
    # before the call to add components to the build.
    function(project)
        set(project_ARGV ARGV)
        __project(${${project_ARGV}})

        # Set the variables that project() normally sets, documented in the
        # command's docs.
        #
        # https://cmake.org/cmake/help/v3.5/command/project.html
        #
        # There is some nuance when it comes to setting version variables in terms of whether
        # CMP0048 is set to OLD or NEW. However, the proper behavior should have bee already handled by the original
        # project call, and we're just echoing the values those variables were set to.
        set(PROJECT_NAME "${PROJECT_NAME}" PARENT_SCOPE)
        set(PROJECT_BINARY_DIR "${PROJECT_BINARY_DIR}" PARENT_SCOPE)
        set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}" PARENT_SCOPE)
        set(PROJECT_VERSION "${PROJECT_VERSION}" PARENT_SCOPE)
        set(PROJECT_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}" PARENT_SCOPE)
        set(PROJECT_VERSION_MINOR "${PROJECT_VERSION_MINOR}" PARENT_SCOPE)
        set(PROJECT_VERSION_PATCH "${PROJECT_VERSION_PATCH}" PARENT_SCOPE)
        set(PROJECT_VERSION_TWEAK "${PROJECT_VERSION_TWEAK}" PARENT_SCOPE)

        set(${PROJECT_NAME}_BINARY_DIR "${${PROJECT_NAME}_BINARY_DIR}" PARENT_SCOPE)
        set(${PROJECT_NAME}_SOURCE_DIR "${${PROJECT_NAME}_SOURCE_DIR}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION_PATCH "${${PROJECT_NAME}_VERSION_PATCH}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION_TWEAK "${${PROJECT_NAME}_VERSION_TWEAK}" PARENT_SCOPE)
    endfunction()

    # Prepare the following arguments for the idf_build_process() call using external
    # user values:
    #
    # SDKCONFIG_DEFAULTS is from external SDKCONFIG_DEFAULTS
    # SDKCONFIG is from external SDKCONFIG
    # BUILD_DIR is set to project binary dir
    #
    # PROJECT_NAME is taken from the passed name from project() call
    # PROJECT_DIR is set to the current directory
    # PROJECT_VER is from the version text or git revision of the current repo
    if(SDKCONFIG_DEFAULTS)
        get_filename_component(sdkconfig_defaults "${SDKCONFIG_DEFAULTS}" ABSOLUTE)
        if(NOT EXISTS "${sdkconfig_defaults}")
            message(FATAL_ERROR "SDKCONFIG_DEFAULTS '${sdkconfig_defaults}' does not exist.")
        endif()
    else()
        if(EXISTS "${CMAKE_SOURCE_DIR}/sdkconfig.defaults")
            set(sdkconfig_defaults "${CMAKE_SOURCE_DIR}/sdkconfig.defaults")
        else()
            set(sdkconfig_defaults "")
        endif()
    endif()

    if(SDKCONFIG)
        get_filename_component(sdkconfig "${SDKCONFIG}" ABSOLUTE)
        if(NOT EXISTS "${sdkconfig}")
            message(FATAL_ERROR "SDKCONFIG '${sdkconfig}' does not exist.")
        endif()
        set(sdkconfig ${SDKCONFIG})
    else()
        set(sdkconfig "${CMAKE_CURRENT_LIST_DIR}/sdkconfig")
    endif()

    if(BUILD_DIR)
        get_filename_component(build_dir "${BUILD_DIR}" ABSOLUTE)
        if(NOT EXISTS "${build_dir}")
            message(FATAL_ERROR "BUILD_DIR '${build_dir}' does not exist.")
        endif()
    else()
        set(build_dir ${CMAKE_BINARY_DIR})
    endif()

    __project_get_revision(project_ver)

    message(STATUS "Building ESP-IDF components for target ${IDF_TARGET}")

    idf_build_process(${IDF_TARGET}
                    SDKCONFIG_DEFAULTS "${sdkconfig_defaults}"
                    SDKCONFIG ${sdkconfig}
                    BUILD_DIR ${build_dir}
                    PROJECT_NAME ${CMAKE_PROJECT_NAME}
                    PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}
                    PROJECT_VER "${project_ver}"
                    COMPONENTS "${components};${test_components}")

    # Special treatment for 'main' component for standard projects (not part of core build system).
    # Have it depend on every other component in the build. This is
    # a convenience behavior for the standard project; thus is done outside of the core build system
    # so that it treats components equally.
    #
    # This behavior should only be when user did not set REQUIRES/PRIV_REQUIRES manually.
    idf_build_get_property(build_components BUILD_COMPONENTS)
    if(idf::main IN_LIST build_components)
        __component_get_target(main_target idf::main)
        __component_get_property(reqs ${main_target} REQUIRES)
        __component_get_property(priv_reqs ${main_target} PRIV_REQUIRES)
        idf_build_get_property(common_reqs __COMPONENT_REQUIRES_COMMON)
        if(reqs STREQUAL common_reqs AND NOT priv_reqs) #if user has not set any requirements
            list(REMOVE_ITEM build_components idf::main)
            __component_get_property(lib ${main_target} COMPONENT_LIB)
            set_property(TARGET ${lib} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${build_components}")
            get_property(type TARGET ${lib} PROPERTY TYPE)
            if(type STREQUAL STATIC_LIBRARY)
                set_property(TARGET ${lib} APPEND PROPERTY LINK_LIBRARIES "${build_components}")
            endif()
        endif()
    endif()

    set(project_elf ${CMAKE_PROJECT_NAME}.elf)

    # Create a dummy file to work around CMake requirement of having a source
    # file while adding an executable
    set(project_elf_src ${CMAKE_BINARY_DIR}/project_elf_src.c)
    add_custom_command(OUTPUT ${project_elf_src}
        COMMAND ${CMAKE_COMMAND} -E touch ${project_elf_src}
        VERBATIM)
    add_custom_target(_project_elf_src DEPENDS "${project_elf_src}")
    add_executable(${project_elf} "${project_elf_src}")
    add_dependencies(${project_elf} _project_elf_src)

    if(test_components)
        target_link_libraries(${project_elf} "-Wl,--whole-archive")
        foreach(test_component ${test_components})
            if(TARGET ${test_component})
                target_link_libraries(${project_elf} ${test_component})
            endif()
        endforeach()
        target_link_libraries(${project_elf} "-Wl,--no-whole-archive")
    endif()

    idf_build_get_property(build_components BUILD_COMPONENTS)
    if(test_components)
        list(REMOVE_ITEM build_components ${test_components})
    endif()
    target_link_libraries(${project_elf} ${build_components})

    set(mapfile "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map")
    target_link_libraries(${project_elf} "-Wl,--cref -Wl,--Map=${mapfile}")

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES
        "${mapfile}" "${project_elf_src}")

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(python PYTHON)

    set(idf_size ${python} ${idf_path}/tools/idf_size.py)
    if(DEFINED OUTPUT_JSON AND OUTPUT_JSON)
        list(APPEND idf_size "--json")
    endif()

    # Add size targets, depend on map file, run idf_size.py
    add_custom_target(size
        DEPENDS ${project_elf}
        COMMAND ${idf_size} ${mapfile}
        )
    add_custom_target(size-files
        DEPENDS ${project_elf}
        COMMAND ${idf_size} --files ${mapfile}
        )
    add_custom_target(size-components
        DEPENDS ${project_elf}
        COMMAND ${idf_size} --archives ${mapfile}
        )

    unset(idf_size)

    idf_build_executable(${project_elf})

    __project_info("${test_components}")
endmacro()
