/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "esp_heap_caps.h"
#include "esp_log.h"

#include "usbh_core.h"

#if CONFIG_CHERRYUSB_HOST_CDC_ACM
#include "usbh_cdc_acm.h"
#endif
#if CONFIG_CHERRYUSB_HOST_FTDI
#include "usbh_ftdi.h"
#endif
#if CONFIG_CHERRYUSB_HOST_CH34X
#include "usbh_ch34x.h"
#endif
#if CONFIG_CHERRYUSB_HOST_CP210X
#include "usbh_cp210x.h"
#endif
#if CONFIG_CHERRYUSB_HOST_PL2303
#include "usbh_pl2303.h"
#endif

static char *TAG = "CDC_ACM";

#define RINGBUF_SIZE 1024
static RingbufHandle_t s_recv_ringbuf = NULL;
static TaskHandle_t s_printf_task_handle = NULL;

typedef enum {
    SERIAL_TYPE_CDC_ACM = 0,
    SERIAL_TYPE_FTDI,
    SERIAL_TYPE_CH34X,
    SERIAL_TYPE_CP210X,
    SERIAL_TYPE_PL2303,
} serial_type_t;

typedef struct {
    serial_type_t type;
    struct usbh_hubport *hport;

    struct usbh_urb *bulkout_urb;
    struct usb_endpoint_descriptor *bulkout;
    uint8_t *out_buffer;

    struct usbh_urb *bulkin_urb;
    struct usb_endpoint_descriptor *bulkin;
    uint8_t *in_buffer;
} serial_t;

static void free_serial_buffer(serial_t *serial);
static esp_err_t serial_start_in(serial_t *serial);

void ld_include_cdc_acm(void)
{
}

#define alloc_serial_buffer(serial_class, serial_type, out_serial) \
    {   \
        out_serial = heap_caps_calloc(1, sizeof(serial_t), MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL); \
        if (out_serial) { \
            out_serial->type = serial_type; \
            out_serial->hport = serial_class->hport; \
            out_serial->bulkout_urb = &serial_class->bulkout_urb; \
            out_serial->bulkout = serial_class->bulkout; \
            out_serial->out_buffer = heap_caps_aligned_alloc(CONFIG_USB_ALIGN_SIZE, out_serial->bulkout->wMaxPacketSize, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL); \
            out_serial->bulkin_urb = &serial_class->bulkin_urb; \
            out_serial->bulkin = serial_class->bulkin; \
            out_serial->in_buffer = heap_caps_aligned_alloc(CONFIG_USB_ALIGN_SIZE, out_serial->bulkin->wMaxPacketSize, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL); \
            if(out_serial->out_buffer == NULL || out_serial->in_buffer == NULL) { \
                free_serial_buffer(out_serial); \
                out_serial = NULL; \
            }   \
        }   \
    }

static void free_serial_buffer(serial_t *serial)
{
    if (serial == NULL) {
        return;
    }
    if (serial->out_buffer) {
        heap_caps_free(serial->out_buffer);
    }
    if (serial->in_buffer) {
        heap_caps_free(serial->in_buffer);
    }
    heap_caps_free(serial);
}

//Note: This callback is in the interrupt context
static void serial_in_cb(void *arg, int nbytes)
{
    BaseType_t xTaskWoken = pdFALSE;
    serial_t *serial = (serial_t *)arg;
    uint8_t *data = serial->in_buffer;
    if (nbytes < 0) {
        return;
    }
    if (serial->type == SERIAL_TYPE_FTDI) {
        if (nbytes <= 2) {
            serial_start_in(serial);
            return;
        }
        //FTDI Skip the first two bytes (header)
        data += 2;
        nbytes -= 2;
    }

    if (s_recv_ringbuf) {
        if (xRingbufferSendFromISR(s_recv_ringbuf, data, nbytes, &xTaskWoken) != pdTRUE) {
            ESP_LOGD(TAG, "Ringbuffer send failed");
        }
    }
    if (xTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
    serial_start_in(serial);
}

static esp_err_t serial_start_in(serial_t *serial)
{
    int ret;
    usbh_bulk_urb_fill(serial->bulkin_urb, serial->hport, serial->bulkin, serial->in_buffer, serial->bulkin->wMaxPacketSize, 0, serial_in_cb, serial);
    ret = usbh_submit_urb(serial->bulkin_urb);
    if (ret != 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t serial_out(serial_t *serial, uint8_t *data, size_t len, uint32_t timeout)
{
    int ret;
    if (len > serial->bulkout->wMaxPacketSize) {
        return ESP_ERR_INVALID_SIZE;
    }
    memcpy(serial->out_buffer, data, len);
    usbh_bulk_urb_fill(serial->bulkout_urb, serial->hport, serial->bulkout, serial->out_buffer, len, timeout, NULL, NULL);
    ret = usbh_submit_urb(serial->bulkout_urb);
    if (ret != 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void usbh_cdc_acm_printf_task(void *arg)
{
    while (1) {
        size_t length = 0;
        char *data = (char *)xRingbufferReceive(s_recv_ringbuf, &length, portMAX_DELAY);
        if (data == NULL) {
            continue;
        }
        ESP_LOGI(TAG, "Data received");
        ESP_LOG_BUFFER_HEXDUMP(TAG, data, length, ESP_LOG_INFO);
        vRingbufferReturnItem(s_recv_ringbuf, (void *)data);
        fflush(stdout);
    }
    vTaskDelete(NULL);
}

static void creat_printf_task(void)
{
    if (s_recv_ringbuf == NULL) {
        s_recv_ringbuf = xRingbufferCreate(RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF);
        if (s_recv_ringbuf == NULL) {
            ESP_LOGE(TAG, "ringbuf create failed");
            return;
        }
    }
    if (s_printf_task_handle == NULL) {
        xTaskCreate(usbh_cdc_acm_printf_task, "usbh_cdc_acm_printf_task", 4096, NULL, 5, &s_printf_task_handle);
    }
}

#if CONFIG_CHERRYUSB_HOST_CDC_ACM

void usbh_cdc_acm_run(struct usbh_cdc_acm *cdc_acm_class)
{
    serial_t *serial;
    alloc_serial_buffer(cdc_acm_class, SERIAL_TYPE_CDC_ACM, serial);
    if (serial == NULL) {
        ESP_LOGW(TAG, "Malloc failed");
        return;
    }
    creat_printf_task();
    cdc_acm_class->user_data = serial;

    struct cdc_line_coding linecoding = {
        .dwDTERate = 115200,
        .bDataBits = 8,
        .bParityType = 0,
        .bCharFormat = 0,
    };
    usbh_cdc_acm_set_line_coding(cdc_acm_class, &linecoding);

    serial_start_in(serial);
    const char data[]  = "CDC: Hello, world!\r\n";
    serial_out(serial, (uint8_t *)data, sizeof(data), 1000);
}

void usbh_cdc_acm_stop(struct usbh_cdc_acm *cdc_acm_class)
{
    free_serial_buffer(cdc_acm_class->user_data);
}

#endif

#if CONFIG_CHERRYUSB_HOST_FTDI
void usbh_ftdi_run(struct usbh_ftdi *ftdi_class)
{
    serial_t *serial;
    alloc_serial_buffer(ftdi_class, SERIAL_TYPE_FTDI, serial);
    if (serial == NULL) {
        ESP_LOGW(TAG, "Malloc failed");
        return;
    }
    creat_printf_task();
    ftdi_class->user_data = serial;

    struct cdc_line_coding linecoding = {
        .dwDTERate = 115200,
        .bDataBits = 8,
        .bParityType = 0,
        .bCharFormat = 0,
    };
    usbh_ftdi_set_line_coding(ftdi_class, &linecoding);

    serial_start_in(serial);
    const char data[] = "FTDI: Hello, world!\r\n";

    serial_out(serial, (uint8_t *)data, sizeof(data), 1000);
}

void usbh_ftdi_stop(struct usbh_ftdi *ftdi_class)
{
    free_serial_buffer(ftdi_class->user_data);
}
#endif

#if CONFIG_CHERRYUSB_HOST_CH34X
void usbh_ch34x_run(struct usbh_ch34x *ch34x_class)
{
    serial_t *serial;
    alloc_serial_buffer(ch34x_class, SERIAL_TYPE_CH34X, serial);
    if (serial == NULL) {
        ESP_LOGW(TAG, "Malloc failed");
        return;
    }
    creat_printf_task();
    ch34x_class->user_data = serial;

    struct cdc_line_coding linecoding = {
        .dwDTERate = 115200,
        .bDataBits = 8,
        .bParityType = 0,
        .bCharFormat = 0,
    };
    usbh_ch34x_set_line_coding(ch34x_class, &linecoding);

    serial_start_in(serial);
    const char data[]  = "CH34x: Hello, world!\r\n";
    serial_out(serial, (uint8_t *)data, sizeof(data), 1000);
}

void usbh_ch34x_stop(struct usbh_ch34x *ch34x_class)
{
    free_serial_buffer(ch34x_class->user_data);
}
#endif

#if CONFIG_CHERRYUSB_HOST_CP210X
void usbh_cp210x_run(struct usbh_cp210x *cp210x_class)
{
    serial_t *serial;
    alloc_serial_buffer(cp210x_class, SERIAL_TYPE_CP210X, serial);
    if (serial == NULL) {
        ESP_LOGW(TAG, "Malloc failed");
        return;
    }
    creat_printf_task();
    cp210x_class->user_data = serial;

    struct cdc_line_coding linecoding = {
        .dwDTERate = 115200,
        .bDataBits = 8,
        .bParityType = 0,
        .bCharFormat = 0,
    };
    usbh_cp210x_set_line_coding(cp210x_class, &linecoding);

    serial_start_in(serial);
    const char data[]  = "CP201x: Hello, world!\r\n";
    serial_out(serial, (uint8_t *)data, sizeof(data), 1000);
}

void usbh_cp210x_stop(struct usbh_cp210x *cp210x_class)
{
    free_serial_buffer(cp210x_class->user_data);
}
#endif

#if CONFIG_CHERRYUSB_HOST_PL2303
void usbh_pl2303_run(struct usbh_pl2303 *pl2303_class)
{
    serial_t *serial;
    alloc_serial_buffer(pl2303_class, SERIAL_TYPE_PL2303, serial);
    if (serial == NULL) {
        ESP_LOGW(TAG, "Malloc failed");
        return;
    }
    creat_printf_task();
    pl2303_class->user_data = serial;

    struct cdc_line_coding linecoding = {
        .dwDTERate = 115200,
        .bDataBits = 8,
        .bParityType = 0,
        .bCharFormat = 0,
    };
    usbh_pl2303_set_line_coding(pl2303_class, &linecoding);

    serial_start_in(serial);
    const char data[]  = "PL2303: Hello, world!\r\n";
    serial_out(serial, (uint8_t *)data, sizeof(data), 1000);
}

void usbh_pl2303_stop(struct usbh_pl2303 *pl2303_class)
{
    free_serial_buffer(pl2303_class->user_data);
}
#endif
