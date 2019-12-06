.. highlight:: c

Error Handling
==============
:link_to_translation:`zh_CN:[中文]`

Overview
--------

Identifying and handling run-time errors is important for developing robust applications. There can be multiple kinds of run-time errors:

- Recoverable errors:
  
  - Errors indicated by functions through return values (error codes)
  - C++ exceptions, thrown using ``throw`` keyword

- Unrecoverable (fatal) errors:
  
  - Failed assertions (using ``assert`` macro and equivalent methods) and ``abort()`` calls.
  - CPU exceptions: access to protected regions of memory, illegal instruction, etc.
  - System level checks: watchdog timeout, cache access error, stack overflow, stack smashing, heap corruption, etc.

This guide explains ESP-IDF error handling mechanisms related to recoverable errors, and provides some common error handling patterns.

For instructions on diagnosing unrecoverable errors, see :doc:`Fatal Errors <fatal-errors>`.

Error codes
-----------

The majority of ESP-IDF-specific functions use :cpp:type:`esp_err_t` type to return error codes. :cpp:type:`esp_err_t` is a signed integer type. Success (no error) is indicated with ``ESP_OK`` code, which is defined as zero.

Various ESP-IDF header files define possible error codes using preprocessor defines. Usually these defines start with ``ESP_ERR_`` prefix. Common error codes for generic failures (out of memory, timeout, invalid argument, etc.) are defined in ``esp_err.h`` file. Various components in ESP-IDF may define additional error codes for specific situations.

For the complete list of error codes, see :doc:`Error Code Reference <../api-reference/error-codes>`.


Converting error codes to error messages
----------------------------------------

For each error code defined in ESP-IDF components, :cpp:type:`esp_err_t` value can be converted to an error code name using :cpp:func:`esp_err_to_name` or :cpp:func:`esp_err_to_name_r` functions. For example, passing ``0x101`` to :cpp:func:`esp_err_to_name` will return "ESP_ERR_NO_MEM" string. Such strings can be used in log output to make it easier to understand which error has happened.

Additionally, :cpp:func:`esp_err_to_name_r` function will attempt to interpret the error code as a `standard POSIX error code <http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html>`_, if no matching ``ESP_ERR_`` value is found. This is done using ``strerror_r`` function. POSIX error codes (such as ``ENOENT``, ``ENOMEM``) are defined in ``errno.h`` and are typically obtained from ``errno`` variable. In ESP-IDF this variable is thread-local: multiple FreeRTOS tasks have their own copies of ``errno``. Functions which set ``errno`` only modify its value for the task they run in.

This feature is enabled by default, but can be disabled to reduce application binary size. See :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP`. When this feature is disabled, :cpp:func:`esp_err_to_name` and :cpp:func:`esp_err_to_name_r` are still defined and can be called. In this case, :cpp:func:`esp_err_to_name` will return ``UNKNOWN ERROR``, and :cpp:func:`esp_err_to_name_r` will return ``Unknown error 0xXXXX(YYYYY)``, where ``0xXXXX`` and ``YYYYY`` are the hexadecimal and decimal representations of the error code, respectively.

``ESP_ERROR_CHECK`` macro
-------------------------

:cpp:func:`ESP_ERROR_CHECK` macro serves similar purpose as ``assert``, except that it checks :cpp:type:`esp_err_t` value rather than a ``bool`` condition. If the argument of :cpp:func:`ESP_ERROR_CHECK` is not equal :c:macro:`ESP_OK`, then an error message is printed on the console, and ``abort()`` is called.

Error message will typically look like this::

    ESP_ERROR_CHECK failed: esp_err_t 0x107 (ESP_ERR_TIMEOUT) at 0x400d1fdf

    file: "/Users/user/esp/example/main/main.c" line 20
    func: app_main
    expression: sdmmc_card_init(host, &card)

    Backtrace: 0x40086e7c:0x3ffb4ff0 0x40087328:0x3ffb5010 0x400d1fdf:0x3ffb5030 0x400d0816:0x3ffb5050

.. note:: If :doc:`IDF monitor <tools/idf-monitor>` is used, addresses in the backtrace will be converted to file names and line numbers.

- The first line mentions the error code as a hexadecimal value, and the identifier used for this error in source code. The latter depends on :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP` option being set. Address in the program where error has occured is printed as well.

- Subsequent lines show the location in the program where :cpp:func:`ESP_ERROR_CHECK` macro was called, and the expression which was passed to the macro as an argument.

- Finally, backtrace is printed. This is part of panic handler output common to all fatal errors. See :doc:`Fatal Errors <fatal-errors>` for more information about the backtrace.


Error handling patterns
-----------------------

1. Attempt to recover. Depending on the situation, this might mean to retry the call after some time, or attempt to de-initialize the driver and re-initialize it again, or fix the error condition using an out-of-band mechanism (e.g reset an external peripheral which is not responding).

   Example::

        esp_err_t err;
        do {
            err = sdio_slave_send_queue(addr, len, arg, timeout);
            // keep retrying while the sending queue is full
        } while (err == ESP_ERR_TIMEOUT);
        if (err != ESP_OK) {
            // handle other errors
        }

2. Propagate the error to the caller. In some middleware components this means that a function must exit with the same error code, making sure any resource allocations are rolled back.

   Example::

        sdmmc_card_t* card = calloc(1, sizeof(sdmmc_card_t));
        if (card == NULL) {
            return ESP_ERR_NO_MEM;
        }
        esp_err_t err = sdmmc_card_init(host, &card);
        if (err != ESP_OK) {
            // Clean up
            free(card);
            // Propagate the error to the upper layer (e.g. to notify the user).
            // Alternatively, application can define and return custom error code.
            return err;
        }

3. Convert into unrecoverable error, for example using ``ESP_ERROR_CHECK``. See `ESP_ERROR_CHECK macro`_ section for details.

   Terminating the application in case of an error is usually undesirable behaviour for middleware components, but is sometimes acceptable at application level.

   Many ESP-IDF examples use ``ESP_ERROR_CHECK`` to handle errors from various APIs. This is not the best practice for applications, and is done to make example code more concise.

   Example::

        ESP_ERROR_CHECK(spi_bus_initialize(host, bus_config, dma_chan));


C++ Exceptions
--------------

Support for C++ Exceptions in ESP-IDF is disabled by default, but can be enabled using :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS` option.

Enabling exception handling normally increases application binary size by a few kB. Additionally it may be necessary to reserve some amount of RAM for exception emergency pool. Memory from this pool will be used if it is not possible to allocate exception object from the heap. Amount of memory in the emergency pool can be set using :ref:`CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE` variable.

If an exception is thrown, but there is no ``catch`` block, the program will be terminated by ``abort`` function, and backtrace will be printed. See :doc:`Fatal Errors <fatal-errors>` for more information about backtraces.

See :example:`system/cpp_exceptions` for an example of C++ exception handling.

