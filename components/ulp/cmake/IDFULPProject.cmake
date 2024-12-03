include(${SDKCONFIG_CMAKE})
enable_language(C ASM)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Check the supported assembler version
if(CONFIG_ULP_COPROC_TYPE_FSM)
    check_expected_tool_version("esp32ulp-elf" ${CMAKE_ASM_COMPILER})
endif()

function(ulp_apply_default_options ulp_app_name)
    if(CONFIG_ULP_COPROC_TYPE_RISCV)
        target_link_options(${ulp_app_name} PRIVATE "-nostartfiles")
        target_link_options(${ulp_app_name} PRIVATE -Wl,--gc-sections)
        target_link_options(${ulp_app_name} PRIVATE "-Wl,--no-warn-rwx-segments")
        target_link_options(${ulp_app_name} PRIVATE -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.map)

    elseif(CONFIG_ULP_COPROC_TYPE_LP_CORE)
        target_link_options(${ulp_app_name} PRIVATE "-nostartfiles")
        target_link_options(${ulp_app_name} PRIVATE "-Wl,--no-warn-rwx-segments")
        target_link_options(${ulp_app_name} PRIVATE -Wl,--gc-sections)
        target_link_options(${ulp_app_name} PRIVATE -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.map)
    else()
        target_link_options(${ulp_app_name} PRIVATE -Map=${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.map)
    endif()
endfunction()

function(ulp_apply_default_sources ulp_app_name)

    function(create_arg_file arguments output_file)
        # Escape all spaces
        list(TRANSFORM arguments REPLACE " " "\\\\ ")
        # Create a single string with all args separated by space
        list(JOIN arguments " " arguments)
        # Write it into the response file
        file(WRITE ${output_file} ${arguments})
    endfunction()

    message(STATUS "Building ULP app ${ulp_app_name}")

    get_filename_component(sdkconfig_dir ${SDKCONFIG_HEADER} DIRECTORY)

    foreach(include ${COMPONENT_INCLUDES})
        list(APPEND component_includes -I${include})
    endforeach()
    list(REMOVE_DUPLICATES component_includes)

    list(APPEND ULP_PREPRO_ARGS ${component_includes})
    list(APPEND ULP_PREPRO_ARGS -I${COMPONENT_DIR})
    list(APPEND ULP_PREPRO_ARGS -I${sdkconfig_dir})
    list(APPEND ULP_PREPRO_ARGS -I${IDF_PATH}/components/esp_system/ld)

    target_include_directories(${ulp_app_name} PRIVATE ${COMPONENT_INCLUDES})

    # Pre-process the linker script
    if(CONFIG_ULP_COPROC_TYPE_RISCV)
        set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/ulp_riscv.ld)
    elseif(CONFIG_ULP_COPROC_TYPE_LP_CORE)
        set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/lp_core_riscv.ld)
    else()
        set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/ulp_fsm.ld)
    endif()

    get_filename_component(ULP_LD_SCRIPT ${ULP_LD_TEMPLATE} NAME)

    # Put all arguments to the list
    set(preprocessor_args -D__ASSEMBLER__ -E -P -xc -o ${ULP_LD_SCRIPT} ${ULP_PREPRO_ARGS} ${ULP_LD_TEMPLATE})
    set(compiler_arguments_file ${CMAKE_CURRENT_BINARY_DIR}/${ULP_LD_SCRIPT}_args.txt)
    create_arg_file("${preprocessor_args}" "${compiler_arguments_file}")

    add_custom_command(OUTPUT ${ULP_LD_SCRIPT}
                    COMMAND ${CMAKE_C_COMPILER} @${compiler_arguments_file}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    MAIN_DEPENDENCY ${ULP_LD_TEMPLATE}
                    DEPENDS ${SDKCONFIG_HEADER}
                    COMMENT "Generating ${ULP_LD_SCRIPT} linker script..."
                    VERBATIM)
    add_custom_target(ld_script DEPENDS ${ULP_LD_SCRIPT})
    add_dependencies(${ulp_app_name} ld_script)
    target_link_options(${ulp_app_name} PRIVATE SHELL:-T ${CMAKE_CURRENT_BINARY_DIR}/${ULP_LD_SCRIPT})

    # To avoid warning "Manually-specified variables were not used by the project"
    set(bypassWarning "${IDF_TARGET}")

    if(CONFIG_ULP_COPROC_TYPE_RISCV)
        #risc-v ulp uses extra files for building:
        list(APPEND ULP_S_SOURCES
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_vectors.S"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/start.S"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_adc.c"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_lock.c"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_uart.c"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_print.c"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_i2c.c"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_utils.c"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_touch.c"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_gpio.c"
            "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/ulp_riscv_interrupt.c")


        target_sources(${ulp_app_name} PRIVATE ${ULP_S_SOURCES})
        #Makes the csr utillies for riscv visible:
        target_include_directories(${ulp_app_name} PRIVATE "${IDF_PATH}/components/ulp/ulp_riscv/ulp_core/include"
                                                        "${IDF_PATH}/components/ulp/ulp_riscv/shared/include"
                                                        "${IDF_PATH}/components/riscv/include")
        target_link_options(${ulp_app_name} PRIVATE SHELL:-T ${IDF_PATH}/components/ulp/ld/${IDF_TARGET}.peripherals.ld)
        target_compile_definitions(${ulp_app_name} PRIVATE IS_ULP_COCPU)
        target_compile_definitions(${ulp_app_name} PRIVATE ULP_RISCV_REGISTER_OPS)

    elseif(CONFIG_ULP_COPROC_TYPE_LP_CORE)
        list(APPEND ULP_S_SOURCES
        "${IDF_PATH}/components/ulp/lp_core/lp_core/start.S"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/vector.S"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/port/${IDF_TARGET}/vector_table.S"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_memory_shared.c"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_lp_timer_shared.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_startup.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_utils.c"

        "${IDF_PATH}/components/hal/uart_hal_iram.c"
        "${IDF_PATH}/components/hal/uart_hal.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_uart.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_print.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_panic.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_interrupt.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_i2c.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_spi.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_ubsan.c"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_lp_adc_shared.c"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_lp_vad_shared.c"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_critical_section_shared.c")

        set(target_folder ${IDF_TARGET})

        target_link_options(${ulp_app_name}
            PRIVATE SHELL:-T ${IDF_PATH}/components/soc/${target_folder}/ld/${IDF_TARGET}.peripherals.ld)

        if(CONFIG_ESP_ROM_HAS_LP_ROM)
            target_link_options(${ulp_app_name}
                PRIVATE SHELL:-T ${IDF_PATH}/components/esp_rom/${IDF_TARGET}/ld/${IDF_TARGET}lp.rom.ld)
            target_link_options(${ulp_app_name}
                PRIVATE SHELL:-T ${IDF_PATH}/components/esp_rom/${IDF_TARGET}/ld/${IDF_TARGET}lp.rom.newlib.ld)
            target_link_options(${ulp_app_name}
                PRIVATE SHELL:-T ${IDF_PATH}/components/esp_rom/${IDF_TARGET}/ld/${IDF_TARGET}lp.rom.version.ld)
            target_link_options(${ULP_APP_NAME}
                PRIVATE SHELL:-T ${IDF_PATH}/components/esp_rom/${IDF_TARGET}/ld/${IDF_TARGET}lp.rom.api.ld)
        endif()

        target_sources(${ulp_app_name} PRIVATE ${ULP_S_SOURCES})
        target_include_directories(${ulp_app_name} PRIVATE "${IDF_PATH}/components/ulp/lp_core/lp_core/include"
                                                        "${IDF_PATH}/components/ulp/lp_core/shared/include")
        target_compile_definitions(${ulp_app_name} PRIVATE IS_ULP_COCPU)

    else()
        foreach(ulp_s_source ${ULP_S_SOURCES})
            get_filename_component(ulp_ps_source ${ulp_s_source} NAME_WE)
            set(ulp_ps_output ${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}.ulp.S)
            # Put all arguments to the list
            set(preprocessor_args -D__ASSEMBLER__ -E -P -xc -o ${ulp_ps_output} ${ULP_PREPRO_ARGS} ${ulp_s_source})

            set(compiler_arguments_file ${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}_args.txt)
            create_arg_file("${preprocessor_args}" "${compiler_arguments_file}")

            # Generate preprocessed assembly files.
            add_custom_command(OUTPUT ${ulp_ps_output}
                            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                            COMMAND ${CMAKE_C_COMPILER} @${compiler_arguments_file}
                            DEPENDS ${ulp_s_source}
                            VERBATIM)
            # During assembly file compilation, output listing files as well.
            set_source_files_properties(${ulp_ps_output}
                                        PROPERTIES COMPILE_FLAGS
                                        "-al=${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}.lst")
            list(APPEND ULP_PS_SOURCES ${ulp_ps_output})
        endforeach()

        target_sources(${ulp_app_name} PRIVATE ${ULP_PS_SOURCES})

    endif()
endfunction()

function(ulp_add_build_binary_targets ulp_app_name)

    if(ADD_PICOLIBC_SPECS)
        target_compile_options(${ulp_app_name} PRIVATE $<$<COMPILE_LANG_AND_ID:C,GNU>:-specs=picolibc.specs>)
        target_compile_options(${ulp_app_name} PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-specs=picolibcpp.specs>)
    endif()

    if(CONFIG_ULP_COPROC_TYPE_LP_CORE)
        set(ULP_BASE_ADDR "0x0")
    else()
        set(ULP_BASE_ADDR "0x50000000")
    endif()

    set(ULP_MAP_GEN ${PYTHON} ${IDF_PATH}/components/ulp/esp32ulp_mapgen.py)

    # Dump the list of global symbols in a convenient format
    add_custom_command(OUTPUT ${ULP_APP_NAME}.sym
                    COMMAND ${CMAKE_NM} -f posix -g $<TARGET_FILE:${ulp_app_name}> > ${ulp_app_name}.sym
                    DEPENDS ${ulp_app_name}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # Dump the binary for inclusion into the project
    add_custom_command(OUTPUT ${ulp_app_name}.bin
                    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${ulp_app_name}> ${ulp_app_name}.bin
                    DEPENDS ${ulp_app_name}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    add_custom_command(OUTPUT ${ulp_app_name}.ld ${ulp_app_name}.h
                    COMMAND ${ULP_MAP_GEN} -s ${ulp_app_name}.sym -o ${ulp_app_name} --base ${ULP_BASE_ADDR}
                    DEPENDS ${ulp_app_name}.sym
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # Building the component separately from the project should result in
    # ULP files being built.
    add_custom_target(build
                    DEPENDS ${ulp_app_name} ${ulp_app_name}.bin ${ulp_app_name}.sym
                            ${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.ld
                            ${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.h
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

endfunction()
