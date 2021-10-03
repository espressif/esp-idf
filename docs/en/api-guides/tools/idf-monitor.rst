***********
IDF Monitor
***********

:link_to_translation:`zh_CN:[中文]`

The IDF monitor tool is mainly a serial terminal program which relays serial data to and from the target device's serial port. It also provides some IDF-specific features.

This tool can be launched from an IDF project by running ``idf.py monitor``. 

For the legacy GNU Make system, run ``make monitor``.


Keyboard Shortcuts
==================

For easy interaction with IDF Monitor, use the keyboard shortcuts given in the table.

.. list-table::               
   :header-rows: 1                
   :widths: 15 25 55                
                              
   * - Keyboard Shortcut
     - Action
     - Description                
   * - Ctrl+]
     - Exit the program
     -                
   * - Ctrl+T
     - Menu escape key
     - Press and follow it by one of the keys given below.                 
   * - * Ctrl+T
     - Send the menu character itself to remote
     -                
   * - * Ctrl+]
     - Send the exit character itself to remote
     -                
   * - * Ctrl+P
     - Reset target into bootloader to pause app via RTS line
     - Resets the target, into bootloader via the RTS line (if connected), so that the board runs nothing. Useful when you need to wait for another device to startup.                
   * - * Ctrl+R
     - Reset target board via RTS
     - Resets the target board and re-starts the application via the RTS line (if connected).               
   * - * Ctrl+F
     - Build and flash the project
     - Pauses idf_monitor to run the project ``flash`` target, then resumes idf_monitor. Any changed source files are recompiled and then re-flashed. Target ``encrypted-flash`` is run if idf_monitor was started with argument ``-E``.                
   * - * Ctrl+A (or A)
     - Build and flash the app only
     - Pauses idf_monitor to run the ``app-flash`` target, then resumes idf_monitor. Similar to the ``flash`` target, but only the main app is built and re-flashed. Target ``encrypted-app-flash`` is run if idf_monitor was started with argument ``-E``.                
   * - * Ctrl+Y
     - Stop/resume log output printing on screen
     - Discards all incoming serial data while activated. Allows to quickly pause and examine log output without quitting the monitor.                
   * - * Ctrl+L
     - Stop/resume log output saved to file
     - Creates a file in the project directory and the output is written to that file until this is disabled with the same keyboard shortcut (or IDF Monitor exits).                
   * - * Ctrl+I (or I)
     - Stop/resume printing timestamps
     - IDF Monitor can print a timestamp in the beginning of each line. The timestamp format can be changed by the ``--timestamp-format`` command line argument.
   * - * Ctrl+H (or H)
     - Display all keyboard shortcuts
     -                
   * - * Ctrl+X (or X)
     - Exit the program
     -                
   * - Ctrl+C
     - Interrupt running application
     - Pauses IDF monitor and run GDB_ project debugger to debug the application at runtime. This requires :ref:CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME option to be enabled.     

Any keys pressed, other than ``Ctrl-]`` and ``Ctrl-T``, will be sent through the serial port.


IDF-specific features
=====================

Automatic Address Decoding
~~~~~~~~~~~~~~~~~~~~~~~~~~

Whenever ESP-IDF outputs a hexadecimal code address of the form ``0x4_______``, IDF Monitor uses addr2line_ to look up the location in the source code and find the function name.

.. highlight:: none

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

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
      0x400d071d: main_task at /home/gus/esp/32/idf/components/{IDF_TARGET_PATH_NAME}/./cpu_start.c:254

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

  If an ESP-IDF app crashes and panics, a register dump and backtrace is produced, such as the following::

      abort() was called at PC 0x42067cd5 on core 0

      Stack dump detected
      Core  0 register dump:
      MEPC    : 0x40386488  RA      : 0x40386b02  SP      : 0x3fc9a350  GP      : 0x3fc923c0
      TP      : 0xa5a5a5a5  T0      : 0x37363534  T1      : 0x7271706f  T2      : 0x33323130
      S0/FP   : 0x00000004  S1      : 0x3fc9a3b4  A0      : 0x3fc9a37c  A1      : 0x3fc9a3b2
      A2      : 0x00000000  A3      : 0x3fc9a3a9  A4      : 0x00000001  A5      : 0x3fc99000
      A6      : 0x7a797877  A7      : 0x76757473  S2      : 0xa5a5a5a5  S3      : 0xa5a5a5a5
      S4      : 0xa5a5a5a5  S5      : 0xa5a5a5a5  S6      : 0xa5a5a5a5  S7      : 0xa5a5a5a5
      S8      : 0xa5a5a5a5  S9      : 0xa5a5a5a5  S10     : 0xa5a5a5a5  S11     : 0xa5a5a5a5
      T3      : 0x6e6d6c6b  T4      : 0x6a696867  T5      : 0x66656463  T6      : 0x62613938
      MSTATUS : 0x00001881  MTVEC   : 0x40380001  MCAUSE  : 0x00000007  MTVAL   : 0x00000000

      MHARTID : 0x00000000

      Stack memory:
      3fc9a350: 0xa5a5a5a5 0xa5a5a5a5 0x3fc9a3b0 0x403906cc 0xa5a5a5a5 0xa5a5a5a5 0xa5a5a5a50
      3fc9a370: 0x3fc9a3b4 0x3fc9423c 0x3fc9a3b0 0x726f6261 0x20292874 0x20736177 0x6c6c61635
      3fc9a390: 0x43502074 0x34783020 0x37363032 0x20356463 0x63206e6f 0x2065726f 0x000000300
      3fc9a3b0: 0x00000030 0x36303234 0x35646337 0x3c093700 0x0000002a 0xa5a5a5a5 0x3c0937f48
      3fc9a3d0: 0x00000001 0x3c0917f8 0x3c0937d4 0x0000002a 0xa5a5a5a5 0xa5a5a5a5 0xa5a5a5a5e
      3fc9a3f0: 0x0001f24c 0x000006c8 0x00000000 0x0001c200 0xffffffff 0xffffffff 0x000000200
      3fc9a410: 0x00001000 0x00000002 0x3c093818 0x3fccb470 0xa5a5a5a5 0xa5a5a5a5 0xa5a5a5a56
      .....

  IDF Monitor adds more details to the dump by analyzing the stack dump::

    abort() was called at PC 0x42067cd5 on core 0
    0x42067cd5: __assert_func at /builds/idf/crosstool-NG/.build/riscv32-esp-elf/src/newlib/newlib/libc/stdlib/assert.c:62 (discriminator 8)

    Stack dump detected
    Core  0 register dump:
    MEPC    : 0x40386488  RA      : 0x40386b02  SP      : 0x3fc9a350  GP      : 0x3fc923c0
    0x40386488: panic_abort at /home/marius/esp-idf_2/components/esp_system/panic.c:367

    0x40386b02: rtos_int_enter at /home/marius/esp-idf_2/components/freertos/port/riscv/portasm.S:35

    TP      : 0xa5a5a5a5  T0      : 0x37363534  T1      : 0x7271706f  T2      : 0x33323130
    S0/FP   : 0x00000004  S1      : 0x3fc9a3b4  A0      : 0x3fc9a37c  A1      : 0x3fc9a3b2
    A2      : 0x00000000  A3      : 0x3fc9a3a9  A4      : 0x00000001  A5      : 0x3fc99000
    A6      : 0x7a797877  A7      : 0x76757473  S2      : 0xa5a5a5a5  S3      : 0xa5a5a5a5
    S4      : 0xa5a5a5a5  S5      : 0xa5a5a5a5  S6      : 0xa5a5a5a5  S7      : 0xa5a5a5a5
    S8      : 0xa5a5a5a5  S9      : 0xa5a5a5a5  S10     : 0xa5a5a5a5  S11     : 0xa5a5a5a5
    T3      : 0x6e6d6c6b  T4      : 0x6a696867  T5      : 0x66656463  T6      : 0x62613938
    MSTATUS : 0x00001881  MTVEC   : 0x40380001  MCAUSE  : 0x00000007  MTVAL   : 0x00000000

    MHARTID : 0x00000000

    Backtrace:
    panic_abort (details=details@entry=0x3fc9a37c "abort() was called at PC 0x42067cd5 on core 0") at /home/marius/esp-idf_2/components/esp_system/panic.c:367
    367	    *((int *) 0) = 0; // NOLINT(clang-analyzer-core.NullDereference) should be an invalid operation on targets
    #0  panic_abort (details=details@entry=0x3fc9a37c "abort() was called at PC 0x42067cd5 on core 0") at /home/marius/esp-idf_2/components/esp_system/panic.c:367
    #1  0x40386b02 in esp_system_abort (details=details@entry=0x3fc9a37c "abort() was called at PC 0x42067cd5 on core 0") at /home/marius/esp-idf_2/components/esp_system/system_api.c:108
    #2  0x403906cc in abort () at /home/marius/esp-idf_2/components/newlib/abort.c:46
    #3  0x42067cd8 in __assert_func (file=file@entry=0x3c0937f4 "", line=line@entry=42, func=func@entry=0x3c0937d4 <__func__.8540> "", failedexpr=failedexpr@entry=0x3c0917f8 "") at /builds/idf/crosstool-NG/.build/riscv32-esp-elf/src/newlib/newlib/libc/stdlib/assert.c:62
    #4  0x4200729e in app_main () at ../main/iperf_example_main.c:42
    #5  0x42086cd6 in main_task (args=<optimized out>) at /home/marius/esp-idf_2/components/freertos/port/port_common.c:133
    #6  0x40389f3a in vPortEnterCritical () at /home/marius/esp-idf_2/components/freertos/port/riscv/port.c:129

To decode each address, IDF Monitor runs the following command in the background::

  {IDF_TARGET_TOOLCHAIN_PREFIX}-addr2line -pfiaC -e build/PROJECT.elf ADDRESS

.. note::

    Set environment variable ``ESP_MONITOR_DECODE`` to ``0`` or call idf_monitor.py with specific command line option: ``idf_monitor.py --disable-address-decoding`` to disable address decoding.

Launching GDB with GDBStub
~~~~~~~~~~~~~~~~~~~~~~~~~~

By default, if esp-idf crashes, the panic handler prints relevant registers and the stack dump (similar to the ones above) over the serial port. Then it resets the board.

Furthermore, the application can be configured to run GDBStub in the background and handle the Ctrl+C event from the monitor.

Optionally, the panic handler can be configured to run GDBStub, the tool which can communicate with  GDB_ project debugger. GDBStub allows to read memory, examine call stack frames and variables, etc. It is not as versatile as JTAG debugging, but this method does not require any special hardware.

To enable GDBStub on panic, open the project configuration menu (``idf.py menuconfig``) and set :ref:`CONFIG_ESP_SYSTEM_PANIC` to ``GDBStub on panic`` or set :ref:`CONFIG_ESP_SYSTEM_PANIC` to ``GDBStub on runtime``.

In this case, if the panic handler or Ctrl+C command is triggered, as soon as IDF Monitor sees that GDBStub has loaded, it automatically pauses serial monitoring and runs GDB with necessary arguments. After GDB exits, the board is reset via the RTS serial line. If this line is not connected, please reset the board manually by pressing its Reset button.

In the background, IDF Monitor runs the following command::

  {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -ex "set serial baud BAUD" -ex "target remote PORT" -ex interrupt build/PROJECT.elf :idf_target:`Hello NAME chip`


Output Filtering
~~~~~~~~~~~~~~~~

IDF monitor can be invoked as ``idf.py monitor --print-filter="xyz"``, where ``--print-filter`` is the parameter for output filtering. The default value is an empty string, which means that everything is printed.

Restrictions on what to print can be specified as a series of ``<tag>:<log_level>`` items where ``<tag>`` is the tag string and ``<log_level>`` is a character from the set ``{N, E, W, I, D, V, *}`` referring to a level for :doc:`logging <../../api-reference/system/log>`.

For example, ``PRINT_FILTER="tag1:W"`` matches and prints only the outputs written with ``ESP_LOGW("tag1", ...)`` or at lower verbosity level, i.e. ``ESP_LOGE("tag1", ...)``. Not specifying a ``<log_level>`` or using ``*`` defaults to Verbose level.

.. note::
   Use primary logging to disable at compilation the outputs you do not need through the :doc:`logging library<../../api-reference/system/log>`. Output filtering with IDF monitor is a secondary solution which can be useful for adjusting the filtering options without recompiling the application.

Your app tags must not contain spaces, asterisks ``*``, or colons ``:`` to be compatible with the output filtering feature.

If the last line of the output in your app is not followed by a carriage return, the output filtering might get confused, i.e., the monitor starts to print the line and later finds out that the line should not have been written. This is a known issue and can be avoided by always adding a carriage return (especially when no output follows immediately afterwards).

Examples Of Filtering Rules:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``*`` can be used to match any tags. However, the string ``PRINT_FILTER="*:I tag1:E"`` with regards to ``tag1`` prints errors only, because the rule for ``tag1`` has a higher priority over the rule for ``*``.
- The default (empty) rule is equivalent to ``*:V`` because matching every tag at the Verbose level or lower means matching everything.
- ``"*:N"`` suppresses not only the outputs from logging functions, but also the prints made by ``printf``, etc. To avoid this, use ``*:E`` or a higher verbosity level.
- Rules ``"tag1:V"``, ``"tag1:v"``, ``"tag1:"``, ``"tag1:*"``, and ``"tag1"`` are equivalent.
- Rule ``"tag1:W tag1:E"`` is equivalent to ``"tag1:E"`` because any consequent occurrence of the same tag name overwrites the previous one.
- Rule ``"tag1:I tag2:W"`` only prints ``tag1`` at the Info verbosity level or lower and ``tag2`` at the Warning verbosity level or lower.
- Rule ``"tag1:I tag2:W tag3:N"`` is essentially equivalent to the previous one because ``tag3:N`` specifies that ``tag3`` should not be printed.
- ``tag3:N`` in the rule ``"tag1:I tag2:W tag3:N *:V"`` is more meaningful because without ``tag3:N`` the ``tag3`` messages could have been printed; the errors for ``tag1`` and ``tag2`` will be printed at the specified (or lower) verbosity level and everything else will be printed by default.


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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Arrow keys, as well as some other keys, do not work in GDB due to Windows Console limitations.
- Occasionally, when "idf.py" or "make" exits, it might stall for up to 30 seconds before IDF Monitor resumes.
- When "gdb" is run, it might stall for a short time before it begins communicating with the GDBStub.

.. _addr2line: https://sourceware.org/binutils/docs/binutils/addr2line.html
.. _gdb: https://sourceware.org/gdb/download/onlinedocs/
.. _pySerial: https://github.com/pyserial/pyserial
.. _miniterm: https://pyserial.readthedocs.org/en/latest/tools.html#module-serial.tools.miniterm