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

/* ---------------------- Spinlocks --------------------- */

/**
 * @brief Deprecated placed holder function to initialize a spinlock
 *
 * Currently does nothing.
 *
 * @deprecated This function is deprecated. If on multi-core, use spinlock_initialize() instead
 * @param[in] mux Spinlock
 */
static inline void __attribute__((deprecated)) __attribute__((always_inline)) vPortCPUInitializeMutex(portMUX_TYPE *mux)
{
    (void)mux;
}

/**
 * @brief Deprecated placed holder function to acquire a spinlock
 *
 * Currently does nothing.
 *
 * @deprecated This function is deprecated. If on multi-core, use spinlock_acquire() instead
 * @param[in] mux Spinlock
 */
static inline void __attribute__((deprecated)) __attribute__((always_inline)) vPortCPUAcquireMutex(portMUX_TYPE *mux)
{
    (void)mux;
}

/**
 * @brief Deprecated placed holder function to acquire a spinlock but with a specified timeout
 *
 * Currently just returns true
 *
 * @deprecated This function is deprecated. If on multi-core, use spinlock_acquire() instead
 * @note Does not have deprecated attribute due to usage in app_trace_util.c
 * @param[in] mux Spinlock
 * @param[in] timeout Timeout in number of CPU cycles
 * @return true Always returns true
 */
static inline bool __attribute__((always_inline))  vPortCPUAcquireMutexTimeout(portMUX_TYPE *mux, int timeout_cycles)
{
    (void)mux;
    (void)timeout_cycles;
    return true;
}

/**
 * @brief Deprecated placed holder function to release a spinlock
 *
 * Currently does nothing.
 *
 * @deprecated This function is deprecated. If on multi-core, use spinlock_release() instead
 * @note Does not have deprecated attribute due to usage in app_trace_util.c
 * @param[in] mux Spinlock
 */
static inline void __attribute__((always_inline)) vPortCPUReleaseMutex(portMUX_TYPE *mux)
{
    (void)mux;
}
