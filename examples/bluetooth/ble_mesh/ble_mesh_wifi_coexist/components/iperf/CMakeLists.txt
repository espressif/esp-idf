set(COMPONENT_SRCS "cmd_wifi.c"
                   "iperf.c")

set(COMPONENT_ADD_INCLUDEDIRS .)

set(COMPONENT_REQUIRES lwip console)

register_component()
