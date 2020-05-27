:orphan:

Anatomy of core dump image
--------------------------

Core dump component can be configured to use old legacy binary format or the new ELF one. The ELF format is recommended for new designs. It provides more information about the CPU and memory state of a program at the moment when panic handler is entered. 
The memory state embeds a snapshot of all tasks mapped in the memory space of the program. The CPU state contains register values when the core dump has been generated. Core dump file uses a subset of the ELF structures to register these information.
Loadable ELF segments are used for the memory state of the process while ELF notes (ELF.PT_NOTE) are used for process metadata (pid, registers, signal, ...). Especially, the CPU status is stored in a note with a special name and type (``CORE``, ``NT_PRSTATUS type``).

Here is an overview of coredump layout:

.. figure:: ../../_static/core_dump_format_elf.png
    :align: center
    :alt: Core dump image format
    :figclass: align-center

    Core dump ELF image format

.. figure:: ../../_static/core_dump_format_bin.png
    :align: center
    :alt: Core dump binary image format
    :figclass: align-center

    Core dump binary image format
    
Note: The format of image file showed on the above pictures represents current version of image and can be changed in future releases.

Overview of implementation
--------------------------

The figure below describes some basic aspects related to implementation of core dump:

.. figure:: ../../_static/core_dump_impl.png
    :align: center
    :alt: Core dump implementation overview
    :figclass: align-center

    Core dump implementation overview
    
Note: The diagram above hide some details and represents current implementation of the core dump and can be changed later. 