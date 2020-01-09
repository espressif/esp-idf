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

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_eth_mac.h"
#include "esp_eth_phy.h"
#include "driver/spi_master.h"

/**
 * @brief SPI Instruction Set
 *
 */
#define ENC28J60_SPI_CMD_RCR (0x00) // Read Control Register
#define ENC28J60_SPI_CMD_RBM (0x01) // Read Buffer Memory
#define ENC28J60_SPI_CMD_WCR (0x02) // Write Control Register
#define ENC28J60_SPI_CMD_WBM (0x03) // Write Buffer Memory
#define ENC28J60_SPI_CMD_BFS (0x04) // Bit Field Set
#define ENC28J60_SPI_CMD_BFC (0x05) // Bit Field Clear
#define ENC28J60_SPI_CMD_SRC (0x07) // Soft Reset

/**
 * @brief Shared Registers in ENC28J60 (accessible on each bank)
 *
 */
#define ENC28J60_EIE      (0x1B) // Ethernet Interrupt Enable
#define ENC28J60_EIR      (0x1C) // Ethernet Interrupt flags
#define ENC28J60_ESTAT    (0x1D) // Ethernet Status
#define ENC28J60_ECON2    (0x1E) // Ethernet Control Register2
#define ENC28J60_ECON1    (0x1F) // Ethernet Control Register1

/**
 * @brief Per-bank Registers in ENC28J60
 * @note Address[15:12]: Register Type, 0 -> ETH, 1 -> MII/MAC
 *       Address[11:8] : Bank address
 *       Address[7:0]  : Register Index
 */
// Bank 0 Registers
#define ENC28J60_ERDPTL   (0x0000) // Read Pointer Low Byte ERDPT<7:0>)
#define ENC28J60_ERDPTH   (0x0001) // Read Pointer High Byte (ERDPT<12:8>)
#define ENC28J60_EWRPTL   (0x0002) // Write Pointer Low Byte (EWRPT<7:0>)
#define ENC28J60_EWRPTH   (0x0003) // Write Pointer High Byte (EWRPT<12:8>)
#define ENC28J60_ETXSTL   (0x0004) // TX Start Low Byte (ETXST<7:0>)
#define ENC28J60_ETXSTH   (0x0005) // TX Start High Byte (ETXST<12:8>)
#define ENC28J60_ETXNDL   (0x0006) // TX End Low Byte (ETXND<7:0>)
#define ENC28J60_ETXNDH   (0x0007) // TX End High Byte (ETXND<12:8>)
#define ENC28J60_ERXSTL   (0x0008) // RX Start Low Byte (ERXST<7:0>)
#define ENC28J60_ERXSTH   (0x0009) // RX Start High Byte (ERXST<12:8>)
#define ENC28J60_ERXNDL   (0x000A) // RX End Low Byte (ERXND<7:0>)
#define ENC28J60_ERXNDH   (0x000B) // RX End High Byte (ERXND<12:8>)
#define ENC28J60_ERXRDPTL (0x000C) // RX RD Pointer Low Byte (ERXRDPT<7:0>)
#define ENC28J60_ERXRDPTH (0x000D) // RX RD Pointer High Byte (ERXRDPT<12:8>)
#define ENC28J60_ERXWRPTL (0x000E) // RX WR Pointer Low Byte (ERXWRPT<7:0>)
#define ENC28J60_ERXWRPTH (0x000F) // RX WR Pointer High Byte (ERXWRPT<12:8>)
#define ENC28J60_EDMASTL  (0x0010) // DMA Start Low Byte (EDMAST<7:0>)
#define ENC28J60_EDMASTH  (0x0011) // DMA Start High Byte (EDMAST<12:8>)
#define ENC28J60_EDMANDL  (0x0012) // DMA End Low Byte (EDMAND<7:0>)
#define ENC28J60_EDMANDH  (0x0013) // DMA End High Byte (EDMAND<12:8>)
#define ENC28J60_EDMADSTL (0x0014) // DMA Destination Low Byte (EDMADST<7:0>)
#define ENC28J60_EDMADSTH (0x0015) // DMA Destination High Byte (EDMADST<12:8>)
#define ENC28J60_EDMACSL  (0x0016) // DMA Checksum Low Byte (EDMACS<7:0>)
#define ENC28J60_EDMACSH  (0x0017) // DMA Checksum High Byte (EDMACS<15:8>)

// Bank 1 Registers
#define ENC28J60_EHT0     (0x0100) // Hash Table Byte 0 (EHT<7:0>)
#define ENC28J60_EHT1     (0x0101) // Hash Table Byte 1 (EHT<15:8>)
#define ENC28J60_EHT2     (0x0102) // Hash Table Byte 2 (EHT<23:16>)
#define ENC28J60_EHT3     (0x0103) // Hash Table Byte 3 (EHT<31:24>)
#define ENC28J60_EHT4     (0x0104) // Hash Table Byte 4 (EHT<39:32>)
#define ENC28J60_EHT5     (0x0105) // Hash Table Byte 5 (EHT<47:40>)
#define ENC28J60_EHT6     (0x0106) // Hash Table Byte 6 (EHT<55:48>)
#define ENC28J60_EHT7     (0x0107) // Hash Table Byte 7 (EHT<63:56>)
#define ENC28J60_EPMM0    (0x0108) // Pattern Match Mask Byte 0 (EPMM<7:0>)
#define ENC28J60_EPMM1    (0x0109) // Pattern Match Mask Byte 1 (EPMM<15:8>)
#define ENC28J60_EPMM2    (0x010A) // Pattern Match Mask Byte 2 (EPMM<23:16>)
#define ENC28J60_EPMM3    (0x010B) // Pattern Match Mask Byte 3 (EPMM<31:24>)
#define ENC28J60_EPMM4    (0x010C) // Pattern Match Mask Byte 4 (EPMM<39:32>)
#define ENC28J60_EPMM5    (0x010D) // Pattern Match Mask Byte 5 (EPMM<47:40>)
#define ENC28J60_EPMM6    (0x010E) // Pattern Match Mask Byte 6 (EPMM<55:48>)
#define ENC28J60_EPMM7    (0x010F) // Pattern Match Mask Byte 7 (EPMM<63:56>)
#define ENC28J60_EPMCSL   (0x0110) // Pattern Match Checksum Low Byte (EPMCS<7:0>)
#define ENC28J60_EPMCSH   (0x0111) // Pattern Match Checksum High Byte (EPMCS<15:0>)
#define ENC28J60_EPMOL    (0x0114) // Pattern Match Offset Low Byte (EPMO<7:0>)
#define ENC28J60_EPMOH    (0x0115) // Pattern Match Offset High Byte (EPMO<12:8>)
#define ENC28J60_ERXFCON  (0x0118) // Receive Fileter Control
#define ENC28J60_EPKTCNT  (0x0119) // Ethernet Packet Count

// Bank 2 Register
#define ENC28J60_MACON1   (0x1200) // MAC Control Register 1
#define ENC28J60_MACON2   (0x1201) // MAC Control Register 2
#define ENC28J60_MACON3   (0x1202) // MAC Control Register 3
#define ENC28J60_MACON4   (0x1203) // MAC Control Register 4
#define ENC28J60_MABBIPG  (0x1204) // Back-to-Back Inter-Packet Gap (BBIPG<6:0>)
#define ENC28J60_MAIPGL   (0x1206) // Non-Back-to-Back Inter-Packet Gap Low Byte (MAIPGL<6:0>)
#define ENC28J60_MAIPGH   (0x1207) // Non-Back-to-Back Inter-Packet Gap High Byte (MAIPGH<6:0>)
#define ENC28J60_MACLCON1 (0x1208) // Retransmission Maximum (RETMAX<3:0>)
#define ENC28J60_MACLCON2 (0x1209) // Collision Window (COLWIN<5:0>)
#define ENC28J60_MAMXFLL  (0x120A) // Maximum Frame Length Low Byte (MAMXFL<7:0>)
#define ENC28J60_MAMXFLH  (0x120B) // Maximum Frame Length High Byte (MAMXFL<15:8>)
#define ENC28J60_MICMD    (0x1212) // MII Command Register
#define ENC28J60_MIREGADR (0x1214) // MII Register Address (MIREGADR<4:0>)
#define ENC28J60_MIWRL    (0x1216) // MII Write Data Low Byte (MIWR<7:0>)
#define ENC28J60_MIWRH    (0x1217) // MII Write Data High Byte (MIWR<15:8>)
#define ENC28J60_MIRDL    (0x1218) // MII Read Data Low Byte (MIRD<7:0>)
#define ENC28J60_MIRDH    (0x1219) // MII Read Data High Byte(MIRD<15:8>)

// Bank 3 Registers
#define ENC28J60_MAADR5   (0x1300) // MAC Address Byte 5 (MAADR<15:8>)
#define ENC28J60_MAADR6   (0x1301) // MAC Address Byte 6 (MAADR<7:0>)
#define ENC28J60_MAADR3   (0x1302) // MAC Address Byte 3 (MAADR<31:24>), OUI Byte 3
#define ENC28J60_MAADR4   (0x1303) // MAC Address Byte 4 (MAADR<23:16>)
#define ENC28J60_MAADR1   (0x1304) // MAC Address Byte 1 (MAADR<47:40>), OUI Byte 1
#define ENC28J60_MAADR2   (0x1305) // MAC Address Byte 2 (MAADR<39:32>), OUI Byte 2
#define ENC28J60_EBSTSD   (0x0306) // Built-in Self-Test Fill Seed (EBSTSD<7:0>)
#define ENC28J60_EBSTCON  (0x0307) // Built-in Self-Test Control
#define ENC28J60_EBSTCSL  (0x0308) // Built-in Self-Test Checksum Low Byte (EBSTCS<7:0>)
#define ENC28J60_EBSTCSH  (0x0309) // Built-in Self-Test Checksum High Byte (EBSTCS<15:8>)
#define ENC28J60_MISTAT   (0x130A) // MII Status Register
#define ENC28J60_EREVID   (0x0312) // Ethernet Revision ID (EREVID<4:0>)
#define ENC28J60_ECOCON   (0x0315) // Clock Output Control Register
#define ENC28J60_EFLOCON  (0x0317) // Ethernet Flow Control
#define ENC28J60_EPAUSL   (0x0318) // Pause Timer Value Low Byte (EPAUS<7:0>)
#define ENC28J60_EPAUSH   (0x0319) // Pause Timer Value High Byte (EPAUS<15:8>)

/**
 * @brief status and flag of ENC28J60 specific registers
 *
 */
// EIE bit definitions
#define EIE_INTIE     (1<<7) // Global INT Interrupt Enable
#define EIE_PKTIE     (1<<6) // Receive Packet Pending Interrupt Enable
#define EIE_DMAIE     (1<<5) // DMA Interrupt Enable
#define EIE_LINKIE    (1<<4) // Link Status Change Interrupt Enable
#define EIE_TXIE      (1<<3) // Transmit Enable
#define EIE_TXERIE    (1<<1) // Transmit Error Interrupt Enable
#define EIE_RXERIE    (1<<0) // Receive Error Interrupt Enable

// EIR bit definitions
#define EIR_PKTIF     (1<<6) // Receive Packet Pending Interrupt Flag
#define EIR_DMAIF     (1<<5) // DMA Interrupt Flag
#define EIR_LINKIF    (1<<4) // Link Change Interrupt Flag
#define EIR_TXIF      (1<<3) // Transmit Interrupt Flag
#define EIR_TXERIF    (1<<1) // Transmit Error Interrupt Flag
#define EIR_RXERIF    (1<<0) // Receive Error Interrupt Flag

// ESTAT bit definitions
#define ESTAT_INT       (1<<7) // INT Interrupt Flag
#define ESTAT_BUFER     (1<<6) // Buffer Error Status
#define ESTAT_LATECOL   (1<<4) // Late Collision Error
#define ESTAT_RXBUSY    (1<<2) // Receive Busy
#define ESTAT_TXABRT    (1<<1) // Transmit Abort Error
#define ESTAT_CLKRDY    (1<<0) // Clock Ready

// ECON2 bit definitions
#define ECON2_AUTOINC   (1<<7) // Automatic Buffer Pointer Increment Enable
#define ECON2_PKTDEC    (1<<6) // Packet Decrement
#define ECON2_PWRSV     (1<<5) // Power Save Enable
#define ECON2_VRPS      (1<<3) // Voltage Regulator Power Save Enable

// ECON1 bit definitions
#define ECON1_TXRST     (1<<7) // Transmit Logic Reset
#define ECON1_RXRST     (1<<6) // Receive Logic Reset
#define ECON1_DMAST     (1<<5) // DMA Start and Busy Status
#define ECON1_CSUMEN    (1<<4) // DMA Checksum Enable
#define ECON1_TXRTS     (1<<3) // Transmit Request to Send
#define ECON1_RXEN      (1<<2) // Receive Enable
#define ECON1_BSEL1     (1<<1) // Bank Select1
#define ECON1_BSEL0     (1<<0) // Bank Select0

// ERXFCON bit definitions
#define ERXFCON_UCEN    (1<<7) // Unicast Filter Enable
#define ERXFCON_ANDOR   (1<<6) // AND/OR Filter Select
#define ERXFCON_CRCEN   (1<<5) // Post-Filter CRC Check Enable
#define ERXFCON_PMEN    (1<<4) // Pattern Match Filter Enable
#define ERXFCON_MPEN    (1<<3) // Magic Packet Filter Enable
#define ERXFCON_HTEN    (1<<2) // Hash Table Filter Enable
#define ERXFCON_MCEN    (1<<1) // Multicast Filter Enable
#define ERXFCON_BCEN    (1<<0) // Broadcast Filter Enable

// MACON1 bit definitions
#define MACON1_TXPAUS   (1<<3) // Pause Control Frame Transmission Enable
#define MACON1_RXPAUS   (1<<2) // Pause Control Frame Reception Enable
#define MACON1_PASSALL  (1<<1) // Pass All Received Frames Enable
#define MACON1_MARXEN   (1<<0) // MAC Receive Enable

// MACON3 bit definitions
#define MACON3_PADCFG2   (1<<7) // Automatic Pad and CRC Configuration bit 2
#define MACON3_PADCFG1   (1<<6) // Automatic Pad and CRC Configuration bit 1
#define MACON3_PADCFG0   (1<<5) // Automatic Pad and CRC Configuration bit 0
#define MACON3_TXCRCEN   (1<<4) // Transmit CRC Enable
#define MACON3_PHDRLEN   (1<<3) // Proprietary Header Enable
#define MACON3_HFRMLEN   (1<<2) // Huge Frame Enable
#define MACON3_FRMLNEN   (1<<1) // Frame Length Checking Enable
#define MACON3_FULDPX    (1<<0) // MAC Full-Duplex Enable

// MACON4 bit definitions
#define MACON4_DEFER     (1<<6) // Defer Transmission Enable
#define MACON4_BPEN      (1<<5) // No Backoff During Backpressure Enable
#define MACON4_NOBKFF    (1<<4) // No Backoff Enable

// MICMD bit definitions
#define MICMD_MIISCAN    (1<<1) // MII Scan Enable
#define MICMD_MIIRD      (1<<0) // MII Read Enable

// EBSTCON bit definitions
#define EBSTCON_PSV2     (1<<7) // Pattern Shift Value 2
#define EBSTCON_PSV1     (1<<6) // Pattern Shift Value 1
#define EBSTCON_PSV0     (1<<5) // Pattern Shift Value 0
#define EBSTCON_PSEL     (1<<4) // Port Select
#define EBSTCON_TMSEL1   (1<<3) // Test Mode Select 1
#define EBSTCON_TMSEL0   (1<<2) // Test Mode Select 0
#define EBSTCON_TME      (1<<1) // Test Mode Enable
#define EBSTCON_BISTST   (1<<0) // Built-in Self-Test Start/Busy

// MISTAT bit definitions
#define MISTAT_NVALID    (1<<2) // MII Management Read Data Not Valid
#define MISTAT_SCAN      (1<<1) // MII Management Scan Operation in Progress
#define MISTAT_BUSY      (1<<0) // MII Management Busy

// EFLOCON bit definitions
#define EFLOCON_FULDPXS  (1<<2) // Full-Duplex Shadown
#define EFLOCON_FCEN1    (1<<1) // Flow Control Enable 1
#define EFLOCON_FCEN0    (1<<0) // Flow Control Enable 0

/**
 * @brief ENC28J60 specific configuration
 *
 */
typedef struct {
    spi_device_handle_t spi_hdl; /*!< Handle of SPI device driver */
    int int_gpio_num;            /*!< Interrupt GPIO number */
} eth_enc28j60_config_t;

/**
 * @brief Default ENC28J60 specific configuration
 *
 */
#define ETH_ENC28J60_DEFAULT_CONFIG(spi_device) \
    {                                           \
        .spi_hdl = spi_device,                  \
        .int_gpio_num = 4,                      \
    }

/**
* @brief Create ENC28J60 Ethernet MAC instance
*
* @param[in] enc28j60_config: ENC28J60 specific configuration
* @param[in] mac_config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_enc28j60(const eth_enc28j60_config_t *enc28j60_config, const eth_mac_config_t *mac_config);

/**
* @brief Create a PHY instance of ENC28J60
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_enc28j60(const eth_phy_config_t *config);

#ifdef __cplusplus
}
#endif
