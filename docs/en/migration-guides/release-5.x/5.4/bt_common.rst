.. only:: esp32

    - :component_file:`/bt/include/esp32/include/esp_bt.h`

.. only:: esp32c3 or esp32s3

    - :component_file:`/bt/include/esp32c3/include/esp_bt.h`

        - Move the declarations of ``esp_wifi_bt_power_domain_on`` and ``esp_wifi_bt_power_domain_off`` from ``esp_bt.h`` to ``esp_phy_init.h``, since they belong to component ``esp_phy`` and are not expected to be used by customer.
