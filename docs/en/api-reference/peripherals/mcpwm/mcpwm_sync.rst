========================================
MCPWM Synchronization: Align PWM Phases
========================================

.. contents::
    :local:
    :depth: 2

Why sync is needed
==================

Each MCPWM timer is an independent hardware counter. When you call :cpp:func:`mcpwm_timer_start()` on two timers, the two writes are issued sequentially by the CPU — the second timer starts a few dozen CPU cycles after the first. Even if both are configured with the same period, their counters will be at different positions relative to the cycle, and the phase relationship between their PWM outputs is unpredictable.

Synchronization solves this by loading a chosen count and direction into a **running** timer when a sync edge arrives. The timers must already be running; sync does not start or stop them. It is a runtime phase correction mechanism.

If the sync edge arrives every period (for example, from a timer sync source at TEZ), the correction repeats each cycle, keeping the phase locked indefinitely. This is the typical use case: one timer acts as the reference, and other timers re-align to it on every period.

MCPWM provides three types of sync sources. All sources produce a handle of type :cpp:type:`mcpwm_sync_handle_t`, and any source can feed any timer in the same group.

GPIO sync source
================

A GPIO sync source reacts to an edge on an external pin — useful when an external controller, sensor, or encoder provides a periodic reference.

.. code-block:: c

    mcpwm_sync_handle_t sync = NULL;
    ESP_ERROR_CHECK(mcpwm_new_gpio_sync_src(
        &(mcpwm_gpio_sync_src_config_t) {
            .group_id = 0,
            .gpio_num = 5,
            .flags.active_neg = false,
        }, &sync));

The GPIO sync source configuration is small:

.. list::

    - :cpp:member:`group_id <mcpwm_gpio_sync_src_config_t::group_id>` — the MCPWM group the source belongs to. It must match the group of every timer that receives this sync.
    - :cpp:member:`gpio_num <mcpwm_gpio_sync_src_config_t::gpio_num>` — the GPIO carrying the sync signal.
    - :cpp:member:`active_neg <mcpwm_gpio_sync_src_config_t::flags::active_neg>` — by default the rising edge is the active edge; set it to treat the falling edge as active instead.

Software sync source
====================

A software sync source produces a sync edge on demand from application code. It has no configuration fields; create it and activate it when needed.

.. code-block:: c

    mcpwm_sync_handle_t soft_sync = NULL;
    ESP_ERROR_CHECK(mcpwm_new_soft_sync_src(NULL, &soft_sync));

    // later, when the application decides to synchronize:
    ESP_ERROR_CHECK(mcpwm_soft_sync_activate(soft_sync));

.. note::

    Activate the soft sync only after binding it to a timer via :cpp:func:`mcpwm_timer_set_phase_on_sync()` or :cpp:func:`mcpwm_capture_timer_set_phase_on_sync()`. The driver does not assign a timer at creation time; calling :cpp:func:`mcpwm_soft_sync_activate()` before binding is undefined behavior.

This is useful when timers are already running and the application needs to trigger a one-time phase correction — for example, after recovering from a fault, or before starting a new control cycle. Because the sync is one-shot, the phase relationship will drift over time if no further sync edges arrive. For sustained phase lock, use a periodic source (GPIO or timer sync).

Timer sync source
=================

A timer sync source emits a sync edge when the timer reaches a chosen event — for example, every time the timer hits zero (TEZ). This lets one timer act as a periodic reference for other timers, keeping their phase locked every cycle.

.. code-block:: c

    mcpwm_sync_handle_t timer_sync = NULL;
    ESP_ERROR_CHECK(mcpwm_new_timer_sync_src(
        timer_a,
        &(mcpwm_timer_sync_src_config_t) {
            .timer_event = MCPWM_TIMER_EVENT_EMPTY,
        },
        &timer_sync));

.. list::

    - :cpp:member:`timer_event <mcpwm_timer_sync_src_config_t::timer_event>` — the timer event that triggers the sync output. Common choices are :cpp:enumerator:`MCPWM_TIMER_EVENT_EMPTY` (zero) for the start of each period or :cpp:enumerator:`MCPWM_TIMER_EVENT_PEAK` for the peak value. In up-counting mode the peak is the period boundary; in up-down mode the peak is the midpoint of the period.
    - :cpp:member:`propagate_input_sync <mcpwm_timer_sync_src_config_t::flags::propagate_input_sync>` — when set, the timer forwards its own received input sync to its sync output, enabling a chain of timers without extra GPIO wiring. In this mode the hardware selects the input sync as the output source, so the :cpp:member:`timer_event` field is ignored.

A timer can create at most one sync source. Multiple timers can receive the same sync source.

Because the timer sync source fires every period, the receiving timer gets corrected on every cycle. This is the most common way to maintain a stable phase relationship between multiple PWM channels.

Set the receiving phase
=======================

No matter which source type you chose, the receiving timer uses the same API. Call :cpp:func:`mcpwm_timer_set_phase_on_sync()` to configure what happens when the sync edge arrives. The timer must already be running for the sync to take effect.

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_timer_set_phase_on_sync(timer,
        &(mcpwm_timer_sync_phase_config_t) {
            .sync_src = sync,
            .count_value = 25,
            .direction = MCPWM_TIMER_DIRECTION_UP,
        }));

.. list::

    - :cpp:member:`sync_src <mcpwm_timer_sync_phase_config_t::sync_src>` — the source object. Set it to ``NULL`` to detach synchronization.
    - :cpp:member:`count_value <mcpwm_timer_sync_phase_config_t::count_value>` — the count loaded when the sync event arrives. Keep it within the timer period.
    - :cpp:member:`direction <mcpwm_timer_sync_phase_config_t::direction>` — the counting direction after loading.

Two outputs with a 90-degree phase shift
========================================

Now that you know all three source types and how to set the receiving phase, here is a complete example. It uses a timer sync source: ``timer_a`` emits a sync every time it reaches zero, and ``timer_b`` receives that sync and loads ``count_value = 25``, producing a 90-degree phase lag. Because the sync repeats each period, the phase relationship between the two channels is maintained indefinitely.

.. code-block:: c

    mcpwm_timer_handle_t timer_a = NULL;
    mcpwm_timer_handle_t timer_b = NULL;
    mcpwm_sync_handle_t timer_a_sync = NULL;

    // timer_a and timer_b already exist, both with period_ticks = 100

    ESP_ERROR_CHECK(mcpwm_new_timer_sync_src(
        timer_a,
        &(mcpwm_timer_sync_src_config_t) {
            .timer_event = MCPWM_TIMER_EVENT_EMPTY,
        },
        &timer_a_sync));

    ESP_ERROR_CHECK(mcpwm_timer_set_phase_on_sync(timer_b,
        &(mcpwm_timer_sync_phase_config_t) {
            .sync_src = timer_a_sync,
            .count_value = 25,
            .direction = MCPWM_TIMER_DIRECTION_UP,
        }));

    // timer_a emits sync at TEZ; timer_b receives it and continues from tick 25.

Understand lead and lag
-----------------------

In the sketch below, ``PWM_A`` starts its cycle first and ``PWM_B`` appears one quarter cycle later. That means ``PWM_B`` lags ``PWM_A`` by 90 degrees; equivalently, ``PWM_A`` leads ``PWM_B`` by 90 degrees.

.. figure:: /../_static/mcpwm/phase_shift.svg
    :align: center
    :alt: PWM phase shift 90 degree lag

    PWM_A and PWM_B with a 90-degree phase shift: PWM_B starts 25 ticks after PWM_A.

Other considerations
====================

The capture timer can use the same source through :cpp:func:`mcpwm_capture_timer_set_phase_on_sync()`; capture always counts up. The receiver and source must remain in the same group. Delete a source only after detaching or deleting every object that uses it.

API Reference
=============

MCPWM Synchronization Driver Functions
--------------------------------------

.. include-build-file:: inc/mcpwm_sync.inc
