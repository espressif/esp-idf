ESP32 Core Dump
===============

Overview
--------

ESP-IDF provides support to generate core dumps on unrecoverable software errors. This useful technique allows post-mortem analysis of software state at the moment of failure.
Upon the crash system enters panic state, prints some information and halts or reboots depending configuration. User can choose to generate core dump in order to analyse
the reason of failure on PC later on. Core dump contains snapshots of all tasks in the system at the moment of failure. Snapshots include tasks control blocks (TCB) and stacks.
So it is possible to find out what task, at what instruction (line of code) and what callstack of that task lead to the crash. 
ESP-IDF provides special script `espcoredump.py` to help users to retrieve and analyse core dumps. This tool provides two commands for core dumps analysis:

* info_corefile - prints crashed task's registers, callstack, list of available tasks in the system, memory regions and contents of memory stored in core dump (TCBs and stacks)
* dbg_corefile - creates core dump ELF file and runs GDB debug session with this file. User can examine memory, variables and tasks states manually. Note that since not all memory is saved in core dump only values of variables allocated on stack will be meaningfull

Configuration
-------------

There are a number of core dump related configuration options which user can choose in configuration menu of the application (`make menuconfig`).

1. Core dump data destination (`Components -> ESP32-specific config -> Core dump destination`):

* Disable core dump generation
* Save core dump to flash
* Print core dump to UART

2. Logging level of core dump module (`Components -> ESP32-specific config -> Core dump module logging level`). Value is a number from 0 (no output) to 5 (most verbose).

3. Delay before core dump will be printed to UART (`Components -> ESP32-specific config -> Core dump print to UART delay`). Value is in ms.


Save core dump to flash
-----------------------

When this option is selected core dumps are saved to special partition on flash. When using default partition table files which are provided with ESP-IDF it automatically 
allocates necessary space on flash, But if user wants to use its own layout file together with core dump feature it should define separate partition for core dump 
as it is shown below::

  # Name,   Type, SubType, Offset,  Size
  # Note: if you change the phy_init or app partition offset, make sure to change the offset in Kconfig.projbuild
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

Running 'espcoredump.py'
------------------------------------

Generic command syntax:

`espcoredump.py [options] command [args]`

:Script Options:
    * --chip,-c {auto,esp32}. Target chip type. Supported values are `auto` and `esp32`.
    * --port,-p PORT. Serial port device.
    * --baud,-b BAUD. Serial port baud rate used when flashing/reading.
:Commands:
    * info_corefile. Retrieve core dump and print useful info.
    * dbg_corefile. Retrieve core dump and start GDB session with it.
:Command Arguments:
    * --gdb,-g GDB.                 Path to gdb to use for data retrieval.
    * --core,-c CORE.               Path to core dump file to use (if skipped core dump will be read from flash).
    * --core-format,-t CORE_FORMAT. Specifies that file passed with "-c" is an ELF ("elf"), dumped raw binary ("raw") or base64-encoded ("b64") format.
    * --off,-o OFF.                 Ofsset of coredump partition in flash (type "make partition_table" to see it).
    * --save-core,-s SAVE_CORE.     Save core to file. Othwerwise temporary core file will be deleted. Ignored with "-c".
    * --print-mem,-m                Print memory dump. Used only with "info_corefile".
