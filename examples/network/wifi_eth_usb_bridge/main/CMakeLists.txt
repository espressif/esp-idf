if(CONFIG_EXAMPLE_WIFI_CONFIGURATION_MANUAL)
    set(config_method manual_config.c)
else()
    set(config_method provisioning.c scheme_generic_httpd.c)
endif()

idf_component_register(SRCS tusb_ncm_main.c
                            ${config_method}
                       INCLUDE_DIRS "")
