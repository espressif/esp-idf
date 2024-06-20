set(srcs cxx_build_test_main.cpp
         test_soc_reg_macros.cpp
         test_cxx_standard.cpp)

if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs test_i2c_lcd.cpp)
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver esp_lcd
                       REQUIRES soc)
