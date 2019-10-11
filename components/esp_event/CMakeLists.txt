idf_component_register(SRCS "default_event_loop.c"
                            "esp_event.c"
                            "esp_event_private.c"
                            "event_loop_legacy.c"
                            "event_send.c"
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "private_include"
                    REQUIRES log tcpip_adapter
                    PRIV_REQUIRES esp_eth
                    LDFRAGMENTS linker.lf)

if(GCC_NOT_5_2_0 AND CONFIG_ESP_EVENT_LOOP_PROFILING)
    # uses C11 atomic feature
    set_source_files_properties(esp_event.c PROPERTIES COMPILE_FLAGS -std=gnu11)
endif()
