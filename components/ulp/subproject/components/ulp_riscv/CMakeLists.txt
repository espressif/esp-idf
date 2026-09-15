# The code compiled for the ULP RISC-V coprocessor.
#
# Sources shared with the driver stay in components/ulp and are named by path
# from here.

idf_build_get_property(target IDF_TARGET)
get_filename_component(ulp_dir "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

# The ULP subproject in build system v1 adds these sources straight to the
# executable, so a strong handler is not dropped in favour of a weak default
# from another runtime object. WHOLE_ARCHIVE keeps that behaviour here.
idf_component_register(
    SRCS "ulp_riscv_vectors.S"
         "start.S"
         "ulp_riscv_adc.c"
         "ulp_riscv_lock.c"
         "ulp_riscv_uart.c"
         "ulp_riscv_print.c"
         "ulp_riscv_i2c.c"
         "ulp_riscv_utils.c"
         "ulp_riscv_touch.c"
         "ulp_riscv_gpio.c"
         "ulp_riscv_interrupt.c"
    INCLUDE_DIRS "${ulp_dir}/ulp_common/include"
                 "${ulp_dir}/ulp_riscv/include"
                 "${ulp_dir}/ulp_riscv/shared/include"
                 "include"
    REQUIRES esp_common
             esp_hal_ana_conv
             esp_hal_gpio
             esp_hal_i2c
             esp_hal_touch_sens
             esp_hw_support
             riscv
             soc
    WHOLE_ARCHIVE)

# -D__ASSEMBLER__ keeps the SoC headers to their macro-only form.
set(ulp_ld_flags "-D__ASSEMBLER__ -I\"${CMAKE_CURRENT_SOURCE_DIR}/ld\"")

target_linker_script(${COMPONENT_LIB} INTERFACE "ld/ulp_riscv.ld.in" MEMORY
                     FLAGS "${ulp_ld_flags}")
target_linker_script(${COMPONENT_LIB} INTERFACE "ld/${target}.peripherals.ld")

target_link_options(${COMPONENT_LIB} INTERFACE
    -nostartfiles
    -Wl,--gc-sections
    -Wl,--no-warn-rwx-segments
    LINKER:-u,main)

target_compile_definitions(${COMPONENT_LIB} PUBLIC
    IS_ULP_COCPU
    ULP_RISCV_REGISTER_OPS)
