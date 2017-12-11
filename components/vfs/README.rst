Virtual filesystem component
============================

Overview
--------

Virtual filesystem (VFS) component provides a unified interface for drivers which can perform operations on file-like objects. This can be a real filesystems (FAT, SPIFFS, etc.), or device drivers which exposes file-like interface.

This component allows C library functions, such as fopen and fprintf, to work with FS drivers. At high level, each FS driver is associated with some path prefix. When one of C library functions needs to open a file, VFS component searches for the FS driver associated with the file's path, and forwards the call to that driver. VFS also forwards read, write, and other calls for the given file to the same FS driver.

For example, one can register a FAT filesystem driver with ``/fat`` prefix, and call ``fopen("/fat/file.txt", "w")``. VFS component will then call ``open`` function of FAT driver and pass ``/file.txt`` argument to it (and appropriate mode flags). All subsequent calls to C library functions for the returned ``FILE*`` stream will also be forwarded to the FAT driver.

FS registration
---------------



To register an FS driver, application needs to define in instance of esp_vfs_t structure and populate it with function pointers to FS APIs:

.. highlight:: c

::

    esp_vfs_t myfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &myfs_write,
        .open = &myfs_open,
        .fstat = &myfs_fstat,
        .close = &myfs_close,
        .read = &myfs_read,
    };

    ESP_ERROR_CHECK(esp_vfs_register("/data", &myfs, NULL));

Depending on the way FS driver declares its APIs, either ``read``, ``write``, etc., or ``read_p``, ``write_p``, etc. should be used.

Case 1: API functions are declared without an extra context pointer (FS driver is a singleton)::

    ssize_t myfs_write(int fd, const void * data, size_t size);

    // In definition of esp_vfs_t:
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &myfs_write,
    // ... other members initialized

    // When registering FS, context pointer (third argument) is NULL:
    ESP_ERROR_CHECK(esp_vfs_register("/data", &myfs, NULL));

Case 2: API functions are declared with an extra context pointer (FS driver supports multiple instances)::

    ssize_t myfs_write(myfs_t* fs, int fd, const void * data, size_t size);

    // In definition of esp_vfs_t:
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .write_p = &myfs_write,
    // ... other members initialized

    // When registering FS, pass the FS context pointer into the third argument
    // (hypothetical myfs_mount function is used for illustrative purposes)
    myfs_t* myfs_inst1 = myfs_mount(partition1->offset, partition1->size);
    ESP_ERROR_CHECK(esp_vfs_register("/data1", &myfs, myfs_inst1));

    // Can register another instance:
    myfs_t* myfs_inst2 = myfs_mount(partition2->offset, partition2->size);
    ESP_ERROR_CHECK(esp_vfs_register("/data2", &myfs, myfs_inst2));

Paths
-----

Each registered FS has a path prefix associated with it. This prefix may be considered a "mount point" of this partition.

In case when mount points are nested, the mount point with the longest matching path prefix is used when opening the file. For instance, suppose that the following filesystems are registered in VFS:

- FS 1 on /data
- FS 2 on /data/static

Then:

- FS 1 will be used when opening a file called ``/data/log.txt``
- FS 2 will be used when opening a file called ``/data/static/index.html``
- Even if ``/index.html"`` doesn't exist in FS 2, FS 1 will *not* be searched for ``/static/index.html``.

As a general rule, mount point names must start with the path separator (``/``) and must contain at least one character after path separator. However an empty mount point name is also supported, and may be used in cases when application needs to provide "fallback" filesystem, or override VFS functionality altogether. Such filesystem will be used if no prefix matches the path given.

VFS does not handle dots (``.``) in path names in any special way. VFS does not treat ``..`` as a reference to the parent directory. I.e. in the above example, using a path ``/data/static/../log.txt`` will not result in a call to FS 1 to open ``/log.txt``. Specific FS drivers (such as FATFS) may handle dots in file names differently.

When opening files, FS driver will only be given relative path to files. For example:

- ``myfs`` driver is registered with ``/data`` as path prefix
- and application calls ``fopen("/data/config.json", ...)``
- then VFS component will call ``myfs_open("/config.json", ...)``.
- ``myfs`` driver will open ``/config.json`` file

VFS doesn't impose a limit on total file path length, but it does limit FS path prefix to ``ESP_VFS_PATH_MAX`` characters. Individual FS drivers may have their own filename length limitations.


File descriptors
----------------

It is suggested that filesystem drivers should use small positive integers as file descriptors. VFS component assumes that ``CONFIG_MAX_FD_BITS`` bits (12 by default) are sufficient to represent a file descriptor.

While file descriptors returned by VFS component to newlib library are rarely seen by the application, the following details may be useful for debugging purposes. File descriptors returned by VFS component are composed of two parts: FS driver ID, and the actual file descriptor. Because newlib stores file descriptors as 16-bit integers, VFS component is also limited by 16 bits to store both parts.

Lower ``CONFIG_MAX_FD_BITS`` bits are used to store zero-based file descriptor. The per-filesystem FD obtained from the FS ``open`` call, and this result is stored in the lower bits of the FD. Higher bits are used to save the index of FS in the internal table of registered filesystems.

When VFS component receives a call from newlib which has a file descriptor, this file descriptor is translated back to the FS-specific file descriptor. First, higher bits of FD are used to identify the FS. Then only the lower ``CONFIG_MAX_FD_BITS`` bits of the fd are masked in, and resulting FD is passed to the FS driver.

.. highlight:: none

::

       FD as seen by newlib                                    FD as seen by FS driver

    +-------+---------------+                               +------------------------+
    | FS id | Zero—based FD |     +-------------------------->                        |
    +---+---+------+--------+     |                          +------------------------+
        |          |              |
        |          +--------------+
        |
        |       +-------------+
        |       | Table of    |
        |       | registered  |
        |       | filesystems |
        |       +-------------+    +-------------+
        +------->  entry      +----> esp_vfs_t   |
        index   +-------------+    | structure   |
                |             |    |             |
                |             |    |             |
                +-------------+    +-------------+


Standard IO streams (stdin, stdout, stderr)
-------------------------------------------

If "UART for console output" menuconfig option is not set to "None", then ``stdin``, ``stdout``, and ``stderr`` are configured to read from, and write to, a UART. It is possible to use UART0 or UART1 for standard IO. By default, UART0 is used, with 115200 baud rate, TX pin is GPIO1 and RX pin is GPIO3. These parameters can be changed in menuconfig.

Writing to ``stdout`` or ``stderr`` will send characters to the UART transmit FIFO. Reading from ``stdin`` will retrieve characters from the UART receive FIFO.

By default, VFS uses simple functions for reading from and writing to UART. Writes busy-wait until all data is put into UART FIFO, and reads are non-blocking, returning only the data present in the FIFO. Because of this non-blocking read behavior, higher level C library calls, such as ``fscanf("%d\n", &var);`` may not have desired results.

Applications which use UART driver may instruct VFS to use the driver's interrupt driven, blocking read and write functions instead. This can be done using a call to ``esp_vfs_dev_uart_use_driver`` function. It is also possible to revert to the basic non-blocking functions using a call to ``esp_vfs_dev_uart_use_nonblocking``.

VFS also provides optional newline conversion feature for input and output. Internally, most applications send and receive lines terminated by LF (''\n'') character. Different terminal programs may require different line termination, such as CR or CRLF. Applications can configure this separately for input and output either via menuconfig, or by calls to ``esp_vfs_dev_uart_set_rx_line_endings`` and ``esp_vfs_dev_uart_set_tx_line_endings`` functions.



Standard streams and FreeRTOS tasks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``FILE`` objects for ``stdin``, ``stdout``, and ``stderr`` are shared between all FreeRTOS tasks, but the pointers to these objects are are stored in per-task ``struct _reent``. The following code:

.. highlight:: c

::

    fprintf(stderr, "42\n");

actually is translated to to this (by the preprocessor)::

    fprintf(__getreent()->_stderr, "42\n");

where the ``__getreent()`` function returns a per-task pointer to ``struct _reent`` (:component_file:`newlib/include/sys/reent.h#L370-L417`). This structure is allocated on the TCB of each task. When a task is initialized, ``_stdin``, ``_stdout`` and ``_stderr`` members of ``struct _reent`` are set to the values of ``_stdin``, ``_stdout`` and ``_stderr`` of ``_GLOBAL_REENT`` (i.e. the structure which is used before FreeRTOS is started).

Such a design has the following consequences:

- It is possible to set ``stdin``, ``stdout``, and ``stderr`` for any given task without affecting other tasks, e.g. by doing ``stdin = fopen("/dev/uart/1", "r")``.
- Closing default ``stdin``, ``stdout``, or ``stderr`` using ``fclose`` will close the ``FILE`` stream object — this will affect all other tasks.
- To change the default ``stdin``, ``stdout``, ``stderr`` streams for new tasks, modify ``_GLOBAL_REENT->_stdin`` (``_stdout``, ``_stderr``) before creating the task.
