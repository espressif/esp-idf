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

#include "bta_api.h"
#include "bta_sys.h"
#include "bta_dm_int.h"


/*****************************************************************************
** Constants and types
*****************************************************************************/

#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_DM_CB  bta_dm_cb;
tBTA_DM_SEARCH_CB bta_dm_search_cb;
tBTA_DM_DI_CB       bta_dm_di_cb;
#endif


#define BTA_DM_NUM_ACTIONS  (BTA_DM_MAX_EVT & 0x00ff)

/* type for action functions */
typedef void (*tBTA_DM_ACTION)(tBTA_DM_MSG *p_data);

/* action function list */
const tBTA_DM_ACTION bta_dm_action[BTA_DM_MAX_EVT] = {

    /* device manager local device API events */
    bta_dm_enable,                          /* 0  BTA_DM_API_ENABLE_EVT */
    bta_dm_disable,                         /* 1  BTA_DM_API_DISABLE_EVT */
    bta_dm_set_dev_name,                    /* 2  BTA_DM_API_SET_NAME_EVT */
    bta_dm_set_visibility,                  /* 3  BTA_DM_API_SET_VISIBILITY_EVT */
    bta_dm_acl_change,                      /* 8  BTA_DM_ACL_CHANGE_EVT */
    bta_dm_add_device,                      /* 9  BTA_DM_API_ADD_DEVICE_EVT */
    bta_dm_close_acl,                       /* 10  BTA_DM_API_ADD_DEVICE_EVT */
#if (SMP_INCLUDED == TRUE)
    /* security API events */
    bta_dm_bond,                            /* 11  BTA_DM_API_BOND_EVT */
    bta_dm_bond_cancel,                     /* 12  BTA_DM_API_BOND_CANCEL_EVT */
    bta_dm_pin_reply,                       /* 13 BTA_DM_API_PIN_REPLY_EVT */
#endif  ///SMP_INCLUDED == TRUE
#if (BTM_SSR_INCLUDED == TRUE)
    /* power manger events */
    bta_dm_pm_btm_status,                   /* 16 BTA_DM_PM_BTM_STATUS_EVT */
    bta_dm_pm_timer,                        /* 17 BTA_DM_PM_TIMER_EVT*/
#endif  ///BTM_SSR_INCLUDED == TRUE
    /* simple pairing events */
#if (SMP_INCLUDED == TRUE)
    bta_dm_confirm,                         /* 18 BTA_DM_API_CONFIRM_EVT */
    bta_dm_set_encryption,                  /* BTA_DM_API_SET_ENCRYPTION_EVT */
#endif  ///SMP_INCLUDED == TRUE
#if (BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
    bta_dm_loc_oob,                         /* 20 BTA_DM_API_LOC_OOB_EVT */
    bta_dm_ci_io_req_act,                   /* 21 BTA_DM_CI_IO_REQ_EVT */
    bta_dm_ci_rmt_oob_act,                  /* 22 BTA_DM_CI_RMT_OOB_EVT */
#endif /* BTM_OOB_INCLUDED */


#if BLE_INCLUDED == TRUE
#if SMP_INCLUDED == TRUE
    bta_dm_add_blekey,                      /*  BTA_DM_API_ADD_BLEKEY_EVT           */
    bta_dm_add_ble_device,                  /*  BTA_DM_API_ADD_BLEDEVICE_EVT        */
    bta_dm_ble_passkey_reply,               /*  BTA_DM_API_BLE_PASSKEY_REPLY_EVT    */
    bta_dm_ble_confirm_reply,               /*  BTA_DM_API_BLE_CONFIRM_REPLY_EVT    */
    bta_dm_security_grant,
#endif  ///SMP_INCLUDED == TRUE
    bta_dm_ble_set_bg_conn_type,
    bta_dm_ble_set_conn_params,             /* BTA_DM_API_BLE_CONN_PARAM_EVT */
    bta_dm_ble_set_conn_scan_params,        /* BTA_DM_API_BLE_CONN_SCAN_PARAM_EVT */
    bta_dm_ble_set_scan_params,             /* BTA_DM_API_BLE_SCAN_PARAM_EVT */
    bta_dm_ble_set_scan_fil_params,         /* BTA_DM_API_BLE_SCAN_FIL_PARAM_EVT */
    bta_dm_ble_observe,                     /* BTA_DM_API_BLE_OBSERVE_EVT*/
    bta_dm_ble_update_conn_params,          /* BTA_DM_API_UPDATE_CONN_PARAM_EVT */
    /* This handler function added by
       Yulong at 2016/9/9 to support the
       random address setting for the APP */
    bta_dm_ble_set_rand_address,            /* BTA_DM_API_SET_RAND_ADDR_EVT*/
    /* This handler function added by
       Yulong at 2016/10/19 to support
       stop the ble advertising setting
       by the APP */
    bta_dm_ble_stop_advertising,            /* BTA_DM_API_BLE_STOP_ADV_EVT*/
#if BLE_PRIVACY_SPT == TRUE
    bta_dm_ble_config_local_privacy,        /* BTA_DM_API_LOCAL_PRIVACY_EVT */
#endif
    bta_dm_ble_set_adv_params,              /* BTA_DM_API_BLE_ADV_PARAM_EVT */
    bta_dm_ble_set_adv_params_all,          /* BTA_DM_API_BLE_ADV_PARAM_All_EVT */
    bta_dm_ble_set_adv_config,              /* BTA_DM_API_BLE_SET_ADV_CONFIG_EVT */
    /* New function to allow set raw adv
       data to HCI */
    bta_dm_ble_set_adv_config_raw,          /* BTA_DM_API_BLE_SET_ADV_CONFIG_RAW_EVT */
    bta_dm_ble_set_scan_rsp,                /* BTA_DM_API_BLE_SET_SCAN_RSP_EVT */
    /* New function to allow set raw scan 
       response data to HCI */
    bta_dm_ble_set_scan_rsp_raw,            /* BTA_DM_API_BLE_SET_SCAN_RSP_RAW_EVT */
    bta_dm_ble_broadcast,                   /* BTA_DM_API_BLE_BROADCAST_EVT */
    bta_dm_ble_set_data_length,             /* BTA_DM_API_SET_DATA_LENGTH_EVT */
#if BLE_ANDROID_CONTROLLER_SCAN_FILTER == TRUE
    bta_dm_cfg_filter_cond,                 /* BTA_DM_API_CFG_FILTER_COND_EVT */
    bta_dm_scan_filter_param_setup,         /* BTA_DM_API_SCAN_FILTER_SETUP_EVT */
    bta_dm_enable_scan_filter,              /* BTA_DM_API_SCAN_FILTER_ENABLE_EVT */
#endif
    bta_dm_ble_multi_adv_enb,               /*  BTA_DM_API_BLE_MULTI_ADV_ENB_EVT*/
    bta_dm_ble_multi_adv_upd_param,         /*  BTA_DM_API_BLE_MULTI_ADV_PARAM_UPD_EVT */
    bta_dm_ble_multi_adv_data,              /*  BTA_DM_API_BLE_MULTI_ADV_DATA_EVT */
    btm_dm_ble_multi_adv_disable,           /*  BTA_DM_API_BLE_MULTI_ADV_DISABLE_EVT */
    bta_dm_ble_setup_storage,               /* BTA_DM_API_BLE_SETUP_STORAGE_EVT */
    bta_dm_ble_enable_batch_scan,           /* BTA_DM_API_BLE_ENABLE_BATCH_SCAN_EVT */
    bta_dm_ble_disable_batch_scan,          /* BTA_DM_API_BLE_DISABLE_BATCH_SCAN_EVT */
    bta_dm_ble_read_scan_reports,           /* BTA_DM_API_BLE_READ_SCAN_REPORTS_EVT */
    bta_dm_ble_track_advertiser,            /* BTA_DM_API_BLE_TRACK_ADVERTISER_EVT */
    bta_dm_ble_get_energy_info,             /* BTA_DM_API_BLE_ENERGY_INFO_EVT */
    bta_dm_ble_disconnect,                  /* BTA_DM_API_BLE_DISCONNECT_EVT */
#endif

    bta_dm_enable_test_mode,                /*  BTA_DM_API_ENABLE_TEST_MODE_EVT     */
    bta_dm_disable_test_mode,               /*  BTA_DM_API_DISABLE_TEST_MODE_EVT    */
    bta_dm_execute_callback,                /*  BTA_DM_API_EXECUTE_CBACK_EVT        */

    bta_dm_remove_all_acl,                  /* BTA_DM_API_REMOVE_ALL_ACL_EVT */
    bta_dm_remove_device,                   /* BTA_DM_API_REMOVE_DEVICE_EVT */
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
void bta_dm_sm_disable( )
{
    bta_sys_deregister( BTA_ID_DM );
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
void bta_dm_search_sm_disable( )
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

