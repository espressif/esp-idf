/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "assert.h"
#include "sdkconfig.h"
#include "esp_efuse_table.h"

/*
 * The chip has only one eFuse key block.
 * There are no eFuse key purpose fields (added only virtual key purposes to support key APIs).
 * Flash Encryption key and Secure Boot key reside in one eFuse block.
 * To burn the FE key and the SB key need to use the batch mode to do it at once.
 */

static esp_err_t esp_efuse_set_key_purpose(esp_efuse_block_t block, esp_efuse_purpose_t purpose);

static __attribute__((unused)) const char *TAG = "efuse";

/**
 * @brief Keys and their attributes are packed into a structure
 */
typedef struct {
    const esp_efuse_desc_t** key;               /**< Key */
    esp_efuse_purpose_t purpose;                /**< purpose of block */
    const esp_efuse_desc_t** key_rd_dis;        /**< Read protection of a key */
    const esp_efuse_desc_t** key_wr_dis;        /**< Write protection of a key*/
} esp_efuse_keys_t;

const esp_efuse_keys_t s_table[EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0] = {
    {ESP_EFUSE_KEY0,            ESP_EFUSE_KEY_PURPOSE_USER,             ESP_EFUSE_RD_DIS_KEY0,     ESP_EFUSE_WR_DIS_KEY0},  // EFUSE_BLK_KEY0
};

// Sets a write protection for the whole block.
esp_err_t esp_efuse_set_write_protect(esp_efuse_block_t blk)
{
    if (blk < EFUSE_BLK_KEY0 || blk >= EFUSE_BLK_KEY_MAX) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    unsigned idx = blk - EFUSE_BLK_KEY0;
    return esp_efuse_write_field_cnt(s_table[idx].key_wr_dis, s_table[idx].key_wr_dis[0]->bit_count);
}

// Sets a read protection for the whole block.
esp_err_t esp_efuse_set_read_protect(esp_efuse_block_t blk)
{
    if (blk < EFUSE_BLK_KEY0 || blk >= EFUSE_BLK_KEY_MAX) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    unsigned idx = blk - EFUSE_BLK_KEY0;
    return esp_efuse_write_field_cnt(s_table[idx].key_rd_dis, s_table[idx].key_rd_dis[0]->bit_count);
}

// get efuse coding_scheme.
esp_efuse_coding_scheme_t esp_efuse_get_coding_scheme(esp_efuse_block_t blk)
{
    esp_efuse_coding_scheme_t scheme;
    if (blk == EFUSE_BLK0) {
        scheme = EFUSE_CODING_SCHEME_NONE;
    } else {
        scheme = EFUSE_CODING_SCHEME_RS;
    }
    return scheme;
}

bool esp_efuse_block_is_empty(esp_efuse_block_t block)
{
    assert(block >= EFUSE_BLK_KEY0 && block < EFUSE_BLK_KEY_MAX);
    const unsigned blk_len_bit = 256;
    uint32_t key[8];
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
    if (esp_efuse_get_key_purpose(block) == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY) {
        uint8_t value = 0;
        esp_err_t err = esp_efuse_read_field_blob(s_table[idx].key_rd_dis, &value, s_table[idx].key_rd_dis[0]->bit_count);
        assert(err == ESP_OK);
        return (err == ESP_OK) && (value == ((1 << s_table[idx].key_rd_dis[0]->bit_count) - 1));
    }
    return esp_efuse_read_field_bit(ESP_EFUSE_RD_DIS_KEY0_LOW);
}

esp_err_t esp_efuse_set_key_dis_read(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (esp_efuse_get_key_purpose(block) == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY) {
        unsigned idx = block - EFUSE_BLK_KEY0;
        uint8_t value = (1 << s_table[idx].key_rd_dis[0]->bit_count) - 1;
        return esp_efuse_write_field_blob(s_table[idx].key_rd_dis, &value, s_table[idx].key_rd_dis[0]->bit_count);
    }
    return esp_efuse_write_field_bit(ESP_EFUSE_RD_DIS_KEY0_LOW);
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
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return ESP_EFUSE_KEY_PURPOSE_MAX;
    }
    if (esp_efuse_read_field_bit(ESP_EFUSE_XTS_KEY_LENGTH_256)) {
        return ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY;
    }
    return ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS;
}


static esp_err_t esp_efuse_set_key_purpose(esp_efuse_block_t block, esp_efuse_purpose_t purpose)
{
    (void)block;
    if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY) {
        return esp_efuse_write_field_bit(ESP_EFUSE_XTS_KEY_LENGTH_256);
    }
    return ESP_OK;
}

bool esp_efuse_get_keypurpose_dis_write(esp_efuse_block_t block)
{
    (void)block;
    return true;
}

bool esp_efuse_find_purpose(esp_efuse_purpose_t purpose, esp_efuse_block_t *block)
{
    (void)purpose;
    esp_efuse_block_t dummy;
    if (block == NULL) {
        block = &dummy;
    }
    *block = EFUSE_BLK_KEY0;
    return true;
}

esp_err_t esp_efuse_write_key(esp_efuse_block_t block, esp_efuse_purpose_t purpose, const void *key, size_t key_size_bytes)
{
    esp_err_t err = ESP_OK;
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX || key_size_bytes > 32 || purpose >= ESP_EFUSE_KEY_PURPOSE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((purpose == ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2 || purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS) && (key_size_bytes != 16)) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_efuse_batch_write_begin();

    if (!esp_efuse_key_block_unused(block)) {
        err = ESP_ERR_INVALID_STATE;
    } else {
        size_t offset_in_bits = (purpose == ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2) ? 16 * 8 : 0;
        ESP_EFUSE_CHK(esp_efuse_write_block(block, key, offset_in_bits, key_size_bytes * 8));
        if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY) {
            ESP_EFUSE_CHK(esp_efuse_set_key_purpose(block, purpose));
            ESP_EFUSE_CHK(esp_efuse_write_field_bit(ESP_EFUSE_RD_DIS_KEY0_LOW));
            ESP_EFUSE_CHK(esp_efuse_write_field_bit(ESP_EFUSE_RD_DIS_KEY0_HI));
        } else if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS) {
            ESP_EFUSE_CHK(esp_efuse_write_field_bit(ESP_EFUSE_RD_DIS_KEY0_LOW));
        }
        ESP_EFUSE_CHK(esp_efuse_set_key_dis_write(block));
        return esp_efuse_batch_write_commit();
    }
err_exit:
    esp_efuse_batch_write_cancel();
    return err;
}

esp_err_t esp_efuse_write_keys(const esp_efuse_purpose_t purposes[], uint8_t keys[][32], unsigned number_of_keys)
{
    esp_err_t err = ESP_FAIL;
    if (number_of_keys == 0 || number_of_keys > 2 || keys == NULL || purposes == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_efuse_purpose_t purpose = 0;

    esp_efuse_batch_write_begin();
    for (unsigned i_key = 0; i_key < number_of_keys; i_key++) {
        purpose = purposes[i_key];
        ESP_LOGI(TAG, "Writing EFUSE_BLK_KEY0 with purpose %d", purpose);
        size_t key_size = (purpose == ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2 || purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS) ? 16 : 32;
        ESP_EFUSE_CHK(esp_efuse_write_key(EFUSE_BLK_KEY0, purpose, keys[i_key], key_size));
    }
    return esp_efuse_batch_write_commit();
err_exit:
    ESP_LOGE(TAG, "Failed to write EFUSE_BLK_KEY0 with purpose %d. Can't continue.", purpose);
    esp_efuse_batch_write_cancel();
    return err;
}

esp_err_t esp_secure_boot_read_key_digests(esp_secure_boot_key_digests_t *trusted_keys)
{
    if (trusted_keys == NULL) {
        return ESP_FAIL;
    }
    trusted_keys->key_digests[0] = (const void *)(esp_efuse_utility_get_read_register_address(EFUSE_BLK_KEY0) + 16);
    return ESP_OK;
}
