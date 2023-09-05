=================
ESP32-DevKitS(-R)
=================

:link_to_translation:`zh_CN:[中文]`

This user guide provides information on ESP32-DevKitS(-R), an ESP32-based flashing board produced by Espressif.

ESP32-DevKitS(-R) is a combination of two board names: ESP32-DevKitS and ESP32-DevKitS-R. S stands for springs, and R stands for WROVER.

+-----------------+-------------------+
| |ESP32-DevKitS| | |ESP32-DevKitS-R| |
+-----------------+-------------------+
|  ESP32-DevKitS  |  ESP32-DevKitS-R  |
+-----------------+-------------------+

.. |ESP32-DevKitS| image:: ../../../_static/esp32-devkits-v1.1-layout-isometric-raw.png

.. |ESP32-DevKitS-R| image:: ../../../_static/esp32-devkits-r-v1.1-layout-isometric-raw.png

The document consists of the following major sections:

- `Getting Started`_: Provides an overview of ESP32-DevKitS(-R) and hardware/software setup instructions to get started.
- `Hardware Reference`_: Provides more detailed information about ESP32-DevKitS(-R)'s hardware.
- `Related Documents`_: Gives links to related documentation.


Getting Started
===============

This section describes how to get started with ESP32-DevKitS(-R). It begins with a few introductory sections about ESP32-DevKitS(-R), then Section `How to Flash a Board`_ provides instructions on how to mount a module onto ESP32-DevKitS(-R), get it ready, and flash firmware onto it.


Overview
--------

ESP32-DevKitS(-R) is Espressif's flashing board designed specifically for ESP32. It can be used to flash an ESP32 module without soldering the module to the power supply and signal lines. With a module mounted, ESP32-DevKitS(-R) can also be used as a mini development board like ESP32-DevKitC.

ESP32-DevKitS and ESP32-DevKitS-R boards vary only in layout of spring pins to fit the following ESP32 modules.

- ESP32-DevKitS:
   - ESP32-WROOM-32
   - ESP32-WROOM-32D
   - ESP32-WROOM-32U
   - ESP32-SOLO-1
   - ESP32-WROOM-32E
   - ESP32-WROOM-32UE
- ESP32-DevKitS-R:
   - ESP32-WROVER (PCB & IPEX)
   - ESP32-WROVER-B (PCB & IPEX)
   - ESP32-WROVER-E
   - ESP32-WROVER-IE

For information about above modules, please refer to `ESP32 Series Modules <https://www.espressif.com/en/products/modules?id=ESP32>`_.


Description of Components
-------------------------

.. figure:: ../../../_static/esp32-devkits-v1.1-layout-front.png
    :align: center
    :alt: ESP32-DevKitS - front
    :figclass: align-center

    ESP32-DevKitS - front

.. figure:: ../../../_static/esp32-devkits-r-v1.1-layout-front.png
    :align: center
    :alt: ESP32-DevKitS-R - front
    :figclass: align-center

    ESP32-DevKitS-R - front


.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - Spring Pins
     - Click the module in. The pins will fit into the module's castellated holes.
   * - 2.54 mm Female Headers
     - These female headers are connected to pins of the module mounted on this board. For description of female headers, please refer to `Header Blocks`_.
   * - USB-to-UART Bridge
     - Single-chip USB to UART bridge provides transfer rates of up to 3 Mbps.
   * - LDO
     - 5V-to-3.3V low-dropout voltage regulator (LDO).
   * - Micro-USB Connector/Micro USB Port
     - USB interface. Power supply for the board as well as the communication interface between a computer and the board.
   * - EN Button
     - Reset button.
   * - Boot Button
     - Download button. Holding down **Boot** and then pressing **EN** initiates Firmware Download mode for downloading firmware through the serial port.
   * - Power On LED
     - Turns on when the USB or power supply is connected to the board.


How to Flash a Board
--------------------

Before powering up your ESP32-DevKitS(-R), please make sure that it is in good condition with no obvious signs of damage.


Required Hardware
^^^^^^^^^^^^^^^^^

- An ESP32 module of your choice
- USB 2.0 cable (Standard-A to Micro-B)
- Computer running Windows, Linux, or macOS


Hardware Setup
^^^^^^^^^^^^^^

Please mount a module of your choice onto your ESP32-DevKitS(-R) according to the following steps:

- Gently put your module on the ESP32-DevKitS(-R) board. Make sure that castellated holes on your module are aligned with spring pins on the board.
- Press your module down into the board until it clicks.
- Check whether all spring pins are inserted into castellated holes. If there are some misaligned spring pins, place them into castellated holes with tweezers.


Software Setup
^^^^^^^^^^^^^^

Preferred Method
++++++++++++++++
The ESP-IDF development framework provides a preferred way of flashing binaries onto ESP32-DevKitS(-R). Please proceed to :doc:`../../get-started/index`, where Section :ref:`get-started-step-by-step` will quickly help you set up the development environment and then flash an application example onto your ESP32-DevKitS(-R).


Alternative Method
++++++++++++++++++

As an alternative, Windows users can flash binaries using the `Flash Download Tool <https://www.espressif.com/en/support/download/other-tools?keys=flash+download+tools>`_. Just download it, unzip it, and follow the instructions inside the *doc* folder.

    .. note::
         1. To flash binary files, ESP32 should be set to Firmware Download mode. This can be done either by the flash tool automatically, or by holding down the Boot button and tapping the EN button.
         2. After flashing binary files, the Flash Download Tool restarts your ESP32 module and boots the flashed application by default.


Board Dimensions
----------------

.. figure:: ../../../_static/esp32-devkits-v1.1-dimensions-back.png
    :align: center
    :alt: ESP32-DevKitS
    :figclass: align-center

    ESP32-DevKitS board dimensions - back

.. figure:: ../../../_static/esp32-devkits-r-v1.1-dimensions-back.png
    :align: center
    :alt: ESP32-DevKitS-R
    :figclass: align-center

    ESP32-DevKitS-R board dimensions - back


Contents and Packaging
----------------------

Retail Orders
^^^^^^^^^^^^^

If you order a few samples, each ESP32-DevKitS(-R) comes in an individual package in either antistatic bag or any packaging depending on a retailer.

For retail orders, please go to https://www.espressif.com/en/contact-us/get-samples.


Wholesale Orders
^^^^^^^^^^^^^^^^

If you order in bulk, the boards come in large cardboard boxes.

For wholesale orders, please go to https://www.espressif.com/en/contact-us/sales-questions.


Hardware Reference
==================

Block Diagram
-------------

A block diagram below shows the components of ESP32-DevKitS(-R) and their interconnections.

.. figure:: ../../../_static/esp32-devkits-r-v1.1-block_diagram.png
    :align: center
    :scale: 70%
    :alt: ESP32-DevKitS(-R) - block diagram (click to enlarge)
    :figclass: align-center

    ESP32-DevKitS(-R) (click to enlarge)


Power Supply Options
^^^^^^^^^^^^^^^^^^^^

There are three mutually exclusive ways to provide power to the board:

- Micro USB port, default power supply
- 5V and GND header pins
- 3V3 and GND header pins

It is advised to use the first option: micro USB port.


Header Blocks
-------------

=======  ================  ======================================
.        Label             Signal
=======  ================  ======================================
L1       3V3               VDD 3V3
L2       EN                CHIP_PU
L3       VP                SENSOR_VP
L4       VN                SENSOR_VN
L5       34                GPIO34
L6       35                GPIO35
L7       32                GPIO32
L8       33                GPIO33
L9       25                GPIO25
L10      26                GPIO26
L11      27                GPIO27
L12      14                GPIO14
L13      12                GPIO12
L14      GND               GND
L15      13                GPIO13
L16      D2                SD_DATA2
L17      D3                SD_DATA3
L18      CMD               SD_CMD
L19      5V                External 5V
R1       GND               GND
R2       23                GPIO23
R3       22                GPIO22
R4       TX                U0TXD
R5       RX                U0RXD
R6       21                GPIO21
R7       GND               GND
R8       19                GPIO19
R9       18                GPIO18
R10      5                 GPIO5
R11      17                GPIO17
R12      16                GPIO16
R13      4                 GPIO4
R14      0                 GPIO0
R15      2                 GPIO2
R16      15                GPIO15
R17      D1                SD_DATA1
R18      D0                SD_DATA0
R19      CLK               SD_CLK
=======  ================  ======================================

For the image of header blocks, please refer to `Description of Components`_.


Related Documents
=================
- `ESP32-DevKitS(-R) Schematics <https://dl.espressif.com/dl/ESP32_DEVKITS_20190621.pdf>`_ (PDF)
- `ESP32 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_ (PDF)
- `ESP32-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf>`_ (PDF)
- `ESP32-WROOM-32D & ESP32-WROOM-32U Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf>`_ (PDF)
- `ESP32-SOLO-1 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-solo-1_datasheet_en.pdf>`_ (PDF)
- `ESP32-WROVER Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`_ (PDF)
- `ESP32-WROVER-B Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-wrover-b_datasheet_en.pdf>`_ (PDF)
- `ESP Product Selector <https://products.espressif.com/#/product-selector?names=>`_
