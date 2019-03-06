ESP32-DevKitC V4 Getting Started Guide (CMake)
==============================================

This user guide shows how to get started with ESP32-DevKitC V4 development board. For description of other versions of the ESP32-DevKitC check :doc:`../hw-reference/index`.


What You Need
-------------

* 1 × :ref:`ESP32-DevKitC V4 board <get-started-esp32-devkitc-board-front-cmake>`
* 1 × USB A / micro USB B cable 
* 1 × PC loaded with Windows, Linux or Mac OS

If you want to start using this board right now, go directly to Section `Start Application Development`_.


Overview
--------

ESP32-DevKitC V4 is a small-sized ESP32-based development board produced by `Espressif <https://espressif.com>`_. Most of the I/O pins are broken out to the pin headers on both sides for easy interfacing. Developers can connect these pins to peripherals as needed. Standard headers also make development easy and convenient when using a breadboard. 

The board supports various ESP32 modules, including :ref:`esp-modules-and-boards-esp32-wroom-32`, :ref:`ESP32-WROOM-32U <esp-modules-and-boards-esp32-wroom-32d-and-u>`, :ref:`ESP32-WROOM-32D <esp-modules-and-boards-esp32-wroom-32d-and-u>` and :ref:`esp-modules-and-boards-esp32-solo-1`.


Functional Description
----------------------

The following list and figure below describe key components, interfaces and controls of ESP32-DevKitC V4 board.

ESP-WROOM-32
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

        Some of broken out pins are used internally be the ESP32 module to communicate with SPI memory. They are grouped on one side of the board besides the USB connector and labeled D0, D1, D2, D3, CMD and CLK. In general these pins should be left unconnected or access to the SPI flash memory / SPI RAM may be disturbed. 

    .. note::

        GPIO16 and 17 are used internally by the ESP32-WROVER module. They are broken out and avialable for use only for boards that have the ESP-WROOM-32 module installed.


.. _get-started-esp32-devkitc-board-front-cmake:

.. figure:: ../../_static/esp32-devkitc-functional-overview.jpg
    :align: center
    :alt: ESP32-DevKitC V4 with ESP-WROOM-32 module soldered
    :figclass: align-center

    ESP32-DevKitC V4 with ESP-WROOM-32 module soldered


Power Supply Options
--------------------

There following options are available to provide power supply to this board:

1. Micro USB port, this is default power supply connection
2. 5V / GND header pins
3. 3V3 / GND header pins

.. warning::

    Above options are mutually exclusive, i.e. the power supply may be provided using only one of the above options. Attempt to power the board using more than one connection at a time may damage the board and/or the power supply source.


Start Application Development
------------------------------

Before powering up the ESP32-DevKitC, please make sure that the board has been received in good condition with no obvious signs of damage.

To start development of applications, proceed to Section :doc:`index`, that will walk you through the :ref:`get-started-step-by-step-cmake`.


Board Dimensions
----------------

.. figure:: ../../_static/esp32-devkitc-dimensions-back.jpg
    :align: center
    :alt: ESP32 DevKitC board dimensions - back
    :figclass: align-center

    ESP32 DevKitC board dimensions - back


Related Documents
-----------------

* `ESP32-DevKitC V4 schematic <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf>`_ (PDF)
* `ESP32 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_ (PDF)
* `ESP-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp-wroom-32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROVER Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`_ (PDF)

.. toctree::
    :hidden:

    get-started-devkitc-v2
