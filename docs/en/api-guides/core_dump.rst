Core Dump
=========

:link_to_translation:`zh_CN:[中文]`

Overview
--------

A core dump is a set of software state information that is automatically saved by the panic handler when a fatal error occurs. Core dumps are useful for conducting post-mortem analysis of the software's state at the moment of failure. ESP-IDF provides support for generating core dumps.

A core dump contains snapshots of all tasks in the system at the moment of failure, where each snapshot includes a task's control block (TCB) and stack. By analyzing the task snapshots, it is possible to find out what task, at what instruction (line of code), and what call stack of that task lead to the crash. It is also possible to dump the contents of variables on demand, provided those variables are assigned special core dump attributes.

Core dump data is saved to a core dump file according to a particular format, see :doc:`Core dump internals <core_dump_internals>` for more details. However, ESP-IDF's ``idf.py`` command provides special subcommands to decode and analyze the core dump file.


Configurations
--------------

Destination
^^^^^^^^^^^

The :ref:`CONFIG_ESP_COREDUMP_TO_FLASH_OR_UART` option enables or disables core dump, and selects the core dump destination if enabled. When a crash occurs, the generated core dump file can either be saved to flash, or output to a connected host over UART.


Format & Size
^^^^^^^^^^^^^

The :ref:`CONFIG_ESP_COREDUMP_DATA_FORMAT` option controls the format of the core dump file, namely ELF format or Binary format.

The ELF format contains extended features and allows more information regarding erroneous tasks and crashed software to be saved. However, using the ELF format causes the core dump file to be larger. This format is recommended for new software designs and is flexible enough to be extended in future revisions to save more information.

The Binary format is kept for compatibility reasons. Binary format core dump files are smaller while provide better performance.

The :ref:`CONFIG_ESP_COREDUMP_MAX_TASKS_NUM` option configures the number of task snapshots saved by the core dump.

Core dump data integrity checking is supported via the ``Components`` > ``Core dump`` > ``Core dump data integrity check`` option.

.. only:: esp32

    Data Integrity Check
    ^^^^^^^^^^^^^^^^^^^^

    Core dump files include a checksum, which can be used to verify the integrity of the core dump file, i.e., the file has not been corrupted. The :ref:`CONFIG_ESP_COREDUMP_CHECKSUM` option controls the type of checksum, namely CRC32 or SHA256 (only supported in the ELF format).

    The CRC32 option provides better calculation performance and consumes less memory for storage.

    The SHA256 hash algorithm provides a greater probability of detecting corruption than a CRC32 with multiple-bit errors.


Reserved Stack Size
^^^^^^^^^^^^^^^^^^^

Core dump routines run from a separate stack due to core dump itself needing to parse and save all other task stacks. The :ref:`CONFIG_ESP_COREDUMP_STACK_SIZE` option controls the size of the core dump's stack in number of bytes.

Setting this option to 0 bytes will cause the core dump routines to run from the ISR stack, thus saving a bit of memory. Setting the option greater than zero will cause a separate stack to be instantiated.

.. note::

   If a separate stack is used, the recommended stack size should be larger than 1300 bytes to ensure that the core dump routines themselves do not cause a stack overflow.


.. only:: not esp32c5

    Core Dump Memory Regions
    ^^^^^^^^^^^^^^^^^^^^^^^^

    By default, core dumps typically save CPU registers, tasks data and summary of the panic reason. When the :ref:`CONFIG_ESP_COREDUMP_CAPTURE_DRAM` option is selected, ``.bss`` and ``.data`` sections and ``heap`` data will also be part of the dump.

    For a better debugging experience, it is recommended to dump these sections. However, this will result in a larger coredump file. The required additional storage space may vary based on the amount of DRAM the application uses.

    .. only:: SOC_SPIRAM_SUPPORTED

        .. note::

            Apart from the crashed task's TCB and stack, data located in the external RAM will not be stored in the core dump file, this include variables defined with ``EXT_RAM_BSS_ATTR`` or ``EXT_RAM_NOINIT_ATTR`` attributes, as well as any data stored in the ``extram_bss`` section.

    .. note::

        This feature is only enabled when using the ELF file format.

Core Dump to Flash
------------------

When the core dump file is saved to flash, the file is saved to a special core dump partition in flash. Specifying the core dump partition will reserve space on the flash chip to store the core dump file.

The core dump partition is automatically declared when using the default partition table provided by ESP-IDF. However, when using a custom partition table, you need to declare the core dump partition, as illustrated below:

.. code-block:: none

    # Name,   Type, SubType, Offset,  Size
    # Note: if you have increased the bootloader size, make sure to update the offsets to avoid overlap
    nvs,      data, nvs,     0x9000,  0x6000
    phy_init, data, phy,     0xf000,  0x1000
    factory,  app,  factory, 0x10000, 1M
    coredump, data, coredump,,        64K

.. important::

    If :doc:`../security/flash-encryption` is enabled on the device, please add an ``encrypted`` flag to the core dump partition declaration. Please note that the core dump cannot be read from encrypted partitions using ``idf.py coredump-info`` or ``idf.py coredump-debug`` commands.
    It is recommended to read the core dump from ESP which will automatically decrypt the partition and send it for analysis, which can be done by running e.g. ``idf.py coredump-info -c <path-to-core-dump>``.

    .. code-block:: none

        coredump, data, coredump,,       64K, encrypted

There are no special requirements for the partition name. It can be chosen according to the application's needs, but the partition type should be ``data`` and the sub-type should be ``coredump``. Also, when choosing partition size, note that the core dump file introduces a constant overhead of 20 bytes and a per-task overhead of 12 bytes. This overhead does not include the size of TCB and stack for every task. So the partition size should be at least ``20 + max tasks number x (12 + TCB size + max task stack size)`` bytes.

An example of the generic command to analyze core dump from flash is:

.. code-block:: bash

    idf.py coredump-info

or

.. code-block:: bash

    idf.py coredump-debug


.. note::

    The ``idf.py coredump-info`` and ``idf.py coredump-debug`` commands are wrappers around the `esp-coredump` tool for easier use in the ESP-IDF environment. For more information see :ref:`core_dump_commands` section.


Core Dump to UART
-----------------

When the core dump file is output to UART, the output file is Base64-encoded. The :ref:`CONFIG_ESP_COREDUMP_DECODE` option allows for selecting whether the output file is automatically decoded by the ESP-IDF monitor or kept encoded for manual decoding.


Automatic Decoding
^^^^^^^^^^^^^^^^^^

If :ref:`CONFIG_ESP_COREDUMP_DECODE` is set to automatically decode the UART core dump, ESP-IDF monitor will automatically decode the data, translate any function addresses to source code lines, and display it in the monitor. The output to ESP-IDF monitor would resemble the following output:

The :ref:`CONFIG_ESP_COREDUMP_UART_DELAY` allows for an optional delay to be added before the core dump file is output to UART.

.. code-block:: none

    ===============================================================
    ==================== ESP32 CORE DUMP START ====================

    Crashed task handle: 0x3ffafba0, name: 'main', GDB name: 'process 1073413024'
    Crashed task is not in the interrupt context
    Panic reason: abort() was called at PC 0x400d66b9 on core 0

    ================== CURRENT THREAD REGISTERS ===================
    exccause       0x1d (StoreProhibitedCause)
    excvaddr       0x0
    epc1           0x40084013
    epc2           0x0
    ...
    ==================== CURRENT THREAD STACK =====================
    #0  0x4008110d in panic_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/panic.c:472
    #1  0x4008510c in esp_system_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/port/esp_system_chip.c:93
    ...
    ======================== THREADS INFO =========================
      Id   Target Id          Frame
    * 1    process 1073413024 0x4008110d in panic_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/panic.c:472
      2    process 1073413368 vPortTaskWrapper (pxCode=0x0, pvParameters=0x0) at /builds/espressif/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:133
    ...
           TCB             NAME PRIO C/B  STACK USED/FREE
    ---------- ---------------- -------- ----------------
    0x3ffafba0             main      1/1         368/3724
    0x3ffafcf8            IDLE0      0/0         288/1240
    0x3ffafe50            IDLE1      0/0         416/1108
    ...
    ==================== THREAD 1 (TCB: 0x3ffafba0, name: 'main') =====================
    #0  0x4008110d in panic_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/panic.c:472
    #1  0x4008510c in esp_system_abort (details=0x3ffb4f0b "abort() was called at PC 0x400d66b9 on core 0") at /builds/espressif/esp-idf/components/esp_system/port/esp_system_chip.c:93
    ...
    ==================== THREAD 2 (TCB: 0x3ffafcf8, name: 'IDLE0') =====================
    #0  vPortTaskWrapper (pxCode=0x0, pvParameters=0x0) at /builds/espressif/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:133
    #1  0x40000000 in ?? ()
    ...
    ======================= ALL MEMORY REGIONS ========================
    Name   Address   Size   Attrs
    ...
    .iram0.vectors 0x40080000 0x403 R XA
    .iram0.text 0x40080404 0xb8ab R XA
    .dram0.data 0x3ffb0000 0x2114 RW A
    ...
    ===================== ESP32 CORE DUMP END =====================
    ===============================================================


Manual Decoding
^^^^^^^^^^^^^^^

If you set :ref:`CONFIG_ESP_COREDUMP_DECODE` to no decoding, then the raw  Base64-encoded body of core dump is output to UART between the following header and footer of the UART output:

.. code-block:: none

    ================= CORE DUMP START =================
    <body of Base64-encoded core dump, save it to file on disk>
    ================= CORE DUMP END ===================

It is advised to manually save the core dump text body to a file. The ``CORE DUMP START`` and ``CORE DUMP END`` lines must not be included in a core dump text file. The saved text can the be decoded using the following command:

.. code-block:: bash

    idf.py coredump-info -c </path/to/saved/base64/text>

or

.. code-block:: bash

    idf.py coredump-debug -c </path/to/saved/base64/text>


.. _core_dump_commands:

Core Dump Commands
------------------

ESP-IDF provides special commands to help to retrieve and analyze core dumps:

* ``idf.py coredump-info`` - prints crashed task's registers, call stack, list of available tasks in the system, memory regions, and contents of memory stored in core dump (TCBs and stacks).
* ``idf.py coredump-debug`` - creates core dump ELF file and runs GDB debug session with this file. You can examine memory, variables, and task states manually. Note that since not all memory is saved in the core dump, only the values of variables allocated on the stack are meaningful.

For advanced users who want to pass additional arguments or use custom ELF files, it is possible to use the `esp-coredump <https://github.com/espressif/esp-coredump>`_ tool directly. For more information, use in ESP-IDF environment:

.. code-block:: bash

    esp-coredump --help


ROM Functions in Backtraces
---------------------------

It is a possible that at the moment of a crash, some tasks and/or the crashed task itself have one or more ROM functions in their call stacks. Since ROM is not part of the program ELF, it is impossible for GDB to parse such call stacks due to GDB analyzing functions' prologues to decode backtraces. Thus, call stack parsing will break with an error message upon the first ROM function that is encountered.

To overcome this issue, the `ROM ELF <https://github.com/espressif/esp-rom-elfs/releases>`_ provided by Espressif is loaded automatically by ESP-IDF monitor based on the target and its revision. More details about ROM ELFs can be found in `esp-rom-elfs <https://github.com/espressif/esp-rom-elfs/blob/master/README.md>`_.


Dumping Variables on Demand
---------------------------

Sometimes you want to read the last value of a variable to understand the root cause of a crash. Core dump supports retrieving variable data over GDB by applying special attributes to declared variables.


Supported Notations and RAM Regions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list::

   * ``COREDUMP_DRAM_ATTR`` places the variable into the DRAM area, which is included in the dump.
   :SOC_RTC_FAST_MEM_SUPPORTED or SOC_RTC_SLOW_MEM_SUPPORTED: * ``COREDUMP_RTC_ATTR`` places the variable into the RTC area, which is included in the dump.
   :SOC_RTC_FAST_MEM_SUPPORTED: * ``COREDUMP_RTC_FAST_ATTR`` places the variable into the RTC_FAST area, which is included in the dump.


Example
^^^^^^^

1. In :ref:`project-configuration-menu`, enable :ref:`COREDUMP TO FLASH <CONFIG_ESP_COREDUMP_TO_FLASH_OR_UART>`, then save and exit.

2. In your project, create a global variable in the DRAM area, such as:

.. code-block:: bash

   // uint8_t global_var;
   COREDUMP_DRAM_ATTR uint8_t global_var;

3. In the main application, set the variable to any value and ``assert(0)`` to cause a crash.

.. code-block:: bash

   global_var = 25;
   assert(0);

4. Build, flash, and run the application on a target device and wait for the dumping information.

5. Run the command below to start core dumping in GDB, where ``PORT`` is the device USB port:

.. code-block:: bash

   idf.py coredump-debug

6. In GDB shell, type ``p global_var`` to get the variable content:

.. code-block:: bash

   (gdb) p global_var
   $1 = 25 '\031'


Running ``idf.py coredump-info`` and ``idf.py coredump-debug``
--------------------------------------------------------------

``idf.py coredump-info --help`` and ``idf.py coredump-debug --help`` commands can be used to get more details on usage.


Related Documents
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 1

    core_dump_internals
