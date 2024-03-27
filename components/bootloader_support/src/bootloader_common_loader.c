/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "string.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_spiflash.h"
#include "esp_rom_crc.h"
#include "esp_rom_gpio.h"
#include "esp_flash_partitions.h"
#include "bootloader_flash.h"
#include "bootloader_common.h"
#include "soc/gpio_periph.h"
#include "soc/rtc.h"
#include "soc/efuse_reg.h"
#include "soc/chip_revision.h"
#include "hal/efuse_hal.h"
#include "hal/gpio_ll.h"
#include "esp_image_format.h"
#include "bootloader_sha.h"
#include "sys/param.h"
#include "bootloader_flash_priv.h"

#define ESP_PARTITION_HASH_LEN 32 /* SHA-256 digest length */
#define IS_MAX_REV_SET(max_chip_rev_full) (((max_chip_rev_full) != 65535) && ((max_chip_rev_full) != 0))

static const char* TAG = "boot_comm";

uint32_t bootloader_common_ota_select_crc(const esp_ota_select_entry_t *s)
{
    return esp_rom_crc32_le(UINT32_MAX, (uint8_t*)&s->ota_seq, 4);
}

bool bootloader_common_ota_select_invalid(const esp_ota_select_entry_t *s)
{
    return s->ota_seq == UINT32_MAX || s->ota_state == ESP_OTA_IMG_INVALID || s->ota_state == ESP_OTA_IMG_ABORTED;
}

bool bootloader_common_ota_select_valid(const esp_ota_select_entry_t *s)
{
    return bootloader_common_ota_select_invalid(s) == false && s->crc == bootloader_common_ota_select_crc(s);
}

int bootloader_common_get_active_otadata(esp_ota_select_entry_t *two_otadata)
{
    if (two_otadata == NULL) {
        return -1;
    }
    bool valid_two_otadata[2];
    valid_two_otadata[0] = bootloader_common_ota_select_valid(&two_otadata[0]);
    valid_two_otadata[1] = bootloader_common_ota_select_valid(&two_otadata[1]);
    return bootloader_common_select_otadata(two_otadata, valid_two_otadata, true);
}

esp_err_t bootloader_common_check_chip_validity(const esp_image_header_t* img_hdr, esp_image_type type)
{
    esp_err_t err = ESP_OK;
    esp_chip_id_t chip_id = CONFIG_IDF_FIRMWARE_CHIP_ID;
    if (chip_id != img_hdr->chip_id) {
        ESP_LOGE(TAG, "mismatch chip ID, expected %d, found %d", chip_id, img_hdr->chip_id);
        err = ESP_FAIL;
    } else {
#ifndef CONFIG_IDF_ENV_FPGA
        unsigned revision = efuse_hal_chip_revision();
        unsigned int major_rev = revision / 100;
        unsigned int minor_rev = revision % 100;
        unsigned min_rev = img_hdr->min_chip_rev_full;
        if (type == ESP_IMAGE_BOOTLOADER || type == ESP_IMAGE_APPLICATION) {
            if (!ESP_CHIP_REV_ABOVE(revision, min_rev)) {
                ESP_LOGE(TAG, "Image requires chip rev >= v%d.%d, but chip is v%d.%d",
                         min_rev / 100, min_rev % 100,
                         major_rev, minor_rev);
                err = ESP_FAIL;
            }
        }
        if (type == ESP_IMAGE_APPLICATION) {
            unsigned max_rev = img_hdr->max_chip_rev_full;
            if ((IS_MAX_REV_SET(max_rev) && (revision > max_rev) && !efuse_hal_get_disable_wafer_version_major())) {
                ESP_LOGE(TAG, "Image requires chip rev <= v%d.%d, but chip is v%d.%d",
                         max_rev / 100, max_rev % 100,
                         major_rev, minor_rev);
                err = ESP_FAIL;
            }
        }
#endif // CONFIG_IDF_ENV_FPGA
    }

    return err;
}

int bootloader_common_select_otadata(const esp_ota_select_entry_t *two_otadata, bool *valid_two_otadata, bool max)
{
    if (two_otadata == NULL || valid_two_otadata == NULL) {
        return -1;
    }
    int active_otadata = -1;
    if (valid_two_otadata[0] && valid_two_otadata[1]) {
        uint32_t condition = (max == true) ? MAX(two_otadata[0].ota_seq, two_otadata[1].ota_seq) : MIN(two_otadata[0].ota_seq, two_otadata[1].ota_seq);
        if (condition == two_otadata[0].ota_seq) {
            active_otadata = 0;
        } else {
            active_otadata = 1;
        }
        ESP_LOGD(TAG, "Both OTA copies are valid");
    } else {
        for (int i = 0; i < 2; ++i) {
            if (valid_two_otadata[i]) {
                active_otadata = i;
                ESP_LOGD(TAG, "Only otadata[%d] is valid", i);
                break;
            }
        }
    }
    return active_otadata;
}

#if CONFIG_BOOTLOADER_RESERVE_RTC_MEM

static uint32_t rtc_retain_mem_size(void) {
#if CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC && !CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC_IN_CRC
    /* A custom memory has been reserved by the user, do not consider this memory into CRC calculation as it may change without
     * the have the user updating the CRC. Return the offset of the custom field, which is equivalent to size of the structure
     * minus the size of everything after (including) `custom` */
    return offsetof(rtc_retain_mem_t, custom);
#else
    return sizeof(rtc_retain_mem_t) - sizeof(bootloader_common_get_rtc_retain_mem()->crc);
#endif
}

static bool is_retain_mem_valid(void)
{
    rtc_retain_mem_t* rtc_retain_mem = bootloader_common_get_rtc_retain_mem();
    return esp_rom_crc32_le(UINT32_MAX, (uint8_t*)rtc_retain_mem, rtc_retain_mem_size()) == rtc_retain_mem->crc && rtc_retain_mem->crc != UINT32_MAX;
}

static void update_rtc_retain_mem_crc(void)
{
    rtc_retain_mem_t* rtc_retain_mem = bootloader_common_get_rtc_retain_mem();
    rtc_retain_mem->crc = esp_rom_crc32_le(UINT32_MAX, (uint8_t*)rtc_retain_mem, rtc_retain_mem_size());
}

NOINLINE_ATTR void bootloader_common_reset_rtc_retain_mem(void)
{
    hal_memset(bootloader_common_get_rtc_retain_mem(), 0, sizeof(rtc_retain_mem_t));
}

uint16_t bootloader_common_get_rtc_retain_mem_reboot_counter(void)
{
    if (is_retain_mem_valid()) {
        return bootloader_common_get_rtc_retain_mem()->reboot_counter;
    }
    return 0;
}

void bootloader_common_set_rtc_retain_mem_factory_reset_state(void)
{
    if (!is_retain_mem_valid()) {
        bootloader_common_reset_rtc_retain_mem();
    }
    bootloader_common_get_rtc_retain_mem()->flags.factory_reset_state = true;
    update_rtc_retain_mem_crc();
}

bool bootloader_common_get_rtc_retain_mem_factory_reset_state(void)
{
    rtc_retain_mem_t* rtc_retain_mem = bootloader_common_get_rtc_retain_mem();
    if (is_retain_mem_valid()) {
        bool factory_reset_state = rtc_retain_mem->flags.factory_reset_state;
        if (factory_reset_state == true) {
            rtc_retain_mem->flags.factory_reset_state = false;
            update_rtc_retain_mem_crc();
        }
        return factory_reset_state;
    }
    return false;
}

esp_partition_pos_t* bootloader_common_get_rtc_retain_mem_partition(void)
{
    if (is_retain_mem_valid()) {
        return &bootloader_common_get_rtc_retain_mem()->partition;
    }
    return NULL;
}

void bootloader_common_update_rtc_retain_mem(esp_partition_pos_t* partition, bool reboot_counter)
{
    rtc_retain_mem_t* rtc_retain_mem = bootloader_common_get_rtc_retain_mem();
    if (reboot_counter) {
        if (!is_retain_mem_valid()) {
            bootloader_common_reset_rtc_retain_mem();
        }
        if (++rtc_retain_mem->reboot_counter == 0) {
            // do not allow to overflow. Stop it.
            --rtc_retain_mem->reboot_counter;
        }

    }

    if (partition != NULL) {
        rtc_retain_mem->partition.offset = partition->offset;
        rtc_retain_mem->partition.size   = partition->size;
    }

    update_rtc_retain_mem_crc();
}

rtc_retain_mem_t* bootloader_common_get_rtc_retain_mem(void)
{
#ifdef BOOTLOADER_BUILD
    #define RTC_RETAIN_MEM_ADDR (SOC_RTC_DRAM_HIGH - sizeof(rtc_retain_mem_t))
    static rtc_retain_mem_t *const s_bootloader_retain_mem = (rtc_retain_mem_t *)RTC_RETAIN_MEM_ADDR;
    return s_bootloader_retain_mem;
#else
    static __attribute__((section(".bootloader_data_rtc_mem"))) rtc_retain_mem_t s_bootloader_retain_mem;
    return &s_bootloader_retain_mem;
#endif // !BOOTLOADER_BUILD
}

#endif // CONFIG_BOOTLOADER_RESERVE_RTC_MEM
