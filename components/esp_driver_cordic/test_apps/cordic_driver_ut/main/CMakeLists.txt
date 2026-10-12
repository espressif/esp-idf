set(srcs "test_app_main.c")

if(CONFIG_SOC_CORDIC_SUPPORTED)
    list(APPEND srcs "test_cordic_common.c" "test_cordic_q15.c" "test_cordic_q31.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_cordic esp_driver_gptimer spi_flash
                       WHOLE_ARCHIVE)
