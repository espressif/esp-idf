/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of cpu_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_core_clk_force_on : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_core_clk_force_on:1;
        /** lp_core_rst_en : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_core_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_cpu_ctrl_reg_t;

/** Type of rom_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_rom_force_on : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_rom_force_on:1;
        /** lp_rom_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_rom_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_rom_ctrl_reg_t;

/** Type of ram_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_ram_clk_en : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_ram_clk_en:1;
        /** lp_ram_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_ram_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_ram_ctrl_reg_t;

/** Type of uart_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** lp_uart_clk_div_num : R/W; bitpos: [27:20]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_clk_div_num:8;
        /** lp_uart_clk_sel : R/W; bitpos: [29:28]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_clk_sel:2;
        /** lp_uart_clk_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_clk_en:1;
        /** lp_uart_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_uart_ctrl_reg_t;

/** Type of i2c_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** lp_i2c_clk_div_num : R/W; bitpos: [27:20]; default: 0;
         *  need_des
         */
        uint32_t lp_i2c_clk_div_num:8;
        /** lp_i2c_clk_sel : R/W; bitpos: [29:28]; default: 0;
         *  need_des
         */
        uint32_t lp_i2c_clk_sel:2;
        /** lp_i2c_clk_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_i2c_clk_en:1;
        /** lp_i2c_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_i2c_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_i2c_ctrl_reg_t;

/** Type of i2cmst_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_i2cmst_clk_en : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_i2cmst_clk_en:1;
        /** lp_i2cmst_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_i2cmst_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_i2cmst_ctrl_reg_t;

/** Type of spi_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_spi_clk_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_spi_clk_en:1;
        /** lp_spi_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_spi_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_spi_ctrl_reg_t;

/** Type of adc_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** lp_adc_div_num : R/W; bitpos: [7:0]; default: 4;
         *  need_des
         */
        uint32_t lp_adc_div_num:8;
        uint32_t reserved_8:20;
        /** lp_adc_clk_sel : R/W; bitpos: [29:28]; default: 0;
         *  need_des
         */
        uint32_t lp_adc_clk_sel:2;
        /** lp_adc_clk_en : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_adc_clk_en:1;
        /** lp_adc_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_adc_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_adc_ctrl_reg_t;

/** Type of efuse_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_efuse_clk_en : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_efuse_clk_en:1;
        /** lp_efuse_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_efuse_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_efuse_ctrl_reg_t;

/** Type of intr_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_intr_clk_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_intr_clk_en:1;
        /** lp_intr_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_intr_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_intr_ctrl_reg_t;

/** Type of touch_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_touch_clk_en : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_touch_clk_en:1;
        /** lp_touch_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_touch_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_touch_ctrl_reg_t;

/** Type of tsens_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_tsens_clk_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_tsens_clk_en:1;
        /** lp_tsens_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_tsens_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_tsens_ctrl_reg_t;

/** Type of iomux_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_iomux_clk_en : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_iomux_clk_en:1;
        /** lp_iomux_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_iomux_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_iomux_ctrl_reg_t;

/** Type of mailbox register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_mailbox_clk_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_mailbox_clk_en:1;
        /** lp_mailbox_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_mailbox_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_mailbox_reg_t;

/** Type of rng_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_rng_clk_en : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_rng_clk_en:1;
        /** lp_rng_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_rng_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_rng_ctrl_reg_t;

/** Type of uart_misc_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** lp_uart_wakeup_flag_clr : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_wakeup_flag_clr:1;
        /** lp_uart_wakeup_flag : R/WTC/SS; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_wakeup_flag:1;
        uint32_t reserved_2:27;
        /** lp_uart_wakeup_en : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_wakeup_en:1;
        /** lp_uart_mem_force_pd : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t lp_uart_mem_force_pd:1;
        /** lp_uart_mem_force_pu : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_uart_mem_force_pu:1;
    };
    uint32_t val;
} lp_periclkrst_uart_misc_ctrl_reg_t;

/** Type of cpu register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lpcore_dbgm_unavailable : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t lpcore_dbgm_unavailable:1;
    };
    uint32_t val;
} lp_periclkrst_cpu_reg_t;

/** Type of ahb_dma_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_ahb_dma_clk_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_ahb_dma_clk_en:1;
        /** lp_ahb_dma_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_ahb_dma_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_ahb_dma_ctrl_reg_t;

/** Type of dac_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** lp_dac_div_num : R/W; bitpos: [7:0]; default: 4;
         *  need_des
         */
        uint32_t lp_dac_div_num:8;
        uint32_t reserved_8:20;
        /** lp_dac_clk_sel : R/W; bitpos: [29:28]; default: 0;
         *  need_des
         */
        uint32_t lp_dac_clk_sel:2;
        /** lp_dac_clk_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t lp_dac_clk_en:1;
        /** lp_dac_rst_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_dac_rst_en:1;
    };
    uint32_t val;
} lp_periclkrst_dac_ctrl_reg_t;

/** Type of dm_ctrl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** lp_dm_clk_en : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t lp_dm_clk_en:1;
    };
    uint32_t val;
} lp_periclkrst_dm_ctrl_reg_t;


/** Group: Version register */
/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 38834448;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_periclkrst_date_reg_t;


typedef struct {
    uint32_t reserved_000;
    volatile lp_periclkrst_cpu_ctrl_reg_t cpu_ctrl;
    volatile lp_periclkrst_rom_ctrl_reg_t rom_ctrl;
    volatile lp_periclkrst_ram_ctrl_reg_t ram_ctrl;
    volatile lp_periclkrst_uart_ctrl_reg_t uart_ctrl;
    volatile lp_periclkrst_i2c_ctrl_reg_t i2c_ctrl;
    volatile lp_periclkrst_i2cmst_ctrl_reg_t i2cmst_ctrl;
    volatile lp_periclkrst_spi_ctrl_reg_t spi_ctrl;
    volatile lp_periclkrst_adc_ctrl_reg_t adc_ctrl;
    volatile lp_periclkrst_efuse_ctrl_reg_t efuse_ctrl;
    volatile lp_periclkrst_intr_ctrl_reg_t intr_ctrl;
    volatile lp_periclkrst_touch_ctrl_reg_t touch_ctrl;
    volatile lp_periclkrst_tsens_ctrl_reg_t tsens_ctrl;
    volatile lp_periclkrst_iomux_ctrl_reg_t iomux_ctrl;
    volatile lp_periclkrst_mailbox_reg_t mailbox;
    volatile lp_periclkrst_rng_ctrl_reg_t rng_ctrl;
    volatile lp_periclkrst_uart_misc_ctrl_reg_t uart_misc_ctrl;
    volatile lp_periclkrst_cpu_reg_t cpu;
    volatile lp_periclkrst_ahb_dma_ctrl_reg_t ahb_dma_ctrl;
    volatile lp_periclkrst_dac_ctrl_reg_t dac_ctrl;
    volatile lp_periclkrst_dm_ctrl_reg_t dm_ctrl;
    uint32_t reserved_054[234];
    volatile lp_periclkrst_date_reg_t date;
} lp_peri_clkrst_dev_t;

extern lp_peri_clkrst_dev_t LP_PERI_CLKRST;

#ifndef __cplusplus
_Static_assert(sizeof(lp_peri_clkrst_dev_t) == 0x400, "Invalid size of lp_peri_clkrst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
