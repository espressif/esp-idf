// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_rom_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hal/cpu_hal.h"
#include "w5500.h"
#include "sdkconfig.h"

static const char *TAG = "w5500-mac";
#define MAC_CHECK(a, str, goto_tag, ret_value, ...)                               \
    do {                                                                          \
        if (!(a)) {                                                               \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

#define W5500_SPI_LOCK_TIMEOUT_MS (50)
#define W5500_TX_MEM_SIZE (0x4000)
#define W5500_RX_MEM_SIZE (0x4000)

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
} emac_w5500_t;

static inline bool w5500_lock(emac_w5500_t *emac)
{
    return xSemaphoreTake(emac->spi_lock, pdMS_TO_TICKS(W5500_SPI_LOCK_TIMEOUT_MS)) == pdTRUE;
}

static inline bool w5500_unlock(emac_w5500_t *emac)
{
    return xSemaphoreGive(emac->spi_lock) == pdTRUE;
}

static esp_err_t w5500_write(emac_w5500_t *emac, uint32_t address, const void *value, uint32_t len)
{
    esp_err_t ret = ESP_OK;

    spi_transaction_t trans = {
        .cmd = (address >> W5500_ADDR_OFFSET),
        .addr = ((address & 0xFFFF) | (W5500_ACCESS_MODE_WRITE << W5500_RWB_OFFSET) | W5500_SPI_OP_MODE_VDM),
        .length = 8 * len,
        .tx_buffer = value
    };
    if (w5500_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        w5500_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

static esp_err_t w5500_read(emac_w5500_t *emac, uint32_t address, void *value, uint32_t len)
{
    esp_err_t ret = ESP_OK;

    spi_transaction_t trans = {
        .cmd = (address >> W5500_ADDR_OFFSET),
        .addr = ((address & 0xFFFF) | (W5500_ACCESS_MODE_READ << W5500_RWB_OFFSET) | W5500_SPI_OP_MODE_VDM),
        .length = 8 * len,
        .rx_buffer = value
    };
    if (w5500_lock(emac)) {
        if (spi_device_polling_transmit(emac->spi_hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        w5500_unlock(emac);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

static esp_err_t w5500_send_command(emac_w5500_t *emac, uint8_t command, uint32_t timeout_ms)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_CR(0), &command, sizeof(command)) == ESP_OK, "write SCR failed", err, ESP_FAIL);
    // after W5500 accepts the command, the command register will be cleared automatically
    uint32_t to = 0;
    for (to = 0; to < timeout_ms / 10; to++) {
        MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_CR(0), &command, sizeof(command)) == ESP_OK, "read SCR failed", err, ESP_FAIL);
        if (!command) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    MAC_CHECK(to < timeout_ms / 10, "send command timeout", err, ESP_ERR_TIMEOUT);

err:
    return ret;
}

static esp_err_t w5500_get_tx_free_size(emac_w5500_t *emac, uint16_t *size)
{
    esp_err_t ret = ESP_OK;
    uint16_t free0, free1 = 0;
    // read TX_FSR register more than once, until we get the same value
    // this is a trick because we might be interrupted between reading the high/low part of the TX_FSR register (16 bits in length)
    do {
        MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_TX_FSR(0), &free0, sizeof(free0)) == ESP_OK, "read TX FSR failed", err, ESP_FAIL);
        MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_TX_FSR(0), &free1, sizeof(free1)) == ESP_OK, "read TX FSR failed", err, ESP_FAIL);
    } while (free0 != free1);

    *size = __builtin_bswap16(free0);

err:
    return ret;
}

static esp_err_t w5500_get_rx_received_size(emac_w5500_t *emac, uint16_t *size)
{
    esp_err_t ret = ESP_OK;
    uint16_t received0, received1 = 0;
    do {
        MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_RX_RSR(0), &received0, sizeof(received0)) == ESP_OK, "read RX RSR failed", err, ESP_FAIL);
        MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_RX_RSR(0), &received1, sizeof(received1)) == ESP_OK, "read RX RSR failed", err, ESP_FAIL);
    } while (received0 != received1);
    *size = __builtin_bswap16(received0);

err:
    return ret;
}

static esp_err_t w5500_write_buffer(emac_w5500_t *emac, const void *buffer, uint32_t len, uint16_t offset)
{
    esp_err_t ret = ESP_OK;
    uint32_t remain = len;
    const uint8_t *buf = buffer;
    offset %= W5500_TX_MEM_SIZE;
    if (offset + len > W5500_TX_MEM_SIZE) {
        remain = (offset + len) % W5500_TX_MEM_SIZE;
        len = W5500_TX_MEM_SIZE - offset;
        MAC_CHECK(w5500_write(emac, W5500_MEM_SOCK_TX(0, offset), buf, len) == ESP_OK, "write TX buffer failed", err, ESP_FAIL);
        offset += len;
        buf += len;
    }
    MAC_CHECK(w5500_write(emac, W5500_MEM_SOCK_TX(0, offset), buf, remain) == ESP_OK, "write TX buffer failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t w5500_read_buffer(emac_w5500_t *emac, void *buffer, uint32_t len, uint16_t offset)
{
    esp_err_t ret = ESP_OK;
    uint32_t remain = len;
    uint8_t *buf = buffer;
    offset %= W5500_RX_MEM_SIZE;
    if (offset + len > W5500_RX_MEM_SIZE) {
        remain = (offset + len) % W5500_RX_MEM_SIZE;
        len = W5500_RX_MEM_SIZE - offset;
        MAC_CHECK(w5500_read(emac, W5500_MEM_SOCK_RX(0, offset), buf, len) == ESP_OK, "read RX buffer failed", err, ESP_FAIL);
        offset += len;
        buf += len;
    }
    MAC_CHECK(w5500_read(emac, W5500_MEM_SOCK_RX(0, offset), buf, remain) == ESP_OK, "read RX buffer failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t w5500_set_mac_addr(emac_w5500_t *emac)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(w5500_write(emac, W5500_REG_MAC, emac->addr, 6) == ESP_OK, "write MAC address register failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t w5500_reset(emac_w5500_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* software reset */
    uint8_t mr = W5500_MR_RST; // Set RST bit (auto clear)
    MAC_CHECK(w5500_write(emac, W5500_REG_MR, &mr, sizeof(mr)) == ESP_OK, "write MR failed", err, ESP_FAIL);
    uint32_t to = 0;
    for (to = 0; to < emac->sw_reset_timeout_ms / 10; to++) {
        MAC_CHECK(w5500_read(emac, W5500_REG_MR, &mr, sizeof(mr)) == ESP_OK, "read MR failed", err, ESP_FAIL);
        if (!(mr & W5500_MR_RST)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    MAC_CHECK(to < emac->sw_reset_timeout_ms / 10, "reset timeout", err, ESP_ERR_TIMEOUT);

err:
    return ret;
}

static esp_err_t w5500_verify_id(emac_w5500_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t version = 0;
    MAC_CHECK(w5500_read(emac, W5500_REG_VERSIONR, &version, sizeof(version)) == ESP_OK,
              "read VERSIONR failed", err, ESP_FAIL);
    // W5500 doesn't have chip ID, we just print the version number instead
    ESP_LOGI(TAG, "version=%x", version);

err:
    return ret;
}

static esp_err_t w5500_setup_default(emac_w5500_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t reg_value = 16;

    // Only SOCK0 can be used as MAC RAW mode, so we give the whole buffer (16KB TX and 16KB RX) to SOCK0
    MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_RXBUF_SIZE(0), &reg_value, sizeof(reg_value)) == ESP_OK, "set rx buffer size failed", err, ESP_FAIL);
    MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_TXBUF_SIZE(0), &reg_value, sizeof(reg_value)) == ESP_OK, "set tx buffer size failed", err, ESP_FAIL);
    reg_value = 0;
    for (int i = 1; i < 8; i++) {
        MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_RXBUF_SIZE(i), &reg_value, sizeof(reg_value)) == ESP_OK, "set rx buffer size failed", err, ESP_FAIL);
        MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_TXBUF_SIZE(i), &reg_value, sizeof(reg_value)) == ESP_OK, "set tx buffer size failed", err, ESP_FAIL);
    }

    /* Enable ping block, disable PPPoE, WOL */
    reg_value = W5500_MR_PB;
    MAC_CHECK(w5500_write(emac, W5500_REG_MR, &reg_value, sizeof(reg_value)) == ESP_OK, "write MR failed", err, ESP_FAIL);
    /* Disable interrupt for all sockets by default */
    reg_value = 0;
    MAC_CHECK(w5500_write(emac, W5500_REG_SIMR, &reg_value, sizeof(reg_value)) == ESP_OK, "write SIMR failed", err, ESP_FAIL);
    /* Enable MAC RAW mode for SOCK0, enable MAC filter, no blocking broadcast and multicast */
    reg_value = W5500_SMR_MAC_RAW | W5500_SMR_MAC_FILTER;
    MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_MR(0), &reg_value, sizeof(reg_value)) == ESP_OK, "write SMR failed", err, ESP_FAIL);
    /* Enable receive and send event for SOCK0 */
    reg_value = W5500_SIR_RECV | W5500_SIR_SEND;
    MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_IMR(0), &reg_value, sizeof(reg_value)) == ESP_OK, "write SOCK0 IMR failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t emac_w5500_start(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    uint8_t reg_value = 0;
    /* open SOCK0 */
    MAC_CHECK(w5500_send_command(emac, W5500_SCR_OPEN, 100) == ESP_OK, "issue OPEN command failed", err, ESP_FAIL);
    /* enable interrupt for SOCK0 */
    reg_value = W5500_SIMR_SOCK0;
    MAC_CHECK(w5500_write(emac, W5500_REG_SIMR, &reg_value, sizeof(reg_value)) == ESP_OK, "write SIMR failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t emac_w5500_stop(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    uint8_t reg_value = 0;
    /* disable interrupt */
    MAC_CHECK(w5500_write(emac, W5500_REG_SIMR, &reg_value, sizeof(reg_value)) == ESP_OK, "write SIMR failed", err, ESP_FAIL);
    /* close SOCK0 */
    MAC_CHECK(w5500_send_command(emac, W5500_SCR_CLOSE, 100) == ESP_OK, "issue CLOSE command failed", err, ESP_FAIL);

err:
    return ret;
}

IRAM_ATTR static void w5500_isr_handler(void *arg)
{
    emac_w5500_t *emac = (emac_w5500_t *)arg;
    BaseType_t high_task_wakeup = pdFALSE;
    /* notify w5500 task */
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup != pdFALSE) {
        portYIELD_FROM_ISR();
    }
}

static void emac_w5500_task(void *arg)
{
    emac_w5500_t *emac = (emac_w5500_t *)arg;
    uint8_t status = 0;
    uint8_t *buffer = NULL;
    uint32_t length = 0;
    while (1) {
        // block indefinitely until some task notifies me
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        /* read interrupt status */
        w5500_read(emac, W5500_REG_SOCK_IR(0), &status, sizeof(status));
        /* packet received */
        if (status & W5500_SIR_RECV) {
            status = W5500_SIR_RECV;
            // clear interrupt status
            w5500_write(emac, W5500_REG_SOCK_IR(0), &status, sizeof(status));
            do {
                length = ETH_MAX_PACKET_SIZE;
                buffer = heap_caps_malloc(length, MALLOC_CAP_DMA);
                if (!buffer) {
                    ESP_LOGE(TAG, "no mem for receive buffer");
                    break;
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

static esp_err_t emac_w5500_set_mediator(esp_eth_mac_t *mac, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(eth, "can't set mac's mediator to null", err, ESP_ERR_INVALID_ARG);
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    emac->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_w5500_write_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    // PHY register and MAC registers are mixed together in W5500
    // The only PHY register is PHYCFGR
    MAC_CHECK(phy_reg == W5500_REG_PHYCFGR, "wrong PHY register", err, ESP_FAIL);
    MAC_CHECK(w5500_write(emac, W5500_REG_PHYCFGR, &reg_value, sizeof(uint8_t)) == ESP_OK, "write PHY register failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t emac_w5500_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(reg_value, "can't set reg_value to null", err, ESP_ERR_INVALID_ARG);
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    // PHY register and MAC registers are mixed together in W5500
    // The only PHY register is PHYCFGR
    MAC_CHECK(phy_reg == W5500_REG_PHYCFGR, "wrong PHY register", err, ESP_FAIL);
    MAC_CHECK(w5500_read(emac, W5500_REG_PHYCFGR, reg_value, sizeof(uint8_t)) == ESP_OK, "read PHY register failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t emac_w5500_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "invalid argument", err, ESP_ERR_INVALID_ARG);
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    memcpy(emac->addr, addr, 6);
    MAC_CHECK(w5500_set_mac_addr(emac) == ESP_OK, "set mac address failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t emac_w5500_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "invalid argument", err, ESP_ERR_INVALID_ARG);
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    memcpy(addr, emac->addr, 6);

err:
    return ret;
}

static esp_err_t emac_w5500_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    switch (link) {
    case ETH_LINK_UP:
        ESP_LOGD(TAG, "link is up");
        MAC_CHECK(mac->start(mac) == ESP_OK, "w5500 start failed", err, ESP_FAIL);
        break;
    case ETH_LINK_DOWN:
        ESP_LOGD(TAG, "link is down");
        MAC_CHECK(mac->stop(mac) == ESP_OK, "w5500 stop failed", err, ESP_FAIL);
        break;
    default:
        MAC_CHECK(false, "unknown link status", err, ESP_ERR_INVALID_ARG);
        break;
    }

err:
    return ret;
}

static esp_err_t emac_w5500_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    switch (speed) {
    case ETH_SPEED_10M:
        ESP_LOGD(TAG, "working in 10Mbps");
        break;
    case ETH_SPEED_100M:
        ESP_LOGD(TAG, "working in 100Mbps");
        break;
    default:
        MAC_CHECK(false, "unknown speed", err, ESP_ERR_INVALID_ARG);
        break;
    }

err:
    return ret;
}

static esp_err_t emac_w5500_set_duplex(esp_eth_mac_t *mac, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    switch (duplex) {
    case ETH_DUPLEX_HALF:
        ESP_LOGD(TAG, "working in half duplex");
        break;
    case ETH_DUPLEX_FULL:
        ESP_LOGD(TAG, "working in full duplex");
        break;
    default:
        MAC_CHECK(false, "unknown duplex", err, ESP_ERR_INVALID_ARG);
        break;
    }

err:
    return ret;
}

static esp_err_t emac_w5500_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    uint8_t smr = 0;
    MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_MR(0), &smr, sizeof(smr)) == ESP_OK, "read SMR failed", err, ESP_FAIL);
    if (enable) {
        smr &= ~W5500_SMR_MAC_FILTER;
    } else {
        smr |= W5500_SMR_MAC_FILTER;
    }
    MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_MR(0), &smr, sizeof(smr)) == ESP_OK, "write SMR failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t emac_w5500_enable_flow_ctrl(esp_eth_mac_t *mac, bool enable)
{
    /* w5500 doesn't support flow control function, so accept any value */
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t emac_w5500_set_peer_pause_ability(esp_eth_mac_t *mac, uint32_t ability)
{
    /* w5500 doesn't suppport PAUSE function, so accept any value */
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t emac_w5500_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    uint16_t offset = 0;

    // check if there're free memory to store this packet
    uint16_t free_size = 0;
    MAC_CHECK(w5500_get_tx_free_size(emac, &free_size) == ESP_OK, "get free size failed", err, ESP_FAIL);
    MAC_CHECK(length <= free_size, "free size (%d) < send length (%d)", err, ESP_FAIL, free_size, length);
    // get current write pointer
    MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_TX_WR(0), &offset, sizeof(offset)) == ESP_OK, "read TX WR failed", err, ESP_FAIL);
    offset = __builtin_bswap16(offset);
    // copy data to tx memory
    MAC_CHECK(w5500_write_buffer(emac, buf, length, offset) == ESP_OK, "write frame failed", err, ESP_FAIL);
    // update write pointer
    offset += length;
    offset = __builtin_bswap16(offset);
    MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_TX_WR(0), &offset, sizeof(offset)) == ESP_OK, "write TX WR failed", err, ESP_FAIL);
    // issue SEND command
    MAC_CHECK(w5500_send_command(emac, W5500_SCR_SEND, 100) == ESP_OK, "issue SEND command failed", err, ESP_FAIL);

    // pooling the TX done event
    uint8_t status = 0;
    do {
        MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_IR(0), &status, sizeof(status)) == ESP_OK, "read SOCK0 IR failed", err, ESP_FAIL);
    } while (!(status & W5500_SIR_SEND));
    // clear the event bit
    status  = W5500_SIR_SEND;
    MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_IR(0), &status, sizeof(status)) == ESP_OK, "write SOCK0 IR failed", err, ESP_FAIL);

err:
    return ret;
}

static esp_err_t emac_w5500_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    uint16_t offset = 0;
    uint16_t rx_len = 0;
    uint16_t remain_bytes = 0;
    emac->packets_remain = false;

    w5500_get_rx_received_size(emac, &remain_bytes);
    if (remain_bytes) {
        // get current read pointer
        MAC_CHECK(w5500_read(emac, W5500_REG_SOCK_RX_RD(0), &offset, sizeof(offset)) == ESP_OK, "read RX RD failed", err, ESP_FAIL);
        offset = __builtin_bswap16(offset);
        // read head first
        MAC_CHECK(w5500_read_buffer(emac, &rx_len, sizeof(rx_len), offset) == ESP_OK, "read frame header failed", err, ESP_FAIL);
        rx_len = __builtin_bswap16(rx_len) - 2; // data size includes 2 bytes of header
        offset += 2;
        // read the payload
        MAC_CHECK(w5500_read_buffer(emac, buf, rx_len, offset) == ESP_OK, "read payload failed, len=%d, offset=%d", err, ESP_FAIL, rx_len, offset);
        offset += rx_len;
        // update read pointer
        offset = __builtin_bswap16(offset);
        MAC_CHECK(w5500_write(emac, W5500_REG_SOCK_RX_RD(0), &offset, sizeof(offset)) == ESP_OK, "write RX RD failed", err, ESP_FAIL);
        /* issue RECV command */
        MAC_CHECK(w5500_send_command(emac, W5500_SCR_RECV, 100) == ESP_OK, "issue RECV command failed", err, ESP_FAIL);
        // check if there're more data need to process
        remain_bytes -= rx_len + 2;
        emac->packets_remain = remain_bytes > 0;
    }

    *length = rx_len;
err:
    return ret;
}

static esp_err_t emac_w5500_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    esp_rom_gpio_pad_select_gpio(emac->int_gpio_num);
    gpio_set_direction(emac->int_gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(emac->int_gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(emac->int_gpio_num, GPIO_INTR_NEGEDGE); // active low
    gpio_intr_enable(emac->int_gpio_num);
    gpio_isr_handler_add(emac->int_gpio_num, w5500_isr_handler, emac);
    MAC_CHECK(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL) == ESP_OK, "lowlevel init failed", err, ESP_FAIL);
    /* reset w5500 */
    MAC_CHECK(w5500_reset(emac) == ESP_OK, "reset w5500 failed", err, ESP_FAIL);
    /* verify chip id */
    MAC_CHECK(w5500_verify_id(emac) == ESP_OK, "vefiry chip ID failed", err, ESP_FAIL);
    /* default setup of internal registers */
    MAC_CHECK(w5500_setup_default(emac) == ESP_OK, "w5500 default setup failed", err, ESP_FAIL);
    return ESP_OK;
err:
    gpio_isr_handler_remove(emac->int_gpio_num);
    gpio_reset_pin(emac->int_gpio_num);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_w5500_deinit(esp_eth_mac_t *mac)
{
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    mac->stop(mac);
    gpio_isr_handler_remove(emac->int_gpio_num);
    gpio_reset_pin(emac->int_gpio_num);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ESP_OK;
}

static esp_err_t emac_w5500_del(esp_eth_mac_t *mac)
{
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    vTaskDelete(emac->rx_task_hdl);
    vSemaphoreDelete(emac->spi_lock);
    free(emac);
    return ESP_OK;
}

esp_eth_mac_t *esp_eth_mac_new_w5500(const eth_w5500_config_t *w5500_config, const eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *ret = NULL;
    emac_w5500_t *emac = NULL;
    MAC_CHECK(w5500_config && mac_config, "invalid argument", err, NULL);
    emac = calloc(1, sizeof(emac_w5500_t));
    MAC_CHECK(emac, "no mem for MAC instance", err, NULL);
    /* w5500 driver is interrupt driven */
    MAC_CHECK(w5500_config->int_gpio_num >= 0, "invalid interrupt gpio number", err, NULL);
    /* bind methods and attributes */
    emac->sw_reset_timeout_ms = mac_config->sw_reset_timeout_ms;
    emac->int_gpio_num = w5500_config->int_gpio_num;
    emac->spi_hdl = w5500_config->spi_hdl;
    emac->parent.set_mediator = emac_w5500_set_mediator;
    emac->parent.init = emac_w5500_init;
    emac->parent.deinit = emac_w5500_deinit;
    emac->parent.start = emac_w5500_start;
    emac->parent.stop = emac_w5500_stop;
    emac->parent.del = emac_w5500_del;
    emac->parent.write_phy_reg = emac_w5500_write_phy_reg;
    emac->parent.read_phy_reg = emac_w5500_read_phy_reg;
    emac->parent.set_addr = emac_w5500_set_addr;
    emac->parent.get_addr = emac_w5500_get_addr;
    emac->parent.set_speed = emac_w5500_set_speed;
    emac->parent.set_duplex = emac_w5500_set_duplex;
    emac->parent.set_link = emac_w5500_set_link;
    emac->parent.set_promiscuous = emac_w5500_set_promiscuous;
    emac->parent.set_peer_pause_ability = emac_w5500_set_peer_pause_ability;
    emac->parent.enable_flow_ctrl = emac_w5500_enable_flow_ctrl;
    emac->parent.transmit = emac_w5500_transmit;
    emac->parent.receive = emac_w5500_receive;
    /* create mutex */
    emac->spi_lock = xSemaphoreCreateMutex();
    MAC_CHECK(emac->spi_lock, "create lock failed", err, NULL);
    /* create w5500 task */
    BaseType_t core_num = tskNO_AFFINITY;
    if (mac_config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = cpu_hal_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(emac_w5500_task, "w5500_tsk", mac_config->rx_task_stack_size, emac,
                           mac_config->rx_task_prio, &emac->rx_task_hdl, core_num);
    MAC_CHECK(xReturned == pdPASS, "create w5500 task failed", err, NULL);
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
