/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This file contains most of the code located in the demo application in the
 * upstream FreeRTOS repository. It is put here so that IDF applications can
 * seamlessly switch between Linux and chip targets without the need to provide
 * or implement additional functionality if the target is the Linux target.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "utils/wait_for_event.h"
#include "esp_log.h"

#define BACKTRACE_PC_ARRAY_SIZE 20
#define ON_SEGFAULT_MESSAGE "ERROR: Segmentation Fault, here's your backtrace:\n"
#define ON_ABORT_MESSAGE "ERROR: Aborted\n"

#if (defined(__APPLE__) && defined(__MACH__))
typedef sig_t sighandler_t;
#endif

static const char *TAG = "port";

static volatile UBaseType_t uxInterruptNesting = 0;


BaseType_t xPortCheckIfInISR(void)
{
    return uxInterruptNesting;
}

#if CONFIG_COMPILER_OPTIMIZATION_DEBUG
#define BACKTRACE_PC_ARRAY_SIZE_DUMMY 1
/**
 * This function calls backtrace once to ensure that libgcc is loaded already.
 */
static void load_libgcc(void)
{
    void *array[BACKTRACE_PC_ARRAY_SIZE_DUMMY];
    size_t size = backtrace(array, BACKTRACE_PC_ARRAY_SIZE_DUMMY);
    assert(size == 1); // Since this function can be called, the first stack frame should be present
}

/*
 * Print a rudimentary backtrace to help users a bit with segfaults.
 */
static void segfault_handler(int sig)
{
    void *array[BACKTRACE_PC_ARRAY_SIZE];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, BACKTRACE_PC_ARRAY_SIZE);

    // we need a raw file write here because other functions are not async-signal-safe
    int written = write(STDERR_FILENO, ON_SEGFAULT_MESSAGE, sizeof(ON_SEGFAULT_MESSAGE));
    (void) written; // The return value is ignored for now, as we don't have a lot of options in case of failure
                    // and EINTR can't happen in a signal handler anyways

    backtrace_symbols_fd(array, size, STDERR_FILENO);
    _exit(1);
}

/*
 * Print a message to signal abort, even in idf.py monitor.
 */
static void abort_handler(int sig)
{
    // we need a raw file write here because other functions are not async-signal-safe
    int written = write(STDERR_FILENO, ON_ABORT_MESSAGE, sizeof(ON_ABORT_MESSAGE));
    (void) written; // The return value is ignored for now, as we don't have a lot of options in case of failure
                    // and EINTR can't happen in a signal handler anyways

    _exit(1);
}
#endif // CONFIG_COMPILER_OPTIMIZATION_DEBUG

void app_main(void);

static void main_task(void* args)
{
    app_main();
    vTaskDelete(NULL);
}

int main(int argc, const char **argv)
{
    // This makes sure that stdio is always synchronized so that idf.py monitor
    // and other tools read text output on time.
    setvbuf(stdout, NULL, _IONBF, 0);

#if CONFIG_COMPILER_OPTIMIZATION_DEBUG
    // Ensures that libgcc is loaded to avoid problems when loading it later in
    // the signal handler (see NOTES section in glibc backtrace man page)
    load_libgcc();
    sighandler_t sig_res;

    // Enable backtraces
    sig_res = signal(SIGSEGV, segfault_handler);
    if (sig_res == SIG_ERR) {
        perror("Failed setting the segfault handler");
        abort();
    }

    // Enable error message on abort
    sig_res = signal(SIGABRT, abort_handler);
    if (sig_res == SIG_ERR) {
        perror("Failed setting the abort handler");
        abort();
    }
#endif // CONFIG_COMPILER_OPTIMIZATION_DEBUG

    usleep(1000);
    BaseType_t res = xTaskCreatePinnedToCore(&main_task, "main",
                                                ESP_TASK_MAIN_STACK, NULL,
                                                ESP_TASK_MAIN_PRIO, NULL, ESP_TASK_MAIN_CORE);
    assert(res == pdTRUE);
    (void)res;

    ESP_LOGI(TAG, "Starting scheduler.");
    vTaskStartScheduler();

    // This line should never be reached
    assert(false);
}

void esp_vApplicationIdleHook(void)
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
     * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
     * task.  It is essential that code added to this hook function never attempts
     * to block in any way (for example, call xQueueReceive() with a block time
     * specified, or call vTaskDelay()).  If application tasks make use of the
     * vTaskDelete() API function to delete themselves then it is also important
     * that vApplicationIdleHook() is permitted to return to its calling function,
     * because it is the responsibility of the idle task to clean up memory
     * allocated by the kernel to any task that has since deleted itself. */


    usleep( 15000 );
}

void esp_vApplicationTickHook( void ) { }

#if  (  configUSE_TICK_HOOK > 0 )
void vApplicationTickHook( void )
{
    esp_vApplicationTickHook();
}
#endif

void vPortYieldOtherCore( BaseType_t coreid ) { } // trying to skip for now

#if ( configSUPPORT_STATIC_ALLOCATION == 1 )
/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
     * state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
#endif // configSUPPORT_STATIC_ALLOCATION == 1
/*-----------------------------------------------------------*/

#if ( (configSUPPORT_STATIC_ALLOCATION == 1) && (configUSE_TIMERS == 1))

/* When configSUPPORT_STATIC_ALLOCATION is set to 1 the application writer can
 * use a callback function to optionally provide the memory required by the idle
 * and timer tasks.  This is the stack that will be used by the timer task.  It is
 * declared here, as a global, so it can be checked by a test that is implemented
 * in a different file. */
StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
 * function then they must be declared static - otherwise they will be allocated on
 * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif // (configSUPPORT_STATIC_ALLOCATION == 1) && (configUSE_TIMERS == 1)

void __attribute__((weak)) vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
#define ERR_STR1 "***ERROR*** A stack overflow in task "
#define ERR_STR2 " has been detected."
    const char *str[] = {ERR_STR1, pcTaskName, ERR_STR2};

    char buf[sizeof(ERR_STR1) + CONFIG_FREERTOS_MAX_TASK_NAME_LEN + sizeof(ERR_STR2) + 1 /* null char */] = {0};

    char *dest = buf;
    for (int i = 0; i < sizeof(str) / sizeof(str[0]); i++) {
        dest = strcat(dest, str[i]);
    }
	printf("%s\n", buf);
    abort();
}
