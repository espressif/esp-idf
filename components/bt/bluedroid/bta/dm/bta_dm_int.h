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

#include "bt_target.h"

#if (BLE_INCLUDED == TRUE && (defined BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE))
#include "bta_gatt_api.h"
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
    BTA_DM_API_SET_VISIBILITY_EVT,

    BTA_DM_ACL_CHANGE_EVT,
    BTA_DM_API_ADD_DEVICE_EVT,
    BTA_DM_API_REMOVE_ACL_EVT,

    /* security API events */
    BTA_DM_API_BOND_EVT,
    BTA_DM_API_BOND_CANCEL_EVT,
    BTA_DM_API_PIN_REPLY_EVT,

    /* power manger events */
    BTA_DM_PM_BTM_STATUS_EVT,
    BTA_DM_PM_TIMER_EVT,

    /* simple pairing events */
    BTA_DM_API_CONFIRM_EVT,

    BTA_DM_API_SET_ENCRYPTION_EVT,

#if (BTM_OOB_INCLUDED == TRUE)
    BTA_DM_API_LOC_OOB_EVT,
    BTA_DM_CI_IO_REQ_EVT,
    BTA_DM_CI_RMT_OOB_EVT,
#endif /* BTM_OOB_INCLUDED */


#if BLE_INCLUDED == TRUE
    BTA_DM_API_ADD_BLEKEY_EVT,
    BTA_DM_API_ADD_BLEDEVICE_EVT,
    BTA_DM_API_BLE_PASSKEY_REPLY_EVT,
    BTA_DM_API_BLE_CONFIRM_REPLY_EVT,
    BTA_DM_API_BLE_SEC_GRANT_EVT,
    BTA_DM_API_BLE_SET_BG_CONN_TYPE,
    BTA_DM_API_BLE_CONN_PARAM_EVT,
    BTA_DM_API_BLE_CONN_SCAN_PARAM_EVT,
    BTA_DM_API_BLE_SCAN_PARAM_EVT,
    /*******This event added by Yulong at 2016/10/25 to
    support the scan filter setting for the APP******/
    BTA_DM_API_BLE_SCAN_FIL_PARAM_EVT,
    BTA_DM_API_BLE_OBSERVE_EVT,
    BTA_DM_API_UPDATE_CONN_PARAM_EVT,
    /*******This event added by Yulong at 2016/9/9 to
    support the random address setting for the APP******/
    BTA_DM_API_SET_RAND_ADDR_EVT,
    /*******This event added by Yulong at 2016/10/19 to
    support stop the ble advertising setting by the APP******/
    BTA_DM_API_BLE_STOP_ADV_EVT,
#if BLE_PRIVACY_SPT == TRUE
    BTA_DM_API_LOCAL_PRIVACY_EVT,
#endif
    BTA_DM_API_BLE_ADV_PARAM_EVT,

    /*******This event added by Yulong at 2016/10/20 to
    support setting the ble advertising param by the APP******/
    BTA_DM_API_BLE_ADV_PARAM_All_EVT,
    BTA_DM_API_BLE_SET_ADV_CONFIG_EVT,
    /* Add for set raw advertising data */
    BTA_DM_API_BLE_SET_ADV_CONFIG_RAW_EVT,
    BTA_DM_API_BLE_SET_SCAN_RSP_EVT,
    /* Add for set raw scan response data */
    BTA_DM_API_BLE_SET_SCAN_RSP_RAW_EVT,
    BTA_DM_API_BLE_BROADCAST_EVT,
    BTA_DM_API_SET_DATA_LENGTH_EVT,

#if BLE_ANDROID_CONTROLLER_SCAN_FILTER == TRUE
    BTA_DM_API_CFG_FILTER_COND_EVT,
    BTA_DM_API_SCAN_FILTER_SETUP_EVT,
    BTA_DM_API_SCAN_FILTER_ENABLE_EVT,
#endif
    BTA_DM_API_BLE_MULTI_ADV_ENB_EVT,
    BTA_DM_API_BLE_MULTI_ADV_PARAM_UPD_EVT,
    BTA_DM_API_BLE_MULTI_ADV_DATA_EVT,
    BTA_DM_API_BLE_MULTI_ADV_DISABLE_EVT,
    BTA_DM_API_BLE_SETUP_STORAGE_EVT,
    BTA_DM_API_BLE_ENABLE_BATCH_SCAN_EVT,
    BTA_DM_API_BLE_DISABLE_BATCH_SCAN_EVT,
    BTA_DM_API_BLE_READ_SCAN_REPORTS_EVT,
    BTA_DM_API_BLE_TRACK_ADVERTISER_EVT,
    BTA_DM_API_BLE_ENERGY_INFO_EVT,

#endif

    BTA_DM_API_ENABLE_TEST_MODE_EVT,
    BTA_DM_API_DISABLE_TEST_MODE_EVT,
    BTA_DM_API_EXECUTE_CBACK_EVT,
    BTA_DM_API_REMOVE_ALL_ACL_EVT,
    BTA_DM_API_REMOVE_DEVICE_EVT,
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
    BTA_DM_DISC_CLOSE_TOUT_EVT

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
} tBTA_DM_API_SET_NAME;

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

/* data type for BTA_DM_API_DI_DISC_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    tBTA_DISCOVERY_DB   *p_sdp_db;
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

/* data type for BTA_DM_API_CONFIRM_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR     bd_addr;
    BOOLEAN     accept;
} tBTA_DM_API_CONFIRM;

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
#if BLE_INCLUDED == TRUE
    UINT16          handle;
    tBT_TRANSPORT   transport;
#endif
} tBTA_DM_ACL_CHANGE;

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
} tBTA_DM_API_ADD_DEVICE;

/* data type for BTA_DM_API_REMOVE_ACL_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
} tBTA_DM_API_REMOVE_DEVICE;

/* data type for BTA_DM_API_EXECUTE_CBACK_EVT */
typedef struct {
    BT_HDR               hdr;
    void                *p_param;
    tBTA_DM_EXEC_CBACK  *p_exec_cback;
} tBTA_DM_API_EXECUTE_CBACK;

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
    BD_ADDR                 bd_addr;
    tBTA_DM_BLE_SEC_GRANT   res;
} tBTA_DM_API_BLE_SEC_GRANT;


typedef struct {
    BT_HDR                  hdr;
    tBTA_DM_BLE_CONN_TYPE   bg_conn_type;
    tBTA_DM_BLE_SEL_CBACK   *p_select_cback;
} tBTA_DM_API_BLE_SET_BG_CONN_TYPE;

/* set prefered BLE connection parameters for a device */
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
} tBTA_DM_API_LOCAL_PRIVACY;

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
    UINT8 scan_filter_policy;
    tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback;
} tBTA_DM_API_BLE_SCAN_FILTER_PARAMS;


/* set scan parameter for BLE connections */
typedef struct {
    BT_HDR                  hdr;
    UINT16                  scan_int;
    UINT16                  scan_window;
} tBTA_DM_API_BLE_CONN_SCAN_PARAMS;

/* Data type for start/stop observe */
typedef struct {
    BT_HDR                  hdr;
    BOOLEAN                 start;
    UINT16                  duration;
    tBTA_DM_SEARCH_CBACK *p_cback;
} tBTA_DM_API_BLE_OBSERVE;

typedef struct {
    BT_HDR      hdr;
    BD_ADDR     remote_bda;
    UINT16      tx_data_length;
} tBTA_DM_API_BLE_SET_DATA_LENGTH;

/* set the address for BLE device
   this type added by Yulong at 2016/9/9*/
typedef struct {
    BT_HDR      hdr;
    tBLE_ADDR_TYPE addr_type;
    BD_ADDR address;
} tBTA_DM_APT_SET_DEV_ADDR;

/* set adv parameter for BLE advertising */
typedef struct {
    BT_HDR                  hdr;
    UINT16                  adv_int_min;
    UINT16                  adv_int_max;
    tBLE_BD_ADDR            *p_dir_bda;
} tBTA_DM_API_BLE_ADV_PARAMS;

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
} tBTA_DM_API_BLE_ADV_PARAMS_ALL;


typedef struct {
    BT_HDR                  hdr;
    BOOLEAN                 enable;

} tBTA_DM_API_BLE_FEATURE;

/* multi adv data structure */
typedef struct {
    BT_HDR                      hdr;
    tBTA_BLE_MULTI_ADV_CBACK    *p_cback;
    void                        *p_ref;
    tBTA_BLE_ADV_PARAMS         *p_params;
} tBTA_DM_API_BLE_MULTI_ADV_ENB;

typedef struct {
    BT_HDR                      hdr;
    UINT8                        inst_id;
    tBTA_BLE_ADV_PARAMS         *p_params;
} tBTA_DM_API_BLE_MULTI_ADV_PARAM;

typedef struct {
    BT_HDR                  hdr;
    UINT8                   inst_id;
    BOOLEAN                 is_scan_rsp;
    tBTA_BLE_AD_MASK        data_mask;
    tBTA_BLE_ADV_DATA      *p_data;
} tBTA_DM_API_BLE_MULTI_ADV_DATA;

typedef struct {
    BT_HDR                  hdr;
    UINT8                   inst_id;
} tBTA_DM_API_BLE_MULTI_ADV_DISABLE;

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
    BT_HDR                  hdr;
    UINT8                   batch_scan_full_max;
    UINT8                   batch_scan_trunc_max;
    UINT8                   batch_scan_notify_threshold;
    tBTA_BLE_SCAN_SETUP_CBACK *p_setup_cback;
    tBTA_BLE_SCAN_THRESHOLD_CBACK *p_thres_cback;
    tBTA_BLE_SCAN_REP_CBACK *p_read_rep_cback;
    tBTA_DM_BLE_REF_VALUE    ref_value;
} tBTA_DM_API_SET_STORAGE_CONFIG;

typedef struct {
    BT_HDR                  hdr;
    tBTA_BLE_BATCH_SCAN_MODE  scan_mode;
    UINT32                  scan_int;
    UINT32                  scan_window;
    tBTA_BLE_DISCARD_RULE   discard_rule;
    tBLE_ADDR_TYPE          addr_type;
    tBTA_DM_BLE_REF_VALUE   ref_value;
} tBTA_DM_API_ENABLE_SCAN;

typedef struct {
    BT_HDR                  hdr;
    tBTA_DM_BLE_REF_VALUE    ref_value;
} tBTA_DM_API_DISABLE_SCAN;

typedef struct {
    BT_HDR                  hdr;
    tBTA_BLE_BATCH_SCAN_MODE scan_type;
    tBTA_DM_BLE_REF_VALUE    ref_value;
} tBTA_DM_API_READ_SCAN_REPORTS;

typedef struct {
    BT_HDR                  hdr;
    tBTA_DM_BLE_REF_VALUE ref_value;
    tBTA_BLE_TRACK_ADV_CBACK *p_track_adv_cback;
} tBTA_DM_API_TRACK_ADVERTISER;

typedef struct {
    BT_HDR                  hdr;
    tBTA_BLE_ENERGY_INFO_CBACK *p_energy_info_cback;
} tBTA_DM_API_ENERGY_INFO;

#endif /* BLE_INCLUDED */

/* data type for BTA_DM_API_REMOVE_ACL_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR     bd_addr;
    BOOLEAN     remove_dev;
    tBTA_TRANSPORT transport;

} tBTA_DM_API_REMOVE_ACL;

/* data type for BTA_DM_API_REMOVE_ALL_ACL_EVT */
typedef struct {
    BT_HDR      hdr;
    tBTA_DM_LINK_TYPE link_type;

} tBTA_DM_API_REMOVE_ALL_ACL;
typedef struct {
    BT_HDR      hdr;
    BD_ADDR     bd_addr;
    UINT16      min_int;
    UINT16      max_int;
    UINT16      latency;
    UINT16      timeout;
} tBTA_DM_API_UPDATE_CONN_PARAM;

#if BLE_ANDROID_CONTROLLER_SCAN_FILTER == TRUE
typedef struct {
    BT_HDR                          hdr;
    tBTA_DM_BLE_SCAN_COND_OP        action;
    tBTA_DM_BLE_PF_COND_TYPE        cond_type;
    tBTA_DM_BLE_PF_FILT_INDEX       filt_index;
    tBTA_DM_BLE_PF_COND_PARAM       *p_cond_param;
    tBTA_DM_BLE_PF_CFG_CBACK      *p_filt_cfg_cback;
    tBTA_DM_BLE_REF_VALUE            ref_value;
} tBTA_DM_API_CFG_FILTER_COND;

typedef struct {
    BT_HDR                          hdr;
    UINT8                           action;
    tBTA_DM_BLE_PF_STATUS_CBACK    *p_filt_status_cback;
    tBTA_DM_BLE_REF_VALUE            ref_value;
} tBTA_DM_API_ENABLE_SCAN_FILTER;

typedef struct {
    BT_HDR                          hdr;
    UINT8                           action;
    tBTA_DM_BLE_PF_FILT_INDEX       filt_index;
    tBTA_DM_BLE_PF_FILT_PARAMS      filt_params;
    tBLE_BD_ADDR                    *p_target;
    tBTA_DM_BLE_PF_PARAM_CBACK      *p_filt_param_cback;
    tBTA_DM_BLE_REF_VALUE            ref_value;
} tBTA_DM_API_SCAN_FILTER_PARAM_SETUP;
#endif

/* union of all data types */
typedef union {
    /* GKI event buffer header */
    BT_HDR              hdr;
    tBTA_DM_API_ENABLE  enable;

    tBTA_DM_API_SET_NAME set_name;

    tBTA_DM_API_SET_VISIBILITY set_visibility;

    tBTA_DM_API_ADD_DEVICE  add_dev;

    tBTA_DM_API_REMOVE_DEVICE remove_dev;

    tBTA_DM_API_SEARCH search;

    tBTA_DM_API_DISCOVER discover;

    tBTA_DM_API_BOND bond;

    tBTA_DM_API_BOND_CANCEL bond_cancel;

    tBTA_DM_API_PIN_REPLY pin_reply;

    tBTA_DM_API_LOC_OOB     loc_oob;
    tBTA_DM_API_CONFIRM     confirm;
    tBTA_DM_CI_IO_REQ       ci_io_req;
    tBTA_DM_CI_RMT_OOB      ci_rmt_oob;

    tBTA_DM_REM_NAME rem_name;

    tBTA_DM_DISC_RESULT disc_result;

    tBTA_DM_INQUIRY_CMPL inq_cmpl;

    tBTA_DM_SDP_RESULT sdp_event;

    tBTA_DM_ACL_CHANGE  acl_change;

    tBTA_DM_PM_BTM_STATUS pm_status;

    tBTA_DM_PM_TIMER pm_timer;

    tBTA_DM_API_DI_DISC     di_disc;

    tBTA_DM_API_EXECUTE_CBACK exec_cback;

    tBTA_DM_API_SET_ENCRYPTION     set_encryption;

#if BLE_INCLUDED == TRUE
    tBTA_DM_API_ADD_BLEKEY              add_ble_key;
    tBTA_DM_API_ADD_BLE_DEVICE          add_ble_device;
    tBTA_DM_API_PASSKEY_REPLY           ble_passkey_reply;
    tBTA_DM_API_BLE_SEC_GRANT           ble_sec_grant;
    tBTA_DM_API_BLE_SET_BG_CONN_TYPE    ble_set_bd_conn_type;
    tBTA_DM_API_BLE_CONN_PARAMS         ble_set_conn_params;
    tBTA_DM_API_BLE_CONN_SCAN_PARAMS    ble_set_conn_scan_params;
    tBTA_DM_API_BLE_SCAN_PARAMS         ble_set_scan_params;
    tBTA_DM_API_BLE_SCAN_FILTER_PARAMS  ble_set_scan_fil_params;
    tBTA_DM_API_BLE_OBSERVE             ble_observe;
    tBTA_DM_API_ENABLE_PRIVACY          ble_remote_privacy;
    tBTA_DM_API_LOCAL_PRIVACY           ble_local_privacy;
    tBTA_DM_API_BLE_ADV_PARAMS          ble_set_adv_params;
    tBTA_DM_API_BLE_ADV_PARAMS_ALL      ble_set_adv_params_all;
    tBTA_DM_API_SET_ADV_CONFIG          ble_set_adv_data;
    tBTA_DM_API_SET_ADV_CONFIG_RAW      ble_set_adv_data_raw;
#if BLE_ANDROID_CONTROLLER_SCAN_FILTER == TRUE
    tBTA_DM_API_SCAN_FILTER_PARAM_SETUP ble_scan_filt_param_setup;
    tBTA_DM_API_CFG_FILTER_COND         ble_cfg_filter_cond;
    tBTA_DM_API_ENABLE_SCAN_FILTER      ble_enable_scan_filt;
#endif
    tBTA_DM_API_UPDATE_CONN_PARAM       ble_update_conn_params;
    tBTA_DM_API_BLE_SET_DATA_LENGTH     ble_set_data_length;
    tBTA_DM_APT_SET_DEV_ADDR            set_addr;
    tBTA_DM_API_BLE_MULTI_ADV_ENB       ble_multi_adv_enb;
    tBTA_DM_API_BLE_MULTI_ADV_PARAM     ble_multi_adv_param;
    tBTA_DM_API_BLE_MULTI_ADV_DATA      ble_multi_adv_data;
    tBTA_DM_API_BLE_MULTI_ADV_DISABLE   ble_multi_adv_disable;

    tBTA_DM_API_SET_STORAGE_CONFIG      ble_set_storage;
    tBTA_DM_API_ENABLE_SCAN             ble_enable_scan;
    tBTA_DM_API_READ_SCAN_REPORTS       ble_read_reports;
    tBTA_DM_API_DISABLE_SCAN            ble_disable_scan;
    tBTA_DM_API_TRACK_ADVERTISER        ble_track_advert;
    tBTA_DM_API_ENERGY_INFO             ble_energy_info;
#endif

    tBTA_DM_API_REMOVE_ACL              remove_acl;
    tBTA_DM_API_REMOVE_ALL_ACL          remove_all_acl;

} tBTA_DM_MSG;


#define BTA_DM_NUM_PEER_DEVICE 7

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
    tBT_TRANSPORT               transport;
#endif
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

extern tBTA_DM_CONNECTED_SRVCS bta_dm_conn_srvcs;

#define BTA_DM_NUM_PM_TIMER 7

/* DM control block */
typedef struct {
    BOOLEAN                     is_bta_dm_active;
    tBTA_DM_ACTIVE_LINK         device_list;
    tBTA_DM_SEC_CBACK           *p_sec_cback;
#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
    tBTA_BLE_SCAN_SETUP_CBACK   *p_setup_cback;
    tBTA_DM_BLE_PF_CFG_CBACK     *p_scan_filt_cfg_cback;
    tBTA_DM_BLE_PF_STATUS_CBACK  *p_scan_filt_status_cback;
    tBTA_DM_BLE_PF_PARAM_CBACK   *p_scan_filt_param_cback;
    tBTA_BLE_MULTI_ADV_CBACK     *p_multi_adv_cback;
    tBTA_BLE_ENERGY_INFO_CBACK   *p_energy_info_cback;
#endif
    UINT16                      state;
    BOOLEAN                     disabling;
    TIMER_LIST_ENT              disable_timer;
    UINT32                      wbt_sdp_handle;          /* WIDCOMM Extensions SDP record handle */
    UINT8                       wbt_scn;                 /* WIDCOMM Extensions SCN */
    UINT8                       num_master_only;
    UINT8                       pm_id;
    tBTA_PM_TIMER               pm_timer[BTA_DM_NUM_PM_TIMER];
    UINT32                      role_policy_mask;   /* the bits set indicates the modules that wants to remove role switch from the default link policy */
    UINT16                      cur_policy;         /* current default link policy */
    UINT16                      rs_event;           /* the event waiting for role switch */
    UINT8                       cur_av_count;       /* current AV connecions */
    BOOLEAN                     disable_pair_mode;          /* disable pair mode or not */
    BOOLEAN                     conn_paired_only;   /* allow connectable to paired device only or not */
    tBTA_DM_API_SEARCH          search_msg;
    UINT16                      page_scan_interval;
    UINT16                      page_scan_window;
    UINT16                      inquiry_scan_interval;
    UINT16                      inquiry_scan_window;

    /* Storage for pin code request parameters */
    BD_ADDR                     pin_bd_addr;
    DEV_CLASS                   pin_dev_class;
    tBTA_DM_SEC_EVT             pin_evt;
    UINT32          num_val;        /* the numeric value for comparison. If just_works, do not show this number to UI */
    BOOLEAN         just_works;     /* TRUE, if "Just Works" association model */
#if ( BTA_EIR_CANNED_UUID_LIST != TRUE )
    /* store UUID list for EIR */
    TIMER_LIST_ENT              app_ready_timer;
    UINT32                      eir_uuid[BTM_EIR_SERVICE_ARRAY_SIZE];
#if (BTA_EIR_SERVER_NUM_CUSTOM_UUID > 0)
    tBT_UUID                    custom_uuid[BTA_EIR_SERVER_NUM_CUSTOM_UUID];
#endif

#endif


    tBTA_DM_ENCRYPT_CBACK      *p_encrypt_cback;
    TIMER_LIST_ENT              switch_delay_timer;

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
    tSDP_DISCOVERY_DB     *p_sdp_db;
    UINT16                 state;
    BD_ADDR                peer_bdaddr;
    BOOLEAN                name_discover_done;
    BD_NAME                peer_name;
    TIMER_LIST_ENT         search_timer;
    UINT8                  service_index;
    tBTA_DM_MSG           *p_search_queue;   /* search or discover commands during search cancel stored here */
    BOOLEAN                wait_disc;
    BOOLEAN                sdp_results;
    tSDP_UUID              uuid;
    UINT8                  peer_scn;
    BOOLEAN                sdp_search;
    BOOLEAN                cancel_pending; /* inquiry cancel is pending */
    tBTA_TRANSPORT         transport;
#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
    tBTA_DM_SEARCH_CBACK *p_scan_cback;
#if ((defined BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE))
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
    tSDP_DISCOVERY_DB     *p_di_db;     /* pointer to the DI discovery database */
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

extern tBTA_DM_CFG *p_bta_dm_cfg;
extern tBTA_DM_RM *p_bta_dm_rm_cfg;

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

extern tBTA_DM_PM_CFG *p_bta_dm_pm_cfg;
extern tBTA_DM_PM_SPEC *p_bta_dm_pm_spec;
extern tBTM_PM_PWR_MD *p_bta_dm_pm_md;
#if (BTM_SSR_INCLUDED == TRUE)
extern tBTA_DM_SSR_SPEC *p_bta_dm_ssr_spec;
#endif

/* update dynamic BRCM Aware EIR data */
extern const tBTA_DM_EIR_CONF bta_dm_eir_cfg;
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
extern tBTA_DM_DI_CB *bta_dm_di_cb_ptr;
#define bta_dm_di_cb (*bta_dm_di_cb_ptr)
#endif

extern BOOLEAN bta_dm_sm_execute(BT_HDR *p_msg);
extern void bta_dm_sm_disable( void );
extern BOOLEAN bta_dm_search_sm_execute(BT_HDR *p_msg);
extern void bta_dm_search_sm_disable( void );


extern void bta_dm_enable (tBTA_DM_MSG *p_data);
extern void bta_dm_disable (tBTA_DM_MSG *p_data);
extern void bta_dm_set_dev_name (tBTA_DM_MSG *p_data);
extern void bta_dm_set_visibility (tBTA_DM_MSG *p_data);

extern void bta_dm_set_scan_config(tBTA_DM_MSG *p_data);
extern void bta_dm_vendor_spec_command(tBTA_DM_MSG *p_data);
extern void bta_dm_bond (tBTA_DM_MSG *p_data);
extern void bta_dm_bond_cancel (tBTA_DM_MSG *p_data);
extern void bta_dm_pin_reply (tBTA_DM_MSG *p_data);
extern void bta_dm_acl_change(tBTA_DM_MSG *p_data);
extern void bta_dm_add_device (tBTA_DM_MSG *p_data);
extern void bta_dm_remove_device (tBTA_DM_MSG *p_data);
extern void bta_dm_close_acl(tBTA_DM_MSG *p_data);


extern void bta_dm_pm_btm_status(tBTA_DM_MSG *p_data);
extern void bta_dm_pm_timer(tBTA_DM_MSG *p_data);
extern void bta_dm_add_ampkey (tBTA_DM_MSG *p_data);

#if BLE_INCLUDED == TRUE
extern void bta_dm_add_blekey (tBTA_DM_MSG *p_data);
extern void bta_dm_add_ble_device (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_passkey_reply (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_confirm_reply (tBTA_DM_MSG *p_data);
extern void bta_dm_security_grant (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_bg_conn_type (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_conn_params (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_scan_params(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_scan_fil_params(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_conn_scan_params (tBTA_DM_MSG *p_data);
extern void bta_dm_close_gatt_conn(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_observe (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_update_conn_params (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_rand_address(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_stop_advertising(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_config_local_privacy (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_adv_params (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_adv_params_all(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_adv_config (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_adv_config_raw (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_scan_rsp (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_scan_rsp_raw (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_broadcast (tBTA_DM_MSG *p_data);
extern void bta_dm_ble_set_data_length(tBTA_DM_MSG *p_data);

#if BLE_ANDROID_CONTROLLER_SCAN_FILTER == TRUE
extern void bta_dm_cfg_filter_cond (tBTA_DM_MSG *p_data);
extern void bta_dm_scan_filter_param_setup (tBTA_DM_MSG *p_data);
extern void bta_dm_enable_scan_filter(tBTA_DM_MSG *p_data);
#endif
extern void btm_dm_ble_multi_adv_disable(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_multi_adv_data(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_multi_adv_upd_param(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_multi_adv_enb(tBTA_DM_MSG *p_data);

extern void bta_dm_ble_setup_storage(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_enable_batch_scan(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_disable_batch_scan(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_read_scan_reports(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_track_advertiser(tBTA_DM_MSG *p_data);
extern void bta_dm_ble_get_energy_info(tBTA_DM_MSG *p_data);

#endif
extern void bta_dm_set_encryption(tBTA_DM_MSG *p_data);
extern void bta_dm_confirm(tBTA_DM_MSG *p_data);
#if (BTM_OOB_INCLUDED == TRUE)
extern void bta_dm_loc_oob(tBTA_DM_MSG *p_data);
extern void bta_dm_ci_io_req_act(tBTA_DM_MSG *p_data);
extern void bta_dm_ci_rmt_oob_act(tBTA_DM_MSG *p_data);
#endif /* BTM_OOB_INCLUDED */

extern void bta_dm_init_pm(void);
extern void bta_dm_disable_pm(void);

extern UINT8 bta_dm_get_av_count(void);
extern void bta_dm_search_start (tBTA_DM_MSG *p_data);
extern void bta_dm_search_cancel (tBTA_DM_MSG *p_data);
extern void bta_dm_discover (tBTA_DM_MSG *p_data);
extern void bta_dm_di_disc (tBTA_DM_MSG *p_data);
extern void bta_dm_inq_cmpl (tBTA_DM_MSG *p_data);
extern void bta_dm_rmt_name (tBTA_DM_MSG *p_data);
extern void bta_dm_sdp_result (tBTA_DM_MSG *p_data);
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

extern void bta_dm_pm_active(BD_ADDR peer_addr);

void bta_dm_eir_update_uuid(UINT16 uuid16, BOOLEAN adding);

extern void bta_dm_enable_test_mode(tBTA_DM_MSG *p_data);
extern void bta_dm_disable_test_mode(tBTA_DM_MSG *p_data);
extern void bta_dm_execute_callback(tBTA_DM_MSG *p_data);


extern void bta_dm_remove_all_acl(tBTA_DM_MSG *p_data);
#endif /* BTA_DM_INT_H */
