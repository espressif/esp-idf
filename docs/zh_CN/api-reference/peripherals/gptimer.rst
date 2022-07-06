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

- :ref:`resource-allocation` - 获取定时器句柄应设置的参数，以及如何在通用定时器完成工作时回收资源。
- :ref:`set-and-get-count-value` - 如何强制定时器从起点开始计数，以及如何随时获取计数值。
- :ref:`set-up-alarm-action` - 启动警报事件应设置的参数。
- :ref:`register-event-callbacks` - 如何将用户的特定代码挂载到警报事件回调函数。
- :ref:`enable-and-disable-timer` - 如何使能和禁用定时器。
- :ref:`start-and-stop-timer` - 通过不同报警行为启动定时器的典型使用场景。
- :ref:`power-management` - 选择不同的时钟源将会如何影响功耗。
- :ref:`iram-safe` - 在 cache 禁用的情况下，如何更好地让定时器处理中断事务以及实现 IO 控制功能。
- :ref:`thread-safety` - 驱动程序保证哪些 API 线程安全。
- :ref:`kconfig-options` - 支持的 Kconfig 选项，这些选项会对驱动程序行为产生不同影响。

.. _resource-allocation:

资源分配
^^^^^^^^^^^^^^^^^^

不同的 ESP 芯片可能有不同数量的独立定时器组，每组内也可能有若干个独立定时器。[1]_

通用定时器实例由 :cpp:type:`gptimer_handle_t` 表示。后台驱动会在资源池中管理所有可用的硬件资源，这样您便无需考虑硬件所属的定时器以及定时器组。

要安装一个定时器实例，需要提前提供配置结构体 :cpp:type:`gptimer_config_t`：

-  :cpp:member:`gptimer_config_t::clk_src` 选择定时器的时钟源。:cpp:type:`gptimer_clock_source_t` 中列出多个可用时钟，仅可选择其中一个时钟。了解不同时钟源对功耗的影响，请查看章节 :ref:`power-management`。

-  :cpp:member:`gptimer_config_t::direction` 设置定时器的计数方向，:cpp:type:`gptimer_count_direction_t` 中列出多个支持的方向，仅可选择其中一个方向。

-  :cpp:member:`gptimer_config_t::resolution_hz` 设置内部计数器的分辨率。计数器每滴答一次相当于 **1 / resolution_hz** 秒。

-  选用 :cpp:member:`gptimer_config_t::intr_shared` 设置是否将定时器中断源标记为共享源。了解共享中断的优缺点，请参考 :doc:`Interrupt Handling <../../api-reference/system/intr_alloc>`。

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

.. _register-event-callbacks:

注册事件回调函数
^^^^^^^^^^^^^^^^^^^^^^^^

定时器启动后，可动态产生特定事件（如“警报事件”）。如需在事件发生时调用某些函数，请通过 :cpp:func:`gptimer_register_event_callbacks` 将函数挂载到中断服务例程 (ISR)。:cpp:type:`gptimer_event_callbacks_t` 中列出了所有支持的事件回调函数：

-  :cpp:member:`gptimer_event_callbacks_t::on_alarm` 设置警报事件的回调函数。由于此函数在 ISR 上下文中调用，必须确保该函数不会试图阻塞（例如，确保仅从函数内调用具有 ``ISR`` 后缀的 FreeRTOS API）。函数原型在 :cpp:type:`gptimer_alarm_cb_t` 中有所声明。

您也可以通过参数 ``user_data`` 将自己的上下文保存到 :cpp:func:`gptimer_register_event_callbacks` 中。用户数据将直接传递给回调函数。

此功能将为定时器延迟安装中断服务，但不使能中断服务。所以，请在 :cpp:func:`gptimer_enable` 之前调用这一函数，否则将返回 :c:macro:`ESP_ERR_INVALID_STATE` 错误。了解详细信息，请查看章节 :ref:`enable-and-disable-timer`。

.. _enable-and-disable-timer:

使能和禁用定时器
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

在对定时器进行 IO 控制之前，需要先调用 :cpp:func:`gptimer_enable` 使能定时器。此函数功能如下：

* 此函数将把定时器驱动程序的状态从 **init** 切换为 **enable**。
* 如果 :cpp:func:`gptimer_register_event_callbacks` 已经延迟安装中断服务，此函数将使能中断服务。
* 如果选择了特定的时钟源（例如 APB 时钟），此函数将获取适当的电源管理锁。了解更多信息，请查看章节 :ref:`power-management`。

调用 :cpp:func:`gptimer_disable` 会进行相反的操作，即将定时器驱动程序恢复到 **init** 状态，禁用中断服务并释放电源管理锁。

.. _start-and-stop-timer:

启动和停止定时器
^^^^^^^^^^^^^^^^

启动和停止是定时器的基本 IO 操作。调用 :cpp:func:`gptimer_start` 可以使内部计数器开始工作，而 :cpp:func:`gptimer_stop` 可以使计数器停止工作。下文说明了如何在存在或不存在警报事件的情况下启动定时器。

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

.. _power-management:

电源管理
^^^^^^^^^^^^^^^^^

当使能电源管理时（即 :ref:`CONFIG_PM_ENABLE` 已打开），系统将在进入 Light-sleep 模式之前调整 APB 频率，从而可能会改变通用定时器的计数步骤周期，导致计时不准确。

然而，驱动程序可以通过获取类型为 :cpp:enumerator:`ESP_PM_APB_FREQ_MAX` 的电源管理锁来阻止系统更改 APB 频率。每当驱动程序创建一个通用定时器实例，且该实例选择 :cpp:enumerator:`GPTIMER_CLK_SRC_APB` 作为其时钟源的时，驱动程序会确保在通过 :cpp:func:`gptimer_enable` 使能定时器时，已经获取了电源管理锁。同样，当为该定时器调用 :cpp:func:`gptimer_disable` 时，驱动程序会释放电源管理锁。

如果选择 :cpp:enumerator:`GPTIMER_CLK_SRC_XTAL` 等其他时钟源，那么驱动程序不会安装电源管理锁。只要时钟源仍可提供足够的分辨率，XTAL 时钟源就更适合低功耗应用。

.. _iram-safe:

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

.. _thread-safety:

线程安全
^^^^^^^^^^^^^^^^^^

驱动程序会保证工厂函数 :cpp:func:`gptimer_new_timer` 的线程安全，这意味着您可以从不同的 RTOS 任务中调用这一函数，而无需额外的锁保护。

由于驱动程序通过使用临界区来防止这些函数在任务和 ISR 中同时被调用，所以以下函数能够在 ISR 上下文中运行。

- :cpp:func:`gptimer_start`
- :cpp:func:`gptimer_stop`
- :cpp:func:`gptimer_get_raw_count`
- :cpp:func:`gptimer_set_raw_count`
- :cpp:func:`gptimer_set_alarm_action`

将 :cpp:type:`gptimer_handle_t` 作为第一个位置参数的其他函数不被视作线程安全，也就是说应该避免从多个任务中调用这些函数。

.. _kconfig-options:

Kconfig 选项
^^^^^^^^^^^^^^^^^^^^^^

- :ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` 控制放置通用定时器控制函数（IRAM 或 flash）的位置。了解更多信息，请参考章节 :ref:`iram-safe`。
- :ref:`CONFIG_GPTIMER_ISR_IRAM_SAFE` 控制默认 ISR 程序在 cache 禁用时是否可以运行。了解更多信息，请参考章节 :ref:`iram-safe`。
- :ref:`CONFIG_GPTIMER_ENABLE_DEBUG_LOG` 用于启用调试日志输出。启用这一选项将增加固件二进制文件大小。

应用示例
------------------

* 示例 :example:`peripherals/timer_group/gptimer` 中列出了通用定时器的典型用例。

API 参考
-------------------

.. include-build-file:: inc/gptimer.inc
.. include-build-file:: inc/timer_types.inc

.. [1]
   不同 ESP 芯片系列的通用定时器实例数量可能不同。了解详细信息，请参考《{IDF_TARGET_NAME} 技术参考手册》 > 章节定时器组 (TIMG) [`PDF <{IDF_TARGET_TRM_CN_URL}#timg>`__]。驱动程序不会禁止您申请更多的定时器，但是当所有可用的硬件资源用完时将会返回错误。在分配资源时，请务必检查返回值（例如 :cpp:func:`gptimer_new_timer`）。

.. [2]
   :cpp:member:`gptimer_event_callbacks_t::on_alarm` 回调函数和这一函数调用的函数也需放在 IRAM 中，请自行处理。
