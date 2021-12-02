if(CONFIG_OPENTHREAD_BR_AUTO_START)
    set(srcs    "esp_ot_br.c")
else()
    set(srcs    "esp_ot_br.c"
                "esp_ot_cli_extension.c"
                "esp_br_wifi_cmd.c")
endif()
    idf_component_register(SRCS "${srcs}"
                           INCLUDE_DIRS ".")
