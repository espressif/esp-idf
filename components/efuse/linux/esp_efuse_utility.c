/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_efuse_utility.h"
#include "soc/efuse_periph.h"
#include "hal/efuse_hal.h"
#include "esp_log.h"
#include "assert.h"
#include "sdkconfig.h"
#include <sys/param.h>

#ifndef CONFIG_EFUSE_VIRTUAL
    #error Linux target supports only VIRTUAL eFuses
#endif // CONFIG_EFUSE_VIRTUAL

static const char *TAG = "efuse";

extern uint32_t virt_blocks[EFUSE_BLK_MAX][COUNT_EFUSE_REG_PER_BLOCK];

/*Range addresses to read blocks*/
const esp_efuse_range_addr_t range_read_addr_blocks[] = {
    {(uintptr_t) &virt_blocks[EFUSE_BLK0][0],  (uintptr_t) &virt_blocks[EFUSE_BLK0][5]},  // range address of EFUSE_BLK0  REPEAT
    {(uintptr_t) &virt_blocks[EFUSE_BLK1][0],  (uintptr_t) &virt_blocks[EFUSE_BLK1][5]},  // range address of EFUSE_BLK1  MAC_SPI_8M
    {(uintptr_t) &virt_blocks[EFUSE_BLK2][0],  (uintptr_t) &virt_blocks[EFUSE_BLK2][7]},  // range address of EFUSE_BLK2  SYS_DATA
    {(uintptr_t) &virt_blocks[EFUSE_BLK3][0],  (uintptr_t) &virt_blocks[EFUSE_BLK3][7]},  // range address of EFUSE_BLK3  USR_DATA
    {(uintptr_t) &virt_blocks[EFUSE_BLK4][0],  (uintptr_t) &virt_blocks[EFUSE_BLK4][7]},  // range address of EFUSE_BLK4  KEY0
    {(uintptr_t) &virt_blocks[EFUSE_BLK5][0],  (uintptr_t) &virt_blocks[EFUSE_BLK5][7]},  // range address of EFUSE_BLK5  KEY1
    {(uintptr_t) &virt_blocks[EFUSE_BLK6][0],  (uintptr_t) &virt_blocks[EFUSE_BLK6][7]},  // range address of EFUSE_BLK6  KEY2
    {(uintptr_t) &virt_blocks[EFUSE_BLK7][0],  (uintptr_t) &virt_blocks[EFUSE_BLK7][7]},  // range address of EFUSE_BLK7  KEY3
    {(uintptr_t) &virt_blocks[EFUSE_BLK8][0],  (uintptr_t) &virt_blocks[EFUSE_BLK8][7]},  // range address of EFUSE_BLK8  KEY4
    {(uintptr_t) &virt_blocks[EFUSE_BLK9][0],  (uintptr_t) &virt_blocks[EFUSE_BLK9][7]},  // range address of EFUSE_BLK9  KEY5
    {(uintptr_t) &virt_blocks[EFUSE_BLK10][0], (uintptr_t) &virt_blocks[EFUSE_BLK10][7]}, // range address of EFUSE_BLK10 KEY6
};

static uint32_t write_mass_blocks[EFUSE_BLK_MAX][COUNT_EFUSE_REG_PER_BLOCK] = { 0 };

/*Range addresses to write blocks (it is not real regs, it is buffer) */
const esp_efuse_range_addr_t range_write_addr_blocks[] = {
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK0][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK0][5]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK1][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK1][5]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK2][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK2][7]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK3][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK3][7]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK4][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK4][7]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK5][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK5][7]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK6][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK6][7]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK7][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK7][7]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK8][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK8][7]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK9][0],  (uintptr_t) &write_mass_blocks[EFUSE_BLK9][7]},
    {(uintptr_t) &write_mass_blocks[EFUSE_BLK10][0], (uintptr_t) &write_mass_blocks[EFUSE_BLK10][7]},
};

// Efuse read operation: copies data from physical efuses to efuse read registers.
void esp_efuse_utility_clear_program_registers(void)
{
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
    ESP_LOGW(TAG, "Virtual efuses enabled: Not really burning eFuses");
    for (int num_block = EFUSE_BLK_MAX - 1; num_block >= EFUSE_BLK0; num_block--) {
        int subblock = 0;
        for (uintptr_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            virt_blocks[num_block][subblock++] |= REG_READ(addr_wr_block);
        }
    }
#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
    esp_efuse_utility_write_efuses_to_flash();
#endif
    esp_efuse_utility_reset();
    return error;
}

// After esp_efuse_write.. functions EFUSE_BLKx_WDATAx_REG were filled is not coded values.
// This function reads EFUSE_BLKx_WDATAx_REG registers, and checks possible to write these data with RS coding scheme.
// The RS coding scheme does not require data changes for the encoded data. esp32s2 has special registers for this.
// They will be filled during the burn operation.
esp_err_t esp_efuse_utility_apply_new_coding_scheme(void)
{
    // start with EFUSE_BLK1. EFUSE_BLK0 - always uses EFUSE_CODING_SCHEME_NONE.
    for (int num_block = EFUSE_BLK1; num_block < EFUSE_BLK_MAX; num_block++) {
        if (esp_efuse_get_coding_scheme(num_block) == EFUSE_CODING_SCHEME_RS) {
            for (uintptr_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
                if (REG_READ(addr_wr_block)) {
                    int num_reg = 0;
                    for (uintptr_t addr_rd_block = range_read_addr_blocks[num_block].start; addr_rd_block <= range_read_addr_blocks[num_block].end; addr_rd_block += 4, ++num_reg) {
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
