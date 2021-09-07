/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Peter Lawrence
 * Modifications Copyright © 2021 Ci4Rail GmbH
 *
 * influenced by lrndis https://github.com/fetisov/lrndis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "esp_cdc_ecm_driver.h"
#include "class/net/net_device.h"
#include "esp_log.h"
#include "esp_event.h"

const static char *TAG = "CDC-ECM Driver";

typedef struct {
    uint8_t* payload;
    uint32_t length;
}rx_frame_t;

#define FRAME_BUFFER_SIZE  5

struct frame_buffer_t {
    uint8_t read_idx;
    uint8_t write_idx;
    rx_frame_t frame[FRAME_BUFFER_SIZE];
};

/* shared between tud_network_recv_cb() and service_traffic() */
static struct frame_buffer_t rx_frame_buffer = {
    .read_idx = 0,
    .write_idx = 0,
    .frame = {}
};

/* semaphore as receive event, is intended to notify rx_task thread in case a new frame was received */
static SemaphoreHandle_t rx_event = NULL;
/* dummy, not really used. Only required for the usage of RNDIS protocol, but we are using CDC-ECM. Is only declared, that there will be no compile error. */
const uint8_t tud_network_mac_address[6];

esp_err_t esp_cdc_ecm_receive(uint8_t* buffer, uint32_t length, esp_netif_t* esp_netif)
{
    return esp_netif_receive(esp_netif, buffer, length, NULL);
}

static void rx_task(void *arg)
{
    esp_netif_t *netif = (esp_netif_t *)arg;
    /* handle any packet received by tud_network_recv_cb() */
    while(rx_frame_buffer.frame[rx_frame_buffer.read_idx].payload) {
        esp_cdc_ecm_receive(rx_frame_buffer.frame[rx_frame_buffer.read_idx].payload,
                            rx_frame_buffer.frame[rx_frame_buffer.read_idx].length,
                            netif);
        
        rx_frame_buffer.frame[rx_frame_buffer.read_idx].payload = NULL;
        rx_frame_buffer.read_idx++;
        if(rx_frame_buffer.read_idx >= FRAME_BUFFER_SIZE) {
            rx_frame_buffer.read_idx = 0;
        }
    }
}


static void esp_cdc_ecm_rx_task(void *arg)
{
    while(1) {
        /* do only service traffic if event was received */
        if(xSemaphoreTake(rx_event, portMAX_DELAY)) {
            rx_task(arg);
        }
    }
}

void esp_cdc_ecm_free_rx_buffer(void *h, void* buffer)
{
    if(buffer != NULL) {
        free(buffer);
    }
}

void tud_network_init_cb(void)
{
    /* if the network is re-initializing and we have a leftover packet, we must do a cleanup */
    for (uint8_t idx = 0; idx < FRAME_BUFFER_SIZE; idx++) {
        if (rx_frame_buffer.frame[idx].payload != NULL) {
            free(rx_frame_buffer.frame[idx].payload);
            rx_frame_buffer.frame[idx].payload = NULL;
        }
    }

    rx_frame_buffer.write_idx = 0;
    rx_frame_buffer.read_idx = 0;
}

esp_err_t esp_cdc_ecm_driver_install(esp_cdc_ecm_driver_t** return_handle)
{
    esp_cdc_ecm_driver_t* handle = calloc(1, sizeof(esp_cdc_ecm_driver_t));
    if(handle == NULL) {
        ESP_LOGE(TAG, "Could not allocate handle.");
        return ESP_FAIL;
    }

    /* create receive event */
    rx_event = xSemaphoreCreateBinary();
    if(rx_event == NULL) {
        ESP_LOGE(TAG, "Could not create semaphore.");
        return ESP_FAIL;
    }

    *return_handle = handle;

    return ESP_OK;
}

esp_err_t esp_cdc_ecm_transmit(void* h, void* buffer, size_t len)
{
    for (;;) {
        /* if TinyUSB isn't ready, we must signal back to lwip that there is nothing we can do */
        if (!tud_ready()) {
            return ESP_FAIL;
        }

        /* if the network driver can accept another packet, we make it happen */
        if (tud_network_can_xmit()) {
            /* pass length for tud_network_xmit_cb */
            tud_network_xmit(buffer, (uint16_t) len);
            return ESP_OK;
        }

        /* transfer execution to TinyUSB in the hopes that it will finish transmitting the prior packet */
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

esp_err_t esp_cdc_ecm_driver_start(esp_cdc_ecm_driver_t* handle, esp_netif_t* netif)
{
    xTaskCreate(esp_cdc_ecm_rx_task, "cdc-ecm_rx_task", 4096, netif, CONFIG_TINYUSB_CDC_ECM_RXTASK_PRIO, NULL);
    esp_event_post(ETH_EVENT, ETHERNET_EVENT_START, &handle, sizeof(esp_cdc_ecm_driver_t*), 0);
    esp_event_post(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &handle, sizeof(esp_cdc_ecm_driver_t*), 0);
    return ESP_OK;
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
    /* check if buffer run full */
    if (rx_frame_buffer.frame[rx_frame_buffer.write_idx].payload != NULL) {
        /* signal the inability to accept it */
        ESP_LOGW(TAG, "Not able to accept packet! - Buffer full!");
        return false;
    }

    if (size) {
        rx_frame_buffer.frame[rx_frame_buffer.write_idx].payload = malloc(size * sizeof(uint8_t));
        rx_frame_buffer.frame[rx_frame_buffer.write_idx].length = size;

        if (rx_frame_buffer.frame[rx_frame_buffer.write_idx].payload) {
            memcpy(rx_frame_buffer.frame[rx_frame_buffer.write_idx].payload, src, size);

            rx_frame_buffer.write_idx++;
            if (rx_frame_buffer.write_idx >= FRAME_BUFFER_SIZE) {
                rx_frame_buffer.write_idx = 0;
            }

            tud_network_recv_renew();

            /* new frame received -> set rx event */
            xSemaphoreGive(rx_event);
        }
        else {
            ESP_LOGW(TAG, "Not able to accept packet! - Could not allocate memory!");
            return false;
        }
    }

    return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
    uint16_t length = arg;

    memcpy(dst, ref, length);

    return length;
}

