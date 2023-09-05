ESP32-DevKitC V2 Getting Started Guide
======================================

:link_to_translation:`zh_CN:[中文]`

This guide shows how to start using the ESP32-DevKitC V2 development board.


What You Need
-------------

* :ref:`ESP32-DevKitC V2 board <get-started-esp32-devkitc-v2-board-front-make>`
* USB A/micro USB B cable
* Computer running Windows, Linux, or macOS

You can skip the introduction sections and go directly to Section `Start Application Development`_.


Overview
--------

ESP32-DevKitC V2 is a small-sized ESP32-based development board produced by `Espressif <https://espressif.com>`_. Most of the I/O pins are broken out to the pin headers on both sides for easy interfacing. Developers can either connect peripherals with jumper wires or mount ESP32-DevKitC V4 on a breadboard.


Functional Description
----------------------

The following figure and the table below describe the key components, interfaces and controls of the ESP32-DevKitC V2 board.

.. _get-started-esp32-devkitc-v2-board-front-make:

.. figure:: ../../../_static/esp32-devkitc-v2-functional-overview.png
    :align: center
    :alt: ESP32-DevKitC V2 board layout
    :figclass: align-center

    ESP32-DevKitC V2 board layout


.. list-table::
    :widths: 25 75
    :header-rows: 1

    * - Key Component
      - Description
    * - ESP32-WROOM-32
      - Standard module with ESP32 at its core. For more information, see `ESP32-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf>`_
    * - EN
      - Reset button.
    * - Boot
      - Download button. Holding down **Boot** and then pressing **EN** initiates Firmware Download mode for downloading firmware through the serial port.
    * - Micro USB Port
      - USB interface. Power supply for the board as well as the communication interface between a computer and ESP32-WROOM-32.
    * - I/O
      - Most of the pins on the ESP module are broken out to the pin headers on the board. You can program ESP32 to enable multiple functions such as PWM, ADC, DAC, I2C, I2S, SPI, etc.


Power Supply Options
--------------------

There are three mutually exclusive ways to provide power to the board:

* Micro USB port, default power supply
* 5V and GND header pins
* 3V3 and GND header pins

.. warning::

    The power supply must be provided using **one and only one of the options above**, otherwise the board and/or the power supply source can be damaged.


Start Application Development
------------------------------

Before powering up your ESP32-DevKitC V2, please make sure that the board is in good condition with no obvious signs of damage.

After that, proceed to :doc:`../../get-started/index`, where Section :ref:`get-started-step-by-step` will quickly help you set up the development environment and then flash an example project onto your board.


Related Documents
-----------------

* `ESP32-DevKitC schematics <https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf>`_ (PDF)
* `ESP32 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf>`_ (PDF)
