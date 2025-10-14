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

/** Group: Configuration Register */
/** Type of det_conf register
 *  zero det cfg reg
 */
typedef union {
    struct {
        /** det_vref_channel_sel : R/W; bitpos: [3:0]; default: 8;
         *  vref pad sel,one hot code and cannot set the same bit with other channels
         */
        uint32_t det_vref_channel_sel:4;
        /** det_comp_channel_3_sel : R/W; bitpos: [7:4]; default: 4;
         *  Channel 3 comp pad sel,one hot code and cannot set the same bit with other channels
         */
        uint32_t det_comp_channel_3_sel:4;
        /** det_comp_channel_2_sel : R/W; bitpos: [11:8]; default: 2;
         *  Channel 2 comp pad sel,one hot code and cannot set the same bit with other channels
         */
        uint32_t det_comp_channel_2_sel:4;
        /** det_comp_channel_1_sel : R/W; bitpos: [15:12]; default: 1;
         *  Channel 1 comp pad sel,one hot code and cannot set the same bit with other channels
         */
        uint32_t det_comp_channel_1_sel:4;
        /** det_channel_3_event_timer_en : R/W; bitpos: [16]; default: 0;
         *  enable channel 3 event timer to trigger  channel 1 event  after
         *  pad_comp_channel_3_int
         */
        uint32_t det_channel_3_event_timer_en:1;
        /** det_channel_2_event_timer_en : R/W; bitpos: [17]; default: 0;
         *  enable channel 2 event timer to trigger  channel 1 event  after
         *  pad_comp_channel_2_int
         */
        uint32_t det_channel_2_event_timer_en:1;
        /** det_channel_1_event_timer_en : R/W; bitpos: [18]; default: 0;
         *  enable channel 1 event timer to trigger  channel 1 event  after
         *  pad_comp_channel_1_int
         */
        uint32_t det_channel_1_event_timer_en:1;
        /** det_channel_timer_en : R/W; bitpos: [19]; default: 0;
         *  enable timer to record the time between two continuous zero det int in each channel
         */
        uint32_t det_channel_timer_en:1;
        /** det_limit_cnt : R/W; bitpos: [27:20]; default: 5;
         *  cfg  continuous zero det num to change zero det result
         */
        uint32_t det_limit_cnt:8;
        /** det_comp_poll_mask : R/W; bitpos: [30:28]; default: 0;
         *  mask channel to do pad compare and zero det
         */
        uint32_t det_comp_poll_mask:3;
        /** det_comp_poll_mode : R/W; bitpos: [31]; default: 0;
         *  cfg channel scan mode ,0 means one trigger scan all mask channel, 1 means one
         *  trigger scan one mask channel
         */
        uint32_t det_comp_poll_mode:1;
    };
    uint32_t val;
} zero_det_conf_reg_t;

/** Type of det_filter_cnt register
 *  protect time reg
 */
typedef union {
    struct {
        /** det_filter_cnt : R/W; bitpos: [31:0]; default: 255;
         *  protect time after det the first zero det int
         */
        uint32_t det_filter_cnt:32;
    };
    uint32_t val;
} zero_det_filter_cnt_reg_t;

/** Type of det_poll_period register
 *  poll period time reg
 */
typedef union {
    struct {
        /** det_comp_poll_period : R/W; bitpos: [15:0]; default: 15;
         *  poll period time for each channel
         */
        uint32_t det_comp_poll_period:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} zero_det_poll_period_reg_t;

/** Type of det_delay_event_time register
 *  delay time reg
 */
typedef union {
    struct {
        /** det_delay_event_time : R/W; bitpos: [15:0]; default: 15;
         *  delay time after zero det int to trigger event  for each channel
         */
        uint32_t det_delay_event_time:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} zero_det_delay_event_time_reg_t;

/** Type of det_int_ena register
 *  zero det int ena
 */
typedef union {
    struct {
        /** det_pad_comp_channel_3_neg_int_ena : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_neg_int_ena:1;
        /** det_pad_comp_channel_3_pos_int_ena : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_pos_int_ena:1;
        /** det_pad_comp_channel_3_int_ena : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_int_ena:1;
        /** det_pad_comp_channel_2_neg_int_ena : R/W; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_neg_int_ena:1;
        /** det_pad_comp_channel_2_pos_int_ena : R/W; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_pos_int_ena:1;
        /** det_pad_comp_channel_2_int_ena : R/W; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_int_ena:1;
        /** det_pad_comp_channel_1_neg_int_ena : R/W; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_neg_int_ena:1;
        /** det_pad_comp_channel_1_pos_int_ena : R/W; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_pos_int_ena:1;
        /** det_pad_comp_channel_1_int_ena : R/W; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_int_ena:1;
        /** det_pad_comp_neg_int_ena : R/W; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_neg_int_ena:1;
        /** det_pad_comp_pos_int_ena : R/W; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_pos_int_ena:1;
        /** det_pad_comp_int_ena : R/W; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_int_ena:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} zero_det_int_ena_reg_t;

/** Type of det_int_raw register
 *  zero det int raw
 */
typedef union {
    struct {
        /** det_pad_comp_channel_3_neg_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_neg_int_raw:1;
        /** det_pad_comp_channel_3_pos_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_pos_int_raw:1;
        /** det_pad_comp_channel_3_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_int_raw:1;
        /** det_pad_comp_channel_2_neg_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_neg_int_raw:1;
        /** det_pad_comp_channel_2_pos_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_pos_int_raw:1;
        /** det_pad_comp_channel_2_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_int_raw:1;
        /** det_pad_comp_channel_1_neg_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_neg_int_raw:1;
        /** det_pad_comp_channel_1_pos_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_pos_int_raw:1;
        /** det_pad_comp_channel_1_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_int_raw:1;
        /** det_pad_comp_neg_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_neg_int_raw:1;
        /** det_pad_comp_pos_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_pos_int_raw:1;
        /** det_pad_comp_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_int_raw:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} zero_det_int_raw_reg_t;

/** Type of det_int_clr register
 *  zero det int clr
 */
typedef union {
    struct {
        /** det_pad_comp_channel_3_neg_int_clr : WT; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_neg_int_clr:1;
        /** det_pad_comp_channel_3_pos_int_clr : WT; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_pos_int_clr:1;
        /** det_pad_comp_channel_3_int_clr : WT; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_int_clr:1;
        /** det_pad_comp_channel_2_neg_int_clr : WT; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_neg_int_clr:1;
        /** det_pad_comp_channel_2_pos_int_clr : WT; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_pos_int_clr:1;
        /** det_pad_comp_channel_2_int_clr : WT; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_int_clr:1;
        /** det_pad_comp_channel_1_neg_int_clr : WT; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_neg_int_clr:1;
        /** det_pad_comp_channel_1_pos_int_clr : WT; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_pos_int_clr:1;
        /** det_pad_comp_channel_1_int_clr : WT; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_int_clr:1;
        /** det_pad_comp_neg_int_clr : WT; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_neg_int_clr:1;
        /** det_pad_comp_pos_int_clr : WT; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_pos_int_clr:1;
        /** det_pad_comp_int_clr : WT; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_int_clr:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} zero_det_int_clr_reg_t;

/** Type of det_int_st register
 *  zero det int st
 */
typedef union {
    struct {
        /** det_pad_comp_channel_3_neg_int_st : RO; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_neg_int_st:1;
        /** det_pad_comp_channel_3_pos_int_st : RO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_pos_int_st:1;
        /** det_pad_comp_channel_3_int_st : RO; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_3_int_st:1;
        /** det_pad_comp_channel_2_neg_int_st : RO; bitpos: [3]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_neg_int_st:1;
        /** det_pad_comp_channel_2_pos_int_st : RO; bitpos: [4]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_pos_int_st:1;
        /** det_pad_comp_channel_2_int_st : RO; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_2_int_st:1;
        /** det_pad_comp_channel_1_neg_int_st : RO; bitpos: [6]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_neg_int_st:1;
        /** det_pad_comp_channel_1_pos_int_st : RO; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_pos_int_st:1;
        /** det_pad_comp_channel_1_int_st : RO; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_channel_1_int_st:1;
        /** det_pad_comp_neg_int_st : RO; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_neg_int_st:1;
        /** det_pad_comp_pos_int_st : RO; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_pos_int_st:1;
        /** det_pad_comp_int_st : RO; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t det_pad_comp_int_st:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} zero_det_int_st_reg_t;

/** Type of det_channel_1_timer0 register
 *  record timer reg
 */
typedef union {
    struct {
        /** det_channel_1_timer0 : RO; bitpos: [31:0]; default: 0;
         *  record the time while detect the first zero det int  in channel 1
         */
        uint32_t det_channel_1_timer0:32;
    };
    uint32_t val;
} zero_det_channel_1_timer0_reg_t;

/** Type of det_channel_1_timer1 register
 *  record timer reg
 */
typedef union {
    struct {
        /** det_channel_1_timer1 : RO; bitpos: [31:0]; default: 0;
         *  record the time while detect the second zero det int  in channel 1
         */
        uint32_t det_channel_1_timer1:32;
    };
    uint32_t val;
} zero_det_channel_1_timer1_reg_t;

/** Type of det_channel_2_timer0 register
 *  record timer reg
 */
typedef union {
    struct {
        /** det_channel_2_timer0 : RO; bitpos: [31:0]; default: 0;
         *  record the time while detect the first zero det int  in channel 2
         */
        uint32_t det_channel_2_timer0:32;
    };
    uint32_t val;
} zero_det_channel_2_timer0_reg_t;

/** Type of det_channel_2_timer1 register
 *  record timer reg
 */
typedef union {
    struct {
        /** det_channel_2_timer1 : RO; bitpos: [31:0]; default: 0;
         *  record the time while detect the second zero det int  in channel 2
         */
        uint32_t det_channel_2_timer1:32;
    };
    uint32_t val;
} zero_det_channel_2_timer1_reg_t;

/** Type of det_channel_3_timer0 register
 *  record timer reg
 */
typedef union {
    struct {
        /** det_channel_3_timer0 : RO; bitpos: [31:0]; default: 0;
         *  record the time while detect the first zero det int  in channel 3
         */
        uint32_t det_channel_3_timer0:32;
    };
    uint32_t val;
} zero_det_channel_3_timer0_reg_t;

/** Type of det_channel_3_timer1 register
 *  record timer reg
 */
typedef union {
    struct {
        /** det_channel_3_timer1 : RO; bitpos: [31:0]; default: 0;
         *  record the time while detect the second zero det int  in channel 3
         */
        uint32_t det_channel_3_timer1:32;
    };
    uint32_t val;
} zero_det_channel_3_timer1_reg_t;

/** Type of det_channel_status register
 *  pad comp status reg
 */
typedef union {
    struct {
        /** det_channel_3_pad_comp_status : RO; bitpos: [0]; default: 0;
         *  record the pad comp status for channel 3, 0 means now is neg int , 1 means now is
         *  pos int
         */
        uint32_t det_channel_3_pad_comp_status:1;
        /** det_channel_2_pad_comp_status : RO; bitpos: [1]; default: 0;
         *  record the pad comp status for channel 2, 0 means now is neg int , 1 means now is
         *  pos int
         */
        uint32_t det_channel_2_pad_comp_status:1;
        /** det_channel_1_pad_comp_status : RO; bitpos: [2]; default: 0;
         *  record the pad comp status for channel 1, 0 means now is neg int , 1 means now is
         *  pos int
         */
        uint32_t det_channel_1_pad_comp_status:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} zero_det_channel_status_reg_t;

/** Type of det_pad_comp_cfg register
 *  pad comp cfg reg
 */
typedef union {
    struct {
        /** det_pad_comp_hys : R/W; bitpos: [2:0]; default: 0;
         *  hys cfg signal
         */
        uint32_t det_pad_comp_hys:3;
        /** det_pad_comp_hys_en : R/W; bitpos: [3]; default: 0;
         *  enable hys function,only works while pad comp mode = 0
         */
        uint32_t det_pad_comp_hys_en:1;
        /** det_pad_comp_dref : R/W; bitpos: [6:4]; default: 0;
         *  internal vref cfg signal,0~2.3v step is 330mv
         */
        uint32_t det_pad_comp_dref:3;
        /** det_pad_comp_mode : R/W; bitpos: [7]; default: 0;
         *  pad comp mode cfg 1:external pad 0:internal vref
         */
        uint32_t det_pad_comp_mode:1;
        /** det_pad_comp_xpd : R/W; bitpos: [8]; default: 0;
         *  pad comp xpd
         */
        uint32_t det_pad_comp_xpd:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} zero_det_pad_comp_cfg_reg_t;


/** Group: Version Register */
/** Type of det_date register
 *  date reg
 */
typedef union {
    struct {
        /** det_date : R/W; bitpos: [27:0]; default: 37773696;
         *  zero det reg change date
         */
        uint32_t det_date:28;
        /** det_clk_en : R/W; bitpos: [28]; default: 0;
         *  reg clk en
         */
        uint32_t det_clk_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} zero_det_date_reg_t;


typedef struct {
    volatile zero_det_conf_reg_t det_conf;
    volatile zero_det_filter_cnt_reg_t det_filter_cnt;
    volatile zero_det_poll_period_reg_t det_poll_period;
    volatile zero_det_delay_event_time_reg_t det_delay_event_time;
    volatile zero_det_int_ena_reg_t det_int_ena;
    volatile zero_det_int_raw_reg_t det_int_raw;
    volatile zero_det_int_clr_reg_t det_int_clr;
    volatile zero_det_int_st_reg_t det_int_st;
    volatile zero_det_channel_1_timer0_reg_t det_channel_1_timer0;
    volatile zero_det_channel_1_timer1_reg_t det_channel_1_timer1;
    volatile zero_det_channel_2_timer0_reg_t det_channel_2_timer0;
    volatile zero_det_channel_2_timer1_reg_t det_channel_2_timer1;
    volatile zero_det_channel_3_timer0_reg_t det_channel_3_timer0;
    volatile zero_det_channel_3_timer1_reg_t det_channel_3_timer1;
    volatile zero_det_channel_status_reg_t det_channel_status;
    volatile zero_det_pad_comp_cfg_reg_t det_pad_comp_cfg;
    volatile zero_det_date_reg_t det_date;
} zero_dev_t;

extern zero_dev_t ZERO_DET;

#ifndef __cplusplus
_Static_assert(sizeof(zero_dev_t) == 0x44, "Invalid size of zero_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
