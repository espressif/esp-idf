/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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

#if SOC_RTC_TIMER_SUPPORTED
#include "hal/rtc_timer_ll.h"
#endif

#include "esp_cpu.h"
#include "ulp_lp_core_cpu_freq_shared.h"

static uint32_t lp_wakeup_cause = 0;

void ulp_lp_core_update_wakeup_cause(void)
{
    lp_wakeup_cause = 0;
    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_HP_CPU) \
            && (pmu_ll_lp_get_interrupt_raw(&PMU) & PMU_HP_SW_TRIGGER_INT_RAW)) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_HP_CPU;
        pmu_ll_lp_clear_intsts_mask(&PMU, PMU_HP_SW_TRIGGER_INT_CLR);
    }

#if SOC_ULP_LP_UART_SUPPORTED
    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_LP_UART) \
            && (uart_ll_get_intraw_mask(&LP_UART) & LP_UART_WAKEUP_INT_RAW)) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_LP_UART;
        uart_ll_clr_intsts_mask(&LP_UART, LP_UART_WAKEUP_INT_CLR);
    }
#endif

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

#if SOC_RTC_TIMER_SUPPORTED
    if ((lp_core_ll_get_wakeup_source() & LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER) \
            && (rtc_timer_ll_get_intr_raw(&LP_TIMER, 1) & LP_TIMER_MAIN_TIMER_LP_INT_RAW)) {
        lp_wakeup_cause |= LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER;
        rtc_timer_ll_clear_alarm_intr_status(&LP_TIMER, 1);
    }
#endif /* SOC_RTC_TIMER_SUPPORTED */

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
    if (us == 0) {
        return;
    }
    uint32_t start = RV_READ_CSR(mcycle) - ULP_LP_CORE_DELAY_CALL_OVERHEAD_IN_CYCLES;
    uint32_t req_delay = us * LP_CORE_CYCLES_PER_US_NUM / LP_CORE_CYCLES_PER_US_DENOM;

    while ((uint32_t)(RV_READ_CSR(mcycle) - start) < req_delay) {
        /* busy wait */
    }
}

/**
 * @brief Makes the co-processor busy wait for a certain number of cycles
 *
 * @param cycles Number of cycles to busy-wait for
 */
void ulp_lp_core_delay_cycles(uint32_t cycles)
{
    if (cycles <= ULP_LP_CORE_DELAY_CALL_OVERHEAD_IN_CYCLES) {
        return;
    }

    uint32_t start = RV_READ_CSR(mcycle) - ULP_LP_CORE_DELAY_CALL_OVERHEAD_IN_CYCLES;
    while ((uint32_t)(RV_READ_CSR(mcycle) - start) < cycles) {
        /* busy wait */
    }
}

#if SOC_ULP_LP_UART_SUPPORTED

void ulp_lp_core_lp_uart_reset_wakeup_en(void)
{
    lp_core_ll_enable_lp_uart_wakeup(false);
    lp_core_ll_enable_lp_uart_wakeup(true);
}
#endif

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

void ulp_lp_core_sw_intr_to_hp_trigger(void)
{
    pmu_ll_lp_trigger_sw_intr(&PMU);
}

void ulp_lp_core_sw_intr_from_hp_enable(bool enable)
{
    pmu_ll_lp_enable_sw_intr(&PMU, enable);
}

void ulp_lp_core_sw_intr_from_hp_clear(void)
{
    pmu_ll_lp_clear_sw_intr_status(&PMU);
}

#if SOC_RTC_TIMER_SUPPORTED
void ulp_lp_core_lp_timer_intr_enable(bool enable)
{
    rtc_timer_ll_alarm_intr_enable(&LP_TIMER, 1, enable);
}

void ulp_lp_core_lp_timer_intr_clear(void)
{
    rtc_timer_ll_clear_alarm_intr_status(&LP_TIMER, 1);
}
#endif

void ulp_lp_core_wait_for_intr(void)
{
    asm volatile("wfi");
}
