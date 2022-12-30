idf_component_register(SRCS "iperf.c"
                            "wifi_stats.c"
                            "wifi_twt.c"
                            "wifi_cmd.c"
                    INCLUDE_DIRS "include"
                    REQUIRES lwip
                    PRIV_REQUIRES esp_timer)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

if(CONFIG_SOC_WIFI_HE_SUPPORT)
    idf_component_optional_requires(PRIVATE esp_wifi console)
endif()
