set(srcs cxx_build_test_main.cpp
         test_soc_reg_macros.cpp
<<<<<<< HEAD
         test_cxx_standard.cpp)
=======
         test_cxx_standard.cpp
         test_sdmmc_sdspi_init.cpp)
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs test_i2c_lcd.cpp)
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver esp_lcd
                       REQUIRES soc)
