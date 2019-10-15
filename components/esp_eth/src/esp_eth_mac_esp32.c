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
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_eth.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "hal/emac.h"
#include "soc/soc.h"
#include "sdkconfig.h"

static const char *TAG = "emac_esp32";
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

#define PHY_OPERATION_TIMEOUT_US (1000)

typedef struct {
    esp_eth_mac_t parent;
    esp_eth_mediator_t *eth;
    emac_hal_context_t hal;
    intr_handle_t intr_hdl;
    TaskHandle_t rx_task_hdl;
    uint32_t sw_reset_timeout_ms;
    uint32_t frames_remain;
    uint8_t addr[6];
    uint8_t *rx_buf[CONFIG_ETH_DMA_RX_BUFFER_NUM];
    uint8_t *tx_buf[CONFIG_ETH_DMA_TX_BUFFER_NUM];
    bool isr_need_yield;
} emac_esp32_t;

static esp_err_t emac_esp32_set_mediator(esp_eth_mac_t *mac, esp_eth_mediator_t *eth)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(eth, "can't set mac's mediator to null", err, ESP_ERR_INVALID_ARG);
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac->eth = eth;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_write_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    MAC_CHECK(!emac_hal_is_mii_busy(&emac->hal), "phy is busy", err, ESP_ERR_INVALID_STATE);
    emac_hal_set_phy_data(&emac->hal, reg_value);
    emac_hal_set_phy_cmd(&emac->hal, phy_addr, phy_reg, true);
    /* polling the busy flag */
    uint32_t to = 0;
    bool busy = true;
    do {
        ets_delay_us(100);
        busy = emac_hal_is_mii_busy(&emac->hal);
        to += 100;
    } while (busy && to < PHY_OPERATION_TIMEOUT_US);
    MAC_CHECK(!busy, "phy is busy", err, ESP_ERR_TIMEOUT);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_read_phy_reg(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(reg_value, "can't set reg_value to null", err, ESP_ERR_INVALID_ARG);
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    MAC_CHECK(!emac_hal_is_mii_busy(&emac->hal), "phy is busy", err, ESP_ERR_INVALID_STATE);
    emac_hal_set_phy_cmd(&emac->hal, phy_addr, phy_reg, false);
    /* polling the busy flag */
    uint32_t to = 0;
    bool busy = true;
    do {
        ets_delay_us(100);
        busy = emac_hal_is_mii_busy(&emac->hal);
        to += 100;
    } while (busy && to < PHY_OPERATION_TIMEOUT_US);
    MAC_CHECK(!busy, "phy is busy", err, ESP_ERR_TIMEOUT);
    /* Store value */
    *reg_value = emac_hal_get_phy_data(&emac->hal);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_set_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "can't set mac addr to null", err, ESP_ERR_INVALID_ARG);
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    memcpy(emac->addr, addr, 6);
    emac_hal_set_address(&emac->hal, emac->addr);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_get_addr(esp_eth_mac_t *mac, uint8_t *addr)
{
    esp_err_t ret = ESP_OK;
    MAC_CHECK(addr, "can't set mac addr to null", err, ESP_ERR_INVALID_ARG);
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    memcpy(addr, emac->addr, 6);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_set_link(esp_eth_mac_t *mac, eth_link_t link)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    switch (link) {
    case ETH_LINK_UP:
        MAC_CHECK(esp_intr_enable(emac->intr_hdl) == ESP_OK, "enable interrupt failed", err, ESP_FAIL);
        emac_hal_start(&emac->hal);
        break;
    case ETH_LINK_DOWN:
        MAC_CHECK(esp_intr_disable(emac->intr_hdl) == ESP_OK, "disable interrupt failed", err, ESP_FAIL);
        emac_hal_stop(&emac->hal);
        break;
    default:
        MAC_CHECK(false, "unknown link status", err, ESP_ERR_INVALID_ARG);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_set_speed(esp_eth_mac_t *mac, eth_speed_t speed)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    switch (speed) {
    case ETH_SPEED_10M:
        emac_hal_set_speed(&emac->hal, EMAC_SPEED_10M);
        break;
    case ETH_SPEED_100M:
        emac_hal_set_speed(&emac->hal, EMAC_SPEED_100M);
        break;
    default:
        MAC_CHECK(false, "unknown speed", err, ESP_ERR_INVALID_ARG);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_set_duplex(esp_eth_mac_t *mac, eth_duplex_t duplex)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    switch (duplex) {
    case ETH_DUPLEX_HALF:
        emac_hal_set_duplex(&emac->hal, EMAC_DUPLEX_HALF);
        break;
    case ETH_DUPLEX_FULL:
        emac_hal_set_duplex(&emac->hal, EMAC_DUPLEX_FULL);
        break;
    default:
        MAC_CHECK(false, "unknown duplex", err, ESP_ERR_INVALID_ARG);
        break;
    }
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_set_promiscuous(esp_eth_mac_t *mac, bool enable)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    emac_hal_set_promiscuous(&emac->hal, enable);
    return ESP_OK;
}

static esp_err_t emac_esp32_transmit(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    MAC_CHECK(buf, "can't set buf to null", err, ESP_ERR_INVALID_ARG);
    MAC_CHECK(length, "buf length can't be zero", err, ESP_ERR_INVALID_ARG);
    /* Check if the descriptor is owned by the Ethernet DMA (when 1) or CPU (when 0) */
    MAC_CHECK(emac_hal_get_tx_desc_owner(&emac->hal) == EMAC_DMADESC_OWNER_CPU,
              "CPU doesn't own the Tx Descriptor", err, ESP_ERR_INVALID_STATE);
    emac_hal_transmit_frame(&emac->hal, buf, length);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t emac_esp32_receive(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    MAC_CHECK(buf && length, "can't set buf and length to null", err, ESP_ERR_INVALID_ARG);
    uint32_t receive_len = emac_hal_receive_frame(&emac->hal, buf, *length, &emac->frames_remain);
    /* we need to check the return value in case the buffer size is not enough */
    if (*length < receive_len) {
        ESP_LOGE(TAG, "buffer size too small");
        /* tell upper layer the size we need */
        *length = receive_len;
        ret = ESP_ERR_INVALID_SIZE;
        goto err;
    }
    *length = receive_len;
    return ESP_OK;
err:
    return ret;
}

static void emac_esp32_rx_task(void *arg)
{
    emac_esp32_t *emac = (emac_esp32_t *)arg;
    uint8_t *buffer = NULL;
    uint32_t length = 0;
    while (1) {
        // block indefinitely until some task notifies me
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        do {
            length = ETH_MAX_PACKET_SIZE;
            buffer = (uint8_t *)malloc(length);
            if (emac_esp32_receive(&emac->parent, buffer, &length) == ESP_OK) {
                /* pass the buffer to stack (e.g. TCP/IP layer) */
                if (length) {
                    emac->eth->stack_input(emac->eth, buffer, length);
                } else {
                    free(buffer);
                }
            } else {
                free(buffer);
            }
        } while (emac->frames_remain);
    }
    vTaskDelete(NULL);
}

static void emac_esp32_init_smi_gpio(void)
{
    /* Setup SMI MDC GPIO */
    gpio_set_direction(CONFIG_ETH_SMI_MDC_GPIO, GPIO_MODE_OUTPUT);
    gpio_matrix_out(CONFIG_ETH_SMI_MDC_GPIO, EMAC_MDC_O_IDX, false, false);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[CONFIG_ETH_SMI_MDC_GPIO], PIN_FUNC_GPIO);
    /* Setup SMI MDIO GPIO */
    gpio_set_direction(CONFIG_ETH_SMI_MDIO_GPIO, GPIO_MODE_INPUT_OUTPUT);
    gpio_matrix_out(CONFIG_ETH_SMI_MDIO_GPIO, EMAC_MDO_O_IDX, false, false);
    gpio_matrix_in(CONFIG_ETH_SMI_MDIO_GPIO, EMAC_MDI_I_IDX, false);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[CONFIG_ETH_SMI_MDIO_GPIO], PIN_FUNC_GPIO);
}

static esp_err_t emac_esp32_init(esp_eth_mac_t *mac)
{
    esp_err_t ret = ESP_OK;
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
    /* enable peripheral clock */
    periph_module_enable(PERIPH_EMAC_MODULE);
    /* enable clock, config gpio, etc */
    emac_hal_lowlevel_init(&emac->hal);
    /* init gpio used by gpio */
    emac_esp32_init_smi_gpio();
#if CONFIG_ETH_PHY_USE_RST
    gpio_pad_select_gpio(CONFIG_ETH_PHY_RST_GPIO);
    gpio_set_direction(CONFIG_ETH_PHY_RST_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(CONFIG_ETH_PHY_RST_GPIO, 1);
#endif
    MAC_CHECK(eth->on_state_changed(eth, ETH_STATE_LLINIT, NULL) == ESP_OK, "lowlevel init failed", err, ESP_FAIL);
    /* software reset */
    emac_hal_reset(&emac->hal);
    uint32_t to = 0;
    for (to = 0; to < emac->sw_reset_timeout_ms / 10; to++) {
        if (emac_hal_is_reset_done(&emac->hal)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    MAC_CHECK(to < emac->sw_reset_timeout_ms / 10, "reset timeout", err, ESP_ERR_TIMEOUT);
    /* set smi clock */
    emac_hal_set_csr_clock_range(&emac->hal);
    /* reset descriptor chain */
    emac_hal_reset_desc_chain(&emac->hal);
    /* init mac registers by default */
    emac_hal_init_mac_default(&emac->hal);
    /* init dma registers by default */
    emac_hal_init_dma_default(&emac->hal);
    /* get emac address from efuse */
    MAC_CHECK(esp_read_mac(emac->addr, ESP_MAC_ETH) == ESP_OK, "fetch ethernet mac address failed", err, ESP_FAIL);
    /* set MAC address to emac register */
    emac_hal_set_address(&emac->hal, emac->addr);
    return ESP_OK;
err:
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    periph_module_disable(PERIPH_EMAC_MODULE);
    return ret;
}

static esp_err_t emac_esp32_deinit(esp_eth_mac_t *mac)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    esp_eth_mediator_t *eth = emac->eth;
#if CONFIG_ETH_PHY_USE_RST
    gpio_set_level(CONFIG_ETH_PHY_RST_GPIO, 0);
#endif
    emac_hal_stop(&emac->hal);
    eth->on_state_changed(eth, ETH_STATE_DEINIT, NULL);
    periph_module_disable(PERIPH_EMAC_MODULE);
    return ESP_OK;
}

static esp_err_t emac_esp32_del(esp_eth_mac_t *mac)
{
    emac_esp32_t *emac = __containerof(mac, emac_esp32_t, parent);
    esp_intr_free(emac->intr_hdl);
    vTaskDelete(emac->rx_task_hdl);
    int i = 0;
    for (i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++) {
        free(emac->hal.rx_buf[i]);
    }
    for (i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
        free(emac->hal.tx_buf[i]);
    }
    free(emac->hal.descriptors);
    free(emac);
    return ESP_OK;
}

void emac_esp32_isr_handler(void *args)
{
    emac_hal_context_t *hal = (emac_hal_context_t *)args;
    emac_esp32_t *emac = __containerof(hal, emac_esp32_t, hal);
    emac_hal_isr(args);
    if (emac->isr_need_yield) {
        emac->isr_need_yield = false;
        portYIELD_FROM_ISR();
    }
}

esp_eth_mac_t *esp_eth_mac_new_esp32(const eth_mac_config_t *config)
{
    esp_eth_mac_t *ret = NULL;
    void *descriptors = NULL;
    emac_esp32_t *emac = NULL;
    MAC_CHECK(config, "can't set mac config to null", err, NULL);
    emac = calloc(1, sizeof(emac_esp32_t));
    MAC_CHECK(emac, "calloc emac failed", err, NULL);
    /* alloc memory for ethernet dma descriptor */
    uint32_t desc_size = CONFIG_ETH_DMA_RX_BUFFER_NUM * sizeof(eth_dma_rx_descriptor_t) +
                         CONFIG_ETH_DMA_TX_BUFFER_NUM * sizeof(eth_dma_tx_descriptor_t);
    descriptors = heap_caps_calloc(1, desc_size, MALLOC_CAP_DMA);
    MAC_CHECK(descriptors, "calloc descriptors failed", err, NULL);
    int i = 0;
    /* alloc memory for ethernet dma buffer */
    for (i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++) {
        emac->rx_buf[i] = heap_caps_calloc(1, CONFIG_ETH_DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
        if (!(emac->rx_buf[i])) {
            goto err;
        }
    }
    for (i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
        emac->tx_buf[i] = heap_caps_calloc(1, CONFIG_ETH_DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
        if (!(emac->tx_buf[i])) {
            goto err;
        }
    }
    /* initialize hal layer driver */
    emac_hal_init(&emac->hal, descriptors, emac->rx_buf, emac->tx_buf);
    emac->sw_reset_timeout_ms = config->sw_reset_timeout_ms;
    emac->parent.set_mediator = emac_esp32_set_mediator;
    emac->parent.init = emac_esp32_init;
    emac->parent.deinit = emac_esp32_deinit;
    emac->parent.del = emac_esp32_del;
    emac->parent.write_phy_reg = emac_esp32_write_phy_reg;
    emac->parent.read_phy_reg = emac_esp32_read_phy_reg;
    emac->parent.set_addr = emac_esp32_set_addr;
    emac->parent.get_addr = emac_esp32_get_addr;
    emac->parent.set_speed = emac_esp32_set_speed;
    emac->parent.set_duplex = emac_esp32_set_duplex;
    emac->parent.set_link = emac_esp32_set_link;
    emac->parent.set_promiscuous = emac_esp32_set_promiscuous;
    emac->parent.transmit = emac_esp32_transmit;
    emac->parent.receive = emac_esp32_receive;
    /* Interrupt configuration */
    MAC_CHECK(esp_intr_alloc(ETS_ETH_MAC_INTR_SOURCE, ESP_INTR_FLAG_IRAM, emac_esp32_isr_handler,
                             &emac->hal, &(emac->intr_hdl)) == ESP_OK,
              "alloc emac interrupt failed", err, NULL);
    /* create rx task */
    BaseType_t xReturned = xTaskCreate(emac_esp32_rx_task, "emac_rx", config->rx_task_stack_size, emac,
                                       config->rx_task_prio, &emac->rx_task_hdl);
    MAC_CHECK(xReturned == pdPASS, "create emac_rx task failed", err, NULL);
    return &(emac->parent);

err:
    if (emac) {
        if (emac->rx_task_hdl) {
            vTaskDelete(emac->rx_task_hdl);
        }
        if (emac->intr_hdl) {
            esp_intr_free(emac->intr_hdl);
        }
        for (int i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++) {
            free(emac->tx_buf[i]);
        }
        for (int i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++) {
            free(emac->rx_buf[i]);
        }
        free(emac);
    }
    if (descriptors) {
        free(descriptors);
    }
    return ret;
}

void emac_hal_rx_complete_cb(void *arg)
{
    emac_hal_context_t *hal = (emac_hal_context_t *)arg;
    emac_esp32_t *emac = __containerof(hal, emac_esp32_t, hal);
    BaseType_t high_task_wakeup;
    /* notify receive task */
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup == pdTRUE) {
        emac->isr_need_yield = true;
    }
}

void emac_hal_rx_unavail_cb(void *arg)
{
    emac_hal_context_t *hal = (emac_hal_context_t *)arg;
    emac_esp32_t *emac = __containerof(hal, emac_esp32_t, hal);
    BaseType_t high_task_wakeup;
    /* notify receive task */
    vTaskNotifyGiveFromISR(emac->rx_task_hdl, &high_task_wakeup);
    if (high_task_wakeup == pdTRUE) {
        emac->isr_need_yield = true;
    }
}
