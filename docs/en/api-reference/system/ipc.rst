Inter-Processor Call
====================

Overview
--------

Due to the dual core nature of the ESP32, there are instances where a certain
function must be run in the context of a particular core (e.g. allocating
ISR to an interrupt source of a particular core). The IPC (Inter-Processor 
Call) feature allows for the execution of functions on a particular CPU.

A given function can be executed on a particular core by calling 
:cpp:func:`esp_ipc_call` or :cpp:func:`esp_ipc_call_blocking`. IPC is 
implemented via two high priority FreeRTOS tasks pinned to each CPU known as
the IPC Tasks. The two IPC Tasks remain inactive (blocked) until 
:cpp:func:`esp_ipc_call` or :cpp:func:`esp_ipc_call_blocking` is called. When 
an IPC Task of a particular core is unblocked, it will preempt the current 
running task on that core and execute a given function. 

Usage
-----

:cpp:func:`esp_ipc_call` unblocks the IPC task on a particular core to execute
a given function. The task that calls :cpp:func:`esp_ipc_call` will be blocked
until the IPC Task begins execution of the given function. 
:cpp:func:`esp_ipc_call_blocking` is similar but will block the calling task
until the IPC Task has completed execution of the given function.
 
Functions executed by IPCs must be functions of type 
`void func(void *arg)`. To run more complex functions which require a larger 
stack, the IPC tasks' stack size can be configured by modifying 
:ref:`CONFIG_IPC_TASK_STACK_SIZE` in `menuconfig`. The IPC API is protected by a
mutex hence simultaneous IPC calls are not possible.

Care should taken to avoid deadlock when writing functions to be executed by
IPC, especially when attempting to take a mutex within the function.

API Reference
-------------

.. include:: /_build/inc/esp_ipc.inc


