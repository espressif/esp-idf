set(srcs "test_app_main.c")

if(CONFIG_SOC_LP_I2S_SUPPORTED)
    list(APPEND srcs "test_lp_i2s.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_i2s
                       WHOLE_ARCHIVE)
