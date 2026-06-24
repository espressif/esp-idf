/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_register_ops.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc_ulp.h"
#include "soc/sens_reg.h"
#include "ulp_riscv_cpu_freq_shared.h"

void ulp_riscv_rescue_from_monitor(void)
{
    /* Rescue RISCV from monitor state. */
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE | RTC_CNTL_COCPU_SHUT_RESET_EN);
}

void ulp_riscv_wakeup_main_processor(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_SW_CPU_INT);
}

void ulp_riscv_halt(void)
{
    /* Setting the delay time after RISCV recv `DONE` signal, Ensure that action `RESET` can be executed in time. */
    REG_SET_FIELD(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_2_CLK_DIS, 0x3F);

    /* Suspends the ulp operation and reset the ULP core. Must be the final operation before going to halt. */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE | RTC_CNTL_COCPU_SHUT_RESET_EN);

    while (1);
}

void ulp_riscv_timer_stop(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
}

void ulp_riscv_timer_resume(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
}

void ulp_riscv_gpio_wakeup_clear(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_GPIO_WAKEUP_CLR);
}

#if CONFIG_ULP_RISCV_INTERRUPT_ENABLE

void ulp_riscv_enable_sw_intr(intr_handler_t handler, void *arg)
{
    /* Enable ULP RISC-V SW interrupt */
    SET_PERI_REG_MASK(SENS_SAR_COCPU_INT_ENA_REG, SENS_COCPU_SW_INT_ENA);

    /* Register interrupt handler */
    if (handler) {
        ulp_riscv_intr_alloc(ULP_RISCV_SW_INTR_SOURCE, handler, arg);
    }
}

void ulp_riscv_disable_sw_intr(void)
{
    /* Disable ULP RISC-V SW interrupt */
    CLEAR_PERI_REG_MASK(SENS_SAR_COCPU_INT_ENA_REG, SENS_COCPU_SW_INT_ENA);

    /* De-register interrupt handler */
    ulp_riscv_intr_free(ULP_RISCV_SW_INTR_SOURCE);
}

void ulp_riscv_trigger_sw_intr(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SW_INT_TRIGGER);
}

#endif /* CONFIG_ULP_RISCV_INTERRUPT_ENABLE */

void ulp_riscv_delay_us(uint32_t delay_us)
{
#if CONFIG_IDF_TARGET_ESP32S3
    /*
     * For very small delays, entering the generic cycle-count loop adds too much fixed overhead.
     * Use a short calibrated NOP path instead to improve small-delay accuracy on ESP32-S3.
     */
    if (delay_us <= 5) {
        goto fast_return;
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    /*
     * Same principle as S3, but with a different threshold due to target-specific timing/overhead.
     */
    if (delay_us <= 10) {
        goto fast_return;
    }
#endif

    /*
     * Generic delay path:
     * - Convert requested microseconds to CPU cycles using ratio macros.
     * - Pre-subtract measured function overhead (55 cycles), so observed delay is closer to request.
     */
    uint32_t start = ulp_riscv_get_cpu_cycles() - 55U;
    uint32_t req_delay = delay_us * ULP_RISCV_CYCLES_PER_US_NUM / ULP_RISCV_CYCLES_PER_US_DENOM;

    /*
     * Busy-wait until elapsed cycles reach req_delay.
     * uint32_t subtraction intentionally relies on wrap-around-safe arithmetic for cycle counter rollover.
     */
    while ((uint32_t)(ulp_riscv_get_cpu_cycles() - start) < req_delay) {
        /* busy wait */
    }
    return;

fast_return:
#if CONFIG_IDF_TARGET_ESP32S3
    /*
     * Fast path for tiny delays:
     * Use discrete NOP counts calibrated for this target.
     * Note: (delay_us == 0 || delay_us <= 2) is intentionally kept as-is to avoid behavior changes.
     */
    if (delay_us == 0 || delay_us <= 2) {
        return;
    } else if (delay_us <= 3) {
        asm volatile("nop\n");
    } else if (delay_us <= 4) {
        asm volatile("nop\n nop\n");
    } else {
        asm volatile("nop\n nop\n nop\n nop\n nop\n");
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    /*
     * ESP32-S2 calibrated NOP mapping for very short delays.
     */
    if (delay_us == 0) {
        return;
    } else if (delay_us <= 6) {
        asm volatile("nop\n");
    } else if (delay_us <= 8) {
        asm volatile("nop\n nop\n");
    } else {
        asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n");
    }
#endif
}
