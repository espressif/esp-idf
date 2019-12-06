// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "assert.h"
#include "sdkconfig.h"
#include "esp_efuse_table.h"

const static char *TAG = "efuse";

#if defined(BOOTLOADER_BUILD)
#define EFUSE_LOCK_ACQUIRE()
#define EFUSE_LOCK_RELEASE()
#define EFUSE_LOCK_ACQUIRE_RUCURSIVE()
#define EFUSE_LOCK_RELEASE_RUCURSIVE()
#else
#include <sys/lock.h>
static _lock_t s_efuse_lock;
#define EFUSE_LOCK_ACQUIRE() _lock_acquire(&s_efuse_lock)
#define EFUSE_LOCK_RELEASE() _lock_release(&s_efuse_lock)
#define EFUSE_LOCK_ACQUIRE_RUCURSIVE() _lock_acquire_recursive(&s_efuse_lock)
#define EFUSE_LOCK_RELEASE_RUCURSIVE() _lock_release_recursive(&s_efuse_lock)
#endif

static bool s_batch_writing_mode = false;

// Public API functions

// read value from EFUSE, writing it into an array
esp_err_t esp_efuse_read_field_blob(const esp_efuse_desc_t* field[], void* dst, size_t dst_size_bits)
{
    EFUSE_LOCK_ACQUIRE();
    esp_err_t err = ESP_OK;
    if (field == NULL || dst == NULL || dst_size_bits == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        memset((uint8_t *)dst, 0, esp_efuse_utility_get_number_of_items(dst_size_bits, 8));
        err = esp_efuse_utility_process(field, dst, dst_size_bits, esp_efuse_utility_fill_buff);
    }
    EFUSE_LOCK_RELEASE();
    return err;
}

// read number of bits programmed as "1" in the particular field
esp_err_t esp_efuse_read_field_cnt(const esp_efuse_desc_t* field[], size_t* out_cnt)
{
    EFUSE_LOCK_ACQUIRE();
    esp_err_t err = ESP_OK;
    if (field == NULL || out_cnt == NULL) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        *out_cnt = 0;
        err = esp_efuse_utility_process(field, out_cnt, 0, esp_efuse_utility_count_once);
    }
    EFUSE_LOCK_RELEASE();
    return err;
}

// write array to EFUSE
esp_err_t esp_efuse_write_field_blob(const esp_efuse_desc_t* field[], const void* src, size_t src_size_bits)
{
    EFUSE_LOCK_ACQUIRE_RUCURSIVE();
    esp_err_t err = ESP_OK;
    if (field == NULL || src == NULL || src_size_bits == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        if (s_batch_writing_mode == false) {
            esp_efuse_utility_reset();
        }
        err = esp_efuse_utility_process(field, (void*)src, src_size_bits, esp_efuse_utility_write_blob);

        if (s_batch_writing_mode == false) {
            if (err == ESP_OK) {
                err = esp_efuse_utility_apply_new_coding_scheme();
                if (err == ESP_OK) {
                    esp_efuse_utility_burn_efuses();
                }
            }
            esp_efuse_utility_reset();
        }
    }
    EFUSE_LOCK_RELEASE_RUCURSIVE();
    return err;
}

// program cnt bits to "1"
esp_err_t esp_efuse_write_field_cnt(const esp_efuse_desc_t* field[], size_t cnt)
{
    EFUSE_LOCK_ACQUIRE_RUCURSIVE();
    esp_err_t err = ESP_OK;
    if (field == NULL || cnt == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        if (s_batch_writing_mode == false) {
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

        if (s_batch_writing_mode == false) {
            if (err == ESP_OK) {
                err = esp_efuse_utility_apply_new_coding_scheme();
                if (err == ESP_OK) {
                    esp_efuse_utility_burn_efuses();
                }
            }
            esp_efuse_utility_reset();
        }
    }
    EFUSE_LOCK_RELEASE_RUCURSIVE();
    return err;
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
    EFUSE_LOCK_ACQUIRE();
    uint32_t ret_val = esp_efuse_utility_read_reg(blk, num_reg);
    EFUSE_LOCK_RELEASE();
    return ret_val;
}

// writing efuse register.
esp_err_t esp_efuse_write_reg(esp_efuse_block_t blk, unsigned int num_reg, uint32_t val)
{
    EFUSE_LOCK_ACQUIRE_RUCURSIVE();
    if (s_batch_writing_mode == false) {
        esp_efuse_utility_reset();
    }
    esp_err_t err = esp_efuse_utility_write_reg(blk, num_reg, val);
    if (s_batch_writing_mode == false) {
        if (err == ESP_OK) {
            err = esp_efuse_utility_apply_new_coding_scheme();
            if (err == ESP_OK) {
                esp_efuse_utility_burn_efuses();
            }
        }
        esp_efuse_utility_reset();
    }
    EFUSE_LOCK_RELEASE_RUCURSIVE();
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
    EFUSE_LOCK_ACQUIRE();
    s_batch_writing_mode = true;
    esp_efuse_utility_reset();
    ESP_LOGI(TAG, "Batch mode of writing fields is enabled");
    return ESP_OK;
}

esp_err_t esp_efuse_batch_write_cancel(void)
{
    if (s_batch_writing_mode == true) {
        s_batch_writing_mode = false;
        esp_efuse_utility_reset();
        ESP_LOGI(TAG, "Batch mode of writing fields is disabled");
        EFUSE_LOCK_RELEASE();
        return ESP_OK;
    } else {
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t esp_efuse_batch_write_commit(void)
{
    if (s_batch_writing_mode == false) {
        return ESP_ERR_INVALID_STATE;
    } else {
        esp_err_t err = esp_efuse_utility_apply_new_coding_scheme();
        if (err == ESP_OK) {
            esp_efuse_utility_burn_efuses();
        }
        esp_efuse_batch_write_cancel();
        return err;
    }
}
