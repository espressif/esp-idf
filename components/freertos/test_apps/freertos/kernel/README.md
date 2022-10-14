# FreeRTOS Tests Guidelines

The FreeRTOS tests are currently being refactored/reorganized with the goal of being upstreamed. This document describes the set of guidelines to which the tests are refactored/reorganized according to.

## Unity Port

These test cases assume that the FreeRTOS port has also ported the [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity). Because each FreeRTOS test case will require the scheduler to be started, the way that each test case is invoked will differ form regular Unity ports.

Regular Unity ports will assume that the `main()` function invokes each test using the `RUN_TEST()` macro. However, these test cases assume the following about the Unity port:

- Each test case is invoked from a `UnityTask` instead of `main()`. Thus each test case is run from the context of the `UnityTask`.
  - The `UnityTask` is created using `xTaskCreate...()` (and pinned to core 0 if SMP) from the port's startup (i.e., `main()`)
  - The port's startup (i.e., `main()`) should also start the scheduler using `vTaskStartScheduler()`
  - Note that this is similar to the startup of most FreeRTOS Demos.
- Each test case is defined using the `TEST_CASE(name, ...)` macro. The `VA_ARGS` of the macro allows each port to specify a set of extra arguments (such as test case labels/tags) to be used into their CI pipelines.
- A `portTestMacro.h` must be provided by each port. This header will contain
  - Some constants used by test cases such as default task stack sizes (e.g., `configTEST_DEFAULT_STACK_SIZE`)
  - Some port implementation specific functions/macros required by test cases such as getting current system time (e.g., `portTEST_GET_TIME()`).

## Test Organization

- Test cases are grouped into sub-directories roughly matching the header files of FreeRTOS (e.g., task, queue, semaphore, event groups etc).
- Each source file should ideally test a particular behavior (e.g., priority scheduling, queue send, scheduler suspend). This should usually result in one test case per behavior, thus one test case per source file
- Some test case behaviors may depend on configuration (e.g., priority scheduling in single core vs SMP). In such cases
  - If the affect is small, use an `#if (config... == 1)` to wrap the affected areas
  - If the affect is large, write a separate test case in a separate source file and wrap the entire test case with `#if (config... == 1)`.

## Test Case Template

Each test case should have the following:

- Test case description describing
  - Purpose of the test case
  - Test case procedure
  - Excepted outcome/behavior of the test case
- The test case code wrapped in its required `config...` macros
- The expected outcomes should be tested using the `TEST_ASSERT_...()` macros provided by unity

```c
// In test_priority_scheduling.c

/*
Test Priority Scheduling (Single Core)

Purpose:
    - Test that the single-core scheduler always schedules the highest priority ready task
Procedure:
    - Raise the unityTask priority to (configMAX_PRIORITIES - 1)
    - unityTask creates the following lower priority tasks
        - task_A (configMAX_PRIORITIES - 2)
        - task_B (configMAX_PRIORITIES - 3)
    - UnityTask blocks for a short period of time to allow task_A to run
    - Clean up and restore unityTask's original priority
Expected:
    - task_A should run after unityTask blocks
    - task_B should never have run
*/
#if ( configNUM_CORES == 1 )

static BaseType_t test_static_var = 0;

static void test_static_func(void)
{
    ...
}

TEST_CASE("Tasks: Priority scheduling single core", "[freertos]")
{
    ...
}

#endif /* configNUM_CORES == 1 */
```
