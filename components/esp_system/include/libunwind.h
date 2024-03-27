/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include <stddef.h>
#include <stdint.h>

#if CONFIG_IDF_TARGET_ARCH_RISCV
#include "libunwind-riscv.h"
#elif CONFIG_IDF_TARGET_X86
#include "libunwind-x86.h"
#else
/* This header must be a standalone one, so, it shall not trigger an error when
 * pre-processed without including any of the architecture header above.
 * The implementation can trigger a compile error if UNW_UNKNOWN_TARGET
 * macro is defined. */
#define UNW_UNKNOWN_TARGET 1
typedef void* ExecutionFrame;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes returned by the functions defined below */
#define UNW_ESUCCESS        0
#define UNW_EUNSPEC         1  /* General failure */
#define UNW_EBADREG         3  /* Register given is wrong */
#define UNW_ESTOPUNWIND     5
#define UNW_EINVAL          8  /* Bad parameter or unimplemented operation */
#define UNW_EBADVERSION     9
#define UNW_ENOINFO         10

/* A libunwind context is the equivalent of an ESP-IDF ExecutionFrame */
typedef ExecutionFrame unw_context_t;

/* A register number is an unsigned word in our case */
typedef uint32_t unw_regnum_t;

/* In our current implementation, a cursor is the same as a context */
typedef unw_context_t unw_cursor_t;

/* long should represent the size of a CPU register */
typedef unsigned long unw_word_t;

/* At the moment, we don't support the operations using the following types,
 * so just set them to void* */
typedef void* unw_addr_space_t;
typedef void* unw_fpreg_t;

/**
 * @brief Get the current CPU context.
 *
 * @param[out] ctx Pointer to `unw_context_t` structure. It must not be NULL
 *                 as it will be filled with the CPU registers value
 *
 * @return UNW_ESUCCESS on success, -UNW_EUNSPEC if ctx is NULL
 *
 * @note This function MUST be inlined. Marking it as "static inline" or
 *       __attribute__((always_inline)) does not guarantee that it will inlined by
 *       the compiler for all the architectures. Thus, define this function as a macro.
 * @note If the caller of this function returns, all the pointers, contexts, cursors
 *       generated out of the initial returned context shall be considered invalid and
 *       thus, must **not** be used.
 */
#define unw_getcontext(ctx) ({  int retval; \
                                if (ctx == NULL) { \
                                    retval = -UNW_EUNSPEC; \
                                } else { \
                                    UNW_GET_CONTEXT(ctx); \
                                    retval = UNW_ESUCCESS; \
                                } \
                                retval; \
                            })

/**
 * @brief Initialize a cursor on a local context. Multiple cursor can be initialized on
 *        a given CPU context, they can then be manipulated independently.
 *
 * @param[out] c Pointer on cursor to be returned. Must not be NULL
 * @param[in] ctx Pointer on the context returned by the function `unw_getcontext`
 *
 * @return UNW_ESUCCESS on success, -UNW_EUNSPEC if one of the parameter is NULL.
 */
int unw_init_local(unw_cursor_t* c, unw_context_t* ctx);

/**
 * @brief Perform a step "up" on the given cursor. After calling this function, the
 *        cursor will point to the caller's CPU context. Thus, it is then possible
 *        to retrieve the caller's address by getting the PC register out of the cursor.
 *        Check `unw_get_reg` function for this.
 *
 * @param[in] cp Current cursor
 *
 * @returns 0 if the previous frame was the last one
 * @returns Positive value on success
 * @returns -UNW_EBADVERSION if the DWARF information's version is not compatible with the eh_frame_parser implementation
 * @returns -UNW_ENOINFO if the caller information are not present in the binary. (if the caller is in ROM for example)
 * @returns -UNW_ESTOPUNWIND if unwinding is terminated
 */
int unw_step(unw_cursor_t* cp);

/**
 * @brief Get the value of a CPU register from a given cursor.
 *
 * @param[in] cp Pointer to the cursor
 * @param reg Register number to retrieve the value of
 * @param[out] valp Pointer that will be filled with the register value
 *
 * @returns UNW_ESUCCESS on success
 * @returns -UNW_EUNSPEC if any pointer passed is NULL
 * @returns -UNW_EBADREG if the register number is invalid
 */
int unw_get_reg(unw_cursor_t* cp, unw_regnum_t reg, unw_word_t* valp);

/**
 * @brief Set the value of a CPU register in a given cursor.
 *
 * @param[in]cp Pointer to the cursor
 * @param reg Register number to set the value of
 * @param val New register value
 *
 * @returns UNW_ESUCCESS on success
 * @returns -UNW_EUNSPEC if the pointer passed is NULL
 * @returns -UNW_EBADREG if the register number is invalid
 */
int unw_set_reg(unw_cursor_t* cp, unw_regnum_t reg, unw_word_t val);

#ifdef __cplusplus
}
#endif
