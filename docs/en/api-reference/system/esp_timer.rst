High Resolution Timer
=====================

Overview
--------

Although FreeRTOS provides software timers, these timers have a few limitations:

- Maximum resolution is equal to RTOS tick period
- Timer callbacks are dispatched from a low-priority task

Hardware timers are free from both of the limitations, but often they are less convenient to use. For example, application components may need timer events to fire at certain times in the future, but the hardware timer only contains one "compare" value used for interrupt generation. This means that some facility needs to be built on top of the hardware timer to manage the list of pending events can dispatch the callbacks for these events as corresponding hardware interrupts happen.

An interrupt level of the handler depends on the :ref:`CONFIG_ESP_TIMER_INTERRUPT_LEVEL` option. It allows to set this: 1, 2 or 3 level (by default 1). Raising the level, the interrupt handler can reduce the timer processing delay.

``esp_timer`` set of APIs provides one-shot and periodic timers, microsecond time resolution, and 64-bit range.

Internally, ``esp_timer`` uses a 64-bit hardware timer, where the implemention depends on :ref:`CONFIG_ESP_TIMER_IMPL`. Available options are:

.. list::

    :esp32: - LAC timer
    :esp32: - (legacy) FRC2 timer
    :not esp32: - SYSTIMER

.. only:: esp32

    .. note:: The FRC2 is a legacy option for ESP32 until v4.2, a 32-bit hardware timer was used. Starting at v4.2, use the new LAC timer option instead, it has a simpler implementation, and has smaller run time overhead because software handling of timer overflow is not needed.

Timer callbacks can dispatched by two methods:

- ``ESP_TIMER_TASK``
- ``ESP_TIMER_ISR``. Available only if :ref:`CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD` is enabled (by default disabled).

``ESP_TIMER_TASK``. Timer callbacks are dispatched from a high-priority ``esp_timer`` task. Because all the callbacks are dispatched from the same task, it is recommended to only do the minimal possible amount of work from the callback itself, posting an event to a lower priority task using a queue instead.

If other tasks with priority higher than ``esp_timer`` are running, callback dispatching will be delayed until ``esp_timer`` task has a chance to run. For example, this will happen if a SPI Flash operation is in progress.

``ESP_TIMER_ISR``. Timer callbacks are dispatched directly from the timer interrupt handler. This method is useful for some simple callbacks which aim for lower latency.

Creating and starting a timer, and dispatching the callback takes some time. Therefore there is a lower limit to the timeout value of one-shot ``esp_timer``. If :cpp:func:`esp_timer_start_once` is called with a timeout value less than 20us, the callback will be dispatched only after approximately 20us.

Periodic ``esp_timer`` also imposes a 50us restriction on the minimal timer period. Periodic software timers with period of less than 50us are not practical since they would consume most of the CPU time. Consider using dedicated hardware peripherals or DMA features if you find that a timer with small period is required.

Using ``esp_timer`` APIs
------------------------

Single timer is represented by :cpp:type:`esp_timer_handle_t` type. Timer has a callback function associated with it. This callback function is called from the ``esp_timer`` task each time the timer elapses.

- To create a timer, call :cpp:func:`esp_timer_create`.
- To delete the timer when it is no longer needed, call :cpp:func:`esp_timer_delete`.

The timer can be started in one-shot mode or in periodic mode.

- To start the timer in one-shot mode, call :cpp:func:`esp_timer_start_once`, passing the time interval after which the callback should be called. When the callback gets called, the timer is considered to be stopped.

- To start the timer in periodic mode, call :cpp:func:`esp_timer_start_periodic`, passing the period with which the callback should be called. The timer keeps running until :cpp:func:`esp_timer_stop` is called.

Note that the timer must not be running when :cpp:func:`esp_timer_start_once` or :cpp:func:`esp_timer_start_periodic` is called. To restart a running timer, call :cpp:func:`esp_timer_stop` first, then call one of the start functions.

Callback functions
------------------

.. note: Keep the callback functions as short as possible otherwise it will affect all timers.

Timer callbacks which are processed by ``ESP_TIMER_ISR`` method should not call the context switch call - ``portYIELD_FROM_ISR()``, instead of this you should use the :cpp:func:`esp_timer_isr_dispatch_need_yield` function.
The context switch will be done after all ISR dispatch timers have been processed, if required by the system.

esp_timer during the light sleep
--------------------------------

During light sleep, the esp_timer counter stops and no callback functions are called.
Instead, the time is counted by the RTC counter. Upon waking up, the system gets the difference
between the counters and calls a function that advances the esp_timer counter.
Since the counter has been advanced, the system starts calling callbacks that were not called during sleep.
The number of callbacks depends on the duration of the sleep and the period of the timers. It can lead to overflow of some queues.
This only applies to periodic timers, one-shot timers will be called once.

This behavior can be changed by calling :cpp:func:`esp_timer_stop` before sleeping.
In some cases, this can be inconvenient, and instead of the stop function,
you can use the `skip_unhandled_events` option during :cpp:func:`esp_timer_create`.
When the `skip_unhandled_events` is true, if a periodic timer expires one or more times during light sleep
then only one callback is called on wake.

Using the `skip_unhandled_events` option with `automatic light sleep` (see :doc:`Power Management APIs <power_management>`) helps to reduce the consumption of the system when it is in light sleep. The duration of light sleep is also determined by esp_timers. Timers with `skip_unhandled_events` option will not wake up the system.

Handling callbacks
------------------

esp_timer is designed to achieve a high-resolution low latency timer and the ability to handle delayed events.
If the timer is late then the callback will be called as soon as possible, it will not be lost.
In the worst case, when the timer has not been processed for more than one period (for periodic timers),
in this case the callbacks will be called one after the other without waiting for the set period.
This can be bad for some applications, and the `skip_unhandled_events` option was introduced to eliminate this behavior.
If `skip_unhandled_events` is set then a periodic timer that has expired multiple times without being able to call
the callback will still result in only one callback event once processing is possible.

Obtaining Current Time
----------------------

``esp_timer`` also provides a convenience function to obtain the time passed since start-up, with microsecond precision: :cpp:func:`esp_timer_get_time`. This function returns the number of microseconds since ``esp_timer`` was initialized, which usually happens shortly before ``app_main`` function is called.

Unlike `gettimeofday` function, values returned by :cpp:func:`esp_timer_get_time`:

- Start from zero after the chip wakes up from deep sleep
- Do not have timezone or DST adjustments applied

Application Example
-------------------

The following example illustrates usage of ``esp_timer`` APIs: :example:`system/esp_timer`.


API Reference
-------------

.. include-build-file:: inc/esp_timer.inc


