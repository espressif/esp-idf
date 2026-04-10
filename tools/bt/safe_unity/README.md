# Safe Unity Test Runner

A safe test execution wrapper for the Unity test framework in ESP-IDF projects, designed to prevent test crashes from terminating the entire test suite.

## Overview

The Safe Unity component provides isolated test execution for Unity framework tests by running each test in a separate child process. This isolation prevents crashes, segmentation faults, or other fatal errors in individual tests from affecting the test runner or other tests.

## Features

- **Process Isolation**: Each test runs in a separate child process
- **Crash Protection**: Handles common crash signals (SIGSEGV, SIGABRT, SIGFPE, SIGILL, SIGBUS)
- **Code Coverage Support**: Integrates with gcov for code coverage collection
- **Detailed Reporting**: Provides clear test result reporting with crash detection
- **Linux Host Testing**: Specifically designed for ESP-IDF host-based unit testing

## Supported Platforms

- **Linux only**: This component is designed specifically for Linux host-based testing
- **ESP targets**: Not supported (component automatically returns for ESP targets)

## Basic Usage

### Simple Test Execution

```c
#include "safe_unity.h"

void test_example_function(void)
{
    TEST_ASSERT_EQUAL(42, my_function_that_returns_42());
}

void app_main(void)
{
    UNITY_BEGIN();
    
    // Run test safely - crashes won't terminate the test runner
    RUN_TEST_SAFE(test_example_function);
    
    UNITY_END();
}
```

## API Reference

### Macros

#### `RUN_TEST_SAFE(func)`

Safely executes a Unity test function in an isolated process.

**Parameters:**
- `func`: Unity test function to execute

**Example:**
```c
RUN_TEST_SAFE(my_test_function);
```

## How It Works

### Process Isolation

1. **Fork Process**: For each test, a child process is created using `fork()`
2. **Signal Handling**: The child process registers signal handlers for crash detection
3. **Test Execution**: The test runs with proper Unity setup/teardown in the child process
4. **Result Collection**: The parent process waits for the child and analyzes the exit status
5. **Coverage Flush**: Code coverage data is flushed before process termination

### Signal Handling

The component handles the following signals in child processes:

- `SIGSEGV`: Segmentation fault
- `SIGABRT`: Abort signal
- `SIGFPE`: Floating point exception
- `SIGILL`: Illegal instruction
- `SIGBUS`: Bus error

When any of these signals are received, the test is marked as crashed and the process exits gracefully after flushing coverage data.

## Code Coverage Integration

The component automatically integrates with gcov for code coverage collection:

- Coverage data is flushed before each test process exits
- Both passing and crashing tests contribute to coverage statistics
- No additional configuration required when using `--coverage` flags

## Build Configuration

The component requires the following CMake configuration:

```cmake
# In your project's CMakeLists.txt
if(${target} STREQUAL "linux")
    idf_component_register(
        SRCS "your_test.c"
        INCLUDE_DIRS "."
        REQUIRES safe_unity
    )
endif()
```

## Examples

### Complete Test Suite

```c
#include "safe_unity.h"

void setUp(void) {
    // Test setup code
}

void tearDown(void) {
    // Test cleanup code
}

void test_normal_operation(void) {
    TEST_ASSERT_EQUAL(42, my_function());
}

void test_edge_case(void) {
    TEST_ASSERT_NULL(my_function_with_null_return());
}

void test_potential_crash(void) {
    // This might crash in some conditions
    my_risky_function();
    TEST_ASSERT_TRUE(true);
}

void app_main(void) {
    UNITY_BEGIN();
    
    RUN_TEST_SAFE(test_normal_operation);
    RUN_TEST_SAFE(test_edge_case);
    RUN_TEST_SAFE(test_potential_crash);
    
    UNITY_END();
}
```
