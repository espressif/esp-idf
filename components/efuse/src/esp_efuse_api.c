/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "assert.h"
#include "sdkconfig.h"
#include "esp_efuse_table.h"

const static char *TAG = "efuse";

#if defined(BOOTLOADER_BUILD)
#define EFUSE_LOCK_ACQUIRE_RECURSIVE()
#define EFUSE_LOCK_RELEASE_RECURSIVE()
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/lock.h>
static _lock_t s_efuse_lock;
#define EFUSE_LOCK_ACQUIRE_RECURSIVE() _lock_acquire_recursive(&s_efuse_lock)
#define EFUSE_LOCK_RELEASE_RECURSIVE() _lock_release_recursive(&s_efuse_lock)
#endif

static int s_batch_writing_mode = 0;

// Public API functions

// read value from EFUSE, writing it into an array
esp_err_t esp_efuse_read_field_blob(const esp_efuse_desc_t* field[], void* dst, size_t dst_size_bits)
{
    esp_err_t err = ESP_OK;
    if (field == NULL || dst == NULL || dst_size_bits == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        do {
            memset((uint8_t *)dst, 0, esp_efuse_utility_get_number_of_items(dst_size_bits, 8));
            err = esp_efuse_utility_process(field, dst, dst_size_bits, esp_efuse_utility_fill_buff);
#ifndef BOOTLOADER_BUILD
            if (err == ESP_ERR_DAMAGED_READING) {
                vTaskDelay(1);
            }
#endif // BOOTLOADER_BUILD
        } while (err == ESP_ERR_DAMAGED_READING);
    }
    return err;
}

bool esp_efuse_read_field_bit(const esp_efuse_desc_t *field[])
{
    uint8_t value = 0;
    esp_err_t err = esp_efuse_read_field_blob(field, &value, 1);
    assert(err == ESP_OK);
    return (err == ESP_OK) && value;
}

// read number of bits programmed as "1" in the particular field
esp_err_t esp_efuse_read_field_cnt(const esp_efuse_desc_t* field[], size_t* out_cnt)
{
    esp_err_t err = ESP_OK;
    if (field == NULL || out_cnt == NULL) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        do {
            *out_cnt = 0;
            err = esp_efuse_utility_process(field, out_cnt, 0, esp_efuse_utility_count_once);
#ifndef BOOTLOADER_BUILD
            if (err == ESP_ERR_DAMAGED_READING) {
                vTaskDelay(1);
            }
#endif // BOOTLOADER_BUILD
        } while (err == ESP_ERR_DAMAGED_READING);
    }
    return err;
}

// write array to EFUSE
esp_err_t esp_efuse_write_field_blob(const esp_efuse_desc_t* field[], const void* src, size_t src_size_bits)
{
    EFUSE_LOCK_ACQUIRE_RECURSIVE();
    esp_err_t err = ESP_OK;
    if (field == NULL || src == NULL || src_size_bits == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        if (s_batch_writing_mode == 0) {
            esp_efuse_utility_reset();
        }
        err = esp_efuse_utility_process(field, (void*)src, src_size_bits, esp_efuse_utility_write_blob);

        if (s_batch_writing_mode == 0) {
            if (err == ESP_OK) {
                err = esp_efuse_utility_apply_new_coding_scheme();
                if (err == ESP_OK) {
                    err = esp_efuse_utility_burn_efuses();
                }
            }
            esp_efuse_utility_reset();
        }
    }
    EFUSE_LOCK_RELEASE_RECURSIVE();
    return err;
}

// program cnt bits to "1"
esp_err_t esp_efuse_write_field_cnt(const esp_efuse_desc_t* field[], size_t cnt)
{
    EFUSE_LOCK_ACQUIRE_RECURSIVE();
    esp_err_t err = ESP_OK;
    if (field == NULL || cnt == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        if (s_batch_writing_mode == 0) {
            esp_efuse_utility_reset();
        }
        err = esp_efuse_utility_process(field, &cnt, 0, esp_efuse_utility_write_cnt);

        if (cnt != 0) {
            ESP_LOGE(TAG, "The required number of bits can not be set. [Not set %d]", cnt);
            err = ESP_ERR_EFUSE_CNT_IS_FULL;
        }
        if (err == ESP_OK_EFUSE_CNT) {
            err = ESP_OK;
        }

        if (s_batch_writing_mode == 0) {
            if (err == ESP_OK) {
                err = esp_efuse_utility_apply_new_coding_scheme();
                if (err == ESP_OK) {
                    err = esp_efuse_utility_burn_efuses();
                }
            }
            esp_efuse_utility_reset();
        }
    }
    EFUSE_LOCK_RELEASE_RECURSIVE();
    return err;
}

esp_err_t esp_efuse_write_field_bit(const esp_efuse_desc_t* field[])
{
    esp_err_t err;
    uint8_t existing = 0;
    const uint8_t one = 1;

    if (field == NULL || field[0]->bit_count != 1) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Check existing value. esp_efuse_write_field_blob() also checks this, but will log an error */
    err = esp_efuse_read_field_blob(field, &existing, 1);
    if (err != ESP_OK || existing) {
        return err; // Error reading, or the bit is already written and we can no-op this
    }

    return esp_efuse_write_field_blob(field, &one, 1);
}

// get the length of the field in bits
int esp_efuse_get_field_size(const esp_efuse_desc_t* field[])
{
    int bits_counter = 0;
    if (field != NULL) {
        int i = 0;
        while (field[i] != NULL) {
            bits_counter += field[i]->bit_count;
            ++i;
        }
    }
    return bits_counter;
}

// reading efuse register.
uint32_t esp_efuse_read_reg(esp_efuse_block_t blk, unsigned int num_reg)
{
    uint32_t ret_val = 0;
    esp_err_t err = esp_efuse_read_block(blk, &ret_val, num_reg * 32, 32);
    assert(err == ESP_OK);
    (void)err;
    return ret_val;
}

// writing efuse register.
esp_err_t esp_efuse_write_reg(esp_efuse_block_t blk, unsigned int num_reg, uint32_t val)
{
    EFUSE_LOCK_ACQUIRE_RECURSIVE();
    if (s_batch_writing_mode == 0) {
        esp_efuse_utility_reset();
    }
    esp_err_t err = esp_efuse_utility_write_reg(blk, num_reg, val);
    if (s_batch_writing_mode == 0) {
        if (err == ESP_OK) {
            err = esp_efuse_utility_apply_new_coding_scheme();
            if (err == ESP_OK) {
                err = esp_efuse_utility_burn_efuses();
            }
        }
        esp_efuse_utility_reset();
    }
    EFUSE_LOCK_RELEASE_RECURSIVE();
    return err;
}

// This function reads the key from the efuse block, starting at the offset and the required size.
esp_err_t esp_efuse_read_block(esp_efuse_block_t blk, void* dst_key, size_t offset_in_bits, size_t size_bits)
{
    esp_err_t err = ESP_OK;
    if (blk == EFUSE_BLK0 || blk >= EFUSE_BLK_MAX || dst_key == NULL || size_bits == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        const esp_efuse_desc_t field_desc[] = {
            {blk, offset_in_bits, size_bits},
        };

        const esp_efuse_desc_t* field[] = {
            &field_desc[0],
            NULL
        };
        err = esp_efuse_read_field_blob(field, dst_key, size_bits);
    }
    return err;
}

// This function writes the key from the efuse block, starting at the offset and the required size.
esp_err_t esp_efuse_write_block(esp_efuse_block_t blk, const void* src_key, size_t offset_in_bits, size_t size_bits)
{
    esp_err_t err = ESP_OK;
    if (blk == EFUSE_BLK0 || blk >= EFUSE_BLK_MAX || src_key == NULL || size_bits == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        const esp_efuse_desc_t field_desc[] = {
            {blk, offset_in_bits, size_bits},
        };

        const esp_efuse_desc_t* field[] = {
            &field_desc[0],
            NULL
        };
        err = esp_efuse_write_field_blob(field, src_key, size_bits);
    }
    return err;
}

esp_err_t esp_efuse_batch_write_begin(void)
{
    EFUSE_LOCK_ACQUIRE_RECURSIVE();
    assert(s_batch_writing_mode >= 0);
    if (++s_batch_writing_mode == 1) {
        esp_efuse_utility_reset();
        ESP_LOGI(TAG, "Batch mode of writing fields is enabled");
    };
    return ESP_OK;
}

esp_err_t esp_efuse_batch_write_cancel(void)
{
    if (s_batch_writing_mode == 0) {
        ESP_LOGE(TAG, "Batch mode was not enabled");
        return ESP_ERR_INVALID_STATE;
    }
    if (--s_batch_writing_mode == 0) {
        esp_efuse_utility_reset();
        ESP_LOGI(TAG, "Batch mode of writing fields is cancelled");
        EFUSE_LOCK_RELEASE_RECURSIVE();
    }
    return ESP_OK;
}

esp_err_t esp_efuse_batch_write_commit(void)
{
    if (s_batch_writing_mode == 0) {
        ESP_LOGE(TAG, "Batch mode was not enabled");
        return ESP_ERR_INVALID_STATE;
    }
    if (--s_batch_writing_mode == 0) {
        esp_err_t err = esp_efuse_utility_apply_new_coding_scheme();
        if (err == ESP_OK) {
            err = esp_efuse_utility_burn_efuses();
            ESP_LOGI(TAG, "Batch mode. Prepared fields are committed");
        } else {
            esp_efuse_utility_reset();
        }
        EFUSE_LOCK_RELEASE_RECURSIVE();
        return err;
    }
    return ESP_OK;
}

esp_err_t esp_efuse_check_errors(void)
{
    return esp_efuse_utility_check_errors();
}

static esp_err_t destroy_block(esp_efuse_block_t block)
{
#if CONFIG_IDF_TARGET_ESP32C2
    bool is_read_protected = esp_efuse_read_field_bit(ESP_EFUSE_RD_DIS_KEY0_LOW) && esp_efuse_read_field_bit(ESP_EFUSE_RD_DIS_KEY0_HI);
#else
    bool is_read_protected = esp_efuse_get_key_dis_read(block);
#endif
    bool is_read_protection_locked = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
    bool is_write_protected = esp_efuse_get_key_dis_write(block);

    // 1. Destroy data in the block, if possible.
    if (!is_write_protected) {
        // The block is not write-protected, so the data in that block can be overwritten.
        // Set the rest unset bits to 1
        // If it is already read-protected then data is all zeros and
        unsigned blk_len_bit = 256;
#if CONFIG_IDF_TARGET_ESP32
        if (esp_efuse_get_coding_scheme(block) == EFUSE_CODING_SCHEME_3_4) {
            blk_len_bit = 192;
        }
#endif // CONFIG_IDF_TARGET_ESP32
        uint32_t data[8 + 3]; // 8 words are data and 3 words are RS coding data
        esp_efuse_read_block(block, data, 0, blk_len_bit);
        // Inverse data to set only unset bit
        for (unsigned i = 0; i < blk_len_bit / 32; i++) {
            data[i] = ~data[i];
        }
        esp_efuse_write_block(block, data, 0, blk_len_bit);
        esp_efuse_utility_burn_chip_opt(true, false);
        ESP_LOGI(TAG, "Data has been destroyed in BLOCK%d", block);
    }

    // 2. Additionally we set the read-protection.
    // Or if data can not be destroyed due to Block is write-protected.
    if (!is_read_protected && !is_read_protection_locked) {
#if CONFIG_IDF_TARGET_ESP32C2
        esp_efuse_write_field_bit(ESP_EFUSE_RD_DIS_KEY0_LOW);
        esp_efuse_write_field_bit(ESP_EFUSE_RD_DIS_KEY0_HI);
#else
        esp_efuse_set_key_dis_read(block);
#endif
        ESP_LOGI(TAG, "Data access has been disabled in BLOCK%d (read-protection is on)", block);
    } else if (is_write_protected) {
        ESP_LOGE(TAG, "Nothing is destroyed, data remains available in BLOCK%d", block);
        ESP_LOGE(TAG, "BLOCK is already write-protected and read protection can not be set either");
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_efuse_destroy_block(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    EFUSE_LOCK_ACQUIRE_RECURSIVE();
    esp_err_t error = destroy_block(block);
    EFUSE_LOCK_RELEASE_RECURSIVE();
    return error;
}
