/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <string.h>
#include "esp_private/esp_system_attr.h"
#include "esp_private/panic_internal.h"
#include "esp_memory_utils.h"
#include "riscv/libunwind-riscv.h"
#include "esp_private/fp_unwind.h"

#define FP_MAX_CALLERS  64
#define RA_INDEX_IN_FP  -1
#define SP_INDEX_IN_FP  -2

/**
 * @brief When one step of the backtrace is generated, output it to the serial.
 * This function can be overridden as it is defined as weak.
 *
 * @param pc Program counter of the backtrace step.
 * @param sp Stack pointer of the backtrace step.
 */
void __attribute__((weak)) esp_fp_generated_step(uint32_t pc, uint32_t sp)
{
    panic_print_str(" 0x");
    panic_print_hex(pc);
    panic_print_str(":0x");
    panic_print_hex(sp);
}

/**
 * @brief Check if the given pointer is a data pointer that can be dereferenced
 *
 * @param ptr Data pointer to check
 */
static inline bool esp_fp_ptr_is_data(void* ptr)
{
    return esp_ptr_in_dram(ptr) || esp_ptr_external_ram(ptr);
}

/**
 * @brief Generate a call stack starting at the given frame pointer.
 *
 * @note Since this function can be called from RAM (from heap trace), it shall also be put in RAM.
 *
 * @param frame[in] Frame pointer to start unrolling from.
 * @param callers[out] Array of callers where 0 will store the most recent caller. Can be NULL.
 * @param stacks[out] Array of callers' stacks where 0 will store the most recent caller's stack. Can be NULL.
 * @param depth[in] Number of maximum entries to fill in the callers array.
 *
 * @returns Number of entries filled in the array.
 */
uint32_t ESP_SYSTEM_IRAM_ATTR esp_fp_get_callers(uint32_t frame, void** callers, void** stacks, uint32_t depth)
{
    uint32_t written = 0;
    uint32_t pc = 0;
    uint32_t sp = 0;
    uint32_t* fp = (uint32_t*) frame;

    if (depth == 0 || (callers == NULL && stacks == NULL)) {
        return 0;
    }

    /**
     * We continue filling the callers array until we meet a function in ROM (not compiled
     * with frame pointer) or the pointer we retrieved is not in RAM (binary libraries
     * not compiled with frame pointer configuration, which means what is stored in the register is not a valid pointer)
     */
    while (depth > 0 && esp_fp_ptr_is_data(fp) && !esp_ptr_in_rom((void *) pc)) {
        /* Dereference the RA register from the frame pointer and store it in PC */
        pc = fp[RA_INDEX_IN_FP];
        sp = fp[SP_INDEX_IN_FP];
        fp = (uint32_t*) sp;

        if (callers) {
            callers[written] = (void*) pc;
        }
        if (stacks) {
            stacks[written] = (void*) sp;
        }

        depth--;
        written++;
    }

    return written;
}

/**
 * @brief Print backtrace for the given execution frame.
 *
 * @param frame_or Snapshot of the CPU registers when the CPU stopped its normal execution.
 */
void esp_fp_print_backtrace(const void *frame_or)
{
    assert(frame_or != NULL);
    ExecutionFrame frame = *((ExecutionFrame*) frame_or);

    const uint32_t sp = EXECUTION_FRAME_SP(frame);
    const uint32_t pc = EXECUTION_FRAME_PC(frame);
    const uint32_t fp = frame.s0;
    void* callers[FP_MAX_CALLERS];
    void* stacks[FP_MAX_CALLERS];

    panic_print_str("Backtrace:");
    esp_fp_generated_step(pc, sp);

    uint32_t len = esp_fp_get_callers(fp, callers, stacks, FP_MAX_CALLERS);

    for (int i = 0; i < len; i++) {
        esp_fp_generated_step((uint32_t) callers[i], (uint32_t) stacks[i]);
    }

    panic_print_str("\r\n");
}
