Inter-Processor Call
====================

.. note::
    The IPC is an **Inter-Processor Call** and **NOT Inter-Process Communication** as found on other operating systems.

Overview
--------

Due to the dual core nature of the {IDF_TARGET_NAME}, there are instances where a certain callback must be run in the context of a particular CPU such as:

- When allocating an ISR to an interrupt source of a particular CPU (applies to freeing a particular CPU's interrupt source as well).
- On particular chips (such as the ESP32), accessing memory that is exclusive to a particular CPU (such as RTC Fast Memory).
- Reading the registers/state of another CPU.

The IPC (Inter-Processor Call) feature allows a particular CPU (the calling CPU) to trigger the execution of a callback function on another CPU (the target CPU). The IPC feature allows execution of a callback function on the target CPU in either a task context, or a High Priority Interrupt context (see :doc:`/api-guides/hlinterrupts` for more details). Depending on the context that the callback function is executed in, different restrictions apply to the implementation of the callback function.

IPC in Task Context
-------------------

The IPC feature implements callback execution in a task context by creating an IPC task for each CPU during application startup. When the calling CPU needs to execute a callback on the target CPU, the callback will execute in the context of the target CPU's IPC task.

When using IPCs in a task context, users need to consider the following:

- IPC callbacks should ideally be simple and short. **An IPC callback should avoid attempting to block or yield**. 
- The IPC tasks are created at the highest possible priority (i.e., ``configMAX_PRIORITIES - 1``) thus the callback should also run at that priority as a result. However, :ref:`CONFIG_ESP_IPC_USES_CALLERS_PRIORITY` is enabled by default which will temporarily lower the priority of the target CPU's IPC task to the calling CPU before executing the callback.
- Depending on the complexity of the callback, users may need to configure the stack size of the IPC task via :ref:`CONFIG_ESP_IPC_TASK_STACK_SIZE`.
- The IPC feature is internally protected by a mutex. Therefore, simultaneous IPC calls from two or more calling CPUs will be handled on a first come first serve basis.

API Usage
^^^^^^^^^

Task Context IPC callbacks have the following restrictions:

- The callback must be of type ``void func(void *arg)``
- The callback should avoid attempting to block or yield as this will result in the target CPU's IPC task blocking or yielding.
- The callback must avoid changing any aspect of the IPC task (e.g., by calling ``vTaskPrioritySet(NULL, x)``).

The IPC feature offers the API listed below to execute a callback in a task context on a target CPU. The API allows the calling CPU to block until the callback's execution has completed, or return immediately once the callback's execution has started.

- :cpp:func:`esp_ipc_call` will trigger an IPC call on the target CPU. This function will block until the target CPU's IPC task **begins** execution of the callback.
- :cpp:func:`esp_ipc_call_blocking` will trigger an IPC on the target CPU. This function will block until the target CPU's IPC task **completes** execution of the callback.

IPC in ISR Context
------------------

In some cases, we need to quickly obtain the state of another CPU such as in a core dump, GDB stub, various unit tests, and DPORT workaround. For such scenarios, the IPC feature supports execution of callbacks in a :doc:`High Priority Interrupt </api-guides/hlinterrupts>` context. The IPC feature implements the High Priority Interrupt context by reserving a High Priority Interrupt on each CPU for IPC usage. When a calling CPU needs to execute a callback on the target CPU, the callback will execute in the context of the High Priority Interrupt of the target CPU.

When using IPCs in High Priority Interrupt context, users need to consider the following:

- Since the callback is executed in a High Priority Interrupt context, the callback must be written entirely in assembly. See the API Usage below for more details regarding writing assembly callbacks.
- The priority of the reserved High Priority Interrupt is dependent on the :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` option
- When the callback executes:
    - The calling CPU will disable interrupts of level 3 and lower
    - Although the priority of the reserved interrupt depends on :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL`, during the execution IPC ISR callback, the target CPU will disable interrupts of level 5 and lower regardless of what :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` is set to.

API Usage
^^^^^^^^^

High Priority Interrupt IPC callbacks have the following restrictions:

- The callback must be of type ``void func(void *arg)`` but implemented entirely in assembly
- The callback is invoked via the ``CALLX0`` instruction with register windowing disabled, thus the callback:
    - Must not call any register window related instructions (e.g., ``entry`` and ``retw``).
    - Must not call other C functions as register windowing is disabled
- The callback should be placed in IRAM at a 4-byte aligned address
- (On invocation of/after returning from) the callback, the registers ``a2, a3, a4`` are (saved/restored) automatically thus can be used in the callback. The callback should **ONLY** use those registers.
    - ``a2`` will contain the ``void *arg`` of the callback
    - ``a3/a4`` are free to use as scratch registers

The IPC feature offers the API listed below to execute a callback in a High Priority Interrupt context. 

- :cpp:func:`esp_ipc_isr_asm_call` will trigger an IPC call on the target CPU. This function will busy-wait until the target CPU begins execution of the callback.
- :cpp:func:`esp_ipc_isr_asm_call_blocking` will trigger an IPC call on the target CPU. This function will busy-wait until the target CPU completes execution of the callback.

The following code-blocks demonstrates a High Priority Interrupt IPC callback written in assembly that simply reads the target CPU's cycle count.

.. code-block:: asm

    /* esp_test_ipc_isr_get_cycle_count_other_cpu(void *arg) */
    // this function reads CCOUNT of the target CPU and stores it in arg.
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

.. code-block:: c

    unit32_t cycle_count;
    esp_ipc_isr_asm_call_blocking(esp_test_ipc_isr_get_cycle_count_other_cpu, (void *)cycle_count);

.. note::
    The number of scratch registers available for use is sufficient for most simple use cases. But if your callback requires more scratch registers, ``void *arg`` can point to a buffer that is used as a register save area. The callback can then save and restore more registers. See the :example:`system/ipc/ipc_isr`.

.. note::
    For more examples of High Priority Interrupt IPC callbacks, see :idf_file:`components/esp_system/port/arch/xtensa/esp_ipc_isr_routines.S` and :`components/esp_system/test/test_ipc_isr.S`

The High Priority Interrupt IPC API also provides the following convenience functions that can stall/resume the target CPU. These API utilize the High Priority Interrupt IPC, but supply their own internal callbacks:

- :cpp:func:`esp_ipc_isr_stall_other_cpu` stalls the target CPU. The calling CPU disables interrupts of level 3 and lower while the target CPU will busy-wait with interrupts of level 5 and lower disabled. The target CPU will busy-wait until :cpp:func:`esp_ipc_isr_release_other_cpu` is called.
- :cpp:func:`esp_ipc_isr_release_other_cpu` resumes the target CPU.

API Reference
-------------

.. include-build-file:: inc/esp_ipc.inc

.. include-build-file:: inc/esp_ipc_isr.inc
