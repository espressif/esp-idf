.. _esp-modules-and-boards:

************************
ESP32 Modules and Boards
************************

Espressif designed and manufactured several development modules and boards to help users evaluate functionality of the ESP32 family of chips. Development boards, depending on intended functionality, have exposed GPIO pins headers, provide USB programming interface, JTAG interface as well as peripherals like touch pads, LCD screen, SD card slot, camera module header, etc.

For details please refer to documentation below, provided together with description of particular boards.

.. note::

    This section describes the latest versions of boards. Previous versions of boards, including these not produced anymore, are described in section :ref:`esp-modules-and-boards-previous`.

.. _esp-wroom-solo-wrover-modules:

WROOM, SOLO and WROVER Modules
==============================

A family of small modules that contain ESP32 chip on board together with some key components including a crystal oscillator and an antenna matching circuit. This makes it easier to provide an ESP32 based solution ready to integrate into final products. Such modules can be also used for evaluation after adding a few extra components like a programming interface, bootstrapping resistors and break out headers. The key characteristics of these modules are summarized in the following table. Some additional details are covered in the following chapters.

===============  ============  =====  ======  ====
--               Key Components                   
---------------  ---------------------------------
Module           Chip          Flash  PSRAM   Ant.
===============  ============  =====  ======  ====
ESP32-WROOM-32   ESP32-D0WDQ6  4MB    --      MIFA
ESP32-WROOM-32D  ESP32-D0WD    4MB    --      MIFA
ESP32-WROOM-32U  ESP32-D0WD    4MB    --      U.FL
ESP32-SOLO-1     ESP32-S0WD    4MB    --      MIFA
ESP32-WROVER     ESP32-D0WDQ6  4MB    8MB     MIFA
ESP32-WROVER-I   ESP32-D0WDQ6  4MB    8MB     U.FL
ESP32-WROVER-B   ESP32-D0WD    4MB    8MB     MIFA
ESP32-WROVER-IB  ESP32-D0WD    4MB    8MB     U.FL
===============  ============  =====  ======  ====

* ESP32-**D**.. denotes dual core, ESP32-**S**.. denotes single core chip
* MIFA - Meandered Inverted-F Antenna 
* U.FL - U.FL / IPEX antenna connector
* ESP32-WROOM-x and ESP32-WROVER-x modules are also available with custom flash sizes of 8MB or 16MB, see `Espressif Products Ordering Information`_ (PDF)
* `ESP32 Chip Datasheet <https://espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`__ (PDF)
* Initial release of ESP32-WROVER module had 4MB of PSRAM
* *ESP32-WROOM-32* was previously called *ESP-WROOM-32*


.. _esp-modules-and-boards-esp32-wroom-32:

ESP32-WROOM-32
--------------

A basic and commonly adopted ESP32 module with ESP32-D0WDQ6 chip on board. The first one of the WROOM / WROVER family released to the market. By default the module has 4MB flash and may be also ordered with custom flash size of 8 or 16MB, see `Espressif Products Ordering Information`_.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wroom-32-front-back.jpg
    :align: center
    :alt: ESP32-WROOM-32 module (front and back)
    :width: 45%

    ESP32-WROOM-32 module (front and back)

Documentation
^^^^^^^^^^^^^

* `ESP32-WROOM-32 Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf>`__ (PDF)
* `ESP32-WROOM-32 Reference Design <https://www.espressif.com/en/support/download/documents?keys=+ESP32-WROOM-32+Reference>`_ containing OrCAD schematic, PCB layout, gerbers and BOM


.. _esp-modules-and-boards-esp32-wroom-32d-and-u:

ESP32-WROOM-32D / ESP32-WROOM-32U
---------------------------------

Both modules have ESP32-D0WD chip on board of a smaller footprint than ESP32-D0WDQ6 installed in :ref:`esp-modules-and-boards-esp32-wroom-32`. By default the module has 4MB flash and may be also ordered with custom flash size of 8 or 16MB, see `Espressif Products Ordering Information`_. Version "D" has a MIFA antenna. Version "U" has just an U.FL / IPEX antenna connector. That makes it 6.3 mm shorter comparing to "D", and also the smallest representative of the whole WROOM / WROVER family of modules.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wroom-32d-front-back.jpg
    :align: center
    :alt: ESP32-WROOM-32D module (front and back)
    :width: 45%

    ESP32-WROOM-32D module (front and back)

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wroom-32u-front-back.jpg
    :align: center
    :alt: ESP32-WROOM-32U module (front and back)
    :width: 45%

    ESP32-WROOM-32U module (front and back)

Documentation
^^^^^^^^^^^^^

* `ESP32-WROOM-32D / ESP32-WROOM-32U Datasheet <http://espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf>`__ (PDF)


.. _esp-modules-and-boards-esp32-solo-1:

ESP32-SOLO-1
------------

Simplified version of ESP32-WROOM-32D module. It contains a single core ESP32 chip that supports clock frequency of up to 160 MHz.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-solo-1-front-back.jpg
    :align: center
    :alt: ESP32-SOLO-1 module (front and back)
    :width: 45%

    ESP32-SOLO-1 module (front and back)


Documentation
^^^^^^^^^^^^^

* `ESP32-SOLO-1 Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32-solo-1_datasheet_en.pdf>`__ (PDF)


.. _esp-modules-and-boards-esp32-wrover:

ESP32-WROVER
------------

A step upgrade of ESP32-WROOM-32x modules with an additional 8MB SPI PSRAM (Pseudo static RAM). 

The module comes in couple of versions listed in section :ref:`esp-wroom-solo-wrover-modules`:

* **ESP32-WROVER** and **ESP32-WROVER-I** have PSRAM that operates at 1.8V and can support up to 144 MHz clock rate.
* **ESP32-WROVER-B** and **ESP32-WROVER-IB** have PSRAM that operates at 3.3V and can support up to 133 MHz clock rate.

By default the module has 4MB flash and may be also ordered with custom flash size of 8 or 16MB, see `Espressif Products Ordering Information`_. 

Depending on version the module has PCB antenna (shown below) or an U.FL / IPEX antenna connector. Because of additional components inside, this module is 5.9 mm longer than :ref:`esp-modules-and-boards-esp32-wroom-32`.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-wrover.jpg
    :align: center
    :alt: ESP32-WROVER module (front and back)
    :width: 40%

    ESP32-WROVER module (front and back)

Documentation
^^^^^^^^^^^^^

* `ESP32-WROVER Datasheet <https://espressif.com/sites/default/files/documentation/esp32-wrover_datasheet_en.pdf>`__ (PDF)
* `ESP-PSRAM64 & ESP-PSRAM64H Datasheet <https://www.espressif.com/sites/default/files/documentation/esp-psram64_esp-psram64h_datasheet_en.pdf>`__ (PDF)
* `ESP32-WROVER Reference Design <https://www.espressif.com/en/support/download/documents?keys=ESP32-WROVER+Reference+Design>`_ containing OrCAD schematic, PCB layout, gerbers and BOM


.. _esp-modules-and-boards-esp32-pico-kit:

ESP32-PICO-KIT V4.1
===================

The smallest ESP32 development board with all the components required to connect it directly to a PC USB port, and pin headers to plug into a mini breadboard. It is equipped with ESP32-PICO-D4 module that integrates 4 MB flash memory, a crystal oscillator, filter capacitors and RF matching circuit in one single package. As result, the fully functional development board requires only a few external components that can easy fit on a 20 x 52 mm PCB including antenna, LDO, USB-UART bridge and two buttons to reset it and put into download mode. 

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-pico-kit-v4.1.jpg
    :align: center
    :alt: ESP32-PICO-KIT V4.1 board
    :width: 50%

    ESP32-PICO-KIT V4.1 board

Comparing to ESP32-PICO-KIT V4, this version contains a more capable CP2102N USB-UART bridge that provides up to 3 Mbps transfers rates.

Documentation
-------------

* :doc:`../get-started/get-started-pico-kit`
* `ESP32-PICO-KIT V4.1 Schematic <https://dl.espressif.com/dl/schematics/esp32-pico-kit-v4.1_schematic.pdf>`_ (PDF)
* `ESP32-PICO-KIT Reference Design <https://www.espressif.com/en/support/download/documents?keys=ESP32-PICO-KIT+Reference+Design>`_ containing OrCAD schematic, PCB layout, gerbers and BOM
* `ESP32-PICO-D4 Datasheet <http://espressif.com/sites/default/files/documentation/esp32-pico-d4_datasheet_en.pdf>`_ (PDF)

Previous Versions
-----------------

* :ref:`esp-modules-and-boards-esp32-pico-kit-v4`
* :ref:`esp-modules-and-boards-esp32-pico-kit-v3`


.. _esp-modules-and-boards-esp32-devkitc:
   
ESP32 DevKitC V4
================

Small and convenient development board with :ref:`esp-modules-and-boards-esp32-wroom-32` module installed, break out pin headers and minimum additional components. Includes USB to serial programming interface, that also provides power supply for the board. Has pushbuttons to reset the board and put it in upload mode. Comparing to the previous :ref:`esp-modules-and-boards-esp32-devkitc-v2`, instead of ESP32-WROOM-32 it can accommodate :ref:`esp-modules-and-boards-esp32-wrover` module and has CP2102N chip that supports faster baud rates.

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp32-devkitc-v4-front.jpg
    :align: center
    :alt: ESP32 DevKitC V4 board
    :width: 50%

    ESP32 DevKitC V4 board

Documentation
-------------

* :doc:`../get-started/get-started-devkitc`
* `ESP32-DevKitC schematic <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf>`_ (PDF)
* `ESP32-DevKitC Reference Design <https://www.espressif.com/en/support/download/documents?keys=ESP32-DevKitC-V4+Reference+Design>`_ containing OrCAD schematic, PCB layout, gerbers and BOM
* `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_

Previous Versions
-----------------

* :ref:`esp-modules-and-boards-esp32-devkitc-v2`


.. _esp-modules-and-boards-esp-wrover-kit:

ESP-WROVER-KIT V4.1
===================

The ESP-WROVER-KIT V4.1 development board has dual port USB to serial converter for programming and JTAG interface for debugging. Power supply is provided by USB interface or from standard 5 mm power supply jack. Power supply selection is done with a jumper and may be put on/off with a separate switch. This board has MicroSD card slot, 3.2” SPI LCD screen and dedicated header to connect a camera. It provides RGB diode for diagnostics. Includes 32.768 kHz XTAL for internal RTC to operate it in low power modes.

This version of ESP-WROVER-KIT board has ESP-WROVER-B module installed that integrates 64-MBit PSRAM for flexible extended storage and data processing capabilities. The board can accommodate other versions of ESP modules described under :ref:`esp-wroom-solo-wrover-modules`.

Comparing to :ref:`esp-modules-and-boards-esp-wrover-kit-v3`, this board has the following design changes:

  * JP8, JP11 and JP13 have been combined into a single JP2
  * USB connector has been changed to DIP type and moved to the lower right corner of the board
  * R61 has been changed to 0R
  * Some other components, e.g. EN and Boot buttons, have been replaced with functional equivalents basing on test results and sourcing options

.. figure:: https://dl.espressif.com/dl/schematics/pictures/esp-wrover-kit-v4.1-front.jpg
   :align: center
   :alt: ESP-WROVER-KIT V4.1 board
   :width: 90%

   ESP-WROVER-KIT V4.1 board

The board on picture above has ESP32-WROVER-B module is installed.

Documentation
-------------

* :doc:`../get-started/get-started-wrover-kit`
* `ESP-WROVER-KIT V4.1 Schematic <https://dl.espressif.com/dl/schematics/ESP-WROVER-KIT_V4_1.pdf>`__ (PDF)
* :doc:`../api-guides/jtag-debugging/index`
* `FTDI Virtual COM Port Drivers`_

Previous Versions
-----------------

* :ref:`esp-modules-and-boards-esp-wrover-kit-v3`
* :ref:`esp-modules-and-boards-esp-wrover-kit-v2`
* :ref:`esp-modules-and-boards-esp-wrover-kit-v1`


Related Documents
=================

* :doc:`modules-and-boards-previous`


.. _FTDI Virtual COM Port Drivers: http://www.ftdichip.com/Drivers/VCP.htm
.. _Espressif Products Ordering Information: https://www.espressif.com/sites/default/files/documentation/espressif_products_ordering_information_en.pdf