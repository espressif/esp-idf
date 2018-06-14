***********
IDF Monitor
***********
:link_to_translation:`zh_CN:[中文]`

The IDF Monitor tool is a Python program which runs when the ``make monitor`` target is invoked in IDF.

It is mainly a serial terminal program which relays serial data to and from the target device's serial port, but it has some other IDF-specific features.

Interacting With IDF Monitor
============================

- ``Ctrl-]`` will exit the monitor.
- ``Ctrl-T Ctrl-H`` will display a help menu with all other keyboard shortcuts.
- Any other key apart from ``Ctrl-]`` and ``Ctrl-T`` is sent through the serial port.

Automatically Decoding Addresses
================================

Any time esp-idf prints a hexadecimal code address of the form ``0x4_______``, IDF Monitor will use addr2line_ to look up the source code location and function name.

.. highlight:: none

When an esp-idf app crashes and panics a register dump and backtrace such as this is produced::

    Guru Meditation Error of type StoreProhibited occurred on core  0. Exception was unhandled.
    Register dump:
    PC      : 0x400f360d  PS      : 0x00060330  A0      : 0x800dbf56  A1      : 0x3ffb7e00
    A2      : 0x3ffb136c  A3      : 0x00000005  A4      : 0x00000000  A5      : 0x00000000
    A6      : 0x00000000  A7      : 0x00000080  A8      : 0x00000000  A9      : 0x3ffb7dd0
    A10     : 0x00000003  A11     : 0x00060f23  A12     : 0x00060f20  A13     : 0x3ffba6d0
    A14     : 0x00000047  A15     : 0x0000000f  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
    EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0x00000000

    Backtrace: 0x400f360d:0x3ffb7e00 0x400dbf56:0x3ffb7e20 0x400dbf5e:0x3ffb7e40 0x400dbf82:0x3ffb7e60 0x400d071d:0x3ffb7e90

IDF Monitor will augment the dump::

    Guru Meditation Error of type StoreProhibited occurred on core  0. Exception was unhandled.
    Register dump:
    PC      : 0x400f360d  PS      : 0x00060330  A0      : 0x800dbf56  A1      : 0x3ffb7e00
    0x400f360d: do_something_to_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:57
    (inlined by) inner_dont_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:52
    A2      : 0x3ffb136c  A3      : 0x00000005  A4      : 0x00000000  A5      : 0x00000000
    A6      : 0x00000000  A7      : 0x00000080  A8      : 0x00000000  A9      : 0x3ffb7dd0
    A10     : 0x00000003  A11     : 0x00060f23  A12     : 0x00060f20  A13     : 0x3ffba6d0
    A14     : 0x00000047  A15     : 0x0000000f  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
    EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0x00000000

    Backtrace: 0x400f360d:0x3ffb7e00 0x400dbf56:0x3ffb7e20 0x400dbf5e:0x3ffb7e40 0x400dbf82:0x3ffb7e60 0x400d071d:0x3ffb7e90
    0x400f360d: do_something_to_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:57
    (inlined by) inner_dont_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:52
    0x400dbf56: still_dont_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:47
    0x400dbf5e: dont_crash at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:42
    0x400dbf82: app_main at /home/gus/esp/32/idf/examples/get-started/hello_world/main/./hello_world_main.c:33
    0x400d071d: main_task at /home/gus/esp/32/idf/components/esp32/./cpu_start.c:254

Behind the scenes, the command IDF Monitor runs to decode each address is::

  xtensa-esp32-elf-addr2line -pfiaC -e build/PROJECT.elf ADDRESS


Launch GDB for GDBStub
======================

By default, if an esp-idf app crashes then the panic handler prints registers and a stack dump as shown above, and then resets.

Optionally, the panic handler can be configured to run a serial "gdb stub" which can communicate with a gdb_ debugger program and allow memory to be read, variables and stack frames examined, etc. This is not as versatile as JTAG debugging, but no special hardware is required.

To enable the gdbstub, run ``make menuconfig`` and set :envvar:`CONFIG_ESP32_PANIC` option to ``Invoke GDBStub``.

If this option is enabled and IDF Monitor sees the gdb stub has loaded, it will automatically pause serial monitoring and run GDB with the correct arguments. After GDB exits, the board will be reset via the RTS serial line (if this is connected.)

Behind the scenes, the command IDF Monitor runs is::

  xtensa-esp32-elf-gdb -ex "set serial baud BAUD" -ex "target remote PORT" -ex interrupt build/PROJECT.elf


Quick Compile and Flash
=======================

The keyboard shortcut ``Ctrl-T Ctrl-F`` will pause IDF Monitor, run the ``make flash`` target, then resume IDF Monitor. Any changed source files will be recompiled before re-flashing.

The keyboard shortcut ``Ctrl-T Ctrl-A`` will pause IDF Monitor, run the ``make app-flash`` target, then resume IDF Monitor. This is similar to ``make flash``, but only the main app is compiled and reflashed.


Quick Reset
===========

The keyboard shortcut ``Ctrl-T Ctrl-R`` will reset the target board via the RTS line (if it is connected.)


Pause the Application
=====================

The keyboard shortcut ``Ctrl-T Ctrl-P`` will reset the target into bootloader, so that the board will run nothing. This is
useful when you want to wait for another device to startup. Then shortcut ``Ctrl-T Ctrl-R`` can be used to restart the
application.


Toggle Output Display
=====================

Sometimes you may want to stop new output printed to screen, to see the log before. The keyboard shortcut ``Ctrl-T Ctrl-Y`` will
toggle the display (discard all serial data when the display is off) so that you can stop to see the log, and revert
again quickly without quitting the monitor.

Filtering the Output
====================

The IDF monitor can be invoked as ``make monitor PRINT_FILTER=""`` with
specifying a custom ``PRINT_FILTER`` option for filtering outputs. The default
value is an empty string which means that everything will be printed.
Restrictions on what to print can be specified as a series of
``<tag>:<log_level>`` items where ``<tag>`` is the tag string and
``<log_level>`` is a character from set ``{N, E, W, I, D, V, *}`` referring to
a level for :doc:`logging <../../api-reference/system/log>`. For example,
``PRINT_FILTER="tag1:W"`` will match and print (only) the outputs written with
``ESP_LOGW("tag1", ...)`` or at lower verbosity level, i.e. ``ESP_LOGE("tag1",
...)``. Not specifying a
``<log_level>`` or using ``*`` defaults to Verbose level.

.. note::
   The primary logging is set up at compilation time through the
   :doc:`logging library<../../api-reference/system/log>`.
   Output filtering by the IDF monitor is only a secondary solution because
   one cannot filter something which has been disabled at compilation time.
   The advantage of the secondary filtering is that one can use various
   filtering options without recompiling the application.

A restriction applies to tags when one wants to use them together with output
filtering: they cannot contain spaces, asterisks ``*`` and semicolons ``:``.

If the last line of the output is written without an end of line then the
output filtering might get confused, i.e. the monitor starts to print the line and only
later finds out that the line should have not been written. This is a known
issue and can be avoided by always adding an end of line after printing
something (especially when no output follows immediately afterwards).

Examples Of Filtering Rules:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Asterisk can be used to match any tags. However, specifying
  ``PRINT_FILTER="*:I tag1:E"`` will print for ``tag1`` only errors because
  the rule for ``tag1`` has a precedence over the rule for ``*``.
- The default (empty) rule is equivalent to ``*:V`` because matching every tag
  at level Verbose or lower means matching everything.
- Rule ``"tag1:W tag1:E"`` is equivalent to ``"tag1:E"`` because any
  consequent occurrence of the same tag name overwrites the previous one.
- Rule ``"tag1:I tag2:W"`` will print only ``tag1`` at verbosity level Info or
  lower and ``tag2`` at verbosity level Warning or lower.
- Rule ``"tag1:I tag2:W tag3:N"`` is essentially equivalent to the previous
  one because ``tag3:N`` specifies that ``tag3`` should not be printed.
- ``tag3:N`` in rule ``"tag1:I tag2:W tag3:N *:V"`` is more meaningful because
  in this context the result will be that ``tag3`` will not be printed,
  ``tag1`` and ``tag2`` will be at the specified (or lower) verbosity level
  and everything else will be printed by default.
- ``"*:N"`` will suppress all outputs even prints made by something else than
  the logging functions, e.g. ``printf``. For printing those outputs one need
  to use ``*:E`` or higher verbosity level.
- Rules ``"tag1:V"``, ``"tag1:v"``, ``"tag1:"``, ``"tag1:*"`` and ``"tag1"``
  are all equivalent ones.

A More Complex Filtering Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following log snippet was acquired using ``make monitor``::

    load:0x40078000,len:13564
    entry 0x40078d4c
    E (31) esp_image: image at 0x30000 has invalid magic byte
    W (31) esp_image: image at 0x30000 has invalid SPI mode 255
    E (39) boot: Factory app partition is not bootable
    I (568) cpu_start: Pro cpu up.
    I (569) heap_init: Initializing. RAM available for dynamic allocation:
    I (603) cpu_start: Pro cpu start user code
    D (309) light_driver: [light_init, 74]:status: 1, mode: 2
    D (318) vfs: esp_vfs_register_fd_range is successful for range <54; 64) and VFS ID 1
    I (328) wifi: wifi driver task: 3ffdbf84, prio:23, stack:4096, core=0

The captured output for ``make monitor PRINT_FILTER="wifi esp_image:E light_driver:I"`` is the following::

    E (31) esp_image: image at 0x30000 has invalid magic byte
    I (328) wifi: wifi driver task: 3ffdbf84, prio:23, stack:4096, core=0

``make monitor PRINT_FILTER="light_driver:D esp_image:N boot:N cpu_start:N vfs:N wifi:N *:V"`` gives the following output::

    load:0x40078000,len:13564
    entry 0x40078d4c
    I (569) heap_init: Initializing. RAM available for dynamic allocation:
    D (309) light_driver: [light_init, 74]:status: 1, mode: 2

Simple Monitor
==============

Earlier versions of ESP-IDF used the pySerial_ command line program miniterm_ as a serial console program.

This program can still be run, via ``make simple_monitor``.

IDF Monitor is based on miniterm and shares the same basic keyboard shortcuts.


Known Issues with IDF Monitor
=============================

Issues Observed on Windows
~~~~~~~~~~~~~~~~~~~~~~~~~~

- If you are using the supported Windows environment and receive the error "winpty: command not found" then run ``pacman -S winpty`` to fix.
- Arrow keys and some other special keys in gdb don't work, due to Windows Console limitations.
- Occasionally when "make" exits, it may stall for up to 30 seconds before idf_monitor resumes.
- Occasionally when "gdb" is run, it may stall for a short time before it begins communicating with the gdbstub.


.. _addr2line: https://sourceware.org/binutils/docs/binutils/addr2line.html
.. _gdb: https://sourceware.org/gdb/download/onlinedocs/
.. _pySerial: https://github.com/pyserial/pyserial
.. _miniterm: https://pyserial.readthedocs.org/en/latest/tools.html#module-serial.tools.miniterm
