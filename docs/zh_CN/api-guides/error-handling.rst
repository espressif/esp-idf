.. highlight:: c

错误处理
========

:link_to_translation:`en:[English]`

概述
----

在应用程序开发中，及时发现并处理在运行时期的错误，对于保证应用程序的健壮性非常重要。常见的运行时错误有如下几种：

-  可恢复的错误：

  - 通过函数的返回值（错误码）表示的错误
  - 使用 ``throw`` 关键字抛出的 C++ 异常

-  不可恢复（严重）的错误：

  - 断言失败（使用 ``assert`` 宏或者其它类似方法，可参考 :ref:`assertions`）或者直接调用 ``abort()`` 函数造成的错误
  - CPU 异常：访问受保护的内存区域、非法指令等
  - 系统级检查：看门狗超时、缓存访问错误、堆栈溢出、堆栈粉碎、堆栈损坏等

本文主要介绍 ESP-IDF 中用于 **可恢复错误** 的处理机制，并提供一些常见错误的处理模式。部分章节也提及了用于 **不可恢复错误** 的相关宏，旨在说明其在不同错误严重程度下的应用场景。

关于如何处理不可恢复的错误，请参阅 :doc:`/api-guides/fatal-errors`。


错误码
------

ESP-IDF 中大多数函数会返回 :cpp:type:`esp_err_t` 类型的错误码，:cpp:type:`esp_err_t` 实质上是带符号的整型，``ESP_OK`` 代表成功（没有错误），具体值定义为 0。

在 ESP-IDF 中，许多头文件都会使用预处理器，定义可能出现的错误代码。这些错误代码通常均以 ``ESP_ERR_`` 前缀开头，一些常见错误（比如内存不足、超时、无效参数等）的错误代码则已经在 ``esp_err.h`` 文件中定义好了。此外，ESP-IDF 中的各种组件 (component) 也都可以针对具体情况，自行定义更多错误代码。

请参考 :doc:`错误代码参考 <../api-reference/error-codes>` 查看完整的错误列表。


错误码转换为错误消息
------------------------------

错误代码并不直观，因此 ESP-IDF 还可以使用 :cpp:func:`esp_err_to_name` 或者 :cpp:func:`esp_err_to_name_r` 函数，将错误代码转换为具体的错误消息。例如，我们可以向 :cpp:func:`esp_err_to_name` 函数传递错误代码 ``0x101``，可以得到返回字符串 ``ESP_ERR_NO_MEM``。这样一来，我们可以在日志中输出更加直观的错误消息，而不是简单的错误码，从而帮助研发人员更快理解发生了何种错误。

此外，如果出现找不到匹配的 ``ESP_ERR_`` 值的情况，函数 :cpp:func:`esp_err_to_name_r` 则会尝试将错误码作为一种 `标准 POSIX 错误代码 <https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/errno.h.html>`_ 进行解释。具体过程为：POSIX 错误代码（例如 ``ENOENT``， ``ENOMEM``）定义在 ``errno.h`` 文件中，可以通过 ``errno`` 变量获得，进而调用 ``strerror_r`` 函数实现。在 ESP-IDF 中，``errno`` 是一个基于线程的局部变量，即每个 FreeRTOS 任务都有自己的 ``errno`` 副本，通过函数修改 ``errno`` 也只会作用于当前任务中的 ``errno`` 变量值。

该功能（即在无法匹配 ``ESP_ERR_`` 值时，尝试用标准 POSIX 解释错误码）默认启用。用户也可以禁用该功能，从而减小应用程序的二进制文件大小，详情可见 :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP`。注意，该功能对禁用并不影响 :cpp:func:`esp_err_to_name` 和 :cpp:func:`esp_err_to_name_r` 函数的定义，用户仍可调用这两个函数转化错误码。在这种情况下， :cpp:func:`esp_err_to_name` 函数在遇到无法匹配错误码的情况会返回 ``UNKNOWN ERROR``，而 :cpp:func:`esp_err_to_name_r` 函数会返回 ``Unknown error 0xXXXX(YYYYY)``，其中 ``0xXXXX`` 和 ``YYYYY`` 分别代表错误代码的十六进制和十进制表示。

.. _esp-error-check-macro:

用于不可恢复错误的宏
----------------------

宏 :c:macro:`ESP_ERROR_CHECK` 用于处理 ESP-IDF 应用中不可恢复的错误，具体定义参考 ``esp_err.h`` 头文件。该宏的功能和标准的 ``assert`` 宏类似，不同之处在于它会专门检查 :cpp:type:`esp_err_t` 的值是否等于 :c:macro:`ESP_OK`。如果该值不为 :c:macro:`ESP_OK`，会将打印详细的错误信息，并调用 ``abort()`` 函数，从而终止程序运行。

使用下列断言配置选项，可以控制 :c:macro:`ESP_ERROR_CHECK` 宏的行为：

- 启用 ``CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_ENABLE`` （默认设置），该宏会打印错误信息并终止程序。
- 启用 ``CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT``，程序会在不打印任何错误信息的情况下直接终止。
- 启用 ``CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE`` （即定义了 ``NDEBUG``），该宏仅打印错误信息，但不会终止程序。

:c:macro:`ESP_ERROR_CHECK` 适用于严重错误且程序无法安全运行的情况。对于可以从错误中恢复的情况，请使用下一节中介绍的相关宏。

``ESP_ERROR_CHECK_WITHOUT_ABORT``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

宏 :c:macro:`ESP_ERROR_CHECK_WITHOUT_ABORT` 用于 **可恢复错误** 的处理， 具体定义见 ``esp_err.h`` 头文件。该宏的行为与 :c:macro:`ESP_ERROR_CHECK` 类似，但在 :cpp:type:`esp_err_t` 值不为 :c:macro:`ESP_OK` 时，不会调用 ``abort()`` 函数终止程序，而是用同样的格式打印错误信息，并返回该错误码，允许程序继续运行。该宏适用于需要报告错误，但无需中断程序的情况。

:c:macro:`ESP_ERROR_CHECK_WITHOUT_ABORT` 宏的行为也受断言相关配置选项（与 :c:macro:`ESP_ERROR_CHECK` 相同）的控制。启用 ``CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE`` 或 ``CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_SILENT`` 时，该宏不打印任何错误信息；否则打印错误信息。

当需要打印错误信息用于诊断，但希望应用程序继续运行时，请使用宏 :c:macro:`ESP_ERROR_CHECK_WITHOUT_ABORT`。

错误消息通常如下所示：

.. code-block:: none

    ESP_ERROR_CHECK failed: esp_err_t 0x107 (ESP_ERR_TIMEOUT) at 0x400d1fdf

    file: "/Users/user/esp/example/main/main.c" line 20
    func: app_main
    expression: sdmmc_card_init(host, &card)

    Backtrace: 0x40086e7c:0x3ffb4ff0 0x40087328:0x3ffb5010 0x400d1fdf:0x3ffb5030 0x400d0816:0x3ffb5050

.. note::

    如果使用 :doc:`tools/idf-monitor`，则最后一行回溯结果中的地址将会被自动解析为相应的文件名和行号。

- 第一行打印错误代码的十六进制表示，及该错误在源代码中的标识符。这个标识符取决于 :ref:`CONFIG_ESP_ERR_TO_NAME_LOOKUP` 选项的设定。最后，第一行还会打印程序中该错误发生的具体位置。

- 下面几行显示了程序中调用 :c:macro:`ESP_ERROR_CHECK` 宏的具体位置，以及传递给该宏的参数。

- 最后一行打印回溯结果。对于所有不可恢复错误，这里在应急处理程序中打印的内容都是一样的。更多有关回溯结果的详细信息，请参阅 :doc:`/api-guides/fatal-errors`。


用于可恢复错误的宏
----------------------

ESP-IDF 提供了一组宏来处理可恢复的错误，定义在 ``esp_check.h`` 头文件中。 **ESP_RETURN_ON_...**、 **ESP_GOTO_ON_...** 和 **ESP_RETURN_VOID_ON_...** 系列宏可简洁、一致地处理错误，提升代码的可读性与可维护性。与 ``ESP_ERROR_CHECK`` 不同，这些宏不会终止程序，而是在检测到错误时打印错误信息并执行返回或跳转。针对中断服务例程 (ISR) 场景，还提供了相应的 ``_ISR`` 版本（如 :c:macro:`ESP_RETURN_ON_ERROR_ISR`），确保中断上下文的安全性。

这些宏的定义如下：

- **ESP_RETURN_ON_...**：当检测到错误或条件失败时，从函数返回：

    - :c:macro:`ESP_RETURN_ON_ERROR` - 检查错误码，如果不是 :c:macro:`ESP_OK`，打印信息并返回该错误码。
    - :c:macro:`ESP_RETURN_ON_FALSE` - 检查某个条件，如果为假，打印信息并返回提供的 ``err_code``。
    - :c:macro:`ESP_RETURN_ON_ERROR_ISR` - 适用于中断服务例程 (ISR) 上下文。
    - :c:macro:`ESP_RETURN_ON_FALSE_ISR` - 适用于中断服务例程 (ISR) 上下文。

- **ESP_GOTO_ON_...**：当检测到错误或条件失败时，跳转到指定标签：

    - :c:macro:`ESP_GOTO_ON_ERROR` - 检查错误码，如果不是 :c:macro:`ESP_OK`，打印信息，将 ``ret`` 设为错误码，并跳转到 ``goto_tag``。
    - :c:macro:`ESP_GOTO_ON_FALSE` - 检查某个条件，如果为假，打印信息，将 ``ret`` 设为错误码，并跳转到 ``goto_tag``。
    - :c:macro:`ESP_GOTO_ON_ERROR_ISR` - 适用于中断服务例程 (ISR) 上下文。
    - :c:macro:`ESP_GOTO_ON_FALSE_ISR` - 适用于中断服务例程 (ISR) 上下文。

- **ESP_RETURN_VOID_...**：当检测到错误或条件失败时，从 ``void`` 函数返回：

    - :c:macro:`ESP_RETURN_VOID_ON_ERROR` - 检查错误码，如果不是 :c:macro:`ESP_OK`，打印信息并返回。
    - :c:macro:`ESP_RETURN_VOID_ON_FALSE` - 检查某个条件，如果为假，打印信息并返回。
    - :c:macro:`ESP_RETURN_VOID_ON_ERROR_ISR` - 适用于中断服务例程 (ISR) 上下文。
    - :c:macro:`ESP_RETURN_VOID_ON_FALSE_ISR` - 适用于中断服务例程 (ISR) 上下文。

这些宏的默认行为可通过 Kconfig 进行配置：如果启用了 :ref:`CONFIG_COMPILER_OPTIMIZATION_CHECKS_SILENT` 选项，错误信息将不会被包含在应用程序二进制文件中，也不会被打印出来。

.. _check_macros_examples:

错误处理示例
-----------------

示例：

.. code-block:: c

    static const char* TAG = "Test";

    esp_err_t test_func(void)
    {
        esp_err_t ret = ESP_OK;

        ESP_ERROR_CHECK(x);                                         // 如果错误码不等于 `ESP_OK`，则打印错误信息，然后调用 `abort()`。
        ESP_ERROR_CHECK_WITHOUT_ABORT(x);                           // 如果错误码不等于 `ESP_OK`，则打印错误信息，不调用 `abort()`。
        ESP_RETURN_ON_ERROR(x, TAG, "fail reason 1");               // 如果错误码不等于 `ESP_OK`，则打印错误信息，并使原函数立刻返回，返回值为给定的错误码.
        ESP_GOTO_ON_ERROR(x, err, TAG, "fail reason 2");            // 如果错误码不等于 `ESP_OK`，则打印错误信息，将局部变量 `ret` 赋值为该错误码，并使原函数跳转至 `err`。
        ESP_RETURN_ON_FALSE(a, err_code, TAG, "fail reason 3");     // 如果给定条件不等于 `true`，则打印错误信息，并使原函数立刻返回，返回值为给定的错误码。
        ESP_GOTO_ON_FALSE(a, err_code, err, TAG, "fail reason 4");  // 如果给定条件不等于 `true`，该宏会打印错误信息，将局部变量 `ret` 赋值为给定的 `err_code`，并使原函数跳转至 `err`。

    err:
        // clean up
        return ret;
    }

错误处理模式
------------

1. 尝试恢复。根据具体情况不同，我们具体可以：

    - 在一段时间后，重新调用该函数；
    - 尝试删除该驱动，然后重新进行“初始化”；
    - 采用其他带外机制，修改导致错误发生的条件（例如，对一直没有响应的外设进行复位等）。

    示例：

    .. code-block:: c

        esp_err_t err;
        do {
            err = sdio_slave_send_queue(addr, len, arg, timeout);
            // 如果发送队列已满就不断重试
        } while (err == ESP_ERR_TIMEOUT);
        if (err != ESP_OK) {
            // 处理其他错误
        }

2. 将错误传递回调用程序。在某些中间件组件中，采用此类处理模式代表函数必须以相同的错误码退出，这样才能确保所有分配的资源都能得到释放。

    示例：

    .. code-block:: c

        sdmmc_card_t* card = calloc(1, sizeof(sdmmc_card_t));
        if (card == NULL) {
            return ESP_ERR_NO_MEM;
        }
        esp_err_t err = sdmmc_card_init(host, &card);
        if (err != ESP_OK) {
            // 释放内存
            free(card);
            // 将错误码传递给上层（例如通知用户）
            // 或者，应用程序可以自定义错误代码并返回
            return err;
        }

3. 转为不可恢复错误，比如使用 ``ESP_ERROR_CHECK``。详情请见 `用于不可恢复错误的宏 <#esp-error-check-macro>`_ 章节。

    对于中间件组件而言，通常并不希望在发生错误时中止应用程序。不过，有时在应用程序级别，这种做法是可以接受的。

    在 ESP-IDF 的示例代码中，很多都会使用 ``ESP_ERROR_CHECK`` 来处理各种 API 引发的错误，虽然这不是应用程序的最佳做法，但可以让示例代码看起来更加简洁。

    示例：

    .. code-block:: c

        ESP_ERROR_CHECK(spi_bus_initialize(host, bus_config, dma_chan));


C++ 异常
--------

请参考 :ref:`cplusplus_exceptions`。

API 参考
-------------

请参考 :ref:`esp-check-api-ref`。
