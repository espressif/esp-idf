set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS proto-c ../protocomm/proto-c)
set(COMPONENT_SRCS  "src/wifi_config.c"
                    "proto-c/wifi_config.pb-c.c"
                    "proto-c/wifi_constants.pb-c.c")

set(COMPONENT_REQUIRES lwip)
set(COMPONENT_PRIV_REQUIRES protobuf-c protocomm)

register_component()
