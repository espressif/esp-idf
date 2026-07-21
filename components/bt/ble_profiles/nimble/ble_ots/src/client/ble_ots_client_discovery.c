/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>

#include "esp_log.h"

#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_npl.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_uuid.h"

#include "ble_ots_client.h"
#include "ble_ots_client_int.h"

static const char *TAG = "ots_client_disc";

/*****************************************************************************
 * Discovery State Machine Steps
 *****************************************************************************/
enum {
    DISC_STATE_IDLE = 0,
    DISC_STATE_SVC,
    DISC_STATE_CHRS,
    DISC_STATE_DSCS,
    DISC_STATE_OACP_CCCD,
    DISC_STATE_OLCP_CCCD,
    DISC_STATE_DONE,
};

/*****************************************************************************
 * Global Module State
 *****************************************************************************/
ble_ots_client_state_t *g_ots_client = NULL;

/*****************************************************************************
 * Forward Declarations
 *****************************************************************************/
static int ots_client_gap_event_handler(struct ble_gap_event *event, void *arg);
static int disc_svc_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                       const struct ble_gatt_svc *service, void *arg);
static int disc_chr_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                       const struct ble_gatt_chr *chr, void *arg);
static int disc_dsc_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                       uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                       void *arg);
static int write_cccd_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                         struct ble_gatt_attr *attr, void *arg);
static int feature_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                           struct ble_gatt_attr *attr, void *arg);
static void disc_complete(uint16_t conn_handle, int status);
static int disc_next_step(uint16_t conn_handle);

/*****************************************************************************
 * Context Management
 *****************************************************************************/

static ble_ots_client_conn_ctx_t *alloc_conn_ctx(uint16_t conn_id)
{
    if (!g_ots_client) {
        return NULL;
    }

    /* Check if context already exists for this conn_id — fully clean up and
     * release it so the allocation loop below can assign a fresh slot. */
    for (int i = 0; i < BLE_OTS_CLIENT_MAX_CONNECTIONS; i++) {
        if (g_ots_client->conns[i].in_use &&
            g_ots_client->conns[i].conn_id == conn_id) {
            ble_ots_client_remove_conn_ctx(conn_id);
            break;
        }
    }

    /* Allocate a free slot */
    for (int i = 0; i < BLE_OTS_CLIENT_MAX_CONNECTIONS; i++) {
        if (!g_ots_client->conns[i].in_use) {
            ble_ots_client_conn_ctx_t *ctx = &g_ots_client->conns[i];
            memset(ctx, 0, sizeof(*ctx));
            ctx->conn_id = conn_id;
            ctx->in_use = true;
            ctx->server_supports_read_long = true;
            ctx->server_supports_write_long = true;

            /* Timers (CP / transfer / OTC retry) are NimBLE callouts created
             * lazily on first use; nothing to do here after the memset. */

            return ctx;
        }
    }

    ESP_LOGE(TAG, "No free connection context slots");
    return NULL;
}

ble_ots_client_conn_ctx_t *ble_ots_client_get_conn_ctx(uint16_t conn_id)
{
    if (!g_ots_client) {
        return NULL;
    }

    for (int i = 0; i < BLE_OTS_CLIENT_MAX_CONNECTIONS; i++) {
        if (g_ots_client->conns[i].in_use &&
            g_ots_client->conns[i].conn_id == conn_id) {
            return &g_ots_client->conns[i];
        }
    }
    return NULL;
}

void ble_ots_client_remove_conn_ctx(uint16_t conn_id)
{
    if (!g_ots_client) {
        return;
    }

    for (int i = 0; i < BLE_OTS_CLIENT_MAX_CONNECTIONS; i++) {
        ble_ots_client_conn_ctx_t *ctx = &g_ots_client->conns[i];
        if (ctx->in_use && ctx->conn_id == conn_id) {
            /* Disconnect L2CAP channel if open */
            if (ctx->otc_open && ctx->otc_chan != NULL) {
                ble_l2cap_disconnect(ctx->otc_chan);
                ctx->otc_chan = NULL;
                ctx->otc_open = false;
            }
            /* Stop and release the CP timer callout */
            if (ctx->cp_timer_inited) {
                ble_npl_callout_stop(&ctx->cp_timer);
                ble_npl_callout_deinit(&ctx->cp_timer);
                ctx->cp_timer_inited = false;
                ctx->cp_timer_active = false;
            }
            /* Stop and release the transfer timer callout */
            if (ctx->transfer_timer_inited) {
                ble_npl_callout_stop(&ctx->transfer_timer);
                ble_npl_callout_deinit(&ctx->transfer_timer);
                ctx->transfer_timer_inited = false;
                ctx->transfer_timer_active = false;
            }
            /* Stop and release the OTC connect retry callout */
            if (ctx->otc_retry_timer_inited) {
                ble_npl_callout_stop(&ctx->otc_retry_timer);
                ble_npl_callout_deinit(&ctx->otc_retry_timer);
                ctx->otc_retry_timer_inited = false;
            }
            /* Remove any pending deferred OACP dispatch. ble_npl_event_deinit()
             * is required as well: the ESP-IDF NPL port allocates the internal
             * event object on first ble_npl_event_init(), and alloc_conn_ctx()
             * memsets the whole context when the slot is reused — which would
             * drop the pointer and leak the block. */
            if (ctx->oacp_dispatch_ev_ready) {
                ble_npl_eventq_remove(nimble_port_get_dflt_eventq(),
                                      &ctx->oacp_dispatch_ev);
                ble_npl_event_deinit(&ctx->oacp_dispatch_ev);
                ctx->oacp_dispatch_ev_ready = false;
            }
            ctx->in_use = false;
            ESP_LOGI(TAG, "Removed conn ctx for conn_handle=%d", conn_id);
            return;
        }
    }
}

/*****************************************************************************
 * Event Dispatch
 *****************************************************************************/

void ble_ots_client_dispatch_event(uint16_t conn_id,
                                   ble_ots_client_event_t event,
                                   const void *param)
{
    if (g_ots_client && g_ots_client->app_cb) {
        g_ots_client->app_cb(conn_id, event, param);
    }
}

/*****************************************************************************
 * Control-Point Timer
 *****************************************************************************/

/* CP-timeout handler. The CP timer is a NimBLE callout bound to the host
 * default event queue, so this runs in the NimBLE host task — serialised with
 * the response handlers that stop the timer, and honouring the documented
 * "callback runs in the BLE host task" contract for the app callback. */
static void cp_timer_cb(struct ble_npl_event *ev)
{
    uint16_t conn_id = (uint16_t)(uintptr_t)ble_npl_event_get_arg(ev);

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (!ctx) {
        return;
    }

    /* Guard against a callout that was already stopped but whose event had
     * been posted to the queue before the stop took effect. */
    if (!ctx->cp_timer_active) {
        ESP_LOGD(TAG, "CP timeout ignored (timer already stopped), conn_handle=%d", conn_id);
        return;
    }

    ESP_LOGW(TAG, "CP timer expired for conn_handle=%d", conn_id);

    ctx->cp_timer_active = false;
    ctx->cp_timed_out = true;

    ble_ots_client_transfer_timeout_t evt = {
        .conn_id = conn_id,
        .opcode = ctx->cp_pending_opcode,
    };

    /* The procedure is over (unsuccessfully): release the synchronous busy
     * flag so it does not block later control-point operations. */
    ctx->cp_pending_opcode = 0;

    ble_ots_client_dispatch_event(conn_id, BLE_OTS_CLIENT_EVT_CP_TIMEOUT, &evt);
}

int ble_ots_client_start_cp_timer(uint16_t conn_id, uint32_t timeout_ms)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (!ctx) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->cp_timed_out) {
        ESP_LOGE(TAG, "CP timed out previously, cannot start new timer");
        return BLE_HS_EREJECT;
    }

    /* Initialise the callout lazily on first use */
    if (!ctx->cp_timer_inited) {
        int rc = ble_npl_callout_init(&ctx->cp_timer, nimble_port_get_dflt_eventq(),
                                      cp_timer_cb, (void *)(uintptr_t)conn_id);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to init CP timer callout: %d", rc);
            return BLE_HS_ENOMEM;
        }
        ctx->cp_timer_inited = true;
    }

    /* ble_npl_callout_reset() re-arms an already-running callout, so there is
     * no need to stop it first. */
    ble_npl_error_t err = ble_npl_callout_reset(&ctx->cp_timer,
                                                ble_npl_time_ms_to_ticks32(timeout_ms));
    if (err != BLE_NPL_OK) {
        ESP_LOGE(TAG, "Failed to start CP timer: %d", err);
        return BLE_HS_EOS;
    }

    ctx->cp_timer_active = true;
    ESP_LOGD(TAG, "CP timer started: %lu ms, conn_handle=%d", (unsigned long)timeout_ms, conn_id);
    return 0;
}

void ble_ots_client_stop_cp_timer(uint16_t conn_id)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (!ctx) {
        return;
    }

    /* Stop unconditionally (not gated on cp_timer_active) so a callout can
     * never be left armed because the flag was out of step with the callout. */
    if (ctx->cp_timer_inited) {
        ble_npl_callout_stop(&ctx->cp_timer);
        ctx->cp_timer_active = false;
        ESP_LOGD(TAG, "CP timer stopped for conn_handle=%d", conn_id);
    }
}

/*****************************************************************************
 * Init / Deinit
 *****************************************************************************/

int ble_ots_client_init(ble_ots_client_event_cb_t callback)
{
    if (!callback) {
        ESP_LOGE(TAG, "Callback must not be NULL");
        return BLE_HS_EINVAL;
    }

    if (g_ots_client && g_ots_client->initialized) {
        ESP_LOGE(TAG, "Already initialized, call deinit first");
        return BLE_HS_EALREADY;
    }

    g_ots_client = calloc(1, sizeof(ble_ots_client_state_t));
    if (!g_ots_client) {
        ESP_LOGE(TAG, "Failed to allocate module state");
        return BLE_HS_ENOMEM;
    }

    g_ots_client->app_cb = callback;
    g_ots_client->initialized = true;

    /* Register GAP event listener. */
    int rc = ble_gap_event_listener_register(&g_ots_client->gap_listener,
                                             ots_client_gap_event_handler,
                                             NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to register GAP listener: %d", rc);
        free(g_ots_client);
        g_ots_client = NULL;
        return rc;
    }

    ESP_LOGI(TAG, "OTS client initialized");
    return 0;
}

int ble_ots_client_deinit(void)
{
    if (!g_ots_client) {
        return 0;
    }

    /* Remove all connection contexts */
    for (int i = 0; i < BLE_OTS_CLIENT_MAX_CONNECTIONS; i++) {
        if (g_ots_client->conns[i].in_use) {
            ble_ots_client_remove_conn_ctx(g_ots_client->conns[i].conn_id);
        }
    }

    /* Unregister the GAP event listener */
    ble_gap_event_listener_unregister(&g_ots_client->gap_listener);

    free(g_ots_client);
    g_ots_client = NULL;

    ESP_LOGI(TAG, "OTS client deinitialized");
    return 0;
}

/*****************************************************************************
 * Service Discovery State Machine
 *****************************************************************************/

int ble_ots_client_discover_service(uint16_t conn_id)
{
    if (!g_ots_client || !g_ots_client->initialized) {
        ESP_LOGE(TAG, "OTS client not initialized");
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = alloc_conn_ctx(conn_id);
    if (!ctx) {
        return BLE_HS_ENOMEM;
    }

    ctx->disc_state = DISC_STATE_SVC;

    ble_uuid16_t svc_uuid = BLE_UUID16_INIT(BLE_OTS_UUID_OTS_SERVICE);
    int rc = ble_gattc_disc_svc_by_uuid(conn_id, &svc_uuid.u, disc_svc_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start service discovery: %d", rc);
        ble_ots_client_remove_conn_ctx(conn_id);
        return rc;
    }

    ESP_LOGI(TAG, "OTS service discovery started, conn_handle=%d", conn_id);
    return 0;
}

static int disc_svc_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                       const struct ble_gatt_svc *service, void *arg)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (!ctx) {
        return BLE_HS_ENOTCONN;
    }

    if (error->status == 0 && service != NULL) {
        /* Service found — store handles */
        ctx->disc_svc_start_handle = service->start_handle;
        ctx->disc_svc_end_handle = service->end_handle;
        ESP_LOGI(TAG, "OTS service found: start=0x%04x end=0x%04x",
                 service->start_handle, service->end_handle);
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        /* Discovery procedure complete */
        if (ctx->disc_svc_start_handle == 0) {
            ESP_LOGE(TAG, "OTS service not found");
            disc_complete(conn_handle, BLE_HS_ENOENT);
            return 0;
        }
        /* Move to characteristic discovery */
        ctx->disc_state = DISC_STATE_CHRS;
        int rc = disc_next_step(conn_handle);
        if (rc != 0) {
            disc_complete(conn_handle, rc);
        }
        return 0;
    }

    /* Error */
    ESP_LOGE(TAG, "Service discovery error: status=%d", error->status);
    disc_complete(conn_handle, error->status);
    return 0;
}

static void map_chr_uuid_to_handle(ble_ots_client_conn_ctx_t *ctx,
                                   const struct ble_gatt_chr *chr)
{
    uint16_t uuid16 = 0;

    if (chr->uuid.u.type == BLE_UUID_TYPE_16) {
        uuid16 = BLE_UUID16(&chr->uuid.u)->value;
    } else {
        return; /* OTS uses only 16-bit UUIDs */
    }

    switch (uuid16) {
    case BLE_OTS_UUID_OTS_FEATURE:
        ctx->handles.ots_feature_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OBJECT_NAME:
        ctx->handles.object_name_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OBJECT_TYPE:
        ctx->handles.object_type_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OBJECT_SIZE:
        ctx->handles.object_size_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OBJECT_FIRST_CREATED:
        ctx->handles.first_created_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OBJECT_LAST_MODIFIED:
        ctx->handles.last_modified_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OBJECT_ID:
        ctx->handles.object_id_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OBJECT_PROPERTIES:
        ctx->handles.object_properties_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OACP:
        ctx->handles.oacp_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OLCP:
        ctx->handles.olcp_handle = chr->val_handle;
        break;
    case BLE_OTS_UUID_OBJECT_LIST_FILTER:
        /* Spec mandates either three instances or none; store them in discovery order */
        for (int i = 0; i < 3; i++) {
            if (ctx->handles.object_list_filter_handle[i] == 0) {
                ctx->handles.object_list_filter_handle[i] = chr->val_handle;
                break;
            }
        }
        break;
    case BLE_OTS_UUID_OBJECT_CHANGED:
        ctx->handles.object_changed_handle = chr->val_handle;
        break;
    default:
        /* Unknown characteristic — be tolerant */
        break;
    }
}

static int disc_chr_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                       const struct ble_gatt_chr *chr, void *arg)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (!ctx) {
        return BLE_HS_ENOTCONN;
    }

    if (error->status == 0 && chr != NULL) {
        map_chr_uuid_to_handle(ctx, chr);
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        /* Characteristic discovery complete — move to descriptor discovery */
        ESP_LOGI(TAG, "Characteristic discovery complete");

        /* Determine multi-object server */
        ctx->multi_object_server = (ctx->handles.olcp_handle != 0);

        /* Move to descriptor discovery */
        ctx->disc_state = DISC_STATE_DSCS;
        int rc = disc_next_step(conn_handle);
        if (rc != 0) {
            disc_complete(conn_handle, rc);
        }
        return 0;
    }

    ESP_LOGE(TAG, "Characteristic discovery error: status=%d", error->status);
    disc_complete(conn_handle, error->status);
    return 0;
}

/**
 * @brief Find the smallest characteristic value handle that is strictly greater
 *        than the given handle, within the discovered OTS characteristics.
 *
 * Used to compute the upper bound for CCCD-to-characteristic mapping.
 *
 * @param handles  Pointer to the discovered handle set
 * @param chr_handle  The characteristic value handle to find the upper bound for
 * @param svc_end     The service end handle (used as fallback upper bound)
 * @return The upper bound handle (exclusive); may be 0x10000 when svc_end is
 *         0xFFFF, hence the wider return type to avoid uint16_t overflow.
 */
static uint32_t find_next_chr_handle(const ble_ots_client_char_handles_t *handles,
                                     uint16_t chr_handle, uint16_t svc_end)
{
    /* Use a 32-bit bound so that svc_end + 1 does not overflow to 0 when
     * svc_end is 0xFFFF (a legal BLE service end handle). */
    uint32_t upper = (uint32_t)svc_end + 1; /* default: end of service range (exclusive) */

    /* Collect all known characteristic value handles */
    const uint16_t all_handles[] = {
        handles->ots_feature_handle,
        handles->object_name_handle,
        handles->object_type_handle,
        handles->object_size_handle,
        handles->first_created_handle,
        handles->last_modified_handle,
        handles->object_id_handle,
        handles->object_properties_handle,
        handles->oacp_handle,
        handles->olcp_handle,
        handles->object_list_filter_handle[0],
        handles->object_list_filter_handle[1],
        handles->object_list_filter_handle[2],
        handles->object_changed_handle,
    };

    for (size_t i = 0; i < sizeof(all_handles) / sizeof(all_handles[0]); i++) {
        if (all_handles[i] > chr_handle && all_handles[i] < upper) {
            upper = all_handles[i];
        }
    }

    return upper;
}

static int disc_dsc_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                       uint16_t chr_val_handle, const struct ble_gatt_dsc *dsc,
                       void *arg)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (!ctx) {
        return BLE_HS_ENOTCONN;
    }

    if (error->status == 0 && dsc != NULL) {
        /* Check if this is a CCCD */
        if (dsc->uuid.u.type == BLE_UUID_TYPE_16 &&
            BLE_UUID16(&dsc->uuid.u)->value == BLE_GATT_DSC_CLT_CFG_UUID16) {
            /* Map CCCD to the correct characteristic using upper-bound checks */
            if (ctx->handles.oacp_handle != 0 &&
                dsc->handle > ctx->handles.oacp_handle &&
                dsc->handle < find_next_chr_handle(&ctx->handles, ctx->handles.oacp_handle, ctx->disc_svc_end_handle) &&
                ctx->handles.oacp_cccd_handle == 0) {
                ctx->handles.oacp_cccd_handle = dsc->handle;
                ESP_LOGD(TAG, "OACP CCCD found: handle=0x%04x", dsc->handle);
            } else if (ctx->handles.olcp_handle != 0 &&
                       dsc->handle > ctx->handles.olcp_handle &&
                       dsc->handle < find_next_chr_handle(&ctx->handles, ctx->handles.olcp_handle, ctx->disc_svc_end_handle) &&
                       ctx->handles.olcp_cccd_handle == 0) {
                ctx->handles.olcp_cccd_handle = dsc->handle;
                ESP_LOGD(TAG, "OLCP CCCD found: handle=0x%04x", dsc->handle);
            } else if (ctx->handles.object_changed_handle != 0 &&
                       dsc->handle > ctx->handles.object_changed_handle &&
                       dsc->handle < find_next_chr_handle(&ctx->handles, ctx->handles.object_changed_handle, ctx->disc_svc_end_handle) &&
                       ctx->handles.object_changed_cccd_handle == 0) {
                ctx->handles.object_changed_cccd_handle = dsc->handle;
                ESP_LOGD(TAG, "Object Changed CCCD found: handle=0x%04x", dsc->handle);
            }
        }
        return 0;
    }

    if (error->status == BLE_HS_EDONE) {
        /* Descriptor discovery complete — move to CCCD configuration */
        ESP_LOGI(TAG, "Descriptor discovery complete");
        ctx->disc_state = DISC_STATE_OACP_CCCD;
        int rc = disc_next_step(conn_handle);
        if (rc != 0) {
            disc_complete(conn_handle, rc);
        }
        return 0;
    }

    ESP_LOGE(TAG, "Descriptor discovery error: status=%d", error->status);
    disc_complete(conn_handle, error->status);
    return 0;
}

static int write_cccd_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                         struct ble_gatt_attr *attr, void *arg)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (!ctx) {
        return 0;
    }

    if (error->status != 0) {
        ESP_LOGE(TAG, "CCCD write failed: status=%d handle=0x%04x",
                 error->status, attr ? attr->handle : 0);
        disc_complete(conn_handle, error->status);
        return 0;
    }

    ESP_LOGD(TAG, "CCCD write success, disc_state=%d", ctx->disc_state);

    /* Advance to next step */
    if (ctx->disc_state == DISC_STATE_OACP_CCCD) {
        ctx->disc_state = DISC_STATE_OLCP_CCCD;
    } else if (ctx->disc_state == DISC_STATE_OLCP_CCCD) {
        ctx->disc_state = DISC_STATE_DONE;
    }

    int rc = disc_next_step(conn_handle);
    if (rc != 0) {
        disc_complete(conn_handle, rc);
    }
    return 0;
}

static int disc_next_step(uint16_t conn_handle)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (!ctx) {
        return BLE_HS_ENOTCONN;
    }

    int rc;

    switch (ctx->disc_state) {
    case DISC_STATE_CHRS:
        rc = ble_gattc_disc_all_chrs(conn_handle,
                                     ctx->disc_svc_start_handle,
                                     ctx->disc_svc_end_handle,
                                     disc_chr_cb, NULL);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to start chr discovery: %d", rc);
        }
        return rc;

    case DISC_STATE_DSCS:
        /* Discover all descriptors within the service range */
        rc = ble_gattc_disc_all_dscs(conn_handle,
                                     ctx->disc_svc_start_handle,
                                     ctx->disc_svc_end_handle,
                                     disc_dsc_cb, NULL);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to start dsc discovery: %d", rc);
        }
        return rc;

    case DISC_STATE_OACP_CCCD: {
        if (ctx->handles.oacp_cccd_handle == 0) {
            ESP_LOGE(TAG, "OACP CCCD not found, discovery failed");
            disc_complete(conn_handle, BLE_HS_ENOENT);
            return 0;
        }
        /* Write CCCD to enable indications (0x0002) */
        uint8_t val[2] = { 0x02, 0x00 };
        rc = ble_gattc_write_flat(conn_handle,
                                  ctx->handles.oacp_cccd_handle,
                                  val, sizeof(val),
                                  write_cccd_cb, NULL);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to write OACP CCCD: %d", rc);
        }
        return rc;
    }

    case DISC_STATE_OLCP_CCCD: {
        if (ctx->handles.olcp_cccd_handle == 0) {
            if (ctx->handles.olcp_handle != 0) {
                /* OLCP characteristic is present but its CCCD is missing — protocol error */
                ESP_LOGE(TAG, "OLCP CCCD not found but OLCP characteristic is present");
                disc_complete(conn_handle, BLE_HS_ENOENT);
                return 0;
            }
            /* OLCP not present — skip to done */
            ctx->disc_state = DISC_STATE_DONE;
            disc_complete(conn_handle, 0);
            return 0;
        }
        /* Write CCCD to enable indications (0x0002) */
        uint8_t val[2] = { 0x02, 0x00 };
        rc = ble_gattc_write_flat(conn_handle,
                                  ctx->handles.olcp_cccd_handle,
                                  val, sizeof(val),
                                  write_cccd_cb, NULL);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to write OLCP CCCD: %d", rc);
        }
        return rc;
    }

    case DISC_STATE_DONE:
        disc_complete(conn_handle, 0);
        return 0;

    default:
        ESP_LOGE(TAG, "Unexpected disc_state=%d", ctx->disc_state);
        return BLE_HS_EUNKNOWN;
    }
}

static void disc_complete(uint16_t conn_handle, int status)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    if (!ctx) {
        return;
    }

    ctx->disc_state = DISC_STATE_IDLE;

    ble_ots_client_discover_complete_t evt = {
        .status = status,
        .multi_object_server = ctx->multi_object_server,
    };
    memcpy(&evt.handles, &ctx->handles, sizeof(evt.handles));

    if (status != 0) {
        ESP_LOGE(TAG, "Discovery failed: status=%d, conn_handle=%d", status, conn_handle);
        /* Release the context BEFORE dispatching the failure event. The event
         * callback runs synchronously and may re-enter the OTS client API to
         * retry discovery (ble_ots_client_discover_service), which allocates a
         * fresh context for this conn_handle. Cleaning up first ensures the
         * deferred remove below (if any) cannot destroy that re-allocated
         * context. evt already holds a full local copy of the data the
         * callback needs, so dispatching after cleanup is safe. */
        ble_ots_client_remove_conn_ctx(conn_handle);
        ble_ots_client_dispatch_event(conn_handle, BLE_OTS_CLIENT_EVT_DISCOVER_COMPLETE, &evt);
    } else {
        ESP_LOGI(TAG, "Discovery complete: conn_handle=%d multi_obj=%d",
                 conn_handle, ctx->multi_object_server);
        ble_ots_client_dispatch_event(conn_handle, BLE_OTS_CLIENT_EVT_DISCOVER_COMPLETE, &evt);
    }
}

/*****************************************************************************
 * Feature Read
 *****************************************************************************/

static int feature_read_cb(uint16_t conn_handle, const struct ble_gatt_error *error,
                           struct ble_gatt_attr *attr, void *arg)
{
    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
    ble_ots_client_feature_read_t evt;
    memset(&evt, 0, sizeof(evt));

    if (error->status != 0) {
        ESP_LOGE(TAG, "Feature read failed: status=%d", error->status);
        evt.status = error->status;
        ble_ots_client_dispatch_event(conn_handle, BLE_OTS_CLIENT_EVT_FEATURE_READ, &evt);
        return 0;
    }

    /* Parse 8 bytes: OACP Features (4) + OLCP Features (4), little-endian */
    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
    if (data_len < 8) {
        ESP_LOGE(TAG, "Feature value too short: %d bytes", data_len);
        evt.status = BLE_HS_EBADDATA;
        ble_ots_client_dispatch_event(conn_handle, BLE_OTS_CLIENT_EVT_FEATURE_READ, &evt);
        return 0;
    }

    uint8_t buf[8];
    int rc = os_mbuf_copydata(attr->om, 0, 8, buf);
    if (rc != 0) {
        evt.status = BLE_HS_EUNKNOWN;
        ble_ots_client_dispatch_event(conn_handle, BLE_OTS_CLIENT_EVT_FEATURE_READ, &evt);
        return 0;
    }

    evt.feature.oacp_features = (uint32_t)buf[0] |
                                ((uint32_t)buf[1] << 8) |
                                ((uint32_t)buf[2] << 16) |
                                ((uint32_t)buf[3] << 24);
    evt.feature.olcp_features = (uint32_t)buf[4] |
                                ((uint32_t)buf[5] << 8) |
                                ((uint32_t)buf[6] << 16) |
                                ((uint32_t)buf[7] << 24);
    evt.status = 0;

    /* Cache in context */
    if (ctx) {
        ctx->feature = evt.feature;
        ctx->feature_valid = true;
    }

    ESP_LOGI(TAG, "Feature read: OACP=0x%08lx OLCP=0x%08lx",
             (unsigned long)evt.feature.oacp_features,
             (unsigned long)evt.feature.olcp_features);

    ble_ots_client_dispatch_event(conn_handle, BLE_OTS_CLIENT_EVT_FEATURE_READ, &evt);
    return 0;
}

int ble_ots_client_read_feature(uint16_t conn_id)
{
    if (!g_ots_client || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (!ctx) {
        ESP_LOGE(TAG, "No context for conn_handle=%d", conn_id);
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.ots_feature_handle == 0) {
        ESP_LOGE(TAG, "OTS Feature handle not discovered");
        return BLE_HS_ENOENT;
    }

    int rc = ble_gattc_read(conn_id, ctx->handles.ots_feature_handle,
                            feature_read_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to read OTS Feature: %d", rc);
    }
    return rc;
}

/*****************************************************************************
 * GAP Event Handler
 *****************************************************************************/

static int ots_client_gap_event_handler(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_DISCONNECT: {
        uint16_t conn_handle = event->disconnect.conn.conn_handle;
        ESP_LOGI(TAG, "Disconnect event: conn_handle=%d reason=%d",
                 conn_handle, event->disconnect.reason);
        ble_ots_client_remove_conn_ctx(conn_handle);
        return 0;
    }

    case BLE_GAP_EVENT_NOTIFY_RX: {
        uint16_t conn_handle = event->notify_rx.conn_handle;
        uint16_t attr_handle = event->notify_rx.attr_handle;

        ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_handle);
        if (!ctx) {
            return 0;
        }

        /* Route OACP indications to transfer module */
        if (attr_handle == ctx->handles.oacp_handle) {
            ble_ots_client_handle_oacp_indication(conn_handle, attr_handle,
                                                  event->notify_rx.om);
            return 0;
        }

        /* Route OLCP and Object Changed indications to object_nav module */
        if ((ctx->handles.olcp_handle != 0 && attr_handle == ctx->handles.olcp_handle) ||
            (ctx->handles.object_changed_handle != 0 &&
             attr_handle == ctx->handles.object_changed_handle)) {
            return ble_ots_client_object_nav_gap_event(event, arg);
        }

        return 0;
    }

    default:
        return 0;
    }
}
