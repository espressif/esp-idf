Flashing Troubleshooting
========================

:link_to_translation:`zh_CN:[中文]`

Failed to Connect
-----------------

{IDF_TARGET_STRAP_GPIO:default="[NEEDS TO BE UPDATED]", esp32="GPIO0", esp32s2="GPIO0", esp32s3="GPIO0", esp32c2="GPIO9", esp32c3="GPIO9", esp32c6="GPIO9", esp32h2="GPIO9", esp32p4="GPIO35", esp32c5="GPIO28", esp32c61="GPIO9"}

If you run the given command and see errors such as "Failed to connect", there might be several reasons for this. One of the reasons might be issues encountered by ``esptool``, the utility that is called by the build system to reset the chip, interact with the first stage (ROM) bootloader, and flash firmware. One simple solution to try is to manually reset as described below. If it does not help, you can find more details about possible issues in the `esptool troubleshooting <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>`_ page.

``esptool`` resets {IDF_TARGET_NAME} automatically by asserting DTR and RTS control lines of the USB-to-UART bridge, i.e., FTDI or CP210x (for more information, see :doc:`establish-serial-connection`). The DTR and RTS control lines are in turn connected to ``{IDF_TARGET_STRAP_GPIO}`` and ``CHIP_PU`` (EN) pins of {IDF_TARGET_NAME}, thus changes in the voltage levels of DTR and RTS will boot {IDF_TARGET_NAME} into Firmware Download mode. As an example, check the `schematic <https://dl.espressif.com/dl/schematics/esp32_devkitc_v4-sch-20180607a.pdf>`_ for the ESP32 DevKitC development board.

In general, you should have no problems with the `official esp-idf development boards <https://www.espressif.com/en/products/devkits>`_. However, ``esptool`` is not able to reset your hardware automatically in the following cases:

- Your hardware does not have the DTR and RTS lines connected to ``{IDF_TARGET_STRAP_GPIO}`` and ``CHIP_PU``.
- The DTR and RTS lines are configured differently.
- There are no such serial control lines at all.

Depending on the kind of hardware you have, it may also be possible to manually put your {IDF_TARGET_NAME} board into Firmware Download mode (reset).

- For development boards produced by Espressif, this information can be found in the respective getting started guides or user guides. For example, to manually reset an ESP-IDF development board, hold down the ``Boot`` button (``{IDF_TARGET_STRAP_GPIO}``) and press the ``EN`` button (``CHIP_PU``).
- For other types of hardware, try pulling ``{IDF_TARGET_STRAP_GPIO}`` down.
