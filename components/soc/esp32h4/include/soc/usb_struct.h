/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* USB IN EP Register block type */
typedef struct usb_in_ep_reg {
    volatile uint32_t diepctl;
    uint32_t reserved;
    volatile uint32_t diepint;
    uint32_t reserved1;
    volatile uint32_t dieptsiz;
    volatile uint32_t diepdma;
    volatile uint32_t dtxfsts;
    uint32_t reserved2;
} usb_in_endpoint_t;

/* USB OUT EP Register block type */
typedef struct usb_out_ep_reg {
    volatile uint32_t doepctl;
    uint32_t reserved;
    volatile uint32_t doepint;
    uint32_t reserved1;
    volatile uint32_t doeptsiz;
    volatile uint32_t doepdma;
    uint32_t reserved2;
    uint32_t reserved3;
} usb_out_endpoint_t;

typedef struct usb_dev_t {
    volatile uint32_t gotgctl;
    volatile uint32_t gotgint;
    volatile uint32_t gahbcfg;
    volatile uint32_t gusbcfg;
    volatile uint32_t grstctl;
    volatile uint32_t gintsts;
    volatile uint32_t gintmsk;
    volatile uint32_t grxstsr;
    volatile uint32_t grxstsp;
    volatile uint32_t grxfsiz;
    volatile uint32_t gnptxfsiz;
    volatile uint32_t gnptxsts;
    uint32_t reserved_0x0030_0x0040[4];
    volatile uint32_t gsnpsid;
    volatile uint32_t ghwcfg1;
    volatile uint32_t ghwcfg2;
    volatile uint32_t ghwcfg3;
    volatile uint32_t ghwcfg4;
    uint32_t reserved_0x0054_0x005c[2];
    volatile uint32_t gdfifocfg;
    uint32_t reserved_0x0060_0x0100[40];
    volatile uint32_t hptxfsiz;
    volatile uint32_t dieptxf[4];
    uint32_t reserved_0x0114_0x0140[11];
    uint32_t reserved_0x0140_0x0400[176];
    volatile uint32_t hcfg;
    volatile uint32_t hfir;
    volatile uint32_t hfnum;
    uint32_t reserved0x40C;
    volatile uint32_t hptxsts;
    volatile uint32_t haint;
    volatile uint32_t haintmsk;
    volatile uint32_t hflbaddr;
    uint32_t reserved0x0420_0x0440[8];
    volatile uint32_t hprt;
    uint32_t reserved_0x0444_0x0500[47];
    volatile uint32_t host_chan_regs[128];
    uint32_t reserved_0x0700_0x0800[64];
    volatile uint32_t dcfg;
    volatile uint32_t dctl;
    volatile uint32_t dsts;
    uint32_t reserved0x80c;
    volatile uint32_t diepmsk;
    volatile uint32_t doepmsk;
    volatile uint32_t daint;
    volatile uint32_t daintmsk;
    uint32_t reserved_0x0820_0x0828[2];
    volatile uint32_t dvbusdis;
    volatile uint32_t dvbuspulse;
    volatile uint32_t dthrctl;
    volatile uint32_t dtknqr4_fifoemptymsk;
    uint32_t reserved_0x0838_0x0900[50];
    usb_in_endpoint_t in_ep_reg[7];
    uint32_t reserved_0x09e0_0x0b00[72];
    usb_out_endpoint_t out_ep_reg[7];
    uint32_t reserved_0x0be0_0x0d00[72];
    uint32_t reserved_0x0d00_0x0e00[64];
    uint32_t pcgctrl;
    uint32_t reserved_0x0e04;
    uint8_t reserved8[0x1000 - 0xe08];
    uint32_t fifo[16][0x400];
    uint8_t reserved0x11000[0x20000 - 0x11000];
    uint32_t dbg_fifo[0x20000];
} usb_dev_t;

extern usb_dev_t USB0;

#ifdef __cplusplus
}
#endif
