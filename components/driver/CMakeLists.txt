idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

# Always compiled source files
set(srcs)

# Always included headers
set(includes "i2c/include"
             "touch_sensor/include"
             "twai/include")

# Always included linker fragments
set(ldfragments "")

# I2C related source files
if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs "i2c/i2c.c")
    list(APPEND ldfragments "i2c/linker.lf")
endif()

# Touch Sensor related source files
if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    if(CONFIG_SOC_TOUCH_SENSOR_VERSION LESS 3)
        list(APPEND srcs "touch_sensor/touch_sensor_common.c"
                        "touch_sensor/${target}/touch_sensor.c")
        list(APPEND includes "touch_sensor/${target}/include")
    endif()
endif()

# TWAI related source files
# TWAIFD is not supported by the legacy driver
if(CONFIG_SOC_TWAI_SUPPORTED AND NOT CONFIG_SOC_TWAI_FD_SUPPORTED)
    list(APPEND srcs "twai/twai.c")
    list(APPEND ldfragments "twai/linker.lf")
endif()

if(BOOTLOADER_BUILD)
    # Bootloader shall NOT depend on the drivers
    idf_component_register()
else()
    # (REQUIRES cannot hide soc headers, since many arguments in the driver headers are chip-dependent)
    idf_component_register(SRCS "${srcs}"
                           INCLUDE_DIRS ${includes}
                           PRIV_REQUIRES esp_timer esp_mm esp_driver_gpio esp_ringbuf esp_pm
                           REQUIRES esp_hal_i2c esp_hal_twai esp_hal_touch_sens
                           LDFRAGMENTS ${ldfragments}
                          )
endif()
