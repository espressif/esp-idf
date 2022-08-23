set(srcs "test_app_main.c"
         "test_i2s_adc.c"
         "test_i2s_dac.c")

idf_component_register(SRCS ${srcs}
                       WHOLE_ARCHIVE)
