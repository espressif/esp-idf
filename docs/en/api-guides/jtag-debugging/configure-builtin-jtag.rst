.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: devkit-defs
   :end-before: ---


Configure {IDF_TARGET_NAME} built-in JTAG Interface
===================================================

{IDF_TARGET_NAME} has a built-in JTAG circuitry and can be debugged without any additional chip. Only an USB cable
connected to the D+/D- pins is necessary. The necessary connections are shown in the following section.

Configure Hardware
^^^^^^^^^^^^^^^^^^

.. list-table:: ESP32-C3 pins and USB signals
    :widths: 25 75
    :header-rows: 1

    * - ESP32-C3 Pin
      - USB Signal
    * - GPIO18
      - D-
    * - GPIO19
      - D+
    * - 5V
      - V_BUS
    * - GND
      - Ground

Please verify that the {IDF_TARGET_NAME} pins used for USB communication are not connected to some other HW that may disturb the JTAG operation.

Configure USB Drivers
^^^^^^^^^^^^^^^^^^^^^

JTAG communication should work out-of-box on Linux and macOS. Windows users might get `LIBUSB_ERROR_NOT_FOUND` errors.
This is a known issue and will be addressed soon.
