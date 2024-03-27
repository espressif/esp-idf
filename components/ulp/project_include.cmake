# ulp_embed_binary
#
# Create ULP binary and embed into the application.
function(ulp_embed_binary app_name s_sources exp_dep_srcs)
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
        idf_build_get_property(idf_path IDF_PATH)
        idf_build_get_property(idf_target IDF_TARGET)
        idf_build_get_property(python PYTHON)
        idf_build_get_property(extra_cmake_args EXTRA_CMAKE_ARGS)

        if(IDF_TARGET STREQUAL "esp32")
            set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-${idf_target}-ulp.cmake)
            set(ULP_IS_RISCV OFF)
        elseif(IDF_TARGET STREQUAL "esp32s2" OR IDF_TARGET STREQUAL "esp32s3")
            if(CONFIG_ULP_COPROC_TYPE_RISCV STREQUAL "y")
                set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-ulp-riscv.cmake)
                set(ULP_IS_RISCV ON)
            else()
                set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-${idf_target}-ulp.cmake)
                set(ULP_IS_RISCV OFF)
            endif()
        elseif(CONFIG_ULP_COPROC_TYPE_LP_CORE)
                set(TOOLCHAIN_FLAG ${idf_path}/components/ulp/cmake/toolchain-lp-core-riscv.cmake)
                set(ULP_IS_LP_CORE_RISCV ON)
                if(CONFIG_ESP_ROM_HAS_LP_ROM)
                    set(CONFIG_ESP_ROM_HAS_LP_ROM ON)
                endif()
        endif()

        externalproject_add(${app_name}
                SOURCE_DIR ${idf_path}/components/ulp/cmake
                BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${app_name}
                INSTALL_COMMAND ""
                CMAKE_ARGS  -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                            -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FLAG}
                            -DULP_S_SOURCES=$<TARGET_PROPERTY:${app_name},ULP_SOURCES>
                            -DULP_APP_NAME=${app_name}
                            -DCOMPONENT_DIR=${COMPONENT_DIR}
                            -DCOMPONENT_INCLUDES=$<TARGET_PROPERTY:${COMPONENT_TARGET},INTERFACE_INCLUDE_DIRECTORIES>
                            -DIDF_TARGET=${idf_target}
                            -DIDF_PATH=${idf_path}
                            -DSDKCONFIG_HEADER=${SDKCONFIG_HEADER}
                            -DPYTHON=${python}
                            -DULP_COCPU_IS_RISCV=${ULP_IS_RISCV}
                            -DULP_COCPU_IS_LP_CORE=${ULP_IS_LP_CORE_RISCV}
                            -DCONFIG_ESP_ROM_HAS_LP_ROM=${CONFIG_ESP_ROM_HAS_LP_ROM}
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
