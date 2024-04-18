内存优化
====================

:link_to_translation:`en:[English]`

{IDF_TARGET_STATIC_MEANS_HEAP:default="Wi-Fi 库和蓝牙控制器", esp32s2="Wi-Fi 库", esp32c6="Wi-Fi 库、蓝牙控制器和 IEEE 802.15.4 库", esp32h2="蓝牙控制器，IEEE 802.15.4 库"}

固件应用程序的可用 RAM 在某些情况下可能处于低水平，甚至完全耗尽。为此，应调整这些情况下固件应用程序的内存使用情况。

固件应用程序通常需要为内部 RAM 保留备用空间，用于应对非常规情况，或在后续版本的更新中，适应 RAM 使用需求的变化。

背景
----------

在进行 ESP-IDF 的内存优化前，应了解有关 {IDF_TARGET_NAME} 内存类型的基础知识、C 语言中静态和动态内存使用的区别、以及 ESP-IDF 中栈和堆的使用方式。以上信息均可参阅 :doc:`/api-reference/system/mem_alloc`。

测量静态内存使用情况
-----------------------------

:ref:`idf.py` 工具可用于生成应用程序静态内存的使用情况报告，请参阅 :ref:`idf.py-size`。

测量动态内存使用情况
------------------------------

ESP-IDF 包含一系列堆 API，可以在运行时测量空闲堆内存，请参阅 :doc:`/api-reference/system/heap_debug`。

.. note::

   在嵌入式系统中，除 RAM 使用总量外，也应重点关注堆碎片化问题。堆测量 API 提供了一些方法，可以测量最大空闲内存块。通过监测最大空闲内存块和总空闲字节数，可以快速了解是否存在堆碎片化问题。

静态内存优化
----------------------------

- 降低应用程序的静态内存使用，会增加运行时堆的可用 RAM 空间，反之亦然。
- 优化静态内存使用通常需要监测 ``.data`` 和 ``.bss`` 的大小，有关工具请参阅 :ref:`idf.py-size`。
- 在 C 语言中，ESP-IDF 内部函数不会占用大量静态 RAM。在多数情况下（例如 {IDF_TARGET_STATIC_MEANS_HEAP}），静态缓冲区仍从堆中分配。然而，这些分配只在功能初始化阶段进行一次，并在功能去初始化时释放，从而在应用程序生命周期中，优化不同阶段的可用内存。

要实现静态内存优化，请执行以下操作：

.. list::

   - 由于常量数据可以存储在 flash 中，不占用 RAM，建议尽量将结构体、缓冲区或其他变量声明为 ``const``。为此，可能需要修改固件参数，使其接收 ``const *`` 参数而非可变指针参数。以上更改还可以减少某些函数的栈内存使用。
   :SOC_BT_SUPPORTED: - 若使用 Bluedroid，请设置 :ref:`CONFIG_BT_BLE_DYNAMIC_ENV_MEMORY` 选项，Bluedroid 将在初始化时分配内存，并在去初始化时释放内存。这并不一定会降低内存使用峰值，但可以将使用静态内存改为运行时使用动态内存。
   - 若使用 OpenThread，请设置 :ref:`CONFIG_OPENTHREAD_PLATFORM_MSGPOOL_MANAGEMENT` 选项，OpenThread 将从外部 PSRAM 中分配消息池缓冲区，从而减少对内部静态内存的使用。

.. _optimize-stack-sizes:

栈内存大小优化
--------------------

在 FreeRTOS 操作系统中，任务栈通常从堆中分配。每个任务的栈大小固定，且会作为参数传递给 :cpp:func:`xTaskCreate`。每个任务可用的栈内存不得超过为其分配的栈内存大小，否则将导致栈内存溢出或堆内存损坏，使原本可用的程序崩溃。

因此，确定每个任务栈内存的最佳大小、最小化每个任务栈内存大小、以及最小化任务栈内存的整体数量，都可以大幅减少 RAM 的使用。

要确定特定任务栈内存的最佳大小，请执行以下操作：

- 程序运行时，如你认为某任务有未使用的栈内存，可通过其任务句柄调用 :cpp:func:`uxTaskGetStackHighWaterMark`。该函数将以字节为单位，返回任务中生命周期最短的空闲栈内存。

  - 从任务本身内部调用 :cpp:func:`uxTaskGetStackHighWaterMark` 是调用该函数最容易的方式：在任务达到其栈内存使用峰值后，调用 ``uxTaskGetStackHighWaterMark(NULL)`` 获取当前任务的高水位标记，换言之，如果有主循环，请多次执行主循环来覆盖各种状态，随后调用 :cpp:func:`uxTaskGetStackHighWaterMark`。
  - 通常可以用任务的栈内存总大小减去调用 :cpp:func:`uxTaskGetStackHighWaterMark` 的返回值，计算任务实际使用的栈内存大小，但应留出一定的安全余量，应对运行时栈内存使用量的小幅意外增长。

- 程序运行时，调用 :cpp:func:`uxTaskGetSystemState` 获取系统中所有任务的摘要，包括各栈内存的高水位标记值。
- 在未使用调试器的监视点时，可以设置 :ref:`CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK` 选项。启用此选项时，系统会使用一个观察点，监视每个任务栈的最后一个字节。如果有新的数据覆盖了该字节（例如发生栈溢出），将立即触发 panic。相比默认 :ref:`CONFIG_FREERTOS_CHECK_STACKOVERFLOW` 选项的 ``Check using canary bytes``，这种方式更可靠，因其能够立即触发 panic，而不是在下一次 RTOS 上下文切换时触发。然而，两种选项都存在缺点，有时栈指针可能会跳过监视点或 canary 字节，损坏 RAM 的其他区域。

要减少特定任务栈内存大小，请执行以下操作：

- 避免占用过多栈内存的函数。字符串格式化函数（如 ``printf()``）会使用大量栈内存，如果任务不调用这类函数，通常可以减小其占用的栈内存。

  - 启用 :ref:`newlib-nano-formatting`，可以在任务调用 ``printf()`` 或其他 C 语言字符串格式化函数时，减少这类任务的栈内存使用量。

- 避免在栈上分配大型变量。在 C 语言声明的默认作用域中，任何分配为自动变量的大型结构体或数组都会占用栈内存。要优化这些变量占用的栈内存大小，可以使用静态分配，或仅在需要时从堆中动态分配。
- 避免调用深度递归函数。尽管调用单个递归函数并不一定会占用大量栈内存，但若每个函数都包含大量基于栈的变量，那么调用这些函数的开销将会很高。

要减少任务的整体数量，请执行以下操作：

- 合并任务。如果从未创建某个特定任务，就不会分配该任务的栈内存，从而极大减少 RAM 使用。如果某些任务可以与另一个任务合并，通常可以将不必要的任务删除。在应用程序中，如果满足以下条件，通常可以合并或删除任务：

   - 任务所执行的内容可以按顺序分解为多个函数调用。
   - 任务所执行的内容可以分解为较小的工作，这些工作可以通过 FreeRTOS 队列或类似机制串行化，并由工作任务执行。

内部任务栈内存大小
^^^^^^^^^^^^^^^^^^^^

为进行系统维护，或操作系统功能，ESP-IDF 分配了许多内部任务，一部分在启动过程中创建，一部分在初始化特定功能时创建。

为了确保支持所有常见的使用模式，这些任务栈内存的默认设置值较大。ESP-IDF 支持配置栈内存大小，因此可以减小任务栈内存，匹配其实际运行时的栈内存使用情况。

.. important::

   如果内部任务的栈内存设置得过小，可能会导致 ESP-IDF 发生无法预测的崩溃。即使任务栈内存溢出是导致崩溃的根本原因，在调试过程中也很难确定具体原因。因此，建议特别关注任务在负载高时的高水位标记，在必要情况下，谨慎减小内部任务的栈内存大小。如果在减小内部任务堆内存大小后，仍遇到问题，请在报告中提供以下信息，以及正在使用的具体配置。

.. list::

   - :ref:`app-main-task` 的栈内存大小为 :ref:`CONFIG_ESP_MAIN_TASK_STACK_SIZE`。
   - 系统任务 :doc:`/api-reference/system/esp_timer` 用于执行回调函数，其栈内存大小为 :ref:`CONFIG_ESP_TIMER_TASK_STACK_SIZE`。
   - 部分 FreeRTOS 定时器任务用于处理 FreeRTOS 定时器回调，其栈内存大小为 :ref:`CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH`。
   - 系统任务 :doc:`/api-reference/system/esp_event` 用于执行默认系统事件循环回调，其栈内存大小为 :ref:`CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE`。
   - TCP/IP 任务 :doc:`/api-guides/lwip`  的栈内存大小为 :ref:`CONFIG_LWIP_TCPIP_TASK_STACK_SIZE`。
   :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/index` 的栈内存大小为 :ref:`CONFIG_BT_BTC_TASK_STACK_SIZE`，:ref:`CONFIG_BT_BTU_TASK_STACK_SIZE`。
   :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/nimble/index` 的栈内存大小为 :ref:`CONFIG_BT_NIMBLE_HOST_TASK_STACK_SIZE`。
   - 以太网驱动程序会创建任务，用于使 MAC 接收以太网帧，在默认配置 ``ETH_MAC_DEFAULT_CONFIG`` 下，任务栈内存大小为 4 KB。在初始化以太网 MAC 时，传递自定义 :cpp:class:`eth_mac_config_t` 结构体可以更改此设置。
   - FreeRTOS 空闲任务栈内存大小由 :ref:`CONFIG_FREERTOS_IDLE_TASK_STACKSIZE` 配置。
   - 使用 :doc:`/api-reference/protocols/mqtt` 组件时会创建一个任务，其栈内存大小由 :ref:`CONFIG_MQTT_TASK_STACK_SIZE` 配置。MQTT 栈内存大小也可以使用 :cpp:class:`esp_mqtt_client_config_t` 结构体中的 ``task_stack`` 字段配置。
   - 有关使用 ``mDNS`` 时内存优化的详细信息，请参阅 `优化内存使用 <https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html#minimizing-ram-usage>`__。

.. note::

   除 ESP 定时器等内置系统功能外，若固件应用程序没有初始化 ESP-IDF 中特定功能，则不会创建相关任务。此时，相关任务的栈内存使用量为零，而这些功能没有与之关联的任务，因此无需考虑其栈内存大小配置。

堆内存优化
-------------------

有关分析运行时堆内存使用的函数，请参阅 :doc:`/api-reference/system/heap_debug`。

通常，堆内存优化包含以下几个方面：分析堆内存使用情况、撤回未使用的 ``malloc()`` 调用、缩小相应的内存使用大小、或提早释放先前分配的缓冲区。

以下是一些 ESP-IDF 配置选项，有助于在运行时实现堆内存优化：

.. list::

   - lwIP 文档中的有关章节介绍了如何配置 :ref:`lwip-ram-usage`。
   :SOC_WIFI_SUPPORTED: - :ref:`wifi-buffer-usage` 中介绍了一些选项，这些选项可以减少对静态缓冲区的使用，或减少运行时动态缓冲区的最大数量，从而最小化内存使用，但可能会影响性能。注意，Wi-Fi 初始化时，仍会从堆中分配静态 Wi-Fi 缓冲区，并在 Wi-Fi 去初始化时释放这些缓冲区。
   :esp32: - 以太网驱动程序在初始化时会为内部以太网 MAC 分配 DMA 缓冲区，配置选项包括 :ref:`CONFIG_ETH_DMA_BUFFER_SIZE`、:ref:`CONFIG_ETH_DMA_RX_BUFFER_NUM` 和 :ref:`CONFIG_ETH_DMA_TX_BUFFER_NUM`。
   - 部分 Mbed TLS 配置选项也可用于堆内存优化，详情请参阅 :ref:`reducing_ram_usage_mbedtls` 的 Mbed TLS 部分。
   :esp32: - 仅在单核模式下，启用 :ref:`CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY`，可以将 IRAM 作为可按字节访问的内存添加到常规堆内存中使用。注意，此选项会影响性能，并存在由可执行数据引发安全问题的风险。若启用此选项，可以通过设置 :ref:`CONFIG_MBEDTLS_MEM_ALLOC_MODE` 和 :ref:`CONFIG_BT_NIMBLE_MEM_ALLOC_MODE` 选项，优先从内存中分配某些缓冲区。
   :esp32: - 若使用 Bluetooth LE，请优化 :ref:`CONFIG_BTDM_CTRL_BLE_MAX_CONN`。
   :esp32: - 若使用经典蓝牙，请优化 :ref:`CONFIG_BTDM_CTRL_BR_EDR_MAX_ACL_CONN`。

.. note::

   如果将某些配置选项更改为非默认值，也会增加运行时的堆内存使用。这类选项未在上文中列出，但配置选项的帮助文档中给出了相应说明。

.. _optimize-iram-usage:

IRAM 优化
-------------

.. only:: not esp32

   程序运行时，由于使用了静态 IRAM，用于堆内存使用的 DRAM 会相应减少。反之，可以通过减少 IRAM 使用，增加可用 DRAM。

如果应用程序分配的静态 IRAM 超过可用上限，应用程序将无法构建，并出现链接器错误，如 ``section '.iram0.text' will not fit in region 'iram0_0_seg'``、``IRAM0 segment data does not fit`` 以及 ``region 'iram0_0_seg' overflowed by 84-bytes``。如果发生这种情况，应找到减少静态 IRAM 使用的方法，链接应用程序。

要分析固件应用程序二进制文件中的 IRAM 使用情况，请使用 :ref:`idf.py-size`。如果固件应用程序链接失败，请参阅 :ref:`idf-size-linker-failed` 中的步骤，分析失败原因。

要对某些 ESP-IDF 功能进行 IRAM 优化，请使用以下选项：

.. list::

    - 启用 :ref:`CONFIG_FREERTOS_PLACE_FUNCTIONS_INTO_FLASH`。只要没有从 ISR 中错误地调用这些函数，就可以在所有配置中安全启用此选项。
    - 启用 :ref:`CONFIG_RINGBUF_PLACE_FUNCTIONS_INTO_FLASH`。只要没有从 ISR 中错误地调用这些函数，就可以在所有配置中安全启用此选项。
    - 启用 :ref:`CONFIG_RINGBUF_PLACE_ISR_FUNCTIONS_INTO_FLASH`。如果从 IRAM 中的中断上下文中使用 ISR ringbuf 函数，例如启用了 :ref:`CONFIG_UART_ISR_IN_IRAM`，则无法安全使用此选项。在此情况下，安装 ESP-IDF 相关驱动程序时，将在运行时报错。
    :SOC_WIFI_SUPPORTED: - 禁用 Wi-Fi 选项 :ref:`CONFIG_ESP_WIFI_IRAM_OPT` 和/或 :ref:`CONFIG_ESP_WIFI_RX_IRAM_OPT` 会释放可用 IRAM，但会牺牲部分 Wi-Fi 性能。
    :CONFIG_ESP_ROM_HAS_SPI_FLASH: - 启用 :ref:`CONFIG_SPI_FLASH_ROM_IMPL` 选项可以释放一些 IRAM，但此时 esp_flash 错误修复程序及新的 flash 芯片支持将失效，详情请参阅 :doc:`/api-reference/peripherals/spi_flash/spi_flash_idf_vs_rom`。
    :esp32: - 禁用 :ref:`CONFIG_SPI_FLASH_ROM_DRIVER_PATCH` 选项可以释放一些 IRAM，但仅适用于某些 flash 配置，详情请参阅配置项帮助文档。
    :esp32: - 如果应用程序基于 ESP32 rev. 3 (ECO3)，且使用 PSRAM，设置 :ref:`CONFIG_ESP32_REV_MIN` 为 ``3``，可以禁用 PSRAM 的错误处理程序，节省 10 KB 乃至更多的 IRAM。
    - 禁用 :ref:`CONFIG_ESP_EVENT_POST_FROM_IRAM_ISR` 可以防止从 :ref:`iram-safe-interrupt-handlers` 中发布 ``esp_event`` 事件，节省 IRAM 空间。
    :SOC_GPSPI_SUPPORTED: - 禁用 :ref:`CONFIG_SPI_MASTER_ISR_IN_IRAM` 可以防止在写入 flash 时发生 spi_master 中断，节省 IRAM 空间，但可能影响 spi_master 的性能。
    :SOC_GPSPI_SUPPORTED: - 禁用 :ref:`CONFIG_SPI_SLAVE_ISR_IN_IRAM` 可以防止在写入 flash 时发生 spi_slave 中断，节省 IRAM 空间。
    - 设置 :ref:`CONFIG_HAL_DEFAULT_ASSERTION_LEVEL` 为禁用 HAL 组件的断言，可以节省 IRAM 空间，对于经常调用 ``HAL_ASSERT`` 且位于 IRAM 中的 HAL 代码尤为如此。
    - 要禁用不需要的 flash 驱动程序，节省 IRAM 空间，请参阅 sdkconfig 菜单中的 ``Auto-detect Flash chips`` 选项。
    :SOC_GPSPI_SUPPORTED: - 启用 :ref:`CONFIG_HEAP_PLACE_FUNCTION_INTO_FLASH`。只要未启用 :ref:`CONFIG_SPI_MASTER_ISR_IN_IRAM` 选项，且没有从 ISR 中错误地调用堆函数，就可以在所有配置中安全启用此选项。
    :esp32c2: - 启用 :ref:`CONFIG_BT_RELEASE_IRAM`。 蓝牙所使用的 data，bss 和 text 段已经被分配在连续的RAM区间。当调用 ``esp_bt_mem_release`` 时，这些段都会被添加到 Heap 中。 这将节省约 22 KB 的 RAM。但要再次使用蓝牙功能，需要重启程序。

.. only:: esp32

   将 SRAM1 用于 IRAM
   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   SRAM1 内存区域通常用于 DRAM 存储，但可以设置 :ref:`CONFIG_ESP_SYSTEM_ESP32_SRAM1_REGION_AS_IRAM` 选项，将其中一部分用作 IRAM 存储。引入该选项前，这个内存区域通常预留给 DRAM 数据使用（如 ``.bss`` ），随后由软件引导加载程序加入到堆中。引入该选项后，引导加载程序的 DRAM 大小会减少到更接近实际需要的值。

   要使用以上选项，ESP-IDF 应能够将新的 SRAM1 区域识别为有效镜像段的加载地址。部分应用程序的代码置于新扩展的 IRAM 区域，如果软件引导加载程序在引入该选项前编译，将无法加载这类应用程序。这类情况通常在进行 OTA 更新时发生，此时仅会更新应用程序。

   如果 IRAM 段放置在无效区域，在启动过程中将检测到以下问题，并导致启动失败：

   .. code-block:: text

      E (204) esp_image: Segment 5 0x400845f8-0x400a126c invalid: bad load address range

   .. warning::

      若与在引入以上配置选项前编译的软件引导加载程序一同使用，使用 :ref:`CONFIG_ESP_SYSTEM_ESP32_SRAM1_REGION_AS_IRAM` 选项编译的应用程序很可能无法启动。若使用旧版本的引导加载程序，并进行 OTA 更新，请在提交任何更新前仔细测试。

   任何最终未用于静态 IRAM 的内存都将添加到堆内存中。

.. only:: esp32c3

    flash 暂停特性
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    在使用 SPI flash API 和基于 SPI flash API 的 API（如 NVS、分区 API 等）时，将禁用缓存。在此期间执行的所有代码都必须放置于内部 RAM 中，详情请参阅 :ref:`concurrency-constraints-flash`。因此，系统只会执行位于内部 RAM 中的中断处理程序。

    为将代码置于内部 RAM，ESP-IDF 驱动通常支持以下两个选项：

    - 将驱动程序的内部 ISR 处理程序放置在内部 RAM 中。
    - 将某些控制函数放置在内部 RAM 中。

    在中断上下文中使用用户 ISR 回调及其相关变量时，也必须将其放置在内部 RAM 中。

    将额外代码放置到 IRAM 中，将增加 IRAM 使用量，ESP-IDF 提供了 :ref:`CONFIG_SPI_FLASH_AUTO_SUSPEND` 选项，可以缓解 IRAM 的使用。通过启用此功能，使用 SPI flash API 和基于 SPI flash API 的 API 时，不会导致缓存禁用，因此 flash 中的代码和数据仍可正常执行或访问，但会有些延迟。有关此功能的详细信息，请参阅 :ref:`auto-suspend`。

    有关 flash 暂停特性的使用及其相应的响应时间延迟，请参阅 :example:`system/flash_suspend`。


.. only:: esp32

    在 flash 中放置 C 语言库函数
    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    编译 ECO3 及之前的 ESP32 版本时（参阅 :ref:`CONFIG_ESP32_REV_MIN`），会启用 PSRAM 缓存错误的解决方法选项（参阅 :ref:`CONFIG_SPIRAM_CACHE_WORKAROUND`），此选项会重新编译通常位于 ROM 中的 C 语言库函数，并将其放置在 IRAM 中。对于大部分应用程序而言，可以放心将多数 C 语言库函数移到 flash 中，以节省 IRAM 空间。相应选项包括：

    .. list::

        - :ref:`CONFIG_SPIRAM_CACHE_LIBJMP_IN_IRAM`：影响函数 ``longjmp`` 和 ``setjump``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBMATH_IN_IRAM`：影响函数 ``abs``、``div``、``labs``、``ldiv``、``quorem``、``fpclassify`` 和 ``nan``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBNUMPARSER_IN_IRAM`：影响函数 ``utoa``、``itoa``、``atoi``、``atol``、``strtol`` 和 ``strtoul``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBIO_IN_IRAM`：影响函数 ``wcrtomb``、``fvwrite``、``wbuf``、``wsetup``、``fputwc``、``wctomb_r``、``ungetc``、``makebuf``、``fflush``、``refill`` 和 ``sccl``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBTIME_IN_IRAM`：影响函数 ``asctime``、``asctime_r``、``ctime``、``ctime_r``、``lcltime``、``lcltime_r``、``gmtime``、``gmtime_r``、``strftime``、``mktime``、``tzset_r``、``tzset``、``time``、``gettzinfo``、``systimes``、``month_lengths``、``timelocal``、``tzvars``、``tzlock``、``tzcalc_limits`` 和 ``strptime``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBCHAR_IN_IRAM`：影响函数 ``ctype_``、``toupper``、``tolower``、``toascii``、``strupr``、``bzero``、``isalnum``、``isalpha``、``isascii``、``isblank``、``iscntrl``、``isdigit``、``isgraph``、``islower``、``isprint``、``ispunct``、``isspace`` 和 ``isupper``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBMEM_IN_IRAM`：影响函数 ``memccpy``、``memchr``、``memmove`` 和 ``memrchr``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBSTR_IN_IRAM`：影响函数 ``strcasecmp``、``strcasestr``、``strchr``、``strcoll``、``strcpy``、``strcspn``、``strdup``、``strdup_r``、``strlcat``、``strlcpy``、``strlen``、``strlwr``、``strncasecmp``、``strncat``、``strncmp``、``strncpy``、``strndup``、``strndup_r``、``strrchr``、``strsep``、``strspn``、``strstr``、``strtok_r 和 ``strupr``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBRAND_IN_IRAM`：影响函数 ``srand``、``rand`` 和 ``rand_r``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBENV_IN_IRAM`：影响函数 ``environ``、``envlock`` 和 ``getenv_r``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBFILE_IN_IRAM`：影响函数 ``lock``、``isatty``、``fclose``、``open``、``close``、``creat``、``read``、``rshift``、``sbrk``、``stdio``、``syssbrk``、``sysclose``、``sysopen``、``creat``、``sysread``、``syswrite``、``impure``、``fwalk`` 和 ``findfp``。
        - :ref:`CONFIG_SPIRAM_CACHE_LIBMISC_IN_IRAM`：影响函数 ``raise`` 和 ``system``。

    具体节省的 IRAM 使用量取决于应用程序实际使用的 C 语言库代码。此外，以下选项可以将更多 C 语言库代码移到 flash 中，但请知悉这可能会影响性能。同时，注意不要在禁用缓存时，从中断使用 :c:macro:`ESP_INTR_FLAG_IRAM` 标记符号分配的 C 语言库函数，详情请参阅 :ref:`iram-safe-interrupt-handlers`。鉴于以上原因，函数 ``itoa``、``memcmp``、``memcpy``、``memset``、``strcat``、``strcmp`` 和 ``strlen`` 始终置于 IRAM 中。

.. note::

    将常用函数从 IRAM 移动到 flash，可能会增加函数的执行时间。

.. note::

    部分配置选项可以将一些功能移动到 IRAM 中，从而提高性能，但这类选项默认不进行配置，因此未在此列出。了解启用上述选项对 IRAM 大小造成的影响，请参阅配置项的帮助文本。
