# SLIP Modem Component

idf_component_register(
    SRCS "library/slip_modem.c" "library/slip_modem_netif.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_netif driver
)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
