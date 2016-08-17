// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SOC_BB_REG_H_
#define _SOC_BB_REG_H_

#define apb_bb_offset 0x6001c000

#define BB_DLY                 apb_bb_offset + 0x00009b00  // reg 00
#define BB_TEST                 apb_bb_offset + 0x00009b08  // reg 02
#define BB_TM1                 apb_bb_offset + 0x00009b0c  // reg 03
#define BB_TM_CNTL             apb_bb_offset + 0x00009b14  // reg 05
#define BB_DEL_CNTL          apb_bb_offset + 0x00009b28  // reg 10
#define BB_PARAL_CNTL          apb_bb_offset + 0x00009b2c  // reg 11
#define BB_FSM1                 apb_bb_offset + 0x00009b44  // reg 17
#define BB_MXG                 apb_bb_offset + 0x00009b48  // reg 18
#define BB_MNOF                apb_bb_offset + 0x00009b4c  // reg 19
#define BB_SIZE                apb_bb_offset + 0x00009b50  // reg 20
#define BB_TM3a                apb_bb_offset + 0x00009b54  // reg 21
#define BB_TM4a                apb_bb_offset + 0x00009b58  // reg 22
#define BB_GAIN                apb_bb_offset + 0x00009b5c  // reg 23
#define BB_CNTL                apb_bb_offset + 0x00009b60  // reg 24
#define BB_CAD                 apb_bb_offset + 0x00009b64  // reg 25
#define BB_DET                 apb_bb_offset + 0x00009b68  // reg 26
#define BB_DETL                apb_bb_offset + 0x00009b6c  // reg 27

#define BB_MASK_PCLL               apb_bb_offset + 0x00009d08  // reg 66
#define BB_MASK_PCLH               apb_bb_offset + 0x00009d0c  // reg 67
#define BB_RX_CTRL4                apb_bb_offset + 0x00009d10  // reg 68
#define BB_RX_CTRL                apb_bb_offset + 0x00009d1c  // reg 71
#define BB_RX_CTRL2                apb_bb_offset + 0x00009d20  // reg 72
#define BB_RX_CTRL3                apb_bb_offset + 0x00009d24  // reg 73
#define BB_DEL4                 apb_bb_offset + 0x00009d40  // reg 80
#define BB_TM5                 apb_bb_offset + 0x00009d44  // reg 81
#define BB_TM6                 apb_bb_offset + 0x00009d48  // reg 82
#define BB_PMCTRL                 apb_bb_offset + 0x00009d4c  // reg 83
#define BB_PWR                 apb_bb_offset + 0x00009d68  // reg 90
#define BB_BCTRL2                 apb_bb_offset + 0x00009d70  // reg 92

#define BB_MASK_PL               apb_bb_offset + 0x00009884  // reg 97
#define BB_MASK_PCHL               apb_bb_offset + 0x00009888  // reg 98
#define BB_MASK_PCHH               apb_bb_offset + 0x0000988c  // reg 99

#define BB_MASK_CL               apb_bb_offset + 0x0000989c  // reg 103
#define BB_TONE               apb_bb_offset + 0x000098a0  // reg 104
#define BB_MASK_CH               apb_bb_offset + 0x000098d4  // reg 117
#define BB_SER               apb_bb_offset + 0x000098ec  // reg 123
#define BB_GN_TB               apb_bb_offset + 0x00009e00  // reg 128

#define BB_MODE                apb_bb_offset + 0x00009c00  // reg 640
#define BB_TXCTRL               apb_bb_offset + 0x00009c04  // reg 641
#define BB_BCTRL3                apb_bb_offset + 0x00009c08  // reg 642
#define BB_BCTRL                apb_bb_offset + 0x00009c28  // reg 650
#define BB_SMCTRL               apb_bb_offset + 0x00009c48  // reg 658
#define BB_SMCTRL2               apb_bb_offset + 0x00009c4C  // reg 659
#define BB_TXCNT               apb_bb_offset + 0x00009c58  // reg 662
#define BB_RXCTRL               apb_bb_offset + 0x00009c68  // reg 666

#define BB_TXGAIN               apb_bb_offset + 0x00009900  // reg 704

#define BB_RXS_CNTL        apb_bb_offset + 0x00009988  // reg 738
#define BB_MASK2_PCLL               apb_bb_offset + 0x000099a8  // reg 746
#define BB_MASK2_PCLH               apb_bb_offset + 0x000099ac  // reg 747
#define BB_MASK_PH               apb_bb_offset + 0x000099b0  // reg 748
#define BB_MASK2_PCHL               apb_bb_offset + 0x000099b8  // reg 750
#define BB_MASK2_PCHH               apb_bb_offset + 0x000099bc  // reg 751
//
#define BB_TX_TONE_CNTL        apb_bb_offset + 0x000099f0  // reg 764
#define BB_ADD_CNTL0        apb_bb_offset + 0x00009a28  // reg 778
#define BB_ADD_CNTL2        apb_bb_offset + 0x00009a2c  // reg 779
#define BB_GAIN_CNTL0        apb_bb_offset + 0x00009a34  // reg 781
#define BB_GAIN_CNTL1        apb_bb_offset + 0x00009a38  // reg 782
#define BB_GAIN_CNTL2        apb_bb_offset + 0x00009a3c  // reg 783
#define BB_AGCMEM_CTRL         apb_bb_offset + 0x00009a68  // reg 794

#define BB_11B_RECORD          apb_bb_offset + 0x00009808  // reg 802
#define BB_FILTER_CNTL         apb_bb_offset + 0x0000980c  // reg 803
#define BB_ANALOG_CTRL1        apb_bb_offset + 0x00009838
#define BB_ANALOG_CTRL2         apb_bb_offset + 0x0000983c  //reg 815
#define BB_ANALOG_CTRL3         apb_bb_offset + 0x00009840  //reg 816
#define BB_RFCFG_CTRL0          apb_bb_offset + 0x00009844  //reg 817
#define BB_RFCFG_CTRL1          apb_bb_offset + 0x00009848  //reg 818

#define BB_ADD_CNTL1             apb_bb_offset + 0x00009860  //reg824
#define BB_PA_CNTL             apb_bb_offset + 0x00009864  //reg825
#define BB_RFCFG_CTRL2          apb_bb_offset + 0x0000986c  //reg827
#define BB_RXDEL_CTRL        apb_bb_offset + 0x00009d18
#define BB_RXLENGTH_CTRL        apb_bb_offset + 0x00009d1c

#endif /* _SOC_BB_REG_H_ */

