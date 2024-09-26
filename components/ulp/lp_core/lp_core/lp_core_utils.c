/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/soc_caps.h"
#include "riscv/csr.h"
#include "soc/soc.h"
#include "soc/pmu_reg.h"
#include "hal/misc.h"
#include "hal/lp_core_ll.h"
#include "hal/pmu_ll.h"
#include "hal/uart_ll.h"
#include "hal/rtc_io_ll.h"
#if SOC_LP_I2S_SUPPORT_VAD
//For VAD
#include "hal/lp_i2s_ll.h"
#endif

#if SOC_LP_TIMER_SUPPORTED
#include "hal/lp_timer_ll.h"
#endif

#include "esp_cpu.h"

/* LP_FAST_CLK is not very accurate, for now use a rough estimate */
#if CONFIG_RTC_FAST_CLK_SRC_RC_FAST
#define LP_CORE_CPU_FREQUENCY_HZ 16000000 // For P4 TRM says 20 MHz by default, but we tune it closer to 16 MHz
#elif CONFIG_RTC_FAST_CLK_SRC_XTAL
#if SOC_XTAL_SUPPORT_48M
#define LP_CORE_CPU_FREQUENCY_HZ 48000000
#else
#define LP_CORE_CPU_FREQUENCY_HZ 40000000
#endif
#else  // Default value in chip without rtc fast clock sel option
#define LP_CORE_CPU_FREQUENCY_HZ 16000000
#endif

static uint32_t lp_wakeup_cause = 0;

void ulp_lp_core_update_wakeup_cause(void)
{
    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_HP_CPU) \
            && (pmu_ll_lp_get_interrupt_raw(&PMU) & PMU_HP_SW_TRIGGER_INT_RAW)) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_HP_CPU;
        pmu_ll_lp_clear_intsts_mask(&PMU, PMU_HP_SW_TRIGGER_INT_CLR);
    }

    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_LP_UART) \
            && (uart_ll_get_intraw_mask(&LP_UART) & LP_UART_WAKEUP_INT_RAW)) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_LP_UART;
        uart_ll_clr_intsts_mask(&LP_UART, LP_UART_WAKEUP_INT_CLR);
    }

    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_LP_IO) \
            && rtcio_ll_get_interrupt_status()) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_LP_IO;
        rtcio_ll_clear_interrupt_status();
    }

#if SOC_LP_VAD_SUPPORTED
    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_LP_VAD)) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_LP_VAD;
        lp_i2s_ll_rx_clear_interrupt_status(&LP_I2S, LP_I2S_LL_EVENT_VAD_DONE_INT);
    }
#endif

#if SOC_ETM_SUPPORTED
    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_ETM) \
            && lp_core_ll_get_etm_wakeup_flag()) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_ETM;
#if CONFIG_IDF_TARGET_ESP32P4
        lp_core_ll_clear_etm_wakeup_status();
#else
        lp_core_ll_clear_etm_wakeup_flag();
#endif
    }
#endif /* SOC_ETM_SUPPORTED */

#if SOC_LP_TIMER_SUPPORTED
    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER) \
            && (lp_timer_ll_get_lp_intr_raw(&LP_TIMER) & LP_TIMER_MAIN_TIMER_LP_INT_RAW)) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER;
        lp_timer_ll_clear_lp_intsts_mask(&LP_TIMER, LP_TIMER_MAIN_TIMER_LP_INT_CLR);
    }
#endif /* SOC_LP_TIMER_SUPPORTED */

}

uint32_t ulp_lp_core_get_wakeup_cause()
{
    return lp_wakeup_cause;
}

/**
 * @brief Wakeup main CPU from sleep or deep sleep.
 *
 * This raises a software interrupt signal, if the
 * main CPU has configured the ULP as a wakeup source
 * calling this function will make the main CPU to
 * exit from sleep or deep sleep.
 */
void ulp_lp_core_wakeup_main_processor(void)
{
    REG_SET_FIELD(PMU_HP_LP_CPU_COMM_REG, PMU_LP_TRIGGER_HP, 1);
}

/**
 * @brief Makes the co-processor busy wait for a certain number of microseconds
 *
 * @param us Number of microseconds to busy-wait for
 */
void ulp_lp_core_delay_us(uint32_t us)
{
    uint32_t start = RV_READ_CSR(mcycle);
    uint32_t end = us * (LP_CORE_CPU_FREQUENCY_HZ / 1000000);

    while ((RV_READ_CSR(mcycle) - start) < end) {
        /* nothing to do */
    }
}

/**
 * @brief Makes the co-processor busy wait for a certain number of cycles
 *
 * @param cycles Number of cycles to busy-wait for
 */
void ulp_lp_core_delay_cycles(uint32_t cycles)
{
    uint32_t start = RV_READ_CSR(mcycle);
    uint32_t end = cycles;

    while ((RV_READ_CSR(mcycle) - start) < end) {
        /* nothing to do */
    }
}

void ulp_lp_core_halt(void)
{
    lp_core_ll_request_sleep();

    while (1);
}

void ulp_lp_core_stop_lp_core(void)
{
    /* Disable wake-up source and put lp core to sleep */
    lp_core_ll_set_wakeup_source(0);
    lp_core_ll_request_sleep();
}

void __attribute__((noreturn)) abort(void)
{
    // By calling abort users expect some panic message to be printed,
    // so cause an exception like it is done in HP core's version of abort().
    // If CONFIG_ULP_PANIC_OUTPUT_ENABLE is YES then panic handler will print smth
    // If debugger is attached it will stop here and user can inspect the backtrace.
    esp_cpu_dbgr_break();
    while (1); // to make compiler happy about noreturn attr
}

void __attribute__((noreturn)) ulp_lp_core_abort(void)
{
    /* Stop the LP Core */
    ulp_lp_core_stop_lp_core();

    while (1);
}

void ulp_lp_core_sw_intr_enable(bool enable)
{
    pmu_ll_lp_enable_sw_intr(&PMU, enable);
}

void ulp_lp_core_sw_intr_clear(void)
{
    pmu_ll_lp_clear_sw_intr_status(&PMU);
}

void ulp_lp_core_wait_for_intr(void)
{
    asm volatile("wfi");
}
