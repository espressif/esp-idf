set(srcs cxx_build_test_main.cpp
         test_soc_reg_macros.cpp
         test_cxx_standard.cpp
         test_sdmmc_sdspi_init.cpp)

if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs test_i2s.cpp)
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_driver_i2s esp_driver_mcpwm esp_driver_sdmmc esp_driver_sdspi)
