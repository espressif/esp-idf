低功耗蓝牙
==========

:link_to_translation:`en:[English]`

Bluedroid
---------

    以下 Bluedroid API 已被移除：

    - :component_file:`bt/host/bluedroid/api/include/api/esp_gap_ble_api.h`

        - 移除 ``esp_err_t esp_ble_gap_set_device_name(const char *name)``

            - 设置本地设备名的调用已经被移到 :cpp:func:`esp_bt_dev_set_device_name` 中。可直接删除该函数。

        - 移除 ``esp_err_t esp_ble_gap_get_device_name(void)``

            - 获取本地设备名的调用已经被移到 :cpp:func:`esp_bt_dev_get_device_name` 中。可直接删除该函数。
