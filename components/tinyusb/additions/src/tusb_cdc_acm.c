/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "tusb.h"
#include "tusb_cdc_acm.h"
#include "cdc.h"
#include "sdkconfig.h"

#define RX_UNREADBUF_SZ_DEFAULT 64 // buffer storing all unread RX data
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


typedef struct {
    bool initialized;
    size_t rx_unread_buf_sz;
    RingbufHandle_t rx_unread_buf;
    SemaphoreHandle_t ringbuf_read_mux;
    uint8_t *rx_tfbuf;
    tusb_cdcacm_callback_t callback_rx;
    tusb_cdcacm_callback_t callback_rx_wanted_char;
    tusb_cdcacm_callback_t callback_line_state_changed;
    tusb_cdcacm_callback_t callback_line_coding_changed;
} esp_tusb_cdcacm_t; /*!< CDC_ACM object */

static const char *TAG = "tusb_cdc_acm";

static inline esp_tusb_cdcacm_t *get_acm(tinyusb_cdcacm_itf_t itf)
{
    esp_tusb_cdc_t *cdc_inst = tinyusb_cdc_get_intf(itf);
    if (cdc_inst == NULL) {
        return (esp_tusb_cdcacm_t *)NULL;
    }
    return (esp_tusb_cdcacm_t *)(cdc_inst->subclass_obj);
}


/* TinyUSB callbacks
   ********************************************************************* */

/* Invoked by cdc interface when line state changed e.g connected/disconnected */
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    esp_tusb_cdcacm_t *acm = get_acm(itf);
    if (dtr && rts) { // connected
        if (acm != NULL) {
            ESP_LOGV(TAG, "Host connected to CDC no.%d.", itf);
        } else {
            ESP_LOGW(TAG, "Host is connected to CDC no.%d, but it is not initialized. Initialize it using `tinyusb_cdc_init`.", itf);
            return;
        }
    } else { // disconnected
        if (acm != NULL) {
            ESP_LOGV(TAG, "Serial device is ready to connect to CDC no.%d", itf);
        } else {
            return;
        }
    }
    if (acm) {
        tusb_cdcacm_callback_t cb = acm->callback_line_state_changed;
        if (cb) {
            cdcacm_event_t event = {
                .type = CDC_EVENT_LINE_STATE_CHANGED,
                .line_state_changed_data = {
                    .dtr = dtr,
                    .rts = rts
                }
            };
            cb(itf, &event);
        }
    }
}

/* Invoked when CDC interface received data from host */
void tud_cdc_rx_cb(uint8_t itf)
{
    esp_tusb_cdcacm_t *acm = get_acm(itf);
    if (acm) {
        if (!acm->rx_unread_buf) {
            ESP_LOGE(TAG, "There is no RX buffer created");
            abort();
        }
    } else {
        tud_cdc_n_read_flush(itf); // we have no place to store data, so just drop it
        return;
    }
    while (tud_cdc_n_available(itf)) {
        int read_res = tud_cdc_n_read(  itf,
                                        acm->rx_tfbuf,
                                        CONFIG_TINYUSB_CDC_RX_BUFSIZE );
        int res = xRingbufferSend(acm->rx_unread_buf,
                                  acm->rx_tfbuf,
                                  read_res, 0);
        if (res != pdTRUE) {
            ESP_LOGW(TAG, "The unread buffer is too small, the data has been lost");
        } else {
            ESP_LOGV(TAG, "Sent %d bytes to the buffer", read_res);
        }
    }
    if (acm) {
        tusb_cdcacm_callback_t cb = acm->callback_rx;
        if (cb) {
            cdcacm_event_t event = {
                .type = CDC_EVENT_RX
            };
            cb(itf, &event);
        }
    }
}

// Invoked when line coding is change via SET_LINE_CODING
void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const *p_line_coding)
{
    esp_tusb_cdcacm_t *acm = get_acm(itf);
    if (acm) {
        tusb_cdcacm_callback_t cb = acm->callback_line_coding_changed;
        if (cb) {
            cdcacm_event_t event = {
                .type = CDC_EVENT_LINE_CODING_CHANGED,
                .line_coding_changed_data = {
                    .p_line_coding = p_line_coding,
                }
            };
            cb(itf, &event);
        }
    } else {
        return;
    }
}

// Invoked when received `wanted_char`
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char)
{
    esp_tusb_cdcacm_t *acm = get_acm(itf);
    if (acm) {
        tusb_cdcacm_callback_t cb = acm->callback_rx_wanted_char;
        if (cb) {
            cdcacm_event_t event = {
                .type = CDC_EVENT_RX_WANTED_CHAR,
                .rx_wanted_char_data = {
                    .wanted_char = wanted_char,
                }
            };
            cb(itf, &event);
        }
    } else {
        return;
    }
}

esp_err_t tinyusb_cdcacm_register_callback(tinyusb_cdcacm_itf_t itf,
        cdcacm_event_type_t event_type,
        tusb_cdcacm_callback_t callback)
{
    esp_tusb_cdcacm_t *acm = get_acm(itf);
    if (acm) {
        switch (event_type) {
        case CDC_EVENT_RX:
            acm->callback_rx = callback;
            return ESP_OK;
        case CDC_EVENT_RX_WANTED_CHAR:
            acm->callback_rx_wanted_char = callback;
            return ESP_OK;
        case CDC_EVENT_LINE_STATE_CHANGED:
            acm->callback_line_state_changed = callback;
            return ESP_OK;
        case CDC_EVENT_LINE_CODING_CHANGED:
            acm->callback_line_coding_changed = callback;
            return ESP_OK;
        default:
            ESP_LOGE(TAG, "Wrong event type");
            return ESP_ERR_INVALID_ARG;
        }
    } else {
        ESP_LOGE(TAG, "CDC-ACM is not initialized");
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t tinyusb_cdcacm_unregister_callback(tinyusb_cdcacm_itf_t itf,
        cdcacm_event_type_t event_type)
{
    esp_tusb_cdcacm_t *acm = get_acm(itf);
    if (!acm) {
        ESP_LOGE(TAG, "Interface is not initialized. Use `tinyusb_cdc_init` for initialization");
        return ESP_ERR_INVALID_STATE;
    }
    switch (event_type) {
    case CDC_EVENT_RX:
        acm->callback_rx = NULL;
        return ESP_OK;
    case CDC_EVENT_RX_WANTED_CHAR:
        acm->callback_rx_wanted_char = NULL;
        return ESP_OK;
    case CDC_EVENT_LINE_STATE_CHANGED:
        acm->callback_line_state_changed = NULL;
        return ESP_OK;
    case CDC_EVENT_LINE_CODING_CHANGED:
        acm->callback_line_coding_changed = NULL;
        return ESP_OK;
    default:
        ESP_LOGE(TAG, "Wrong event type");
        return ESP_ERR_INVALID_ARG;
    }
}

/*********************************************************************** TinyUSB callbacks*/
/* CDC-ACM
   ********************************************************************* */

static esp_err_t read_from_rx_unread_to_buffer(esp_tusb_cdcacm_t *acm, uint8_t *out_buf, size_t req_bytes, size_t *read_bytes)
{
    uint8_t *buf = xRingbufferReceiveUpTo(acm->rx_unread_buf, read_bytes, 0, req_bytes);
    if (buf) {
        memcpy(out_buf, buf, *read_bytes);
        vRingbufferReturnItem(acm->rx_unread_buf, (void *)(buf));
        return ESP_OK;
    } else {
        return ESP_ERR_NO_MEM;
    }
}

static esp_err_t ringbuf_mux_take(esp_tusb_cdcacm_t *acm)
{
    if (xSemaphoreTake(acm->ringbuf_read_mux, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Read error: ACM is busy");
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

static esp_err_t ringbuf_mux_give(esp_tusb_cdcacm_t *acm)
{
    BaseType_t ret = xSemaphoreGive(acm->ringbuf_read_mux);
    assert(ret == pdTRUE);
    return ESP_OK;
}

esp_err_t tinyusb_cdcacm_read(tinyusb_cdcacm_itf_t itf, uint8_t *out_buf, size_t out_buf_sz, size_t *rx_data_size)
{
    esp_tusb_cdcacm_t *acm = get_acm(itf);
    ESP_RETURN_ON_FALSE(acm, ESP_ERR_INVALID_STATE, TAG, "Interface is not initialized. Use `tinyusb_cdc_init` for initialization");
    size_t read_sz;

    /* Take a mutex to proceed two uninterrupted read operations */
    ESP_RETURN_ON_ERROR(ringbuf_mux_take(acm), TAG, "ringbuf_mux_take failed");

    esp_err_t res = read_from_rx_unread_to_buffer(acm, out_buf, out_buf_sz, &read_sz);
    if (res != ESP_OK) {
        ESP_RETURN_ON_ERROR(ringbuf_mux_give(acm), TAG, "ringbuf_mux_give failed");
        return res;
    }

    *rx_data_size = read_sz;
    /* Buffer's data can be wrapped, at that situations we should make another retrievement */
    if (read_from_rx_unread_to_buffer(acm, out_buf + read_sz, out_buf_sz - read_sz, &read_sz) == ESP_OK) {
        *rx_data_size += read_sz;
    }

    ESP_RETURN_ON_ERROR(ringbuf_mux_give(acm), TAG, "ringbuf_mux_give failed");
    return ESP_OK;
}

size_t tinyusb_cdcacm_write_queue_char(tinyusb_cdcacm_itf_t itf, char ch)
{
    if (!get_acm(itf)) { // non-initialized
        return 0;
    }
    return tud_cdc_n_write_char(itf, ch);
}

size_t tinyusb_cdcacm_write_queue(tinyusb_cdcacm_itf_t itf, const uint8_t *in_buf, size_t in_size)
{
    if (!get_acm(itf)) { // non-initialized
        return 0;
    }
    const uint32_t size_available = tud_cdc_n_write_available(itf);
    return tud_cdc_n_write(itf, in_buf, MIN(in_size, size_available));
}

static uint32_t tud_cdc_n_write_occupied(tinyusb_cdcacm_itf_t itf)
{
    return CFG_TUD_CDC_TX_BUFSIZE - tud_cdc_n_write_available(itf);
}

esp_err_t tinyusb_cdcacm_write_flush(tinyusb_cdcacm_itf_t itf, uint32_t timeout_ticks)
{
    if (!get_acm(itf)) { // non-initialized
        return ESP_FAIL;
    }

    if (!timeout_ticks) { // if no timeout - nonblocking mode
        int res = tud_cdc_n_write_flush(itf);
        if (!res) {
            ESP_LOGW(TAG, "flush failed (res: %d)", res);
            return ESP_FAIL;
        } else {
            if (tud_cdc_n_write_occupied(itf)) {
                ESP_LOGW(TAG, "remained data to flush!");
                return ESP_FAIL;
            }
        }
        return ESP_ERR_TIMEOUT;
    } else { // trying during the timeout
        uint32_t ticks_start = xTaskGetTickCount();
        uint32_t ticks_now = ticks_start;
        while (1) { // loop until success or until the time runs out
            ticks_now = xTaskGetTickCount();
            if (!tud_cdc_n_write_occupied(itf)) { // if nothing to write - nothing to flush
                break;
            }
            if (tud_cdc_n_write_flush(itf)) { // Success
                break;
            }
            if ( (ticks_now - ticks_start) > timeout_ticks ) { // Time is up
                ESP_LOGW(TAG, "Flush failed");
                return ESP_ERR_TIMEOUT;
            }
            vTaskDelay(1);
        }
        return ESP_OK;
    }
}

static esp_err_t alloc_obj(tinyusb_cdcacm_itf_t itf)
{
    esp_tusb_cdc_t *cdc_inst = tinyusb_cdc_get_intf(itf);
    cdc_inst->subclass_obj = calloc(1, sizeof(esp_tusb_cdcacm_t));
    if (!cdc_inst->subclass_obj) {
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }
}

static void free_obj(tinyusb_cdcacm_itf_t itf)
{
    esp_tusb_cdc_t *cdc_inst = tinyusb_cdc_get_intf(itf);
    free(cdc_inst->subclass_obj);
    cdc_inst->subclass_obj = NULL;
}

esp_err_t tusb_cdc_acm_init(const tinyusb_config_cdcacm_t *cfg)
{
    int itf = (int)cfg->cdc_port;
    /* Creating a CDC object */
    const tinyusb_config_cdc_t cdc_cfg = {
        .usb_dev = cfg->usb_dev,
        .cdc_class = TUSB_CLASS_CDC,
        .cdc_subclass.comm_subclass = CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL
    };
    ESP_RETURN_ON_ERROR(tinyusb_cdc_init(itf, &cdc_cfg), TAG, "tinyusb_cdc_init failed");
    ESP_RETURN_ON_ERROR(alloc_obj(itf), TAG, "alloc_obj failed");

    esp_tusb_cdcacm_t *acm = get_acm(itf);
    /* Callbacks setting up*/
    if (cfg->callback_rx) {
        tinyusb_cdcacm_register_callback(itf, CDC_EVENT_RX, cfg->callback_rx);
    }
    if (cfg->callback_rx_wanted_char) {
        tinyusb_cdcacm_register_callback(itf, CDC_EVENT_RX_WANTED_CHAR, cfg->callback_rx_wanted_char);
    }
    if (cfg->callback_line_state_changed) {
        tinyusb_cdcacm_register_callback(itf, CDC_EVENT_LINE_STATE_CHANGED, cfg->callback_line_state_changed);
    }
    if (cfg->callback_line_coding_changed) {
        tinyusb_cdcacm_register_callback( itf, CDC_EVENT_LINE_CODING_CHANGED, cfg->callback_line_coding_changed);
    }

    /* Buffers */

    acm->ringbuf_read_mux = xSemaphoreCreateMutex();
    if (acm->ringbuf_read_mux == NULL) {
        ESP_LOGE(TAG, "Creation of a ringbuf mutex failed");
        free_obj(itf);
        return ESP_ERR_NO_MEM;
    }

    acm->rx_tfbuf = malloc(CONFIG_TINYUSB_CDC_RX_BUFSIZE);
    if (!acm->rx_tfbuf) {
        ESP_LOGE(TAG, "Creation buffer error");
        free_obj(itf);
        return ESP_ERR_NO_MEM;
    }
    acm->rx_unread_buf_sz = cfg->rx_unread_buf_sz == 0 ? RX_UNREADBUF_SZ_DEFAULT : cfg->rx_unread_buf_sz;
    acm->rx_unread_buf = xRingbufferCreate(acm->rx_unread_buf_sz, RINGBUF_TYPE_BYTEBUF);
    if (acm->rx_unread_buf == NULL) {
        ESP_LOGE(TAG, "Creation buffer error");
        free_obj(itf);
        return ESP_ERR_NO_MEM;
    } else {
        ESP_LOGD(TAG, "Comm Initialized buff:%d bytes", cfg->rx_unread_buf_sz);
        return ESP_OK;
    }
}

bool tusb_cdc_acm_initialized(tinyusb_cdcacm_itf_t itf)
{
    esp_tusb_cdcacm_t *acm = get_acm(itf);
    if (acm) {
        return true;
    } else {
        return false;
    }
}
/*********************************************************************** CDC-ACM*/
