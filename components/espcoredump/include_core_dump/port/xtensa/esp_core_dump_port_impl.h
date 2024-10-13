/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_CORE_DUMP_PORT_IMPL_H_
#define ESP_CORE_DUMP_PORT_IMPL_H_

/**
 * @file
 * @brief Core dump port interface implementation for Xtensa boards.
 */
#include "esp_core_dump_types.h"
#include "esp_app_format.h"
/**
 * Included for SET_STACK macro
 */
#include <xtensa/xtruntime.h>
#include <xt_instr_macros.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define the type that will be used to describe the current context when
 * doing a backup of the current stack. This same structure is used to restore the stack.
 */
typedef struct {
    uint32_t sp;
    uint32_t a0;
    uint32_t ps;
    uint32_t windowbase;
    uint32_t windowstart;
} core_dump_stack_context_t;

/**
 * @brief Set the stack pointer to the address passed as a parameter.
 * @note This function must be inlined.
 *
 * @param new_sp New stack pointer to set in sp register.
 * @param old_ctx CPU context, related to SP, to fill. It will be given back when restoring SP.
 */
FORCE_INLINE_ATTR void esp_core_dump_replace_sp(void* new_sp, core_dump_stack_context_t* old_ctx)
{
    /* We have to spill all the windows to the stack first as the new stack pointer
     * represents a clean new environment. */
    xthal_window_spill();

    /* Backup the special registers PS, WindowBase and WindowStart. We will need to restore them later */
    asm volatile("mov %0, sp \n" \
                 "mov %1, a0 \n" \
                 "rsr.ps %2 \n"\
                 "rsr.windowbase %3 \n"\
                 "rsr.windowstart %4 \n"\
                 : "=r"(old_ctx->sp),
                 "=r"(old_ctx->a0),
                 "=r"(old_ctx->ps),
                 "=r"(old_ctx->windowbase),
                 "=r"(old_ctx->windowstart) :);

    /* Set the new stack */
    SET_STACK(new_sp);
}

/**
 * @brief Restore the stack pointer that was returned when calling `esp_core_dump_replace_sp()` function.
 *
 * @param ctx CPU context, related to SP, to restore.
 */
FORCE_INLINE_ATTR void esp_core_dump_restore_sp(core_dump_stack_context_t* old_ctx)
{
    /* Start by disabling WindowOverflowEnable bit from PS to make sure we won't get a Window Overflow exception
     * restoring WindowBase and WindowStart registers */
    const uint32_t ps_woe = old_ctx->ps & ~(PS_WOE_MASK);
    asm volatile(\
                 "wsr.ps %0 \n"\
                 "rsync \n"\
                 "wsr.windowbase %1 \n"\
                 "rsync \n"\
                 "wsr.windowstart %2 \n"\
                 "rsync \n"\
                 "mov sp, %3 \n" \
                 "mov a0, %4 \n" \
                 "wsr.ps %5 \n"\
                 "rsync \n"\
                 :: "r"(ps_woe),
                 "r"(old_ctx->windowbase),
                 "r"(old_ctx->windowstart),
                 "r"(old_ctx->sp),
                 "r"(old_ctx->a0),
                 "r"(old_ctx->ps));
}

#ifdef __cplusplus
}
#endif

#endif
