set(srcs "test_app_main.c"
         "test_legacy_adc.c")

idf_component_register(SRCS ${srcs}
                       WHOLE_ARCHIVE)
