/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief USB-Serial-JTAG transport adapter for esp_trace
 *
 * This implementation uses LL (Low-Level) functions directly instead of the
 * high-level USB-Serial-JTAG driver to avoid FreeRTOS primitives, which would
 * cause deadlocks when tracing FreeRTOS operations with SystemView.
 *
 * Locking: This transport does not implement its own lock. The encoder (e.g. sysview)
 * is responsible for serializing access using esp_trace_lock_init(), esp_trace_lock_take(),
 * and esp_trace_lock_give(). All transport operations (read, write, flush_nolock) are
 * invoked while the encoder holds the lock, so no transport-level locking is required.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_attr.h"
#include "esp_rom_caps.h"
#include "esp_heap_caps.h"
#include "esp_private/periph_ctrl.h"
#include "hal/usb_serial_jtag_ll.h"
#include "soc/usb_serial_jtag_struct.h"
#include "driver/usb_serial_jtag.h"
#include "esp_trace_registry.h"
#include "esp_trace_port_transport.h"
#include "esp_trace_types.h"
#include "esp_trace_util.h"

static const char *TAG = "usj_transport";

/* Transport context */
typedef struct {
    int inited;                         ///< Initialization flag (bitmask per core)
    esp_trace_rb_t tx_ring;             ///< TX ring buffer
    esp_trace_rb_t rx_ring;             ///< RX ring buffer

    /* Flush configuration */
    uint32_t flush_tmo;                 ///< Flush timeout in microseconds
    uint32_t flush_thresh;              ///< Flush threshold in bytes
} usj_ctx_t;

#define USJ_FLUSH_TIMEOUT_US (1000000)  // 1 second
#define USJ_FLUSH_THRESH_BYTES (0)      // 0 bytes

/* USB Serial JTAG hardware FIFO size (RX and TX) is 64 bytes (USB FS bulk endpoint max packet size) */
#define USJ_HW_FIFO_SIZE    (64)
#define USJ_RX_BUFFER_SIZE  USJ_HW_FIFO_SIZE

/* ----------------------- HW FIFO Helpers ----------------------- */
static uint32_t usj_write_fifo(usj_ctx_t *ctx, esp_trace_rb_t *rb)
{
    if (!usb_serial_jtag_ll_txfifo_writable()) {
        /* FIFO is full, no blocking */
        return 0;
    }

    const uint8_t *ptr;
    uint32_t to_send = esp_trace_rb_peek_contiguous(rb, &ptr);
    if (to_send == 0) {
        return 0;
    }

    uint32_t written = usb_serial_jtag_ll_write_txfifo(ptr, to_send);
    esp_trace_rb_consume(rb, written);

    /* Flush to send data or zero-byte packet to end USB transfer */
    usb_serial_jtag_ll_txfifo_flush();

    return written;
}

static void usj_read_rx_fifo(usj_ctx_t *ctx)
{
    uint8_t tmp[USJ_HW_FIFO_SIZE];
    while (usb_serial_jtag_ll_rxfifo_data_available()) {
        uint32_t n = usb_serial_jtag_ll_read_rxfifo(tmp, sizeof(tmp));
        if (n == 0) {
            break;
        }
        esp_trace_rb_put(&ctx->rx_ring, tmp, n);
    }
}

/* ----------------------- Transport Functions ----------------------- */
_Static_assert((CONFIG_ESP_TRACE_USJ_TX_BUFFER_SIZE & (CONFIG_ESP_TRACE_USJ_TX_BUFFER_SIZE - 1)) == 0,
               "CONFIG_ESP_TRACE_USJ_TX_BUFFER_SIZE must be a power of 2");

static esp_err_t usj_init(esp_trace_transport_t *tp, const void *tp_cfg)
{
    (void)tp_cfg;

    if (!tp) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Create context if not already done */
    if (!tp->ctx) {
        usj_ctx_t *ctx = heap_caps_calloc(1, sizeof(*ctx), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (!ctx) {
            return ESP_ERR_NO_MEM;
        }
        tp->ctx = ctx;
    }

    usj_ctx_t *ctx = (usj_ctx_t *)tp->ctx;
    int core_id = esp_cpu_get_core_id();

    /* Only do main setup on core 0 */
    if (core_id == 0) {
        /* Set default flush configuration */
        ctx->flush_tmo = USJ_FLUSH_TIMEOUT_US;
        ctx->flush_thresh = USJ_FLUSH_THRESH_BYTES;

        /* Enable USB-Serial-JTAG peripheral module clock */
        PERIPH_RCC_ATOMIC() {
            usb_serial_jtag_ll_enable_bus_clock(true);
        }

        /* Configure USB PHY */
#if USB_SERIAL_JTAG_LL_EXT_PHY_SUPPORTED
        usb_serial_jtag_ll_phy_enable_external(false);  /* Use internal PHY */
        usb_serial_jtag_ll_phy_enable_pad(true);        /* Enable USB PHY pads */
#else
        usb_serial_jtag_ll_phy_set_defaults();          /* Set default PHY values */
#endif

        /* Disable RX and TX interrupts */
        usb_serial_jtag_ll_disable_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_IN_EMPTY
                                             | USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);

        /* Initialize TX ring buffer */
        esp_err_t ret = esp_trace_rb_init(&ctx->tx_ring, CONFIG_ESP_TRACE_USJ_TX_BUFFER_SIZE);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize TX ring buffer");
            goto err_ctx;
        }

        /* Initialize RX ring buffer to capture host commands */
        ret = esp_trace_rb_init(&ctx->rx_ring, USJ_RX_BUFFER_SIZE);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize RX ring buffer");
            goto err_tx_ring;
        }

#if ESP_ROM_HAS_ETS_PRINTF_BUG
        /* Make sure no printf output is sent to USB-Serial-JTAG */
        extern bool g_usb_print;
        g_usb_print = false;
#endif

    }

    ctx->inited |= (1 << core_id);

    return ESP_OK;

err_tx_ring:
    heap_caps_free(ctx->tx_ring.buffer);
err_ctx:
    heap_caps_free(ctx);
    tp->ctx = NULL;
    return ESP_FAIL;
}

static esp_err_t usj_read(esp_trace_transport_t *tp, void *data, size_t *size, uint32_t tmo)
{
    usj_ctx_t *ctx = (usj_ctx_t *)tp->ctx;

    if (!data || !size || *size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t *buf = data;
    uint32_t req_size = *size;
    uint32_t total_read = 0;

    esp_trace_tmo_t timeout;
    esp_trace_tmo_init(&timeout, tmo);

    /* First read any pending RX data from the RX ring buffer */
    total_read = esp_trace_rb_get(&ctx->rx_ring, buf, req_size);

    while (total_read < req_size) {
        /* Try to read from HW RX FIFO directly */
        if (usb_serial_jtag_ll_rxfifo_data_available()) {
            uint32_t to_read = req_size - total_read;
            uint32_t read = usb_serial_jtag_ll_read_rxfifo(buf + total_read, to_read);
            total_read += read;
            continue;
        }

        if (esp_trace_tmo_check(&timeout) != ESP_OK) {
            break;
        }
        esp_rom_delay_us(100);
    }

    *size = total_read;
    return (total_read > 0) ? ESP_OK : ESP_ERR_TIMEOUT;
}

static esp_err_t usj_write(esp_trace_transport_t *tp, const void *data, size_t size, uint32_t tmo)
{
    (void)tmo;  /* Write is non-blocking via ring buffer */

    usj_ctx_t *ctx = (usj_ctx_t *)tp->ctx;
    esp_trace_rb_t *rb = &ctx->tx_ring;

    if (!data || size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Read any new RX data into the RX ring buffer to avoid losing host commands in case of heavy trace output */
    usj_read_rx_fifo(ctx);

    /* Add data to TX ring buffer */
    esp_trace_rb_put(rb, (const uint8_t *)data, size);

    /* Try to flush some data to HW FIFO immediately (non-blocking) */
    while (esp_trace_rb_data_len(rb) > 0) {
        if (usj_write_fifo(ctx, rb) == 0) {
            break;  /* FIFO full, will be drained on next write or flush */
        }
    }

    return ESP_OK;
}

static esp_err_t usj_down_buffer_config(esp_trace_transport_t *tp, uint8_t *buf, uint32_t size)
{
    (void)tp;
    (void)buf;
    (void)size;

    /* No action needed - data was already read in get function */

    return ESP_OK;
}

static esp_err_t usj_flush_nolock(esp_trace_transport_t *tp)
{
    usj_ctx_t *ctx = (usj_ctx_t *)tp->ctx;
    esp_trace_rb_t *rb = &ctx->tx_ring;

    uint32_t pending = esp_trace_rb_data_len(rb);
    if (pending < ctx->flush_thresh) {
        return ESP_OK;
    }

    esp_trace_tmo_t timeout;
    esp_trace_tmo_init(&timeout, ctx->flush_tmo);

    /* Drain ring buffer to HW FIFO */
    while (esp_trace_rb_data_len(rb) > 0) {
        usj_write_fifo(ctx, rb);
        if (esp_trace_tmo_check(&timeout) != ESP_OK) {
            return ESP_ERR_TIMEOUT;
        }
        esp_rom_delay_us(100);
    }

    return ESP_OK;
}

static bool usj_is_host_connected(esp_trace_transport_t *tp)
{
    (void)tp;
    return usb_serial_jtag_is_connected();
}

static esp_trace_link_types_t usj_get_link_type(esp_trace_transport_t *tp)
{
    (void)tp;
    return ESP_TRACE_LINK_USB_SERIAL_JTAG;
}

static esp_err_t usj_set_config(esp_trace_transport_t *tp, esp_trace_transport_cfg_key_t key, const void *value)
{
    if (!value) {
        return ESP_ERR_INVALID_ARG;
    }

    usj_ctx_t *ctx = (usj_ctx_t *)tp->ctx;
    if (!ctx) {
        return ESP_ERR_INVALID_STATE;
    }

    switch (key) {
    case ESP_TRACE_TRANSPORT_CFG_HEADER_SIZE:
        /* USB-Serial-JTAG doesn't need header size configuration */
        return ESP_OK;
    case ESP_TRACE_TRANSPORT_CFG_FLUSH_TMO:
        ctx->flush_tmo = *(const uint32_t *)value;
        return ESP_OK;
    case ESP_TRACE_TRANSPORT_CFG_FLUSH_THRESH:
        ctx->flush_thresh = *(const uint32_t *)value;
        return ESP_OK;
    default:
        ESP_LOGE(TAG, "Key %d is not supported", key);
        return ESP_ERR_NOT_SUPPORTED;
    }
}

static esp_err_t usj_get_config(esp_trace_transport_t *tp, esp_trace_transport_cfg_key_t key, void *value)
{
    if (!value) {
        return ESP_ERR_INVALID_ARG;
    }

    usj_ctx_t *ctx = (usj_ctx_t *)tp->ctx;
    if (!ctx) {
        return ESP_ERR_INVALID_STATE;
    }

    switch (key) {
    case ESP_TRACE_TRANSPORT_CFG_FLUSH_TMO:
        *(uint32_t *)value = ctx->flush_tmo;
        return ESP_OK;
    case ESP_TRACE_TRANSPORT_CFG_FLUSH_THRESH:
        *(uint32_t *)value = ctx->flush_thresh;
        return ESP_OK;
    default:
        ESP_LOGE(TAG, "Key %d is not supported", key);
        return ESP_ERR_NOT_SUPPORTED;
    }
}

static void usj_panic_handler(esp_trace_transport_t *tp, const void *info)
{
    (void)info;
    usj_flush_nolock(tp);
}

/* ----------------------- Transport Registration ----------------------- */
static const esp_trace_transport_vtable_t s_usb_serial_jtag_vt = {
    .init                  = usj_init,
    .set_config            = usj_set_config,
    .get_config            = usj_get_config,
    .read                  = usj_read,
    .write                 = usj_write,
    .flush_nolock          = usj_flush_nolock,
    .down_buffer_config    = usj_down_buffer_config,
    .is_host_connected     = usj_is_host_connected,
    .get_link_type         = usj_get_link_type,
    .panic_handler         = usj_panic_handler,
};

ESP_TRACE_REGISTER_TRANSPORT("usb_serial_jtag", &s_usb_serial_jtag_vt);
