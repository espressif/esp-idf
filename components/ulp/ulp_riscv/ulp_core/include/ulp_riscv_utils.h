/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"
#include <stdint.h>
#include "ulp_riscv_register_ops.h"
#include "ulp_riscv_interrupt.h"
#include "ulp_riscv_cpu_freq_shared.h"

/**
 * @brief Wakeup main CPU from sleep or deep sleep.
 *
 * This raises a software interrupt signal, if the
 * main CPU has configured the ULP as a wakeup source
 * calling this function will make the main CPU to
 * exit from sleep or deep sleep.
 */
void ulp_riscv_wakeup_main_processor(void);

/**
 * @brief Rescues the cpu from monitor mode
 *
 * This function cancels the low power mode
 * of the ULP-RISC-V, should be called
 * every time the co-processor starts.
 *
 * @note by convenience this function is
 *       automatically called in startup code.
 */
void ulp_riscv_rescue_from_monitor(void);

/**
 * @brief Finishes the ULP program and powers down the ULP
 *        until next wakeup.
 *
 * @note This function does not return. After called it will
 *       fully reset the ULP.
 *
 * @note Returning from main() in the ULP program results on
 *       calling this function.
 *
 * @note To stop the ULP from waking up, call ulp_riscv_timer_stop()
 *       before halting.
 *
 * This function should be called after the ULP program Finishes
 * its processing, it will trigger the timer for the next wakeup,
 * put the ULP in monitor mode and triggers a reset.
 *
 */
void __attribute__((__noreturn__)) ulp_riscv_halt(void);

#define ulp_riscv_shutdown ulp_riscv_halt

/**
 * @brief Stop the ULP timer
 *
 * @note This will stop the ULP from waking up if halted, but will not abort any program
 *       currently executing on the ULP.
 */
void ulp_riscv_timer_stop(void);

/**
 * @brief Resumes the ULP timer
 *
 * @note This will resume an already configured timer, but does no other configuration
 *
 */
void ulp_riscv_timer_resume(void);

#define ULP_RISCV_GET_CCOUNT()  ({ int __ccount; \
                asm volatile("rdcycle %0;" : "=r"(__ccount)); \
                __ccount; })

#define ULP_RISCV_CYCLES_PER_US ULP_RISCV_CYCLES_PER_US_NUM / ULP_RISCV_CYCLES_PER_US_DENOM
#define ULP_RISCV_CYCLES_PER_MS 1000U * ULP_RISCV_CYCLES_PER_US

/**
 * @brief Retrieves the current number of CPU cycles.
 *
 * @return The current CPU cycle count.
 */
static inline uint32_t ulp_riscv_get_cpu_cycles(void)
{
    return ULP_RISCV_GET_CCOUNT();
}

/**
 * @brief Makes the co-processor busy-wait for a certain number of CPU cycles.
 *
 * @note This function is not accurate for delays shorter than 20 cycles because the
 *       function overhead may exceed the requested delay.
 *
 * @note The maximum supported delay is 0x7FFFFFFF cycles.
 *       For larger values, the behavior is undefined. Split longer delays into smaller
 *       chunks if needed.
 *
 * For reference, this corresponds approximately to:
 *       - ESP32-S2 ULP-RISC-V @ 8.5 MHz:  0x7FFFFFFF cycles ≈ 252.645 s
 *       - ESP32-S3 ULP-RISC-V @ 17.5 MHz: 0x7FFFFFFF cycles ≈ 122.713 s
 *
 * @param cycles Number of cycles to busy-wait.
 */
static inline void ulp_riscv_delay_cycles(uint32_t cycles)
{
    if (cycles <= 20U) { // estimate of cycles for this function overhead
        return;
    }
    // To improve accuracy subtract (20 + 15) cycles overhead, defined by delay calibration test
    uint32_t start = ULP_RISCV_GET_CCOUNT() - 20U - 15U;
    while ((uint32_t)(ULP_RISCV_GET_CCOUNT() - start) < cycles) {
        /* busy wait */
    }
}

/**
 * @brief Makes the co-processor busy-wait for a certain number of microseconds.
 *
 * @note This function is not accurate for short delays because the function overhead
 *       may exceed the requested delay. For very small delays the implementation uses
 *       a fixed sequence of NOPs (chip-dependent thresholds).
 *
 * @note The maximum supported delay depends on the ULP-RISC-V cycle counter width and on
 *       the internal cycles-per-microsecond conversion. For values above the limits below,
 *       the computed delay may overflow and the result is undefined.
 *       - ESP32-S2 ULP-RISC-V @ 8.5 MHz: delay_us must be <= 252645135 (about 252.6 s)
 *       - ESP32-S3 ULP-RISC-V @ 17.5 MHz: delay_us must be <= 122713351 (about 122.7 s)
 *
 * @param delay_us Number of microseconds to busy wait.
 */
void ulp_riscv_delay_us(uint32_t delay_us);

/**
 * @brief Clears the GPIO wakeup interrupt bit
 *
 */
void ulp_riscv_gpio_wakeup_clear(void);

#if CONFIG_ULP_RISCV_INTERRUPT_ENABLE
/**
 * @brief Enable ULP RISC-V SW Interrupt
 *
 * @param handler       Interrupt handler
 * @param arg           Interrupt handler argument
 */
void ulp_riscv_enable_sw_intr(intr_handler_t handler, void *arg);

/**
 * @brief Disable ULP RISC-V SW Interrupt
 */
void ulp_riscv_disable_sw_intr(void);

/**
 * @brief Trigger ULP RISC-V SW Interrupt
 *
 * @note The SW interrupt will only trigger if it has been enabled previously using ulp_riscv_enable_sw_intr().
 */
void ulp_riscv_trigger_sw_intr(void);

/**
 * @brief Enter a critical section by disabling all interrupts
 *        This inline assembly construct uses the t0 register and is equivalent to:
 *
 *        li t0, 0x80000007
 *        maskirq_insn(zero, t0)    // Mask all interrupt bits
 */
#define ULP_RISCV_ENTER_CRITICAL()  \
    asm volatile (                  \
        "li t0, 0x80000007\n"       \
        ".word 0x0602e00b"          \
    );                              \

/**
 * @brief Exit a critical section by enabling all interrupts
 *        This inline assembly construct is equivalent to:
 *
 *        maskirq_insn(zero, zero)  // Unmask all interrupt bits
 */
#define ULP_RISCV_EXIT_CRITICAL()   asm volatile (".word 0x0600600b");

#else /* CONFIG_ULP_RISCV_INTERRUPT_ENABLE */

#define ULP_RISCV_ENTER_CRITICAL()
#define ULP_RISCV_EXIT_CRITICAL()

#endif /* CONFIG_ULP_RISCV_INTERRUPT_ENABLE */

#ifdef __cplusplus
}
#endif
