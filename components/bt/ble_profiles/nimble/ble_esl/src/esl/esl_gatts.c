/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file esl_gatts.c
 * @brief ESL GATT server, OTS setup, DIS PnP ID.
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_uuid.h"
#include "host/ble_store.h"
#include "services/dis/ble_svc_dis.h"

#include "ble_esl.h"
#include "ble_esl_int.h"
#include "ble_esl_state_int.h"

#if CONFIG_BLE_ESL_OTS_SUPPORT
#include "ble_ots_server.h"
#endif

static const char *TAG = "esl_gatts";

/* ========================== Forward Declarations ========================== */
static int esl_gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg);

/* ========================== GATT Service Definition ========================== */

/*
 * We build the GATT service table dynamically at init time because
 * conditional characteristics depend on config element counts.
 * Maximum: 9 characteristics + 1 terminator = 10 entries.
 */
#define ESL_MAX_CHRS    10

/* Error response TLV: opcode byte (Tag=0, Length=0) + error code byte */
#define ESL_ECP_ERROR_RESP_SIZE     2

static const ble_uuid16_t s_esl_svc_uuid   = BLE_UUID16_INIT(BLE_ESL_SVC_UUID);
static const ble_uuid16_t s_uuid_address    = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_ESL_ADDRESS);
static const ble_uuid16_t s_uuid_ap_sync    = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_AP_SYNC_KEY);
static const ble_uuid16_t s_uuid_resp_key   = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_RESP_KEY);
static const ble_uuid16_t s_uuid_abs_time   = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_CURRENT_ABS_TIME);
static const ble_uuid16_t s_uuid_disp_info  = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_DISPLAY_INFO);
static const ble_uuid16_t s_uuid_img_info   = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_IMAGE_INFO);
static const ble_uuid16_t s_uuid_sensor_info = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_SENSOR_INFO);
static const ble_uuid16_t s_uuid_led_info   = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_LED_INFO);
static const ble_uuid16_t s_uuid_ecp        = BLE_UUID16_INIT(BLE_ESL_CHR_UUID_ECP);

/* Index mapping for characteristic handles */
enum {
    CHR_IDX_ADDRESS = 0,
    CHR_IDX_AP_SYNC,
    CHR_IDX_RESP_KEY,
    CHR_IDX_ABS_TIME,
    CHR_IDX_ECP,
    /* Conditional chars start here; indices assigned dynamically */
};

/*
 * All of this module's mutable static state is aggregated into a single
 * heap-allocated context, so it costs no RAM until the ESL GATT server is
 * initialized. NimBLE retains pointers into esl_chrs / esl_svcs / chr_handles
 * after ble_gatts_add_svcs(), so this context must live from esl_gatts_init()
 * until esl_gatts_deinit().
 */
typedef struct {
    esl_state_ctx_t *ctx;                /* borrowed state context (not owned) */

    struct ble_gatt_chr_def esl_chrs[ESL_MAX_CHRS];
    uint16_t chr_handles[9];             /* val handles for up to 9 chars */
    struct ble_gatt_svc_def esl_svcs[2]; /* service + terminator */
    uint8_t ecp_chr_idx;                 /* actual index of ECP in array */

#if CONFIG_BLE_ESL_OTS_SUPPORT
    ble_ots_obj_id_t ots_obj_ids[CONFIG_BLE_ESL_MAX_IMAGES]; /* reverse map */
    uint8_t ots_obj_count;
    bool ots_initialized;                /* OTS server init state */
#endif
} esl_gatts_ctx_t;

static esl_gatts_ctx_t *s_esl_gatts = NULL;

/**
 * @brief Build the ESL GATT service definition based on config
 */
static void esl_build_gatt_svcs(const ble_esl_config_t *config)
{
    int idx = 0;

    /* Mandatory: ESL Address (Write, Encrypted) */
    memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
    s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_address.u;
    s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
    s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
    s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
    idx++;

    /* Mandatory: AP Sync Key Material (Write, Encrypted) */
    memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
    s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_ap_sync.u;
    s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
    s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
    s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
    idx++;

    /* Mandatory: ESL Response Key Material (Write, Encrypted) */
    memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
    s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_resp_key.u;
    s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
    s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
    s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
    idx++;

    /* Mandatory: ESL Current Absolute Time (Write, Encrypted) */
    memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
    s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_abs_time.u;
    s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
    s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
    s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
    idx++;

    /* Conditional: Display Information (Read, Encrypted) */
    if (config->num_displays > 0) {
        memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
        s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_disp_info.u;
        s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
        s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
        s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
        idx++;
    }

    /* Conditional: Image Information (Read, Encrypted) — requires both display and image support */
    if (config->num_displays > 0 && config->num_images > 0) {
        memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
        s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_img_info.u;
        s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
        s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
        s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
        idx++;
    }

    /* Conditional: Sensor Information (Read, Encrypted) */
    if (config->num_sensors > 0) {
        memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
        s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_sensor_info.u;
        s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
        s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
        s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
        idx++;
    }

    /* Conditional: LED Information (Read, Encrypted) */
    if (config->num_leds > 0) {
        memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
        s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_led_info.u;
        s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
        s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
        s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
        idx++;
    }

    /* Mandatory: ECP (Write Without Response, Write, Notify, Encrypted) */
    s_esl_gatts->ecp_chr_idx = idx;
    memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));
    s_esl_gatts->esl_chrs[idx].uuid = &s_uuid_ecp.u;
    s_esl_gatts->esl_chrs[idx].access_cb = esl_gatt_access_cb;
    s_esl_gatts->esl_chrs[idx].val_handle = &s_esl_gatts->chr_handles[idx];
    s_esl_gatts->esl_chrs[idx].flags = BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_WRITE |
                            BLE_GATT_CHR_F_NOTIFY |
                            BLE_GATT_CHR_F_WRITE_ENC;
    idx++;

    /* Terminator */
    memset(&s_esl_gatts->esl_chrs[idx], 0, sizeof(struct ble_gatt_chr_def));

    /* Build service definition */
    memset(s_esl_gatts->esl_svcs, 0, sizeof(s_esl_gatts->esl_svcs));
    s_esl_gatts->esl_svcs[0].type = BLE_GATT_SVC_TYPE_PRIMARY;
    s_esl_gatts->esl_svcs[0].uuid = &s_esl_svc_uuid.u;
    s_esl_gatts->esl_svcs[0].characteristics = s_esl_gatts->esl_chrs;
    /* s_esl_gatts->esl_svcs[1] is zero-terminated */
}

/* ========================== GATT Access Callback ========================== */

/**
 * @brief Helper to extract flat data from an mbuf
 */
static int esl_gatt_write_flat(struct os_mbuf *om, void *dst,
                               uint16_t min_len, uint16_t max_len,
                               uint16_t *out_len)
{
    uint16_t om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }
    int rc = ble_hs_mbuf_to_flat(om, dst, max_len, out_len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    return 0;
}

/**
 * @brief Handle write to ESL Address characteristic
 */
static int handle_write_esl_address(struct os_mbuf *om)
{
    ble_esl_address_t addr;
    uint16_t len;
    int rc = esl_gatt_write_flat(om, &addr, sizeof(addr), sizeof(addr), &len);
    if (rc != 0) {
        return rc;
    }

    if (addr.esl_id == BLE_ESL_BROADCAST_ADDRESS) {
        ESP_LOGW(TAG, "Rejected broadcast address as ESL address");
        return BLE_ATT_ERR_VALUE_NOT_ALLOWED;
    }

    s_esl_gatts->ctx->esl_address = addr;
    s_esl_gatts->ctx->address_valid = true;
    s_esl_gatts->ctx->config_complete |= CONFIG_BIT_ADDRESS;
    ESP_LOGI(TAG, "ESL Address set: id=0x%02x group=0x%02x",
             addr.esl_id, BLE_ESL_ADDR_GROUP_ID(addr));
    return 0;
}

/**
 * @brief Handle write to AP Sync Key Material characteristic
 */
static int handle_write_ap_sync_key(struct os_mbuf *om)
{
    uint16_t len;
    int rc = esl_gatt_write_flat(om, &s_esl_gatts->ctx->ap_sync_key,
                                 BLE_ESL_KEY_MATERIAL_SIZE,
                                 BLE_ESL_KEY_MATERIAL_SIZE, &len);
    if (rc != 0) {
        return rc;
    }

    s_esl_gatts->ctx->ap_sync_key_valid = true;
    s_esl_gatts->ctx->config_complete |= CONFIG_BIT_AP_SYNC_KEY;
    ESP_LOGI(TAG, "AP Sync Key Material written");
    return 0;
}

/**
 * @brief Handle write to ESL Response Key Material characteristic
 */
static int handle_write_resp_key(struct os_mbuf *om)
{
    uint16_t len;
    int rc = esl_gatt_write_flat(om, &s_esl_gatts->ctx->resp_key,
                                 BLE_ESL_KEY_MATERIAL_SIZE,
                                 BLE_ESL_KEY_MATERIAL_SIZE, &len);
    if (rc != 0) {
        return rc;
    }

    s_esl_gatts->ctx->resp_key_valid = true;
    s_esl_gatts->ctx->config_complete |= CONFIG_BIT_RESP_KEY;
    ESP_LOGI(TAG, "ESL Response Key Material written");
    return 0;
}

/**
 * @brief Handle write to ESL Current Absolute Time characteristic
 */
static int handle_write_abs_time(struct os_mbuf *om)
{
    uint32_t time_val;
    uint16_t len;
    int rc = esl_gatt_write_flat(om, &time_val, sizeof(time_val),
                                 sizeof(time_val), &len);
    if (rc != 0) {
        return rc;
    }

    s_esl_gatts->ctx->abs_time_base = time_val;
    s_esl_gatts->ctx->abs_time_offset_us = esp_timer_get_time();
    s_esl_gatts->ctx->config_complete |= CONFIG_BIT_ABS_TIME;
    ESP_LOGI(TAG, "ESL Absolute Time set: %" PRIu32 " ms", time_val);
    return 0;
}

/**
 * @brief Handle write to ECP characteristic — decode TLV, dispatch, respond
 */
static int handle_write_ecp(uint16_t conn_handle, struct os_mbuf *om)
{
    uint8_t buf[BLE_ESL_TLV_MAX_SIZE];
    uint16_t om_len = OS_MBUF_PKTLEN(om);

    if (om_len < BLE_ESL_TLV_MIN_SIZE || om_len > BLE_ESL_TLV_MAX_SIZE) {
        ESP_LOGW(TAG, "ECP write: invalid length %u", om_len);
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    uint16_t flat_len;
    int rc = ble_hs_mbuf_to_flat(om, buf, sizeof(buf), &flat_len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    /* Decode TLV */
    uint8_t opcode;
    const uint8_t *params;
    uint8_t params_len;
    esp_err_t err = ble_esl_tlv_decode(buf, flat_len, &opcode, &params, &params_len);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ECP: TLV decode failed");
        return BLE_ATT_ERR_UNLIKELY;
    }

    /* Start ECP procedure timeout timer */
    esp_timer_stop(s_esl_gatts->ctx->ecp_timer);
    esp_timer_start_once(s_esl_gatts->ctx->ecp_timer, ESL_ECP_TIMEOUT_US);

    /* Dispatch command */
    ble_esl_cmd_result_t result;
    memset(&result, 0, sizeof(result));
    err = esl_cmd_dispatch(opcode, params, params_len, &result, ESL_TRANSPORT_ECP);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "ECP: command dispatch failed: %d", (int)err);
    }

    /* Send response notification if command produced one */
    if (result.has_response) {
        /* Stop ECP timeout — response is being sent now */
        esp_timer_stop(s_esl_gatts->ctx->ecp_timer);
        uint8_t resp_buf[BLE_ESL_TLV_MAX_SIZE];
        uint8_t resp_len;
        err = ble_esl_tlv_encode(result.resp_opcode, result.resp_params,
                                 result.resp_params_len,
                                 resp_buf, &resp_len);
        if (err != ESP_OK) {
            /* The AP must always get a response (the timeout timer is already
             * stopped), so fall back to an Unspecified Error TLV — same
             * defensive handling as the PAwR transport. */
            ESP_LOGW(TAG, "ECP: response TLV encode failed, sending Unspecified Error");
            resp_buf[0] = BLE_ESL_RESP_ERROR;
            resp_buf[1] = BLE_ESL_ERR_UNSPECIFIED;
            resp_len = ESL_ECP_ERROR_RESP_SIZE;
        }
        err = esl_send_ecp_response(resp_buf, resp_len);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "ECP: response send failed: %d", (int)err);
            /* No notification was enqueued, so neither the notify callback nor
             * the notify-tx event will run the deferred Unassociate cleanup. */
            if (esl_is_unassociate_pending()) {
                esl_execute_unassociate_cleanup();
            }
        }
    } else {
        /* No response expected — stop the ECP timeout timer */
        esp_timer_stop(s_esl_gatts->ctx->ecp_timer);
    }

    return 0;
}

/**
 * @brief Handle read of Display Information characteristic
 */
static int handle_read_display_info(struct ble_gatt_access_ctxt *ctxt)
{
    if (s_esl_gatts->ctx->config.display_info == NULL || s_esl_gatts->ctx->config.num_displays == 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    uint16_t len = (uint16_t)(s_esl_gatts->ctx->config.num_displays * sizeof(ble_esl_display_info_t));
    int rc = os_mbuf_append(ctxt->om, s_esl_gatts->ctx->config.display_info, len);
    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

/**
 * @brief Handle read of Image Information characteristic
 */
static int handle_read_image_info(struct ble_gatt_access_ctxt *ctxt)
{
    if (s_esl_gatts->ctx->config.num_images == 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    uint8_t max_image_index = s_esl_gatts->ctx->config.num_images - 1;
    int rc = os_mbuf_append(ctxt->om, &max_image_index, 1);
    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

/**
 * @brief Handle read of Sensor Information characteristic
 */
static int handle_read_sensor_info(struct ble_gatt_access_ctxt *ctxt)
{
    if (s_esl_gatts->ctx->config.sensor_info == NULL || s_esl_gatts->ctx->config.sensor_info_len == 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    int rc = os_mbuf_append(ctxt->om, s_esl_gatts->ctx->config.sensor_info,
                            s_esl_gatts->ctx->config.sensor_info_len);
    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

/**
 * @brief Handle read of LED Information characteristic
 */
static int handle_read_led_info(struct ble_gatt_access_ctxt *ctxt)
{
    if (s_esl_gatts->ctx->config.led_info == NULL || s_esl_gatts->ctx->config.num_leds == 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    int rc = os_mbuf_append(ctxt->om, s_esl_gatts->ctx->config.led_info,
                            s_esl_gatts->ctx->config.num_leds);
    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

/**
 * @brief GATT access callback for all ESL Service characteristics
 */
static int esl_gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (s_esl_gatts == NULL || s_esl_gatts->ctx == NULL) {
        ESP_LOGW(TAG, "GATT access while context is NULL");
        return BLE_ATT_ERR_UNLIKELY;
    }

    const ble_uuid_t *uuid;

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        uuid = ctxt->chr->uuid;
        if (ble_uuid_cmp(uuid, &s_uuid_address.u) == 0 ||
            ble_uuid_cmp(uuid, &s_uuid_ap_sync.u) == 0 ||
            ble_uuid_cmp(uuid, &s_uuid_resp_key.u) == 0 ||
            ble_uuid_cmp(uuid, &s_uuid_abs_time.u) == 0) {
            /* ESL spec: configurable characteristics may only be written
             * in Configuring or Updating states */
            if (s_esl_gatts->ctx->state != BLE_ESL_STATE_CONFIGURING &&
                s_esl_gatts->ctx->state != BLE_ESL_STATE_UPDATING) {
                ESP_LOGW(TAG, "GATT write rejected: invalid state %d", s_esl_gatts->ctx->state);
                return BLE_ATT_ERR_WRITE_NOT_PERMITTED;
            }
        }
        if (ble_uuid_cmp(uuid, &s_uuid_address.u) == 0) {
            return handle_write_esl_address(ctxt->om);
        }
        if (ble_uuid_cmp(uuid, &s_uuid_ap_sync.u) == 0) {
            return handle_write_ap_sync_key(ctxt->om);
        }
        if (ble_uuid_cmp(uuid, &s_uuid_resp_key.u) == 0) {
            return handle_write_resp_key(ctxt->om);
        }
        if (ble_uuid_cmp(uuid, &s_uuid_abs_time.u) == 0) {
            return handle_write_abs_time(ctxt->om);
        }
        if (ble_uuid_cmp(uuid, &s_uuid_ecp.u) == 0) {
            return handle_write_ecp(conn_handle, ctxt->om);
        }
        break;

    case BLE_GATT_ACCESS_OP_READ_CHR:
        uuid = ctxt->chr->uuid;
        if (ble_uuid_cmp(uuid, &s_uuid_disp_info.u) == 0) {
            return handle_read_display_info(ctxt);
        }
        if (ble_uuid_cmp(uuid, &s_uuid_img_info.u) == 0) {
            return handle_read_image_info(ctxt);
        }
        if (ble_uuid_cmp(uuid, &s_uuid_sensor_info.u) == 0) {
            return handle_read_sensor_info(ctxt);
        }
        if (ble_uuid_cmp(uuid, &s_uuid_led_info.u) == 0) {
            return handle_read_led_info(ctxt);
        }
        break;

    default:
        break;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

/* ========================== OTS Setup ========================== */

#if CONFIG_BLE_ESL_OTS_SUPPORT

/* OTS object-ID reverse map, count and init state live in esl_gatts_ctx_t. */

/**
 * @brief OTS event callback — handles write-complete events,
 *        maps object ID to image index and notifies app
 */
static void esl_ots_write_cb(ble_ots_server_event_t event,
                              ble_ots_server_cb_param_t *param)
{
    if (event != BLE_OTS_SERVER_EVT_WRITE_COMPLETE || param == NULL) {
        return;
    }

    ble_ots_obj_id_t obj_id = param->write_complete.object_id;
    uint32_t offset = param->write_complete.offset;
    uint32_t length = param->write_complete.bytes_received;

    if (s_esl_gatts == NULL || s_esl_gatts->ctx == NULL) {
        return;
    }
    /* Map obj_id back to image_index through the reverse map built by
     * esl_setup_ots(). The OTS server assigns IDs from its own monotonic
     * counter, so they are not guaranteed to start at
     * BLE_ESL_OTS_OBJECT_ID_BASE nor to be contiguous. */
    uint8_t image_index = 0;
    bool found = false;
    for (uint8_t i = 0; i < s_esl_gatts->ots_obj_count; i++) {
        if (s_esl_gatts->ots_obj_ids[i] == obj_id) {
            image_index = i;
            found = true;
            break;
        }
    }
    if (!found) {
        ESP_LOGW(TAG, "OTS write: unknown obj_id=0x%06llx", (unsigned long long)obj_id);
        return;
    }

    ble_esl_cb_param_t cb_param = {
        .image_write = {
            .image_index = image_index,
            .data = NULL,
            .length = length,
            .offset = offset,
        }
    };
    esl_notify_app(BLE_ESL_EVT_IMAGE_WRITE, &cb_param);
}

static esp_err_t esl_setup_ots(const ble_esl_config_t *config)
{
    if (config->num_images == 0) {
        return ESP_OK;
    }

    s_esl_gatts->ots_obj_count = 0;

    ble_ots_server_config_t ots_config = {
        .oacp_features = BLE_OTS_OACP_FEAT_WRITE | BLE_OTS_OACP_FEAT_TRUNCATE,
        .olcp_features = BLE_OTS_OLCP_FEAT_GO_TO,
        .supported_types = NULL,
        .num_supported_types = 0,
        .include_obj_first_created = false,
        .include_obj_last_modified = false,
        .include_obj_changed = false,
        .include_obj_list_filter = false,
        .has_realtime_clock = false,
        .obj_name_writable = false,
        .obj_properties_writable = false,
    };

    int rc = ble_ots_server_init(&ots_config);
    if (rc != 0) {
        ESP_LOGE(TAG, "OTS server init failed: rc=%d", rc);
        return ESP_FAIL;
    }

    s_esl_gatts->ots_initialized = true;

    /* Pre-create objects for each image slot */
    for (uint8_t i = 0; i < config->num_images; i++) {
        char name[16];
        snprintf(name, sizeof(name), "Image_%u", i);

        uint32_t props = BLE_OTS_OBJ_PROP_READ;
        if (config->image_writable_mask != NULL && config->image_writable_mask[i]) {
            props |= BLE_OTS_OBJ_PROP_WRITE | BLE_OTS_OBJ_PROP_TRUNCATE;
        }

        ble_ots_obj_type_entry_t obj_type = { .uuid_len = 2, .uuid = {0} };
        ble_ots_server_obj_params_t obj_params = {
            .name = name,
            .name_len = strlen(name),
            .type = obj_type,
            .properties = props,
            .first_created = {0},
            .last_modified = {0},
            .data = NULL,
            .data_len = 0,
            .allocated_size = CONFIG_BLE_ESL_MAX_IMAGE_SIZE,
        };

        ble_ots_obj_id_t obj_id;
        rc = ble_ots_server_add_object(&obj_params, &obj_id);
        if (rc != 0) {
            ESP_LOGE(TAG, "OTS add object %u failed: rc=%d", i, rc);
            return ESP_FAIL;
        }
        if (s_esl_gatts->ots_obj_count < CONFIG_BLE_ESL_MAX_IMAGES) {
            s_esl_gatts->ots_obj_ids[s_esl_gatts->ots_obj_count++] = obj_id;
        }
        ESP_LOGI(TAG, "OTS object created: Image_%u, obj_id=0x%06llx",
                 i, (unsigned long long)obj_id);
    }

    /* Register write callback for image data reception */
    rc = ble_ots_server_register_cb(esl_ots_write_cb);
    if (rc != 0) {
        ESP_LOGE(TAG, "OTS register write cb failed: rc=%d", rc);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void esl_teardown_ots(void)
{
    if (s_esl_gatts->ots_initialized) {
        ble_ots_server_deinit();
        s_esl_gatts->ots_initialized = false;
    }
}
#endif /* CONFIG_BLE_ESL_OTS_SUPPORT */

/* ========================== DIS PnP ID Setup ========================== */

#if CONFIG_BLE_ESL_VENDOR_SPECIFIC
/**
 * @brief Static buffer for PnP ID to work around NimBLE DIS strlen limitation.
 *
 * NimBLE's ble_svc_dis uses strlen() on the PnP ID pointer, which truncates
 * binary data containing 0x00 bytes. We copy the 7-octet PnP ID into a static
 * buffer padded with 0xFF to avoid premature truncation by strlen, and cap at
 * 7 bytes on the read side (NimBLE clamps len to 7).
 */
static uint8_t s_pnp_id_buf[8]; /* 7 octets + padding byte */

static esp_err_t esl_setup_dis_pnp(const ble_esl_config_t *config)
{
    if (!config->vendor_specific || config->pnp_id == NULL) {
        return ESP_OK;
    }

    ble_svc_dis_init();

    /* Copy 7-octet binary PnP ID and pad with non-zero byte to prevent
     * strlen from reading past the buffer. Note: if any of the 7 data
     * bytes is 0x00, NimBLE's strlen-based read will still truncate the
     * value. This is a known NimBLE limitation for binary characteristics. */
    memcpy(s_pnp_id_buf, config->pnp_id, 7);
    s_pnp_id_buf[7] = 0xFF; /* non-zero padding to bound strlen */

    /* Check for embedded zero bytes and warn */
    for (int i = 0; i < 7; i++) {
        if (s_pnp_id_buf[i] == 0x00) {
            ESP_LOGW(TAG, "PnP ID contains 0x00 at offset %d — NimBLE DIS may "
                     "truncate the value due to strlen usage", i);
            break;
        }
    }

    int rc = ble_svc_dis_pnp_id_set((const char *)s_pnp_id_buf);
    if (rc != 0) {
        ESP_LOGE(TAG, "DIS PnP ID set failed: rc=%d", rc);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "DIS PnP ID registered");
    return ESP_OK;
}
#endif /* CONFIG_BLE_ESL_VENDOR_SPECIFIC */

/* ========================== ECP Response ========================== */

/*
 * The ECP response notification must not be sent synchronously from within a
 * GATT access callback (the ATT Write Response for the ECP write has not yet
 * been transmitted). The notification mbuf is built up-front (the caller's
 * TLV buffer may live on the stack) and the actual ble_gatts_notify_custom()
 * call is deferred to a NimBLE host-task event.
 */
struct esl_ecp_notify_ctx {
    struct ble_npl_event ev;
    uint16_t conn_handle;
    uint16_t val_handle;
    struct os_mbuf *om;
};

static void esl_ecp_notify_event_cb(struct ble_npl_event *ev)
{
    struct esl_ecp_notify_ctx *ctx =
        (struct esl_ecp_notify_ctx *)ble_npl_event_get_arg(ev);
    if (ctx == NULL) {
        return;
    }

    /* ble_gatts_notify_custom() consumes the mbuf on all return paths. */
    int rc = ble_gatts_notify_custom(ctx->conn_handle, ctx->val_handle, ctx->om);
    if (rc != 0) {
        ESP_LOGW(TAG, "ECP response: notify failed rc=%d", rc);
        if (esl_is_unassociate_pending()) {
            esl_execute_unassociate_cleanup();
        }
    }
    ble_npl_event_deinit(ev);
    free(ctx);
}

void esl_handle_ecp_notify_tx(uint16_t conn_handle, uint16_t attr_handle,
                              int status, uint8_t indication)
{
    if (s_esl_gatts == NULL || s_esl_gatts->ctx == NULL || indication ||
        s_esl_gatts->ctx->conn_handle != conn_handle ||
        attr_handle != s_esl_gatts->chr_handles[s_esl_gatts->ecp_chr_idx] ||
        !esl_is_unassociate_pending()) {
        return;
    }

    if (status != 0) {
        ESP_LOGW(TAG, "ECP response notify-tx failed: status=%d", status);
    }

    esl_execute_unassociate_cleanup();
}

esp_err_t esl_send_ecp_response(const uint8_t *response_tlv, uint8_t response_len)
{
    if (s_esl_gatts == NULL || s_esl_gatts->ctx == NULL ||
        response_tlv == NULL || response_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Stop ECP procedure timeout — response is being sent */
    esp_timer_stop(s_esl_gatts->ctx->ecp_timer);

    if (s_esl_gatts->ctx->conn_handle == BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGW(TAG, "ECP response: no active connection");
        return ESP_FAIL;
    }

    struct os_mbuf *om = ble_hs_mbuf_from_flat(response_tlv, response_len);
    if (om == NULL) {
        ESP_LOGE(TAG, "ECP response: mbuf alloc failed");
        return ESP_ERR_NO_MEM;
    }

    /* Defer the actual notification to a NimBLE host-task event so it is not
     * sent from within a GATT access callback. */
    struct esl_ecp_notify_ctx *ctx = malloc(sizeof(*ctx));
    if (ctx == NULL) {
        os_mbuf_free_chain(om);
        ESP_LOGE(TAG, "ECP response: event context alloc failed");
        return ESP_ERR_NO_MEM;
    }
    ctx->conn_handle = s_esl_gatts->ctx->conn_handle;
    ctx->val_handle = s_esl_gatts->chr_handles[s_esl_gatts->ecp_chr_idx];
    ctx->om = om;
    ble_npl_event_init(&ctx->ev, esl_ecp_notify_event_cb, ctx);
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(), &ctx->ev);

    return ESP_OK;
}

esp_err_t esl_gatts_init(const ble_esl_config_t *config)
{
    esp_err_t err;

    if (s_esl_gatts != NULL) {
        ESP_LOGW(TAG, "esl_gatts already initialized");
        return ESP_FAIL;
    }

    /* Allocate the module context once. NimBLE will retain pointers into the
     * GATT tables held here, so it must persist until esl_gatts_deinit(). */
    s_esl_gatts = calloc(1, sizeof(*s_esl_gatts));
    if (s_esl_gatts == NULL) {
        ESP_LOGE(TAG, "Failed to allocate esl_gatts context");
        return ESP_ERR_NO_MEM;
    }
    s_esl_gatts->ecp_chr_idx = CHR_IDX_ECP;

    s_esl_gatts->ctx = esl_state_ctx_get();
    if (s_esl_gatts->ctx == NULL) {
        free(s_esl_gatts);
        s_esl_gatts = NULL;
        return ESP_FAIL;
    }

    /* Build and register GATT service */
    esl_build_gatt_svcs(config);

    int rc = ble_gatts_count_cfg(s_esl_gatts->esl_svcs);
    if (rc != 0) {
        free(s_esl_gatts);
        s_esl_gatts = NULL;
        return ESP_FAIL;
    }

    rc = ble_gatts_add_svcs(s_esl_gatts->esl_svcs);
    if (rc != 0) {
        free(s_esl_gatts);
        s_esl_gatts = NULL;
        return ESP_FAIL;
    }

#if CONFIG_BLE_ESL_OTS_SUPPORT
    err = esl_setup_ots(config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "OTS setup failed");
        /* Leave the module in its pre-init state: drop any partial OTS state
         * and the already-registered GATT service. */
        esl_teardown_ots();
        ble_gatts_delete_svc(&s_esl_svc_uuid.u);
        free(s_esl_gatts);
        s_esl_gatts = NULL;
        return err;
    }
#endif

#if CONFIG_BLE_ESL_VENDOR_SPECIFIC
    err = esl_setup_dis_pnp(config);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "DIS PnP setup failed (non-fatal)");
    }
#endif

    return ESP_OK;
}

esp_err_t esl_gatts_deinit(void)
{
    if (s_esl_gatts == NULL) {
        return ESP_OK;
    }

#if CONFIG_BLE_ESL_OTS_SUPPORT
    esl_teardown_ots();
#endif

    /* Remove GATT service */
    ble_gatts_delete_svc(&s_esl_svc_uuid.u);

    free(s_esl_gatts);
    s_esl_gatts = NULL;

    return ESP_OK;
}
