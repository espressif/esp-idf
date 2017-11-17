TIMER
=====

Introduction
------------

The ESP32 chip contains two hardware timer groups. Each group has two general-purpose hardware timers. They are all 64-bit generic timers based on 16-bit prescalers and 64-bit auto-reload-capable up / down counters.


Functional Overview
-------------------

Typical steps to configure an operate the timer are described in the following sections:

* :ref:`timer-api-timer-initialization` - what parameters should be set up to get the timer working and what specific functionality is provided depending on the set up.
* :ref:`timer-api-timer-control` - how to read the timer's value, pause / start the timer, and change how it operates.
* :ref:`timer-api-alarms` - setting and using alarms.
* :ref:`timer-api-interrupts`- how to enable and use interrupts.


.. _timer-api-timer-initialization:

Timer Initialization
^^^^^^^^^^^^^^^^^^^^

The two timer groups on-board of the ESP32 are identified using :cpp:type:`timer_group_t`. Individual timers in a group are identified with :cpp:type:`timer_idx_t`. The two groups, each having two timers, provide the total of four individual timers to our disposal.

Before starting the timer, it should be initialized by calling :cpp:func:`timer_init`. This function should be provided with a structure :cpp:type:`timer_config_t` to define how timer should operate. In particular the following timer's parameters may be set:

    * **Divider**: How quickly the timer's counter is "ticking". This depends on the setting of :cpp:member:`divider`, that will be used as divisor of the incoming 80 MHz APB_CLK clock.
    * **Mode**: If the the counter is incrementing or decrementing, defined using :cpp:member:`counter_dir` by selecting one of values from :cpp:type:`timer_count_dir_t`.
    * **Counter Enable**: If the counter is enabled, then it will start incrementing / decrementing immediately after calling :cpp:func:`timer_init`. This action is set using :cpp:member:`counter_en` by selecting one of vales from :cpp:type:`timer_start_t`.
    * **Alarm Enable**: Determined by the setting of :cpp:member:`alarm_en`.
    * **Auto Reload**: Whether the counter should :cpp:member:`auto_reload` a specific initial value on the timer's alarm, or continue incrementing or decrementing.
    * **Interrupt Type**: Whether an interrupt is triggered on timer's alarm. Set the value defined in :cpp:type:`timer_intr_mode_t`.

To get the current values of the timers settings, use function :cpp:func:`timer_get_config`.


.. _timer-api-timer-control:

Timer Control
^^^^^^^^^^^^^

Once the timer is configured and enabled, it is already "ticking". To check it's current value call :cpp:func:`timer_get_counter_value` or :cpp:func:`timer_get_counter_time_sec`. To set the timer to specific starting value call :cpp:func:`timer_set_counter_value`.

The timer may be paused at any time by calling :cpp:func:`timer_pause`. To start it again call :cpp:func:`timer_start`.

To change how the timer operates you can call once more :cpp:func:`timer_init` described in section :ref:`timer-api-timer-initialization`. Another option is to use dedicated functions to change individual settings: 

    * **Divider** value - :cpp:func:`timer_set_divider`. **Note:** the timer should be paused when changing the divider to avoid unpredictable results. If the timer is already running, :cpp:func:`timer_set_divider` will first pause the timer, change the divider, and finally start the timer again.
    * **Mode** (whether the counter incrementing or decrementing) - :cpp:func:`timer_set_counter_mode`
    * **Auto Reload** counter on alarm - :cpp:func:`timer_set_auto_reload`


.. _timer-api-alarms:

Alarms
^^^^^^

To set an alarm, call function :cpp:func:`timer_set_alarm_value` and then enable it with :cpp:func:`timer_set_alarm`. The alarm may be also enabled during the timer initialization stage, when :cpp:func:`timer_init` is called.

After the alarm is enabled and the timer reaches the alarm value, depending on configuration, the following two actions may happen:

    * An interrupt will be triggered, if previously configured. See section :ref:`timer-api-interrupts` how to configure interrupts.
    * When :cpp:member:`auto_reload` is enabled, the timer's counter will be reloaded to start counting from specific initial value. The value to start should be set in advance with :cpp:func:`timer_set_counter_value`.

.. note::

    The alarm will be triggered immediately, if an alarm value is set and the timer has already passed this value.

To check what alarm value has been set up, call :cpp:func:`timer_get_alarm_value`.


.. _timer-api-interrupts:

Interrupts
^^^^^^^^^^

Registration of the interrupt handler for a specific timer group and timer is done be calling :cpp:func:`timer_isr_register`. 

To enable interrupts for a timer group call :cpp:func:`timer_group_intr_enable`. To do it for a specific timer, call :cpp:func:`timer_enable_intr`. Disabling of interrupts is done with corresponding functions :cpp:func:`timer_group_intr_disable` and :cpp:func:`timer_disable_intr`.

When servicing an interrupt within an ISR, the interrupt need to explicitly cleared. To do so, set the ``TIMERGN.int_clr_timers.tM`` structure defined in :component_file:`soc/esp32/include/soc/timer_group_struct.h`, where N is the timer group number [0, 1] and M is the timer number [0, 1]. For example to clear an interrupt for the timer 1 in the timer group 0, call the following::

    TIMERG0.int_clr_timers.t1 = 1

See the application example below how to use interrupts.


Application Example
-------------------

The 64-bit hardware timer example: :example:`peripherals/timer_group`.


API Reference
-------------

.. include:: /_build/inc/timer.inc
