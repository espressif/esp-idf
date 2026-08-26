/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "emac_priv.h"
#include "soc/emac_reg.h"

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
#define EMAC_MAC_RETENTION_REGS_BASE (DR_REG_EMAC_BASE + 0x0)
static const uint32_t emac_mac_regs_map[4] = {0x53, 0x0, 0x0, 0x0};

#define EMAC_DMA_RETENTION_REGS_CNT 3
#define EMAC_DMA_RETENTION_REGS_BASE (DR_REG_EMAC_BASE + 0x1000)
static const uint32_t emac_dma_regs_map[4] = {0xc1, 0x0, 0x0, 0x0};

const regdma_entries_config_t emac_regdma_entries[] = {
    // backup stage: stop TX/RX DMA
    [0] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x00),
                                         EMAC_OPERATIONMODE_REG, 0x0, EMAC_SR_M | EMAC_ST_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: wait for the TX done (debug reg);
    // IDF-13600
    [1] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_EMAC_LINK(0x01),
                                        EMAC_DEBUG_REG, 0x0, EMAC_TFCSTS_M, 0, 1),
        .owner = ENTRY(0)
    },

    // backup stage: stop TX/RX in MAC layer
    [2] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x02),
                                         EMAC_MACCONFIGURATION_REG, 0x0, EMAC_RE_M | EMAC_TE_M, 0, 1),
        .owner = ENTRY(0)
    },

    // restore stage: EMAC SW reset
    [3] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x03),
                                         EMAC_BUSMODE_REG, EMAC_SWR, EMAC_SWR_M, 1, 0),
        .owner = ENTRY(0)
    },
    // restore stage: wait for the SW reset done
    [4] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_EMAC_LINK(0x04),
                                        EMAC_BUSMODE_REG, 0x0, EMAC_SWR_M, 1, 0),
        .owner = ENTRY(0)
    },

    // backup stage: save Current Host Tx/Rx Descriptor Register
    // restore stage: restore to Rx/Tx Descriptor List Address Register
    [5] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x05),
                                              EMAC_CURRENTHOSTRECEIVEDESCRIPTOR_REG, EMAC_RECEIVEDESCRIPTORLISTADDRESS_REG, 1, 0, 0),
        .owner = ENTRY(0)
    },
    [6] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x06),
                                              EMAC_CURRENTHOSTTRANSMITDESCRIPTOR_REG, EMAC_TRANSMITDESCRIPTORLISTADDRESS_REG, 1, 0, 0),
        .owner = ENTRY(0)
    },

    // backup stage: save MAC Configuration Register (0), MAC Frame Filter (1), GMII Address Register (4) and Flow Control Register (6)
    // restore stage: restore at the same positions
    [7] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_EMAC_LINK(0x07),
                                            EMAC_MAC_RETENTION_REGS_BASE, EMAC_MAC_RETENTION_REGS_BASE,
                                            EMAC_MAC_RETENTION_REGS_CNT, 0, 0,
                                            emac_mac_regs_map[0], emac_mac_regs_map[1],
                                            emac_mac_regs_map[2], emac_mac_regs_map[3]),
        .owner = ENTRY(0)
    },

    // backup stage: save DMA Bus Mode Register (0), Operation Mode Register (6) and Interrupt Enable Register (7)
    // restore stage: restore at the same positions
    [8] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_EMAC_LINK(0x08),
                                            EMAC_DMA_RETENTION_REGS_BASE, EMAC_DMA_RETENTION_REGS_BASE,
                                            EMAC_DMA_RETENTION_REGS_CNT, 0, 0,
                                            emac_dma_regs_map[0], emac_dma_regs_map[1],
                                            emac_dma_regs_map[2], emac_dma_regs_map[3]),
        .owner = ENTRY(0)
    },

    // backup stage: save Interrupt Mask Register (15) and MAC Address Registers (16-...)
    // restore stage: restore at the same positions
    [9] = {
        // 1 word for int. reg, 2 words for MAC Addr0, 8*2 words for MAC Addr1-8
        .config =  REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x09),
                                               EMAC_INTERRUPTMASK_REG, EMAC_INTERRUPTMASK_REG, 1 + 2 + 8 * 2, 0, 0),
        .owner = ENTRY(0)
    },

    //** ------------------------------------------------------------------------------------------------
    //  Below steps are to be performed only when link is up and EMAC is started
    //** ------------------------------------------------------------------------------------------------
    // restore stage: start TX in MAC layer
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(EMAC_REGDMA_LINK_EMAC_START_BEGIN),
                                         EMAC_MACCONFIGURATION_REG, EMAC_TE, EMAC_TE_M, 1, 1),
        .owner = ENTRY(0)
    },

    // restore stage: start DMA TX and RX
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN + 1] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK((EMAC_REGDMA_LINK_EMAC_START_BEGIN + 1)),
                                         EMAC_OPERATIONMODE_REG, EMAC_ST | EMAC_SR, EMAC_ST_M | EMAC_SR_M, 1, 1),
        .owner = ENTRY(0)
    },

    // restore stage: start RX in MAC layer
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN + 2] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK((EMAC_REGDMA_LINK_EMAC_START_BEGIN + 2)),
                                         EMAC_MACCONFIGURATION_REG, EMAC_RE, EMAC_RE_M, 1, 1),
        .owner = ENTRY(0)
    },
};

const emac_reg_retention_info_t emac_reg_retention_info = {
    .module_id = SLEEP_RETENTION_MODULE_EMAC,
    .entry_array = emac_regdma_entries,
    .array_size = ARRAY_SIZE(emac_regdma_entries)
};
