/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_log.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_npl.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_l2cap.h"
#include "os/os_mbuf.h"

#include "ble_ots_client_int.h"

static const char *TAG = "ots_client_transfer";

/*****************************************************************************
 * Constants
 *****************************************************************************/
#define PSM_OTS                     0x0025
#define OTC_MTU                     256

#define OACP_RESPONSE_OPCODE        0x60

/*****************************************************************************
 * Forward Declarations
 *****************************************************************************/
static int ble_ots_client_l2cap_event_cb(struct ble_l2cap_event *event, void *arg);
static void ble_ots_client_transfer_timer_cb(struct ble_npl_event *ev);

/*****************************************************************************
 * Transfer Inactivity Timer
 *
 * All OTS client timers are NimBLE callouts bound to the host default event
 * queue, so their callbacks run in the NimBLE host task. That serialises them
 * with the GATT/L2CAP handlers that mutate the same connection context (no
 * data races, no lock needed), keeps NimBLE API calls on the host task as the
 * stack requires, and honours the ble_ots_client_event_cb_t contract that
 * application callbacks are invoked from the BLE host task.
 *****************************************************************************/

/**
 * @brief Create the transfer inactivity timer for a connection context.
 */
static int ble_ots_client_create_transfer_timer(ble_ots_client_conn_ctx_t *ctx)
{
    if (ctx->transfer_timer_inited) {
        return 0;
    }

    int rc = ble_npl_callout_init(&ctx->transfer_timer, nimble_port_get_dflt_eventq(),
                                  ble_ots_client_transfer_timer_cb,
                                  (void *)(uintptr_t)ctx->conn_id);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to init transfer timer callout: %d", rc);
        return BLE_HS_ENOMEM;
    }

    ctx->transfer_timer_inited = true;
    return 0;
}

/**
 * @brief Start or restart the transfer inactivity timer.
 */
static int ble_ots_client_start_transfer_timer(ble_ots_client_conn_ctx_t *ctx)
{
    int rc = ble_ots_client_create_transfer_timer(ctx);
    if (rc != 0) {
        return rc;
    }

    /* ble_npl_callout_reset() re-arms an already-running callout. */
    ble_npl_error_t err =
        ble_npl_callout_reset(&ctx->transfer_timer,
                              ble_npl_time_ms_to_ticks32(CONFIG_BLE_OTS_CLIENT_TRANSFER_TIMEOUT_MS));
    if (err != BLE_NPL_OK) {
        ESP_LOGE(TAG, "Failed to start transfer timer: %d", err);
        return BLE_HS_EOS;
    }

    ctx->transfer_timer_active = true;
    return 0;
}

/**
 * @brief Stop the transfer inactivity timer.
 */
static void ble_ots_client_stop_transfer_timer(ble_ots_client_conn_ctx_t *ctx)
{
    /* Stop unconditionally (not gated on transfer_timer_active) so the callout
     * can never be left armed because the flag was out of step with it. */
    if (ctx->transfer_timer_inited) {
        ble_npl_callout_stop(&ctx->transfer_timer);
    }
    ctx->transfer_timer_active = false;
}

/**
 * @brief Transfer inactivity timer callback (runs in the NimBLE host task).
 */
static void ble_ots_client_transfer_timer_cb(struct ble_npl_event *ev)
{
    uint16_t conn_id = (uint16_t)(uintptr_t)ble_npl_event_get_arg(ev);

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return;
    }

    /* The callout may have been stopped after its event was already queued. */
    if (!ctx->transfer_timer_active) {
        ESP_LOGD(TAG, "Transfer timeout ignored (timer already stopped), conn_id=%d", conn_id);
        return;
    }

    ESP_LOGW(TAG, "Transfer inactivity timeout, conn_id=%d", conn_id);

    ctx->transfer_timer_active = false;
    ctx->transfer_in_progress = false;
    ctx->transfer_opcode = 0;

    ble_ots_client_transfer_timeout_t evt = {
        .conn_id = conn_id,
        .opcode = 0x00,  /* Data-transfer inactivity timeout */
    };

    ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_TRANSFER_TIMEOUT, &evt);
}

/*****************************************************************************
 * OACP Write Callback
 *****************************************************************************/

/**
 * @brief GATT write callback for OACP control-point writes.
 */
static int ble_ots_client_oacp_write_cb(uint16_t conn_handle,
                                        const struct ble_gatt_error *error,
                                        struct ble_gatt_attr *attr,
                                        void *arg)
{
    uint8_t opcode = (uint8_t)(uintptr_t)arg;

    if (error->status != 0) {
        ESP_LOGE(TAG, "OACP write failed: status=%d, opcode=0x%02x", error->status, opcode);

        ble_ots_client_conn_ctx_t *err_ctx = ble_ots_client_get_conn_ctx(conn_handle);
        if (err_ctx != NULL) {
            /* The procedure never started — release the synchronous busy flag
             * so the next control-point operation is not rejected forever. */
            err_ctx->cp_pending_opcode = 0;

            /* Clear transfer_in_progress if it was set early for Read/Write */
            if (opcode == BLE_OTS_OACP_OPCODE_READ || opcode == BLE_OTS_OACP_OPCODE_WRITE) {
                err_ctx->transfer_in_progress = false;
                err_ctx->transfer_opcode = 0;
            }
        }

        /* Dispatch a failure event so the application is not left waiting */
        ble_ots_client_oacp_response_t resp = {
            .request_opcode = opcode,
            .result_code = BLE_OTS_OACP_RESULT_OPERATION_FAILED,
            .checksum = 0,
            .has_checksum = false,
        };
        ble_ots_client_dispatch_event(conn_handle, BLE_OTS_CLIENT_EVT_OACP_RESPONSE, &resp);
        return 0;
    }

    ESP_LOGD(TAG, "OACP write success, opcode=0x%02x, starting CP timer", opcode);

    /* Start the control-point response timer. cp_pending_opcode was already
     * set synchronously by the command API, so it is not touched here. */
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (ctx != NULL) {
        int rc = ble_ots_client_start_cp_timer(conn_handle, CONFIG_BLE_OTS_CLIENT_OACP_TIMEOUT_MS);
        if (rc != 0) {
            /* The server has accepted the command and will answer with an
             * indication; only the local timeout guard is missing. Keep the
             * pending state so the state machine stays in sync with the peer. */
            ESP_LOGE(TAG, "Failed to start CP timer: rc=%d (procedure continues unguarded)", rc);
        }
    }

    return 0;
}

/*****************************************************************************
 * OACP Indication Handler
 *****************************************************************************/

/* Deferred dispatch of a Write-success OACP response. Posted to the NimBLE host
 * event queue so it runs in a later host-task iteration — after this
 * indication's processing (and its ATT confirmation) has completed — so the
 * object-data send the app issues from its OACP_RESPONSE handler goes out after
 * the confirmation. A synchronous dispatch would race it and the peer would
 * miss the data. */
static void ble_ots_client_oacp_dispatch_ev_cb(struct ble_npl_event *ev)
{
    uint16_t conn_id = (uint16_t)(uintptr_t)ble_npl_event_get_arg(ev);

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return;
    }

    ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_OACP_RESPONSE,
                                  &ctx->pending_oacp_resp);
}

/**
 * @brief Handle an OACP indication (called from GAP event handler in discovery module).
 *
 * Parses the OACP Response Code indication:
 *   [0] = Response Op Code (0x60)
 *   [1] = Request Op Code
 *   [2] = Result Code
 *   [3..6] = Optional checksum (for Calculate Checksum + Success)
 */
void ble_ots_client_handle_oacp_indication(uint16_t conn_handle,
                                           uint16_t attr_handle,
                                           struct os_mbuf *om)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "OACP indication: no context for conn_handle=%d", conn_handle);
        return;
    }

    uint16_t data_len = OS_MBUF_PKTLEN(om);
    if (data_len < 3) {
        ESP_LOGE(TAG, "OACP indication too short: len=%d", data_len);
        return;
    }

    uint8_t buf[7]; /* Max: opcode(1) + req_opcode(1) + result(1) + checksum(4) */
    uint16_t copy_len = data_len > sizeof(buf) ? sizeof(buf) : data_len;
    int rc = os_mbuf_copydata(om, 0, copy_len, buf);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to copy OACP indication data");
        return;
    }

    uint8_t response_opcode = buf[0];
    if (response_opcode != OACP_RESPONSE_OPCODE) {
        ESP_LOGW(TAG, "Unexpected OACP opcode: 0x%02x", response_opcode);
        return;
    }

    uint8_t request_opcode = buf[1];
    uint8_t result_code = buf[2];

    ESP_LOGI(TAG, "OACP response: req_opcode=0x%02x, result=0x%02x", request_opcode, result_code);

    /* The procedure is complete: stop the control-point timer and release the
     * synchronous busy flag. Without clearing cp_pending_opcode here, every
     * later OACP/OLCP operation would be rejected with BLE_HS_EBUSY for the
     * rest of the connection. */
    ble_ots_client_stop_cp_timer(conn_handle);
    ctx->cp_pending_opcode = 0;

    /* Build the OACP response event */
    ble_ots_client_oacp_response_t resp = {
        .request_opcode = request_opcode,
        .result_code = result_code,
        .checksum = 0,
        .has_checksum = false,
    };

    /* Check for checksum in Calculate Checksum + Success */
    if (request_opcode == BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM &&
        result_code == BLE_OTS_OACP_RESULT_SUCCESS &&
        data_len >= 7) {
        resp.checksum = (uint32_t)buf[3] |
                        ((uint32_t)buf[4] << 8) |
                        ((uint32_t)buf[5] << 16) |
                        ((uint32_t)buf[6] << 24);
        resp.has_checksum = true;
        ESP_LOGI(TAG, "OACP checksum: 0x%08" PRIx32, resp.checksum);
    }

    /* Manage transfer tracking BEFORE dispatching the event, so the transfer
     * state is already set up when the app issues a data send from its
     * OACP_RESPONSE handler. */
    bool defer_dispatch = false;
    if (request_opcode == BLE_OTS_OACP_OPCODE_READ ||
        request_opcode == BLE_OTS_OACP_OPCODE_WRITE) {
        if (result_code == BLE_OTS_OACP_RESULT_SUCCESS) {
            if (!ctx->otc_open) {
                /* The OTC was open when the command was issued but has since
                 * closed, so no object data can flow. Report the procedure to
                 * the application as failed rather than passing the server's
                 * Success up: otherwise the app would wait for data that can
                 * never arrive, or push data that cannot be sent. */
                ESP_LOGE(TAG, "OTC not open, cannot start transfer for opcode=0x%02x",
                         request_opcode);
                ctx->transfer_in_progress = false;
                ctx->transfer_opcode = 0;
                resp.result_code = BLE_OTS_OACP_RESULT_CHANNEL_UNAVAILABLE;
            } else if (request_opcode == BLE_OTS_OACP_OPCODE_READ) {
                ctx->transfer_opcode = BLE_OTS_OACP_OPCODE_READ;
                ctx->transfer_received = 0;
                /* transfer_offset and transfer_length were set when the command was issued */
                ble_ots_client_start_transfer_timer(ctx);
                ESP_LOGI(TAG, "Read transfer started: offset=%" PRIu32 ", length=%" PRIu32,
                         ctx->transfer_offset, ctx->transfer_length);
            } else {
                ctx->transfer_opcode = BLE_OTS_OACP_OPCODE_WRITE;
                ctx->transfer_sent = 0;
                /* transfer_offset and transfer_length were set when the command was issued */
                ble_ots_client_start_transfer_timer(ctx);
                ESP_LOGI(TAG, "Write transfer started: offset=%" PRIu32 ", length=%" PRIu32,
                         ctx->transfer_offset, ctx->transfer_length);
                /* On Write success the app pushes object data over the OTC from
                 * its OACP_RESPONSE handler; defer that dispatch so the send
                 * goes out after this indication's ATT confirmation. */
                defer_dispatch = true;
            }
        } else {
            /* OACP response indicates failure — clear the early transfer_in_progress flag */
            ctx->transfer_in_progress = false;
            ctx->transfer_opcode = 0;
        }
    }

    if (defer_dispatch) {
        /* Initialise the deferral event once per connection */
        if (!ctx->oacp_dispatch_ev_ready) {
            ble_npl_event_init(&ctx->oacp_dispatch_ev,
                               ble_ots_client_oacp_dispatch_ev_cb,
                               (void *)(uintptr_t)conn_handle);
            ctx->oacp_dispatch_ev_ready = true;
        }
        ctx->pending_oacp_resp = resp;
        /* Runs in a later host-task iteration, i.e. after this indication's ATT
         * confirmation has been emitted, so the app's data send follows it. */
        ble_npl_eventq_put(nimble_port_get_dflt_eventq(), &ctx->oacp_dispatch_ev);
    } else {
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_OACP_RESPONSE, &resp);
    }
}

/*****************************************************************************
 * OACP Command APIs
 *****************************************************************************/

/**
 * @brief Report whether a control-point procedure is already outstanding.
 *
 * cp_timer_active alone is not sufficient: it is only set from the ATT write
 * response callback, so between a command API returning and that callback
 * running it is still false. cp_pending_opcode is set synchronously by the
 * command APIs and therefore closes that window.
 */
static bool ble_ots_client_cp_busy(const ble_ots_client_conn_ctx_t *ctx)
{
    return ctx->cp_timer_active || ctx->cp_pending_opcode != 0;
}

int ble_ots_client_create_object(uint16_t conn_id,
                                 uint32_t size,
                                 const uint8_t *type_uuid,
                                 uint8_t type_uuid_len)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    if (type_uuid == NULL || (type_uuid_len != 2 && type_uuid_len != 16)) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out, reconnect required");
        return BLE_HS_ETIMEOUT;
    }

    if (ble_ots_client_cp_busy(ctx)) {
        ESP_LOGE(TAG, "CP operation already in progress");
        return BLE_HS_EBUSY;
    }

    if (ctx->handles.oacp_handle == 0) {
        ESP_LOGE(TAG, "OACP handle not discovered");
        return BLE_HS_ENOENT;
    }

    /* Build OACP Create command:
     * [0]     = Op Code (0x01)
     * [1..4]  = Size (UINT32, little-endian)
     * [5..N]  = Type UUID (2 or 16 octets)
     */
    uint8_t buf[1 + 4 + 16]; /* Max size */
    uint16_t buf_len = 0;

    buf[buf_len++] = BLE_OTS_OACP_OPCODE_CREATE;
    buf[buf_len++] = (uint8_t)(size & 0xFF);
    buf[buf_len++] = (uint8_t)((size >> 8) & 0xFF);
    buf[buf_len++] = (uint8_t)((size >> 16) & 0xFF);
    buf[buf_len++] = (uint8_t)((size >> 24) & 0xFF);
    memcpy(&buf[buf_len], type_uuid, type_uuid_len);
    buf_len += type_uuid_len;

    /* Mark the control point busy before the write so a second command issued
     * before the write-response callback runs is rejected. */
    ctx->cp_pending_opcode = BLE_OTS_OACP_OPCODE_CREATE;

    int rc = ble_gattc_write_flat(conn_id, ctx->handles.oacp_handle,
                                  buf, buf_len,
                                  ble_ots_client_oacp_write_cb,
                                  (void *)(uintptr_t)BLE_OTS_OACP_OPCODE_CREATE);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to write OACP Create: rc=%d", rc);
        ctx->cp_pending_opcode = 0;
    }

    return rc;
}

int ble_ots_client_delete_object(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out, reconnect required");
        return BLE_HS_ETIMEOUT;
    }

    if (ble_ots_client_cp_busy(ctx)) {
        ESP_LOGE(TAG, "CP operation already in progress");
        return BLE_HS_EBUSY;
    }

    if (ctx->handles.oacp_handle == 0) {
        ESP_LOGE(TAG, "OACP handle not discovered");
        return BLE_HS_ENOENT;
    }

    uint8_t buf[1];
    buf[0] = BLE_OTS_OACP_OPCODE_DELETE;

    ctx->cp_pending_opcode = BLE_OTS_OACP_OPCODE_DELETE;

    int rc = ble_gattc_write_flat(conn_id, ctx->handles.oacp_handle,
                                  buf, sizeof(buf),
                                  ble_ots_client_oacp_write_cb,
                                  (void *)(uintptr_t)BLE_OTS_OACP_OPCODE_DELETE);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to write OACP Delete: rc=%d", rc);
        ctx->cp_pending_opcode = 0;
    }

    return rc;
}

int ble_ots_client_calculate_checksum(uint16_t conn_id,
                                      uint32_t offset,
                                      uint32_t length)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out, reconnect required");
        return BLE_HS_ETIMEOUT;
    }

    if (ble_ots_client_cp_busy(ctx)) {
        ESP_LOGE(TAG, "CP operation already in progress");
        return BLE_HS_EBUSY;
    }

    if (ctx->handles.oacp_handle == 0) {
        ESP_LOGE(TAG, "OACP handle not discovered");
        return BLE_HS_ENOENT;
    }

    /* Build OACP Calculate Checksum command:
     * [0]     = Op Code (0x03)
     * [1..4]  = Offset (UINT32, little-endian)
     * [5..8]  = Length (UINT32, little-endian)
     */
    uint8_t buf[9];
    buf[0] = BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM;
    buf[1] = (uint8_t)(offset & 0xFF);
    buf[2] = (uint8_t)((offset >> 8) & 0xFF);
    buf[3] = (uint8_t)((offset >> 16) & 0xFF);
    buf[4] = (uint8_t)((offset >> 24) & 0xFF);
    buf[5] = (uint8_t)(length & 0xFF);
    buf[6] = (uint8_t)((length >> 8) & 0xFF);
    buf[7] = (uint8_t)((length >> 16) & 0xFF);
    buf[8] = (uint8_t)((length >> 24) & 0xFF);

    ctx->cp_pending_opcode = BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM;

    int rc = ble_gattc_write_flat(conn_id, ctx->handles.oacp_handle,
                                  buf, sizeof(buf),
                                  ble_ots_client_oacp_write_cb,
                                  (void *)(uintptr_t)BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to write OACP Calculate Checksum: rc=%d", rc);
        ctx->cp_pending_opcode = 0;
    }

    return rc;
}

int ble_ots_client_execute_object(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out, reconnect required");
        return BLE_HS_ETIMEOUT;
    }

    if (ble_ots_client_cp_busy(ctx)) {
        ESP_LOGE(TAG, "CP operation already in progress");
        return BLE_HS_EBUSY;
    }

    if (ctx->handles.oacp_handle == 0) {
        ESP_LOGE(TAG, "OACP handle not discovered");
        return BLE_HS_ENOENT;
    }

    uint8_t buf[1];
    buf[0] = BLE_OTS_OACP_OPCODE_EXECUTE;

    ctx->cp_pending_opcode = BLE_OTS_OACP_OPCODE_EXECUTE;

    int rc = ble_gattc_write_flat(conn_id, ctx->handles.oacp_handle,
                                  buf, sizeof(buf),
                                  ble_ots_client_oacp_write_cb,
                                  (void *)(uintptr_t)BLE_OTS_OACP_OPCODE_EXECUTE);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to write OACP Execute: rc=%d", rc);
        ctx->cp_pending_opcode = 0;
    }

    return rc;
}

int ble_ots_client_read_object_content(uint16_t conn_id,
                                       uint32_t offset,
                                       uint32_t length)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    if (length == 0) {
        ESP_LOGE(TAG, "OACP Read length must be non-zero");
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out, reconnect required");
        return BLE_HS_ETIMEOUT;
    }

    if (ble_ots_client_cp_busy(ctx)) {
        ESP_LOGE(TAG, "CP operation already in progress");
        return BLE_HS_EBUSY;
    }

    if (!ctx->otc_open) {
        ESP_LOGE(TAG, "Object Transfer Channel not open");
        return BLE_HS_ENOENT;
    }

    if (ctx->transfer_in_progress) {
        ESP_LOGE(TAG, "Transfer already in progress");
        return BLE_HS_EBUSY;
    }

    if (ctx->handles.oacp_handle == 0) {
        ESP_LOGE(TAG, "OACP handle not discovered");
        return BLE_HS_ENOENT;
    }

    /* Store transfer parameters for tracking after OACP Success */
    ctx->transfer_offset = offset;
    ctx->transfer_length = length;
    ctx->transfer_received = 0;
    ctx->transfer_opcode = 0;   /* Set only once the server answers Success */

    /* Build OACP Read command:
     * [0]     = Op Code (0x05)
     * [1..4]  = Offset (UINT32, little-endian)
     * [5..8]  = Length (UINT32, little-endian)
     */
    uint8_t buf[9];
    buf[0] = BLE_OTS_OACP_OPCODE_READ;
    buf[1] = (uint8_t)(offset & 0xFF);
    buf[2] = (uint8_t)((offset >> 8) & 0xFF);
    buf[3] = (uint8_t)((offset >> 16) & 0xFF);
    buf[4] = (uint8_t)((offset >> 24) & 0xFF);
    buf[5] = (uint8_t)(length & 0xFF);
    buf[6] = (uint8_t)((length >> 8) & 0xFF);
    buf[7] = (uint8_t)((length >> 16) & 0xFF);
    buf[8] = (uint8_t)((length >> 24) & 0xFF);

    ctx->cp_pending_opcode = BLE_OTS_OACP_OPCODE_READ;

    int rc = ble_gattc_write_flat(conn_id, ctx->handles.oacp_handle,
                                  buf, sizeof(buf),
                                  ble_ots_client_oacp_write_cb,
                                  (void *)(uintptr_t)BLE_OTS_OACP_OPCODE_READ);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to write OACP Read: rc=%d", rc);
        ctx->transfer_offset = 0;
        ctx->transfer_length = 0;
        ctx->cp_pending_opcode = 0;
    } else {
        /* Mark transfer pending immediately to prevent overlapping requests */
        ctx->transfer_in_progress = true;
    }

    return rc;
}

int ble_ots_client_write_object_content(uint16_t conn_id,
                                        uint32_t offset,
                                        uint32_t length,
                                        uint8_t mode)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    /* Same reasoning as ble_ots_client_read_object_content(): a zero-length
     * transfer cannot be tracked over the OTC and would complete on the first
     * chunk, so reject it up front. */
    if (length == 0) {
        ESP_LOGE(TAG, "OACP Write length must be non-zero");
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out, reconnect required");
        return BLE_HS_ETIMEOUT;
    }

    if (ble_ots_client_cp_busy(ctx)) {
        ESP_LOGE(TAG, "CP operation already in progress");
        return BLE_HS_EBUSY;
    }

    if (!ctx->otc_open) {
        ESP_LOGE(TAG, "Object Transfer Channel not open");
        return BLE_HS_ENOENT;
    }

    if (ctx->transfer_in_progress) {
        ESP_LOGE(TAG, "Transfer already in progress");
        return BLE_HS_EBUSY;
    }

    if (ctx->handles.oacp_handle == 0) {
        ESP_LOGE(TAG, "OACP handle not discovered");
        return BLE_HS_ENOENT;
    }

    /* Store transfer parameters for tracking after OACP Success */
    ctx->transfer_offset = offset;
    ctx->transfer_length = length;
    ctx->transfer_sent = 0;
    ctx->transfer_opcode = 0;   /* Set only once the server answers Success */

    /* Build OACP Write command:
     * [0]     = Op Code (0x06)
     * [1..4]  = Offset (UINT32, little-endian)
     * [5..8]  = Length (UINT32, little-endian)
     * [9]     = Mode (UINT8)
     */
    uint8_t buf[10];
    buf[0] = BLE_OTS_OACP_OPCODE_WRITE;
    buf[1] = (uint8_t)(offset & 0xFF);
    buf[2] = (uint8_t)((offset >> 8) & 0xFF);
    buf[3] = (uint8_t)((offset >> 16) & 0xFF);
    buf[4] = (uint8_t)((offset >> 24) & 0xFF);
    buf[5] = (uint8_t)(length & 0xFF);
    buf[6] = (uint8_t)((length >> 8) & 0xFF);
    buf[7] = (uint8_t)((length >> 16) & 0xFF);
    buf[8] = (uint8_t)((length >> 24) & 0xFF);
    buf[9] = mode;

    ctx->cp_pending_opcode = BLE_OTS_OACP_OPCODE_WRITE;

    int rc = ble_gattc_write_flat(conn_id, ctx->handles.oacp_handle,
                                  buf, sizeof(buf),
                                  ble_ots_client_oacp_write_cb,
                                  (void *)(uintptr_t)BLE_OTS_OACP_OPCODE_WRITE);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to write OACP Write: rc=%d", rc);
        ctx->transfer_offset = 0;
        ctx->transfer_length = 0;
        ctx->cp_pending_opcode = 0;
    } else {
        /* Mark transfer pending immediately to prevent overlapping requests.
         * This only reserves the transfer slot — it does NOT authorise data on
         * the OTC; ble_ots_client_send_data() additionally requires
         * transfer_opcode, which is set only when the server answers Success. */
        ctx->transfer_in_progress = true;
    }

    return rc;
}

int ble_ots_client_abort_transfer(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out, reconnect required");
        return BLE_HS_ETIMEOUT;
    }

    /* OTS v1.0 section 4.4 explicitly allows the client to write the Abort Op
     * Code while the server is processing another OACP procedure, so Abort is
     * deliberately exempt from the ble_ots_client_cp_busy() guard — that is
     * the whole point of the opcode. Only a second concurrent Abort is
     * refused. Note that this overwrites cp_pending_opcode, so whichever
     * indication arrives first releases the busy flag; that fails open (an
     * extra command may be accepted) rather than deadlocking the client. */
    if (ctx->cp_pending_opcode == BLE_OTS_OACP_OPCODE_ABORT) {
        ESP_LOGE(TAG, "Abort already in progress");
        return BLE_HS_EBUSY;
    }

    if (ctx->handles.oacp_handle == 0) {
        ESP_LOGE(TAG, "OACP handle not discovered");
        return BLE_HS_ENOENT;
    }

    uint8_t buf[1];
    buf[0] = BLE_OTS_OACP_OPCODE_ABORT;

    uint8_t prev_opcode = ctx->cp_pending_opcode;
    ctx->cp_pending_opcode = BLE_OTS_OACP_OPCODE_ABORT;

    int rc = ble_gattc_write_flat(conn_id, ctx->handles.oacp_handle,
                                  buf, sizeof(buf),
                                  ble_ots_client_oacp_write_cb,
                                  (void *)(uintptr_t)BLE_OTS_OACP_OPCODE_ABORT);
    if (rc != 0) {
        /* The Abort never reached the stack. Leave the transfer state alone:
         * clearing transfer_in_progress and stopping the inactivity timer here
         * would lock out ble_ots_client_send_data() for a Write transfer and
         * remove the only mechanism that could still recover the stalled OTC. */
        ESP_LOGE(TAG, "Failed to write OACP Abort: rc=%d", rc);
        ctx->cp_pending_opcode = prev_opcode;
        return rc;
    }

    /* The Abort is queued — the client stops feeding/expecting object data. */
    ble_ots_client_stop_transfer_timer(ctx);
    ctx->transfer_in_progress = false;
    ctx->transfer_opcode = 0;

    return 0;
}

/*****************************************************************************
 * L2CAP Channel Management
 *****************************************************************************/

/*
 * Some OTS servers transiently reject the LE Credit Based Connection Request
 * for the Object Transfer Channel with "LE_PSM not supported"
 * (BLE_HS_ENOTSUP) — most notably PTS, which unregisters and immediately
 * re-registers PSM 0x0025 right before it expects the OTC to open. A single
 * connect attempt races that re-registration and gets rejected, so we back
 * off briefly and retry a bounded number of times before giving up.
 */
#define OTC_CONNECT_MAX_RETRIES     3
#define OTC_CONNECT_RETRY_DELAY_MS  200

/* Issue a single LE Credit Based Connection Request for the OTC.
 * Caller must have verified the channel is not already open. */
static int ble_ots_client_otc_connect(ble_ots_client_conn_ctx_t *ctx)
{
    struct os_mbuf *sdu_rx = os_msys_get_pkthdr(OTC_MTU, 0);
    if (sdu_rx == NULL) {
        ESP_LOGE(TAG, "Failed to allocate SDU rx buffer");
        return BLE_HS_ENOMEM;
    }

    /* ble_l2cap_connect() takes ownership of sdu_rx on BOTH the success and
     * the failure path: ble_l2cap_sig_connect_nolock() frees it directly on
     * its early error returns, and once ble_l2cap_coc_chan_alloc() has
     * attached it to the channel, every later error frees the channel and with
     * it the SDU. Freeing it here as well would be a double free. */
    int rc = ble_l2cap_connect(ctx->conn_id, PSM_OTS, OTC_MTU, sdu_rx,
                               ble_ots_client_l2cap_event_cb,
                               (void *)(uintptr_t)ctx->conn_id);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate L2CAP connect: rc=%d", rc);
        return rc;
    }

    ESP_LOGI(TAG, "L2CAP connect initiated, conn_id=%d, PSM=0x%04x",
             ctx->conn_id, PSM_OTS);
    return 0;
}

/* Backoff timer callback: re-attempt the OTC connect after an ENOTSUP reject.
 * Runs in the NimBLE host task (callout on the host default event queue), which
 * is required because it calls ble_l2cap_connect() and dispatches app events. */
static void ble_ots_client_otc_retry_timer_cb(struct ble_npl_event *ev)
{
    uint16_t conn_id = (uint16_t)(uintptr_t)ble_npl_event_get_arg(ev);

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL || ctx->otc_open) {
        return;  /* Gone, or already connected in the meantime */
    }

    ESP_LOGW(TAG, "Retrying OTC connect (attempt %d/%d)",
             ctx->otc_retry_count, OTC_CONNECT_MAX_RETRIES);

    if (ble_ots_client_otc_connect(ctx) != 0) {
        /* Could not even queue the request — report the channel as closed */
        ble_ots_client_channel_closed_t closed_evt = {
            .conn_id = conn_id,
            .reason = (uint8_t)BLE_HS_ENOTSUP,
        };
        ble_ots_client_dispatch_event(conn_id,
                                      BLE_OTS_CLIENT_EVT_CHANNEL_CLOSED, &closed_evt);
    }
}

int ble_ots_client_open_channel(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->otc_open) {
        ESP_LOGW(TAG, "Object Transfer Channel already open");
        return BLE_HS_EALREADY;
    }

    /* Create the transfer timer if not yet created */
    int rc = ble_ots_client_create_transfer_timer(ctx);
    if (rc != 0) {
        return rc;
    }

    /* Create the OTC connect retry timer if not yet created */
    if (!ctx->otc_retry_timer_inited) {
        rc = ble_npl_callout_init(&ctx->otc_retry_timer, nimble_port_get_dflt_eventq(),
                                  ble_ots_client_otc_retry_timer_cb,
                                  (void *)(uintptr_t)ctx->conn_id);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to init OTC retry callout: %d", rc);
            return BLE_HS_ENOMEM;
        }
        ctx->otc_retry_timer_inited = true;
    }

    ctx->otc_retry_count = 0;
    return ble_ots_client_otc_connect(ctx);
}

int ble_ots_client_close_channel(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (!ctx->otc_open || ctx->otc_chan == NULL) {
        ESP_LOGW(TAG, "Object Transfer Channel not open");
        return BLE_HS_ENOENT;
    }

    int rc = ble_l2cap_disconnect(ctx->otc_chan);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to disconnect L2CAP channel: rc=%d", rc);
        return rc;
    }

    /* Stop any active transfer only after disconnect succeeded */
    ble_ots_client_stop_transfer_timer(ctx);
    ctx->transfer_in_progress = false;
    ctx->transfer_opcode = 0;

    return 0;
}

/*****************************************************************************
 * L2CAP Send Data
 *****************************************************************************/

int ble_ots_client_send_data(uint16_t conn_id,
                             const uint8_t *data,
                             uint16_t data_len)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    if (data == NULL || data_len == 0) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        ESP_LOGE(TAG, "No context for conn_id=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (!ctx->otc_open || ctx->otc_chan == NULL) {
        ESP_LOGE(TAG, "Object Transfer Channel not open");
        return BLE_HS_ENOENT;
    }

    if (!ctx->transfer_in_progress) {
        ESP_LOGE(TAG, "No transfer in progress");
        return BLE_HS_EINVAL;
    }

    /* transfer_in_progress is set as soon as the OACP Write command is queued,
     * to reserve the transfer slot. It is therefore NOT proof that the server
     * authorised the transfer. transfer_opcode is set only by the OACP Write
     * Success indication, so it is what actually gates data on the OTC —
     * without this check the app could push object data before the server has
     * accepted (or while it is rejecting) the Write procedure. */
    if (ctx->transfer_opcode != BLE_OTS_OACP_OPCODE_WRITE) {
        ESP_LOGE(TAG, "OACP Write not acknowledged yet, cannot send object data");
        return BLE_HS_EINVAL;
    }

    /* Allocate an mbuf for the data */
    struct os_mbuf *sdu_tx = os_msys_get_pkthdr(data_len, 0);
    if (sdu_tx == NULL) {
        ESP_LOGE(TAG, "Failed to allocate SDU tx buffer");
        return BLE_HS_ENOMEM;
    }

    int rc = os_mbuf_append(sdu_tx, data, data_len);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to append data to mbuf");
        os_mbuf_free_chain(sdu_tx);
        return BLE_HS_ENOMEM;
    }

    /* mbuf ownership rules of ble_l2cap_send()/ble_l2cap_coc_send():
     *   0                -> the whole SDU went out and the stack freed it.
     *   BLE_HS_ESTALLED  -> the stack HAS taken the SDU (it is parked in
     *                       chan->coc_tx.sdus[0]) and will finish sending it
     *                       when the peer grants credits; freeing it here
     *                       would be a double free / use-after-free.
     *   anything else    -> the stack either rejected the SDU before taking it
     *                       (EBADDATA/EBUSY) or dropped its reference without
     *                       freeing, because ble_l2cap_coc_send() calls
     *                       ble_l2cap_coc_continue_tx() with free_on_err=false.
     *                       In both cases the caller still owns the mbuf and
     *                       must free it, or it leaks. */
    rc = ble_l2cap_send(ctx->otc_chan, sdu_tx);
    if (rc == BLE_HS_ESTALLED) {
        /* Credits exhausted mid-SDU. The stack owns and will deliver the rest,
         * so count the octets as sent, but do NOT declare the transfer
         * complete yet — completion is detected on TX_UNSTALLED once the SDU
         * has actually drained. Keep the inactivity timer armed so a peer that
         * never returns credits still trips the timeout. */
        ESP_LOGD(TAG, "L2CAP send stalled, waiting for unstall");
        ctx->transfer_sent += data_len;
        ble_ots_client_start_transfer_timer(ctx);
        return 0;
    } else if (rc != 0) {
        ESP_LOGE(TAG, "Failed to send L2CAP data: rc=%d", rc);
        os_mbuf_free_chain(sdu_tx);
        return rc;
    }

    /* Data sent successfully — update tracking */
    ctx->transfer_sent += data_len;

    /* Reset the transfer inactivity timer */
    ble_ots_client_start_transfer_timer(ctx);

    /* Dispatch DATA_SENT event */
    uint32_t remaining = 0;
    if (ctx->transfer_length > ctx->transfer_sent) {
        remaining = ctx->transfer_length - ctx->transfer_sent;
    }

    ble_ots_client_data_sent_t evt = {
        .data_len = data_len,
        .remaining = remaining,
    };

    ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_DATA_SENT, &evt);

    /* Check if transfer is complete */
    if (ctx->transfer_sent >= ctx->transfer_length) {
        ESP_LOGI(TAG, "Write transfer complete: sent=%" PRIu32, ctx->transfer_sent);
        ble_ots_client_stop_transfer_timer(ctx);
        ctx->transfer_in_progress = false;
        ctx->transfer_opcode = 0;
    }

    return 0;
}

/*****************************************************************************
 * L2CAP Event Handler
 *****************************************************************************/

static int ble_ots_client_l2cap_event_cb(struct ble_l2cap_event *event, void *arg)
{
    uint16_t conn_id = (uint16_t)(uintptr_t)arg;

    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_CONNECTED: {
        ESP_LOGI(TAG, "L2CAP COC connected: status=%d, conn_handle=%d",
                 event->connect.status, event->connect.conn_handle);

        conn_id = event->connect.conn_handle;
        ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
        if (ctx == NULL) {
            ESP_LOGE(TAG, "No context for conn_id=%d on L2CAP connect", conn_id);
            return 0;
        }

        if (event->connect.status != 0) {
            /* Connection failed */
            ESP_LOGE(TAG, "L2CAP COC connect failed: status=%d", event->connect.status);
            ctx->otc_chan = NULL;
            ctx->otc_open = false;

            /* A peer that is momentarily not accepting the PSM (e.g. PTS while
             * it re-registers PSM 0x0025) rejects with ENOTSUP. Back off and
             * retry a bounded number of times before giving up. */
            if (event->connect.status == BLE_HS_ENOTSUP &&
                ctx->otc_retry_timer_inited &&
                ctx->otc_retry_count < OTC_CONNECT_MAX_RETRIES) {
                ctx->otc_retry_count++;
                ble_npl_error_t err =
                    ble_npl_callout_reset(&ctx->otc_retry_timer,
                                          ble_npl_time_ms_to_ticks32(OTC_CONNECT_RETRY_DELAY_MS));
                if (err == BLE_NPL_OK) {
                    ESP_LOGW(TAG, "OTC connect rejected (ENOTSUP); retry %d/%d in %d ms",
                             ctx->otc_retry_count, OTC_CONNECT_MAX_RETRIES,
                             OTC_CONNECT_RETRY_DELAY_MS);
                    return 0;
                }
                ESP_LOGE(TAG, "Failed to arm OTC retry timer: %d", err);
            }

            ble_ots_client_channel_closed_t closed_evt = {
                .conn_id = conn_id,
                .reason = (uint8_t)event->connect.status,
            };
            ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_CHANNEL_CLOSED, &closed_evt);
            return 0;
        }

        ctx->otc_chan = event->connect.chan;
        ctx->otc_open = true;
        ctx->otc_retry_count = 0;
        if (ctx->otc_retry_timer_inited) {
            ble_npl_callout_stop(&ctx->otc_retry_timer);
        }

        /* Get channel info for MTU/MPS */
        struct ble_l2cap_chan_info chan_info;
        memset(&chan_info, 0, sizeof(chan_info));
        ble_l2cap_get_chan_info(event->connect.chan, &chan_info);

        ble_ots_client_channel_info_t open_evt = {
            .conn_id = conn_id,
            .channel_id = chan_info.scid,
            .mtu = chan_info.our_coc_mtu,
            .mps = chan_info.our_l2cap_mtu,
        };

        ESP_LOGI(TAG, "OTC opened: channel_id=%d, mtu=%d, mps=%d",
                 open_evt.channel_id, open_evt.mtu, open_evt.mps);

        ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_CHANNEL_OPEN, &open_evt);
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_DISCONNECTED: {
        ESP_LOGI(TAG, "L2CAP COC disconnected: conn_handle=%d",
                 event->disconnect.conn_handle);

        conn_id = event->disconnect.conn_handle;
        ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
        if (ctx == NULL) {
            ESP_LOGW(TAG, "No context for conn_id=%d on L2CAP disconnect", conn_id);
            return 0;
        }

        /* Stop any active transfer */
        ble_ots_client_stop_transfer_timer(ctx);
        ctx->transfer_in_progress = false;
        ctx->transfer_opcode = 0;
        ctx->otc_chan = NULL;
        ctx->otc_open = false;

        ble_ots_client_channel_closed_t closed_evt = {
            .conn_id = conn_id,
            .reason = 0,  /* Local or remote close */
        };

        ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_CHANNEL_CLOSED, &closed_evt);
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED: {
        conn_id = event->receive.conn_handle;
        ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
        if (ctx == NULL) {
            ESP_LOGW(TAG, "No context for conn_id=%d on L2CAP data received", conn_id);
            os_mbuf_free_chain(event->receive.sdu_rx);
            return 0;
        }

        struct os_mbuf *sdu_rx = event->receive.sdu_rx;
        uint16_t data_len = OS_MBUF_PKTLEN(sdu_rx);

        ESP_LOGD(TAG, "L2CAP data received: conn_id=%d, len=%d", conn_id, data_len);

        /* Drop unsolicited or trailing data. The OTC stays open after a
         * transfer is aborted, times out or completes, so in-flight SDUs can
         * still arrive. Feeding them into the tracking counters would corrupt
         * the offsets of the next transfer, and — worse — re-arm the
         * inactivity timer for a transfer that will never make progress,
         * producing a spurious TRANSFER_TIMEOUT event later. Just replenish
         * the channel credits and discard. */
        if (!ctx->transfer_in_progress) {
            ESP_LOGW(TAG, "Discarding %d bytes on OTC with no transfer in progress", data_len);
            os_mbuf_free_chain(sdu_rx);
            struct os_mbuf *stale_sdu = os_msys_get_pkthdr(OTC_MTU, 0);
            if (stale_sdu != NULL) {
                ble_l2cap_recv_ready(event->receive.chan, stale_sdu);
            }
            return 0;
        }

        /* Copy data from mbuf. A failure here means this chunk is lost; since
         * the OTC carries no retransmission and transfer_received would stop
         * tracking the peer's byte stream, every later chunk would be handed
         * to the application at the wrong offset. Tear the channel down
         * instead of silently corrupting the object — the resulting
         * COC_DISCONNECTED event tells the application the transfer failed. */
        uint8_t *data_buf = malloc(data_len);
        if (data_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate buffer for received data, aborting transfer");
            os_mbuf_free_chain(sdu_rx);
            ble_ots_client_stop_transfer_timer(ctx);
            ctx->transfer_in_progress = false;
            ctx->transfer_opcode = 0;
            if (ctx->otc_chan != NULL) {
                ble_l2cap_disconnect(ctx->otc_chan);
            }
            return 0;
        }

        int rc = os_mbuf_copydata(sdu_rx, 0, data_len, data_buf);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to copy received data from mbuf, aborting transfer");
            free(data_buf);
            os_mbuf_free_chain(sdu_rx);
            ble_ots_client_stop_transfer_timer(ctx);
            ctx->transfer_in_progress = false;
            ctx->transfer_opcode = 0;
            if (ctx->otc_chan != NULL) {
                ble_l2cap_disconnect(ctx->otc_chan);
            }
            return 0;
        }

        /* Free the received SDU mbuf chain — ownership was transferred to us */
        os_mbuf_free_chain(sdu_rx);

        /* Calculate the current offset for this chunk */
        uint32_t chunk_offset = ctx->transfer_offset + ctx->transfer_received;

        /* Update tracking */
        ctx->transfer_received += data_len;

        /* Reset the transfer inactivity timer */
        ble_ots_client_start_transfer_timer(ctx);

        /* Dispatch DATA_RECEIVED event */
        ble_ots_client_data_received_t recv_evt = {
            .data = data_buf,
            .data_len = data_len,
            .offset = chunk_offset,
        };

        ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_DATA_RECEIVED, &recv_evt);

        free(data_buf);

        /* Check if transfer is complete */
        if (ctx->transfer_received >= ctx->transfer_length) {
            ESP_LOGI(TAG, "Read transfer complete: received=%" PRIu32, ctx->transfer_received);
            ble_ots_client_stop_transfer_timer(ctx);
            ctx->transfer_in_progress = false;
            ctx->transfer_opcode = 0;
        }

        /* Provide a new SDU rx buffer for the next receive */
        struct os_mbuf *next_sdu = os_msys_get_pkthdr(OTC_MTU, 0);
        if (next_sdu != NULL) {
            ble_l2cap_recv_ready(event->receive.chan, next_sdu);
        }

        return 0;
    }

    case BLE_L2CAP_EVENT_COC_TX_UNSTALLED: {
        conn_id = event->tx_unstalled.conn_handle;
        ESP_LOGD(TAG, "L2CAP TX un-stalled: conn_id=%d, status=%d",
                 conn_id, event->tx_unstalled.status);

        ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
        if (ctx == NULL) {
            return 0;
        }

        /* The previously stalled SDU has now drained. If it was the last one,
         * complete the transfer here — ble_ots_client_send_data() deliberately
         * leaves completion to this point when it returns on ESTALLED. */
        if (ctx->transfer_in_progress && ctx->transfer_sent >= ctx->transfer_length) {
            ESP_LOGI(TAG, "Write transfer complete (un-stalled): sent=%" PRIu32,
                     ctx->transfer_sent);
            ble_ots_client_stop_transfer_timer(ctx);
            ctx->transfer_in_progress = false;
            ctx->transfer_opcode = 0;
        } else if (ctx->transfer_in_progress) {
            /* Reset the transfer inactivity timer */
            ble_ots_client_start_transfer_timer(ctx);
        }

        /* Dispatch DATA_SENT event to let the application know it can send more */
        uint32_t remaining = 0;
        if (ctx->transfer_length > ctx->transfer_sent) {
            remaining = ctx->transfer_length - ctx->transfer_sent;
        }

        ble_ots_client_data_sent_t sent_evt = {
            .data_len = 0,  /* No new data was sent in this event */
            .remaining = remaining,
        };

        ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_DATA_SENT, &sent_evt);
        return 0;
    }

    default:
        ESP_LOGD(TAG, "Unhandled L2CAP event: type=%d", event->type);
        return 0;
    }
}
