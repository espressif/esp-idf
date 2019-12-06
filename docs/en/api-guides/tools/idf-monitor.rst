***********
IDF Monitor
***********

:link_to_translation:`zh_CN:[中文]`

The IDF monitor tool is mainly a serial terminal program which relays serial data to and from the target device's serial port. It also provides some IDF-specific features.

This tool can be launched from an IDF project by running ``idf.py monitor``.

(For the legacy GNU Make system, run ``make monitor``.)

Keyboard Shortcuts
==================

For easy interaction with IDF Monitor, use the keyboard shortcuts given in the table.

+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Keyboard Shortcut | Action                                                 | Description                                                                                                                                                      |
+===================+========================================================+==================================================================================================================================================================+
| Ctrl+]            | Exit the program                                       |                                                                                                                                                                  |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
| Ctrl+T            | Menu escape key                                        | Press and follow it by one of the keys given below.                                                                                                              |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+T         | Send the menu character itself to remote               |                                                                                                                                                                  |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+]         | Send the exit character itself to remote               |                                                                                                                                                                  |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+P         | Reset target into bootloader to pause app via RTS line | Resets the target, into bootloader via the RTS line (if connected), so that the board runs nothing. Useful when you need to wait for another device to startup.  |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+R         | Reset target board via RTS                             | Resets the target board and re-starts the application via the RTS line (if connected).                                                                           |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+F         | Build and flash the project                            | Pauses idf_monitor to run the project ``flash`` target, then resumes idf_monitor. Any changed source files are recompiled and then re-flashed.                   |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+A (or A)  | Build and flash the app only                           | Pauses idf_monitor to run the ``app-flash`` target, then resumes idf_monitor. Similar to the ``flash`` target, but only the main app is built and re-flashed.    |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+Y         | Stop/resume log output printing on screen              | Discards all incoming serial data while activated. Allows to quickly pause and examine log output without quitting the monitor.                                  |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+L         | Stop/resume log output saved to file                   | Creates a file in the project directory and the output is written to that file until this is disabled with the same keyboard shortcut (or IDF Monitor exits).    |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+H (or H)  | Display all keyboard shortcuts                         |                                                                                                                                                                  |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|  - Ctrl+X (or X)  | Exit the program                                       |                                                                                                                                                                  |
+-------------------+--------------------------------------------------------+------------------------------------------------------------------------------------------------------------------------------------------------------------------+

Any keys pressed, other than ``Ctrl-]`` and ``Ctrl-T``, will be sent through the serial port.


IDF-specific features
=====================


Automatic Address Decoding
~~~~~~~~~~~~~~~~~~~~~~~~~~

Whenever ESP-IDF outputs a hexadecimal code address of the form ``0x4_______``, IDF Monitor uses addr2line_ to look up the location in the source code and find the function name.

.. highlight:: none

If an ESP-IDF app crashes and panics, a register dump and backtrace is produced, such as the following::

    Guru Meditation Error of type StoreProhibited occurred on core  0. Exception was unhandled.
    Register dump:
    PC      : 0x400f360d  PS      : 0x00060330  A0      : 0x800dbf56  A1      : 0x3ffb7e00
    A2      : 0x3ffb136c  A3      : 0x00000005  A4      : 0x00000000  A5      : 0x00000000
    A6      : 0x00000000  A7      : 0x00000080  A8      : 0x00000000  A9      : 0x3ffb7dd0
    A10     : 0x00000003  A11     : 0x00060f23  A12     : 0x00060f20  A13     : 0x3ffba6d0
    A14     : 0x00000047  A15     : 0x0000000f  SAR     : 0x00000019  EXCCAUSE: 0x0000001d
    EXCVADDR: 0x00000000  LBEG    : 0x4000c46c  LEND    : 0x4000c477  LCOUNT  : 0x00000000

    Backtrace: 0x400f360d:0x3ffb7e00 0x400dbf56:0x3ffb7e20 0x400dbf5e:0x3ffb7e40 0x400dbf82:0x3ffb7e60 0x400d071d:0x3ffb7e90

IDF Monitor adds more details to the dump::

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

To decode each address, IDF Monitor runs the following command in the background::

  xtensa-esp32-elf-addr2line -pfiaC -e build/PROJECT.elf ADDRESS


Launching GDB with GDBStub
~~~~~~~~~~~~~~~~~~~~~~~~~~

By default, if esp-idf crashes, the panic handler prints relevant registers and the stack dump (similar to the ones above) over the serial port. Then it resets the board.

Optionally, the panic handler can be configured to run GDBStub, the tool which can communicate with  GDB_ project debugger. GDBStub allows to read memory, examine call stack frames and variables, etc. It is not as versatile as JTAG debugging, but this method does not require any special hardware.

To enable GDBStub, open the project configuration menu (``idf.py menuconfig``) and set :ref:`CONFIG_ESP32_PANIC` to ``Invoke GDBStub``.

In this case, if the panic handler is triggered, as soon as IDF Monitor sees that GDBStub has loaded, it automatically pauses serial monitoring and runs GDB with necessary arguments. After GDB exits, the board is reset via the RTS serial line. If this line is not connected, please reset the board manually by pressing its Reset button.

In the background, IDF Monitor runs the following command::

  xtensa-esp32-elf-gdb -ex "set serial baud BAUD" -ex "target remote PORT" -ex interrupt build/PROJECT.elf


Output Filtering
~~~~~~~~~~~~~~~~

IDF monitor can be invoked as ``idf.py monitor --print-filter="xyz"``, where ``--print-filter`` is the parameter for output filtering. The default value is an empty string, which means that everything is printed.

Restrictions on what to print can be specified as a series of ``<tag>:<log_level>`` items where ``<tag>`` is the tag string and ``<log_level>`` is a character from the set ``{N, E, W, I, D, V, *}`` referring to a level for :doc:`logging <../../api-reference/system/log>`.

For example, ``PRINT_FILTER="tag1:W"`` matches and prints only the outputs written with ``ESP_LOGW("tag1", ...)`` or at lower verbosity level, i.e. ``ESP_LOGE("tag1", ...)``. Not specifying a ``<log_level>`` or using ``*`` defaults to Verbose level.

.. note::
   Use primary logging to disable at compilation the outputs you do not
   need through the :doc:`logging library<../../api-reference/system/log>`.
   Output filtering with IDF monitor is a secondary solution
   which can be useful for adjusting the filtering options without
   recompiling the application.

Your app tags must not contain spaces, asterisks ``*``, 
and semicolons ``:`` to be compatible with the output filtering feature.

If the last line of the output in your app is not followed by a carriage return, the output filtering might get confused, i.e., the monitor starts to print the line and later finds out that the line should not have been written. This is a known issue and can be avoided by always adding a carriage return (especially when no output follows immediately afterwards).

Examples Of Filtering Rules:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``*`` can be used to match any tags. However, the string
  ``PRINT_FILTER="*:I tag1:E"`` with regards to ``tag1`` prints errors
  only, because the rule for ``tag1`` has a higher priority over the rule for ``*``.
- The default (empty) rule is equivalent to ``*:V`` because matching every tag
  at the Verbose level or lower means matching everything.
- ``"*:N"`` suppresses not only the outputs from logging functions, but also
  the prints made by ``printf``, etc. To avoid this, use ``*:E`` or a higher verbosity level.
- Rules ``"tag1:V"``, ``"tag1:v"``, ``"tag1:"``, ``"tag1:*"``, and ``"tag1"``
  are equivalent.
- Rule ``"tag1:W tag1:E"`` is equivalent to ``"tag1:E"`` because any
  consequent occurrence of the same tag name overwrites the previous one.
- Rule ``"tag1:I tag2:W"`` only prints ``tag1`` at the Info verbosity level or
  lower and ``tag2`` at the Warning verbosity level or lower.
- Rule ``"tag1:I tag2:W tag3:N"`` is essentially equivalent to the previous
  one because ``tag3:N`` specifies that ``tag3`` should not be printed.
- ``tag3:N`` in the rule ``"tag1:I tag2:W tag3:N *:V"`` is more meaningful because
  without ``tag3:N`` the ``tag3`` messages could have been printed;
  the errors for ``tag1`` and ``tag2`` will be printed at the specified (or lower)
  verbosity level and everything else will be printed by default.



A More Complex Filtering Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following log snippet was acquired without any filtering options::

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

The captured output for the filtering options ``PRINT_FILTER="wifi esp_image:E light_driver:I"`` is given below::

    E (31) esp_image: image at 0x30000 has invalid magic byte
    I (328) wifi: wifi driver task: 3ffdbf84, prio:23, stack:4096, core=0

``The options ``PRINT_FILTER="light_driver:D esp_image:N boot:N cpu_start:N vfs:N wifi:N *:V"`` show the following output::

    load:0x40078000,len:13564
    entry 0x40078d4c
    I (569) heap_init: Initializing. RAM available for dynamic allocation:
    D (309) light_driver: [light_init, 74]:status: 1, mode: 2


Known Issues with IDF Monitor
=============================

Issues Observed on Windows
~~~~~~~~~~~~~~~~~~~~~~~~~~

- If in the Windows environment you receive the error "winpty: command not found", fix it by running ``pacman -S winpty``.
- Arrow keys, as well as some other keys, do not work in GDB due to Windows Console limitations.
- Occasionally, when "idf.py" or "make" exits, it might stall for up to 30 seconds before IDF Monitor resumes.
- When "gdb" is run, it might stall for a short time before it begins communicating with the GDBStub.


.. _addr2line: https://sourceware.org/binutils/docs/binutils/addr2line.html
.. _gdb: https://sourceware.org/gdb/download/onlinedocs/
.. _pySerial: https://github.com/pyserial/pyserial
.. _miniterm: https://pyserial.readthedocs.org/en/latest/tools.html#module-serial.tools.miniterm
