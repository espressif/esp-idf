/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "esp32/clk.h"
#include "esp_log.h"
#include "assert.h"
#include "sdkconfig.h"
#include <sys/param.h>

static const char *TAG = "efuse";

#ifdef CONFIG_EFUSE_VIRTUAL
extern uint32_t virt_blocks[EFUSE_BLK_MAX][COUNT_EFUSE_REG_PER_BLOCK];
#endif // CONFIG_EFUSE_VIRTUAL

/*Range addresses to read blocks*/
const esp_efuse_range_addr_t range_read_addr_blocks[] = {
    {EFUSE_BLK0_RDATA0_REG, EFUSE_BLK0_RDATA6_REG},    // range address of EFUSE_BLK0
    {EFUSE_BLK1_RDATA0_REG, EFUSE_BLK1_RDATA7_REG},    // range address of EFUSE_BLK1
    {EFUSE_BLK2_RDATA0_REG, EFUSE_BLK2_RDATA7_REG},    // range address of EFUSE_BLK2
    {EFUSE_BLK3_RDATA0_REG, EFUSE_BLK3_RDATA7_REG}     // range address of EFUSE_BLK3
};

static uint32_t write_mass_blocks[EFUSE_BLK_MAX][COUNT_EFUSE_REG_PER_BLOCK] = { 0 };

/*Range addresses to write blocks (it is not real regs, it is a buffer) */
const esp_efuse_range_addr_t range_write_addr_blocks[] = {
    {(uint32_t) &write_mass_blocks[EFUSE_BLK0][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK0][6]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK1][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK1][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK2][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK2][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK3][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK3][7]},
};

#define EFUSE_CONF_WRITE          0x5A5A /* eFuse_pgm_op_ena, force no rd/wr disable. */
#define EFUSE_CONF_READ           0x5AA5 /* eFuse_read_op_ena, release force. */
#define EFUSE_CMD_PGM             0x02   /* Command to program. */
#define EFUSE_CMD_READ            0x01   /* Command to read. */

#ifndef CONFIG_EFUSE_VIRTUAL
/* Addresses to write blocks*/
const uint32_t start_write_addr[] = {
    EFUSE_BLK0_WDATA0_REG,
    EFUSE_BLK1_WDATA0_REG,
    EFUSE_BLK2_WDATA0_REG,
    EFUSE_BLK3_WDATA0_REG,
};

static void apply_repeat_encoding(const uint8_t *in_bytes, uint32_t *out_words, size_t in_bytes_len);

// Update Efuse timing configuration
static esp_err_t esp_efuse_set_timing(void)
{
    uint32_t apb_freq_mhz = esp_clk_apb_freq() / 1000000;
    uint32_t clk_sel0, clk_sel1, dac_clk_div;
    if (apb_freq_mhz <= 26) {
        clk_sel0 = 250;
        clk_sel1 = 255;
        dac_clk_div = 52;
    } else if (apb_freq_mhz <= 40) {
        clk_sel0 = 160;
        clk_sel1 = 255;
        dac_clk_div = 80;
    } else {
        clk_sel0 = 80;
        clk_sel1 = 128;
        dac_clk_div = 100;
    }
    REG_SET_FIELD(EFUSE_DAC_CONF_REG, EFUSE_DAC_CLK_DIV, dac_clk_div);
    REG_SET_FIELD(EFUSE_CLK_REG, EFUSE_CLK_SEL0, clk_sel0);
    REG_SET_FIELD(EFUSE_CLK_REG, EFUSE_CLK_SEL1, clk_sel1);
    return ESP_OK;
}

__attribute__((always_inline)) static inline bool efuse_ll_get_dec_warnings(unsigned block)
{
    if (block == 0 || block > 4) {
        return false;
    }
    uint32_t error_reg = REG_GET_FIELD(EFUSE_DEC_STATUS_REG, EFUSE_DEC_WARNINGS);
    if (((error_reg >> (4 * (block - 1))) & 0x0F) != 0) {
        return true;
    }
    return false;
}

static bool efuse_hal_is_coding_error_in_block(unsigned block)
{
    if (block > 0) {
        if (esp_efuse_get_coding_scheme(block) == EFUSE_CODING_SCHEME_3_4) {
            if (efuse_ll_get_dec_warnings(block)) {
                return true;
            }
        }
    }
    return false;
}

#endif // ifndef CONFIG_EFUSE_VIRTUAL

// Efuse read operation: copies data from physical efuses to efuse read registers.
void esp_efuse_utility_clear_program_registers(void)
{
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
}

// Burn values written to the efuse write registers
void esp_efuse_utility_burn_chip(void)
{
#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGW(TAG, "Virtual efuses enabled: Not really burning eFuses");
    for (int num_block = EFUSE_BLK_MAX - 1; num_block >= EFUSE_BLK0; num_block--) {
        int subblock = 0;
        for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            virt_blocks[num_block][subblock++] |= REG_READ(addr_wr_block);
        }
    }
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    esp_efuse_utility_write_efuses_to_flash();
#endif
#else
    if (esp_efuse_set_timing() != ESP_OK) {
        ESP_LOGE(TAG, "Efuse fields are not burnt");
    } else {
        // Permanently update values written to the efuse write registers
        // It is necessary to process blocks in the order from MAX-> EFUSE_BLK0, because EFUSE_BLK0 has protection bits for other blocks.
        for (int num_block = EFUSE_BLK_MAX - 1; num_block >= EFUSE_BLK0; num_block--) {
            esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(num_block);
            for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
                if (REG_READ(addr_wr_block) != 0) {
                    unsigned w_data_len;
                    unsigned r_data_len;
                    if (scheme == EFUSE_CODING_SCHEME_3_4) {
                        esp_efuse_utility_apply_34_encoding((void *)range_write_addr_blocks[num_block].start, (uint32_t *)start_write_addr[num_block], 24);
                        r_data_len = 24;
                        w_data_len = 32;
                    } else if (scheme == EFUSE_CODING_SCHEME_REPEAT) {
                        apply_repeat_encoding((void *)range_write_addr_blocks[num_block].start, (uint32_t *)start_write_addr[num_block], 16);
                        r_data_len = 16;
                        w_data_len = 32;
                    } else {
                        r_data_len = (range_read_addr_blocks[num_block].end - range_read_addr_blocks[num_block].start) + sizeof(uint32_t);
                        w_data_len = (range_write_addr_blocks[num_block].end - range_write_addr_blocks[num_block].start) + sizeof(uint32_t);
                        memcpy((void *)start_write_addr[num_block], (void *)range_write_addr_blocks[num_block].start, w_data_len);
                    }

                    uint32_t backup_write_data[8];
                    memcpy(backup_write_data, (void *)start_write_addr[num_block], w_data_len);
                    int repeat_burn_op = 1;
                    bool correct_written_data;
                    bool coding_error_before = efuse_hal_is_coding_error_in_block(num_block);
                    bool coding_error_occurred;

                    do {
                        ESP_LOGI(TAG, "BURN BLOCK%d", num_block);
                        // BURN a block
                        REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_WRITE);
                        REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_PGM);
                        while (REG_READ(EFUSE_CMD_REG) != 0) {};
                        REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
                        REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_READ);
                        while (REG_READ(EFUSE_CMD_REG) != 0) {};

                        bool coding_error_after = efuse_hal_is_coding_error_in_block(num_block);
                        coding_error_occurred = (coding_error_before != coding_error_after) && coding_error_before == false;
                        if (coding_error_occurred) {
                            ESP_LOGE(TAG, "BLOCK%d has an error", num_block);
                        }

                        correct_written_data = esp_efuse_utility_is_correct_written_data(num_block, r_data_len);
                        if (!correct_written_data || coding_error_occurred) {
                            ESP_LOGW(TAG, "BLOCK%d: next retry [%d/3]...", num_block, repeat_burn_op);
                            memcpy((void *)start_write_addr[num_block], (void *)backup_write_data, w_data_len);
                        }

                    } while ((!correct_written_data || coding_error_occurred) && repeat_burn_op++ < 3);
                    if (!correct_written_data) {
                        ESP_LOGE(TAG, "Written data are incorrect");
                    }
                    if (coding_error_occurred) {
                        ESP_LOGE(TAG, "Coding error occurred in block");
                    }
                    break;
                }
            }
        }
    }
#endif // CONFIG_EFUSE_VIRTUAL
    esp_efuse_utility_reset();
}

esp_err_t esp_efuse_utility_apply_34_encoding(const uint8_t *in_bytes, uint32_t *out_words, size_t in_bytes_len)
{
    if (in_bytes == NULL || out_words == NULL || in_bytes_len % 6 != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    while (in_bytes_len > 0) {
        uint8_t out[8];
        uint8_t xor = 0;
        uint8_t mul = 0;
        for (int i = 0; i < 6; i++) {
            xor ^= in_bytes[i];
            mul += (i + 1) * __builtin_popcount(in_bytes[i]);
        }

        memcpy(out, in_bytes, 6); // Data bytes
        out[6] = xor;
        out[7] = mul;

        memcpy(out_words, out, 8);

        in_bytes_len -= 6;
        in_bytes += 6;
        out_words += 2;
    }

    return ESP_OK;
}

#ifndef CONFIG_EFUSE_VIRTUAL

static void apply_repeat_encoding(const uint8_t *in_bytes, uint32_t *out_words, size_t in_bytes_len)
{
    for (unsigned i = 0; i < 2; i++) {
        memcpy(&out_words[i * 4], (uint32_t *)in_bytes, in_bytes_len);
    }
}
#endif // CONFIG_EFUSE_VIRTUAL

// This function just checkes that given data for blocks will not rise a coding issue during the burn operation.
// Encoded data will be set during the burn operation.
esp_err_t esp_efuse_utility_apply_new_coding_scheme()
{
    // start with EFUSE_BLK1. EFUSE_BLK0 - always uses EFUSE_CODING_SCHEME_NONE.
    for (int num_block = EFUSE_BLK1; num_block < EFUSE_BLK_MAX; num_block++) {
        esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(num_block);
        if (scheme != EFUSE_CODING_SCHEME_NONE) {
            bool is_write_data = false;
            for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
                if (REG_READ(addr_wr_block)) {
                    is_write_data = true;
                }
            }
            if (is_write_data) {
                uint8_t* buf_w_data = (uint8_t*)range_write_addr_blocks[num_block].start;
                uint8_t* buf_r_data = (uint8_t*)range_read_addr_blocks[num_block].start;
                if (scheme == EFUSE_CODING_SCHEME_3_4) {
                    if (*((uint32_t*)buf_w_data + 6) != 0 || *((uint32_t*)buf_w_data + 7) != 0) {
                        return ESP_ERR_CODING;
                    }
                    for (int i = 0; i < 24; ++i) {
                        if (buf_w_data[i] != 0) {
                            int st_offset_buf = (i / 6) * 6;
                            // check that place is free.
                            for (int n = st_offset_buf; n < st_offset_buf + 6; ++n) {
                                if (buf_r_data[n] != 0) {
                                    ESP_LOGE(TAG, "Bits are not empty. Write operation is forbidden.");
                                    return ESP_ERR_CODING;
                                }
                            }
                        }
                    }
                } else if (scheme == EFUSE_CODING_SCHEME_REPEAT) {
                    for (int i = 4; i < 8; ++i) {
                        if (*((uint32_t*)buf_w_data + i) != 0) {
                            return ESP_ERR_CODING;
                        }
                    }
                }
            }
        }
    }
    return ESP_OK;
}
