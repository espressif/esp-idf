idf_component_register(SRCS "cybergear_example_main.c" "cybergear.c" "cybergear_console.c"
                    REQUIRES console esp_driver_twai
                    INCLUDE_DIRS "."
                    )
