idf_component_register(SRCS "cmd_twai_dump.c" "cmd_twai_send.c" "cmd_twai_core.c" "cmd_twai.c" "twai_utils_main.c"
                           "twai_utils_parser.c"
                    REQUIRES esp_driver_twai esp_driver_gpio console
                    INCLUDE_DIRS ".")
