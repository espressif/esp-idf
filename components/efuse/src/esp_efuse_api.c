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
#define EFUSE_LOCK_ACQUIRE_RECURSIVE()
#define EFUSE_LOCK_RELEASE_RECURSIVE()
#else
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
    EFUSE_LOCK_ACQUIRE_RECURSIVE();
    esp_err_t err = ESP_OK;
    if (field == NULL || dst == NULL || dst_size_bits == 0) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        memset((uint8_t *)dst, 0, esp_efuse_utility_get_number_of_items(dst_size_bits, 8));
        err = esp_efuse_utility_process(field, dst, dst_size_bits, esp_efuse_utility_fill_buff);
    }
    EFUSE_LOCK_RELEASE_RECURSIVE();
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
    EFUSE_LOCK_ACQUIRE_RECURSIVE();
    esp_err_t err = ESP_OK;
    if (field == NULL || out_cnt == NULL) {
        err = ESP_ERR_INVALID_ARG;
    } else {
        *out_cnt = 0;
        err = esp_efuse_utility_process(field, out_cnt, 0, esp_efuse_utility_count_once);
    }
    EFUSE_LOCK_RELEASE_RECURSIVE();
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
                    esp_efuse_utility_burn_efuses();
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
                    esp_efuse_utility_burn_efuses();
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
    EFUSE_LOCK_ACQUIRE_RECURSIVE();
    uint32_t ret_val = esp_efuse_utility_read_reg(blk, num_reg);
    EFUSE_LOCK_RELEASE_RECURSIVE();
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
                esp_efuse_utility_burn_efuses();
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
            esp_efuse_utility_burn_efuses();
            ESP_LOGI(TAG, "Batch mode. Prepared fields are committed");
        } else {
            esp_efuse_utility_reset();
        }
        EFUSE_LOCK_RELEASE_RECURSIVE();
        return err;
    }
    return ESP_OK;
}


#ifndef CONFIG_IDF_TARGET_ESP32

/**
 * @brief Keys and their attributes are packed into a structure
 */
typedef struct {
    const esp_efuse_desc_t** key;               /**< Key */
    const esp_efuse_desc_t** keypurpose;        /**< Key purpose */
    const esp_efuse_desc_t** key_rd_dis;        /**< Read protection of a key */
    const esp_efuse_desc_t** key_wr_dis;        /**< Write protection of a key*/
    const esp_efuse_desc_t** keypurpose_wr_dis; /**< Write protection of a key purpose*/
} esp_efuse_keys_t;

typedef struct {
    const esp_efuse_desc_t** revoke;
    const esp_efuse_desc_t** revoke_wr_dis;
} esp_efuse_revokes_t;

const esp_efuse_keys_t s_table[EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0] = {
    {ESP_EFUSE_KEY0, ESP_EFUSE_KEY_PURPOSE_0, ESP_EFUSE_RD_DIS_KEY0, ESP_EFUSE_WR_DIS_KEY0, ESP_EFUSE_WR_DIS_KEY0_PURPOSE},
    {ESP_EFUSE_KEY1, ESP_EFUSE_KEY_PURPOSE_1, ESP_EFUSE_RD_DIS_KEY1, ESP_EFUSE_WR_DIS_KEY1, ESP_EFUSE_WR_DIS_KEY1_PURPOSE},
    {ESP_EFUSE_KEY2, ESP_EFUSE_KEY_PURPOSE_2, ESP_EFUSE_RD_DIS_KEY2, ESP_EFUSE_WR_DIS_KEY2, ESP_EFUSE_WR_DIS_KEY2_PURPOSE},
    {ESP_EFUSE_KEY3, ESP_EFUSE_KEY_PURPOSE_3, ESP_EFUSE_RD_DIS_KEY3, ESP_EFUSE_WR_DIS_KEY3, ESP_EFUSE_WR_DIS_KEY3_PURPOSE},
    {ESP_EFUSE_KEY4, ESP_EFUSE_KEY_PURPOSE_4, ESP_EFUSE_RD_DIS_KEY4, ESP_EFUSE_WR_DIS_KEY4, ESP_EFUSE_WR_DIS_KEY4_PURPOSE},
    {ESP_EFUSE_KEY5, ESP_EFUSE_KEY_PURPOSE_5, ESP_EFUSE_RD_DIS_KEY5, ESP_EFUSE_WR_DIS_KEY5, ESP_EFUSE_WR_DIS_KEY5_PURPOSE},
#if 0
    {ESP_EFUSE_KEY6, ESP_EFUSE_KEY_PURPOSE_6, ESP_EFUSE_RD_DIS_KEY6, ESP_EFUSE_WR_DIS_KEY6, ESP_EFUSE_WR_DIS_KEY6_PURPOSE},
#endif
};

const esp_efuse_revokes_t s_revoke_table[] = {
    {ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0, ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0},
    {ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1, ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1},
    {ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2, ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2},
};

#define ESP_EFUSE_CHK(ret)       \
    do                           \
    {                            \
        if( ( err = (ret) ) != ESP_OK ) \
            goto err_exit;        \
    } while( 0 )


const esp_efuse_desc_t **esp_efuse_get_purpose_field(esp_efuse_block_t block)
{
    switch(block) {
    case EFUSE_BLK_KEY0:
        return ESP_EFUSE_KEY_PURPOSE_0;
    case EFUSE_BLK_KEY1:
        return ESP_EFUSE_KEY_PURPOSE_1;
    case EFUSE_BLK_KEY2:
        return ESP_EFUSE_KEY_PURPOSE_2;
    case EFUSE_BLK_KEY3:
        return ESP_EFUSE_KEY_PURPOSE_3;
    case EFUSE_BLK_KEY4:
        return ESP_EFUSE_KEY_PURPOSE_4;
    case EFUSE_BLK_KEY5:
        return ESP_EFUSE_KEY_PURPOSE_5;
    default:
        return NULL;
    }
}

const esp_efuse_desc_t** esp_efuse_get_key(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return NULL;
    }
    unsigned idx = block - EFUSE_BLK_KEY0;
    return s_table[idx].key;
}

bool esp_efuse_get_key_dis_read(esp_efuse_block_t block)
{
    assert(block >= EFUSE_BLK_KEY0 && block < EFUSE_BLK_KEY_MAX);
    unsigned idx = block - EFUSE_BLK_KEY0;
    return esp_efuse_read_field_bit(s_table[idx].key_rd_dis);
}

esp_err_t esp_efuse_set_key_dis_read(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    unsigned idx = block - EFUSE_BLK_KEY0;
    const uint8_t one = 1;
    return esp_efuse_write_field_blob(s_table[idx].key_rd_dis, &one, 1);
}

bool esp_efuse_get_key_dis_write(esp_efuse_block_t block)
{
    assert(block >= EFUSE_BLK_KEY0 && block < EFUSE_BLK_KEY_MAX);
    unsigned idx = block - EFUSE_BLK_KEY0;
    return esp_efuse_read_field_bit(s_table[idx].key_wr_dis);
}

esp_err_t esp_efuse_set_key_dis_write(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    unsigned idx = block - EFUSE_BLK_KEY0;
    const uint8_t one = 1;
    return esp_efuse_write_field_blob(s_table[idx].key_wr_dis, &one, 1);
}

esp_efuse_purpose_t esp_efuse_get_key_purpose(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return ESP_EFUSE_KEY_PURPOSE_MAX;
    }
    unsigned idx = block - EFUSE_BLK_KEY0;
    uint8_t value = 0;
    esp_err_t err = esp_efuse_read_field_blob(s_table[idx].keypurpose, &value, s_table[idx].keypurpose[0]->bit_count);
    if (err != ESP_OK) {
        return ESP_EFUSE_KEY_PURPOSE_MAX;
    }
    return value;
}

esp_err_t esp_efuse_set_key_purpose(esp_efuse_block_t block, esp_efuse_purpose_t purpose)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    unsigned idx = block - EFUSE_BLK_KEY0;
    return esp_efuse_write_field_blob(s_table[idx].keypurpose, &purpose, s_table[idx].keypurpose[0]->bit_count);
}

bool esp_efuse_get_keypurpose_dis_write(esp_efuse_block_t block)
{
    assert(block >= EFUSE_BLK_KEY0 && block < EFUSE_BLK_KEY_MAX);
    unsigned idx = block - EFUSE_BLK_KEY0;
    return esp_efuse_read_field_bit(s_table[idx].keypurpose_wr_dis);
}

esp_err_t esp_efuse_set_keypurpose_dis_write(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    unsigned idx = block - EFUSE_BLK_KEY0;
    const uint8_t one = 1;
    return esp_efuse_write_field_blob(s_table[idx].keypurpose_wr_dis, &one, 1);
}

bool esp_efuse_find_purpose(esp_efuse_purpose_t purpose, esp_efuse_block_t *block)
{
    esp_efuse_block_t dummy;
    if (block == NULL) {
        block = &dummy;
    }

    for (esp_efuse_block_t b = EFUSE_BLK_KEY0; b < EFUSE_BLK_KEY_MAX; b++) {
        if (esp_efuse_get_key_purpose(b) == purpose) {
            *block = b;
            return true;
        }
    }

    return false;
}

esp_efuse_block_t esp_efuse_find_unused_key_block(void)
{
    for (esp_efuse_block_t b = EFUSE_BLK_KEY0; b < EFUSE_BLK_KEY_MAX; b++) {
        if (esp_efuse_key_block_unused(b)) {
                return b;
        }
    }
    return EFUSE_BLK_KEY_MAX; // nothing
}

unsigned esp_efuse_count_unused_key_blocks(void)
{
    unsigned r = 0;
    for (esp_efuse_block_t b = EFUSE_BLK_KEY0; b < EFUSE_BLK_KEY_MAX; b++) {
        if (esp_efuse_key_block_unused(b)) {
            r++;
        }
    }
    return r;
}

bool esp_efuse_key_block_unused(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return false; // Not a key block
    }

    if (esp_efuse_get_key_purpose(block) != ESP_EFUSE_KEY_PURPOSE_USER ||
            esp_efuse_get_keypurpose_dis_write(block) ||
            esp_efuse_get_key_dis_read(block) ||
            esp_efuse_get_key_dis_write(block)) {
        return false; // Block in use!
    }

    for (int i = 0; i < 8; ++i) {
        if (esp_efuse_read_reg(block, i) != 0) {
            return false; // Block in use!
        }
    }

    return true; // Unused
}

bool esp_efuse_get_digest_revoke(unsigned num_digest)
{
    assert(num_digest < sizeof(s_revoke_table) / sizeof(esp_efuse_revokes_t));
    return esp_efuse_read_field_bit(s_revoke_table[num_digest].revoke);
}

esp_err_t esp_efuse_set_digest_revoke(unsigned num_digest)
{
    if (num_digest >= sizeof(s_revoke_table) / sizeof(esp_efuse_revokes_t)) {
        return ESP_ERR_INVALID_ARG;
    }
    return esp_efuse_write_field_bit(s_revoke_table[num_digest].revoke);
}

bool esp_efuse_get_write_protect_of_digest_revoke(unsigned num_digest)
{
    assert(num_digest < sizeof(s_revoke_table) / sizeof(esp_efuse_revokes_t));
    return esp_efuse_read_field_bit(s_revoke_table[num_digest].revoke_wr_dis);
}

esp_err_t esp_efuse_set_write_protect_of_digest_revoke(unsigned num_digest)
{
    if (num_digest >= sizeof(s_revoke_table) / sizeof(esp_efuse_revokes_t)) {
        return ESP_ERR_INVALID_ARG;
    }
    return esp_efuse_write_field_bit(s_revoke_table[num_digest].revoke_wr_dis);
}

esp_err_t esp_efuse_write_key(esp_efuse_block_t block, esp_efuse_purpose_t purpose, const void *key, size_t key_size_bytes)
{
    esp_err_t err = ESP_OK;
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX || key_size_bytes > 32 || purpose >= ESP_EFUSE_KEY_PURPOSE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_efuse_batch_write_begin();

    if (!esp_efuse_key_block_unused(block)) {
        err = ESP_ERR_INVALID_STATE;
    } else {
        unsigned idx = block - EFUSE_BLK_KEY0;
        ESP_EFUSE_CHK(esp_efuse_write_field_blob(s_table[idx].key, key, key_size_bytes * 8));
        ESP_EFUSE_CHK(esp_efuse_set_key_dis_write(block));
        if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1 ||
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2 ||
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
            ESP_EFUSE_CHK(esp_efuse_set_key_dis_read(block));
        }
        ESP_EFUSE_CHK(esp_efuse_set_key_purpose(block, purpose));
        ESP_EFUSE_CHK(esp_efuse_set_keypurpose_dis_write(block));
        return esp_efuse_batch_write_commit();
    }
err_exit:
    esp_efuse_batch_write_cancel();
    return err;
}

esp_err_t esp_efuse_write_keys(esp_efuse_purpose_t purposes[], uint8_t keys[][32], unsigned number_of_keys)
{
    esp_err_t err = ESP_OK;
    if (number_of_keys == 0 || number_of_keys > (EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0) || keys == NULL || purposes == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_efuse_purpose_t purpose = 0;
    esp_efuse_block_t block = EFUSE_BLK_KEY0;

    esp_efuse_batch_write_begin();

    unsigned unused_keys = esp_efuse_count_unused_key_blocks();
    if (number_of_keys > unused_keys) {
        ESP_LOGE(TAG, "Not enough unused key blocks available. Required %d, was %d", number_of_keys, unused_keys);
        err = ESP_ERR_NOT_ENOUGH_UNUSED_KEY_BLOCKS;
    } else {
        for (int i_key = 0; (block < EFUSE_BLK_KEY_MAX) && (i_key < number_of_keys); block++) {
            if (esp_efuse_key_block_unused(block)) {
                purpose = purposes[i_key];
                ESP_LOGI(TAG, "Writing EFUSE_BLK_KEY%d with purpose %d", block - EFUSE_BLK_KEY0, purpose);
                ESP_EFUSE_CHK(esp_efuse_write_key(block, purpose, keys[i_key], 32));
                i_key++;
            }
        }
        return esp_efuse_batch_write_commit();
err_exit:
        ESP_LOGE(TAG, "Failed to write EFUSE_BLK_KEY%d with purpose %d. Can't continue.", block - EFUSE_BLK_KEY0, purpose);
    }
    esp_efuse_batch_write_cancel();
    return err;
}

#endif // not CONFIG_IDF_TARGET_ESP32
