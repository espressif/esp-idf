================
ESP32-S2-Saola-1
================

:link_to_translation:`zh_CN:[中文]`

This user guide provides information on ESP32-S2-Saola-1, a small-sized `ESP32-S2 <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf>`_ based development board produced by Espressif.

.. figure:: ../../../_static/esp32-s2-saola-1-v1.2-isometric.png
    :align: center
    :alt: ESP32-S2-Saola-1
    :figclass: align-center

    ESP32-S2-Saola-1

The document consists of the following major sections:

- `Getting started`_: Provides an overview of the ESP32-S2-Saola-1 and hardware/software setup instructions to get started.
- `Hardware reference`_: Provides more detailed information about the ESP32-S2-Saola-1's hardware.
- `Related Documents`_: Gives links to related documentation.

Getting Started
===============

This section describes how to get started with ESP32-S2-Saola-1. It begins with a few introductory sections about the ESP32-S2-Saola-1, then Section `Start Application Development`_ provides instructions on how to get the ESP32-S2-Saola-1 ready and flash firmware into it.

Overview
--------

ESP32-S2-Saola-1 is a small-sized ESP32-S2 based development board produced by Espressif. Most of the I/O pins are broken out to the pin headers on both sides for easy interfacing. Developers can either connect peripherals with jumper wires or mount ESP32-S2-Saola-1 on a breadboard.

To cover a wide range of users' needs, ESP32-S2-Saola-1 supports:

- `ESP32-S2-WROVER <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_en.pdf>`_
- `ESP32-S2-WROVER-I <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_en.pdf>`_
- `ESP32-S2-WROOM <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_en.pdf>`_
- `ESP32-S2-WROOM-I <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_en.pdf>`_

In this guide, we take ESP32-S2-Saola-1 equipped with ESP32-S2-WROVER as an example.

Contents and Packaging
----------------------

Retail orders
^^^^^^^^^^^^^

If you order a few samples, each ESP32-S2-Saola-1 comes in an individual package in either antistatic bag or any packaging depending on your retailer.

For retail orders, please go to https://www.espressif.com/en/company/contact/buy-a-sample.


Wholesale Orders
^^^^^^^^^^^^^^^^

If you order in bulk, the boards come in large cardboard boxes.

For wholesale orders, please check `Espressif Product Ordering Information <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_en.pdf>`_ (PDF)

Description of Components
-------------------------

.. figure:: ../../../_static/esp32-s2-saola-1-v1.2-annotated-photo.png
    :align: center
    :alt: ESP32-S2-Saola-1 - front/back
    :figclass: align-center

    ESP32-S2-Saola-1 - front

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - ESP32-S2-WROVER
     - ESP32-S2-WROVER is a powerful, generic Wi-Fi MCU module that integrates ESP32-S2. It has a PCB antenna, a 4 MB external SPI flash and an additional 2 MB PSRAM.
   * - I/O Connector
     - All available GPIO pins (except for the SPI bus for flash and PSRAM) are broken out to the pin headers on the board. Users can program ESP32-S2 chip to enable multiple functions such as SPI, I2S, UART, I2C, touch sensors, PWM etc.
   * - USB to UART Bridge
     - Single USB-UART bridge chip provides transfer rates up to 3 Mbps.
   * - Boot Button
     - Download button. Holding down Boot and then pressing Reset initiates Firmware Download mode for downloading firmware through the serial port.
   * - Reset Button
     - Reset button.
   * - 3.3 V Power On LED
     - Turns on when the USB power is connected to the board.
   * - Micro-USB Port
     - USB interface. Power supply for the board as well as the communication interface between a computer and the ESP32-S2 chip.
   * - RGB LED
     - Addressable RGB LED (WS2812), driven by GPIO18.


Start Application Development
-----------------------------

Before powering up your ESP32-S2-Saola-1, please make sure that it is in good condition with no obvious signs of damage.

Required Hardware
^^^^^^^^^^^^^^^^^

- ESP32-S2-Saola-1
- USB 2.0 cable (Standard-A to Micro-B)
- Computer running Windows, Linux, or macOS

Software Setup
^^^^^^^^^^^^^^

Please proceed to :doc:`../../get-started/index`, where Section :ref:`get-started-step-by-step` will quickly help you set up the development environment and then flash an application example into your ESP32-S2-Saola-1.

.. note::
    
    ESP32-S2 only supports ESP-IDF master or version v4.2 and higher.

Hardware Reference
==================

Block Diagram
-------------

A block diagram below shows the components of ESP32-S2-Saola-1 and their interconnections.

.. figure:: ../../../_static/esp32-s2-saola-1-v1.2-block-diags.png
    :align: center
    :scale: 70%
    :alt: ESP32-S2-Saola-1 (click to enlarge)
    :figclass: align-center

    ESP32-S2-Saola-1 (click to enlarge)

Power Supply Options
^^^^^^^^^^^^^^^^^^^^

There are three mutually exclusive ways to provide power to the board:

- Micro USB port, default power supply
- 5V and GND header pins
- 3V3 and GND header pins

Related Documents
=================
* `ESP32-S2-Saola-1 Schematics <https://dl.espressif.com/dl/schematics/ESP32-S2-SAOLA-1_V1.1_schematics.pdf>`_ (PDF)
* `ESP32-S2 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s2_datasheet_en.pdf>`_ (PDF)
* `ESP32-S2-WROVER & ESP32-S2-WROVER-I Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_en.pdf>`_ (PDF)
* `ESP32-S2-WROOM & ESP32-S2-WROOM-I Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wroom_esp32-s2-wroom-i_datasheet_en.pdf>`_ (PDF)
* `Espressif Product Ordering Information <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_en.pdf>`_ (PDF) 
