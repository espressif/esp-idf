set(srcs "main.c")
if(CONFIG_ENABLE_DTM_CONFIGURATION_COMMAND)
list(APPEND srcs
    "dtm_configuration_command.c")
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES bt nvs_flash esp_driver_uart console
                    INCLUDE_DIRS ".")
