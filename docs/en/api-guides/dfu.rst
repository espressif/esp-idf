***********************************************
Device Firmware Upgrade through USB
***********************************************

Device Firmware Upgrade (DFU) is a mechanism for upgrading the firmware of devices through Universal Serial Bus (USB).
DFU is supported by {IDF_TARGET_NAME} chips. The necessary connections for the USB peripheral are shown in the following table.

+------+-------------+
| GPIO | USB         |
+======+=============+
| 20   | D+ (green)  |
+------+-------------+
| 19   | D- (white)  |
+------+-------------+
| GND  | GND (black) |
+------+-------------+
| +5V  | +5V (red)   |
+------+-------------+

.. only:: esp32s3

    By default, :doc:`USB_SERIAL_JTAG<usb-serial-jtag-console>` module is connected to the internal PHY of the ESP32-S3, while USB_OTG peripheral can be used only if the external USB PHY is connected. Since DFU mode is provided via USB_OTG peripheral, it cannot be used through the internal PHY in this configuration.

    You can permanently switch the internal USB PHY to work with USB_OTG peripheral instead of USB_SERIAL_JTAG by burning ``USB_PHY_SEL`` eFuse. See ESP32-S3 Technical Reference Manual for more details about USB_SERIAL_JTAG and USB_OTG.

.. note::
    The {IDF_TARGET_NAME} chip needs to be in bootloader mode for the detection as a DFU device and flashing. This can be
    achieved by pulling GPIO0 down (e.g. pressing the BOOT button), pulsing RESET down for a moment and releasing
    GPIO0.

.. warning::
    Some cables are wired up with non-standard colors and some drivers are able to work with swapped D+ and D-
    connections. Please try to swap the cables connecting to D+ and D- if your device is not detected.

The software requirements of DFU are included in :ref:`get-started-get-prerequisites` of the Getting Started Guide.

Section :ref:`api_guide_dfu_build` describes how to build firmware for DFU with ESP-IDF and
Section :ref:`api_guide_dfu_flash` deals with flashing the firmware.

.. _api_guide_dfu_build:

Building the DFU Image
======================

The DFU image can be created by running::

    idf.py dfu

which creates ``dfu.bin`` in the build directory.

.. note::
    Don't forget to set the target chip by ``idf.py set-target`` before running ``idf.py dfu``. Otherwise, you might
    create an image for a different chip or receive an error message something like ``unknown target 'dfu'``.

.. _api_guide_dfu_flash:

Flashing the Chip with the DFU Image
====================================

The DFU image is downloaded into the chip by running::

    idf.py dfu-flash

which relies on `dfu-util <http://dfu-util.sourceforge.net/>`_. Please see :ref:`get-started-get-prerequisites` for
installing ``dfu-util``. ``dfu-util`` needs additional setup for :ref:`api_guide_dfu_flash_win` or setting up an
:ref:`api_guide_dfu_flash_udev`. Mac OS users should be able to use ``dfu-util`` without further setup.

If there are more boards with the same chip connected then ``idf.py dfu-list`` can be used to list the available
devices, for example::

    Found Runtime: [303a:0002] ver=0723, devnum=4, cfg=1, intf=2, path="1-10", alt=0, name="UNKNOWN", serial="0"
    Found Runtime: [303a:0002] ver=0723, devnum=6, cfg=1, intf=2, path="1-2", alt=0, name="UNKNOWN", serial="0"

Consequently, the desired device can be selected for flashing by the ``--path`` argument. For example, the devices
listed above can be flashed individually by the following commands::

    idf.py dfu-flash --path 1-10
    idf.py dfu-flash --path 1-2

.. note::
    The vendor and product identificators are set based on the selected chip target by the ``idf.py set-target``
    command and it is not selectable during the ``idf.py dfu-flash`` call.

See :ref:`api_guide_dfu_flash_errors` and their solutions.

.. _api_guide_dfu_flash_udev:

udev rule (Linux only)
----------------------

udev is a device manager for the Linux kernel. It allows us to run ``dfu-util`` (and ``idf.py dfu-flash``) without
``sudo`` for gaining access to the chip.

Create file ``/etc/udev/rules.d/40-dfuse.rules`` with the following content::

    SUBSYSTEMS=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="00??", GROUP="plugdev", MODE="0666"

.. note::
    Please check the output of command ``groups``. The user has to be a member of the `GROUP` specified above. You may
    use some other existing group for this purpose (e.g. `uucp` on some systems instead of `plugdev`) or create a new
    group for this purpose.

Restart your computer so the previous setting could take into affect or run ``sudo udevadm trigger`` to force
manually udev to trigger your new rule.

.. _api_guide_dfu_flash_win:

USB drivers (Windows only)
--------------------------

``dfu-util`` uses `libusb` to access the device. You have to register on Windows the device with the `WinUSB` driver.
Please see the `libusb wiki <https://github.com/libusb/libusb/wiki/Windows#How_to_use_libusb_on_Windows>`_ for more
details.

The drivers can be installed by the `Zadig tool <https://zadig.akeo.ie/>`_. Please make sure that the device is in
download mode before running the tool and that it detects the {IDF_TARGET_NAME} device before installing the drivers. The Zadig
tool might detect several USB interfaces of {IDF_TARGET_NAME}. Please install the WinUSB driver for only that interface for
which there is no driver installed (probably it is Interface 2) and don't re-install the driver for the other interface.

.. warning::
    The manual installation of the driver in Device Manager of Windows is not recommended because the flashing might
    not work properly.

.. _api_guide_dfu_flash_errors:

Common errors and known issues
------------------------------

- ``dfu-util: command not found`` might indicate that the tool hasn't been installed or is not available from the terminal.
  An easy way of checking the tool is running ``dfu-util --version``. Please see :ref:`get-started-get-prerequisites` for
  installing ``dfu-util``.
- The reason for ``No DFU capable USB device available`` could be that the USB driver wasn't properly installed on
  Windows (see :ref:`api_guide_dfu_flash_win`), udev rule was not setup on Linux
  (see :ref:`api_guide_dfu_flash_udev`) or the device isn't in bootloader mode.
- Flashing with ``dfu-util`` on Windows fails on the first attempt with error ``Lost device after RESET?``. Please
  retry the flashing and it should succeed the next time.
