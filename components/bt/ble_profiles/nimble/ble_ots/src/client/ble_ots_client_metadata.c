/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "host/ble_att.h"
#include "host/ble_hs_mbuf.h"
#include "os/os_mbuf.h"
#include "os/endian.h"
#include "ble_ots_client_int.h"

static const char *TAG = "ble_ots_meta";

/*****************************************************************************
 * Helper: encode ble_ots_date_time_t into 7-octet LE buffer
 *****************************************************************************/
static void encode_datetime(const ble_ots_date_time_t *dt, uint8_t buf[7])
{
    put_le16(buf, dt->year);
    buf[2] = dt->month;
    buf[3] = dt->day;
    buf[4] = dt->hours;
    buf[5] = dt->minutes;
    buf[6] = dt->seconds;
}

/*****************************************************************************
 * Helper: decode 7-octet LE buffer into ble_ots_date_time_t
 *****************************************************************************/
static void decode_datetime(const uint8_t buf[7], ble_ots_date_time_t *dt)
{
    dt->year    = get_le16(buf);
    dt->month   = buf[2];
    dt->day     = buf[3];
    dt->hours   = buf[4];
    dt->minutes = buf[5];
    dt->seconds = buf[6];
}

/*****************************************************************************
 * Helper: get LE uint48 from 6-byte buffer (stored in uint64_t)
 *****************************************************************************/
static uint64_t ots_get_le48(const uint8_t *buf)
{
    uint64_t val = 0;
    for (int i = 5; i >= 0; i--) {
        val = (val << 8) | buf[i];
    }
    return val;
}

/*****************************************************************************
 * GATT Read Callback: Object Name (Read Long fallback)
 *****************************************************************************/

/* Context passed to name read long callback */
typedef struct {
    uint16_t conn_id;
} name_read_ctx_t;

static int
name_read_long_cb(uint16_t conn_handle,
                  const struct ble_gatt_error *error,
                  struct ble_gatt_attr *attr,
                  void *arg)
{
    name_read_ctx_t *ctx = (name_read_ctx_t *)arg;
    ble_ots_client_conn_ctx_t *conn_ctx = ble_ots_client_get_conn_ctx(ctx->conn_id);

    if (conn_ctx == NULL) {
        /* Connection was disconnected during Read Long */
        ble_ots_client_object_name_read_t evt = {0};
        evt.status   = (error->status != 0) ? error->status : BLE_HS_ENOTCONN;
        evt.name     = NULL;
        evt.name_len = 0;
        ble_ots_client_dispatch_event(ctx->conn_id,
                                      BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ,
                                      &evt);
        free(ctx);
        return 0;
    }

    if (error->status == 0 && attr != NULL && attr->om != NULL) {
        /* Accumulate data fragment into name_buf */
        uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
        uint16_t space = CONFIG_BLE_OTS_CLIENT_MAX_NAME_LEN - conn_ctx->name_buf_len;
        uint16_t copy_len = data_len < space ? data_len : space;

        if (copy_len > 0) {
            os_mbuf_copydata(attr->om, 0, copy_len,
                             conn_ctx->name_buf + conn_ctx->name_buf_len);
            conn_ctx->name_buf_len += copy_len;
        }
        /* More fragments may follow; return 0 to continue */
        return 0;
    }

    /* Completion or error */
    ble_ots_client_object_name_read_t evt = {0};

    if (error->status == BLE_HS_EDONE || error->status == 0) {
        /* Read Long completed successfully */
        evt.status   = 0;
        evt.name     = conn_ctx->name_buf;
        evt.name_len = conn_ctx->name_buf_len;
    } else if (error->status == BLE_HS_ATT_ERR(BLE_ATT_ERR_ATTR_NOT_LONG)) {
        /* No more data beyond what we already have */
        evt.status   = 0;
        evt.name     = conn_ctx->name_buf;
        evt.name_len = conn_ctx->name_buf_len;
    } else if (error->status == BLE_HS_ATT_ERR(BLE_ATT_ERR_REQ_NOT_SUPPORTED)) {
        /* Server doesn't support Read Long */
        if (conn_ctx) {
            conn_ctx->server_supports_read_long = false;
        }
        evt.status   = 0;
        evt.name     = conn_ctx->name_buf;
        evt.name_len = conn_ctx->name_buf_len;
    } else {
        evt.status   = error->status;
        evt.name     = conn_ctx->name_buf;
        evt.name_len = conn_ctx->name_buf_len;
    }

    ble_ots_client_dispatch_event(ctx->conn_id,
                                  BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ,
                                  &evt);
    free(ctx);
    return 0;
}

static int
name_read_cb(uint16_t conn_handle,
             const struct ble_gatt_error *error,
             struct ble_gatt_attr *attr,
             void *arg)
{
    uint16_t conn_id = conn_handle;
    ble_ots_client_conn_ctx_t *conn_ctx = ble_ots_client_get_conn_ctx(conn_id);

    if (error->status != 0 || attr == NULL || attr->om == NULL) {
        /* Error or no data */
        ble_ots_client_object_name_read_t evt = {0};
        evt.status = error->status;
        ble_ots_client_dispatch_event(conn_id,
                                      BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ,
                                      &evt);
        return 0;
    }

    if (conn_ctx == NULL) {
        ble_ots_client_object_name_read_t evt = {0};
        evt.status = BLE_HS_ENOTCONN;
        ble_ots_client_dispatch_event(conn_id,
                                      BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ,
                                      &evt);
        return 0;
    }

    /* Copy initial read data into name_buf */
    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
    uint16_t copy_len = data_len;
    if (copy_len > CONFIG_BLE_OTS_CLIENT_MAX_NAME_LEN) {
        copy_len = CONFIG_BLE_OTS_CLIENT_MAX_NAME_LEN;
    }

    conn_ctx->name_buf_len = 0;
    os_mbuf_copydata(attr->om, 0, copy_len, conn_ctx->name_buf);
    conn_ctx->name_buf_len = copy_len;

    /* Check if we need Read Long: if data_len == ATT_MTU - 1, there may be more */
    uint16_t att_mtu = ble_att_mtu(conn_handle);
    if (att_mtu > 0 && data_len == (att_mtu - 1) &&
        conn_ctx->server_supports_read_long &&
        conn_ctx->name_buf_len < CONFIG_BLE_OTS_CLIENT_MAX_NAME_LEN) {
        /* Issue Read Long starting from offset = data_len */
        name_read_ctx_t *ctx = malloc(sizeof(name_read_ctx_t));
        if (ctx == NULL) {
            ble_ots_client_object_name_read_t evt = {0};
            evt.status   = BLE_HS_ENOMEM;
            evt.name     = conn_ctx->name_buf;
            evt.name_len = conn_ctx->name_buf_len;
            ble_ots_client_dispatch_event(conn_id,
                                          BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ,
                                          &evt);
            return 0;
        }
        ctx->conn_id = conn_id;

        int rc = ble_gattc_read_long(conn_handle,
                                     conn_ctx->handles.object_name_handle,
                                     data_len,
                                     name_read_long_cb,
                                     ctx);
        if (rc != 0) {
            ESP_LOGE(TAG, "Read Long failed to initiate; rc=%d", rc);
            free(ctx);
            /* Fall through and report what we have */
            ble_ots_client_object_name_read_t evt = {0};
            evt.status   = rc;
            evt.name     = conn_ctx->name_buf;
            evt.name_len = conn_ctx->name_buf_len;
            ble_ots_client_dispatch_event(conn_id,
                                          BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ,
                                          &evt);
        }
        return 0;
    }

    /* No Read Long needed — report the result directly */
    ble_ots_client_object_name_read_t evt = {0};
    evt.status   = 0;
    evt.name     = conn_ctx->name_buf;
    evt.name_len = conn_ctx->name_buf_len;
    ble_ots_client_dispatch_event(conn_id,
                                  BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ,
                                  &evt);
    return 0;
}

/*****************************************************************************
 * GATT Read Callback: Object Type
 *****************************************************************************/
static int
type_read_cb(uint16_t conn_handle,
             const struct ble_gatt_error *error,
             struct ble_gatt_attr *attr,
             void *arg)
{
    ble_ots_client_object_type_read_t evt = {0};

    if (error->status != 0 || attr == NULL || attr->om == NULL) {
        evt.status = error->status;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_OBJECT_TYPE_READ,
                                      &evt);
        return 0;
    }

    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);

    if (data_len == 2 || data_len == 16) {
        evt.status   = 0;
        evt.uuid_len = (uint8_t)data_len;
        os_mbuf_copydata(attr->om, 0, data_len, evt.uuid);
    } else {
        ESP_LOGE(TAG, "Unexpected Object Type length: %u", data_len);
        evt.status = BLE_HS_EINVAL;
    }

    ble_ots_client_dispatch_event(conn_handle,
                                  BLE_OTS_CLIENT_EVT_OBJECT_TYPE_READ,
                                  &evt);
    return 0;
}

/*****************************************************************************
 * GATT Read Callback: Object Size
 *****************************************************************************/
static int
size_read_cb(uint16_t conn_handle,
             const struct ble_gatt_error *error,
             struct ble_gatt_attr *attr,
             void *arg)
{
    ble_ots_client_object_size_read_t evt = {0};

    if (error->status != 0 || attr == NULL || attr->om == NULL) {
        evt.status = error->status;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_OBJECT_SIZE_READ,
                                      &evt);
        return 0;
    }

    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
    if (data_len < 8) {
        ESP_LOGE(TAG, "Object Size too short: %u", data_len);
        evt.status = BLE_HS_EINVAL;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_OBJECT_SIZE_READ,
                                      &evt);
        return 0;
    }

    uint8_t buf[8];
    os_mbuf_copydata(attr->om, 0, 8, buf);

    evt.status         = 0;
    evt.current_size   = get_le32(buf);
    evt.allocated_size = get_le32(buf + 4);

    ble_ots_client_dispatch_event(conn_handle,
                                  BLE_OTS_CLIENT_EVT_OBJECT_SIZE_READ,
                                  &evt);
    return 0;
}

/*****************************************************************************
 * GATT Read Callback: First-Created
 *****************************************************************************/
static int
first_created_read_cb(uint16_t conn_handle,
                      const struct ble_gatt_error *error,
                      struct ble_gatt_attr *attr,
                      void *arg)
{
    ble_ots_client_datetime_read_t evt = {0};

    if (error->status != 0 || attr == NULL || attr->om == NULL) {
        evt.status = error->status;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_FIRST_CREATED_READ,
                                      &evt);
        return 0;
    }

    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
    if (data_len < 7) {
        ESP_LOGE(TAG, "First-Created too short: %u", data_len);
        evt.status = BLE_HS_EINVAL;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_FIRST_CREATED_READ,
                                      &evt);
        return 0;
    }

    uint8_t buf[7];
    os_mbuf_copydata(attr->om, 0, 7, buf);
    decode_datetime(buf, &evt.datetime);
    evt.status = 0;

    ble_ots_client_dispatch_event(conn_handle,
                                  BLE_OTS_CLIENT_EVT_FIRST_CREATED_READ,
                                  &evt);
    return 0;
}

/*****************************************************************************
 * GATT Read Callback: Last-Modified
 *****************************************************************************/
static int
last_modified_read_cb(uint16_t conn_handle,
                      const struct ble_gatt_error *error,
                      struct ble_gatt_attr *attr,
                      void *arg)
{
    ble_ots_client_datetime_read_t evt = {0};

    if (error->status != 0 || attr == NULL || attr->om == NULL) {
        evt.status = error->status;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_LAST_MODIFIED_READ,
                                      &evt);
        return 0;
    }

    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
    if (data_len < 7) {
        ESP_LOGE(TAG, "Last-Modified too short: %u", data_len);
        evt.status = BLE_HS_EINVAL;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_LAST_MODIFIED_READ,
                                      &evt);
        return 0;
    }

    uint8_t buf[7];
    os_mbuf_copydata(attr->om, 0, 7, buf);
    decode_datetime(buf, &evt.datetime);
    evt.status = 0;

    ble_ots_client_dispatch_event(conn_handle,
                                  BLE_OTS_CLIENT_EVT_LAST_MODIFIED_READ,
                                  &evt);
    return 0;
}

/*****************************************************************************
 * GATT Read Callback: Object ID
 *****************************************************************************/
static int
object_id_read_cb(uint16_t conn_handle,
                  const struct ble_gatt_error *error,
                  struct ble_gatt_attr *attr,
                  void *arg)
{
    ble_ots_client_object_id_read_t evt = {0};

    if (error->status != 0 || attr == NULL || attr->om == NULL) {
        evt.status = error->status;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_OBJECT_ID_READ,
                                      &evt);
        return 0;
    }

    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
    if (data_len < 6) {
        ESP_LOGE(TAG, "Object ID too short: %u", data_len);
        evt.status = BLE_HS_EINVAL;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_OBJECT_ID_READ,
                                      &evt);
        return 0;
    }

    uint8_t buf[6];
    os_mbuf_copydata(attr->om, 0, 6, buf);
    evt.status    = 0;
    evt.object_id = ots_get_le48(buf);

    ble_ots_client_dispatch_event(conn_handle,
                                  BLE_OTS_CLIENT_EVT_OBJECT_ID_READ,
                                  &evt);
    return 0;
}

/*****************************************************************************
 * GATT Read Callback: Object Properties
 *****************************************************************************/
static int
properties_read_cb(uint16_t conn_handle,
                   const struct ble_gatt_error *error,
                   struct ble_gatt_attr *attr,
                   void *arg)
{
    ble_ots_client_properties_read_t evt = {0};

    if (error->status != 0 || attr == NULL || attr->om == NULL) {
        evt.status = error->status;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_PROPERTIES_READ,
                                      &evt);
        return 0;
    }

    uint16_t data_len = OS_MBUF_PKTLEN(attr->om);
    if (data_len < 4) {
        ESP_LOGE(TAG, "Object Properties too short: %u", data_len);
        evt.status = BLE_HS_EINVAL;
        ble_ots_client_dispatch_event(conn_handle,
                                      BLE_OTS_CLIENT_EVT_PROPERTIES_READ,
                                      &evt);
        return 0;
    }

    uint8_t buf[4];
    os_mbuf_copydata(attr->om, 0, 4, buf);
    evt.status     = 0;
    evt.properties = get_le32(buf);

    ble_ots_client_dispatch_event(conn_handle,
                                  BLE_OTS_CLIENT_EVT_PROPERTIES_READ,
                                  &evt);
    return 0;
}

/*****************************************************************************
 * GATT Write Callback: Metadata Written (generic for all metadata writes)
 *****************************************************************************/

typedef struct {
    uint16_t conn_id;
    ble_ots_client_metadata_type_t metadata_type;
} metadata_write_ctx_t;

static int
metadata_write_cb(uint16_t conn_handle,
                  const struct ble_gatt_error *error,
                  struct ble_gatt_attr *attr,
                  void *arg)
{
    metadata_write_ctx_t *ctx = (metadata_write_ctx_t *)arg;

    ble_ots_client_metadata_written_t evt = {0};
    evt.status        = error->status;
    evt.metadata_type = ctx->metadata_type;

    /* Check if Write Long was rejected with "Request Not Supported" */
    if (error->status == BLE_HS_ATT_ERR(BLE_ATT_ERR_REQ_NOT_SUPPORTED)) {
        ble_ots_client_conn_ctx_t *conn_ctx = ble_ots_client_get_conn_ctx(ctx->conn_id);
        if (conn_ctx) {
            conn_ctx->server_supports_write_long = false;
        }
    }

    ble_ots_client_dispatch_event(ctx->conn_id,
                                  BLE_OTS_CLIENT_EVT_METADATA_WRITTEN,
                                  &evt);
    free(ctx);
    return 0;
}

/*****************************************************************************
 * Public API: Read Object Name
 *****************************************************************************/
int ble_ots_client_read_object_name(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.object_name_handle == 0) {
        return BLE_HS_EINVAL;
    }

    /* Reset name buffer */
    ctx->name_buf_len = 0;
    memset(ctx->name_buf, 0, sizeof(ctx->name_buf));

    int rc = ble_gattc_read(conn_id, ctx->handles.object_name_handle,
                            name_read_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate Object Name read; rc=%d", rc);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Read Object Type
 *****************************************************************************/
int ble_ots_client_read_object_type(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.object_type_handle == 0) {
        return BLE_HS_EINVAL;
    }

    int rc = ble_gattc_read(conn_id, ctx->handles.object_type_handle,
                            type_read_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate Object Type read; rc=%d", rc);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Read Object Size
 *****************************************************************************/
int ble_ots_client_read_object_size(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.object_size_handle == 0) {
        return BLE_HS_EINVAL;
    }

    int rc = ble_gattc_read(conn_id, ctx->handles.object_size_handle,
                            size_read_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate Object Size read; rc=%d", rc);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Read Object First-Created
 *****************************************************************************/
int ble_ots_client_read_object_first_created(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.first_created_handle == 0) {
        ESP_LOGE(TAG, "First-Created characteristic not discovered");
        return BLE_HS_EINVAL;
    }

    int rc = ble_gattc_read(conn_id, ctx->handles.first_created_handle,
                            first_created_read_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate First-Created read; rc=%d", rc);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Read Object Last-Modified
 *****************************************************************************/
int ble_ots_client_read_object_last_modified(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.last_modified_handle == 0) {
        ESP_LOGE(TAG, "Last-Modified characteristic not discovered");
        return BLE_HS_EINVAL;
    }

    int rc = ble_gattc_read(conn_id, ctx->handles.last_modified_handle,
                            last_modified_read_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate Last-Modified read; rc=%d", rc);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Read Object ID
 *****************************************************************************/
int ble_ots_client_read_object_id(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.object_id_handle == 0) {
        ESP_LOGE(TAG, "Object ID characteristic not discovered (single-object server)");
        return BLE_HS_EINVAL;
    }

    int rc = ble_gattc_read(conn_id, ctx->handles.object_id_handle,
                            object_id_read_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate Object ID read; rc=%d", rc);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Read Object Properties
 *****************************************************************************/
int ble_ots_client_read_object_properties(uint16_t conn_id)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.object_properties_handle == 0) {
        return BLE_HS_EINVAL;
    }

    int rc = ble_gattc_read(conn_id, ctx->handles.object_properties_handle,
                            properties_read_cb, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate Object Properties read; rc=%d", rc);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Write Object Name
 *****************************************************************************/
int ble_ots_client_write_object_name(uint16_t conn_id,
                                     const char *name,
                                     uint16_t name_len)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    if (name == NULL || name_len == 0 || name_len > BLE_OTS_OBJECT_NAME_MAX_LEN) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.object_name_handle == 0) {
        return BLE_HS_EINVAL;
    }

    /* Allocate write context */
    metadata_write_ctx_t *wctx = malloc(sizeof(metadata_write_ctx_t));
    if (wctx == NULL) {
        return BLE_HS_ENOMEM;
    }
    wctx->conn_id       = conn_id;
    wctx->metadata_type = BLE_OTS_CLIENT_METADATA_OBJECT_NAME;

    uint16_t att_mtu = ble_att_mtu(conn_id);
    uint16_t max_write_len = (att_mtu > 3) ? (att_mtu - 3) : 0;

    if (name_len <= max_write_len || max_write_len == 0) {
        /* Simple write fits within ATT_MTU - 3 */
        int rc = ble_gattc_write_flat(conn_id,
                                      ctx->handles.object_name_handle,
                                      name, name_len,
                                      metadata_write_cb, wctx);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to initiate Object Name write; rc=%d", rc);
            free(wctx);
        }
        return rc;
    }

    /* Name exceeds ATT_MTU - 3 */
    if (ctx->server_supports_write_long) {
        /* Attempt Write Long */
        struct os_mbuf *om = ble_hs_mbuf_from_flat(name, name_len);
        if (om == NULL) {
            free(wctx);
            return BLE_HS_ENOMEM;
        }

        int rc = ble_gattc_write_long(conn_id,
                                      ctx->handles.object_name_handle,
                                      0, om,
                                      metadata_write_cb, wctx);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to initiate Object Name Write Long; rc=%d", rc);
            free(wctx);
            /* Note: ble_gattc_write_long consumes om regardless of rc */
        }
        return rc;
    }

    /* Server doesn't support Write Long — truncate to ATT_MTU - 3 */
    ESP_LOGW(TAG, "Server doesn't support Write Long, truncating name to %u bytes",
             max_write_len);
    int rc = ble_gattc_write_flat(conn_id,
                                  ctx->handles.object_name_handle,
                                  name, max_write_len,
                                  metadata_write_cb, wctx);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate truncated Object Name write; rc=%d", rc);
        free(wctx);
    }
    return rc;
}

/*****************************************************************************
 * Public API: Write Object First-Created
 *****************************************************************************/
int ble_ots_client_write_object_first_created(uint16_t conn_id,
                                              const ble_ots_date_time_t *datetime)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    if (datetime == NULL) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.first_created_handle == 0) {
        ESP_LOGE(TAG, "First-Created characteristic not discovered");
        return BLE_HS_EINVAL;
    }

    metadata_write_ctx_t *wctx = malloc(sizeof(metadata_write_ctx_t));
    if (wctx == NULL) {
        return BLE_HS_ENOMEM;
    }
    wctx->conn_id       = conn_id;
    wctx->metadata_type = BLE_OTS_CLIENT_METADATA_FIRST_CREATED;

    uint8_t buf[7];
    encode_datetime(datetime, buf);

    int rc = ble_gattc_write_flat(conn_id,
                                  ctx->handles.first_created_handle,
                                  buf, sizeof(buf),
                                  metadata_write_cb, wctx);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate First-Created write; rc=%d", rc);
        free(wctx);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Write Object Last-Modified
 *****************************************************************************/
int ble_ots_client_write_object_last_modified(uint16_t conn_id,
                                              const ble_ots_date_time_t *datetime)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    if (datetime == NULL) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.last_modified_handle == 0) {
        ESP_LOGE(TAG, "Last-Modified characteristic not discovered");
        return BLE_HS_EINVAL;
    }

    metadata_write_ctx_t *wctx = malloc(sizeof(metadata_write_ctx_t));
    if (wctx == NULL) {
        return BLE_HS_ENOMEM;
    }
    wctx->conn_id       = conn_id;
    wctx->metadata_type = BLE_OTS_CLIENT_METADATA_LAST_MODIFIED;

    uint8_t buf[7];
    encode_datetime(datetime, buf);

    int rc = ble_gattc_write_flat(conn_id,
                                  ctx->handles.last_modified_handle,
                                  buf, sizeof(buf),
                                  metadata_write_cb, wctx);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate Last-Modified write; rc=%d", rc);
        free(wctx);
    }

    return rc;
}

/*****************************************************************************
 * Public API: Write Object Properties
 *****************************************************************************/
int ble_ots_client_write_object_properties(uint16_t conn_id,
                                           uint32_t properties)
{
    if (g_ots_client == NULL || !g_ots_client->initialized) {
        return BLE_HS_EINVAL;
    }

    ble_ots_client_conn_ctx_t *ctx = ble_ots_client_get_conn_ctx(conn_id);
    if (ctx == NULL) {
        return BLE_HS_ENOTCONN;
    }

    if (ctx->handles.object_properties_handle == 0) {
        return BLE_HS_EINVAL;
    }

    metadata_write_ctx_t *wctx = malloc(sizeof(metadata_write_ctx_t));
    if (wctx == NULL) {
        return BLE_HS_ENOMEM;
    }
    wctx->conn_id       = conn_id;
    wctx->metadata_type = BLE_OTS_CLIENT_METADATA_PROPERTIES;

    uint8_t buf[4];
    put_le32(buf, properties);

    int rc = ble_gattc_write_flat(conn_id,
                                  ctx->handles.object_properties_handle,
                                  buf, sizeof(buf),
                                  metadata_write_cb, wctx);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to initiate Object Properties write; rc=%d", rc);
        free(wctx);
    }

    return rc;
}
