===================
ESP32-C3-DevKitC-02
===================

:link_to_translation:`zh_CN:[中文]`

This user guide will help you get started with ESP32-C3-DevKitC-02 and will also provide more in-depth information.

ESP32-C3-DevKitC-02 is an entry-level development board based on `ESP32-C3-WROOM-02 <https://www.espressif.com/sites/default/files/documentation/esp32-c3-wroom-02_datasheet_en.pdf>`_, a general-purpose module with 4 MB SPI flash. This board integrates complete Wi-Fi and Bluetooth® Low Energy functions.

Most of the I/O pins are broken out to the pin headers on both sides for easy interfacing. Developers can either connect peripherals with jumper wires or mount ESP32-C3-DevKitC-02 on a breadboard.

.. figure:: ../../../_static/esp32-c3-devkitc-02-v1-isometric.png
    :align: center
    :alt: ESP32-C3-DevKitC-02
    :figclass: align-center

    ESP32-C3-DevKitC-02

The document consists of the following major sections:

- `Getting Started`_: Overview of ESP32-C3-DevKitC-02 and hardware/software setup instructions to get started.
- `Hardware Reference`_: More detailed information about the ESP32-C3-DevKitC-02's hardware.
- `Hardware Revision Details`_: Revision history, known issues, and links to user guides for previous versions (if any) of ESP32-C3-DevKitC-02.
- `Related Documents`_: Links to related documentation.


Getting Started
===============

This section provides a brief introduction of ESP32-C3-DevKitC-02, instructions on how to do the initial hardware setup and how to flash firmware onto it.


Description of Components
-------------------------

.. _user-guide-c3-devkitc-02-v1-board-front:

.. figure:: ../../../_static/esp32-c3-devkitc-02-v1-annotated-photo.png
    :align: center
    :alt: ESP32-C3-DevKitC-02 - front
    :figclass: align-center

    ESP32-C3-DevKitC-02 - front

The key components of the board are described in a counter-clockwise direction.

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - ESP32-C3-WROOM-02
     - ESP32-C3-WROOM-02 from Espressif is a powerful and general-purpose module that offers Wi-Fi and Bluetooth Low Energy coexistence. It has a PCB antenna and a 4 MB SPI flash.
   * - 5 V to 3.3 V LDO
     - Power regulator that converts a 5 V supply into a 3.3 V output.
   * - 5 V Power On LED
     - Turns on when the USB power is connected to the board.
   * - Pin Headers
     - All available GPIO pins (except for the SPI bus for flash) are broken out to the pin headers on the board. For details, please see :ref:`user-guide-c3-devkitc-02-v1-header-blocks`.
   * - Boot Button
     - Download button. Holding down **Boot** and then pressing **Reset** initiates Firmware Download mode for downloading firmware through the serial port.
   * - Micro-USB Port
     - USB interface. Power supply for the board as well as the communication interface between a computer and the ESP32-C3 chip.
   * - Reset Button
     - Press this button to restart the system.
   * - USB-to-UART Bridge
     - Single USB-to-UART bridge chip provides transfer rates up to 3 Mbps.
   * - RGB LED
     - Addressable RGB LED, driven by GPIO8.


Start Application Development
-----------------------------

Before powering up your ESP32-C3-DevKitC-02, please make sure that it is in good condition with no obvious signs of damage.


Required Hardware
^^^^^^^^^^^^^^^^^

- ESP32-C3-DevKitC-02
- USB 2.0 cable (Standard-A to Micro-B)
- Computer running Windows, Linux, or macOS

.. note::

  Be sure to use an appropriate USB cable. Some cables are for charging only and do not provide the needed data lines nor work for programming the boards.


Software Setup
^^^^^^^^^^^^^^

Please proceed to :doc:`../../get-started/index`, where Section :ref:`get-started-step-by-step` will quickly help you set up the development environment and then flash an application example into your ESP32-C3-DevKitC-02.


Contents and Packaging
----------------------

Retail Orders
^^^^^^^^^^^^^

If you order a few samples, each ESP32-C3-DevKitC-02 comes in an individual package in either antistatic bag or any packaging depending on your retailer.

For retail orders, please go to https://www.espressif.com/en/contact-us/get-samples.


Wholesale Orders
^^^^^^^^^^^^^^^^

If you order in bulk, the boards come in large cardboard boxes.

For wholesale orders, please go to https://www.espressif.com/en/contact-us/sales-questions.


Hardware Reference
==================

Block Diagram
-------------

The block diagram below shows the components of ESP32-C3-DevKitC-02 and their interconnections.

.. figure:: ../../../_static/esp32-c3-devkitc-02-v1-block-diags.png
    :align: center
    :scale: 70%
    :alt: ESP32-C3-DevKitC-02 (click to enlarge)
    :figclass: align-center

    ESP32-C3-DevKitC-02 (click to enlarge)


Power Supply Options
^^^^^^^^^^^^^^^^^^^^

There are three mutually exclusive ways to provide power to the board:

- Micro-USB Port, default power supply
- 5V and GND pin headers
- 3V3 and GND pin headers

It is recommended to use the first option: Micro-USB Port.


.. _user-guide-c3-devkitc-02-v1-header-blocks:

Header Block
------------

The two tables below provide the **Name** and **Function** of the pin headers on both sides of the board (J1 and J3). The pin header names are shown in :ref:`user-guide-c3-devkitc-02-v1-board-front`. The numbering is the same as in the `ESP32-C3-DevKitC-02 Schematic`_ (PDF).


J1
^^^

===  ====  ==========  ===================================
No.  Name  Type [1]_    Function
===  ====  ==========  ===================================
1    G     G           Ground
2    3V3   P           3.3 V power supply
3    3V3   P           3.3 V power supply
4    RST   I           CHIP_PU
5    G     G           Ground
6    4     I/O/T       GPIO4, ADC1_CH4, FSPIHD, MTMS
7    5     I/O/T       GPIO5, ADC2_CH0, FSPIWP, MTDI
8    6     I/O/T       GPIO6, FSPICLK, MTCK
9    7     I/O/T       GPIO7, FSPID, MTDO
10   G     G           Ground
11   8     I/O/T       GPIO8 [2]_, RGB LED
12   9     I/O/T       GPIO9 [2]_
13   5V    P           5 V power supply
14   5V    P           5 V power supply
15   G     G           Ground
===  ====  ==========  ===================================


J3
^^^

===  ====  ==========  ====================================
No.  Name  Type [1]_   Function
===  ====  ==========  ====================================
1    G     G           Ground
2    0     I/O/T       GPIO0, ADC1_CH0, XTAL_32K_P
3    1     I/O/T       GPIO1, ADC1_CH1, XTAL_32K_N
4    2     I/O/T       GPIO2 [2]_, ADC1_CH2, FSPIQ
5    3     I/O/T       GPIO3, ADC1_CH3
6    G     G           Ground
7    10    I/O/T       GPIO10, FSPICS0
8    G     G           Ground
9    RX    I/O/T       GPIO20, U0RXD
10   TX    I/O/T       GPIO21, U0TXD
11   G     G           Ground
12   18    I/O/T       GPIO18
13   19    I/O/T       GPIO19
14   G     G           Ground
15   G     G           Ground
===  ====  ==========  ====================================

.. [1] P: Power supply; I: Input; O: Output; T: High impedance.
.. [2] GPIO2, GPIO8, and GPIO9 are strapping pins of the ESP32-C3 chip. These pins are used to control several chip functions depending on binary voltage values applied to the pins during chip power-up or system reset. For description and application of the strapping pins, please refer to Section Strapping Pins in `ESP32-C3 Datasheet`_.


Pin Layout
^^^^^^^^^^

.. figure:: ../../../_static/esp32-c3-devkitc-02-v1-pinout.png
    :align: center
    :scale: 50%
    :alt: ESP32-C3-DevKitC-02 (click to enlarge)
    :figclass: align-center

    ESP32-C3-DevKitC-02 Pin Layout (click to enlarge)


Hardware Revision Details
=========================

No previous versions available.


Related Documents
=================

* `Build Secure and Cost-effective Connected Devices with ESP32-C3 <http://c3.espressif.com/>`_
* `ESP32-C3 Datasheet`_ (PDF)
* `ESP32-C3-WROOM-02 Datasheet`_ (PDF)
* `ESP32-C3-DevKitC-02 Schematic`_ (PDF)
* `ESP32-C3-DevKitC-02 PCB Layout <https://dl.espressif.com/dl/schematics/PCB_ESP32-C3-DevKitC-02_V1_1_20210121AA.pdf>`_ (PDF)
* `ESP32-C3-DevKitC-02 Dimensions <https://dl.espressif.com/dl/schematics/DXF_ESP32-C3-DevKitC-02_V1_1_20210121AA.pdf>`_ (PDF)
* `ESP32-C3-DevKitC-02 Dimensions source file <https://dl.espressif.com/dl/schematics/DXF_ESP32-C3-DevKitC-02_V1_1_20210121AA.dxf>`_ (DXF) - You can view it with `Autodesk Viewer <https://viewer.autodesk.com/>`_ online

For further design documentation for the board, please contact us at `sales@espressif.com <sales@espressif.com>`_.


.. _ESP32-C3 Datasheet: https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf
.. _ESP32-C3-WROOM-02 Datasheet: https://www.espressif.com/sites/default/files/documentation/esp32-c3-wroom-02_datasheet_en.pdf
.. _ESP32-C3-DevKitC-02 Schematic: https://dl.espressif.com/dl/schematics/SCH_ESP32-C3-DEVKITC-02_V1_1_20210126A.pdf
