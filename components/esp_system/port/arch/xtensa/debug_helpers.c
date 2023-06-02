/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "sdkconfig.h"

#if CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/task_snapshot.h"
#endif

#include "esp_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_debug_helpers.h"
#include "soc/soc_memory_layout.h"
#include "esp_cpu_utils.h"
#include "esp_private/panic_internal.h"

#include "xtensa/xtensa_context.h"

#include "sdkconfig.h"

#include "esp_rom_sys.h"

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
        esp_rom_printf(" 0x%08X:0x%08X", pc, sp);
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
    esp_backtrace_frame_t stk_frame = { 0 };
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


#if CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT

esp_backtrace_frame_t esp_task_snapshot_to_backtrace_frame(TaskSnapshot_t snapshot)
{
    XtExcFrame* xtf = (XtExcFrame*)snapshot.pxTopOfStack;

    esp_backtrace_frame_t frame = {
        .pc = xtf->pc,
        .sp = xtf->a1,
        .next_pc = xtf->a0,
        .exc_frame = xtf,
    };

    return frame;
}

esp_err_t IRAM_ATTR esp_backtrace_print_all_tasks(int depth, bool panic)
{
    // todo: should we disable context switching, and the other core?
    // It works fine without that, but still seems like a good idea.

    uint32_t task_count = uxTaskGetNumberOfTasks();

    TaskSnapshot_t* snapshots = (TaskSnapshot_t*) calloc(task_count * sizeof(TaskSnapshot_t), 1);

    // get snapshots
    UBaseType_t tcb_size = 0;
    uint32_t got = uxTaskGetSnapshotAll(snapshots, task_count, &tcb_size);

    uint32_t len = got < task_count ? got : task_count;

    print_str("printing all tasks:\n\n", panic);

    esp_err_t err = ESP_OK;

    for (uint32_t i = 0; i < len; i++) {

        TaskHandle_t handle = (TaskHandle_t) snapshots[i].pxTCB;

        char* name = pcTaskGetName(handle);

        print_str(name ? name : "No Name" , panic);

        esp_backtrace_frame_t frame = esp_task_snapshot_to_backtrace_frame(snapshots[i]);

        esp_err_t nerr = esp_backtrace_print_from_frame(depth, &frame, panic);
        if (nerr != ESP_OK) {
            err = nerr;
        }
    }

    free(snapshots);
    return err;
}

#endif // CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT
