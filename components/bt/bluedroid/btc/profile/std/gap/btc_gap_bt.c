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

#include <string.h>
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "btc_gap_bt.h"
#include "bta_api.h"
#include "bt_trace.h"
#include "bt_target.h"
#include "btc_manage.h"
#include "btc_util.h"
#include "allocator.h"

#if (BTC_GAP_BT_INCLUDED == TRUE)

#define COD_UNCLASSIFIED ((0x1F) << 8)

#define BTC_STORAGE_FILL_PROPERTY(p_prop, t, l, p_v) \
         (p_prop)->type = t;(p_prop)->len = l; (p_prop)->val = (p_v);

static void bte_search_devices_evt(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);
static void bte_dm_search_services_evt(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);
static void bte_dm_remote_service_record_evt(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);
static void search_services_copy_cb(btc_msg_t *msg, void *p_dest, void *p_src);
static void search_service_record_copy_cb(btc_msg_t *msg, void *p_dest, void *p_src);

static bool btc_gap_bt_inquiry_in_progress = false;

static inline void btc_gap_bt_cb_to_app(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    esp_bt_gap_cb_t cb = (esp_bt_gap_cb_t)btc_profile_cb_get(BTC_PID_GAP_BT);
    if (cb) {
        cb(event, param);
    }
}

static void btc_bt_set_scan_mode(esp_bt_scan_mode_t mode)
{
    tBTA_DM_DISC disc_mode;
    tBTA_DM_CONN conn_mode;

    switch (mode) {
    case ESP_BT_SCAN_MODE_NONE:
        disc_mode = BTA_DM_NON_DISC;
        conn_mode = BTA_DM_NON_CONN;
        break;

    case ESP_BT_SCAN_MODE_CONNECTABLE:
        disc_mode = BTA_DM_NON_DISC;
        conn_mode = BTA_DM_CONN;
        break;

    case ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE:
        disc_mode = BTA_DM_GENERAL_DISC;
        conn_mode = BTA_DM_CONN;
        break;

    default:
        LOG_WARN("invalid scan mode (0x%x)", mode);
        return;
    }

    BTA_DmSetVisibility(disc_mode, conn_mode, BTA_DM_IGNORE, BTA_DM_IGNORE);
    return;
}

static void btc_gap_bt_start_discovery(btc_gap_bt_args_t *arg)
{
    tBTA_DM_INQ inq_params;
    tBTA_SERVICE_MASK services = 0;

    BTIF_TRACE_EVENT("%s", __FUNCTION__);

    inq_params.mode = (arg->start_disc.mode == ESP_BT_INQ_MODE_GENERAL_INQUIRY) ?
                      BTA_DM_GENERAL_INQUIRY : BTA_DM_LIMITED_INQUIRY;
    inq_params.duration = arg->start_disc.inq_len;
    inq_params.max_resps = arg->start_disc.num_rsps;

    inq_params.report_dup = TRUE;
    inq_params.filter_type = BTA_DM_INQ_CLR;
    /* TODO: Filter device by BDA needs to be implemented here */

    /* Will be enabled to TRUE once inquiry busy level has been received */
    btc_gap_bt_inquiry_in_progress = FALSE;
    /* find nearby devices */
    BTA_DmSearch(&inq_params, services, bte_search_devices_evt);

    return;
}

static void btc_gap_bt_cancel_discovery(void)
{
    BTA_DmSearchCancel();
}

static void btc_gap_bt_get_remote_services(bt_bdaddr_t *remote_bda)
{
    BTA_DmDiscover(remote_bda->address, BTA_ALL_SERVICE_MASK,
                   bte_dm_search_services_evt, TRUE);
}

static void btc_gap_bt_get_remote_service_record(btc_gap_bt_args_t *arg)
{
    esp_bt_uuid_t *uuid = &arg->get_rmt_srv_rcd.uuid;
    bt_bdaddr_t *remote_bda = &arg->get_rmt_srv_rcd.bda;

    tSDP_UUID sdp_uuid;

    sdp_uuid.len = uuid->len;
    memcpy(&sdp_uuid.uu, &uuid->uuid, uuid->len);

    BTA_DmDiscoverUUID(remote_bda->address, &sdp_uuid,
                       bte_dm_remote_service_record_evt, TRUE);
}


/*******************************************************************************
**
** Function         search_devices_copy_cb
**
** Description      Deep copy callback for search devices event
**
** Returns          void
**
*******************************************************************************/
static void search_devices_copy_cb(btc_msg_t *msg, void *p_dest, void *p_src)
{
    tBTA_DM_SEARCH *p_dest_data =  (tBTA_DM_SEARCH *) p_dest;
    tBTA_DM_SEARCH *p_src_data =  (tBTA_DM_SEARCH *) p_src;

    if (!p_src) {
        return;
    }

    switch (msg->aid) {
    case BTA_DM_INQ_RES_EVT: {
        if (p_src_data->inq_res.p_eir) {
            p_dest_data->inq_res.p_eir = (UINT8 *)(p_dest + sizeof(tBTA_DM_SEARCH));
            memcpy(p_dest_data->inq_res.p_eir, p_src_data->inq_res.p_eir, HCI_EXT_INQ_RESPONSE_LEN);
        }
    }
    break;

    case BTA_DM_DISC_RES_EVT: {
        if (p_src_data->disc_res.raw_data_size && p_src_data->disc_res.p_raw_data) {
            p_dest_data->disc_res.p_raw_data = (UINT8 *)(p_dest + sizeof(tBTA_DM_SEARCH));
            memcpy(p_dest_data->disc_res.p_raw_data,
                   p_src_data->disc_res.p_raw_data,
                   p_src_data->disc_res.raw_data_size);
        }
    }
    break;
    }
}

/*******************************************************************************
**
** Function         search_service_record_copy_cb
**
** Description      Deep copy callback for search service record event
**
** Returns          void
**
*******************************************************************************/
static void search_service_record_copy_cb(btc_msg_t *msg, void *p_dest, void *p_src)
{
    tBTA_DM_SEARCH *p_dest_data =  (tBTA_DM_SEARCH *) p_dest;
    tBTA_DM_SEARCH *p_src_data =  (tBTA_DM_SEARCH *) p_src;

    if (!p_src) {
        return;
    }

    switch (msg->aid) {
    case BTA_DM_DISC_RES_EVT: {
        if (p_src_data->disc_res.p_raw_data && p_src_data->disc_res.raw_data_size > 0) {
            p_dest_data->disc_res.p_raw_data = (UINT8 *)(p_dest + sizeof(tBTA_DM_SEARCH));
            memcpy(p_dest_data->disc_res.p_raw_data,
                   p_src_data->disc_res.p_raw_data,
                   p_src_data->disc_res.raw_data_size);
        }
    }
    break;

    default:
        break;
    }
}

/*******************************************************************************
**
** Function         check_eir_remote_name
**
** Description      Check if remote name is in the EIR data
**
** Returns          TRUE if remote name found
**                  Populate p_remote_name, if provided and remote name found
**
*******************************************************************************/
static BOOLEAN check_eir_remote_name(tBTA_DM_SEARCH *p_search_data,
                                     UINT8 *p_remote_name, UINT8 *p_remote_name_len)
{
    UINT8 *p_eir_remote_name = NULL;
    UINT8 remote_name_len = 0;

    /* Check EIR for remote name and services */
    if (p_search_data->inq_res.p_eir) {
        p_eir_remote_name = BTM_CheckEirData(p_search_data->inq_res.p_eir,
                                             BTM_EIR_COMPLETE_LOCAL_NAME_TYPE, &remote_name_len);
        if (!p_eir_remote_name) {
            p_eir_remote_name = BTM_CheckEirData(p_search_data->inq_res.p_eir,
                                                 BTM_EIR_SHORTENED_LOCAL_NAME_TYPE, &remote_name_len);
        }

        if (p_eir_remote_name) {
            if (remote_name_len > BD_NAME_LEN) {
                remote_name_len = BD_NAME_LEN;
            }

            if (p_remote_name && p_remote_name_len) {
                memcpy(p_remote_name, p_eir_remote_name, remote_name_len);
                *(p_remote_name + remote_name_len) = 0;
                *p_remote_name_len = remote_name_len;
            }

            return TRUE;
        }
    }

    return FALSE;

}

/*******************************************************************************
**
** Function         bte_search_devices_evt
**
** Description      Switches context from BTE to BTIF for DM search events
**
** Returns          void
**
*******************************************************************************/
static void bte_search_devices_evt(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    UINT16 param_len = 0;

    if (p_data) {
        param_len += sizeof(tBTA_DM_SEARCH);
    }
    /* Allocate buffer to hold the pointers (deep copy). The pointers will point to the end of the tBTA_DM_SEARCH */
    switch (event) {
    case BTA_DM_INQ_RES_EVT: {
        if (p_data->inq_res.p_eir) {
            param_len += HCI_EXT_INQ_RESPONSE_LEN;
        }
    }
    break;

    case BTA_DM_DISC_RES_EVT: {
        if (p_data->disc_res.raw_data_size && p_data->disc_res.p_raw_data) {
            param_len += p_data->disc_res.raw_data_size;
        }
    }
    break;
    }

    /* if remote name is available in EIR, set the flag so that stack doesn't trigger RNR */
    if (event == BTA_DM_INQ_RES_EVT) {
        p_data->inq_res.remt_name_not_required = check_eir_remote_name(p_data, NULL, NULL);
    }

    do {
        btc_msg_t msg;
        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_GAP_BT;
        msg.act = BTC_GAP_BT_ACT_SEARCH_DEVICES;
        msg.aid = event;

        btc_transfer_context(&msg, p_data, param_len,
                             (param_len > sizeof(tBTA_DM_SEARCH)) ? search_devices_copy_cb : NULL);
    } while (0);
}

static void btc_gap_bt_search_devices_evt(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    switch (event) {
    case BTA_DM_DISC_RES_EVT: {
        /* remote name update */
        uint32_t bdname_len = strlen((const char *)p_data->disc_res.bd_name);
        if (bdname_len) {
            esp_bt_gap_dev_prop_t prop[1];

            BTC_STORAGE_FILL_PROPERTY(&prop[0], ESP_BT_GAP_DEV_PROP_BDNAME, bdname_len + 1, p_data->disc_res.bd_name);

            esp_bt_gap_cb_param_t param;
            bdcpy(param.disc_res.bda, p_data->disc_res.bd_addr);
            param.disc_res.num_prop = 1;
            param.disc_res.prop = prop;
            btc_gap_bt_cb_to_app(ESP_BT_GAP_DISC_RES_EVT, &param);
        }
        break;
    }
    case BTA_DM_INQ_RES_EVT: {
        /* inquiry result */
        uint32_t cod = devclass2uint (p_data->inq_res.dev_class);

        if (cod == 0) {
            LOG_DEBUG("%s cod is 0, set as unclassified", __func__);
            cod = COD_UNCLASSIFIED;
        }

        do {
            esp_bt_gap_dev_prop_t prop[3];
            int num_prop = 0;

            memset(prop, 0, sizeof(prop));
            BTC_STORAGE_FILL_PROPERTY(&prop[0], ESP_BT_GAP_DEV_PROP_COD, sizeof(cod), &cod);
            num_prop++;

            BTC_STORAGE_FILL_PROPERTY(&prop[1], ESP_BT_GAP_DEV_PROP_RSSI, 1, &(p_data->inq_res.rssi));
            num_prop++;

            if (p_data->inq_res.p_eir) {
                BTC_STORAGE_FILL_PROPERTY(&prop[2], ESP_BT_GAP_DEV_PROP_EIR, HCI_EXT_INQ_RESPONSE_LEN, p_data->inq_res.p_eir);
                num_prop++;
            }

            /* Callback to notify upper layer of device */
            esp_bt_gap_cb_param_t param;
            bdcpy(param.disc_res.bda, p_data->inq_res.bd_addr);
            param.disc_res.num_prop = num_prop;
            param.disc_res.prop = prop;
            btc_gap_bt_cb_to_app(ESP_BT_GAP_DISC_RES_EVT, &param);
        } while (0);
    }
    break;

    case BTA_DM_INQ_CMPL_EVT:
        break;
    case BTA_DM_DISC_CMPL_EVT: {
        esp_bt_gap_cb_param_t param;
        param.disc_st_chg.state = ESP_BT_GAP_DISCOVERY_STOPPED;
        btc_gap_bt_cb_to_app(ESP_BT_GAP_DISC_STATE_CHANGED_EVT, &param);
        break;
    }
    case BTA_DM_SEARCH_CANCEL_CMPL_EVT: {
        /* if inquiry is not in progress and we get a cancel event, then
         * it means we are done with inquiry, but remote_name fetches are in
         * progress
         *
         * if inquiry is in progress, then we don't want to act on this cancel_cmpl_evt
         * but instead wait for the cancel_cmpl_evt_via the busy level
         */
        if (btc_gap_bt_inquiry_in_progress == false) {
            esp_bt_gap_cb_param_t param;
            param.disc_st_chg.state = ESP_BT_GAP_DISCOVERY_STOPPED;
            btc_gap_bt_cb_to_app(ESP_BT_GAP_DISC_STATE_CHANGED_EVT, &param);
        }
        break;
    }
    }
}
/*******************************************************************************
**
** Function         btc_gap_bt_search_service_record
**
** Description      Executes search service record event in btif context
**
** Returns          void
**
*******************************************************************************/
static void btc_gap_bt_search_service_record(UINT16 event, char *p_param)
{
    tBTA_DM_SEARCH *p_data = (tBTA_DM_SEARCH *)p_param;

    switch (event) {
    case BTA_DM_DISC_RES_EVT: {
        esp_bt_gap_cb_param_t param;
        memcpy(param.rmt_srvcs.bda, p_data->disc_res.bd_addr, BD_ADDR_LEN);
        if (p_data->disc_res.p_raw_data && p_data->disc_res.raw_data_size > 0) {
            param.rmt_srvc_rec.stat = ESP_BT_STATUS_SUCCESS;
            // param.rmt_srvc_rec.raw_data_size = p_data->disc_res.raw_data_size;
            // param.rmt_srvc_rec.raw_data = p_data->disc_res.p_raw_data;
        } else {
            param.rmt_srvc_rec.stat = ESP_BT_STATUS_FAIL;
            // param.rmt_srvc_rec.raw_data_size = 0;
            // param.rmt_srvc_rec.raw_data = NULL;
        }
        btc_gap_bt_cb_to_app(ESP_BT_GAP_RMT_SRVC_REC_EVT, &param);
    }
    break;
    case BTA_DM_DISC_CMPL_EVT:
    default:
        break;
    }
}


/*******************************************************************************
**
** Function         bte_dm_remote_service_record_evt
**
** Description      Switches context from BTE to BTC for DM search service
**                  record event
**
** Returns          void
**
*******************************************************************************/
static void bte_dm_remote_service_record_evt(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    UINT16 param_len = 0;

    if (p_data) {
        param_len += sizeof(tBTA_DM_SEARCH);
    }
    /* Allocate buffer to hold the pointers (deep copy). The pointers will point to the end of the tBTA_DM_SEARCH */
    if (event == BTA_DM_DISC_RES_EVT) {
        if (p_data->disc_res.raw_data_size && p_data->disc_res.p_raw_data) {
            param_len += p_data->disc_res.raw_data_size;
        }
    }

    do {
        btc_msg_t msg;
        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_GAP_BT;
        msg.act = BTC_GAP_BT_ACT_SEARCH_SERVICE_RECORD;
        msg.aid = event;

        btc_transfer_context(&msg, p_data, param_len,
                             (param_len > sizeof(tBTA_DM_SEARCH)) ? search_service_record_copy_cb : NULL);

    } while (0);

}

/*******************************************************************************
**
** Function         btc_gap_bt_search_services
**
** Description      Executes search services event in btc context
**
** Returns          void
**
*******************************************************************************/
static void btc_gap_bt_search_services(UINT16 event, char *p_param)
{
    tBTA_DM_SEARCH *p_data = (tBTA_DM_SEARCH *)p_param;

    switch (event) {
    case BTA_DM_DISC_RES_EVT: {
        esp_bt_gap_cb_param_t param;
        esp_bt_uuid_t *uuid_list = NULL;
        memcpy(param.rmt_srvcs.bda, p_data->disc_res.bd_addr, BD_ADDR_LEN);

        param.rmt_srvcs.stat = ESP_BT_STATUS_FAIL;
        if (p_data->disc_res.result == BTA_SUCCESS) {
            uuid_list = malloc(sizeof(esp_bt_uuid_t) * p_data->disc_res.num_uuids);
            if (uuid_list) {
                param.rmt_srvcs.stat = ESP_BT_STATUS_SUCCESS;
                param.rmt_srvcs.num_uuids = p_data->disc_res.num_uuids;
                param.rmt_srvcs.uuid_list = uuid_list;
                // copy UUID list
                uint8_t *i_uu = (uint8_t *)p_data->disc_res.p_uuid_list;
                esp_bt_uuid_t *o_uu = uuid_list;
                for (int i = 0; i < p_data->disc_res.num_uuids; i++, i_uu += ESP_UUID_LEN_128, o_uu++) {
                    uuid128_be_to_esp_uuid(o_uu, i_uu);
                }
            }
        }

        if (param.rmt_srvcs.stat == ESP_BT_STATUS_FAIL) {
            param.rmt_srvcs.num_uuids = 0;
            param.rmt_srvcs.uuid_list = NULL;
        }
        btc_gap_bt_cb_to_app(ESP_BT_GAP_RMT_SRVCS_EVT, &param);

        if (uuid_list) {
            osi_free(uuid_list);
        }
    }
    break;

    case BTA_DM_DISC_BLE_RES_EVT:
    case BTA_DM_DISC_CMPL_EVT:
    default:
        break;
    }
}

/*******************************************************************************
**
** Function         bte_dm_search_services_evt
**
** Description      Switches context from BTE to BTIF for DM search services
**                  event
**
** Returns          void
**
*******************************************************************************/
static void bte_dm_search_services_evt(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    UINT16 param_len = 0;
    if (p_data) {
        param_len += sizeof(tBTA_DM_SEARCH);
    }

    switch (event) {
    case BTA_DM_DISC_RES_EVT: {
        if ((p_data->disc_res.result == BTA_SUCCESS) && (p_data->disc_res.num_uuids > 0)) {
            param_len += (p_data->disc_res.num_uuids * MAX_UUID_SIZE);
        }
    } break;
    }

    /* TODO: The only other member that needs a deep copy is the p_raw_data. But not sure
     * if raw_data is needed. */
    do {
        btc_msg_t msg;
        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_GAP_BT;
        msg.act = BTC_GAP_BT_ACT_SEARCH_SERVICES;
        msg.aid = event;

        btc_transfer_context(&msg, p_data, param_len,
                             (param_len > sizeof(tBTA_DM_SEARCH)) ? search_services_copy_cb : NULL);
    } while (0);
}

static void search_services_copy_cb(btc_msg_t *msg, void *p_dest, void *p_src)
{
    tBTA_DM_SEARCH *p_dest_data =  (tBTA_DM_SEARCH *) p_dest;
    tBTA_DM_SEARCH *p_src_data =  (tBTA_DM_SEARCH *) p_src;

    if (!p_src) {
        return;
    }

    switch (msg->aid) {
    case BTA_DM_DISC_RES_EVT: {
        if (p_src_data->disc_res.result == BTA_SUCCESS) {
            if (p_src_data->disc_res.num_uuids > 0) {
                p_dest_data->disc_res.p_uuid_list = (UINT8 *)(p_dest + sizeof(tBTA_DM_SEARCH));
                memcpy(p_dest_data->disc_res.p_uuid_list, p_src_data->disc_res.p_uuid_list,
                       p_src_data->disc_res.num_uuids * MAX_UUID_SIZE);
                osi_free(p_src_data->disc_res.p_uuid_list);
                p_src_data->disc_res.p_uuid_list = NULL;
            }
            if (p_src_data->disc_res.p_raw_data != NULL) {
                osi_free(p_src_data->disc_res.p_raw_data);
                p_src_data->disc_res.p_raw_data = NULL;
            }
        }
    } break;
    }
}

void btc_gap_bt_call_handler(btc_msg_t *msg)
{
    btc_gap_bt_args_t *arg = (btc_gap_bt_args_t *)msg->arg;
    LOG_DEBUG("%s act %d\n", __func__, msg->act);
    switch (msg->act) {
    case BTC_GAP_BT_ACT_SET_SCAN_MODE: {
        btc_bt_set_scan_mode(arg->set_scan_mode.mode);
        break;
    }
    case BTC_GAP_BT_ACT_START_DISCOVERY: {
        btc_gap_bt_start_discovery(msg->arg);
        break;
    }
    case BTC_GAP_BT_ACT_SEARCH_DEVICES: {
        btc_gap_bt_search_devices_evt(msg->aid, msg->arg);
        break;
    }
    case BTC_GAP_BT_ACT_CANCEL_DISCOVERY: {
        btc_gap_bt_cancel_discovery();
        break;
    }
    case BTC_GAP_BT_ACT_GET_REMOTE_SERVICES: {
        btc_gap_bt_get_remote_services(msg->arg);
        break;
    }
    case BTC_GAP_BT_ACT_SEARCH_SERVICES: {
        btc_gap_bt_search_services(msg->aid, msg->arg);
        break;
    }
    case BTC_GAP_BT_ACT_GET_REMOTE_SERVICE_RECORD: {
        btc_gap_bt_get_remote_service_record(msg->arg);
        break;
    }
    case BTC_GAP_BT_ACT_SEARCH_SERVICE_RECORD: {
        btc_gap_bt_search_service_record(msg->aid, msg->arg);
        break;
    }
    default:
        break;
    }

    return;
}

void btc_gap_bt_busy_level_updated(uint8_t bl_flags)
{
    esp_bt_gap_cb_param_t param;

    if (bl_flags == BTM_BL_INQUIRY_STARTED) {
        param.disc_st_chg.state = ESP_BT_GAP_DISCOVERY_STARTED;
        btc_gap_bt_cb_to_app(ESP_BT_GAP_DISC_STATE_CHANGED_EVT, &param);
        btc_gap_bt_inquiry_in_progress = true;
    } else if (bl_flags == BTM_BL_INQUIRY_CANCELLED ||
               bl_flags == BTM_BL_INQUIRY_COMPLETE) {
        param.disc_st_chg.state = ESP_BT_GAP_DISCOVERY_STOPPED;
        btc_gap_bt_cb_to_app(ESP_BT_GAP_DISC_STATE_CHANGED_EVT, &param);
        btc_gap_bt_inquiry_in_progress = false;
    }
}

#endif /* (BTC_GAP_BT_INCLUDED == TRUE) */
