Configure WROVER JTAG Interface
===============================

All versions of ESP32 WROVER KIT boards have JTAG functionality build in. Putting it to work requires setting  jumpers to enable JTAG functionality, setting SPI flash voltage and configuring USB drivers. Please refer to step by step instructions below.


Configure Hardware
^^^^^^^^^^^^^^^^^^

1.  Enable on-board JTAG functionality by setting JP8 according to :doc:`../../get-started/get-started-wrover-kit`, section :ref:`esp-wrover-setup-options`. 

2.  Verify if ESP32 pins used for JTAG communication are not connected to some other h/w that may disturb JTAG operation:

    +---+---------------+-------------+
    |   | ESP32 Pin     | JTAG Signal |
    +===+===============+=============+
    | 1 | CHIP_PU       | TRST_N      |
    +---+---------------+-------------+
    | 2 | MTDO / GPIO15 | TDO         |
    +---+---------------+-------------+
    | 3 | MTDI / GPIO12 | TDI         |
    +---+---------------+-------------+
    | 4 | MTCK / GPIO13 | TCK         |
    +---+---------------+-------------+
    | 5 | MTMS / GPIO14 | TMS         |
    +---+---------------+-------------+


Configure USB Drivers
^^^^^^^^^^^^^^^^^^^^^

Install and configure USB drivers, so OpenOCD is able to communicate with JTAG interface on ESP32 WROVER KIT board as well as with UART interface used to upload application for flash. Follow steps below specific to your operating system.


Windows
"""""""

1.  Using standard USB A / mini USB B cable connect ESP32 WROVER KIT to the computer. Switch the WROVER KIT on.

2.  Wait until USB ports of WROVER KIT are recognized by Windows and drives are installed. If they do not install automatically, then then download them from http://www.ftdichip.com/Drivers/D2XX.htm and install manually.

3.  Download Zadig tool (Zadig_X.X.exe) from http://zadig.akeo.ie/ and run it.

4.  In Zadig tool go to "Options" and check "List All Devices".

5.  Check the list of devices that should contain two WROVER specific USB entries: "Dual RS232-HS (Interface 0)" and "Dual RS232-HS (Interface 1)". The driver name would be "FTDIBUS (vxxxx)" and USB ID: 0403 6010.

    .. figure:: ../../_static/jtag-usb-configuration-zadig.jpg
        :align: center
        :alt: Configuration of JTAG USB driver in Zadig tool
        :figclass: align-center

        Configuration of JTAG USB driver in Zadig tool

6.  The first device (Dual RS232-HS (Interface 0)) is connected to the JTAG port of the ESP32. Original "FTDIBUS (vxxxx)" driver of this device should be replaced with "WinUSB (v6xxxxx)". To do so, select "Dual RS232-HS (Interface 0) and reinstall attached driver to the "WinUSB (v6xxxxx)", see picture above.

.. note::

    Do not change the second device "Dual RS232-HS (Interface 1)". It is routed to ESP32's serial port (UART) used for upload of application to ESP32's flash.

Now ESP32 WROVER KIT's JTAG interface should be available to the OpenOCD. To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.


Linux
"""""

1.  Using standard USB A / mini USB B cable connect ESP32 WROVER KIT board to the computer. Power on the board.

.. highlight:: none

2.  Open a terminal, enter ``ls -l /dev/ttyUSB*`` command and check, if board's USB ports are recognized by the O/S. You are looking for similar result:

    ::

        user-name@computer-name:~/esp$ ls -l /dev/ttyUSB*
        crw-rw---- 1 root dialout 188, 0 Jul 10 19:04 /dev/ttyUSB0
        crw-rw---- 1 root dialout 188, 1 Jul 10 19:04 /dev/ttyUSB1


3.  Following section "Permissions delegation" in OpenOCD's README, set up the access permissions to both USB ports.

4.  Log off and login, then cycle the power to the board to make the changes effective. In terminal enter again ``ls -l /dev/ttyUSB*`` command to verify, if group-owner has changed from ``dialout`` to ``plugdev``:

    ::

        user-name@computer-name:~/esp$ ls -l /dev/ttyUSB*
        crw-rw-r-- 1 root plugdev 188, 0 Jul 10 19:07 /dev/ttyUSB0
        crw-rw-r-- 1 root plugdev 188, 1 Jul 10 19:07 /dev/ttyUSB1

    If you see similar result and you are a member of ``plugdev`` group, then the set up is complete.

    The ``/dev/ttyUSBn`` interface with lower number is used for JTAG communication. The other interface is routed to ESP32's serial port (UART) used for upload of application to ESP32's flash.

Now ESP32 WROVER KIT's JTAG interface should be available to the OpenOCD. To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.


MacOS
"""""

[TBD]

Now ESP32 WROVER KIT's JTAG interface should be available to the OpenOCD. To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.
