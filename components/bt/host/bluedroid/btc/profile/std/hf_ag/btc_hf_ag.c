/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/************************************************************************************
 *
 *  Filename:      btc_hf.c
 *
 *  Description:   Handsfree Profile Bluetooth Interface
 * *
 ***********************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "time.h"
#include "btc/btc_dm.h"
#include "btc_hf_ag.h"
#include "btc/btc_profile_queue.h"
#include "btc/btc_manage.h"
#include "btc/btc_util.h"
#include "btc/btc_common.h"
#include "bta/bta_ag_api.h"
#include "bt_common.h"
#include "common/bt_target.h"
#include "common/bt_trace.h"
#include "common/bt_defs.h"
#include "device/bdaddr.h"
#include "esp_bt.h"
#include "esp_hf_ag_api.h"
#include "osi/allocator.h"


#if (BTC_HF_INCLUDED == TRUE)
/************************************************************************************
**  Constants & Macros
************************************************************************************/
/* Max HF Clients Supported From App */
static UINT16 btc_max_hf_clients = 1;
/* HF Param Definition */
static hf_local_param_t hf_local_param[BTC_HF_NUM_CB];

#if (BTM_WBS_INCLUDED == TRUE)
#ifndef BTC_HF_FEATURES
#define BTC_HF_FEATURES    ( BTA_AG_FEAT_ECNR   | \
                             BTA_AG_FEAT_REJECT | \
                             BTA_AG_FEAT_ECS    | \
                             BTA_AG_FEAT_EXTERR | \
                             BTA_AG_FEAT_VREC   | \
                             BTA_AG_FEAT_INBAND | \
                             BTA_AG_FEAT_CODEC  | \
                             BTA_AG_FEAT_UNAT)
#endif
#else
#ifndef BTC_HF_FEATURES
#define BTC_HF_FEATURES    ( BTA_AG_FEAT_ECNR   | \
                             BTA_AG_FEAT_REJECT | \
                             BTA_AG_FEAT_ECS    | \
                             BTA_AG_FEAT_EXTERR | \
                             BTA_AG_FEAT_VREC   | \
                             BTA_AG_FEAT_INBAND | \
                             BTA_AG_FEAT_UNAT)
#endif
#endif

/* wide band synchronous */
#ifndef BTC_HF_WBS_PREFERRED
#define BTC_HF_WBS_PREFERRED   FALSE
#endif
BOOLEAN btc_conf_hf_force_wbs = BTC_HF_WBS_PREFERRED;

#define CHECK_HF_INIT() do { \
if (! hf_local_param[idx].btc_hf_cb.initialized) { \
BTIF_TRACE_WARNING("CHECK_HF_INIT: %s: HF AG not initialized", __FUNCTION__);\
return BT_STATUS_NOT_READY; \
} \
else\
{\
BTIF_TRACE_EVENT("CHECK_HF_INIT: %s", __FUNCTION__);\
}\
} while (0)

#define CHECK_HF_SLC_CONNECTED() do { \
if (! hf_local_param[idx].btc_hf_cb.initialized || \
    hf_local_param[idx].btc_hf_cb.connection_state != ESP_HF_CONNECTION_STATE_SLC_CONNECTED) { \
BTIF_TRACE_WARNING("CHECK_HF_INIT: %s: HF AG not initialized", __FUNCTION__);\
return BT_STATUS_NOT_READY; \
} \
else\
{\
BTIF_TRACE_EVENT("CHECK_HF_INIT: %s", __FUNCTION__);\
}\
} while (0)


#define clear_phone_state() \
    hf_local_param[idx].btc_hf_cb.call_state = ESP_HF_CALL_STATUS_NO_CALLS; \
    hf_local_param[idx].btc_hf_cb.call_setup_state = ESP_HF_CALL_SETUP_STATUS_IDLE;\
    hf_local_param[idx].btc_hf_cb.num_active = 0;  \
    hf_local_param[idx].btc_hf_cb.num_held = 0;

/************************************************************************************
**                                Static Function
************************************************************************************/
static int btc_hf_idx_by_bdaddr(bt_bdaddr_t *bd_addr)
{
    for (int i = 0; i < btc_max_hf_clients; ++i) {
        if ((bdcmp(bd_addr->address, hf_local_param[i].btc_hf_cb.connected_bda.address) == 0)
            || bd_addr->address) {
            return i;
        }
    }
    return BTC_HF_INVALID_IDX;
}

static BOOLEAN is_connected(bt_bdaddr_t *bd_addr)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    if (((hf_local_param[idx].btc_hf_cb.connection_state == ESP_HF_CONNECTION_STATE_CONNECTED) ||
            (hf_local_param[idx].btc_hf_cb.connection_state == ESP_HF_CONNECTION_STATE_SLC_CONNECTED)) &&
            ((bd_addr == NULL) || (bdcmp(bd_addr->address,hf_local_param[idx].btc_hf_cb.connected_bda.address) == 0))) {
        return TRUE;
    }
    return FALSE;
}

static int btc_hf_latest_connected_idx(void)
{
    struct timespec   now, conn_time_delta;
    int latest_conn_idx = BTC_HF_INVALID_IDX;
    clock_gettime(CLOCK_MONOTONIC, &now);
    conn_time_delta.tv_sec = now.tv_sec;

    for (int i = 0; i < btc_max_hf_clients; i++) {
        if (hf_local_param[i].btc_hf_cb.connection_state  == ESP_HF_CONNECTION_STATE_SLC_CONNECTED) {
            if ((now.tv_sec - hf_local_param[i].btc_hf_cb.connected_timestamp.tv_sec) < conn_time_delta.tv_sec) {
                conn_time_delta.tv_sec = now.tv_sec - hf_local_param[i].btc_hf_cb.connected_timestamp.tv_sec;
                latest_conn_idx = i;
            }
        }
    }
    return latest_conn_idx;
}

/************************************************************************************
**  Cb and Evt
************************************************************************************/
static inline void btc_hf_cb_to_app(esp_hf_cb_event_t event, esp_hf_cb_param_t *param)
{
    esp_hf_cb_t btc_hf_callbacks = (esp_hf_cb_t)btc_profile_cb_get(BTC_PID_HF);
    if (btc_hf_callbacks) {
        btc_hf_callbacks(event, param);
    }
}

static void send_indicator_update(UINT16 indicator, UINT16 value)
{
    tBTA_AG_RES_DATA ag_res;
    memset(&ag_res, 0, sizeof(tBTA_AG_RES_DATA));
    ag_res.ind.type = indicator;
    ag_res.ind.value = value;
    BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_IND_RES, &ag_res);
}

static void btc_hf_cind_evt(tBTA_AG_IND *ind)
{
    esp_hf_cb_param_t param;
    memset(&param, 0, sizeof(esp_hf_cb_param_t));

    switch (ind->type) {
        case BTA_AG_IND_CALL:
            param.cind.call_status = ind->value;
            break;
        case BTA_AG_IND_CALLSETUP:
            param.cind.call_setup_status = ind->value;
            break;
        case BTA_AG_IND_SERVICE:
            param.cind.svc = ind->value;
            break;
        case BTA_AG_IND_SIGNAL:
            param.cind.signal_strength = ind->value;
            break;
        case BTA_AG_IND_ROAM:
            param.cind.roam = ind->value;
            break;
        case BTA_AG_IND_BATTCHG:
            param.cind.battery_level = ind->value;
            break;
        case BTA_AG_IND_CALLHELD:
            param.cind.call_held_status = ind->value;
            break;
    }
    btc_hf_cb_to_app(ESP_HF_CIND_RESPONSE_EVT, &param);
}

static void bte_hf_evt(tBTA_AG_EVT event, tBTA_AG *param)
{
    int param_len = 0;
    /* TODO: BTA sends the union members and not tBTA_AG. If using param_len=sizeof(tBTA_AG), we get a crash on memcpy */
    if (BTA_AG_REGISTER_EVT == event) {
        param_len = sizeof(tBTA_AG_REGISTER);
    }
    else if (BTA_AG_OPEN_EVT == event) {
        param_len = sizeof(tBTA_AG_OPEN);
    }
    else if ((BTA_AG_CLOSE_EVT == event) || (BTA_AG_AUDIO_OPEN_EVT == event) || (BTA_AG_AUDIO_CLOSE_EVT == event)) {
        param_len = sizeof(tBTA_AG_HDR);
    }
    else if (BTA_AG_CONN_EVT == event) {
        param_len = sizeof(tBTA_AG_CONN);
    }
    else if (param) {
        param_len = sizeof(tBTA_AG_VAL);
    }
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HF;
    msg.act = event;

    /* Switch to BTC context */
    bt_status_t status = btc_transfer_context(&msg, param, param_len, NULL);
    /* catch any failed context transfers */
    BTC_ASSERTC(status == BT_STATUS_SUCCESS, "context transfer failed", status);
}

/************************************************************************************
**  Data flow control & Service management.
************************************************************************************/
void btc_hf_reg_data_cb(esp_hf_incoming_data_cb_t recv, esp_hf_outgoing_data_cb_t send)
{
    hf_local_param[0].btc_hf_incoming_data_cb = recv;
    hf_local_param[0].btc_hf_outgoing_data_cb = send;
}

void btc_hf_incoming_data_cb_to_app(const uint8_t *data, uint32_t len)
{
    int idx = 0;
    // todo: critical section protection
    if (hf_local_param[idx].btc_hf_incoming_data_cb) {
        hf_local_param[idx].btc_hf_incoming_data_cb(data, len);
    }
}

uint32_t btc_hf_outgoing_data_cb_to_app(uint8_t *data, uint32_t len)
{
    int idx = 0;
    // todo: critical section protection
    if (hf_local_param[idx].btc_hf_outgoing_data_cb) {
        return hf_local_param[idx].btc_hf_outgoing_data_cb(data, len);
    } else {
        return 0;
    }
}

bt_status_t btc_hf_execute_service(BOOLEAN b_enable)
{
    char * p_service_names[] = BTC_HF_SERVICE_NAMES;
    int idx;
    if (b_enable) {
        /* Enable and register with BTA-AG */
        BTA_AgEnable(BTA_AG_PARSE, bte_hf_evt);
        for (idx = 0; idx < btc_max_hf_clients; idx++) {
            BTA_AgRegister(BTC_HF_SERVICES, BTC_HF_SECURITY, BTC_HF_FEATURES, p_service_names, BTC_HF_ID_1);
        }
    } else {
        /* De-register AG */
        for (idx = 0; idx < btc_max_hf_clients; idx++) {
            BTA_AgDeregister(hf_local_param[idx].btc_hf_cb.handle);
        }
        /* Disable AG */
        BTA_AgDisable();
    }
    return BT_STATUS_SUCCESS;
}

/************************************************************************************
**  BTC HFP AG API FUNCTION 
************************************************************************************/
/************************************************************************************
**  Initialization and Connection Handle
************************************************************************************/
bt_status_t btc_hf_init(bt_bdaddr_t *bd_addr)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    BTC_TRACE_DEBUG("%s - max_hf_clients=%d", __func__, btc_max_hf_clients);
    /* Invoke the enable service API to the core to set the appropriate service_id
     * Internally, the HSP_SERVICE_ID shall also be enabled if HFP is enabled (phone)
     * othwerwise only HSP is enabled (tablet)*/
#if (defined(BTC_HF_SERVICES) && (BTC_HF_SERVICES & BTA_HFP_SERVICE_MASK))
    btc_dm_enable_service(BTA_HFP_SERVICE_ID);
#else
    btc_dm_enable_service(BTA_HSP_SERVICE_ID);
#endif
    clear_phone_state();
    memset(&hf_local_param[idx].btc_hf_cb, 0, sizeof(btc_hf_cb_t));
    // custom initialization here
    hf_local_param[idx].btc_hf_cb.initialized = true;
// set audio path
#if BTM_SCO_HCI_INCLUDED
    uint8_t data_path = ESP_SCO_DATA_PATH_HCI;
#else
    uint8_t data_path = ESP_SCO_DATA_PATH_PCM;
#endif
    esp_bredr_sco_datapath_set(data_path);
    return BT_STATUS_SUCCESS;
}

void btc_hf_deinit(bt_bdaddr_t *bd_addr)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    BTC_TRACE_EVENT("%s", __FUNCTION__);
    btc_dm_disable_service(BTA_HFP_SERVICE_ID);
    hf_local_param[idx].btc_hf_cb.initialized = false;
}

static bt_status_t connect_init(bt_bdaddr_t *bd_addr, uint16_t uuid)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_INIT();
    if (idx == btc_max_hf_clients) {
        return BT_STATUS_BUSY;
    }

    if (!is_connected(bd_addr)) {
        hf_local_param[idx].btc_hf_cb.connection_state  = ESP_HF_CONNECTION_STATE_CONNECTING;
        bdcpy(hf_local_param[idx].btc_hf_cb.connected_bda.address, bd_addr->address);
        BTA_AgOpen(hf_local_param[idx].btc_hf_cb.handle, hf_local_param[idx].btc_hf_cb.connected_bda.address, BTC_HF_SECURITY, BTC_HF_SERVICES); 
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_BUSY;
}
bt_status_t btc_hf_connect(bt_bdaddr_t *bd_addr)
{
    return btc_queue_connect(UUID_SERVCLASS_AG_HANDSFREE, bd_addr, connect_init);
}

bt_status_t btc_hf_disconnect(bt_bdaddr_t *bd_addr)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_INIT();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        BTA_AgClose(hf_local_param[idx].btc_hf_cb.handle);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

bt_status_t btc_hf_connect_audio(bt_bdaddr_t *bd_addr)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_SLC_CONNECTED();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        BTA_AgAudioOpen(hf_local_param[idx].btc_hf_cb.handle);
        /* Inform the application that the audio connection has been initiated successfully */
        do {
            esp_hf_cb_param_t param;
            memset(&param, 0, sizeof(esp_hf_cb_param_t));
            param.audio_stat.state = ESP_HF_AUDIO_STATE_CONNECTING;
            memcpy(param.audio_stat.remote_addr, &hf_local_param[idx].btc_hf_cb.connected_bda, sizeof(esp_bd_addr_t));
            btc_hf_cb_to_app(ESP_HF_AUDIO_STATE_EVT, &param);
        } while (0);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

bt_status_t btc_hf_disconnect_audio(bt_bdaddr_t *bd_addr)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_SLC_CONNECTED();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        BTA_AgAudioClose(hf_local_param[idx].btc_hf_cb.handle);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

/************************************************************************************
**  AT cmd Handle
************************************************************************************/
//AT+VRA
static bt_status_t btc_hf_vra(bt_bdaddr_t *bd_addr, esp_hf_vr_state_t value)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_SLC_CONNECTED();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        if (hf_local_param[idx].btc_hf_cb.peer_feat & BTA_AG_PEER_FEAT_VREC) {
            tBTA_AG_RES_DATA ag_res;
            memset(&ag_res, 0, sizeof(ag_res));
            ag_res.state = value;
            BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_BVRA_RES, &ag_res);
            return BT_STATUS_SUCCESS;
        } else {
            return BT_STATUS_UNSUPPORTED;
        }
    }
    return BT_STATUS_NOT_READY;
}

static bt_status_t btc_hf_volume_control(bt_bdaddr_t *bd_addr, esp_hf_volume_type_t type, int volume)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_SLC_CONNECTED();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }
    tBTA_AG_RES_DATA ag_res;
    memset(&ag_res, 0, sizeof(tBTA_AG_RES_DATA));

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        ag_res.num = volume;
        BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, (type == ESP_HF_VOLUME_TYPE_SPK) ? BTA_AG_SPK_RES : BTA_AG_MIC_RES, &ag_res);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

// Pre-formatted AT response, typically in response to unknown AT cmd
static bt_status_t btc_hf_unat_response(bt_bdaddr_t *bd_addr, const char *unat)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_INIT();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }

    if (is_connected(NULL) && (idx != BTC_HF_INVALID_IDX))
    {
        tBTA_AG_RES_DATA    ag_res;
        /* Format the response and send */
        memset(&ag_res, 0, sizeof(ag_res));
        strncpy(ag_res.str, unat, BTA_AG_AT_MAX_LEN);
        BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_UNAT_RES, &ag_res);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

// +CMEE:<response>
static bt_status_t btc_hf_cmee_response(bt_bdaddr_t *bd_addr, esp_hf_at_response_code_t response_code, esp_hf_cme_err_t error_code)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_INIT();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }
    
    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        tBTA_AG_RES_DATA    ag_res;
        memset(&ag_res, 0, sizeof(ag_res));
        if (response_code == ESP_HF_AT_RESPONSE_CODE_OK) {
            ag_res.ok_flag = BTA_AG_OK_DONE;
        } else {
            ag_res.ok_flag = BTA_AG_OK_ERROR;
            ag_res.errcode = error_code;
        }
        BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_UNAT_RES, &ag_res);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

// +CIEV<...> for device status update
static bt_status_t btc_hf_indchange_notification(bt_bdaddr_t *bd_addr,
                                                esp_hf_call_status_t call_state,
                                                esp_hf_call_setup_status_t call_setup_state,
                                                esp_hf_network_state_t ntk_state, int signal)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_INIT();
    if (is_connected(bd_addr)) {
        /* Send all indicators to BTA.
         * BTA will make sure no duplicates are sent out*/
        send_indicator_update(BTA_AG_IND_CALL, call_state);
        send_indicator_update(BTA_AG_IND_CALLSETUP, call_setup_state);
        send_indicator_update(BTA_AG_IND_SERVICE, ntk_state);
        send_indicator_update(BTA_AG_IND_SIGNAL, signal);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_SUCCESS;
}

//AT+CIND response
static bt_status_t btc_hf_cind_response(bt_bdaddr_t *bd_addr,
                                        esp_hf_call_setup_status_t call_status, 
                                        esp_hf_call_setup_status_t call_setup_status,
                                        esp_hf_network_state_t ntk_state, int signal, esp_hf_service_type_t roam, int batt_lev,
                                        esp_hf_call_held_status_t  call_held_status)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_INIT();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        tBTA_AG_RES_DATA    ag_res;
        memset(&ag_res, 0, sizeof (ag_res));
        sprintf(ag_res.str, "%d,%d,%d,%d,%d,%d,%d",
                call_status,                                           /* Call state*/
                call_setup_status,                                     /* Callsetup state */
                ntk_state,                                             /* network service */
                signal,                                                /* Signal strength */
                roam,                                                  /* Roaming indicator */
                batt_lev,                                              /* Battery level */
                call_held_status                                       /* Callheld state */  
        );
        BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_CIND_RES, &ag_res);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

//AT+COPS response
static bt_status_t btc_hf_cops_response(bt_bdaddr_t *bd_addr, const char *name)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_SLC_CONNECTED();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        tBTA_AG_RES_DATA    ag_res;
        memset (&ag_res, 0, sizeof (ag_res));
        /* Format the response */
        sprintf(ag_res.str, "0,0,\"%s\"", name);
        ag_res.ok_flag = BTA_AG_OK_DONE;
        BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_COPS_RES, &ag_res);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

//AT+CLCC response
static bt_status_t btc_hf_clcc_response(bt_bdaddr_t *bd_addr, int index, esp_hf_current_call_direction_t dir,
                                        esp_hf_current_call_status_t current_call_state,
                                        esp_hf_current_call_mode_t mode, esp_hf_current_call_mpty_type_t mpty, 
                                        const char *number, esp_hf_call_addr_type_t type)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_SLC_CONNECTED();
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return BT_STATUS_FAIL;
    }

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        tBTA_AG_RES_DATA    ag_res;
        memset (&ag_res, 0, sizeof (ag_res));
        if (index == 0) {
            ag_res.ok_flag = BTA_AG_OK_DONE;
        } else {
            BTC_TRACE_EVENT("clcc_response: [%d] dir: %d current_call_state: %d mode: %d number: %s type: %d",
                            index, dir, current_call_state, mode, number, type);
            int loc = sprintf (ag_res.str, "%d,%d,%d,%d,%d", index, dir, current_call_state, mode, mpty);
            if (number) {
               if ((type == ESP_HF_CALL_ADDR_TYPE_INTERNATIONAL) && (*number != '+')) {
                    sprintf(&ag_res.str[loc], ",\"+%s\",%d", number, type);
                } else {
                    sprintf(&ag_res.str[loc], ",\"%s\",%d", number, type);
                }
            }
        }
        BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_CLCC_RES, &ag_res);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

//AT+CNUM
static bt_status_t btc_hf_cnum_response(bt_bdaddr_t *bd_addr, const char *number, esp_hf_subscriber_service_type_t type)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_SLC_CONNECTED();

    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        tBTA_AG_RES_DATA    ag_res;
        memset(&ag_res, 0, sizeof (ag_res));
        BTC_TRACE_EVENT("cnum_response: number = %s, type = %d", number, type);
        if (number) {
            sprintf(ag_res.str, ",\"%s\",%d",number, type);
        } else {
            sprintf(ag_res.str, ",\"\",%d",type);
        }
        ag_res.ok_flag = BTA_AG_OK_DONE;
        BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_CNUM_RES, &ag_res);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

//+BSIR
static bt_status_t btc_hf_inband_ring(bt_bdaddr_t *bd_addr, esp_hf_in_band_ring_state_t state)
{
    int idx = btc_hf_idx_by_bdaddr(bd_addr);
    CHECK_HF_SLC_CONNECTED();
    
    if (is_connected(bd_addr) && (idx != BTC_HF_INVALID_IDX)) {
        tBTA_AG_RES_DATA    ag_res;
        memset (&ag_res, 0, sizeof (ag_res));
        ag_res.state = state;
        BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_INBAND_RING_RES, &ag_res);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

//Update phone status whether AG or HF act
static bt_status_t btc_hf_phone_state_update(bt_bdaddr_t *bd_addr,int num_active, int num_held,
                                            esp_hf_call_status_t call_state, esp_hf_call_setup_status_t call_setup_state,
                                            char *number, esp_hf_call_addr_type_t type)
{
    tBTA_AG_RES res = 0xff;
    tBTA_AG_RES_DATA ag_res;
    bt_status_t status = BT_STATUS_SUCCESS;
    BOOLEAN activeCallUpdated = FALSE;
    int idx = btc_hf_idx_by_bdaddr(bd_addr), i;

    /* hf_idx is index of connected HS that sent ATA/BLDN, otherwise index of latest connected HS */
    if (hf_local_param->hf_idx != BTC_HF_INVALID_IDX) {
        idx = hf_local_param->hf_idx;
    } else {
        idx = btc_hf_latest_connected_idx();
    }

    BTC_TRACE_DEBUG("phone_state_change: idx = %d", idx);
    CHECK_HF_SLC_CONNECTED();
    BTC_TRACE_DEBUG("phone_state_change: num_active=%d [prev: %d]  num_held=%d[prev: %d] call =%s [prev: %s] call_setup=%s [prev: %s]",
                    num_active, hf_local_param[idx].btc_hf_cb.num_active,
                    num_held, hf_local_param[idx].btc_hf_cb.num_held,
                    dump_hf_call_state(call_state), dump_hf_call_state(hf_local_param[idx].btc_hf_cb.call_state),
                    dump_hf_call_setup_state(call_setup_state), dump_hf_call_setup_state(hf_local_param[idx].btc_hf_cb.call_setup_state));

    /* If all indicators are 0, send end call and return */
    if (num_active == 0 && num_held == 0 && call_state == ESP_HF_CALL_STATUS_NO_CALLS && call_setup_state == ESP_HF_CALL_SETUP_STATUS_IDLE) {
        BTC_TRACE_DEBUG("%s: Phone on hook", __FUNCTION__);

        /* Record call termination timestamp if there was an active/held call or call_setup_state > ESP_HF_CALL_SETUP_STATUS_IDLE */
        if ((hf_local_param[idx].btc_hf_cb.call_state != ESP_HF_CALL_STATUS_NO_CALLS) ||
            (hf_local_param[idx].btc_hf_cb.call_setup_state != ESP_HF_CALL_SETUP_STATUS_IDLE) ||
            (hf_local_param[idx].btc_hf_cb.num_active) ||
            (hf_local_param[idx].btc_hf_cb.num_held)) {
            BTC_TRACE_DEBUG("%s: Record call termination timestamp", __FUNCTION__);
            clock_gettime(CLOCK_MONOTONIC, &hf_local_param[0].btc_hf_cb.call_end_timestamp);
        }
        BTA_AgResult(BTA_AG_HANDLE_ALL, BTA_AG_END_CALL_RES, NULL);
        hf_local_param->hf_idx = BTC_HF_INVALID_IDX;

        /* If held call was present, reset that as well. */
        if (hf_local_param[idx].btc_hf_cb.num_held) {
            send_indicator_update(BTA_AG_IND_CALLHELD, 0);
        }
        goto update_call_states;
    }

    /* Phone state can change when:
    *  1. An outgoing/incoming call was answered.
    *  2. A held was resumed.
    *  3. Without callsetup notifications, call became active. (HF Unit links in during an Active call.)
    */

    /* Handle case(3) here prior to call setup handling.*/
    if (((num_active + num_held) > 0) && 
        (hf_local_param[idx].btc_hf_cb.num_active == 0) &&
        (hf_local_param[idx].btc_hf_cb.num_held == 0) &&
        (hf_local_param[idx].btc_hf_cb.call_setup_state == ESP_HF_CALL_SETUP_STATUS_IDLE)) {
        BTC_TRACE_DEBUG("%s: Active/Held call notification received without call setup update", __FUNCTION__);

        memset(&ag_res, 0, sizeof(tBTA_AG_RES_DATA));
        ag_res.audio_handle = hf_local_param[idx].btc_hf_cb.handle;

        /* Addition callsetup with the Active call. */
        if (call_setup_state != ESP_HF_CALL_SETUP_STATUS_IDLE) {
            res = BTA_AG_MULTI_CALL_RES;
        } else {
            res = BTA_AG_OUT_CALL_CONN_RES;
        }
        /* CIND response should have been updated. */
        BTA_AgResult(BTA_AG_HANDLE_ALL, res, &ag_res);
        /* Just open SCO conenction. */
        BTA_AgAudioOpen(ag_res.audio_handle);
        activeCallUpdated = TRUE;
    }

    /* Handle call_setup indicator change. */
    if (call_setup_state != hf_local_param[idx].btc_hf_cb.call_setup_state) {
        BTC_TRACE_DEBUG("%s: Call setup states changed. old: %s new: %s", __FUNCTION__,
                        dump_hf_call_setup_state(hf_local_param[idx].btc_hf_cb.call_setup_state),
                        dump_hf_call_setup_state(call_setup_state));
        memset(&ag_res, 0, sizeof(tBTA_AG_RES_DATA));

        switch(call_setup_state)
        {
            case ESP_HF_CALL_SETUP_STATUS_IDLE:
            {
                switch(hf_local_param[idx].btc_hf_cb.call_setup_state)
                {
                    case ESP_HF_CALL_SETUP_STATUS_INCOMING:
                    {
                        if (num_active > hf_local_param[idx].btc_hf_cb.num_active) {
                            res = BTA_AG_IN_CALL_CONN_RES;
                            ag_res.audio_handle = hf_local_param[idx].btc_hf_cb.handle;
                        } else if (num_held > hf_local_param[idx].btc_hf_cb.num_held) {
                            res = BTA_AG_IN_CALL_HELD_RES;
                        } else {
                            res = BTA_AG_CALL_CANCEL_RES;
                        }
                        break;
                    }

                    case ESP_HF_CALL_SETUP_STATUS_OUTGOING_DIALING:
                    case ESP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING:
                    {
                        if (num_active > hf_local_param[idx].btc_hf_cb.num_active) {
                            res = BTA_AG_OUT_CALL_CONN_RES;
                            ag_res.audio_handle = BTA_AG_HANDLE_SCO_NO_CHANGE;
                        } else {
                            res = BTA_AG_CALL_CANCEL_RES;
                        }
                        break;
                    }
                    
                    default:
                        BTC_TRACE_ERROR("%s: Call setup state no change.", __FUNCTION__);
                        status = BT_STATUS_PARM_INVALID;
                        break;
                }
                break;
            }

            case ESP_HF_CALL_SETUP_STATUS_INCOMING:
            {
                if (num_active || num_held) {
                    res = BTA_AG_CALL_WAIT_RES;
                } else {
                    res = BTA_AG_IN_CALL_RES;
                }

                if (number) {
                    int loc = 0;
                    if ((type == ESP_HF_CALL_ADDR_TYPE_INTERNATIONAL) && (*number != '+')) {
                        loc = sprintf (ag_res.str, "\"+%s\"", number);
                    } else {
                        loc = sprintf (ag_res.str, "\"%s\"", number);
                    }
                    ag_res.num = type;
                    if (res == BTA_AG_CALL_WAIT_RES) {
                        sprintf(&ag_res.str[loc], ",%d", type);
                    }
                }
                break;
            }

            case ESP_HF_CALL_SETUP_STATUS_OUTGOING_DIALING:
            {
                if (!(num_active + num_held)) {
                    ag_res.audio_handle = hf_local_param[idx].btc_hf_cb.handle;
                }
                res = BTA_AG_OUT_CALL_ORIG_RES;
                break;
            }

            case ESP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING:
            {
                if ((hf_local_param[idx].btc_hf_cb.call_setup_state == ESP_HF_CALL_SETUP_STATUS_IDLE) &&
                    !(num_active + num_held)) {
                    ag_res.audio_handle = hf_local_param[idx].btc_hf_cb.handle;
                    /* Force SCO setup here.*/
                    BTA_AgAudioOpen(ag_res.audio_handle);
                }
                res = BTA_AG_OUT_CALL_ALERT_RES;
                break;
            }

            default:
                BTC_TRACE_ERROR("%s: Incorrect new ringing call state", __FUNCTION__);
                status = BT_STATUS_PARM_INVALID;
                break;
        }
        BTC_TRACE_DEBUG("%s: Call setup state changed. res=%d, audio_handle=%d", __FUNCTION__, res, ag_res.audio_handle);
        if (res) {
            BTA_AgResult(BTA_AG_HANDLE_ALL, res, &ag_res);
        }
        /* If call setup is idle, we have already updated call indicator, jump out */
        if (call_setup_state == ESP_HF_CALL_SETUP_STATUS_IDLE) {
            /* Check & Update call_held_state */
            if ((num_held > 0) && (num_active > 0))
                send_indicator_update(BTA_AG_IND_CALLHELD, ESP_HF_CALL_HELD_STATUS_HELD_AND_ACTIVE);
            goto update_call_states;
        }
    }

    memset(&ag_res, 0, sizeof(tBTA_AG_RES_DATA));

    /* Handle call_state indicator change. */
    if (!activeCallUpdated &&
        ((num_active + num_held) != (hf_local_param[idx].btc_hf_cb.num_active + hf_local_param[idx].btc_hf_cb.num_held))) {
        BTC_TRACE_DEBUG("%s: Active call states changed. old: %d new: %d", __FUNCTION__, hf_local_param[idx].btc_hf_cb.num_active, num_active);
        send_indicator_update(BTA_AG_IND_CALL, ((num_active + num_held) > 0) ? 1 : 0);
    }

    /* Handle call_held_state indicator change. */
    if (num_held != hf_local_param[idx].btc_hf_cb.num_held  ||
        ((num_active == 0) && ((num_held + hf_local_param[idx].btc_hf_cb.num_held) > 1))) {
        BTC_TRACE_DEBUG("%s: Held call states changed. old: %d new: %d", __FUNCTION__, hf_local_param[idx].btc_hf_cb.num_held, num_held);
        send_indicator_update(BTA_AG_IND_CALLHELD, ((num_held == 0) ? 0 : ((num_active == 0) ? 2 : 1)));
    }

    /* Handle Call Active/Held Swap indicator update.*/
    if ((call_setup_state == hf_local_param[idx].btc_hf_cb.call_setup_state) &&
        (num_active) &&
        (num_held) &&
        (num_active == hf_local_param[idx].btc_hf_cb.num_active) &&
        (num_held == hf_local_param[idx].btc_hf_cb.num_held)) {
        BTC_TRACE_DEBUG("%s: Calls swapped", __FUNCTION__);
        send_indicator_update(BTA_AG_IND_CALLHELD, 1);
    }

update_call_states:
    for (i = 0; i < btc_max_hf_clients; i++) {
        hf_local_param[i].btc_hf_cb.num_active = num_active;
        hf_local_param[i].btc_hf_cb.num_held = num_held;
        hf_local_param[i].btc_hf_cb.call_state = call_state;
        hf_local_param[i].btc_hf_cb.call_setup_state = call_setup_state;
    }
    return status;
}

/************************************************************************************
**  Memory malloc and release
************************************************************************************/
void btc_hf_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_hf_args_t *dst = (btc_hf_args_t *) p_dest;
    btc_hf_args_t *src = (btc_hf_args_t *) p_src;

    switch (msg->act) {
        case BTC_HF_UNAT_RESPONSE_EVT:
        {
            dst->unat_rep.unat = (char *)osi_malloc(strlen(src->unat_rep.unat)+1);
            if(dst->unat_rep.unat) {
                memcpy(dst->unat_rep.unat, src->unat_rep.unat, strlen(src->unat_rep.unat)+1);
            } else if (strlen(src->unat_rep.unat) == 0) {
                BTC_TRACE_DEBUG("%s %d no mem\n", __func__, msg->act);
            } else {
                BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
            }
            break;
        }

        case BTC_HF_COPS_RESPONSE_EVT:
        {
            dst->cops_rep.name = (char *)osi_malloc(strlen(src->cops_rep.name)+1);
            if(dst->cops_rep.name) {
                memcpy(dst->cops_rep.name, src->cops_rep.name, strlen(src->cops_rep.name)+1);
            } else if (strlen(src->cops_rep.name) == 0) {
                BTC_TRACE_DEBUG("%s %d no mem\n", __func__, msg->act);
            } else {
                BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
            }
            break;
        }

        case BTC_HF_CLCC_RESPONSE_EVT:
        {
            dst->clcc_rep.number = (char *)osi_malloc(strlen(src->clcc_rep.number)+1);
            if(dst->clcc_rep.number) {
                memcpy(dst->clcc_rep.number, src->clcc_rep.number, strlen(src->clcc_rep.number)+1);
            } else if (strlen(src->clcc_rep.number) == 0) {
                BTC_TRACE_DEBUG("%s %d no mem\n", __func__, msg->act);
            } else {
                BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
            }
            break;
        }

        case BTC_HF_CNUM_RESPONSE_EVT:
        {
            dst->cnum_rep.number = (char *)osi_malloc(strlen(src->cnum_rep.number)+1);
            if(dst->cnum_rep.number) {
                memcpy(dst->cnum_rep.number, src->cnum_rep.number, strlen(src->cnum_rep.number)+1);
            } else if (strlen(src->cnum_rep.number) == 0) {
                BTC_TRACE_DEBUG("%s %d no mem\n", __func__, msg->act);
            } else {
                BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
            }
            break;
        }
        
        case BTC_HF_AC_INCALL_EVT:
        case BTC_HF_RJ_INCALL_EVT:
        case BTC_HF_OUT_CALL_EVT:
        case BTC_HF_END_CALL_EVT:
        {
            dst->phone.number = (char *)osi_malloc(strlen(src->phone.number)+1);
            if(dst->phone.number) {
                memcpy(dst->phone.number, src->phone.number, strlen(src->phone.number)+1);
            } else if (strlen(src->phone.number) == 0) {
                BTC_TRACE_DEBUG("%s %d no mem\n", __func__, msg->act);
            } else {
                BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
            }
            break;
        }

        default:
            break;
    }
}

void btc_hf_arg_deep_free(btc_msg_t *msg)
{
    btc_hf_args_t *arg = (btc_hf_args_t *)msg->arg;

    switch (msg->act) {
        case BTC_HF_UNAT_RESPONSE_EVT:
        {
            if (arg->unat_rep.unat) {
                osi_free(arg->unat_rep.unat);
            }
            break;
        }

        case BTC_HF_COPS_RESPONSE_EVT:
        {
            if (arg->cops_rep.name) {
                osi_free(arg->cops_rep.name);
            }
            break;
        }

        case BTC_HF_CLCC_RESPONSE_EVT:
        {
            if (arg->clcc_rep.number) {
                osi_free(arg->clcc_rep.number);
            }
            break;
        }

        case BTC_HF_CNUM_RESPONSE_EVT:
        {
            if (arg->cnum_rep.number) {
                osi_free(arg->cnum_rep.number);
            }
            break;
        }
        
        case BTC_HF_AC_INCALL_EVT:
        case BTC_HF_RJ_INCALL_EVT:
        case BTC_HF_OUT_CALL_EVT:
        case BTC_HF_END_CALL_EVT:
        {
            if (arg->phone.number) {
                osi_free(arg->phone.number);
            }
            break;
        }

        default:
            break;
    }
}

/************************************************************************************
**  Handler Functions (handle the cmd from app)
************************************************************************************/
void btc_hf_call_handler(btc_msg_t *msg)
{
    btc_hf_args_t *arg = (btc_hf_args_t *)(msg->arg);
    
    switch (msg->act) {
        case BTC_HF_INIT_EVT:
        {
            btc_hf_init(&arg->init);
            break;
        }

        case BTC_HF_DEINIT_EVT:
        {
            btc_hf_deinit(&arg->deinit);
            break;
        }

        case BTC_HF_CONNECT_EVT:
        {
            btc_hf_connect(&arg->connect);
            break;
        }

        case BTC_HF_DISCONNECT_EVT:
        {
            btc_hf_disconnect(&arg->disconnect);
            break;
        }

        case BTC_HF_CONNECT_AUDIO_EVT:
        {
            btc_hf_connect_audio(&arg->connect_audio);
            break;
        }

        case BTC_HF_DISCONNECT_AUDIO_EVT:
        {
            btc_hf_disconnect_audio(&arg->disconnect_audio);
            break;
        }

        case BTC_HF_VRA_EVT:
        {
            btc_hf_vra(&arg->vra_rep.remote_addr, arg->vra_rep.value);
            if (arg->vra_rep.value) {
                btc_hf_connect_audio(&arg->vra_rep.remote_addr);
            } else {
                btc_hf_disconnect_audio(&arg->vra_rep.remote_addr);
            }
            break;
        }

        case BTC_HF_VOLUME_CONTROL_EVT:
        {
            btc_hf_volume_control(&arg->volcon.remote_addr, arg->volcon.target_type, arg->volcon.volume);
            break;
        }

        case BTC_HF_UNAT_RESPONSE_EVT:
        {
            btc_hf_unat_response(&arg->unat_rep.remote_addr, arg->unat_rep.unat);
        }

        case BTC_HF_CME_ERR_EVT:
        {
            btc_hf_cmee_response(&arg->ext_at.remote_addr, arg->ext_at.response_code, arg->ext_at.error_code);
            break;
        }

        case BTC_HF_IND_NOTIFICATION_EVT:
        {
            btc_hf_indchange_notification(&arg->ind_change.remote_addr,
                                        arg->ind_change.call_state, arg->ind_change.call_setup_state,
                                        arg->ind_change.ntk_state, arg->ind_change.signal);
            break;
        }

        case BTC_HF_CIND_RESPONSE_EVT:
        {
            btc_hf_cind_response(&arg->cind_rep.remote_addr,
                                arg->cind_rep.call_state, arg->cind_rep.call_setup_state,
                                arg->cind_rep.ntk_state, arg->cind_rep.signal, arg->cind_rep.roam, arg->cind_rep.batt_lev,
                                arg->cind_rep.call_held_state);
            break;
        }

        case BTC_HF_COPS_RESPONSE_EVT:
        {
            btc_hf_cops_response(&arg->cops_rep.remote_addr, arg->cops_rep.name);
            break;
        }

       case BTC_HF_CLCC_RESPONSE_EVT:
        {
            btc_hf_clcc_response(&arg->clcc_rep.remote_addr, arg->clcc_rep.index,
                                arg->clcc_rep.dir, arg->clcc_rep.current_call_state,
                                arg->clcc_rep.mode, arg->clcc_rep.mpty, arg->clcc_rep.number, arg->clcc_rep.type);
            break;
        }

        case BTC_HF_CNUM_RESPONSE_EVT:
        {
            btc_hf_cnum_response(&arg->cnum_rep.remote_addr, arg->cnum_rep.number, arg->cnum_rep.type);
            break;
        }

        case BTC_HF_INBAND_RING_EVT:
        {
            btc_hf_inband_ring(&arg->bsir.remote_addr, arg->bsir.state);
            break;
        }

        case BTC_HF_AC_INCALL_EVT:
        {
            btc_hf_phone_state_update(&arg->phone.remote_addr, arg->phone.num_active, arg->phone.num_held,
                                    arg->phone.call_state, arg->phone.call_setup_state, arg->phone.number,
                                    arg->phone.call_addr_type);
            break;
        }

        case BTC_HF_RJ_INCALL_EVT:
        {
            btc_hf_phone_state_update(&arg->phone.remote_addr, arg->phone.num_active, arg->phone.num_held,
                                    arg->phone.call_state, arg->phone.call_setup_state, arg->phone.number,
                                    arg->phone.call_addr_type);
            btc_hf_disconnect_audio(&arg->phone.remote_addr);
            break;
        }

        case BTC_HF_OUT_CALL_EVT:
        {
            btc_hf_connect_audio(&arg->phone.remote_addr);
            btc_hf_phone_state_update(&arg->phone.remote_addr, arg->phone.num_active, arg->phone.num_held,
                                    arg->phone.call_state, arg->phone.call_setup_state, arg->phone.number,
                                    arg->phone.call_addr_type);
            break;
        }

        case BTC_HF_END_CALL_EVT:
        {
            btc_hf_phone_state_update(&arg->phone.remote_addr, arg->phone.num_active, arg->phone.num_held,
                                    arg->phone.call_state, arg->phone.call_setup_state, arg->phone.number,
                                    arg->phone.call_addr_type);
            btc_hf_disconnect_audio(&arg->phone.remote_addr);
            break;
        }

        case BTC_HF_REGISTER_DATA_CALLBACK_EVT:
        {
            btc_hf_reg_data_cb(arg->reg_data_cb.recv, arg->reg_data_cb.send);
        }
            break;

        default:
            BTC_TRACE_WARNING("%s : unhandled event: %d\n", __FUNCTION__, msg->act);
    }
    btc_hf_arg_deep_free(msg);
}

void btc_hf_cb_handler(btc_msg_t *msg)
{
    uint16_t event = msg->act;
    tBTA_AG *p_data = (tBTA_AG *)msg->arg;
    esp_hf_cb_param_t  param;
    bdstr_t bdstr;
    int idx;

    if (p_data == NULL) {
        idx = BTC_HF_INVALID_IDX;
    } else {
        idx = p_data->hdr.handle - 1;
    }

    BTC_TRACE_DEBUG("%s: event = %s", __FUNCTION__, dump_hf_event(event));
    if ((idx < 0) || (idx >= BTC_HF_NUM_CB)) {
        BTC_TRACE_ERROR("%s: Invalid index %d", __FUNCTION__, idx);
        return;
    }

    switch (event) {
        case BTA_AG_ENABLE_EVT:
        case BTA_AG_DISABLE_EVT:
            break;

        case BTA_AG_REGISTER_EVT:
        {
            hf_local_param[idx].btc_hf_cb.handle = p_data->reg.hdr.handle;
            BTC_TRACE_DEBUG("%s: BTA_AG_REGISTER_EVT," "hf_local_param[%d].btc_hf_cb.handle = %d",
                            __FUNCTION__, idx, hf_local_param[idx].btc_hf_cb.handle);
            break;            
        }

        case BTA_AG_OPEN_EVT:
        {
            if (p_data->open.status == BTA_AG_SUCCESS) 
            {
                bdcpy(hf_local_param[idx].btc_hf_cb.connected_bda.address, p_data->open.bd_addr);
                hf_local_param[idx].btc_hf_cb.connection_state  = ESP_HF_CONNECTION_STATE_CONNECTED;
                hf_local_param[idx].btc_hf_cb.peer_feat = 0;
                hf_local_param[idx].btc_hf_cb.chld_feat = 0;
                //clear_phone_state();
            } else if (hf_local_param[idx].btc_hf_cb.connection_state  == ESP_HF_CONNECTION_STATE_CONNECTING) {
                hf_local_param[idx].btc_hf_cb.connection_state  = ESP_HF_CONNECTION_STATE_DISCONNECTED;
            } else {
                BTC_TRACE_WARNING("%s: AG open failed, but another device connected. status=%d state=%d connected device=%s", __FUNCTION__, 
                                    p_data->open.status, hf_local_param[idx].btc_hf_cb.connection_state,
                                    bdaddr_to_string(&hf_local_param[idx].btc_hf_cb.connected_bda, bdstr, sizeof(bdstr)));
                break;
            }

            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                memcpy(param.conn_stat.remote_bda, &hf_local_param[idx].btc_hf_cb.connected_bda, sizeof(esp_bd_addr_t));
                param.conn_stat.state = hf_local_param[idx].btc_hf_cb.connection_state;
                param.conn_stat.peer_feat = 0;
                param.conn_stat.chld_feat = 0;
                btc_hf_cb_to_app(ESP_HF_CONNECTION_STATE_EVT, &param);
            } while (0);

            if (hf_local_param[idx].btc_hf_cb.connection_state  == ESP_HF_CONNECTION_STATE_DISCONNECTED)
                bdsetany(hf_local_param[idx].btc_hf_cb.connected_bda.address);

            if (p_data->open.status != BTA_AG_SUCCESS)
                btc_queue_advance();
            break;
        }

        case BTA_AG_CONN_EVT:
        {
            clock_gettime(CLOCK_MONOTONIC, &(hf_local_param[idx].btc_hf_cb.connected_timestamp));
            BTC_TRACE_DEBUG("%s: BTA_AG_CONN_EVT, idx = %d ", __FUNCTION__, idx);
            hf_local_param[idx].btc_hf_cb.peer_feat = p_data->conn.peer_feat;
            hf_local_param[idx].btc_hf_cb.chld_feat = p_data->conn.chld_feat;
            hf_local_param[idx].btc_hf_cb.connection_state  = ESP_HF_CONNECTION_STATE_SLC_CONNECTED;

            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                param.conn_stat.state = hf_local_param[idx].btc_hf_cb.connection_state; 
                param.conn_stat.peer_feat = hf_local_param[idx].btc_hf_cb.peer_feat;
                param.conn_stat.chld_feat = hf_local_param[idx].btc_hf_cb.chld_feat;
                memcpy(param.conn_stat.remote_bda, &hf_local_param[idx].btc_hf_cb.connected_bda, sizeof(esp_bd_addr_t));
                btc_hf_cb_to_app(ESP_HF_CONNECTION_STATE_EVT, &param);
            } while(0);
            hf_local_param[idx].hf_idx = btc_hf_latest_connected_idx();
            btc_queue_advance();
            break;
        }

        case BTA_AG_CLOSE_EVT:
        {
            hf_local_param[idx].btc_hf_cb.connected_timestamp.tv_sec = 0;
            hf_local_param[idx].btc_hf_cb.connection_state  = ESP_HF_CONNECTION_STATE_DISCONNECTED;
            BTC_TRACE_DEBUG("%s: BTA_AG_CLOSE_EVT," "hf_local_param[%d].btc_hf_cb.handle = %d", __FUNCTION__, 
                            idx, hf_local_param[idx].btc_hf_cb.handle);
            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                param.conn_stat.state = ESP_HF_CONNECTION_STATE_DISCONNECTED;
                param.conn_stat.peer_feat = 0;
                param.conn_stat.chld_feat = 0;
                memcpy(param.conn_stat.remote_bda, &hf_local_param[idx].btc_hf_cb.connected_bda, sizeof(esp_bd_addr_t));
                btc_hf_cb_to_app(ESP_HF_CONNECTION_STATE_EVT, &param);
            } while(0);
            bdsetany(hf_local_param[idx].btc_hf_cb.connected_bda.address);
            clear_phone_state();
            hf_local_param[idx].hf_idx = btc_hf_latest_connected_idx();
            btc_queue_advance();
            break;
        }

        case BTA_AG_AUDIO_OPEN_EVT:
        {
            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                param.audio_stat.state = ESP_HF_AUDIO_STATE_CONNECTED;
                memcpy(param.audio_stat.remote_addr, &hf_local_param[idx].btc_hf_cb.connected_bda,sizeof(esp_bd_addr_t));
                btc_hf_cb_to_app(ESP_HF_AUDIO_STATE_EVT, &param);
            } while(0);
            break;
        }

        case BTA_AG_AUDIO_CLOSE_EVT:
        {
            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                param.audio_stat.state = ESP_HF_AUDIO_STATE_DISCONNECTED;
                memcpy(param.audio_stat.remote_addr, &hf_local_param[idx].btc_hf_cb.connected_bda, sizeof(esp_bd_addr_t));
                btc_hf_cb_to_app(ESP_HF_AUDIO_STATE_EVT, &param);
            } while(0);
            break;
        }

        case BTA_AG_AT_BVRA_EVT:
        {
            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                param.vra_rep.value = p_data->val.num;
                btc_hf_cb_to_app(ESP_HF_BVRA_EVT, &param);
            } while (0);
            break;
        }

        case BTA_AG_SPK_EVT:
        case BTA_AG_MIC_EVT:
        {
            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                param.volume_control.type = (event == BTA_AG_SPK_EVT) ? ESP_HF_VOLUME_CONTROL_TARGET_SPK : ESP_HF_VOLUME_CONTROL_TARGET_MIC;
                param.volume_control.volume = p_data->val.num;
                btc_hf_cb_to_app(ESP_HF_VOLUME_CONTROL_EVT, &param);
            } while (0);
            break;
        }

        case BTA_AG_AT_UNAT_EVT:
        {
            memset(&param, 0, sizeof(esp_hf_cb_param_t));
            param.unat_rep.unat = p_data->val.str;
            btc_hf_cb_to_app(ESP_HF_UNAT_RESPONSE_EVT, &param);
            break;
        }

        case BTA_AG_AT_CBC_EVT:
        case BTA_AG_AT_CIND_EVT:
        {
            btc_hf_cind_evt(&p_data->ind);
            break;
        }

        case BTA_AG_AT_COPS_EVT:
        {
            btc_hf_cb_to_app(ESP_HF_COPS_RESPONSE_EVT, NULL);
            break;
        }

        case BTA_AG_AT_CLCC_EVT:
        {
            btc_hf_cb_to_app(ESP_HF_CLCC_RESPONSE_EVT, NULL);
            break;
        }

        case BTA_AG_AT_CNUM_EVT:
        {
            btc_hf_cb_to_app(ESP_HF_CNUM_RESPONSE_EVT, NULL);
            break;
        }

        case BTA_AG_AT_VTS_EVT:
        {
            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                param.vts_rep.code = p_data->val.str;
                btc_hf_cb_to_app(ESP_HF_VTS_RESPONSE_EVT, &param);
            } while(0);
            break;
        }

        case BTA_AG_AT_NREC_EVT:
        {
            do {
                memset(&param, 0, sizeof(esp_hf_cb_param_t));
                param.nrec.state = p_data->val.num;
                btc_hf_cb_to_app(ESP_HF_NREC_RESPONSE_EVT, &param);
            } while(0);
            break;
        }

        case BTA_AG_AT_A_EVT:
        {
            btc_hf_cb_to_app(ESP_HF_ATA_RESPONSE_EVT, NULL);
            break;
        }

        case BTA_AG_AT_CHUP_EVT:
        {
            btc_hf_cb_to_app(ESP_HF_CHUP_RESPONSE_EVT, NULL);
            break;
        }

        case BTA_AG_AT_BLDN_EVT:
        case BTA_AG_AT_D_EVT:
        {
            do {
                if (event == BTA_AG_AT_D_EVT && p_data->val.str) {           // dial_number_or_memory
                    memset(&param, 0, sizeof(esp_hf_cb_param_t));
                    param.out_call.num_or_loc = osi_malloc((strlen(p_data->val.str) + 1) * sizeof(char));
                    sprintf(param.out_call.num_or_loc, p_data->val.str);
                    btc_hf_cb_to_app(ESP_HF_DIAL_EVT, &param);
                    send_indicator_update(BTA_AG_IND_CALLSETUP,BTA_AG_CALLSETUP_OUTGOING);
                    osi_free(param.out_call.num_or_loc);
                } else if (event == BTA_AG_AT_BLDN_EVT) {                    //dial_last
                    memset(&param, 0, sizeof(esp_hf_cb_param_t));
                    btc_hf_cb_to_app(ESP_HF_DIAL_EVT, &param);
                }
            } while(0);
            break;
        }

        case BTA_AG_AT_BINP_EVT:
        case BTA_AG_AT_BTRH_EVT:
        {
            tBTA_AG_RES_DATA ag_res;
            memset(&ag_res, 0, sizeof(ag_res));
            ag_res.ok_flag = BTA_AG_OK_ERROR;
            ag_res.errcode = BTA_AG_ERR_OP_NOT_SUPPORTED;
            BTA_AgResult(hf_local_param[idx].btc_hf_cb.handle, BTA_AG_UNAT_RES, &ag_res);
            break;
        }

        case BTA_AG_AT_BAC_EVT:
        {
            BTC_TRACE_DEBUG("AG Bitmap of peer-codecs %d", p_data->val.num);
            memset(&param, 0, sizeof(esp_hf_cb_param_t));
            param.codec.mode = p_data->val.num;
            btc_hf_cb_to_app(ESP_HF_BAC_RESPONSE_EVT, &param);
            break;
        }

#if (BTM_WBS_INCLUDED == TRUE )
        case BTA_AG_WBS_EVT:
        {
            BTC_TRACE_DEBUG("BTA_AG_WBS_EVT Set codec status %d codec %d 1=CVSD 2=MSBC", p_data->val.hdr.status, p_data->val.num);
            memset(&param, 0, sizeof(esp_hf_cb_param_t));
            param.codec.mode = p_data->val.num;
            if(p_data->val.num == BTA_AG_CODEC_CVSD) {
                btc_hf_cb_to_app(ESP_HF_BCS_RESPONSE_EVT, &param);
            } else if(p_data->val.num == BTA_AG_CODEC_MSBC) {
                btc_hf_cb_to_app(ESP_HF_BCS_RESPONSE_EVT, &param);
            } else {
                btc_hf_cb_to_app(ESP_HF_BCS_RESPONSE_EVT, &param);
            }
            break;
        }
#endif

#if (BTM_WBS_INCLUDED == TRUE)
            /* If the peer supports mSBC and the BTC prefferred codec is also mSBC, then
            ** we should set the BTA AG Codec to mSBC. This would trigger a +BCS to mSBC at the time
            ** of SCO connection establishment */
            if ((btc_conf_hf_force_wbs == TRUE) && (p_data->val.num & BTA_AG_CODEC_MSBC)) {
                  BTC_TRACE_EVENT("%s btc_hf override-Preferred Codec to MSBC", __FUNCTION__);
                  BTA_AgSetCodec(hf_local_param[idx].btc_hf_cb.handle,BTA_AG_CODEC_MSBC);
            }
            else {
                  BTC_TRACE_EVENT("%s btif_hf override-Preferred Codec to CVSD", __FUNCTION__);
                  BTA_AgSetCodec(hf_local_param[idx].btc_hf_cb.handle,BTA_AG_CODEC_CVSD);
            }
#endif
            break;
        case BTA_AG_AT_BCS_EVT:
            BTC_TRACE_DEBUG("AG final seleded codec is %d 1=CVSD 2=MSBC", p_data->val.num);
            /*  no ESP_HF_WBS_NONE case, becuase HF 1.6 supported device can send BCS */
            btc_hf_cb_to_app(ESP_HF_BCS_RESPONSE_EVT, &param);
            break;
        default:
            BTC_TRACE_WARNING("%s: Unhandled event: %d", __FUNCTION__, event);
            break;
    }
}
#endif // #if (BTC_HF_INCLUDED == TRUE)