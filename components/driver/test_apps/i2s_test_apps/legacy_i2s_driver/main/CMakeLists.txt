set(srcs "test_app_main.c"
         "test_legacy_i2s.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity esp_driver_pcnt driver
                       WHOLE_ARCHIVE)
