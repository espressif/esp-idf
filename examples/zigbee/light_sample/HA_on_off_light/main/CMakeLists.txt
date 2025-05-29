idf_component_register(
    SRC_DIRS  "." "$ENV{IDF_PATH}/examples/zigbee/common/zcl_utility/src"
    INCLUDE_DIRS "." "$ENV{IDF_PATH}/examples/zigbee/common/zcl_utility/include"
    PRIV_REQUIRES nvs_flash esp_driver_uart ieee802154
)
