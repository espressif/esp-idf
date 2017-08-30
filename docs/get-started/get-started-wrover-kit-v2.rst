ESP-WROVER-KIT V2 Getting Started Guide
=======================================

This user guide shows how to get started with ESP-WROVER-KIT V2 development board including description of its functionality and configuration options. You can find out what version you have in section :ref:`esp-modules-and-boards-esp-wrover-kit`.

If you like to start using this board right now, go directly to section :ref:`esp-wrover-kit-v2-start-development`.


What You Need
-------------

* 1 × ESP-WROVER-KIT V2 board
* 1 x Micro USB 2.0 Cable, Type A to Micro B
* 1 × PC loaded with Windows, Linux or Mac OS


Overview
^^^^^^^^

The ESP-WROVER-KIT is a development board produced by `Espressif <http://espressif.com>`_ built around ESP32. This board is compatible with ESP32 modules, including the ESP-WROOM-32 and ESP32-WROVER. The ESP-WROVER-KIT features support for an LCD and MicroSD card. The I/O pins have been broken out from the ESP32 module for easy extension. The board carries an advanced multi-protocol USB bridge (the FTDI FT2232HL), enabling developers to use JTAG directly to debug the ESP32 through the USB interface. The development board makes secondary development easy and cost-effective.

.. note::

    ESP-WROVER-KIT V2 integrates the ESP-WROOM-32 module by default.


Functionality Overview
^^^^^^^^^^^^^^^^^^^^^^

Block diagram below presents main components of ESP-WROVER-KIT and interconnections between components.

.. figure:: ../_static/esp32-wrover-kit-block-diagram.png
    :align: center
    :alt: ESP-WROVER-KIT block diagram
    :figclass: align-center

    ESP-WROVER-KIT block diagram


Functional Description
^^^^^^^^^^^^^^^^^^^^^^

The following list and figures below describe key components, interfaces and controls of ESP-WROVER-KIT board.

32.768 kHz
    An external precision 32.768 kHz crystal oscillator provides the chip with a clock of low-power consumption during the Deep-sleep mode.
ESP32 Module
    ESP-WROVER-KIT is compatible with both ESP-WROOM-32 and ESP32-WROVER. The ESP32-WROVER module features all the functions of ESP-WROOM-32 and integrates an external 32-MBit PSRAM for flexible extended storage and data processing capabilities.

    .. note::

        GPIO16 and GPIO17 are used as the CS and clock signal for PSRAM. To ensure reliable performance, the two GPIOs are not broken out.

CTS/RTS
    Serial port flow control signals: the pins are not connected to the circuitry by default. To enable them, respective pins of JP14 must be shorted with jumpers.
UART
    Serial port: the serial TX/RX signals on FT2232HL and ESP32 are broken out to the two sides of JP11. By default, the two signals are connected with jumpers. To use the ESP32 module serial interface only, the jumpers may be removed and the module can be connected to another external serial device.
SPI
    SPI interface: the SPI interface connects to an external flash (PSRAM). To interface another SPI device, an extra CS signal is needed. If an ESP32-WROVER is being used, please note that the electrical level on the flash and SRAM is 1.8V.
JTAG
    JTAG interface: the JTAG signals on FT2232HL and ESP32 are broken out to the two sides of JP8. By default, the two signals are disconnected. To enable JTAG, shorting jumpers are required on the signals.
FT2232
    FT2232 chip is a multi-protocol USB-to-serial bridge. The FT2232 chip features USB-to-UART and USB-to-JTAG functionalities. Users can control and program the FT2232 chip through the USB interface to establish communication with ESP32.

    The embedded FT2232 chip is one of the distinguishing features of the ESP-WROVER-KIT. It enhances users’ convenience in terms of application development and debugging. In addition, uses do not need to buy a JTAG debugger separately, which reduces the development cost, see `ESP-WROVER-KIT V2 schematic`_.
EN
    Reset button: pressing this button resets the system.
Boot
    Download button: holding down the **Boot** button and pressing the **EN** button initiates the firmware download mode. Then user can download firmware through the serial port.
USB
    USB interface. It functions as the power supply for the board and the communication interface between PC and ESP32 module.
Power Select
    Power supply selection interface: the ESP-WROVER-KIT can be powered through the USB interface or the 5V Input interface. The user can select the power supply with a jumper. More details can be found in section :ref:`esp-wrover-kit-v2-setup-options`, jumper header JP7.
Power Key
    Power on/off button: toggling to the right powers the board on; toggling to the left powers the board off.
5V Input
    The 5V power supply interface is used as a backup power supply in case of full-load operation.
LDO
    NCP1117(1A). 5V-to-3.3V LDO. (There is an alternative pin-compatible LDO — LM317DCY, with an output current of up to 1.5A). NCP1117 can provide a maximum current of 1A. The LDO solutions are available with both fixed output voltage and variable output voltage. For details please refer to `ESP-WROVER-KIT V2 schematic`_.
Camera
    Camera interface: a standard OV7670 camera module is supported.
RGB
    Red, green and blue (RGB) light emitting diodes (LEDs), which may be controlled by pulse width modulation (PWM).
I/O
    All the pins on the ESP32 module are led out to the pin headers on the ESPWROVER-KIT. Users can program ESP32 to enable multiple functions such as PWM, ADC, DAC, I2C, I2S, SPI, etc.

Micro SD Card
    Micro SD card slot for data storage: when ESP32 enters the download mode, GPIO2 cannot be held high. However, a pull-up resistor is required on GPIO2 to enable the Micro SD Card. By default, GPIO2 and the pull-up resistor R153 are disconnected. To enable the SD Card, use jumpers on JP1 as shown in section :ref:`esp-wrover-kit-v2-setup-options`.
LCD
    ESP-WROVER-KIT supports mounting and interfacing a 3.2” SPI (standard 4-wire Serial Peripheral Interface) LCD, as shown on figure :ref:`esp-wrover-kit-v2-board-back`.

.. figure:: ../_static/esp-wrover-kit-v2-layout-front.png
    :align: center
    :alt: ESP-WROVER-KIT board layout - front
    :figclass: align-center

    ESP-WROVER-KIT board layout - front

.. _esp-wrover-kit-v2-board-back:

.. figure:: ../_static/esp-wrover-kit-v2-layout-back.png
    :align: center
    :alt: ESP-WROVER-KIT board layout - back
    :figclass: align-center

    ESP-WROVER-KIT board layout - back


.. _esp-wrover-kit-v2-setup-options:

Setup Options
^^^^^^^^^^^^^

There are five jumper headers available to set up the board functionality. Typical options to select from are listed in table below.

+--------+----------------------+-------------------------------------------------+
| Header | Jumper Setting       | Description of Functionality                    |
+--------+----------------------+-------------------------------------------------+
|  JP1   | |jp1-sd_io2|         | Enable pull up for the Micro SD Card            |
+--------+----------------------+-------------------------------------------------+
|  JP1   | |jp1-both|           | Assert GPIO2 low during each download           |
|        |                      | (by jumping it to GPIO0)                        |
+--------+----------------------+-------------------------------------------------+
|  JP7   | |jp7-ext_5v|         | Power ESP-WROVER-KIT board from an external     |
|        |                      | power supply                                    |
+--------+----------------------+-------------------------------------------------+
|  JP7   | |jp7-usb_5v|         | Power ESP-WROVER-KIT board from an USB port     |
+--------+----------------------+-------------------------------------------------+
|  JP8   | |jp8|                | Enable JTAG functionality                       |
+--------+----------------------+-------------------------------------------------+
|  JP11  | |jp11-rx-tx|         | Enable UART communication                       |
+--------+----------------------+-------------------------------------------------+
|  JP14  | |jp14|               | Enable RTS/CTS flow control for serial          |
|        |                      | communication                                   |
+--------+----------------------+-------------------------------------------------+


.. _esp-wrover-kit-v2-start-development:

Start Application Development
-----------------------------

Before powering up the ESP-WROVER-KIT, please make sure that the board has been received in good condition with no obvious signs of damage.


Initial Setup
^^^^^^^^^^^^^

Select the source of power supply for the board by setting jumper JP7. The options are either USB port or an external power supply. For this application selection of USB port is sufficient. Enable UART communication by installing jumpers on JP11. Both selections are shown in table below.

+----------------------+----------------------+
| Power up             | Enable UART          |
| from USB port        | communication        |
+----------------------+----------------------+
| |jp7-usb_5v|         | |jp11-rx-tx|         |
+----------------------+----------------------+

Do not install any other jumpers.


Now to Development
^^^^^^^^^^^^^^^^^^

To start development of applications for ESP32-DevKitC, proceed to section :doc:`index`, that will walk you through the following steps:

* :ref:`get-started-setup-toolchain` in your PC to develop applications for ESP32 in C language
* :ref:`get-started-connect` the module to the PC and verify if it is accessible
* :ref:`get-started-build-flash` an example application to the ESP32
* :ref:`get-started-build-monitor` instantly what the application is doing


Related Documents
-----------------

* `ESP-WROVER-KIT V2 schematic`_ (PDF)
* `ESP32 Datasheet <http://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_ (PDF)
* `ESP-WROOM-32 Datasheet <http://www.espressif.com/sites/default/files/documentation/esp_wroom_32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROVER Datasheet <http://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`_ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* :doc:`../hw-reference/modules-and-boards`



.. |jp1-sd_io2| image:: ../_static/wrover-jp1-sd_io2.png
.. |jp1-both| image:: ../_static/wrover-jp1-both.png
.. |jp7-ext_5v| image:: ../_static/wrover-jp7-ext_5v.png
.. |jp7-usb_5v| image:: ../_static/wrover-jp7-usb_5v.png
.. |jp8| image:: ../_static/wrover-jp8.png
.. |jp11-rx-tx| image:: ../_static/wrover-jp11-tx-rx.png
.. |jp14| image:: ../_static/wrover-jp14.png

.. _ESP-WROVER-KIT V2 schematic: http://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-2.pdf
