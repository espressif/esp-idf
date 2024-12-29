.. only:: esp32

    - :component_file:`/bt/include/esp32/include/esp_bt.h`

.. only:: esp32c3 or esp32s3

    - :component_file:`/bt/include/esp32c3/include/esp_bt.h`

        - 将 ``esp_wifi_bt_power_domain_on`` 和 ``esp_wifi_bt_power_domain_off`` 的声明从 ``esp_bt.h`` 移至 ``esp_phy_init.h`` ， 因为它们属于组件 ``esp_phy`` 并且不希望被客户使用。
