idf_component_register(
    SRC_DIRS  "." "../../../common/zcl_utility/src"
    INCLUDE_DIRS "." "../../../common/zcl_utility/include"
    PRIV_REQUIRES nvs_flash esp_driver_gpio esp_driver_uart ieee802154
)
