Core Dump
=========

Overview
--------

ESP-IDF provides support to generate core dumps on unrecoverable software errors. This useful technique allows post-mortem analysis of software state at the moment of failure.
Upon the crash system enters panic state, prints some information and halts or reboots depending configuration. User can choose to generate core dump in order to analyse
the reason of failure on PC later on. Core dump contains snapshots of all tasks in the system at the moment of failure. Snapshots include tasks control blocks (TCB) and stacks.
So it is possible to find out what task, at what instruction (line of code) and what callstack of that task lead to the crash. It is also possible dumping variables content on
demand if previously attributed accordingly.
ESP-IDF provides special commands to help users to retrieve and analyse core dumps:

* ``idf.py coredump-info`` - prints crashed task's registers, callstack, list of available tasks in the system, memory regions and contents of memory stored in core dump (TCBs and stacks)
* ``idf.py coredump-debug`` - creates core dump ELF file and runs GDB debug session with this file. User can examine memory, variables and tasks states manually. Note that since not all memory is saved in core dump only values of variables allocated on stack will be meaningful

For more information about core dump internals see the - :doc:`Core dump internals <core_dump_internals>`

Configurations
--------------

There are a number of core dump related configuration options which user can choose in project configuration menu (``idf.py menuconfig``).

**Core dump data destination (Components -> Core dump -> Data destination)**

   * Save core dump to Flash (Flash)
   * Print core dump to UART (UART)
   * Disable core dump generation (None)

**Core dump data format (Components -> Core dump -> Core dump data format)**

   * ELF format (Executable and Linkable Format file for core dump)
   * Binary format (Basic binary format for core dump)

   The ELF format contains extended features and allow to save more information about broken tasks and crashed software but it requires more space in the flash memory.
   This format of core dump is recommended for new software designs and is flexible enough to extend saved information for future revisions.

   The Binary format is kept for compatibility reasons, it uses less space in the memory to keep data and provides better performance.

**Core dump data integrity check (Components -> Core dump -> Core dump data integrity check)**

   .. only:: esp32

      * Use CRC32 for core dump integrity verification
      * Use SHA256 for core dump integrity verification (only work in ELF format)

      The CRC32 option provides better calculation performance and consumes less memory for storage.

      The SHA256 hash algorithm provides greater probability of detecting corruption than a CRC32 with multiple bit errors.

   .. only:: not esp32

      * Use CRC32 for core dump integrity verification

**Maximum number of tasks snapshots in core dump (Components -> Core dump -> Maximum number of tasks)**

**Delay before core dump is printed to UART (Components -> Core dump -> Delay before print to UART)**

   The value is in ms.

**Handling of UART core dumps in IDF Monitor (Components -> Core dump -> Delay before print to UART)**

   The value is base64 encoded.

   * Decode and show summary (info_corefile)
   * Don't decode

**Reserved stack size (Components -> Core dump -> Reserved stack size)**

   Size of the memory to be reserved for core dump stack. If 0 core dump process will run on the stack of crashed task/ISR, otherwise special stack will be allocated.
   To ensure that core dump itself will not overflow task/ISR stack set this to the value above 800.

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

.. important::
    If :doc:`Flash Encryption <../security/flash-encryption>` is enabled on the device then please add `encrypted` flag to the coredump partition::

        coredump, data, coredump,,       64K, encrypted

There are no special requirements for partition name. It can be chosen according to the user application needs, but partition type should be 'data' and
sub-type should be 'coredump'. Also when choosing partition size note that core dump data structure introduces constant overhead of 20 bytes and per-task overhead of 12 bytes.
This overhead does not include size of TCB and stack for every task. So partition size should be at least 20 + max tasks number x (12 + TCB size + max task stack size) bytes.

The example of generic command to analyze core dump from flash is:

.. code-block:: bash

    idf.py coredump-info

or

.. code-block:: bash

    idf.py coredump-debug

Print core dump to UART
-----------------------

When this option is selected base64-encoded core dumps are printed on UART upon system panic. In this case user should save core dump text body to some file manually and
then run the following command:

.. code-block:: bash

    idf.py coredump-info -c </path/to/saved/base64/text>

or

.. code-block:: bash

    idf.py coredump-debug -c </path/to/saved/base64/text>

Base64-encoded body of core dump will be between the following header and footer::

   ================= CORE DUMP START =================
   <body of base64-encoded core dump, save it to file on disk>
   ================= CORE DUMP END ===================

The ``CORE DUMP START`` and ``CORE DUMP END`` lines must not be included in core dump text file.

ROM Functions in Backtraces
---------------------------

It is possible situation that at the moment of crash some tasks or/and crashed task itself have one or more ROM functions in their callstacks.
Since ROM is not part of the program ELF it will be impossible for GDB to parse such callstacks, because it tries to analyse functions' prologues to accomplish that.
In that case callstack printing will be broken with error message at the first ROM function.
To overcome this issue, `ROM ELF <https://github.com/espressif/esp-rom-elfs/releases>`_ provided by Espressif is loaded automatically based on the target and its revision. More details about ROM ELFs can be found `here <https://github.com/espressif/esp-rom-elfs/blob/master/README.md>`_.

Dumping variables on demand
---------------------------

Sometimes you want to read the last value of a variable to understand the root cause of a crash.
Core dump supports retrieving variable data over GDB by attributing special notations declared variables.

Supported notations and RAM regions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list::

   * ``COREDUMP_DRAM_ATTR`` places variable into DRAM area which will be included into dump.
   :SOC_RTC_FAST_MEM_SUPPORTED or SOC_RTC_SLOW_MEM_SUPPORTED: * ``COREDUMP_RTC_ATTR`` places variable into RTC area which will be included into dump.
   :SOC_RTC_FAST_MEM_SUPPORTED: * ``COREDUMP_RTC_FAST_ATTR`` places variable into RTC_FAST area which will be included into dump.

Example
^^^^^^^

1. In :ref:`project-configuration-menu`, enable :ref:`COREDUMP TO FLASH <CONFIG_ESP_COREDUMP_TO_FLASH_OR_UART>`, then save and exit.

2. In your project, create a global variable in DRAM area as such as:

.. code-block:: bash

   // uint8_t global_var;
   COREDUMP_DRAM_ATTR uint8_t global_var;

3. In main application, set the variable to any value and ``assert(0)`` to cause a crash.

.. code-block:: bash

   global_var = 25;
   assert(0);

4. Build, flash and run the application on a target device and wait for the dumping information.

5. Run the command below to start core dumping in GDB, where ``PORT`` is the device USB port:

.. code-block:: bash

   idf.py coredump-debug

6. In GDB shell, type ``p global_var`` to get the variable content:

.. code-block:: bash

   (gdb) p global_var
   $1 = 25 '\031'

Running ``idf.py coredump-info`` and ``idf.py coredump-debug``
--------------------------------------------------------------

``idf.py coredump-info --help`` and ``idf.py coredump-debug --help`` commands can be used to get more details on usage

Related Documents
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 1

    core_dump_internals
