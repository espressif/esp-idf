// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "common/bt_target.h"
#include <stdlib.h>
#include <string.h>
#include "btc/btc_common.h"
#include "btc/btc_dm.h"
#include "btc/btc_main.h"
#include "common/bt_trace.h"
#include "common/bt_target.h"
#include "btc/btc_storage.h"
#include "btc/btc_ble_storage.h"
#include "esp_gap_ble_api.h"
#include "btm_int.h"
#include "bta/bta_api.h"
#include "bta/bta_gatt_api.h"
#include "osi/allocator.h"
#include "btc/btc_manage.h"


#if (BTC_GAP_BT_INCLUDED == TRUE)
#include "btc_gap_bt.h"
#endif /* BTC_GAP_BT_INCLUDED == TRUE */
/******************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTA_SERVICE_ID_TO_SERVICE_MASK(id)       (1 << (id))

/******************************************************************************
**  Static variables
******************************************************************************/
#if BTC_DYNAMIC_MEMORY == FALSE
btc_dm_cb_t btc_dm_cb = {0};
#else
btc_dm_cb_t *btc_dm_cb_ptr;
#endif

/******************************************************************************
**  Static functions
******************************************************************************/
/******************************************************************************
**  Externs
******************************************************************************/
#if BTC_AV_INCLUDED
extern bt_status_t btc_av_source_execute_service(BOOLEAN b_enable);
extern bt_status_t btc_av_sink_execute_service(BOOLEAN b_enable);
#endif
#if BTC_HF_INCLUDED
extern bt_status_t btc_hf_execute_service(BOOLEAN b_enable);
#endif
#if BTC_HF_CLIENT_INCLUDED
extern bt_status_t btc_hf_client_execute_service(BOOLEAN b_enable);
#endif
/******************************************************************************
**  Functions
******************************************************************************/
static void btc_dm_sec_arg_deep_free(btc_msg_t *msg)
{
    btc_dm_sec_args_t *arg = (btc_dm_sec_args_t *)(msg->arg);
    if (msg->act == BTA_DM_BLE_KEY_EVT) {
        osi_free(arg->sec.ble_key.p_key_value);
    }
}

void btc_dm_sec_arg_deep_copy(btc_msg_t *msg, void *dst, void *src)
{
    tBTA_DM_SEC *dst_dm_sec = (tBTA_DM_SEC *)dst;
    tBTA_DM_SEC *src_dm_sec = (tBTA_DM_SEC *)src;

    if (!src_dm_sec) {
        return;
    }

    assert(dst_dm_sec);
    memcpy(dst_dm_sec, src_dm_sec, sizeof(tBTA_DM_SEC));

    if (msg->act == BTA_DM_BLE_KEY_EVT) {
        dst_dm_sec->ble_key.p_key_value = osi_malloc(sizeof(tBTM_LE_KEY_VALUE));
        assert(src_dm_sec->ble_key.p_key_value);
        assert(dst_dm_sec->ble_key.p_key_value);
        memcpy(dst_dm_sec->ble_key.p_key_value, src_dm_sec->ble_key.p_key_value, sizeof(tBTM_LE_KEY_VALUE));
    }
}

/*******************************************************************************
**
** Function         btc_dm_evt
**
** Description      Switches context from BTE to BTC for all DM events
**
** Returns          void
**
*******************************************************************************/

void btc_dm_sec_evt(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *data)
{
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_DM_SEC;
    msg.act = event;

    btc_transfer_context(&msg, (btc_dm_sec_args_t *)data, sizeof(btc_dm_sec_args_t), btc_dm_sec_arg_deep_copy);
}

static void btc_enable_bluetooth_evt(tBTA_STATUS status)
{
    if (status == BTA_SUCCESS) {
        future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_SUCCESS);
    } else {
        future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_FAIL);
    }
}

static void btc_disable_bluetooth_evt(void)
{
    BTC_TRACE_DEBUG("%s", __FUNCTION__);

    future_ready(*btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE), FUTURE_SUCCESS);
}

#if (SMP_INCLUDED == TRUE)
#if (BLE_INCLUDED == TRUE)
void btc_dm_load_ble_local_keys(void)
{
    memset(&btc_dm_cb.ble_local_key_cb, 0, sizeof(btc_dm_local_key_cb_t));

    if (btc_storage_get_ble_local_key(BTC_LE_LOCAL_KEY_ER,(char*)&btc_dm_cb.ble_local_key_cb.er[0],
                                      BT_OCTET16_LEN)== BT_STATUS_SUCCESS) {
        btc_dm_cb.ble_local_key_cb.is_er_rcvd = TRUE;
        BTC_TRACE_DEBUG("%s BLE ER key loaded",__func__ );
    }

    if ((btc_storage_get_ble_local_key(BTC_LE_LOCAL_KEY_IR,(char*)&btc_dm_cb.ble_local_key_cb.id_keys.ir[0],
                                       BT_OCTET16_LEN)== BT_STATUS_SUCCESS )&&
            (btc_storage_get_ble_local_key(BTC_LE_LOCAL_KEY_IRK, (char*)&btc_dm_cb.ble_local_key_cb.id_keys.irk[0],
                                           BT_OCTET16_LEN)== BT_STATUS_SUCCESS)&&
            (btc_storage_get_ble_local_key(BTC_LE_LOCAL_KEY_DHK,(char*)&btc_dm_cb.ble_local_key_cb.id_keys.dhk[0],
                                           BT_OCTET16_LEN)== BT_STATUS_SUCCESS)) {
        btc_dm_cb.ble_local_key_cb.is_id_keys_rcvd = TRUE;
        BTC_TRACE_DEBUG("%s BLE ID keys loaded", __func__);
    }

}
void btc_dm_get_ble_local_keys(tBTA_DM_BLE_LOCAL_KEY_MASK *p_key_mask, BT_OCTET16 er,
                               tBTA_BLE_LOCAL_ID_KEYS *p_id_keys)
{
    if (btc_dm_cb.ble_local_key_cb.is_er_rcvd ) {
        memcpy(&er[0], &btc_dm_cb.ble_local_key_cb.er[0], sizeof(BT_OCTET16));
        *p_key_mask |= BTA_BLE_LOCAL_KEY_TYPE_ER;
    }

    if (btc_dm_cb.ble_local_key_cb.is_id_keys_rcvd) {
        memcpy(&p_id_keys->ir[0], &btc_dm_cb.ble_local_key_cb.id_keys.ir[0], sizeof(BT_OCTET16));
        memcpy(&p_id_keys->irk[0],  &btc_dm_cb.ble_local_key_cb.id_keys.irk[0], sizeof(BT_OCTET16));
        memcpy(&p_id_keys->dhk[0],  &btc_dm_cb.ble_local_key_cb.id_keys.dhk[0], sizeof(BT_OCTET16));
        *p_key_mask |= BTA_BLE_LOCAL_KEY_TYPE_ID;
    }
    BTC_TRACE_DEBUG("%s  *p_key_mask=0x%02x",__func__,   *p_key_mask);
}


static void btc_dm_remove_ble_bonding_keys(void)
{
    bt_bdaddr_t bd_addr;
    BTC_TRACE_DEBUG("%s\n",__func__);

    bdcpy(bd_addr.address, btc_dm_cb.pairing_cb.bd_addr);

    btc_storage_remove_remote_addr_type(&bd_addr, false);
    btc_storage_remove_ble_dev_auth_mode(&bd_addr, false);
    btc_storage_remove_ble_dev_type(&bd_addr, false);
    btc_storage_remove_ble_bonding_keys(&bd_addr);
}

static void btc_dm_save_ble_bonding_keys(void)
{
    if (!(btc_dm_cb.pairing_cb.ble.is_penc_key_rcvd || btc_dm_cb.pairing_cb.ble.is_pid_key_rcvd || btc_dm_cb.pairing_cb.ble.is_pcsrk_key_rcvd ||
         btc_dm_cb.pairing_cb.ble.is_lenc_key_rcvd || btc_dm_cb.pairing_cb.ble.is_lcsrk_key_rcvd || btc_dm_cb.pairing_cb.ble.is_lidk_key_rcvd)) {
        return ;
    }
    bt_bdaddr_t bd_addr;

    bdcpy(bd_addr.address, btc_dm_cb.pairing_cb.bd_addr);

    btc_storage_set_ble_dev_type(&bd_addr, false);
    BTC_TRACE_DEBUG("%s, penc = %d, pid = %d", __func__, btc_dm_cb.pairing_cb.ble.is_penc_key_rcvd, btc_dm_cb.pairing_cb.ble.is_pid_key_rcvd);
    if (btc_dm_cb.pairing_cb.ble.is_penc_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &btc_dm_cb.pairing_cb.ble.penc_key,
                                        BTM_LE_KEY_PENC,
                                        sizeof(tBTM_LE_PENC_KEYS));
        btc_dm_cb.pairing_cb.ble.is_penc_key_rcvd = false;
    }

    if (btc_dm_cb.pairing_cb.ble.is_pid_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &btc_dm_cb.pairing_cb.ble.pid_key,
                                        BTM_LE_KEY_PID,
                                        sizeof(tBTM_LE_PID_KEYS));
        btc_dm_cb.pairing_cb.ble.is_pid_key_rcvd = false;
    }


    if (btc_dm_cb.pairing_cb.ble.is_pcsrk_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &btc_dm_cb.pairing_cb.ble.pcsrk_key,
                                        BTM_LE_KEY_PCSRK,
                                        sizeof(tBTM_LE_PCSRK_KEYS));
        btc_dm_cb.pairing_cb.ble.is_pcsrk_key_rcvd = false;
    }


    if (btc_dm_cb.pairing_cb.ble.is_lenc_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &btc_dm_cb.pairing_cb.ble.lenc_key,
                                        BTM_LE_KEY_LENC,
                                        sizeof(tBTM_LE_LENC_KEYS));
        btc_dm_cb.pairing_cb.ble.is_lenc_key_rcvd = false;
    }

    if (btc_dm_cb.pairing_cb.ble.is_lcsrk_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &btc_dm_cb.pairing_cb.ble.lcsrk_key,
                                        BTM_LE_KEY_LCSRK,
                                        sizeof(tBTM_LE_LCSRK_KEYS));
        btc_dm_cb.pairing_cb.ble.is_lcsrk_key_rcvd = false;
    }

    if (btc_dm_cb.pairing_cb.ble.is_lidk_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        NULL,
                                        BTM_LE_KEY_LID,
                                        0);
        btc_dm_cb.pairing_cb.ble.is_lidk_key_rcvd = false;
    }
}

static void btc_dm_ble_auth_cmpl_evt (tBTA_DM_AUTH_CMPL *p_auth_cmpl)
{
    /* Save link key, if not temporary */
    BTC_TRACE_DEBUG("%s, status = %d", __func__, p_auth_cmpl->success);
    bt_status_t status = BT_STATUS_FAIL;
    int addr_type;
    bt_bdaddr_t bdaddr;
    bdcpy(bdaddr.address, p_auth_cmpl->bd_addr);
    bdcpy(btc_dm_cb.pairing_cb.bd_addr, p_auth_cmpl->bd_addr);

    if (p_auth_cmpl->success) {
        status = BT_STATUS_SUCCESS;
        BTC_TRACE_DEBUG ("%s, -  p_auth_cmpl->bd_addr: %08x%04x", __func__,
                             (p_auth_cmpl->bd_addr[0] << 24) + (p_auth_cmpl->bd_addr[1] << 16) + (p_auth_cmpl->bd_addr[2] << 8) + p_auth_cmpl->bd_addr[3],
                             (p_auth_cmpl->bd_addr[4] << 8) + p_auth_cmpl->bd_addr[5]);

        // Check if need to save BLE keys
        if((p_auth_cmpl->auth_mode & SMP_AUTH_GEN_BOND) == 0) {
            return;
        }

         if (btc_storage_get_remote_addr_type(&bdaddr, &addr_type) != BT_STATUS_SUCCESS) {
            btc_storage_set_remote_addr_type(&bdaddr, p_auth_cmpl->addr_type, true);
        }
        btc_storage_set_ble_dev_auth_mode(&bdaddr, p_auth_cmpl->auth_mode, true);
        btc_dm_save_ble_bonding_keys();
    } else {
        /*Map the HCI fail reason  to  bt status  */
        switch (p_auth_cmpl->fail_reason) {
        case BTA_DM_AUTH_SMP_PAIR_AUTH_FAIL:
        case BTA_DM_AUTH_SMP_CONFIRM_VALUE_FAIL:
            btc_dm_remove_ble_bonding_keys();
            status = BT_STATUS_AUTH_FAILURE;
            break;
        case BTA_DM_AUTH_SMP_PAIR_NOT_SUPPORT:
            status = BT_STATUS_AUTH_REJECTED;
            break;
        default:
            btc_dm_remove_ble_bonding_keys();
            status =  BT_STATUS_FAIL;
            break;
        }

    }

#if (CONFIG_BT_STACK_NO_LOG)
    (void) status;
#endif
    BTC_TRACE_DEBUG("%s, authentication status = %x", __func__, status);
    return;

}
#endif  ///BLE_INCLUDED == TRUE
#endif ///SMP_INCLUDED == TRUE

static void btc_dm_auth_cmpl_evt (tBTA_DM_AUTH_CMPL *p_auth_cmpl)
{
    /* Save link key, if not temporary */
    bt_bdaddr_t bd_addr;
    bt_status_t status;
    BTC_TRACE_DEBUG("%s: bond state success %d, present %d, type%d\n", __func__, p_auth_cmpl->success,
              p_auth_cmpl->key_present, p_auth_cmpl->key_type);

    bdcpy(bd_addr.address, p_auth_cmpl->bd_addr);
    if ( (p_auth_cmpl->success == TRUE) && (p_auth_cmpl->key_present) ) {
#if 0
        if ((p_auth_cmpl->key_type < HCI_LKEY_TYPE_DEBUG_COMB) ||
                (p_auth_cmpl->key_type == HCI_LKEY_TYPE_AUTH_COMB) ||
                (p_auth_cmpl->key_type == HCI_LKEY_TYPE_CHANGED_COMB) ||
                (p_auth_cmpl->key_type == HCI_LKEY_TYPE_AUTH_COMB_P_256)
           )
#endif
            if (1) {
                bt_status_t ret;
                BTC_TRACE_DEBUG("%s: Storing link key. key_type=0x%x",
                          __FUNCTION__, p_auth_cmpl->key_type);
                ret = btc_storage_add_bonded_device(&bd_addr,
                                                    p_auth_cmpl->key, p_auth_cmpl->key_type,
                                                    16);
                BTC_ASSERTC(ret == BT_STATUS_SUCCESS, "storing link key failed", ret);
            } else {
                BTC_TRACE_DEBUG("%s: Temporary key. Not storing. key_type=0x%x",
                          __FUNCTION__, p_auth_cmpl->key_type);
            }
    }

    // Skip SDP for certain  HID Devices
    if (p_auth_cmpl->success) {
        status = BT_STATUS_SUCCESS;
    } else {
        // Map the HCI fail reason  to  bt status
        switch (p_auth_cmpl->fail_reason) {
        case HCI_ERR_PAGE_TIMEOUT:
            BTC_TRACE_WARNING("%s() - Pairing timeout; retrying () ...", __FUNCTION__);
            return;
        /* Fall-through */
        case HCI_ERR_CONNECTION_TOUT:
            status =  BT_STATUS_RMT_DEV_DOWN;
            break;

        case HCI_ERR_PAIRING_NOT_ALLOWED:
            status = BT_STATUS_AUTH_REJECTED;
            break;

        case HCI_ERR_LMP_RESPONSE_TIMEOUT:
            status =  BT_STATUS_AUTH_FAILURE;
            break;

        /* map the auth failure codes, so we can retry pairing if necessary */
        case HCI_ERR_AUTH_FAILURE:
        case HCI_ERR_KEY_MISSING:
            btc_storage_remove_bonded_device(&bd_addr);
        case HCI_ERR_HOST_REJECT_SECURITY:
        case HCI_ERR_ENCRY_MODE_NOT_ACCEPTABLE:
        case HCI_ERR_UNIT_KEY_USED:
        case HCI_ERR_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED:
        case HCI_ERR_INSUFFCIENT_SECURITY:
        case HCI_ERR_PEER_USER:
        case HCI_ERR_UNSPECIFIED:
            BTC_TRACE_ERROR(" %s() Authentication fail reason %d",
                      __FUNCTION__, p_auth_cmpl->fail_reason);
            /* if autopair attempts are more than 1, or not attempted */
            status =  BT_STATUS_AUTH_FAILURE;
            break;
        default:
            status =  BT_STATUS_FAIL;
        }
    }
#if (BTC_GAP_BT_INCLUDED == TRUE)
    esp_bt_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_AUTH_CMPL_EVT;
    param.auth_cmpl.stat = status;
    memcpy(param.auth_cmpl.bda, p_auth_cmpl->bd_addr, ESP_BD_ADDR_LEN);
    memcpy(param.auth_cmpl.device_name, p_auth_cmpl->bd_name, ESP_BT_GAP_MAX_BDNAME_LEN + 1);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_bt_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }

#endif /// BTC_GAP_BT_INCLUDED == TRUE
    (void) status;
}

static void btc_dm_pin_req_evt(tBTA_DM_PIN_REQ *p_pin_req)
{
#if (BTC_GAP_BT_INCLUDED == TRUE)
    esp_bt_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_PIN_REQ_EVT;
    param.pin_req.min_16_digit = p_pin_req->min_16_digit;
    memcpy(param.pin_req.bda, p_pin_req->bd_addr, ESP_BD_ADDR_LEN);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_bt_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
#endif /// BTC_GAP_BT_INCLUDED == TRUE
}

#if (BT_SSP_INCLUDED == TRUE)
static void btc_dm_sp_cfm_req_evt(tBTA_DM_SP_CFM_REQ *p_cfm_req)
{
#if (BTC_GAP_BT_INCLUDED == TRUE)
    if (p_cfm_req->just_works) {
        // just work, not show to users.
        BTA_DmConfirm(p_cfm_req->bd_addr, true);
        return;
    }

    esp_bt_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_CFM_REQ_EVT;
    param.cfm_req.num_val = p_cfm_req->num_val;
    memcpy(param.cfm_req.bda, p_cfm_req->bd_addr, ESP_BD_ADDR_LEN);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_bt_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
#endif /// BTC_GAP_BT_INCLUDED == TRUE
}

static void btc_dm_sp_key_notif_evt(tBTA_DM_SP_KEY_NOTIF *p_key_notif)
{
#if (BTC_GAP_BT_INCLUDED == TRUE)
    esp_bt_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_KEY_NOTIF_EVT;
    param.key_notif.passkey = p_key_notif->passkey;
    memcpy(param.key_notif.bda, p_key_notif->bd_addr, ESP_BD_ADDR_LEN);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_bt_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
#endif /// BTC_GAP_BT_INCLUDED == TRUE
}

static void btc_dm_sp_key_req_evt(tBTA_DM_SP_KEY_REQ *p_key_req)
{
#if (BTC_GAP_BT_INCLUDED == TRUE)
    esp_bt_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BT;
    msg.act = BTC_GAP_BT_KEY_REQ_EVT;
    memcpy(param.key_req.bda, p_key_req->bd_addr, ESP_BD_ADDR_LEN);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_bt_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
#endif /// BTC_GAP_BT_INCLUDED == TRUE
}
#endif /// BT_SSP_INCLUDED == TRUE

tBTA_SERVICE_MASK btc_get_enabled_services_mask(void)
{
    return btc_dm_cb.btc_enabled_services;
}

void btc_clear_services_mask(void)
{
    btc_dm_cb.btc_enabled_services = 0;
}

static bt_status_t btc_in_execute_service_request(tBTA_SERVICE_ID service_id,
        BOOLEAN b_enable)
{
    BTC_TRACE_DEBUG("%s service_id: %d\n", __FUNCTION__, service_id);
    /* Check the service_ID and invoke the profile's BT state changed API */
    switch (service_id) {
#if BTC_AV_INCLUDED
    case BTA_A2DP_SOURCE_SERVICE_ID:
        btc_av_source_execute_service(b_enable);
        break;
    case BTA_A2DP_SINK_SERVICE_ID:
        btc_av_sink_execute_service(b_enable);
        break;
#endif
#if BTC_HF_INCLUDED
    case BTA_HFP_SERVICE_ID:
        btc_hf_execute_service(b_enable);
        break;
#endif /* #if BTC_HF_INCLUDED */
#if BTC_HF_CLIENT_INCLUDED
    case BTA_HFP_HS_SERVICE_ID:
        btc_hf_client_execute_service(b_enable);
        break;
#endif /* #if BTC_HF_CLIENT_INCLUDED */
    default:
        BTC_TRACE_ERROR("%s: Unknown service being enabled\n", __FUNCTION__);
        return BT_STATUS_FAIL;
    }
    return BT_STATUS_SUCCESS;
}

void btc_dm_execute_service_request(BOOLEAN enable, char *p_param)
{
    btc_in_execute_service_request(*((tBTA_SERVICE_ID *)p_param), enable);
}

bt_status_t btc_dm_enable_service(tBTA_SERVICE_ID service_id)
{
    tBTA_SERVICE_ID *p_id = &service_id;

    btc_dm_cb.btc_enabled_services |= (1 << service_id);

    BTC_TRACE_DEBUG("%s: current services:0x%x", __FUNCTION__, btc_dm_cb.btc_enabled_services);

    btc_dm_execute_service_request(TRUE, (char *)p_id);

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_dm_disable_service(tBTA_SERVICE_ID service_id)
{
    tBTA_SERVICE_ID *p_id = &service_id;

    btc_dm_cb.btc_enabled_services &= (tBTA_SERVICE_MASK)(~(1 << service_id));

    BTC_TRACE_DEBUG("%s: Current Services:0x%x", __FUNCTION__, btc_dm_cb.btc_enabled_services);

    btc_dm_execute_service_request(FALSE, (char *)p_id);

    return BT_STATUS_SUCCESS;
}

void btc_dm_sec_cb_handler(btc_msg_t *msg)
{
    btc_dm_sec_args_t *arg = (btc_dm_sec_args_t *)(msg->arg);
    tBTA_DM_SEC *p_data = &(arg->sec);
    esp_ble_gap_cb_param_t param = {0};
    btc_msg_t ble_msg = {0};
    bool rsp_app = false;
    bt_status_t ret = BT_STATUS_SUCCESS;
    ble_msg.sig = BTC_SIG_API_CB;
    ble_msg.pid = BTC_PID_GAP_BLE;
    // tBTA_SERVICE_MASK service_mask;
    BTC_TRACE_DEBUG("btc_dm_upstreams_cback  ev: %d\n", msg->act);

    switch (msg->act) {
    case BTA_DM_ENABLE_EVT: {
        btc_clear_services_mask();
#if (SMP_INCLUDED == TRUE)
        btc_storage_load_bonded_devices();
#if (BLE_INCLUDED == TRUE)
        //load the bonding device to the btm layer
        btc_storage_load_bonded_ble_devices();
#endif  ///BLE_INCLUDED == TRUE
#endif  ///SMP_INCLUDED == TRUE

        /* Set initial device name, it can be overwritten later */
        if (p_data->enable.status == BTA_SUCCESS) {
            const char *initial_device_name = "ESP32";
            BTA_DmSetDeviceName(initial_device_name);
        }
        btc_enable_bluetooth_evt(p_data->enable.status);
        break;
    }
    case BTA_DM_DISABLE_EVT: {
        tBTA_SERVICE_MASK service_mask = btc_get_enabled_services_mask();
        for (int i = 0; i <= BTA_MAX_SERVICE_ID; i++) {
            if (service_mask &
                    (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(i))) {
                btc_in_execute_service_request(i, FALSE);
            }
        }
        btc_disable_bluetooth_evt();
        break;
    }
    case BTA_DM_PIN_REQ_EVT:
        BTC_TRACE_DEBUG("BTA_DM_PIN_REQ_EVT");
        btc_dm_pin_req_evt(&p_data->pin_req);
        break;
    case BTA_DM_AUTH_CMPL_EVT:
        btc_dm_auth_cmpl_evt(&p_data->auth_cmpl);
        break;
    case BTA_DM_BOND_CANCEL_CMPL_EVT:
        BTC_TRACE_DEBUG("BTA_DM_BOND_CANCEL_CMPL_EVT");
        break;
#if (BT_SSP_INCLUDED == TRUE)
    case BTA_DM_SP_CFM_REQ_EVT:
        btc_dm_sp_cfm_req_evt(&p_data->cfm_req);
        break;
    case BTA_DM_SP_KEY_NOTIF_EVT:
        btc_dm_sp_key_notif_evt(&p_data->key_notif);
        break;
    case BTA_DM_SP_KEY_REQ_EVT:
        btc_dm_sp_key_req_evt(&p_data->key_req);
        break;
    case BTA_DM_SP_RMT_OOB_EVT:
        BTC_TRACE_DEBUG("BTA_DM_SP_RMT_OOB_EVT");
        break;
    case BTA_DM_SP_KEYPRESS_EVT:
        BTC_TRACE_DEBUG("BTA_DM_SP_KEYPRESS_EVT");
        break;
#endif ///BT_SSP_INCLUDED == TRUE

    case BTA_DM_DEV_UNPAIRED_EVT: {
#if (SMP_INCLUDED == TRUE)
        bt_bdaddr_t bd_addr;
        BTC_TRACE_DEBUG("BTA_DM_DEV_UNPAIRED_EVT");
        memcpy(bd_addr.address, p_data->link_down.bd_addr, sizeof(BD_ADDR));
        btm_set_bond_type_dev(p_data->link_down.bd_addr, BOND_TYPE_UNKNOWN);
        if (p_data->link_down.status == HCI_SUCCESS) {
            //remove the bonded key in the config and nvs flash.
            btc_storage_remove_bonded_device(&bd_addr);
        }
#endif /* #if (SMP_INCLUDED == TRUE) */
        break;
    }
#if (BLE_INCLUDED == TRUE)
    case BTA_DM_BLE_DEV_UNPAIRED_EVT: {
#if (SMP_INCLUDED == TRUE)
        bt_bdaddr_t bd_addr;
        rsp_app = true;
        BTC_TRACE_DEBUG("BTA_DM_BLE_DEV_UNPAIRED_EVT");
        memcpy(bd_addr.address, p_data->link_down.bd_addr, sizeof(BD_ADDR));
        btm_set_bond_type_dev(p_data->link_down.bd_addr, BOND_TYPE_UNKNOWN);
        param.remove_bond_dev_cmpl.status = ESP_BT_STATUS_FAIL;

        if (p_data->link_down.status == HCI_SUCCESS) {
            //remove the bonded key in the config and nvs flash.
            btc_storage_remove_ble_dev_type(&bd_addr, false);
            btc_storage_remove_remote_addr_type(&bd_addr, false);
            btc_storage_remove_ble_dev_auth_mode(&bd_addr, false);
            param.remove_bond_dev_cmpl.status = btc_storage_remove_ble_bonding_keys(&bd_addr);
        }
        ble_msg.act = ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT;
        memcpy(param.remove_bond_dev_cmpl.bd_addr, p_data->link_down.bd_addr, sizeof(BD_ADDR));
#endif /* #if (SMP_INCLUDED == TRUE) */
        break;
    }
#endif  ///BLE_INCLUDED == TRUE
    case BTA_DM_BUSY_LEVEL_EVT:
#if (BTC_GAP_BT_INCLUDED == TRUE)
        {
        if (p_data->busy_level.level_flags & BTM_BL_INQUIRY_PAGING_MASK) {
            btc_gap_bt_busy_level_updated(p_data->busy_level.level_flags);
        }
        break;
        }
#endif /* BTC_GAP_BT_INCLUDED  == TRUE */
    case BTA_DM_LINK_UP_EVT:
    case BTA_DM_LINK_DOWN_EVT:
    case BTA_DM_HW_ERROR_EVT:
        BTC_TRACE_DEBUG( "btc_dm_sec_cback : unhandled event (%d)\n", msg->act );
        break;
#if ((BLE_INCLUDED == TRUE) && (SMP_INCLUDED == TRUE))
    case BTA_DM_BLE_AUTH_CMPL_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_AUTH_CMPL_EVT;
        param.ble_security.auth_cmpl.addr_type = p_data->auth_cmpl.addr_type;
        param.ble_security.auth_cmpl.dev_type = p_data->auth_cmpl.dev_type;
        param.ble_security.auth_cmpl.key_type = p_data->auth_cmpl.key_type;
        param.ble_security.auth_cmpl.fail_reason = p_data->auth_cmpl.fail_reason;
        param.ble_security.auth_cmpl.success = p_data->auth_cmpl.success ? true : false;
        param.ble_security.auth_cmpl.key_present = p_data->auth_cmpl.key_present;
        memcpy(param.ble_security.auth_cmpl.bd_addr, p_data->auth_cmpl.bd_addr, sizeof(BD_ADDR));
        memcpy(param.ble_security.auth_cmpl.key, p_data->auth_cmpl.key, sizeof(LINK_KEY));
        param.ble_security.auth_cmpl.auth_mode = p_data->auth_cmpl.auth_mode;
        btc_dm_ble_auth_cmpl_evt(&p_data->auth_cmpl);
        break;
    }
    case BTA_DM_BLE_KEY_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_KEY_EVT;
        param.ble_security.ble_key.key_type = p_data->ble_key.key_type;
        memcpy(param.ble_security.ble_key.bd_addr, p_data->ble_key.bd_addr, BD_ADDR_LEN);
        switch (p_data->ble_key.key_type) {
            case BTM_LE_KEY_PENC: {
                BTC_TRACE_DEBUG("Rcv BTA_LE_KEY_PENC");
                btc_dm_cb.pairing_cb.ble.is_penc_key_rcvd = TRUE;
                btc_dm_cb.pairing_cb.ble.penc_key = p_data->ble_key.p_key_value->penc_key;
                memcpy(&btc_dm_cb.pairing_cb.ble.penc_key, &p_data->ble_key.p_key_value->penc_key,
                             sizeof(tBTM_LE_PENC_KEYS));
                memcpy(&param.ble_security.ble_key.p_key_value.penc_key,
                             &p_data->ble_key.p_key_value->penc_key, sizeof(tBTM_LE_PENC_KEYS));
                break;
            }
            case BTM_LE_KEY_PID: {
                BTC_TRACE_DEBUG("Rcv BTA_LE_KEY_PID");
                btc_dm_cb.pairing_cb.ble.is_pid_key_rcvd = TRUE;
                memcpy(&btc_dm_cb.pairing_cb.ble.pid_key, &p_data->ble_key.p_key_value->pid_key,
                            sizeof(tBTM_LE_PID_KEYS));
                memcpy(&param.ble_security.ble_key.p_key_value.pid_key,
                             &p_data->ble_key.p_key_value->pid_key, sizeof(tBTM_LE_PID_KEYS));
                break;
            }
            case BTM_LE_KEY_PCSRK: {
                BTC_TRACE_DEBUG("Rcv BTA_LE_KEY_PCSRK");
                btc_dm_cb.pairing_cb.ble.is_pcsrk_key_rcvd = TRUE;
                memcpy(&btc_dm_cb.pairing_cb.ble.pcsrk_key, &p_data->ble_key.p_key_value->pcsrk_key,
                             sizeof(tBTM_LE_PCSRK_KEYS));
                memcpy(&param.ble_security.ble_key.p_key_value.pcsrk_key,
                             &p_data->ble_key.p_key_value->pcsrk_key, sizeof(tBTM_LE_PCSRK_KEYS));
                break;
            }
            case BTM_LE_KEY_LENC: {
                BTC_TRACE_DEBUG("Rcv BTA_LE_KEY_LENC");
                btc_dm_cb.pairing_cb.ble.is_lenc_key_rcvd = TRUE;
                memcpy(&btc_dm_cb.pairing_cb.ble.lenc_key, &p_data->ble_key.p_key_value->lenc_key,
                            sizeof(tBTM_LE_LENC_KEYS));
                memcpy(&param.ble_security.ble_key.p_key_value.lenc_key,
                             &p_data->ble_key.p_key_value->lenc_key, sizeof(tBTM_LE_LENC_KEYS));
                break;
            }
            case BTM_LE_KEY_LCSRK: {
                BTC_TRACE_DEBUG("Rcv BTA_LE_KEY_LCSRK");
                btc_dm_cb.pairing_cb.ble.is_lcsrk_key_rcvd = TRUE;
                memcpy(&btc_dm_cb.pairing_cb.ble.lcsrk_key, &p_data->ble_key.p_key_value->lcsrk_key,
                            sizeof(tBTM_LE_LCSRK_KEYS));
                memcpy(&param.ble_security.ble_key.p_key_value.lcsrk_key,
                             &p_data->ble_key.p_key_value->lcsrk_key, sizeof(tBTM_LE_LCSRK_KEYS));
                break;
            }
            case BTM_LE_KEY_LID: {
                BTC_TRACE_DEBUG("Rcv BTA_LE_KEY_LID");
                btc_dm_cb.pairing_cb.ble.is_lidk_key_rcvd =  TRUE;
                break;
            }
            default:
                break;
        }

        break;
    }
    case BTA_DM_BLE_SEC_REQ_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_SEC_REQ_EVT;
        memcpy(param.ble_security.ble_req.bd_addr, p_data->ble_req.bd_addr, BD_ADDR_LEN);
        break;
    }
    case BTA_DM_BLE_PASSKEY_NOTIF_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_PASSKEY_NOTIF_EVT;
        param.ble_security.key_notif.passkey = p_data->key_notif.passkey;
        memcpy(param.ble_security.key_notif.bd_addr, p_data->ble_req.bd_addr, BD_ADDR_LEN);
        break;
    }
    case BTA_DM_BLE_PASSKEY_REQ_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_PASSKEY_REQ_EVT;
        memcpy(param.ble_security.ble_req.bd_addr, p_data->ble_req.bd_addr, BD_ADDR_LEN);
        break;
    }
    case BTA_DM_BLE_OOB_REQ_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_OOB_REQ_EVT;
        memcpy(param.ble_security.ble_req.bd_addr, p_data->ble_req.bd_addr, BD_ADDR_LEN);
        break;
    }
    case BTA_DM_BLE_LOCAL_IR_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_LOCAL_IR_EVT;
        memcpy(&param.ble_security.ble_id_keys, &p_data->ble_id_keys, sizeof(tBTA_BLE_LOCAL_ID_KEYS));
        BTC_TRACE_DEBUG("BTA_DM_BLE_LOCAL_IR_EVT. ");
        btc_dm_cb.ble_local_key_cb.is_id_keys_rcvd = TRUE;
        memcpy(&btc_dm_cb.ble_local_key_cb.id_keys.irk[0],
               &p_data->ble_id_keys.irk[0], sizeof(BT_OCTET16));
        memcpy(&btc_dm_cb.ble_local_key_cb.id_keys.ir[0],
               &p_data->ble_id_keys.ir[0], sizeof(BT_OCTET16));
        memcpy(&btc_dm_cb.ble_local_key_cb.id_keys.dhk[0],
               &p_data->ble_id_keys.dhk[0], sizeof(BT_OCTET16));
        btc_storage_add_ble_local_key( (char *)&btc_dm_cb.ble_local_key_cb.id_keys.irk[0],
                                       BTC_LE_LOCAL_KEY_IRK,
                                       BT_OCTET16_LEN);
        btc_storage_add_ble_local_key( (char *)&btc_dm_cb.ble_local_key_cb.id_keys.ir[0],
                                       BTC_LE_LOCAL_KEY_IR,
                                       BT_OCTET16_LEN);
        btc_storage_add_ble_local_key( (char *)&btc_dm_cb.ble_local_key_cb.id_keys.dhk[0],
                                       BTC_LE_LOCAL_KEY_DHK,
                                       BT_OCTET16_LEN);
        break;
    }
    case BTA_DM_BLE_LOCAL_ER_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_LOCAL_ER_EVT;
        memcpy(&param.ble_security.ble_id_keys, &p_data->ble_id_keys, sizeof(tBTA_BLE_LOCAL_ID_KEYS));
        BTC_TRACE_DEBUG("BTA_DM_BLE_LOCAL_ER_EVT. ");
        btc_dm_cb.ble_local_key_cb.is_er_rcvd = TRUE;
        memcpy(&btc_dm_cb.ble_local_key_cb.er[0], &p_data->ble_er[0], sizeof(BT_OCTET16));
        btc_storage_add_ble_local_key( (char *)&btc_dm_cb.ble_local_key_cb.er[0],
                                       BTC_LE_LOCAL_KEY_ER,
                                       BT_OCTET16_LEN);
        break;
    }
    case BTA_DM_BLE_NC_REQ_EVT: {
        rsp_app = true;
        ble_msg.act = ESP_GAP_BLE_NC_REQ_EVT;
        memcpy(param.ble_security.key_notif.bd_addr, p_data->key_notif.bd_addr, BD_ADDR_LEN);
        param.ble_security.key_notif.passkey = p_data->key_notif.passkey;
        break;
    }
#endif

    case BTA_DM_AUTHORIZE_EVT:
    case BTA_DM_SIG_STRENGTH_EVT:
    case BTA_DM_ROLE_CHG_EVT:
        BTC_TRACE_DEBUG( "btc_dm_sec_cback : unhandled event (%d)\n", msg->act );
        break;
    default:
        BTC_TRACE_DEBUG( "btc_dm_sec_cback : unhandled event (%d)\n", msg->act );
        break;
    }

    if (rsp_app) {
        ret = btc_transfer_context(&ble_msg, &param,
                                   sizeof(esp_ble_gap_cb_param_t), NULL);

        if (ret != BT_STATUS_SUCCESS) {
            BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
        }
    }
    btc_dm_sec_arg_deep_free(msg);
}
