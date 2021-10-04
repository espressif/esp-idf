set(srcs "esp_ot_cli.c")

if(CONFIG_OPENTHREAD_CLI_ESP_EXTENSION)
    list(APPEND srcs "esp_ot_cli_extension.c" "esp_ot_tcp_socket.c" "esp_ot_udp_socket.c" "esp_ot_iperf.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ".")
