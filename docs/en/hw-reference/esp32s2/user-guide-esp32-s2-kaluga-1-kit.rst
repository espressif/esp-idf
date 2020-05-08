=====================
ESP32-S2-Kaluga-1 Kit
=====================

:link_to_translation:`zh_CN:[中文]`

The ESP32-S2-Kaluga-1 kit is a development kit by Espressif that is mainly created to:

- Demonstrate the ESP32-S2's human-computer interaction functionalities
- Provide the users with the tools for development of human-computer interaction applications based on the ESP32-S2

There are many ways of how the ESP32-S2's abundant functionalities can be used. For starters, the possible use cases may include:

- **Smart home**: From simplest smart lighting, smart door locks, smart sockets, to video streaming devices, security cameras, OTT devices, and home appliances
- **Battery-powered equipment**: Wi-Fi mesh sensor networks, Wi-Fi-networked toys, wearable devices, health management equipment
- **Industrial automation equipment**: Wireless control and robot technology, intelligent lighting, HVAC control equipment, etc.
- **Retail and catering industry**: POS machines and service robots


.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-kit-v1.0-3d.png
    :align: center
    :scale: 90%
    :alt: ESP32-S2-Kaluga-1-Kit-Assembly
    :figclass: align-center

    ESP32-S2-Kaluga-1-Kit Overview (click to enlarge)


The ESP32-S2-Kaluga-1 kit consists of the following boards:

- Main board: *ESP32-S2-Kaluga-1*
- Extension boards:

  - :doc:`ESP-LyraT-8311A <user-guide-esp-lyrat-8311a_v1.2>` audio player
  - :doc:`ESP-LyraP-TouchA <user-guide-esp-lyrap-toucha-v1.1>` touch panel
  - :doc:`ESP-LyraP-LCD32 <user-guide-esp-lyrap-lcd32-v1.1>` 3.2" LCD screen
  - :doc:`ESP-LyraP-CAM <user-guide-esp-lyrap-cam-v1.0>` camera board

This document is **mostly dedicated to the main board** and its interaction with the extension boards. For more detailed information on each extension board, click their respective links.

This guide covers:

- `Getting Started`_: Provides an overview of the ESP32-S2-Kaluga-1 and hardware/software setup instructions to get started.
- `Hardware reference`_: Provides more detailed information about the ESP32-S2-Kaluga-1's hardware.
- `Hardware Revision Details`_: Covers revision history, known issues, and links to user guides for previous versions of the ESP32-S2-Kaluga-1.
- `Related Documents`_: Gives links to related documentation.


Getting Started
===============


This section describes how to get started with the ESP32-S2-Kaluga-1. It begins with a few introductory sections about the ESP32-S2-Kaluga-1, then Section `Start Application Development`_ provides instructions on how to do the initial hardware setup and then how to flash firmware onto the ESP32-S2-Kaluga-1.


Overview
--------

The ESP32-S2-Kaluga-1 main board is the heart of the kit. It integrates the ESP32-S2-WROVER module and all the connectors for extension boards. This board is the key tool in prototyping human-computer interaction interfaces.

The ESP32-S2-Kaluga-1 board has connectors for:

- Boards with the Extension Header (ESP-LyraT-8311A, ESP-LyraP-LCD32)
- Camera board (ESP-LyraP-CAM)
- Touch panel (ESP-LyraP-TouchA)
- LCD displays (no official extension boards yet)
- I2C devices (no official extension boards yet)

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-v1.2-3d.png
    :align: center
    :scale: 90%
    :alt: ESP32-S2-Kaluga-1
    :figclass: align-center

    ESP32-S2-Kaluga-1 (click to enlarge)

All the four extension boards are specially desgined to support the following features:

* Touch panel control
    * 14 touch sensors, three of which support distance sensing (proximity mode)
    * Supports acrylic panels up to 5 mm
    * Wet hand operation
    * Water rejection, ESP32-S2 can be configured to disable all touchpads automatically if multiple pads are simultaneously covered with water and to re-enable touchpads if the water is removed
 
* Audio playback
    * Connect speakers to play audio
    * Use together with the Touch panel to control audio playback and adjust volume
 
* LCD display
    * LCD interface (8-bit parallel RGB, 8080, and 6800 interface)
 
* Camera image acquisition
    * Supports OV2640 and OV3660 camera modules
    * 8-bit DVP image sensor interface (ESP32-S2 also supports 16-bit DVP image sensors, you can design it yourself)
    * Clock frequency up to 40 MHz
    * Optimized DMA transmission bandwidth for easier transmission of high-resolution images


Description of Components
-------------------------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-v1.2-layout-front.png
    :align: center
    :scale: 90%
    :alt: ESP32-S2-Kaluga-1 - front
    :figclass: align-center

    ESP32-S2-Kaluga-1 - front (click to enlarge)


The description of components starts from the ESP32-S2 module on the left side and then goes clockwise.

**Reserved** means that the functionality is available, but the current version of the kit does not use it.


.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - ESP32-S2-WROVER Module
     - Module integrating the ESP32-S2 chip that provides Wi-Fi connectivity, data processing power, and flexible data storage
   * - 4.3" LCD FPC Connector
     - (Reserved) Connect to a 4.3" LCD extension board using the FPC cable
   * - ESP Prog Connector
     - (Reserved) Connection for Espressif's download device (ESP-Prog) to flash ESP32-S2 system
   * - JTAG Switch
     - Switch to ON to enable connection between ESP32-S2 and FT2232
   * - Breakout Header 2
     - Some GPIO pins of the ESP32-S2-WROVER module are broken out to this header, see labels on the board
   * - USB-to-UART/JTAG Bridge
     - FT2232 adapter board allowing for communication over USB port using UART/JTAG protocols
   * - Camera Header
     - Mount a camera extension board here (e.g., ESP-LyraP-CAM)
   * - Extension Header
     - Mount the extension boards having such connectors here
   * - Reset Button
     - Press this button to restart the system
   * - Boot Button
     - Holding down **Boot** and then pressing **Reset** initiates Firmware Download mode for downloading firmware through the serial port
   * - USB-UART/JTAG Port
     - Communication interface (UART or JTAG) between a PC and the ESP32-S2 module
   * - USB Power Port
     - Power supply for the board
   * - Battery Port
     - Connect an external battery to the 2-pin connector
   * - Power Switch
     - Switch to ON to power the system
   * - RGB Jumper
     - To have access to the RGB LED, place a jumper onto the pins
   * - RGB LED
     - Programmable RGB LED and controlled by GPIO45. Before using it, you need to put RGB Jumper ON.
   * - Power Regulator
     - Regulator converts 5 V to 3.3 V
   * - I2C FPC Connector
     - (Reserved) Connect to other I2C extension boards using the FPC cable
   * - Breakout Header 1
     - Some GPIO pins of the ESP32-S2-WROVER module are broken out to this header, see labels on the board
   * - Touch FPC Connector
     - Connect the ESP-LyraP-TouchA extension board using the FPC cable
   * - Touch Switch
     - In OFF position, GPIO1 to GPIO14 are used for connection to touch sensors; switch to ON if you want to use them for other purposes
   * - 3.2" LCD FPC connector
     - Connect a 3.2" LCD extension board (e.g., ESP-LyraP-LCD32) using the FPC cable


Start Application Development
-----------------------------

Before powering up your ESP32-S2-Kaluga-1, please make sure that it is in good condition with no obvious signs of damage.


Required Hardware
^^^^^^^^^^^^^^^^^

- ESP32-S2-Kaluga-1
- Two USB 2.0 cables (Standard-A to Micro-B)

  - For power supply
  - For UART/JTAG communication

- Computer running Windows, Linux, or macOS
- Any extension boards of your choice


Hardware Setup
^^^^^^^^^^^^^^

1. Connect the extension board(s) of your choice (go to their respective user guides if necessary)
2. Plug in both USB cables
3. Turn the **Power Switch** to ON - the Power On LED will light up


.. _user-guide-esp32-s2-kaluga-1-kit-software-setup:

Software Setup
^^^^^^^^^^^^^^

Please proceed to :doc:`../../get-started/index`, where Section :ref:`get-started-step-by-step` will quickly help you set up the development environment.

The programming guide and application examples for your ESP32-S2-Kaluga-1 kit can be found `here <https://github.com/espressif/esp-dev-kits/tree/master/esp32-s2-kaluga-1>`_.


Contents and Packaging
----------------------

Retail orders
^^^^^^^^^^^^^

If you order one or several samples of the kit, each ESP32-S2-Kaluga-1 development kit comes in an individual package containing:

- Main Board
    - ESP32-S2-Kaluga-1
- Extension Boards:
    - ESP-LyraT-8311A
    - ESP-LyraP-CAM
    - ESP-LyraP-TouchA
    - ESP-LyraP-LCD32
- Connectors
    - 20-pin FPC cable (to connect ESP32-S2-Kaluga-1 to ESP-LyraP-TouchA)
- Fasteners
    - Mounting bolts (x8)
    - Screws (x4)
    - Nuts (x4)

For retail orders, please go to https://www.espressif.com/en/company/contact/buy-a-sample.


Wholesale Orders
^^^^^^^^^^^^^^^^

If you order in bulk, the boards come in large cardboard boxes.

For wholesale orders, please check `Espressif Product Ordering Information <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_en.pdf>`_ (PDF)


Hardware Reference
==================


Block Diagram
-------------

A block diagram below shows the components of the ESP32-S2-Kaluga-1 and their interconnections.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-s2-kaluga-1-v1.2-block-diagram.png
    :align: center
    :alt: ESP32-S2-Kaluga-1 block diagram
    :figclass: align-center

    ESP32-S2-Kaluga-1 block diagram


Power Supply Options
--------------------

There are four ways to provide power to the board:

- Micro USB port, default power supply
- External battery via the 2-pin *BAT* connector
- 5V and GND header pins
- 3V3 and GND header pins


Hardware Revision Details
=========================

No previous versions available.


Related Documents
=================

.. toctree::
    :hidden:

    user-guide-esp-lyrap-cam-v1.0
    user-guide-esp-lyrap-lcd32-v1.1
    user-guide-esp-lyrap-toucha-v1.1
    user-guide-esp-lyrat-8311a_v1.2

- `ESP32-S2-WROVER Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-s2-wrover_esp32-s2-wrover-i_datasheet_en.pdf>`_ (PDF)
- `Espressif Product Ordering Information <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_en.pdf>`_ (PDF)

- `ESP32-S2-Kaluga-1 Schematic <https://dl.espressif.com/dl/schematics/ESP32-S2-Kaluga-1_V1_2_SCH_20200424A.pdf>`_ (PDF)
- `ESP32-S2-Kaluga-1 PCB Layout <https://dl.espressif.com/dl/schematics/ESP32-S2-Kaluga-1_V1_2_PCB_20200325AF.pdf>`_ (PDF)
- `ESP32-S2-Kaluga-1 Pin Mapping <https://dl.espressif.com/dl/schematics/ESP32-S2-Kaluga-1_V1.2_Pin-Mapping.xlsx>`_ (Excel)

For other design documentation for the board, please contact us at sales@espressif.com.