/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdint.h>
#include "riscv/csr.h"
#include "soc/soc.h"
#include "soc/pmu_reg.h"
#include "hal/misc.h"
#include "ulp_lp_core.h"
#include "hal/etm_ll.h"
#include "hal/lp_timer_ll.h"
#include "hal/pmu_ll.h"
#include "hal/uart_ll.h"
#include "hal/rtc_io_ll.h"

/* LP_FAST_CLK is not very accurate, for now use a rough estimate */
#define LP_CORE_CPU_FREQUENCY_HZ 16000000

static uint32_t lp_wakeup_cause = 0;

void ulp_lp_core_update_wakeup_cause(void)
{
    if ((REG_GET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN) & ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU) \
        && (pmu_ll_lp_get_interrupt_raw(&PMU) & PMU_HP_SW_TRIGGER_INT_RAW)) {
        lp_wakeup_cause |= ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU;
        pmu_ll_lp_clear_intsts_mask(&PMU, PMU_HP_SW_TRIGGER_INT_CLR);
    }

    if ((REG_GET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN) & ULP_LP_CORE_WAKEUP_SOURCE_LP_UART) \
        && (uart_ll_get_intraw_mask(&LP_UART) & LP_UART_WAKEUP_INT_RAW)) {
        lp_wakeup_cause |= ULP_LP_CORE_WAKEUP_SOURCE_LP_UART;
        uart_ll_clr_intsts_mask(&LP_UART, LP_UART_WAKEUP_INT_CLR);
    }

    if ((REG_GET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN) & ULP_LP_CORE_WAKEUP_SOURCE_LP_IO) \
        && rtcio_ll_get_interrupt_status()) {
        lp_wakeup_cause |= ULP_LP_CORE_WAKEUP_SOURCE_LP_IO;
        rtcio_ll_clear_interrupt_status();
    }

    if ((REG_GET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN) & ULP_LP_CORE_WAKEUP_SOURCE_ETM) \
        && etm_ll_is_lpcore_wakeup_triggered()) {
        lp_wakeup_cause |= ULP_LP_CORE_WAKEUP_SOURCE_ETM;
        etm_ll_clear_lpcore_wakeup_status();
    }

    if ((REG_GET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN) & ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER) \
        && (lp_timer_ll_get_lp_intr_raw(&LP_TIMER) & LP_TIMER_MAIN_TIMER_LP_INT_RAW)) {
        lp_wakeup_cause |= ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER;
        lp_timer_ll_clear_lp_intsts_mask(&LP_TIMER, LP_TIMER_MAIN_TIMER_LP_INT_CLR);
    }
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
    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_SLEEP_REQ, 1);

    while(1);
}

void ulp_lp_core_stop_lp_core(void)
{
    /* Disable wake-up source and put lp core to sleep */
    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN, 0);
    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_SLEEP_REQ, 1);
}

void __attribute__((noreturn)) abort(void)
{
    /* Stop the LP Core */
    ulp_lp_core_stop_lp_core();

    while (1);
}
