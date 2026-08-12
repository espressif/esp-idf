============================================
Motor Control Pulse Width Modulator (MCPWM)
============================================

:link_to_translation:`zh_CN:[中文]`

Start Here
==========

MCPWM turns a counter into accurately timed output edges. It is a good fit when an LEDC-style PWM is no longer enough: motor bridges need complementary outputs and dead time, inverters need synchronized phases, and sensors need pulse-width measurement.

The smallest useful MCPWM design has four objects: a :doc:`timer <mcpwm_timer>` supplies time, an :doc:`operator <mcpwm_operator>` owns the waveform resources, a :doc:`comparator <mcpwm_cmpr>` chooses an edge position, and a :doc:`generator <mcpwm_gen>` drives a GPIO. The other modules extend that design without changing its foundation.

Build a PWM output
==================

For a first PWM output, create objects from left to right in the following flow. Each stage of the main path is color-coded by role: time base (blue), operator core (purple), waveform setup (cyan), start and output (green). Amber nodes are optional additions; the red node is the safety brake. Start the timer only after all output actions are configured.

.. mermaid::

    flowchart LR
        T1["1. Create timer<br/>mcpwm_new_timer"]:::time
        O1["2. Create operator<br/>mcpwm_new_operator"]:::core
        LINK["3. Connect time base<br/>mcpwm_operator_connect_timer"]:::core
        C1["4. Create comparator<br/>mcpwm_new_comparator"]:::wave
        G1["5. Create generator<br/>mcpwm_new_generator"]:::wave
        A1["6. Describe edges<br/>mcpwm_generator_set_action_on_*_event"]:::wave
        RUN["7. Enable and start<br/>mcpwm_timer_enable<br/>mcpwm_timer_start_stop"]:::run
        PIN["PWM on GPIO"]:::output

        T1 --> O1 --> LINK --> C1 --> G1 --> A1 --> RUN --> PIN

        DT["Dead time<br/>mcpwm_generator_set_dead_time"]:::optional
        BR["Fault and brake<br/>mcpwm_new_*_fault<br/>mcpwm_operator_set_brake_on_fault"]:::safety
        SY["Phase synchronization<br/>mcpwm_new_*_sync_src<br/>mcpwm_timer_set_phase_on_sync"]:::optional
        CA["Carrier modulation<br/>mcpwm_operator_apply_carrier"]:::optional

        A1 -. extend .-> DT
        O1 -. protect .-> BR
        T1 -. align .-> SY
        O1 -. modulate .-> CA

        classDef time fill:#dbeafe,stroke:#2563eb,color:#172554
        classDef core fill:#ede9fe,stroke:#7c3aed,color:#2e1065
        classDef wave fill:#cffafe,stroke:#0891b2,color:#164e63
        classDef run fill:#dcfce7,stroke:#16a34a,color:#14532d
        classDef output fill:#bbf7d0,stroke:#15803d,color:#14532d
        classDef optional fill:#fef3c7,stroke:#d97706,color:#78350f
        classDef safety fill:#fee2e2,stroke:#dc2626,color:#7f1d1d

The following code creates one 20 kHz PWM output with a 30% duty cycle. It is meant to be read before the individual pages so a first-time user can see the whole object chain in one place. It also shows the most common runtime adjustment: changing the comparator rather than rebuilding the waveform.

.. code-block:: c

    mcpwm_timer_handle_t timer = NULL;
    mcpwm_oper_handle_t oper = NULL;
    mcpwm_cmpr_handle_t comparator = NULL;
    mcpwm_gen_handle_t generator = NULL;

    // 1 MHz → 1 tick = 1 µs
    // 50 ticks → 50 µs period → 20 kHz
    ESP_ERROR_CHECK(mcpwm_new_timer(
        &(mcpwm_timer_config_t) {
            .group_id = 0,
            .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
            .resolution_hz = 1000000,
            .period_ticks = 50,
            .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        },
        &timer));

    ESP_ERROR_CHECK(mcpwm_new_operator(
        &(mcpwm_operator_config_t) {
            .group_id = 0,
        },
        &oper));
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    ESP_ERROR_CHECK(mcpwm_new_comparator(
        oper,
        &(mcpwm_comparator_config_t) {
            .flags.update_cmp_on_tez = true,
        },
        &comparator));
    // 15 / 50 = 30% duty cycle
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 15));

    ESP_ERROR_CHECK(mcpwm_new_generator(
        oper,
        &(mcpwm_generator_config_t) {
            .gen_gpio_num = 18,
        },
        &generator));

    // timer empty → output HIGH; compare match → output LOW
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(
        generator,
        MCPWM_GEN_TIMER_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP,
            MCPWM_TIMER_EVENT_EMPTY,
            MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
        generator,
        MCPWM_GEN_COMPARE_EVENT_ACTION(
            MCPWM_TIMER_DIRECTION_UP,
            comparator,
            MCPWM_GEN_ACTION_LOW)));

    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    // Change duty at run time by moving the edge.
    // 25 / 50 = 50% duty
    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, 25));

The timer's ``resolution_hz`` and ``period_ticks`` set the timing scale. The comparator's ``compare_value`` chooses an edge in that scale, and the generator action APIs decide the output level at the timer boundary or comparator crossing. This division is useful when tuning: change the timer for frequency, the comparator for duty or edge position, and generator actions for polarity or waveform shape.

After the waveform is configured, call :cpp:func:`mcpwm_timer_enable()` and :cpp:func:`mcpwm_timer_start_stop()`. At run time, update the comparator with :cpp:func:`mcpwm_comparator_set_compare_value()` rather than rebuilding the generator actions. Use the relevant optional branch only when the application needs it: dead time for a half bridge, fault and brake for a safety path, sync for phase alignment, and carrier for isolated drive.

Feature map
===========

.. list-table::
    :header-rows: 1
    :widths: 18 38 34 24

    * - Goal
      - Read first
      - Key APIs
      - Typical use
    * - One PWM output
      - :doc:`timer <mcpwm_timer>` -> :doc:`operator <mcpwm_operator>` -> :doc:`comparator <mcpwm_cmpr>`

        :doc:`generator <mcpwm_gen>`
      - :cpp:func:`mcpwm_new_timer`

        :cpp:func:`mcpwm_new_comparator`

        ``mcpwm_generator_set_action_on_*_event``
      - Servo, dimming, basic control
    * - Complementary half-bridge PWM
      - dead-time section in :doc:`generator <mcpwm_gen>` + :doc:`fault <mcpwm_fault>`
      - :cpp:func:`mcpwm_generator_set_dead_time`

        :cpp:func:`mcpwm_operator_set_brake_on_fault`
      - Half bridge, inverter leg
    * - Aligned or phase-shifted outputs
      - :doc:`sync <mcpwm_sync>`
      - :cpp:func:`mcpwm_timer_set_phase_on_sync`

        :cpp:func:`mcpwm_new_timer_sync_src`
      - Multi-phase motor, paralleled converters
    * - Measure pulse width or period
      - :doc:`capture <mcpwm_cap>`
      - :cpp:func:`mcpwm_new_capture_timer`

        :cpp:func:`mcpwm_capture_channel_register_event_callbacks`
      - HC-SR04, tachometer, RC input
    * - Hardware peripheral linking
      - :doc:`ETM <mcpwm_etm>`
      - :cpp:func:`mcpwm_timer_new_etm_event`

        :cpp:func:`mcpwm_new_event_comparator`
      - ADC trigger, timing chains

Each page in this guide covers one MCPWM module:

.. toctree::
    :maxdepth: 1

    mcpwm_timer
    mcpwm_operator
    mcpwm_cmpr
    mcpwm_gen
    mcpwm_fault
    mcpwm_sync
    mcpwm_cap
    mcpwm_etm
    mcpwm_advanced

Resource and lifetime rules
===========================

All objects belong to an MCPWM group. A timer and the operator connected to it must be in the same group; GPIO fault and GPIO sync sources can likewise be consumed only inside their group. Hardware resources are limited, so creation can return :c:macro:`ESP_ERR_NOT_FOUND`.

Every object is created by a ``mcpwm_new_*()`` factory that returns an opaque handle, and released with the matching ``mcpwm_del_*()`` function — for example :cpp:func:`mcpwm_new_timer()` and :cpp:func:`mcpwm_del_timer()`. Create parent objects before their children and destroy them in reverse order: generators/comparators first, then their operator, then the timer. A timer must be disabled before it can be deleted. Capture channels must be deleted before their capture timer.

The group clock divider is shared by timers and, on some targets, capture timers. Allocate objects in monotonic requested-resolution order (high-to-low or low-to-high) to avoid a divider conflict. See :doc:`advanced topics <mcpwm_advanced>` for the exact resolution rules.

Glossary
========

.. list::

    - **TEZ:** Timer equals zero, when the timer count reaches zero.
    - **TEP:** Timer equals peak, when the timer count reaches its peak.
    - **Timer:** The time base that defines PWM frequency and tick granularity.
    - **Operator:** The container between the timer and the outputs; it manages comparators, generators, brake, dead time, and carrier.
    - **Comparator:** Emits an event when the count reaches a threshold; often used to place an edge or define duty.
    - **Generator:** Drives the GPIO level in response to timer, comparator, fault, or sync events.
    - **Dead time:** A non-overlap interval between half-bridge transitions to avoid shoot-through.
    - **Fault:** An abnormal condition source, from GPIO or software.
    - **Brake:** The output safety policy applied after a fault.
    - **CBC:** Cycle-by-cycle braking that recovers automatically at a cycle boundary after the fault clears.
    - **OST:** One-shot braking that stays latched until software recovers it.
    - **Sync:** Loading a timer to a chosen count and direction on a sync edge.
    - **Capture:** Timestamping external input edges to measure pulse width, period, or speed.

Application examples
====================

.. list::

    - :example:`peripherals/mcpwm/mcpwm_servo_control` — one PWM output for an RC servo.
    - :example:`peripherals/mcpwm/mcpwm_bdc_speed_control` — brushed DC motor and speed feedback.
    - :example:`peripherals/mcpwm/mcpwm_bldc_hall_control` — BLDC commutation using Hall-sensor feedback.
    - :example:`peripherals/mcpwm/mcpwm_capture_hc_sr04` — pulse width measurement with an HC-SR04.
    - :example:`peripherals/mcpwm/mcpwm_sync` — GPIO, timer, and software synchronization.
    - :example:`peripherals/mcpwm/mcpwm_foc_svpwm_open_loop` — three complementary PWM pairs for open-loop FOC.

API Reference
=============

Common types
------------

.. include-build-file:: inc/components/esp_driver_mcpwm/include/driver/mcpwm_types.inc
.. include-build-file:: inc/components/esp_hal_mcpwm/include/hal/mcpwm_types.inc
