/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
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
#include "esp32c2/rom/efuse.h"

/*Range addresses to read blocks*/
const esp_efuse_range_addr_t range_read_addr_blocks[] = {
    {EFUSE_RD_WR_DIS_REG,       EFUSE_RD_REPEAT_DATA0_REG},      // range address of EFUSE_BLK0  REPEAT
    {EFUSE_RD_BLK1_DATA0_REG,   EFUSE_RD_BLK1_DATA2_REG},        // range address of EFUSE_BLK1  SYS_DATA_PART0
    {EFUSE_RD_BLK2_DATA0_REG,   EFUSE_RD_BLK2_DATA7_REG},        // range address of EFUSE_BLK2  SYS_DATA_PART_1
    {EFUSE_RD_BLK3_DATA0_REG,   EFUSE_RD_BLK3_DATA7_REG},        // range address of EFUSE_BLK3  KEY0
};

// Efuse read operation: copies data from physical efuses to efuse read registers.
void esp_efuse_utility_clear_program_registers(void)
{
    abort();
}

// Burn values written to the efuse write registers
void esp_efuse_utility_burn_chip(void)
{
    abort();
}

// After esp_efuse_write.. functions EFUSE_BLKx_WDATAx_REG were filled is not coded values.
// This function reads EFUSE_BLKx_WDATAx_REG registers, and checks possible to write these data with RS coding scheme.
// The RS coding scheme does not require data changes for the encoded data. esp32s2 has special registers for this.
// They will be filled during the burn operation.
esp_err_t esp_efuse_utility_apply_new_coding_scheme()
{
    abort();
}
