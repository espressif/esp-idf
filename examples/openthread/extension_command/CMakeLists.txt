set(srcs    "src/esp_ot_cli_extension.c")

if(CONFIG_OPENTHREAD_CLI_IPERF)
    list(APPEND srcs   "src/esp_ot_iperf.c")
endif()

if(CONFIG_OPENTHREAD_CLI_TCP_SOCKET)
    list(APPEND srcs   "src/esp_ot_tcp_socket.c")
endif()

if(CONFIG_OPENTHREAD_CLI_UDP_SOCKET)
    list(APPEND srcs   "src/esp_ot_udp_socket.c")
endif()

if(CONFIG_OPENTHREAD_CLI_WIFI)
    list(APPEND srcs   "src/esp_ot_wifi_cmd.c")
endif()

set(include "include")
idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include}"
                    REQUIRES lwip openthread iperf)
