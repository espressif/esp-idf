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
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "dm9051.h"
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

#define DM9051_SPI_LOCK_TIMEOUT_MS (50)
#define DM9051_PHY_OPERATION_TIMEOUT_US (1000)

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
    int int_gpio_num;
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
 * @brief peek buffer from dm9051 internal memory (without internal cursor moved)
 */
static esp_err_t dm9051_memory_peek(emac_dm9051_t *emac, uint8_t *buffer, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = DM9051_SPI_RD,
        .addr = DM9051_MRCMDX1,
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
    MAC_CHECK(0x0A == id[1] && 0x46 == id[0], "wrong Vendor ID", err, ESP_ERR_INVALID_VERSION);
    MAC_CHECK(dm9051_register_read(emac, DM9051_PIDL, &id[0]) == ESP_OK, "read PIDL failed", err, ESP_FAIL);
    MAC_CHECK(dm9051_register_read(emac, DM9051_PIDH, &id[1]) == ESP_OK, "read PIDH failed", err, ESP_FAIL);
    MAC_CHECK(0x90 == id[1] && 0x51 == id[0], "wrong Product ID", err, ESP_ERR_INVALID_VERSION);
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

IRAM_ATTR static void dm9051_isr_handler(void *arg)
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
        // block indefinitely until some task notifies me
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        /* clear interrupt status */
        dm9051_register_read(emac, DM9051_ISR, &status);
        dm9051_register_write(emac, DM9051_ISR, status);
        /* packet received */
        if (status & ISR_PR) {
            do {
                length = ETH_MAX_PACKET_SIZE;
                buffer = (uint8_t *)heap_caps_malloc(length, MALLOC_CAP_DMA);
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
        MAC_CHECK(dm9051_register_write(emac, DM9051_MPTRCR, MPTRCR_RST_RX) == ESP_OK,
                  "write MPTRCR failed", err, ESP_FAIL);
        ets_delay_us(10);
        MAC_CHECK(dm9051_start(emac) == ESP_OK, "start dm9051 failed", err, ESP_FAIL);
        MAC_CHECK(false, "reset rx fifo pointer", err, ESP_FAIL);
    } else if (rxbyte) {
        MAC_CHECK(dm9051_memory_peek(emac, (uint8_t *)&header, sizeof(header)) == ESP_OK,
                  "peek rx header failed", err, ESP_FAIL);
        rx_len = header.length_low + (header.length_high << 8);
        /* check if the buffer can hold all the incoming data */
        if (*length < rx_len - 4) {
            ESP_LOGE(TAG, "buffer size too small");
            /* tell upper layer the size we need */
            *length = rx_len - 4;
            ret = ESP_ERR_INVALID_SIZE;
            goto err;
        }
        MAC_CHECK(*length >= rx_len - 4, "buffer size too small", err, ESP_ERR_INVALID_SIZE);
        MAC_CHECK(dm9051_memory_read(emac, (uint8_t *)&header, sizeof(header)) == ESP_OK,
                  "read rx header failed", err, ESP_FAIL);
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
    gpio_pad_select_gpio(emac->int_gpio_num);
    gpio_set_direction(emac->int_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(emac->int_gpio_num, GPIO_PULLDOWN_ONLY);
    gpio_set_intr_type(emac->int_gpio_num, GPIO_INTR_POSEDGE);
    gpio_intr_enable(emac->int_gpio_num);
    gpio_isr_handler_add(emac->int_gpio_num, dm9051_isr_handler, emac);
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
    gpio_isr_handler_remove(emac->int_gpio_num);
    gpio_reset_pin(emac->int_gpio_num);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_dm9051_deinit(esp_eth_mac_t *mac)
{
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    dm9051_stop(emac);
    gpio_isr_handler_remove(emac->int_gpio_num);
    gpio_reset_pin(emac->int_gpio_num);
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
    emac_dm9051_t *emac = NULL;
    MAC_CHECK(dm9051_config, "can't set dm9051 specific config to null", err, NULL);
    MAC_CHECK(mac_config, "can't set mac config to null", err, NULL);
    emac = calloc(1, sizeof(emac_dm9051_t));
    MAC_CHECK(emac, "calloc emac failed", err, NULL);
    /* dm9051 receive is driven by interrupt only for now*/
    MAC_CHECK(dm9051_config->int_gpio_num >= 0, "error interrupt gpio number", err, NULL);
    /* bind methods and attributes */
    emac->sw_reset_timeout_ms = mac_config->sw_reset_timeout_ms;
    emac->int_gpio_num = dm9051_config->int_gpio_num;
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
    MAC_CHECK(emac->spi_lock, "create lock failed", err, NULL);
    /* create dm9051 task */
    BaseType_t xReturned = xTaskCreate(emac_dm9051_task, "dm9051_tsk", mac_config->rx_task_stack_size, emac,
                                       mac_config->rx_task_prio, &emac->rx_task_hdl);
    MAC_CHECK(xReturned == pdPASS, "create dm9051 task failed", err, NULL);
    return &(emac->parent);

err:
    if (emac) {
        if (emac->rx_task_hdl) {
            vTaskDelete(emac->rx_task_hdl);
        }
        if (emac->spi_lock) {
            vSemaphoreDelete(emac->spi_lock);
        }
        free(emac);
    }
    return ret;
}
