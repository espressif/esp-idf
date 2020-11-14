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

#include <stdint.h>
#include "usb_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct usb_reg {
    volatile uint32_t gotgctl; /*!< 0x0 */
    volatile uint32_t gotgint;
    volatile uint32_t gahbcfg;
    volatile uint32_t gusbcfg;
    volatile uint32_t grstctl; /*!< 0x10 */
    volatile uint32_t gintsts;
    volatile uint32_t gintmsk;
    volatile uint32_t grxstsr;
    volatile uint32_t grxstsp; /*!< 0x20 */
    volatile uint32_t grxfsiz;
    volatile uint32_t gnptxfsiz;
    volatile uint32_t gnptxsts;
    volatile uint32_t reserved0x2c;
    volatile uint32_t gpvndctl; /*!< 0x30 */
    volatile uint32_t ggpio;
    volatile uint32_t guid;
    volatile uint32_t gsnpsid;
    volatile uint32_t ghwcfg1; /*!< 0x40 */
    volatile uint32_t ghwcfg2;
    volatile uint32_t ghwcfg3;
    volatile uint32_t ghwcfg4;   /*!< 0x50 */
    volatile uint32_t glpmcfg;   /*!< 0x54 */
    volatile uint32_t gpwrdn;    /*!< 0x58 */
    volatile uint32_t gdfifocfg; /*!< 0x5c */
    volatile uint32_t gadpctl;   /*!< 0x60 */
    uint32_t reserved0x64[39];
    volatile uint32_t hptxfsiz;    /*!< 0x100 */
    volatile uint32_t dieptxf[15]; /*!< 0x104 */
    uint32_t reserved0x140[176];       /*!<  0x140 */
    /**
     * The Host Global Registers structure defines the size and relative
     * field offsets for the Host Mode Global Registers.  Host Global
     * Registers offsets 400h-7FFh.
    */
    volatile uint32_t hcfg;     /*!< Host Configuration Register.   <i>Offset: 400h</i> */
    volatile uint32_t hfir;     /*!< Host Frame Interval Register.  <i>Offset: 404h</i> */
    volatile uint32_t hfnum;    /*!< Host Frame Number / Frame Remaining Register. <i>Offset: 408h</i> */
    uint32_t reserved0x40C;       /*!< Reserved.  <i>Offset: 40Ch</i> */
    volatile uint32_t hptxsts;  /*!< Host Periodic Transmit FIFO/ Queue Status Register. <i>Offset: 410h</i> */
    volatile uint32_t haint;    /*!< Host All Channels Interrupt Register. <i>Offset: 414h</i> */
    volatile uint32_t haintmsk; /*!< Host All Channels Interrupt Mask Register. <i>Offset: 418h</i> */
    volatile uint32_t hflbaddr; /*!< Host Frame List Base Address Register . <i>Offset: 41Ch</i> */
    uint32_t reserved0x420[7];
    volatile uint32_t hprt; //0x440
    uint32_t reserved0x444[240];
    volatile uint32_t dcfg;                 /*!< Device Configuration Register. <i>Offset 800h</i> */
    volatile uint32_t dctl;                 /*!< Device Control Register. <i>Offset: 804h</i> */
    volatile uint32_t dsts;                 /*!< Device Status Register (Read Only). <i>Offset: 808h</i> */
    uint32_t reserved0x80c;                 /*!< Reserved. <i>Offset: 80Ch</i> */
    volatile uint32_t diepmsk;              /*!< Device IN Endpoint Common Interrupt Mask Register. <i>Offset: 810h</i> */
    volatile uint32_t doepmsk;              /*!< Device OUT Endpoint Common Interrupt Mask Register. <i>Offset: 814h</i> */
    volatile uint32_t daint;                /*!< Device All Endpoints Interrupt Register.  <i>Offset: 818h</i> */
    volatile uint32_t daintmsk;             /*!< Device All Endpoints Interrupt Mask Register.  <i>Offset: 81Ch</i> */
    volatile uint32_t dtknqr1;              /*!< Device IN Token Queue Read Register-1 (Read Only). <i>Offset: 820h</i> */
    volatile uint32_t dtknqr2;              /*!< Device IN Token Queue Read Register-2 (Read Only). <i>Offset: 824h</i> */
    volatile uint32_t dvbusdis;             /*!< Device VBUS     discharge Register.  <i>Offset: 828h</i> */
    volatile uint32_t dvbuspulse;           /*!< Device VBUS Pulse Register.     <i>Offset: 82Ch</i> */
    volatile uint32_t dtknqr3_dthrctl;      /*!< Device IN Token Queue Read Register-3 (Read Only). Device Thresholding control register (Read/Write) <i>Offset: 830h</i> */
    volatile uint32_t dtknqr4_fifoemptymsk; /*!< Device IN Token Queue Read Register-4 (Read Only). Device IN EPs empty Inr. Mask Register (Read/Write)<i>Offset: 834h</i> */
    volatile uint32_t deachint;             /*!< Device Each Endpoint Interrupt Register (Read Only). <i>Offset: 838h</i> */
    volatile uint32_t deachintmsk;          /*!< Device Each Endpoint Interrupt mask Register (Read/Write). <i>Offset: 83Ch</i> */
    volatile uint32_t diepeachintmsk[16];   /*!< Device Each In Endpoint Interrupt mask Register (Read/Write). <i>Offset: 840h</i> */
    volatile uint32_t doepeachintmsk[16];   /*!< Device Each Out Endpoint Interrupt mask Register (Read/Write). <i>Offset: 880h</i> */
    uint32_t reserved0x8c0[16];
    /* Input Endpoints*/
    usb_in_endpoint_t in_ep_reg[USB_IN_EP_NUM]; /*!< 0x900*/
    uint32_t reserved6[72];
    /* Output Endpoints */
    usb_out_endpoint_t out_ep_reg[USB_OUT_EP_NUM];
    uint32_t reserved7[136];
    uint32_t pcgctrl; /*!<0xe00*/
    uint32_t pcgctrl1;
    uint8_t reserved8[0x1000 - 0xe08]; /*!<0xd00*/
    uint32_t fifo[16][0x400];          /*!<0x1000*/
    uint8_t reserved0x11000[0x20000 - 0x11000];
    uint32_t dbg_fifo[0x20000]; /*!< 0x20000*/
} usb_dev_t;

extern usb_dev_t USB0;

#ifdef __cplusplus
}
#endif
