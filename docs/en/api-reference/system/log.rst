Logging library
===============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-IDF provides a flexible logging system with two configurable versions, **Log V1** and **Log V2**, selectable via :ref:`CONFIG_LOG_VERSION`. This document outlines their features, configurations, usage guidelines, and performance comparisons.

- **Log V1** (default): The original implementation designed for simplicity. It is optimized for early and DRAM logging but has higher flash usage and lacks flexibility.
- **Log V2**: The enhanced implementation improves flexibility, reduces flash usage, and centralizes log formatting but requires a bit more stack.

**Log V2** is backward-compatible with **Log V1**, meaning projects written using **Log V1** can switch to **Log V2** without modification. However, projects utilizing Log V2-specific features cannot revert to **Log V1** due to compatibility constraints.

Features of **Log V1**
^^^^^^^^^^^^^^^^^^^^^^

- Formatting is included in the ``format`` argument and compiled into Flash.
- Fast early and DRAM logging compared to ESP_LOG.
- Simple implementation but has limitations:

  - Larger binary size due to redundant formatting items.
  - Inflexible due to the lack of support for custom log formatting.
  - Build errors point to the wrong argument number in macros.

Features of **Log V2**
^^^^^^^^^^^^^^^^^^^^^^

- Centralized formatting via a single function, :cpp:func:`esp_log`.
- Reduces binary size by only storing the user-defined format string.
- The timestamp is captured only when it is required for output and the log level permits logging.
- Allows customization for log output:

  - Disable/enable color, timestamps, or tags globally, per file or per message log.
  - Output logs without formatting (useful for binary logging).
  - Apply different log settings for bootloader and app.

- The format argument can be dynamic and set as a variable, allowing greater flexibility in constructing log messages.
- Unified handler for logs across bootloader, ISR, startup code, and constrained environments.
- Drawbacks:

  - Consumes more stack and memory.
  - The log handler is slightly slower than **Log V1**, but the difference is negligible compared to the time spent transferring the data, e.g. over UART.

Log Levels
----------

Log levels are configured separately for application and bootloader. This separation allows developers to apply different logging settings via Kconfig options for each. For example, concise logs can be enabled for the bootloader while detailed debugging is enabled for the application. Use the bootloader-specific Kconfig options to configure log levels for the bootloader independently of the main application.

There are six verbosity levels:

- **Verbose** - Highly detailed and frequent debugging messages, often including internal states, that may overwhelm the output. (highest)
- **Debug** - Detailed diagnostic messages intended to aid in debugging (e.g., variable values, pointer addresses).
- **Info** - General information messages that describe the normal operation of the system.
- **Warning** - Events that could potentially cause issues but have been handled or mitigated.
- **Error** - Critical errors indicating that the software cannot recover without intervention.
- **None** - No log output. Used to completely disable logging. (lowest)

Log Level Settings
------------------

Log level settings control which logs are included in the binary and their visibility at runtime. There are two types of log level settings:

- **Log level**: Specifies which log levels are displayed at runtime. The bootloader's **log level** is configured via :ref:`CONFIG_BOOTLOADER_LOG_LEVEL`, while the application's **log level** is set via :ref:`CONFIG_LOG_DEFAULT_LEVEL`. The current log level can be retrieved using the function ``esp_log_get_default_level``.

- **Maximum log level**: Determines which log levels are included in the binary. Logs above this level are discarded at compile time and excluded from the final image. It can be set higher than the **log level**, allowing additional logs to be included in the binary and enabling them for debugging later if needed via :cpp:func:`esp_log_level_set`. The :ref:`CONFIG_LOG_MAXIMUM_LEVEL` option enables this feature for the application, but the bootloader does not support it. For the bootloader, the **maximum log level** is always the same as the **log level**.

Example for the application: if the **log level** is set to **Warning** and the **maximum log level** is set to **Debug**, the binary will include log messages of levels **Error**, **Warning**, **Info**, and **Debug**. However, at runtime, only log messages of levels **Error** and **Warning** will be outputted unless the log level is explicitly changed using :cpp:func:`esp_log_level_set`. The log level can be adjusted, increased or decreased, depending on the user's needs.

``Maximum Log Level`` Setting
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``LOG_LOCAL_LEVEL`` definition allows you to override the **maximum log level** for a specific source file or component without modifying the Kconfig options. It effectively sets the **maximum log level** locally, enabling or excluding specific logs in the binary.

This approach is especially useful when you need more detailed logs for specific areas of the code without globally increasing the **maximum log level**, thereby avoiding unnecessary impacts on the binary size.

- Change the **maximum log level** for a source file (do not add it in header files, as it may not work due to the single-inclusion approach used by header files). Define ``LOG_LOCAL_LEVEL`` with one of the values from :cpp:type:`esp_log_level_t` before including ``esp_log.h``. This allows you to control which log messages are included in the binary for that specific source file.

    .. code-block:: c

        // in a my_file.c file
        #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
        #include "esp_log.h"

- Change the **maximum log level** for an entire component by defining ``LOG_LOCAL_LEVEL`` in the component's `CMakeLists.txt`. This ensures that the specified log level is applied across all source files within the component, controlling which log messages are included in the binary:

    .. code-block:: cmake

        # in a component's CMakeLists.txt file
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")

Runtime **Log Level** Setting
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Only the application supports changing the log level at runtime. The bootloader does not support this feature.

By default, all log levels up to the **log level** are enabled at the startup. The function :cpp:func:`esp_log_level_set` can be used to set the **log level** globally or on a per-module basis. Modules are identified by their tags, which are human-readable ASCII zero-terminated strings. This functionality depends on :ref:`CONFIG_LOG_DYNAMIC_LEVEL_CONTROL`, which is enabled by default. If this feature is not required, you can disable it to reduce code size and improve performance.

Example: Set the log level to ``ERROR`` for all components (global setting):

.. code-block:: c

   esp_log_level_set("*", ESP_LOG_ERROR);

Adjusting log output per module (tag) depends on :ref:`CONFIG_LOG_TAG_LEVEL_IMPL`, which is enabled by default. If this feature is not required, you can disable it to reduce code size and improve performance:

Example: Set the log level to ``WARNING`` only for the Wi-Fi component (module-specific setting):

.. code-block:: c

   esp_log_level_set("wifi", ESP_LOG_WARN);

Use Logging Library
---------------------

In each C file that uses the logging functionality, define the ``TAG`` variable.

.. code-block:: c

    // #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE // Optional: Increase log level that will be included in binary (only for this file)
    #include "esp_log.h"
    static const char* TAG = "MyModule";
    // ...
    ESP_LOGI(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);
    ESP_EARLY_LOGW(TAG, "Early log message %d", i++);
    ESP_DRAM_LOGE(DRAM_STR("TAG_IN_DRAM"), "DRAM log message %d", i++); // Use DRAM_STR macro to put in DRAM if needed

.. code-block:: bash

    I (112500) MyModule: Baud rate error 1.5%. Requested: 115200 baud, actual: 116928 baud
    W (112500) MyModule: Early log message 1
    E TAG_IN_DRAM: DRAM log message 2

.. note::

    The ``TAG`` variable points to a string literal stored in flash memory. If the same ``TAG`` string is used multiple times within a single build unit (translation unit), the compiler and linker typically optimize it to a single copy in flash through a process called **string pooling**. However, if the same ``TAG`` string is used across different components or translation units, each component or unit will have its own copy in flash unless global linker optimizations are applied.

The logging library provides a wide range of macros to accommodate various use cases, from general-purpose logging to early startup and constrained environments. Choosing the right macro and structuring your program accordingly can help optimize performance and ensure reliable operation. However, it is recommended to structure your program to avoid logging in constrained environments whenever possible.

- Verbose: :c:macro:`ESP_LOGV`, :c:macro:`ESP_EARLY_LOGV`, :c:macro:`ESP_DRAM_LOGV`.
- Debug: :c:macro:`ESP_LOGD`, :c:macro:`ESP_EARLY_LOGD`, :c:macro:`ESP_DRAM_LOGD`.
- Info: :c:macro:`ESP_LOGI`, :c:macro:`ESP_EARLY_LOGI`, :c:macro:`ESP_DRAM_LOGI`.
- Warning: :c:macro:`ESP_LOGW`, :c:macro:`ESP_EARLY_LOGW`, :c:macro:`ESP_DRAM_LOGW`.
- Error: :c:macro:`ESP_LOGE`, :c:macro:`ESP_EARLY_LOGE`, :c:macro:`ESP_DRAM_LOGE`.

There are three groups of macros available:

- **ESP_LOGx**: Standard logging macros suitable for most use cases during normal operation. Use these in your application code for logging in non-constrained environments, avoiding use in ISRs, early startup, or when the flash cache is disabled. A key characteristic of these macros is that they use the `vprintf` function from the Newlib library for formatting and outputting logs.

- **ESP_EARLY_LOGx**: Designed for use in constrained environments during early startup, before the heap allocator or syscalls are initialized. These macros are commonly used in critical startup code or in critical sections where interrupts are disabled. A key characteristic of these macros is that they use the ROM `printf` function, always output timestamps in microseconds, and do not support per-module log verbosity settings.

- **ESP_DRAM_LOGx**: Designed for use in constrained environments where logging occurs with interrupts disabled or when the flash cache is inaccessible. These macros should be used sparingly, as they can impact performance. They are suitable for critical sections or interrupt routines where other logging macros may not work reliably. A key characteristic of these macros is that they use the ROM `printf` function, do not output timestamps, allocate the format argument in DRAM to ensure accessibility when the cache is disabled, and do not support per-module log verbosity settings.

.. Note::
    Use the **DRAM_STR("my_tag")** macro to allocate the tag in DRAM. This is necessary to ensure access to the tag when the flash cache is disabled.

The difference between **Log V1** and **Log V2** is that in **Log V2**, all logs from these macros are routed through a single handler. This handler can automatically detect constrained environments (e.g., early startup, disabled interrupts, or flash cache inaccessible) and dynamically selects the appropriate printing function, ensuring efficient logging across various runtime contexts.

Log Format
----------

- **Log V1**: Only supports disabling color formatting globally. Other formatting options, such as timestamp and tag, are always enabled.

- **Log V2**:

  - Allows complete customization of formatting, including the ability to disable color, tag, and timestamp formatting globally, per file, per module, or even for individual log messages.
  - Provides finer control over log output, making it more adaptable to specific use cases and environments.

.. code-block:: c

    // #define ESP_LOG_COLOR_DISABLED     (1)  /* For Log v2 only */
    // #define ESP_LOG_TIMESTAMP_DISABLED (1)  /* For Log v2 only */
    #include "esp_log.h"
    static const char* TAG = "boot";
    // ...
    ESP_LOGI(TAG, "chip revision: v%d.%d", major, minor);

.. code-block:: none

    I (56) boot: chip revision: v3.0

        level name                            |end of line
           |                                  |
     [0;32mI (56) boot: chip revision: v3.0[0m
    |_____|  |___||____||_________________||_|
    |start   |    |tag  |                  |end color
    |color   |          |user string
             |timestamp

The logging system supports the following formatting options, applicable for both the application and bootloader:

- **Color**: Adds color codes to enhance log visibility globally. Controlled by :ref:`CONFIG_LOG_COLORS`, which is disabled by default because the ESP-IDF monitor tool (`idf.py monitor`) can detect the log level by its **level name** and apply the standard IDF color scheme.

  - For **Log V2**, the :ref:`CONFIG_LOG_COLORS_SUPPORT` option enables runtime support for adding color output to specific logs, files, or components, even if global color is disabled. To enable color for a specific context use ``ESP_LOG_COLOR_DISABLED``.

- **Level Name**: A single letter (I, W, E, D, V) indicating log verbosity, displayed at the start of each message. Useful for identifying log levels, especially when color is disabled, as utilized by the ESP-IDF monitor tool.

- **Timestamp**: Adds a timestamp to log messages globally. Controlled by :ref:`CONFIG_LOG_TIMESTAMP_SOURCE`.

  - **None**: No timestamp. Useful for log analysis or debugging where timing is not critical. Saves processing power and memory. Available only for **Log V2**.
  - **Milliseconds since boot** `(18532)` (default): Derived from the RTOS tick count multiplied by the tick period.
  - **System time (HH:MM:SS.sss)** `(14:31:18.532)`: Displays time in hours, minutes, seconds, and milliseconds.
  - **System time (YY-MM-DD HH:MM:SS.sss)** `(2023-08-15 14:31:18.532)`: Similar to the above, but also includes the date.
  - **Unix time in milliseconds** `(1692099078532)`: Displays Unix time in milliseconds.
  - For **Log V2**, the :ref:`CONFIG_LOG_TIMESTAMP_SUPPORT` option enables runtime support for adding timestamp output to specific logs, files, or components, even if global timestamp is disabled. To enable the **Milliseconds since boot** timestamp for a specific context, use ``ESP_LOG_TIMESTAMP_DISABLED``.

- **Tag**: Displays a user-defined identifier for the source module.

  - For **Log V2**, the tag can be passed to the macros as ``NULL``, in which case it will not be printed, and per-component log level check will not work.

- **End Line**: Adds a newline character at the end of the log messages.

The following options are applicable only for **Log V2** and are used alongside the provided log macros. These definitions can be set in the same manner as ``LOG_LOCAL_LEVEL``. Their scope depends on where they are defined (e.g., file, component, or globally):

- **ESP_LOG_CONSTRAINED_ENV**:

  - Define as ``1`` to force the log handler :cpp:func:`esp_log` to use a safe printf function suitable for the specified scope.

- **ESP_LOG_FORMATTING_DISABLED**:

  - Default: ``0`` (enables all formatting items such as color, timestamps, tags, and end line).
  - Define as ``1`` to disable all formatting items for the specified scope.

- **ESP_LOG_COLOR_DISABLED**: Requires :ref:`CONFIG_LOG_COLORS_SUPPORT` to be enabled.

  - If global color (:ref:`CONFIG_LOG_COLORS`) is disabled, define as ``0`` to enable color output for the specified scope.
  - If global color (:ref:`CONFIG_LOG_COLORS`) is enabled, define as ``1`` to disable color output for the specified scope.

- **ESP_LOG_TIMESTAMP_DISABLED**: Requires :ref:`CONFIG_LOG_TIMESTAMP_SUPPORT` to be enabled.

  - If global timestamping (:ref:`CONFIG_LOG_TIMESTAMP_SOURCE`) is disabled, define as ``0`` to enable tick timestamp output for the specified scope.
  - If global timestamping (:ref:`CONFIG_LOG_TIMESTAMP_SOURCE`) is enabled, define as ``1`` to disable tick timestamp output for the specified scope.

- **ESP_LOG_MODE_BINARY_EN**: Requires ``CONFIG_LOG_MODE_BINARY`` or ``CONFIG_BOOTLOADER_LOG_MODE_BINARY`` to be enabled.

  - Setting ``ESP_LOG_MODE_BINARY_EN`` to ``0`` does not make sense for regular use because logs will still be sent in binary mode. However, the format string will not be removed from flash, and argument analysis will be performed at runtime. This setting may be useful for specific scenarios such as debugging or testing.
  - Setting ``ESP_LOG_MODE_BINARY_EN`` to ``1`` when text logging mode is enabled will have no effect. In this case, ``ESP_LOG_MODE_BINARY_EN`` will be suppressed and automatically defined as ``0``.

Per-Log Formatting
^^^^^^^^^^^^^^^^^^

The above definition works seamlessly with the provided log macros. However, if you require more flexibility or the ability to change settings at runtime, such as adjusting the log level based on a value (for example, temperature), this can be done using alternative macros. Note that in this case, the logs cannot be discarded from the binary, as they bypass compile-time log level checks.

The example below demonstrates how to adjust formatting for individual log messages:

.. code-block:: c

    #include "esp_log.h"
    esp_log_config_t configs = {
        .opts = {
            .log_level = ESP_LOG_INFO,                 // Set log level
            .constrained_env = false,                  // Specify constrained environment
            .require_formatting = true,                // Enable formatting
            .dis_color = ESP_LOG_COLOR_DISABLED,       // Use global color setting
            .dis_timestamp = ESP_LOG_TIMESTAMP_DISABLED, // Use global timestamp setting
            .reserved = 0,                             // Reserved for future use
        }
    };
    // ...
    if (temperature > 55) {
        configs.opts.log_level = ESP_LOG_WARN;
    }
    // Similar to ESP_LOGx macros but allows applying custom configurations
    // If the configs var is constant, the compiler can exclude the log during compilation
    // if it is below the maximum log level, otherwise not.
    ESP_LOG_LEVEL_LOCAL(configs, TAG, "Temp = %dC", temperature);

    // Note: The following calls bypass compile-time log level checks,
    // they cannot be discarded from the binary
    esp_log(configs, TAG, "Temp = %dC", temperature);
    ESP_LOG_LEVEL(configs, TAG, "Temp = %dC", temperature);

Log Level Control
-----------------

Only the application supports changing the log level at runtime. The bootloader does not support this feature.

The logging library allows adjusting log output per module (tag) at runtime using the function :cpp:func:`esp_log_level_set`. This feature applies only to non-constrained environments (**ESP_LOGx** macros). Constrained environments (e.g., **ESP_EARLY_LOGx** or **ESP_DRAM_LOGx**) do not support dynamic log levels due to the absence of locks and lightweight requirements in their log handlers.

.. code-block:: c

   // Set log level to ERROR for all components (global setting)
   esp_log_level_set("*", ESP_LOG_ERROR);

   // Set log level to WARNING for the WiFi component (module-specific setting)
   esp_log_level_set("wifi", ESP_LOG_WARN);

   // Set log level to INFO for the DHCP client (module-specific setting)
   esp_log_level_set("dhcpc", ESP_LOG_INFO);

There are three settings that control the ability to change the log level at runtime globally or per module (tag):

- **Dynamic Log Level Control** (:ref:`CONFIG_LOG_DYNAMIC_LEVEL_CONTROL`, enabled by default): Enables runtime log level changes via :cpp:func:`esp_log_level_set`. This feature increases flexibility but adds memory and performance overhead. If binary size is a concern and dynamic log level changes are unnecessary, consider disabling this option, especially when :ref:`CONFIG_LOG_TAG_LEVEL_IMPL` is set to **None**, to minimize program size.

  If your application does not require dynamic log level adjustments, disabling this option can improve efficiency by:

  - Reducing memory consumption:

    - **IRAM**: about 260 bytes
    - **DRAM**: about 264 bytes
    - **Flash**: about 1 KB

  - Boosting log operation performance by up to 10 times.

- **Tag-Level Checks** (:ref:`CONFIG_LOG_TAG_LEVEL_IMPL`, default **Cache + Linked List**): Determines how per-tag log level checks are performed, affecting memory usage and lookup speed:

  - **None**: Disables per-tag log level checks entirely, reducing overhead but removing runtime flexibility.

  - **Linked List**: Enables per-tag log level settings using a linked list-only implementation (no cache). This method searches through all tags in the linked list to determine the log level, which may result in slower lookups for a large number of tags but consumes less memory compared to the **Cache** approach. The linked list approach performs full string comparisons of log tags to identify the appropriate log level. Unlike **Cache**, it does not rely on tag pointer comparisons, making it suitable for dynamic tag definitions. Select this option if you prioritize memory savings, need to enable or disable logs for specific modules, or want to use tags defined as variables. Selecting this option automatically enables **Dynamic Log Level Control**. The linked list entries are allocated on the heap during the execution of ``ESP_LOGx`` macros when a new tag is encountered.

  - **Cache + Linked List** (Default): It is a hybrid mode that combines caching with a **linked list** for log tag level checks. This hybrid approach offers a balance between speed and memory usage. The cache stores recently accessed log tags and their corresponding log levels, providing faster lookups for frequently used tags. The cache approach compares the tag pointers, which is faster than performing full string comparisons. For less frequently used tags, the **linked list** is utilized to search for the log level. This option may not work properly when dynamic tag definitions are used, as it relies on tag pointer comparisons in the cache, which are not suitable for dynamically defined tags. This hybrid approach improves the efficiency of log level retrieval by leveraging the speed of caching for common tags and the memory efficiency of a linked list for less frequently used tags. Selecting this option automatically enables **Dynamic Log Level Control**.

    There are some cache configurations to balance memory usage and lookup performance. These settings determine how log tag levels are stored and accessed: :ref:`CONFIG_LOG_TAG_LEVEL_CACHE_IMPL`.

    - **Array**: A simple implementation without reordering, suitable for low-memory applications that prioritize simplicity.

    - **Binary Min-Heap** (default): An optimized implementation for fast lookups with automatic reordering. Ideal for high-performance applications with sufficient memory. The **Cache Size** (:ref:`CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_SIZE`) defines the capacity, which defaults to 31 entries.

    A larger cache size enhances lookup performance for frequently accessed log tags but increases memory consumption. In contrast, a smaller cache size conserves memory but may result in more frequent evictions of less commonly used log tags.

- **Master Log Level** (:ref:`CONFIG_LOG_MASTER_LEVEL`, disabled by default): It is an optional setting designed for specific debugging scenarios. It enables a global "master" log level check that occurs before timestamps and tag cache lookups. This is useful for compiling numerous logs that can be selectively enabled or disabled at runtime while minimizing performance impact when log output is unnecessary.

  Common use cases include temporarily disabling logs during time-critical or CPU-intensive operations and re-enabling them later.

  .. note:: For **Log V1**, this feature may significantly increase program size based on the number of compiled logs. For **Log V2**, the impact is minimal as the check is integrated within the log handler.

  If enabled, the master log level defaults to :ref:`CONFIG_LOG_DEFAULT_LEVEL` and can be adjusted at runtime using :cpp:func:`esp_log_set_level_master`. This global check takes precedence over ``esp_log_get_default_level``.

  The snippet below shows how it works. Setting the **Master log level** to ``ESP_LOG_NONE`` disables all logging globally. :cpp:func:`esp_log_level_set` does not currently affect logging. However, after the **Master log level** is adjusted to a higher level, logs will be printed as configured by :cpp:func:`esp_log_level_set`:

  .. code-block:: c

      // Master logging level is CONFIG_LOG_DEFAULT_LEVEL at start-up and = ESP_LOG_INFO
      ESP_LOGI("lib_name", "Message for print");          // Prints an INFO message
      esp_log_level_set("lib_name", ESP_LOG_WARN);        // Enables WARN logs for lib_name

      // Disables all logs globally. esp_log_level_set has no effect at the moment
      esp_log_set_level_master(ESP_LOG_NONE);

      ESP_LOGW("lib_name", "Message for print");          // No print, Master logging level blocks it
      esp_log_level_set("lib_name", ESP_LOG_INFO);        // Enables INFO logs for lib_name
      ESP_LOGI("lib_name", "Message for print");          // No print, Master logging level blocks it

      // Enables all INFO logs globally
      esp_log_set_level_master(ESP_LOG_INFO);

      ESP_LOGI("lib_name", "Message for print");          // Prints an INFO message

.. note::

    Even when logs are disabled by tag, processing still takes approximately 10.9 microseconds. To reduce this overhead, consider using the **Master Log Level** or disabling **Tag-Level Checks** functionality.

Logging of Buffers
------------------

The logging system provides macros for logging buffer data. These macros can be used in both bootloader and application, and they are independent of the log version. Available macros:

- :c:macro:`ESP_LOG_BUFFER_HEX` and :c:macro:`ESP_LOG_BUFFER_HEX_LEVEL`: Logs a buffer of hexadecimal bytes. The data is split into lines with 16 bytes per line. :c:macro:`ESP_LOG_BUFFER_HEX` is only for the ``Info`` log level.

  .. code-block:: c

    #include "esp_log_buffer.h"
    uint8_t buffer[] = {
        0x54, 0x68, 0x65, 0x20, 0x77, 0x61, 0x79, 0x20,
        0x74, 0x6f, 0x20, 0x67, 0x65, 0x74, 0x20, 0x73,
        0x74, 0x61, 0x72, 0x74, 0x65, 0x64, 0x20, 0x69,
        0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x66
    };
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buffer, sizeof(buffer), ESP_LOG_DEBUG);

  .. code-block:: none

    I (954) MyModule: 54 68 65 20 77 61 79 20 74 6f 20 67 65 74 20 73
    I (964) MyModule: 74 61 72 74 65 64 20 69 73 20 61 6e 64 20 66

- :c:macro:`ESP_LOG_BUFFER_CHAR` and :c:macro:`ESP_LOG_BUFFER_CHAR_LEVEL`: Logs a buffer of printable characters. Each line contains up to 16 characters. :c:macro:`ESP_LOG_BUFFER_CHAR` is only for the ``Info`` log level.

  .. code-block:: c

    #include "esp_log_buffer.h"
    char buffer[] = "The quick brown fox jumps over the lazy dog.";
    ESP_LOG_BUFFER_CHAR_LEVEL(TAG, buffer, sizeof(buffer), ESP_LOG_WARN);

  .. code-block:: none

    I (980) MyModule: The quick brown
    I (985) MyModule: fox jumps over
    I (990) MyModule: the lazy dog.

- :c:macro:`ESP_LOG_BUFFER_HEXDUMP`: Dumps a buffer in a formatted hex dump style, displaying both the memory address and corresponding ASCII values. This is especially useful for debugging raw memory content.

  .. code-block:: c

    #include "esp_log_buffer.h"
    uint8_t buffer[] = {
        0x54, 0x68, 0x65, 0x20, 0x77, 0x61, 0x79, 0x20,
        0x74, 0x6f, 0x20, 0x67, 0x65, 0x74, 0x20, 0x73,
        0x74, 0x61, 0x72, 0x74, 0x65, 0x64, 0x20, 0x69
    };
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, sizeof(buffer), ESP_LOG_INFO);

  .. code-block:: none

    I (1013) MyModule: 0x3ffb5bc0   54 68 65 20 77 61 79 20  74 6f 20 67 65 74 20 73  |The way to get s|
    I (1024) MyModule: 0x3ffb5bd0   74 61 72 74 65 64 20 69  73 20 74 6f 20 71 75 69  |tarted is to qui|

The number of lines in the output depends on the size of the buffer.

Binary Logging
--------------

Binary logging is a feature available only in **Log V2**, enabling logs to be transmitted in binary format instead of text. This is configured separately for the **bootloader** (``CONFIG_BOOTLOADER_LOG_MODE_BINARY``) and **application** (``CONFIG_LOG_MODE_BINARY``) via Kconfig options.

By default, when **Log V2** is enabled, the logging system uses **text mode**. Enabling binary logging reduces flash memory usage by removing log format strings from flash and sending only their addresses instead. Additionally, ``printf`` functions are not used, which reduces both stack usage and flash consumption.

This feature introduces the :c:macro:`ESP_LOG_ATTR_STR` macro, which relocates format strings to a ``.noload`` section, effectively removing them from the final binary image. You can also use this mechanism for assertions or user-defined logging messages to further minimize flash usage.

Summary of Benefits:

- Reduces **flash size** by approximately **10% – 35%**, depending on the application. The more extensive the logging in a program, the greater the potential savings.
- Minimizes **stack usage** by eliminating the need for the ``vprintf``-like function for log formatting.
- Reduces **log transmission overhead** by transmitting compact binary data.

Binary logging is especially beneficial in **resource-constrained** environments where flash size optimization and efficient logging are critical.

Binary Logging Workflow
-----------------------

Binary logging consists of two main components:

1.  :ref:`Chip Side <chip-side>`: Encodes and transmits log data.

    - Encoding process
    - Argument type encoding
    - Runtime argument type encoding

2.  :ref:`Host Side <host-side>`: Receives and decodes data using the `esp-idf-monitor tool <https://github.com/espressif/esp-idf-monitor>`_. The ``idf.py monitor`` command automatically decodes binary logs.

    - Detects binary log packets.
    - Extracts packet fields (log level, format, tag, timestamp, arguments).
    - Determines whether addresses reference:

        - **ELF file** (requires lookup)
        - **Embedded string** (contained in the packet)

    - Decodes arguments using the format string and the given array of arguments.
    - Reconstructs the final log message by coupling the format string with the decoded arguments.
    - Applies terminal colorization.

.. _chip-side:

Chip Side
^^^^^^^^^

Encoding Process
""""""""""""""""

Binary logs are transmitted as structured packets. Strings are sent as addresses if they exist in the ELF file. For runtime-generated strings, an embedded string format is used to transmit the string to the host.

Packet structure:

.. code-block:: none

    [0]  - Message Type (1: bootloader, 2: application, ...)
    [1]  - Control Byte (log level, version, time_64bits flag)
    [2]  - Length (10-bit, max 1023 bytes)
    [3-6]  - Format Address (if present in ELF) or embedded string
    [7-10] - Tag Address (if present in ELF) or embedded string
    [11-14] - Timestamp (32-bit if timestamp does not exceed 32 bits. [11-18] - occupied if timestamp takes 64 bits, time_64bits flag is set in Control Byte)
    [...] - Arguments (optional). It is an array of arguments: 32-bit, 64-bit, pointers, and embedded string/data.
    [15] - CRC8 checksum

The embedded string format is used if string is not present in ELF file, it follows this structure:

.. code-block:: none

    [0] - Embedded Identifier (0xFF - 0xFC)
    [0,1] - (10-bit) Negative Length of the string = 1 - len(str)
    [...] - String Content

.. note::

    All multi-byte fields in the packet structure use big-endian encoding.

Argument Type Encoding
""""""""""""""""""""""

Since the format string is removed from the final binary, the chip must still identify argument types to correctly transmit them to the host. This is achieved using the :c:macro:`ESP_LOG_ARGS_TYPE` macro, which leverages the `_Generic` feature to classify user arguments at compile time into three categories: **32-bit**, **64-bit**, and **pointers**. This macro generates an **argument type array** at runtime and passes it to ``esp_log`` before the user arguments, ensuring that:

- The chip transmits data with the correct size and offset.
- The host tool reconstructs the log message accurately.

Runtime Behavior
""""""""""""""""

The ``esp_log`` function first checks if **binary logging** is enabled in the given configuration. If enabled, it extracts the **argument type array** from ``va_list``. However, if the binary log flag is **not** set, no preprocessed argument type array is available. In this case, the binary log handler **extracts argument types** from the format string at runtime.

This runtime extraction is less efficient than explicitly using ``ESP_LOG_ATTR_STR(format) ESP_LOG_ARGS(__VA_ARGS__)``, which generates the argument type array at compile time and removes the format string from flash. Nevertheless, this mechanism ensures that even if a **third-party library does not support binary logging**, logs will still be output correctly.

Special Handling for Buffer Logs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Binary logging supports buffer log functions such as:

- :c:macro:`ESP_LOG_BUFFER_HEX_LEVEL`
- :c:macro:`ESP_LOG_BUFFER_CHAR_LEVEL`
- :c:macro:`ESP_LOG_BUFFER_HEXDUMP`

In such cases, the binary log handler checks whether the format address matches any predefined constants (e.g., ``ESP_BUFFER_HEX_FORMAT``). If it does, the handler skips sending a format string and instead **transmits the raw buffer data directly**.

.. _host-side:

Host Side (Monitor Tool)
^^^^^^^^^^^^^^^^^^^^^^^^

On the **host side**, the `esp-idf-monitor tool <https://github.com/espressif/esp-idf-monitor>`_ decodes binary logs automatically. It is important that the monitor tool operates with the correct version of ELF files. The **bootloader** and **application** each have their own ELF files, which the tool automatically selects when ``idf.py monitor`` is calling.

If an **ELF address** is received, the monitor tool **retrieves the string from the ELF file** using the corresponding ``message type`` byte. If the **address starts with 0xFF** (range: ``0xFF - 0xFC``), it indicates an **embedded string**, with its length encoded in **10 bits**.

Once all components are retrieved, they are formatted and output to the terminal.

Performance and Measurements
----------------------------

When logging is used in a task, the task stack must be configured with at least 2 KB of space to ensure sufficient memory for logging operations.

The following measurements were performed using tests inside the log component with default settings (the maximum and default log levels were set to INFO, color support was disabled, without master log and timestamps were enabled) across different chips:

- Performance measurements for log APIs
- Stack usage for log APIs

``esp_rom_printf`` and ``esp_rom_vprintf`` produce similar results. Similarly, ``vprintf`` and ``printf`` yield comparable outcomes. Hence, only one of each pair is included in the tables below.

.. list-table:: Stack Usage (bytes)
   :header-rows: 1

   * - Function
     - ESP32
     - ESP32C2
     - ESP32C3
   * - esp_rom_printf
     - 128
     - 192
     - 192
   * - ESP_EARLY_LOGI V1
     - 128
     - 192
     - 192
   * - ESP_EARLY_LOGI V2
     - 336
     - 324
     - 324
   * - ESP_DRAM_LOGI V1
     - 128
     - 192
     - 192
   * - ESP_DRAM_LOGI V2
     - 336
     - 324
     - 324
   * - vprintf
     - 1168
     - 384
     - 1344
   * - ESP_LOGI V1
     - 1184
     - 384
     - 1344
   * - ESP_LOGI V2
     - 1152
     - 592
     - 1504

The stack usage differences between **Log V1** and **Log V2** are negligible.

.. list-table:: Performance (without output in microseconds)
   :header-rows: 1

   * - Function
     - ESP32
     - ESP32C2
     - ESP32C3
   * - esp_rom_printf
     - 1
     - 2
     - 1
   * - ESP_EARLY_LOGI V1
     - 15
     - 24
     - 14
   * - ESP_EARLY_LOGI V2
     - 28
     - 36
     - 25
   * - ESP_DRAM_LOGI V1
     - 6
     - 9
     - 5
   * - ESP_DRAM_LOGI V2
     - 19
     - 22
     - 14
   * - vprintf
     - 15
     - 9
     - 7
   * - ESP_LOGI V1
     - 27
     - 16
     - 12
   * - ESP_LOGI V2
     - 77
     - 54
     - 40

If logging to UART is measured, the performance numbers for **Log V1** and **Log V2** are nearly identical. The slight differences in processing overhead introduced by **Log V2** become negligible compared to the time it takes to send logs over UART. Thus, in most practical use cases, the performance impact of switching to **Log V2** will be unnoticeable.

**Memory Usage (bytes)**

The following measurements were performed using the ``esp_timer`` example with default settings for ESP32: the maximum and default log levels were set to INFO, color support was disabled, and timestamps were enabled. After enabling the **Log V2** option, the example was rebuilt, and the memory usage differences were compared using the command:

.. code-block:: bash

  idf.py size --diff ~/esp/logv2/build_v1

.. list-table::
   :header-rows: 1

   * - Version
     - IRAM
     - DRAM
     - Flash Code
     - Flash Data
     - App binary size
   * - Log V2
     - +1772
     - –36
     - –956
     - –1172
     - 181104 (–384)


.. list-table::
   :header-rows: 1
   :align: center

   * - Version
     - Bootloader binary size
   * - Log V2
     - 26272 (+160)

Enabling **Log V2** increases IRAM usage while reducing the overall application binary size, Flash code, and data usage.

Logging to Host via JTAG
------------------------

By default, the logging library uses the vprintf-like function to write formatted output to the dedicated UART. By calling a simple API, all log output may be routed to JTAG instead, making logging several times faster. For details, please refer to Section :ref:`app_trace-logging-to-host`.

Thread Safety
-------------

Logging from constrained environments (or for **ESP_EARLY_LOGx** and **ESP_DRAM_LOGx**) does not use locking mechanisms, which can lead to rare cases of log corruption if other tasks are logging in parallel. To minimize such risks, it is recommended to use general-purpose macros whenever possible.

General-purpose macros (**ESP_LOGx**) ensure thread safety by acquiring locks during log output. In **Log V2**, additional protection is provided by ``flockfile`` during multiple ``vprintf`` calls for formatting.

Logs are first written to a memory buffer before being sent to the UART, ensuring thread-safe operations across different tasks. Avoid logging from constrained environments unless necessary to maintain reliable log output.

Application Example
-------------------

The logging library is commonly used by most ESP-IDF components and examples. For demonstration of log functionality, check ESP-IDF's :idf:`examples` directory. The most relevant examples that deal with logging are the following:

* :example:`system/ota`
* :example:`storage/sd_card`
* :example:`protocols/https_request`

API Reference
-------------

.. include-build-file:: inc/esp_log.inc
.. include-build-file:: inc/esp_log_level.inc
.. include-build-file:: inc/esp_log_buffer.inc
.. include-build-file:: inc/esp_log_timestamp.inc
.. include-build-file:: inc/esp_log_color.inc
.. include-build-file:: inc/esp_log_write.inc
