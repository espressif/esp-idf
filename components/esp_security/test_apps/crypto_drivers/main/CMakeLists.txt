set(srcs "test_app_main.c")

if(CONFIG_SOC_HMAC_SUPPORTED)
    list(APPEND srcs "test_hmac.c")
endif()

if(CONFIG_SOC_DIG_SIGN_SUPPORTED)
    list(APPEND srcs "test_ds.c")
endif()

if(CONFIG_ESP_SECURITY_IS_KEY_MANAGER_SUPPORTED)
    list(APPEND srcs "test_key_mgr.c")
endif()

if(CONFIG_SOC_ECC_SUPPORT_CURVE_SM2 AND CONFIG_SOC_ECC_EXTENDED_MODES_SUPPORTED)
    list(APPEND srcs "test_esp_sm2.c")
endif()

idf_component_register(SRCS ${srcs}
                       REQUIRES unity efuse test_utils spi_flash esp_security
                       WHOLE_ARCHIVE)
