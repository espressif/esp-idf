set(COMPONENT_ADD_INCLUDEDIRS .)

set(COMPONENT_SRCS "sh2lib.c")

set(COMPONENT_REQUIRES nghttp)
set(COMPONENT_PRIV_REQUIRES lwip esp-tls)

register_component()
