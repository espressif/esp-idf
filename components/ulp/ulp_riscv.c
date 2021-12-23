/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
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
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/clk.h"
#include "esp32s2/ulp.h"
#include "esp32s2/ulp_riscv.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/clk.h"
#include "esp32s3/ulp.h"
#include "esp32s3/ulp_riscv.h"
#endif
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "ulp_private.h"
#include "esp_rom_sys.h"

esp_err_t ulp_riscv_run(void)
{
#if CONFIG_IDF_TARGET_ESP32S2
    /* Reset COCPU when power on. */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);
    esp_rom_delay_us(20);
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);

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

    /* start ULP_TIMER */
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG, RTC_CNTL_ULP_CP_FORCE_START_TOP);
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);

    return ESP_OK;
#elif CONFIG_IDF_TARGET_ESP32S3
    /* Reset COCPU when power on. */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_CLK_FO);
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);
    esp_rom_delay_us(20);
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_CLK_FO);
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);

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

    /* start ULP_TIMER */
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG, RTC_CNTL_ULP_CP_FORCE_START_TOP);
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);

    /* Select RISC-V as the ULP_TIMER trigger target
     * Selecting the RISC-V as the Coprocessor at the end is a workaround
     * for the hang issue recorded in IDF-4510.
     */
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SEL);

    /* Clear any spurious wakeup trigger interrupts upon ULP startup */
    esp_rom_delay_us(20);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, RTC_CNTL_COCPU_INT_CLR | RTC_CNTL_COCPU_TRAP_INT_CLR | RTC_CNTL_ULP_CP_INT_CLR);

    return ESP_OK;
#endif
}

esp_err_t ulp_riscv_load_binary(const uint8_t* program_binary, size_t program_size_bytes)
{
    if (program_binary == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (program_size_bytes > ULP_RESERVE_MEM) {
        return ESP_ERR_INVALID_SIZE;
    }

    uint8_t* base = (uint8_t*) RTC_SLOW_MEM;

    //Start by clearing memory reserved with zeros, this will also will initialize the bss:
    memset(base, 0, ULP_RESERVE_MEM);
    memcpy(base, program_binary, program_size_bytes);

    return ESP_OK;
}
