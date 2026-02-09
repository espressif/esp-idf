/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This is the private interface file for the BTA device manager.
 *
 ******************************************************************************/
#ifndef BTA_DM_INT_H
#define BTA_DM_INT_H

#include "common/bt_target.h"
#include "freertos/semphr.h"
#include "bta/bta_sys.h"
#if (BLE_INCLUDED == TRUE && (defined BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE))
#include "bta/bta_gatt_api.h"
#endif



/*****************************************************************************
**  Constants and data types
*****************************************************************************/


#define BTA_COPY_DEVICE_CLASS(coddst, codsrc)   {((UINT8 *)(coddst))[0] = ((UINT8 *)(codsrc))[0]; \
                                                 ((UINT8 *)(coddst))[1] = ((UINT8 *)(codsrc))[1];  \
                                                 ((UINT8 *)(coddst))[2] = ((UINT8 *)(codsrc))[2];}


#define BTA_DM_MSG_LEN 50

#define BTA_SERVICE_ID_TO_SERVICE_MASK(id)       (1 << (id))

/* DM events */
enum {
    /* device manager local device API events */
    BTA_DM_API_ENABLE_EVT = BTA_SYS_EVT_START(BTA_ID_DM),
    BTA_DM_API_DISABLE_EVT,
    BTA_DM_API_SET_NAME_EVT,
    BTA_DM_API_GET_NAME_EVT,
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    BTA_DM_API_CFG_COEX_ST_EVT,
#endif
#if (BLE_VENDOR_HCI_EN == TRUE)
    BTA_DM_API_SEND_VENDOR_HCI_CMD_EVT,
    BTA_DM_API_BLE_CLEAR_ADV_EVT,
    BTA_DM_API_BLE_SET_CSA_SUPPORT_EVT,
    BTA_DM_API_BLE_SET_VENDOR_EVT_MASK_EVT,
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)
#if (CLASSIC_BT_INCLUDED == TRUE)
    BTA_DM_API_CONFIG_EIR_EVT,
    BTA_DM_API_PAGE_TO_SET_EVT,
    BTA_DM_API_PAGE_TO_GET_EVT,
    BTA_DM_API_SET_ACL_PKT_TYPES_EVT,
#if (ENC_KEY_SIZE_CTRL_MODE != ENC_KEY_SIZE_CTRL_MODE_NONE)
    BTA_DM_API_SET_MIN_ENC_KEY_SIZE_EVT,
#endif
    BTA_DM_API_SET_AFH_CHANNELS_EVT,
#if (SDP_INCLUDED == TRUE)
    BTA_DM_API_GET_REMOTE_NAME_EVT,
#endif
    BTA_DM_API_SET_VISIBILITY_EVT,
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

    BTA_DM_ACL_CHANGE_EVT,
    BTA_DM_API_ADD_DEVICE_EVT,
#if (SMP_INCLUDED == TRUE)
    /* security API events */
    BTA_DM_API_BOND_EVT,
    BTA_DM_API_BOND_CANCEL_EVT,
    BTA_DM_API_SET_PIN_TYPE_EVT,
    BTA_DM_API_PIN_REPLY_EVT,
#endif  ///SMP_INCLUDED == TRUE
#if (BTA_DM_PM_INCLUDED == TRUE)
    /* power manager events */
    BTA_DM_PM_BTM_STATUS_EVT,
    BTA_DM_PM_TIMER_EVT,
#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */
#if (BTA_DM_QOS_INCLUDED == TRUE)
    /* Quality of Service set events */
    BTA_DM_API_QOS_SET_EVT,
#endif /* #if (BTA_DM_QOS_INCLUDED == TRUE) */
#if (SMP_INCLUDED == TRUE)
#if (CLASSIC_BT_INCLUDED == TRUE)
    /* simple pairing events */
    BTA_DM_API_CONFIRM_EVT,
    BTA_DM_API_KEY_REQ_EVT,
#endif /* (CLASSIC_BT_INCLUDED == TRUE) */
    BTA_DM_API_SET_ENCRYPTION_EVT,
#endif /* (SMP_INCLUDED == TRUE) */
#if (BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
    BTA_DM_API_LOC_OOB_EVT,
    BTA_DM_API_OOB_REPLY_EVT,
    BTA_DM_API_SC_OOB_REPLY_EVT,
    BTA_DM_API_SC_CR_OOB_DATA_EVT,
    BTA_DM_CI_IO_REQ_EVT,
    BTA_DM_CI_RMT_OOB_EVT,
#endif /* BTM_OOB_INCLUDED */


#if BLE_INCLUDED == TRUE
#if SMP_INCLUDED == TRUE
    BTA_DM_API_ADD_BLEKEY_EVT,
    BTA_DM_API_ADD_BLEDEVICE_EVT,
    BTA_DM_API_BLE_PASSKEY_REPLY_EVT,
    BTA_DM_API_BLE_SET_STATIC_PASSKEY_EVT,
    BTA_DM_API_BLE_CONFIRM_REPLY_EVT,
    BTA_DM_API_BLE_SEC_GRANT_EVT,
#endif  ///SMP_INCLUDED == TRUE
#if (BLE_GATT_BGCONN == TRUE)
    BTA_DM_API_BLE_SET_BG_CONN_TYPE,
#endif // (BLE_GATT_BGCONN == TRUE)
    BTA_DM_API_BLE_CONN_PARAM_EVT,
    /*******This event added by Yulong at 2016/10/25 to
    support the scan filter setting for the APP******/
#if (BLE_42_SCAN_EN == TRUE)
    BTA_DM_API_BLE_SCAN_FIL_PARAM_EVT,
#endif // #if (BLE_42_SCAN_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    BTA_DM_API_BLE_SCAN_EVT,
#endif // #if (BLE_42_SCAN_EN == TRUE)
    BTA_DM_API_UPDATE_CONN_PARAM_EVT,
    /*******This event added by Yulong at 2016/9/9 to
    support the random address setting for the APP******/
    BTA_DM_API_SET_RAND_ADDR_EVT,
    BTA_DM_API_CLEAR_RAND_ADDR_EVT,
#if BLE_PRIVACY_SPT == TRUE
    BTA_DM_API_LOCAL_PRIVACY_EVT,
#endif
    BTA_DM_API_LOCAL_ICON_EVT,
#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
    BTA_DM_API_KEY_MATERIAL_EVT,
#endif

    /*******This event added by Yulong at 2016/10/20 to
    support setting the ble advertising param by the APP******/
#if (BLE_42_ADV_EN == TRUE)
    BTA_DM_API_BLE_ADV_PARAM_All_EVT,
    BTA_DM_API_BLE_SET_ADV_CONFIG_EVT,
    /* Add for set raw advertising data */
    BTA_DM_API_BLE_SET_ADV_CONFIG_RAW_EVT,
    BTA_DM_API_BLE_SET_SCAN_RSP_EVT,
    /* Add for set raw scan response data */
    BTA_DM_API_BLE_SET_SCAN_RSP_RAW_EVT,
    BTA_DM_API_BLE_ADVSTOP_EVT,
#endif // #if (BLE_42_ADV_EN == TRUE)
    BTA_DM_API_SET_DATA_LENGTH_EVT,

    BTA_DM_API_BLE_DISCONNECT_EVT,

#endif
    BTA_DM_API_REMOVE_DEVICE_EVT,
    BTA_DM_API_BLE_SET_CHANNELS_EVT,
    BTA_DM_API_UPDATE_WHITE_LIST_EVT,
    BTA_DM_API_CLEAR_WHITE_LIST_EVT,
    BTA_DM_API_READ_RSSI_EVT,
#if BLE_INCLUDED == TRUE
#if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
    BTA_DM_API_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_EVT,
#endif // ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
#endif
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    BTA_DM_API_READ_PHY_EVT,
    BTA_DM_API_SET_PER_DEF_PHY_EVT,
    BTA_DM_API_SET_PER_PHY_EVT,
#if (BLE_50_EXTEND_ADV_EN == TRUE)
    BTA_DM_API_SET_EXT_ADV_RAND_ADDR_EVT,
    BTA_DM_API_SET_EXT_ADV_PARAMS_EVT,
    BTA_DM_API_CFG_ADV_DATA_RAW_EVT,
    BTA_DM_API_EXT_ADV_ENABLE_EVT,
    BTA_DM_API_EXT_ADV_SET_REMOVE_EVT,
    BTA_DM_API_EXT_ADV_SET_CLEAR_EVT,
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)
#if (BLE_50_PERIODIC_ADV_EN == TRUE)
    BTA_DM_API_PERIODIC_ADV_SET_PARAMS_EVT,
    BTA_DM_API_PERIODIC_ADV_CFG_DATA_EVT,
    BTA_DM_API_PERIODIC_ADV_ENABLE_EVT,
#endif // #if (BLE_50_PERIODIC_ADV_EN == TRUE)
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
    BTA_DM_API_PERIODIC_ADV_SYNC_EVT,
    BTA_DM_API_PERIODIC_ADV_SYNC_CANCEL_EVT,
    BTA_DM_API_PERIODIC_ADV_SYNC_TERMINATE_EVT,
    BTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LSIT_EVT,
    BTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LSIT_EVT,
    BTA_DM_API_PERIODIC_ADV_CLEAR_DEV_EVT,
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
    BTA_DM_API_SET_EXT_SCAN_PARAMS_EVT,
    BTA_DM_API_START_EXT_SCAN_EVT,
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
    BTA_DM_API_SET_PERF_EXT_CONN_PARAMS_EVT,
    BTA_DM_API_EXT_CONN_EVT,
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_50_DTM_TEST_EN == TRUE)
    BTA_DM_API_DTM_ENH_TX_START_EVT,
    BTA_DM_API_DTM_ENH_RX_START_EVT,
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    BTA_DM_API_PERIODIC_ADV_RECV_ENABLE_EVT,
    BTA_DM_API_PERIODIC_ADV_SYNC_TRANS_EVT,
    BTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS_EVT,
    BTA_DM_API_SET_PERIODIC_ADV_SYNC_TRANS_PARAMS_EVT,
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#if BLE_INCLUDED == TRUE
#if (BLE_42_DTM_TEST_EN == TRUE)
    BTA_DM_API_DTM_TX_START_EVT,
    BTA_DM_API_DTM_RX_START_EVT,
#endif // #if (BLE_42_DTM_TEST_EN == TRUE)
#if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))
    BTA_DM_API_DTM_STOP_EVT,
#endif // #if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))
    BTA_DM_API_SET_RPA_TIMEOUT_EVT,
    BTA_DM_API_ADD_DEV_TO_RESOLVING_LIST_EVT,
    BTA_DM_API_SET_PRIVACY_MODE_EVT,
    BTA_DM_API_BLE_READ_CH_MAP_EVT,
#endif
#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
    BTA_DM_API_ISO_BIG_CREATE_EVT,
    BTA_DM_API_ISO_BIG_CREATE_TEST_EVT,
    BTA_DM_API_ISO_BIG_TERMINATE_EVT,
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    BTA_DM_API_ISO_BIG_SYNC_CREATE_EVT,
    BTA_DM_API_ISO_BIG_SYNC_TERMINATE_EVT,
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    BTA_DM_API_ISO_SET_DATA_PATH_EVT,
    BTA_DM_API_ISO_REMOVE_DATA_PATH_EVT,
    BTA_DM_API_ISO_READ_TX_SYNC_EVT,
    BTA_DM_API_ISO_READ_LINK_QUALITY_EVT,
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
    BTA_DM_API_SET_CIG_PARAMS_EVT,
    BTA_DM_API_SET_CIG_PARAMS_TEST_EVT,
    BTA_DM_API_CREATE_CIS_EVT,
    BTA_DM_API_REMOVE_CIG_PARAMS_EVT,
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
    BTA_DM_API_ACCEPT_CIS_REQ_PARAMS_EVT,
    BTA_DM_API_REJECT_CIS_REQ_PARAMS_EVT,
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
    BTA_DM_API_DISCON_CIS_EVT,
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
#endif // #if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    BTA_DM_API_CTE_SET_TRANS_PARAMS,
    BTA_DM_API_CTE_SET_TRANS_ENABLE,
    BTA_DM_API_CTE_SET_IQ_SAMPLING_EN,
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    BTA_DM_API_CTE_SET_CONN_CTE_RECV_PARAMS,
    BTA_DM_API_CTE_SET_CONN_CTE_TRANS_PARAMS,
    BTA_DM_API_CTE_SET_CONN_CTE_REQUEST_EN,
    BTA_DM_API_CTE_SET_CONN_CTE_RESPONSE_EN,
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    BTA_DM_API_CTE_READ_ANTENNA_INFOR,
#endif // #if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
    BTA_DM_API_ENH_READ_TRANS_POWER_LEVEL,
    BTA_DM_API_READ_REM_TRANS_POWER_LEVEL,
    BTA_DM_API_SET_PATH_LOSS_REPORT_PARAMS,
    BTA_DM_API_SET_PATH_LOSS_REPORTING_EN,
    BTA_DM_API_SET_TRANS_POWER_REPORTING_EN,
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BLE_FEAT_CONN_SUBRATING == TRUE)
    BTA_DM_API_SET_DEFALT_SUBRATE,
    BTA_DM_API_SUBRATE_REQUEST,
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    BTA_DM_API_SET_HOST_FEATURE_EVT,
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    BTA_DM_API_SET_PA_SUBEVT_DATA,
    BTA_DM_API_SET_PA_RSP_DATA,
    BTA_DM_API_SET_PA_SYNC_SUBEVT,
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
    BTA_DM_API_CS_READ_LOCAL_SUPPORTED_CAPS,
    BTA_DM_API_CS_READ_REMOTE_SUPPORTED_CAPS,
    BTA_DM_API_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPS,
    BTA_DM_API_CS_SECURITY_ENABLE,
    BTA_DM_API_CS_SET_DEFAULT_SETTINGS,
    BTA_DM_API_CS_READ_REMOTE_FAE_TABLE,
    BTA_DM_API_CS_WRITE_CACHED_REMOTE_FAE_TABLE,
    BTA_DM_API_CS_CREATE_CONFIG,
    BTA_DM_API_CS_REMOVE_CONFIG,
    BTA_DM_API_CS_SET_CAHNNEL_CLASSIFICATION,
    BTA_DM_API_CS_SET_PROCEDURE_PARAMS,
    BTA_DM_API_CS_PROCEDURE_ENABLE,
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
    BTA_DM_MAX_EVT
};


/* DM search events */
enum {
    /* DM search API events */
    BTA_DM_API_SEARCH_EVT = BTA_SYS_EVT_START(BTA_ID_DM_SEARCH),
    BTA_DM_API_SEARCH_CANCEL_EVT,
    BTA_DM_API_DISCOVER_EVT,
    BTA_DM_INQUIRY_CMPL_EVT,
    BTA_DM_REMT_NAME_EVT,
    BTA_DM_SDP_RESULT_EVT,
    BTA_DM_SEARCH_CMPL_EVT,
    BTA_DM_DISCOVERY_RESULT_EVT,
    BTA_DM_API_DI_DISCOVER_EVT,
    BTA_DM_DISC_CLOSE_TOUT_EVT,
};

/* data type for BTA_DM_API_ENABLE_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_DM_SEC_CBACK *p_sec_cback;
} tBTA_DM_API_ENABLE;

/* data type for BTA_DM_API_SET_NAME_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_NAME             name; /* max 248 bytes name, plus must be Null terminated */
    tBT_DEVICE_TYPE     name_type; /* name for BLE, name for BT or name for BTDM */
} tBTA_DM_API_SET_NAME;

typedef struct {
    BT_HDR                  hdr;
    tBTA_GET_DEV_NAME_CBACK *p_cback;
    tBT_DEVICE_TYPE         name_type;
} tBTA_DM_API_GET_NAME;

#if (ESP_COEX_VSC_INCLUDED == TRUE)
typedef struct {
    BT_HDR hdr;
    UINT8  op;
    UINT8  type;
    UINT8  status;
} tBTA_DM_API_CFG_COEX_STATUS;
#endif

typedef struct {
    BT_HDR    hdr;
    UINT16    opcode;
    UINT8    param_len;
    UINT8     *p_param_buf;
    tBTA_SEND_VENDOR_HCI_CMPL_CBACK *vendor_hci_cb;
}tBTA_DM_API_SEND_VENDOR_HCI_CMD;

/* data type for BTA_DM_API_CONFIG_EIR_EVT */
typedef struct {
    BT_HDR              hdr;
    BOOLEAN             eir_fec_required;
    BOOLEAN             eir_included_name;
    BOOLEAN             eir_included_tx_power;
    BOOLEAN             eir_included_uuid;
    UINT8               eir_flags;
    UINT8               eir_manufac_spec_len;
    UINT8               *eir_manufac_spec;
    UINT8               eir_url_len;
    UINT8               *eir_url;
    UINT8               data[];
}tBTA_DM_API_CONFIG_EIR;

/* data type for BTA_DM_API_SET_AFH_CHANNELS_EVT */
typedef struct {
    BT_HDR              hdr;
    AFH_CHANNELS        channels;
    tBTA_CMPL_CB        *set_afh_cb;
}tBTA_DM_API_SET_AFH_CHANNELS;

/* data type for BTA_DM_API_PAGE_TO_SET_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT16              page_to;
    tBTM_CMPL_CB        *set_page_to_cb;
} tBTA_DM_API_PAGE_TO_SET;

/* data type for BTA_DM_API_PAGE_TO_GET_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTM_CMPL_CB        *get_page_to_cb;
} tBTA_DM_API_PAGE_TO_GET;

/* data type for BTA_DM_API_SET_ACL_PKT_TYPES_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             rmt_addr;
    UINT16              pkt_types;
    tBTM_CMPL_CB        *set_acl_pkt_types_cb;
} tBTA_DM_API_SET_ACL_PKT_TYPES;

#if (ENC_KEY_SIZE_CTRL_MODE != ENC_KEY_SIZE_CTRL_MODE_NONE)
/* data type for BTA_DM_API_SET_MIN_ENC_KEY_SIZE_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT8               key_size;
    tBTM_CMPL_CB        *set_min_enc_key_size_cb;
} tBTA_DM_API_SET_MIN_ENC_KEY_SIZE;
#endif

/* data type for BTA_DM_API_GET_REMOTE_NAME_EVT */
typedef struct {
    BT_HDR         hdr;
    BD_ADDR        rmt_addr;
    BD_NAME        rmt_name;
    tBTA_TRANSPORT transport;
    tBTA_CMPL_CB   *rmt_name_cb;
} tBTA_DM_API_GET_REMOTE_NAME;

#if (BLE_INCLUDED == TRUE)
/* data type for BTA_DM_API_BLE_SET_CHANNELS_EVT */
typedef struct {
    BT_HDR              hdr;
    AFH_CHANNELS        channels;
    tBTA_CMPL_CB        *set_channels_cb;
}tBTA_DM_API_BLE_SET_CHANNELS;

typedef struct {
    BT_HDR    hdr;
    BOOLEAN   add_remove;
    BD_ADDR   remote_addr;
    tBLE_ADDR_TYPE addr_type;
    tBTA_UPDATE_WHITELIST_CBACK *update_wl_cb;
}tBTA_DM_API_UPDATE_WHITE_LIST;

#if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
typedef struct {
    BT_HDR    hdr;
    UINT8     subcode;
    UINT32    type;
    BD_ADDR   device_info;
    tBTA_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_CMPL_CBACK *exceptional_list_cb;
}tBTA_DM_API_UPDATE_DUPLICATE_EXCEPTIONAL_LIST;
#endif // ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))

#endif  ///BLE_INCLUDED == TRUE

typedef struct {
    BT_HDR        hdr;
    BD_ADDR       remote_addr;
    tBTA_TRANSPORT  transport;
    tBTA_CMPL_CB  *read_rssi_cb;
}tBTA_DM_API_READ_RSSI;

typedef struct {
    BT_HDR            hdr;
    BD_ADDR          remote_addr;
    tBTA_CMPL_CB    *read_ch_map_cb;
} tBTA_DM_API_READ_CH_MAP;

/* data type for BTA_DM_API_SET_VISIBILITY_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_DM_DISC    disc_mode;
    tBTA_DM_CONN    conn_mode;
    UINT8           pair_mode;
    UINT8           conn_paired_only;
} tBTA_DM_API_SET_VISIBILITY;

enum {
    BTA_DM_RS_NONE,     /* straight API call */
    BTA_DM_RS_OK,       /* the role switch result - successful */
    BTA_DM_RS_FAIL      /* the role switch result - failed */
};
typedef UINT8 tBTA_DM_RS_RES;

/* data type for BTA_DM_API_SEARCH_EVT */
typedef struct {
    BT_HDR      hdr;
    tBTA_DM_INQ inq_params;
    tBTA_SERVICE_MASK services;
    tBTA_DM_SEARCH_CBACK *p_cback;
    tBTA_DM_RS_RES  rs_res;
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
    UINT8           num_uuid;
    tBT_UUID        *p_uuid;
#endif
} tBTA_DM_API_SEARCH;

#if (SDP_INCLUDED == TRUE)
/* data type for BTA_DM_API_DISCOVER_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR bd_addr;
    tBTA_SERVICE_MASK services;
    tBTA_DM_SEARCH_CBACK *p_cback;
    BOOLEAN         sdp_search;
    tBTA_TRANSPORT  transport;
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
    UINT8           num_uuid;
    tBT_UUID        *p_uuid;
#endif
    tSDP_UUID    uuid;
} tBTA_DM_API_DISCOVER;
#endif  ///SDP_INCLUDED == TRUE

/* data type for BTA_DM_API_DI_DISC_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
#if (SDP_INCLUDED == TRUE)
    tBTA_DISCOVERY_DB   *p_sdp_db;
#endif  ///SDP_INCLUDED == TRUE
    UINT32              len;
    tBTA_DM_SEARCH_CBACK *p_cback;
} tBTA_DM_API_DI_DISC;

/* data type for BTA_DM_API_BOND_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR bd_addr;
    tBTA_TRANSPORT transport;
} tBTA_DM_API_BOND;

/* data type for BTA_DM_API_BOND_CANCEL_EVT */
typedef struct {
    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    tBTA_TRANSPORT  transport;
} tBTA_DM_API_BOND_CANCEL;

/* data type for BTA_DM_API_SET_PIN_TYPE_EVT */
typedef struct {
    BT_HDR      hdr;
    UINT8       pin_type;
    UINT8       pin_len;
    UINT8       p_pin[PIN_CODE_LEN];
} tBTA_DM_API_SET_PIN_TYPE;

/* data type for BTA_DM_API_PIN_REPLY_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR bd_addr;
    BOOLEAN accept;
    UINT8 pin_len;
    UINT8 p_pin[PIN_CODE_LEN];
} tBTA_DM_API_PIN_REPLY;

/* data type for BTA_DM_API_LOC_OOB_EVT */
typedef struct {
    BT_HDR      hdr;
} tBTA_DM_API_LOC_OOB;

/* data type for BTA_DM_API_OOB_REPLY_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR bd_addr;
    UINT8       len;
    UINT8       value[BT_OCTET16_LEN];
    UINT8       c[BT_OCTET16_LEN];
    UINT8       r[BT_OCTET16_LEN];
} tBTA_DM_API_OOB_REPLY;

/* data type for BTA_DM_API_SC_OOB_REPLY_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR     bd_addr;
    UINT8       c[BT_OCTET16_LEN];
    UINT8       r[BT_OCTET16_LEN];
} tBTA_DM_API_SC_OOB_REPLY;

/* data type for BTA_DM_API_SC_CR_OOB_DATA_EVT */
typedef struct {
    BT_HDR      hdr;
} tBTA_DM_API_SC_CR_OOB_DATA;

/* data type for BTA_DM_API_CONFIRM_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR     bd_addr;
    BOOLEAN     accept;
} tBTA_DM_API_CONFIRM;

/* data type for BTA_DM_API_KEY_REQ_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR     bd_addr;
    BOOLEAN     accept;
    UINT32      passkey;
} tBTA_DM_API_KEY_REQ;

/* data type for BTA_DM_CI_IO_REQ_EVT */
typedef struct {
    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    tBTA_IO_CAP     io_cap;
    tBTA_OOB_DATA   oob_data;
    tBTA_AUTH_REQ   auth_req;
} tBTA_DM_CI_IO_REQ;

/* data type for BTA_DM_CI_RMT_OOB_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR     bd_addr;
    BT_OCTET16  c;
    BT_OCTET16  r;
    BOOLEAN     accept;
} tBTA_DM_CI_RMT_OOB;

/* data type for BTA_DM_REMT_NAME_EVT */
typedef struct {
    BT_HDR      hdr;
    tBTA_DM_SEARCH  result;
} tBTA_DM_REM_NAME;

/* data type for tBTA_DM_DISC_RESULT */
typedef struct {
    BT_HDR      hdr;
    tBTA_DM_SEARCH  result;
} tBTA_DM_DISC_RESULT;


/* data type for BTA_DM_INQUIRY_CMPL_EVT */
typedef struct {
    BT_HDR      hdr;
    UINT8       num;
} tBTA_DM_INQUIRY_CMPL;

/* data type for BTA_DM_SDP_RESULT_EVT */
typedef struct {
    BT_HDR      hdr;
    UINT16 sdp_result;
} tBTA_DM_SDP_RESULT;

/* data type for BTA_DM_ACL_CHANGE_EVT */
typedef struct {
    BT_HDR          hdr;
    tBTM_BL_EVENT   event;
    UINT8           busy_level;
    UINT8           busy_level_flags;
    BOOLEAN         is_new;
    UINT8           new_role;
    BD_ADDR         bd_addr;
    UINT8           hci_status;
    BOOLEAN         sc_downgrade;
#if BLE_INCLUDED == TRUE
    UINT16          handle;
#endif
    tBT_TRANSPORT   transport;
} tBTA_DM_ACL_CHANGE;

#if (BTA_DM_PM_INCLUDED == TRUE)
/* data type for BTA_DM_PM_BTM_STATUS_EVT */
typedef struct {

    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    tBTM_PM_STATUS  status;
    UINT16          value;
    UINT8           hci_status;

} tBTA_DM_PM_BTM_STATUS;

/* data type for BTA_DM_PM_TIMER_EVT */
typedef struct {
    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    tBTA_DM_PM_ACTION  pm_request;
} tBTA_DM_PM_TIMER;
#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */

#if (BTA_DM_QOS_INCLUDED == TRUE)
/* data type for BTA_DM_API_QOS_SET_EVT */
typedef struct {
    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    UINT32          t_poll;
    tBTM_CMPL_CB    *p_cb;
} tBTA_DM_API_QOS_SET;
#endif /* #if (BTA_DM_QOS_INCLUDED == TRUE) */

/* data type for BTA_DM_API_ADD_DEVICE_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    DEV_CLASS           dc;
    LINK_KEY            link_key;
    tBTA_SERVICE_MASK   tm;
    BOOLEAN             is_trusted;
    UINT8               key_type;
    tBTA_IO_CAP         io_cap;
    BOOLEAN             link_key_known;
    BOOLEAN             dc_known;
    BD_NAME             bd_name;
    UINT8               features[BTA_FEATURE_BYTES_PER_PAGE * (BTA_EXT_FEATURES_PAGE_MAX + 1)];
    UINT8               pin_length;
    UINT8               sc_support;
} tBTA_DM_API_ADD_DEVICE;

/* data type for BTA_DM_API_REMOVE_ACL_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    UINT8               transport;
} tBTA_DM_API_REMOVE_DEVICE;

/* data type for tBTA_DM_API_SET_ENCRYPTION */
typedef struct {
    BT_HDR                    hdr;
    tBTA_TRANSPORT            transport;
    tBTA_DM_ENCRYPT_CBACK     *p_callback;
    tBTA_DM_BLE_SEC_ACT       sec_act;
    BD_ADDR                   bd_addr;
} tBTA_DM_API_SET_ENCRYPTION;

#if BLE_INCLUDED == TRUE
typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 bd_addr;
    tBTA_LE_KEY_VALUE       blekey;
    tBTA_LE_KEY_TYPE        key_type;

} tBTA_DM_API_ADD_BLEKEY;

typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 bd_addr;
    tBT_DEVICE_TYPE         dev_type ;
    UINT32                  auth_mode;
    tBLE_ADDR_TYPE          addr_type;

} tBTA_DM_API_ADD_BLE_DEVICE;

typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 bd_addr;
    BOOLEAN                 accept;
    UINT32                  passkey;
} tBTA_DM_API_PASSKEY_REPLY;

typedef struct {
    BT_HDR                  hdr;
    BOOLEAN                 add;
    UINT32                  static_passkey;
} tBTA_DM_API_SET_DEFAULT_PASSKEY;

typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 bd_addr;
    tBTA_DM_BLE_SEC_GRANT   res;
} tBTA_DM_API_BLE_SEC_GRANT;


typedef struct {
    BT_HDR                  hdr;
    tBTA_DM_BLE_CONN_TYPE   bg_conn_type;
    tBTA_DM_BLE_SEL_CBACK   *p_select_cback;
} tBTA_DM_API_BLE_SET_BG_CONN_TYPE;

/* set preferred BLE connection parameters for a device */
typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 peer_bda;
    UINT16                  conn_int_min;
    UINT16                  conn_int_max;
    UINT16                  supervision_tout;
    UINT16                  slave_latency;

} tBTA_DM_API_BLE_CONN_PARAMS;

typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 peer_bda;
    BOOLEAN                 privacy_enable;

} tBTA_DM_API_ENABLE_PRIVACY;

typedef struct {
    BT_HDR                  hdr;
    BOOLEAN                 privacy_enable;
    tBTA_SET_LOCAL_PRIVACY_CBACK *set_local_privacy_cback;
} tBTA_DM_API_LOCAL_PRIVACY;

typedef struct {
    BT_HDR                  hdr;
    uint16_t                icon;
} tBTA_DM_API_LOCAL_ICON;

#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
typedef struct {
    BT_HDR                  hdr;
    uint8_t                 session_key[16];
    uint8_t                 iv[8];
} tBTA_DM_API_KEY_MATERIAL;
#endif

/* set scan parameter for BLE connections */
typedef struct {
    BT_HDR hdr;
    tBTA_GATTC_IF client_if;
    UINT32 scan_int;
    UINT32 scan_window;
    tBLE_SCAN_MODE scan_mode;
    tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback;
} tBTA_DM_API_BLE_SCAN_PARAMS;

typedef struct {
    BT_HDR hdr;
    tBTA_GATTC_IF client_if;
    UINT32 scan_int;
    UINT32 scan_window;
    tBLE_SCAN_MODE scan_mode;
    UINT8 addr_type_own;
    UINT8 scan_duplicate_filter;
    UINT8 scan_filter_policy;
    tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback;
} tBTA_DM_API_BLE_SCAN_FILTER_PARAMS;

/* Data type for start/stop observe */
typedef struct {
    BT_HDR                  hdr;
    BOOLEAN                 start;
    UINT32                  duration;
    tBTA_DM_SEARCH_CBACK    *p_cback;
    tBTA_START_STOP_SCAN_CMPL_CBACK *p_start_scan_cback;
    tBTA_START_STOP_SCAN_CMPL_CBACK *p_stop_scan_cback;
    tBTA_START_STOP_ADV_CMPL_CBACK  *p_stop_adv_cback;
} tBTA_DM_API_BLE_OBSERVE;

/* Data type for start/stop scan */
typedef struct {
    BT_HDR                  hdr;
    BOOLEAN                 start;
    UINT32                  duration;
    tBTA_DM_SEARCH_CBACK    *p_cback;
    tBTA_START_STOP_SCAN_CMPL_CBACK *p_start_scan_cback;
    tBTA_START_STOP_SCAN_CMPL_CBACK *p_stop_scan_cback;
    tBTA_START_STOP_ADV_CMPL_CBACK  *p_stop_adv_cback;
} tBTA_DM_API_BLE_SCAN;

typedef struct {
    BT_HDR      hdr;
    BD_ADDR     remote_bda;
    UINT16      tx_data_length;
    tBTA_SET_PKT_DATA_LENGTH_CBACK *p_set_pkt_data_cback;
} tBTA_DM_API_BLE_SET_DATA_LENGTH;

/* set the address for BLE device
   this type added by Yulong at 2016/9/9*/
typedef struct {
    BT_HDR      hdr;
    tBLE_ADDR_TYPE addr_type;
    BD_ADDR address;
    tBTA_SET_RAND_ADDR_CBACK *p_set_rand_addr_cback;
} tBTA_DM_APT_SET_DEV_ADDR;

typedef struct {
    BT_HDR      hdr;
} tBTA_DM_APT_CLEAR_ADDR;

typedef struct {
    BT_HDR      hdr;
    UINT16      rpa_timeout;
    tBTA_SET_RPA_TIMEOUT_CMPL_CBACK  *p_set_rpa_timeout_cback;
} tBTA_DM_API_SET_RPA_TIMEOUT;

typedef struct {
    BT_HDR      hdr;                                    // Event header
    esp_bd_addr_t addr;                                 // Bluetooth device address
    UINT8       addr_type;                              // Type of the address
    UINT8       irk[PEER_IRK_LEN];                     // Identity Resolving Key (IRK)
    tBTA_ADD_DEV_TO_RESOLVING_LIST_CMPL_CBACK *p_add_dev_to_resolving_list_callback; // Callback function pointer
} tBTA_DM_API_ADD_DEV_TO_RESOLVING_LIST;

/* set adv parameter for BLE advertising */
typedef struct {
    BT_HDR                  hdr;
    UINT16                  adv_int_min;
    UINT16                  adv_int_max;
    UINT8                   adv_type;
    tBLE_ADDR_TYPE          addr_type_own;
    tBTM_BLE_ADV_CHNL_MAP   channel_map;
    tBTM_BLE_AFP            adv_filter_policy;
    tBLE_BD_ADDR            *p_dir_bda;
    tBTA_START_ADV_CMPL_CBACK  *p_start_adv_cback;
} tBTA_DM_API_BLE_ADV_PARAMS_ALL;


typedef struct {
    BT_HDR                  hdr;
    BOOLEAN                 enable;

} tBTA_DM_API_BLE_FEATURE;

typedef struct {
    BT_HDR                  hdr;
    UINT32                  data_mask;
    tBTA_BLE_ADV_DATA       *p_adv_cfg;
    tBTA_SET_ADV_DATA_CMPL_CBACK    *p_adv_data_cback;
} tBTA_DM_API_SET_ADV_CONFIG;

/* raw scan response and raw advertising data use
   the same structure */
typedef struct {
    BT_HDR                  hdr;
    UINT8                   *p_raw_adv;
    UINT32                  raw_adv_len;
    tBTA_SET_ADV_DATA_CMPL_CBACK    *p_adv_data_cback;
} tBTA_DM_API_SET_ADV_CONFIG_RAW;

typedef struct {
    BT_HDR      hdr;
    BD_ADDR     remote_bda;
} tBTA_DM_API_BLE_DISCONNECT;

#if (BLE_42_DTM_TEST_EN == TRUE)
typedef struct {
    BT_HDR      hdr;
    UINT8      tx_channel;
    UINT8      len_of_data;
    UINT8      pkt_payload;
    tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback;
} tBTA_DM_API_BLE_DTM_TX_START;

typedef struct {
    BT_HDR      hdr;
    UINT8      rx_channel;
    tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback;
} tBTA_DM_API_BLE_DTM_RX_START;
#endif // #if (BLE_42_DTM_TEST_EN == TRUE)

typedef struct {
    BT_HDR      hdr;
    tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback;
} tBTA_DM_API_BLE_DTM_STOP;

typedef struct {
    BT_HDR                  hdr;
    tBTA_CLEAR_ADV_CMPL_CBACK       *p_clear_adv_cback;
} tBTA_DM_API_CLEAR_ADV;

typedef struct {
    BT_HDR                              hdr;
    tBLE_ADDR_TYPE                      addr_type;
    BD_ADDR                             addr;
    UINT8                               privacy_mode;
    tBTA_SET_PRIVACY_MODE_CMPL_CBACK    *p_cback;
} tBTA_DM_API_SET_PRIVACY_MODE;

typedef struct {
    BT_HDR                              hdr;
    UINT8                               csa_select;
    tBTA_SET_CSA_SUPPORT_CMPL_CBACK     *p_cback;
} tBTA_DM_API_BLE_SET_CSA_SUPPORT;

typedef struct {
    BT_HDR                              hdr;
    UINT32                              evt_mask;
    tBTA_SET_VENDOR_EVT_MASK_CBACK      *p_cback;
} tBTA_DM_API_BLE_SET_VENDOR_EVT_MASK;

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
typedef struct {
    BT_HDR                              hdr;
    UINT16                              conn_handle;
    UINT8                               phy;
} tBTA_DM_API_BLE_ENH_READ_TANS_PWR_LEVEL;

typedef struct {
    BT_HDR                              hdr;
    UINT16                              conn_handle;
    UINT8                               phy;
} tBTA_DM_API_BLE_READ_REMOTE_TANS_PWR_LEVEL;

typedef struct {
    BT_HDR                            hdr;
    UINT16                            conn_handle;
	UINT8                             high_threshold;
	UINT8                             high_hysteresis;
	UINT8                             low_threshold;
	UINT8                             low_hysteresis;
	UINT16                            min_time_spent;
} tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_PARAMS;

typedef struct {
    BT_HDR                            hdr;
    UINT16                            conn_handle;
	UINT8                             enable;
} tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_ENABLE;

typedef struct {
    BT_HDR                            hdr;
    UINT16                            conn_handle;
	UINT8                             local_enable;
    UINT8                             remote_enable;
} tBTA_DM_API_BLE_SET_TRANS_PWR_RPT_ENABLE;
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)

#if (BLE_FEAT_CONN_SUBRATING == TRUE)
typedef struct {
    BT_HDR hdr;
    UINT16 subrate_min;
    UINT16 subrate_max;
    UINT16 max_latency;
    UINT16 continuation_number;
    UINT16 supervision_timeout;
} tBTA_DM_API_BLE_SET_DEFAULT_SUBRATE;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
    UINT16 subrate_min;
    UINT16 subrate_max;
    UINT16 max_latency;
    UINT16 continuation_number;
    UINT16 supervision_timeout;
} tBTA_DM_API_BLE_SUBRATE_REQUEST;
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
typedef struct {
    BT_HDR                          hdr;
    uint16_t                        bit_num;
    uint8_t                         bit_val;
} tBTA_DM_API_SET_HOST_FEATURE;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BT_BLE_FEAT_PAWR_EN == TRUE)

typedef struct {
	UINT8 subevent;
	UINT8 response_slot_start;
	UINT8 response_slot_count;
    UINT8 subevent_data_len;
	UINT8 *subevent_data;
} tBTA_BLE_SUBEVENT_PARAMS;

typedef struct {
	UINT8 subevent;
	UINT8 response_slot_start;
	UINT8 response_slot_count;
    UINT8 subevent_data_len;
	UINT8 subevent_data[251];
} tBTA_DM_API_BLE_SUBEVENT_PARAMS;

typedef struct {
    BT_HDR hdr;
    UINT8 adv_handle;
    UINT8 num_subevents_with_data;
    tBTA_DM_API_BLE_SUBEVENT_PARAMS *subevent_params;
} tBTA_DM_API_BLE_PA_SUBEVENT_DATA;

typedef struct {
    BT_HDR hdr;
    UINT16 sync_handle;
    UINT16 request_event;
    UINT8 request_subevent;
    UINT8 rsp_subevent;
    UINT8 rsp_slot;
    UINT8 rsp_data_len;
    UINT8 *rsp_data;
} tBTA_DM_API_BLE_PA_RSP_DATA;

typedef struct {
    BT_HDR hdr;
    UINT16 sync_handle;
    UINT16 periodic_adv_properties;
    UINT8 num_subevents_to_sync;
    UINT8 *subevent;
} tBTA_DM_API_BLE_PA_SYNC_SUBEVT;
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)

#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
typedef struct {
    BT_HDR hdr;
} tBTA_DM_API_CS_READ_LOCAL_SUPP_CAPS;
typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
} tBTA_DM_API_CS_READ_REMOTE_SUPP_CAPS;
typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
    UINT8 num_config_supported;
    UINT16 max_consecutive_proc_supported;
    UINT8 num_ant_supported;
    UINT8 max_ant_paths_supported;
    UINT8 roles_supported;
    UINT8 modes_supported;
    UINT8 rtt_capability;
    UINT8 rtt_aa_only_n;
    UINT8 rtt_sounding_n;
    UINT8 rtt_random_payload_n;
    UINT16 NADM_sounding_capability;
    UINT16 NADM_random_capability;
    UINT8  cs_sync_phys_supported;
    UINT16 subfeatures_supported;
    UINT16 T_IP1_times_supported;
    UINT16 T_IP2_times_supported;
    UINT16 T_FCS_times_supported;
    UINT16 T_PM_times_supported;
    UINT8 T_SW_times_supported;
    UINT8 TX_SNR_capability;
} tBTA_DM_API_CS_WRITE_CACHED_REMOTE_SUPP_CAPS;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
} tBTA_DM_API_CS_SECURITY_ENABLE;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
    UINT8 role_enable;
    UINT8 cs_sync_ant_selection;
    INT8 max_tx_power;
} tBTA_DM_API_CS_SET_DEFAULT_SETTING_PARAMS;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
} tBTA_DM_API_CS_READ_REMOTE_TAB;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
    UINT8 remote_fae_table[72];
} tBTA_DM_API_CS_WRITE_CACHED_REMOTE_FAE_TAB_PARAMS;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
    UINT8 config_id;
    UINT8 create_context;
    UINT8 main_mode_type;
    UINT8 sub_mode_type;
    UINT8 min_main_mode_steps;
    UINT8 max_main_mode_steps;
    UINT8 main_mode_repetition;
    UINT8 mode_0_steps;
    UINT8 role;
    UINT8 rtt_type;
    UINT8 cs_sync_phy;
    UINT8 channel_map[10];
    UINT8 channel_map_repetition;
    UINT8 channel_selection_type;
    UINT8 ch3c_shape;
    UINT8 ch3c_jump;
    UINT8 reserved;
} tBTA_DM_API_CS_CREATE_CONFIG_PARAMS;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
    UINT8 config_id;
} tBTA_DM_API_CS_REMOVE_CONFIG_PARAMS;

typedef struct {
    BT_HDR hdr;
    UINT8 channel_class[10];
} tBTA_DM_API_CS_SET_CHANNEL_CLASS_PARAMS;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
    UINT8 config_id;
    UINT16 max_procedure_len;
    UINT16 min_procedure_interval;
    UINT16 max_procedure_interval;
    UINT16 max_procedure_count;
    UINT32 min_subevent_len;
    UINT32 max_subevent_len;
    UINT8 tone_ant_config_selection;
    UINT8 phy;
    UINT8 tx_power_delta;
    UINT8 preferred_peer_antenna;
    UINT8 SNR_control_initiator;
    UINT8 SNR_control_reflector;
} tBTA_DM_API_CS_SET_PROC_PARAMS;

typedef struct {
    BT_HDR hdr;
    UINT16 conn_handle;
    UINT8 config_id;
    UINT8 enable;
} tBTA_DM_API_CS_PROC_ENABLE_PARAMS;
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)

#endif /* BLE_INCLUDED */

typedef struct {
    BT_HDR      hdr;
    BD_ADDR     bd_addr;
    UINT16      min_int;
    UINT16      max_int;
    UINT16      latency;
    UINT16      timeout;
} tBTA_DM_API_UPDATE_CONN_PARAM;

#if (BLE_50_DTM_TEST_EN == TRUE)
typedef struct {
    BT_HDR     hdr;
    UINT8      tx_channel;
    UINT8      len_of_data;
    UINT8      pkt_payload;
    UINT8      phy;
    tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback;
} tBTA_DM_API_BLE_DTM_ENH_TX_START;
typedef struct {
    BT_HDR     hdr;
    UINT8      rx_channel;
    UINT8      phy;
    UINT8      modulation_index;
    tBTA_DTM_CMD_CMPL_CBACK *p_dtm_cmpl_cback;
} tBTA_DM_API_BLE_DTM_ENH_RX_START;
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
#define BTA_PHY_1M_MASK                   (1 << 0)
#define BTA_PHY_2M_MASK                   (1 << 1)
#define BTA_PHY_CODED_MASK                (1 << 2)
typedef struct {
    BT_HDR                          hdr;
    BD_ADDR                         bd_addr;
} tBTA_DM_API_READ_PHY;

typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_GAP_PHY_MASK        tx_phy_mask;
    tBTA_DM_BLE_GAP_PHY_MASK        rx_phy_mask;
} tBTA_DM_API_SET_PER_DEF_PHY;

typedef struct {
    BT_HDR                          hdr;
    BD_ADDR                         bd_addr;
    UINT8                           all_phys;
    tBTA_DM_BLE_GAP_PHY_MASK        tx_phy_mask;
    tBTA_DM_BLE_GAP_PHY_MASK        rx_phy_mask;
    UINT16                          phy_options;
} tBTA_DM_API_SET_PER_PHY;

typedef struct {
    BT_HDR                          hdr;
    UINT16                          instance;
    BD_ADDR                         rand_addr;
} tBTA_DM_API_EXT_ADV_SET_RAND_ADDR;

typedef struct {
    BT_HDR                          hdr;
    UINT16                          instance;
    tBTA_DM_BLE_GAP_EXT_ADV_PARAMS  params;
} tBTA_DM_API_EXT_ADV_SET_PARAMS;

typedef struct {
    BT_HDR                          hdr;
    BOOLEAN                         is_scan_rsp;
    UINT8                           instance;
    UINT16                          length;
    UINT8                           *data;
} tBTA_DM_API_CFG_EXT_ADV_DATA;

typedef struct {
    BT_HDR                         hdr;
    BOOLEAN                        enable;
    UINT8                          num;
    tBTA_DM_BLE_EXT_ADV            *ext_adv;
} tBTA_DM_API_BLE_EXT_ADV;

typedef struct {
    BT_HDR                         hdr;
    UINT16                         instance;
} tBTA_DM_API_BLE_EXT_ADV_SET_REMOVE;

typedef struct {
    BT_HDR                         hdr;
} tBTA_DM_API_BLE_EXT_ADV_SET_CLEAR;

typedef struct {
    BT_HDR                         hdr;
    UINT8                          instance;
    tBTA_DM_BLE_Periodic_Adv_Params params;
} tBTA_DM_API_BLE_PERIODIC_ADV_SET_PARAMS;

typedef struct {
    BT_HDR                          hdr;
    UINT8                           instance;
    UINT16                          length;
    UINT8                           *data;
    BOOLEAN                         only_update_did;
} tBTA_DM_API_CFG_PERIODIC_ADV_DATA;

typedef struct {
    BT_HDR                          hdr;
    UINT8                           instance;
    UINT8                           enable;
} tBTA_DM_API_ENABLE_PERIODIC_ADV;

typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_Periodic_Sync_Params params;
} tBTA_DM_API_PERIODIC_ADV_SYNC;

typedef struct {
    BT_HDR                          hdr;
} tBTA_DM_API_PERIODIC_ADV_SYNC_CANCEL;

typedef struct {
    BT_HDR                          hdr;
    UINT16                          sync_handle;
} tBTA_DM_API_PERIODIC_ADV_SYNC_TERM;

typedef struct {
    BT_HDR                          hdr;
    tBLE_ADDR_TYPE                  addr_type;
    BD_ADDR                         addr;
    UINT16                          sid;
} tBTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LIST;

typedef struct {
    BT_HDR                          hdr;
    tBLE_ADDR_TYPE                  addr_type;
    BD_ADDR                         addr;
    UINT16                          sid;
} tBTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LIST;

typedef struct {
    BT_HDR                          hdr;
} tBTA_DM_API_PERIODIC_ADV_DEV_CLEAR;


typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_EXT_SCAN_PARAMS     params;
} tBTA_DM_API_SET_EXT_SCAN_PARAMS;

typedef struct {
    BT_HDR                          hdr;
    BOOLEAN                         start;
    UINT32                          duration;
    UINT16                          period;
} tBTA_DM_API_EXT_SCAN;

typedef struct {
    BT_HDR                          hdr;
    BD_ADDR                         bd_addr;
    UINT8                           phy_mask;
    tBTA_DM_BLE_CONN_PARAMS         phy_1m_conn_params;
    tBTA_DM_BLE_CONN_PARAMS         phy_2m_conn_params;
    tBTA_DM_BLE_CONN_PARAMS         phy_coded_conn_params;
} tBTA_DM_API_SET_PER_EXT_CONN_PARAMS;

typedef struct {
    BT_HDR                          hdr;
    tBLE_ADDR_TYPE                  own_addr_type;
    BD_ADDR                         peer_addr;
} tBTA_DM_API_EXT_CONN;
#endif //#if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
typedef struct {
    BT_HDR                          hdr;
    UINT16                          sync_handle;
    UINT8                           enable;
} tBTA_DM_API_PERIODIC_ADV_RECV_ENABLE;

typedef struct {
    BT_HDR                          hdr;
    BD_ADDR                         addr;
    UINT16                          service_data;
    UINT16                          sync_handle;
} tBTA_DM_API_PERIODIC_ADV_SYNC_TRANS;

typedef struct {
    BT_HDR                          hdr;
    BD_ADDR                         addr;
    UINT16                          service_data;
    UINT8                           adv_hanlde;
} tBTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS;

typedef struct {
    BT_HDR                          hdr;
    BD_ADDR                         addr;
    tBTA_DM_BLE_PAST_PARAMS         params;
} tBTA_DM_API_SET_PAST_PARAMS;
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_BIG_CREATE_PARAMS big_creat_param;
} tBTA_DM_API_BIG_CREATE;
typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_BIG_CREATE_TEST_PARAMS big_creat_test_param;
} tBTA_DM_API_BIG_CREATE_TEST;
typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_BIG_TERMINATE_PARAMS big_terminate_param;
} tBTA_DM_API_BIG_TERMINATE;
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_BIG_SYNC_CREATE_PARAMS big_sync_param;
} tBTA_DM_API_BIG_SYNC_CREATE;

typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_BIG_SYNC_TERMINATE_PARAMS big_sync_terminate_param;
} tBTA_DM_API_BIG_SYNC_TERMINATE;
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_ISO_SET_DATA_PATH_PARAMS iso_data_path_param;
} tBTA_DM_API_ISO_DATA_PATH;

typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_ISO_REMOVE_DATA_PATH_PARAMS iso_data_path_remove_param;
} tBTA_DM_API_ISO_DATA_PATH_REMOVE;

typedef struct {
    BT_HDR                          hdr;
    uint16_t                        iso_hdl;
} tBTA_DM_API_ISO_READ_TX_SYNC;

typedef struct {
    BT_HDR                          hdr;
    uint16_t                        iso_hdl;
} tBTA_DM_API_ISO_READ_LINK_QUALITY;

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
struct bta_iso_cis_params {
    UINT8 cis_id;
    UINT16 max_sdu_c_to_p;
    UINT16 max_sdu_p_to_c;
    UINT8 phy_c_to_p;
    UINT8 phy_p_to_c;
    UINT8 rtn_c_to_p;
    UINT8 rtn_p_to_c;
} __attribute__((packed));

struct bta_iso_cis_params_test {
    UINT8 cis_id;
    UINT8 nse;
    UINT16 max_sdu_c_to_p;
    UINT16 max_sdu_p_to_c;
    UINT16 max_pdu_c_to_p;
    UINT16 max_pdu_p_to_c;
    UINT8 phy_c_to_p;
    UINT8 phy_p_to_c;
    UINT8 bn_c_to_p;
    UINT8 bn_p_to_c;
} __attribute__((packed));

typedef struct {
    BT_HDR                          hdr;
    UINT8                           cig_id;
    UINT32                          sdu_int_c_to_p;
    UINT32                          sdu_int_p_to_c;
    UINT8                           worse_case_SCA;
    UINT8                           packing;
    UINT8                           framing;
    UINT16                          mtl_c_to_p; // max_transport_latency_c_to_p
    UINT16                          mtl_p_to_c; // max_transport_latency_p_to_c
    UINT8                           cis_cnt;
    struct bta_iso_cis_params       cis_params[BLE_ISO_CIS_MAX_COUNT];
} tBTA_DM_API_SET_CIG_PARAM;

typedef struct {
    BT_HDR                          hdr;
    UINT8                           cig_id;
    UINT32                          sdu_int_c_to_p;
    UINT32                          sdu_int_p_to_c;
    UINT8                           ft_c_to_p;
    UINT8                           ft_p_to_c;
    UINT16                          iso_interval;
    UINT8                           worse_case_SCA;
    UINT8                           packing;
    UINT8                           framing;
    UINT8                           cis_cnt;
    struct bta_iso_cis_params_test  cis_params_test[BLE_ISO_CIS_MAX_COUNT];
} tBTA_DM_API_SET_CIG_PARAM_TEST;

struct bta_iso_cis_hdls {
    uint16_t cis_hdl;
    uint16_t acl_hdl;
}__attribute__((packed));

typedef struct {
    BT_HDR                          hdr;
    uint8_t                         cis_count;
    struct bta_iso_cis_hdls         cis_hdls[BLE_ISO_CIS_MAX_COUNT];
} tBTA_DM_API_CREATE_CIS_PARAM;

typedef struct {
    BT_HDR                          hdr;
    UINT8                           cig_id;
} tBTA_DM_API_REMOVE_CIG_PARAM;

#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
typedef struct {
    BT_HDR                          hdr;
    UINT16                          cis_handle;
} tBTA_DM_API_ACCEPT_CIS_REQ_PARAM;

typedef struct {
    BT_HDR                          hdr;
    UINT16                          cis_handle;
    UINT8                           reason;
} tBTA_DM_API_REJECT_CIS_REQ_PARAM;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_EN == TRUE)
typedef struct {
    BT_HDR                          hdr;
    UINT16                          cis_handle;
    UINT8                           reason;
} tBTA_DM_API_DISCON_CIS_PARAM;
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)

#endif // #if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_CTE_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
typedef struct {
    BT_HDR  hdr;
    UINT8 adv_handle;
    UINT8 cte_len;
    UINT8 cte_type;
    UINT8 cte_count;
    UINT8 switching_pattern_len;
    UINT8 *antenna_ids;
} tBTA_DM_BLE_CTE_SET_TRANS_PARAMS;

typedef struct {
    BT_HDR  hdr;
    UINT8 adv_handle;
    UINT8 cte_enable;
} tBTA_DM_BLE_CTE_SET_TRANS_ENABLE;

typedef struct {
    BT_HDR  hdr;
    UINT16 sync_handle;
    UINT8 sampling_en;
    UINT8 slot_dur;
    UINT8 max_sampled_ctes;
    UINT8 switching_pattern_len;
    UINT8 *antenna_ids;
} tBTA_DM_BLE_CTE_IQ_SAMP_EN;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
typedef struct {
    BT_HDR  hdr;
    UINT16 conn_handle;
    UINT8 sampling_en;
    UINT8 slot_dur;
    UINT8 switching_pattern_len;
    UINT8 *antenna_ids;
} tBTA_DM_BLE_CTE_RECV_PARAMS;

typedef struct {
    BT_HDR  hdr;
    UINT16 conn_handle;
    UINT8 cte_types;
    UINT8 switching_pattern_len;
    UINT8 *antenna_ids;
} tBTA_DM_BLE_CONN_CTE_TRANS_PARAMS;

typedef struct {
    BT_HDR  hdr;
    UINT16 conn_handle;
    UINT8 enable;
    UINT16 cte_req_interval;
    UINT8 req_cte_len;
    UINT8 req_cte_Type;
} tBTA_DM_BLE_CONN_CTE_REQ_EN;

typedef struct {
    BT_HDR  hdr;
    UINT16 conn_handle;
    UINT8 enable;
} tBTA_DM_BLE_CONN_CTE_RSP_EN;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

typedef struct {
    BT_HDR  hdr;
} tBTA_DM_BLE_READ_ANT_INFOR;
#endif // #if (BLE_FEAT_CTE_EN == TRUE)

/* union of all data types */
typedef union {
    /* event buffer header */
    BT_HDR              hdr;
    tBTA_DM_API_ENABLE  enable;

    tBTA_DM_API_SET_NAME set_name;
    tBTA_DM_API_GET_NAME get_name;
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    tBTA_DM_API_CFG_COEX_STATUS cfg_coex_status;
#endif
    tBTA_DM_API_SEND_VENDOR_HCI_CMD vendor_hci_cmd;
#if CLASSIC_BT_INCLUDED
    tBTA_DM_API_CONFIG_EIR config_eir;

    tBTA_DM_API_SET_AFH_CHANNELS set_afh_channels;
    tBTA_DM_API_PAGE_TO_SET set_page_timeout;
    tBTA_DM_API_PAGE_TO_GET get_page_timeout;
    tBTA_DM_API_SET_ACL_PKT_TYPES set_acl_pkt_types;
#endif /* CLASSIC_BT_INCLUDED */
#if (ENC_KEY_SIZE_CTRL_MODE != ENC_KEY_SIZE_CTRL_MODE_NONE)
    tBTA_DM_API_SET_MIN_ENC_KEY_SIZE set_min_enc_key_size;
#endif
#if (SDP_INCLUDED == TRUE)
    tBTA_DM_API_GET_REMOTE_NAME  get_rmt_name;
#endif

#if (BLE_INCLUDED == TRUE)
    tBTA_DM_API_BLE_SET_CHANNELS  ble_set_channels;
    tBTA_DM_API_UPDATE_WHITE_LIST white_list;
#endif  ///BLE_INCLUDED == TRUE
    tBTA_DM_API_READ_RSSI rssi;

    tBTA_DM_API_READ_CH_MAP ch_map;

    tBTA_DM_API_SET_VISIBILITY set_visibility;

    tBTA_DM_API_ADD_DEVICE  add_dev;

    tBTA_DM_API_REMOVE_DEVICE remove_dev;

    tBTA_DM_API_SEARCH search;
#if (SDP_INCLUDED == TRUE)
    tBTA_DM_API_DISCOVER discover;
#endif  ///SDP_INCLUDED == TRUE
    tBTA_DM_API_BOND bond;

    tBTA_DM_API_BOND_CANCEL bond_cancel;

    tBTA_DM_API_SET_PIN_TYPE set_pin_type;
    tBTA_DM_API_PIN_REPLY pin_reply;

    tBTA_DM_API_LOC_OOB     loc_oob;
    tBTA_DM_API_OOB_REPLY   oob_reply;
    tBTA_DM_API_SC_OOB_REPLY sc_oob_reply;
    tBTA_DM_API_CONFIRM     confirm;
    tBTA_DM_API_KEY_REQ     key_req;
    tBTA_DM_CI_IO_REQ       ci_io_req;
    tBTA_DM_CI_RMT_OOB      ci_rmt_oob;

    tBTA_DM_REM_NAME rem_name;

    tBTA_DM_DISC_RESULT disc_result;

    tBTA_DM_INQUIRY_CMPL inq_cmpl;

    tBTA_DM_SDP_RESULT sdp_event;

    tBTA_DM_ACL_CHANGE  acl_change;

#if (BTA_DM_PM_INCLUDED == TRUE)
    tBTA_DM_PM_BTM_STATUS pm_status;

    tBTA_DM_PM_TIMER pm_timer;
#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */

#if (BTA_DM_QOS_INCLUDED == TRUE)
    /* Quality of Service set events */
    tBTA_DM_API_QOS_SET qos_set;
#endif /* #if (BTA_DM_QOS_INCLUDED == TRUE) */

    tBTA_DM_API_DI_DISC     di_disc;
    tBTA_DM_API_SET_ENCRYPTION     set_encryption;

#if BLE_INCLUDED == TRUE
    tBTA_DM_API_ADD_BLEKEY              add_ble_key;
    tBTA_DM_API_ADD_BLE_DEVICE          add_ble_device;
    tBTA_DM_API_PASSKEY_REPLY           ble_passkey_reply;
    tBTA_DM_API_SET_DEFAULT_PASSKEY     ble_set_static_passkey;
    tBTA_DM_API_BLE_SEC_GRANT           ble_sec_grant;
    tBTA_DM_API_BLE_SET_BG_CONN_TYPE    ble_set_bd_conn_type;
    tBTA_DM_API_BLE_CONN_PARAMS         ble_set_conn_params;
    tBTA_DM_API_BLE_SCAN_FILTER_PARAMS  ble_set_scan_fil_params;
    tBTA_DM_API_BLE_OBSERVE             ble_observe;
    tBTA_DM_API_BLE_SCAN                ble_scan;
    tBTA_DM_API_ENABLE_PRIVACY          ble_remote_privacy;
    tBTA_DM_API_LOCAL_PRIVACY           ble_local_privacy;
    tBTA_DM_API_LOCAL_ICON              ble_local_icon;
#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
    tBTA_DM_API_KEY_MATERIAL            ble_key_material;
#endif
    tBTA_DM_API_BLE_ADV_PARAMS_ALL      ble_set_adv_params_all;
    tBTA_DM_API_SET_ADV_CONFIG          ble_set_adv_data;
    tBTA_DM_API_SET_ADV_CONFIG_RAW      ble_set_adv_data_raw;
    tBTA_DM_API_UPDATE_CONN_PARAM       ble_update_conn_params;
    tBTA_DM_API_BLE_SET_DATA_LENGTH     ble_set_data_length;
    tBTA_DM_APT_SET_DEV_ADDR            set_addr;
    tBTA_DM_APT_CLEAR_ADDR              clear_addr;
    tBTA_DM_API_SET_RPA_TIMEOUT         set_rpa_timeout;
    tBTA_DM_API_ADD_DEV_TO_RESOLVING_LIST add_dev_to_resolving_list;
    tBTA_DM_API_BLE_DISCONNECT          ble_disconnect;
#if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
    tBTA_DM_API_UPDATE_DUPLICATE_EXCEPTIONAL_LIST ble_duplicate_exceptional_list;
#endif // ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    tBTA_DM_API_READ_PHY                ble_read_phy;
    tBTA_DM_API_SET_PER_DEF_PHY         ble_set_per_def_phy;
    tBTA_DM_API_SET_PER_PHY             ble_set_per_phy;
#if (BLE_50_EXTEND_ADV_EN == TRUE)
    tBTA_DM_API_EXT_ADV_SET_RAND_ADDR   ble_set_ext_adv_rand_addr;
    tBTA_DM_API_EXT_ADV_SET_PARAMS      ble_set_ext_adv_params;
    tBTA_DM_API_CFG_EXT_ADV_DATA        ble_cfg_ext_adv_data;
    tBTA_DM_API_BLE_EXT_ADV             ble_start_ext_adv;
    tBTA_DM_API_BLE_EXT_ADV_SET_REMOVE  ble_ext_adv_set_remove;
    tBTA_DM_API_BLE_EXT_ADV_SET_CLEAR   ble_ext_adv_set_clear;
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)
#if (BLE_50_PERIODIC_ADV_EN == TRUE)
    tBTA_DM_API_BLE_PERIODIC_ADV_SET_PARAMS ble_set_periodic_adv_params;
    tBTA_DM_API_CFG_PERIODIC_ADV_DATA   ble_cfg_periodic_adv_data;
    tBTA_DM_API_ENABLE_PERIODIC_ADV     ble_enable_periodic_adv;
#endif // #if (BLE_50_PERIODIC_ADV_EN == TRUE)
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
    tBTA_DM_API_PERIODIC_ADV_SYNC       ble_periodic_adv_sync;
    tBTA_DM_API_PERIODIC_ADV_SYNC_CANCEL ble_periodic_adv_sync_cancel;
    tBTA_DM_API_PERIODIC_ADV_SYNC_TERM   ble_periodic_adv_sync_term;
    tBTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LIST ble_periodic_adv_add_dev_to_list;
    tBTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LIST ble_periodic_adv_remove_dev_from_list;
    tBTA_DM_API_PERIODIC_ADV_DEV_CLEAR  ble_periodic_adv_clear_dev;
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
    tBTA_DM_API_SET_EXT_SCAN_PARAMS     ble_set_ext_scan_params;
    tBTA_DM_API_EXT_SCAN                ble_ext_scan;
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
    tBTA_DM_API_SET_PER_EXT_CONN_PARAMS ble_set_per_ext_conn_params;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_50_DTM_TEST_EN == TRUE)
    tBTA_DM_API_BLE_DTM_ENH_TX_START    dtm_enh_tx_start;
    tBTA_DM_API_BLE_DTM_ENH_RX_START    dtm_enh_rx_start;
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    tBTA_DM_API_PERIODIC_ADV_RECV_ENABLE ble_periodic_adv_recv_enable;
    tBTA_DM_API_PERIODIC_ADV_SYNC_TRANS ble_periodic_adv_sync_trans;
    tBTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS ble_periodic_adv_set_info_trans;
    tBTA_DM_API_SET_PAST_PARAMS ble_set_past_params;
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#if (BLE_42_DTM_TEST_EN == TRUE)
    tBTA_DM_API_BLE_DTM_TX_START    dtm_tx_start;
    tBTA_DM_API_BLE_DTM_RX_START    dtm_rx_start;
#endif // #if (BLE_42_DTM_TEST_EN == TRUE)
    tBTA_DM_API_BLE_DTM_STOP        dtm_stop;
    tBTA_DM_API_CLEAR_ADV           ble_clear_adv;
    tBTA_DM_API_SET_PRIVACY_MODE    ble_set_privacy_mode;
    tBTA_DM_API_BLE_SET_CSA_SUPPORT ble_set_csa_support;
    tBTA_DM_API_BLE_SET_VENDOR_EVT_MASK ble_set_vendor_evt_mask;
#endif
#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
    tBTA_DM_API_BIG_CREATE              big_creat;
    tBTA_DM_API_BIG_CREATE_TEST         big_creat_test;
    tBTA_DM_API_BIG_TERMINATE           big_terminate;
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    tBTA_DM_API_BIG_SYNC_CREATE         big_sync;
    tBTA_DM_API_BIG_SYNC_TERMINATE      big_sync_terminate;
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    tBTA_DM_API_ISO_DATA_PATH           iso_set_data_path;
    tBTA_DM_API_ISO_DATA_PATH_REMOVE    iso_remove_data_path;
    tBTA_DM_API_ISO_READ_TX_SYNC        iso_read_tx_sync;
    tBTA_DM_API_ISO_READ_LINK_QUALITY   iso_read_link_quality;
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
    tBTA_DM_API_SET_CIG_PARAM           api_cig_params;
    tBTA_DM_API_SET_CIG_PARAM_TEST      api_cig_params_test;
    tBTA_DM_API_CREATE_CIS_PARAM        create_cis;
    tBTA_DM_API_REMOVE_CIG_PARAM        remove_cig;
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
    tBTA_DM_API_ACCEPT_CIS_REQ_PARAM    accept_cis_req;
    tBTA_DM_API_REJECT_CIS_REQ_PARAM    reject_cis_req;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
    tBTA_DM_API_DISCON_CIS_PARAM        discon_cis;
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
#endif // #if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    tBTA_DM_BLE_CTE_SET_TRANS_PARAMS   set_cte_trans_params;
    tBTA_DM_BLE_CTE_SET_TRANS_ENABLE   set_trans_en;
    tBTA_DM_BLE_CTE_IQ_SAMP_EN         iq_samp_en;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    tBTA_DM_BLE_CTE_RECV_PARAMS        recv_params;
    tBTA_DM_BLE_CONN_CTE_TRANS_PARAMS  conn_trans_params;
    tBTA_DM_BLE_CONN_CTE_REQ_EN        conn_req_en;
    tBTA_DM_BLE_CONN_CTE_RSP_EN        conn_rsp_en;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    tBTA_DM_BLE_READ_ANT_INFOR         read_ant_infor;
#endif // #if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
    tBTA_DM_API_BLE_ENH_READ_TANS_PWR_LEVEL      enh_read_trans_pwr_level;
    tBTA_DM_API_BLE_READ_REMOTE_TANS_PWR_LEVEL   remote_trans_pwr_level;
    tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_PARAMS     path_loss_rpt_params;
    tBTA_DM_API_BLE_SET_PATH_LOSS_RPT_ENABLE     path_loss_rpt_en;
    tBTA_DM_API_BLE_SET_TRANS_PWR_RPT_ENABLE     trans_pwr_rpt_en;
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BLE_FEAT_CONN_SUBRATING == TRUE)
    tBTA_DM_API_BLE_SET_DEFAULT_SUBRATE  default_subrate;
    tBTA_DM_API_BLE_SUBRATE_REQUEST      subrate_request;
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    tBTA_DM_API_SET_HOST_FEATURE    set_host_feat;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    tBTA_DM_API_BLE_PA_SUBEVENT_DATA pa_subevt_data;
    tBTA_DM_API_BLE_PA_RSP_DATA    pa_rsp_data;
    tBTA_DM_API_BLE_PA_SYNC_SUBEVT pa_sync_subevt;
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
    tBTA_DM_API_CS_READ_LOCAL_SUPP_CAPS read_local_supp_caps;
    tBTA_DM_API_CS_READ_REMOTE_SUPP_CAPS read_remote_supp_caps;
    tBTA_DM_API_CS_WRITE_CACHED_REMOTE_SUPP_CAPS write_cached_remote_caps;
    tBTA_DM_API_CS_SECURITY_ENABLE  security_enable;
    tBTA_DM_API_CS_SET_DEFAULT_SETTING_PARAMS set_default_setting_params;
    tBTA_DM_API_CS_READ_REMOTE_TAB read_remote_tab;
    tBTA_DM_API_CS_WRITE_CACHED_REMOTE_FAE_TAB_PARAMS write_cached_remote_fae_tab_params;
    tBTA_DM_API_CS_CREATE_CONFIG_PARAMS create_config_params;
    tBTA_DM_API_CS_REMOVE_CONFIG_PARAMS remove_config_params;
    tBTA_DM_API_CS_SET_CHANNEL_CLASS_PARAMS set_channel_class_params;
    tBTA_DM_API_CS_SET_PROC_PARAMS set_proc_params;
    tBTA_DM_API_CS_PROC_ENABLE_PARAMS proc_enable_params;
#endif
} tBTA_DM_MSG;


#define BTA_DM_NUM_PEER_DEVICE  MAX_ACL_CONNECTIONS

#define BTA_DM_NOT_CONNECTED  0
#define BTA_DM_CONNECTED      1
#define BTA_DM_UNPAIRING      2
typedef UINT8 tBTA_DM_CONN_STATE;


#define BTA_DM_DI_NONE          0x00       /* nothing special */
#define BTA_DM_DI_USE_SSR       0x10       /* set this bit if ssr is supported for this link */
#define BTA_DM_DI_AV_ACTIVE     0x20       /* set this bit if AV is active for this link */
#define BTA_DM_DI_SET_SNIFF     0x01       /* set this bit if call BTM_SetPowerMode(sniff) */
#define BTA_DM_DI_INT_SNIFF     0x02       /* set this bit if call BTM_SetPowerMode(sniff) & enter sniff mode */
#define BTA_DM_DI_ACP_SNIFF     0x04       /* set this bit if peer init sniff */
typedef UINT8 tBTA_DM_DEV_INFO;

/* set power mode request type */
#define BTA_DM_PM_RESTART       1
#define BTA_DM_PM_NEW_REQ       2
#define BTA_DM_PM_EXECUTE       3
typedef UINT8   tBTA_DM_PM_REQ;

typedef struct {
    BD_ADDR                     peer_bdaddr;
    UINT16                      link_policy;
    tBTA_DM_CONN_STATE          conn_state;
    tBTA_PREF_ROLES             pref_role;
    BOOLEAN                     in_use;
    tBTA_DM_DEV_INFO            info;
    tBTA_DM_ENCRYPT_CBACK      *p_encrypt_cback;
#if (BTM_SSR_INCLUDED == TRUE)
    tBTM_PM_STATUS              prev_low;   /* previous low power mode used */
#endif
    tBTA_DM_PM_ACTION           pm_mode_attempted;
    tBTA_DM_PM_ACTION           pm_mode_failed;
    BOOLEAN                     remove_dev_pending;
#if BLE_INCLUDED == TRUE
    UINT16                      conn_handle;
#endif
    tBT_TRANSPORT               transport;
} tBTA_DM_PEER_DEVICE;



/* structure to store list of
  active connections */
typedef struct {
    tBTA_DM_PEER_DEVICE    peer_device[BTA_DM_NUM_PEER_DEVICE];
    UINT8                  count;
#if BLE_INCLUDED == TRUE
    UINT8                  le_count;
#endif
} tBTA_DM_ACTIVE_LINK;


typedef struct {
    BD_ADDR                 peer_bdaddr;
    tBTA_SYS_ID             id;
    UINT8                   app_id;
    tBTA_SYS_CONN_STATUS    state;
    BOOLEAN                 new_request;

} tBTA_DM_SRVCS;

#ifndef BTA_DM_NUM_CONN_SRVS
#define BTA_DM_NUM_CONN_SRVS   10
#endif

typedef struct {

    UINT8 count;
    tBTA_DM_SRVCS  conn_srvc[BTA_DM_NUM_CONN_SRVS];

}  tBTA_DM_CONNECTED_SRVCS;


#if (BTA_DM_PM_INCLUDED == TRUE)

typedef struct {
#define BTA_DM_PM_SNIFF_TIMER_IDX   0
#define BTA_DM_PM_PARK_TIMER_IDX    1
#define BTA_DM_PM_SUSPEND_TIMER_IDX 2
#define BTA_DM_PM_MODE_TIMER_MAX    3
    /*
     * Keep three different timers for PARK, SNIFF and SUSPEND if TBFC is
     * supported.
     */
    TIMER_LIST_ENT          timer[BTA_DM_PM_MODE_TIMER_MAX];

    UINT8                   srvc_id[BTA_DM_PM_MODE_TIMER_MAX];
    UINT8                   pm_action[BTA_DM_PM_MODE_TIMER_MAX];
    UINT8                   active;     /* number of active timer */

    BD_ADDR                 peer_bdaddr;
    BOOLEAN                 in_use;
} tBTA_PM_TIMER;

#define BTA_DM_NUM_PM_TIMER 7
#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */

/* DM control block */
typedef struct {
    BOOLEAN                     is_bta_dm_active;
    tBTA_DM_ACTIVE_LINK         device_list;
    tBTA_DM_SEC_CBACK           *p_sec_cback;
    UINT16                      state;
    BOOLEAN                     disabling;
    TIMER_LIST_ENT              disable_timer;
#if (BTA_DM_PM_INCLUDED == TRUE)
    UINT8                       pm_id;
    tBTA_PM_TIMER               pm_timer[BTA_DM_NUM_PM_TIMER];
#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */
    UINT32                      role_policy_mask;   /* the bits set indicates the modules that wants to remove role switch from the default link policy */
    UINT16                      cur_policy;         /* current default link policy */
    UINT16                      rs_event;           /* the event waiting for role switch */
#if (CLASSIC_BT_INCLUDED == TRUE)
    UINT8                       cur_av_count;       /* current AV connections */
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
    BOOLEAN                     disable_pair_mode;          /* disable pair mode or not */
    BOOLEAN                     conn_paired_only;   /* allow connectable to paired device only or not */
    tBTA_DM_API_SEARCH          search_msg;

#if (CLASSIC_BT_INCLUDED == TRUE)
    UINT16                      page_scan_interval;
    UINT16                      page_scan_window;
    UINT16                      inquiry_scan_interval;
    UINT16                      inquiry_scan_window;

    /* Storage for pin code request parameters */
    BD_ADDR                     pin_bd_addr;
    DEV_CLASS                   pin_dev_class;
    tBTA_DM_SEC_EVT             pin_evt;
    UINT32                      num_val;        /* the numeric value for comparison. If just_works, do not show this number to UI */
    BOOLEAN                     just_works;     /* TRUE, if "Just Works" association model */
#endif

#if ( BTA_EIR_CANNED_UUID_LIST != TRUE )
    /* store UUID list for EIR */
    TIMER_LIST_ENT              app_ready_timer;
    UINT32                      eir_uuid[BTM_EIR_SERVICE_ARRAY_SIZE];
#if (BTA_EIR_SERVER_NUM_CUSTOM_UUID > 0)
    tBT_UUID                    custom_uuid[BTA_EIR_SERVER_NUM_CUSTOM_UUID];
#endif

#endif


    tBTA_DM_ENCRYPT_CBACK      *p_encrypt_cback;
#if (CLASSIC_BT_INCLUDED == TRUE)
    TIMER_LIST_ENT              switch_delay_timer[BTA_DM_NUM_PEER_DEVICE];
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

} tBTA_DM_CB;

#ifndef BTA_DM_SDP_DB_SIZE
#define BTA_DM_SDP_DB_SIZE 250
#endif

/* DM search control block */
typedef struct {

    tBTA_DM_SEARCH_CBACK *p_search_cback;
    tBTM_INQ_INFO         *p_btm_inq_info;
    tBTA_SERVICE_MASK      services;
    tBTA_SERVICE_MASK      services_to_search;
    tBTA_SERVICE_MASK      services_found;
#if (SDP_INCLUDED == TRUE)
    tSDP_DISCOVERY_DB     *p_sdp_db;
#endif  ///SDP_INCLUDED == TRUE
    UINT16                 state;
    BD_ADDR                peer_bdaddr;
    BOOLEAN                name_discover_done;
    BD_NAME                peer_name;
    TIMER_LIST_ENT         search_timer;
    UINT8                  service_index;
    tBTA_DM_MSG           *p_search_queue;   /* search or discover commands during search cancel stored here */
    BOOLEAN                wait_disc;
    BOOLEAN                sdp_results;
#if (SDP_INCLUDED == TRUE)
    tSDP_UUID              uuid;
#endif  ///SDP_INCLUDED == TRUE
    UINT8                  peer_scn;
    BOOLEAN                sdp_search;
    BOOLEAN                cancel_pending; /* inquiry cancel is pending */
    tBTA_TRANSPORT         transport;
#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
    tBTA_DM_SEARCH_CBACK *p_scan_cback;
#if ((defined BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE) && SDP_INCLUDED == TRUE)
    tBTA_GATTC_IF          client_if;
    UINT8                  num_uuid;
    tBT_UUID               *p_srvc_uuid;
    UINT8                  uuid_to_search;
    BOOLEAN                gatt_disc_active;
    UINT16                 conn_id;
    UINT8                  *p_ble_rawdata;
    UINT32                 ble_raw_size;
    UINT32                 ble_raw_used;
    TIMER_LIST_ENT         gatt_close_timer; /* GATT channel close delay timer */
    BD_ADDR                pending_close_bda; /* pending GATT channel remote device address */
#endif
#endif


} tBTA_DM_SEARCH_CB;

/* DI control block */
typedef struct {
#if (SDP_INCLUDED == TRUE)
    tSDP_DISCOVERY_DB     *p_di_db;     /* pointer to the DI discovery database */
#endif  ///SDP_INCLUDED == TRUE
    BOOLEAN             uuid_added;
    UINT8               di_num;         /* total local DI record number */
    UINT32              di_handle[BTA_DI_NUM_MAX];  /* local DI record handle, the first one is primary record */
} tBTA_DM_DI_CB;

/* DM search state */
enum {

    BTA_DM_SEARCH_IDLE,
    BTA_DM_SEARCH_ACTIVE,
    BTA_DM_SEARCH_CANCELLING,
    BTA_DM_DISCOVER_ACTIVE

};



typedef struct {
    DEV_CLASS      dev_class;          /* local device class */
    UINT16         policy_settings;    /* link policy setting hold, sniff, park, MS switch */
    UINT16         page_timeout;       /* timeout for page in slots */
    UINT16         link_timeout;       /* link supervision timeout in slots */
    BOOLEAN        avoid_scatter;      /* TRUE to avoid scatternet when av is streaming (be the master) */

} tBTA_DM_CFG;

extern const UINT32 bta_service_id_to_btm_srv_id_lkup_tbl[];


typedef struct {
    UINT8   id;
    UINT8   app_id;
    UINT8   cfg;

} tBTA_DM_RM ;

extern tBTA_DM_CFG *const p_bta_dm_cfg;
extern tBTA_DM_RM *const p_bta_dm_rm_cfg;

typedef struct {

    UINT8  id;
    UINT8  app_id;
    UINT8  spec_idx;  /* index of spec table to use */

} tBTA_DM_PM_CFG;


typedef struct {

    tBTA_DM_PM_ACTION   power_mode;
    UINT16              timeout;

} tBTA_DM_PM_ACTN;

typedef struct {

    UINT8  allow_mask;         /* mask of sniff/hold/park modes to allow */
#if (BTM_SSR_INCLUDED == TRUE)
    UINT8  ssr;                /* set SSR on conn open/unpark */
#endif
    tBTA_DM_PM_ACTN actn_tbl [BTA_DM_PM_NUM_EVTS][2];

} tBTA_DM_PM_SPEC;

typedef struct {
    UINT16      max_lat;
    UINT16      min_rmt_to;
    UINT16      min_loc_to;
} tBTA_DM_SSR_SPEC;

typedef struct {
    UINT16 manufacturer;
    UINT16 lmp_sub_version;
    UINT8 lmp_version;
} tBTA_DM_LMP_VER_INFO;

#if (BTA_DM_PM_INCLUDED == TRUE)
extern tBTA_DM_PM_CFG *const p_bta_dm_pm_cfg;
extern tBTA_DM_PM_SPEC *const p_bta_dm_pm_spec;
extern tBTM_PM_PWR_MD *const p_bta_dm_pm_md;
#if (BTM_SSR_INCLUDED == TRUE)
extern tBTA_DM_SSR_SPEC *const p_bta_dm_ssr_spec;
#endif
#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */

/* update dynamic BRCM Aware EIR data */
extern tBTA_DM_EIR_CONF bta_dm_eir_cfg;
extern tBTA_DM_EIR_CONF *p_bta_dm_eir_cfg;

/* DM control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_DM_CB  bta_dm_cb;
#else
extern tBTA_DM_CB *bta_dm_cb_ptr;
#define bta_dm_cb (*bta_dm_cb_ptr)
#endif

/* DM search control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_DM_SEARCH_CB  bta_dm_search_cb;
#else
extern tBTA_DM_SEARCH_CB *bta_dm_search_cb_ptr;
#define bta_dm_search_cb (*bta_dm_search_cb_ptr)
#endif

/* DI control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_DM_DI_CB  bta_dm_di_cb;
#else
#if (CLASSIC_BT_INCLUDED == TRUE)
extern tBTA_DM_DI_CB *bta_dm_di_cb_ptr;
#define bta_dm_di_cb (*bta_dm_di_cb_ptr)
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
extern SemaphoreHandle_t deinit_semaphore;
#endif

#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_DM_CONNECTED_SRVCS  bta_dm_conn_srvcs;
#else
extern tBTA_DM_CONNECTED_SRVCS *bta_dm_conn_srvcs_ptr;
#define bta_dm_conn_srvcs (*bta_dm_conn_srvcs_ptr)
#endif

/* Discovery raw data buffer */
#define MAX_DISC_RAW_DATA_BUF       (1024)
#if BTA_DYNAMIC_MEMORY == TRUE
extern UINT8 *g_disc_raw_data_buf;
#endif

extern BOOLEAN bta_dm_sm_execute(BT_HDR *p_msg);
extern void bta_dm_sm_disable( void );
extern void bta_dm_sm_deinit(void);
extern BOOLEAN bta_dm_search_sm_execute(BT_HDR *p_msg);
extern void bta_dm_search_sm_disable( void );


extern void bta_dm_enable (tBTA_DM_MSG *p_data);
extern void bta_dm_disable (tBTA_DM_MSG *p_data);
extern void bta_dm_set_dev_name (tBTA_DM_MSG *p_data);
extern void bta_dm_get_dev_name (tBTA_DM_MSG *p_data);
#if (ESP_COEX_VSC_INCLUDED == TRUE)
extern void bta_dm_cfg_coex_status(tBTA_DM_MSG *p_data);
#endif
extern void bta_dm_send_vendor_hci(tBTA_DM_MSG *p_data);
#if (CLASSIC_BT_INCLUDED == TRUE)
extern void bta_dm_config_eir (tBTA_DM_MSG *p_data);
extern void bta_dm_set_page_timeout (tBTA_DM_MSG *p_data);
extern void bta_dm_get_page_timeout (tBTA_DM_MSG *p_data);
extern void bta_dm_set_acl_pkt_types (tBTA_DM_MSG *p_data);
#if (ENC_KEY_SIZE_CTRL_MODE != ENC_KEY_SIZE_CTRL_MODE_NONE)
extern void bta_dm_set_min_enc_key_size (tBTA_DM_MSG *p_data);
#endif
#endif
extern void bta_dm_set_afh_channels (tBTA_DM_MSG *p_data);
extern void bta_dm_read_rmt_name(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_channels (tBTA_DM_MSG *p_data);
extern void bta_dm_update_white_list(tBTA_DM_MSG *p_data);
extern void bta_dm_clear_white_list(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_read_adv_tx_power(tBTA_DM_MSG *p_data);
extern void bta_dm_read_rssi(tBTA_DM_MSG *p_data);
extern void bta_dm_read_ble_channel_map(tBTA_DM_MSG *p_data);
#if (CLASSIC_BT_INCLUDED == TRUE)
extern void bta_dm_set_visibility (tBTA_DM_MSG *p_data);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

extern void bta_dm_set_scan_config(tBTA_DM_MSG *p_data);
extern void bta_dm_vendor_spec_command(tBTA_DM_MSG *p_data);
extern void bta_dm_bond (tBTA_DM_MSG *p_data);
extern void bta_dm_bond_cancel (tBTA_DM_MSG *p_data);
extern void bta_dm_set_pin_type (tBTA_DM_MSG *p_data);
extern void bta_dm_pin_reply (tBTA_DM_MSG *p_data);
extern void bta_dm_acl_change(tBTA_DM_MSG *p_data);
extern void bta_dm_add_device (tBTA_DM_MSG *p_data);
extern void bta_dm_remove_device (tBTA_DM_MSG *p_data);
extern void bta_dm_close_acl(tBTA_DM_MSG *p_data);

extern void bta_dm_add_ampkey (tBTA_DM_MSG *p_data);

#if BLE_INCLUDED == TRUE
extern void bta_dm_add_blekey (tBTA_DM_MSG *p_data);
extern void bta_dm_add_ble_device (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_passkey_reply (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_static_passkey(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_confirm_reply (tBTA_DM_MSG *p_data);
extern void bta_dm_security_grant (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_bg_conn_type (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_conn_params (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_scan_params(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_scan_fil_params(tBTA_DM_MSG *p_data);
#if ((defined BTA_GATT_INCLUDED) &&  (BTA_GATT_INCLUDED == TRUE) && SDP_INCLUDED == TRUE) && (GATTC_INCLUDED == TRUE)
extern void bta_dm_close_gatt_conn(tBTA_DM_MSG *p_data);
#endif /* ((defined BTA_GATT_INCLUDED) &&  (BTA_GATT_INCLUDED == TRUE) && SDP_INCLUDED == TRUE) && (GATTC_INCLUDED == TRUE) */
extern void bta_dm_ble_observe (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_scan (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_update_conn_params (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_disconnect (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_rand_address(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_clear_rand_address(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_config_local_privacy (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_config_local_icon (tBTA_DM_MSG *p_data);
#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
extern void bta_dm_ble_set_key_material (tBTA_DM_MSG *p_data);
#endif
extern void bta_dm_ble_set_adv_params_all(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_adv_config (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_adv_config_raw (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_scan_rsp (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_scan_rsp_raw (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_advstop (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_data_length(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_update_duplicate_exceptional_list(tBTA_DM_MSG *p_data);
#if SMP_INCLUDED == TRUE
extern void bta_dm_co_security_param_init(void);
#endif
extern void btm_dm_ble_multi_adv_disable(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_multi_adv_data(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_multi_adv_upd_param(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_multi_adv_enb(tBTA_DM_MSG *p_data);
#if (BLE_42_DTM_TEST_EN == TRUE)
extern void bta_dm_ble_gap_dtm_tx_start(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_dtm_rx_start(tBTA_DM_MSG *p_data);
#endif// #if (BLE_42_DTM_TEST_EN == TRUE)
extern void bta_dm_ble_gap_dtm_stop(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_clear_adv(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_set_rpa_timeout(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_add_dev_to_resolving_list(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_set_privacy_mode(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_set_csa_support(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_set_vendor_evt_mask(tBTA_DM_MSG *p_data);
#if (BLE_50_DTM_TEST_EN == TRUE)
extern void bta_dm_ble_gap_dtm_enhance_tx_start(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_dtm_enhance_rx_start(tBTA_DM_MSG *p_data);
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
extern void bta_dm_ble_gap_read_phy(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_set_prefer_default_phy(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_set_prefer_phy(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_ext_adv_set_rand_addr(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_ext_adv_set_params(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_config_ext_adv_data_raw(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_set_ext_scan_params(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_ext_scan(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_gap_set_prefer_ext_conn_params(tBTA_DM_MSG *p_data);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
extern void bta_dm_ble_read_scan_reports(tBTA_DM_MSG *p_data);

#endif
extern void bta_dm_set_encryption(tBTA_DM_MSG *p_data);
extern void bta_dm_confirm(tBTA_DM_MSG *p_data);
extern void bta_dm_key_req(tBTA_DM_MSG *p_data);

#if (BTA_HD_INCLUDED == TRUE)
extern BOOLEAN bta_dm_check_if_only_hd_connected(BD_ADDR peer_addr);
#endif /* BTA_HD_INCLUDED */

#if (BTM_OOB_INCLUDED == TRUE)
extern void bta_dm_loc_oob(tBTA_DM_MSG *p_data);
extern void bta_dm_oob_reply(tBTA_DM_MSG *p_data);
extern void bta_dm_sc_oob_reply(tBTA_DM_MSG *p_data);
extern void bta_dm_sc_create_oob_data(tBTA_DM_MSG *p_data);
extern void bta_dm_ci_io_req_act(tBTA_DM_MSG *p_data);
extern void bta_dm_ci_rmt_oob_act(tBTA_DM_MSG *p_data);
#endif /* BTM_OOB_INCLUDED */

#if (BTA_DM_PM_INCLUDED == TRUE)
extern void bta_dm_init_pm(void);
extern void bta_dm_disable_pm(void);
extern void bta_dm_pm_active(BD_ADDR peer_addr);
extern void bta_dm_pm_btm_status(tBTA_DM_MSG *p_data);
extern void bta_dm_pm_timer(tBTA_DM_MSG *p_data);
#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */

#if (BTA_DM_QOS_INCLUDED == TRUE)
extern void bta_dm_set_qos(tBTA_DM_MSG *p_data);
#endif /* #if (BTA_DM_QOS_INCLUDED == TRUE) */

extern UINT8 bta_dm_get_av_count(void);
extern void bta_dm_search_start (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cancel (tBTA_DM_MSG *p_data);
extern void bta_dm_discover (tBTA_DM_MSG *p_data);
#if (SDP_INCLUDED == TRUE)
extern void bta_dm_di_disc (tBTA_DM_MSG *p_data);
#endif  ///SDP_INCLUDED == TRUE
extern void bta_dm_inq_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_rmt_name (tBTA_DM_MSG *p_data);
#if (SDP_INCLUDED == TRUE)
extern void bta_dm_sdp_result (tBTA_DM_MSG *p_data);
#endif  ///SDP_INCLUDED == TRUE
extern void bta_dm_search_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_free_sdp_db (tBTA_DM_MSG *p_data);
extern void bta_dm_disc_result (tBTA_DM_MSG *p_data);
extern void bta_dm_search_result (tBTA_DM_MSG *p_data);
extern void bta_dm_discovery_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_queue_search (tBTA_DM_MSG *p_data);
extern void bta_dm_queue_disc (tBTA_DM_MSG *p_data);
extern void bta_dm_search_clear_queue (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cancel_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cancel_notify (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cancel_transac_cmpl(tBTA_DM_MSG *p_data);
extern void bta_dm_disc_rmt_name (tBTA_DM_MSG *p_data);
extern tBTA_DM_PEER_DEVICE *bta_dm_find_peer_device(BD_ADDR peer_addr);
void bta_dm_eir_update_uuid(tBT_UUID uuid, BOOLEAN adding);

extern void bta_dm_enable_test_mode(tBTA_DM_MSG *p_data);
extern void bta_dm_disable_test_mode(tBTA_DM_MSG *p_data);
extern void bta_dm_execute_callback(tBTA_DM_MSG *p_data);


#if (BLE_50_FEATURE_SUPPORT == TRUE)
extern void bta_dm_ble_gap_read_phy(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_set_prefer_default_phy(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_set_prefer_phy(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_ext_adv_set_rand_addr(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_ext_adv_set_params(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_config_ext_adv_data_raw(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_start_ext_adv(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_ext_adv_set_remove(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_ext_adv_set_clear(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_set_params(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_cfg_data_raw(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_enable(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_create_sync(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_sync_cancel(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_sync_terminate(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_add_dev_to_list(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_remove_dev_from_list(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_clear_dev(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_set_ext_scan_params(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_ext_scan(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_set_prefer_ext_conn_params(tBTA_DM_MSG *p_data);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
extern void bta_dm_ble_gap_periodic_adv_recv_enable(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_sync_trans(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_periodic_adv_set_info_trans(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_gap_set_periodic_adv_sync_trans_params(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
extern void bta_dm_ble_big_create(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_big_create_test(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_big_terminate(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
extern void bta_dm_ble_big_sync_create(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_big_sync_terminate(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
extern void bta_dm_ble_iso_set_data_path(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_iso_remove_data_path(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_iso_read_tx_sync(tBTA_DM_MSG *p_data);
void bta_dm_ble_iso_read_link_quality(tBTA_DM_MSG *p_data);
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
void bta_dm_ble_set_cig_params(tBTA_DM_MSG *p_data);
void bta_dm_ble_set_cig_params_test(tBTA_DM_MSG *p_data);
void bta_dm_ble_create_cis(tBTA_DM_MSG *p_data);
void bta_dm_ble_remove_cig(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
void bta_dm_ble_accept_cis_req(tBTA_DM_MSG *p_data);
void bta_dm_ble_reject_cis_req(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
void bta_dm_ble_discon_cis(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
#endif // #if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
void bta_dm_ble_set_cte_trans_params(tBTA_DM_MSG *p_data);
void bta_dm_ble_set_cte_trans_enable(tBTA_DM_MSG *p_data);
void bta_dm_ble_set_iq_sampling_en(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
void bta_dm_ble_set_conn_cte_recv_params(tBTA_DM_MSG *p_data);
void bta_dm_ble_set_conn_trans_params(tBTA_DM_MSG *p_data);
void bta_dm_ble_set_conn_cte_req_en(tBTA_DM_MSG *p_data);
void bta_dm_ble_set_conn_cte_rsp_en(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

void bta_dm_ble_read_cte_ant_infor(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_CTE_EN == TRUE)

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
void bta_dm_api_enh_read_trans_power_level(tBTA_DM_MSG *p_data);
void bta_dm_api_read_rem_trans_power_level(tBTA_DM_MSG *p_data);
void bta_dm_api_set_path_loss_report_params(tBTA_DM_MSG *p_data);
void bta_dm_api_set_path_loss_reporting_en(tBTA_DM_MSG *p_data);
void bta_dm_api_set_trans_power_reporting_en(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BLE_FEAT_CONN_SUBRATING == TRUE)
void bta_dm_api_set_default_subrate(tBTA_DM_MSG *p_data);
void bta_dm_api_subrate_request(tBTA_DM_MSG *p_data);
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
extern void bta_dm_ble_set_host_feature(tBTA_DM_MSG *p_data);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
void bta_dm_api_set_periodic_adv_subevt_data(tBTA_DM_MSG *p_data);
void bta_dm_api_set_periodic_adv_response_data(tBTA_DM_MSG *p_data);
void bta_dm_api_set_periodic_sync_subevt(tBTA_DM_MSG *p_data);
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
void bta_dm_api_cs_read_local_supported_caps(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_read_remote_supported_caps(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_write_cached_remote_supported_caps(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_security_enable(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_set_default_settings(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_read_remote_fae_table(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_write_cached_remote_fae_table(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_create_config(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_remove_config(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_set_channel_classification(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_set_procedure_params(tBTA_DM_MSG *p_data);
void bta_dm_api_cs_procedure_enable(tBTA_DM_MSG *p_data);
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
#endif /* BTA_DM_INT_H */
