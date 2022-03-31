Pulse Counter (PCNT)
====================

Introduction
------------

The PCNT (Pulse Counter) module is designed to count the number of rising and/or falling edges of input signals. The {IDF_TARGET_NAME} contains multiple pulse counter units in the module. [1]_ Each unit is in effect an independent counter with multiple channels, where each channel can increment/decrement the counter on a rising/falling edge. Furthermore, each channel can be configured separately.

PCNT channels can react to signals of **edge** type and **level** type, however for simple applications, detecting the edge signal is usually sufficient. PCNT channels can be configured react to both pulse edges (i.e., rising and falling edge), and can be configured to increase, decrease or do nothing to the unit's counter on each edge. The level signal is the so-called **control signal**, which is used to control the counting mode of the edge signals that are attached to the same channel. By combining the usage of both edge and level signals, a PCNT unit can act as a **quadrature decoder**.

Besides that, PCNT unit is equipped with a separate glitch filter, which is helpful to remove noise from the signal.

Typically, a PCNT module can be used in scenarios like:

-  Calculate periodic signal's frequency by counting the pulse numbers within a time slice
-  Decode quadrature signals into speed and direction

Functional Overview
-------------------

Description of the PCNT functionality is broken down into the following sections:

-  `Resource Allocation <#resource-allocation>`__ - covers how to allocate PCNT units and channels with properly set of configurations. It also covers how to recycle the resources when they finished working.

-  `Set Up Channel Actions <#set-up-channel-actions>`__ - covers how to configure the PCNT channel to behave on different signal edges and levels.
-  `Watch Points <#watch-points>`__ - describes how to configure PCNT watch points (i.e., tell PCNT unit to trigger an event when the count reaches a certain value).
-  `Register Event Callbacks <#register-event-callbacks>`__ - describes how to hook user specific code to the watch point event callback function.
-  `Unit IO Control <#unit-io-control>`__ - describes IO control functions of PCNT unit, like enable glitch filter, start and stop unit, get and clear count value.
-  `Power Management <#power-management>`__ - describes what functionality will prevent the chip from going into low power mode.
-  `IRAM Safe <#iram-safe>`__ - describes tips on how to make the PCNT interrupt and IO control functions work better along with a disabled cache.
-  `Thread Safety <#thread-safety>`__ - lists which APIs are guaranteed to be thread safe by the driver.
-  `Kconfig options <#kconfig-options>`__ - lists the supported Kconfig options that can be used to make a different effect on driver behavior.

Resource Allocation
^^^^^^^^^^^^^^^^^^^

The PCNT unit and channel are represented by :cpp:type:`pcnt_unit_handle_t` and :cpp:type:`pcnt_channel_handle_t` respectively. All available units and channels are maintained by the driver in a resource pool, so the user doesn't need to know the exact underlying instance ID.

Install PCNT Unit
~~~~~~~~~~~~~~~~~

To install a PCNT unit, there's a configuration structure that needs to be given in advance: :cpp:type:`pcnt_unit_config_t`:

-  :cpp:member:`pcnt_unit_config_t::low_limit` and :cpp:member:`pcnt_unit_config_t::high_limit` specify the range for the internal counter. Counter will back to zero when it crosses either limit value.

Unit allocation and initialization is done by calling a function :cpp:func:`pcnt_new_unit` with :cpp:type:`pcnt_unit_config_t` as an input parameter. The function will return a PCNT unit handle only when it runs correctly. Specifically, when there are no more free PCNT units in the pool (i.e. unit resources have been used up), then this function will return :c:macro:`ESP_ERR_NOT_FOUND` error. The total number of available PCNT units is recorded by :c:macro:`SOC_PCNT_UNITS_PER_GROUP` for reference.

If a previously created PCNT unit is no longer needed, it's recommended to recycle the resource by calling :cpp:func:`pcnt_del_unit`. Which in return allows the underlying unit hardware to be used for other purposes. Before deleting a PCNT unit, one should ensure the following prerequisites:

- The unit is in a stop state, in other words, the unit either is stopped by :cpp:func:`pcnt_unit_stop` or not started yet.
- The unit ought to stop watching any "watch point". See `Watch Points <#watch-points>`__ for how to removing a watch point.

.. code:: c

    #define EXAMPLE_PCNT_HIGH_LIMIT 100
    #define EXAMPLE_PCNT_LOW_LIMIT  -100

    pcnt_unit_config_t unit_config = {
        .high_limit = EXAMPLE_PCNT_HIGH_LIMIT,
        .low_limit = EXAMPLE_PCNT_LOW_LIMIT,
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

Install PCNT Channel
~~~~~~~~~~~~~~~~~~~~

To install a PCNT channel, there's a configuration structure that needs to be given in advance: :cpp:type:`pcnt_chan_config_t` as well:

-  :cpp:member:`pcnt_chan_config_t::edge_gpio_num` and :cpp:member:`pcnt_chan_config_t::level_gpio_num` specify the GPIO numbers used by **edge** type signal and **level** type signal. :cpp:member:`pcnt_chan_config_t::level_gpio_num` is optional and can be assigned with `-1` if it's not used whereas the :cpp:member:`pcnt_chan_config_t::edge_gpio_num` is mandatory.
-  :cpp:member:`pcnt_chan_config_t::invert_edge_input` and :cpp:member:`pcnt_chan_config_t::invert_level_input` are used to decide whether to invert the input signals before they going into PCNT hardware. The invert is done by GPIO matrix instead of PCNT hardware.
-  :cpp:member:`pcnt_chan_config_t::io_loop_back` is for debug only, which enables both the GPIO's input and output paths. This can help to simulate the pulse signals by function :cpp:func:`gpio_set_level` on the same GPIO.

Channel allocating and initialization is done by calling a function :cpp:func:`pcnt_new_channel` with the above :cpp:type:`pcnt_chan_config_t` input parameter plus a PCNT unit handle returned from :cpp:func:`pcnt_new_unit`. This function will return a PCNT channel handle if it runs correctly. Specifically, when there are no more free PCNT channel within the unit (i.e. channel resources have been used up), then this function will return :c:macro:`ESP_ERR_NOT_FOUND` error. The total number of available PCNT channels within the unit is recorded by :c:macro:`SOC_PCNT_CHANNELS_PER_UNIT` for reference.

If a previously created PCNT channel is no longer needed, it's recommended to recycle the resources by calling :cpp:func:`pcnt_del_channel`. Which in return allows the underlying channel hardware to be used for other purposes.

.. code:: c

    #define EXAMPLE_CHAN_GPIO_A 0
    #define EXAMPLE_CHAN_GPIO_B 2

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = EXAMPLE_CHAN_GPIO_A,
        .level_gpio_num = EXAMPLE_CHAN_GPIO_B,
    };
    pcnt_channel_handle_t pcnt_chan = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));

Set Up Channel Actions
^^^^^^^^^^^^^^^^^^^^^^

The PCNT will increase/decrease/hold its internal count value when the input pulse signal toggles. User can set different actions for edge signal and/or level signal.

-  :cpp:func:`pcnt_channel_set_edge_action` function is to set specific actions for rising and falling edge of the signal attached to the :cpp:member:`pcnt_chan_config_t::edge_gpio_num`. Supported actions are listed in :cpp:type:`pcnt_channel_edge_action_t`.
-  :cpp:func:`pcnt_channel_set_level_action` function is to set specific actions for high and low level of the signal attached to the :cpp:member:`pcnt_chan_config_t::level_gpio_num`. Supported actions are listed in :cpp:type:`pcnt_channel_level_action_t`. This function is not mandatory if the :cpp:member:`pcnt_chan_config_t::level_gpio_num` is set to `-1` when allocating PCNT channel by :cpp:func:`pcnt_new_channel`.

.. code:: c

    // decrease the counter on rising edge, increase the counter on falling edge
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    // keep the counting mode when the control signal is high level, and reverse the counting mode when the control signal is low level
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

Watch Points
^^^^^^^^^^^^

Each PCNT unit can be configured to watch several different values that you're interested in. The value to be watched is also called **Watch Point**. The watch point itself can't exceed the range set in :cpp:type:`pcnt_unit_config_t` by :cpp:member:`pcnt_unit_config_t::low_limit` and :cpp:member:`pcnt_unit_config_t::high_limit`. When the counter reaches either watch point, a watch event will be triggered and notify user by interrupt if any watch event callback has ever registered in :cpp:func:`pcnt_unit_register_event_callbacks`. See `Register Event Callbacks <#register-event-callbacks>`__ for how to register event callbacks.

The watch point can be added and removed by :cpp:func:`pcnt_unit_add_watch_point` and :cpp:func:`pcnt_unit_remove_watch_point`. The commonly used watch points are: **zero cross**, **maximum / minimum count** and other threshold values. The number of available watch point is limited, :cpp:func:`pcnt_unit_add_watch_point` will return error :c:macro:`ESP_ERR_NOT_FOUND` if it can't find any free hardware resource to save the watch point. User can't add the same watch point for multiple times, otherwise it will return error :c:macro:`ESP_ERR_INVALID_STATE`.

It is recommended to remove the unused watch point by :cpp:func:`pcnt_unit_remove_watch_point` to recycle the watch point resources. Be careful when you recycle the PCNT unit by :cpp:func:`pcnt_del_unit`, the using watch points must be removed from the unit in advance.

.. code:: c

    // add zero across watch point
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, 0));
    // add high limit watch point
    ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, EXAMPLE_PCNT_HIGH_LIMIT));

Register Event Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^

When PCNT unit reaches any enabled watch point, specific event will be generated and notify the CPU by interrupt. If you have some function that want to get executed when event happens, you should hook your function to the interrupt service routine by calling :cpp:func:`pcnt_unit_register_event_callbacks`. All supported event callbacks are listed in the :cpp:type:`pcnt_event_callbacks_t`:

-  :cpp:member:`pcnt_event_callbacks_t::on_reach` sets a callback function for watch point event. As this function is called within the ISR context, user must ensure that the function doesn't attempt to block (e.g., by making sure that only FreeRTOS APIs with ``ISR`` suffix are called from within the function). The function prototype is declared in :cpp:type:`pcnt_watch_cb_t`.

User can save their own context to :cpp:func:`pcnt_unit_register_event_callbacks` as well, via the parameter ``user_ctx``. This user data will be directly passed to the callback functions.

In the callback function, the driver will fill in the event data of specific event. For example, the watch point event data is declared as :cpp:type:`pcnt_watch_event_data_t`:

-  :cpp:member:`pcnt_watch_event_data_t::watch_point_value` saves the watch point value that triggers the event.
-  :cpp:member:`pcnt_watch_event_data_t::zero_cross_mode` saves how the PCNT unit crosses the zero point in the latest time. The possible zero cross modes are listed in the :cpp:type:`pcnt_unit_zero_cross_mode_t`. Usually different zero cross mode means different **counting direction** and **counting step size**.

.. code:: c

    static bool example_pcnt_on_reach(pcnt_unit_handle_t unit, pcnt_watch_event_data_t *edata, void *user_ctx)
    {
        BaseType_t high_task_wakeup;
        QueueHandle_t queue = (QueueHandle_t)user_ctx;
        // send watch point to queue, from this interrupt callback
        xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
        // return whether a high priority task has been waken up by this function
        return (high_task_wakeup == pdTRUE);
    }

    pcnt_event_callbacks_t cbs = {
        .on_reach = example_pcnt_on_reach,
    };
    QueueHandle_t queue = xQueueCreate(10, sizeof(int));
    ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, queue));

Unit IO Control
^^^^^^^^^^^^^^^

Set Glitch Filter
~~~~~~~~~~~~~~~~~

The PCNT unit features filters to ignore possible short glitches in the signals. The parameters that can be configured for the glitch filter are listed in :cpp:type:`pcnt_glitch_filter_config_t`:

-  :cpp:member:`pcnt_glitch_filter_config_t::max_glitch_ns` sets the maximum glitch width, in nano seconds. If a signal pulse's width is smaller than this value, then it will be treated as noise and won't increase/decrease the internal counter.

User can enable the glitch filter for PCNT unit by calling :cpp:func:`pcnt_unit_set_glitch_filter` with the filter configuration provided above. Particularly, user can disable the glitch filter later by calling :cpp:func:`pcnt_unit_set_glitch_filter` with a `NULL` filter configuration.

.. note::

    The glitch filter is clocked from APB. For the counter not to miss any pulses, the maximum glitch width should be longer than one APB_CLK cycle (usually 12.5 ns if APB equals 80MHz). As the APB frequency would be changed after DFS (Dynamic Frequency Scaling) enabled, which means the filter won't work as expect in that case. So the driver will install a PM lock for PCNT unit during the first time user enables the glitch filter. For more information related to power management strategy used in PCNT driver, please see `Power Management <#power-management>`__.

.. code:: c

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

Start/Stop and Clear
~~~~~~~~~~~~~~~~~~~~

Calling :cpp:func:`pcnt_unit_start` will make the PCNT unit start to work, increase or decrease counter according to pulse signals. On the contrary, calling :cpp:func:`pcnt_unit_stop` will stop the PCNT unit but retain current count value. Instead, clearing counter can only be done by calling :cpp:func:`pcnt_unit_clear_count`.

.. code::c

    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

Get Count Value
~~~~~~~~~~~~~~~

User can check current count value at any time by calling :cpp:func:`pcnt_unit_get_count`.

.. note::

    The returned count value is a **signed** integer, where the sign can be used to reflect the direction. The internal counter will overflow when it reaches high or low limit, but this function doesn't compensate for that loss.

.. code:: c

    int pulse_count = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &pulse_count));

Power Management
^^^^^^^^^^^^^^^^

When power management is enabled (i.e. :ref:`CONFIG_PM_ENABLE` is on), the system will adjust the APB frequency before going into light sleep, thus potentially changing the behavior of PCNT glitch filter and leading to valid signal being treated as noise.

However, the driver can prevent the system from changing APB frequency by acquiring a power management lock of type :cpp:enumerator:`ESP_PM_APB_FREQ_MAX`. Whenever user enables the glitch filter by :cpp:func:`pcnt_unit_set_glitch_filter`, the driver will guarantee that the power management lock is acquired after the PCNT unit is started by :cpp:func:`pcnt_unit_start`. Likewise, the driver releases the lock after :cpp:func:`pcnt_unit_stop` is called. This requires that the :cpp:func:`pcnt_unit_start` and :cpp:func:`pcnt_unit_stop` should appear in pairs, otherwise the power management will be out of action.

IRAM Safe
^^^^^^^^^

By default, the PCNT interrupt will be deferred when the Cache is disabled for reasons like writing/erasing Flash. Thus the alarm interrupt will not get executed in time, which is not expected in a real-time application.

There's a Kconfig option :ref:`CONFIG_PCNT_ISR_IRAM_SAFE` that will:

1. Enable the interrupt being serviced even when cache is disabled

2. Place all functions that used by the ISR into IRAM [2]_

3. Place driver object into DRAM (in case it's linked to PSRAM by accident)

This will allow the interrupt to run while the cache is disabled but will come at the cost of increased IRAM consumption.

There's another Kconfig option :ref:`CONFIG_PCNT_CTRL_FUNC_IN_IRAM` that can put commonly used IO control functions into IRAM as well. So that these functions can also be executable when the cache is disabled. These IO control functions are as follows:

- :cpp:func:`pcnt_unit_start`
- :cpp:func:`pcnt_unit_stop`
- :cpp:func:`pcnt_unit_clear_count`
- :cpp:func:`pcnt_unit_get_count`

Thread Safety
^^^^^^^^^^^^^

The factory function :cpp:func:`pcnt_new_unit`  and :cpp:func:`pcnt_new_channel` are guaranteed to be thread safe by the driver, which means, user can call them from different RTOS tasks without protection by extra locks.
Other functions that take the :cpp:type:`pcnt_unit_handle_t` and :cpp:type:`pcnt_channel_handle_t` as the first positional parameter, are not thread safe. The lifecycle of the PCNT unit and channel handle is maintained by the user. So user should avoid calling them concurrently. If it has to, another mutex should be added to prevent them being accessed concurrently.

Kconfig Options
^^^^^^^^^^^^^^^

- :ref:`CONFIG_PCNT_CTRL_FUNC_IN_IRAM` controls where to place the PCNT control functions (IRAM or Flash), see `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_PCNT_ISR_IRAM_SAFE` controls whether the default ISR handler can work when cache is disabled, see `IRAM Safe <#iram-safe>`__ for more information.
- :ref:`CONFIG_PCNT_ENABLE_DEBUG_LOG` is used to enabled the debug log output. Enable this option will increase the firmware binary size.

Application Examples
--------------------

* Decode the quadrature signals from rotary encoder: :example:`peripherals/pcnt/rotary_encoder`.


API Reference
-------------

.. include-build-file:: inc/pulse_cnt.inc
.. include-build-file:: inc/pcnt_types.inc

.. [1]
   Different ESP chip series might have different number of PCNT units and channels. Please refer to the [`TRM <{IDF_TARGET_TRM_EN_URL}#pcnt>`__] for details. The driver won't forbid you from applying for more PCNT units and channels, but it will return error when all available hardware resources are used up. Please always check the return value when doing resource allocation (e.g. :cpp:func:`pcnt_new_unit`).

.. [2]
   :cpp:member:`pcnt_event_callbacks_t::on_reach` callback and the functions invoked by itself should also be placed in IRAM, users need to take care of them by themselves.
