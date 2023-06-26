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

{IDF_TARGET_USB_DP_GPIO:default="Not Updated!",esp32c3="19",esp32s3="20", esp32c6="13", esp32h2="27"}
{IDF_TARGET_USB_DM_GPIO:default="Not Updated!",esp32c3="18",esp32s3="19", esp32c6="12", esp32h2="26"}

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

{IDF_TARGET_BOOT_PIN:default = "Not Updated!", esp32c3 = "GPIO9", esp32s3 = "GPIO0", esp32c6 = "GPIO9"}

1. If the application accidentally reconfigures the USB peripheral pins, or disables the USB Serial/JTAG Controller, the device will disappear from the system. After fixing the issue in the application, you will need to manually put the {IDF_TARGET_NAME} into download mode by pulling low {IDF_TARGET_BOOT_PIN} and resetting the chip.

2. If the application enters deep sleep mode, the USB Serial/JTAG device will disappear from the system.

3. For data sent in the direction of {IDF_TARGET_NAME} to PC Terminal (e.g. stdout, logs), the {IDF_TARGET_NAME} first writes to a small internal buffer. If this buffer becomes full (for example, if no PC Terminal is connected), the {IDF_TARGET_NAME} will do a one-time wait of 50ms hoping for the PC Terminal to request the data. This can appear as a very brief 'pause' in your application.

4. For data sent in the PC Terminal to {IDF_TARGET_NAME} direction (e.g. console commands), many PC Terminals will wait for the {IDF_TARGET_NAME} to ingest the bytes before allowing you to sending more data. This is in contrast to using a USB-to-Serial (UART) bridge chip, which will always ingest the bytes and send them to a (possibly not listening) {IDF_TARGET_NAME}.

.. only:: not SOC_USB_SERIAL_JTAG_SUPPORT_LIGHT_SLEEP

    5. The USB Serial/JTAG controller will not work during sleep (both light and deep sleep) due to the lack of an APB and USB PHY clock during sleep. Thus, entering sleep has the following implications on the USB Serial/JTAG controller:

        i. Both the APB clock and the USB PHY clock (derived form the main PLL clock) will be disabled during sleep. As a result, the USB Serial/JTAG controller will not be able receive or respond to any USB transactions from the connected host (including periodic CDC Data IN transactions). Thus it may appear to the host that the USB Serial/JTAG controller has disconnected.

        ii. If users enter sleep manually (via :cpp:func:`esp_light_sleep_start` or :cpp:func:`esp_deep_sleep_start`), users should be cognizant of the fact that USB Serial/JTAG controller will not work during sleep. ESP-IDF **does not add any safety check to reject entry to sleep** even if the USB Serial/JTAG controller is connected. In the case where sleep is entered while the USB Serial/JTAG controller is connected, connection can be re-established by unplugging and re-plugging the USB cable.

        iii. If users enter sleep automatically (via :cpp:func:`esp_pm_configure`), enabling the :ref:`CONFIG_USJ_NO_AUTO_LS_ON_CONNECTION` option will allow the {IDF_TARGET_NAME} to automatically detect whether the USB Serial/JTAG controller is currently connected to a host, and prevent automatic entry to sleep as long as the connection persists. However, note that this option will increase power consumption.
