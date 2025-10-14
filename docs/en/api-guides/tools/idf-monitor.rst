***********
IDF Monitor
***********

:link_to_translation:`zh_CN:[中文]`

IDF Monitor uses the esp-idf-monitor_ package as a serial terminal program which relays serial data to and from the target device's serial port. It also provides some ESP-IDF-specific features.

IDF Monitor can be launched from an ESP-IDF project by running ``idf.py monitor``.

Keyboard Shortcuts
==================

For easy interaction with IDF Monitor, use the keyboard shortcuts given in the table. These keyboard shortcuts can be customized, for more details see `Configuration File`_ section.

.. list-table::
   :header-rows: 1
   :widths: 15 25 55

   * - Keyboard Shortcut
     - Action
     - Description
   * - Ctrl + ]
     - Exit the program
     -
   * - Ctrl + T
     - Menu escape key
     - Press and follow it by one of the keys given below.
   * - * Ctrl + T
     - Send the menu character itself to remote
     -
   * - * Ctrl + ]
     - Send the exit character itself to remote
     -
   * - * Ctrl + P
     - Reset target into bootloader to pause app via RTS and DTR lines
     - Reset the target into the bootloader using the RTS and DTR lines (if connected). This stops the board from executing the application, making it useful when waiting for another device to start. For additional details, refer to :ref:`target-reset-into-bootloader`.
   * - * Ctrl + R
     - Reset target board via RTS
     - Reset the target board and re-starts the application via the RTS line (if connected).
   * - * Ctrl + F
     - Build and flash the project
     - Pause idf_monitor to run the project ``flash`` target, then resumes idf_monitor. Any changed source files are recompiled and then re-flashed. Target ``encrypted-flash`` is run if idf_monitor was started with argument ``-E``.
   * - * Ctrl + A (or A)
     - Build and flash the app only
     - Pause idf_monitor to run the ``app-flash`` target, then resumes idf_monitor. Similar to the ``flash`` target, but only the main app is built and re-flashed. Target ``encrypted-app-flash`` is run if idf_monitor was started with argument ``-E``.
   * - * Ctrl + Y
     - Stop/resume log output printing on screen
     - Discard all incoming serial data while activated. Allows to quickly pause and examine log output without quitting the monitor.
   * - * Ctrl + L
     - Stop/resume log output saved to file
     - Create a file in the project directory and the output is written to that file until this is disabled with the same keyboard shortcut (or IDF Monitor exits).
   * - * Ctrl + I (or I)
     - Stop/resume printing timestamps
     - IDF Monitor can print a timestamp in the beginning of each line. The timestamp format can be changed by the ``--timestamp-format`` command line argument.
   * - * Ctrl + H (or H)
     - Display all keyboard shortcuts
     -
   * - * Ctrl + X (or X)
     - Exit the program
     -
   * - Ctrl + C
     - Interrupt running application
     - Pause IDF Monitor and runs GDB_ project debugger to debug the application at runtime. This requires :ref:`CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME` option to be enabled.

Any keys pressed, other than ``Ctrl-]`` and ``Ctrl-T``, will be sent through the serial port.


Automatic Coloring
==================

IDF Monitor automatically colors the output based on the log level. This feature reduces the number of bytes transferred over the serial console by avoiding redundant log formatting, which can improve performance by reducing latency in log transmission. Other benefits include adding colors to precompiled libraries (such as Wi-Fi) and reduced binary size of the application.

The automatic coloring is enabled by default. To disable it, use the command line option ``--disable-auto-color``.

The coloring is done based on the log level followed by optional timestamp and tag. For option to enable coloring on the {IDF_TARGET_NAME} side, see :ref:`CONFIG_LOG_COLORS`.

For more details on the log, see :doc:`Logging <../../api-reference/system/log>`.

.. note::

    The automatic coloring will not work properly if the message contains new lines. In this case the IDF Monitor will only color the first line of the message.


ESP-IDF-specific Features
=========================

Automatic Address Decoding
~~~~~~~~~~~~~~~~~~~~~~~~~~

Whenever the chip outputs a hexadecimal address that points to executable code, IDF monitor looks up the location in the source code (file name and line number) and prints the location on the next line in yellow.

.. highlight:: none

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

  If an ESP-IDF app crashes and panics, a register dump and backtrace are produced, such as the following::

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

  If an ESP-IDF app crashes and panics, a register dump and backtrace are produced, such as the following::

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
    367     *((int *) 0) = 0; // NOLINT(clang-analyzer-core.NullDereference) should be an invalid operation on targets
    #0  panic_abort (details=details@entry=0x3fc9a37c "abort() was called at PC 0x42067cd5 on core 0") at /home/marius/esp-idf_2/components/esp_system/panic.c:367
    #1  0x40386b02 in esp_system_abort (details=details@entry=0x3fc9a37c "abort() was called at PC 0x42067cd5 on core 0") at /home/marius/esp-idf_2/components/esp_system/system_api.c:108
    #2  0x403906cc in abort () at /home/marius/esp-idf_2/components/newlib/abort.c:46
    #3  0x42067cd8 in __assert_func (file=file@entry=0x3c0937f4 "", line=line@entry=42, func=func@entry=0x3c0937d4 <__func__.8540> "", failedexpr=failedexpr@entry=0x3c0917f8 "") at /builds/idf/crosstool-NG/.build/riscv32-esp-elf/src/newlib/newlib/libc/stdlib/assert.c:62
    #4  0x4200729e in app_main () at ../main/iperf_example_main.c:42
    #5  0x42086cd6 in main_task (args=<optimized out>) at /home/marius/esp-idf_2/components/freertos/port/port_common.c:133
    #6  0x40389f3a in vPortEnterCritical () at /home/marius/esp-idf_2/components/freertos/port/riscv/port.c:129

To decode each address, IDF Monitor runs the following command in the background::

  {IDF_TARGET_TOOLCHAIN_PREFIX}-addr2line -pfiaC -e build/PROJECT.elf ADDRESS

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

  If an address is not matched in the app source code, IDF monitor also checks the ROM code. Instead of printing the source file name and line number, only the function name followed by ``in ROM`` is displayed::

    abort() was called at PC 0x40007c69 on core 0
    0x40007c69: ets_write_char in ROM

    Backtrace: 0x40081656:0x3ffb4ac0 0x40085729:0x3ffb4ae0 0x4008a7ce:0x3ffb4b00 0x40007c69:0x3ffb4b70 0x40008148:0x3ffb4b90 0x400d51d7:0x3ffb4c20 0x400e31bc:0x3ffb4c50 0x40087bc5:0x3ffb4c80
    0x40081656: panic_abort at /Users/espressif/esp-idf/components/esp_system/panic.c:452
    0x40085729: esp_system_abort at /Users/espressif/esp-idf/components/esp_system/port/esp_system_chip.c:90
    0x4008a7ce: abort at /Users/espressif/esp-idf/components/newlib/abort.c:38
    0x40007c69: ets_write_char in ROM
    0x40008148: ets_printf in ROM
    0x400d51d7: app_main at /Users/espressif/esp-idf/examples/get-started/hello_world/main/hello_world_main.c:49
    0x400e31bc: main_task at /Users/espressif/esp-idf/components/freertos/app_startup.c:208 (discriminator 13)
    0x40087bc5: vPortTaskWrapper at /Users/espressif/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:162
    .....

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

  If an address is not matched in the app source code, IDF monitor also checks the ROM code. Instead of printing the source file name and line number, only the function name followed by ``in ROM`` is displayed::

    abort() was called at PC 0x400481c1 on core 0
    0x400481c1: ets_rsa_pss_verify in ROM

    Stack dump detected
    Core  0 register dump:
    MEPC    : 0x4038051c  RA      : 0x40383840  SP      : 0x3fc8f6b0  GP      : 0x3fc8b000
    0x4038051c: panic_abort at /Users/espressif/esp-idf/components/esp_system/panic.c:452
    0x40383840: __ubsan_include at /Users/espressif/esp-idf/components/esp_system/ubsan.c:313

    TP      : 0x3fc8721c  T0      : 0x37363534  T1      : 0x7271706f  T2      : 0x33323130
    S0/FP   : 0x00000004  S1      : 0x3fc8f714  A0      : 0x3fc8f6dc  A1      : 0x3fc8f712
    A2      : 0x00000000  A3      : 0x3fc8f709  A4      : 0x00000001  A5      : 0x3fc8c000
    A6      : 0x7a797877  A7      : 0x76757473  S2      : 0x00000000  S3      : 0x3fc8f750
    S4      : 0x3fc8f7e4  S5      : 0x00000000  S6      : 0x400481b0  S7      : 0x3c025841
    0x400481b0: ets_rsa_pss_verify in ROM
    .....

The ROM ELF file is automatically loaded from a location based on the ``IDF_PATH`` and ``ESP_ROM_ELF_DIR`` environment variables. This can be overridden by calling ``esp_idf_monitor`` and providing a path to a specific ROM ELF file: ``python -m esp_idf_monitor --rom-elf-file [path to ROM ELF file]``.

.. note::

    Set environment variable ``ESP_MONITOR_DECODE`` to ``0`` or call esp_idf_monitor with specific command line option: ``python -m esp_idf_monitor --disable-address-decoding`` to disable address decoding.

Target Reset on Connection
~~~~~~~~~~~~~~~~~~~~~~~~~~

By default, IDF Monitor will reset the target when connecting to it. The reset of the target chip is performed using the DTR and RTS serial lines. To prevent IDF Monitor from automatically resetting the target on connection, call IDF Monitor with the ``--no-reset`` option (e.g., ``idf.py monitor --no-reset``). You can also set the environment variable ``ESP_IDF_MONITOR_NO_RESET`` to ``1`` to achieve the same behavior.

.. note::

    The ``--no-reset`` option applies the same behavior even when connecting IDF Monitor to a particular port (e.g., ``idf.py monitor --no-reset -p [PORT]``).


.. _target-reset-into-bootloader:

Target Reset into Bootloader
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

IDF Monitor provides the capability to reset a chip into the bootloader using a pre-defined reset sequence that has been tuned to work in most environments. Additionally, users have the flexibility to set a custom reset sequence, allowing for fine-tuning and adaptability to diverse scenarios.

Using Pre-defined Reset Sequence
--------------------------------

IDF Monitor's default reset sequence is designed to work seamlessly across a wide range of environments. To trigger a reset into the bootloader using the default sequence, no additional configuration is required.

Custom Reset Sequence
---------------------

For more advanced users or specific use cases, IDF Monitor supports the configuration of a custom reset sequence using :ref:`configuration-file`. This is particularly useful in extreme edge cases where the default sequence may not suffice.

If you would like to use a custom reset sequence, take a look at the `IDF Monitor documentation`_ for more details.

Launching GDB with GDBStub
~~~~~~~~~~~~~~~~~~~~~~~~~~

GDBStub is a useful runtime debugging feature that runs on the target and connects to the host over the serial port to receive debugging commands. GDBStub supports commands such as reading memory and variables, examining call stack frames etc. Although GDBStub is less versatile than JTAG debugging, it does not require any special hardware (such as a JTAG to USB bridge) as communication is done entirely over the serial port.

A target can be configured to run GDBStub in the background by setting the :ref:`CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME`. GDBStub will run in the background until a ``Ctrl+C`` message is sent over the serial port and causes the GDBStub to break (i.e., stop the execution of) the program, thus allowing GDBStub to handle debugging commands.

Furthermore, the panic handler can be configured to run GDBStub on a crash by setting the :ref:`CONFIG_ESP_SYSTEM_PANIC` to ``GDBStub on panic``. When a crash occurs, GDBStub will output a special string pattern over the serial port to indicate that it is running.

In both cases (i.e., sending the ``Ctrl+C`` message, or receiving the special string pattern), IDF Monitor will automatically launch GDB in order to allow the user to send debugging commands. After GDB exits, the target is reset via the RTS serial line. If this line is not connected, users can reset their target (by pressing the board's Reset button).

.. note::

    In the background, IDF Monitor runs the following command to launch GDB::

        {IDF_TARGET_TOOLCHAIN_PREFIX}-gdb -ex "set serial baud BAUD" -ex "target remote PORT" -ex interrupt build/PROJECT.elf :idf_target:`Hello NAME chip`


Output Filtering
~~~~~~~~~~~~~~~~

IDF monitor can be invoked as ``idf.py monitor --print-filter="xyz"``, where ``--print-filter`` is the parameter for output filtering. The default value is an empty string, which means that everything is printed. Filtering can also be configured using the ``ESP_IDF_MONITOR_PRINT_FILTER`` environment variable.

.. note::

   When using both the environment variable ``ESP_IDF_MONITOR_PRINT_FILTER`` and the argument ``--print-filter``, the setting from the CLI argument will take precedence.

Restrictions on what to print can be specified as a series of ``<tag>:<log_level>`` items where ``<tag>`` is the tag string and ``<log_level>`` is a character from the set ``{N, E, W, I, D, V, *}`` referring to a level for :doc:`logging <../../api-reference/system/log>`.

For example, ``--print_filter="tag1:W"`` matches and prints only the outputs written with ``ESP_LOGW("tag1", ...)`` or at lower verbosity level, i.e., ``ESP_LOGE("tag1", ...)``. Not specifying a ``<log_level>`` or using ``*`` defaults to a Verbose level.

.. note::

   Use primary logging to disable at compilation the outputs you do not need through the :doc:`logging library <../../api-reference/system/log>`. Output filtering with the IDF monitor is a secondary solution that can be useful for adjusting the filtering options without recompiling the application.

Your app tags must not contain spaces, asterisks ``*``, or colons ``:`` to be compatible with the output filtering feature.

If the last line of the output in your app is not followed by a carriage return, the output filtering might get confused, i.e., the monitor starts to print the line and later finds out that the line should not have been written. This is a known issue and can be avoided by always adding a carriage return (especially when no output follows immediately afterwards).

Examples of Filtering Rules:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``*`` can be used to match any tags. However, the string ``--print_filter="*:I tag1:E"`` with regards to ``tag1`` prints errors only, because the rule for ``tag1`` has a higher priority over the rule for ``*``.
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

The captured output for the filtering options ``--print_filter="wifi esp_image:E light_driver:I"`` is given below::

    E (31) esp_image: image at 0x30000 has invalid magic byte
    I (328) wifi: wifi driver task: 3ffdbf84, prio:23, stack:4096, core=0

The options ``--print_filter="light_driver:D esp_image:N boot:N cpu_start:N vfs:N wifi:N *:V"`` show the following output::

    load:0x40078000,len:13564
    entry 0x40078d4c
    I (569) heap_init: Initializing. RAM available for dynamic allocation:
    D (309) light_driver: [light_init, 74]:status: 1, mode: 2


.. _configuration-file:

Configuration File
==================

``esp-idf-monitor`` offers option to change its default behavior with configuration file. This file can be used for example to set custom key bindings, or set a custom reset sequence for resetting the chip into bootloader mode.

For more details on the configuration file, see the `IDF Monitor documentation`_.


Known Issues with IDF Monitor
=============================

The following issues are currently known:

- Autocoloring cannot detect the log level if the message contains new lines. In this case, the IDF Monitor will only color the first line of the message.

  To work around this issue, enable :ref:`CONFIG_LOG_COLORS` in menuconfig. Please note that this might have some impact on binary size and performance.

- On Windows, if the terminal is closed without first closing the IDF Monitor, some drivers may fail to release the serial port. To release the port, you may need to unplug and replug the USB cable, or in some cases even restart the computer. This issue has been observed with the CH9102 USB-to-UART bridge. Other drivers, such as CP210x and CH340, should work fine.

  To prevent this issue, make sure to close the IDF Monitor properly before exiting the terminal, or consider using an alternative USB-to-UART bridge.

If you experience any other issues while using IDF Monitor, check our `GitHub repository <https://github.com/espressif/esp-idf-monitor/issues>`_ for a list of known issues and their current status. If you come across a problem that hasn't been documented yet, we encourage you to create a new issue report.

.. _esp-idf-monitor: https://github.com/espressif/esp-idf-monitor
.. _IDF Monitor documentation: https://github.com/espressif/esp-idf-monitor/blob/v1.5.0/README.md#documentation
.. _gdb: https://sourceware.org/gdb/download/onlinedocs/
