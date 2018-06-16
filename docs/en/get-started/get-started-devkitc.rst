ESP32-DevKitC V4 Getting Started Guide
======================================

This user guide shows how to get started with ESP32-DevKitC V4 development board. For description of other versions of the ESP32-DevKitC check :doc:`../hw-reference/index`.


What You Need
-------------

* 1 × :ref:`ESP32-DevKitC V4 board <get-started-esp32-devkitc-board-front>`
* 1 × USB A / micro USB B cable 
* 1 × PC loaded with Windows, Linux or Mac OS


Overview
--------

ESP32-DevKitC V4 is a small-sized ESP32-based development board produced by `Espressif <https://espressif.com>`_. Most of the I/O pins are broken out to the pin headers on both sides for easy interfacing. Developers can connect these pins to peripherals as needed. Standard headers also make development easy and convenient when using a breadboard. 

The board supports various ESP32 modules, including :ref:`esp-modules-and-boards-esp32-wroom-32`, :ref:`ESP32-WROOM-32U <esp-modules-and-boards-esp32-wroom-32d-and-u>`, :ref:`ESP32-WROOM-32D <esp-modules-and-boards-esp32-wroom-32d-and-u>` and :ref:`esp-modules-and-boards-esp32-solo-1`.


Functional Description
----------------------

The following list and figure below describe key components, interfaces and controls of ESP32-DevKitC V4 board.

ESP32-WROOM-32
    :ref:`esp-modules-and-boards-esp32-wroom-32` module soldered to the ESP32-DevKitC V4 board. Optionally ESP32-WROOM-32D, ESP32-WROOM-32U or ESP32-SOLO-1 module may be soldered instead of the ESP32-WROOM-32.
USB-UART Bridge
    A single chip USB-UART bridge provides up to 3 Mbps transfers rates.
Boot
    Download button: holding down the **Boot** button and pressing the **EN** button initiates the firmware download mode. Then user can download firmware through the serial port.
Micro USB Port
    USB interface. It functions as the power supply for the board and the communication interface between PC and the ESP module.
5V Power On LED
    This LED lights when the USB or an external 5V power supply is applied to the board. For details see schematic in `Related Documents`_.
EN
    Reset button: pressing this button resets the system.
I/O
    Most of the pins on the ESP module are broken out to the pin headers on the board. Users can program ESP32 to enable multiple functions such as PWM, ADC, DAC, I2C, I2S, SPI, etc.

    .. note::

        Some of broken out pins are used internally by the ESP32-WROOM-32, ESP32-WROOM-32D/U and ESP32-SOLO-1 modules to communicate with SPI memory. They are grouped on one side of the board besides the USB connector and labeled CLK, D0, D1, D2, D3 and CMD (GPIO6 - GPIO11). In general these pins should be left unconnected, otherwise access to the SPI flash memory / SPI RAM may be disturbed. 

.. _get-started-esp32-devkitc-board-front:

.. figure:: ../../_static/esp32-devkitc-functional-overview.jpg
    :align: center
    :alt: ESP32-DevKitC V4 with ESP32-WROOM-32 module soldered
    :figclass: align-center

    ESP32-DevKitC V4 with ESP32-WROOM-32 module soldered


Power Supply Options
--------------------

There following options are available to provide power supply to this board:

1. Micro USB port, this is default power supply connection
2. 5V / GND header pins
3. 3V3 / GND header pins

.. warning::

    Above options are mutually exclusive, i.e. the power supply may be provided using only one of the above options. Attempt to power the board using more than one connection at a time may damage the board and/or the power supply source.


Note on C15
-----------

The C15, on the board of earlier batches of V4, may bring two issues:

1. The board may boot into download mode;
2. If users output clock on GPIO0, C15 may impact the clock output.

As a result, if users believe that C15 will impact their use of the board, they can remove it completely (please refer to the screenshot below for the precise location of C15 that is colored in yellow). Otherwise, users do not need to concern about C15.

.. figure:: ../../_static/esp32-devkitc-c15-location.png
    :align: center
    :alt: Location of C15 (colored yellow) on ESP32-DevKitC V4 board
    :figclass: align-center
    :width: 30%

    Location of C15 (colored yellow) on ESP32-DevKitC V4 board


Start Application Development
------------------------------

Before powering up the ESP32-DevKitC, please make sure that the board has been received in good condition with no obvious signs of damage.

To start development of applications, proceed to section :doc:`index`, that will walk you through the following steps:

* :ref:`get-started-setup-toolchain` in your PC to develop applications for ESP32 in C language
* :ref:`get-started-connect` the module to the PC and verify if it is accessible
* :ref:`get-started-build-flash` an example application to the ESP32
* :ref:`get-started-build-monitor` instantly what the application is doing


Board Dimensions
----------------

.. figure:: ../../_static/esp32-devkitc-dimensions-back.jpg
    :align: center
    :alt: ESP32 DevKitC board dimensions - back
    :figclass: align-center

    ESP32 DevKitC board dimensions - back


Related Documents
-----------------

* `ESP32-DevKitC V4 schematic <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf>`_ (PDF)
* `ESP32 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-32D/U Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf>`_ (PDF)


.. toctree::
    :hidden:

    get-started-devkitc-v2