/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * USB-CAN (gs_usb / candleLight) adapter internals.
 *
 * Control path: TinyUSB vendor control transfers (bit timing, start/stop, caps).
 * Data path: vendor bulk endpoints carry a fixed-length byte stream of gs_host_frame.
 * Host TX confirmation: after TWAI finishes a host-originated frame, echo the same
 * gs_host_frame back on USB (see tx_echo_task). RX frames use echo_id = UINT32_MAX.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "hal/twai_types.h"
#include "gs_usb.h"

#define CANDLELIGHT_TAG         "candlelight_twai"

/* sw_version: keep > 2 so Linux does not apply legacy device quirks; YYMMDD is fine.
 * hw_version: board/hardware revision, start from 1.
 */
#define GS_DEVICE_SW_VERSION     260715
#define GS_DEVICE_HW_VERSION     1
#define GS_DEVICE_CHANNEL_COUNT  1

#define TWAI_TX_GPIO           4
#define TWAI_RX_GPIO           5

/* Frame pool depth for each directional buffer (USB->TWAI and TWAI->USB), must be a power of 2. */
#define FRAME_POOL_DEPTH       256
_Static_assert((UINT32_MAX % FRAME_POOL_DEPTH) == (FRAME_POOL_DEPTH - 1), "invalid FRAME_POOL_DEPTH value");

enum {
    ITF_NUM_VENDOR = 0, /* TinyUSB vendor interface index for gs_usb bulk endpoints */
    ITF_NUM_TOTAL,      /* Number of USB interfaces in the configuration descriptor */
};

/**
 * One pool slot: TWAI header + gs_usb wire frame.
 * twai_frame.buffer points at gs_frame.data so payload is zero-copied.
 */
typedef struct {
    twai_frame_t twai_frame;
    struct gs_host_frame gs_frame;
} adapter_frame_t;

/**
 * Ring of adapter frames for one direction.
 *
 * TX and RX use separate pools: USB->TWAI (tx_pool) and TWAI->USB (rx_pool) have
 * different producers/consumers and overflow rules (RX keeps one slot for error frames).
 * in_idx is the next free write slot; out_idx is the next slot to consume.
 */
typedef struct {
    adapter_frame_t frame[FRAME_POOL_DEPTH];
    uint32_t in_idx;
    uint32_t out_idx;
} adapter_frame_pool_t;

/* Shared runtime context for the USB-to-TWAI adapter tasks and state. */
typedef struct {
    adapter_frame_pool_t tx_pool;
    adapter_frame_pool_t rx_pool;
    SemaphoreHandle_t usb_tx_mutex;
    SemaphoreHandle_t tx_done_sem;
    SemaphoreHandle_t rx_cnt_sem;

    TaskHandle_t twai_rx_task_handle;
    TaskHandle_t tx_echo_task_handle;

    struct gs_host_config host_config;
    struct gs_device_bt_const_extended gsdev_bt_const;
    struct gs_device_bittiming requested_bittiming;
    struct gs_device_bittiming requested_data_bittiming;
    struct gs_device_mode requested_mode;
    struct gs_device_state device_state;
    uint32_t device_timestamp_us;

    twai_node_handle_t node_hdl;
    uint32_t usb_rx_frame_size;     /* Host -> device bulk frame size (no timestamp) */
    uint32_t usb_tx_frame_size;     /* Device -> host bulk frame size (may include timestamp) */
    volatile uint32_t tud_rx_pending;
} adapter_ctx_t;

extern adapter_ctx_t g_ctx;

static inline adapter_frame_t *frame_pool_slot(adapter_frame_pool_t *pool, uint32_t idx)
{
    return &pool->frame[idx % FRAME_POOL_DEPTH];
}

static inline uint32_t frame_pool_count(const adapter_frame_pool_t *pool)
{
    return (uint32_t)(pool->in_idx - pool->out_idx);
}

static inline bool frame_pool_full_with_reserved(const adapter_frame_pool_t *pool, uint32_t reserved_slots)
{
    return frame_pool_count(pool) >= (FRAME_POOL_DEPTH - reserved_slots);
}

/* Populate GS-USB descriptors with the local TWAI hardware capabilities. */
void candlelight_fetch_hw_caps(void);

/* Initialize the USB device stack used by the Candlelight adapter. */
esp_err_t candlelight_init_usb(void);

/* Create and start the TWAI node used to exchange frames with the bus. */
esp_err_t candlelight_twai_init_and_start(void);

/* Send a frame to the TWAI driver. */
void candlelight_twai_send_frame(adapter_frame_t *frame);

/* Stop TWAI traffic and tasks, and delete the TWAI node. */
void candlelight_twai_stop_and_delete(void);
