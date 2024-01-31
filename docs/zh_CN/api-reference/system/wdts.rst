看门狗
=========

:link_to_translation:`en:[English]`

概述
--------

ESP-IDF 支持以下类型的看门狗定时器：

.. list::

    - 硬件看门狗定时器
    - 中断看门狗定时器 (IWDT)
    - 任务看门狗定时器 (TWDT)
    :SOC_XT_WDT_SUPPORTED: - XTAL32K 看门狗定时器 (Crystal 32K 看门狗定时器，即 XTWDT)

中断看门狗负责确保 ISR（中断服务程序）不被长时间阻塞，TWDT 负责检测任务长时间运行而不让步的情况。

通过 :ref:`project-configuration-menu` 可启用各种看门狗定时器。其中，TWDT 也可以在程序运行时启用。

.. _app-hardware-watchdog-timers:

硬件看门狗定时器
----------------

芯片有两组看门狗定时器：

.. list::

    :not esp32c2: - 主系统看门狗定时器 (MWDT_WDT) - 用于中断看门狗定时器 (IWDT) 和任务看门狗定时器 (TWDT)。
    :esp32c2: - 主系统看门狗定时器 (MWDT_WDT) - 用于中断看门狗定时器 (IWDT)。
    - RTC 看门狗定时器 (RTC_WDT) - 用于跟踪从上电到执行用户主函数的启动时间（默认情况下，RTC 看门狗在执行用户主函数之前会被立即禁用）。

请参阅 :ref:`bootloader-watchdog` 小节，了解如何在引导加载程序中使用看门狗。

用户可以调整应用程序行为，使 RTC 看门狗在应用程序启动后保持启用状态。应用程序需要显式重置（即喂狗）或禁用看门狗，以避免芯片重置。具体而言，用户可设置 :ref:`CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE` 选项，根据需要修改应用程序并重新编译。此过程中应使用以下 API：

.. list::

    - :cpp:func:`wdt_hal_disable`：参考 :ref:`hw-abstraction-hal-layer-disable-rtc-wdt`
    - :cpp:func:`wdt_hal_feed`：参考 :ref:`hw-abstraction-hal-layer-feed-rtc-wdt`
    :esp32 or esp32s2: - :cpp:func:`rtc_wdt_feed`
    :esp32 or esp32s2: - :cpp:func:`rtc_wdt_disable`

如果未能及时重置或禁用 RTC_WDT，芯片将自动重置。请参阅 :ref:`RTC-Watchdog-Timeout` 了解更多信息。

中断看门狗定时器 (IWDT)
-------------------------------

{IDF_TARGET_IWDT_TIMER_GROUP:default="定时器组 1", esp32c2="定时器组 0"}

IWDT 的目的是，确保中断服务例程 (ISR) 运行不会受到长时间阻塞（即 IWDT 超时）。阻塞 ISR 及时运行会增加 ISR 延迟，也会阻止任务切换（因为任务切换是从 ISR 执行的）。阻止 ISR 运行的事项包括：

- 禁用中断
- 临界区（也会禁用中断）
- 其他相同或更高优先级的 ISR，在完成前会阻止相同或较低优先级的 ISR

IWDT 利用 {IDF_TARGET_IWDT_TIMER_GROUP} 中的 MWDT_WDT 看门狗定时器作为其底层硬件定时器，并在每个 CPU 上使用 FreeRTOS 时钟滴答中断，即 tick 中断。如果某个 CPU 上的 tick 中断没有在 IWDT 超时前运行，就表明该 CPU 上的 ISR 运行受阻（参见上文原因列表）。

当 IWDT 超时后，默认操作是调用紧急处理程序 (Panic Handler)，并显示 出错原因（ ``Interrupt wdt timeout on CPU0`` 或 ``Interrupt wdt timeout on CPU1``，视情况而定）。根据紧急处理程序的配置行为（参见 :ref:`CONFIG_ESP_SYSTEM_PANIC`），用户可通过回溯、OpenOCD、gdbstub 等来调试 IWDT 超时问题，也可以重置芯片（这在生产环境中可能是首选）。

如果出于某种原因，IWDT 超时后紧急处理程序无法运行，IWDT 还可以通过其二阶段超时来硬重置芯片（即系统重置）。

配置
^^^^^^^^^^^^^

- IWDT 默认通过 :ref:`CONFIG_ESP_INT_WDT` 选项启用。
- 通过 :ref:`CONFIG_ESP_INT_WDT_TIMEOUT_MS` 选项设置 IWDT 超时。

    - 注意，如果启用了 PSRAM 支持，那么默认的超时时间会更长，因为在某些情况下，临界区或中断例程访问大量 PSRAM 需要更长时间。
    - 超时时间至少应是 FreeRTOS tick 周期的两倍时长（参见 :ref:`CONFIG_FREERTOS_HZ`）。

调优
^^^^^^

如果 IWDT 超时是中断或临界区运行超时导致的，可以考虑重写代码：

- 临界区应尽可能短。任何非关键的代码或计算都应放在临界区外。
- 中断处理程序也应尽可能减少计算量。考虑让 ISR 使用队列向任务推送数据，从而将计算推迟到任务中进行。

临界区或中断处理程序都不应阻塞其他事件。如果不能或不希望通过更改代码减少处理时间，可以通过设置 :ref:`CONFIG_ESP_INT_WDT_TIMEOUT_MS` 延长超时时间。

.. _task-watchdog-timer:

任务看门狗定时器 (TWDT)
--------------------------

任务看门狗定时器 (TWDT) 用于监视特定任务，确保任务在配置的超时时间内执行。TWDT 主要监视每个 CPU 的空闲任务，但其他任务也可以订阅 TWDT 监视。通过监视每个 CPU 的空闲任务，TWDT 可以检测到任务长时间运行没有让出的情况。这可能表明代码编写不当，在外设上自旋循环，或者任务陷入了无限循环。

.. only:: not esp32c2

    TWDT 是基于定时器组 0 中的 MWDT_WDT 看门狗定时器构建的。超时发生时会触发中断。

.. only:: esp32c2

    {IDF_TARGET_NAME} 只有一个定时器组，由中断看门狗 (IWDT) 使用。因此，任务看门狗是基于 ``esp_timer`` 组件构建的，以实现软件定时器的作用。超时发生时会触发中断，并通知 ``esp_timer`` 的主任务，后者接到通知后会执行之前注册的 TWDT 回调。

可以在用户代码中定义函数 ``esp_task_wdt_isr_user_handler`` 来接收超时事件，并扩展默认行为。


使用
^^^^^

调用以下函数，用 TWDT 监视任务：

- :cpp:func:`esp_task_wdt_init` 初始化 TWDT 并订阅空闲任务。
- :cpp:func:`esp_task_wdt_add` 为其他任务订阅 TWDT。
- 订阅后，应从任务中调用 :cpp:func:`esp_task_wdt_reset` 来喂 TWDT。
- :cpp:func:`esp_task_wdt_delete()` 可以取消之前订阅的任务。
- :cpp:func:`esp_task_wdt_deinit()` 取消订阅空闲任务并反初始化 TWDT。


在需要更细粒度级别监视的情况下（即确保调用特定的函数、存根、代码路径），TWDT 允许订阅 ``users``。

- :cpp:func:`esp_task_wdt_add_user` 订阅 TWDT 的任意用户。此函数返回添加用户的用户句柄。
- 必须使用用户句柄调用 :cpp:func:`esp_task_wdt_reset_user`，防止 TWDT 超时。
- :cpp:func:`esp_task_wdt_delete_user` 取消订阅 TWDT 的任意用户。

配置
^^^^^^^^^^^^^

TWDT 的默认超时时间可以通过 :ref:`CONFIG_ESP_TASK_WDT_TIMEOUT_S` 配置项进行设置，并应至少设置为任何单个任务预计需要独占 CPU 的时长，例如某应用程序将进行长时间的密集计算且不让位给其他任务时的预计时长。也可以调用 :cpp:func:`esp_task_wdt_init`，在运行时更改此时间。

.. note::

    擦除较大的 flash 区域可能会非常耗时，并可能导致任务连续运行，触发 TWDT 超时。以下两种方法可以避免这种情况：

    - 在 menuconfig 中增加 :ref:`CONFIG_ESP_TASK_WDT_TIMEOUT_S`，延长看门狗超时时间。
    - 在擦除 flash 区域前，调用 :cpp:func:`esp_task_wdt_init` 增加看门狗超时时间。

    如需了解更多信息，请参考 :doc:`../peripherals/spi_flash/index`。

以下配置选项控制 TWDT 配置，默认情况下全部启用：

{IDF_TARGET_IDLE_TASK:default="空闲任务", esp32="CPU0 空闲任务", esp32s3="CPU0 空闲任务"}

.. list::

    - :ref:`CONFIG_ESP_TASK_WDT_EN` - 启用 TWDT 功能。如果禁用此选项， TWDT 即使运行时已初始化也无法使用。
    - :ref:`CONFIG_ESP_TASK_WDT_INIT` - TWDT 在启动期间自动初始化。禁用此选项时，仍可以调用 :cpp:func:`esp_task_wdt_init` 在运行时初始化 TWDT。
    - :ref:`CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0` - {IDF_TARGET_IDLE_TASK}在启动时订阅了 TWDT。如果此选项被禁用，仍可以调用 :cpp:func:`esp_task_wdt_init` 再次订阅。
    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - :ref:`CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1` - CPU1 空闲任务在启动时订阅了 TWDT。


.. note::

    如果 TWDT 超时，会默认在继续运行应用程序前打印警告和回溯。如希望超时触发系统严重错误和系统重置，可以通过 :ref:`CONFIG_ESP_TASK_WDT_PANIC` 进行配置。


.. only:: SOC_XT_WDT_SUPPORTED

    XTAL32K 看门狗定时器 (XTWDT)
    ------------------------------

    {IDF_TARGET_NAME} 的一个可选时钟输入是外部 32 kHz 无源晶振 (XTAL32K)，它常用作各种子系统（如 RTC）的时钟源 (``XTAL32K_CLK``)。

    XTWDT 是一个专用看门狗定时器，用于确保 XTAL32K 正常工作。如果 ``XTAL32K_CLK`` 是 ``RTC_SLOW_CLK`` 的时钟源，当它停止振荡时，XTWDT 会检测到并生成中断。XTWDT 还具有切换振荡器功能，可以自动切换到内部振荡器（准确度较低）作为 ``RTC_SLOW_CLK`` 的时钟源。

    由于切换到备用时钟是在硬件中完成的，因此切换也可以在 Deep-sleep 期间发生。这也说明，即使在芯片处于 Deep-sleep 并等待定时器超时时， ``XTAL32K_CLK`` 停止工作，芯片还是能按计划唤醒。

    如果 ``XTAL32K_CLK`` 重新开始正常工作，则可以调用 ``esp_xt_wdt_restore_clk`` 切换回时钟源，重新启用看门狗定时器。

    配置
    """""""""""""

    - 选择外部 32 KHz 晶体或振荡器时 (:ref:`CONFIG_RTC_CLK_SRC`)，通过 :ref:`CONFIG_ESP_XT_WDT` 配置选项启用 XTWDT。
    - 设置 :ref:`CONFIG_ESP_XT_WDT_TIMEOUT` 选项来配置超时时间。
    - 通过 :ref:`CONFIG_ESP_XT_WDT_BACKUP_CLK_ENABLE` 配置选项启用自动切换备用时钟功能。


JTAG & 看门狗
----------------

在使用 OpenOCD 进行调试时，CPU 会在每次达到断点时停止运行。然而，如果遇到断点后看门狗定时器继续运行，就会最终触发复位，为调试代码带来巨大的困难。因此， OpenOCD 会在每个断点处禁用中断和任务的看门狗的硬件定时器。此外，在离开断点时，OpenOCD 也不会重新启用定时器，也就是说，中断看门狗和任务看门狗实际上被禁用。当 {IDF_TARGET_NAME} 通过 JTAG 连接到 OpenOCD 时，看门狗不会打印任何警告或出现严重错误。


API 参考
-------------

任务看门狗
^^^^^^^^^^^^^

在 ESP-IDF 中使用任务看门狗的完整示例：:example:`system/task_watchdog`

.. include-build-file:: inc/esp_task_wdt.inc
