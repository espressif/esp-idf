经典蓝牙
=================

:link_to_translation:`en:[English]`

Bluedroid
---------

    以下 Bluedroid API 已被废弃：

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_bt_device.h`

        - 废弃 ``esp_err_t esp_bt_dev_set_device_name(const char *name)``

            - 设置设备名 API 已被替换为 ``esp_err_t esp_bt_gap_set_device_name(const char *name)`` 或 ``esp_err_t esp_ble_gap_set_device_name(const char *name)``。 原来的函数已经被废弃。

        - 废弃 ``esp_err_t esp_bt_dev_get_device_name(void)``

            - 获取设备名 API 已被替换为 ``esp_err_t esp_bt_gap_get_device_name(void)`` 或 ``esp_err_t esp_ble_gap_get_device_name(void)``。 原来的函数已经被废弃。
