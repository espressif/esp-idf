/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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

typedef struct usb_reg {
    volatile uint32_t gotgctl;              // 0x0000 OTG Control and Status Register
    volatile uint32_t gotgint;              // 0x0004 OTG Interrupt Register
    volatile uint32_t gahbcfg;              // 0x0008 AHB Configuration Register
    volatile uint32_t gusbcfg;              // 0x000c USB Configuration Register
    volatile uint32_t grstctl;              // 0x0010 Reset Register
    volatile uint32_t gintsts;              // 0x0014 Interrupt Register
    volatile uint32_t gintmsk;              // 0x0018 Interrupt Mask Register
    volatile uint32_t grxstsr;              // 0x001c Receive Status Debug Read Register
    volatile uint32_t grxstsp;              // 0x0020 Receive Status Read/Pop Register
    volatile uint32_t grxfsiz;              // 0x0024 Receive FIFO Size Register
    volatile uint32_t gnptxfsiz;            // 0x0028 Non-periodic Transmit FIFO Size Register
    volatile uint32_t gnptxsts;             // 0x002c Non-periodic Transmit FIFO/Queue Status Register
    uint32_t reserved_0x0030_0x0040[4];     // 0x0030 to 0x0040
    volatile uint32_t gsnpsid;              // 0x0040 ID Register
    volatile uint32_t ghwcfg1;              // 0x0044 User Hardware Configuration 1 Register
    volatile uint32_t ghwcfg2;              // 0x0048 User Hardware Configuration 2 Register
    volatile uint32_t ghwcfg3;              // 0x004c User Hardware Configuration 3 Register
    volatile uint32_t ghwcfg4;              // 0x0050 User Hardware Configuration 4 Register
    uint32_t reserved_0x0054_0x005c[2];     // 0x0054 to 0x005c
    volatile uint32_t gdfifocfg;            // 0x005c Global DFIFO Configuration Register
    uint32_t reserved_0x0060_0x0100[40];    // 0x0060 to 0x0100
    volatile uint32_t hptxfsiz;             // 0x0100 Host Periodic Transmit FIFO Size Register
    volatile uint32_t dieptxf[4];           // 0x0104 to 0x0114 Device IN Endpoint Transmit FIFO Size Register i
    uint32_t reserved_0x0114_0x0140[11];    // 0x0114 to 0x0140
    uint32_t reserved_0x0140_0x0400[176];   // 0x0140 to 0x0400
    /**
     * Host mode registers offsets from 0x0400 to 0x07FF
     */
    volatile uint32_t hcfg;                 // 0x0400 Host Configuration Register
    volatile uint32_t hfir;                 // 0x0404 Host Frame Interval Register
    volatile uint32_t hfnum;                // 0x0408 Host Frame Number/Frame Remaining Register
    uint32_t reserved0x40C;                 // 0x040c Reserved
    volatile uint32_t hptxsts;              // 0x0410 Host Periodic Transmit FIFO/ Queue Status Register
    volatile uint32_t haint;                // 0x0414 Host All Channels Interrupt Register
    volatile uint32_t haintmsk;             // 0x0418 Host All Channels Interrupt Mask Register
    volatile uint32_t hflbaddr;             // 0x041c Host Frame List Base Address Register
    uint32_t reserved0x0420_0x0440[8];      // 0x0420 to 0x0440
    volatile uint32_t hprt;                 // 0x0440 Host Port Control and Status Register
    uint32_t reserved_0x0444_0x0500[47];    // 0x0444 to 0x0500
    //Skip over the host channel registers
    volatile uint32_t host_chan_regs[128];  // 0x0500 to 0x0700
    uint32_t reserved_0x0700_0x0800[64];    // 0x0700 to 0x0800
    /**
     * Device mode registers offsets from
     */
    volatile uint32_t dcfg;                 // 0x0800 Device Configuration Register
    volatile uint32_t dctl;                 // 0x0804 Device Control Register
    volatile uint32_t dsts;                 // 0x0808 Device Status Register (Read Only)
    uint32_t reserved0x80c;                 // 0x080c
    volatile uint32_t diepmsk;              // 0x0810 Device IN Endpoint Common Interrupt Mask Register
    volatile uint32_t doepmsk;              // 0x0814 Device OUT Endpoint Common Interrupt Mask Register
    volatile uint32_t daint;                // 0x0818 Device All Endpoints Interrupt Register
    volatile uint32_t daintmsk;             // 0x081c Device All Endpoints Interrupt Mask Register
    uint32_t reserved_0x0820_0x0828[2];     // 0x0820 to 0x0828
    volatile uint32_t dvbusdis;             // 0x0828 Device VBUS discharge Register
    volatile uint32_t dvbuspulse;           // 0x082c Device VBUS Pulse Register
    volatile uint32_t dthrctl;              // 0x0830 Device Thresholding control register (Read/Write)
    volatile uint32_t dtknqr4_fifoemptymsk; // 0x0834 Device IN Endpoint FIFO Empty Interrupt Mask register
    uint32_t reserved_0x0838_0x0900[50];    // 0x0838 to 0x0900
    // Input Endpoints
    usb_in_endpoint_t in_ep_reg[7];         // 0x0900 to 0x09e0 IN EP registers
    uint32_t reserved_0x09e0_0x0b00[72];    // 0x09e0 to 0x0b00
    // Output Endpoints
    usb_out_endpoint_t out_ep_reg[7];       // 0x0b00 to 0x0be0 OUT EP registers
    uint32_t reserved_0x0be0_0x0d00[72];    // 0x0be0 to 0x0d00
    uint32_t reserved_0x0d00_0x0e00[64];    // 0x0d00 to 0x0e00
    /**
     * Power Control and direct FIFO access
     */
    uint32_t pcgctrl;                       // 0x0e00 Power and Clock Gating Control Register
    uint32_t reserved_0x0e04;               // 0x0e04
    uint8_t reserved8[0x1000 - 0xe08];      // 0x0d00 to 0x1000
    uint32_t fifo[16][0x400];               // 0x1000 to 0x2000 Device EP i/Host Channel i FIFO
    uint8_t reserved0x11000[0x20000 - 0x11000];
    uint32_t dbg_fifo[0x20000];             // 0x2000 to 0x22000 Direct Access to Data FIFO RAM for Debugging
} usb_dev_t;

extern usb_dev_t USB0;

#ifdef __cplusplus
}
#endif
