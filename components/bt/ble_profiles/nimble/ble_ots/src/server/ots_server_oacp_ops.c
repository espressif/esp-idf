/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "host/ble_gap.h"
#include "host/ble_l2cap.h"
#include "os/os_mbuf.h"
#include "ble_ots_common.h"
#include "ble_ots_server.h"
#include "ble_ots_server_int.h"

static const char *TAG = "ots_oacp_ops";

/*****************************************************************************
 * Helper: check if OACP CCCD is configured for indications
 *****************************************************************************/
static bool oacp_cccd_configured(uint16_t conn_id)
{
    uint16_t chr_val_handle = ble_ots_server_oacp_handle_get();
    if (chr_val_handle == 0) {
        return false;
    }

    /* Check the tracked OACP CCCD subscription state for this connection.
     * The oacp_subscribed flag is updated via BLE_GAP_EVENT_SUBSCRIBE in the
     * GAP event callback (ots_server_init.c). */
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (cs == NULL) {
        return false;
    }

    return cs->oacp_subscribed;
}

/*****************************************************************************
 * Helper: send OACP response indication
 *****************************************************************************/
static int oacp_send_response(uint16_t conn_id, uint8_t request_opcode,
                              uint8_t result_code,
                              const uint8_t *resp_param, uint8_t resp_param_len)
{
    uint8_t buf[3 + 4]; /* response opcode + request opcode + result code + up to 4 bytes param */
    uint16_t len = 0;

    buf[len++] = BLE_OTS_OACP_OPCODE_RESPONSE; /* 0x60 */
    buf[len++] = request_opcode;
    buf[len++] = result_code;

    if (resp_param != NULL && resp_param_len > 0) {
        memcpy(&buf[len], resp_param, resp_param_len);
        len += resp_param_len;
    }

    return ble_ots_server_indicate_response(conn_id, BLE_OTS_UUID_OACP, buf, len);
}

/*****************************************************************************
 * Helper: check if object type is in supported_types list
 *****************************************************************************/
static bool type_is_supported(const ble_ots_obj_type_entry_t *type)
{
    const ble_ots_server_config_t *cfg = &ble_ots_server_env.config;

    if (cfg->supported_types == NULL || cfg->num_supported_types == 0) {
        /* No type restriction — all types accepted */
        return true;
    }

    for (uint8_t i = 0; i < cfg->num_supported_types; i++) {
        const ble_ots_obj_type_entry_t *entry = &cfg->supported_types[i];
        if (entry->uuid_len == type->uuid_len &&
            memcmp(entry->uuid, type->uuid, type->uuid_len) == 0) {
            return true;
        }
    }

    return false;
}

/*****************************************************************************
 * Helper: check if an object is locked or has an active transfer
 *****************************************************************************/
static bool object_is_locked_or_transferring(ble_ots_obj_id_t object_id,
                                             uint16_t conn_id)
{
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (obj == NULL) {
        return false;
    }

    /* Check if locked by another connection */
    if (obj->locked_by != BLE_OTS_CONN_ID_NONE && obj->locked_by != conn_id) {
        return true;
    }

    /* Check if any connection has an active transfer on this object */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[i];
        if (cs->in_pool && cs->transfer_active &&
            cs->transfer_object_id == object_id) {
            return true;
        }
    }

    return false;
}

/*****************************************************************************
 * OACP Create Procedure
 *****************************************************************************/
static void oacp_create_proc(uint16_t conn_id, const uint8_t *param,
                             uint16_t param_len)
{
    uint8_t opcode = BLE_OTS_OACP_OPCODE_CREATE;

    /* Priority 1: Check if Create is supported */
    if (!(ble_ots_server_env.config.oacp_features & BLE_OTS_OACP_FEAT_CREATE)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED, NULL, 0);
        return;
    }

    /* Parse parameters: Size (UINT32) + Type (2 or 16 octets UUID) */
    if (param_len < 6) { /* 4 bytes size + at least 2 bytes UUID */
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER, NULL, 0);
        return;
    }

    uint32_t size = (uint32_t)param[0] |
                    ((uint32_t)param[1] << 8) |
                    ((uint32_t)param[2] << 16) |
                    ((uint32_t)param[3] << 24);

    uint16_t uuid_len = param_len - 4;
    if (uuid_len != 2 && uuid_len != 16) {
        /* Priority 4: Invalid Parameter — UUID must be 2 or 16 octets */
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER, NULL, 0);
        return;
    }

    ble_ots_obj_type_entry_t type;
    memset(&type, 0, sizeof(type));
    type.uuid_len = (uint8_t)uuid_len;
    memcpy(type.uuid, &param[4], uuid_len);

    /* Priority 2: Check if type is supported */
    if (!type_is_supported(&type)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_UNSUPPORTED_TYPE, NULL, 0);
        return;
    }

    /* Priority 3: Create object in database (checks resources internally) */
    ble_ots_obj_id_t new_id = ble_ots_server_obj_db_create(&type, size);
    if (new_id == 0) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INSUFFICIENT_RESOURCES, NULL, 0);
        return;
    }

    /* Reset filters for this connection if Object List Filter is supported */
    if (ble_ots_server_env.config.include_obj_list_filter) {
        ble_ots_server_filter_reset(conn_id);
    }

    /* Set newly created object as Current Object */
    int rc = ble_ots_server_current_obj_set(conn_id, new_id);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set current object after create, rc=%d", rc);
        /* Attempt to clean up */
        ble_ots_server_obj_db_delete(new_id);
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OPERATION_FAILED, NULL, 0);
        return;
    }

    /* Track created object for post-disconnect cleanup */
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (cs != NULL && cs->created_obj_count < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS) {
        cs->created_obj_ids[cs->created_obj_count++] = new_id;
    }

    /* Send success indication */
    oacp_send_response(conn_id, opcode,
                       BLE_OTS_OACP_RESULT_SUCCESS, NULL, 0);

    /* Dispatch EVT_OBJECT_CREATED to application */
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(new_id);
    ble_ots_server_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    cb_param.object_created.object_id = new_id;
    cb_param.object_created.type = type;
    cb_param.object_created.allocated_size = (obj != NULL) ? obj->allocated_size : size;
    ble_ots_server_dispatch_event(BLE_OTS_SERVER_EVT_OBJECT_CREATED, &cb_param);

    /* Dispatch Object Changed indication with Creation flag + Source=Client */
    uint8_t flags = BLE_OTS_OBJ_CHANGED_FLAG_CREATION | BLE_OTS_OBJ_CHANGED_FLAG_SOURCE;
    ble_ots_server_dispatch_obj_changed(new_id, flags, conn_id);
}

/*****************************************************************************
 * OACP Delete Procedure
 *****************************************************************************/
static void oacp_delete_proc(uint16_t conn_id)
{
    uint8_t opcode = BLE_OTS_OACP_OPCODE_DELETE;

    /* Priority 1: Check if Delete is supported */
    if (!(ble_ots_server_env.config.oacp_features & BLE_OTS_OACP_FEAT_DELETE)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED, NULL, 0);
        return;
    }

    /* Priority 2: Check if Current Object is valid */
    ble_ots_obj_id_t obj_id = ble_ots_server_current_obj_get(conn_id);
    if (obj_id == BLE_OTS_OBJ_ID_INVALID) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT, NULL, 0);
        return;
    }

    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(obj_id);
    if (obj == NULL) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT, NULL, 0);
        return;
    }

    /* Priority 3: Check Delete property bit */
    if (!(obj->properties & BLE_OTS_OBJ_PROP_DELETE)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED, NULL, 0);
        return;
    }

    /* Priority 4 & 5: Check if locked or transfer in progress */
    if (obj->locked_by != BLE_OTS_CONN_ID_NONE) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OBJECT_LOCKED, NULL, 0);
        return;
    }

    /* Check if any transfer is active on this object */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[i];
        if (cs->in_pool && cs->transfer_active &&
            cs->transfer_object_id == obj_id) {
            oacp_send_response(conn_id, opcode,
                               BLE_OTS_OACP_RESULT_OBJECT_LOCKED, NULL, 0);
            return;
        }
    }

    /* Delete the object from the database */
    int rc = ble_ots_server_obj_db_delete(obj_id);
    if (rc != 0) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OPERATION_FAILED, NULL, 0);
        return;
    }

    /* Remove deleted object from all connections' created_obj_ids tracking */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[i];
        if (!cs->in_pool) {
            continue;
        }
        for (int j = 0; j < cs->created_obj_count; j++) {
            if (cs->created_obj_ids[j] == obj_id) {
                /* Shift remaining entries left */
                for (int k = j; k < cs->created_obj_count - 1; k++) {
                    cs->created_obj_ids[k] = cs->created_obj_ids[k + 1];
                }
                cs->created_obj_count--;
                break;
            }
        }
    }

    /* Invalidate Current Object for all connections that had this object selected */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[i];
        if (cs->in_pool && cs->current_obj_valid &&
            cs->current_object_id == obj_id) {
            cs->current_obj_valid = false;
            cs->current_object_id = BLE_OTS_OBJ_ID_INVALID;
        }
    }

    /* Send success indication */
    oacp_send_response(conn_id, opcode,
                       BLE_OTS_OACP_RESULT_SUCCESS, NULL, 0);

    /* Dispatch EVT_OBJECT_DELETED to application */
    ble_ots_server_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    cb_param.object_deleted.object_id = obj_id;
    ble_ots_server_dispatch_event(BLE_OTS_SERVER_EVT_OBJECT_DELETED, &cb_param);

    /* Dispatch Object Changed indication with Deletion flag + Source=Client */
    uint8_t flags = BLE_OTS_OBJ_CHANGED_FLAG_DELETION | BLE_OTS_OBJ_CHANGED_FLAG_SOURCE;
    ble_ots_server_dispatch_obj_changed(obj_id, flags, conn_id);
}

/*****************************************************************************
 * OACP Calculate Checksum Procedure
 *****************************************************************************/
static void oacp_calculate_checksum_proc(uint16_t conn_id, const uint8_t *param,
                                         uint16_t param_len)
{
    uint8_t opcode = BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM;

    /* Priority 1: Check if Calculate Checksum is supported */
    if (!(ble_ots_server_env.config.oacp_features & BLE_OTS_OACP_FEAT_CALCULATE_CHECKSUM)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED, NULL, 0);
        return;
    }

    /* Priority 2: Check if Current Object is valid */
    ble_ots_obj_id_t obj_id = ble_ots_server_current_obj_get(conn_id);
    if (obj_id == BLE_OTS_OBJ_ID_INVALID) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT, NULL, 0);
        return;
    }

    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(obj_id);
    if (obj == NULL) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT, NULL, 0);
        return;
    }

    /* Parse parameters: Offset (UINT32) + Length (UINT32) = 8 bytes */
    if (param_len != 8) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER, NULL, 0);
        return;
    }

    uint32_t offset = (uint32_t)param[0] |
                      ((uint32_t)param[1] << 8) |
                      ((uint32_t)param[2] << 16) |
                      ((uint32_t)param[3] << 24);

    uint32_t length = (uint32_t)param[4] |
                      ((uint32_t)param[5] << 8) |
                      ((uint32_t)param[6] << 16) |
                      ((uint32_t)param[7] << 24);

    /* Priority 3: The checksum range must be a non-empty range inside the
     * object contents. A zero Length, or an Offset at or past the end of the
     * object, selects no octets at all — same rule the Read procedure applies
     * (see ble_ots_server_oacp_read_proc). */
    if (length == 0 || offset >= obj->current_size ||
        (uint64_t)offset + (uint64_t)length > obj->current_size) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER, NULL, 0);
        return;
    }

    /* Priority 4 & 5: Check if locked by another connection or transfer in progress */
    if (object_is_locked_or_transferring(obj_id, conn_id)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OBJECT_LOCKED, NULL, 0);
        return;
    }

    /* Compute CRC-32 directly from the resident object data buffer */
    if (obj->data == NULL) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OPERATION_FAILED, NULL, 0);
        return;
    }

    /* Object data is already in memory — compute checksum without copying */
    uint32_t checksum = ble_ots_checksum_calculate(obj->data, offset, length);

    /* Build response parameter: Checksum (UINT32, little-endian) */
    uint8_t checksum_param[4];
    checksum_param[0] = (uint8_t)(checksum & 0xFF);
    checksum_param[1] = (uint8_t)((checksum >> 8) & 0xFF);
    checksum_param[2] = (uint8_t)((checksum >> 16) & 0xFF);
    checksum_param[3] = (uint8_t)((checksum >> 24) & 0xFF);

    /* Send success indication with checksum */
    oacp_send_response(conn_id, opcode,
                       BLE_OTS_OACP_RESULT_SUCCESS, checksum_param, 4);

    /* Dispatch EVT_CHECKSUM_REQUEST to application */
    ble_ots_server_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    cb_param.checksum.object_id = obj_id;
    cb_param.checksum.offset = offset;
    cb_param.checksum.length = length;
    cb_param.checksum.checksum = checksum;
    ble_ots_server_dispatch_event(BLE_OTS_SERVER_EVT_CHECKSUM_REQUEST, &cb_param);
}

/*****************************************************************************
 * OACP Execute Procedure
 *****************************************************************************/
static void oacp_execute_proc(uint16_t conn_id, const uint8_t *param,
                              uint16_t param_len)
{
    uint8_t opcode = BLE_OTS_OACP_OPCODE_EXECUTE;

    /* Priority 1: Check if Execute is supported */
    if (!(ble_ots_server_env.config.oacp_features & BLE_OTS_OACP_FEAT_EXECUTE)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED, NULL, 0);
        return;
    }

    /* Priority 2: Check if Current Object is valid */
    ble_ots_obj_id_t obj_id = ble_ots_server_current_obj_get(conn_id);
    if (obj_id == BLE_OTS_OBJ_ID_INVALID) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT, NULL, 0);
        return;
    }

    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(obj_id);
    if (obj == NULL) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_OBJECT, NULL, 0);
        return;
    }

    /* Priority 3: Check Execute property bit */
    if (!(obj->properties & BLE_OTS_OBJ_PROP_EXECUTE)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_PROCEDURE_NOT_PERMITTED, NULL, 0);
        return;
    }

    /* Priority 4: Check if locked by another connection or transfer in progress */
    if (object_is_locked_or_transferring(obj_id, conn_id)) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OBJECT_LOCKED, NULL, 0);
        return;
    }

    /* The Result Code has to report whether the execution itself succeeded, so
     * the application runs the action first. The callback is invoked
     * synchronously in this context and may overwrite cb_param.execute.result. */
    ble_ots_server_cb_param_t cb_param;
    memset(&cb_param, 0, sizeof(cb_param));
    cb_param.execute.object_id = obj_id;
    cb_param.execute.param = (param_len > 0) ? param : NULL;
    cb_param.execute.param_len = param_len;
    cb_param.execute.result = BLE_OTS_OACP_RESULT_SUCCESS;
    ble_ots_server_dispatch_event(BLE_OTS_SERVER_EVT_EXECUTE, &cb_param);

    /* Guard against an application returning a code outside the OACP range */
    uint8_t result = cb_param.execute.result;
    if (result < BLE_OTS_OACP_RESULT_SUCCESS ||
        result > BLE_OTS_OACP_RESULT_OPERATION_FAILED) {
        ESP_LOGE(TAG, "Invalid execute result 0x%02x from application", result);
        result = BLE_OTS_OACP_RESULT_OPERATION_FAILED;
    }

    /* Send the indication carrying the application's result */
    oacp_send_response(conn_id, opcode, result, NULL, 0);
}

/*****************************************************************************
 * OACP Read Procedure (delegate to ots_server_oacp_transfer.c)
 *****************************************************************************/
static int oacp_handle_read(uint16_t conn_id, const uint8_t *param,
                            uint16_t param_len)
{
    uint8_t opcode = BLE_OTS_OACP_OPCODE_READ;

    /* Parameter: Offset (UINT32) + Length (UINT32) = 8 bytes */
    if (param_len != 8) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER, NULL, 0);
        return 0;
    }

    uint32_t offset = (uint32_t)param[0] |
                      ((uint32_t)param[1] << 8) |
                      ((uint32_t)param[2] << 16) |
                      ((uint32_t)param[3] << 24);

    uint32_t length = (uint32_t)param[4] |
                      ((uint32_t)param[5] << 8) |
                      ((uint32_t)param[6] << 16) |
                      ((uint32_t)param[7] << 24);

    return ble_ots_server_oacp_read_proc(conn_id, offset, length);
}

/*****************************************************************************
 * OACP Write Procedure (delegate to ots_server_oacp_transfer.c)
 *****************************************************************************/
static int oacp_handle_write(uint16_t conn_id, const uint8_t *param,
                             uint16_t param_len)
{
    uint8_t opcode = BLE_OTS_OACP_OPCODE_WRITE;

    /* Parameter: Offset (UINT32) + Length (UINT32) + Mode (UINT8) = 9 bytes.
     * All three fields are mandatory for the Write Op Code. */
    if (param_len != 9) {
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_INVALID_PARAMETER, NULL, 0);
        return 0;
    }

    uint32_t offset = (uint32_t)param[0] |
                      ((uint32_t)param[1] << 8) |
                      ((uint32_t)param[2] << 16) |
                      ((uint32_t)param[3] << 24);

    uint32_t length = (uint32_t)param[4] |
                      ((uint32_t)param[5] << 8) |
                      ((uint32_t)param[6] << 16) |
                      ((uint32_t)param[7] << 24);

    uint8_t mode = param[8];

    return ble_ots_server_oacp_write_proc(conn_id, offset, length, mode);
}

/*****************************************************************************
 * OACP Abort Procedure (delegate to ots_server_oacp_transfer.c)
 *****************************************************************************/
static int oacp_handle_abort(uint16_t conn_id)
{
    return ble_ots_server_oacp_abort_proc(conn_id);
}

/*****************************************************************************
 * OACP Characteristic Write Handler — main dispatch
 *
 * This is the GATT access callback for writes to the OACP characteristic.
 * It is called from the GATT service table registered in ots_server_init.c.
 *****************************************************************************/
static int ble_ots_server_oacp_write_handler(uint16_t conn_id, uint16_t attr_handle,
                                             struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint8_t stack_buf[64];
    uint8_t *buf = stack_buf;
    uint8_t *dyn_buf = NULL;
    uint16_t data_len = 0;
    int rc;

    /* Extract write data from mbuf */
    uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);
    if (om_len == 0) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    if (om_len > sizeof(stack_buf)) {
        /* Only the Execute opcode allows arbitrary-length parameters.
         * Peek at the first byte (opcode) to decide whether to accept. */
        uint8_t peek_opcode;
        rc = ble_hs_mbuf_to_flat(ctxt->om, &peek_opcode, 1, NULL);
        if (rc != 0 || peek_opcode != BLE_OTS_OACP_OPCODE_EXECUTE) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }

        dyn_buf = malloc(om_len);
        if (dyn_buf == NULL) {
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        buf = dyn_buf;
    }

    rc = ble_hs_mbuf_to_flat(ctxt->om, buf, om_len, &data_len);
    if (rc != 0) {
        free(dyn_buf);
        return BLE_ATT_ERR_UNLIKELY;
    }

    if (data_len < 1) {
        free(dyn_buf);
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    uint8_t opcode = buf[0];
    const uint8_t *param = (data_len > 1) ? &buf[1] : NULL;
    uint16_t param_len = data_len - 1;

    /* ATT-level check: concurrency */
    if (!ble_ots_server_concurrency_check(conn_id)) {
        free(dyn_buf);
        return BLE_OTS_APP_ERR_CONCURRENCY_LIMIT_EXCEEDED;
    }

    /* ATT-level check: OACP CCCD must be configured for indications */
    if (!oacp_cccd_configured(conn_id)) {
        free(dyn_buf);
        return ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
    }

    /* ATT-level check: procedure already in progress */
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (cs == NULL) {
        free(dyn_buf);
        return BLE_ATT_ERR_UNLIKELY;
    }

    /* For Read/Write/Abort, check if a transfer is already active (only for
     * non-abort opcodes when a transfer is in progress) */
    if (cs->transfer_active && opcode != BLE_OTS_OACP_OPCODE_ABORT) {
        free(dyn_buf);
        return ATT_ERR_PROC_ALREADY_IN_PROGRESS;
    }

    /* ATT-level validation: parameter size for opcodes that require no parameter */
    switch (opcode) {
    case BLE_OTS_OACP_OPCODE_DELETE:
    case BLE_OTS_OACP_OPCODE_ABORT:
        if (param_len != 0) {
            free(dyn_buf);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        break;

    case BLE_OTS_OACP_OPCODE_CREATE:
        /* Size (4) + Type UUID (2 or 16) = 6 or 20 */
        if (param_len < 6 || (param_len != 6 && param_len != 20)) {
            free(dyn_buf);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        break;

    case BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM:
        /* Offset (4) + Length (4) = 8 */
        if (param_len != 8) {
            free(dyn_buf);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        break;

    case BLE_OTS_OACP_OPCODE_READ:
        /* Offset (4) + Length (4) = 8 */
        if (param_len != 8) {
            free(dyn_buf);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        break;

    case BLE_OTS_OACP_OPCODE_WRITE:
        /* Offset (4) + Length (4) + Mode (1) = 9 */
        if (param_len != 9) {
            free(dyn_buf);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        break;

    case BLE_OTS_OACP_OPCODE_EXECUTE:
        /* Optional parameter — any length is acceptable */
        break;

    default:
        /* Unknown/reserved opcode — accept the write, then send Op Code Not Supported */
        break;
    }

    /* ATT Write Response is sent by NimBLE stack (return 0).
     * Now dispatch to the appropriate procedure handler.
     * The procedure handler will send the OACP response indication. */

    switch (opcode) {
    case BLE_OTS_OACP_OPCODE_CREATE:
        oacp_create_proc(conn_id, param, param_len);
        break;

    case BLE_OTS_OACP_OPCODE_DELETE:
        oacp_delete_proc(conn_id);
        break;

    case BLE_OTS_OACP_OPCODE_CALCULATE_CHECKSUM:
        oacp_calculate_checksum_proc(conn_id, param, param_len);
        break;

    case BLE_OTS_OACP_OPCODE_EXECUTE:
        oacp_execute_proc(conn_id, param, param_len);
        break;

    case BLE_OTS_OACP_OPCODE_READ:
        rc = oacp_handle_read(conn_id, param, param_len);
        if (rc != 0) {
            ESP_LOGE(TAG, "OACP Read proc failed, rc=%d", rc);
        }
        break;

    case BLE_OTS_OACP_OPCODE_WRITE:
        rc = oacp_handle_write(conn_id, param, param_len);
        if (rc != 0) {
            ESP_LOGE(TAG, "OACP Write proc failed, rc=%d", rc);
        }
        break;

    case BLE_OTS_OACP_OPCODE_ABORT:
        rc = oacp_handle_abort(conn_id);
        if (rc != 0) {
            ESP_LOGE(TAG, "OACP Abort proc failed, rc=%d", rc);
        }
        break;

    default:
        /* Reserved or unknown opcode — respond with Op Code Not Supported */
        oacp_send_response(conn_id, opcode,
                           BLE_OTS_OACP_RESULT_OP_CODE_NOT_SUPPORTED, NULL, 0);
        break;
    }

    free(dyn_buf);
    return 0;
}

/*****************************************************************************
 * OACP Characteristic Write — public wrapper
 *
 * Called from the central GATT access callback in ots_server_init.c.
 *****************************************************************************/
int ble_ots_server_oacp_write(uint16_t conn_handle,
                              struct ble_gatt_access_ctxt *ctxt)
{
    return ble_ots_server_oacp_write_handler(conn_handle, 0, ctxt, NULL);
}
