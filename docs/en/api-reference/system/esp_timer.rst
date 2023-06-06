High Resolution Timer (ESP Timer)
=================================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_HR_TIMER:default = "SYSTIMER", esp32 = "LAC timer"}

{IDF_TARGET_HR_TIMER_Resolution:default = "Not updated", esp32 = "64", esp32s2 = "64", esp32c3 = "52", esp32s3 = "52", esp32c2 = "52", esp32c6 = "52", esp32h2 = "52"}


Overview
--------

Although FreeRTOS provides software timers, FreeRTOS software timers have a few limitations:

- Maximum resolution is equal to the RTOS tick period
- Timer callbacks are dispatched from a low-priority timer service (i.e., daemon) task. This task can be preempted by other tasks, leading to decreased precision and accuracy.

Although hardware timers are not subject to the limitations mentioned, they may not be as user-friendly. For instance, application components may require timer events to be triggered at specific future times, but hardware timers typically have only one "compare" value for interrupt generation. This necessitates the creation of an additional system on top of the hardware timer to keep track of pending events and ensure that callbacks are executed when the corresponding hardware interrupts occur.

.. only:: esp32

    The hardware timer interrupt's priority is configured via the :ref:`CONFIG_ESP_TIMER_INTERRUPT_LEVEL` option (possible priorities being 1, 2, or 3). Raising the timer interrupt's priority can reduce the timer processing delay caused by interrupt latency.


``esp_timer`` set of APIs provides one-shot and periodic timers, microsecond time resolution, and {IDF_TARGET_HR_TIMER_Resolution}-bit range.

Internally, ``esp_timer`` uses a {IDF_TARGET_HR_TIMER_Resolution}-bit hardware timer. The exact hardware timer implementation used will depend on the target, where {IDF_TARGET_HR_TIMER} is used for {IDF_TARGET_NAME}.

Timer callbacks can be dispatched by two methods:

- ``ESP_TIMER_TASK``.
- ``ESP_TIMER_ISR``. Available only if :ref:`CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD` is enabled (by default disabled).

``ESP_TIMER_TASK``. Timer callbacks are dispatched from a high-priority ``esp_timer`` task. Because all the callbacks are dispatched from the same task, it is recommended to only do the minimal possible amount of work from the callback itself, posting an event to a lower-priority task using a queue instead.

If other tasks with a priority higher than ``esp_timer`` are running, callback dispatching will be delayed until the ``esp_timer`` task has a chance to run. For example, this will happen if an SPI Flash operation is in progress.

``ESP_TIMER_ISR``. Timer callbacks are dispatched directly from the timer interrupt handler. This method is useful for some simple callbacks which aim for lower latency.

Creating and starting a timer, and dispatching the callback takes some time. Therefore, there is a lower limit to the timeout value of one-shot ``esp_timer``. If :cpp:func:`esp_timer_start_once` is called with a timeout value of less than 20 us, the callback will be dispatched only after approximately 20 us.

Periodic ``esp_timer`` also imposes a 50 us restriction on the minimal timer period. Periodic software timers with a period of less than 50 us are not practical since they would consume most of the CPU time. Consider using dedicated hardware peripherals or DMA features if you find that a timer with a small period is required.

Using ``esp_timer`` APIs
------------------------

A single timer is represented by :cpp:type:`esp_timer_handle_t` type. Each timer has a callback function associated with it. This callback function is called from the ``esp_timer`` task each time the timer elapses.

- To create a timer, call :cpp:func:`esp_timer_create`.
- To delete the timer when it is no longer needed, call :cpp:func:`esp_timer_delete`.

The timer can be started in one-shot mode or in periodic mode.

- To start the timer in one-shot mode, call :cpp:func:`esp_timer_start_once`, passing the time interval after which the callback should be called. When the callback gets called, the timer is considered to be stopped.

- To start the timer in periodic mode, call :cpp:func:`esp_timer_start_periodic`, passing the period with which the callback should be called. The timer keeps running until :cpp:func:`esp_timer_stop` is called.

Note that the timer must not be running when :cpp:func:`esp_timer_start_once` or :cpp:func:`esp_timer_start_periodic` is called. To restart a running timer, call :cpp:func:`esp_timer_stop` first, then call one of the start functions.

Callback Functions
------------------

.. note:: Keep the callback functions as short as possible. Otherwise, it will affect all timers.

Timer callbacks that are processed by the ``ESP_TIMER_ISR`` method should not call the context switch call - ``portYIELD_FROM_ISR()``. Instead, use the :cpp:func:`esp_timer_isr_dispatch_need_yield` function. The context switch will be done after all ISR dispatch timers have been processed if required by the system.

.. only:: SOC_ETM_SUPPORTED and SOC_SYSTIMER_SUPPORT_ETM

    ETM Event
    ---------

    The ``esp_timer`` is constructed based on a hardware timer called *systimer*, which is able to generate the alarm event and interact with the :doc:`ETM </api-reference/peripherals/etm>` module. You can call :cpp:func:`esp_timer_new_etm_alarm_event` to get the corresponding ETM event handle.

    To know more about how to connect the event to an ETM channel, please refer to the :doc:`ETM </api-reference/peripherals/etm>` documentation.

``esp_timer`` During Light-sleep
--------------------------------

During Light-sleep, the ``esp_timer`` counter stops and no callback functions are called. Instead, the time is counted by the RTC counter. Upon waking up, the system gets the difference between the counters and calls a function that advances the ``esp_timer`` counter. Since the counter has been advanced, the system starts calling callbacks that were not called during sleep. The number of callbacks depends on the duration of the sleep and the period of the timers. It can lead to the overflow of some queues. This only applies to periodic timers, since one-shot timers will be called once.

This behavior can be changed by calling :cpp:func:`esp_timer_stop` before sleeping. In some cases, this can be inconvenient, and instead of the stop function, you can use the ``skip_unhandled_events`` option during :cpp:func:`esp_timer_create`. When the ``skip_unhandled_events`` is true, if a periodic timer expires one or more times during Light-sleep, then only one callback is called on wake.

Using the ``skip_unhandled_events`` option with automatic Light-sleep (see :doc:`Power Management APIs <power_management>`) helps to reduce the power consumption of the system when it is in Light-sleep. The duration of Light-sleep is also in part determined by the next event occurs. Timers with ``skip_unhandled_events`` option will not wake up the system.

Handling Callbacks
------------------

``esp_timer`` is designed to achieve a high-resolution and low-latency timer with the ability to handle delayed events. If the timer is late, then the callback will be called as soon as possible, and it will not be lost. In the worst case, when the timer has not been processed for more than one period (for periodic timers), the callbacks will be called one after the other without waiting for the set period. This can be bad for some applications, and the ``skip_unhandled_events`` option is introduced to eliminate this behavior. If ``skip_unhandled_events`` is set, then a periodic timer that has expired multiple times without being able to call the callback will still result in only one callback event once processing is possible.

Obtaining Current Time
----------------------

``esp_timer`` also provides a convenience function to obtain the time passed since start-up, with microsecond precision: :cpp:func:`esp_timer_get_time`. This function returns the number of microseconds since ``esp_timer`` was initialized, which usually happens shortly before ``app_main`` function is called.

Unlike ``gettimeofday`` function, values returned by :cpp:func:`esp_timer_get_time`:

- Start from zero after the chip wakes up from Deep-sleep
- Do not have timezone or DST adjustments applied

Application Example
-------------------

The following example illustrates the usage of ``esp_timer`` APIs: :example:`system/esp_timer`.


API Reference
-------------

.. include-build-file:: inc/esp_timer.inc


