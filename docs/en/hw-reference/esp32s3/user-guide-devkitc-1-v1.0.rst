==================
ESP32-S3-DevKitC-1
==================

:link_to_translation:`zh_CN:[中文]`

The latest version: :doc:`user-guide-devkitc-1`

This user guide will help you get started with ESP32-S3-DevKitC-1 and will also provide more in-depth information.

The ESP32-S3-DevKitC-1 is an entry-level development board equipped with ESP32-S3-WROOM-1, ESP32-S3-WROOM-1U, or ESP32-S3-WROOM-2, a general-purpose Wi-Fi + Bluetooth® Low Energy MCU module that integrates complete Wi-Fi and Bluetooth Low Energy functions.

Most of the I/O pins on the module are broken out to the pin headers on both sides of this board for easy interfacing. Developers can either connect peripherals with jumper wires or mount ESP32-S3-DevKitC-1 on a breadboard.

.. figure:: ../../../_static/esp32-s3-devkitc-1-v1-isometric.png
    :align: center
    :alt: ESP32-S3-DevKitC-1 with ESP32-S3-WROOM-1 Module

    ESP32-S3-DevKitC-1 with ESP32-S3-WROOM-1 Module


The document consists of the following major sections:

- `Getting started`_: Overview of the board and hardware/software setup instructions to get started.
- `Hardware Reference`_: More detailed information about the board's hardware.
- `Hardware Revision Details`_: Revision history, known issues, and links to user guides for previous versions (if any) of the board.
- `Related Documents`_: Links to related documentation.


Getting Started
===============

This section provides a brief introduction of ESP32-S3-DevKitC-1, instructions on how to do the initial hardware setup and how to flash firmware onto it.


Description of Components
-------------------------

.. _user-guide-s3-devkitc-1-v1-board-front:

.. figure:: ../../../_static/ESP32-S3-DevKitC-1_v2-annotated-photo.png
    :align: center
    :alt: ESP32-S3-DevKitC-1 - front

    ESP32-S3-DevKitC-1 - front

The key components of the board are described in a counter-clockwise direction.

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - ESP32-S3-WROOM-1/1U/2
     - ESP32-S3-WROOM-1, ESP32-S3-WROOM-1U, and ESP32-S3-WROOM-2 are powerful, generic Wi-Fi + Bluetooth Low Energy MCU modules that have a rich set of peripherals. They provide acceleration for neural network computing and signal processing workloads. ESP32-S3-WROOM-1 and ESP32-S3-WROOM-2 comes with a PCB antenna. ESP32-S3-WROOM-1U comes with an external antenna connector.
   * - 5 V to 3.3 V LDO
     - Power regulator that converts a 5 V supply into a 3.3 V output.
   * - Pin Headers
     - All available GPIO pins (except for the SPI bus for flash) are broken out to the pin headers on the board for easy interfacing and programming. For details, please see :ref:`user-guide-s3-devkitc-1-v1-header-blocks`.
   * - USB-to-UART Port
     - A Micro-USB port used for power supply to the board, for flashing applications to the chip, as well as for communication with the chip via the on-board USB-to-UART bridge.
   * - Boot Button
     - Download button. Holding down **Boot** and then pressing **Reset** initiates Firmware Download mode for downloading firmware through the serial port.
   * - Reset Button
     - Press this button to restart the system.
   * - ESP32-S3 USB Port
     - ESP32-S3 full-speed USB OTG interface, compliant with the USB 1.1 specification. The interface is used for power supply to the board, for flashing applications to the chip, for communication with the chip using USB 1.1 protocols, as well as for JTAG debugging.
   * - USB-to-UART Bridge
     - Single USB-to-UART bridge chip provides transfer rates up to 3 Mbps.
   * - RGB LED
     - Addressable RGB LED, driven by GPIO48.
   * - 3.3 V Power On LED
     - Turns on when the USB power is connected to the board.

.. note::

    For boards with Octal SPI flash/PSRAM memory embedded ESP32-S3-WROOM-1/1U modules, and boards with ESP32-S3-WROOM-2 modules, the pins GPIO35, GPIO36 and GPIO37 are used for the internal communication between ESP32-S3 and SPI flash/PSRAM memory, thus not available for external use.


Start Application Development
-----------------------------

Before powering up your board, please make sure that it is in good condition with no obvious signs of damage.


Required Hardware
^^^^^^^^^^^^^^^^^

- ESP32-S3-DevKitC-1
- USB 2.0 cable (Standard-A to Micro-B)
- Computer running Windows, Linux, or macOS

.. note::

  Be sure to use an appropriate USB cable. Some cables are for charging only and do not provide the needed data lines nor work for programming the boards.


Hardware Setup
^^^^^^^^^^^^^^

Connect the board with the computer using **USB-to-UART Port**. Connection using **ESP32-S3 USB Port** is not fully implemented in software. In subsequent steps, **USB-to-UART Port** will be used by default.


Software Setup
^^^^^^^^^^^^^^

Please proceed to :doc:`../../get-started/index`, where Section :ref:`get-started-step-by-step` will quickly help you set up the development environment and then flash an application example onto your board.


Contents and Packaging
----------------------

Ordering Information
^^^^^^^^^^^^^^^^^^^^

The development board has a variety of variants to choose from, as shown in the table below.

.. list-table::
   :widths: 28 29 19 17 15
   :header-rows: 1

   * - Ordering Code
     - Module Integrated
     - Flash
     - PSRAM
     - SPI Voltage
   * - ESP32-S3-DevKitC-1-N8
     - ESP32-S3-WROOM-1-N8
     - 8 MB QD
     - —
     - 3.3 V
   * - ESP32-S3-DevKitC-1-N8R2
     - ESP32-S3-WROOM-1-N8R2
     - 8 MB QD
     - 2 MB QD
     - 3.3 V
   * - ESP32-S3-DevKitC-1-N8R8
     - ESP32-S3-WROOM-1-N8R8
     - 8 MB QD
     - 8 MB OT
     - 3.3 V
   * - ESP32-S3-DevKitC-1-N16R8V
     - ESP32-S3-WROOM-2-N16R8V
     - 16 MB OT
     - 8 MB OT
     - 1.8 V
   * - ESP32-S3-DevKitC-1-N32R8V
     - ESP32-S3-WROOM-2-N32R8V
     - 32 MB OT
     - 8 MB OT
     - 1.8 V
   * - ESP32-S3-DevKitC-1U-N8
     - ESP32-S3-WROOM-1U-N8
     - 8 MB QD
     - —
     - 3.3 V
   * - ESP32-S3-DevKitC-1U-N8R2
     - ESP32-S3-WROOM-1U-N8R2
     - 8 MB QD
     - 2 MB QD
     - 3.3 V
   * - ESP32-S3-DevKitC-1U-N8R8
     - ESP32-S3-WROOM-1U-N8R8
     - 8 MB QD
     - 8 MB OT
     - 3.3 V

.. note::

  In the table above, QD stands for Quad SPI and OT stands for Octal SPI.


Retail Orders
^^^^^^^^^^^^^

If you order a few samples, each board comes in an individual package in either antistatic bag or any packaging depending on your retailer.

For retail orders, please go to https://www.espressif.com/en/company/contact/buy-a-sample.


Wholesale Orders
^^^^^^^^^^^^^^^^

If you order in bulk, the boards come in large cardboard boxes.

For wholesale orders, please go to https://www.espressif.com/en/contact-us/sales-questions.


Hardware Reference
==================

Block Diagram
-------------

The block diagram below shows the components of ESP32-S3-DevKitC-1 and their interconnections.

.. figure:: ../../../_static/ESP32-S3-DevKitC-1_v2-SystemBlock.png
    :align: center
    :scale: 70%
    :alt: ESP32-S3-DevKitC-1 (click to enlarge)

    ESP32-S3-DevKitC-1 (click to enlarge)


Power Supply Options
^^^^^^^^^^^^^^^^^^^^

There are three mutually exclusive ways to provide power to the board:

- USB-to-UART Port and ESP32-S3 USB Port (either one or both), default power supply (recommended)
- 5V and G (GND) pins
- 3V3 and G (GND) pins


.. _user-guide-s3-devkitc-1-v1-header-blocks:

Header Block
------------

The two tables below provide the **Name** and **Function** of the pins on both sides of the board (J1 and J3). The pin names are shown in :ref:`user-guide-s3-devkitc-1-v1-board-front`. The numbering is the same as in the `Board Schematic <https://dl.espressif.com/dl/SCH_ESP32-S3-DEVKITC-1_V1_20210312C.pdf>`_ (PDF).


J1
^^^

===  ====  ==========  ===================================
No.  Name  Type [#]_    Function
===  ====  ==========  ===================================
1    3V3   P           3.3 V power supply
2    3V3   P           3.3 V power supply
3    RST   I           EN
4    4     I/O/T       RTC_GPIO4, GPIO4, TOUCH4, ADC1_CH3
5    5     I/O/T       RTC_GPIO5, GPIO5, TOUCH5, ADC1_CH4
6    6     I/O/T       RTC_GPIO6, GPIO6, TOUCH6, ADC1_CH5
7    7     I/O/T       RTC_GPIO7, GPIO7, TOUCH7, ADC1_CH6
8    15    I/O/T       RTC_GPIO15, GPIO15, U0RTS, ADC2_CH4, XTAL_32K_P
9    16    I/O/T       RTC_GPIO16, GPIO16, U0CTS, ADC2_CH5, XTAL_32K_N
10   17    I/O/T       RTC_GPIO17, GPIO17, U1TXD, ADC2_CH6
11   18    I/O/T       RTC_GPIO18, GPIO18, U1RXD, ADC2_CH7, CLK_OUT3
12   8     I/O/T       RTC_GPIO8, GPIO8, TOUCH8, ADC1_CH7, SUBSPICS1
13   3     I/O/T       RTC_GPIO3, GPIO3, TOUCH3, ADC1_CH2
14   46    I/O/T       GPIO46
15   9     I/O/T       RTC_GPIO9, GPIO9, TOUCH9, ADC1_CH8, FSPIHD, SUBSPIHD
16   10    I/O/T       RTC_GPIO10, GPIO10, TOUCH10, ADC1_CH9, FSPICS0, FSPIIO4, SUBSPICS0
17   11    I/O/T       RTC_GPIO11, GPIO11, TOUCH11, ADC2_CH0, FSPID, FSPIIO5, SUBSPID
18   12    I/O/T       RTC_GPIO12, GPIO12, TOUCH12, ADC2_CH1, FSPICLK, FSPIIO6, SUBSPICLK
19   13    I/O/T       RTC_GPIO13, GPIO13, TOUCH13, ADC2_CH2, FSPIQ, FSPIIO7, SUBSPIQ
20   14    I/O/T       RTC_GPIO14, GPIO14, TOUCH14, ADC2_CH3, FSPIWP, FSPIDQS, SUBSPIWP
21   5V    P           5 V power supply
22   G     G           Ground
===  ====  ==========  ===================================


J3
^^^

===  ====  =====  ====================================
No.  Name  Type   Function
===  ====  =====  ====================================
1    G     G      Ground
2    TX    I/O/T  U0TXD, GPIO43, CLK_OUT1
3    RX    I/O/T  U0RXD, GPIO44, CLK_OUT2
4    1     I/O/T  RTC_GPIO1, GPIO1, TOUCH1, ADC1_CH0
5    2     I/O/T  RTC_GPIO2, GPIO2, TOUCH2, ADC1_CH1
6    42    I/O/T  MTMS, GPIO42
7    41    I/O/T  MTDI, GPIO41, CLK_OUT1
8    40    I/O/T  MTDO, GPIO40, CLK_OUT2
9    39    I/O/T  MTCK, GPIO39, CLK_OUT3, SUBSPICS1
10   38    I/O/T  GPIO38, FSPIWP, SUBSPIWP
11   37    I/O/T  SPIDQS, GPIO37, FSPIQ, SUBSPIQ
12   36    I/O/T  SPIIO7, GPIO36, FSPICLK, SUBSPICLK
13   35    I/O/T  SPIIO6, GPIO35, FSPID, SUBSPID
14   0     I/O/T  RTC_GPIO0, GPIO0
15   45    I/O/T  GPIO45
16   48    I/O/T  GPIO48, SPICLK_N, SUBSPICLK_N_DIFF, RGB LED
17   47    I/O/T  GPIO47, SPICLK_P, SUBSPICLK_P_DIFF
18   21    I/O/T  RTC_GPIO21, GPIO21
19   20    I/O/T  RTC_GPIO20, GPIO20, U1CTS, ADC2_CH9, CLK_OUT1, USB_D+
20   19    I/O/T  RTC_GPIO19, GPIO19, U1RTS, ADC2_CH8, CLK_OUT2, USB_D-
21   G     G      Ground
22   G     G      Ground
===  ====  =====  ====================================


.. [#] P: Power supply; I: Input; O: Output; T: High impedance.


For description of function names, please refer to `Chip Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf>`_ (PDF).


Pin Layout
^^^^^^^^^^

.. figure:: ../../../_static/ESP32-S3_DevKitC-1_pinlayout.jpg
    :align: center
    :scale: 50%
    :alt: ESP32-S3-DevKitC-1 (click to enlarge)

    ESP32-S3-DevKitC-1 Pin Layout (click to enlarge)


Hardware Revision Details
=========================

This is the first revision of this board released.


Related Documents
=================

- `ESP32-S3 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf>`_ (PDF)
- `ESP32-S3-WROOM-1 & ESP32-S3-WROOM-1U Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf>`_ (PDF)
- `ESP32-S3-WROOM-2 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-2_datasheet_en.pdf>`_ (PDF)
- `ESP32-S3-DevKitC-1 Schematic <https://dl.espressif.com/dl/SCH_ESP32-S3-DEVKITC-1_V1_20210312C.pdf>`_ (PDF)
- `ESP32-S3-DevKitC-1 PCB layout <https://dl.espressif.com/dl/PCB_ESP32-S3-DevKitC-1_V1_20210312CB.pdf>`_ (PDF)
- `ESP32-S3-DevKitC-1 Dimensions <https://dl.espressif.com/dl/DXF_ESP32-S3-DevKitC-1_V1_20210312CB.pdf>`_ (PDF)
- `ESP32-S3-DevKitC-1 Dimensions source file <https://dl.espressif.com/dl/DXF_ESP32-S3-DevKitC-1_V1_20210312CB.dxf>`_ (DXF) - You can view it with `Autodesk Viewer <https://viewer.autodesk.com/>`_ online

For further design documentation for the board, please contact us at `sales@espressif.com <sales@espressif.com>`_.
