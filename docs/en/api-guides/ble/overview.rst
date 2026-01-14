Introduction
=============

:link_to_translation:`zh_CN:[中文]`

This document provides an architecture overview of the Bluetooth Low Energy (Bluetooth LE) stack in ESP-IDF and some quick links to related documents and application examples.

.. only:: esp32

    {IDF_TARGET_NAME} supports Dual-Mode Bluetooth 4.2 and is certified for Dual-Mode Bluetooth 4.2 and Bluetooth LE 5.0.

.. only:: esp32c3 or esp32s3

    {IDF_TARGET_NAME} supports Bluetooth 5.0 (LE) and is certified for Bluetooth LE 5.4.

.. only:: esp32c2 or esp32c6 or esp32h2 or esp32c5 or esp32c61

    {IDF_TARGET_NAME} supports Bluetooth 5.0 (LE) and is certified for Bluetooth LE 5.3.

The Bluetooth LE stack in ESP-IDF is a layered architecture that enables Bluetooth functionality on {IDF_TARGET_NAME} chip series. The figure below shows its architecture.

.. only:: esp32 or esp32s3 or esp32c3 or esp32c6 or esp32c5 or esp32c61

    .. figure:: ../../../_static/bluetooth-architecture.png
        :align: center
        :scale: 90%
        :alt: {IDF_TARGET_NAME} Bluetooth LE Stack Architecture

        {IDF_TARGET_NAME} Bluetooth LE Stack Architecture

.. only:: esp32c2

    .. figure:: ../../../_static/bluetooth-architecture-no-ble-mesh.png
        :align: center
        :scale: 90%
        :alt: {IDF_TARGET_NAME} Bluetooth LE Stack Architecture

        {IDF_TARGET_NAME} Bluetooth LE Stack Architecture

.. only:: esp32h2

    .. figure:: ../../../_static/bluetooth-architecture-no-blufi.png
        :align: center
        :scale: 90%
        :alt: {IDF_TARGET_NAME} Bluetooth LE Stack Architecture

        {IDF_TARGET_NAME} Bluetooth LE Stack Architecture

The table below shows whether the Bluetooth LE modules are supported in a specific chip series.

.. list-table::
    :width: 100%
    :widths: auto
    :header-rows: 1

    * - Chip Series
      - Controller
      - ESP-Bluedroid
      - ESP-NimBLE
      - ESP-BLE-MESH
      - BluFi
    * - ESP32
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-S3
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-C2
      - Y
      - Y
      - Y
      - \–
      - Y
    * - ESP32-C3
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-C5
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-C6
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-C61
      - Y
      - Y
      - Y
      - Y
      - Y
    * - ESP32-H2
      - Y
      - Y
      - Y
      - Y
      - \–

The following sections briefly describe each layer and provide quick links to the related documents and application examples.


ESP Bluetooth Controller
------------------------

At the bottom layer is ESP Bluetooth Controller, which encompasses various modules such as PHY, Baseband, Link Controller, Link Manager, Device Manager, and HCI. It handles hardware interface management and link management. It provides functions in the form of libraries and is accessible through APIs. This layer directly interacts with the hardware and low-level Bluetooth protocols.

- :doc:`API reference <../../api-reference/bluetooth/controller_vhci>`
- :example:`Application examples <bluetooth/hci>`


Hosts
-----

There are two hosts, ESP-Bluedroid and ESP-NimBLE. The major difference between them is as follows:

- Although both support Bluetooth LE, ESP-NimBLE requires less heap and flash size.

.. only:: esp32

  - ESP-Bluedroid supports both Classic Bluetooth and Bluetooth LE, while ESP-NimBLE only supports Bluetooth LE.


ESP-Bluedroid
^^^^^^^^^^^^^

ESP-Bluedroid is a modified version of the native Android Bluetooth stack, Bluedroid. It consists of two layers: the Bluetooth Upper Layer (BTU) and the Bluetooth Transport Controller layer (BTC). The BTU layer is responsible for processing bottom layer Bluetooth protocols such as L2CAP, GATT/ATT, SMP, GAP, and other profiles. The BTU layer provides an interface prefixed with "bta". The BTC layer is mainly responsible for providing a supported interface, prefixed with "esp", to the application layer, processing GATT-based profiles and handling miscellaneous tasks. All the APIs are located in the ESP_API layer. Developers should use the Bluetooth Low Energy APIs prefixed with "esp".

.. only:: esp32

  ESP-Bluedroid for {IDF_TARGET_NAME} supports Classic Bluetooth and Bluetooth LE.

.. only:: not esp32

  ESP-Bluedroid for {IDF_TARGET_NAME} supports Bluetooth LE only. Classic Bluetooth is not supported.

- API references

  - :doc:`../../api-reference/bluetooth/bt_common`
  - :doc:`Bluetooth LE <../../api-reference/bluetooth/bt_le>`

.. only:: esp32

    - :example:`Bluetooth LE 4.2 Application Examples <bluetooth/bluedroid/ble>`

.. only:: not esp32

    - :example:`Bluetooth LE 4.2 Application Examples <bluetooth/bluedroid/ble>`
    - :example:`Bluetooth LE 5.0 Application Examples <bluetooth/bluedroid/ble_50>`

ESP-NimBLE
^^^^^^^^^^

ESP-NimBLE is a host stack built on top of the NimBLE host stack developed by Apache Mynewt. The NimBLE host stack is ported for {IDF_TARGET_NAME} chip series and FreeRTOS. The porting layer is kept clean by maintaining all the existing APIs of NimBLE along with a single ESP-NimBLE API for initialization, making it simpler for the application developers.

ESP-NimBLE supports Bluetooth LE only. Classic Bluetooth is not supported.

- `Apache Mynewt NimBLE User Guide <https://mynewt.apache.org/latest/network/index.html>`__
- API references

  - `NimBLE API references <https://mynewt.apache.org/latest/network/ble_hs/ble_hs.html>`__
  - :doc:`ESP-NimBLE API references for initialization <../../api-reference/bluetooth/nimble/index>`

- :example:`Application examples <bluetooth/nimble>`


Profiles
--------

Above the host stacks are the profile implementations by Espressif and some common profiles. Depending on your configuration, these profiles can run on ESP-Bluedroid or ESP-NimBLE.


.. only:: SOC_BLE_MESH_SUPPORTED

  ESP-BLE-MESH
  ^^^^^^^^^^^^

  Built on top of Zephyr Bluetooth Mesh stack, the ESP-BLE-MESH implementation supports device provisioning and node control. It also supports such node features as Proxy, Relay, Low power and Friend.

  - :doc:`ESP-BLE-MESH documentation <../esp-ble-mesh/ble-mesh-index>`: feature list, get started, architecture, description of application examples, frequently asked questions, etc.
  - :example:`Application examples <bluetooth/esp_ble_mesh>`


.. only:: SOC_BLUFI_SUPPORTED

  BluFi
  ^^^^^

  The BluFi for {IDF_TARGET_NAME} is a Wi-Fi network configuration function via Bluetooth channel. It provides a secure protocol to pass Wi-Fi configuration and credentials to {IDF_TARGET_NAME}. Using this information, {IDF_TARGET_NAME} can then connect to an AP or establish a softAP.

  - :doc:`BluFi documentation <blufi>`
  - :example:`Application examples <bluetooth/blufi>`


Applications
------------

At the uppermost layer are applications. You can build your own applications on top of the ESP-Bluedroid and ESP-NimBLE stacks, leveraging the provided APIs and profiles to create Bluetooth LE-enabled applications tailored to specific use cases.
