Configure Other JTAG Interface
==============================
:link_to_translation:`zh_CN:[中文]`

Refer to section :ref:`jtag-debugging-selecting-jtag-adapter` for guidance what JTAG interface to select, so it is able to operate with OpenOCD and {IDF_TARGET_NAME}. Then follow three configuration steps below to get it working.


.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

   Configure eFuses
   ^^^^^^^^^^^^^^^^

   By default, {IDF_TARGET_NAME} JTAG interface is connected to the :doc:`built-in USB_SERIAL_JTAG peripheral <configure-builtin-jtag>`. To use an external JTAG adapter instead, you need to switch the JTAG interface to the GPIO pins. This can be done by burning eFuses using ``espefuse.py`` tool.

   Burning eFuses is an irreversible operation, so consider both options below before starting the process.

   - Burning ``DIS_USB_JTAG`` eFuse will permanently disable the connection between USB_SERIAL_JTAG and the JTAG port of the CPU. JTAG interface can then be connected to |jtag-gpio-list|. Note that USB CDC functionality of USB_SERIAL_JTAG will still be useable, i.e. flashing and monitoring over USB CDC will still work.

   - Burning ``JTAG_SEL_ENABLE`` eFuse will enable selection of JTAG interface by a strapping pin, |jtag-sel-gpio|. If the strapping pin is low when {IDF_TARGET_NAME} is reset, JTAG interface will use |jtag-gpio-list|. If the strapping pin is high, USB_SERIAL_JTAG will be used as the JTAG interface.

Configure Hardware
^^^^^^^^^^^^^^^^^^

1.  Identify all pins / signals on JTAG interface and {IDF_TARGET_NAME} board, that should be connected to establish communication.

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: jtag-pins
        :end-before: ---

2.  Verify if {IDF_TARGET_NAME} pins used for JTAG communication are not connected to some other h/w that may disturb JTAG operation.

3.  Connect identified pin / signals of {IDF_TARGET_NAME} and JTAG interface.


Configure Drivers
^^^^^^^^^^^^^^^^^
You may need to install driver s/w to make JTAG work with computer. Refer to documentation of JTAG adapter, that should provide related details.


Connect
^^^^^^^

Connect JTAG interface to the computer. Power on {IDF_TARGET_NAME} and JTAG interface boards. Check if JTAG interface is visible by computer.


To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.

