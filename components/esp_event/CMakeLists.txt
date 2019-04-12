set(COMPONENT_SRCS "default_event_loop.c"
                   "esp_event.c"
                   "esp_event_private.c"
                   "event_loop_legacy.c"
                   "event_send.c")

set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "private_include")

set(COMPONENT_REQUIRES log tcpip_adapter)
set(COMPONENT_PRIV_REQUIRES ethernet)
set(COMPONENT_REQUIRES log tcpip_adapter ethernet)

set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

register_component()

if(GCC_NOT_5_2_0 AND CONFIG_EVENT_LOOP_PROFILING)
    # uses C11 atomic feature
    set_source_files_properties(esp_event.c PROPERTIES COMPILE_FLAGS -std=gnu11)
endif()