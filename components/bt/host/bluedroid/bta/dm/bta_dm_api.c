/******************************************************************************
 *
 *  Copyright (C) 2003-2014 Broadcom Corporation
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

/******************************************************************************
 *
 *  This is the API implementation file for the BTA device manager.
 *
 ******************************************************************************/

#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta_dm_int.h"
#include "bta_sys_int.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include <string.h>
#include <assert.h>
#include "bta/utl.h"
#include "osi/allocator.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_dm_reg = {
    bta_dm_sm_execute,
    bta_dm_sm_disable
};

static const tBTA_SYS_REG bta_dm_search_reg = {
    bta_dm_search_sm_execute,
    bta_dm_search_sm_disable
};

/*******************************************************************************
**
** Function         BTA_EnableBluetooth
**
** Description      Enables bluetooth service.  This function must be
**                  called before any other functions in the BTA API are called.
**
**
** Returns          tBTA_STATUS
**
*******************************************************************************/
tBTA_STATUS BTA_EnableBluetooth(tBTA_DM_SEC_CBACK *p_cback)
{

    tBTA_DM_API_ENABLE    *p_msg;

    /* Bluetooth disabling is in progress */
    if (bta_dm_cb.disabling) {
        return BTA_FAILURE;
    }

    memset(&bta_dm_cb, 0, sizeof(bta_dm_cb));

    bta_sys_register (BTA_ID_DM, &bta_dm_reg );
    bta_sys_register (BTA_ID_DM_SEARCH, &bta_dm_search_reg );

#if (CLASSIC_BT_INCLUDED == TRUE)
    /* if UUID list is not provided as static data */
    bta_sys_eir_register(bta_dm_eir_update_uuid);
#endif // (CLASSIC_BT_INCLUDED == TRUE)

    if ((p_msg = (tBTA_DM_API_ENABLE *) osi_malloc(sizeof(tBTA_DM_API_ENABLE))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_ENABLE_EVT;
        p_msg->p_sec_cback = p_cback;
        bta_sys_sendmsg(p_msg);
        return BTA_SUCCESS;
    }
    return BTA_FAILURE;

}

/*******************************************************************************
**
** Function         BTA_DisableBluetooth
**
** Description      Disables bluetooth service.  This function is called when
**                  the application no longer needs bluetooth service
**
** Returns          void
**
*******************************************************************************/
tBTA_STATUS BTA_DisableBluetooth(void)
{

    BT_HDR    *p_msg;

    if ((p_msg = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_msg->event = BTA_DM_API_DISABLE_EVT;
        bta_sys_sendmsg(p_msg);
    } else {
        return BTA_FAILURE;
    }

    return BTA_SUCCESS;
}

/*******************************************************************************
**
** Function         BTA_DmSetDeviceName
**
** Description      This function sets the Bluetooth name of local device
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSetDeviceName(const char *p_name, tBT_DEVICE_TYPE name_type)
{

    tBTA_DM_API_SET_NAME    *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_NAME *) osi_malloc(sizeof(tBTA_DM_API_SET_NAME))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_NAME_EVT;
        /* truncate the name if needed */
        BCM_STRNCPY_S((char *)p_msg->name, p_name, BD_NAME_LEN);
        p_msg->name[BD_NAME_LEN] = '\0';
        p_msg->name_type = name_type;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmGetDeviceName
**
** Description      This function gets the Bluetooth name of local device
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmGetDeviceName(tBTA_GET_DEV_NAME_CBACK *p_cback, tBT_DEVICE_TYPE name_type)
{
    tBTA_DM_API_GET_NAME *p_msg;

    if ((p_msg = (tBTA_DM_API_GET_NAME *) osi_malloc(sizeof(tBTA_DM_API_GET_NAME))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_GET_NAME_EVT;
        p_msg->p_cback = p_cback;
        p_msg->name_type = name_type;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmCfgCoexStatus
**
** Description      This function configures the coexist status
**
**
** Returns          void
**
*******************************************************************************/
#if (ESP_COEX_VSC_INCLUDED == TRUE)
void BTA_DmCfgCoexStatus(UINT8 op, UINT8 type, UINT8 status)
{
    tBTA_DM_API_CFG_COEX_STATUS *p_msg;

    if ((p_msg = (tBTA_DM_API_CFG_COEX_STATUS *) osi_malloc(sizeof(tBTA_DM_API_CFG_COEX_STATUS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CFG_COEX_ST_EVT;
        p_msg->op = op;
        p_msg->type = type;
        p_msg->status = status;
        bta_sys_sendmsg(p_msg);
    }
}
#endif

#if (BLE_VENDOR_HCI_EN == TRUE)
void BTA_DmsendVendorHciCmd(UINT16 opcode, UINT8 param_len, UINT8 *p_param_buf, tBTA_SEND_VENDOR_HCI_CMPL_CBACK p_vendor_cmd_complete_cback)
{
    tBTA_DM_API_SEND_VENDOR_HCI_CMD *p_msg;
    if ((p_msg = (tBTA_DM_API_SEND_VENDOR_HCI_CMD *)osi_malloc(sizeof(tBTA_DM_API_SEND_VENDOR_HCI_CMD) + param_len)) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SEND_VENDOR_HCI_CMD_EVT;
        p_msg->opcode = opcode;
        p_msg->param_len = param_len;
        p_msg->p_param_buf = (UINT8 *)(p_msg + 1);
        memcpy(p_msg->p_param_buf, p_param_buf, param_len);
        p_msg->vendor_hci_cb = p_vendor_cmd_complete_cback;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmBleClearAdv
**
** Description      This function is called to clear Advertising
**
** Parameters       p_adv_data_cback : clear adv complete callback.
**
** Returns          None
**
*******************************************************************************/
void BTA_DmBleClearAdv (tBTA_CLEAR_ADV_CMPL_CBACK *p_clear_adv_cback)
{
    tBTA_DM_API_CLEAR_ADV  *p_msg;

    if ((p_msg = (tBTA_DM_API_CLEAR_ADV *)
                 osi_malloc(sizeof(tBTA_DM_API_CLEAR_ADV))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_CLEAR_ADV_EVT;
        p_msg->p_clear_adv_cback = p_clear_adv_cback;

        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapSetCsaSupport(uint8_t csa_select, tBTA_SET_CSA_SUPPORT_CMPL_CBACK *p_callback)
{
    tBTA_DM_API_BLE_SET_CSA_SUPPORT *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_CSA_SUPPORT *)osi_malloc(sizeof(tBTA_DM_API_BLE_SET_CSA_SUPPORT)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_SET_CSA_SUPPORT_EVT;
        p_msg->csa_select = csa_select;
        p_msg->p_cback = p_callback;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapSetVendorEventMask(uint32_t evt_mask, tBTA_SET_VENDOR_EVT_MASK_CBACK *p_callback)
{
    tBTA_DM_API_BLE_SET_VENDOR_EVT_MASK *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_VENDOR_EVT_MASK *)osi_malloc(sizeof(tBTA_DM_API_BLE_SET_VENDOR_EVT_MASK)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_SET_VENDOR_EVT_MASK_EVT;
        p_msg->evt_mask = evt_mask;
        p_msg->p_cback = p_callback;
        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)

#if (CLASSIC_BT_INCLUDED == TRUE)

void BTA_DmConfigEir(tBTA_DM_EIR_CONF *eir_config)
{
    tBTA_DM_API_CONFIG_EIR    *p_msg;

    UINT8 eir_manufac_spec_len = eir_config->bta_dm_eir_manufac_spec_len;
    UINT8 eir_url_len = eir_config->bta_dm_eir_url_len;

    if (eir_manufac_spec_len > HCI_EXT_INQ_RESPONSE_LEN) {
        APPL_TRACE_WARNING ("%s: Manufacturer data is too long(%d), cut it to %d\n",
                            __func__, eir_manufac_spec_len, HCI_EXT_INQ_RESPONSE_LEN);
        eir_manufac_spec_len = HCI_EXT_INQ_RESPONSE_LEN;
    }
    if (eir_url_len > HCI_EXT_INQ_RESPONSE_LEN) {
        APPL_TRACE_WARNING ("%s: URL is too long(%d), cut it to %d\n",
                            __func__, eir_url_len, HCI_EXT_INQ_RESPONSE_LEN);
        eir_url_len = HCI_EXT_INQ_RESPONSE_LEN;
    }

    if ((p_msg = (tBTA_DM_API_CONFIG_EIR *) osi_malloc(sizeof(tBTA_DM_API_CONFIG_EIR) + eir_manufac_spec_len + eir_url_len)) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CONFIG_EIR_EVT;

        p_msg->eir_fec_required = eir_config->bta_dm_eir_fec_required;
        p_msg->eir_included_name = eir_config->bta_dm_eir_included_name;
        p_msg->eir_included_tx_power = eir_config->bta_dm_eir_included_tx_power;
        p_msg->eir_included_uuid = eir_config->bta_dm_eir_included_uuid;
        p_msg->eir_flags = eir_config->bta_dm_eir_flags;
        p_msg->eir_manufac_spec_len = eir_manufac_spec_len;
        p_msg->eir_manufac_spec = p_msg->data;
        p_msg->eir_url_len = eir_url_len;
        p_msg->eir_url = p_msg->data + eir_manufac_spec_len;

        if (eir_manufac_spec_len > 0) {
            memcpy(p_msg->eir_manufac_spec, eir_config->bta_dm_eir_manufac_spec, eir_manufac_spec_len);
        }

        if (eir_url_len > 0) {
            memcpy(p_msg->eir_url, eir_config->bta_dm_eir_url, eir_url_len);
        }

        bta_sys_sendmsg(p_msg);
    }
}


/*******************************************************************************
**
** Function         BTA_DmSetAfhChannels
**
** Description      This function sets the AFH channels
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSetAfhChannels(const uint8_t *channels, tBTA_CMPL_CB  *set_afh_cb)
{
    tBTA_DM_API_SET_AFH_CHANNELS *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_AFH_CHANNELS *) osi_malloc(sizeof(tBTA_DM_API_SET_AFH_CHANNELS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_AFH_CHANNELS_EVT;

        p_msg->set_afh_cb = set_afh_cb;
        memcpy(p_msg->channels, channels, AFH_CHANNELS_LEN);

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmSetPageTimeout
**
** Description      This function sets the Bluetooth page timeout.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSetPageTimeout(UINT16 page_to, tBTM_CMPL_CB *p_cb)
{
    tBTA_DM_API_PAGE_TO_SET *p_msg;

    if ((p_msg = (tBTA_DM_API_PAGE_TO_SET *) osi_malloc(sizeof(tBTA_DM_API_PAGE_TO_SET))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_PAGE_TO_SET_EVT;
        p_msg->page_to = page_to;
        p_msg->set_page_to_cb = p_cb;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmGetPageTimeout
**
** Description      This function gets the Bluetooth page timeout.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmGetPageTimeout(tBTM_CMPL_CB *p_cb)
{
    tBTA_DM_API_PAGE_TO_GET *p_msg;

    if ((p_msg = (tBTA_DM_API_PAGE_TO_GET *) osi_malloc(sizeof(tBTA_DM_API_PAGE_TO_GET))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_PAGE_TO_GET_EVT;
        p_msg->get_page_to_cb = p_cb;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmSetAclPktTypes
**
** Description      This function sets the packet types used for ACL traffic.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSetAclPktTypes(BD_ADDR remote_addr, UINT16 pkt_types, tBTM_CMPL_CB *p_cb)
{
    tBTA_DM_API_SET_ACL_PKT_TYPES *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_ACL_PKT_TYPES *) osi_malloc(sizeof(tBTA_DM_API_SET_ACL_PKT_TYPES))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_ACL_PKT_TYPES_EVT;
        bdcpy(p_msg->rmt_addr, remote_addr);
        p_msg->pkt_types = pkt_types;
        p_msg->set_acl_pkt_types_cb = p_cb;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmSetMinEncKeySize
**
** Description      This function sets the minimal size of encryption key.
**
**
** Returns          void
**
*******************************************************************************/
#if (ENC_KEY_SIZE_CTRL_MODE != ENC_KEY_SIZE_CTRL_MODE_NONE)
void BTA_DmSetMinEncKeySize(UINT8 key_size, tBTM_CMPL_CB *p_cb)
{
    tBTA_DM_API_SET_MIN_ENC_KEY_SIZE *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_MIN_ENC_KEY_SIZE *) osi_malloc(sizeof(tBTA_DM_API_SET_MIN_ENC_KEY_SIZE))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_MIN_ENC_KEY_SIZE_EVT;
        p_msg->key_size = key_size;
        p_msg->set_min_enc_key_size_cb = p_cb;

        bta_sys_sendmsg(p_msg);
    }
}
#endif
#endif /// CLASSIC_BT_INCLUDED == TRUE

#if (SDP_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmGetRemoteName
**
** Description      This function gets the peer device's Bluetooth name.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmGetRemoteName(BD_ADDR remote_addr, tBTA_CMPL_CB *rmt_name_cb)
{
    tBTA_DM_API_GET_REMOTE_NAME *p_msg;

    if ((p_msg = (tBTA_DM_API_GET_REMOTE_NAME *) osi_malloc(sizeof(tBTA_DM_API_GET_REMOTE_NAME))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_GET_REMOTE_NAME_EVT;
        p_msg->rmt_name_cb = rmt_name_cb;
        bdcpy(p_msg->rmt_addr, remote_addr);
        bta_sys_sendmsg(p_msg);
    }
}
#endif

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmBleSetChannels
**
** Description      This function sets BLE channels
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleSetChannels(const uint8_t *channels, tBTA_CMPL_CB  *set_channels_cb)
{

    tBTA_DM_API_BLE_SET_CHANNELS *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_CHANNELS *) osi_malloc(sizeof(tBTA_DM_API_BLE_SET_CHANNELS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_SET_CHANNELS_EVT;

        p_msg->set_channels_cb = set_channels_cb;
        memcpy(p_msg->channels, channels, BLE_CHANNELS_LEN);

        bta_sys_sendmsg(p_msg);
    }


}

void BTA_DmUpdateWhiteList(BOOLEAN add_remove,  BD_ADDR remote_addr, tBLE_ADDR_TYPE addr_type, tBTA_UPDATE_WHITELIST_CBACK *update_wl_cb)
{
    tBTA_DM_API_UPDATE_WHITE_LIST *p_msg;
    if ((p_msg = (tBTA_DM_API_UPDATE_WHITE_LIST *)osi_malloc(sizeof(tBTA_DM_API_UPDATE_WHITE_LIST))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_UPDATE_WHITE_LIST_EVT;
        p_msg->add_remove = add_remove;
        p_msg->addr_type = addr_type;
        p_msg->update_wl_cb = update_wl_cb;
        memcpy(p_msg->remote_addr, remote_addr, sizeof(BD_ADDR));

        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmClearWhiteList(tBTA_UPDATE_WHITELIST_CBACK *update_wl_cb)
{
    tBTA_DM_API_UPDATE_WHITE_LIST *p_msg;
    if ((p_msg = (tBTA_DM_API_UPDATE_WHITE_LIST *)osi_malloc(sizeof(tBTA_DM_API_UPDATE_WHITE_LIST))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CLEAR_WHITE_LIST_EVT;
        p_msg->update_wl_cb = update_wl_cb;

        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleReadChannelMap(BD_ADDR remote_device, tBTA_CMPL_CB *p_callback)
{
    if (!remote_device || !p_callback) {
        return;
    }
    tBTA_DM_API_READ_CH_MAP *p_msg;

    if ((p_msg = (tBTA_DM_API_READ_CH_MAP *)osi_malloc(sizeof(tBTA_DM_API_READ_CH_MAP))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_READ_CH_MAP_EVT;
        memcpy(p_msg->remote_addr, remote_device, sizeof(BD_ADDR));
        p_msg->read_ch_map_cb = p_callback;
        bta_sys_sendmsg(p_msg);
    }
}
#endif  ///BLE_INCLUDED == TRUE

void BTA_DmReadRSSI(BD_ADDR remote_addr, tBTA_TRANSPORT transport, tBTA_CMPL_CB *cmpl_cb)
{
    tBTA_DM_API_READ_RSSI *p_msg;
    if ((p_msg = (tBTA_DM_API_READ_RSSI *)osi_malloc(sizeof(tBTA_DM_API_READ_RSSI))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_READ_RSSI_EVT;
        memcpy(p_msg->remote_addr, remote_addr, sizeof(BD_ADDR));
        p_msg->transport = transport;
        p_msg->read_rssi_cb = cmpl_cb;
        bta_sys_sendmsg(p_msg);
    }
}

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmSetVisibility
**
** Description      This function sets the Bluetooth connectable,
**                  discoverable, pairable and conn paired only modes of local device
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSetVisibility(tBTA_DM_DISC disc_mode, tBTA_DM_CONN conn_mode, UINT8 pairable_mode, UINT8 conn_filter )
{

    tBTA_DM_API_SET_VISIBILITY    *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_VISIBILITY *) osi_malloc(sizeof(tBTA_DM_API_SET_VISIBILITY))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_VISIBILITY_EVT;
        p_msg->disc_mode = disc_mode;
        p_msg->conn_mode = conn_mode;
        p_msg->pair_mode = pairable_mode;
        p_msg->conn_paired_only = conn_filter;


        bta_sys_sendmsg(p_msg);
    }


}
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

/*******************************************************************************
**
** Function         BTA_DmSearch
**
** Description      This function searches for peer Bluetooth devices. It performs
**                  an inquiry and gets the remote name for devices. Service
**                  discovery is done if services is non zero
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSearch(tBTA_DM_INQ *p_dm_inq, tBTA_SERVICE_MASK services, tBTA_DM_SEARCH_CBACK *p_cback)
{

    tBTA_DM_API_SEARCH    *p_msg;

    if ((p_msg = (tBTA_DM_API_SEARCH *) osi_malloc(sizeof(tBTA_DM_API_SEARCH))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SEARCH));

        p_msg->hdr.event = BTA_DM_API_SEARCH_EVT;
        memcpy(&p_msg->inq_params, p_dm_inq, sizeof(tBTA_DM_INQ));
        p_msg->services = services;
        p_msg->p_cback = p_cback;
        p_msg->rs_res  = BTA_DM_RS_NONE;
        bta_sys_sendmsg(p_msg);
    }

}


/*******************************************************************************
**
** Function         BTA_DmSearchCancel
**
** Description      This function  cancels a search initiated by BTA_DmSearch
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSearchCancel(void)
{
    BT_HDR    *p_msg;

    if ((p_msg = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_msg->event = BTA_DM_API_SEARCH_CANCEL_EVT;
        bta_sys_sendmsg(p_msg);
    }

}

#if (SDP_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmDiscover
**
** Description      This function does service discovery for services of a
**                  peer device
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmDiscover(BD_ADDR bd_addr, tBTA_SERVICE_MASK services,
                    tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search)
{
    tBTA_DM_API_DISCOVER    *p_msg;

    if ((p_msg = (tBTA_DM_API_DISCOVER *) osi_malloc(sizeof(tBTA_DM_API_DISCOVER))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_DISCOVER));

        p_msg->hdr.event = BTA_DM_API_DISCOVER_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->services = services;
        p_msg->p_cback = p_cback;
        p_msg->sdp_search = sdp_search;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmDiscoverUUID
**
** Description      This function does service discovery for services of a
**                  peer device
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmDiscoverUUID(BD_ADDR bd_addr, tSDP_UUID *uuid,
                        tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search)
{
    tBTA_DM_API_DISCOVER    *p_msg;

    if ((p_msg = (tBTA_DM_API_DISCOVER *) osi_malloc(sizeof(tBTA_DM_API_DISCOVER))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_DISCOVER_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->services = BTA_USER_SERVICE_MASK; //Not exposed at API level
        p_msg->p_cback = p_cback;
        p_msg->sdp_search = sdp_search;

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
        p_msg->num_uuid = 0;
        p_msg->p_uuid = NULL;
#endif
        memcpy( &p_msg->uuid, uuid, sizeof(tSDP_UUID) );
        bta_sys_sendmsg(p_msg);
    }
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         BTA_DmBond
**
** Description      This function initiates a bonding procedure with a peer
**                  device
**
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void BTA_DmBond(BD_ADDR bd_addr)
{
    tBTA_DM_API_BOND    *p_msg;

    p_msg = (tBTA_DM_API_BOND *) osi_malloc(sizeof(tBTA_DM_API_BOND));
    if (p_msg != NULL) {
        p_msg->hdr.event = BTA_DM_API_BOND_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->transport = BTA_TRANSPORT_UNKNOWN;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmBondByTransports
**
** Description      This function initiates a bonding procedure with a peer
**                  device
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBondByTransport(BD_ADDR bd_addr, tBTA_TRANSPORT transport)
{
    tBTA_DM_API_BOND    *p_msg;

    if ((p_msg = (tBTA_DM_API_BOND *) osi_malloc(sizeof(tBTA_DM_API_BOND))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BOND_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->transport = transport;
        bta_sys_sendmsg(p_msg);
    }


}

/*******************************************************************************
**
** Function         BTA_DmBondCancel
**
** Description      This function cancels the bonding procedure with a peer
**                  device
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBondCancel(BD_ADDR bd_addr)
{
    tBTA_DM_API_BOND_CANCEL    *p_msg;

    if ((p_msg = (tBTA_DM_API_BOND_CANCEL *) osi_malloc(sizeof(tBTA_DM_API_BOND_CANCEL))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BOND_CANCEL_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        bta_sys_sendmsg(p_msg);
    }
}
#endif  ///SMP_INCLUDED == TRUE

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DMSetPinType
**
** Description      This function set pin type as BTM_PIN_TYPE_FIXED or BTM_PIN_TYPE_VARIABLE
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DMSetPinType (UINT8 pin_type, UINT8 *pin_code, UINT8 pin_code_len)
{
    tBTA_DM_API_SET_PIN_TYPE    *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_PIN_TYPE *) osi_malloc(sizeof(tBTA_DM_API_SET_PIN_TYPE))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_PIN_TYPE_EVT;
        p_msg->pin_type = pin_type;
        p_msg->pin_len = pin_code_len;
        memcpy(p_msg->p_pin, pin_code, pin_code_len);
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmPinReply
**
** Description      This function provides a pincode for a remote device when
**                  one is requested by DM through BTA_DM_PIN_REQ_EVT
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmPinReply(BD_ADDR bd_addr, BOOLEAN accept, UINT8 pin_len, UINT8 *p_pin)
{
    tBTA_DM_API_PIN_REPLY    *p_msg;

    if ((p_msg = (tBTA_DM_API_PIN_REPLY *) osi_malloc(sizeof(tBTA_DM_API_PIN_REPLY))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_PIN_REPLY_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->accept = accept;
        if (accept) {
            p_msg->pin_len = pin_len;
            memcpy(p_msg->p_pin, p_pin, pin_len);
        }
        bta_sys_sendmsg(p_msg);
    }

}
#endif  ///CLASSIC_BT_INCLUDED == TRUE

#if (BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmLocalOob
**
** Description      This function retrieves the OOB data from local controller.
**                  The result is reported by:
**                  - bta_dm_co_loc_oob_ext() if device supports secure
**                    connections (SC)
**                  - bta_dm_co_loc_oob() if device doesn't support SC
**
** Returns          void
**
*******************************************************************************/
void BTA_DmLocalOob(void)
{
    tBTA_DM_API_LOC_OOB    *p_msg;

    if ((p_msg = (tBTA_DM_API_LOC_OOB *) osi_malloc(sizeof(tBTA_DM_API_LOC_OOB))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_LOC_OOB_EVT;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmOobReply
**
**                  This function is called to provide the OOB data for
**                  SMP in response to BTA_LE_OOB_REQ_EVT
**
** Parameters:      bd_addr     - Address of the peer device
**                  len         - length of simple pairing Randomizer  C
**                  p_value     - simple pairing Randomizer  C.
**
** Returns          void
**
*******************************************************************************/
void BTA_DmOobReply(BD_ADDR bd_addr, UINT8 len, UINT8 *p_value)
{
    tBTA_DM_API_OOB_REPLY    *p_msg;

    if ((p_msg = (tBTA_DM_API_OOB_REPLY *) osi_malloc(sizeof(tBTA_DM_API_OOB_REPLY))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_OOB_REPLY_EVT;
        if(p_value == NULL || len > BT_OCTET16_LEN) {
            osi_free(p_msg);
            return;
        }
        memcpy(p_msg->bd_addr, bd_addr, BD_ADDR_LEN);
        p_msg->len = len;
        memcpy(p_msg->value, p_value, len);
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmSecureConnectionOobReply
**
**                  This function is called to provide the OOB data for
**                  SMP in response to BTA_LE_OOB_REQ_EVT
**
** Parameters:      bd_addr     - Address of the peer device
**                  p_c         - Pointer to Confirmation
**                  p_r         - Pointer to Randomizer
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSecureConnectionOobReply(BD_ADDR bd_addr, UINT8 *p_c, UINT8 *p_r)
{
    tBTA_DM_API_SC_OOB_REPLY    *p_msg;

    if ((p_msg = (tBTA_DM_API_SC_OOB_REPLY *) osi_malloc(sizeof(tBTA_DM_API_OOB_REPLY))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SC_OOB_REPLY_EVT;
        if((p_c == NULL) || (p_r == NULL)) {
            return;
        }
        memcpy(p_msg->bd_addr, bd_addr, BD_ADDR_LEN);
        memcpy(p_msg->c, p_c, BT_OCTET16_LEN);
        memcpy(p_msg->r, p_r, BT_OCTET16_LEN);
        bta_sys_sendmsg(p_msg);
    }
}
/*******************************************************************************
**
** Function         BTA_DmSecureConnectionCreateOobData
**
**                  This function is called to create the OOB data for
**                  SMP when secure connection
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSecureConnectionCreateOobData(void)
{
    tBTA_DM_API_SC_CR_OOB_DATA *p_msg;

    if ((p_msg = (tBTA_DM_API_SC_CR_OOB_DATA *) osi_malloc(sizeof(tBTA_DM_API_SC_CR_OOB_DATA))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_SC_CR_OOB_DATA_EVT;
        bta_sys_sendmsg(p_msg);
    }
}
#endif /* BTM_OOB_INCLUDED */
/*******************************************************************************
**
** Function         BTA_DmConfirm
**
** Description      This function accepts or rejects the numerical value of the
**                  Simple Pairing process on BTA_DM_SP_CFM_REQ_EVT
**
** Returns          void
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
void BTA_DmConfirm(BD_ADDR bd_addr, BOOLEAN accept)
{
    tBTA_DM_API_CONFIRM    *p_msg;

    if ((p_msg = (tBTA_DM_API_CONFIRM *) osi_malloc(sizeof(tBTA_DM_API_CONFIRM))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CONFIRM_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->accept = accept;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmPasskeyReqReply
**
** Description      This function is called to provide the passkey for
**                  Simple Pairing in response to BTA_DM_SP_KEY_REQ_EVT
**
** Returns          void
**
*******************************************************************************/
void BTA_DmPasskeyReqReply(BOOLEAN accept, BD_ADDR bd_addr, UINT32 passkey)
{
    tBTA_DM_API_KEY_REQ    *p_msg;
    if ((p_msg = (tBTA_DM_API_KEY_REQ *) osi_malloc(sizeof(tBTA_DM_API_KEY_REQ))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_KEY_REQ_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->accept = accept;
        p_msg->passkey = passkey;
        bta_sys_sendmsg(p_msg);
    }
}
#endif ///CLASSIC_BT_INCLUDED == TRUE
/*******************************************************************************
**
** Function         BTA_DmAddDevice
**
** Description      This function adds a device to the security database list of
**                  peer device
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmAddDevice(BD_ADDR bd_addr, DEV_CLASS dev_class, LINK_KEY link_key,
                     tBTA_SERVICE_MASK trusted_mask, BOOLEAN is_trusted,
                     UINT8 key_type, tBTA_IO_CAP io_cap, UINT8 pin_length,
                     UINT8 sc_support)
{

    tBTA_DM_API_ADD_DEVICE *p_msg;

    if ((p_msg = (tBTA_DM_API_ADD_DEVICE *) osi_malloc(sizeof(tBTA_DM_API_ADD_DEVICE))) != NULL) {
        memset (p_msg, 0, sizeof(tBTA_DM_API_ADD_DEVICE));

        p_msg->hdr.event = BTA_DM_API_ADD_DEVICE_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->tm = trusted_mask;
        p_msg->is_trusted = is_trusted;
        p_msg->io_cap = io_cap;
        p_msg->sc_support = sc_support;

        if (link_key) {
            p_msg->link_key_known = TRUE;
            p_msg->key_type = key_type;
            memcpy(p_msg->link_key, link_key, LINK_KEY_LEN);
        }

        /* Load device class if specified */
        if (dev_class) {
            p_msg->dc_known = TRUE;
            memcpy (p_msg->dc, dev_class, DEV_CLASS_LEN);
        }

        memset (p_msg->bd_name, 0, BD_NAME_LEN + 1);
        memset (p_msg->features, 0, sizeof (p_msg->features));
        p_msg->pin_length = pin_length;

        bta_sys_sendmsg(p_msg);
    }
}


/*******************************************************************************
**
** Function         BTA_DmRemoveDevice
**
** Description      This function removes a device from the security database list of
**                  peer device. It manages unpairing even while connected.
**
**
** Returns          void
**
*******************************************************************************/
tBTA_STATUS BTA_DmRemoveDevice(BD_ADDR bd_addr, tBT_TRANSPORT transport)
{
    tBTA_DM_API_REMOVE_DEVICE *p_msg;

    if ((p_msg = (tBTA_DM_API_REMOVE_DEVICE *) osi_malloc(sizeof(tBTA_DM_API_REMOVE_DEVICE))) != NULL) {
        memset (p_msg, 0, sizeof(tBTA_DM_API_REMOVE_DEVICE));

        p_msg->hdr.event = BTA_DM_API_REMOVE_DEVICE_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->transport = transport;
        bta_sys_sendmsg(p_msg);
    } else {
        return BTA_FAILURE;
    }

    return BTA_SUCCESS;
}
// #endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         BTA_GetEirService
**
** Description      This function is called to get BTA service mask from EIR.
**
** Parameters       p_eir - pointer of EIR significant part
**                  p_services - return the BTA service mask
**
** Returns          None
**
*******************************************************************************/
extern const UINT16 bta_service_id_to_uuid_lkup_tbl [];
void BTA_GetEirService( UINT8 *p_eir, tBTA_SERVICE_MASK *p_services )
{
    UINT8 xx, yy;
    UINT8 num_uuid, max_num_uuid = 32;
    UINT8 uuid_list[32 * LEN_UUID_16];
    UINT16 *p_uuid16 = (UINT16 *)uuid_list;
    tBTA_SERVICE_MASK mask;

    BTM_GetEirUuidList( p_eir, LEN_UUID_16, &num_uuid, uuid_list, max_num_uuid);
    for ( xx = 0; xx < num_uuid; xx++ ) {
        mask = 1;
        for ( yy = 0; yy < BTA_MAX_SERVICE_ID; yy++ ) {
            if ( *(p_uuid16 + xx) == bta_service_id_to_uuid_lkup_tbl[yy] ) {
                *p_services |= mask;
                break;
            }
            mask <<= 1;
        }

        /* for HSP v1.2 only device */
        if (*(p_uuid16 + xx) == UUID_SERVCLASS_HEADSET_HS) {
            *p_services |= BTA_HSP_SERVICE_MASK;
        }

        if (*(p_uuid16 + xx) == UUID_SERVCLASS_HDP_SOURCE) {
            *p_services |= BTA_HL_SERVICE_MASK;
        }

        if (*(p_uuid16 + xx) == UUID_SERVCLASS_HDP_SINK) {
            *p_services |= BTA_HL_SERVICE_MASK;
        }
    }
}

/*******************************************************************************
**
** Function         BTA_DmGetConnectionState
**
** Description      Returns whether the remote device is currently connected.
**
** Returns          0 if the device is NOT connected.
**
*******************************************************************************/
UINT16 BTA_DmGetConnectionState( BD_ADDR bd_addr )
{
    tBTA_DM_PEER_DEVICE *p_dev = bta_dm_find_peer_device(bd_addr);
    return (p_dev && p_dev->conn_state == BTA_DM_CONNECTED);
}

#if (SDP_INCLUDED == TRUE)
/*******************************************************************************
**                   Device Identification (DI) Server Functions
*******************************************************************************/
/*******************************************************************************
**
** Function         BTA_DmSetLocalDiRecord
**
** Description      This function adds a DI record to the local SDP database.
**
** Returns          BTA_SUCCESS if record set successfully, otherwise error code.
**
*******************************************************************************/
tBTA_STATUS BTA_DmSetLocalDiRecord( tBTA_DI_RECORD *p_device_info,
                                    UINT32 *p_handle )
{
    tBTA_STATUS status = BTA_FAILURE;

    if (bta_dm_di_cb.di_num < BTA_DI_NUM_MAX) {
        if (SDP_SetLocalDiRecord((tSDP_DI_RECORD *)p_device_info, p_handle) == SDP_SUCCESS) {
            if (!p_device_info->primary_record) {
                for (uint8_t i = 1; i < BTA_DI_NUM_MAX; i++) {
                    if (!bta_dm_di_cb.di_handle[i]) {
                        bta_dm_di_cb.di_handle[i] = *p_handle;
                        break;
                    }
                }
                bta_dm_di_cb.di_num++;
            } else if (!bta_dm_di_cb.di_handle[0]) {
                bta_dm_di_cb.di_handle[0] = *p_handle;
                bta_dm_di_cb.di_num++;
            } else {
                assert(bta_dm_di_cb.di_handle[0] == (*p_handle));
            }

            if (!bta_dm_di_cb.uuid_added) {
                bta_sys_add_uuid(UUID_SERVCLASS_PNP_INFORMATION);
                bta_dm_di_cb.uuid_added = TRUE;
            }

            status = BTA_SUCCESS;
        }
    }

    return status;
}

/*******************************************************************************
**
** Function         BTA_DmRemoveLocalDiRecord
**
** Description      This function removes a DI record from the local SDP database.
**
** Returns          BTA_SUCCESS if record is removed successfully, otherwise error code.
**
*******************************************************************************/
tBTA_STATUS BTA_DmRemoveLocalDiRecord(UINT32 handle)
{
    tBTA_STATUS status = BTA_FAILURE;

    for (uint8_t i = 0; i < BTA_DI_NUM_MAX; i++) {
        if (bta_dm_di_cb.di_handle[i] == handle) {
            if (SDP_DeleteRecord(handle)) {
                bta_dm_di_cb.di_handle[i] = 0;
                bta_dm_di_cb.di_num--;
                status = BTA_SUCCESS;
                break;
            }
        }
    }

    if (bta_dm_di_cb.di_num == 0 && bta_dm_di_cb.uuid_added) {
        bta_sys_remove_uuid(UUID_SERVCLASS_PNP_INFORMATION);
    }

    return status;
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         BTA_DmAddBleKey
**
** Description      Add/modify LE device information.  This function will be
**                  normally called during host startup to restore all required
**                  information stored in the NVRAM.
**
** Parameters:      bd_addr          - BD address of the peer
**                  p_le_key         - LE key values.
**                  key_type         - LE SMP key type.
**
** Returns          BTA_SUCCESS if successful
**                  BTA_FAIL if operation failed.
**
*******************************************************************************/
#if BLE_INCLUDED == TRUE
#if SMP_INCLUDED == TRUE
void BTA_DmAddBleKey (BD_ADDR bd_addr, tBTA_LE_KEY_VALUE *p_le_key, tBTA_LE_KEY_TYPE key_type)
{
    tBTA_DM_API_ADD_BLEKEY *p_msg;

    if ((p_msg = (tBTA_DM_API_ADD_BLEKEY *) osi_malloc(sizeof(tBTA_DM_API_ADD_BLEKEY))) != NULL) {
        memset (p_msg, 0, sizeof(tBTA_DM_API_ADD_BLEKEY));

        p_msg->hdr.event = BTA_DM_API_ADD_BLEKEY_EVT;
        p_msg->key_type = key_type;
        bdcpy(p_msg->bd_addr, bd_addr);
        memcpy(&p_msg->blekey, p_le_key, sizeof(tBTA_LE_KEY_VALUE));

        bta_sys_sendmsg(p_msg);
    }

}

/*******************************************************************************
**
** Function         BTA_DmAddBleDevice
**
** Description      Add a BLE device.  This function will be normally called
**                  during host startup to restore all required information
**                  for a LE device stored in the NVRAM.
**
** Parameters:      bd_addr          - BD address of the peer
**                  dev_type         - Remote device's device type.
**                  auth_mode        - auth mode
**                  addr_type        - LE device address type.
**
** Returns          void
**
*******************************************************************************/
void BTA_DmAddBleDevice(BD_ADDR bd_addr, tBLE_ADDR_TYPE addr_type, int auth_mode, tBT_DEVICE_TYPE dev_type)
{
    tBTA_DM_API_ADD_BLE_DEVICE *p_msg;

    if ((p_msg = (tBTA_DM_API_ADD_BLE_DEVICE *) osi_malloc(sizeof(tBTA_DM_API_ADD_BLE_DEVICE))) != NULL) {
        memset (p_msg, 0, sizeof(tBTA_DM_API_ADD_BLE_DEVICE));

        p_msg->hdr.event = BTA_DM_API_ADD_BLEDEVICE_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->addr_type = addr_type;
        p_msg->auth_mode = auth_mode;
        p_msg->dev_type = dev_type;

        bta_sys_sendmsg(p_msg);
    }
}
/*******************************************************************************
**
** Function         BTA_DmBlePasskeyReply
**
** Description      Send BLE SMP passkey reply.
**
** Parameters:      bd_addr          - BD address of the peer
**                  accept           - passkey entry successful or declined.
**                  passkey          - passkey value, must be a 6 digit number,
**                                     can be lead by 0.
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBlePasskeyReply(BD_ADDR bd_addr, BOOLEAN accept, UINT32 passkey)
{
    tBTA_DM_API_PASSKEY_REPLY    *p_msg;

    if ((p_msg = (tBTA_DM_API_PASSKEY_REPLY *) osi_malloc(sizeof(tBTA_DM_API_PASSKEY_REPLY))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PASSKEY_REPLY));

        p_msg->hdr.event = BTA_DM_API_BLE_PASSKEY_REPLY_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->accept = accept;

        if (accept) {
            p_msg->passkey = passkey;
        }
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleSetStaticPasskey(bool add, uint32_t passkey)
{
    tBTA_DM_API_SET_DEFAULT_PASSKEY    *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_DEFAULT_PASSKEY *) osi_malloc(sizeof(tBTA_DM_API_SET_DEFAULT_PASSKEY))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_DEFAULT_PASSKEY));

        p_msg->hdr.event = BTA_DM_API_BLE_SET_STATIC_PASSKEY_EVT;
        p_msg->add = add;
        p_msg->static_passkey = passkey;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmBleConfirmReply
**
** Description      Send BLE SMP SC user confirmation reply.
**
** Parameters:      bd_addr          - BD address of the peer
**                  accept           - numbers to compare are the same or different.
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleConfirmReply(BD_ADDR bd_addr, BOOLEAN accept)
{
    tBTA_DM_API_CONFIRM *p_msg = (tBTA_DM_API_CONFIRM *)osi_malloc(sizeof(tBTA_DM_API_CONFIRM));
    if (p_msg != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_CONFIRM));
        p_msg->hdr.event = BTA_DM_API_BLE_CONFIRM_REPLY_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->accept = accept;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmBleSecurityGrant
**
** Description      Grant security request access.
**
** Parameters:      bd_addr          - BD address of the peer
**                  res              - security grant status.
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleSecurityGrant(BD_ADDR bd_addr, tBTA_DM_BLE_SEC_GRANT res)
{
    tBTA_DM_API_BLE_SEC_GRANT    *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SEC_GRANT *) osi_malloc(sizeof(tBTA_DM_API_BLE_SEC_GRANT))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_SEC_GRANT));

        p_msg->hdr.event = BTA_DM_API_BLE_SEC_GRANT_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->res = res;

        bta_sys_sendmsg(p_msg);
    }
}
#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         BTA_DmSetBlePrefConnParams
**
** Description      This function is called to set the preferred connection
**                  parameters when default connection parameter is not desired.
**
** Parameters:      bd_addr          - BD address of the peripheral
**                  scan_interval    - scan interval
**                  scan_window      - scan window
**                  min_conn_int     - minimum preferred connection interval
**                  max_conn_int     - maximum preferred connection interval
**                  slave_latency    - preferred slave latency
**                  supervision_tout - preferred supervision timeout
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSetBlePrefConnParams(BD_ADDR bd_addr,
                                UINT16 min_conn_int, UINT16 max_conn_int,
                                UINT16 slave_latency, UINT16 supervision_tout )
{
#if BLE_INCLUDED == TRUE
    tBTA_DM_API_BLE_CONN_PARAMS    *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_CONN_PARAMS *) osi_malloc(sizeof(tBTA_DM_API_BLE_CONN_PARAMS))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_CONN_PARAMS));

        p_msg->hdr.event = BTA_DM_API_BLE_CONN_PARAM_EVT;

        memcpy(p_msg->peer_bda, bd_addr, BD_ADDR_LEN);

        p_msg->conn_int_max     = max_conn_int;
        p_msg->conn_int_min     = min_conn_int;
        p_msg->slave_latency    = slave_latency;
        p_msg->supervision_tout = supervision_tout;

        bta_sys_sendmsg(p_msg);
    }
#endif
}

#if (BLE_42_SCAN_EN == TRUE)
/*******************************************************************************
**
** Function         BTA_DmSetBleScanFilterParams
**
** Description      This function is called to set scan parameters
**
** Parameters:      client_if - Client IF
**                  scan_interval - scan interval
**                  scan_window - scan window
**                  scan_mode - scan mode
**                  scan_duplicate_filter - scan duplicate filter
**                  scan_param_setup_status_cback - Set scan param status callback
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSetBleScanFilterParams(tGATT_IF client_if, UINT32 scan_interval,
                                  UINT32 scan_window, tBLE_SCAN_MODE scan_mode, UINT8 scan_fil_poilcy,
                                  UINT8 addr_type_own, UINT8 scan_duplicate_filter, tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback)
{
    tBTA_DM_API_BLE_SCAN_FILTER_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SCAN_FILTER_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_BLE_SCAN_FILTER_PARAMS))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_SCAN_FILTER_PARAMS));
        p_msg->hdr.event = BTA_DM_API_BLE_SCAN_FIL_PARAM_EVT;
        p_msg->client_if = client_if;
        p_msg->scan_int = scan_interval;
        p_msg->scan_window = scan_window;
        p_msg->scan_mode = scan_mode;
        p_msg->addr_type_own = addr_type_own;
        p_msg->scan_duplicate_filter = scan_duplicate_filter;
        p_msg->scan_filter_policy = scan_fil_poilcy;
        p_msg->scan_param_setup_cback = scan_param_setup_cback;

        bta_sys_sendmsg(p_msg);
    }


}
#endif // #if (BLE_42_SCAN_EN == TRUE)

#if (BLE_42_ADV_EN == TRUE)
void BTA_DmSetBleAdvParamsAll (UINT16 adv_int_min, UINT16 adv_int_max,
                               UINT8 adv_type, tBLE_ADDR_TYPE addr_type_own,
                               tBTM_BLE_ADV_CHNL_MAP chnl_map, tBTM_BLE_AFP adv_fil_pol,
                               tBLE_BD_ADDR *p_dir_bda, tBTA_START_ADV_CMPL_CBACK p_start_adv_cb)
{
#if BLE_INCLUDED == TRUE
    tBTA_DM_API_BLE_ADV_PARAMS_ALL    *p_msg;

    APPL_TRACE_API ("BTA_DmSetBleAdvParamsAll: %d, %d\n", adv_int_min, adv_int_max);
    APPL_TRACE_API ("adv_type = %d, addr_type_own = %d, chnl_map = %d, adv_fil_pol = %d\n",
                      adv_type, addr_type_own, chnl_map, adv_fil_pol);
    if ((p_msg = (tBTA_DM_API_BLE_ADV_PARAMS_ALL *) osi_malloc(sizeof(tBTA_DM_API_BLE_ADV_PARAMS_ALL)
                 + sizeof(tBLE_BD_ADDR))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_ADV_PARAMS_ALL));

        p_msg->hdr.event = BTA_DM_API_BLE_ADV_PARAM_All_EVT;

        p_msg->adv_int_min      = adv_int_min;
        p_msg->adv_int_max      = adv_int_max;
        p_msg->adv_type         = adv_type;
        p_msg->addr_type_own    = addr_type_own;
        p_msg->channel_map      = chnl_map;
        p_msg->adv_filter_policy    = adv_fil_pol;
        p_msg->p_start_adv_cback    = p_start_adv_cb;
        if (p_dir_bda != NULL) {
            p_msg->p_dir_bda = (tBLE_BD_ADDR *)(p_msg + 1);
            memcpy(p_msg->p_dir_bda, p_dir_bda, sizeof(tBLE_BD_ADDR));
        }

        bta_sys_sendmsg(p_msg);
    }
#endif
}
#endif // #if (BLE_42_ADV_EN == TRUE)

#endif  ///BLE_INCLUDED == TRUE


/*******************************************************************************
**                      BLE ADV data management API
********************************************************************************/

#if BLE_INCLUDED == TRUE
#if (BLE_42_ADV_EN == TRUE)
/*******************************************************************************
**
** Function         BTA_DmBleSetAdvConfig
**
** Description      This function is called to override the BTA default ADV parameters.
**
** Parameters       data_mask: adv data mask.
**                  p_adv_cfg: Pointer to User defined ADV data structure. This
**                             memory space can not be freed until p_adv_data_cback
**                             is received.
**                  p_adv_data_cback: set adv data complete callback.
**
** Returns          None
**
*******************************************************************************/
void BTA_DmBleSetAdvConfig (tBTA_BLE_AD_MASK data_mask, tBTA_BLE_ADV_DATA *p_adv_cfg,
                            tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback)
{
    tBTA_DM_API_SET_ADV_CONFIG  *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_ADV_CONFIG *)
                 osi_malloc(sizeof(tBTA_DM_API_SET_ADV_CONFIG))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_SET_ADV_CONFIG_EVT;
        p_msg->data_mask = data_mask;
        p_msg->p_adv_data_cback = p_adv_data_cback;
        p_msg->p_adv_cfg = p_adv_cfg;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmBleSetAdvConfigRaw
**
** Description      This function is called to set raw Advertising data
**
** Parameters       p_raw_adv : raw advertising data.
**                  raw_adv_len : raw advertising data length.
**                  p_adv_data_cback : set adv data complete callback.
**
** Returns          None
**
*******************************************************************************/
void BTA_DmBleSetAdvConfigRaw (UINT8 *p_raw_adv, UINT32 raw_adv_len,
                            tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback)
{
    tBTA_DM_API_SET_ADV_CONFIG_RAW  *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_ADV_CONFIG_RAW *)
                 osi_malloc(sizeof(tBTA_DM_API_SET_ADV_CONFIG_RAW) + raw_adv_len)) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_SET_ADV_CONFIG_RAW_EVT;
        p_msg->p_adv_data_cback = p_adv_data_cback;
        p_msg->p_raw_adv = (UINT8 *)(p_msg + 1);
        memcpy(p_msg->p_raw_adv, p_raw_adv, raw_adv_len);
        p_msg->raw_adv_len = raw_adv_len;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmBleSetScanRsp
**
** Description      This function is called to override the BTA scan response.
**
** Parameters       Pointer to User defined ADV data structure
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleSetScanRsp (tBTA_BLE_AD_MASK data_mask, tBTA_BLE_ADV_DATA *p_adv_cfg,
                                 tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback)
{
    tBTA_DM_API_SET_ADV_CONFIG  *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_ADV_CONFIG *)
                 osi_malloc(sizeof(tBTA_DM_API_SET_ADV_CONFIG))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_SET_SCAN_RSP_EVT;
        p_msg->data_mask = data_mask;
        p_msg->p_adv_data_cback = p_adv_data_cback;
        p_msg->p_adv_cfg = p_adv_cfg;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmBleSetScanRspRaw
**
** Description      This function is called to set raw scan response data
**
** Parameters       p_raw_scan_rsp : raw scan_rspertising data.
**                  raw_scan_rsp_len : raw scan_rspertising data length.
**                  p_scan_rsp_data_cback : set scan_rsp data complete callback.
**
** Returns          None
**
*******************************************************************************/
void BTA_DmBleSetScanRspRaw (UINT8 *p_raw_scan_rsp, UINT32 raw_scan_rsp_len,
                            tBTA_SET_ADV_DATA_CMPL_CBACK *p_scan_rsp_data_cback)
{
    tBTA_DM_API_SET_ADV_CONFIG_RAW  *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_ADV_CONFIG_RAW *)
                 osi_malloc(sizeof(tBTA_DM_API_SET_ADV_CONFIG_RAW) + raw_scan_rsp_len)) != NULL) {
        p_msg->hdr.event = BTA_DM_API_BLE_SET_SCAN_RSP_RAW_EVT;
        p_msg->p_adv_data_cback = p_scan_rsp_data_cback;
        p_msg->p_raw_adv = (UINT8 *)(p_msg + 1);
        memcpy(p_msg->p_raw_adv, p_raw_scan_rsp, raw_scan_rsp_len);
        p_msg->raw_adv_len = raw_scan_rsp_len;

        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if (BLE_42_ADV_EN == TRUE)

#if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
/*******************************************************************************
**
** Function         BTA_DmUpdateDuplicateExceptionalList
**
** Description      This function is called to update duplicate scan exceptional list
**
** Parameters       subcode : add, remove or clean duplicate scan exceptional list.
**                  type : device info type.
**                  device_info:  device info
**                  p_update_duplicate_ignore_list_cback :  update complete callback.
**
** Returns          None
**
*******************************************************************************/
void BTA_DmUpdateDuplicateExceptionalList(UINT8 subcode, UINT32 type, BD_ADDR device_info, tBTA_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_CMPL_CBACK p_update_duplicate_exceptional_list_cback)
{
    tBTA_DM_API_UPDATE_DUPLICATE_EXCEPTIONAL_LIST *p_msg;
    if ((p_msg = (tBTA_DM_API_UPDATE_DUPLICATE_EXCEPTIONAL_LIST *)osi_malloc(sizeof(tBTA_DM_API_UPDATE_DUPLICATE_EXCEPTIONAL_LIST))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_EVT;
        p_msg->subcode = subcode;
        p_msg->type = type;
        p_msg->exceptional_list_cb = p_update_duplicate_exceptional_list_cback;
        memcpy(p_msg->device_info, device_info, sizeof(BD_ADDR));

        bta_sys_sendmsg(p_msg);
    }
}
#endif // ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))

#endif

/*******************************************************************************
**                      BLE ADV data management API
********************************************************************************/
#if BLE_INCLUDED == TRUE
#if (BLE_42_ADV_EN == TRUE)
/*******************************************************************************
**
** Function         BTA_DmBleAdvStop
**
** Description      This function starts or stops LE broadcasting.
**
** Parameters       start: always be false.
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleAdvStop (BOOLEAN start, tBTA_START_STOP_ADV_CMPL_CBACK *p_start_stop_adv_cb)
{
    tBTA_DM_API_BLE_OBSERVE   *p_msg;

    APPL_TRACE_API("BTA_DmBleAdvStop: start = %d \n", start);

    if ((p_msg = (tBTA_DM_API_BLE_OBSERVE *) osi_malloc(sizeof(tBTA_DM_API_BLE_OBSERVE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_OBSERVE));

        p_msg->hdr.event = BTA_DM_API_BLE_ADVSTOP_EVT;
        p_msg->start = start;
        if (start == FALSE){
            p_msg->p_stop_adv_cback= p_start_stop_adv_cb;
        }

        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if (BLE_42_ADV_EN == TRUE)

#endif

#if (BLE_GATT_BGCONN == TRUE)
/*******************************************************************************
**
** Function         BTA_DmBleSetBgConnType
**
** Description      This function is called to set BLE connectable mode for a
**                  peripheral device.
**
** Parameters       bg_conn_type: it can be auto connection, or selective connection.
**                  p_select_cback: callback function when selective connection procedure
**                              is being used.
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleSetBgConnType(tBTA_DM_BLE_CONN_TYPE bg_conn_type, tBTA_DM_BLE_SEL_CBACK *p_select_cback)
{
#if BLE_INCLUDED == TRUE
    tBTA_DM_API_BLE_SET_BG_CONN_TYPE    *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_BG_CONN_TYPE *) osi_malloc(sizeof(tBTA_DM_API_BLE_SET_BG_CONN_TYPE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_SET_BG_CONN_TYPE));

        p_msg->hdr.event        = BTA_DM_API_BLE_SET_BG_CONN_TYPE;
        p_msg->bg_conn_type     = bg_conn_type;
        p_msg->p_select_cback   = p_select_cback;

        bta_sys_sendmsg(p_msg);
    }
#endif
}
#endif// (BLE_GATT_BGCONN == TRUE)

/*******************************************************************************
**
** Function         bta_dm_discover_send_msg
**
** Description      This function send discover message to BTA task.
**
** Returns          void
**
*******************************************************************************/
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE
static void bta_dm_discover_send_msg(BD_ADDR bd_addr, tBTA_SERVICE_MASK_EXT *p_services,
                                     tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search,
                                     tBTA_TRANSPORT transport)
{
    tBTA_DM_API_DISCOVER    *p_msg;
    UINT16  len = p_services ? (sizeof(tBTA_DM_API_DISCOVER) +
                                sizeof(tBT_UUID) * p_services->num_uuid) :
                  sizeof(tBTA_DM_API_DISCOVER);

    if ((p_msg = (tBTA_DM_API_DISCOVER *) osi_malloc(len)) != NULL) {
        memset(p_msg, 0, len);

        p_msg->hdr.event = BTA_DM_API_DISCOVER_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->p_cback = p_cback;
        p_msg->sdp_search = sdp_search;
        p_msg->transport    = transport;

        if (p_services != NULL) {
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
            p_msg->services = p_services->srvc_mask;
            p_msg->num_uuid = p_services->num_uuid;
            if (p_services->num_uuid != 0) {
                p_msg->p_uuid = (tBT_UUID *)(p_msg + 1);
                memcpy(p_msg->p_uuid, p_services->p_uuid, sizeof(tBT_UUID) * p_services->num_uuid);
            }
#endif
        }

        bta_sys_sendmsg(p_msg);
    }
}
#endif
/*******************************************************************************
**
** Function         BTA_DmDiscoverByTransport
**
** Description      This function does service discovery on particular transport
**                  for services of a
**                  peer device. When services.num_uuid is 0, it indicates all
**                  GATT based services are to be searched; otherwise a list of
**                  UUID of interested services should be provided through
**                  p_services->p_uuid.
**
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmDiscoverByTransport(BD_ADDR bd_addr, tBTA_SERVICE_MASK_EXT *p_services,
                               tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search,
                               tBTA_TRANSPORT transport)
{
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE
    bta_dm_discover_send_msg(bd_addr, p_services, p_cback, sdp_search, transport);
#endif
}


/*******************************************************************************
**
** Function         BTA_DmDiscoverExt
**
** Description      This function does service discovery for services of a
**                  peer device. When services.num_uuid is 0, it indicates all
**                  GATT based services are to be searched; other wise a list of
**                  UUID of interested services should be provided through
**                  p_services->p_uuid.
**
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmDiscoverExt(BD_ADDR bd_addr, tBTA_SERVICE_MASK_EXT *p_services,
                       tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search)
{
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE
    bta_dm_discover_send_msg(bd_addr, p_services, p_cback, sdp_search, BTA_TRANSPORT_UNKNOWN);
#endif

}

/*******************************************************************************
**
** Function         BTA_DmSearchExt
**
** Description      This function searches for peer Bluetooth devices. It performs
**                  an inquiry and gets the remote name for devices. Service
**                  discovery is done if services is non zero
**
** Parameters       p_dm_inq: inquiry conditions
**                  p_services: if service is not empty, service discovery will be done.
**                            for all GATT based service condition, put num_uuid, and
**                            p_uuid is the pointer to the list of UUID values.
**                  p_cback: callback function when search is completed.
**
**
**
** Returns          void
**
*******************************************************************************/
void BTA_DmSearchExt(tBTA_DM_INQ *p_dm_inq, tBTA_SERVICE_MASK_EXT *p_services, tBTA_DM_SEARCH_CBACK *p_cback)
{
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
    tBTA_DM_API_SEARCH    *p_msg;
    UINT16  len = p_services ? (sizeof(tBTA_DM_API_SEARCH) + sizeof(tBT_UUID) * p_services->num_uuid) :
                  sizeof(tBTA_DM_API_SEARCH);

    if ((p_msg = (tBTA_DM_API_SEARCH *) osi_malloc(len)) != NULL) {
        memset(p_msg, 0, len);

        p_msg->hdr.event = BTA_DM_API_SEARCH_EVT;
        memcpy(&p_msg->inq_params, p_dm_inq, sizeof(tBTA_DM_INQ));
        p_msg->p_cback = p_cback;
        p_msg->rs_res  = BTA_DM_RS_NONE;


        if (p_services != NULL) {
            p_msg->services = p_services->srvc_mask;
            p_msg->num_uuid = p_services->num_uuid;

            if (p_services->num_uuid != 0) {
                p_msg->p_uuid = (tBT_UUID *)(p_msg + 1);
                memcpy(p_msg->p_uuid, p_services->p_uuid, sizeof(tBT_UUID) * p_services->num_uuid);
            } else {
                p_msg->p_uuid = NULL;
            }
        }

        bta_sys_sendmsg(p_msg);
    }
#else
    UNUSED(p_dm_inq);
    UNUSED(p_services);
    UNUSED(p_cback);
#endif
}
/*******************************************************************************
**
** Function         BTA_DmBleUpdateConnectionParam
**
** Description      Update connection parameters, can only be used when connection is up.
**
** Parameters:      bd_addr          - BD address of the peer
**                  min_int   -     minimum connection interval, [0x0004~ 0x4000]
**                  max_int   -     maximum connection interval, [0x0004~ 0x4000]
**                  latency   -     slave latency [0 ~ 500]
**                  timeout   -     supervision timeout [0x000a ~ 0xc80]
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleUpdateConnectionParam(BD_ADDR bd_addr, UINT16 min_int,
                                    UINT16 max_int, UINT16 latency,
                                    UINT16 timeout)
{
#if BLE_INCLUDED == TRUE
    tBTA_DM_API_UPDATE_CONN_PARAM *p_msg;

    p_msg = (tBTA_DM_API_UPDATE_CONN_PARAM *) osi_malloc(sizeof(tBTA_DM_API_UPDATE_CONN_PARAM));
    if (p_msg != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_UPDATE_CONN_PARAM));

        p_msg->hdr.event = BTA_DM_API_UPDATE_CONN_PARAM_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->min_int   = min_int;
        p_msg->max_int   = max_int;
        p_msg->latency   = latency;
        p_msg->timeout   = timeout;

        bta_sys_sendmsg(p_msg);
    }
#endif
}

#if BLE_INCLUDED == TRUE
/*******************************************************************************
**
** Function         BTA_DmBleConfigLocalPrivacy
**
** Description      Enable/disable privacy on the local device
**
** Parameters:      privacy_enable   - enable/disable privacy on remote device.
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleConfigLocalPrivacy(BOOLEAN privacy_enable, tBTA_SET_LOCAL_PRIVACY_CBACK *set_local_privacy_cback)
{
    ///This function used the irk to generate the resolve address
#if BLE_INCLUDED == TRUE && BLE_PRIVACY_SPT == TRUE
    tBTA_DM_API_LOCAL_PRIVACY *p_msg;

    if ((p_msg = (tBTA_DM_API_LOCAL_PRIVACY *) osi_malloc(sizeof(tBTA_DM_API_ENABLE_PRIVACY))) != NULL) {
        memset (p_msg, 0, sizeof(tBTA_DM_API_LOCAL_PRIVACY));

        p_msg->hdr.event = BTA_DM_API_LOCAL_PRIVACY_EVT;
        p_msg->privacy_enable   = privacy_enable;
        p_msg->set_local_privacy_cback = set_local_privacy_cback;
        bta_sys_sendmsg(p_msg);
    }
#else
    UNUSED (privacy_enable);
#endif
}

/*******************************************************************************
**
** Function         BTA_DmBleConfigLocalIcon
**
** Description      set gap local icon
**
** Parameters:      icon   - appearance value.
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleConfigLocalIcon(uint16_t icon)
{
    tBTA_DM_API_LOCAL_ICON *p_msg;

    if ((p_msg = (tBTA_DM_API_LOCAL_ICON *) osi_malloc(sizeof(tBTA_DM_API_LOCAL_ICON))) != NULL) {
        memset (p_msg, 0, sizeof(tBTA_DM_API_LOCAL_ICON));

        p_msg->hdr.event = BTA_DM_API_LOCAL_ICON_EVT;
        p_msg->icon   = icon;
        bta_sys_sendmsg(p_msg);
    }
}

#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
/*******************************************************************************
**
** Function         BTA_DmBleSetKeyMaterial
**
** Description      Set the Encrypted Data Key Material in GAP service
**
** Parameters:      session_key - 16-byte session key (must not be NULL)
**                  iv          - 8-byte initialization vector (must not be NULL)
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleSetKeyMaterial(const uint8_t *session_key, const uint8_t *iv)
{
    tBTA_DM_API_KEY_MATERIAL *p_msg;

    if (session_key == NULL || iv == NULL) {
        APPL_TRACE_ERROR("%s: NULL pointer parameter", __func__);
        return;
    }

    if ((p_msg = (tBTA_DM_API_KEY_MATERIAL *) osi_malloc(sizeof(tBTA_DM_API_KEY_MATERIAL))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_KEY_MATERIAL));

        p_msg->hdr.event = BTA_DM_API_KEY_MATERIAL_EVT;
        memcpy(p_msg->session_key, session_key, 16);
        memcpy(p_msg->iv, iv, 8);
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s: failed to allocate memory", __func__);
    }
}
#endif

/*******************************************************************************
**
** Function         BTA_DmBleUpdateConnectionParams
**
** Description      Update connection parameters, can only be used when connection is up.
**
** Parameters:      bd_addr   - BD address of the peer
**                  min_int   -     minimum connection interval, [0x0004~ 0x4000]
**                  max_int   -     maximum connection interval, [0x0004~ 0x4000]
**                  latency   -     slave latency [0 ~ 500]
**                  timeout   -     supervision timeout [0x000a ~ 0xc80]
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleUpdateConnectionParams(BD_ADDR bd_addr, UINT16 min_int, UINT16 max_int,
                                     UINT16 latency, UINT16 timeout)
{
    tBTA_DM_API_UPDATE_CONN_PARAM *p_msg;

    if ((p_msg = (tBTA_DM_API_UPDATE_CONN_PARAM *) osi_malloc(sizeof(tBTA_DM_API_UPDATE_CONN_PARAM))) != NULL) {
        memset (p_msg, 0, sizeof(tBTA_DM_API_UPDATE_CONN_PARAM));

        p_msg->hdr.event = BTA_DM_API_UPDATE_CONN_PARAM_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->min_int   = min_int;
        p_msg->max_int   = max_int;
        p_msg->latency   = latency;
        p_msg->timeout   = timeout;
        bta_sys_sendmsg(p_msg);
    }
}
/*******************************************************************************
**
** Function         BTA_DmBleDisconnect
**
** Description      Disconnect the ble connection, can only be used when connection is up.
**
** Parameters:      bd_addr   - BD address of the peer
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleDisconnect(BD_ADDR bd_addr)
{
    tBTA_DM_API_BLE_DISCONNECT *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_DISCONNECT *) osi_malloc(sizeof(tBTA_DM_API_BLE_DISCONNECT))) != NULL) {
        memset (p_msg, 0, sizeof(tBTA_DM_API_BLE_DISCONNECT));

        p_msg->hdr.event = BTA_DM_API_BLE_DISCONNECT_EVT;
        bdcpy(p_msg->remote_bda, bd_addr);

        bta_sys_sendmsg(p_msg);
    }
}
/*******************************************************************************
**
** Function         BTA_DmBleSetDataLength
**
** Description      This function is to set maximum LE data packet size
**
** Returns          void
**
**
*******************************************************************************/
void BTA_DmBleSetDataLength(BD_ADDR remote_device, UINT16 tx_data_length, tBTA_SET_PKT_DATA_LENGTH_CBACK *p_set_pkt_data_cback)
{
    tBTA_DM_API_BLE_SET_DATA_LENGTH *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_DATA_LENGTH *)osi_malloc(sizeof(tBTA_DM_API_BLE_SET_DATA_LENGTH)))
            != NULL) {
        bdcpy(p_msg->remote_bda, remote_device);
        p_msg->hdr.event = BTA_DM_API_SET_DATA_LENGTH_EVT;
        p_msg->tx_data_length = tx_data_length;
        p_msg->p_set_pkt_data_cback = p_set_pkt_data_cback;

        bta_sys_sendmsg(p_msg);
    }
}

#if (BLE_42_DTM_TEST_EN == TRUE)
void BTA_DmBleDtmTxStart(uint8_t tx_channel, uint8_t len_of_data, uint8_t pkt_payload, tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback)
{
    tBTA_DM_API_BLE_DTM_TX_START *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_DTM_TX_START *)osi_malloc(sizeof(tBTA_DM_API_BLE_DTM_TX_START)))
            != NULL) {
        p_msg->hdr.event = BTA_DM_API_DTM_TX_START_EVT;
        p_msg->tx_channel = tx_channel;
        p_msg->len_of_data = len_of_data;
        p_msg->pkt_payload = pkt_payload;
        p_msg->p_dtm_cmpl_cback = p_dtm_cmpl_cback;

        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleDtmRxStart(uint8_t rx_channel, tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback)
{
    tBTA_DM_API_BLE_DTM_RX_START *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_DTM_RX_START *)osi_malloc(sizeof(tBTA_DM_API_BLE_DTM_RX_START)))
            != NULL) {
        p_msg->hdr.event = BTA_DM_API_DTM_RX_START_EVT;
        p_msg->rx_channel= rx_channel;
        p_msg->p_dtm_cmpl_cback = p_dtm_cmpl_cback;

        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if (BLE_42_DTM_TEST_EN == TRUE)

#if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))
void BTA_DmBleDtmStop(tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback)
{
    tBTA_DM_API_BLE_DTM_STOP *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_DTM_STOP *)osi_malloc(sizeof(tBTA_DM_API_BLE_DTM_STOP)))
            != NULL) {
        p_msg->hdr.event = BTA_DM_API_DTM_STOP_EVT;
        p_msg->p_dtm_cmpl_cback = p_dtm_cmpl_cback;

        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))

void BTA_DmBleSetPrivacyMode(uint8_t addr_type, BD_ADDR addr, uint8_t privacy_mode, tBTA_SET_PRIVACY_MODE_CMPL_CBACK *p_cback)
{
    tBTA_DM_API_SET_PRIVACY_MODE *p_msg;

    if ((p_msg = (tBTA_DM_API_SET_PRIVACY_MODE *)osi_malloc(sizeof(tBTA_DM_API_SET_PRIVACY_MODE)))
            != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_PRIVACY_MODE_EVT;
        p_msg->addr_type = addr_type;
        memcpy(p_msg->addr, addr, sizeof(BD_ADDR));
        p_msg->privacy_mode = privacy_mode;
        p_msg->p_cback = p_cback;
        bta_sys_sendmsg(p_msg);
    }
}

#endif

/*******************************************************************************
**
** Function         BTA_DmSetEncryption
**
** Description      This function is called to ensure that connection is
**                  encrypted.  Should be called only on an open connection.
**                  Typically only needed for connections that first want to
**                  bring up unencrypted links, then later encrypt them.
**
** Parameters:      bd_addr       - Address of the peer device
**                  transport     - transport of the link to be encruypted
**                  p_callback    - Pointer to callback function to indicate the
**                                  link encryption status
**                  sec_act       - This is the security action to indicate
**                                  what kind of BLE security level is required for
**                                  the BLE link if the BLE is supported
**                                  Note: This parameter is ignored for the BR/EDR link
**                                        or the BLE is not supported
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void BTA_DmSetEncryption(BD_ADDR bd_addr, tBTA_TRANSPORT transport, tBTA_DM_ENCRYPT_CBACK *p_callback,
                         tBTA_DM_BLE_SEC_ACT sec_act)
{
    tBTA_DM_API_SET_ENCRYPTION   *p_msg;

    APPL_TRACE_API("BTA_DmSetEncryption"); //todo
    if ((p_msg = (tBTA_DM_API_SET_ENCRYPTION *) osi_malloc(sizeof(tBTA_DM_API_SET_ENCRYPTION))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_ENCRYPTION));

        p_msg->hdr.event = BTA_DM_API_SET_ENCRYPTION_EVT;

        memcpy(p_msg->bd_addr, bd_addr, BD_ADDR_LEN);
        p_msg->transport    = transport;
        p_msg->p_callback      = p_callback;
        p_msg->sec_act         = sec_act;

        bta_sys_sendmsg(p_msg);
    }
}
#endif  ///SMP_INCLUDED == TRUE

#if BLE_INCLUDED == TRUE

#if (BLE_42_SCAN_EN == TRUE)
/*******************************************************************************
**
** Function         BTA_DmBleScan
**
** Description      This procedure keep the device listening for advertising
**                  events from a broadcast device.
**
** Parameters       start: start or stop scan.
**
** Returns          void

**
** Returns          void.
**
*******************************************************************************/
extern void BTA_DmBleScan(BOOLEAN start, UINT32 duration,
                             tBTA_DM_SEARCH_CBACK *p_results_cb,
                             tBTA_START_STOP_SCAN_CMPL_CBACK *p_start_stop_scan_cb)
{
    tBTA_DM_API_BLE_SCAN   *p_msg;

    APPL_TRACE_API("BTA_DmBleScan:start = %d ", start);

    if ((p_msg = (tBTA_DM_API_BLE_SCAN *) osi_malloc(sizeof(tBTA_DM_API_BLE_SCAN))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_SCAN));

        p_msg->hdr.event = BTA_DM_API_BLE_SCAN_EVT;
        p_msg->start = start;
        p_msg->duration = duration;
        p_msg->p_cback = p_results_cb;
        if (start){
            p_msg->p_start_scan_cback = p_start_stop_scan_cb;
        }
        else {
            p_msg->p_stop_scan_cback = p_start_stop_scan_cb;
        }

        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if (BLE_42_SCAN_EN == TRUE)

/*******************************************************************************
**
** Function         BTA_DmSetRandAddress
**
** Description      This function set the random address for the APP
**
** Parameters       rand_addr: the random address with should be setting
**                  p_set_rand_addr_cback: complete callback
** Returns          void
**
**
*******************************************************************************/
extern void BTA_DmSetRandAddress(BD_ADDR rand_addr, tBTA_SET_RAND_ADDR_CBACK *p_set_rand_addr_cback)
{
    tBTA_DM_APT_SET_DEV_ADDR *p_msg;
    APPL_TRACE_API("set the random address ");
    if ((p_msg = (tBTA_DM_APT_SET_DEV_ADDR *) osi_malloc(sizeof(tBTA_DM_APT_SET_DEV_ADDR))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_APT_SET_DEV_ADDR));
        memcpy(p_msg->address, rand_addr, BD_ADDR_LEN);
        p_msg->hdr.event = BTA_DM_API_SET_RAND_ADDR_EVT;
        p_msg->addr_type = BLE_ADDR_RANDOM;
        p_msg->p_set_rand_addr_cback = p_set_rand_addr_cback;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    }
}
/*******************************************************************************
**
** Function         BTA_DmBleSetRpaTimeout
**
** Description      This function sets the Resolvable Private Address (RPA) timeout
**                  for the Bluetooth device. The RPA timeout defines how long an RPA
**                  remains in use before a new one is generated.
**
** Parameters       rpa_timeout: The timeout in seconds within the range of 1s to 1 hour
**                               as defined by the Bluetooth specification. This duration
**                               specifies how long the controller uses an RPA before
**                               generating a new one.
** Returns          void
**
**
*******************************************************************************/
void BTA_DmBleSetRpaTimeout(uint16_t rpa_timeout,tBTA_SET_RPA_TIMEOUT_CMPL_CBACK *p_set_rpa_timeout_cback)
{
    tBTA_DM_API_SET_RPA_TIMEOUT *p_msg;
    if ((p_msg = (tBTA_DM_API_SET_RPA_TIMEOUT *) osi_malloc(sizeof(tBTA_DM_API_SET_RPA_TIMEOUT))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_RPA_TIMEOUT));
        p_msg->hdr.event = BTA_DM_API_SET_RPA_TIMEOUT_EVT;
        p_msg->rpa_timeout = rpa_timeout; // Assign the RPA timeout value to the message
        p_msg->p_set_rpa_timeout_cback = p_set_rpa_timeout_cback;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         BTA_DmBleAddDevToResolvingList
**
** Description      This function adds a device to the resolving list of the
**                  Bluetooth controller. The resolving list is used for resolving
**                  the identity of devices using resolvable private addresses (RPAs).
**
** Parameters       addr: Bluetooth device address to be added to the resolving list
**                  addr_type: Type of the address (public or random)
**                  irk: Identity Resolving Key (IRK) of the device
**                  add_dev_to_resolving_list_callback: Callback function to be invoked
**                                                     upon completion of the operation
**
** Returns          void
**
*******************************************************************************/
void BTA_DmBleAddDevToResolvingList(BD_ADDR addr,
                                    uint8_t addr_type,
                                    PEER_IRK irk,
                                    tBTA_ADD_DEV_TO_RESOLVING_LIST_CMPL_CBACK *add_dev_to_resolving_list_callback)
{
    tBTA_DM_API_ADD_DEV_TO_RESOLVING_LIST *p_msg;
    if ((p_msg = (tBTA_DM_API_ADD_DEV_TO_RESOLVING_LIST *) osi_malloc(sizeof(tBTA_DM_API_ADD_DEV_TO_RESOLVING_LIST))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_ADD_DEV_TO_RESOLVING_LIST));
        p_msg->hdr.event = BTA_DM_API_ADD_DEV_TO_RESOLVING_LIST_EVT;
        memcpy(p_msg->addr, addr, BD_ADDR_LEN); // Copy the device address to the message
        p_msg->addr_type = addr_type;                      // Assign the address type to the message
        memcpy(p_msg->irk, irk, PEER_IRK_LEN);            // Copy the IRK to the message
        p_msg->p_add_dev_to_resolving_list_callback = add_dev_to_resolving_list_callback;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmClearRandAddress(void)
{
    tBTA_DM_APT_CLEAR_ADDR *p_msg;
    if ((p_msg = (tBTA_DM_APT_CLEAR_ADDR *) osi_malloc(sizeof(tBTA_DM_APT_CLEAR_ADDR))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_APT_CLEAR_ADDR));
        p_msg->hdr.event = BTA_DM_API_CLEAR_RAND_ADDR_EVT;
        bta_sys_sendmsg(p_msg);
    }
}

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
void BTA_DmBleGapEnhReadTransPwrLevel(uint16_t conn_handle, uint8_t phy)
{
    tBTA_DM_API_BLE_ENH_READ_TANS_PWR_LEVEL *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_ENH_READ_TANS_PWR_LEVEL *)osi_malloc(sizeof(tBTA_DM_API_BLE_ENH_READ_TANS_PWR_LEVEL)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_ENH_READ_TRANS_POWER_LEVEL;
        p_msg->conn_handle = conn_handle;
        p_msg->phy = phy;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapReadRemoteTransPwrLevel(uint16_t conn_handle, uint8_t phy)
{
    tBTA_DM_API_BLE_READ_REMOTE_TANS_PWR_LEVEL *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_READ_REMOTE_TANS_PWR_LEVEL *)osi_malloc(sizeof(tBTA_DM_API_BLE_READ_REMOTE_TANS_PWR_LEVEL)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_READ_REM_TRANS_POWER_LEVEL;
        p_msg->conn_handle = conn_handle;
        p_msg->phy = phy;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapSetPathLossRptParams(uint16_t conn_handle, uint8_t high_threshold, uint8_t high_hysteresis,
                                        uint8_t low_threshold, uint8_t low_hysteresis, uint16_t min_time_spent)
{
    tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_PARAMS)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_PATH_LOSS_REPORT_PARAMS;
        p_msg->conn_handle = conn_handle;
        p_msg->high_threshold = high_threshold;
        p_msg->high_hysteresis = high_hysteresis;
        p_msg->low_threshold = low_threshold;
        p_msg->low_hysteresis = low_hysteresis;
        p_msg->min_time_spent = min_time_spent;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapSetPathLossRptEnable(uint16_t conn_handle, uint8_t enable)
{
    tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_ENABLE *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_ENABLE *)osi_malloc(sizeof(tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_ENABLE)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_PATH_LOSS_REPORTING_EN;
        p_msg->conn_handle = conn_handle;
        p_msg->enable = enable;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapSetTransPwrRptEnable(uint16_t conn_handle, uint8_t local_enable, uint8_t remote_enable)
{
    tBTA_DM_API_BLE_SET_TRANS_PWR_RPT_ENABLE *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_TRANS_PWR_RPT_ENABLE *)osi_malloc(sizeof(tBTA_DM_API_BLE_SET_TRANS_PWR_RPT_ENABLE)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_TRANS_POWER_REPORTING_EN;
        p_msg->conn_handle = conn_handle;
        p_msg->local_enable = local_enable;
        p_msg->remote_enable = remote_enable;
        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)

#if (BLE_FEAT_CONN_SUBRATING == TRUE)
void BTA_DmBleGapSetDefaultSubrate(uint16_t subrate_min, uint16_t subrate_max, uint16_t max_latency, uint16_t continuation_number, uint16_t supervision_timeout)
{
    tBTA_DM_API_BLE_SET_DEFAULT_SUBRATE *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SET_DEFAULT_SUBRATE *)osi_malloc(sizeof(tBTA_DM_API_BLE_SET_DEFAULT_SUBRATE)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_DEFALT_SUBRATE;
        p_msg->subrate_min = subrate_min;
        p_msg->subrate_max = subrate_max;
        p_msg->max_latency = max_latency;
        p_msg->continuation_number = continuation_number;
        p_msg->supervision_timeout = supervision_timeout;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapSubrateReqest(uint16_t conn_handle, uint16_t subrate_min, uint16_t subrate_max, uint16_t max_latency, uint16_t continuation_number, uint16_t supervision_timeout)
{
    tBTA_DM_API_BLE_SUBRATE_REQUEST *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_SUBRATE_REQUEST *)osi_malloc(sizeof(tBTA_DM_API_BLE_SUBRATE_REQUEST)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_SUBRATE_REQUEST;
        p_msg->conn_handle = conn_handle;
        p_msg->subrate_min = subrate_min;
        p_msg->subrate_max = subrate_max;
        p_msg->max_latency = max_latency;
        p_msg->continuation_number = continuation_number;
        p_msg->supervision_timeout = supervision_timeout;
        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
void BTA_DmBleGapSetHostFeature(uint16_t bit_num, uint8_t bit_val)
{
    tBTA_DM_API_SET_HOST_FEATURE *p_msg;
    APPL_TRACE_API("%s", __func__);
    if ((p_msg = (tBTA_DM_API_SET_HOST_FEATURE *) osi_malloc(sizeof(tBTA_DM_API_SET_HOST_FEATURE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_HOST_FEATURE));
        p_msg->hdr.event = BTA_DM_API_SET_HOST_FEATURE_EVT;

        p_msg->bit_num = bit_num;
        p_msg->bit_val = bit_val;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BT_BLE_FEAT_PAWR_EN == TRUE)
void BTA_DmBleGapSetPASubevtData(uint8_t adv_handle, uint8_t num_subevents_with_data, uint8_t *subevent_params)
{
    tBTA_DM_API_BLE_PA_SUBEVENT_DATA *p_msg;
    tBTA_BLE_SUBEVENT_PARAMS *p_subevent_params = (tBTA_BLE_SUBEVENT_PARAMS*)subevent_params;

    if ((p_msg = (tBTA_DM_API_BLE_PA_SUBEVENT_DATA *)osi_malloc(sizeof(tBTA_DM_API_BLE_PA_SUBEVENT_DATA) + num_subevents_with_data * sizeof(tBTA_DM_API_BLE_SUBEVENT_PARAMS)))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_PA_SUBEVT_DATA;
        p_msg->adv_handle = adv_handle;
        p_msg->num_subevents_with_data = num_subevents_with_data;
        p_msg->subevent_params = (tBTA_DM_API_BLE_SUBEVENT_PARAMS *)(p_msg + 1);
        for (uint8_t i = 0; i < num_subevents_with_data; i++)
        {
            p_msg->subevent_params[i].subevent = p_subevent_params[i].subevent;
            p_msg->subevent_params[i].response_slot_start = p_subevent_params[i].response_slot_start;
            p_msg->subevent_params[i].response_slot_count = p_subevent_params[i].response_slot_count;
            p_msg->subevent_params[i].subevent_data_len = p_subevent_params[i].subevent_data_len;
            memcpy(&(p_msg->subevent_params[i].subevent_data), p_subevent_params[i].subevent_data, p_subevent_params[i].subevent_data_len);
        }

        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapSetPeriodicAdvRspData(uint16_t sync_handle, uint16_t request_event, uint8_t request_subevent,
                                        uint8_t rsp_subevent, uint8_t rsp_slot, uint8_t rsp_data_len, uint8_t *rsp_data)
{
    tBTA_DM_API_BLE_PA_RSP_DATA *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_PA_RSP_DATA *)osi_malloc(sizeof(tBTA_DM_API_BLE_PA_RSP_DATA) + rsp_data_len))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_PA_RSP_DATA;
        p_msg->sync_handle = sync_handle;
        p_msg->request_event = request_event;
        p_msg->request_subevent = request_subevent;
        p_msg->rsp_subevent = rsp_subevent;
        p_msg->rsp_slot = rsp_slot;
        p_msg->rsp_data_len = rsp_data_len;
        p_msg->rsp_data = (UINT8 *)(p_msg + 1);
        if (rsp_data_len && rsp_data) {
            memcpy(p_msg->rsp_data, rsp_data, rsp_data_len);
        } else if (rsp_data_len) {
            APPL_TRACE_ERROR("%s rsp_data is NULL", __func__);
        }
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapSetPeriodicSyncSubevt(uint16_t sync_handle, uint16_t periodic_adv_properties, uint8_t num_subevents_to_sync, uint8_t *subevent)
{
    tBTA_DM_API_BLE_PA_SYNC_SUBEVT *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_PA_SYNC_SUBEVT *)osi_malloc(sizeof(tBTA_DM_API_BLE_PA_SYNC_SUBEVT) + num_subevents_to_sync))
        != NULL) {
        p_msg->hdr.event = BTA_DM_API_SET_PA_SYNC_SUBEVT;
        p_msg->sync_handle = sync_handle;
        p_msg->periodic_adv_properties = periodic_adv_properties;
        p_msg->num_subevents_to_sync = num_subevents_to_sync;
        p_msg->subevent = (UINT8 *)(p_msg + 1);
        if (num_subevents_to_sync && subevent) {
            memcpy(p_msg->subevent, subevent, num_subevents_to_sync);
        } else if (num_subevents_to_sync) {
            APPL_TRACE_ERROR("%s subevent is NULL", __func__);
        }
        bta_sys_sendmsg(p_msg);
    }
}

#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)

#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
void BTA_DmBleGapReadLocalSupportedCaps(void)
{
    tBTA_DM_API_CS_READ_LOCAL_SUPP_CAPS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_READ_LOCAL_SUPP_CAPS *)osi_malloc(sizeof(tBTA_DM_API_CS_READ_LOCAL_SUPP_CAPS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_READ_LOCAL_SUPPORTED_CAPS;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapReadRemoteSupportedCaps(uint16_t conn_handle)
{
    tBTA_DM_API_CS_READ_REMOTE_SUPP_CAPS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_READ_REMOTE_SUPP_CAPS *)osi_malloc(sizeof(tBTA_DM_API_CS_READ_REMOTE_SUPP_CAPS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_READ_REMOTE_SUPPORTED_CAPS;
        p_msg->conn_handle = conn_handle;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapWriteCachedRemoteSupportedCaps(tBTA_DM_CS_WRITE_CACHED_REMOTE_SUPP_CAPS *write_cachedremote_supp_caps)
{
    tBTA_DM_API_CS_WRITE_CACHED_REMOTE_SUPP_CAPS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_WRITE_CACHED_REMOTE_SUPP_CAPS *)osi_malloc(sizeof(tBTA_DM_API_CS_WRITE_CACHED_REMOTE_SUPP_CAPS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPS;
        memcpy(&p_msg->conn_handle, &write_cachedremote_supp_caps->conn_handle, sizeof(tBTA_DM_CS_WRITE_CACHED_REMOTE_SUPP_CAPS));
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapCsSecurityEnable(uint16_t conn_handle)
{
    tBTA_DM_API_CS_SECURITY_ENABLE *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_SECURITY_ENABLE *)osi_malloc(sizeof(tBTA_DM_API_CS_SECURITY_ENABLE))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_SECURITY_ENABLE;
        p_msg->conn_handle = conn_handle;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapCsSetDefaultSetting(uint16_t conn_handle, uint8_t role_enable, uint8_t cs_sync_ant_selection, int8_t max_tx_power)
{
    tBTA_DM_API_CS_SET_DEFAULT_SETTING_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_SET_DEFAULT_SETTING_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_CS_SET_DEFAULT_SETTING_PARAMS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_SET_DEFAULT_SETTINGS;
        p_msg->conn_handle = conn_handle;
        p_msg->role_enable = role_enable;
        p_msg->cs_sync_ant_selection = cs_sync_ant_selection;
        p_msg->max_tx_power = max_tx_power;
        bta_sys_sendmsg(p_msg);
    }
}
void BTA_DmBleGapCsReadRemoteFaeTable(uint16_t conn_handle)
{
    tBTA_DM_API_CS_READ_REMOTE_TAB *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_READ_REMOTE_TAB *)osi_malloc(sizeof(tBTA_DM_API_CS_READ_REMOTE_TAB))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_READ_REMOTE_FAE_TABLE;
        p_msg->conn_handle = conn_handle;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapWriteCachedRemoteFaeTable(uint16_t conn_handle, uint8_t *remote_fae_table, uint8_t table_len)
{
    tBTA_DM_API_CS_WRITE_CACHED_REMOTE_FAE_TAB_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_WRITE_CACHED_REMOTE_FAE_TAB_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_CS_WRITE_CACHED_REMOTE_FAE_TAB_PARAMS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_WRITE_CACHED_REMOTE_FAE_TABLE;
        p_msg->conn_handle = conn_handle;
        memcpy(&p_msg->remote_fae_table[0], remote_fae_table, table_len);
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapCsCreateConfig(tBTA_DM_CS_CREATE_CONFIG_PARAMS *create_config_params)
{
    tBTA_DM_API_CS_CREATE_CONFIG_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_CREATE_CONFIG_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_CS_CREATE_CONFIG_PARAMS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_CREATE_CONFIG;
        memcpy(&p_msg->conn_handle, &create_config_params->conn_handle, sizeof(tBTA_DM_CS_CREATE_CONFIG_PARAMS));
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapCsRemoveConfig(uint16_t conn_handle, uint8_t config_id)
{
    tBTA_DM_API_CS_REMOVE_CONFIG_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_REMOVE_CONFIG_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_CS_REMOVE_CONFIG_PARAMS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_REMOVE_CONFIG;
        p_msg->conn_handle = conn_handle;
        p_msg->config_id = config_id;
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapCsSetChannelClass(uint8_t *channel_class, uint8_t channl_len)
{
    tBTA_DM_API_CS_SET_CHANNEL_CLASS_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_SET_CHANNEL_CLASS_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_CS_SET_CHANNEL_CLASS_PARAMS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_SET_CAHNNEL_CLASSIFICATION;
        memcpy(&p_msg->channel_class[0], channel_class,  channl_len);
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapCsSetProcPatams(tBTA_DM_CS_SET_PROC_PARAMS *set_proc_params)
{
    tBTA_DM_API_CS_SET_PROC_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_SET_PROC_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_CS_SET_PROC_PARAMS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_SET_PROCEDURE_PARAMS;
        memcpy(&p_msg->conn_handle, &set_proc_params->conn_handle, sizeof(tBTA_DM_CS_SET_PROC_PARAMS));
        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleGapCsProcEnable(uint16_t conn_handle, uint8_t config_id, uint8_t enable)
{
    tBTA_DM_API_CS_PROC_ENABLE_PARAMS *p_msg;

    if ((p_msg = (tBTA_DM_API_CS_PROC_ENABLE_PARAMS *)osi_malloc(sizeof(tBTA_DM_API_CS_PROC_ENABLE_PARAMS))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_CS_PROCEDURE_ENABLE;
        p_msg->conn_handle = conn_handle;
        p_msg->config_id = config_id;
        p_msg->enable = enable;
        bta_sys_sendmsg(p_msg);
    }
}

#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)

/*******************************************************************************
**
** Function         BTA_VendorInit
**
** Description      This function initializes vendor specific
**
** Returns          void
**
*******************************************************************************/
void BTA_VendorInit (void)
{
    APPL_TRACE_API("BTA_VendorInit");
}

#if (BLE_50_FEATURE_SUPPORT == TRUE)
void BTA_DmBleGapReadPHY(BD_ADDR addr)
{
    tBTA_DM_API_READ_PHY *p_msg;
    APPL_TRACE_API("%s, read phy.", __func__);
    if ((p_msg = (tBTA_DM_API_READ_PHY *) osi_malloc(sizeof(tBTA_DM_API_READ_PHY))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_READ_PHY));
        p_msg->hdr.event = BTA_DM_API_READ_PHY_EVT;
        memcpy(p_msg->bd_addr, addr, BD_ADDR_LEN);
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapSetPreferedDefaultPHY(tBTA_DM_BLE_GAP_PHY_MASK tx_phy_mask,
                                                          tBTA_DM_BLE_GAP_PHY_MASK rx_phy_mask)
{
    tBTA_DM_API_SET_PER_DEF_PHY *p_msg;
    APPL_TRACE_API("%s, Set preferred default phy.", __func__);
    if ((p_msg = (tBTA_DM_API_SET_PER_DEF_PHY *) osi_malloc(sizeof(tBTA_DM_API_SET_PER_DEF_PHY))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_PER_DEF_PHY));
        p_msg->hdr.event = BTA_DM_API_SET_PER_DEF_PHY_EVT;
        p_msg->tx_phy_mask = tx_phy_mask;
        p_msg->rx_phy_mask = rx_phy_mask;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapSetPreferedPHY(BD_ADDR addr,
                                               UINT8 all_phys,
                                               tBTA_DM_BLE_GAP_PHY_MASK tx_phy_mask,
                                               tBTA_DM_BLE_GAP_PHY_MASK rx_phy_mask,
                                               UINT16 phy_options)
{
    tBTA_DM_API_SET_PER_PHY *p_msg;
    APPL_TRACE_API("%s, Set preferred phy.", __func__);
    if ((p_msg = (tBTA_DM_API_SET_PER_PHY *) osi_malloc(sizeof(tBTA_DM_API_SET_PER_PHY))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_PER_PHY));
        p_msg->hdr.event = BTA_DM_API_SET_PER_PHY_EVT;
        memcpy(p_msg->bd_addr, addr, BD_ADDR_LEN);
        p_msg->all_phys = all_phys;
        p_msg->tx_phy_mask = tx_phy_mask;
        p_msg->rx_phy_mask = rx_phy_mask;
        p_msg->phy_options = phy_options;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}
#if (BLE_50_EXTEND_ADV_EN == TRUE)
void BTA_DmBleGapExtAdvSetRandaddr(UINT16 instance, BD_ADDR addr)
{
    tBTA_DM_API_EXT_ADV_SET_RAND_ADDR *p_msg;
    APPL_TRACE_API("%s, Set extended ADV parameters.", __func__);
    if ((p_msg = (tBTA_DM_API_EXT_ADV_SET_RAND_ADDR *) osi_malloc(sizeof(tBTA_DM_API_EXT_ADV_SET_RAND_ADDR))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_EXT_ADV_SET_RAND_ADDR));
        p_msg->hdr.event = BTA_DM_API_SET_EXT_ADV_RAND_ADDR_EVT;
        p_msg->instance = instance;
        memcpy(&p_msg->rand_addr, addr, BD_ADDR_LEN);
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapExtAdvSetParams(UINT16 instance,
                                                 const tBTA_DM_BLE_GAP_EXT_ADV_PARAMS *params)
{
    tBTA_DM_API_EXT_ADV_SET_PARAMS *p_msg;
    APPL_TRACE_API("%s, Set extended ADV parameters.", __func__);
    if ((p_msg = (tBTA_DM_API_EXT_ADV_SET_PARAMS *) osi_malloc(sizeof(tBTA_DM_API_EXT_ADV_SET_PARAMS))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_EXT_ADV_SET_PARAMS));
        p_msg->hdr.event = BTA_DM_API_SET_EXT_ADV_PARAMS_EVT;
        p_msg->instance = instance;
        memcpy(&p_msg->params, params, sizeof(tBTA_DM_BLE_GAP_EXT_ADV_PARAMS));
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapConfigExtAdvDataRaw(BOOLEAN is_scan_rsp, UINT8 instance, UINT16 length,
                                                        const UINT8 *data)
{
    tBTA_DM_API_CFG_EXT_ADV_DATA *p_msg;
    APPL_TRACE_API("%s, Config extended %s data.", __func__, is_scan_rsp ? "Scan rsp" : "Adv");
    if ((p_msg = (tBTA_DM_API_CFG_EXT_ADV_DATA *) osi_malloc(sizeof(tBTA_DM_API_CFG_EXT_ADV_DATA) + length)) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_CFG_EXT_ADV_DATA) + length);
        p_msg->hdr.event = BTA_DM_API_CFG_ADV_DATA_RAW_EVT;
        p_msg->is_scan_rsp = is_scan_rsp;
        p_msg->instance = instance;
        p_msg->length = length;
        p_msg->data = length != 0 ? (UINT8 *)(p_msg + 1) : NULL;
        if (data) {
            memcpy(p_msg->data, data, length);
        }
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapExtAdvEnable(BOOLEAN enable, UINT8 num, tBTA_DM_BLE_EXT_ADV *ext_adv)
{
    tBTA_DM_API_BLE_EXT_ADV *p_msg;
    APPL_TRACE_API("%s, Start extended ADV", __func__);
    if ((p_msg = (tBTA_DM_API_BLE_EXT_ADV *) osi_malloc(sizeof(tBTA_DM_API_BLE_EXT_ADV) + sizeof(tBTA_DM_BLE_EXT_ADV)*num)) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_EXT_ADV) + sizeof(tBTA_DM_BLE_EXT_ADV)*num);
        p_msg->hdr.event = BTA_DM_API_EXT_ADV_ENABLE_EVT;
        p_msg->enable = enable;
        p_msg->num = num;
        p_msg->ext_adv = (tBTA_DM_BLE_EXT_ADV *)(p_msg + 1);
        if (ext_adv) {
            memcpy(p_msg->ext_adv, ext_adv, sizeof(tBTA_DM_BLE_EXT_ADV)*num);
        }
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapExtAdvSetRemove(UINT8 instance)
{
    tBTA_DM_API_BLE_EXT_ADV_SET_REMOVE *p_msg;
    APPL_TRACE_API("%s, Remove extended ADV", __func__);
    if ((p_msg = (tBTA_DM_API_BLE_EXT_ADV_SET_REMOVE *) osi_malloc(sizeof(tBTA_DM_API_BLE_EXT_ADV_SET_REMOVE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_EXT_ADV_SET_REMOVE));
        p_msg->hdr.event = BTA_DM_API_EXT_ADV_SET_REMOVE_EVT;
        p_msg->instance = instance;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapExtAdvSetClear(void)
{
    tBTA_DM_API_BLE_EXT_ADV_SET_CLEAR *p_msg;
    APPL_TRACE_API("%s, Clear extended ADV", __func__);
    if ((p_msg = (tBTA_DM_API_BLE_EXT_ADV_SET_CLEAR *) osi_malloc(sizeof(tBTA_DM_API_BLE_EXT_ADV_SET_CLEAR))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_EXT_ADV_SET_CLEAR));
        p_msg->hdr.event = BTA_DM_API_EXT_ADV_SET_CLEAR_EVT;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

#if (BLE_50_PERIODIC_ADV_EN == TRUE)
void BTA_DmBleGapPeriodicAdvSetParams(UINT8 instance,
                                                         tBTA_DM_BLE_Periodic_Adv_Params *params)
{
    tBTA_DM_API_BLE_PERIODIC_ADV_SET_PARAMS *p_msg;
    APPL_TRACE_API("%s, Periodic ADV set parameters.", __func__);
    if ((p_msg = (tBTA_DM_API_BLE_PERIODIC_ADV_SET_PARAMS *) osi_malloc(sizeof(tBTA_DM_API_BLE_PERIODIC_ADV_SET_PARAMS))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BLE_PERIODIC_ADV_SET_PARAMS));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_SET_PARAMS_EVT;
        p_msg->instance = instance;
        memcpy(&p_msg->params, params, sizeof(tBTA_DM_BLE_Periodic_Adv_Params));
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapPeriodicAdvCfgDataRaw(UINT8 instance, UINT16 length,
                                                           const UINT8 *data,bool only_update_did)
{
    tBTA_DM_API_CFG_PERIODIC_ADV_DATA *p_msg;
    APPL_TRACE_API("%s, Periodic ADV config data raw.", __func__);
    if ((p_msg = (tBTA_DM_API_CFG_PERIODIC_ADV_DATA *) osi_malloc(sizeof(tBTA_DM_API_CFG_PERIODIC_ADV_DATA) + length)) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_CFG_PERIODIC_ADV_DATA) + length);
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_CFG_DATA_EVT;
        p_msg->instance = instance;
        p_msg->length = length;
        p_msg->data = (UINT8 *)(p_msg + 1);
        memcpy(p_msg->data, data, length);
        p_msg->data = length != 0 ? (UINT8 *)(p_msg + 1) : NULL;
        p_msg->only_update_did = only_update_did;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapPeriodicAdvEnable(UINT8 enable, UINT8 instance)
{
    tBTA_DM_API_ENABLE_PERIODIC_ADV *p_msg;
    APPL_TRACE_API("%s, Periodic ADV %s.", __func__, enable ? "start" : "stop");
    if ((p_msg = (tBTA_DM_API_ENABLE_PERIODIC_ADV *) osi_malloc(sizeof(tBTA_DM_API_ENABLE_PERIODIC_ADV))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_ENABLE_PERIODIC_ADV));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_ENABLE_EVT;
        p_msg->instance = instance;
        p_msg->enable = enable;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}
#endif // #if (BLE_50_PERIODIC_ADV_EN == TRUE)

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
void BTA_DmBleGapPeriodicAdvCreateSync(tBTA_DM_BLE_Periodic_Sync_Params *params)
{
    tBTA_DM_API_PERIODIC_ADV_SYNC *p_msg;
    APPL_TRACE_API("%s, Periodic ADV create sync.", __func__);
    if ((p_msg = (tBTA_DM_API_PERIODIC_ADV_SYNC *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_SYNC))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_SYNC));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_SYNC_EVT;
        memcpy(&p_msg->params, params, sizeof(tBTA_DM_BLE_Periodic_Sync_Params));
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapPeriodicAdvSyncCancel(void)
{
    tBTA_DM_API_PERIODIC_ADV_SYNC_CANCEL *p_msg;
    APPL_TRACE_API("%s, Periodic ADV sync cancel.", __func__);
    if ((p_msg = (tBTA_DM_API_PERIODIC_ADV_SYNC_CANCEL *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_SYNC_CANCEL))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_SYNC_CANCEL));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_SYNC_CANCEL_EVT;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapPeriodicAdvSyncTerm(UINT16 sync_handle)
{
    tBTA_DM_API_PERIODIC_ADV_SYNC_TERM *p_msg;
    APPL_TRACE_API("%s, Periodic ADV sync terminat.", __func__);
    if ((p_msg = (tBTA_DM_API_PERIODIC_ADV_SYNC_TERM *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_SYNC_TERM))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_SYNC_TERM));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_SYNC_TERMINATE_EVT;
        p_msg->sync_handle = sync_handle;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapPeriodicAdvAddDevToList(tBLE_ADDR_TYPE addr_type,
                                                              BD_ADDR addr,
                                                              UINT16 sid)
{
    tBTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LIST *p_msg;
    APPL_TRACE_API("%s, Periodic ADV add device to list.", __func__);
    if ((p_msg = (tBTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LIST *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LIST))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LIST));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LSIT_EVT;
        p_msg->addr_type = addr_type;
        p_msg->sid = sid;
        memcpy(p_msg->addr, addr, sizeof(BD_ADDR));
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapPeriodicAdvRemoveDevFromList(tBLE_ADDR_TYPE addr_type,
                                                              BD_ADDR addr,
                                                              UINT16 sid)
{
    tBTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LIST *p_msg;
    APPL_TRACE_API("%s, Periodic ADV remove device from list.", __func__);
    if ((p_msg = (tBTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LIST *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LIST))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LIST));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LSIT_EVT;
        p_msg->addr_type = addr_type;
        p_msg->sid = sid;
        memcpy(p_msg->addr, addr, sizeof(BD_ADDR));
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapPeriodicAdvClearDev(void)
{
    tBTA_DM_API_PERIODIC_ADV_DEV_CLEAR *p_msg;
    APPL_TRACE_API("%s, Periodic ADV clear device from list.", __func__);
    if ((p_msg = (tBTA_DM_API_PERIODIC_ADV_DEV_CLEAR *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_DEV_CLEAR))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_DEV_CLEAR));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_CLEAR_DEV_EVT;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)

#if (BLE_50_EXTEND_SCAN_EN == TRUE)
void BTA_DmBleGapSetExtScanParams(tBTA_DM_BLE_EXT_SCAN_PARAMS *params)
{
    tBTA_DM_API_SET_EXT_SCAN_PARAMS *p_msg;
    APPL_TRACE_API("%s, Set extended scan parameters.", __func__);
    if ((p_msg = (tBTA_DM_API_SET_EXT_SCAN_PARAMS *) osi_malloc(sizeof(tBTA_DM_API_SET_EXT_SCAN_PARAMS))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_EXT_SCAN_PARAMS));
        p_msg->hdr.event = BTA_DM_API_SET_EXT_SCAN_PARAMS_EVT;
        memcpy(&p_msg->params, params, sizeof(tBTA_DM_BLE_EXT_SCAN_PARAMS));
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapExtScan(BOOLEAN start, UINT32 duration, UINT16 period)
{
    tBTA_DM_API_EXT_SCAN *p_msg;
    APPL_TRACE_API("%s, %s extended scan.", __func__, start ? "Start" : "Stop");
    if ((p_msg = (tBTA_DM_API_EXT_SCAN *) osi_malloc(sizeof(tBTA_DM_API_EXT_SCAN))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_EXT_SCAN));
        p_msg->hdr.event = BTA_DM_API_START_EXT_SCAN_EVT;
        p_msg->start = start;
        p_msg->duration = duration;
        p_msg->period = period;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)

void BTA_DmBleGapPreferExtConnectParamsSet(BD_ADDR bd_addr,
                                                                 UINT8 phy_mask,
                                                                 const tBTA_DM_BLE_CONN_PARAMS *phy_1m_conn_params,
                                                                 const tBTA_DM_BLE_CONN_PARAMS *phy_2m_conn_params,
                                                                 const tBTA_DM_BLE_CONN_PARAMS *phy_coded_conn_params)
{
    tBTA_DM_API_SET_PER_EXT_CONN_PARAMS *p_msg;
    APPL_TRACE_API("%s, Set prefer extended connection parameters.", __func__);
    if ((p_msg = (tBTA_DM_API_SET_PER_EXT_CONN_PARAMS *) osi_malloc(sizeof(tBTA_DM_API_SET_PER_EXT_CONN_PARAMS))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_PER_EXT_CONN_PARAMS));
        p_msg->hdr.event = BTA_DM_API_SET_PERF_EXT_CONN_PARAMS_EVT;
        p_msg->phy_mask = phy_mask;

        memcpy(p_msg->bd_addr, bd_addr, sizeof(BD_ADDR));

        if (phy_1m_conn_params) {
            memcpy(&p_msg->phy_1m_conn_params, phy_1m_conn_params, sizeof(tBTA_DM_BLE_CONN_PARAMS));
        }

        if (phy_2m_conn_params) {
            memcpy(&p_msg->phy_2m_conn_params, phy_2m_conn_params, sizeof(tBTA_DM_BLE_CONN_PARAMS));
        }

        if (phy_coded_conn_params) {
            memcpy(&p_msg->phy_coded_conn_params, phy_coded_conn_params, sizeof(tBTA_DM_BLE_CONN_PARAMS));
        }
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }


}

void BTA_DmBleGapExtConnect(tBLE_ADDR_TYPE own_addr_type, const BD_ADDR peer_addr)
{
    tBTA_DM_API_EXT_CONN *p_msg;
    APPL_TRACE_API("%s, Start Extended connect.", __func__);
    APPL_TRACE_API("%s, Set prefer extended connection parameters.", __func__);
    if ((p_msg = (tBTA_DM_API_EXT_CONN *) osi_malloc(sizeof(tBTA_DM_API_EXT_CONN))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_EXT_CONN));
        p_msg->hdr.event = BTA_DM_API_EXT_CONN_EVT;
        p_msg->own_addr_type = own_addr_type;
        memcpy(p_msg->peer_addr, peer_addr, sizeof(BD_ADDR));
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_50_DTM_TEST_EN == TRUE)
void BTA_DmBleDtmEnhTxStart(uint8_t tx_channel, uint8_t len_of_data, uint8_t pkt_payload, uint8_t phy, tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback)
{
    tBTA_DM_API_BLE_DTM_ENH_TX_START *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_DTM_ENH_TX_START *)osi_malloc(sizeof(tBTA_DM_API_BLE_DTM_ENH_TX_START)))
            != NULL) {
        p_msg->hdr.event = BTA_DM_API_DTM_ENH_TX_START_EVT;
        p_msg->tx_channel = tx_channel;
        p_msg->len_of_data = len_of_data;
        p_msg->pkt_payload = pkt_payload;
        p_msg->phy = phy;
        p_msg->p_dtm_cmpl_cback = p_dtm_cmpl_cback;

        bta_sys_sendmsg(p_msg);
    }
}

void BTA_DmBleDtmEnhRxStart(uint8_t rx_channel, uint8_t phy, uint8_t modulation_index, tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback)
{
    tBTA_DM_API_BLE_DTM_ENH_RX_START *p_msg;

    if ((p_msg = (tBTA_DM_API_BLE_DTM_ENH_RX_START *)osi_malloc(sizeof(tBTA_DM_API_BLE_DTM_ENH_RX_START)))
            != NULL) {
        p_msg->hdr.event = BTA_DM_API_DTM_ENH_RX_START_EVT;
        p_msg->rx_channel= rx_channel;
        p_msg->phy = phy;
        p_msg->modulation_index = modulation_index;
        p_msg->p_dtm_cmpl_cback = p_dtm_cmpl_cback;

        bta_sys_sendmsg(p_msg);
    }
}
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
void BTA_DmBleGapPeriodicAdvRecvEnable(UINT16 sync_handle, UINT8 enable)
{
    tBTA_DM_API_PERIODIC_ADV_RECV_ENABLE *p_msg;
    p_msg = (tBTA_DM_API_PERIODIC_ADV_RECV_ENABLE *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_RECV_ENABLE));
    if (p_msg != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_RECV_ENABLE));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_RECV_ENABLE_EVT;
        p_msg->sync_handle = sync_handle;
        p_msg->enable = enable;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapPeriodicAdvSyncTrans(BD_ADDR peer_addr, UINT16 service_data, UINT16 sync_handle)
{
    tBTA_DM_API_PERIODIC_ADV_SYNC_TRANS *p_msg;
    p_msg = (tBTA_DM_API_PERIODIC_ADV_SYNC_TRANS *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_SYNC_TRANS));
    if (p_msg != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_SYNC_TRANS));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_SYNC_TRANS_EVT;
        memcpy(p_msg->addr, peer_addr, sizeof(BD_ADDR));
        p_msg->service_data = service_data;
        p_msg->sync_handle = sync_handle;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapPeriodicAdvSetInfoTrans(BD_ADDR peer_addr, UINT16 service_data, UINT8 adv_handle)
{
    tBTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS *p_msg;
    p_msg = (tBTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS *) osi_malloc(sizeof(tBTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS));
    if (p_msg != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS));
        p_msg->hdr.event = BTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS_EVT;
        memcpy(p_msg->addr, peer_addr, sizeof(BD_ADDR));
        p_msg->service_data = service_data;
        p_msg->adv_hanlde = adv_handle;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapSetPeriodicAdvSyncTransParams(BD_ADDR peer_addr, tBTA_DM_BLE_PAST_PARAMS *params)
{
    tBTA_DM_API_SET_PAST_PARAMS *p_msg;
    p_msg = (tBTA_DM_API_SET_PAST_PARAMS *) osi_malloc(sizeof(tBTA_DM_API_SET_PAST_PARAMS));
    if (p_msg != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_SET_PAST_PARAMS));
        p_msg->hdr.event = BTA_DM_API_SET_PERIODIC_ADV_SYNC_TRANS_PARAMS_EVT;
        memcpy(p_msg->addr, peer_addr, sizeof(BD_ADDR));
        memcpy(&p_msg->params, params, sizeof(tBTA_DM_BLE_PAST_PARAMS));
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
void BTA_DmBleGapIsoBigCreate(tBTA_DM_BLE_BIG_CREATE_PARAMS *p_big_creat_param)
{
    tBTA_DM_API_BIG_CREATE *p_msg;
    APPL_TRACE_API("%s.", __func__);
    if ((p_msg = (tBTA_DM_API_BIG_CREATE *) osi_malloc(sizeof(tBTA_DM_API_BIG_CREATE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BIG_CREATE));
        p_msg->hdr.event = BTA_DM_API_ISO_BIG_CREATE_EVT;
        p_msg->big_creat_param.big_handle = p_big_creat_param->big_handle;
        p_msg->big_creat_param.adv_handle = p_big_creat_param->adv_handle;
        p_msg->big_creat_param.num_bis = p_big_creat_param->num_bis;
        p_msg->big_creat_param.sdu_interval = p_big_creat_param->sdu_interval;
        p_msg->big_creat_param.max_sdu = p_big_creat_param->max_sdu;
        p_msg->big_creat_param.max_transport_latency = p_big_creat_param->max_transport_latency;
        p_msg->big_creat_param.rtn = p_big_creat_param->rtn;
        p_msg->big_creat_param.phy = p_big_creat_param->phy;
        p_msg->big_creat_param.packing = p_big_creat_param->packing;
        p_msg->big_creat_param.framing = p_big_creat_param->framing;
        p_msg->big_creat_param.encryption = p_big_creat_param->encryption;
        memcpy(&p_msg->big_creat_param.broadcast_code[0], p_big_creat_param->broadcast_code, 16);
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }

}

void BTA_DmBleGapIsoBigCreateTest(tBTA_DM_BLE_BIG_CREATE_TEST_PARAMS *p_big_creat_test_param)
{
    tBTA_DM_API_BIG_CREATE_TEST *p_msg;
    APPL_TRACE_API("%s.", __func__);
    if ((p_msg = (tBTA_DM_API_BIG_CREATE_TEST *) osi_malloc(sizeof(tBTA_DM_API_BIG_CREATE_TEST))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BIG_CREATE_TEST));
        p_msg->hdr.event = BTA_DM_API_ISO_BIG_CREATE_TEST_EVT;
        p_msg->big_creat_test_param.big_handle = p_big_creat_test_param->big_handle;
        p_msg->big_creat_test_param.adv_handle = p_big_creat_test_param->adv_handle;
        p_msg->big_creat_test_param.num_bis = p_big_creat_test_param->num_bis;
        p_msg->big_creat_test_param.sdu_interval = p_big_creat_test_param->sdu_interval;
        p_msg->big_creat_test_param.iso_interval = p_big_creat_test_param->iso_interval;
        p_msg->big_creat_test_param.nse = p_big_creat_test_param->nse;
        p_msg->big_creat_test_param.max_sdu = p_big_creat_test_param->max_sdu;
        p_msg->big_creat_test_param.max_pdu = p_big_creat_test_param->max_pdu;
        p_msg->big_creat_test_param.phy = p_big_creat_test_param->phy;
        p_msg->big_creat_test_param.packing = p_big_creat_test_param->packing;
        p_msg->big_creat_test_param.framing = p_big_creat_test_param->framing;
        p_msg->big_creat_test_param.bn = p_big_creat_test_param->bn;
        p_msg->big_creat_test_param.irc = p_big_creat_test_param->irc;
        p_msg->big_creat_test_param.pto = p_big_creat_test_param->pto;
        p_msg->big_creat_test_param.encryption = p_big_creat_test_param->encryption;
        memcpy(&p_msg->big_creat_test_param.broadcast_code[0], p_big_creat_test_param->broadcast_code, 16);

        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapIsoBigTerminate(tBTA_DM_BLE_BIG_TERMINATE_PARAMS *p_big_terminate_param)
{
    tBTA_DM_API_BIG_TERMINATE *p_msg;
    APPL_TRACE_API("%s.", __func__);
    if ((p_msg = (tBTA_DM_API_BIG_TERMINATE *) osi_malloc(sizeof(tBTA_DM_API_BIG_TERMINATE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BIG_TERMINATE));
        p_msg->hdr.event = BTA_DM_API_ISO_BIG_TERMINATE_EVT;
        p_msg->big_terminate_param.big_handle = p_big_terminate_param->big_handle;
        p_msg->big_terminate_param.reason = p_big_terminate_param->reason;

        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
void BTA_DmBleGapIsoBigSyncCreate(tBTA_DM_BLE_BIG_SYNC_CREATE_PARAMS *p_big_sync_param)
{
    tBTA_DM_API_BIG_SYNC_CREATE *p_msg;
    APPL_TRACE_API("%s.", __func__);
    if ((p_msg = (tBTA_DM_API_BIG_SYNC_CREATE *) osi_malloc(sizeof(tBTA_DM_API_BIG_SYNC_CREATE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BIG_SYNC_CREATE));
        p_msg->hdr.event = BTA_DM_API_ISO_BIG_SYNC_CREATE_EVT;

        p_msg->big_sync_param.big_handle = p_big_sync_param->big_handle;
        p_msg->big_sync_param.sync_handle = p_big_sync_param->sync_handle;
        p_msg->big_sync_param.encryption = p_big_sync_param->encryption;
        memcpy(&p_msg->big_sync_param.bc_code[0], &p_big_sync_param->bc_code[0], 16);
        p_msg->big_sync_param.mse = p_big_sync_param->mse;
        p_msg->big_sync_param.big_sync_timeout = p_big_sync_param->big_sync_timeout;
        p_msg->big_sync_param.num_bis = p_big_sync_param->num_bis;
        memcpy(&p_msg->big_sync_param.bis[0], &p_big_sync_param->bis[0], p_big_sync_param->num_bis);

        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapIsoBigSyncTerminate(tBTA_DM_BLE_BIG_SYNC_TERMINATE_PARAMS *p_big_sync_terminate_param)
{
    tBTA_DM_API_BIG_SYNC_TERMINATE *p_msg;
    APPL_TRACE_API("%s.", __func__);
    if ((p_msg = (tBTA_DM_API_BIG_SYNC_TERMINATE *) osi_malloc(sizeof(tBTA_DM_API_BIG_SYNC_TERMINATE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_BIG_SYNC_TERMINATE));
        p_msg->hdr.event = BTA_DM_API_ISO_BIG_SYNC_TERMINATE_EVT;

        p_msg->big_sync_terminate_param.big_handle = p_big_sync_terminate_param->big_handle;

        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
void BTA_DmBleGapIsoDataPathSet(tBTA_DM_BLE_ISO_SET_DATA_PATH_PARAMS *p_iso_data_path_param)
{
    tBTA_DM_API_ISO_DATA_PATH *p_msg;
    uint8_t codec_cfg_len = p_iso_data_path_param->codec_len;
    APPL_TRACE_API("%s", __func__);
    if ((p_msg = (tBTA_DM_API_ISO_DATA_PATH *) osi_malloc(sizeof(tBTA_DM_API_ISO_DATA_PATH) + codec_cfg_len)) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_ISO_DATA_PATH));
        p_msg->hdr.event = BTA_DM_API_ISO_SET_DATA_PATH_EVT;

        p_msg->iso_data_path_param.conn_handle = p_iso_data_path_param->conn_handle;
        p_msg->iso_data_path_param.data_path_dir = p_iso_data_path_param->data_path_dir;
        p_msg->iso_data_path_param.data_path_id = p_iso_data_path_param->data_path_id;
        p_msg->iso_data_path_param.coding_fmt = p_iso_data_path_param->coding_fmt;
        p_msg->iso_data_path_param.company_id = p_iso_data_path_param->company_id;
        p_msg->iso_data_path_param.vs_codec_id = p_iso_data_path_param->vs_codec_id;
        p_msg->iso_data_path_param.controller_delay = p_iso_data_path_param->controller_delay;
        p_msg->iso_data_path_param.codec_len = p_iso_data_path_param->codec_len;
        p_msg->iso_data_path_param.codec_cfg = (codec_cfg_len != 0) ? (UINT8 *)(p_msg + 1) : NULL;
        if (codec_cfg_len) {
            memcpy(p_msg->iso_data_path_param.codec_cfg, p_iso_data_path_param->codec_cfg, p_iso_data_path_param->codec_len);
        }
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapIsoDataPathRemove(tBTA_DM_BLE_ISO_REMOVE_DATA_PATH_PARAMS *p_iso_data_path_param)
{
    tBTA_DM_API_ISO_DATA_PATH_REMOVE *p_msg;
    APPL_TRACE_API("%s", __func__);
    if ((p_msg = (tBTA_DM_API_ISO_DATA_PATH_REMOVE *) osi_malloc(sizeof(tBTA_DM_API_ISO_DATA_PATH_REMOVE))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_ISO_DATA_PATH_REMOVE));
        p_msg->hdr.event = BTA_DM_API_ISO_REMOVE_DATA_PATH_EVT;

        p_msg->iso_data_path_remove_param.conn_handle = p_iso_data_path_param->conn_handle;
        p_msg->iso_data_path_remove_param.data_path_dir = p_iso_data_path_param->data_path_dir;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapIsoReadTxSync(uint16_t iso_handle)
{
    tBTA_DM_API_ISO_READ_TX_SYNC *p_msg;
    APPL_TRACE_API("%s", __func__);
    if ((p_msg = (tBTA_DM_API_ISO_READ_TX_SYNC *) osi_malloc(sizeof(tBTA_DM_API_ISO_READ_TX_SYNC))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_ISO_READ_TX_SYNC));
        p_msg->hdr.event = BTA_DM_API_ISO_READ_TX_SYNC_EVT;

        p_msg->iso_hdl= iso_handle;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleGapIsoReadLinkQuality(uint16_t iso_handle)
{
    tBTA_DM_API_ISO_READ_LINK_QUALITY *p_msg;
    APPL_TRACE_API("%s", __func__);
    if ((p_msg = (tBTA_DM_API_ISO_READ_LINK_QUALITY *) osi_malloc(sizeof(tBTA_DM_API_ISO_READ_LINK_QUALITY))) != NULL) {
        memset(p_msg, 0, sizeof(tBTA_DM_API_ISO_READ_LINK_QUALITY));
        p_msg->hdr.event = BTA_DM_API_ISO_READ_LINK_QUALITY_EVT;

        p_msg->iso_hdl= iso_handle;
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_msg);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
void BTA_DmBleIsoSetCigParams(uint8_t cig_id, uint32_t sdu_int_c_to_p, uint32_t sdu_int_p_to_c, uint8_t worse_case_SCA, uint8_t packing,
                            uint8_t framing, uint16_t mtl_c_to_p, uint16_t mtl_p_to_c, uint8_t cis_cnt, uint8_t *cis_params)
{
    tBTA_DM_API_SET_CIG_PARAM *p_buf;
    if ((p_buf = (tBTA_DM_API_SET_CIG_PARAM *)osi_malloc(sizeof(tBTA_DM_API_SET_CIG_PARAM))) != NULL) {
        p_buf->hdr.event = BTA_DM_API_SET_CIG_PARAMS_EVT;
        p_buf->cig_id = cig_id;
        p_buf->sdu_int_c_to_p = sdu_int_c_to_p;
        p_buf->sdu_int_p_to_c = sdu_int_p_to_c;
        p_buf->worse_case_SCA = worse_case_SCA;
        p_buf->packing = packing;
        p_buf->framing = framing;
        p_buf->mtl_c_to_p = mtl_c_to_p;
        p_buf->mtl_p_to_c = mtl_p_to_c;
        p_buf->cis_cnt = cis_cnt;
        memcpy(&p_buf->cis_params[0], cis_params, sizeof(struct bta_iso_cis_params) * cis_cnt);

        bta_sys_sendmsg(p_buf);
    }
    return;
}

void BTA_DmBleIsoSetCigParamsTest(uint8_t cig_id, uint32_t sdu_int_c_to_p, uint32_t sdu_int_p_to_c, uint8_t ft_c_to_p, uint8_t ft_p_to_c,
                                 uint16_t iso_interval, uint8_t worse_case_SCA, uint8_t packing, uint8_t framing, uint8_t cis_cnt, uint8_t *cis_params_test)
{
    tBTA_DM_API_SET_CIG_PARAM_TEST *p_buf;
    if ((p_buf = (tBTA_DM_API_SET_CIG_PARAM_TEST *)osi_malloc(sizeof(tBTA_DM_API_SET_CIG_PARAM_TEST))) != NULL) {
        p_buf->hdr.event = BTA_DM_API_SET_CIG_PARAMS_TEST_EVT;
        p_buf->cig_id = cig_id;
        p_buf->sdu_int_c_to_p = sdu_int_c_to_p;
        p_buf->sdu_int_p_to_c = sdu_int_p_to_c;
        p_buf->ft_c_to_p = ft_c_to_p;
        p_buf->ft_p_to_c = ft_p_to_c;
        p_buf->iso_interval = iso_interval;
        p_buf->worse_case_SCA = worse_case_SCA;
        p_buf->packing = packing;
        p_buf->framing = framing;
        p_buf->cis_cnt = cis_cnt;
        memcpy(&p_buf->cis_params_test[0], cis_params_test, sizeof(struct bta_iso_cis_params_test) * cis_cnt);

        bta_sys_sendmsg(p_buf);
    }
    return;
}

void BTA_DmBleIsoCreateCis(uint8_t cig_count, uint8_t *cis_hdl_info)
{
    tBTA_DM_API_CREATE_CIS_PARAM *p_buf;
    if ((p_buf = (tBTA_DM_API_CREATE_CIS_PARAM *)osi_malloc(sizeof(tBTA_DM_API_CREATE_CIS_PARAM))) != NULL) {
        p_buf->hdr.event = BTA_DM_API_CREATE_CIS_EVT;
        p_buf->cis_count = cig_count;

        struct bta_iso_cis_hdls *hdls = (struct bta_iso_cis_hdls *)cis_hdl_info;

        for (uint8_t i = 0; i < cig_count; i++)
        {
            p_buf->cis_hdls[i].cis_hdl = hdls[i].cis_hdl;
            p_buf->cis_hdls[i].acl_hdl = hdls[i].acl_hdl;
        }

        bta_sys_sendmsg(p_buf);
    }
    return;
}

void BTA_DmBleIsoRemoveCig(uint8_t cig_id)
{
    tBTA_DM_API_REMOVE_CIG_PARAM *p_buf;
    if ((p_buf = (tBTA_DM_API_REMOVE_CIG_PARAM *)osi_malloc(sizeof(tBTA_DM_API_REMOVE_CIG_PARAM))) != NULL) {
        p_buf->hdr.event = BTA_DM_API_REMOVE_CIG_PARAMS_EVT;
        p_buf->cig_id = cig_id;
        bta_sys_sendmsg(p_buf);
    }
    return;
}

#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
void BTA_DmBleIsoAcceptCisReq(uint16_t cis_handle)
{
    tBTA_DM_API_ACCEPT_CIS_REQ_PARAM *p_buf;
    if ((p_buf = (tBTA_DM_API_ACCEPT_CIS_REQ_PARAM *)osi_malloc(sizeof(tBTA_DM_API_ACCEPT_CIS_REQ_PARAM))) != NULL) {
        p_buf->hdr.event = BTA_DM_API_ACCEPT_CIS_REQ_PARAMS_EVT;
        p_buf->cis_handle = cis_handle;
        bta_sys_sendmsg(p_buf);
    }
    return;
}

void BTA_DmBleIsoRejectCisReq(uint16_t cis_handle, uint8_t reason)
{
    tBTA_DM_API_REJECT_CIS_REQ_PARAM *p_buf;
    if ((p_buf = (tBTA_DM_API_REJECT_CIS_REQ_PARAM *)osi_malloc(sizeof(tBTA_DM_API_REJECT_CIS_REQ_PARAM))) != NULL) {
        p_buf->hdr.event = BTA_DM_API_REJECT_CIS_REQ_PARAMS_EVT;
        p_buf->cis_handle = cis_handle;
        p_buf->reason = reason;
        bta_sys_sendmsg(p_buf);
    }
    return;
}
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_EN == TRUE)
void BTA_DmBleIsoDisconCis(uint16_t cis_handle, uint8_t reason)
{
    tBTA_DM_API_DISCON_CIS_PARAM *p_buf;
    if ((p_buf = (tBTA_DM_API_DISCON_CIS_PARAM *)osi_malloc(sizeof(tBTA_DM_API_DISCON_CIS_PARAM))) != NULL) {
        p_buf->hdr.event = BTA_DM_API_DISCON_CIS_EVT;
        p_buf->cis_handle = cis_handle;
        p_buf->reason = reason;
        bta_sys_sendmsg(p_buf);
    }
    return;
}
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)

#endif // #if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_CTE_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
void BTA_DmBleCteSetConnectionlessTransParams(uint8_t adv_handle, uint8_t cte_len, uint8_t cte_type,
                                                uint8_t cte_count, uint8_t switching_pattern_len, uint8_t *antenna_ids)
{
    APPL_TRACE_API("%s", __func__);

    tBTA_DM_BLE_CTE_SET_TRANS_PARAMS *p_buf;
    if ((p_buf = (tBTA_DM_BLE_CTE_SET_TRANS_PARAMS *) osi_malloc(sizeof(tBTA_DM_BLE_CTE_SET_TRANS_PARAMS) + switching_pattern_len)) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_DM_BLE_CTE_SET_TRANS_PARAMS) + switching_pattern_len);
        p_buf->hdr.event = BTA_DM_API_CTE_SET_TRANS_PARAMS;

        p_buf->adv_handle = adv_handle;
        p_buf->cte_len = cte_len;
        p_buf->cte_type = cte_type;
        p_buf->cte_count = cte_count;
        p_buf->switching_pattern_len = switching_pattern_len;
        p_buf->antenna_ids = (switching_pattern_len != 0) ? (UINT8 *)(p_buf + 1) : NULL;
        if (switching_pattern_len) {
            memcpy(p_buf->antenna_ids, antenna_ids, switching_pattern_len);
        }
        //start sent the msg to the bta system control module
        bta_sys_sendmsg(p_buf);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleCteSetConnectionlessTransEnable(uint8_t adv_handle, uint8_t cte_en)
{
    APPL_TRACE_API("%s", __func__);

    tBTA_DM_BLE_CTE_SET_TRANS_ENABLE *p_buf;
    if ((p_buf = (tBTA_DM_BLE_CTE_SET_TRANS_ENABLE *) osi_malloc(sizeof(tBTA_DM_BLE_CTE_SET_TRANS_ENABLE))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_DM_BLE_CTE_SET_TRANS_ENABLE));
        p_buf->hdr.event = BTA_DM_API_CTE_SET_TRANS_ENABLE;

        p_buf->adv_handle = adv_handle;
        p_buf->cte_enable = cte_en;
        // start sent the msg to the bta system control module
        bta_sys_sendmsg(p_buf);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleCteSetConnectionlessIqSamplingEnable(uint16_t sync_handle, uint8_t sampling_en, uint8_t slot_dur,
                                                    uint8_t max_sampled_ctes, uint8_t switching_pattern_len, uint8_t *ant_ids)
{
    APPL_TRACE_API("%s", __func__);

    tBTA_DM_BLE_CTE_IQ_SAMP_EN *p_buf;
    if ((p_buf = (tBTA_DM_BLE_CTE_IQ_SAMP_EN *) osi_malloc(sizeof(tBTA_DM_BLE_CTE_IQ_SAMP_EN) + switching_pattern_len)) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_DM_BLE_CTE_IQ_SAMP_EN) + switching_pattern_len);
        p_buf->hdr.event = BTA_DM_API_CTE_SET_IQ_SAMPLING_EN;

        p_buf->sync_handle = sync_handle;
        p_buf->sampling_en = sampling_en;
        p_buf->slot_dur = slot_dur;
        p_buf->max_sampled_ctes = max_sampled_ctes;
        p_buf->switching_pattern_len = switching_pattern_len;
        p_buf->antenna_ids = (switching_pattern_len != 0) ? (UINT8 *)(p_buf + 1) : NULL;
        if (switching_pattern_len) {
            memcpy(p_buf->antenna_ids, ant_ids, switching_pattern_len);
        }
        // start sent the msg to the bta system control module
        bta_sys_sendmsg(p_buf);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
void BTA_DmBleCteSetConnectionReceiveParams(uint16_t conn_handle, uint8_t sampling_en, uint8_t slot_dur,
                                            uint8_t switching_pattern_len, uint8_t *ant_ids)
{
    APPL_TRACE_API("%s", __func__);

    tBTA_DM_BLE_CTE_RECV_PARAMS *p_buf;
    if ((p_buf = (tBTA_DM_BLE_CTE_RECV_PARAMS *) osi_malloc(sizeof(tBTA_DM_BLE_CTE_RECV_PARAMS) + switching_pattern_len)) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_DM_BLE_CTE_RECV_PARAMS) + switching_pattern_len);
        p_buf->hdr.event = BTA_DM_API_CTE_SET_CONN_CTE_RECV_PARAMS;

        p_buf->conn_handle = conn_handle;
        p_buf->sampling_en = sampling_en;
        p_buf->slot_dur = slot_dur;
        p_buf->switching_pattern_len = switching_pattern_len;
        p_buf->antenna_ids = (switching_pattern_len != 0) ? (UINT8 *)(p_buf + 1) : NULL;
        if (switching_pattern_len) {
            memcpy(p_buf->antenna_ids, ant_ids, switching_pattern_len);
        }
        // start sent the msg to the bta system control module
        bta_sys_sendmsg(p_buf);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleCteSetConnectionTransParams(uint16_t conn_handle, uint8_t cte_types, uint8_t switching_pattern_len, uint8_t *ant_ids)
{
    APPL_TRACE_API("%s", __func__);

    tBTA_DM_BLE_CONN_CTE_TRANS_PARAMS *p_buf;
    if ((p_buf = (tBTA_DM_BLE_CONN_CTE_TRANS_PARAMS *) osi_malloc(sizeof(tBTA_DM_BLE_CONN_CTE_TRANS_PARAMS) + switching_pattern_len)) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_DM_BLE_CONN_CTE_TRANS_PARAMS) + switching_pattern_len);
        p_buf->hdr.event = BTA_DM_API_CTE_SET_CONN_CTE_TRANS_PARAMS;

        p_buf->conn_handle = conn_handle;
        p_buf->cte_types = cte_types;
        p_buf->switching_pattern_len = switching_pattern_len;
        p_buf->antenna_ids = (switching_pattern_len != 0) ? (UINT8 *)(p_buf + 1) : NULL;
        if (switching_pattern_len) {
            memcpy(p_buf->antenna_ids, ant_ids, switching_pattern_len);
        }
        // start sent the msg to the bta system control module
        bta_sys_sendmsg(p_buf);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleCteSetConnectionRequestEnable(uint16_t conn_handle, uint8_t enable, uint16_t cte_req_interval,
                                            uint8_t req_cte_len, uint8_t req_cte_Type)
{
    APPL_TRACE_API("%s", __func__);

    tBTA_DM_BLE_CONN_CTE_REQ_EN *p_buf;
    if ((p_buf = (tBTA_DM_BLE_CONN_CTE_REQ_EN *) osi_malloc(sizeof(tBTA_DM_BLE_CONN_CTE_REQ_EN))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_DM_BLE_CONN_CTE_REQ_EN));
        p_buf->hdr.event = BTA_DM_API_CTE_SET_CONN_CTE_REQUEST_EN;

        p_buf->conn_handle = conn_handle;
        p_buf->enable = enable;
        p_buf->cte_req_interval = cte_req_interval;
        p_buf->req_cte_len = req_cte_len;
        p_buf->req_cte_Type = req_cte_Type;
        // start sent the msg to the bta system control module
        bta_sys_sendmsg(p_buf);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

void BTA_DmBleCteSetConnectionRspEnable(uint16_t conn_handle, uint8_t enable)
{
    APPL_TRACE_API("%s", __func__);

    tBTA_DM_BLE_CONN_CTE_RSP_EN *p_buf;
    if ((p_buf = (tBTA_DM_BLE_CONN_CTE_RSP_EN *) osi_malloc(sizeof(tBTA_DM_BLE_CONN_CTE_RSP_EN))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_DM_BLE_CONN_CTE_RSP_EN));
        p_buf->hdr.event = BTA_DM_API_CTE_SET_CONN_CTE_RESPONSE_EN;

        p_buf->conn_handle = conn_handle;
        p_buf->enable = enable;
        // start sent the msg to the bta system control module
        bta_sys_sendmsg(p_buf);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
void BTA_DmBleCteReadAntInfor(void)
{
    APPL_TRACE_API("%s", __func__);

    tBTA_DM_BLE_READ_ANT_INFOR *p_buf;
    if ((p_buf = (tBTA_DM_BLE_READ_ANT_INFOR *) osi_malloc(sizeof(tBTA_DM_BLE_READ_ANT_INFOR))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_DM_BLE_READ_ANT_INFOR));
        p_buf->hdr.event = BTA_DM_API_CTE_READ_ANTENNA_INFOR;
        // start sent the msg to the bta system control module
        bta_sys_sendmsg(p_buf);
    } else {
        APPL_TRACE_ERROR("%s malloc failed", __func__);
    }
}

#endif // #if (BLE_FEAT_CTE_EN == TRUE)
#endif
