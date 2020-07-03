Configure Other JTAG Interface
==============================
:link_to_translation:`zh_CN:[中文]`

Refer to section :ref:`jtag-debugging-selecting-jtag-adapter` for guidance what JTAG interface to select, so it is able to operate with OpenOCD and {IDF_TARGET_NAME}. Then follow three configuration steps below to get it working.


Configure Hardware
^^^^^^^^^^^^^^^^^^

1.  Identify all pins / signals on JTAG interface and {IDF_TARGET_NAME} board, that should be connected to establish communication.

    .. include:: {IDF_TARGET_TOOLCHAIN_NAME}.inc
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

