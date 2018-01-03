High Resolution Timer
=====================

Overview
--------

Although FreeRTOS provides software timers, these timers have a few limitations:

- Maximum resolution is equal to RTOS tick period
- Timer callbacks are dispatched from a low-priority task

Hardware timers are free from both of the limitations, but often they are less convenient to use. For example, application components may need timer events to fire at certain times in the future, but the hardware timer only contains one "compare" value used for interrupt generation. This means that some facility needs to be built on top of the hardware timer to manage the list of pending events can dispatch the callbacks for these events as corresponding hardware interrupts happen.

``esp_timer`` set of APIs provide such facility. Internally, ``esp_timer`` uses a 32-bit hardware timer (FRC1, "legacy" timer). ``esp_timer`` provides one-shot and periodic timers, microsecond time resolution, and 64-bit range.

Timer callbacks are dispatched from a high-priority ``esp_timer`` task. Because all the callbacks are dispatched from the same task, it is recommended to only do the minimal possible amount of work from the callback itself, posting an event to a lower priority task using a queue instead.

.. note: Provisions are made to dispatch some simple callbacks directly from the interrupt handler, if needed. However this option is not implemented at the moment.

Using ``esp_timer`` APIs
------------------------

Single timer is represented by :cpp:type:`esp_timer_handle_t` type. Timer has a callback function associated with it. This callback function is called from the ``esp_timer`` task each time the timer elapses.

- To create a timer, call :cpp:func:`esp_timer_create`.
- To delete the timer when it is no longer needed, call :cpp:func:`esp_timer_delete`.

The timer can be started in one-shot mode or in periodic mode.

- To start the timer in one-shot mode, call :cpp:func:`esp_timer_start_once`, passing the time interval after which the callback should be called. When the callback gets called, the timer is considered to be stopped.

- To start the timer in periodic mode, call :cpp:func:`esp_timer_start_periodic`, passing the period with which the callback should be called. The timer keeps running until :cpp:func:`esp_timer_stop` is called.

Note that the timer must not be running when :cpp:func:`esp_timer_start_once` or :cpp:func:`esp_timer_start_periodic` is called. To restart a running timer, call :cpp:func:`esp_timer_stop` first, then call one of the start functions.

Obtaining Current Time
----------------------

``esp_timer`` also provides a convenience function to obtain the time passed since start-up, with microsecond precision: :cpp:func:`esp_timer_get_time`. This function returns the number of microseconds since ``esp_timer`` was initialized, which usually happens shortly before ``app_main`` function is called.

Unlike `gettimeofday` function, values returned by :cpp:func:`esp_timer_get_time`:

- Start from zero after the chip wakes up from deep sleep
- Do not have timezone or DST adjustments applied


API Reference
-------------

.. include:: /_build/inc/esp_timer.inc


