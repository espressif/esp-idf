/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
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
 * Each eFuse key block has a special field that defines the purpose of this block.
 * This special field is called key_purpose.
 */

const static char *TAG = "efuse";

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
    const esp_efuse_purpose_t digest_purpose;
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

#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY
const esp_efuse_revokes_t s_revoke_table[] = {
    {ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0, ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0, ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0},
    {ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1, ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1, ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1},
    {ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2, ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2, ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2},
};
#endif
bool esp_efuse_block_is_empty(esp_efuse_block_t block)
{
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

// Sets a write protection for the whole block.
esp_err_t esp_efuse_set_write_protect(esp_efuse_block_t blk)
{
    if (blk == EFUSE_BLK1) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_BLK1, 1);
    } else if (blk == EFUSE_BLK2) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_SYS_DATA_PART1, 1);
    } else if (blk == EFUSE_BLK3) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_USER_DATA, 1);
    } else if (blk == EFUSE_BLK10) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_WR_DIS_SYS_DATA_PART2, 1);
    } else if (blk >= EFUSE_BLK_KEY0 && blk < EFUSE_BLK_KEY_MAX) {
        unsigned idx = blk - EFUSE_BLK_KEY0;
        return esp_efuse_write_field_cnt(s_table[idx].key_wr_dis, 1);
    }
    return ESP_ERR_NOT_SUPPORTED;
}

// read protect for blk.
esp_err_t esp_efuse_set_read_protect(esp_efuse_block_t blk)
{
    if (blk >= EFUSE_BLK_KEY0 && blk < EFUSE_BLK_KEY_MAX) {
        unsigned idx = blk - EFUSE_BLK_KEY0;
        return esp_efuse_write_field_cnt(s_table[idx].key_rd_dis, 1);
    }
    else if (blk == EFUSE_BLK10) {
        return esp_efuse_write_field_cnt(ESP_EFUSE_RD_DIS_SYS_DATA_PART2, 1);
    }
    return ESP_ERR_NOT_SUPPORTED;

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

const esp_efuse_desc_t **esp_efuse_get_purpose_field(esp_efuse_block_t block)
{
    if (block < EFUSE_BLK_KEY0 || block >= EFUSE_BLK_KEY_MAX) {
        return NULL;
    }
    unsigned idx = block - EFUSE_BLK_KEY0;
    return s_table[idx].keypurpose;
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
    return esp_efuse_write_field_bit(s_table[idx].keypurpose_wr_dis);
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

    if (!esp_efuse_block_is_empty(block)) {
        return false; // Block in use!
    }

    return true; // Unused
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

#if SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK
        if (block == EFUSE_BLK9 && (
#if SOC_FLASH_ENCRYPTION_XTS_AES_256
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1 ||
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2 ||
#endif
#if SOC_ECDSA_SUPPORTED
            purpose == ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY ||
#endif
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY)) {
            ESP_LOGE(TAG, "BLOCK9 can not have the %d purpose because of HW bug (see TRM for more details)", purpose);
            err = ESP_ERR_NOT_SUPPORTED;
            goto err_exit;
        }
#endif // SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK

        if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY ||
#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_256
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1 ||
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2 ||
#endif //#ifdef SOC_EFUSE_SUPPORT_XTS_AES_256_KEYS
#if SOC_ECDSA_SUPPORTED
            purpose == ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY ||
#endif
#if SOC_KEY_MANAGER_SUPPORTED
            purpose == ESP_EFUSE_KEY_PURPOSE_KM_INIT_KEY ||
#endif
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
            ESP_EFUSE_CHK(esp_efuse_set_key_dis_read(block));
        }
#if SOC_EFUSE_ECDSA_USE_HARDWARE_K
        if (purpose == ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY) {
            // Permanently enable the hardware TRNG supplied k mode (most secure mode)
            ESP_EFUSE_CHK(esp_efuse_write_field_bit(ESP_EFUSE_ECDSA_FORCE_USE_HARDWARE_K));
        }
#endif
        ESP_EFUSE_CHK(esp_efuse_set_key_purpose(block, purpose));
        ESP_EFUSE_CHK(esp_efuse_set_keypurpose_dis_write(block));
        return esp_efuse_batch_write_commit();
    }
err_exit:
    esp_efuse_batch_write_cancel();
    return err;
}

esp_err_t esp_efuse_write_keys(const esp_efuse_purpose_t purposes[], uint8_t keys[][32], unsigned number_of_keys)
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


#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY
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

esp_err_t esp_secure_boot_read_key_digests(esp_secure_boot_key_digests_t *trusted_keys)
{
    bool found = false;
    esp_efuse_block_t key_block;

    if (trusted_keys == NULL) {
        return ESP_FAIL;
    }

    for (unsigned i = 0; i < SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS; i++) {
        trusted_keys->key_digests[i] = NULL;
        if (esp_efuse_get_digest_revoke(i)) {
            continue;
        }

        // Anti-FI check that this efuse really is not revoked
        assert(esp_efuse_get_digest_revoke(i) == 0);

        if (!esp_efuse_find_purpose(s_revoke_table[i].digest_purpose, &key_block)) {
            continue;
        }
        trusted_keys->key_digests[i] = (const void *)esp_efuse_utility_get_read_register_address(key_block);
        found = found || (trusted_keys->key_digests[i] != NULL);
    }

    if (!found) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif //#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY
