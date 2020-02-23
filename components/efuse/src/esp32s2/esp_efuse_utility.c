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
#include "esp32s2/clk.h"
#include "esp_log.h"
#include "assert.h"
#include "sdkconfig.h"
#include <sys/param.h>
#include "esp32s2/rom/efuse.h"

static const char *TAG = "efuse";

#ifdef CONFIG_EFUSE_VIRTUAL
extern uint32_t virt_blocks[COUNT_EFUSE_BLOCKS][COUNT_EFUSE_REG_PER_BLOCK];
#endif // CONFIG_EFUSE_VIRTUAL

/*Range addresses to read blocks*/
const esp_efuse_range_addr_t range_read_addr_blocks[] = {
    {EFUSE_RD_WR_DIS_REG,       EFUSE_RD_REPEAT_DATA4_REG},      // range address of EFUSE_BLK0  REPEAT
    {EFUSE_RD_MAC_SPI_SYS_0_REG, EFUSE_RD_MAC_SPI_SYS_5_REG},      // range address of EFUSE_BLK1  MAC_SPI_8M
    {EFUSE_RD_SYS_PART1_DATA0_REG,    EFUSE_RD_SYS_PART1_DATA7_REG},         // range address of EFUSE_BLK2  SYS_DATA
    {EFUSE_RD_USR_DATA0_REG,    EFUSE_RD_USR_DATA7_REG},         // range address of EFUSE_BLK3  USR_DATA
    {EFUSE_RD_KEY0_DATA0_REG,   EFUSE_RD_KEY0_DATA7_REG},        // range address of EFUSE_BLK4  KEY0
    {EFUSE_RD_KEY1_DATA0_REG,   EFUSE_RD_KEY1_DATA7_REG},        // range address of EFUSE_BLK5  KEY1
    {EFUSE_RD_KEY2_DATA0_REG,   EFUSE_RD_KEY2_DATA7_REG},        // range address of EFUSE_BLK6  KEY2
    {EFUSE_RD_KEY3_DATA0_REG,   EFUSE_RD_KEY3_DATA7_REG},        // range address of EFUSE_BLK7  KEY3
    {EFUSE_RD_KEY4_DATA0_REG,   EFUSE_RD_KEY4_DATA7_REG},        // range address of EFUSE_BLK8  KEY4
    {EFUSE_RD_KEY5_DATA0_REG,   EFUSE_RD_KEY5_DATA7_REG},        // range address of EFUSE_BLK9  KEY5
    {EFUSE_RD_SYS_PART2_DATA0_REG,   EFUSE_RD_SYS_PART2_DATA7_REG}         // range address of EFUSE_BLK10 KEY6
};

static uint32_t write_mass_blocks[COUNT_EFUSE_BLOCKS][COUNT_EFUSE_REG_PER_BLOCK] = { 0 };

/*Range addresses to write blocks (it is not real regs, it is buffer) */
const esp_efuse_range_addr_t range_write_addr_blocks[] = {
    {(uint32_t) &write_mass_blocks[EFUSE_BLK0][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK0][5]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK1][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK1][5]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK2][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK2][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK3][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK3][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK4][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK4][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK5][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK5][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK6][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK6][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK7][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK7][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK8][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK8][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK9][0],  (uint32_t) &write_mass_blocks[EFUSE_BLK9][7]},
    {(uint32_t) &write_mass_blocks[EFUSE_BLK10][0], (uint32_t) &write_mass_blocks[EFUSE_BLK10][7]},
};

#ifndef CONFIG_EFUSE_VIRTUAL
// Update Efuse timing configuration
static esp_err_t esp_efuse_set_timing(void)
{
    uint32_t clock_hz = esp_clk_apb_freq();
    return ets_efuse_set_timing(clock_hz) ? ESP_FAIL : ESP_OK;
}
#endif // ifndef CONFIG_EFUSE_VIRTUAL

// Efuse read operation: copies data from physical efuses to efuse read registers.
void esp_efuse_utility_clear_program_registers(void)
{
    ets_efuse_read();
    ets_efuse_clear_program_registers();
}

// Burn values written to the efuse write registers
void esp_efuse_utility_burn_efuses(void)
{
#ifdef CONFIG_EFUSE_VIRTUAL
    ESP_LOGW(TAG, "Virtual efuses enabled: Not really burning eFuses");
    for (int num_block = 0; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
        int subblock = 0;
        for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
            virt_blocks[num_block][subblock++] |= REG_READ(addr_wr_block);
        }
    }
#else
    if (esp_efuse_set_timing() != ESP_OK) {
        ESP_LOGE(TAG, "Efuse fields are not burnt");
    } else {
        // Permanently update values written to the efuse write registers
        for (int num_block = 0; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
            for (uint32_t addr_wr_block = range_write_addr_blocks[num_block].start; addr_wr_block <= range_write_addr_blocks[num_block].end; addr_wr_block += 4) {
                if (REG_READ(addr_wr_block) != 0) {
                    if (esp_efuse_get_coding_scheme(num_block) == EFUSE_CODING_SCHEME_RS) {
                        uint8_t block_rs[12];
                        ets_efuse_rs_calculate((void *)range_write_addr_blocks[num_block].start, block_rs);
                        memcpy((void *)EFUSE_PGM_CHECK_VALUE0_REG, block_rs, sizeof(block_rs));
                    }
                    int data_len = (range_write_addr_blocks[num_block].end - range_write_addr_blocks[num_block].start) + sizeof(uint32_t);
                    memcpy((void *)EFUSE_PGM_DATA0_REG, (void *)range_write_addr_blocks[num_block].start, data_len);
                    ets_efuse_program(num_block);
                    break;
                }
            }
        }
    }
#endif // CONFIG_EFUSE_VIRTUAL
    esp_efuse_utility_reset();
}

// After esp_efuse_write.. functions EFUSE_BLKx_WDATAx_REG were filled is not coded values.
// This function reads EFUSE_BLKx_WDATAx_REG registers, and checks possible to write these data with RS coding scheme.
// The RS coding scheme does not require data changes for the encoded data. esp32s2 has special registers for this.
// They will be filled during the burn operation.
esp_err_t esp_efuse_utility_apply_new_coding_scheme()
{
    // start with EFUSE_BLK1. EFUSE_BLK0 - always uses EFUSE_CODING_SCHEME_NONE.
    for (int num_block = 1; num_block < COUNT_EFUSE_BLOCKS; num_block++) {
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
