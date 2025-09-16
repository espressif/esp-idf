idf_build_get_property(target IDF_TARGET)

list(APPEND includes "${target}/include")
list(APPEND includes "include")

set(srcs)
set(include "include")
set(target_folder "${target}")

# I2C related source files
if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs "i2c_hal.c" "i2c_hal_iram.c" "${target_folder}/i2c_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
