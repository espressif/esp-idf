# Overview

# Terminology

The following terms will be used in this document to avoid confusion between the different FreeRTOS versions currently in ESP-IDF

- SMP FreeRTOS: The SMP branch of the FreeRTOS kernel found [here](https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/smp)
- IDF FreeRTOS: The version of FreeRTOS used in mainline ESP-IDF that contained custom modifications to support SMP features specific to the ESP chips.

# Organization

This directory contains a copy of SMP FreeRTOS based off of upstream commit [a97741a](https://github.com/FreeRTOS/FreeRTOS-Kernel/commit/a97741a08d36ac08d913b8bc86abf128df627e85)

- IDF FreeRTOS remains in  `components/freertos/FreeRTOS-Kernel`
- SMP FreeRTOS is entirely contained in `components/freertos/FreeRTOS-Kernel-SMP`
  - The Xtensa port files are copied from IDF FreeRTOS with minimal modifications.
  - `port.c` and `portmacro.h` were mostly re-written from scratch
  - Some changes were made to SMP FreeRTOS `tasks.c` in order to be compatible with IDF.
    - All of these changes are wrapped in `#ifdef ESP_PLATFORM`
    - These additions will be removed after the compatibility issues are resolved in IDF.
- SMP FreeRTOS is only compiled if `CONFIG_FREERTOS_SMP` is set in `menuconfig`
- All changes made to the rest of ESP-IDF to support SMP FreeRTOS are wrapped in `#ifdef CONFIG_FREERTOS_SMP`. Thus SMP FreeRTOS changes should have no effect on mainline ESP-IDF.

# Behavioral Differences and Porting

This section covers

- The major feature/behavior differences between SMP FreeRTOS vs IDF FreeRTOS
- The changes made to the port or SMP FreeRTOS sources to remedy these differences
- Future Todos

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

- IDF FreeRTOS uses granualar spin locks for critical sections 
  - Callers need to instantiate and provide their own spinlocks
  - But allows different cores to simultaneously enter separate critical sections using different locks without contesting
- `portENTER_CRITICAL()`/`portEXIT_CRITICAL()` modified to accept a spinlock argument
- Added variant `portTRY_ENTER_CRITICAL()` to allow for timeout to be specified when entering critical sections

SMP FreeRTOS:

- Uses a giant lock (task and ISR locks to be specific). Cores simultaneously entering a critical section will contest with each other.
- `portENTER_CRITICAL()`/`portEXIT_CRITICAL()` does not accept arguments. Maps directly to `vTaskEnterCritical()`/`vTaskExitCritical()`
- Has added capability of checking for state change, allowing critical section contester to yield

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

- There are now two idle task functions. `prvIdleTask()` and `prvMinimalIdleTask()`
  - `prvMinimalIdleTask()` simply calls the `vApplicationMinimalIdleHook()` and handles idle yielding
  - `prvIdleTask()` has the added responsibility of calling `prvCheckTasksWaitingTermination()` and `vApplicationIdleHook()`.
  - One `prvIdleTask()`  is created, the remaining idle tasks are `prvMinimalIdleTask()`
- The created idle tasks are all unpinned.

Changes Made:

- Idle tasks were now pinned in order to be compatible with ESP-IDF
- Currently, `vApplicationIdleHook()` and `vApplicationMinimalIdleHook()` both just call `esp_vApplicationIdleHook()` in order to feed the task watchdog.

Todo:

- Remove the pinning of idle tasks.
  - Find some way to handle how FPU register contexts are cleaned up on task deletion. Idle tasks are no longer pinned, thus we need to handle the case where FPU cleanup is triggered across cores.
  - Check if unpinned idle tasks will affect the feeding of the idle watchdog timer.
- Find some way to integrate TLSP Deletion Callbacks to the idle tasks.
- Check if tickless Idle (and automatic light sleep) works in SMP FreeRTOS
- Separate out `esp_vApplicationIdleHook()` from the SMP FreeRTOS Idle hooks for the following reasons:
  - Users may want to provide their own `vApplicationIdleHook()` and `vApplicationMinimalIdleHook()`, thus IDF should not use those identifiers
  - `prvIdleTask()` will call both `vApplicationIdleHook()` and `vApplicationMinimalIdleHook()`, thus the watchdog is fed twice.
- Add support for `configIDLE_SHOULD_YIELD`
- See if TLSP Deletion Callbacks can be upstreamed as a new feature

## Tick Interrupt

IDF FreeRTOS:

- All cores have their own tick interrupts (same frequency but can be out of phase)
- Each core calls `xTaskIncrementTick()` independently. `xTaskIncrementTick()` has been modified as such:
  - Core 0 is responsible for time keeping (incrementing tick count and unblocking timed out tasks)
  - Each core independently checks for time slicing, and calls `vApplicationTickHook()`
  - There's also an additional `esp_vApplicationTickHook()` used to feed the interrupt watchdog.

SMP FreeRTOS:

- Only core 0 calls `xTaskIncrementTick()`. Thus core 0 is solely responsible for:
  - Time keeping (increment ticks and unblocking timed out tasks)
  - Time slicing (calls `prvYieldCore()` on each core that needs to yield)
  - Calls `vApplicationTickHook()`
  - Increments pended ticks when the scheduler is suspended

Changes Made:

- Each core still has a tick interrupt (to call `esp_vApplicationTickHook()`). But only core 0 will call `xTaskIncrementTick()`

Todo:

- See if we can find another way to implement the interrupt watchdog. Requiring each core to interrupt just to feed the watchdog loses the benefit of having core 0 handling the time slicing of all cores.

## Additional API

IDF FreeRTOS added several APIs. These are copied over to SMP FreeRTOS to maintain compatibility. These API can either be:
- Upstreamed to SMP FreeRTOS officially
- Moved to `freertos_tasks_c_additions.h`
- Removed from IDF entirely

### `xTaskCreatePinnedToCore()`/`xTaskCreateStaticPinnedToCore()`

- `xTaskCreate...AffinitySet()` have been upstreamed
- `xTaskCreate...PinnedToCore()` now just map to the `xTaskCreate...AffinitySet()` equivalent functions.

### `vTaskSetThreadLocalStoragePointerAndDelCallback()`

- Used to set the TLSP deletion callbacks.

### `xTaskGetCurrentTaskHandleForCPU()`

- Convenience function to the get current task of a particular CPU
- Check if this can be upstreamed

### `xTaskGetIdleTaskHandleForCPU()`

- Convenience function to the get the idle task handle for a particular CPU
- Can be entirely replaced with `xTaskGetIdleTaskHandle()`

### `xTaskGetAffinity()`

- Returns what core a task is pinned to, and not an affinity mask.
- Can be entirely replaced with `vTaskCoreAffinityGet()`
