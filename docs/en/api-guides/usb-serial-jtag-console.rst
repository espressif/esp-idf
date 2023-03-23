**********************************
USB Serial/JTAG Controller Console
**********************************

On chips with an integrated USB Serial/JTAG Controller, it is possible to use the part of this controller that implements a serial port (CDC) to implement the serial console, instead of using UART with an external USB-UART bridge chip. {IDF_TARGET_NAME} contains this controller, providing the following functions:

* Bidirectional serial console, which can be used with :doc:`IDF Monitor <tools/idf-monitor>` or another serial monitor.
* Flashing using ``esptool.py`` and ``idf.py flash``.
* JTAG debugging using e.g. OpenOCD, simultaneous with serial operations.

Note that, in contrast with the USB OTG peripheral in some Espressif chips, the USB Serial/JTAG Controller is a fixed function device, implemented entirely in hardware. This means it cannot be reconfigured to perform any function other than to provide a serial channel and JTAG debugging functionality.

Hardware Requirements
=====================

{IDF_TARGET_USB_DP_GPIO:default="Not Updated!",esp32c3="19",esp32s3="20"}
{IDF_TARGET_USB_DM_GPIO:default="Not Updated!",esp32c3="18",esp32s3="19"}

Connect {IDF_TARGET_NAME} to the USB port as follows:

+------+-------------+
| GPIO | USB         |
+======+=============+
| {IDF_TARGET_USB_DP_GPIO}   | D+ (green)  |
+------+-------------+
| {IDF_TARGET_USB_DM_GPIO}   | D- (white)  |
+------+-------------+
| GND  | GND (black) |
+------+-------------+
|      | +5V (red)   |
+------+-------------+

Some development boards may offer a USB connector for the USB Serial/JTAG Controller — in that case, no extra connections are required.

Software Configuration
======================

USB console feature can be enabled using ``CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG`` option in menuconfig tool (see :ref:`CONFIG_ESP_CONSOLE_UART`).

Once the option is enabled, build the project as usual.

Alternatively, you can access the output through usb_serial_jtag port but make sure the option ``CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG`` in choice ``ESP_CONSOLE_SECONDARY`` is selected.

.. warning::

    Besides output, if you also want to input or use REPL with console, please select ``CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG``.

Uploading the Application
=========================

The USB Serial/JTAG Controller is able to put the {IDF_TARGET_NAME} into download mode automatically. Simply flash as usual, but specify the USB Serial/JTAG Controller port on your system: ``idf.py flash -p PORT`` where ``PORT`` is the name of the proper port.

Limitations
===========

There are several limitations to the USB Serial/JTAG console feature. These may or may not be significant, depending on the type of application being developed, and the development workflow.

{IDF_TARGET_BOOT_PIN:default = "Not Updated!", esp32c3 = "GPIO9", esp32s3 = "GPIO0"}

1. If the application accidentally reconfigures the USB peripheral pins, or disables the USB Serial/JTAG Controller, the device will disappear from the system. After fixing the issue in the application, you will need to manually put the {IDF_TARGET_NAME} into download mode by pulling low {IDF_TARGET_BOOT_PIN} and resetting the chip.

2. If the application enters deep sleep mode, the USB Serial/JTAG device will disappear from the system.

3. For data sent in the direction of {IDF_TARGET_NAME} to PC Terminal (e.g. stdout, logs), the {IDF_TARGET_NAME} first writes to a small internal buffer. If this buffer becomes full (for example, if no PC Terminal is connected), the {IDF_TARGET_NAME} will do a one-time wait of 50ms hoping for the PC Terminal to request the data. This can appear as a very brief 'pause' in your application.

4. For data sent in the PC Terminal to {IDF_TARGET_NAME} direction (e.g. console commands), many PC Terminals will wait for the {IDF_TARGET_NAME} to ingest the bytes before allowing you to sending more data. This is in contrast to using a USB-to-Serial (UART) bridge chip, which will always ingest the bytes and send them to a (possibly not listening) {IDF_TARGET_NAME}.

5. The USB Serial/JTAG device won't work in sleep modes as normal due to the lack of APB clock in sleep modes. This includes deep-sleep, light-sleep (automataic light-sleep as well).

6. The power consumption in sleep modes will be higher if the USB Serial/JTAG device is in use.

   This is because we want to keep the USB Serial/JTAG device alive during software reset by default.

   However there is an issue that this might also increase the power consumption in sleep modes. This is because the software keeps a clock source on during the reset to keep the USB Serial/JTAG device alive. As a side-effect, the clock is also kept on during sleep modes. There is one exception: the clock will only be kept on when your USB Serial/JTAG port is really in use (like data transaction), therefore, if your USB Serial/JTAG is connected to power bank or battery, etc., instead of a valid USB host (for example, a PC), the power consumption will not increase.

   If you still want to keep low power consumption in sleep modes:

    1. If you are not using the USB Serial/JTAG port, you don't need to do anything. Software will detect if the USB Serial/JTAG is connected to a valid host before going to sleep, and keep the clocks only when the host is connected. Otherwise the clocks will be turned off as normal.

    2. If you are using the USB Serial/JTAG port, please disable the menuconfig option ``CONFIG_RTC_CLOCK_BBPLL_POWER_ON_WITH_USB``. The clock will be switched off as normal during software reset and in sleep modes. In these cases, the USB Serial/JTAG device may be unplugged from the host.