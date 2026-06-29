idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "")
set(includes "include")

# Add target-specific include directory if it exists
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

# TODO(IDF-15950): potentially remove __ULP_BUILD handling from esp_hal_regi2c
if(__ULP_BUILD)
    # ULP ADC support reaches REGI2C headers through ADC HAL includes used by
    # ulp/ulp_riscv/ulp_core/ulp_riscv_adc.c.
    idf_component_register(INCLUDE_DIRS ${includes}
                           REQUIRES soc hal esp_rom)
    return()
endif()

# Per-target regi2c implementation sources (replaces ROM implementation when needed)
if(NOT CONFIG_ESP_ROM_HAS_REGI2C_IMPL)
    list(APPEND srcs "${target}/regi2c_impl.c")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES soc hal
    LDFRAGMENTS linker.lf
)

# Link the ROM REGI2C HAL implementation linker script on specific targets
if(CONFIG_ESP_ROM_HAS_REGI2C_IMPL)
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/rom.regi2c.ld")
endif()
