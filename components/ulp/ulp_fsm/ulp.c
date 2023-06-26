/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
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
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/ulp.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/ulp.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/ulp.h"
#endif

#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include "ulp_common.h"
#include "esp_rom_sys.h"

#include "esp_check.h"
#include "esp_private/rtc_ctrl.h"

typedef struct {
    uint32_t magic;
    uint16_t text_offset;
    uint16_t text_size;
    uint16_t data_size;
    uint16_t bss_size;
} ulp_binary_header_t;

#define ULP_BINARY_MAGIC_ESP32 (0x00706c75)

static const char* TAG = "ulp";

esp_err_t ulp_isr_register(intr_handler_t fn, void *arg)
{
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TAG, "ULP ISR is NULL");
    REG_SET_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_ULP_CP_INT_ENA_M);
#if CONFIG_IDF_TARGET_ESP32
    return rtc_isr_register(fn, arg, RTC_CNTL_SAR_INT_ST_M, 0);
#else
    return rtc_isr_register(fn, arg, RTC_CNTL_ULP_CP_INT_ST_M, 0);
#endif /* CONFIG_IDF_TARGET_ESP32 */
}

esp_err_t ulp_isr_deregister(intr_handler_t fn, void *arg)
{
    ESP_RETURN_ON_FALSE(fn, ESP_ERR_INVALID_ARG, TAG, "ULP ISR is NULL");
    REG_CLR_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_ULP_CP_INT_ENA_M);
    return rtc_isr_deregister(fn, arg);
}

esp_err_t ulp_run(uint32_t entry_point)
{
#if CONFIG_IDF_TARGET_ESP32
    // disable ULP timer
    CLEAR_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    // wait for at least 1 RTC_SLOW_CLK cycle
    esp_rom_delay_us(10);
    // set entry point
    REG_SET_FIELD(SENS_SAR_START_FORCE_REG, SENS_PC_INIT, entry_point);
    // disable force start
    CLEAR_PERI_REG_MASK(SENS_SAR_START_FORCE_REG, SENS_ULP_CP_FORCE_START_TOP_M);
    // set time until wakeup is allowed to the smallest possible
    REG_SET_FIELD(RTC_CNTL_TIMER5_REG, RTC_CNTL_MIN_SLP_VAL, RTC_CNTL_MIN_SLP_VAL_MIN);
    // make sure voltage is raised when RTC 8MCLK is enabled
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_I2C_FOLW_8M);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_CORE_FOLW_8M);
    SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_BIAS_SLEEP_FOLW_8M);
    // enable ULP timer
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
#else
    /* Reset COCPU when power on. */
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG, RTC_CNTL_ULP_CP_RESET);
    esp_rom_delay_us(20);
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG, RTC_CNTL_ULP_CP_RESET);
    // disable ULP timer
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    // wait for at least 1 RTC_SLOW_CLK cycle
    esp_rom_delay_us(10);
    // set entry point
    REG_SET_FIELD(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_PC_INIT, entry_point);
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SEL);         // Select ULP_TIMER trigger target for ULP.
    // start ULP clock gate.
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG ,RTC_CNTL_ULP_CP_CLK_FO);
    // ULP FSM sends the DONE signal.
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE_FORCE);
#if CONFIG_IDF_TARGET_ESP32S3
    /* Set the CLKGATE_EN signal on esp32s3 */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_CLKGATE_EN);
#endif
    /* Clear interrupt COCPU status */
    REG_WRITE(RTC_CNTL_INT_CLR_REG, RTC_CNTL_COCPU_INT_CLR | RTC_CNTL_COCPU_TRAP_INT_CLR | RTC_CNTL_ULP_CP_INT_CLR);
    // 1: start with timer. wait ULP_TIMER cnt timer.
    CLEAR_PERI_REG_MASK(RTC_CNTL_ULP_CP_CTRL_REG, RTC_CNTL_ULP_CP_FORCE_START_TOP); // Select ULP_TIMER timer as COCPU trigger source
    SET_PERI_REG_MASK(RTC_CNTL_ULP_CP_TIMER_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);     // Software to turn on the ULP_TIMER timer
#endif
    return ESP_OK;
}

esp_err_t ulp_load_binary(uint32_t load_addr, const uint8_t* program_binary, size_t program_size)
{
    size_t program_size_bytes = program_size * sizeof(uint32_t);
    size_t load_addr_bytes = load_addr * sizeof(uint32_t);

    if (program_size_bytes < sizeof(ulp_binary_header_t)) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (load_addr_bytes > CONFIG_ULP_COPROC_RESERVE_MEM) {
        return ESP_ERR_INVALID_ARG;
    }
    if (load_addr_bytes + program_size_bytes > CONFIG_ULP_COPROC_RESERVE_MEM) {
        return ESP_ERR_INVALID_SIZE;
    }

    // Make a copy of a header in case program_binary isn't aligned
    ulp_binary_header_t header;
    memcpy(&header, program_binary, sizeof(header));

    if (header.magic != ULP_BINARY_MAGIC_ESP32) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    size_t total_size = (size_t) header.text_offset + (size_t) header.text_size +
            (size_t) header.data_size;

    ESP_LOGD(TAG, "program_size_bytes: %d total_size: %d offset: %d .text: %d, .data: %d, .bss: %d",
            program_size_bytes, total_size, header.text_offset,
            header.text_size, header.data_size, header.bss_size);

    if (total_size != program_size_bytes) {
        return ESP_ERR_INVALID_SIZE;
    }

    size_t text_data_size = header.text_size + header.data_size;
    uint8_t* base = (uint8_t*) RTC_SLOW_MEM;

    memcpy(base + load_addr_bytes, program_binary + header.text_offset, text_data_size);
    memset(base + load_addr_bytes + text_data_size, 0, header.bss_size);

    return ESP_OK;
}
