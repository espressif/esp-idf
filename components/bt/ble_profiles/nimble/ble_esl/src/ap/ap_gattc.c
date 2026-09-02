/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ap_gattc.c
 * @brief ESL AP — GATT client: service/characteristic discovery and
 *        internal GATT read/write/ECP helpers.
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"

#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"

#include "ble_esl_ap.h"
#include "ble_esl_ap_int.h"
#include "ble_ots_client.h"

static const char *TAG = "esl_ap_gattc";

/* ========================== Forward Declarations ========================== */
/* Discovery state machine steps */
static int disc_svc_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                       const struct ble_gatt_svc *service, void *arg);
static void disc_start_esl_chr_discovery(uint16_t conn_handle);
static int disc_esl_chr_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                           const struct ble_gatt_chr *chr, void *arg);
static void disc_start_dis_chr_discovery(uint16_t conn_handle);
static int disc_dis_chr_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                           const struct ble_gatt_chr *chr, void *arg);
static void disc_start_ecp_dsc_discovery(uint16_t conn_handle);
static int disc_ecp_dsc_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                           uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                           void *arg);
static void disc_enable_ecp_notifications(uint16_t conn_handle);
static int disc_cccd_write_cb(uint16_t conn_handle,
                              const struct ble_gatt_error *error,
                              struct ble_gatt_attr *attr, void *arg);
static void disc_start_ots(uint16_t conn_handle);
static void disc_complete(uint16_t conn_handle, bool success);

/* GATT operation callbacks */
static int gatt_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr, void *arg);
static int gatt_read_long_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                             struct ble_gatt_attr *attr, void *arg);
static int gatt_write_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                         struct ble_gatt_attr *attr, void *arg);
static int ecp_write_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr, void *arg);

/* Characteristic handle resolver */
static uint16_t resolve_char_handle(ble_esl_ap_conn_t *conn, uint16_t char_uuid);

/* ========================== OTS Client Callback ========================== */

static void ble_esl_ap_ots_event_cb(uint16_t conn_id,
                                    ble_ots_client_event_t event,
                                    const void *param)
{
    if (event == BLE_OTS_CLIENT_EVT_DISCOVER_COMPLETE) {
        const ble_ots_client_discover_complete_t *d = param;
        ESP_LOGI(TAG, "OTS discovery %s; conn_id=%d",
                 d->status == 0 ? "complete" : "failed", conn_id);
        disc_complete(conn_id, d->status == 0);
        return;
    }

    ble_esl_ap_lifecycle_handle_ots_event(conn_id, (int)event, param);
}

/* ========================== Service Discovery State Machine ========================== */

/**
 * @brief Step 1: Discover all primary services
 */
void ble_esl_ap_start_svc_discovery(uint16_t conn_handle)
{
    ESP_LOGD(TAG, "Starting service discovery; conn_handle=%d", conn_handle);

    int rc = ble_gattc_disc_all_svcs(conn_handle, disc_svc_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start service discovery; rc=%d", rc);
        disc_complete(conn_handle, false);
    }
}

/**
 * @brief Service discovery callback — collects ESL Service and DIS handles
 */
static int disc_svc_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                       const struct ble_gatt_svc *service, void *arg)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return BLE_HS_ENOTCONN;
    }

    if (error->status == 0 && service != NULL) {
        uint16_t uuid16 = ble_uuid_u16(&service->uuid.u);

        if (uuid16 == BLE_ESL_SVC_UUID) {
            conn->handles.esl_svc_start = service->start_handle;
            conn->handles.esl_svc_end = service->end_handle;
            ESP_LOGD(TAG, "Found ESL Service: start=%d end=%d",
                     service->start_handle, service->end_handle);
        } else if (uuid16 == BLE_ESL_AP_DIS_UUID) {
            conn->handles.dis_svc_start = service->start_handle;
            conn->handles.dis_svc_end = service->end_handle;
            ESP_LOGD(TAG, "Found DIS: start=%d end=%d",
                     service->start_handle, service->end_handle);
        } else if (uuid16 == BLE_OTS_UUID_OTS_SERVICE) {
            conn->has_ots = true;
            ESP_LOGD(TAG, "Found OTS Service: start=%d end=%d",
                     service->start_handle, service->end_handle);
        }
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        /* Service discovery complete — check mandatory ESL Service */
        if (conn->handles.esl_svc_start == 0) {
            ESP_LOGE(TAG, "ESL Service not found; disconnecting");
            disc_complete(conn_handle, false);
            return 0;
        }
        /* Proceed to ESL characteristic discovery */
        disc_start_esl_chr_discovery(conn_handle);
        return 0;
    }

    /* Error during discovery */
    ESP_LOGE(TAG, "Service discovery error; status=%d", error->status);
    disc_complete(conn_handle, false);
    return 0;
}

/**
 * @brief Step 2: Discover all characteristics within the ESL Service
 */
static void disc_start_esl_chr_discovery(uint16_t conn_handle)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return;
    }

    ESP_LOGD(TAG, "Discovering ESL Service characteristics; conn_handle=%d", conn_handle);

    int rc = ble_gattc_disc_all_chrs(conn_handle,
                                     conn->handles.esl_svc_start,
                                     conn->handles.esl_svc_end,
                                     disc_esl_chr_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start ESL chr discovery; rc=%d", rc);
        disc_complete(conn_handle, false);
    }
}

/**
 * @brief ESL Service characteristic discovery callback
 */
static int disc_esl_chr_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                           const struct ble_gatt_chr *chr, void *arg)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return BLE_HS_ENOTCONN;
    }

    if (error->status == 0 && chr != NULL) {
        uint16_t uuid16 = ble_uuid_u16(&chr->uuid.u);

        switch (uuid16) {
        case BLE_ESL_CHR_UUID_ESL_ADDRESS:
            conn->handles.esl_addr_handle = chr->val_handle;
            break;
        case BLE_ESL_CHR_UUID_AP_SYNC_KEY:
            conn->handles.ap_sync_key_handle = chr->val_handle;
            break;
        case BLE_ESL_CHR_UUID_RESP_KEY:
            conn->handles.resp_key_handle = chr->val_handle;
            break;
        case BLE_ESL_CHR_UUID_CURRENT_ABS_TIME:
            conn->handles.abs_time_handle = chr->val_handle;
            break;
        case BLE_ESL_CHR_UUID_DISPLAY_INFO:
            conn->handles.display_info_handle = chr->val_handle;
            break;
        case BLE_ESL_CHR_UUID_IMAGE_INFO:
            conn->handles.image_info_handle = chr->val_handle;
            break;
        case BLE_ESL_CHR_UUID_SENSOR_INFO:
            conn->handles.sensor_info_handle = chr->val_handle;
            break;
        case BLE_ESL_CHR_UUID_LED_INFO:
            conn->handles.led_info_handle = chr->val_handle;
            break;
        case BLE_ESL_CHR_UUID_ECP:
            conn->handles.ecp_handle = chr->val_handle;
            break;
        default:
            break;
        }
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        /* Verify mandatory characteristics */
        if (conn->handles.esl_addr_handle == 0 ||
            conn->handles.ap_sync_key_handle == 0 ||
            conn->handles.resp_key_handle == 0 ||
            conn->handles.abs_time_handle == 0 ||
            conn->handles.ecp_handle == 0) {
            ESP_LOGE(TAG, "Missing mandatory ESL characteristics; disconnecting");
            disc_complete(conn_handle, false);
            return 0;
        }

        /* Proceed to DIS characteristic discovery or ECP descriptor discovery */
        if (conn->handles.dis_svc_start != 0) {
            disc_start_dis_chr_discovery(conn_handle);
        } else {
            disc_start_ecp_dsc_discovery(conn_handle);
        }
        return 0;
    }

    ESP_LOGE(TAG, "ESL chr discovery error; status=%d", error->status);
    disc_complete(conn_handle, false);
    return 0;
}

/**
 * @brief Step 3: Discover DIS characteristics (PnP ID)
 */
static void disc_start_dis_chr_discovery(uint16_t conn_handle)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return;
    }

    ESP_LOGD(TAG, "Discovering DIS characteristics; conn_handle=%d", conn_handle);

    int rc = ble_gattc_disc_all_chrs(conn_handle,
                                     conn->handles.dis_svc_start,
                                     conn->handles.dis_svc_end,
                                     disc_dis_chr_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start DIS chr discovery; rc=%d", rc);
        /* DIS is optional; proceed to ECP descriptor discovery */
        disc_start_ecp_dsc_discovery(conn_handle);
    }
}

/**
 * @brief DIS characteristic discovery callback
 */
static int disc_dis_chr_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                           const struct ble_gatt_chr *chr, void *arg)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return BLE_HS_ENOTCONN;
    }

    if (error->status == 0 && chr != NULL) {
        uint16_t uuid16 = ble_uuid_u16(&chr->uuid.u);
        if (uuid16 == BLE_ESL_AP_PNP_ID_UUID) {
            conn->handles.pnp_id_handle = chr->val_handle;
            ESP_LOGD(TAG, "Found PnP ID characteristic; handle=%d", chr->val_handle);
        }
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        /* Proceed to ECP descriptor discovery */
        disc_start_ecp_dsc_discovery(conn_handle);
        return 0;
    }

    ESP_LOGW(TAG, "DIS chr discovery error; status=%d", error->status);
    /* DIS is optional; proceed anyway */
    disc_start_ecp_dsc_discovery(conn_handle);
    return 0;
}

/**
 * @brief Step 4: Discover ECP CCCD descriptor
 */
static void disc_start_ecp_dsc_discovery(uint16_t conn_handle)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return;
    }

    ESP_LOGD(TAG, "Discovering ECP descriptors; conn_handle=%d", conn_handle);

    /* Discover descriptors between ECP value handle and ESL service end handle.
     * ble_gattc_disc_all_dscs expects the characteristic value handle as
     * start_handle; it internally starts the Find Information Request at
     * start_handle + 1, so passing ecp_handle ensures the CCCD at
     * ecp_handle + 1 is found. */
    uint16_t start = conn->handles.ecp_handle;
    uint16_t end = conn->handles.esl_svc_end;

    if (start >= end) {
        ESP_LOGE(TAG, "Invalid ECP descriptor range");
        disc_complete(conn_handle, false);
        return;
    }

    int rc = ble_gattc_disc_all_dscs(conn_handle, start, end,
                                     disc_ecp_dsc_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start ECP dsc discovery; rc=%d", rc);
        disc_complete(conn_handle, false);
    }
}

/**
 * @brief ECP descriptor discovery callback — looks for CCCD
 */
static int disc_ecp_dsc_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                           uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                           void *arg)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return BLE_HS_ENOTCONN;
    }

    if (error->status == 0 && dsc != NULL) {
        uint16_t uuid16 = ble_uuid_u16(&dsc->uuid.u);
        if (uuid16 == BLE_GATT_DSC_CLT_CFG_UUID16) {
            conn->handles.ecp_cccd_handle = dsc->handle;
            ESP_LOGD(TAG, "Found ECP CCCD; handle=%d", dsc->handle);
        }
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        if (conn->handles.ecp_cccd_handle == 0) {
            ESP_LOGE(TAG, "ECP CCCD not found; disconnecting");
            disc_complete(conn_handle, false);
            return 0;
        }
        /* Enable notifications on ECP */
        disc_enable_ecp_notifications(conn_handle);
        return 0;
    }

    ESP_LOGE(TAG, "ECP dsc discovery error; status=%d", error->status);
    disc_complete(conn_handle, false);
    return 0;
}

/**
 * @brief Step 5: Write CCCD to enable ECP notifications
 */
static void disc_enable_ecp_notifications(uint16_t conn_handle)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return;
    }

    ESP_LOGD(TAG, "Enabling ECP notifications; conn_handle=%d", conn_handle);

    uint8_t value[2] = {1, 0}; /* Enable notifications */
    int rc = ble_gattc_write_flat(conn_handle, conn->handles.ecp_cccd_handle,
                                  value, sizeof(value),
                                  disc_cccd_write_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to write ECP CCCD; rc=%d", rc);
        disc_complete(conn_handle, false);
    }
}

/**
 * @brief CCCD write completion callback
 */
static int disc_cccd_write_cb(uint16_t conn_handle,
                              const struct ble_gatt_error *error,
                              struct ble_gatt_attr *attr, void *arg)
{
    if (error->status != 0) {
        ESP_LOGE(TAG, "CCCD write failed; status=%d", error->status);
        disc_complete(conn_handle, false);
        return 0;
    }

    ESP_LOGD(TAG, "ECP notifications enabled; conn_handle=%d", conn_handle);
    disc_start_ots(conn_handle);
    return 0;
}

/**
 * @brief Step 6: Start OTS service discovery if the tag exposes OTS,
 *        otherwise go straight to disc_complete.
 *
 * BLE_OTS_CLIENT_EVT_DISCOVER_COMPLETE (intercepted in ble_esl_ap_ots_event_cb)
 * will call disc_complete once OTS discovery finishes.
 */
static void disc_start_ots(uint16_t conn_handle)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return;
    }

    if (!conn->has_ots) {
        disc_complete(conn_handle, true);
        return;
    }

    ESP_LOGD(TAG, "Starting OTS discovery; conn_handle=%d", conn_handle);
    int rc = ble_ots_client_discover_service(conn_handle);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start OTS discovery; rc=%d", rc);
        disc_complete(conn_handle, false);
    }
    /* On success: wait for BLE_OTS_CLIENT_EVT_DISCOVER_COMPLETE */
}

/**
 * @brief Discovery complete — fire connected or disconnected event
 */
static void disc_complete(uint16_t conn_handle, bool success)
{
    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return;
    }

    if (!success) {
        /* Disconnect and let the disconnect handler fire the event */
        ESP_LOGW(TAG, "Discovery failed; disconnecting conn_handle=%d", conn_handle);
        ble_gap_terminate(conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        return;
    }

    conn->disc_done = true;

    /* Build connected event info */
    if (g_esl_ap && g_esl_ap->app_cb) {
        ble_esl_ap_conn_info_t info;
        memset(&info, 0, sizeof(info));
        info.conn_handle = conn_handle;
        memcpy(info.addr, conn->addr, 6);
        info.addr_type = conn->addr_type;
        info.has_display_info = (conn->handles.display_info_handle != 0);
        info.has_image_info = (conn->handles.image_info_handle != 0);
        info.has_sensor_info = (conn->handles.sensor_info_handle != 0);
        info.has_led_info = (conn->handles.led_info_handle != 0);
        info.has_dis = (conn->handles.dis_svc_start != 0);
        info.has_pnp_id = (conn->handles.pnp_id_handle != 0);

        ESP_LOGI(TAG, "Discovery complete; conn_handle=%d display=%d image=%d "
                 "sensor=%d led=%d dis=%d pnp=%d",
                 conn_handle, info.has_display_info, info.has_image_info,
                 info.has_sensor_info, info.has_led_info, info.has_dis,
                 info.has_pnp_id);

        g_esl_ap->app_cb(BLE_ESL_AP_EVT_CONNECTED, &info);
    }
}

/* ========================== Characteristic Handle Resolver ========================== */

/**
 * @brief Resolve a 16-bit characteristic UUID to its discovered attribute handle
 *
 * @param conn      Connection context
 * @param char_uuid 16-bit UUID
 * @return Attribute handle, or 0 if not found
 */
static uint16_t resolve_char_handle(ble_esl_ap_conn_t *conn, uint16_t char_uuid)
{
    if (!conn) {
        return 0;
    }

    switch (char_uuid) {
    case BLE_ESL_CHR_UUID_ESL_ADDRESS:
        return conn->handles.esl_addr_handle;
    case BLE_ESL_CHR_UUID_AP_SYNC_KEY:
        return conn->handles.ap_sync_key_handle;
    case BLE_ESL_CHR_UUID_RESP_KEY:
        return conn->handles.resp_key_handle;
    case BLE_ESL_CHR_UUID_CURRENT_ABS_TIME:
        return conn->handles.abs_time_handle;
    case BLE_ESL_CHR_UUID_DISPLAY_INFO:
        return conn->handles.display_info_handle;
    case BLE_ESL_CHR_UUID_IMAGE_INFO:
        return conn->handles.image_info_handle;
    case BLE_ESL_CHR_UUID_SENSOR_INFO:
        return conn->handles.sensor_info_handle;
    case BLE_ESL_CHR_UUID_LED_INFO:
        return conn->handles.led_info_handle;
    case BLE_ESL_CHR_UUID_ECP:
        return conn->handles.ecp_handle;
    case BLE_ESL_AP_PNP_ID_UUID:
        return conn->handles.pnp_id_handle;
    default:
        return 0;
    }
}

/* ========================== Internal GATT Helpers ========================== */

/**
 * @brief Context passed through GATT read callbacks
 */
typedef struct {
    ble_esl_ap_gatt_cb_t cb;
    void *user_data;
    uint16_t attr_handle;
    bool use_read_long;
} gatt_read_ctx_t;

/**
 * @brief Callback for ble_gattc_read (single-shot read)
 *
 * If the read value fills the entire ATT_MTU-1, we switch to Read Long
 * to get the complete value.
 */
static int gatt_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr, void *arg)
{
    gatt_read_ctx_t *ctx = (gatt_read_ctx_t *)arg;
    if (!ctx) {
        return 0;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);

    if (error->status != 0) {
        /* Read failed */
        if (conn) {
            conn->read_pending = false;
        }
        if (ctx->cb) {
            ctx->cb(conn_handle, ESP_FAIL, NULL, 0, ctx->user_data);
        }
        free(ctx);
        return 0;
    }

    /* Copy data from mbuf */
    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);

    if (conn && data_len > 0) {
        uint16_t copy_len = data_len;
        if (copy_len > BLE_ESL_AP_READ_BUF_MAX) {
            copy_len = BLE_ESL_AP_READ_BUF_MAX;
        }
        int rc = os_mbuf_copydata(attr->om, 0, copy_len, conn->read_buf);
        if (rc != 0) {
            conn->read_pending = false;
            if (ctx->cb) {
                ctx->cb(conn_handle, ESP_FAIL, NULL, 0, ctx->user_data);
            }
            free(ctx);
            return 0;
        }
        conn->read_offset = copy_len;

        /* Check if we need Read Long: only if the data fills exactly
         * ATT_MTU-1 bytes (indicating more data may be available) and
         * we still have buffer space. Use the negotiated MTU dynamically. */
        uint16_t mtu = ble_att_mtu(conn_handle);
        if (mtu > 0 && data_len == (mtu - 1) &&
            conn->read_offset < BLE_ESL_AP_READ_BUF_MAX) {
            /* Switch to Read Long to get the rest */
            int rl_rc = ble_gattc_read_long(conn_handle, ctx->attr_handle,
                                            copy_len, gatt_read_long_cb, ctx);
            if (rl_rc == 0) {
                return 0; /* ctx ownership transferred to read_long_cb */
            }
            /* If Read Long fails to start, deliver what we have */
        }

        /* Deliver the data we have */
        conn->read_pending = false;
        if (ctx->cb) {
            ctx->cb(conn_handle, ESP_OK, conn->read_buf, conn->read_offset,
                    ctx->user_data);
        }
    } else {
        /* No data or no connection */
        if (conn) {
            conn->read_pending = false;
        }
        if (ctx->cb) {
            ctx->cb(conn_handle, ESP_OK, NULL, 0, ctx->user_data);
        }
    }

    free(ctx);
    return 0;
}

/**
 * @brief Callback for ble_gattc_read_long — called multiple times per fragment,
 *        then once with error->status == BLE_HS_EDONE on completion.
 */
static int gatt_read_long_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                             struct ble_gatt_attr *attr, void *arg)
{
    gatt_read_ctx_t *ctx = (gatt_read_ctx_t *)arg;
    if (!ctx) {
        return 0;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);

    if (error->status == 0 && attr != NULL) {
        /* Fragment received — append to read buffer */
        if (conn) {
            uint16_t frag_len = OS_MBUF_PKTLEN(attr->om);
            uint16_t space = BLE_ESL_AP_READ_BUF_MAX - conn->read_offset;
            uint16_t copy_len = (frag_len < space) ? frag_len : space;

            if (copy_len > 0) {
                int rc = os_mbuf_copydata(attr->om, 0, copy_len,
                                          conn->read_buf + conn->read_offset);
                if (rc == 0) {
                    conn->read_offset += copy_len;
                }
            }
        }
        return 0; /* More fragments may follow */
    }

    if (error->status == BLE_HS_EDONE) {
        /* Read Long complete — deliver full value */
        if (conn) {
            conn->read_pending = false;
        }
        if (ctx->cb && conn) {
            ctx->cb(conn_handle, ESP_OK, conn->read_buf, conn->read_offset,
                    ctx->user_data);
        } else if (ctx->cb) {
            /* Connection lost — report failure */
            ctx->cb(conn_handle, ESP_FAIL, NULL, 0, ctx->user_data);
        }
        free(ctx);
        return 0;
    }

    /* Error — clear read_pending */
    if (conn) {
        conn->read_pending = false;
    }
    if (conn && conn->read_offset > 0 &&
        (error->status == BLE_HS_ATT_ERR(BLE_ATT_ERR_INVALID_OFFSET) ||
         error->status == BLE_HS_ATT_ERR(BLE_ATT_ERR_ATTR_NOT_LONG))) {
        /* ATT error indicates end-of-attribute — deliver partial data as complete */
        if (ctx->cb) {
            ctx->cb(conn_handle, ESP_OK, conn->read_buf, conn->read_offset,
                    ctx->user_data);
        }
    } else {
        /* Real GATT error (timeout, disconnect, etc.) — propagate failure */
        if (ctx->cb) {
            ctx->cb(conn_handle, ESP_FAIL, NULL, 0, ctx->user_data);
        }
    }
    free(ctx);
    return 0;
}

esp_err_t ble_esl_ap_gatt_read(uint16_t conn_handle, uint16_t char_uuid,
                               ble_esl_ap_gatt_cb_t cb, void *user_data)
{
    if (!g_esl_ap || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t attr_handle = resolve_char_handle(conn, char_uuid);
    if (attr_handle == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    /* Prevent concurrent GATT reads on the same connection */
    if (conn->read_pending) {
        ESP_LOGW(TAG, "GATT read already in progress on conn_handle=%d", conn_handle);
        return ESP_ERR_INVALID_STATE;
    }

    /* Allocate read context */
    gatt_read_ctx_t *ctx = calloc(1, sizeof(gatt_read_ctx_t));
    if (!ctx) {
        return ESP_ERR_NO_MEM;
    }
    ctx->cb = cb;
    ctx->user_data = user_data;
    ctx->attr_handle = attr_handle;
    ctx->use_read_long = false;

    /* Mark read as in progress and reset read buffer */
    conn->read_pending = true;
    conn->read_offset = 0;

    /* For characteristics known to potentially exceed ATT_MTU-1,
     * use Read Long directly */
    if (char_uuid == BLE_ESL_CHR_UUID_DISPLAY_INFO ||
        char_uuid == BLE_ESL_CHR_UUID_SENSOR_INFO ||
        char_uuid == BLE_ESL_CHR_UUID_LED_INFO) {
        ctx->use_read_long = true;
        int rc = ble_gattc_read_long(conn_handle, attr_handle, 0,
                                     gatt_read_long_cb, ctx);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to start read long; rc=%d", rc);
            conn->read_pending = false;
            free(ctx);
            return ESP_FAIL;
        }
        return ESP_OK;
    }

    /* Standard single-shot read */
    int rc = ble_gattc_read(conn_handle, attr_handle, gatt_read_cb, ctx);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start read; rc=%d", rc);
        conn->read_pending = false;
        free(ctx);
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief Context passed through GATT write callbacks
 */
typedef struct {
    ble_esl_ap_gatt_cb_t cb;
    void *user_data;
} gatt_write_ctx_t;

/**
 * @brief Callback for ble_gattc_write_flat
 */
static int gatt_write_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                         struct ble_gatt_attr *attr, void *arg)
{
    gatt_write_ctx_t *ctx = (gatt_write_ctx_t *)arg;
    if (!ctx) {
        return 0;
    }

    esp_err_t status = (error->status == 0) ? ESP_OK : ESP_FAIL;

    if (ctx->cb) {
        ctx->cb(conn_handle, status, NULL, 0, ctx->user_data);
    }

    free(ctx);
    return 0;
}

esp_err_t ble_esl_ap_gatt_write(uint16_t conn_handle, uint16_t char_uuid,
                                const uint8_t *data, uint16_t data_len,
                                ble_esl_ap_gatt_cb_t cb, void *user_data)
{
    if (!g_esl_ap || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t attr_handle = resolve_char_handle(conn, char_uuid);
    if (attr_handle == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    /* Allocate write context */
    gatt_write_ctx_t *ctx = calloc(1, sizeof(gatt_write_ctx_t));
    if (!ctx) {
        return ESP_ERR_NO_MEM;
    }
    ctx->cb = cb;
    ctx->user_data = user_data;

    /* A single Write Request carries at most ATT_MTU - 3 bytes of value
     * (1 byte opcode + 2 byte handle). If the payload exceeds that, fall back
     * to the Write Long procedure. Use the negotiated MTU dynamically. */
    uint16_t mtu = ble_att_mtu(conn_handle);
    if (mtu < 3) {
        /* ble_att_mtu() returns 0 when the ATT channel is gone (e.g. the peer
         * disconnected while this write was being prepared). Bail out instead
         * of deriving a bogus payload limit from it. */
        ESP_LOGE(TAG, "Invalid MTU for write; conn_handle=%d mtu=%d", conn_handle, mtu);
        free(ctx);
        return ESP_ERR_INVALID_STATE;
    }

    int rc;
    if (data_len > (mtu - 3)) {
        /* Write Long — payload must be carried in an mbuf */
        struct os_mbuf *om = ble_hs_mbuf_from_flat(data, data_len);
        if (!om) {
            ESP_LOGE(TAG, "Failed to allocate mbuf for write long");
            free(ctx);
            return ESP_ERR_NO_MEM;
        }
        rc = ble_gattc_write_long(conn_handle, attr_handle, 0, om,
                                  gatt_write_cb, ctx);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to start write long; rc=%d", rc);
            /* om is consumed by ble_gattc_write_long regardless of outcome */
            free(ctx);
            return ESP_FAIL;
        }
    } else {
        rc = ble_gattc_write_flat(conn_handle, attr_handle,
                                  data, data_len,
                                  gatt_write_cb, ctx);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to start write; rc=%d", rc);
            free(ctx);
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

/**
 * @brief Callback for ECP write completion
 *
 * After the write completes, we wait for the ECP notification.
 * If the write itself fails, invoke the ECP callback with error.
 */
static int ecp_write_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                        struct ble_gatt_attr *attr, void *arg)
{
    (void)arg;

    if (error->status != 0) {
        ESP_LOGE(TAG, "ECP write failed; status=%d", error->status);

        ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
        if (conn && conn->ecp_pending) {
            conn->ecp_pending = false;
            ble_esl_ap_gatt_cb_t cb = conn->ecp_cb;
            void *user_data = conn->ecp_user_data;
            conn->ecp_cb = NULL;
            conn->ecp_user_data = NULL;

            if (cb) {
                cb(conn_handle, ESP_FAIL, NULL, 0, user_data);
            }
        }
    }
    /* On success, we wait for the notification (handled in handle_notify_rx) */
    return 0;
}

esp_err_t ble_esl_ap_ecp_write(uint16_t conn_handle, const uint8_t *tlv_data,
                               uint8_t tlv_len, ble_esl_ap_gatt_cb_t cb,
                               void *user_data)
{
    if (!g_esl_ap || !g_esl_ap->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    ble_esl_ap_conn_t *conn = ble_esl_ap_find_conn(conn_handle);
    if (!conn) {
        return ESP_ERR_INVALID_STATE;
    }

    if (conn->handles.ecp_handle == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    if (conn->ecp_pending) {
        ESP_LOGW(TAG, "ECP operation already pending on conn_handle=%d", conn_handle);
        return ESP_ERR_INVALID_STATE;
    }

    /* Set up ECP pending state */
    conn->ecp_pending = true;
    conn->ecp_cb = cb;
    conn->ecp_user_data = user_data;

    /* Write to ECP characteristic */
    int rc = ble_gattc_write_flat(conn_handle, conn->handles.ecp_handle,
                                  tlv_data, tlv_len,
                                  ecp_write_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to write ECP; rc=%d", rc);
        conn->ecp_pending = false;
        conn->ecp_cb = NULL;
        conn->ecp_user_data = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t ble_esl_ap_gattc_init(void)
{
    int rc = ble_ots_client_init(ble_esl_ap_ots_event_cb);
    return (rc == 0) ? ESP_OK : ESP_FAIL;
}

void ble_esl_ap_gattc_deinit(void)
{
    ble_ots_client_deinit();
}
