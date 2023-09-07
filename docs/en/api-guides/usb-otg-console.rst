***************
USB OTG Console
***************

On chips with an integrated USB peripheral, it is possible to use USB Communication Device Class (CDC) to implement the serial console, instead of using UART with an external USB-UART bridge chip. {IDF_TARGET_NAME} ROM code contains a USB CDC implementation, which supports for some basic functionality without requiring the application to include the USB stack:

* Bidirectional serial console, which can be used with :doc:`IDF Monitor <tools/idf-monitor>` or another serial monitor
* Flashing using ``esptool.py`` and ``idf.py flash``.
* :doc:`Device Firmware Update (DFU) <dfu>` interface for flashing the device using ``dfu-util`` and ``idf.py dfu``.

.. note::

    At the moment, this "USB Console" feature is incompatible with TinyUSB stack. However, if TinyUSB is used, it can provide its own CDC implementation.

Hardware Requirements
=====================

Connect {IDF_TARGET_NAME} to the USB port as follows

+------+-------------+
| GPIO | USB         |
+======+=============+
| 20   | D+ (green)  |
+------+-------------+
| 19   | D- (white)  |
+------+-------------+
| GND  | GND (black) |
+------+-------------+
|      | +5V (red)   |
+------+-------------+

Some development boards may offer a USB connector for the internal USB peripheral — in that case, no extra connections are required.

.. only:: esp32s3

    By default, :doc:`USB_SERIAL_JTAG <usb-serial-jtag-console>` module is connected to the internal PHY of the ESP32-S3, while USB_OTG peripheral can be used only if the external USB PHY is connected. Since CDC console is provided via USB_OTG peripheral, it cannot be used through the internal PHY in this configuration.

    You can permanently switch the internal USB PHY to work with USB_OTG peripheral instead of USB_SERIAL_JTAG by burning ``USB_PHY_SEL`` eFuse. See ESP32-S3 Technical Reference Manual for more details about USB_SERIAL_JTAG and USB_OTG.

    Note however that USB_SERIAL_JTAG also provides a CDC console, so enabling the CDC console should not be the primary reason for switching from USB_SERIAL_JTAG to USB_CDC.


Software Configuration
======================

USB console feature can be enabled using ``CONFIG_ESP_CONSOLE_USB_CDC`` option in menuconfig tool (see :ref:`CONFIG_ESP_CONSOLE_UART`).

Once the option is enabled, build the project as usual.

Uploading the Application
=========================

.. _usb_console_initial_upload:

Initial Upload
--------------

If the {IDF_TARGET_NAME} is not yet flashed with a program that enables a USB console, an initial upload of the program is required. There are 3 alternative options to perform the initial upload.

Once the initial upload is done, the application will start up and a USB CDC port will appear in the system.

.. note::

    The port name may change after the initial upload, so check the port list again before running ``idf.py monitor``.


Initial Upload Using the ROM Download Mode, over USB CDC
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Place {IDF_TARGET_NAME} into download mode. To do this, keep GPIO0 low while toggling reset. On many development boards, the "Boot" button is connected to GPIO0, and you can press "Reset" button while holding "Boot".
* A serial port will appear in the system. On most operating systems (Windows 8 and later, Linux, macOS) driver installation is not required. Find the port name using Device Manager (Windows) or by listing ``/dev/ttyACM*`` devices on Linux or ``/dev/cu*`` devices on macOS.
* Run ``idf.py flash -p PORT`` to upload the application, with ``PORT`` determined in the previous step.

Initial Upload Using the ROM Download Mode, over USB DFU
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Place {IDF_TARGET_NAME} into download mode. To do this, keep GPIO0 low while toggling reset. On many development boards, the "Boot" button is connected to GPIO0, and you can press "Reset" button while holding "Boot".
* Run ``idf.py dfu-flash``.

See :ref:`api_guide_dfu_flash` for details about DFU flashing.

Initial Upload Using UART
^^^^^^^^^^^^^^^^^^^^^^^^^

On development boards with a USB-UART bridge, upload the application over UART: ``idf.py flash -p PORT`` where ``PORT`` is the name of the serial port provided by the USB-UART bridge.

Subsequent Usage
----------------

Once the application is uploaded for the first time, you can run ``idf.py flash`` and ``idf.py monitor`` as usual.

Limitations
===========

There are several limitations to the USB console feature. These may or may not be significant, depending on the type of application being developed, and the development workflow. Most of these limitations stem from the fact that USB CDC is implemented in software, so the console working over USB CDC is more fragile and complex than a console working over UART.

1. If the application crashes, panic handler output may not be sent over USB CDC in some cases. If the memory used by the CDC driver is corrupted, or there is some other system-level issue, CDC may not work for sending panic handler messages over USB. This does work in many situations, but is not guaranteed to work as reliably as the UART output does. Similarly, if the application enters a boot loop before the USB CDC driver has a chance to start up, there will be no console output.

2. If the application accidentally reconfigures the USB peripheral pins, or disables the USB peripheral, USB CDC device will disappear from the system. After fixing the issue in the application, you will need to follow the :ref:`usb_console_initial_upload` process to flash the application again.

3. If the application enters light sleep (including automatic light sleep) or deep sleep mode, USB CDC device will disappear from the system.

4. USB CDC driver reserves some amount of RAM and increases application code size. Keep this in mind if trying to optimize application memory usage.

5. By default, the low-level ``esp_rom_printf`` feature and ``ESP_EARLY_LOG`` are disabled when USB CDC is used. These can be enabled using :ref:`CONFIG_ESP_CONSOLE_USB_CDC_SUPPORT_ETS_PRINTF` option. With this option enabled, ``esp_rom_printf`` can be used, at the expense of increased IRAM usage. Keep in mind that the cost of ``esp_rom_printf`` and ``ESP_EARLY_LOG`` over USB CDC is significantly higher than over UART. This makes these logging mechanisms much less suitable for "printf debugging", especially in the interrupt handlers.

6. If you are developing an application which uses the USB peripheral with the TinyUSB stack, this USB Console feature can not be used. This is mainly due to the following reasons:

   * This feature relies on a different USB CDC software stack in {IDF_TARGET_NAME} ROM.
   * USB descriptors used by the ROM CDC stack may be different from the descriptors used by TinyUSB.
   * When developing applications which use USB peripheral, it is very likely that USB functionality will not work or will not fully work at some moments during development. This can be due to misconfigured USB descriptors, errors in the USB stack usage, or other reasons. In this case, using the UART console for flashing and monitoring provides a much better development experience.

7. When debugging the application using JTAG, USB CDC may stop working if the CPU is stopped on a breakpoint. USB CDC operation relies on interrupts from the USB peripheral being serviced periodically. If the host computer does not receive valid responses from the USB device side for some time, it may decide to disconnect the device. The actual time depends on the OS and the driver, and ranges from a few hundred milliseconds to a few seconds.

