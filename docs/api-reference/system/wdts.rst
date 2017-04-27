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
specified by the task watchdog timeout (which is configurable using ``make menuconfig``), the watchdog will
print out a warning with information about which processes are running on the ESP32 CPUs and which processes
failed to feed the watchdog.

By default, the task watchdog watches the idle tasks. The usual cause of idle tasks not feeding the watchdog 
is a higher-priority process looping without yielding to the lower-priority processes, and can be an indicator
of badly-written code that spinloops on a peripheral or a task that is stuck in an infinite loop.

Other task can elect to be watched by the task watchdog by calling ``esp_task_wdt_feed()``. Calling this routine
for the first time will register the task to the task watchdog; calling it subsequent times will feed
the watchdog. If a task does not want to be watched anymore (e.g. because it is finished and will call 
``vTaskDelete()`` on itself), it needs to call ``esp_task_wdt_delete()``.

The task watchdog is built around the hardware watchdog in timer group 0. If this watchdog for some reason
cannot execute the interrupt handler that prints the task data (e.g. because IRAM is overwritten by garbage
or interrupts are disabled entirely) it will hard-reset the SOC.

JTAG and watchdogs
^^^^^^^^^^^^^^^^^^

While debugging using OpenOCD, if the CPUs are halted the watchdogs will keep running, eventually resetting the
CPU. This makes it very hard to debug code; that is why the OpenOCD config will disable both watchdogs on startup.
This does mean that you will not get any warnings or panics from either the task or interrupt watchdog when the ESP32
is connected to OpenOCD via JTAG.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`esp32/include/esp_int_wdt.h`
  * :component_file:`esp32/include/esp_task_wdt.h`


Functions
---------

.. doxygenfunction:: esp_int_wdt_init
.. doxygenfunction:: esp_task_wdt_init
.. doxygenfunction:: esp_task_wdt_feed
.. doxygenfunction:: esp_task_wdt_delete
