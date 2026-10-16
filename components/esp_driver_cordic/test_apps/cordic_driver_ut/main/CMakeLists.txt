set(srcs "test_app_main.c")

if(CONFIG_SOC_CORDIC_SUPPORTED)
    list(APPEND srcs "test_cordic_common.cpp" "test_cordic_q15.cpp" "test_cordic_q31.cpp")
    if(CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP)
        list(APPEND srcs "test_cordic_sleep.cpp")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_cordic esp_driver_gptimer spi_flash esp_hw_support
                       WHOLE_ARCHIVE)
