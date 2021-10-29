/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ---------------------------------------------------- Deprecate ------------------------------------------------------
 * - Macros or functions that should be deprecated in v5.0, then removed in the next major release
 * - Kept as not to cause a breaking change
 * - Include this header at the end of portmacro.h
 * ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief Disable interrupts in a nested manner
 *
 * Does the exact same thing as portSET_INTERRUPT_MASK_FROM_ISR()
 *
 * @deprecated This function is deprecated. Call portSET_INTERRUPT_MASK_FROM_ISR() instead
 */
static inline __attribute__((deprecated)) UBaseType_t portENTER_CRITICAL_NESTED(void) {
    return portSET_INTERRUPT_MASK_FROM_ISR();
}

/**
 * @brief Reenables interrupts in a nested manner
 *
 * Does the exact same thing as portCLEAR_INTERRUPT_MASK_FROM_ISR()
 *
 * @deprecated This function is deprecated. Call portCLEAR_INTERRUPT_MASK_FROM_ISR() instead
 */
static inline void __attribute__((deprecated)) portEXIT_CRITICAL_NESTED(UBaseType_t prev_level)
{
    portCLEAR_INTERRUPT_MASK_FROM_ISR(prev_level);
}
