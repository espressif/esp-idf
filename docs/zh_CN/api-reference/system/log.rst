日志库
===============

:link_to_translation:`en:[English]`

概述
--------

ESP-IDF 提供了一套灵活的日志系统，包括两个可配置版本 **Log V1** 和 **Log V2**，可通过 :ref:`CONFIG_LOG_VERSION` 参数进行选择。本文档概述了这两个日志系统版本的特性、配置及使用方法，并比较了二者的性能表现。

- **Log V1**：默认的原始实现方式，具备简洁性，针对早期日志和 DRAM 日志进行了优化，但 flash 占用较高，缺乏灵活性。
- **Log V2**：增强的实现方式，更加灵活，降低了 flash 占用，并集中处理日志格式，但需要更多的堆栈。

**Log V2** 向后兼容 **Log V1**，这意味着使用 **Log V1** 编写的项目可以直接切换到 **Log V2**，无需额外修改。但是，由于兼容性限制，使用 **Log V2** 特定功能的项目不能恢复到 **Log V1**。

**Log V1** 的特性
^^^^^^^^^^^^^^^^^^^^^^

- 日志格式由 ``format`` 参数定义，在编译时嵌入了 flash 中。
- 相比 ESP_LOG，能更快记录早期日志和 DRAM 日志。
- 实现简单，但具有局限性：

  - 由于包含冗余的格式化信息，二进制文件体积较大。
  - 不支持自定义日志格式，缺乏灵活性。
  - 编译错误所指向的宏中的参数位置编号不准确。

**Log V2** 的特性
^^^^^^^^^^^^^^^^^^^^^^

- 通过单个函数 :cpp:func:`esp_log` 集中处理格式。
- 仅存储用户定义的格式字符串，从而减小二进制文件大小。
- 仅在输出需要且日志级别允许记录时，才会获取时间戳。
- 允许自定义日志输出：

  - 为某个层级（全局、文件或日志消息）启用或禁用颜色、时间戳或标签。
  - 输出不经过格式化处理的原始日志（适用于二进制日志）。
  - 为引导加载程序和应用程序采用不同的日志设置。

- 格式参数可以动态设置为变量，构建日志消息更灵活。
- 在引导加载程序、ISR、启动代码和受限环境中，日志处理机制保持统一。
- 缺点：

  - 消耗更多的栈和内存。
  - 日志处理速度比 **Log V1** 略慢，但与传输数据的时间相比（例如通过 UART），差异可以忽略不计。

日志级别
----------

对于应用程序和引导加载程序，日志级别需要分别配置。开发者可以通过 Kconfig 选项为每个模块设置不同的日志级别，从而实现配置的独立性。例如，可以为引导加载程序启用简洁的日志，而为应用程序启用详细的调试日志。使用引导加载程序专用的 Kconfig 选项，可以为引导加载程序独立配置日志级别，不会影响主应用程序。

日志库共有六个详细程度级别：

- **Verbose** - 输出高度详细且频繁的调试信息，通常包括内部状态，可能会使输出过于繁杂。（最高级别）
- **Debug** - 输出详细的诊断信息（例如变量值、指针地址等），适用于调试。
- **Info** - 输出描述系统正常运行的一般信息。
- **Warning** - 输出可能引发问题，但已被处理或减轻影响的事件。
- **Error** - 仅输出严重错误，这些错误如果不进行干预处理，软件无法自行恢复。
- **None** - 无日志输出，即完全禁用日志。（最低级别）

日志级别设置
------------------

通过日志级别设置，可以选择将哪些日志包含在二进制文件中，并决定这些日志在运行时的可见性。日志级别设置包括以下两种：

- **日志级别**：指定在运行时显示哪些级别的日志。引导加载程序的 **日志级别** 通过 :ref:`CONFIG_BOOTLOADER_LOG_LEVEL` 配置，而应用程序的 **日志级别** 通过 :ref:`CONFIG_LOG_DEFAULT_LEVEL` 设置。通过函数 ``esp_log_get_default_level`` 能够获取当前日志级别。

- **最高日志级别**：指定将哪些日志级别包含在二进制文件中。高于此级别的日志会在编译时丢弃，不包含在最终镜像中。对于应用程序，**最高日志级别** 可以设置得高于 **日志级别**，从而在二进制文件中包含额外的日志，必要时，便可通过 :cpp:func:`esp_log_level_set` 启用这些日志以帮助调试。使用 :ref:`CONFIG_LOG_MAXIMUM_LEVEL` 选项可以为应用程序启用此功能。引导加载程序不支持此功能，其 **最高日志级别** 始终与 **日志级别** 相同。

例如，如果将 **日志级别** 设置为 **Warning**，**最高日志级别** 设置为 **Debug**，则二进制文件会包含 **Error**、**Warning**、**Info** 和 **Debug** 级别的日志。然而，在运行时仅输出 **Error** 和 **Warning** 级别的日志，除非通过 :cpp:func:`esp_log_level_set` 显式更改日志级别。根据具体需求，日志级别可以提高或降低。

设置 ``最高日志级别``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

根据 ``LOG_LOCAL_LEVEL`` 的定义，可使用此参数覆盖特定源文件或组件的 **最高日志级别**，而无需修改 Kconfig 选项。此参数能设置一个本地的 **最高日志级别**，从而启用或排除二进制文件中的特定日志。

通过此方法，能够有效为代码的特定部分提供更详细的日志，而无需全局提高 **最高日志级别**，避免了对二进制文件大小产生不必要的影响。

- 更改某个源文件的 **最高日志级别** （不要在头文件中添加该定义，因为头文件采用单次包含的机制，可能无法生效）：在包含 ``esp_log.h`` 之前，使用 :cpp:type:`esp_log_level_t` 中的一个值来定义 ``LOG_LOCAL_LEVEL``，指定将哪些日志消息包含在该源文件的二进制文件中。

    .. code-block:: c

        // 在某个 my_file.c 文件中
        #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
        #include "esp_log.h"

- 更改整个组件的 **最高日志级别**：在组件的 `CMakeLists.txt` 文件中定义 ``LOG_LOCAL_LEVEL``。这确保指定的日志级别适用于组件内的所有源文件，指定将哪些日志消息包含在二进制文件中：

    .. code-block:: cmake

        # 在组件的 CMakeLists.txt 文件中
        target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")

运行时更改 ``日志级别``
^^^^^^^^^^^^^^^^^^^^^^^^^

仅应用程序支持在运行时更改日志级别，启动引导加载程序不支持此功能。

默认情况下，系统启动时会启用 **日志级别** 以下的所有日志级别。可以使用函数 :cpp:func:`esp_log_level_set` 全局或按模块设置 **日志级别**。模块可通过标签识别，这些标签是人类可读以零结尾的 ASCII 字符串。此功能依赖于 :ref:`CONFIG_LOG_DYNAMIC_LEVEL_CONTROL`，此选项默认启用。如无需此功能，可以将其禁用，以减少代码量并提升性能。

例如，将所有组件的日志级别设置为 ``ERROR`` （全局设置）：

.. code-block:: c

   esp_log_level_set("*", ESP_LOG_ERROR);

根据模块（标签）调整日志输出的功能依赖于 :ref:`CONFIG_LOG_TAG_LEVEL_IMPL`，该选项默认启用。如不需要此功能，可以将其禁用，以减少代码量并提升性能。

例如，仅将 Wi-Fi 组件的日志级别设置为 ``WARNING`` （特定模块设置）：

.. code-block:: c

   esp_log_level_set("wifi", ESP_LOG_WARN);

使用日志库
---------------

在每个使用日志功能的 C 文件中定义 ``TAG`` 变量。

.. code-block:: c

    // #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE // 可选：增加包含在二进制文件中的日志级别（仅适用于本文件）
    #include "esp_log.h"
    static const char* TAG = "MyModule";
    // ...
    ESP_LOGI(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);
    ESP_EARLY_LOGW(TAG, "Early log message %d", i++);
    ESP_DRAM_LOGE(DRAM_STR("TAG_IN_DRAM"), "DRAM log message %d", i++); // 如果需要，使用 DRAM_STR 宏添加 DRAM

.. code-block:: bash

    I (112500) MyModule: Baud rate error 1.5%. Requested: 115200 baud, actual: 116928 baud
    W (112500) MyModule: Early log message 1
    E TAG_IN_DRAM: DRAM log message 2

.. note::

    ``TAG`` 变量指向存储在 flash 中的一个字符串字面量。如果在单个构建单元（翻译单元）中多次使用相同的 ``TAG`` 字符串，编译器和链接器通常会通过 **字符串池化** 过程将其优化为 flash 中的单个副本。然而，如果不同的组件或翻译单元使用了相同的 ``TAG`` 字符串，每个组件或单元在 flash 中都会存储一个副本，除非应用了全局链接器优化。

日志库提供了多种宏以适应不同的使用场景，例如通用日志记录、早期启动日志记录和受限环境日志等，如下所示。选择合适的宏并据此构建相应的程序结构，有助于优化性能，确保可靠运行。但是，建议在设计程序结构时尽量避免在受限环境中进行日志记录。

- Verbose: :c:macro:`ESP_LOGV`, :c:macro:`ESP_EARLY_LOGV`, :c:macro:`ESP_DRAM_LOGV`.
- Debug: :c:macro:`ESP_LOGD`, :c:macro:`ESP_EARLY_LOGD`, :c:macro:`ESP_DRAM_LOGD`.
- Info: :c:macro:`ESP_LOGI`, :c:macro:`ESP_EARLY_LOGI`, :c:macro:`ESP_DRAM_LOGI`.
- Warning: :c:macro:`ESP_LOGW`, :c:macro:`ESP_EARLY_LOGW`, :c:macro:`ESP_DRAM_LOGW`.
- Error: :c:macro:`ESP_LOGE`, :c:macro:`ESP_EARLY_LOGE`, :c:macro:`ESP_DRAM_LOGE`.

这些宏可分为以下三组：

- **ESP_LOGx**: 标准日志宏，适用于正常运行期间的大多数用例。在非受限环境下，可在应用程序代码中使用这些宏来记录日志，但不要在中断服务例程 (ISR)、早期启动阶段或 flash 缓存被禁用时使用。这些宏的一个重要特点是，它们使用 Newlib 库的 `vprintf` 函数进行格式处理和日志输出。

- **ESP_EARLY_LOGx**: 专为早期启动阶段的受限环境设计，在堆分配器或系统调用尚未初始化时使用。这些宏通常用于关键的启动代码或中断被禁用的关键区域。这些宏的一个重要特点是，它们使用 ROM 的 `printf` 函数，以微秒为单位输出时间戳，并且不支持按模块设置日志详细级别。

- **ESP_DRAM_LOGx**: 专为受限环境设计，在中断被禁用或 flash 缓存不可访问时记录日志。这些宏可能会影响性能，应谨慎使用。这些宏适用于其他日志宏可能无法可靠运行的关键区域或中断例程。这些宏的特点是，它们使用 ROM 的 `printf` 函数，不输出时间戳，将格式参数分配在 DRAM 中以确保缓存禁用时的可访问性，并且不支持按模块设置日志详细级别。

.. Note::
    使用 **DRAM_STR("my_tag")** 宏在 DRAM 中分配标签。这能够确保在 flash 缓存被禁用时仍能访问标签。

**Log V1** 和 **Log V2** 的区别在于，在 **Log V2** 中，所有来自这些宏的日志都发送到同一个处理程序进行处理。该处理程序可以自动检测受限环境（例如，早期启动、禁用中断或 flash 缓存不可访问的情景），并动态选择适当的打印函数，确保在不同的运行环境中实现高效的日志记录。

日志格式
----------

- **Log V1**：仅支持全局禁用颜色格式。其他格式选项（如时间戳和标签）始终启用。

- **Log V2**：

  - 允许完全自定义日志格式，包括全局、按文件、按模块、为单个日志消息禁用颜色、标签和时间戳格式。
  - 更精细的日志输出控制，更适用于特定的用例和环境。

.. code-block:: c

    // #define ESP_LOG_COLOR_DISABLED     (1)  /* 仅用于 Log v2 */
    // #define ESP_LOG_TIMESTAMP_DISABLED (1)  /* 仅用于 Log v2 */
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

日志系统支持以下格式选项，并且同时适用于应用程序和引导加载程序：

- **Color**：增加颜色代码，全局增强日志的可见性。由 :ref:`CONFIG_LOG_COLORS` 控制，默认情况下禁用，因为 ESP-IDF 监视工具 `idf.py monitor` 可以通过 **级别名称** 检测日志级别并应用标准的 IDF 颜色方案。

  - 对于 **Log V2**，选项 :ref:`CONFIG_LOG_COLORS_SUPPORT` 支持在运行时为特定日志、文件或组件添加颜色输出，即使全局颜色已禁用。此时要为特定上下文启用颜色，请使用 ``ESP_LOG_COLOR_DISABLED``。

- **Level Name**：表示日志详细级别的单个字母（I, W, E, D, V），显示在每条日志消息的开头，用于识别日志级别。这在禁用颜色时非常有用，例如在禁用颜色时 ESP-IDF 监视工具就会使用该信息。

- **Timestamp**：为日志消息全局添加时间戳。由 :ref:`CONFIG_LOG_TIMESTAMP_SOURCE` 控制。

  - **None**：不显示时间戳。在日志分析或调试中，当时间不关键时非常有用，还能够节省处理性能和内存。仅适用于 **Log V2**。
  - **Milliseconds since boot** `(18532)` （默认）：通过 RTOS 时钟 tick 计数乘以 tick 周期得出。
  - **System time (HH:MM:SS.sss)** `14:31:18.532`：以小时、分钟、秒和毫秒显示时间。
  - **System time (YY-MM-DD HH:MM:SS.sss)** `(2023-08-15 14:31:18.532)`：同上，还包括日期。
  - **Unix time in milliseconds** `(1692099078532)`：以毫秒显示 Unix 时间。
  - 对于 **Log V2**，选项 :ref:`CONFIG_LOG_TIMESTAMP_SUPPORT` 支持在运行时为特定日志、文件或组件添加时间戳输出，即使全局时间戳已禁用。要为特定上下文启用 **Milliseconds since boot** 时间戳，请使用 ``ESP_LOG_TIMESTAMP_DISABLED``。

- **Tag**：显示用户定义的源模块标识符。

  - 对于 **Log V2**，可以将 tag 设置为 ``NULL`` 传递给宏，在这种情况下，tag 不会被打印，且无法按组件进行日志级别检查。

- **End Line**：在日志消息的末尾添加换行符。

以下选项仅适用于 **Log V2**，并与提供的日志宏一起使用。这些定义可以用和 ``LOG_LOCAL_LEVEL`` 相同的方式设置。它们的作用范围取决于定义的位置（例如文件、组件或全局）：

- **ESP_LOG_CONSTRAINED_ENV**：

  - 定义为 ``1`` 时，强制日志处理程序 :cpp:func:`esp_log` 使用适合指定作用域的安全 printf 函数。

- **ESP_LOG_FORMATTING_DISABLED**:

  - 默认为 ``0``，即启用所有格式化项，如颜色、时间戳、标记和末尾换行。
  - 定义为 ``1`` 时，为指定范围禁用所有的格式化项。

- **ESP_LOG_COLOR_DISABLED**：要求启用 :ref:`CONFIG_LOG_COLORS_SUPPORT`。

  - 如果全局颜色 (:ref:`CONFIG_LOG_COLORS`) 已禁用，则定义为 ``0``，以启用指定范围的颜色输出。
  - 如果启用了全局颜色 (:ref:`CONFIG_LOG_COLORS`)，则定义为 ``1``，表示禁用指定范围的颜色输出。

- **ESP_LOG_TIMESTAMP_DISABLED**：要求启用 :ref:`CONFIG_LOG_TIMESTAMP_SUPPORT`。

  - 如果已禁用全局时间戳（:ref:`CONFIG_LOG_TIMESTAMP_SOURCE`），则定义为 ``0``，以启用指定范围的时间戳输出。
  - 如果全局时间戳（:ref:`CONFIG_LOG_TIMESTAMP_SOURCE`）已启用，则定义为 ``1``，表示禁用指定范围的时间戳输出。

- **ESP_LOG_MODE_BINARY_EN**：要求启用 ``CONFIG_LOG_MODE_BINARY`` 或 ``CONFIG_BOOTLOADER_LOG_MODE_BINARY`` 配置项。

  - 将 ``ESP_LOG_MODE_BINARY_EN`` 设置为 ``0`` 在常规使用中意义不大，因为日志仍会以二进制模式输出。但格式字符串不会从 flash 中移除，且运行时仍会进行参数分析。此设置可用于调试或测试等特定场景。
  - 启用文本日志模式时，``ESP_LOG_MODE_BINARY_EN`` 会被忽略并自动定义为 ``0``，即便手动将其设置为 ``1`` 也是无效的，

设置每条日志的输出格式
^^^^^^^^^^^^^^^^^^^^^^^^^^^

上述定义可以与提供的日志宏无缝配合使用。如果需要更高的灵活性，或需要在运行时调整设置，例如根据某个值（例如温度）调整日志级别，可以使用其他的宏来实现。需要注意的是，在这种情况下，日志不能从二进制文件中丢弃，因为它们绕过了编译时的日志级别检查。

下面的示例演示了如何调整单个日志消息的格式：

.. code-block:: c

    #include "esp_log.h"
    esp_log_config_t configs = {
        .opts = {
            .log_level = ESP_LOG_INFO,                 // 设置 log level
            .constrained_env = false,                  // 指定是否为受限环境
            .require_formatting = true,                // 启用格式处理
            .dis_color = ESP_LOG_COLOR_DISABLED,       // 使用全局颜色设置
            .dis_timestamp = ESP_LOG_TIMESTAMP_DISABLED, // 使用全局时间戳设置
            .reserved = 0,                             // 保留后续使用
        }
    };
    // ...
    if (temperature > 55) {
        configs.opts.log_level = ESP_LOG_WARN;
    }
    //与 ESP_LOGx 宏相似，但可以采用自定义配置
    // 如果 configs 变量为常量，编译器在编译过程中会排除低于 maximum log level 的日志
    //如果 configs 不是常量则不适用
    ESP_LOG_LEVEL_LOCAL(configs, TAG, "Temp = %dC", temperature);

    // // 注意：以下调用绕过了编译时日志级别检查
    // 这些日志无法从二进制文件中丢弃
    esp_log(configs, TAG, "Temp = %dC", temperature);
    ESP_LOG_LEVEL(configs, TAG, "Temp = %dC", temperature);

日志级别控制
-----------------

只有应用程序支持在运行时更改日志级别。引导加载程序不支持此功能。

日志库允许在运行时使用函数 :cpp:func:`esp_log_level_set` 调整每个模块（标签）的日志输出。此功能仅适用于非受限环境（**ESP_LOGx** 宏）。受限环境（如 **ESP_EARLY_LOGx** 或 **ESP_DRAM_LOGx**）不支持动态日志级别，因为它们的日志处理程序中没有锁和轻量级要求。

.. code-block:: c

   // 将所有组件的日志级别设置为ERROR（全局设置）
   esp_log_level_set("*", ESP_LOG_ERROR);

   // 将 Wi-Fi 组件的日志级别设置为 WARNING（特定模块设置）
   esp_log_level_set("wifi", ESP_LOG_WARN);

   // 将 DHCP 客户端的日志级别设置为 INFO（模块相关设置）
   esp_log_level_set("dhcpc", ESP_LOG_INFO);

下列三种设置可在运行时全局更改日志级别，或为单个模块（标签）更改日志级别：

- **Dynamic Log Level Control** （:ref:`CONFIG_LOG_DYNAMIC_LEVEL_CONTROL`，默认已启用）：动态日志级别控制。启用后，可以通过 :cpp:func:`esp_log_level_set` 函数在运行时更改日志级别。该功能提高了灵活性，但也增加了内存和性能开销。如需考虑二进制文件的大小，并且无需在运行时动态更改日志级别，建议禁用此选项，特别是在 :ref:`CONFIG_LOG_TAG_LEVEL_IMPL` 设置为 **None** 时，以尽量减小程序大小。

  如果你的应用程序不需要动态调整日志级别，禁用此选项可以提高效率：

  - 降低内存消耗：

    - **IRAM**: 约 260 bytes
    - **DRAM**: 约 264 bytes
    - **Flash**: 约 1 KB

  - 提高日志操作性能，最多提高 10 倍。

- **Tag-Level Checks** （:ref:`CONFIG_LOG_TAG_LEVEL_IMPL`，默认值为 **Cache + Linked List**）：标签级别检查，决定了如何检查每个标签的日志级别，影响内存使用和查找速度：

  - **None**：完全禁用按标签进行日志级别检查，能够减少开销，但失去了运行时的灵活性。

  - **Linked List**：仅使用链表实现按标签设置日志级别（不使用缓存）。这种方法会遍历链表中的所有标签来确定日志级别，因此当标签数量较大时，会导致查找速度变慢，但与 **Cache** 方式相比，能节省更多内存空间。链表方法对日志标签进行完整的字符串比较，从而识别日志级别。与 **Cache** 方法不同，链表方法不依赖于标签指针比较，因此更适用于动态的标签定义。如需优先考虑节省内存、对特定模块启用或禁用日志，或希望使用定义为变量的标签，请选择此方法。选择此方法会自动启用 **Dynamic Log Level Control** （动态日志级别控制）功能。运行 ``ESP_LOGx`` 宏遇到新标签时，链表中的项会分配到堆栈上。

- **Cache + Linked List** （默认）：缓存 + 链表，通过缓存与链表结合的方式进行日志标签级别检查，实现了内存占用和运行速度之间的平衡。缓存用于存储最近访问的日志标签及其对应的日志级别，加速了常用标签的查找。这是因为缓存方式会比较标签指针，与执行完整字符串相比速度更快。对不常用标签，通过链表进行日志级别查找。注意，使用动态标签定义时，此选项可能无法正常工作，因为它依赖缓存中的标签指针比较，不适用于动态定义的标签。此混合方法利用了常用标签的缓存速度优势和不常用标签的链表存储效率，提升了日志级别查找的总体效率。选择此选项会自动启用 **Dynamic Log Level Control**。

有一些缓存配置可以平衡内存使用和查找性能。这些配置决定了日志标签级别的存储和访问方式，详见 :ref:`CONFIG_LOG_TAG_LEVEL_CACHE_IMPL`。

    - **Array**：数组方式，实现简单，不进行重新排序，适合注重简洁性的低内存应用。

    - **Binary Min-Heap** （默认配置）最小二叉堆，优化的实现方式，支持快速查找并自动重新排序，适用于具有充足内存的高性能应用。其容量由 **缓存大小** (:ref:`CONFIG_LOG_TAG_LEVEL_IMPL_CACHE_SIZE`) 定义，默认包含 31 个条目。

    缓存容量越大，查找常用日志标签的性能越高，但内存消耗也会增加。相反，缓存容量越小越节省内存，但可能导致不常用的日志标签被更频繁地移除。

- **Master Log Level** （:ref:`CONFIG_LOG_MASTER_LEVEL`，默认禁用）：这是一个可选设置，专为特定调试场景设计。此设置启用后，会在生成时间戳和标签缓存查找之前，启用全局 master 日志级别检查。这一选项适用于编译大量日志的情况，可以在运行时有选择地启用或禁用日志，同时在不需要日志输出时尽量减少对性能的影响。

  例如，通常可以在在时间紧迫或 CPU 密集型操作期间临时禁用日志，并在之后重新启用日志。

  .. note:: 对于 **Log V1**，此功能可能会基于已编译日志的数量而显著增加程序大小。对于 **Log V2** 影响很小，因为检查已集成到了日志处理程序中。

  如果启用此功能，master 日志级别默认为 :ref:`CONFIG_LOG_DEFAULT_LEVEL`，并可在运行时通过 :cpp:func:`esp_log_set_level_master` 进行调整。此全局检查优先于 ``esp_log_get_default_level``。

  以下代码片段演示了此功能的原理。将 **Master Log Level** 设置为 ``ESP_LOG_NONE``，会在全局范围内禁用所有日志。此时，:cpp:func:`esp_log_level_set` 不会影响日志输出。但是，当 **Master Log Level** 调整为更高级别后，日志会按照 :cpp:func:`esp_log_level_set` 的配置打印出来：

  .. code-block:: c

      // master 日志级别在启动时为 CONFIG_LOG_DEFAULT_LEVEL, 且等于 ESP_LOG_INFO
      ESP_LOGI("lib_name", "Message for print");          // 打印 INFO 消息
      esp_log_level_set("lib_name", ESP_LOG_WARN);        // 为 lib_name 启用 WARN 级别日志

      // 全局禁用所有日志，esp_log_level_set 目前没有作用
      esp_log_set_level_master(ESP_LOG_NONE);

      ESP_LOGW("lib_name", "Message for print");          // master 日志级别阻止了打印
      esp_log_level_set("lib_name", ESP_LOG_INFO);        // 开启 lib_name 的 INFO 日志
      ESP_LOGI("lib_name", "Message for print");          // master 日志级别阻止了打印

      // 全局启用所有 INFO 日志
      esp_log_set_level_master(ESP_LOG_INFO);

      ESP_LOGI("lib_name", "Message for print");          // 打印 INFO 信息

.. note::

    即使按标签禁用日志，处理时间仍需约 10.9 微秒。要减少这一开销，可考虑使用 **Master Log Level** 或禁用 **Tag-Level Checks** 功能。

缓冲区日志
----------

日志系统提供用于记录缓冲区数据的宏。这些宏可在引导加载程序和应用程序中使用，且不限制日志版本。可用的宏有：

- :c:macro:`ESP_LOG_BUFFER_HEX` 和 :c:macro:`ESP_LOG_BUFFER_HEX_LEVEL`：记录十六进制字节缓冲区。数据按每行 16 个字节分割。:c:macro:`ESP_LOG_BUFFER_HEX` 仅适用于 ``Info`` 日志级别。

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

- :c:macro:`ESP_LOG_BUFFER_CHAR` 和 :c:macro:`ESP_LOG_BUFFER_CHAR_LEVEL`：记录可打印字符的缓冲区。每行最多包含 16 个字符。:c:macro:`ESP_LOG_BUFFER_CHAR` 仅适用于 ``Info`` 日志级别。

  .. code-block:: c

    #include "esp_log_buffer.h"
    char buffer[] = "The quick brown fox jumps over the lazy dog.";
    ESP_LOG_BUFFER_CHAR_LEVEL(TAG, buffer, sizeof(buffer), ESP_LOG_WARN);

  .. code-block:: none

    I (980) MyModule: The quick brown
    I (985) MyModule: fox jumps over
    I (990) MyModule: the lazy dog.

- :c:macro:`EP_LOG_BUFFER_HEXDUMP`：以格式化的十六进制转储方式输出缓冲区内容，同时显示内存地址和相应的 ASCII 值。适用于调试原始内存内容。

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

输出中包含的行数取决于缓冲区的大小。

二进制日志
----------

二进制日志是一项仅在 **Log V2** 中可用的功能，允许日志以二进制格式而非文本格式进行传输。该功能可通过 Kconfig 选项分别为 **引导加载程序** (``CONFIG_BOOTLOADER_LOG_MODE_BINARY``) 和 **应用程序** (``CONFIG_LOG_MODE_BINARY``) 配置。

默认情况下，当启用 **Log V2** 时，日志系统使用 **文本模式**。启用二进制日志可减少 flash 内存占用，因为日志格式字符串将从 flash 中移除，仅发送其地址。此外，不使用 ``printf`` 函数也将进一步减少堆栈使用和 flash 消耗。

该功能引入了 :c:macro:`ESP_LOG_ATTR_STR` 宏，将格式字符串移至 ``.noload`` 段，并从最终的二进制镜像中移除。该机制还能优化断言或自定义的日志消息，从而进一步降低 flash 消耗。

优势总结：

- 依据应用程序具体情况，最多可减少约 **10% ～ 35%** 的 **flash 占用**。日志越多，节省空间越显著。
- 通过移除类似 ``vprintf`` 的日志格式化函数，降低 **堆栈使用率**。
- 以紧凑的二进制数据传输，减少 **日志传输开销**。

在资源高度受限的环境中，二进制日志优势显著，有助于优化 flash 占用和提升日志效率。

二进制日志工作流程
------------------

二进制日志由两个主要部分组成：

1.  :ref:`芯片端 <chip-side>`：编码并传输日志数据。

    - 编码流程
    - 参数类型编码
    - 运行时参数类型编码

2.  :ref:`主机端 <host-side>`：使用 `esp-idf-monitor 工具 <https://github.com/espressif/esp-idf-monitor>`_ 接收并解码数据。``idf.py monitor`` 命令可自动解析二进制日志。

    - 识别二进制日志数据包。
    - 提取数据包字段（日志等级、格式、标签、时间戳、参数）。
    - 判断地址是否引用：

        - **ELF 文件** （需要查找）
        - **内嵌字符串** （包含在数据包中）

    - 使用格式字符串与参数数组解码日志参数。
    - 将格式字符串与解码后的参数结合，重构最终日志消息。
    - 应用终端颜色输出。

.. _chip-side:

芯片端
^^^^^^

编码流程
""""""""

二进制日志以结构化数据包形式进行传输。如果字符串存在于 ELF 文件中，则以地址形式发送；如果为运行时生成的字符串，则采用内嵌字符串格式发送。

数据包结构如下：

.. code-block:: none

    [0]  - 消息类型（1：引导加载程序、2：应用程序……）
    [1]  - 控制字节（日志等级、版本、time_64bits 标志）
    [2]  - 长度（10 位，最大 1023 字节）
    [3-6]  - 格式地址（若存在于 ELF 中）或内嵌字符串
    [7-10] - 标签地址（若存在于 ELF 中）或内嵌字符串
    [11-14] - 时间戳（若为 32 位）；若时间戳为 64 位，则占用 [11-18]，并在控制字节中设置 time_64bits 标志
    [...] - 参数（可选）：参数数组，包括 32 位、64 位、指针、内嵌字符串/数据等
    [15] - CRC8 校验和

若字符串不在 ELF 文件中，将使用内嵌字符串格式，结构如下：

.. code-block:: none

    [0] - 嵌入标识符 (0xFF - 0xFC)
    [0,1] - （10 位）字符串负长度 = 1 - len(str)
    [...] - 字符串内容

.. note::

    数据包中所有的多字节字段均采用大端 (big-endian) 编码。

参数类型编码
""""""""""""

由于格式字符串已从最终的二进制文件中移除，芯片仍需识别参数类型以实现正确的传输。为此，引入了 :c:macro:`ESP_LOG_ARGS_TYPE` 宏，借助 `_Generic` 功能在编译期间将用户参数划分为三类：**32 位**、**64 位** 和 **指针**。该宏在运行时生成一个 **参数类型数组**，并在调用 ``esp_log`` 时先于用户参数传入数组，以确保：

- 芯片端以正确大小和偏移传输数据。
- 主机工具能够准确重构日志消息。

运行时的行为
""""""""""""

``esp_log`` 函数首先检查是否启用了 **二进制日志**，若已启用，将从 ``va_list`` 中提取 **参数类型数组**。但如果未设置二进制日志标志，系统将无法预处理参数类型数组，这时日志处理器会在运行时从格式字符串中 **提取参数类型**。

运行时提取参数类型效率较低，不如使用 ``ESP_LOG_ATTR_STR(format) ESP_LOG_ARGS(__VA_ARGS__)`` 进行显式指定，在编译时生成参数类型数组，并将格式字符串从 flash 中移除。但在运行时提取参数类型能确保即使 **第三方库不支持二进制日志**，日志依然被正常输出。

buffer 日志需特殊处理
^^^^^^^^^^^^^^^^^^^^^

二进制日志支持以下 buffer 日志函数：

- :c:macro:`ESP_LOG_BUFFER_HEX_LEVEL`
- :c:macro:`ESP_LOG_BUFFER_CHAR_LEVEL`
- :c:macro:`ESP_LOG_BUFFER_HEXDUMP`

在处理上述函数时，二进制日志处理器会检查格式地址是否与预定义常量（如 ``ESP_BUFFER_HEX_FORMAT``）匹配。如果匹配，则 **直接发送原始 buffer 数据**，而非格式字符串。

.. _host-side:

主机端（监视器工具）
^^^^^^^^^^^^^^^^^^^^

在 **主机端**，`esp-idf-monitor 工具 <https://github.com/espressif/esp-idf-monitor>`_ 会自动解码二进制日志。必须确保监视器工具使用正确版本的 ELF 文件。**引导加载程序** 和 **应用程序** 各自对应不同的 ELF 文件，调用 ``idf.py monitor`` 时会自动选择相应的文件。

当接收到 **ELF 地址** 时，监视器工具会根据 ``消息类型`` 字节 **在相应的 ELF 文件中检索对应字符串**。若 **地址以 0xFF 开头** （范围： ``0xFF - 0xFC``），则表示该字段为 **内嵌字符串**，其长度为 **10 位编码**。

一旦获取所有组件，系统便将其格式化并输出到终端。

性能测试
----------

在任务中使用日志时，任务栈必须配置至少 2 KB 的空间，确保有足够的内存进行日志操作。

使用日志组件中的测试工具，基于默认设置（最大和默认日志级别设置为 INFO，禁用颜色支持，未启用 master 日志级别，启用时间戳），在不同芯片上进行了如下两组测试：

- 日志 API 性能测试
- 日志 API 堆栈用量测试

``esp_rom_printf`` 和 ``esp_rom_vprintf`` 的结果相似，同样，``vprintf`` 和 ``printf`` 也得出相似结果。因此，下表仅展示每对相似测试中的一个结果。

.. list-table:: **堆栈使用情况（单位：字节）**
   :header-rows: 1

   * - 功能
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

**Log V1** 和 **Log V2** 之间的堆栈使用量差异可以忽略不计。

.. list-table:: 性能（不包括输出，单位：微秒）
   :header-rows: 1

   * - 功能
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

关于通过 UART 输出日志的性能，**Log V1** 和 **Log V2** 的几乎完全相同。与通过 UART 发送日志所需的时间相比，**Log V2** 在处理开销方面带来的微小差异可以忽略不计。因此，在大多数实际用例中，切换到 **Log V2** 对性能的影响可以忽略。

**内存占用（字节）**

以下测试使用了 ``esp_timer`` 示例和 ESP32 的默认设置，最大和默认日志级别为 INFO，禁用颜色支持，启用时间戳。启用 **Log V2** 后重新构建了示例，然后使用以下命令比较内存占用的差异：

.. code-block:: bash

  idf.py size --diff ~/esp/logv2/build_v1

.. list-table::
   :header-rows: 1

   * - 日志系统版本
     - IRAM
     - DRAM
     - flash 代码
     - flash 数据
     - App 二进制大小
   * - Log V2
     - +1772
     - –36
     - –956
     - –1172
     - 181104 (–384)


.. list-table::
   :header-rows: 1
   :align: center

   * - 日志系统版本
     - 引导加载程序二进制大小
   * - Log V2
     - 26272 (+160)

启用 **Log V2** 会增加 IRAM 的使用量，同时减少整个应用程序的二进制文件大小、flash 代码和数据量。

通过 JTAG 将日志记录到主机
------------------------------

默认情况下，日志库使用类似 vprintf 的函数将格式化的输出写入专用 UART。通过调用一个简单的 API，所有日志输出都可以路由到 JTAG，从而使日志记录速度提高数倍。详情请参阅章节 :ref:`app_trace-logging-to-host`。

线程安全
-------------

在受限环境（或 **ESP_EARLY_LOGx** 和 **ESP_DRAM_LOGx**）记录日志时不使用锁机制，因此，如果其他任务并行记录日志，可能会导致日志损坏的罕见情况。为降低此类风险，建议尽可能使用通用宏。

通用宏 (**ESP_LOGx**) 通过在日志输出过程中获取锁来确保线程安全。在 **Log V2** 中，``flockfile`` 在多个 ``vprintf`` 调用进行格式化处理时提供了额外保护。

日志首先写入内存 buffer，然后发送到 UART 打印，从而确保不同任务之间的线程安全。除非需要确保可靠的日志输出，否则应避免在受限环境中记录日志。

应用示例
-------------------

大多数 ESP-IDF 组件和示例都会使用日志库。如需查看有关日志功能的应用示例，请前往 ESP-IDF 的 :idf:`examples` 目录。与日志最相关的示例如下：

* :example:`system/ota`
* :example:`storage/sd_card`
* :example:`protocols/https_request`

API 参考
-------------

.. include-build-file:: inc/esp_log.inc
.. include-build-file:: inc/esp_log_level.inc
.. include-build-file:: inc/esp_log_buffer.inc
.. include-build-file:: inc/esp_log_timestamp.inc
.. include-build-file:: inc/esp_log_color.inc
.. include-build-file:: inc/esp_log_write.inc
