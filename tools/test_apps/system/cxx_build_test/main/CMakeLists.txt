set(srcs cxx_build_test_main.cpp
         test_soc_reg_macros.cpp
         test_cxx_standard.cpp)

if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs test_i2c_lcd.cpp)
endif()

if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs test_i2s.cpp)
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver esp_lcd esp_driver_i2s
                       REQUIRES soc)
