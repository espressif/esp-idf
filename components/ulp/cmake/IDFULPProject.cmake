if(IDF_BUILD_V2 AND COMMAND idf_project_init)
    include(${CMAKE_CURRENT_LIST_DIR}/IDFULPProjectv2.cmake)
    return()
endif()

# Legacy ULP child projects are plain CMake projects, so the parent-provided
# sdkconfig.cmake has to be loaded here before the common ULP helpers inspect
# CONFIG_* values. CMake v1 callers pass this path explicitly.
include(${SDKCONFIG_CMAKE})
include(${CMAKE_CURRENT_LIST_DIR}/IDFULPProjectCommon.cmake)
enable_language(C ASM)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

ulp_detect_build_type()
ulp_apply_build_type_options()

# Check the supported assembler version
if(BUILD_FSM)
    check_expected_tool_version("esp32ulp-elf" ${CMAKE_ASM_COMPILER})
endif()

function(ulp_apply_default_options ulp_app_name)
    if(BUILD_RISCV)
        target_link_options(${ulp_app_name} PRIVATE "-nostartfiles")
        target_link_options(${ulp_app_name} PRIVATE -Wl,--gc-sections)
        target_link_options(${ulp_app_name} PRIVATE "-Wl,--no-warn-rwx-segments")
        target_link_options(${ulp_app_name} PRIVATE -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.map)
    elseif(BUILD_LP_CORE)
        target_link_options(${ulp_app_name} PRIVATE "-nostartfiles")
        target_link_options(${ulp_app_name} PRIVATE "-Wl,--no-warn-rwx-segments")
        target_link_options(${ulp_app_name} PRIVATE -Wl,--gc-sections)
        target_link_options(${ulp_app_name} PRIVATE -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.map)
    elseif(BUILD_FSM)
        target_link_options(${ulp_app_name} PRIVATE -Map=${CMAKE_CURRENT_BINARY_DIR}/${ulp_app_name}.map)
    endif()
endfunction()

function(ulp_apply_default_sources ulp_app_name)
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
    list(APPEND ULP_PREPRO_ARGS -I${IDF_PATH}/components/esp_system/ld/${IDF_TARGET})

    target_include_directories(${ulp_app_name} PRIVATE ${COMPONENT_INCLUDES} ${sdkconfig_dir})

    # Pre-process the linker script
    if(BUILD_RISCV)
        set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/ulp_riscv.ld)
    elseif(BUILD_LP_CORE)
        set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/lp_core_riscv.ld)
    elseif(BUILD_FSM)
        set(ULP_LD_TEMPLATE ${IDF_PATH}/components/ulp/ld/ulp_fsm.ld)
    else()
        message(FATAL_ERROR "Unable to determine ULP type. ")
    endif()

    get_filename_component(ULP_LD_SCRIPT ${ULP_LD_TEMPLATE} NAME)
    __ulp_add_preprocessed_linker_script(${ulp_app_name} ${ULP_LD_TEMPLATE} ${ULP_LD_SCRIPT}
                                         ld_script ${ULP_PREPRO_ARGS})

    # To avoid warning "Manually-specified variables were not used by the project"
    set(bypassWarning "${IDF_TARGET}")
    set(bypassWarning "${ULP_VAR_PREFIX}")
    set(bypassWarning "${ULP_TYPE}")

    # Save user sources before adding core sources
    set(ULP_USER_SOURCES ${ULP_S_SOURCES})

    # Clear ULP_S_SOURCES and rebuild it with only the sources we need
    set(ULP_S_SOURCES ${ULP_USER_SOURCES})

    if(BUILD_RISCV)
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

    elseif(BUILD_FSM)
        foreach(ulp_s_source ${ULP_S_SOURCES})
            get_filename_component(ulp_ps_source ${ulp_s_source} NAME_WE)
            set(ulp_ps_output ${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}.ulp.S)
            # Put all arguments to the list
            set(preprocessor_args -D__ASSEMBLER__ -E -P -xc -o ${ulp_ps_output} ${ULP_PREPRO_ARGS} ${ulp_s_source})

            set(compiler_arguments_file ${CMAKE_CURRENT_BINARY_DIR}/${ulp_ps_source}_args.txt)
            __ulp_create_arg_file("${preprocessor_args}" "${compiler_arguments_file}")

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

    elseif(BUILD_LP_CORE)
        list(APPEND ULP_S_SOURCES
        "${IDF_PATH}/components/ulp/lp_core/lp_core/start.S"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/vector.S"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/port/${IDF_TARGET}/vector_table.S"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_memory_shared.c"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_lp_timer_shared.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_startup.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_pmp.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_utils.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_print.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_panic.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_interrupt.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_ubsan.c"
        "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_mailbox.c"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_lp_adc_shared.c"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_lp_vad_shared.c"
        "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_critical_section_shared.c")

        if(CONFIG_SOC_LP_CORE_SUPPORT_I2C)
            list(APPEND ULP_S_SOURCES
                "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_i2c.c")
        endif()

        if(CONFIG_SOC_LP_SPI_SUPPORTED)
            list(APPEND ULP_S_SOURCES
                "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_spi.c")
        endif()

        if(CONFIG_SOC_ULP_LP_UART_SUPPORTED)
            list(APPEND ULP_S_SOURCES
                "${IDF_PATH}/components/ulp/lp_core/shared/ulp_lp_core_lp_uart_shared.c"
                "${IDF_PATH}/components/esp_driver_uart/src/uart_wakeup.c"
                "${IDF_PATH}/components/esp_hal_uart/uart_hal_iram.c"
                "${IDF_PATH}/components/esp_hal_uart/uart_hal.c"
                "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_uart.c")
        endif()

        if(CONFIG_SOC_LP_MAILBOX_SUPPORTED)
            list(APPEND ULP_S_SOURCES
                "${IDF_PATH}/components/ulp/lp_core/lp_core/port/lp_core_mailbox_impl_hw.c")
        else()
            list(APPEND ULP_S_SOURCES
                "${IDF_PATH}/components/ulp/lp_core/lp_core/port/lp_core_mailbox_impl_sw.c")
        endif()

        if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
            list(APPEND ULP_S_SOURCES
                "${IDF_PATH}/components/ulp/lp_core/lp_core/lp_core_touch.c")
        endif()

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

    endif()
endfunction()
