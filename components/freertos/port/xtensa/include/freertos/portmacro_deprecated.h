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
 * @brief Initialize a spinlock
 *
 * Does the exact same thing as spinlock_initialize();
 *
 * @deprecated This function is deprecated. Call spinlock_initialize() instead
 * @param[in] mux Spinlock
 */
static inline void __attribute__((deprecated)) __attribute__((always_inline)) vPortCPUInitializeMutex(portMUX_TYPE *mux)
{
    spinlock_initialize(mux);
}

/**
 * @brief Acquire a spinlock
 *
 * Does the exact same thing as spinlock_acquire() with unlimited timeout
 *
 * @deprecated This function is deprecated. Call spinlock_acquire() instead
 * @param[in] mux Spinlock
 */
static inline void __attribute__((deprecated)) __attribute__((always_inline)) vPortCPUAcquireMutex(portMUX_TYPE *mux)
{
    spinlock_acquire(mux, portMUX_NO_TIMEOUT);
}

/**
 * @brief Acquire a spinlock
 *
 * Does the exact same thing as spinlock_acquire() with a specified timeout
 *
 * @deprecated This function is deprecated. Call spinlock_acquire() instead
 * @note Does not have deprecated attribute due to usage in app_trace_util.c
 * @param[in] mux Spinlock
 * @param timeout
 * @return true Spinlock acquired
 * @return false Timed out
 */
static inline bool __attribute__((always_inline)) vPortCPUAcquireMutexTimeout(portMUX_TYPE *mux, int timeout)
{
    return (spinlock_acquire(mux, timeout));
}

/**
 * @brief Release a spinlock
 *
 * Does the exact same thing as spinlock_release()
 *
 * @deprecated This function is deprecated. Call spinlock_release() instead
 * @note Does not have deprecated attribute due to usage in app_trace_util.c
 * @param[in] mux Spinlock
 */
static inline void __attribute__((always_inline)) vPortCPUReleaseMutex(portMUX_TYPE *mux)
{
    spinlock_release(mux);
}
