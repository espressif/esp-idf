set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS src/port/esp32 src/util)
set(COMPONENT_SRCDIRS src src/util)

set(COMPONENT_REQUIRES nghttp)  # for http_parser.h
set(COMPONENT_PRIV_REQUIRES lwip)

register_component()
