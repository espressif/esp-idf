/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"
#include "ulp_riscv.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "hal/misc.h"
#include "ulp_common.h"
#include "esp_rom_sys.h"
#include "esp_check.h"
#include "esp_private/rtc_ctrl.h"

__attribute__((unused)) static const char* TAG = "ulp-riscv";

esp_err_t ulp_riscv_isr_register(intr_handler_t fn, void *arg, uint32_t mask)
{
    /* Verify that the ISR callback is valid */
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TAG, "ULP RISC-V ISR is NULL");

    /* Verify that the interrupt bits are valid */
    if (!(mask & (RTC_CNTL_COCPU_INT_ST_M | RTC_CNTL_COCPU_TRAP_INT_ST_M))) {
        ESP_LOGE(TAG, "Invalid bitmask for ULP RISC-V interrupts");
        return ESP_ERR_INVALID_ARG;
    }

    /* Make sure we enable only the  ULP interrupt bits.
     * We don't want other RTC interrupts triggering this ISR.
     */
    mask &= (RTC_CNTL_COCPU_INT_ST_M | RTC_CNTL_COCPU_TRAP_INT_ST_M);

    /* Register the RTC ISR */
    ESP_RETURN_ON_ERROR(rtc_isr_register(fn, arg, mask, 0), TAG, "rtc_isr_register() failed");

    /* Enable the interrupt bits */
    SET_PERI_REG_MASK(RTC_CNTL_INT_ENA_REG, mask);

    return ESP_OK;
}

esp_err_t ulp_riscv_isr_deregister(intr_handler_t fn, void *arg, uint32_t mask)
{
    /* Verify that the ISR callback is valid */
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TAG, "ULP RISC-V ISR is NULL");

    /* Verify that the interrupt bits are valid */
    if (!(mask & (RTC_CNTL_COCPU_INT_ST_M | RTC_CNTL_COCPU_TRAP_INT_ST_M))) {
        ESP_LOGE(TAG, "Invalid bitmask for ULP RISC-V interrupts");
        return ESP_ERR_INVALID_ARG;
    }

    /* Make sure we disable only the ULP interrupt bits */
    mask &= (RTC_CNTL_COCPU_INT_ST_M | RTC_CNTL_COCPU_TRAP_INT_ST_M);

    /* Disable the interrupt bits */
    CLEAR_PERI_REG_MASK(RTC_CNTL_INT_ENA_REG, mask);

    /* Deregister the RTC ISR */
    ESP_RETURN_ON_ERROR(rtc_isr_deregister(fn, arg), TAG, "rtc_isr_deregister() failed");

    return ESP_OK;
}

static esp_err_t ulp_riscv_config_wakeup_source(ulp_riscv_wakeup_source_t wakeup_source)
{
    esp_err_t ret = ESP_OK;

    switch (wakeup_source) {
        case ULP_RISCV_WAKEUP_SOURCE_TIMER:
            /* start ULP_TIMER */
            CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG, RTC_CNTL_ULP_CP_FORCE_START_TOP);
            SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
            break;

        case ULP_RISCV_WAKEUP_SOURCE_GPIO:
            SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_GPIO_WAKEUP_ENA);
            break;

        default:
            ret = ESP_ERR_INVALID_ARG;
    }

    return ret;
}

esp_err_t ulp_riscv_config_and_run(ulp_riscv_cfg_t* cfg)
{
    esp_err_t ret = ESP_OK;


#if CONFIG_IDF_TARGET_ESP32S2
    /* Reset COCPU when power on. */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);

     /* The coprocessor cpu trap signal doesnt have a stable reset value,
       force ULP-RISC-V clock on to stop RTC_COCPU_TRAP_TRIG_EN from waking the CPU*/
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_CLK_FO);

    /* Disable ULP timer */
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    /* wait for at least 1 RTC_SLOW_CLK cycle */
    esp_rom_delay_us(20);
    /* Select RISC-V as the ULP_TIMER trigger target. */
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SEL);

    /* Select ULP-RISC-V to send the DONE signal. */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE_FORCE);

    ret = ulp_riscv_config_wakeup_source(cfg->wakeup_source);

#elif CONFIG_IDF_TARGET_ESP32S3
    /* Reset COCPU when power on. */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);

     /* The coprocessor cpu trap signal doesnt have a stable reset value,
       force ULP-RISC-V clock on to stop RTC_COCPU_TRAP_TRIG_EN from waking the CPU*/
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_CLK_FO);


    /* Disable ULP timer */
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    /* wait for at least 1 RTC_SLOW_CLK cycle */
    esp_rom_delay_us(20);

    /* We do not select RISC-V as the Coprocessor here as this could lead to a hang
     * in the main CPU. Instead, we reset RTC_CNTL_COCPU_SEL after we have enabled the ULP timer.
     *
     * IDF-4510
     */
    //CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SEL);

    /* Select ULP-RISC-V to send the DONE signal */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE_FORCE);

    /* Set the CLKGATE_EN signal */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_CLKGATE_EN);

    ret = ulp_riscv_config_wakeup_source(cfg->wakeup_source);

    /* Select RISC-V as the ULP_TIMER trigger target
     * Selecting the RISC-V as the Coprocessor at the end is a workaround
     * for the hang issue recorded in IDF-4510.
     */
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SEL);

    /* Clear any spurious wakeup trigger interrupts upon ULP startup */
    esp_rom_delay_us(20);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, RTC_CNTL_COCPU_INT_CLR | RTC_CNTL_COCPU_TRAP_INT_CLR | RTC_CNTL_ULP_CP_INT_CLR);


#endif

    return ret;
}

esp_err_t ulp_riscv_run(void)
{
    ulp_riscv_cfg_t cfg = ULP_RISCV_DEFAULT_CONFIG();
    return ulp_riscv_config_and_run(&cfg);
}

void ulp_riscv_timer_stop(void)
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
}

void ulp_riscv_timer_resume(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
}

void ulp_riscv_halt(void)
{
    ulp_riscv_timer_stop();

    /* suspends the ulp operation*/
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE);

    /* Resets the processor */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);
}

void ulp_riscv_reset()
{
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT | RTC_CNTL_COCPU_DONE);
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);
    esp_rom_delay_us(20);
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT | RTC_CNTL_COCPU_DONE);
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);
}

esp_err_t ulp_riscv_load_binary(const uint8_t* program_binary, size_t program_size_bytes)
{
    if (program_binary == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (program_size_bytes > CONFIG_ULP_COPROC_RESERVE_MEM) {
        return ESP_ERR_INVALID_SIZE;
    }

    uint8_t* base = (uint8_t*) RTC_SLOW_MEM;

    //Start by clearing memory reserved with zeros, this will also will initialize the bss:
    hal_memset(base, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
    hal_memcpy(base, program_binary, program_size_bytes);

    return ESP_OK;
}
