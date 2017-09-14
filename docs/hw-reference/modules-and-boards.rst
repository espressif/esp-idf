.. _esp-modules-and-boards:

ESP32 Modules and Boards
========================

Espressif designed and manufactured several development modules and boards to help users evaluate functionality of ESP32 chip. Development boards, depending on intended functionality, have exposed GPIO pins headers, provide USB programming interface, JTAG interface as well as peripherals like touch pads, LCD screen, SD card slot, camera module header, etc.

For details please refer to documentation below, provided together with description of particular boards.


.. _esp-modules-and-boards-esp-wroom-32:

ESP-WROOM-32
------------

The smallest module intended for installation in final products. Can be also used for evaluation after adding extra components like programming interface, boot strapping resistors and break out headers.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wroom-32.jpg
    :align: center
    :alt: ESP-WROOM-32 module (front and back)
    :width: 40%

    ESP-WROOM-32 module (front and back)

* `Schematic <https://dl.espressif.com/dl/schematics/ESP-WROOM-32-v3.2_sch.pdf>`__ (PDF)
* `Datasheet <https://espressif.com/sites/default/files/documentation/esp-wroom-32_datasheet_en.pdf>`__ (PDF)
* `ESP32 Module Reference Design <https://espressif.com/sites/default/files/documentation/esp32_module_reference_design.zip>`_ (ZIP) containing OrCAD schematic, PCB layout, gerbers and BOM


.. _esp-modules-and-boards-esp32-wrover:

ESP32-WROVER
------------

A step upgrade of ESP-WROOM-32 described above with an additional 4 MB SPI PSRAM (Pseudo static RAM). Module is provided in two versions: 'ESP32-WROVER' with PCB antenna (shown below) and 'ESP32-WROVER-I' with an IPEX antenna.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wrover.jpg
    :align: center
    :alt: ESP32-WROVER module (front and back)
    :width: 40%

    ESP32-WROVER module (front and back)

* `Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`__ (PDF)
* `ESP-PSRAM32 Datasheet <https://espressif.com/sites/default/files/documentation/esp-psram32_datasheet_en.pdf>`__ (PDF)


.. _esp-modules-and-boards-esp32-devkitc:
   
ESP32 Core Board V2 / ESP32 DevKitC
-----------------------------------

Small and convenient development board with ESP-WROOM-32 module installed, break out pin headers and minimum additional components. Includes USB to serial programming interface, that also provides power supply for the board. Has press buttons to reset the board and put it in upload mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-core-board-v2.jpg
    :align: center
    :alt: ESP32 Core Board V2 / ESP32 DevKitC board
    :width: 50%

    ESP32 Core Board V2 / ESP32 DevKitC board

* `Schematic <https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf>`__ (PDF)
* `ESP32 Development Board Reference Design <https://espressif.com/sites/default/files/documentation/esp32_development_board_reference_design.zip>`_ (ZIP) containing OrCAD schematic, PCB layout, gerbers and BOM
* :doc:`../get-started/get-started-devkitc`
* `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_


.. _esp-modules-and-boards-esp32-demo-board-v2:

ESP32 Demo Board V2
-------------------

One of first feature rich evaluation boards that contains several pin headers, dip switches, USB to serial programming interface, reset and boot mode press buttons, power switch, 10 touch pads and separate header to connect LCD screen.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-demo-board-v2.jpg
    :align: center
    :alt: ESP32 Demo Board V2

    ESP32 Demo Board V2

* `Schematic <https://dl.espressif.com/dl/schematics/ESP32-Demo-Board-V2_sch.pdf>`__ (PDF)
* `FTDI Virtual COM Port Drivers`_


.. _esp-modules-and-boards-esp-wrover-kit:

ESP-WROVER-KIT
--------------

This section describes several revisions of ESP-WROVER-KIT development board.

All versions of ESP-WROVER-KIT are ready to accommodate an :ref:`esp-modules-and-boards-esp-wroom-32` or :ref:`esp-modules-and-boards-esp32-wrover` module.

ESP-WROVER-KIT has dual port USB to serial converter for programming and JTAG interface for debugging. Power supply is provided by USB interface or from standard 5 mm power supply jack. Power supply selection is done with a jumper and may be put on/off with a separate switch. The board has MicroSD card slot, 3.2” SPI LCD screen and dedicated header to connect a camera. It provides RGB diode for diagnostics. Includes 32.768 kHz XTAL for internal RTC to operate it in low power modes.


.. _esp-modules-and-boards-esp-wrover-kit-v1:

ESP-WROVER-KIT V1 / ESP32 DevKitJ V1
""""""""""""""""""""""""""""""""""""

First version of ESP-WROVER-KIT. Shipped with ESP-WROOM-32 on board.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-devkitj-v1.jpg
    :align: center
    :alt: ESP-WROVER-KIT V1 / ESP32 DevKitJ V1 board
    :width: 90%

    ESP-WROVER-KIT V1 / ESP32 DevKitJ V1 board

The board has red soldermask.

* `Schematic <https://dl.espressif.com/dl/schematics/ESP32-DevKitJ-v1_sch.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_


.. _esp-modules-and-boards-esp-wrover-kit-v2:

ESP-WROVER-KIT V2
"""""""""""""""""

This is updated version of ESP32 DevKitJ V1 described above with design improvements identified when DevKitJ was in use, e.g. improved support for SD card. By default board has ESP-WROOM-32 module installed.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v2.jpg
   :align: center
   :alt: ESP-WROVER-KIT V2 board
   :width: 90%

   ESP-WROVER-KIT V2 board

Comparing to previous version, this board has a shiny black finish and a male camera header.

* `Schematic V2 <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-2.pdf>`__ (PDF)
* :doc:`../get-started/get-started-wrover-kit-v2`
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_


.. _esp-modules-and-boards-esp-wrover-kit-v3:

ESP-WROVER-KIT V3
"""""""""""""""""

The first release of ESP-WROVER-KIT shipped with ESP32-WROVER module installed by default. This release also introduced several design changes to conditioning and interlocking of signals to the bootstrapping pins. Also, a zero Ohm resistor (R166) has been added between WROVER/WROOM module and VDD33 net, which can be desoldered, or replaced with a shunt resistor, for current measurement. This is intended to facilitate power consumption analysis in various operation modes of ESP32. Refer to schematic - the changes are enclosed in green border. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v3.jpg
   :align: center
   :alt: ESP-WROVER-KIT V3 board
   :width: 90%

   ESP-WROVER-KIT V3 board

The camera header has been changed from male back to female. The board soldermask is matte black. The board on picture above has ESP32-WROVER is installed.

* `Schematic V3 <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-3.pdf>`__ (PDF)
* :doc:`../get-started/get-started-wrover-kit`
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_


.. _FTDI Virtual COM Port Drivers: http://www.ftdichip.com/Drivers/D2XX.htm
