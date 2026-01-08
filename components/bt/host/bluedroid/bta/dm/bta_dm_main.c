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
 *  This is the main implementation file for the BTA device manager.
 *
 ******************************************************************************/

#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta_dm_int.h"
#if (ESP_COEX_VSC_INCLUDED == TRUE)
#include "stack/btm_api.h"
#endif
#include "osi/allocator.h"
#include <string.h>

#ifdef CONFIG_ESP_COEX_ENABLED
#include "esp_coexist.h"
#endif

/*****************************************************************************
** Constants and types
*****************************************************************************/

#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_DM_CB  bta_dm_cb;
tBTA_DM_SEARCH_CB bta_dm_search_cb;
#if (CLASSIC_BT_INCLUDED == TRUE)
tBTA_DM_DI_CB       bta_dm_di_cb;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
#else
tBTA_DM_CB  *bta_dm_cb_ptr;
tBTA_DM_SEARCH_CB *bta_dm_search_cb_ptr;
#if (CLASSIC_BT_INCLUDED == TRUE)
 tBTA_DM_DI_CB       *bta_dm_di_cb_ptr;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
SemaphoreHandle_t deinit_semaphore;
#endif


#define BTA_DM_NUM_ACTIONS  (BTA_DM_MAX_EVT & 0x00ff)

/* type for action functions */
typedef void (*tBTA_DM_ACTION)(tBTA_DM_MSG *p_data);

/* action function list */
const tBTA_DM_ACTION bta_dm_action[BTA_DM_MAX_EVT] = {

    /* device manager local device API events */
    bta_dm_enable,                          /* BTA_DM_API_ENABLE_EVT */
    bta_dm_disable,                         /* BTA_DM_API_DISABLE_EVT */
    bta_dm_set_dev_name,                    /* BTA_DM_API_SET_NAME_EVT */
    bta_dm_get_dev_name,                    /* BTA_DM_API_GET_NAME_EVT */
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    bta_dm_cfg_coex_status,                 /* BTA_DM_API_CFG_COEX_ST_EVT */
#endif
#if (BLE_VENDOR_HCI_EN == TRUE)
    bta_dm_send_vendor_hci,                 /* BTA_DM_API_SEND_VENDOR_HCI_CMD_EVT */
    bta_dm_ble_gap_clear_adv,               /* BTA_DM_API_BLE_CLEAR_ADV_EVT */
    bta_dm_ble_gap_set_csa_support,         /* BTA_DM_API_BLE_SET_CSA_SUPPORT_EVT */
    bta_dm_ble_gap_set_vendor_evt_mask,     /* BTA_DM_API_BLE_SET_VENDOR_EVT_MASK_EVT */
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)
#if (CLASSIC_BT_INCLUDED == TRUE)
    bta_dm_config_eir,                      /* BTA_DM_API_CONFIG_EIR_EVT */
    bta_dm_set_page_timeout,                /* BTA_DM_API_PAGE_TO_SET_EVT */
    bta_dm_get_page_timeout,                /* BTA_DM_API_PAGE_TO_GET_EVT */
    bta_dm_set_acl_pkt_types,               /* BTA_DM_API_SET_ACL_PKT_TYPES_EVT */
#if (ENC_KEY_SIZE_CTRL_MODE != ENC_KEY_SIZE_CTRL_MODE_NONE)
    bta_dm_set_min_enc_key_size,            /* BTA_DM_API_SET_MIN_ENC_KEY_SIZE_EVT */
#endif

    bta_dm_set_afh_channels,                /* BTA_DM_API_SET_AFH_CHANNELS_EVT */
#if (SDP_INCLUDED == TRUE)
    bta_dm_read_rmt_name,                    /* BTA_DM_API_GET_REMOTE_NAME_EVT*/
#endif
    bta_dm_set_visibility,                  /* BTA_DM_API_SET_VISIBILITY_EVT */
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
    bta_dm_acl_change,                      /* BTA_DM_ACL_CHANGE_EVT */
    bta_dm_add_device,                      /* BTA_DM_API_ADD_DEVICE_EVT */
#if (SMP_INCLUDED == TRUE)
    /* security API events */
    bta_dm_bond,                            /* BTA_DM_API_BOND_EVT */
    bta_dm_bond_cancel,                     /* BTA_DM_API_BOND_CANCEL_EVT */
    bta_dm_set_pin_type,                    /* BTA_DM_API_SET_PIN_TYPE_EVT */
    bta_dm_pin_reply,                       /* BTA_DM_API_PIN_REPLY_EVT */
#endif  ///SMP_INCLUDED == TRUE
#if (BTA_DM_PM_INCLUDED == TRUE)
    /* power manager events */
    bta_dm_pm_btm_status,                   /* BTA_DM_PM_BTM_STATUS_EVT */
    bta_dm_pm_timer,                        /* BTA_DM_PM_TIMER_EVT */
#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */
#if (BTA_DM_QOS_INCLUDED == TRUE)
    /* Quality of Service set events */
    bta_dm_set_qos,                         /* BTA_DM_API_QOS_SET_EVT */
#endif /* #if (BTA_DM_QOS_INCLUDED == TRUE) */
    /* simple pairing events */
#if (SMP_INCLUDED == TRUE)
#if (CLASSIC_BT_INCLUDED == TRUE)
    bta_dm_confirm,                         /* BTA_DM_API_CONFIRM_EVT */
    bta_dm_key_req,                         /* BTA_DM_API_KEY_REQ_EVT */
#endif  /* (CLASSIC_BT_INCLUDED == TRUE) */
    bta_dm_set_encryption,                  /* BTA_DM_API_SET_ENCRYPTION_EVT */
#endif /* (SMP_INCLUDED == TRUE) */
#if (BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
    bta_dm_loc_oob,                         /* BTA_DM_API_LOC_OOB_EVT */
    bta_dm_oob_reply,                       /* BTA_DM_API_OOB_REPLY_EVT */
    bta_dm_sc_oob_reply,                    /* BTA_DM_API_SC_OOB_REPLY_EVT */
    bta_dm_sc_create_oob_data,              /* BTA_DM_API_SC_CR_OOB_DATA_EVT */
    bta_dm_ci_io_req_act,                   /* BTA_DM_CI_IO_REQ_EVT */
    bta_dm_ci_rmt_oob_act,                  /* BTA_DM_CI_RMT_OOB_EVT */
#endif /* BTM_OOB_INCLUDED */


#if BLE_INCLUDED == TRUE
#if SMP_INCLUDED == TRUE
    bta_dm_add_blekey,                      /*  BTA_DM_API_ADD_BLEKEY_EVT           */
    bta_dm_add_ble_device,                  /*  BTA_DM_API_ADD_BLEDEVICE_EVT        */
    bta_dm_ble_passkey_reply,               /*  BTA_DM_API_BLE_PASSKEY_REPLY_EVT    */
    bta_dm_ble_set_static_passkey,          /* BTA_DM_API_BLE_SET_STATIC_PASSKEY_EVT */
    bta_dm_ble_confirm_reply,               /*  BTA_DM_API_BLE_CONFIRM_REPLY_EVT    */
    bta_dm_security_grant,                  /* BTA_DM_API_BLE_SEC_GRANT_EVT */
#endif  ///SMP_INCLUDED == TRUE
#if (BLE_GATT_BGCONN == TRUE)
    bta_dm_ble_set_bg_conn_type,            /* BTA_DM_API_BLE_SET_BG_CONN_TYPE */
#endif // (BLE_GATT_BGCONN == TRUE)
    bta_dm_ble_set_conn_params,             /* BTA_DM_API_BLE_CONN_PARAM_EVT */
#if (BLE_42_SCAN_EN == TRUE)
    bta_dm_ble_set_scan_fil_params,         /* BTA_DM_API_BLE_SCAN_FIL_PARAM_EVT */
#endif // #if (BLE_42_SCAN_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    bta_dm_ble_scan,                        /* BTA_DM_API_BLE_SCAN_EVT */
#endif // #if (BLE_42_SCAN_EN == TRUE)
    bta_dm_ble_update_conn_params,          /* BTA_DM_API_UPDATE_CONN_PARAM_EVT */
    bta_dm_ble_set_rand_address,            /* BTA_DM_API_SET_RAND_ADDR_EVT*/
    bta_dm_ble_clear_rand_address,          /* BTA_DM_API_CLEAR_RAND_ADDR_EVT */
#if BLE_PRIVACY_SPT == TRUE
    bta_dm_ble_config_local_privacy,        /* BTA_DM_API_LOCAL_PRIVACY_EVT */
#endif
    bta_dm_ble_config_local_icon,           /* BTA_DM_API_LOCAL_ICON_EVT */
#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
    bta_dm_ble_set_key_material,            /* BTA_DM_API_KEY_MATERIAL_EVT */
#endif
#if (BLE_42_ADV_EN == TRUE)
    bta_dm_ble_set_adv_params_all,          /* BTA_DM_API_BLE_ADV_PARAM_All_EVT */
    bta_dm_ble_set_adv_config,              /* BTA_DM_API_BLE_SET_ADV_CONFIG_EVT */
    /* New function to allow set raw adv
       data to HCI */
    bta_dm_ble_set_adv_config_raw,          /* BTA_DM_API_BLE_SET_ADV_CONFIG_RAW_EVT */
    bta_dm_ble_set_scan_rsp,                /* BTA_DM_API_BLE_SET_SCAN_RSP_EVT */
    /* New function to allow set raw scan
       response data to HCI */
    bta_dm_ble_set_scan_rsp_raw,            /* BTA_DM_API_BLE_SET_SCAN_RSP_RAW_EVT */
    bta_dm_ble_advstop,                   /* BTA_DM_API_BLE_ADVSTOP_EVT */
#endif // #if (BLE_42_ADV_EN == TRUE)
    bta_dm_ble_set_data_length,             /* BTA_DM_API_SET_DATA_LENGTH_EVT */
    bta_dm_ble_disconnect,                  /* BTA_DM_API_BLE_DISCONNECT_EVT */
#endif
    bta_dm_remove_device,                   /* BTA_DM_API_REMOVE_DEVICE_EVT */
    bta_dm_ble_set_channels,                /* BTA_DM_API_BLE_SET_CHANNELS_EVT */
    bta_dm_update_white_list,               /* BTA_DM_API_UPDATE_WHITE_LIST_EVT */
    bta_dm_clear_white_list,                /* BTA_DM_API_CLEAR_WHITE_LIST_EVT */
    bta_dm_read_rssi,                       /* BTA_DM_API_READ_RSSI_EVT */
#if BLE_INCLUDED == TRUE
#if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
    bta_dm_ble_update_duplicate_exceptional_list,/* BTA_DM_API_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_EVT */
#endif // ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
#endif
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    bta_dm_ble_gap_read_phy,                /* BTA_DM_API_READ_PHY_EVT */
    bta_dm_ble_gap_set_prefer_default_phy,  /* BTA_DM_API_SET_PER_DEF_PHY_EVT */
    bta_dm_ble_gap_set_prefer_phy,          /* BTA_DM_API_SET_PER_PHY_EVT */
#if (BLE_50_EXTEND_ADV_EN == TRUE)
    bta_dm_ble_gap_ext_adv_set_rand_addr,   /* BTA_DM_API_SET_EXT_ADV_RAND_ADDR_EVT */
    bta_dm_ble_gap_ext_adv_set_params,      /* BTA_DM_API_SET_EXT_ADV_PARAMS_EVT */
    bta_dm_ble_gap_config_ext_adv_data_raw, /* BTA_DM_API_CFG_ADV_DATA_RAW_EVT */
    bta_dm_ble_gap_start_ext_adv,           /* BTA_DM_API_EXT_ADV_ENABLE_EVT */
    bta_dm_ble_gap_ext_adv_set_remove,      /* BTA_DM_API_EXT_ADV_SET_REMOVE_EVT */
    bta_dm_ble_gap_ext_adv_set_clear,       /* BTA_DM_API_EXT_ADV_SET_CLEAR_EVT */
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)
#if (BLE_50_PERIODIC_ADV_EN == TRUE)
    bta_dm_ble_gap_periodic_adv_set_params, /* BTA_DM_API_PERIODIC_ADV_SET_PARAMS_EVT */
    bta_dm_ble_gap_periodic_adv_cfg_data_raw, /* BTA_DM_API_PERIODIC_ADV_CFG_DATA_EVT */
    bta_dm_ble_gap_periodic_adv_enable,     /* BTA_DM_API_PERIODIC_ADV_ENABLE_EVT */
#endif // #if (BLE_50_PERIODIC_ADV_EN == TRUE)
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
    bta_dm_ble_gap_periodic_adv_create_sync, /* BTA_DM_API_PERIODIC_ADV_SYNC_EVT */
    bta_dm_ble_gap_periodic_adv_sync_cancel, /* BTA_DM_API_PERIODIC_ADV_SYNC_CANCEL_EVT */
    bta_dm_ble_gap_periodic_adv_sync_terminate, /* BTA_DM_API_PERIODIC_ADV_SYNC_TERMINATE_EVT */
    bta_dm_ble_gap_periodic_adv_add_dev_to_list, /* BTA_DM_API_PERIODIC_ADV_ADD_DEV_TO_LSIT_EVT */
    bta_dm_ble_gap_periodic_adv_remove_dev_from_list, /* BTA_DM_API_PERIODIC_ADV_REMOVE_DEV_FROM_LSIT_EVT */
    bta_dm_ble_gap_periodic_adv_clear_dev,  /* BTA_DM_API_PERIODIC_ADV_CLEAR_DEV_EVT */
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
    bta_dm_ble_gap_set_ext_scan_params,     /* BTA_DM_API_SET_EXT_SCAN_PARAMS_EVT */
    bta_dm_ble_gap_ext_scan,                /* BTA_DM_API_START_EXT_SCAN_EVT */
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
    bta_dm_ble_gap_set_prefer_ext_conn_params, /* BTA_DM_API_SET_PERF_EXT_CONN_PARAMS_EVT */
    NULL,                                   /* BTA_DM_API_EXT_CONN_EVT */
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_50_DTM_TEST_EN == TRUE)
    bta_dm_ble_gap_dtm_enhance_tx_start,    /* BTA_DM_API_DTM_ENH_TX_START_EVT */
    bta_dm_ble_gap_dtm_enhance_rx_start,    /* BTA_DM_API_DTM_ENH_RX_START_EVT */
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    bta_dm_ble_gap_periodic_adv_recv_enable, /* BTA_DM_API_PERIODIC_ADV_RECV_ENABLE_EVT */
    bta_dm_ble_gap_periodic_adv_sync_trans,  /* BTA_DM_API_PERIODIC_ADV_SYNC_TRANS_EVT */
    bta_dm_ble_gap_periodic_adv_set_info_trans, /* BTA_DM_API_PERIODIC_ADV_SET_INFO_TRANS_EVT */
    bta_dm_ble_gap_set_periodic_adv_sync_trans_params, /* BTA_DM_API_SET_PERIODIC_ADV_SYNC_TRANS_PARAMS_EVT */
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#if BLE_INCLUDED == TRUE
#if (BLE_42_DTM_TEST_EN == TRUE)
    bta_dm_ble_gap_dtm_tx_start, /* BTA_DM_API_DTM_TX_START_EVT */
    bta_dm_ble_gap_dtm_rx_start, /* BTA_DM_API_DTM_RX_START_EVT */
#endif // #if (BLE_42_DTM_TEST_EN == TRUE)
#if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))
    bta_dm_ble_gap_dtm_stop, /* BTA_DM_API_DTM_STOP_EVT */
#endif // #if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))
    bta_dm_ble_gap_set_rpa_timeout, /* BTA_DM_API_SET_RPA_TIMEOUT_EVT */
    bta_dm_ble_gap_add_dev_to_resolving_list, /* BTA_DM_API_ADD_DEV_TO_RESOLVING_LIST_EVT */
    bta_dm_ble_gap_set_privacy_mode, /* BTA_DM_API_SET_PRIVACY_MODE_EVT */
    bta_dm_read_ble_channel_map,        /* BTA_DM_API_BLE_READ_CH_MAP_EVT */
#endif
#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
    bta_dm_ble_big_create,             /* BTA_DM_API_ISO_BIG_CREATE_EVT */
    bta_dm_ble_big_create_test,        /* BTA_DM_API_ISO_BIG_CREATE_TEST_EVT */
    bta_dm_ble_big_terminate,          /* BTA_DM_API_ISO_BIG_TERMINATE_EVT */
#endif// #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    bta_dm_ble_big_sync_create,        /* BTA_DM_API_ISO_BIG_SYNC_CREATE_EVT */
    bta_dm_ble_big_sync_terminate,     /* BTA_DM_API_ISO_BIG_SYNC_TERMINATE_EVT */
#endif// #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    bta_dm_ble_iso_set_data_path,      /* BTA_DM_API_ISO_SET_DATA_PATH_EVT */
    bta_dm_ble_iso_remove_data_path,   /* BTA_DM_API_ISO_REMOVE_DATA_PATH_EVT */
    bta_dm_ble_iso_read_tx_sync,       /* BTA_DM_API_ISO_READ_TX_SYNC_EVT */
    bta_dm_ble_iso_read_link_quality,  /* BTA_DM_API_ISO_READ_LINK_QUALITY_EVT */
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
    bta_dm_ble_set_cig_params,         /* BTA_DM_API_SET_CIG_PARAMS_EVT */
    bta_dm_ble_set_cig_params_test,    /* BTA_DM_API_SET_CIG_PARAMS_TEST_EVT */
    bta_dm_ble_create_cis,             /* BTA_DM_API_CREATE_CIS_EVT */
    bta_dm_ble_remove_cig,             /* BTA_DM_API_REMOVE_CIG_PARAMS_EVT*/
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
    bta_dm_ble_accept_cis_req,         /* BTA_DM_API_ACCEPT_CIS_REQ_PARAMS_EVT */
    bta_dm_ble_reject_cis_req,         /* BTA_DM_API_REJECT_CIS_REQ_PARAMS_EVT */
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
    bta_dm_ble_discon_cis,            /* BTA_DM_API_DISCON_CIS_EVT */
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
#endif // #if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    bta_dm_ble_set_cte_trans_params, /* BTA_DM_API_CTE_SET_TRANS_PARAMS */
    bta_dm_ble_set_cte_trans_enable, /* BTA_DM_API_CTE_SET_TRANS_ENABLE */
    bta_dm_ble_set_iq_sampling_en, /* BTA_DM_API_CTE_SET_IQ_SAMPLING_EN */
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    bta_dm_ble_set_conn_cte_recv_params, /* BTA_DM_API_CTE_SET_CONN_CTE_RECV_PARAMS */
    bta_dm_ble_set_conn_trans_params, /* BTA_DM_API_CTE_SET_CONN_CTE_TRANS_PARAMS */
    bta_dm_ble_set_conn_cte_req_en, /* BTA_DM_API_CTE_SET_CONN_CTE_REQUEST_EN */
    bta_dm_ble_set_conn_cte_rsp_en, /* BTA_DM_API_CTE_SET_CONN_CTE_RESPONSE_EN */
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    bta_dm_ble_read_cte_ant_infor, /* BTA_DM_API_CTE_READ_ANTENNA_INFOR */
#endif // #if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
    bta_dm_api_enh_read_trans_power_level, /* BTA_DM_API_ENH_READ_TRANS_POWER_LEVEL */
    bta_dm_api_read_rem_trans_power_level, /* BTA_DM_API_READ_REM_TRANS_POWER_LEVEL */
    bta_dm_api_set_path_loss_report_params, /* BTA_DM_API_SET_PATH_LOSS_REPORT_PARAMS */
    bta_dm_api_set_path_loss_reporting_en, /* BTA_DM_API_SET_PATH_LOSS_REPORTING_EN */
    bta_dm_api_set_trans_power_reporting_en, /* BTA_DM_API_SET_TRANS_POWER_REPORTING_EN */
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BLE_FEAT_CONN_SUBRATING == TRUE)
    bta_dm_api_set_default_subrate, /* BTA_DM_API_SET_DEFALT_SUBRATE */
    bta_dm_api_subrate_request, /* BTA_DM_API_SUBRATE_REQUEST */
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    bta_dm_ble_set_host_feature,   /* BTA_DM_API_SET_HOST_FEATURE_EVT */
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    bta_dm_api_set_periodic_adv_subevt_data,   /* BTA_DM_API_SET_PA_SUBEVT_DATA */
    bta_dm_api_set_periodic_adv_response_data, /* BTA_DM_API_SET_PA_RSP_DATA */
    bta_dm_api_set_periodic_sync_subevt,       /* BTA_DM_API_SET_PA_SYNC_SUBEVT */
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
    bta_dm_api_cs_read_local_supported_caps,          /* BTA_DM_API_CS_READ_LOCAL_SUPPORTED_CAPS */
    bta_dm_api_cs_read_remote_supported_caps,         /* BTA_DM_API_CS_READ_REMOTE_SUPPORTED_CAPS */
    bta_dm_api_cs_write_cached_remote_supported_caps, /* BTA_DM_API_CS_WRITE_CACHED_REMOTE_SUPPORTED_CAPS */
    bta_dm_api_cs_security_enable,                    /* BTA_DM_API_CS_SECURITY_ENABLE */
    bta_dm_api_cs_set_default_settings,               /* BTA_DM_API_CS_SET_DEFAULT_SETTINGS */
    bta_dm_api_cs_read_remote_fae_table,              /* BTA_DM_API_CS_READ_REMOTE_FAE_TABLE */
    bta_dm_api_cs_write_cached_remote_fae_table,      /* BTA_DM_API_CS_WRITE_CACHED_REMOTE_FAE_TABLE */
    bta_dm_api_cs_create_config,                      /* BTA_DM_API_CS_CREATE_CONFIG */
    bta_dm_api_cs_remove_config,                      /* BTA_DM_API_CS_REMOVE_CONFIG */
    bta_dm_api_cs_set_channel_classification,         /* BTA_DM_API_CS_SET_CAHNNEL_CLASSIFICATION */
    bta_dm_api_cs_set_procedure_params,               /* BTA_DM_API_CS_SET_PROCEDURE_PARAMS */
    bta_dm_api_cs_procedure_enable,                   /* BTA_DM_API_CS_PROCEDURE_ENABLE */
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
};



/* state machine action enumeration list */
enum {
    BTA_DM_API_SEARCH,                  /* 0 bta_dm_search_start */
    BTA_DM_API_SEARCH_CANCEL,           /* 1 bta_dm_search_cancel */
#if (SDP_INCLUDED == TRUE)
    BTA_DM_API_DISCOVER,                /* 2 bta_dm_discover */
#endif  ///SDP_INCLUDED == TRUE
    BTA_DM_INQUIRY_CMPL,                /* 3 bta_dm_inq_cmpl */
    BTA_DM_REMT_NAME,                   /* 4 bta_dm_rmt_name */
#if (SDP_INCLUDED == TRUE)
    BTA_DM_SDP_RESULT,                  /* 5 bta_dm_sdp_result */
#endif  ///SDP_INCLUDED == TRUE
    BTA_DM_SEARCH_CMPL,                 /* 6 bta_dm_search_cmpl*/
#if (SDP_INCLUDED == TRUE)
    BTA_DM_FREE_SDP_DB,                 /* 7 bta_dm_free_sdp_db */
#endif  ///SDP_INCLUDED == TRUE
    BTA_DM_DISC_RESULT,                 /* 8 bta_dm_disc_result */
    BTA_DM_SEARCH_RESULT,               /* 9 bta_dm_search_result */
    BTA_DM_QUEUE_SEARCH,                /* 10 bta_dm_queue_search */
#if (SDP_INCLUDED == TRUE)
    BTA_DM_QUEUE_DISC,                  /* 11 bta_dm_queue_disc */
#endif  ///SDP_INCLUDED == TRUE
    BTA_DM_SEARCH_CLEAR_QUEUE,          /* 12 bta_dm_search_clear_queue */
    BTA_DM_SEARCH_CANCEL_CMPL,          /* 13 bta_dm_search_cancel_cmpl */
    BTA_DM_SEARCH_CANCEL_NOTIFY,        /* 14 bta_dm_search_cancel_notify */
#if (SDP_INCLUDED == TRUE)
    BTA_DM_SEARCH_CANCEL_TRANSAC_CMPL,  /* 15 bta_dm_search_cancel_transac_cmpl */
#endif  ///SDP_INCLUDED == TRUE
    BTA_DM_DISC_RMT_NAME,               /* 16 bta_dm_disc_rmt_name */
#if (SDP_INCLUDED == TRUE)
    BTA_DM_API_DI_DISCOVER,             /* 17 bta_dm_di_disc */
#endif  ///SDP_INCLUDED == TRUE
#if BLE_INCLUDED == TRUE && SDP_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && GATTC_INCLUDED == TRUE
    BTA_DM_CLOSE_GATT_CONN,             /* 18 bta_dm_close_gatt_conn */
#endif /* BLE_INCLUDED == TRUE && SDP_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && GATTC_INCLUDED == TRUE */
    BTA_DM_SEARCH_NUM_ACTIONS           /* 19 */
};


/* action function list */
const tBTA_DM_ACTION bta_dm_search_action[] = {

    bta_dm_search_start,                /* 0 BTA_DM_API_SEARCH */
    bta_dm_search_cancel,               /* 1 BTA_DM_API_SEARCH_CANCEL */
#if (SDP_INCLUDED == TRUE)
    bta_dm_discover,                    /* 2 BTA_DM_API_DISCOVER */
#endif  ///SDP_INCLUDED == TRUE
    bta_dm_inq_cmpl,                    /* 3 BTA_DM_INQUIRY_CMPL */
    bta_dm_rmt_name,                    /* 4 BTA_DM_REMT_NAME */
#if (SDP_INCLUDED == TRUE)
    bta_dm_sdp_result,                  /* 5 BTA_DM_SDP_RESULT */
#endif  ///SDP_INCLUDED == TRUE
    bta_dm_search_cmpl,                 /* 6 BTA_DM_SEARCH_CMPL */
#if (SDP_INCLUDED == TRUE)
    bta_dm_free_sdp_db,                 /* 7 BTA_DM_FREE_SDP_DB */
#endif  ///SDP_INCLUDED == TRUE
    bta_dm_disc_result,                 /* 8 BTA_DM_DISC_RESULT */
    bta_dm_search_result,               /* 9 BTA_DM_SEARCH_RESULT */
    bta_dm_queue_search,                /* 10 BTA_DM_QUEUE_SEARCH */
#if (SDP_INCLUDED == TRUE)
    bta_dm_queue_disc,                  /* 11 BTA_DM_QUEUE_DISC */
#endif  ///SDP_INCLUDED == TRUE
    bta_dm_search_clear_queue,          /* 12 BTA_DM_SEARCH_CLEAR_QUEUE */
    bta_dm_search_cancel_cmpl,          /* 13 BTA_DM_SEARCH_CANCEL_CMPL */
    bta_dm_search_cancel_notify,        /* 14 BTA_DM_SEARCH_CANCEL_NOTIFY */
#if (SDP_INCLUDED == TRUE)
    bta_dm_search_cancel_transac_cmpl,  /* 15 BTA_DM_SEARCH_CANCEL_TRANSAC_CMPL */
#endif  ///SDP_INCLUDED == TRUE
    bta_dm_disc_rmt_name,               /* 16 BTA_DM_DISC_RMT_NAME */
#if (SDP_INCLUDED == TRUE)
    bta_dm_di_disc                      /* 17 BTA_DM_API_DI_DISCOVER */
#endif  ///SDP_INCLUDED == TRUE
#if BLE_INCLUDED == TRUE && SDP_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && GATTC_INCLUDED == TRUE
    , bta_dm_close_gatt_conn
#endif
};

#define BTA_DM_SEARCH_IGNORE       BTA_DM_SEARCH_NUM_ACTIONS
/* state table information */
#define BTA_DM_SEARCH_ACTIONS              2       /* number of actions */
#define BTA_DM_SEARCH_NEXT_STATE           2       /* position of next state */
#define BTA_DM_SEARCH_NUM_COLS             3       /* number of columns in state tables */



/* state table for listen state */
const UINT8 bta_dm_search_idle_st_table[][BTA_DM_SEARCH_NUM_COLS] = {

    /* Event                        Action 1                            Action 2                    Next State */
    /* API_SEARCH */            {BTA_DM_API_SEARCH,                BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE},
    /* API_SEARCH_CANCEL */     {BTA_DM_SEARCH_CANCEL_NOTIFY,      BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
#if (SDP_INCLUDED == TRUE)
    /* API_SEARCH_DISC */       {BTA_DM_API_DISCOVER,              BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE},
#endif  ///SDP_INCLUDED == TRUE
    /* INQUIRY_CMPL */          {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
    /* REMT_NAME_EVT */         {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
#if (SDP_INCLUDED == TRUE)
    /* SDP_RESULT_EVT */        {BTA_DM_FREE_SDP_DB,               BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
#endif  ///SDP_INCLUDED == TRUE
    /* SEARCH_CMPL_EVT */       {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
    /* DISCV_RES_EVT */         {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
#if (SDP_INCLUDED == TRUE)
    /* API_DI_DISCOVER_EVT */   {BTA_DM_API_DI_DISCOVER,           BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE},
#endif  ///SDP_INCLUDED == TRUE
#if BLE_INCLUDED == TRUE && SDP_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && GATTC_INCLUDED == TRUE
    // #if BLE_INCLUDED == TRUE
    /* DISC_CLOSE_TOUT_EVT */   {BTA_DM_CLOSE_GATT_CONN,           BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
#endif
};
const UINT8 bta_dm_search_search_active_st_table[][BTA_DM_SEARCH_NUM_COLS] = {

    /* Event                        Action 1                            Action 2                    Next State */
    /* API_SEARCH */            {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE},
    /* API_SEARCH_CANCEL */     {BTA_DM_API_SEARCH_CANCEL,         BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_CANCELLING},
    /* API_SEARCH_DISC */       {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE},
    /* INQUIRY_CMPL */          {BTA_DM_INQUIRY_CMPL,              BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE},
    /* REMT_NAME_EVT */         {BTA_DM_REMT_NAME,                 BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE},
#if (SDP_INCLUDED == TRUE)
    /* SDP_RESULT_EVT */        {BTA_DM_SDP_RESULT,                BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE},
#endif  ///SDP_INCLUDED == TRUE
    /* SEARCH_CMPL_EVT */       {BTA_DM_SEARCH_CMPL,               BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
    /* DISCV_RES_EVT */         {BTA_DM_SEARCH_RESULT,             BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE},
    /* API_DI_DISCOVER_EVT */   {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE}
#if BLE_INCLUDED == TRUE && SDP_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && GATTC_INCLUDED == TRUE
    // #if BLE_INCLUDED == TRUE
    /* DISC_CLOSE_TOUT_EVT */   , {BTA_DM_CLOSE_GATT_CONN,          BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_ACTIVE}
#endif

};

const UINT8 bta_dm_search_search_cancelling_st_table[][BTA_DM_SEARCH_NUM_COLS] = {

    /* Event                        Action 1                            Action 2                    Next State */
    /* API_SEARCH */            {BTA_DM_QUEUE_SEARCH,               BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_CANCELLING},
    /* API_SEARCH_CANCEL */     {BTA_DM_SEARCH_CLEAR_QUEUE,         BTA_DM_SEARCH_CANCEL_NOTIFY,   BTA_DM_SEARCH_CANCELLING},
#if (SDP_INCLUDED == TRUE)
    /* API_SEARCH_DISC */       {BTA_DM_QUEUE_DISC,                 BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_CANCELLING},
#endif  ///SDP_INCLUDED == TRUE
    /* INQUIRY_CMPL */          {BTA_DM_SEARCH_CANCEL_CMPL,         BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
#if (SDP_INCLUDED == TRUE)
    /* REMT_NAME_EVT */         {BTA_DM_SEARCH_CANCEL_TRANSAC_CMPL, BTA_DM_SEARCH_CANCEL_CMPL,     BTA_DM_SEARCH_IDLE},
    /* SDP_RESULT_EVT */        {BTA_DM_SEARCH_CANCEL_TRANSAC_CMPL, BTA_DM_SEARCH_CANCEL_CMPL,     BTA_DM_SEARCH_IDLE},
    /* SEARCH_CMPL_EVT */       {BTA_DM_SEARCH_CANCEL_TRANSAC_CMPL, BTA_DM_SEARCH_CANCEL_CMPL,     BTA_DM_SEARCH_IDLE},
    /* DISCV_RES_EVT */         {BTA_DM_SEARCH_CANCEL_TRANSAC_CMPL, BTA_DM_SEARCH_CANCEL_CMPL,     BTA_DM_SEARCH_IDLE},
#endif  ///SDP_INCLUDED == TRUE
    /* API_DI_DISCOVER_EVT */   {BTA_DM_SEARCH_IGNORE,              BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_CANCELLING}
#if BLE_INCLUDED == TRUE
    /* DISC_CLOSE_TOUT_EVT */   , {BTA_DM_SEARCH_IGNORE,              BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_CANCELLING}
#endif

};

const UINT8 bta_dm_search_disc_active_st_table[][BTA_DM_SEARCH_NUM_COLS] = {

    /* Event                        Action 1                            Action 2                    Next State */
    /* API_SEARCH */            {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE},
    /* API_SEARCH_CANCEL */     {BTA_DM_SEARCH_CANCEL_NOTIFY,      BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_CANCELLING},
    /* API_SEARCH_DISC */       {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE},
    /* INQUIRY_CMPL */          {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE},
    /* REMT_NAME_EVT */         {BTA_DM_DISC_RMT_NAME,             BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE},
#if (SDP_INCLUDED == TRUE)
    /* SDP_RESULT_EVT */        {BTA_DM_SDP_RESULT,                BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE},
#endif  ///SDP_INCLUDED == TRUE
    /* SEARCH_CMPL_EVT */       {BTA_DM_SEARCH_CMPL,               BTA_DM_SEARCH_IGNORE,          BTA_DM_SEARCH_IDLE},
    /* DISCV_RES_EVT */         {BTA_DM_DISC_RESULT,               BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE},
    /* API_DI_DISCOVER_EVT */   {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE}

#if BLE_INCLUDED == TRUE
    /* DISC_CLOSE_TOUT_EVT */   , {BTA_DM_SEARCH_IGNORE,             BTA_DM_SEARCH_IGNORE,          BTA_DM_DISCOVER_ACTIVE}
#endif
};

typedef const UINT8 (*tBTA_DM_ST_TBL)[BTA_DM_SEARCH_NUM_COLS];

/* state table */
const tBTA_DM_ST_TBL bta_dm_search_st_tbl[] = {
    bta_dm_search_idle_st_table,
    bta_dm_search_search_active_st_table,
    bta_dm_search_search_cancelling_st_table,
    bta_dm_search_disc_active_st_table
};


/*******************************************************************************
**
** Function         bta_dm_sm_disable
**
** Description     unregister BTA DM
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_sm_disable(void)
{
    bta_sys_deregister( BTA_ID_DM );
}

void bta_dm_sm_deinit(void)
{
    memset(&bta_dm_cb, 0, sizeof(tBTA_DM_CB));
    memset(&bta_dm_search_cb, 0, sizeof(tBTA_DM_SEARCH_CB));
#if (CLASSIC_BT_INCLUDED == TRUE)
    memset(&bta_dm_di_cb, 0, sizeof(tBTA_DM_DI_CB));
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
#if BTA_DYNAMIC_MEMORY
    FREE_AND_RESET(bta_dm_cb_ptr);
    FREE_AND_RESET(bta_dm_search_cb_ptr);
#if (CLASSIC_BT_INCLUDED == TRUE)
    FREE_AND_RESET(bta_dm_di_cb_ptr);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
#endif /* #if BTA_DYNAMIC_MEMORY */
}


/*******************************************************************************
**
** Function         bta_dm_sm_execute
**
** Description      State machine event handling function for DM
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_dm_sm_execute(BT_HDR *p_msg)
{
    UINT16  event = p_msg->event & 0x00ff;

    APPL_TRACE_EVENT("bta_dm_sm_execute event:0x%x", event);

    /* execute action functions */
    if (event < BTA_DM_NUM_ACTIONS) {
        (*bta_dm_action[event])( (tBTA_DM_MSG *) p_msg);
    }

    return TRUE;
}

void BTA_DmCoexEventTrigger(uint32_t event)
{
    switch(event) {
    case BTA_COEX_EVT_SCAN_STARTED:
    case BTA_COEX_EVT_SCAN_STOPPED:
    case BTA_COEX_EVT_SNIFF_ENTER:
    case BTA_COEX_EVT_SNIFF_EXIT:
    case BTA_COEX_EVT_A2DP_PAUSED_ENTER:
    case BTA_COEX_EVT_A2DP_PAUSED_EXIT:
    case BTA_COEX_EVT_ACL_CONNECTED:
    case BTA_COEX_EVT_ACL_DISCONNECTED:
        break;
    case BTA_COEX_EVT_STREAMING_STARTED:
#if (ESP_COEX_VSC_INCLUDED == TRUE)
        BTM_ConfigCoexStatus(BTM_COEX_OP_SET, BTM_COEX_TYPE_BT, BTM_COEX_BT_ST_A2DP_STREAMING);
        BTM_ConfigCoexStatus(BTM_COEX_OP_CLEAR, BTM_COEX_TYPE_BT, BTM_COEX_BT_ST_A2DP_PAUSED);
#endif
        break;
    case BTA_COEX_EVT_STREAMING_STOPPED:
#if (ESP_COEX_VSC_INCLUDED == TRUE)
        BTM_ConfigCoexStatus(BTM_COEX_OP_CLEAR, BTM_COEX_TYPE_BT, BTM_COEX_BT_ST_A2DP_STREAMING);
        BTM_ConfigCoexStatus(BTM_COEX_OP_CLEAR, BTM_COEX_TYPE_BT, BTM_COEX_BT_ST_A2DP_PAUSED);
#endif
        break;
    default:
        break;
    }
}

/*******************************************************************************
**
** Function         bta_dm_sm_search_disable
**
** Description     unregister BTA SEARCH DM
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_search_sm_disable(void)
{
    bta_sys_deregister( BTA_ID_DM_SEARCH );

}


/*******************************************************************************
**
** Function         bta_dm_search_sm_execute
**
** Description      State machine event handling function for DM
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_dm_search_sm_execute(BT_HDR *p_msg)
{
    tBTA_DM_ST_TBL      state_table;
    UINT8               action;
    int                 i;

    APPL_TRACE_EVENT("bta_dm_search_sm_execute state:%d, event:0x%x",
                     bta_dm_search_cb.state, p_msg->event);

    /* look up the state table for the current state */
    state_table = bta_dm_search_st_tbl[bta_dm_search_cb.state];

    bta_dm_search_cb.state = state_table[p_msg->event & 0x00ff][BTA_DM_SEARCH_NEXT_STATE];


    /* execute action functions */
    for (i = 0; i < BTA_DM_SEARCH_ACTIONS; i++) {
        if ((action = state_table[p_msg->event & 0x00ff][i]) != BTA_DM_SEARCH_IGNORE) {
            (*bta_dm_search_action[action])( (tBTA_DM_MSG *) p_msg);
        } else {
            break;
        }
    }
    return TRUE;
}
