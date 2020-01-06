// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_netif.h"
#include "esp_modem.h"
#include "esp_log.h"

static const char *TAG = "esp-modem-netif";

/**
 * @brief ESP32 Modem handle to be used as netif IO object
 */
typedef struct esp_modem_netif_driver_s {
    esp_netif_driver_base_t base;           /*!< base structure reserved as esp-netif driver */
    modem_dte_t            *dte;        /*!< ptr to the esp_modem objects (DTE) */
} esp_modem_netif_driver_t;

/**
 * @brief Transmit function called from esp_netif to output network stack data
 *
 * Note: This API has to conform to esp-netif transmit prototype
 *
 * @param h Opaque pointer representing esp-netif driver, esp_dte in this case of esp_modem
 * @param data data buffer
 * @param length length of data to send
 *
 * @return ESP_OK on success
 */
static esp_err_t esp_modem_dte_transmit(void *h, void *buffer, size_t len)
{
    modem_dte_t *dte = h;
    if (dte->send_data(dte, (const char *)buffer, len) > 0) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

/**
 * @brief Post attach adapter for esp-modem
 *
 * Used to exchange internal callbacks, context between esp-netif nad modem-netif
 *
 * @param esp_netif handle to esp-netif object
 * @param args pointer to modem-netif driver
 *
 * @return ESP_OK on success
 */
static esp_err_t esp_modem_post_attach_start(esp_netif_t * esp_netif, void * args)
{
    esp_modem_netif_driver_t *driver = args;
    modem_dte_t *dte = driver->dte;
    const esp_netif_driver_ifconfig_t driver_ifconfig = {
            .driver_free_rx_buffer = NULL,
            .transmit = esp_modem_dte_transmit,
            .handle = dte
    };
    driver->base.netif = esp_netif;
    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
    esp_modem_start_ppp(dte);
    return ESP_OK;
}

/**
 * @brief Data path callback from esp-modem to pass data to esp-netif
 *
 * @param buffer data pointer
 * @param len data length
 * @param context context data used for esp-modem-netif handle
 *
 * @return ESP_OK on success
 */
static esp_err_t modem_netif_receive_cb(void *buffer, size_t len, void *context)
{
    esp_modem_netif_driver_t *driver = context;
    esp_netif_receive(driver->base.netif, buffer, len, NULL);
    return ESP_OK;
}

void *esp_modem_netif_setup(modem_dte_t *dte)
{
    esp_modem_netif_driver_t *driver =  calloc(1, sizeof(esp_modem_netif_driver_t));
    if (driver == NULL) {
        ESP_LOGE(TAG, "Cannot allocate esp_modem_netif_driver_t");
        goto drv_create_failed;
    }
    esp_err_t err = esp_modem_set_rx_cb(dte, modem_netif_receive_cb, driver);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_modem_set_rx_cb failed with: %d", err);
        goto drv_create_failed;
    }

    driver->base.post_attach = esp_modem_post_attach_start;
    driver->dte = dte;
    return driver;

drv_create_failed:
    return NULL;
}

void esp_modem_netif_teardown(void *h)
{
    esp_modem_netif_driver_t *driver = h;
    esp_netif_destroy(driver->base.netif);
    free(driver);
}

esp_err_t esp_modem_netif_clear_default_handlers(void *h)
{
    esp_modem_netif_driver_t *driver = h;
    esp_err_t ret;
    ret = esp_modem_remove_event_handler(driver->dte, esp_netif_action_start);
    if (ret != ESP_OK) {
        goto clear_event_failed;
    }
    ret = esp_modem_remove_event_handler(driver->dte, esp_netif_action_stop);
    if (ret != ESP_OK) {
        goto clear_event_failed;
    }
    return ESP_OK;

clear_event_failed:
    ESP_LOGE(TAG, "Failed to unregister event handlers");
    return ESP_FAIL;

}

esp_err_t esp_modem_netif_set_default_handlers(void *h, esp_netif_t * esp_netif)
{
    esp_modem_netif_driver_t *driver = h;
    esp_err_t ret;
    ret = esp_modem_set_event_handler(driver->dte, esp_netif_action_start, ESP_MODEM_EVENT_PPP_START, esp_netif);
    if (ret != ESP_OK) {
        goto set_event_failed;
    }
    ret = esp_modem_set_event_handler(driver->dte, esp_netif_action_stop, ESP_MODEM_EVENT_PPP_STOP, esp_netif);
    if (ret != ESP_OK) {
        goto set_event_failed;
    }
    return ESP_OK;

set_event_failed:
    ESP_LOGE(TAG, "Failed to register event handlers");
    esp_modem_netif_clear_default_handlers(driver);
    return ESP_FAIL;
}
