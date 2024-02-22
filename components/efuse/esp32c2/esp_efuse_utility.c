/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "assert.h"
#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "esp_private/esp_clk.h"
#include "hal/efuse_hal.h"

static const char *TAG = "efuse";

#ifdef CONFIG_EFUSE_VIRTUAL
extern uint32_t virt_blocks[EFUSE_BLK_MAX][COUNT_EFUSE_REG_PER_BLOCK];
#endif // CONFIG_EFUSE_VIRTUAL

/*Range addresses to read blocks*/
const esp_efuse_range_addr_t range_read_addr_blocks[] = {
    {EFUSE_RD_WR_DIS_REG,       EFUSE_RD_REPEAT_DATA0_REG}, // range address of EFUSE_BLK0 (2 regs) REPEAT
    {EFUSE_RD_BLK1_DATA0_REG,   EFUSE_RD_BLK1_DATA2_REG},   // range address of EFUSE_BLK1 (3 regs) SYS_DATA_PART0
    {EFUSE_RD_BLK2_DATA0_REG,   EFUSE_RD_BLK2_DATA7_REG},   // range address of EFUSE_BLK2 (8 regs) SYS_DATA_PART_1
    {EFUSE_RD_BLK3_DATA0_REG,   EFUSE_RD_BLK3_DATA7_REG},   // range address of EFUSE_BLK3 (8 regs) KEY0
};

static uint32_t write_mass_blocks[EFUSE_BLK_MAX][COUNT_EFUSE_REG_PER_BLOCK] = { 0 };

/*Range addresses to write blocks (it is not real regs, it is buffer) */
const esp_efuse_range_addr_t range_write_addr_blocks[] = {
    {(uint32_t) &write_mass_blocks[EFUSE_BLK0][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK0][1]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK1][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK1][2]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK2][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK2][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK3][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK3][7]},
};

#ifndef CONFIG_EFUSE_VIRTUAL
// Update Efuse timing configuration
static esp_err_t esp_efuse_set_timing(void)
{
    efuse_hal_set_timing(0);
    return ESP_OK;
}
#endif // ifndef CONFIG_EFUSE_VIRTUAL

// Efuse read operation: copies data from physical efuses to efuse read registers.
void esp_efuse_utility_clear_program_registers(void)
{
    ets_efuse_read();
    ets_efuse_clear_program_registers();
}

esp_err_t esp_efuse_utility_check_errors(void)
{
    return ESP_OK;
}

// Burn values written to the efuse write registers
esp_err_t esp_efuse_utility_burn_chip(void)
{
    return esp_efuse_utility_burn_chip_opt(false, true);
}

esp_err_t esp_efuse_utility_burn_chip_opt(bool ignore_coding_errors, bool verify_written_data)
{
    esp_err_t error = ESP_OK;
#ifdef CONFIG_EFUSE_VIRTUAL
    (void) ignore_coding_errors;
    (void) verify_written_data;
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
            if (esp_efuse_get_coding_scheme(num_block) == EFUSE_CODING_SCHEME_RS) {
                uint8_t block_rs[12];
                efuse_hal_rs_calculate((void *)range_write_addr_blocks[num_block].start, block_rs);
                hal_memcpy((void *)EFUSE_PGM_CHECK_VALUE0_REG, block_rs, sizeof(block_rs));
            }
            unsigned r_data_len = (range_read_addr_blocks[num_block].end - range_read_addr_blocks[num_block].start) + sizeof(uint32_t);
            unsigned data_len = (range_write_addr_blocks[num_block].end - range_write_addr_blocks[num_block].start) + sizeof(uint32_t);
            memcpy((void *)EFUSE_PGM_DATA0_REG, (void *)range_write_addr_blocks[num_block].start, data_len);

            uint32_t backup_write_data[8 + 3]; // 8 words are data and 3 words are RS coding data
            hal_memcpy(backup_write_data, (void *)EFUSE_PGM_DATA0_REG, sizeof(backup_write_data));
            int repeat_burn_op = 1;
            bool correct_written_data;
            bool coding_error_before = !ignore_coding_errors && efuse_hal_is_coding_error_in_block(num_block);
            if (coding_error_before) {
                ESP_LOGW(TAG, "BLOCK%d already has a coding error", num_block);
            }
            bool coding_error_occurred;

            do {
                ESP_LOGI(TAG, "BURN BLOCK%d", num_block);
                efuse_hal_program(num_block); // BURN a block

                bool coding_error_after;
                for (unsigned i = 0; i < 5; i++) {
                    efuse_hal_read();
                    coding_error_after = efuse_hal_is_coding_error_in_block(num_block);
                    if (coding_error_after == true) {
                        break;
                    }
                }
                coding_error_occurred = !ignore_coding_errors && (coding_error_before != coding_error_after) && !coding_error_before;
                if (coding_error_occurred) {
                    ESP_LOGW(TAG, "BLOCK%d got a coding error", num_block);
                }

                correct_written_data = (verify_written_data) ? esp_efuse_utility_is_correct_written_data(num_block, r_data_len) : true;
                if (!correct_written_data || coding_error_occurred) {
                    ESP_LOGW(TAG, "BLOCK%d: next retry to fix an error [%d/3]...", num_block, repeat_burn_op);
                    hal_memcpy((void *)EFUSE_PGM_DATA0_REG, (void *)backup_write_data, sizeof(backup_write_data));
                }

            } while ((!correct_written_data || coding_error_occurred) && repeat_burn_op++ < 3);

            if (coding_error_occurred) {
                ESP_LOGW(TAG, "Coding error was not fixed");
                if (num_block == 0) {
                    ESP_LOGE(TAG, "BLOCK0 got a coding error, which might be critical for security");
                    error = ESP_FAIL;
                }
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

// After esp_efuse_write.. functions EFUSE_BLKx_WDATAx_REG were filled is not coded values.
// This function reads EFUSE_BLKx_WDATAx_REG registers, and checks possible to write these data with RS coding scheme.
// The RS coding scheme does not require data changes for the encoded data. esp32s2 has special registers for this.
// They will be filled during the burn operation.
esp_err_t esp_efuse_utility_apply_new_coding_scheme()
{
    // start with EFUSE_BLK1. EFUSE_BLK0 - always uses EFUSE_CODING_SCHEME_NONE.
    for (int num_block = EFUSE_BLK1; num_block < EFUSE_BLK_MAX; num_block++) {
        if (esp_efuse_get_coding_scheme(num_block) == EFUSE_CODING_SCHEME_RS) {
            for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
                if (REG_READ(addr_wr_block)) {
                    int num_reg = 0;
                    for (uint32_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4, ++num_reg) {
                        if (esp_efuse_utility_read_reg(num_block, num_reg)) {
                            ESP_LOGE(TAG, "Bits are not empty. Write operation is forbidden.");
                            return ESP_ERR_CODING;
                        }
                    }
                    break;
                }
            }
        }
    }
    return ESP_OK;
}
