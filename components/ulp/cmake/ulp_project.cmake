# ULP subproject entry point for CMake v2 full-subproject builds.
#
# A ULP child project includes this file instead of tools/cmakev2/idf.cmake
# directly. It pulls in the cmakev2 build system and layers the ULP subproject
# API on top: ulp_project_init, ulp_build_executable, ulp_build_binary and
# ulp_project_default, the ULP analogs of idf_project_init,
# idf_build_executable, idf_build_binary and idf_project_default.

include(${IDF_PATH}/tools/cmakev2/idf.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/IDFULPProjectCommon.cmake)

# Reset the compile/link state inherited from the parent app build and select
# ULP-specific build behavior. Idempotent within a child configure.
function(__ulp_prepare_build)
    idf_build_get_property(ulp_build_prepared __ULP_BUILD_PREPARED)
    if(ulp_build_prepared)
        return()
    endif()

    if(BUILD_FSM)
        check_expected_tool_version("esp32ulp-elf" ${CMAKE_ASM_COMPILER})
        # FSM links with esp32ulp-elf-ld directly, not through GCC, so avoid
        # GCC-driver link grouping flags from the generic executable helper.
        idf_build_set_property(LINKER_TYPE ULP_FSM)
    endif()

    # ULP projects use their own toolchain files and component-provided options,
    # so discard compile/link options added for normal app builds.
    foreach(property IN ITEMS
        COMPILE_OPTIONS
        C_COMPILE_OPTIONS
        CXX_COMPILE_OPTIONS
        ASM_COMPILE_OPTIONS
        COMPILE_DEFINITIONS
        LINK_OPTIONS)
        idf_build_set_property(${property} "")
    endforeach()

    # Do not apply the default app component closure to ULP child projects.
    # ULP components should contribute dependencies explicitly.
    idf_build_set_property(__COMPONENT_REQUIRES_COMMON "")
    idf_build_set_property(__COMMON_COMPONENT_INTERFACES "")
    idf_build_set_property(__COMMON_COMPONENTS_INITIALIZED YES)

    idf_build_set_property(__ULP_BUILD_PREPARED YES)
endfunction()

#[[api
.. cmakev2:macro:: ulp_project_init

    .. code-block:: cmake

        ulp_project_init()

    Initialize a ULP full-subproject build. Calls :cmakev2:ref:`idf_project_init`
    and then applies the ULP-specific setup: it detects the ULP type (RISC-V, LP
    core or FSM) from the ``ULP_TYPE`` variable or the project configuration and
    resets the compile and link options inherited from the parent application
    build, so the ULP executable uses only its own toolchain and
    component-provided options.

    This is the ULP analog of :cmakev2:ref:`idf_project_init` and is the entry
    point for projects that build more than one ULP executable, followed by
    :cmakev2:ref:`ulp_build_executable` and :cmakev2:ref:`ulp_build_binary`
    calls. For the common single-executable case use
    :cmakev2:ref:`ulp_project_default` instead.
#]]
macro(ulp_project_init)
    idf_project_init()
    ulp_detect_build_type()
    ulp_apply_build_type_options()
    __ulp_prepare_build()
endmacro()

#[[api
.. cmakev2:function:: ulp_build_executable

    .. code-block:: cmake

        ulp_build_executable(<executable>
                             [COMPONENTS <component>...]
                             [MAPFILE_TARGET <target>])

    *executable[in]*

        Name of the ULP executable target to create.

    Build a ULP executable. This is the ULP analog of
    :cmakev2:ref:`idf_build_executable` and forwards its arguments to
    :cmakev2:ref:`idf_build_executable`. Call :cmakev2:ref:`ulp_build_binary`
    for each executable that should produce embeddable ULP artifacts.
#]]
function(ulp_build_executable executable)
    if("PREFIX" IN_LIST ARGN)
        message(FATAL_ERROR "Use ulp_build_binary(${executable} PREFIX ...) to set ULP symbol prefixes.")
    endif()

    idf_build_executable(${executable} ${ARGN})
endfunction()

#[[api
.. cmakev2:function:: ulp_build_binary

    .. code-block:: cmake

        ulp_build_binary(<executable>
                         [PREFIX <prefix>])

    *executable[in]*

        ULP executable target for which to generate embeddable artifacts.

    *PREFIX[in,opt]*

        Symbol name prefix for the generated symbol header and linker export
        files. Defaults to ``ulp_``. Use a distinct prefix per executable when a
        project builds several ULP binaries.

    Generate the ``.bin`` payload and symbol header/linker export files for a
    ULP executable. This is the ULP analog of :cmakev2:ref:`idf_build_binary`.
#]]
function(ulp_build_binary executable)
    cmake_parse_arguments(_ULP "" "PREFIX" "" ${ARGN})
    if(_ULP_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unexpected arguments to ulp_build_binary(): ${_ULP_UNPARSED_ARGUMENTS}")
    endif()

    if(DEFINED _ULP_PREFIX)
        ulp_add_build_binary_targets(${executable} PREFIX "${_ULP_PREFIX}")
    elseif(ULP_PREFIX_APPEND_BIN_NAME)
        set(prefix "ulp_")
        if(DEFINED ULP_VAR_PREFIX)
            set(prefix "${ULP_VAR_PREFIX}")
        endif()
        string(MAKE_C_IDENTIFIER "${prefix}${executable}_" prefix)
        ulp_add_build_binary_targets(${executable} PREFIX "${prefix}")
    else()
        ulp_add_build_binary_targets("${executable}")
    endif()
endfunction()

# Build the single default ULP executable from the main component.
function(__ulp_project_default)
    set(ulp_app_name "${PROJECT_NAME}")

    set(executable_args COMPONENTS main)
    set(binary_args)
    if(NOT BUILD_FSM)
        list(APPEND executable_args MAPFILE_TARGET ${ulp_app_name}_mapfile)
    endif()
    if(DEFINED ULP_VAR_PREFIX AND NOT "${ULP_VAR_PREFIX}" STREQUAL "")
        list(APPEND binary_args PREFIX ${ULP_VAR_PREFIX})
    endif()

    ulp_build_executable(${ulp_app_name} ${executable_args})
    ulp_build_binary(${ulp_app_name} ${binary_args})

    idf_build_generate_metadata(EXECUTABLE ${ulp_app_name})

    if(TARGET "${ulp_app_name}_mapfile")
        idf_create_size_report("${ulp_app_name}_mapfile" TARGET size)
    endif()

    idf_build_generate_depgraph("${ulp_app_name}")
endfunction()

#[[api
.. cmakev2:macro:: ulp_project_default

    .. code-block:: cmake

        ulp_project_default()

    Build a single ULP executable from the ``main`` component and its transitive
    dependencies, the ULP analog of :cmakev2:ref:`idf_project_default`. Calls
    :cmakev2:ref:`ulp_project_init`, then builds the executable (named after
    ``PROJECT_NAME``) with :cmakev2:ref:`ulp_build_executable` and generates its
    embeddable artifacts with :cmakev2:ref:`ulp_build_binary`.
#]]
macro(ulp_project_default)
    ulp_project_init()

    # A single executable is built, so DEFERRED optional-requires resolution is
    # safe and keeps the linked component set minimal.
    idf_build_set_property(IDF_COMPONENT_OPTIONAL_REQUIRES_MODE DEFERRED)

    __ulp_project_default()
endmacro()
