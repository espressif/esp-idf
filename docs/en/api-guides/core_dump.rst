Core Dump
=========

Overview
--------

.. only:: not esp32

    .. note::

        The python utility does not fully support {IDF_TARGET_NAME}

ESP-IDF provides support to generate core dumps on unrecoverable software errors. This useful technique allows post-mortem analysis of software state at the moment of failure.
Upon the crash system enters panic state, prints some information and halts or reboots depending configuration. User can choose to generate core dump in order to analyse
the reason of failure on PC later on. Core dump contains snapshots of all tasks in the system at the moment of failure. Snapshots include tasks control blocks (TCB) and stacks.
So it is possible to find out what task, at what instruction (line of code) and what callstack of that task lead to the crash. It is also possible dumping variables content on
demand if previously attributed accordingly.
ESP-IDF provides special script `espcoredump.py` to help users to retrieve and analyse core dumps. This tool provides two commands for core dumps analysis:

* info_corefile - prints crashed task's registers, callstack, list of available tasks in the system, memory regions and contents of memory stored in core dump (TCBs and stacks)
* dbg_corefile - creates core dump ELF file and runs GDB debug session with this file. User can examine memory, variables and tasks states manually. Note that since not all memory is saved in core dump only values of variables allocated on stack will be meaningfull

For more information about core dump internals see the - :doc:`Core dump internals <core_dump_internals>`

Configuration
-------------

There are a number of core dump related configuration options which user can choose in project configuration menu (`idf.py menuconfig`).

1. Core dump data destination (`Components -> Core dump -> Data destination`):

* Save core dump to Flash (Flash)
* Print core dump to UART (UART)
* Disable core dump generation (None)

2. Core dump data format (`Components -> Core dump -> Core dump data format`):

* ELF format (Executable and Linkable Format file for core dump)
* Binary format (Basic binary format for core dump)

The ELF format contains extended features and allow to save more information about broken tasks and crashed software but it requires more space in the flash memory.
It also stores SHA256 of crashed application image. This format of core dump is recommended for new software designs and is flexible enough to extend saved information for future revisions.
The Binary format is kept for compatibility standpoint, it uses less space in the memory to keep data and provides better performance.

3. Maximum number of tasks snapshots in core dump (`Components -> Core dump -> Maximum number of tasks`).

4. Delay before core dump is printed to UART (`Components -> Core dump -> Delay before print to UART`). Value is in ms.

5. Type of data integrity check for core dump (`Components -> Core dump -> Core dump data integrity check`).

* Use CRC32 for core dump integrity verification
* Use SHA256 for core dump integrity verification

The SHA256 hash algorithm provides greater probability of detecting corruption than a CRC32 with multiple bit errors. The CRC32 option provides better calculation performance and consumes less memory for storage.

Save core dump to flash
-----------------------

When this option is selected core dumps are saved to special partition on flash. When using default partition table files which are provided with ESP-IDF it automatically
allocates necessary space on flash, But if user wants to use its own layout file together with core dump feature it should define separate partition for core dump
as it is shown below::

  # Name,   Type, SubType, Offset,  Size
  # Note: if you have increased the bootloader size, make sure to update the offsets to avoid overlap
  nvs,      data, nvs,     0x9000,  0x6000
  phy_init, data, phy,     0xf000,  0x1000
  factory,  app,  factory, 0x10000, 1M
  coredump, data, coredump,,        64K

There are no special requrements for partition name. It can be choosen according to the user application needs, but partition type should be 'data' and
sub-type should be 'coredump'. Also when choosing partition size note that core dump data structure introduces constant overhead of 20 bytes and per-task overhead of 12 bytes.
This overhead does not include size of TCB and stack for every task. So partirion size should be at least 20 + max tasks number x (12 + TCB size + max task stack size) bytes.

The example of generic command to analyze core dump from flash is: `espcoredump.py -p </path/to/serial/port> info_corefile </path/to/program/elf/file>`
or `espcoredump.py -p </path/to/serial/port> dbg_corefile </path/to/program/elf/file>`

Print core dump to UART
-----------------------

When this option is selected base64-encoded core dumps are printed on UART upon system panic. In this case user should save core dump text body to some file manually and
then run the following command: `espcoredump.py info_corefile -t b64 -c </path/to/saved/base64/text> </path/to/program/elf/file>`
or `espcoredump.py dbg_corefile -t b64 -c </path/to/saved/base64/text> </path/to/program/elf/file>`

Base64-encoded body of core dump will be between the following header and footer::

 ================= CORE DUMP START =================
 <body of base64-encoded core dump, save it to file on disk>
 ================= CORE DUMP END ===================

The `CORE DUMP START` and `CORE DUMP END` lines must not be included in core dump text file.

ROM Functions in Backtraces
---------------------------

It is possible situation that at the moment of crash some tasks or/and crashed task itself have one or more ROM functions in their callstacks.
Since ROM is not part of the program ELF it will be impossible for GDB to parse such callstacks, because it tries to analyse functions' prologues to acomplish that.
In that case callstack printing will be broken with error message at the first ROM function.
To overcome this issue you can use ROM ELF provided by Espressif (https://dl.espressif.com/dl/{IDF_TARGET_PATH_NAME}_rom.elf) and pass it to 'espcoredump.py'.

Dumping variables on demand
---------------------------

Sometimes you want to read the last value of a variable to understand the root cause of a crash.
Core dump supports retrieving variable data over GDB by attributing special notations declared variables.

Supported notations and RAM regions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: esp32

  - ``COREDUMP_DRAM_ATTR`` places variable into DRAM area which will be included into dump.
  - ``COREDUMP_RTC_ATTR`` places variable into RTC area which will be included into dump.
  - ``COREDUMP_RTC_FAST_ATTR`` places variable into RTC_FAST area which will be included into dump.
  - ``COREDUMP_IRAM_ATTR`` places variable into IRAM area which will be included into dump when :ref:`Enable IRAM as 8 bit accessible memory <CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY>` is set.

.. only:: esp32s2

  - ``COREDUMP_DRAM_ATTR`` places variable into DRAM area which will be included into dump.
  - ``COREDUMP_RTC_ATTR`` places variable into RTC area which will be included into dump.
  - ``COREDUMP_RTC_FAST_ATTR`` places variable into RTC_FAST area which will be included into dump.

Example
^^^^^^^

1. In :ref:`project-configuration-menu`, enable :ref:`COREDUMP TO FLASH <CONFIG_ESP_COREDUMP_TO_FLASH_OR_UART>`, then save and exit.

2. In your project, create a global variable in DRAM area as such as:

  .. code-block:: bash

      // uint8_t global_var;
      COREDUMP_DRAM_ATTR uint8_t global_var;

3. In main application, set the variable to any value and `assert(0)` to cause a crash.

  .. code-block:: bash

      global_var = 25;
      assert(0);

4. Build, flash and run the application on a target device and wait for the dumping information.

5. Run the command below to start core dumping in GDB, where ``PORT`` is the device USB port:

  .. code-block:: bash

      espcoredump.py -p PORT dbg_corefile <path/to/elf>

6. In GDB shell, type ``p global_var`` to get the variable content:

  .. code-block:: bash

      (gdb) p global_var
      $1 = 25 '\031'

Running 'espcoredump.py'
------------------------

Generic command syntax:

`espcoredump.py [options] command [args]`

:Script Options:
    * --port,-p PORT. Serial port device.
    * --baud,-b BAUD. Serial port baud rate used when flashing/reading.
:Commands:
    * info_corefile. Retrieve core dump and print useful info.
    * dbg_corefile. Retrieve core dump and start GDB session with it.
:Command Arguments:
    * --debug,-d DEBUG.             Log level (0..3).
    * --gdb,-g GDB.                 Path to gdb to use for data retrieval.
    * --core,-c CORE.               Path to core dump file to use (if skipped core dump will be read from flash).
    * --core-format,-t CORE_FORMAT. Specifies that file passed with "-c" is an ELF ("elf"), dumped raw binary ("raw") or base64-encoded ("b64") format.
    * --off,-o OFF.                 Offset of coredump partition in flash (type `idf.py partition_table` to see it).
    * --save-core,-s SAVE_CORE.     Save core to file. Othwerwise temporary core file will be deleted. Ignored with "-c".
    * --rom-elf,-r ROM_ELF.         Path to ROM ELF file to use (if skipped "esp32_rom.elf" is used).
    * --print-mem,-m                Print memory dump. Used only with "info_corefile".
    * <prog>                        Path to program ELF file.
