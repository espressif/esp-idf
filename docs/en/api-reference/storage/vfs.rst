Virtual Filesystem Component
============================

:link_to_translation:`zh_CN:[中文]`

Overview
--------

Virtual filesystem (VFS) component provides a unified interface for drivers which can perform operations on file-like objects. These can be real filesystems (FAT, SPIFFS, etc.) or device drivers which provide a file-like interface.

This component allows C library functions, such as fopen and fprintf, to work with FS drivers. At a high level, each FS driver is associated with some path prefix. When one of C library functions needs to open a file, the VFS component searches for the FS driver associated with the file path and forwards the call to that driver. VFS also forwards read, write, and other calls for the given file to the same FS driver.

For example, one can register a FAT filesystem driver with the ``/fat`` prefix and call ``fopen("/fat/file.txt", "w")``. Then the VFS component calls the function ``open`` of the FAT driver and pass the argument ``/file.txt`` to it together with appropriate mode flags. All subsequent calls to C library functions for the returned ``FILE*`` stream will also be forwarded to the FAT driver.


FS Registration
---------------

To register an FS driver, an application needs to define an instance of the :cpp:type:`esp_vfs_t` structure and populate it with function pointers to FS APIs:

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

Depending on the way how the FS driver declares its API functions, either ``read``, ``write``, etc., or ``read_p``, ``write_p``, etc., should be used.

Case 1: API functions are declared without an extra context pointer (the FS driver is a singleton)::

    ssize_t myfs_write(int fd, const void * data, size_t size);

    // In definition of esp_vfs_t:
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &myfs_write,
    // ... other members initialized

    // When registering FS, context pointer (third argument) is NULL:
    ESP_ERROR_CHECK(esp_vfs_register("/data", &myfs, NULL));

Case 2: API functions are declared with an extra context pointer (the FS driver supports multiple instances)::

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

Synchronous Input/Output Multiplexing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Synchronous input/output multiplexing by :cpp:func:`select` is supported in the VFS component. The implementation works in the following way.

1. :cpp:func:`select` is called with file descriptors which could belong to various VFS drivers.

2. The file descriptors are divided into groups each belonging to one VFS driver.

3. The file descriptors belonging to non-socket VFS drivers are handed over to the given VFS drivers by :cpp:func:`start_select`, described later on this page. This function represents the driver-specific implementation of :cpp:func:`select` for the given driver. This should be a non-blocking call which means the function should immediately return after setting up the environment for checking events related to the given file descriptors.

4. The file descriptors belonging to the socket VFS driver are handed over to the socket driver by :cpp:func:`socket_select` described later on this page. This is a blocking call which means that it will return only if there is an event related to socket file descriptors or a non-socket driver signals :cpp:func:`socket_select` to exit.

5. Results are collected from each VFS driver and all drivers are stopped by de-initialization of the environment for checking events.

6. The :cpp:func:`select` call ends and returns the appropriate results.

Non-Socket VFS Drivers
""""""""""""""""""""""

If you want to use :cpp:func:`select` with a file descriptor belonging to a non-socket VFS driver, then you need to register the driver with functions :cpp:func:`start_select` and :cpp:func:`end_select` similarly to the following example:

.. highlight:: c

::

    // In definition of esp_vfs_t:
        .start_select = &uart_start_select,
        .end_select = &uart_end_select,
    // ... other members initialized

:cpp:func:`start_select` is called for setting up the environment for detection of read/write/error conditions on file descriptors belonging to the given VFS driver.

:cpp:func:`end_select` is called to stop/deinitialize/free the environment which was setup by :cpp:func:`start_select`.

.. note::

    :cpp:func:`end_select` might be called without a previous :cpp:func:`start_select` call in some rare circumstances. :cpp:func:`end_select` should fail gracefully if this is the case (i.e., should not crash but return an error instead).

Please refer to the reference implementation for the UART peripheral in :component_file:`esp_driver_uart/src/uart_vfs.c` and most particularly to the functions :cpp:func:`uart_vfs_dev_register`, :cpp:func:`uart_start_select`, and :cpp:func:`uart_end_select` for more information.

Please check the following examples that demonstrate the use of :cpp:func:`select` with VFS file descriptors:

- :example:`peripherals/uart/uart_select`
- :example:`system/select`


Socket VFS Drivers
""""""""""""""""""

A socket VFS driver is using its own internal implementation of :cpp:func:`select` and non-socket VFS drivers notify it upon read/write/error conditions.

A socket VFS driver needs to be registered with the following functions defined:

.. highlight:: c

::

    // In definition of esp_vfs_t:
        .socket_select = &lwip_select,
        .get_socket_select_semaphore = &lwip_get_socket_select_semaphore,
        .stop_socket_select = &lwip_stop_socket_select,
        .stop_socket_select_isr = &lwip_stop_socket_select_isr,
    // ... other members initialized

:cpp:func:`socket_select` is the internal implementation of :cpp:func:`select` for the socket driver. It works only with file descriptors belonging to the socket VFS.

:cpp:func:`get_socket_select_semaphore` returns the signalization object (semaphore) which is used in non-socket drivers to stop the waiting in :cpp:func:`socket_select`.

:cpp:func:`stop_socket_select` call is used to stop the waiting in :cpp:func:`socket_select` by passing the object returned by :cpp:func:`get_socket_select_semaphore`.

:cpp:func:`stop_socket_select_isr` has the same functionality as :cpp:func:`stop_socket_select` but it can be used from ISR.

Please see :component_file:`lwip/port/esp32xx/vfs_lwip.c` for a reference socket driver implementation using LWIP.

.. note::
    If you use :cpp:func:`select` for socket file descriptors only then you can disable the :ref:`CONFIG_VFS_SUPPORT_SELECT` option to reduce the code size and improve performance.
    You should not change the socket driver during an active :cpp:func:`select` call or you might experience some undefined behavior.

Paths
-----

Each registered FS has a path prefix associated with it. This prefix can be considered as a "mount point" of this partition.

In case when mount points are nested, the mount point with the longest matching path prefix is used when opening the file. For instance, suppose that the following filesystems are registered in VFS:

- FS 1 on /data
- FS 2 on /data/static

Then:

- FS 1 will be used when opening a file called ``/data/log.txt``
- FS 2 will be used when opening a file called ``/data/static/index.html``
- Even if ``/index.html"`` does not exist in FS 2, FS 1 will **not** be searched for ``/static/index.html``.

As a general rule, mount point names must start with the path separator (``/``) and must contain at least one character after path separator. However, an empty mount point name is also supported and might be used in cases when an application needs to provide a "fallback" filesystem or to override VFS functionality altogether. Such filesystem will be used if no prefix matches the path given.

VFS does not handle dots (``.``) in path names in any special way. VFS does not treat ``..`` as a reference to the parent directory. In the above example, using a path ``/data/static/../log.txt`` will not result in a call to FS 1 to open ``/log.txt``. Specific FS drivers (such as FATFS) might handle dots in file names differently.

When opening files, the FS driver receives only relative paths to files. For example:

1. The ``myfs`` driver is registered with ``/data`` as a path prefix.
2. The application calls ``fopen("/data/config.json", ...)``.
3. The VFS component calls ``myfs_open("/config.json", ...)``.
4. The ``myfs`` driver opens the ``/config.json`` file.

VFS does not impose any limit on total file path length, but it does limit the FS path prefix to ``ESP_VFS_PATH_MAX`` characters. Individual FS drivers may have their own filename length limitations.


File Descriptors
----------------

File descriptors are small positive integers from ``0`` to ``FD_SETSIZE - 1``, where ``FD_SETSIZE`` is defined in ``sys/select.h``. The largest file descriptors (configured by ``CONFIG_LWIP_MAX_SOCKETS``) are reserved for sockets. The VFS component contains a lookup-table called ``s_fd_table`` for mapping global file descriptors to VFS driver indexes registered in the ``s_vfs`` array.

Standard I/O streams (``stdin``, ``stdout``, ``stderr``) are mapped to file descriptors ``0``, ``1``, and ``2`` respectively. For more information on standard I/O, see :doc:`../../api-guides/stdio`.

``eventfd()``
-------------

``eventfd()`` call is a powerful tool to notify a ``select()`` based loop of custom events. The ``eventfd()`` implementation in ESP-IDF is generally the same as described in `man(2) eventfd <https://man7.org/linux/man-pages/man2/eventfd.2.html>`_ except for:

- ``esp_vfs_eventfd_register()`` has to be called before calling ``eventfd()``
- Options ``EFD_CLOEXEC``, ``EFD_NONBLOCK`` and ``EFD_SEMAPHORE`` are not supported in flags.
- Option ``EFD_SUPPORT_ISR`` has been added in flags. This flag is required to read and write the eventfd in an interrupt handler.

Note that creating an eventfd with ``EFD_SUPPORT_ISR`` will cause interrupts to be temporarily disabled when reading, writing the file and during the beginning and the ending of the ``select()`` when this file is set.

Well Known VFS Devices
----------------------

IDF defines several VFS devices that can be used by applications. These devices are, among others:

 * ``/dev/uart/<UART NUMBER>`` - file mapping to an UART opened with the VFS driver. The UART number is the number of the UART peripheral.
 * ``/dev/null`` - file that discards all data written to it and returns EOF when read. It is automatically created if :ref:`CONFIG_VFS_INITIALIZE_DEV_NULL` is enabled.
 * ``/dev/console`` - file that is connected to the primary and secondary outputs specified in the menuconfig by :ref:`CONFIG_ESP_CONSOLE_UART` and :ref:`CONFIG_ESP_CONSOLE_SECONDARY` respectively. More information can be found here :doc:`../../api-guides/stdio`.

Application Examples
--------------------

- :example:`system/eventfd` demonstrates how to use ``eventfd()`` to collect events from tasks and ISRs in a ``select()`` based main loop, using two tasks and a timer ISR (interrupt service routine) callback.

- :example:`system/select` demonstrates how to use synchronous I/O multiplexing with the ``select()`` function, using UART and socket file descriptors, and configuring both to act as loopbacks to receive messages sent from other tasks.

API Reference
-------------

.. include-build-file:: inc/esp_vfs.inc

.. include-build-file:: inc/esp_vfs_dev.inc

.. include-build-file:: inc/uart_vfs.inc

.. include-build-file:: inc/esp_vfs_eventfd.inc

.. include-build-file:: inc/esp_vfs_null.inc
