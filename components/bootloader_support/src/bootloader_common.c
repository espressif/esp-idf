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
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/spi_flash.h"
#include "esp32/rom/crc.h"
#include "esp32/rom/gpio.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/spi_flash.h"
#include "esp32s2beta/rom/crc.h"
#include "esp32s2beta/rom/ets_sys.h"
#include "esp32s2beta/rom/gpio.h"
#endif
#include "esp_flash_partitions.h"
#include "bootloader_flash.h"
#include "bootloader_common.h"
#include "soc/gpio_periph.h"
#include "soc/rtc.h"
#include "soc/efuse_reg.h"
#include "esp_image_format.h"
#include "bootloader_sha.h"
#include "sys/param.h"

#define ESP_PARTITION_HASH_LEN 32 /* SHA-256 digest length */

static const char* TAG = "boot_comm";

uint32_t bootloader_common_ota_select_crc(const esp_ota_select_entry_t *s)
{
    return crc32_le(UINT32_MAX, (uint8_t*)&s->ota_seq, 4);
}

bool bootloader_common_ota_select_invalid(const esp_ota_select_entry_t *s)
{
    return s->ota_seq == UINT32_MAX || s->ota_state == ESP_OTA_IMG_INVALID || s->ota_state == ESP_OTA_IMG_ABORTED;
}

bool bootloader_common_ota_select_valid(const esp_ota_select_entry_t *s)
{
    return bootloader_common_ota_select_invalid(s) == false && s->crc == bootloader_common_ota_select_crc(s);
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

int bootloader_common_select_otadata(const esp_ota_select_entry_t *two_otadata, bool *valid_two_otadata, bool max)
{
    if (two_otadata == NULL || valid_two_otadata == NULL) {
        return -1;
    }
    int active_otadata = -1;
    if (valid_two_otadata[0] && valid_two_otadata[1]) {
        int condition = (max == true) ? MAX(two_otadata[0].ota_seq, two_otadata[1].ota_seq) : MIN(two_otadata[0].ota_seq, two_otadata[1].ota_seq);
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

esp_err_t bootloader_common_get_partition_description(const esp_partition_pos_t *partition, esp_app_desc_t *app_desc)
{
    if (partition == NULL || app_desc == NULL || partition->offset == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    const uint8_t *image = bootloader_mmap(partition->offset, partition->size);
    if (image == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", partition->offset, partition->size);
        return ESP_FAIL;
    }

    memcpy(app_desc, image + sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t), sizeof(esp_app_desc_t));
    bootloader_munmap(image);

    if (app_desc->magic_word != ESP_APP_DESC_MAGIC_WORD) {
        return ESP_ERR_NOT_FOUND;
    }

    return ESP_OK;
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
        ets_delay_us(10); // wait for regulator to become stable
    }
#endif // CONFIG_BOOTLOADER_VDDSDIO_BOOST
}


esp_err_t bootloader_common_check_chip_validity(const esp_image_header_t* img_hdr, esp_image_type type)
{
    esp_err_t err = ESP_OK;
    esp_chip_id_t chip_id = CONFIG_IDF_FIRMWARE_CHIP_ID;
    if (chip_id != img_hdr->chip_id) {
        ESP_LOGE(TAG, "mismatch chip ID, expected %d, found %d", chip_id, img_hdr->chip_id);
        err = ESP_FAIL;
    }
    uint8_t revision = bootloader_common_get_chip_revision();
    if (revision < img_hdr->min_chip_rev) {
        ESP_LOGE(TAG, "can't run on lower chip revision, expected %d, found %d", revision, img_hdr->min_chip_rev);
        err = ESP_FAIL;
    } else if (revision != img_hdr->min_chip_rev) {
        ESP_LOGI(TAG, "chip revision: %d, min. %s chip revision: %d", revision, type == ESP_IMAGE_BOOTLOADER ? "bootloader" : "application", img_hdr->min_chip_rev);
    }
    return err;
}

#if defined( CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP ) || defined( CONFIG_BOOTLOADER_CUSTOM_RESERVE_RTC )

rtc_retain_mem_t *const rtc_retain_mem = (rtc_retain_mem_t *)(SOC_RTC_DRAM_HIGH - sizeof(rtc_retain_mem_t));

static bool check_rtc_retain_mem(void)
{
    return crc32_le(UINT32_MAX, (uint8_t*)rtc_retain_mem, sizeof(rtc_retain_mem_t) - sizeof(rtc_retain_mem->crc)) == rtc_retain_mem->crc && rtc_retain_mem->crc != UINT32_MAX;
}

static void update_rtc_retain_mem_crc(void)
{
    rtc_retain_mem->crc = crc32_le(UINT32_MAX, (uint8_t*)rtc_retain_mem, sizeof(rtc_retain_mem_t) - sizeof(rtc_retain_mem->crc));
}

void bootloader_common_reset_rtc_retain_mem(void)
{
    memset(rtc_retain_mem, 0, sizeof(rtc_retain_mem_t));
}

uint16_t bootloader_common_get_rtc_retain_mem_reboot_counter(void)
{
    if (check_rtc_retain_mem()) {
        return rtc_retain_mem->reboot_counter;
    }
    return 0;
}

esp_partition_pos_t* bootloader_common_get_rtc_retain_mem_partition(void)
{
    if (check_rtc_retain_mem()) {
        return &rtc_retain_mem->partition;
    }
    return NULL;
}

void bootloader_common_update_rtc_retain_mem(esp_partition_pos_t* partition, bool reboot_counter)
{
    if (reboot_counter) {
        if (!check_rtc_retain_mem()) {
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
    return rtc_retain_mem;
}
#endif
