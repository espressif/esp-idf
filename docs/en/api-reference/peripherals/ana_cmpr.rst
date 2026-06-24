=================
Analog Comparator
=================

:link_to_translation:`zh_CN:[中文]`

.. contents::
    :local:
    :depth: 2

Overview
========

An analog comparator tells you whether a source signal is above or below a reference signal. The reference can come from either an internal generator or an external input.

This makes the analog comparator useful for tasks such as:

- Detecting whether a waveform crosses a fixed threshold
- Turning a sine wave into a digital square wave around a chosen reference level
- Comparing one analog signal against another analog signal
- Generating interrupts or ETM events when the comparison result changes
- Building simple hardware signal-processing pipelines without continuous CPU involvement

.. note::

    The analog comparator peripheral is not a continuous-time comparator in the same sense as a discrete analog comparator IC or an op-amp based analog front end. It is driven by a source clock, so comparison results are sampled in hardware time. In practice, clock source, scan timing, debounce, and ETM configuration all affect what you observe on real signals.

Quick Start
===========

If you are new to this peripheral, start with the simplest workflow:

1. Create a comparator unit
2. Select the source signal and reference source
3. Configure the internal reference or external reference input
4. Optionally configure debounce
5. Register a callback or create ETM connections
6. Enable the comparator

The following example uses an internal reference and an interrupt callback to detect when a source signal crosses 50% VDD:

.. code:: c

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,                            // Pick a comparator unit that is available on your target
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,  // Most applications can use the default clock source
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL, // Use the internal reference for a simple fixed threshold
        .cross_type = ANA_CMPR_CROSS_ANY,     // Trigger on both positive and negative crossings
        .src_chan0_gpio = 0,                  // Replace with the analog comparator source GPIO wired on your board
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD, // Set the internal reference voltage to 50% of VDD, i.e., compare whether the source signal is above or below half of the supply voltage
    };
    ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10, // Set a debounce time of 10 microseconds to suppress duplicate interrupts from noisy signals around the threshold
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback, // Register a callback to be called when a crossing event occurs; this callback should be defined elsewhere in your code
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));

This flow introduces the most important ideas:

- :cpp:func:`ana_cmpr_new_unit` creates a comparator instance and selects the source GPIO and reference source.
- :cpp:func:`ana_cmpr_set_internal_reference` chooses the threshold when using the internal reference.
- :cpp:func:`ana_cmpr_set_debounce` can suppress duplicate crossing interrupts caused by noisy or slow-moving signals.
- :cpp:func:`ana_cmpr_register_event_callbacks` lets your code react when the comparison result changes.
- :cpp:func:`ana_cmpr_enable` starts the hardware after configuration is complete.

The rest of this document expands this flow into typical usage scenarios.

Lifecycle and Valid API States
------------------------------

The following diagram shows the analog comparator lifecycle and the states in which each API is intended to be used:

.. mermaid::

    flowchart TD
        NC([Not created]) -->|ana_cmpr_new_unit| INIT[Init / Disabled]
        INIT -->|ana_cmpr_enable| EN[Enabled]
        EN -->|ana_cmpr_disable| INIT
        INIT -->|ana_cmpr_del_unit| NC

        subgraph INIT_APIS [Init-state APIs]
            SCAN[ana_cmpr_set_scan_config]
            ADD[ana_cmpr_add_src_chan<br/>ana_cmpr_remove_src_chan]
            CB[ana_cmpr_register_event_callbacks]
        end

        subgraph BOTH_APIS [APIs usable in both states]
            IR[ana_cmpr_set_internal_reference]
            DBC[ana_cmpr_set_debounce]
        end

        subgraph EN_APIS [Enabled-only API]
            TRIG[ana_cmpr_trigger_scan]
        end

        INIT -. can call .-> SCAN
        INIT -. can call .-> ADD
        INIT -. can call .-> CB
        INIT -. can call .-> IR
        INIT -. can call .-> DBC
        EN -. can call .-> IR
        EN -. can call .-> DBC
        EN -. can call .-> TRIG

        classDef stateNeutral fill:#f8fafc,stroke:#64748b,stroke-width:1.5px,color:#111827;
        classDef stateInit fill:#eff6ff,stroke:#2563eb,stroke-width:1.5px,color:#111827;
        classDef stateEnabled fill:#fdf2f8,stroke:#db2777,stroke-width:1.5px,color:#111827;
        classDef initApi fill:#eff6ff,stroke:#60a5fa,color:#111827;
        classDef bothApi fill:#ecfdf5,stroke:#10b981,color:#111827;
        classDef enabledApi fill:#fdf2f8,stroke:#f472b6,color:#111827;

        class NC stateNeutral;
        class INIT stateInit;
        class EN stateEnabled;
        class SCAN,ADD,CB initApi;
        class IR,DBC bothApi;
        class TRIG enabledApi;

The main constraints to remember are:

- :cpp:func:`ana_cmpr_register_event_callbacks`, :cpp:func:`ana_cmpr_add_src_chan`, :cpp:func:`ana_cmpr_remove_src_chan`, and :cpp:func:`ana_cmpr_set_scan_config` are intended to be used while the comparator is still in init state.
- :cpp:func:`ana_cmpr_trigger_scan` only works after :cpp:func:`ana_cmpr_enable`.
- :cpp:func:`ana_cmpr_del_unit` requires the comparator to be disabled first.

Scenario 1: Detect When a Signal Crosses a Fixed Threshold
==========================================================

Use this setup when you have one analog input and you only care whether it is above or below a threshold.

Typical examples:

- Detect whether a sensor output rises above a limit
- Convert a sine wave into a digital waveform around a fixed threshold
- Trigger software when a signal changes polarity around mid-supply

The easiest way to build this is to compare the source signal against the internal reference.

Example: Internal Reference with Callback
-----------------------------------------

.. code:: c

    static bool IRAM_ATTR example_ana_cmpr_on_cross_callback(ana_cmpr_handle_t cmpr,
                                                             const ana_cmpr_cross_event_data_t *edata,
                                                             void *user_ctx)
    {
        if (edata->cross_type == ANA_CMPR_CROSS_POS) {
            gpio_set_level(EXAMPLE_MONITOR_GPIO_NUM, 1);
        } else if (edata->cross_type == ANA_CMPR_CROSS_NEG) {
            gpio_set_level(EXAMPLE_MONITOR_GPIO_NUM, 0);
        }
        return false;
    }

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,                            // Use a valid unit index for the selected target
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,  // Start with default unless you need specific timing
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL, // Compare source against internal threshold
        .cross_type = ANA_CMPR_CROSS_ANY,     // Report both positive and negative crossings
        .src_chan0_gpio = 0,                  // Replace with the source channel GPIO connected on your board
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_internal_ref_config_t ref_cfg = {
        .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback,
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));

How to Read the Configuration
-----------------------------

The :cpp:type:`ana_cmpr_config_t` structure describes the basic shape of the comparator instance:

- :cpp:member:`ana_cmpr_config_t::unit` selects which comparator unit to use.
- :cpp:member:`ana_cmpr_config_t::clk_src` selects the comparator source clock. For most applications, :cpp:enumerator:`ANA_CMPR_CLK_SRC_DEFAULT` is the right starting point.
- :cpp:member:`ana_cmpr_config_t::ref_src` selects whether the reference comes from the internal threshold generator or an external analog input.
- :cpp:member:`ana_cmpr_config_t::cross_type` selects which crossings should trigger the interrupt callback: positive, negative, or both.
- :cpp:member:`ana_cmpr_config_t::src_chan0_gpio` selects the source GPIO on targets whose analog comparator pads are configurable.
- :cpp:member:`ana_cmpr_config_t::resample_limit` sets the unit-wide number of consecutive consistent samples required before a channel state update is accepted.

When using the internal reference, :cpp:func:`ana_cmpr_set_internal_reference` sets the threshold with :cpp:member:`ana_cmpr_internal_ref_config_t::ref_volt`. The available values are fixed percentages of VDD. For example, :cpp:enumerator:`ANA_CMPR_REF_VOLT_50_PCT_VDD` means the comparator reports whether the source signal is above or below half of the supply voltage.

Debounce and Signal Stability
-----------------------------

Real analog signals are often noisy near the threshold. Without debounce, a signal that slowly moves across the threshold or jitters around it may trigger several crossing interrupts in a short time.

Use :cpp:func:`ana_cmpr_set_debounce` to suppress these duplicate crossings:

- :cpp:member:`ana_cmpr_debounce_config_t::wait_us` is the time window during which new crossing interrupts are temporarily ignored after one crossing has already been reported.

As a rule of thumb:

- Increase ``wait_us`` if your signal is noisy and you see too many interrupts.
- Decrease ``wait_us`` if your signal changes quickly and you start missing real crossings.

Callback Behavior
-----------------

Register callbacks with :cpp:func:`ana_cmpr_register_event_callbacks` before enabling the unit.

The comparator currently provides one callback group member:

- :cpp:member:`ana_cmpr_event_callbacks_t::on_cross`

The callback receives :cpp:type:`ana_cmpr_cross_event_data_t`, which tells you:

- :cpp:member:`ana_cmpr_cross_event_data_t::cross_type` - whether the source crossed upward or downward
- :cpp:member:`ana_cmpr_cross_event_data_t::src_chan_id` - which source channel triggered the event on targets that support multiple source channels

On targets that do not support independent positive and negative interrupt reporting, :cpp:member:`ana_cmpr_cross_event_data_t::cross_type` can always be reported as :cpp:enumerator:`ANA_CMPR_CROSS_ANY`.

The callback runs in interrupt context, so keep it short and non-blocking.

.. note::

    If :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE` is enabled, place the callback and any accessed data in internal RAM.

Scenario 2: Compare a Signal Against an External Reference
==========================================================

Use this setup when the reference level is not a fixed percentage of VDD.

Typical examples:

- Compare a sensor signal against a voltage divider output
- Compare a waveform against another waveform
- Generate a digital result from a source/reference pair that both come from analog front-end circuitry

In this case, the comparator source is still your input signal, but the reference comes from another analog input.

Example: External Reference Input
---------------------------------

.. code:: c

    ana_cmpr_handle_t cmpr = NULL;
    ana_cmpr_config_t config = {
        .unit = 0,                             // Use a valid unit index for the selected target
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,   // Default clock is usually enough
        .ref_src = ANA_CMPR_REF_SRC_EXTERNAL,  // Select external reference mode
        .cross_type = ANA_CMPR_CROSS_ANY,      // Report both crossing directions
        .src_chan0_gpio = 0,                   // Source signal GPIO from your board design
        .ext_ref_gpio = 1,                     // Reference signal GPIO from your board design
    };
    ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));

    ana_cmpr_debounce_config_t dbc_cfg = {
        .wait_us = 10,
    };
    ESP_ERROR_CHECK(ana_cmpr_set_debounce(cmpr, &dbc_cfg));

    ana_cmpr_event_callbacks_t cbs = {
        .on_cross = example_ana_cmpr_on_cross_callback,
    };
    ESP_ERROR_CHECK(ana_cmpr_register_event_callbacks(cmpr, &cbs, NULL));

    ESP_ERROR_CHECK(ana_cmpr_enable(cmpr));

What Changes Compared with Scenario 1
-------------------------------------

The main difference is :cpp:member:`ana_cmpr_config_t::ref_src`:

- :cpp:enumerator:`ANA_CMPR_REF_SRC_INTERNAL` means the reference is generated inside the comparator block.
- :cpp:enumerator:`ANA_CMPR_REF_SRC_EXTERNAL` means the reference comes from a dedicated analog input.

When using an external reference:

- :cpp:member:`ana_cmpr_config_t::ext_ref_gpio` becomes meaningful on targets with configurable analog comparator pads.
- :cpp:func:`ana_cmpr_set_internal_reference` is not used.

You can query the actual GPIOs in use with :cpp:func:`ana_cmpr_get_channel_gpio`:

.. code:: c

    gpio_num_t src_gpio = -1;
    gpio_num_t ext_ref_gpio = -1;
    // Read back actual channel mapping after initialization.
    ESP_ERROR_CHECK(ana_cmpr_get_channel_gpio(cmpr, ANA_CMPR_SOURCE_CHAN, 0, &src_gpio));
    ESP_ERROR_CHECK(ana_cmpr_get_channel_gpio(cmpr, ANA_CMPR_EXT_REF_CHAN, 0, &ext_ref_gpio));

.. only:: SOC_ANA_CMPR_SUPPORT_ETM_SCAN

    Scenario 3: Build a Low-CPU Hardware Signal Path with ETM
    =========================================================

    Use this setup when you want the hardware to react to crossings directly, without routing every event through the CPU.

    Typical examples:

    - Turn a sampled analog input into a GPIO waveform with very low CPU load
    - Use a timer to schedule comparator scans periodically
    - Connect comparator crossings to other peripherals through ETM

    Example: GPTimer Periodic Scan with ETM
    ---------------------------------------

    :example:`peripherals/analog_comparator/etm_periodic_scan` example builds the following hardware chain:

    - GPTimer alarm event -> GPTimer enable-alarm task
    - GPTimer alarm event -> Analog comparator start task
    - Analog comparator positive cross event -> GPIO set task
    - Analog comparator negative cross event -> GPIO clear task

    In other words, the timer schedules scans, the comparator samples the source signal, and the crossing result drives a monitor GPIO directly through ETM.

    The key comparator-side configuration looks like this:

    .. code:: c

        ana_cmpr_handle_t cmpr = NULL;
        ana_cmpr_config_t config = {
            .unit = 0,                             // Use a valid unit index for your target
            .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,   // Keep default unless ETM timing requires otherwise
            .ref_src = ANA_CMPR_REF_SRC_INTERNAL,  // Use internal reference for threshold
            .cross_type = ANA_CMPR_CROSS_ANY,      // Keep both edge directions for ETM output waveform
            .src_chan0_gpio = 0,                   // Set according to your board's source signal wiring
            .resample_limit = 3,                   // Improve noise immunity before reporting channel state
        };
        ESP_ERROR_CHECK(ana_cmpr_new_unit(&config, &cmpr));

        ana_cmpr_internal_ref_config_t ref_cfg = {
            .ref_volt = ANA_CMPR_REF_VOLT_50_PCT_VDD,
        };
        ESP_ERROR_CHECK(ana_cmpr_set_internal_reference(cmpr, &ref_cfg));

        ana_cmpr_scan_config_t scan_cfg = {
            .scan_mode = ANA_CMPR_SCAN_MODE_FULL,  // Scan all configured channels each cycle
            .poll_period_us = 2,                   // Tune this for update rate vs. timing margin
        };
        ESP_ERROR_CHECK(ana_cmpr_set_scan_config(cmpr, &scan_cfg));

        ana_cmpr_etm_task_config_t cmpr_task_cfg = {
            .task_type = ANA_CMPR_TASK_START,
        };
        ESP_ERROR_CHECK(ana_cmpr_new_etm_task(cmpr, &cmpr_task_cfg, &cmpr_start_task));

        ana_cmpr_etm_event_config_t pos_evt_cfg = {
            .event_type = ANA_CMPR_EVENT_POS_CROSS, // Rising crossing event
            .src_chan_id = 0,                       // Monitor source channel 0 in this example
        };
        ESP_ERROR_CHECK(ana_cmpr_new_etm_event(cmpr, &pos_evt_cfg, &cmpr_pos_evt));

        ana_cmpr_etm_event_config_t neg_evt_cfg = {
            .event_type = ANA_CMPR_EVENT_NEG_CROSS, // Falling crossing event
            .src_chan_id = 0,                       // Same source channel as positive crossing
        };
        ESP_ERROR_CHECK(ana_cmpr_new_etm_event(cmpr, &neg_evt_cfg, &cmpr_neg_evt));

    Understanding Scan Configuration
    --------------------------------

    When the target supports source-channel scan, :cpp:func:`ana_cmpr_set_scan_config` controls how the comparator walks through source channels:

    - :cpp:member:`ana_cmpr_scan_config_t::scan_mode` selects the scanning behavior.
    - :cpp:member:`ana_cmpr_scan_config_t::poll_period_us` sets the wait time between channel comparisons.

    These fields matter most when:

    - you use multiple source channels
    - the source signal is noisy
    - you want periodic sampling instead of interrupting on every hardware edge

    In practice:

    - Shorter ``poll_period_us`` means faster scan response but tighter timing margins.

    ETM Event and Task Handles
    --------------------------

    The analog comparator provides ETM handles through two APIs:

    - :cpp:func:`ana_cmpr_new_etm_event` creates an event handle from a comparator crossing.
    - :cpp:func:`ana_cmpr_new_etm_task` creates a task handle that can be triggered by another ETM event.

    In the current analog comparator driver:

    - ETM event type is selected by :cpp:member:`ana_cmpr_etm_event_config_t::event_type`
    - The source channel is selected by :cpp:member:`ana_cmpr_etm_event_config_t::src_chan_id`
    - Optional event delay is configured by :cpp:member:`ana_cmpr_etm_event_config_t::event_delay`
    - Task type is selected by :cpp:member:`ana_cmpr_etm_task_config_t::task_type`

    For how to connect the comparator events and tasks to ETM channels, refer to :doc:`ETM </api-reference/peripherals/etm>`.

Working with Multiple Source Channels
=====================================

Some targets support more than one source channel per comparator unit. If your application needs to sample several analog inputs against the same reference, you can configure extra source channels while the comparator is in init state.

The relevant APIs are:

- :cpp:func:`ana_cmpr_add_src_chan`
- :cpp:func:`ana_cmpr_remove_src_chan`
- :cpp:func:`ana_cmpr_set_src_chan_cross_type`

Example:

.. code:: c

    ana_cmpr_src_chan_config_t src_cfg = {
        .gpio_num = EXAMPLE_SRC_CHAN1_GPIO,
        .cross_type = ANA_CMPR_CROSS_ANY,
    };
    ESP_ERROR_CHECK(ana_cmpr_add_src_chan(cmpr, 1, &src_cfg));

When using multiple source channels:

- Add or remove channels only before :cpp:func:`ana_cmpr_enable`.
- If you remove every configured source channel, :cpp:func:`ana_cmpr_enable` fails with ``ESP_ERR_INVALID_STATE``.
- :cpp:func:`ana_cmpr_set_src_chan_cross_type` is only runtime-configurable on ESP32-H2.

Runtime Observation and Timing Helpers
======================================

The following APIs are general runtime helpers and are not tied to multi-source configuration:

- :cpp:func:`ana_cmpr_trigger_scan` forces one software-triggered scan sequence (on targets that support software-triggered scan).
- :cpp:func:`ana_cmpr_get_output_level` reads the latest sampled comparator output of a source channel (on targets that support output-level reporting).
- :cpp:func:`ana_cmpr_get_clock_resolution_hz` returns PAD_COMP_CLK resolution in Hz.
- :cpp:func:`ana_cmpr_get_capture_timestamps` reads current/previous crossing timestamps (on capture-capable targets).

Use :cpp:func:`ana_cmpr_get_output_level` when you need current digital comparison state from software. Use :cpp:func:`ana_cmpr_get_clock_resolution_hz` when converting PAD_COMP_CLK ticks (for example, capture timestamps) to time units. Use :cpp:func:`ana_cmpr_trigger_scan` when you want deterministic software-controlled refresh timing; it requires :cpp:func:`ana_cmpr_enable` and returns ``ESP_ERR_INVALID_STATE`` in init state.

To get meaningful capture timestamps, enable the capture timer when creating the unit:

.. code:: c

    ana_cmpr_config_t config = {
        .unit = 0,                            // Use a valid unit index for your target
        .clk_src = ANA_CMPR_CLK_SRC_DEFAULT,  // PAD_COMP_CLK source
        .ref_src = ANA_CMPR_REF_SRC_INTERNAL, // Reference mode used by your capture scenario
        .cross_type = ANA_CMPR_CROSS_ANY,     // Keep both crossing directions by default
        .src_chan0_gpio = 0,                  // Must match your board's source signal wiring
        .en_capture_timer = true,             // Required for capture timestamp APIs
    };

Then convert ticks to time with:

.. code:: c

    uint32_t resolution_hz = 0;
    uint32_t current_ticks = 0;
    ESP_ERROR_CHECK(ana_cmpr_get_clock_resolution_hz(cmpr, &resolution_hz));
    ESP_ERROR_CHECK(ana_cmpr_get_capture_timestamps(cmpr, 0, &current_ticks, NULL));
    uint32_t time_us = (uint32_t)(((uint64_t)current_ticks * 1000000U) / resolution_hz);

Practical Configuration Notes
=============================

Choosing the Clock Source
-------------------------

The comparator clock affects sampling behavior. In most applications, use :cpp:enumerator:`ANA_CMPR_CLK_SRC_DEFAULT` unless you have a specific clocking requirement.

Note that the analog comparator clock source can be shared with other GPIO-extension style peripherals on some targets. If different peripherals request incompatible clock sources, initialization can fail.

Choosing the Internal Reference Level
-------------------------------------

The internal reference is defined as a percentage of VDD, not as an arbitrary voltage in volts.

This is convenient when:

- the input signal naturally swings with the supply range
- you want a simple mid-supply or supply-relative threshold

This is less suitable when:

- you need a precise threshold independent of VDD variation
- the threshold must change continuously instead of in coarse steps

Tuning Debounce
---------------

Debounce is one of the first parameters to tune on real hardware.

- Too little debounce: duplicate interrupts or unstable event behavior near the threshold
- Too much debounce: missed crossings on fast-changing signals

There is no single good value for every application. Start small and adjust based on observed signal behavior.

Tuning Scan Timing
------------------

On scan-capable targets, scan timing and resampling should be chosen together:

- Reduce ``poll_period_us`` when you need quicker updates
- Increase :cpp:member:`ana_cmpr_config_t::resample_limit` when you need better noise rejection
- Avoid choosing a scan rate that is much slower than the effective input dynamics you want to detect

Operational Notes
=================

Enable and Disable Lifecycle
----------------------------

The comparator follows a simple lifecycle:

1. Create and configure it
2. Enable it
3. Run your application
4. Disable it before deleting it

Use :cpp:func:`ana_cmpr_enable` and :cpp:func:`ana_cmpr_disable` as a pair.

While enabled, only a limited set of runtime control functions are intended to be adjusted. Configuration functions such as source channel management and scan configuration must be done before enabling the unit.

Power Management
----------------

When :ref:`CONFIG_PM_ENABLE` is enabled, sleep and clock changes can affect the comparator. The driver uses a power management lock internally when needed, so enabling the comparator can prevent light sleep while the unit is active.

If power consumption matters, structure your application so the comparator is only enabled when needed.

IRAM Safe
---------

If you need comparator interrupts to keep working while the cache is disabled, enable :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE`.

If you also need the runtime control functions to remain callable in that situation, enable :ref:`CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM`.

The following control APIs can be placed in IRAM:

- :cpp:func:`ana_cmpr_set_internal_reference`
- :cpp:func:`ana_cmpr_set_debounce`
- :cpp:func:`ana_cmpr_set_src_chan_cross_type`
- :cpp:func:`ana_cmpr_trigger_scan`

Thread Safety
-------------

The driver is designed to be thread-safe for concurrent API calls on the same :cpp:type:`ana_cmpr_handle_t`, by combining:

- A per-handle FSM (INIT/ENABLE/WAIT) with atomic state transitions to serialize incompatible operations
- Short critical sections to keep shared software state and hardware registers updated atomically
- Slot lifecycle protection for create/delete windows

The main lifecycle contract still applies: call :cpp:func:`ana_cmpr_disable` before :cpp:func:`ana_cmpr_del_unit`, and make sure no active dependent objects (for example ETM handles) remain when deleting the unit.

Kconfig Options
---------------

The most relevant Kconfig options are:

- :ref:`CONFIG_ANA_CMPR_ISR_CACHE_SAFE`
    Keeps the default ISR path available when cache is disabled. Enable this if comparator interrupts must work in cache-off windows.
- :ref:`CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM`
    Places ISR-callable runtime control APIs in IRAM, so they remain callable when cache is disabled.
- :ref:`CONFIG_ANA_CMPR_ENABLE_DEBUG_LOG`
    Enables driver debug logs for troubleshooting and bring-up. This increases binary size and log verbosity.

Examples
========

.. list::

    :SOC_ANA_CMPR_SUPPORT_AUTO_SCAN: - :example:`peripherals/analog_comparator/auto_scan` shows auto-scan based threshold detection. After enabling the comparator, hardware scans continuously and updates output in real time, while monitor GPIO is driven via interrupt or ETM depending on target capabilities.
    :SOC_ANA_CMPR_SUPPORT_ETM_SCAN: - :example:`peripherals/analog_comparator/etm_periodic_scan` shows how to use GPTimer and ETM to trigger periodic comparator scans and drive a monitor GPIO from comparator crossing events.

API Reference
=============

Driver APIs
-----------

.. include-build-file:: inc/ana_cmpr.inc

Driver Types
------------

.. include-build-file:: inc/ana_cmpr_types.inc
