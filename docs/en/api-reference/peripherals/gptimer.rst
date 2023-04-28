General Purpose Timer (GPTimer)
===============================

Introduction
------------

The GPTimer (General Purpose Timer) is the driver of {IDF_TARGET_NAME} Timer Group peripheral. It features a hardware timer with high resolution and flexible alarm action. The behavior when the internal counter of a timer reaches a specific target value is called timer alarm. When a timer alarms, a user registered per-timer callback would be called.

Typically, a general purpose timer can be used in scenarios like:

-  Free running as a wall clock, fetching a high-resolution time stamp at any time and any places
-  Generate period alarms, trigger events periodically
-  Generate one-shot alarm, respond in target time

Functional Overview
-------------------

The following sections of this document cover the typical steps to install and operate a timer:

-  `Resource Allocation <#resource-allocation>`__ - covers which parameters should be set up to get a timer handle and how to recycle the resources when GPTimer finishes working.
-  `Set and Get count value <#set-and-get-count-value>`__ - covers how to force the timer counting from a start point and how to get the count value at anytime.
-  `Set Up Alarm Action <#set-up-alarm-action>`__ - covers the parameters that should be set up to enable the alarm event.
-  `Register Event Callbacks <#register-event-callbacks>`__ - covers how to hook user specific code to the alarm event callback function.
-  `Enable and Disable timer <#enable-and-disable-timer>`__ - covers how to enable and disable the timer.
-  `Start and Stop timer <#start-and-stop-timer>`__ - shows some typical use cases that start the timer with different alarm behavior.
-  `Power Management <#power-management>`__ - describes how different source clock selections can affect power consumption.
-  `IRAM Safe <#iram-safe>`__ - describes tips on how to make the timer interrupt and IO control functions work better along with a disabled cache.
-  `Thread Safety <#thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.
-  `Kconfig options <#kconfig-options>`__ - lists the supported Kconfig options that can be used to make a different effect on driver behavior.

Resource Allocation
^^^^^^^^^^^^^^^^^^^

Different ESP chips might have different numbers of independent timer groups, and within each group, there could also be several independent timers. [1]_

From driver's point of view, a GPTimer instance is represented by :cpp:type:`gptimer_handle_t`. The driver behind will manage all available hardware resources in a pool, so that users don't need to care about which timer and which group it belongs to.

To install a timer instance, there's a configuration structure that needs to be given in advance: :cpp:type:`gptimer_config_t`:

-  :cpp:member:`gptimer_config_t::clk_src` selects the source clock for the timer. The available clocks are listed in :cpp:type:`gptimer_clock_source_t`, you can only pick one of them. For the effect on power consumption of different clock source, please refer to `Power management <#power-management>`__ section.

-  :cpp:member:`gptimer_config_t::direction` sets the counting direction of the timer, supported directions are listed in :cpp:type:`gptimer_count_direction_t`, you can only pick one of them.

-  :cpp:member:`gptimer_config_t::resolution_hz` sets the resolution of the internal counter. Each count step is equivalent to **1 / resolution_hz** seconds.

-  Optional :cpp:member:`gptimer_config_t::intr_shared` sets whether or not mark the timer interrupt source as a shared one. For the pros/cons of a shared interrupt, you can refer to :doc:`Interrupt Handling <../../api-reference/system/intr_alloc>`.

With all the above configurations set in the structure, the structure can be passed to :cpp:func:`gptimer_new_timer` which will instantiate the timer instance and return a handle of the timer.

The function can fail due to various errors such as insufficient memory, invalid arguments, etc. Specifically, when there are no more free timers (i.e. all hardware resources have been used up), then :c:macro:`ESP_ERR_NOT_FOUND` will be returned. The total number of available timers is represented by the :c:macro:`SOC_TIMER_GROUP_TOTAL_TIMERS` and its value will depend on the ESP chip.

If a previously created GPTimer instance is no longer required, you should recycle the timer by calling :cpp:func:`gptimer_del_timer`. This will allow the underlying HW timer to be used for other purposes. Before deleting a GPTimer handle, you should disable it by :cpp:func:`gptimer_disable` in advance or make sure it has not enabled yet by :cpp:func:`gptimer_enable`.

Creating a GPTimer Handle with Resolution of 1MHz
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

   gptimer_handle_t gptimer = NULL;
   gptimer_config_t timer_config = {
       .clk_src = GPTIMER_CLK_SRC_DEFAULT,
       .direction = GPTIMER_COUNT_UP,
       .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
   };
   ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

Set and Get Count Value
^^^^^^^^^^^^^^^^^^^^^^^

When the GPTimer is created, the internal counter will be reset to zero by default. The counter value can be updated asynchronously by :cpp:func:`gptimer_set_raw_count`. The maximum count value is dependent on the hardware timer's bit-width, which is also reflected by the SOC macro :c:macro:`SOC_TIMER_GROUP_COUNTER_BIT_WIDTH`. When updating the raw count of an active timer, the timer will immediately start counting from the new value.

Count value can be retrieved by :cpp:func:`gptimer_get_raw_count`, at anytime.

Set Up Alarm Action
^^^^^^^^^^^^^^^^^^^

Most of the use cases of GPTimer should set up the alarm action before starting the timer, except for the simple wall-clock scenario, where a free running timer is enough. To set up the alarm action, one should configure several members of :cpp:type:`gptimer_alarm_config_t` based on how he takes use of the alarm event:

-  :cpp:member:`gptimer_alarm_config_t::alarm_count` sets the target count value that will trigger the alarm event. You should also take the counting direction into consideration when setting the alarm value.
   Specially, :cpp:member:`gptimer_alarm_config_t::alarm_count` and :cpp:member:`gptimer_alarm_config_t::reload_count` can't be set to the same value when :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` is true, as keeping reload with a target alarm count is meaningless.

-  :cpp:member:`gptimer_alarm_config_t::reload_count` sets the count value to be reloaded when the alarm event happens. This configuration only takes effect when :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` is set to true.

-  :cpp:member:`gptimer_alarm_config_t::auto_reload_on_alarm` flag sets whether to enable the auto-reload feature. If enabled, the hardware timer will reload the value of :cpp:member:`gptimer_alarm_config_t::reload_count` into counter immediately when alarm event happens.

To make the alarm configurations take effect, one should call :cpp:func:`gptimer_set_alarm_action`. Especially, if :cpp:type:`gptimer_alarm_config_t` is set to ``NULL``, the alarm function will be disabled.

.. note::

    If an alarm value is set and the timer has already crossed this value, the alarm will be triggered immediately.

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

After the timer starts up, it can generate specific event (e.g. the "Alarm Event") dynamically. If you have some function that should be called when event happens, you should hook your function to the interrupt service routine by calling :cpp:func:`gptimer_register_event_callbacks`. All supported event callbacks are listed in the :cpp:type:`gptimer_event_callbacks_t`:

-  :cpp:member:`gptimer_event_callbacks_t::on_alarm` sets callback function for alarm event. As this function is called within the ISR context, user must ensure that the function doesn't attempt to block (e.g., by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function). The function prototype is declared in :cpp:type:`gptimer_alarm_cb_t`.

One can save his own context to :cpp:func:`gptimer_register_event_callbacks` as well, via the parameter ``user_data``. The user data will be directly passed to the callback functions.

This function will lazy install interrupt service for the timer but not enable it. So user should call this function before :cpp:func:`gptimer_enable`, otherwise the :c:macro:`ESP_ERR_INVALID_STATE` error will be returned. See also `Enable and Disable timer <#enable-and-disable-timer>`__ for more information.

Enable and Disable Timer
^^^^^^^^^^^^^^^^^^^^^^^^

Before doing IO control to the timer, user needs to enable the timer first, by calling :cpp:func:`gptimer_enable`. Internally, this function will:

* switch the timer driver state from **init** to **enable**.
* enable the interrupt service if it has been lazy installed by :cpp:func:`gptimer_register_event_callbacks`.
* acquire a proper power management lock if a specific clock source (e.g. APB clock) is selected. See also `Power management <#power-management>`__ for more information.

On the contrary, calling :cpp:func:`gptimer_disable` will do the opposite, that is, put the timer driver back to the **init** state, disable the interrupts service and release the power management lock.

Start and Stop Timer
^^^^^^^^^^^^^^^^^^^^

The basic IO operation of a timer is to start and stop. Calling :cpp:func:`gptimer_start` can make the internal counter work, while calling :cpp:func:`gptimer_stop` can make the counter stop working. The following will illustrate on how to start a timer with or without alarm event.

Start Timer As a Wall Clock
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    // Retrieve timestamp at anytime
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
        // Retrieve count value from event data
        example_queue_element_t ele = {
            .event_count = edata->count_value
        };
        // Optional: send the event data to other task by OS queue
        // Don't introduce complex logics in callbacks.
        // Suggest dealing with event data in the main loop, instead of in this callback.
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
        // Retrieve count value from event data
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

Alarm value can be updated dynamically inside the ISR handler callback, by changing the :cpp:member:`gptimer_alarm_event_data_t::alarm_value`. Then the alarm value will be updated after the callback function returns.

.. code:: c

    typedef struct {
        uint64_t event_count;
    } example_queue_element_t;

    static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
    {
        BaseType_t high_task_awoken = pdFALSE;
        QueueHandle_t queue = (QueueHandle_t)user_data;
        // Retrieve count value from event data
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

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled (i.e. :ref:`CONFIG_PM_ENABLE` is on), the system will adjust the APB frequency before going into light sleep, thus potentially changing the period of a GPTimer's counting step and leading to inaccurate time keeping.

However, the driver can prevent the system from changing APB frequency by acquiring a power management lock of type :cpp:enumerator:`ESP_PM_APB_FREQ_MAX`. Whenever the driver creates a GPTimer instance that has selected :cpp:enumerator:`GPTIMER_CLK_SRC_APB` as its clock source, the driver will guarantee that the power management lock is acquired when enable the timer by :cpp:func:`gptimer_enable`. Likewise, the driver releases the lock when :cpp:func:`gptimer_disable` is called for that timer.

If the gptimer clock source is selected to others like :cpp:enumerator:`GPTIMER_CLK_SRC_XTAL`, then the driver won't install power management lock for it, which is more suitable for a low power application as long as the source clock can still provide sufficient resolution.

IRAM Safe
^^^^^^^^^

By default, the GPTimer interrupt will be deferred when the Cache is disabled for reasons like writing/erasing Flash. Thus the alarm interrupt will not get executed in time, which is not expected in a real-time application.

There's a Kconfig option :ref:`CONFIG_GPTIMER_ISR_IRAM_SAFE` that will:

1. Enable the interrupt being serviced even when cache is disabled
2. Place all functions that used by the ISR into IRAM [2]_
3. Place driver object into DRAM (in case it's mapped to PSRAM by accident)

This will allow the interrupt to run while the cache is disabled but will come at the cost of increased IRAM consumption.

There's another Kconfig option :ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` that can put commonly used IO control functions into IRAM as well. So that these functions can also be executable when the cache is disabled. These IO control functions are as follows:

- :cpp:func:`gptimer_start`
- :cpp:func:`gptimer_stop`
- :cpp:func:`gptimer_get_raw_count`
- :cpp:func:`gptimer_set_raw_count`
- :cpp:func:`gptimer_set_alarm_action`

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`gptimer_new_timer` is guaranteed to be thread safe by the driver, which means, user can call it from different RTOS tasks without protection by extra locks.
The following functions are allowed to run under ISR context, the driver uses a critical section to prevent them being called concurrently in both task and ISR.

- :cpp:func:`gptimer_start`
- :cpp:func:`gptimer_stop`
- :cpp:func:`gptimer_get_raw_count`
- :cpp:func:`gptimer_set_raw_count`
- :cpp:func:`gptimer_set_alarm_action`

Other functions that take the :cpp:type:`gptimer_handle_t` as the first positional parameter, are not treated as thread safe. Which means the user should avoid calling them from multiple tasks.

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` controls where to place the GPTimer control functions (IRAM or Flash), see `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_GPTIMER_ISR_IRAM_SAFE` controls whether the default ISR handler can work when cache is disabled, see `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_GPTIMER_ENABLE_DEBUG_LOG` is used to enabled the debug log output. Enable this option will increase the firmware binary size.

Application Examples
--------------------

* Typical use cases of GPTimer are listed in the example: :example:`peripherals/timer_group/gptimer`.

API Reference
-------------

.. include-build-file:: inc/gptimer.inc
.. include-build-file:: inc/timer_types.inc

.. [1]
   Different ESP chip series might have different numbers of GPTimer instances. Please refer to Chapter `Timer Group (TIMG) <{IDF_TARGET_TRM_EN_URL}#timg>`__ in {IDF_TARGET_NAME} Technical Reference Manual for more details. The driver won't forbid you from applying for more timers, but it will return error when all available hardware resources are used up. Please always check the return value when doing resource allocation (e.g. :cpp:func:`gptimer_new_timer`).

.. [2]
   :cpp:member:`gptimer_event_callbacks_t::on_alarm` callback and the functions invoked by itself should also be placed in IRAM, users need to take care of them by themselves.
