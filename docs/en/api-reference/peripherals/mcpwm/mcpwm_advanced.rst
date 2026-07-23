======================
MCPWM Advanced Topics
======================

.. contents::
    :local:
    :depth: 2

Resolution and shared clocks
============================

``resolution_hz`` is the timer tick rate; one tick lasts ``1 / resolution_hz`` seconds. MCPWM selects dividers from the selected source clock. If the requested rate is exactly divisible, it is preferred. Otherwise the driver keeps the group clock as high as possible and selects the closest achievable submodule rate.

The group divider is shared. On targets with capture clock from the group, PWM and capture timers share it too. When multiple objects need different resolutions, allocate them in one monotonic order (all high-to-low or all low-to-high), rather than allocating an arbitrary mix.

The clock tree below shows how a single source clock fans out. The group divider is fixed once for the whole group, and each timer only gets its own prescaler on top of it:

.. mermaid::

    flowchart LR
        src["Source clock<br/>(e.g. 80 MHz)"]:::src -->|"÷ group prescale<br/>shared by the group"| grp["Group clock<br/>(e.g. 40 MHz)"]:::grp
        grp -->|"÷ timer prescale"| pwm["PWM timer<br/>resolution_hz = 10 MHz"]:::mod
        grp -->|"÷ capture prescale"| cap["Capture timer<br/>resolution_hz = 20 MHz"]:::mod
        classDef src fill:#fef3c7,stroke:#d97706,color:#78350f
        classDef grp fill:#dbeafe,stroke:#2563eb,color:#172554
        classDef mod fill:#dcfce7,stroke:#16a34a,color:#14532d

Because the group prescale is shared, it is chosen once for all timers. A later request for a different resolution cannot move that fixed divider, so the driver keeps the group clock as high as possible and picks the closest achievable prescale for the new submodule instead.

Power management and sleep
==========================

With power management enabled, :cpp:func:`mcpwm_timer_enable()` and :cpp:func:`mcpwm_capture_timer_enable()` hold an :cpp:enumerator:`esp_pm_lock_type_t::ESP_PM_NO_LIGHT_SLEEP` lock so the timer keeps a stable clock frequency. Call :cpp:func:`mcpwm_timer_disable()` or :cpp:func:`mcpwm_capture_timer_disable()` to release the lock.

Both the timer config and the capture timer config carry an :cpp:member:`allow_pd <mcpwm_timer_config_t::flags::allow_pd>` field (the latter in :cpp:type:`mcpwm_capture_timer_config_t`). Set it if the application permits MCPWM's power domain to power down during sleep. The driver then saves and restores registers, at the cost of extra RAM. This capability is target dependent.

ISR and task safety
===================

Callbacks for timer, comparator, fault, operator brake, and capture run in ISR context. Keep them non-blocking and use ISR-safe RTOS calls only. The first callback registered in a group fixes its shared interrupt priority; make all later event users in that group use the same priority.

Factory functions such as :cpp:func:`mcpwm_new_timer()` are thread-safe. :cpp:func:`mcpwm_timer_set_period()` and :cpp:func:`mcpwm_comparator_set_compare_value()` may run from ISR context. Other control APIs are not generally thread-safe; serialize them if more than one task can access the same object.

Cache-safe real-time operation
==============================

Normally, MCPWM interrupts are deferred while cache is disabled (for example during flash operations). Enable :menuitem:`CONFIG_MCPWM_ISR_CACHE_SAFE` when callbacks must continue to run; this places ISR-required code in IRAM and objects in DRAM, increasing internal RAM use.

.. note::

    With this option enabled, MCPWM interrupts still fire immediately and are not deferred even while cache is disabled. However, every function on the interrupt path — including your registered callback and all functions it calls — must live in IRAM: with cache off, the CPU cannot fetch instructions from flash, so calling any function still resident in flash crashes the CPU. The option only moves the driver's own ISR code to IRAM; you must place your callbacks and the functions they call in IRAM explicitly (for example with ``IRAM_ATTR``).

:menuitem:`CONFIG_MCPWM_CTRL_FUNC_IN_IRAM` additionally places :cpp:func:`mcpwm_timer_set_period()` and :cpp:func:`mcpwm_comparator_set_compare_value()` in IRAM, so those calls keep working even while cache is off — for example you can retune the PWM period or duty from a cache-disabled context, such as inside a flash-writing routine, without waiting for the cache to be re-enabled.

Kconfig options
===============

.. list::

    - :menuitem:`CONFIG_MCPWM_ISR_CACHE_SAFE` enables cache-safe interrupts.
    - :menuitem:`CONFIG_MCPWM_CTRL_FUNC_IN_IRAM` moves selected control functions to IRAM.
    - :menuitem:`CONFIG_MCPWM_ENABLE_DEBUG_LOG` forces the MCPWM driver to compile in and print its own debug logs, ignoring the global log settings, and raises the runtime log level to verbose for the driver only — other modules are unaffected. The price is increased firmware size.
