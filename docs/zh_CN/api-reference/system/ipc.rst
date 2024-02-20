处理器间调用 (IPC)
====================

:link_to_translation:`en:[English]`

.. note::

    IPC 指 **处理器间调用** (Inter-Processor Call)，而不是其他操作系统中所指的 **进程间通信** (Inter-Process Communication)。

概述
--------

由于 {IDF_TARGET_NAME} 的双核特性，在某些情况下，某个回调必须在特定的内核上下文中运行。例如：

- 为特定内核的中断源分配 ISR，或释放特定内核的中断源时
- 在特定芯片（如 ESP32）上访问某个内核独有的内存，如 RTC Fast Memory 时
- 读取另一个内核的寄存器或状态时

IPC 功能允许一个特定的内核（下文称“调用内核”）触发另一个内核（下文称“目标内核”）的回调函数执行，并允许目标内核在任务上下文或中断上下文中执行回调函数。在不同的上下文中，回调函数的实现限制有所不同。

任务上下文中的 IPC
-------------------

在应用程序启动时，IPC 功能为每个内核创建一个 IPC 任务，从而在任务上下文中执行回调。当调用内核需要在目标内核中执行回调时，回调将在目标内核的 IPC 任务的上下文中执行。

在任务上下文中使用 IPC 功能时，需考虑以下几点：

- IPC 回调应该尽可能简短。 **IPC 回调决不能阻塞或让出**。
- IPC 任务是以尽可能高的优先级创建的（即 ``configMAX_PRIORITIES - 1``）。

    - 如果启用了 :ref:`CONFIG_ESP_IPC_USES_CALLERS_PRIORITY`，执行回调前会降低目标内核的 IPC 任务优先级，使其等于调用内核的优先级。
    - 如果禁用了 :ref:`CONFIG_ESP_IPC_USES_CALLERS_PRIORITY`，目标内核将始终以尽可能高的优先级执行回调。

- 如果回调较为复杂，用户可能需要通过 :ref:`CONFIG_ESP_IPC_TASK_STACK_SIZE` 来配置 IPC 任务的堆栈大小。
- IPC 功能受内部互斥锁保护。因此，如果同时收到来自两个或多个调用内核的 IPC 请求，将按照“先到先得”的原则按顺序处理。

API 用法
^^^^^^^^^

任务上下文中的 IPC 回调具有以下限制：

- 回调类型必须是 :cpp:type:`esp_ipc_func_t`。
- 回调 **决不能阻塞或让出**，以免导致目标内核的 IPC 任务阻塞或让出。
- 回调必须避免改变 IPC 任务的任何状态，例如，不能调用 ``vTaskPrioritySet(NULL, x)``。

IPC 功能提供了以下 API，用于在目标内核的任务上下文中执行回调。这两个 API 允许调用内核在回调执行完成前处于阻塞，或者在回调开始执行后立即返回。

- :cpp:func:`esp_ipc_call` 会在目标内核上触发一个 IPC 调用。在目标内核的 IPC 任务 **开始** 执行回调前，此函数会一直处于阻塞状态。
- :cpp:func:`esp_ipc_call_blocking` 会在目标内核上触发一个 IPC。在目标内核的 IPC 任务 **完成** 回调执行前，此函数会一直处于阻塞状态。

中断上下文中的 IPC
------------------------

在某些情况下，我们需要快速获取另一个内核的状态，如在核心转储、GDB Stub、各种单元测试和绕过硬件错误的过程中。IPC ISR 功能会在每个内核上保留一个高优先级中断供 IPC 使用，从而在高优先级中断上下文中执行回调。当调用内核需要在目标内核上执行回调时，该回调将在目标内核的高优先级中断的上下文中执行。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    在这种情况下，IPC ISR 功能支持在 :doc:`高优先级中断 </api-guides/hlinterrupts>` 上下文中执行回调。

在高优先级中断上下文中使用 IPC 时，需要考虑以下几点：

.. list::

    :CONFIG_IDF_TARGET_ARCH_XTENSA: - 由于回调是在高优先级中断上下文中执行的，因此，回调必须完全用汇编语言编写。如需了解更多关于用汇编语言编写回调的内容，请参阅下文的 API 使用介绍。
    - 保留的高优先级中断的优先级取决于 :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` 选项。

当回调执行时，需考虑以下几点：

.. list::

    - 调用内核会禁用 3 级及以下优先级的中断。
    :CONFIG_IDF_TARGET_ARCH_XTENSA: - 虽然保留中断的优先级取决于 :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL`，但是在执行 IPC ISR 回调期间，无论 :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL` 如何设置，目标内核都会禁用 5 级及以下优先级的中断。
    :CONFIG_IDF_TARGET_ARCH_RISCV: - 虽然保留中断的优先级取决于 :ref:`CONFIG_ESP_SYSTEM_CHECK_INT_LEVEL`，但是在执行 IPC ISR 回调期间，目标内核会禁用所有的中断。

API 用法
^^^^^^^^^

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    高优先级中断的 IPC 回调具有以下限制：

    - 回调必须是 :cpp:type:`esp_ipc_isr_func_t` 类型，但必须完全用汇编语言实现。
    - 回调通过 ``CALLX0`` 指令调用，同时禁用寄存器窗口，因此，该回调：
        - 不得调用任何与寄存器窗口相关的指令，例如 ``entry`` 和 ``retw``。
        - 由于禁用了寄存器窗口，不得调用其他 C 函数。
    - 回调应放在 4 字节对齐的 IRAM 地址上。
    - 在调用或返回回调后，寄存器 ``a2``，``a3``，和 ``a4`` 会被自动保存或恢复，从而在回调中使用。回调 **只能使用这些寄存器**。
        - ``a2`` 包含回调的 ``void *arg``。
        - ``a3/a4`` 可以作为临时寄存器使用。

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    高优先级中断 IPC 回调函数的类型必须是 :cpp:type:`esp_ipc_isr_func_t`，其限制条件与常规中断处理程序相同。回调函数可以用 C 语言编写。

IPC 功能提供了下列 API，以在高优先级中断的上下文中执行回调：

- :cpp:func:`esp_ipc_isr_call` 能够在目标内核上触发一个 IPC 调用。在目标内核 **开始** 执行回调前，此函数将一直处于忙等待。
- :cpp:func:`esp_ipc_isr_call_blocking` 能够在目标内核上触发一个 IPC 调用。在目标内核 **完成** 回调执行前，此函数将一直处于忙等待。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    以下示例代码用汇编语言编写了一个高优先级中断 IPC 回调，该回调的作用为读取目标内核的周期计数：

    .. code-block:: asm

        /* esp_test_ipc_isr_get_cycle_count_other_cpu(void *arg) */
        // 此函数读取目标内核的 CCOUNT，并将其存储到 arg 中。
        // 此处仅使用 a2、a3 和 a4 寄存器。
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

        对于大多数简单用例，可用的临时寄存器数量是足够的。如果需要在回调函数中使用更多的临时寄存器，可以用 ``void *arg`` 指向一个用作寄存器保存区域的 buffer，使回调函数保存或恢复更多的寄存器。前往 :example:`system/ipc/ipc_isr` 查看示例。

    .. note::

        如需查看更多高优先级中断 IPC 回调的示例，请前往 :idf_file:`components/esp_system/port/arch/xtensa/esp_ipc_isr_routines.S` 和 :idf_file:`components/esp_system/test_apps/esp_system_unity_tests/main/port/arch/xtensa/test_ipc_isr.S`。

.. only:: CONFIG_IDF_TARGET_ARCH_RISCV

    请前往 :idf_file:`examples/system/ipc/ipc_isr/riscv/main/main.c` 查看使用示例。

.. only:: CONFIG_IDF_TARGET_ARCH_XTENSA

    请前往 :idf_file:`examples/system/ipc/ipc_isr/xtensa/main/main.c` 查看使用示例。

高优先级中断 IPC API 还提供了以下便利函数，这些函数可以暂停或恢复目标内核的执行。这些 API 利用高优先级中断 IPC，但同时提供了自己的内部回调函数：

.. list::

    :CONFIG_IDF_TARGET_ARCH_RISCV: - :cpp:func:`esp_ipc_isr_stall_other_cpu`：暂停目标内核。调用内核禁用 3 级及以下级别的中断，而目标内核将在所有中断被禁用的情况下进入忙等待。在调用 :cpp:func:`esp_ipc_isr_release_other_cpu` 前，目标内核会保持忙等待。
    :CONFIG_IDF_TARGET_ARCH_XTENSA: - :cpp:func:`esp_ipc_isr_stall_other_cpu`：暂停目标内核。调用内核禁用 3 级及以下级别的中断，而目标内核将在 5 级及以下的中断被禁用的情况下进入忙等待。在调用 :cpp:func:`esp_ipc_isr_release_other_cpu` 前，目标内核会保持忙等待。
    - :cpp:func:`esp_ipc_isr_release_other_cpu`：恢复目标内核。

API 参考
-------------

.. include-build-file:: inc/esp_ipc.inc

.. include-build-file:: inc/esp_ipc_isr.inc
