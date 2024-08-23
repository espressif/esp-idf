/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "esp_log.h"
#include "hal/usb_serial_jtag_ll.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "esp_intr_alloc.h"
#include "driver/usb_serial_jtag.h"
#include "soc/periph_defs.h"
#include "soc/soc_caps.h"
#include "esp_check.h"
#include "esp_private/periph_ctrl.h"

/*
 Note: Before you add a workaround for an issue in this driver, please please try
 to figure out the actual root cause first. The USB-serial-JTAG is a simple device,
 it shouldn't need anything more than a simple, straightforward driver.
*/

#if !SOC_RCC_IS_INDEPENDENT
#define USJ_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define USJ_RCC_ATOMIC()
#endif

// The hardware buffer max size is 64, both for RX and TX.
#define USB_SER_JTAG_ENDP_SIZE          (64)
#define USB_SER_JTAG_RX_MAX_SIZE        (USB_SER_JTAG_ENDP_SIZE)

typedef struct{
    intr_handle_t intr_handle;          /*!< USB-SERIAL-JTAG interrupt handler */

    // RX parameters
    RingbufHandle_t rx_ring_buf;        /*!< RX ring buffer handler */

    // TX parameters
    RingbufHandle_t tx_ring_buf;        /*!< TX ring buffer handler */
    uint8_t tx_stash_buf[USB_SER_JTAG_ENDP_SIZE];  /*!< Data buffer to stash TX FIFO data */
    size_t tx_stash_cnt;                          /*!< Number of stashed TX FIFO bytes */
} usb_serial_jtag_obj_t;

static usb_serial_jtag_obj_t *p_usb_serial_jtag_obj = NULL;

static const char* USB_SERIAL_JTAG_TAG = "usb_serial_jtag";

static void usb_serial_jtag_isr_handler_default(void *arg)
{
    BaseType_t xTaskWoken = 0;
    uint32_t usbjtag_intr_status = 0;
    usbjtag_intr_status = usb_serial_jtag_ll_get_intsts_mask();

    if (usbjtag_intr_status & USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY) {
        //Clear interrupt so we won't be called until the next transfer finishes.
        usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY);

        // Interrupt tells us the host picked up the data we sent.
        // If we have more data, we can put it in the buffer and the host will pick that up next.
        // We expect the TX FIFO to be writable for this. If it's not, somehow someone else
        // (ROM print routines?) have snuck in a full buffer before we got here. In that case,
        // we simply ignore the interrupt, a new one will come if the buffer is empty again.
        if (usb_serial_jtag_ll_txfifo_writable() == 1) {
            // Retrieve data from either the stash buffer or, if that's empty, from the ring buffer.
            size_t queued_size;
            uint8_t *queued_buf = NULL;
            if (p_usb_serial_jtag_obj->tx_stash_cnt != 0) {
                // Send stashed tx bytes before reading bytes from ring buffer
                queued_buf = p_usb_serial_jtag_obj->tx_stash_buf;
                queued_size = p_usb_serial_jtag_obj->tx_stash_cnt;
            } else {
                // Max 64 data payload size in a single EndPoint
                queued_buf = (uint8_t *)xRingbufferReceiveUpToFromISR(p_usb_serial_jtag_obj->tx_ring_buf, &queued_size, USB_SER_JTAG_ENDP_SIZE);
            }

            if (queued_buf != NULL && queued_size > 0) {
                // We have some data to send. Send it.
                uint32_t sent_size = usb_serial_jtag_ll_write_txfifo(queued_buf, queued_size);
                usb_serial_jtag_ll_txfifo_flush();

                // Check if we were able to send everything.
                if (sent_size < queued_size) {
                    // Not all bytes could be sent at once; stash the unwritten bytes in a buffer
                    // This will happen if e.g. the rom output functions manage to sneak a few bytes into the
                    // TX FIFO before this interrupt triggers. Note stash_size will not larger than
                    // USB_SER_JTAG_ENDP_SIZE because queued_size is obtained from xRingbufferReceiveUpToFromISR.
                    size_t stash_size = queued_size - sent_size;
                    memcpy(p_usb_serial_jtag_obj->tx_stash_buf, &queued_buf[sent_size], stash_size);
                    p_usb_serial_jtag_obj->tx_stash_cnt = stash_size;
                } else {
                    p_usb_serial_jtag_obj->tx_stash_cnt = 0;
                }
                // Return the buffer if we got it from the ring buffer.
                if (queued_buf != p_usb_serial_jtag_obj->tx_stash_buf) {
                    vRingbufferReturnItemFromISR(p_usb_serial_jtag_obj->tx_ring_buf, queued_buf, &xTaskWoken);
                }
            } else {
                // No data to send.
                // The last transmit may have sent a full EP worth of data. The host will interpret
                // this as a transaction that hasn't finished yet and keep the data in its internal
                // buffers rather than releasing it to the program listening on the CDC serial port.
                // We need to flush again in order to send a 0-byte packet that ends the transaction.
                usb_serial_jtag_ll_txfifo_flush();

                // We will also disable the interrupt as for now there's no need to handle the
                // TX interrupt again. We'll re-enable this externally if we need data sent.
                usb_serial_jtag_ll_disable_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY);
            }
        }
    }

    if (usbjtag_intr_status & USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT) {
        // Acknowledge interrupt
        usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
        // Read RX FIFO and send available data to ringbuffer.
        uint8_t buf[USB_SER_JTAG_RX_MAX_SIZE];
        uint32_t rx_fifo_len = usb_serial_jtag_ll_read_rxfifo(buf, USB_SER_JTAG_RX_MAX_SIZE);
        xRingbufferSendFromISR(p_usb_serial_jtag_obj->rx_ring_buf, buf, rx_fifo_len, &xTaskWoken);
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
    p_usb_serial_jtag_obj = (usb_serial_jtag_obj_t*) heap_caps_calloc(1, sizeof(usb_serial_jtag_obj_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (p_usb_serial_jtag_obj == NULL) {
        ESP_LOGE(USB_SERIAL_JTAG_TAG, "memory allocate error");
        // no `goto _exit` here as there's nothing to clean up and that would make the uninstall
        // routine unhappy.
        return ESP_ERR_NO_MEM;
    }
    p_usb_serial_jtag_obj->tx_stash_cnt = 0;

    p_usb_serial_jtag_obj->rx_ring_buf = xRingbufferCreate(usb_serial_jtag_config->rx_buffer_size, RINGBUF_TYPE_BYTEBUF);
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
#if USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
    usb_serial_jtag_ll_phy_enable_external(false);  // Use internal PHY
    usb_serial_jtag_ll_phy_enable_pad(true);        // Enable USB PHY pads
#else // USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
    usb_serial_jtag_ll_phy_set_defaults();          // External PHY not supported. Set default values.
#endif // USB_WRAP_LL_EXT_PHY_SUPPORTED

    // Note: DO NOT clear the interrupt status bits here. The output routine needs
    // USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY set because it needs the ISR to trigger
    // as soon as data is sent; the input routine needs the status to retrieve any
    // data that is still in the FIFOs.

    // We only enable the RX interrupt; we'll enable the TX one when we actually
    // have anything to send.
    usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);

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

    BaseType_t result = pdTRUE;
    result = xRingbufferSend(p_usb_serial_jtag_obj->tx_ring_buf, (void*)src, size, ticks_to_wait);
    // Re-enable the TX interrupt. If this was disabled, this will immediately trigger the ISR
    // and send the things we just put in the ringbuffer.
    usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY);
    return (result == pdFALSE) ? 0 : size;
}

//Note that this is also called when usb_serial_jtag_driver_install errors out and as such should
//work on a half-initialized driver as well.
esp_err_t usb_serial_jtag_driver_uninstall(void)
{
    if (p_usb_serial_jtag_obj == NULL) {
        ESP_LOGE(USB_SERIAL_JTAG_TAG, "uninstall without install called");
        return ESP_OK;
    }

    /* Don't disable the module clock and usb_pad_enable here since the USJ stdout might still depends on it. */

    usb_serial_jtag_ll_disable_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY | USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
    if (p_usb_serial_jtag_obj->intr_handle) {
        esp_intr_free(p_usb_serial_jtag_obj->intr_handle);
    }

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
