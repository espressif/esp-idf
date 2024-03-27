Anatomy of Core Dump Image
--------------------------

:link_to_translation:`zh_CN:[中文]`

A core dump file's format can be configured to use the ELF format, or a legacy binary format. The ELF format is recommended for all new designs as it provides more information regarding the software's state at the moment the crash occurs, e.g., CPU registers and memory contents.

The memory state embeds a snapshot of all tasks mapped in the memory space of the program. The CPU state contains register values when the core dump has been generated. The core dump file uses a subset of the ELF structures to register this information.

Loadable ELF segments are used to store the process' memory state, while ELF notes (``ELF.PT_NOTE``) are used to store the process' metadata (e.g., PID, registers, signal etc). In particular, the CPU's status is stored in a note with a special name and type (``CORE``, ``NT_PRSTATUS type``).

Here is an overview of the core dump layout:

.. figure:: ../../_static/core_dump_format_elf.png
    :align: center
    :alt: Core Dump ELF Image Format
    :figclass: align-center

    Core Dump ELF Image Format

.. figure:: ../../_static/core_dump_format_bin.png
    :align: center
    :alt: Core Dump Binary Image Format
    :figclass: align-center

    Core Dump Binary Image Format

.. note::

    The format of the image file shown in the above pictures represents the current version of the image and can be changed in future releases.


Overview of Implementation
--------------------------

The figure below describes some basic aspects related to the implementation of the core dump:

.. figure:: ../../_static/core_dump_impl.png
    :align: center
    :alt: Core Dump Implementation Overview
    :figclass: align-center

    Core Dump Implementation Overview

.. note::

    The diagram above hides some details and represents the current implementation of the core dump which can be changed later.
