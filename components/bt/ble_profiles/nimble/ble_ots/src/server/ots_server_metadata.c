/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "host/ble_gap.h"
#include "os/os_mbuf.h"

#include "ble_ots_common.h"
#include "ble_ots_server.h"
#include "ble_ots_server_int.h"

static const char *TAG = "ots_metadata";

/*****************************************************************************
 * Helper: common pre-check for metadata read/write
 *****************************************************************************/

/**
 * @brief Common pre-check for all metadata characteristic access.
 *
 * Checks concurrency, retrieves current object, and looks it up in the DB.
 *
 * @param conn_handle  BLE connection handle
 * @param[out] out_obj Pointer to receive the object pointer
 * @param[out] out_id  Pointer to receive the current object ID
 * @return 0 on success, ATT error code on failure
 */
static int
metadata_precheck(uint16_t conn_handle, ble_ots_server_obj_t **out_obj,
                  ble_ots_obj_id_t *out_id)
{
    /* Check concurrency pool */
    if (!ble_ots_server_concurrency_check(conn_handle)) {
        return BLE_OTS_APP_ERR_CONCURRENCY_LIMIT_EXCEEDED;
    }

    /* Get current object for this connection */
    ble_ots_obj_id_t obj_id = ble_ots_server_current_obj_get(conn_handle);
    if (obj_id == BLE_OTS_OBJ_ID_INVALID) {
        return BLE_OTS_APP_ERR_OBJECT_NOT_SELECTED;
    }

    /* Look up object in database */
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(obj_id);
    if (obj == NULL) {
        return BLE_OTS_APP_ERR_OBJECT_NOT_SELECTED;
    }

    if (out_obj) {
        *out_obj = obj;
    }
    if (out_id) {
        *out_id = obj_id;
    }

    return 0;
}

static bool
ble_ots_obj_name_is_valid(const char *name, uint16_t name_len)
{
    if (name_len == 0) {
        return false;
    }

    for (uint16_t i = 0; i < name_len; i++) {
        unsigned char c = (unsigned char)name[i];
        if (c < 0x20 || c == 0x7F) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Dispatch metadata written event and object changed indication.
 */
static void
metadata_dispatch_write_event(uint16_t conn_handle, ble_ots_obj_id_t object_id,
                              uint16_t char_uuid, const char *name,
                              uint8_t name_len, ble_ots_date_time_t *dt,
                              uint32_t properties)
{
    /* Dispatch application event */
    ble_ots_server_cb_param_t param;
    memset(&param, 0, sizeof(param));
    param.metadata_written.conn_id = conn_handle;
    param.metadata_written.object_id = object_id;
    param.metadata_written.char_uuid = char_uuid;
    if (name) {
        param.metadata_written.name = name;
        param.metadata_written.name_len = name_len;
    }
    if (dt) {
        param.metadata_written.date_time = *dt;
    }
    param.metadata_written.properties = properties;

    ble_ots_server_dispatch_event(BLE_OTS_SERVER_EVT_METADATA_WRITTEN, &param);

    /* Dispatch object changed: Source = Client (bit 0 = 1), Metadata changed (bit 2 = 1) */
    uint8_t flags = BLE_OTS_OBJ_CHANGED_FLAG_SOURCE | BLE_OTS_OBJ_CHANGED_FLAG_METADATA;
    ble_ots_server_dispatch_obj_changed(object_id, flags, conn_handle);
}

/*****************************************************************************
 * OTS Feature Characteristic (UUID 0x2ABD)
 *****************************************************************************/

static int
ble_ots_server_feature_access(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int rc;

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        /* Return 8-byte OTS Feature: OACP Features (4) + OLCP Features (4) */
        uint8_t buf[8];
        uint32_t oacp = ble_ots_server_env.ots_feature.oacp_features;
        uint32_t olcp = ble_ots_server_env.ots_feature.olcp_features;

        /* Little-endian encoding */
        buf[0] = (uint8_t)(oacp & 0xFF);
        buf[1] = (uint8_t)((oacp >> 8) & 0xFF);
        buf[2] = (uint8_t)((oacp >> 16) & 0xFF);
        buf[3] = (uint8_t)((oacp >> 24) & 0xFF);
        buf[4] = (uint8_t)(olcp & 0xFF);
        buf[5] = (uint8_t)((olcp >> 8) & 0xFF);
        buf[6] = (uint8_t)((olcp >> 16) & 0xFF);
        buf[7] = (uint8_t)((olcp >> 24) & 0xFF);

        rc = os_mbuf_append(ctxt->om, buf, sizeof(buf));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    /* Not writable */
    return BLE_ATT_ERR_WRITE_NOT_PERMITTED;
}

/*****************************************************************************
 * Object Name Characteristic (UUID 0x2ABE)
 *****************************************************************************/

static int
ble_ots_server_obj_name_access(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ble_ots_server_obj_t *obj = NULL;
    ble_ots_obj_id_t obj_id = 0;
    int rc;

    rc = metadata_precheck(conn_handle, &obj, &obj_id);
    if (rc != 0) {
        return rc;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        /* Return the object name as UTF-8 string */
        rc = os_mbuf_append(ctxt->om, obj->name, obj->name_len);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        /* Extract the written value */
        char name_buf[BLE_OTS_OBJECT_NAME_MAX_LEN];
        uint16_t name_len = 0;
        uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);

        /* Validate name length */
        if (om_len > BLE_OTS_OBJECT_NAME_MAX_LEN) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }

        rc = ble_hs_mbuf_to_flat(ctxt->om, name_buf, sizeof(name_buf), &name_len);
        if (rc != 0) {
            return BLE_ATT_ERR_UNLIKELY;
        }

        /*
         * Per OTP spec 4.5.5.1, the name shall not be a zero length string and
         * shall not include any ASCII control characters.
         */
        if (!ble_ots_obj_name_is_valid(name_buf, name_len)) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }

        /* Check uniqueness: the new name must not belong to another object */
        if (!ble_ots_server_name_is_unique(name_buf, (uint8_t)name_len, obj_id)) {
            return BLE_OTS_APP_ERR_OBJECT_NAME_ALREADY_EXISTS;
        }

        /* Update the object name */
        memcpy(obj->name, name_buf, name_len);
        obj->name_len = (uint8_t)name_len;

        /* Dispatch events */
        metadata_dispatch_write_event(conn_handle, obj_id,
                                      BLE_OTS_UUID_OBJECT_NAME,
                                      obj->name, obj->name_len,
                                      NULL, 0);
        return 0;
    }

    return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
}

/*****************************************************************************
 * Object Type Characteristic (UUID 0x2ABF)
 *****************************************************************************/

static int
ble_ots_server_obj_type_access(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ble_ots_server_obj_t *obj = NULL;
    int rc;

    rc = metadata_precheck(conn_handle, &obj, NULL);
    if (rc != 0) {
        return rc;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        /* Return the type UUID: 2 octets for 16-bit, 16 octets for 128-bit */
        rc = os_mbuf_append(ctxt->om, obj->type_uuid, obj->type_uuid_len);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    /* Not writable */
    return BLE_ATT_ERR_WRITE_NOT_PERMITTED;
}

/*****************************************************************************
 * Object Size Characteristic (UUID 0x2AC0)
 *****************************************************************************/

static int
ble_ots_server_obj_size_access(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ble_ots_server_obj_t *obj = NULL;
    int rc;

    rc = metadata_precheck(conn_handle, &obj, NULL);
    if (rc != 0) {
        return rc;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        /* Return 8 octets: Current Size (UINT32 LE) + Allocated Size (UINT32 LE) */
        uint8_t buf[8];
        uint32_t cs = obj->current_size;
        uint32_t as = obj->allocated_size;

        buf[0] = (uint8_t)(cs & 0xFF);
        buf[1] = (uint8_t)((cs >> 8) & 0xFF);
        buf[2] = (uint8_t)((cs >> 16) & 0xFF);
        buf[3] = (uint8_t)((cs >> 24) & 0xFF);
        buf[4] = (uint8_t)(as & 0xFF);
        buf[5] = (uint8_t)((as >> 8) & 0xFF);
        buf[6] = (uint8_t)((as >> 16) & 0xFF);
        buf[7] = (uint8_t)((as >> 24) & 0xFF);

        rc = os_mbuf_append(ctxt->om, buf, sizeof(buf));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    /* Not writable */
    return BLE_ATT_ERR_WRITE_NOT_PERMITTED;
}

/*****************************************************************************
 * Object First-Created Characteristic (UUID 0x2AC1)
 *****************************************************************************/

/**
 * @brief Encode a ble_ots_date_time_t into a 7-byte little-endian buffer.
 */
static void
encode_date_time(const ble_ots_date_time_t *dt, uint8_t *buf)
{
    buf[0] = (uint8_t)(dt->year & 0xFF);
    buf[1] = (uint8_t)((dt->year >> 8) & 0xFF);
    buf[2] = dt->month;
    buf[3] = dt->day;
    buf[4] = dt->hours;
    buf[5] = dt->minutes;
    buf[6] = dt->seconds;
}

/**
 * @brief Decode a 7-byte little-endian buffer into a ble_ots_date_time_t.
 */
static void
decode_date_time(const uint8_t *buf, ble_ots_date_time_t *dt)
{
    dt->year = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
    dt->month = buf[2];
    dt->day = buf[3];
    dt->hours = buf[4];
    dt->minutes = buf[5];
    dt->seconds = buf[6];
}

/**
 * @brief Validate a decoded ble_ots_date_time_t per the BLE OTS specification.
 *
 * Valid ranges: year 1582–9999 or 0, month 1–12 or 0, day 1–31 or 0,
 * hours 0–23, minutes 0–59, seconds 0–59.
 *
 * A year of 0 denotes the special "unknown" timestamp (e.g. a newly created
 * object); in that case the month, day, hours, minutes and seconds MUST all be
 * zero. A non-zero month/day/time alongside a zero year is not a valid value.
 *
 * @param dt Pointer to the date-time structure to validate
 * @return true if all fields are within valid ranges, false otherwise
 */
static bool
validate_date_time(const ble_ots_date_time_t *dt)
{
    if (dt->year == 0) {
        /* Unknown timestamp: every other field must also be zero */
        return dt->month == 0 && dt->day == 0 && dt->hours == 0 &&
               dt->minutes == 0 && dt->seconds == 0;
    }
    if (dt->year < 1582 || dt->year > 9999) {
        return false;
    }
    if (dt->month > 12) {
        return false;
    }
    if (dt->day > 31) {
        return false;
    }
    if (dt->hours > 23) {
        return false;
    }
    if (dt->minutes > 59) {
        return false;
    }
    if (dt->seconds > 59) {
        return false;
    }
    return true;
}

static int
ble_ots_server_obj_first_created_access(uint16_t conn_handle, uint16_t attr_handle,
                                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ble_ots_server_obj_t *obj = NULL;
    ble_ots_obj_id_t obj_id = 0;
    int rc;

    rc = metadata_precheck(conn_handle, &obj, &obj_id);
    if (rc != 0) {
        return rc;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        uint8_t buf[7];
        encode_date_time(&obj->first_created, buf);
        rc = os_mbuf_append(ctxt->om, buf, sizeof(buf));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        uint8_t buf[7];
        uint16_t len = 0;
        uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);

        if (om_len != 7) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }

        rc = ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &len);
        if (rc != 0) {
            return BLE_ATT_ERR_UNLIKELY;
        }

        ble_ots_date_time_t new_dt;
        decode_date_time(buf, &new_dt);

        /* Validate date-time fields per OTS specification */
        if (!validate_date_time(&new_dt)) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }

        /* Update the object's first-created timestamp */
        obj->first_created = new_dt;

        /* Dispatch events */
        metadata_dispatch_write_event(conn_handle, obj_id,
                                      BLE_OTS_UUID_OBJECT_FIRST_CREATED,
                                      NULL, 0, &new_dt, 0);
        return 0;
    }

    return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
}

/*****************************************************************************
 * Object Last-Modified Characteristic (UUID 0x2AC2)
 *****************************************************************************/

static int
ble_ots_server_obj_last_modified_access(uint16_t conn_handle, uint16_t attr_handle,
                                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ble_ots_server_obj_t *obj = NULL;
    ble_ots_obj_id_t obj_id = 0;
    int rc;

    rc = metadata_precheck(conn_handle, &obj, &obj_id);
    if (rc != 0) {
        return rc;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        uint8_t buf[7];
        encode_date_time(&obj->last_modified, buf);
        rc = os_mbuf_append(ctxt->om, buf, sizeof(buf));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        /* Writable only when server does NOT have a real-time clock */
        if (ble_ots_server_env.config.has_realtime_clock) {
            return BLE_ATT_ERR_WRITE_NOT_PERMITTED;
        }

        uint8_t buf[7];
        uint16_t len = 0;
        uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);

        if (om_len != 7) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }

        rc = ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &len);
        if (rc != 0) {
            return BLE_ATT_ERR_UNLIKELY;
        }

        ble_ots_date_time_t new_dt;
        decode_date_time(buf, &new_dt);

        /* Validate date-time fields per OTS specification */
        if (!validate_date_time(&new_dt)) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }

        /* Update the object's last-modified timestamp */
        obj->last_modified = new_dt;

        /* Dispatch events */
        metadata_dispatch_write_event(conn_handle, obj_id,
                                      BLE_OTS_UUID_OBJECT_LAST_MODIFIED,
                                      NULL, 0, &new_dt, 0);
        return 0;
    }

    return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
}

/*****************************************************************************
 * Object ID Characteristic (UUID 0x2AC3)
 *****************************************************************************/

static int
ble_ots_server_obj_id_access(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ble_ots_server_obj_t *obj = NULL;
    int rc;

    rc = metadata_precheck(conn_handle, &obj, NULL);
    if (rc != 0) {
        return rc;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        /* Return 6-byte UINT48 Object ID in little-endian */
        uint8_t buf[6];
        uint64_t id = obj->object_id;

        buf[0] = (uint8_t)(id & 0xFF);
        buf[1] = (uint8_t)((id >> 8) & 0xFF);
        buf[2] = (uint8_t)((id >> 16) & 0xFF);
        buf[3] = (uint8_t)((id >> 24) & 0xFF);
        buf[4] = (uint8_t)((id >> 32) & 0xFF);
        buf[5] = (uint8_t)((id >> 40) & 0xFF);

        rc = os_mbuf_append(ctxt->om, buf, sizeof(buf));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    /* Not writable */
    return BLE_ATT_ERR_WRITE_NOT_PERMITTED;
}

/*****************************************************************************
 * Object Properties Characteristic (UUID 0x2AC4)
 *****************************************************************************/

static int
ble_ots_server_obj_properties_access(uint16_t conn_handle, uint16_t attr_handle,
                                     struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ble_ots_server_obj_t *obj = NULL;
    ble_ots_obj_id_t obj_id = 0;
    int rc;

    rc = metadata_precheck(conn_handle, &obj, &obj_id);
    if (rc != 0) {
        return rc;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        /* Return 4-byte properties bit-field in little-endian */
        uint8_t buf[4];
        uint32_t props = obj->properties;

        buf[0] = (uint8_t)(props & 0xFF);
        buf[1] = (uint8_t)((props >> 8) & 0xFF);
        buf[2] = (uint8_t)((props >> 16) & 0xFF);
        buf[3] = (uint8_t)((props >> 24) & 0xFF);

        rc = os_mbuf_append(ctxt->om, buf, sizeof(buf));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        uint8_t buf[4];
        uint16_t len = 0;
        uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);

        if (om_len != 4) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }

        rc = ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &len);
        if (rc != 0) {
            return BLE_ATT_ERR_UNLIKELY;
        }

        uint32_t new_props = (uint32_t)buf[0] |
                             ((uint32_t)buf[1] << 8) |
                             ((uint32_t)buf[2] << 16) |
                             ((uint32_t)buf[3] << 24);

        /* Reject if any RFU bits (8–31) are set */
        if (new_props & 0xFFFFFF00) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }

        /* Update the object's properties */
        obj->properties = new_props;

        /* Dispatch events */
        metadata_dispatch_write_event(conn_handle, obj_id,
                                      BLE_OTS_UUID_OBJECT_PROPERTIES,
                                      NULL, 0, NULL, new_props);
        return 0;
    }

    return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
}

/*****************************************************************************
 * Metadata Read/Write Dispatchers — called from ots_server_init.c
 *****************************************************************************/

int
ble_ots_server_metadata_read(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt)
{
    const ble_uuid_t *uuid = ctxt->chr->uuid;
    if (uuid->type != BLE_UUID_TYPE_16) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    uint16_t uuid16 = BLE_UUID16(uuid)->value;

    switch (uuid16) {
    case BLE_OTS_UUID_OTS_FEATURE:
        return ble_ots_server_feature_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_NAME:
        return ble_ots_server_obj_name_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_TYPE:
        return ble_ots_server_obj_type_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_SIZE:
        return ble_ots_server_obj_size_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_FIRST_CREATED:
        return ble_ots_server_obj_first_created_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_LAST_MODIFIED:
        return ble_ots_server_obj_last_modified_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_ID:
        return ble_ots_server_obj_id_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_PROPERTIES:
        return ble_ots_server_obj_properties_access(conn_handle, attr_handle, ctxt, NULL);
    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

int
ble_ots_server_metadata_write(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt)
{
    const ble_uuid_t *uuid = ctxt->chr->uuid;
    if (uuid->type != BLE_UUID_TYPE_16) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    uint16_t uuid16 = BLE_UUID16(uuid)->value;

    /* The Directory Listing Object is read-only: its metadata is generated by
     * the server from the object database, so a client must not be able to
     * overwrite it while the DLO is its Current Object. When no object is
     * selected the ID is BLE_OTS_OBJ_ID_INVALID, which falls through to the
     * access handlers so that Object Not Selected keeps being reported. */
    if (ble_ots_server_current_obj_get(conn_handle) ==
        BLE_OTS_OBJ_ID_DIRECTORY_LISTING) {
        return BLE_ATT_ERR_WRITE_NOT_PERMITTED;
    }

    switch (uuid16) {
    case BLE_OTS_UUID_OBJECT_NAME:
        return ble_ots_server_obj_name_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_FIRST_CREATED:
        return ble_ots_server_obj_first_created_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_LAST_MODIFIED:
        return ble_ots_server_obj_last_modified_access(conn_handle, attr_handle, ctxt, NULL);
    case BLE_OTS_UUID_OBJECT_PROPERTIES:
        return ble_ots_server_obj_properties_access(conn_handle, attr_handle, ctxt, NULL);
    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

/*****************************************************************************
 * Public APIs — Server-Initiated Object Management
 *****************************************************************************/

/**
 * @brief Create the object and populate its metadata.
 *
 * Caller must hold the OTS server mutex: the uniqueness check, the slot
 * allocation and the metadata/content writes below form one atomic operation
 * with respect to the NimBLE host task.
 *
 * @param params     Object parameters supplied by the application
 * @param out_obj_id Receives the allocated Object ID on success
 * @return 0 on success, negative errno on failure
 */
static int ots_add_object_locked(const ble_ots_server_obj_params_t *params,
                                 ble_ots_obj_id_t *out_obj_id)
{
    /* Validate name length */
    if (params->name_len > BLE_OTS_OBJECT_NAME_MAX_LEN) {
        return -EINVAL;
    }

    /* Reject a length without a buffer, as ble_ots_server_set_object_data does,
     * instead of silently creating an object with no content */
    if (params->data == NULL && params->data_len > 0) {
        return -EINVAL;
    }

    /* Validate allocated_size >= data_len */
    uint32_t alloc_size = params->allocated_size;
    if (alloc_size == 0 && params->data_len > 0) {
        alloc_size = params->data_len;
    }
    if (alloc_size < params->data_len) {
        return -EINVAL;
    }

    /* Validate type UUID length */
    if (params->type.uuid_len != 2 && params->type.uuid_len != 16) {
        return -EINVAL;
    }

    /* Check name uniqueness */
    const char *name = params->name;
    uint8_t name_len = name ? params->name_len : 0;

    /* Only named objects take part in the uniqueness check: an object created
     * over OACP starts with a zero-length name and the Create procedure has no
     * result code for a name clash, so several unnamed objects must be able to
     * coexist until the client assigns each one a name. */
    if (name_len > 0) {
        if (!ble_ots_server_name_is_unique(name, name_len, BLE_OTS_OBJ_ID_INVALID)) {
            return -EEXIST;
        }
    }

    /* Create object in database (allocates Object ID) */
    ble_ots_obj_id_t new_id = ble_ots_server_obj_db_create(&params->type, alloc_size);
    if (new_id == 0) {
        return -ENOMEM;
    }

    /* Look up the newly created object to set metadata */
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(new_id);
    if (obj == NULL) {
        return -ENOMEM;
    }

    /* Set name */
    if (name_len > 0) {
        memcpy(obj->name, name, name_len);
    }
    obj->name_len = name_len;

    /* Set properties */
    obj->properties = params->properties;

    /* Set timestamps */
    obj->first_created = params->first_created;
    obj->last_modified = params->last_modified;

    /* Set initial data if provided */
    if (params->data != NULL && params->data_len > 0) {
        int rc = ble_ots_server_obj_data_write(new_id, 0, params->data, params->data_len);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to write initial object data, rc=%d", rc);
            ble_ots_server_obj_db_delete(new_id);
            return (rc == BLE_HS_ENOMEM) ? -ENOMEM :
                   (rc == BLE_HS_ENOENT) ? -ENOENT : -EINVAL;
        }
        obj->current_size = params->data_len;
    } else {
        obj->current_size = 0;
    }

    obj->allocated_size = alloc_size;

    /* Output the allocated Object ID */
    *out_obj_id = new_id;

    ESP_LOGI(TAG, "Object added: ID=0x%012llX name_len=%u",
             (unsigned long long)new_id, name_len);

    return 0;
}

int
ble_ots_server_add_object(const ble_ots_server_obj_params_t *params,
                          ble_ots_obj_id_t *out_obj_id)
{
    ble_ots_obj_id_t new_id = BLE_OTS_OBJ_ID_INVALID;
    int rc;

    if (!BLE_OTS_SERVER_ENV_OK()) {
        return -EPERM;
    }

    if (params == NULL) {
        return -EINVAL;
    }

    ble_ots_server_lock();
    rc = ots_add_object_locked(params, &new_id);
    ble_ots_server_unlock();

    if (rc != 0) {
        return rc;
    }

    if (out_obj_id != NULL) {
        *out_obj_id = new_id;
    }

    /* Dispatch Object Changed: Creation (bit 3), Source = Server (bit 0 = 0).
     * Done outside the lock — it queues indications and reaches NimBLE. */
    ble_ots_server_dispatch_obj_changed(new_id,
                                        BLE_OTS_OBJ_CHANGED_FLAG_CREATION,
                                        BLE_OTS_CONN_ID_NONE);

    return 0;
}

int
ble_ots_server_remove_object(ble_ots_obj_id_t object_id)
{
    if (!BLE_OTS_SERVER_ENV_OK()) {
        return -EPERM;
    }

    /* DLO cannot be removed */
    if (object_id == BLE_OTS_OBJ_ID_DIRECTORY_LISTING) {
        return -EINVAL;
    }

    ble_ots_server_lock();

    /* Look up the object */
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (obj == NULL) {
        ble_ots_server_unlock();
        return -ENOENT;
    }

    /* Invalidate current object for any connection that has this object selected */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[i];
        if (cs->in_pool && cs->current_obj_valid &&
            cs->current_object_id == object_id) {
            cs->current_obj_valid = false;
            cs->current_object_id = BLE_OTS_OBJ_ID_INVALID;
            ESP_LOGW(TAG, "Invalidated current object for conn=%u due to removal",
                     cs->conn_id);
        }
    }

    ble_ots_server_unlock();

    /* Delete the object from the database. Takes the lock itself and then
     * aborts any transfer on the object without it, since that reaches the
     * application callback. */
    int rc = ble_ots_server_obj_db_delete(object_id);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to delete object 0x%012llX, rc=%d",
                 (unsigned long long)object_id, rc);
        return (rc == BLE_HS_ENOENT) ? -ENOENT :
               (rc == BLE_HS_EINVAL) ? -EINVAL : -EINVAL;
    }

    ESP_LOGI(TAG, "Object removed: ID=0x%012llX", (unsigned long long)object_id);

    /* Dispatch Object Changed: Deletion (bit 4), Source = Server (bit 0 = 0) */
    ble_ots_server_dispatch_obj_changed(object_id,
                                        BLE_OTS_OBJ_CHANGED_FLAG_DELETION,
                                        BLE_OTS_CONN_ID_NONE);

    return 0;
}

int
ble_ots_server_set_object_data(ble_ots_obj_id_t object_id,
                               const uint8_t *data,
                               uint32_t offset,
                               uint32_t length)
{
    if (!BLE_OTS_SERVER_ENV_OK()) {
        return -EPERM;
    }

    if (data == NULL && length > 0) {
        return -EINVAL;
    }

    /* Validate that offset + length does not overflow uint32_t */
    if (length > 0 && offset > UINT32_MAX - length) {
        return -EINVAL;
    }

    ble_ots_server_lock();

    /* Look up the object */
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (obj == NULL) {
        ble_ots_server_unlock();
        return -ENOENT;
    }

    /* Write data at the specified offset */
    int rc = ble_ots_server_obj_data_write(object_id, offset, data, length);
    if (rc != 0) {
        ble_ots_server_unlock();
        ESP_LOGE(TAG, "Failed to write object data for 0x%012llX, rc=%d",
                 (unsigned long long)object_id, rc);
        return (rc == BLE_HS_ENOMEM) ? -ENOMEM :
               (rc == BLE_HS_ENOENT) ? -ENOENT : -EINVAL;
    }

    /* If server has a real-time clock, update Last-Modified timestamp.
     * Note: In a real implementation this would use the actual RTC time.
     * Since we may not have a real RTC API available, we reset to zero
     * if no real-time clock, or set a placeholder if we do. */
    if (ble_ots_server_env.config.has_realtime_clock) {
        /* In a production system, this would call an RTC API to get current UTC.
         * For now, we leave the timestamp as-is since we don't have a
         * portable RTC interface. The application can update it via the
         * metadata write event or directly. */
    }

    ESP_LOGD(TAG, "Object data set: ID=0x%012llX offset=%lu len=%lu cur_size=%lu",
             (unsigned long long)object_id,
             (unsigned long)offset, (unsigned long)length,
             (unsigned long)obj->current_size);

    ble_ots_server_unlock();

    /* Dispatch Object Changed: Content changed (bit 1), Source = Server (bit 0 = 0) */
    ble_ots_server_dispatch_obj_changed(object_id,
                                        BLE_OTS_OBJ_CHANGED_FLAG_CONTENT,
                                        BLE_OTS_CONN_ID_NONE);

    return 0;
}
