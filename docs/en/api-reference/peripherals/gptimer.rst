General Purpose Timer (GPTimer)
===============================

Introduction
------------

GPTimer (General Purpose Timer) is the driver of {IDF_TARGET_NAME} Timer Group peripheral. The hardware timer features high resolution and flexible alarm action. The behavior when the internal counter of a timer reaches a specific target value is called a timer alarm. When a timer alarms, a user registered per-timer callback would be called.

Typically, a general purpose timer can be used in scenarios like:

- Free running as a wall clock, fetching a high-resolution timestamp at any time and any places
- Generate period alarms, trigger events periodically
- Generate one-shot alarm, respond in target time

Functional Overview
-------------------

The following sections of this document cover the typical steps to install and operate a timer:

.. list::

    - :ref:`gptimer-resource-allocation` - covers which parameters should be set up to get a timer handle and how to recycle the resources when GPTimer finishes working.
    - :ref:`set-and-get-count-value` - covers how to force the timer counting from a start point and how to get the count value at anytime.
    - :ref:`set-up-alarm-action` - covers the parameters that should be set up to enable the alarm event.
    - :ref:`gptimer-register-event-callbacks` - covers how to hook user specific code to the alarm event callback function.
    - :ref:`enable-and-disable-timer` - covers how to enable and disable the timer.
    - :ref:`start-and-stop-timer` - shows some typical use cases that start the timer with different alarm behavior.
    :SOC_TIMER_SUPPORT_ETM: - :ref:`gptimer-etm-event-and-task` - describes what the events and tasks can be connected to the ETM channel.
    - :ref:`gptimer-power-management` - describes how different source clock selections can affect power consumption.
    - :ref:`gptimer-iram-safe` - describes tips on how to make the timer interrupt and IO control functions work better along with a disabled cache.
    - :ref:`gptimer-thread-safety` - lists which APIs are guaranteed to be thread safe by the driver.
    - :ref:`gptimer-kconfig-options` - lists the supported Kconfig options that can be used to make a different effect on driver behavior.

.. _gptimer-resource-allocation:

Resource Allocation
^^^^^^^^^^^^^^^^^^^

Different ESP chips might have different numbers of independent timer groups, and within each group, there could also be several independent timers. [1]_

A GPTimer instance is represented by :cpp:type:`gptimer_handle_t`. The driver behind manages all available hardware resources in a pool, so that you do not need to care about which timer and which group it belongs to.

To install a timer instance, there is a configuration structure that needs to be given in advance: :cpp:type:`gptimer_config_t`:

- :cpp:member:`gptimer_config_t::clk_src` selects the source clock for the timer. The available clocks are listed in :cpp:type:`gptimer_clock_source_t`, you can only pick one of them. For the effect on power consumption of different clock source, please refer to Section :ref:`gptimer-power-management`.
- :cpp:member:`gptimer_config_t::direction` sets the counting direction of the timer, supported directions are listed in :cpp:type:`gptimer_count_direction_t`, you can only pick one of them.
- :cpp:member:`gptimer_config_t::resolution_hz` sets the resolution of the internal counter. Each count step is equivalent to **1 / resolution_hz** seconds.
- :cpp:member:`gptimer_config::intr_priority` sets the priority of the timer interrupt. If it is set to ``0``, the driver will allocate an interrupt with a default priority. Otherwise, the driver will use the given priority.
- :cpp:member:`gptimer_config_t::backup_before_sleep` enables the backup of the GPTimer registers before entering sleep mode. This option implies an balance between power consumption and memory usage. If the power consumption is not a concern, you can disable this option to save memory. But if you want to save more power, you should enable this option to backup the GPTimer registers before entering sleep mode, and restore them after waking up. This feature depends on specific hardware module, if you enable this flag on an unsupported chip, you will get an error message like ``register back up is not supported``.
- Optional :cpp:member:`gptimer_config_t::intr_shared` sets whether or not mark the timer interrupt source as a shared one. For the pros/cons of a shared interrupt, you can refer to :doc:`Interrupt Handling <../../api-reference/system/intr_alloc>`.

With all the above configurations set in the structure, the structure can be passed to :cpp:func:`gptimer_new_timer` which will instantiate the timer instance and return a handle of the timer.

The function can fail due to various errors such as insufficient memory, invalid arguments, etc. Specifically, when there are no more free timers (i.e., all hardware resources have been used up), then :c:macro:`ESP_ERR_NOT_FOUND` will be returned. The total number of available timers is represented by the :c:macro:`SOC_TIMER_GROUP_TOTAL_TIMERS` and its value depends on the ESP chip.

If a previously created GPTimer instance is no longer required, you should recycle the timer by calling :cpp:func:`gptimer_del_timer`. This allows the underlying HW timer to be used for other purposes. Before deleting a GPTimer handle, please disable it by :cpp:func:`gptimer_disable` in advance or make sure it has not enabled yet by :cpp:func:`gptimer_enable`.

Creating a GPTimer Handle with Resolution of 1 MHz
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

Set and Get Count Value
^^^^^^^^^^^^^^^^^^^^^^^

When the GPTimer is created, the internal counter will be reset to zero by default. The counter value can be updated asynchronously by :cpp:func:`gptimer_set_raw_count`. The maximum count value is dependent on the bit width of the hardware timer, which is also reflected by the SOC macro :c:macro:`SOC_TIMER_GROUP_COUNTER_BIT_WIDTH`. When updating the raw count of an active timer, the timer will immediately start counting from the new value.

Count value can be retrieved by :cpp:func:`gptimer_get_raw_count`, at any time.

.. _set-up-alarm-action:

Set up Alarm Action
^^^^^^^^^^^^^^^^^^^

For most of the use cases of GPTimer, you should set up the alarm action before starting the timer, except for the simple wall-clock scenario, where a free running timer is enough. To set up the alarm action, you should configure several members of :cpp:type:`gptimer_alarm_config_t` based on how you make use of the alarm event:

- :cpp:member:`gptimer_alarm_config_t::alarm_count` sets the target count value that triggers the alarm event. You should also take the counting direction into consideration when setting the alarm value. Specially, :cpp:member:`gptimer_alarm_config_t::alarm_count` and :cpp:member:`gptimer_alarm_config_t::reload_count` cannot be set to the same value when :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` is true, as keeping reload with a target alarm count is meaningless.

- :cpp:member:`gptimer_alarm_config_t::reload_count` sets the count value to be reloaded when the alarm event happens. This configuration only takes effect when :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` is set to true.

- :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` flag sets whether to enable the auto-reload feature. If enabled, the hardware timer will reload the value of :cpp:member:`gptimer_alarm_config_t::reload_count` into counter immediately when an alarm event happens.

To make the alarm configurations take effect, you should call :cpp:func:`gptimer_set_alarm_action`. Especially, if :cpp:type:`gptimer_alarm_config_t` is set to ``NULL``, the alarm function will be disabled.

.. note::

    If an alarm value is set and the timer has already exceeded this value, the alarm will be triggered immediately.

.. _gptimer-register-event-callbacks:

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

After the timer starts up, it can generate a specific event (e.g., the "Alarm Event") dynamically. If you have some functions that should be called when the event happens, please hook your function to the interrupt service routine by calling :cpp:func:`gptimer_register_event_callbacks`. All supported event callbacks are listed in :cpp:type:`gptimer_event_callbacks_t`:

-  :cpp:member:`gptimer_event_callbacks_t::on_alarm` sets a callback function for alarm events. As this function is called within the ISR context, you must ensure that the function does not attempt to block (e.g., by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function). The function prototype is declared in :cpp:type:`gptimer_alarm_cb_t`.

You can save your own context to :cpp:func:`gptimer_register_event_callbacks` as well, via the parameter ``user_data``. The user data will be directly passed to the callback function.

This function lazy installs the interrupt service for the timer but not enable it. So please call this function before :cpp:func:`gptimer_enable`, otherwise the :c:macro:`ESP_ERR_INVALID_STATE` error will be returned. See Section :ref:`enable-and-disable-timer` for more information.

.. _enable-and-disable-timer:

Enable and Disable Timer
^^^^^^^^^^^^^^^^^^^^^^^^

Before doing IO control to the timer, you needs to enable the timer first, by calling :cpp:func:`gptimer_enable`. This function:

* Switches the timer driver state from **init** to **enable**.
* Enables the interrupt service if it has been lazy installed by :cpp:func:`gptimer_register_event_callbacks`.
* Acquires a proper power management lock if a specific clock source (e.g., APB clock) is selected. See Section :ref:`gptimer-power-management` for more information.

Calling :cpp:func:`gptimer_disable` does the opposite, that is, put the timer driver back to the **init** state, disable the interrupts service and release the power management lock.

.. _start-and-stop-timer:

Start and Stop Timer
^^^^^^^^^^^^^^^^^^^^

The basic IO operation of a timer is to start and stop. Calling :cpp:func:`gptimer_start` can make the internal counter work, while calling :cpp:func:`gptimer_stop` can make the counter stop working. The following illustrates how to start a timer with or without an alarm event.

Calling :cpp:func:`gptimer_start` transits the driver state from **enable** to **run**, and vice versa. You need to make sure the start and stop functions are used in pairs, otherwise, the functions may return :c:macro:`ESP_ERR_INVALID_STATE`. Most of the time, this error means that the timer is already stopped or in the "start protection" state (i.e., :cpp:func:`gptimer_start` is called but not finished).

Start Timer as a Wall Clock
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    // Retrieve the timestamp at any time
    uint64_t count;
    ESP_ERROR_CHECK(gptimer_get_raw_count(gptimer, &count));

Trigger Period Events
~~~~~~~~~~~~~~~~~~~~~

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
        // Do not introduce complex logics in callbacks
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

Trigger One-Shot Event
~~~~~~~~~~~~~~~~~~~~~~

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

Dynamic Alarm Update
~~~~~~~~~~~~~~~~~~~~

Alarm value can be updated dynamically inside the ISR handler callback, by changing :cpp:member:`gptimer_alarm_event_data_t::alarm_value`. Then the alarm value will be updated after the callback function returns.

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

    ETM Event and Task
    ^^^^^^^^^^^^^^^^^^

    GPTimer is able to generate various events that can interact with the :doc:`ETM </api-reference/peripherals/etm>` module. The supported events are listed in the :cpp:type:`gptimer_etm_event_type_t`. You can call :cpp:func:`gptimer_new_etm_event` to get the corresponding ETM event handle. Likewise, GPTimer exposes several tasks that can be triggered by other ETM events. The supported tasks are listed in the :cpp:type:`gptimer_etm_task_type_t`. You can call :cpp:func:`gptimer_new_etm_task` to get the corresponding ETM task handle.

    For how to connect the event and task to an ETM channel, please refer to the :doc:`ETM </api-reference/peripherals/etm>` documentation.

    .. _gptimer-power-management:

.. only:: not SOC_TIMER_SUPPORT_ETM

    .. _gptimer-power-management:

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled, i.e., :ref:`CONFIG_PM_ENABLE` is on, the system may adjust or disable the clock source before going to sleep. As a result, the time keeping will be inaccurate.

The driver can prevent the above issue by creating a power management lock. The lock type is set based on different clock sources. The driver will acquire the lock in :cpp:func:`gptimer_enable`, and release it in :cpp:func:`gptimer_disable`. So that the timer can work correctly in between these two functions, because the clock source won't be disabled or adjusted its frequency during this time.

.. only:: SOC_TIMER_SUPPORT_SLEEP_RETENTION

    Besides the potential changes to the clock source, when the power management is enabled, the system can also power down a domain where GPTimer register located. To ensure the GPTimer driver can continue work after sleep, we can either backup the GPTimer registers to the RAM, or just refuse to power down. You can choose what to do in :cpp:member:`gptimer_config_t::backup_before_sleep`. It's a balance between power saving and memory consumption. Set it based on your application requirements.

.. _gptimer-iram-safe:

IRAM Safe
^^^^^^^^^

By default, the GPTimer interrupt will be deferred when the cache is disabled because of writing or erasing the flash. Thus the alarm interrupt will not get executed in time, which is not expected in a real-time application.

There is a Kconfig option :ref:`CONFIG_GPTIMER_ISR_IRAM_SAFE` that:

-  Enables the interrupt being serviced even when the cache is disabled
-  Places all functions that used by the ISR into IRAM [2]_
-  Places driver object into DRAM (in case it is mapped to PSRAM by accident)

This allows the interrupt to run while the cache is disabled, but comes at the cost of increased IRAM consumption.

There is another Kconfig option :ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` that can put commonly used IO control functions into IRAM as well. So, these functions can also be executable when the cache is disabled. These IO control functions are as follows:

- :cpp:func:`gptimer_start`
- :cpp:func:`gptimer_stop`
- :cpp:func:`gptimer_get_raw_count`
- :cpp:func:`gptimer_set_raw_count`
- :cpp:func:`gptimer_set_alarm_action`

.. _gptimer-thread-safety:

Thread Safety
^^^^^^^^^^^^^

All the APIs provided by the driver are guaranteed to be thread safe, which means you can call them from different RTOS tasks without protection by extra locks. The following functions are allowed to run under ISR context.

- :cpp:func:`gptimer_start`
- :cpp:func:`gptimer_stop`
- :cpp:func:`gptimer_get_raw_count`
- :cpp:func:`gptimer_set_raw_count`
- :cpp:func:`gptimer_get_captured_count`
- :cpp:func:`gptimer_set_alarm_action`

.. _gptimer-kconfig-options:

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` controls where to place the GPTimer control functions (IRAM or flash).
- :ref:`CONFIG_GPTIMER_ISR_HANDLER_IN_IRAM` controls where to place the GPTimer ISR handler (IRAM or flash).
- :ref:`CONFIG_GPTIMER_ISR_IRAM_SAFE` controls whether the default ISR handler should be masked when the cache is disabled, see Section :ref:`gptimer-iram-safe` for more information.
- :ref:`CONFIG_GPTIMER_ENABLE_DEBUG_LOG` is used to enabled the debug log output. Enable this option will increase the firmware binary size.

Application Examples
--------------------

.. list::

    - Typical use cases of GPTimer are listed in the example :example:`peripherals/timer_group/gptimer`.
    :SOC_TIMER_SUPPORT_ETM: - GPTimer capture external event's timestamp, with the help of ETM module: :example:`peripherals/timer_group/gptimer_capture_hc_sr04`.

API Reference
-------------

.. include-build-file:: inc/gptimer.inc
.. include-build-file:: inc/gptimer_types.inc
.. include-build-file:: inc/timer_types.inc

.. only:: SOC_TIMER_SUPPORT_ETM

    .. include-build-file:: inc/gptimer_etm.inc

.. [1]
   Different ESP chip series might have different numbers of GPTimer instances. For more details, please refer to **{IDF_TARGET_NAME} Technical Reference Manual** > Chapter **Timer Group (TIMG)** [`PDF <{IDF_TARGET_TRM_EN_URL}#timg>`__]. The driver does forbid you from applying for more timers, but it returns error when all available hardware resources are used up. Please always check the return value when doing resource allocation (e.g., :cpp:func:`gptimer_new_timer`).

.. [2]
   :cpp:member:`gptimer_event_callbacks_t::on_alarm` callback and the functions invoked by the callback should also be placed in IRAM, please take care of them by yourself.
