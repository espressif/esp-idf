/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_riscv_utils.h"
#include "ulp_riscv_register_ops.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc_ulp.h"
#include "soc/sens_reg.h"

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
