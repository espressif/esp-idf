Watchdogs
=========

The purpose of a watchdog timer is to monitor the system's operation and automatically recover from software or hardware faults by restarting the system if it becomes unresponsive.

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-IDF supports multiple types of watchdog timers:

.. list::

    - Interrupt Watchdog Timer (IWDT)
    - Task Watchdog Timer (TWDT)
    - RTC/LP Watchdog Timer (RTC_WDT/LP_WDT)
    :SOC_XT_WDT_SUPPORTED: - XTAL32K Watchdog Timer (Crystal 32K Watchdog Timer, i.e., XTWDT)

The Interrupt Watchdog Timer (IWDT) is responsible for ensuring that ISRs (Interrupt Service Routines) are not blocked from running for a prolonged period of time, and also ensures that an ISR isn't stuck while executing. The Task Watchdog Timer (TWDT) is responsible for detecting instances of tasks running without yielding for too long. The RTC Watchdog Timer (RTC_WDT) is used to track the boot time from power-up until the user's main function, during panic handling and it can also be used from the low-power domain.

The various watchdog timers can be enabled by :ref:`project-configuration-menu`. However, the TWDT can also be enabled during runtime.

.. _app-hardware-watchdog-timers:

Hardware Watchdog Timers
------------------------

The chips have two groups of watchdog timers:

.. list::

    :not esp32c2: - Main System Watchdog Timer (MWDT_WDT) - used by Interrupt Watchdog Timer (IWDT) and Task Watchdog Timer (TWDT).
    :esp32c2: - Main System Watchdog Timer (MWDT_WDT) - used by Interrupt Watchdog Timer (IWDT).
    - RTC Watchdog Timer (RTC_WDT) - used to track the boot time from power-up until the user's main function (by default RTC Watchdog is disabled immediately before the user's main function).

Refer to the :ref:`bootloader-watchdog` section to understand how watchdogs are utilized in the bootloader.

The app's behaviour can be adjusted so the RTC Watchdog remains enabled after app startup. The Watchdog would need to be explicitly reset (i.e., fed) or disabled by the app to avoid the chip reset. To do this, set the :ref:`CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE` option, modify the app as needed, and then recompile the app. In this case, the following APIs should be used:

.. list::

    - :cpp:func:`wdt_hal_disable`: see :ref:`hw-abstraction-hal-layer-disable-rtc-wdt`
    - :cpp:func:`wdt_hal_feed`: see :ref:`hw-abstraction-hal-layer-feed-rtc-wdt`
    :esp32 or esp32s2: - :cpp:func:`rtc_wdt_feed`
    :esp32 or esp32s2: - :cpp:func:`rtc_wdt_disable`

If RTC_WDT is not reset/disabled in time, the chip will be automatically reset. See :ref:`RTC-Watchdog-Timeout` for more information.

Interrupt Watchdog Timer (IWDT)
-------------------------------

{IDF_TARGET_IWDT_TIMER_GROUP:default="Timer Group 1", esp32c2="Timer Group 0"}

The purpose of the IWDT is to ensure that interrupt service routines (ISRs) are not blocked from running for a prolonged period of time (i.e., the IWDT timeout period). Preventing ISRs from running in a timely manner is undesirable as it can increase ISR latency, and also prevent task switching (as task switching is executed from an ISR). The things that can block ISRs from running include:

- Disabling interrupts
- Critical Sections (also disables interrupts)
- Other same/higher priority ISRs which block same/lower priority ISRs from running

The IWDT utilizes the MWDT_WDT watchdog timer in {IDF_TARGET_IWDT_TIMER_GROUP} as its underlying hardware timer and leverages the FreeRTOS tick interrupt on each CPU to feed the watchdog timer. If the tick interrupt on a particular CPU is not executed within the IWDT timeout period, it indicates that something is blocking ISRs from being run on that CPU (see the list of reasons above).

When the IWDT times out, the default action is to invoke the panic handler and display the panic reason as ``Interrupt wdt timeout on CPU0`` or ``Interrupt wdt timeout on CPU1`` (as applicable). Depending on the panic handler's configured behavior (see :ref:`CONFIG_ESP_SYSTEM_PANIC`), users can then debug the source of the IWDT timeout (via the backtrace, OpenOCD, gdbstub etc) or simply reset the chip (which may be preferred in a production environment).

If for whatever reason the panic handler is unable to run after an IWDT timeout, the IWDT has a second stage timeout that will hard-reset the chip (i.e., a system reset).

Configuration
^^^^^^^^^^^^^

- The IWDT is enabled by default via the :ref:`CONFIG_ESP_INT_WDT` option.
- The IWDT's timeout is configured by setting the :ref:`CONFIG_ESP_INT_WDT_TIMEOUT_MS` option.

    - Note that the default timeout is higher if PSRAM support is enabled, as a critical section or interrupt routine that accesses a large amount of PSRAM takes longer to complete in some circumstances.
    - The configured timeout duration for IWDT should always be at least twice longer than the period between two FreeRTOS ticks, e.g., if two FreeRTOS ticks occur 10 ms apart, then IWDT timeout duration should at least be more than 20 ms (see :ref:`CONFIG_FREERTOS_HZ`).

Tuning
^^^^^^

If you find the IWDT timeout is triggered because an interrupt or critical section is running longer than the timeout period, consider rewriting the code:

- Critical sections should be made as short as possible. Any non-critical code/computation should be placed outside the critical section.
- Interrupt handlers should also perform the minimum possible amount of computation. Users can consider deferring any computation to a task by having the ISR push data to a task using queues.

Neither critical sections or interrupt handlers should ever block waiting for another event to occur. If changing the code to reduce the processing time is not possible or desirable, it is possible to increase the :ref:`CONFIG_ESP_INT_WDT_TIMEOUT_MS` setting instead.

.. _task-watchdog-timer:

Task Watchdog Timer (TWDT)
--------------------------

The Task Watchdog Timer (TWDT) is used to monitor particular tasks, ensuring that they are able to execute within a given timeout period. By default the TWDT watches the Idle Tasks of each CPU, however any task can subscribe to be watched by the TWDT. By watching the Idle Tasks of each CPU, the TWDT can detect instances of tasks running for a prolonged period of time without yielding. This can be an indicator of poorly written code that spinloops on a peripheral, or a task that is stuck in an infinite loop.

.. only:: not esp32c2

    The TWDT is built around the MWDT_WDT watchdog timer in Timer Group 0. When a timeout occurs, an interrupt is triggered.

.. only:: esp32c2

    The {IDF_TARGET_NAME} has only a single Timer Group, used by Interrupt Watchdog (IWDT). Thus, the Task Watchdog is built around the ``esp_timer`` component in order to implement a software timer. When a timeout occurs, an interrupt is triggered, notifying the ``esp_timer``'s main task. The latter then executes the TWDT callback previously registered.

Users can define the function ``esp_task_wdt_isr_user_handler`` in the user code, in order to receive the timeout event and extend the default behavior.


Usage
^^^^^

The following functions can be used to watch tasks using the TWDT:

- :cpp:func:`esp_task_wdt_init` to initialize the TWDT and subscribe the idle tasks.
- :cpp:func:`esp_task_wdt_add` subscribes other tasks to the TWDT.
- Once subscribed, :cpp:func:`esp_task_wdt_reset` should be called from the task to feed the TWDT.
- :cpp:func:`esp_task_wdt_delete()` unsubscribes a previously subscribed task.
- :cpp:func:`esp_task_wdt_deinit()` unsubscribes the idle tasks and deinitializes the TWDT.


In the case where applications need to watch at a more granular level (i.e., ensure that a particular functions/stub/code-path is called), the TWDT allows subscription of ``users``.

- :cpp:func:`esp_task_wdt_add_user` to subscribe an arbitrary user of the TWDT. This function returns a user handle to the added user.
- :cpp:func:`esp_task_wdt_reset_user` must be called using the user handle in order to prevent a TWDT timeout.
- :cpp:func:`esp_task_wdt_delete_user` unsubscribes an arbitrary user of the TWDT.

Configuration
^^^^^^^^^^^^^

The default timeout period for the TWDT is set using config item :ref:`CONFIG_ESP_TASK_WDT_TIMEOUT_S`. This should be set to at least as long as you expect any single task needs to monopolize the CPU (for example, if you expect the app will do a long intensive calculation and should not yield to other tasks). It is also possible to change this timeout at runtime by calling :cpp:func:`esp_task_wdt_init`.

.. note::

    Erasing large flash areas can be time consuming and can cause a task to run continuously, thus triggering a TWDT timeout. The following two methods can be used to avoid this:

    - Increase :ref:`CONFIG_ESP_TASK_WDT_TIMEOUT_S` in menuconfig for a larger watchdog timeout period.
    - You can also call :cpp:func:`esp_task_wdt_init` again to increase the watchdog timeout period before erasing a large flash area.

    For more information, you can refer to :doc:`../peripherals/spi_flash/index`.

The following config options control TWDT configuration. They are all enabled by default:

{IDF_TARGET_IDLE_TASK:default="Idle task", esp32="CPU0 Idle task", esp32s3="CPU0 Idle task", esp32p4="CPU0 Idle task"}

.. list::

    - :ref:`CONFIG_ESP_TASK_WDT_EN` - enables TWDT feature. If this option is disabled, TWDT cannot be used, even if initialized at runtime.
    - :ref:`CONFIG_ESP_TASK_WDT_INIT` - initializes the TWDT automatically during startup. If this option is disabled, it is still possible to initialize the Task WDT at runtime by calling :cpp:func:`esp_task_wdt_init`.
    - :ref:`CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0` - subscribes {IDF_TARGET_IDLE_TASK} to the TWDT during startup. If this option is disabled, it is still possible to subscribe the idle task by calling :cpp:func:`esp_task_wdt_init` again, or by using :cpp:func:`esp_task_wdt_add` and passing it the idle task handle obtained via :cpp:func:`xTaskGetIdleTaskHandleForCore`.
    :SOC_HP_CPU_HAS_MULTIPLE_CORES: - :ref:`CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1` - Subscribes CPU1 Idle task to the TWDT during startup.


.. note::

    On a TWDT timeout the default behaviour is to simply print a warning and a backtrace before continuing running the app. If you want a timeout to cause a panic and a system reset then this can be configured through :ref:`CONFIG_ESP_TASK_WDT_PANIC`.


.. only:: SOC_XT_WDT_SUPPORTED

    XTAL32K Watchdog Timer (XTWDT)
    ------------------------------

    One of the optional clock inputs to the {IDF_TARGET_NAME} is an external 32 kHz crystal oscillator (XTAL32K) that is used as a clock source (``XTAL32K_CLK``) to various subsystems (such as the RTC).

    The XTWDT is a dedicated watchdog timer used to ensure that the XTAL32K is functioning correctly. When ``XTAL32K_CLK`` works as the clock source of ``RTC_SLOW_CLK`` and stops oscillating, the XTWDT will detect this and generate an interrupt. It also provides functionality for automatically switching over to the internal, but less accurate oscillator as the ``RTC_SLOW_CLK`` source.

    Since the switch to the backup clock is done in hardware it can also happen during Deep Sleep. This means that even if ``XTAL32K_CLK`` stops functioning while the chip is in Deep Sleep, waiting for a timer to expire, it is still able to wake-up as planned.

    If the ``XTAL32K_CLK`` starts functioning normally again, you can call ``esp_xt_wdt_restore_clk`` to switch back to this clock source and re-enable the watchdog timer.

    Configuration
    """""""""""""

    - When the external 32 KHz crystal or oscillator is selected (:ref:`CONFIG_RTC_CLK_SRC`) the XTWDT can be enabled via the :ref:`CONFIG_ESP_XT_WDT` configuration option.
    - The timeout is configured by setting the :ref:`CONFIG_ESP_XT_WDT_TIMEOUT` option.
    - The automatic backup clock functionality is enabled via the ref:`CONFIG_ESP_XT_WDT_BACKUP_CLK_ENABLE` configuration option.

Timeout Stages
--------------

Hardware watchdog timers in ESP-IDF have four timeout stages. If the WDT is not fed in the previous stage within the configured timeout duration of that stage, it will trigger the next stage. Each stage can be configured to take one out of below mentioned actions after the timeout duration:

- Trigger an interrupt. When the stage expires an interrupt is triggered.
- Reset a CPU core. When the stage expires the designated CPU core will be reset.
- Reset the main system. When the stage expires, the main system, including the MWDTs, will be reset. The main system includes the CPU and all peripherals. The RTC is an exception to this, and it will not be reset.
- Reset the main system and RTC. When the stage expires the main system and the RTC will both be reset. This action is only available in the RTC_WDT.
- Disabled. This stage will have no effects on the system.

The typical configuration of the stages can be done by having interrupt actions in the earlier stages and allowing stepwise handling up to a final system reset action in the later stages.

Stages can be configured via :cpp:func:`wdt_hal_config_stage` (or equivalent APIs), selecting actions per stage to match application behavior.

JTAG & Watchdogs
----------------

While debugging using OpenOCD, the CPUs are halted every time a breakpoint is reached. However if the watchdog timers continue to run when a breakpoint is encountered, they will eventually trigger a reset making it very difficult to debug code. Therefore OpenOCD will disable the hardware timers of both the interrupt and task watchdogs at every breakpoint. Moreover, OpenOCD will not re-enable them upon leaving the breakpoint. This means that interrupt watchdog and task watchdog functionality will essentially be disabled. No warnings or panics from either watchdogs will be generated when the {IDF_TARGET_NAME} is connected to OpenOCD via JTAG.

Common error logs when WDT triggers and possible resolutions
------------------------------------------------------------

- ``Guru Meditation Error: Core  0 panic'ed (Interrupt wdt timeout on CPU0).`` followed by a backtrace: Indicates that the Interrupt Watchdog Timer (IWDT) has detected that interrupts have been blocked on CPU 0 for longer than the configured timeout period. This can be fixed by reducing the duration of long-running ISRs or critical sections, or by increasing the IWDT timeout period.
- ``Task watchdog got triggered. The following tasks/users did not reset the watchdog in time: - IDLE0 (CPU 0), Tasks currently running: CPU 0: main, CPU 1: IDLE1``: Indicates that the Task Watchdog Timer (TWDT) has detected that one or more tasks have not yielded within the configured timeout period, and hence the IDLE task couldn't feed the TWDT in time. This can be fixed by ensuring that tasks yield appropriately, reducing the duration of long-running tasks, or by increasing the TWDT timeout period. User can also use :cpp:func:`esp_task_wdt_add`, :cpp:func:`esp_task_wdt_add_user` and :cpp:func:`esp_task_wdt_reset_user` APIs to find out which task and which code path inside that task is taking the longest time and causing the TWDT timeout.
- :ref:`CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE` is enabled and causes WDT timeout: Make sure RTC WDT is fed frequently enough from user code.
- WDT Reset happens during boot-up: Ensure that a valid secondary bootloader is flashed correctly and investigate possible communication issues with external flash.
- WDT Reset happens during operation: Try to identify when it happens, is it during a panic, a restart or enter/exit light-sleep? If it happens during any of these system operations, it could point towards an issue inside ESP-IDF.

Application Examples
--------------------

- :example:`system/task_watchdog` demonstrates how to initialize, subscribe and unsubscribe tasks and users to the task watchdog, and how tasks and users can reset (feed) the task watchdog.

API Reference
-------------

.. include-build-file:: inc/esp_task_wdt.inc
