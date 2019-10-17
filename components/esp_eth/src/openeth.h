// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
#include <stddef.h>
#include <assert.h>
#include "sdkconfig.h"
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

// These are the register definitions for the OpenCores Ethernet MAC.
// See comments in esp_eth_mac_openeth.c for more details about this driver.

// DMA buffers configuration
#define DMA_BUF_SIZE    1600
#define RX_BUF_COUNT    CONFIG_ETH_OPENETH_DMA_RX_BUFFER_NUM
#define TX_BUF_COUNT    CONFIG_ETH_OPENETH_DMA_TX_BUFFER_NUM

// This driver uses the interrupt source number of the internal EMAC of the ESP32 chip,
// and uses the same register address base. This of course only works in QEMU, where
// the OpenCores MAC is mapped to the same register base and to the same interrupt
// source. This driver does a sanity check that it is not running on the real ESP32
// chip, using the EMAC date register.
#define OPENETH_INTR_SOURCE         ETS_ETH_MAC_INTR_SOURCE
#define OPENETH_BASE                DR_REG_EMAC_BASE

// OpenCores ethmac registers
#define OPENETH_MODER_REG           (OPENETH_BASE + 0x00)
#define OPENETH_MODER_DEFAULT       0xa000
// OPENETH_RST: reset the MAC
#define OPENETH_RST   BIT(11)
// OPENETH_PRO: enable promiscuous mode
#define OPENETH_PRO   BIT(5)
// OPENETH_TXEN: enable transmit
#define OPENETH_TXEN   BIT(1)
// OPENETH_RXEN: enable receive
#define OPENETH_RXEN   BIT(0)

#define OPENETH_INT_SOURCE_REG      (OPENETH_BASE + 0x04)
#define OPENETH_INT_MASK_REG        (OPENETH_BASE + 0x08)
// These bits apply to INT_SOURCE and INT_MASK registers:
// OPENETH_INT_BUSY: Buffer was received and discarded due to lack of buffers
#define OPENETH_INT_BUSY   BIT(4)
// OPENETH_INT_RXB: Frame received
#define OPENETH_INT_RXB   BIT(2)
// OPENETH_INT_TXB: Frame transmitted
#define OPENETH_INT_TXB   BIT(0)

// IPGT, IPGR1, IPGR2 registers are not implemented in QEMU, hence not used here
#define OPENETH_PACKETLEN_REG       (OPENETH_BASE + 0x18)
// OPENETH_MINFL: minimum frame length
#define OPENETH_MINFL_S 16
#define OPENETH_MINFL_V 0xffff
#define OPENETH_MINFL_M (OPENETH_MINFL_V << OPENETH_MINFL_S)
// OPENETH_MAXFL: maximum frame length
#define OPENETH_MAXFL_S 0
#define OPENETH_MAXFL_V 0xffff
#define OPENETH_MAXFL_M (OPENETH_MAXFL_V << OPENETH_MAXFL_S)

// COLLCONF is not implemented in QEMU
#define OPENETH_TX_BD_NUM_REG       (OPENETH_BASE + 0x20)
// CTRLMODER, MIIMODER are not implemented in QEMU
#define OPENETH_MIICOMMAND_REG      (OPENETH_BASE + 0x2c)
// OPENETH_WCTRLDATA: write control data
#define OPENETH_WCTRLDATA   BIT(2)
// OPENETH_RSTAT: read status
#define OPENETH_RSTAT       BIT(1)
// OPENETH_SCANSTAT: scan status
#define OPENETH_SCANSTAT    BIT(0)

#define OPENETH_MIIADDRESS_REG      (OPENETH_BASE + 0x30)
// OPENETH_RGAD: register address
#define OPENETH_RGAD_S 8
#define OPENETH_RGAD_V 0x1f
#define OPENETH_RGAD_M (OPENETH_RGAD_V << OPENETH_RGAD_S)
// OPENETH_FIAD: PHY address
#define OPENETH_FIAD_S 0
#define OPENETH_FIAD_V 0x1f
#define OPENETH_FIAD_N (OPENETH_FIAD_V << OPENETH_FIAD_S)

#define OPENETH_MIITX_DATA_REG      (OPENETH_BASE + 0x34)
#define OPENETH_MIIRX_DATA_REG      (OPENETH_BASE + 0x38)
#define OPENETH_MII_DATA_MASK 0xffff

#define OPENETH_MIISTATUS_REG       (OPENETH_BASE + 0x3c)
// OPENETH_LINKFAIL: link is down
#define OPENETH_LINKFAIL BIT(0)

// OPENETH_MAC_ADDR0_REG: bytes 2-5 of the MAC address (byte 5 in LSB)
#define OPENETH_MAC_ADDR0_REG       (OPENETH_BASE + 0x40)
// OPENETH_MAC_ADDR1_REG: bytes 0-1 of the MAC address (byte 1 in LSB)
#define OPENETH_MAC_ADDR1_REG       (OPENETH_BASE + 0x44)

#define OPENETH_HASH0_ADR_REG       (OPENETH_BASE + 0x48)
#define OPENETH_HASH1_ADR_REG       (OPENETH_BASE + 0x4c)

// Location of the DMA descriptors
#define OPENETH_DESC_BASE           (OPENETH_BASE + 0x400)
// Total number of (TX + RX) DMA descriptors
#define OPENETH_DESC_CNT            128


// Structures describing TX and RX descriptors.
// The field names are same as in the OpenCores ethmac documentation.
typedef struct {
    uint16_t cs: 1;     //!< Carrier sense lost (flag set by HW)
    uint16_t df: 1;     //!< Defer indication (flag set by HW)
    uint16_t lc: 1;     //!< Late collision occured (flag set by HW)
    uint16_t rl: 1;     //!< TX failed due to retransmission limit (flag set by HW)
    uint16_t rtry: 4;   //!< Number of retries before the frame was sent (set by HW)
    uint16_t ur: 1;     //!< Underrun status (flag set by HW)
    uint16_t rsv: 2;    //!< Reserved
    uint16_t crc: 1;    //!< Add CRC at the end of the packet
    uint16_t pad: 1;    //!< Add padding to the end of short packets
    uint16_t wr: 1;     //!< Wrap-around. 0: not the last descriptor in the table, 1: last descriptor.
    uint16_t irq: 1;    //!< Generate interrupt after this descriptor is transmitted
    uint16_t rd: 1;     //!< Descriptor ready. 0: descriptor owned by SW, 1: descriptor owned by HW. Cleared by HW.

    uint16_t len;       //!< Number of bytes to be transmitted
    void* txpnt;        //!< Pointer to the data to transmit
} openeth_tx_desc_t;

_Static_assert(sizeof(openeth_tx_desc_t) == 8, "incorrect size of openeth_tx_desc_t");

typedef struct {
    uint16_t lc: 1;     //!< Late collision flag
    uint16_t crc: 1;    //!< RX CRC error flag
    uint16_t sf: 1;     //!< Frame shorter than set in PACKETLEN register
    uint16_t tl: 1;     //!< Frame longer than set in PACKETLEN register
    uint16_t dn: 1;     //!< Dribble nibble (frame length not divisible by 8 bits) flag
    uint16_t is: 1;     //!< Invalid symbol flag
    uint16_t or: 1;     //!< Overrun flag
    uint16_t m: 1;      //!< Frame received because of the promiscuous mode
    uint16_t rsv: 5;    //!< Reserved
    uint16_t wr: 1;     //!< Wrap-around. 0: not the last descriptor in the table, 1: last descriptor.
    uint16_t irq: 1;    //!< Generate interrupt after this descriptor is transmitted
    uint16_t e: 1;      //!< The buffer is empty. 0: descriptor owned by SW, 1: descriptor owned by HW.

    uint16_t len;       //!< Number of bytes received (filled by HW)
    void* rxpnt;        //!< Pointer to the receive buffer
} openeth_rx_desc_t;

_Static_assert(sizeof(openeth_rx_desc_t) == 8, "incorrect size of openeth_rx_desc_t");


static inline openeth_tx_desc_t* openeth_tx_desc(int idx)
{
    assert(idx < TX_BUF_COUNT);
    return &((openeth_tx_desc_t*)OPENETH_DESC_BASE)[idx];
}

static inline openeth_rx_desc_t* openeth_rx_desc(int idx)
{
    assert(idx < OPENETH_DESC_CNT - TX_BUF_COUNT);
    return &((openeth_rx_desc_t*)OPENETH_DESC_BASE)[idx + TX_BUF_COUNT];
}

static inline void openeth_enable(void)
{
    REG_SET_BIT(OPENETH_MODER_REG, OPENETH_TXEN | OPENETH_RXEN | OPENETH_PRO);
    REG_SET_BIT(OPENETH_INT_MASK_REG, OPENETH_INT_RXB);
}

static inline void openeth_disable(void)
{
    REG_CLR_BIT(OPENETH_INT_MASK_REG, OPENETH_INT_RXB);
    REG_CLR_BIT(OPENETH_MODER_REG, OPENETH_TXEN | OPENETH_RXEN | OPENETH_PRO);
}

static inline void openeth_reset(void)
{
    REG_SET_BIT(OPENETH_MODER_REG, OPENETH_RST);
    REG_CLR_BIT(OPENETH_MODER_REG, OPENETH_RST);
}

static inline void openeth_init_tx_desc(openeth_tx_desc_t* desc, void* buf)
{
    *desc = (openeth_tx_desc_t) {
        .rd = 0,
        .txpnt = buf
    };
}

static inline void openeth_init_rx_desc(openeth_rx_desc_t* desc, void* buf)
{
    *desc = (openeth_rx_desc_t) {
        .e = 1,
        .irq = 1,
        .rxpnt = buf
    };
}

static inline void openeth_set_tx_desc_cnt(int tx_desc_cnt)
{
    assert(tx_desc_cnt <= OPENETH_DESC_CNT);
    REG_WRITE(OPENETH_TX_BD_NUM_REG, tx_desc_cnt);
}

#ifdef __cplusplus
}
#endif
