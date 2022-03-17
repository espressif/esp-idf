/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "hal/efuse_ll.h"
#include "assert.h"
#include "sdkconfig.h"
#include "esp_efuse_table.h"

/*
 * FE uses eFuse block1.
 * SB uses eFuse block2.
 * Block3 can be used by customer.
 * There are no eFuse key purpose fields (added only virtual key purposes to support key APIs).
 */

static __attribute__((unused)) const char *TAG = "efuse";

/**
 * @brief Keys and their attributes are packed into a structure
 */
typedef struct {
    const esp_efuse_desc_t** key_rd_dis;        /**< Read protection of a key */
    const esp_efuse_desc_t** key_wr_dis;        /**< Write protection of a key*/
} esp_efuse_keys_t;

const esp_efuse_keys_t s_table[EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0] = {
    {ESP_EFUSE_RD_DIS_BLK1, ESP_EFUSE_WR_DIS_BLK1},
    {ESP_EFUSE_RD_DIS_BLK2, ESP_EFUSE_WR_DIS_BLK2},
    {ESP_EFUSE_RD_DIS_BLK3, ESP_EFUSE_WR_DIS_BLK3},
};


// Sets a write protection for the whole block.
esp_err_t esp_efuse_set_write_protect(esp_efuse_block_t blk)
{
    if (blk == EFUSE_BLK0 || blk >= EFUSE_BLK_MAX) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    unsigned idx = blk - EFUSE_BLK1;
    return esp_efuse_write_field_cnt(s_table[idx].key_wr_dis, 1);
}

// read protect for blk.
esp_err_t esp_efuse_set_read_protect(esp_efuse_block_t blk)
{
    if (blk == EFUSE_BLK0 || blk >= EFUSE_BLK_MAX) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    unsigned idx = blk - EFUSE_BLK1;
    return esp_efuse_write_field_cnt(s_table[idx].key_rd_dis, 1);
}

// get efuse coding_scheme.
esp_efuse_coding_scheme_t esp_efuse_get_coding_scheme(esp_efuse_block_t blk)
{
    esp_efuse_coding_scheme_t scheme;
    if (blk == EFUSE_BLK0) {
        scheme = EFUSE_CODING_SCHEME_NONE;
    } else {
        uint32_t coding_scheme = efuse_ll_get_coding_scheme();
        if (coding_scheme == EFUSE_CODING_SCHEME_VAL_NONE ||
            coding_scheme == (EFUSE_CODING_SCHEME_VAL_34 | EFUSE_CODING_SCHEME_VAL_REPEAT)) {
            scheme = EFUSE_CODING_SCHEME_NONE;
        } else if (coding_scheme == EFUSE_CODING_SCHEME_VAL_34) {
            scheme = EFUSE_CODING_SCHEME_3_4;
        } else {
            scheme = EFUSE_CODING_SCHEME_REPEAT;
        }
    }
    return scheme;
}

bool esp_efuse_block_is_empty(esp_efuse_block_t block)
{
    unsigned blk_len_bit = 256;
    uint32_t key[8];
    if (esp_efuse_get_coding_scheme(block) == EFUSE_CODING_SCHEME_3_4) {
        blk_len_bit = 192;
    }
    esp_err_t err = esp_efuse_read_block(block, &key, 0, blk_len_bit);
    if (err != ESP_OK) {
        return false;
    }
    unsigned zeros = 0;
    for (unsigned i = 0; i < blk_len_bit / 32; ++i) {
        if (key[i] == 0) {
            ++zeros;
        }
    }
    if (zeros == blk_len_bit / 32) {
        return true;
    }
    return false;
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
    return esp_efuse_write_field_bit(s_table[idx].key_rd_dis);
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
    return esp_efuse_write_field_bit(s_table[idx].key_wr_dis);
}

bool esp_efuse_key_block_unused(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return false; // Not a key block
    }

    if (esp_efuse_get_key_dis_read(block) || esp_efuse_get_key_dis_write(block) ||
            !esp_efuse_block_is_empty(block)) {
        return false; // Block in use!
    }

    return true; // Unused
}

esp_efuse_purpose_t esp_efuse_get_key_purpose(esp_efuse_block_t block)
{
    esp_efuse_purpose_t ret_purpose;
    if (block == EFUSE_BLK0) {
        ret_purpose = ESP_EFUSE_KEY_PURPOSE_SYSTEM;
    } else if (block == EFUSE_BLK_ENCRYPT_FLASH) {
        ret_purpose = ESP_EFUSE_KEY_PURPOSE_FLASH_ENCRYPTION;
    } else if (block == EFUSE_BLK_SECURE_BOOT) {
        ret_purpose = ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2;
    } else if (block == EFUSE_BLK3) {
        ret_purpose = ESP_EFUSE_KEY_PURPOSE_USER;
    } else {
        ret_purpose = ESP_EFUSE_KEY_PURPOSE_MAX;
    }
    return ret_purpose;
}

bool esp_efuse_get_keypurpose_dis_write(esp_efuse_block_t block)
{
    (void)block;
    return true;
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

esp_err_t esp_efuse_write_key(esp_efuse_block_t block, esp_efuse_purpose_t purpose, const void *key, size_t key_size_bytes)
{
    esp_err_t err = ESP_OK;
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX || key_size_bytes > 32 || purpose >= ESP_EFUSE_KEY_PURPOSE_MAX
        || esp_efuse_get_key_purpose(block) != purpose) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_efuse_batch_write_begin();

    if (!esp_efuse_key_block_unused(block)) {
        err = ESP_ERR_INVALID_STATE;
    } else {
        ESP_EFUSE_CHK(esp_efuse_write_block(block, key, 0, key_size_bytes * 8));
        ESP_EFUSE_CHK(esp_efuse_set_key_dis_write(block));
        if (purpose == ESP_EFUSE_KEY_PURPOSE_FLASH_ENCRYPTION) {
            ESP_EFUSE_CHK(esp_efuse_set_key_dis_read(block));
        }
        return esp_efuse_batch_write_commit();
    }
err_exit:
    esp_efuse_batch_write_cancel();
    return err;
}

esp_err_t esp_efuse_write_keys(const esp_efuse_purpose_t purposes[], uint8_t keys[][32], unsigned number_of_keys)
{
    esp_err_t err = ESP_FAIL;
    if (number_of_keys == 0 || number_of_keys > (EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0) || keys == NULL || purposes == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t key_size = 32;
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(EFUSE_BLK_KEY0);
    if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
        key_size = 24;
    }
    esp_efuse_purpose_t purpose = 0;
    esp_efuse_block_t block = EFUSE_BLK_KEY0;

    esp_efuse_batch_write_begin();
    for (unsigned i_key = 0; (block < EFUSE_BLK_KEY_MAX) && (i_key < number_of_keys); block++) {
        purpose = purposes[i_key];
        if (esp_efuse_get_key_purpose(block) == purpose) {
            ESP_LOGI(TAG, "Writing EFUSE_BLK_KEY%d with purpose %d", block - EFUSE_BLK_KEY0, purpose);
            ESP_EFUSE_CHK(esp_efuse_write_key(block, purpose, keys[i_key], key_size));
            i_key++;
        }
    }
    return esp_efuse_batch_write_commit();
err_exit:
    ESP_LOGE(TAG, "Failed to write EFUSE_BLK_KEY%d with purpose %d. Can't continue.", block - EFUSE_BLK_KEY0, purpose);
    esp_efuse_batch_write_cancel();
    return err;
}

#if CONFIG_ESP32_REV_MIN_FULL >= 300
esp_err_t esp_secure_boot_read_key_digests(esp_secure_boot_key_digests_t *trusted_keys)
{
    if (trusted_keys == NULL) {
        return ESP_FAIL;
    }
    trusted_keys->key_digests[0] = (const void *)esp_efuse_utility_get_read_register_address(EFUSE_BLK_SECURE_BOOT);
    return ESP_OK;
}
#endif // CONFIG_ESP32_REV_MIN_FULL >= 300
