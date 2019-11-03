set(COMPONENT_SRCDIRS ".")
set(COMPONENT_PRIV_INCLUDEDIRS "../private_include" ".")
set(COMPONENT_PRIV_REQUIRES unity test_utils esp_netif nvs_flash)

register_component()