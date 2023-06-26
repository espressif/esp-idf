/*
 * SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <assert.h>
#include "string.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_spiflash.h"
#include "esp_rom_crc.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "esp_flash_partitions.h"
#include "bootloader_flash_priv.h"
#include "bootloader_common.h"
#include "bootloader_utility.h"
#include "soc/gpio_periph.h"
#include "soc/rtc.h"
#include "soc/efuse_reg.h"
#include "hal/gpio_ll.h"
#include "esp_image_format.h"
#include "bootloader_sha.h"
#include "sys/param.h"

#define ESP_PARTITION_HASH_LEN 32 /* SHA-256 digest length */

static const char* TAG = "boot_comm";

esp_comm_gpio_hold_t bootloader_common_check_long_hold_gpio(uint32_t num_pin, uint32_t delay_sec)
{
    return bootloader_common_check_long_hold_gpio_level(num_pin, delay_sec, false);
}

esp_comm_gpio_hold_t bootloader_common_check_long_hold_gpio_level(uint32_t num_pin, uint32_t delay_sec, bool level)
{
    esp_rom_gpio_pad_select_gpio(num_pin);
    if (GPIO_PIN_MUX_REG[num_pin]) {
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[num_pin]);
    }
    esp_rom_gpio_pad_pullup_only(num_pin);
    uint32_t tm_start = esp_log_early_timestamp();
    if (gpio_ll_get_level(&GPIO, num_pin) != level) {
        return GPIO_NOT_HOLD;
    }
    do {
        if (gpio_ll_get_level(&GPIO, num_pin) != level) {
            return GPIO_SHORT_HOLD;
        }
    } while (delay_sec > ((esp_log_early_timestamp() - tm_start) / 1000L));
    return GPIO_LONG_HOLD;
}

// Search for a label in the list. list = "nvs1, nvs2, otadata, nvs"; label = "nvs".
bool bootloader_common_label_search(const char *list, char *label)
{
    if (list == NULL || label == NULL) {
        return false;
    }
    const char *sub_list_start_like_label = strstr(list, label);
    while (sub_list_start_like_label != NULL) {

        // ["," or " "] + label + ["," or " " or "\0"]
        // first character before the label found there must be a delimiter ["," or " "].
        int idx_first = sub_list_start_like_label - list;
        if (idx_first == 0 || (idx_first != 0 && (list[idx_first - 1] == ',' || list[idx_first - 1] == ' '))) {
            // next character after the label found there must be a delimiter ["," or " " or "\0"].
            int len_label = strlen(label);
            if (sub_list_start_like_label[len_label] == 0   ||
                sub_list_start_like_label[len_label] == ',' ||
                sub_list_start_like_label[len_label] == ' ') {
                return true;
            }
        }

        // [start_delim] + label + [end_delim] was not found.
        // Position is moving to next delimiter if it is not the end of list.
        size_t pos_delim = strcspn(sub_list_start_like_label, ", ");
        if (pos_delim == strlen(sub_list_start_like_label)) {
            break;
        }
        sub_list_start_like_label = strstr(&sub_list_start_like_label[pos_delim], label);
    }
    return false;
}

bool bootloader_common_erase_part_type_data(const char *list_erase, bool ota_data_erase)
{
    const esp_partition_info_t *partitions;
    const char *marker;
    esp_err_t err;
    int num_partitions;
    bool ret = true;

    partitions = bootloader_mmap(ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
    if (!partitions) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", ESP_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_MAX_LEN);
        return false;
    }
    ESP_LOGD(TAG, "mapped partition table 0x%x at 0x%x", ESP_PARTITION_TABLE_OFFSET, (intptr_t)partitions);

    err = esp_partition_table_verify(partitions, true, &num_partitions);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to verify partition table");
        ret = false;
    } else {
        ESP_LOGI(TAG, "## Label            Usage Offset   Length   Cleaned");
        for (int i = 0; i < num_partitions; i++) {
            const esp_partition_info_t *partition = &partitions[i];
            char label[sizeof(partition->label) + 1] = {0};
            if (partition->type == PART_TYPE_DATA) {
                bool fl_ota_data_erase = false;
                if (ota_data_erase == true && partition->subtype == PART_SUBTYPE_DATA_OTA) {
                    fl_ota_data_erase = true;
                }
                // partition->label is not null-terminated string.
                strncpy(label, (char *)&partition->label, sizeof(label) - 1);
                if (fl_ota_data_erase == true || (bootloader_common_label_search(list_erase, label) == true)) {
                    err = bootloader_flash_erase_range(partition->pos.offset, partition->pos.size);
                    if (err != ESP_OK) {
                        ret = false;
                        marker = "err";
                    } else {
                        marker = "yes";
                    }
                } else {
                    marker = "no";
                }

                ESP_LOGI(TAG, "%2d %-16s data  %08"PRIx32" %08"PRIx32" [%s]", i, partition->label,
                         partition->pos.offset, partition->pos.size, marker);
            }
        }
    }

    bootloader_munmap(partitions);

    return ret;
}

esp_err_t bootloader_common_get_sha256_of_partition (uint32_t address, uint32_t size, int type, uint8_t *out_sha_256)
{
    if (out_sha_256 == NULL || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (type == PART_TYPE_APP) {
        const esp_partition_pos_t partition_pos = {
            .offset = address,
            .size = size,
        };
        esp_image_metadata_t data;
        if (esp_image_get_metadata(&partition_pos, &data) != ESP_OK) {
            return ESP_ERR_IMAGE_INVALID;
        }
        if (data.image.hash_appended) {
            memcpy(out_sha_256, data.image_digest, ESP_PARTITION_HASH_LEN);
            uint8_t calc_sha256[ESP_PARTITION_HASH_LEN];
            // The hash is verified before returning, if app content is invalid then the function returns ESP_ERR_IMAGE_INVALID.
            esp_err_t error = bootloader_sha256_flash_contents(address, data.image_len - ESP_PARTITION_HASH_LEN, calc_sha256);
            if (error || memcmp(data.image_digest, calc_sha256, ESP_PARTITION_HASH_LEN) != 0) {
                return ESP_ERR_IMAGE_INVALID;
            }
            return ESP_OK;
        }
        // If image doesn't have a appended hash then hash calculates for entire image.
        size = data.image_len;
    }
    // If image is type by data then hash is calculated for entire image.
    return bootloader_sha256_flash_contents(address, size, out_sha_256);
}

void bootloader_common_vddsdio_configure(void)
{
#if CONFIG_BOOTLOADER_VDDSDIO_BOOST_1_9V
    rtc_vddsdio_config_t cfg = rtc_vddsdio_get_config();
    if (cfg.enable == 1 && cfg.tieh == RTC_VDDSDIO_TIEH_1_8V) {    // VDDSDIO regulator is enabled @ 1.8V
        cfg.drefh = 3;
        cfg.drefm = 3;
        cfg.drefl = 3;
        cfg.force = 1;
        rtc_vddsdio_set_config(cfg);
        esp_rom_delay_us(10); // wait for regulator to become stable
    }
#endif // CONFIG_BOOTLOADER_VDDSDIO_BOOST
}
