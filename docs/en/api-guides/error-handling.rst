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

  - Failed assertions (using ``assert`` macro and equivalent methods, see :ref:`assertions`) and ``abort()`` calls.
  - CPU exceptions: access to protected regions of memory, illegal instruction, etc.
  - System level checks: watchdog timeout, cache access error, stack overflow, stack smashing, heap corruption, etc.

This guide explains ESP-IDF error handling mechanisms related to recoverable errors, and provides some common error handling patterns.

For instructions on diagnosing unrecoverable errors, see :doc:`Fatal Errors <fatal-errors>`.


Error Codes
-----------

The majority of ESP-IDF-specific functions use :cpp:type:`esp_err_t` type to return error codes. :cpp:type:`esp_err_t` is a signed integer type. Success (no error) is indicated with ``ESP_OK`` code, which is defined as zero.

Various ESP-IDF header files define possible error codes using preprocessor defines. Usually these defines start with ``ESP_ERR_`` prefix. Common error codes for generic failures (out of memory, timeout, invalid argument, etc.) are defined in ``esp_err.h`` file. Various components in ESP-IDF may define additional error codes for specific situations.

For the complete list of error codes, see :doc:`Error Code Reference <../api-reference/error-codes>`.


Converting Error Codes to Error Messages
----------------------------------------

For each error code defined in ESP-IDF components, :cpp:type:`esp_err_t` value can be converted to an error code name using :cpp:func:`esp_err_to_name` or :cpp:func:`esp_err_to_name_r` functions. For example, passing ``0x101`` to :cpp:func:`esp_err_to_name` will return a ``ESP_ERR_NO_MEM`` string. Such strings can be used in log output to make it easier to understand which error has happened.

Additionally, :cpp:func:`esp_err_to_name_r` function will attempt to interpret the error code as a `standard POSIX error code <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html>`_, if no matching ``ESP_ERR_`` value is found. This is done using ``strerror_r`` function. POSIX error codes (such as ``ENOENT``, ``ENOMEM``) are defined in ``errno.h`` and are typically obtained from ``errno`` variable. In ESP-IDF this variable is thread-local: multiple FreeRTOS tasks have their own copies of ``errno``. Functions which set ``errno`` only modify its value for the task they run in.

This feature is enabled by default, but can be disabled to reduce application binary size. See :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP`. When this feature is disabled, :cpp:func:`esp_err_to_name` and :cpp:func:`esp_err_to_name_r` are still defined and can be called. In this case, :cpp:func:`esp_err_to_name` will return ``UNKNOWN ERROR``, and :cpp:func:`esp_err_to_name_r` will return ``Unknown error 0xXXXX(YYYYY)``, where ``0xXXXX`` and ``YYYYY`` are the hexadecimal and decimal representations of the error code, respectively.


.. _esp-error-check-macro:

``ESP_ERROR_CHECK`` Macro
-------------------------

:c:macro:`ESP_ERROR_CHECK` macro serves similar purpose as ``assert``, except that it checks :cpp:type:`esp_err_t` value rather than a ``bool`` condition. If the argument of :c:macro:`ESP_ERROR_CHECK` is not equal :c:macro:`ESP_OK`, then an error message is printed on the console, and ``abort()`` is called.

Error message will typically look like this:

.. code-block:: none

    ESP_ERROR_CHECK failed: esp_err_t 0x107 (ESP_ERR_TIMEOUT) at 0x400d1fdf

    file: "/Users/user/esp/example/main/main.c" line 20
    func: app_main
    expression: sdmmc_card_init(host, &card)

    Backtrace: 0x40086e7c:0x3ffb4ff0 0x40087328:0x3ffb5010 0x400d1fdf:0x3ffb5030 0x400d0816:0x3ffb5050

.. note::

    If :doc:`ESP-IDF monitor <tools/idf-monitor>` is used, addresses in the backtrace will be converted to file names and line numbers.

- The first line mentions the error code as a hexadecimal value, and the identifier used for this error in source code. The latter depends on :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP` option being set. Address in the program where error has occurred is printed as well.

- Subsequent lines show the location in the program where :c:macro:`ESP_ERROR_CHECK` macro was called, and the expression which was passed to the macro as an argument.

- Finally, backtrace is printed. This is part of panic handler output common to all fatal errors. See :doc:`Fatal Errors <fatal-errors>` for more information about the backtrace.


.. _esp-error-check-without-abort-macro:

``ESP_ERROR_CHECK_WITHOUT_ABORT`` Macro
---------------------------------------

:c:macro:`ESP_ERROR_CHECK_WITHOUT_ABORT` macro serves similar purpose as ``ESP_ERROR_CHECK``, except that it will not call ``abort()``.


.. _esp-return-on-error-macro:

``ESP_RETURN_ON_ERROR`` Macro
-----------------------------

:c:macro:`ESP_RETURN_ON_ERROR` macro checks the error code, if the error code is not equal :c:macro:`ESP_OK`, it prints the message and returns the error code.


.. _esp-goto-on-error-macro:

``ESP_GOTO_ON_ERROR`` Macro
---------------------------

:c:macro:`ESP_GOTO_ON_ERROR` macro checks the error code, if the error code is not equal :c:macro:`ESP_OK`, it prints the message, sets the local variable ``ret`` to the code, and then exits by jumping to ``goto_tag``.


.. _esp-return-on-false-macro:

``ESP_RETURN_ON_FALSE`` Macro
-----------------------------

:c:macro:`ESP_RETURN_ON_FALSE` macro checks the condition, if the condition is not equal ``true``, it prints the message and returns with the supplied ``err_code``.


.. _esp-goto-on-false-macro:

``ESP_GOTO_ON_FALSE`` Macro
---------------------------

:c:macro:`ESP_GOTO_ON_FALSE` macro checks the condition, if the condition is not equal ``true``, it prints the message, sets the local variable ``ret`` to the supplied ``err_code``, and then exits by jumping to ``goto_tag``.


.. _check_macros_examples:

``CHECK MACROS`` Examples
-------------------------

Some examples

.. code-block:: c

    static const char* TAG = "Test";

    esp_err_t test_func(void)
    {
        esp_err_t ret = ESP_OK;

        ESP_ERROR_CHECK(x);                                         // err message printed if `x` is not `ESP_OK`, and then `abort()`.
        ESP_ERROR_CHECK_WITHOUT_ABORT(x);                           // err message printed if `x` is not `ESP_OK`, without `abort()`.
        ESP_RETURN_ON_ERROR(x, TAG, "fail reason 1");               // err message printed if `x` is not `ESP_OK`, and then function returns with code `x`.
        ESP_GOTO_ON_ERROR(x, err, TAG, "fail reason 2");            // err message printed if `x` is not `ESP_OK`, `ret` is set to `x`, and then jumps to `err`.
        ESP_RETURN_ON_FALSE(a, err_code, TAG, "fail reason 3");     // err message printed if `a` is not `true`, and then function returns with code `err_code`.
        ESP_GOTO_ON_FALSE(a, err_code, err, TAG, "fail reason 4");  // err message printed if `a` is not `true`, `ret` is set to `err_code`, and then jumps to `err`.

    err:
        // clean up
        return ret;
    }

.. note::

     If the option :ref:`CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT` in Kconfig is enabled, the error message will be discarded, while the other action works as is.

     The ``ESP_RETURN_XX`` and ``ESP_GOTO_xx`` macros cannot be called from ISR. While there are ``xx_ISR`` versions for each of them, e.g., ``ESP_RETURN_ON_ERROR_ISR``, these macros could be used in ISR.


Error Handling Patterns
-----------------------

1. Attempt to recover. Depending on the situation, we may try the following methods:

    - retry the call after some time;
    - attempt to de-initialize the driver and re-initialize it again;
    - fix the error condition using an out-of-band mechanism (e.g reset an external peripheral which is not responding).

    Example:

    .. code-block:: c

        esp_err_t err;
        do {
            err = sdio_slave_send_queue(addr, len, arg, timeout);
            // keep retrying while the sending queue is full
        } while (err == ESP_ERR_TIMEOUT);
        if (err != ESP_OK) {
            // handle other errors
        }

2. Propagate the error to the caller. In some middleware components this means that a function must exit with the same error code, making sure any resource allocations are rolled back.

    Example:

    .. code-block:: c

        sdmmc_card_t* card = calloc(1, sizeof(sdmmc_card_t));
        if (card == NULL) {
            return ESP_ERR_NO_MEM;
        }
        esp_err_t err = sdmmc_card_init(host, &card);
        if (err != ESP_OK) {
            // Clean up
            free(card);
            // Propagate the error to the upper layer (e.g., to notify the user).
            // Alternatively, application can define and return custom error code.
            return err;
        }

3. Convert into unrecoverable error, for example using ``ESP_ERROR_CHECK``. See `ESP_ERROR_CHECK macro`_ section for details.

    Terminating the application in case of an error is usually undesirable behavior for middleware components, but is sometimes acceptable at application level.

    Many ESP-IDF examples use ``ESP_ERROR_CHECK`` to handle errors from various APIs. This is not the best practice for applications, and is done to make example code more concise.

    Example:

    .. code-block:: c

        ESP_ERROR_CHECK(spi_bus_initialize(host, bus_config, dma_chan));


C++ Exceptions
--------------

See :ref:`cplusplus_exceptions`.
