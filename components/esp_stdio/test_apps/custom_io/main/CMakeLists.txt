idf_component_register(SRCS "test_app_main.c"
                            "test_custom_io.c"
                            "mock_vfs.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_stdio vfs
                       WHOLE_ARCHIVE)
