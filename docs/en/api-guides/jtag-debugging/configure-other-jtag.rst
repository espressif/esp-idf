Configure Other JTAG Interfaces
===============================
:link_to_translation:`zh_CN:[中文]`

For guidance about which JTAG interface to select when using OpenOCD with {IDF_TARGET_NAME}, refer to the section :ref:`jtag-debugging-selecting-jtag-adapter`. Then follow the configuration steps below to get it working.


.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED and not esp32c3

    Configure eFuses
    ^^^^^^^^^^^^^^^^

    By default, {IDF_TARGET_NAME} JTAG interface is connected to the :doc:`built-in USB_SERIAL_JTAG peripheral <configure-builtin-jtag>`. To use an external JTAG adapter instead, you need to switch the JTAG interface to the GPIO pins. This can be done by burning eFuses using ``espefuse.py`` tool.

    - Burning ``DIS_USB_JTAG`` eFuse will permanently disable the connection between USB_SERIAL_JTAG and the JTAG port of the CPU. JTAG interface can then be connected to |jtag-gpio-list|. Note that USB CDC functionality of USB_SERIAL_JTAG will still be usable, i.e., flashing and monitoring over USB CDC will still work.
    - Burning ``STRAP_JTAG_SEL`` eFuse will enable selection of JTAG interface by a strapping pin, |jtag-sel-gpio|. If the strapping pin is low when {IDF_TARGET_NAME} is reset, JTAG interface will use |jtag-gpio-list|. If the strapping pin is high, USB_SERIAL_JTAG will be used as the JTAG interface.

    .. warning::
        Burning eFuses is an irreversible operation, so please consider the above options before starting the process.


.. only:: esp32c3

    Configure eFuses
    ^^^^^^^^^^^^^^^^

    By default, {IDF_TARGET_NAME} JTAG interface is connected to the :doc:`built-in USB_SERIAL_JTAG peripheral <configure-builtin-jtag>`. To use an external JTAG adapter instead, you need to switch the JTAG interface to the GPIO pins. This can be done by burning eFuses using ``espefuse.py`` tool.

    Burning ``DIS_USB_JTAG`` eFuse will permanently disable the connection between USB_SERIAL_JTAG and the JTAG port of the CPU. JTAG interface can then be connected to |jtag-gpio-list|. Note that USB CDC functionality of USB_SERIAL_JTAG will still be usable, i.e., flashing and monitoring over USB CDC will still work.

    .. warning::
        Burning eFuses is an irreversible operation, so please consider the above option before starting the process.

Configure Hardware
^^^^^^^^^^^^^^^^^^

1.  Identify all pins/signals on JTAG interface and {IDF_TARGET_NAME} board that should be connected to establish communication.

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: jtag-pins
        :end-before: ---

2.  Verify if {IDF_TARGET_NAME} pins used for JTAG communication are not connected to some other hardware that may disturb JTAG operation.

3.  Connect identified pin/signals of {IDF_TARGET_NAME} and JTAG interface.


Configure Drivers
^^^^^^^^^^^^^^^^^

You may need to install driver software to make JTAG work with computer. Refer to documentation of your JTAG adapter for related details.


Connect
^^^^^^^

Connect JTAG interface to the computer. Power on {IDF_TARGET_NAME} and JTAG interface boards. Check if the JTAG interface is visible on the computer.


To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.
