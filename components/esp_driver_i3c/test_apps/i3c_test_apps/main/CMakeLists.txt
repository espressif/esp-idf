set(srcs "test_app_main.c"
         "test_i3c_master_common.c"
    )

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity test_utils esp_driver_i3c
                       WHOLE_ARCHIVE)
