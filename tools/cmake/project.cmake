# Designed to be included from an IDF app's CMakeLists.txt file
cmake_minimum_required(VERSION 3.16)

# Get the currently selected sdkconfig file early, so this doesn't
# have to be done multiple times on different places.
if(SDKCONFIG)
    get_filename_component(sdkconfig "${SDKCONFIG}" ABSOLUTE)
else()
    set(sdkconfig "${CMAKE_SOURCE_DIR}/sdkconfig")
endif()

# Check if the cmake was started as part of the set-target action.
# If so, check for existing sdkconfig file and rename it.
# This is done before __target_init, so the existing IDF_TARGET from sdkconfig
# is not considered for consistence checking.
if("$ENV{_IDF_PY_SET_TARGET_ACTION}" EQUAL "1" AND EXISTS "${sdkconfig}")
    file(RENAME "${sdkconfig}" "${sdkconfig}.old")
    message(STATUS "Existing sdkconfig '${sdkconfig}' renamed to '${sdkconfig}.old'.")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/targets.cmake)
# Initialize build target for this build using the environment variable or
# value passed externally.
__target_init("${sdkconfig}")

# The mere inclusion of this CMake file sets up some internal build properties.
# These properties can be modified in between this inclusion the the idf_build_process
# call.
include(${CMAKE_CURRENT_LIST_DIR}/idf.cmake)

# setting PYTHON variable here for compatibility only, new code should use
# idf_build_get_property(variable PYTHON)
idf_build_get_property(PYTHON PYTHON)
if(NOT PYTHON)
    message(FATAL_ERROR "Internal error, PYTHON build property not set correctly.")
endif()

# legacy variable for compatibility
set(IDFTOOL ${PYTHON} "${IDF_PATH}/tools/idf.py")

# On processing, checking Python required modules can be turned off if it was
# already checked externally.
if(PYTHON_DEPS_CHECKED)
    idf_build_set_property(__CHECK_PYTHON 0)
endif()

# Store CMake arguments that need to be passed into all CMake sub-projects as well
# (bootloader, ULP, etc)
#
# It's not possible to tell if CMake was called with --warn-uninitialized, so to also
# have these warnings in sub-projects we set a cache variable as well and then check that.
if(WARN_UNINITIALIZED)
    idf_build_set_property(EXTRA_CMAKE_ARGS --warn-uninitialized)
else()
    idf_build_set_property(EXTRA_CMAKE_ARGS "")
endif()


# Enable the component manager for regular projects if not explicitly disabled.
if(NOT "$ENV{IDF_COMPONENT_MANAGER}" EQUAL "0")
    idf_build_set_property(IDF_COMPONENT_MANAGER 1)
endif()
# Set component manager interface version
idf_build_set_property(__COMPONENT_MANAGER_INTERFACE_VERSION 2)

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
                message(STATUS "Could not use 'git describe' to determine PROJECT_VER.")
                set(PROJECT_VER 1)
            endif()
        endif()
    endif()
    set(${var} "${PROJECT_VER}" PARENT_SCOPE)
endfunction()


# paths_with_spaces_to_list
#
# Replacement for spaces2list in cases where it was previously used on
# directory lists.
#
# If the variable doesn't contain spaces, (e.g. is already a CMake list)
# then the variable is unchanged. Otherwise an external Python script is called
# to try to split the paths, and the variable is updated with the result.
#
# This feature is added only for compatibility. Please do not introduce new
# space separated path lists.
#
function(paths_with_spaces_to_list variable_name)
    if("${${variable_name}}" MATCHES "[ \t]")
        idf_build_get_property(python PYTHON)
        idf_build_get_property(idf_path IDF_PATH)
        execute_process(
            COMMAND ${python}
                "${idf_path}/tools/split_paths_by_spaces.py"
                "--var-name=${variable_name}"
                "${${variable_name}}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
            OUTPUT_VARIABLE result
            RESULT_VARIABLE ret)
        if(NOT ret EQUAL 0)
            message(FATAL_ERROR "Failed to parse ${variable_name}, see diagnostics above")
        endif()
        set("${variable_name}" "${result}" PARENT_SCOPE)
    endif()
endfunction()

function(__component_info components output)
    set(components_json "")
    foreach(name ${components})
        __component_get_target(target ${name})
        __component_get_property(alias ${target} COMPONENT_ALIAS)
        __component_get_property(prefix ${target} __PREFIX)
        __component_get_property(dir ${target} COMPONENT_DIR)
        __component_get_property(type ${target} COMPONENT_TYPE)
        __component_get_property(lib ${target} COMPONENT_LIB)
        __component_get_property(reqs ${target} REQUIRES)
        __component_get_property(include_dirs ${target} INCLUDE_DIRS)
        __component_get_property(priv_reqs ${target} PRIV_REQUIRES)
        __component_get_property(managed_reqs ${target} MANAGED_REQUIRES)
        __component_get_property(managed_priv_reqs ${target} MANAGED_PRIV_REQUIRES)
        if("${type}" STREQUAL "LIBRARY")
            set(file "$<TARGET_LINKER_FILE:${lib}>")

            # The idf_component_register function is converting each source file path defined
            # in SRCS into absolute one. But source files can be also added with cmake's
            # target_sources and have relative paths. This is used for example in log
            # component. Let's make sure all source files have absolute path.
            set(sources "")
            get_target_property(srcs ${lib} SOURCES)
            foreach(src ${srcs})
                get_filename_component(src "${src}" ABSOLUTE BASE_DIR "${dir}")
                list(APPEND sources "${src}")
            endforeach()

        else()
            set(file "")
            set(sources "")
        endif()

        make_json_list("${reqs}" reqs)
        make_json_list("${priv_reqs}" priv_reqs)
        make_json_list("${managed_reqs}" managed_reqs)
        make_json_list("${managed_priv_reqs}" managed_priv_reqs)
        make_json_list("${include_dirs}" include_dirs)
        make_json_list("${sources}" sources)

        string(JOIN "\n" component_json
            "        \"${name}\": {"
            "            \"alias\": \"${alias}\","
            "            \"target\": \"${target}\","
            "            \"prefix\": \"${prefix}\","
            "            \"dir\": \"${dir}\","
            "            \"type\": \"${type}\","
            "            \"lib\": \"${lib}\","
            "            \"reqs\": ${reqs},"
            "            \"priv_reqs\": ${priv_reqs},"
            "            \"managed_reqs\": ${managed_reqs},"
            "            \"managed_priv_reqs\": ${managed_priv_reqs},"
            "            \"file\": \"${file}\","
            "            \"sources\": ${sources},"
            "            \"include_dirs\": ${include_dirs}"
            "        }"
        )
        string(CONFIGURE "${component_json}" component_json)
        if(NOT "${components_json}" STREQUAL "")
            string(APPEND components_json ",\n")
        endif()
        string(APPEND components_json "${component_json}")
    endforeach()
    string(PREPEND components_json "{\n")
    string(APPEND components_json "\n    }")
    set(${output} "${components_json}" PARENT_SCOPE)
endfunction()

#
# Output the built components to the user. Generates files for invoking esp_idf_monitor
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
    idf_build_get_property(PROJECT_VER PROJECT_VER)
    idf_build_get_property(PROJECT_PATH PROJECT_DIR)
    idf_build_get_property(BUILD_DIR BUILD_DIR)
    idf_build_get_property(SDKCONFIG SDKCONFIG)
    idf_build_get_property(SDKCONFIG_DEFAULTS SDKCONFIG_DEFAULTS)
    idf_build_get_property(PROJECT_EXECUTABLE EXECUTABLE)
    set(PROJECT_BIN ${CMAKE_PROJECT_NAME}.bin)
    idf_build_get_property(IDF_VER IDF_VER)
    idf_build_get_property(common_component_reqs __COMPONENT_REQUIRES_COMMON)

    idf_build_get_property(sdkconfig_cmake SDKCONFIG_CMAKE)
    include(${sdkconfig_cmake})
    idf_build_get_property(COMPONENT_KCONFIGS KCONFIGS)
    idf_build_get_property(COMPONENT_KCONFIGS_PROJBUILD KCONFIG_PROJBUILDS)
    idf_build_get_property(debug_prefix_map_gdbinit DEBUG_PREFIX_MAP_GDBINIT)

    if(CONFIG_APP_BUILD_TYPE_RAM)
        set(PROJECT_BUILD_TYPE ram_app)
    else()
        set(PROJECT_BUILD_TYPE flash_app)
    endif()

    # Write project description JSON file
    idf_build_get_property(build_dir BUILD_DIR)
    make_json_list("${build_components};${test_components}" build_components_json)
    make_json_list("${build_component_paths};${test_component_paths}" build_component_paths_json)
    make_json_list("${common_component_reqs}" common_component_reqs_json)

    __component_info("${build_components};${test_components}" build_component_info_json)

    # The configure_file function doesn't process generator expressions, which are needed
    # e.g. to get component target library(TARGET_LINKER_FILE), so the project_description
    # file is created in two steps. The first step, with configure_file, creates a temporary
    # file with cmake's variables substituted and unprocessed generator expressions. The second
    # step, with file(GENERATE), processes the temporary file and substitute generator expression
    # into the final project_description.json file.
    configure_file("${idf_path}/tools/cmake/project_description.json.in"
        "${build_dir}/project_description.json.templ")
    file(READ "${build_dir}/project_description.json.templ" project_description_json_templ)
    file(REMOVE "${build_dir}/project_description.json.templ")
    file(GENERATE OUTPUT "${build_dir}/project_description.json"
         CONTENT "${project_description_json_templ}")

    # Generate component dependency graph
    depgraph_generate("${build_dir}/component_deps.dot")

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
        # The directory itself is a valid idf component
        if(EXISTS ${component_dir}/CMakeLists.txt)
            idf_build_component(${component_dir})
        else()
            # otherwise, check whether the subfolders are potential idf components
            file(GLOB component_dirs ${component_dir}/*)
            foreach(component_dir ${component_dirs})
                if(IS_DIRECTORY ${component_dir})
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
        paths_with_spaces_to_list(COMPONENT_DIRS)
        idf_build_set_property(__COMPONENT_TARGETS "")
        foreach(component_dir ${COMPONENT_DIRS})
            get_filename_component(component_abs_path ${component_dir} ABSOLUTE)
            if(NOT EXISTS ${component_abs_path})
                message(FATAL_ERROR "Directory specified in COMPONENT_DIRS doesn't exist: ${component_abs_path}")
            endif()
            __project_component_dir(${component_dir})
        endforeach()
    else()
        if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/main")
            __project_component_dir("${CMAKE_CURRENT_LIST_DIR}/main")
        endif()

        paths_with_spaces_to_list(EXTRA_COMPONENT_DIRS)
        foreach(component_dir ${EXTRA_COMPONENT_DIRS})
            get_filename_component(component_abs_path ${component_dir} ABSOLUTE)
            if(NOT EXISTS ${component_abs_path})
                message(FATAL_ERROR "Directory specified in EXTRA_COMPONENT_DIRS doesn't exist: ${component_abs_path}")
            endif()
            __project_component_dir("${component_dir}")
        endforeach()

        # Look for components in the usual places: CMAKE_CURRENT_LIST_DIR/main,
        # extra component dirs, and CMAKE_CURRENT_LIST_DIR/components
        __project_component_dir("${CMAKE_CURRENT_LIST_DIR}/components")
    endif()

    # For bootloader components, we only need to set-up the Kconfig files.
    # Indeed, bootloader is currently compiled as a subproject, thus,
    # its components are not part of the main project.
    # However, in order to be able to configure these bootloader components
    # using menuconfig, we need to look for their Kconfig-related files now.
    file(GLOB bootloader_component_dirs "${CMAKE_CURRENT_LIST_DIR}/bootloader_components/*")
    list(SORT bootloader_component_dirs)
    foreach(bootloader_component_dir ${bootloader_component_dirs})
        if(IS_DIRECTORY ${bootloader_component_dir})
            __component_dir_quick_check(is_component ${bootloader_component_dir})
            if(is_component)
                __kconfig_bootloader_component_add("${bootloader_component_dir}")
            endif()
        endif()
    endforeach()

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

# Trick to temporarily redefine project(). When functions are overridden in CMake, the originals can still be accessed
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
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # If CMAKE_COLOR_DIAGNOSTICS not set in project CMakeLists.txt or in the environment,
    # enable it by default.
    if(NOT DEFINED CMAKE_COLOR_DIAGNOSTICS AND NOT DEFINED ENV{CMAKE_COLOR_DIAGNOSTICS})
        set(CMAKE_COLOR_DIAGNOSTICS ON)
    endif()

    # Since components can import third-party libraries, the original definition of project() should be restored
    # before the call to add components to the build.
    function(project)
        set(project_ARGV ARGV)
        __project(${${project_ARGV}})

        # Set the variables that project() normally sets, documented in the
        # command's docs.
        #
        # https://cmake.org/cmake/help/v3.16/command/project.html
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
        set(PROJECT_DESCRIPTION "${PROJECT_DESCRIPTION}" PARENT_SCOPE)
        set(PROJECT_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}" PARENT_SCOPE)

        set(${PROJECT_NAME}_BINARY_DIR "${${PROJECT_NAME}_BINARY_DIR}" PARENT_SCOPE)
        set(${PROJECT_NAME}_SOURCE_DIR "${${PROJECT_NAME}_SOURCE_DIR}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION_PATCH "${${PROJECT_NAME}_VERSION_PATCH}" PARENT_SCOPE)
        set(${PROJECT_NAME}_VERSION_TWEAK "${${PROJECT_NAME}_VERSION_TWEAK}" PARENT_SCOPE)
        set(${PROJECT_NAME}_DESCRIPTION "${${PROJECT_NAME}_DESCRIPTION}" PARENT_SCOPE)
        set(${PROJECT_NAME}_HOMEPAGE_URL "${${PROJECT_NAME}_HOMEPAGE_URL}" PARENT_SCOPE)
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

    # SDKCONFIG_DEFAULTS environment variable may specify a file name relative to the root of the project.
    # When building the bootloader, ignore this variable, since:
    # 1. The bootloader project uses an existing SDKCONFIG file from the top-level project
    # 2. File specified by SDKCONFIG_DEFAULTS will not be found relative to the root of the bootloader project
    if(NOT BOOTLOADER_BUILD)
        set(_sdkconfig_defaults "$ENV{SDKCONFIG_DEFAULTS}")
    endif()

    if(NOT _sdkconfig_defaults)
        if(EXISTS "${CMAKE_SOURCE_DIR}/sdkconfig.defaults")
            set(_sdkconfig_defaults "${CMAKE_SOURCE_DIR}/sdkconfig.defaults")
        else()
            set(_sdkconfig_defaults "")
        endif()
    endif()

    if(SDKCONFIG_DEFAULTS)
        set(_sdkconfig_defaults "${SDKCONFIG_DEFAULTS}")
    endif()

    foreach(sdkconfig_default ${_sdkconfig_defaults})
        get_filename_component(sdkconfig_default "${sdkconfig_default}" ABSOLUTE)
        if(NOT EXISTS "${sdkconfig_default}")
            message(FATAL_ERROR "SDKCONFIG_DEFAULTS '${sdkconfig_default}' does not exist.")
        endif()
        list(APPEND sdkconfig_defaults ${sdkconfig_default})
    endforeach()

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
    idf_build_get_property(build_components BUILD_COMPONENT_ALIASES)
    if(idf::main IN_LIST build_components)
        __component_get_target(main_target idf::main)
        __component_get_property(reqs ${main_target} REQUIRES)
        __component_get_property(priv_reqs ${main_target} PRIV_REQUIRES)
        __component_get_property(managed_reqs ${main_target} MANAGED_REQUIRES)
        __component_get_property(managed_priv_reqs ${main_target} MANAGED_PRIV_REQUIRES)
        #if user has not set any requirements, except ones added with the component manager
        if((NOT reqs OR reqs STREQUAL managed_reqs) AND (NOT priv_reqs OR priv_reqs STREQUAL managed_priv_reqs))
            if(test_components)
                list(REMOVE_ITEM build_components ${test_components})
            endif()
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

    # Create a dummy file to work around CMake requirement of having a source file while adding an
    # executable. This is also used by esp_idf_size to detect the target
    set(project_elf_src ${CMAKE_BINARY_DIR}/project_elf_src_${IDF_TARGET}.c)
    add_custom_command(OUTPUT ${project_elf_src}
        COMMAND ${CMAKE_COMMAND} -E touch ${project_elf_src}
        VERBATIM)
    add_custom_target(_project_elf_src DEPENDS "${project_elf_src}")
    add_executable(${project_elf} "${project_elf_src}")
    add_dependencies(${project_elf} _project_elf_src)

    if(__PROJECT_GROUP_LINK_COMPONENTS)
        target_link_libraries(${project_elf} PRIVATE "-Wl,--start-group")
    endif()

    if(test_components)
        target_link_libraries(${project_elf} PRIVATE "-Wl,--whole-archive")
        foreach(test_component ${test_components})
            if(TARGET ${test_component})
                target_link_libraries(${project_elf} PRIVATE ${test_component})
            endif()
        endforeach()
        target_link_libraries(${project_elf} PRIVATE "-Wl,--no-whole-archive")
    endif()

    idf_build_get_property(build_components BUILD_COMPONENT_ALIASES)
    if(test_components)
        list(REMOVE_ITEM build_components ${test_components})
    endif()

    if(CONFIG_IDF_TARGET_LINUX AND CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        # Compiling for the host, and the host is macOS, so the linker is Darwin LD.
        # Note, when adding support for Clang and LLD based toolchain this check will
        # need to be modified.
        set(linker_type "Darwin")
    else()
        set(linker_type "GNU")
    endif()

    foreach(build_component ${build_components})
        __component_get_target(build_component_target ${build_component})
        __component_get_property(whole_archive ${build_component_target} WHOLE_ARCHIVE)
        if(whole_archive)
            if(linker_type STREQUAL "GNU")
                message(STATUS "Component ${build_component} will be linked with -Wl,--whole-archive")
                target_link_libraries(${project_elf} PRIVATE
                                       "-Wl,--whole-archive"
                                       ${build_component}
                                       "-Wl,--no-whole-archive")
            elseif(linker_type STREQUAL "Darwin")
                message(STATUS "Component ${build_component} will be linked with -Wl,-force_load")
                target_link_libraries(${project_elf} PRIVATE
                                       "-Wl,-force_load"
                                       ${build_component})
            endif()
        else()
            target_link_libraries(${project_elf} PRIVATE ${build_component})
        endif()
    endforeach()


    if(linker_type STREQUAL "GNU")
        set(mapfile "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map")
        set(idf_target "${IDF_TARGET}")
        string(TOUPPER ${idf_target} idf_target)
        # Add cross-reference table to the map file
        target_link_options(${project_elf} PRIVATE "-Wl,--cref")
        # Add this symbol as a hint for esp_idf_size to guess the target name
        target_link_options(${project_elf} PRIVATE "-Wl,--defsym=IDF_TARGET_${idf_target}=0")
        # Enable map file output
        target_link_options(${project_elf} PRIVATE "-Wl,--Map=${mapfile}")
        # Check if linker supports --no-warn-rwx-segments
        execute_process(COMMAND ${CMAKE_LINKER} "--no-warn-rwx-segments" "--version"
            RESULT_VARIABLE result
            OUTPUT_QUIET
            ERROR_QUIET)
        if(${result} EQUAL 0)
            # Do not print RWX segment warnings
            target_link_options(${project_elf} PRIVATE "-Wl,--no-warn-rwx-segments")
        endif()
        unset(idf_target)
    endif()

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
        ADDITIONAL_CLEAN_FILES
        "${mapfile}" "${project_elf_src}")

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(python PYTHON)

    set(idf_size ${python} -m esp_idf_size)

    # Add size targets, depend on map file, run esp_idf_size
    # OUTPUT_JSON is passed for compatibility reasons, SIZE_OUTPUT_FORMAT
    # environment variable is recommended and has higher priority
    add_custom_target(size
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_SIZE_TOOL=${idf_size}"
        -D "MAP_FILE=${mapfile}"
        -D "OUTPUT_JSON=${OUTPUT_JSON}"
        -P "${idf_path}/tools/cmake/run_size_tool.cmake"
        DEPENDS ${mapfile}
        USES_TERMINAL
        VERBATIM
    )

    add_custom_target(size-files
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_SIZE_TOOL=${idf_size}"
        -D "IDF_SIZE_MODE=--files"
        -D "MAP_FILE=${mapfile}"
        -D "OUTPUT_JSON=${OUTPUT_JSON}"
        -P "${idf_path}/tools/cmake/run_size_tool.cmake"
        DEPENDS ${mapfile}
        USES_TERMINAL
        VERBATIM
    )

    add_custom_target(size-components
        COMMAND ${CMAKE_COMMAND}
        -D "IDF_SIZE_TOOL=${idf_size}"
        -D "IDF_SIZE_MODE=--archives"
        -D "MAP_FILE=${mapfile}"
        -D "OUTPUT_JSON=${OUTPUT_JSON}"
        -P "${idf_path}/tools/cmake/run_size_tool.cmake"
        DEPENDS ${mapfile}
        USES_TERMINAL
        VERBATIM
    )

    unset(idf_size)

    # Add DFU build and flash targets
    __add_dfu_targets()

    # Add UF2 build targets
    __add_uf2_targets()

    idf_build_executable(${project_elf})

    __project_info("${test_components}")
endmacro()
