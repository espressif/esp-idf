/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LP_CORE_SINGLE_INTERRUPT_VECTOR
#define LP_CORE_ISR_ATTR // On chips with just a single interrupt entry point registers are saved by us before calling the ISR
#else
#define LP_CORE_ISR_ATTR __attribute__((interrupt))
#endif

/**
  *  Available interrupt handlers for the low power core are as follows:
  *
  *  ulp_lp_core_lp_io_intr_handler(void);
  *  ulp_lp_core_lp_i2c_intr_handler(void);
  *  ulp_lp_core_lp_uart_intr_handler(void);
  *  ulp_lp_core_lp_timer_intr_handler(void);
  *  ulp_lp_core_lp_pmu_intr_handler(void);
  *  ulp_lp_core_lp_spi_intr_handler(void);
  *  ulp_lp_core_trng_intr_handler(void);
  *  ulp_lp_core_lp_adc_intr_handler(void);
  *  ulp_lp_core_lp_touch_intr_handler(void);
  *  ulp_lp_core_tsens_intr_handler(void);
  *  ulp_lp_core_efuse_intr_handler(void);
  *  ulp_lp_core_lp_sysreg_intr_handler(void);
  *  ulp_lp_core_lp_ana_peri_intr_handler(void);
  *  ulp_lp_core_mailbox_intr_handler(void);
  *  ulp_lp_core_lp_wdt_intr_handler(void);
  *  ulp_lp_core_lp_rtc_intr_handler(void);
  *  ulp_lp_core_sw_intr_handler(void);
  *
  * Not all handlers are available on all chips. Please refer to the Technical Reference Manual for your chip for more information.
*/

/**
 * @brief Enables interrupts globally for the low power core
 *
 */
void ulp_lp_core_intr_enable(void);

/**
 * @brief Disables interrupts globally for the low power core
 *
 */
void ulp_lp_core_intr_disable(void);

#ifdef __cplusplus
}
#endif
