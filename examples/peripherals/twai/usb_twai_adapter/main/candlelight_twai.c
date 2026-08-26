/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include "candlelight_internal.h"
#include "esp_check.h"
#include "esp_log.h"
#include "tinyusb.h"

/* Convert gs_usb header fields only; payload stays in the shared gs_frame.data buffer. */
static void frame_gs_to_twai(twai_frame_t *twai_out, const struct gs_host_frame *gs_in)
{
    bool is_ext = !!(gs_in->can_id & CAN_EFF_FLAG);

    twai_out->header.id = gs_in->can_id & (is_ext ? TWAI_EXT_ID_MASK : TWAI_STD_ID_MASK);
    twai_out->header.dlc = gs_in->can_dlc;
    twai_out->header.ide = is_ext;
    twai_out->header.rtr = !!(gs_in->can_id & CAN_RTR_FLAG);
    twai_out->header.fdf = !!(gs_in->flags & GS_CAN_FLAG_FD);
    twai_out->header.brs = !!(gs_in->flags & GS_CAN_FLAG_BRS);
    twai_out->header.esi = !!(gs_in->flags & GS_CAN_FLAG_ESI);
    twai_out->header.timestamp = 0; // tx don't use timestamp
}

/* Same as frame_gs_to_twai: header only, payload already in place. */
static void frame_twai_to_gs(struct gs_host_frame *gs_out, const twai_frame_t *twai_in, uint32_t echo_id)
{
    const twai_frame_header_t *twai_header = &twai_in->header;

    gs_out->echo_id = echo_id;
    gs_out->can_id = twai_header->id & (twai_header->ide ? TWAI_EXT_ID_MASK : TWAI_STD_ID_MASK);
    if (twai_header->ide) {
        gs_out->can_id |= CAN_EFF_FLAG;
    }
    if (twai_header->rtr) {
        gs_out->can_id |= CAN_RTR_FLAG;
    }
    gs_out->can_id = (twai_header->id & CAN_ERR_FLAG) ? twai_header->id : gs_out->can_id;
    gs_out->can_dlc = twai_header->dlc;
    gs_out->channel = 0;
    gs_out->flags = (twai_header->fdf ? GS_CAN_FLAG_FD : 0) |
                    (twai_header->brs ? GS_CAN_FLAG_BRS : 0) |
                    (twai_header->esi ? GS_CAN_FLAG_ESI : 0);
    if (g_ctx.requested_mode.flags & GS_CAN_MODE_HW_TIMESTAMP) {
        /* TWAI node fills header.timestamp when timestamp_resolution_hz is enabled. */
        gs_host_frame_set_timestamp(gs_out, !!(g_ctx.requested_mode.flags & GS_CAN_MODE_FD), (uint32_t)twai_header->timestamp);
    }
}

static void timing_config_gs_to_twai(twai_timing_advanced_config_t *twai_bt, const struct gs_device_bittiming *gs_bt, bool is_fd)
{
    // gs_usb describes SEG1 as prop_seg + phase_seg1, but don't know them's hardware limits; split it for the TWAI HAL limits.
    twai_timing_limits_t timing_limits = {};
    twai_node_onchip_get_timing_limits(is_fd, &timing_limits);

    uint32_t whole_seg1 = gs_bt->phase_seg1 + gs_bt->prop_seg;
    twai_bt->tseg_1 = (whole_seg1 * 3) / 4;  // tseg_1 is usually larger than prop_seg.
    twai_bt->tseg_1 = MAX(timing_limits.tseg1_min, MIN(twai_bt->tseg_1, timing_limits.tseg1_max));
    twai_bt->prop_seg = whole_seg1 - twai_bt->tseg_1;
    twai_bt->tseg_2 = gs_bt->phase_seg2;
    twai_bt->sjw = gs_bt->sjw;
    twai_bt->brp = gs_bt->brp;
}

// The gs_usb driver receives state (active, warning ...) as special RX frame.
static void IRAM_ATTR make_state_change_frame(adapter_frame_t *frame, twai_error_state_t new_state)
{
    twai_frame_header_t *twai_header = &frame->twai_frame.header;
    uint8_t *data = frame->gs_frame.data;

    memset(twai_header, 0, sizeof(twai_frame_header_t));
    memset(data, 0, CAN_ERR_DLC);

    twai_header->id = CAN_ERR_FLAG;
    twai_header->dlc = CAN_ERR_DLC;

    switch (new_state) {
    case TWAI_ERROR_ACTIVE:
        twai_header->id |= CAN_ERR_CRTL;
        data[1] = CAN_ERR_CRTL_ACTIVE;
        break;
    case TWAI_ERROR_WARNING:
        twai_header->id |= CAN_ERR_CRTL;
        data[1] = CAN_ERR_CRTL_TX_WARNING | CAN_ERR_CRTL_RX_WARNING;
        break;
    case TWAI_ERROR_PASSIVE:
        twai_header->id |= CAN_ERR_CRTL;
        data[1] = CAN_ERR_CRTL_TX_PASSIVE | CAN_ERR_CRTL_RX_PASSIVE;
        break;
    case TWAI_ERROR_BUS_OFF:
        twai_header->id |= CAN_ERR_BUSOFF;
        break;
    default:
        break;
    }
}

static bool IRAM_ATTR twai_tx_done_callback(twai_node_handle_t handle, const twai_tx_done_event_data_t *edata, void *user_ctx)
{
    (void)handle;
    (void)edata;
    (void)user_ctx;

    BaseType_t task_woken = pdFALSE;
    xSemaphoreGiveFromISR(g_ctx.tx_done_sem, &task_woken);
    return (task_woken == pdTRUE);
}

static bool IRAM_ATTR twai_rx_done_callback(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    (void)edata;
    (void)user_ctx;

    BaseType_t task_woken = pdFALSE;
    adapter_frame_pool_t *rx_pool = &g_ctx.rx_pool;

    // Keep one slot free for state-change error frames.
    if (frame_pool_full_with_reserved(rx_pool, 1)) {
        ESP_EARLY_LOGW(CANDLELIGHT_TAG, "No mem, drop esp rx frame");
        return false;
    }

    twai_frame_t *rx_frame = &frame_pool_slot(rx_pool, rx_pool->in_idx)->twai_frame;
    if (twai_node_receive_from_isr(handle, rx_frame) == ESP_OK) {
        rx_pool->in_idx++;
        xSemaphoreGiveFromISR(g_ctx.rx_cnt_sem, &task_woken);
    }
    return (task_woken == pdTRUE);
}

static bool IRAM_ATTR twai_state_change_callback(twai_node_handle_t handle, const twai_state_change_event_data_t *edata, void *user_ctx)
{
    (void)handle;
    (void)user_ctx;

    BaseType_t task_woken = pdFALSE;
    adapter_frame_pool_t *rx_pool = &g_ctx.rx_pool;

    if (frame_pool_full_with_reserved(rx_pool, 0)) {
        ESP_EARLY_LOGW(CANDLELIGHT_TAG, "No mem, drop state frame");
        return false;
    }

    // The state-change and RX callbacks run from the same ISR context, so in_idx does not need extra locking here.
    make_state_change_frame(frame_pool_slot(rx_pool, rx_pool->in_idx), edata->new_sta);
    rx_pool->in_idx++;
    xSemaphoreGiveFromISR(g_ctx.rx_cnt_sem, &task_woken);
    return (task_woken == pdTRUE);
}

/* Echo host TX frames back on USB after TWAI TX-done; gs_usb uses this as TX confirmation. */
static void tx_echo_task(void *param)
{
    (void)param;

    uint32_t pending_len = g_ctx.usb_tx_frame_size;
    adapter_frame_pool_t *tx_pool = &g_ctx.tx_pool;

    while (1) {
        xSemaphoreTake(g_ctx.usb_tx_mutex, portMAX_DELAY);
        while (pending_len < g_ctx.usb_tx_frame_size) {
            adapter_frame_t *frame = frame_pool_slot(tx_pool, tx_pool->out_idx);
            uint8_t *usb_frame = (uint8_t *)&frame->gs_frame;

            pending_len += tud_vendor_n_write(ITF_NUM_VENDOR, usb_frame + pending_len, g_ctx.usb_tx_frame_size - pending_len);
            tud_vendor_n_write_flush(ITF_NUM_VENDOR);
            if (pending_len == g_ctx.usb_tx_frame_size) {
                tx_pool->out_idx++;
                break;
            }
        }
        xSemaphoreGive(g_ctx.usb_tx_mutex);

        if (xSemaphoreTake(g_ctx.tx_done_sem, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        pending_len = 0;
    }
}

static void twai_rx_task(void *param)
{
    (void)param;

    uint32_t pending_len = g_ctx.usb_tx_frame_size;
    adapter_frame_pool_t *rx_pool = &g_ctx.rx_pool;

    while (1) {
        xSemaphoreTake(g_ctx.usb_tx_mutex, portMAX_DELAY);
        while (pending_len < g_ctx.usb_tx_frame_size) {
            adapter_frame_t *frame = frame_pool_slot(rx_pool, rx_pool->out_idx);
            uint8_t *usb_frame = (uint8_t *)&frame->gs_frame;

            frame_twai_to_gs(&frame->gs_frame, &frame->twai_frame, GS_HOST_FRAME_ECHO_ID_RX);
            if (frame->gs_frame.can_id & CAN_ERR_FLAG) {
                twai_node_status_t twai_status;
                twai_node_get_info(g_ctx.node_hdl, &twai_status, NULL);
                frame->gs_frame.data[6] = twai_status.tx_error_count;
                frame->gs_frame.data[7] = twai_status.rx_error_count;
            }

            pending_len += tud_vendor_n_write(ITF_NUM_VENDOR, usb_frame + pending_len, g_ctx.usb_tx_frame_size - pending_len);
            tud_vendor_n_write_flush(ITF_NUM_VENDOR);
            if (pending_len == g_ctx.usb_tx_frame_size) {
                rx_pool->out_idx++;
                break;
            }
        }
        xSemaphoreGive(g_ctx.usb_tx_mutex);

        if (xSemaphoreTake(g_ctx.rx_cnt_sem, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        pending_len = 0;
    }
}

/* Queue one USB-originated frame to TWAI; tx_echo_task reports completion to the host. */
void candlelight_twai_send_frame(adapter_frame_t *frame)
{
    frame_gs_to_twai(&frame->twai_frame, &frame->gs_frame);
    twai_node_transmit(g_ctx.node_hdl, &frame->twai_frame, portMAX_DELAY);
}

// --------------- init and delete helpers ---------------
static void semaphore_delete_and_set_null(SemaphoreHandle_t *semaphore)
{
    if (*semaphore) {
        vSemaphoreDelete(*semaphore);
        *semaphore = NULL;
    }
}

static void runtime_resources_delete(void)
{
    semaphore_delete_and_set_null(&g_ctx.rx_cnt_sem);
    semaphore_delete_and_set_null(&g_ctx.tx_done_sem);
    semaphore_delete_and_set_null(&g_ctx.usb_tx_mutex);
    g_ctx.tx_pool.in_idx = 0;
    g_ctx.tx_pool.out_idx = 0;
    g_ctx.rx_pool.in_idx = 0;
    g_ctx.rx_pool.out_idx = 0;
}

static esp_err_t runtime_resources_create(void)
{
    g_ctx.rx_cnt_sem = xSemaphoreCreateCounting(FRAME_POOL_DEPTH, 0);
    g_ctx.tx_done_sem = xSemaphoreCreateCounting(FRAME_POOL_DEPTH, 0);
    g_ctx.usb_tx_mutex = xSemaphoreCreateMutex();
    if (g_ctx.usb_tx_mutex && g_ctx.rx_cnt_sem && g_ctx.tx_done_sem) {
        return ESP_OK;
    }
    runtime_resources_delete();
    return ESP_ERR_NO_MEM;
}

esp_err_t candlelight_twai_init_and_start(void)
{
    esp_err_t ret = ESP_OK;

    candlelight_twai_stop_and_delete();

    twai_onchip_node_config_t node_config = {
        .io_cfg = {
            .tx = TWAI_TX_GPIO,
            .rx = TWAI_RX_GPIO,
            .quanta_clk_out = GPIO_NUM_NC,
            .bus_off_indicator = GPIO_NUM_NC,
        },
        .bit_timing = {
            .bitrate = 500000,  // Just tmp bitrate for driver install, the usb will update the bitrate later.
        },
        .timestamp_resolution_hz = (g_ctx.requested_mode.flags & GS_CAN_MODE_HW_TIMESTAMP) ? 1000000 : 0,
        .tx_queue_depth = FRAME_POOL_DEPTH,
        .fail_retry_cnt = (g_ctx.requested_mode.flags & GS_CAN_MODE_ONE_SHOT) ? 0 : -1,
        .flags = {
            .enable_loopback = !!(g_ctx.requested_mode.flags & GS_CAN_MODE_LOOP_BACK),
            .enable_listen_only = !!(g_ctx.requested_mode.flags & GS_CAN_MODE_LISTEN_ONLY),
        },
    };
    ESP_GOTO_ON_ERROR(runtime_resources_create(), err, CANDLELIGHT_TAG, "Failed to create runtime resources");
    ESP_GOTO_ON_ERROR(twai_new_node_onchip(&node_config, &g_ctx.node_hdl), err, CANDLELIGHT_TAG, "Failed to create TWAI node");

    twai_event_callbacks_t user_cbs = {
        .on_tx_done = twai_tx_done_callback,
        .on_rx_done = twai_rx_done_callback,
        .on_state_change = twai_state_change_callback,
    };
    ESP_GOTO_ON_ERROR(twai_node_register_event_callbacks(g_ctx.node_hdl, &user_cbs, NULL), err, CANDLELIGHT_TAG, "Failed to register TWAI callbacks");

    twai_timing_advanced_config_t btcfg = {}, dbtcfg = {}, *dbtcfg_ptr = NULL;
    timing_config_gs_to_twai(&btcfg, &g_ctx.requested_bittiming, false);
    // Classic TWAI maps non-zero ssp_offset to triple sampling; FD uses it as secondary sample point.
    if (g_ctx.requested_mode.flags & GS_CAN_MODE_TRIPLE_SAMPLE) {
        btcfg.ssp_offset = (uint8_t)(btcfg.prop_seg + btcfg.tseg_1);
    }
    ESP_LOGI(CANDLELIGHT_TAG, "btcfg brp %u prop %u seg1 %u seg2 %u sjw %u ssp %u", btcfg.brp, btcfg.prop_seg, btcfg.tseg_1, btcfg.tseg_2, btcfg.sjw, btcfg.ssp_offset);
    if (g_ctx.requested_mode.flags & GS_CAN_MODE_FD) {
        timing_config_gs_to_twai(&dbtcfg, &g_ctx.requested_data_bittiming, true);
        dbtcfg_ptr = &dbtcfg;
        ESP_LOGI(CANDLELIGHT_TAG, "dbtcfg brp %u prop %u seg1 %u seg2 %u sjw %u", dbtcfg.brp, dbtcfg.prop_seg, dbtcfg.tseg_1, dbtcfg.tseg_2, dbtcfg.sjw);
    }
    ESP_GOTO_ON_ERROR(twai_node_reconfig_timing(g_ctx.node_hdl, &btcfg, dbtcfg_ptr), err, CANDLELIGHT_TAG, "Failed to reconfigure TWAI timing");

    ESP_GOTO_ON_ERROR(twai_node_enable(g_ctx.node_hdl), err, CANDLELIGHT_TAG, "Failed to enable TWAI node");

    ESP_GOTO_ON_FALSE(pdPASS == xTaskCreate(tx_echo_task, "tx_echo_task", 4096, NULL, 5, &g_ctx.tx_echo_task_handle),
                      ESP_ERR_NO_MEM, err, CANDLELIGHT_TAG, "Failed to create TX echo task");
    ESP_GOTO_ON_FALSE(pdPASS == xTaskCreate(twai_rx_task, "twai_rx_task", 4096, NULL, 5, &g_ctx.twai_rx_task_handle),
                      ESP_ERR_NO_MEM, err, CANDLELIGHT_TAG, "Failed to create TWAI RX task");

    return ESP_OK;

err:
    candlelight_twai_stop_and_delete();
    return ret;
}

static void task_delete_and_set_null(TaskHandle_t *task_handle)
{
    if (*task_handle) {
        vTaskDelete(*task_handle);
        *task_handle = NULL;
    }
}

void candlelight_twai_stop_and_delete(void)
{
    if (g_ctx.node_hdl) {
        twai_node_disable(g_ctx.node_hdl);
    }
    task_delete_and_set_null(&g_ctx.tx_echo_task_handle);
    task_delete_and_set_null(&g_ctx.twai_rx_task_handle);
    if (g_ctx.node_hdl) {
        twai_node_delete(g_ctx.node_hdl);
        g_ctx.node_hdl = NULL;
    }
    runtime_resources_delete();
}
