.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: devkit-defs
   :end-before: ---


Configure {IDF_TARGET_NAME} built-in JTAG Interface
===================================================

{IDF_TARGET_JTAG_PIN_Dneg:default="Not Updated!", esp32c3="GPIO18", esp32s3="GPIO19"} 
{IDF_TARGET_JTAG_PIN_Dpos:default="Not Updated!", esp32c3="GPIO19", esp32s3="GPIO20"} 

{IDF_TARGET_NAME} has a built-in JTAG circuitry and can be debugged without any additional chip. Only an USB cable connected to the D+/D- pins is necessary. The necessary connections are shown in the following section.

Configure Hardware
^^^^^^^^^^^^^^^^^^

.. list-table:: {IDF_TARGET_NAME} pins and USB signals
    :widths: 25 75
    :header-rows: 1

    * - {IDF_TARGET_NAME} Pin
      - USB Signal
    * - {IDF_TARGET_JTAG_PIN_Dneg}
      - D-
    * - {IDF_TARGET_JTAG_PIN_Dpos}
      - D+
    * - 5V
      - V_BUS
    * - GND
      - Ground

Please verify that the {IDF_TARGET_NAME} pins used for USB communication are not connected to some other HW that may disturb the JTAG operation.

Configure USB Drivers
^^^^^^^^^^^^^^^^^^^^^

JTAG communication should work on all supported platforms. Windows users might get `LIBUSB_ERROR_NOT_FOUND` errors. Please use version 2.8 (or newer) of the :ref:`get-started-windows-tools-installer` and select the driver "Espressif - WinUSB support for JTAG (ESP32-C3/S3)" in order to resolve this issue. If you don't want to re-run the installer then the same can be achieved with `idf-env <https://github.com/espressif/idf-env>`_ by running the following command from PowerShell::

    Invoke-WebRequest 'https://dl.espressif.com/dl/idf-env/idf-env.exe' -OutFile .\idf-env.exe; .\idf-env.exe driver install --espressif
