/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/lcd_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/lcd_cam_reg.h"

const soc_lcd_i80_signal_desc_t soc_lcd_i80_signals[1] = {
    [0] = {
        .module = PERIPH_LCD_CAM_MODULE,
        .irq_id = ETS_LCD_CAM_INTR_SOURCE,
        .data_sigs = {
            LCD_DATA_OUT_PAD_OUT0_IDX,
            LCD_DATA_OUT_PAD_OUT1_IDX,
            LCD_DATA_OUT_PAD_OUT2_IDX,
            LCD_DATA_OUT_PAD_OUT3_IDX,
            LCD_DATA_OUT_PAD_OUT4_IDX,
            LCD_DATA_OUT_PAD_OUT5_IDX,
            LCD_DATA_OUT_PAD_OUT6_IDX,
            LCD_DATA_OUT_PAD_OUT7_IDX,
            LCD_DATA_OUT_PAD_OUT8_IDX,
            LCD_DATA_OUT_PAD_OUT9_IDX,
            LCD_DATA_OUT_PAD_OUT10_IDX,
            LCD_DATA_OUT_PAD_OUT11_IDX,
            LCD_DATA_OUT_PAD_OUT12_IDX,
            LCD_DATA_OUT_PAD_OUT13_IDX,
            LCD_DATA_OUT_PAD_OUT14_IDX,
            LCD_DATA_OUT_PAD_OUT15_IDX,
            LCD_DATA_OUT_PAD_OUT16_IDX,
            LCD_DATA_OUT_PAD_OUT17_IDX,
            LCD_DATA_OUT_PAD_OUT18_IDX,
            LCD_DATA_OUT_PAD_OUT19_IDX,
            LCD_DATA_OUT_PAD_OUT20_IDX,
            LCD_DATA_OUT_PAD_OUT21_IDX,
            LCD_DATA_OUT_PAD_OUT22_IDX,
            LCD_DATA_OUT_PAD_OUT23_IDX,
        },
        .cs_sig = LCD_CS_PAD_OUT_IDX,
        .dc_sig = LCD_DC_PAD_OUT_IDX,
        .wr_sig = LCD_PCLK_PAD_OUT_IDX
    }
};

const soc_lcd_rgb_signal_desc_t soc_lcd_rgb_signals[1] = {
    [0] = {
        .module = PERIPH_LCD_CAM_MODULE,
        .irq_id = ETS_LCD_CAM_INTR_SOURCE,
        .data_sigs = {
            LCD_DATA_OUT_PAD_OUT0_IDX,
            LCD_DATA_OUT_PAD_OUT1_IDX,
            LCD_DATA_OUT_PAD_OUT2_IDX,
            LCD_DATA_OUT_PAD_OUT3_IDX,
            LCD_DATA_OUT_PAD_OUT4_IDX,
            LCD_DATA_OUT_PAD_OUT5_IDX,
            LCD_DATA_OUT_PAD_OUT6_IDX,
            LCD_DATA_OUT_PAD_OUT7_IDX,
            LCD_DATA_OUT_PAD_OUT8_IDX,
            LCD_DATA_OUT_PAD_OUT9_IDX,
            LCD_DATA_OUT_PAD_OUT10_IDX,
            LCD_DATA_OUT_PAD_OUT11_IDX,
            LCD_DATA_OUT_PAD_OUT12_IDX,
            LCD_DATA_OUT_PAD_OUT13_IDX,
            LCD_DATA_OUT_PAD_OUT14_IDX,
            LCD_DATA_OUT_PAD_OUT15_IDX,
            LCD_DATA_OUT_PAD_OUT16_IDX,
            LCD_DATA_OUT_PAD_OUT17_IDX,
            LCD_DATA_OUT_PAD_OUT18_IDX,
            LCD_DATA_OUT_PAD_OUT19_IDX,
            LCD_DATA_OUT_PAD_OUT20_IDX,
            LCD_DATA_OUT_PAD_OUT21_IDX,
            LCD_DATA_OUT_PAD_OUT22_IDX,
            LCD_DATA_OUT_PAD_OUT23_IDX,
        },
        .hsync_sig = LCD_H_SYNC_PAD_OUT_IDX,
        .vsync_sig = LCD_V_SYNC_PAD_OUT_IDX,
        .pclk_sig = LCD_PCLK_PAD_OUT_IDX,
        .de_sig = LCD_H_ENABLE_PAD_OUT_IDX,
        .disp_sig = SIG_GPIO_OUT_IDX,
    }
};

/**
 * LCD_CAM Registers to be saved during sleep retention
 * - LCD Clock Configuration registers: LCDCAM_LCD_CLOCK_REG (0x0)
 * - LCD RGB/YUV Configuration registers: LCDCAM_LCD_RGB_YUV_REG (0x10)
 * - LCD User Configuration registers: LCDCAM_LCD_USER_REG (0x14), LCDCAM_LCD_MISC_REG (0x18)
 * - LCD Control registers: LCDCAM_LCD_CTRL_REG (0x1c), LCDCAM_LCD_CTRL1_REG (0x20), LCDCAM_LCD_CTRL2_REG (0x24)
 * - LCD Command Value registers: LCDCAM_LCD_FIRST_CMD_VAL_REG (0x28), LCDCAM_LCD_LATTER_CMD_VAL_REG (0x2c)
 * - LCD Delay Mode Configuration registers: LCDCAM_LCD_DLY_MODE_CFG1_REG (0x30), LCDCAM_LCD_DLY_MODE_CFG2_REG (0x38)
 * - LCD DMA Interrupt Enable register: LCDCAM_LC_DMA_INT_ENA_REG (0x64)
 *
 *  NOTE: Only I80 LCD supports sleep retention, not RGB LCD.
 */
#define LCD_RETENTION_REGS_CNT 12
#define LCD_RETENTION_REGS_BASE (DR_REG_LCDCAM_BASE + 0x0)
static const uint32_t lcd_cam_regs_map[4] = {0x2005ff1, 0x0, 0x0, 0x0};
static const regdma_entries_config_t lcd_regs_retention[] = {
    // backup stage: save configuration registers
    // restore stage: restore the configuration registers
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_LCDCAM_LINK(0x00),
                                            LCD_RETENTION_REGS_BASE, LCD_RETENTION_REGS_BASE,
                                            LCD_RETENTION_REGS_CNT, 0, 0,
                                            lcd_cam_regs_map[0], lcd_cam_regs_map[1],
                                            lcd_cam_regs_map[2], lcd_cam_regs_map[3]),
        .owner = ENTRY(0),
    },
};

const soc_i80_lcd_retention_info_t soc_i80_lcd_retention_info[1] = {
    [0] = {
        .regdma_entry_array = lcd_regs_retention,
        .array_size = ARRAY_SIZE(lcd_regs_retention),
        .retention_module = SLEEP_RETENTION_MODULE_LCDCAM
    },
};
