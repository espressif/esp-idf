# ulp_embed_binary
#
# Create ULP binary and embed into the application.

function(__setup_ulp_project app_name project_path prefix type s_sources exp_dep_srcs)

    if(NOT CMAKE_BUILD_EARLY_EXPANSION)
        spaces2list(s_sources)
        foreach(source ${s_sources})
            get_filename_component(source ${source} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
            list(APPEND sources ${source})
        endforeach()

        foreach(source ${sources})
            get_filename_component(ps_source ${source} NAME_WE)
            set(ps_output ${CMAKE_CURRENT_BINARY_DIR}/${app_name}/${ps_source}.ulp.S)
            list(APPEND ps_sources ${ps_output})
        endforeach()

        set(ulp_artifacts_prefix ${CMAKE_CURRENT_BINARY_DIR}/${app_name}/${app_name})

        set(ulp_artifacts ${ulp_artifacts_prefix}.bin
                            ${ulp_artifacts_prefix}.ld
                            ${ulp_artifacts_prefix}.h)

        set(ulp_artifacts_extras ${ulp_artifacts_prefix}.map
                            ${ulp_artifacts_prefix}.sym
                            ${CMAKE_CURRENT_BINARY_DIR}/${app_name}/esp32.ulp.ld)

        # Replace the separator for the list of ULP source files that will be passed to
        # the external ULP project. This is a workaround to the bug https://public.kitware.com/Bug/view.php?id=16137.
        string(REPLACE ";" "|" ulp_s_sources "${ulp_s_sources}")

        idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)
        idf_build_get_property(sdkconfig_cmake SDKCONFIG_CMAKE)
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
            set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-${idf_target}-ulp.cmake)
            set(ULP_IS_RISCV OFF)
        elseif(IDF_TARGET STREQUAL "esp32s2" OR IDF_TARGET STREQUAL "esp32s3")
            # If both FSM and RISC-V are enabled in sdkconfig and a TYPE was
            # provided by the caller, use TYPE to disambiguate which toolchain
            # to select for this ULP external project.
            if((CONFIG_ULP_COPROC_TYPE_RISCV STREQUAL "y") AND (CONFIG_ULP_COPROC_TYPE_FSM STREQUAL "y"))
                message(STATUS "Both RISCV and FSM are enabled, using '${type}' toolchain for ${app_name} ULP project.")
                string(TOLOWER "${type}" type_lower)
                if(type_lower STREQUAL "riscv")
                    set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-ulp-riscv.cmake)
                elseif(type_lower STREQUAL "fsm")
                    set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-${idf_target}-ulp.cmake)
                else()
                    message(FATAL_ERROR "Invalid ULP_TYPE '${type}'; expected 'fsm' or 'riscv'.")
                endif()
            else()
                if(CONFIG_ULP_COPROC_TYPE_RISCV STREQUAL "y")
                    set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-ulp-riscv.cmake)
                else()
                    set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-${idf_target}-ulp.cmake)
                endif()
            endif()
        elseif(CONFIG_ULP_COPROC_TYPE_LP_CORE)
                set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-lp-core-riscv.cmake)
        endif()

        externalproject_add(${app_name}
                SOURCE_DIR ${project_path}
                BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${app_name}
                INSTALL_COMMAND ""
                CMAKE_ARGS  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
                            -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                            -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FLAG}
                            -DULP_S_SOURCES=$<TARGET_PROPERTY:${app_name},ULP_SOURCES>
                            -DULP_APP_NAME=${app_name}
                            -DADD_PICOLIBC_SPECS=${CONFIG_LIBC_PICOLIBC}
                            -DULP_VAR_PREFIX=${prefix}
                            -DULP_TYPE=${type}
                            -DCOMPONENT_DIR=${COMPONENT_DIR}
                            -DCOMPONENT_INCLUDES=$<TARGET_PROPERTY:${COMPONENT_TARGET},INTERFACE_INCLUDE_DIRECTORIES>
                            -DIDF_TARGET=${idf_target}
                            -DIDF_PATH=${idf_path}
                            -DSDKCONFIG_HEADER=${SDKCONFIG_HEADER}
                            -DSDKCONFIG_CMAKE=${SDKCONFIG_CMAKE}
                            -DPYTHON=${python}
                            -DCMAKE_MODULE_PATH=${idf_path}/components/ulp/cmake/
                            ${extra_cmake_args}
                BUILD_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/${app_name} --target build
                BUILD_BYPRODUCTS ${ulp_artifacts} ${ulp_artifacts_extras} ${ulp_ps_sources}
                                ${CMAKE_CURRENT_BINARY_DIR}/${app_name}/${app_name}
                BUILD_ALWAYS 1
                )

        set_property(TARGET ${app_name} PROPERTY ULP_SOURCES "${sources}")

        spaces2list(exp_dep_srcs)
        set_source_files_properties(${exp_dep_srcs} PROPERTIES OBJECT_DEPENDS ${ulp_artifacts})

        include_directories(${CMAKE_CURRENT_BINARY_DIR}/${app_name})

        add_custom_target(${app_name}_artifacts DEPENDS ${app_name})

        add_dependencies(${COMPONENT_LIB} ${app_name}_artifacts)

        target_linker_script(${COMPONENT_LIB} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/${app_name}/${app_name}.ld)
        target_add_binary_data(${COMPONENT_LIB} ${CMAKE_CURRENT_BINARY_DIR}/${app_name}/${app_name}.bin BINARY)
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

function(ulp_embed_binary app_name s_sources exp_dep_srcs)
    cmake_parse_arguments(ULP "" "PREFIX;TYPE" "" ${ARGN})
    if(NOT ULP_PREFIX)
        set(ULP_PREFIX "ulp_")
    endif()

    validate_ulp_type("${ULP_TYPE}")

    __setup_ulp_project("${app_name}" "${idf_path}/components/ulp/cmake"
                        "${ULP_PREFIX}" "${ULP_TYPE}" "${s_sources}" "${exp_dep_srcs}")
endfunction()

function(ulp_add_project app_name project_path)
    cmake_parse_arguments(ULP "" "PREFIX;TYPE" "" ${ARGN})
    if(NOT ULP_PREFIX)
        set(ULP_PREFIX "ulp_")
    endif()

    validate_ulp_type("${ULP_TYPE}")

    __setup_ulp_project("${app_name}" "${project_path}" "${ULP_PREFIX}" "${ULP_TYPE}" "" "")
endfunction()
