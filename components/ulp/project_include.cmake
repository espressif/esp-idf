if(IDF_BUILD_V2 AND __ULP_BUILD)
    return()
endif()

# ulp_embed_binary
#
# Create ULP binary and embed into the application.

# Generate a minimal CMake v2 ULP project in the parent build directory for
# legacy ulp_embed_binary() callers. This lets the v2 ULP component graph build
# legacy source lists without writing into the application source tree.
function(__ulp_write_legacy_project project_dir sources component_dir)
    set(template_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/legacy_project")
    set(main_dir "${project_dir}/main")
    file(MAKE_DIRECTORY "${main_dir}")

    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "${template_dir}/CMakeLists.txt.in"
        "${template_dir}/main/CMakeLists.txt.in"
        "${template_dir}/main/ulp_legacy_inputs.cmake.in")

    file(GENERATE OUTPUT "${project_dir}/CMakeLists.txt"
                  INPUT "${template_dir}/CMakeLists.txt.in")
    file(GENERATE OUTPUT "${main_dir}/CMakeLists.txt"
                  INPUT "${template_dir}/main/CMakeLists.txt.in")

    set(ULP_LEGACY_SOURCES)
    foreach(source IN LISTS sources)
        string(APPEND ULP_LEGACY_SOURCES "    [=[${source}]=]\n")
    endforeach()

    set(ULP_LEGACY_INCLUDE_DIR_CANDIDATES)
    if(DEFINED COMPONENT_NAME)
        set(legacy_include_components "${COMPONENT_NAME}")
        foreach(require_property IN ITEMS REQUIRES PRIV_REQUIRES)
            idf_component_get_property(component_requires "${COMPONENT_NAME}" ${require_property})
            list(APPEND legacy_include_components ${component_requires})
        endforeach()
        list(REMOVE_DUPLICATES legacy_include_components)

        foreach(component IN LISTS legacy_include_components)
            if(component STREQUAL "ulp")
                continue()
            endif()
            idf_component_get_property(include_component_dir "${component}" COMPONENT_DIR)
            set(include_properties INCLUDE_DIRS)
            if(component STREQUAL COMPONENT_NAME)
                list(APPEND include_properties PRIV_INCLUDE_DIRS)
            endif()
            foreach(include_property IN LISTS include_properties)
                idf_component_get_property(component_includes "${component}" ${include_property})
                foreach(include IN LISTS component_includes)
                    if(IS_ABSOLUTE "${include}")
                        string(APPEND ULP_LEGACY_INCLUDE_DIR_CANDIDATES "    [=[${include}]=]\n")
                    elseif(include_component_dir)
                        string(APPEND ULP_LEGACY_INCLUDE_DIR_CANDIDATES
                            "    [=[${include_component_dir}/${include}]=]\n")
                    endif()
                endforeach()
            endforeach()
        endforeach()
    endif()
    if(component_dir)
        string(APPEND ULP_LEGACY_INCLUDE_DIR_CANDIDATES "    [=[${component_dir}]=]\n")
    endif()

    set(ULP_LEGACY_PARENT_SDKCONFIG_HEADER)
    if(SDKCONFIG_HEADER AND EXISTS "${SDKCONFIG_HEADER}")
        set(ULP_LEGACY_PARENT_SDKCONFIG_HEADER "${main_dir}/ulp_parent_sdkconfig.h")
        file(STRINGS "${SDKCONFIG_HEADER}" parent_sdkconfig_lines)
        set(parent_sdkconfig_content
            "/* Generated from the parent project sdkconfig for legacy ULP sources. */\n")
        foreach(parent_sdkconfig_line IN LISTS parent_sdkconfig_lines)
            if(parent_sdkconfig_line MATCHES "^#define CONFIG_IDF_TOOLCHAIN(_[A-Z]+)?[ \t]")
                continue()
            endif()
            string(APPEND parent_sdkconfig_content "${parent_sdkconfig_line}\n")
        endforeach()
        file(GENERATE OUTPUT "${ULP_LEGACY_PARENT_SDKCONFIG_HEADER}"
                      CONTENT "${parent_sdkconfig_content}")
    endif()

    file(READ "${template_dir}/main/ulp_legacy_inputs.cmake.in" inputs_template)
    string(CONFIGURE "${inputs_template}" inputs_content @ONLY)
    file(GENERATE OUTPUT "${main_dir}/ulp_legacy_inputs.cmake"
                  CONTENT "${inputs_content}")
endfunction()

function(__setup_ulp_project app_name project_path prefix prefix_append_bin_name type binary_names
                             legacy_project s_sources exp_dep_srcs)

    if(NOT CMAKE_BUILD_EARLY_EXPANSION)
        set(ulp_cmake_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake")
        set(sources "")
        spaces2list(s_sources)
        foreach(source ${s_sources})
            get_filename_component(source ${source} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
            list(APPEND sources ${source})
        endforeach()

        # In cmakev2, anchor the ULP sub-project's binary dir to the parent's
        # BUILD_DIR so it is not nested under the component that happened to
        # register it, and so that wiping the parent build dir removes the ULP
        # artifacts together with any child-owned sdkconfig.
        if(IDF_BUILD_V2)
            idf_build_get_property(build_dir BUILD_DIR)
            set(ulp_binary_dir "${build_dir}/subprojects/${app_name}")

            # Safety net for cases where the build directory is not wiped
            # before changing target (e.g. someone reconfigures directly via
            # `cmake -B build -DIDF_TARGET=...` instead of `idf.py set-target`,
            # which would normally pull in `fullclean` and remove the whole
            # build tree). When `_IDF_PY_SET_TARGET_ACTION=1` is set, mirror
            # what the parent does to its own sdkconfig (cmakev1 renames it,
            # cmakev2 relaxes the consistency check) and move the sub-project's
            # sdkconfig aside. The ULP's externalproject_add configure is lazy
            # and runs after the parent's configure exits, so by then
            # _IDF_PY_SET_TARGET_ACTION is no longer in the environment.
            # Without this, a stale sub-project sdkconfig from a previous
            # target would trip the cross-check inside the ULP's
            # __init_idf_target.
            set(ulp_sdkconfig "${ulp_binary_dir}/sdkconfig")
            if("$ENV{_IDF_PY_SET_TARGET_ACTION}" STREQUAL "1" AND EXISTS "${ulp_sdkconfig}")
                file(RENAME "${ulp_sdkconfig}" "${ulp_sdkconfig}.old")
                message(STATUS "Existing sub-project sdkconfig '${ulp_sdkconfig}' "
                               "renamed to '${ulp_sdkconfig}.old'.")
            endif()
        else()
            set(ulp_binary_dir "${CMAKE_CURRENT_BINARY_DIR}/${app_name}")
        endif()

        foreach(source ${sources})
            get_filename_component(ps_source ${source} NAME_WE)
            set(ps_output ${ulp_binary_dir}/${ps_source}.ulp.S)
            list(APPEND ps_sources ${ps_output})
        endforeach()

        if(NOT binary_names)
            set(binary_names "${app_name}")
        endif()

        # Full ULP subprojects may emit multiple binaries. Embed each declared
        # output in the main app so runtime code can choose which ULP program
        # to load.
        foreach(binary_name IN LISTS binary_names)
            set(ulp_artifacts_prefix ${ulp_binary_dir}/${binary_name})

            list(APPEND ulp_artifacts
                ${ulp_artifacts_prefix}.bin
                ${ulp_artifacts_prefix}.ld
                ${ulp_artifacts_prefix}.h)

            list(APPEND ulp_artifacts_extras
                ${ulp_artifacts_prefix}.map
                ${ulp_artifacts_prefix}.sym
                ${ulp_binary_dir}/${binary_name}.elf)
        endforeach()

        # Replace the separator for the list of ULP source files that will be passed to
        # the external ULP project. This is a workaround to the bug https://public.kitware.com/Bug/view.php?id=16137.
        string(REPLACE ";" "|" ulp_s_sources "${ulp_s_sources}")

        idf_build_get_property(sdkconfig SDKCONFIG)
        idf_build_get_property(idf_path IDF_PATH)
        idf_build_get_property(idf_target IDF_TARGET)
        idf_build_get_property(python PYTHON)
        idf_build_get_property(extra_cmake_args EXTRA_CMAKE_ARGS)

        if(IDF_TARGET STREQUAL "esp32")
            # esp32 only supports FSM
            if(type)
                string(TOLOWER "${type}" type_lower)
                if(type_lower STREQUAL "riscv")
                    message(FATAL_ERROR "TYPE=riscv is not supported on ${IDF_TARGET}. "
                                        "Only FSM type is available for ULP on this target.")
                endif()
            endif()
            set(TOOLCHAIN_FLAG ${ulp_cmake_dir}/toolchain-${idf_target}-ulp.cmake)
            set(ULP_IS_RISCV OFF)
        elseif(IDF_TARGET STREQUAL "esp32s2" OR IDF_TARGET STREQUAL "esp32s3")
            # If both FSM and RISC-V are enabled in sdkconfig and a TYPE was
            # provided by the caller, use TYPE to disambiguate which toolchain
            # to select for this ULP external project.
            if((CONFIG_ULP_COPROC_TYPE_RISCV STREQUAL "y") AND (CONFIG_ULP_COPROC_TYPE_FSM STREQUAL "y"))
                message(STATUS "Both RISCV and FSM are enabled, using '${type}' toolchain for ${app_name} ULP project.")
                string(TOLOWER "${type}" type_lower)
                if(type_lower STREQUAL "riscv")
                    set(TOOLCHAIN_FLAG ${ulp_cmake_dir}/toolchain-ulp-riscv.cmake)
                elseif(type_lower STREQUAL "fsm")
                    set(TOOLCHAIN_FLAG ${ulp_cmake_dir}/toolchain-${idf_target}-ulp.cmake)
                else()
                    message(FATAL_ERROR "Invalid ULP_TYPE '${type}'; expected 'fsm' or 'riscv'.")
                endif()
            else()
                if(CONFIG_ULP_COPROC_TYPE_RISCV STREQUAL "y")
                    set(TOOLCHAIN_FLAG ${ulp_cmake_dir}/toolchain-ulp-riscv.cmake)
                else()
                    set(TOOLCHAIN_FLAG ${ulp_cmake_dir}/toolchain-${idf_target}-ulp.cmake)
                endif()
            endif()
        elseif(CONFIG_ULP_COPROC_TYPE_LP_CORE)
                set(TOOLCHAIN_FLAG ${ulp_cmake_dir}/toolchain-lp-core-riscv.cmake)
        endif()

        set(ulp_project_args)
        if(IDF_BUILD_V2)
            set(ulp_project_args
                # The parent passes a superset of variables that different ULP
                # subproject styles consume (e.g. ULP_VAR_PREFIX only by
                # ulp_project_default, COMPONENT_INCLUDES only by hand-written
                # subprojects). Do not warn about the ones a given child ignores.
                --no-warn-unused-cli
                -DIDF_DEFAULT_PROJECT_NAME=${app_name}
                -DULP_APP_NAME=${app_name}
                -DIDF_BUILD_V2=y
                -DCMAKE_MODULE_PATH=${ulp_cmake_dir}
                # Internal marker for the ULP child project component graph.
                -D__ULP_BUILD=1
                -DIDF_PARENT_BUILD_DIR=${build_dir}
                -DULP_PREFIX_APPEND_BIN_NAME=${prefix_append_bin_name}
                -DSDKCONFIG=${sdkconfig}
                -DSDKCONFIG_HEADER=${SDKCONFIG_HEADER}
                -DSDKCONFIG_CMAKE=${SDKCONFIG_CMAKE}
                -DSDKCONFIG_DEFAULTS=
                -DGENERATE_SDKCONFIG=0
                -DDEPENDENCIES_LOCK=${ulp_binary_dir}/dependencies.lock
                # All ULP child builds use ULP-specific toolchain files today.
                # LP core and ULP RISC-V are GCC-based and could eventually
                # share more of the normal IDF app toolchain response-file
                # setup, but keep them under IDF_CUSTOM_TOOLCHAIN for now.
                -DIDF_CUSTOM_TOOLCHAIN=1)
            if(legacy_project)
                set(generated_project_dir "${ulp_binary_dir}/legacy_project")
                __ulp_write_legacy_project("${generated_project_dir}" "${sources}" "${COMPONENT_DIR}")
                set(project_path "${generated_project_dir}")
            elseif(s_sources)
                list(APPEND ulp_project_args
                    -DULP_S_SOURCES=$<TARGET_PROPERTY:${app_name},ULP_SOURCES>)
            endif()
            if(NOT legacy_project)
                if(DEFINED COMPONENT_TARGET AND TARGET ${COMPONENT_TARGET})
                    # Backward compatibility for existing ULP subprojects that
                    # include headers from the parent component. New full
                    # subprojects should declare their own component include
                    # directories and dependencies instead.
                    list(APPEND ulp_project_args
                        -DCOMPONENT_INCLUDES=$<TARGET_PROPERTY:${COMPONENT_TARGET},INTERFACE_INCLUDE_DIRECTORIES>)
                endif()
                if(COMPONENT_DIR)
                    list(APPEND ulp_project_args
                        -DCOMPONENT_DIR=${COMPONENT_DIR})
                endif()
            endif()
        else()
            list(APPEND ulp_project_args
                -DULP_S_SOURCES=$<TARGET_PROPERTY:${app_name},ULP_SOURCES>
                -DULP_APP_NAME=${app_name}
                -DCOMPONENT_DIR=${COMPONENT_DIR}
                -DCOMPONENT_INCLUDES=$<TARGET_PROPERTY:${COMPONENT_TARGET},INTERFACE_INCLUDE_DIRECTORIES>
                -DSDKCONFIG_HEADER=${SDKCONFIG_HEADER}
                -DSDKCONFIG_CMAKE=${SDKCONFIG_CMAKE}
                # The v1 ULP child resolves include(IDFULPProject) via the module path.
                -DCMAKE_MODULE_PATH=${ulp_cmake_dir})
        endif()

        externalproject_add(${app_name}
            SOURCE_DIR ${project_path}
            BINARY_DIR ${ulp_binary_dir}
            INSTALL_COMMAND ""
            CMAKE_ARGS  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
                        -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                        -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FLAG}
                        -DADD_PICOLIBC_SPECS=${CONFIG_LIBC_PICOLIBC}
                        -DULP_VAR_PREFIX=${prefix}
                        -DULP_TYPE=${type}
                        ${ulp_project_args}
                        -DIDF_TARGET=${idf_target}
                        -DIDF_PATH=${idf_path}
                        -DPYTHON=${python}
                        ${extra_cmake_args}
            BUILD_COMMAND ${CMAKE_COMMAND} --build ${ulp_binary_dir} --target build
            BUILD_BYPRODUCTS ${ulp_artifacts} ${ulp_artifacts_extras} ${ulp_ps_sources}
                            ${ulp_binary_dir}/${app_name}
            BUILD_ALWAYS 1
            )

        if(IDF_BUILD_V2)
            foreach(binary_name IN LISTS binary_names)
                set(ulp_compat_dir "${CMAKE_CURRENT_BINARY_DIR}/${binary_name}")
                foreach(extension IN ITEMS bin ld h map sym elf)
                    set(ulp_compat_artifact "${ulp_compat_dir}/${binary_name}.${extension}")
                    set(ulp_source_artifact "${ulp_binary_dir}/${binary_name}.${extension}")
                    add_custom_command(OUTPUT "${ulp_compat_artifact}"
                        COMMAND ${CMAKE_COMMAND} -E make_directory "${ulp_compat_dir}"
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                "${ulp_source_artifact}" "${ulp_compat_artifact}"
                        DEPENDS "${ulp_source_artifact}"
                        VERBATIM)
                endforeach()
            endforeach()
        endif()

        # Only the CMake v1 path reads this property when passing ULP_S_SOURCES
        # to the child project. Leaving it set for v2 is harmless and keeps the
        # post-ExternalProject artifact wiring common. Can be dropped when
        # v1 support is dropped.
        set_property(TARGET ${app_name} PROPERTY ULP_SOURCES "${sources}")

        spaces2list(exp_dep_srcs)
        if(exp_dep_srcs)
            set_source_files_properties(${exp_dep_srcs} PROPERTIES OBJECT_DEPENDS ${ulp_artifacts})
        endif()

        include_directories(${ulp_binary_dir})

        add_custom_target(${app_name}_artifacts DEPENDS ${app_name})

        add_dependencies(${COMPONENT_LIB} ${app_name}_artifacts)

        foreach(binary_name IN LISTS binary_names)
            target_linker_script(${COMPONENT_LIB} INTERFACE ${ulp_binary_dir}/${binary_name}.ld)
            target_add_binary_data(${COMPONENT_LIB} ${ulp_binary_dir}/${binary_name}.bin BINARY)
        endforeach()
    endif()
endfunction()

function(validate_ulp_type ULP_TYPE)
    if(CONFIG_ULP_COPROC_ENABLED)
        if(NOT CONFIG_ULP_COPROC_TYPE_FSM AND NOT CONFIG_ULP_COPROC_TYPE_RISCV AND NOT CONFIG_ULP_COPROC_TYPE_LP_CORE)
            message(FATAL_ERROR "ULP co-processor is enabled (CONFIG_ULP_COPROC_ENABLED=y), but no ULP type "
                                "is selected. Please enable at least one of: CONFIG_ULP_COPROC_TYPE_FSM, "
                                "CONFIG_ULP_COPROC_TYPE_RISCV, or CONFIG_ULP_COPROC_TYPE_LP_CORE in menuconfig.")
        endif()
    endif()

    if(NOT ULP_TYPE)
        # If the user didn't pass TYPE here and both ULP types are enabled in sdkconfig,
        # require an explicit TYPE to decide which toolchain to use.
        if((CONFIG_ULP_COPROC_TYPE_RISCV STREQUAL "y") AND (CONFIG_ULP_COPROC_TYPE_FSM STREQUAL "y"))
            message(FATAL_ERROR "Both CONFIG_ULP_COPROC_TYPE_FSM and CONFIG_ULP_COPROC_TYPE_RISCV are enabled "
                                "in menuconfig, but calling ulp_embed_binary without TYPE, so the toolchain to use "
                                "is ambiguous. Call ulp_embed_binary(... TYPE fsm) or ulp_embed_binary(... TYPE riscv)"
                                " to select the desired ULP type in your CMakeLists.txt file.")
        endif()
    endif()
endfunction()

function(__ulp_resolve_type out_var type)
    # CMake v2 ULP subprojects receive an explicit ULP_TYPE argument because the
    # child project uses it to choose the component graph. CMake v1 legacy builds
    # keep the historical empty/uppercase TYPE handling in __setup_ulp_project.
    if(type)
        string(TOLOWER "${type}" resolved_type)
    elseif(CONFIG_ULP_COPROC_TYPE_RISCV)
        set(resolved_type "riscv")
    elseif(CONFIG_ULP_COPROC_TYPE_LP_CORE)
        set(resolved_type "lp_core")
    elseif(CONFIG_ULP_COPROC_TYPE_FSM)
        set(resolved_type "fsm")
    endif()

    set(${out_var} "${resolved_type}" PARENT_SCOPE)
endfunction()

function(ulp_embed_binary app_name s_sources exp_dep_srcs)
    cmake_parse_arguments(ULP "" "PREFIX;TYPE" "" ${ARGN})
    if(NOT ULP_PREFIX)
        set(ULP_PREFIX "ulp_")
    endif()

    validate_ulp_type("${ULP_TYPE}")
    if(IDF_BUILD_V2)
        __ulp_resolve_type(ULP_TYPE "${ULP_TYPE}")
    endif()
    set(ulp_cmake_dir "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake")

    __setup_ulp_project("${app_name}" "${ulp_cmake_dir}"
                        "${ULP_PREFIX}" FALSE "${ULP_TYPE}"
                        "${app_name}"
                        TRUE "${s_sources}" "${exp_dep_srcs}")
endfunction()

function(ulp_add_project app_name project_path)
    cmake_parse_arguments(ULP "" "PREFIX;TYPE" "BINARIES" ${ARGN})
    if(NOT ULP_BINARIES)
        set(ULP_BINARIES "${app_name}")
    endif()

    list(LENGTH ULP_BINARIES ULP_BINARY_COUNT)
    set(ULP_PREFIX_APPEND_BIN_NAME FALSE)
    if(IDF_BUILD_V2 AND ULP_BINARY_COUNT GREATER 1)
        set(ULP_PREFIX_APPEND_BIN_NAME TRUE)
    endif()

    if(NOT ULP_PREFIX)
        set(ULP_PREFIX "ulp_")
    endif()

    validate_ulp_type("${ULP_TYPE}")
    if(IDF_BUILD_V2)
        __ulp_resolve_type(ULP_TYPE "${ULP_TYPE}")
    endif()

    __setup_ulp_project("${app_name}" "${project_path}" "${ULP_PREFIX}"
                        "${ULP_PREFIX_APPEND_BIN_NAME}" "${ULP_TYPE}" "${ULP_BINARIES}" FALSE "" "")
endfunction()
