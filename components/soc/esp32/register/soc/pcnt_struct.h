/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pcnt_dev_t {
    volatile struct {
        union {
            struct {
                uint32_t filter_thres:  10;         /*This register is used to filter pulse whose width is smaller than this value for unit0.*/
                uint32_t filter_en:      1;         /*This is the enable bit for filtering input signals for unit0.*/
                uint32_t thr_zero_en:    1;         /*This is the enable bit for comparing unit0's count with 0 value.*/
                uint32_t thr_h_lim_en:   1;         /*This is the enable bit for  comparing unit0's count with thr_h_lim value.*/
                uint32_t thr_l_lim_en:   1;         /*This is the enable bit for comparing unit0's count with thr_l_lim  value.*/
                uint32_t thr_thres0_en:  1;         /*This is the enable bit for comparing unit0's count with  thres0 value.*/
                uint32_t thr_thres1_en:  1;         /*This is the enable bit for  comparing  unit0's count with thres1 value .*/
                uint32_t ch0_neg_mode:   2;         /*This register is used to control the mode of channel0's input neg-edge signal for unit0. 2'd1：increase at the negedge of input signal    2'd2:decrease at the negedge of input signal    others:forbidden*/
                uint32_t ch0_pos_mode:   2;         /*This register is used to control the mode of channel0's input pos-edge signal for unit0. 2'd1：increase at the posedge of input signal    2'd2:decrease at the posedge of input signal    others:forbidden*/
                uint32_t ch0_hctrl_mode: 2;         /*This register is used to control the mode of channel0's high control signal for unit0. 2'd0:increase when control signal is low   2'd1：decrease when control signal is high   others:forbidden*/
                uint32_t ch0_lctrl_mode: 2;         /*This register is used to control the mode of channel0's low control signal for unit0. 2'd0:increase when control signal is low   2'd1：decrease when control signal is high   others:forbidden*/
                uint32_t ch1_neg_mode:   2;         /*This register is used to control the mode of channel1's input neg-edge signal for unit0. 2'd1：increase at the negedge of input signal    2'd2:decrease at the negedge of input signal    others:forbidden*/
                uint32_t ch1_pos_mode:   2;         /*This register is used to control the mode of channel1's input pos-edge signal for unit0. 2'd1：increase at the posedge of input signal    2'd2:decrease at the posedge of input signal    others:forbidden*/
                uint32_t ch1_hctrl_mode: 2;         /*This register is used to control the mode of channel1's high control signal for unit0. 2'd0:increase when control signal is low   2'd1：decrease when control signal is high   others:forbidden*/
                uint32_t ch1_lctrl_mode: 2;         /*This register is used to control the mode of channel1's low control signal for unit0. 2'd0:increase when control signal is low   2'd1：decrease when control signal is high   others:forbidden*/
            };
            uint32_t val;
        } conf0;
        union {
            struct {
                uint32_t cnt_thres0: 16;            /*This register is used to configure thres0 value for unit0.*/
                uint32_t cnt_thres1: 16;            /*This register is used to configure thres1 value for unit0.*/
            };
            uint32_t val;
        } conf1;
        union {
            struct {
                uint32_t cnt_h_lim: 16;             /*This register is used to configure thr_h_lim value for unit0.*/
                uint32_t cnt_l_lim: 16;             /*This register is used to configure thr_l_lim value for unit0.*/
            };
            uint32_t val;
        } conf2;
    } conf_unit[8];
    volatile union {
        struct {
            uint32_t cnt_val   : 16;                /*This register stores the current pulse count value for unit0.*/
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } cnt_unit[8];
    volatile union {
        struct {
            uint32_t cnt_thr_event_u0: 1;           /*This is the interrupt raw bit for channel0 event.*/
            uint32_t cnt_thr_event_u1: 1;           /*This is the interrupt raw bit for channel1 event.*/
            uint32_t cnt_thr_event_u2: 1;           /*This is the interrupt raw bit for channel2 event.*/
            uint32_t cnt_thr_event_u3: 1;           /*This is the interrupt raw bit for channel3 event.*/
            uint32_t cnt_thr_event_u4: 1;           /*This is the interrupt raw bit for channel4 event.*/
            uint32_t cnt_thr_event_u5: 1;           /*This is the interrupt raw bit for channel5 event.*/
            uint32_t cnt_thr_event_u6: 1;           /*This is the interrupt raw bit for channel6 event.*/
            uint32_t cnt_thr_event_u7: 1;           /*This is the interrupt raw bit for channel7 event.*/
            uint32_t reserved8:       24;
        };
        uint32_t val;
    } int_raw;
    volatile union {
        struct {
            uint32_t cnt_thr_event_u0: 1;            /*This is the  interrupt status bit for channel0 event.*/
            uint32_t cnt_thr_event_u1: 1;            /*This is the  interrupt status bit for channel1 event.*/
            uint32_t cnt_thr_event_u2: 1;            /*This is the  interrupt status bit for channel2 event.*/
            uint32_t cnt_thr_event_u3: 1;            /*This is the  interrupt status bit for channel3 event.*/
            uint32_t cnt_thr_event_u4: 1;            /*This is the  interrupt status bit for channel4 event.*/
            uint32_t cnt_thr_event_u5: 1;            /*This is the  interrupt status bit for channel5 event.*/
            uint32_t cnt_thr_event_u6: 1;            /*This is the  interrupt status bit for channel6 event.*/
            uint32_t cnt_thr_event_u7: 1;            /*This is the  interrupt status bit for channel7 event.*/
            uint32_t reserved8:       24;
        };
        uint32_t val;
    } int_st;
    volatile union {
        struct {
            uint32_t cnt_thr_event_u0: 1;           /*This is the  interrupt enable bit for channel0 event.*/
            uint32_t cnt_thr_event_u1: 1;           /*This is the  interrupt enable bit for channel1 event.*/
            uint32_t cnt_thr_event_u2: 1;           /*This is the  interrupt enable bit for channel2 event.*/
            uint32_t cnt_thr_event_u3: 1;           /*This is the  interrupt enable bit for channel3 event.*/
            uint32_t cnt_thr_event_u4: 1;           /*This is the  interrupt enable bit for channel4 event.*/
            uint32_t cnt_thr_event_u5: 1;           /*This is the  interrupt enable bit for channel5 event.*/
            uint32_t cnt_thr_event_u6: 1;           /*This is the  interrupt enable bit for channel6 event.*/
            uint32_t cnt_thr_event_u7: 1;           /*This is the  interrupt enable bit for channel7 event.*/
            uint32_t reserved8:       24;
        };
        uint32_t val;
    } int_ena;
    volatile union {
        struct {
            uint32_t cnt_thr_event_u0: 1;           /*Set this bit to clear channel0 event interrupt.*/
            uint32_t cnt_thr_event_u1: 1;           /*Set this bit to clear channel1 event interrupt.*/
            uint32_t cnt_thr_event_u2: 1;           /*Set this bit to clear channel2 event interrupt.*/
            uint32_t cnt_thr_event_u3: 1;           /*Set this bit to clear channel3 event interrupt.*/
            uint32_t cnt_thr_event_u4: 1;           /*Set this bit to clear channel4 event interrupt.*/
            uint32_t cnt_thr_event_u5: 1;           /*Set this bit to clear channel5 event interrupt.*/
            uint32_t cnt_thr_event_u6: 1;           /*Set this bit to clear channel6 event interrupt.*/
            uint32_t cnt_thr_event_u7: 1;           /*Set this bit to clear channel7 event interrupt.*/
            uint32_t reserved8:       24;
        };
        uint32_t val;
    } int_clr;
    volatile union {
        struct {
            uint32_t cnt_mode: 2;                   /*0: positive value to zero; 1: negative value to zero; 2: counter value negative ; 3: counter value positive*/
            uint32_t thres1_lat: 1;                 /* counter value equals to thresh1*/
            uint32_t thres0_lat: 1;                 /* counter value equals to thresh0*/
            uint32_t l_lim_lat: 1;                  /* counter value reaches h_lim*/
            uint32_t h_lim_lat: 1;                  /* counter value reaches l_lim*/
            uint32_t zero_lat: 1;                   /* counter value equals zero*/
            uint32_t reserved7: 25;
        };
        uint32_t val;
    } status_unit[8];
    volatile union {
        struct {
            uint32_t cnt_rst_u0:   1;               /*Set this bit to clear unit0's counter.*/
            uint32_t cnt_pause_u0: 1;               /*Set this bit to pause unit0's counter.*/
            uint32_t cnt_rst_u1:   1;               /*Set this bit to clear unit1's counter.*/
            uint32_t cnt_pause_u1: 1;               /*Set this bit to pause unit1's counter.*/
            uint32_t cnt_rst_u2:   1;               /*Set this bit to clear unit2's counter.*/
            uint32_t cnt_pause_u2: 1;               /*Set this bit to pause unit2's counter.*/
            uint32_t cnt_rst_u3:   1;               /*Set this bit to clear unit3's counter.*/
            uint32_t cnt_pause_u3: 1;               /*Set this bit to pause unit3's counter.*/
            uint32_t cnt_rst_u4:   1;               /*Set this bit to clear unit4's counter.*/
            uint32_t cnt_pause_u4: 1;               /*Set this bit to pause unit4's counter.*/
            uint32_t cnt_rst_u5:   1;               /*Set this bit to clear unit5's counter.*/
            uint32_t cnt_pause_u5: 1;               /*Set this bit to pause unit5's counter.*/
            uint32_t cnt_rst_u6:   1;               /*Set this bit to clear unit6's counter.*/
            uint32_t cnt_pause_u6: 1;               /*Set this bit to pause unit6's counter.*/
            uint32_t cnt_rst_u7:   1;               /*Set this bit to clear unit7's counter.*/
            uint32_t cnt_pause_u7: 1;               /*Set this bit to pause unit7's counter.*/
            uint32_t clk_en:       1;
            uint32_t reserved17:  15;
        };
        uint32_t val;
    } ctrl;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    volatile uint32_t date;
} pcnt_dev_t;

extern pcnt_dev_t PCNT;

#ifdef __cplusplus
}
#endif
