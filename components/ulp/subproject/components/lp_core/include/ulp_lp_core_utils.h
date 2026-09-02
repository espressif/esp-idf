/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "riscv/csr.h"
#include "soc/soc_caps.h"

/**
 * @brief Traverse all possible wake-up sources and update the wake-up cause so that
 *        ulp_lp_core_get_wakeup_cause can obtain the bitmap of the wake-up reasons.
 * @note Do not call it from user ULP programs because it will clear the wake-up cause bits
 *       which were set at ULP startup in lp_core_startup().
 */
void ulp_lp_core_update_wakeup_cause(void);

/**
 * @brief Get the wakeup source which caused LP_CPU to wakeup from sleep
 *
 * @return  Wakeup cause in bit map, for the meaning of each bit, refer
 *          to the definition of wakeup source in lp_core_ll.h
 */
uint32_t ulp_lp_core_get_wakeup_cause(void);

/**
 * @brief Wakeup main CPU from sleep or deep sleep.
 *
 * This raises a software interrupt signal, if the
 * main CPU has configured the ULP as a wakeup source
 * calling this function will make the main CPU to
 * exit from sleep or deep sleep.
 */
void ulp_lp_core_wakeup_main_processor(void);

/**
 * @brief Retrieves the current number of CPU cycles.
 *
 * @return The current CPU cycle count.
 */
static inline uint32_t ulp_lp_core_get_cpu_cycles(void)
{
    return RV_READ_CSR(mcycle);
}

/**
 * @brief Check whether an mcycle-based timeout has elapsed.
 *
 * @note A timeout value of -1 means "wait forever".
 *       Other values are interpreted as unsigned cycle counts.
 *
 * @param start_cycle_count Cycle counter value captured at timeout start.
 * @param cycles_to_wait Timeout in CPU cycles, or -1 to disable timeout.
 *
 * @return true if timeout elapsed, false otherwise.
 */
static inline bool ulp_lp_core_is_timeout_elapsed(uint32_t start_cycle_count, int32_t cycles_to_wait)
{
    if (cycles_to_wait == -1) {
        return false;
    }

    return (ulp_lp_core_get_cpu_cycles() - start_cycle_count) >= (uint32_t)cycles_to_wait;
}

/**
 * @brief Makes the co-processor busy-wait for a certain number of microseconds.
 *
 * @note The maximum supported delay depends on the LP core clock source and frequency.
 *       For values above the limits below, the computed delay may overflow and the result
 *       is undefined.
 *       - LP core @ 16 MHz (RC_FAST / default): us must be <= 134217727 (about 134.2 s)
 *       - LP core @ 40 MHz (XTAL 40 MHz):       us must be <= 53687091  (about  53.7 s)
 *       - LP core @ 48 MHz (XTAL 48 MHz):       us must be <= 44739242  (about  44.7 s)
 *
 * @param us Number of microseconds to busy-wait for
 */
void ulp_lp_core_delay_us(uint32_t us);

/**
 * @brief Makes the co-processor busy-wait for a certain number of cycles.
 *
 * @note The maximum supported delay is 0x7FFFFFFF cycles.
 *       For larger values, the behavior is undefined. Split longer delays into smaller
 *       chunks if needed.
 *
 * For reference, this corresponds approximately to:
 *       - LP core @ 16 MHz (RC_FAST / default): 0x7FFFFFFF cycles ≈ 134.2 s
 *       - LP core @ 40 MHz (XTAL 40 MHz):       0x7FFFFFFF cycles ≈  53.7 s
 *       - LP core @ 48 MHz (XTAL 48 MHz):       0x7FFFFFFF cycles ≈  44.7 s
 *
 * @param cycles Number of cycles to busy-wait for
 */
void ulp_lp_core_delay_cycles(uint32_t cycles);

#if SOC_ULP_LP_UART_SUPPORTED
/**
 * @brief Reset LP CORE uart wakeup enable.
 */
void ulp_lp_core_lp_uart_reset_wakeup_en(void);
#endif

/**
 * @brief Finishes the ULP program and powers down the ULP
 *        until next wakeup.
 *
 * @note This function does not return. After called it will
 *       fully reset the ULP.
 *
 * @note The program will automatically call this function when
 *       returning from main().
 *
 * @note To stop the ULP from waking up, call ulp_lp_core_lp_timer_disable()
 *       before halting.
 *
 */
__attribute__((__noreturn__))  void ulp_lp_core_halt(void);

/**
 * @brief The LP core puts itself to sleep and disables all wakeup sources.
 */
__attribute__((__noreturn__))  void ulp_lp_core_stop_lp_core(void);

/**
 * @brief Abort LP core operation.
 */
void __attribute__((noreturn)) ulp_lp_core_abort(void);

/**
 * @brief Trigger a software interrupt on the HP core
 */
void ulp_lp_core_sw_intr_to_hp_trigger(void);

/**
 * @brief Enable the SW triggered interrupt from the PMU
 *
 * @note This is the same SW trigger interrupt that is used to wake up the LP CPU
 *
 * @param enable true to enable, false to disable
 *
 */
void ulp_lp_core_sw_intr_from_hp_enable(bool enable);

/**
 * @brief Enable the SW triggered interrupt from the PMU
 *
 * @note Alias for the `ulp_lp_core_sw_intr_from_hp_enable` function, for backward compatibility.
 *
 * @param enable true to enable, false to disable
 */
static inline void ulp_lp_core_sw_intr_enable(bool enable)
{
    return ulp_lp_core_sw_intr_from_hp_enable(enable);
}

/**
 * @brief Clear the interrupt status for the SW triggered interrupt from the PMU
 */
void ulp_lp_core_sw_intr_from_hp_clear(void);

/**
 * @brief Clear the interrupt status for the SW triggered interrupt from the PMU
 *
 * @note Alias for the `ulp_lp_core_sw_intr_from_hp_clear` function, for backward compatibility.
 */
static inline void ulp_lp_core_sw_intr_clear(void)
{
    return ulp_lp_core_sw_intr_from_hp_clear();
}

#if SOC_RTC_TIMER_SUPPORTED
/**
 * @brief Enable the LP Timer interrupt
 *
 */
void ulp_lp_core_lp_timer_intr_enable(bool enable);

/**
 * @brief Clear the interrupt status for the LP Timer interrupt
 *
 */
void ulp_lp_core_lp_timer_intr_clear(void);
#endif

/**
 * @brief Puts the CPU into a wait state until an interrupt is triggered
 *
 * @note The CPU will draw less power when in this state compared to actively running
 *
 */
void ulp_lp_core_wait_for_intr(void);

#ifdef __cplusplus
}
#endif
