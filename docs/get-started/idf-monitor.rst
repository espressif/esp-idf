***********
IDF Monitor
***********

The idf_monitor tool is a Python program which runs when the ``make monitor`` target is invoked in IDF.

It is mainly a serial terminal program which relays serial data to and from the target device's serial port, but it has some other IDF-specific xfeatures.

Interacting With idf_monitor
============================

- ``Ctrl-]`` will exit the monitor.
- ``Ctrl-T Ctrl-H`` will display a help menu with all other keyboard shortcuts.
- Any other key apart from ``Ctrl-]`` and ``Ctrl-T`` is sent through the serial port.

Automatically Decoding Addresses
================================

Any time esp-idf prints a hexadecimal code address of the form ``0x4_______``, idf_monitor will use addr2line_ to look up the source code location and function name.

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

idf_monitor will augment the dump::

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

Behind the scenes, the command idf_monitor runs to decode each address is::

  xtensa-esp32-elf-addr2line -pfiaC -e build/PROJECT.elf ADDRESS


Launch GDB for GDBStub
======================

By default, if an esp-idf app crashes then the panic handler prints registers and a stack dump as shown above, and then resets.

Optionally, the panic handler can be configured to run a serial "gdb stub" which can communicate with a gdb_ debugger program and allow memory to be read, variables and stack frames examined, etc. This is not as versatile as JTAG debugging, but no special hardware is required.

To enable the gdbstub, run ``make menuconfig`` and set :ref:`CONFIG_ESP32_PANIC` option to ``Invoke GDBStub``.

If this option is enabled and idf_monitor sees the gdb stub has loaded, it will automatically pause serial monitoring and run GDB with the correct arguments. After GDB exits, the board will be reset via the RTS serial line (if this is connected.)

Behind the scenes, the command idf_monitor runs is::

  xtensa-esp32-elf-gdb -ex "set serial baud BAUD" -ex "target remote PORT" -ex interrupt build/PROJECT.elf


Quick Compile and Flash
=======================

The keyboard shortcut ``Ctrl-T Ctrl-F`` will pause idf_monitor, run the ``make flash`` target, then resume idf_monitor. Any changed source files will be recompiled before re-flashing.

The keyboard shortcut ``Ctrl-T Ctrl-A`` will pause idf-monitor, run the ``make app-flash`` target, then resume idf_monitor. This is similar to ``make flash``, but only the main app is compiled and reflashed.


Quick Reset
===========

The keyboard shortcut ``Ctrl-T Ctrl-R`` will reset the target board via the RTS line (if it is connected.)


Simple Monitor
==============

Earlier versions of ESP-IDF used the pySerial_ command line program miniterm_ as a serial console program.

This program can still be run, via ``make simple_monitor``.

idf_monitor is based on miniterm and shares the same basic keyboard shortcuts.


Known Issues with idf_monitor
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
