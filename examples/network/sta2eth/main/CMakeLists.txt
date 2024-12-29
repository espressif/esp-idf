if(CONFIG_EXAMPLE_WIFI_CONFIGURATION_MANUAL)
    set(config_method manual_config.c)
else()
    set(config_method provisioning.c scheme_generic_httpd.c)
endif()

if(CONFIG_EXAMPLE_WIRED_INTERFACE_IS_ETHERNET)
    set(wired_iface ethernet_iface.c)
else()
    set(wired_iface usb_ncm_iface.c)
endif()

idf_component_register(SRCS sta2eth_main.c
                            ${wired_iface}
                            ${config_method}
                       PRIV_REQUIRES esp_wifi esp_timer esp_eth esp_http_server
                                     nvs_flash esp_driver_gpio wifi_provisioning
                       INCLUDE_DIRS "")
