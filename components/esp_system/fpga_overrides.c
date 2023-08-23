/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc.h"
#include "esp_private/periph_ctrl.h"
#ifndef CONFIG_IDF_TARGET_ESP32
#include "soc/system_reg.h"
#endif // not CONFIG_IDF_TARGET_ESP32
#include "soc/rtc.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/rtc.h"
#include "esp_private/esp_pmu.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/rtc.h"
#endif
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"
#include "esp_attr.h"

static const char *TAG = "fpga";

static void s_warn(void)
{
    ESP_EARLY_LOGW(TAG, "Project configuration is for internal FPGA use, not all functions will work");
}

void bootloader_clock_configure(void)
{
    s_warn();
    esp_rom_uart_tx_wait_idle(0);

    uint32_t xtal_freq_mhz = 40;
#ifdef CONFIG_IDF_TARGET_ESP32S2
    uint32_t apb_freq_hz = 20000000;
#else
    uint32_t apb_freq_hz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 1000000;
#endif // CONFIG_IDF_TARGET_ESP32S2
    esp_rom_set_cpu_ticks_per_us(apb_freq_hz / 1000000);
#ifdef RTC_APB_FREQ_REG
    REG_WRITE(RTC_APB_FREQ_REG, (apb_freq_hz >> 12) | ((apb_freq_hz >> 12) << 16));
#endif
    REG_WRITE(RTC_XTAL_FREQ_REG, (xtal_freq_mhz) | ((xtal_freq_mhz) << 16));
}

/* Placed in IRAM since test_apps expects it to be */
void IRAM_ATTR bootloader_fill_random(void *buffer, size_t length)
{
    uint8_t *buffer_bytes = (uint8_t *)buffer;
    for (int i = 0; i < length; i++) {
        buffer_bytes[i] = 0x5A;
    }
}

void esp_clk_init(void)
{
    s_warn();
#if SOC_PMU_SUPPORTED
    pmu_init();
#endif
}

void esp_perip_clk_init(void)
{
}

/**
 * @brief No-op function, used to force linking this file
 *
 */
void esp_common_include_fpga_overrides(void)
{
}
