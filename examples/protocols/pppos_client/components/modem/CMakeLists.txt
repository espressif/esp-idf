set(COMPONENT_ADD_INCLUDEDIRS .)

set(COMPONENT_SRCS  "src/esp_modem.c"
                    "src/esp_modem_dce_service"
                    "src/sim800.c"
                    "src/bg96.c")

set(COMPONENT_ADD_INCLUDEDIRS "include")

set(COMPONENT_REQUIRES driver)

register_component()
