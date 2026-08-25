/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>

#include "esp_log.h"

#include "nimble/nimble_port.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_l2cap.h"
#include "host/ble_uuid.h"
#include "os/os_mbuf.h"

#include "ble_ots_common.h"
#include "ble_ots_server.h"
#include "ble_ots_server_int.h"

static const char *TAG = "ots_server_init";

/*****************************************************************************
 * Global Control Block
 *
 * Heap-allocated in ble_ots_server_init(), released in ble_ots_server_deinit().
 * NULL while the server is not initialized — see BLE_OTS_SERVER_ENV_OK().
 *****************************************************************************/
ble_ots_server_cb_env_t *p_ble_ots_server_env;

/*****************************************************************************
 * Mutex helpers — protect object DB and connection pool from concurrent access
 *
 * The object database, the object content buffers and the connection pool are
 * reached from two contexts: the NimBLE host task (GATT/GAP/L2CAP callbacks and
 * callouts) and the application task (the public ble_ots_server_* APIs). Every
 * accessor of that state must hold this mutex.
 *
 * The mutex is recursive because the public APIs and the host-task callbacks
 * take it at their entry point and then call the internal helpers, which lock
 * again. Lock ordering is always OTS mutex -> NimBLE host lock; NimBLE never
 * invokes our callbacks with ble_hs_lock() held, so no inversion is possible.
 *
 * The handle deliberately lives outside the control block and is created only
 * once for the lifetime of the module: it has to stay valid while the block
 * itself is being torn down in ble_ots_server_deinit(), and destroying it there
 * could free a mutex that a host-task callback is still blocked on.
 *****************************************************************************/
static SemaphoreHandle_t s_ots_server_mutex;

#define OTS_LOCK()   do { if (s_ots_server_mutex) xSemaphoreTakeRecursive(s_ots_server_mutex, portMAX_DELAY); } while (0)
#define OTS_UNLOCK() do { if (s_ots_server_mutex) xSemaphoreGiveRecursive(s_ots_server_mutex); } while (0)

void ble_ots_server_lock(void)
{
    OTS_LOCK();
}

void ble_ots_server_unlock(void)
{
    OTS_UNLOCK();
}

/*****************************************************************************
 * L2CAP OTC MTU
 *****************************************************************************/
#define OTS_L2CAP_COC_MTU   256

/*****************************************************************************
 * Directory Listing Object name
 *****************************************************************************/
#define BLE_OTS_DLO_NAME    "Directory Listing"

/*****************************************************************************
 * Forward Declarations — GATT access callbacks
 *****************************************************************************/
static int ots_gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg);

/*****************************************************************************
 * Forward Declarations — L2CAP event callback
 *****************************************************************************/
static int ots_l2cap_event_cb(struct ble_l2cap_event *event, void *arg);

/*****************************************************************************
 * Forward Declarations — GAP event listener
 *****************************************************************************/
static int ots_gap_event_cb(struct ble_gap_event *event, void *arg);

/*****************************************************************************
 * Forward Declarations — Transfer timer callback
 *****************************************************************************/
static void ots_transfer_timer_cb(struct ble_npl_event *ev);

/*****************************************************************************
 * Forward Declarations — Deferred indication queue
 *****************************************************************************/
static void ots_indicate_queue_flush(ble_ots_server_conn_state_t *cs);

/*****************************************************************************
 * Forward Declarations — External handlers (from other .c files)
 *****************************************************************************/

/* Metadata handlers — ots_server_metadata.c */
extern int ble_ots_server_metadata_read(uint16_t conn_handle, uint16_t attr_handle,
                                         struct ble_gatt_access_ctxt *ctxt);
extern int ble_ots_server_metadata_write(uint16_t conn_handle, uint16_t attr_handle,
                                          struct ble_gatt_access_ctxt *ctxt);

/* OACP handler — ots_server_oacp_ops.c */
extern int ble_ots_server_oacp_write(uint16_t conn_handle,
                                      struct ble_gatt_access_ctxt *ctxt);

/* OLCP handler — ots_server_olcp.c */
extern int ble_ots_server_olcp_write(uint16_t conn_handle,
                                      struct ble_gatt_access_ctxt *ctxt);

/* Transfer L2CAP data handler — ots_server_oacp_transfer.c */
extern int ble_ots_server_oacp_l2cap_recv(uint16_t conn_handle,
                                           struct ble_l2cap_chan *chan,
                                           struct os_mbuf *sdu_rx);
extern void ble_ots_server_oacp_transfer_timeout(uint16_t conn_id);
extern void ble_ots_server_otc_disconnected_cb(uint16_t conn_handle,
                                               struct ble_l2cap_chan *chan);
extern void ble_ots_server_otc_tx_unstalled_cb(uint16_t conn_handle,
                                                struct ble_l2cap_chan *chan);

/*****************************************************************************
 * GATT Service Definition — UUIDs
 *****************************************************************************/
static const ble_uuid16_t ots_svc_uuid        = BLE_UUID16_INIT(BLE_OTS_UUID_OTS_SERVICE);
static const ble_uuid16_t ots_feature_uuid     = BLE_UUID16_INIT(BLE_OTS_UUID_OTS_FEATURE);
static const ble_uuid16_t obj_name_uuid        = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_NAME);
static const ble_uuid16_t obj_type_uuid        = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_TYPE);
static const ble_uuid16_t obj_size_uuid        = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_SIZE);
static const ble_uuid16_t obj_first_created_uuid = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_FIRST_CREATED);
static const ble_uuid16_t obj_last_modified_uuid = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_LAST_MODIFIED);
static const ble_uuid16_t obj_id_uuid          = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_ID);
static const ble_uuid16_t obj_properties_uuid  = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_PROPERTIES);
static const ble_uuid16_t oacp_uuid            = BLE_UUID16_INIT(BLE_OTS_UUID_OACP);
static const ble_uuid16_t olcp_uuid            = BLE_UUID16_INIT(BLE_OTS_UUID_OLCP);
static const ble_uuid16_t obj_list_filter_uuid = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_LIST_FILTER);
static const ble_uuid16_t obj_changed_uuid     = BLE_UUID16_INIT(BLE_OTS_UUID_OBJECT_CHANGED);

/*****************************************************************************
 * Dynamic GATT service table
 *
 * We build the characteristic array dynamically based on config, but use
 * a statically-sized array large enough for the worst case:
 *   12 characteristics + 1 terminator = 13 entries max
 *   (OTS Feature, Object Name, Object Type, Object Size,
 *    Object First-Created, Object Last-Modified, Object ID,
 *    Object Properties, OACP, OLCP, Object List Filter x3,
 *    Object Changed)
 *   Actually max = 15 (3 filter instances counted separately) + 1 terminator = 16
 *****************************************************************************/
#define OTS_MAX_CHARS   16

/*****************************************************************************
 * Module-lifetime init context (heap-allocated)
 *
 * Aggregates the GATT service/characteristic definition tables, the value
 * handles we track, and the GAP event listener into a single heap block held
 * by s_ots_server_init.
 *
 * Lifetime: allocated once and then kept for the lifetime of the module. It is
 * NOT released by ble_ots_server_deinit() and must not be released once
 * ble_gatts_add_svcs() has accepted it: NimBLE stores the caller's svc_defs
 * pointer without copying, and every registered attribute keeps a pointer into
 * chr_defs[] as its access callback argument. NimBLE exposes no API to
 * unregister a service, so freeing this block would leave the host with
 * dangling pointers. Only the init error paths that run *before*
 * ble_gatts_add_svcs() succeeds may free it — see s_ots_gatt_registered.
 *****************************************************************************/
typedef struct {
    /* GAP event listener registered for connect/disconnect/subscribe */
    struct ble_gap_event_listener gap_listener;

    /* Dynamic GATT characteristic table (worst case + terminator) */
    struct ble_gatt_chr_def chr_defs[OTS_MAX_CHARS];

    /* Service definition table (primary service + terminator) */
    struct ble_gatt_svc_def svc_defs[2];

    /* Value handles for characteristics we need to track */
    uint16_t ots_feature_val_handle;
    uint16_t obj_name_val_handle;
    uint16_t obj_type_val_handle;
    uint16_t obj_size_val_handle;
    uint16_t obj_first_created_val_handle;
    uint16_t obj_last_modified_val_handle;
    uint16_t obj_id_val_handle;
    uint16_t obj_properties_val_handle;
    uint16_t oacp_val_handle;
    uint16_t olcp_val_handle;
    uint16_t obj_changed_val_handle;
    uint16_t filter_val_handles[BLE_OTS_FILTER_INSTANCE_COUNT];
} ots_server_init_ctx_t;

static ots_server_init_ctx_t *s_ots_server_init;

/* Set once ble_gatts_add_svcs() has taken ownership of s_ots_server_init->
 * svc_defs. From that point the block is owned by the NimBLE host: it must
 * never be freed nor rebuilt, and the service must never be added a second
 * time. */
static bool s_ots_gatt_registered;

uint16_t ble_ots_server_oacp_handle_get(void)
{
    return s_ots_server_init ? s_ots_server_init->oacp_val_handle : 0;
}

uint16_t ble_ots_server_olcp_handle_get(void)
{
    return s_ots_server_init ? s_ots_server_init->olcp_val_handle : 0;
}

/*****************************************************************************
 * Helper: conn_state_get — scan concurrency pool by conn_id
 *****************************************************************************/
ble_ots_server_conn_state_t *ble_ots_server_conn_state_get(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *found = NULL;

    OTS_LOCK();
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        if (ble_ots_server_env.conn_pool[i].in_pool &&
            ble_ots_server_env.conn_pool[i].conn_id == conn_id) {
            found = &ble_ots_server_env.conn_pool[i];
            break;
        }
    }
    OTS_UNLOCK();
    return found;
}

/*****************************************************************************
 * Helper: find a free slot in the concurrency pool
 *****************************************************************************/
static ble_ots_server_conn_state_t *conn_pool_alloc(uint16_t conn_id)
{
    OTS_LOCK();
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        if (!ble_ots_server_env.conn_pool[i].in_pool) {
            ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[i];
            memset(cs, 0, sizeof(*cs));
            cs->in_pool = true;
            cs->conn_id = conn_id;
            cs->current_object_id = BLE_OTS_OBJ_ID_INVALID;
            cs->current_obj_valid = false;
            cs->otc_chan = NULL;
            cs->transfer_active = false;
            cs->created_obj_count = 0;
            cs->sort_order = BLE_OTS_SORT_ORDER_NAME_ASC;
            /* Reset filters to No Filter */
            for (int f = 0; f < BLE_OTS_FILTER_INSTANCE_COUNT; f++) {
                cs->filter[f].filter_type = BLE_OTS_FILTER_NO_FILTER;
            }

            /* If single-object server, auto-select the only object */
            if (CONFIG_BLE_OTS_SERVER_MAX_OBJECTS == 1) {
                for (int j = 0; j < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; j++) {
                    if (ble_ots_server_env.obj_db[j].in_use) {
                        cs->current_object_id = ble_ots_server_env.obj_db[j].object_id;
                        cs->current_obj_valid = true;
                        break;
                    }
                }
            }

            OTS_UNLOCK();
            return cs;
        }
    }
    OTS_UNLOCK();
    return NULL;
}

/*****************************************************************************
 * Helper: release a concurrency pool slot
 *****************************************************************************/
static void conn_pool_free(ble_ots_server_conn_state_t *cs)
{
    if (cs) {
        OTS_LOCK();
        /* Stop and deinit transfer timer if initialized */
        if (cs->transfer_timer_inited) {
            ble_npl_callout_stop(&cs->transfer_timer);
            ble_npl_callout_deinit(&cs->transfer_timer);
            cs->transfer_timer_inited = false;
        }
        /* Same for the read-transfer back-off callout */
        if (cs->transfer_retry_timer_inited) {
            ble_npl_callout_stop(&cs->transfer_retry_timer);
            ble_npl_callout_deinit(&cs->transfer_retry_timer);
            cs->transfer_retry_timer_inited = false;
        }
        /* Remove any pending deferred read-data send event. ble_npl_event_init()
         * allocates the port-private event object, so it has to be released
         * here: the slot is memset() on the next conn_pool_alloc(), which would
         * otherwise drop the only pointer to it. */
        if (cs->transfer_data_ev_inited) {
            ble_npl_eventq_remove(nimble_port_get_dflt_eventq(), &cs->transfer_data_ev);
            ble_npl_event_deinit(&cs->transfer_data_ev);
            cs->transfer_data_ev_inited = false;
        }
        /* Drop indications that will never be sent on this connection */
        ots_indicate_queue_flush(cs);
        cs->in_pool = false;
        cs->conn_id = BLE_OTS_CONN_ID_NONE;
        OTS_UNLOCK();
    }
}

/*****************************************************************************
 * Concurrency Check
 *
 * A connection normally takes its pool slot on BLE_GAP_EVENT_CONNECT. If the
 * pool was full at that moment, admit it lazily here instead of excluding it
 * for the rest of the connection: by the time the peer actually uses OTS a slot
 * may well have been released by another client.
 *****************************************************************************/
bool ble_ots_server_concurrency_check(uint16_t conn_id)
{
    bool admitted;

    OTS_LOCK();
    admitted = (ble_ots_server_conn_state_get(conn_id) != NULL) ||
               (conn_pool_alloc(conn_id) != NULL);
    OTS_UNLOCK();

    return admitted;
}

/*****************************************************************************
 * Object Database — Create
 *
 * Caller must hold the OTS mutex: the free-slot scan and the Object ID search
 * below must not be interleaved with another creation, or both would claim the
 * same slot and the loser's data buffer would be leaked.
 *****************************************************************************/
static ble_ots_obj_id_t obj_db_create_locked(const ble_ots_obj_type_entry_t *type,
                                             uint32_t size)
{
    /* Find a free slot */
    int slot = -1;
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        if (!ble_ots_server_env.obj_db[i].in_use) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        ESP_LOGE(TAG, "Object database full");
        return 0;
    }

    /* Allocate Object ID — always validate uniqueness to prevent collisions */
    ble_ots_obj_id_t new_id = ble_ots_server_env.next_obj_id;
    if (new_id > BLE_OTS_OBJ_ID_MAX_USER) {
        new_id = BLE_OTS_OBJ_ID_MIN_USER;
    }

    {
        /* Search for a free (non-colliding) Object ID starting from new_id */
        bool found = false;
        ble_ots_obj_id_t candidate = new_id;
        ble_ots_obj_id_t start = candidate;
        bool wrapped = false;

        while (!wrapped || candidate < start) {
            bool in_use = false;
            for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
                if (ble_ots_server_env.obj_db[i].in_use &&
                    ble_ots_server_env.obj_db[i].object_id == candidate) {
                    in_use = true;
                    break;
                }
            }
            if (!in_use) {
                new_id = candidate;
                found = true;
                break;
            }
            candidate++;
            if (candidate > BLE_OTS_OBJ_ID_MAX_USER) {
                candidate = BLE_OTS_OBJ_ID_MIN_USER;
                wrapped = true;
            }
        }
        if (!found) {
            ESP_LOGE(TAG, "No free Object IDs");
            return 0;
        }
    }

    ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[slot];
    memset(obj, 0, sizeof(*obj));
    obj->in_use = true;
    obj->object_id = new_id;
    obj->name_len = 0;
    obj->type_uuid_len = type->uuid_len;
    memcpy(obj->type_uuid, type->uuid, type->uuid_len);
    obj->current_size = 0;
    obj->allocated_size = size;
    obj->properties = BLE_OTS_OBJ_PROP_READ | BLE_OTS_OBJ_PROP_WRITE; /* Default: read-write */
    obj->locked_by = BLE_OTS_CONN_ID_NONE;
    obj->marked = 0;
    memset(&obj->first_created, 0, sizeof(ble_ots_date_time_t));
    memset(&obj->last_modified, 0, sizeof(ble_ots_date_time_t));

    /* Allocate data buffer if size > 0 */
    if (size > 0) {
        obj->data = calloc(1, size);
        if (!obj->data) {
            ESP_LOGE(TAG, "Failed to allocate object data buffer (%lu bytes)", (unsigned long)size);
            obj->in_use = false;
            return 0;
        }
    } else {
        obj->data = NULL;
    }

    /* Advance next_obj_id */
    ble_ots_server_env.next_obj_id = new_id + 1;
    if (ble_ots_server_env.next_obj_id < BLE_OTS_OBJ_ID_MIN_USER) {
        ble_ots_server_env.next_obj_id = BLE_OTS_OBJ_ID_MIN_USER;
    }

    ESP_LOGI(TAG, "Object created: ID=0x%012llX slot=%d alloc_size=%lu",
             (unsigned long long)new_id, slot, (unsigned long)size);
    return new_id;
}

ble_ots_obj_id_t ble_ots_server_obj_db_create(const ble_ots_obj_type_entry_t *type,
                                                uint32_t size)
{
    ble_ots_obj_id_t new_id;

    if (!type) {
        return 0;
    }

    /* Only 16-bit and 128-bit UUIDs exist. Enforcing it here keeps the copy
     * into the fixed 16-octet type_uuid field in bounds, and lets every reader
     * of type_uuid_len (Object Type reads, DLO records) trust the field. */
    if (type->uuid_len != 2 && type->uuid_len != 16) {
        ESP_LOGE(TAG, "Invalid object type UUID length: %u", type->uuid_len);
        return 0;
    }

    OTS_LOCK();
    new_id = obj_db_create_locked(type, size);
    OTS_UNLOCK();

    return new_id;
}

/*****************************************************************************
 * Object Database — Delete
 *****************************************************************************/
int ble_ots_server_obj_db_delete(ble_ots_obj_id_t object_id)
{
    uint16_t abort_conn_ids[CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY];
    int abort_count = 0;
    bool found = false;

    /* DLO cannot be deleted */
    if (object_id == BLE_OTS_OBJ_ID_DIRECTORY_LISTING) {
        ESP_LOGE(TAG, "Cannot delete Directory Listing Object");
        return BLE_HS_EINVAL;
    }

    OTS_LOCK();
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[i];
        if (!obj->in_use || obj->object_id != object_id) {
            continue;
        }

        found = true;
        free(obj->data);
        obj->data = NULL;
        obj->in_use = false;

        /* Invalidate the Current Object of every connection pointing at this
         * object, and note the connections whose active transfer has to be
         * aborted. The abort itself is deferred until the mutex is released:
         * it calls into NimBLE and into the application callback, neither of
         * which may run under the mutex. */
        for (int c = 0; c < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; c++) {
            ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[c];
            if (!cs->in_pool) {
                continue;
            }
            if (cs->transfer_active && cs->transfer_object_id == object_id) {
                abort_conn_ids[abort_count++] = cs->conn_id;
            }
            if (cs->current_object_id == object_id) {
                cs->current_object_id = BLE_OTS_OBJ_ID_INVALID;
                cs->current_obj_valid = false;
            }
        }
        break;
    }
    OTS_UNLOCK();

    if (!found) {
        ESP_LOGE(TAG, "Object not found for delete: ID=0x%012llX",
                 (unsigned long long)object_id);
        return BLE_HS_ENOENT;
    }

    for (int i = 0; i < abort_count; i++) {
        ESP_LOGW(TAG, "Aborting active transfer on deleted object "
                 "ID=0x%012llX conn=%d",
                 (unsigned long long)object_id, abort_conn_ids[i]);
        ble_ots_server_transfer_timer_stop(abort_conn_ids[i]);
        ble_ots_server_otc_close(abort_conn_ids[i]);
        ble_ots_server_oacp_transfer_timeout(abort_conn_ids[i]);
    }

    ESP_LOGI(TAG, "Object deleted: ID=0x%012llX", (unsigned long long)object_id);
    return 0;
}

/*****************************************************************************
 * Object Database — Lookup
 *****************************************************************************/
ble_ots_server_obj_t *ble_ots_server_obj_db_lookup(ble_ots_obj_id_t object_id)
{
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        if (ble_ots_server_env.obj_db[i].in_use &&
            ble_ots_server_env.obj_db[i].object_id == object_id) {
            return &ble_ots_server_env.obj_db[i];
        }
    }
    return NULL;
}

/*****************************************************************************
 * Object Data — Read
 *****************************************************************************/
int ble_ots_server_obj_data_read(ble_ots_obj_id_t object_id, uint32_t offset,
                                  uint32_t length, uint8_t *buf)
{
    int rc = 0;

    if (!buf) {
        return BLE_HS_EINVAL;
    }

    /* The lock must be held across the memcpy, not just around the lookup:
     * obj->data can be realloc'd by obj_data_write or freed by obj_db_delete
     * from the other task while the copy is in progress. */
    OTS_LOCK();

    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (!obj) {
        rc = BLE_HS_ENOENT;
    } else if (offset + length > obj->current_size) {
        rc = BLE_HS_EINVAL;
    } else if (obj->data) {
        memcpy(buf, obj->data + offset, length);
    }

    OTS_UNLOCK();
    return rc;
}

/*****************************************************************************
 * Object Data — Write
 *****************************************************************************/
int ble_ots_server_obj_data_write(ble_ots_obj_id_t object_id, uint32_t offset,
                                   const uint8_t *data, uint32_t length)
{
    /* DLO is read-only and managed internally; reject external writes */
    if (object_id == BLE_OTS_OBJ_ID_DIRECTORY_LISTING) {
        ESP_LOGE(TAG, "Cannot write to Directory Listing Object");
        return BLE_HS_EINVAL;
    }

    OTS_LOCK();

    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (!obj) {
        OTS_UNLOCK();
        return BLE_HS_ENOENT;
    }
    if (!data || length == 0) {
        OTS_UNLOCK();
        return BLE_HS_EINVAL;
    }

    uint32_t end = offset + length;

    /* Expand allocated size if needed */
    if (end > obj->allocated_size) {
        uint8_t *new_data = realloc(obj->data, end);
        if (!new_data) {
            ESP_LOGE(TAG, "Failed to expand object data buffer");
            OTS_UNLOCK();
            return BLE_HS_ENOMEM;
        }
        /* Zero-fill the gap between old allocated_size and offset if any */
        if (obj->allocated_size < offset) {
            memset(new_data + obj->allocated_size, 0, offset - obj->allocated_size);
        }
        obj->data = new_data;
        obj->allocated_size = end;
    }

    memcpy(obj->data + offset, data, length);

    /* Update current size */
    if (end > obj->current_size) {
        obj->current_size = end;
    }

    OTS_UNLOCK();
    return 0;
}

/*****************************************************************************
 * Current Object — Get / Set
 *****************************************************************************/
ble_ots_obj_id_t ble_ots_server_current_obj_get(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return BLE_OTS_OBJ_ID_INVALID;
    }

    /* In single-object mode, dynamically auto-select the only object if it
     * exists, even if it was created after the connection was established
     * (OLCP is not available in single-object mode). */
    if (CONFIG_BLE_OTS_SERVER_MAX_OBJECTS == 1) {
        for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
            if (ble_ots_server_env.obj_db[i].in_use) {
                cs->current_object_id = ble_ots_server_env.obj_db[i].object_id;
                cs->current_obj_valid = true;
                return cs->current_object_id;
            }
        }
        /* No object exists */
        cs->current_obj_valid = false;
        cs->current_object_id = BLE_OTS_OBJ_ID_INVALID;
        return BLE_OTS_OBJ_ID_INVALID;
    }

    if (!cs->current_obj_valid) {
        return BLE_OTS_OBJ_ID_INVALID;
    }
    return cs->current_object_id;
}

int ble_ots_server_current_obj_set(uint16_t conn_id, ble_ots_obj_id_t object_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return BLE_HS_ENOENT;
    }

    if (object_id == BLE_OTS_OBJ_ID_INVALID) {
        cs->current_object_id = BLE_OTS_OBJ_ID_INVALID;
        cs->current_obj_valid = false;
    } else {
        cs->current_object_id = object_id;
        cs->current_obj_valid = true;
    }
    return 0;
}

/*****************************************************************************
 * Object Locking
 *****************************************************************************/
int ble_ots_server_obj_lock(ble_ots_obj_id_t object_id, uint16_t conn_id)
{
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (!obj) {
        return BLE_HS_ENOENT;
    }
    if (obj->locked_by != BLE_OTS_CONN_ID_NONE && obj->locked_by != conn_id) {
        return BLE_HS_EBUSY;
    }
    obj->locked_by = conn_id;
    return 0;
}

int ble_ots_server_obj_unlock(ble_ots_obj_id_t object_id)
{
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (!obj) {
        return BLE_HS_ENOENT;
    }
    if (obj->locked_by == BLE_OTS_CONN_ID_NONE) {
        return BLE_HS_EALREADY;
    }
    obj->locked_by = BLE_OTS_CONN_ID_NONE;
    return 0;
}

/*****************************************************************************
 * OTC Channel — Get / Close
 *****************************************************************************/
struct ble_l2cap_chan *ble_ots_server_otc_get(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return NULL;
    }
    return cs->otc_chan;
}

int ble_ots_server_otc_close(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs || !cs->otc_chan) {
        return BLE_HS_ENOENT;
    }
    int rc = ble_l2cap_disconnect(cs->otc_chan);
    if (rc == 0) {
        cs->otc_chan = NULL;
    }
    return rc;
}

/*****************************************************************************
 * Deferred Indication via NimBLE Event Queue
 *
 * Indications must not be sent synchronously from within a GATT access
 * callback (the ATT Write Response has not yet been transmitted). The
 * indication mbuf is built up-front (the caller's data buffer may live on the
 * stack) and the actual ble_gatts_indicate_custom() call is deferred to a
 * NimBLE host-task event.
 *
 * ATT permits only one outstanding indication per connection, so the requests
 * are additionally queued per connection and released one at a time: the next
 * one is posted only once BLE_GAP_EVENT_NOTIFY_TX reports the previous
 * indication as completed (confirmed or timed out). Without this, an OACP/OLCP
 * response and an Object Changed indication raised close together would both be
 * handed to NimBLE inside the same confirmation window.
 *****************************************************************************/
struct ots_indicate_ctx {
    struct ble_npl_event ev;
    struct ots_indicate_ctx *next;
    uint16_t conn_id;
    uint16_t val_handle;
    struct os_mbuf *om;
    bool posted;                /* Handed to the host task: it owns the block */
    bool cancelled;             /* Connection went away — drop instead of send */
};

static void ots_indicate_event_cb(struct ble_npl_event *ev);

/**
 * @brief Release an indication context and its payload.
 */
static void ots_indicate_ctx_free(struct ots_indicate_ctx *ctx)
{
    ble_npl_event_deinit(&ctx->ev);
    os_mbuf_free_chain(ctx->om);
    free(ctx);
}

/**
 * @brief Post the head of the connection's indication queue to the host task.
 *
 * Caller must hold the OTS mutex. Does nothing when the queue is empty or an
 * indication is still awaiting its confirmation.
 */
static void ots_indicate_queue_kick(ble_ots_server_conn_state_t *cs)
{
    if (cs->indicate_in_flight || !cs->indicate_head) {
        return;
    }

    cs->indicate_in_flight = true;
    cs->indicate_head->posted = true;
    ble_npl_eventq_put(nimble_port_get_dflt_eventq(), &cs->indicate_head->ev);
}

/**
 * @brief Drop every queued indication for a connection.
 *
 * Caller must hold the OTS mutex. Used when the connection goes away, so the
 * queued contexts are never delivered.
 */
static void ots_indicate_queue_flush(ble_ots_server_conn_state_t *cs)
{
    struct ots_indicate_ctx *ctx = cs->indicate_head;

    cs->indicate_head = NULL;
    cs->indicate_tail = NULL;
    cs->indicate_in_flight = false;

    while (ctx) {
        struct ots_indicate_ctx *next = ctx->next;
        ctx->next = NULL;
        ctx->cancelled = true;
        if (!ctx->posted) {
            ots_indicate_ctx_free(ctx);
        }
        ctx = next;
    }
}

/**
 * @brief Complete the in-flight indication and start the next queued one.
 *
 * Called from the GAP NOTIFY_TX handler once the outstanding indication has
 * been confirmed by the peer, has timed out, or failed to go out at all.
 */
static void ots_indicate_completed(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs;

    OTS_LOCK();
    cs = ble_ots_server_conn_state_get(conn_id);
    if (cs) {
        cs->indicate_in_flight = false;
        ots_indicate_queue_kick(cs);
    }
    OTS_UNLOCK();
}

static void ots_indicate_event_cb(struct ble_npl_event *ev)
{
    struct ots_indicate_ctx *ctx = (struct ots_indicate_ctx *)ble_npl_event_get_arg(ev);
    struct os_mbuf *om;
    uint16_t conn_id;
    uint16_t val_handle;

    if (!ctx) {
        return;
    }

    OTS_LOCK();

    if (ctx->cancelled) {
        /* The connection was released while this event was in the host queue;
         * ots_indicate_queue_flush() left the block for us to reclaim. */
        OTS_UNLOCK();
        ots_indicate_ctx_free(ctx);
        return;
    }

    /* Unlink before sending: ble_gatts_indicate_custom() reports the outcome
     * synchronously through BLE_GAP_EVENT_NOTIFY_TX, and that handler must not
     * find this context still queued. */
    ble_ots_server_conn_state_t *cs = BLE_OTS_SERVER_ENV_OK() ?
                                      ble_ots_server_conn_state_get(ctx->conn_id) : NULL;
    if (cs && cs->indicate_head == ctx) {
        cs->indicate_head = ctx->next;
        if (!cs->indicate_head) {
            cs->indicate_tail = NULL;
        }
    }
    conn_id = ctx->conn_id;
    val_handle = ctx->val_handle;
    om = ctx->om;
    /* Ownership of the mbuf passes to ble_gatts_indicate_custom() below. */
    ctx->om = NULL;
    ots_indicate_ctx_free(ctx);
    OTS_UNLOCK();

    /* ble_gatts_indicate_custom() consumes the mbuf on all return paths, and
     * always raises BLE_GAP_EVENT_NOTIFY_TX — the queue is advanced from there,
     * both for a failed send and for the eventual confirmation. */
    int rc = ble_gatts_indicate_custom(conn_id, val_handle, om);
    if (rc != 0) {
        ESP_LOGE(TAG, "indicate_event: ble_gatts_indicate_custom failed rc=%d", rc);
    }
}

/*****************************************************************************
 * Indicate Response
 *****************************************************************************/

/**
 * @brief Resolve the value handle to indicate on, checking CCCD subscription.
 *
 * @param conn_id        BLE connection identifier
 * @param char_uuid      Characteristic UUID (OACP/OLCP/Object Changed)
 * @param out_val_handle Receives the characteristic value handle on success
 * @return 0 on success, non-zero (BLE_HS_* or ATT error) otherwise
 */
static int ots_indicate_val_handle_get(uint16_t conn_id, uint16_t char_uuid,
                                       uint16_t *out_val_handle)
{
    uint16_t val_handle;
    bool subscribed;

    OTS_LOCK();

    /* Verify CCCD subscription before sending indication */
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        OTS_UNLOCK();
        return BLE_HS_ENOENT;
    }

    switch (char_uuid) {
    case BLE_OTS_UUID_OACP:
        val_handle = s_ots_server_init->oacp_val_handle;
        subscribed = cs->oacp_subscribed;
        break;
    case BLE_OTS_UUID_OLCP:
        val_handle = s_ots_server_init->olcp_val_handle;
        subscribed = cs->olcp_indicate_subscribed;
        break;
    case BLE_OTS_UUID_OBJECT_CHANGED:
        val_handle = s_ots_server_init->obj_changed_val_handle;
        subscribed = cs->obj_changed_subscribed;
        break;
    default:
        OTS_UNLOCK();
        ESP_LOGE(TAG, "indicate_response: unknown char UUID 0x%04X", char_uuid);
        return BLE_HS_EINVAL;
    }

    OTS_UNLOCK();

    if (!subscribed) {
        ESP_LOGW(TAG, "indicate_response: CCCD 0x%04X not subscribed for conn=%d",
                 char_uuid, conn_id);
        return ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
    }
    if (val_handle == 0) {
        ESP_LOGE(TAG, "indicate_response: characteristic not registered");
        return BLE_HS_ENOENT;
    }

    *out_val_handle = val_handle;
    return 0;
}

int ble_ots_server_indicate_response(uint16_t conn_id, uint16_t char_uuid,
                                      const uint8_t *data, uint16_t len)
{
    ble_ots_server_conn_state_t *cs;
    uint16_t val_handle = 0;

    int rc = ots_indicate_val_handle_get(conn_id, char_uuid, &val_handle);
    if (rc != 0) {
        return rc;
    }

    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
    if (!om) {
        ESP_LOGE(TAG, "indicate_response: failed to allocate mbuf");
        return BLE_HS_ENOMEM;
    }

    /* Defer the actual indication to a NimBLE host-task event so it is not
     * sent from within a GATT access callback. */
    struct ots_indicate_ctx *ctx = calloc(1, sizeof(*ctx));
    if (!ctx) {
        os_mbuf_free_chain(om);
        ESP_LOGE(TAG, "indicate_response: failed to allocate event context");
        return BLE_HS_ENOMEM;
    }
    ctx->conn_id = conn_id;
    ctx->val_handle = val_handle;
    ctx->om = om;
    ctx->next = NULL;
    ble_npl_event_init(&ctx->ev, ots_indicate_event_cb, ctx);

    /* Append to the per-connection queue. It is only posted to the host task
     * once the previously sent indication has been acknowledged. */
    OTS_LOCK();
    cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        ots_indicate_ctx_free(ctx);
        OTS_UNLOCK();
        return BLE_HS_ENOENT;
    }
    if (cs->indicate_tail) {
        cs->indicate_tail->next = ctx;
    } else {
        cs->indicate_head = ctx;
    }
    cs->indicate_tail = ctx;
    ots_indicate_queue_kick(cs);
    OTS_UNLOCK();

    return 0;
}

/*****************************************************************************
 * Dispatch Event
 *****************************************************************************/
void ble_ots_server_dispatch_event(ble_ots_server_event_t event,
                                    ble_ots_server_cb_param_t *param)
{
    if (ble_ots_server_env.app_cb) {
        ble_ots_server_env.app_cb(event, param);
    }
}

/*****************************************************************************
 * Transfer Timer Management
 *****************************************************************************/
static void ots_transfer_timer_cb(struct ble_npl_event *ev)
{
    uint16_t conn_id;

    /* The callout fires in the NimBLE host task context, so the pool slot it
     * points at can be freed concurrently by the application task. Resolve the
     * connection under the mutex, then run the cleanup without it — that path
     * reaches the application callback. */
    OTS_LOCK();
    ble_ots_server_conn_state_t *cs = BLE_OTS_SERVER_ENV_OK() ?
        (ble_ots_server_conn_state_t *)ble_npl_event_get_arg(ev) : NULL;
    if (!cs || !cs->in_pool || !cs->transfer_active) {
        OTS_UNLOCK();
        return;
    }
    conn_id = cs->conn_id;
    OTS_UNLOCK();

    ESP_LOGW(TAG, "Transfer timeout for conn_id=%d", conn_id);

    /* Delegate to transfer module for proper cleanup */
    ble_ots_server_oacp_transfer_timeout(conn_id);
}

void ble_ots_server_transfer_timer_start(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return;
    }

    if (!cs->transfer_timer_inited) {
        int rc = ble_npl_callout_init(&cs->transfer_timer,
                                       nimble_port_get_dflt_eventq(),
                                       ots_transfer_timer_cb, cs);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to init transfer callout: %d", rc);
            return;
        }
        cs->transfer_timer_inited = true;
    }

    uint32_t timeout_ms = (uint32_t)CONFIG_BLE_OTS_SERVER_TRANSFER_TIMEOUT_SEC * 1000U;
    ble_npl_callout_reset(&cs->transfer_timer,
                           ble_npl_time_ms_to_ticks32(timeout_ms));
}

void ble_ots_server_transfer_timer_stop(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs || !cs->transfer_timer_inited) {
        return;
    }
    ble_npl_callout_stop(&cs->transfer_timer);
}

void ble_ots_server_transfer_timer_reset(uint16_t conn_id)
{
    ble_ots_server_transfer_timer_stop(conn_id);
    ble_ots_server_transfer_timer_start(conn_id);
}

/*****************************************************************************
 * Name Uniqueness Check
 *****************************************************************************/
bool ble_ots_server_name_is_unique(const char *name, uint8_t name_len,
                                    ble_ots_obj_id_t exclude_id)
{
    bool unique = true;

    if (!name || name_len == 0) {
        /* Empty names are not checked for uniqueness */
        return true;
    }

    OTS_LOCK();
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[i];
        if (!obj->in_use) {
            continue;
        }
        if (obj->object_id == exclude_id) {
            continue;
        }
        if (obj->name_len == name_len &&
            memcmp(obj->name, name, name_len) == 0) {
            unique = false;
            break;
        }
    }
    OTS_UNLOCK();

    return unique;
}

/*****************************************************************************
 * GATT Access Callback — routes to appropriate handlers
 *****************************************************************************/
static int ots_gatt_access_handle(uint16_t conn_handle, uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt)
{
    const ble_uuid_t *uuid;
    uint16_t uuid16;

    /* Check concurrency */
    if (!ble_ots_server_concurrency_check(conn_handle)) {
        return BLE_OTS_APP_ERR_CONCURRENCY_LIMIT_EXCEEDED;
    }

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        uuid = ctxt->chr->uuid;
        if (uuid->type != BLE_UUID_TYPE_16) {
            return BLE_ATT_ERR_UNLIKELY;
        }
        uuid16 = BLE_UUID16(uuid)->value;

        switch (uuid16) {
        case BLE_OTS_UUID_OTS_FEATURE: {
            /* OTS Feature: 8 octets (OACP features + OLCP features) */
            uint8_t buf[8];
            buf[0] = (uint8_t)(ble_ots_server_env.ots_feature.oacp_features);
            buf[1] = (uint8_t)(ble_ots_server_env.ots_feature.oacp_features >> 8);
            buf[2] = (uint8_t)(ble_ots_server_env.ots_feature.oacp_features >> 16);
            buf[3] = (uint8_t)(ble_ots_server_env.ots_feature.oacp_features >> 24);
            buf[4] = (uint8_t)(ble_ots_server_env.ots_feature.olcp_features);
            buf[5] = (uint8_t)(ble_ots_server_env.ots_feature.olcp_features >> 8);
            buf[6] = (uint8_t)(ble_ots_server_env.ots_feature.olcp_features >> 16);
            buf[7] = (uint8_t)(ble_ots_server_env.ots_feature.olcp_features >> 24);
            int rc = os_mbuf_append(ctxt->om, buf, sizeof(buf));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

        case BLE_OTS_UUID_OBJECT_NAME:
        case BLE_OTS_UUID_OBJECT_TYPE:
        case BLE_OTS_UUID_OBJECT_SIZE:
        case BLE_OTS_UUID_OBJECT_FIRST_CREATED:
        case BLE_OTS_UUID_OBJECT_LAST_MODIFIED:
        case BLE_OTS_UUID_OBJECT_ID:
        case BLE_OTS_UUID_OBJECT_PROPERTIES:
            return ble_ots_server_metadata_read(conn_handle, attr_handle, ctxt);

        case BLE_OTS_UUID_OBJECT_LIST_FILTER: {
            /* Determine which filter instance by matching val_handle */
            int instance_idx = -1;
            for (int i = 0; i < BLE_OTS_FILTER_INSTANCE_COUNT; i++) {
                if (attr_handle == s_ots_server_init->filter_val_handles[i]) {
                    instance_idx = i;
                    break;
                }
            }
            if (instance_idx < 0) {
                return BLE_ATT_ERR_UNLIKELY;
            }
            uint8_t buf[BLE_OTS_FILTER_VALUE_MAX_LEN];
            uint16_t buf_len = sizeof(buf);
            int rc = ble_ots_server_filter_read(conn_handle, (uint8_t)instance_idx,
                                                 buf, &buf_len);
            if (rc != 0) {
                return rc;
            }
            rc = os_mbuf_append(ctxt->om, buf, buf_len);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

        default:
            return BLE_ATT_ERR_UNLIKELY;
        }
        break;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        uuid = ctxt->chr->uuid;
        if (uuid->type != BLE_UUID_TYPE_16) {
            return BLE_ATT_ERR_UNLIKELY;
        }
        uuid16 = BLE_UUID16(uuid)->value;

        switch (uuid16) {
        case BLE_OTS_UUID_OBJECT_NAME:
        case BLE_OTS_UUID_OBJECT_FIRST_CREATED:
        case BLE_OTS_UUID_OBJECT_LAST_MODIFIED:
        case BLE_OTS_UUID_OBJECT_PROPERTIES:
            return ble_ots_server_metadata_write(conn_handle, attr_handle, ctxt);

        case BLE_OTS_UUID_OACP:
            return ble_ots_server_oacp_write(conn_handle, ctxt);

        case BLE_OTS_UUID_OLCP:
            return ble_ots_server_olcp_write(conn_handle, ctxt);

        case BLE_OTS_UUID_OBJECT_LIST_FILTER: {
            int instance_idx = -1;
            for (int i = 0; i < BLE_OTS_FILTER_INSTANCE_COUNT; i++) {
                if (attr_handle == s_ots_server_init->filter_val_handles[i]) {
                    instance_idx = i;
                    break;
                }
            }
            if (instance_idx < 0) {
                return BLE_ATT_ERR_UNLIKELY;
            }
            /* Reject an oversized write up front */
            uint8_t write_buf[BLE_OTS_FILTER_VALUE_MAX_LEN];
            uint16_t write_len = 0;
            if (OS_MBUF_PKTLEN(ctxt->om) > sizeof(write_buf)) {
                return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
            }
            int rc = ble_hs_mbuf_to_flat(ctxt->om, write_buf, sizeof(write_buf), &write_len);
            if (rc != 0) {
                return BLE_ATT_ERR_UNLIKELY;
            }
            return ble_ots_server_filter_write(conn_handle, (uint8_t)instance_idx,
                                                write_buf, write_len);
        }

        default:
            return BLE_ATT_ERR_UNLIKELY;
        }
        break;

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

static int ots_gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int rc;

    /* NimBLE offers no way to unregister a GATT service, so this callback stays
     * dispatchable after ble_ots_server_deinit(). Re-validate the control block
     * under the mutex that deinit holds while releasing it — checking it before
     * taking the lock would only be a TOCTOU window. */
    OTS_LOCK();
    if (!BLE_OTS_SERVER_ENV_OK() || !ble_ots_server_env.started) {
        OTS_UNLOCK();
        return BLE_ATT_ERR_UNLIKELY;
    }
    rc = ots_gatt_access_handle(conn_handle, attr_handle, ctxt);
    OTS_UNLOCK();

    return rc;
}

/*****************************************************************************
 * Build GATT Service Table
 *****************************************************************************/
static int ots_build_gatt_service(const ble_ots_server_config_t *config)
{
    int idx = 0;
    bool multi_object = (CONFIG_BLE_OTS_SERVER_MAX_OBJECTS > 1);

    /* 1. OTS Feature — Mandatory, Read, Encryption Required */
    s_ots_server_init->chr_defs[idx].uuid = &ots_feature_uuid.u;
    s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
    s_ots_server_init->chr_defs[idx].arg = NULL;
    s_ots_server_init->chr_defs[idx].descriptors = NULL;
    s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
    s_ots_server_init->chr_defs[idx].min_key_size = 0;
    s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->ots_feature_val_handle;
    s_ots_server_init->chr_defs[idx].cpfd = NULL;
    idx++;

    /* 2. Object Name — Mandatory, Read + optional Write, Encryption Required */
    s_ots_server_init->chr_defs[idx].uuid = &obj_name_uuid.u;
    s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
    s_ots_server_init->chr_defs[idx].arg = NULL;
    s_ots_server_init->chr_defs[idx].descriptors = NULL;
    s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
    if (config->obj_name_writable) {
        s_ots_server_init->chr_defs[idx].flags |= BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
    }
    s_ots_server_init->chr_defs[idx].min_key_size = 0;
    s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->obj_name_val_handle;
    s_ots_server_init->chr_defs[idx].cpfd = NULL;
    idx++;

    /* 3. Object Type — Mandatory, Read, Encryption Required */
    s_ots_server_init->chr_defs[idx].uuid = &obj_type_uuid.u;
    s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
    s_ots_server_init->chr_defs[idx].arg = NULL;
    s_ots_server_init->chr_defs[idx].descriptors = NULL;
    s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
    s_ots_server_init->chr_defs[idx].min_key_size = 0;
    s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->obj_type_val_handle;
    s_ots_server_init->chr_defs[idx].cpfd = NULL;
    idx++;

    /* 4. Object Size — Mandatory, Read, Encryption Required */
    s_ots_server_init->chr_defs[idx].uuid = &obj_size_uuid.u;
    s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
    s_ots_server_init->chr_defs[idx].arg = NULL;
    s_ots_server_init->chr_defs[idx].descriptors = NULL;
    s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
    s_ots_server_init->chr_defs[idx].min_key_size = 0;
    s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->obj_size_val_handle;
    s_ots_server_init->chr_defs[idx].cpfd = NULL;
    idx++;

    /* 5. Object First-Created — Optional */
    if (config->include_obj_first_created) {
        s_ots_server_init->chr_defs[idx].uuid = &obj_first_created_uuid.u;
        s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
        s_ots_server_init->chr_defs[idx].arg = NULL;
        s_ots_server_init->chr_defs[idx].descriptors = NULL;
        s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
        /* Write is optional (C.3: mandatory if OACP Create supported) */
        if (config->oacp_features & BLE_OTS_OACP_FEAT_CREATE) {
            s_ots_server_init->chr_defs[idx].flags |= BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
        }
        s_ots_server_init->chr_defs[idx].min_key_size = 0;
        s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->obj_first_created_val_handle;
        s_ots_server_init->chr_defs[idx].cpfd = NULL;
        idx++;
    }

    /* 6. Object Last-Modified — Optional */
    if (config->include_obj_last_modified) {
        s_ots_server_init->chr_defs[idx].uuid = &obj_last_modified_uuid.u;
        s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
        s_ots_server_init->chr_defs[idx].arg = NULL;
        s_ots_server_init->chr_defs[idx].descriptors = NULL;
        s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
        /* C.4: Write mandatory if no real-time clock */
        if (!config->has_realtime_clock) {
            s_ots_server_init->chr_defs[idx].flags |= BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
        }
        s_ots_server_init->chr_defs[idx].min_key_size = 0;
        s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->obj_last_modified_val_handle;
        s_ots_server_init->chr_defs[idx].cpfd = NULL;
        idx++;
    }

    /* 7. Object ID — C.1: Mandatory if multi-object */
    if (multi_object) {
        s_ots_server_init->chr_defs[idx].uuid = &obj_id_uuid.u;
        s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
        s_ots_server_init->chr_defs[idx].arg = NULL;
        s_ots_server_init->chr_defs[idx].descriptors = NULL;
        s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
        s_ots_server_init->chr_defs[idx].min_key_size = 0;
        s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->obj_id_val_handle;
        s_ots_server_init->chr_defs[idx].cpfd = NULL;
        idx++;
    }

    /* 8. Object Properties — Mandatory, Read + optional Write, Encryption Required */
    s_ots_server_init->chr_defs[idx].uuid = &obj_properties_uuid.u;
    s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
    s_ots_server_init->chr_defs[idx].arg = NULL;
    s_ots_server_init->chr_defs[idx].descriptors = NULL;
    s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC;
    if (config->obj_properties_writable) {
        s_ots_server_init->chr_defs[idx].flags |= BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
    }
    s_ots_server_init->chr_defs[idx].min_key_size = 0;
    s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->obj_properties_val_handle;
    s_ots_server_init->chr_defs[idx].cpfd = NULL;
    idx++;

    /* 9. OACP — Mandatory, Write + Indicate, Encryption Required, CCCD auto-added */
    s_ots_server_init->chr_defs[idx].uuid = &oacp_uuid.u;
    s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
    s_ots_server_init->chr_defs[idx].arg = NULL;
    s_ots_server_init->chr_defs[idx].descriptors = NULL;
    s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                               BLE_GATT_CHR_F_INDICATE;
    s_ots_server_init->chr_defs[idx].min_key_size = 0;
    s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->oacp_val_handle;
    s_ots_server_init->chr_defs[idx].cpfd = NULL;
    idx++;

    /* 10. OLCP — C.1: Mandatory if multi-object, Write + Indicate, Encryption Required */
    if (multi_object) {
        s_ots_server_init->chr_defs[idx].uuid = &olcp_uuid.u;
        s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
        s_ots_server_init->chr_defs[idx].arg = NULL;
        s_ots_server_init->chr_defs[idx].descriptors = NULL;
        s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC |
                                   BLE_GATT_CHR_F_INDICATE;
        s_ots_server_init->chr_defs[idx].min_key_size = 0;
        s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->olcp_val_handle;
        s_ots_server_init->chr_defs[idx].cpfd = NULL;
        idx++;
    }

    /* 11. Object List Filter (×3) — C.2: Optional if multi-object */
    if (multi_object && config->include_obj_list_filter) {
        for (int f = 0; f < BLE_OTS_FILTER_INSTANCE_COUNT; f++) {
            s_ots_server_init->chr_defs[idx].uuid = &obj_list_filter_uuid.u;
            s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
            s_ots_server_init->chr_defs[idx].arg = NULL;
            s_ots_server_init->chr_defs[idx].descriptors = NULL;
            s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC |
                                       BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_ENC;
            s_ots_server_init->chr_defs[idx].min_key_size = 0;
            s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->filter_val_handles[f];
            s_ots_server_init->chr_defs[idx].cpfd = NULL;
            idx++;
        }
    }

    /* 12. Object Changed — Optional, Indicate, Encryption Required */
    if (config->include_obj_changed) {
        s_ots_server_init->chr_defs[idx].uuid = &obj_changed_uuid.u;
        s_ots_server_init->chr_defs[idx].access_cb = ots_gatt_access_cb;
        s_ots_server_init->chr_defs[idx].arg = NULL;
        s_ots_server_init->chr_defs[idx].descriptors = NULL;
        s_ots_server_init->chr_defs[idx].flags = BLE_GATT_CHR_F_INDICATE;
        s_ots_server_init->chr_defs[idx].min_key_size = 0;
        s_ots_server_init->chr_defs[idx].val_handle = &s_ots_server_init->obj_changed_val_handle;
        s_ots_server_init->chr_defs[idx].cpfd = NULL;
        idx++;
    }

    /* Terminator */
    memset(&s_ots_server_init->chr_defs[idx], 0, sizeof(struct ble_gatt_chr_def));

    /* Build service definition */
    s_ots_server_init->svc_defs[0].type = BLE_GATT_SVC_TYPE_PRIMARY;
    s_ots_server_init->svc_defs[0].uuid = &ots_svc_uuid.u;
    s_ots_server_init->svc_defs[0].includes = NULL;
    s_ots_server_init->svc_defs[0].characteristics = s_ots_server_init->chr_defs;

    /* Terminator */
    memset(&s_ots_server_init->svc_defs[1], 0, sizeof(struct ble_gatt_svc_def));

    return 0;
}

/*****************************************************************************
 * Create Directory Listing Object
 *****************************************************************************/
static int ots_create_dlo(void)
{
    /* Find a free slot for the DLO */
    int slot = -1;
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        if (!ble_ots_server_env.obj_db[i].in_use) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        ESP_LOGE(TAG, "No slot for DLO");
        return BLE_HS_ENOMEM;
    }

    ble_ots_server_obj_t *dlo = &ble_ots_server_env.obj_db[slot];
    memset(dlo, 0, sizeof(*dlo));
    dlo->in_use = true;
    dlo->object_id = BLE_OTS_OBJ_ID_DIRECTORY_LISTING;
    dlo->name_len = (uint8_t)(sizeof(BLE_OTS_DLO_NAME) - 1);
    memcpy(dlo->name, BLE_OTS_DLO_NAME, dlo->name_len);
    dlo->type_uuid_len = 2;
    dlo->type_uuid[0] = (uint8_t)(BLE_OTS_UUID_DIRECTORY_LISTING & 0xFF);
    dlo->type_uuid[1] = (uint8_t)((BLE_OTS_UUID_DIRECTORY_LISTING >> 8) & 0xFF);
    dlo->properties = BLE_OTS_OBJ_PROP_READ; /* Read-only */
    dlo->locked_by = BLE_OTS_CONN_ID_NONE;
    dlo->marked = 0;

    /* Allocate DLO content buffer */
    ble_ots_server_env.dlo_content = calloc(1, BLE_OTS_DLO_MAX_BUF_SIZE);
    if (!ble_ots_server_env.dlo_content) {
        ESP_LOGE(TAG, "Failed to allocate DLO content buffer");
        dlo->in_use = false;
        return BLE_HS_ENOMEM;
    }
    ble_ots_server_env.dlo_content_size = 0;

    /* Point DLO data to the shared buffer */
    dlo->data = ble_ots_server_env.dlo_content;
    dlo->current_size = 0;
    dlo->allocated_size = BLE_OTS_DLO_MAX_BUF_SIZE;

    ESP_LOGI(TAG, "Directory Listing Object created (ID=0x000000000000)");
    return 0;
}

/*****************************************************************************
 * L2CAP OTC Event Callback
 *****************************************************************************/
static int ots_l2cap_event_handle(struct ble_l2cap_event *event)
{
    switch (event->type) {
    case BLE_L2CAP_EVENT_COC_ACCEPT: {
        uint16_t conn_handle = event->accept.conn_handle;
        ESP_LOGI(TAG, "L2CAP COC accept: conn_handle=%d", conn_handle);

        /* Reject new L2CAP connections if server is not started */
        if (!ble_ots_server_env.started) {
            ESP_LOGW(TAG, "L2CAP accept: server not started, rejecting");
            return BLE_HS_ENOTSUP;
        }

        ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_handle);
        if (!cs) {
            ESP_LOGW(TAG, "L2CAP accept: connection not in pool, rejecting");
            return BLE_HS_ENOMEM;
        }

        /* Provide an SDU receive buffer */
        struct os_mbuf *sdu_rx = ble_hs_mbuf_from_flat(NULL, 0);
        if (!sdu_rx) {
            ESP_LOGE(TAG, "L2CAP accept: failed to allocate SDU rx buffer");
            return BLE_HS_ENOMEM;
        }
        ble_l2cap_recv_ready(event->accept.chan, sdu_rx);
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_CONNECTED: {
        uint16_t conn_handle = event->connect.conn_handle;
        if (event->connect.status != 0) {
            ESP_LOGW(TAG, "L2CAP COC connect failed: conn_handle=%d status=%d",
                     conn_handle, event->connect.status);
            return 0;
        }
        ESP_LOGI(TAG, "L2CAP COC connected: conn_handle=%d", conn_handle);

        ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_handle);
        if (cs) {
            cs->otc_chan = event->connect.chan;
        }
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_DISCONNECTED: {
        uint16_t conn_handle = event->disconnect.conn_handle;
        ESP_LOGI(TAG, "L2CAP COC disconnected: conn_handle=%d", conn_handle);

        ble_ots_server_otc_disconnected_cb(conn_handle, event->disconnect.chan);
        return 0;
    }

    case BLE_L2CAP_EVENT_COC_DATA_RECEIVED: {
        uint16_t conn_handle = event->receive.conn_handle;
        ESP_LOGD(TAG, "L2CAP COC data received: conn_handle=%d", conn_handle);

        ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_handle);
        if (!cs) {
            /* Free the received SDU to avoid memory leak */
            os_mbuf_free_chain(event->receive.sdu_rx);
            return 0;
        }

        int rc = ble_ots_server_oacp_l2cap_recv(conn_handle, event->receive.chan,
                                                  event->receive.sdu_rx);

        /* Provide a new SDU rx buffer for next receive */
        struct os_mbuf *sdu_rx = ble_hs_mbuf_from_flat(NULL, 0);
        if (sdu_rx) {
            ble_l2cap_recv_ready(event->receive.chan, sdu_rx);
        }
        return rc;
    }

    case BLE_L2CAP_EVENT_COC_TX_UNSTALLED: {
        uint16_t conn_handle = event->tx_unstalled.conn_handle;
        ESP_LOGD(TAG, "L2CAP COC TX un-stalled: conn_handle=%d status=%d",
                 conn_handle, event->tx_unstalled.status);

        ble_ots_server_otc_tx_unstalled_cb(conn_handle, event->tx_unstalled.chan);
        return 0;
    }

    default:
        return 0;
    }
}

static int ots_l2cap_event_cb(struct ble_l2cap_event *event, void *arg)
{
    int rc;

    /* NimBLE offers no way to unregister an L2CAP server, so this callback
     * stays dispatchable after ble_ots_server_deinit(); re-validate the control
     * block under the mutex deinit holds while releasing it. */
    OTS_LOCK();
    if (!BLE_OTS_SERVER_ENV_OK()) {
        OTS_UNLOCK();
        return BLE_HS_ENOTSUP;
    }
    rc = ots_l2cap_event_handle(event);
    OTS_UNLOCK();

    return rc;
}

/*****************************************************************************
 * Post-Disconnection Cleanup
 *****************************************************************************/
static void ots_disconnect_cleanup(uint16_t conn_handle)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_handle);
    if (!cs) {
        return;
    }

    ESP_LOGI(TAG, "Disconnect cleanup for conn_handle=%d", conn_handle);

    /* Step 1: Delete unnamed objects created during this session */
    for (int i = 0; i < cs->created_obj_count; i++) {
        ble_ots_obj_id_t obj_id = cs->created_obj_ids[i];
        ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(obj_id);
        if (obj && obj->name_len == 0) {
            ESP_LOGI(TAG, "Deleting unnamed object ID=0x%012llX",
                     (unsigned long long)obj_id);
            ble_ots_server_obj_db_delete(obj_id);
        }
    }

    /* Step 2: Unlock objects locked by this connection */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[i];
        if (obj->in_use && obj->locked_by == conn_handle) {
            obj->locked_by = BLE_OTS_CONN_ID_NONE;
            ESP_LOGI(TAG, "Unlocked object ID=0x%012llX on disconnect",
                     (unsigned long long)obj->object_id);
        }
    }

    /* Step 3: Filter and current object cache discard (handled by pool free) */
    ble_ots_server_filter_reset(conn_handle);

    /* Step 4: Abort active transfer and notify the application */
    if (cs->transfer_active) {
        ble_ots_server_transfer_timer_stop(conn_handle);
        /* Notify the application of the aborted transfer before clearing state */
        ble_ots_server_oacp_transfer_timeout(conn_handle);
    }

    /* Step 5: Close OTC if still open */
    if (cs->otc_chan) {
        cs->otc_chan = NULL; /* Channel is implicitly released on disconnect */
    }

    /* Release the pool slot */
    conn_pool_free(cs);

    /* Rebuild DLO if multi-object (objects may have been deleted) */
    if (CONFIG_BLE_OTS_SERVER_MAX_OBJECTS > 1) {
        ble_ots_server_dlo_rebuild();
    }
}

/*****************************************************************************
 * GAP Event Callback
 *****************************************************************************/
static int ots_gap_event_handle(struct ble_gap_event *event)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            uint16_t conn_handle = event->connect.conn_handle;
            ESP_LOGI(TAG, "GAP connect: conn_handle=%d", conn_handle);

            if (!ble_ots_server_env.started) {
                ESP_LOGW(TAG, "GAP connect: server not started, ignoring conn_handle=%d",
                         conn_handle);
                return 0;
            }

            /* Try to admit to concurrency pool */
            ble_ots_server_conn_state_t *cs = conn_pool_alloc(conn_handle);
            if (!cs) {
                ESP_LOGW(TAG, "Concurrency pool full, conn_handle=%d excluded",
                         conn_handle);
            }
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT: {
        uint16_t conn_handle = event->disconnect.conn.conn_handle;
        ESP_LOGI(TAG, "GAP disconnect: conn_handle=%d reason=%d",
                 conn_handle, event->disconnect.reason);
        /* Release the peer's bond slot and marked bits.  Done from here rather
         * than ots_disconnect_cleanup() because a peer may hold a bond slot
         * (via ble_ots_server_mark_object) without ever entering the
         * concurrency pool.  The peer identity comes from the descriptor
         * carried by the event: the connection object is already gone. */
        ble_ots_server_mark_cleanup_on_disconnect(&event->disconnect.conn.peer_id_addr,
                                                  event->disconnect.conn.sec_state.bonded);
        ots_disconnect_cleanup(conn_handle);
        return 0;
    }

    case BLE_GAP_EVENT_SUBSCRIBE: {
        uint16_t conn_handle = event->subscribe.conn_handle;
        uint16_t attr_handle = event->subscribe.attr_handle;
        bool indicate = event->subscribe.cur_indicate;

        ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_handle);
        if (!cs) {
            return 0;
        }

        if (attr_handle == s_ots_server_init->oacp_val_handle) {
            cs->oacp_subscribed = indicate;
            ESP_LOGI(TAG, "OACP CCCD indicate %s for conn=%d",
                     indicate ? "enabled" : "disabled", conn_handle);
        } else if (attr_handle == s_ots_server_init->olcp_val_handle) {
            cs->olcp_indicate_subscribed = indicate;
            ESP_LOGI(TAG, "OLCP CCCD indicate %s for conn=%d",
                     indicate ? "enabled" : "disabled", conn_handle);
        } else if (attr_handle == s_ots_server_init->obj_changed_val_handle) {
            cs->obj_changed_subscribed = indicate;
            ESP_LOGI(TAG, "Object Changed CCCD indicate %s for conn=%d",
                     indicate ? "enabled" : "disabled", conn_handle);
        }
        return 0;
    }

    case BLE_GAP_EVENT_NOTIFY_TX:
        /* Only the terminal outcome of one of our own indications releases the
         * per-connection slot. status == 0 merely reports that the indication
         * was handed to the controller; the confirmation is still pending. */
        if (event->notify_tx.indication && event->notify_tx.status != 0 &&
            (event->notify_tx.attr_handle == s_ots_server_init->oacp_val_handle ||
             event->notify_tx.attr_handle == s_ots_server_init->olcp_val_handle ||
             event->notify_tx.attr_handle == s_ots_server_init->obj_changed_val_handle)) {
            ots_indicate_completed(event->notify_tx.conn_handle);
        }
        return 0;

    default:
        return 0;
    }
}

static int ots_gap_event_cb(struct ble_gap_event *event, void *arg)
{
    int rc;

    /* Serialize against the application task and re-validate under the lock:
     * ble_ots_server_deinit() releases the control block while holding it. */
    OTS_LOCK();
    if (!BLE_OTS_SERVER_ENV_OK() || !ble_ots_server_env.initialized) {
        OTS_UNLOCK();
        return 0;
    }
    rc = ots_gap_event_handle(event);
    OTS_UNLOCK();

    return rc;
}

/*****************************************************************************
 * Public API: ble_ots_server_init
 *****************************************************************************/
int ble_ots_server_init(const ble_ots_server_config_t *config)
{
    int rc;

    if (!config) {
        ESP_LOGE(TAG, "init: config is NULL");
        return BLE_HS_EINVAL;
    }

    if (BLE_OTS_SERVER_ENV_OK()) {
        ESP_LOGE(TAG, "init: already initialized");
        return BLE_HS_EALREADY;
    }

    /* Validate config */
    if ((config->oacp_features & BLE_OTS_OACP_FEAT_CREATE) && !config->obj_name_writable) {
        ESP_LOGE(TAG, "init: OACP Create requires obj_name_writable");
        return BLE_HS_EINVAL;
    }

    /* Create the mutex before anything else so that every path below — and any
     * NimBLE callback still registered from an earlier init/deinit cycle — is
     * serialized. It is created once and deliberately never destroyed. */
    if (!s_ots_server_mutex) {
        s_ots_server_mutex = xSemaphoreCreateRecursiveMutex();
        if (!s_ots_server_mutex) {
            ESP_LOGE(TAG, "init: failed to create mutex");
            return BLE_HS_ENOMEM;
        }
    }

    /* Allocate the module-lifetime init context (GATT service/characteristic
     * tables, tracked value handles, and the GAP event listener) on the heap.
     * calloc zero-initializes it. Reused as-is across init/deinit cycles: once
     * the GATT service has been registered, NimBLE holds pointers into it and
     * it can neither be freed nor rebuilt. */
    if (!s_ots_server_init) {
        s_ots_server_init = calloc(1, sizeof(ots_server_init_ctx_t));
        if (!s_ots_server_init) {
            ESP_LOGE(TAG, "init: failed to allocate server init context");
            return BLE_HS_ENOMEM;
        }
    }

    /* Allocate the control block. calloc zero-initializes it, matching the
     * previous .bss semantics, so no extra memset is needed below. */
    p_ble_ots_server_env = calloc(1, sizeof(ble_ots_server_cb_env_t));
    if (!p_ble_ots_server_env) {
        ESP_LOGE(TAG, "init: failed to allocate control block");
        rc = BLE_HS_ENOMEM;
        goto err_cleanup_init_ctx;
    }

    /* Copy configuration */
    memcpy(&ble_ots_server_env.config, config, sizeof(ble_ots_server_config_t));

    /* Set OTS Feature */
    ble_ots_server_env.ots_feature.oacp_features = config->oacp_features;
    ble_ots_server_env.ots_feature.olcp_features = config->olcp_features;

    /* Initialize object database */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        ble_ots_server_env.obj_db[i].in_use = false;
        ble_ots_server_env.obj_db[i].locked_by = BLE_OTS_CONN_ID_NONE;
    }
    ble_ots_server_env.next_obj_id = BLE_OTS_OBJ_ID_MIN_USER;

    /* Initialize concurrency pool */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        ble_ots_server_env.conn_pool[i].in_pool = false;
        ble_ots_server_env.conn_pool[i].conn_id = BLE_OTS_CONN_ID_NONE;
        ble_ots_server_env.conn_pool[i].transfer_timer_inited = false;
        ble_ots_server_env.conn_pool[i].transfer_retry_timer_inited = false;
    }

    /* Allocate the filter/changed sub-module context */
    rc = ble_ots_server_filter_changed_init();
    if (rc != 0) {
        ESP_LOGE(TAG, "init: failed to init filter/changed context rc=%d", rc);
        goto err_cleanup_env;
    }

    /* Create DLO if multi-object */
    if (CONFIG_BLE_OTS_SERVER_MAX_OBJECTS > 1) {
        rc = ots_create_dlo();
        if (rc != 0) {
            ESP_LOGE(TAG, "init: failed to create DLO rc=%d", rc);
            goto err_cleanup_filter;
        }
    }

    /* Build and register the GATT service — exactly once per boot.
     * ble_gatts_add_svcs() stores the svc_defs pointer without copying and the
     * registered attributes keep pointers into chr_defs[], so on a re-init the
     * tables must be left untouched and the service must not be added again:
     * a second registration would expose a duplicate OTS service whose
     * attribute handles no longer match the ones tracked here. */
    if (!s_ots_gatt_registered) {
        rc = ots_build_gatt_service(config);
        if (rc != 0) {
            ESP_LOGE(TAG, "init: failed to build GATT service rc=%d", rc);
            goto err_cleanup_dlo;
        }

        rc = ble_gatts_count_cfg(s_ots_server_init->svc_defs);
        if (rc != 0) {
            ESP_LOGE(TAG, "init: ble_gatts_count_cfg failed rc=%d", rc);
            goto err_cleanup_dlo;
        }

        rc = ble_gatts_add_svcs(s_ots_server_init->svc_defs);
        if (rc != 0) {
            ESP_LOGE(TAG, "init: ble_gatts_add_svcs failed rc=%d", rc);
            goto err_cleanup_dlo;
        }

        /* From here on the host owns s_ots_server_init. */
        s_ots_gatt_registered = true;
    } else {
        ESP_LOGW(TAG, "init: reusing the GATT service registered by a previous "
                 "init; characteristic layout follows that configuration");
    }

    /* Register L2CAP server on PSM_OTS.
     * BLE_HS_EALREADY is acceptable — the server may already be registered
     * from a previous init cycle (NimBLE does not provide an unregister API). */
    rc = ble_l2cap_create_server(BLE_OTS_PSM_OTS, OTS_L2CAP_COC_MTU,
                                  ots_l2cap_event_cb, NULL);
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        ESP_LOGE(TAG, "init: ble_l2cap_create_server failed rc=%d", rc);
        goto err_cleanup_dlo;
    }

    /* Register GAP event listener for connect/disconnect */
    rc = ble_gap_event_listener_register(&s_ots_server_init->gap_listener, ots_gap_event_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "init: ble_gap_event_listener_register failed rc=%d", rc);
        goto err_cleanup_dlo;
    }

    ble_ots_server_env.initialized = true;
    ble_ots_server_env.started = true;
    ESP_LOGI(TAG, "OTS server initialized (max_objects=%d, max_concurrency=%d)",
             CONFIG_BLE_OTS_SERVER_MAX_OBJECTS, CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY);
    return 0;

err_cleanup_dlo:
    /* Free DLO content buffer and mark DLO slot unused on init failure */
    if (ble_ots_server_env.dlo_content) {
        for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
            ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[i];
            if (obj->in_use && obj->object_id == BLE_OTS_OBJ_ID_DIRECTORY_LISTING) {
                obj->data = NULL;
                obj->in_use = false;
                break;
            }
        }
        free(ble_ots_server_env.dlo_content);
        ble_ots_server_env.dlo_content = NULL;
        ble_ots_server_env.dlo_content_size = 0;
    }
err_cleanup_filter:
    ble_ots_server_filter_changed_deinit();
err_cleanup_env:
    /* Callbacks that are already registered guard on BLE_OTS_SERVER_ENV_OK()
     * under this mutex, so take it before dropping the control block. */
    OTS_LOCK();
    free(p_ble_ots_server_env);
    p_ble_ots_server_env = NULL;
    OTS_UNLOCK();
err_cleanup_init_ctx:
    /* Only safe while the host has not taken ownership of the GATT tables.
     * Once it has, the block is kept for the lifetime of the module. */
    if (!s_ots_gatt_registered) {
        free(s_ots_server_init);
        s_ots_server_init = NULL;
    }
    /* s_ots_server_mutex is intentionally kept — see its definition. */
    return rc;
}

/*****************************************************************************
 * Public API: ble_ots_server_deinit
 *****************************************************************************/
int ble_ots_server_deinit(void)
{
    if (!BLE_OTS_SERVER_ENV_OK() || !ble_ots_server_env.initialized) {
        return BLE_HS_EALREADY;
    }

    /* Unregister the GAP event listener before anything is torn down: a
     * disconnect delivered on the host task would otherwise run
     * ots_disconnect_cleanup() against the state being freed here. The GATT
     * and L2CAP callbacks cannot be unregistered at all (NimBLE has no such
     * API), so they re-validate the control block under the mutex taken below.
     */
    ble_gap_event_listener_unregister(&s_ots_server_init->gap_listener);

    OTS_LOCK();

    /* Stop serving first: NimBLE callbacks that are already running observe
     * this and bail out before we start tearing the control block down. */
    ble_ots_server_env.started = false;

    /* Close all OTC channels and free pool slots */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[i];
        if (cs->in_pool) {
            if (cs->otc_chan) {
                ble_l2cap_disconnect(cs->otc_chan);
                cs->otc_chan = NULL;
            }
            conn_pool_free(cs);
        }
    }

    /* Free object data buffers */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[i];
        if (obj->in_use) {
            /* Don't free DLO data — it's the shared dlo_content buffer */
            if (obj->object_id != BLE_OTS_OBJ_ID_DIRECTORY_LISTING && obj->data) {
                free(obj->data);
            }
            obj->data = NULL;
            obj->in_use = false;
        }
    }

    /* Free DLO content buffer */
    if (ble_ots_server_env.dlo_content) {
        free(ble_ots_server_env.dlo_content);
        ble_ots_server_env.dlo_content = NULL;
    }

    /* Free the filter/changed sub-module context */
    ble_ots_server_filter_changed_deinit();

    /* Release the control block. The still-registered GATT/L2CAP callbacks
     * take this same mutex and re-check BLE_OTS_SERVER_ENV_OK() before
     * dereferencing the pointer, so none of them can be mid-access here. */
    ble_ots_server_env.app_cb = NULL;
    ble_ots_server_env.initialized = false;
    free(p_ble_ots_server_env);
    p_ble_ots_server_env = NULL;

    OTS_UNLOCK();

    /* s_ots_server_mutex and s_ots_server_init are deliberately kept: NimBLE
     * still holds the GATT/L2CAP registrations, so their callbacks — and the
     * mutex they synchronize on — must stay valid. Both are reused verbatim by
     * a subsequent ble_ots_server_init(). */

    ESP_LOGI(TAG, "OTS server deinitialized");
    return 0;
}

/*****************************************************************************
 * Public API: ble_ots_server_register_cb
 *****************************************************************************/
int ble_ots_server_register_cb(ble_ots_server_cb_t callback)
{
    if (!callback) {
        return BLE_HS_EINVAL;
    }

    if (!BLE_OTS_SERVER_ENV_OK()) {
        ESP_LOGE(TAG, "register_cb: server not initialized");
        return BLE_HS_EINVAL;
    }

    ble_ots_server_env.app_cb = callback;
    return 0;
}
