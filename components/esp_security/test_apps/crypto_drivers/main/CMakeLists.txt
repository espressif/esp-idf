set(srcs "test_app_main.c")

if(CONFIG_SOC_HMAC_SUPPORTED)
    list(APPEND srcs "test_hmac.c")
endif()

if(CONFIG_SOC_DIG_SIGN_SUPPORTED)
    list(APPEND srcs "test_ds.c")
endif()

if(CONFIG_SOC_KEY_MANAGER_SUPPORTED)
    list(APPEND srcs "test_key_mgr.c")
endif()

idf_component_register(SRCS ${srcs}
                       REQUIRES unity efuse test_utils spi_flash esp_security
                       WHOLE_ARCHIVE)
