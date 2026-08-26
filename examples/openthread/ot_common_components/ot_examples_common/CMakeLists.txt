set(srcs "")

if(CONFIG_OPENTHREAD_FTD OR CONFIG_OPENTHREAD_MTD)
    list(APPEND srcs "ot_network.c")
endif()

if(CONFIG_OPENTHREAD_CLI)
    list(APPEND srcs "ot_console.c")
endif()

if(CONFIG_ESP_COEX_EXTERNAL_COEXIST_ENABLE)
    list(APPEND srcs "ot_external_coexist.c")
endif()

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES console cmd_system esp_coex openthread iperf-cmd iperf
)
