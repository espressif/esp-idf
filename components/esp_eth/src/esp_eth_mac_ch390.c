/*
 * SPDX-FileCopyrightText: 2024 Sergey Kharenko
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2024 Sergey Kharenko
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "ch390.h"

static const char *TAG = "ch390.mac";

#define CH390_SPI_LOCK_TIMEOUT_MS           (50)
#define CH390_MAC_TX_WAIT_TIMEOUT_US        (1000)
#define CH390_PHY_OPERATION_TIMEOUT_US      (1000)

typedef struct {
    uint8_t flag;
    uint8_t status;
    uint8_t length_low;
    uint8_t length_high;
} ch390_rx_header_t;

typedef struct {
    spi_device_handle_t hdl;
    SemaphoreHandle_t lock;
} eth_spi_info_t;

typedef struct {
    void *ctx;
    void *(*init)(const void *spi_config);
    esp_err_t (*deinit)(void *spi_ctx);
    esp_err_t (*read)(void *spi_ctx, uint32_t cmd, uint32_t addr, void *data, uint32_t data_len);
    esp_err_t (*write)(void *spi_ctx, uint32_t cmd, uint32_t addr, const void *data, uint32_t data_len);
} eth_spi_custom_driver_t;

typedef struct {
    esp_eth_mac_t           parent;
    esp_eth_mediator_t      *eth;
    eth_spi_custom_driver_t spi;
    TaskHandle_t            rx_task_hdl;
    uint32_t                sw_reset_timeout_ms;
    int                     int_gpio_num;
    esp_timer_handle_t      poll_timer;
    uint32_t                poll_period_ms;
    uint8_t                 addr[6];
    bool                    flow_ctrl_enabled;
    uint8_t                 *rx_buffer;
    uint32_t                rx_len;
} emac_ch390_t;

static void *ch390_spi_init(const void *spi_config)
{
    void *ret = NULL;
    eth_ch390_config_t *ch390_config = (eth_ch390_config_t *)spi_config;
    eth_spi_info_t *spi = calloc(1, sizeof(eth_spi_info_t));
    ESP_GOTO_ON_FALSE(spi, NULL, err, TAG, "no memory for SPI context data");

    /* SPI device init */
    spi_device_interface_config_t spi_devcfg;
    spi_devcfg = *(ch390_config->spi_devcfg);
    if (ch390_config->spi_devcfg->command_bits == 0 && ch390_config->spi_devcfg->address_bits == 0) {
        /* configure default SPI frame format */
        spi_devcfg.command_bits = 1;
        spi_devcfg.address_bits = 7;
    } else {
        ESP_GOTO_ON_FALSE(ch390_config->spi_devcfg->command_bits == 1 && ch390_config->spi_devcfg->address_bits == 7,
                            NULL, err, TAG, "incorrect SPI frame format (command_bits/address_bits)");
    }
    ESP_GOTO_ON_FALSE(spi_bus_add_device(ch390_config->spi_host_id, &spi_devcfg, &spi->hdl) == ESP_OK,
                                            NULL, err, TAG, "adding device to SPI host #%d failed", ch390_config->spi_host_id + 1);

    /* create mutex */
    spi->lock = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(spi->lock, NULL, err, TAG, "create lock failed");

    ret = spi;
    return ret;
err:
    if (spi) {
        if (spi->lock) {
            vSemaphoreDelete(spi->lock);
        }
        free(spi);
    }
    return ret;
}

static esp_err_t ch390_spi_deinit(void *spi_ctx)
{
    esp_err_t ret = ESP_OK;
    eth_spi_info_t *spi = (eth_spi_info_t *)spi_ctx;

    spi_bus_remove_device(spi->hdl);
    vSemaphoreDelete(spi->lock);

    free(spi);
    return ret;
}

static inline bool ch390_spi_lock(eth_spi_info_t *spi)
{
    return xSemaphoreTake(spi->lock, pdMS_TO_TICKS(CH390_SPI_LOCK_TIMEOUT_MS)) == pdTRUE;
}

static inline bool ch390_spi_unlock(eth_spi_info_t *spi)
{
    return xSemaphoreGive(spi->lock) == pdTRUE;
}

static esp_err_t ch390_spi_write(void *spi_ctx, uint32_t cmd, uint32_t addr, const void *value, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    eth_spi_info_t *spi = (eth_spi_info_t *)spi_ctx;

    spi_transaction_t trans = {
        .cmd = cmd,
        .addr = addr,
        .length = 8 * len,
        .tx_buffer = value
    };
    if (ch390_spi_lock(spi)) {
        if (spi_device_polling_transmit(spi->hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ch390_spi_unlock(spi);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

static esp_err_t ch390_spi_read(void *spi_ctx, uint32_t cmd, uint32_t addr, void *value, uint32_t len)
{
    esp_err_t ret = ESP_OK;
    eth_spi_info_t *spi = (eth_spi_info_t *)spi_ctx;

    spi_transaction_t trans = {
        .cmd = cmd,
        .addr = addr,
        .length = 8 * len,
        .rx_buffer = value
    };
    if (ch390_spi_lock(spi)) {
        if (spi_device_polling_transmit(spi->hdl, &trans) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
            ret = ESP_FAIL;
        }
        ch390_spi_unlock(spi);
    } else {
        ret = ESP_ERR_TIMEOUT;
    }
    return ret;
}

/**
 * @brief write value to ch390 internal register
 */
static esp_err_t ch390_register_write(emac_ch390_t *emac, uint8_t reg_addr, uint8_t value)
{
    return emac->spi.write(emac->spi.ctx, CH390_SPI_WR, reg_addr, &value, 1);
}

/**
 * @brief read value from ch390 internal register
 */
static esp_err_t ch390_register_read(emac_ch390_t *emac, uint8_t reg_addr, uint8_t *value)
{
    return emac->spi.read(emac->spi.ctx, CH390_SPI_RD, reg_addr, value, 1);
}

/**
 * @brief write buffer to ch390 internal memory
 */
static esp_err_t ch390_memory_write(emac_ch390_t *emac, uint8_t *buffer, uint32_t len)
{
    return emac->spi.write(emac->spi.ctx, CH390_SPI_WR, CH390_MWCMD, buffer, len);
}

/**
 * @brief read buffer from ch390 internal memory
 */
static esp_err_t ch390_memory_read(emac_ch390_t *emac, uint8_t *buffer, uint32_t len)
{
    return emac->spi.read(emac->spi.ctx, CH390_SPI_RD, CH390_MRCMD, buffer, len);
}

/**
 * @brief read mac address from internal registers
 */
static esp_err_t ch390_get_mac_addr(emac_ch390_t *emac)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < 6; i++) {
        ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_PAR + i, &emac->addr[i]), err, TAG, "read PAR failed");
    }
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief set new mac address to internal registers
 */
static esp_err_t ch390_set_mac_addr(emac_ch390_t *emac)
{
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < 6; i++) {
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_PAR + i, emac->addr[i]), err, TAG, "write PAR failed");
    }
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief clear multicast hash table
 */
static esp_err_t ch390_clear_multicast_table(emac_ch390_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* rx broadcast packet control by bit7 of MAC register 1DH */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_BCASTCR, 0x00), err, TAG, "write BCASTCR failed");
    for (int i = 0; i < 7; i++) {
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_MAR + i, 0x00), err, TAG, "write MAR failed");
    }
    /* enable receive broadcast paclets */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_MAR + 7, 0x80), err, TAG, "write MAR failed");
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief software reset ch390 internal register
 */
static esp_err_t ch390_reset(emac_ch390_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* software reset */
    uint8_t ncr = NCR_RST;
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_NCR, ncr), err, TAG, "write NCR failed");
    uint32_t to = 0;
    for (to = 0; to < emac->sw_reset_timeout_ms / 10; to++) {
        ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_NCR, &ncr), err, TAG, "read NCR failed");
        if (!(ncr & NCR_RST)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_GOTO_ON_FALSE(to < emac->sw_reset_timeout_ms / 10, ESP_ERR_TIMEOUT, err, TAG, "reset timeout");

    /* power on phy */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_GPR, 0x00), err, TAG, "write GPR failed");
    /* mac and phy register won't be accesable within at least 1ms */
    vTaskDelay(pdMS_TO_TICKS(100));

    return ESP_OK;
err:
    return ret;
}

/**
 * @brief verify ch390 chip ID
 */
static esp_err_t ch390_verify_id(emac_ch390_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t id[2];
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_VIDL, &id[0]), err, TAG, "read VIDL failed");
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_VIDH, &id[1]), err, TAG, "read VIDH failed");
    ESP_GOTO_ON_FALSE(0x1C == id[1] && 0x00 == id[0], ESP_ERR_INVALID_VERSION, err, TAG, "wrong Vendor ID");
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_PIDL, &id[0]), err, TAG, "read PIDL failed");
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_PIDH, &id[1]), err, TAG, "read PIDH failed");
    ESP_GOTO_ON_FALSE(0x91 == id[1] && 0x51 == id[0], ESP_ERR_INVALID_VERSION, err, TAG, "wrong Product ID");
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief default setup for ch390 internal registers
 */
static esp_err_t ch390_setup_default(emac_ch390_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* disable wakeup */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_NCR, 0x00), err, TAG, "write NCR failed");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_WCR, 0x00), err, TAG, "write WCR failed");
    /* stop transmitting, enable appending pad, crc for packets */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_TCR, 0x00), err, TAG, "write TCR failed");
    /* stop receiving, no promiscuous mode, no runt packet(size < 64bytes), receive all multicast packets */
    /* discard long packet(size > 1522bytes) and crc error packet, enable watchdog */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_RCR, RCR_DIS_CRC | RCR_ALL), err, TAG, "write RCR failed");
    /* retry late collision packet, at most two transmit command can be issued before transmit complete */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_TCR2, TCR2_RLCP), err, TAG, "write TCR2 failed");
    /* enable auto transmit */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_ATCR, ATCR_AUTO_TX), err, TAG, "write ATCR failed");
    /* generate checksum for UDP, TCP and IPv4 packets */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_TCSCR, TCSCR_IPCSE | TCSCR_TCPCSE | TCSCR_UDPCSE), err, TAG, "write TCSCR failed");
    /* disable check sum for receive packets */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_RCSCSR, 0x00), err, TAG, "write RCSCSR failed");
    /* interrupt pin config: push-pull output, active high */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_INTCR, 0x00), err, TAG, "write INTCR failed");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_INTCKCR, 0x00), err, TAG, "write INTCKCR failed");
    /* no length limitation for rx packets */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_RLENCR, 0x00), err, TAG, "write RLENCR failed");
    // /* 3K-byte for TX and 13K-byte for RX */
    // ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_MEMSCR, 0x00), err, TAG, "write MEMSCR failed");
    /* clear network status: wakeup event, tx complete */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END), err, TAG, "write NSR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ch390_enable_flow_ctrl(emac_ch390_t *emac, bool enable)
{
    esp_err_t ret = ESP_OK;
    if (enable) {
        /* send jam pattern (duration time = 1.15ms) when rx free space < 3k bytes */
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_BPTR, 0x3F), err, TAG, "write BPTR failed");
        /* flow control: high water threshold = 3k bytes, low water threshold = 8k bytes */
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_FCTR, 0x38), err, TAG, "write FCTR failed");
        /* enable flow control */
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_FCR, FCR_FLOW_ENABLE), err, TAG, "write FCR failed");
    } else {
        /* disable flow control */
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_FCR, 0), err, TAG, "write FCR failed");
    }
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief start ch390: enable interrupt and start receive
 */
static esp_err_t emac_ch390_start(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
   /* reset tx and rx memory pointer */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_MPTRCR, MPTRCR_RST_RX | MPTRCR_RST_TX), err, TAG, "write MPTRCR failed");
    /* clear interrupt status */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_ISR, ISR_CLR_STATUS), err, TAG, "write ISR failed");
    /* enable only Rx related interrupts as others are processed synchronously */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_IMR, IMR_PAR | IMR_PRI), err, TAG, "write IMR failed");
    /* enable rx */
    uint8_t rcr = 0;
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_RCR, &rcr), err, TAG, "read RCR failed");
    rcr |= RCR_RXEN;
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_RCR, rcr), err, TAG, "write RCR failed");
    return ESP_OK;
err:
    return ret;
}

/**
 * @brief stop ch390: disable interrupt and stop receive
 */
static esp_err_t emac_ch390_stop(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    /* disable interrupt */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_IMR, 0x00), err, TAG, "write IMR failed");
    /* disable rx */
    uint8_t rcr = 0;
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_RCR, &rcr), err, TAG, "read RCR failed");
    rcr &= ~RCR_RXEN;
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_RCR, rcr), err, TAG, "write RCR failed");
    return ESP_OK;
err:
    return ret;
}

IRAM_ATTR static void ch390_isr_handler(void *arg)
{
    emac_ch390_t *emac = (emac_ch390_t *)arg;
    BaseType_t high_task_wakeup = pdFALSE;
    /* notify ch390 task */
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup != pdFALSE) {
        portYIELD_FROM_ISR();
    }
}

static void ch390_poll_timer(void *arg)
{
    emac_ch390_t *emac = (emac_ch390_t *)arg;
    xTaskNotifyGive(emac->rx_task_hdl);
}

static esp_err_t emac_ch390_set_mediator(esp_eth_mac_t *mac, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac's mediator to null");
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    emac->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_write_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    /* check if phy access is in progress */
    uint8_t epcr = 0;
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_EPCR, &epcr), err, TAG, "read EPCR failed");
    ESP_GOTO_ON_FALSE(!(epcr & EPCR_ERRE), ESP_ERR_INVALID_STATE, err, TAG, "phy is busy");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_EPAR, (uint8_t)( CH390_PHY | phy_reg)), err, TAG, "write EPAR failed");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_EPDRL, (uint8_t)(reg_value & 0xFF)), err, TAG, "write EPDRL failed");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_EPDRH, (uint8_t)((reg_value >> 8) & 0xFF)), err, TAG, "write EPDRH failed");
    /* select PHY and select write operation */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_EPCR, EPCR_EPOS | EPCR_ERPRW), err, TAG, "write EPCR failed");
    /* polling the busy flag */
    uint32_t to = 0;
    do {
        esp_rom_delay_us(100);
        ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_EPCR, &epcr), err, TAG, "read EPCR failed");
        to += 100;
    } while ((epcr & EPCR_ERRE) && to < CH390_PHY_OPERATION_TIMEOUT_US);
    ESP_GOTO_ON_FALSE(!(epcr & EPCR_ERRE), ESP_ERR_TIMEOUT, err, TAG, "phy is busy");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(reg_value, ESP_ERR_INVALID_ARG, err, TAG, "can't set reg_value to null");
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    /* check if phy access is in progress */
    uint8_t epcr = 0;
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_EPCR, &epcr), err, TAG, "read EPCR failed");
    ESP_GOTO_ON_FALSE(!(epcr & 0x01), ESP_ERR_INVALID_STATE, err, TAG, "phy is busy");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_EPAR, (uint8_t)(CH390_PHY | phy_reg)), err, TAG, "write EPAR failed");
    /* Select PHY and select read operation */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_EPCR, 0x0C), err, TAG, "write EPCR failed");
    /* polling the busy flag */
    uint32_t to = 0;
    do {
        esp_rom_delay_us(100);
        ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_EPCR, &epcr), err, TAG, "read EPCR failed");
        to += 100;
    } while ((epcr & EPCR_ERRE) && to < CH390_PHY_OPERATION_TIMEOUT_US);
    ESP_GOTO_ON_FALSE(!(epcr & EPCR_ERRE), ESP_ERR_TIMEOUT, err, TAG, "phy is busy");
    uint8_t value_h = 0;
    uint8_t value_l = 0;
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_EPDRH, &value_h), err, TAG, "read EPDRH failed");
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_EPDRL, &value_l), err, TAG, "read EPDRL failed");
    *reg_value = (value_h << 8) | value_l;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac addr to null");
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    memcpy(emac->addr, addr, 6);
    ESP_GOTO_ON_ERROR(ch390_set_mac_addr(emac), err, TAG, "set mac address failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "can't set mac addr to null");
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    memcpy(addr, emac->addr, 6);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    switch (link)
    {
    case ETH_LINK_UP:
        ESP_GOTO_ON_ERROR(mac->start(mac), err, TAG, "ch390 start failed");
        if (emac->poll_timer) {
            ESP_GOTO_ON_ERROR(esp_timer_start_periodic(emac->poll_timer, emac->poll_period_ms * 1000),
                                err, TAG, "start poll timer failed");
        }
        break;
    case ETH_LINK_DOWN:
        ESP_GOTO_ON_ERROR(mac->stop(mac), err, TAG, "ch390 stop failed");
        if (emac->poll_timer) {
            ESP_GOTO_ON_ERROR(esp_timer_stop(emac->poll_timer),
                                err, TAG, "stop poll timer failed");
        }
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG, "unknown link status");
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
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
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_set_duplex(esp_eth_mac_t *mac, eth_duplex_t duplex)
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
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    esp_err_t ret = ESP_OK;
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    uint8_t rcr = 0;
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_RCR, &rcr), err, TAG, "read RCR failed");
    if (enable) {
        rcr |= RCR_PRMSC;
    } else {
        rcr &= ~RCR_PRMSC;
    }
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_RCR, rcr), err, TAG, "write RCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch390_enable_flow_ctrl(esp_eth_mac_t *mac, bool enable)
{
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    emac->flow_ctrl_enabled = enable;
    return ESP_OK;
}

static esp_err_t emac_ch390_set_peer_pause_ability(esp_eth_mac_t *mac, uint32_t ability)
{
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    // we want to enable flow control, and peer does support pause function
    // then configure the MAC layer to enable flow control feature
    if (emac->flow_ctrl_enabled && ability)
        ch390_enable_flow_ctrl(emac, true);

    else
    {
        ch390_enable_flow_ctrl(emac, false);
        ESP_LOGD(TAG, "Flow control not enabled for the link");
    }
    return ESP_OK;
}

static esp_err_t emac_ch390_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    /* Check if last transmit complete */
    uint8_t tcr = 0;

    ESP_GOTO_ON_FALSE(length <= ETH_MAX_PACKET_SIZE, ESP_ERR_INVALID_ARG, err,
                      TAG, "frame size is too big (actual %lu, maximum %u)", length, ETH_MAX_PACKET_SIZE);

    int64_t wait_time = esp_timer_get_time();
    do
    {
        ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_TCR, &tcr), err, TAG, "read TCR failed");
    } while ((tcr & TCR_TXREQ) && ((esp_timer_get_time() - wait_time) < CH390_MAC_TX_WAIT_TIMEOUT_US));

    if (tcr & TCR_TXREQ)
    {
        ESP_LOGE(TAG, "last transmit still in progress, cannot send.");
        return ESP_ERR_INVALID_STATE;
    }

    /* set tx length */
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_TXPLL, length & 0xFF), err, TAG, "write TXPLL failed");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_TXPLH, (length >> 8) & 0xFF), err, TAG, "write TXPLH failed");
    /* copy data to tx memory */
    ESP_GOTO_ON_ERROR(ch390_memory_write(emac, buf, length), err, TAG, "write memory failed");
    /* issue tx polling command */
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_TCR, &tcr), err, TAG, "read TCR failed");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_TCR, tcr | TCR_TXREQ), err, TAG, "write TCR failed");
    return ESP_OK;
err:
    return ret;
}

static esp_err_t ch390_drop_frame(emac_ch390_t *emac, uint16_t length){
    esp_err_t ret = ESP_OK;
    uint8_t mrrh, mrrl;

    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_MRRH, &mrrh), err, TAG, "read MDRAH failed");
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_MRRL, &mrrl), err, TAG, "read MDRAL failed");

    uint16_t addr = mrrh << 8 | mrrl;
    /* include 4B for header */
    addr += length;

    addr = addr < 0x4000 ? addr : addr - 0x3400;
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_MRRH, addr >> 8), err, TAG, "write MDRAH failed");
    ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_MRRL, addr & 0xFF), err, TAG, "write MDRAL failed");
err:
    return ret;
}

static esp_err_t emac_ch390_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);

    uint8_t ready;
    /* dummy read, get the most updated data */
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_MRCMDX, &ready), err, TAG, "read MRCMDX failed");
    ESP_GOTO_ON_ERROR(ch390_register_read(emac, CH390_MRCMDX, &ready), err, TAG, "read MRCMDX failed");

    // if ready != 1 or 0 reset device
    if (ready & CH390_PKT_ERR)
    {
        // Reset RX FIFO pointer
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_RCR, RCR_DEFAULT), err,
                          TAG, "write RCR failed"); // RX disable
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_MPTRCR, MPTRCR_RST_RX), err,
                          TAG, "write MPTRCR failed"); // Reset RX FIFO pointer
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_MRRH, 0x0C), err,
                          TAG, "write MRRH failed");
        esp_rom_delay_us(1000);
        ESP_GOTO_ON_ERROR(ch390_register_write(emac, CH390_RCR, RCR_RXEN), err,
                          TAG, "write RCR failed"); // RX enable

        ESP_LOGE(TAG,"PACK ERR");
    }
    else
    {
        __attribute__((aligned(4))) ch390_rx_header_t rx_header; // SPI driver needs the rx buffer 4 byte align

        if (ready & CH390_PKT_RDY)
        {
            ESP_GOTO_ON_ERROR(ch390_memory_read(emac, (uint8_t *)&(rx_header), sizeof(rx_header)),
                              err, TAG, "peek rx header failed");
            *length = (rx_header.length_high << 8) + rx_header.length_low;
            if ((rx_header.status & 0x3F) || (*length > CH390_PKT_MAX))
            {
                ch390_drop_frame(emac, *length);
                *length = 0;
            }
            else
            {
                ESP_GOTO_ON_ERROR(ch390_memory_read(emac, buf, *length), err, TAG, "read rx data failed");
                *length -= ETH_CRC_LEN;
            }
        }
        else
            *length = 0;
        return ESP_OK;
    }

err:
    *length = 0;
    return ret;
}

static esp_err_t emac_ch390_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    if (emac->int_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(emac->int_gpio_num);
        gpio_set_direction(emac->int_gpio_num, GPIO_MODE_INPUT);
        gpio_set_pull_mode(emac->int_gpio_num, GPIO_PULLDOWN_ONLY);
        gpio_set_intr_type(emac->int_gpio_num, GPIO_INTR_POSEDGE);
        gpio_intr_enable(emac->int_gpio_num);
        gpio_isr_handler_add(emac->int_gpio_num, ch390_isr_handler, emac);
    }
    ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL), err, TAG, "lowlevel init failed");
    /* reset ch390 */
    ESP_GOTO_ON_ERROR(ch390_reset(emac), err, TAG, "reset ch390 failed");
    /* verify chip id */
    ESP_GOTO_ON_ERROR(ch390_verify_id(emac), err, TAG, "vefiry chip ID failed");
    /* default setup of internal registers */
    ESP_GOTO_ON_ERROR(ch390_setup_default(emac), err, TAG, "ch390 default setup failed");
    /* clear multicast hash table */
    ESP_GOTO_ON_ERROR(ch390_clear_multicast_table(emac), err, TAG, "clear multicast table failed");
    /* get emac address from eeprom */
    ESP_GOTO_ON_ERROR(ch390_get_mac_addr(emac), err, TAG, "fetch ethernet mac address failed");
    return ESP_OK;
err:
    if (emac->int_gpio_num >= 0) {
        gpio_isr_handler_remove(emac->int_gpio_num);
        gpio_reset_pin(emac->int_gpio_num);
    }
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_ch390_deinit(esp_eth_mac_t *mac)
{
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    mac->stop(mac);
    if (emac->int_gpio_num >= 0) {
        gpio_isr_handler_remove(emac->int_gpio_num);
        gpio_reset_pin(emac->int_gpio_num);
    }
    if (emac->poll_timer && esp_timer_is_active(emac->poll_timer)) {
        esp_timer_stop(emac->poll_timer);
    }
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ESP_OK;
}

static void emac_ch390_task(void *arg)
{
    emac_ch390_t *emac = (emac_ch390_t *)arg;
    uint8_t status = 0;
    uint8_t *cache;
    while (1) {
        // check if the task receives any notification
        if (emac->int_gpio_num >= 0) {                                   // if in interrupt mode
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) == 0 &&   // if no notification ...
                gpio_get_level(emac->int_gpio_num) == 0) {               // ...and no interrupt asserted
                continue;                                                // -> just continue to check again
            }
        } else {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        /* clear interrupt status */
        ch390_register_read(emac, CH390_ISR, &status);
        ch390_register_write(emac, CH390_ISR, status);
        /* packet received */
        if (status & ISR_PR) {
            do {
                if (emac->parent.receive(&emac->parent, emac->rx_buffer, &emac->rx_len) == ESP_OK) {
                    if (emac->rx_len == 0) {
                        break;
                    }
                    else {
                        ESP_LOGD(TAG, "receive len=%lu", emac->rx_len);
                        /* pass the buffer to stack (e.g. TCP/IP layer) */
                        cache=malloc(emac->rx_len);
                        memcpy(cache,emac->rx_buffer,emac->rx_len);
                        emac->eth->stack_input(emac->eth, cache, emac->rx_len);
                        // emac->eth->stack_input(emac->eth, emac->rx_buffer, emac->rx_len);
                    }
                } else {
                    ESP_LOGE(TAG, "frame read from module failed");
                    break;
                }
            } while (1);
        }
    }
    vTaskDelete(NULL);
}

static esp_err_t emac_ch390_del(esp_eth_mac_t *mac)
{
    emac_ch390_t *emac = __containerof(mac, emac_ch390_t, parent);
    if (emac->poll_timer) {
        esp_timer_delete(emac->poll_timer);
    }
    vTaskDelete(emac->rx_task_hdl);
    emac->spi.deinit(emac->spi.ctx);
    heap_caps_free(emac->rx_buffer);
    free(emac);
    return ESP_OK;
}

esp_eth_mac_t *esp_eth_mac_new_ch390(const eth_ch390_config_t *ch390_config, const eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *ret = NULL;
    emac_ch390_t *emac = NULL;
    ESP_GOTO_ON_FALSE(ch390_config, NULL, err, TAG, "can't set ch390 specific config to null");
    ESP_GOTO_ON_FALSE(mac_config, NULL, err, TAG, "can't set mac config to null");
    emac = calloc(1, sizeof(emac_ch390_t));
    ESP_GOTO_ON_FALSE(emac, NULL, err, TAG, "calloc emac failed");
    /* ch390 receive is driven by interrupt only for now*/
    ESP_GOTO_ON_FALSE((ch390_config->int_gpio_num >= 0) != (ch390_config->poll_period_ms > 0), NULL, err, TAG, "invalid configuration argument combination");
    /* bind methods and attributes */
    emac->sw_reset_timeout_ms = mac_config->sw_reset_timeout_ms;
    emac->int_gpio_num = ch390_config->int_gpio_num;
    emac->poll_period_ms = ch390_config->poll_period_ms;
    emac->parent.set_mediator = emac_ch390_set_mediator;
    emac->parent.init = emac_ch390_init;
    emac->parent.deinit = emac_ch390_deinit;
    emac->parent.start = emac_ch390_start;
    emac->parent.stop = emac_ch390_stop;
    emac->parent.del = emac_ch390_del;
    emac->parent.write_phy_reg = emac_ch390_write_phy_reg;
    emac->parent.read_phy_reg = emac_ch390_read_phy_reg;
    emac->parent.set_addr = emac_ch390_set_addr;
    emac->parent.get_addr = emac_ch390_get_addr;
    emac->parent.set_speed = emac_ch390_set_speed;
    emac->parent.set_duplex = emac_ch390_set_duplex;
    emac->parent.set_link = emac_ch390_set_link;
    emac->parent.set_promiscuous = emac_ch390_set_promiscuous;
    emac->parent.set_peer_pause_ability = emac_ch390_set_peer_pause_ability;
    emac->parent.enable_flow_ctrl = emac_ch390_enable_flow_ctrl;
    emac->parent.transmit = emac_ch390_transmit;
    emac->parent.receive = emac_ch390_receive;

    if (ch390_config->custom_spi_driver.init != NULL && ch390_config->custom_spi_driver.deinit != NULL
        && ch390_config->custom_spi_driver.read != NULL && ch390_config->custom_spi_driver.write != NULL) {
        ESP_LOGD(TAG, "Using user's custom SPI Driver");
        emac->spi.init = ch390_config->custom_spi_driver.init;
        emac->spi.deinit = ch390_config->custom_spi_driver.deinit;
        emac->spi.read = ch390_config->custom_spi_driver.read;
        emac->spi.write = ch390_config->custom_spi_driver.write;
        /* Custom SPI driver device init */
        ESP_GOTO_ON_FALSE((emac->spi.ctx = emac->spi.init(ch390_config->custom_spi_driver.config)) != NULL, NULL, err, TAG, "SPI initialization failed");
    } else {
        ESP_LOGD(TAG, "Using default SPI Driver");
        emac->spi.init = ch390_spi_init;
        emac->spi.deinit = ch390_spi_deinit;
        emac->spi.read = ch390_spi_read;
        emac->spi.write = ch390_spi_write;
        /* SPI device init */
        ESP_GOTO_ON_FALSE((emac->spi.ctx = emac->spi.init(ch390_config)) != NULL, NULL, err, TAG, "SPI initialization failed");
    }

    /* create ch390 task */
    BaseType_t core_num = tskNO_AFFINITY;
    if (mac_config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = esp_cpu_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(emac_ch390_task, "ch390_tsk", mac_config->rx_task_stack_size, emac,
                           mac_config->rx_task_prio, &emac->rx_task_hdl, core_num);
    ESP_GOTO_ON_FALSE(xReturned == pdPASS, NULL, err, TAG, "create ch390 task failed");

    emac->rx_buffer = heap_caps_malloc(CH390_PKT_MAX, MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(emac->rx_buffer, NULL, err, TAG, "RX buffer allocation failed");

    if (emac->int_gpio_num < 0) {
        const esp_timer_create_args_t poll_timer_args = {
            .callback = ch390_poll_timer,
            .name = "emac_spi_poll_timer",
            .arg = emac,
            .skip_unhandled_events = true
        };
        ESP_GOTO_ON_FALSE(esp_timer_create(&poll_timer_args, &emac->poll_timer) == ESP_OK, NULL, err, TAG, "create poll timer failed");
    }


    return &(emac->parent);

err:
    if (emac) {
        if (emac->rx_task_hdl) {
            vTaskDelete(emac->rx_task_hdl);
        }
        if (emac->spi.ctx) {
            emac->spi.deinit(emac->spi.ctx);
        }
        heap_caps_free(emac->rx_buffer);
        free(emac);
    }
    return ret;
}
