============================================
MCPWM Comparator: Turn a Ratio into an Edge
============================================

.. contents::
    :local:
    :depth: 2

A comparator emits an event when the timer count reaches ``cmp_ticks``. A generator converts that event into a GPIO transition. In the usual up-counting PWM arrangement, the comparator value is the high-time in ticks.

In practice, a comparator is how you turn "I want the edge earlier, later, narrower, or wider" into hardware timing. Runtime duty control is usually nothing more than changing the comparator threshold.

Set a 30% duty cycle
=====================

Create a comparator from an existing operator, then set its threshold. With the 50-tick timer from the :doc:`timer page <mcpwm_timer>`, a value of 15 represents 30% duty. The waveform below shows the compare event at tick 15 — the generator can use this to end the high pulse.

.. figure:: /../_static/mcpwm/compare_event.svg
    :align: center
    :alt: Timer counts up; the comparator fires at tick 15. The generator turns this into a falling edge.

    Timer counts up; the comparator fires at tick 15. The generator turns this into a falling edge.

.. code-block:: c

    mcpwm_cmpr_handle_t comparator = NULL;
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true, // Change duty only at cycle boundary
    };
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 15));

The comparator configuration has only one field besides the flags:

.. list::

    - :cpp:member:`intr_priority <mcpwm_comparator_config_t::intr_priority>` — the interrupt priority used by the :cpp:member:`on_reach <mcpwm_comparator_event_callbacks_t::on_reach>` callback. Not setting it (``0``) lets the driver choose a low priority; raise it when the callback must preempt other ISRs.
    - :cpp:member:`flags <mcpwm_comparator_config_t::flags>` — the update points explained below. The example enables :cpp:member:`update_cmp_on_tez <mcpwm_comparator_config_t::flags::update_cmp_on_tez>`, which is the usual choice for changing duty at the cycle boundary.

For a runtime duty request in percent, calculate ``period_ticks * percent / 100``. Keep the result within the timer period.

This is why duty updates normally change the comparator rather than the generator actions: actions describe the waveform rule, while the comparator is the runtime edge position.

Why buffer the update?
======================

Updating a comparator immediately can move an edge in the middle of the active cycle. :cpp:member:`update_cmp_on_tez <mcpwm_comparator_config_t::flags::update_cmp_on_tez>` buffers it until the counter reaches zero, :cpp:member:`update_cmp_on_tep <mcpwm_comparator_config_t::flags::update_cmp_on_tep>` until it reaches the peak, and :cpp:member:`update_cmp_on_sync <mcpwm_comparator_config_t::flags::update_cmp_on_sync>` until a sync event. In up-counting or down-counting mode the peak coincides with the cycle boundary, so ``tez`` and ``tep`` select almost the same update point; only in up-down mode does the peak sit at the midpoint of the cycle, making ``tez`` and ``tep`` two distinct update points. The buffered choice is usually the right one for motors and power conversion.

Two comparators for pulse placement
====================================

A single comparator gives one edge per cycle. With two comparators in the same operator, you can place a pulse anywhere inside the period — one comparator opens the pulse and the other closes it. This is useful for sampling windows, trigger signals, or asymmetric dead-time compensation.

That pattern appears often in motor control, for example when an ADC sample window should sit away from switching noise, or when an external device needs a timing pulse that is not tied to the PWM boundary.

.. code-block:: c

    mcpwm_cmpr_handle_t cmp_a, cmp_b;
    mcpwm_new_comparator(oper, &comparator_config, &cmp_a);
    mcpwm_new_comparator(oper, &comparator_config, &cmp_b);
    mcpwm_comparator_set_compare_value(cmp_a, 10);
    mcpwm_comparator_set_compare_value(cmp_b, 30);

Use compare events as a timing marker
=====================================

The :cpp:member:`on_reach <mcpwm_comparator_event_callbacks_t::on_reach>` callback fires when the compare value is reached. This is useful when software must observe a precise point in the PWM cycle. Register it before starting time-critical work. The callback runs in ISR context, so keep it short.

.. code-block:: c

    static bool IRAM_ATTR on_compare(mcpwm_cmpr_handle_t cmpr,
                                     const mcpwm_compare_event_data_t *edata,
                                     void *user_ctx)
    {
        // Signal a task or trigger only ISR-safe work.
        return false;
    }

    mcpwm_comparator_event_callbacks_t callbacks = { .on_reach = on_compare };
    ESP_ERROR_CHECK(mcpwm_comparator_register_event_callbacks(comparator,
                                                              &callbacks, NULL));

Comparator kinds
================

The **operator comparator**, created with :cpp:func:`mcpwm_new_comparator()`, drives the generators so it can shape the PWM output.

.. only:: SOC_MCPWM_SUPPORT_EVENT_COMPARATOR

    The **event comparator**, created with :cpp:func:`mcpwm_new_event_comparator()`. Its compare event only reaches other peripherals through :doc:`ETM </api-reference/peripherals/etm>`; it never drives a generator and does not affect the PWM output.

    .. note::

        The name is the trap: the operator comparator can also produce ETM events, so the event comparator is not the only way to link MCPWM to ETM. The event comparator exists to *supplement* the operator comparator, not to replace it. An event comparator consumes none of the operator-comparator slots and never moves a PWM edge, which makes it the flexible choice when you need an extra compare point purely as a timing marker — for example, to find a sampling window for an ADC trigger that must not disturb the PWM waveform.

    Both types accept the same compare value and ETM event setup:

    .. code-block:: c

        mcpwm_event_comparator_config_t evt_cmp_cfg = {};
        mcpwm_cmpr_handle_t evt_cmpr = NULL;
        ESP_ERROR_CHECK(mcpwm_new_event_comparator(oper, &evt_cmp_cfg, &evt_cmpr));
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(evt_cmpr, 25));

        esp_etm_event_handle_t evt = NULL;
        ESP_ERROR_CHECK(mcpwm_comparator_new_etm_event(evt_cmpr,
            &(mcpwm_cmpr_etm_event_config_t){ .event_type = MCPWM_CMPR_ETM_EVENT_EQUAL },
            &evt));

API Reference
=============

MCPWM Comparator Driver Functions
----------------------------------

.. include-build-file:: inc/mcpwm_cmpr.inc
