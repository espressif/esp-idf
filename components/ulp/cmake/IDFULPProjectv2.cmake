include(${CMAKE_CURRENT_LIST_DIR}/IDFULPProjectCommon.cmake)

if(NOT COMMAND idf_project_init)
    message(FATAL_ERROR "CMake v2 ULP projects must include tools/cmakev2/idf.cmake "
                        "before including IDFULPProject.")
endif()

# Full IDF-style ULP projects include sdkconfig.cmake again during
# idf_project_init(). Re-including it is harmless because the generated file
# only assigns CONFIG_* variables, and this early include keeps lower-level ULP
# helper APIs working before idf_project_init().
include(${SDKCONFIG_CMAKE})

ulp_detect_build_type()

get_property(enabled_languages GLOBAL PROPERTY ENABLED_LANGUAGES)
if(NOT ASM IN_LIST enabled_languages)
    enable_language(ASM)
endif()
ulp_apply_build_type_options()

function(ulp_prepare_build)
    idf_build_get_property(ulp_build_prepared __ULP_BUILD_PREPARED)
    if(ulp_build_prepared)
        return()
    endif()

    if(BUILD_FSM)
        check_expected_tool_version("esp32ulp-elf" ${CMAKE_ASM_COMPILER})
        # FSM links with esp32ulp-elf-ld directly, not through GCC, so avoid
        # GCC-driver link grouping flags from the generic IDF executable helper.
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

    # Some helpers can be called in sequence by custom ULP projects; keep this
    # setup idempotent within a child configure.
    idf_build_set_property(__ULP_BUILD_PREPARED YES)
endfunction()

function(__ulp_project_default)
    set(ulp_app_name "${PROJECT_NAME}")

    set(executable_args COMPONENTS main)
    if(NOT BUILD_FSM)
        list(APPEND executable_args MAPFILE_TARGET ${ulp_app_name}_mapfile)
    endif()

    idf_build_executable(${ulp_app_name} ${executable_args})

    idf_build_generate_metadata(EXECUTABLE ${ulp_app_name})

    if(TARGET "${ulp_app_name}_mapfile")
        idf_create_size_report("${ulp_app_name}_mapfile"
                               TARGET size)
    endif()

    idf_build_generate_depgraph("${ulp_app_name}")
endfunction()

macro(ulp_project_default)
    idf_project_init()
    ulp_prepare_build()

    # Use DEFERRED optional-requires resolution only when this will be the sole
    # library being built.
    idf_build_set_property(IDF_COMPONENT_OPTIONAL_REQUIRES_MODE DEFERRED)

    __ulp_project_default()
endmacro()
