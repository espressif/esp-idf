/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "i80_io_priv.h"
#include "soc/lcd_cam_reg.h"

/**
 * LCD_CAM Registers to be saved during sleep retention
 * - LCD Clock Configuration registers: LCDCAM_LCD_CLOCK_REG (0x0)
 * - LCD User Configuration registers: LCDCAM_LCD_USER_REG (0x14), LCDCAM_LCD_MISC_REG (0x18)
 * - LCD Delay Mode Configuration registers: LCDCAM_LCD_DLY_MODE_CFG1_REG (0x34), LCDCAM_LCD_DLY_MODE_CFG2_REG (0x38)
 * - LCD Transfer Buffer Configuration registers: LCDCAM_LCD_TRANS_BUFF_CFG_REG(0x3C)
 * - LCD DMA Interrupt Enable register: LCDCAM_LC_DMA_INT_ENA_REG (0x64)
 *
 *  NOTE: Only I80 LCD supports sleep retention, not RGB LCD.
 */
#define LCD_RETENTION_REGS_CNT 7
#define LCD_RETENTION_REGS_BASE (DR_REG_LCDCAM_BASE + 0x0)
static const uint32_t lcd_cam_regs_map[4] = {0x200e061, 0x0, 0x0, 0x0};
static const regdma_entries_config_t lcd_regs_retention[] = {
    // backup stage: save configuration registers
    // restore stage: restore the configuration registers
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_LCDCAM_LINK(0x00),
                                            LCD_RETENTION_REGS_BASE, LCD_RETENTION_REGS_BASE,
                                            LCD_RETENTION_REGS_CNT, 0, 0,
                                            lcd_cam_regs_map[0], lcd_cam_regs_map[1],
                                            lcd_cam_regs_map[2], lcd_cam_regs_map[3]),
        .owner = ENTRY(0) | ENTRY(2),
    },
};

const lcd_i80_reg_retention_info_t lcd_i80_reg_retention_info[1] = {
    [0] = {
        .regdma_entry_array = lcd_regs_retention,
        .array_size = ARRAY_SIZE(lcd_regs_retention),
        .retention_module = SLEEP_RETENTION_MODULE_LCDCAM
    },
};
