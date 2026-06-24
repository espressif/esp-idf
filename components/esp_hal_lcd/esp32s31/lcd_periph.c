/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/lcd_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_num.h"

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
        .wr_sig = LCD_PCLK_PAD_OUT_IDX,
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

const soc_lcd_rgb_iomux_desc_t soc_lcd_rgb_iomux_descs[1] = {
    [0] = {
        .data_pins = {
            { .gpio_num = GPIO_NUM_8,  .func = FUNC_GPIO8_LCD_DATA0_OUT_PAD },
            { .gpio_num = GPIO_NUM_9,  .func = FUNC_GPIO9_LCD_DATA1_OUT_PAD },
            { .gpio_num = GPIO_NUM_10, .func = FUNC_GPIO10_LCD_DATA2_OUT_PAD },
            { .gpio_num = GPIO_NUM_11, .func = FUNC_GPIO11_LCD_DATA3_OUT_PAD },
            { .gpio_num = GPIO_NUM_12, .func = FUNC_GPIO12_LCD_DATA4_OUT_PAD },
            { .gpio_num = GPIO_NUM_13, .func = FUNC_GPIO13_LCD_DATA5_OUT_PAD },
            { .gpio_num = GPIO_NUM_14, .func = FUNC_GPIO14_LCD_DATA6_OUT_PAD },
            { .gpio_num = GPIO_NUM_15, .func = FUNC_GPIO15_LCD_DATA7_OUT_PAD },
            { .gpio_num = GPIO_NUM_16, .func = FUNC_GPIO16_LCD_DATA8_OUT_PAD },
            { .gpio_num = GPIO_NUM_17, .func = FUNC_GPIO17_LCD_DATA9_OUT_PAD },
            { .gpio_num = GPIO_NUM_18, .func = FUNC_GPIO18_LCD_DATA10_OUT_PAD },
            { .gpio_num = GPIO_NUM_19, .func = FUNC_GPIO19_LCD_DATA11_OUT_PAD },
            { .gpio_num = GPIO_NUM_33, .func = FUNC_GPIO33_LCD_DATA12_OUT_PAD },
            { .gpio_num = GPIO_NUM_34, .func = FUNC_GPIO34_LCD_DATA13_OUT_PAD },
            { .gpio_num = GPIO_NUM_35, .func = FUNC_GPIO35_LCD_DATA14_OUT_PAD },
            { .gpio_num = GPIO_NUM_36, .func = FUNC_GPIO36_LCD_DATA15_OUT_PAD },
            { .gpio_num = GPIO_NUM_37, .func = FUNC_GPIO37_LCD_DATA16_OUT_PAD },
            { .gpio_num = GPIO_NUM_38, .func = FUNC_GPIO38_LCD_DATA17_OUT_PAD },
            { .gpio_num = GPIO_NUM_39, .func = FUNC_GPIO39_LCD_DATA18_OUT_PAD },
            { .gpio_num = GPIO_NUM_2,  .func = FUNC_GPIO2_LCD_DATA19_OUT_PAD },
            { .gpio_num = GPIO_NUM_3,  .func = FUNC_GPIO3_LCD_DATA20_OUT_PAD },
            { .gpio_num = GPIO_NUM_4,  .func = FUNC_GPIO4_LCD_DATA21_OUT_PAD },
            { .gpio_num = GPIO_NUM_5,  .func = FUNC_GPIO5_LCD_DATA22_OUT_PAD },
            { .gpio_num = GPIO_NUM_7,  .func = FUNC_GPIO7_LCD_DATA23_OUT_PAD },
        },
        .hsync_pin = { .gpio_num = GPIO_NUM_44, .func = FUNC_GPIO44_LCD_H_SYNC_PAD },
        .vsync_pin = { .gpio_num = GPIO_NUM_45, .func = FUNC_GPIO45_LCD_V_SYNC_PAD },
        .pclk_pin = { .gpio_num = GPIO_NUM_40, .func = FUNC_GPIO40_LCD_PCLK_PAD },
        .de_pin = { .gpio_num = GPIO_NUM_43, .func = FUNC_GPIO43_LCD_H_ENABLE_PAD },
    },
};

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

const soc_i80_lcd_retention_info_t soc_i80_lcd_retention_info[1] = {
    [0] = {
        .regdma_entry_array = lcd_regs_retention,
        .array_size = ARRAY_SIZE(lcd_regs_retention),
        .retention_module = SLEEP_RETENTION_MODULE_LCDCAM
    },
};
