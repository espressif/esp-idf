低功耗蓝牙\ :sup:`®`
========================

:link_to_translation:`en:[English]`

**低功耗蓝牙** 提供适合物联网应用的低功耗无线通信，常用于可穿戴设备、传感器及智能家居等场景。本章节介绍 **低功耗蓝牙 API 参考**，涵盖用于设备发现、数据交换以及通过低功耗蓝牙进行 Wi-Fi 配网的 API。

如需了解使用概念和开发教程，请参阅 **API 指南** 中的 :doc:`低功耗蓝牙 <../../../api-guides/ble/index>` 章节。

ESP-IDF 中的低功耗蓝牙 API 包括以下部分：

- :doc:`Bluetooth Low Energy GAP <esp_gap_ble>`

  用于设备广播、扫描、连接管理及安全操作

- :doc:`Bluetooth Low Energy GATT Define <esp_gatt_defs>`

  定义 GATT 操作中使用的属性、特征、UUID 及相关常量和数据类型

- :doc:`Bluetooth Low Energy GATT Server <esp_gatts>`

  向远程客户端提供服务和特征（外围设备角色）

- :doc:`Bluetooth Low Energy GATT Client <esp_gattc>`

  发现并访问远程服务器的服务（中心设备角色）

.. only:: SOC_BLUFI_SUPPORTED

   - :doc:`Bluetooth Low Energy BluFi <esp_blufi>`

     通过低功耗蓝牙实现 Wi-Fi 配网和配置

每个部分通常包含 **概述**、**应用示例** 和 **API 参考**，涵盖用途、主要功能、示例用法及详细 API 文档。

.. toctree::
   :maxdepth: 1
   :hidden:

   Bluetooth Low Energy GAP <esp_gap_ble>
   Bluetooth Low Energy GATT Define <esp_gatt_defs>
   Bluetooth Low Energy GATT Server <esp_gatts>
   Bluetooth Low Energy GATT Client <esp_gattc>
   :SOC_BLUFI_SUPPORTED: Bluetooth Low Energy BluFi <esp_blufi>
