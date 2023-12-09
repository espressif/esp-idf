/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include "esp_err.h"
#include "soc/soc.h"  // [refactor-todo] IDF-2297
#include "esp_cpu.h"

/*
 * @brief   Structure used for backtracing
 *
 * This structure stores the backtrace information of a particular stack frame
 * (i.e. the PC and SP). This structure is used iteratively with the
 * esp_cpu_get_next_backtrace_frame() function to traverse each frame within a
 * single stack. The next_pc represents the PC of the current frame's caller, thus
 * a next_pc of 0 indicates that the current frame is the last frame on the stack.
 *
 * @note    Call esp_backtrace_get_start() to obtain initialization values for
 *          this structure
 */
typedef struct {
    uint32_t pc;       /* PC of the current frame */
    uint32_t sp;       /* SP of the current frame */
    uint32_t next_pc;  /* PC of the current frame's caller */
    const void *exc_frame;  /* Pointer to the full frame data structure, if applicable */
} esp_backtrace_frame_t;

/**
 * @brief If an OCD is connected over JTAG. set breakpoint 0 to the given function
 *        address. Do nothing otherwise.
 * @param fn  Pointer to the target breakpoint position
 */
void esp_set_breakpoint_if_jtag(void *fn);

/**
 * Get the first frame of the current stack's backtrace
 *
 * Given the following function call flow (B -> A -> X -> esp_backtrace_get_start),
 * this function will do the following.
 * - Flush CPU registers and window frames onto the current stack
 * - Return PC and SP of function A (i.e. start of the stack's backtrace)
 * - Return PC of function B (i.e. next_pc)
 *
 * @note This function is implemented in assembly
 *
 * @param[out] pc       PC of the first frame in the backtrace
 * @param[out] sp       SP of the first frame in the backtrace
 * @param[out] next_pc  PC of the first frame's caller
 */
extern void esp_backtrace_get_start(uint32_t *pc, uint32_t *sp, uint32_t *next_pc);

/**
 * Get the next frame on a stack for backtracing
 *
 * Given a stack frame(i), this function will obtain the next stack frame(i-1)
 * on the same call stack (i.e. the caller of frame(i)). This function is meant to be
 * called iteratively when doing a backtrace.
 *
 * Entry Conditions: Frame structure containing valid SP and next_pc
 * Exit Conditions:
 *  - Frame structure updated with SP and PC of frame(i-1). next_pc now points to frame(i-2).
 *  - If a next_pc of 0 is returned, it indicates that frame(i-1) is last frame on the stack
 *
 * @param[inout] frame  Pointer to frame structure
 *
 * @return
 *  - True if the SP and PC of the next frame(i-1) are sane
 *  - False otherwise
 */
bool esp_backtrace_get_next_frame(esp_backtrace_frame_t *frame);

/**
 * @brief Print the backtrace from specified frame.
 *
 * @param depth The maximum number of stack frames to print (should be > 0)
 * @param frame Starting frame to print from
 * @param panic Indicator if backtrace print is during a system panic
 *
 * @note On the ESP32, users must call esp_backtrace_get_start() first to flush the stack.
 * @note If a esp_backtrace_frame_t* frame is obtained though a call to esp_backtrace_get_start()
 * from some example function func_a(), then frame is only valid within the frame/scope of func_a().
 * Users should not attempt to pass/use frame other frames within the same stack of different stacks.
 *
 * @return
 *      - ESP_OK    Backtrace successfully printed to completion or to depth limit
 *      - ESP_FAIL  Backtrace is corrupted
 */
esp_err_t esp_backtrace_print_from_frame(int depth, const esp_backtrace_frame_t* frame, bool panic);

/**
 * @brief Print the backtrace of the current stack
 *
 * @param depth The maximum number of stack frames to print (should be > 0)
 *
 * @note On RISC-V targets printing backtrace at run-time is only available if
 *       CONFIG_ESP_SYSTEM_USE_EH_FRAME is selected. Otherwise we simply print
 *       a register dump. Function assumes it is called in a context where the
 *       calling task will not migrate to another core, e.g. interrupts disabled/panic handler.
 *
 * @return
 *      - ESP_OK    Backtrace successfully printed to completion or to depth limit
 *      - ESP_FAIL  Backtrace is corrupted
 */
esp_err_t esp_backtrace_print(int depth);

/**
 * @brief Set a watchpoint to break/panic when a certain memory range is accessed.
 * Superseded by esp_cpu_set_watchpoint in esp_cpu.h.
 */
static inline __attribute__((deprecated)) esp_err_t esp_set_watchpoint(int no, void *adr, int size, int flags)
{
    return esp_cpu_set_watchpoint(no, adr, size, (esp_cpu_watchpoint_trigger_t)flags);
}

/**
 * @brief Set a watchpoint to break/panic when a certain memory range is accessed.
 * Superseded by esp_cpu_clear_watchpoint in esp_cpu.h.
 */
static inline __attribute__((deprecated)) void esp_clear_watchpoint(int no)
{
    esp_cpu_clear_watchpoint(no);
}

#endif
#ifdef __cplusplus
}
#endif
