FreeRTOS (IDF)
==============

:link_to_translation:`zh_CN:[中文]`

This document provides information regarding the dual-core SMP implementation of FreeRTOS inside ESP-IDF. This document is split into the following sections:

.. contents:: Sections
  :depth: 2

.. ---------------------------------------------------- Overview -------------------------------------------------------

Overview
--------

The original FreeRTOS (hereinafter referred to as **Vanilla FreeRTOS**) is a compact and efficient real-time operating system supported on numerous single-core MCUs and SoCs. However, to support dual-core ESP targets, such as ESP32, ESP32-S3, and ESP32-P4, ESP-IDF provides a unique implementation of FreeRTOS with dual-core symmetric multiprocessing (SMP) capabilities (hereinafter referred to as **IDF FreeRTOS**).

IDF FreeRTOS source code is based on Vanilla FreeRTOS v10.5.1 but contains significant modifications to both kernel behavior and API in order to support dual-core SMP. However, IDF FreeRTOS can also be configured for single-core by enabling the :ref:`CONFIG_FREERTOS_UNICORE` option (see :ref:`freertos-idf-single-core` for more details).

.. note::

    This document assumes that the reader has a requisite understanding of Vanilla FreeRTOS, i.e., its features, behavior, and API usage. Refer to the `Vanilla FreeRTOS documentation <https://www.freertos.org/index.html>`_ for more details.

.. -------------------------------------------- Symmetric Multiprocessing ----------------------------------------------

Symmetric Multiprocessing
-------------------------

Basic Concepts
^^^^^^^^^^^^^^

Symmetric multiprocessing is a computing architecture where two or more identical CPU cores are connected to a single shared main memory and controlled by a single operating system. In general, an SMP system:

- has multiple cores running independently. Each core has its own register file, interrupts, and interrupt handling.
- presents an identical view of memory to each core. Thus, a piece of code that accesses a particular memory address has the same effect regardless of which core it runs on.

The main advantages of an SMP system compared to single-core or asymmetric multiprocessing systems are that:

- the presence of multiple cores allows for multiple hardware threads, thus increasing overall processing throughput.
- having symmetric memory means that threads can switch cores during execution. This, in general, can lead to better CPU utilization.

Although an SMP system allows threads to switch cores, there are scenarios where a thread must/should only run on a particular core. Therefore, threads in an SMP system also have a core affinity that specifies which particular core the thread is allowed to run on.

- A thread that is pinned to a particular core is only able to run on that core.
- A thread that is unpinned will be allowed to switch between cores during execution instead of being pinned to a particular core.

SMP on an ESP Target
^^^^^^^^^^^^^^^^^^^^

ESP targets such as ESP32, ESP32-S3, and ESP32-P4 are dual-core SMP SoCs. These targets have the following hardware features that make them SMP-capable:

- Two identical cores are known as Core 0 and Core 1. This means that the execution of a piece of code is identical regardless of which core it runs on.
- Symmetric memory (with some small exceptions).

  - If multiple cores access the same memory address simultaneously, their access will be serialized by the memory bus.
  - True atomic access to the same memory address is achieved via an atomic compare-and-swap instruction provided by the ISA.

- Cross-core interrupts that allow one core to trigger an interrupt on the other core. This allows cores to signal events to each other (such as requesting a context switch on the other core).

.. note::

    Within ESP-IDF, Core 0 and Core 1 are sometimes referred to as ``PRO_CPU`` and ``APP_CPU`` respectively. The aliases exist in ESP-IDF as they reflect how typical ESP-IDF applications utilize the two cores. Typically, the tasks responsible for handling protocol related processing such as Wi-Fi or Bluetooth are pinned to Core 0 (thus the name ``PRO_CPU``), where as the tasks handling the remainder of the application are pinned to Core 1, (thus the name ``APP_CPU``).

.. ------------------------------------------------------ Tasks --------------------------------------------------------

Tasks
-----

Creation
^^^^^^^^

Vanilla FreeRTOS provides the following functions to create a task:

- :cpp:func:`xTaskCreate` creates a task. The task's memory is dynamically allocated.
- :cpp:func:`xTaskCreateStatic` creates a task. The task's memory is statically allocated, i.e., provided by the user.

However, in an SMP system, tasks need to be assigned a particular affinity. Therefore, ESP-IDF provides a ``...PinnedToCore()`` version of Vanilla FreeRTOS's task creation functions:

- :cpp:func:`xTaskCreatePinnedToCore` creates a task with a particular core affinity. The task's memory is dynamically allocated.
- :cpp:func:`xTaskCreateStaticPinnedToCore` creates a task with a particular core affinity. The task's memory is statically allocated, i.e., provided by the user.

The ``...PinnedToCore()`` versions of the task creation function API differ from their vanilla counterparts by having an extra ``xCoreID`` parameter that is used to specify the created task's core affinity. The valid values for core affinity are:

- ``0``, which pins the created task to Core 0
- ``1``, which pins the created task to Core 1
- ``tskNO_AFFINITY``, which allows the task to be run on both cores

Note that IDF FreeRTOS still supports the vanilla versions of the task creation functions. However, these standard functions have been modified to essentially invoke their respective ``...PinnedToCore()`` counterparts while setting the core affinity to ``tskNO_AFFINITY``.

.. note::

    IDF FreeRTOS also changes the units of ``ulStackDepth`` in the task creation functions. Task stack sizes in Vanilla FreeRTOS are specified in a number of words, whereas in IDF FreeRTOS, the task stack sizes are specified in bytes.

Execution
^^^^^^^^^

The anatomy of a task in IDF FreeRTOS is the same as in Vanilla FreeRTOS. More specifically, IDF FreeRTOS tasks:

- Can only be in one of the following states: Running, Ready, Blocked, or Suspended.
- Task functions are typically implemented as an infinite loop.
- Task functions should never return.

Deletion
^^^^^^^^

Task deletion in Vanilla FreeRTOS is called via :cpp:func:`vTaskDelete`. The function allows deletion of another task or the currently running task if the provided task handle is ``NULL``. The actual freeing of the task's memory is sometimes delegated to the idle task if the task being deleted is the currently running task.

IDF FreeRTOS provides the same :cpp:func:`vTaskDelete` function. However, due to the dual-core nature, there are some behavioral differences when calling :cpp:func:`vTaskDelete` in IDF FreeRTOS:

- When deleting a task that is currently running on the other core, a yield is triggered on the other core, and the task's memory is freed by one of the idle tasks.
- A deleted task's memory is freed immediately if it is not running on either core.

Please avoid deleting a task that is running on another core as it is difficult to determine what the task is performing, which may lead to unpredictable behavior such as:

- Deleting a task that is holding a mutex.
- Deleting a task that has yet to free memory it previously allocated.

Where possible, please design your own application so that when calling :cpp:func:`vTaskDelete`, the deleted task is in a known state. For example:

- Tasks self-deleting via ``vTaskDelete(NULL)`` when their execution is complete and have also cleaned up all resources used within the task.
- Tasks placing themselves in the suspend state via :cpp:func:`vTaskSuspend` before being deleted by another task.


.. --------------------------------------------------- Scheduling ------------------------------------------------------

SMP Scheduler
-------------

The Vanilla FreeRTOS scheduler is best described as a **fixed priority preemptive scheduler with time slicing** meaning that:

- Each task is given a constant priority upon creation. The scheduler executes the highest priority ready-state task.
- The scheduler can switch execution to another task without the cooperation of the currently running task.
- The scheduler periodically switches execution between ready-state tasks of the same priority in a round-robin fashion. Time slicing is governed by a tick interrupt.

The IDF FreeRTOS scheduler supports the same scheduling features, i.e., Fixed Priority, Preemption, and Time Slicing, albeit with some small behavioral differences.

Fixed Priority
^^^^^^^^^^^^^^

In Vanilla FreeRTOS, when the scheduler selects a new task to run, it always selects the current highest priority ready-state task. In IDF FreeRTOS, each core independently schedules tasks to run. When a particular core selects a task, the core will select the highest priority ready-state task that can be run by the core. A task can be run by the core if:

- The task has a compatible affinity, i.e., is either pinned to that core or is unpinned.
- The task is not currently being run by another core.

However, please do not assume that the two highest priority ready-state tasks are always run by the scheduler, as a task's core affinity must also be accounted for. For example, given the following tasks:

- Task A of priority 10 pinned to Core 0
- Task B of priority 9 pinned to Core 0
- Task C of priority 8 pinned to Core 1

The resulting schedule will have Task A running on Core 0 and Task C running on Core 1. Task B is not run even though it is the second-highest priority task.

Preemption
^^^^^^^^^^

In Vanilla FreeRTOS, the scheduler can preempt the currently running task if a higher priority task becomes ready to execute. Likewise in IDF FreeRTOS, each core can be individually preempted by the scheduler if the scheduler determines that a higher-priority task can run on that core.

However, there are some instances where a higher-priority task that becomes ready can be run on multiple cores. In this case, the scheduler only preempts one core. The scheduler always gives preference to the current core when multiple cores can be preempted. In other words, if the higher priority ready task is unpinned and has a higher priority than the current priority of both cores, the scheduler will always choose to preempt the current core. For example, given the following tasks:

- Task A of priority 8 currently running on Core 0
- Task B of priority 9 currently running on Core 1
- Task C of priority 10 that is unpinned and was unblocked by Task B

The resulting schedule will have Task A running on Core 0 and Task C preempting Task B given that the scheduler always gives preference to the current core.

Time Slicing
^^^^^^^^^^^^

The Vanilla FreeRTOS scheduler implements time slicing, which means that if the current highest ready priority contains multiple ready tasks, the scheduler will switch between those tasks periodically in a round-robin fashion.

However, in IDF FreeRTOS, it is not possible to implement perfect Round Robin time slicing due to the fact that a particular task may not be able to run on a particular core due to the following reasons:

- The task is pinned to another core.
- For unpinned tasks, the task is already being run by another core.

Therefore, when a core searches the ready-state task list for a task to run, the core may need to skip over a few tasks in the same priority list or drop to a lower priority in order to find a ready-state task that the core can run.

The IDF FreeRTOS scheduler implements a Best Effort Round Robin time slicing for ready-state tasks of the same priority by ensuring that tasks that have been selected to run are placed at the back of the list, thus giving unselected tasks a higher priority on the next scheduling iteration (i.e., the next tick interrupt or yield).

The following example demonstrates the Best Effort Round Robin time slicing in action. Assume that:

- There are four ready-state tasks of the same priority ``AX``, ``B0``, ``C1``, and ``D1`` where:

  - The priority is the current highest priority with ready-state .
  - The first character represents the task's name, i.e., ``A``, ``B``, ``C``, ``D``.
  - The second character represents the task's core pinning, and ``X`` means unpinned.

- The task list is always searched from the head.

1. Starting state. None of the ready-state tasks have been selected to run.

    .. code-block:: none

        Head [ AX , B0 , C1 , D0 ] Tail

2. Core 0 has a tick interrupt and searches for a task to run. Task A is selected and moved to the back of the list.

    .. code-block:: none

        Core 0 ─┐
                ▼
        Head [ AX , B0 , C1 , D0 ] Tail

                              [0]
        Head [ B0 , C1 , D0 , AX ] Tail

3. Core 1 has a tick interrupt and searches for a task to run. Task B cannot be run due to incompatible affinity, so Core 1 skips to Task C. Task C is selected and moved to the back of the list.

    .. code-block:: none

        Core 1 ──────┐
                     ▼        [0]
        Head [ B0 , C1 , D0 , AX ] Tail

                         [0]  [1]
        Head [ B0 , D0 , AX , C1 ] Tail

4. Core 0 has another tick interrupt and searches for a task to run. Task B is selected and moved to the back of the list.

    .. code-block:: none

        Core 0 ─┐
                ▼             [1]
        Head [ B0 , D0 , AX , C1 ] Tail

                         [1]  [0]
        Head [ D0 , AX , C1 , B0 ] Tail

5. Core 1 has another tick and searches for a task to run. Task D cannot be run due to incompatible affinity, so Core 1 skips to Task A. Task A is selected and moved to the back of the list.

    .. code-block:: none

        Core 1 ──────┐
                     ▼        [0]
        Head [ D0 , AX , C1 , B0 ] Tail

                         [0]  [1]
        Head [ D0 , C1 , B0 , AX ] Tail

The implications to users regarding the Best Effort Round Robin time slicing:

- Users cannot expect multiple ready-state tasks of the same priority to run sequentially as is the case in Vanilla FreeRTOS. As demonstrated in the example above, a core may need to skip over tasks.
- However, given enough ticks, a task will eventually be given some processing time.
- If a core cannot find a task runnable task at the highest ready-state priority, it will drop to a lower priority to search for tasks.
- To achieve ideal round-robin time slicing, users should ensure that all tasks of a particular priority are pinned to the same core.

Tick Interrupts
^^^^^^^^^^^^^^^

Vanilla FreeRTOS requires that a periodic tick interrupt occurs. The tick interrupt is responsible for:

- Incrementing the scheduler's tick count
- Unblocking any blocked tasks that have timed out
- Checking if time slicing is required, i.e., triggering a context switch
- Executing the application tick hook

In IDF FreeRTOS, each core receives a periodic interrupt and independently runs the tick interrupt. The tick interrupts on each core are of the same period but can be out of phase. However, the tick responsibilities listed above are not run by all cores:

- Core 0 executes all of the tick interrupt responsibilities listed above
- Core 1 only checks for time slicing and executes the application tick hook

.. note::

    Core 0 is solely responsible for keeping time in IDF FreeRTOS. Therefore, anything that prevents Core 0 from incrementing the tick count, such as suspending the scheduler on Core 0, will cause the entire scheduler's timekeeping to lag behind.

Idle Tasks
^^^^^^^^^^

Vanilla FreeRTOS will implicitly create an idle task of priority 0 when the scheduler is started. The idle task runs when no other task is ready to run, and it has the following responsibilities:

- Freeing the memory of deleted tasks
- Executing the application idle hook

In IDF FreeRTOS, a separate pinned idle task is created for each core. The idle tasks on each core have the same responsibilities as their vanilla counterparts.

Scheduler Suspension
^^^^^^^^^^^^^^^^^^^^

Vanilla FreeRTOS allows the scheduler to be suspended/resumed by calling :cpp:func:`vTaskSuspendAll` and :cpp:func:`xTaskResumeAll` respectively. While the scheduler is suspended:

- Task switching is disabled but interrupts are left enabled.
- Calling any blocking/yielding function is forbidden, and time slicing is disabled.
- The tick count is frozen, but the tick interrupt still occurs to execute the application tick hook.

On scheduler resumption, :cpp:func:`xTaskResumeAll` catches up all of the lost ticks and unblock any timed-out tasks.

In IDF FreeRTOS, suspending the scheduler across multiple cores is not possible. Therefore when :cpp:func:`vTaskSuspendAll` is called on a particular core (e.g., core A):

- Task switching is disabled only on core A but interrupts for core A are left enabled.
- Calling any blocking/yielding function on core A is forbidden. Time slicing is disabled on core A.
- If an interrupt on core A unblocks any tasks, tasks with affinity to core A will go into core A's own pending ready task list. Unpinned tasks or tasks with affinity to other cores can be scheduled on cores with the scheduler running.
- If the scheduler is suspended on all cores, tasks unblocked by an interrupt will be directed to the pending ready task lists of their pinned cores. For unpinned tasks, they will be placed in the pending ready list of the core where the interrupt occurred.
- If core A is on Core 0, the tick count is frozen, and a pended tick count is incremented instead. However, the tick interrupt will still occur in order to execute the application tick hook.

When :cpp:func:`xTaskResumeAll` is called on a particular core (e.g., core A):

- Any tasks added to core A's pending ready task list will be resumed.
- If core A is Core 0, the pended tick count is unwound to catch up with the lost ticks.

.. warning::

    Given that scheduler suspension on IDF FreeRTOS only suspends scheduling on a particular core, scheduler suspension is **NOT** a valid method of ensuring mutual exclusion between tasks when accessing shared data. Users should use proper locking primitives such as mutexes or spinlocks if they require mutual exclusion.

.. ------------------------------------------------ Critical Sections --------------------------------------------------

Critical Sections
-----------------

Disabling Interrupts
^^^^^^^^^^^^^^^^^^^^

Vanilla FreeRTOS allows interrupts to be disabled and enabled by calling :c:macro:`taskDISABLE_INTERRUPTS` and :c:macro:`taskENABLE_INTERRUPTS` respectively. IDF FreeRTOS provides the same API. However, interrupts are only disabled or enabled on the current core.

Disabling interrupts is a valid method of achieving mutual exclusion in Vanilla FreeRTOS (and single-core systems in general). **However, in an SMP system, disabling interrupts is not a valid method of ensuring mutual exclusion**. Critical sections that utilize a spinlock should be used instead.

API Changes
^^^^^^^^^^^

Vanilla FreeRTOS implements critical sections by disabling interrupts, which prevents preemptive context switches and the servicing of ISRs during a critical section. Thus a task/ISR that enters a critical section is guaranteed to be the sole entity to access a shared resource. Critical sections in Vanilla FreeRTOS have the following API:

- ``taskENTER_CRITICAL()`` enters a critical section by disabling interrupts
- ``taskEXIT_CRITICAL()`` exits a critical section by reenabling interrupts
- ``taskENTER_CRITICAL_FROM_ISR()`` enters a critical section from an ISR by disabling interrupt nesting
- ``taskEXIT_CRITICAL_FROM_ISR()`` exits a critical section from an ISR by reenabling interrupt nesting

However, in an SMP system, merely disabling interrupts does not constitute a critical section as the presence of other cores means that a shared resource can still be concurrently accessed. Therefore, critical sections in IDF FreeRTOS are implemented using spinlocks. To accommodate the spinlocks, the IDF FreeRTOS critical section APIs contain an additional spinlock parameter as shown below:

- Spinlocks are of ``portMUX_TYPE`` (**not to be confused to FreeRTOS mutexes**)
- ``taskENTER_CRITICAL(&spinlock)`` enters a critical from a task context
- ``taskEXIT_CRITICAL(&spinlock)`` exits a critical section from a task context
- ``taskENTER_CRITICAL_ISR(&spinlock)`` enters a critical section from an interrupt context
- ``taskEXIT_CRITICAL_ISR(&spinlock)`` exits a critical section from an interrupt context

.. note::

    The critical section API can be called recursively, i.e., nested critical sections. Entering a critical section multiple times recursively is valid so long as the critical section is exited the same number of times it was entered. However, given that critical sections can target different spinlocks, users should take care to avoid deadlocking when entering critical sections recursively.

Spinlocks can be allocated statically or dynamically. As such, macros are provided for both static and dynamic initialization of spinlocks, as demonstrated by the following code snippets.

- Allocating a static spinlock and initializing it using ``portMUX_INITIALIZER_UNLOCKED``:

  .. code:: c

      // Statically allocate and initialize the spinlock
      static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;

      void some_function(void)
      {
          taskENTER_CRITICAL(&my_spinlock);
          // We are now in a critical section
          taskEXIT_CRITICAL(&my_spinlock);
      }

- Allocating a dynamic spinlock and initializing it using ``portMUX_INITIALIZE()``:

  .. code:: c

      // Allocate the spinlock dynamically
      portMUX_TYPE *my_spinlock = malloc(sizeof(portMUX_TYPE));
      // Initialize the spinlock dynamically
      portMUX_INITIALIZE(my_spinlock);

      ...

      taskENTER_CRITICAL(my_spinlock);
      // Access the resource
      taskEXIT_CRITICAL(my_spinlock);

Implementation
^^^^^^^^^^^^^^

In IDF FreeRTOS, the process of a particular core entering and exiting a critical section is as follows:

- For ``taskENTER_CRITICAL(&spinlock)`` or ``taskENTER_CRITICAL_ISR(&spinlock)``

  #. The core disables its interrupts or interrupt nesting up to ``configMAX_SYSCALL_INTERRUPT_PRIORITY``.
  #. The core then spins on the spinlock using an atomic compare-and-set instruction until it acquires the lock. A lock is acquired when the core is able to set the lock's owner value to the core's ID.
  #. Once the spinlock is acquired, the function returns. The remainder of the critical section runs with interrupts or interrupt nesting disabled.

- For ``taskEXIT_CRITICAL(&spinlock)`` or ``taskEXIT_CRITICAL_ISR(&spinlock)``

  #. The core releases the spinlock by clearing the spinlock's owner value.
  #. The core re-enables interrupts or interrupt nesting.

Restrictions and Considerations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Given that interrupts (or interrupt nesting) are disabled during a critical section, there are multiple restrictions regarding what can be done within critical sections. During a critical section, users should keep the following restrictions and considerations in mind:

- Critical sections should be kept as short as possible

  - The longer the critical section lasts, the longer a pending interrupt can be delayed.
  - A typical critical section should only access a few data structures and/or hardware registers.
  - If possible, defer as much processing and/or event handling to the outside of critical sections.

- FreeRTOS API should not be called from within a critical section
- Users should never call any blocking or yielding functions within a critical section


.. ------------------------------------------------------ Misc ---------------------------------------------------------

Misc
----

.. only:: SOC_CPU_HAS_FPU

    Floating Point Usage
    ^^^^^^^^^^^^^^^^^^^^

    Usually, when a context switch occurs:

    - the current state of a core's registers are saved to the stack of the task being switched out
    - the previously saved state of the core's registers is loaded from the stack of the task being switched in

    However, IDF FreeRTOS implements Lazy Context Switching for the Floating Point Unit (FPU) registers of a core. In other words, when a context switch occurs on a particular core (e.g., Core 0), the state of the core's FPU registers is not immediately saved to the stack of the task getting switched out (e.g., Task A). The FPU registers are left untouched until:

    - A different task (e.g., Task B) runs on the same core and uses FPU. This will trigger an exception that saves the FPU registers to Task A's stack.
    - Task A gets scheduled to the same core and continues execution. Saving and restoring the FPU registers is not necessary in this case.

    However, given that tasks can be unpinned and thus can be scheduled on different cores (e.g., Task A switches to Core 1), it is unfeasible to copy and restore the FPU registers across cores. Therefore, when a task utilizes FPU by using a ``float`` type in its call flow, IDF FreeRTOS will automatically pin the task to the current core it is running on. This ensures that all tasks that use FPU are always pinned to a particular core.

    Furthermore, IDF FreeRTOS by default does not support the usage of FPU within an interrupt context given that the FPU register state is tied to a particular task.

    .. only:: esp32

        .. note::

            Users that require the use of the ``float`` type in an ISR routine should refer to the :ref:`CONFIG_FREERTOS_FPU_IN_ISR` configuration option.

    .. note::

        ESP targets that contain an FPU do not support hardware acceleration for double precision floating point arithmetic (``double``). Instead, ``double`` is implemented via software, hence the behavioral restrictions regarding the ``float`` type do not apply to ``double``. Note that due to the lack of hardware acceleration, ``double`` operations may consume significantly more CPU time in comparison to ``float``.


.. -------------------------------------------------- Single Core  -----------------------------------------------------

.. _freertos-idf-single-core:

Single-Core Mode
----------------

Although IDF FreeRTOS is modified for dual-core SMP, IDF FreeRTOS can also be built for single-core by enabling the :ref:`CONFIG_FREERTOS_UNICORE` option.

For single-core targets (such as ESP32-S2 and ESP32-C3), the :ref:`CONFIG_FREERTOS_UNICORE` option is always enabled. For multi-core targets (such as ESP32 and ESP32-S3), :ref:`CONFIG_FREERTOS_UNICORE` can also be set, but will result in the application only running Core 0.

When building in single-core mode, IDF FreeRTOS is designed to be identical to Vanilla FreeRTOS, thus all aforementioned SMP changes to kernel behavior are removed. As a result, building IDF FreeRTOS in single-core mode has the following characteristics:

- All operations performed by the kernel inside critical sections are now deterministic (i.e., no walking of linked lists inside critical sections).
- Vanilla FreeRTOS scheduling algorithm is restored (including perfect Round Robin time slicing).
- All SMP specific data is removed from single-core builds.

SMP APIs can still be called in single-core mode. These APIs remain exposed to allow source code to be built for single-core and multi-core, without needing to call a different set of APIs. However, SMP APIs will not exhibit any SMP behavior in single-core mode, thus becoming equivalent to their single-core counterparts. For example:

- any ``...ForCore(..., BaseType_t xCoreID)`` SMP API will only accept ``0`` as a valid value for ``xCoreID``.
- ``...PinnedToCore()`` task creation APIs will simply ignore the ``xCoreID`` core affinity argument.
- Critical section APIs will still require a spinlock argument, but no spinlock will be taken and critical sections revert to simply disabling/enabling interrupts.


.. ------------------------------------------------- API References ----------------------------------------------------

API Reference
-------------

This section introduces FreeRTOS types, functions, and macros. It is automatically generated from FreeRTOS header files.

Task API
^^^^^^^^

.. include-build-file:: inc/task.inc

Queue API
^^^^^^^^^

.. include-build-file:: inc/queue.inc

Semaphore API
^^^^^^^^^^^^^

.. include-build-file:: inc/semphr.inc

Timer API
^^^^^^^^^

.. include-build-file:: inc/timers.inc

Event Group API
^^^^^^^^^^^^^^^

.. include-build-file:: inc/event_groups.inc

Stream Buffer API
^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/stream_buffer.inc

Message Buffer API
^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/message_buffer.inc
