/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* Include headers */
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <unistd.h>

#include "safe_unity.h"

/* Extern gcov exit hook */
extern void __gcov_exit(void);

/* Private function declarations */
static void register_signal_handler(void);
static void signal_handler(int sig);
static void isolated_test_runner_child(UnityTestFunction func);
static void isolated_test_runner(UnityTestFunction func);

/* Signal handling */
static volatile sig_atomic_t signal_received = 0;

/* Private functions */
static void register_signal_handler(void)
{
    signal(SIGSEGV, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGILL, signal_handler);
    signal(SIGBUS, signal_handler);
}

static void signal_handler(int sig)
{
    /* Prevent recursive signal handling */
    if (signal_received) {
        _exit(SAFE_UNITY_TEST_CRASHED);
    }
    signal_received = 1;

    /* Flush gcov data */
    __gcov_exit();

    /* Exit with code indicating crashed */
    _exit(SAFE_UNITY_TEST_CRASHED);
}

static void isolated_test_runner_child(UnityTestFunction func)
{
    /* Register signal handlers */
    register_signal_handler();

    /* Run the test */
    if (TEST_PROTECT()) {
        setUp();
        func();
    }
    if (TEST_PROTECT()) {
        tearDown();
    }

    /* Flush gcov data */
    __gcov_exit();

    /* Exit with code indicating test result */
    Unity.CurrentTestFailed ? _exit(SAFE_UNITY_TEST_FAILED) : _exit(SAFE_UNITY_TEST_PASSED);
}

static void isolated_test_runner(UnityTestFunction func)
{
    /* Fork the process */
    pid_t pid = fork();

    /* Fork failed */
    if (pid < 0) {
        printf("[FAIL] Fork failed unexpectedly\n");
        Unity.CurrentTestFailed = 1;
        return;
    }

    /* Child process */
    if (pid == 0) {
        isolated_test_runner_child(func);

        /* Should never reach here */
        _exit(0);
    }

    /* Parent process */
    /* Wait for child process to finish */
    int status;
    waitpid(pid, &status, 0);

    /* Child process exited */
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        switch (exit_code) {
        case SAFE_UNITY_TEST_PASSED:
            /* Test passed */
            Unity.CurrentTestFailed = 0;
            break;
        case SAFE_UNITY_TEST_FAILED:
            printf("\n[FAIL] Test failed");
            Unity.CurrentTestFailed = 1;
            break;
        case SAFE_UNITY_TEST_CRASHED:
            /* Test crashed */
            printf("[FAIL] Test crashed");
            Unity.CurrentTestFailed = 1;
            break;
        default:
            /* Unexpected exit code */
            Unity.CurrentTestFailed = 1;
            printf("[FAIL] Test exited with unexpected code %d\n", exit_code);
            break;
        }
        return;
    }

    /* Child process terminated by signals that can not be captured */
    Unity.CurrentTestFailed = 1;
    if (WIFSIGNALED(status)) {
        printf("[CRASH] Test terminated by signal %d\n", WTERMSIG(status));
    } else {
        printf("[CRASH] Test terminated with unknown reason\n");
    }
}

/* Test runner */
void safe_unity_run_test(UnityTestFunction func, const char* func_name, const int func_line_num)
{
    /* Announce test start */
    printf("========== Running Test: %s ==========\n", func_name);

    /* Update Unity singleton */
    Unity.CurrentTestName = func_name;
    Unity.CurrentTestLineNumber = (UNITY_LINE_TYPE)func_line_num;
    Unity.NumberOfTests++;

    /* Run test in isolated test runner */
    isolated_test_runner(func);

    /* Conclude test */
    UnityConcludeTest();

    /* Announce test completion */
    if (Unity.CurrentTestFailed) {
        printf("========== Test %s FAILED ==========\n\n", func_name);
    } else {
        printf("========== Test %s PASSED ==========\n\n", func_name);
    }
}
