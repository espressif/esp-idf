***********************************************
Device Firmware Upgrade through USB
***********************************************

.. only:: esp32

    .. note::
        Device Firmware Upgrade through USB is not supported with ESP32 chips.

Device Firmware Upgrade (DFU) is a mechanism for upgrading the firmware of devices through Universal Serial Bus (USB).
DFU is supported by ESP32-S2 chips. The necessary connections for the USB peripheral are shown in the following table.

+------+-------------+
| GPIO | USB         |
+======+=============+
| 19   | D- (green)  |
+------+-------------+
| 20   | D+ (white)  |
+------+-------------+
| GND  | GND (black) |
+------+-------------+
|      | +5V (red)   |
+------+-------------+

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

.. _api_guide_dfu_flash_errors:

Common errors
-------------

- ``dfu-util: command not found`` might indicate that the tool hasn't been installed or is not available from the terminal.
  An easy way of checking the tool is running ``dfu-util --version``. Please see :ref:`get-started-get-prerequisites` for
  installing ``dfu-util``.
- The reason for ``No DFU capable USB device available`` could be that the USB driver wasn't properly installed on
  Windows (see :ref:`api_guide_dfu_flash_win`) or udev rule was not setup on Linux
  (see :ref:`api_guide_dfu_flash_udev`).
