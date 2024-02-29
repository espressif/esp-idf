/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/************************************************************************************
 *
 *  Filename:      btc_util.c
 *
 *  Description:   Miscellaneous helper functions
 *
 *
 ***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "btc/btc_util.h"
#if (BTA_AV_INCLUDED == TRUE)
#include "bta/bta_av_api.h"
#endif  ///BTA_AV_INCLUDED == TRUE

#if (BTA_AG_INCLUDED == TRUE)
#include "bta/bta_ag_api.h"
#endif  ///BTA_AG_INCLUDED == TRUE

#if (BTA_HH_INCLUDED == TRUE)
#include "bta/bta_hh_api.h"
#endif  ///BTA_HH_INCLUDED == TRUE
#include "bta/bta_hd_api.h"
#include "common/bt_defs.h"
#include "stack/btm_api.h"
#include "bta/bta_api.h"

/************************************************************************************
**  Constants & Macros
************************************************************************************/
#define ISDIGIT(a)  ((a>='0') && (a<='9'))
#define ISXDIGIT(a) (((a>='0') && (a<='9'))||((a>='A') && (a<='F'))||((a>='a') && (a<='f')))

/************************************************************************************
**  Local type definitions
************************************************************************************/

/************************************************************************************
**  Static variables
************************************************************************************/

/************************************************************************************
**  Static functions
************************************************************************************/

/************************************************************************************
**  Externs
************************************************************************************/

/************************************************************************************
**  Functions
************************************************************************************/

/*****************************************************************************
**   Logging helper functions
*****************************************************************************/
#if(BTA_AV_INCLUDED == TRUE)
const char *dump_rc_event(UINT8 event)
{
    switch (event) {
        CASE_RETURN_STR(BTA_AV_RC_OPEN_EVT)
        CASE_RETURN_STR(BTA_AV_RC_CLOSE_EVT)
        CASE_RETURN_STR(BTA_AV_REMOTE_CMD_EVT)
        CASE_RETURN_STR(BTA_AV_REMOTE_RSP_EVT)
        CASE_RETURN_STR(BTA_AV_VENDOR_CMD_EVT)
        CASE_RETURN_STR(BTA_AV_VENDOR_RSP_EVT)
        CASE_RETURN_STR(BTA_AV_META_MSG_EVT)
        CASE_RETURN_STR(BTA_AV_RC_FEAT_EVT)
    default:
        return "UNKNOWN_EVENT";
    }
}

const char *dump_rc_notification_event_id(UINT8 event_id)
{
    switch (event_id) {
        CASE_RETURN_STR(AVRC_EVT_PLAY_STATUS_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_TRACK_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_TRACK_REACHED_END)
        CASE_RETURN_STR(AVRC_EVT_TRACK_REACHED_START)
        CASE_RETURN_STR(AVRC_EVT_PLAY_POS_CHANGED)
        CASE_RETURN_STR(AVRC_EVT_BATTERY_STATUS_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_SYSTEM_STATUS_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_APP_SETTING_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_VOLUME_CHANGE)

    default:
        return "Unhandled Event ID";
    }
}

const char  *dump_rc_pdu(UINT8 pdu)
{
    switch (pdu) {
        CASE_RETURN_STR(AVRC_PDU_LIST_PLAYER_APP_ATTR)
        CASE_RETURN_STR(AVRC_PDU_LIST_PLAYER_APP_VALUES)
        CASE_RETURN_STR(AVRC_PDU_GET_CUR_PLAYER_APP_VALUE)
        CASE_RETURN_STR(AVRC_PDU_SET_PLAYER_APP_VALUE)
        CASE_RETURN_STR(AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT)
        CASE_RETURN_STR(AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT)
        CASE_RETURN_STR(AVRC_PDU_INFORM_DISPLAY_CHARSET)
        CASE_RETURN_STR(AVRC_PDU_INFORM_BATTERY_STAT_OF_CT)
        CASE_RETURN_STR(AVRC_PDU_GET_ELEMENT_ATTR)
        CASE_RETURN_STR(AVRC_PDU_GET_PLAY_STATUS)
        CASE_RETURN_STR(AVRC_PDU_REGISTER_NOTIFICATION)
        CASE_RETURN_STR(AVRC_PDU_REQUEST_CONTINUATION_RSP)
        CASE_RETURN_STR(AVRC_PDU_ABORT_CONTINUATION_RSP)
        CASE_RETURN_STR(AVRC_PDU_SET_ABSOLUTE_VOLUME)
    default:
        return "Unknown PDU";
    }
}
#endif  ///BTA_AV_INCLUDED == TRUE

#if (BTA_AG_INCLUDED == TRUE)
const char* dump_hf_conn_state(UINT16 event)
{
    switch(event)
    {
        CASE_RETURN_STR(ESP_HF_CONNECTION_STATE_DISCONNECTED)
        CASE_RETURN_STR(ESP_HF_CONNECTION_STATE_CONNECTING)
        CASE_RETURN_STR(ESP_HF_CONNECTION_STATE_CONNECTED)
        CASE_RETURN_STR(ESP_HF_CONNECTION_STATE_SLC_CONNECTED)
        CASE_RETURN_STR(ESP_HF_CONNECTION_STATE_DISCONNECTING)
        default:
            return "UNKNOWN MSG ID";
    }
}

const char* dump_hf_event(UINT16 event)
{
    switch(event)
    {
        CASE_RETURN_STR(BTA_AG_ENABLE_EVT)
        CASE_RETURN_STR(BTA_AG_REGISTER_EVT)
        CASE_RETURN_STR(BTA_AG_OPEN_EVT)
        CASE_RETURN_STR(BTA_AG_CLOSE_EVT)
        CASE_RETURN_STR(BTA_AG_CONN_EVT)
        CASE_RETURN_STR(BTA_AG_AUDIO_OPEN_EVT)
        CASE_RETURN_STR(BTA_AG_AUDIO_CLOSE_EVT)
        CASE_RETURN_STR(BTA_AG_SPK_EVT)
        CASE_RETURN_STR(BTA_AG_MIC_EVT)
        CASE_RETURN_STR(BTA_AG_AT_CKPD_EVT)
        CASE_RETURN_STR(BTA_AG_DISABLE_EVT)
#if (BTM_WBS_INCLUDED == TRUE)
        CASE_RETURN_STR(BTA_AG_WBS_EVT)
#endif
        CASE_RETURN_STR(BTA_AG_AT_A_EVT)
        CASE_RETURN_STR(BTA_AG_AT_D_EVT)
        CASE_RETURN_STR(BTA_AG_AT_CHLD_EVT)
        CASE_RETURN_STR(BTA_AG_AT_CHUP_EVT)
        CASE_RETURN_STR(BTA_AG_AT_CIND_EVT)
        CASE_RETURN_STR(BTA_AG_AT_VTS_EVT)
        CASE_RETURN_STR(BTA_AG_AT_BINP_EVT)
        CASE_RETURN_STR(BTA_AG_AT_BLDN_EVT)
        CASE_RETURN_STR(BTA_AG_AT_BVRA_EVT)
        CASE_RETURN_STR(BTA_AG_AT_NREC_EVT)
        CASE_RETURN_STR(BTA_AG_AT_CNUM_EVT)
        CASE_RETURN_STR(BTA_AG_AT_BTRH_EVT)
        CASE_RETURN_STR(BTA_AG_AT_CLCC_EVT)
        CASE_RETURN_STR(BTA_AG_AT_COPS_EVT)
        CASE_RETURN_STR(BTA_AG_AT_UNAT_EVT)
        CASE_RETURN_STR(BTA_AG_AT_CBC_EVT)
        CASE_RETURN_STR(BTA_AG_AT_BAC_EVT)
        CASE_RETURN_STR(BTA_AG_AT_BCS_EVT)

        default:
            return "UNKNOWN MSG ID";
     }
}

const char* dump_hf_call_state(esp_hf_call_status_t  call_state)
{
    switch(call_state)
    {
        CASE_RETURN_STR(ESP_HF_CALL_STATUS_NO_CALLS)
        CASE_RETURN_STR(ESP_HF_CALL_STATUS_CALL_IN_PROGRESS)
        default:
            return "UNKNOWN CALL STATE";
    }
}

const char* dump_hf_call_setup_state(esp_hf_call_setup_status_t call_setup_state)
{
    switch(call_setup_state)
    {
        CASE_RETURN_STR(ESP_HF_CALL_SETUP_STATUS_IDLE)
        CASE_RETURN_STR(ESP_HF_CALL_SETUP_STATUS_INCOMING)
        CASE_RETURN_STR(ESP_HF_CALL_SETUP_STATUS_OUTGOING_DIALING)
        CASE_RETURN_STR(ESP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING)
        default:
            return "UNKNOWN CALL SETUP STATE";
    }
}

#endif // #if (BTA_AG_INCLUDED == TRUE)

#if (BTA_HH_INCLUDED == TRUE)
const char *dump_hh_event(uint16_t event)
{
    switch (event) {
        CASE_RETURN_STR(BTA_HH_ENABLE_EVT)
        CASE_RETURN_STR(BTA_HH_DISABLE_EVT)
        CASE_RETURN_STR(BTA_HH_OPEN_EVT)
        CASE_RETURN_STR(BTA_HH_CLOSE_EVT)
        CASE_RETURN_STR(BTA_HH_GET_RPT_EVT)
        CASE_RETURN_STR(BTA_HH_SET_RPT_EVT)
        CASE_RETURN_STR(BTA_HH_GET_PROTO_EVT)
        CASE_RETURN_STR(BTA_HH_SET_PROTO_EVT)
        CASE_RETURN_STR(BTA_HH_GET_IDLE_EVT)
        CASE_RETURN_STR(BTA_HH_SET_IDLE_EVT)
        CASE_RETURN_STR(BTA_HH_GET_DSCP_EVT)
        CASE_RETURN_STR(BTA_HH_ADD_DEV_EVT)
        CASE_RETURN_STR(BTA_HH_RMV_DEV_EVT)
        CASE_RETURN_STR(BTA_HH_VC_UNPLUG_EVT)
        CASE_RETURN_STR(BTA_HH_DATA_EVT)
        CASE_RETURN_STR(BTA_HH_API_ERR_EVT)
        CASE_RETURN_STR(BTA_HH_UPDATE_SCPP_EVT)
        CASE_RETURN_STR(BTA_HH_DATA_IND_EVT)
    default:
        return "UNKNOWN MSG ID";
    }
}
#endif ///BTA_HH_INCLUDED

#if BTA_HD_INCLUDED == TRUE
const char* dump_hd_event(uint16_t event) {
  switch (event) {
    CASE_RETURN_STR(BTA_HD_ENABLE_EVT)
    CASE_RETURN_STR(BTA_HD_DISABLE_EVT)
    CASE_RETURN_STR(BTA_HD_REGISTER_APP_EVT)
    CASE_RETURN_STR(BTA_HD_UNREGISTER_APP_EVT)
    CASE_RETURN_STR(BTA_HD_OPEN_EVT)
    CASE_RETURN_STR(BTA_HD_CLOSE_EVT)
    CASE_RETURN_STR(BTA_HD_GET_REPORT_EVT)
    CASE_RETURN_STR(BTA_HD_SET_REPORT_EVT)
    CASE_RETURN_STR(BTA_HD_SET_PROTOCOL_EVT)
    CASE_RETURN_STR(BTA_HD_INTR_DATA_EVT)
    CASE_RETURN_STR(BTA_HD_VC_UNPLUG_EVT)
    //CASE_RETURN_STR(BTA_HD_CONN_STATE_EVT)
    CASE_RETURN_STR(BTA_HD_API_ERR_EVT)
    default:
      return "UNKNOWN MSG ID";
  }
}
#endif ///BTA_HD_INCLUDED

UINT32 devclass2uint(DEV_CLASS dev_class)
{
    UINT32 cod = 0;

    if (dev_class != NULL) {
        /* if COD is 0, irrespective of the device type set it to Unclassified device */
        cod = (dev_class[2]) | (dev_class[1] << 8) | (dev_class[0] << 16);
    }
    return cod;
}

void uint2devclass(UINT32 cod, DEV_CLASS dev_class)
{
    dev_class[2] = (UINT8)cod;
    dev_class[1] = (UINT8)(cod >> 8);
    dev_class[0] = (UINT8)(cod >> 16);
}

static const UINT8  base_uuid_be[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                                      0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};

void uuid128_be_to_esp_uuid(esp_bt_uuid_t *u, uint8_t* uuid128)
{
    if (memcmp(base_uuid_be+4, uuid128 + 4, 12) != 0) {
        u->len = ESP_UUID_LEN_128;
        uint8_t *p_i = uuid128 + ESP_UUID_LEN_128 - 1;
        uint8_t *p_o = u->uuid.uuid128;
        uint8_t *p_end = p_o + ESP_UUID_LEN_128;
        for (; p_o != p_end; *p_o++ = *p_i--)
            ;
    } else if (uuid128[0] == 0 && uuid128[1] == 0) {
        u->len = 2;
        u->uuid.uuid16 = (uuid128[2] << 8) + uuid128[3];
    } else {
        u->len = 4;
        u->uuid.uuid32 = (uuid128[2] << 8) + uuid128[3];
        u->uuid.uuid32 += (uuid128[0] << 24) + (uuid128[1] << 16);
    }

    return;
}

void uuid_to_string_legacy(bt_uuid_t *p_uuid, char *str)
{
    uint32_t uuid0, uuid4;
    uint16_t uuid1, uuid2, uuid3, uuid5;

    memcpy(&uuid0, &(p_uuid->uu[0]), 4);
    memcpy(&uuid1, &(p_uuid->uu[4]), 2);
    memcpy(&uuid2, &(p_uuid->uu[6]), 2);
    memcpy(&uuid3, &(p_uuid->uu[8]), 2);
    memcpy(&uuid4, &(p_uuid->uu[10]), 4);
    memcpy(&uuid5, &(p_uuid->uu[14]), 2);

    sprintf((char *)str, "%.8x-%.4x-%.4x-%.4x-%.8x%.4x",
            ntohl(uuid0), ntohs(uuid1),
            ntohs(uuid2), ntohs(uuid3),
            ntohl(uuid4), ntohs(uuid5));
    return;
}

esp_bt_status_t btc_hci_to_esp_status(uint8_t hci_status)
{
    esp_bt_status_t esp_status = ESP_BT_STATUS_FAIL;
    switch(hci_status) {
        case HCI_SUCCESS:
            esp_status = ESP_BT_STATUS_SUCCESS;
            break;
        case HCI_ERR_HOST_TIMEOUT:
            esp_status = ESP_BT_STATUS_TIMEOUT;
            break;
        case HCI_ERR_ILLEGAL_COMMAND:
            esp_status = ESP_BT_STATUS_PENDING;
            break;
        case HCI_ERR_UNACCEPT_CONN_INTERVAL:
            esp_status = ESP_BT_STATUS_UNACCEPT_CONN_INTERVAL;
            break;
        case HCI_ERR_PARAM_OUT_OF_RANGE:
            esp_status = ESP_BT_STATUS_PARAM_OUT_OF_RANGE;
            break;
        case HCI_ERR_ILLEGAL_PARAMETER_FMT:
            esp_status = ESP_BT_STATUS_ERR_ILLEGAL_PARAMETER_FMT;
            break;
        case HCI_ERR_UNSUPPORTED_VALUE:
            esp_status = ESP_BT_STATUS_UNSUPPORTED;
            break;
        default:
            esp_status = hci_status | ESP_BT_STATUS_BASE_FOR_HCI_ERR;
            break;
    }

    return esp_status;
}

esp_bt_status_t btc_btm_status_to_esp_status (uint8_t btm_status)
{
    esp_bt_status_t esp_status = ESP_BT_STATUS_FAIL;
    switch(btm_status){
        case BTM_SUCCESS:
            esp_status = ESP_BT_STATUS_SUCCESS;
            break;
        case BTM_BUSY:
            esp_status = ESP_BT_STATUS_BUSY;
            break;
        case BTM_NO_RESOURCES:
            esp_status = ESP_BT_STATUS_NOMEM;
            break;
        case BTM_ILLEGAL_VALUE:
            esp_status = ESP_BT_STATUS_PARM_INVALID;
            break;
        case BTM_ERR_PROCESSING:
            esp_status = ESP_BT_STATUS_PENDING;
            break;
        case BTM_PEER_LE_DATA_LEN_UNSUPPORTED:
            esp_status = ESP_BT_STATUS_PEER_LE_DATA_LEN_UNSUPPORTED;
            break;
        case BTM_CONTROL_LE_DATA_LEN_UNSUPPORTED:
            esp_status = ESP_BT_STATUS_CONTROL_LE_DATA_LEN_UNSUPPORTED;
            break;
        case BTM_SET_PRIVACY_SUCCESS:
            esp_status = ESP_BT_STATUS_SUCCESS;
            break;
        case BTM_SET_PRIVACY_FAIL:
            esp_status = ESP_BT_STATUS_FAIL;
            break;
        default:
            esp_status = ESP_BT_STATUS_FAIL;
            break;
    }

    return esp_status;
}

esp_bt_status_t btc_bta_status_to_esp_status (uint8_t bta_status)
{
    esp_bt_status_t esp_status = ESP_BT_STATUS_FAIL;
    switch(bta_status){
        case BTA_SUCCESS:
            esp_status = ESP_BT_STATUS_SUCCESS;
            break;
        case BTA_FAILURE:
            esp_status = ESP_BT_STATUS_FAIL;
            break;
        case BTA_PENDING:
            esp_status = ESP_BT_STATUS_PENDING;
            break;
        case BTA_BUSY:
            esp_status = ESP_BT_STATUS_BUSY;
            break;
        case BTA_NO_RESOURCES:
            esp_status = ESP_BT_STATUS_NOMEM;
            break;
        case BTA_WRONG_MODE:
            esp_status = ESP_BT_STATUS_NOT_READY;
            break;
        case BTA_EIR_TOO_LARGE:
            esp_status = ESP_BT_STATUS_EIR_TOO_LARGE;
            break;
        default:
            esp_status = ESP_BT_STATUS_FAIL;
            break;
    }

    return esp_status;
}
