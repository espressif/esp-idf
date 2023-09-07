.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: devkit-defs
   :end-before: ---

Configure |devkit-name| JTAG Interface
======================================

:link_to_translation:`zh_CN:[中文]`

All versions of |devkit-name| boards have built-in JTAG functionality. Putting it to work requires setting jumpers or DIP switches to enable JTAG functionality, and configuring USB drivers. Please refer to step by step instructions below.

Configure Hardware
^^^^^^^^^^^^^^^^^^

.. include:: {IDF_TARGET_PATH_NAME}.inc
    :start-after: devkit-hw-config
    :end-before: ---

* Verify if {IDF_TARGET_NAME} pins used for JTAG communication are not connected to some other h/w that may disturb JTAG operation:

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: jtag-pins
        :end-before: ---

Configure USB Drivers
^^^^^^^^^^^^^^^^^^^^^

Install and configure USB drivers, so OpenOCD is able to communicate with JTAG interface on |devkit-name| board as well as with UART interface used to upload application for flash. Follow steps below specific to your operating system.

.. note:: |devkit-name| uses an FT2232 adapter. The following instructions can also be used for other FT2232 based JTAG adapters.


Windows
"""""""

1.  Using standard USB A/micro USB B cable connect |devkit-name| to the computer. Switch the |devkit-name| on.

2.  Wait until USB ports of |devkit-name| are recognized by Windows and drives are installed. If they do not install automatically, then download them from https://ftdichip.com/drivers/d2xx-drivers/ and install manually.

3.  Download Zadig tool (Zadig_X.X.exe) from https://zadig.akeo.ie/ and run it.

4.  In Zadig tool go to "Options" and check "List All Devices".

5.  Check the list of devices that should contain two |devkit-name| specific USB entries: "Dual RS232-HS (Interface 0)" and "Dual RS232-HS (Interface 1)". The driver name would be "FTDIBUS (vxxxx)" and USB ID: 0403 6010.

    .. figure:: ../../../_static/jtag-usb-configuration-zadig.jpg
        :align: center
        :alt: Configuration of JTAG USB driver in Zadig tool
        :figclass: align-center

        Configuration of JTAG USB driver in Zadig tool

6.  The first device (Dual RS232-HS (Interface 0)) is connected to the JTAG port of the {IDF_TARGET_NAME}. Original "FTDIBUS (vxxxx)" driver of this device should be replaced with "WinUSB (v6xxxxx)". To do so, select "Dual RS232-HS (Interface 0) and reinstall attached driver to the "WinUSB (v6xxxxx)", see picture above.

.. note::

    Do not change the second device "Dual RS232-HS (Interface 1)". It is routed to {IDF_TARGET_NAME}'s serial port (UART) used for upload of application to {IDF_TARGET_NAME}'s flash.

Now |devkit-name|'s JTAG interface should be available to the OpenOCD. To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.


Linux
"""""

1.  Using standard USB A/micro USB B cable connect |devkit-name| board to the computer. Power on the board.

.. highlight:: none

2.  Open a terminal, enter ``ls -l /dev/ttyUSB*`` command and check, if board's USB ports are recognized by the OS. You are looking for similar result:

    ::

        user-name@computer-name:~/esp$ ls -l /dev/ttyUSB*
        crw-rw---- 1 root dialout 188, 0 Jul 10 19:04 /dev/ttyUSB0
        crw-rw---- 1 root dialout 188, 1 Jul 10 19:04 /dev/ttyUSB1


3.  To set up access permissions to USB devices supported by OpenOCD, copy the `udev rules file <https://github.com/espressif/openocd-esp32/blob/master/contrib/60-openocd.rules>`_ into the ``/etc/udev/rules.d`` directory.

4.  Log off and login, then cycle the power to the board to make the changes effective. In terminal enter again ``ls -l /dev/ttyUSB*`` command to verify, if group-owner has changed from ``dialout`` to ``plugdev``:

    ::

        user-name@computer-name:~/esp$ ls -l /dev/ttyUSB*
        crw-rw-r-- 1 root plugdev 188, 0 Jul 10 19:07 /dev/ttyUSB0
        crw-rw-r-- 1 root plugdev 188, 1 Jul 10 19:07 /dev/ttyUSB1

    If you see similar result and you are a member of ``plugdev`` group, then the set up is complete.

    The ``/dev/ttyUSBn`` interface with lower number is used for JTAG communication. The other interface is routed to {IDF_TARGET_NAME}'s serial port (UART) used for upload of application to {IDF_TARGET_NAME}'s flash.

Now |devkit-name|'s JTAG interface should be available to the OpenOCD. To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.


MacOS
"""""

On macOS, using FT2232 for JTAG and serial port at the same time needs some additional steps. When the OS loads FTDI serial port driver, it does so for both channels of FT2232 chip. However only one of these channels is used as a serial port, while the other is used as JTAG. If the OS has loaded FTDI serial port driver for the channel used for JTAG, OpenOCD will not be able to connect to the chip. There are two ways around this:

1. Manually unload the FTDI serial port driver before starting OpenOCD, start OpenOCD, then load the serial port driver.

2. Modify FTDI driver configuration so that it does not load itself for channel A of FT2232 chip, which is the channel used for JTAG on |devkit-name|.

Manually unloading the driver
.............................

1. Install FTDI driver from https://ftdichip.com/drivers/vcp-drivers/

2. Connect USB cable to the |devkit-name|.

3. Unload the serial port driver::

    sudo kextunload -b com.FTDI.driver.FTDIUSBSerialDriver

   In some cases you may need to unload Apple's FTDI driver as well:

   * macOS < 10.15::

        sudo kextunload -b com.apple.driver.AppleUSBFTDI

   * macOS 10.15::

        sudo kextunload -b com.apple.DriverKit-AppleUSBFTDI

   .. warning::

      Attempting to use serial over the wrong channel with the FTDI driver will cause a kernel panic. The ESP-WROVER-KIT uses channel A for JTAG and channel B for serial.

4. Run OpenOCD:

   .. include:: {IDF_TARGET_PATH_NAME}.inc
       :start-after: run-openocd
       :end-before: ---

5. In another terminal window, load FTDI serial port driver again::

    sudo kextload -b com.FTDI.driver.FTDIUSBSerialDriver

.. note::

   If you need to restart OpenOCD, there is no need to unload FTDI driver again — just stop OpenOCD and start it again. The driver only needs to be unloaded if |devkit-name| was reconnected or power was toggled.

This procedure can be wrapped into a shell script, if desired.

Modifying FTDI driver
.....................

In a nutshell, this approach requires modification to FTDI driver configuration file, which prevents the driver from being loaded for channel B of FT2232H.

.. note::

    Other boards may use channel A for JTAG, so use this option with caution.

.. warning::

    This approach also needs signature verification of drivers to be disabled, so may not be acceptable for all users.


1. Open FTDI driver configuration file using a text editor (note ``sudo``)::

    sudo nano /Library/Extensions/FTDIUSBSerialDriver.kext/Contents/Info.plist

2. Find and delete the following lines::

        <key>FT2232H_B</key>
        <dict>
            <key>CFBundleIdentifier</key>
            <string>com.FTDI.driver.FTDIUSBSerialDriver</string>
            <key>IOClass</key>
            <string>FTDIUSBSerialDriver</string>
            <key>IOProviderClass</key>
            <string>IOUSBInterface</string>
            <key>bConfigurationValue</key>
            <integer>1</integer>
            <key>bInterfaceNumber</key>
            <integer>1</integer>
            <key>bcdDevice</key>
            <integer>1792</integer>
            <key>idProduct</key>
            <integer>24592</integer>
            <key>idVendor</key>
            <integer>1027</integer>
        </dict>

3. Save and close the file

4. Disable driver signature verification:

   1. Open Apple logo menu, choose "Restart..."

   2. When you hear the chime after reboot, press CMD+R immediately

   3. Once Recovery mode starts up, open Terminal

   4. Run the command::

       csrutil enable --without kext

   5. Restart again

After these steps, serial port and JTAG can be used at the same time.

To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.
