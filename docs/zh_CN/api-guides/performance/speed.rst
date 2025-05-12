速度优化
=========

:link_to_translation:`en:[English]`

{IDF_TARGET_CONTROLLER_CORE_CONFIG:default="CONFIG_BT_CTRL_PINNED_TO_CORE", esp32="CONFIG_BTDM_CTRL_PINNED_TO_CORE_CHOICE", esp32s3="CONFIG_BT_CTRL_PINNED_TO_CORE_CHOICE"}
{IDF_TARGET_RF_TYPE:default="Wi-Fi/蓝牙", esp32s2="Wi-Fi", esp32c6="Wi-Fi/蓝牙/802.15.4", esp32c61="Wi-Fi/蓝牙", esp32h2="蓝牙/802.15.4", esp32h21="蓝牙/802.15.4", esp32h4="蓝牙/802.15.4", esp32c5="Wi-Fi/蓝牙/802.15.4"}

概述
-----------

提高代码执行速度是提升软件性能的关键要素，该优化也可能带来其他积极影响，比如降低总体功耗。然而，提高代码执行速度可能会牺牲其他性能，如 :doc:`size` 。

决定优化目标
-----------------------------

如果应用程序固件中的某个函数仅每周在后台执行一次，其执行时间是 10 ms 还是 100 ms 对整体性能的影响或可忽略不计。但如果某个函数以 10 Hz 的频率持续执行，其执行时间是 10 ms 还是 100 ms 就会对系统性能产生显著影响。

大多数应用程序固件中，只有一小部分函数需要优化性能，例如频繁执行的函数，或者必须满足应用程序对延迟或吞吐量的要求的函数。应针对这些特定函数优化其执行速度。

测量性能
---------------------

想要提升某方面性能，首先要对其进行测量。

基本性能测量方法
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

可以直接测量与外部交互的性能，例如，测量一般网络性能可以参考 :example:`wifi/iperf` 和 :example:`ethernet/iperf` ，或者使用示波器或逻辑分析仪来测量与设备外设的交互时间。

此外，另一种测量性能的方法是在代码中添加计时测量：

.. code-block:: c

    #include "esp_timer.h"

    void measure_important_function(void) {
        const unsigned MEASUREMENTS = 5000;
        uint64_t start = esp_timer_get_time();

        for (int retries = 0; retries < MEASUREMENTS; retries++) {
            important_function(); // 需要测量的代码
        }

        uint64_t end = esp_timer_get_time();

        printf("%u iterations took %llu milliseconds (%llu microseconds per invocation)\n",
               MEASUREMENTS, (end - start)/1000, (end - start)/MEASUREMENTS);
    }

通过多次执行目标代码可以降低其他因素的影响，例如实时操作系统 (RTOS) 的上下文切换、测量的开销等。

- 使用 :cpp:func:`esp_timer_get_time` 可以生成微秒级精度的“墙钟”时间戳，但每次调用计时函数都会产生适量开销。
- 也可以使用标准 Unix 函数 ``gettimeofday()`` 和 ``utime()`` 来进行计时测量，尽管其开销略高一些。
- 此外，代码中包含 ``hal/cpu_hal.h`` 头文件，并调用 HAL 函数 ``cpu_hal_get_cycle_count()`` 可以返回已执行的 CPU 循环数。该函数开销较低，适用于高精度测量执行时间极短的代码。

  .. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

      CPU 周期是各内核独立计数的，因此本方法仅适用于测量中断处理程序或固定在单个核上的任务。

- 在执行“微基准测试”时（即仅对运行时间不到 1-2 ms 的小代码段进行基准测试），二进制文件会影响 flash 缓存的性能，进而可能会导致计时测量出现较大差异。这是因为二进制布局可能会导致在特定的执行顺序中产生不同模式的缓存缺失。执行较大测试代码通常可以抵消这种影响。在基准测试时多次执行一个小函数可以减少 flash 缓存缺失的影响。另外，将该代码移到 IRAM 中（参见 :ref:`speed-targeted-optimizations` ）也可以解决这个问题。

外部跟踪
^^^^^^^^^^^^^^^^^^^^

:doc:`/api-guides/app_trace` 可以在几乎不影响代码执行的情况下测量其执行速度。

任务
^^^^^^^

如果启用了选项 :ref:`CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS` ，则可以使用 FreeRTOS API :cpp:func:`vTaskGetRunTimeStats` 来获取各个 FreeRTOS 任务运行时占用处理器的时间。

:ref:`SEGGER SystemView <app_trace-system-behaviour-analysis-with-segger-systemview>` 是一款出色的工具，可将任务执行情况可视化，也可用于排查系统整体的性能问题或改进方向。

提高整体速度
-----------------------------

以下优化措施将提高几乎所有代码的执行效果，包括启动时间、吞吐量、延迟等：

.. list::

    :esp32: - 设置 :ref:`CONFIG_ESPTOOLPY_FLASHFREQ` 为 80 MHz。该值为默认值 40 MHz 的两倍，这意味着从 flash 加载或执行代码的速度也将翻倍。在更改此设置之前，应事先确认连接 {IDF_TARGET_NAME} 和 flash 的板或模块在温度限制范围内支持 80 MHz 的操作。相关信息参见硬件数据手册。
    - 设置 :ref:`CONFIG_ESPTOOLPY_FLASHMODE` 为 QIO 或 QOUT 模式（四线 I/O 模式）。相较于默认的 DIO 模式，在这两种模式下，从 flash 加载或执行代码的速度几乎翻倍。如果两种模式都支持，QIO 会稍微快于 QOUT。请注意，flash 芯片以及 {IDF_TARGET_NAME} 与 flash 芯片之间的电气连接都必须支持四线 I/O 模式，否则 SoC 将无法正常工作。
    - 设置 :ref:`CONFIG_COMPILER_OPTIMIZATION` 为 ``Optimize for performance (-O2)`` 。相较于默认设置，这可能会略微增加二进制文件大小，但几乎必然会提高某些代码的性能。请注意，如果代码包含 C 或 C++ 的未定义行为，提高编译器优化级别可能会暴露出原本未发现的错误。
    :SOC_ASSIST_DEBUG_SUPPORTED: - 禁用 :ref:`CONFIG_ESP_SYSTEM_HW_STACK_GUARD` 可能会小幅提高代码性能，尤其是在设备上出现大量中断的情况下。
    :esp32: - 如果应用程序是基于 ESP32 rev. 3 (ECO3) 的项目并且使用 PSRAM，设置 :ref:`CONFIG_ESP32_REV_MIN` 为 ``3`` 将禁用 PSRAM 的错误修复工作，可以减小代码大小并提高整体性能。
    :SOC_CPU_HAS_FPU: - 避免使用浮点运算 ``float``。尽管 {IDF_TARGET_NAME} 具备单精度浮点运算器，但是浮点运算总是慢于整数运算。因此可以考虑使用不同的整数表示方法进行运算，如定点表示法，或者将部分计算用整数运算后再切换为浮点运算。
    :not SOC_CPU_HAS_FPU: - 避免使用浮点运算 ``float``。{IDF_TARGET_NAME} 通过软件模拟进行浮点运算，因此速度非常慢。可以考虑使用不同的整数表示方法进行运算，如定点表示法，或者将部分计算用整数运算后再切换为浮点运算。
    - 避免使用双精度浮点运算 ``double``。{IDF_TARGET_NAME} 通过软件模拟进行双精度浮点运算，因此速度非常慢。可以考虑使用基于整数的表示方法或单精度浮点数。
    :CONFIG_ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY: - 在性能要求较高的代码段中，应避免执行未对齐的 4 字节内存访问。为提升性能，可以考虑启用 :ref:`CONFIG_LIBC_OPTIMIZED_MISALIGNED_ACCESS`。启用此选项将额外占用约 190 字节的 IRAM 和 870 字节的 flash 存储。请注意，正确对齐的内存操作始终能够以全速执行，且不会产生性能损耗。


.. only:: esp32s2 or esp32s3 or esp32p4

    更改 cache 大小
    ^^^^^^^^^^^^^^^

    在 {IDF_TARGET_NAME} 上，通过下面列出的 Kconfig 选项增加 cache 的大小，“cache 缺失”的频率可能会降低，从而在一定程度上提高整体速度。

    .. list::

        :esp32s2: - :ref:`CONFIG_ESP32S2_INSTRUCTION_CACHE_SIZE`.
        :esp32s2: - :ref:`CONFIG_ESP32S2_DATA_CACHE_SIZE`.
        :esp32s3: - :ref:`CONFIG_ESP32S3_INSTRUCTION_CACHE_SIZE`.
        :esp32s3: - :ref:`CONFIG_ESP32S3_DATA_CACHE_SIZE`.
        :esp32p4: - :ref:`CONFIG_CACHE_L2_CACHE_SIZE`.


    .. note::

        增加 cache 大小也将导致可用 RAM 的减少。


.. only:: SOC_CACHE_L2_CACHE_SIZE_CONFIGURABLE

    .. note::

        在 {IDF_TARGET_NAME} 上，可以通过 Kconfig 选项 :ref:`CONFIG_CACHE_L2_CACHE_SIZE` 来配置 L2 cache 大小。
        将 L2 cache 大小设为最小，则可用 RAM 大小达到最大，但也可能提高“cache 缺失”的频率。
        将 L2 cache 大小设为最大，则“cache 缺失”的频率可能降低，但可用 RAM 大小也随之减少。

减少日志开销
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

尽管标准输出会先存储在缓冲区中，但缓冲区缺少可用空间时，应用程序将数据输出到日志的速度可能会受限。这点在程序启动并输出大量日志时尤为明显，但也可能随时发生。为解决这一问题，可以采取以下几种方法：

.. list::

    - 通过调低应用日志默认等级 :ref:`CONFIG_LOG_DEFAULT_LEVEL` （引导加载程序日志等级的相应配置为 :ref:`CONFIG_BOOTLOADER_LOG_LEVEL`）来减少日志输出量。这样做不仅可以减小二进制文件大小，还可以节省一些 CPU 用于格式化字符串的时间。
    :not SOC_USB_OTG_SUPPORTED: - 增加 :ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE` ，可以提高日志输出速度。
    :SOC_USB_OTG_SUPPORTED: - 增加 :ref:`CONFIG_ESP_CONSOLE_UART_BAUDRATE` ，可以提高日志输出速度。如果使用内置 USB-CDC 作为串口控制台，那么串口传输速率不会受配置的波特率影响。
    - 如果应用程序不需要动态更改日志级别，并且不需要使用标签来控制每个模块的日志，建议禁用 :ref:`CONFIG_LOG_DYNAMIC_LEVEL_CONTROL` 并更改 :ref:`CONFIG_LOG_TAG_LEVEL_IMPL`。与默认选项相比，这可以减少内存使用，并且将应用程序中的日志操作速度提高约 10 倍。

不建议的选项
^^^^^^^^^^^^^^^^^^

以下选项也可以提高执行速度，但不建议使用，因为它们会降低固件应用程序的可调试性，并可能导致出现更严重的 bug。

.. list::

   - 禁用 :ref:`CONFIG_COMPILER_OPTIMIZATION_ASSERTION_LEVEL` 。这也会略微减小固件二进制文件大小。然而，它可能导致出现更严重的 bug，甚至出现安全性 bug。如果为了优化特定函数而必须禁用该选项，可以考虑在该源文件的顶部单独添加 ``#define NDEBUG`` 。

.. _speed-targeted-optimizations:


针对性优化
---------------------------

以下更改将提高固件应用程序特定部分的速度：

.. list::

    - 将频繁执行的代码移至 IRAM。应用程序中的所有代码都默认从 flash 中执行。这意味着缓存缺失时，CPU 需要等待从 flash 加载后续指令。如果将函数复制到 IRAM 中，则仅需要在启动时加载一次，然后始终以全速执行。

      IRAM 资源有限，使用更多的 IRAM 可能会减少可用的 DRAM。因此，将代码移动到 IRAM 需要有所取舍。更多信息参见 :ref:`iram` 。

    -  针对不需要放置在 IRAM 中的单个源文件，可以重新启用跳转表优化。这将提高大型 ``switch cases`` 代码中的热路径性能。关于如何在编译单个源文件时添加 -fjump-tables -ftree-switch-conversion 选项，参见 :ref:`component_build_control` 。

减少启动时间
----------------------------

除了上述提高整体性能的方法外，还可以微调以下选项来专门减少启动时间：

.. list::

   - 最小化 :ref:`CONFIG_LOG_DEFAULT_LEVEL` 和 :ref:`CONFIG_BOOTLOADER_LOG_LEVEL` 可以大幅减少启动时间。如要在应用程序启动后获取更多日志，可以设置 :ref:`CONFIG_LOG_MAXIMUM_LEVEL`，然后调用 :cpp:func:`esp_log_level_set` 来恢复更高级别的日志输出。示例 :example:`system/startup_time` 的主函数展示了如何实现这一点。
   :SOC_RTC_FAST_MEM_SUPPORTED: - 如果使用 Deep-sleep 模式，启用 :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` 可以加快从睡眠中唤醒的速度。请注意，启用该选项后在唤醒时将不会执行安全启动验证，需要考量安全风险。
   - 设置 :ref:`CONFIG_BOOTLOADER_SKIP_VALIDATE_ON_POWER_ON` 可以在每次上电复位启动时跳过二进制文件验证，节省的时间取决于二进制文件大小和 flash 设置。请注意，如果 flash 意外损坏，此设置将有一定风险。更多关于使用该选项的解释和建议，参见 :ref:`项目配置 <CONFIG_BOOTLOADER_SKIP_VALIDATE_ON_POWER_ON>` 。
   - 禁用 RTC 慢速时钟校准可以节省一小部分启动时间。设置 :ref:`CONFIG_RTC_CLK_CAL_CYCLES` 为 0 可以实现该操作。设置后，以 RTC 慢速时钟为时钟源的固件部分精确度将降低。
   :SOC_SPIRAM_SUPPORTED: - 使用外部内存（启用 :ref:`CONFIG_SPIRAM`）时，启用外部内存 (:ref:`CONFIG_SPIRAM_MEMTEST`) 测试可能会大大增加启动时间（每测试 4 MB 的内存大约增加 1 秒）。禁用内存测试将减少启动时间，但将无法对外部存储器进行测试。
   :SOC_SPIRAM_SUPPORTED: - 使用外部内存（启用 :ref:`CONFIG_SPIRAM`）时，所有用作堆的内存（包括外部内存）都将被设为默认值，所以启用全面的 poisoning 将增加启动时间（每设置 4 MiB 的内存大约增加 300 毫秒）。

示例项目 :example:`system/startup_time` 预配了优化启动时间的设置，文件 :example_file:`system/startup_time/sdkconfig.defaults` 包含了所有相关设置。可以将这些设置追加到项目中 ``sdkconfig`` 文件的末尾并合并，但请事先阅读每个设置的相关说明。

任务优先级
--------------------

ESP-IDF FreeRTOS 是实时操作系统，因此需确保高吞吐量或低延迟的任务获得更高优先级，以便立即运行。调用 :cpp:func:`xTaskCreate` 或 :cpp:func:`xTaskCreatePinnedToCore` 会设定优先级，并且可以在运行时调用 :cpp:func:`vTaskPrioritySet` 进行更改。

此外，还需确保任务适时释放 CPU（通过调用 :cpp:func:`vTaskDelay` 或 ``sleep()`` ，或在信号量、队列、任务通知等方面进行阻塞），以避免低优先级任务饥饿并造成系统性问题。 :ref:`task-watchdog-timer` 提供任务饥饿自动检测机制，但请注意，正确的固件操作有时需要长时间运算，因此任务看门狗定时器超时并不总意味着存在问题。在这些情况下，可能需要微调超时时限，甚至禁用任务看门狗定时器。

.. _built-in-task-priorities:

内置任务优先级
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF 启动的系统任务预设了固定优先级。启动时，一些任务会自动启动，而另一些仅在应用程序固件初始化特定功能时启动。为优化性能，请合理设置应用程序任务优先级，以确保它们不会被系统任务阻塞，同时需确保系统任务不会饥饿进而影响其他系统功能。

为此，可能需要分解特定任务。例如，可以在高优先级任务或中断处理程序中执行实时操作，并在较低优先级任务中处理非实时操作。

头文件 :idf_file:`components/esp_system/include/esp_task.h` 包含了用于设置 ESP-IDF 内置任务系统优先级的宏定义。更多系统任务详情，参见 :ref:`freertos_system_tasks` 。

常见优先级包括：

.. Note: 以下两个列表应保持一致，但第二个列表还展示了 CPU 亲和性。

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    .. list::

        - :ref:`app-main-task` 中执行 app_main 函数的主任务优先级最低 (1)。
        - 系统任务 :doc:`/api-reference/system/esp_timer` 用于管理定时器事件并执行回调函数，优先级较高 (22, ``ESP_TASK_TIMER_PRIO``)。
        - FreeRTOS 初始化调度器时会创建定时器任务，用于处理 FreeRTOS 定时器的回调函数，优先级最低（1, :ref:`可配置 <CONFIG_FREERTOS_TIMER_TASK_PRIORITY>` ）。
        - 系统任务 :doc:`/api-reference/system/esp_event` 用于管理默认的系统事件循环并执行回调函数，优先级较高 (20, ``ESP_TASK_EVENT_PRIO``)。仅在应用程序调用 :cpp:func:`esp_event_loop_create_default` 时使用此配置。可以调用 :cpp:func:`esp_event_loop_create` 添加自定义任务配置。
        - :doc:`/api-guides/lwip` TCP/IP 任务优先级较高 (18, ``ESP_TASK_TCPIP_PRIO``)。
        :SOC_WIFI_SUPPORTED: - :doc:`/api-guides/wifi` 任务优先级较高 (23).
        :SOC_WIFI_SUPPORTED: - 使用 Wi-Fi Protected Setup (WPS)、WPA2 EAP-TLS、Device Provisioning Protocol (DPP) 或 BSS Transition Management (BTM) 等功能时，Wi-Fi wpa_supplicant 组件可能会创建优先级较低的专用任务 (2)。
        :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/controller_vhci` 任务优先级较高 (23, ``ESP_TASK_BT_CONTROLLER_PRIO``)。蓝牙控制器需要以低延迟响应请求，因此其任务应始终为系统最高优先级的任务之一。
        :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/nimble/index` 任务优先级较高 (21).
        - 以太网驱动程序会创建一个 MAC 任务，用于接收以太网帧。如果使用默认配置 ``ETH_MAC_DEFAULT_CONFIG`` ，则该任务为中高优先级 (15)。可以在以太网 MAC 初始化时输入自定义 :cpp:class:`eth_mac_config_t` 结构体来更改此设置。
        - 如果使用 :doc:`/api-reference/protocols/mqtt` 组件，它会创建优先级默认为 5 的任务（ :ref:`可配置 <CONFIG_MQTT_TASK_PRIORITY>` ），可通过 :ref:`CONFIG_MQTT_USE_CUSTOM_CONFIG` 调整，也可以在运行时通过 :cpp:class:`esp_mqtt_client_config_t` 结构体中的 ``task_prio`` 字段调整。
        - 关于 ``mDNS`` 服务的任务优先级，参见 `性能优化 <https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html#performance-optimization>`__ 。

.. only:: SOC_HP_CPU_HAS_MULTIPLE_CORES

    .. list::

        - :ref:`app-main-task` 中执行 app_main 函数的主任务优先级最低 (1) 且默认固定在核 0 上执行（ :ref:`可配置 <CONFIG_ESP_MAIN_TASK_AFFINITY>` ）。
        - 系统任务 :doc:`/api-reference/system/esp_timer` 用于管理定时器事件并执行回调函数，优先级较高 (22, ``ESP_TASK_TIMER_PRIO``) 且固定在核 0 上执行。
        - FreeRTOS 初始化调度器时会创建定时器任务，用于处理 FreeRTOS 定时器的回调函数，优先级最低（1， :ref:`可配置 <CONFIG_FREERTOS_TIMER_TASK_PRIORITY>` ）且固定在核 0 上执行。
        - 系统任务 :doc:`/api-reference/system/esp_event` 用于管理默认的系统事件循环并执行回调函数，优先级较高 (20, ``ESP_TASK_EVENT_PRIO``) 且固定在核 0 上执行。此配置仅在应用程序调用 :cpp:func:`esp_event_loop_create_default` 时使用。可以调用 :cpp:func:`esp_event_loop_create` 添加自定义任务配置。
        - :doc:`/api-guides/lwip` TCP/IP 任务优先级较高 (18, ``ESP_TASK_TCPIP_PRIO``) 且并未固定在特定内核上执行（ :ref:`可配置 <CONFIG_LWIP_TCPIP_TASK_AFFINITY>` ）。
        :SOC_WIFI_SUPPORTED: - :doc:`/api-guides/wifi` 任务优先级较高 (23) 且默认固定在核 0 上执行（ :ref:`可配置 <CONFIG_ESP_WIFI_TASK_CORE_ID>` ）。
        :SOC_WIFI_SUPPORTED: - 使用 Wi-Fi Protected Setup (WPS)、WPA2 EAP-TLS、Device Provisioning Protocol (DPP) 或 BSS Transition Management (BTM) 等功能时，Wi-Fi wpa_supplicant 组件可能会创建优先级较低的专用任务 (2)，这些任务并未固定在特定内核上执行。
        :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/controller_vhci` 任务优先级较高 (23, ``ESP_TASK_BT_CONTROLLER_PRIO``) 且默认固定在核 0 上执行（ :ref:`可配置 <{IDF_TARGET_CONTROLLER_CORE_CONFIG}>` ）。蓝牙控制器需要以低延迟响应请求，因此其任务应始终为最高优先级的任务之一并分配给单个 CPU 执行。
        :SOC_BT_SUPPORTED: - :doc:`/api-reference/bluetooth/nimble/index` 任务优先级较高 (21) 且默认固定在核 0 上执行（ :ref:`可配置 <CONFIG_BT_NIMBLE_PINNED_TO_CORE_CHOICE>` ）.
        :esp32: - 使用 :doc:`/api-reference/bluetooth/index` 时会创建多个任务:
               - 堆栈事件回调任务 ("BTC") 优先级较高 (19)。
               - 堆栈 BTU 层任务优先级较高 (20)。
               - Host HCI 主任务优先级较高 (22)。

               所有 Bluedroid 任务默认固定在同一个核心上执行，即核 0（ :ref:`可配置 <CONFIG_BT_BLUEDROID_PINNED_TO_CORE_CHOICE>` ）。

        - 以太网驱动程序会创建一个 MAC 任务，用于接收以太网帧。如果使用默认配置 ``ETH_MAC_DEFAULT_CONFIG`` ，则该任务为中高优先级 (15) 且并未固定在特定内核上执行。可以在以太网 MAC 初始化时输入自定义 :cpp:class:`eth_mac_config_t` 结构体来更改此设置。
        - 如果使用 :doc:`/api-reference/protocols/mqtt` 组件，它会创建优先级默认为 5 的任务（ :ref:`可配置 <CONFIG_MQTT_TASK_PRIORITY>` ，也可通过 :ref:`CONFIG_MQTT_USE_CUSTOM_CONFIG` 调整）。该任务未固定在特定内核上执行（ :ref:`可配置 <CONFIG_MQTT_TASK_CORE_SELECTION_ENABLED>` ）。
        - 关于 ``mDNS`` 服务的任务优先级，参见 `性能优化 <https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html#performance-optimization>`__ 。


设定应用程序任务优先级
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    .. only:: SOC_WIFI_SUPPORTED or SOC_BT_SUPPORTED or SOC_IEEE802154_SUPPORTED

        一般情况下，不建议将任务优先级设置得比内置的 {IDF_TARGET_RF_TYPE} 操作更高，因为这样可能会使 CPU 被长时间占用，导致系统不稳定。

    对于非常短、对时序要求严格且不涉及网络的操作，可以使用中断服务程序或是限制运行时间的最高优先级 (24) 任务。

    .. only:: SOC_WIFI_SUPPORTED or SOC_BT_SUPPORTED or SOC_IEEE802154_SUPPORTED

        将特定任务优先级设为 19，则较低层级的 {IDF_TARGET_RF_TYPE} 功能可以无延迟运行，且仍然会抢占 lwIP TCP/IP 堆栈以及其他非实时内部功能，这对于不执行网络操作的实时任务而言是最佳选项。

    lwIP TCP/IP 任务优先级 (18) 应高于所有执行 TCP/IP 网络操作的任务，从而避免优先级反转的问题。

.. only:: not SOC_HP_CPU_HAS_MULTIPLE_CORES

    默认配置下，除了个别例外，尤其是 lwIP TCP/IP 任务，大多数内置任务都固定在核 0 上执行。因此，应用程序可以方便地将高优先级任务放置在核 1 上执行。优先级大于等于 19 的应用程序任务在核 1 上运行时可以确保不会被任何内置任务抢占。为了进一步隔离各个 CPU 上运行的任务，配置 :ref:`lwIP 任务 <CONFIG_LWIP_TCPIP_TASK_AFFINITY>` ，可以使 lwIP 任务仅在核 0 上运行，而非其他内核，这可能会根据其他任务的运行情况减少总 TCP/IP 吞吐量。

    .. only:: SOC_WIFI_SUPPORTED or SOC_BT_SUPPORTED or SOC_IEEE802154_SUPPORTED

        一般情况下，不建议将核 0 上的任务优先级设置得比内置的 {IDF_TARGET_RF_TYPE} 操作更高，因为这样可能会使 CPU 被长时间占用，导致系统不稳定。选择优先级为 19 并在核 0 上运行可以使底层 {IDF_TARGET_RF_TYPE} 功能运行无延迟，但仍会抢占 lwIP TCP/IP 栈和其他不太关键的内部功能。这对于无需执行网络操作且时序要求高的任务来说是一个选择。执行 TCP/IP 网络操作的任何任务都应该以低于 lwIP TCP/IP 任务 (18) 的优先级运行，以避免优先级反转问题。

    .. note::

        如果要让特定任务始终先于 ESP-IDF 内置任务运行，并不需要将其固定在核 1 上。将该任务优先级设置为小于等于 17，则无需与内核绑定，那么核 0 上没有执行较高优先级的内置任务时，该任务也可以选择在核 0 上执行。使用未固定的任务可以提高整体 CPU 利用率，但这会增加任务调度的复杂性。

.. note::

    对内置 SPI flash 芯片进行写入操作时，任务会完全暂停执行。只有 :ref:`iram-safe-interrupt-handlers` 会继续执行。

提高中断性能
-------------------------------------

ESP-IDF 支持动态 :doc:`/api-reference/system/intr_alloc` 和中断抢占。系统中每个中断都有相应优先级，较高优先级的中断将优先执行。

只要其他任务不在临界区内，中断处理程序将优先于所有其他任务执行。因此，尽量减少中断处理程序的执行时间十分重要。

要以最佳性能执行特定中断处理程序，可以考虑：

.. list::

    - 调用 :cpp:func:`esp_intr_alloc` 时使用 ``ESP_INTR_FLAG_LEVEL2`` 或 ``ESP_INTR_FLAG_LEVEL3`` 等标志，可以为更重要的中断设定更高优先级。
    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - 将中断分配到不运行内置 {IDF_TARGET_RF_TYPE} 任务的 CPU 上执行，即默认情况下，将中断分配到核 1 上执行，参见 :ref:`built-in-task-priorities` 。调用 :cpp:func:`esp_intr_alloc`  函数即可将中断分配到函数所在 CPU。
    - 如果确定整个中断处理程序可以在 IRAM 中运行（参见 :ref:`iram-safe-interrupt-handlers` ），那么在调用 :cpp:func:`esp_intr_alloc` 分配中断时，请设置 ``ESP_INTR_FLAG_IRAM`` 标志，这样可以防止在应用程序固件写入内置 SPI flash 时临时禁用中断。
    - 即使是非 IRAM 安全的中断处理程序，如果需要频繁执行，可以考虑将处理程序的函数移到 IRAM 中，从而尽可能规避执行中断代码时发生 flash 缓存缺失的可能性（参见 :ref:`speed-targeted-optimizations` ）。如果可以确保只有部分处理程序位于 IRAM 中，则无需添加 ``ESP_INTR_FLAG_IRAM`` 标志将程序标记为 IRAM 安全。

.. _improve-network-speed:

提高网络速度
-----------------------------

.. list::

    :SOC_WIFI_SUPPORTED: * 关于提高 Wi-Fi 网速，参见 :ref:`How-to-improve-Wi-Fi-performance` 和 :ref:`wifi-buffer-usage` 。
    * 关于提高 lwIP TCP/IP 网速，参见 :ref:`lwip-performance` 。
    :SOC_WIFI_SUPPORTED: * 示例 :example:`wifi/iperf` 中的配置针对 Wi-Fi TCP/IP 吞吐量进行了大量优化，但该配置会占用更多 RAM。将文件 :example_file:`wifi/iperf/sdkconfig.defaults` 、 :example_file:`wifi/iperf/sdkconfig.defaults.{IDF_TARGET_PATH_NAME}` 和 :example_file:`wifi/iperf/sdkconfig.ci.99` 的内容追加到项目的 ``sdkconfig`` 文件中，即可添加所有相关选项。请注意，部分选项可能会导致可调试性降低、固件大小增加、内存使用增加或其他功能的性能降低等影响。为了获得最佳结果，请阅读上述链接文档，并据此确定哪些选项最适合当前应用程序。
    :SOC_EMAC_SUPPORTED: * 示例 :example:`ethernet/iperf` 中的配置针对以太网 TCP/IP 吞吐量进行了大量优化，但该配置会占用更多 RAM。如需了解详情，请查看 :example_file:`ethernet/iperf/sdkconfig.defaults`。请注意，部分选项可能会导致可调试性降低、固件大小增加、内存使用增加或其他功能的性能降低等影响。为了获得最佳结果，请阅读上述链接文档，并据此确定哪些选项最适合当前应用程序。

提高 I/O 性能
----------------------------------

使用标准 C 库函数，如 ``fread`` 和 ``fwrite``，相较于使用平台特定的不带缓冲系统调用，如 ``read`` 和 ``write``，可能会导致 I/O 性能下降。

``fread`` 与 ``fwrite`` 函数是为可移植性而设计的，而非速度，其缓冲性质会引入一些额外的开销。关于如何使用这两个函数，请参考示例 :example:`storage/fatfs/getting_started`。

与之相比，``read`` 与 ``write`` 函数是标准的 POSIX API，可直接通过 VFS 处理 FatFs，由 ESP-IDF 负责底层实现。关于如何使用这两个函数，请参考示例 :example:`storage/fatfs/fs_operations`。

下面提供了一些提示，更多信息请见 :doc:`/api-reference/storage/fatfs`。

.. list::

    - 读取/写入请求的最大大小等于 FatFS 簇大小（分配单元大小）。
    - 为了获得更好的性能，建议使用 ``read`` 和 ``write``，而非 ``fread`` 和 ``fwrite``。
    - 要提高诸如 ``fread`` 和 ``fgets`` 等缓冲读取函数的执行速度，可以增加文件缓冲区的大小。Newlib 的默认值为 128 字节，但可将其增加到 4096、8192 或 16384 字节。为此，可以使用 ``setvbuf`` 函数对特定文件指针进行局部设置，或者通过修改 :ref:`CONFIG_FATFS_VFS_FSTAT_BLKSIZE` 设置来进行全局修改。

        .. note::

            增加缓冲区的大小会增加堆内存的使用量。
