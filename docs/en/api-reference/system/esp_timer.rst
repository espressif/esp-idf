ESP Timer (High Resolution Timer)
=================================

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_HR_TIMER:default = "SYSTIMER", esp32 = "LAC timer"}

{IDF_TARGET_HR_TIMER_Resolution:default = "52", esp32 = "64", esp32s2 = "64"}

.. only:: html

    This document covers the ESP-IDF feature called ESP Timer. The contents are as follows:

    .. contents::
        :local:
        :depth: 2


Overview
--------

The ESP Timer feature allows for creating software timers and invoking their callback functions (dispatching callbacks) on timeout. ESP Timer is useful when user software needs to perform delayed or periodic actions, such as delayed device start/stop or periodic sampling of sensor data.

ESP Timer hides the complexity associated with managing multiple timers, dispatching callbacks, accounting for clock frequency changes (if dynamic frequency scaling is enabled), and maintaining correct time after light sleep.

For application scenarios that require better real-time performance (such as generating waveforms) or configurable timer resolution, it is recommended that :doc:`GPTimer </api-reference/peripherals/gptimer>` be used instead. Also, GPTimer has features not available in ESP Timer, such as event capture.

Finally, FreeRTOS has its own software timers. As explained in :ref:`FreeRTOS Timers`, they have much lower resolution compared to ESP Timer, but FreeRTOS timers are portable (non-dependent on ESP-IDF) which might be an advantage in some cases.


Features and Concepts
---------------------

The ESP Timer API provides:

- One-shot and periodic timers
- Relative and absolute timing
- Multiple callback dispatch methods
- Handling overdue callbacks
- Bit range: {IDF_TARGET_HR_TIMER_Resolution} bits
- Time resolution: one microsecond


One-Shot and Periodic Timers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A one-shot timer invokes its callback function only once upon expiration and then stops operation. One-shot timers are useful for single delayed or timed actions, such as turning off a device or reading a sensor after a specified time interval or at a specified time point.

A periodic timer invokes its callback function upon expiration and restarts itself automatically, resulting in the callback function being invoked at a defined interval until the periodic timer is manually stopped. Periodic timers are useful for repeated actions, such as sampling sensor data, updating display information, or generating a waveform.


.. _Callback Methods:

Callback Dispatch Methods
^^^^^^^^^^^^^^^^^^^^^^^^^

Timer callbacks can be dispatched using the following methods:

- Task Dispatch method (default):

    - Dispatches timer callbacks from a single high-priority ESP Timer task (esp_timer task (notified by ISR) > callback).
    - Suitable for handling timer callbacks that are not time-critical.

- Interrupt Dispatch method (:cpp:enumerator:`ESP_TIMER_ISR <esp_timer_dispatch_t::ESP_TIMER_ISR>`):

    - Dispatches timer callbacks directly from an interrupt handler (ISR > callback).
    - Suitable for simple, low-latency timer callbacks which take a few microseconds to run.
    - Ensures shorter delay between the event and the callback execution.
    - Not affected by other active tasks.


Task Dispatch Specifics
~~~~~~~~~~~~~~~~~~~~~~~

The execution of callbacks in the ESP Timer task is serialized. Thus, when multiple timeouts occur simultaneously, the execution time of one callback will delay the execution of subsequent callbacks. For this reason, it is recommended to keep the callbacks short. If the callback needs to perform more work, the work should be deferred to a lower-priority task using FreeRTOS primitives, such as queues and semaphores.

If other FreeRTOS tasks with higher priority are running, such as an SPI flash operation, callback dispatching will be delayed until the ESP Timer task has a chance to run.

To maintain predictable and timely execution of tasks, callbacks should never attempt block (waiting for resources) or yield (give up control) operations, because such operations disrupt the serialized execution of callbacks.


Interrupt Dispatch Specifics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Timers using the Interrupt Dispatch method have their callbacks executed from an interrupt handler. As interrupts can preempt all tasks, the Interrupt Dispatch method offers lower latency. Interrupt dispatched timer callbacks should never attempt to block and should not attempt to trigger a context switch via ``portYIELD_FROM_ISR()``. Instead, the function :cpp:func:`esp_timer_isr_dispatch_need_yield` should be used. The context switch will happen after all timers using the ISR dispatch method are processed.

While using interrupt dispatched timers, the standard logging or debugging methods, such as ``printf`` should be avoided. To debug an application or display certain information in the console, the ESP-IDF logging macros should be used, such as :c:macro:`ESP_DRAM_LOGI`, :c:macro:`ESP_EARLY_LOGI`, etc. These macros are specifically designed to work in various contexts, including interrupt service routines.


Obtaining Current Time
^^^^^^^^^^^^^^^^^^^^^^

The time passed since the initialization of ESP Timer can be obtained using the convenience function :cpp:func:`esp_timer_get_time`. The initialization happens shortly before the ``app_main`` function is called. This function is fast and has no locking mechanisms that could potentially introduce delays or conflicts. As a result, it can be useful for fine-grained timing, with the accuracy of 1 μs, in tasks as well as in ISR routines.

Unlike the ``gettimeofday()`` function, :cpp:func:`esp_timer_get_time` has the following specifics:

- Upon wakeup from deep sleep, the initialization timer restarts from zero.
- The returned value has no timezone settings or daylight saving time adjustments.


System Integration
------------------

This section mainly covers some aspects of how to optimize the operation of ESP Timer and integrate it with other ESP-IDF features.


Timeout Value Limits
^^^^^^^^^^^^^^^^^^^^

As callback dispatching can never be instantaneous, the one-shot and periodic timers created with ESP Timer also have timeout value limits. These limits cannot be estimated precisely, because they depend on multiple factors.

For reference, the ESP32 running at 240 MHz and using the Task Dispatch method has the approximate minimum timeout values as follows:

* One-shot timers: ~20 μs

    * If :cpp:func:`esp_timer_start_once` is called, this is the earliest time after which the system will be able to dispatch a callback.

* Periodic timers: ~50 μs

    * Periodic software timers with a smaller timeout value would simply consume most of the CPU time, which is impractical.

The lower the CPU frequency, the higher the minimum timeout values will be. The general guideline is if the required timeout values are in the order of tens of microseconds, the user application needs to undergo thorough testing to ensure stable operation.

If the minimum timeout values slightly exceed the requirements, the Interrupt Dispatch method might offer an improvement.

.. only:: not SOC_PARLIO_SUPPORTED and SOC_RMT_SUPPORTED

    For even smaller timeout values, for example, to generate or receive waveforms or do bit banging, the resolution of ESP Timer may be insufficient. In this case, it is recommended to use dedicated peripherals, such as :doc:`GPTimer </api-reference/peripherals/gptimer>` or :doc:`RMT </api-reference/peripherals/rmt>`, and their DMA features if available.

.. only:: SOC_PARLIO_SUPPORTED

    For even smaller timeout values, for example, to generate or receive waveforms or do bit banging, the resolution of ESP Timer may be insufficient. In this case, it is recommended to use dedicated peripherals, such as :doc:`Parallel IO </api-reference/peripherals/parlio/index>`, and their DMA features if available.


Sleep Mode Considerations
^^^^^^^^^^^^^^^^^^^^^^^^^

If a timer is started, and there are no other tasks being executed during the wait time, the chip can be put into sleep to optimize power consumption.

Sleep can be induced in the following ways:

* **Automatic sleep** provided by :doc:`Power Management APIs <power_management>`: If no tasks are being executed, the chip can automatically enter light sleep and automatically wake up at the appropriate time for ESP Timer to dispatch a pending callback.
* **Manual sleep** provided by :doc:`Sleep Mode APIs <sleep_modes>`: The chip can be put into sleep regardless of whether other tasks are being executed.

For manually induced sleep, the following sleep modes exist:

* Deep-sleep mode: ESP Timer is deactivated

    The user application restarts from scratch upon wakeup from deep sleep. This makes deep sleep unsuitable for continuous ESP Timer operation. However, deep sleep can be used if the running timers are not expected to persist across wakeups.

* Light-sleep mode: ESP Timer is suspended

    While in light sleep, ESP Timer counter and callbacks are suspended. Timekeeping is done by the RTC timer. Once the chip is woken up, the counter of ESP Timer is automatically advanced by the amount of time spent in sleep, then timekeeping and callback execution is resumed.

    At this point, ESP Timer will attempt to dispatch all unhandled callbacks if there are any. It can potentially lead to the overflow of ESP Timer callback execution queue. This behavior may be undesirable for certain applications, and the ways to avoid it are covered in :ref:`Handling Callbacks in Light Sleep`.


.. _FreeRTOS Timers:

FreeRTOS Timers
^^^^^^^^^^^^^^^

Although FreeRTOS provides `software timers <https://www.freertos.org/RTOS-software-timer.html>`_, they have limitations:

- FreeRTOS timer resolution is bound by the `tick frequency <https://www.freertos.org/a00110.html#configTICK_RATE_HZ>`_, which is typically in the range of 100 to 1000 Hz.
- Timer callbacks are dispatched from a low-priority timer task that can be preempted by other tasks, leading to decreased timer precision and accuracy.

However, FreeRTOS timers are portable (non-dependent on ESP-IDF) and are written to be deterministic as they do not dispatch callbacks from ISRs.


.. only:: SOC_ETM_SUPPORTED and SOC_SYSTIMER_SUPPORT_ETM

    ETM Events
    ^^^^^^^^^^

    ESP Timer has connection to the :doc:`Event Task Matrix </api-reference/peripherals/etm>` (ETM) module. This module allows notifying a number of peripherals about events without involving CPU interrupts. Direct notifications reduce latency and decrease CPU workload. The function :cpp:func:`esp_timer_new_etm_alarm_event` can be called to get the corresponding ETM event handle.


Usage
-----

While setting up your ESP-IDF project, make sure to:

- Add required component dependencies to your ``CMakeLists.txt``.
- Include required header files in your ``.c`` files.
- (Optional) Set Kconfig options. For this, see :ref:`Kconfig Options <component-config-esp-timer-high-resolution-timer->` > *ESP Timer (High Resolution Timer)*


.. _General Procedure:

General Procedure
^^^^^^^^^^^^^^^^^

The general procedure to create, start, stop, and delete a timer is as follows:

1. Create a timer

    - Define a timer handle using the type :cpp:type:`esp_timer_handle_t`.
    - Set the timer configuration parameters by defining the structure :cpp:struct:`esp_timer_create_args_t` which also includes the callback function.

        .. note::

            It is recommended to keep callbacks as short as possible to avoid delaying other callbacks.

    - To create a timer, call the function :cpp:func:`esp_timer_create`.

2. Start the timer in one-shot mode or periodic mode depending on your requirements

    - To start the timer in one-shot mode, call :cpp:func:`esp_timer_start_once` to invoke the callback after a specified timeout duration, or :cpp:func:`esp_timer_start_once_at` to invoke it at a specific absolute time.
    - To start the timer in periodic mode, call :cpp:func:`esp_timer_start_periodic` to invoke the callback repeatedly at regular intervals, or :cpp:func:`esp_timer_start_periodic_at` to start the first invocation at a specific absolute time and then repeat at regular intervals; the timer will continue running until you explicitly stop it using :cpp:func:`esp_timer_stop`.

    .. note::

        When executing a start function, ensure that the timer is not running. If a timer is running, either call :cpp:func:`esp_timer_restart` / :cpp:func:`esp_timer_restart_at` or stop it first using :cpp:func:`esp_timer_stop` and then call one of the start functions.

3. Stop the timer

    - To stop the running timer, call the function :cpp:func:`esp_timer_stop`. But it does not guarantee that after this call, the callback will not be running one or more times. To check if the callback is not running after stopping the timer, you can use :cpp:func:`esp_timer_is_active`. Another approach is to use a blocking stop API.
    - Blocking the timer stop operation until any in-flight callback completes can be done using :cpp:func:`esp_timer_stop_blocking`.

4. Delete the timer

    - When the timer is no longer needed, delete it to free up memory using the function :cpp:func:`esp_timer_delete`.


.. _Using ESP_TIMER_ISR Callback Method:

Using the Interrupt Dispatch Method
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Out of the available :ref:`callback dispatch methods <Callback Methods>`, if you choose the Interrupt Dispatch method, follow these steps:

1. Set Kconfig options

    - Enable :ref:`CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD`.

2. Create a timer

    - Set the timer configuration parameters by defining the structure :cpp:struct:`esp_timer_create_args_t`:

    .. code-block:: c

        const esp_timer_create_args_t timer = {
            ... ,
            .dispatch_method = ESP_TIMER_ISR,
            ...
        };

    - To create a timer, call the function :cpp:func:`esp_timer_create`.

For further steps, refer to :ref:`General Procedure`.


.. _Handling Callbacks in Light Sleep:

Handling Callbacks in Light-sleep Mode
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Light sleep allows you to save power while maintaining the ability to quickly wake up for specific actions. To use ESP Timer in conjunction with Light-sleep mode, see :doc:`Sleep Mode APIs <sleep_modes>`.

During light sleep, to keep unhandled callbacks under control and avoid potential overflow of ESP Timer callback execution queue on wakeup, do one of the following:

- Prevent the invocation of callbacks in the first place: stop the timer before entering light sleep by using :cpp:func:`esp_timer_stop`.
- If calling the stop function is not desirable for any reason, use the option :cpp:member:`esp_timer_create_args_t::skip_unhandled_events`. In this case, if a periodic timer expires one or more times during light sleep, then only one callback is executed on wakeup.


Debugging Timers
^^^^^^^^^^^^^^^^

The function :cpp:func:`esp_timer_dump` allows dumping information about either all or only running timers: the parameters for timers, the number of times the timers were started, triggered, skipped, and time taken by timer callbacks to execute. This information can be helpful in debugging.

To debug timers, use the following procedure:

1. Set Kconfig options for more detailed output:

    - Enable :ref:`CONFIG_ESP_TIMER_PROFILING`.

    .. note::

        Enabling this option increases code size and heap memory usage.

2. Wherever required in your code, call the function :cpp:func:`esp_timer_dump` to print the information and use it to debug your timers.

3. Once debugging is complete, consider disabling :ref:`CONFIG_ESP_TIMER_PROFILING`.


Troubleshooting
---------------

Unstable Callback Dispatch Time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

While dispatching the same callback function repeatedly, if the response time varies considerably, try to stabilize it by doing the following:

.. list::

    - Use the :ref:`Interrupt Dispatch method <Using ESP_TIMER_ISR Callback Method>`.
    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - Use the Kconfig option :ref:`CONFIG_ESP_TIMER_TASK_AFFINITY` to run the ESP Timer task on any of the available cores.


Significant Delays while Dispatching Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If dispatching a callback function takes a considerable amount of time, the problem can lie in the callback function itself. More precisely, as all callback functions are processed one by one in a single esp_timer task, the delays might be caused by other callback functions earlier in the queue.

For this reason, make sure that all callback functions in your application can execute on their own quickly and without any blocking operations.


Repeated Callback Dispatches After Sleep
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the callback functions are executed repeatedly upon wakeup from sleep, see :ref:`Handling Callbacks in Light Sleep`.


Stack Overflow While Dispatching Callbacks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you see a stack overflow error when executing a callback function, consider reducing the stack usage within your callback function. Alternatively, try increasing the size of the ESP Timer task stack by adjusting :ref:`CONFIG_ESP_TIMER_TASK_STACK_SIZE`.


Application Examples
--------------------

- :example:`system/esp_timer` creates and starts one-shot and periodic software timers, shows how they work with Light-sleep mode, and then stops and deletes the timers.


API Reference
-------------

.. include-build-file:: inc/esp_timer.inc
