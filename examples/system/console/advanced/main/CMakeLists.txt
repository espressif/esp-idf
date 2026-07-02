idf_component_register(SRCS "console_example_main.c" "console_settings.c"
                       PRIV_REQUIRES console  fatfs nvs_flash cmd_system cmd_wifi cmd_nvs esp_stdio
                       INCLUDE_DIRS ".")
