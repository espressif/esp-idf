Bluetooth\ :sup:`®` Low Energy
====================================

:link_to_translation:`zh_CN:[中文]`

**Bluetooth Low Energy (Bluetooth LE)** provides low-power wireless communication for IoT devices such as wearables, sensors, and smart home products. This section documents the **Bluetooth LE API Reference**, covering APIs for device discovery, data exchange, and Wi-Fi provisioning over Bluetooth LE.

For usage concepts and tutorials, see :doc:`Bluetooth Low Energy <../../../api-guides/ble/index>` in **API Guides**.

The Bluetooth LE API in ESP-IDF is organized into the following parts:

- :doc:`Bluetooth Low Energy GAP <esp_gap_ble>`

  Handles advertising, scanning, connection management, and security operations

- :doc:`Bluetooth Low Energy GATT Define <esp_gatt_defs>`

  Common data types and constants for attributes, characteristics, and UUIDs used in GATT operations

- :doc:`Bluetooth Low Energy GATT Server <esp_gatts>`

  Exposes services and characteristics to remote clients (peripheral role)

- :doc:`Bluetooth Low Energy GATT Client <esp_gattc>`

  Discovers and accesses services on remote servers (central role)

.. only:: SOC_BLUFI_SUPPORTED

   - :doc:`Bluetooth Low Energy BluFi <esp_blufi>`

     Enables Wi-Fi provisioning and configuration via Bluetooth LE

Each part typically includes an **Overview**, **Application Examples**, and **API Reference**, covering purpose, main functionality, sample usage, and detailed API documentation.

.. toctree::
   :maxdepth: 1
   :hidden:

   Bluetooth Low Energy GAP <esp_gap_ble>
   Bluetooth Low Energy GATT Define <esp_gatt_defs>
   Bluetooth Low Energy GATT Server <esp_gatts>
   Bluetooth Low Energy GATT Client <esp_gattc>
   :SOC_BLUFI_SUPPORTED: Bluetooth Low Energy BluFi <esp_blufi>
