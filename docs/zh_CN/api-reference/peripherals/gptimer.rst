通用定时器
=====================

简介
-----------------

通用定时器是 {IDF_TARGET_NAME} 定时器组外设的驱动程序。{IDF_TARGET_NAME} 硬件定时器分辨率高，具有灵活的报警功能。定时器内部计数器达到特定目标数值的行为被称为定时器报警。定时器报警时将调用用户注册的不同定时器回调函数。

通用定时器通常在以下场景中使用：

-  如同挂钟一般自由运行，随时随地获取高分辨率时间戳；
-  生成周期性警报，定期触发事件；
-  生成一次性警报，在目标时间内响应。

功能概述
-----------------

下文介绍了配置和操作定时器的常规步骤：

.. list::

    - :ref:`gptimer-resource-allocation` - 获取定时器句柄应设置的参数，以及如何在通用定时器完成工作时回收资源。
    - :ref:`set-and-get-count-value` - 如何强制定时器从起点开始计数，以及如何随时获取计数值。
    - :ref:`set-up-alarm-action` - 启动警报事件应设置的参数。
    - :ref:`gptimer-register-event-callbacks` - 如何将用户的特定代码挂载到警报事件回调函数。
    - :ref:`enable-and-disable-timer` - 如何使能和禁用定时器。
    - :ref:`start-and-stop-timer` - 通过不同报警行为启动定时器的典型使用场景。
    :SOC_TIMER_SUPPORT_ETM: - :ref:`gptimer-etm-event-and-task` - 定时器提供了哪些事件和任务可以连接到 ETM 通道上。
    - :ref:`gptimer-power-management` - 选择不同的时钟源将会如何影响功耗。
    - :ref:`gptimer-iram-safe` - 在 cache 禁用的情况下，如何更好地让定时器处理中断事务以及实现 IO 控制功能。
    - :ref:`gptimer-thread-safety` - 驱动程序保证哪些 API 线程安全。
    - :ref:`gptimer-kconfig-options` - 支持的 Kconfig 选项，这些选项会对驱动程序行为产生不同影响。

.. _gptimer-resource-allocation:

资源分配
^^^^^^^^^^^^^^^^^^

不同的 ESP 芯片可能有不同数量的独立定时器组，每组内也可能有若干个独立定时器。[1]_

通用定时器实例由 :cpp:type:`gptimer_handle_t` 表示。可用硬件资源汇集在资源池内，由后台驱动程序管理，无需考虑硬件所属的定时器以及定时器组。

要安装一个定时器实例，需要提前提供配置结构体 :cpp:type:`gptimer_config_t`：

- :cpp:member:`gptimer_config_t::clk_src` 选择定时器的时钟源。:cpp:type:`gptimer_clock_source_t` 中列出多个可用时钟，仅可选择其中一个时钟。了解不同时钟源对功耗的影响，请查看章节 :ref:`gptimer-power-management`。
- :cpp:member:`gptimer_config_t::direction` 设置定时器的计数方向，:cpp:type:`gptimer_count_direction_t` 中列出多个支持的方向，仅可选择其中一个方向。
- :cpp:member:`gptimer_config_t::resolution_hz` 设置内部计数器的分辨率。计数器每滴答一次相当于 **1 / resolution_hz** 秒。
- :cpp:member:`gptimer_config::intr_priority` 设置中断的优先级。如果设置为 ``0``，则会分配一个默认优先级的中断，否则会使用指定的优先级。
- :cpp:member:`gptimer_config::backup_before_sleep` 用于使能在进入睡眠模式前备份 GPTimer 寄存器。这个选项需要用户在功耗和内存使用之间取得平衡。如果功耗不是一个问题，可以禁用这个选项来节省内存。但如果想要节省功耗，应该使能这个选项，在进入睡眠模式前备份 GPTimer 寄存器，并在唤醒后恢复它们。这个功能依赖于特定的硬件模块，如果你在不支持的芯片上启用它，你会得到一个错误信息，如 ``register back up is not supported``。
- 可选地， :cpp:member:`gptimer_config_t::intr_shared` 设置是否将定时器中断源标记为共享源。了解共享中断的优缺点，请参考 :doc:`Interrupt Handling <../../api-reference/system/intr_alloc>`。

完成上述结构配置之后，可以将结构传递给 :cpp:func:`gptimer_new_timer`，用以实例化定时器实例并返回定时器句柄。

该函数可能由于内存不足、参数无效等错误而失败。具体来说，当没有更多的空闲定时器（即所有硬件资源已用完）时，将返回 :c:macro:`ESP_ERR_NOT_FOUND`。可用定时器总数由 :c:macro:`SOC_TIMER_GROUP_TOTAL_TIMERS` 表示，不同的 ESP 芯片该数值不同。

如已不再需要之前创建的通用定时器实例，应通过调用 :cpp:func:`gptimer_del_timer` 回收定时器，以便底层硬件定时器用于其他目的。在删除通用定时器句柄之前，请通过 :cpp:func:`gptimer_disable` 禁用定时器，或者通过 :cpp:func:`gptimer_enable` 确认定时器尚未使能。

创建分辨率为 1 MHz 的通用定时器句柄
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

   gptimer_handle_t gptimer = NULL;
   gptimer_config_t timer_config = {
       .clk_src = GPTIMER_CLK_SRC_DEFAULT,
       .direction = GPTIMER_COUNT_UP,
       .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
   };
   ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

.. _set-and-get-count-value:

设置和获取计数值
^^^^^^^^^^^^^^^^^^^^^^^^^

创建通用定时器时，内部计数器将默认重置为零。计数值可以通过 :cpp:func:`gptimer_set_raw_count` 异步更新。最大计数值取决于硬件定时器的位宽，这也会在 SOC 宏 :c:macro:`SOC_TIMER_GROUP_COUNTER_BIT_WIDTH` 中有所反映。当更新活动定时器的原始计数值时，定时器将立即从新值开始计数。

计数值可以随时通过 :cpp:func:`gptimer_get_raw_count` 获取。

.. _set-up-alarm-action:

设置警报动作
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

对于大多数通用定时器使用场景而言，应在启动定时器之前设置警报动作，但不包括简单的挂钟场景，该场景仅需自由运行的定时器。设置警报动作，需要根据如何使用警报事件来配置 :cpp:type:`gptimer_alarm_config_t` 的不同参数：

-  :cpp:member:`gptimer_alarm_config_t::alarm_count` 设置触发警报事件的目标计数值。设置警报值时还需考虑计数方向。尤其是当 :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` 为 true 时，:cpp:member:`gptimer_alarm_config_t::alarm_count` 和 :cpp:member:`gptimer_alarm_config_t::reload_count` 不能设置为相同的值，因为警报值和重载值相同时没有意义。

-  :cpp:member:`gptimer_alarm_config_t::reload_count` 代表警报事件发生时要重载的计数值。此配置仅在 :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` 设置为 true 时生效。

-  :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` 标志设置是否使能自动重载功能。如果使能，硬件定时器将在警报事件发生时立即将 :cpp:member:`gptimer_alarm_config_t::reload_count` 的值重载到计数器中。

要使警报配置生效，需要调用 :cpp:func:`gptimer_set_alarm_action`。特别是当 :cpp:type:`gptimer_alarm_config_t` 设置为 ``NULL`` 时，报警功能将被禁用。

.. 注解::

    如果警报值已设置且定时器超过该值，则会立即触发警报。

.. _gptimer-register-event-callbacks:

注册事件回调函数
^^^^^^^^^^^^^^^^^^^^^^^^

定时器启动后，可动态产生特定事件（如“警报事件”）。如需在事件发生时调用某些函数，请通过 :cpp:func:`gptimer_register_event_callbacks` 将函数挂载到中断服务例程 (ISR)。:cpp:type:`gptimer_event_callbacks_t` 中列出了所有支持的事件回调函数：

-  :cpp:member:`gptimer_event_callbacks_t::on_alarm` 设置警报事件的回调函数。由于此函数在 ISR 上下文中调用，必须确保该函数不会试图阻塞（例如，确保仅从函数内调用具有 ``ISR`` 后缀的 FreeRTOS API）。函数原型在 :cpp:type:`gptimer_alarm_cb_t` 中有所声明。

也可以通过参数 ``user_data``，将自己的上下文保存到 :cpp:func:`gptimer_register_event_callbacks` 中。用户数据将直接传递给回调函数。

此功能将为定时器延迟安装中断服务，但不使能中断服务。所以，请在 :cpp:func:`gptimer_enable` 之前调用这一函数，否则将返回 :c:macro:`ESP_ERR_INVALID_STATE` 错误。了解详细信息，请查看章节 :ref:`enable-and-disable-timer`。

.. _enable-and-disable-timer:

使能和禁用定时器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在对定时器进行 IO 控制之前，需要先调用 :cpp:func:`gptimer_enable` 使能定时器。此函数功能如下：

* 此函数将把定时器驱动程序的状态从 **init** 切换为 **enable**。
* 如果 :cpp:func:`gptimer_register_event_callbacks` 已经延迟安装中断服务，此函数将使能中断服务。
* 如果选择了特定的时钟源（例如 APB 时钟），此函数将获取适当的电源管理锁。了解更多信息，请查看章节 :ref:`gptimer-power-management`。

调用 :cpp:func:`gptimer_disable` 会进行相反的操作，即将定时器驱动程序恢复到 **init** 状态，禁用中断服务并释放电源管理锁。

.. _start-and-stop-timer:

启动和停止定时器
^^^^^^^^^^^^^^^^

启动和停止是定时器的基本 IO 操作。调用 :cpp:func:`gptimer_start` 可以使内部计数器开始工作，而 :cpp:func:`gptimer_stop` 可以使计数器停止工作。下文说明了如何在存在或不存在警报事件的情况下启动定时器。

调用 :cpp:func:`gptimer_start` 将使驱动程序状态从 enable 转换为 run, 反之亦然。注意确保 start 和 stop 函数成对使用，否则，函数可能返回 :c:macro:`ESP_ERR_INVALID_STATE`。

将定时器作为挂钟启动
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    // Retrieve the timestamp at anytime
    uint64_t count;
    ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &count));

触发周期性事件
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    typedef struct {
        uint64_t event_count;
    } example_queue_element_t;

    static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
    {
        BaseType_t high_task_awoken = pdFALSE;
        QueueHandle_t queue = (QueueHandle_t)user_ctx;
        // Retrieve the count value from event data
        example_queue_element_t ele = {
            .event_count = edata->count_value
        };
        // Optional: send the event data to other task by OS queue
        // Don't introduce complex logics in callbacks
        // Suggest dealing with event data in the main loop, instead of in this callback
        xQueueSendFromISR(queue, &ele, &high_task_awoken);
        // return whether we need to yield at the end of ISR
        return high_task_awoken == pdTRUE;
    }

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0, // counter will reload with 0 on alarm event
        .alarm_count = 1000000, // period = 1s @resolution 1MHz
        .flags.auto_reload_on_alarm = true, // enable auto-reload
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb, // register user callback
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

触发一次性事件
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    typedef struct {
        uint64_t event_count;
    } example_queue_element_t;

    static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
    {
        BaseType_t high_task_awoken = pdFALSE;
        QueueHandle_t queue = (QueueHandle_t)user_ctx;
        // Stop timer the sooner the better
        gptimer_stop(timer);
        // Retrieve the count value from event data
        example_queue_element_t ele = {
            .event_count = edata->count_value
        };
        // Optional: send the event data to other task by OS queue
        xQueueSendFromISR(queue, &ele, &high_task_awoken);
        // return whether we need to yield at the end of ISR
        return high_task_awoken == pdTRUE;
    }

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1 * 1000 * 1000, // alarm target = 1s @resolution 1MHz
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb, // register user callback
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

警报值动态更新
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

通过更改 :cpp:member:`gptimer_alarm_event_data_t::alarm_value`，可以在 ISR 程序回调中动态更新警报值。警报值将在回调函数返回后更新。

.. code:: c

    typedef struct {
        uint64_t event_count;
    } example_queue_element_t;

    static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
    {
        BaseType_t high_task_awoken = pdFALSE;
        QueueHandle_t queue = (QueueHandle_t)user_data;
        // Retrieve the count value from event data
        example_queue_element_t ele = {
            .event_count = edata->count_value
        };
        // Optional: send the event data to other task by OS queue
        xQueueSendFromISR(queue, &ele, &high_task_awoken);
        // reconfigure alarm value
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = edata->alarm_value + 1000000, // alarm in next 1s
        };
        gptimer_set_alarm_action(timer, &alarm_config);
        // return whether we need to yield at the end of ISR
        return high_task_awoken == pdTRUE;
    }

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 1000000, // initial alarm target = 1s @resolution 1MHz
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_on_alarm_cb, // register user callback
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer, &alarm_config));


.. only:: SOC_TIMER_SUPPORT_ETM

    .. _gptimer-etm-event-and-task:

    ETM 事件与任务
    ^^^^^^^^^^^^^^

    定时器可以产生多种事件，这些事件可以连接到 :doc:`ETM </api-reference/peripherals/etm>` 模块。:cpp:type:`gptimer_etm_event_type_t` 中列出了定时器能够产生的事件类型。用户可以通过调用 :cpp:func:`gptimer_new_etm_event` 来获得相应事件的 ETM event 句柄。同样地，定时器还公开了一些可被其他事件触发然后自动执行的任务。:cpp:type:`gptimer_etm_task_type_t` 中列出了定时器能够支持的任务类型。 用户可以通过调用 :cpp:func:`gptimer_new_etm_task` 来获得相应任务的 ETM task 句柄。

    关于如何将定时器事件和任务连接到 ETM 通道中，请参阅 :doc:`ETM </api-reference/peripherals/etm>` 文档。

    .. _gptimer-power-management:

.. only:: not SOC_TIMER_SUPPORT_ETM

    .. _gptimer-power-management:

电源管理
^^^^^^^^

当电源管理 :ref:`CONFIG_PM_ENABLE` 被启用的时候，系统在进入睡眠前可能会调整或禁用时钟源。结果导致 GPTimer 的计时不准确。

驱动程序可以通过创建一个电源管理锁来防止上述问题。锁的类型会根据不同的时钟源来设置。驱动程序将在 :cpp:func:`gptimer_enable` 中拿锁，并在 :cpp:func:`gptimer_disable` 中释放锁。这意味着，在这两个函数之间，定时器可以正确工作，因为此时时钟源不会被禁用或改变频率。

.. only:: SOC_TIMER_SUPPORT_SLEEP_RETENTION

    除了时钟源的潜在变化外，当启用电源管理时，系统还可以关闭 GPTimer 寄存器所在的电源域。为确保 GPTimer 驱动程序在睡眠后继续工作，用户要么选择将 GPTimer 相关的寄存器备份到 RAM 中，要么拒绝关闭电源域。你可以根据应用需求在 :cpp:member:`gptimer_config_t::backup_before_sleep` 中设置是否需要启用寄存器备份，在功耗和内存使用之间做权衡。

.. _gptimer-iram-safe:

IRAM 安全
^^^^^^^^^^^^^^^^^^

默认情况下，当 cache 因写入或擦除 flash 等原因而被禁用时，通用定时器的中断服务将会延迟，造成警报中断无法及时执行。在实时应用程序中通常需要避免这一情况发生。

调用 Kconfig 选项 :ref:`CONFIG_GPTIMER_ISR_IRAM_SAFE` 可实现如下功能：

-  即使禁用 cache 也可使能正在运行的中断
-  将 ISR 使用的所有函数放入 IRAM [2]_
-  将驱动程序对象放入 DRAM（以防意外映射到 PSRAM）

这将允许中断在 cache 禁用时运行，但会增加 IRAM 使用量。

调用另一 Kconfig 选项 :ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` 也可将常用的 IO 控制功能放入 IRAM，以便这些函数在 cache 禁用时也能执行。常用的 IO 控制功能如下：

- :cpp:func:`gptimer_start`
- :cpp:func:`gptimer_stop`
- :cpp:func:`gptimer_get_raw_count`
- :cpp:func:`gptimer_set_raw_count`
- :cpp:func:`gptimer_set_alarm_action`

.. _gptimer-thread-safety:

线程安全
^^^^^^^^

驱动提供的所有 API 都是线程安全的。使用时，可以直接从不同的 RTOS 任务中调用此类函数，无需额外锁保护。以下这些函数还支持在中断上下文中运行。

- :cpp:func:`gptimer_start`
- :cpp:func:`gptimer_stop`
- :cpp:func:`gptimer_get_raw_count`
- :cpp:func:`gptimer_set_raw_count`
- :cpp:func:`gptimer_get_captured_count`
- :cpp:func:`gptimer_set_alarm_action`

.. _gptimer-kconfig-options:

Kconfig 选项
^^^^^^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` 控制着定时器控制函数的存放位置（IRAM 或 flash）。
- :ref:`CONFIG_GPTIMER_ISR_HANDLER_IN_IRAM` 控制着定时器中断处理函数的存放位置（IRAM 或 flash）。
- :ref:`CONFIG_GPTIMER_ISR_IRAM_SAFE` 控制着中断处理函数是否需要在 cache 关闭的时候被屏蔽掉。更多信息，请参阅 :ref:`gptimer-iram-safe`。
- :ref:`CONFIG_GPTIMER_ENABLE_DEBUG_LOG` 用于启用调试日志输出。启用这一选项将增加固件二进制文件大小。

应用示例
------------------

.. list::

    - 示例 :example:`peripherals/timer_group/gptimer` 中列出了通用定时器的典型用例。
    :SOC_TIMER_SUPPORT_ETM: - 示例 :example:`peripherals/timer_group/gptimer_capture_hc_sr04` 展示了如何在 ETM 模块的帮助下，用定时器捕获外部事件的时间戳。

API 参考
-------------------

.. include-build-file:: inc/gptimer.inc
.. include-build-file:: inc/gptimer_types.inc
.. include-build-file:: inc/timer_types.inc

.. only:: SOC_TIMER_SUPPORT_ETM

    .. include-build-file:: inc/gptimer_etm.inc

.. [1]
   不同 ESP 芯片系列的通用定时器实例数量可能不同。了解详细信息，请参考《{IDF_TARGET_NAME} 技术参考手册》 > 章节定时器组 (TIMG) [`PDF <{IDF_TARGET_TRM_CN_URL}#timg>`__]。驱动程序对通道申请数量不做限制，但当硬件资源用尽时，驱动程序将返回错误。在分配资源时，请务必检查返回值（例如 :cpp:func:`gptimer_new_timer`）。

.. [2]
   :cpp:member:`gptimer_event_callbacks_t::on_alarm` 回调函数和这一函数调用的函数也需放在 IRAM 中，请自行处理。
