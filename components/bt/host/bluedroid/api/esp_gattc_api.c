/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_gattc_api.h"
#include "esp_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_gattc.h"
#include "btc_gatt_util.h"
#include "stack/l2cdefs.h"
#include "stack/l2c_api.h"
#include "gatt_int.h"


#if (GATTC_INCLUDED == TRUE)
esp_err_t esp_ble_gattc_register_callback(esp_gattc_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_GATTC, callback);
    return ESP_OK;
}

esp_gattc_cb_t esp_ble_gattc_get_callback(void)
{
    return (esp_gattc_cb_t) btc_profile_cb_get(BTC_PID_GATTC);
}

esp_err_t esp_ble_gattc_app_register(uint16_t app_id)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (app_id > ESP_APP_ID_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_APP_REGISTER;
    arg.app_reg.app_id = app_id;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_app_unregister(esp_gatt_if_t gattc_if)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_APP_UNREGISTER;
    arg.app_unreg.gattc_if = gattc_if;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_enh_open(esp_gatt_if_t gattc_if, esp_ble_gatt_creat_conn_params_t *creat_conn_params)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;
    const esp_ble_conn_params_t *conn_params;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (!creat_conn_params) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_OPEN;
    arg.open.gattc_if = gattc_if;
    memcpy(arg.open.remote_bda, creat_conn_params->remote_bda, ESP_BD_ADDR_LEN);
    arg.open.remote_addr_type = creat_conn_params->remote_addr_type;
    arg.open.is_direct = creat_conn_params->is_direct;
    arg.open.is_aux= creat_conn_params->is_aux;
    arg.open.own_addr_type = creat_conn_params->own_addr_type;
    arg.open.phy_mask = creat_conn_params->phy_mask;

    // If not aux open, shouldn't set 2M and coded PHY connection params
    if (!creat_conn_params->is_aux &&
        ((creat_conn_params->phy_mask & ESP_BLE_PHY_2M_PREF_MASK) ||
        (creat_conn_params->phy_mask & ESP_BLE_PHY_CODED_PREF_MASK))) {
        return ESP_ERR_INVALID_ARG;
    }

    if (creat_conn_params->phy_mask & ESP_BLE_PHY_1M_PREF_MASK) {
        if (!creat_conn_params->phy_1m_conn_params) {
            return ESP_ERR_INVALID_ARG;
        }

        conn_params = creat_conn_params->phy_1m_conn_params;
        if (ESP_BLE_IS_VALID_PARAM(conn_params->interval_min, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(conn_params->interval_max, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(conn_params->supervision_timeout, ESP_BLE_CONN_SUP_TOUT_MIN, ESP_BLE_CONN_SUP_TOUT_MAX) &&
            (conn_params->latency <= ESP_BLE_CONN_LATENCY_MAX) &&
            ((conn_params->supervision_timeout * 10) >= ((1 + conn_params->latency) * ((conn_params->interval_max * 5) >> 1))) &&
            (conn_params->interval_min <= conn_params->interval_max)) {
            memcpy(&arg.open.phy_1m_conn_params, conn_params, sizeof(esp_ble_conn_params_t));
        } else {
            LOG_ERROR("%s, invalid 1M PHY connection params: min_int = %d, max_int = %d, latency = %d, timeout = %d", __func__,
                                    conn_params->interval_min,
                                    conn_params->interval_max,
                                    conn_params->latency,
                                    conn_params->supervision_timeout);
            return ESP_ERR_INVALID_ARG;
        }
    }

    if (creat_conn_params->phy_mask & ESP_BLE_PHY_2M_PREF_MASK) {
        if (!creat_conn_params->phy_2m_conn_params) {
            return ESP_ERR_INVALID_ARG;
        }

        conn_params = creat_conn_params->phy_2m_conn_params;
        if (ESP_BLE_IS_VALID_PARAM(conn_params->interval_min, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(conn_params->interval_max, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(conn_params->supervision_timeout, ESP_BLE_CONN_SUP_TOUT_MIN, ESP_BLE_CONN_SUP_TOUT_MAX) &&
            (conn_params->latency <= ESP_BLE_CONN_LATENCY_MAX) &&
            ((conn_params->supervision_timeout * 10) >= ((1 + conn_params->latency) * ((conn_params->interval_max * 5) >> 1))) &&
            (conn_params->interval_min <= conn_params->interval_max)) {
            memcpy(&arg.open.phy_2m_conn_params, conn_params, sizeof(esp_ble_conn_params_t));
        } else {
            LOG_ERROR("%s, invalid 2M PHY connection params: min_int = %d, max_int = %d, latency = %d, timeout = %d", __func__,
                                    conn_params->interval_min,
                                    conn_params->interval_max,
                                    conn_params->latency,
                                    conn_params->supervision_timeout);
            return ESP_ERR_INVALID_ARG;
        }
    }

    if (creat_conn_params->phy_mask & ESP_BLE_PHY_CODED_PREF_MASK) {
        if (!creat_conn_params->phy_coded_conn_params) {
            return ESP_ERR_INVALID_ARG;
        }

        conn_params = creat_conn_params->phy_coded_conn_params;
        if (ESP_BLE_IS_VALID_PARAM(conn_params->interval_min, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(conn_params->interval_max, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(conn_params->supervision_timeout, ESP_BLE_CONN_SUP_TOUT_MIN, ESP_BLE_CONN_SUP_TOUT_MAX) &&
            (conn_params->latency <= ESP_BLE_CONN_LATENCY_MAX) &&
            ((conn_params->supervision_timeout * 10) >= ((1 + conn_params->latency) * ((conn_params->interval_max * 5) >> 1))) &&
            (conn_params->interval_min <= conn_params->interval_max)) {
            memcpy(&arg.open.phy_coded_conn_params, conn_params, sizeof(esp_ble_conn_params_t));
        } else {
            LOG_ERROR("%s, invalid Coded PHY connection params: min_int = %d, max_int = %d, latency = %d, timeout = %d", __func__,
                                    conn_params->interval_min,
                                    conn_params->interval_max,
                                    conn_params->latency,
                                    conn_params->supervision_timeout);
            return ESP_ERR_INVALID_ARG;
        }
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#if (BLE_42_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gattc_open(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda, esp_ble_addr_type_t remote_addr_type, bool is_direct)
{
    esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
    memcpy(creat_conn_params.remote_bda, remote_bda, ESP_BD_ADDR_LEN);
    creat_conn_params.remote_addr_type = remote_addr_type;
    creat_conn_params.is_direct = is_direct;
    creat_conn_params.is_aux = false;
    creat_conn_params.own_addr_type = 0xff; //undefined, will use local value
    creat_conn_params.phy_mask = 0x0;
    return esp_ble_gattc_enh_open(gattc_if, &creat_conn_params);
}
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gattc_aux_open(esp_gatt_if_t gattc_if, esp_bd_addr_t remote_bda, esp_ble_addr_type_t remote_addr_type, bool is_direct)
{
    esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
    memcpy(creat_conn_params.remote_bda, remote_bda, ESP_BD_ADDR_LEN);
    creat_conn_params.remote_addr_type = remote_addr_type;
    creat_conn_params.is_direct = is_direct;
    creat_conn_params.is_aux = true;
    creat_conn_params.own_addr_type = 0xff; //undefined, will use local value
    creat_conn_params.phy_mask = 0x0;
    return esp_ble_gattc_enh_open(gattc_if, &creat_conn_params);
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

esp_err_t esp_ble_gattc_close (esp_gatt_if_t gattc_if, uint16_t conn_id)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CLOSE;
    arg.close.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_send_mtu_req (esp_gatt_if_t gattc_if, uint16_t conn_id)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CFG_MTU;
    arg.cfg_mtu.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_search_service(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_bt_uuid_t *filter_uuid)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_SEARCH_SERVICE;
    arg.search_srvc.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);

    if (filter_uuid) {
        arg.search_srvc.filter_uuid_enable = true;
        memcpy(&arg.search_srvc.filter_uuid, filter_uuid, sizeof(esp_bt_uuid_t));
    } else {
        arg.search_srvc.filter_uuid_enable = false;
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_gatt_status_t esp_ble_gattc_get_service(esp_gatt_if_t gattc_if, uint16_t conn_id, esp_bt_uuid_t *svc_uuid,
                                            esp_gattc_service_elem_t *result, uint16_t *count, uint16_t offset)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_service(conn_hdl, svc_uuid, result, count, offset);
}


esp_gatt_status_t esp_ble_gattc_get_all_char(esp_gatt_if_t gattc_if,
                                             uint16_t conn_id,
                                             uint16_t start_handle,
                                             uint16_t end_handle,
                                             esp_gattc_char_elem_t *result,
                                             uint16_t *count, uint16_t offset)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((start_handle == 0) && (end_handle == 0)) {
        *count = 0;
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_all_char(conn_hdl, start_handle, end_handle, result, count, offset);
}

esp_gatt_status_t esp_ble_gattc_get_all_descr(esp_gatt_if_t gattc_if,
                                              uint16_t conn_id,
                                              uint16_t char_handle,
                                              esp_gattc_descr_elem_t *result,
                                              uint16_t *count, uint16_t offset)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (char_handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_all_descr(conn_hdl, char_handle, result, count, offset);
}

esp_gatt_status_t esp_ble_gattc_get_char_by_uuid(esp_gatt_if_t gattc_if,
                                                 uint16_t conn_id,
                                                 uint16_t start_handle,
                                                 uint16_t end_handle,
                                                 esp_bt_uuid_t char_uuid,
                                                 esp_gattc_char_elem_t *result,
                                                 uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (start_handle == 0 && end_handle == 0) {
        *count = 0;
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if,conn_id);
    return btc_ble_gattc_get_char_by_uuid(conn_hdl, start_handle, end_handle, char_uuid, result, count);
}


esp_gatt_status_t esp_ble_gattc_get_descr_by_uuid(esp_gatt_if_t gattc_if,
                                                  uint16_t conn_id,
                                                  uint16_t start_handle,
                                                  uint16_t end_handle,
                                                  esp_bt_uuid_t char_uuid,
                                                  esp_bt_uuid_t descr_uuid,
                                                  esp_gattc_descr_elem_t *result,
                                                  uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_descr_by_uuid(conn_hdl, start_handle, end_handle, char_uuid, descr_uuid, result, count);
}

esp_gatt_status_t esp_ble_gattc_get_descr_by_char_handle(esp_gatt_if_t gattc_if,
                                                         uint16_t conn_id,
                                                         uint16_t char_handle,
                                                         esp_bt_uuid_t descr_uuid,
                                                         esp_gattc_descr_elem_t *result,
                                                         uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (char_handle == 0) {
        *count = 0;
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_descr_by_char_handle(conn_hdl, char_handle, descr_uuid, result, count);
}

esp_gatt_status_t esp_ble_gattc_get_include_service(esp_gatt_if_t gattc_if,
                                                    uint16_t conn_id,
                                                    uint16_t start_handle,
                                                    uint16_t end_handle,
                                                    esp_bt_uuid_t *incl_uuid,
                                                    esp_gattc_incl_svc_elem_t *result,
                                                    uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (start_handle == 0 && end_handle == 0) {
        *count = 0;
        return ESP_GATT_INVALID_HANDLE;
    }

    if (result == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_include_service(conn_hdl, start_handle, end_handle, incl_uuid, result, count);
}

esp_gatt_status_t esp_ble_gattc_get_attr_count(esp_gatt_if_t gattc_if,
                                               uint16_t conn_id,
                                               esp_gatt_db_attr_type_t type,
                                               uint16_t start_handle,
                                               uint16_t end_handle,
                                               uint16_t char_handle,
                                               uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((start_handle == 0 && end_handle == 0) && (type != ESP_GATT_DB_DESCRIPTOR)) {
        *count = 0;
        return ESP_GATT_INVALID_HANDLE;
    }

    if (count == NULL) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_attr_count(conn_hdl, type, start_handle, end_handle, char_handle, count);
}

esp_gatt_status_t esp_ble_gattc_get_db(esp_gatt_if_t gattc_if, uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
                                       esp_gattc_db_elem_t *db, uint16_t *count)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (start_handle == 0 && end_handle == 0) {
        *count = 0;
        return ESP_GATT_INVALID_HANDLE;
    }

    if (db == NULL || count == NULL || *count == 0) {
        return ESP_GATT_INVALID_PDU;
    }

    uint16_t conn_hdl = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    return btc_ble_gattc_get_db(conn_hdl, start_handle, end_handle, db, count);
}


esp_err_t esp_ble_gattc_read_char (esp_gatt_if_t gattc_if,
                                   uint16_t conn_id, uint16_t handle,
                                   esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    if (handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_CHAR;
    arg.read_char.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.read_char.handle = handle;
    arg.read_char.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_read_by_type (esp_gatt_if_t gattc_if,
                                      uint16_t conn_id,
                                      uint16_t start_handle,
                                      uint16_t end_handle,
                                      esp_bt_uuid_t *uuid,
                                      esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (uuid == NULL) {
        return ESP_GATT_ILLEGAL_PARAMETER;
    }

    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    if (start_handle == 0 || end_handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_BY_TYPE;
    arg.read_by_type.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.read_by_type.s_handle = start_handle;
    arg.read_by_type.e_handle = end_handle;
    arg.read_by_type.auth_req = auth_req;
    memcpy(&(arg.read_by_type.uuid), uuid, sizeof(esp_bt_uuid_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_read_multiple(esp_gatt_if_t gattc_if,
                                      uint16_t conn_id, esp_gattc_multi_t *read_multi,
                                      esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_MULTIPLE_CHAR;
    arg.read_multiple.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.read_multiple.num_attr = read_multi->num_attr;
    arg.read_multiple.auth_req = auth_req;

    if (read_multi->num_attr > 0) {
        memcpy(arg.read_multiple.handles, read_multi->handles, sizeof(uint16_t)*read_multi->num_attr);
    } else {
        LOG_ERROR("%s(), the num_attr should not be 0.", __func__);
        return ESP_FAIL;
    }
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_read_multiple_variable(esp_gatt_if_t gattc_if,
                                      uint16_t conn_id, esp_gattc_multi_t *read_multi,
                                      esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_MULTIPLE_VARIABLE_CHAR;
    arg.read_multiple.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.read_multiple.num_attr = read_multi->num_attr;
    arg.read_multiple.auth_req = auth_req;

    if (read_multi->num_attr > 0) {
        memcpy(arg.read_multiple.handles, read_multi->handles, sizeof(uint16_t)*read_multi->num_attr);
    } else {
        LOG_ERROR("%s(), the num_attr should not be 0.", __func__);
        return ESP_FAIL;
    }
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_read_char_descr (esp_gatt_if_t gattc_if,
                                         uint16_t conn_id, uint16_t handle,
                                         esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    if (handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_READ_CHAR_DESCR;
    arg.read_descr.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.read_descr.handle = handle;
    arg.read_descr.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_write_char(esp_gatt_if_t gattc_if,
                                   uint16_t conn_id, uint16_t handle,
                                   uint16_t value_len,
                                   uint8_t *value,
                                   esp_gatt_write_type_t write_type,
                                   esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    if (handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_WRITE_CHAR;
    arg.write_char.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.write_char.handle = handle;
    arg.write_char.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len;
    arg.write_char.value = value;
    arg.write_char.write_type = write_type;
    arg.write_char.auth_req = auth_req;
    if(write_type == ESP_GATT_WRITE_TYPE_NO_RSP){
        l2ble_update_att_acl_pkt_num(L2CA_ADD_BTC_NUM, NULL);
    }
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy,
                btc_gattc_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_write_char_descr (esp_gatt_if_t gattc_if,
                                          uint16_t conn_id, uint16_t handle,
                                          uint16_t value_len,
                                          uint8_t *value,
                                          esp_gatt_write_type_t write_type,
                                          esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    if (handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_WRITE_CHAR_DESCR;
    arg.write_descr.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.write_descr.handle = handle;
    arg.write_descr.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len;
    arg.write_descr.value = value;
    arg.write_descr.write_type = write_type;
    arg.write_descr.auth_req = auth_req;
    if(write_type == ESP_GATT_WRITE_TYPE_NO_RSP){
        l2ble_update_att_acl_pkt_num(L2CA_ADD_BTC_NUM, NULL);
    }
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy,
                btc_gattc_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_prepare_write(esp_gatt_if_t gattc_if,
                                      uint16_t conn_id, uint16_t handle,
                                      uint16_t offset,
                                      uint16_t value_len,
                                      uint8_t *value,
                                      esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    if (handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_PREPARE_WRITE;
    arg.prep_write.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.prep_write.handle = handle;
    arg.prep_write.offset = offset;
    arg.prep_write.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len; // length check ?
    arg.prep_write.value = value;
    arg.prep_write.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy,
                btc_gattc_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_prepare_write_char_descr(esp_gatt_if_t gattc_if,
                                                 uint16_t conn_id, uint16_t handle,
                                                 uint16_t offset,
                                                 uint16_t value_len,
                                                 uint8_t *value,
                                                 esp_gatt_auth_req_t auth_req)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(conn_id);
    if (!gatt_check_connection_state_by_tcb(p_tcb)) {
        LOG_WARN("%s, The connection not created.", __func__);
        return ESP_ERR_INVALID_STATE;
    }

    if (L2CA_CheckIsCongest(L2CAP_ATT_CID, p_tcb->peer_bda)) {
        LOG_DEBUG("%s, the l2cap channel is congest.", __func__);
        return ESP_FAIL;
    }

    if (handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR;
    arg.prep_write_descr.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.prep_write_descr.handle = handle;
    arg.prep_write_descr.offset = offset;
    arg.prep_write_descr.value_len = value_len > ESP_GATT_MAX_ATTR_LEN ? ESP_GATT_MAX_ATTR_LEN : value_len; // length check ?
    arg.prep_write_descr.value = value;
    arg.prep_write_descr.auth_req = auth_req;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), btc_gattc_arg_deep_copy,
                btc_gattc_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_execute_write (esp_gatt_if_t gattc_if, uint16_t conn_id, bool is_execute)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_EXECUTE_WRITE;
    arg.exec_write.conn_id = BTC_GATT_CREATE_CONN_ID(gattc_if, conn_id);
    arg.exec_write.is_execute = is_execute;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_register_for_notify (esp_gatt_if_t gattc_if,
                                             esp_bd_addr_t server_bda, uint16_t handle)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_REG_FOR_NOTIFY;
    arg.reg_for_notify.gattc_if = gattc_if;
    memcpy(arg.reg_for_notify.remote_bda, server_bda, sizeof(esp_bd_addr_t));
    arg.reg_for_notify.handle = handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_unregister_for_notify (esp_gatt_if_t gattc_if,
                                               esp_bd_addr_t server_bda, uint16_t handle)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (handle == 0) {
        return ESP_GATT_INVALID_HANDLE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_UNREG_FOR_NOTIFY;
    arg.unreg_for_notify.gattc_if = gattc_if;
    arg.unreg_for_notify.handle = handle;
    memcpy(arg.unreg_for_notify.remote_bda, server_bda, sizeof(esp_bd_addr_t));
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_cache_refresh(esp_bd_addr_t remote_bda)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CACHE_REFRESH;
    memcpy(arg.cache_refresh.remote_bda, remote_bda, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_cache_clean(esp_bd_addr_t remote_bda)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CACHE_CLEAN;
    memcpy(arg.cache_clean.remote_bda, remote_bda, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_cache_assoc(esp_gatt_if_t gattc_if, esp_bd_addr_t src_addr, esp_bd_addr_t assoc_addr, bool is_assoc)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ACT_CACHE_ASSOC;
    arg.cache_assoc.is_assoc = is_assoc;
    arg.cache_assoc.gattc_if = gattc_if;
    memcpy(arg.cache_assoc.src_addr, src_addr, sizeof(esp_bd_addr_t));
    memcpy(arg.cache_assoc.assoc_addr, assoc_addr, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gattc_cache_get_addr_list(esp_gatt_if_t gattc_if)
{
    btc_msg_t msg = {0};
    btc_ble_gattc_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTC;
    msg.act = BTC_GATTC_ATC_CACHE_GET_ADDR_LIST;
    arg.get_addr_list.gattc_if = gattc_if;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gattc_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#endif  ///GATTC_INCLUDED == TRUE
