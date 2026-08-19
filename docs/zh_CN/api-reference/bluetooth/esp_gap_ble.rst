GAP API
=======

:link_to_translation:`en:[English]`

应用示例
-------------------

- :example:`bluetooth/bluedroid/ble/gatt_security_client` 演示使用 ESP 低功耗蓝牙 security API，{IDF_TARGET_NAME} 作为 GATT 客户端时如何建立安全连接并加密与对等设备的通信。

- :example:`bluetooth/bluedroid/ble/gatt_security_server` 演示使用 ESP 低功耗蓝牙 security API，{IDF_TARGET_NAME} 作为 GATT 服务器时如何建立安全连接并加密与对等设备的通信。

- :example:`bluetooth/bluedroid/ble/ble_enc_adv_data/enc_adv_data_prph` 演示加密广播数据，并通过 :cpp:func:`esp_ble_gap_set_key_material` 发布 Key Material。另见 :doc:`esp_ble_ead`。

API 参考
-------------

.. include-build-file:: inc/esp_gap_ble_api.inc
