// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include "esp_err.h"
#include "soc/soc.h"

#define ESP_WATCHPOINT_LOAD 0x40000000
#define ESP_WATCHPOINT_STORE 0x80000000
#define ESP_WATCHPOINT_ACCESS 0xC0000000

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
} esp_backtrace_frame_t;

/**
 * @brief If an OCD is connected over JTAG. set breakpoint 0 to the given function
 *        address. Do nothing otherwise.
 * @param fn  Pointer to the target breakpoint position
 */
void esp_set_breakpoint_if_jtag(void *fn);

/**
 * @brief Set a watchpoint to break/panic when a certain memory range is accessed.
 *
 * @param no Watchpoint number. On the ESP32, this can be 0 or 1.
 * @param adr Base address to watch
 * @param size Size of the region, starting at the base address, to watch. Must
 *             be one of 2^n, with n in [0..6].
 * @param flags One of ESP_WATCHPOINT_* flags
 *
 * @return ESP_ERR_INVALID_ARG on invalid arg, ESP_OK otherwise
 *
 * @warning The ESP32 watchpoint hardware watches a region of bytes by effectively
 *          masking away the lower n bits for a region with size 2^n. If adr does
 *          not have zero for these lower n bits, you may not be watching the
 *          region you intended.
 */
esp_err_t esp_set_watchpoint(int no, void *adr, int size, int flags);

/**
 * @brief Clear a watchpoint
 *
 * @param no Watchpoint to clear
 *
 */
void esp_clear_watchpoint(int no);

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
 * @brief Print the backtrace of the current stack
 *
 * @param depth The maximum number of stack frames to print (should be > 0)
 *
 * @return
 *      - ESP_OK    Backtrace successfully printed to completion or to depth limit
 *      - ESP_FAIL  Backtrace is corrupted
 */
esp_err_t esp_backtrace_print(int depth);

#endif
#ifdef __cplusplus
}
#endif
