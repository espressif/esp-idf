ESP-IDF FreeRTOS (SMP)
======================

.. note::
  This document assumes that the reader has a requisite understanding of Vanilla FreeRTOS (its features, behavior, and API usage). Refer to the `Vanilla FreeRTOS documentation <https://www.freertos.org/index.html>`_ for more details.

This document describes the API and behavioral differences between Vanilla FreeRTOS and ESP-IDF FreeRTOS that were made in order to support Symmetric Multiprocessing (SMP). This document is split into the following parts.

.. contents:: Contents
  :depth: 3


.. ---------------------------------------------------- Overview -------------------------------------------------------

Overview
--------

The original FreeRTOS (hereinafter referred to as Vanilla FreeRTOS) is a small and efficient Real Time Operating System supported on many single-core MCUs and SoCs. However, numerous ESP targets (such as the ESP32 and ESP32-S3) are capable of dual core symmetric multiprocessing (SMP). Therefore, the version of FreeRTOS used in ESP-IDF (hereinafter referred to as ESP-IDF FreeRTOS) is a modified version of Vanilla FreeRTOS v10.4.3. Theses modifications allow ESP-IDF FreeRTOS to utilize the dual core SMP capabilities of ESP SoCs.

.. only:: CONFIG_FREERTOS_UNICORE

  .. note::
    Some ESP targets (such as the ESP32-S2 and ESP32-C3) are single core SoCs. ESP-IDF applications built for these targets will be built with **ESP-IDF FreeRTOS instead of Vanilla FreeRTOS**. However, the builds for these single core targets will always have the :ref:`CONFIG_FREERTOS_UNICORE` configuration enabled. See :ref:`freertos-smp-single-core` for more details.

.. note::
  - For information regarding features that have been added to ESP-IDF FreeRTOS, see :doc:`ESP-IDF FreeRTOS Additions</api-reference/system/freertos_additions>`.
  - For a detailed ESP-IDF FreeRTOS API Reference, see :doc:`FreeRTOS API reference<../api-reference/system/freertos>`.


.. -------------------------------------------- Symmetric Multiprocessing ----------------------------------------------

Symmetric Multiprocessing
-------------------------

Basic Concepts
^^^^^^^^^^^^^^

SMP (Symmetric Multiprocessing) is a computing architecture where two or more identical CPUs (cores) are connected to a single shared main memory and controlled by a single operating system. In general, an SMP system...

- has multiple cores running independently. Each core has its own register file, interrupts, and interrupt handling.
- presents an identical view of memory to each core. Thus a piece of code that accesses a particular memory address will have the same effect regardless of which core it runs on.

The main advantages of an SMP system compared to single core or Asymmetric Multiprocessing systems are that...

- the presence of multiple CPUs allows for multiple hardware threads, thus increases overall processing throughput.
- having symmetric memory means that threads can switch cores during execution. This in general can lead to better CPU utilization.

Although an SMP system allows threads to switch cores, there are scenarios where a thread must/should only run on a particular core. Therefore, threads in an SMP systems will also have a core affinity that specifies which particular core the thread is allowed to run on.

- A thread that is pinned to a particular core will only be able to run on that core
- A thread that is unpinned will be allowed to switch between cores during execution instead of being pinned to a particular core.

SMP on an ESP Target
^^^^^^^^^^^^^^^^^^^^

ESP targets (such as the ESP32, ESP32-S3) are dual core SMP SoCs. These targets have the following hardware features that make them SMP capable:

- Two identical cores known as CPU0 (i.e., Protocol CPU or PRO_CPU) and CPU1 (i.e., Application CPU or APP_CPU). This  means that the execution of a piece of code is identical regardless of which core it runs on.
- Symmetric memory (with some small exceptions).

  - If multiple cores access the same memory address, their access will be serialized at the memory bus level.
  - True atomic access to the same memory address is achieved via an atomic compare-and-swap instruction provided by the ISA.

- Cross-core interrupts that allow one CPU to trigger and interrupt on another CPU. This allows cores to signal each other.

.. note::
  The "PRO_CPU" and "APP_CPU" aliases for CPU0 and CPU1 exist in ESP-IDF as they reflect how typical IDF applications will utilize the two CPUs. Typically, the tasks responsible for handling wireless networking (e.g., WiFi or Bluetooth) will be pinned to CPU0 (thus the name PRO_CPU), whereas the tasks handling the remainder of the application will be pinned to CPU1 (thus the name APP_CPU).


.. ------------------------------------------------------ Tasks --------------------------------------------------------

Tasks
-----

Creation
^^^^^^^^

Vanilla FreeRTOS provides the following functions to create a task:

- :cpp:func:`xTaskCreate` creates a task. The task's memory is dynamically allocated
- :cpp:func:`xTaskCreateStatic` creates a task. The task's memory is statically allocated (i.e., provided by the user)

However, in an SMP system, tasks need to be assigned a particular affinity. Therefore, ESP-IDF provides a ``PinnedToCore`` version of Vanilla FreeRTOS's task creation functions:

- :cpp:func:`xTaskCreatePinnedToCore` creates a task with a particular core affinity. The task's memory is dynamically allocated.
- :cpp:func:`xTaskCreateStaticPinnedToCore` creates a task with a particular core affinity. The task's memory is statically allocated (i.e., provided by the user)

The ``PinnedToCore`` versions of the task creation functions API differ from their vanilla counter parts by having an extra ``xCoreID`` parameter that is used to specify the created task's core affinity. The valid values for core affinity are:

-	``0`` which pins the created task to CPU0
-	``1`` which pins the created task to CPU1
-	``tskNO_AFFINITY`` which allows the task to be run on both CPUs

Note that ESP-IDF FreeRTOS still supports the vanilla versions of the task creation functions. However, they have been modified to simply call their ``PinnedToCore`` counterparts with ``tskNO_AFFINITY``.

.. note::
  ESP-IDF FreeRTOS also changes the units of ``ulStackDepth`` in the task creation functions. Task stack sizes in Vanilla FreeRTOS are specified in number of words, whereas in ESP-IDF FreeRTOS, the task stack sizes are specified in bytes.

Execution
^^^^^^^^^

The anatomy of a task in ESP-IDF FreeRTOS is the same as Vanilla FreeRTOS. More specifically, ESP-IDF FreeRTOS tasks:

- Can only be in one of following states: Running, Ready, Blocked, or Suspended.
- Task functions are typically implemented as an infinite loop
- Task functions should never return

Deletion
^^^^^^^^

Task deletion in Vanilla FreeRTOS is called via :cpp:func:`vTaskDelete`. The function allows deletion of another task or the currently running task (if the provided task handle is ``NULL``). The actual freeing of the task's memory is sometimes delegated to the idle task (if the task being deleted is the currently running task).

ESP-IDF FreeRTOS provides the same :cpp:func:`vTaskDelete` function. However, due to the dual core nature, there are some behavioral differences when calling :cpp:func:`vTaskDelete` in ESP-IDF FreeRTOS:

- When deleting a task that is pinned to the other core, that task's memory is always freed by the idle task of the other core (due to the need to clear FPU registers).
- When deleting a task that is currently running on the other core, a yield is triggered on the other core and the task's memory is freed by one of the idle tasks (depending on the task's core affinity)
- A deleted task's memory is freed immediately if...

  - The tasks is currently running on this core and is also pinned to this core
  - The task is not currently running and is not pinned to any core

Users should avoid calling :cpp:func:`vTaskDelete`  on a task that is currently running on the other core. This is due to the fact that it is difficult to know what the task currently running on the other core is executing, thus can lead to unpredictable behavior such as...

- Deleting a task that is holding a mutex
- Deleting a task that has yet to free memory it previously allocated

Where possible, users should design their application such that :cpp:func:`vTaskDelete` is only ever called on tasks in a known state. For example:

- Tasks self deleting (via ``vTaskDelete(NULL)``) when their execution is complete and have also cleaned up all resources used within the task.
- Tasks placing themselves in the suspend state (via :cpp:func:`vTaskSuspend`) before being deleted by another task.


.. --------------------------------------------------- Scheduling ------------------------------------------------------

SMP Scheduler
-------------

The Vanilla FreeRTOS scheduler is best described as a **Fixed Priority Preemptive scheduler with Time Slicing** meaning that:

- Each tasks is given a constant priority upon creation. The scheduler executes highest priority ready state task
- The scheduler can switch execution to another task without the cooperation of the currently running task
- The scheduler will periodically switch execution between ready state tasks of the same priority (in a round robin fashion). Time slicing is governed by a tick interrupt.

The ESP-IDF FreeRTOS scheduler supports the same scheduling features (i.e., Fixed Priority, Preemption, and Time Slicing) albeit with some small behavioral differences.

Fixed Priority
^^^^^^^^^^^^^^

In Vanilla FreeRTOS, when scheduler selects a new task to run, it will always select the current highest priority ready state task. In ESP-IDF FreeRTOS, each core will independently schedule tasks to run. When a particular core selects a task, the core will select the highest priority ready state task that can be run by the core. A task can be run by the core if:

- The task has a compatible affinity (i.e., is either pinned to that core or is unpinned)
- The task is not currently being run by another core

However, users should not assume that the two highest priority ready state tasks are always run by the scheduler as a task's core affinity must also be accounted for. For example, given the following tasks:

- Task A of priority 10 pinned to CPU0
- Task B of priority 9 pinned to CPU0
- Task C of priority 8 pinned to CPU1

The resulting schedule will have Task A running on CPU0 and Task C running on CPU1. Task B is not run even though it is the second highest priority task.

Preemption
^^^^^^^^^^

In Vanilla FreeRTOS, the scheduler can preempt the currently running task if a higher priority task becomes ready to execute. Likewise in ESP-IDF FreeRTOS, each core can be individually preempted by the scheduler if the scheduler determines that a higher priority task can run on that core.

However, there are some instances where a higher priority task that becomes ready can be run on multiple cores. In this case, the scheduler will only preempt one core. The scheduler always gives preference to the current core when multiple cores can be preempted. In other words, if the higher priority ready task is unpinned and has a higher priority than the current priority of both cores, the scheduler will always choose to preempt the current core. For example, given the following tasks:

- Task A of priority 8 currently running on CPU0
- Task B of priority 9 currently running on CPU1
- Task C of priority 10 that is unpinned and was unblocked by Task B

The resulting schedule will have Task A running on CPU0 and Task C preempting Task B given that the scheduler always gives preference to the current core.

Time Slicing
^^^^^^^^^^^^

The Vanilla FreeRTOS scheduler implements time slicing meaning that if current highest ready priority contains multiple ready tasks, the scheduler will switch between those tasks periodically in a round robin fashion. 

However, in ESP-IDF FreeRTOS, it is not possible to implement perfect Round Robin time slicing due to the fact that a particular task may not be able to run on a particular core due to the following reasons:

- The task is pinned to the another core.
- For unpinned tasks, the task is already being run by another core.

Therefore, when a core searches the ready state task list for a task to run, the core may need to skip over a few tasks in the same priority list or drop to a lower priority in order to find a ready state task that the core can run.

The ESP-IDF FreeRTOS scheduler implements a Best Effort Round Robin time slicing for ready state tasks of the same priority by ensuring that tasks that have been selected to run will be placed at the back of the list, thus giving unselected tasks a higher priority on the next scheduling iteration (i.e., the next tick interrupt or yield)

The following example demonstrates the Best Effort Round Robin time slicing in action. Assume that:

- There are four ready state tasks of the same priority ``AX, B0, C1, D1`` where:
  - The priority is the current highest priority with ready state tasks
  - The first character represents the task's names (i.e., ``A, B, C, D``)
  - And the second character represents the tasks core pinning (and ``X`` means unpinned)
- The task list is always searched from the head

.. code-block:: none

    --------------------------------------------------------------------------------

    1. Starting state. None of the ready state tasks have been selected to run

    Head [ AX , B0 , C1 , D0 ] Tail

    --------------------------------------------------------------------------------

    2. Core 0 has tick interrupt and searches for a task to run.
      Task A is selected and is moved to the back of the list

    Core0--|
    Head [ AX , B0 , C1 , D0 ] Tail

                          0
    Head [ B0 , C1 , D0 , AX ] Tail

    --------------------------------------------------------------------------------

    3. Core 1 has a tick interrupt and searches for a task to run.
      Task B cannot be run due to incompatible affinity, so core 1 skips to Task C.
      Task C is selected and is moved to the back of the list

    Core1-------|         0
    Head [ B0 , C1 , D0 , AX ] Tail

                     0    1
    Head [ B0 , D0 , AX , C1 ] Tail

    --------------------------------------------------------------------------------

    4. Core 0 has another tick interrupt and searches for a task to run.
      Task B is selected and moved to the back of the list


    Core0--|              1
    Head [ B0 , D0 , AX , C1 ] Tail

                     1    0
    Head [ D0 , AX , C1 , B0 ] Tail

    --------------------------------------------------------------------------------

    5. Core 1 has another tick and searches for a task to run.
      Task D cannot be run due to incompatible affinity, so core 1 skips to Task A
      Task A is selected and moved to the back of the list

    Core1-------|         0
    Head [ D0 , AX , C1 , B0 ] Tail

                     0    1
    Head [ D0 , C1 , B0 , AX ] Tail


The implications to users regarding the Best Effort Round Robin time slicing:

- Users cannot expect multiple ready state tasks of the same priority to run sequentially (as is the case in Vanilla FreeRTOS). As demonstrated in the example above, a core may need to skip over tasks.
- However, given enough ticks, a task will eventually be given some processing time.
- If a core cannot find a task runnable task at the highest ready state priority, it will drop to a lower priority to search for tasks.
- To achieve ideal round robin time slicing, users should ensure that all tasks of a particular priority are pinned to the same core.

Tick Interrupts
^^^^^^^^^^^^^^^

Vanilla FreeRTOS requires that a periodic tick interrupt occurs. The tick interrupt is responsible for:

- Incrementing the scheduler's tick count
- Unblocking any blocked tasks that have timed out
- Checking if time slicing is required (i.e., triggering a context switch)
- Executing the application tick hook

In ESP-IDF FreeRTOS, each core will receive a periodic interrupt and independently run the tick interrupt. The tick interrupts on each core are of the same period but can be out of phase. Furthermore, the tick interrupt responsibilities listed above are not run by all cores:

- CPU0 will execute all of the tick interrupt responsibilities listed above
- CPU1 will only check for time slicing and execute the application tick hook

.. note::
  CPU0 is solely responsible for keeping time in ESP-IDF FreeRTOS. Therefore anything that prevents CPU0 from incrementing the tick count (such as suspending the scheduler on CPU0) will cause the entire schedulers time keeping to lag behind.

Idle Tasks
^^^^^^^^^^

Vanilla FreeRTOS will implicitly create an idle task of priority 0 when the scheduler is started. The idle task runs when no other task is ready to run, and it has the following responsibilities:

- Freeing the memory of deleted tasks
- Executing the application idle hook

In ESP-IDF FreeRTOS, a separate pinned idle task is created for each core. The idle tasks on each core have the same responsibilities as their vanilla counterparts.

Scheduler Suspension
^^^^^^^^^^^^^^^^^^^^

Vanilla FreeRTOS allows the scheduler to be suspended/resumed by calling :cpp:func:`vTaskSuspendAll` and :cpp:func:`xTaskResumeAll` respectively. While the scheduler is suspended:

- Task switching is disabled but interrupts are left enabled.
- Calling any blocking/yielding function is forbidden, and time slicing is disabled.
- The tick count is frozen (but the tick interrupt will still occur to execute the application tick hook)

On scheduler resumption, :cpp:func:`xTaskResumeAll` will catch up all of the lost ticks and unblock any timed out tasks.

In ESP-IDF FreeRTOS, suspending the scheduler across multiple cores is not possible. Therefore when :cpp:func:`vTaskSuspendAll` is called:

- Task switching is disabled only on the current core but interrupts for the current core are left enabled
- Calling any blocking/yielding function on the current core is forbidden. Time slicing is disabled on the current core.
- If suspending on CPU0, the tick count is frozen. The tick interrupt will still occur to execute the application tick hook.

When resuming the scheduler on CPU0, :cpp:func:`xTaskResumeAll` will catch up all of the lost ticks and unblock any timed out tasks.

.. warning::
  Given that scheduler suspension on ESP-IDF FreeRTOS will only suspend scheduling on a particular core, scheduler suspension is **NOT** a valid method ensuring mutual exclusion between tasks when accessing shared data. Users should use proper locking primitives such as mutexes or spinlocks if they require mutual exclusion.

Disabling Interrupts
^^^^^^^^^^^^^^^^^^^^

Vanilla FreeRTOS allows interrupts to be disabled and enabled by calling :c:macro:`taskDISABLE_INTERRUPTS` and :c:macro:`taskENABLE_INTERRUPTS` respectively.

ESP-IDF FreeRTOS provides the same API, however interrupts will only disabled or enabled on the current core.

.. warning::
  Disabling interrupts is a valid method of achieve mutual exclusion in Vanilla FreeRTOS (and single core systems in general). However, in an SMP system, disabling interrupts  is **NOT** a valid method ensuring mutual exclusion. Refer to Critical Sections for more details.

Startup and Termination
^^^^^^^^^^^^^^^^^^^^^^^

ESP-IDF FreeRTOS **does not** require users to call :cpp:func:`vTaskStartScheduler` to start the scheduler. The startup flow of an ESP-IDF application will already call this automatically. The entry point for user code is a user defined ``void app_main(void)`` function. For more details regarding the startup of ESP-IDF FreeRTOS applications, see :ref:`freertos-applications`.

ESP-IDF FreeRTOS **does not** support scheduler termination. Calling :cpp:func:`vTaskEndScheduler` will simply cause the application to abort.


.. ------------------------------------------------ Critical Sections --------------------------------------------------

Critical Sections
-----------------

API Changes
^^^^^^^^^^^

Vanilla FreeRTOS implements critical sections by disabling interrupts, This prevents preemptive context switches and the servicing of ISRs during a critical section. Thus a task/ISR that enters a critical section is guaranteed to be the sole entity to access a shared resource. Critical sections in Vanilla FreeRTOS have the following API:

- ``taskENTER_CRITICAL()`` enters a critical section by disabling interrupts
- ``taskEXIT_CRITICAL()`` exits a critical section by reenabling interrupts
- ``taskENTER_CRITICAL_FROM_ISR()`` enters a critical section from an ISR by disabling interrupt nesting
- ``taskEXIT_CRITICAL_FROM_ISR()`` exits a critical section from an ISR by reenabling interrupt nesting

However, in an SMP system, merely disabling interrupts does not constitute a critical section as the presence of other cores means that a shared resource can still be concurrently accessed. Therefore, critical sections in ESP-IDF FreeRTOS are implemented using spinlocks. To accommodate the spinlocks, the ESP-IDF FreeRTOS critical section APIs contain an additional spinlock parameter as shown below:

- Spinlocks are of ``portMUX_TYPE`` (**not to be confused to FreeRTOS mutexes**)
- ``taskENTER_CRITICAL(&mux)`` enters a critical from a task context
- ``taskEXIT_CRITICAL(&mux)`` exits a critical section from a task context
- ``taskENTER_CRITICAL_ISR(&mux)`` enters a critical section from an interrupt context
- ``taskEXIT_CRITICAL_ISR(&mux)`` exits a critical section from an interrupt context

.. note::
  The critical section API can be called recursively (i.e., nested critical sections). Entering a critical section multiple times recursively is valid so long as the critical section is exited the same number of times it was entered. However, given that critical sections can target different spinlocks, users should take care to avoid dead locking when entering critical sections recursively.

Implementation
^^^^^^^^^^^^^^

In ESP-IDF FreeRTOS, the process of a particular core entering and exiting a critical section is as follows:

- For ``taskENTER_CRITICAL(&mux)`` (or ``taskENTER_CRITICAL_ISR(&mux)``)

  #. The core disables its interrupts (or interrupt nesting) up to ``configMAX_SYSCALL_INTERRUPT_PRIORITY``
  #. The core then spins on the spinlock using an atomic compare-and-set instruction until it acquires the lock. A lock is acquired when the core is able to set the lock's owner value to the core's ID.
  #. Once the spinlock is acquired, the function returns. The remainder of the critical section runs with interrupts (or interrupt nesting) disabled.

- For ``taskEXIT_CRITICAL(&mux)`` (or ``taskEXIT_CRITICAL_ISR(&mux)``)

  #. The core releases the spinlock by clearing the spinlock's owner value
  #. The core re-enables interrupts (or interrupt nesting)

Restrictions and Considerations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Given that interrupts (or interrupt nesting) are disabled during a critical section, there are multiple restrictions regarding what can be done within a critical sections. During a critical section, users should keep the following restrictions and considerations in mind:

- Critical sections should be as kept as short as possible

  - The longer the critical section lasts, the longer a pending interrupt can be delayed.
  - A typical critical section should only access a few data structures and/or hardware registers
  - If possible, defer as much processing and/or event handling to the outside of critical sections.

- FreeRTOS API should not be called from within a critical section
- Users should never call any blocking or yielding functions within a critical section


.. ------------------------------------------------------ Misc ---------------------------------------------------------

Misc
----

Floating Point Usage
^^^^^^^^^^^^^^^^^^^^

Usually, when a context switch occurs:

- the current state of a CPU's registers are saved to the stack of task being switch out
- the previously saved state of the CPU's registers are loaded from the stack of the task being switched in

However, ESP-IDF FreeRTOS implements Lazy Context Switching for the FPU (Floating Point Unit) registers of a CPU. In other words, when a context switch occurs on a particular core (e.g., CPU0), the state of the core's FPU registers are not immediately saved to the stack of the task getting switched out (e.g., Task A). The FPU's registers are left untouched until:

- A different task (e.g., Task B) runs on the same core and uses the FPU. This will trigger an exception that will save the FPU registers to Task A's stack.
- Task A get's scheduled to the same core and continues execution. Saving and restoring the FPU's registers is not necessary in this case.

However, given that tasks can be unpinned thus can be scheduled on different cores (e.g., Task A switches to CPU1), it is unfeasible to copy and restore the FPU's registers across cores. Therefore, when a task utilizes the FPU (by using a ``float`` type in its call flow), ESP-IDF FreeRTOS will automatically pin the task to the current core it is running on. This ensures that all tasks that uses the FPU are always pinned to a particular core.

Furthermore, ESP-IDF FreeRTOS by default does not support the usage of the FPU within an interrupt context given that the FPU's register state is tied to a particular task.

.. only: esp32

  .. note::
    Users that require the use of the ``float`` type in an ISR routine should refer to the :ref:`CONFIG_FREERTOS_FPU_IN_ISR` configuration option.

.. note::
  ESP targets that contain an FPU do not support hardware acceleration for double precision floating point arithmetic (``double``). Instead ``double`` is implemented via software hence the behavioral restrictions regarding the ``float`` type do not apply to ``double``. Note that due to the lack of hardware acceleration, ``double`` operations may consume significantly more CPU time in comparison to ``float``.


.. -------------------------------------------------- Single Core  -----------------------------------------------------

.. _freertos-smp-single-core:

ESP-IDF FreeRTOS Single Core
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Although ESP-IDF FreeRTOS is an SMP scheduler, some ESP targets are single core (such as the ESP32-S2 and ESP32-C3). When building ESP-IDF applications for these targets, ESP-IDF FreeRTOS is still used but the number of cores will be set to `1` (i.e., the :ref:`CONFIG_FREERTOS_UNICORE` will always be enabled for single core targets).

For multicore targets (such as the ESP32 and ESP32-S3), :ref:`CONFIG_FREERTOS_UNICORE` can also be set. This will result in ESP-IDF FreeRTOS only running on CPU0, and all other cores will be inactive.

.. note::
  Users should bear in mind that enabling :ref:`CONFIG_FREERTOS_UNICORE` **is NOT equivalent to running Vanilla FreeRTOS**. The additional API of ESP-IDF FreeRTOS can still be called, and the behavior changes of ESP-IDF FreeRTOS will incur a small amount of overhead even when compiled for only a single core.
