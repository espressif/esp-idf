Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid API have been deprecated:

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_bt_device.h`

        - Deprecate ``esp_err_t esp_bt_dev_set_device_name(const char *name)``

            - Set device name API has been replaced by ``esp_err_t esp_bt_gap_set_device_name(const char *name)`` or ``esp_err_t esp_ble_gap_set_device_name(const char *name)``. The original function has been deprecated.

        - Deprecate ``esp_err_t esp_bt_dev_get_device_name(void)``

            - Get device name API has been replaced by ``esp_err_t esp_bt_gap_get_device_name(void)`` or ``esp_err_t esp_ble_gap_get_device_name(void)``. The original function has been deprecated.
