Watchdogs
=========

Overview
--------

The ESP-IDF has support for multiple types of watchdogs, with the two main ones being: The Interrupt Watchdog Timer
and the Task Watchdog Timer (TWDT). The Interrupt Watchdog Timer and the TWDT
can both be enabled using :ref:`project-configuration-menu`, however the TWDT can also be
enabled during runtime. The Interrupt Watchdog is responsible for detecting
instances where FreeRTOS task switching is blocked for a prolonged period of
time. The TWDT is responsible for detecting instances of tasks running without
yielding for a prolonged period.

Interrupt watchdog
^^^^^^^^^^^^^^^^^^

The interrupt watchdog makes sure the FreeRTOS task switching interrupt isn't blocked for a long time. This
is bad because no other tasks, including potentially important ones like the WiFi task and the idle task,
can't get any CPU runtime. A blocked task switching interrupt can happen because a program runs into an
infinite loop with interrupts disabled or hangs in an interrupt.

The default action of the interrupt watchdog is to invoke the panic handler. causing a register dump and an opportunity
for the programmer to find out, using either OpenOCD or gdbstub, what bit of code is stuck with interrupts
disabled. Depending on the configuration of the panic handler, it can also blindly reset the CPU, which may be
preferred in a production environment.

The interrupt watchdog is built around the hardware watchdog in timer group 1. If this watchdog for
some reason cannot execute the NMI handler that invokes the panic handler (e.g. because IRAM is
overwritten by garbage), it will hard-reset the SOC. If the panic handler executes, it will display
the panic reason as "Interrupt wdt timeout on CPU0" or "Interrupt wdt timeout on CPU1" (as
applicable).

Configuration
@@@@@@@@@@@@@

The interrupt watchdog is enabled by default via the :ref:`CONFIG_ESP_INT_WDT` configuration
flag. The timeout is configured by setting :ref:`CONFIG_ESP_INT_WDT_TIMEOUT_MS`. The default
timeout is higher if PSRAM support is enabled, as a critical section or interrupt routine that
accesses a large amount of PSRAM will take longer to complete in some circumstances. The INT WDT
timeout should always be longer than the period between FreeRTOS ticks (see
:ref:`CONFIG_FREERTOS_HZ`).

Tuning
@@@@@@

If you find the Interrupt watchdog timeout is triggering because an interrupt or critical section is
running longer than the timeout period, consider rewriting the code: critical sections should be
made as short as possible, with non-critical computation happening outside the critical
section. Interrupt handlers should also perform the minimum possible amount of computation, consider
pushing data into a queue from the ISR and processing it in a task instead. Neither critical
sections or interrupt handlers should ever block waiting for another event to occur.

If changing the code to reduce the processing time is not possible or desirable, it's possible to
increase the :ref:`CONFIG_ESP_INT_WDT_TIMEOUT_MS` setting instead.

.. _task-watchdog-timer:

Task Watchdog Timer
^^^^^^^^^^^^^^^^^^^

The Task Watchdog Timer (TWDT) is responsible for detecting instances of tasks
running for a prolonged period of time without yielding. This is a symptom of
CPU starvation and is usually caused by a higher priority task looping without
yielding to a lower-priority task thus starving the lower priority task from
CPU time. This can be an indicator of poorly written code that spinloops on a
peripheral, or a task that is stuck in an infinite loop.

{IDF_TARGET_IDLE_TASKS:default="Idle task", esp32="Idle Tasks of each CPU"}

By default the TWDT will watch the {IDF_TARGET_IDLE_TASKS}, however any task can
subscribe to be watched by the TWDT. Each watched task must 'reset' the TWDT
periodically to indicate that they have been allocated CPU time. If a task does
not reset within the TWDT timeout period, a warning will be printed with
information about which tasks failed to reset the TWDT in time and which
tasks are currently running.

It is also possible to redefine the function `esp_task_wdt_isr_user_handler`
in the user code, in order to receive the timeout event and handle it differently.

The TWDT is built around the Hardware Watchdog Timer in Timer Group 0. The TWDT
can be initialized by calling :cpp:func:`esp_task_wdt_init` which will configure
the hardware timer. A task can then subscribe to the TWDT using
:cpp:func:`esp_task_wdt_add` in order to be watched. Each subscribed task must
periodically call :cpp:func:`esp_task_wdt_reset` to reset the TWDT. Failure by
any subscribed tasks to periodically call :cpp:func:`esp_task_wdt_reset`
indicates that one or more tasks have been starved of CPU time or are stuck in a
loop somewhere.

A watched task can be unsubscribed from the TWDT using
:cpp:func:`esp_task_wdt_delete()`. A task that has been unsubscribed should no
longer call :cpp:func:`esp_task_wdt_reset`. Once all tasks have unsubscribed
form the TWDT, the TWDT can be deinitialized by calling
:cpp:func:`esp_task_wdt_deinit()`.

The default timeout period for the TWDT is set using config item
:ref:`CONFIG_ESP_TASK_WDT_TIMEOUT_S`. This should be set to at least as long as you expect any
single task will need to monopolise the CPU (for example, if you expect the app will do a long
intensive calculation and should not yield to other tasks). It is also possible to change this
timeout at runtime by calling :cpp:func:`esp_task_wdt_init`.

The following config options control TWDT configuration at startup. They are all enabled by default:

{IDF_TARGET_IDLE_TASK:default="Idle task", esp32="CPU0 Idle task", esp32s3="CPU0 Idle task"}

.. list::

    - :ref:`CONFIG_ESP_TASK_WDT` - the TWDT is initialized automatically during startup. If this option is disabled, it is still possible to initialize the Task WDT at runtime by calling :cpp:func:`esp_task_wdt_init`.
    - :ref:`CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0` - {IDF_TARGET_IDLE_TASK} is subscribed to the TWDT during startup. If this option is disabled, it is still possible to subscribe the idle task by calling :cpp:func:`esp_task_wdt_add` at any time.
    :not CONFIG_FREERTOS_UNICORE: - :ref:`CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1` - CPU1 Idle task is subscribed to the TWDT during startup.


JTAG and watchdogs
^^^^^^^^^^^^^^^^^^

While debugging using OpenOCD, the CPUs will be halted every time a breakpoint
is reached. However if the watchdog timers continue to run when a breakpoint is
encountered, they will eventually trigger a reset making it very difficult to
debug code. Therefore OpenOCD will disable the hardware timers of both the
interrupt and task watchdogs at every breakpoint. Moreover, OpenOCD will not
reenable them upon leaving the breakpoint. This means that interrupt watchdog
and task watchdog functionality will essentially be disabled. No warnings or
panics from either watchdogs will be generated when the {IDF_TARGET_NAME} is connected to
OpenOCD via JTAG.


.. only:: SOC_XT_WDT_SUPPORTED

  XTAL32K Watchdog Timer (XTWDT)
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  The XTAL32K watchdog makes sure the (optional) external 32 KHz crystal or oscillator is functioning correctly.

  When `XTAL32K_CLK` works as the clock source of `RTC_SLOW_CLK` and stops oscillating, the XTAL32K watchdog timer will detect this and generate an interrupt.
  It also provides functionality for automatically switching over to the internal, but less accurate oscillator as the `RTC_SLOW_CLK` source.

  Since the switch to the backup clock is done in hardware it can also happen during deep sleep. This means that even if `XTAL32K_CLK` stops functioning while the chip in deep sleep, waiting for a timer to expire, it will still be able to wake-up as planned.

  If the `XTAL32K_CLK` starts functioning normally again, you can call `esp_xt_wdt_restore_clk` to switch back to this clock source and re-enable the watchdog timer.

  Configuration
  @@@@@@@@@@@@@

  When the external 32KHz crystal or oscillator is selected (:ref:`CONFIG_{IDF_TARGET_CFG_PREFIX}_RTC_CLK_SRC`) the XTAL32K watchdog can be enabled via the :ref:`CONFIG_ESP_XT_WDT` configuration
  flag. The timeout is configured by setting :ref:`CONFIG_ESP_XT_WDT_TIMEOUT`. The automatic backup clock functionality is enabled via the ref:`CONFIG_ESP_XT_WDT_BACKUP_CLK_ENABLE` configuration.

Interrupt Watchdog API Reference
--------------------------------

Header File
^^^^^^^^^^^

  * :component_file:`esp_system/include/esp_int_wdt.h`


Functions
---------

.. doxygenfunction:: esp_int_wdt_init

Task Watchdog API Reference
----------------------------

A full example using the Task Watchdog is available in esp-idf: :example:`system/task_watchdog`

.. include-build-file:: inc/esp_task_wdt.inc
