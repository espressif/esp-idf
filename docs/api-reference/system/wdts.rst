Watchdogs
=========

Overview
--------

Esp-idf has support for two types of watchdogs: a task watchdog as well as an interrupt watchdog. Both can be
enabled using ``make menuconfig`` and selecting the appropriate options.

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

Task watchdog
^^^^^^^^^^^^^

Any tasks can elect to be watched by the task watchdog. If such a task does not feed the watchdog within the time
specified by the task watchdog timeout, the watchdog will print out a warning with information about which processes 
are running on the ESP32 CPUs and which processes failed to feed the watchdog.

Ideally, the task watchdog should watch the idle tasks. The usual cause of the idle tasks not feeding the watchdog 
is a higher-priority process looping without yielding to the lower-priority processes, and can be an indicator
of badly-written code that spinloops on a peripheral or a task that is stuck in an infinite loop.

Other task can elect to be watched by the task watchdog. If the watchdog watches multiple tasks, all the tasks
must feed before the watchdog times out. If one or more tasks fails to feed, the watchdog timeout will trigger.
The watchdog timer can be initialized using :cpp:func:`esp_task_wdt_init` which will configure the hardware timer and
establish the hardware timer interrupts. In order for a task to be watched by the task watchdog, a task must subscribe to
the task watchdog using :cpp:func:`esp_task_wdt_add`. Once added, each subscribed task must call :cpp:func:`esp_task_wdt_feed` 
periodically to prevent the watchdog from timing out. If all tasks being watched have fed, the watchdog timer counter immediately resets
and starts recounting to the timeout value from zero. To unsubscribe a task from the task watchdog, call :cpp:func:`esp_task_wdt_delete()`.
Once all tasks have been unsubscribed form the task watchdog, :cpp:func:`esp_task_wdt_deinit()` can be called to deinitialize
the task watchdog. Deinitialization will stop the hardware timer, deregister the hardware timer interrupts, and remove the idle hooks
if idle tasks were being watched.

By setting the task watchdog options in ``make menuconfig``, the task watchdog can be automatically initialized
during startup by enabling the :ref:`CONFIG_TASK_WDT`. Moreover the two Idle tasks can also be subscibed to the 
task watchdog on startup as well by enabling :ref:`CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU0` and 
:ref:`CONFIG_TASK_WDT_CHECK_IDLE_TASK_CPU1`.

The task watchdog is built around the hardware watchdog in timer group 0. If the watchdog for some reason
cannot execute the interrupt handler that prints the task data (e.g. because IRAM is overwritten by garbage
or interrupts are disabled entirely) it will hard-reset the SOC.

Note: ``make menuconfig`` provides a :ref:`CONFIG_TASK_WDT_LEGACY_BEHAVIOR` option which will change the behavior of the 
task watchdog to make it compatible with the legacy code. Note that the legacy behavior causes tasks to subscribe to the
task watchdog on their first call to :cpp:func:`esp_task_wdt_feed`. Moreover, legacy behavior only allows
tasks to unsubscribe on their own behalf when calling :cpp:func:`esp_task_wdt_delete()`. It is strongly recommended that 
non-legacy behavior is used as additions/deletions/feeds are explicit.


JTAG and watchdogs
^^^^^^^^^^^^^^^^^^

While debugging using OpenOCD, if the CPUs are halted the watchdogs will keep running, eventually resetting the
CPU. This makes it very hard to debug code; that is why the OpenOCD config will disable both watchdogs on startup.
This does mean that you will not get any warnings or panics from either the task or interrupt watchdog when the ESP32
is connected to OpenOCD via JTAG.

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

NOTE: Ensure that the :ref:`CONFIG_TASK_WDT_LEGACY_BEHAVIOR` configuraiton is disabled using ``make menuconfig``
to prevent legacy behavior of the task watchdog.

A full example using the Task Watchdog is available in esp-idf: :example:`system/task_watchdog`


.. include:: /_build/inc/esp_task_wdt.inc
