/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "emac_priv.h"
#include "soc/gmac_reg.h"

/* Registers in retention context:
    DMA:
       0 Bus Mode Register
       3 Receive Descriptor List Address Register (can write to this register only when Rx DMA has stopped)
       4 Transmit Descriptor List Address Register - same as above
       6 Operation Mode Register
       7 Interrupt Enable Register
       18 Current Host Transmit Descriptor Register (RO!) => save to  Transmit Descriptor Reg 4
       19 Current Host Receive Descriptor Register (RO) => save to  Receive Descriptor Reg 3

    MAC:
       0 MAC Configuration Register
       1 MAC Frame Filter
       4 GMII Address Register
       6 Flow Control Register
       15 Interrupt Mask Register
       16 MAC Address0 High Register
       17 MAC Address0 Low Register
       18 - (18 + n*2) MAC Address1-n Registers

    IEEE1588:
        no need to save/restore since such use case does not make sense (system cannot sleep when wants to maintain nsec sync precision)
*/
#define EMAC_MAC_RETENTION_REGS_CNT 4
#define EMAC_MAC_RETENTION_REGS_BASE (DR_REG_GMAC_BASE + 0x0)
static const uint32_t emac_mac_regs_map[4] = {0x53, 0x0, 0x0, 0x0};

#define EMAC_DMA_RETENTION_REGS_CNT 3
#define EMAC_DMA_RETENTION_REGS_BASE (DR_REG_GMAC_BASE + 0x1000)
static const uint32_t emac_dma_regs_map[4] = {0xc1, 0x0, 0x0, 0x0};

const regdma_entries_config_t emac_regdma_entries[] = {
    // backup stage: stop TX/RX DMA
    [0] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x00),
                                         GMAC_REGISTER6_OPERATIONMODEREGISTER_REG, 0x0, GMAC_SR_M | GMAC_ST_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // backup stage: wait for the TX done (debug reg);
    // IDF-13600
    [1] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_EMAC_LINK(0x01),
                                        GMAC_REGISTER9_DEBUGREGISTER_REG, 0x0, GMAC_TFCSTS_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // backup stage: stop TX/RX in MAC layer
    [2] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x02),
                                         GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG, 0x0, GMAC_RE_M | GMAC_TE_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // restore stage: EMAC SW reset
    [3] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x03),
                                         GMAC_REGISTER0_BUSMODEREGISTER_REG, GMAC_SWR, GMAC_SWR_M, 1, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // restore stage: wait for the SW reset done
    [4] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_EMAC_LINK(0x04),
                                        GMAC_REGISTER0_BUSMODEREGISTER_REG, 0x0, GMAC_SWR_M, 1, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // backup stage: save Current Host Tx/Rx Descriptor Register
    // restore stage: restore to Rx/Tx Descriptor List Address Register
    [5] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x05),
                                              GMAC_REGISTER19_CURRENTHOSTRECEIVEDESCRIPTORREGISTER_REG, GMAC_REGISTER3_RECEIVEDESCRIPTORLISTADDRESSREGISTER_REG, 1, 0, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },
    [6] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x06),
                                              GMAC_REGISTER18_CURRENTHOSTTRANSMITDESCRIPTORREGISTER_REG, GMAC_REGISTER4_TRANSMITDESCRIPTORLISTADDRESSREGISTER_REG, 1, 0, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // backup stage: save MAC Configuration Register (0), MAC Frame Filter (1), GMII Address Register (4) and Flow Control Register (6)
    // restore stage: restore at the same positions
    [7] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_EMAC_LINK(0x07),
                                            EMAC_MAC_RETENTION_REGS_BASE, EMAC_MAC_RETENTION_REGS_BASE,
                                            EMAC_MAC_RETENTION_REGS_CNT, 0, 0,
                                            emac_mac_regs_map[0], emac_mac_regs_map[1],
                                            emac_mac_regs_map[2], emac_mac_regs_map[3]),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // backup stage: save DMA Bus Mode Register (0), Operation Mode Register (6) and Interrupt Enable Register (7)
    // restore stage: restore at the same positions
    [8] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_EMAC_LINK(0x08),
                                            EMAC_DMA_RETENTION_REGS_BASE, EMAC_DMA_RETENTION_REGS_BASE,
                                            EMAC_DMA_RETENTION_REGS_CNT, 0, 0,
                                            emac_dma_regs_map[0], emac_dma_regs_map[1],
                                            emac_dma_regs_map[2], emac_dma_regs_map[3]),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // backup stage: save Interrupt Mask Register (15) and MAC Address Registers (16-...)
    // restore stage: restore at the same positions
    [9] = {
        // 1 word for int. reg, 2 words for MAC Addr0, 8*2 words for MAC Addr1-8
        .config =  REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x09),
                                               GMAC_REGISTER15_INTERRUPTMASKREGISTER_REG, GMAC_REGISTER15_INTERRUPTMASKREGISTER_REG, 1 + 2 + 8 * 2, 0, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },

    //** ------------------------------------------------------------------------------------------------
    //  Below steps are to be performed only when link is up and EMAC is started
    //** ------------------------------------------------------------------------------------------------
    // restore stage: start TX in MAC layer
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(EMAC_REGDMA_LINK_EMAC_START_BEGIN),
                                         GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG, GMAC_TE, GMAC_TE_M, 1, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // restore stage: start DMA TX and RX
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN + 1] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK((EMAC_REGDMA_LINK_EMAC_START_BEGIN + 1)),
                                         GMAC_REGISTER6_OPERATIONMODEREGISTER_REG, GMAC_ST | GMAC_SR, GMAC_ST_M | GMAC_SR_M, 1, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // restore stage: start RX in MAC layer
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN + 2] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK((EMAC_REGDMA_LINK_EMAC_START_BEGIN + 2)),
                                         GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG, GMAC_RE, GMAC_RE_M, 1, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
};

const emac_reg_retention_info_t emac_reg_retention_info = {
    .module_id = SLEEP_RETENTION_MODULE_EMAC,
    .entry_array = emac_regdma_entries,
    .array_size = ARRAY_SIZE(emac_regdma_entries)
};


// -------------------------------------------------
//                 EMAC WoL (PMT power-down)
// -------------------------------------------------
// CNNT stays powered; no SW reset and no register save/restore.
#define EMAC_WOL_RX_IDLE_MASK (GMAC_RXFSTS_M | GMAC_RRCSTS_M | GMAC_RWCSTS_M)

const regdma_entries_config_t emac_regdma_entries_wol[] = {
    // backup: stop Transmit DMA (leave Rx DMA running to drain the FIFO)
    [0] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x00),
                                         GMAC_REGISTER6_OPERATIONMODEREGISTER_REG, 0x0, GMAC_ST_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // backup: wait until the MAC transmit path is idle
    [1] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_EMAC_LINK(0x01),
                                        GMAC_REGISTER9_DEBUGREGISTER_REG, 0x0, GMAC_TFCSTS_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // backup: disable MAC transmitter and receiver
    [2] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x02),
                                         GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG, 0x0, GMAC_RE_M | GMAC_TE_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // backup: wait until Rx FIFO is empty and Rx controllers are idle
    [3] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_EMAC_LINK(0x03),
                                        GMAC_REGISTER9_DEBUGREGISTER_REG, 0x0, EMAC_WOL_RX_IDLE_MASK, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // backup: enable magic-packet detection
    [4] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x04),
                                         GMAC_REGISTER11_PMTCONTROLANDSTATUSREGISTER_REG, GMAC_MGKPKTEN, GMAC_MGKPKTEN_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // backup: enter PMT power-down (must follow MGKPKTEN)
    [5] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x05),
                                         GMAC_REGISTER11_PMTCONTROLANDSTATUSREGISTER_REG, GMAC_PWRDWN, GMAC_PWRDWN_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // backup: re-enable MAC receiver so magic packets can be detected
    [6] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x06),
                                         GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG, GMAC_RE, GMAC_RE_M, 0, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },

    // restore: leave PMT power-down (also self-clears on magic packet).
    // The masked write reads PMT CSR, which clears MGKPRCVD and deasserts pmt_intr_o.
    [7] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x07),
                                         GMAC_REGISTER11_PMTCONTROLANDSTATUSREGISTER_REG, 0x0, GMAC_PWRDWN_M, 1, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // restore: disable MAC receiver
    [8] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x08),
                                         GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG, 0x0, GMAC_RE_M, 1, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // restore: disable receive DMA
    [9] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x09),
                                         GMAC_REGISTER6_OPERATIONMODEREGISTER_REG, 0x0, GMAC_SR_M, 1, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // restore: disable magic-packet detection
    [10] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x0A),
                                         GMAC_REGISTER11_PMTCONTROLANDSTATUSREGISTER_REG, 0x0, GMAC_MGKPKTEN_M, 1, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // backup/restore: MAC Address0 High+Low.
    // Databook: rewrite Address0 on wake so the CSR module resyncs with the always-on block
    [11] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x0B),
                                              GMAC_REGISTER16_MACADDRESS0HIGHREGISTER_REG,
                                              GMAC_REGISTER16_MACADDRESS0HIGHREGISTER_REG,
                                              2, 0, 0),
        .owner = ENTRY(0) | ENTRY(2)
    },

    //** ------------------------------------------------------------------------------------------------
    //  Below steps are to be performed only when link is up
    //** ------------------------------------------------------------------------------------------------
    // restore: start TX in MAC layer
    [EMAC_REGDMA_LINK_EMAC_WOL_START_BEGIN] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(EMAC_REGDMA_LINK_EMAC_WOL_START_BEGIN),
                                         GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG, GMAC_TE, GMAC_TE_M, 1, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // restore: start DMA TX and RX
    [EMAC_REGDMA_LINK_EMAC_WOL_START_BEGIN + 1] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK((EMAC_REGDMA_LINK_EMAC_WOL_START_BEGIN + 1)),
                                         GMAC_REGISTER6_OPERATIONMODEREGISTER_REG, GMAC_ST | GMAC_SR, GMAC_ST_M | GMAC_SR_M, 1, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
    // restore: start RX in MAC layer
    [EMAC_REGDMA_LINK_EMAC_WOL_START_BEGIN + 2] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK((EMAC_REGDMA_LINK_EMAC_WOL_START_BEGIN + 2)),
                                         GMAC_REGISTER0_MACCONFIGURATIONREGISTER_REG, GMAC_RE, GMAC_RE_M, 1, 1),
        .owner = ENTRY(0) | ENTRY(2)
    },
};

const emac_reg_retention_info_t emac_reg_retention_info_wol = {
    .module_id = SLEEP_RETENTION_MODULE_EMAC,
    .entry_array = emac_regdma_entries_wol,
    .array_size = ARRAY_SIZE(emac_regdma_entries_wol)
};
