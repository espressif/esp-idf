/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "hal/wdt_hal.h"
#include "hal/efuse_hal.h"
#include "esp_bootloader_desc.h"

static const char *TAG = "boot";

#if !CONFIG_APP_BUILD_TYPE_RAM
esp_image_header_t WORD_ALIGNED_ATTR bootloader_image_hdr;
#endif

void bootloader_clear_bss_section(void)
{
    memset(&_bss_start, 0, (&_bss_end - &_bss_start) * sizeof(_bss_start));
}

esp_err_t bootloader_read_bootloader_header(void)
{
    /* load bootloader image header */
    if (bootloader_flash_read(ESP_BOOTLOADER_OFFSET, &bootloader_image_hdr, sizeof(esp_image_header_t), true) != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "failed to load bootloader image header!");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t bootloader_check_bootloader_validity(void)
{
    unsigned int revision = efuse_hal_chip_revision();
    unsigned int major = revision / 100;
    unsigned int minor = revision % 100;
    ESP_EARLY_LOGI(TAG, "chip revision: v%d.%d", major, minor);
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
    wdt_hal_context_t rwdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rwdt_ctx);
    wdt_hal_set_flashboot_en(&rwdt_ctx, false);
    wdt_hal_write_protect_enable(&rwdt_ctx);

#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
    //Initialize and start RWDT to protect the  for bootloader if configured to do so
    ESP_EARLY_LOGD(TAG, "Enabling RTCWDT(%d ms)", CONFIG_BOOTLOADER_WDT_TIME_MS);
    wdt_hal_init(&rwdt_ctx, WDT_RWDT, 0, false);
    uint32_t stage_timeout_ticks = (uint32_t)((uint64_t)CONFIG_BOOTLOADER_WDT_TIME_MS * rtc_clk_slow_freq_get_hz() / 1000);
    wdt_hal_write_protect_disable(&rwdt_ctx);
    wdt_hal_config_stage(&rwdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_enable(&rwdt_ctx);
    wdt_hal_write_protect_enable(&rwdt_ctx);
#endif

    //Disable MWDT0 flashboot protection. But only after we've enabled the RWDT first so that there's not gap in WDT protection.
    wdt_hal_context_t mwdt_ctx = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&mwdt_ctx);
    wdt_hal_set_flashboot_en(&mwdt_ctx, false);
    wdt_hal_write_protect_enable(&mwdt_ctx);
}

void bootloader_enable_random(void)
{
    ESP_EARLY_LOGI(TAG, "Enabling RNG early entropy source...");
    bootloader_random_enable();
}

void bootloader_print_banner(void)
{
    if (CONFIG_BOOTLOADER_LOG_LEVEL >= ESP_LOG_INFO) {
        const esp_bootloader_desc_t *desc = esp_bootloader_get_description();
        ESP_EARLY_LOGI(TAG, "ESP-IDF %s 2nd stage bootloader", desc->idf_ver);
#ifdef CONFIG_BOOTLOADER_COMPILE_TIME_DATE
        ESP_EARLY_LOGI(TAG, "compile time %s", desc->date_time);
#endif
    }

#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#if (SOC_CPU_CORES_NUM > 1)
    ESP_EARLY_LOGW(TAG, "Unicore bootloader");
#endif
#else
    ESP_EARLY_LOGI(TAG, "Multicore bootloader");
#endif
}
