/*
 * SPDX-FileCopyrightText: 2010-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_private/periph_ctrl.h"
#ifndef CONFIG_IDF_TARGET_ESP32
#include "soc/system_reg.h"
#endif // not CONFIG_IDF_TARGET_ESP32
#include "soc/rtc.h"
#include "rom/rtc.h"

#if SOC_PMU_SUPPORTED
#include "esp_private/esp_pmu.h"
#endif

#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"

static const char *TAG = "fpga_clk";

static void s_warn(void)
{
    ESP_EARLY_LOGE(TAG, "Project configuration is for internal FPGA use, clock functions will not work");
}

void bootloader_clock_configure(void)
{
    s_warn();
    esp_rom_output_tx_wait_idle(0);
    uint32_t xtal_freq_mhz __attribute__((unused));
#if CONFIG_IDF_TARGET_ESP32H21 || CONFIG_IDF_TARGET_ESP32H4
    xtal_freq_mhz = 32;
#else
    xtal_freq_mhz = 40;
#endif
#ifdef CONFIG_IDF_TARGET_ESP32S2
    uint32_t apb_freq_hz = 20000000;
#else
    uint32_t apb_freq_hz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 1000000;
#endif // CONFIG_IDF_TARGET_ESP32S2
    esp_rom_set_cpu_ticks_per_us(apb_freq_hz / 1000000);
#ifdef RTC_APB_FREQ_REG
    REG_WRITE(RTC_APB_FREQ_REG, (apb_freq_hz >> 12) | ((apb_freq_hz >> 12) << 16));
#endif
#ifdef RTC_XTAL_FREQ_REG
    REG_WRITE(RTC_XTAL_FREQ_REG, (xtal_freq_mhz) | ((xtal_freq_mhz) << 16));
#endif
}

void esp_rtc_init(void)
{
#if SOC_PMU_SUPPORTED
    pmu_init();
#endif
}

void esp_clk_init(void)
{
    s_warn();
}

void esp_perip_clk_init(void)
{
}

/**
 * @brief No-op function, used to force linking this file
 *
 */
void esp_common_include_fpga_overrides_clk(void)
{
}
