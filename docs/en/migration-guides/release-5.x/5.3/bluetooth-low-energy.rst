Bluetooth Low Energy
====================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid APIs have been removed:

    - :component_file:`bt/host/bluedroid/api/include/api/esp_gap_ble_api.h`

        - Remove ``esp_err_t esp_ble_gap_set_device_name(const char *name)``

            - Local device name setting calls have been moved to :cpp:func:`esp_bt_dev_set_device_name`. This function can be deleted directly.

        - Remove ``esp_err_t esp_ble_gap_get_device_name(void)``

            - Local device name getting calls have been moved to :cpp:func:`esp_bt_dev_get_device_name`. This function can be deleted directly.
