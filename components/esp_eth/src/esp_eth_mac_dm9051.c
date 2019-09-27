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
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"

static const char *TAG = "emac_dm9051";
#define MAC_CHECK(a, str, goto_tag, ret_value, ...)                               \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

#define RX_QUEUE_WAIT_MS (100)
#define DM9051_SPI_LOCK_TIMEOUT_MS (50)
#define DM9051_PHY_OPERATION_TIMEOUT_US (1000)

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

#define RCR_WTDIS (1 << 6)    // Watchdog Timer Disable
#define RCR_DIS_LONG (1 << 5) // Discard Long Packet
#define RCR_DIS_CRC (1 << 4)  // Discard CRC Error Packet
#define RCR_ALL (1 << 3)      // Receive All Multicast
#define RCR_RUNT (1 << 2)     // Receive Runt Packet
#define RCR_PRMSC (1 << 1)    // Promiscuous Mode
#define RCR_RXEN (1 << 0)     // RX Enable

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

typedef struct {
    uint8_t flag;
    uint8_t status;
    uint8_t length_low;
    uint8_t length_high;
} dm9051_rx_header_t;

typedef struct {
    esp_eth_mac_t parent;
    esp_eth_mediator_t *eth;
    spi_device_handle_t spi_hdl;
    SemaphoreHandle_t spi_lock;
    TaskHandle_t rx_task_hdl;
    uint32_t sw_reset_timeout_ms;
    uint8_t addr[6];
    bool packets_remain;
} emac_dm9051_t;

static inline bool dm9051_lock(emac_dm9051_t *emac)
{
    return xSemaphoreTake(emac->spi_lock, DM9051_SPI_LOCK_TIMEOUT_MS) == pdTRUE;
}

static inline bool dm9051_unlock(emac_dm9051_t *emac)
{
    return xSemaphoreGive(emac->spi_lock) == pdTRUE;
}

/**
 * @brief write value to dm9051 internal register
 */
static esp_err_t dm9051_register_write(emac_dm9051_t *emac, uint8_t reg_addr, uint8_t value)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = DM9051_SPI_WR,
        .addr = reg_addr,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA
    };
    trans.tx_data[0] = value;
    if (dm9051_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        dm9051_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief read value from dm9051 internal register
 */
static esp_err_t dm9051_register_read(emac_dm9051_t *emac, uint8_t reg_addr, uint8_t *value)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = DM9051_SPI_RD,
        .addr = reg_addr,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA
    };
    if (dm9051_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        } else {
            *value = trans.rx_data[0];
        }
        dm9051_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief write buffer to dm9051 internal memory
 */
static esp_err_t dm9051_memory_write(emac_dm9051_t *emac, uint8_t *buffer, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = DM9051_SPI_WR,
        .addr = DM9051_MWCMD,
        .length = len * 8,
        .tx_buffer = buffer
    };
    if (dm9051_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        dm9051_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief read buffer from dm9051 internal memory
 */
static esp_err_t dm9051_memory_read(emac_dm9051_t *emac, uint8_t *buffer, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = DM9051_SPI_RD,
        .addr = DM9051_MRCMD,
        .length = len * 8,
        .rx_buffer = buffer
    };
    if (dm9051_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        dm9051_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief read mac address from internal registers
 */
static esp_err_t dm9051_get_mac_addr(emac_dm9051_t *emac)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < 6; i++) {
        MAC_CHECK(dm9051_register_read(emac, DM9051_PAR + i, &emac->addr[i]) == ESP_OK, "read PAR failed", err, ESP_FAIL);
    }
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief set new mac address to internal registers
 */
static esp_err_t dm9051_set_mac_addr(emac_dm9051_t *emac)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < 6; i++) {
        MAC_CHECK(dm9051_register_write(emac, DM9051_PAR + i, emac->addr[i]) == ESP_OK, "write PAR failed", err, ESP_FAIL);
    }
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief clear multicast hash table
 */
static esp_err_t dm9051_clear_multicast_table(emac_dm9051_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* rx broadcast packet control by bit7 of MAC register 1DH */
    MAC_CHECK(dm9051_register_write(emac, DM9051_BCASTCR, 0x00) == ESP_OK, "write BCASTCR failed", err, ESP_FAIL);
    for (int i = 0; i < 7; i++) {
        MAC_CHECK(dm9051_register_write(emac, DM9051_MAR + i, 0x00) == ESP_OK, "write MAR failed", err, ESP_FAIL);
    }
    /* enable receive broadcast paclets */
    MAC_CHECK(dm9051_register_write(emac, DM9051_MAR + 7, 0x80) == ESP_OK, "write MAR failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief software reset dm9051 internal register
 */
static esp_err_t dm9051_reset(emac_dm9051_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* power on phy */
    MAC_CHECK(dm9051_register_write(emac, DM9051_GPR, 0x00) == ESP_OK, "write GPR failed", err, ESP_FAIL);
    /* mac and phy register won't be accesable within at least 1ms */
    vTaskDelay(pdMS_TO_TICKS(10));
    /* software reset */
    uint8_t ncr = NCR_RST;
    MAC_CHECK(dm9051_register_write(emac, DM9051_NCR, ncr) == ESP_OK, "write NCR failed", err, ESP_FAIL);
    uint32_t to = 0;
    for (to = 0; to < emac->sw_reset_timeout_ms / 10; to++) {
        MAC_CHECK(dm9051_register_read(emac, DM9051_NCR, &ncr) == ESP_OK, "read NCR failed", err, ESP_FAIL);
        if (!(ncr & NCR_RST)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    MAC_CHECK(to < emac->sw_reset_timeout_ms / 10, "reset timeout", err, ESP_ERR_TIMEOUT);
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief verify dm9051 chip ID
 */
static esp_err_t dm9051_verify_id(emac_dm9051_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t id[2];
    MAC_CHECK(dm9051_register_read(emac, DM9051_VIDL, &id[0]) == ESP_OK, "read VIDL failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_read(emac, DM9051_VIDH, &id[1]) == ESP_OK, "read VIDH failed", err, ESP_FAIL);
    MAC_CHECK(0x0A46 == *(uint16_t *)id, "wrong Vendor ID", err, ESP_ERR_INVALID_VERSION);
    MAC_CHECK(dm9051_register_read(emac, DM9051_PIDL, &id[0]) == ESP_OK, "read PIDL failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_read(emac, DM9051_PIDH, &id[1]) == ESP_OK, "read PIDH failed", err, ESP_FAIL);
    MAC_CHECK(0x9051 == *(uint16_t *)id, "wrong Product ID", err, ESP_ERR_INVALID_VERSION);
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief default setup for dm9051 internal registers
 */
static esp_err_t dm9051_setup_default(emac_dm9051_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* disable wakeup */
    MAC_CHECK(dm9051_register_write(emac, DM9051_NCR, 0x00) == ESP_OK, "write NCR failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_write(emac, DM9051_WCR, 0x00) == ESP_OK, "write WCR failed", err, ESP_FAIL);
    /* stop transmitting, enable appending pad, crc for packets */
    MAC_CHECK(dm9051_register_write(emac, DM9051_TCR, 0x00) == ESP_OK, "write TCR failed", err, ESP_FAIL);
    /* stop receiving, no promiscuous mode, no runt packet(size < 64bytes), not all multicast packets*/
    /* discard long packet(size > 1522bytes) and crc error packet, enable watchdog */
    MAC_CHECK(dm9051_register_write(emac, DM9051_RCR, RCR_DIS_LONG | RCR_DIS_CRC) == ESP_OK, "write RCR failed", err, ESP_FAIL);
    /* send jam pattern (duration time = 1.15ms) when rx free space < 3k bytes */
    MAC_CHECK(dm9051_register_write(emac, DM9051_BPTR, 0x3F) == ESP_OK, "write BPTR failed", err, ESP_FAIL);
    /* flow control: high water threshold = 3k bytes, low water threshold = 8k bytes */
    MAC_CHECK(dm9051_register_write(emac, DM9051_FCTR, 0x38) == ESP_OK, "write FCTR failed", err, ESP_FAIL);
    /* enable flow control */
    MAC_CHECK(dm9051_register_write(emac, DM9051_FCR, FCR_FLOW_ENABLE) == ESP_OK, "write FCR failed", err, ESP_FAIL);
    /* retry late collision packet, at most two transmit command can be issued before transmit complete */
    MAC_CHECK(dm9051_register_write(emac, DM9051_TCR2, TCR2_RLCP) == ESP_OK, "write TCR2 failed", err, ESP_FAIL);
    /* enable auto transmit */
    MAC_CHECK(dm9051_register_write(emac, DM9051_ATCR, ATCR_AUTO_TX) == ESP_OK, "write ATCR failed", err, ESP_FAIL);
    /* generate checksum for UDP, TCP and IPv4 packets */
    MAC_CHECK(dm9051_register_write(emac, DM9051_TCSCR, TCSCR_IPCSE | TCSCR_TCPCSE | TCSCR_UDPCSE) == ESP_OK,
              "write TCSCR failed", err, ESP_FAIL);
    /* disable check sum for receive packets */
    MAC_CHECK(dm9051_register_write(emac, DM9051_RCSCSR, 0x00) == ESP_OK, "write RCSCSR failed", err, ESP_FAIL);
    /* interrupt pin config: push-pull output, active high */
    MAC_CHECK(dm9051_register_write(emac, DM9051_INTCR, 0x00) == ESP_OK, "write INTCR failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_write(emac, DM9051_INTCKCR, 0x00) == ESP_OK, "write INTCKCR failed", err, ESP_FAIL);
    /* no length limitation for rx packets */
    MAC_CHECK(dm9051_register_write(emac, DM9051_RLENCR, 0x00) == ESP_OK, "write RLENCR failed", err, ESP_FAIL);
    /* 3K-byte for TX and 13K-byte for RX */
    MAC_CHECK(dm9051_register_write(emac, DM9051_MEMSCR, 0x00) == ESP_OK, "write MEMSCR failed", err, ESP_FAIL);
    /* reset tx and rx memory pointer */
    MAC_CHECK(dm9051_register_write(emac, DM9051_MPTRCR, MPTRCR_RST_RX | MPTRCR_RST_TX) == ESP_OK,
              "write MPTRCR failed", err, ESP_FAIL);
    /* clear network status: wakeup event, tx complete */
    MAC_CHECK(dm9051_register_write(emac, DM9051_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END) == ESP_OK, "write NSR failed", err, ESP_FAIL);
    /* clear interrupt status */
    MAC_CHECK(dm9051_register_write(emac, DM9051_ISR, ISR_CLR_STATUS) == ESP_OK, "write ISR failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief start dm9051: enable interrupt and start receive
 */
static esp_err_t dm9051_start(emac_dm9051_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* enable interrupt */
    MAC_CHECK(dm9051_register_write(emac, DM9051_IMR, IMR_ALL) == ESP_OK, "write IMR failed", err, ESP_FAIL);
    /* enable rx */
    uint8_t rcr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_RCR, &rcr) == ESP_OK, "read RCR failed", err, ESP_FAIL);
    rcr |= RCR_RXEN;
    MAC_CHECK(dm9051_register_write(emac, DM9051_RCR, rcr) == ESP_OK, "write RCR failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief stop dm9051: disable interrupt and stop receive
 */
static esp_err_t dm9051_stop(emac_dm9051_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* disable interrupt */
    MAC_CHECK(dm9051_register_write(emac, DM9051_IMR, 0x00) == ESP_OK, "write IMR failed", err, ESP_FAIL);
    /* disable rx */
    uint8_t rcr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_RCR, &rcr) == ESP_OK, "read RCR failed", err, ESP_FAIL);
    rcr &= ~RCR_RXEN;
    MAC_CHECK(dm9051_register_write(emac, DM9051_RCR, rcr) == ESP_OK, "write RCR failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

static void dm9051_isr_handler(void *arg)
{
    emac_dm9051_t *emac = (emac_dm9051_t *)arg;
    BaseType_t high_task_wakeup = pdFALSE;
    /* notify dm9051 task */
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup != pdFALSE) {
        portYIELD_FROM_ISR();
    }
}

static void emac_dm9051_task(void *arg)
{
    emac_dm9051_t *emac = (emac_dm9051_t *)arg;
    uint8_t status = 0;
    uint8_t *buffer = NULL;
    uint32_t length = 0;
    while (1) {
        if (ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(RX_QUEUE_WAIT_MS))) {
            /* clear interrupt status */
            dm9051_register_read(emac, DM9051_ISR, &status);
            dm9051_register_write(emac, DM9051_ISR, status);
            /* packet received */
            if (status & ISR_PR) {
                do {
                    buffer = (uint8_t *)heap_caps_malloc(ETH_MAX_PACKET_SIZE, MALLOC_CAP_DMA);
                    if (emac->parent.receive(&emac->parent, buffer, &length) == ESP_OK) {
                        /* pass the buffer to stack (e.g. TCP/IP layer) */
                        if (length) {
                            emac->eth->stack_input(emac->eth, buffer, length);
                        } else {
                            free(buffer);
                        }
                    } else {
                        free(buffer);
                    }
                } while (emac->packets_remain);
            }
        }
    }
    vTaskDelete(NULL);
}

static esp_err_t emac_dm9051_set_mediator(esp_eth_mac_t *mac, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(eth, "can't set mac's mediator to null", err, ESP_ERR_INVALID_ARG);
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    emac->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_write_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    /* check if phy access is in progress */
    uint8_t epcr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_EPCR, &epcr) == ESP_OK, "read EPCR failed", err, ESP_FAIL);
    MAC_CHECK(!(epcr & EPCR_ERRE), "phy is busy", err, ESP_ERR_INVALID_STATE);
    MAC_CHECK(dm9051_register_write(emac, DM9051_EPAR, (uint8_t)(((phy_addr << 6) & 0xFF) | phy_reg)) == ESP_OK,
              "write EPAR failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_write(emac, DM9051_EPDRL, (uint8_t)(reg_value & 0xFF)) == ESP_OK,
              "write EPDRL failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_write(emac, DM9051_EPDRH, (uint8_t)((reg_value >> 8) & 0xFF)) == ESP_OK,
              "write EPDRH failed", err, ESP_FAIL);
    /* select PHY and select write operation */
    MAC_CHECK(dm9051_register_write(emac, DM9051_EPCR, EPCR_EPOS | EPCR_ERPRW) == ESP_OK, "write EPCR failed", err, ESP_FAIL);
    /* polling the busy flag */
    uint32_t to = 0;
    do {
        ets_delay_us(100);
        MAC_CHECK(dm9051_register_read(emac, DM9051_EPCR, &epcr) == ESP_OK, "read EPCR failed", err, ESP_FAIL);
        to += 100;
    } while ((epcr & EPCR_ERRE) && to < DM9051_PHY_OPERATION_TIMEOUT_US);
    MAC_CHECK(!(epcr & EPCR_ERRE), "phy is busy", err, ESP_ERR_TIMEOUT);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(reg_value, "can't set reg_value to null", err, ESP_ERR_INVALID_ARG);
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    /* check if phy access is in progress */
    uint8_t epcr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_EPCR, &epcr) == ESP_OK, "read EPCR failed", err, ESP_FAIL);
    MAC_CHECK(!(epcr & 0x01), "phy is busy", err, ESP_ERR_INVALID_STATE);
    MAC_CHECK(dm9051_register_write(emac, DM9051_EPAR, (uint8_t)(((phy_addr << 6) & 0xFF) | phy_reg)) == ESP_OK,
              "write EPAR failed", err, ESP_FAIL);
    /* Select PHY and select read operation */
    MAC_CHECK(dm9051_register_write(emac, DM9051_EPCR, 0x0C) == ESP_OK, "write EPCR failed", err, ESP_FAIL);
    /* polling the busy flag */
    uint32_t to = 0;
    do {
        ets_delay_us(100);
        MAC_CHECK(dm9051_register_read(emac, DM9051_EPCR, &epcr) == ESP_OK, "read EPCR failed", err, ESP_FAIL);
        to += 100;
    } while ((epcr & EPCR_ERRE) && to < DM9051_PHY_OPERATION_TIMEOUT_US);
    MAC_CHECK(!(epcr & EPCR_ERRE), "phy is busy", err, ESP_ERR_TIMEOUT);
    uint8_t value_h = 0;
    uint8_t value_l = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_EPDRH, &value_h) == ESP_OK, "read EPDRH failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_read(emac, DM9051_EPDRL, &value_l) == ESP_OK, "read EPDRL failed", err, ESP_FAIL);
    *reg_value = (value_h << 8) | value_l;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "can't set mac addr to null", err, ESP_ERR_INVALID_ARG);
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    memcpy(emac->addr, addr, 6);
    MAC_CHECK(dm9051_set_mac_addr(emac) == ESP_OK, "set mac address failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "can't set mac addr to null", err, ESP_ERR_INVALID_ARG);
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    memcpy(addr, emac->addr, 6);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    uint8_t nsr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_NSR, &nsr) == ESP_OK, "read NSR failed", err, ESP_FAIL);
    switch (link) {
    case ETH_LINK_UP:
        MAC_CHECK(nsr & NSR_LINKST, "phy is not link up", err, ESP_ERR_INVALID_STATE);
        MAC_CHECK(dm9051_start(emac) == ESP_OK, "dm9051 start failed", err, ESP_FAIL);
        break;
    case ETH_LINK_DOWN:
        MAC_CHECK(!(nsr & NSR_LINKST), "phy is not link down", err, ESP_ERR_INVALID_STATE);
        MAC_CHECK(dm9051_stop(emac) == ESP_OK, "dm9051 stop failed", err, ESP_FAIL);
        break;
    default:
        MAC_CHECK(false, "unknown link status", err, ESP_ERR_INVALID_ARG);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    uint8_t nsr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_NSR, &nsr) == ESP_OK, "read NSR failed", err, ESP_FAIL);
    switch (speed) {
    case ETH_SPEED_10M:
        MAC_CHECK(nsr & NSR_SPEED, "phy speed is not at 10Mbps", err, ESP_ERR_INVALID_STATE);
        break;
    case ETH_SPEED_100M:
        MAC_CHECK(!(nsr & NSR_SPEED), "phy speed is not at 100Mbps", err, ESP_ERR_INVALID_STATE);
        break;
    default:
        MAC_CHECK(false, "unknown speed", err, ESP_ERR_INVALID_ARG);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_set_duplex(esp_eth_mac_t *mac, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    uint8_t ncr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_NCR, &ncr) == ESP_OK, "read NCR failed", err, ESP_FAIL);
    switch (duplex) {
    case ETH_DUPLEX_HALF:
        MAC_CHECK(!(ncr & NCR_FDX), "phy is not at half duplex", err, ESP_ERR_INVALID_STATE);
        break;
    case ETH_DUPLEX_FULL:
        MAC_CHECK(ncr & NCR_FDX, "phy is not at full duplex", err, ESP_ERR_INVALID_STATE);
        break;
    default:
        MAC_CHECK(false, "unknown duplex", err, ESP_ERR_INVALID_ARG);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    uint8_t rcr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_EPDRL, &rcr) == ESP_OK, "read RCR failed", err, ESP_FAIL);
    if (enable) {
        rcr |= RCR_PRMSC;
    } else {
        rcr &= ~RCR_PRMSC;
    }
    MAC_CHECK(dm9051_register_write(emac, DM9051_RCR, rcr) == ESP_OK, "write RCR failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    MAC_CHECK(buf, "can't set buf to null", err, ESP_ERR_INVALID_ARG);
    MAC_CHECK(length, "buf length can't be zero", err, ESP_ERR_INVALID_ARG);
    /* Check if last transmit complete */
    uint8_t tcr = 0;
    MAC_CHECK(dm9051_register_read(emac, DM9051_TCR, &tcr) == ESP_OK, "read TCR failed", err, ESP_FAIL);
    MAC_CHECK(!(tcr & TCR_TXREQ), "last transmit still in progress", err, ESP_ERR_INVALID_STATE);
    /* set tx length */
    MAC_CHECK(dm9051_register_write(emac, DM9051_TXPLL, length & 0xFF) == ESP_OK, "write TXPLL failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_write(emac, DM9051_TXPLH, (length >> 8) & 0xFF) == ESP_OK, "write TXPLH failed", err, ESP_FAIL);
    /* copy data to tx memory */
    MAC_CHECK(dm9051_memory_write(emac, buf, length) == ESP_OK, "write memory failed", err, ESP_FAIL);
    /* issue tx polling command */
    tcr |= TCR_TXREQ;
    MAC_CHECK(dm9051_register_write(emac, DM9051_TCR, tcr) == ESP_OK, "write TCR failed", err, ESP_FAIL);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    MAC_CHECK(buf && length, "can't set buf and length to null", err, ESP_ERR_INVALID_ARG);
    uint8_t rxbyte = 0;
    uint16_t rx_len = 0;
    __attribute__((aligned(4))) dm9051_rx_header_t header; // SPI driver needs the rx buffer 4 byte align
    emac->packets_remain = false;
    /* dummy read, get the most updated data */
    MAC_CHECK(dm9051_register_read(emac, DM9051_MRCMDX, &rxbyte) == ESP_OK, "read MRCMDX failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_read(emac, DM9051_MRCMDX, &rxbyte) == ESP_OK, "read MRCMDX failed", err, ESP_FAIL);
    /* rxbyte must be 0xFF, 0 or 1 */
    if (rxbyte > 1) {
        MAC_CHECK(dm9051_stop(emac) == ESP_OK, "stop dm9051 failed", err, ESP_FAIL);
        /* reset rx fifo pointer */
        MAC_CHECK(dm9051_register_write(emac, DM9051_MPTRCR, MPTRCR_RST_RX) == ESP_OK, "write MPTRCR failed", err, ESP_FAIL);
        ets_delay_us(10);
        MAC_CHECK(dm9051_start(emac) == ESP_OK, "start dm9051 failed", err, ESP_FAIL);
        MAC_CHECK(false, "reset rx fifo pointer", err, ESP_FAIL);
    } else if (rxbyte) {
        MAC_CHECK(dm9051_memory_read(emac, (uint8_t *)&header, sizeof(header)) == ESP_OK, "read rx header failed", err, ESP_FAIL);
        rx_len = header.length_low + (header.length_high << 8);
        MAC_CHECK(dm9051_memory_read(emac, buf, rx_len) == ESP_OK, "read rx data failed", err, ESP_FAIL);
        MAC_CHECK(!(header.status & 0xBF), "receive status error: %xH", err, ESP_FAIL, header.status);
        *length = rx_len - 4; // substract the CRC length (4Bytes)
        /* dummy read, get the most updated data */
        MAC_CHECK(dm9051_register_read(emac, DM9051_MRCMDX, &rxbyte) == ESP_OK, "read MRCMDX failed", err, ESP_FAIL);
        MAC_CHECK(dm9051_register_read(emac, DM9051_MRCMDX, &rxbyte) == ESP_OK, "read MRCMDX failed", err, ESP_FAIL);
        emac->packets_remain = rxbyte > 0;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    /* init gpio used by spi-ethernet interrupt */
    gpio_pad_select_gpio(CONFIG_ETH_DM9051_INT_GPIO);
    gpio_set_direction(CONFIG_ETH_DM9051_INT_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CONFIG_ETH_DM9051_INT_GPIO, GPIO_PULLDOWN_ONLY);
    gpio_set_intr_type(CONFIG_ETH_DM9051_INT_GPIO, GPIO_INTR_POSEDGE);
    gpio_intr_enable(CONFIG_ETH_DM9051_INT_GPIO);
    gpio_isr_handler_add(CONFIG_ETH_DM9051_INT_GPIO, dm9051_isr_handler, emac);
    MAC_CHECK(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL) == ESP_OK, "lowlevel init failed", err, ESP_FAIL);
    /* reset dm9051 */
    MAC_CHECK(dm9051_reset(emac) == ESP_OK, "reset dm9051 failed", err, ESP_FAIL);
    /* verify chip id */
    MAC_CHECK(dm9051_verify_id(emac) == ESP_OK, "vefiry chip ID failed", err, ESP_FAIL);
    /* default setup of internal registers */
    MAC_CHECK(dm9051_setup_default(emac) == ESP_OK, "dm9051 default setup failed", err, ESP_FAIL);
    /* clear multicast hash table */
    MAC_CHECK(dm9051_clear_multicast_table(emac) == ESP_OK, "clear multicast table failed", err, ESP_FAIL);
    /* get emac address from eeprom */
    MAC_CHECK(dm9051_get_mac_addr(emac) == ESP_OK, "fetch ethernet mac address failed", err, ESP_FAIL);
    return ESP_OK;
err:
    gpio_isr_handler_remove(CONFIG_ETH_DM9051_INT_GPIO);
    gpio_reset_pin(CONFIG_ETH_DM9051_INT_GPIO);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_dm9051_deinit(esp_eth_mac_t *mac)
{
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    dm9051_stop(emac);
    gpio_isr_handler_remove(CONFIG_ETH_DM9051_INT_GPIO);
    gpio_reset_pin(CONFIG_ETH_DM9051_INT_GPIO);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ESP_OK;
}

static esp_err_t emac_dm9051_del(esp_eth_mac_t *mac)
{
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    vTaskDelete(emac->rx_task_hdl);
    vSemaphoreDelete(emac->spi_lock);
    free(emac);
    return ESP_OK;
}

esp_eth_mac_t *esp_eth_mac_new_dm9051(const eth_dm9051_config_t *dm9051_config, const eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *ret = NULL;
    MAC_CHECK(dm9051_config, "can't set dm9051 specific config to null", err, NULL);
    MAC_CHECK(mac_config, "can't set mac config to null", err, NULL);
    emac_dm9051_t *emac = calloc(1, sizeof(emac_dm9051_t));
    MAC_CHECK(emac, "calloc emac failed", err, NULL);
    /* bind methods and attributes */
    emac->sw_reset_timeout_ms = mac_config->sw_reset_timeout_ms;
    emac->spi_hdl = dm9051_config->spi_hdl;
    emac->parent.set_mediator = emac_dm9051_set_mediator;
    emac->parent.init = emac_dm9051_init;
    emac->parent.deinit = emac_dm9051_deinit;
    emac->parent.del = emac_dm9051_del;
    emac->parent.write_phy_reg = emac_dm9051_write_phy_reg;
    emac->parent.read_phy_reg = emac_dm9051_read_phy_reg;
    emac->parent.set_addr = emac_dm9051_set_addr;
    emac->parent.get_addr = emac_dm9051_get_addr;
    emac->parent.set_speed = emac_dm9051_set_speed;
    emac->parent.set_duplex = emac_dm9051_set_duplex;
    emac->parent.set_link = emac_dm9051_set_link;
    emac->parent.set_promiscuous = emac_dm9051_set_promiscuous;
    emac->parent.transmit = emac_dm9051_transmit;
    emac->parent.receive = emac_dm9051_receive;
    /* create mutex */
    emac->spi_lock = xSemaphoreCreateMutex();
    MAC_CHECK(emac->spi_lock, "create lock failed", err_lock, NULL);
    /* create dm9051 task */
    BaseType_t xReturned = xTaskCreate(emac_dm9051_task, "dm9051_tsk", mac_config->rx_task_stack_size, emac,
                                       mac_config->rx_task_prio, &emac->rx_task_hdl);
    MAC_CHECK(xReturned == pdPASS, "create dm9051 task failed", err_tsk, NULL);
    return &(emac->parent);
err_tsk:
    vSemaphoreDelete(emac->spi_lock);
err_lock:
    free(emac);
err:
    return ret;
}
