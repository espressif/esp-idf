/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of pad_cfg register
 *  configure dac pad register
 */
typedef union {
    struct {
        /** hs_mode_pad_0 : R/W; bitpos: [0]; default: 0;
         *  1 to let DAC PAD 0 enter High speed mode
         */
        uint32_t hs_mode_pad_0:1;
        /** xpd_pad_0 : R/W; bitpos: [1]; default: 0;
         *  1 to power on DAC PAD 0
         */
        uint32_t xpd_pad_0:1;
        /** en_adc_mux_pad_0 : R/W; bitpos: [2]; default: 0;
         *  1 to enable adc mux for DAC PAD 0
         */
        uint32_t en_adc_mux_pad_0:1;
        /** dsamp_ls_pad_0 : R/W; bitpos: [3]; default: 0;
         *  1 to start sample and hold mode for DAC PAD 0
         */
        uint32_t dsamp_ls_pad_0:1;
        /** dcal_ls_pad_0 : R/W; bitpos: [4]; default: 0;
         *  1 to Calibrate the buffer of DAC PAD 0 (DBUF of pad 1 must be 1 during this
         *  operation)
         */
        uint32_t dcal_ls_pad_0:1;
        /** dbuf_ls_pad_0 : R/W; bitpos: [5]; default: 0;
         *  1 to turn on the buffer of DAC PAD 0
         */
        uint32_t dbuf_ls_pad_0:1;
        /** on_ls_pad_0 : R/W; bitpos: [6]; default: 0;
         *  1 to turn on the DAC PAD 0 during sample and hold mode
         */
        uint32_t on_ls_pad_0:1;
        /** hs_mode_pad_1 : R/W; bitpos: [7]; default: 0;
         *  1 to let DAC PAD 1 enter High speed mode
         */
        uint32_t hs_mode_pad_1:1;
        /** xpd_pad_1 : R/W; bitpos: [8]; default: 0;
         *  1 to power on DAC PAD 1
         */
        uint32_t xpd_pad_1:1;
        /** en_adc_mux_pad_1 : R/W; bitpos: [9]; default: 0;
         *  1 to enable adc mux for DAC PAD 1
         */
        uint32_t en_adc_mux_pad_1:1;
        /** dsamp_ls_pad_1 : R/W; bitpos: [10]; default: 0;
         *  1 to start sample and hold mode for DAC PAD 1
         */
        uint32_t dsamp_ls_pad_1:1;
        /** dcal_ls_pad_1 : R/W; bitpos: [11]; default: 0;
         *  1 to Calibrate the buffer of DAC PAD 1 (DBUF of pad 1 must be 1 during this
         *  operation)
         */
        uint32_t dcal_ls_pad_1:1;
        /** dbuf_ls_pad_1 : R/W; bitpos: [12]; default: 0;
         *  1 to turn on the buffer of DAC PAD 1
         */
        uint32_t dbuf_ls_pad_1:1;
        /** on_ls_pad_1 : R/W; bitpos: [13]; default: 0;
         *  1 to turn on the DAC PAD 1 during sample and hold mode
         */
        uint32_t on_ls_pad_1:1;
        /** div_num : R/W; bitpos: [29:14]; default: 1;
         *  divide number between clk ls and clk hs
         */
        uint32_t div_num:16;
        /** dbuf_range_ls_pad_0 : R/W; bitpos: [30]; default: 0;
         *  1:when buffer is activated, allow using full code of maximum bitwidth
         *  0:when buffer is not activated, should always be 0
         */
        uint32_t dbuf_range_ls_pad_0:1;
        /** dbuf_range_ls_pad_1 : R/W; bitpos: [31]; default: 0;
         *  1:when buffer is activated, allow using full code of maximum bitwidth
         *  0:when buffer is not activated, should always be 0
         */
        uint32_t dbuf_range_ls_pad_1:1;
    };
    uint32_t val;
} dac_pad_cfg_reg_t;

/** Type of cali_result register
 *  DAC CALI result register
 */
typedef union {
    struct {
        /** cali_result_pad_0 : R/W; bitpos: [11:0]; default: 0;
         *  cali result register for pad 0 output
         */
        uint32_t cali_result_pad_0:12;
        /** cali_result_pad_1 : R/W; bitpos: [23:12]; default: 0;
         *  cali result register for pad 1 output
         */
        uint32_t cali_result_pad_1:12;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dac_cali_result_reg_t;

/** Type of pdma_cfg register
 *  dac cfg register
 */
typedef union {
    struct {
        /** pdma_alter_mode : R/W; bitpos: [0]; default: 0;
         *  1:make rotation between two pads when outputting data of ppdma input path requires
         *  reg_dac_1/0_data_sel = 1
         *  0:no change
         */
        uint32_t pdma_alter_mode:1;
        /** pdma_reset_fifo : WT; bitpos: [1]; default: 0;
         *  1:reset dac fifo
         *  0:no change
         */
        uint32_t pdma_reset_fifo:1;
        /** pdma_trans : R/W; bitpos: [2]; default: 0;
         *  1:start transaction for pdma path output
         *  0:stop transaction for pdma path output
         */
        uint32_t pdma_trans:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} dac_pdma_cfg_reg_t;

/** Type of pdma_timer_cfg register
 *  PDMA path timer register
 */
typedef union {
    struct {
        /** pdma_timer_en : R/W; bitpos: [0]; default: 0;
         *  1:enables using dac timer to reduce output frequency
         *  0:no data output
         */
        uint32_t pdma_timer_en:1;
        /** pdma_timer_target : R/W; bitpos: [24:1]; default: 2;
         *  dac pdma timer wait target
         */
        uint32_t pdma_timer_target:24;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} dac_pdma_timer_cfg_reg_t;

/** Type of data_output_cfg register
 *  dac DATA OUTPUT cfg register
 */
typedef union {
    struct {
        /** pad_0_data_sel : R/W; bitpos: [0]; default: 0;
         *  1:output pdma data
         *  0:output sintx data
         */
        uint32_t pad_0_data_sel:1;
        /** pad_1_data_sel : R/W; bitpos: [1]; default: 0;
         *  1:output pdma data
         *  0:output sintx data
         */
        uint32_t pad_1_data_sel:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dac_data_output_cfg_reg_t;

/** Type of sintx_cfg register
 *  dac rstn register
 */
typedef union {
    struct {
        /** sw_tone : R/W; bitpos: [0]; default: 0;
         *  1:enable software adjusting current sintx output data
         *  0:disable software adjustment
         */
        uint32_t sw_tone:1;
        /** sw_fstep : R/W; bitpos: [16:1]; default: 1;
         *  software adjust angle velocity foot step for sintx LUT input
         */
        uint32_t sw_fstep:16;
        /** scale_1 : R/W; bitpos: [18:17]; default: 0;
         *  right shift scaling config for LUT output of sintx for pad 0
         */
        uint32_t scale_1:2;
        /** scale_2 : R/W; bitpos: [20:19]; default: 0;
         *  software adjust angle velocity foot step for sintx LUT input
         */
        uint32_t scale_2:2;
        /** inv_1 : R/W; bitpos: [22:21]; default: 0;
         *  bit 1: invert the highest bit of output for pad 0
         *  Bit 0: invert the entire output for pad 0
         */
        uint32_t inv_1:2;
        /** inv_2 : R/W; bitpos: [24:23]; default: 0;
         *  bit 1: invert the highest bit of output for pad 1
         *  Bit 0: invert the entire output for pad 1
         */
        uint32_t inv_2:2;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} dac_sintx_cfg_reg_t;

/** Type of sintx_timer_cfg register
 *  SINTX path timer register
 */
typedef union {
    struct {
        /** sintx_timer_en : R/W; bitpos: [0]; default: 0;
         *  1:enables using dac timer to reduce output frequency
         *  0:no data output
         */
        uint32_t sintx_timer_en:1;
        /** sintx_timer_target : R/W; bitpos: [24:1]; default: 2;
         *  dac sintx timer wait target
         */
        uint32_t sintx_timer_target:24;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} dac_sintx_timer_cfg_reg_t;

/** Type of sintx_data register
 *  dac output register for sintx path
 */
typedef union {
    struct {
        /** dc_1 : R/W; bitpos: [11:0]; default: 0;
         *  the data output for DAC PAD0 for sintx path
         */
        uint32_t dc_1:12;
        /** dc_2 : R/W; bitpos: [23:12]; default: 0;
         *  the data output for DAC PAD1 for sintx path
         */
        uint32_t dc_2:12;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dac_sintx_data_reg_t;

/** Type of cali register
 *  cali algorithm register for DAC
 */
typedef union {
    struct {
        /** cali_out_pad_0 : RO; bitpos: [0]; default: 0;
         *  cali out from DAC PAD 0
         */
        uint32_t cali_out_pad_0:1;
        /** cali_out_pad_1 : RO; bitpos: [1]; default: 0;
         *  cali out from DAC PAD 1
         */
        uint32_t cali_out_pad_1:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dac_cali_reg_t;

/** Type of sample_wait_cfg register
 *  cali sample phase duration register
 */
typedef union {
    struct {
        /** wait_target_sample_pad_0 : R/W; bitpos: [15:0]; default: 630;
         *  sample wait target for DAC PAD 1
         */
        uint32_t wait_target_sample_pad_0:16;
        /** wait_target_sample_pad_1 : R/W; bitpos: [31:16]; default: 630;
         *  sample wait target for DAC PAD 0
         */
        uint32_t wait_target_sample_pad_1:16;
    };
    uint32_t val;
} dac_sample_wait_cfg_reg_t;

/** Type of hold_wait_cfg register
 *  cali hold phase duration register
 */
typedef union {
    struct {
        /** wait_target_hold_pad_0 : R/W; bitpos: [15:0]; default: 61;
         *  hold wait target for DAC PAD 0
         */
        uint32_t wait_target_hold_pad_0:16;
        /** wait_target_hold_pad_1 : R/W; bitpos: [31:16]; default: 61;
         *  hold wait target for DAC PAD 1
         */
        uint32_t wait_target_hold_pad_1:16;
    };
    uint32_t val;
} dac_hold_wait_cfg_reg_t;

/** Type of refresh_wait_cfg register
 *  cali refresh phase duration register
 */
typedef union {
    struct {
        /** wait_target_refresh_pad_0 : R/W; bitpos: [15:0]; default: 8;
         *  refresh wait target for DAC PAD 0
         */
        uint32_t wait_target_refresh_pad_0:16;
        /** wait_target_refresh_pad_1 : R/W; bitpos: [31:16]; default: 8;
         *  refresh wait target for DAC PAD 1
         */
        uint32_t wait_target_refresh_pad_1:16;
    };
    uint32_t val;
} dac_refresh_wait_cfg_reg_t;

/** Type of pdma_int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** pdma_empty_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t pdma_empty_int_raw:1;
    };
    uint32_t val;
} dac_pdma_int_raw_reg_t;

/** Type of int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** pdma_empty_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t pdma_empty_int_st:1;
    };
    uint32_t val;
} dac_int_st_reg_t;

/** Type of int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** pdma_empty_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t pdma_empty_int_ena:1;
    };
    uint32_t val;
} dac_int_ena_reg_t;

/** Type of int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** pdma_empty_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t pdma_empty_int_clr:1;
    };
    uint32_t val;
} dac_int_clr_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 2426393;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} dac_date_reg_t;


typedef struct {
    volatile dac_pad_cfg_reg_t pad_cfg;
    volatile dac_cali_result_reg_t cali_result;
    volatile dac_pdma_cfg_reg_t pdma_cfg;
    volatile dac_pdma_timer_cfg_reg_t pdma_timer_cfg;
    volatile dac_data_output_cfg_reg_t data_output_cfg;
    volatile dac_sintx_cfg_reg_t sintx_cfg;
    volatile dac_sintx_timer_cfg_reg_t sintx_timer_cfg;
    volatile dac_sintx_data_reg_t sintx_data;
    volatile dac_cali_reg_t cali;
    volatile dac_sample_wait_cfg_reg_t sample_wait_cfg;
    volatile dac_hold_wait_cfg_reg_t hold_wait_cfg;
    volatile dac_refresh_wait_cfg_reg_t refresh_wait_cfg;
    volatile dac_pdma_int_raw_reg_t pdma_int_raw;
    volatile dac_int_st_reg_t int_st;
    volatile dac_int_ena_reg_t int_ena;
    volatile dac_int_clr_reg_t int_clr;
    uint32_t reserved_040[239];
    volatile dac_date_reg_t date;
} dac_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(dac_dev_t) == 0x400, "Invalid size of dac_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
