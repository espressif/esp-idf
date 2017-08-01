Configure Other JTAG Interface
==============================

Refer to section :ref:`jtag-debugging-selecting-jtag-adapter` for guidance what JTAG interface to select, so it is able to operate with OpenOCD and ESP32. Then follow three configuration steps below to get it working. 


Configure Hardware
^^^^^^^^^^^^^^^^^^

1.  Identify all pins / signals on JTAG interface and ESP32 board, that should be connected to establish communication. 

    +---+---------------+-------------+
    |   | ESP32 Pin     | JTAG Signal |
    +===+===============+=============+
    | 1 | CHIP_PU       | TRST_N      |
    +---+---------------+-------------+
    | 2 | MTDO / GPIO15 | TDO         |
    +---+---------------+-------------+
    | 3 | MTDI / GPIO12 | TDI         |
    +---+---------------+-------------+
    | 4 | MTCK / GPIO13 | TCK         |
    +---+---------------+-------------+
    | 5 | MTMS / GPIO14 | TMS         |
    +---+---------------+-------------+
    | 6 | GND           | GND         |
    +---+---------------+-------------+

2.  Verify if ESP32 pins used for JTAG communication are not connected to some other h/w that may disturb JTAG operation.

3.  Connect identified pin / signals of ESP32 and JTAG interface.


Configure Drivers
^^^^^^^^^^^^^^^^^
You may need to install driver s/w to make JTAG work with computer. Refer to documentation of JTAG adapter, that should provide related details. 


Connect
^^^^^^^

Connect JTAG interface to the computer. Power on ESP32 and JTAG interface boards. Check if JTAG interface is visible by computer. 


To carry on with debugging environment setup, proceed to section :ref:`jtag-debugging-run-openocd`.

