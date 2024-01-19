====================
ESP-LyraP-LCD32 v1.1
====================

:link_to_translation:`zh_CN:[中文]`

This user guide provides information on the ESP-LyraP-LCD32 extension board.

This extension board cannot be bought separately and is usually sold together with other Espressif development boards (e.g., ESP32-S2-Kaluga-1), which will be referred to as *main boards* below.

Currently, ESP-LyraP-LCD32 v1.1 is sold as part of the :doc:`user-guide-esp32-s2-kaluga-1-kit-v1.2`.

The ESP-LyraP-LCD32 extends the functionality of your main board by adding an LCD graphic display.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-lcd32-v1.1-3d.png
    :align: center
    :width: 2243px
    :height: 1534px
    :scale: 30%
    :alt: ESP-LyraP-LCD32
    :figclass: align-center

    ESP-LyraP-LCD32 (click to enlarge)

The document consists of the following major sections:

- `Overview`_: Provides an overview and hardware/software setup instructions to get started.
- `Hardware reference`_: Provides more detailed information about the ESP-LyraP-LCD32's hardware.
- `Hardware Revision Details`_: Covers revision history, known issues, and links to user guides for previous versions of the ESP-LyraP-LCD32.
- `Related Documents`_: Gives links to related documentation.


Overview
========

This extension board adds a 3.2" LCD graphic display with the resolution of 320x240. This display is connected to ESP32-S2 over the SPI bus.


Description of Components
-------------------------

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-lcd32-v1.1-layout-front.png
    :align: center
    :width: 934px
    :height: 600px
    :scale: 70%
    :alt: ESP-LyraP-LCD32 - front
    :figclass: align-center

    ESP-LyraP-LCD32 - front (click to enlarge)


In the description of components below, **Reserved** means that the functionality is available, but the current version of the kit does not use it.


.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Key Component
     - Description
   * - Extension Header
     - Male Extension Header for mounting onto a female Extension Header
   * - LCD display
     - This version has a 3.2" 320x240 SPI LCD display module; the display driver/controller is Sitronix ST7789V
   * - Touch Screen Switch
     - No support for touch screens, keep the switches to OFF to make the pins available for other uses
   * - Main Board 3.2" LCD FPC Connector
     - (Reserved) Connect to main board's 3.2" LCD FPC connector
   * - Control Switch
     - Switch to ON to set Reset/Backlight_control/CS to default high or low; switch to OFF to make the pins available for other uses


Start Application Development
-----------------------------

Before powering up your ESP-LyraP-LCD32, please make sure that it is in good condition with no obvious signs of damage.


Required Hardware
^^^^^^^^^^^^^^^^^

- Board with a female Extension Header (e.g., ESP32-S2-Kaluga-1, ESP-LyraT-8311A)
- ESP-LyraP-LCD32 extension board
- Four mounting bolts (for stable mounting)
- Computer running Windows, Linux, or macOS


Hardware Setup
^^^^^^^^^^^^^^

To mount your ESP-LyraP-LCD32 onto the board with a female Extension Header:

1. Install the four mounting bolts onto the board with a female Extension Header
2. Align the ESP-LyraP-LCD32 with the bolts and Extension Header and insert it carefully


Software Setup
^^^^^^^^^^^^^^

See Section :ref:`user-guide-esp32-s2-kaluga-1-kit-v1.2-software-setup` of the ESP32-S2-Kaluga-1 kit user guide.


Hardware Reference
==================

Block Diagram
-------------

A block diagram below shows the components of the ESP-LyraP-LCD32 and their interconnections.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-lyrap-lcd32-v1.1-block-diagram.png
    :align: center
    :alt: ESP-LyraP-LCD32 block diagram
    :figclass: align-center

    ESP-LyraP-LCD32 block diagram


Hardware Revision Details
=========================

No previous versions available.


Related Documents
=================

- `ESP-LyraP-LCD32 Schematic <https://dl.espressif.com/dl/schematics/ESP-LyraP-LCD32_V1_1_SCH_20200324A.pdf>`_ (PDF)
- `ESP-LyraP-LCD32 PCB Layout <https://dl.espressif.com/dl/schematics/ESP-LyraP-LCD32_V1_1_PCB_20200324AC.pdf>`_ (PDF)

For other design documentation for the board, please contact us at sales@espressif.com.
