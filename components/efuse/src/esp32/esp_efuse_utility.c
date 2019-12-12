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

#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "esp32/clk.h"
#include "esp_log.h"
#include "assert.h"
#include "sdkconfig.h"
#include <sys/param.h>

static const char *TAG = "efuse";

#ifdef CONFIG_EFUSE_VIRTUAL
extern uint32_t virt_blocks[COUNT_EFUSE_BLOCKS][COUNT_EFUSE_REG_PER_BLOCK];
#endif // CONFIG_EFUSE_VIRTUAL

/*Range addresses to read blocks*/
const esp_efuse_range_addr_t range_read_addr_blocks[] = {
    {EFUSE_BLK0_RDATA0_REG, EFUSE_BLK0_RDATA6_REG},    // range address of EFUSE_BLK0
    {EFUSE_BLK1_RDATA0_REG, EFUSE_BLK1_RDATA7_REG},    // range address of EFUSE_BLK1
    {EFUSE_BLK2_RDATA0_REG, EFUSE_BLK2_RDATA7_REG},    // range address of EFUSE_BLK2
    {EFUSE_BLK3_RDATA0_REG, EFUSE_BLK3_RDATA7_REG}     // range address of EFUSE_BLK3
};

/*Range addresses to write blocks*/
const esp_efuse_range_addr_t range_write_addr_blocks[] = {
    {EFUSE_BLK0_WDATA0_REG, EFUSE_BLK0_WDATA6_REG},    // range address of EFUSE_BLK0
    {EFUSE_BLK1_WDATA0_REG, EFUSE_BLK1_WDATA7_REG},    // range address of EFUSE_BLK1
    {EFUSE_BLK2_WDATA0_REG, EFUSE_BLK2_WDATA7_REG},    // range address of EFUSE_BLK2
    {EFUSE_BLK3_WDATA0_REG, EFUSE_BLK3_WDATA7_REG}     // range address of EFUSE_BLK3
};

#define EFUSE_CONF_WRITE          0x5A5A /* eFuse_pgm_op_ena, force no rd/wr disable. */
#define EFUSE_CONF_READ           0x5AA5 /* eFuse_read_op_ena, release force. */
#define EFUSE_CMD_PGM             0x02   /* Command to program. */
#define EFUSE_CMD_READ            0x01   /* Command to read. */

#ifndef CONFIG_EFUSE_VIRTUAL
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
#endif // ifndef CONFIG_EFUSE_VIRTUAL

// Efuse read operation: copies data from physical efuses to efuse read registers.
void esp_efuse_utility_clear_program_registers(void)
{
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
}

// Burn values written to the efuse write registers
void esp_efuse_utility_burn_efuses(void)
{
#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGW(TAG, "Virtual efuses enabled: Not really burning eFuses");
    for (int num_block = 0; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
        esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(num_block);
        if (scheme == EFUSE_CODING_SCHEME_3_4) {
            uint8_t buf[COUNT_EFUSE_REG_PER_BLOCK * 4] = { 0 };
            int i = 0;
            for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4, ++i) {
                *((uint32_t*)buf + i) = REG_READ(addr_wr_block);
            }
            int j = 0;
            uint32_t out_buf[COUNT_EFUSE_REG_PER_BLOCK] = { 0 };
            for (int k = 0; k < 4; ++k, ++j) {
                memcpy((uint8_t*)out_buf + j * 6, &buf[k * 8], 6);
            }
            for (int k = 0; k < COUNT_EFUSE_REG_PER_BLOCK; ++k) {
                REG_WRITE(range_write_addr_blocks[num_block].start + k * 4,  out_buf[k]);
            }
        }
        int subblock = 0;
        for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            virt_blocks[num_block][subblock++] |= REG_READ(addr_wr_block);
        }
    }
#else
    esp_efuse_set_timing();
    // Permanently update values written to the efuse write registers
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_WRITE);
    REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_PGM);
    while (REG_READ(EFUSE_CMD_REG) != 0) {};
    REG_WRITE(EFUSE_CONF_REG, EFUSE_CONF_READ);
    REG_WRITE(EFUSE_CMD_REG,  EFUSE_CMD_READ);
    while (REG_READ(EFUSE_CMD_REG) != 0) {};
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

static bool read_w_data_and_check_fill(esp_efuse_block_t num_block, uint32_t *buf_w_data)
{
    bool blk_is_filled = false;
    int i = 0;
    for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4, ++i) {
        buf_w_data[i] = REG_READ(addr_wr_block);
        if (buf_w_data[i] != 0) {
            REG_WRITE(addr_wr_block, 0);
            blk_is_filled = true;
        }
    }
    return blk_is_filled;
}

static void read_r_data(esp_efuse_block_t num_block, uint32_t* buf_r_data)
{
    int i = 0;
    for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4, ++i) {
        buf_r_data[i] = esp_efuse_utility_read_reg(num_block, i);
    }
}

// After esp_efuse_write.. functions EFUSE_BLKx_WDATAx_REG were filled is not coded values.
// This function reads EFUSE_BLKx_WDATAx_REG registers, applies coding scheme and writes encoded values back to EFUSE_BLKx_WDATAx_REG.
esp_err_t esp_efuse_utility_apply_new_coding_scheme()
{
    uint8_t buf_w_data[COUNT_EFUSE_REG_PER_BLOCK * 4];
    uint8_t buf_r_data[COUNT_EFUSE_REG_PER_BLOCK * 4];
    uint32_t reg[COUNT_EFUSE_REG_PER_BLOCK];
    // start with EFUSE_BLK1. EFUSE_BLK0 - always uses EFUSE_CODING_SCHEME_NONE.
    for (int num_block = 1; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
        esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(num_block);
        // check and apply a new coding scheme.
        if (scheme != EFUSE_CODING_SCHEME_NONE) {
            memset(buf_w_data, 0, sizeof(buf_w_data));
            memset((uint8_t*)reg, 0, sizeof(reg));
            if (read_w_data_and_check_fill(num_block, (uint32_t*)buf_w_data) == true) {
                read_r_data(num_block, (uint32_t*)buf_r_data);
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

                            esp_err_t err = esp_efuse_utility_apply_34_encoding(&buf_w_data[st_offset_buf], reg, 6);
                            if (err != ESP_OK) {
                                return err;
                            }

                            int num_reg = (st_offset_buf / 6) * 2;
                            for (int r = 0; r < 2; r++) {
                                REG_WRITE(range_write_addr_blocks[num_block].start + (num_reg + r) * 4, reg[r]);
                            }
                            i = st_offset_buf + 5;
                        }
                    }
                } else if (scheme == EFUSE_CODING_SCHEME_REPEAT) {
                    uint32_t* buf_32 = (uint32_t*)buf_w_data;
                    for (int i = 4; i < 8; ++i) {
                        if (*(buf_32 + i) != 0) {
                            return ESP_ERR_CODING;
                        }
                    }
                    for (int i = 0; i < 4; ++i) {
                        if (buf_32[i] != 0) {
                            REG_WRITE(range_write_addr_blocks[num_block].start + i * 4, buf_32[i]);
                            REG_WRITE(range_write_addr_blocks[num_block].start + (i + 4) * 4, buf_32[i]);
                        }
                    }
                }
            }
        }
    }
    return ESP_OK;
}
