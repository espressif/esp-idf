set(srcs "test_app_main.c"
          "test_ds.c"
          "test_hmac.c"
          "test_key_mgr.c")

idf_component_register(SRCS ${srcs}
                       REQUIRES unity efuse test_utils spi_flash esp_security
                       WHOLE_ARCHIVE)
