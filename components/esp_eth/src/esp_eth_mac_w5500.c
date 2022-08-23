/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_eth_driver.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "esp_rom_gpio.h"
#include "esp_cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "w5500.h"
#include "sdkconfig.h"

static const char *TAG = "w5500.mac";

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
        .flags = len <= 4 ? SPI_TRANS_USE_RXDATA : 0, // use direct reads for registers to prevent overwrites by 4-byte boundary writes
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
    if ((trans.flags&SPI_TRANS_USE_RXDATA) && len <= 4) {
        memcpy(value, trans.rx_data, len);  // copy register values to output
    }
    return ret;
}

static esp_err_t w5500_send_command(emac_w5500_t *emac, uint8_t command, uint32_t timeout_ms)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_CR(0), &command, sizeof(command)), err, TAG, "write SCR failed");
    // after W5500 accepts the command, the command register will be cleared automatically
    uint32_t to = 0;
    for (to = 0; to < timeout_ms / 10; to++) {
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_CR(0), &command, sizeof(command)), err, TAG, "read SCR failed");
        if (!command) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_GOTO_ON_FALSE(to < timeout_ms / 10, ESP_ERR_TIMEOUT, err, TAG, "send command timeout");

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
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_TX_FSR(0), &free0, sizeof(free0)), err, TAG, "read TX FSR failed");
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_TX_FSR(0), &free1, sizeof(free1)), err, TAG, "read TX FSR failed");
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
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_RX_RSR(0), &received0, sizeof(received0)), err, TAG, "read RX RSR failed");
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_RX_RSR(0), &received1, sizeof(received1)), err, TAG, "read RX RSR failed");
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
        ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_MEM_SOCK_TX(0, offset), buf, len), err, TAG, "write TX buffer failed");
        offset += len;
        buf += len;
    }
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_MEM_SOCK_TX(0, offset), buf, remain), err, TAG, "write TX buffer failed");

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
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_MEM_SOCK_RX(0, offset), buf, len), err, TAG, "read RX buffer failed");
        offset += len;
        buf += len;
    }
    ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_MEM_SOCK_RX(0, offset), buf, remain), err, TAG, "read RX buffer failed");

err:
    return ret;
}

static esp_err_t w5500_set_mac_addr(emac_w5500_t *emac)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_MAC, emac->addr, 6), err, TAG, "write MAC address register failed");

err:
    return ret;
}

static esp_err_t w5500_reset(emac_w5500_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* software reset */
    uint8_t mr = W5500_MR_RST; // Set RST bit (auto clear)
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_MR, &mr, sizeof(mr)), err, TAG, "write MR failed");
    uint32_t to = 0;
    for (to = 0; to < emac->sw_reset_timeout_ms / 10; to++) {
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_MR, &mr, sizeof(mr)), err, TAG, "read MR failed");
        if (!(mr & W5500_MR_RST)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_GOTO_ON_FALSE(to < emac->sw_reset_timeout_ms / 10, ESP_ERR_TIMEOUT, err, TAG, "reset timeout");

err:
    return ret;
}

static esp_err_t w5500_verify_id(emac_w5500_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t version = 0;
    ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_VERSIONR, &version, sizeof(version)), err, TAG, "read VERSIONR failed");
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
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_RXBUF_SIZE(0), &reg_value, sizeof(reg_value)), err, TAG, "set rx buffer size failed");
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_TXBUF_SIZE(0), &reg_value, sizeof(reg_value)), err, TAG, "set tx buffer size failed");
    reg_value = 0;
    for (int i = 1; i < 8; i++) {
        ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_RXBUF_SIZE(i), &reg_value, sizeof(reg_value)), err, TAG, "set rx buffer size failed");
        ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_TXBUF_SIZE(i), &reg_value, sizeof(reg_value)), err, TAG, "set tx buffer size failed");
    }

    /* Enable ping block, disable PPPoE, WOL */
    reg_value = W5500_MR_PB;
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_MR, &reg_value, sizeof(reg_value)), err, TAG, "write MR failed");
    /* Disable interrupt for all sockets by default */
    reg_value = 0;
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SIMR, &reg_value, sizeof(reg_value)), err, TAG, "write SIMR failed");
    /* Enable MAC RAW mode for SOCK0, enable MAC filter, no blocking broadcast and multicast */
    reg_value = W5500_SMR_MAC_RAW | W5500_SMR_MAC_FILTER;
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_MR(0), &reg_value, sizeof(reg_value)), err, TAG, "write SMR failed");
    /* Enable receive event for SOCK0 */
    reg_value = W5500_SIR_RECV;
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_IMR(0), &reg_value, sizeof(reg_value)), err, TAG, "write SOCK0 IMR failed");
    /* Set the interrupt re-assert level to maximum (~1.5ms) to lower the chances of missing it */
    uint16_t int_level = __builtin_bswap16(0xFFFF);
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_INTLEVEL, &int_level, sizeof(int_level)), err, TAG, "write INTLEVEL failed");

err:
    return ret;
}

static esp_err_t emac_w5500_start(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    uint8_t reg_value = 0;
    /* open SOCK0 */
    ESP_GOTO_ON_ERROR(w5500_send_command(emac, W5500_SCR_OPEN, 100), err, TAG, "issue OPEN command failed");
    /* enable interrupt for SOCK0 */
    reg_value = W5500_SIMR_SOCK0;
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SIMR, &reg_value, sizeof(reg_value)), err, TAG, "write SIMR failed");

err:
    return ret;
}

static esp_err_t emac_w5500_stop(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    uint8_t reg_value = 0;
    /* disable interrupt */
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SIMR, &reg_value, sizeof(reg_value)), err, TAG, "write SIMR failed");
    /* close SOCK0 */
    ESP_GOTO_ON_ERROR(w5500_send_command(emac, W5500_SCR_CLOSE, 100), err, TAG, "issue CLOSE command failed");

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
        // check if the task receives any notification
        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) == 0 &&    // if no notification ...
            gpio_get_level(emac->int_gpio_num) != 0) {               // ...and no interrupt asserted
            continue;                                                // -> just continue to check again
        }

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
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac's mediator to null");
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
    ESP_GOTO_ON_FALSE(phy_reg == W5500_REG_PHYCFGR, ESP_FAIL, err, TAG, "wrong PHY register");
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_PHYCFGR, &reg_value, sizeof(uint8_t)), err, TAG, "write PHY register failed");

err:
    return ret;
}

static esp_err_t emac_w5500_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(reg_value, ESP_ERR_INVALID_ARG, err, TAG, "can't set reg_value to null");
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    // PHY register and MAC registers are mixed together in W5500
    // The only PHY register is PHYCFGR
    ESP_GOTO_ON_FALSE(phy_reg == W5500_REG_PHYCFGR, ESP_FAIL, err, TAG, "wrong PHY register");
    ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_PHYCFGR, reg_value, sizeof(uint8_t)), err, TAG, "read PHY register failed");

err:
    return ret;
}

static esp_err_t emac_w5500_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    memcpy(emac->addr, addr, 6);
    ESP_GOTO_ON_ERROR(w5500_set_mac_addr(emac), err, TAG, "set mac address failed");

err:
    return ret;
}

static esp_err_t emac_w5500_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
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
        ESP_GOTO_ON_ERROR(mac->start(mac), err, TAG, "w5500 start failed");
        break;
    case ETH_LINK_DOWN:
        ESP_LOGD(TAG, "link is down");
        ESP_GOTO_ON_ERROR(mac->stop(mac), err, TAG, "w5500 stop failed");
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown link status");
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
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown speed");
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
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown duplex");
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
    ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_MR(0), &smr, sizeof(smr)), err, TAG, "read SMR failed");
    if (enable) {
        smr &= ~W5500_SMR_MAC_FILTER;
    } else {
        smr |= W5500_SMR_MAC_FILTER;
    }
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_MR(0), &smr, sizeof(smr)), err, TAG, "write SMR failed");

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

static inline bool is_w5500_sane_for_rxtx(emac_w5500_t *emac)
{
    uint8_t phycfg;
    /* phy is ok for rx and tx operations if bits RST and LNK are set (no link down, no reset) */
    if (w5500_read(emac, W5500_REG_PHYCFGR, &phycfg, 1) == ESP_OK && (phycfg & 0x8001)) {
        return true;
    }
   return false;
}

static esp_err_t emac_w5500_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_w5500_t *emac = __containerof(mac, emac_w5500_t, parent);
    uint16_t offset = 0;

    // check if there're free memory to store this packet
    uint16_t free_size = 0;
    ESP_GOTO_ON_ERROR(w5500_get_tx_free_size(emac, &free_size), err, TAG, "get free size failed");
    ESP_GOTO_ON_FALSE(length <= free_size, ESP_ERR_NO_MEM, err, TAG, "free size (%d) < send length (%d)", length, free_size);
    // get current write pointer
    ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_TX_WR(0), &offset, sizeof(offset)), err, TAG, "read TX WR failed");
    offset = __builtin_bswap16(offset);
    // copy data to tx memory
    ESP_GOTO_ON_ERROR(w5500_write_buffer(emac, buf, length, offset), err, TAG, "write frame failed");
    // update write pointer
    offset += length;
    offset = __builtin_bswap16(offset);
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_TX_WR(0), &offset, sizeof(offset)), err, TAG, "write TX WR failed");
    // issue SEND command
    ESP_GOTO_ON_ERROR(w5500_send_command(emac, W5500_SCR_SEND, 100), err, TAG, "issue SEND command failed");

    // pooling the TX done event
    int retry = 0;
    uint8_t status = 0;
    while (!(status & W5500_SIR_SEND)) {
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_IR(0), &status, sizeof(status)), err, TAG, "read SOCK0 IR failed");
        if ((retry++ > 3 && !is_w5500_sane_for_rxtx(emac)) || retry > 10) {
            return ESP_FAIL;
        }
    }
    // clear the event bit
    status  = W5500_SIR_SEND;
    ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_IR(0), &status, sizeof(status)), err, TAG, "write SOCK0 IR failed");

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
        ESP_GOTO_ON_ERROR(w5500_read(emac, W5500_REG_SOCK_RX_RD(0), &offset, sizeof(offset)), err, TAG, "read RX RD failed");
        offset = __builtin_bswap16(offset);
        // read head first
        ESP_GOTO_ON_ERROR(w5500_read_buffer(emac, &rx_len, sizeof(rx_len), offset), err, TAG, "read frame header failed");
        rx_len = __builtin_bswap16(rx_len) - 2; // data size includes 2 bytes of header
        offset += 2;
        // read the payload
        ESP_GOTO_ON_ERROR(w5500_read_buffer(emac, buf, rx_len, offset), err, TAG, "read payload failed, len=%d, offset=%d", rx_len, offset);
        offset += rx_len;
        // update read pointer
        offset = __builtin_bswap16(offset);
        ESP_GOTO_ON_ERROR(w5500_write(emac, W5500_REG_SOCK_RX_RD(0), &offset, sizeof(offset)), err, TAG, "write RX RD failed");
        /* issue RECV command */
        ESP_GOTO_ON_ERROR(w5500_send_command(emac, W5500_SCR_RECV, 100), err, TAG, "issue RECV command failed");
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
    ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL), err, TAG, "lowlevel init failed");
    /* reset w5500 */
    ESP_GOTO_ON_ERROR(w5500_reset(emac), err, TAG, "reset w5500 failed");
    /* verify chip id */
    ESP_GOTO_ON_ERROR(w5500_verify_id(emac), err, TAG, "vefiry chip ID failed");
    /* default setup of internal registers */
    ESP_GOTO_ON_ERROR(w5500_setup_default(emac), err, TAG, "w5500 default setup failed");
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
    spi_bus_remove_device(emac->spi_hdl);
    vSemaphoreDelete(emac->spi_lock);
    free(emac);
    return ESP_OK;
}

esp_eth_mac_t *esp_eth_mac_new_w5500(const eth_w5500_config_t *w5500_config, const eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *ret = NULL;
    emac_w5500_t *emac = NULL;
    ESP_GOTO_ON_FALSE(w5500_config && mac_config, NULL, err, TAG, "invalid argument");
    emac = calloc(1, sizeof(emac_w5500_t));
    ESP_GOTO_ON_FALSE(emac, NULL, err, TAG, "no mem for MAC instance");
    /* w5500 driver is interrupt driven */
    ESP_GOTO_ON_FALSE(w5500_config->int_gpio_num >= 0, NULL, err, TAG, "invalid interrupt gpio number");
    /* SPI device init */
    spi_device_interface_config_t spi_devcfg;
    memcpy(&spi_devcfg, w5500_config->spi_devcfg, sizeof(spi_device_interface_config_t));
    if (w5500_config->spi_devcfg->command_bits == 0 && w5500_config->spi_devcfg->address_bits == 0) {
        /* configure default SPI frame format */
        spi_devcfg.command_bits = 16; // Actually it's the address phase in W5500 SPI frame
        spi_devcfg.address_bits = 8;  // Actually it's the control phase in W5500 SPI frame
    } else {
        ESP_GOTO_ON_FALSE(w5500_config->spi_devcfg->command_bits == 16 || w5500_config->spi_devcfg->address_bits == 8,
                            NULL, err, TAG, "incorrect SPI frame format (command_bits/address_bits)");
    }
    ESP_GOTO_ON_FALSE(spi_bus_add_device(w5500_config->spi_host_id, &spi_devcfg, &emac->spi_hdl) == ESP_OK,
                                            NULL, err, TAG, "adding device to SPI host #%d failed", w5500_config->spi_host_id + 1);
    /* bind methods and attributes */
    emac->sw_reset_timeout_ms = mac_config->sw_reset_timeout_ms;
    emac->int_gpio_num = w5500_config->int_gpio_num;
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
    ESP_GOTO_ON_FALSE(emac->spi_lock, NULL, err, TAG, "create lock failed");
    /* create w5500 task */
    BaseType_t core_num = tskNO_AFFINITY;
    if (mac_config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = esp_cpu_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(emac_w5500_task, "w5500_tsk", mac_config->rx_task_stack_size, emac,
                           mac_config->rx_task_prio, &emac->rx_task_hdl, core_num);
    ESP_GOTO_ON_FALSE(xReturned == pdPASS, NULL, err, TAG, "create w5500 task failed");
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
