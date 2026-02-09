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

/** Group: Configure Register */
/** Type of ctrl0 register
 *  Register
 */
typedef union {
    struct {
        /** sar1_trigger_stop : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t sar1_trigger_stop:1;
        /** sar1_trigger_start : WT; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t sar1_trigger_start:1;
        /** sar1_trigger_mode : R/W; bitpos: [3:2]; default: 0;
         *  SAR clock divider
         */
        uint32_t sar1_trigger_mode:2;
        uint32_t reserved_4:10;
        /** sar1_patt_len : R/W; bitpos: [17:14]; default: 15;
         *  0 ~ 15 means length 1 ~ 16
         */
        uint32_t sar1_patt_len:4;
        uint32_t reserved_18:4;
        /** sar1_patt_p_clear : WT; bitpos: [22]; default: 0;
         *  clear the pointer of pattern table for DIG ADC1 CTRL
         */
        uint32_t sar1_patt_p_clear:1;
        /** sar1_patt_type : R/W; bitpos: [23]; default: 0;
         *  1: select meas start as table update 0:select mead done as table update
         */
        uint32_t sar1_patt_type:1;
        /** sar1_clk_pos_sel : R/W; bitpos: [24]; default: 0;
         *  1: sar_sel will be coded by the MSB of the 16-bit output data, in this case the
         *  resolution should not be larger than 11 bits.
         */
        uint32_t sar1_clk_pos_sel:1;
        /** sar1_continue_mode_en : R/W; bitpos: [25]; default: 0;
         *  1: I2S input data is from SAR ADC (for DMA), 0: I2S input data is from GPIO matrix
         */
        uint32_t sar1_continue_mode_en:1;
        /** xpd_sar1_force : R/W; bitpos: [27:26]; default: 0;
         *  force option to xpd sar1 blocks
         */
        uint32_t xpd_sar1_force:2;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} adc_ctrl0_reg_t;

/** Type of ctrl1 register
 *  Register
 */
typedef union {
    struct {
        /** sar2_trigger_stop : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t sar2_trigger_stop:1;
        /** sar2_trigger_start : WT; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t sar2_trigger_start:1;
        /** sar2_trigger_mode : R/W; bitpos: [3:2]; default: 0;
         *  SAR clock divider
         */
        uint32_t sar2_trigger_mode:2;
        uint32_t reserved_4:10;
        /** sar2_patt_len : R/W; bitpos: [17:14]; default: 15;
         *  0 ~ 15 means length 1 ~ 16
         */
        uint32_t sar2_patt_len:4;
        uint32_t reserved_18:4;
        /** sar2_patt_p_clear : WT; bitpos: [22]; default: 0;
         *  clear the pointer of pattern table for DIG ADC1 CTRL
         */
        uint32_t sar2_patt_p_clear:1;
        /** sar2_patt_type : R/W; bitpos: [23]; default: 0;
         *  1: select meas start as table update 0:select mead done as table update
         */
        uint32_t sar2_patt_type:1;
        /** sar2_clk_pos_sel : R/W; bitpos: [24]; default: 0;
         *  1: sar_sel will be coded by the MSB of the 16-bit output data, in this case the
         *  resolution should not be larger than 11 bits.
         */
        uint32_t sar2_clk_pos_sel:1;
        /** sar2_continue_mode_en : R/W; bitpos: [25]; default: 0;
         *  1: I2S input data is from SAR ADC (for DMA), 0: I2S input data is from GPIO matrix
         */
        uint32_t sar2_continue_mode_en:1;
        /** xpd_sar2_force : R/W; bitpos: [27:26]; default: 0;
         *  force option to xpd sar1 blocks
         */
        uint32_t xpd_sar2_force:2;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} adc_ctrl1_reg_t;

/** Type of ctrl2 register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** timer_target : R/W; bitpos: [23:12]; default: 10;
         *  to set saradc timer target
         */
        uint32_t timer_target:12;
        /** timer_en : R/W; bitpos: [24]; default: 0;
         *  to enable saradc timer trigger
         */
        uint32_t timer_en:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} adc_ctrl2_reg_t;

/** Type of filter_ctrl1 register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** filter_factor1 : R/W; bitpos: [28:26]; default: 0;
         *  need_des
         */
        uint32_t filter_factor1:3;
        /** filter_factor0 : R/W; bitpos: [31:29]; default: 0;
         *  need_des
         */
        uint32_t filter_factor0:3;
    };
    uint32_t val;
} adc_filter_ctrl1_reg_t;

/** Type of sar1_patt_tab1 register
 *  Register
 */
typedef union {
    struct {
        /** sar1_patt_tab1 : R/W; bitpos: [23:0]; default: 15728640;
         *  item 0 ~ 3 for pattern table 1 (each item one byte)
         */
        uint32_t sar1_patt_tab1:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} adc_sar1_patt_tab1_reg_t;

/** Type of sar1_patt_tab2 register
 *  Register
 */
typedef union {
    struct {
        /** sar1_patt_tab2 : R/W; bitpos: [23:0]; default: 0;
         *  Item 4 ~ 7 for pattern table 1 (each item one byte)
         */
        uint32_t sar1_patt_tab2:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} adc_sar1_patt_tab2_reg_t;

/** Type of sar1_patt_tab3 register
 *  Register
 */
typedef union {
    struct {
        /** sar1_patt_tab3 : R/W; bitpos: [23:0]; default: 0;
         *  Item 8 ~ 11 for pattern table 1 (each item one byte)
         */
        uint32_t sar1_patt_tab3:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} adc_sar1_patt_tab3_reg_t;

/** Type of sar1_patt_tab4 register
 *  Register
 */
typedef union {
    struct {
        /** sar1_patt_tab4 : R/W; bitpos: [23:0]; default: 0;
         *  Item 12 ~ 15 for pattern table 1 (each item one byte)
         */
        uint32_t sar1_patt_tab4:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} adc_sar1_patt_tab4_reg_t;

/** Type of sar2_patt_tab1 register
 *  Register
 */
typedef union {
    struct {
        /** sar2_patt_tab1 : R/W; bitpos: [23:0]; default: 15728640;
         *  item 0 ~ 3 for pattern table 2 (each item one byte)
         */
        uint32_t sar2_patt_tab1:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} adc_sar2_patt_tab1_reg_t;

/** Type of sar2_patt_tab2 register
 *  Register
 */
typedef union {
    struct {
        /** sar2_patt_tab2 : R/W; bitpos: [23:0]; default: 0;
         *  Item 4 ~ 7 for pattern table 2 (each item one byte)
         */
        uint32_t sar2_patt_tab2:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} adc_sar2_patt_tab2_reg_t;

/** Type of sar2_patt_tab3 register
 *  Register
 */
typedef union {
    struct {
        /** sar2_patt_tab3 : R/W; bitpos: [23:0]; default: 0;
         *  Item 8 ~ 11 for pattern table 2 (each item one byte)
         */
        uint32_t sar2_patt_tab3:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} adc_sar2_patt_tab3_reg_t;

/** Type of sar2_patt_tab4 register
 *  Register
 */
typedef union {
    struct {
        /** sar2_patt_tab4 : R/W; bitpos: [23:0]; default: 0;
         *  Item 12 ~ 15 for pattern table 2 (each item one byte)
         */
        uint32_t sar2_patt_tab4:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} adc_sar2_patt_tab4_reg_t;

/** Type of filter_ctrl0 register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:14;
        /** filter_channel1 : R/W; bitpos: [18:14]; default: 13;
         *  need_des
         */
        uint32_t filter_channel1:5;
        /** filter_channel0 : R/W; bitpos: [23:19]; default: 13;
         *  apb_adc1_filter_factor
         */
        uint32_t filter_channel0:5;
        uint32_t reserved_24:7;
        /** filter_reset : WT; bitpos: [31]; default: 0;
         *  enable apb_adc1_filter
         */
        uint32_t filter_reset:1;
    };
    uint32_t val;
} adc_filter_ctrl0_reg_t;

/** Type of sar1_data_status register
 *  Register
 */
typedef union {
    struct {
        /** apb_saradc1_data : RO; bitpos: [21:0]; default: 0;
         *  need_des
         */
        uint32_t apb_saradc1_data:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} adc_sar1_data_status_reg_t;

/** Type of sar2_data_status register
 *  Register
 */
typedef union {
    struct {
        /** apb_saradc2_data : RO; bitpos: [21:0]; default: 0;
         *  need_des
         */
        uint32_t apb_saradc2_data:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} adc_sar2_data_status_reg_t;

/** Type of thres0_high_thres register
 *  Register
 */
typedef union {
    struct {
        /** thres0_high : R/W; bitpos: [16:0]; default: 255;
         *  need_des
         */
        uint32_t thres0_high:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} adc_thres0_high_thres_reg_t;

/** Type of thres0_low_thres register
 *  Register
 */
typedef union {
    struct {
        /** thres0_low : R/W; bitpos: [16:0]; default: 0;
         *  need_des
         */
        uint32_t thres0_low:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} adc_thres0_low_thres_reg_t;

/** Type of thres0_ctrl register
 *  Register
 */
typedef union {
    struct {
        /** thres0_channel : R/W; bitpos: [4:0]; default: 13;
         *  need_des
         */
        uint32_t thres0_channel:5;
        /** thres0_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t thres0_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} adc_thres0_ctrl_reg_t;

/** Type of thres1_high_thres register
 *  Register
 */
typedef union {
    struct {
        /** thres1_high : R/W; bitpos: [16:0]; default: 255;
         *  need_des
         */
        uint32_t thres1_high:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} adc_thres1_high_thres_reg_t;

/** Type of thres1_low_thres register
 *  Register
 */
typedef union {
    struct {
        /** thres1_low : R/W; bitpos: [16:0]; default: 0;
         *  need_des
         */
        uint32_t thres1_low:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} adc_thres1_low_thres_reg_t;

/** Type of thres1_ctrl register
 *  Register
 */
typedef union {
    struct {
        /** thres1_channel : R/W; bitpos: [4:0]; default: 13;
         *  need_des
         */
        uint32_t thres1_channel:5;
        /** thres1_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t thres1_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} adc_thres1_ctrl_reg_t;

/** Type of thres_ctrl register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** thres_all_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t thres_all_en:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} adc_thres_ctrl_reg_t;

/** Type of int_ena register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** fifo_overflow_int_ena : R/W; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t fifo_overflow_int_ena:1;
        /** thres1_low_int_ena : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t thres1_low_int_ena:1;
        /** thres0_low_int_ena : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t thres0_low_int_ena:1;
        /** thres1_high_int_ena : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t thres1_high_int_ena:1;
        /** thres0_high_int_ena : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t thres0_high_int_ena:1;
        /** sar2_done_int_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t sar2_done_int_ena:1;
        /** sar1_done_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t sar1_done_int_ena:1;
    };
    uint32_t val;
} adc_int_ena_reg_t;

/** Type of int_raw register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** fifo_overflow_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t fifo_overflow_int_raw:1;
        /** thres1_low_int_raw : R/WTC/SS; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t thres1_low_int_raw:1;
        /** thres0_low_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t thres0_low_int_raw:1;
        /** thres1_high_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t thres1_high_int_raw:1;
        /** thres0_high_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t thres0_high_int_raw:1;
        /** sar2_done_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t sar2_done_int_raw:1;
        /** sar1_done_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t sar1_done_int_raw:1;
    };
    uint32_t val;
} adc_int_raw_reg_t;

/** Type of int_st register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** fifo_overflow_int_st : RO; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t fifo_overflow_int_st:1;
        /** thres1_low_int_st : RO; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t thres1_low_int_st:1;
        /** thres0_low_int_st : RO; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t thres0_low_int_st:1;
        /** thres1_high_int_st : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t thres1_high_int_st:1;
        /** thres0_high_int_st : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t thres0_high_int_st:1;
        /** sar2_done_int_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t sar2_done_int_st:1;
        /** sar1_done_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t sar1_done_int_st:1;
    };
    uint32_t val;
} adc_int_st_reg_t;

/** Type of int_clr register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** fifo_overflow_int_clr : WT; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t fifo_overflow_int_clr:1;
        /** thres1_low_int_clr : WT; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t thres1_low_int_clr:1;
        /** thres0_low_int_clr : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t thres0_low_int_clr:1;
        /** thres1_high_int_clr : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t thres1_high_int_clr:1;
        /** thres0_high_int_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t thres0_high_int_clr:1;
        /** sar2_done_int_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t sar2_done_int_clr:1;
        /** sar1_done_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t sar1_done_int_clr:1;
    };
    uint32_t val;
} adc_int_clr_reg_t;

/** Type of dma_conf register
 *  Register
 */
typedef union {
    struct {
        /** apb_adc_eof_num : R/W; bitpos: [15:0]; default: 255;
         *  the dma_in_suc_eof gen when sample cnt = spi_eof_num
         */
        uint32_t apb_adc_eof_num:16;
        /** dma_eof_num_clr : WT; bitpos: [16]; default: 0;
         *  clear eof cnt
         */
        uint32_t dma_eof_num_clr:1;
        uint32_t reserved_17:14;
        /** apb_adc_trans : R/W; bitpos: [31]; default: 0;
         *  enable apb_adc use spi_dma
         */
        uint32_t apb_adc_trans:1;
    };
    uint32_t val;
} adc_dma_conf_reg_t;

/** Type of ref_control register
 *  Register
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** rtc_ref_delay : R/W; bitpos: [29]; default: 0;
         *  control the refgen short vdd
         */
        uint32_t rtc_ref_delay:1;
        /** rtc_pre_charge : R/W; bitpos: [30]; default: 0;
         *  control the refgen module pre charge
         */
        uint32_t rtc_pre_charge:1;
        /** rtc_xpd_refgen : R/W; bitpos: [31]; default: 0;
         *  control the refgen module power up
         */
        uint32_t rtc_xpd_refgen:1;
    };
    uint32_t val;
} adc_ref_control_reg_t;

/** Type of rnd_eco_low register
 *  Register
 */
typedef union {
    struct {
        /** adc_rnd_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  rnd eco low
         */
        uint32_t adc_rnd_eco_low:32;
    };
    uint32_t val;
} adc_rnd_eco_low_reg_t;

/** Type of rnd_eco_high register
 *  Register
 */
typedef union {
    struct {
        /** adc_rnd_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  rnd eco high
         */
        uint32_t adc_rnd_eco_high:32;
    };
    uint32_t val;
} adc_rnd_eco_high_reg_t;

/** Type of rnd_eco_cs register
 *  Register
 */
typedef union {
    struct {
        /** rnd_eco_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t rnd_eco_en:1;
        /** rnd_eco_result : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t rnd_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} adc_rnd_eco_cs_reg_t;

/** Type of ctrl_date register
 *  Register
 */
typedef union {
    struct {
        /** ctrl_date : R/W; bitpos: [30:0]; default: 38830624;
         *  need_des
         */
        uint32_t ctrl_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} adc_ctrl_date_reg_t;


typedef struct {
    volatile adc_ctrl0_reg_t ctrl0;
    volatile adc_ctrl1_reg_t ctrl1;
    volatile adc_ctrl2_reg_t ctrl2;
    volatile adc_filter_ctrl1_reg_t filter_ctrl1;
    uint32_t reserved_010[3];
    volatile adc_sar1_patt_tab1_reg_t sar1_patt_tab1;
    volatile adc_sar1_patt_tab2_reg_t sar1_patt_tab2;
    volatile adc_sar1_patt_tab3_reg_t sar1_patt_tab3;
    volatile adc_sar1_patt_tab4_reg_t sar1_patt_tab4;
    volatile adc_sar2_patt_tab1_reg_t sar2_patt_tab1;
    volatile adc_sar2_patt_tab2_reg_t sar2_patt_tab2;
    volatile adc_sar2_patt_tab3_reg_t sar2_patt_tab3;
    volatile adc_sar2_patt_tab4_reg_t sar2_patt_tab4;
    volatile adc_filter_ctrl0_reg_t filter_ctrl0;
    volatile adc_sar1_data_status_reg_t sar1_data_status;
    volatile adc_sar2_data_status_reg_t sar2_data_status;
    volatile adc_thres0_high_thres_reg_t thres0_high_thres;
    volatile adc_thres0_low_thres_reg_t thres0_low_thres;
    volatile adc_thres0_ctrl_reg_t thres0_ctrl;
    volatile adc_thres1_high_thres_reg_t thres1_high_thres;
    volatile adc_thres1_low_thres_reg_t thres1_low_thres;
    volatile adc_thres1_ctrl_reg_t thres1_ctrl;
    volatile adc_thres_ctrl_reg_t thres_ctrl;
    volatile adc_int_ena_reg_t int_ena;
    volatile adc_int_raw_reg_t int_raw;
    volatile adc_int_st_reg_t int_st;
    volatile adc_int_clr_reg_t int_clr;
    volatile adc_dma_conf_reg_t dma_conf;
    volatile adc_ref_control_reg_t ref_control;
    volatile adc_rnd_eco_low_reg_t rnd_eco_low;
    volatile adc_rnd_eco_high_reg_t rnd_eco_high;
    volatile adc_rnd_eco_cs_reg_t rnd_eco_cs;
    uint32_t reserved_088[221];
    volatile adc_ctrl_date_reg_t ctrl_date;
} adc_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(adc_dev_t) == 0x400, "Invalid size of adc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
