idf_component_register(SRCS "sysview_tracing.c"
                    PRIV_REQUIRES esp_driver_gptimer app_trace
                    INCLUDE_DIRS ".")
