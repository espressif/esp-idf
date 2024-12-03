Standard I/O and Console Output
===============================

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
    :esp32s2 or esp32s3: - USB CDC (using USB_OTG peripheral)
    - "Null" (no output)

Standard I/O is not limited to these options, though. See below on enabling custom destinations for standard I/O.

Configuration
-------------

Built-in implementations of standard I/O can be selected using several Kconfig options:

.. list::

    - :ref:`CONFIG_ESP_CONSOLE_UART_DEFAULT<CONFIG_ESP_CONSOLE_UART_DEFAULT>` — Enables UART with default options (pin numbers, baud rate) for standard I/O.
    - :ref:`CONFIG_ESP_CONSOLE_UART_CUSTOM<CONFIG_ESP_CONSOLE_UART_CUSTOM>` — Enables UART for standard I/O, with TX/RX pin numbers and baud rate configurable via Kconfig.
    :esp32s2 or esp32s3: - :ref:`CONFIG_ESP_CONSOLE_USB_CDC<CONFIG_ESP_CONSOLE_USB_CDC>` — Enables USB CDC (using USB_OTG peripheral) for standard I/O. See :doc:`usb-otg-console` for details about hardware connections required.
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - :ref:`CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG<CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG>` — Enables USB Serial/JTAG for standard I/O. See :doc:`usb-serial-jtag-console` for details about hardware connections required.
    - :ref:`CONFIG_ESP_CONSOLE_NONE<CONFIG_ESP_CONSOLE_NONE>` — Disables standard I/O. If this option is selected, ``stdin``, ``stdout``, and ``stderr`` will be mapped to ``/dev/null`` and won't produce any output or generate any input.

Enabling one of these option will cause the corresponding VFS driver to be built into the application and used to open ``stdin``, ``stdout``, and ``stderr`` streams. Data written to ``stdout`` and ``stderr`` will be sent over the selected interface, and input from the selected interface will be available on ``stdin``.

.. only:: SOC_USB_SERIAL_JTAG_SUPPORTED

    Secondary output
    ^^^^^^^^^^^^^^^^

    ESP-IDF has built-in support for sending standard output to a secondary destination. This option makes the application output visible on two interfaces at once, for example on both UART and USB Serial/JTAG.

    Note that secondary console is output-only:

        - data written to ``stdout`` and ``stderr`` by the application will be sent to both primary and secondary consoles
        - ``stdin`` will only contain data sent by the host to the primary console.

    The following secondary console options are available:

        - :ref:`CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG<CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG>`

Standard Streams and FreeRTOS Tasks
-----------------------------------

In ESP-IDF, to save RAM, ``FILE`` objects for ``stdin``, ``stdout``, and ``stderr`` are shared between all FreeRTOS tasks, but the pointers to these objects are unique for every task. This means that:

- It is possible to change ``stdin``, ``stdout``, and ``stderr`` for any given task without affecting other tasks, e.g., by doing ``stdin = fopen("/dev/uart/1", "r")``.
- To change the default ``stdin``, ``stdout``, ``stderr`` streams for new tasks, modify ``_GLOBAL_REENT->_stdin`` (``_stdout``, ``_stderr``) before creating the task.
- Closing default ``stdin``, ``stdout``, or ``stderr`` using ``fclose`` closes the ``FILE`` stream object, which will affect all other tasks.

Each stream (``stdin``, ``stdout``, ``stderr``) has a mutex associated with it. This mutex is used to protect the stream from concurrent access by multiple tasks. For example, if two tasks are writing to ``stdout`` at the same time, the mutex will ensure that the outputs from each task are not mixed together.

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

    Similar to UART, the VFS driver for USB Serial/JTAG defaults to a simplified implementation: writes are blocking (busy-wait until all the data has been sent) and reads are non-blocking, returning only the data present in the FIFO. This behavior can be changed to use the interrupt driven, blocking read and write functions of USB Serial/JTAG driver using a call to the :cpp:func:`usb_serial_jtag_vfs_use_nonblocking` function. Note that the USB Serial/JTAG driver has to be initialized using :cpp:func:`usb_serial_jtag_driver_install` beforehand. It is also possible to revert to the basic non-blocking functions using a call to :cpp:func:`usb_serial_jtag_vfs_use_nonblocking`.

    When the interrupt-driven driver is installed, it is also possible to enable/disable non-blocking behavior using ``fcntl`` function with ``O_NONBLOCK`` flag.

.. only:: esp32s2 or esp32s3

    USB CDC (using USB_OTG peripheral)
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    USB CDC VFS driver provides blocking I/O behavior by default. It is possible to enable non-blocking behavior using ``fcntl`` function with ``O_NONBLOCK`` flag.

Newline conversion
------------------

VFS drivers provide an optional newline conversion feature for input and output. Internally, most applications send and receive lines terminated by the LF (``\n``) character. Different terminal programs may require different line termination, such as CR or CRLF.

Applications can configure this behavior globally using the following Kconfig options:

    - :ref:`CONFIG_LIBC_STDOUT_LINE_ENDING_CRLF<CONFIG_LIBC_STDOUT_LINE_ENDING_CRLF>`, :ref:`CONFIG_LIBC_STDOUT_LINE_ENDING_CR<CONFIG_LIBC_STDOUT_LINE_ENDING_CR>`, :ref:`CONFIG_LIBC_STDOUT_LINE_ENDING_LF<CONFIG_LIBC_STDOUT_LINE_ENDING_LF>` - for output
    - :ref:`CONFIG_LIBC_STDIN_LINE_ENDING_CRLF<CONFIG_LIBC_STDIN_LINE_ENDING_CRLF>`, :ref:`CONFIG_LIBC_STDIN_LINE_ENDING_CR<CONFIG_LIBC_STDIN_LINE_ENDING_CR>`, :ref:`CONFIG_LIBC_STDIN_LINE_ENDING_LF<CONFIG_LIBC_STDIN_LINE_ENDING_LF>` - for input


It is also possible to configure line ending conversion for the specific VFS driver:

.. list::

    - For UART: :cpp:func:`uart_vfs_dev_port_set_rx_line_endings` and :cpp:func:`uart_vfs_dev_port_set_tx_line_endings`
    :SOC_USB_SERIAL_JTAG_SUPPORTED: - For USB Serial/JTAG: :cpp:func:`usb_serial_jtag_vfs_set_rx_line_endings` and :cpp:func:`usb_serial_jtag_vfs_set_tx_line_endings`
    :esp32s2 or esp32s3: - For USB CDC (using USB_OTG peripheral): :cpp:func:`esp_vfs_dev_cdcacm_set_rx_line_endings` and :cpp:func:`esp_vfs_dev_cdcacm_set_tx_line_endings`

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
