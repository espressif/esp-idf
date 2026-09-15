加密广播数据 (EAD)
==================

:link_to_translation:`en:[English]`

概述
--------

加密广播数据 (Encrypted Advertising Data，EAD) 引入于蓝牙核心规范 5.4。设备可以使用 AES-CCM 加密一段或多段广播结构，只有持有对应 Session Key 和 IV 的对端才能还原明文。

Bluedroid host 在 ``esp_ble_ead.h`` 中提供一组同步 API。加解密在 host 侧完成，不依赖控制器特性位。

启用 ``CONFIG_BT_BLE_FEAT_ENC_ADV_DATA`` 后会编译这些 API。

GAP Key Material 特征 (UUID 0x2B88，``CONFIG_BT_GATTS_KEY_MATERIAL_CHAR``) 是 Peripheral 发布 Session Key 和 IV 的标准做法。打开该选项会同时选中 EAD 加解密 API。调用 :cpp:func:`esp_ble_gap_set_key_material` 写入特征值后，对端可在加密 GATT 连接上读取，再用 :cpp:func:`esp_ble_ead_decrypt` 解密广播。

若 Central 已持有预共享密钥，只需打开 ``CONFIG_BT_BLE_FEAT_ENC_ADV_DATA``。

应用示例
--------------------

- :example:`bluetooth/bluedroid/ble/ble_enc_adv_data/enc_adv_data_prph` 演示如何加密广播数据，并通过 GAP 服务提供 Key Material。

- :example:`bluetooth/bluedroid/ble/ble_enc_adv_data/enc_adv_data_cent` 演示如何扫描加密广播数据，并在读取或使用 Key Material 后解密。

在 menuconfig 中见 **Bluedroid Options → Encrypted Advertising Data (EAD)**。

API 参考
-------------

.. include-build-file:: inc/esp_ble_ead.inc
