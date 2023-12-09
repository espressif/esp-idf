ESP32-DevKitC V4 Getting Started Guide
======================================

:link_to_translation:`zh_CN:[中文]`

This guide shows how to start using the ESP32-DevKitC V4 development board.


What You Need
-------------

* :ref:`ESP32-DevKitC V4 board <get-started-esp32-devkitc-board-front>`
* USB A/micro USB B cable
* Computer running Windows, Linux, or macOS

You can skip the introduction sections and go directly to Section `Start Application Development`_.


.. _DevKitC-Overview:

Overview
--------

ESP32-DevKitC V4 is a small-sized ESP32-based development board produced by `Espressif <https://espressif.com>`_. Most of the I/O pins are broken out to the pin headers on both sides for easy interfacing. Developers can either connect peripherals with jumper wires or mount ESP32-DevKitC V4 on a breadboard.

To cover a wide range of user requirements, the following versions of ESP32-DevKitC V4 are available:

- different ESP32 modules

   - `ESP32-WROOM-DA <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-da_datasheet_en.pdf>`_
   - `ESP32-WROOM-32E <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf>`_
   - `ESP32-WROOM-32UE <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf>`_
   - `ESP32-WROOM-32D <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf>`_
   - `ESP32-WROOM-32U <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf>`_
   - `ESP32-SOLO-1 <https://www.espressif.com/sites/default/files/documentation/esp32-solo-1_datasheet_en.pdf>`_
   - `ESP32-WROVER-E <https://www.espressif.com/sites/default/files/documentation/esp32-wrover-e_esp32-wrover-ie_datasheet_en.pdf>`_
   - `ESP32-WROVER-IE <https://www.espressif.com/sites/default/files/documentation/esp32-wrover-e_esp32-wrover-ie_datasheet_en.pdf>`_

- male or female pin headers.

For details please refer to `ESP Product Selector <https://products.espressif.com/#/product-selector?names=>`_.


Functional Description
----------------------

The following figure and the table below describe the key components, interfaces and controls of the ESP32-DevKitC V4 board.

.. _get-started-esp32-devkitc-board-front:

.. figure:: ../../../_static/esp32-devkitc-functional-overview.jpg
    :align: center
    :alt: ESP32-DevKitC V4 with ESP-WROOM-32 module soldered

    ESP32-DevKitC V4 with ESP32-WROOM-32 module soldered


.. list-table::
    :widths: 25 75
    :header-rows: 1

    * - Key Component
      - Description
    * - ESP32-WROOM-32
      - A module with ESP32 at its core. For more information, see `ESP32-WROOM-32 Datasheet`_.
    * - EN
      - Reset button.
    * - Boot
      - Download button. Holding down **Boot** and then pressing **EN** initiates Firmware Download mode for downloading firmware through the serial port.
    * - USB-to-UART Bridge
      - Single USB-UART bridge chip provides transfer rates of up to 3 Mbps.
    * - Micro USB Port
      - USB interface. Power supply for the board as well as the communication interface between a computer and the ESP32-WROOM-32 module.
    * - 5V Power On LED
      - Turns on when the USB or an external 5V power supply is connected to the board. For details see the schematics in `Related Documents`_.
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


Header Block
------------

The two tables below provide the **Name** and **Function** of I/O header pins on both sides of the board, as shown in :ref:`get-started-esp32-devkitc-board-front`.


J2
^^^
===  ====  ==========  ===============================================
No.  Name  Type [1]_   Function
===  ====  ==========  ===============================================
1    3V3   P           3.3 V power supply
2    EN    I           CHIP_PU, Reset
3    VP    I           GPIO36, ADC1_CH0, S_VP
4    VN    I           GPIO39, ADC1_CH3, S_VN
5    IO34  I           GPIO34, ADC1_CH6, VDET_1
6    IO35  I           GPIO35, ADC1_CH7, VDET_2
7    IO32  I/O         GPIO32, ADC1_CH4, TOUCH_CH9, XTAL_32K_P
8    IO33  I/O         GPIO33, ADC1_CH5, TOUCH_CH8, XTAL_32K_N
9    IO25  I/O         GPIO25, ADC1_CH8, DAC_1
10   IO26  I/O         GPIO26, ADC2_CH9, DAC_2
11   IO27  I/O         GPIO27, ADC2_CH7, TOUCH_CH7
12   IO14  I/O         GPIO14, ADC2_CH6, TOUCH_CH6, MTMS
13   IO12  I/O         GPIO12, ADC2_CH5, TOUCH_CH5, MTDI
14   GND   G           Ground
15   IO13  I/O         GPIO13, ADC2_CH4, TOUCH_CH4, MTCK
16   D2    I/O         GPIO9, D2 [2]_
17   D3    I/O         GPIO10, D3 [2]_
18   CMD   I/O         GPIO11, CMD [2]_
19   5V    P           5 V power supply
===  ====  ==========  ===============================================


J3
^^^
===  ====  ==========  ===============================================
No.  Name  Type [1]_   Function
===  ====  ==========  ===============================================
1    GND   G           Ground
2    IO23  I/O         GPIO23
3    IO22  I/O         GPIO22
4    TX    I/O         GPIO1, U0TXD
5    RX    I/O         GPIO3, U0RXD
6    IO21  I/O         GPIO21
7    GND   G           Ground
8    IO19  I/O         GPIO19
9    IO18  I/O         GPIO18
10   IO5   I/O         GPIO5
11   IO17  I/O         GPIO17 [3]_
12   IO16  I/O         GPIO16 [3]_
13   IO4   I/O         GPIO4, ADC2_CH0, TOUCH_CH0
14   IO0   I/O         GPIO0, ADC2_CH1, TOUCH_CH1, Boot
15   IO2   I/O         GPIO2, ADC2_CH2, TOUCH_CH2
16   IO15  I/O         GPIO15, ADC2_CH3, TOUCH_CH3, MTDO
17   D1    I/O         GPIO8, D1 [2]_
18   D0    I/O         GPIO7, D0 [2]_
19   CLK   I/O         GPIO6, CLK [2]_
===  ====  ==========  ===============================================

.. [1] P: Power supply; I: Input; O: Output.
.. [2] The pins D0, D1, D2, D3, CMD and CLK are used internally for communication between ESP32 and SPI flash memory. They are grouped on both sides near the USB connector. Avoid using these pins, as it may disrupt access to the SPI flash memory/SPI RAM.
.. [3] The pins GPIO16 and GPIO17 are available for use only on the boards with the modules ESP32-WROOM and ESP32-SOLO-1. The boards with ESP32-WROVER modules have the pins reserved for internal use.


Pin Layout
^^^^^^^^^^^
.. figure:: ../../../_static/esp32-devkitC-v4-pinout.png
    :align: center
    :scale: 45%
    :alt: ESP32-DevKitC (click to enlarge)

    ESP32-DevKitC Pin Layout (click to enlarge)


Note on C15
-----------

The component C15 may cause the following issues on earlier ESP32-DevKitC V4 boards:

* The board may boot into Download mode
* If you output clock on GPIO0, C15 may impact the signal

In case these issues occur, please remove the component. The figure below shows the location of C15 highlighted in yellow.


.. figure:: ../../../_static/esp32-devkitc-c15-location.png
    :align: center
    :alt: Location of C15 (colored yellow) on ESP32-DevKitC V4 board
    :width: 30%

    Location of C15 (yellow) on ESP32-DevKitC V4 board


Start Application Development
-----------------------------

Before powering up your ESP32-DevKitC V4, please make sure that the board is in good condition with no obvious signs of damage.

After that, proceed to :doc:`../../get-started/index`, where Section :ref:`get-started-step-by-step` will quickly help you set up the development environment and then flash an example project onto your board.


Board Dimensions
----------------

.. figure:: ../../../_static/esp32-devkitc-dimensions-back.jpg
    :align: center
    :scale: 80%
    :alt: Dimensions of ESP32-DevKitC board with ESP32-WROOM-32 module soldered - back (click to enlarge)

    Dimensions of ESP32-DevKitC board with ESP32-WROOM-32 module soldered - back (click to enlarge)


Related Documents
-----------------

* `ESP32-DevKitC V4 schematics <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch.pdf>`_ (PDF)
* `ESP32 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-32D and ESP32-WROOM-32U Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-32E and ESP32-WROOM-32UE Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROOM-DA Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROVER Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`_ (PDF)
* `ESP32-WROVER-B Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-wrover-b_datasheet_en.pdf>`_ (PDF)
* `ESP Product Selector <https://products.espressif.com/#/product-selector?names=>`_

For further design documentation for the board, please contact us at `sales@espressif.com <sales@espressif.com>`_.


.. toctree::
    :hidden:

    get-started-devkitc-v2
