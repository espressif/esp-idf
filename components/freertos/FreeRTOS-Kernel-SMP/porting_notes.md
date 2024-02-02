# Overview

This document outlines some notes regarding the Amazon AMP FreeRTOS port in ESP-IDF

# Terminology

The following terms will be used in this document to avoid confusion between the different FreeRTOS versions currently in ESP-IDF

- SMP FreeRTOS: An SMP capable release of the FreeRTOS kernel described [here](https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/smp)
- IDF FreeRTOS: The version of FreeRTOS used in mainline ESP-IDF that contained custom modifications to support SMP features specific to the ESP chips.

# Organization

This directory contains a copy of upstream [v11.0.1](https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/V11.0.1) FreeRTOS which is SMP capable.

- IDF FreeRTOS remains in  `components/freertos/FreeRTOS-Kernel`
- SMP FreeRTOS is entirely contained in `components/freertos/FreeRTOS-Kernel-SMP`
  - `port.c` and `portmacro.h` were mostly re-written from scratch
  - Some changes were made to SMP FreeRTOS `tasks.c` in order to be compatible with IDF.
    - All of these changes are wrapped in `#ifdef ESP_PLATFORM`
    - These additions will be removed after the compatibility issues are resolved in IDF.
- SMP FreeRTOS is only compiled if `CONFIG_FREERTOS_SMP` is set in `menuconfig`
- All changes made to the rest of ESP-IDF to support SMP FreeRTOS are wrapped in `#ifdef CONFIG_FREERTOS_SMP`. Thus SMP FreeRTOS changes should have no effect on mainline ESP-IDF if `CONFIG_FREERTOS_SMP` is disabled.

# Behavioral Differences and Porting

This section covers

- The major feature/behavior differences between SMP FreeRTOS vs IDF FreeRTOS
- The changes made to the port or SMP FreeRTOS sources to remedy these differences
- Future Todos

## Task Core Affinity

IDF FreeRTOS:

- Tasks can either be pinned to a core or completely unpinned

SMP FreeRTOS:

- Tasks in SMP FreeRTOS use an affinity mask thus can run on any combination of cores

Changes Made:

- Upstreamed `xTaskCreate...AffinitySet()` to allow creation of tasks with affinity (instead of requiring a separate call to set the affinity).
- Wrapped the IDF `xTaskCreate...PinnedToCore()` functions to call `xTaskCreate...AffinitySet()`

## Scheduler Suspension

IDF FreeRTOS:

- Each core had its own suspendable scheduler (i.e., calling `vTaskSuspendAll()` on one core has no effect on the other)
- A core that calls `vTaskSuspendAll()` will only disable task switching on that core.

SMP FreeRTOS:

- `vTaskSuspendAll()` is global (no cores can switch tasks once the scheduler is disabled)
- While the scheduler is suspend, any other core that attempts to block, call FreeRTOS API, or also suspend the scheduler will result that the other core spinning with interrupts disabled (due to task lock contention)

Changes Made:

- Replaced all calls of `vTaskSuspendAll()`/`xTaskResumeAll()` in ESP-IDF with `vTaskPreemptionDisable()`/`vTaskPreemptionEnable()` when using SMP FreeRTOS

## Critical Sections

IDF FreeRTOS:

- IDF FreeRTOS uses granular spin locks for critical sections
  - Callers need to instantiate and provide their own spinlocks
  - Allows different cores to simultaneously enter separate critical sections using different locks without contention
  - Some FreeRTOS objects (such as queues and timers) have their own spinlocks
- `portENTER_CRITICAL()`/`portEXIT_CRITICAL()` modified to accept a spinlock argument
- Added variant `portTRY_ENTER_CRITICAL()` to allow for timeout to be specified when entering critical sections

SMP FreeRTOS:

- Uses a giant lock (task and ISR locks to be specific). Cores simultaneously entering a critical section will contest with each other.
- `portENTER_CRITICAL()`/`portEXIT_CRITICAL()` does not accept arguments. Maps directly to `vTaskEnterCritical()`/`vTaskExitCritical()`
- The critical sections now **checks for state change**. This means if a task has just entered a critical section and another higher priority task can run, the critical section will exit to yield and then retry entering.

Changes Made:

- In order to be compatible with IDF style critical sections, `portENTER_CRITICAL(...)`/`portEXIT_CRITICAL(...)` now accepts `VA_ARGS`
  - If no argument exists, it calls SMP FreeRTOS critical sections `vTaskEnterCritical()`
  - If an argument exists, it calls IDF FreeRTOS critical sections (where the argument is the spinlock)

Todo:

- Assess how using a giant lock (i.e., SMP FreeRTOS critical sections) will affect performance on time sensitive components of ESP-IDF (e.g., BT, WiFi)
  - If performance is acceptable, replace all critical sections in ESP-IDF with SMP FreeRTOS critical sections
  - Else, refactor IDF FreeRTOS critical sections to a separate API, and have IDF components call those instead.

## Idle Tasks

IDF FreeRTOS:

- Only uses a single idle task function `prvIdleTask()`
- An idle task is created for each core and pinned to that core
- `prvIdleTask()` calls a custom `esp_vApplicationIdleHook()`, mostly used to feed the task watchdog
- IDF adds a Thread Local Storage Pointer (TLSP) Deletion Callback that is called on task deletion (i.e., can be called from the idle task).

SMP FreeRTOS:

- There are now two types of idle task functions. The `prvIdleTask()` and `prvPassiveIdleTask()`
  - `prvPassiveIdleTask()` simply calls the `vApplicationPassiveIdleHook()`
  - `prvIdleTask()` calls `prvCheckTasksWaitingTermination()`, `vApplicationIdleHook()`, `vApplicationPassiveIdleHook()`, and handles tickless idling.
  - On an N core build, one `prvIdleTask()` task is created and N-1 `prvPassiveIdleTask()` tasks are created.
- The created idle tasks are all unpinned. The idle tasks are run on a "first come first serve" basis meaning when a core goes idle, it selects whatever available idle task it can run.

Changes Made:

- The `esp_vApplicationIdleHook()` is now called from `vApplicationPassiveIdleHook()` since every idle task calls the `vApplicationPassiveIdleHook()`.
- Since the idle tasks are unpinned, the task WDT has been updated to use the "User" feature. Thus, feeding the task watchdog now tracks which "core" has fed the task WDT instead of which specific idle task has fed.
- Since `prvIdleTask()` is solely responsible for calling `prvCheckTasksWaitingTermination()` but can run on any core, multiple IDF cleanup routines are now routed through `portCLEAN_UP_TCB()`
  - FPU registers of a task are now cleaned up via `portCLEAN_UP_TCB() -> vPortCleanUpCoprocArea()` and can clean FPU save areas across cores.
  - TLSP Deletion callbacks are now run via `portCLEAN_UP_TCB() -> vPortTLSPointersDelCb()`

Todo:

- Add support for `configIDLE_SHOULD_YIELD`

## Tick Interrupt

IDF FreeRTOS:

- All cores have their own tick interrupts (same frequency but can be out of phase)
- Each core calls `xTaskIncrementTick()` independently. `xTaskIncrementTick()` has been modified as such:
  - Core 0 is responsible for time keeping (incrementing tick count and unblocking timed out tasks)
  - Each core independently checks for time slicing, and calls `vApplicationTickHook()`
  - There's also an additional `esp_vApplicationTickHook()` used to feed the interrupt watchdog.

SMP FreeRTOS:

- SMP FreeRTOS requires that `xTaskIncrementTick()` only be called from one core (core 0 in our case). That core's tick interrupt is solely responsible for the following via its call to `xTaskIncrementTick()`:
  - Time keeping (increment ticks and unblocking timed out tasks)
  - Time slicing (calls `prvYieldCore()` on each core that needs to yield)
  - Calls `vApplicationTickHook()`
  - Increments pended ticks when the scheduler is suspended

Changes Made:

- Each core still has a HW tick interrupt that occurs (where the ISR `xPortSysTickHandler()`) and will:
  - Call `esp_vApplicationTickHook()` in order to feed the interrupt WDT on behalf of the current core.
  - Call `xTaskIncrementTick()` only on core 0.

Todo:

- See if we can find another way to implement the interrupt watchdog. Requiring each core to interrupt just to feed the watchdog loses the benefit of having core 0 handling the time slicing of all cores.

## IDF Additional Features and API

IDF FreeRTOS added multiple features/APIs that are specific to IDF. For SMP FreeRTOS, these features/APIs have either been:

- Upstreamed to SMP FreeRTOS officially
- Moved their definition/declaration to `freertos_tasks_c_additions.h`/`idf_additions.h` respectively. Users should  `#include "freertos/idf_additions.h` manually so that we can keep the SMP FreeRTOS API clean.
- Deprecated/Removed from IDF entirely

### `xTaskCreatePinnedToCore()`/`xTaskCreateStaticPinnedToCore()`

- The `xTaskCreate...AffinitySet()` API has been added in order to apply a core affinity on task creation.
- `xTaskCreate...PinnedToCore()` now just wrap the `xTaskCreate...AffinitySet()` equivalent functions and handle the conversion of a core ID into a core affinity mask.
- `xTaskCreate...PinnedToCore()` is now provided via `idf_additions.h`

### `vTaskSetThreadLocalStoragePointerAndDelCallback()`

- If TLSP deletion callbacks are used, `configNUM_THREAD_LOCAL_STORAGE_POINTERS` will be doubled (in order to store the callback pointers in the same array as the TLSPs themselves)
- `vTaskSetThreadLocalStoragePointerAndDelCallback()` moved to `freertos_tasks_c_additions.h`/`idf_additions.h`
- Deletion callbacks invoked from the main idle task via `portCLEAN_UP_TCB()`
