/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_debug_helpers.h"
#include "esp_private/panic_reason.h"
#include "esp_private/panic_internal.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/freertos_debug.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_private/esp_system_attr.h"
#include "esp_private/esp_cpu_internal.h"
#include "esp_rom_sys.h"
#include "riscv/rvruntime-frames.h"
#include "riscv/libunwind-riscv.h"
#include <string.h>
#include <sys/param.h>

#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif // !CONFIG_FREERTOS_UNICORE

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
#include "esp_private/eh_frame_parser.h"

#elif CONFIG_ESP_SYSTEM_USE_FRAME_POINTER
#include "esp_private/fp_unwind.h"

#else // !CONFIG_ESP_SYSTEM_USE_EH_FRAME && !CONFIG_ESP_SYSTEM_USE_FRAME_POINTER
extern void panic_print_registers(const void *frame, int core);
#endif // CONFIG_ESP_SYSTEM_USE_EH_FRAME

static const char *DEBUG_HELPER_TAG = "DBG HLPR";

/**
 * @brief Print the backtrace (or register dump) for a single saved CPU frame.
 *
 * Targets based on a RISC-V CPU cannot perform backtracing that easily. Depending on
 * the configured backtracing method (ESP_BACKTRACING_METHOD) this prints either a full
 * call stack (EH-frame or frame-pointer mode) or, as a fallback, a register dump that
 * can be decoded offline by idf.py monitor.
 *
 * @param task_name Name of the task being printed. If NULL, no name line is printed
 *                  (used when printing the current stack's backtrace).
 * @param frame     Saved/captured register frame to start backtracing from
 * @param core_id   Core whose CSRs are reflected in the register-dump fallback
 */
static void ESP_SYSTEM_IRAM_ATTR print_task_backtrace(const char *task_name, const RvExcFrame *frame, int core_id)
{
    if (task_name) {
        esp_rom_printf("%s\r\n", task_name);
    }

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
    (void)core_id;
    esp_eh_frame_print_backtrace(frame);
#elif CONFIG_ESP_SYSTEM_USE_FRAME_POINTER
    (void)core_id;
    esp_fp_print_backtrace(frame);
#else // !CONFIG_ESP_SYSTEM_USE_EH_FRAME && !CONFIG_ESP_SYSTEM_USE_FRAME_POINTER
    esp_cpu_frame_t backtrace_frame;
    memcpy(&backtrace_frame, frame, sizeof(esp_cpu_frame_t));
    panic_prepare_frame_from_ctx(&backtrace_frame);
    panic_print_registers(&backtrace_frame, core_id);
    esp_rom_printf("\r\n");
    esp_rom_printf("Please enable CONFIG_ESP_SYSTEM_USE_FRAME_POINTER option to have a full backtrace.\r\n");
#endif // CONFIG_ESP_SYSTEM_USE_EH_FRAME
}

/*
 * Note: On RISC-V the depth argument is not honored. The frame-pointer and eh_frame
 * unwinders always unwind to completion, and the register-dump fallback prints no stack.
 */
esp_err_t ESP_SYSTEM_IRAM_ATTR esp_backtrace_print(int depth)
{
    (void)depth;

    const int current_core = xPortGetCoreID();

    TaskSnapshot_t snapshot = { 0 };
    BaseType_t ret = vTaskGetSnapshot(xTaskGetCurrentTaskHandleForCore(current_core), &snapshot);

    if (ret != pdTRUE) {
        return ESP_ERR_NOT_FOUND;
    }

    print_task_backtrace(NULL, (const RvExcFrame *)snapshot.pxTopOfStack, current_core);

    return ESP_OK;
}

typedef struct {
#if !CONFIG_FREERTOS_UNICORE
    volatile bool start_tracing;
    volatile bool finished_tracing;
#endif // !CONFIG_FREERTOS_UNICORE
    struct {
        TaskHandle_t task_hdl;
        RvExcFrame frame;
    } cur_tasks[configNUMBER_OF_CORES];
} riscv_backtrace_ctrl_t;

#if !CONFIG_FREERTOS_UNICORE
static void backtrace_other_cores_ipc_func(void *arg)
{
    riscv_backtrace_ctrl_t *ctrl = (riscv_backtrace_ctrl_t *)arg;

    vTaskSuspendAll();

    BaseType_t core_id = xPortGetCoreID();
    ctrl->cur_tasks[core_id].task_hdl = xTaskGetCurrentTaskHandle();
    UNW_GET_CONTEXT(&ctrl->cur_tasks[core_id].frame);

    /* Ensure the task_hdl and frame writes are visible to the backtracing core
       before it observes start_tracing == true. */
    __sync_synchronize();
    ctrl->start_tracing = true;
    while (!ctrl->finished_tracing) {
        ;
    }

    xTaskResumeAll();
}
#endif // !CONFIG_FREERTOS_UNICORE

esp_err_t esp_backtrace_print_all_tasks(int depth)
{
    if (depth <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;
    TaskSnapshot_t *task_snapshots;
    riscv_backtrace_ctrl_t ctrl = {0};

    const UBaseType_t num_tasks = uxTaskGetNumberOfTasks();
    task_snapshots = calloc(num_tasks, sizeof(TaskSnapshot_t));
    ESP_GOTO_ON_FALSE(task_snapshots, ESP_ERR_NO_MEM, malloc_err, DEBUG_HELPER_TAG, "Task snapshot alloc failed");

#if !CONFIG_FREERTOS_UNICORE
    // Use IPC call to prepare other core for backtracing
    ESP_GOTO_ON_ERROR(esp_ipc_call(!xPortGetCoreID(), backtrace_other_cores_ipc_func, (void *)&ctrl),
                      ipc_err,
                      DEBUG_HELPER_TAG,
                      "IPC call failed");
    // Wait for other core to confirm it is ready for backtracing
    while (!ctrl.start_tracing) {
        ;
    }
#endif // !CONFIG_FREERTOS_UNICORE

    // Suspend the scheduler to prevent task switching
    vTaskSuspendAll();

    // Capture the current core's running task context now that task switching is disabled
    const BaseType_t cur_core_id = xPortGetCoreID();
    ctrl.cur_tasks[cur_core_id].task_hdl = xTaskGetCurrentTaskHandle();
    UNW_GET_CONTEXT(&ctrl.cur_tasks[cur_core_id].frame);

    // Get snapshot of all tasks in the system
    UBaseType_t snapshot_count = uxTaskGetSnapshotAll(task_snapshots, num_tasks, NULL);
    const UBaseType_t num_snapshots = MIN(num_tasks, snapshot_count);

    // Print the backtrace of every task in the system
    for (UBaseType_t task_idx = 0; task_idx < num_snapshots; task_idx++) {
        TaskHandle_t task_hdl = (TaskHandle_t)task_snapshots[task_idx].pxTCB;
        const RvExcFrame *frame_ptr = NULL;
        RvExcFrame saved_frame;

        // Check if the task is one of the currently running tasks
        bool cur_running = false;
        BaseType_t running_core_id = 0;
        for (BaseType_t i = 0; i < configNUMBER_OF_CORES; i++) {
            if (task_hdl == ctrl.cur_tasks[i].task_hdl) {
                cur_running = true;
                running_core_id = i;
                break;
            }
        }

        if (cur_running) {
            /*
             * For the currently running task(s) use the context we captured
             * via UNW_GET_CONTEXT() rather than pxTopOfStack, which may not
             * yet reflect the suspended state.
             */
            memcpy(&saved_frame, &ctrl.cur_tasks[running_core_id].frame, sizeof(RvExcFrame));
            frame_ptr = &saved_frame;
        } else {
            // Set the starting backtrace frame using the task's saved stack pointer
            frame_ptr = (const RvExcFrame *)task_snapshots[task_idx].pxTopOfStack;
        }

        /*
         * The register-dump fallback reflects the CSRs of the core executing this
         * function, so report the current core for the dump's "Core N" label.
         */
        char *name = pcTaskGetName(task_hdl);
        print_task_backtrace(name ? name : "No Name", frame_ptr, (int)cur_core_id);
    }

    // Resume the scheduler to allow task switching again
    xTaskResumeAll();

#if !CONFIG_FREERTOS_UNICORE
    // Indicate to the other core that backtracing is complete
    ctrl.finished_tracing = true;
#endif // !CONFIG_FREERTOS_UNICORE

    free(task_snapshots);
    return ret;

#if !CONFIG_FREERTOS_UNICORE
ipc_err:
    free(task_snapshots);
#endif // !CONFIG_FREERTOS_UNICORE
malloc_err:
    return ret;
}
