Tips and Quirks
---------------

:link_to_translation:`zh_CN:[中文]`

This section provides collection of all tips and quirks referred to from various parts of this guide.

.. _jtag-debugging-tip-breakpoints:

Breakpoints and Watchpoints Available
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

{IDF_TARGET_NAME} debugger supports {IDF_TARGET_SOC_CPU_BREAKPOINTS_NUM} hardware implemented breakpoints and 64 software ones. Hardware breakpoints are implemented by {IDF_TARGET_NAME} chip's logic and can be set anywhere in the code: either in flash or IRAM program's regions. Additionally there are 2 types of software breakpoints implemented by OpenOCD: flash (up to 32) and IRAM (up to 32) breakpoints. Currently GDB can not set software breakpoints in flash. So until this limitation is removed those breakpoints have to be emulated by OpenOCD as hardware ones (see :ref:`below <jtag-debugging-tip-where-breakpoints>` for details). {IDF_TARGET_NAME} also supports {IDF_TARGET_SOC_CPU_WATCHPOINTS_NUM} watchpoints, so {IDF_TARGET_SOC_CPU_WATCHPOINTS_NUM} variables can be watched for change or read by the GDB command ``watch myVariable``. Note that menuconfig option :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` uses the last watchpoint and will not provide expected results, if you also try to use it within OpenOCD/GDB. See menuconfig's help for detailed description.


.. _jtag-debugging-tip-where-breakpoints:

What Else Should I Know About Breakpoints?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Emulating part of hardware breakpoints using software flash ones means that the GDB command ``hb myFunction`` which is invoked for function in flash will use pure hardware breakpoint if it is available otherwise one of the 32 software flash breakpoints is used. The same rule applies to ``b myFunction``-like commands. In this case GDB will decide what type of breakpoint to set itself. If ``myFunction`` is resided in writable region (IRAM) software IRAM breakpoint will be used otherwise hardware or software flash breakpoint is used as it is done for ``hb`` command.


.. _jtag-debugging-tip-flash-mappings:

Flash Mappings vs SW Flash Breakpoints
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In order to set/clear software breakpoints in flash, OpenOCD needs to know their flash addresses. To accomplish conversion from the {IDF_TARGET_NAME} address space to the flash one, OpenOCD uses mappings of program's code regions resided in flash. Those mappings are kept in the image header which is prepended to program binary data (code and data segments) and is specific to every application image written to the flash. So to support software flash breakpoints OpenOCD should know where application image under debugging is resided in the flash. By default OpenOCD reads partition table at 0x8000 and uses mappings from the first found application image, but there can be the cases when it will not work, e.g., partition table is not at standard flash location or even there can be multiple images: one factory and two OTA and you may want to debug any of them. To cover all possible debugging scenarios OpenOCD supports special command which can be used to set arbitrary location of application image to debug. The command has the following format:

``esp appimage_offset <offset>``

Offset should be in hex format. To reset to the default behaviour you can specify ``-1`` as offset.

.. note::

    Since GDB requests memory map from OpenOCD only once when connecting to it, this command should be specified in one of the TCL configuration files, or passed to OpenOCD via its command line. In the latter case command line should look like below:

    .. highlight:: bash

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: run-openocd-appimage-offset
        :end-before: ---

    Another option is to execute that command via OpenOCD telnet session and then connect GDB, but it seems to be less handy.

.. _jtag-debugging-tip-why-next-works-as-step:

Why Stepping with "next" Does Not Bypass Subroutine Calls?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When stepping through the code with ``next`` command, GDB is internally setting a breakpoint ahead in the code to bypass the subroutine calls. If all {IDF_TARGET_SOC_CPU_BREAKPOINTS_NUM} breakpoints are already set, this functionality will not work. If this is the case, delete breakpoints to have one "spare". With all breakpoints already used, stepping through the code with ``next`` command will work as like with ``step`` command and debugger will step inside subroutine calls.


.. _jtag-debugging-tip-code-options:

Support Options for OpenOCD at Compile Time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF has some support options for OpenOCD debugging which can be set at compile time:

* :ref:`CONFIG_ESP_DEBUG_OCDAWARE` is enabled by default. If a panic or unhandled exception is thrown and a JTAG debugger is connected (ie  OpenOCD is running), ESP-IDF will break into the debugger.

* :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` (disabled by default) sets watchpoint index 1 (the second of two) at the end of any task stack. This is the most accurate way to debug task stack overflows. Click the link for more details.

Please see the :ref:`project configuration menu <get-started-configure>` menu for more details on setting compile-time options.

.. _jtag-debugging-tip-freertos-support:

FreeRTOS Support
^^^^^^^^^^^^^^^^

OpenOCD has explicit support for the ESP-IDF FreeRTOS. GDB can see FreeRTOS tasks as threads. Viewing them all can be done using the GDB ``i threads`` command, changing to a certain task is done with ``thread n``, with ``n`` being the number of the thread. FreeRTOS detection can be disabled in target's configuration. For more details see :ref:`jtag-debugging-tip-openocd-configure-target`.

GDB has a Python extension for FreeRTOS support. ESP-IDF automatically loads this module into GDB with the ``idf.py gdb`` command when the system requirements are met. See more details in :ref:`jtag-debugging-examples-command-line-08`.

.. only:: esp32

    .. _jtag-debugging-tip-code-flash-voltage:

    Why to Set SPI Flash Voltage in OpenOCD Configuration?
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The MTDI pin of ESP32, being among four pins used for JTAG communication, is also one of ESP32's bootstrapping pins. On power up ESP32 is sampling binary level on MTDI to set it's internal voltage regulator used to supply power to external SPI flash chip. If binary level on MDTI pin on power up is low, the voltage regulator is set to deliver 3.3 V, if it is high, then the voltage is set to 1.8 V. The MTDI pin should have a pull-up or may rely on internal weak pull down resistor (see `ESP32 Series Datasheet <https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf>`_ for details), depending on the type of SPI chip used. Once JTAG is connected, it overrides the pull-up or pull-down resistor that is supposed to do the bootstrapping.

    To handle this issue OpenOCD's board configuration file (e.g. ``board\esp32-wrover-kit-3.3v.cfg`` for ESP-WROVER-KIT board) provides ``ESP32_FLASH_VOLTAGE`` parameter to set the idle state of the ``TDO`` line to a specified binary level, therefore reducing the chance of a bad bootup of application due to incorrect flash voltage.

    Check specification of ESP32 module connected to JTAG, what is the power supply voltage of SPI flash chip. Then set ``ESP32_FLASH_VOLTAGE`` accordingly. Most WROOM modules use 3.3 V flash. WROVER earlier than ESP32-WROVER-B use 1.8 V flash, while ESP32-WROVER-B and -E modules use 3.3 V flash.

    .. _jtag-debugging-tip-optimize-jtag-speed:

.. only:: not esp32

    .. _jtag-debugging-tip-optimize-jtag-speed:

Optimize JTAG Speed
^^^^^^^^^^^^^^^^^^^

In order to achieve higher data rates and minimize number of dropped packets it is recommended to optimize setting of JTAG clock frequency, so it is at maximum and still provides stable operation of JTAG. To do so use the following tips.

1.  The upper limit of JTAG clock frequency is 20 MHz if CPU runs at 80 MHz, or 26 MHz if CPU runs at 160 MHz or 240 MHz.
2.  Depending on particular JTAG adapter and the length of connecting cables, you may need to reduce JTAG frequency below 20 MHz or 26 MHz.
3.  In particular reduce frequency, if you get DSR/DIR errors (and they do not relate to OpenOCD trying to read from a memory range without physical memory being present there).
4.  ESP-WROVER-KIT operates stable at 20 MHz or 26 MHz.


.. _jtag-debugging-tip-debugger-startup-commands:

What Is the Meaning of Debugger's Startup Commands?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On startup, debugger is issuing sequence of commands to reset the chip and halt it at specific line of code. This sequence (shown below) is user defined to pick up at most convenient/appropriate line and start debugging.

* ``set remote hardware-watchpoint-limit {IDF_TARGET_SOC_CPU_WATCHPOINTS_NUM}`` — Restrict GDB to using available hardware watchpoints supported by the chip, {IDF_TARGET_SOC_CPU_WATCHPOINTS_NUM} for {IDF_TARGET_NAME}. For more information see https://sourceware.org/gdb/onlinedocs/gdb/Remote-Configuration.html.
* ``mon reset halt`` — reset the chip and keep the CPUs halted
* ``maintenance flush register-cache`` — monitor (``mon``) command can not inform GDB that the target state has changed. GDB will assume that whatever stack the target had before ``mon reset halt`` will still be valid. In fact, after reset the target state will change, and executing ``maintenance flush register-cache`` is a way to force GDB to get new state from the target.
* ``thb app_main`` — insert a temporary hardware breakpoint at ``app_main``, put here another function name if required
* ``c`` — resume the program. It will then stop at breakpoint inserted at ``app_main``.


.. _jtag-debugging-tip-openocd-configure-target:

Configuration of OpenOCD for Specific Target
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are several kinds of OpenOCD configuration files (``*.cfg``). All configuration files are located in subdirectories of ``share/openocd/scripts`` directory of OpenOCD distribution (or ``tcl/scripts`` directory of the source repository). For the purposes of this guide, the most important ones are ``board``, ``interface`` and ``target``.

* ``interface`` configuration files describe the JTAG adapter. Examples of JTAG adapters are ESP-Prog and J-Link.
* ``target`` configuration files describe specific chips, or in some cases, modules.
* ``board`` configuration files are provided for development boards with a built-in JTAG adapter. Such files include an ``interface`` configuration file to choose the adapter, and ``target`` configuration file to choose the chip/module.

The following configuration files are available for {IDF_TARGET_NAME}:

.. include:: {IDF_TARGET_PATH_NAME}.inc
    :start-after: openocd-cfg-files
    :end-before: ---


If you are using one of the boards which have a pre-defined configuration file, you only need to pass one ``-f`` argument to OpenOCD, specifying that file.

If you are using a board not listed here, you need to specify both the interface configuration file and target configuration file.

Custom Configuration Files
""""""""""""""""""""""""""

OpenOCD configuration files are written in TCL, and include a variety of choices for customization and scripting. This can be useful for non-standard debugging situations. Please refer to `OpenOCD Manual`_ for the TCL scripting reference.

.. _jtag-debugging-tip-openocd-config-vars:

OpenOCD Configuration Variables
"""""""""""""""""""""""""""""""

The following variables can be optionally set before including the ESP-specific target configuration file. This can be done either in a custom configuration file, or from the command line.

The syntax for setting a variable in TCL is:

.. code-block:: tcl

    set VARIABLE_NAME value

To set a variable from the command line (replace the name of .cfg file with the correct file for your board):

.. code-block:: bash

    openocd -c 'set VARIABLE_NAME value' -f board/esp-xxxxx-kit.cfg

It is important to set the variable before including the ESP-specific configuration file, otherwise the variable will not have effect. You can set multiple variables by repeating the ``-c`` option.

.. list-table:: Common ESP-related OpenOCD variables
    :widths: 25 75
    :header-rows: 1

    * - Variable
      - Description
    * - ``ESP_RTOS``
      - Set to ``none`` to disable RTOS support. In this case, thread list will not be available in GDB. Can be useful when debugging FreeRTOS itself, and stepping through the scheduler code.
    * - ``ESP_FLASH_SIZE``
      - Set to ``0`` to disable Flash breakpoints support.
    * - ``ESP_SEMIHOST_BASEDIR``
      - Set to the path (on the host) which will be the default directory for semihosting functions.

.. include:: {IDF_TARGET_PATH_NAME}.inc
    :start-after: openocd-target-specific-config-vars
    :end-before: ---

.. _jtag-debugging-tip-reset-by-debugger:

How Debugger Resets {IDF_TARGET_NAME}?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The board can be reset by entering ``mon reset`` or ``mon reset halt`` into GDB.


.. _jtag-debugging-tip-jtag-pins-reconfigured:

Can JTAG Pins Be Used for Other Purposes?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    {IDF_TARGET_NAME} contains a USB Serial/JTAG Controller which can be used for debugging. By default, {IDF_TARGET_NAME} JTAG interface is connected to the built-in USB SERIAL/JTAG peripheral. For details, please refer to :doc:`Configure {IDF_TARGET_NAME} built-in JTAG Interface <../jtag-debugging/configure-builtin-jtag>`.

    When you use USB Serial/JTAG Controller for debugging, |jtag-gpio-list| can be used for other purposes.

    However, if you switch the USB JTAG interface to the GPIOs by burning eFuses, |jtag-gpio-list| can be used for JTAG debugging. When they perform this function, they cannot be used for other purposes.

Operation of JTAG may be disturbed, if some other hardware is connected to JTAG pins besides {IDF_TARGET_NAME} module and JTAG adapter. {IDF_TARGET_NAME} JTAG is using the following pins:

.. include:: {IDF_TARGET_PATH_NAME}.inc
    :start-after: jtag-pins
    :end-before: ---

JTAG communication will likely fail, if configuration of JTAG pins is changed by a user application. If OpenOCD initializes correctly (detects all the CPU cores in the SOC), but loses sync and spews out a lot of DTR/DIR errors when the program is running, it is likely that the application reconfigures the JTAG pins to something else, or the user forgot to connect Vtar to a JTAG adapter that requires it.

.. only:: esp32

    .. highlight:: none

    Below is an excerpt from series of errors on the dual-core {IDF_TARGET_NAME} reported by GDB after the application stepped into the code that reconfigured MTDO pin to be an input::

        cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates target still busy!
        cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an exception!
        cpu0: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an overrun!
        cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates target still busy!
        cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an exception!
        cpu1: xtensa_resume (line 431): DSR (FFFFFFFF) indicates DIR instruction generated an overrun!

.. _jtag-debugging-security-features:

JTAG with Flash Encryption or Secure Boot
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, enabling Flash Encryption and/or Secure Boot will disable JTAG debugging. On first boot, the bootloader will burn an eFuse bit to permanently disable JTAG at the same time it enables the other features.

.. only:: SOC_HMAC_SUPPORTED

    Please note that once JTAG is permanently disabled, it cannot be re-enabled for JTAG access. However, we do have the option of disabling JTAG softly. For more details on soft disabling and re-enabling soft-disabled JTAG, please refer to the :ref:`hmac_for_enabling_jtag`.

The project configuration option :ref:`CONFIG_SECURE_BOOT_ALLOW_JTAG` will keep JTAG enabled at this time, removing all physical security but allowing debugging. (Although the name suggests Secure Boot, this option can be applied even when only Flash Encryption is enabled).

However, OpenOCD may attempt to automatically read and write the flash in order to set :ref:`software breakpoints <jtag-debugging-tip-where-breakpoints>`. This has two problems:

- Software breakpoints are incompatible with Flash Encryption, OpenOCD currently has no support for encrypting or decrypting flash contents.
- If Secure Boot is enabled, setting a software breakpoint will change the digest of a signed app and make the signature invalid. This means if a software breakpoint is set and then a reset occurs, the signature verification will fail on boot.

To disable software breakpoints while using JTAG, add an extra argument ``-c 'set ESP_FLASH_SIZE 0'`` to the start of the OpenOCD command line, see :ref:`jtag-debugging-tip-openocd-config-vars`.

.. note::

   For the same reason, the ESP-IDF app may fail bootloader verification of app signatures, when this option is enabled and a software breakpoint is set.

.. only:: esp32

    JTAG and ESP32-WROOM-32 AT Firmware Compatibility Issue
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    The ESP32-WROOM series of modules come pre-flashed with AT firmware. This firmware configures the pins GPIO12 to GPIO15 as SPI slave interface, which makes using JTAG impossible.

    To make JTAG available, build new firmware that is not using pins GPIO12 to GPIO15 dedicated to JTAG communication. After that, flash the firmware onto your module. See also :ref:`jtag-debugging-tip-jtag-pins-reconfigured`.

.. _jtag-debugging-tip-reporting-issues:

Reporting Issues with OpenOCD/GDB
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In case you encounter a problem with OpenOCD or GDB programs itself and do not find a solution searching available resources on the web, open an issue in the OpenOCD issue tracker under https://github.com/espressif/openocd-esp32/issues.

1.  In issue report provide details of your configuration:

    a. JTAG adapter type, and the chip/module being debugged.
    b. Release of ESP-IDF used to compile and load application that is being debugged.
    c. Details of OS used for debugging.
    d. Is OS running natively on a PC or on a virtual machine?

2.  Create a simple example that is representative to observed issue. Describe steps how to reproduce it. In such an example debugging should not be affected by non-deterministic behaviour introduced by the Wi-Fi stack, so problems will likely be easier to reproduce, if encountered once.

.. highlight:: bash

3.  Prepare logs from debugging session by adding additional parameters to start up commands.

    OpenOCD:

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: run-openocd-d3
        :end-before: ---

    Logging to a file this way will prevent information displayed on the terminal. This may be a good thing taken amount of information provided, when increased debug level ``-d3`` is set. If you still like to see the log on the screen, then use another command instead:

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: run-openocd-d3-tee
        :end-before: ---

    Debugger:

    .. include:: {IDF_TARGET_PATH_NAME}.inc
        :start-after: run-gdb-remotelog
        :end-before: ---

    Optionally add command ``remotelogfile gdb_log.txt`` to the ``gdbinit`` file.


4.  Attach both ``openocd_log.txt`` and ``gdb_log.txt`` files to your issue report.


.. _OpenOCD Manual: https://openocd.org/doc/html/index.html
