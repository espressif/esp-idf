// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
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
extern "C" {
#endif

#include <stdint.h>

typedef volatile struct {
    union {
        struct {
            uint32_t in_rst_ch0:            1;                /*This bit is used to reset DMA channel 0 Rx FSM and Rx FIFO pointer.*/
            uint32_t out_rst_ch0:           1;                /*This bit is used to reset DMA channel 0 Tx FSM and Tx FIFO pointer.*/
            uint32_t in_loop_test_ch0:      1;                /*reserved*/
            uint32_t out_loop_test_ch0:     1;                /*reserved*/
            uint32_t out_auto_wrback_ch0:   1;                /*Set this bit to enable automatic outlink-writeback when all the data in tx buffer has been transmitted.*/
            uint32_t out_eof_mode_ch0:      1;                /*EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 0 is generated when data need to transmit has been popped from FIFO in DMA*/
            uint32_t outdscr_burst_en_ch0:  1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link descriptor when accessing internal SRAM.*/
            uint32_t indscr_burst_en_ch0:   1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 0 reading link descriptor when accessing internal SRAM.*/
            uint32_t out_data_burst_en_ch0: 1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 0 transmitting data when accessing internal SRAM.*/
            uint32_t in_data_burst_en_ch0:  1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 0 receiving data when accessing internal SRAM.*/
            uint32_t mem_trans_en_ch0:      1;                /*Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.*/
            uint32_t reserved11:           21;                /*reserved*/
        };
        uint32_t val;
    } conf0_ch0;
    union {
        struct {
            uint32_t in_rst_ch1:            1;                /*This bit is used to reset DMA channel 1 Rx FSM and Rx FIFO pointer.*/
            uint32_t out_rst_ch1:           1;                /*This bit is used to reset DMA channel 1 Tx FSM and Tx FIFO pointer.*/
            uint32_t in_loop_test_ch1:      1;                /*reserved*/
            uint32_t out_loop_test_ch1:     1;                /*reserved*/
            uint32_t out_auto_wrback_ch1:   1;                /*Set this bit to enable automatic outlink-writeback when all the data in tx buffer has been transmitted.*/
            uint32_t out_eof_mode_ch1:      1;                /*EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 1 is generated when data need to transmit has been popped from FIFO in DMA*/
            uint32_t outdscr_burst_en_ch1:  1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 1 reading link descriptor when accessing internal SRAM.*/
            uint32_t indscr_burst_en_ch1:   1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 1 reading link descriptor when accessing internal SRAM.*/
            uint32_t out_data_burst_en_ch1: 1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 1 transmitting data when accessing internal SRAM.*/
            uint32_t in_data_burst_en_ch1:  1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 1 receiving data when accessing internal SRAM.*/
            uint32_t mem_trans_en_ch1:      1;                /*Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.*/
            uint32_t reserved11:           21;                /*reserved*/
        };
        uint32_t val;
    } conf0_ch1;
    union {
        struct {
            uint32_t in_rst_ch2:            1;                /*This bit is used to reset DMA channel 2 Rx FSM and Rx FIFO pointer.*/
            uint32_t out_rst_ch2:           1;                /*This bit is used to reset DMA channel 2 Tx FSM and Tx FIFO pointer.*/
            uint32_t in_loop_test_ch2:      1;                /*reserved*/
            uint32_t out_loop_test_ch2:     1;                /*reserved*/
            uint32_t out_auto_wrback_ch2:   1;                /*Set this bit to enable automatic outlink-writeback when all the data in tx buffer has been transmitted.*/
            uint32_t out_eof_mode_ch2:      1;                /*EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 2 is generated when data need to transmit has been popped from FIFO in DMA*/
            uint32_t outdscr_burst_en_ch2:  1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 2 reading link descriptor when accessing internal SRAM.*/
            uint32_t indscr_burst_en_ch2:   1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 2 reading link descriptor when accessing internal SRAM.*/
            uint32_t out_data_burst_en_ch2: 1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 2 transmitting data when accessing internal SRAM.*/
            uint32_t in_data_burst_en_ch2:  1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 2 receiving data when accessing internal SRAM.*/
            uint32_t mem_trans_en_ch2:      1;                /*Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.*/
            uint32_t reserved11:           21;                /*reserved*/
        };
        uint32_t val;
    } conf0_ch2;
    union {
        struct {
            uint32_t in_rst_ch3:            1;                /*This bit is used to reset DMA channel 3 Rx FSM and Rx FIFO pointer.*/
            uint32_t out_rst_ch3:           1;                /*This bit is used to reset DMA channel 3 Tx FSM and Tx FIFO pointer.*/
            uint32_t in_loop_test_ch3:      1;                /*reserved*/
            uint32_t out_loop_test_ch3:     1;                /*reserved*/
            uint32_t out_auto_wrback_ch3:   1;                /*Set this bit to enable automatic outlink-writeback when all the data in tx buffer has been transmitted.*/
            uint32_t out_eof_mode_ch3:      1;                /*EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 3 is generated when data need to transmit has been popped from FIFO in DMA*/
            uint32_t outdscr_burst_en_ch3:  1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 3 reading link descriptor when accessing internal SRAM.*/
            uint32_t indscr_burst_en_ch3:   1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 3 reading link descriptor when accessing internal SRAM.*/
            uint32_t out_data_burst_en_ch3: 1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 3 transmitting data when accessing internal SRAM.*/
            uint32_t in_data_burst_en_ch3:  1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 3 receiving data when accessing internal SRAM.*/
            uint32_t mem_trans_en_ch3:      1;                /*Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.*/
            uint32_t reserved11:           21;                /*reserved*/
        };
        uint32_t val;
    } conf0_ch3;
    union {
        struct {
            uint32_t in_rst_ch4:            1;                /*This bit is used to reset DMA channel 4 Rx FSM and Rx FIFO pointer.*/
            uint32_t out_rst_ch4:           1;                /*This bit is used to reset DMA channel 4 Tx FSM and Tx FIFO pointer.*/
            uint32_t in_loop_test_ch4:      1;                /*reserved*/
            uint32_t out_loop_test_ch4:     1;                /*reserved*/
            uint32_t out_auto_wrback_ch4:   1;                /*Set this bit to enable automatic outlink-writeback when all the data in tx buffer has been transmitted.*/
            uint32_t out_eof_mode_ch4:      1;                /*EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 4 is generated when data need to transmit has been popped from FIFO in DMA*/
            uint32_t outdscr_burst_en_ch4:  1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 4 reading link descriptor when accessing internal SRAM.*/
            uint32_t indscr_burst_en_ch4:   1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 4 reading link descriptor when accessing internal SRAM.*/
            uint32_t out_data_burst_en_ch4: 1;                /*Set this bit to 1 to enable INCR burst transfer for Tx channel 4 transmitting data when accessing internal SRAM.*/
            uint32_t in_data_burst_en_ch4:  1;                /*Set this bit to 1 to enable INCR burst transfer for Rx channel 4 receiving data when accessing internal SRAM.*/
            uint32_t mem_trans_en_ch4:      1;                /*Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.*/
            uint32_t reserved11:           21;                /*reserved*/
        };
        uint32_t val;
    } conf0_ch4;
    union {
        struct {
            uint32_t infifo_full_thrs_ch0:   12;              /*This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx channel 0 received byte number in Rx FIFO is up to the value of the register.*/
            uint32_t check_owner_ch0:         1;              /*Set this bit to enable checking the owner attribute of the link descriptor.*/
            uint32_t in_ext_mem_bk_size_ch0:  2;              /*Block size of Rx channel 0 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t out_ext_mem_bk_size_ch0: 2;              /*Block size of Tx channel 0 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t reserved17:             15;              /*reserved*/
        };
        uint32_t val;
    } conf1_ch0;
    union {
        struct {
            uint32_t infifo_full_thrs_ch1:   12;              /*This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx channel 1 received byte number in Rx FIFO is up to the value of the register.*/
            uint32_t check_owner_ch1:         1;              /*Set this bit to enable checking the owner attribute of the link descriptor.*/
            uint32_t in_ext_mem_bk_size_ch1:  2;              /*Block size of Rx channel 1 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t out_ext_mem_bk_size_ch1: 2;              /*Block size of Tx channel 1 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t reserved17:             15;              /*reserved*/
        };
        uint32_t val;
    } conf1_ch1;
    union {
        struct {
            uint32_t infifo_full_thrs_ch2:   12;              /*This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx channel 2 received byte number in Rx FIFO is up to the value of the register.*/
            uint32_t check_owner_ch2:         1;              /*Set this bit to enable checking the owner attribute of the link descriptor.*/
            uint32_t in_ext_mem_bk_size_ch2:  2;              /*Block size of Rx channel 2 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t out_ext_mem_bk_size_ch2: 2;              /*Block size of Tx channel 2 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t reserved17:             15;              /*reserved*/
        };
        uint32_t val;
    } conf1_ch2;
    union {
        struct {
            uint32_t infifo_full_thrs_ch3:   12;              /*This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx channel 3 received byte number in Rx FIFO is up to the value of the register.*/
            uint32_t check_owner_ch3:         1;              /*Set this bit to enable checking the owner attribute of the link descriptor.*/
            uint32_t in_ext_mem_bk_size_ch3:  2;              /*Block size of Rx channel 3 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t out_ext_mem_bk_size_ch3: 2;              /*Block size of Tx channel 3 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t reserved17:             15;              /*reserved*/
        };
        uint32_t val;
    } conf1_ch3;
    union {
        struct {
            uint32_t infifo_full_thrs_ch4:   12;              /*This register is used to generate the INFIFO_FULL_WM_INT interrupt when Rx channel 4 received byte number in Rx FIFO is up to the value of the register.*/
            uint32_t check_owner_ch4:         1;              /*Set this bit to enable checking the owner attribute of the link descriptor.*/
            uint32_t in_ext_mem_bk_size_ch4:  2;              /*Block size of Rx channel 4 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t out_ext_mem_bk_size_ch4: 2;              /*Block size of Tx channel 4 when DMA access external SRAM. 0: 16 bytes      1: 32 bytes    2/3:reserved*/
            uint32_t reserved17:             15;              /*reserved*/
        };
        uint32_t val;
    } conf1_ch4;
    union {
        struct {
            uint32_t in_done_ch0:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 0.*/
            uint32_t in_suc_eof_ch0:             1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 0. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 0.*/
            uint32_t in_err_eof_ch0:             1;           /*The raw interrupt bit turns to high level when data error is detected only in the case that the peripheral is UHCI0 for Rx channel 0. For other peripherals  this raw interrupt is reserved.*/
            uint32_t out_done_ch0:               1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 0.*/
            uint32_t out_eof_ch0:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been read from memory for Tx channel 0.*/
            uint32_t in_dscr_err_ch0:            1;           /*The raw interrupt bit turns to high level when detecting inlink descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 0.*/
            uint32_t out_dscr_err_ch0:           1;           /*The raw interrupt bit turns to high level when detecting outlink descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 0.*/
            uint32_t in_dscr_empty_ch0:          1;           /*The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 0.*/
            uint32_t out_total_eof_ch0:          1;           /*The raw interrupt bit turns to high level when data corresponding a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 0.*/
            uint32_t infifo_full_wm_ch0:         1;           /*The raw interrupt bit turns to high level when received data byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 0.*/
            uint32_t infifo_ovf_l1_ch0:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is overflow.*/
            uint32_t infifo_udf_l1_ch0:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is underflow.*/
            uint32_t infifo_ovf_l3_ch0:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 0 is overflow.*/
            uint32_t infifo_udf_l3_ch0:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 0 is underflow.*/
            uint32_t outfifo_ovf_l1_ch0:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is overflow.*/
            uint32_t outfifo_udf_l1_ch0:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is underflow.*/
            uint32_t outfifo_ovf_l3_ch0:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 0 is overflow.*/
            uint32_t outfifo_udf_l3_ch0:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 0 is underflow.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_raw_ch0;
    union {
        struct {
            uint32_t in_done_ch1:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 1.*/
            uint32_t in_suc_eof_ch1:             1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 1. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 1.*/
            uint32_t in_err_eof_ch1:             1;           /*The raw interrupt bit turns to high level when data error is detected only in the case that the peripheral is UHCI0 for Rx channel 1. For other peripherals  this raw interrupt is reserved.*/
            uint32_t out_done_ch1:               1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 1.*/
            uint32_t out_eof_ch1:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been read from memory for Tx channel 1.*/
            uint32_t in_dscr_err_ch1:            1;           /*The raw interrupt bit turns to high level when detecting inlink descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 1.*/
            uint32_t out_dscr_err_ch1:           1;           /*The raw interrupt bit turns to high level when detecting outlink descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 1.*/
            uint32_t in_dscr_empty_ch1:          1;           /*The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 1.*/
            uint32_t out_total_eof_ch1:          1;           /*The raw interrupt bit turns to high level when data corresponding a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 1.*/
            uint32_t infifo_full_wm_ch1:         1;           /*The raw interrupt bit turns to high level when received data byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 1.*/
            uint32_t infifo_ovf_l1_ch1:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is overflow.*/
            uint32_t infifo_udf_l1_ch1:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is underflow.*/
            uint32_t infifo_ovf_l3_ch1:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 1 is overflow.*/
            uint32_t infifo_udf_l3_ch1:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 1 is underflow.*/
            uint32_t outfifo_ovf_l1_ch1:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 1 is overflow.*/
            uint32_t outfifo_udf_l1_ch1:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 1 is underflow.*/
            uint32_t outfifo_ovf_l3_ch1:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 1 is overflow.*/
            uint32_t outfifo_udf_l3_ch1:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 1 is underflow.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_raw_ch1;
    union {
        struct {
            uint32_t in_done_ch2:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 2.*/
            uint32_t in_suc_eof_ch2:             1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 2. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 2.*/
            uint32_t in_err_eof_ch2:             1;           /*The raw interrupt bit turns to high level when data error is detected only in the case that the peripheral is UHCI0 for Rx channel 2. For other peripherals  this raw interrupt is reserved.*/
            uint32_t out_done_ch2:               1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 2.*/
            uint32_t out_eof_ch2:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been read from memory for Tx channel 2.*/
            uint32_t in_dscr_err_ch2:            1;           /*The raw interrupt bit turns to high level when detecting inlink descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 2.*/
            uint32_t out_dscr_err_ch2:           1;           /*The raw interrupt bit turns to high level when detecting outlink descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 2.*/
            uint32_t in_dscr_empty_ch2:          1;           /*The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 2.*/
            uint32_t out_total_eof_ch2:          1;           /*The raw interrupt bit turns to high level when data corresponding a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 2.*/
            uint32_t infifo_full_wm_ch2:         1;           /*The raw interrupt bit turns to high level when received data byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 2.*/
            uint32_t infifo_ovf_l1_ch2:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is overflow.*/
            uint32_t infifo_udf_l1_ch2:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is underflow.*/
            uint32_t infifo_ovf_l3_ch2:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 2 is overflow.*/
            uint32_t infifo_udf_l3_ch2:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 2 is underflow.*/
            uint32_t outfifo_ovf_l1_ch2:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 2 is overflow.*/
            uint32_t outfifo_udf_l1_ch2:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 2 is underflow.*/
            uint32_t outfifo_ovf_l3_ch2:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 2 is overflow.*/
            uint32_t outfifo_udf_l3_ch2:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 2 is underflow.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_raw_ch2;
    union {
        struct {
            uint32_t in_done_ch3:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 3.*/
            uint32_t in_suc_eof_ch3:             1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 3. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 3.*/
            uint32_t in_err_eof_ch3:             1;           /*The raw interrupt bit turns to high level when data error is detected only in the case that the peripheral is UHCI0 for Rx channel 3. For other peripherals  this raw interrupt is reserved.*/
            uint32_t out_done_ch3:               1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 3.*/
            uint32_t out_eof_ch3:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been read from memory for Tx channel 3.*/
            uint32_t in_dscr_err_ch3:            1;           /*The raw interrupt bit turns to high level when detecting inlink descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 3.*/
            uint32_t out_dscr_err_ch3:           1;           /*The raw interrupt bit turns to high level when detecting outlink descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 3.*/
            uint32_t in_dscr_empty_ch3:          1;           /*The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 3.*/
            uint32_t out_total_eof_ch3:          1;           /*The raw interrupt bit turns to high level when data corresponding a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 3.*/
            uint32_t infifo_full_wm_ch3:         1;           /*The raw interrupt bit turns to high level when received data byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 3.*/
            uint32_t infifo_ovf_l1_ch3:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 3 is overflow.*/
            uint32_t infifo_udf_l1_ch3:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 3 is underflow.*/
            uint32_t infifo_ovf_l3_ch3:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 3 is overflow.*/
            uint32_t infifo_udf_l3_ch3:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 3 is underflow.*/
            uint32_t outfifo_ovf_l1_ch3:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 3 is overflow.*/
            uint32_t outfifo_udf_l1_ch3:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 3 is underflow.*/
            uint32_t outfifo_ovf_l3_ch3:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 3 is overflow.*/
            uint32_t outfifo_udf_l3_ch3:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 3 is underflow.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_raw_ch3;
    union {
        struct {
            uint32_t in_done_ch4:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 4.*/
            uint32_t in_suc_eof_ch4:             1;           /*The raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received for Rx channel 4. For UHCI0  the raw interrupt bit turns to high level when the last data pointed by one inlink descriptor has been received and no data error is detected for Rx channel 4.*/
            uint32_t in_err_eof_ch4:             1;           /*The raw interrupt bit turns to high level when data error is detected only in the case that the peripheral is UHCI0 for Rx channel 4. For other peripherals  this raw interrupt is reserved.*/
            uint32_t out_done_ch4:               1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been transmitted to peripherals for Tx channel 4.*/
            uint32_t out_eof_ch4:                1;           /*The raw interrupt bit turns to high level when the last data pointed by one outlink descriptor has been read from memory for Tx channel 4.*/
            uint32_t in_dscr_err_ch4:            1;           /*The raw interrupt bit turns to high level when detecting inlink descriptor error  including owner error  the second and third word error of inlink descriptor for Rx channel 4.*/
            uint32_t out_dscr_err_ch4:           1;           /*The raw interrupt bit turns to high level when detecting outlink descriptor error  including owner error  the second and third word error of outlink descriptor for Tx channel 4.*/
            uint32_t in_dscr_empty_ch4:          1;           /*The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full and receiving data is not completed  but there is no more inlink for Rx channel 4.*/
            uint32_t out_total_eof_ch4:          1;           /*The raw interrupt bit turns to high level when data corresponding a outlink (includes one link descriptor or few link descriptors) is transmitted out for Tx channel 4.*/
            uint32_t infifo_full_wm_ch4:         1;           /*The raw interrupt bit turns to high level when received data byte number is up to threshold configured by REG_DMA_INFIFO_FULL_THRS_CH0 in Rx FIFO of channel 4.*/
            uint32_t infifo_ovf_l1_ch4:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 4 is overflow.*/
            uint32_t infifo_udf_l1_ch4:          1;           /*This raw interrupt bit turns to high level when level 1 fifo of Rx channel 4 is underflow.*/
            uint32_t infifo_ovf_l3_ch4:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 4 is overflow.*/
            uint32_t infifo_udf_l3_ch4:          1;           /*This raw interrupt bit turns to high level when level 3 fifo of Rx channel 4 is underflow.*/
            uint32_t outfifo_ovf_l1_ch4:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 4 is overflow.*/
            uint32_t outfifo_udf_l1_ch4:         1;           /*This raw interrupt bit turns to high level when level 1 fifo of Tx channel 4 is underflow.*/
            uint32_t outfifo_ovf_l3_ch4:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 4 is overflow.*/
            uint32_t outfifo_udf_l3_ch4:         1;           /*This raw interrupt bit turns to high level when level 3 fifo of Tx channel 4 is underflow.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_raw_ch4;
    uint32_t reserved_3c;
    union {
        struct {
            uint32_t in_done_ch0:               1;            /*The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch0:            1;            /*The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch0:            1;            /*The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch0:              1;            /*The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch0:               1;            /*The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch0:           1;            /*The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch0:          1;            /*The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch0:         1;            /*The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch0:         1;            /*The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch0:        1;            /*The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch0:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch0:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch0:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch0:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch0:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch0:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch0:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch0:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:               14;            /*reserved*/
        };
        uint32_t val;
    } int_st_ch0;
    union {
        struct {
            uint32_t in_done_ch1:               1;            /*The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch1:            1;            /*The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch1:            1;            /*The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch1:              1;            /*The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch1:               1;            /*The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch1:           1;            /*The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch1:          1;            /*The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch1:         1;            /*The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch1:         1;            /*The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch1:        1;            /*The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch1:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch1:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch1:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch1:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch1:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch1:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch1:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch1:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:               14;            /*reserved*/
        };
        uint32_t val;
    } int_st_ch1;
    union {
        struct {
            uint32_t in_done_ch2:               1;            /*The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch2:            1;            /*The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch2:            1;            /*The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch2:              1;            /*The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch2:               1;            /*The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch2:           1;            /*The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch2:          1;            /*The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch2:         1;            /*The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch2:         1;            /*The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch2:        1;            /*The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch2:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch2:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch2:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch2:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch2:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch2:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch2:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch2:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:               14;            /*reserved*/
        };
        uint32_t val;
    } int_st_ch2;
    union {
        struct {
            uint32_t in_done_ch3:               1;            /*The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch3:            1;            /*The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch3:            1;            /*The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch3:              1;            /*The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch3:               1;            /*The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch3:           1;            /*The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch3:          1;            /*The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch3:         1;            /*The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch3:         1;            /*The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch3:        1;            /*The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch3:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch3:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch3:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch3:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch3:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch3:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch3:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch3:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:               14;            /*reserved*/
        };
        uint32_t val;
    } int_st_ch3;
    union {
        struct {
            uint32_t in_done_ch4:               1;            /*The raw interrupt status bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch4:            1;            /*The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch4:            1;            /*The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch4:              1;            /*The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch4:               1;            /*The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch4:           1;            /*The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch4:          1;            /*The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch4:         1;            /*The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch4:         1;            /*The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch4:        1;            /*The raw interrupt status bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch4:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch4:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch4:         1;            /*The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch4:         1;            /*The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch4:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch4:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch4:        1;            /*The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch4:        1;            /*The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:               14;            /*reserved*/
        };
        uint32_t val;
    } int_st_ch4;
    union {
        struct {
            uint32_t in_done_ch0:                1;           /*The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch0:             1;           /*The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch0:             1;           /*The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch0:               1;           /*The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch0:                1;           /*The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch0:            1;           /*The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch0:           1;           /*The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch0:          1;           /*The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch0:          1;           /*The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch0:         1;           /*The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch0:          1;           /*The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch0:          1;           /*The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch0:          1;           /*The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch0:          1;           /*The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch0:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch0:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch0:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch0:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_ena_ch0;
    union {
        struct {
            uint32_t in_done_ch1:                1;           /*The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch1:             1;           /*The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch1:             1;           /*The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch1:               1;           /*The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch1:                1;           /*The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch1:            1;           /*The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch1:           1;           /*The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch1:          1;           /*The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch1:          1;           /*The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch1:         1;           /*The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch1:          1;           /*The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch1:          1;           /*The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch1:          1;           /*The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch1:          1;           /*The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch1:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch1:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch1:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch1:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_ena_ch1;
    union {
        struct {
            uint32_t in_done_ch2:                1;           /*The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch2:             1;           /*The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch2:             1;           /*The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch2:               1;           /*The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch2:                1;           /*The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch2:            1;           /*The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch2:           1;           /*The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch2:          1;           /*The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch2:          1;           /*The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch2:         1;           /*The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch2:          1;           /*The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch2:          1;           /*The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch2:          1;           /*The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch2:          1;           /*The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch2:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch2:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch2:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch2:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_ena_ch2;
    union {
        struct {
            uint32_t in_done_ch3:                1;           /*The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch3:             1;           /*The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch3:             1;           /*The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch3:               1;           /*The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch3:                1;           /*The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch3:            1;           /*The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch3:           1;           /*The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch3:          1;           /*The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch3:          1;           /*The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch3:         1;           /*The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch3:          1;           /*The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch3:          1;           /*The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch3:          1;           /*The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch3:          1;           /*The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch3:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch3:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch3:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch3:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_ena_ch3;
    union {
        struct {
            uint32_t in_done_ch4:                1;           /*The interrupt enable bit for the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch4:             1;           /*The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch4:             1;           /*The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch4:               1;           /*The interrupt enable bit for the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch4:                1;           /*The interrupt enable bit for the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch4:            1;           /*The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch4:           1;           /*The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch4:          1;           /*The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch4:          1;           /*The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch4:         1;           /*The interrupt enable bit for the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch4:          1;           /*The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch4:          1;           /*The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch4:          1;           /*The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch4:          1;           /*The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch4:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch4:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch4:         1;           /*The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch4:         1;           /*The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_ena_ch4;
    union {
        struct {
            uint32_t in_done_ch0:                1;           /*Set this bit to clear the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch0:             1;           /*Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch0:             1;           /*Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch0:               1;           /*Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch0:                1;           /*Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch0:            1;           /*Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch0:           1;           /*Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch0:          1;           /*Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch0:          1;           /*Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch0:         1;           /*Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch0:          1;           /*Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch0:          1;           /*Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch0:          1;           /*Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch0:          1;           /*Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch0:         1;           /*Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch0:         1;           /*Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch0:         1;           /*Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch0:         1;           /*Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_clr_ch0;
    union {
        struct {
            uint32_t in_done_ch1:                1;           /*Set this bit to clear the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch1:             1;           /*Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch1:             1;           /*Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch1:               1;           /*Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch1:                1;           /*Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch1:            1;           /*Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch1:           1;           /*Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch1:          1;           /*Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch1:          1;           /*Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch1:         1;           /*Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch1:          1;           /*Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch1:          1;           /*Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch1:          1;           /*Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch1:          1;           /*Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch1:         1;           /*Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch1:         1;           /*Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch1:         1;           /*Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch1:         1;           /*Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_clr_ch1;
    union {
        struct {
            uint32_t in_done_ch2:                1;           /*Set this bit to clear the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch2:             1;           /*Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch2:             1;           /*Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch2:               1;           /*Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch2:                1;           /*Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch2:            1;           /*Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch2:           1;           /*Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch2:          1;           /*Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch2:          1;           /*Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch2:         1;           /*Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch2:          1;           /*Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch2:          1;           /*Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch2:          1;           /*Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch2:          1;           /*Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch2:         1;           /*Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch2:         1;           /*Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch2:         1;           /*Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch2:         1;           /*Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_clr_ch2;
    union {
        struct {
            uint32_t in_done_ch3:                1;           /*Set this bit to clear the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch3:             1;           /*Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch3:             1;           /*Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch3:               1;           /*Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch3:                1;           /*Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch3:            1;           /*Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch3:           1;           /*Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch3:          1;           /*Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch3:          1;           /*Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch3:         1;           /*Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch3:          1;           /*Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch3:          1;           /*Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch3:          1;           /*Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch3:          1;           /*Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch3:         1;           /*Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch3:         1;           /*Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch3:         1;           /*Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch3:         1;           /*Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_clr_ch3;
    union {
        struct {
            uint32_t in_done_ch4:                1;           /*Set this bit to clear the IN_DONE_CH_INT interrupt.*/
            uint32_t in_suc_eof_ch4:             1;           /*Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.*/
            uint32_t in_err_eof_ch4:             1;           /*Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.*/
            uint32_t out_done_ch4:               1;           /*Set this bit to clear the OUT_DONE_CH_INT interrupt.*/
            uint32_t out_eof_ch4:                1;           /*Set this bit to clear the OUT_EOF_CH_INT interrupt.*/
            uint32_t in_dscr_err_ch4:            1;           /*Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.*/
            uint32_t out_dscr_err_ch4:           1;           /*Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.*/
            uint32_t in_dscr_empty_ch4:          1;           /*Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.*/
            uint32_t out_total_eof_ch4:          1;           /*Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.*/
            uint32_t infifo_full_wm_ch4:         1;           /*Set this bit to clear the INFIFO_FULL_WM_CH_INT interrupt.*/
            uint32_t infifo_ovf_l1_ch4:          1;           /*Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t infifo_udf_l1_ch4:          1;           /*Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t infifo_ovf_l3_ch4:          1;           /*Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t infifo_udf_l3_ch4:          1;           /*Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l1_ch4:         1;           /*Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.*/
            uint32_t outfifo_udf_l1_ch4:         1;           /*Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.*/
            uint32_t outfifo_ovf_l3_ch4:         1;           /*Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.*/
            uint32_t outfifo_udf_l3_ch4:         1;           /*Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.*/
            uint32_t reserved18:                14;           /*reserved*/
        };
        uint32_t val;
    } int_clr_ch4;
    union {
        struct {
            uint32_t infifo_full_l1_ch0:        1;            /*L1 Rx FIFO full signal for Rx channel 0.*/
            uint32_t infifo_empty_l1_ch0:       1;            /*L1 Rx FIFO empty signal for Rx channel 0.*/
            uint32_t infifo_full_l2_ch0:        1;            /*L2 Rx FIFO full signal for Rx channel 0.*/
            uint32_t infifo_empty_l2_ch0:       1;            /*L2 Rx FIFO empty signal for Rx channel 0.*/
            uint32_t infifo_full_l3_ch0:        1;            /*L3 Rx FIFO full signal for Rx channel 0.*/
            uint32_t infifo_empty_l3_ch0:       1;            /*L3 Rx FIFO empty signal for Rx channel 0.*/
            uint32_t infifo_cnt_l1_ch0:         5;            /*The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0.*/
            uint32_t infifo_cnt_l2_ch0:         7;            /*The register stores the byte number of the data in L2 Rx FIFO for Rx channel 0.*/
            uint32_t infifo_cnt_l3_ch0:         5;            /*The register stores the byte number of the data in L3 Rx FIFO for Rx channel 0.*/
            uint32_t in_remain_under_1b_l3_ch0: 1;            /*reserved*/
            uint32_t in_remain_under_2b_l3_ch0: 1;            /*reserved*/
            uint32_t in_remain_under_3b_l3_ch0: 1;            /*reserved*/
            uint32_t in_remain_under_4b_l3_ch0: 1;            /*reserved*/
            uint32_t in_buf_hungry_ch0:         1;            /*reserved*/
            uint32_t reserved28:                4;            /*reserved*/
        };
        uint32_t val;
    } infifo_status_ch0;
    union {
        struct {
            uint32_t infifo_full_l1_ch1:        1;            /*L1 Rx FIFO full signal for Rx channel 1.*/
            uint32_t infifo_empty_l1_ch1:       1;            /*L1 Rx FIFO empty signal for Rx channel 1.*/
            uint32_t infifo_full_l2_ch1:        1;            /*L2 Rx FIFO full signal for Rx channel 1.*/
            uint32_t infifo_empty_l2_ch1:       1;            /*L2 Rx FIFO empty signal for Rx channel 1.*/
            uint32_t infifo_full_l3_ch1:        1;            /*L3 Rx FIFO full signal for Rx channel 1.*/
            uint32_t infifo_empty_l3_ch1:       1;            /*L3 Rx FIFO empty signal for Rx channel 1.*/
            uint32_t infifo_cnt_l1_ch1:         5;            /*The register stores the byte number of the data in L1 Rx FIFO for Rx channel 1.*/
            uint32_t infifo_cnt_l2_ch1:         7;            /*The register stores the byte number of the data in L2 Rx FIFO for Rx channel 1.*/
            uint32_t infifo_cnt_l3_ch1:         5;            /*The register stores the byte number of the data in L3 Rx FIFO for Rx channel 1.*/
            uint32_t in_remain_under_1b_l3_ch1: 1;            /*reserved*/
            uint32_t in_remain_under_2b_l3_ch1: 1;            /*reserved*/
            uint32_t in_remain_under_3b_l3_ch1: 1;            /*reserved*/
            uint32_t in_remain_under_4b_l3_ch1: 1;            /*reserved*/
            uint32_t in_buf_hungry_ch1:         1;            /*reserved*/
            uint32_t reserved28:                4;            /*reserved*/
        };
        uint32_t val;
    } infifo_status_ch1;
    union {
        struct {
            uint32_t infifo_full_l1_ch2:        1;            /*L1 Rx FIFO full signal for Rx channel 2.*/
            uint32_t infifo_empty_l1_ch2:       1;            /*L1 Rx FIFO empty signal for Rx channel 2.*/
            uint32_t infifo_full_l2_ch2:        1;            /*L2 Rx FIFO full signal for Rx channel 2.*/
            uint32_t infifo_empty_l2_ch2:       1;            /*L2 Rx FIFO empty signal for Rx channel 2.*/
            uint32_t infifo_full_l3_ch2:        1;            /*L3 Rx FIFO full signal for Rx channel 2.*/
            uint32_t infifo_empty_l3_ch2:       1;            /*L3 Rx FIFO empty signal for Rx channel 2.*/
            uint32_t infifo_cnt_l1_ch2:         5;            /*The register stores the byte number of the data in L1 Rx FIFO for Rx channel 2.*/
            uint32_t infifo_cnt_l2_ch2:         7;            /*The register stores the byte number of the data in L2 Rx FIFO for Rx channel 2.*/
            uint32_t infifo_cnt_l3_ch2:         5;            /*The register stores the byte number of the data in L3 Rx FIFO for Rx channel 2.*/
            uint32_t in_remain_under_1b_l3_ch2: 1;            /*reserved*/
            uint32_t in_remain_under_2b_l3_ch2: 1;            /*reserved*/
            uint32_t in_remain_under_3b_l3_ch2: 1;            /*reserved*/
            uint32_t in_remain_under_4b_l3_ch2: 1;            /*reserved*/
            uint32_t in_buf_hungry_ch2:         1;            /*reserved*/
            uint32_t reserved28:                4;            /*reserved*/
        };
        uint32_t val;
    } infifo_status_ch2;
    union {
        struct {
            uint32_t infifo_full_l1_ch3:        1;            /*L1 Rx FIFO full signal for Rx channel 3.*/
            uint32_t infifo_empty_l1_ch3:       1;            /*L1 Rx FIFO empty signal for Rx channel 3.*/
            uint32_t infifo_full_l2_ch3:        1;            /*L2 Rx FIFO full signal for Rx channel 3.*/
            uint32_t infifo_empty_l2_ch3:       1;            /*L2 Rx FIFO empty signal for Rx channel 3.*/
            uint32_t infifo_full_l3_ch3:        1;            /*L3 Rx FIFO full signal for Rx channel 3.*/
            uint32_t infifo_empty_l3_ch3:       1;            /*L3 Rx FIFO empty signal for Rx channel 3.*/
            uint32_t infifo_cnt_l1_ch3:         5;            /*The register stores the byte number of the data in L1 Rx FIFO for Rx channel 3.*/
            uint32_t infifo_cnt_l2_ch3:         7;            /*The register stores the byte number of the data in L2 Rx FIFO for Rx channel 3.*/
            uint32_t infifo_cnt_l3_ch3:         5;            /*The register stores the byte number of the data in L3 Rx FIFO for Rx channel 3.*/
            uint32_t in_remain_under_1b_l3_ch3: 1;            /*reserved*/
            uint32_t in_remain_under_2b_l3_ch3: 1;            /*reserved*/
            uint32_t in_remain_under_3b_l3_ch3: 1;            /*reserved*/
            uint32_t in_remain_under_4b_l3_ch3: 1;            /*reserved*/
            uint32_t in_buf_hungry_ch3:         1;            /*reserved*/
            uint32_t reserved28:                4;            /*reserved*/
        };
        uint32_t val;
    } infifo_status_ch3;
    union {
        struct {
            uint32_t infifo_full_l1_ch4:        1;            /*L1 Rx FIFO full signal for Rx channel 4.*/
            uint32_t infifo_empty_l1_ch4:       1;            /*L1 Rx FIFO empty signal for Rx channel 4.*/
            uint32_t infifo_full_l2_ch4:        1;            /*L2 Rx FIFO full signal for Rx channel 4.*/
            uint32_t infifo_empty_l2_ch4:       1;            /*L2 Rx FIFO empty signal for Rx channel 4.*/
            uint32_t infifo_full_l3_ch4:        1;            /*L3 Rx FIFO full signal for Rx channel 4.*/
            uint32_t infifo_empty_l3_ch4:       1;            /*L3 Rx FIFO empty signal for Rx channel 4.*/
            uint32_t infifo_cnt_l1_ch4:         5;            /*The register stores the byte number of the data in L1 Rx FIFO for Rx channel 4.*/
            uint32_t infifo_cnt_l2_ch4:         7;            /*The register stores the byte number of the data in L2 Rx FIFO for Rx channel 4.*/
            uint32_t infifo_cnt_l3_ch4:         5;            /*The register stores the byte number of the data in L3 Rx FIFO for Rx channel 4.*/
            uint32_t in_remain_under_1b_l3_ch4: 1;            /*reserved*/
            uint32_t in_remain_under_2b_l3_ch4: 1;            /*reserved*/
            uint32_t in_remain_under_3b_l3_ch4: 1;            /*reserved*/
            uint32_t in_remain_under_4b_l3_ch4: 1;            /*reserved*/
            uint32_t in_buf_hungry_ch4:         1;            /*reserved*/
            uint32_t reserved28:                4;            /*reserved*/
        };
        uint32_t val;
    } infifo_status_ch4;
    union {
        struct {
            uint32_t outfifo_full_l1_ch0:        1;           /*L1 Tx FIFO full signal for Tx channel 0.*/
            uint32_t outfifo_empty_l1_ch0:       1;           /*L1 Tx FIFO empty signal for Tx channel 0.*/
            uint32_t outfifo_full_l2_ch0:        1;           /*L2 Tx FIFO full signal for Tx channel 0.*/
            uint32_t outfifo_empty_l2_ch0:       1;           /*L2 Tx FIFO empty signal for Tx channel 0.*/
            uint32_t outfifo_full_l3_ch0:        1;           /*L3 Tx FIFO full signal for Tx channel 0.*/
            uint32_t outfifo_empty_l3_ch0:       1;           /*L3 Tx FIFO empty signal for Tx channel 0.*/
            uint32_t outfifo_cnt_l1_ch0:         5;           /*The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0.*/
            uint32_t outfifo_cnt_l2_ch0:         7;           /*The register stores the byte number of the data in L2 Tx FIFO for Tx channel 0.*/
            uint32_t outfifo_cnt_l3_ch0:         5;           /*The register stores the byte number of the data in L3 Tx FIFO for Tx channel 0.*/
            uint32_t out_remain_under_1b_l3_ch0: 1;           /*reserved*/
            uint32_t out_remain_under_2b_l3_ch0: 1;           /*reserved*/
            uint32_t out_remain_under_3b_l3_ch0: 1;           /*reserved*/
            uint32_t out_remain_under_4b_l3_ch0: 1;           /*reserved*/
            uint32_t reserved27:                 5;           /*reserved*/
        };
        uint32_t val;
    } outfifo_status_ch0;
    union {
        struct {
            uint32_t outfifo_full_l1_ch1:        1;           /*L1 Tx FIFO full signal for Tx channel 1.*/
            uint32_t outfifo_empty_l1_ch1:       1;           /*L1 Tx FIFO empty signal for Tx channel 1.*/
            uint32_t outfifo_full_l2_ch1:        1;           /*L2 Tx FIFO full signal for Tx channel 1.*/
            uint32_t outfifo_empty_l2_ch1:       1;           /*L2 Tx FIFO empty signal for Tx channel 1.*/
            uint32_t outfifo_full_l3_ch1:        1;           /*L3 Tx FIFO full signal for Tx channel 1.*/
            uint32_t outfifo_empty_l3_ch1:       1;           /*L3 Tx FIFO empty signal for Tx channel 1.*/
            uint32_t outfifo_cnt_l1_ch1:         5;           /*The register stores the byte number of the data in L1 Tx FIFO for Tx channel 1.*/
            uint32_t outfifo_cnt_l2_ch1:         7;           /*The register stores the byte number of the data in L2 Tx FIFO for Tx channel 1.*/
            uint32_t outfifo_cnt_l3_ch1:         5;           /*The register stores the byte number of the data in L3 Tx FIFO for Tx channel 1.*/
            uint32_t out_remain_under_1b_l3_ch1: 1;           /*reserved*/
            uint32_t out_remain_under_2b_l3_ch1: 1;           /*reserved*/
            uint32_t out_remain_under_3b_l3_ch1: 1;           /*reserved*/
            uint32_t out_remain_under_4b_l3_ch1: 1;           /*reserved*/
            uint32_t reserved27:                 5;           /*reserved*/
        };
        uint32_t val;
    } outfifo_status_ch1;
    union {
        struct {
            uint32_t outfifo_full_l1_ch2:        1;           /*L1 Tx FIFO full signal for Tx channel 2.*/
            uint32_t outfifo_empty_l1_ch2:       1;           /*L1 Tx FIFO empty signal for Tx channel 2.*/
            uint32_t outfifo_full_l2_ch2:        1;           /*L2 Tx FIFO full signal for Tx channel 2.*/
            uint32_t outfifo_empty_l2_ch2:       1;           /*L2 Tx FIFO empty signal for Tx channel 2.*/
            uint32_t outfifo_full_l3_ch2:        1;           /*L3 Tx FIFO full signal for Tx channel 2.*/
            uint32_t outfifo_empty_l3_ch2:       1;           /*L3 Tx FIFO empty signal for Tx channel 2.*/
            uint32_t outfifo_cnt_l1_ch2:         5;           /*The register stores the byte number of the data in L1 Tx FIFO for Tx channel 2.*/
            uint32_t outfifo_cnt_l2_ch2:         7;           /*The register stores the byte number of the data in L2 Tx FIFO for Tx channel 2.*/
            uint32_t outfifo_cnt_l3_ch2:         5;           /*The register stores the byte number of the data in L3 Tx FIFO for Tx channel 2.*/
            uint32_t out_remain_under_1b_l3_ch2: 1;           /*reserved*/
            uint32_t out_remain_under_2b_l3_ch2: 1;           /*reserved*/
            uint32_t out_remain_under_3b_l3_ch2: 1;           /*reserved*/
            uint32_t out_remain_under_4b_l3_ch2: 1;           /*reserved*/
            uint32_t reserved27:                 5;           /*reserved*/
        };
        uint32_t val;
    } outfifo_status_ch2;
    union {
        struct {
            uint32_t outfifo_full_l1_ch3:        1;           /*L1 Tx FIFO full signal for Tx channel 3.*/
            uint32_t outfifo_empty_l1_ch3:       1;           /*L1 Tx FIFO empty signal for Tx channel 3.*/
            uint32_t outfifo_full_l2_ch3:        1;           /*L2 Tx FIFO full signal for Tx channel 3.*/
            uint32_t outfifo_empty_l2_ch3:       1;           /*L2 Tx FIFO empty signal for Tx channel 3.*/
            uint32_t outfifo_full_l3_ch3:        1;           /*L3 Tx FIFO full signal for Tx channel 3.*/
            uint32_t outfifo_empty_l3_ch3:       1;           /*L3 Tx FIFO empty signal for Tx channel 3.*/
            uint32_t outfifo_cnt_l1_ch3:         5;           /*The register stores the byte number of the data in L1 Tx FIFO for Tx channel 3.*/
            uint32_t outfifo_cnt_l2_ch3:         7;           /*The register stores the byte number of the data in L2 Tx FIFO for Tx channel 3.*/
            uint32_t outfifo_cnt_l3_ch3:         5;           /*The register stores the byte number of the data in L3 Tx FIFO for Tx channel 3.*/
            uint32_t out_remain_under_1b_l3_ch3: 1;           /*reserved*/
            uint32_t out_remain_under_2b_l3_ch3: 1;           /*reserved*/
            uint32_t out_remain_under_3b_l3_ch3: 1;           /*reserved*/
            uint32_t out_remain_under_4b_l3_ch3: 1;           /*reserved*/
            uint32_t reserved27:                 5;           /*reserved*/
        };
        uint32_t val;
    } outfifo_status_ch3;
    union {
        struct {
            uint32_t outfifo_full_l1_ch4:        1;           /*L1 Tx FIFO full signal for Tx channel 4.*/
            uint32_t outfifo_empty_l1_ch4:       1;           /*L1 Tx FIFO empty signal for Tx channel 4.*/
            uint32_t outfifo_full_l2_ch4:        1;           /*L2 Tx FIFO full signal for Tx channel 4.*/
            uint32_t outfifo_empty_l2_ch4:       1;           /*L2 Tx FIFO empty signal for Tx channel 4.*/
            uint32_t outfifo_full_l3_ch4:        1;           /*L3 Tx FIFO full signal for Tx channel 4.*/
            uint32_t outfifo_empty_l3_ch4:       1;           /*L3 Tx FIFO empty signal for Tx channel 4.*/
            uint32_t outfifo_cnt_l1_ch4:         5;           /*The register stores the byte number of the data in L1 Tx FIFO for Tx channel 4.*/
            uint32_t outfifo_cnt_l2_ch4:         7;           /*The register stores the byte number of the data in L2 Tx FIFO for Tx channel 4.*/
            uint32_t outfifo_cnt_l3_ch4:         5;           /*The register stores the byte number of the data in L3 Tx FIFO for Tx channel 4.*/
            uint32_t out_remain_under_1b_l3_ch4: 1;           /*reserved*/
            uint32_t out_remain_under_2b_l3_ch4: 1;           /*reserved*/
            uint32_t out_remain_under_3b_l3_ch4: 1;           /*reserved*/
            uint32_t out_remain_under_4b_l3_ch4: 1;           /*reserved*/
            uint32_t reserved27:                 5;           /*reserved*/
        };
        uint32_t val;
    } outfifo_status_ch4;
    union {
        struct {
            uint32_t outfifo_wdata_ch0: 9;                    /*This register stores the data that need to be pushed into DMA FIFO.*/
            uint32_t outfifo_push_ch0:  1;                    /*Set this bit to push data into DMA FIFO.*/
            uint32_t reserved10:       22;                    /*reserved*/
        };
        uint32_t val;
    } out_push_ch0;
    union {
        struct {
            uint32_t outfifo_wdata_ch1: 9;                    /*This register stores the data that need to be pushed into DMA FIFO.*/
            uint32_t outfifo_push_ch1:  1;                    /*Set this bit to push data into DMA FIFO.*/
            uint32_t reserved10:       22;                    /*reserved*/
        };
        uint32_t val;
    } out_push_ch1;
    union {
        struct {
            uint32_t outfifo_wdata_ch2: 9;                    /*This register stores the data that need to be pushed into DMA FIFO.*/
            uint32_t outfifo_push_ch2:  1;                    /*Set this bit to push data into DMA FIFO.*/
            uint32_t reserved10:       22;                    /*reserved*/
        };
        uint32_t val;
    } out_push_ch2;
    union {
        struct {
            uint32_t outfifo_wdata_ch3: 9;                    /*This register stores the data that need to be pushed into DMA FIFO.*/
            uint32_t outfifo_push_ch3:  1;                    /*Set this bit to push data into DMA FIFO.*/
            uint32_t reserved10:       22;                    /*reserved*/
        };
        uint32_t val;
    } out_push_ch3;
    union {
        struct {
            uint32_t outfifo_wdata_ch4: 9;                    /*This register stores the data that need to be pushed into DMA FIFO.*/
            uint32_t outfifo_push_ch4:  1;                    /*Set this bit to push data into DMA FIFO.*/
            uint32_t reserved10:       22;                    /*reserved*/
        };
        uint32_t val;
    } out_push_ch4;
    union {
        struct {
            uint32_t infifo_rdata_ch0: 12;                    /*This register stores the data popping from DMA FIFO.*/
            uint32_t infifo_pop_ch0:   1;                     /*Set this bit to pop data from DMA FIFO.*/
            uint32_t reserved13:      19;                     /*reserved*/
        };
        uint32_t val;
    } in_pop_ch0;
    union {
        struct {
            uint32_t infifo_rdata_ch1: 12;                    /*This register stores the data popping from DMA FIFO.*/
            uint32_t infifo_pop_ch1:   1;                     /*Set this bit to pop data from DMA FIFO.*/
            uint32_t reserved13:      19;                     /*reserved*/
        };
        uint32_t val;
    } in_pop_ch1;
    union {
        struct {
            uint32_t infifo_rdata_ch2: 12;                    /*This register stores the data popping from DMA FIFO.*/
            uint32_t infifo_pop_ch2:   1;                     /*Set this bit to pop data from DMA FIFO.*/
            uint32_t reserved13:      19;                     /*reserved*/
        };
        uint32_t val;
    } in_pop_ch2;
    union {
        struct {
            uint32_t infifo_rdata_ch3: 12;                    /*This register stores the data popping from DMA FIFO.*/
            uint32_t infifo_pop_ch3:   1;                     /*Set this bit to pop data from DMA FIFO.*/
            uint32_t reserved13:      19;                     /*reserved*/
        };
        uint32_t val;
    } in_pop_ch3;
    union {
        struct {
            uint32_t infifo_rdata_ch4: 12;                    /*This register stores the data popping from DMA FIFO.*/
            uint32_t infifo_pop_ch4:   1;                     /*Set this bit to pop data from DMA FIFO.*/
            uint32_t reserved13:      19;                     /*reserved*/
        };
        uint32_t val;
    } in_pop_ch4;
    union {
        struct {
            uint32_t outlink_addr_ch0:   20;                  /*This register stores the 20 least significant bits of the first outlink descriptor's address.*/
            uint32_t outlink_stop_ch0:    1;                  /*Set this bit to stop dealing with the outlink descriptors.*/
            uint32_t outlink_start_ch0:   1;                  /*Set this bit to start dealing with the outlink descriptors.*/
            uint32_t outlink_restart_ch0: 1;                  /*Set this bit to restart a new outlink from the last address.*/
            uint32_t outlink_park_ch0:    1;                  /*1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM is working.*/
            uint32_t reserved24:          8;
        };
        uint32_t val;
    } out_link_ch0;
    union {
        struct {
            uint32_t outlink_addr_ch1:   20;                  /*This register stores the 20 least significant bits of the first outlink descriptor's address.*/
            uint32_t outlink_stop_ch1:    1;                  /*Set this bit to stop dealing with the outlink descriptors.*/
            uint32_t outlink_start_ch1:   1;                  /*Set this bit to start dealing with the outlink descriptors.*/
            uint32_t outlink_restart_ch1: 1;                  /*Set this bit to restart a new outlink from the last address.*/
            uint32_t outlink_park_ch1:    1;                  /*1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM is working.*/
            uint32_t reserved24:          8;
        };
        uint32_t val;
    } out_link_ch1;
    union {
        struct {
            uint32_t outlink_addr_ch2:   20;                  /*This register stores the 20 least significant bits of the first outlink descriptor's address.*/
            uint32_t outlink_stop_ch2:    1;                  /*Set this bit to stop dealing with the outlink descriptors.*/
            uint32_t outlink_start_ch2:   1;                  /*Set this bit to start dealing with the outlink descriptors.*/
            uint32_t outlink_restart_ch2: 1;                  /*Set this bit to restart a new outlink from the last address.*/
            uint32_t outlink_park_ch2:    1;                  /*1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM is working.*/
            uint32_t reserved24:          8;
        };
        uint32_t val;
    } out_link_ch2;
    union {
        struct {
            uint32_t outlink_addr_ch3:   20;                  /*This register stores the 20 least significant bits of the first outlink descriptor's address.*/
            uint32_t outlink_stop_ch3:    1;                  /*Set this bit to stop dealing with the outlink descriptors.*/
            uint32_t outlink_start_ch3:   1;                  /*Set this bit to start dealing with the outlink descriptors.*/
            uint32_t outlink_restart_ch3: 1;                  /*Set this bit to restart a new outlink from the last address.*/
            uint32_t outlink_park_ch3:    1;                  /*1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM is working.*/
            uint32_t reserved24:          8;
        };
        uint32_t val;
    } out_link_ch3;
    union {
        struct {
            uint32_t outlink_addr_ch4:   20;                  /*This register stores the 20 least significant bits of the first outlink descriptor's address.*/
            uint32_t outlink_stop_ch4:    1;                  /*Set this bit to stop dealing with the outlink descriptors.*/
            uint32_t outlink_start_ch4:   1;                  /*Set this bit to start dealing with the outlink descriptors.*/
            uint32_t outlink_restart_ch4: 1;                  /*Set this bit to restart a new outlink from the last address.*/
            uint32_t outlink_park_ch4:    1;                  /*1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM is working.*/
            uint32_t reserved24:          8;
        };
        uint32_t val;
    } out_link_ch4;
    union {
        struct {
            uint32_t inlink_addr_ch0:    20;                  /*This register stores the 20 least significant bits of the first inlink descriptor's address.*/
            uint32_t inlink_auto_ret_ch0: 1;                  /*Set this bit to return to current inlink descriptor's address  when there are some errors in current receiving data.*/
            uint32_t inlink_stop_ch0:     1;                  /*Set this bit to stop dealing with the inlink descriptors.*/
            uint32_t inlink_start_ch0:    1;                  /*Set this bit to start dealing with the inlink descriptors.*/
            uint32_t inlink_restart_ch0:  1;                  /*Set this bit to mount a new inlink descriptor.*/
            uint32_t inlink_park_ch0:     1;                  /*1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is working.*/
            uint32_t reserved25:          7;
        };
        uint32_t val;
    } in_link_ch0;
    union {
        struct {
            uint32_t inlink_addr_ch1:    20;                  /*This register stores the 20 least significant bits of the first inlink descriptor's address.*/
            uint32_t inlink_auto_ret_ch1: 1;                  /*Set this bit to return to current inlink descriptor's address  when there are some errors in current receiving data.*/
            uint32_t inlink_stop_ch1:     1;                  /*Set this bit to stop dealing with the inlink descriptors.*/
            uint32_t inlink_start_ch1:    1;                  /*Set this bit to start dealing with the inlink descriptors.*/
            uint32_t inlink_restart_ch1:  1;                  /*Set this bit to mount a new inlink descriptor.*/
            uint32_t inlink_park_ch1:     1;                  /*1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is working.*/
            uint32_t reserved25:          7;
        };
        uint32_t val;
    } in_link_ch1;
    union {
        struct {
            uint32_t inlink_addr_ch2:    20;                  /*This register stores the 20 least significant bits of the first inlink descriptor's address.*/
            uint32_t inlink_auto_ret_ch2: 1;                  /*Set this bit to return to current inlink descriptor's address  when there are some errors in current receiving data.*/
            uint32_t inlink_stop_ch2:     1;                  /*Set this bit to stop dealing with the inlink descriptors.*/
            uint32_t inlink_start_ch2:    1;                  /*Set this bit to start dealing with the inlink descriptors.*/
            uint32_t inlink_restart_ch2:  1;                  /*Set this bit to mount a new inlink descriptor.*/
            uint32_t inlink_park_ch2:     1;                  /*1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is working.*/
            uint32_t reserved25:          7;
        };
        uint32_t val;
    } in_link_ch2;
    union {
        struct {
            uint32_t inlink_addr_ch3:    20;                  /*This register stores the 20 least significant bits of the first inlink descriptor's address.*/
            uint32_t inlink_auto_ret_ch3: 1;                  /*Set this bit to return to current inlink descriptor's address  when there are some errors in current receiving data.*/
            uint32_t inlink_stop_ch3:     1;                  /*Set this bit to stop dealing with the inlink descriptors.*/
            uint32_t inlink_start_ch3:    1;                  /*Set this bit to start dealing with the inlink descriptors.*/
            uint32_t inlink_restart_ch3:  1;                  /*Set this bit to mount a new inlink descriptor.*/
            uint32_t inlink_park_ch3:     1;                  /*1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is working.*/
            uint32_t reserved25:          7;
        };
        uint32_t val;
    } in_link_ch3;
    union {
        struct {
            uint32_t inlink_addr_ch4:    20;                  /*This register stores the 20 least significant bits of the first inlink descriptor's address.*/
            uint32_t inlink_auto_ret_ch4: 1;                  /*Set this bit to return to current inlink descriptor's address  when there are some errors in current receiving data.*/
            uint32_t inlink_stop_ch4:     1;                  /*Set this bit to stop dealing with the inlink descriptors.*/
            uint32_t inlink_start_ch4:    1;                  /*Set this bit to start dealing with the inlink descriptors.*/
            uint32_t inlink_restart_ch4:  1;                  /*Set this bit to mount a new inlink descriptor.*/
            uint32_t inlink_park_ch4:     1;                  /*1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is working.*/
            uint32_t reserved25:          7;
        };
        uint32_t val;
    } in_link_ch4;
    union {
        struct {
            uint32_t inlink_dscr_addr_ch0: 18;                /*This register stores the current inlink descriptor's address.*/
            uint32_t in_dscr_state_ch0:    2;                 /*reserved*/
            uint32_t in_state_ch0:         3;                 /*reserved*/
            uint32_t reserved23:           9;                 /*reserved*/
        };
        uint32_t val;
    } in_state_ch0;
    union {
        struct {
            uint32_t inlink_dscr_addr_ch1: 18;                /*This register stores the current inlink descriptor's address.*/
            uint32_t in_dscr_state_ch1:    2;                 /*reserved*/
            uint32_t in_state_ch1:         3;                 /*reserved*/
            uint32_t reserved23:           9;                 /*reserved*/
        };
        uint32_t val;
    } in_state_ch1;
    union {
        struct {
            uint32_t inlink_dscr_addr_ch2: 18;                /*This register stores the current inlink descriptor's address.*/
            uint32_t in_dscr_state_ch2:    2;                 /*reserved*/
            uint32_t in_state_ch2:         3;                 /*reserved*/
            uint32_t reserved23:           9;                 /*reserved*/
        };
        uint32_t val;
    } in_state_ch2;
    union {
        struct {
            uint32_t inlink_dscr_addr_ch3: 18;                /*This register stores the current inlink descriptor's address.*/
            uint32_t in_dscr_state_ch3:    2;                 /*reserved*/
            uint32_t in_state_ch3:         3;                 /*reserved*/
            uint32_t reserved23:           9;                 /*reserved*/
        };
        uint32_t val;
    } in_state_ch3;
    union {
        struct {
            uint32_t inlink_dscr_addr_ch4: 18;                /*This register stores the current inlink descriptor's address.*/
            uint32_t in_dscr_state_ch4:    2;                 /*reserved*/
            uint32_t in_state_ch4:         3;                 /*reserved*/
            uint32_t reserved23:           9;                 /*reserved*/
        };
        uint32_t val;
    } in_state_ch4;
    union {
        struct {
            uint32_t outlink_dscr_addr_ch0: 18;               /*This register stores the current outlink descriptor's address.*/
            uint32_t out_dscr_state_ch0:    2;                /*reserved*/
            uint32_t out_state_ch0:         3;                /*reserved*/
            uint32_t reserved23:            9;                /*reserved*/
        };
        uint32_t val;
    } out_state_ch0;
    union {
        struct {
            uint32_t outlink_dscr_addr_ch1: 18;               /*This register stores the current outlink descriptor's address.*/
            uint32_t out_dscr_state_ch1:    2;                /*reserved*/
            uint32_t out_state_ch1:         3;                /*reserved*/
            uint32_t reserved23:            9;                /*reserved*/
        };
        uint32_t val;
    } out_state_ch1;
    union {
        struct {
            uint32_t outlink_dscr_addr_ch2: 18;               /*This register stores the current outlink descriptor's address.*/
            uint32_t out_dscr_state_ch2:    2;                /*reserved*/
            uint32_t out_state_ch2:         3;                /*reserved*/
            uint32_t reserved23:            9;                /*reserved*/
        };
        uint32_t val;
    } out_state_ch2;
    union {
        struct {
            uint32_t outlink_dscr_addr_ch3: 18;               /*This register stores the current outlink descriptor's address.*/
            uint32_t out_dscr_state_ch3:    2;                /*reserved*/
            uint32_t out_state_ch3:         3;                /*reserved*/
            uint32_t reserved23:            9;                /*reserved*/
        };
        uint32_t val;
    } out_state_ch3;
    union {
        struct {
            uint32_t outlink_dscr_addr_ch4: 18;               /*This register stores the current outlink descriptor's address.*/
            uint32_t out_dscr_state_ch4:    2;                /*reserved*/
            uint32_t out_state_ch4:         3;                /*reserved*/
            uint32_t reserved23:            9;                /*reserved*/
        };
        uint32_t val;
    } out_state_ch4;
    uint32_t out_eof_des_addr_ch0;                            /*This register stores the address of the outlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t out_eof_des_addr_ch1;                            /*This register stores the address of the outlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t out_eof_des_addr_ch2;                            /*This register stores the address of the outlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t out_eof_des_addr_ch3;                            /*This register stores the address of the outlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t out_eof_des_addr_ch4;                            /*This register stores the address of the outlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t in_suc_eof_des_addr_ch0;                         /*This register stores the address of the inlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t in_suc_eof_des_addr_ch1;                         /*This register stores the address of the inlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t in_suc_eof_des_addr_ch2;                         /*This register stores the address of the inlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t in_suc_eof_des_addr_ch3;                         /*This register stores the address of the inlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t in_suc_eof_des_addr_ch4;                         /*This register stores the address of the inlink descriptor when the EOF bit in this descriptor is 1.*/
    uint32_t in_err_eof_des_addr_ch0;                         /*This register stores the address of the inlink descriptor when there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
    uint32_t in_err_eof_des_addr_ch1;                         /*This register stores the address of the inlink descriptor when there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
    uint32_t in_err_eof_des_addr_ch2;                         /*This register stores the address of the inlink descriptor when there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
    uint32_t in_err_eof_des_addr_ch3;                         /*This register stores the address of the inlink descriptor when there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
    uint32_t in_err_eof_des_addr_ch4;                         /*This register stores the address of the inlink descriptor when there are some errors in current receiving data. Only used when peripheral is UHCI0.*/
    uint32_t out_eof_bfr_des_addr_ch0;                        /*This register stores the address of the outlink descriptor before the last outlink descriptor.*/
    uint32_t out_eof_bfr_des_addr_ch1;                        /*This register stores the address of the outlink descriptor before the last outlink descriptor.*/
    uint32_t out_eof_bfr_des_addr_ch2;                        /*This register stores the address of the outlink descriptor before the last outlink descriptor.*/
    uint32_t out_eof_bfr_des_addr_ch3;                        /*This register stores the address of the outlink descriptor before the last outlink descriptor.*/
    uint32_t out_eof_bfr_des_addr_ch4;                        /*This register stores the address of the outlink descriptor before the last outlink descriptor.*/
    union {
        struct {
            uint32_t ahb_testmode: 3;                         /*reserved*/
            uint32_t reserved3:    1;                         /*reserved*/
            uint32_t ahb_testaddr: 2;                         /*reserved*/
            uint32_t reserved6:   26;                         /*reserved*/
        };
        uint32_t val;
    } ahb_test;
    uint32_t in_dscr_ch0;                                     /*The address of the current inlink descriptor x.*/
    uint32_t in_dscr_ch1;                                     /*The address of the current inlink descriptor x.*/
    uint32_t in_dscr_ch2;                                     /*The address of the current inlink descriptor x.*/
    uint32_t in_dscr_ch3;                                     /*The address of the current inlink descriptor x.*/
    uint32_t in_dscr_ch4;                                     /*The address of the current inlink descriptor x.*/
    uint32_t in_dscr_bf0_ch0;                                 /*The address of the last inlink descriptor x-1.*/
    uint32_t in_dscr_bf0_ch1;                                 /*The address of the last inlink descriptor x-1.*/
    uint32_t in_dscr_bf0_ch2;                                 /*The address of the last inlink descriptor x-1.*/
    uint32_t in_dscr_bf0_ch3;                                 /*The address of the last inlink descriptor x-1.*/
    uint32_t in_dscr_bf0_ch4;                                 /*The address of the last inlink descriptor x-1.*/
    uint32_t in_dscr_bf1_ch0;                                 /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t in_dscr_bf1_ch1;                                 /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t in_dscr_bf1_ch2;                                 /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t in_dscr_bf1_ch3;                                 /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t in_dscr_bf1_ch4;                                 /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t out_dscr_ch0;                                    /*The address of the current outlink descriptor y.*/
    uint32_t out_dscr_ch1;                                    /*The address of the current outlink descriptor y.*/
    uint32_t out_dscr_ch2;                                    /*The address of the current outlink descriptor y.*/
    uint32_t out_dscr_ch3;                                    /*The address of the current outlink descriptor y.*/
    uint32_t out_dscr_ch4;                                    /*The address of the current outlink descriptor y.*/
    uint32_t out_dscr_bf0_ch0;                                /*The address of the last outlink descriptor y-1.*/
    uint32_t out_dscr_bf0_ch1;                                /*The address of the last outlink descriptor y-1.*/
    uint32_t out_dscr_bf0_ch2;                                /*The address of the last outlink descriptor y-1.*/
    uint32_t out_dscr_bf0_ch3;                                /*The address of the last outlink descriptor y-1.*/
    uint32_t out_dscr_bf0_ch4;                                /*The address of the last outlink descriptor y-1.*/
    uint32_t out_dscr_bf1_ch0;                                /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t out_dscr_bf1_ch1;                                /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t out_dscr_bf1_ch2;                                /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t out_dscr_bf1_ch3;                                /*The address of the second-to-last inlink descriptor x-2.*/
    uint32_t out_dscr_bf1_ch4;                                /*The address of the second-to-last inlink descriptor x-2.*/
    union {
        struct {
            uint32_t reserved0:    4;                         /*reserved*/
            uint32_t ram_force_pd: 1;                         /*power down*/
            uint32_t ram_force_pu: 1;
            uint32_t ram_clk_fo:   1;                         /*1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA. 0: A gate-clock will be used when accessing the RAM in DMA.*/
            uint32_t reserved7:   25;                         /*reserved*/
        };
        uint32_t val;
    } pd_conf;
    union {
        struct {
            uint32_t tx_weight_ch0: 4;                        /*The weight of Tx channel 0.*/
            uint32_t rx_weight_ch0: 4;                        /*The weight of Rx channel 0.*/
            uint32_t reserved8:    24;
        };
        uint32_t val;
    } wight_ch0;
    union {
        struct {
            uint32_t tx_weight_ch1: 4;                        /*The weight of Tx channel 1.*/
            uint32_t rx_weight_ch1: 4;                        /*The weight of Rx channel 1.*/
            uint32_t reserved8:    24;
        };
        uint32_t val;
    } wight_ch1;
    union {
        struct {
            uint32_t tx_weight_ch2: 4;                        /*The weight of Tx channel 2.*/
            uint32_t rx_weight_ch2: 4;                        /*The weight of Rx channel 2.*/
            uint32_t reserved8:    24;
        };
        uint32_t val;
    } wight_ch2;
    union {
        struct {
            uint32_t tx_weight_ch3: 4;                        /*The weight of Tx channel 3.*/
            uint32_t rx_weight_ch3: 4;                        /*The weight of Rx channel 3.*/
            uint32_t reserved8:    24;
        };
        uint32_t val;
    } wight_ch3;
    union {
        struct {
            uint32_t tx_weight_ch4: 4;                        /*The weight of Tx channel 4.*/
            uint32_t rx_weight_ch4: 4;                        /*The weight of Rx channel 4.*/
            uint32_t reserved8:    24;
        };
        uint32_t val;
    } wight_ch4;
    union {
        struct {
            uint32_t tx_pri_ch0: 4;                           /*The priority of Tx channel 0. The larger of the value  the higher of the priority.*/
            uint32_t rx_pri_ch0: 4;                           /*The priority of Rx channel 0. The larger of the value  the higher of the priority.*/
            uint32_t reserved8: 24;
        };
        uint32_t val;
    } pri_ch0;
    union {
        struct {
            uint32_t tx_pri_ch1: 4;                           /*The priority of Tx channel 1. The larger of the value  the higher of the priority.*/
            uint32_t rx_pri_ch1: 4;                           /*The priority of Rx channel 1. The larger of the value  the higher of the priority.*/
            uint32_t reserved8: 24;
        };
        uint32_t val;
    } pri_ch1;
    union {
        struct {
            uint32_t tx_pri_ch2: 4;                           /*The priority of Tx channel 2. The larger of the value  the higher of the priority.*/
            uint32_t rx_pri_ch2: 4;                           /*The priority of Rx channel 2. The larger of the value  the higher of the priority.*/
            uint32_t reserved8: 24;
        };
        uint32_t val;
    } pri_ch2;
    union {
        struct {
            uint32_t tx_pri_ch3: 4;                           /*The priority of Tx channel 3. The larger of the value  the higher of the priority.*/
            uint32_t rx_pri_ch3: 4;                           /*The priority of Rx channel 3. The larger of the value  the higher of the priority.*/
            uint32_t reserved8: 24;
        };
        uint32_t val;
    } pri_ch3;
    union {
        struct {
            uint32_t tx_pri_ch4: 4;                           /*The priority of Tx channel 4. The larger of the value  the higher of the priority.*/
            uint32_t rx_pri_ch4: 4;                           /*The priority of Rx channel 4. The larger of the value  the higher of the priority.*/
            uint32_t reserved8: 24;
        };
        uint32_t val;
    } pri_ch4;
    union {
        struct {
            uint32_t ahbm_rst_inter: 1;                       /*Set this bit  then clear this bit to reset the internal ahb FSM.*/
            uint32_t ahbm_rst_exter: 1;                       /*Set this bit  then clear this bit to reset the external ahb FSM.*/
            uint32_t arb_pri_dis:    1;                       /*Set this bit to disable priority arbitration function.*/
            uint32_t clk_en:         1;
            uint32_t reserved4:     28;
        };
        uint32_t val;
    } misc_conf;
    union {
        struct {
            uint32_t peri_in_sel_ch0:  6;                     /*This register is used to select peripheral for Rx channel 0. 0:SPI2*/
            uint32_t peri_out_sel_ch0: 6;                     /*This register is used to select peripheral for Tx channel 0. 0:SPI2*/
            uint32_t reserved12:      20;
        };
        uint32_t val;
    } peri_sel_ch0;
    union {
        struct {
            uint32_t peri_in_sel_ch1:  6;                     /*This register is used to select peripheral for Rx channel 1. 0:SPI2*/
            uint32_t peri_out_sel_ch1: 6;                     /*This register is used to select peripheral for Tx channel 1. 0:SPI2*/
            uint32_t reserved12:      20;
        };
        uint32_t val;
    } peri_sel_ch1;
    union {
        struct {
            uint32_t peri_in_sel_ch2:  6;                     /*This register is used to select peripheral for Rx channel 2. 0:SPI2*/
            uint32_t peri_out_sel_ch2: 6;                     /*This register is used to select peripheral for Tx channel 2. 0:SPI2*/
            uint32_t reserved12:      20;
        };
        uint32_t val;
    } peri_sel_ch2;
    union {
        struct {
            uint32_t peri_in_sel_ch3:  6;                     /*This register is used to select peripheral for Rx channel 3. 0:SPI2*/
            uint32_t peri_out_sel_ch3: 6;                     /*This register is used to select peripheral for Tx channel 3. 0:SPI2*/
            uint32_t reserved12:      20;
        };
        uint32_t val;
    } peri_sel_ch3;
    union {
        struct {
            uint32_t peri_in_sel_ch4:  6;                     /*This register is used to select peripheral for Rx channel 4. 0:SPI2*/
            uint32_t peri_out_sel_ch4: 6;                     /*This register is used to select peripheral for Tx channel 4. 0:SPI2*/
            uint32_t reserved12:      20;
        };
        uint32_t val;
    } peri_sel_ch4;
    union {
        struct {
            uint32_t in_size_ch0:  5;                         /*This register is used to configure the size of L2 Rx FIFO for Rx channel 0. 0:16 bytes*/
            uint32_t out_size_ch0: 5;                         /*This register is used to configure the size of L2 Tx FIFO for Tx channel 0. 0:16 bytes*/
            uint32_t reserved10:  22;
        };
        uint32_t val;
    } sram_size_ch0;
    union {
        struct {
            uint32_t in_size_ch1:  5;                         /*This register is used to configure the size of L2 Rx FIFO for Rx channel 1. 0:16 bytes*/
            uint32_t out_size_ch1: 5;                         /*This register is used to configure the size of L2 Tx FIFO for Tx channel 1. 0:16 bytes*/
            uint32_t reserved10:  22;
        };
        uint32_t val;
    } sram_size_ch1;
    union {
        struct {
            uint32_t in_size_ch2:  5;                         /*This register is used to configure the size of L2 Rx FIFO for Rx channel 2. 0:16 bytes*/
            uint32_t out_size_ch2: 5;                         /*This register is used to configure the size of L2 Tx FIFO for Tx channel 2. 0:16 bytes*/
            uint32_t reserved10:  22;
        };
        uint32_t val;
    } sram_size_ch2;
    union {
        struct {
            uint32_t in_size_ch3:  5;                         /*This register is used to configure the size of L2 Rx FIFO for Rx channel 3. 0:16 bytes*/
            uint32_t out_size_ch3: 5;                         /*This register is used to configure the size of L2 Tx FIFO for Tx channel 3. 0:16 bytes*/
            uint32_t reserved10:  22;
        };
        uint32_t val;
    } sram_size_ch3;
    union {
        struct {
            uint32_t in_size_ch4:  5;                         /*This register is used to configure the size of L2 Rx FIFO for Rx channel 4. 0:16 bytes*/
            uint32_t out_size_ch4: 5;                         /*This register is used to configure the size of L2 Tx FIFO for Tx channel 4. 0:16 bytes*/
            uint32_t reserved10:  22;
        };
        uint32_t val;
    } sram_size_ch4;
    uint32_t date;                                            /*register version.*/
} dma_dev_t;

extern dma_dev_t DMA;

#ifdef __cplusplus
}
#endif
