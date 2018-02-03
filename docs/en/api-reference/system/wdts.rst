Watchdogs
=========

Overview
--------

The ESP-IDF has support for two types of watchdogs: The Interrupt Watchdog Timer
and the Task Watchdog Timer (TWDT). The Interrupt Watchdog Timer and the TWDT
can both be enabled using ``make menuconfig``, however the TWDT can also be
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

The interrupt watchdog is built around the hardware watchdog in timer group 1. If this watchdog for some reason
cannot execute the NMI handler that invokes the panic handler (e.g. because IRAM is overwritten by garbage),
it will hard-reset the SOC.

Task Watchdog Timer
^^^^^^^^^^^^^^^^^^^

The Task Watchdog Timer (TWDT) is responsible for detecting instances of tasks 
running for a prolonged period of time without yielding. This is a symptom of 
CPU starvation and is usually caused by a higher priority task looping without
yielding to a lower-priority task thus starving the lower priority task from
CPU time. This can be an indicator of poorly written code that spinloops on a
peripheral, or a task that is stuck in an infinite loop. 

By default the TWDT will watch the Idle Tasks of each CPU, however any task can 
elect to be watched by the TWDT. Each watched task must 'reset' the TWDT
periodically to indicate that they have been allocated CPU time. If a task does 
not reset within the TWDT timeout period, a warning will be printed with 
information about which tasks failed to reset the TWDT in time and which 
tasks are currently running on the ESP32 CPUs and.

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

By default :ref:`CONFIG_TASK_WDT` in ``make menuconfig`` will be enabled causing
the TWDT to be initialized automatically during startup. Likewise
:ref:`CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU0` and 
:ref:`CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU1` are also enabled by default causing
the two Idle Tasks to be subscribed to the TWDT during startup.

JTAG and watchdogs
^^^^^^^^^^^^^^^^^^

While debugging using OpenOCD, the CPUs will be halted every time a breakpoint 
is reached. However if the watchdog timers continue to run when a breakpoint is 
encountered, they will eventually trigger a reset making it very difficult to 
debug code. Therefore OpenOCD will disable the hardware timers of both the 
interrupt and task watchdogs at every breakpoint. Moreover, OpenOCD will not 
reenable them upon leaving the breakpoint. This means that interrupt watchdog
and task watchdog functionality will essentially be disabled. No warnings or 
panics from either watchdogs will be generated when the ESP32 is connected to 
OpenOCD via JTAG.


Interrupt Watchdog API Reference
--------------------------------

Header File
^^^^^^^^^^^

  * :component_file:`esp32/include/esp_int_wdt.h`


Functions
---------
 
.. doxygenfunction:: esp_int_wdt_init

Task Watchdog API Reference
----------------------------

A full example using the Task Watchdog is available in esp-idf: :example:`system/task_watchdog`

.. include:: /_build/inc/esp_task_wdt.inc
