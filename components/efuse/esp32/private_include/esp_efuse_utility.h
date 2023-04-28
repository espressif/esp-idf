/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_EFUSE_LEN_OF_3_4_SCHEME_BLOCK_IN_BYTES (24)
#define ESP_EFUSE_LEN_OF_REPEAT_BLOCK_IN_BYTES (16)

#define COUNT_EFUSE_REG_PER_BLOCK 8      /* The number of registers per block. */

#define ESP_EFUSE_SECURE_VERSION_NUM_BLOCK EFUSE_BLK3

#define ESP_EFUSE_FIELD_CORRESPONDS_CODING_SCHEME(blk, max_num_bit)                                   \
	esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(blk);                              \
    if ((scheme == EFUSE_CODING_SCHEME_3_4    && max_num_bit > 192) ||                                \
        (scheme == EFUSE_CODING_SCHEME_REPEAT && max_num_bit > 128)) {                                \
    	return false;                                                                                 \
    }

/* @brief Encode one or more sets of 6 byte sequences into
 * 8 bytes suitable for 3/4 Coding Scheme.
 *
 * This function is only useful if the CODING_SCHEME efuse
 * is set to value 1 for 3/4 Coding Scheme.
 *
 * @param[in] in_bytes Pointer to a sequence of bytes to encode for 3/4 Coding Scheme. Must have length in_bytes_len. After being written to hardware, these bytes will read back as little-endian words.
 * @param[out] out_words Pointer to array of words suitable for writing to efuse write registers. Array must contain 2 words (8 bytes) for every 6 bytes in in_bytes_len. Can be a pointer to efuse write registers.
 * @param in_bytes_len. Length of array pointed to by in_bytes, in bytes. Must be a multiple of 6.
 *
 * @return ESP_ERR_INVALID_ARG if either pointer is null or in_bytes_len is not a multiple of 6. ESP_OK otherwise.
 */
esp_err_t esp_efuse_utility_apply_34_encoding(const uint8_t *in_bytes, uint32_t *out_words, size_t in_bytes_len);

#ifdef __cplusplus
}
#endif
