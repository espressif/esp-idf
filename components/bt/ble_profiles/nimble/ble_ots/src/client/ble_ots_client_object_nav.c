/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_att.h"
#include "host/ble_hs_mbuf.h"
#include "os/os_mbuf.h"

#include "ble_ots_client_int.h"

static const char *TAG = "ble_ots_client_nav";

/*****************************************************************************
 * Forward Declarations
 *****************************************************************************/
static int olcp_write_cb(uint16_t conn_handle,
                         const struct ble_gatt_error *error,
                         struct ble_gatt_attr *attr,
                         void *arg);
static int filter_write_cb(uint16_t conn_handle,
                           const struct ble_gatt_error *error,
                           struct ble_gatt_attr *attr,
                           void *arg);
static int filter_write_long_cb(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attr,
                                void *arg);
static int filter_read_cb(uint16_t conn_handle,
                          const struct ble_gatt_error *error,
                          struct ble_gatt_attr *attr,
                          void *arg);
static int filter_read_long_cb(uint16_t conn_handle,
                               const struct ble_gatt_error *error,
                               struct ble_gatt_attr *attr,
                               void *arg);
static int obj_changed_cccd_write_cb(uint16_t conn_handle,
                                     const struct ble_gatt_error *error,
                                     struct ble_gatt_attr *attr,
                                     void *arg);

/*****************************************************************************
 * Helper: Common OLCP prerequisite checks
 *****************************************************************************/

/**
 * @brief Validate that OLCP operations can be performed on this connection.
 *
 * Checks: module initialized, context exists, multi-object server (OLCP handle
 * present), OLCP CCCD configured, and no prior CP timeout.
 *
 * @param conn_id   Connection identifier
 * @param[out] ctx  Pointer to receive the connection context
 * @return 0 on success, BLE_HS error code on failure
 */
static int olcp_validate(uint16_t conn_id, ble_ots_client_conn_ctx_t **ctx)
{
    if (!g_ots_client || !g_ots_client->initialized) {
        ESP_LOGE(TAG, "OTS client not initialized");
        return BLE_HS_ENOTCONN;
    }

    ble_ots_client_conn_ctx_t *c = ble_ots_client_get_conn_ctx(conn_id);
    if (!c) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (!c->multi_object_server || c->handles.olcp_handle == 0) {
        ESP_LOGE(TAG, "OLCP not available (not a multi-object server)");
        return BLE_HS_ENOTSUP;
    }

    if (c->handles.olcp_cccd_handle == 0) {
        ESP_LOGE(TAG, "OLCP CCCD not configured");
        return BLE_HS_ENOTSUP;
    }

    if (c->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out; reject new CP operations");
        return BLE_HS_ETIMEOUT;
    }

    if (c->cp_timer_active || c->cp_pending_opcode != 0) {
        ESP_LOGE(TAG, "CP operation already in progress");
        return BLE_HS_EBUSY;
    }

    *ctx = c;
    return 0;
}

/*****************************************************************************
 * Helper: Write a simple OLCP command (opcode only, no parameters)
 *****************************************************************************/
static int olcp_write_simple(uint16_t conn_id, uint8_t opcode)
{
    ble_ots_client_conn_ctx_t *ctx = NULL;
    int rc;

    rc = olcp_validate(conn_id, &ctx);
    if (rc != 0) {
        return rc;
    }

    uint8_t buf[1];
    buf[0] = opcode;

    ctx->cp_pending_opcode = opcode;

    rc = ble_gattc_write_flat(conn_id, ctx->handles.olcp_handle,
                              buf, sizeof(buf), olcp_write_cb, (void *)(uintptr_t)conn_id);
    if (rc != 0) {
        ESP_LOGE(TAG, "OLCP write failed; opcode=0x%02x rc=%d", opcode, rc);
        ctx->cp_pending_opcode = 0;
        return rc;
    }

    return 0;
}

/*****************************************************************************
 * OLCP Write Callback
 *****************************************************************************/

/**
 * @brief Callback for OLCP GATT write completion.
 *
 * On success (ATT Write Response), starts the CP timer.
 * On failure (ATT Error Response), the operation is not started.
 */
static int olcp_write_cb(uint16_t conn_handle,
                         const struct ble_gatt_error *error,
                         struct ble_gatt_attr *attr,
                         void *arg)
{
    uint16_t conn_id = (uint16_t)(uintptr_t)arg;

    if (error->status == 0) {
        /* Write accepted — start CP timer */
        ESP_LOGD(TAG, "OLCP write accepted, starting CP timer; conn_id=%d", conn_id);
        int rc = ble_ots_client_start_cp_timer(conn_id, CONFIG_BLE_OTS_CLIENT_OLCP_TIMEOUT_MS);
        if (rc != 0) {
            /* The server has already accepted the command and will eventually
             * complete it by sending an OLCP indication. We only failed to arm
             * the local timeout timer, so we must NOT abandon the operation:
             * clearing cp_pending_opcode or dispatching a synthetic failure
             * here would desynchronize the client state machine from the
             * server and could let the app start a concurrent CP command.
             * Keep the pending state intact (just without timeout protection)
             * and let handle_olcp_indication complete it normally. */
            ESP_LOGE(TAG, "Failed to start CP timer; rc=%d (operation continues without timeout protection)", rc);
        }
    } else {
        /* ATT error — operation not started, no timer needed */
        ESP_LOGE(TAG, "OLCP write error; status=0x%04x conn_id=%d", error->status, conn_id);
        ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
        if (ctx) {
            uint8_t pending_opcode = ctx->cp_pending_opcode;
            ctx->cp_pending_opcode = 0;

            /* Notify the application of the failure so it does not hang */
            ble_ots_client_olcp_response_t resp = {
                .request_opcode = pending_opcode,
                .result_code = BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                .num_objects = 0,
            };
            ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_OLCP_RESPONSE, &resp);
        }
    }

    return 0;
}

/*****************************************************************************
 * Public APIs — OLCP Navigation
 *****************************************************************************/

int ble_ots_client_first_object(uint16_t conn_id)
{
    return olcp_write_simple(conn_id, BLE_OTS_OLCP_OPCODE_FIRST);
}

int ble_ots_client_last_object(uint16_t conn_id)
{
    return olcp_write_simple(conn_id, BLE_OTS_OLCP_OPCODE_LAST);
}

int ble_ots_client_prev_object(uint16_t conn_id)
{
    return olcp_write_simple(conn_id, BLE_OTS_OLCP_OPCODE_PREVIOUS);
}

int ble_ots_client_next_object(uint16_t conn_id)
{
    return olcp_write_simple(conn_id, BLE_OTS_OLCP_OPCODE_NEXT);
}

int ble_ots_client_goto_object(uint16_t conn_id, uint64_t object_id)
{
    ble_ots_client_conn_ctx_t *ctx = NULL;
    int rc;

    rc = olcp_validate(conn_id, &ctx);
    if (rc != 0) {
        return rc;
    }

    /* OLCP Go To: 1-byte opcode + 6-byte Object ID (UINT48, little-endian) */
    uint8_t buf[7];
    buf[0] = BLE_OTS_OLCP_OPCODE_GO_TO;
    buf[1] = (uint8_t)(object_id & 0xFF);
    buf[2] = (uint8_t)((object_id >> 8) & 0xFF);
    buf[3] = (uint8_t)((object_id >> 16) & 0xFF);
    buf[4] = (uint8_t)((object_id >> 24) & 0xFF);
    buf[5] = (uint8_t)((object_id >> 32) & 0xFF);
    buf[6] = (uint8_t)((object_id >> 40) & 0xFF);

    ctx->cp_pending_opcode = BLE_OTS_OLCP_OPCODE_GO_TO;

    rc = ble_gattc_write_flat(conn_id, ctx->handles.olcp_handle,
                              buf, sizeof(buf), olcp_write_cb, (void *)(uintptr_t)conn_id);
    if (rc != 0) {
        ESP_LOGE(TAG, "OLCP Go To write failed; rc=%d", rc);
        ctx->cp_pending_opcode = 0;
        return rc;
    }

    return 0;
}

int ble_ots_client_order_objects(uint16_t conn_id, uint8_t sort_order)
{
    ble_ots_client_conn_ctx_t *ctx = NULL;
    int rc;

    rc = olcp_validate(conn_id, &ctx);
    if (rc != 0) {
        return rc;
    }

    /* OLCP Order: 1-byte opcode + 1-byte sort order */
    uint8_t buf[2];
    buf[0] = BLE_OTS_OLCP_OPCODE_ORDER;
    buf[1] = sort_order;

    ctx->cp_pending_opcode = BLE_OTS_OLCP_OPCODE_ORDER;

    rc = ble_gattc_write_flat(conn_id, ctx->handles.olcp_handle,
                              buf, sizeof(buf), olcp_write_cb, (void *)(uintptr_t)conn_id);
    if (rc != 0) {
        ESP_LOGE(TAG, "OLCP Order write failed; rc=%d", rc);
        ctx->cp_pending_opcode = 0;
        return rc;
    }

    return 0;
}

int ble_ots_client_request_num_objects(uint16_t conn_id)
{
    return olcp_write_simple(conn_id, BLE_OTS_OLCP_OPCODE_REQUEST_NUM_OF_OBJECTS);
}

int ble_ots_client_clear_marking(uint16_t conn_id)
{
    return olcp_write_simple(conn_id, BLE_OTS_OLCP_OPCODE_CLEAR_MARKING);
}

/*****************************************************************************
 * Object List Filter Write
 *****************************************************************************/

/** Context passed through filter write callbacks */
typedef struct {
    uint16_t conn_id;
    uint8_t  instance;
} filter_write_ctx_t;

static int filter_write_cb(uint16_t conn_handle,
                           const struct ble_gatt_error *error,
                           struct ble_gatt_attr *attr,
                           void *arg)
{
    filter_write_ctx_t *fctx = (filter_write_ctx_t *)arg;
    if (!fctx) {
        return 0;
    }

    ble_ots_client_filter_set_t evt = {
        .instance = fctx->instance,
        .status = error->status,
    };

    ESP_LOGD(TAG, "Filter write complete; instance=%d status=0x%04x", fctx->instance, error->status);

    ble_ots_client_dispatch_event(fctx->conn_id, BLE_OTS_CLIENT_EVT_FILTER_SET, &evt);

    free(fctx);
    return 0;
}

static int filter_write_long_cb(uint16_t conn_handle,
                                const struct ble_gatt_error *error,
                                struct ble_gatt_attr *attr,
                                void *arg)
{
    filter_write_ctx_t *fctx = (filter_write_ctx_t *)arg;
    if (!fctx) {
        return 0;
    }

    /* Check if server returned "Request Not Supported" for Write Long */
    if (error->status == BLE_HS_ATT_ERR(BLE_ATT_ERR_REQ_NOT_SUPPORTED)) {
        ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(fctx->conn_id);
        if (ctx) {
            ctx->server_supports_write_long = false;
            ESP_LOGW(TAG, "Server does not support Write Long; disabling for this connection");
        }
    }

    ble_ots_client_filter_set_t evt = {
        .instance = fctx->instance,
        .status = error->status,
    };

    ESP_LOGD(TAG, "Filter write long complete; instance=%d status=0x%04x", fctx->instance, error->status);

    ble_ots_client_dispatch_event(fctx->conn_id, BLE_OTS_CLIENT_EVT_FILTER_SET, &evt);

    free(fctx);
    return 0;
}

int ble_ots_client_set_filter(uint16_t conn_id,
                              uint8_t instance,
                              uint8_t filter_type,
                              const uint8_t *param,
                              uint16_t param_len)
{
    if (!g_ots_client || !g_ots_client->initialized) {
        ESP_LOGE(TAG, "OTS client not initialized");
        return BLE_HS_ENOTCONN;
    }

    if (instance > 2) {
        ESP_LOGE(TAG, "Invalid filter instance: %d", instance);
        return BLE_HS_EINVAL;
    }

    if (param_len > 0 && param == NULL) {
        ESP_LOGE(TAG, "param is NULL but param_len=%d", param_len);
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (!ctx) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    uint16_t filter_handle = ctx->handles.object_list_filter_handle[instance];
    if (filter_handle == 0) {
        ESP_LOGE(TAG, "Filter instance %d handle not discovered", instance);
        return BLE_HS_ENOTSUP;
    }

    /* Build the characteristic value: 1-byte filter_type + param data */
    uint16_t total_len = 1 + param_len;
    uint8_t *buf = malloc(total_len);
    if (!buf) {
        ESP_LOGE(TAG, "Failed to allocate filter buffer");
        return BLE_HS_ENOMEM;
    }

    buf[0] = filter_type;
    if (param && param_len > 0) {
        memcpy(&buf[1], param, param_len);
    }

    /* Allocate callback context */
    filter_write_ctx_t *fctx = malloc(sizeof(filter_write_ctx_t));
    if (!fctx) {
        free(buf);
        ESP_LOGE(TAG, "Failed to allocate filter write context");
        return BLE_HS_ENOMEM;
    }
    fctx->conn_id = conn_id;
    fctx->instance = instance;

    int rc;

    /* Determine if we need Write Long.
     * ATT_MTU - 3 is the max payload for a single Write Request.
     * We use ble_att_mtu() to get the current MTU for the connection. */
    uint16_t mtu = ble_att_mtu(conn_id);
    uint16_t max_write_len = (mtu > 3) ? (mtu - 3) : 0;

    if (total_len > max_write_len && ctx->server_supports_write_long) {
        /* Use Write Long */
        struct os_mbuf *om = ble_hs_mbuf_from_flat(buf, total_len);
        free(buf);
        if (!om) {
            free(fctx);
            ESP_LOGE(TAG, "Failed to allocate mbuf for Write Long");
            return BLE_HS_ENOMEM;
        }

        rc = ble_gattc_write_long(conn_id, filter_handle, 0, om,
                                  filter_write_long_cb, fctx);
        if (rc != 0) {
            ESP_LOGE(TAG, "Filter Write Long failed; rc=%d", rc);
            /* om is consumed by ble_gattc_write_long regardless of outcome */
            free(fctx);
            return rc;
        }
    } else if (total_len > max_write_len && !ctx->server_supports_write_long) {
        /* Server doesn't support Write Long and data is too long */
        free(buf);
        free(fctx);
        ESP_LOGE(TAG, "Filter data too long and Write Long not supported");
        return BLE_HS_EMSGSIZE;
    } else {
        /* Use regular Write */
        rc = ble_gattc_write_flat(conn_id, filter_handle,
                                  buf, total_len, filter_write_cb, fctx);
        free(buf);
        if (rc != 0) {
            ESP_LOGE(TAG, "Filter write failed; rc=%d", rc);
            free(fctx);
            return rc;
        }
    }

    return 0;
}

/*****************************************************************************
 * Object List Filter Read
 *****************************************************************************/

/** Context passed through filter read callbacks (accumulates Read Long data). */
typedef struct {
    uint16_t conn_id;
    uint8_t  instance;
    uint16_t buf_len;
    uint8_t  buf[1 + BLE_OTS_OBJECT_NAME_MAX_LEN];
} filter_read_ctx_t;

static void filter_read_dispatch(filter_read_ctx_t *fctx, int status)
{
    ble_ots_client_filter_read_t evt = {
        .instance    = fctx->instance,
        .status      = status,
        .filter_type = 0,
        .param       = NULL,
        .param_len   = 0,
    };

    if (status == 0 && fctx->buf_len >= 1) {
        evt.filter_type = fctx->buf[0];
        if (fctx->buf_len > 1) {
            evt.param     = &fctx->buf[1];
            evt.param_len = fctx->buf_len - 1;
        }
    } else if (status == 0) {
        /* Success but empty value is not a valid filter encoding */
        evt.status = BLE_HS_EINVAL;
    }

    ble_ots_client_dispatch_event(fctx->conn_id, BLE_OTS_CLIENT_EVT_FILTER_READ, &evt);
}

static int filter_read_long_cb(uint16_t conn_handle,
                               const struct ble_gatt_error *error,
                               struct ble_gatt_attr *attr,
                               void *arg)
{
    filter_read_ctx_t *fctx = (filter_read_ctx_t *)arg;
    if (!fctx) {
        return 0;
    }

    ble_ots_client_conn_ctx_t *conn_ctx = ble_ots_client_get_conn_ctx(fctx->conn_id);
    if (conn_ctx == NULL) {
        /* Connection dropped during Read Long */
        filter_read_dispatch(fctx, (error->status != 0) ? error->status : BLE_HS_ENOTCONN);
        free(fctx);
        /* If this was a fragment reception (status == 0), we must return a
         * non-zero value so NimBLE aborts the Read Long procedure and does not
         * re-invoke this callback with the now-freed context. */
        return (error->status == 0) ? BLE_HS_ENOTCONN : 0;
    }

    if (error->status == 0 && attr != NULL && attr->om != NULL) {
        /* Accumulate a fragment; more may follow */
        uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
        uint16_t space = sizeof(fctx->buf) - fctx->buf_len;
        uint16_t copy_len = data_len < space ? data_len : space;
        if (copy_len > 0) {
            os_mbuf_copydata(attr->om, 0, copy_len, fctx->buf + fctx->buf_len);
            fctx->buf_len += copy_len;
        }
        return 0;
    }

    /* Completion or error */
    if (error->status == BLE_HS_EDONE || error->status == BLE_HS_ATT_ERR(BLE_ATT_ERR_ATTR_NOT_LONG)) {
        filter_read_dispatch(fctx, 0);
    } else if (error->status == BLE_HS_ATT_ERR(BLE_ATT_ERR_REQ_NOT_SUPPORTED)) {
        conn_ctx->server_supports_read_long = false;
        filter_read_dispatch(fctx, 0);
    } else {
        filter_read_dispatch(fctx, error->status);
    }

    free(fctx);

    /* Note: if we reach here with error->status == 0 (e.g. a defensive check
     * such as attr == NULL failed on a fragment reception), we must return a
     * non-zero value to abort the procedure. Returning 0 would cause NimBLE to
     * issue another Read Blob request and re-invoke this callback with the
     * freed context, resulting in a use-after-free / double-free. */
    return (error->status == 0) ? BLE_HS_EAPP : 0;
}

static int filter_read_cb(uint16_t conn_handle,
                          const struct ble_gatt_error *error,
                          struct ble_gatt_attr *attr,
                          void *arg)
{
    filter_read_ctx_t *fctx = (filter_read_ctx_t *)arg;
    if (!fctx) {
        return 0;
    }

    if (error->status != 0 || attr == NULL || attr->om == NULL) {
        filter_read_dispatch(fctx, (error->status != 0) ? error->status : BLE_HS_EINVAL);
        free(fctx);
        return 0;
    }

    ble_ots_client_conn_ctx_t *conn_ctx = ble_ots_client_get_conn_ctx(fctx->conn_id);
    if (conn_ctx == NULL) {
        filter_read_dispatch(fctx, BLE_HS_ENOTCONN);
        free(fctx);
        return 0;
    }

    /* Copy the initial fragment */
    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
    uint16_t copy_len = data_len < sizeof(fctx->buf) ? data_len : sizeof(fctx->buf);
    os_mbuf_copydata(attr->om, 0, copy_len, fctx->buf);
    fctx->buf_len = copy_len;

    /* A full (ATT_MTU - 1) payload suggests the value was truncated; continue
     * with Read Long if the server supports it and we have buffer space. */
    uint16_t att_mtu = ble_att_mtu(conn_handle);
    if (att_mtu > 0 && data_len == (att_mtu - 1) &&
        conn_ctx->server_supports_read_long &&
        fctx->buf_len < sizeof(fctx->buf)) {
        int rc = ble_gattc_read_long(conn_handle,
                                     conn_ctx->handles.object_list_filter_handle[fctx->instance],
                                     data_len,
                                     filter_read_long_cb, fctx);
        if (rc != 0) {
            ESP_LOGE(TAG, "Filter Read Long failed to initiate; rc=%d", rc);
            filter_read_dispatch(fctx, 0); /* report what we already have */
            free(fctx);
        }
        return 0;
    }

    filter_read_dispatch(fctx, 0);
    free(fctx);
    return 0;
}

int ble_ots_client_read_filter(uint16_t conn_id, uint8_t instance)
{
    if (!g_ots_client || !g_ots_client->initialized) {
        ESP_LOGE(TAG, "OTS client not initialized");
        return BLE_HS_ENOTCONN;
    }

    if (instance > 2) {
        ESP_LOGE(TAG, "Invalid filter instance: %d", instance);
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (!ctx) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    uint16_t filter_handle = ctx->handles.object_list_filter_handle[instance];
    if (filter_handle == 0) {
        ESP_LOGE(TAG, "Filter instance %d handle not discovered", instance);
        return BLE_HS_ENOTSUP;
    }

    filter_read_ctx_t *fctx = malloc(sizeof(filter_read_ctx_t));
    if (!fctx) {
        ESP_LOGE(TAG, "Failed to allocate filter read context");
        return BLE_HS_ENOMEM;
    }
    fctx->conn_id  = conn_id;
    fctx->instance = instance;
    fctx->buf_len  = 0;

    int rc = ble_gattc_read(conn_id, filter_handle, filter_read_cb, fctx);
    if (rc != 0) {
        ESP_LOGE(TAG, "Filter read failed; instance=%d rc=%d", instance, rc);
        free(fctx);
        return rc;
    }

    return 0;
}

/*****************************************************************************
 * Object Changed CCCD Subscription
 *****************************************************************************/

static int obj_changed_cccd_write_cb(uint16_t conn_handle,
                                     const struct ble_gatt_error *error,
                                     struct ble_gatt_attr *attr,
                                     void *arg)
{
    if (error->status != 0) {
        ESP_LOGE(TAG, "Object Changed CCCD write error; status=0x%04x", error->status);
    } else {
        ESP_LOGD(TAG, "Object Changed CCCD write success; conn_handle=%d", conn_handle);
    }

    return 0;
}

int ble_ots_client_subscribe_object_changed(uint16_t conn_id, bool enable)
{
    if (!g_ots_client || !g_ots_client->initialized) {
        ESP_LOGE(TAG, "OTS client not initialized");
        return BLE_HS_ENOTCONN;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (!ctx) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.object_changed_cccd_handle == 0) {
        ESP_LOGE(TAG, "Object Changed CCCD handle not discovered");
        return BLE_HS_ENOTSUP;
    }

    /* CCCD value: 0x0002 for indications, 0x0000 to disable */
    uint8_t value[2];
    if (enable) {
        value[0] = 0x02;
        value[1] = 0x00;
    } else {
        value[0] = 0x00;
        value[1] = 0x00;
    }

    int rc = ble_gattc_write_flat(conn_id, ctx->handles.object_changed_cccd_handle,
                                  value, sizeof(value),
                                  obj_changed_cccd_write_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Object Changed CCCD write failed; rc=%d", rc);
        return rc;
    }

    return 0;
}

/*****************************************************************************
 * OLCP Indication Handler
 *****************************************************************************/

/**
 * @brief Handle an OLCP indication (Response Code 0x70).
 *
 * Parses the indication payload:
 *   byte[0] = Response opcode (0x70)
 *   byte[1] = Request opcode
 *   byte[2] = Result code
 *   byte[3..6] = Optional num_objects (UINT32 LE, only for Request Num Objects + Success)
 *
 * Stops the CP timer and dispatches BLE_OTS_CLIENT_EVT_OLCP_RESPONSE.
 */
static void handle_olcp_indication(uint16_t conn_id, const uint8_t *data, uint16_t data_len)
{
    if (data_len < 3) {
        ESP_LOGE(TAG, "OLCP indication too short; len=%d", data_len);
        return;
    }

    /* byte[0] should be 0x70 (Response Code) — already verified by caller */
    uint8_t request_opcode = data[1];
    uint8_t result_code = data[2];

    ble_ots_client_olcp_response_t resp = {
        .request_opcode = request_opcode,
        .result_code = result_code,
        .num_objects = 0,
    };

    /* If Request Number of Objects and Success, parse the UINT32 response parameter */
    if (request_opcode == BLE_OTS_OLCP_OPCODE_REQUEST_NUM_OF_OBJECTS &&
        result_code == BLE_OTS_OLCP_RESULT_SUCCESS &&
        data_len >= 7) {
        resp.num_objects = (uint32_t)data[3] |
                          ((uint32_t)data[4] << 8) |
                          ((uint32_t)data[5] << 16) |
                          ((uint32_t)data[6] << 24);
    }

    /* Stop the CP timer */
    ble_ots_client_stop_cp_timer(conn_id);

    /* Clear pending opcode */
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx) {
        ctx->cp_pending_opcode = 0;
    }

    ESP_LOGD(TAG, "OLCP response: req_op=0x%02x result=0x%02x num_objects=%lu",
             request_opcode, result_code, (unsigned long)resp.num_objects);

    ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_OLCP_RESPONSE, &resp);
}

/*****************************************************************************
 * Object Changed Indication Handler
 *****************************************************************************/

/**
 * @brief Handle an Object Changed indication.
 *
 * Parses the 7-byte payload:
 *   byte[0] = Flags
 *   byte[1..6] = Object ID (UINT48, little-endian)
 *
 * Dispatches BLE_OTS_CLIENT_EVT_OBJECT_CHANGED.
 */
static void handle_object_changed_indication(uint16_t conn_id, const uint8_t *data, uint16_t data_len)
{
    if (data_len < 7) {
        ESP_LOGE(TAG, "Object Changed indication too short; len=%d", data_len);
        return;
    }

    uint8_t flags = data[0];

    /* Decode Object ID (UINT48, little-endian) */
    uint64_t object_id = (uint64_t)data[1] |
                         ((uint64_t)data[2] << 8) |
                         ((uint64_t)data[3] << 16) |
                         ((uint64_t)data[4] << 24) |
                         ((uint64_t)data[5] << 32) |
                         ((uint64_t)data[6] << 40);

    ble_ots_client_object_changed_t evt = {
        .source_of_change = (flags & BLE_OTS_OBJ_CHANGED_FLAG_SOURCE) ? 1 : 0,
        .contents_changed = (flags & BLE_OTS_OBJ_CHANGED_FLAG_CONTENT) ? true : false,
        .metadata_changed = (flags & BLE_OTS_OBJ_CHANGED_FLAG_METADATA) ? true : false,
        .object_created   = (flags & BLE_OTS_OBJ_CHANGED_FLAG_CREATION) ? true : false,
        .object_deleted   = (flags & BLE_OTS_OBJ_CHANGED_FLAG_DELETION) ? true : false,
        .object_id        = object_id,
    };

    ESP_LOGD(TAG, "Object Changed: flags=0x%02x obj_id=0x%012llx",
             flags, (unsigned long long)object_id);

    ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_OBJECT_CHANGED, &evt);
}

/*****************************************************************************
 * GAP Event Handler — Notification/Indication Reception
 *****************************************************************************/

/**
 * @brief GAP event handler for OLCP and Object Changed indications.
 *
 * This function should be registered as a GAP event listener or called from
 * the main GAP event handler. It processes BLE_GAP_EVENT_NOTIFY_RX events
 * for the OLCP and Object Changed characteristic handles.
 *
 * @param event GAP event
 * @param arg   Unused
 * @return 0 on success
 */
int ble_ots_client_object_nav_gap_event(struct ble_gap_event *event, void *arg)
{
    if (!g_ots_client || !g_ots_client->initialized) {
        return 0;
    }

    if (event->type != BLE_GAP_EVENT_NOTIFY_RX) {
        return 0;
    }

    uint16_t conn_handle = event->notify_rx.conn_handle;
    uint16_t attr_handle = event->notify_rx.attr_handle;

    /* Only process indications */
    if (!event->notify_rx.indication) {
        return 0;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (!ctx) {
        return 0;
    }

    /* Copy data from mbuf */
    uint16_t data_len = OS_MBUF_PKTLEN(event->notify_rx.om);
    if (data_len == 0) {
        return 0;
    }

    uint8_t data_buf[32]; /* Max expected indication payload size */
    uint16_t copy_len = (data_len > sizeof(data_buf)) ? sizeof(data_buf) : data_len;
    int rc = os_mbuf_copydata(event->notify_rx.om, 0, copy_len, data_buf);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to copy indication data; rc=%d", rc);
        return 0;
    }

    /* Check if this is an OLCP indication */
    if (ctx->handles.olcp_handle != 0 && attr_handle == ctx->handles.olcp_handle) {
        /* Verify Response Code opcode (0x70) */
        if (copy_len >= 1 && data_buf[0] == BLE_OTS_OLCP_OPCODE_RESPONSE) {
            handle_olcp_indication(conn_handle, data_buf, copy_len);
        } else {
            ESP_LOGW(TAG, "Unexpected OLCP indication opcode: 0x%02x",
                     copy_len > 0 ? data_buf[0] : 0);
        }
        return 0;
    }

    /* Check if this is an Object Changed indication */
    if (ctx->handles.object_changed_handle != 0 && attr_handle == ctx->handles.object_changed_handle) {
        handle_object_changed_indication(conn_handle, data_buf, copy_len);
        return 0;
    }

    return 0;
}
