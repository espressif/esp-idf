Logging library
===============

Overview
--------

Log library has two ways of managing log verbosity: compile time, set via menuconfig; and runtime, using ``esp_log_level_set`` function.

At compile time, filtering is done using ``CONFIG_LOG_DEFAULT_LEVEL`` macro, set via menuconfig. All logging statments for levels higher than ``CONFIG_LOG_DEFAULT_LEVEL`` will be removed by the preprocessor.

At run time, all logs below ``CONFIG_LOG_DEFAULT_LEVEL`` are enabled by default. ``esp_log_level_set`` function may be used to set logging level per module. Modules are identified by their tags, which are human-readable ASCII zero-terminated strings.

How to use this library
-----------------------

In each C file which uses logging functionality, define TAG variable like this:

.. code-block:: c

   static const char* TAG = "MyModule";

then use one of logging macros to produce output, e.g:

.. code-block:: c

   ESP_LOGW(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);

Several macros are available for different verbosity levels:

* ``ESP_LOGE`` - error
* ``ESP_LOGW`` - warning
* ``ESP_LOGI`` - info
* ``ESP_LOGD`` - debug
* ``ESP_LOGV`` - verbose

Additionally there is an _EARLY_ variant for each of these macros (e.g. ``ESP_EARLY_LOGE`` ).These variants can run in startup code, before heap allocator and syscalls have been initialized. When compiling bootloader, normal ``ESP_LOGx`` macros fall back to the same implementation as ``ESP_EARLY_LOGx`` macros. So the only place where ``ESP_EARLY_LOGx`` have to be used explicitly is the early startup code, such as heap allocator initialization code.

(Note that such distinction would not have been necessary if we would have an ``ets_vprintf`` function in the ROM. Then it would be possible to switch implementation from _EARLY_ version to normal version on the fly. Unfortunately, ``ets_vprintf`` in ROM has been inlined by the compiler into ``ets_printf``, so it is not accessible outside.)

To override default verbosity level at file or component scope, define ``LOG_LOCAL_LEVEL`` macro. At file scope, define it before including ``esp_log.h``, e.g.:

.. code-block:: c

   #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
   #include "esp_log.h"


At component scope, define it in component makefile:

.. code-block:: make

   CFLAGS += -D LOG_LOCAL_LEVEL=ESP_LOG_DEBUG

To configure logging output per module at runtime, add calls to ``esp_log_level_set`` function:

.. code-block:: c

   esp_log_level_set("*", ESP_LOG_ERROR);        // set all components to ERROR level
   esp_log_level_set("wifi", ESP_LOG_WARN);      // enable WARN logs from WiFi stack
   esp_log_level_set("dhcpc", ESP_LOG_INFO);     // enable INFO logs from DHCP client

