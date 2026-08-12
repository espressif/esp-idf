===============================
MCPWM Timer: Set the Frequency
===============================

.. contents::
    :local:
    :depth: 2

The timer is the time base for every PWM output attached to its operator. It counts ticks at :cpp:member:`resolution_hz <mcpwm_timer_config_t::resolution_hz>` and wraps around at :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>`. Choose the resolution first — it determines the finest step of edge placement — then choose the period for the target frequency.

For a servo, speed loop, or inverter, the timer answers the two most basic questions: how fine is one tick, and how long is one PWM cycle. Comparators and generators only place edges on top of that time base.

Build a 20 kHz time base
=========================

For an up-counting timer, ``period_ticks = resolution_hz / frequency_hz``. The following timer has a 1 MHz tick (one microsecond per tick) and a 50-tick period, giving 20 kHz. The diagram below shows the counter climbing from 0 to 50, then resetting — the ``TEZ`` (timer event zero) and ``TEP`` (timer event peak) markers are the two boundaries that generators use.

.. figure:: /../_static/mcpwm/timer_up_count.svg
    :align: center
    :alt: Up-counting timer: the counter forms a sawtooth, rising from 0 to 50, firing TEZ at zero and TEP at peak.

    Up-counting timer: the counter forms a sawtooth, rising from 0 to 50, firing TEZ at zero and TEP at peak.

.. code-block:: c

    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .period_ticks = 50,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

The timer configuration is worth reading field by field, because a few important knobs are not shown in the example:

.. list::

    - :cpp:member:`group_id <mcpwm_timer_config_t::group_id>` — the MCPWM group the timer is allocated from. Chips may expose more than one groups; each group bundles timers, operators, comparators, and generators that share clock dividers. ``0`` selects the first group, which is enough for most designs.
    - :cpp:member:`clk_src <mcpwm_timer_config_t::clk_src>` — the clock that feeds the timer. :c:macro:`MCPWM_TIMER_CLK_SRC_DEFAULT` selects a PLL clock and is right for almost every application. On targets with extra sources, you can pick one explicitly — for example to keep the timer counting when the PLL is switched off, such as during light sleep.
    - :cpp:member:`resolution_hz <mcpwm_timer_config_t::resolution_hz>` — the tick rate of the counter. One tick lasts ``1 / resolution_hz`` seconds, so 1 MHz means one microsecond per tick. This sets the finest edge step available to the comparator.
    - :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` — the length of one full PWM cycle in ticks. The frequency is ``resolution_hz / period_ticks``.
    - :cpp:member:`count_mode <mcpwm_timer_config_t::count_mode>` — whether the counter counts up only (edge-aligned PWM) or up and down (center-aligned PWM). See :ref:`Counting modes and waveforms <mcpwm-timer-counting-modes>` for the two shapes; the hardware also supports counting down.
    - :cpp:member:`intr_priority <mcpwm_timer_config_t::intr_priority>` — the interrupt priority used by the timer callbacks. Not setting it (``0``) lets the driver choose a low priority; raise it when a callback must preempt other ISRs, for example in tightly timed motor control.

The example does not touch :cpp:member:`flags <mcpwm_timer_config_t::flags>`, so all of them are off — which is the safe default. Two of them are worth knowing:

.. list::

    - :cpp:member:`update_period_on_empty <mcpwm_timer_config_t::flags::update_period_on_empty>` and :cpp:member:`update_period_on_sync <mcpwm_timer_config_t::flags::update_period_on_sync>` — off by default, so :cpp:func:`mcpwm_timer_set_period()` takes effect immediately. Turn them on to defer frequency changes to a safe boundary; see :ref:`Safe frequency updates <mcpwm-timer-safe-update>`.
    - :cpp:member:`allow_pd <mcpwm_timer_config_t::flags::allow_pd>` — lets the MCPWM power domain switch off during sleep. The driver then backs up and restores the timer registers around the sleep transition, saving power at the cost of extra RAM.

Do not start the timer yet. First create and connect the operator, comparator, and generator (see the following pages), then enable and start:

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

:cpp:func:`mcpwm_timer_enable()` activates the services the timer needs to run: it enables the timer interrupt and, with power management on, holds the group's power-management lock so clock scaling cannot disturb PWM timing. :cpp:func:`mcpwm_timer_start_stop()` then starts and later stops the counter. Call :cpp:func:`mcpwm_timer_disable()` to reverse the enable before freeing the timer with :cpp:func:`mcpwm_del_timer()`.

The third argument of :cpp:func:`mcpwm_timer_start_stop()` selects the stop behavior:

.. list::

    - :c:macro:`MCPWM_TIMER_START_NO_STOP` — runs continuously until you explicitly stop it.
    - :c:macro:`MCPWM_TIMER_START_STOP_EMPTY` — stops automatically when the next count reaches zero (TEZ). Use this for a single-shot or synchronized start where the cycle should complete before stopping.
    - :c:macro:`MCPWM_TIMER_START_STOP_FULL` — stops automatically when the next count reaches the peak (TEP). Use this for a single cycle that ends at the period boundary.

.. _mcpwm-timer-counting-modes:

Counting modes and waveforms
============================

In **up mode**, the counter counts from 0 to :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` and resets. The waveform is a sawtooth and the PWM edges align to one side of the period — this is called *edge-aligned* PWM.

In **up-down mode**, the counter counts up to ``period_ticks / 2`` and then down to 0. The waveform is a triangle and the PWM edges are centered around the middle of the period — *center-aligned* PWM. Center-aligned PWM is preferred for motor control because it produces less harmonic distortion.

.. figure:: /../_static/mcpwm/timer_up_down_count.svg
    :align: center
    :alt: Up-down counting: the counter forms a triangle, rising to 25 (half of 50), then falling back to 0.

    Up-down counting: the counter forms a triangle, rising to 25 (half of 50), then falling back to 0.

The frequency is still ``resolution_hz / period_ticks`` in both modes. Choose a resolution high enough for the duty precision you need, then choose a period for the desired frequency.

.. important::

    :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` is the total number of ticks in one full PWM cycle. It is not always the same thing as the timer peak value.

    .. list::

        - In ``MCPWM_TIMER_COUNT_MODE_UP``, the counter runs ``0 -> period_ticks``.
        - In ``MCPWM_TIMER_COUNT_MODE_UP_DOWN``, the hardware peak is ``period_ticks / 2``, and the full cycle is ``0 -> peak -> 0``.

    For example, with :cpp:member:`resolution_hz <mcpwm_timer_config_t::resolution_hz>` = 1 MHz and :cpp:member:`period_ticks <mcpwm_timer_config_t::period_ticks>` = 50: up mode counts ``0 -> 50``, while up-down mode counts ``0 -> 25 -> 0``. Both still take 50 microseconds for a full cycle, so both are 20 kHz. What changes is the edge placement, not the period length.

.. _mcpwm-timer-safe-update:

Safe frequency updates
======================

:cpp:func:`mcpwm_timer_set_period()` takes effect immediately by default. That can truncate the current cycle and produce a runt pulse. Set :cpp:member:`update_period_on_empty <mcpwm_timer_config_t::flags::update_period_on_empty>` to defer the new period until the counter reaches zero, or :cpp:member:`update_period_on_sync <mcpwm_timer_config_t::flags::update_period_on_sync>` to defer it until a sync event. When changing the period, also scale the comparator threshold if the duty ratio must remain unchanged:

.. code-block:: c

    // Keep 40 % duty while changing a 50-tick period to 100 ticks.
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 40));
    ESP_ERROR_CHECK(mcpwm_timer_set_period(timer, 100));

In most runtime tuning paths, change the comparator to change duty and touch the timer only when the PWM frequency itself must change. Motor-control and power-conversion applications should usually combine this with :cpp:member:`update_period_on_empty <mcpwm_timer_config_t::flags::update_period_on_empty>` or a sync-triggered update to avoid mid-cycle changes.

Timer event callbacks
=====================

The timer can notify your application at peak (:cpp:member:`on_full <mcpwm_timer_event_callbacks_t::on_full>`), zero (:cpp:member:`on_empty <mcpwm_timer_event_callbacks_t::on_empty>`), or when it stops (:cpp:member:`on_stop <mcpwm_timer_event_callbacks_t::on_stop>`). Register callbacks before enabling the timer. They run in ISR context: do not block, allocate memory, or call normal FreeRTOS APIs; use ``...FromISR`` variants when needed.

.. note::

    The timer and capture timer may share a divider with other objects in the same group. When one group needs several resolutions, create objects in monotonic requested-resolution order to avoid divider conflicts. See :doc:`advanced topics <mcpwm_advanced>` for the full rule.

.. code-block:: c

    static bool IRAM_ATTR on_timer_empty(mcpwm_timer_handle_t timer,
                                         const mcpwm_timer_event_data_t *edata,
                                         void *user_ctx)
    {
        BaseType_t high_task_woken = pdFALSE;
        vTaskNotifyGiveFromISR((TaskHandle_t)user_ctx, &high_task_woken);
        return high_task_woken == pdTRUE;
    }

    mcpwm_timer_event_callbacks_t cbs = { .on_empty = on_timer_empty };
    ESP_ERROR_CHECK(mcpwm_timer_register_event_callbacks(timer, &cbs,
                                                          xTaskGetCurrentTaskHandle()));

The :doc:`synchronization <mcpwm_sync>` page shows how a timer can reset to a chosen phase on a sync edge.

API Reference
=============

MCPWM Timer Driver Functions
----------------------------

.. include-build-file:: inc/mcpwm_timer.inc
