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
#include "bootloader_flash.h"
#include "bootloader_flash_config.h"
#include "bootloader_random.h"
#include "bootloader_clock.h"
#include "bootloader_common.h"
#include "esp_flash_encrypt.h"
#include "hal/timer_ll.h"
#include "soc/cpu.h"
#include "soc/rtc.h"
#include "soc/rtc_wdt.h"


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
#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
    ESP_LOGD(TAG, "Enabling RTCWDT(%d ms)", CONFIG_BOOTLOADER_WDT_TIME_MS);
    rtc_wdt_protect_off();
    rtc_wdt_disable();
    rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_3_2us);
    rtc_wdt_set_length_of_reset_signal(RTC_WDT_CPU_RESET_SIG, RTC_WDT_LENGTH_3_2us);
    rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_RTC);
    rtc_wdt_set_time(RTC_WDT_STAGE0, CONFIG_BOOTLOADER_WDT_TIME_MS);
    rtc_wdt_enable();
    rtc_wdt_protect_on();
#else /* disable watch dog */
    rtc_wdt_disable();
#endif
    timer_ll_wdt_set_protect(&TIMERG0, false);
    timer_ll_wdt_flashboot_en(&TIMERG0, false);
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
