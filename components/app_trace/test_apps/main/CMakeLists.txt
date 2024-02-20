idf_component_register(SRCS "test_app_trace_main.c" "test_trace.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES app_trace unity esp_driver_gptimer
                    WHOLE_ARCHIVE)
