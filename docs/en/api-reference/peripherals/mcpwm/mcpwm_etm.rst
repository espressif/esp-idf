===============================================
MCPWM ETM: Hardware Linking Between Peripherals
===============================================

.. contents::
    :local:
    :depth: 2

.. only:: SOC_MCPWM_SUPPORT_ETM

    The Event Task Matrix (ETM) can route an MCPWM timer or comparator event directly to an ETM task, avoiding ISR latency. Use it when another peripheral must respond at an exact PWM phase.

    Create an ETM event from the timer or comparator, create a compatible task from the destination peripheral, then connect both with an ETM channel. The destination driver's documentation defines its task and the complete channel setup. For the general ETM workflow — allocating a channel and connecting an event to a task — see the :doc:`ETM </api-reference/peripherals/etm>` documentation.

    .. mermaid::

        flowchart LR
            T["MCPWM Timer<br/>TEZ/TEP event"]:::source --> E["ETM Channel"]:::route
            C["MCPWM Comparator<br/>compare event"]:::source --> E
            E --> D["Destination<br/>peripheral task"]:::dest
            classDef source fill:#dbeafe,stroke:#2563eb,color:#172554
            classDef route fill:#ede9fe,stroke:#7c3aed,color:#2e1065
            classDef dest fill:#dcfce7,stroke:#16a34a,color:#14532d

    A timer emits a `TEZ` (timer reaches zero) or `TEP` (timer reaches peak) event. To get one:

    .. code-block:: c

        esp_etm_event_handle_t timer_event = NULL;
        ESP_ERROR_CHECK(mcpwm_timer_new_etm_event(timer,
            &(mcpwm_timer_etm_event_config_t) {
                .event_type = MCPWM_TIMER_ETM_EVENT_TEZ,
            }, &timer_event));
        // Create a destination ETM task, allocate a channel, then connect:
        // esp_etm_channel_connect(channel, timer_event, destination_task);

    A comparator provides an `EQUAL` event, firing each time the timer count equals the comparator value. This pins the event to an arbitrary phase of the PWM period rather than just the crest or trough. To get one:

    .. code-block:: c

        esp_etm_event_handle_t cmp_event = NULL;
        ESP_ERROR_CHECK(mcpwm_comparator_new_etm_event(cmp,
            &(mcpwm_cmpr_etm_event_config_t) {
                .event_type = MCPWM_CMPR_ETM_EVENT_EQUAL,
            }, &cmp_event));
        // esp_etm_channel_connect(channel, cmp_event, destination_task);

    Release the event with :cpp:func:`esp_etm_del_event()` when no longer needed.

    A very common use of a comparator event is to trigger an ADC sampling: place the comparator at the phase you want to sample, then have the comparator event start an ADC so the converter samples a clean, settled waveform exactly in phase with the PWM cycle. Because the whole link is done in hardware, the ADC sample point tracks the PWM with no CPU and no ISR latency.

    .. only:: SOC_MCPWM_SUPPORT_EVENT_COMPARATOR

        Which comparator feeds the event matters. The **operator comparator** (:cpp:func:`mcpwm_new_comparator()`) also drives the generators, so its compare value defines an actual PWM output edge, and its ETM event is limited to that edge — you cannot ask it for a phase that is not one of the edges it produces. The **event comparator** (:cpp:func:`mcpwm_new_event_comparator()`) is a dedicated ETM timing marker: it drives no generator, consumes none of the operator-comparator slots, and therefore can fire at *any* phase inside the PWM period, with no effect on the PWM waveform. That freedom is exactly what an ADC trigger needs, so the event comparator is the recommended source — pick a sample point where the voltage has settled, not just where an edge happens to be.

    API Reference
    =============

    MCPWM ETM Driver Functions
    --------------------------

    .. include-build-file:: inc/mcpwm_etm.inc

.. only:: not SOC_MCPWM_SUPPORT_ETM

    {IDF_TARGET_NAME} does not support MCPWM ETM events.
