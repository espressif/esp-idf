Device Firmware Upgrade via USB
===============================

:link_to_translation:`zh_CN:[中文]`

.. only:: not SOC_USB_SERIAL_JTAG_SUPPORTED

    Typically, the firmware of {IDF_TARGET_NAME} is flashed via the chip's serial port. However, flashing via the serial port requires a USB to serial converter chip (e.g., CP210x or FTDI) to be connected to {IDF_TARGET_NAME}. Please see :doc:`Establish Serial Connection with {IDF_TARGET_NAME} <../get-started/establish-serial-connection>` for more details. {IDF_TARGET_NAME} contains a USB OTG peripheral, making it possible to connect {IDF_TARGET_NAME} to the host directly via USB (thus not requiring a USB to serial converter chip).

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    Typically, the firmware of {IDF_TARGET_NAME} is flashed via the chip's serial port or USB_SERIAL_JTAG (see :doc:`Establish Serial Connection with {IDF_TARGET_NAME} <../get-started/establish-serial-connection>` for more details). {IDF_TARGET_NAME} also contains a USB OTG peripheral making it possible to connect {IDF_TARGET_NAME} to the host directly via USB Device Firmware Upgrade.

.. only:: esp32s3

    By default, the :doc:`USB_SERIAL_JTAG <usb-serial-jtag-console>` module is connected to {IDF_TARGET_NAME}'s internal USB PHY, while the USB OTG peripheral can be used only if an external USB PHY is connected. Since DFU is provided via the USB OTG peripheral, it cannot be used through the internal PHY in this configuration.

    However, you can permanently switch the internal USB PHY to work with USB OTG peripheral instead of USB_SERIAL_JTAG by burning the ``USB_PHY_SEL`` eFuse. See *{IDF_TARGET_NAME} Technical Reference Manual* [`PDF <{IDF_TARGET_TRM_EN_URL}>`__] for more details about USB_SERIAL_JTAG and USB OTG.

Device Firmware Upgrade (DFU) is a mechanism for upgrading the firmware of {IDF_TARGET_NAME} directly via the Universal Serial Bus (USB). However, enabling Secure Boot or flash encryption disables the USB-OTG USB stack in the ROM, disallowing updates via the serial emulation or DFU on that port.

- :ref:`get-started-get-prerequisites` of the Getting Started Guide introduces the software requirements of DFU.
- Section :ref:`api_guide_dfu_build` describes how to build firmware for DFU with ESP-IDF.
- Section :ref:`api_guide_dfu_flash` deals with flashing the firmware.


USB Connection
--------------

.. only:: esp32p4

    {IDF_TARGET_NAME} routes the USB D+ and D- signals to their dedicated pins. For USB device functionality, these pins must be connected to the USB bus (e.g., via a Micro-B port, USB-C port, or directly to standard-A plug).

.. only:: esp32s2 or esp32s3

    The necessary connections for {IDF_TARGET_NAME}'s internal USB PHY (transceiver) are shown in the following table:

    .. list-table::
       :header-rows: 1
       :widths: 25 20

       * - GPIO
         - USB

       * - 20
         - D+ (green)

       * - 19
         - D- (white)

       * - GND
         - GND (black)

       * - +5V
         - +5V (red)

.. warning::

    Some cables are wired up with non-standard colors and some drivers are able to work with swapped D+ and D- connections. Please try to swap the cables connecting to D+ and D- if your device is not detected.

.. note::

    {IDF_TARGET_NAME} chip needs to be in bootloader mode before it can be detected as a DFU device and flash. Please refer to `Boot Mode Selection <https://docs.espressif.com/projects/esptool/en/latest/{IDF_TARGET_PATH_NAME}/advanced-topics/boot-mode-selection.html#select-bootloader-mode>`_ for more information about how to enter bootloader mode.


.. _api_guide_dfu_build:

Building the DFU Image
----------------------

The command below will create a DFU image named ``dfu.bin`` that is placed in the project's ``build`` directory::

    idf.py dfu

.. note::

    Do not forget to set the target chip by ``idf.py set-target`` before running ``idf.py dfu``. Otherwise, you might create an image for a different chip or receive an error message like ``unknown target 'dfu'``.


.. _api_guide_dfu_flash:

Flashing the DFU Image
----------------------

The command below will download the DFU image into {IDF_TARGET_NAME}::

    idf.py dfu-flash

The command relies on `dfu-util <http://dfu-util.sourceforge.net/>`_. Please see :ref:`get-started-get-prerequisites` for installing ``dfu-util``. ``dfu-util`` needs additional setup for :ref:`api_guide_dfu_flash_win` or setting up an :ref:`api_guide_dfu_flash_udev`. macOS users should be able to use ``dfu-util`` without further setup.

If there are more boards with the same chip connected then ``idf.py dfu-list`` can be used to list the available devices, for example::

    Found Runtime: [303a:0002] ver=0723, devnum=4, cfg=1, intf=2, path="1-10", alt=0, name="UNKNOWN", serial="0"
    Found Runtime: [303a:0002] ver=0723, devnum=6, cfg=1, intf=2, path="1-2", alt=0, name="UNKNOWN", serial="0"

Consequently, the desired device can be selected for flashing by the ``--path`` argument. For example, the devices listed above can be flashed individually by the following commands::

    idf.py dfu-flash --path 1-10
    idf.py dfu-flash --path 1-2

.. note::

    The vendor and product identificators are set based on the selected chip target by the ``idf.py set-target`` command and they are not selectable during the ``idf.py dfu-flash`` call.

See :ref:`api_guide_dfu_flash_errors` and their solutions.


.. _api_guide_dfu_flash_udev:

Udev Rule (Linux Only)
----------------------

Udev is a device manager for the Linux kernel. It allows running ``dfu-util`` (and ``idf.py dfu-flash``) without ``sudo`` for gaining access to the chip.

Create file ``/etc/udev/rules.d/40-dfuse.rules`` with the following content::

    SUBSYSTEMS=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="00??", GROUP="plugdev", MODE="0666"

.. note::

    Please check the output of the command ``groups``. You need to be a member of the `GROUP` specified above. You may use some other existing groups for this purpose (e.g., `uucp` on some systems instead of `plugdev`) or create a new group for this purpose.

Restart your computer so the previous setting could take into affect or run ``sudo udevadm trigger`` to force manually udev to trigger your new rule.


.. _api_guide_dfu_flash_win:

USB Drivers (Windows Only)
--------------------------

``dfu-util`` uses `libusb` to access the device. On Windows, the `WinUSB` driver is the recommended driver which has to be installed for the device to work properly. For more details please see the `libusb wiki <https://github.com/libusb/libusb/wiki/Windows#How_to_use_libusb_on_Windows>`_.

.. only:: esp32s2

    The development board driver can be downloaded from https://github.com/espressif/esp-win-usb-drivers/releases. The files need to be extracted and `installed <https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/using-an-inf-file-to-install-a-file-system-filter-driver#right-click-install>`_. This should change or install the WinUSB driver for the right interface of the device.

.. note::

    If the feature is not working please proceed with the manual driver assignment. Otherwise, the following section can be skipped.

USB Drivers (Windows Only) - manual driver assignment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Manual driver assignment can be performed with the `Zadig tool <https://zadig.akeo.ie/>`_. Please make sure that the device is in download mode before running the tool and that the {IDF_TARGET_NAME} device is detected before the driver installation.

The Zadig tool might detect several USB interfaces of {IDF_TARGET_NAME}. Please install the `WinUSB` driver **only** for the interface where there is no driver installed (probably it is Interface 2) and do not re-install the driver for the other interface.

.. warning::

    The manual installation of the driver in Device Manager of Windows is not recommended because the flashing might not work properly.


.. _api_guide_dfu_flash_errors:

Common Errors and Known Issues
------------------------------

- ``dfu-util: command not found`` might indicate that the tool has not been installed or is not available from the terminal. An easy way of checking the tool is running ``dfu-util --version``. Please see :ref:`get-started-get-prerequisites` for installing ``dfu-util``.

- The reason for ``No DFU capable USB device available`` could be that the USB driver was not properly installed on Windows (see :ref:`api_guide_dfu_flash_win`), udev rule was not setup on Linux (see :ref:`api_guide_dfu_flash_udev`) or the device is not in bootloader mode.

- Flashing with ``dfu-util`` on Windows fails on the first attempt with error ``Lost device after RESET?``. Please retry the flashing and it should succeed the next time.


.. only:: SOC_SUPPORTS_SECURE_DL_MODE

    Secure Download Mode
    --------------------

    When Secure Download Mode is enabled, DFU is no longer possible. Please see :doc:`Flash Encryption <../security/flash-encryption>` guide for more details.
