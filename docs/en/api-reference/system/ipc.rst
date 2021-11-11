Inter-Processor Call
====================

Overview
--------

Due to the dual core nature of the {IDF_TARGET_NAME}, there are instances where a certain
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
:ref:`CONFIG_ESP_IPC_TASK_STACK_SIZE` in `menuconfig`. The IPC API is protected by a
mutex hence simultaneous IPC calls are not possible.

Care should taken to avoid deadlock when writing functions to be executed by
IPC, especially when attempting to take a mutex within the function.

.. _hi_priority_ipc:

Hi-priority IPC
---------------

In some cases, we need to quickly get the state of the other CPU (for example, in the core dump, in the GDB stub, in some unit tests, in the dport workaround, etc.), and usually it can be implemented as a fairly simple function and can be written in assembler. 
For these purposes, the IPC API has special functions :cpp:func:`esp_ipc_isr_asm_call` and :cpp:func:`esp_ipc_isr_asm_call_blocking`. 
The `esp_ipc_isr_asm...(asm_func, arg)` functions trigger the High-priority interrupt (4 level) on the other CPU. The given assembler function (asm_func) will run on the other CPU in the context of the interrupt. This assembler function will be called by the CALLX0 command therefore it must be written in assembler.

:cpp:func:`esp_ipc_isr_asm_call` - triggers the Hi-priority interrupt on a particular core to execute a given function. The CPU which called this function will be blocked until the other CPU begins execution of the given function.

:cpp:func:`esp_ipc_isr_asm_call_blocking` is similar but will block the calling CPU until the other CPU has completed the execution of the given function.

:cpp:func:`esp_ipc_isr_stall_other_cpu` stalls the other CPU and the calling CPU disables interrupts with level 3 and lower. To finish stalling the other CPU call :cpp:func:`esp_ipc_isr_release_other_cpu`. The stalled CPU disables interrupts with level 5 and lower.

Functions executed by Hi-priority IPC must be functions of type `void func(void *arg)`. Examples of a assembler function see in :idf_file:`components/esp_system/port/arch/xtensa/esp_ipc_isr_routines.S`, :idf_file:`components/esp_system/test/test_ipc_isr.S` and below. In the asm function, you can use only a few registers as they were saved in the interrupt handler before calling this function, their use is safe. The registers:`a2` as `void *arg`, a3 and a4 are free for use.

Some feature:

- The asm function should be placed in the IRAM memory and aligned on a memory address multiple of 4.
- As the asm function is run in the context of the High-priority interrupt, a C function can no be called because the windows spill is disabled.
- Use only a2, a3 and a4 registers in the asm function (to workaround it see esp_complex_asm_func).
- A CPU, that called these APIs `esp_ipc_isr_asm...(asm_func, arg)`, disables interrupts with level 3 and lower.
- A CPU, where the asm function is executed, disables interrupts with level 5 and lower.
- You do not need to take care about handling of the Hi-priority interrupt.


.. code-block:: asm

    /* esp_test_ipc_isr_get_cycle_count_other_cpu(void *arg) */
    // this function puts CCOUNT of the other CPU in the arg.
    // use only a2, a3 and a4 regs here.
    .section    .iram1, "ax"
    .align      4
    .global     esp_test_ipc_isr_get_cycle_count_other_cpu
    .type       esp_test_ipc_isr_get_cycle_count_other_cpu, @function
    // Args:
    // a2 - void* arg
    esp_test_ipc_isr_get_cycle_count_other_cpu:
    rsr.ccount a3
    s32i    a3, a2, 0
    ret


This number of registers available for use is sufficient for most simple cases. But if you need to run a more complex asm function, you can pass as an argument a pointer to a structure that can accept additional registers to a buffer to make them free to use. Remember to restore them before returning See the :example:`system/ipc/ipc_isr`.

API Reference
-------------

.. include-build-file:: inc/esp_ipc.inc
