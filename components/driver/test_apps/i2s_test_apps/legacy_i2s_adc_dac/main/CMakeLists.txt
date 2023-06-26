set(srcs "test_app_main.c"
         "test_i2s_adc.c"
         "test_i2s_dac.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity driver esp_event esp_adc
                       WHOLE_ARCHIVE)
