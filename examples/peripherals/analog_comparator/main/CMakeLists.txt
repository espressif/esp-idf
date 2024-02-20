set(src "ana_cmpr_example_main.c")

if(CONFIG_EXAMPLE_USE_ETM)
    list(APPEND src "ana_cmpr_example_etm.c")
else()
    list(APPEND src "ana_cmpr_example_intr.c")
endif()

idf_component_register(SRCS ${src}
                    PRIV_REQUIRES esp_driver_ana_cmpr esp_driver_gpio
                    INCLUDE_DIRS ".")
