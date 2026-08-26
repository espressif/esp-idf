/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_rom_crc.h"
#include "hal/efuse_hal.h"
#include "esp_efuse.h"
#include "esp_efuse_utility.h"
#include "esp_private/log_lock.h"
#include "esp_private/log_util.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *b64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
extern const esp_efuse_range_addr_t range_read_addr_blocks[];
extern const esp_efuse_range_addr_t range_write_addr_blocks[];

extern void esp_efuse_lock_acquire(void);
extern void esp_efuse_lock_release(void);

ESP_LOG_ATTR_TAG(TAG, "efuse");

// -------------------------- EFSR/W/RW token dump ----------------------------

/*
 * Context structure for output function
 */
typedef struct {
    uint32_t crc;       /**< CRC32 of dumped data */
    char *buf;          /**< Buffer for dumped data. If NULL, output to log */
    unsigned buf_idx;   /**< Current index in buf */
    size_t buf_len;     /**< Maximum length of buf */
} out_dump_ctx_t;

static esp_err_t output(const char *s, out_dump_ctx_t *out_ctx)
{
    if (s == NULL || out_ctx == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    unsigned slen = strlen(s);
    if (out_ctx->buf) {
        unsigned len = s[0] == '\0' ? 1 : slen;
        if (out_ctx->buf_idx + len < out_ctx->buf_len) {
            memcpy(&out_ctx->buf[out_ctx->buf_idx], s, len);
            out_ctx->buf_idx += len;
        } else {
            return ESP_ERR_INVALID_SIZE;
        }
    } else {
        esp_log_config_t config = {
            .opts = {
                .log_level = ESP_LOG_INFO,
                .constrained_env = 0,
                .require_formatting = 0,
                .dis_color = 1,
                .dis_timestamp = 1,
                .binary_mode = 0,
            },
        };
        esp_log(config, NULL, "%s", (s[0] == '\0') ? "\n" : s);
    }
    out_ctx->crc = esp_rom_crc32_le(out_ctx->crc, (const uint8_t *)s, slen);
    return ESP_OK;
}

// --------------------- Streaming Base64URL encoder (no '=') ----------------

/*
 * Base64URL encoding stream structure
 */
typedef struct {
    uint8_t rem[3];  // Remaining bytes
    int rlen;        // Number of remaining bytes in rem[]
} b64_stream_t;

static esp_err_t b64_output(uint32_t data, out_dump_ctx_t *out_ctx)
{
    // data contains 24 bits (3 bytes)
    char buffer[5] = {
        b64_table[(data >> 18) & 63],
        b64_table[(data >> 12) & 63],
        b64_table[(data >>  6) & 63],
        b64_table[ data        & 63],
        0,
    };
    return output(buffer, out_ctx);
}

static esp_err_t b64_convert(b64_stream_t *s, const uint32_t *data, size_t num_words, out_dump_ctx_t *out_ctx)
{
    esp_err_t err = ESP_OK;
    uint8_t *p = (uint8_t *)data;
    size_t n = num_words * 4; // bytes

    // Complete leftover first
    if (s->rlen) {
        while (s->rlen < 3 && n) {
            s->rem[s->rlen++] = *p++;
            --n;
        }
        if (s->rlen == 3) {
            uint32_t v = ((uint32_t)s->rem[0] << 16) | ((uint32_t)s->rem[1] << 8) | s->rem[2];
            ESP_EFUSE_CHK(b64_output(v, out_ctx));
            s->rlen = 0;
        }
    }

    // Fast path: 3-byte groups
    while (n >= 3) {
        uint32_t v = ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
        ESP_EFUSE_CHK(b64_output(v, out_ctx));
        p += 3;
        n -= 3;
    }

    // Remainder
    while (n--) {
        s->rem[s->rlen++] = *p++;
    }
err_exit:
    return err;
}

static esp_err_t b64_flush(b64_stream_t *s, out_dump_ctx_t *out_ctx)
{
    char buffer[5] = { 0 };
    if (s->rlen == 0) {
        return ESP_OK;
    } else if (s->rlen == 1) {
        uint32_t data = ((uint32_t)s->rem[0] << 16);
        buffer[0] = b64_table[(data >> 18) & 63];
        buffer[1] = b64_table[(data >> 12) & 63];
        buffer[2] = 0;
    } else if (s->rlen == 2) {
        uint32_t data = ((uint32_t)s->rem[0] << 16) | ((uint32_t)s->rem[1] << 8);
        buffer[0] = b64_table[(data >> 18) & 63];
        buffer[1] = b64_table[(data >> 12) & 63];
        buffer[2] = b64_table[(data >>  6) & 63];
        buffer[3] = 0;
    }
    esp_err_t err = output(buffer, out_ctx);
    s->rlen = 0;
    return err;
}

// Reading efuse register.
static uint32_t read_of_write_reg(esp_efuse_block_t blk, unsigned int num_reg)
{
    assert(blk >= 0 && blk < EFUSE_BLK_MAX);
    assert(num_reg <= (range_write_addr_blocks[blk].end - range_write_addr_blocks[blk].start) / sizeof(uint32_t));
    return REG_READ(range_write_addr_blocks[blk].start + num_reg * 4);
}

static uint32_t get_data(esp_efuse_block_t blk, unsigned int num_reg, esp_efuse_token_type_t dump_type)
{
    uint32_t data = 0;
    if (dump_type & ESP_EFUSE_TOKEN_FROM_READ) {
        data = esp_efuse_utility_read_reg(blk, num_reg);
    }
    if (dump_type & ESP_EFUSE_TOKEN_FROM_STAGED) {
        data |= read_of_write_reg(blk, num_reg);
    }
    return data;
}

static bool is_block_empty(esp_efuse_block_t blk, esp_efuse_token_type_t dump_type)
{
    bool ret = true;
    int num_reg = 0;
    for (uintptr_t addr_rd_block = range_read_addr_blocks[blk].start; addr_rd_block <= range_read_addr_blocks[blk].end; addr_rd_block += 4, ++num_reg) {
        if (get_data(blk, num_reg, dump_type) != 0) {
            ret = false;
            break;
        }
    }
    return ret;
}

// -------------------------- Dump helpers ------------------------------

static esp_err_t output_chip_version(out_dump_ctx_t *out_ctx)
{
    char str_chip_version[5];
    /* Encode chip version as a decimal string (major * 100 + minor).
     * The major and minor versions are stored in eFuse fields (typically 2-3 bits for major version).
     * Maximum major version is 7 (3 bits), allowing versions like "702" (7.02).
     * Buffer size of 5 chars safely accommodates up to 4 digits plus null terminator.
     * If the major version field is extended in the future, this function will still work
     * correctly since esp_log_util_cvt_dec() handles arbitrary values and the buffer has
     * sufficient capacity.
     */
    esp_log_util_cvt_dec(efuse_hal_chip_revision(), 3, str_chip_version);
    return output(str_chip_version, out_ctx);
}

static esp_err_t output_efuse_blocks(esp_efuse_token_type_t dump_type, out_dump_ctx_t *out_ctx)
{
    esp_err_t err = ESP_OK;
    b64_stream_t enc = { 0 };

    for (esp_efuse_block_t blk = EFUSE_BLK0; blk < EFUSE_BLK_MAX; blk++) {
        if (!is_block_empty(blk, dump_type)) {
            int num_reg = 0;
            for (uintptr_t addr_rd_block = range_read_addr_blocks[blk].start; addr_rd_block <= range_read_addr_blocks[blk].end; addr_rd_block += 4, ++num_reg) {
                uint32_t data = get_data(blk, num_reg, dump_type);

                ESP_EFUSE_CHK(b64_convert(&enc, &data, 1, out_ctx));
            }
            ESP_EFUSE_CHK(b64_flush(&enc, out_ctx));
        }
        if (blk != EFUSE_BLK_MAX - 1) {
            ESP_EFUSE_CHK(output(":", out_ctx));
        }
    }
err_exit:
    return err;
}

static esp_err_t output_coding_error_data(out_dump_ctx_t *out_ctx)
{
    esp_err_t err = ESP_OK;
    uint32_t data[] = {
#if CONFIG_IDF_TARGET_ESP32
        efuse_ll_get_coding_error(0)
#elif CONFIG_IDF_TARGET_ESP32C2
        efuse_ll_get_coding_error(0),
        efuse_ll_get_coding_error(1)
#elif CONFIG_IDF_TARGET_ESP32S31
        efuse_ll_get_coding_error(0),
        efuse_ll_get_coding_error(1),
        efuse_ll_get_coding_error(2),
        efuse_ll_get_coding_error(3),
        efuse_ll_get_coding_error(4),
        efuse_ll_get_coding_error(5),
        efuse_ll_get_coding_error(6),
        efuse_ll_get_coding_error(7),
        efuse_ll_get_coding_error(8),
        efuse_ll_get_coding_error(9)
#else
        efuse_ll_get_coding_error(0),
        efuse_ll_get_coding_error(1),
        efuse_ll_get_coding_error(2),
        efuse_ll_get_coding_error(3),
        efuse_ll_get_coding_error(4),
        efuse_ll_get_coding_error(5),
        efuse_ll_get_coding_error(6)
#endif
    };

    bool all_zero = true;
    for (int i = 0; i < sizeof(data) / sizeof(data[0]); ++i) {
        if (data[i] != 0) {
            all_zero = false;
            break;
        }
    }
    if (!all_zero) {
        b64_stream_t enc = { 0 };
        ESP_EFUSE_CHK(b64_convert(&enc, data, sizeof(data) / sizeof(data[0]), out_ctx));
        ESP_EFUSE_CHK(b64_flush(&enc, out_ctx));
    }
err_exit:
    return err;
}

static esp_err_t output_crc32(out_dump_ctx_t *out_ctx)
{
    esp_err_t err = ESP_OK;
    b64_stream_t enc = { 0 };
    uint32_t crc = out_ctx->crc;
    ESP_EFUSE_CHK(b64_convert(&enc, &crc, 1, out_ctx));
    ESP_EFUSE_CHK(b64_flush(&enc, out_ctx));
err_exit:
    return err;
}

static esp_err_t split_token(const char *token, const char **f4_blocks, const char **f6_crc)
{
    // 6 fields: magic_str, chip, ver, blocks, cerr, crc
    const char *fields[] = { token, NULL, NULL, NULL, NULL, NULL };
    int field_counter = 1;
    unsigned number_of_blocks = 0;
    const unsigned block_start_position = 4;
    bool skip_blocks = false;
    for (; *token; ++token) {
        if (*token == ':') {
            if (field_counter >= sizeof(fields) / sizeof(fields[0])) {
                return ESP_ERR_INVALID_ARG;
            }
            if (skip_blocks) {
                if (number_of_blocks++ < EFUSE_BLK_MAX - 1) {
                    continue;
                }
            }
            fields[field_counter++] = token + 1;
            if (field_counter == block_start_position) {
                skip_blocks = true;
            }
        }
        if (*token == '\0') {
            break;
        }
    }
    if (field_counter != sizeof(fields) / sizeof(fields[0])) {
        return ESP_ERR_INVALID_ARG;
    }

    // *f1_magic = fields[0];
    // *f2_chip  = fields[1];
    // *f3_ver   = fields[2];
    *f4_blocks = fields[3];
    // *f5_cerr  = fields[4];
    *f6_crc   = fields[5];
    return ESP_OK;
}

/* Base64URL is UNPADDED (no '=');
 * It maps a value char to its 6-bit value
 */
static unsigned char b64url_val(char c)
{
    if (c >= 'A' && c <= 'Z') return (unsigned char)(c - 'A');          // 0..25
    if (c >= 'a' && c <= 'z') return (unsigned char)(c - 'a' + 26);     // 26..51
    if (c >= '0' && c <= '9') return (unsigned char)(c - '0' + 52);     // 52..61
    if (c == '-')             return 62;                                // 62
    if (c == '_')             return 63;                                // 63
    return 0xFF;
}

// Decode one 24-bit chunk from 4 Base64URL chars.
static uint32_t b64_quartet24(const char *p)
{
    if (p == NULL) {
        return 0;
    }
    uint32_t v0 = b64url_val(p[0]);
    uint32_t v1 = b64url_val(p[1]);
    uint32_t v2 = b64url_val(p[2]);
    uint32_t v3 = b64url_val(p[3]);
    return (v0 << 18) | (v1 << 12) | (v2 << 6) | v3;  // 24 useful bits
}

/*
 * Return the little-endian 32-bit word at idx position
 * Each 4 b64 chars -> 3 bytes. Find the quartet enclosing byte idx.
 */
static uint32_t b64_decode_u32(const char *in, unsigned idx)
{
    const size_t q = (idx / 3) * 4;      // starting quartet (char index)

    // Decode two consecutive quartets (always sufficient to cover 4 bytes)
    const uint32_t q0 = b64_quartet24(&in[q]);
    const uint32_t b0 = ((q0 >> 16) & 0xFF);
    const uint32_t b1 = ((q0 >>  8) & 0xFF);
    const uint32_t b2 = ( q0        & 0xFF);

    const uint32_t q1 = b64_quartet24(&in[q + 4]);
    const uint32_t c0 = ((q1 >> 16) & 0xFF);
    const uint32_t c1 = ((q1 >>  8) & 0xFF);
    const uint32_t c2 = ( q1        & 0xFF);

    uint32_t out;
    const int r = idx % 3;
    if (r == 0) {
        // bytes: b0 b1 b2 c0
        out = b0 | (b1 << 8) | (b2 << 16) | (c0 << 24);
    } else if (r == 1) {
        // bytes: b1 b2 c0 c1
        out = b1 | (b2 << 8) | (c0 << 16) | (c1 << 24);
    } else { // r == 2
        // bytes: b2 c0 c1 c2
        out = b2 | (c0 << 8) | (c1 << 16) | (c2 << 24);
    }
    return out;
}

static bool efuse_block_can_be_written(esp_efuse_block_t blk, esp_efuse_coding_scheme_t coding_scheme)
{
    if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
        bool blk_can_be_written = true;
#if CONFIG_IDF_TARGET_ESP32
        if (blk == EFUSE_BLK_KEY0 || blk == EFUSE_BLK_KEY1) {
            blk_can_be_written = esp_efuse_key_block_unused(blk);
        } else if (blk == EFUSE_BLK3) {
            blk_can_be_written = !esp_efuse_get_key_dis_write(blk);
        }
#endif // CONFIG_IDF_TARGET_ESP32
        return blk_can_be_written;
    } else {
        return (blk >= EFUSE_BLK_KEY0 && blk < EFUSE_BLK_KEY_MAX)
                ? esp_efuse_key_block_unused(blk)
                : is_block_empty(blk, ESP_EFUSE_TOKEN_FROM_READ);
    }
}

static esp_err_t validate_chip_version(const char *token_in, bool ignore_ver, unsigned int *out_len)
{
    // Parse version from token (up to 4 digits)
    char token_version[5] = { 0 };
    unsigned int len = 0;
    while (len < 4 && token_in[len] != ':' && token_in[len] != '\0') {
        token_version[len] = token_in[len];
        len++;
    }

    if (len < 3) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!ignore_ver) {
        // Compare major versions (divide by 100)
        unsigned int token_major = atoi(token_version) / 100;
        if (token_major != efuse_hal_get_major_chip_version()) {  // TODO: use esp_chip_revision()
            return ESP_ERR_INVALID_VERSION;
        }
    }

    *out_len = len;
    return ESP_OK;
}

static esp_err_t validate_token(const char *token_in, bool ignore_ver, const char **b64_blocks)
{
    if (!token_in || !b64_blocks) {
        return ESP_ERR_INVALID_ARG;
    }

    const char *b64_crc;
    if (split_token(token_in, b64_blocks, &b64_crc) != ESP_OK) {
        return ESP_ERR_INVALID_ARG;
    }

    unsigned pos = 0;
    unsigned len = sizeof("EFSW") - 1;
    if (strncmp(&token_in[pos], "EFSW", len) != 0) { // magic check
        return ESP_ERR_INVALID_ARG;
    }
    pos += len + 1; // skip ':'

    len = sizeof(CONFIG_IDF_TARGET) - 1;
    if (strncmp(&token_in[pos], CONFIG_IDF_TARGET, len) != 0 || token_in[pos + len] != ':') {
        return ESP_ERR_INVALID_ARG;
    }
    pos += len + 1; // skip ':'

    esp_err_t err = validate_chip_version(&token_in[pos], ignore_ver, &len);
    if (err != ESP_OK) {
        return err;
    }
    pos += len + 1; // skip ':'

    const int token_len = strlen(token_in) - 6; // len of token without crc part
    if (esp_rom_crc32_le(0, (const uint8_t *)token_in, token_len) != b64_decode_u32(b64_crc, 0)) {
        return ESP_ERR_INVALID_CRC;
    }
    return ESP_OK;
}

// ESSR:chip_name:chip_version:b64_bock0:b64_bock1:...:b64_bock10:b64_errors:b64_crc32
esp_err_t esp_efuse_token_dump(esp_efuse_token_type_t dump_type, char *buf, size_t buf_len)
{
    esp_err_t err = ESP_OK;
    bool log_output = (buf == NULL);

    out_dump_ctx_t out_ctx = {
        .crc = 0,
        .buf = buf,
        .buf_len = buf_len,
    };

    // 1. token name
    const char *efs_marker[3] = {
        "EFSR",
        "EFSW",
        "EFSRW",
    };
    if (dump_type == 0 || dump_type > (ESP_EFUSE_TOKEN_FROM_READ | ESP_EFUSE_TOKEN_FROM_STAGED)) {
        return ESP_ERR_INVALID_ARG;
    }
#if !CONFIG_EFUSE_ENABLE_STAGED_TOKEN_API
    if (dump_type & ESP_EFUSE_TOKEN_FROM_STAGED) {
        ESP_LOGW(TAG, "Staged token support is disabled because it can expose "
                  "keys in plaintext before burn/read-protect. Enable "
                  "CONFIG_EFUSE_ENABLE_STAGED_TOKEN_API to use staged "
                  "eFuse token dump modes.");
        return ESP_ERR_NOT_SUPPORTED;
    }
#endif
    if (log_output) {
        esp_log_impl_lock();
    }
    ESP_EFUSE_CHK(output(efs_marker[dump_type - 1], &out_ctx));

    // 2. chip_name
    ESP_EFUSE_CHK(output(":" CONFIG_IDF_TARGET ":", &out_ctx));

    // 3. chip_version
    ESP_EFUSE_CHK(output_chip_version(&out_ctx));

    // 4. b64_efuse_blocks
    ESP_EFUSE_CHK(output(":", &out_ctx));
    ESP_EFUSE_CHK(output_efuse_blocks(dump_type, &out_ctx));

    // 5. b64_coding_error_data
    ESP_EFUSE_CHK(output(":", &out_ctx));
    ESP_EFUSE_CHK(output_coding_error_data(&out_ctx));

    // 6. b64_crc32
    ESP_EFUSE_CHK(output(":", &out_ctx));
    ESP_EFUSE_CHK(output_crc32(&out_ctx));

    ESP_EFUSE_CHK(output("\0", &out_ctx));

err_exit:
    if (log_output) {
        esp_log_impl_unlock();
    }
    return err;
}

esp_err_t esp_efuse_token_burn(const char *token_in, bool ignore_ver)
{
    const char *b64_blocks;
    esp_err_t err = validate_token(token_in, ignore_ver, &b64_blocks);
    if (err) {
        return err;
    }

    esp_efuse_lock_acquire();
    if (s_batch_writing_mode == 0) {
        esp_efuse_utility_reset();
    }

    int idx = 0;
    for (esp_efuse_block_t blk = EFUSE_BLK0; blk < EFUSE_BLK_MAX; blk++) {
        if (b64_blocks[idx] == ':') {
            idx++;
            if (b64_blocks[idx] == ':') {
                continue; // dump for current block is empty, skip it.
            }
        }
        esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(blk);
        bool blk_can_be_written = efuse_block_can_be_written(blk, coding_scheme);
        int num_reg = 0;
        for (uintptr_t a = range_write_addr_blocks[blk].start; a <= range_write_addr_blocks[blk].end; a += 4, ++num_reg) {
            uint32_t reg_to_write = b64_decode_u32(&b64_blocks[idx], num_reg * 4);
            if (reg_to_write == 0) {
                continue;
            }
            if (blk_can_be_written) {
                if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
                    reg_to_write &= ~esp_efuse_utility_read_reg(blk, num_reg); // remove already set bits
                }
                err = esp_efuse_utility_write_reg(blk, num_reg, reg_to_write);
            } else {
                ESP_LOGE(TAG, "eFuse BLOCK%d is not empty. Skip updating it from token dump.", blk);
                err = ESP_FAIL;
            }
            if (err != ESP_OK) {
                break;
            }
        }
        idx += (16 * num_reg + 2) / 3;
        if (err != ESP_OK) {
            break;
        }
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
    esp_efuse_lock_release();
    return err;
}
