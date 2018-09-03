// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#include <stdbool.h>
#include <assert.h>
#include "string.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "rom/spi_flash.h"
#include "rom/crc.h"
#include "rom/ets_sys.h"
#include "rom/gpio.h"
#include "esp_flash_data_types.h"
#include "esp_secure_boot.h"
#include "esp_flash_partitions.h"
#include "bootloader_flash.h"
#include "bootloader_common.h"
#include "soc/gpio_periph.h"
#include "esp_image_format.h"
#include "bootloader_sha.h"

#define ESP_PARTITION_HASH_LEN 32 /* SHA-256 digest length */

static const char* TAG = "boot_comm";

uint32_t bootloader_common_ota_select_crc(const esp_ota_select_entry_t *s)
{
    return crc32_le(UINT32_MAX, (uint8_t*)&s->ota_seq, 4);
}

bool bootloader_common_ota_select_valid(const esp_ota_select_entry_t *s)
{
    return s->ota_seq != UINT32_MAX && s->crc == bootloader_common_ota_select_crc(s);
}

esp_comm_gpio_hold_t bootloader_common_check_long_hold_gpio(uint32_t num_pin, uint32_t delay_sec)
{
    gpio_pad_select_gpio(num_pin);
    if (GPIO_PIN_MUX_REG[num_pin]) {
        PIN_INPUT_ENABLE(GPIO_PIN_MUX_REG[num_pin]);
    }
    gpio_pad_pullup(num_pin);
    uint32_t tm_start = esp_log_early_timestamp();
    if (GPIO_INPUT_GET(num_pin) == 1) {
        return GPIO_NOT_HOLD;
    }
    do {
        if (GPIO_INPUT_GET(num_pin) != 0) {
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
        int pos_delim = strcspn(sub_list_start_like_label, ", ");
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

                ESP_LOGI(TAG, "%2d %-16s data  %08x %08x [%s]", i, partition->label,
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
        // Function esp_image_verify() verifies and fills the structure data.
        // here important to get: image_digest, image_len, hash_appended.
        if (esp_image_verify(ESP_IMAGE_VERIFY_SILENT, &partition_pos, &data) != ESP_OK) {
            return ESP_ERR_IMAGE_INVALID;
        }
        if (data.image.hash_appended) {
            memcpy(out_sha_256, data.image_digest, ESP_PARTITION_HASH_LEN);
            return ESP_OK;
        }
        // If image doesn't have a appended hash then hash calculates for entire image.
        size = data.image_len;
    }
    // If image is type by data then hash is calculated for entire image.
    const void *partition_bin = bootloader_mmap(address, size);
    if (partition_bin == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", address, size);
        return ESP_FAIL;
    }
    bootloader_sha256_handle_t sha_handle = bootloader_sha256_start();
    if (sha_handle == NULL) {
        bootloader_munmap(partition_bin);
        return ESP_ERR_NO_MEM;
    }
    bootloader_sha256_data(sha_handle, partition_bin, size);
    bootloader_sha256_finish(sha_handle, out_sha_256);

    bootloader_munmap(partition_bin);

    return ESP_OK;
}
