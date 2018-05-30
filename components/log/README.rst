Logging library
===============

Overview
--------

Log library has two ways of managing log verbosity: compile time, set via menuconfig; and runtime, using :cpp:func:`esp_log_level_set` function.

The log levels are Error, Warning, Info, Debug, and Verbose (from lowest to highest level of verbosity).

At compile time, filtering is done using :ref:`CONFIG_LOG_DEFAULT_LEVEL` option, set via menuconfig. All logging statements for levels higher than :ref:`CONFIG_LOG_DEFAULT_LEVEL` will be removed by the preprocessor.

At run time, all logs below :ref:`CONFIG_LOG_DEFAULT_LEVEL` are enabled by default. :cpp:func:`esp_log_level_set` function may be used to reduce logging level per module. Modules are identified by their tags, which are human-readable ASCII zero-terminated strings. 

Note that :cpp:func:`esp_log_level_set` can not increase logging level beyound that set by :ref:`CONFIG_LOG_DEFAULT_LEVEL`. To increase log level for a specific file at compile time, `LOG_LOCAL_LEVEL` macro can be used (see below for details).

How to use this library
-----------------------

In each C file which uses logging functionality, define TAG variable like this:

.. code-block:: c

   static const char* TAG = "MyModule";

then use one of logging macros to produce output, e.g:

.. code-block:: c

   ESP_LOGW(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);

Several macros are available for different verbosity levels:

* ``ESP_LOGE`` - error (lowest)
* ``ESP_LOGW`` - warning
* ``ESP_LOGI`` - info
* ``ESP_LOGD`` - debug
* ``ESP_LOGV`` - verbose (highest)

Additionally there is an ``_EARLY`` variant for each of these macros (e.g. :c:macro:`ESP_EARLY_LOGE`). These variants can run in startup code, before heap allocator and syscalls have been initialized. When compiling bootloader, normal ``ESP_LOGx`` macros fall back to the same implementation as ``ESP_EARLY_LOGx`` macros. So the only place where ``ESP_EARLY_LOGx`` have to be used explicitly is the early startup code, such as heap allocator initialization code.

To override default verbosity level at file or component scope, define ``LOG_LOCAL_LEVEL`` macro. At file scope, define it before including ``esp_log.h``, e.g.:

.. code-block:: c

   #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
   #include "esp_log.h"


At component scope, define it in component makefile:

.. code-block:: make

   CFLAGS += -D LOG_LOCAL_LEVEL=ESP_LOG_DEBUG

To configure logging output per module at runtime, add calls to :cpp:func:`esp_log_level_set` function:

.. code-block:: c

   esp_log_level_set("*", ESP_LOG_ERROR);        // set all components to ERROR level
   esp_log_level_set("wifi", ESP_LOG_WARN);      // enable WARN logs from WiFi stack
   esp_log_level_set("dhcpc", ESP_LOG_INFO);     // enable INFO logs from DHCP client

Logging to Host via JTAG
^^^^^^^^^^^^^^^^^^^^^^^^

By default logging library uses vprintf-like function to write formatted output to dedicated UART. By calling a simple API, all log output may be routed to JTAG instead, making logging several times faster. For details please refer to section :ref:`app_trace-logging-to-host`.

