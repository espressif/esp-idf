.. include:: {IDF_TARGET_PATH_NAME}.inc
   :start-after: devkit-defs
   :end-before: ---


Configure {IDF_TARGET_NAME} Built-in JTAG Interface
===================================================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_JTAG_PIN_Dneg:default="Not Updated!", esp32c3="GPIO18", esp32c6="GPIO12", esp32s3="GPIO19", esp32h2="GPIO26", esp32p4="GPIO24"}
{IDF_TARGET_JTAG_PIN_Dpos:default="Not Updated!", esp32c3="GPIO19", esp32c6="GPIO13", esp32s3="GPIO20", esp32h2="GPIO27", esp32p4="GPIO25"}

{IDF_TARGET_NAME} has built-in JTAG circuitry and can be debugged without any additional chip. Only a USB cable connected to the D+/D- pins is necessary. For dev kits without an exposed USB Serial Jtag connector, a USB breakout cable can be used. The necessary connections are shown in the following section.

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

JTAG communication should work on all supported platforms. Windows and Linux require extra steps as described below.

Windows
"""""""

Windows users might get `LIBUSB_ERROR_NOT_FOUND` errors. To resolve this, install drivers using one of the following methods:

- In :doc:`Espressif Installation Manager (EIM) <../../get-started/windows-setup>` graphical user interface (GUI), click ``Open Dashboard`` under ``Manage Installations``, and then click ``Install Drivers``:

    .. figure:: ../../../_static/jtag-debugging-install-usb-drivers-eim.png
      :align: center
      :alt: Install Drivers in EIM GUI
      :figclass: align-center

      Install Drivers in EIM GUI

- Run the following command from PowerShell to install drivers with EIM command line interface:

    .. code-block:: bash

      eim install-drivers

- Run the following command from PowerShell to install drivers with `idf-env <https://github.com/espressif/idf-env>`_:

    .. code-block:: bash

      Invoke-WebRequest 'https://dl.espressif.com/dl/idf-env/idf-env.exe' -OutFile .\idf-env.exe; .\idf-env.exe driver install --espressif

Linux
"""""

On Linux adding OpenOCD udev rules is required and is done by placing the following `udev rules file <https://github.com/espressif/openocd-esp32/blob/master/contrib/60-openocd.rules>`_ in the ``/etc/udev/rules.d`` folder.
