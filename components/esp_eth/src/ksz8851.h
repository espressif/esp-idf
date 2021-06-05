// Copyright (c) 2021 Vladimir Chistyakov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once


typedef enum {
    KSZ8851_CCR                = 0x08, ///< Chip Configuration Register
    KSZ8851_MARL               = 0x10, ///< Host MAC Address Register Low
    KSZ8851_MARM               = 0x12, ///< Host MAC Address Register Middle
    KSZ8851_MARH               = 0x14, ///< Host MAC Address Register High
    KSZ8851_OBCR               = 0x20, ///< On-Chip Bus Control Register
    KSZ8851_EEPCR              = 0x22, ///< EEPROM Control Register
    KSZ8851_MBIR               = 0x24, ///< Memory Built-In Self-Test (BIST) Info Register
    KSZ8851_GRR                = 0x26, ///< Global Reset Register
    KSZ8851_WFCR               = 0x2A, ///< Wakeup Frame Control Register
    KSZ8851_WF0CRC0            = 0x30, ///< Wakeup Frame 0 CRC0 Register (lower 16 bits)
    KSZ8851_WF0CRC1            = 0x32, ///< Wakeup Frame 0 CRC1 Register (upper 16 bits)
    KSZ8851_WF0BM0             = 0x34, ///< Wakeup Frame 0 Byte Mask 0 Register (0-15)
    KSZ8851_WF0BM1             = 0x36, ///< Wakeup Frame 0 Byte Mask 1 Register (16-31)
    KSZ8851_WF0BM2             = 0x38, ///< Wakeup Frame 0 Byte Mask 2 Register (32-47)
    KSZ8851_WF0BM3             = 0x3A, ///< Wakeup Frame 0 Byte Mask 3 Register (48-63)
    KSZ8851_WF1CRC0            = 0x40, ///< Wakeup Frame 1 CRC0 Register (lower 16 bits)
    KSZ8851_WF1CRC1            = 0x42, ///< Wakeup Frame 1 CRC1 Register (upper 16 bits)
    KSZ8851_WF1BM0             = 0x44, ///< Wakeup Frame 1 Byte Mask 0 Register (0-15)
    KSZ8851_WF1BM1             = 0x46, ///< Wakeup Frame 1 Byte Mask 1 Register (16-31)
    KSZ8851_WF1BM2             = 0x48, ///< Wakeup Frame 1 Byte Mask 2 Register (32-47)
    KSZ8851_WF1BM3             = 0x4A, ///< Wakeup Frame 1 Byte Mask 3 Register (48-63)
    KSZ8851_WF2CRC0            = 0x50, ///< Wakeup Frame 2 CRC0 Register (lower 16 bits)
    KSZ8851_WF2CRC1            = 0x52, ///< Wakeup Frame 2 CRC1 Register (upper 16 bits)
    KSZ8851_WF2BM0             = 0x54, ///< Wakeup Frame 2 Byte Mask 0 Register (0-15)
    KSZ8851_WF2BM1             = 0x56, ///< Wakeup Frame 2 Byte Mask 1 Register (16-31)
    KSZ8851_WF2BM2             = 0x58, ///< Wakeup Frame 2 Byte Mask 2 Register (32-47)
    KSZ8851_WF2BM3             = 0x5A, ///< Wakeup Frame 2 Byte Mask 3 Register (48-63)
    KSZ8851_WF3CRC0            = 0x60, ///< Wakeup Frame 3 CRC0 Register (lower 16 bits)
    KSZ8851_WF3CRC1            = 0x62, ///< Wakeup Frame 3 CRC1 Register (upper 16 bits)
    KSZ8851_WF3BM0             = 0x64, ///< Wakeup Frame 3 Byte Mask 0 Register (0-15)
    KSZ8851_WF3BM1             = 0x66, ///< Wakeup Frame 3 Byte Mask 1 Register (16-31)
    KSZ8851_WF3BM2             = 0x68, ///< Wakeup Frame 3 Byte Mask 2 Register (32-47)
    KSZ8851_WF3BM3             = 0x6A, ///< Wakeup Frame 3 Byte Mask 3 Register (48-63)
    KSZ8851_TXCR               = 0x70, ///< Transmit Control Register
    KSZ8851_TXSR               = 0x72, ///< Transmit Status Register
    KSZ8851_RXCR1              = 0x74, ///< Receive Control Register 1
    KSZ8851_RXCR2              = 0x76, ///< Receive Control Register 2
    KSZ8851_TXMIR              = 0x78, ///< TXQ Memory Information Register
    KSZ8851_RXFHSR             = 0x7C, ///< Receive Frame Header Status Register
    KSZ8851_RXFHBCR            = 0x7E, ///< Receive Frame Header Byte Count Register
    KSZ8851_TXQCR              = 0x80, ///< TXQ Command Register
    KSZ8851_RXQCR              = 0x82, ///< RXQ Command Register
    KSZ8851_TXFDPR             = 0x84, ///< TX Frame Data Pointer Register
    KSZ8851_RXFDPR             = 0x86, ///< RX Frame Data Pointer Register
    KSZ8851_RXDTTR             = 0x8C, ///< RX Duration Timer Threshold Register
    KSZ8851_RXDBCTR            = 0x8E, ///< RX Data Byte Count Threshold Register
    KSZ8851_IER                = 0x90, ///< Interrupt Enable Register
    KSZ8851_ISR                = 0x92, ///< Interrupt Status Register
    KSZ8851_RXFCTR             = 0x9C, ///< RX Frame Count & Threshold Register
    KSZ8851_TXNTFSR            = 0x9E, ///< TX Next Total Frames Size Register
    KSZ8851_MAHTR0             = 0xA0, ///< MAC Address Hash Table Register 0
    KSZ8851_MAHTR1             = 0xA2, ///< MAC Address Hash Table Register 1
    KSZ8851_MAHTR2             = 0xA4, ///< MAC Address Hash Table Register 2
    KSZ8851_MAHTR3             = 0xA6, ///< MAC Address Hash Table Register 3
    KSZ8851_FCLWR              = 0xB0, ///< Flow Control Low Watermark Register
    KSZ8851_FCHWR              = 0xB2, ///< Flow Control High Watermark Register
    KSZ8851_FCOWR              = 0xB4, ///< Flow Control Overrun Watermark Register
    KSZ8851_CIDER              = 0xC0, ///< Chip ID and Enable Register
    KSZ8851_CGCR               = 0xC6, ///< Chip Global Control Register
    KSZ8851_IACR               = 0xC8, ///< Indirect Access Control Register
    KSZ8851_IADLR              = 0xD0, ///< Indirect Access Data Low Register
    KSZ8851_IADHR              = 0xD2, ///< Indirect Access Data High Register
    KSZ8851_PMECR              = 0xD4, ///< Power Management Event Control Register
    KSZ8851_GSWUTR             = 0xD6, ///< Go-Sleep & Wake-Up Time Register
    KSZ8851_PHYRR              = 0xD8, ///< PHY Reset Register
    KSZ8851_P1MBCR             = 0xE4, ///< PHY 1 MII-Register Basic Control Register
    KSZ8851_P1MBSR             = 0xE6, ///< PHY 1 MII-Register Basic Status Register
    KSZ8851_PHY1ILR            = 0xE8, ///< PHY 1 PHY ID Low Register
    KSZ8851_PHY1IHR            = 0xEA, ///< PHY 1 PHY ID High Register
    KSZ8851_P1ANAR             = 0xEC, ///< PHY 1 Auto-Negotiation Advertisement Register
    KSZ8851_P1ANLPR            = 0xEE, ///< PHY 1 Auto-Negotiation Link Partner Ability Register
    KSZ8851_P1SCLMD            = 0xF4, ///< Port 1 PHY Special Control/Status, LinkMD
    KSZ8851_P1CR               = 0xF6, ///< Port 1 Control Register
    KSZ8851_P1SR               = 0xF8, ///< Port 1 Status Register
    KSZ8851_VALID_ADDRESS_MASK = 0xFE, ///< All register addresses are under this mask
} ksz8851_registers_t;

typedef enum {
    CCR_EEPROM_PRESENCE    = 0x0200U, ///< RO EEPROM presence
    CCR_SPI_BUS_MODE       = 0x0100U, ///< RO SPI bus mode
    CCR_32PIN_CHIP_PACKAGE = 0x0001U, ///< RO 32-Pin Chip Package

    OBCR_OUTPUT_PIN_DRIVE_STRENGTH    = 0x0040U, ///< RW Output Pin Drive Strength: 8mA (0) or 16mA (1)
    OBCR_ONCHIP_BUS_CLOCK_SELECTION   = 0X0004U, ///< RW On-Chip Bus Clock Selection: 125MHz (0)
    OBCR_ONCHIP_BUS_CLOCK_DIVIDE_BY_1 = 0x0000U, ///< RW On-Chip Bus Clock Divider Selection
    OBCR_ONCHIP_BUS_CLCOK_DIVIDE_BY_2 = 0x0001U, ///< Rw On-Chip Bus Clock Divider Selection
    OBCR_ONCHIP_BUS_CLCOK_DIVIDE_BY_3 = 0x0002U, ///< RW On-Chip Bus Clock Divider Selection

    EEPCR_EESRWA = 0x0020U, ///< RW EEPROM Software Read (0) or Write (1) Access
    EEPCR_EESA   = 0x0010U, ///< RW EEPROM Software Access
    EEPCR_EESB   = 0x0008U, ///< RO EEPROM Data receive
    EEPCR_EECB2  = 0x0004U, ///< RW EEPROM Data transmit
    EEPCR_EECB1  = 0x0002U, ///< RW EEPROM Serial clock
    EEPCR_EECB0  = 0x0001U, ///< RW EEPROM Chip select

    MBIR_TXMBF        = 0x1000U,                  ///< RO TX Memory BIST Test Finish
    MBIR_TXMBFA       = 0x0800U,                  ///< RO TX Memory BIST Test Fail
    MBIR_TXMBFC_SHIFT = 8U,                       ///< RO TX Memory BIST Test Fail Count Shift
    MBIR_TXMBFC_MASK  = 0x7 << MBIR_TXMBFC_SHIFT, ///< RO TX Memory BIST Test Fail Count Mask
    MBIR_RXMBF        = 0x0010U,                  ///< RO RX Memory Bist Finish
    MBIR_RXMBFA       = 0x0008U,                  ///< RO RX Memory Bist Fail
    MBIR_RXMBFC       = 0x7U,                     ///< RO RX Memory BIST Test Fail Count

    GRR_QMU_MODULE_SOFT_RESET = 0x0002U, ///< RW QMU Module Soft Reset
    GRR_GLOBAL_SOFT_RESET     = 0x0001U, ///< Rw Global Soft Reset

    WFCR_MPRXE = 0x0080U, ///< RW Magic Packet RX Enable
    WFCR_WF3E  = 0x0008U, ///< RW Wake up Frame 3 Enable
    WFCR_WF2E  = 0x0004U, ///< RW Wake up Frame 2 Enable
    WFCR_WF1E  = 0x0002U, ///< RW Wake up Frame 1 Enable
    WFCR_WF0E  = 0x0001U, ///< RW Wake up Frame 0 Enable

    TXCR_TCGICMP = 0x0100U, ///< RW Transmit Checksum Generation for ICMP
    TXCR_TCGTCP  = 0x0040U, ///< RW Transmit Checksum Generation for TCP
    TXCR_TCGIP   = 0x0020U, ///< RW Transmit Checksum Generation for IP
    TXCR_FTXQ    = 0x0010U, ///< RW Flush Transmit Queue
    TXCR_TXFCE   = 0x0008U, ///< RW Transmit Flow Control Enable
    TXCR_TXPE    = 0x0004U, ///< RW Transmit Padding Enable
    TXCR_TXCE    = 0x0002U, ///< RW Transmit CRC Enable
    TXCR_TXE     = 0x0001U, ///< RW Transmit Enable

    TXSR_TXLC       = 0x2000U, ///< RO Transmit Late Collision
    TXSR_TXMC       = 0x1000U, ///< RO Transmit Maximum Collision
    TXSR_TXFID_MASK = 0x003FU, ///< RO Transmit Frame ID Mask

    RXCR1_FRXQ     = 0x8000U, ///< RW Flush Receive Queue
    RXCR1_RXUDPFCC = 0x4000U, ///< RW Receive UDP Frame Checksum Check Enable
    RXCR1_RXTCPFCC = 0x2000U, ///< RW Receive TCP Frame Checksum Check Enable
    RXCR1_RXIPFCC  = 0x1000U, ///< RW Receive IP Frame Checksum Check Enable
    RXCR1_RXPAFMA  = 0x0800U, ///< RW Receive Physical Address Filtering with MAC Address Enable
    RXCR1_RXFCE    = 0x0400U, ///< RW Receive Flow Control Enable
    RXCR1_RXEFE    = 0x0200U, ///< RW Receive Error Frame Enable
    RXCR1_RXMAFMA  = 0x0100U, ///< RW Receive Multicast Address Filtering with MAC Address Enable
    RXCR1_RXBE     = 0x0080U, ///< RW Receive Broadcast Enable
    RXCR1_RXME     = 0x0040U, ///< RW Receive Multicast Enable
    RXCR1_RXUE     = 0x0020U, ///< RW Receive Unicast Enable
    RXCR1_RXAE     = 0x0010U, ///< RW Receive All Enable
    RXCR1_RXINVF   = 0x0002U, ///< RW Receive Inverse Filtering
    RXCR1_RXE      = 0x0001U, ///< RW Receive Enable

    RXCR2_SRDBL_SHIFT = 5U,      ///< WO SPI Receive Data Burst Length: 4/8/16/32/frame (0-4)
    RXCR2_IUFFP       = 0x0010U, ///< RW IPv4/IPv6/UDP Fragment Frame Pass
    RXCR2_RXIUFCEZ    = 0x0008U, ///< RW Receive IPv4/IPv6/UDP Frame Checksum Equal Zero
    RXCR2_UDPLFE      = 0x0004U, ///< RW Lite Frame Enable
    RXCR2_RXICMPFCC   = 0x0002U, ///< RW Receive ICMP Frame Checksum Check Enable
    RXCR2_RXSAF       = 0x0001U, ///< RW Receive Source Address Filtering

    TXMIR_TXMA_MASK = 0x1FFFU, ///< RO Transmit Memory Available Mask

    RXFHSR_RXFV      = 0x8000U, ///< RO Receive Frame Valid
    RXFHSR_RXICMPFCS = 0x2000U, ///< RO Receive ICMP Frame Checksum Status
    RXFHSR_RXIPFCS   = 0x1000U, ///< RO Receive IP Frame Checksum Status
    RXFHSR_RXTCPFCS  = 0x0800U, ///< RO Receive TCP Frame Checksum Status
    RXFHSR_RXUDPFCS  = 0x0400U, ///< RO Receive UDP Frame Checksum Status
    RXFHSR_RXBF      = 0x0080U, ///< RO Receive Broadcast Frame
    RXFHSR_RXMF      = 0x0040U, ///< RO Receive Multicast Frame
    RXFHSR_RXUF      = 0x0020U, ///< RO Receive Unicast Frame
    RXFHSR_RXMR      = 0x0010U, ///< RO Receive MII Error
    RXFHSR_RXFT      = 0x0008U, ///< RO Receive Frame Type
    RXFHSR_RXFTL     = 0x0004U, ///< RO Receive Frame Too Long
    RXFHSR_RXRF      = 0x0002U, ///< RO Receive Runt Frame
    RXFHSR_RXCE      = 0x0001U, ///< RO Receive CRC Error

    RXFHBCR_RXBC_MASK = 0x0FFFU, ///< RO Receive Byte Count Mask

    TXQCR_AETFE  = 0x0004U, ///< RW Auto-Enqueue TXQ Frame Enable
    TXQCR_TXQMAM = 0x0002U, ///< RW TXQ Memory Available Monitor
    TXQCR_METFE  = 0x0001U, ///< RW (SC) Manual Enqueue TXQ Frame Enable

    RXQCR_RXDTTS   = 0x1000U, ///< RO RX Duration Timer Threshold Status
    RXQCR_RXDBCTS  = 0x0800U, ///< RO RX Data Byte Count Threshold Status
    RXQCR_RXFCTS   = 0x0400U, ///< RO RX Frame Count Threshold Status
    RXQCR_RXIPHTOE = 0x0200U, ///< RW RX IP Header Two-Byte Offset Enable
    RXQCR_RXDTTE   = 0x0080U, ///< RW RX Duration Timer Threshold Enable
    RXQCR_RXDBCTE  = 0x0040U, ///< RW RX Data Byte Count Threshold Enable
    RXQCR_RXFCTE   = 0x0020U, ///< RW RX Frame Count Threshold Enable
    RXQCR_ADRFE    = 0x0010U, ///< RW Auto-Dequeue RXQ Frame Enable
    RXQCR_SDA      = 0x0008U, ///< WO Start DMA Access
    RXQCR_RRXEF    = 0x0001U, ///< RW Release RX Error Frame

    TXFDPR_TXFPAI    = 0x4000U, ///< RW TX Frame Data Pointer Auto Increment
    TXFDPR_TXFP_MASK = 0x07FFU, ///< RO TX Frame Pointer Mask

    RXFDPR_RXFPAI    = 0x4000U, ///< RW RX Frame Pointer Auto Increment
    RXFDPR_RXFP_MASK = 0x07FFU, ///< WO RX Frame Pointer Mask

    IER_LCIE    = 0x8000U, ///< RW Link Change Interrupt Enable
    IER_TXIE    = 0x4000U, ///< RW Transmit Interrupt Enable
    IER_RXIE    = 0x2000U, ///< RW Receive Interrupt Enable
    IER_RXOIE   = 0x0800U, ///< RW Receive Overrun Interrupt Enable
    IER_TXPSIE  = 0x0200U, ///< RW Transmit Process Stopped Interrupt Enable
    IER_RXPSIE  = 0x0100U, ///< RW Receive Process Stopped Interrupt Enable
    IER_TXSAIE  = 0x0040U, ///< RW Transmit Space Available Interrupt Enable
    IER_RXWFDIE = 0x0020U, ///< RW Receive Wake-up Frame Detect Interrupt Enable
    IER_RXMPDIE = 0x0010U, ///< RW Receive Magic Packet Detect Interrupt Enable
    IER_LDIE    = 0x0008U, ///< RW Linkup Detect Interrupt Enable
    IER_EDIE    = 0x0004U, ///< RW Energy Detect Interrupt Enable
    IER_SPIBEIE = 0x0002U, ///< RW SPI Bus Error Interrupt Enable
    IER_DEDIE   = 0x0001U, ///< RW Delay Energy Detect Interrupt Enable

    ISR_LCIS    = 0x8000U, ///< RO (W1C) Link Change Interrupt Status
    ISR_TXIS    = 0x4000U, ///< RO (W1C) Transmit Interrupt Status
    ISR_RXIS    = 0x2000U, ///< RO (W1C) Receive Interrupt Status
    ISR_RXOIS   = 0x0800U, ///< RO (W1C) Receive Overrun Interrupt Status
    ISR_TXPSIS  = 0x0200U, ///< RO (W1C) Transmit Process Stopped Interrupt Status
    ISR_RXPSIS  = 0x0100U, ///< RO (W1C) Receive Process Stopped Interrupt Status
    ISR_TXSAIS  = 0x0040U, ///< RO (W1C) Transmit Space Available Interrupt Status
    ISR_RXWFDIS = 0x0020U, ///< RO (W1C) Receive Wakeup Frame Detect Interrupt Status
    ISR_RXMPDIS = 0x0010U, ///< RO (W1C) Receive Magic Packet Detect Interrupt Status
    ISR_LDIS    = 0x0008U, ///< RO (W1C) Linkup Detect Interrupt Status
    ISR_EDIS    = 0x0004U, ///< RO (W1C) Energy Detect Interrupt Status
    ISR_SPIBEIS = 0x0002U, ///< RO (W1C) SPI Bus Error Interrupt Status
    ISR_ALL     = 0xFFFFU, ///< WO Clear register value

    RXFCTR_RXFC_SHIFT = 8U,                        ///< RO RX Frame Count Shift
    RXFCTR_RXFC_MASK  = 0xFF << RXFCTR_RXFC_SHIFT, ///< RO RX Frame Count Mask
    RXFCTR_RXFCT_MASK = 0xFFU,                     ///< RW Receive Frame Count Threshold

    FCLWR_MASK = 0x0FFFU, ///< RW  Flow Control Low Watermark Configuration Mask
    FCHWR_MASK = 0x0FFFU, ///< RW  Flow Control High Watermark Configuration Mask
    FCOWR_MASK = 0x0FFFU, ///< RW  Flow Control Overrun Watermark Configuration Mask

    CIDER_KSZ8851SNL_FAMILY_ID = 0x88U,                          ///< KSZ8851SNL Family ID
    CIDER_KSZ8851SNL_CHIP_ID   = 0x7U,                           ///< KSZ8851SNL Chip ID
    CIDER_FAMILY_ID_SHIFT      = 8U,                             ///< RO Family ID Shift
    CIDER_FAMILY_ID_MASK       = 0xFF << CIDER_FAMILY_ID_SHIFT,  ///< RO Family ID Mask
    CIDER_CHIP_ID_SHIFT        = 4U,                             ///< RO Chip ID Shift
    CIDER_CHIP_ID_MASK         = 0xF << CIDER_CHIP_ID_SHIFT,     ///< RO Chip ID Mask
    CIDER_REVISION_ID_SHIFT    = 1U,                             ///< RO Revision ID Shift
    CIDER_REVISION_ID_MASK     = 0x7 << CIDER_REVISION_ID_SHIFT, ///< RO Revision ID Mask

    CGCR_LEDSEL0 = 0x0200U, ///< RW PHY LED Mode: 0 - 100BT + LINK/ACTU, 1 - ACT + LINK

    IACR_READ_ENABLE           = 0x1000U, ///< RW Read Enable
    IACR_MIB_COUNTER_SELECT    = 0x0C00U, ///< RW Table Select
    IACR_INDIRECT_ADDRESS_MASK = 0x001FU, ///< RW Indirect Address Mask

    PMECR_PME_DELAY_ENABLE         = 0x4000U, ///< RW PME Delay Enable
    PMECR_PME_OUTPUT_POLARITY      = 0x1000U, ///< RW PME Output Polarity
    PMECR_WUP_FRAME_EN             = 0x0800U, ///< RW Wake-on-LAN to PME Output Enable receive wake-up frame
    PMECR_MAGIC_PACKET             = 0x0400U, ///< RW Wake-on-LAN to PME Output Enable receive magic packet
    PMECR_LINK_CHANGE_TO_UP        = 0x0200U, ///< RW Wake-on-LAN to PME Output Enable link change to up
    PMECR_SIGNAL_ENERGY_DETECTED   = 0x0100U, ///< RW Wake-on-LAN to PME Output Enable energy detected
    PMECR_AUTO_WAKEUP_ENABLE       = 0x0080U, ///< RW Auto Wake-Up Enable
    PMECR_WAKEUP_TO_NORMAL         = 0x0040U, ///< RW Wake-Up to Normal Operation Mode
    PMECR_WAKEUP_FRAME_EVENT       = 0x0020U, ///< RO (W1C) Wake-Up Event Indication wakeup frame event detected
    PMECR_WAKEUP_MAGIC_PACKET      = 0x0010U, ///< RO (W1C) Wake-Up Event Indication magic packet event detected
    PMECR_WAKEUP_LINK              = 0x0008U, ///< RO (W1C) Wake-Up Event Indication link up event detected
    PMECR_WAKEUP_ENERGY            = 0x0004U, ///< RO (W1C) Wake-Up Event Indication energy event detected
    PMECR_PME_MODE_MASK            = 0x0003U, ///< RW Power Management Mode Mask
    PMECR_PME_MODE_NORMAL          = 0x0000U, ///< RW Normal Operation Mode
    PMECR_PME_MODE_ENERGY_DETECT   = 0x0001U, ///< RW Energy Detect Mode
    PMECR_PME_MODE_SOFT_POWER_DOWN = 0x0002U, ///< RW Soft Power Down Mode
    PMECR_PME_MODE_POWER_SAVING    = 0x0003U, ///< RW Power Saving Mode

    GSWUTR_WAKE_UP_TIME_SHIFT = 8U,                                ///< RW Wake-up Time Shift
    GSWUTR_WAKE_UP_TIME_MASK  = 0xFF << GSWUTR_WAKE_UP_TIME_SHIFT, ///< RW Wake-up Time Mask
    GSWUTR_GO_SLEEP_TIME_MASK = 0x0003U,                           ///< RW Go-sleep Time Mask

    PHYRR_PHY_RESET = 0x0001U, ///< WO (SC) PHY Reset Bit

    P1MBCR_LOCAL_LOOPBACK    = 0x4000U, ///< RW Local (far-end) loopback (llb)
    P1MBCR_FORCE100          = 0x2000U, ///< RW Force 100
    P1MBCR_AN_ENABLE         = 0x1000U, ///< RW AN Enable
    P1MBCR_RESTART_AN        = 0x0200U, ///< RW Restart AN
    P1MBCR_FORCE_FULL_DUPLEX = 0x0100U, ///< RW Force Full-Duplex
    P1MBCR_HP_MDIX           = 0x0020U, ///< RW HP Auto MDI-X mode
    P1MBCR_FORCE_MDIX        = 0x0010U, ///< RWForce MDI-X
    P1MBCR_DISABLE_MDIX      = 0x0008U, ///< RW Disable MDI-X
    P1MBCR_DISABLE_TRANSMIT  = 0x0002U, ///< RW Disable Transmit
    P1MBCR_DISABLE_LED       = 0x0001U, ///< RW Disable LED

    P1MBSR_T4_CAPABLE          = 0x8000U, ///< RO T4 Capable
    P1MBSR_100_FULL_CAPABLE    = 0x4000U, ///< RO 100 Full Capable
    P1MBSR_100_HALF_CAPABLE    = 0x2000U, ///< RO 100 Half Capable
    P1MBSR_10_FULL_CAPABLE     = 0x1000U, ///< RO 10 Full Capable
    P1MBSR_10_HALF_CAPABLE     = 0x0800U, ///< RO 10 Half Capable
    P1MBSR_PREAMBLE_SUPPRESSED = 0x0040U, ///< RO Preamble suppressed (not supported)
    P1MBSR_AN_COMPLETE         = 0x0020U, ///< RO AN Complete
    P1MBSR_AN_CAPABLE          = 0x0008U, ///< RO AN Capable
    P1MBSR_LINK_STATUS         = 0x0004U, ///< RO Link Status
    P1MBSR_JABBER_TEST         = 0x0002U, ///< RO Jabber test (not supported)
    P1MBSR_EXTENDED_CAPABLE    = 0x0001U, ///< RO Extended Capable

    P1ANAR_NEXT_PAGE    = 0x8000U, ///< RO Next page (not supported)
    P1ANAR_REMOTE_FAULT = 0x2000U, ///< RO Remote fault (not supported)
    P1ANAR_PAUSE        = 0x0400U, ///< RW Pause (flow control capability)
    P1ANAR_ADV_100_FULL = 0x0100U, ///< RW Adv 100 Full
    P1ANAR_ADV_100_HALF = 0x0080U, ///< RW Adv 100 Half
    P1ANAR_ADV_10_FULL  = 0x0040U, ///< RW Adv 10 Full
    P1ANAR_ADV_10_HALF  = 0x0020U, ///< RW Adv 10 Half

    P1ANLPR_NEXT_PAGE    = 0x8000U, ///< RO Next page (not supported)
    P1ANLPR_LP_ACK       = 0x4000U, ///< RO LP ACK (not suppported)
    P1ANLPR_REMOTE_FAULT = 0x2000U, ///< RO Remote fault (not supported)
    P1ANLPR_PAUSE        = 0x0400U, ///< RO Pause
    P1ANLPR_ADV_100_FULL = 0x0100U, ///< RO Adv 100 Full
    P1ANLPR_ADV_100_HALF = 0x0080U, ///< RO Adv 100 Half
    P1ANLPR_ADV_10_FULL  = 0x0040U, ///< RO Adv 10 Full
    P1ANLPR_ADV_10_HALF  = 0x0020U, ///< RO Adv 10 Half

    P1SCLMD_VCT_RESULT_SHIFT       = 13U,                             ///< RO VCT result Shift
    P1SCLMD_VCT_RESULT_MASK        = 0x3 << P1SCLMD_VCT_RESULT_SHIFT, ///< RO VCT result Mask
    P1SCLMD_VCT_RESULT_NORMAL      = 0x0U,                            ///< RO VCT result normal condition
    P1SCLMD_VCT_RESULT_OPEN        = 0x1U,                            ///< RO VCT result open cable condition
    P1SCLMD_VCT_RESULT_SHORT       = 0x2U,                            ///< RO VCT result short cable condition
    P1SCLMD_VCT_RESULT_TEST_FAILED = 0x3U,                            ///< RO VCT result test failed
    P1SCLMD_VCT_ENABLE             = 0x1000U,                         ///< RW (SC) VCT Enable
    P1SCLMD_FORCE_LINK             = 0x0800U,                         ///< RW Force link
    P1SCLMD_REMOTE_LOOPBACK        = 0x0200U,                         ///< RW Remote (Near-end) loopback (rlb)
    P1SCLMD_VCT_FAULT_COUNT_MASK   = 0x1FU,                           ///< RO Distance to the fault * 0.4m

    P1CR_LED_OFF                                 = 0x8000U, ///< RW Turn off all of the port 1 LEDs
    P1CR_TXIDS                                   = 0x4000U, ///< RW Disable the port’s transmitter.
    P1CR_RESTART_AN                              = 0x2000U, ///< RW Restart AN
    P1CR_DISABLE_AUTO_MDI_MDIX                   = 0x0400U, ///< RW Disable auto MDI/MDI-X
    P1CR_FORCE_MDIX                              = 0x0200U, ///< RW Force MDI-X
    P1CR_AUTO_NEGOTIATION_ENABLE                 = 0x0080U, ///< RW Auto Negotiation Enable
    P1CR_FORCE_SPEED                             = 0x0040U, ///< RW Force Speed 100
    P1CR_FORCE_DUPLEX                            = 0x0020U, ///< RW Force Full Duplex
    P1CR_ADVERTISED_FLOW_CONTROL_CAPABILITY      = 0x0010U, ///< RW Advertise flow control capability
    P1CR_ADVERTISED_100BT_FULL_DUPLEX_CAPABILITY = 0x0008U, ///< RW Advertise 100BT full-duplex capability
    P1CR_ADVERTISED_100BT_HALF_DUPLEX_CAPABILITY = 0x0004U, ///< RW Advertise 100BT half-duplex capability
    P1CR_ADVERTISED_10BT_FULL_DUPLEX_CAPABILITY  = 0x0002U, ///< RW Advertise 10BT full-duplex capability
    P1CR_ADVERTISED_10BT_HALF_DUPLEX_CAPABILITY  = 0x0001U, ///< RW Advertise 10BT half-duplex capability

    P1SR_HP_MDIX                              = 0x8000U, ///< RW HP Auto MDI-X mode
    P1SR_POLARITY_REVERSE                     = 0x2000U, ///< RO Polarity Reverse
    P1SR_OPERATION_SPEED                      = 0x0400U, ///< RO Operation Speed 100
    P1SR_OPERATION_DUPLEX                     = 0x0200U, ///< RO Operation Duplex Full
    P1SR_MDIX_STATUS                          = 0x0080U, ///< RO MDI status
    P1SR_AN_DONE                              = 0x0040U, ///< RO AN Done
    P1SR_LINK_GOOD                            = 0x0020U, ///< RO Link Good
    P1SR_PARTNER_FLOW_CONTROL_CAPABILITY      = 0x0010U, ///< RO Partner flow control capability
    P1SR_PARTNER_100BT_FULL_DUPLEX_CAPABILITY = 0x0008U, ///< RO Partner 100BT full-duplex capability
    P1SR_PARTNER_100BT_HALF_DUPLEX_CAPABILITY = 0x0004U, ///< RO Partner 100BT half-duplex capability
    P1SR_PARTNER_10BT_FULL_DUPLEX_CAPABILITY  = 0x0002U, ///< RO Partner 10BT full-duplex capability
    P1SR_PARTNER_10BT_HALF_DUPLEX_CAPABILITY  = 0x0001U, ///< RO Partner 10BT half-duplex capability
} ksz8851_register_bits_t;
