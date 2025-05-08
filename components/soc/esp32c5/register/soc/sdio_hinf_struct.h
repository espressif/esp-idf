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

/** Group: Configuration registers */
/** Type of cfg_data0 register
 *  Configure sdio cis content
 */
typedef union {
    struct {
        /** device_id_fn1 : R/W; bitpos: [15:0]; default: 26214;
         *  configure device id of function1 in cis
         */
        uint32_t device_id_fn1:16;
        /** user_id_fn1 : R/W; bitpos: [31:16]; default: 146;
         *  configure user id of function1 in cis
         */
        uint32_t user_id_fn1:16;
    };
    uint32_t val;
} hinf_cfg_data0_reg_t;

/** Type of cfg_data1 register
 *  SDIO configuration register
 */
typedef union {
    struct {
        /** sdio_enable : R/W; bitpos: [0]; default: 1;
         *  Sdio clock enable
         */
        uint32_t sdio_enable:1;
        /** sdio_ioready1 : R/W; bitpos: [1]; default: 0;
         *  sdio function1 io ready signal in cis
         */
        uint32_t sdio_ioready1:1;
        /** highspeed_enable : R/W; bitpos: [2]; default: 0;
         *  Highspeed enable in cccr
         */
        uint32_t highspeed_enable:1;
        /** highspeed_mode : RO; bitpos: [3]; default: 0;
         *  highspeed mode status in cccr
         */
        uint32_t highspeed_mode:1;
        /** sdio_cd_enable : R/W; bitpos: [4]; default: 1;
         *  sdio card detect enable
         */
        uint32_t sdio_cd_enable:1;
        /** sdio_ioready2 : R/W; bitpos: [5]; default: 0;
         *  sdio function1 io ready signal in cis
         */
        uint32_t sdio_ioready2:1;
        /** sdio_int_mask : R/W; bitpos: [6]; default: 0;
         *  mask sdio interrupt in cccr, high active
         */
        uint32_t sdio_int_mask:1;
        /** ioenable2 : RO; bitpos: [7]; default: 0;
         *  ioe2 status in cccr
         */
        uint32_t ioenable2:1;
        /** cd_disable : RO; bitpos: [8]; default: 0;
         *  card disable status in cccr
         */
        uint32_t cd_disable:1;
        /** func1_eps : RO; bitpos: [9]; default: 0;
         *  function1 eps status in fbr
         */
        uint32_t func1_eps:1;
        /** emp : RO; bitpos: [10]; default: 0;
         *  empc status in cccr
         */
        uint32_t emp:1;
        /** ioenable1 : RO; bitpos: [11]; default: 0;
         *  ioe1 status in cccr
         */
        uint32_t ioenable1:1;
        /** sdio_ver : R/W; bitpos: [23:12]; default: 562;
         *  sdio version in cccr
         */
        uint32_t sdio_ver:12;
        /** func2_eps : RO; bitpos: [24]; default: 0;
         *  function2 eps status in fbr
         */
        uint32_t func2_eps:1;
        /** sdio20_conf : R/W; bitpos: [31:25]; default: 0;
         *  [29],sdio negedge sample enablel.[30],sdio posedge sample enable.[31],sdio cmd/dat
         *  in delayed cycles control,0:no delay, 1:delay 1 cycle.
         *  [25]: sdio1.1 dat/cmd sending out edge control,1:negedge,0:posedge when highseed
         *  mode.
         *  [26]: sdio2.0 dat/cmd sending out edge control,1:negedge when [12]=0,0:negedge when
         *  [12]=0,posedge when highspeed mode enable.
         *  [27]: sdio interrupt sending out delay control,1:delay one cycle, 0: no delay.
         *  [28]: sdio data pad pull up enable
         */
        uint32_t sdio20_conf:7;
    };
    uint32_t val;
} hinf_cfg_data1_reg_t;

/** Type of cfg_timing register
 *  Timing configuration registers
 */
typedef union {
    struct {
        /** ncrc : R/W; bitpos: [2:0]; default: 2;
         *  configure Ncrc parameter in sdr50/104 mode, no more than 6.
         */
        uint32_t ncrc:3;
        /** pst_end_cmd_low_value : R/W; bitpos: [9:3]; default: 2;
         *  configure cycles to lower cmd after voltage is changed to 1.8V.
         */
        uint32_t pst_end_cmd_low_value:7;
        /** pst_end_data_low_value : R/W; bitpos: [15:10]; default: 2;
         *  configure cycles to lower data after voltage is changed to 1.8V.
         */
        uint32_t pst_end_data_low_value:6;
        /** sdclk_stop_thres : R/W; bitpos: [26:16]; default: 1400;
         *  Configure the number of cycles of module clk to judge sdclk has stopped
         */
        uint32_t sdclk_stop_thres:11;
        uint32_t reserved_27:1;
        /** sample_clk_divider : R/W; bitpos: [31:28]; default: 1;
         *  module clk divider to sample sdclk
         */
        uint32_t sample_clk_divider:4;
    };
    uint32_t val;
} hinf_cfg_timing_reg_t;

/** Type of cfg_update register
 *  update sdio configurations
 */
typedef union {
    struct {
        /** conf_update : WT; bitpos: [0]; default: 0;
         *  update the timing configurations
         */
        uint32_t conf_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hinf_cfg_update_reg_t;

/** Type of cfg_data7 register
 *  SDIO configuration register
 */
typedef union {
    struct {
        /** pin_state : R/W; bitpos: [7:0]; default: 0;
         *  configure cis addr 318 and 574
         */
        uint32_t pin_state:8;
        /** chip_state : R/W; bitpos: [15:8]; default: 0;
         *  configure cis addr 312, 315, 568 and 571
         */
        uint32_t chip_state:8;
        /** sdio_rst : R/W; bitpos: [16]; default: 0;
         *  soft reset control for sdio module
         */
        uint32_t sdio_rst:1;
        /** sdio_ioready0 : R/W; bitpos: [17]; default: 1;
         *  sdio io ready, high enable
         */
        uint32_t sdio_ioready0:1;
        /** sdio_mem_pd : R/W; bitpos: [18]; default: 0;
         *  sdio memory power down, high active
         */
        uint32_t sdio_mem_pd:1;
        /** esdio_data1_int_en : R/W; bitpos: [19]; default: 0;
         *  enable sdio interrupt on data1 line
         */
        uint32_t esdio_data1_int_en:1;
        /** sdio_switch_volt_sw : R/W; bitpos: [20]; default: 0;
         *  control switch voltage change to 1.8V by software. 0:3.3V,1:1.8V
         */
        uint32_t sdio_switch_volt_sw:1;
        /** ddr50_blk_len_fix_en : R/W; bitpos: [21]; default: 0;
         *  enable block length to be fixed to 512 bytes in ddr50 mode
         */
        uint32_t ddr50_blk_len_fix_en:1;
        /** clk_en : R/W; bitpos: [22]; default: 0;
         *  sdio apb clock for configuration force on control:0-gating,1-force on.
         */
        uint32_t clk_en:1;
        /** sddr50 : R/W; bitpos: [23]; default: 1;
         *  configure if support sdr50 mode in cccr
         */
        uint32_t sddr50:1;
        /** ssdr104 : R/W; bitpos: [24]; default: 1;
         *  configure if support sdr104 mode in cccr
         */
        uint32_t ssdr104:1;
        /** ssdr50 : R/W; bitpos: [25]; default: 1;
         *  configure if support ddr50 mode in cccr
         */
        uint32_t ssdr50:1;
        /** sdtd : R/W; bitpos: [26]; default: 0;
         *  configure if support driver type D in cccr
         */
        uint32_t sdtd:1;
        /** sdta : R/W; bitpos: [27]; default: 0;
         *  configure if support driver type A in cccr
         */
        uint32_t sdta:1;
        /** sdtc : R/W; bitpos: [28]; default: 0;
         *  configure if support driver type C in cccr
         */
        uint32_t sdtc:1;
        /** sai : R/W; bitpos: [29]; default: 1;
         *  configure if support asynchronous interrupt in cccr
         */
        uint32_t sai:1;
        /** sdio_wakeup_clr : R/W; bitpos: [30]; default: 0;
         *  clear sdio_wake_up signal after the chip wakes up
         */
        uint32_t sdio_wakeup_clr:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} hinf_cfg_data7_reg_t;

/** Type of cis_conf_w0 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** cis_conf_w0 : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configure cis addr 39~36
         */
        uint32_t cis_conf_w0:32;
    };
    uint32_t val;
} hinf_cis_conf_w0_reg_t;

/** Type of cis_conf_w1 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** cis_conf_w1 : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configure cis addr 43~40
         */
        uint32_t cis_conf_w1:32;
    };
    uint32_t val;
} hinf_cis_conf_w1_reg_t;

/** Type of cis_conf_w2 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** cis_conf_w2 : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configure cis addr 47~44
         */
        uint32_t cis_conf_w2:32;
    };
    uint32_t val;
} hinf_cis_conf_w2_reg_t;

/** Type of cis_conf_w3 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** cis_conf_w3 : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configure cis addr 51~48
         */
        uint32_t cis_conf_w3:32;
    };
    uint32_t val;
} hinf_cis_conf_w3_reg_t;

/** Type of cis_conf_w4 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** cis_conf_w4 : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configure cis addr 55~52
         */
        uint32_t cis_conf_w4:32;
    };
    uint32_t val;
} hinf_cis_conf_w4_reg_t;

/** Type of cis_conf_w5 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** cis_conf_w5 : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configure cis addr 59~56
         */
        uint32_t cis_conf_w5:32;
    };
    uint32_t val;
} hinf_cis_conf_w5_reg_t;

/** Type of cis_conf_w6 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** cis_conf_w6 : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configure cis addr 63~60
         */
        uint32_t cis_conf_w6:32;
    };
    uint32_t val;
} hinf_cis_conf_w6_reg_t;

/** Type of cis_conf_w7 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** cis_conf_w7 : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configure cis addr 67~64
         */
        uint32_t cis_conf_w7:32;
    };
    uint32_t val;
} hinf_cis_conf_w7_reg_t;

/** Type of cfg_data16 register
 *  SDIO cis configuration register
 */
typedef union {
    struct {
        /** device_id_fn2 : R/W; bitpos: [15:0]; default: 30583;
         *  configure device id of function2 in cis
         */
        uint32_t device_id_fn2:16;
        /** user_id_fn2 : R/W; bitpos: [31:16]; default: 146;
         *  configure user id of function2 in cis
         */
        uint32_t user_id_fn2:16;
    };
    uint32_t val;
} hinf_cfg_data16_reg_t;

/** Type of cfg_uhs1_int_mode register
 *  configure int to start and end ahead of time in uhs1 mode
 */
typedef union {
    struct {
        /** intoe_end_ahead_mode : R/W; bitpos: [1:0]; default: 0;
         *  intoe on dat1 end ahead of time: 0/3-no, 1-ahead 1sdclk, 2-ahead 2sdclk
         */
        uint32_t intoe_end_ahead_mode:2;
        /** int_end_ahead_mode : R/W; bitpos: [3:2]; default: 0;
         *  int on dat1 end ahead of time: 0/3-no, 1-ahead 1sdclk, 2-ahead 2sdclk
         */
        uint32_t int_end_ahead_mode:2;
        /** intoe_st_ahead_mode : R/W; bitpos: [5:4]; default: 0;
         *  intoe on dat1 start ahead of time: 0/3-no, 1-ahead 1sdclk, 2-ahead 2sdclk
         */
        uint32_t intoe_st_ahead_mode:2;
        /** int_st_ahead_mode : R/W; bitpos: [7:6]; default: 0;
         *  int on dat1 start ahead of time: 0/3-no, 1-ahead 1sdclk, 2-ahead 2sdclk
         */
        uint32_t int_st_ahead_mode:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hinf_cfg_uhs1_int_mode_reg_t;

/** Type of sdio_slave_ldo_conf register
 *  sdio slave ldo control register
 */
typedef union {
    struct {
        /** ldo_ready_ctl_in_en : R/W; bitpos: [0]; default: 0;
         *  control ldo ready signal by sdio slave itself
         */
        uint32_t ldo_ready_ctl_in_en:1;
        /** ldo_ready_thres : R/W; bitpos: [5:1]; default: 10;
         *  configure ldo ready counting threshold value, the actual counting target is
         *  2^(ldo_ready_thres)-1
         */
        uint32_t ldo_ready_thres:5;
        /** ldo_ready_ignore_en : R/W; bitpos: [6]; default: 0;
         *  ignore ldo ready signal
         */
        uint32_t ldo_ready_ignore_en:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hinf_sdio_slave_ldo_conf_reg_t;


/** Group: Status registers */
/** Type of conf_status register
 *  func0 config0 status
 */
typedef union {
    struct {
        /** func0_config0 : RO; bitpos: [7:0]; default: 0;
         *  func0 config0 (addr: 0x20f0 ) status
         */
        uint32_t func0_config0:8;
        /** sdr25_st : RO; bitpos: [8]; default: 0;
         *  sdr25 status
         */
        uint32_t sdr25_st:1;
        /** sdr50_st : RO; bitpos: [9]; default: 0;
         *  sdr50 status
         */
        uint32_t sdr50_st:1;
        /** sdr104_st : RO; bitpos: [10]; default: 0;
         *  sdr104 status
         */
        uint32_t sdr104_st:1;
        /** ddr50_st : RO; bitpos: [11]; default: 0;
         *  ddr50 status
         */
        uint32_t ddr50_st:1;
        /** tune_st : RO; bitpos: [14:12]; default: 0;
         *  tune_st fsm status
         */
        uint32_t tune_st:3;
        /** sdio_switch_volt_st : RO; bitpos: [15]; default: 0;
         *  sdio switch voltage status:0-3.3V, 1-1.8V.
         */
        uint32_t sdio_switch_volt_st:1;
        /** sdio_switch_end : RO; bitpos: [16]; default: 0;
         *  sdio switch voltage ldo ready
         */
        uint32_t sdio_switch_end:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hinf_conf_status_reg_t;


/** Group: Version register */
/** Type of sdio_date register
 *  ******* Description ***********
 */
typedef union {
    struct {
        /** sdio_date : R/W; bitpos: [31:0]; default: 37822544;
         *  sdio version date.
         */
        uint32_t sdio_date:32;
    };
    uint32_t val;
} hinf_sdio_date_reg_t;


typedef struct {
    volatile hinf_cfg_data0_reg_t cfg_data0;
    volatile hinf_cfg_data1_reg_t cfg_data1;
    volatile hinf_cfg_timing_reg_t cfg_timing;
    volatile hinf_cfg_update_reg_t cfg_update;
    uint32_t reserved_010[3];
    volatile hinf_cfg_data7_reg_t cfg_data7;
    volatile hinf_cis_conf_w0_reg_t cis_conf_w0;
    volatile hinf_cis_conf_w1_reg_t cis_conf_w1;
    volatile hinf_cis_conf_w2_reg_t cis_conf_w2;
    volatile hinf_cis_conf_w3_reg_t cis_conf_w3;
    volatile hinf_cis_conf_w4_reg_t cis_conf_w4;
    volatile hinf_cis_conf_w5_reg_t cis_conf_w5;
    volatile hinf_cis_conf_w6_reg_t cis_conf_w6;
    volatile hinf_cis_conf_w7_reg_t cis_conf_w7;
    volatile hinf_cfg_data16_reg_t cfg_data16;
    volatile hinf_cfg_uhs1_int_mode_reg_t cfg_uhs1_int_mode;
    uint32_t reserved_048[3];
    volatile hinf_conf_status_reg_t conf_status;
    uint32_t reserved_058[22];
    volatile hinf_sdio_slave_ldo_conf_reg_t sdio_slave_ldo_conf;
    uint32_t reserved_0b4[18];
    volatile hinf_sdio_date_reg_t sdio_date;
} hinf_dev_t;

extern hinf_dev_t HINF;

#ifndef __cplusplus
_Static_assert(sizeof(hinf_dev_t) == 0x100, "Invalid size of hinf_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
