// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef volatile struct {
    union {
        struct {
            uint32_t in_rst : 1;            /*This bit is used to reset DMA channel 0 Rx FSM and Rx FIFO pointer.*/
            uint32_t out_rst : 1;           /*This bit is used to reset DMA channel 0 Tx FSM and Tx FIFO pointer.*/
            uint32_t in_loop_test : 1;      /*reserved*/
            uint32_t out_loop_test : 1;     /*reserved*/
            uint32_t out_auto_wrback : 1;   /*Set this bit to enable automatic outlink-writeback when all the data in tx buffer has been transmitted.*/
            uint32_t out_eof_mode : 1;      /*EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 0 is generated when data need to transmit has been popped from FIFO in DMA*/
            uint32_t outdscr_burst_en : 1;  /*Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link descriptor when accessing internal SRAM.*/
            uint32_t indscr_burst_en : 1;   /*Set this bit to 1 to enable INCR burst transfer for Rx channel 0 reading link descriptor when accessing internal SRAM.*/
            uint32_t out_data_burst_en : 1; /*Set this bit to 1 to enable INCR burst transfer for Tx channel 0 transmitting data when accessing internal SRAM.*/
            uint32_t in_data_burst_en : 1;  /*Set this bit to 1 to enable INCR burst transfer for Rx channel 0 receiving data when accessing internal SRAM.*/
            uint32_t mem_trans_en : 1;      /*Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.*/
            uint32_t reserved11 : 21;       /*reserved*/
        };
        uint32_t val;
    } conf0[5];
    union {
        struct {
            uint32_t infifo_full_thrs : 12;   /*This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx channel 0 received byte number in Rx FIFO is up to the value of the register.*/
            uint32_t check_owner : 1;         /*Set this bit to enable checking the owner attribute of the link descriptor.*/
            uint32_t in_ext_mem_bk_size : 2;  /*Block size of Rx channel 0 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t out_ext_mem_bk_size : 2; /*Block size of Tx channel 0 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t reserved17 : 15;         /*reserved*/
        };
        uint32_t val;
    } conf1[5];
    union {
        struct {
            uint32_t in_done : 1;        /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 0.*/
            uint32_t in_suc_eof : 1;     /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 0. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 0.*/
            uint32_t in_err_eof : 1;     /*The raw interrupt bit turns to high level when data error is detected only in the case that the peripheral is UHCI0 for Rx channel 0. For other peripherals  this raw interrupt is reserved.*/
            uint32_t out_done : 1;       /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 0.*/
            uint32_t out_eof : 1;        /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been read from memory for Tx channel 0.*/
            uint32_t in_dscr_err : 1;    /*The raw interrupt bit turns to high level when detecting inlink descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 0.*/
            uint32_t out_dscr_err : 1;   /*The raw interrupt bit turns to high level when detecting outlink descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 0.*/
            uint32_t in_dscr_empty : 1;  /*The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 0.*/
            uint32_t out_total_eof : 1;  /*The raw interrupt bit turns to high level when data corresponding a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 0.*/
            uint32_t infifo_full_wm : 1; /*The raw interrupt bit turns to high level when received data byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 0.*/
            uint32_t infifo_ovf_l1 : 1;  /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is overflow.*/
            uint32_t infifo_udf_l1 : 1;  /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is underflow.*/
            uint32_t infifo_ovf_l3 : 1;  /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 0 is overflow.*/
            uint32_t infifo_udf_l3 : 1;  /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 0 is underflow.*/
            uint32_t outfifo_ovf_l1 : 1; /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is overflow.*/
            uint32_t outfifo_udf_l1 : 1; /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is underflow.*/
            uint32_t outfifo_ovf_l3 : 1; /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 0 is overflow.*/
            uint32_t outfifo_udf_l3 : 1; /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 0 is underflow.*/
            uint32_t reserved18 : 14;    /*reserved*/
        };
        uint32_t val;
    } int_raw[5];
    uint32_t reserved_3c;
    union {
        struct {
            uint32_t in_done : 1;        /*The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof : 1;     /*The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof : 1;     /*The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done : 1;       /*The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof : 1;        /*The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err : 1;    /*The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err : 1;   /*The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty : 1;  /*The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof : 1;  /*The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm : 1; /*The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1 : 1;  /*The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1 : 1;  /*The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3 : 1;  /*The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3 : 1;  /*The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1 : 1; /*The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1 : 1; /*The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3 : 1; /*The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3 : 1; /*The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18 : 14;    /*reserved*/
        };
        uint32_t val;
    } int_st[5];
    union {
        struct {
            uint32_t in_done : 1;        /*The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof : 1;     /*The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof : 1;     /*The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done : 1;       /*The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof : 1;        /*The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err : 1;    /*The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err : 1;   /*The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty : 1;  /*The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof : 1;  /*The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm : 1; /*The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1 : 1;  /*The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1 : 1;  /*The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3 : 1;  /*The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3 : 1;  /*The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1 : 1; /*The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1 : 1; /*The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3 : 1; /*The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3 : 1; /*The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18 : 14;    /*reserved*/
        };
        uint32_t val;
    } int_ena[5];
    union {
        struct {
            uint32_t in_done : 1;        /*Set this bit to clear the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof : 1;     /*Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof : 1;     /*Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done : 1;       /*Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof : 1;        /*Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err : 1;    /*Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err : 1;   /*Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty : 1;  /*Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof : 1;  /*Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm : 1; /*Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1 : 1;  /*Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1 : 1;  /*Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3 : 1;  /*Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3 : 1;  /*Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1 : 1; /*Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1 : 1; /*Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3 : 1; /*Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3 : 1; /*Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18 : 14;    /*reserved*/
        };
        uint32_t val;
    } int_clr[5];
    union {
        struct {
            uint32_t infifo_full_l1 : 1;        /*L1 Rx FIFO full signal for Rx channel 0.*/
            uint32_t infifo_empty_l1 : 1;       /*L1 Rx FIFO empty signal for Rx channel 0.*/
            uint32_t infifo_full_l2 : 1;        /*L2 Rx FIFO full signal for Rx channel 0.*/
            uint32_t infifo_empty_l2 : 1;       /*L2 Rx FIFO empty signal for Rx channel 0.*/
            uint32_t infifo_full_l3 : 1;        /*L3 Rx FIFO full signal for Rx channel 0.*/
            uint32_t infifo_empty_l3 : 1;       /*L3 Rx FIFO empty signal for Rx channel 0.*/
            uint32_t infifo_cnt_l1 : 5;         /*The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0.*/
            uint32_t infifo_cnt_l2 : 7;         /*The register stores the byte number of the data in L2 Rx FIFO for Rx channel 0.*/
            uint32_t infifo_cnt_l3 : 5;         /*The register stores the byte number of the data in L3 Rx FIFO for Rx channel 0.*/
            uint32_t in_remain_under_1b_l3 : 1; /*reserved*/
            uint32_t in_remain_under_2b_l3 : 1; /*reserved*/
            uint32_t in_remain_under_3b_l3 : 1; /*reserved*/
            uint32_t in_remain_under_4b_l3 : 1; /*reserved*/
            uint32_t in_buf_hungry : 1;         /*reserved*/
            uint32_t reserved28 : 4;            /*reserved*/
        };
        uint32_t val;
    } infifo_status[5];
    union {
        struct {
            uint32_t outfifo_full_l1 : 1;        /*L1 Tx FIFO full signal for Tx channel 0.*/
            uint32_t outfifo_empty_l1 : 1;       /*L1 Tx FIFO empty signal for Tx channel 0.*/
            uint32_t outfifo_full_l2 : 1;        /*L2 Tx FIFO full signal for Tx channel 0.*/
            uint32_t outfifo_empty_l2 : 1;       /*L2 Tx FIFO empty signal for Tx channel 0.*/
            uint32_t outfifo_full_l3 : 1;        /*L3 Tx FIFO full signal for Tx channel 0.*/
            uint32_t outfifo_empty_l3 : 1;       /*L3 Tx FIFO empty signal for Tx channel 0.*/
            uint32_t outfifo_cnt_l1 : 5;         /*The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0.*/
            uint32_t outfifo_cnt_l2 : 7;         /*The register stores the byte number of the data in L2 Tx FIFO for Tx channel 0.*/
            uint32_t outfifo_cnt_l3 : 5;         /*The register stores the byte number of the data in L3 Tx FIFO for Tx channel 0.*/
            uint32_t out_remain_under_1b_l3 : 1; /*reserved*/
            uint32_t out_remain_under_2b_l3 : 1; /*reserved*/
            uint32_t out_remain_under_3b_l3 : 1; /*reserved*/
            uint32_t out_remain_under_4b_l3 : 1; /*reserved*/
            uint32_t reserved27 : 5;             /*reserved*/
        };
        uint32_t val;
    } outfifo_status[5];
    union {
        struct {
            uint32_t outfifo_wdata : 9; /*This register stores the data that need to be pushed into DMA FIFO.*/
            uint32_t outfifo_push : 1;  /*Set this bit to push data into DMA FIFO.*/
            uint32_t reserved10 : 22;   /*reserved*/
        };
        uint32_t val;
    } out_push[5];
    union {
        struct {
            uint32_t infifo_rdata : 12; /*This register stores the data popping from DMA FIFO.*/
            uint32_t infifo_pop : 1;    /*Set this bit to pop data from DMA FIFO.*/
            uint32_t reserved13 : 19;   /*reserved*/
        };
        uint32_t val;
    } in_pop[5];
    union {
        struct {
            uint32_t addr : 20;   /*This register stores the 20 least significant bits of the first outlink descriptor's address.*/
            uint32_t stop : 1;    /*Set this bit to stop dealing with the outlink descriptors.*/
            uint32_t start : 1;   /*Set this bit to start dealing with the outlink descriptors.*/
            uint32_t restart : 1; /*Set this bit to restart a new outlink from the last address.*/
            uint32_t park : 1;    /*1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM is working.*/
            uint32_t reserved24 : 8;
        };
        uint32_t val;
    } out_link[5];
    union {
        struct {
            uint32_t addr : 20;    /*This register stores the 20 least significant bits of the first inlink descriptor's address.*/
            uint32_t auto_ret : 1; /*Set this bit to return to current inlink descriptor's address  when there are some errors in current receiving data.*/
            uint32_t stop : 1;     /*Set this bit to stop dealing with the inlink descriptors.*/
            uint32_t start : 1;    /*Set this bit to start dealing with the inlink descriptors.*/
            uint32_t restart : 1;  /*Set this bit to mount a new inlink descriptor.*/
            uint32_t park : 1;     /*1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is working.*/
            uint32_t reserved25 : 7;
        };
        uint32_t val;
    } in_link[5];
    union {
        struct {
            uint32_t inlink_dscr_addr : 18; /*This register stores the current inlink descriptor's address.*/
            uint32_t in_dscr_state : 2;     /*reserved*/
            uint32_t in_state : 3;          /*reserved*/
            uint32_t reserved23 : 9;        /*reserved*/
        };
        uint32_t val;
    } in_state[5];
    union {
        struct {
            uint32_t outlink_dscr_addr : 18; /*This register stores the current outlink descriptor's address.*/
            uint32_t out_dscr_state : 2;     /*reserved*/
            uint32_t out_state : 3;          /*reserved*/
            uint32_t reserved23 : 9;         /*reserved*/
        };
        uint32_t val;
    } out_state[5];
    uint32_t out_eof_des_addr[5];     /*This register stores the address of the outlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t in_suc_eof_des_addr[5];  /*This register stores the address of the inlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t in_err_eof_des_addr[5];  /*This register stores the address of the inlink descriptor when there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
    uint32_t out_eof_bfr_des_addr[5]; /*This register stores the address of the outlink descriptor before the last outlink descriptor.*/
    union {
        struct {
            uint32_t ahb_testmode : 3; /*reserved*/
            uint32_t reserved3 : 1;    /*reserved*/
            uint32_t ahb_testaddr : 2; /*reserved*/
            uint32_t reserved6 : 26;   /*reserved*/
        };
        uint32_t val;
    } ahb_test;
    uint32_t in_dscr[5];      /*The address of the current inlink descriptor x.*/
    uint32_t in_dscr_bf0[5];  /*The address of the last inlink descriptor x-1.*/
    uint32_t in_dscr_bf1[5];  /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t out_dscr[5];     /*The address of the current outlink descriptor y.*/
    uint32_t out_dscr_bf0[5]; /*The address of the last outlink descriptor y-1.*/
    uint32_t out_dscr_bf1[5]; /*The address of the second-to-last inlink descriptor y-2.*/
    union {
        struct {
            uint32_t reserved0 : 4;    /*reserved*/
            uint32_t ram_force_pd : 1; /*power down*/
            uint32_t ram_force_pu : 1;
            uint32_t ram_clk_fo : 1; /*1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA. 0: A gate-clock will be used when accessing the RAM in DMA.*/
            uint32_t reserved7 : 25; /*reserved*/
        };
        uint32_t val;
    } pd_conf;
    union {
        struct {
            uint32_t tx_weight : 4; /*The weight of Tx channel 0.*/
            uint32_t rx_weight : 4; /*The weight of Rx channel 0.*/
            uint32_t reserved8 : 24;
        };
        uint32_t val;
    } wight[5];
    union {
        struct {
            uint32_t tx_pri : 4; /*The priority of Tx channel 0. The larger of the value  the higher of the priority.*/
            uint32_t rx_pri : 4; /*The priority of Rx channel 0. The larger of the value  the higher of the priority.*/
            uint32_t reserved8 : 24;
        };
        uint32_t val;
    } pri[5];
    union {
        struct {
            uint32_t ahbm_rst_inter : 1; /*Set this bit  then clear this bit to reset the internal ahb FSM.*/
            uint32_t ahbm_rst_exter : 1; /*Set this bit  then clear this bit to reset the external ahb FSM.*/
            uint32_t arb_pri_dis : 1;    /*Set this bit to disable priority arbitration function.*/
            uint32_t clk_en : 1;
            uint32_t reserved4 : 28;
        };
        uint32_t val;
    } misc_conf;
    union {
        struct {
            uint32_t peri_in_sel : 6;  /*This register is used to select peripheral for Rx channel 0. 0:SPI2*/
            uint32_t peri_out_sel : 6; /*This register is used to select peripheral for Tx channel 0. 0:SPI2*/
            uint32_t reserved12 : 20;
        };
        uint32_t val;
    } peri_sel[5];
    union {
        struct {
            uint32_t in_size : 5;  /*This register is used to configure the size of L2 Rx FIFO for Rx channel 0. 0:16 bytes*/
            uint32_t out_size : 5; /*This register is used to configure the size of L2 Tx FIFO for Tx channel 0. 0:16 bytes*/
            uint32_t reserved10 : 22;
        };
        uint32_t val;
    } sram_size[5];
    uint32_t date; /*register version.*/
} gdma_dev_t;

_Static_assert(sizeof(gdma_dev_t) == 0x244, "incorrect size of gdma_dev_t.");

extern gdma_dev_t GDMA;

#ifdef __cplusplus
}
#endif
