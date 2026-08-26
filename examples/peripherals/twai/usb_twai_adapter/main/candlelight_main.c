/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "candlelight_internal.h"
#include "esp_log.h"
#include <string.h>

adapter_ctx_t g_ctx;    // global context for the adapter

void app_main(void)
{
    memset(&g_ctx, 0, sizeof(g_ctx));

    /*
     * Point each TWAI frame buffer at the same slot's gs_usb payload.
     * Host MODE will chooses classic vs FD, classic uses first 8 bytes, FD uses up to 64.
     */
    for (int i = 0; i < FRAME_POOL_DEPTH; i++) {
        g_ctx.tx_pool.frame[i].twai_frame.buffer = g_ctx.tx_pool.frame[i].gs_frame.data;
        g_ctx.rx_pool.frame[i].twai_frame.buffer = g_ctx.rx_pool.frame[i].gs_frame.data;
        g_ctx.rx_pool.frame[i].twai_frame.buffer_len = 64;
    }
    ESP_LOGI(CANDLELIGHT_TAG, "Buffer initialized: %d slots for burst data", FRAME_POOL_DEPTH);

    // populate the hardware capabilities and initialize the USB stack
    candlelight_fetch_hw_caps();
    candlelight_init_usb();
    // just return the main task, the tinyusb task already there handling.
}
