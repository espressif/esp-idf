/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <string.h>
#include <sys/param.h>
#include "soc/soc_memory_layout.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_ipc.h"
#include "esp_debug_helpers.h"
#include "esp_cpu_utils.h"
#include "esp_private/panic_internal.h"
#include "esp_private/freertos_debug.h"
#include "esp_rom_sys.h"
#include "xtensa_context.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char *DEBUG_HELPER_TAG = "DBG HLPR";

bool IRAM_ATTR esp_backtrace_get_next_frame(esp_backtrace_frame_t *frame)
{
    //Use frame(i-1)'s BS area located below frame(i)'s sp to get frame(i-1)'s sp and frame(i-2)'s pc
    void *base_save = (void *)frame->sp;     //Base save area consists of 4 words under SP
    frame->pc = frame->next_pc;
    frame->next_pc = *((uint32_t *)(base_save - 16));     //If next_pc = 0, indicates frame(i-1) is the last frame on the stack
    frame->sp =  *((uint32_t *)(base_save - 12));

    //Return true if both sp and pc of frame(i-1) are sane, false otherwise
    return (esp_stack_ptr_is_sane(frame->sp) && esp_ptr_executable((void*)esp_cpu_process_stack_pc(frame->pc)));
}

static void IRAM_ATTR print_entry(uint32_t pc, uint32_t sp, bool panic)
{
    if (panic) {
        panic_print_str(" 0x");
        panic_print_hex(pc);
        panic_print_str(":0x");
        panic_print_hex(sp);
    } else {
        esp_rom_printf(" 0x%08" PRIX32 ":0x%08" PRIX32, pc, sp);
    }
}

static void IRAM_ATTR print_str(const char* str, bool panic)
{
    if (panic) {
        panic_print_str(str);
    } else {
        esp_rom_printf(str);
    }
}

esp_err_t IRAM_ATTR esp_backtrace_print_from_frame(int depth, const esp_backtrace_frame_t* frame, bool panic)
{
    //Check arguments
    if (depth <= 0) {
        return ESP_ERR_INVALID_ARG;
    }

    //Initialize stk_frame with first frame of stack
    esp_backtrace_frame_t stk_frame;
    memcpy(&stk_frame, frame, sizeof(esp_backtrace_frame_t));

    print_str("\r\n\r\nBacktrace:", panic);
    print_entry(esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp, panic);

    //Check if first frame is valid
    bool corrupted = !(esp_stack_ptr_is_sane(stk_frame.sp) &&
                       (esp_ptr_executable((void *)esp_cpu_process_stack_pc(stk_frame.pc)) ||
                        /* Ignore the first corrupted PC in case of InstrFetchProhibited */
                        (stk_frame.exc_frame && ((XtExcFrame *)stk_frame.exc_frame)->exccause == EXCCAUSE_INSTR_PROHIBITED)));

    uint32_t i = (depth <= 0) ? INT32_MAX : depth;
    while (i-- > 0 && stk_frame.next_pc != 0 && !corrupted) {
        if (!esp_backtrace_get_next_frame(&stk_frame)) {    //Get previous stack frame
            corrupted = true;
        }
        print_entry(esp_cpu_process_stack_pc(stk_frame.pc), stk_frame.sp, panic);
    }

    //Print backtrace termination marker
    esp_err_t ret = ESP_OK;
    if (corrupted) {
        print_str(" |<-CORRUPTED", panic);
        ret =  ESP_FAIL;
    } else if (stk_frame.next_pc != 0) {    //Backtrace continues
        print_str(" |<-CONTINUES", panic);
    }
    print_str("\r\n\r\n", panic);
    return ret;
}

esp_err_t IRAM_ATTR esp_backtrace_print(int depth)
{
    //Initialize stk_frame with first frame of stack
    esp_backtrace_frame_t start = { 0 };
    esp_backtrace_get_start(&(start.pc), &(start.sp), &(start.next_pc));
    return esp_backtrace_print_from_frame(depth, &start, false);
}

typedef struct {
#if !CONFIG_FREERTOS_UNICORE
    volatile bool start_tracing;
    volatile bool finished_tracing;
#endif // !CONFIG_FREERTOS_UNICORE
    struct {
        TaskHandle_t task_hdl;
        uint32_t starting_pc;
        uint32_t starting_sp;
        uint32_t next_pc;
    } cur_tasks[configNUMBER_OF_CORES];
} cur_task_backtrace_ctrl_t;

#if !CONFIG_FREERTOS_UNICORE
static void backtrace_other_cores_ipc_func(void *arg)
{
    cur_task_backtrace_ctrl_t *ctrl = (cur_task_backtrace_ctrl_t *)arg;

    // Suspend the scheduler to prevent task switching
    vTaskSuspendAll();
    /*
    Initialize backtracing for this core:

    - Flush current core's register windows back onto current task's stack using esp_backtrace_get_start()
    - Get starting frame for backtracing (starting frame is the caller of this function) using esp_backtrace_get_start()
    - Save the starting frame details into the control block
    */
    BaseType_t core_id = xPortGetCoreID();  // Get core ID now that task switching is disabled
    ctrl->cur_tasks[core_id].task_hdl = xTaskGetCurrentTaskHandle();
    esp_backtrace_get_start(&ctrl->cur_tasks[core_id].starting_pc,
                            &ctrl->cur_tasks[core_id].starting_sp,
                            &ctrl->cur_tasks[core_id].next_pc);

    // Indicate to backtracing core that this core is ready for backtracing
    ctrl->start_tracing = true;
    // Wait for backtracing core to indicate completion
    while (!ctrl->finished_tracing) {
        ;
    }
    // Resume the scheduler to allow task switching again
    xTaskResumeAll();
}
#endif // !CONFIG_FREERTOS_UNICORE

esp_err_t IRAM_ATTR esp_backtrace_print_all_tasks(int depth)
{
    esp_err_t ret = ESP_OK;
    TaskSnapshot_t *task_snapshots;
    cur_task_backtrace_ctrl_t ctrl = {0};

    /*
    Allocate array to store task snapshots. Users are responsible for ensuring
    tasks don't get created/deleted while backtracing.
    */
    const UBaseType_t num_tasks = uxTaskGetNumberOfTasks();
    task_snapshots = calloc(num_tasks, sizeof(TaskSnapshot_t));
    ESP_GOTO_ON_FALSE(task_snapshots, ESP_ERR_NO_MEM, malloc_err, DEBUG_HELPER_TAG, "Task snapshot alloc failed");

#if !CONFIG_FREERTOS_UNICORE
    // Use IPC call to prepare other core for backtracing
    ESP_GOTO_ON_ERROR(esp_ipc_call(!xPortGetCoreID(), backtrace_other_cores_ipc_func, (void *)&ctrl),
                      ipc_err,
                      DEBUG_HELPER_TAG,
                      "IPC call failed");
    // Wait for other core to confirm its ready for backtracing
    while (!ctrl.start_tracing) {
        ;
    }
#endif // !CONFIG_FREERTOS_UNICORE

    // Suspend the scheduler to prevent task switching
    vTaskSuspendAll();

    /*
    Initialize backtracing for this core:

    - Flush current core's register windows back onto current task's stack using esp_backtrace_get_start()
    - Get starting frame for backtracing (starting frame is the caller of this function) using esp_backtrace_get_start()
    - Save the starting frame details into the control block
    */
    BaseType_t core_id = xPortGetCoreID();  // Get core ID now that task switching is disabled
    ctrl.cur_tasks[core_id].task_hdl = xTaskGetCurrentTaskHandle();
    esp_backtrace_get_start(&ctrl.cur_tasks[core_id].starting_pc,
                            &ctrl.cur_tasks[core_id].starting_sp,
                            &ctrl.cur_tasks[core_id].next_pc);

    // Get snapshot of all tasks in the system
    const UBaseType_t num_snapshots = MIN(num_tasks, uxTaskGetSnapshotAll(task_snapshots, num_tasks, NULL));
    // Print the backtrace of every task in the system
    for (UBaseType_t task_idx = 0; task_idx < num_snapshots; task_idx++) {
        bool cur_running = false;
        TaskHandle_t task_hdl = (TaskHandle_t) task_snapshots[task_idx].pxTCB;
        esp_backtrace_frame_t stk_frame = {0};

        // Check if the task is one of the currently running tasks
        for (BaseType_t core_id = 0; core_id < configNUMBER_OF_CORES; core_id++) {
            if (task_hdl == ctrl.cur_tasks[core_id].task_hdl) {
                cur_running = true;
                break;
            }
        }
        // Initialize the starting backtrace frame of the task
        if (cur_running) {
            /*
            Setting the starting backtrace frame for currently running tasks is different. We cannot
            use the current frame of each running task as the starting frame (due to the possibility
            of the SP changing). Thus, each currently running task will have initialized their callers
            as the starting frame for backtracing, which is saved inside the
            cur_task_backtrace_ctrl_t block.
            */
            stk_frame.pc = ctrl.cur_tasks[core_id].starting_pc;
            stk_frame.sp = ctrl.cur_tasks[core_id].starting_sp;
            stk_frame.next_pc = ctrl.cur_tasks[core_id].next_pc;
        } else {
            // Set the starting backtrace frame using the task's saved stack pointer
            XtExcFrame* exc_frame = (XtExcFrame*) task_snapshots[task_idx].pxTopOfStack;
            stk_frame.pc = exc_frame->pc;
            stk_frame.sp = exc_frame->a1;
            stk_frame.next_pc = exc_frame->a0;
        }
        // Print backtrace
        char* name = pcTaskGetName(task_hdl);
        print_str(name ? name : "No Name", false);
        esp_err_t bt_ret = esp_backtrace_print_from_frame(depth, &stk_frame, false);
        if (bt_ret != ESP_OK) {
            ret = bt_ret;
        }
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
