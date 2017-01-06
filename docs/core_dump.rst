ESP32 Core Dump
================

Overview
--------

ESP-IDF provides support to generate core dumps on unrecoverable software errors. This useful technique allows post-mortem analisys of software state at the moment of failure.
Upon the crash system enters panic state, prints some information and halts or reboots depending configuration. User can choose to generate core dump in order to analyse
the reason of failure on PC later on. Core dump contains snapshots of all tasks in the system at the moment of failure. Snapshots include tasks control blocks (TCB) and stacks.
So it is possible to find out what task, at what instruction (line of code) and what callstack of that task lead to the crash. 
ESP-IDF provides special script `espcoredump.py` to help users to retrieve and analyse core dumps. This tool provides two commands for core dumps analysis:

* info_corefile - prints crashed task's registers, callstack, list of available tasks in the system, memory regions and contents of memory stored in core dump (TCBs and stacks)
* dbg_corefile - creates core dump ELF file and runs GDB debug session with this file. User can examine memory, variables and tasks states manually. Note that since not all memory is saved in core dump only values of variables allocated on stack will be meaningfull

Configuration
-------------

Currently there are three options related to core dump generation which user can choose in configuration menu of the application (`make menuconfig`):

* Disable core dump generation
* Save core dump to flash
* Print core dump to UART

These options can be choosen in Components -> ESP32-specific config -> Core dump destination menu item.

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
  coredump, data, 3,       ,        64K
  
There are no special requrements for partition name. It can be choosen according to the user application needs, but partition type should be 'data' and 
sub-type should be 3. Also when choosing partition size note that core dump data structure introduces constant overhead of 20 bytes and per-task overhead of 12 bytes. 
This overhead does not include size of TCB and stack for every task. So partirion size should be at least 20 + max tasks number x (12 + TCB size + max task stack size) bytes.

The example of generic command to analyze core dump from flash is: `espcoredump.py -p </path/to/serial/port> info_corefile </path/to/program/elf/file>`
or `espcoredump.py -p </path/to/serial/port> dbg_corefile </path/to/program/elf/file>`

Print core dump to UART
-----------------------

When this option is selected base64-encoded core dumps are printed on UART upon system panic. In this case user should save core dump text body to some file manually and 
then run the following command: `espcoredump.py -p </path/to/serial/port> info_corefile -t b64 -c </path/to/saved/base64/text> </path/to/program/elf/file>`
or `espcoredump.py -p </path/to/serial/port> dbg_corefile -t b64 -c </path/to/saved/base64/text> </path/to/program/elf/file>`

Base64-encoded body of core dump will be between the following header and footer::

 ================= CORE DUMP START =================
 <body of base64-encoded core dump, copy it to file on disk>
 ================= CORE DUMP END ===================

Command Options For 'espcoredump.py'
--------------------------------------------

usage: coredumper [-h] [--chip {auto,esp32}] [--port PORT] [--baud BAUD]
                  {dbg_corefile,info_corefile} ...

espcoredump.py v0.1-dev - ESP32 Core Dump Utility

positional arguments:
  {dbg_corefile,info_corefile}
                        Run coredumper {command} -h for additional help
    dbg_corefile        Starts GDB debugging session with specified corefile
    info_corefile       Print core dump info from file

optional arguments:
  -h, --help            show this help message and exit
  --chip {auto,esp32}, -c {auto,esp32}
                        Target chip type
  --port PORT, -p PORT  Serial port device
  --baud BAUD, -b BAUD  Serial port baud rate used when flashing/reading


usage: coredumper info_corefile [-h] [--gdb GDB] [--core CORE]
                                [--core-format CORE_FORMAT] [--off OFF]
                                [--save-core SAVE_CORE] [--print-mem]
                                prog

positional arguments:
  prog                  Path to program's ELF binary

optional arguments:
  -h, --help            show this help message and exit
  --gdb GDB, -g GDB     Path to gdb
  --core CORE, -c CORE  Path to core dump file (if skipped core dump will be
                        read from flash)
  --core-format CORE_FORMAT, -t CORE_FORMAT
                        (elf, raw or b64). File specified with "-c" is an ELF
                        ("elf"), raw (raw) or base64-encoded (b64) binary
  --off OFF, -o OFF     Ofsset of coredump partition in flash (type "make
                        partition_table" to see).
  --save-core SAVE_CORE, -s SAVE_CORE
                        Save core to file. Othwerwise temporary core file will
                        be deleted. Does not work with "-c"
  --print-mem, -m       Print memory dump


usage: coredumper dbg_corefile [-h] [--gdb GDB] [--core CORE]
                               [--core-format CORE_FORMAT] [--off OFF]
                               [--save-core SAVE_CORE]
                               prog

positional arguments:
  prog                  Path to program's ELF binary

optional arguments:
  -h, --help            show this help message and exit
  --gdb GDB, -g GDB     Path to gdb
  --core CORE, -c CORE  Path to core dump file (if skipped core dump will be
                        read from flash)
  --core-format CORE_FORMAT, -t CORE_FORMAT
                        (elf, raw or b64). File specified with "-c" is an ELF
                        ("elf"), raw (raw) or base64-encoded (b64) binary
  --off OFF, -o OFF     Ofsset of coredump partition in flash (type "make
                        partition_table" to see).
  --save-core SAVE_CORE, -s SAVE_CORE
                        Save core to file. Othwerwise temporary core file will
                        be deleted. Ignored with "-c"
