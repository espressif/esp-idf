========================
通用硬件定时器 (GPTimer)
========================

:link_to_translation:`en:[English]`


本文介绍了 ESP-IDF 中的通用硬件定时器驱动的功能，章节目录如下：

.. contents::
    :local:
    :depth: 2

概述
====

通用定时器是 {IDF_TARGET_NAME} [`定时器组外设 <{IDF_TARGET_TRM_CN_URL}#timg>`__]的专用驱动程序。该定时器可以选择不同的时钟源和分频系数，能满足纳秒级的分辨率要求。此外，它还具有灵活的超时报警功能，并允许在报警时刻自动更新计数值，从而实现非常精准的定时周期。

基于硬件定时器的 **高分辨率、高计数范围和高响应** 的能力，该驱动的主要应用场景包括：

- 作为自由运行的挂历时钟，给其他模块提供时间戳服务
- 产生周期性警报，完成周期性任务
- 产生一次性警报，配合警报值的异步更新，可实现单调型的软件定时器链表
- 配合 GPIO 模块，可以实现 PWM 信号输出和输入捕获
- 其他

快速入门
========

本节将带你快速了解如何使用 GPTimer 驱动。通过简单的示例，展示如何创建一个定时器并启动它，如何设置警报事件，以及如何注册事件回调函数。一般的使用流程如下：

.. blockdiag::
    :scale: 100%
    :caption: GPTimer 驱动的一般使用流程（点击图片查看大图）
    :align: center

    blockdiag {
        default_fontsize = 14;
        node_width = 250;
        node_height = 80;
        class emphasis [color = pink, style = dashed];

        create [label="gptimer_new_timer"];
        config [label="gptimer_set_alarm_action \n gptimer_register_event_callbacks"];
        enable [label="gptimer_enable"];
        start [label="gptimer_start"];
        running [label="Timer Running", class="emphasis"]
        stop [label="gptimer_stop"];
        disable [label="gptimer_disable"];
        cleanup [label="gptimer_delete_timer"];

        create -> config -> enable -> start -> running -> stop -> disable -> cleanup;
        enable -> start [folded];
        stop -> disable [folded];
    }

创建和启动定时器
----------------

首先，我们需要创建一个定时器实例。以下代码展示了如何创建一个分辨率为 1 MHz 的定时器：

.. code:: c

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT, // 选择默认的时钟源
        .direction = GPTIMER_COUNT_UP,      // 计数方向为向上计数
        .resolution_hz = 1 * 1000 * 1000,   // 分辨率为 1 MHz，即 1 次滴答为 1 微秒
    };
    // 创建定时器实例
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    // 使能定时器
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // 启动定时器
    ESP_ERROR_CHECK(gptimer_start(gptimer));

当创建定时器实例时，我们需要通过 :cpp:type:`gptimer_config_t` 配置时钟源、计数方向和分辨率等参数。这些参数将决定定时器的工作方式。然后调用 :cpp:func:`gptimer_new_timer` 函数创建一个新的定时器实例，该函数将返回一个指向新实例的句柄。定时器的句柄实际上是一个指向定时器内存对象的指针，类型为 :cpp:type:`gptimer_handle_t`。

以下是 :cpp:type:`gptimer_config_t` 结构体的其他配置参数及其解释：

- :cpp:member:`gptimer_config_t::clk_src` 选择定时器的时钟源。可用时钟源列在 :cpp:type:`gptimer_clock_source_t` 中，只能选择其中一个。不同的时钟源会在分辨率，精度和功耗上有所不同。
- :cpp:member:`gptimer_config_t::direction` 设置定时器的计数方向。支持的方向列在 :cpp:type:`gptimer_count_direction_t` 中，只能选择其中一个。
- :cpp:member:`gptimer_config_t::resolution_hz` 设置内部计数器的分辨率。计数器每滴答一次相当于 **1 / resolution_hz** 秒。
- :cpp:member:`gptimer_config_t::intr_priority` 设置中断的优先级。如果设置为 ``0``，则会分配一个默认优先级的中断，否则会使用指定的优先级。
- :cpp:member:`gptimer_config_t::flags` 通常用来微调驱动的一些行为，包括以下选项：

    - :cpp:member:`gptimer_config_t::flags::allow_pd` 配置驱动程序是否允许系统在睡眠模式下关闭外设电源。在进入睡眠之前，系统将备份 GPTimer 寄存器上下文，当系统从睡眠唤醒时时，这些上下文将被恢复。请注意，关闭外设可以节省功耗，但会消耗更多内存来保存寄存器上下文。你需要在功耗和内存消耗之间做权衡。此配置选项依赖于特定的硬件功能，如果在不支持的芯片上启用它，你将看到类似 ``not able to power down in light sleep`` 的错误消息。

.. note::

    请注意，如果当前芯片中所有的硬件定时器都已经被申请使用，那么 :cpp:func:`gptimer_new_timer` 将返回 :c:macro:`ESP_ERR_NOT_FOUND` 错误。

定时器在启动前必须要先使能，使能函数 :cpp:func:`gptimer_enable` 可以将驱动的内部状态机切换到激活状态，这里面还会包括一些系统性服务的申请/注册等工作，如申请电源管理锁。与使能函数相对应的是禁用函数 :cpp:func:`gptimer_disable`，它会释放所有的系统性服务。

.. note::

    调用 :cpp:func:`gptimer_enable` 和 :cpp:func:`gptimer_disable` 函数时，需要成对使用。这意味着，你不能连续调用两次 :cpp:func:`gptimer_enable` 或 :cpp:func:`gptimer_disable` 函数。这种成对调用的原则确保了资源的正确管理和释放。

:cpp:func:`gptimer_start` 函数用于启动定时器。启动后,定时器将开始计数,并在计数到达最大值或者最小值时（取决于计数方向）自动溢出,从0开始重新计数。
:cpp:func:`gptimer_stop` 函数用于停止定时器。请注意，停止一个定时器并不会将计数器当前的值清零。如果想清零计数器，需要使用后面介绍的函数 :cpp:func:`gptimer_set_raw_count`。
:cpp:func:`gptimer_start` 和 :cpp:func:`gptimer_stop` 函数遵循幂等原则。这意味着，如果定时器已经启动，再次调用 :cpp:func:`gptimer_start` 函数不会产生任何效果。同样，如果定时器已经停止，再次调用 :cpp:func:`gptimer_stop` 函数也不会产生任何效果。

.. note::

    但是请注意，当定时器处于启动的 **中间状态** 时（启动开始了，但还没有启动完毕），此时如果另外一个线程调用 :cpp:func:`gptimer_start` 或者 :cpp:func:`gptimer_stop` 函数，则会返回 :c:macro:`ESP_ERR_INVALID_STATE` 错误，避免触发不确定的行为。

设置和获取计数值
----------------

一个刚创建的定时器，其内部计数器值默认为 0。你可以通过 :cpp:func:`gptimer_set_raw_count` 设置其他的计数值。最大计数值取决于硬件定时器的位宽（通常不少于 ``54 bit``）。

.. note::

    如果定时器已经处于启动状态，:cpp:func:`gptimer_set_raw_count` 会让定时器立即跳到新值处开始计数。

:cpp:func:`gptimer_get_raw_count` 函数用于获取定时器的当前计数值。这个计数值是定时器从启动以来所累积的计数（假设是从 0 开始启动的话），请注意，返回的数值还没有经过任何单位转换，是一个纯粹的计数值。你需要根据定时器的实际分辨率来把计数值转换成时间单位。定时器的分辨率可以通过 :cpp:func:`gptimer_get_resolution` 函数来获取。

.. code:: c

    // 查看定时器的分辨率
    uint32_t resolution_hz;
    ESP_ERROR_CHECK(gptimer_get_resolution(gptimer, &resolution_hz));
    // 读取当前计数值
    uint64_t count;
    ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &count));
    // （可选的）将计数值转换成时间单位 (秒)
    double time = (double)count / resolution_hz;

触发周期性警报事件
------------------

除了时间戳功能以外，通用定时器还支持警报功能。以下代码展示了如何设置一个周期性警报，每秒触发一次：

.. code-block:: c
    :emphasize-lines: 10-32

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT, // 选择默认的时钟源
        .direction = GPTIMER_COUNT_UP,      // 计数方向为向上计数
        .resolution_hz = 1 * 1000 * 1000,   // 分辨率为 1 MHz，即 1 次滴答为 1 微秒
    };
    // 创建定时器实例
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
    {
        // 处理事件回调的一般流程：
        // 1. 从 user_ctx 中拿到用户上下文数据（需事先从 gptimer_register_event_callbacks 中传入）
        // 2. 从 edata 中获取警报事件数据，比如 edata->count_value
        // 3. 执行用户自定义操作
        // 4. 返回上述操作期间是否有高优先级的任务被唤醒了，以便通知调度器做切换任务
        return false;
    }

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,      // 当警报事件发生时，定时器会自动重载到 0
        .alarm_count = 1000000, // 设置实际的警报周期，因为分辨率是 1us，所以 1000000 代表 1s
        .flags.auto_reload_on_alarm = true, // 使能自动重载功能
    };
    // 设置定时器的警报动作
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb, // 当警报事件发生时，调用用户回调函数
    };
    // 注册定时器事件回调函数，允许携带用户上下文
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    // 使能定时器
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // 启动定时器
    ESP_ERROR_CHECK(gptimer_start(gptimer));

:cpp:func:`gptimer_set_alarm_action` 函数用于配置定时器的警报动作。当定时器计数值达到指定的警报值时，将发出警报事件。用户可以选择在警报事件发生时自动重载预设的计数值，从而实现周期性警报。

以下是 :cpp:type:`gptimer_alarm_config_t` 结构体中的每个必要成员项的作用，通过配置这些参数，用户可以灵活地控制定时器的警报行为，以满足不同的应用需求。

- :cpp:member:`gptimer_alarm_config_t::alarm_count` 设置触发警报事件的目标计数值。当定时器计数值达到该值时，将触发警报事件。设置警报值的时候也需要考虑定时器的计数方向，如果当前计数值已经 **越过** 了警报值，那么警报事件会立刻触发。
- :cpp:member:`gptimer_alarm_config_t::reload_count` 设置警报事件发生时要自动重载的计数值。此配置仅在 :cpp:member:`gptimer_alarm_config_t::flags::auto_reload_on_alarm` 标志为 ``true`` 时生效。实际的警报周期将会由 ``|alarm_count - reload_count|`` 决定。从实际应用触发，不建议将警报周期设置成小于 5us。

.. note::

    特别地， ``gptimer_set_alarm_action(gptimer, NULL);`` 表示关闭定时器的警报功能。

:cpp:func:`gptimer_register_event_callbacks` 函数用于注册定时器事件的回调函数。当定时器触发特定事件（如警报事件）时，将调用用户定义的回调函数。用户可以在回调函数中执行自定义操作，例如发送信号，从而实现更灵活的事件处理机制。由于回调函数是在中断上下文中执行的，因此在回调函数中应该避免执行复杂的操作（包括任何可能导致阻塞的操作），以免影响系统的实时性。:cpp:func:`gptimer_register_event_callbacks` 还允许用户传递一个上下文指针，以便在回调函数中访问用户定义的数据。

GPTimer 支持的事件回调函数有下面这些：

- :cpp:type:`gptimer_alarm_cb_t` 警报事件回调函数，它有一个配套的数据结构 :cpp:type:`gptimer_alarm_event_data_t`，用于传递警报事件的相关数据：
  - :cpp:member:`gptimer_alarm_event_data_t::alarm_value` 保存的是警报值，即触发警报事件的目标计数值。
  - :cpp:member:`gptimer_alarm_event_data_t::count_value` 保存的是警报发生后，进入中断处理器时的计数值。该值会不同于警报值，因为中断处理器会带来一定的延迟，并且计数值在警报发生时可能已经被自动重载了。

.. note::

    请务必在调用 :cpp:func:`gptimer_enable` 之前注册回调函数，否则定时器事件将无法正确触发中断服务。

触发一次性警报事件
------------------

还有一些应用场景只需要触发一次警报中断，以下代码展示了如何设置一个一次性警报，在 1 秒后触发：

.. code-block:: c
    :emphasize-lines: 12-13,24

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT, // 选择默认的时钟源
        .direction = GPTIMER_COUNT_UP,      // 计数方向为向上计数
        .resolution_hz = 1 * 1000 * 1000,   // 分辨率为 1 MHz，即 1 次滴答为 1 微秒
    };
    // 创建定时器实例
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
    {
        // 这里只是演示如何在警报第一次发生时让定时器停止工作
        gptimer_stop(timer);
        // 处理事件回调的一般流程：
        // 1. 从 user_ctx 中拿到用户上下文数据（需事先从 gptimer_register_event_callbacks 中传入）
        // 2. 从 edata 中获取警报事件数据，比如 edata->count_value
        // 3. 执行用户自定义操作
        // 4. 返回上述操作期间是否有高优先级的任务被唤醒了，以便通知调度器做切换任务
        return false;
    }

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000000, // 设置实际的警报周期，因为分辨率是 1us，所以 1000000 代表 1s
        .flags.auto_reload_on_alarm = false; // 关闭自动重载功能
    };
    // 设置定时器的警报动作
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb, // 当警报事件发生时，调用用户回调函数
    };
    // 注册定时器事件回调函数，允许携带用户上下文
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    // 使能定时器
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // 启动定时器
    ESP_ERROR_CHECK(gptimer_start(gptimer));

与周期性警报不同，上述代码在配置警报行为时关闭了自动重载功能。这意味着，当警报事件发生后，定时器将不会自动重载到预设的计数值，而是继续计数直到溢出。如果希望定时器在警报后立即停止，可以在回调函数中调用 :cpp:func:`gptimer_stop`。

资源回收
--------

当不再需要使用定时器时，应该调用 :cpp:func:`gptimer_delete_timer` 函数来释放软硬件资源。删除前请确保定时器已经处于停止状态。

进阶功能
========

在了解了基本用法后，我们可以进一步探索 GPTimer 驱动的更多玩法。

动态更新警报值
--------------

GPTimer 驱动支持在中断回调函数中调用 :cpp:func:`gptimer_set_alarm_action` 函数来动态更新警报值，从而实现单调型的软件定时器链表。以下代码展示了如何在警报事件发生时，重新设置下一次警报的触发时间：

.. code-block:: c
    :emphasize-lines: 12-16

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT, // 选择默认的时钟源
        .direction = GPTIMER_COUNT_UP,      // 计数方向为向上计数
        .resolution_hz = 1 * 1000 * 1000,   // 分辨率为 1 MHz，即 1 次滴答为 1 微秒
    };
    // 创建定时器实例
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
    {
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = edata->alarm_value + 1000000, // 下一次警报在当前警报的基础上加 1s
        };
        // 更新警报值
        gptimer_set_alarm_action(timer, &alarm_config);
        return false;
    }

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000000, // 设置实际的警报周期，因为分辨率是 1us，所以 1000000 代表 1s
        .flags.auto_reload_on_alarm = false; // 关闭自动重载功能
    };
    // 设置定时器的警报动作
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb, // 当警报事件发生时，调用用户回调函数
    };
    // 注册定时器事件回调函数，允许携带用户上下文
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    // 使能定时器
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    // 启动定时器
    ESP_ERROR_CHECK(gptimer_start(gptimer));

.. only:: SOC_TIMER_SUPPORT_ETM and SOC_ETM_SUPPORTED

    .. _gptimer-etm-event-and-task:

    GPTimer 的 ETM 事件与任务
    -------------------------

    GPTimer 可以生成多种事件，这些事件可以连接到 :doc:`ETM </api-reference/peripherals/etm>` 模块。事件类型列在 :cpp:type:`gptimer_etm_event_type_t` 中。用户可以通过调用 :cpp:func:`gptimer_new_etm_event` 来创建 ``ETM event`` 句柄。
    GPTimer 还支持一些可由其他事件触发并自动执行的任务。任务类型列在 :cpp:type:`gptimer_etm_task_type_t` 中。用户可以通过调用 :cpp:func:`gptimer_new_etm_task` 来创建 ``ETM task`` 句柄。

    有关如何将定时器事件和任务连接到 ETM 通道，请参阅 :doc:`ETM </api-reference/peripherals/etm>` 文档。

关于低功耗
----------

当启用电源管理 :ref:`CONFIG_PM_ENABLE` 时，系统在进入睡眠模式前可能会调整或禁用时钟源，从而导致 GPTimer 的计时出错。

为了防止这种情况发生， GPTimer 驱动内部创建了一个电源管理锁。当调用 :cpp:func:`gptimer_enable` 函数后，该锁将被激活，确保系统不会进入睡眠模式，从而保持定时器的正确工作。如果需要降低功耗，可以调用 :cpp:func:`gptimer_disable` 函数来释放电源管理锁，使系统能够进入睡眠模式。但是，这样做会导致定时器停止计数，因此在唤醒后需要重新启动定时器。

.. only:: SOC_TIMER_SUPPORT_SLEEP_RETENTION

    除了关闭时钟源外，系统在进入睡眠模式时还可以关闭 GPTimer 的电源以进一步降低功耗。要实现这一点，需要将 :cpp:member:`gptimer_config_t::allow_pd` 设置为 ``true``。在系统进入睡眠模式之前， GPTimer 的寄存器上下文会被备份到内存中，并在系统唤醒后恢复。请注意，启用此选项虽然可以降低功耗，但会增加内存的使用量。因此，在使用该功能时需要在功耗和内存消耗之间进行权衡。

关于线程安全
------------

驱动使用了临界区保证了对寄存器的原子操作。句柄内部的关键成员也受临界区保护。驱动内部的状态机使用了原子指令保证了线程安全，通过状态检查还能进一步防止一些不合法的并发操作（例如 `start` 和 `stop` 冲突）。因此， GPTimer 驱动的 API 可以在多线程环境下使用，无需自行加锁。

同时，以下这些函数还允许在中断上下文中使用：

.. list::

    - :cpp:func:`gptimer_start`
    - :cpp:func:`gptimer_stop`
    - :cpp:func:`gptimer_get_raw_count`
    - :cpp:func:`gptimer_set_raw_count`
    - :cpp:func:`gptimer_get_captured_count`
    - :cpp:func:`gptimer_set_alarm_action`

关于 Cache 安全
---------------

在文件系统进行 Flash 读写操作时，为了避免 Cache 从 Flash 加载指令和数据时出现错误，系统会暂时禁用 Cache 功能。这会导致 GPTimer 的中断处理程序在此期间无法响应，从而使用户的回调函数无法及时执行。如果希望在 Cache 被禁用期间，中断处理程序仍能正常运行，可以启用 :ref:`CONFIG_GPTIMER_ISR_CACHE_SAFE` 选项。

.. note::

    请注意，在启用该选项后，所有的中断回调函数及其上下文数据 **必须存放在内部存储空间** 中。因为在 Cache 被禁用时，系统无法从 Flash 中加载数据和指令。

关于性能
--------

为了提升中断处理的实时响应能力， GPTimer 驱动提供了 :ref:`CONFIG_GPTIMER_ISR_HANDLER_IN_IRAM` 选项。启用该选项后，中断处理程序将被放置在内部 RAM 中运行，从而减少了从 Flash 加载指令时可能出现的缓存丢失带来的延迟。

.. note::

    但是，中断处理程序调用的用户回调函数和用户上下文数据仍然可能位于 Flash 中，缓存缺失的问题还是会存在，这需要用户自己将回调函数和数据放入内部 RAM 中，比如使用 :c:macro:`IRAM_ATTR` 和 :c:macro:`DRAM_ATTR`。

前文还提到， GPTimer 驱动允许部分函数在中断上下文中使用。:ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` 选项可以将这些函数放入 IRAM 中，一来，可以避免缓存缺失带来的性能损失，二来，这些函数在 Cache 关闭期间也能使用。

其他 Kconfig 选项
-----------------

- :ref:`CONFIG_GPTIMER_ENABLE_DEBUG_LOG` 选项允许强制启用 GPTimer 驱动的所有调试日志，无论全局日志级别设置如何。启用此选项可以帮助开发人员在调试过程中获取更详细的日志信息，从而更容易定位和解决问题。

关于资源消耗
------------

使用 :doc:`/api-guides/tools/idf-size` 工具可以查看 GPTimer 驱动的代码和数据消耗。以下是测试前提条件（以 ESP32-C2 为例）：

- 编译器优化等级设置为 ``-Os``，以确保代码尺寸最小化。
- 默认日志等级设置为 ``ESP_LOG_INFO``，以平衡调试信息和性能。
- 关闭以下驱动优化选项：
    - :ref:`CONFIG_GPTIMER_ISR_HANDLER_IN_IRAM` - 中断处理程序不放入 IRAM。
    - :ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` - 控制函数不放入 IRAM。
    - :ref:`CONFIG_GPTIMER_ISR_CACHE_SAFE` - 不启用 Cache 安全选项。

**注意，以下数据不是精确值，仅供参考，在不同型号的芯片上，数据会有所出入。**

+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+
| Component Layer  | Total Size | DIRAM | .bss | .data | .text | Flash Code | .text | Flash Data | .rodata |
+==================+============+=======+======+=======+=======+============+=======+============+=========+
| soc              |          8 |     0 |    0 |     0 |     0 |          0 |     0 |          8 |       8 |
+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+
| hal              |        206 |     0 |    0 |     0 |     0 |        206 |   206 |          0 |       0 |
+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+
| driver           |       4251 |    12 |   12 |     0 |     0 |       4046 |  4046 |        193 |     193 |
+------------------+------------+-------+------+-------+-------+------------+-------+------------+---------+

此外，每一个 GPTimer 句柄会从 heap 中动态申请约 ``100`` 字节的内存。如果还使能了 :cpp:member:`gptimer_config_t::flags::allow_pd` 选项，那么每个定时器还会在睡眠期间额外消耗约 ``30`` 字节的内存用于保存寄存器上下文。

应用示例
========

.. list::

    - :example:`peripherals/timer_group/gptimer` 演示了如何在 ESP 芯片上使用通用定时器 API 生成周期性警报事件，触发不同的警报动作。
    - :example:`peripherals/timer_group/wiegand_interface` 使用两个定时器（一个在单次警报模式下，另一个在周期警报模式下），触发中断并在警报事件的回调函数中改变 GPIO 的输出状态，从而模拟出了 Wiegand 协议的输出波形。
    :SOC_TIMER_SUPPORT_ETM and SOC_ETM_SUPPORTED: - :example:`peripherals/timer_group/gptimer_capture_hc_sr04` 展示了如何使用通用定时器和事件任务矩阵(ETM)来精确捕获超声波传感器事件的时间戳，并据此换算成距离信息。

API 参考
========

GPTimer 驱动 API
-----------------

.. include-build-file:: inc/gptimer.inc

GPTimer 驱动类型
----------------

.. include-build-file:: inc/gptimer_types.inc

GPTimer HAL 类型
----------------

.. include-build-file:: inc/timer_types.inc

.. only:: SOC_TIMER_SUPPORT_ETM and SOC_ETM_SUPPORTED

    GPTimer ETM API
    ---------------

    .. include-build-file:: inc/gptimer_etm.inc
