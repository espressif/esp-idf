Establish Serial Connection with ESP32
==============================================

:link_to_translation:`zh_CN:[中文]`

This section provides guidance how to establish serial connection between ESP32 and PC.


Connect ESP32 to PC
--------------------

Connect the ESP32 board to the PC using the USB cable. If device driver does not install automatically, identify USB to serial converter chip on your ESP32 board (or external converter dongle), search for drivers in internet and install them.

Below are the links to drivers for ESP32 boards produced by Espressif:


.. csv-table::
    :header: Development Board, USB Driver, Remarks
    :widths: 40, 20, 40

    :ref:`ESP32-DevKitC <esp-modules-and-boards-esp32-devkitc>`,  `CP210x`_
    `ESP32-LyraT <https://www.espressif.com/en/products/hardware/esp32-lyrat>`_, `CP210x`_
    `ESP32-LyraTD-MSC <https://www.espressif.com/en/products/hardware/esp32-lyratd-msc>`_, `CP210x`_
    :ref:`ESP32-PICO-KIT <esp-modules-and-boards-esp32-pico-kit>`, `CP210x`_
    :ref:`ESP-WROVER-KIT <esp-modules-and-boards-esp-wrover-kit>`, `FTDI`_
    :ref:`ESP32 Demo Board <esp-modules-and-boards-esp32-demo-board>`, `FTDI`_
    `ESP-Prog`_, `FTDI`_, Programmer board (w/o ESP32)
    `ESP32-MeshKit-Sense <https://github.com/espressif/esp-iot-solution/blob/master/documents/evaluation_boards/ESP32-MeshKit-Sense_guide_en.md#esp32-meshkit-sense-hardware-design-guidelines>`_, n/a, Use with `ESP-Prog`_
    `ESP32-Sense Kit <https://github.com/espressif/esp-iot-solution/blob/master/documents/evaluation_boards/esp32_sense_kit_guide_en.md#guide-for-esp32-sense-development-kit>`_, n/a, Use with `ESP-Prog`_

.. _CP210x: https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers
.. _FTDI: http://www.ftdichip.com/Drivers/VCP.htm 
.. _ESP-Prog: https://github.com/espressif/esp-iot-solution/blob/master/documents/evaluation_boards/ESP-Prog_guide_en.md#introduction-to-the-esp-prog-board

* CP210x: `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_ 
* FTDI: `FTDI Virtual COM Port Drivers <http://www.ftdichip.com/Drivers/VCP.htm>`_

The drivers above are primarily for reference. Under normal circumstances, the drivers should be bundled with and operating system and automatically installed upon connecting one of the listed boards to the PC.


Check port on Windows
---------------------

Check the list of identified COM ports in the Windows Device Manager. Disconnect ESP32 and connect it back, to verify which port disappears from the list and then shows back again.

Figures below show serial port for ESP32 DevKitC and ESP32 WROVER KIT

.. figure:: ../../_static/esp32-devkitc-in-device-manager.png
    :align: center
    :alt: USB to UART bridge of ESP32-DevKitC in Windows Device Manager
    :figclass: align-center

    USB to UART bridge of ESP32-DevKitC in Windows Device Manager

.. figure:: ../../_static/esp32-wrover-kit-in-device-manager.png
    :align: center
    :alt: Two USB Serial Ports of ESP-WROVER-KIT in Windows Device Manager
    :figclass: align-center

    Two USB Serial Ports of ESP-WROVER-KIT in Windows Device Manager


Check port on Linux and MacOS
-----------------------------

To check the device name for the serial port of your ESP32 board (or external converter dongle), run this command two times, first with the board / dongle unplugged, then with plugged in. The port which appears the second time is the one you need:

Linux ::

    ls /dev/tty*

MacOS ::

    ls /dev/cu.*

.. note::

    MacOS users: if you don't see the serial port then check you have the USB/serial drivers installed as shown in the Getting Started guide for your particular development board. For MacOS High Sierra (10.13), you may also have to explicitly allow the drivers to load. Open System Preferences -> Security & Privacy -> General and check if there is a message shown here about "System Software from developer ..." where the developer name is Silicon Labs or FTDI.


.. _linux-dialout-group:

Adding user to ``dialout`` on Linux
-----------------------------------

The currently logged user should have read and write access the serial port over USB. On most Linux distributions, this is done by adding the user to ``dialout`` group with the following command::

    sudo usermod -a -G dialout $USER

on Arch Linux this is done by adding the user to ``uucp`` group with the following command::

    sudo usermod -a -G uucp $USER

Make sure you re-login to enable read and write permissions for the serial port. 


Verify serial connection
------------------------

Now verify that the serial connection is operational. You can do this using a serial terminal program. In this example we will use `PuTTY SSH Client <http://www.putty.org/>`_ that is available for both Windows and Linux. You can use other serial program and set communication parameters like below.

Run terminal, set identified serial port, baud rate = 115200, data bits = 8, stop bits = 1, and parity = N. Below are example screen shots of setting the port and such transmission parameters (in short described as  115200-8-1-N) on Windows and Linux. Remember to select exactly the same serial port you have identified in steps above.

.. figure:: ../../_static/putty-settings-windows.png
    :align: center
    :alt: Setting Serial Communication in PuTTY on Windows
    :figclass: align-center

    Setting Serial Communication in PuTTY on Windows

.. figure:: ../../_static/putty-settings-linux.png
    :align: center
    :alt: Setting Serial Communication in PuTTY on Linux
    :figclass: align-center

    Setting Serial Communication in PuTTY on Linux


Then open serial port in terminal and check, if you see any log printed out by ESP32. The log contents will depend on application loaded to ESP32. An example log by ESP32 is shown below.

.. highlight:: none

::

    ets Jun  8 2016 00:22:57

    rst:0x5 (DEEPSLEEP_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    ets Jun  8 2016 00:22:57

    rst:0x7 (TG0WDT_SYS_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    configsip: 0, SPIWP:0x00
    clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
    mode:DIO, clock div:2
    load:0x3fff0008,len:8
    load:0x3fff0010,len:3464
    load:0x40078000,len:7828
    load:0x40080000,len:252
    entry 0x40080034
    I (44) boot: ESP-IDF v2.0-rc1-401-gf9fba35 2nd stage bootloader
    I (45) boot: compile time 18:48:10

    ...

If you can see readable log output, it means serial connection is working and you are ready to proceed with installation and finally upload of application to ESP32.

.. note::

   For some serial port wiring configurations, the serial RTS & DTR pins need to be disabled in the terminal program before the ESP32 will boot and produce serial output. This depends on the hardware itself, most development boards (including all Espressif boards) *do not* have this issue. The issue is present if RTS & DTR are wired directly to the EN & GPIO0 pins. See the `esptool documentation`_ for more details.

.. note::

   Close serial terminal after verification that communication is working. In the next step we are going to use a different application to upload a new firmware to ESP32. This application will not be able to access serial port while it is open in terminal.

If you got here from :ref:`get-started-connect` when installing s/w for ESP32 development, then you can continue with :ref:`get-started-configure`.

.. _esptool documentation: https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection#automatic-bootloader
