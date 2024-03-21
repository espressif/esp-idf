/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2024 Sergey Kharenko
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_cpu.h"
#include "esp_err.h"
#include "esp_eth_mac.h"
#include "esp_eth_spec.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "ch395.h"
#include "portmacro.h"

static const char *TAG = "ch395.mac";

#define CH395_UART_DEFAULT_BAUDRATE         (9600)
#define CH395_MAC_TX_WAIT_TIMEOUT_MS        (50)
#define CH395_LOCK_TIMEOUT_MS               (50)
#define CH395_MIN_TX_PACKSZ                 (60)
#define CH395_POWER_ON_DELAY_MS             (200)
#define CH395_RX_WAIT_TIMEOUT_MS            (40)

const static uint8_t CH395_CMD_TXBUFFER = CMD30_WRITE_SEND_BUF_SN;
const static uint8_t CH395_CMD_RXBUFFER = CMD30_READ_RECV_BUF_SN;
const static uint8_t CH395_CMD_GETPHY = CMD01_GET_PHY_STATUS;
const static uint8_t CH395_CMD_SETPHY = CMD10_SET_PHY;
const static uint8_t CH395_CMD_GETRXSZ = CMD12_GET_RECV_LEN_SN;
const static uint8_t CH395_PARAM_SOCKID = 0;

const static uint8_t CH395_PADDING_BUFF[CH395_MIN_TX_PACKSZ] = {0x00};
const static uint8_t CH395_UART_HEAD_BUFF[] = {SER_SYNC_CODE1, SER_SYNC_CODE2};


typedef struct {
    int cs_pin;
    spi_device_handle_t hdl;
    spi_transaction_t trans;
    uint8_t cache[2];
    SemaphoreHandle_t lock;
} eth_spi_ctx_t;

typedef struct {
    uart_port_t port;
    uint8_t cache[2];
    SemaphoreHandle_t lock;
} eth_uart_ctx_t;

typedef struct {
    void *ctx;
    void *(*init)(const void *config);
    esp_err_t (*deinit)(void *ctx);
    esp_err_t (*io)(void *ctx, uint8_t cmd, uint8_t *tx_data,
                    uint32_t tx_len, uint8_t *rx_data, uint32_t rx_len);

    esp_err_t (*tx_buffer)(void *ctx, uint8_t *data, uint32_t len);
    esp_err_t (*rx_buffer)(void *ctx, uint8_t *data, uint32_t len);
    esp_err_t (*get_rx_size)(void *ctx, uint32_t *len);

    esp_err_t (*set_phy)(void *ctx, uint8_t mode);
    esp_err_t (*get_phy)(void *ctx, uint8_t *sta);
} eth_generic_driver_t;

typedef struct {
    esp_eth_mac_t parent;
    esp_eth_mediator_t *eth;

    eth_generic_driver_t drv;

    int int_gpio_num;

    TaskHandle_t rx_task_hdl;
    SemaphoreHandle_t tx_busy_lock;

    esp_timer_handle_t poll_timer;
    uint32_t poll_period_ms;

    uint8_t version;
    uint8_t addr[6];
    uint8_t *rx_buffer;
    uint32_t rx_len;
} emac_ch395_t;


static void *ch395_spi_init(const void *config)
{
    void *ret = NULL;
    eth_ch395_config_t *ch395_config = (eth_ch395_config_t *)config;
    eth_spi_ctx_t *spi = calloc(1, sizeof(eth_spi_ctx_t));
    ESP_GOTO_ON_FALSE(spi, NULL, err, TAG, "no memory for SPI context data");

    /* SPI device init */
    spi_device_interface_config_t *spi_devcfg;
    spi_devcfg = ch395_config->spi.spi_devcfg;

    /* Cancel autocontrol of CS pin*/
    ESP_GOTO_ON_FALSE(
        spi_devcfg->spics_io_num >= 0, NULL, err, TAG, "invalid CS pin number");

    spi->cs_pin = spi_devcfg->spics_io_num;
    spi_devcfg->spics_io_num = -1;

    gpio_config_t gpio_cfg = {.pin_bit_mask =
                                  1ull << (spi->cs_pin),
                              .mode = GPIO_MODE_OUTPUT,
                              .intr_type = GPIO_INTR_DISABLE
                             };
    gpio_config(&gpio_cfg);
    ESP_GOTO_ON_FALSE(gpio_set_level(spi->cs_pin, 1)
                      == ESP_OK,
                      NULL, err, TAG, "cannot init CS pin");

    ESP_GOTO_ON_FALSE(
        spi_devcfg->command_bits == 0 && spi_devcfg->address_bits == 0, NULL,
        err, TAG, "incorrect SPI frame format (command_bits/address_bits)");
    ESP_GOTO_ON_FALSE(
        spi_bus_add_device(ch395_config->spi.spi_host_id, spi_devcfg, &spi->hdl)
        == ESP_OK,
        NULL, err, TAG, "adding device to SPI host #%d failed",
        ch395_config->spi.spi_host_id + 1);
    /* create mutex */
    spi->lock = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(spi->lock, NULL, err, TAG, "create lock failed");

    spi->trans.flags = 0;
    spi->trans.length = 8;

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

static esp_err_t ch395_spi_deinit(void *spi_ctx)
{
    esp_err_t ret = ESP_OK;
    eth_spi_ctx_t *spi = (eth_spi_ctx_t *)spi_ctx;

    gpio_reset_pin(spi->cs_pin);
    spi_bus_remove_device(spi->hdl);
    vSemaphoreDelete(spi->lock);

    free(spi);
    return ret;
}


static inline bool ch395_spi_lock(eth_spi_ctx_t *spi)
{
    return xSemaphoreTake(spi->lock, pdMS_TO_TICKS(CH395_LOCK_TIMEOUT_MS))
           == pdTRUE;
}

static inline bool ch395_spi_unlock(eth_spi_ctx_t *spi)
{
    return xSemaphoreGive(spi->lock) == pdTRUE;
}

static inline void ch395_spi_start(eth_spi_ctx_t *spi)
{
    gpio_set_level(spi->cs_pin, 0);
}

static inline void ch395_spi_stop(eth_spi_ctx_t *spi)
{
    gpio_set_level(spi->cs_pin, 1);
}

static esp_err_t ch395_spi_io(void *spi_ctx, uint8_t cmd, uint8_t *tx_data,
                              uint32_t tx_len, uint8_t *rx_data,
                              uint32_t rx_len)
{
    eth_spi_ctx_t *spi = (eth_spi_ctx_t *)spi_ctx;

    if (!ch395_spi_lock(spi)) {
        return ESP_ERR_TIMEOUT;
    }
    ch395_spi_start(spi);

    spi->trans.length = 8;
    spi->trans.tx_buffer = &cmd;
    spi->trans.rx_buffer = NULL;

    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    if (tx_len > 0) {
        for (uint32_t i = 0; i < tx_len; i++) {
            spi->trans.tx_buffer = tx_data + i;
            if (spi_device_polling_transmit(spi->hdl, &(spi->trans))
                    != ESP_OK) {
                ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__,
                         __LINE__);
                ch395_spi_stop(spi);
                ch395_spi_unlock(spi);
                return ESP_FAIL;
            }
        }
    }

    if (rx_len > 0) {
        spi->trans.tx_buffer = NULL;
        for (uint32_t i = 0; i < rx_len; i++) {
            spi->trans.rx_buffer = rx_data + i;
            if (spi_device_polling_transmit(spi->hdl, &(spi->trans))
                    != ESP_OK) {
                ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__,
                         __LINE__);
                ch395_spi_stop(spi);
                ch395_spi_unlock(spi);
                return ESP_FAIL;
            }
        }
    }

    ch395_spi_stop(spi);
    ch395_spi_unlock(spi);

    return ESP_OK;
}

static esp_err_t ch395_spi_tx_buffer(void *spi_ctx, uint8_t *data,
                                 uint32_t len)
{
    eth_spi_ctx_t *spi = (eth_spi_ctx_t *)spi_ctx;

    if (!ch395_spi_lock(spi)) {
        return ESP_ERR_TIMEOUT;
    }
    ch395_spi_start(spi);

    spi->trans.length = 8;
    spi->trans.tx_buffer = &CH395_CMD_TXBUFFER;
    spi->trans.rx_buffer = NULL;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->trans.tx_buffer = &CH395_PARAM_SOCKID;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    if (len < CH395_MIN_TX_PACKSZ) {
        spi->cache[0] = (uint8_t)CH395_MIN_TX_PACKSZ;
        spi->cache[1] = (uint8_t)(CH395_MIN_TX_PACKSZ >> 8);
    } else {
        spi->cache[0] = (uint8_t)len;
        spi->cache[1] = (uint8_t)(len >> 8);
    }

    spi->trans.length = 8 * 2;
    spi->trans.tx_buffer = spi->cache;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__,
                    __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->trans.length = 8 * len;
    spi->trans.tx_buffer = data;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__,
                    __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    if (len < CH395_MIN_TX_PACKSZ) {
        spi->trans.length = 8 * (CH395_MIN_TX_PACKSZ - len);
        spi->trans.tx_buffer = CH395_PADDING_BUFF;
        if (spi_device_polling_transmit(spi->hdl, &(spi->trans))
                != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__,
                        __LINE__);
            ch395_spi_stop(spi);
            ch395_spi_unlock(spi);
            return ESP_FAIL;
        }
    }

    ch395_spi_stop(spi);
    ch395_spi_unlock(spi);

    return ESP_OK;
}


static esp_err_t ch395_spi_rx_buffer(void *spi_ctx, uint8_t *data,
                                 uint32_t len)
{
    eth_spi_ctx_t *spi = (eth_spi_ctx_t *)spi_ctx;

    if (!ch395_spi_lock(spi)) {
        return ESP_ERR_TIMEOUT;
    }

    ch395_spi_start(spi);

    spi->trans.length = 8;
    spi->trans.tx_buffer = &CH395_CMD_RXBUFFER;
    spi->trans.rx_buffer = NULL;

    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->trans.tx_buffer = &CH395_PARAM_SOCKID;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->cache[0] = (uint8_t)len;
    spi->cache[1] = (uint8_t)(len >> 8);

    spi->trans.length = 8 * 2;
    spi->trans.tx_buffer = spi->cache;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__,
                    __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->trans.length = 8 * 4;
    spi->trans.tx_buffer = NULL;
    while(len>4){
        spi->trans.rx_buffer = data;
        if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
            ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__,
                        __LINE__);
            ch395_spi_stop(spi);
            ch395_spi_unlock(spi);
            return ESP_FAIL;
        }
        data += 4;
        len -= 4;
    }

    spi->trans.length = 8 * len;
    spi->trans.rx_buffer = data;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__,
                    __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    ch395_spi_stop(spi);
    ch395_spi_unlock(spi);

    return ESP_OK;
}

static esp_err_t ch395_spi_get_rx_size(void *spi_ctx, uint32_t *len)
{
    eth_spi_ctx_t *spi = (eth_spi_ctx_t *)spi_ctx;

    if (!ch395_spi_lock(spi)) {
        return ESP_ERR_TIMEOUT;
    }
    ch395_spi_start(spi);

    spi->trans.length = 8;
    spi->trans.tx_buffer = &CH395_CMD_GETRXSZ;
    spi->trans.rx_buffer = NULL;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->trans.tx_buffer = &CH395_PARAM_SOCKID;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->trans.length = 8 * 2;
    spi->trans.tx_buffer = NULL;
    spi->trans.rx_buffer = spi->cache;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    ch395_spi_stop(spi);
    ch395_spi_unlock(spi);

    *len = spi->cache[0] + ((spi->cache[1]) << 8);

    return ESP_OK;
}

static esp_err_t ch395_spi_get_phy_status(void *spi_ctx, uint8_t *sta)
{
    eth_spi_ctx_t *spi = (eth_spi_ctx_t *)spi_ctx;
    if (!ch395_spi_lock(spi)) {
        return ESP_ERR_TIMEOUT;
    }
    ch395_spi_start(spi);

    spi->trans.length = 8;
    spi->trans.tx_buffer = &CH395_CMD_GETPHY;
    spi->trans.rx_buffer = NULL;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->trans.tx_buffer = NULL;
    spi->trans.rx_buffer = sta;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    ch395_spi_stop(spi);
    ch395_spi_unlock(spi);

    return ESP_OK;
}

static esp_err_t ch395_spi_set_phy_mode(void *spi_ctx, uint8_t mode)
{
    eth_spi_ctx_t *spi = (eth_spi_ctx_t *)spi_ctx;
    if (!ch395_spi_lock(spi)) {
        return ESP_ERR_TIMEOUT;
    }
    ch395_spi_start(spi);

    spi->trans.length = 8;
    spi->trans.tx_buffer = &CH395_CMD_SETPHY;
    spi->trans.rx_buffer = NULL;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    spi->trans.tx_buffer = &mode;
    if (spi_device_polling_transmit(spi->hdl, &(spi->trans)) != ESP_OK) {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ch395_spi_stop(spi);
        ch395_spi_unlock(spi);
        return ESP_FAIL;
    }

    ch395_spi_stop(spi);
    ch395_spi_unlock(spi);

    return ESP_OK;
}



static void *ch395_uart_init(const void *config)
{
    void *ret = NULL;
    eth_ch395_config_t *ch395_config = (eth_ch395_config_t *)config;
    eth_uart_ctx_t *uart = calloc(1, sizeof(eth_uart_ctx_t));
    ESP_GOTO_ON_FALSE(uart, NULL, err, TAG, "no memory for UART context data");

    /* UART device init */
    uart_config_t uart_devcfg;
    uart_devcfg = *(ch395_config->uart.uart_devcfg);

    ESP_GOTO_ON_FALSE(uart_driver_install(ch395_config->uart.uart_port_id, 2048,
                                          0, 0, NULL, 0)
                          == ESP_OK,
                      NULL, err, TAG, "uart port init failed");
    ESP_GOTO_ON_FALSE(uart_devcfg.data_bits == UART_DATA_8_BITS
                          && uart_devcfg.flow_ctrl == UART_HW_FLOWCTRL_DISABLE
                          && uart_devcfg.parity == UART_PARITY_DISABLE
                          && uart_devcfg.stop_bits == UART_STOP_BITS_1,
                      NULL, err, TAG, "incorrect UART configuration");
    ESP_GOTO_ON_FALSE(
        uart_param_config(ch395_config->uart.uart_port_id, &uart_devcfg)
            == ESP_OK,
        NULL, err, TAG, "uart param init failed");
    ESP_GOTO_ON_FALSE(uart_set_pin(ch395_config->uart.uart_port_id,
                                   ch395_config->uart.uart_tx_gpio_num,
                                   ch395_config->uart.uart_rx_gpio_num, -1, -1)
                          == ESP_OK,
                      NULL, err, TAG, "uart pin set failed");

    uart->port = ch395_config->uart.uart_port_id;

    /* create mutex */
    uart->lock = xSemaphoreCreateMutex();
    ESP_GOTO_ON_FALSE(uart->lock, NULL, err, TAG, "create lock failed");

    ret = (void *)uart;
    return ret;

err:
    if (uart) {
        if (uart->lock) { vSemaphoreDelete(uart->lock); }
        free(uart);
    }
    return ret;
}

static esp_err_t ch395_uart_deinit(void *uart_ctx)
{
    esp_err_t ret = ESP_OK;
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)uart_ctx;

    uart_driver_delete(uart->port);
    vSemaphoreDelete(uart->lock);

    free(uart);
    return ret;
}


static inline bool ch395_uart_lock(eth_uart_ctx_t *uart)
{
    return xSemaphoreTake(uart->lock, pdMS_TO_TICKS(CH395_LOCK_TIMEOUT_MS))
           == pdTRUE;
}

static inline bool ch395_uart_unlock(eth_uart_ctx_t *uart)
{
    return xSemaphoreGive(uart->lock) == pdTRUE;
}

static esp_err_t ch395_uart_io(void *uart_ctx, uint8_t cmd, uint8_t *tx_data,
                               uint32_t tx_len, uint8_t *rx_data,
                               uint32_t rx_len)
{
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)uart_ctx;

    if (!ch395_uart_lock(uart)) {
        return ESP_ERR_TIMEOUT;
    }

    uart_flush(uart->port);
    if (uart_write_bytes(uart->port, CH395_UART_HEAD_BUFF, 2) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &cmd, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (tx_len > 0) {
        if (uart_write_bytes(uart->port, tx_data, tx_len) < 0) {
            ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__,
                     __LINE__);
            ch395_uart_unlock(uart);
            return ESP_FAIL;
        }
    }

    if (rx_len > 0) {
        if (uart_read_bytes(uart->port, rx_data, rx_len,
                            pdMS_TO_TICKS(CH395_RX_WAIT_TIMEOUT_MS))
                < 0) {
            ESP_LOGE(TAG, "%s(%d): uart receive failed", __FUNCTION__,
                     __LINE__);
            ch395_uart_unlock(uart);
            return ESP_FAIL;
        }
    }

    ch395_uart_unlock(uart);
    return ESP_OK;
}

static esp_err_t ch395_uart_tx_buffer(void *uart_ctx, uint8_t *data,
                                 uint32_t len)
{
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)uart_ctx;

    if (!ch395_uart_lock(uart)) {
        return ESP_ERR_TIMEOUT;
    }

    if (uart_write_bytes(uart->port, CH395_UART_HEAD_BUFF, 2) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &CH395_CMD_TXBUFFER, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &CH395_PARAM_SOCKID, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (len < CH395_MIN_TX_PACKSZ) {
        uart->cache[0] = (uint8_t)CH395_MIN_TX_PACKSZ;
        uart->cache[1] = (uint8_t)(CH395_MIN_TX_PACKSZ >> 8);
    } else {
        uart->cache[0] = (uint8_t)len;
        uart->cache[1] = (uint8_t)(len >> 8);
    }

    if (uart_write_bytes(uart->port, uart->cache, 2) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__,
                 __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, data, len) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (len < 60) {
        if (uart_write_bytes(uart->port, CH395_PADDING_BUFF, 60 - len) < 0) {
            ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__,
                     __LINE__);
            ch395_uart_unlock(uart);
            return ESP_FAIL;
        }
    }

    ch395_uart_unlock(uart);
    return ESP_OK;
}


static esp_err_t ch395_uart_rx_buffer(void *uart_ctx, uint8_t *data,
                                 uint32_t len)
{
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)uart_ctx;

    if (!ch395_uart_lock(uart)) {
        return ESP_ERR_TIMEOUT;
    }

    uart_flush(uart->port);
    if (uart_write_bytes(uart->port, CH395_UART_HEAD_BUFF, 2) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &CH395_CMD_RXBUFFER, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &CH395_PARAM_SOCKID, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    uart->cache[0] = (uint8_t)len;
    uart->cache[1] = (uint8_t)(len >> 8);

    if (uart_write_bytes(uart->port, uart->cache, 2) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__,
                 __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_read_bytes(uart->port, data, len,
                        pdMS_TO_TICKS(CH395_RX_WAIT_TIMEOUT_MS))
            < 0) {
        ESP_LOGE(TAG, "%s(%d): uart receive failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    ch395_uart_unlock(uart);
    return ESP_OK;
}



static esp_err_t ch395_uart_get_rx_size(void *uart_ctx, uint32_t *len)
{
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)uart_ctx;

    if (!ch395_uart_lock(uart)) {
        return ESP_ERR_TIMEOUT;
    }

    uart_flush(uart->port);
    if (uart_write_bytes(uart->port, CH395_UART_HEAD_BUFF, 2) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &CH395_CMD_GETRXSZ, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &CH395_PARAM_SOCKID, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_read_bytes(uart->port, uart->cache, 2,
                        pdMS_TO_TICKS(CH395_RX_WAIT_TIMEOUT_MS))
            < 0) {
        ESP_LOGE(TAG, "%s(%d): uart receive failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    *len = uart->cache[0] + ((uart->cache[1]) << 8);

    ch395_uart_unlock(uart);
    return ESP_OK;
}

static esp_err_t ch395_uart_get_phy_status(void *uart_ctx, uint8_t *sta)
{
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)uart_ctx;

    if (!ch395_uart_lock(uart)) {
        return ESP_ERR_TIMEOUT;
    }

    uart_flush(uart->port);
    if (uart_write_bytes(uart->port, CH395_UART_HEAD_BUFF, 2) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &CH395_CMD_GETPHY, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_read_bytes(uart->port, sta, 1,
                        pdMS_TO_TICKS(CH395_RX_WAIT_TIMEOUT_MS))
            < 0) {
        ESP_LOGE(TAG, "%s(%d): uart receive failed", __FUNCTION__,
                 __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    ch395_uart_unlock(uart);
    return ESP_OK;
}

static esp_err_t ch395_uart_set_phy_mode(void *uart_ctx, uint8_t mode)
{
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)uart_ctx;

    if (!ch395_uart_lock(uart)) {
        return ESP_ERR_TIMEOUT;
    }

    uart_flush(uart->port);
    if (uart_write_bytes(uart->port, CH395_UART_HEAD_BUFF, 2) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &CH395_CMD_SETPHY, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    if (uart_write_bytes(uart->port, &mode, 1) < 0) {
        ESP_LOGE(TAG, "%s(%d): uart transmit failed", __FUNCTION__, __LINE__);
        ch395_uart_unlock(uart);
        return ESP_FAIL;
    }

    ch395_uart_unlock(uart);
    return ESP_OK;
}



static esp_err_t ch395_cmd_pending_finish(emac_ch395_t *emac)
{
    uint8_t wait_cnt = 10;
    uint8_t status;

    while (wait_cnt--) {
        if (emac->drv.io(emac->drv.ctx, CMD01_GET_CMD_STATUS, NULL, 0, &status, 1)
                != ESP_OK) {
            return ESP_FAIL;
        }

        switch (status) {
        case CH395_ERR_SUCCESS: return ESP_OK;

        case CH395_ERR_BUSY: continue;

        default: return ESP_ERR_INVALID_STATE;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if (wait_cnt == 0) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

static esp_err_t ch395_set_baudrate(emac_ch395_t *emac, uint32_t baudrate)
{
    esp_err_t ret = ESP_OK;
    uint8_t params[3];

    params[0] = (uint8_t)baudrate;
    params[1] = (uint8_t)(baudrate >> 8);
    params[2] = (uint8_t)(baudrate >> 16);

    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD31_SET_BAUDRATE, params, 3, NULL, 0), err,
        TAG, "self check failed");
    vTaskDelay(pdMS_TO_TICKS(100));
err:
    return ret;
}

static esp_err_t ch395_set_host_baudrate(emac_ch395_t *emac, uint32_t baudrate)
{
    esp_err_t ret = ESP_OK;
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)emac->drv.ctx;

    ESP_GOTO_ON_ERROR(uart_set_baudrate(uart->port, baudrate), err, TAG,
                      "uart set baudrate failed");
err:
    return ret;
}

static esp_err_t ch395_get_host_baudrate(emac_ch395_t *emac, uint32_t *baudrate)
{
    esp_err_t ret = ESP_OK;
    eth_uart_ctx_t *uart = (eth_uart_ctx_t *)emac->drv.ctx;

    ESP_GOTO_ON_ERROR(uart_get_baudrate(uart->port, baudrate), err, TAG,
                      "uart get baudrate failed");
err:
    return ret;
}

static esp_err_t ch395_self_check(emac_ch395_t *emac)
{
    esp_err_t ret = ESP_OK;

    uint8_t idata = 0xA5;
    uint8_t odata;
    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD11_CHECK_EXIST, &idata, 1, &odata, 1), err,
        TAG, "self check failed");

    if (odata == 0x5A) {
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;

err:
    return ret;
}


static esp_err_t ch395_strict_self_check(emac_ch395_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t idata = 0;
    uint8_t odata;

    ESP_LOGI(TAG, "communication self check started. this will take some time...");

    for (; idata < 0xFF; idata++) {
        ESP_GOTO_ON_ERROR(
            emac->drv.io(emac->drv.ctx, CMD11_CHECK_EXIST, &idata, 1, &odata, 1),
            err, TAG, "self check failed");
        ESP_GOTO_ON_FALSE(odata == (uint8_t)(~idata), ESP_ERR_NOT_FOUND, err,
                          TAG, "self check failed");
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    ESP_LOGI(TAG, "Communication self check passed!");
err:
    return ret;
}

static esp_err_t ch395_set_mac_addr(emac_ch395_t *emac)
{
    ESP_LOGE(TAG,
             "%s(%d): You are trying to setting the MAC address. Once it is "
             "overwritten, it can NOT be recovered FOREVER!!!"
             "Therefore, this operation is FORBIDDEN in the driver. If you "
             "really want to do that, please manually exec related"
             " code or using DEBUG395.exe(Provided in "
             "https://www.wch.cn/downloads/CH395EVT_ZIP.html ) instead",
             __FUNCTION__, __LINE__);
    return ESP_FAIL;
}

static esp_err_t ch395_get_mac_addr(emac_ch395_t *emac)
{
    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD06_GET_MAC_ADDR, NULL, 0, emac->addr, 6),
        err, TAG, "read MAC failed");

err:
    return ret;
}

static esp_err_t ch395_reset(emac_ch395_t *emac)
{
    esp_err_t ret = ESP_OK;
    /* software reset */
    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD00_RESET_ALL, NULL, 0, NULL, 0), err, TAG,
        "reset all failed");
    vTaskDelay(pdMS_TO_TICKS(60));

err:
    return ret;
}

static esp_err_t ch395_verify_id(emac_ch395_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t version = 0;

    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD01_GET_IC_VER, NULL, 0, &version, 1), err,
        TAG, "read chip version failed");
    if (version >> 6 == 0x01) {
        emac->version = version;
        ESP_LOGI(TAG, "ch395 version: %02X", version);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG,
                 "ch395 version mismatched, got %02X, bit 7, 6 should be 0, 1!",
                 version);
        return ESP_ERR_INVALID_VERSION;
    }

err:
    return ret;
}

static esp_err_t ch395_setup_default(emac_ch395_t *emac)
{
    esp_err_t ret = ESP_OK;
    uint8_t param = 0;

    /* Disable PING*/
    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD10_PING_ENABLE, &param, 1, NULL, 0), err,
        TAG, "disable PING failed");

    /* Disable PPPOE*/
    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD10_PPPOE_ENABLE, &param, 1, NULL, 0), err,
        TAG, "disable PPPOE failed");
    vTaskDelay(pdMS_TO_TICKS(20));

    /* Disable DHCP*/
    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD10_DHCP_ENABLE, &param, 1, NULL, 0), err,
        TAG, "disable DHCP failed");
    vTaskDelay(pdMS_TO_TICKS(20));

    /* Disable SEND_OK_INT(only valid for 0x46 and later version)*/
    if (emac->version >= 0x46) {
        uint8_t fun_params[4] = {SOCK_DISABLE_SEND_OK_INT, 0x00};
        ESP_GOTO_ON_ERROR(emac->drv.io(emac->drv.ctx, CMD40_SET_FUN_PARA,
                                              fun_params, 4, NULL, 0),
                          err, TAG, "disable Send OK INT failed");
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD0W_INIT_CH395, NULL, 0, NULL, 0), err, TAG,
        "init CH395 failed");
    vTaskDelay(pdMS_TO_TICKS(400));
    ESP_GOTO_ON_ERROR(ch395_cmd_pending_finish(emac), err, TAG,
                      "init CH395 timeout");

err:
    return ret;
}

static esp_err_t emac_ch395_start(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    uint8_t sock_num = 0;

    /* Enable MAC RAW mode for SOCK0*/
    uint8_t sock_params[2];
    sock_params[0] = 0;
    sock_params[1] = PROTO_TYPE_MAC_RAW;
    ESP_GOTO_ON_ERROR(emac->drv.io(emac->drv.ctx, CMD20_SET_PROTO_TYPE_SN,
                                          sock_params, 2, NULL, 0),
                      err, TAG, "setting MAC Raw mode failed");

    /* Enable MAC filter, no blocking broadcast and multicast */
    uint8_t macfil_params[9];
    memset(macfil_params, 0, 0);
    macfil_params[0] = MAC_FILT_SEND_ENABLE | MAC_FILT_RECV_ENABLE
                       | MAC_FILT_RECV_BORADPKT | MAC_FILT_RECV_MULTIPKT;
    ESP_GOTO_ON_ERROR(emac->drv.io(emac->drv.ctx, CMD90_SET_MAC_FILT,
                                          macfil_params, 9, NULL, 0),
                      err, TAG, "setting MAC Filter failed");
    /* open SOCK0 */
    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD1W_OPEN_SOCKET_SN, &sock_num, 1, NULL, 0),
        err, TAG, "issue OPEN command failed");
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_GOTO_ON_ERROR(ch395_cmd_pending_finish(emac), err, TAG,
                      "open SOCK 0 failed");
err:
    return ret;
}

static esp_err_t emac_ch395_stop(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    uint8_t sock_num = 0;

    /* close SOCK0 */
    ESP_GOTO_ON_ERROR(
        emac->drv.io(emac->drv.ctx, CMD1W_CLOSE_SOCKET_SN, &sock_num, 1, NULL, 0),
        err, TAG, "issue CLOSE command failed");
    ESP_GOTO_ON_ERROR(ch395_cmd_pending_finish(emac), err, TAG,
                      "close SOCK 0 failed");
err:
    return ret;
}

static esp_err_t emac_ch395_set_mediator(esp_eth_mac_t *mac,
        esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(eth, ESP_ERR_INVALID_ARG, err, TAG,
                      "can't set mac's mediator to null");
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    emac->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_ch395_write_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr,
        uint32_t phy_reg,
        uint32_t reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);

    ESP_GOTO_ON_FALSE(phy_reg == CMD10_SET_PHY, ESP_ERR_INVALID_ARG, err, TAG,
                      "wrong PHY CMD");

    ESP_GOTO_ON_ERROR(emac->drv.set_phy(emac->drv.ctx, (uint8_t)reg_value), err, TAG,
                      "setting up PHY status failed");
err:
    return ret;
}

static esp_err_t emac_ch395_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr,
        uint32_t phy_reg,
        uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(reg_value, ESP_ERR_INVALID_ARG, err, TAG,
                      "can't set reg_value to null");
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);

    ESP_GOTO_ON_FALSE(phy_reg == CMD01_GET_PHY_STATUS, ESP_ERR_INVALID_ARG, err,
                      TAG, "wrong PHY CMD");

    ESP_GOTO_ON_ERROR(emac->drv.get_phy(emac->drv.ctx, (uint8_t *)reg_value), err,
                      TAG, "read PHY status failed");
err:
    return ret;
}

static esp_err_t emac_ch395_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    memcpy(emac->addr, addr, 6);
    ESP_GOTO_ON_ERROR(ch395_set_mac_addr(emac), err, TAG,
                      "set mac address failed");

err:
    return ret;
}

static esp_err_t emac_ch395_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(addr, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    memcpy(addr, emac->addr, 6);

err:
    return ret;
}

static esp_err_t emac_ch395_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);

    switch (link) {
    case ETH_LINK_UP:
        ESP_LOGD(TAG, "link is up");
        ESP_GOTO_ON_ERROR(mac->start(mac), err, TAG, "ch395 start failed");
        if (emac->poll_timer) {
            ESP_GOTO_ON_ERROR(
                esp_timer_start_periodic(emac->poll_timer,
                                         emac->poll_period_ms * 1000),
                err, TAG, "start poll timer failed");
        }
        break;
    case ETH_LINK_DOWN:
        ESP_LOGD(TAG, "link is down");
        ESP_GOTO_ON_ERROR(mac->stop(mac), err, TAG, "ch395 stop failed");
        if (emac->poll_timer) {
            ESP_GOTO_ON_ERROR(esp_timer_stop(emac->poll_timer), err, TAG,
                              "stop poll timer failed");
        }
        break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG,
                          "unknown link status");
        break;
    }

err:
    return ret;
}

static esp_err_t emac_ch395_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;

    switch (speed) {
    case ETH_SPEED_10M: ESP_LOGD(TAG, "working in 10Mbps"); break;
    case ETH_SPEED_100M: ESP_LOGD(TAG, "working in 100Mbps"); break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG,
                          "unknown speed");
        break;
    }

err:
    return ret;
}

static esp_err_t emac_ch395_set_duplex(esp_eth_mac_t *mac,
                                       eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;

    switch (duplex) {
    case ETH_DUPLEX_HALF: ESP_LOGD(TAG, "working in half duplex"); break;
    case ETH_DUPLEX_FULL: ESP_LOGD(TAG, "working in full duplex"); break;
    default:
        ESP_GOTO_ON_FALSE(false, ESP_ERR_INVALID_ARG, err, TAG,
                          "unknown duplex");
        break;
    }

err:
    return ret;
}

static esp_err_t emac_ch395_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    /* ch395 doesn't support promiscuous function, so accept any value */
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t emac_ch395_enable_flow_ctrl(esp_eth_mac_t *mac, bool enable)
{
    /* ch395 doesn't support flow control function, so accept any value */
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t emac_ch395_set_peer_pause_ability(esp_eth_mac_t *mac,
        uint32_t ability)
{
    /* ch395 doesn't suppport PAUSE function, so accept any value */
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t emac_ch395_flush_recv_frame(emac_ch395_t *emac)
{
    esp_err_t ret = ESP_OK;

    uint8_t sock_num = 0;
    ESP_GOTO_ON_ERROR(emac->drv.io(emac->drv.ctx, CMD10_CLEAR_RECV_BUF_SN,
                                          &sock_num, 1, NULL, 1),
                      err, TAG, "flush receive buffer failed");
err:
    return ret;
}

static esp_err_t emac_ch395_transmit(esp_eth_mac_t *mac, uint8_t *buf,
                                     uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);

    ESP_GOTO_ON_FALSE(length <= ETH_MAX_PACKET_SIZE, ESP_ERR_INVALID_ARG, err,
                      TAG, "frame size is too big (actual %lu, maximum %lu)",
                      length, (uint32_t)ETH_MAX_PACKET_SIZE);

    ESP_GOTO_ON_FALSE(
        xSemaphoreTake(emac->tx_busy_lock,
                       pdMS_TO_TICKS(CH395_MAC_TX_WAIT_TIMEOUT_MS))
        == pdTRUE,
        ESP_ERR_TIMEOUT, err, TAG, "waiting last transmit time out");

    ESP_GOTO_ON_ERROR(emac->drv.tx_buffer(emac->drv.ctx, buf, length), err, TAG,
                      "write frame failed");

err:
    return ret;
}

static esp_err_t emac_ch395_receive(esp_eth_mac_t *mac, uint8_t *buf,
                                    uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);

    ESP_GOTO_ON_ERROR(emac->drv.get_rx_size(emac->drv.ctx, length), err, TAG,
                      "read receive length failed");

    ESP_GOTO_ON_FALSE(*length <= ETH_MAX_PACKET_SIZE - ETH_CRC_LEN
                      && *length >= ETH_MIN_PACKET_SIZE - ETH_CRC_LEN,
                      ESP_ERR_INVALID_SIZE, err, TAG,
                      "received invalid frame, length(%lu)", *length);

    ESP_GOTO_ON_ERROR(emac->drv.rx_buffer(emac->drv.ctx, buf, *length), err, TAG,
                      "receive frame failed");
    return ret;

err:
    *length = 0;
    emac_ch395_flush_recv_frame(emac);
    return ret;
}

IRAM_ATTR static void ch395_isr_handler(void *arg)
{
    emac_ch395_t *emac = (emac_ch395_t *)arg;
    BaseType_t high_task_wakeup = pdFALSE;

    /* notify ch395 task */
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup != pdFALSE) {
        portYIELD_FROM_ISR();
    }
}

static void ch395_poll_timer(void *arg)
{
    emac_ch395_t *emac = (emac_ch395_t *)arg;
    xTaskNotifyGive(emac->rx_task_hdl);
}

static void emac_ch395_task(void *arg)
{
    emac_ch395_t *emac = (emac_ch395_t *)arg;
    uint8_t sock_num = 0;
    uint8_t g_intr = 0;
    uint8_t s_intr = 0;

    uint8_t *cache = NULL;

    while (1) {
        /* check if the task receives any notification */
        if (emac->int_gpio_num >= 0) { // if in interrupt mode
            if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)) == 0
                    && // if no notification ...
                    gpio_get_level(emac->int_gpio_num)
                    != 0) { // ...and no interrupt asserted
                continue;   // -> just continue to check again
            }
        } else {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        /* read interrupt status */
        emac->drv.io(emac->drv.ctx, CMD01_GET_GLOB_INT_STATUS, NULL, 0, &g_intr, 1);
        if (!(g_intr & GINT_STAT_SOCK0)) {
            continue;
        }

        emac->drv.io(emac->drv.ctx, CMD11_GET_INT_STATUS_SN, &sock_num, 1, &s_intr, 1);
        if (s_intr & (SINT_STAT_SENBUF_FREE)) {
            xSemaphoreGive(emac->tx_busy_lock);
        }

        if (s_intr & SINT_STAT_RECV) {
            if (emac->parent.receive(&emac->parent, emac->rx_buffer,
                                     &emac->rx_len) == ESP_OK) {
                ESP_LOGD(TAG, "receive len=%lu", emac->rx_len);

                /* allocate memory and check whether it failed */
                cache = malloc(emac->rx_len);
                if(cache == NULL){
                    ESP_LOGE(TAG, "no memory for receive buffer");
                    continue;
                }
                memcpy(cache, emac->rx_buffer, emac->rx_len);

                emac->eth->stack_input(emac->eth, cache, emac->rx_len);
            } else {
                ESP_LOGE(TAG, "frame read from module failed");
            }
        }
    }
    vTaskDelete(NULL);
}

static esp_err_t emac_ch395_spi_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    esp_eth_mediator_t *eth = emac->eth;

    if (emac->int_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(emac->int_gpio_num);
        gpio_set_direction(emac->int_gpio_num, GPIO_MODE_INPUT);
        gpio_set_pull_mode(emac->int_gpio_num, GPIO_PULLUP_ONLY);
        gpio_set_intr_type(emac->int_gpio_num, GPIO_INTR_NEGEDGE); // active low
        gpio_intr_enable(emac->int_gpio_num);
        gpio_isr_handler_add(emac->int_gpio_num, ch395_isr_handler, emac);
    }
    ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL), err,
                      TAG, "lowlevel init failed");

    vTaskDelay(pdMS_TO_TICKS(CH395_POWER_ON_DELAY_MS));

    ESP_GOTO_ON_ERROR(ch395_strict_self_check(emac), err, TAG,
                    "cannot pass self check");
    /* reset ch395 */
    ESP_GOTO_ON_ERROR(ch395_reset(emac), err, TAG, "reset ch395 failed");

    /* verify chip id */
    ESP_GOTO_ON_ERROR(ch395_verify_id(emac), err, TAG, "vefiry chip ID failed");
    /* get emac address */
    ESP_GOTO_ON_ERROR(ch395_get_mac_addr(emac), err, TAG,
                      "fetch ethernet mac address failed");
    /* default setup of internal registers */
    ESP_GOTO_ON_ERROR(ch395_setup_default(emac), err, TAG,
                      "ch395 default setup failed");

    return ESP_OK;
err:
    if (emac->int_gpio_num >= 0) {
        gpio_isr_handler_remove(emac->int_gpio_num);
        gpio_reset_pin(emac->int_gpio_num);
    }
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;

}

static esp_err_t emac_ch395_uart_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    esp_eth_mediator_t *eth = emac->eth;

    if (emac->int_gpio_num >= 0) {
        esp_rom_gpio_pad_select_gpio(emac->int_gpio_num);
        gpio_set_direction(emac->int_gpio_num, GPIO_MODE_INPUT);
        gpio_set_pull_mode(emac->int_gpio_num, GPIO_PULLUP_ONLY);
        gpio_set_intr_type(emac->int_gpio_num, GPIO_INTR_NEGEDGE); // active low
        gpio_intr_enable(emac->int_gpio_num);
        gpio_isr_handler_add(emac->int_gpio_num, ch395_isr_handler, emac);
    }
    ESP_GOTO_ON_ERROR(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL), err,
                      TAG, "lowlevel init failed");

    vTaskDelay(pdMS_TO_TICKS(CH395_POWER_ON_DELAY_MS));

    if (ch395_self_check(emac) != ESP_OK) {
        uint32_t baudrate;
        ch395_get_host_baudrate(emac, &baudrate);

        ESP_LOGW(
            TAG,
            "cannot communicate using current baudrate(%lu bps). this may happen when the chip cold boots. "
            "trying initial baudrate(9600 bps)...",
            baudrate);
        vTaskDelay(pdMS_TO_TICKS(50));

        ch395_set_host_baudrate(emac, CH395_UART_DEFAULT_BAUDRATE);
        ESP_GOTO_ON_ERROR(ch395_self_check(emac), err, TAG,
                          "cannot communicate with CH395");
        ESP_LOGI(
            TAG,
            "successfully communicate with CH395, now changing baudrate to the setting...");

        ch395_set_baudrate(emac, baudrate);
        ch395_set_host_baudrate(emac, baudrate);
        ESP_GOTO_ON_ERROR(
            ch395_self_check(emac), err, TAG,
            "cannot communicate with CH395 using setting baudrate(%lu bps)",
            baudrate);
    }

    /* reset ch395 */
    ESP_GOTO_ON_ERROR(ch395_reset(emac), err, TAG, "reset ch395 failed");

    uint32_t baudrate;
    ch395_get_host_baudrate(emac, &baudrate);
    ch395_set_host_baudrate(emac, CH395_UART_DEFAULT_BAUDRATE);
    ESP_GOTO_ON_ERROR(ch395_self_check(emac), err, TAG,
                      "cannot communicate with CH395");
    ch395_set_baudrate(emac, baudrate);
    ch395_set_host_baudrate(emac, baudrate);
    ESP_GOTO_ON_ERROR(
        ch395_strict_self_check(emac), err, TAG,
        "cannot communicate with CH395 using setting baudrate(%lu bps)",
        baudrate);

    /* verify chip id */
    ESP_GOTO_ON_ERROR(ch395_verify_id(emac), err, TAG, "vefiry chip ID failed");
    /* get emac address */
    ESP_GOTO_ON_ERROR(ch395_get_mac_addr(emac), err, TAG,
                      "fetch ethernet mac address failed");
    /* default setup of internal registers */
    ESP_GOTO_ON_ERROR(ch395_setup_default(emac), err, TAG,
                      "ch395 default setup failed");

    return ESP_OK;
err:
    if (emac->int_gpio_num >= 0) {
        gpio_isr_handler_remove(emac->int_gpio_num);
        gpio_reset_pin(emac->int_gpio_num);
    }
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    return ret;
}

static esp_err_t emac_ch395_deinit(esp_eth_mac_t *mac)
{
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
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

static esp_err_t emac_ch395_spi_del(esp_eth_mac_t *mac)
{
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    if (emac->poll_timer) {
        esp_timer_delete(emac->poll_timer);
    }
    vTaskDelete(emac->rx_task_hdl);
    ch395_spi_deinit(emac->drv.ctx);
    heap_caps_free(emac->rx_buffer);
    free(emac);
    return ESP_OK;
}

static esp_err_t emac_ch395_uart_del(esp_eth_mac_t *mac)
{
    emac_ch395_t *emac = __containerof(mac, emac_ch395_t, parent);
    if (emac->poll_timer) {
        esp_timer_delete(emac->poll_timer);
    }
    vTaskDelete(emac->rx_task_hdl);
    ch395_uart_deinit(emac->drv.ctx);
    heap_caps_free(emac->rx_buffer);
    free(emac);
    return ESP_OK;
}

esp_eth_mac_t *esp_eth_mac_new_ch395(const eth_ch395_config_t *ch395_config,
                                     const eth_mac_config_t *mac_config)
{
    esp_eth_mac_t *ret = NULL;
    emac_ch395_t *emac = NULL;
    ESP_GOTO_ON_FALSE(ch395_config && mac_config, NULL, err, TAG,
                      "invalid argument");
    emac = calloc(1, sizeof(emac_ch395_t));
    ESP_GOTO_ON_FALSE(emac, NULL, err, TAG, "no mem for MAC instance");

    ESP_GOTO_ON_FALSE((ch395_config->mode == ETH_CH395_MODE_SPI)
                          || (ch395_config->mode == ETH_CH395_MODE_UART),
                      NULL, err, TAG, "invalid mode");

    /* bind methods and attributes */
    emac->int_gpio_num = ch395_config->int_gpio_num;
    emac->poll_period_ms = ch395_config->poll_period_ms;
    emac->parent.set_mediator = emac_ch395_set_mediator;
    emac->parent.deinit = emac_ch395_deinit;
    emac->parent.start = emac_ch395_start;
    emac->parent.stop = emac_ch395_stop;
    emac->parent.write_phy_reg = emac_ch395_write_phy_reg;
    emac->parent.read_phy_reg = emac_ch395_read_phy_reg;
    emac->parent.set_addr = emac_ch395_set_addr;
    emac->parent.get_addr = emac_ch395_get_addr;
    emac->parent.set_speed = emac_ch395_set_speed;
    emac->parent.set_duplex = emac_ch395_set_duplex;
    emac->parent.set_link = emac_ch395_set_link;
    emac->parent.set_promiscuous = emac_ch395_set_promiscuous;
    emac->parent.set_peer_pause_ability = emac_ch395_set_peer_pause_ability;
    emac->parent.enable_flow_ctrl = emac_ch395_enable_flow_ctrl;
    emac->parent.transmit = emac_ch395_transmit;
    emac->parent.receive = emac_ch395_receive;

    if(ch395_config->mode == ETH_CH395_MODE_SPI){
        ESP_LOGD(TAG, "Using SPI Driver");
        emac->parent.init = emac_ch395_spi_init;
        emac->parent.del = emac_ch395_spi_del;

        emac->drv.init = ch395_spi_init;
        emac->drv.deinit = ch395_spi_deinit;
        emac->drv.io = ch395_spi_io;
        emac->drv.tx_buffer = ch395_spi_tx_buffer;
        emac->drv.rx_buffer = ch395_spi_rx_buffer;
        emac->drv.get_rx_size = ch395_spi_get_rx_size;
        emac->drv.set_phy = ch395_spi_set_phy_mode;
        emac->drv.get_phy = ch395_spi_get_phy_status;

        /* SPI device init */
        ESP_GOTO_ON_FALSE((emac->drv.ctx = emac->drv.init(ch395_config)) != NULL,
                        NULL, err, TAG, "SPI initialization failed");
    }
    else{
        ESP_LOGD(TAG, "Using UART Driver");
        emac->parent.init = emac_ch395_uart_init;
        emac->parent.del = emac_ch395_uart_del;

        emac->drv.init = ch395_uart_init;
        emac->drv.deinit = ch395_uart_deinit;
        emac->drv.io = ch395_uart_io;
        emac->drv.tx_buffer = ch395_uart_tx_buffer;
        emac->drv.rx_buffer = ch395_uart_rx_buffer;
        emac->drv.get_rx_size = ch395_uart_get_rx_size;
        emac->drv.set_phy = ch395_uart_set_phy_mode;
        emac->drv.get_phy = ch395_uart_get_phy_status;

        /* UART device init */
        ESP_GOTO_ON_FALSE((emac->drv.ctx = emac->drv.init(ch395_config)) != NULL,
                        NULL, err, TAG, "UART initialization failed");
    }

    /* create tx mutex */
    emac->tx_busy_lock = xSemaphoreCreateBinary();
    ESP_GOTO_ON_FALSE(emac->tx_busy_lock, NULL, err, TAG,
                      "create ch395 tx mutex failed");
    xSemaphoreGive(emac->tx_busy_lock);

    /* create ch395 task */
    BaseType_t core_num = tskNO_AFFINITY;
    if (mac_config->flags & ETH_MAC_FLAG_PIN_TO_CORE) {
        core_num = esp_cpu_get_core_id();
    }
    BaseType_t xReturned = xTaskCreatePinnedToCore(
                               emac_ch395_task, "ch395_tsk", mac_config->rx_task_stack_size, emac,
                               mac_config->rx_task_prio, &emac->rx_task_hdl, core_num);
    ESP_GOTO_ON_FALSE(xReturned == pdPASS, NULL, err, TAG,
                      "create ch395 task failed");

    emac->rx_buffer = heap_caps_malloc(ETH_MAX_PACKET_SIZE, MALLOC_CAP_DMA);
    ESP_GOTO_ON_FALSE(emac->rx_buffer, NULL, err, TAG,
                      "RX buffer allocation failed");

    if (emac->int_gpio_num < 0) {
        const esp_timer_create_args_t poll_timer_args = {
            .callback = ch395_poll_timer,
            .name = "emac_spi_poll_timer",
            .arg = emac,
            .skip_unhandled_events = true
        };
        ESP_GOTO_ON_FALSE(esp_timer_create(&poll_timer_args, &emac->poll_timer)
                          == ESP_OK,
                          NULL, err, TAG, "create poll timer failed");
    }

    return &(emac->parent);

err:
    if (emac) {
        if (emac->poll_timer) {
            esp_timer_delete(emac->poll_timer);
        }

        if (emac->rx_task_hdl) {
            vTaskDelete(emac->rx_task_hdl);
        }

        if (emac->drv.ctx) {
            emac->drv.deinit(emac->drv.ctx);
        }

        heap_caps_free(emac->rx_buffer);
        free(emac);
    }
    return ret;
}
