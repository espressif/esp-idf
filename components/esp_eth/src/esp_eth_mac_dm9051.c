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
#include "esp_check.h"
#include "esp_eth.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hal/cpu_hal.h"
#include "dm9051.h"
#include "sdkconfig.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"

static const char *TAG = "dm9051.mac";

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
    bool flow_ctrl_enabled;
} emac_dm9051_t;

static inline bool dm9051_lock(emac_dm9051_t *emac)
{
    return xSemaphoreTake(emac->spi_lock, pdMS_TO_TICKS(DM9051_SPI_LOCK_TIMEOUT_MS)) == pdTRUE;
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
        ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_PAR + i, &emac->addr[i]), err, TAG, "read PAR failed");
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
        ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_PAR + i, emac->addr[i]), err, TAG, "write PAR failed");
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
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_BCASTCR, 0x00), err, TAG, "write BCASTCR failed");
    for (int i = 0; i < 7; i++) {
        ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_MAR + i, 0x00), err, TAG, "write MAR failed");
    }
    /* enable receive broadcast paclets */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_MAR + 7, 0x80), err, TAG, "write MAR failed");
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
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_GPR, 0x00), err, TAG, "write GPR failed");
    /* mac and phy register won't be accesable within at least 1ms */
    vTaskDelay(pdMS_TO_TICKS(10));
    /* software reset */
    uint8_t ncr = NCR_RST;
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_NCR, ncr), err, TAG, "write NCR failed");
    uint32_t to = 0;
    for (to = 0; to < emac->sw_reset_timeout_ms / 10; to++) {
        ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_NCR, &ncr), err, TAG, "read NCR failed");
        if (!(ncr & NCR_RST)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_GOTO_ON_FALSE(to < emac->sw_reset_timeout_ms / 10, ESP_ERR_TIMEOUT, err, TAG, "reset timeout");
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
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_VIDL, &id[0]), err, TAG, "read VIDL failed");
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_VIDH, &id[1]), err, TAG, "read VIDH failed");
    ESP_GOTO_ON_FALSE(0x0A == id[1] && 0x46 == id[0], ESP_ERR_INVALID_VERSION, err, TAG, "wrong Vendor ID");
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_PIDL, &id[0]), err, TAG, "read PIDL failed");
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_PIDH, &id[1]), err, TAG, "read PIDH failed");
    ESP_GOTO_ON_FALSE(0x90 == id[1] && 0x51 == id[0], ESP_ERR_INVALID_VERSION, err, TAG, "wrong Product ID");
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
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_NCR, 0x00), err, TAG, "write NCR failed");
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_WCR, 0x00), err, TAG, "write WCR failed");
    /* stop transmitting, enable appending pad, crc for packets */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_TCR, 0x00), err, TAG, "write TCR failed");
    /* stop receiving, no promiscuous mode, no runt packet(size < 64bytes), not all multicast packets*/
    /* discard long packet(size > 1522bytes) and crc error packet, enable watchdog */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_RCR, RCR_DIS_LONG | RCR_DIS_CRC), err, TAG, "write RCR failed");
    /* retry late collision packet, at most two transmit command can be issued before transmit complete */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_TCR2, TCR2_RLCP), err, TAG, "write TCR2 failed");
    /* enable auto transmit */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_ATCR, ATCR_AUTO_TX), err, TAG, "write ATCR failed");
    /* generate checksum for UDP, TCP and IPv4 packets */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_TCSCR, TCSCR_IPCSE | TCSCR_TCPCSE | TCSCR_UDPCSE), err, TAG, "write TCSCR failed");
    /* disable check sum for receive packets */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_RCSCSR, 0x00), err, TAG, "write RCSCSR failed");
    /* interrupt pin config: push-pull output, active high */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_INTCR, 0x00), err, TAG, "write INTCR failed");
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_INTCKCR, 0x00), err, TAG, "write INTCKCR failed");
    /* no length limitation for rx packets */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_RLENCR, 0x00), err, TAG, "write RLENCR failed");
    /* 3K-byte for TX and 13K-byte for RX */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_MEMSCR, 0x00), err, TAG, "write MEMSCR failed");
    /* reset tx and rx memory pointer */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_MPTRCR, MPTRCR_RST_RX | MPTRCR_RST_TX), err, TAG, "write MPTRCR failed");
    /* clear network status: wakeup event, tx complete */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END), err, TAG, "write NSR failed");
    /* clear interrupt status */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_ISR, ISR_CLR_STATUS), err, TAG, "write ISR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t dm9051_enable_flow_ctrl(emac_dm9051_t *emac, bool enable)
{
    esp_err_t ret = ESP_OK;
    if (enable) {
        /* send jam pattern (duration time = 1.15ms) when rx free space < 3k bytes */
        ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_BPTR, 0x3F), err, TAG, "write BPTR failed");
        /* flow control: high water threshold = 3k bytes, low water threshold = 8k bytes */
        ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_FCTR, 0x38), err, TAG, "write FCTR failed");
        /* enable flow control */
        ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_FCR, FCR_FLOW_ENABLE), err, TAG, "write FCR failed");
    } else {
        /* disable flow control */
        ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_FCR, 0), err, TAG, "write FCR failed");
    }
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief start dm9051: enable interrupt and start receive
 */
static esp_err_t emac_dm9051_start(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    /* enable only Rx related interrupts as others are processed synchronously */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_IMR, IMR_PAR | IMR_PRI), err, TAG, "write IMR failed");
    /* enable rx */
    uint8_t rcr = 0;
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_RCR, &rcr), err, TAG, "read RCR failed");
    rcr |= RCR_RXEN;
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_RCR, rcr), err, TAG, "write RCR failed");
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief stop dm9051: disable interrupt and stop receive
 */
static esp_err_t emac_dm9051_stop(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    /* disable interrupt */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_IMR, 0x00), err, TAG, "write IMR failed");
    /* disable rx */
    uint8_t rcr = 0;
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_RCR, &rcr), err, TAG, "read RCR failed");
    rcr &= ~RCR_RXEN;
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_RCR, rcr), err, TAG, "write RCR failed");
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
        // check if the task receives any notification
        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) == 0 &&    // if no notification ...
            gpio_get_level(emac->int_gpio_num) == 0) {               // ...and no interrupt asserted
            continue;                                                // -> just continue to check again
        }
        /* clear interrupt status */
        dm9051_register_read(emac, DM9051_ISR, &status);
        dm9051_register_write(emac, DM9051_ISR, status);
        /* packet received */
        if (status & ISR_PR) {
            do {
                length = ETH_MAX_PACKET_SIZE;
                buffer = heap_caps_malloc(length, MALLOC_CAP_DMA);
                if (!buffer) {
                    ESP_LOGE(TAG, "no mem for receive buffer");
                } else if (emac->parent.receive(&emac->parent, buffer, &length) == ESP_OK) {
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
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac's mediator to null");
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
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_EPCR, &epcr), err, TAG, "read EPCR failed");
    ESP_GOTO_ON_FALSE(!(epcr & EPCR_ERRE), ESP_ERR_INVALID_STATE, err, TAG, "phy is busy");
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_EPAR, (uint8_t)(((phy_addr << 6) & 0xFF) | phy_reg)), err, TAG, "write EPAR failed");
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_EPDRL, (uint8_t)(reg_value & 0xFF)), err, TAG, "write EPDRL failed");
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_EPDRH, (uint8_t)((reg_value >> 8) & 0xFF)), err, TAG, "write EPDRH failed");
    /* select PHY and select write operation */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_EPCR, EPCR_EPOS | EPCR_ERPRW), err, TAG, "write EPCR failed");
    /* polling the busy flag */
    uint32_t to = 0;
    do {
        esp_rom_delay_us(100);
        ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_EPCR, &epcr), err, TAG, "read EPCR failed");
        to += 100;
    } while ((epcr & EPCR_ERRE) && to < DM9051_PHY_OPERATION_TIMEOUT_US);
    ESP_GOTO_ON_FALSE(!(epcr & EPCR_ERRE), ESP_ERR_TIMEOUT, err, TAG, "phy is busy");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(reg_value, ESP_ERR_INVALID_ARG, err, TAG, "can't set reg_value to null");
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    /* check if phy access is in progress */
    uint8_t epcr = 0;
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_EPCR, &epcr), err, TAG, "read EPCR failed");
    ESP_GOTO_ON_FALSE(!(epcr & 0x01), ESP_ERR_INVALID_STATE, err, TAG, "phy is busy");
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_EPAR, (uint8_t)(((phy_addr << 6) & 0xFF) | phy_reg)), err, TAG, "write EPAR failed");
    /* Select PHY and select read operation */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_EPCR, 0x0C), err, TAG, "write EPCR failed");
    /* polling the busy flag */
    uint32_t to = 0;
    do {
        esp_rom_delay_us(100);
        ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_EPCR, &epcr), err, TAG, "read EPCR failed");
        to += 100;
    } while ((epcr & EPCR_ERRE) && to < DM9051_PHY_OPERATION_TIMEOUT_US);
    ESP_GOTO_ON_FALSE(!(epcr & EPCR_ERRE), ESP_ERR_TIMEOUT, err, TAG, "phy is busy");
    uint8_t value_h = 0;
    uint8_t value_l = 0;
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_EPDRH, &value_h), err, TAG, "read EPDRH failed");
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_EPDRL, &value_l), err, TAG, "read EPDRL failed");
    *reg_value = (value_h << 8) | value_l;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac addr to null");
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    memcpy(emac->addr, addr, 6);
    ESP_GOTO_ON_ERROR(dm9051_set_mac_addr(emac), err, TAG, "set mac address failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac addr to null");
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
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_NSR, &nsr), err, TAG, "read NSR failed");
    switch (link) {
    case ETH_LINK_UP:
        ESP_GOTO_ON_FALSE(nsr & NSR_LINKST, ESP_ERR_INVALID_STATE, err, TAG, "phy is not link up");
        ESP_GOTO_ON_ERROR(mac->start(mac), err, TAG, "dm9051 start failed");
        break;
    case ETH_LINK_DOWN:
        ESP_GOTO_ON_FALSE(!(nsr & NSR_LINKST), ESP_ERR_INVALID_STATE, err, TAG, "phy is not link down");
        ESP_GOTO_ON_ERROR(mac->stop(mac), err, TAG, "dm9051 stop failed");
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown link status");
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
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_NSR, &nsr), err, TAG, "read NSR failed");
    switch (speed) {
    case ETH_SPEED_10M:
        ESP_GOTO_ON_FALSE(nsr & NSR_SPEED, ESP_ERR_INVALID_STATE, err, TAG, "phy speed is not at 10Mbps");
        ESP_LOGD(TAG, "working in 10Mbps");
        break;
    case ETH_SPEED_100M:
        ESP_GOTO_ON_FALSE(!(nsr & NSR_SPEED), ESP_ERR_INVALID_STATE, err, TAG, "phy speed is not at 100Mbps");
        ESP_LOGD(TAG, "working in 100Mbps");
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown speed");
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
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_NCR, &ncr), err, TAG, "read NCR failed");
    switch (duplex) {
    case ETH_DUPLEX_HALF:
        ESP_LOGD(TAG, "working in half duplex");
        ESP_GOTO_ON_FALSE(!(ncr & NCR_FDX), ESP_ERR_INVALID_STATE, err, TAG, "phy is not at half duplex");
        break;
    case ETH_DUPLEX_FULL:
        ESP_LOGD(TAG, "working in full duplex");
        ESP_GOTO_ON_FALSE(ncr & NCR_FDX, ESP_ERR_INVALID_STATE, err, TAG, "phy is not at full duplex");
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown duplex");
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
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_EPDRL, &rcr), err, TAG, "read RCR failed");
    if (enable) {
        rcr |= RCR_PRMSC;
    } else {
        rcr &= ~RCR_PRMSC;
    }
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_RCR, rcr), err, TAG, "write RCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_enable_flow_ctrl(esp_eth_mac_t *mac, bool enable)
{
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    emac->flow_ctrl_enabled = enable;
    return ESP_OK;
}

static esp_err_t emac_dm9051_set_peer_pause_ability(esp_eth_mac_t *mac, uint32_t ability)
{
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    // we want to enable flow control, and peer does support pause function
    // then configure the MAC layer to enable flow control feature
    if (emac->flow_ctrl_enabled && ability) {
        dm9051_enable_flow_ctrl(emac, true);
    } else {
        dm9051_enable_flow_ctrl(emac, false);
        ESP_LOGD(TAG, "Flow control not enabled for the link");
    }
    return ESP_OK;
}

static esp_err_t emac_dm9051_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    /* Check if last transmit complete */
    uint8_t tcr = 0;
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_TCR, &tcr), err, TAG, "read TCR failed");
    ESP_GOTO_ON_FALSE(!(tcr & TCR_TXREQ), ESP_ERR_INVALID_STATE, err, TAG, "last transmit still in progress");
    /* set tx length */
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_TXPLL, length & 0xFF), err, TAG, "write TXPLL failed");
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_TXPLH, (length >> 8) & 0xFF), err, TAG, "write TXPLH failed");
    /* copy data to tx memory */
    ESP_GOTO_ON_ERROR(dm9051_memory_write(emac, buf, length), err, TAG, "write memory failed");
    /* issue tx polling command */
    tcr |= TCR_TXREQ;
    ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_TCR, tcr), err, TAG, "write TCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_dm9051_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    emac_dm9051_t *emac = __containerof(mac, emac_dm9051_t, parent);
    uint8_t rxbyte = 0;
    uint16_t rx_len = 0;
    __attribute__((aligned(4))) dm9051_rx_header_t header; // SPI driver needs the rx buffer 4 byte align
    emac->packets_remain = false;
    /* dummy read, get the most updated data */
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_MRCMDX, &rxbyte), err, TAG, "read MRCMDX failed");
    ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_MRCMDX, &rxbyte), err, TAG, "read MRCMDX failed");
    /* rxbyte must be 0xFF, 0 or 1 */
    if (rxbyte > 1) {
        ESP_GOTO_ON_ERROR(mac->stop(mac), err, TAG, "stop dm9051 failed");
        /* reset rx fifo pointer */
        ESP_GOTO_ON_ERROR(dm9051_register_write(emac, DM9051_MPTRCR, MPTRCR_RST_RX), err, TAG, "write MPTRCR failed");
        esp_rom_delay_us(10);
        ESP_GOTO_ON_ERROR(mac->start(mac), err, TAG, "start dm9051 failed");
        ESP_GOTO_ON_FALSE(false, ESP_FAIL, err, TAG, "reset rx fifo pointer");
    } else if (rxbyte) {
        ESP_GOTO_ON_ERROR(dm9051_memory_peek(emac, (uint8_t *)&header, sizeof(header)), err, TAG, "peek rx header failed");
        rx_len = header.length_low + (header.length_high << 8);
        /* check if the buffer can hold all the incoming data */
        if (*length < rx_len - 4) {
            ESP_LOGE(TAG, "buffer size too small, needs %d", rx_len - 4);
            /* tell upper layer the size we need */
            *length = rx_len - 4;
            ret = ESP_ERR_INVALID_SIZE;
            goto err;
        }
        ESP_GOTO_ON_ERROR(dm9051_memory_read(emac, (uint8_t *)&header, sizeof(header)), err, TAG, "read rx header failed");
        ESP_GOTO_ON_ERROR(dm9051_memory_read(emac, buf, rx_len), err, TAG, "read rx data failed");
        ESP_GOTO_ON_FALSE(!(header.status & 0xBF), ESP_FAIL, err, TAG, "receive status error: %xH", header.status);
        *length = rx_len - 4; // substract the CRC length (4Bytes)
        /* dummy read, get the most updated data */
        ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_MRCMDX, &rxbyte), err, TAG, "read MRCMDX failed");
        ESP_GOTO_ON_ERROR(dm9051_register_read(emac, DM9051_MRCMDX, &rxbyte), err, TAG, "read MRCMDX failed");
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
    esp_rom_gpio_pad_select_gpio(emac->int_gpio_num);
    gpio_set_direction(emac->int_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(emac->int_gpio_num, GPIO_PULLDOWN_ONLY);
    gpio_set_intr_type(emac->int_gpio_num, GPIO_INTR_POSEDGE);
    gpio_intr_enable(emac->int_gpio_num);
    gpio_isr_handler_add(emac->int_gpio_num, dm9051_isr_handler, emac);
    ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL), err, TAG, "lowlevel init failed");
    /* reset dm9051 */
    ESP_GOTO_ON_ERROR(dm9051_reset(emac), err, TAG, "reset dm9051 failed");
    /* verify chip id */
    ESP_GOTO_ON_ERROR(dm9051_verify_id(emac), err, TAG, "vefiry chip ID failed");
    /* default setup of internal registers */
    ESP_GOTO_ON_ERROR(dm9051_setup_default(emac), err, TAG, "dm9051 default setup failed");
    /* clear multicast hash table */
    ESP_GOTO_ON_ERROR(dm9051_clear_multicast_table(emac), err, TAG, "clear multicast table failed");
    /* get emac address from eeprom */
    ESP_GOTO_ON_ERROR(dm9051_get_mac_addr(emac), err, TAG, "fetch ethernet mac address failed");
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
    mac->stop(mac);
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
    ESP_GOTO_ON_FALSE(dm9051_config, NULL, err, TAG, "can't set dm9051 specific config to null");
    ESP_GOTO_ON_FALSE(mac_config, NULL, err, TAG, "can't set mac config to null");
    emac = calloc(1, sizeof(emac_dm9051_t));
    ESP_GOTO_ON_FALSE(emac, NULL, err, TAG, "calloc emac failed");
    /* dm9051 receive is driven by interrupt only for now*/
    ESP_GOTO_ON_FALSE(dm9051_config->int_gpio_num >= 0, NULL, err, TAG, "error interrupt gpio number");
    /* bind methods and attributes */
    emac->sw_reset_timeout_ms = mac_config->sw_reset_timeout_ms;
    emac->int_gpio_num = dm9051_config->int_gpio_num;
    emac->spi_hdl = dm9051_config->spi_hdl;
    emac->parent.set_mediator = emac_dm9051_set_mediator;
    emac->parent.init = emac_dm9051_init;
    emac->parent.deinit = emac_dm9051_deinit;
    emac->parent.start = emac_dm9051_start;
    emac->parent.stop = emac_dm9051_stop;
    emac->parent.del = emac_dm9051_del;
    emac->parent.write_phy_reg = emac_dm9051_write_phy_reg;
    emac->parent.read_phy_reg = emac_dm9051_read_phy_reg;
    emac->parent.set_addr = emac_dm9051_set_addr;
    emac->parent.get_addr = emac_dm9051_get_addr;
    emac->parent.set_speed = emac_dm9051_set_speed;
    emac->parent.set_duplex = emac_dm9051_set_duplex;
    emac->parent.set_link = emac_dm9051_set_link;
    emac->parent.set_promiscuous = emac_dm9051_set_promiscuous;
    emac->parent.set_peer_pause_ability = emac_dm9051_set_peer_pause_ability;
    emac->parent.enable_flow_ctrl = emac_dm9051_enable_flow_ctrl;
    emac->parent.transmit = emac_dm9051_transmit;
    emac->parent.receive = emac_dm9051_receive;
    /* create mutex */
    emac->spi_lock = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(emac->spi_lock, NULL, err, TAG, "create lock failed");
    /* create dm9051 task */
    BaseType_t core_num = tskNO_AFFINITY;
    if (mac_config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = cpu_hal_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(emac_dm9051_task, "dm9051_tsk", mac_config->rx_task_stack_size, emac,
                           mac_config->rx_task_prio, &emac->rx_task_hdl, core_num);
    ESP_GOTO_ON_FALSE(xReturned == pdPASS, NULL, err, TAG, "create dm9051 task failed");
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
