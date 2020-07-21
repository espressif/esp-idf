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
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hal/cpu_hal.h"
#include "enc28j60.h"
#include "sdkconfig.h"

static const char *TAG = "enc28j60";
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

#define ENC28J60_SPI_LOCK_TIMEOUT_MS (50)
#define ENC28J60_PHY_OPERATION_TIMEOUT_US (1000)
#define ENC28J60_SYSTEM_RESET_ADDITION_TIME_US (1000)

#define ENC28J60_BUFFER_SIZE (0x2000) // 8KB built-in buffer
/**
 *  ______
 * |__TX__| TX: 2 KB : [0x1800, 0x2000)
 * |      |
 * |  RX  | RX: 6 KB : [0x0000, 0x1800)
 * |______|
 *
 */
#define ENC28J60_BUF_RX_START (0)
#define ENC28J60_BUF_RX_END (ENC28J60_BUF_TX_START - 1)
#define ENC28J60_BUF_TX_START ((ENC28J60_BUFFER_SIZE / 4) * 3)
#define ENC28J60_BUF_TX_END (ENC28J60_BUFFER_SIZE - 1)

#define ENC28J60_RSV_SIZE (6) // Receive Status Vector Size

typedef struct {
    uint8_t next_packet_low;
    uint8_t next_packet_high;
    uint8_t length_low;
    uint8_t length_high;
    uint8_t status_low;
    uint8_t status_high;
} enc28j60_rx_header_t;

typedef struct {
    esp_eth_mac_t parent;
    esp_eth_mediator_t *eth;
    spi_device_handle_t spi_hdl;
    SemaphoreHandle_t spi_lock;
    TaskHandle_t rx_task_hdl;
    uint32_t sw_reset_timeout_ms;
    uint32_t next_packet_ptr;
    int int_gpio_num;
    uint8_t addr[6];
    uint8_t last_bank;
    bool packets_remain;
} emac_enc28j60_t;

static inline bool enc28j60_lock(emac_enc28j60_t *emac)
{
    return xSemaphoreTake(emac->spi_lock, pdMS_TO_TICKS(ENC28J60_SPI_LOCK_TIMEOUT_MS)) == pdTRUE;
}

static inline bool enc28j60_unlock(emac_enc28j60_t *emac)
{
    return xSemaphoreGive(emac->spi_lock) == pdTRUE;
}

/**
 * @brief ERXRDPT need to be set always at odd addresses
 */
static inline uint32_t enc28j60_next_ptr_align_odd(uint32_t next_packet_ptr, uint32_t start, uint32_t end)
{
    uint32_t erxrdpt;

    if ((next_packet_ptr - 1 < start) || (next_packet_ptr - 1 > end)) {
        erxrdpt = end;
    } else {
        erxrdpt = next_packet_ptr - 1;
    }

    return erxrdpt;
}

/**
 * @brief Calculate wrap around when reading beyond the end of the RX buffer
 */
static inline uint32_t enc28j60_rx_packet_start(uint32_t start_addr, uint32_t off)
{
    if (start_addr + off > ENC28J60_BUF_RX_END) {
        return (start_addr + off) - (ENC28J60_BUF_RX_END - ENC28J60_BUF_RX_START + 1);
    } else {
        return start_addr + off;
    }
}

/**
 * @brief SPI operation wrapper for writing ENC28J60 internal register
 */
static esp_err_t enc28j60_do_register_write(emac_enc28j60_t *emac, uint8_t reg_addr, uint8_t value)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = ENC28J60_SPI_CMD_WCR, // Write control register
        .addr = reg_addr,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = {
            [0] = value
        }
    };
    if (enc28j60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        enc28j60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief SPI operation wrapper for reading ENC28J60 internal register
 */
static esp_err_t enc28j60_do_register_read(emac_enc28j60_t *emac, bool is_eth_reg, uint8_t reg_addr, uint8_t *value)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = ENC28J60_SPI_CMD_RCR, // Read control register
        .addr = reg_addr,
        .length = is_eth_reg ? 8 : 16, // read operation is different for ETH register and non-ETH register
        .flags = SPI_TRANS_USE_RXDATA
    };
    if (enc28j60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        } else {
            *value = is_eth_reg ? trans.rx_data[0] : trans.rx_data[1];
        }
        enc28j60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief SPI operation wrapper for bitwise setting ENC28J60 internal register
 * @note can only be used for ETH registers
 */
static esp_err_t enc28j60_do_bitwise_set(emac_enc28j60_t *emac, uint8_t reg_addr, uint8_t mask)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = ENC28J60_SPI_CMD_BFS, // Bit field set
        .addr = reg_addr,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = {
            [0] = mask
        }
    };
    if (enc28j60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        enc28j60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief SPI operation wrapper for bitwise clearing ENC28J60 internal register
 * @note can only be used for ETH registers
 */
static esp_err_t enc28j60_do_bitwise_clr(emac_enc28j60_t *emac, uint8_t reg_addr, uint8_t mask)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = ENC28J60_SPI_CMD_BFC, // Bit field clear
        .addr = reg_addr,
        .length = 8,
        .flags = SPI_TRANS_USE_TXDATA,
        .tx_data = {
            [0] = mask
        }
    };
    if (enc28j60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        enc28j60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief SPI operation wrapper for writing ENC28J60 internal memory
 */
static esp_err_t enc28j60_do_memory_write(emac_enc28j60_t *emac, uint8_t *buffer, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = ENC28J60_SPI_CMD_WBM, // Write buffer memory
        .addr = 0x1A,
        .length = len * 8,
        .tx_buffer = buffer
    };
    if (enc28j60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        enc28j60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief SPI operation wrapper for reading ENC28J60 internal memory
 */
static esp_err_t enc28j60_do_memory_read(emac_enc28j60_t *emac, uint8_t *buffer, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = ENC28J60_SPI_CMD_RBM, // Read buffer memory
        .addr = 0x1A,
        .length = len * 8,
        .rx_buffer = buffer
    };

    if (enc28j60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        enc28j60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief SPI operation wrapper for resetting ENC28J60
 */
static esp_err_t enc28j60_do_reset(emac_enc28j60_t *emac)
{
    esp_err_t ret = ESP_OK;
    spi_transaction_t trans = {
        .cmd = ENC28J60_SPI_CMD_SRC, // Soft reset
        .addr = 0x1F,
    };
    if (enc28j60_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        enc28j60_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }

    // After reset, wait at least 1ms for the device to be ready
    esp_rom_delay_us(ENC28J60_SYSTEM_RESET_ADDITION_TIME_US);

    return ret;
}

/**
 * @brief Switch ENC28J60 register bank
 */
static esp_err_t enc28j60_switch_register_bank(emac_enc28j60_t *emac, uint8_t bank)
{
    esp_err_t ret = ESP_OK;
    if (bank != emac->last_bank) {
        MAC_CHECK(enc28j60_do_bitwise_clr(emac, ENC28J60_ECON1, 0x03) == ESP_OK,
                  "clear ECON1[1:0] failed", out, ESP_FAIL);
        MAC_CHECK(enc28j60_do_bitwise_set(emac, ENC28J60_ECON1, bank & 0x03) == ESP_OK,
                  "set ECON1[1:0] failed", out, ESP_FAIL);
        emac->last_bank = bank;
    }
out:
    return ret;
}

/**
 * @brief Write ENC28J60 register
 */
static esp_err_t enc28j60_register_write(emac_enc28j60_t *emac, uint16_t reg_addr, uint8_t value)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(enc28j60_switch_register_bank(emac, (reg_addr & 0xF00) >> 8) == ESP_OK,
              "switch bank failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_do_register_write(emac, reg_addr & 0xFF, value) == ESP_OK,
              "write register failed", out, ESP_FAIL);
out:
    return ret;
}

/**
 * @brief Read ENC28J60 register
 */
static esp_err_t enc28j60_register_read(emac_enc28j60_t *emac, uint16_t reg_addr, uint8_t *value)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(enc28j60_switch_register_bank(emac, (reg_addr & 0xF00) >> 8) == ESP_OK,
              "switch bank failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_do_register_read(emac, !(reg_addr & 0xF000), reg_addr & 0xFF, value) == ESP_OK,
              "read register failed", out, ESP_FAIL);
out:
    return ret;
}

/**
 * @brief Read ENC28J60 internal memroy
 */
static esp_err_t enc28j60_read_packet(emac_enc28j60_t *emac, uint32_t addr, uint8_t *packet, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERDPTL, addr & 0xFF) == ESP_OK,
              "write ERDPTL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERDPTH, (addr & 0xFF00) >> 8) == ESP_OK,
              "write ERDPTH failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_do_memory_read(emac, packet, len) == ESP_OK,
              "read memory failed", out, ESP_FAIL);
out:
    return ret;
}

/**
 * @brief Write ENC28J60 internal PHY register
 */
static esp_err_t emac_enc28j60_write_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr,
        uint32_t phy_reg, uint32_t reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    uint8_t mii_status;

    /* check if phy access is in progress */
    MAC_CHECK(enc28j60_register_read(emac, ENC28J60_MISTAT, &mii_status) == ESP_OK,
              "read MISTAT failed", out, ESP_FAIL);
    MAC_CHECK(!(mii_status & MISTAT_BUSY), "phy is busy", out, ESP_ERR_INVALID_STATE);

    /* tell the PHY address to write */
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MIREGADR, phy_reg & 0xFF) == ESP_OK,
              "write MIREGADR failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MIWRL, reg_value & 0xFF) == ESP_OK,
              "write MIWRL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MIWRH, (reg_value & 0xFF00) >> 8) == ESP_OK,
              "write MIWRH failed", out, ESP_FAIL);

    /* polling the busy flag */
    uint32_t to = 0;
    do {
        esp_rom_delay_us(100);
        MAC_CHECK(enc28j60_register_read(emac, ENC28J60_MISTAT, &mii_status) == ESP_OK,
                  "read MISTAT failed", out, ESP_FAIL);
        to += 100;
    } while ((mii_status & MISTAT_BUSY) && to < ENC28J60_PHY_OPERATION_TIMEOUT_US);
    MAC_CHECK(!(mii_status & MISTAT_BUSY), "phy is busy", out, ESP_ERR_TIMEOUT);
out:
    return ret;
}

/**
 * @brief Read ENC28J60 internal PHY register
 */
static esp_err_t emac_enc28j60_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr,
        uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(reg_value, "can't set reg_value to null", out, ESP_ERR_INVALID_ARG);
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    uint8_t mii_status;
    uint8_t mii_cmd;

    /* check if phy access is in progress */
    MAC_CHECK(enc28j60_register_read(emac, ENC28J60_MISTAT, &mii_status) == ESP_OK,
              "read MISTAT failed", out, ESP_FAIL);
    MAC_CHECK(!(mii_status & MISTAT_BUSY), "phy is busy", out, ESP_ERR_INVALID_STATE);

    /* tell the PHY address to read */
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MIREGADR, phy_reg & 0xFF) == ESP_OK,
              "write MIREGADR failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_read(emac, ENC28J60_MICMD, &mii_cmd) == ESP_OK,
              "read MICMD failed", out, ESP_FAIL);
    mii_cmd |= MICMD_MIIRD;
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MICMD, mii_cmd) == ESP_OK,
              "write MICMD failed", out, ESP_FAIL);

    /* polling the busy flag */
    uint32_t to = 0;
    do {
        esp_rom_delay_us(100);
        MAC_CHECK(enc28j60_register_read(emac, ENC28J60_MISTAT, &mii_status) == ESP_OK,
                  "read MISTAT failed", out, ESP_FAIL);
        to += 100;
    } while ((mii_status & MISTAT_BUSY) && to < ENC28J60_PHY_OPERATION_TIMEOUT_US);
    MAC_CHECK(!(mii_status & MISTAT_BUSY), "phy is busy", out, ESP_ERR_TIMEOUT);

    mii_cmd &= (~MICMD_MIIRD);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MICMD, mii_cmd) == ESP_OK,
              "write MICMD failed", out, ESP_FAIL);

    uint8_t value_l = 0;
    uint8_t value_h = 0;
    MAC_CHECK(enc28j60_register_read(emac, ENC28J60_MIRDL, &value_l) == ESP_OK,
              "read MIRDL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_read(emac, ENC28J60_MIRDH, &value_h) == ESP_OK,
              "read MIRDH failed", out, ESP_FAIL);
    *reg_value = (value_h << 8) | value_l;
out:
    return ret;
}

/**
 * @brief Set mediator for Ethernet MAC
 */
static esp_err_t emac_enc28j60_set_mediator(esp_eth_mac_t *mac, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(eth, "can't set mac's mediator to null", out, ESP_ERR_INVALID_ARG);
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    emac->eth = eth;
out:
    return ret;
}

/**
 * @brief Verify chip ID
 */
static esp_err_t enc28j60_verify_id(emac_enc28j60_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t id;
    MAC_CHECK(enc28j60_register_read(emac, ENC28J60_EREVID, &id) == ESP_OK,
              "read EREVID failed", out, ESP_FAIL);
    ESP_LOGI(TAG, "revision: %d", id);
    MAC_CHECK(id > 0 && id < 7, "wrong chip ID", out, ESP_ERR_INVALID_VERSION);
out:
    return ret;
}

/**
 * @brief Write mac address to internal registers
 */
static esp_err_t enc28j60_set_mac_addr(emac_enc28j60_t *emac)
{
    esp_err_t ret = ESP_OK;

    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MAADR6, emac->addr[5]) == ESP_OK,
              "write MAADR6 failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MAADR5, emac->addr[4]) == ESP_OK,
              "write MAADR5 failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MAADR4, emac->addr[3]) == ESP_OK,
              "write MAADR4 failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MAADR3, emac->addr[2]) == ESP_OK,
              "write MAADR3 failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MAADR2, emac->addr[1]) == ESP_OK,
              "write MAADR2 failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MAADR1, emac->addr[0]) == ESP_OK,
              "write MAADR1 failed", out, ESP_FAIL);
out:
    return ret;
}

/**
 * @brief   Clear multicast hash table
 */
static esp_err_t enc28j60_clear_multicast_table(emac_enc28j60_t *emac)
{
    esp_err_t ret = ESP_OK;

    for (int i = 0; i < 7; i++) {
        MAC_CHECK(enc28j60_register_write(emac, ENC28J60_EHT0 + i, 0x00) == ESP_OK,
                  "write ENC28J60_EHT%d failed", out, ESP_FAIL, i);
    }
out:
    return ret;
}

/**
 * @brief Default setup for ENC28J60 internal registers
 */
static esp_err_t enc28j60_setup_default(emac_enc28j60_t *emac)
{
    esp_err_t ret = ESP_OK;

    // set up receive buffer start + end
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXSTL, ENC28J60_BUF_RX_START & 0xFF) == ESP_OK,
              "write ERXSTL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXSTH, (ENC28J60_BUF_RX_START & 0xFF00) >> 8) == ESP_OK,
              "write ERXSTH failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXNDL, ENC28J60_BUF_RX_END & 0xFF) == ESP_OK,
              "write ERXNDL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXNDH, (ENC28J60_BUF_RX_END & 0xFF00) >> 8) == ESP_OK,
              "write ERXNDH failed", out, ESP_FAIL);
    uint32_t erxrdpt = enc28j60_next_ptr_align_odd(ENC28J60_BUF_RX_START, ENC28J60_BUF_RX_START, ENC28J60_BUF_RX_END);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXRDPTL, erxrdpt & 0xFF) == ESP_OK,
              "write ERXRDPTL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXRDPTH, (erxrdpt & 0xFF00) >> 8) == ESP_OK,
              "write ERXRDPTH failed", out, ESP_FAIL);

    // set up transmit buffer start + end
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ETXSTL, ENC28J60_BUF_TX_START & 0xFF) == ESP_OK,
              "write ETXSTL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ETXSTH, (ENC28J60_BUF_TX_START & 0xFF00) >> 8) == ESP_OK,
              "write ETXSTH failed", out, ESP_FAIL);

    // set up default filter mode: (unicast OR broadcast) AND crc valid
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN) == ESP_OK,
              "write ERXFCON failed", out, ESP_FAIL);

    // enable MAC receive, enable pause control frame on Tx and Rx path
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MACON1, MACON1_MARXEN | MACON1_RXPAUS | MACON1_TXPAUS) == ESP_OK,
              "write MACON1 failed", out, ESP_FAIL);
    // enable automatic padding, append CRC, check frame length, half duplex by default (can update at runtime)
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN) == ESP_OK, "write MACON3 failed", out, ESP_FAIL);
    // enable defer transmission (effective only in half duplex)
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MACON4, MACON4_DEFER) == ESP_OK,
              "write MACON4 failed", out, ESP_FAIL);
    // set inter-frame gap (back-to-back)
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MABBIPG, 0x12) == ESP_OK,
              "write MABBIPG failed", out, ESP_FAIL);
    // set inter-frame gap (non-back-to-back)
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MAIPGL, 0x12) == ESP_OK,
              "write MAIPGL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MAIPGH, 0x0C) == ESP_OK,
              "write MAIPGH failed", out, ESP_FAIL);

out:
    return ret;
}

/**
 * @brief Start enc28j60: enable interrupt and start receive
 */
static esp_err_t emac_enc28j60_start(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    /* enable interrupt */
    MAC_CHECK(enc28j60_do_bitwise_clr(emac, ENC28J60_EIR, 0xFF) == ESP_OK,
              "clear EIR failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_do_bitwise_set(emac, ENC28J60_EIE, EIE_PKTIE | EIE_INTIE) == ESP_OK,
              "set EIE.[PKTIE|INTIE] failed", out, ESP_FAIL);
    /* enable rx logic */
    MAC_CHECK(enc28j60_do_bitwise_set(emac, ENC28J60_ECON1, ECON1_RXEN) == ESP_OK,
              "set ECON1.RXEN failed", out, ESP_FAIL);

    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERDPTL, 0x00) == ESP_OK,
              "write ERDPTL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERDPTH, 0x00) == ESP_OK,
              "write ERDPTH failed", out, ESP_FAIL);
out:
    return ret;
}

/**
 * @brief   Stop enc28j60: disable interrupt and stop receiving packets
 */
static esp_err_t emac_enc28j60_stop(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    /* disable interrupt */
    MAC_CHECK(enc28j60_do_bitwise_clr(emac, ENC28J60_EIE, 0xFF) == ESP_OK,
              "clear EIE failed", out, ESP_FAIL);
    /* disable rx */
    MAC_CHECK(enc28j60_do_bitwise_clr(emac, ENC28J60_ECON1, ECON1_RXEN) == ESP_OK,
              "clear ECON1.RXEN failed", out, ESP_FAIL);
out:
    return ret;
}

static esp_err_t emac_enc28j60_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "can't set mac addr to null", out, ESP_ERR_INVALID_ARG);
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    memcpy(emac->addr, addr, 6);
    MAC_CHECK(enc28j60_set_mac_addr(emac) == ESP_OK, "set mac address failed", out, ESP_FAIL);
out:
    return ret;
}

static esp_err_t emac_enc28j60_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "can't set mac addr to null", out, ESP_ERR_INVALID_ARG);
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    memcpy(addr, emac->addr, 6);
out:
    return ret;
}

static void enc28j60_isr_handler(void *arg)
{
    emac_enc28j60_t *emac = (emac_enc28j60_t *)arg;
    BaseType_t high_task_wakeup = pdFALSE;
    /* notify enc28j60 task */
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup != pdFALSE) {
        portYIELD_FROM_ISR();
    }
}

static void emac_enc28j60_task(void *arg)
{
    emac_enc28j60_t *emac = (emac_enc28j60_t *)arg;
    uint8_t status = 0;
    uint8_t *buffer = NULL;
    uint32_t length = 0;

    while (1) {
        // block indefinitely until some task notifies me
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        /* clear interrupt status */
        enc28j60_do_register_read(emac, true, ENC28J60_EIR, &status);
        /* packet received */
        if (status & EIR_PKTIF) {
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

static esp_err_t emac_enc28j60_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    switch (link) {
    case ETH_LINK_UP:
        MAC_CHECK(mac->start(mac) == ESP_OK, "enc28j60 start failed", out, ESP_FAIL);
        break;
    case ETH_LINK_DOWN:
        MAC_CHECK(mac->stop(mac) == ESP_OK, "enc28j60 stop failed", out, ESP_FAIL);
        break;
    default:
        MAC_CHECK(false, "unknown link status", out, ESP_ERR_INVALID_ARG);
        break;
    }
out:
    return ret;
}

static esp_err_t emac_enc28j60_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    switch (speed) {
    case ETH_SPEED_10M:
        ESP_LOGI(TAG, "working in 10Mbps");
        break;
    default:
        MAC_CHECK(false, "100Mbps unsupported", out, ESP_ERR_NOT_SUPPORTED);
        break;
    }
out:
    return ret;
}

static esp_err_t emac_enc28j60_set_duplex(esp_eth_mac_t *mac, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    uint8_t mac3 = 0;
    MAC_CHECK(enc28j60_register_read(emac, ENC28J60_MACON3, &mac3) == ESP_OK,
              "read MACON3 failed", out, ESP_FAIL);
    switch (duplex) {
    case ETH_DUPLEX_HALF:
        mac3 &= ~MACON3_FULDPX;
        MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MABBIPG, 0x12) == ESP_OK,
                  "write MABBIPG failed", out, ESP_FAIL);
        ESP_LOGI(TAG, "working in half duplex");
        break;
    case ETH_DUPLEX_FULL:
        mac3 |= MACON3_FULDPX;
        MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MABBIPG, 0x15) == ESP_OK,
                  "write MABBIPG failed", out, ESP_FAIL);
        ESP_LOGI(TAG, "working in full duplex");
        break;
    default:
        MAC_CHECK(false, "unknown duplex", out, ESP_ERR_INVALID_ARG);
        break;
    }
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_MACON3, mac3) == ESP_OK,
              "write MACON3 failed", out, ESP_FAIL);
out:
    return ret;
}

static esp_err_t emac_enc28j60_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    esp_err_t ret = ESP_OK;
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    if (enable) {
        MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXFCON, 0x00) == ESP_OK,
                  "write ERXFCON failed", out, ESP_FAIL);
    }
out:
    return ret;
}

static esp_err_t emac_enc28j60_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    uint8_t econ1 = 0;

    /* Check if last transmit complete */
    MAC_CHECK(enc28j60_do_register_read(emac, true, ENC28J60_ECON1, &econ1) == ESP_OK,
              "read ECON1 failed", out, ESP_FAIL);
    MAC_CHECK(!(econ1 & ECON1_TXRTS), "last transmit still in progress", out, ESP_ERR_INVALID_STATE);

    /* Set the write pointer to start of transmit buffer area */
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_EWRPTL, ENC28J60_BUF_TX_START & 0xFF) == ESP_OK,
              "write EWRPTL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_EWRPTH, (ENC28J60_BUF_TX_START & 0xFF00) >> 8) == ESP_OK,
              "write EWRPTH failed", out, ESP_FAIL);

    /* Set the end pointer to correspond to the packet size given */
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ETXNDL, (ENC28J60_BUF_TX_START + length) & 0xFF) == ESP_OK,
              "write ETXNDL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ETXNDH, ((ENC28J60_BUF_TX_START + length) & 0xFF00) >> 8) == ESP_OK,
              "write ETXNDH failed", out, ESP_FAIL);

    /* copy data to tx memory */
    uint8_t per_pkt_control = 0; // MACON3 will be used to determine how the packet will be transmitted
    MAC_CHECK(enc28j60_do_memory_write(emac, &per_pkt_control, 1) == ESP_OK,
              "write packet control byte failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_do_memory_write(emac, buf, length) == ESP_OK,
              "buffer memory write failed", out, ESP_FAIL);
    /* issue tx polling command */
    MAC_CHECK(enc28j60_do_bitwise_set(emac, ENC28J60_ECON1, ECON1_TXRTS) == ESP_OK,
              "set ECON1.TXRTS failed", out, ESP_FAIL);
out:
    return ret;
}

static esp_err_t emac_enc28j60_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    uint8_t pk_counter = 0;
    uint16_t rx_len = 0;
    uint32_t next_packet_addr = 0;
    __attribute__((aligned(4))) enc28j60_rx_header_t header; // SPI driver needs the rx buffer 4 byte align

    // read packet header
    MAC_CHECK(enc28j60_read_packet(emac, emac->next_packet_ptr, (uint8_t *)&header, sizeof(header)) == ESP_OK,
              "read header failed", out, ESP_FAIL);

    // get packets' length, address
    rx_len = header.length_low + (header.length_high << 8);
    next_packet_addr = header.next_packet_low + (header.next_packet_high << 8);

    // read packet content
    MAC_CHECK(enc28j60_read_packet(emac, enc28j60_rx_packet_start(emac->next_packet_ptr, ENC28J60_RSV_SIZE), buf, rx_len) == ESP_OK,
              "read packet content failed", out, ESP_FAIL);

    // free receive buffer space
    uint32_t erxrdpt = enc28j60_next_ptr_align_odd(next_packet_addr, ENC28J60_BUF_RX_START, ENC28J60_BUF_RX_END);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXRDPTL, (erxrdpt & 0xFF)) == ESP_OK,
              "write ERXRDPTL failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_write(emac, ENC28J60_ERXRDPTH, (erxrdpt & 0xFF00) >> 8) == ESP_OK,
              "write ERXRDPTH failed", out, ESP_FAIL);
    emac->next_packet_ptr = next_packet_addr;

    MAC_CHECK(enc28j60_do_bitwise_set(emac, ENC28J60_ECON2, ECON2_PKTDEC) == ESP_OK,
              "set ECON2.PKTDEC failed", out, ESP_FAIL);
    MAC_CHECK(enc28j60_register_read(emac, ENC28J60_EPKTCNT, &pk_counter) == ESP_OK,
              "read EPKTCNT failed", out, ESP_FAIL);

    *length = rx_len - 4; // substract the CRC length
    emac->packets_remain = pk_counter > 0;
out:
    return ret;
}

static esp_err_t emac_enc28j60_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    esp_eth_mediator_t *eth = emac->eth;

    /* init gpio used for reporting enc28j60 interrupt */
    gpio_reset_pin(emac->int_gpio_num);
    gpio_set_direction(emac->int_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(emac->int_gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(emac->int_gpio_num, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(emac->int_gpio_num);
    gpio_isr_handler_add(emac->int_gpio_num, enc28j60_isr_handler, emac);
    MAC_CHECK(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL) == ESP_OK,
              "lowlevel init failed", out, ESP_FAIL);

    /* reset enc28j60 */
    MAC_CHECK(enc28j60_do_reset(emac) == ESP_OK, "reset enc28j60 failed", out, ESP_FAIL);
    /* verify chip id */
    MAC_CHECK(enc28j60_verify_id(emac) == ESP_OK, "vefiry chip ID failed", out, ESP_FAIL);
    /* default setup of internal registers */
    MAC_CHECK(enc28j60_setup_default(emac) == ESP_OK, "enc28j60 default setup failed", out, ESP_FAIL);
    /* clear multicast hash table */
    MAC_CHECK(enc28j60_clear_multicast_table(emac) == ESP_OK, "clear multicast table failed", out, ESP_FAIL);

    return ESP_OK;
out:
    gpio_isr_handler_remove(emac->int_gpio_num);
    gpio_reset_pin(emac->int_gpio_num);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_enc28j60_deinit(esp_eth_mac_t *mac)
{
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    mac->stop(mac);
    gpio_isr_handler_remove(emac->int_gpio_num);
    gpio_reset_pin(emac->int_gpio_num);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ESP_OK;
}

static esp_err_t emac_enc28j60_del(esp_eth_mac_t *mac)
{
    emac_enc28j60_t *emac = __containerof(mac, emac_enc28j60_t, parent);
    vTaskDelete(emac->rx_task_hdl);
    vSemaphoreDelete(emac->spi_lock);
    free(emac);
    return ESP_OK;
}

esp_eth_mac_t *esp_eth_mac_new_enc28j60(const eth_enc28j60_config_t *enc28j60_config, const eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *ret = NULL;
    emac_enc28j60_t *emac = NULL;
    MAC_CHECK(enc28j60_config, "can't set enc28j60 specific config to null", err, NULL);
    MAC_CHECK(mac_config, "can't set mac config to null", err, NULL);
    emac = calloc(1, sizeof(emac_enc28j60_t));
    MAC_CHECK(emac, "calloc emac failed", err, NULL);
    /* enc28j60 driver is interrupt driven */
    MAC_CHECK(enc28j60_config->int_gpio_num >= 0, "error interrupt gpio number", err, NULL);
    emac->last_bank = 0xFF;
    emac->next_packet_ptr = ENC28J60_BUF_RX_START;
    /* bind methods and attributes */
    emac->sw_reset_timeout_ms = mac_config->sw_reset_timeout_ms;
    emac->int_gpio_num = enc28j60_config->int_gpio_num;
    emac->spi_hdl = enc28j60_config->spi_hdl;
    emac->parent.set_mediator = emac_enc28j60_set_mediator;
    emac->parent.init = emac_enc28j60_init;
    emac->parent.deinit = emac_enc28j60_deinit;
    emac->parent.start = emac_enc28j60_start;
    emac->parent.stop = emac_enc28j60_stop;
    emac->parent.del = emac_enc28j60_del;
    emac->parent.write_phy_reg = emac_enc28j60_write_phy_reg;
    emac->parent.read_phy_reg = emac_enc28j60_read_phy_reg;
    emac->parent.set_addr = emac_enc28j60_set_addr;
    emac->parent.get_addr = emac_enc28j60_get_addr;
    emac->parent.set_speed = emac_enc28j60_set_speed;
    emac->parent.set_duplex = emac_enc28j60_set_duplex;
    emac->parent.set_link = emac_enc28j60_set_link;
    emac->parent.set_promiscuous = emac_enc28j60_set_promiscuous;
    emac->parent.transmit = emac_enc28j60_transmit;
    emac->parent.receive = emac_enc28j60_receive;
    /* create mutex */
    emac->spi_lock = xSemaphoreCreateMutex();
    MAC_CHECK(emac->spi_lock, "create lock failed", err, NULL);
    /* create enc28j60 task */
    BaseType_t core_num = tskNO_AFFINITY;
    if (mac_config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = cpu_hal_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(emac_enc28j60_task, "enc28j60_tsk", mac_config->rx_task_stack_size, emac,
                           mac_config->rx_task_prio, &emac->rx_task_hdl, core_num);
    MAC_CHECK(xReturned == pdPASS, "create enc28j60 task failed", err, NULL);

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
