=========================================
MCPWM Generator: Create the PWM Waveform
=========================================

.. contents::
    :local:
    :depth: 2

The generator is the final digital output. It does not have a fixed duty-cycle setting; instead, you teach it what level to drive at timer and comparator events. This makes simple PWM easy and leaves room for asymmetric, complementary, and phase-sensitive waveforms.

This is one of the biggest differences from a simpler PWM peripheral: MCPWM does not just ask for frequency and duty, it lets you describe what should happen at each important event. That adds concepts up front, but gives much tighter control over waveform structure.

Your first PWM output
=====================

This is the completion of the :doc:`timer <mcpwm_timer>`/:doc:`operator <mcpwm_operator>`/:doc:`comparator <mcpwm_cmpr>` setup in the preceding pages. At timer zero, drive the GPIO high. When the comparator reaches 15, drive it low. With a 50-tick period, the output is high for 15 ticks (30%).

Application: basic single-output PWM
------------------------------------

Use this for the simplest single-output PWM cases, such as an RC servo control signal, LED dimming, or a basic duty-controlled output where polarity and protection are already handled elsewhere.

.. code-block:: c

    mcpwm_gen_handle_t generator = NULL;
    mcpwm_generator_config_t gen_config = { .gen_gpio_num = 18 };
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &gen_config, &generator));

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        generator, MCPWM_GEN_TIMER_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY,
            MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)));

.. figure:: /../_static/mcpwm/single_edge_asym_active_high.svg
    :align: center
    :alt: Up-counting, active-high PWM: set high at zero and low at compare.

    Up-counting, active-high PWM: set high at zero and low at compare.

The generator configuration is small:

.. list::

    - :cpp:member:`gen_gpio_num <mcpwm_generator_config_t::gen_gpio_num>` — the GPIO that carries the PWM output. A second generator in the same operator, configured with the same actions, drives a second pin from the same time base.
    - :cpp:member:`invert_pwm <mcpwm_generator_config_t::flags::invert_pwm>` — inverts the PWM signal through the GPIO matrix. This is a hardware inversion of the final output, distinct from changing the actions; choose one of the two, not both.

Action configuration
====================

The configuration names say exactly what happens: :cpp:enumerator:`MCPWM_GEN_ACTION_HIGH <mcpwm_generator_action_t::MCPWM_GEN_ACTION_HIGH>`, :cpp:enumerator:`MCPWM_GEN_ACTION_LOW <mcpwm_generator_action_t::MCPWM_GEN_ACTION_LOW>`, or :cpp:enumerator:`MCPWM_GEN_ACTION_TOGGLE <mcpwm_generator_action_t::MCPWM_GEN_ACTION_TOGGLE>` at a particular event. The helper macros make the three important choices visible at the call site — direction, event source, and output level.

For an up-counting timer, :cpp:enumerator:`MCPWM_TIMER_EVENT_EMPTY <mcpwm_timer_event_t::MCPWM_TIMER_EVENT_EMPTY>` is the zero boundary and :cpp:enumerator:`MCPWM_TIMER_EVENT_FULL <mcpwm_timer_event_t::MCPWM_TIMER_EVENT_FULL>` fires at the timer peak. In up-counting mode the peak equals the period, so ``FULL`` lands on the period boundary; in up-down mode the peak is ``period_ticks / 2``, so ``FULL`` lands in the middle of the cycle. A compare action uses the comparator's threshold. The first example therefore means "start the cycle high; end the active part when the count reaches 15." A compare value outside the timer range never produces its event.

Every action must specify a timer direction, even though the choice only makes a visible difference in up-down mode. In up-counting mode the counter only runs upward, so the action configured for :cpp:enumerator:`MCPWM_TIMER_DIRECTION_UP <mcpwm_timer_direction_t::MCPWM_TIMER_DIRECTION_UP>` is the one that fires — you still have to write it explicitly. In up-down mode, configure actions for both :cpp:enumerator:`MCPWM_TIMER_DIRECTION_UP <mcpwm_timer_direction_t::MCPWM_TIMER_DIRECTION_UP>` and :cpp:enumerator:`MCPWM_TIMER_DIRECTION_DOWN <mcpwm_timer_direction_t::MCPWM_TIMER_DIRECTION_DOWN>` when both edges matter. This is what turns one comparator into a center-aligned PWM.

Classic Waveform Examples
=========================

The examples below build on the first PWM output from the previous section, reusing the same timer, operator, and comparator objects to create other common waveforms.

Invert the active polarity
--------------------------

Some gate drivers and LEDs are active low, such as a low-active gate-driver enable, an inverted LED path, or a board-level interface that is already inverted. Instead of adding GPIO inversion, set low at the period boundary and high at the comparator:

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        generator, MCPWM_GEN_TIMER_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_HIGH)));

.. figure:: /../_static/mcpwm/single_edge_asym_active_low.svg
    :align: center
    :alt: Up-counting, active-low PWM. Change the actions, rather than the wiring, when the output polarity is part of the design.

    Up-counting, active-low PWM. Change the actions, rather than the wiring, when the output polarity is part of the design.

Place a pulse inside the period
-------------------------------

When a short pulse must sit at a controlled position inside the cycle — an ADC sample window, a peripheral trigger pulse, or a latch strobe — two compare values choose its opening and closing edges:

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator_a, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator_b, MCPWM_GEN_ACTION_LOW)));

.. figure:: /../_static/mcpwm/pulse_placement_asym.svg
    :align: center
    :alt: Pulse placement: the distance between the two compare values is the pulse width.

    Pulse placement: the distance between the two compare values is the pulse width.

Set ``comparator_a`` below ``comparator_b``. Moving both by the same tick offset changes the position without changing width; moving only one changes width. Hardware places both edges, so this is more precise than a timer callback.

Two-edge asymmetric PWM
-----------------------

When several edges must be placed independently within one cycle and the active interval does not need to stay symmetric around the center — for example in certain asymmetric inverter modulation or custom gate-drive timing — use two generators and two comparators. Each generator has its own edge per cycle, so the high time splits across the period boundary:

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        gen_a, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, cmp_a, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        gen_a, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, cmp_b, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        gen_b, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, cmp_a, MCPWM_GEN_ACTION_LOW)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        gen_b, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, cmp_b, MCPWM_GEN_ACTION_HIGH)));

.. figure:: /../_static/mcpwm/dual_edge_asym_active_low.svg
    :align: center
    :alt: Dual-edge asymmetric (edge-aligned) PWM: two generators produce complementary outputs with two edges per cycle.

    Dual-edge asymmetric (edge-aligned) PWM: two generators produce complementary outputs with two edges per cycle.

Center-aligned PWM
------------------

Motor drives, inverters, and power stages that care about harmonic behavior often prefer center-aligned PWM because it gives more symmetric switching and lower harmonic distortion. Select ``MCPWM_TIMER_COUNT_MODE_UP_DOWN`` when creating the timer, then use the same threshold in both directions:

.. code-block:: c

    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator, MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_DOWN, comparator, MCPWM_GEN_ACTION_LOW)));

.. figure:: /../_static/mcpwm/dual_edge_sym_active_low.svg
    :align: center
    :alt: Center-aligned PWM: the up-count and down-count actions create matching edges around the period center.

    Center-aligned PWM: the up-count and down-count actions create matching edges around the period center.

The timer reaches its peak and returns to zero in each complete up-down cycle, so account for both legs when calculating frequency. Adding a second generator with opposite actions creates complementary *logical* outputs:

.. figure:: /../_static/mcpwm/dual_edge_sym_complementary.svg
    :align: center
    :alt: Complementary generator actions have no dead time by themselves; do not connect them directly to a power stage.

    Complementary generator actions have no dead time by themselves; do not connect them directly to a power stage.

.. warning::

    Logical complementary outputs are not yet safe half-bridge outputs. If the high-side and low-side devices have finite turn-off delay, add dead time and verify non-overlap at the actual gate pins.

Duty updates
============

Change duty by setting the comparator threshold, not the generator actions. The threshold is expressed in timer ticks: for an up-counting active-high waveform, ``compare_value / period_ticks`` is the duty ratio. Choose a timer resolution high enough that one tick gives the adjustment granularity the application needs.

.. code-block:: c

    mcpwm_comparator_set_compare_value(comparator, 25); // 50 %

Forced levels
=============

Adjusting the comparator threshold changes the normal duty. When you instead need to temporarily take over the output, ignore all event actions, and hold a fixed level, use the force-level API. :func:`mcpwm_generator_set_force_level` takes a ``level`` and a ``hold_on`` flag:

- ``level`` (second parameter) is the raw generator level to force: ``0`` or ``1`` overrides all event actions, while ``-1`` releases the force and returns control to event actions.
- ``hold_on`` (third parameter) decides how long the forced level lasts: ``true`` holds it until another call releases it, whereas ``false`` lets the next event action override it.

For example, ``mcpwm_generator_set_force_level(generator, 0, true)`` overrides all event actions and holds the raw generator low. Force acts before dead time and GPIO inversion, so confirm the physical pin level with a scope when using those features.

Force level is useful for power-up checks, a temporary post-fault safe output, or a mode-transition state. It is not a long-term replacement for a proper PWM configuration.

For a half bridge, add a second generator and configure the :ref:`dead-time module <mcpwm-dead-time>` to produce non-overlapping complementary outputs.

.. _mcpwm-dead-time:

Dead time and half-bridge drive
===============================

Dead time delays an output edge, leaving a short interval in which both switches in a half bridge are off. It compensates for transistor turn-off delay and helps prevent shoot-through. Configure and verify it before connecting a power stage.

A half bridge drives a load from a DC bus through a high-side and a low-side switch. Both switches are usually N-channel MOSFETs: the low-side source sits at ground and is easy to drive, while the high-side source swings with the output, so its gate needs a drive voltage above the bus voltage. The MCPWM outputs are 3.3 V logic and cannot drive the gates directly. For example, an IRS2101 uses a separate low-voltage driver supply (VCC, typically 10-20 V), with COM connected to power ground. Its bootstrap diode should be connected from VCC to VB, and the external bootstrap capacitor between VB and VS; VS must be connected to the OUT switch node. The high-side output is HO and the low-side output is LO, and both drive their MOSFET gates through gate resistors. VCC is not the high-voltage DC bus: the bootstrap diode charges the bootstrap capacitor from the regulated driver supply while the low-side switch is on. If the two switches were turned on and off simultaneously, the switch that is still turning off would overlap the one already turning on, shorting the bus to ground through both switches. Dead time inserts a both-off gap so the next switch turns on only after the previous one has fully turned off:

.. figure:: /../_static/mcpwm/half_bridge_dead_time.svg
    :align: center
    :alt: Half-bridge gate-driver circuit with bootstrap supply and dead-time comparison.

    Half-bridge gate-driver circuit with bootstrap supply and dead-time comparison.

Create complementary outputs
----------------------------

Create two generators in one operator. Feed generator A into its own output with a rising-edge delay, then feed it into generator B with a falling-edge delay and inversion.

.. note::

    Here, generator A is the first generator allocated from the operator handle, and generator B is the second.

.. code-block:: c

    mcpwm_dead_time_config_t dead_time = { .posedge_delay_ticks = 2 };
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gen_a, gen_a, &dead_time));
    dead_time = (mcpwm_dead_time_config_t) {
        .negedge_delay_ticks = 2,
        .flags.invert_output = true,
    };
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gen_a, gen_b, &dead_time));

.. figure:: /../_static/mcpwm/deadtime_active_high_complementary.svg
    :align: center
    :alt: Complementary PWM with a dead-time interval between the switch transitions.

    Complementary PWM with a dead-time interval between the switch transitions.

Understanding the routing and parameters
----------------------------------------

:func:`mcpwm_generator_set_dead_time(in_generator, out_generator, config) <mcpwm_generator_set_dead_time>` treats dead time as a small signal-processing stage. Passing the same generator for both handles changes that output in place. Passing ``gen_a`` as input and ``gen_b`` as output derives B from A, which is how the complementary example shares one PWM source.

:cpp:member:`posedge_delay_ticks <mcpwm_dead_time_config_t::posedge_delay_ticks>` delays a rising edge and :cpp:member:`negedge_delay_ticks <mcpwm_dead_time_config_t::negedge_delay_ticks>` delays a falling edge. Ticks use the connected timer's resolution, so a 2-tick setting at 10 MHz is 200 ns. The diagram below shows the basic effect: the rising edge of ``pwm_A`` is delayed (RED) and the falling edge of ``pwm_B`` is delayed (FED) relative to the original signal. Start with the maximum turn-off delay from the switch and gate-driver data sheets plus margin; then measure at the transistor gates and reduce it only after confirming that process, temperature, and layout still leave enough margin. Set both delays to zero to bypass the dead-time stage. :cpp:member:`invert_output <mcpwm_dead_time_config_t::flags::invert_output>` changes polarity after that stage.

.. figure:: /../_static/mcpwm/deadtime_active_high.svg
    :align: center
    :alt: Basic dead-time effect: rising edge delayed (RED) and falling edge delayed (FED) relative to the original.

    Basic dead-time effect: rising edge delayed (RED) and falling edge delayed (FED) relative to the original.

Resource limits per operator
----------------------------

The hardware has one rising-edge and one falling-edge delay resource per operator, so do not assign the same delay type independently to both generators. The following requests the one rising-edge resource twice and is invalid:

.. code-block:: c

    mcpwm_dead_time_config_t rise_delay = { .posedge_delay_ticks = 10 };
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gen_a, gen_a, &rise_delay));
    // This second independent rising-edge delay cannot be allocated.
    ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(gen_b, gen_b, &rise_delay));

You may assign the rising delay to A and the falling delay to B. You may also use both delay resources for B while A bypasses the module. If the first generator uses both delay resources, the other generator cannot use dead time.

More output patterns
--------------------

The complementary configuration above is the usual half-bridge starting point. Swap the output inversions to make both outputs active low while retaining the non-overlap:

.. figure:: /../_static/mcpwm/deadtime_active_low_complementary.svg
    :align: center
    :alt: Active-low complementary outputs. The timing resources are the same; only the post-dead-time polarity changes.

    Active-low complementary outputs. The timing resources are the same; only the post-dead-time polarity changes.

Dead time is also useful when only one channel needs an edge delay. Keep one output bypassed by passing a zero-delay configuration, and apply the available delay to the other:

.. figure:: /../_static/mcpwm/deadtime_reda_bypassb.svg
    :align: center
    :alt: Delay A's rising edge while B bypasses dead time. This is not a complementary half bridge; it is an independent edge-placement tool.

    Delay A's rising edge while B bypasses dead time. This is not a complementary half bridge; it is an independent edge-placement tool.

.. figure:: /../_static/mcpwm/deadtime_redb_fedb_bypassa.svg
    :align: center
    :alt: Bypass A and delay both edges of B, consuming both delay resources.

    Bypass A and delay both edges of B, consuming both delay resources.

A single-edge delay can also be applied individually. The next diagram shows the falling edge delayed on B while A is bypassed, using only the FED resource:

.. figure:: /../_static/mcpwm/deadtime_fedb_bypassa.svg
    :align: center
    :alt: Apply only the falling-edge delay to B, leaving A unchanged. This uses one delay resource.

    Apply only the falling-edge delay to B, leaving A unchanged. This uses one delay resource.

When the output is inverted, the dead-time behavior shifts accordingly. The following shows the active-low version of the basic delay, where the invert flag flips the polarity of both outputs:

.. figure:: /../_static/mcpwm/deadtime_active_low.svg
    :align: center
    :alt: Active-low dead time: same delay resources, but the output polarity is inverted after the delay stage.

    Active-low dead time: same delay resources, but the output polarity is inverted after the delay stage.

.. note::

    For a waveform where each edge must have an independently movable position, use two comparators and generator actions instead. The dead-time module is the better choice when the requirement is specifically a delayed edge plus polarity control.

Update at a safe boundary
-------------------------

Set the operator's :cpp:member:`update_dead_time_on_tez <mcpwm_operator_config_t::flags::update_dead_time_on_tez>`, :cpp:member:`update_dead_time_on_tep <mcpwm_operator_config_t::flags::update_dead_time_on_tep>`, or :cpp:member:`update_dead_time_on_sync <mcpwm_operator_config_t::flags::update_dead_time_on_sync>` flag when a changed dead-time value must take effect only at a known boundary.

.. note::

    Probe both physical gate pins: GPIO inversion, carrier modulation, and gate-driver polarity can all alter what appears at the transistor. When several stages invert the signal, two inversions can cancel out and look correct in software while the hardware does something unexpected, so always verify against the real waveform.

Other event sources
===================

Generator actions can also react directly to GPIO fault events or a sync event:

.. list::

    - :cpp:func:`mcpwm_generator_set_action_on_fault_event()` — immediate hardware reaction to a GPIO fault. Uses limited operator trigger slots.
    - :cpp:func:`mcpwm_generator_set_action_on_sync_event()` — transition at a synchronization edge. Each generator has one sync-action slot.
    - :cpp:func:`mcpwm_generator_set_action_on_brake_event()` — per-generator output state during an operator :ref:`brake <mcpwm-brake>`. It is set per brake mode and timer direction; see :ref:`Fault connection <mcpwm-brake-fault-connection>` for a full example.

For safety policy and persistent braking, prefer the operator :ref:`brake mechanism <mcpwm-brake>`. A generator fault action is best for a local edge-level response; a brake defines the safe state and recovery behavior for the whole output stage.

API Reference
=============

MCPWM Generator Driver Functions
---------------------------------

.. include-build-file:: inc/mcpwm_gen.inc
