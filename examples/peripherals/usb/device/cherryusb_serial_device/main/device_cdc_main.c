/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "usbd_core.h"
#include "usbd_cdc_acm.h"

static char *TAG = "device_cdc_main";

#ifndef CONFIG_EXAMPLE_CHERRYUSB_CDC_ACM_TWO_CHANNEL
#define CDC_ACM_CHANNEL_NUM 1
#else
#define CDC_ACM_CHANNEL_NUM 2
#endif

#define INTERFACES_NUM (2 * CDC_ACM_CHANNEL_NUM)

#define NOTIFY_USB_CONFIGURED_BIT BIT0
#define NOTIFY_EXIT_BIT BIT1
#define NOTIFY_EP_BIT  BIT2

static TaskHandle_t s_task_handle = NULL;

/*!< endpoint address */
#define CDC_IN_EP  0x81
#define CDC_OUT_EP 0x01
#define CDC_INT_EP 0x83
#if CDC_ACM_CHANNEL_NUM >= 2
#define CDC_IN_EP1  0x82
#define CDC_OUT_EP1 0x02
#define CDC_INT_EP1 0x84
#endif

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN * CDC_ACM_CHANNEL_NUM)

#define CDC_FS_MAX_MPS 64
#define CDC_HS_MAX_MPS 512

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS CDC_HS_MAX_MPS
#else
#define CDC_MAX_MPS CDC_FS_MAX_MPS
#endif

typedef struct {
    uint8_t rx_addr;
    uint8_t tx_addr;
    bool rx_busy_flag;
    bool tx_busy_flag;
    uint32_t read_len;
    uint32_t write_len;
} ep_status_t;

static ep_status_t s_ep_status[CDC_ACM_CHANNEL_NUM] = {
    { CDC_OUT_EP, CDC_IN_EP, false, false, 0, 0 },
#ifdef CONFIG_EXAMPLE_CHERRYUSB_CDC_ACM_TWO_CHANNEL
    { CDC_OUT_EP1, CDC_IN_EP1, false, false, 0, 0 },
#endif
};

#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))

#define WRITE_BUFFER_SIZE ALIGN_UP(ALIGN_UP(2048, CDC_MAX_MPS), CONFIG_USB_ALIGN_SIZE)

#if CONFIG_EXAMPLE_CHERRYUSB_SET_READ_BUFFER_SIZE_MPS
#define READ_BUFFER_SIZE ALIGN_UP(CDC_MAX_MPS, CONFIG_USB_ALIGN_SIZE)
#else
#define READ_BUFFER_SIZE WRITE_BUFFER_SIZE
#endif

static uint32_t s_mps = CDC_MAX_MPS;
static DRAM_DMA_ALIGNED_ATTR uint8_t read_buffer[CDC_ACM_CHANNEL_NUM][READ_BUFFER_SIZE];
static DRAM_DMA_ALIGNED_ATTR uint8_t write_buffer[CDC_ACM_CHANNEL_NUM][WRITE_BUFFER_SIZE];

static const uint8_t device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01)
};

static const uint8_t config_descriptor_fs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTERFACES_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_FS_MAX_MPS, 0x02),
#ifdef CONFIG_EXAMPLE_CHERRYUSB_CDC_ACM_TWO_CHANNEL
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC_INT_EP1, CDC_OUT_EP1, CDC_IN_EP1, CDC_FS_MAX_MPS, 0x02),
#endif
};

#ifdef CONFIG_USB_HS
static const uint8_t config_descriptor_hs[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTERFACES_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_HS_MAX_MPS, 0x02),
#ifdef CONFIG_EXAMPLE_CHERRYUSB_CDC_ACM_TWO_CHANNEL
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC_INT_EP1, CDC_OUT_EP1, CDC_IN_EP1, CDC_HS_MAX_MPS, 0x02),
#endif
};
#endif

static const uint8_t device_quality_descriptor[] = {
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
};

static const char *string_descriptors[] = {
    (const char[])
    {
        0x09, 0x04
    }, /* Langid */
    "CherryUSB",                  /* Manufacturer */
    "CherryUSB CDC MULTI DEMO",   /* Product */
    "2025060518",                 /* Serial Number */
};

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    return device_descriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
#ifdef CONFIG_USB_HS
    if (speed >= USB_SPEED_HIGH) {
        s_mps = CDC_HS_MAX_MPS;
        return config_descriptor_hs;
    }
    s_mps = CDC_FS_MAX_MPS;
#endif
    return config_descriptor_fs;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    if (index > 3) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor cdc_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};

//Note: This callback is in the interrupt context
void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED: {
        BaseType_t HPTaskAwoken = pdFALSE;
        xTaskNotifyFromISR(s_task_handle, NOTIFY_USB_CONFIGURED_BIT, eSetBits, &HPTaskAwoken);
        if (HPTaskAwoken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
        break;
    }
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

//Note: This callback is in the interrupt context
static void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    ESP_EARLY_LOGI(TAG, "ep 0x%02X actual out len:%d", ep, nbytes);
    BaseType_t HPTaskAwoken = pdFALSE;
    for (size_t i = 0; i < CDC_ACM_CHANNEL_NUM; i++) {
        ep_status_t *ep_status = &s_ep_status[i];
        if (ep_status->rx_addr == ep) {
            if (nbytes > 0) {
                if (ep_status->write_len == 0) {
                    memcpy(write_buffer[i], read_buffer[i], nbytes);
                    ep_status->write_len = nbytes;
#if CONFIG_EXAMPLE_CHERRYUSB_SET_READ_BUFFER_SIZE_MPS
                    usbd_ep_start_read(0, ep, read_buffer[i], s_mps);
#else
                    usbd_ep_start_read(0, ep, read_buffer[i], sizeof(read_buffer[0]));
#endif
                } else {
                    ep_status->read_len = nbytes;
                    ep_status->rx_busy_flag = false;
                }
                xTaskNotifyFromISR(s_task_handle, NOTIFY_EP_BIT, eSetBits, &HPTaskAwoken);
            } else {
#if CONFIG_EXAMPLE_CHERRYUSB_SET_READ_BUFFER_SIZE_MPS
                usbd_ep_start_read(0, ep, read_buffer[i], s_mps);
#else
                usbd_ep_start_read(0, ep, read_buffer[i], sizeof(read_buffer[i]));
#endif
            }
            break;
        }
    }
    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

//Note: This callback is in the interrupt context
static void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    BaseType_t HPTaskAwoken = pdFALSE;
    ESP_EARLY_LOGI(TAG, "ep 0x%02X actual in len:%d", ep, nbytes);
    if ((nbytes % s_mps) == 0 && nbytes) {
        usbd_ep_start_write(0, ep, NULL, 0);
    } else {
        for (size_t i = 0; i < CDC_ACM_CHANNEL_NUM; i++) {
            ep_status_t *ep_status = &s_ep_status[i];
            if (ep_status->tx_addr == ep) {
                ep_status->write_len = 0;
                ep_status->tx_busy_flag = false;
                xTaskNotifyFromISR(s_task_handle, NOTIFY_EP_BIT, eSetBits, &HPTaskAwoken);
                break;
            }
        }
    }
    if (HPTaskAwoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

struct usbd_interface intf0;
struct usbd_interface intf1;

#ifdef CONFIG_EXAMPLE_CHERRYUSB_CDC_ACM_TWO_CHANNEL
struct usbd_endpoint cdc_out_ep1 = {
    .ep_addr = CDC_OUT_EP1,
    .ep_cb = usbd_cdc_acm_bulk_out
};

struct usbd_endpoint cdc_in_ep1 = {
    .ep_addr = CDC_IN_EP1,
    .ep_cb = usbd_cdc_acm_bulk_in
};

struct usbd_interface intf2;
struct usbd_interface intf3;
#endif

static void cdc_acm_init(void)
{
    usbd_desc_register(0, &cdc_descriptor);

    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf0));
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf1));
    usbd_add_endpoint(0, &cdc_out_ep);
    usbd_add_endpoint(0, &cdc_in_ep);
#ifdef CONFIG_EXAMPLE_CHERRYUSB_CDC_ACM_TWO_CHANNEL
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf2));
    usbd_add_interface(0, usbd_cdc_acm_init_intf(0, &intf3));
    usbd_add_endpoint(0, &cdc_out_ep1);
    usbd_add_endpoint(0, &cdc_in_ep1);
#endif

#if CONFIG_EXAMPLE_USB_DEVICE_RHPORT_HS
    usbd_initialize(0, ESP_USB_HS0_BASE, usbd_event_handler);
#else
    usbd_initialize(0, ESP_USB_FS0_BASE, usbd_event_handler);
#endif
}

//Note: This callback is in the interrupt context
void usbd_cdc_acm_set_dtr(uint8_t busid, uint8_t intf, bool dtr)
{
    ESP_EARLY_LOGI(TAG, "intf:%u, dtr:%d", intf, dtr);
}

//Note: This callback is in the interrupt context
void usbd_cdc_acm_set_rts(uint8_t busid, uint8_t intf, bool rts)
{
    ESP_EARLY_LOGI(TAG, "intf:%u, rts:%d", intf, rts);
}

void cdc_acm_task(void *arg)
{
    uint32_t notify_value = 0;
    ESP_LOGI(TAG, "usb cdc acm task start");
    // Initialize the USB driver and CDC interface
    cdc_acm_init();
    ESP_LOGI(TAG, "usb cdc acm init done");

    while (1) {
        xTaskNotifyWait(0, 0xFFFFFFFF, &notify_value, portMAX_DELAY);
        if (notify_value & NOTIFY_EXIT_BIT) {
            break;
        }
        if (notify_value & NOTIFY_USB_CONFIGURED_BIT) {
            /* setup first out ep read transfer */
            for (size_t i = 0; i < sizeof(s_ep_status) / sizeof(s_ep_status[0]); i++) {
                ep_status_t *ep_status = &s_ep_status[i];
                ep_status->write_len = 0;
                ep_status->tx_busy_flag = false;

                ep_status->read_len = 0;
                ep_status->rx_busy_flag = true;
#if CONFIG_EXAMPLE_CHERRYUSB_SET_READ_BUFFER_SIZE_MPS
                usbd_ep_start_read(0, ep_status->rx_addr, read_buffer[i], s_mps);
#else
                usbd_ep_start_read(0, ep_status->rx_addr, read_buffer[i], sizeof(read_buffer[0]));
#endif
            }
        }
        for (size_t i = 0; i < sizeof(s_ep_status) / sizeof(s_ep_status[0]); i++) {
            ep_status_t *ep_status = &s_ep_status[i];
            if (ep_status->tx_busy_flag == false) {
                if (ep_status->write_len == 0 && ep_status->read_len > 0) {
                    memcpy(&write_buffer[i][0], &read_buffer[i][0], ep_status->read_len);
                    ep_status->write_len = ep_status->read_len;
                    ep_status->read_len = 0;
                }
                if (ep_status->write_len > 0) {
                    ep_status->tx_busy_flag = true;
                    usbd_ep_start_write(0, ep_status->tx_addr, write_buffer[i], ep_status->write_len);
                }
            }
            if (ep_status->rx_busy_flag == false) {
                if (ep_status->read_len == 0) {
                    ep_status->rx_busy_flag = true;
#if CONFIG_EXAMPLE_CHERRYUSB_SET_READ_BUFFER_SIZE_MPS
                    usbd_ep_start_read(0, ep_status->rx_addr, read_buffer[i], s_mps);
#else
                    usbd_ep_start_read(0, ep_status->rx_addr, read_buffer[i], sizeof(read_buffer[i]));
#endif
                }
            }
        }
    }

    usbd_deinitialize(0);
    ESP_LOGW(TAG, "task exit");
    s_task_handle = NULL;
    vTaskDelete(NULL);
}

void app_main(void)
{
#if CONFIG_EXAMPLE_CHERRYUSB_INIT_DEINIT_LOOP
    while (1)
#endif
    {
        xTaskCreatePinnedToCore(&cdc_acm_task, "cdc_acm_task", 1024 * 3, NULL, 10, &s_task_handle, 0);

#if CONFIG_EXAMPLE_CHERRYUSB_INIT_DEINIT_LOOP
        for (int i = 10; i >= 0; i--) {
            ESP_LOGW(TAG, "Deinit usb after %d seconds...", i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGW(TAG, "Notify deinit usb");
        xTaskNotify(s_task_handle, NOTIFY_EXIT_BIT, eSetBits);
        while (s_task_handle) {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
#endif
    }

}
