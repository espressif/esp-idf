.. _esp-modules-and-boards:

************************
ESP32 Modules and Boards
************************

Espressif designed and manufactured several development modules and boards to help users evaluate functionality of the ESP32 family of chips. Development boards, depending on intended functionality, have exposed GPIO pins headers, provide USB programming interface, JTAG interface as well as peripherals like touch pads, LCD screen, SD card slot, camera module header, etc.

For details please refer to documentation below, provided together with description of particular boards.

.. note::

    This section describes the latest versions of boards. Previous versions of boards, including these not produced anymore, are described in section :ref:`esp-modules-and-boards-previous`.


WROOM and WROVER Modules
========================

A family of small modules that contain ESP32 chip on board together with some key components including a crystal oscillator and an antenna matching circuit. This makes it easier to provide an ESP32 based solution ready to integrate into final products. Such modules can be also used for evaluation after adding a few extra components like a programming interface, bootstrapping resistors and break out headers. The key characteristics of these modules are summarized in the following table. Some additional details are covered in the following chapters.

===============  ============  =====  ====  ====  ====  ====  ====
--               Key Components                   Dimensions [mm]
---------------  -------------------------------  ----------------
Module           Chip          Flash  RAM   Ant.  L     W     D
===============  ============  =====  ====  ====  ====  ====  ====
ESP-WROOM-32     ESP32-D0WDQ6  4MB    --    MIFA  25.5  18    3.1
ESP-WROOM-32D    ESP32-D0WD    4MB    --    MIFA  25.5  18    3.1
ESP32-WROOM-32U  ESP32-D0WD    4MB    --    U.FL  19.2  18    3.2
ESP32-WROVER     ESP32-D0WDQ6  4MB    4MB   MIFA  31.4  18    3.2
ESP32-WROVER-I   ESP32-D0WDQ6  4MB    4MB   U.FL  31.4  18    3.5
===============  ============  =====  ====  ====  ====  ====  ====

* MIFA - Meandered Inverted-F Antenna 
* U.FL - U.FL / IPEX antenna connector
* `ESP32 Chip Datasheet <https://espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`__ (PDF)


.. _esp-modules-and-boards-esp-wroom-32:

ESP-WROOM-32
------------

A basic and commonly adopted ESP32 module with ESP32-D0WDQ6 chip on board. The first one of the WROOM / WROVER family released to the market. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wroom-32.jpg
    :align: center
    :alt: ESP-WROOM-32 module (front and back)
    :width: 40%

    ESP-WROOM-32 module (front and back)

Documentation
^^^^^^^^^^^^^

* `ESP-WROOM-32 Schematic <https://dl.espressif.com/dl/schematics/ESP-WROOM-32-v3.2_sch.pdf>`__ (PDF)
* `ESP-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp-wroom-32_datasheet_en.pdf>`__ (PDF)
* `ESP32 Module Reference Design <https://espressif.com/sites/default/files/documentation/esp32_module_reference_design.zip>`_ (ZIP) containing OrCAD schematic, PCB layout, gerbers and BOM


.. _esp-modules-and-boards-esp-wroom-32d-and-u:

ESP-WROOM-32D / ESP32-WROOM-32U
-------------------------------

Both modules have ESP32-D0WD chip on board of a smaller footprint than ESP32-D0WDQ6 installed in :ref:`ESP-WROOM-32 <esp-modules-and-boards-esp-wroom-32>`. Version "D" has a MIFA antenna. Version "U" has just an U.FL / IPEX antenna connector. That makes it 6.3 mm shorter comparing to "D", and also the smallest representative of the whole WROOM / WROVER family of modules.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wroom-32d-front-back.jpg
    :align: center
    :alt: ESP-WROOM-32D module (back and front)
    :width: 40%

    ESP-WROOM-32D module (back and front)

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wroom-32u-front-back.jpg
    :align: center
    :alt: ESP32-WROOM-32U module (back and front)
    :width: 40%

    ESP32-WROOM-32U module (back and front)


Documentation
^^^^^^^^^^^^^

* `ESP-WROOM-32D / ESP32-WROOM-32U Datasheet <http://espressif.com/sites/default/files/documentation/esp-wroom-32d_esp32-wroom-32u_datasheet_en.pdf>`__ (PDF)


.. _esp-modules-and-boards-esp32-wrover:

ESP32-WROVER
------------

A step upgrade of :ref:`esp-modules-and-boards-esp-wroom-32` with an additional 4 MB SPI PSRAM (Pseudo static RAM). This module is provided in two versions: 'ESP32-WROVER' with PCB antenna (shown below) and 'ESP32-WROVER-I' with an U.FL / IPEX antenna connector. Because of additional components inside, this module is 5.9 mm longer than :ref:`esp-modules-and-boards-esp-wroom-32`.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wrover.jpg
    :align: center
    :alt: ESP32-WROVER module (front and back)
    :width: 40%

    ESP32-WROVER module (front and back)

Documentation
^^^^^^^^^^^^^

* `ESP32-WROVER Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`__ (PDF)
* `ESP-PSRAM32 Datasheet <https://espressif.com/sites/default/files/documentation/esp-psram32_datasheet_en.pdf>`__ (PDF)


.. _esp-modules-and-boards-esp32-pico-pit-v4:

ESP32-PICO-KIT V4
=================

The smallest ESP32 development board with all the components required to connect it directly to a PC USB port, and pin headers to plug into a mini breadboard. It is equipped with ESP32-PICO-D4 chip that integrates 4MB flash memory, a crystal oscillator, filter capacitors and RF matching circuit in one single package. As result the fully functional development board requires only a few external components that can easy fit on a 20 x 52 mm PCB including antenna, LDO, USB-UART bridge and two buttons to reset it and put into download mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v4.jpeg
    :align: center
    :alt: ESP32-PICO-KIT V4 board
    :width: 50%

    ESP32-PICO-KIT V4 board

Comparing to ESP32-PICO-KIT V3, this version has revised printout and reduced number of exposed pins. Instead of 20, only 17 header pins are populated, so V4 can fit into a mini breadboard. 


Documentation
-------------

* :doc:`../get-started/get-started-pico-kit`
* `ESP32-PICO-KIT V4 Schematic <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v4_schematic.pdf>`_ (PDF)
* `ESP32-PICO-D4 Datasheet <http://espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf>`_ (PDF)

Previous Versions
-----------------

* :ref:`esp-modules-and-boards-esp32-pico-pit-v3`


.. _esp-modules-and-boards-esp32-devkitc:
   
ESP32 Core Board V2 / ESP32 DevKitC
===================================

Small and convenient development board with :ref:`esp-modules-and-boards-esp-wroom-32` module installed, break out pin headers and minimum additional components. Includes USB to serial programming interface, that also provides power supply for the board. Has pushbuttons to reset the board and put it in upload mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-core-board-v2.png
    :align: center
    :alt: ESP32 Core Board V2 / ESP32 DevKitC board
    :width: 50%

    ESP32 Core Board V2 / ESP32 DevKitC board

Documentation
-------------

* :doc:`../get-started/get-started-devkitc`
* `ESP32 DevKitC Schematic <https://dl.espressif.com/dl/schematics/ESP32-Core-Board-V2_sch.pdf>`__ (PDF)
* `ESP32 Development Board Reference Design <https://espressif.com/sites/default/files/documentation/esp32_development_board_reference_design.zip>`_ (ZIP) containing OrCAD schematic, PCB layout, gerbers and BOM
* `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_


.. _esp-modules-and-boards-esp-wrover-kit-v3:

ESP-WROVER-KIT V3
=================

The ESP-WROVER-KIT V3 development board has dual port USB to serial converter for programming and JTAG interface for debugging. Power supply is provided by USB interface or from standard 5 mm power supply jack. Power supply selection is done with a jumper and may be put on/off with a separate switch. This board has MicroSD card slot, 3.2” SPI LCD screen and dedicated header to connect a camera. It provides RGB diode for diagnostics. Includes 32.768 kHz XTAL for internal RTC to operate it in low power modes.

As all previous version of ESP-WROVER-KIT boards, it is ready to accommodate an :ref:`esp-modules-and-boards-esp-wroom-32` or :ref:`esp-modules-and-boards-esp32-wrover` module.

This is the first release of ESP-WROVER-KIT shipped with :ref:`esp-modules-and-boards-esp32-wrover` module installed by default. This release also introduced several design changes to conditioning and interlocking of signals to the bootstrapping pins. Also, a zero Ohm resistor (R166) has been added between WROVER/WROOM module and VDD33 net, which can be desoldered, or replaced with a shunt resistor, for current measurement. This is intended to facilitate power consumption analysis in various operation modes of ESP32. Refer to schematic - the changes are enclosed in green border. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v3.jpg
   :align: center
   :alt: ESP-WROVER-KIT V3 board
   :width: 90%

   ESP-WROVER-KIT V3 board

The camera header has been changed from male back to female. The board soldermask is matte black. The board on picture above has :ref:`esp-modules-and-boards-esp32-wrover` is installed.

Documentation
-------------

* :doc:`../get-started/get-started-wrover-kit`
* `ESP-WROVER-KIT V3 Schematic <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_SCH-3.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_

Previous Versions
-----------------

* :ref:`esp-modules-and-boards-esp-wrover-kit-v1`
* :ref:`esp-modules-and-boards-esp-wrover-kit-v2`


Related Documents
=================

* :doc:`modules-and-boards-previous`


.. _FTDI Virtual COM Port Drivers: http://www.ftdichip.com/Drivers/D2XX.htm
