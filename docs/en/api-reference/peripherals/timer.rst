Timer
=====

:link_to_translation:`zh_CN:[中文]`

Introduction
------------

The ESP32 chip contains two hardware timer groups. Each group has two general-purpose hardware timers. They are all 64-bit generic timers based on 16-bit prescalers and 64-bit up / down counters which are capable of being auto-reloaded.


Functional Overview
-------------------

The following sections of this document cover the typical steps to configure and operate a timer:

* :ref:`timer-api-timer-initialization` - covers which parameters should be set up to get the timer working, and also what specific functionality is provided depending on the timer configuration.
* :ref:`timer-api-timer-control` - describes how to read a timer's value, pause or start a timer, and change how it operates.
* :ref:`timer-api-alarms` - shows how to set and use alarms.
* :ref:`timer-api-interrupts`- explains how to enable and use interrupts.


.. _timer-api-timer-initialization:

Timer Initialization
^^^^^^^^^^^^^^^^^^^^

The two ESP32 timer groups, with two timers in each, provide the total of four individual timers for use. An ESP32 timer group should be identified using :cpp:type:`timer_group_t`. An individual timer in a group should be identified with :cpp:type:`timer_idx_t`.

First of all, the timer should be initialized by calling the function :cpp:func:`timer_init` and passing a structure :cpp:type:`timer_config_t` to it to define how the timer should operate. In particular, the following timer parameters can be set:

    * **Divider**: Sets how quickly the timer's counter is "ticking". The setting :cpp:member:`divider` is used as a divisor of the incoming 80 MHz APB_CLK clock.
    * **Mode**: Sets if the counter should be incrementing or decrementing. It can be defined using :cpp:member:`counter_dir` by selecting one of the values from :cpp:type:`timer_count_dir_t`.
    * **Counter Enable**: If the counter is enabled, it will start incrementing / decrementing immediately after calling :cpp:func:`timer_init`. You can change the behavior with :cpp:member:`counter_en` by selecting one of the values from :cpp:type:`timer_start_t`.
    * **Alarm Enable**: Can be set using :cpp:member:`alarm_en`.
    * **Auto Reload**: Sets if the counter should :cpp:member:`auto_reload` the initial counter value on the timer's alarm or continue incrementing or decrementing.
    * **Interrupt Type**: Select which interrupt type should be triggered on the timer's alarm. Set the value defined in :cpp:type:`timer_intr_mode_t`.

To get the current values of the timer's settings, use the function :cpp:func:`timer_get_config`.


.. _timer-api-timer-control:

Timer Control
^^^^^^^^^^^^^

Once the timer is enabled, its counter starts running. To enable the timer, call the function :cpp:func:`timer_init` with :cpp:member:`counter_en` set to ``true``, or call :cpp:func:`timer_start`. You can specify the timer's initial counter value by calling :cpp:func:`timer_set_counter_value`. To check the timer's current value, call :cpp:func:`timer_get_counter_value` or :cpp:func:`timer_get_counter_time_sec`.

To pause the timer at any time, call :cpp:func:`timer_pause`. To resume it, call :cpp:func:`timer_start`.

To reconfigure the timer, you can call :cpp:func:`timer_init`. This function is described in Section :ref:`timer-api-timer-initialization`.

You can also reconfigure the timer by using dedicated functions to change individual settings:

=============  ===================================  ==========================================================================
Setting        Dedicated Function                   Description
=============  ===================================  ==========================================================================
Divider        :cpp:func:`timer_set_divider`        Change the rate of ticking. To avoid unpredictable results, the timer should be paused when changing the divider. If the timer is running, :cpp:func:`timer_set_divider` pauses it, change the setting, and start the timer again.
Mode           :cpp:func:`timer_set_counter_mode`   Set if the counter should be incrementing or decrementing
Auto Reload    :cpp:func:`timer_set_auto_reload`    Set if the initial counter value should be reloaded on the timer's alarm
=============  ===================================  ==========================================================================

.. _timer-api-alarms:

Alarms
^^^^^^

To set an alarm, call the function :cpp:func:`timer_set_alarm_value` and then enable the alarm using :cpp:func:`timer_set_alarm`. The alarm can also be enabled during the timer initialization stage, when :cpp:func:`timer_init` is called.

After the alarm is enabled, and the timer reaches the alarm value, the following two actions can occur depending on the configuration:

    * An interrupt will be triggered if previously configured. See Section :ref:`timer-api-interrupts` on how to configure interrupts.
    * When :cpp:member:`auto_reload` is enabled, the timer's counter will automatically be reloaded to start counting again from a previously configured value. This value should be set in advance with :cpp:func:`timer_set_counter_value`.

.. note::

    * If an alarm value is set and the timer has already reached this value, the alarm is triggered immediately.
    * Once triggered, the alarm is disabled automatically and needs to be re-enabled to trigger again.

To check the specified alarm value, call :cpp:func:`timer_get_alarm_value`.


.. _timer-api-interrupts:

Interrupts
^^^^^^^^^^

Registration of the interrupt handler for a specific timer or a timer group can be done by calling :cpp:func:`timer_isr_register`. 

To enable interrupts for a timer group, call :cpp:func:`timer_group_intr_enable`, for a specific timer call :cpp:func:`timer_enable_intr`.
To disable interrupts for a timer group, call :cpp:func:`timer_group_intr_disable`, for a specified timer, call :cpp:func:`timer_disable_intr`.

When handling an interrupt within an interrupt serivce routine (ISR), the interrupt status bit needs to be explicitly cleared. To do that, set the ``TIMERGN.int_clr_timers.tM`` structure, defined in :component_file:`soc/esp32/include/soc/timer_group_struct.h`. In this structure, ``N`` is the timer group number [0, 1], ``M`` is the timer number [0, 1]. For example, to clear an interrupt status bit for the timer 1 in the timer group 0, call the following::

    TIMERG0.int_clr_timers.t1 = 1

For more information on how to use interrupts, please see the application example below.


Application Example
-------------------

The 64-bit hardware timer example: :example:`peripherals/timer_group`.


API Reference
-------------

.. include:: /_build/inc/timer.inc
