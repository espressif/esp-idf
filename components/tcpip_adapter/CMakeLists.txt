set(COMPONENT_SRCS "event_handlers.c"
                   "tcpip_adapter_lwip.c")
set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES lwip)

register_component()
