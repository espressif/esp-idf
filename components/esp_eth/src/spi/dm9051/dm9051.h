/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Registers in DM9051
 *
 */
#define DM9051_NCR (0x00)     // Network Control Register
#define DM9051_NSR (0x01)     // Network Status Register
#define DM9051_TCR (0x02)     // Tx Control Register
#define DM9051_TSR1 (0x03)    // Tx Status Register I
#define DM9051_TSR2 (0x04)    // Tx Status Register II
#define DM9051_RCR (0x05)     // Rx Control Register
#define DM9051_RSR (0x06)     // Rx Status Register
#define DM9051_ROCR (0x07)    // Receive Overflow Counter Register
#define DM9051_BPTR (0x08)    // Back Pressure Threshold Register
#define DM9051_FCTR (0x09)    // Flow Control Threshold Register
#define DM9051_FCR (0x0A)     // Rx/Tx Flow Control Register
#define DM9051_EPCR (0x0B)    // EEPROM & PHY Control Register
#define DM9051_EPAR (0x0C)    // EEPROM & PHY Address Register
#define DM9051_EPDRL (0x0D)   // EEPROM & PHY Data Register Low
#define DM9051_EPDRH (0x0E)   // EEPROM & PHY Data Register High
#define DM9051_WCR (0x0F)     // Wake Up Control Register
#define DM9051_PAR (0x10)     // Physical Address Register
#define DM9051_MAR (0x16)     // Multicast Address Hash Table Register
#define DM9051_GPCR (0x1E)    // General Purpose Control Register
#define DM9051_GPR (0x1F)     // General Purpose Register
#define DM9051_TRPAL (0x22)   // Tx Memory Read Pointer Address Low Byte
#define DM9051_TRPAH (0x23)   // Tx Memory Read Pointer Address High Byte
#define DM9051_RWPAL (0x24)   // Rx Memory Read Pointer Address Low Byte
#define DM9051_RWPAH (0x25)   // Rx Memory Read Pointer Address High Byte
#define DM9051_VIDL (0x28)    // Vendor ID Low Byte
#define DM9051_VIDH (0x29)    // Vendor ID High Byte
#define DM9051_PIDL (0x2A)    // Product ID Low Byte
#define DM9051_PIDH (0x2B)    // Product ID High Byte
#define DM9051_CHIPR (0x2C)   // CHIP Revision
#define DM9051_TCR2 (0x2D)    // Transmit Control Register 2
#define DM9051_ATCR (0x30)    // Auto-Transmit Control Register
#define DM9051_TCSCR (0x31)   // Transmit Check Sum Control Register
#define DM9051_RCSCSR (0x32)  // Receive Check Sum Control Status Register
#define DM9051_SBCR (0x38)    // SPI Bus Control Register
#define DM9051_INTCR (0x39)   // INT Pin Control Register
#define DM9051_PPCSR (0x3D)   // Pause Packet Control Status Register
#define DM9051_EEE_IN (0x3E)  // IEEE 802.3az Enter Counter Register
#define DM9051_EEE_OUT (0x3F) // IEEE 802.3az Leave Counter Register
#define DM9051_ALNCR (0x4A)   // SPI Byte Align Error Counter Register
#define DM9051_RLENCR (0x52)  // Rx Packet Length Control Register
#define DM9051_BCASTCR (0x53) // RX Broadcast Control Register
#define DM9051_INTCKCR (0x54) // INT Pin Clock Output Control Register
#define DM9051_MPTRCR (0x55)  // Memory Pointer Control Register
#define DM9051_MLEDCR (0x57)  // More LED Control Register
#define DM9051_MEMSCR (0x59)  // Memory Control Register
#define DM9051_TMEMR (0x5A)   // Transmit Memory Size Register
#define DM9051_MBSR (0x5D)    // Memory BIST Status Register
#define DM9051_MRCMDX (0x70)  // Memory Data Pre-Fetch Read Command Without Address Increment Register
#define DM9051_MRCMDX1 (0x71) // Memory Read Command Without Pre-Fetch and Without Address Increment Register
#define DM9051_MRCMD (0x72)   // Memory Data Read Command With Address Increment Register
#define DM9051_SDR_DLY (0x73) // SPI Data Read Delay Counter Register
#define DM9051_MRRL (0x74)    // Memory Data Read Address Register Low Byte
#define DM9051_MRRH (0x75)    // Memory Data Read Address Register High Byte
#define DM9051_MWCMDX (0x76)  // Memory Data Write Command Without Address Increment Register
#define DM9051_MWCMD (0x78)   // Memory Data Write Command With Address Increment Register
#define DM9051_MWRL (0x7A)    // Memory Data Write Address Register Low Byte
#define DM9051_MWRH (0x7B)    // Memory Data Write Address Register High Byte
#define DM9051_TXPLL (0x7C)   // TX Packet Length Low Byte Register
#define DM9051_TXPLH (0x7D)   // TX Packet Length High Byte Register
#define DM9051_ISR (0x7E)     // Interrupt Status Register
#define DM9051_IMR (0x7F)     // Interrupt Mask Register

/**
 * @brief status and flag of DM9051 specific registers
 *
 */
#define DM9051_SPI_RD (0) // Burst Read Command
#define DM9051_SPI_WR (1) // Burst Write Command

#define NCR_WAKEEN (1 << 6) // Enable Wakeup Function
#define NCR_FDX (1 << 3)    // Duplex Mode of the Internal PHY
#define NCR_RST (1 << 0)    // Software Reset and Auto-Clear after 10us

#define NSR_SPEED (1 << 7)  // Speed of Internal PHY
#define NSR_LINKST (1 << 6) // Link Status of Internal PHY
#define NSR_WAKEST (1 << 5) // Wakeup Event Status
#define NSR_TX2END (1 << 3) // TX Packet Index II Complete Status
#define NSR_TX1END (1 << 2) // TX Packet Index I Complete Status
#define NSR_RXOV (1 << 1)   // RX Memory Overflow Status
#define NSR_RXRDY (1 << 0)  // RX Packet Ready

#define TCR_TXREQ (1 << 0) // TX Request. Auto-Clear after Sending Completely

#define RCR_WTDIS (1 << 6)     // Watchdog Timer Disable
#define RCR_DIS_LONG (1 << 5)  // Discard Long Packet
#define RCR_DIS_CRC (1 << 4)   // Discard CRC Error Packet
#define RCR_ALL_MCAST (1 << 3) // Receive All Multicast
#define RCR_RUNT (1 << 2)      // Receive Runt Packet
#define RCR_PRMSC (1 << 1)     // Promiscuous Mode
#define RCR_RXEN (1 << 0)      // RX Enable

#define RSR_RF (1 << 7)   // Runt Frame
#define RSR_MF (1 << 6)   // Multicast Frame
#define RSR_LCS (1 << 5)  // Late Collision Seen
#define RSR_RWTO (1 << 4) // Receive Watchdog Time-Out
#define RSR_PLE (1 << 3)  // Physical Layer Error
#define RSR_AE (1 << 2)   //  Alignment Error
#define RSR_CE (1 << 1)   // CRC Error
#define RSR_FOE (1 << 0)  // RX Memory Overflow Error

#define FCR_FLOW_ENABLE (0x39) // Enable Flow Control

#define EPCR_REEP (1 << 5)  // Reload EEPROM
#define EPCR_WEP (1 << 4)   // Write EEPROM Enable
#define EPCR_EPOS (1 << 3)  // EEPROM or PHY Operation Select
#define EPCR_ERPRR (1 << 2) // EEPROM Read or PHY Register Read Command
#define EPCR_ERPRW (1 << 1) // EEPROM Write or PHY Register Write Command
#define EPCR_ERRE (1 << 0)  // EEPROM Access Status or PHY Access Status

#define TCR2_RLCP (1 << 6) // Retry Late Collision Packet

#define ATCR_AUTO_TX (1 << 7) // Auto-Transmit Control

#define TCSCR_UDPCSE (1 << 2) // UDP CheckSum Generation
#define TCSCR_TCPCSE (1 << 1) // TCP CheckSum Generation
#define TCSCR_IPCSE (1 << 0)  // IPv4 CheckSum Generation

#define MPTRCR_RST_TX (1 << 1) // Reset TX Memory Pointer
#define MPTRCR_RST_RX (1 << 0) // Reset RX Memory Pointer

#define ISR_LNKCHGS (1 << 5) // Link Status Change
#define ISR_ROO (1 << 3)     // Receive Overflow Counter Overflow
#define ISR_ROS (1 << 2)     // Receive Overflow
#define ISR_PT (1 << 1)      // Packet Transmitted
#define ISR_PR (1 << 0)      // Packet Received
#define ISR_CLR_STATUS (ISR_LNKCHGS | ISR_ROO | ISR_ROS | ISR_PT | ISR_PR)

#define IMR_PAR (1 << 7)     // Pointer Auto-Return Mode
#define IMR_LNKCHGI (1 << 5) // Enable Link Status Change Interrupt
#define IMR_ROOI (1 << 3)    // Enable Receive Overflow Counter Overflow Interrupt
#define IMR_ROI (1 << 2)     // Enable Receive Overflow Interrupt
#define IMR_PTI (1 << 1)     // Enable Packet Transmitted Interrupt
#define IMR_PRI (1 << 0)     // Enable Packet Received Interrupt
#define IMR_ALL (IMR_PAR | IMR_LNKCHGI | IMR_ROOI | IMR_ROI | IMR_PTI | IMR_PRI)

#ifdef __cplusplus
}
#endif
