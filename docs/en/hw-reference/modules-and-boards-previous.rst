.. _esp-modules-and-boards-previous:

*********************************************
Previous Versions of ESP32 Modules and Boards
*********************************************

This sections contains overview and links to documentation of previous version ESP32 Modules and Boards that have been replaced with newer versions or discontinued. It is maintained for convenience of users as previous versions of some modules and boards are still in use and some may still be available for purchase. 


Modules (No updated or discontinued modules)
============================================

So far, no modules have been updated or discontinued.


Development Boards
==================

To see the latest development boards, please refer to section :ref:`esp-modules-and-boards`.

.. _esp-modules-and-boards-esp32-pico-kit-v4:

ESP32-PICO-KIT V4
-----------------

The smallest ESP32 development board with all the components required to connect it directly to a PC USB port, and pin headers to plug into a mini breadboard. It is equipped with ESP32-PICO-D4 module that integrates 4 MB flash memory, a crystal oscillator, filter capacitors and RF matching circuit in one single package. As result, the fully functional development board requires only a few external components that can easy fit on a 20 x 52 mm PCB including antenna, LDO, USB-UART bridge and two buttons to reset it and put into download mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v4.jpeg
    :align: center
    :alt: ESP32-PICO-KIT V4 board
    :width: 50%

    ESP32-PICO-KIT V4 board

Comparing to ESP32-PICO-KIT V3, this version has revised printout and reduced number of exposed pins. Instead of 20, only 17 header pins are populated, so V4 can fit into a mini breadboard.

Documentation
^^^^^^^^^^^^^

* :doc:`../hw-reference/get-started-pico-kit`
* `ESP32-PICO-KIT V4 Schematic <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v4_schematic.pdf>`_ (PDF)
* `ESP32-PICO-D4 Datasheet <http://espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf>`_ (PDF)

.. _esp-modules-and-boards-esp32-pico-kit-v3:

ESP32-PICO-KIT V3
-----------------

The first public release of Espressif's ESP32-PICO-D4 module on a mini development board. The board has a USB port for programming and debugging and two rows of 20 pin headers to plug into a breadboard. The ESP32-PICO-D4 module itself is small and requires only a few external components. Besides two core CPUs it integrates 4MB flash memory, a crystal oscillator and antenna matching components in one single 7 x 7 mm package. As a result the module and all the components making the complete development board fit into 20 x 52 mm PCB. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v3.jpeg
    :align: center
    :alt: ESP32-PICO-KIT V3 board
    :width: 50%

    ESP32-PICO-KIT V3 board

Documentation
^^^^^^^^^^^^^

* :doc:`../hw-reference/get-started-pico-kit-v3`
* `ESP32-PICO-KIT V3 Schematic <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v3_schematic.pdf>`_ (PDF)
* `ESP32-PICO-D4 Datasheet <http://espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf>`_ (PDF)


.. _esp-modules-and-boards-esp32-devkitc-v2:

ESP32 Core Board V2 / ESP32 DevKitC
-----------------------------------

Small and convenient development board with ESP-WROOM-32 module installed, break out pin headers and minimum additional components. Includes USB to serial programming interface, that also provides power supply for the board. Has pushbuttons to reset the board and put it in upload mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-core-board-v2.png
    :align: center
    :alt: ESP32 Core Board V2 / ESP32 DevKitC board
    :width: 50%

    ESP32 Core Board V2 / ESP32 DevKitC board

Documentation
^^^^^^^^^^^^^

* :doc:`../hw-reference/get-started-devkitc-v2`
* `ESP32 DevKitC V2 Schematic <https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf>`__ (PDF)
* `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_

.. _esp-modules-and-boards-esp-wrover-kit-v3:

ESP-WROVER-KIT V3
-----------------

The ESP-WROVER-KIT V3 development board has dual port USB to serial converter for programming and JTAG interface for debugging. Power supply is provided by USB interface or from standard 5 mm power supply jack. Power supply selection is done with a jumper and may be put on/off with a separate switch. This board has MicroSD card slot, 3.2” SPI LCD screen and dedicated header to connect a camera. It provides RGB diode for diagnostics. Includes 32.768 kHz XTAL for internal RTC to operate it in low power modes.

As all previous versions of ESP-WROVER-KIT boards, it is ready to accommodate an :ref:`esp-modules-and-boards-esp32-wroom-32` or :ref:`esp-modules-and-boards-esp32-wrover` module.

This is the first release of ESP-WROVER-KIT shipped with :ref:`esp-modules-and-boards-esp32-wrover` module installed by default. This release also introduced several design changes to conditioning and interlocking of signals to the bootstrapping pins. Also, a zero Ohm resistor (R166) has been added between WROVER/WROOM module and VDD33 net, which can be desoldered, or replaced with a shunt resistor, for current measurement. This is intended to facilitate power consumption analysis in various operation modes of ESP32. Refer to schematic - the changes are enclosed in green border. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v3.jpg
   :align: center
   :alt: ESP-WROVER-KIT V3 board
   :width: 90%

   ESP-WROVER-KIT V3 board

The camera header has been changed from male back to female. The board soldermask is matte black. The board on picture above has :ref:`esp-modules-and-boards-esp32-wrover` is installed.

Documentation
^^^^^^^^^^^^^

* :doc:`../hw-reference/get-started-wrover-kit-v3`
* `ESP-WROVER-KIT V3 Schematic <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-3.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_

.. _esp-modules-and-boards-esp-wrover-kit-v2:

ESP-WROVER-KIT V2
-----------------

This is updated version of ESP32 DevKitJ V1 described above with design improvements identified when DevKitJ was in use, e.g. improved support for SD card. By default board has ESP-WROOM-32 module installed.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v2.jpg
   :align: center
   :alt: ESP-WROVER-KIT V2 board
   :width: 90%

   ESP-WROVER-KIT V2 board

Comparing to previous version, this board has a shiny black finish and a male camera header.

Documentation
^^^^^^^^^^^^^

* :doc:`../hw-reference/get-started-wrover-kit-v2`
* `ESP-WROVER-KIT V2 Schematic <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-2.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_

.. _esp-modules-and-boards-esp-wrover-kit-v1:

ESP-WROVER-KIT V1 / ESP32 DevKitJ V1
------------------------------------

The first version of ESP-WROVER-KIT development board. Shipped with ESP-WROOM-32 on board.

ESP-WROVER-KIT has dual port USB to serial converter for programming and JTAG interface for debugging. Power supply is provided by USB interface or from standard 5 mm power supply jack. Power supply selection is done with a jumper and may be put on/off with a separate switch. The board has MicroSD card slot, 3.2” SPI LCD screen and dedicated header to connect a camera. It provides RGB diode for diagnostics. Includes 32.768 kHz XTAL for internal RTC to operate it in low power modes.

All versions of ESP-WROVER-KIT are ready to accommodate an ESP-WROOM-32 or ESP32-WROVER module.


.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-devkitj-v1.jpg
    :align: center
    :alt: ESP-WROVER-KIT V1 / ESP32 DevKitJ V1 board
    :width: 90%

    ESP-WROVER-KIT V1 / ESP32 DevKitJ V1 board

The board has red soldermask.

Documentation
^^^^^^^^^^^^^

* `ESP-WROVER-KIT V1 Schematic <https://dl.espressif.com/dl/schematics/ESP32-DevKitJ-v1_sch.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_

.. _esp-modules-and-boards-esp32-demo-board:

ESP32 Demo Board V2
-------------------

One of first feature rich evaluation boards that contains several pin headers, dip switches, USB to serial programming interface, reset and boot mode press buttons, power switch, 10 touch pads and separate header to connect LCD screen.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-demo-board-v2.jpg
    :align: center
    :alt: ESP32 Demo Board V2

    ESP32 Demo Board V2

Production of this board is discontinued.

Documentation
^^^^^^^^^^^^^

* `ESP32 Demo Board V2 Schematic <https://dl.espressif.com/dl/schematics/ESP32-Demo-Board-V2_sch.pdf>`__ (PDF)
* `FTDI Virtual COM Port Drivers`_


Related Documents
=================

* :doc:`modules-and-boards`


.. _FTDI Virtual COM Port Drivers: http://www.ftdichip.com/Drivers/VCP.htm
