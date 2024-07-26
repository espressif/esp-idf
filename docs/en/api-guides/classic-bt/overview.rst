Introduction
=============

:link_to_translation:`zh_CN:[中文]`

This document provides an architecture overview of the Bluetooth Classic stack in ESP-IDF and some quick links to related documents and application examples.

.. only:: esp32

    {IDF_TARGET_NAME} supports Dual-Mode Bluetooth 4.2.

The Bluetooth Classic stack in ESP-IDF is a layered architecture that enables Bluetooth functionality on {IDF_TARGET_NAME} chip series. The table below shows its architecture.

.. only:: esp32

    .. figure:: ../../../_static/classic-bluetooth-architecture.png
        :align: center
        :scale: 90%
        :alt: {IDF_TARGET_NAME} Bluetooth Classic Stack Architecture

        {IDF_TARGET_NAME} Bluetooth Classic Stack Architecture

The table below shows whether the Bluetooth Classic Controller are supported in a specific chip series.

.. list-table::
    :width: 100%
    :widths: auto
    :header-rows: 1

    * - Chip Series
      - Controller
    * - ESP32
      - Y
    * - ESP32-S2
      - \–
    * - ESP32-S3
      - \–
    * - ESP32-C2
      - \–
    * - ESP32-C3
      - \-
    * - ESP32-C6
      - \-
    * - ESP32-H2
      - \-

The following sections briefly describe each layer and provide quick links to the related documents and application examples.


ESP Bluetooth Controller
------------------------

At the bottom layer is ESP Bluetooth Controller, which encompasses various modules such as PHY, Baseband, Link Controller, Link Manager, Device Manager, and HCI. It handles hardware interface management and link management. It provides functions in the form of libraries and is accessible through APIs. This layer directly interacts with the hardware and low-level Bluetooth protocols.

- :doc:`API reference <../../api-reference/bluetooth/controller_vhci>`
- :example:`Application examples <bluetooth/hci/controller_hci_uart_esp32>`


Hosts
-----

There is one host, ESP-Bluedroid, supporting Classic Bluetooth in IDF.


ESP-Bluedroid
^^^^^^^^^^^^^

ESP-Bluedroid is a modified version of the native Android Bluetooth stack, Bluedroid. It consists of two layers: the Bluetooth Upper Layer (BTU) and the Bluetooth Transport Controller layer (BTC). The BTU layer is responsible for processing bottom layer Bluetooth protocols such as L2CAP and other profiles. The BTU layer provides an interface prefixed with "bta". The BTC layer is mainly responsible for providing a supported interface, prefixed with "esp", to the application layer and handling miscellaneous tasks. All the APIs are located in the ESP_API layer. Developers should use the Classic Bluetooth APIs prefixed with "esp".

- API references

  - :doc:`../../api-reference/bluetooth/bt_common`
  - :doc:`../../api-reference/bluetooth/classic_bt`
- :example:`Application examples <bluetooth/bluedroid/classic_bt>`


Applications
------------

At the uppermost layer are applications. You can build your own applications on top of the ESP-Bluedroid stacks, leveraging the provided APIs and profiles to create Bluetooth Classic applications tailored to specific use cases.
