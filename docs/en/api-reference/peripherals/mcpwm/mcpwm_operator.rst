=========================================
MCPWM Operator: Assemble an Output Stage
=========================================

.. contents::
    :local:
    :depth: 2

An operator is the container between a timer and its generators. It owns the comparators, generator actions, brake handling, dead-time routing, and carrier modulation. One timer can drive multiple operators in the same group, while an operator connects to exactly one timer.

If the timer is the clock source, the operator is the output-stage container. It lets several outputs share one time base while keeping protection, dead time, and carrier features grouped with the power stage they belong to.

Connect the building blocks
===========================

Create the operator in the same group as the timer, then connect them with :cpp:func:`mcpwm_operator_connect_timer()`. The connection must exist before the generator can use timer events.

.. code-block:: c

    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t oper_config = {
        .group_id = 0,
        .flags.update_gen_action_on_tez = true,
        .flags.update_dead_time_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&oper_config, &oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

The operator configuration is small, but a few fields do not appear in the example:

.. list::

    - :cpp:member:`group_id <mcpwm_operator_config_t::group_id>` — the MCPWM group the operator is allocated from. It must match the timer's group, because an operator can only connect to a timer inside the same group.
    - :cpp:member:`intr_priority <mcpwm_operator_config_t::intr_priority>` — the interrupt priority used by the brake event callbacks. Not setting it (``0``) lets the driver choose a low priority; raise it when brake notifications must preempt other ISRs.

The ``flags`` choose when new generator actions and dead-time settings take effect. They are all off by default, so changes apply immediately — possibly in the middle of a PWM cycle:

.. list::

    - :cpp:member:`update_gen_action_on_tez <mcpwm_operator_config_t::flags::update_gen_action_on_tez>`, :cpp:member:`update_gen_action_on_tep <mcpwm_operator_config_t::flags::update_gen_action_on_tep>`, and :cpp:member:`update_gen_action_on_sync <mcpwm_operator_config_t::flags::update_gen_action_on_sync>` — buffer generator action changes until the counter reaches zero, the peak, or a sync event.
    - :cpp:member:`update_dead_time_on_tez <mcpwm_operator_config_t::flags::update_dead_time_on_tez>`, :cpp:member:`update_dead_time_on_tep <mcpwm_operator_config_t::flags::update_dead_time_on_tep>`, and :cpp:member:`update_dead_time_on_sync <mcpwm_operator_config_t::flags::update_dead_time_on_sync>` — buffer dead-time changes the same way; see :ref:`dead time <mcpwm-dead-time>` for the update-point rules.

For a running power stage, use the zero (``tez``), peak (``tep``), or sync update point to avoid partial cycles.

One timer, multiple operators
=============================

The same timer can drive several operators, each producing a different waveform. This is useful for multi-phase inverters or multiple motors running at the same frequency but with independent duty cycles.

The reverse is also important: one operator connects to exactly one timer, so all comparators and generators inside that operator inherently share the same time base. That is why in-phase, complementary, and paired outputs are easy to build there.

.. code-block:: c

    mcpwm_oper_handle_t oper_b = NULL;
    mcpwm_operator_config_t oper_config_b = {
        .group_id = 0,
        .flags.update_gen_action_on_tez = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_operator(&oper_config_b, &oper_b));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper_b, timer));
    // Create separate comparators and generators under oper_b.

.. _mcpwm-brake:

Brake and safe output state
===========================

The operator turns a :doc:`fault <mcpwm_fault>` into a brake action. Arrange brake actions before starting PWM: this makes the reaction entirely hardware driven and avoids software latency in the fault path.

Recovery policy
---------------

.. list::

    - **CBC (cycle by cycle):** brakes while the fault is active and recovers at the configured timer zero or peak. This suits a transient current limit.
    - **OST (one shot):** stays braked after the fault disappears. Software must explicitly recover it. Use it for an interlock or serious over-current condition.

For CBC, set :cpp:member:`cbc_recover_on_tez <mcpwm_brake_config_t::flags::cbc_recover_on_tez>` or :cpp:member:`cbc_recover_on_tep <mcpwm_brake_config_t::flags::cbc_recover_on_tep>` to choose the boundary at which a cleared fault releases the outputs. A boundary avoids restoring a switch in the middle of a PWM cycle.

.. warning::

    Do not enable both ``cbc_recover_on_tez`` and ``cbc_recover_on_tep`` at the same time; choose the boundary that matches the waveform and gate-driver timing.

.. _mcpwm-brake-fault-connection:

Fault connection
----------------

Connect the fault to the operator, then specify the state of every generator during that brake mode. This example drives the raw generator low in both timer directions for OST braking:

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_operator_set_brake_on_fault(oper,
        &(mcpwm_brake_config_t) {
            .fault = fault,
            .brake_mode = MCPWM_OPER_BRAKE_MODE_OST,
        }));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(
        generator, MCPWM_GEN_BRAKE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_OST,
            MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_brake_event(
        generator, MCPWM_GEN_BRAKE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_DOWN, MCPWM_OPER_BRAKE_MODE_OST,
            MCPWM_GEN_ACTION_LOW)));

For a bridge, configure both generators with the same brake action. Confirm the electrical safe state at the gate driver; a logical low can be inverted by dead time, GPIO matrix, or external circuitry.

The distinction from a generator fault action is important: generator fault actions are best for a local immediate edge response, while operator brake defines the safe state, latch behavior, and recovery policy for the entire output stage. The primary protection path should usually use operator brake.

OST fault recovery
------------------

CBC recovers on its configured boundary after the fault goes inactive. For OST, remove and validate the root cause first, then call:

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_operator_recover_from_fault(oper, fault));

The call fails while the source is still active.

.. figure:: /../_static/mcpwm/brake_cbc_ost.svg
    :align: center
    :alt: A fault asserts while PWM runs. CBC holds the output at the brake level while the fault is active and resumes at the next cycle boundary; OST stays latched until software recovery.

    CBC brakes only while the fault is active and recovers at the next cycle boundary; OST stays latched until software recovery.

Brake event callbacks
---------------------

The operator can report brake events through the :cpp:member:`on_brake_cbc <mcpwm_operator_event_callbacks_t::on_brake_cbc>` and :cpp:member:`on_brake_ost <mcpwm_operator_event_callbacks_t::on_brake_ost>` callbacks. They run in ISR context; use them for notification, not blocking recovery.

.. code-block:: c

    mcpwm_operator_event_callbacks_t cbs = {
        .on_brake_cbc = my_brake_cbc_cb,
        .on_brake_ost = my_brake_ost_cb,
    };
    ESP_ERROR_CHECK(mcpwm_operator_register_event_callbacks(oper, &cbs, NULL));

.. _mcpwm-carrier:

Carrier modulation
==================

Carrier modulation superimposes a high-frequency carrier on an operator's PWM output. It is commonly used with transformer-isolated gate-drive schemes: even a base PWM held at 100% duty then contains transitions that can cross the isolation barrier. Configure the base PWM first; carrier settings affect the operator's all generators.

Carrier configuration
---------------------

.. code-block:: c

    mcpwm_carrier_config_t carrier = {
        .clk_src = MCPWM_CARRIER_CLK_SRC_DEFAULT,
        .frequency_hz = 100000,
        .duty_cycle = 0.5f,
        .first_pulse_duration_us = 20,
    };
    ESP_ERROR_CHECK(mcpwm_operator_apply_carrier(oper, &carrier));

.. figure:: /../_static/mcpwm/carrier_modulation.svg
    :align: center
    :alt: Carrier modulation of a 50% duty base PWM

    A 100 kHz carrier gates a 50% duty base PWM. The first pulse is stretched to 20 us (two carrier periods), and no chopping occurs while the base PWM is low.

Carrier parameters
------------------

.. list::

    - :cpp:member:`clk_src <mcpwm_carrier_config_t::clk_src>` selects the carrier clock source. It defaults to an internal PLL clock (e.g. PLL_F160M); some chips also expose RC_FAST or XTAL as alternatives. Different sources offer different resolution and power consumption. The default is fine for most applications; pick a different source only to avoid noise from a particular clock, when PLL precision is insufficient, or when power consumption matters.
    - :cpp:member:`frequency_hz <mcpwm_carrier_config_t::frequency_hz>` is the carrier frequency; select a value compatible with the isolation transformer, gate driver, switching loss budget, and target clock resolution.
    - :cpp:member:`duty_cycle <mcpwm_carrier_config_t::duty_cycle>` accepts the hardware steps 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, or 0.875, rather than an arbitrary ratio.
    - :cpp:member:`first_pulse_duration_us <mcpwm_carrier_config_t::first_pulse_duration_us>` controls the first pulse after modulation begins. It must be nonzero and at least one carrier period. A longer first pulse can help establish current in an inductive isolation path, but must stay within the gate-drive system's limits.
    - Use :cpp:member:`invert_before_modulate <mcpwm_carrier_config_t::flags::invert_before_modulate>` when the raw PWM needs a polarity change and :cpp:member:`invert_after_modulate <mcpwm_carrier_config_t::flags::invert_after_modulate>` when the modulated output needs one.

Pass ``NULL`` as the configuration to :cpp:func:`mcpwm_operator_apply_carrier` when carrier modulation is not needed.

API Reference
=============

MCPWM Operator Driver Functions
-------------------------------

.. include-build-file:: inc/mcpwm_oper.inc
