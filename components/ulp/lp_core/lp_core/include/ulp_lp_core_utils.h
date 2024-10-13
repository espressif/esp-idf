/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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

/**
 * @brief Traverse all possible wake-up sources and update the wake-up cause so that
 *        ulp_lp_core_get_wakeup_cause can obtain the bitmap of the wake-up reasons.
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
 * @brief Makes the co-processor busy wait for a certain number of microseconds
 *
 * @param us Number of microseconds to busy-wait for
 */
void ulp_lp_core_delay_us(uint32_t us);

/**
 * @brief Makes the co-processor busy wait for a certain number of cycles
 *
 * @param cycles Number of cycles to busy-wait for
 */
void ulp_lp_core_delay_cycles(uint32_t cycles);

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
 * @brief Enable the SW triggered interrupt from the PMU
 *
 * @note This is the same SW trigger interrupt that is used to wake up the LP CPU
 *
 * @param enable true to enable, false to disable
 *
 */
void ulp_lp_core_sw_intr_enable(bool enable);

/**
 * @brief Clear the interrupt status for the SW triggered interrupt from the PMU
 *
 */
void ulp_lp_core_sw_intr_clear(void);

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
