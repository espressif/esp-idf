set(srcs "test_app_main.c"
         "test_legacy_rmt.c")

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity driver infrared_tools
                       WHOLE_ARCHIVE)
