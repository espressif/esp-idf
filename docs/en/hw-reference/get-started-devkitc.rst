ESP32-DevKitC V4 Getting Started Guide
======================================

:link_to_translation:`zh_CN:[中文]`

This guide shows how to start using the ESP32-DevKitC V4 development board. For description of other versions of ESP32-DevKitC check :doc:`../hw-reference/index`.


What You Need
-------------

* ESP32-DevKitC V4 board
* USB A / micro USB B cable
* Computer running Windows, Linux, or macOS

You can skip the introduction sections and go directly to Section `Start Application Development`_.


.. _DevKitC-Overview:

Overview
--------

ESP32-DevKitC V4 is a small-sized ESP32-based development board produced by `Espressif <https://espressif.com>`_. Most of the I/O pins are broken out to the pin headers on both sides for easy interfacing. Developers can either connect peripherals with jumper wires or mount ESP32-DevKitC V4 on a breadboard. 

To cover a wide range of user requirements, the following versions of ESP32-DevKitC V4 are available:

- different ESP32 modules

   - :ref:`esp-modules-and-boards-esp32-wroom-32`
   - :ref:`ESP32-WROOM-32D <esp-modules-and-boards-esp32-wroom-32d-and-u>`
   - :ref:`ESP32-WROOM-32U <esp-modules-and-boards-esp32-wroom-32d-and-u>`
   - :ref:`esp-modules-and-boards-esp32-solo-1`
   - :ref:`ESP32-WROVER <esp-modules-and-boards-esp32-wrover>`
   - :ref:`ESP32-WROVER-B <esp-modules-and-boards-esp32-wrover>`
   - :ref:`ESP32-WROVER-I <esp-modules-and-boards-esp32-wrover>`
   - :ref:`ESP32-WROVER-B (IPEX) <esp-modules-and-boards-esp32-wrover>`

- male or female pin headers.

For details please refer to `Espressif Product Ordering Information`_.


Functional Description
----------------------

The following figure and the table below describe the key components, interfaces and controls of the ESP32-DevKitC V4 board.

.. _get-started-esp32-devkitc-board-front:

.. figure:: ../../_static/esp32-devkitc-functional-overview.jpg
    :align: center
    :alt: ESP32-DevKitC V4 with ESP-WROOM-32 module soldered
    :figclass: align-center

    ESP32-DevKitC V4 with ESP32-WROOM-32 module soldered


+--------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Key Component      | Description                                                                                                                                                                          |
+====================+======================================================================================================================================================================================+
| ESP32-WROOM-32     | A module with ESP32 at its core.                                                                                                                                                     |
+--------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| EN                 | Reset button.                                                                                                                                                                        |
+--------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Boot               | Download button. Holding down **Boot** and then pressing **EN** initiates Firmware Download mode for downloading firmware through the serial port.                                   |
+--------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| USB-to-UART Bridge | Single USB-UART bridge chip provides transfer rates of up to 3 Mbps.                                                                                                                 |
+--------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Micro USB Port     | USB interface. Power supply for the board as well as the communication interface between a computer and the ESP32 module.                                                            |
+--------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| 5V Power On LED    | Turns on when the USB or an external 5V power supply is connected to the board. For details see the schematics in `Related Documents`_.                                              |
+--------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| I/O                | Most of the pins on the ESP module are broken out to the pin headers on the board. You can program ESP32 to enable multiple functions such as PWM, ADC, DAC, I2C, I2S, SPI, etc.     |
+--------------------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+

    .. note::

        The pins D0, D1, D2, D3, CMD and CLK are used internally for communication between ESP32 and SPI flash memory. They are grouped on both sides near the USB connector. Avoid using these pins, as it may disrupt access to the SPI flash memory / SPI RAM. 

    .. note::

        The pins GPIO16 and GPIO17 are available for use only on the boards with the modules ESP32-WROOM and ESP32-SOLO-1. The boards with ESP32-WROVER modules have the pins reserved for internal use.


Power Supply Options
--------------------

There are three mutually exclusive ways to provide power to the board:

* Micro USB port, default power supply
* 5V / GND header pins
* 3V3 / GND header pins

.. warning::

    The power supply must be provided using **one and only one of the options above**, otherwise the board and/or the power supply source can be damaged.


Note on C15
-----------

The component C15 may cause the following issues on earlier ESP32-DevKitC V4 boards:

* The board may boot into Download mode
* If you output clock on GPIO0, C15 may impact the signal

In case these issues occur, please remove the component. The figure below shows C15 highlighted in yellow.


.. figure:: ../../_static/esp32-devkitc-c15-location.png
    :align: center
    :alt: Location of C15 (colored yellow) on ESP32-DevKitC V4 board
    :figclass: align-center
    :width: 30%

    Location of C15 (yellow) on ESP32-DevKitC V4 board


Start Application Development
------------------------------

Before powering up your ESP32-DevKitC V4, please make sure that the board is in good condition with no obvious signs of damage.

After that, proceed to :doc:`index`, where Section :ref:`get-started-step-by-step` will quickly help you set up the development environment and then flash an example project onto your board.


Board Dimensions
----------------

.. figure:: ../../_static/esp32-devkitc-dimensions-back.jpg
    :align: center
    :alt: ESP32 DevKitC board dimensions - back
    :figclass: align-center

    ESP32 DevKitC board dimensions - back


Related Documents
-----------------

* `ESP32-DevKitC V4 schematics <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf>`_ (PDF)
* `ESP32 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-32D & ESP32-WROOM-32U Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROVER Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROVER-B Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-wrover-b_datasheet_en.pdf>`_ (PDF)
* `Espressif Product Ordering Information <https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_en.pdf>`_ (PDF) 

.. toctree::
    :hidden:

    get-started-devkitc-v2
