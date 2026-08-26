/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "candlelight_internal.h"
#include "esp_clk_tree.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "tinyusb.h"
#include "tinyusb_default_config.h"

#define TUSB_DESC_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN)

// gs_usb driver endpoints
enum {
    EDPT_VENDOR_OUT = 0x02,
    EDPT_VENDOR_IN = 0x81,
};

static const struct gs_device_config s_device_config = {
    .icount = GS_DEVICE_CHANNEL_COUNT - 1,
    .sw_version = GS_DEVICE_SW_VERSION,
    .hw_version = GS_DEVICE_HW_VERSION,
};

// Fixed VID/PID (openmoko candleLight) so Linux loads the in-tree gs_usb driver.
static const tusb_desc_device_t s_device_desc = {
    .bLength = sizeof(s_device_desc),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = 0x1D50,
    .idProduct = 0x606F,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01,
};

static const char *s_string_desc[] = {
    (const char[]){ 0x09, 0x04 },           // 0: English (0x0409)
    "Espressif System (SH).",               // 1: Manufacturer
    "TWAI based CandleLight CANFD",         // 2: Product
    "260715",                               // 3: Serial
};

static const uint8_t s_vendor_fs_config_desc[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, 0, 100),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 0, EDPT_VENDOR_OUT, EDPT_VENDOR_IN, 64),
};

#if (TUD_OPT_HIGH_SPEED)
static const uint8_t s_vendor_hs_config_desc[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, TUSB_DESC_TOTAL_LEN, 0, 100),

    // Interface number, string index, EP Out & EP In address, EP size
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 0, EDPT_VENDOR_OUT, EDPT_VENDOR_IN, 512),
};
#endif // TUD_OPT_HIGH_SPEED

static enum gs_can_state twai_state_to_gs_state(twai_error_state_t state)
{
    switch (state) {
    case TWAI_ERROR_ACTIVE:
        return GS_CAN_STATE_ERROR_ACTIVE;
    case TWAI_ERROR_WARNING:
        return GS_CAN_STATE_ERROR_WARNING;
    case TWAI_ERROR_PASSIVE:
        return GS_CAN_STATE_ERROR_PASSIVE;
    case TWAI_ERROR_BUS_OFF:
        return GS_CAN_STATE_BUS_OFF;
    default:
        return GS_CAN_STATE_STOPPED;
    }
}

static void timing_const_twai_to_gs(struct can_bt_const *bt_const, const twai_timing_limits_t *timing_limits)
{
    bt_const->tseg1_min = timing_limits->tseg1_min + timing_limits->prop_min;
    bt_const->tseg1_max = timing_limits->tseg1_max + timing_limits->prop_max;
    bt_const->tseg2_min = timing_limits->tseg2_min;
    bt_const->tseg2_max = timing_limits->tseg2_max;
    bt_const->sjw_max = timing_limits->sjw_max;
    bt_const->brp_min = timing_limits->brp_min;
    bt_const->brp_max = timing_limits->brp_max;
    bt_const->brp_inc = timing_limits->brp_inc;
}

/*
 * gs_usb vendor control path. Each bRequest has SETUP then ACK stages.
 * Typical host sequence: HOST_FORMAT -> GET_BT_CONST[_EXT] -> SET_BITTIMING
 * [-> SET_DATA_BITTIMING] -> MODE(start) ... MODE(stop).
 */
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request)
{
    if (request->bmRequestType_bit.type != TUSB_REQ_TYPE_VENDOR ||
            request->bmRequestType_bit.recipient != TUSB_REQ_RCPT_INTERFACE) {
        return false;
    }

    ESP_LOGD(CANDLELIGHT_TAG, "tud_vendor_control_xfer_cb: request->bRequest = %d, stage = %d", request->bRequest, stage);
    switch ((enum gs_usb_breq)request->bRequest) {
    case GS_USB_BREQ_HOST_FORMAT:           /* endianness probe */
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, &g_ctx.host_config, sizeof(g_ctx.host_config));
        }
        return true;

    case GS_USB_BREQ_DEVICE_CONFIG:         /* channel count / versions */
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, (void *)&s_device_config, sizeof(s_device_config));
        }
        return true;

    case GS_USB_BREQ_GET_BT_CONST:          /* classic timing limits */
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, (void *)&g_ctx.gsdev_bt_const, sizeof(struct gs_device_bt_const));
        }
        return true;

    /* only chips who report `GS_CAN_FEATURE_BT_CONST_EXT` will trigger this request */
    case GS_USB_BREQ_GET_BT_CONST_EXT:      /* classic + FD data-phase limits */
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, (void *)&g_ctx.gsdev_bt_const, sizeof(struct gs_device_bt_const_extended));
        }
        return true;

    case GS_USB_BREQ_SET_BITTIMING:         /* arbitration / classic bitrate */
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, &g_ctx.requested_bittiming, sizeof(g_ctx.requested_bittiming));
        }
        return true;

    case GS_USB_BREQ_SET_DATA_BITTIMING:    /* FD data-phase bitrate */
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, &g_ctx.requested_data_bittiming, sizeof(g_ctx.requested_data_bittiming));
        }
        return true;

    case GS_USB_BREQ_MODE:                  /* start/stop channel; create/delete TWAI node */
        if (stage == CONTROL_STAGE_SETUP) {
            return tud_control_xfer(rhport, request, &g_ctx.requested_mode, sizeof(g_ctx.requested_mode));
        } else if (stage == CONTROL_STAGE_ACK) {
            if (g_ctx.requested_mode.mode == GS_CAN_MODE_START) {
                // host request start, save configs and create twai node
                g_ctx.tud_rx_pending = 0;
                bool is_fd = g_ctx.requested_mode.flags & GS_CAN_MODE_FD;
                bool hw_ts = g_ctx.requested_mode.flags & GS_CAN_MODE_HW_TIMESTAMP;
                g_ctx.usb_rx_frame_size = is_fd ? GS_HOST_FRAME_FD_SIZE : GS_HOST_FRAME_CLASSIC_SIZE;
                g_ctx.usb_tx_frame_size = g_ctx.usb_rx_frame_size +
                                          (hw_ts ? GS_HOST_FRAME_TIMESTAMP_SIZE : 0);
                if (candlelight_twai_init_and_start() != ESP_OK) {
                    g_ctx.requested_mode.mode = GS_CAN_MODE_RESET;
                    g_ctx.usb_rx_frame_size = 0;
                    g_ctx.usb_tx_frame_size = 0;
                    return false;
                }
            } else {
                // host request stop, stop twai node and reset configs
                candlelight_twai_stop_and_delete();
                g_ctx.usb_rx_frame_size = 0;
                g_ctx.usb_tx_frame_size = 0;
            }
        }
        return true;

    case GS_USB_BREQ_GET_STATE:             /* error state + TEC/REC */
        if (stage == CONTROL_STAGE_SETUP) {
            g_ctx.device_state.state = GS_CAN_STATE_STOPPED;
            g_ctx.device_state.rxerr = 0;
            g_ctx.device_state.txerr = 0;
            twai_node_status_t status = {};
            if (g_ctx.node_hdl && g_ctx.requested_mode.mode == GS_CAN_MODE_START &&
                    twai_node_get_info(g_ctx.node_hdl, &status, NULL) == ESP_OK) {
                g_ctx.device_state.state = twai_state_to_gs_state(status.state);
                g_ctx.device_state.rxerr = status.rx_error_count;
                g_ctx.device_state.txerr = status.tx_error_count;
            }
            return tud_control_xfer(rhport, request, &g_ctx.device_state, sizeof(g_ctx.device_state));
        }
        return true;

    case GS_USB_BREQ_TIMESTAMP:             /* µs clock for host HW timestamp sync */
        if (stage == CONTROL_STAGE_SETUP) {
            g_ctx.device_timestamp_us = (uint32_t)esp_timer_get_time();
            ESP_LOGI(CANDLELIGHT_TAG, "ts_sync: %u", g_ctx.device_timestamp_us);
            return tud_control_xfer(rhport, request, &g_ctx.device_timestamp_us, sizeof(g_ctx.device_timestamp_us));
        }
        return true;

    default:
        return false;
    }
}

/*
 * USB OUT path: host sends a fixed-length byte stream of gs_host_frame.
 * Reassemble with usb_rx_frame_size (classic 20 or FD 76), then hand off to TWAI.
 */
void tud_vendor_rx_cb(uint8_t itf, uint8_t const *buffer, uint16_t bufsize)
{
    (void)buffer;
    (void)bufsize;

    adapter_frame_pool_t *tx_pool = &g_ctx.tx_pool;

    if (g_ctx.usb_rx_frame_size == 0) {
        return;
    }

    // Host sends a fixed-length stream; slice into frames of usb_rx_frame_size.
    while (tud_vendor_n_available(itf) > 0) {
        uint8_t *tmp_frame = (uint8_t *) & (frame_pool_slot(tx_pool, tx_pool->in_idx)->gs_frame);

        g_ctx.tud_rx_pending += tud_vendor_n_read(itf, tmp_frame + g_ctx.tud_rx_pending, g_ctx.usb_rx_frame_size - g_ctx.tud_rx_pending);
        if (g_ctx.tud_rx_pending < g_ctx.usb_rx_frame_size) {
            break;
        }
        g_ctx.tud_rx_pending = 0;

        // The input stream writes directly into the next slot; keep that slot free until a full frame arrives.
        if (frame_pool_full_with_reserved(tx_pool, 1)) {
            ESP_LOGW(CANDLELIGHT_TAG, "No mem, drop usb frame");
            break;
        }

        // as `tud_vendor_rx_cb` is task context, we can send frame here
        candlelight_twai_send_frame(frame_pool_slot(tx_pool, tx_pool->in_idx));
        tx_pool->in_idx++;
    }
}

void candlelight_fetch_hw_caps(void)
{
    twai_timing_limits_t timing_limits = {};
    twai_node_onchip_get_timing_limits(false, &timing_limits);
    timing_const_twai_to_gs(&g_ctx.gsdev_bt_const.bt_const, &timing_limits);

    uint32_t clk_src_freq_hz = 0;
    esp_clk_tree_src_get_freq_hz(TWAI_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);
    g_ctx.gsdev_bt_const.fclk_can = clk_src_freq_hz;
    g_ctx.gsdev_bt_const.feature = GS_CAN_FEATURE_LISTEN_ONLY | GS_CAN_FEATURE_LOOP_BACK |
                                   GS_CAN_FEATURE_ONE_SHOT | GS_CAN_FEATURE_GET_STATE |
                                   GS_CAN_FEATURE_TRIPLE_SAMPLE | GS_CAN_FEATURE_BERR_REPORTING |
                                   GS_CAN_FEATURE_HW_TIMESTAMP;

#if SOC_HAS(TWAI_FD)
    twai_node_onchip_get_timing_limits(true, &timing_limits);
    timing_const_twai_to_gs(&g_ctx.gsdev_bt_const.dbt_const, &timing_limits);
    g_ctx.gsdev_bt_const.feature |= GS_CAN_FEATURE_FD | GS_CAN_FEATURE_BT_CONST_EXT;
#endif
}

esp_err_t candlelight_init_usb(void)
{
    tinyusb_config_t tusb_cfg = TINYUSB_DEFAULT_CONFIG();
    tusb_cfg.phy.skip_setup = false;
    tusb_cfg.phy.self_powered = false;
    tusb_cfg.descriptor.device = &s_device_desc;
    tusb_cfg.descriptor.string = s_string_desc;
    tusb_cfg.descriptor.string_count = sizeof(s_string_desc) / sizeof(s_string_desc[0]);
    tusb_cfg.descriptor.full_speed_config = s_vendor_fs_config_desc;
#if (TUD_OPT_HIGH_SPEED)
    tusb_cfg.descriptor.high_speed_config = s_vendor_hs_config_desc;
    tusb_cfg.descriptor.qualifier = NULL;
#endif // TUD_OPT_HIGH_SPEED

    ESP_RETURN_ON_ERROR(tinyusb_driver_install(&tusb_cfg), CANDLELIGHT_TAG, "tinyusb_driver_install failed");
    ESP_LOGI(CANDLELIGHT_TAG, "tinyusb_driver_install success");
    return ESP_OK;
}
