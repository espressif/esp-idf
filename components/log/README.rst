Logging library
===============

Overview
--------

The logging library provides two ways for setting log verbosity:

- **At compile time**: in menuconfig, set the verbosity level using the option :ref:`CONFIG_LOG_DEFAULT_LEVEL`.
- Optionally, also in menuconfig, set the maximum verbosity level using the option :ref:`CONFIG_LOG_MAXIMUM_LEVEL`. By default this is the same as the default level, but it can be set higher in order to compile more optional logs into the firmware.
- **At runtime**: all logs for verbosity levels lower than :ref:`CONFIG_LOG_DEFAULT_LEVEL` are enabled by default. The function :cpp:func:`esp_log_level_set` can be used to set a logging level on a per module basis. Modules are identified by their tags, which are human-readable ASCII zero-terminated strings.

There are the following verbosity levels:

- Error (lowest)
- Warning
- Info
- Debug
- Verbose (highest)

.. note::

    The function :cpp:func:`esp_log_level_set` cannot set logging levels higher than specified by :ref:`CONFIG_LOG_MAXIMUM_LEVEL`. To increase log level for a specific file above this maximum at compile time, use the macro `LOG_LOCAL_LEVEL` (see the details below).


How to use this library
-----------------------

In each C file that uses logging functionality, define the TAG variable as shown below:

.. code-block:: c

   static const char* TAG = "MyModule";

Then use one of logging macros to produce output, e.g:

.. code-block:: c

   ESP_LOGW(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);

Several macros are available for different verbosity levels:

* ``ESP_LOGE`` - error (lowest)
* ``ESP_LOGW`` - warning
* ``ESP_LOGI`` - info
* ``ESP_LOGD`` - debug
* ``ESP_LOGV`` - verbose (highest)

Additionally, there are ``ESP_EARLY_LOGx`` versions for each of these macros, e.g. :c:macro:`ESP_EARLY_LOGE`. These versions have to be used explicitly in the early startup code only, before heap allocator and syscalls have been initialized. Normal ``ESP_LOGx`` macros can also be used while compiling the bootloader, but they will fall back to the same implementation as ``ESP_EARLY_LOGx`` macros.

There are also ``ESP_DRAM_LOGx`` versions for each of these macros, e.g. :c:macro:`ESP_DRAM_LOGE`. These versions are used in some places where logging may occur with interrupts disabled or with flash cache inaccessible. Use of this macros should be as sparing as possible, as logging in these types of code should be avoided for performance reasons.

.. note::

   Inside critical sections interrupts are disabled so it's only possible to use ``ESP_DRAM_LOGx`` (preferred) or ``ESP_EARLY_LOGx``. Even though it's possible to log in these situations, it's better if your program can be structured not to require it.

To override default verbosity level at file or component scope, define the ``LOG_LOCAL_LEVEL`` macro.

At file scope, define it before including ``esp_log.h``, e.g.:

.. code-block:: c

   #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
   #include "esp_log.h"

At component scope, define it in the component CMakeLists:

.. code-block:: cmake

   target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")

To configure logging output per module at runtime, add calls to the function :cpp:func:`esp_log_level_set` as follows:

.. code-block:: c

   esp_log_level_set("*", ESP_LOG_ERROR);        // set all components to ERROR level
   esp_log_level_set("wifi", ESP_LOG_WARN);      // enable WARN logs from WiFi stack
   esp_log_level_set("dhcpc", ESP_LOG_INFO);     // enable INFO logs from DHCP client

.. note::

   The "DRAM" and "EARLY" log macro variants documented above do not support per module setting of log verbosity. These macros will always log at the "default" verbosity level, which can only be changed at runtime by calling ``esp_log_level("*", level)``.

Logging to Host via JTAG
^^^^^^^^^^^^^^^^^^^^^^^^

By default, the logging library uses the vprintf-like function to write formatted output to the dedicated UART. By calling a simple API, all log output may be routed to JTAG instead, making logging several times faster. For details, please refer to Section :ref:`app_trace-logging-to-host`.

