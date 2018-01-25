.. _esp-modules-and-boards-previous:

Previous Versions of ESP32 Modules and Boards
=============================================

This sections contains overview and links to documentation of previous version ESP32 Modules and Boards that have been replaced with newer versions or discontinued. It is maintained for convenience of users as several of these boards are still in use and some may still be available for purchase. 

To see the latest development boards, please refer to section :ref:`esp-modules-and-boards`.


.. _esp-modules-and-boards-esp32-pico-pit-v3:

ESP32-PICO-KIT V3
-----------------

The first public release of Espressif's ESP32-PICO-D4 chip on a mini development board. The board has a USB port for programming and debugging and two rows of 20 pin headers to plug into a breadboard. The ESP32-PICO-D4 chip itself is small and requires only a few external components. Besides two core CPUs it integrates 4MB flash memory, a crystal oscillator and antenna matching components in one single 7 x 7 mm package. As a result the chip and all the components making the complete development board fit into 20 x 52 mm PCB. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v3.jpeg
    :align: center
    :alt: ESP32-PICO-KIT V3 board
    :width: 50%

    ESP32-PICO-KIT V3 board

Documentation
"""""""""""""

* :doc:`../get-started/get-started-pico-kit-v3`
* `ESP32-PICO-KIT V3 Schematic <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v3_schematic.pdf>`_ (PDF)
* `ESP32-PICO-D4 Datasheet <http://espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf>`_ (PDF)


.. _esp-modules-and-boards-esp32-devkitc-v2:

ESP32 Core Board V2 / ESP32 DevKitC
-----------------------------------

Small and convenient development board with :ref:`esp-modules-and-boards-esp-wroom-32` module installed, break out pin headers and minimum additional components. Includes USB to serial programming interface, that also provides power supply for the board. Has pushbuttons to reset the board and put it in upload mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-core-board-v2.png
    :align: center
    :alt: ESP32 Core Board V2 / ESP32 DevKitC board
    :width: 50%

    ESP32 Core Board V2 / ESP32 DevKitC board

Documentation
"""""""""""""

* :doc:`../get-started/get-started-devkitc-v2`
* `ESP32 DevKitC V2 Schematic <https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf>`__ (PDF)
* `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_


.. _esp-modules-and-boards-esp-wrover-kit-v1:

ESP-WROVER-KIT V1 / ESP32 DevKitJ V1
------------------------------------

The first version of ESP-WROVER-KIT development board. Shipped with ESP-WROOM-32 on board.

ESP-WROVER-KIT has dual port USB to serial converter for programming and JTAG interface for debugging. Power supply is provided by USB interface or from standard 5 mm power supply jack. Power supply selection is done with a jumper and may be put on/off with a separate switch. The board has MicroSD card slot, 3.2” SPI LCD screen and dedicated header to connect a camera. It provides RGB diode for diagnostics. Includes 32.768 kHz XTAL for internal RTC to operate it in low power modes.

All versions of ESP-WROVER-KIT are ready to accommodate an :ref:`esp-modules-and-boards-esp-wroom-32` or :ref:`esp-modules-and-boards-esp32-wrover` module.


.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-devkitj-v1.jpg
    :align: center
    :alt: ESP-WROVER-KIT V1 / ESP32 DevKitJ V1 board
    :width: 90%

    ESP-WROVER-KIT V1 / ESP32 DevKitJ V1 board

The board has red soldermask.

Documentation
"""""""""""""

* `ESP-WROVER-KIT V1 Schematic <https://dl.espressif.com/dl/schematics/ESP32-DevKitJ-v1_sch.pdf>`__ (PDF)
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
"""""""""""""

* :doc:`../get-started/get-started-wrover-kit-v2`
* `ESP-WROVER-KIT V2 Schematic <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-2.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_


.. _esp-modules-and-boards-esp32-demo-board-v2:

ESP32 Demo Board V2
-------------------

One of first feature rich evaluation boards that contains several pin headers, dip switches, USB to serial programming interface, reset and boot mode press buttons, power switch, 10 touch pads and separate header to connect LCD screen.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-demo-board-v2.jpg
    :align: center
    :alt: ESP32 Demo Board V2

    ESP32 Demo Board V2

Production of this board is discontinued.

Documentation
"""""""""""""

* `ESP32 Demo Board V2 Schematic <https://dl.espressif.com/dl/schematics/ESP32-Demo-Board-V2_sch.pdf>`__ (PDF)
* `FTDI Virtual COM Port Drivers`_


Related Documents
-----------------

* :doc:`modules-and-boards`


.. _FTDI Virtual COM Port Drivers: http://www.ftdichip.com/Drivers/D2XX.htm
