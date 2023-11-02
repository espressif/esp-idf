/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <stdint.h>
#include "riscv/csr.h"
#include "soc/soc.h"
#include "soc/pmu_reg.h"

/* LP_FAST_CLK is not very accurate, for now use a rough estimate */
#if CONFIG_IDF_TARGET_ESP32C6
#define LP_CORE_CPU_FREQUENCY_HZ 16000000
#elif CONFIG_IDF_TARGET_ESP32P4
#define LP_CORE_CPU_FREQUENCY_HZ 20000000
#endif

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
