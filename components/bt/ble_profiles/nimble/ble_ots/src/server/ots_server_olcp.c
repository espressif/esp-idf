/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "host/ble_gap.h"
#include "os/os_mbuf.h"
#include "ble_ots_server_int.h"

static const char *TAG = "ots_olcp";

/*****************************************************************************
 * Helper: Check if OLCP CCCD is configured for indications
 *
 * Per the OTS specification, if the OLCP CCCD has not been configured for
 * indications, the server must reject any write with the ATT error
 * "Client Characteristic Configuration Descriptor Improperly Configured"
 * (0xFD). Subscription state is tracked via BLE_GAP_EVENT_SUBSCRIBE events.
 *****************************************************************************/
static bool olcp_cccd_configured(uint16_t conn_id)
{
    /* Verify connection exists */
    struct ble_gap_conn_desc desc;
    if (ble_gap_conn_find(conn_id, &desc) != 0) {
        return false;
    }

    /* Verify OLCP value handle is registered */
    if (ble_ots_server_olcp_handle_get() == 0) {
        return false;
    }

    /* Check the per-connection OLCP indication subscription state */
    ble_ots_server_conn_state_t *state = ble_ots_server_conn_state_get(conn_id);
    if (state == NULL) {
        return false;
    }

    return state->olcp_indicate_subscribed;
}

/*****************************************************************************
 * Handle BLE_GAP_EVENT_SUBSCRIBE for OLCP CCCD tracking
 *****************************************************************************/
void ble_ots_server_olcp_handle_subscribe(uint16_t conn_handle,
                                           uint16_t attr_handle,
                                           bool cur_indicate)
{
    if (attr_handle != ble_ots_server_olcp_handle_get()) {
        return;
    }

    ble_ots_server_conn_state_t *state = ble_ots_server_conn_state_get(conn_handle);
    if (state == NULL) {
        return;
    }

    state->olcp_indicate_subscribed = cur_indicate;
    ESP_LOGI(TAG, "OLCP CCCD subscribe: conn=%d indicate=%d",
             conn_handle, cur_indicate);
}

/*****************************************************************************
 * Helper: Check if the object database contains any user objects
 *
 * The Directory Listing Object is always present by default, so it is skipped
 * here to reflect whether any user object exists.
 *****************************************************************************/
bool ble_ots_server_obj_db_has_objects(void)
{
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[i];
        if (obj->in_use && obj->object_id != BLE_OTS_OBJ_ID_DIRECTORY_LISTING) {
            return true;
        }
    }
    return false;
}

/*****************************************************************************
 * Helper: Check if a sort order value is valid
 *****************************************************************************/
static bool is_valid_sort_order(uint8_t order)
{
    switch (order) {
    case BLE_OTS_SORT_ORDER_NAME_ASC:
    case BLE_OTS_SORT_ORDER_TYPE_ASC:
    case BLE_OTS_SORT_ORDER_CURRENT_SIZE_ASC:
    case BLE_OTS_SORT_ORDER_FIRST_CREATED_ASC:
    case BLE_OTS_SORT_ORDER_LAST_MODIFIED_ASC:
    case BLE_OTS_SORT_ORDER_NAME_DESC:
    case BLE_OTS_SORT_ORDER_TYPE_DESC:
    case BLE_OTS_SORT_ORDER_CURRENT_SIZE_DESC:
    case BLE_OTS_SORT_ORDER_FIRST_CREATED_DESC:
    case BLE_OTS_SORT_ORDER_LAST_MODIFIED_DESC:
        return true;
    default:
        return false;
    }
}

/*****************************************************************************
 * Helper: Check if an opcode is supported based on OLCP features
 *****************************************************************************/
static bool olcp_opcode_supported(uint8_t opcode)
{
    uint32_t olcp_feat = ble_ots_server_env.ots_feature.olcp_features;

    switch (opcode) {
    /* Mandatory opcodes - always supported */
    case BLE_OTS_OLCP_OPCODE_FIRST:
    case BLE_OTS_OLCP_OPCODE_LAST:
    case BLE_OTS_OLCP_OPCODE_PREVIOUS:
    case BLE_OTS_OLCP_OPCODE_NEXT:
        return true;

    /* Optional opcodes - check feature bits */
    case BLE_OTS_OLCP_OPCODE_GO_TO:
        return (olcp_feat & BLE_OTS_OLCP_FEAT_GO_TO) != 0;
    case BLE_OTS_OLCP_OPCODE_ORDER:
        return (olcp_feat & BLE_OTS_OLCP_FEAT_ORDER) != 0;
    case BLE_OTS_OLCP_OPCODE_REQUEST_NUM_OF_OBJECTS:
        return (olcp_feat & BLE_OTS_OLCP_FEAT_REQUEST_NUM_OF_OBJECTS) != 0;
    case BLE_OTS_OLCP_OPCODE_CLEAR_MARKING:
        return (olcp_feat & BLE_OTS_OLCP_FEAT_CLEAR_MARKING) != 0;

    default:
        return false;
    }
}

/*****************************************************************************
 * Helper: Get expected parameter length for an opcode
 *****************************************************************************/
static int olcp_get_param_len(uint8_t opcode)
{
    switch (opcode) {
    case BLE_OTS_OLCP_OPCODE_FIRST:
    case BLE_OTS_OLCP_OPCODE_LAST:
    case BLE_OTS_OLCP_OPCODE_PREVIOUS:
    case BLE_OTS_OLCP_OPCODE_NEXT:
    case BLE_OTS_OLCP_OPCODE_REQUEST_NUM_OF_OBJECTS:
    case BLE_OTS_OLCP_OPCODE_CLEAR_MARKING:
        return 0;
    case BLE_OTS_OLCP_OPCODE_GO_TO:
        return 6; /* UINT48 Object ID */
    case BLE_OTS_OLCP_OPCODE_ORDER:
        return 1; /* UINT8 sort order */
    default:
        return -1; /* Unknown opcode */
    }
}

/*****************************************************************************
 * Helper: Send OLCP response indication
 *****************************************************************************/
static void olcp_send_response(uint16_t conn_id, uint8_t request_opcode,
                               uint8_t result_code, const uint8_t *param,
                               uint16_t param_len)
{
    /* Response format: [0x70, request_opcode, result_code, optional_param] */
    uint8_t resp_buf[3 + 4]; /* Max: 3 header + 4 bytes param (UINT32) */
    uint16_t resp_len = 3;

    resp_buf[0] = BLE_OTS_OLCP_OPCODE_RESPONSE; /* 0x70 */
    resp_buf[1] = request_opcode;
    resp_buf[2] = result_code;

    if (param != NULL && param_len > 0 && param_len <= 4) {
        memcpy(&resp_buf[3], param, param_len);
        resp_len += param_len;
    }

    int rc = ble_ots_server_indicate_response(conn_id, BLE_OTS_UUID_OLCP,
                                               resp_buf, resp_len);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to send OLCP response indication; rc=%d", rc);
    }
}

/*****************************************************************************
 * Procedure: First (Op Code 0x01)
 * Note: The caller evaluates the filtered list and the No Object check, so
 *       @p list is guaranteed non-NULL and non-empty here.
 *****************************************************************************/
static void olcp_proc_first(uint16_t conn_id, uint8_t opcode,
                            const ble_ots_server_filtered_list_t *list)
{
    int rc = ble_ots_server_current_obj_set(conn_id, list->object_ids[0]);
    if (rc != 0) {
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                           NULL, 0);
        return;
    }

    olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_SUCCESS, NULL, 0);
}

/*****************************************************************************
 * Procedure: Last (Op Code 0x02)
 * Note: The caller evaluates the filtered list and the No Object check, so
 *       @p list is guaranteed non-NULL and non-empty here.
 *****************************************************************************/
static void olcp_proc_last(uint16_t conn_id, uint8_t opcode,
                           const ble_ots_server_filtered_list_t *list)
{
    int rc = ble_ots_server_current_obj_set(conn_id,
                                             list->object_ids[list->count - 1]);
    if (rc != 0) {
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                           NULL, 0);
        return;
    }

    olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_SUCCESS, NULL, 0);
}

/*****************************************************************************
 * Procedure: Previous (Op Code 0x03)
 * Note: The caller evaluates the filtered list and the No Object check, so
 *       @p list is guaranteed non-NULL and non-empty here.
 *****************************************************************************/
static void olcp_proc_previous(uint16_t conn_id, uint8_t opcode,
                               const ble_ots_server_filtered_list_t *list)
{
    ble_ots_obj_id_t current_id = ble_ots_server_current_obj_get(conn_id);
    if (current_id == BLE_OTS_OBJ_ID_INVALID) {
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                           NULL, 0);
        return;
    }

    /* Find current object position in the filtered list */
    int pos = -1;
    for (uint32_t i = 0; i < list->count; i++) {
        if (list->object_ids[i] == current_id) {
            pos = (int)i;
            break;
        }
    }

    if (pos < 0) {
        /* Current object not found in filtered list */
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                           NULL, 0);
        return;
    }

    if (pos == 0) {
        /* Already at the first object - Out of Bounds */
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OUT_OF_BOUNDS,
                           NULL, 0);
        return;
    }

    int rc = ble_ots_server_current_obj_set(conn_id,
                                             list->object_ids[pos - 1]);
    if (rc != 0) {
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                           NULL, 0);
        return;
    }

    olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_SUCCESS, NULL, 0);
}

/*****************************************************************************
 * Procedure: Next (Op Code 0x04)
 * Note: The caller evaluates the filtered list and the No Object check, so
 *       @p list is guaranteed non-NULL and non-empty here.
 *****************************************************************************/
static void olcp_proc_next(uint16_t conn_id, uint8_t opcode,
                           const ble_ots_server_filtered_list_t *list)
{
    ble_ots_obj_id_t current_id = ble_ots_server_current_obj_get(conn_id);
    if (current_id == BLE_OTS_OBJ_ID_INVALID) {
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                           NULL, 0);
        return;
    }

    /* Find current object position in the filtered list */
    int pos = -1;
    for (uint32_t i = 0; i < list->count; i++) {
        if (list->object_ids[i] == current_id) {
            pos = (int)i;
            break;
        }
    }

    if (pos < 0) {
        /* Current object not found in filtered list */
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                           NULL, 0);
        return;
    }

    if (pos == (int)(list->count - 1)) {
        /* Already at the last object - Out of Bounds */
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OUT_OF_BOUNDS,
                           NULL, 0);
        return;
    }

    int rc = ble_ots_server_current_obj_set(conn_id,
                                             list->object_ids[pos + 1]);
    if (rc != 0) {
        olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_OPERATION_FAILED,
                           NULL, 0);
        return;
    }

    olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_SUCCESS, NULL, 0);
}

/*****************************************************************************
 * Procedure: Go To (Op Code 0x05)
 *****************************************************************************/
static void olcp_proc_go_to(uint16_t conn_id, uint8_t opcode,
                             const uint8_t *param)
{
    /* Parse 6-byte Object ID (UINT48, little-endian) */
    uint64_t object_id = 0;
    for (int i = 0; i < 6; i++) {
        object_id |= ((uint64_t)param[i]) << (8 * i);
    }

    /* Look up the object directly in the database (filter bypass) */
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (obj == NULL) {
        olcp_send_response(conn_id, opcode,
                           BLE_OTS_OLCP_RESULT_OBJECT_ID_NOT_FOUND, NULL, 0);
        return;
    }

    /* Reset all three filter instances to No Filter for this connection */
    ble_ots_server_filter_reset(conn_id);

    /* Set the specified object as the Current Object */
    int rc = ble_ots_server_current_obj_set(conn_id, object_id);
    if (rc != 0) {
        olcp_send_response(conn_id, opcode,
                           BLE_OTS_OLCP_RESULT_OPERATION_FAILED, NULL, 0);
        return;
    }

    olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_SUCCESS, NULL, 0);
}

/*****************************************************************************
 * Procedure: Order (Op Code 0x06)
 *****************************************************************************/
static void olcp_proc_order(uint16_t conn_id, uint8_t opcode,
                             const uint8_t *param)
{
    uint8_t sort_order = param[0];

    /* Validate the sort order value */
    if (!is_valid_sort_order(sort_order)) {
        olcp_send_response(conn_id, opcode,
                           BLE_OTS_OLCP_RESULT_INVALID_PARAMETER, NULL, 0);
        return;
    }

    /* Store the sort order in the connection state */
    ble_ots_server_conn_state_t *state = ble_ots_server_conn_state_get(conn_id);
    if (state == NULL) {
        olcp_send_response(conn_id, opcode,
                           BLE_OTS_OLCP_RESULT_OPERATION_FAILED, NULL, 0);
        return;
    }

    state->sort_order = (ble_ots_list_sort_order_t)sort_order;

    olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_SUCCESS, NULL, 0);
}

/*****************************************************************************
 * Procedure: Request Number of Objects (Op Code 0x07)
 * Note: The caller evaluates the filtered list and the No Object check, so
 *       @p list is guaranteed non-NULL and non-empty here.
 *****************************************************************************/
static void olcp_proc_request_num_objects(uint16_t conn_id, uint8_t opcode,
                                          const ble_ots_server_filtered_list_t *list)
{
    /* Build the 4-byte UINT32 count parameter (little-endian) */
    uint8_t count_param[4];
    uint32_t count = list->count;
    count_param[0] = (uint8_t)(count & 0xFF);
    count_param[1] = (uint8_t)((count >> 8) & 0xFF);
    count_param[2] = (uint8_t)((count >> 16) & 0xFF);
    count_param[3] = (uint8_t)((count >> 24) & 0xFF);

    olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_SUCCESS,
                       count_param, sizeof(count_param));
}

/*****************************************************************************
 * Procedure: Clear Marking (Op Code 0x08)
 * Note: The caller evaluates the filtered list and the No Object check, so
 *       @p list is guaranteed non-NULL and non-empty here.
 *****************************************************************************/
static void olcp_proc_clear_marking(uint16_t conn_id, uint8_t opcode,
                                    const ble_ots_server_filtered_list_t *list)
{
    /* Clear the marked bit on all objects in the filtered list for this
     * connection's bond */
    for (uint32_t i = 0; i < list->count; i++) {
        int rc = ble_ots_server_mark_object(conn_id, list->object_ids[i], false);
        if (rc != 0) {
            ESP_LOGW(TAG, "Failed to clear mark for object 0x%06llx; rc=%d",
                     (unsigned long long)list->object_ids[i], rc);
        }
    }

    olcp_send_response(conn_id, opcode, BLE_OTS_OLCP_RESULT_SUCCESS, NULL, 0);
}

/*****************************************************************************
 * OLCP Characteristic Access Callback (NimBLE ble_gatt_access_fn)
 *****************************************************************************/
static int ble_ots_server_olcp_access(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
        /* OLCP is write-only from the client's perspective (+ indicate) */
        return BLE_ATT_ERR_REQ_NOT_SUPPORTED;
    }

    /* Extract the write data from the mbuf */
    uint8_t buf[1 + 6]; /* Max: 1 opcode + 6 bytes param (UINT48) */
    uint16_t data_len = 0;
    uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);

    if (om_len < 1) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    if (om_len > sizeof(buf)) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    int rc = ble_hs_mbuf_to_flat(ctxt->om, buf, sizeof(buf), &data_len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    uint8_t opcode = buf[0];
    uint8_t *param = (data_len > 1) ? &buf[1] : NULL;
    uint16_t param_len = (data_len > 1) ? (data_len - 1) : 0;

    /* --- ATT-level validation (Priority 1) --- */

    /* Check CCCD configured (ATT error 0xFD) */
    if (!olcp_cccd_configured(conn_handle)) {
        return ATT_ERR_CCCD_IMPROPERLY_CONFIGURED;
    }

    /* Check concurrency pool (ATT error 0x82) */
    if (!ble_ots_server_concurrency_check(conn_handle)) {
        return BLE_OTS_APP_ERR_CONCURRENCY_LIMIT_EXCEEDED;
    }

    /* Check no transfer in progress */
    ble_ots_server_conn_state_t *state = ble_ots_server_conn_state_get(conn_handle);
    if (state == NULL) {
        return BLE_OTS_APP_ERR_CONCURRENCY_LIMIT_EXCEEDED;
    }

    if (state->transfer_active) {
        return ATT_ERR_PROC_ALREADY_IN_PROGRESS;
    }

    /* Validate attribute value length for known opcodes */
    if (opcode >= BLE_OTS_OLCP_OPCODE_FIRST && opcode <= BLE_OTS_OLCP_OPCODE_CLEAR_MARKING) {
        int expected_param_len = olcp_get_param_len(opcode);
        if (expected_param_len >= 0 && param_len != (uint16_t)expected_param_len) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
    }

    /* --- ATT Write Response is sent (procedure starts) --- */
    /* From here on, errors are indicated via OLCP response, not ATT errors.
     * Error priority order (2-8) is enforced below. */

    /* Priority 2: Op Code Not Supported
     * Covers reserved opcodes and the Response Code (0x70), which a client
     * must never write, as well as unsupported optional opcodes. */
    if (opcode == 0x00 ||
        (opcode >= 0x09 && opcode <= 0x6F) ||
        (opcode >= BLE_OTS_OLCP_OPCODE_RESPONSE)) {
        olcp_send_response(conn_handle, opcode,
                           BLE_OTS_OLCP_RESULT_OP_CODE_NOT_SUPPORTED, NULL, 0);
        return 0;
    }

    if (!olcp_opcode_supported(opcode)) {
        olcp_send_response(conn_handle, opcode,
                           BLE_OTS_OLCP_RESULT_OP_CODE_NOT_SUPPORTED, NULL, 0);
        return 0;
    }

    /* Priority 3: No Object - filtered list contains zero objects.
     * For Go To (0x05), the procedure bypasses all active filters and
     * performs a direct database lookup. Only return No Object for Go To
     * if the entire object database is empty. For all other opcodes,
     * check the filtered list as usual. */
    ble_ots_server_filtered_list_t *list = ble_ots_server_filter_get_list(conn_handle);
    if (list == NULL) {
        olcp_send_response(conn_handle, opcode,
                           BLE_OTS_OLCP_RESULT_OPERATION_FAILED, NULL, 0);
        return 0;
    }

    if (opcode == BLE_OTS_OLCP_OPCODE_GO_TO) {
        /* Go To bypasses filters; only reject if the database is empty */
        if (!ble_ots_server_obj_db_has_objects()) {
            olcp_send_response(conn_handle, opcode,
                               BLE_OTS_OLCP_RESULT_NO_OBJECT, NULL, 0);
            return 0;
        }
    } else if (list->count == 0) {
        olcp_send_response(conn_handle, opcode,
                           BLE_OTS_OLCP_RESULT_NO_OBJECT, NULL, 0);
        return 0;
    }

    /* Priorities 4-8 are procedure-specific and handled within each proc */

    /* Execute the procedure based on opcode */
    switch (opcode) {
    case BLE_OTS_OLCP_OPCODE_FIRST:
        olcp_proc_first(conn_handle, opcode, list);
        break;

    case BLE_OTS_OLCP_OPCODE_LAST:
        olcp_proc_last(conn_handle, opcode, list);
        break;

    case BLE_OTS_OLCP_OPCODE_PREVIOUS:
        olcp_proc_previous(conn_handle, opcode, list);
        break;

    case BLE_OTS_OLCP_OPCODE_NEXT:
        olcp_proc_next(conn_handle, opcode, list);
        break;

    case BLE_OTS_OLCP_OPCODE_GO_TO:
        olcp_proc_go_to(conn_handle, opcode, param);
        break;

    case BLE_OTS_OLCP_OPCODE_ORDER:
        olcp_proc_order(conn_handle, opcode, param);
        break;

    case BLE_OTS_OLCP_OPCODE_REQUEST_NUM_OF_OBJECTS:
        olcp_proc_request_num_objects(conn_handle, opcode, list);
        break;

    case BLE_OTS_OLCP_OPCODE_CLEAR_MARKING:
        olcp_proc_clear_marking(conn_handle, opcode, list);
        break;

    default:
        /* Should not reach here due to earlier checks */
        olcp_send_response(conn_handle, opcode,
                           BLE_OTS_OLCP_RESULT_OP_CODE_NOT_SUPPORTED, NULL, 0);
        break;
    }

    return 0;
}

/*****************************************************************************
 * OLCP Write Wrapper (called from central GATT callback)
 *****************************************************************************/
int ble_ots_server_olcp_write(uint16_t conn_handle,
                               struct ble_gatt_access_ctxt *ctxt)
{
    return ble_ots_server_olcp_access(conn_handle, 0, ctxt, NULL);
}
