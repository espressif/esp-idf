/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include "soc/soc_caps.h"
#include "hal/efuse_ll.h"
#include "hal/assert.h"
#include "hal/efuse_hal.h"
#include "soc/syscon_reg.h"
#include "esp_attr.h"

IRAM_ATTR uint32_t efuse_hal_get_major_chip_version(void)
{
    uint8_t eco_bit0 = efuse_ll_get_chip_ver_rev1();
    uint8_t eco_bit1 = efuse_ll_get_chip_ver_rev2();
    uint8_t eco_bit2 = (REG_READ(SYSCON_DATE_REG) & 0x80000000) >> 31;
    uint32_t combine_value = (eco_bit2 << 2) | (eco_bit1 << 1) | eco_bit0;
    uint32_t chip_ver = 0;
    switch (combine_value) {
    case 0:
        chip_ver = 0;
        break;
    case 1:
        chip_ver = 1;
        break;
    case 3:
        chip_ver = 2;
        break;
#if CONFIG_IDF_ENV_FPGA
    case 4: /* Empty efuses, but SYSCON_DATE_REG bit is set */
        chip_ver = 3;
        break;
#endif // CONFIG_IDF_ENV_FPGA
    case 7:
        chip_ver = 3;
        break;
    default:
        chip_ver = 0;
        break;
    }
    return chip_ver;
}

IRAM_ATTR uint32_t efuse_hal_get_minor_chip_version(void)
{
    return efuse_ll_get_chip_wafer_version_minor();
}

uint32_t efuse_hal_get_rated_freq_mhz(void)
{
    //Check if ESP32 is rated for a CPU frequency of 160MHz only
    if (efuse_ll_get_chip_cpu_freq_rated() && efuse_ll_get_chip_cpu_freq_low()) {
        return 160;
    }
    return 240;
}

/******************* eFuse control functions *************************/

void efuse_hal_set_timing(uint32_t apb_freq_mhz)
{
    uint32_t clk_sel0;
    uint32_t clk_sel1;
    uint32_t dac_clk_div;
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
    efuse_ll_set_dac_clk_div(dac_clk_div);
    efuse_ll_set_dac_clk_sel0(clk_sel0);
    efuse_ll_set_dac_clk_sel1(clk_sel1);
}

void efuse_hal_read(void)
{
    efuse_ll_set_conf_read_op_code();
    efuse_ll_set_read_cmd();
    while (efuse_ll_get_cmd() != 0) { };
}

void efuse_hal_clear_program_registers(void)
{
    for (uint32_t r = EFUSE_BLK0_WDATA0_REG; r <= EFUSE_BLK0_WDATA6_REG; r += 4) {
        REG_WRITE(r, 0);
    }
    for (uint32_t r = EFUSE_BLK1_WDATA0_REG; r <= EFUSE_BLK1_WDATA7_REG; r += 4) {
        REG_WRITE(r, 0);
    }
    for (uint32_t r = EFUSE_BLK2_WDATA0_REG; r <= EFUSE_BLK2_WDATA7_REG; r += 4) {
        REG_WRITE(r, 0);
    }
    for (uint32_t r = EFUSE_BLK3_WDATA0_REG; r <= EFUSE_BLK3_WDATA7_REG; r += 4) {
        REG_WRITE(r, 0);
    }
}

void efuse_hal_program(uint32_t block)
{
    (void) block;
    // Permanently update values written to the efuse write registers
    efuse_ll_set_conf_write_op_code();
    efuse_ll_set_pgm_cmd();
    while (efuse_ll_get_cmd() != 0) { };

    efuse_hal_read();
}

/******************* eFuse control functions *************************/

bool efuse_hal_is_coding_error_in_block(unsigned block)
{
    return block > 0 &&
           efuse_ll_get_coding_scheme() == 1 && // 3/4 coding scheme
           efuse_ll_get_dec_warnings(block);
}
