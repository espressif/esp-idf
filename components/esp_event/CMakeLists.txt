set(COMPONENT_SRCS "default_event_loop.c"
                   "esp_event.c"
                   "esp_event_private.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_PRIV_INCLUDEDIRS "private_include")

set(COMPONENT_REQUIRES log)

register_component()
