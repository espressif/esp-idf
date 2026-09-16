Standard I/O and Console Output
===============================

:link_to_translation:`zh_CN:[中文]`

ESP-IDF provides C standard I/O facilities, such as ``stdin``, ``stdout``, and ``stderr`` streams, as well as C standard library functions such as ``printf()`` which operate on these streams.

As common in POSIX systems, these streams are buffering wrappers around file descriptors:

- ``stdin`` is a buffered stream for reading input from the user, wrapping file descriptor ``STDIN_FILENO`` (0).
- ``stdout`` is a buffered stream for writing output to the user, wrapping ``STDOUT_FILENO`` (1).
- ``stderr`` is a buffered stream for writing error messages to the user, wrapping ``STDERR_FILENO`` (2).

In ESP-IDF, there is no practical distinction between ``stdout`` and ``stderr``, as both streams are sent to the same physical interface. Most applications will use only ``stdout``. For example, ESP-IDF logging functions always write to ``stdout`` regardless of the log level.

The underlying stdin, stdout, and stderr file descriptors are implemented based on :doc:`VFS drivers <../api-reference/storage/vfs>`.

On {IDF_TARGET_NAME}, ESP-IDF provides implementations of VFS drivers for I/O over:

.. list::

    - UART
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - USB Serial/JTAG
    :SOC_USB_OTG_CONSOLE_SUPPORTED: - USB CDC (using USB_OTG peripheral)
    - "Null" (no output)

Standard I/O is not limited to these options, though. See below on enabling custom destinations for standard I/O.

.. note::

    The console path is separate from an application UART that you open with the :doc:`UART driver <../api-reference/peripherals/uart>` on other pins. If a UART peripheral provides console output, a USB-to-UART bridge chip is required. Many development boards already include this bridge chip onboard for console communication between the PC and the default UART0. If a USB or USB Serial/JTAG peripheral provides console output, no adapter chip is needed. The USB connector on the development board connects directly to the chip's USB D+/D- pins. Traffic on other UART ports does not appear in ``idf.py monitor`` unless a given port is configured as the console. For details, see :doc:`Establish Serial Connection with {IDF_TARGET_NAME} <../get-started/establish-serial-connection>`.

Configuration
-------------

Built-in implementations of standard I/O can be selected using several Kconfig options:

.. list::

    - :menuitem:`CONFIG_ESP_CONSOLE_UART_DEFAULT` — Enables UART with default options (pin numbers, baud rate) for standard I/O.
    - :menuitem:`CONFIG_ESP_CONSOLE_UART_CUSTOM` — Enables UART for standard I/O, with TX/RX pin numbers and baud rate configurable via Kconfig.
    :SOC_USB_OTG_CONSOLE_SUPPORTED: - :menuitem:`CONFIG_ESP_CONSOLE_USB_CDC` — Enables USB CDC (using USB_OTG peripheral) for standard I/O. See :doc:`usb-otg-console` for details about hardware connections required.
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - :menuitem:`CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG` — Enables USB Serial/JTAG for standard I/O. See :doc:`usb-serial-jtag-console` for details about hardware connections required.
    - :menuitem:`CONFIG_ESP_CONSOLE_NONE` — Disables standard I/O. If this option is selected, ``stdin``, ``stdout``, and ``stderr`` will be mapped to ``/dev/null`` and won't produce any output or generate any input.

Enabling one of these options will cause the corresponding VFS driver to be built into the application and used to open ``stdin``, ``stdout``, and ``stderr`` streams. Data written to ``stdout`` and ``stderr`` will be sent over the selected interface, and input from the selected interface will be available on ``stdin``.

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    Secondary output
    ^^^^^^^^^^^^^^^^

    ESP-IDF has built-in support for sending standard output to a secondary destination. This option makes the application output visible on two interfaces at once, for example on both UART and USB Serial/JTAG.

    Note that secondary console is output-only:

        - data written to ``stdout`` and ``stderr`` by the application will be sent to both primary and secondary consoles
        - ``stdin`` will only contain data sent by the host to the primary console.

    The following secondary console options are available:

        - :menuitem:`CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG`

Standard Streams and FreeRTOS Tasks
-----------------------------------

ESP-IDF provides two different implementations of standard I/O streams based on the selected LibC implementation defined by :menuitem:`CONFIG_LIBC`. The behavior of ``stdin``, ``stdout``, and ``stderr`` streams differs between these implementations, particularly regarding how they are shared across FreeRTOS tasks.

Common to both implementations, each stream (``stdin``, ``stdout``, ``stderr``) has a mutex associated with it to protect the stream from concurrent access by multiple tasks. For example, if two tasks are writing to ``stdout`` at the same time, the mutex ensures that the outputs from each task are not mixed together.

Newlib
^^^^^^

In ESP-IDF, to save RAM, ``FILE`` objects for ``stdin``, ``stdout``, and ``stderr`` are shared between all FreeRTOS tasks, but the pointers to these objects are unique for every task. This means that:

- It is possible to change ``stdin``, ``stdout``, and ``stderr`` for any given task without affecting other tasks, e.g., by doing ``stdin = fopen("/dev/uart/1", "r")``.
- To change the default ``stdin``, ``stdout``, ``stderr`` streams for new tasks, modify ``_GLOBAL_REENT->_stdin`` (``_stdout``, ``_stderr``) before creating the task.
- Closing default ``stdin``, ``stdout``, or ``stderr`` using ``fclose`` closes the ``FILE`` stream object, which will affect all other tasks.

Picolibc
^^^^^^^^

According to the POSIX standard, all default ``stdin``, ``stdout``, and ``stderr`` streams are global and shared between all FreeRTOS tasks. This means that:

- Modifying ``stdin``, ``stdout``, or ``stderr`` will affect all other tasks. It is not possible to change standard I/O streams for specific tasks.
- If a thread-local stream is needed, it should be implemented in the application code by opening a file stream and using it within tasks, e.g., ``fscanf()``, ``fprintf()``, etc.

Blocking and non-blocking I/O
-----------------------------

UART
^^^^

By default, UART VFS uses simplified functions for reading from and writing to UART. Writes busy-wait until all data is put into UART FIFO, and reads are non-blocking, returning only the data present in the FIFO. Due to this non-blocking read behavior, higher level C library calls, such as ``fscanf("%d\n", &var);``, might not have desired results.

Applications which use the UART driver can instruct VFS to use the driver's interrupt driven, blocking read and write functions instead. This can be done using a call to the :cpp:func:`uart_vfs_dev_use_driver` function. It is also possible to revert to the basic non-blocking functions using a call to :cpp:func:`uart_vfs_dev_use_nonblocking`.

When the interrupt-driven driver is installed, it is also possible to enable/disable non-blocking behavior using ``fcntl`` function with ``O_NONBLOCK`` flag.

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    USB Serial/JTAG
    ^^^^^^^^^^^^^^^

    Similar to UART, the VFS driver for USB Serial/JTAG defaults to a simplified implementation: writes are blocking (busy-wait until all the data has been sent) and reads are non-blocking, returning only the data present in the FIFO. This behavior can be changed to use the interrupt-driven, blocking read and write functions of the USB Serial/JTAG driver by calling :cpp:func:`usb_serial_jtag_vfs_use_nonblocking`. Note that the USB Serial/JTAG driver has to be initialized using :cpp:func:`usb_serial_jtag_driver_install` beforehand. It is also possible to revert to the basic non-blocking functions using a call to :cpp:func:`usb_serial_jtag_vfs_use_nonblocking`.

    When the interrupt-driven driver is installed, it is also possible to enable/disable non-blocking behavior using ``fcntl`` function with ``O_NONBLOCK`` flag.

.. only:: SOC_USB_OTG_CONSOLE_SUPPORTED

    USB CDC (using USB_OTG peripheral)
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    USB CDC VFS driver provides blocking I/O behavior by default. It is possible to enable non-blocking behavior using ``fcntl`` function with ``O_NONBLOCK`` flag.

Newline conversion
------------------

VFS drivers provide an optional newline conversion feature for input and output. Internally, most applications send and receive lines terminated by the LF (``\n``) character. Different terminal programs may require different line termination, such as CR or CRLF.

Applications can configure this behavior globally using the following Kconfig options:

    - For output
        - :menuitem:`CONFIG_LIBC_STDOUT_LINE_ENDING_CRLF`
        - :menuitem:`CONFIG_LIBC_STDOUT_LINE_ENDING_CR`
        - :menuitem:`CONFIG_LIBC_STDOUT_LINE_ENDING_LF`
    - For input
        - :menuitem:`CONFIG_LIBC_STDIN_LINE_ENDING_CRLF`
        - :menuitem:`CONFIG_LIBC_STDIN_LINE_ENDING_CR`
        - :menuitem:`CONFIG_LIBC_STDIN_LINE_ENDING_LF`


It is also possible to configure line ending conversion for the specific VFS driver:

.. list::

    - For UART: :cpp:func:`uart_vfs_dev_port_set_rx_line_endings` and :cpp:func:`uart_vfs_dev_port_set_tx_line_endings`
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - For USB Serial/JTAG: :cpp:func:`usb_serial_jtag_vfs_set_rx_line_endings` and :cpp:func:`usb_serial_jtag_vfs_set_tx_line_endings`
    :SOC_USB_OTG_CONSOLE_SUPPORTED: - For USB CDC (using USB_OTG peripheral): :cpp:func:`esp_vfs_dev_cdcacm_set_rx_line_endings` and :cpp:func:`esp_vfs_dev_cdcacm_set_tx_line_endings`

Buffering
---------

By default, standard I/O streams are line buffered. This means that data written to the stream is not sent to the underlying device until a newline character is written, or the buffer is full. This means, for example, that if you call ``printf("Hello")``, the text will not be sent to the UART until you call ``printf("\n")`` or the stream buffer fills up due to other prints.

This behavior can be changed using the ``setvbuf()`` function. For example, to disable buffering for ``stdout``:

.. code-block:: c

    setvbuf(stdout, NULL, _IONBF, 0);

You can also use ``setvbuf()`` to increase the buffer size, or switch to fully buffered mode.

Custom channels for standard I/O
--------------------------------

To send application output to a custom channel (for example, a WebSocket connection), it is possible to create a custom VFS driver. See the :doc:`VFS documentation <../api-reference/storage/vfs>` for details. The VFS driver has to implement at least the following functions:

    - ``open()`` and ``close()``
    - ``write()``
    - ``read()`` — only if the custom channel is also used for input
    - ``fstat()`` — recommended, to provide correct buffering behavior for the I/O streams
    - ``fcntl()`` — only if non-blocking I/O has to be supported

Once you have created a custom VFS driver, use :cpp:func:`esp_vfs_register_fs()` to register it with VFS. Then, use ``fopen()`` to redirect ``stdout`` and ``stderr`` to the custom channel. For example:

.. code-block:: c

    FILE *f = fopen("/dev/mychannel", "w");
    if (f == NULL) {
        // handle the error here
    }
    stdout = f;
    stderr = f;

Note that logging functions (``ESP_LOGE()``, etc.) write their output to ``stdout``. Keep this in mind when using logging within the implementation of your custom VFS (or any components which it calls). For example, if the custom VFS driver's ``write()`` operation fails and uses ``ESP_LOGE()`` to log the error, this will cause the output to be sent to ``stdout``, which would again call the custom VFS driver's ``write()`` operation. This would result in an infinite loop. It is recommended to keep track of this re-entry condition in the VFS driver's ``write()`` implementation, and return immediately if the write operation is still in progress.

Console I/O multiplexer
-----------------------

In addition to redirecting ``stdout`` and ``stderr`` manually (as shown above), ESP-IDF provides a console I/O multiplexer that lets an application register one or more custom VFS backends and switch between them at runtime, without reassigning the ``stdin``/``stdout``/``stderr`` streams itself.

The multiplexer is exposed on ``/dev/console`` and manages backends in two roles:

- **Primary** — the active read and write backend. Both application input (``stdin``) and output (``stdout``/``stderr``) go through it. Primaries are kept on a stack: pushing a new primary suspends the previous one, and popping restores it. The default console selected in Kconfig sits at the base of the stack and is always kept as the ultimate fallback.
- **Auxiliary** — a write-only sink. Every byte written to ``stdout`` and ``stderr`` is fanned out to all registered auxiliaries in addition to the primary. This is useful, for example, to mirror console output to a log file or a network connection while keeping the interactive console on the physical interface.

The maximum number of backends that can be registered at once (primaries plus auxiliaries, including the Kconfig default) is set by :ref:`CONFIG_ESP_STDIO_MAX_VFS_ENTRIES`.

API overview
^^^^^^^^^^^^

The multiplexer API is declared in ``esp_stdio.h`` and is available when ``CONFIG_VFS_SUPPORT_IO`` is enabled:

- ``esp_stdio_register_io()`` — register a VFS backend as a write-only auxiliary and return an opaque handle. The backend starts receiving the write fan-out immediately.
- ``esp_stdio_push_primary()`` — promote a previously registered handle to the active read and write primary. The current primary is suspended on the stack.
- ``esp_stdio_pop_primary()`` — remove a primary from the stack, returning it to auxiliary status. Passing ``NULL`` removes the current (top) primary; passing a specific handle removes it from wherever it sits in the stack.
- ``esp_stdio_unregister_io()`` — remove a backend from the multiplexer entirely. If it is currently the active primary, it is popped first.

The backend is described by an ``esp_stdio_io_config_t`` structure:

.. list::

    - ``vfs_ops`` — pointer to the VFS operations table (must not be ``NULL``).
    - ``vfs_ctx`` — context pointer forwarded to every VFS callback.
    - ``path`` — path passed to ``open()`` inside the driver, for example ``"/"``.

.. note::

    The ops table, context, and path string passed in the configuration must remain valid until the backend is removed with ``esp_stdio_unregister_io()``.

Example
^^^^^^^

The following example registers a custom backend, makes it the active console, and later restores the previous one:

.. code-block:: c

    #include "esp_stdio.h"

    // A VFS operations table implemented by the application.
    extern const esp_vfs_fs_ops_t my_console_vfs;

    esp_stdio_handle_t handle;
    esp_stdio_io_config_t config = {
        .vfs_ops = &my_console_vfs,
        .vfs_ctx = NULL,
        .path = "/",
    };

    // Register as a write-only auxiliary: output is now mirrored to it.
    ESP_ERROR_CHECK(esp_stdio_register_io(&config, &handle));

    // Promote it to the active read + write primary.
    ESP_ERROR_CHECK(esp_stdio_push_primary(handle));

    // ... application uses the custom console for stdin/stdout/stderr ...

    // Restore the previous primary (the custom backend reverts to auxiliary).
    ESP_ERROR_CHECK(esp_stdio_pop_primary(handle));

    // Remove the backend from the multiplexer entirely.
    ESP_ERROR_CHECK(esp_stdio_unregister_io(handle));

The same re-entrancy caveat described above for logging applies here: the backend's ``write()`` implementation must not trigger logging that would recursively write to the console.
