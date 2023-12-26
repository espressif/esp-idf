/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "hal/usb_serial_jtag_ll.h"
#include "hal/usb_fsls_phy_ll.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "esp_intr_alloc.h"
#include "driver/usb_serial_jtag.h"
#include "soc/periph_defs.h"
#include "soc/soc_caps.h"
#include "esp_check.h"
#include "esp_private/periph_ctrl.h"

#if !SOC_RCC_IS_INDEPENDENT
#define USJ_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define USJ_RCC_ATOMIC()
#endif

// The hardware buffer max size is 64
#define USB_SER_JTAG_ENDP_SIZE          (64)
#define USB_SER_JTAG_RX_MAX_SIZE        (64)

typedef struct{
    intr_handle_t intr_handle;          /*!< USB-SERIAL-JTAG interrupt handler */

    // RX parameters
    RingbufHandle_t rx_ring_buf;        /*!< RX ring buffer handler */
    uint32_t rx_buf_size;               /*!< TX buffer size */
    uint8_t rx_data_buf[USB_SER_JTAG_ENDP_SIZE];            /*!< Data buffer to stash FIFO data */

    // TX parameters
    uint32_t tx_buf_size;               /*!< TX buffer size */
    RingbufHandle_t tx_ring_buf;        /*!< TX ring buffer handler */
    uint8_t tx_data_buf[USB_SER_JTAG_ENDP_SIZE];  /*!< Data buffer to stash TX FIFO data */
    size_t tx_stash_cnt;                          /*!< Number of stashed TX FIFO bytes */
} usb_serial_jtag_obj_t;

static usb_serial_jtag_obj_t *p_usb_serial_jtag_obj = NULL;

static const char* USB_SERIAL_JTAG_TAG = "usb_serial_jtag";

static size_t usb_serial_jtag_write_and_flush(const uint8_t *buf, uint32_t wr_len)
{
    size_t size = usb_serial_jtag_ll_write_txfifo(buf, wr_len);
    usb_serial_jtag_ll_txfifo_flush();
    return size;
}

static void usb_serial_jtag_isr_handler_default(void *arg) {
    BaseType_t xTaskWoken = 0;
    uint32_t usbjtag_intr_status = 0;
    usbjtag_intr_status = usb_serial_jtag_ll_get_intsts_mask();

    if (usbjtag_intr_status & USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY) {
        // Interrupt tells us the host picked up the data we sent.
        // If we have more data, we can put it in the buffer and the host will pick that up next.
        // Send data in isr.
        // If the hardware fifo is available, write in it. Otherwise, do nothing.
        if (usb_serial_jtag_ll_txfifo_writable() == 1) {
            // We disable the interrupt here so that the interrupt won't be triggered if there is no data to send.
            usb_serial_jtag_ll_disable_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY);
            size_t queued_size;
            uint8_t *queued_buff = NULL;
            bool is_stashed_data = false;
            if (p_usb_serial_jtag_obj->tx_stash_cnt != 0) {
                // Send stashed tx bytes before reading bytes from ring buffer
                queued_buff = p_usb_serial_jtag_obj->tx_data_buf;
                queued_size = p_usb_serial_jtag_obj->tx_stash_cnt;
                is_stashed_data = true;
            } else {
                // Max 64 data payload size in a single EndPoint
                queued_buff = (uint8_t *)xRingbufferReceiveUpToFromISR(p_usb_serial_jtag_obj->tx_ring_buf, &queued_size, USB_SER_JTAG_ENDP_SIZE);
            }

            usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY);

            if (queued_buff != NULL) {

                // Although tx_queued_bytes may be larger than 0, we may have
                // interrupted before xRingbufferSend() was called.
                // Copy the queued buffer into the TX FIFO

                // On ringbuffer wrap-around the size can be 0 even though the buffer returned is not NULL
                if (queued_size > 0) {
                    uint32_t sent_size = usb_serial_jtag_write_and_flush(queued_buff, queued_size);

                    if (sent_size < queued_size) {
                        // Not all bytes could be sent at once, stash the unwritten bytes in a tx buffer
                        // stash_size will not larger than USB_SER_JTAG_ENDP_SIZE because queued_size is got from xRingbufferReceiveUpToFromISR
                        size_t stash_size = queued_size - sent_size;
                        memcpy(p_usb_serial_jtag_obj->tx_data_buf, &queued_buff[sent_size], stash_size);
                        p_usb_serial_jtag_obj->tx_stash_cnt = stash_size;
                    } else {
                        p_usb_serial_jtag_obj->tx_stash_cnt = 0;
                        // assert if sent_size is larger than queued_size.
                        assert(sent_size <= queued_size);
                    }
                }
                if (is_stashed_data == false) {
                    vRingbufferReturnItemFromISR(p_usb_serial_jtag_obj->tx_ring_buf, queued_buff, &xTaskWoken);
                }
                usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY);
            }
        } else {
            usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY);
        }
    }

    if (usbjtag_intr_status & USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT) {
        // read rx buffer(max length is 64), and send avaliable data to ringbuffer.
        // Ensure the rx buffer size is larger than RX_MAX_SIZE.
        usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
        uint32_t rx_fifo_len = usb_serial_jtag_ll_read_rxfifo(p_usb_serial_jtag_obj->rx_data_buf, USB_SER_JTAG_RX_MAX_SIZE);
        xRingbufferSendFromISR(p_usb_serial_jtag_obj->rx_ring_buf, p_usb_serial_jtag_obj->rx_data_buf, rx_fifo_len, &xTaskWoken);
    }

    if (xTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t usb_serial_jtag_driver_install(usb_serial_jtag_driver_config_t *usb_serial_jtag_config)
{
    esp_err_t err = ESP_OK;
    ESP_RETURN_ON_FALSE((p_usb_serial_jtag_obj == NULL), ESP_ERR_INVALID_STATE, USB_SERIAL_JTAG_TAG, "Driver already installed");
    ESP_RETURN_ON_FALSE((usb_serial_jtag_config->rx_buffer_size > 0), ESP_ERR_INVALID_ARG, USB_SERIAL_JTAG_TAG, "RX buffer is not prepared");
    ESP_RETURN_ON_FALSE((usb_serial_jtag_config->rx_buffer_size > USB_SER_JTAG_RX_MAX_SIZE), ESP_ERR_INVALID_ARG, USB_SERIAL_JTAG_TAG, "RX buffer prepared is so small, should larger than 64");
    ESP_RETURN_ON_FALSE((usb_serial_jtag_config->tx_buffer_size > 0), ESP_ERR_INVALID_ARG, USB_SERIAL_JTAG_TAG, "TX buffer is not prepared");
    p_usb_serial_jtag_obj = (usb_serial_jtag_obj_t*) heap_caps_calloc(1, sizeof(usb_serial_jtag_obj_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    p_usb_serial_jtag_obj->rx_buf_size = usb_serial_jtag_config->rx_buffer_size;
    p_usb_serial_jtag_obj->tx_buf_size = usb_serial_jtag_config->tx_buffer_size;
    p_usb_serial_jtag_obj->tx_stash_cnt = 0;
    if (p_usb_serial_jtag_obj == NULL) {
        ESP_LOGE(USB_SERIAL_JTAG_TAG, "memory allocate error");
        err = ESP_ERR_NO_MEM;
        goto _exit;
    }

    p_usb_serial_jtag_obj->rx_ring_buf = xRingbufferCreate(p_usb_serial_jtag_obj->rx_buf_size, RINGBUF_TYPE_BYTEBUF);
    if (p_usb_serial_jtag_obj->rx_ring_buf == NULL) {
        ESP_LOGE(USB_SERIAL_JTAG_TAG, "ringbuffer create error");
        err = ESP_ERR_NO_MEM;
        goto _exit;
    }

    p_usb_serial_jtag_obj->tx_ring_buf = xRingbufferCreate(usb_serial_jtag_config->tx_buffer_size, RINGBUF_TYPE_BYTEBUF);
    if (p_usb_serial_jtag_obj->rx_ring_buf == NULL) {
        ESP_LOGE(USB_SERIAL_JTAG_TAG, "ringbuffer create error");
        err = ESP_ERR_NO_MEM;
        goto _exit;
    }

    // Enable USB-Serial-JTAG peripheral module clock
    USJ_RCC_ATOMIC() {
        usb_serial_jtag_ll_enable_bus_clock(true);
    }

    // Configure PHY
    usb_fsls_phy_ll_int_jtag_enable(&USB_SERIAL_JTAG);

    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY|
                                         USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
    usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY|
                                     USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);

    err = esp_intr_alloc(ETS_USB_SERIAL_JTAG_INTR_SOURCE, 0, usb_serial_jtag_isr_handler_default, NULL, &p_usb_serial_jtag_obj->intr_handle);
    if (err != ESP_OK) {
        goto _exit;
    }
    return ESP_OK;

_exit:
    usb_serial_jtag_driver_uninstall();
    return err;
}

int usb_serial_jtag_read_bytes(void* buf, uint32_t length, TickType_t ticks_to_wait)
{
    uint8_t *data = NULL;
    size_t data_read_len = 0;

    if (length == 0) {
        return 0;
    }

    // Recieve new data from ISR
    data = (uint8_t*) xRingbufferReceiveUpTo(p_usb_serial_jtag_obj->rx_ring_buf, &data_read_len, (TickType_t) ticks_to_wait, length);
    if (data == NULL) {
        // If there is no data received from ringbuffer, return 0 directly.
        return 0;
    }

    memcpy((uint8_t*)buf, data, data_read_len);
    vRingbufferReturnItem(p_usb_serial_jtag_obj->rx_ring_buf, data);
    data = NULL;

    return data_read_len;
}

int usb_serial_jtag_write_bytes(const void* src, size_t size, TickType_t ticks_to_wait)
{
    ESP_RETURN_ON_FALSE(size != 0, ESP_ERR_INVALID_ARG, USB_SERIAL_JTAG_TAG, "size should be larger than 0");
    ESP_RETURN_ON_FALSE(src != NULL, ESP_ERR_INVALID_ARG, USB_SERIAL_JTAG_TAG, "Invalid buffer pointer.");
    ESP_RETURN_ON_FALSE(p_usb_serial_jtag_obj != NULL, ESP_ERR_INVALID_ARG, USB_SERIAL_JTAG_TAG, "The driver hasn't been initialized");

    const uint8_t *buff = (const uint8_t *)src;
    // Blocking method, Sending data to ringbuffer, and handle the data in ISR.
    BaseType_t result = xRingbufferSend(p_usb_serial_jtag_obj->tx_ring_buf, (void*) (buff), size, ticks_to_wait);
    // Now trigger the ISR to read data from the ring buffer.
    usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY);
    return (result == pdFALSE) ? 0 : size;
}

esp_err_t usb_serial_jtag_driver_uninstall(void)
{
    if(p_usb_serial_jtag_obj == NULL) {
        ESP_LOGI(USB_SERIAL_JTAG_TAG, "ALREADY NULL");
        return ESP_OK;
    }

    /* Not disable the module clock and usb_pad_enable here since the USJ stdout might still depends on it. */
    //Disable tx/rx interrupt.
    usb_serial_jtag_ll_disable_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY | USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
    esp_intr_free(p_usb_serial_jtag_obj->intr_handle);

    if(p_usb_serial_jtag_obj->rx_ring_buf) {
        vRingbufferDelete(p_usb_serial_jtag_obj->rx_ring_buf);
        p_usb_serial_jtag_obj->rx_ring_buf = NULL;
    }
    if(p_usb_serial_jtag_obj->tx_ring_buf) {
        vRingbufferDelete(p_usb_serial_jtag_obj->tx_ring_buf);
        p_usb_serial_jtag_obj->tx_ring_buf = NULL;
    }
    heap_caps_free(p_usb_serial_jtag_obj);
    p_usb_serial_jtag_obj = NULL;
    return ESP_OK;
}
