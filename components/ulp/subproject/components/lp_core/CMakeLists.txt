# The code compiled for the LP core.
#
# Sources shared with the driver stay in components/ulp and are named by path
# from here.

idf_build_get_property(target IDF_TARGET)
get_filename_component(ulp_dir "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

set(srcs "start.S"
         "vector.S"
         "port/${target}/vector_table.S"
         "${ulp_dir}/lp_core/shared/ulp_lp_core_memory_shared.c"
         "lp_core_startup.c"
         "lp_core_pmp.c"
         "lp_core_utils.c"
         "lp_core_print.c"
         "lp_core_panic.c"
         "lp_core_interrupt.c"
         "lp_core_ubsan.c"
         "lp_core_mailbox.c"
         "${ulp_dir}/lp_core/shared/ulp_lp_core_critical_section_shared.c")

set(requires esp_common
             esp_hal_gpio
             esp_hal_uart
             esp_rom
             hal
             riscv
             soc)
set(priv_requires esp_hal_pmu
                  esp_hw_support)

if(CONFIG_SOC_RTC_TIMER_V2 OR CONFIG_SOC_RTC_TIMER_V3)
    list(APPEND srcs "${ulp_dir}/lp_core/shared/ulp_lp_core_lp_timer_shared.c")
    list(APPEND priv_requires esp_hal_clock esp_hal_rtc_timer)
endif()

if(CONFIG_SOC_LP_CORE_SUPPORT_I2C)
    list(APPEND srcs "lp_core_i2c.c")
    list(APPEND requires esp_hal_i2c)
endif()

if(CONFIG_SOC_LP_SPI_SUPPORTED)
    list(APPEND srcs "lp_core_spi.c")
    list(APPEND priv_requires esp_hal_gpspi)
endif()

if(CONFIG_SOC_ULP_LP_UART_SUPPORTED)
    list(APPEND srcs
        "${ulp_dir}/lp_core/shared/ulp_lp_core_lp_uart_shared.c"
        "lp_core_uart.c")
    list(APPEND requires esp_driver_uart)
endif()

if(CONFIG_SOC_LP_MAILBOX_SUPPORTED)
    list(APPEND srcs "port/lp_core_mailbox_impl_hw.c")
else()
    list(APPEND srcs "port/lp_core_mailbox_impl_sw.c")
endif()

if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    list(APPEND srcs "lp_core_touch.c")
    list(APPEND requires esp_hal_touch_sens)
endif()

if(CONFIG_SOC_LP_ADC_SUPPORTED)
    list(APPEND srcs "${ulp_dir}/lp_core/shared/ulp_lp_core_lp_adc_shared.c")
    list(APPEND requires esp_adc esp_hal_ana_conv)
endif()

if(CONFIG_SOC_LP_VAD_SUPPORTED)
    list(APPEND srcs "${ulp_dir}/lp_core/shared/ulp_lp_core_lp_vad_shared.c")
    list(APPEND requires esp_driver_i2s)
endif()

# The ULP subproject in build system v1 adds these sources straight to the
# executable, so a strong handler such as ulp_lp_core_panic_handler is not
# dropped in favour of a weak default from another runtime object.
# WHOLE_ARCHIVE keeps that behaviour here.
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "${ulp_dir}/ulp_common/include"
                                    "${ulp_dir}/lp_core/include"
                                    "include"
                                    "${ulp_dir}/lp_core/shared/include"
                       REQUIRES ${requires}
                       PRIV_REQUIRES ${priv_requires}
                       WHOLE_ARCHIVE)

# -D__ASSEMBLER__ keeps the SoC headers to their macro-only form.
set(ulp_ld_flags "-D__ASSEMBLER__ -I\"${CMAKE_CURRENT_SOURCE_DIR}/ld\"")

# lp_core_riscv.ld includes soc/soc.h and esp_common headers, resolved from the
# linked component graph. It also includes the esp_system ld snippets
# (ld.common, ld.hp_mem_defs); those live in that component's ld/ directory,
# which is not a public include dir, so pass it via FLAGS.
idf_component_get_property(esp_system_dir esp_system COMPONENT_DIR)
set(lp_core_ld_flags "${ulp_ld_flags} -I\"${esp_system_dir}/ld\" -I\"${esp_system_dir}/ld/${target}\"")

# A user-supplied LINKER layout replaces the default one composed into
# lp_core_riscv.ld.in. It reaches this child project as LP_CORE_LINKER_SCRIPT
# (passed by __setup_ulp_project, same as the build system v1 path). Hand it to
# the wrapper as the LP_CORE_LINKER_INCLUDE macro it #includes. The escaped
# quotes survive separate_arguments() in the linker-script preprocessor so the
# macro expands to a quoted header-name token ("/abs/path.ld").
if(LP_CORE_LINKER_SCRIPT)
    message(STATUS "Using custom LP-core linker layout: ${LP_CORE_LINKER_SCRIPT}")
    string(APPEND lp_core_ld_flags " -DLP_CORE_LINKER_INCLUDE=\\\"${LP_CORE_LINKER_SCRIPT}\\\"")
endif()

target_linker_script(${COMPONENT_LIB} INTERFACE "ld/lp_core_riscv.ld.in" MEMORY
                     FLAGS "${lp_core_ld_flags}")

target_link_options(${COMPONENT_LIB} INTERFACE
    -nostartfiles
    -Wl,--gc-sections
    -Wl,--no-warn-rwx-segments
    LINKER:-u,main)

target_compile_definitions(${COMPONENT_LIB} PUBLIC IS_ULP_COCPU)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-implicit-fallthrough)
