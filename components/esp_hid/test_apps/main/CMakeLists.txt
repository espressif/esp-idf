idf_component_register(SRCS "test_esp_hid_main.c"
                            "test_esp_hid.c"
                       PRIV_REQUIRES unity esp_hid
                       WHOLE_ARCHIVE)
