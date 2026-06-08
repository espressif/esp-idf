idf_component_register(SRCS "test_app_main.c"
                            "test_esp_sys_event.c"
                    PRIV_INCLUDE_DIRS .
                    PRIV_REQUIRES "unity" "esp_system"
                    WHOLE_ARCHIVE)
