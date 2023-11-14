# ESP-IDF Changes

This document is used to track all changes made the to FreeRTOS V10.5.1 source code when adding dual core SMP support or IDF additional features.

Todo: Add these to ESP-IDF docs once v10.5.1 becomes default kernel (IDF-8203)

## License Headers

- Added `SPDX-FileCopyrightText` and `SPDX-FileContributor` tags to all files to pass ESP-IDF pre-commit checks.
- Changed kernel version tag to `FreeRTOS Kernel V10.5.1 (ESP-IDF SMP modified)` in all files to indicate files are different from upstream V10.5.1.

## Omitted Features

- Removed `croutine.c` and `croutine.h`

## Changes from Upstream Main Branch not Included in v10.5.1

- Added ...GetStaticBuffers functions that were upstreamed but not included in v10.5.1

## Kernel SMP Changes

List of changes to the Vanilla FreeRTOS V10.5.1 kernel in order to support dual-core SMP

### Scheduling Behavior Changes

- The kernel now executes two tasks concurrently
- The kernel now creates two IDLE tasks (pinned to each core)
- Tasks can be pinned to either core, or have no affinity (can run on both cores)
- Each core receives a tick interrupt, but only core 0 increments the tick count and unblocks timed out tasks
  - Core 0 calls `xTaskIncrementTick()`
  - Core 1 calls `xTaskIncrementTickOtherCores()`
- Each core independently calls `vTaskSwitchContext()` to pick the highest priority task it can currently run
  - In single-core scheduling algorithm `taskSELECT_HIGHEST_PRIORITY_TASK()` unchanged
  - In SMP, `prvSelectHighestPriorityTaskSMP()` is called. This will select the highest priority ready state task that...
    - Has a compatible core affinity
    - Is not being run by another core
- Each core can suspend scheduling independently (i.e., `vTaskSuspendAll()`)

### Configuration

Following configurations have been added

- Added `configNUMBER_OF_CORES` to specify the number of cores to build. Can be `1` for vanilla, or `2` for SMP, error otherwise
- Disable `configUSE_PORT_OPTIMISED_TASK_SELECTION` for SMP

### Data Structure Changes (`tasks.c`)

The following data fields have been expanded to have `configNUMBER_OF_CORES` copies:

- `pxCurrentTCBs`: Each core now has its own currently running task
- `xPendingReadyList`: Each core has its own list to pend ready tasks if the scheduler is suspended on the core
- `xYieldPending`: Each core has its own flag to track whether it has a pending yield
- `xIdleTaskHandle`: Each core now has its own idle task
- `uxSchedulerSuspended`: Each core can independently suspend scheduling on its core
- `ulTaskSwitchedInTime`: Each core tracks its own "task switched in" time

Their access is now indexed by a `xCoreID` if in SMP, or set to `0` in single core.

The following data structures have been added:

- `TCB_t.xCoreID`: All tasks now store their core affinity in a TCB member. Always set to 0 in single-core

### API Additions

The following APIs have been added to support SMP

- `xTaskCreatePinnedToCore()` and `xTaskCreateStaticPinnedToCore()` to create tasks with a core affinity
  - In single-core, core affinity is ignored. Same behavior as `xTaskCreate()`
- `xTaskGetCoreID()` to get a task's affinity
- Add `ForCore()` versions of the following API
    - `xTaskGetIdleTaskHandleForCore()`
    - `xTaskGetCurrentTaskHandleForCore()`
    - `ulTaskGetIdleRunTimeCounterForCore()`

### API Modifications

Added the following macros that abstract away single-core and SMP differences:

- `taskYIELD_CORE()` triggers a particular core to yield
- `taskIS_YIELD_REQUIRED()`/`taskIS_YIELD_REQUIRED_USING_PRIORITY()` check if current core requires a yield after a task is unblocked
- `taskIS_AFFINITY_COMPATIBLE()` check if a task has compatible affinity
- `taskIS_CURRENTLY_RUNNING()`/`taskIS_CURRENTLY_RUNNING_ON_CORE()` checks if a task is running on either core
- `taskCAN_BE_SCHEDULED()` checks if an unblocked task can be scheduled on any core
- `taskIS_SCHEDULER_SUSPENDED()` checks if the scheduler on the current core is suspended
- `taskSELECT_HIGHEST_PRIORITY_TASK()` selects the highest priority task to execute for the current core
- `prvGetTCBFromHandle()` updated in SMP to call `xTaskGetCurrentTaskHandle()` when the handle is `NULL`. Done so for thread safety (in case the current task switches cores at the same time).

The following functions were modified to accommodate SMP behavior:

- `prvInitialiseNewTask()`
  - Added `xCoreID` argument to pin task on creation
  - For single-core, `xCoreID` is hard coded to `0`
- `prvAddNewTaskToReadyList()`
  - Checks if new task can be scheduled on core 1
- `vTaskDelete()`
  - Checks if the deleted task is currently running on the other core.
  - If so, sends a yield to the other core.
- `vTaskPrioritySet()`
  - Checks if the task is currently running on the both cores, and yields the appropriate core if so
- `vTaskSuspend()`
  - Checks if the task is currently running on the other core, and yields the other core if so.
- `prvTaskIsTaskSuspended()`
  - Checks the `xPendingReadyList` of both cores to see if a task is suspended
- `xTaskResumeAll()`
  - Limit catching up of tick counts to core 0 (given only core 0 calls `xTaskIncrementTick()`)
- `xTaskIncrementTick()`
  - Limited to core 0
- `vTaskSwitchContext()`
  - Switches context for current core
- `xTaskRemoveFromEventList()`
  - Created SMP copy of the function
    - Checks if `pxEventList` has already been emptied by the other core before removing
    - Checks if task can be scheduled on both cores, adds it to the appropriate core's pending list if it can't be scheduled.
- `vTaskRemoveFromUnorderedEventList()`
  - In SMP, check if the task can be scheduled before adding it to the appropriate list. Whereas in single-core, the scheduler is always suspended thus the unblocked task always goes onto the pending ready list.
- `eTaskConfirmSleepModeStatus()`
  - Updated logic to determine whether sleep is possible in SMP by checking the status of both cores.
- `prvCheckTasksWaitingTermination()`
  - Updated logic so that we don't delete tasks on `xTasksWaitingTermination` which are still currently running on the other core.
- `xTaskGetCurrentTaskHandle()`
  - In SMP, the function will now disables interrupts to ensure that the calling task does not switch cores while fetching the current core's TCB.
- `xTaskGetSchedulerState()`
  - In SMP, the function now disables interrupts to ensure that the calling task does not switch cores while checking its own copy of `uxSchedulerSuspended`.
- `prvAddCurrentTaskToDelayedList()`
  - Added extra check to see if current blocking task has already been deleted by the other core.

### Critical Section Changes

- Granular Locks: The following objects are now given their own spinlocks
  - Kernel objects (i.e., `tasks.c`): `xKernelLock`
  - Each queue: `xQueueLock`
  - Queue Registry: `xQueueRegistryLock`
  - Each event group: `xEventGroupLock`
  - Each stream buffer: `xStreamBufferLock`
  - All timers: `xTimerLock`
- Critical sections now target the appropriate spinlocks
- Added missing critical sections for SMP (see `..._SMP_ONLY()` critical section calls)
- Queues no longer use queue locks (see `queueUSE_LOCKS`)
  - Queues now just use critical sections and skips queue locking
  - Queue functions can now execute within a single critical section block

## Single Core Differences

List of differences between Vanilla FreeRTOS V10.5.1 and building the dual-core SMP kernel with `congigNUMBER_OF_CORES == 1`.

- `prvAddNewTaskToReadyList()`
  - Extended critical section so that SMP can check for yields while still inside critical section
- `vTaskStepTick()`
  - Extended critical section so that SMP can access `xTickCount` while still inside critical section

## Header File & Doxygen Changes

List of changes made to Vanilla FreeRTOS V10.5.1 header files to allow for building in ESP-IDF documentation build system.

- Removed leading header name line (e.g., `xxx.h`) in doxygen comment blocks. For example:

  ```c
  /**
   * xxx.h
   *
   * Documentation from some func
   */
  void some_func(void);
  ```

- Removed leading `@code{c}` blocks in containing redundant function prototypes. For example:

  ```c
  /**
   * @code{c}
   * void some_func(int var_a, int var_b);
   * @endcode
   *
   * Documentation from some func
   */
  void some_func(int var_a, int var_b);
  ```

- Added `/** @cond !DOC_EXCLUDE_HEADER_SECTION */` and `/** @endcond */` labels to exclude various doxygen sections from being included into documentation builds. These excluded sections include:
    - In doxygen blocks that describe multiple related set of functions/macros, only the function/macro that matches the doxygen blocks parameters is included. For example:
      ```c
      /**
       * Description that covers both some_func() and some_func_extra()
       *
       * @param var_a var_a description
       * @param var_b var_b description
       */
      /** @cond !DOC_EXCLUDE_HEADER_SECTION */
      #define some_func(var_a)                #define some_func_generic(var_a, NULL)
      /** @endcond */
      #define some_func_extra(var_a, var_b)   #define some_func_generic(var_a, var_b)
      ```
    - In functions/macros that are not meant to be directly called by users (i.e., internal), such as the various `Generic` variants of functions

- Some types/functions/macros are manually documented, thus are documented with regular comment blocks (i.e., `/* */`) instead of doxygen comment blocks (i.e., `/** */`). Some of these blocks are changed into doxygen blocks.

## Changes backported to IDF-FreeRTOS Kernel from upstream kernel beyond v10.5.1 LTS release

### tasks.c

- Backported a change where the IDLE tasks are created with the core ID as a suffix in the task name.
