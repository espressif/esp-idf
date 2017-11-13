.. _esp-modules-and-boards:

ESP32 Modules and Boards
========================

Espressif designed and manufactured several development modules and boards to help users evaluate functionality of the ESP32 family of chips. Development boards, depending on intended functionality, have exposed GPIO pins headers, provide USB programming interface, JTAG interface as well as peripherals like touch pads, LCD screen, SD card slot, camera module header, etc.

For details please refer to documentation below, provided together with description of particular boards.

.. note::

    This section describes the latest versions of boards. Previous versions of boards, including these not produced anymore, are described in section :ref:`esp-modules-and-boards-previous`.


.. _esp-modules-and-boards-esp-wroom-32:

ESP-WROOM-32
------------

The smallest module intended for installation in final products. Can be also used for evaluation after adding extra components like programming interface, boot strapping resistors and break out headers.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wroom-32.jpg
    :align: center
    :alt: ESP-WROOM-32 module (front and back)
    :width: 40%

    ESP-WROOM-32 module (front and back)

* `ESP-WROOM-32 Schematic <https://dl.espressif.com/dl/schematics/ESP-WROOM-32-v3.2_sch.pdf>`__ (PDF)
* `ESP-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp-wroom-32_datasheet_en.pdf>`__ (PDF)
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

Documentation
"""""""""""""

* `ESP32-WROVER Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`__ (PDF)
* `ESP-PSRAM32 Datasheet <https://espressif.com/sites/default/files/documentation/esp-psram32_datasheet_en.pdf>`__ (PDF)


.. _esp-modules-and-boards-esp32-pico-pit-v4:

ESP32-PICO-KIT V4
-----------------

The smallest ESP32 development board with all the components required to connect it directly to a PC USB port, and pin headers to plug into a mini breadboard. It is equipped with ESP32-PICO-D4 chip that integrates 4MB flash memory, a crystal oscillator, filter capacitors and RF matching circuit in one single package. As result the fully functional development board requires only a few external components that can easy fit on a 20 x 52 mm PCB including antenna, LDO, USB-UART bridge and two buttons to reset it and put into download mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v4.jpeg
    :align: center
    :alt: ESP32-PICO-KIT V4 board
    :width: 50%

    ESP32-PICO-KIT V4 board

Comparing to ESP32-PICO-KIT V3, this version has revised printout and reduced number of exposed pins. Instead of 20, only 17 header pins are populated, so V4 can fit into a mini breadboard. 


Documentation
"""""""""""""

* :doc:`../get-started/get-started-pico-kit`
* `ESP32-PICO-KIT V4 Schematic <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v4_schematic.pdf>`_ (PDF)
* `ESP32-PICO-D4 Datasheet <http://espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf>`_ (PDF)

Previous Versions
"""""""""""""""""

* :ref:`esp-modules-and-boards-esp32-pico-pit-v3`


.. _esp-modules-and-boards-esp32-devkitc:
   
ESP32 Core Board V2 / ESP32 DevKitC
-----------------------------------

Small and convenient development board with ESP-WROOM-32 module installed, break out pin headers and minimum additional components. Includes USB to serial programming interface, that also provides power supply for the board. Has press buttons to reset the board and put it in upload mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-core-board-v2.png
    :align: center
    :alt: ESP32 Core Board V2 / ESP32 DevKitC board
    :width: 50%

    ESP32 Core Board V2 / ESP32 DevKitC board

Documentation
"""""""""""""

* :doc:`../get-started/get-started-devkitc`
* `ESP32 DevKitC Schematic <https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf>`__ (PDF)
* `ESP32 Development Board Reference Design <https://espressif.com/sites/default/files/documentation/esp32_development_board_reference_design.zip>`_ (ZIP) containing OrCAD schematic, PCB layout, gerbers and BOM
* `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_


.. _esp-modules-and-boards-esp-wrover-kit-v3:

ESP-WROVER-KIT V3
-----------------

The ESP-WROVER-KIT V3 development board has dual port USB to serial converter for programming and JTAG interface for debugging. Power supply is provided by USB interface or from standard 5 mm power supply jack. Power supply selection is done with a jumper and may be put on/off with a separate switch. This board has MicroSD card slot, 3.2” SPI LCD screen and dedicated header to connect a camera. It provides RGB diode for diagnostics. Includes 32.768 kHz XTAL for internal RTC to operate it in low power modes.

As all previous version of ESP-WROVER-KIT boards, it is ready to accommodate an :ref:`esp-modules-and-boards-esp-wroom-32` or :ref:`esp-modules-and-boards-esp32-wrover` module.

This is the first release of ESP-WROVER-KIT shipped with ESP32-WROVER module installed by default. This release also introduced several design changes to conditioning and interlocking of signals to the bootstrapping pins. Also, a zero Ohm resistor (R166) has been added between WROVER/WROOM module and VDD33 net, which can be desoldered, or replaced with a shunt resistor, for current measurement. This is intended to facilitate power consumption analysis in various operation modes of ESP32. Refer to schematic - the changes are enclosed in green border. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v3.jpg
   :align: center
   :alt: ESP-WROVER-KIT V3 board
   :width: 90%

   ESP-WROVER-KIT V3 board

The camera header has been changed from male back to female. The board soldermask is matte black. The board on picture above has ESP32-WROVER is installed.

Documentation
"""""""""""""

* :doc:`../get-started/get-started-wrover-kit`
* `ESP-WROVER-KIT V3 Schematic <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-3.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_

Previous Versions
"""""""""""""""""

* :ref:`esp-modules-and-boards-esp-wrover-kit-v1`
* :ref:`esp-modules-and-boards-esp-wrover-kit-v2`


Related Documents
-----------------

* :doc:`modules-and-boards-previous`


.. _FTDI Virtual COM Port Drivers: http://www.ftdichip.com/Drivers/D2XX.htm
