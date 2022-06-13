/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "hal/efuse_hal.h"
#include "esp_private/esp_clk.h"
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
    efuse_hal_set_timing(apb_freq_mhz);
    return ESP_OK;
}
#endif // ifndef CONFIG_EFUSE_VIRTUAL

// Efuse read operation: copies data from physical efuses to efuse read registers.
void esp_efuse_utility_clear_program_registers(void)
{
    efuse_hal_clear_program_registers();
}

esp_err_t esp_efuse_utility_check_errors(void)
{
    return ESP_OK;
}

// Burn values written to the efuse write registers
esp_err_t esp_efuse_utility_burn_chip(void)
{
    esp_err_t error = ESP_OK;
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
#else // CONFIG_EFUSE_VIRTUAL
    if (esp_efuse_set_timing() != ESP_OK) {
        ESP_LOGE(TAG, "Efuse fields are not burnt");
    } else {
        // Permanently update values written to the efuse write registers
        // It is necessary to process blocks in the order from MAX-> EFUSE_BLK0, because EFUSE_BLK0 has protection bits for other blocks.
        for (int num_block = EFUSE_BLK_MAX - 1; num_block >= EFUSE_BLK0; num_block--) {
            esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(num_block);
            bool need_burn_block = false;
            for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
                if (REG_READ(addr_wr_block) != 0) {
                    need_burn_block = true;
                    break;
                }
            }
            if (!need_burn_block) {
                continue;
            }
            if (error) {
                // It is done for a use case: BLOCK2 (Flash encryption key) could have an error (incorrect written data)
                // in this case we can not burn any data into BLOCK0 because it might set read/write protections of BLOCK2.
                ESP_LOGE(TAG, "BLOCK%d can not be burned because a previous block got an error, skipped.", num_block);
                continue;
            }
            efuse_hal_clear_program_registers();
            unsigned w_data_len;
            unsigned r_data_len;
            if (scheme == EFUSE_CODING_SCHEME_3_4) {
                esp_efuse_utility_apply_34_encoding((void *)range_write_addr_blocks[num_block].start, (uint32_t *)start_write_addr[num_block], ESP_EFUSE_LEN_OF_3_4_SCHEME_BLOCK_IN_BYTES);
                r_data_len = ESP_EFUSE_LEN_OF_3_4_SCHEME_BLOCK_IN_BYTES;
                w_data_len = 32;
            } else if (scheme == EFUSE_CODING_SCHEME_REPEAT) {
                apply_repeat_encoding((void *)range_write_addr_blocks[num_block].start, (uint32_t *)start_write_addr[num_block], 16);
                r_data_len = ESP_EFUSE_LEN_OF_REPEAT_BLOCK_IN_BYTES;
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
            if (coding_error_before) {
                ESP_LOGW(TAG, "BLOCK%d already has a coding error", num_block);
            }
            bool coding_error_occurred;

            do {
                ESP_LOGI(TAG, "BURN BLOCK%d", num_block);
                efuse_hal_program(0); // BURN a block

                bool coding_error_after = efuse_hal_is_coding_error_in_block(num_block);
                coding_error_occurred = (coding_error_before != coding_error_after) && coding_error_before == false;
                if (coding_error_occurred) {
                    ESP_LOGW(TAG, "BLOCK%d got a coding error", num_block);
                }

                correct_written_data = esp_efuse_utility_is_correct_written_data(num_block, r_data_len);
                if (!correct_written_data || coding_error_occurred) {
                    ESP_LOGW(TAG, "BLOCK%d: next retry to fix an error [%d/3]...", num_block, repeat_burn_op);
                    memcpy((void *)start_write_addr[num_block], (void *)backup_write_data, w_data_len);
                }

            } while ((!correct_written_data || coding_error_occurred) && repeat_burn_op++ < 3);

            if (coding_error_occurred) {
                ESP_LOGW(TAG, "Coding error was not fixed");
            }
            if (!correct_written_data) {
                ESP_LOGE(TAG, "Written data are incorrect");
                error = ESP_FAIL;
            }
        }
    }
#endif // CONFIG_EFUSE_VIRTUAL
    esp_efuse_utility_reset();
    return error;
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

static void read_r_data(esp_efuse_block_t num_block, uint32_t* buf_r_data)
{
    int i = 0;
    for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4, ++i) {
        buf_r_data[i] = esp_efuse_utility_read_reg(num_block, i);
    }
}

// This function just checkes that given data for blocks will not rise a coding issue during the burn operation.
// Encoded data will be set during the burn operation.
esp_err_t esp_efuse_utility_apply_new_coding_scheme()
{
    uint8_t buf_r_data[COUNT_EFUSE_REG_PER_BLOCK * 4];
    // start with EFUSE_BLK1. EFUSE_BLK0 - always uses EFUSE_CODING_SCHEME_NONE.
    for (int num_block = EFUSE_BLK1; num_block < EFUSE_BLK_MAX; num_block++) {
        esp_efuse_coding_scheme_t scheme = esp_efuse_get_coding_scheme(num_block);
        if (scheme != EFUSE_CODING_SCHEME_NONE) {
            bool is_write_data = false;
            for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
                if (REG_READ(addr_wr_block)) {
                    is_write_data = true;
                    break;
                }
            }
            if (is_write_data) {
                read_r_data(num_block, (uint32_t*)buf_r_data);
                uint8_t* buf_w_data = (uint8_t*)range_write_addr_blocks[num_block].start;
                if (scheme == EFUSE_CODING_SCHEME_3_4) {
                    if (*((uint32_t*)buf_w_data + 6) != 0 || *((uint32_t*)buf_w_data + 7) != 0) {
                        return ESP_ERR_CODING;
                    }
                    for (int i = 0; i < ESP_EFUSE_LEN_OF_3_4_SCHEME_BLOCK_IN_BYTES; ++i) {
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
