Semihosting Feature
-------------------

Semihosting is a mechanism that lets a program running on the target use I/O facilities on the host machine where the debugger runs. It is useful for debugging and testing embedded applications without having to implement hardware-specific I/O on the target side.

OpenOCD implements an extended semihosting protocol for Espressif targets that goes beyond the standard ARM Semihosting specification. This allows embedded applications to interact with the host system for file operations, directory management, and other system calls.

.. warning::

    Each semihosting call is implemented as a sequence containing a software breakpoint instruction. If a build that contains semihosting calls runs **without** a debugger attached, an exception will be triggered instead.

.. note::

    Each semihosting call halts the CPU until the host returns a result, so semihosting is not suitable for latency-sensitive or real-time code paths.


.. _jtag-debugging-semihosting-available-operations:

Available Operations
^^^^^^^^^^^^^^^^^^^^

Header :idf_file:`components/vfs/openocd_semihosting.h` declares the full set of available semihosting operations. Common ones include:

* **File Operations**: ``open``, ``close``, ``read``, ``write``, ``lseek``, ``fsync``, ``link``, ``unlink``
* **Directory Operations**: ``opendir``, ``readdir``, ``seekdir``, ``telldir``, ``closedir``, ``mkdir``, ``rmdir``
* **File Attribute Operations**: ``rename``, ``truncate``, ``fstat``, ``stat``, ``utime``, ``access``

In addition, the target can use debugging hooks to trigger events that OpenOCD processes directly:

* ``panic_reason``: notify user about detailed panic information directly in the debugger console.

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

   * ``breakpoint_set``, ``watchpoint_set``: allow configuring breakpoints and watchpoints from the target side, without user interaction.


.. _jtag-debugging-semihosting-using-from-app:

Using Semihosting From an Application
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The most convenient way to use semihosting from application code is through the Virtual File System (VFS) driver. Calling :cpp:func:`esp_vfs_semihost_register` mounts a host directory as a regular VFS path, so that ``fopen``, ``read``, ``write``, and similar standard calls work transparently:

.. code-block:: c

    #include "esp_vfs_semihost.h"

    esp_vfs_semihost_register("/host");
    FILE *f = fopen("/host/log.txt", "w");

See the :doc:`Virtual File System Component API Reference <../../api-reference/storage/vfs>` and the :example:`storage/semihost_vfs` example for the full flow.

See also `OpenOCD semihosting test application <https://github.com/espressif/openocd-esp32/blob/master/testing/esp/test_apps/gen_ut_app/main/semihost_tests.c>`_.

.. _jtag-debugging-semihosting-configuration:

Configuration
^^^^^^^^^^^^^

By default, semihosting file operations use the current directory (where OpenOCD is started) as the base directory. To specify a different base directory, add an extra argument ``-c 'set ESP_SEMIHOST_BASEDIR /path/to/semihost/root'`` to the start of the OpenOCD command line. See :ref:`jtag-debugging-tip-openocd-config-vars`.

.. _jtag-debugging-semihosting-gdb-semihosting:

GDB Semihosting
^^^^^^^^^^^^^^^

GDB also provides built-in support for semihosting, which complements OpenOCD's implementation. This is especially useful when GDB is connected to OpenOCD remotely and runs on a different machine — semihosting file operations then resolve against the GDB host rather than the OpenOCD host.

To redirect semihosting requests to GDB, enter ``mon arm semihosting_fileio enable`` in GDB. For multi-core targets, this only enables semihosting for the current core; use ``mon <target name> arm semihosting_fileio enable`` per core if needed (targets can be listed with ``mon targets``).

With file I/O enabled, OpenOCD does not process the system operation itself after intercepting a semihosting call. Instead, it sends a file I/O request packet to GDB and keeps the target halted until GDB responds with the result. This process is fully transparent to the code running on the target.
