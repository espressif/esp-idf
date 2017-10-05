Tips and Quirks
---------------

This section provides collection of all tips and quirks referred to from various parts of this guide.


.. _jtag-debugging-tip-breakpoints:

Breakpoints and watchpoints available
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ESP32 supports 2 hardware breakpoints. It also supports two watchpoints, so two variables can be watched for change or read by the GDB command ``watch myVariable``. Note that menuconfig option :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` uses the 2nd watchpoint and will not provide expected results, if you also try to use it within OpenOCD / GDB. See menuconfig's help for detailed description.


.. _jtag-debugging-tip-where-breakpoints:

What else should I know about breakpoints?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Normal GDB breakpoints (``b myFunction``) can only be set in IRAM, because that memory is writable. Setting these types of breakpoints in code in flash will not work. Instead, use a hardware breakpoint (``hb myFunction``). 


.. _jtag-debugging-tip-why-next-works-as-step:

Why stepping with "next" does not bypass subroutine calls?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When stepping through the code with ``next`` command, GDB is internally setting a breakpoint (one out of two available) ahead in the code to bypass the subroutine calls. This functionality will not work, if the two available breakpoints are already set elsewhere in the code. If this is the case, delete breakpoints to have one "spare". With both breakpoints already used, stepping through the code with ``next`` command will work as like with ``step`` command and debugger will step inside subroutine calls.


.. _jtag-debugging-tip-code-options:

Support options for OpenOCD at compile time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF has some support options for OpenOCD debugging which can be set at compile time:

* :ref:`CONFIG_ESP32_DEBUG_OCDAWARE` is enabled by default. If a panic or unhandled exception is thrown and a JTAG debugger is connected (ie openocd is running), ESP-IDF will break into the debugger.
* :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` (disabled by default) sets watchpoint index 1 (the second of two) at the end of any task stack. This is the most accurate way to debug task stack overflows. Click the link for more details.

Please see the :ref:`make menuconfig <get-started-configure>` menu for more details on setting compile-time options.

.. _jtag-debugging-tip-freertos-support:

FreeRTOS support
^^^^^^^^^^^^^^^^

OpenOCD has explicit support for the ESP-IDF FreeRTOS. GDB can see FreeRTOS tasks as threads. Viewing them all can be done using the GDB ``i threads`` command, changing to a certain task is done with ``thread n``, with ``n`` being the number of the thread. FreeRTOS detection can be disabled in target's configuration. For more details see :ref:`jtag-debugging-tip-openocd-configure-target`.


.. _jtag-debugging-tip-code-flash-voltage:

Why to set SPI flash voltage in OpenOCD configuration?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The MTDI pin of ESP32, being among four pins used for JTAG communication, is also one of ESP32's bootstrapping pins. On power up ESP32 is sampling binary level on MTDI to set it's internal voltage regulator used to supply power to external SPI flash chip. If binary level on MDTI pin on power up is low, the voltage regulator is set to deliver 3.3V, if it is high, then the voltage is set to 1.8V. The MTDI pin should have a pull-up or may rely on internal weak pull down resistor (see ESP32 Datasheet for details), depending on the type of SPI chip used. Once JTAG is connected, it overrides the pull-up or pull-down resistor that is supposed to do the bootstrapping. 

To handle this issue OpenOCD's board configuration file (e.g. ``boards\esp-wroom-32.cfg`` for ESP-WROOM-32 module) provides ``ESP32_FLASH_VOLTAGE`` parameter to set the idle state of the ``TDO`` line to a specified binary level, therefore reducing the chance of a bad bootup of application due to incorrect flash voltage.

Check specification of ESP32 module connected to JTAG, what is the power supply voltage of SPI flash chip. Then set ``ESP32_FLASH_VOLTAGE`` accordingly. Most WROOM modules use 3.3V flash, while WROVER modules use 1.8V flash. 


.. _jtag-debugging-tip-optimize-jtag-speed:

Optimize JTAG speed
^^^^^^^^^^^^^^^^^^^

In order to achieve higher data rates and minimize number of dropped packets it is recommended to optimize setting of JTAG clock frequency, so it is at maximum and still provides stable operation of JTAG. To do so use the following tips.

1.  The upper limit of JTAG clock frequency is 20 MHz if CPU runs at 80 MHz, or 26 MHz if CPU runs at 160 MHz or 240 MHz.
2.  Depending on particular JTAG adapter and the length of connecting cables, you may need to reduce JTAG frequency below 20 / 26 MHz.
3.  In particular reduce frequency, if you get DSR/DIR errors (and they do not relate to OpenOCD trying to read from a memory range without physical memory being present there).
4.  ESP-WROVER-KIT operates stable at 20 / 26 MHz.


.. _jtag-debugging-tip-debugger-startup-commands:

What is the meaning of debugger's startup commands?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On startup, debugger is issuing sequence of commands to reset the chip and halt it at specific line of code. This sequence (shown below) is user defined to pick up at most convenient / appropriate line and start debugging. 

* ``mon reset halt`` — reset the chip and keep the CPUs halted
* ``thb app_main`` — insert a temporary hardware breakpoint at ``app_main``, put here another function name if required
* ``x $a1=0`` — this is the tricky part. As far as we can tell, there is no way for a ``mon`` command to tell GDB that the target state has changed. GDB will assume that whatever stack the target had before ``mon reset halt`` will still be valid. In fact, after reset the target state will change and executing ``x $a1=0`` is a way to force GDB to get new state from the target.
* ``c`` — resume the program. It will then stop at breakpoint inserted at ``app_main``.


.. _jtag-debugging-tip-openocd-configure-target:

Configuration of OpenOCD for specific target
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

OpenOCD needs to be told what JTAG adapter **interface** to use, as well as what type of **board** and processor the JTAG adapter is connected to. To do so, use existing configuration files located in OpenOCD's ``share/openocd/scripts/interface`` and ``share/openocd/scripts/board`` folders. 

For example, if you connect to ESP-WROVER-KIT with ESP-WROOM-32 module installed (see section :doc:`ESP32 WROVER KIT <../../hw-reference/modules-and-boards>`), use the following configuration files:

* ``interface/ftdi/esp32_devkitj_v1.cfg``
* ``board/esp-wroom-32.cfg``

Optionally prepare configuration by yourself. To do so, you can check existing files and modify them to match you specific hardware. Below is the summary of available configuration parameters for **board** configuration.


.. highlight:: none

Adapter's clock speed
""""""""""""""""""""""

::

    adapter_khz 20000

See :ref:`jtag-debugging-tip-optimize-jtag-speed` for guidance how to set this value.


Single core debugging
"""""""""""""""""""""

::

    set ESP32_ONLYCPU 1

Comment out this line for dual core debugging.


Disable RTOS support
""""""""""""""""""""

::

    set ESP32_RTOS none

Comment out this line to have RTOS support.


Power supply voltage of ESP32's SPI flash chip
""""""""""""""""""""""""""""""""""""""""""""""

::

    set ESP32_FLASH_VOLTAGE 1.8

Comment out this line to set 3.3V, ref: :ref:`jtag-debugging-tip-code-flash-voltage`


Configuration file for ESP32 targets
""""""""""""""""""""""""""""""""""""

::

    source [find target/esp32.cfg]

.. note::

    Do not change ``source [find target/esp32.cfg]`` line unless you are familiar with OpenOCD internals.

Currently ``target/esp32.cfg`` remains the only configuration file for ESP32 targets (esp108 and esp32). The matrix of supported configurations is as follows:

    +---------------+---------------+---------------+
    | Dual/single   | RTOS          | Target used   |
    +===============+===============+===============+
    | dual          | FreeRTOS      | esp32         |
    +---------------+---------------+---------------+
    | single        | FreeRTOS      | esp108 (*)    |
    +---------------+---------------+---------------+
    | dual          | none          | esp108        |
    +---------------+---------------+---------------+
    | single        | none          | esp108        |
    +---------------+---------------+---------------+

    (*) — we plan to fix this and add support for single core debugging with esp32 target in a subsequent commits.

Look inside ``board/esp-wroom-32.cfg`` for additional information provided in comments besides each configuration parameter.


.. _jtag-debugging-tip-reset-by-debugger:

How debugger resets ESP32?
^^^^^^^^^^^^^^^^^^^^^^^^^^

The board can be reset by entering ``mon reset`` or ``mon reset halt`` into GDB.


.. _jtag-debugging-tip-jtag-pins-reconfigured:

Do not use JTAG pins for something else
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Operation of JTAG may be disturbed, if some other h/w is connected to JTAG pins besides ESP32 module and JTAG adapter. ESP32 JTAG us using the following pins:

    +---+----------------+-------------+
    |   | ESP32 JTAG Pin | JTAG Signal |
    +===+================+=============+
    | 1 | MTDO / GPIO15  | TDO         |
    +---+----------------+-------------+
    | 2 | MTDI / GPIO12  | TDI         |
    +---+----------------+-------------+
    | 3 | MTCK / GPIO13  | TCK         |
    +---+----------------+-------------+
    | 4 | MTMS / GPIO14  | TMS         |
    +---+----------------+-------------+

JTAG communication will likely fail, if configuration of JTAG pins is changed by user application. If OpenOCD initializes correctly (detects the two Tensilica cores), but loses sync and spews out a lot of DTR/DIR errors when the program is ran, it is likely that the application reconfigures the JTAG pins to something else, or the user forgot to connect Vtar to a JTAG adapter that needed it. 

.. highlight:: none

Below is an excerpt from series of errors reported by GDB after the application stepped into the code that reconfigured MTDO / GPIO15 to be an input::

    cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates target still busy!
    cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an exception!
    cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an overrun!
    cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates target still busy!
    cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an exception!
    cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an overrun!


.. _jtag-debugging-tip-reporting-issues:

Reporting issues with OpenOCD / GDB
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In case you encounter a problem with OpenOCD or GDB programs itself and do not find a solution searching available resources on the web, open an issue in the OpenOCD issue tracker under https://github.com/espressif/openocd-esp32/issues. 

1.  In issue report provide details of your configuration:

    a. JTAG adapter type.
    b. Release of ESP-IDF used to compile and load application that is being debugged.
    c. Details of OS used for debugging.
    d. Is OS running natively on a PC or on a virtual machine?

2.  Create a simple example that is representative to observed issue. Describe steps how to reproduce it. In such an example debugging should not be affected by non-deterministic behaviour introduced by the Wi-Fi stack, so problems will likely be easier to reproduce, if encountered once.

.. highlight:: bash

3.  Prepare logs from debugging session by adding additional parameters to start up commands.

    OpenOCD:

        ::

            bin/openocd -l openocd_log.txt -d 3 -s share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg

        Logging to a file this way will prevent information displayed on the terminal. This may be a good thing taken amount of information provided, when increased debug level ``-d 3`` is set. If you still like to see the log on the screen, then use another command instead:

        ::

            bin/openocd -d 3 -s share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg 2>&1 | tee openocd.log

        .. note::

            See :ref:`jtag-debugging-building-openocd` for slightly different command format, when running OpenOCD built from sources.

    Debugger:

        ::

           xtensa-esp32-elf-gdb -ex "set remotelogfile gdb_log.txt" <all other options>

        Optionally add command ``remotelogfile gdb_log.txt`` to the ``gdbinit`` file.


4.  Attach both ``openocd_log.txt`` and ``gdb_log.txt`` files to your issue report.
