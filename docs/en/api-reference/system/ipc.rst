Inter-Processor Call (IPC)
==========================

:link_to_translation:`zh_CN:[中文]`

.. note::

    IPC stands for an **"Inter-Processor Call"** and **NOT** "Inter-Process Communication" as found on other operating systems.

Overview
--------

Due to the dual core nature of the {IDF_TARGET_NAME}, there are some scenarios where a certain callback must be executed from a particular core such as:

- When allocating an ISR to an interrupt source of a particular core (applies to freeing a particular core's interrupt source as well)
- On particular chips (such as the ESP32), accessing memory that is exclusive to a particular core (such as RTC Fast Memory)
- Reading the registers/state of another core

The IPC (Inter-Processor Call) feature allows a particular core (the calling core) to trigger the execution of a callback function on another core (the target core). The IPC feature allows execution of a callback function on the target core in either a task context, or an interrupt context. Depending on the context that the callback function is executed in, different restrictions apply to the implementation of the callback function.

IPC in Task Context
-------------------

The IPC feature implements callback execution in a task context by creating an IPC task for each core during application startup. When the calling core needs to execute a callback on the target core, the callback will execute in the context of the target core's IPC task.

When using IPCs in a task context, users need to consider the following:

- IPC callbacks should ideally be simple and short. An IPC callback **must never block or yield**.
- The IPC tasks are created at the highest possible priority (i.e., ``configMAX_PRIORITIES - 1``).

    - If :ref:`CONFIG_ESP_IPC_USES_CALLERS_PRIORITY` is enabled, the target core's IPC task will be lowered to the current priority of the target core before executing the callback.
    - If :ref:`CONFIG_ESP_IPC_USES_CALLERS_PRIORITY` is disabled, the target core will always execute the callback at the highest possible priority.

- Depending on the complexity of the callback, users may need to configure the stack size of the IPC task via :ref:`CONFIG_ESP_IPC_TASK_STACK_SIZE`.
- The IPC feature is internally protected by a mutex. Therefore, simultaneous IPC calls from two or more calling core's are serialized on a first come first serve basis.

API Usage
^^^^^^^^^

Task Context IPC callbacks have the following restrictions:

- The callback must be of the :cpp:type:`esp_ipc_func_t` type.
- The callback **must never block or yield** as this will result in the target core's IPC task blocking or yielding.
- The callback must avoid changing any aspect of the IPC task's state, e.g., by calling ``vTaskPrioritySet(NULL, x)``.

The IPC feature offers the API listed below to execute a callback in a task context on a target core. The API allows the calling core to block until the callback's execution has completed, or return immediately once the callback's execution has started.

- :cpp:func:`esp_ipc_call` triggers an IPC call on the target core. This function will block until the target core's IPC task **begins** execution of the callback.
- :cpp:func:`esp_ipc_call_blocking` triggers an IPC on the target core. This function will block until the target core's IPC task **completes** execution of the callback.

IPC in Interrupt Context
------------------------

In some cases, we need to quickly obtain the state of another core such as in a core dump, GDB stub, various unit tests, and hardware errata workarounds. The IPC ISR feature implements callback execution from a High Priority Interrupt context by reserving a High Priority Interrupt on each core for IPC usage. When a calling core needs to execute a callback on the target core, the callback will execute in the context of the High Priority Interrupt of the target core.

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    For such scenarios, the IPC ISR feature supports execution of callbacks in a :doc:`High Priority Interrupt </api-guides/hlinterrupts>` context.

When using IPCs in High Priority Interrupt context, users need to consider the following:

.. list::

    :CONFIG_IDF_TARGET_ARCH_XTENSA: - Since the callback is executed in a High Priority Interrupt context, the callback must be written entirely in assembly. See the API Usage below for more details regarding writing assembly callbacks.
    - The priority of the reserved High Priority Interrupt is dependent on the :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` option.

When the callback executes, users need to consider the following:

.. list::

    - The calling core will disable interrupts of priority level 3 and lower.
    :CONFIG_IDF_TARGET_ARCH_XTENSA: - Although the priority of the reserved interrupt depends on :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL`, during the execution of IPC ISR callback, the target core will disable interrupts of priority level 5 and lower regardless of what :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` is set to.
    :CONFIG_IDF_TARGET_ARCH_RISCV: - Although the priority of the reserved interrupt depends on :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL`, during the execution of IPC ISR callback, the target core will disable all interrupts.

API Usage
^^^^^^^^^

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    High Priority Interrupt IPC callbacks have the following restrictions:

    - The callback must be of type :cpp:type:`esp_ipc_isr_func_t` but implemented entirely in assembly.
    - The callback is invoked via the ``CALLX0`` instruction with register windowing disabled, thus the callback:
        - must not call any register window related instructions, e.g., ``entry`` and ``retw``.
        - must not call other C functions as register windowing is disabled.
    - The callback should be placed in IRAM at a 4-byte aligned address.
    - On invocation of, or after returning from the callback, the registers ``a2, a3, a4`` are saved/restored automatically, thus can be used in the callback. The callback should **ONLY** use those registers.
        - ``a2`` contains the ``void *arg`` of the callback.
        - ``a3/a4`` are free to use as scratch registers.

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    High Priority Interrupt IPC callbacks must be of type :cpp:type:`esp_ipc_isr_func_t` and have the same restrictions as for regular interrupt handlers. The callback function can be written in C.

The IPC feature offers the API listed below to execute a callback in a High Priority Interrupt context:

- :cpp:func:`esp_ipc_isr_call` triggers an IPC call on the target core. This function will busy-wait until the target core **begins** execution of the callback.
- :cpp:func:`esp_ipc_isr_call_blocking` triggers an IPC call on the target core. This function will busy-wait until the target core **completes** execution of the callback.

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    The following code-blocks demonstrates a High Priority Interrupt IPC callback written in assembly that simply reads the target core's cycle count:

    .. code-block:: asm

        /* esp_test_ipc_isr_get_cycle_count_other_cpu(void *arg) */
        // this function reads CCOUNT of the target core and stores it in arg.
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
        esp_ipc_isr_call_blocking(esp_test_ipc_isr_get_cycle_count_other_cpu, (void *)cycle_count);

    .. note::

        The number of scratch registers available for use is sufficient for most simple use cases. But if your callback requires more scratch registers, ``void *arg`` can point to a buffer that is used as a register save area. The callback can then save and restore more registers. See the :example:`system/ipc/ipc_isr`.

    .. note::

        For more examples of High Priority Interrupt IPC callbacks, you can refer to :idf_file:`components/esp_system/port/arch/xtensa/esp_ipc_isr_routines.S` and :idf_file:`components/esp_system/test_apps/esp_system_unity_tests/main/port/arch/xtensa/test_ipc_isr.S`.

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    See :idf_file:`examples/system/ipc/ipc_isr/riscv/main/main.c` for an example of its use.

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    See :idf_file:`examples/system/ipc/ipc_isr/xtensa/main/main.c` for an example of its use.

The High Priority Interrupt IPC API also provides the following convenience functions that can stall/resume the target core. These APIs utilize the High Priority Interrupt IPC, but supply their own internal callbacks:

.. list::

    :CONFIG_IDF_TARGET_ARCH_RISCV: - :cpp:func:`esp_ipc_isr_stall_other_cpu` stalls the target core. The calling core disables interrupts of level 3 and lower, while the target core will busy-wait with all interrupts disabled. The target core will busy-wait until :cpp:func:`esp_ipc_isr_release_other_cpu` is called.
    :CONFIG_IDF_TARGET_ARCH_XTENSA: - :cpp:func:`esp_ipc_isr_stall_other_cpu` stalls the target core. The calling core disables interrupts of level 3 and lower while the target core will busy-wait with interrupts of level 5 and lower disabled. The target core will busy-wait until :cpp:func:`esp_ipc_isr_release_other_cpu` is called.
    - :cpp:func:`esp_ipc_isr_release_other_cpu` resumes the target core.

API Reference
-------------

.. include-build-file:: inc/esp_ipc.inc

.. include-build-file:: inc/esp_ipc_isr.inc
