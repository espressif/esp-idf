Establish Serial Connection with {IDF_TARGET_NAME}
==================================================

:link_to_translation:`zh_CN:[中文]`

This section provides guidance how to establish serial connection between {IDF_TARGET_NAME} and PC.


Connect {IDF_TARGET_NAME} to PC
----------------------------------

Connect the {IDF_TARGET_NAME} board to the PC using the USB cable. If device driver does not install automatically, identify USB to serial converter chip on your {IDF_TARGET_NAME} board (or external converter dongle), search for drivers in internet and install them.

Below is the list of USB to serial converter chips installed on most of the {IDF_TARGET_NAME} boards produced by Espressif together with links to the drivers:

* CP210x: `CP210x USB to UART Bridge VCP Drivers <https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers>`_
* FTDI: `FTDI Virtual COM Port Drivers <https://www.ftdichip.com/Drivers/VCP.htm>`_

Please check the board user guide for specific USB to serial converter chip used. The drivers above are primarily for reference. Under normal circumstances, the drivers should be bundled with an operating system and automatically installed upon connecting the board to the PC.

Check port on Windows
---------------------

Check the list of identified COM ports in the Windows Device Manager. Disconnect {IDF_TARGET_NAME} and connect it back, to verify which port disappears from the list and then shows back again.

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


Check port on Linux and macOS
-----------------------------

To check the device name for the serial port of your {IDF_TARGET_NAME} board (or external converter dongle), run this command two times, first with the board / dongle unplugged, then with plugged in. The port which appears the second time is the one you need:

Linux ::

    ls /dev/tty*

macOS ::

    ls /dev/cu.*

.. note::

    macOS users: if you don't see the serial port then check you have the USB/serial drivers installed as shown in the Getting Started guide for your particular development board. For macOS High Sierra (10.13), you may also have to explicitly allow the drivers to load. Open System Preferences -> Security & Privacy -> General and check if there is a message shown here about "System Software from developer ..." where the developer name is Silicon Labs or FTDI.


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

Now verify that the serial connection is operational. You can do this using a serial terminal program by checking if you get any output on the terminal after reseting {IDF_TARGET_NAME}. 

Windows and Linux
^^^^^^^^^^^^^^^^^

In this example we will use `PuTTY SSH Client <http://www.putty.org/>`_ that is available for both Windows and Linux. You can use other serial program and set communication parameters like below.

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


Then open serial port in terminal and check, if you see any log printed out by {IDF_TARGET_NAME}. The log contents will depend on application loaded to {IDF_TARGET_NAME}, see `Example Output`_.

.. note::

   Close the serial terminal after verification that communication is working. If you keep the terminal session open, the serial port will be inaccessible for uploading firmware later.

macOS
^^^^^^^^

To spare you the trouble of installing a serial terminal program, macOS offers the **screen** command.

- As discussed in `Check port on Linux and macOS`_, run::

    ls /dev/cu.*

- You should see similar output::

    /dev/cu.Bluetooth-Incoming-Port /dev/cu.SLAB_USBtoUART      /dev/cu.SLAB_USBtoUART7

- The output will vary depending on the type and the number of boards connected to your PC. Then pick the device name of your board and run::

    screen /dev/cu.device_name 115200

  Replace ``device_name`` with the name found running ``ls /dev/cu.*``. 

- What you are looking for is some log displayed by the **screen**. The log contents will depend on application loaded to {IDF_TARGET_NAME}, see `Example Output`_. To exit the **screen** session type Ctrl-A + \\ .

.. note::

   Do not forget to **exit the screen session** after verifying that the communication is working. If you fail to do it and just close the terminal window, the serial port will be inaccessible for uploading firmware later.


Example Output
^^^^^^^^^^^^^^

An example log by {IDF_TARGET_NAME} is shown below. Reset the board if you do not see anything.

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

If you can see readable log output, it means serial connection is working and you are ready to proceed with installation and finally upload of application to {IDF_TARGET_NAME}.

.. note::

   For some serial port wiring configurations, the serial RTS & DTR pins need to be disabled in the terminal program before the {IDF_TARGET_NAME} will boot and produce serial output. This depends on the hardware itself, most development boards (including all Espressif boards) *do not* have this issue. The issue is present if RTS & DTR are wired directly to the EN & GPIO0 pins. See the `esptool documentation`_ for more details.

If you got here from :ref:`get-started-connect` when installing s/w for {IDF_TARGET_NAME} development, then you can continue with :ref:`get-started-configure`.

.. _esptool documentation: https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection#automatic-bootloader