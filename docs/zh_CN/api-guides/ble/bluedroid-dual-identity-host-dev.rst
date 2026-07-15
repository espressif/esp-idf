Bluedroid 双本地身份 Host 开发说明
=====================================

:link_to_translation:`en:[English]`

简介
----

当同一部手机通过两个不同的**本地身份**（例如来自两个扩展广播集的 Public 地址与固定的 Static Random 地址）连接到 ESP32 外围设备时，默认 Bluedroid Host 会将两条链路视为同一对端。Bond、LTK 与 NVS 区段可能相互覆盖。

启用 :ref:`BT_BLE_PERIPH_PSEUDO_ADDR_BOND <CONFIG_BT_BLE_PERIPH_PSEUDO_ADDR_BOND>`\ 后，Host 会为每条链路派生内部 **伪地址 (pseudo address)** ``f(local_identity, peer)``。应用层对同一手机会看到两个不同的 ``remote_bda``，而 SMP 与控制器仍使用空口真实对端身份。

示例
----

请参阅 :example:`ble50_dual_identity_server <bluetooth/bluedroid/ble_50/ble50_dual_identity_server>`\ ：该 Bluetooth LE 5.0 外围设备同时广播两个身份、分别配对，并为每个 **(local, peer)** 对保留**独立的 bond**。

应用要点
--------

- 在 GATTS 调用中以 **conn_id** 作为链路键；不要用 ``remote_bda`` 区分链路。
- 启用本特性后，GAP/GATTS 事件中的 ``remote_bda`` 为 **pseudo 地址**。
- 连接态下调用 ``esp_ble_gap_get_conn_identity()`` 可恢复真实对端与本地身份。
- 使用 ``esp_ble_gap_remove_bond_for_identity()`` 只删除一路身份的 bond，不影响另一路。
- 控制器相关操作（白名单、定向广播）须使用**真实对端**地址，切勿使用伪地址。
