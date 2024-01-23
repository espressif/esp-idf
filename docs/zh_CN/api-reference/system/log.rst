日志库
===============

:link_to_translation:`en:[English]`

概述
--------

日志库提供了三种设置日志级别的方式：

- **编译时**：在 menuconfig 中，使用选项 :ref:`CONFIG_LOG_DEFAULT_LEVEL` 来设置日志级别。
- 另外，还可以选择在 menuconfig 中使用选项 :ref:`CONFIG_LOG_MAXIMUM_LEVEL` 设置最高日志级别。这个选项默认被配置为默认级别，但这个选项也可以被配置为更高级别，将更多的可选日志编译到固件中。
- **运行时**：默认启用所有级别低于 :ref:`CONFIG_LOG_DEFAULT_LEVEL` 的日志。:cpp:func:`esp_log_level_set` 函数可以为各个模块分别设置不同的日志级别，可通过人类可读的 ASCII 零终止字符串标签来识别不同的模块。
- **运行时**：启用 :ref:`CONFIG_LOG_MASTER_LEVEL` 时，可以使用 :cpp:func:`esp_log_set_level_master` 函数设置 ``主日志级别`` (Master logging level)。该选项会为所有已编译的日志添加额外的日志级别检查。注意，使用此选项会增加应用程序大小。如果希望在运行时编译大量可选日志，同时避免在不需要日志输出时查找标签及其级别带来的性能损耗，此功能会非常有用。

以下是不同的日志级别：

- 错误（Error，最低级别）
- 警告 (Warning)
- 普通 (Info)
- 调试 (Debug)
- 冗余（Verbose，最高级别）

.. note::

    注意，函数 :cpp:func:`esp_log_level_set` 无法将日志级别设置为高于 :ref:`CONFIG_LOG_MAXIMUM_LEVEL` 指定的级别。如需在编译时将特定文件的日志级别提高到此最高级别以上，请使用 `LOG_LOCAL_LEVEL` 宏（详细信息见下文）。


如何使用日志库
-----------------------

在使用日志功能的所有 C 文件中，将 TAG 变量定义如下：

.. code-block:: c

    static const char* TAG = "MyModule";

然后使用一个日志宏进行输出，例如：

.. code-block:: c

    ESP_LOGW(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);

可使用下列宏来定义不同的日志级别：

* ``ESP_LOGE`` - 错误（最低级别）
* ``ESP_LOGW`` - 警告
* ``ESP_LOGI`` - 普通
* ``ESP_LOGD`` - 调试
* ``ESP_LOGV`` - 冗余（最高级别）

此外，上述宏还有对应的 ``ESP_EARLY_LOGx`` 版本，如 :c:macro:`ESP_EARLY_LOGE`。这些版本的宏必须在堆分配器和系统调用初始化之前，在早期启动代码中显式使用。通常情况下，编译引导加载程序时也可以使用普通的 ``ESP_LOGx`` 宏，但其最终实现与 ``ESP_EARLY_LOGx`` 宏相同。

上述宏还有对应的 ``ESP_DRAM_LOGx`` 版本，如 :c:macro:`ESP_DRAM_LOGE`。在禁用中断或无法访问 flash cache 的情况下需要输出日志时，可以使用这些版本的宏。但是，应尽量避免使用这些宏版本，因为在上述情况下输出日志可能会影响性能。

.. note::

    在关键部分中断被禁用，因此只能使用 ``ESP_DRAM_LOGx`` （首选）或 ``ESP_EARLY_LOGx`` 宏。尽管这样可以输出日志，但最好可以调整程序使其不用输出日志。

如需在文件或组件范围内覆盖默认的日志级别，请定义 ``LOG_LOCAL_LEVEL`` 宏。

在文件中，该宏应在包含 ``esp_log.h`` 文件前进行定义，例如：

.. code-block:: c

    #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
    #include "esp_log.h"

在组件中，该宏应在组件的 CMakeList 中进行定义：

.. code-block:: cmake

    target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")

如需在运行时按模块配置日志输出，请按如下方式调用 :cpp:func:`esp_log_level_set` 函数：

.. code-block:: c

   esp_log_level_set("*", ESP_LOG_ERROR);        // 将所有组件的日志级别设置为错误 (ERROR) 级别
   esp_log_level_set("wifi", ESP_LOG_WARN);      // 启用来自 WiFi 堆栈的警告 (WARN) 日志
   esp_log_level_set("dhcpc", ESP_LOG_INFO);     // 启用来自 DHCP 客户端的普通 (INFO) 日志

.. note::

    上文介绍的 "DRAM" 和 "EARLY" 日志宏变型不支持按照模块设置日志级别。这些宏始终以“默认”级别记录日志，且只能在运行时调用 ``esp_log_level("*", level)`` 对日志级别进行更改。

即使已通过标签名称禁用日志输出，每个条目仍需约 10.9 微秒的处理时间。

主日志级别
^^^^^^^^^^^^^^^^^^^^

要启用主日志级别功能，须启用 :ref:`CONFIG_LOG_MASTER_LEVEL` 选项。该功能在调用 :cpp:func:`esp_log_write` 之前为 ``ESP_LOGx`` 宏添加了额外的级别检查。这样就可以设置更高的 :ref:`CONFIG_LOG_MAXIMUM_LEVEL`，并且不会在正常操作期间对性能造成影响（仅在有指示时）。应用程序可以全局设置主日志级别（:cpp:func:`esp_log_set_level_master`）以强制执行最高日志级别。高于此级别的 ``ESP_LOGx`` 宏将直接跳过，不会调用 :cpp:func:`esp_log_write` 并进行标签查找。建议只在顶层应用程序中使用此功能，不要在共享组件中使用，因为这将覆盖所有使用该组件的用户的全局日志级别。默认情况下，启动时主日志级别是 :ref:`CONFIG_LOG_DEFAULT_LEVEL`。

注意，由于此功能为所有 ``ESP_LOGx`` 宏添加了额外的检查，会导致应用程序的大小增加。

以下代码片段展示了主日志级别的运行方式。将主日志级别设置为 ``ESP_LOG_NONE`` 将在全局范围内禁用所有日志记录。:cpp:func:`esp_log_level_set` 目前不会影响日志记录。但在主日志级别释放后，日志将按照 :cpp:func:`esp_log_level_set` 中的设置打印输出。

.. code-block:: c

    // 在启动时，主日志级别为 CONFIG_LOG_DEFAULT_LEVEL，并等于ESP_LOG_INFO
    ESP_LOGI("lib_name", "用于打印的消息");          // 打印普通 (INFO) 级别消息
    esp_log_level_set("lib_name", ESP_LOG_WARN);        // 启用 lib_name 的警告 (WARN) 日志

    esp_log_set_level_master(ESP_LOG_NONE);             // 全局禁用所有日志。esp_log_level_set 目前没有生效

    ESP_LOGW("lib_name", "用于打印的消息");          // 主日志级别阻止了打印
    esp_log_level_set("lib_name", ESP_LOG_INFO);        // 启用 lib_name 的 INFO 日志
    ESP_LOGI("lib_name", "用于打印的消息");          // 主日志级别阻止了打印

    esp_log_set_level_master(ESP_LOG_INFO);             // 全局启用所有 INFO 日志

    ESP_LOGI("lib_name", "用于打印的消息");          // 打印一条 INFO 消息

通过 JTAG 将日志记录到主机
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

默认情况下，日志库使用类似 vprintf 的函数将格式化输出写入专用 UART。通过调用一个简单的 API，即可将所有日志通过 JTAG 输出，将日志输出速度提高数倍。如需了解详情，请参阅 :ref:`app_trace-logging-to-host`。

线程安全
^^^^^^^^^^^^^

日志字符串首先被写入内存 buffer，然后发送到 UART 打印。日志调用是线程安全的，即不同线程的日志不会互相冲突。


应用示例
-------------------

大多数 ESP-IDF 组件和示例都会使用日志库。如需查看有关日志功能的应用示例，请前往 ESP-IDF 的 :idf:`examples` 目录。与日志最相关的示例如下：

* :example:`system/ota`
* :example:`storage/sd_card`
* :example:`protocols/https_request`

API 参考
-------------

.. include-build-file:: inc/esp_log.inc
