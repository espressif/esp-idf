// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "bootloader_init.h"
#include "bootloader_flash_priv.h"
#include "bootloader_flash_config.h"
#include "bootloader_random.h"
#include "bootloader_clock.h"
#include "bootloader_common.h"
#include "esp_flash_encrypt.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "hal/wdt_hal.h"

static const char *TAG = "boot";

esp_image_header_t WORD_ALIGNED_ATTR bootloader_image_hdr;

void bootloader_clear_bss_section(void)
{
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));
}

esp_err_t bootloader_read_bootloader_header(void)
{
    /* load bootloader image header */
    if (bootloader_flash_read(ESP_BOOTLOADER_OFFSET, &bootloader_image_hdr, sizeof(esp_image_header_t), true) != ESP_OK) {
        ESP_LOGE(TAG, "failed to load bootloader image header!");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t bootloader_check_bootloader_validity(void)
{
    /* read chip revision from efuse */
    uint8_t revision = bootloader_common_get_chip_revision();
    ESP_LOGI(TAG, "chip revision: %d", revision);
    /* compare with the one set in bootloader image header */
    if (bootloader_common_check_chip_validity(&bootloader_image_hdr, ESP_IMAGE_BOOTLOADER) != ESP_OK) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

void bootloader_config_wdt(void)
{
    /*
     * At this point, the flashboot protection of RWDT and MWDT0 will have been
     * automatically enabled. We can disable flashboot protection as it's not
     * needed anymore. If configured to do so, we also initialize the RWDT to
     * protect the remainder of the bootloader process.
     */
    //Disable RWDT flashboot protection.
    wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_set_flashboot_en(&rtc_wdt_ctx, false);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);

#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
    //Initialize and start RWDT to protect the  for bootloader if configured to do so
    ESP_LOGD(TAG, "Enabling RTCWDT(%d ms)", CONFIG_BOOTLOADER_WDT_TIME_MS);
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
    uint32_t stage_timeout_ticks = (uint32_t)((uint64_t)CONFIG_BOOTLOADER_WDT_TIME_MS * rtc_clk_slow_freq_get_hz() / 1000);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_enable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
#endif

    //Disable MWDT0 flashboot protection. But only after we've enabled the RWDT first so that there's not gap in WDT protection.
    wdt_hal_context_t wdt_ctx = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&wdt_ctx);
    wdt_hal_set_flashboot_en(&wdt_ctx, false);
    wdt_hal_write_protect_enable(&wdt_ctx);
}

void bootloader_enable_random(void)
{
    ESP_LOGI(TAG, "Enabling RNG early entropy source...");
    bootloader_random_enable();
}

void bootloader_print_banner(void)
{
    ESP_LOGI(TAG, "ESP-IDF %s 2nd stage bootloader", IDF_VER);
    ESP_LOGI(TAG, "compile time " __TIME__);
}

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    ESP_LOGE(TAG, "Assert failed in %s, %s:%d (%s)", func, file, line, expr);
    while (1) {
    }
}
