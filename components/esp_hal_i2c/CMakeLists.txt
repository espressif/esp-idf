idf_build_get_property(target IDF_TARGET)

set(includes)

set(srcs)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()
list(APPEND includes "include")

# TODO(IDF-15987): potentially remove __ULP_BUILDV2 handling from esp_hal_i2c
if(__ULP_BUILDV2)
    # ULP I2C code includes hal/i2c_ll.h and hal/i2c_types.h from
    # ulp/ulp_riscv/ulp_core/ulp_riscv_i2c.c and
    # ulp/lp_core/lp_core/lp_core_i2c.c.
    idf_component_register(INCLUDE_DIRS ${includes}
                           REQUIRES soc hal esp_common)
    return()
endif()

set(target_folder "${target}")

# I2C related source files
if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs "i2c_hal.c" "i2c_hal_iram.c" "${target_folder}/i2c_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
