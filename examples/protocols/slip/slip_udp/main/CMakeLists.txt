# Slip client example

idf_component_register(
    SRCS "slip_client_main.c"
    INCLUDE_DIRS "."
    REQUIRES esp_netif slip_modem driver
)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
