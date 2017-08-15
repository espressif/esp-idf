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
 *  This file contains the action functions for device manager state
 *  machine.
 *
 ******************************************************************************/

#include "bt_target.h"
#include "bt_types.h"
#include "bta_sys.h"
#include "bta_api.h"
#include "bta_dm_int.h"
#include "bta_dm_co.h"
#include "btm_api.h"
#include "btm_int.h"
#include "btu.h"
#include "sdp_api.h"
#include "l2c_api.h"
#include "utl.h"
#include "gap_api.h"    /* For GAP_BleReadPeerPrefConnParams */
#include <string.h>
#include "controller.h"

#define LOG_TAG "bt_bta_dm"
// #include "osi/include/log.h"

#if (GAP_INCLUDED == TRUE)
#include "gap_api.h"
#endif

static void bta_dm_inq_results_cb (tBTM_INQ_RESULTS *p_inq, UINT8 *p_eir);
static void bta_dm_inq_cmpl_cb (void *p_result);
static void bta_dm_service_search_remname_cback (BD_ADDR bd_addr, DEV_CLASS dc, BD_NAME bd_name);
static void bta_dm_remname_cback (tBTM_REMOTE_DEV_NAME *p_remote_name);
#if (SDP_INCLUDED == TRUE)
static void bta_dm_find_services ( BD_ADDR bd_addr);
#endif  ///SDP_INCLUDED == TRUE
static void bta_dm_discover_next_device(void);
#if (SDP_INCLUDED == TRUE)
static void bta_dm_sdp_callback (UINT16 sdp_status);
#endif  ///SDP_INCLUDED == TRUE
#if (SMP_INCLUDED == TRUE)
static UINT8 bta_dm_authorize_cback (BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name, UINT8 *service_name, UINT8 service_id, BOOLEAN is_originator);
static UINT8 bta_dm_pin_cback (BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name, BOOLEAN min_16_digit);
static UINT8 bta_dm_new_link_key_cback(BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name, LINK_KEY key, UINT8 key_type);
static UINT8 bta_dm_authentication_complete_cback(BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name, int result);
#endif  ///SMP_INCLUDED == TRUE
static void bta_dm_local_name_cback(BD_ADDR bd_addr);
static BOOLEAN bta_dm_check_av(UINT16 event);
static void bta_dm_bl_change_cback (tBTM_BL_EVENT_DATA *p_data);


static void bta_dm_policy_cback(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr);

/* Extended Inquiry Response */
#if (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE && SMP_INCLUDED == TRUE)
static UINT8 bta_dm_sp_cback (tBTM_SP_EVT event, tBTM_SP_EVT_DATA *p_data);
#endif /* (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE) */

static void bta_dm_set_eir (char *local_name);
#if (SDP_INCLUDED == TRUE)
static void bta_dm_eir_search_services( tBTM_INQ_RESULTS  *p_result,
                                        tBTA_SERVICE_MASK *p_services_to_search,
                                        tBTA_SERVICE_MASK *p_services_found);
#endif  ///SDP_INCLUDED == TRUE
static void bta_dm_search_timer_cback (TIMER_LIST_ENT *p_tle);
static void bta_dm_disable_conn_down_timer_cback (TIMER_LIST_ENT *p_tle);
static void bta_dm_rm_cback(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
static void bta_dm_adjust_roles(BOOLEAN delay_role_switch);
#if (SDP_INCLUDED == TRUE || SMP_INCLUDED == TRUE)
static char *bta_dm_get_remname(void);
#endif  ///SDP_INCLUDED == TRUE || SMP_INCLUDED == TRUE
#if (SMP_INCLUDED == TRUE)
static void bta_dm_bond_cancel_complete_cback(tBTM_STATUS result);
#endif  ///SMP_INCLUDED == TRUE
#if (SDP_INCLUDED == TRUE)
static BOOLEAN bta_dm_read_remote_device_name (BD_ADDR bd_addr, tBT_TRANSPORT transport);
static void bta_dm_discover_device(BD_ADDR remote_bd_addr);
#endif  ///SDP_INCLUDED == TRUE
static void bta_dm_sys_hw_cback( tBTA_SYS_HW_EVT status );
static void bta_dm_disable_search_and_disc(void);

#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
#if ((defined SMP_INCLUDED) && (SMP_INCLUDED == TRUE))
static UINT8 bta_dm_ble_smp_cback (tBTM_LE_EVT event, BD_ADDR bda, tBTM_LE_EVT_DATA *p_data);
static void bta_dm_ble_id_key_cback (UINT8 key_type, tBTM_BLE_LOCAL_KEYS *p_key);
#endif  ///SMP_INCLUDED == TRUE
#if ((defined BTA_GATT_INCLUDED) &&  (BTA_GATT_INCLUDED == TRUE) && SDP_INCLUDED == TRUE)
#if (GATTC_INCLUDED == TRUE)
static void bta_dm_gattc_register(void);
static void btm_dm_start_gatt_discovery(BD_ADDR bd_addr);
static void bta_dm_cancel_gatt_discovery(BD_ADDR bd_addr);
static void bta_dm_gattc_callback(tBTA_GATTC_EVT event, tBTA_GATTC *p_data);
#endif // (GATTC_INCLUDED == TRUE)
extern tBTA_DM_CONTRL_STATE bta_dm_pm_obtain_controller_state(void);
#endif

#if BLE_VND_INCLUDED == TRUE
static void bta_dm_ctrl_features_rd_cmpl_cback(tBTM_STATUS result);
#endif

#ifndef BTA_DM_BLE_ADV_CHNL_MAP
#define BTA_DM_BLE_ADV_CHNL_MAP (BTM_BLE_ADV_CHNL_37|BTM_BLE_ADV_CHNL_38|BTM_BLE_ADV_CHNL_39)
#endif
#endif
#if (SMP_INCLUDED == TRUE)
static void bta_dm_remove_sec_dev_entry(BD_ADDR remote_bd_addr);
#endif  ///SMP_INCLUDED == TRUE
static void bta_dm_observe_results_cb(tBTM_INQ_RESULTS *p_inq, UINT8 *p_eir);
static void bta_dm_observe_cmpl_cb(void *p_result);
static void bta_dm_delay_role_switch_cback(TIMER_LIST_ENT *p_tle);
extern void sdpu_uuid16_to_uuid128(UINT16 uuid16, UINT8 *p_uuid128);
static void bta_dm_disable_timer_cback(TIMER_LIST_ENT *p_tle);


const UINT16 bta_service_id_to_uuid_lkup_tbl [BTA_MAX_SERVICE_ID] = {
    UUID_SERVCLASS_PNP_INFORMATION,         /* Reserved */
    UUID_SERVCLASS_SERIAL_PORT,             /* BTA_SPP_SERVICE_ID */
    UUID_SERVCLASS_DIALUP_NETWORKING,       /* BTA_DUN_SERVICE_ID */
    UUID_SERVCLASS_AUDIO_SOURCE,            /* BTA_A2DP_SOURCE_SERVICE_ID */
    UUID_SERVCLASS_LAN_ACCESS_USING_PPP,    /* BTA_LAP_SERVICE_ID */
    UUID_SERVCLASS_HEADSET,                 /* BTA_HSP_HS_SERVICE_ID */
    UUID_SERVCLASS_HF_HANDSFREE,            /* BTA_HFP_HS_SERVICE_ID */
    UUID_SERVCLASS_OBEX_OBJECT_PUSH,        /* BTA_OPP_SERVICE_ID */
    UUID_SERVCLASS_OBEX_FILE_TRANSFER,      /* BTA_FTP_SERVICE_ID */
    UUID_SERVCLASS_CORDLESS_TELEPHONY,      /* BTA_CTP_SERVICE_ID */
    UUID_SERVCLASS_INTERCOM,                /* BTA_ICP_SERVICE_ID */
    UUID_SERVCLASS_IRMC_SYNC,               /* BTA_SYNC_SERVICE_ID */
    UUID_SERVCLASS_DIRECT_PRINTING,         /* BTA_BPP_SERVICE_ID */
    UUID_SERVCLASS_IMAGING_RESPONDER,       /* BTA_BIP_SERVICE_ID */
    UUID_SERVCLASS_PANU,                    /* BTA_PANU_SERVICE_ID */
    UUID_SERVCLASS_NAP,                     /* BTA_NAP_SERVICE_ID */
    UUID_SERVCLASS_GN,                      /* BTA_GN_SERVICE_ID */
    UUID_SERVCLASS_SAP,                     /* BTA_SAP_SERVICE_ID */
    UUID_SERVCLASS_AUDIO_SINK,              /* BTA_A2DP_SERVICE_ID */
    UUID_SERVCLASS_AV_REMOTE_CONTROL,       /* BTA_AVRCP_SERVICE_ID */
    UUID_SERVCLASS_HUMAN_INTERFACE,         /* BTA_HID_SERVICE_ID */
    UUID_SERVCLASS_VIDEO_SINK,              /* BTA_VDP_SERVICE_ID */
    UUID_SERVCLASS_PBAP_PSE,                /* BTA_PBAP_SERVICE_ID */
    UUID_SERVCLASS_HEADSET_AUDIO_GATEWAY,   /* BTA_HSP_SERVICE_ID */
    UUID_SERVCLASS_AG_HANDSFREE,            /* BTA_HFP_SERVICE_ID */
    UUID_SERVCLASS_MESSAGE_ACCESS,          /* BTA_MAP_SERVICE_ID */
    UUID_SERVCLASS_MESSAGE_NOTIFICATION,    /* BTA_MN_SERVICE_ID */
    UUID_SERVCLASS_HDP_PROFILE,             /* BTA_HDP_SERVICE_ID */
    UUID_SERVCLASS_PBAP_PCE                 /* BTA_PCE_SERVICE_ID */
#if BLE_INCLUDED && BTA_GATT_INCLUDED
    , UUID_PROTOCOL_ATT                      /* BTA_GATT_SERVICE_ID */
#endif
};

/*
 * NOTE : The number of element in bta_service_id_to_btm_srv_id_lkup_tbl should be matching with
 *        the value BTA_MAX_SERVICE_ID in bta_api.h
 *
 *        i.e., If you add new Service ID for BTA, the correct security ID of the new service
 *              from Security service definitions (btm_api.h) should be added to this lookup table.
 */
const UINT32 bta_service_id_to_btm_srv_id_lkup_tbl [BTA_MAX_SERVICE_ID] = {
    0,                                      /* Reserved */
    BTM_SEC_SERVICE_SERIAL_PORT,            /* BTA_SPP_SERVICE_ID */
    BTM_SEC_SERVICE_DUN,                    /* BTA_DUN_SERVICE_ID */
    BTM_SEC_SERVICE_AVDTP,                  /* BTA_AUDIO_SOURCE_SERVICE_ID */
    BTM_SEC_SERVICE_LAN_ACCESS,             /* BTA_LAP_SERVICE_ID */
    BTM_SEC_SERVICE_HEADSET_AG,             /* BTA_HSP_SERVICE_ID */
    BTM_SEC_SERVICE_AG_HANDSFREE,           /* BTA_HFP_SERVICE_ID */
    BTM_SEC_SERVICE_OBEX,                   /* BTA_OPP_SERVICE_ID */
    BTM_SEC_SERVICE_OBEX_FTP,               /* BTA_FTP_SERVICE_ID */
    BTM_SEC_SERVICE_CORDLESS,               /* BTA_CTP_SERVICE_ID */
    BTM_SEC_SERVICE_INTERCOM,               /* BTA_ICP_SERVICE_ID */
    BTM_SEC_SERVICE_IRMC_SYNC,              /* BTA_SYNC_SERVICE_ID */
    BTM_SEC_SERVICE_BPP_JOB,                /* BTA_BPP_SERVICE_ID */
    BTM_SEC_SERVICE_BIP,                    /* BTA_BIP_SERVICE_ID */
    BTM_SEC_SERVICE_BNEP_PANU,              /* BTA_PANU_SERVICE_ID */
    BTM_SEC_SERVICE_BNEP_NAP,               /* BTA_NAP_SERVICE_ID */
    BTM_SEC_SERVICE_BNEP_GN,                /* BTA_GN_SERVICE_ID */
    BTM_SEC_SERVICE_SAP,                    /* BTA_SAP_SERVICE_ID */
    BTM_SEC_SERVICE_AVDTP,                  /* BTA_A2DP_SERVICE_ID */
    BTM_SEC_SERVICE_AVCTP,                  /* BTA_AVRCP_SERVICE_ID */
    BTM_SEC_SERVICE_HIDH_SEC_CTRL,          /* BTA_HID_SERVICE_ID */
    BTM_SEC_SERVICE_AVDTP,                  /* BTA_VDP_SERVICE_ID */
    BTM_SEC_SERVICE_PBAP,                   /* BTA_PBAP_SERVICE_ID */
    BTM_SEC_SERVICE_HEADSET,                /* BTA_HSP_HS_SERVICE_ID */
    BTM_SEC_SERVICE_HF_HANDSFREE,           /* BTA_HFP_HS_SERVICE_ID */
    BTM_SEC_SERVICE_MAP,                    /* BTA_MAP_SERVICE_ID */
    BTM_SEC_SERVICE_MAP,                    /* BTA_MN_SERVICE_ID */
    BTM_SEC_SERVICE_HDP_SNK,                /* BTA_HDP_SERVICE_ID */
    BTM_SEC_SERVICE_PBAP                    /* BTA_PCE_SERVICE_ID */
#if BLE_INCLUDED && BTA_GATT_INCLUDED
    , BTM_SEC_SERVICE_ATT                   /* BTA_GATT_SERVICE_ID */
#endif

};

/* bta security callback */
#if (SMP_INCLUDED == TRUE)
const tBTM_APPL_INFO bta_security = {
    &bta_dm_authorize_cback,
    &bta_dm_pin_cback,
    &bta_dm_new_link_key_cback,
    &bta_dm_authentication_complete_cback,
    &bta_dm_bond_cancel_complete_cback,
#if (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE)
    &bta_dm_sp_cback,
#else
    NULL,
#endif
#if BLE_INCLUDED == TRUE
    &bta_dm_ble_smp_cback,
    &bta_dm_ble_id_key_cback,
#endif  ///BLE_INCLUDED == TRUE

};
#endif  ///SMP_INCLUDED == TRUE

#if (SDP_INCLUDED == TRUE)
#define MAX_DISC_RAW_DATA_BUF       (1024)
UINT8 g_disc_raw_data_buf[MAX_DISC_RAW_DATA_BUF];
#endif  ///SDP_INCLUDED == TRUE
extern DEV_CLASS local_device_default_class;

/*******************************************************************************
**
** Function         bta_dm_enable
**
** Description      Initialises the BT device manager
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_enable(tBTA_DM_MSG *p_data)
{
    tBTA_SYS_HW_MSG *sys_enable_event;
    tBTA_DM_ENABLE enable_event;

    /* if already in use, return an error */
    if ( bta_dm_cb.is_bta_dm_active == TRUE  ) {
        APPL_TRACE_WARNING("%s Device already started by another application", __func__);
        memset(&enable_event, 0, sizeof(tBTA_DM_ENABLE));
        enable_event.status = BTA_FAILURE;
        if (p_data->enable.p_sec_cback != NULL) {
            p_data->enable.p_sec_cback(BTA_DM_ENABLE_EVT, (tBTA_DM_SEC *)&enable_event);
        }
        return;
    }

    /* first, register our callback to SYS HW manager */
    bta_sys_hw_register( BTA_SYS_HW_BLUETOOTH, bta_dm_sys_hw_cback );

    /* make sure security callback is saved - if no callback, do not erase the previous one,
    it could be an error recovery mechanism */
    if ( p_data->enable.p_sec_cback != NULL  ) {
        bta_dm_cb.p_sec_cback = p_data->enable.p_sec_cback;
    }
    /* notify BTA DM is now active */
    bta_dm_cb.is_bta_dm_active = TRUE;

    /* send a message to BTA SYS */
    if ((sys_enable_event = (tBTA_SYS_HW_MSG *) osi_malloc(sizeof(tBTA_SYS_HW_MSG))) != NULL) {
        sys_enable_event->hdr.event = BTA_SYS_API_ENABLE_EVT;
        sys_enable_event->hw_module = BTA_SYS_HW_BLUETOOTH;

        bta_sys_sendmsg(sys_enable_event);
    }
}

/*******************************************************************************
**
** Function         bta_dm_sys_hw_cback
**
** Description     callback register to SYS to get HW status updates
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_sys_hw_cback( tBTA_SYS_HW_EVT status )
{
    DEV_CLASS   dev_class;
    tBTA_DM_SEC_CBACK           *temp_cback;
#if BLE_INCLUDED == TRUE
    UINT8                   key_mask = 0;
    BT_OCTET16              er;
    tBTA_BLE_LOCAL_ID_KEYS  id_key;
#endif

    APPL_TRACE_DEBUG("%s with event: %i", __func__, status);

    /* On H/W error evt, report to the registered DM application callback */
    if (status == BTA_SYS_HW_ERROR_EVT) {
        if ( bta_dm_cb.p_sec_cback != NULL ) {
            bta_dm_cb.p_sec_cback(BTA_DM_HW_ERROR_EVT, NULL);
        }
        return;
    }

    if ( status == BTA_SYS_HW_OFF_EVT ) {
        if ( bta_dm_cb.p_sec_cback != NULL ) {
            bta_dm_cb.p_sec_cback(BTA_DM_DISABLE_EVT, NULL);
        }

        /* reinitialize the control block */
        memset(&bta_dm_cb, 0, sizeof(bta_dm_cb));

        /* unregister from SYS */
        bta_sys_hw_unregister( BTA_SYS_HW_BLUETOOTH );
        /* notify BTA DM is now unactive */
        bta_dm_cb.is_bta_dm_active = FALSE;
    } else if ( status == BTA_SYS_HW_ON_EVT ) {
        /* FIXME: We should not unregister as the SYS shall invoke this callback on a H/W error.
        * We need to revisit when this platform has more than one BLuetooth H/W chip */
        //bta_sys_hw_unregister( BTA_SYS_HW_BLUETOOTH);

        /* save security callback */
        temp_cback = bta_dm_cb.p_sec_cback;
        /* make sure the control block is properly initialized */
        memset(&bta_dm_cb, 0, sizeof(bta_dm_cb));
        /* and retrieve the callback */
        bta_dm_cb.p_sec_cback = temp_cback;
        bta_dm_cb.is_bta_dm_active = TRUE;

        /* hw is ready, go on with BTA DM initialization */
        memset(&bta_dm_search_cb, 0x00, sizeof(bta_dm_search_cb));
#if (BTM_SSR_INCLUDED == TRUE)
        memset(&bta_dm_conn_srvcs, 0x00, sizeof(bta_dm_conn_srvcs));
#endif  ///BTM_SSR_INCLUDED == TRUE
        memset(&bta_dm_di_cb, 0, sizeof(tBTA_DM_DI_CB));

        memcpy(dev_class, p_bta_dm_cfg->dev_class, sizeof(dev_class));
        BTM_SetDeviceClass (dev_class);

#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE)
        /* load BLE local information: ID keys, ER if available */
        bta_dm_co_ble_load_local_keys(&key_mask, er, &id_key);

        if (key_mask & BTA_BLE_LOCAL_KEY_TYPE_ER) {
            BTM_BleLoadLocalKeys(BTA_BLE_LOCAL_KEY_TYPE_ER, (tBTM_BLE_LOCAL_KEYS *)&er);
        }
        if (key_mask & BTA_BLE_LOCAL_KEY_TYPE_ID) {
            BTM_BleLoadLocalKeys(BTA_BLE_LOCAL_KEY_TYPE_ID, (tBTM_BLE_LOCAL_KEYS *)&id_key);
        }
#if ((defined BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE) && SDP_INCLUDED == TRUE)
        bta_dm_search_cb.conn_id = BTA_GATT_INVALID_CONN_ID;
#endif
#endif
#if (SMP_INCLUDED == TRUE)
        BTM_SecRegister((tBTM_APPL_INFO *)&bta_security);
#endif  ///SMP_INCLUDED == TRUE
        BTM_SetDefaultLinkSuperTout(p_bta_dm_cfg->link_timeout);
        BTM_WritePageTimeout(p_bta_dm_cfg->page_timeout);
        bta_dm_cb.cur_policy = p_bta_dm_cfg->policy_settings;
        BTM_SetDefaultLinkPolicy(bta_dm_cb.cur_policy);
        BTM_RegBusyLevelNotif (bta_dm_bl_change_cback, NULL, BTM_BL_UPDATE_MASK | BTM_BL_ROLE_CHG_MASK);

#if BLE_VND_INCLUDED == TRUE
        BTM_BleReadControllerFeatures (bta_dm_ctrl_features_rd_cmpl_cback);
#endif

        /* Earlier, we used to invoke BTM_ReadLocalAddr which was just copying the bd_addr
           from the control block and invoking the callback which was sending the DM_ENABLE_EVT.
           But then we have a few HCI commands being invoked above which were still in progress
           when the ENABLE_EVT was sent. So modified this to fetch the local name which forces
           the DM_ENABLE_EVT to be sent only after all the init steps are complete */
        BTM_ReadLocalDeviceNameFromController((tBTM_CMPL_CB *)bta_dm_local_name_cback);

        bta_sys_rm_register((tBTA_SYS_CONN_CBACK *)bta_dm_rm_cback);
#if (BTM_SSR_INCLUDED == TRUE)
        /* initialize bluetooth low power manager */
        bta_dm_init_pm();
#endif  ///BTM_SSR_INCLUDED == TRUE
        bta_sys_policy_register((tBTA_SYS_CONN_CBACK *)bta_dm_policy_cback);

#if (BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE) && (GATTC_INCLUDED == TRUE)
        bta_dm_gattc_register();
#endif

    } else {
        APPL_TRACE_DEBUG(" --- ignored event");
    }

}


/*******************************************************************************
**
** Function         bta_dm_disable
**
** Description      Disables the BT device manager
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_disable (tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);

    /* Set l2cap idle timeout to 0 (so BTE immediately disconnects ACL link after last channel is closed) */
    L2CA_SetIdleTimeoutByBdAddr((UINT8 *)BT_BD_ANY, 0, BT_TRANSPORT_BR_EDR);
    L2CA_SetIdleTimeoutByBdAddr((UINT8 *)BT_BD_ANY, 0, BT_TRANSPORT_LE);

    /* disable all active subsystems */
    bta_sys_disable(BTA_SYS_HW_BLUETOOTH);

    BTM_SetDiscoverability(BTM_NON_DISCOVERABLE, 0, 0);
    BTM_SetConnectability(BTM_NON_CONNECTABLE, 0, 0);
#if (BTM_SSR_INCLUDED == TRUE)
    bta_dm_disable_pm();
#endif  ///BTM_SSR_INCLUDED == TRUE
    bta_dm_disable_search_and_disc();
    bta_dm_cb.disabling = TRUE;

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
    BTM_BleClearBgConnDev();
#endif

    if (BTM_GetNumAclLinks() == 0) {
#if (defined(BTA_DISABLE_DELAY) && BTA_DISABLE_DELAY > 0)
        /* If BTA_DISABLE_DELAY is defined and greater than zero, then delay the shutdown by
         * BTA_DISABLE_DELAY milliseconds
         */
        APPL_TRACE_WARNING("%s BTA_DISABLE_DELAY set to %d ms",
                           __FUNCTION__, BTA_DISABLE_DELAY);
        bta_sys_stop_timer(&bta_dm_cb.disable_timer);
        bta_dm_cb.disable_timer.p_cback = (TIMER_CBACK *)&bta_dm_disable_conn_down_timer_cback;
        bta_sys_start_timer(&bta_dm_cb.disable_timer, 0, BTA_DISABLE_DELAY);
#else
        bta_dm_disable_conn_down_timer_cback(NULL);
#endif
    } else {
        bta_dm_cb.disable_timer.p_cback = (TIMER_CBACK *)&bta_dm_disable_timer_cback;
        bta_dm_cb.disable_timer.param = 0;
        bta_sys_start_timer(&bta_dm_cb.disable_timer, 0, 5000);
    }

#if BLE_PRIVACY_SPT == TRUE
    btm_ble_resolving_list_cleanup ();  //by TH, because cmn_ble_vsc_cb.max_filter has something mistake as btm_ble_adv_filter_cleanup
#endif

}

/*******************************************************************************
**
** Function         bta_dm_disable_timer_cback
**
** Description      Called if the disable timer expires
**                  Used to close ACL connections which are still active
**
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_disable_timer_cback (TIMER_LIST_ENT *p_tle)
{
    UNUSED(p_tle);
    UINT8 i;
    tBT_TRANSPORT transport = BT_TRANSPORT_BR_EDR;
    BOOLEAN trigger_disc = FALSE;


    APPL_TRACE_EVENT(" bta_dm_disable_timer_cback trial %d ", p_tle->param);

    if (BTM_GetNumAclLinks() && p_tle->param == 0) {
        for (i = 0; i < bta_dm_cb.device_list.count; i++) {
#if (BLE_INCLUDED == TRUE)
            transport = bta_dm_cb.device_list.peer_device[i].transport;
#endif
            btm_remove_acl(bta_dm_cb.device_list.peer_device[i].peer_bdaddr, transport);
            trigger_disc = TRUE;
        }

        /* Retrigger disable timer in case ACL disconnect failed, DISABLE_EVT still need
            to be sent out to avoid jave layer disable timeout */
        if (trigger_disc) {
            bta_dm_cb.disable_timer.p_cback = (TIMER_CBACK *)&bta_dm_disable_timer_cback;
            bta_dm_cb.disable_timer.param = 1;
            bta_sys_start_timer(&bta_dm_cb.disable_timer, 0, 1500);
        }
    } else {
        bta_dm_cb.disabling = FALSE;

        bta_sys_remove_uuid(UUID_SERVCLASS_PNP_INFORMATION);
        bta_dm_cb.p_sec_cback(BTA_DM_DISABLE_EVT, NULL);
    }
}




/*******************************************************************************
**
** Function         bta_dm_set_dev_name
**
** Description      Sets local device name
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_set_dev_name (tBTA_DM_MSG *p_data)
{

    BTM_SetLocalDeviceName((char *)p_data->set_name.name);
    bta_dm_set_eir ((char *)p_data->set_name.name);
}

void bta_dm_update_white_list(tBTA_DM_MSG *p_data)
{
    BTM_BleUpdateAdvWhitelist(p_data->white_list.add_remove, p_data->white_list.remote_addr);
}

void bta_dm_ble_read_adv_tx_power(tBTA_DM_MSG *p_data)
{
    if (p_data->read_tx_power.read_tx_power_cb != NULL) {
        BTM_BleReadAdvTxPower(p_data->read_tx_power.read_tx_power_cb);
    } else {
        APPL_TRACE_ERROR("%s(), the callback function cann't be NULL.", __func__);
    }
}

void bta_dm_ble_read_rssi(tBTA_DM_MSG *p_data)
{
    if (p_data->rssi.read_rssi_cb != NULL) {
        BTM_ReadRSSI(p_data->rssi.remote_addr, p_data->rssi.read_rssi_cb);
    } else {
        APPL_TRACE_ERROR("%s(), the callback function cann't be NULL.", __func__);
    }
}

/*******************************************************************************
**
** Function         bta_dm_set_visibility
**
** Description      Sets discoverability, connectability and pairability
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_set_visibility(tBTA_DM_MSG *p_data)
{
    UINT16 window, interval;
    UINT16 le_disc_mode = BTM_BleReadDiscoverability();
    UINT16 disc_mode = BTM_ReadDiscoverability(&window, &interval);
    UINT16 le_conn_mode = BTM_BleReadConnectability();
    UINT16 conn_mode = BTM_ReadConnectability(&window, &interval);

    /* set modes for Discoverability and connectability if not ignore */
    if (p_data->set_visibility.disc_mode != (BTA_DM_IGNORE | BTA_DM_LE_IGNORE)) {
        if ((p_data->set_visibility.disc_mode & BTA_DM_LE_IGNORE) == BTA_DM_LE_IGNORE)
            p_data->set_visibility.disc_mode =
                ((p_data->set_visibility.disc_mode & ~BTA_DM_LE_IGNORE) | le_disc_mode);

        if ((p_data->set_visibility.disc_mode & BTA_DM_IGNORE) == BTA_DM_IGNORE)
            p_data->set_visibility.disc_mode =
                ((p_data->set_visibility.disc_mode & ~BTA_DM_IGNORE) | disc_mode);

        BTM_SetDiscoverability(p_data->set_visibility.disc_mode,
                               bta_dm_cb.inquiry_scan_window,
                               bta_dm_cb.inquiry_scan_interval);
    }

    if (p_data->set_visibility.conn_mode != (BTA_DM_IGNORE | BTA_DM_LE_IGNORE)) {
        if ((p_data->set_visibility.conn_mode & BTA_DM_LE_IGNORE) == BTA_DM_LE_IGNORE)
            p_data->set_visibility.conn_mode =
                ((p_data->set_visibility.conn_mode & ~BTA_DM_LE_IGNORE) | le_conn_mode);

        if ((p_data->set_visibility.conn_mode & BTA_DM_IGNORE) == BTA_DM_IGNORE)
            p_data->set_visibility.conn_mode =
                ((p_data->set_visibility.conn_mode & ~BTA_DM_IGNORE) | conn_mode);

        BTM_SetConnectability(p_data->set_visibility.conn_mode,
                              bta_dm_cb.page_scan_window,
                              bta_dm_cb.page_scan_interval);
    }

    /* Send False or True if not ignore */
    if (p_data->set_visibility.pair_mode != BTA_DM_IGNORE ) {

        if (p_data->set_visibility.pair_mode == BTA_DM_NON_PAIRABLE) {
            bta_dm_cb.disable_pair_mode = TRUE;
        } else {
            bta_dm_cb.disable_pair_mode = FALSE;
        }

    }

    /* Send False or True if not ignore */
    if (p_data->set_visibility.conn_paired_only != BTA_DM_IGNORE) {

        if (p_data->set_visibility.conn_paired_only == BTA_DM_CONN_ALL) {
            bta_dm_cb.conn_paired_only = FALSE;
        } else {
            bta_dm_cb.conn_paired_only = TRUE;
        }

    }

    /* Change mode if either mode is not ignore */
    if (p_data->set_visibility.pair_mode != BTA_DM_IGNORE || p_data->set_visibility.conn_paired_only != BTA_DM_IGNORE) {
        BTM_SetPairableMode((BOOLEAN)(!(bta_dm_cb.disable_pair_mode)), bta_dm_cb.conn_paired_only);
    }

}

/*******************************************************************************
**
** Function         bta_dm_process_remove_device
**
** Description      Removes device, Disconnects ACL link if required.
****
*******************************************************************************/
void bta_dm_process_remove_device(BD_ADDR bd_addr)
{
#if (BLE_INCLUDED == TRUE && GATTC_INCLUDED == TRUE)
    /* need to remove all pending background connection before unpair */
    BTA_GATTC_CancelOpen(0, bd_addr, FALSE);
#endif

    BTM_SecDeleteDevice(bd_addr);

#if (BLE_INCLUDED == TRUE && GATTC_INCLUDED == TRUE)
    /* remove all cached GATT information */
    BTA_GATTC_Refresh(bd_addr);
#endif

    if (bta_dm_cb.p_sec_cback) {
        tBTA_DM_SEC sec_event;
        bdcpy(sec_event.link_down.bd_addr, bd_addr);
        /* No connection, set status to success (acl disc code not valid) */
        sec_event.link_down.status = HCI_SUCCESS;
        bta_dm_cb.p_sec_cback(BTA_DM_DEV_UNPAIRED_EVT, &sec_event);
    }
}

/*******************************************************************************
**
** Function         bta_dm_remove_device
**
** Description      Removes device, disconnects ACL link if required.
****
*******************************************************************************/
void bta_dm_remove_device(tBTA_DM_MSG *p_data)
{
    tBTA_DM_API_REMOVE_DEVICE *p_dev = &p_data->remove_dev;
    if (p_dev == NULL) {
        return;
    }

    BD_ADDR other_address;
    bdcpy(other_address, p_dev->bd_addr);

    /* If ACL exists for the device in the remove_bond message*/
    BOOLEAN continue_delete_dev = FALSE;
    UINT8 other_transport = BT_TRANSPORT_INVALID;

    if (BTM_IsAclConnectionUp(p_dev->bd_addr, BT_TRANSPORT_LE) ||
            BTM_IsAclConnectionUp(p_dev->bd_addr, BT_TRANSPORT_BR_EDR)) {
        APPL_TRACE_DEBUG("%s: ACL Up count  %d", __func__, bta_dm_cb.device_list.count);
        continue_delete_dev = FALSE;

        /* Take the link down first, and mark the device for removal when disconnected */
        for (int i = 0; i < bta_dm_cb.device_list.count; i++) {
            if (!bdcmp(bta_dm_cb.device_list.peer_device[i].peer_bdaddr, p_dev->bd_addr)) {
                bta_dm_cb.device_list.peer_device[i].conn_state = BTA_DM_UNPAIRING;
                btm_remove_acl( p_dev->bd_addr, bta_dm_cb.device_list.peer_device[i].transport);
                APPL_TRACE_DEBUG("%s:transport = %d", __func__,
                                 bta_dm_cb.device_list.peer_device[i].transport);

                /* save the other transport to check if device is connected on other_transport */
                if (bta_dm_cb.device_list.peer_device[i].transport == BT_TRANSPORT_LE) {
                    other_transport = BT_TRANSPORT_BR_EDR;
                } else {
                    other_transport = BT_TRANSPORT_LE;
                }
                break;
            }
        }
    } else {
        continue_delete_dev = TRUE;
    }

    // If it is DUMO device and device is paired as different address, unpair that device
    // if different address
    BOOLEAN continue_delete_other_dev = FALSE;
    if ((other_transport && (BTM_ReadConnectedTransportAddress(other_address, other_transport))) ||
            (!other_transport && (BTM_ReadConnectedTransportAddress(other_address, BT_TRANSPORT_BR_EDR) ||
                                  BTM_ReadConnectedTransportAddress(other_address, BT_TRANSPORT_LE)))) {
        continue_delete_other_dev = FALSE;
        /* Take the link down first, and mark the device for removal when disconnected */
        for (int i = 0; i < bta_dm_cb.device_list.count; i++) {
            if (!bdcmp(bta_dm_cb.device_list.peer_device[i].peer_bdaddr, other_address)) {
                bta_dm_cb.device_list.peer_device[i].conn_state = BTA_DM_UNPAIRING;
                btm_remove_acl(other_address, bta_dm_cb.device_list.peer_device[i].transport);
                break;
            }
        }
    } else {
        APPL_TRACE_DEBUG("%s: continue to delete the other dev ", __func__);
        continue_delete_other_dev = TRUE;
    }

    /* Delete the device mentioned in the msg */
    if (continue_delete_dev) {
        bta_dm_process_remove_device(p_dev->bd_addr);
    }

    /* Delete the other paired device too */
    BD_ADDR dummy_bda = {0};
    if (continue_delete_other_dev && (bdcmp(other_address, dummy_bda) != 0)) {
        bta_dm_process_remove_device(other_address);
    }
}

/*******************************************************************************
**
** Function         bta_dm_add_device
**
** Description      This function adds a Link Key to an security database entry.
**                  It is normally called during host startup to restore all required information
**                  stored in the NVRAM.
****
*******************************************************************************/
void bta_dm_add_device (tBTA_DM_MSG *p_data)
{
    tBTA_DM_API_ADD_DEVICE *p_dev = &p_data->add_dev;
    UINT8   *p_dc = NULL;
    UINT8   *p_lc = NULL;
    UINT32  trusted_services_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
    UINT8   index = 0;
    UINT8   btm_mask_index = 0;

    memset (trusted_services_mask, 0, sizeof(trusted_services_mask));

    /* If not all zeros, the device class has been specified */
    if (p_dev->dc_known) {
        p_dc = (UINT8 *)p_dev->dc;
    }

    if (p_dev->link_key_known) {
        p_lc = (UINT8 *)p_dev->link_key;
    }

    if (p_dev->is_trusted) {
        /* covert BTA service mask to BTM mask */
        while (p_dev->tm && (index < BTA_MAX_SERVICE_ID)) {
            if (p_dev->tm & (UINT32)(1 << index)) {

                btm_mask_index =  bta_service_id_to_btm_srv_id_lkup_tbl[index] / BTM_SEC_ARRAY_BITS;
                trusted_services_mask[btm_mask_index] |= (UINT32)(1 << (bta_service_id_to_btm_srv_id_lkup_tbl[index] - (UINT32)(btm_mask_index * 32)));

                p_dev->tm &= (UINT32)(~(1 << index));

            }
            index++;
        }
    }

    if (!BTM_SecAddDevice (p_dev->bd_addr, p_dc, p_dev->bd_name, p_dev->features,
                           trusted_services_mask, p_lc, p_dev->key_type, p_dev->io_cap,
                           p_dev->pin_length)) {
        APPL_TRACE_ERROR ("BTA_DM: Error adding device %08x%04x",
                          (p_dev->bd_addr[0] << 24) + (p_dev->bd_addr[1] << 16) + (p_dev->bd_addr[2] << 8) + p_dev->bd_addr[3],
                          (p_dev->bd_addr[4] << 8) + p_dev->bd_addr[5]);
    }
}

/*******************************************************************************
**
** Function         bta_dm_close_acl
**
** Description      This function forces to close the connection to a remote device
**                  and optionaly remove the device from security database if
**                  required.
****
*******************************************************************************/
void bta_dm_close_acl(tBTA_DM_MSG *p_data)
{
    tBTA_DM_API_REMOVE_ACL *p_remove_acl = &p_data->remove_acl;
    UINT8   index;

    APPL_TRACE_DEBUG("bta_dm_close_acl");

    if (BTM_IsAclConnectionUp(p_remove_acl->bd_addr, p_remove_acl->transport)) {
        for (index = 0; index < bta_dm_cb.device_list.count; index ++) {
            if (!bdcmp( bta_dm_cb.device_list.peer_device[index].peer_bdaddr, p_remove_acl->bd_addr)) {
                break;
            }
        }
        if (index != bta_dm_cb.device_list.count) {
            if (p_remove_acl->remove_dev) {
                bta_dm_cb.device_list.peer_device[index].remove_dev_pending = TRUE;
            }
        } else {
            APPL_TRACE_ERROR("unknown device, remove ACL failed");
        }
        /* Disconnect the ACL link */
        btm_remove_acl(p_remove_acl->bd_addr, p_remove_acl->transport);
    }
    /* if to remove the device from security database ? do it now */
    else if (p_remove_acl->remove_dev) {
        if (!BTM_SecDeleteDevice(p_remove_acl->bd_addr)) {
            APPL_TRACE_ERROR("delete device from security database failed.");
        }
#if (BLE_INCLUDED == TRUE && GATTC_INCLUDED == TRUE)
        /* need to remove all pending background connection if any */
        BTA_GATTC_CancelOpen(0, p_remove_acl->bd_addr, FALSE);
        /* remove all cached GATT information */
        BTA_GATTC_Refresh(p_remove_acl->bd_addr);
#endif
    }
    /* otherwise, no action needed */

}

/*******************************************************************************
**
** Function         bta_dm_remove_all_acl
**
** Description      This function forces to close all the ACL links specified by link type
****
*******************************************************************************/
void bta_dm_remove_all_acl(tBTA_DM_MSG *p_data)
{
    const tBTA_DM_LINK_TYPE link_type = p_data->remove_all_acl.link_type;
    tBT_TRANSPORT transport = BT_TRANSPORT_BR_EDR;

    APPL_TRACE_DEBUG("%s link type = %d", __func__, link_type);

    for (UINT8 i = 0; i < bta_dm_cb.device_list.count; i++) {
        BD_ADDR addr = {0};
        bdcpy(addr, bta_dm_cb.device_list.peer_device[i].peer_bdaddr);
#if defined (BLE_INCLUDED) && (BLE_INCLUDED == TRUE)
        transport = bta_dm_cb.device_list.peer_device[i].transport;
#endif
        if ((link_type == BTA_DM_LINK_TYPE_ALL) ||
                ((link_type == BTA_DM_LINK_TYPE_LE) && (transport == BT_TRANSPORT_LE)) ||
                ((link_type == BTA_DM_LINK_TYPE_BR_EDR) && (transport == BT_TRANSPORT_BR_EDR))) {
            /* Disconnect the ACL link */
            btm_remove_acl(addr, transport);
        }
    }
}


/*******************************************************************************
**
** Function         bta_dm_bond
**
** Description      Bonds with peer device
**
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void bta_dm_bond (tBTA_DM_MSG *p_data)
{
    tBTM_STATUS status;
    tBTA_DM_SEC sec_event;
    char        *p_name;

    if (p_data->bond.transport == BTA_TRANSPORT_UNKNOWN) {
        status = BTM_SecBond ( p_data->bond.bd_addr, 0, NULL, 0 );
    } else {
        status = BTM_SecBondByTransport ( p_data->bond.bd_addr, p_data->bond.transport, 0, NULL, 0 );
    }


    if (bta_dm_cb.p_sec_cback && (status != BTM_CMD_STARTED)) {

        memset(&sec_event, 0, sizeof(tBTA_DM_SEC));
        bdcpy(sec_event.auth_cmpl.bd_addr, p_data->bond.bd_addr);
        p_name = BTM_SecReadDevName(p_data->bond.bd_addr);
        if (p_name != NULL) {
            memcpy(sec_event.auth_cmpl.bd_name, p_name, (BD_NAME_LEN - 1));
            sec_event.auth_cmpl.bd_name[BD_NAME_LEN - 1] = 0;
        }

        /*      taken care of by memset [above]
                sec_event.auth_cmpl.key_present = FALSE;
                sec_event.auth_cmpl.success = FALSE;
        */
        sec_event.auth_cmpl.fail_reason = HCI_ERR_ILLEGAL_COMMAND;
        if (status == BTM_SUCCESS) {
            sec_event.auth_cmpl.success = TRUE;
        } else {
            /* delete this device entry from Sec Dev DB */
            bta_dm_remove_sec_dev_entry(p_data->bond.bd_addr);
        }
        bta_dm_cb.p_sec_cback(BTA_DM_AUTH_CMPL_EVT, &sec_event);
    }

}

/*******************************************************************************
**
** Function         bta_dm_bond_cancel
**
** Description      Cancels bonding with a peer device
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_bond_cancel (tBTA_DM_MSG *p_data)
{
    tBTM_STATUS status;
    tBTA_DM_SEC sec_event;

    APPL_TRACE_EVENT(" bta_dm_bond_cancel ");
    status = BTM_SecBondCancel ( p_data->bond_cancel.bd_addr );

    if (bta_dm_cb.p_sec_cback && (status != BTM_CMD_STARTED && status != BTM_SUCCESS)) {
        sec_event.bond_cancel_cmpl.result = BTA_FAILURE;

        bta_dm_cb.p_sec_cback(BTA_DM_BOND_CANCEL_CMPL_EVT, &sec_event);
    }

}

/*******************************************************************************
**
** Function         bta_dm_pin_reply
**
** Description      Send the pin_reply to a request from BTM
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_pin_reply (tBTA_DM_MSG *p_data)
{
    UINT32  trusted_mask[BTM_SEC_SERVICE_ARRAY_SIZE];
    UINT32   *current_trusted_mask;

    current_trusted_mask = BTM_ReadTrustedMask(p_data->pin_reply.bd_addr);

    if (current_trusted_mask) {
        memcpy(trusted_mask, current_trusted_mask, sizeof(trusted_mask));
    } else {
        memset(trusted_mask, 0, sizeof(trusted_mask));
    }

    if (p_data->pin_reply.accept) {

        BTM_PINCodeReply(p_data->pin_reply.bd_addr, BTM_SUCCESS, p_data->pin_reply.pin_len, p_data->pin_reply.p_pin, trusted_mask );
    } else {
        BTM_PINCodeReply(p_data->pin_reply.bd_addr, BTM_NOT_AUTHORIZED, 0, NULL, trusted_mask );
    }

}
#endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_policy_cback
**
** Description      process the link policy changes
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_policy_cback(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    tBTA_DM_PEER_DEVICE *p_dev = NULL;
    UINT16  policy = app_id;
    UINT32  mask = (UINT32)(1 << id);

    if (peer_addr) {
        p_dev = bta_dm_find_peer_device(peer_addr);
    }

    APPL_TRACE_DEBUG(" bta_dm_policy_cback cmd:%d, policy:0x%x",
                     status, policy);
    switch (status) {
    case BTA_SYS_PLCY_SET:
        if (!p_dev) {
            return;
        }
        /* restore the default link policy */
        p_dev->link_policy |= policy;
        BTM_SetLinkPolicy(p_dev->peer_bdaddr, &(p_dev->link_policy));
        break;

    case BTA_SYS_PLCY_CLR:
        if (!p_dev) {
            return;
        }
        /* clear the policy from the default link policy */
        p_dev->link_policy &= (~policy);
        BTM_SetLinkPolicy(p_dev->peer_bdaddr, &(p_dev->link_policy));

        if (policy & (HCI_ENABLE_SNIFF_MODE | HCI_ENABLE_PARK_MODE)) {
            /* if clearing sniff/park, wake the link */
#if (BTM_SSR_INCLUDED == TRUE)
            bta_dm_pm_active(p_dev->peer_bdaddr);
#endif  ///BTM_SSR_INCLUDED == TRUE
        }
        break;

    case BTA_SYS_PLCY_DEF_SET:
        /* want to restore/set the role switch policy */
        bta_dm_cb.role_policy_mask &= ~mask;
        if (0 == bta_dm_cb.role_policy_mask) {
            /* if nobody wants to insist on the role */
            bta_dm_cb.cur_policy |= HCI_ENABLE_MASTER_SLAVE_SWITCH;
            BTM_SetDefaultLinkPolicy(bta_dm_cb.cur_policy);
        }
        break;

    case BTA_SYS_PLCY_DEF_CLR:
        /* want to remove the role switch policy */
        bta_dm_cb.role_policy_mask |= mask;
        bta_dm_cb.cur_policy &= ~HCI_ENABLE_MASTER_SLAVE_SWITCH;
        BTM_SetDefaultLinkPolicy(bta_dm_cb.cur_policy);
        break;
    }
}

/*******************************************************************************
**
** Function         bta_dm_confirm
**
** Description      Send the user confirm request reply in response to a
**                  request from BTM
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void bta_dm_confirm(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS res = BTM_NOT_AUTHORIZED;

    if (p_data->confirm.accept == TRUE) {
        res = BTM_SUCCESS;
    }
    BTM_ConfirmReqReply(res, p_data->confirm.bd_addr);
}
#endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_loc_oob
**
** Description      Retrieve the OOB data from the local LM
**
** Returns          void
**
*******************************************************************************/
#if (BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
void bta_dm_loc_oob(tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    BTM_ReadLocalOobData();
}

/*******************************************************************************
**
** Function         bta_dm_ci_io_req_act
**
** Description      respond to the IO capabilities request from BTM
**
** Returns          void
**
*******************************************************************************/
void bta_dm_ci_io_req_act(tBTA_DM_MSG *p_data)
{
    tBTM_AUTH_REQ   auth_req = BTM_AUTH_AP_NO;
    if (p_data->ci_io_req.auth_req) {
        auth_req = BTM_AUTH_AP_YES;
    }
    BTM_IoCapRsp(p_data->ci_io_req.bd_addr, p_data->ci_io_req.io_cap,
                 p_data->ci_io_req.oob_data, auth_req);
}

/*******************************************************************************
**
** Function         bta_dm_ci_rmt_oob_act
**
** Description      respond to the OOB data request for the remote device from BTM
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_ci_rmt_oob_act(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS res = BTM_NOT_AUTHORIZED;

    if (p_data->ci_rmt_oob.accept == TRUE) {
        res = BTM_SUCCESS;
    }
    BTM_RemoteOobDataReply(res, p_data->ci_rmt_oob.bd_addr,
                           p_data->ci_rmt_oob.c, p_data->ci_rmt_oob.r );
}
#endif /* BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE */

/*******************************************************************************
**
** Function         bta_dm_search_start
**
** Description      Starts an inquiry
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_search_start (tBTA_DM_MSG *p_data)
{
    tBTM_INQUIRY_CMPL result;

#if (BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE) && (GATTC_INCLUDED == TRUE)
    UINT16 len = (UINT16)(sizeof(tBT_UUID) * p_data->search.num_uuid);
    bta_dm_gattc_register();
#endif

    APPL_TRACE_DEBUG("%s avoid_scatter=%d", __func__, p_bta_dm_cfg->avoid_scatter);

    if (p_bta_dm_cfg->avoid_scatter &&
            (p_data->search.rs_res == BTA_DM_RS_NONE) && bta_dm_check_av(BTA_DM_API_SEARCH_EVT)) {
        memcpy(&bta_dm_cb.search_msg, &p_data->search, sizeof(tBTA_DM_API_SEARCH));
        return;
    }

    BTM_ClearInqDb(NULL);
    /* save search params */
    bta_dm_search_cb.p_search_cback = p_data->search.p_cback;
    bta_dm_search_cb.services = p_data->search.services;

#if (BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE) && (GATTC_INCLUDED == TRUE)
    utl_freebuf((void **)&bta_dm_search_cb.p_srvc_uuid);

    if ((bta_dm_search_cb.num_uuid = p_data->search.num_uuid) != 0 &&
            p_data->search.p_uuid != NULL) {
        if ((bta_dm_search_cb.p_srvc_uuid = (tBT_UUID *)osi_malloc(len)) == NULL) {
            APPL_TRACE_ERROR("%s no resources", __func__);

            result.status = BTA_FAILURE;
            result.num_resp = 0;
            bta_dm_inq_cmpl_cb ((void *)&result);
            return;
        }

        memcpy(bta_dm_search_cb.p_srvc_uuid, p_data->search.p_uuid, len);
    }
#endif
    result.status = BTM_StartInquiry(   (tBTM_INQ_PARMS *)&p_data->search.inq_params,
                                        bta_dm_inq_results_cb,
                                        (tBTM_CMPL_CB *) bta_dm_inq_cmpl_cb);

    APPL_TRACE_EVENT("%s status=%d", __func__, result.status);
    if (result.status != BTM_CMD_STARTED) {
        result.num_resp = 0;
        bta_dm_inq_cmpl_cb ((void *)&result);
    }
}

/*******************************************************************************
**
** Function         bta_dm_search_cancel
**
** Description      Cancels an ongoing search for devices
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_search_cancel (tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    tBTA_DM_MSG *p_msg;

    if (BTM_IsInquiryActive()) {
        if (BTM_CancelInquiry() != BTM_CMD_STARTED) {
            bta_dm_search_cancel_notify(NULL);
            p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG));
            if (p_msg != NULL) {
                p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
                p_msg->hdr.layer_specific = BTA_DM_API_DISCOVER_EVT;
                bta_sys_sendmsg(p_msg);
            }
        } else {
            /* flag a search cancel is pending */
            bta_dm_search_cb.cancel_pending = TRUE;
        }
    }
    /* If no Service Search going on then issue cancel remote name in case it is active */
    else if (!bta_dm_search_cb.name_discover_done) {
        BTM_CancelRemoteDeviceName();

        if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
            p_msg->hdr.layer_specific = BTA_DM_API_DISCOVER_EVT;
            bta_sys_sendmsg(p_msg);
        }

    } else {
        if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_INQUIRY_CMPL_EVT;
            p_msg->hdr.layer_specific = BTA_DM_API_DISCOVER_EVT;
            bta_sys_sendmsg(p_msg);
        }
    }

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE && GATTC_INCLUDED == TRUE
    if (bta_dm_search_cb.gatt_disc_active) {
        bta_dm_cancel_gatt_discovery(bta_dm_search_cb.peer_bdaddr);
    }
#endif
}

/*******************************************************************************
**
** Function         bta_dm_discover
**
** Description      Discovers services on a remote device
**
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
void bta_dm_discover (tBTA_DM_MSG *p_data)
{
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE  && GATTC_INCLUDED == TRUE
    UINT16 len = (UINT16)(sizeof(tBT_UUID) * p_data->discover.num_uuid);
#endif
    APPL_TRACE_EVENT("%s services_to_search=0x%04X, sdp_search=%d", __func__,
                     p_data->discover.services, p_data->discover.sdp_search);

    /* save the search condition */
    bta_dm_search_cb.services = p_data->discover.services;

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && GATTC_INCLUDED == TRUE
    bta_dm_gattc_register();
    utl_freebuf((void **)&bta_dm_search_cb.p_srvc_uuid);
    if ((bta_dm_search_cb.num_uuid = p_data->discover.num_uuid) != 0 &&
            p_data->discover.p_uuid != NULL) {
        if ((bta_dm_search_cb.p_srvc_uuid = (tBT_UUID *)osi_malloc(len)) == NULL) {
            p_data->discover.p_cback(BTA_DM_DISC_CMPL_EVT, NULL);
            return;
        }
        memcpy(bta_dm_search_cb.p_srvc_uuid, p_data->discover.p_uuid, len);
    }
    bta_dm_search_cb.uuid_to_search = bta_dm_search_cb.num_uuid;
#endif

    bta_dm_search_cb.p_search_cback = p_data->discover.p_cback;
    bta_dm_search_cb.sdp_search = p_data->discover.sdp_search;
    bta_dm_search_cb.services_to_search = bta_dm_search_cb.services;
    bta_dm_search_cb.service_index = 0;
    bta_dm_search_cb.services_found = 0;
    bta_dm_search_cb.peer_name[0] = 0;
    bta_dm_search_cb.sdp_search = p_data->discover.sdp_search;
    bta_dm_search_cb.p_btm_inq_info = BTM_InqDbRead (p_data->discover.bd_addr);
    bta_dm_search_cb.transport = p_data->discover.transport;

    bta_dm_search_cb.name_discover_done = FALSE;
    memcpy(&bta_dm_search_cb.uuid, &p_data->discover.uuid, sizeof(tSDP_UUID));
    bta_dm_discover_device(p_data->discover.bd_addr);
}

/*******************************************************************************
**
** Function         bta_dm_di_disc_cmpl
**
** Description      Sends event to application when DI discovery complete
**
** Returns          void
**
*******************************************************************************/
void bta_dm_di_disc_cmpl(tBTA_DM_MSG *p_data)
{
    tBTA_DM_DI_DISC_CMPL    di_disc;

    memset(&di_disc, 0, sizeof(tBTA_DM_DI_DISC_CMPL));
    bdcpy(di_disc.bd_addr, bta_dm_search_cb.peer_bdaddr);

    if ((p_data->hdr.offset == SDP_SUCCESS)
            || (p_data->hdr.offset == SDP_DB_FULL)) {
        di_disc.num_record  = SDP_GetNumDiRecords(bta_dm_di_cb.p_di_db);
    } else {
        di_disc.result      = BTA_FAILURE;
    }

    bta_dm_di_cb.p_di_db = NULL;
    bta_dm_search_cb.p_search_cback(BTA_DM_DI_DISC_CMPL_EVT, (tBTA_DM_SEARCH *) &di_disc);
}

/*******************************************************************************
**
** Function         bta_dm_di_disc_callback
**
** Description      This function queries a remote device for DI information.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_di_disc_callback(UINT16 result)
{
    tBTA_DM_MSG *p_msg;

    if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
        p_msg->hdr.event            = BTA_DM_SEARCH_CMPL_EVT;
        p_msg->hdr.layer_specific   = BTA_DM_API_DI_DISCOVER_EVT;
        p_msg->hdr.offset           = result;
        bta_sys_sendmsg(p_msg);
    }
}
#endif  ///SDP_INCLUDED == TRUE
/*******************************************************************************
**
** Function         bta_dm_disable_search_and_disc
**
** Description      Cancels an ongoing search or discovery for devices in case of
**                  a Bluetooth disable
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_disable_search_and_disc (void)
{
#if (SDP_INCLUDED == TRUE)
    tBTA_DM_DI_DISC_CMPL    di_disc;
#endif ///SDP_INCLUDED == TRUE
    if (bta_dm_search_cb.state != BTA_DM_SEARCH_IDLE) {
        bta_dm_search_cancel(NULL);
    }
#if (SDP_INCLUDED == TRUE)
    if (bta_dm_di_cb.p_di_db != NULL) {
        memset(&di_disc, 0, sizeof(tBTA_DM_DI_DISC_CMPL));
        bdcpy(di_disc.bd_addr, bta_dm_search_cb.peer_bdaddr);
        di_disc.result      = BTA_FAILURE;

        bta_dm_di_cb.p_di_db = NULL;
        bta_dm_search_cb.p_search_cback(BTA_DM_DI_DISC_CMPL_EVT, NULL);
    }
#endif  ///SDP_INCLUDED == TRUE
}

/*******************************************************************************
**
** Function         bta_dm_di_disc
**
** Description      This function queries a remote device for DI information.
**
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
void bta_dm_di_disc (tBTA_DM_MSG *p_data)
{
    UINT16  result = BTA_FAILURE;
    tBTA_DM_MSG *p_msg;

    bta_dm_search_cb.p_search_cback = p_data->di_disc.p_cback;
    bdcpy(bta_dm_search_cb.peer_bdaddr, p_data->di_disc.bd_addr);
    bta_dm_di_cb.p_di_db = p_data->di_disc.p_sdp_db;

    if ((bta_dm_search_cb.p_sdp_db = (tSDP_DISCOVERY_DB *)osi_malloc(BTA_DM_SDP_DB_SIZE)) != NULL) {
        if ( SDP_DiDiscover(bta_dm_search_cb.peer_bdaddr, p_data->di_disc.p_sdp_db,
                            p_data->di_disc.len, bta_dm_di_disc_callback) == SDP_SUCCESS) {
            result = BTA_SUCCESS;
        }
    } else {
        APPL_TRACE_ERROR("No buffer to start DI discovery");
    }

    if ( result == BTA_FAILURE &&
            (p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
        p_msg->hdr.event            = BTA_DM_SEARCH_CMPL_EVT;
        p_msg->hdr.layer_specific   = BTA_DM_API_DI_DISCOVER_EVT;
        p_data->hdr.offset          = result;
        bta_sys_sendmsg(p_msg);
    }
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_read_remote_device_name
**
** Description      Initiate to get remote device name
**
** Returns          TRUE if started to get remote name
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
static BOOLEAN bta_dm_read_remote_device_name (BD_ADDR bd_addr, tBT_TRANSPORT transport)
{
    tBTM_STATUS  btm_status;

    APPL_TRACE_DEBUG("bta_dm_read_remote_device_name");

    bdcpy(bta_dm_search_cb.peer_bdaddr, bd_addr);
    bta_dm_search_cb.peer_name[0] = 0;

    btm_status = BTM_ReadRemoteDeviceName (bta_dm_search_cb.peer_bdaddr,
                                           (tBTM_CMPL_CB *) bta_dm_remname_cback,
                                           transport);

    if ( btm_status == BTM_CMD_STARTED ) {
        APPL_TRACE_DEBUG("bta_dm_read_remote_device_name: BTM_ReadRemoteDeviceName is started");

        return (TRUE);
    } else if ( btm_status == BTM_BUSY ) {
        APPL_TRACE_DEBUG("bta_dm_read_remote_device_name: BTM_ReadRemoteDeviceName is busy");

        /* Remote name discovery is on going now so BTM cannot notify through "bta_dm_remname_cback" */
        /* adding callback to get notified that current reading remore name done */
        BTM_SecAddRmtNameNotifyCallback(&bta_dm_service_search_remname_cback);

        return (TRUE);
    } else {
        APPL_TRACE_WARNING("bta_dm_read_remote_device_name: BTM_ReadRemoteDeviceName returns 0x%02X", btm_status);

        return (FALSE);
    }
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_inq_cmpl
**
** Description      Process the inquiry complete event from BTM
**
** Returns          void
**
*******************************************************************************/
void bta_dm_inq_cmpl (tBTA_DM_MSG *p_data)
{
    tBTA_DM_MSG *p_msg;
    tBTA_DM_SEARCH  data;

    APPL_TRACE_DEBUG("bta_dm_inq_cmpl");

    data.inq_cmpl.num_resps = p_data->inq_cmpl.num;
    bta_dm_search_cb.p_search_cback(BTA_DM_INQ_CMPL_EVT, &data);

    if ((bta_dm_search_cb.p_btm_inq_info = BTM_InqDbFirst()) != NULL) {
        /* start name and service discovery from the first device on inquiry result */
        bta_dm_search_cb.name_discover_done = FALSE;
        bta_dm_search_cb.peer_name[0]       = 0;
#if (SDP_INCLUDED == TRUE)
        bta_dm_discover_device(bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
#endif  ///SDP_INCLUDED == TRUE
    } else {
        /* no devices, search complete */
        bta_dm_search_cb.services = 0;

        if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event          = BTA_DM_SEARCH_CMPL_EVT;
            p_msg->hdr.layer_specific = BTA_DM_API_DISCOVER_EVT;
            bta_sys_sendmsg(p_msg);
        }
    }
}

/*******************************************************************************
**
** Function         bta_dm_rmt_name
**
** Description      Process the remote name result from BTM
**
** Returns          void
**
*******************************************************************************/
void bta_dm_rmt_name (tBTA_DM_MSG *p_data)
{
    APPL_TRACE_DEBUG("bta_dm_rmt_name");

    if ( p_data->rem_name.result.disc_res.bd_name[0] && bta_dm_search_cb.p_btm_inq_info) {
        bta_dm_search_cb.p_btm_inq_info->appl_knows_rem_name = TRUE;
    }
#if (SDP_INCLUDED == TRUE)
    bta_dm_discover_device(bta_dm_search_cb.peer_bdaddr);
#endif  ///SDP_INCLUDED == TRUE
}

/*******************************************************************************
**
** Function         bta_dm_disc_rmt_name
**
** Description      Process the remote name result from BTM when application
**                  wants to find the name for a bdaddr
**
** Returns          void
**
*******************************************************************************/
void bta_dm_disc_rmt_name (tBTA_DM_MSG *p_data)
{
    tBTM_INQ_INFO *p_btm_inq_info;

    APPL_TRACE_DEBUG("bta_dm_disc_rmt_name");

    p_btm_inq_info = BTM_InqDbRead (p_data->rem_name.result.disc_res.bd_addr);
    if ( p_btm_inq_info ) {
        if ( p_data->rem_name.result.disc_res.bd_name[0] ) {
            p_btm_inq_info->appl_knows_rem_name = TRUE;
        }
    }
#if (SDP_INCLUDED == TRUE)
    bta_dm_discover_device(p_data->rem_name.result.disc_res.bd_addr);
#endif  ///SDP_INCLUDED == TRUE
}

/*******************************************************************************
**
** Function         bta_dm_sdp_result
**
** Description      Process the discovery result from sdp
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
void bta_dm_sdp_result (tBTA_DM_MSG *p_data)
{
    tSDP_DISC_REC   *p_sdp_rec = NULL;
    tBTA_DM_MSG     *p_msg;
    BOOLEAN          scn_found = FALSE;
    UINT16           service = 0xFFFF;
    tSDP_PROTOCOL_ELEM  pe;

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
    tBT_UUID           *p_uuid = bta_dm_search_cb.p_srvc_uuid;
    tBTA_DM_SEARCH      result;
    tBT_UUID            service_uuid;
#endif

    UINT32 num_uuids = 0;
    UINT8  uuid_list[32][MAX_UUID_SIZE]; // assuming a max of 32 services

    if ((p_data->sdp_event.sdp_result == SDP_SUCCESS)
            || (p_data->sdp_event.sdp_result == SDP_NO_RECS_MATCH)
            || (p_data->sdp_event.sdp_result == SDP_DB_FULL)) {
        APPL_TRACE_DEBUG("sdp_result::0x%x", p_data->sdp_event.sdp_result);
        do {

            p_sdp_rec = NULL;
            if ( bta_dm_search_cb.service_index == (BTA_USER_SERVICE_ID + 1) ) {
                p_sdp_rec = SDP_FindServiceUUIDInDb(bta_dm_search_cb.p_sdp_db, &bta_dm_search_cb.uuid, p_sdp_rec);

                if (p_sdp_rec && SDP_FindProtocolListElemInRec(p_sdp_rec, UUID_PROTOCOL_RFCOMM, &pe)) {
                    bta_dm_search_cb.peer_scn = (UINT8) pe.params[0];
                    scn_found = TRUE;
                }
            } else {
                service = bta_service_id_to_uuid_lkup_tbl[bta_dm_search_cb.service_index - 1];
                p_sdp_rec = SDP_FindServiceInDb(bta_dm_search_cb.p_sdp_db, service, p_sdp_rec);
            }
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
            /* finished with BR/EDR services, now we check the result for GATT based service UUID */
            if (bta_dm_search_cb.service_index == BTA_MAX_SERVICE_ID) {
                if (bta_dm_search_cb.uuid_to_search != 0 && p_uuid != NULL) {
                    p_uuid +=  (bta_dm_search_cb.num_uuid - bta_dm_search_cb.uuid_to_search);
                    /* only support 16 bits UUID for now */
                    service = p_uuid->uu.uuid16;

                }
                /* all GATT based services */
                do {
                    /* find a service record, report it */
                    p_sdp_rec = SDP_FindServiceInDb(bta_dm_search_cb.p_sdp_db,
                                                    0, p_sdp_rec);
                    if (p_sdp_rec) {
                        if (SDP_FindServiceUUIDInRec(p_sdp_rec, &service_uuid)) {
                            /* send result back to app now, one by one */
                            bdcpy (result.disc_ble_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
                            BCM_STRNCPY_S((char *)result.disc_ble_res.bd_name, sizeof(BD_NAME), bta_dm_get_remname(), (BD_NAME_LEN));
                            result.disc_ble_res.bd_name[BD_NAME_LEN] = 0;
                            result.disc_ble_res.service.len = service_uuid.len;
                            result.disc_ble_res.service.uu.uuid16 = service_uuid.uu.uuid16;

                            bta_dm_search_cb.p_search_cback(BTA_DM_DISC_BLE_RES_EVT, &result);
                        }
                    }

                    if (bta_dm_search_cb.uuid_to_search > 0) {
                        break;
                    }

                } while (p_sdp_rec);
            } else
#endif
            {
                /* SDP_DB_FULL means some records with the
                   required attributes were received */
                if (((p_data->sdp_event.sdp_result == SDP_DB_FULL) &&
                        bta_dm_search_cb.services != BTA_ALL_SERVICE_MASK) ||
                        (p_sdp_rec != NULL)) {
                    if (service != UUID_SERVCLASS_PNP_INFORMATION) {
                        UINT16 tmp_svc = 0xFFFF;
                        bta_dm_search_cb.services_found |=
                            (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(bta_dm_search_cb.service_index - 1));
                        tmp_svc = bta_service_id_to_uuid_lkup_tbl[bta_dm_search_cb.service_index - 1];
                        /* Add to the list of UUIDs */
                        sdpu_uuid16_to_uuid128(tmp_svc, uuid_list[num_uuids]);
                        num_uuids++;
                    }
                }
            }

            if (bta_dm_search_cb.services == BTA_ALL_SERVICE_MASK &&
                    bta_dm_search_cb.services_to_search == 0) {
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
                if ( bta_dm_search_cb.service_index == BTA_BLE_SERVICE_ID &&
                        bta_dm_search_cb.uuid_to_search > 0) {
                    bta_dm_search_cb.uuid_to_search --;
                }

                if (bta_dm_search_cb.uuid_to_search == 0 ||
                        bta_dm_search_cb.service_index != BTA_BLE_SERVICE_ID)
#endif
                    bta_dm_search_cb.service_index++;
            } else { /* regular one service per search or PNP search */
                break;
            }

        } while (bta_dm_search_cb.service_index <= BTA_MAX_SERVICE_ID);

//        osi_free(bta_dm_search_cb.p_sdp_db);
//        bta_dm_search_cb.p_sdp_db = NULL;
        APPL_TRACE_DEBUG("%s services_found = %04x", __FUNCTION__,
                         bta_dm_search_cb.services_found);

        /* Collect the 128-bit services here and put them into the list */
        if (bta_dm_search_cb.services == BTA_ALL_SERVICE_MASK) {
            p_sdp_rec = NULL;
            do {
                tBT_UUID temp_uuid;
                /* find a service record, report it */
                p_sdp_rec = SDP_FindServiceInDb_128bit(bta_dm_search_cb.p_sdp_db, p_sdp_rec);
                if (p_sdp_rec) {
                    if (SDP_FindServiceUUIDInRec_128bit(p_sdp_rec, &temp_uuid)) {
                        memcpy(uuid_list[num_uuids], temp_uuid.uu.uuid128, MAX_UUID_SIZE);
                        num_uuids++;
                    }
                }
            } while (p_sdp_rec);
        }
        /* if there are more services to search for */
        if (bta_dm_search_cb.services_to_search) {
            /* Free up the p_sdp_db before checking the next one */
            bta_dm_free_sdp_db(NULL);
            bta_dm_find_services(bta_dm_search_cb.peer_bdaddr);
        } else {
            /* callbacks */
            /* start next bd_addr if necessary */

            BTM_SecDeleteRmtNameNotifyCallback(&bta_dm_service_search_remname_cback);


            if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
                p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
                p_msg->disc_result.result.disc_res.result = BTA_SUCCESS;
                p_msg->disc_result.result.disc_res.p_raw_data = NULL;
                p_msg->disc_result.result.disc_res.raw_data_size = 0;
                p_msg->disc_result.result.disc_res.num_uuids = num_uuids;
                p_msg->disc_result.result.disc_res.p_uuid_list = NULL;
                if (num_uuids > 0) {
                    p_msg->disc_result.result.disc_res.p_uuid_list = (UINT8 *)osi_malloc(num_uuids * MAX_UUID_SIZE);
                    if (p_msg->disc_result.result.disc_res.p_uuid_list) {
                        memcpy(p_msg->disc_result.result.disc_res.p_uuid_list, uuid_list,
                               num_uuids * MAX_UUID_SIZE);
                    } else {
                        p_msg->disc_result.result.disc_res.num_uuids = 0;
                        APPL_TRACE_ERROR("%s: Unable to allocate memory for uuid_list", __func__);
                    }
                }
                //copy the raw_data to the discovery result  structure
                //

                if (  bta_dm_search_cb.p_sdp_db != NULL && bta_dm_search_cb.p_sdp_db->raw_used != 0   &&
                        bta_dm_search_cb.p_sdp_db->raw_data != NULL) {
                    APPL_TRACE_DEBUG(
                        "%s raw_data used = 0x%x raw_data_ptr = %p", __func__,
                        bta_dm_search_cb.p_sdp_db->raw_used,
                        bta_dm_search_cb.p_sdp_db->raw_data);

                    p_msg->disc_result.result.disc_res.p_raw_data = osi_malloc(bta_dm_search_cb.p_sdp_db->raw_used);
                    if ( NULL != p_msg->disc_result.result.disc_res.p_raw_data  ) {
                        memcpy(     p_msg->disc_result.result.disc_res.p_raw_data,
                                    bta_dm_search_cb.p_sdp_db->raw_data,
                                    bta_dm_search_cb.p_sdp_db->raw_used );

                        p_msg->disc_result.result.disc_res.raw_data_size =
                            bta_dm_search_cb.p_sdp_db->raw_used;

                    } else {
                        APPL_TRACE_DEBUG("%s Alloc failed to allocate %d bytes !!", __func__,
                                         bta_dm_search_cb.p_sdp_db->raw_used);
                    }

                    bta_dm_search_cb.p_sdp_db->raw_data = NULL;     //no need to free this - it is a global assigned.
                    bta_dm_search_cb.p_sdp_db->raw_used = 0;
                    bta_dm_search_cb.p_sdp_db->raw_size = 0;
                } else {
                    APPL_TRACE_DEBUG("%s raw data size is 0 or raw_data is null!!", __func__);
                }
                /* Done with p_sdp_db. Free it */
                bta_dm_free_sdp_db(NULL);
                p_msg->disc_result.result.disc_res.services = bta_dm_search_cb.services_found;

                //Piggy back the SCN over result field
                if ( scn_found ) {
                    p_msg->disc_result.result.disc_res.result = (3 + bta_dm_search_cb.peer_scn);
                    p_msg->disc_result.result.disc_res.services |= BTA_USER_SERVICE_MASK;

                    APPL_TRACE_EVENT(" Piggy back the SCN over result field  SCN=%d", bta_dm_search_cb.peer_scn);

                }
                bdcpy (p_msg->disc_result.result.disc_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
                BCM_STRNCPY_S((char *)p_msg->disc_result.result.disc_res.bd_name, sizeof(BD_NAME),
                              bta_dm_get_remname(), (BD_NAME_LEN - 1));

                /* make sure the string is null terminated */
                p_msg->disc_result.result.disc_res.bd_name[BD_NAME_LEN - 1] = 0;

                bta_sys_sendmsg(p_msg);
            }
        }
    } else {
        /* conn failed. No need for timer */
        if (p_data->sdp_event.sdp_result == SDP_CONN_FAILED || p_data->sdp_event.sdp_result == SDP_CONN_REJECTED
                || p_data->sdp_event.sdp_result == SDP_SECURITY_ERR) {
            bta_dm_search_cb.wait_disc = FALSE;
        }

        /* not able to connect go to next device */
        osi_free(bta_dm_search_cb.p_sdp_db);
        bta_dm_search_cb.p_sdp_db = NULL;

        BTM_SecDeleteRmtNameNotifyCallback(&bta_dm_service_search_remname_cback);

        if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
            p_msg->disc_result.result.disc_res.result = BTA_FAILURE;
            p_msg->disc_result.result.disc_res.services = bta_dm_search_cb.services_found;
            bdcpy (p_msg->disc_result.result.disc_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
            BCM_STRNCPY_S((char *)p_msg->disc_result.result.disc_res.bd_name, sizeof(BD_NAME),
                          bta_dm_get_remname(), (BD_NAME_LEN - 1));

            /* make sure the string is null terminated */
            p_msg->disc_result.result.disc_res.bd_name[BD_NAME_LEN - 1] = 0;

            bta_sys_sendmsg(p_msg);
        }
    }
}
#endif  ///SDP_INCLUDE == TRUE

/*******************************************************************************
**
** Function         bta_dm_search_cmpl
**
** Description      Sends event to application
**
** Returns          void
**
*******************************************************************************/
void bta_dm_search_cmpl (tBTA_DM_MSG *p_data)
{
    APPL_TRACE_EVENT("%s", __func__);

#if (BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE)
    utl_freebuf((void **)&bta_dm_search_cb.p_srvc_uuid);
#endif

    if (p_data->hdr.layer_specific == BTA_DM_API_DI_DISCOVER_EVT) {
 #if (SDP_INCLUDED == TRUE)
        bta_dm_di_disc_cmpl(p_data);
 #endif ///SDP_INCLUDED == TRUE
    } else {
        bta_dm_search_cb.p_search_cback(BTA_DM_DISC_CMPL_EVT, NULL);
    }
}

/*******************************************************************************
**
** Function         bta_dm_disc_result
**
** Description      Service discovery result when discovering services on a device
**
** Returns          void
**
*******************************************************************************/
void bta_dm_disc_result (tBTA_DM_MSG *p_data)
{
    APPL_TRACE_EVENT("%s", __func__);

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
    /* if any BR/EDR service discovery has been done, report the event */
    if ((bta_dm_search_cb.services & ((BTA_ALL_SERVICE_MASK | BTA_USER_SERVICE_MASK ) & ~BTA_BLE_SERVICE_MASK)))
#endif
        bta_dm_search_cb.p_search_cback(BTA_DM_DISC_RES_EVT, &p_data->disc_result.result);

    tBTA_DM_MSG *p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG));

    /* send a message to change state */
    if (p_msg != NULL) {
        p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
        p_msg->hdr.layer_specific = BTA_DM_API_DISCOVER_EVT;
        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         bta_dm_search_result
**
** Description      Service discovery result while searching for devices
**
** Returns          void
**
*******************************************************************************/
void bta_dm_search_result (tBTA_DM_MSG *p_data)
{
    APPL_TRACE_DEBUG("%s searching:0x%04x, result:0x%04x", __func__,
                     bta_dm_search_cb.services,
                     p_data->disc_result.result.disc_res.services);

    /* call back if application wants name discovery or found services that application is searching */
    if (( !bta_dm_search_cb.services )
            || (( bta_dm_search_cb.services ) && ( p_data->disc_result.result.disc_res.services ))) {
        bta_dm_search_cb.p_search_cback(BTA_DM_DISC_RES_EVT, &p_data->disc_result.result);
    }

    /* if searching did not initiate to create link */
    if (!bta_dm_search_cb.wait_disc ) {
        /* if service searching is done with EIR, don't search next device */
        if ( bta_dm_search_cb.p_btm_inq_info ) {
            bta_dm_discover_next_device();
        }
    } else {
        /* wait until link is disconnected or timeout */
        bta_dm_search_cb.sdp_results = TRUE;
        bta_dm_search_cb.search_timer.p_cback = (TIMER_CBACK *)&bta_dm_search_timer_cback;
        bta_sys_start_timer(&bta_dm_search_cb.search_timer, 0, 1000 * (L2CAP_LINK_INACTIVITY_TOUT + 1) );
    }

}

/*******************************************************************************
**
** Function         bta_dm_search_timer_cback
**
** Description      Called when ACL disconnect time is over
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_search_timer_cback (TIMER_LIST_ENT *p_tle)
{
    UNUSED(p_tle);

    APPL_TRACE_EVENT("%s", __func__);
    bta_dm_search_cb.wait_disc = FALSE;

    /* proceed with next device */
    bta_dm_discover_next_device();

}


/*******************************************************************************
**
** Function         bta_dm_free_sdp_db
**
** Description      Frees SDP data base
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
void bta_dm_free_sdp_db (tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    if (bta_dm_search_cb.p_sdp_db) {
        osi_free(bta_dm_search_cb.p_sdp_db);
        bta_dm_search_cb.p_sdp_db = NULL;
    }
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_queue_search
**
** Description      Queues search command while search is being cancelled
**
** Returns          void
**
*******************************************************************************/
void bta_dm_queue_search (tBTA_DM_MSG *p_data)
{
    if (bta_dm_search_cb.p_search_queue) {
        osi_free(bta_dm_search_cb.p_search_queue);
    }

    bta_dm_search_cb.p_search_queue = (tBTA_DM_MSG *)osi_malloc(sizeof(tBTA_DM_API_SEARCH));
    memcpy(bta_dm_search_cb.p_search_queue, p_data, sizeof(tBTA_DM_API_SEARCH));

}

/*******************************************************************************
**
** Function         bta_dm_queue_disc
**
** Description      Queues discovery command while search is being cancelled
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
void bta_dm_queue_disc (tBTA_DM_MSG *p_data)
{
    if (bta_dm_search_cb.p_search_queue) {
        osi_free(bta_dm_search_cb.p_search_queue);
    }

    bta_dm_search_cb.p_search_queue = (tBTA_DM_MSG *)osi_malloc(sizeof(tBTA_DM_API_DISCOVER));
    memcpy(bta_dm_search_cb.p_search_queue, p_data, sizeof(tBTA_DM_API_DISCOVER));
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_search_clear_queue
**
** Description      Clears the queue if API search cancel is called
**
** Returns          void
**
*******************************************************************************/
void bta_dm_search_clear_queue (tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    if (bta_dm_search_cb.p_search_queue) {
        osi_free(bta_dm_search_cb.p_search_queue);
        bta_dm_search_cb.p_search_queue = NULL;
    }
}

/*******************************************************************************
**
** Function         bta_dm_search_cancel_cmpl
**
** Description      Search cancel is complete
**
** Returns          void
**
*******************************************************************************/
void bta_dm_search_cancel_cmpl (tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    if (bta_dm_search_cb.p_search_queue) {
        bta_sys_sendmsg(bta_dm_search_cb.p_search_queue);
        bta_dm_search_cb.p_search_queue = NULL;
    }

}

/*******************************************************************************
**
** Function         bta_dm_search_cancel_transac_cmpl
**
** Description      Current Service Discovery or remote name procedure is
**                  completed after search cancellation
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
void bta_dm_search_cancel_transac_cmpl(tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    if (bta_dm_search_cb.p_sdp_db) {
        osi_free(bta_dm_search_cb.p_sdp_db);
        bta_dm_search_cb.p_sdp_db = NULL;
    }

    bta_dm_search_cancel_notify(NULL);
}
#endif  ///SDP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         bta_dm_search_cancel_notify
**
** Description      Notify application that search has been cancelled
**
** Returns          void
**
*******************************************************************************/
void bta_dm_search_cancel_notify (tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    if (bta_dm_search_cb.p_search_cback) {
        bta_dm_search_cb.p_search_cback(BTA_DM_SEARCH_CANCEL_CMPL_EVT, NULL);
    }
    if (!bta_dm_search_cb.name_discover_done) {
        BTM_CancelRemoteDeviceName();
    }
#if (BLE_INCLUDED == TRUE) && (BTA_GATT_INCLUDED == TRUE) && (SDP_INCLUDED == TRUE) && (GATTC_INCLUDED) == TRUE
    if (bta_dm_search_cb.gatt_disc_active) {
        bta_dm_cancel_gatt_discovery(bta_dm_search_cb.peer_bdaddr);
    }
#endif

}

/*******************************************************************************
**
** Function         bta_dm_find_services
**
** Description      Starts discovery on a device
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
static void bta_dm_find_services ( BD_ADDR bd_addr)
{
    tSDP_UUID    uuid;
    tBTA_DM_MSG *p_msg;

    memset (&uuid, 0, sizeof(tSDP_UUID));

    while (bta_dm_search_cb.service_index < BTA_MAX_SERVICE_ID) {
        if ( bta_dm_search_cb.services_to_search
                & (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(bta_dm_search_cb.service_index))) {
            if ((bta_dm_search_cb.p_sdp_db = (tSDP_DISCOVERY_DB *)osi_malloc(BTA_DM_SDP_DB_SIZE)) != NULL) {
                APPL_TRACE_DEBUG("bta_dm_search_cb.services = %04x***********", bta_dm_search_cb.services);
                /* try to search all services by search based on L2CAP UUID */
                if (bta_dm_search_cb.services == BTA_ALL_SERVICE_MASK ) {
                    LOG_INFO("%s services_to_search=%08x", __func__, bta_dm_search_cb.services_to_search);
                    if (bta_dm_search_cb.services_to_search & BTA_RES_SERVICE_MASK) {
                        uuid.uu.uuid16 = bta_service_id_to_uuid_lkup_tbl[0];
                        bta_dm_search_cb.services_to_search &= ~BTA_RES_SERVICE_MASK;
                    } else {
                        uuid.uu.uuid16 = UUID_PROTOCOL_L2CAP;
                        bta_dm_search_cb.services_to_search = 0;
                    }
                } else {
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
                    /* for LE only profile */
                    if (bta_dm_search_cb.service_index == BTA_BLE_SERVICE_ID) {
                        if (bta_dm_search_cb.uuid_to_search > 0 && bta_dm_search_cb.p_srvc_uuid) {
                            memcpy(&uuid,
                                   (const void *)(bta_dm_search_cb.p_srvc_uuid + \
                                                  bta_dm_search_cb.num_uuid - bta_dm_search_cb.uuid_to_search),
                                   sizeof(tBT_UUID));

                            bta_dm_search_cb.uuid_to_search -- ;
                        } else {
                            uuid.uu.uuid16 = bta_service_id_to_uuid_lkup_tbl[bta_dm_search_cb.service_index];
                        }

                        /* last one? clear the BLE service bit if all discovery has been done */
                        if (bta_dm_search_cb.uuid_to_search == 0)
                            bta_dm_search_cb.services_to_search &=
                                (tBTA_SERVICE_MASK)(~(BTA_SERVICE_ID_TO_SERVICE_MASK(bta_dm_search_cb.service_index)));

                    } else
#endif
                    {
                        /* remove the service from services to be searched  */
                        bta_dm_search_cb.services_to_search &=
                            (tBTA_SERVICE_MASK)(~(BTA_SERVICE_ID_TO_SERVICE_MASK(bta_dm_search_cb.service_index)));
                        uuid.uu.uuid16 = bta_service_id_to_uuid_lkup_tbl[bta_dm_search_cb.service_index];
                    }
                }

                if (uuid.len == 0) {
                    uuid.len = LEN_UUID_16;
                }

                if (bta_dm_search_cb.service_index == BTA_USER_SERVICE_ID) {
                    memcpy(&uuid, &bta_dm_search_cb.uuid, sizeof(tSDP_UUID));
                }

                LOG_INFO("%s search UUID = %04x", __func__, uuid.uu.uuid16);
                SDP_InitDiscoveryDb (bta_dm_search_cb.p_sdp_db, BTA_DM_SDP_DB_SIZE, 1, &uuid, 0, NULL);

                memset(g_disc_raw_data_buf, 0, sizeof(g_disc_raw_data_buf));
                bta_dm_search_cb.p_sdp_db->raw_data = g_disc_raw_data_buf;

                bta_dm_search_cb.p_sdp_db->raw_size = MAX_DISC_RAW_DATA_BUF;

                if (!SDP_ServiceSearchAttributeRequest (bd_addr, bta_dm_search_cb.p_sdp_db, &bta_dm_sdp_callback)) {
                    /* if discovery not successful with this device
                    proceed to next one */
                    osi_free(bta_dm_search_cb.p_sdp_db);
                    bta_dm_search_cb.p_sdp_db = NULL;
                    bta_dm_search_cb.service_index = BTA_MAX_SERVICE_ID;

                } else {
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
                    if ((bta_dm_search_cb.service_index == BTA_BLE_SERVICE_ID &&
                            bta_dm_search_cb.uuid_to_search == 0) ||
                            bta_dm_search_cb.service_index != BTA_BLE_SERVICE_ID)
#endif
                        bta_dm_search_cb.service_index++;
                    return;
                }
            } else {
                APPL_TRACE_ERROR("#### Failed to allocate SDP DB buffer! ####");
            }
        }

        bta_dm_search_cb.service_index++;
    }

    /* no more services to be discovered */
    if (bta_dm_search_cb.service_index >= BTA_MAX_SERVICE_ID) {
        if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
            p_msg->disc_result.result.disc_res.services = bta_dm_search_cb.services_found;
            bdcpy (p_msg->disc_result.result.disc_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
            BCM_STRNCPY_S((char *)p_msg->disc_result.result.disc_res.bd_name, sizeof(BD_NAME),
                          bta_dm_get_remname(), (BD_NAME_LEN - 1));

            /* make sure the string is terminated */
            p_msg->disc_result.result.disc_res.bd_name[BD_NAME_LEN - 1] = 0;

            bta_sys_sendmsg(p_msg);
        }
    }
}
#endif  ///SDP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_discover_next_device
**
** Description      Starts discovery on the next device in Inquiry data base
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_discover_next_device(void)
{

    tBTA_DM_MSG *p_msg;

    APPL_TRACE_DEBUG("bta_dm_discover_next_device");

    /* searching next device on inquiry result */
    if ((bta_dm_search_cb.p_btm_inq_info = BTM_InqDbNext(bta_dm_search_cb.p_btm_inq_info)) != NULL) {
        bta_dm_search_cb.name_discover_done = FALSE;
        bta_dm_search_cb.peer_name[0]       = 0;
#if (SDP_INCLUDED == TRUE)
        bta_dm_discover_device(bta_dm_search_cb.p_btm_inq_info->results.remote_bd_addr);
#endif  ///SDP_INCLUDED == TRUE
    } else {
        /* no devices, search complete */
        bta_dm_search_cb.services = 0;

        if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event          = BTA_DM_SEARCH_CMPL_EVT;
            p_msg->hdr.layer_specific = BTA_DM_API_DISCOVER_EVT;
            bta_sys_sendmsg(p_msg);
        }
    }
}

/*******************************************************************************
**
** Function         bta_dm_discover_device
**
** Description      Starts name and service discovery on the device
**
** Returns          void
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
static void bta_dm_discover_device(BD_ADDR remote_bd_addr)
{
    tBTA_DM_MSG *p_msg;
    tBT_TRANSPORT transport = BT_TRANSPORT_BR_EDR;

#if BLE_INCLUDED == TRUE
    if (bta_dm_search_cb.transport == BTA_TRANSPORT_UNKNOWN) {
        tBT_DEVICE_TYPE dev_type;
        tBLE_ADDR_TYPE  addr_type;

        BTM_ReadDevInfo(remote_bd_addr, &dev_type, &addr_type);
        if (dev_type == BT_DEVICE_TYPE_BLE || addr_type == BLE_ADDR_RANDOM) {
            transport = BT_TRANSPORT_LE;
        }
    } else {
        transport = bta_dm_search_cb.transport;
    }
#endif

    /* Reset transport state for next discovery */
    bta_dm_search_cb.transport = BTA_TRANSPORT_UNKNOWN;

    APPL_TRACE_DEBUG("%s BDA:0x%02X%02X%02X%02X%02X%02X", __func__,
                     remote_bd_addr[0], remote_bd_addr[1],
                     remote_bd_addr[2], remote_bd_addr[3],
                     remote_bd_addr[4], remote_bd_addr[5]);

    bdcpy(bta_dm_search_cb.peer_bdaddr, remote_bd_addr);

    APPL_TRACE_DEBUG("%s name_discover_done = %d p_btm_inq_info %p state = %d, transport=%d",
                     __func__,
                     bta_dm_search_cb.name_discover_done,
                     bta_dm_search_cb.p_btm_inq_info,
                     bta_dm_search_cb.state,
                     transport);

    if (bta_dm_search_cb.p_btm_inq_info) {
        APPL_TRACE_DEBUG("%s appl_knows_rem_name %d", __func__,
                         bta_dm_search_cb.p_btm_inq_info->appl_knows_rem_name);
    }

    if ((bta_dm_search_cb.p_btm_inq_info)
            && (bta_dm_search_cb.p_btm_inq_info->results.device_type == BT_DEVICE_TYPE_BLE)
            && (bta_dm_search_cb.state == BTA_DM_SEARCH_ACTIVE)) {
        /* Do not perform RNR for LE devices at inquiry complete*/
        bta_dm_search_cb.name_discover_done = TRUE;
    }

    /* if name discovery is not done and application needs remote name */
    if ((!bta_dm_search_cb.name_discover_done)
            && (( bta_dm_search_cb.p_btm_inq_info == NULL )
                || (bta_dm_search_cb.p_btm_inq_info && (!bta_dm_search_cb.p_btm_inq_info->appl_knows_rem_name)))) {
        if (bta_dm_read_remote_device_name(bta_dm_search_cb.peer_bdaddr, transport) == TRUE) {
            return;
        }

        /* starting name discovery failed */
        bta_dm_search_cb.name_discover_done = TRUE;
    }

    /* if application wants to discover service */
    if ( bta_dm_search_cb.services ) {
        /* initialize variables */
        bta_dm_search_cb.service_index      = 0;
        bta_dm_search_cb.services_found     = 0;
        bta_dm_search_cb.services_to_search = bta_dm_search_cb.services;
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE && SDP_INCLUDED == TRUE
        bta_dm_search_cb.uuid_to_search     = bta_dm_search_cb.num_uuid;
#endif
        if ((bta_dm_search_cb.p_btm_inq_info != NULL) &&
                bta_dm_search_cb.services != BTA_USER_SERVICE_MASK
                && (bta_dm_search_cb.sdp_search == FALSE)) {
            /* check if EIR provides the information of supported services */
            bta_dm_eir_search_services( &bta_dm_search_cb.p_btm_inq_info->results,
                                        &bta_dm_search_cb.services_to_search,
                                        &bta_dm_search_cb.services_found );
        }

        /* if seaching with EIR is not completed */
        if (bta_dm_search_cb.services_to_search) {
            /* check whether connection already exists to the device
               if connection exists, we don't have to wait for ACL
               link to go down to start search on next device */
            if (BTM_IsAclConnectionUp(bta_dm_search_cb.peer_bdaddr, BT_TRANSPORT_BR_EDR)) {
                bta_dm_search_cb.wait_disc = FALSE;
            } else {
                bta_dm_search_cb.wait_disc = TRUE;
            }

#if (BLE_INCLUDED == TRUE && (defined BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE) && SDP_INCLUDED == TRUE) && (GATTC_INCLUDED == TRUE)
            if ( bta_dm_search_cb.p_btm_inq_info ) {
                APPL_TRACE_DEBUG("%s p_btm_inq_info %p results.device_type 0x%x services_to_search 0x%x",
                                 __func__,
                                 bta_dm_search_cb.p_btm_inq_info,
                                 bta_dm_search_cb.p_btm_inq_info->results.device_type,
                                 bta_dm_search_cb.services_to_search);
            }

            if (transport == BT_TRANSPORT_LE) {
                if (bta_dm_search_cb.services_to_search & BTA_BLE_SERVICE_MASK) {
                    //set the raw data buffer here
                    memset(g_disc_raw_data_buf, 0, sizeof(g_disc_raw_data_buf));
                    bta_dm_search_cb.p_ble_rawdata = g_disc_raw_data_buf;

                    bta_dm_search_cb.ble_raw_size = MAX_DISC_RAW_DATA_BUF;
                    bta_dm_search_cb.ble_raw_used = 0;

                    /* start GATT for service discovery */
                    btm_dm_start_gatt_discovery(bta_dm_search_cb.peer_bdaddr);
                    return;
                }
            } else
#endif
            {
                bta_dm_search_cb.sdp_results = FALSE;
                bta_dm_find_services(bta_dm_search_cb.peer_bdaddr);
                return;
            }
        }
    }

    /* name discovery and service discovery are done for this device */
    if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
        p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
        /* initialize the data structure - includes p_raw_data and raw_data_size */
        memset(&(p_msg->disc_result.result), 0, sizeof(tBTA_DM_DISC_RES));
        p_msg->disc_result.result.disc_res.result = BTA_SUCCESS;
        p_msg->disc_result.result.disc_res.services = bta_dm_search_cb.services_found;
        bdcpy (p_msg->disc_result.result.disc_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
        BCM_STRNCPY_S((char *)p_msg->disc_result.result.disc_res.bd_name,  sizeof(BD_NAME),
                      (char *)bta_dm_search_cb.peer_name, (BD_NAME_LEN - 1));

        /* make sure the string is terminated */
        p_msg->disc_result.result.disc_res.bd_name[BD_NAME_LEN - 1] = 0;

        bta_sys_sendmsg(p_msg);
    }
}

/*******************************************************************************
**
** Function         bta_dm_sdp_callback
**
** Description      Callback from sdp with discovery status
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_sdp_callback (UINT16 sdp_status)
{

    tBTA_DM_SDP_RESULT *p_msg;

    if ((p_msg = (tBTA_DM_SDP_RESULT *) osi_malloc(sizeof(tBTA_DM_SDP_RESULT))) != NULL) {
        p_msg->hdr.event = BTA_DM_SDP_RESULT_EVT;
        p_msg->sdp_result = sdp_status;
        bta_sys_sendmsg(p_msg);

    }
}
#endif  ///SDP_INCLUDED == TRUE
/*******************************************************************************
**
** Function         bta_dm_inq_results_cb
**
** Description      Inquiry results callback from BTM
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_inq_results_cb (tBTM_INQ_RESULTS *p_inq, UINT8 *p_eir)
{

    tBTA_DM_SEARCH     result;
    tBTM_INQ_INFO      *p_inq_info;
    UINT16             service_class;

    bdcpy(result.inq_res.bd_addr, p_inq->remote_bd_addr);
    memcpy(result.inq_res.dev_class, p_inq->dev_class, DEV_CLASS_LEN);
    BTM_COD_SERVICE_CLASS(service_class, p_inq->dev_class);
    result.inq_res.is_limited = (service_class & BTM_COD_SERVICE_LMTD_DISCOVER) ? TRUE : FALSE;
    result.inq_res.rssi = p_inq->rssi;

#if (BLE_INCLUDED == TRUE)
    result.inq_res.ble_addr_type    = p_inq->ble_addr_type;
    result.inq_res.inq_result_type  = p_inq->inq_result_type;
    result.inq_res.device_type      = p_inq->device_type;
    result.inq_res.flag             = p_inq->flag;
#endif

    /* application will parse EIR to find out remote device name */
    result.inq_res.p_eir = p_eir;

    if ((p_inq_info = BTM_InqDbRead(p_inq->remote_bd_addr)) != NULL) {
        /* initialize remt_name_not_required to FALSE so that we get the name by default */
        result.inq_res.remt_name_not_required = FALSE;

    }

    if (bta_dm_search_cb.p_search_cback) {
        bta_dm_search_cb.p_search_cback(BTA_DM_INQ_RES_EVT, &result);
    }

    if (p_inq_info) {
        /* application indicates if it knows the remote name, inside the callback
         copy that to the inquiry data base*/
        if (result.inq_res.remt_name_not_required) {
            p_inq_info->appl_knows_rem_name = TRUE;
        }

    }


}


/*******************************************************************************
**
** Function         bta_dm_inq_cmpl_cb
**
** Description      Inquiry complete callback from BTM
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_inq_cmpl_cb (void *p_result)
{
    tBTA_DM_MSG *p_msg;

    if (bta_dm_search_cb.cancel_pending == FALSE) {
        APPL_TRACE_DEBUG("%s", __FUNCTION__);
        p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG));
        if (p_msg != NULL) {
            p_msg->inq_cmpl.hdr.event = BTA_DM_INQUIRY_CMPL_EVT;
            p_msg->inq_cmpl.num = ((tBTM_INQUIRY_CMPL *)p_result)->num_resp;
            bta_sys_sendmsg(p_msg);
        }
    } else {
        bta_dm_search_cb.cancel_pending = FALSE;
        bta_dm_search_cancel_notify(NULL);

        p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG));
        if (p_msg != NULL) {
            p_msg->hdr.event = BTA_DM_SEARCH_CMPL_EVT;
            p_msg->hdr.layer_specific = BTA_DM_API_DISCOVER_EVT;
            bta_sys_sendmsg(p_msg);
        }
    }
}

/*******************************************************************************
**
** Function         bta_dm_service_search_remname_cback
**
** Description      Remote name call back from BTM during service discovery
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_service_search_remname_cback (BD_ADDR bd_addr, DEV_CLASS dc, BD_NAME bd_name)
{
    tBTM_REMOTE_DEV_NAME    rem_name;
    tBTM_STATUS             btm_status;
    UNUSED(dc);

    APPL_TRACE_DEBUG("bta_dm_service_search_remname_cback name=<%s>", bd_name);

    /* if this is what we are looking for */
    if (!bdcmp( bta_dm_search_cb.peer_bdaddr, bd_addr)) {
        rem_name.length = strlen((char *)bd_name);
        if (rem_name.length > (BD_NAME_LEN - 1)) {
            rem_name.length = (BD_NAME_LEN - 1);
            rem_name.remote_bd_name[(BD_NAME_LEN - 1)] = 0;
        }
        BCM_STRNCPY_S((char *)rem_name.remote_bd_name,  sizeof(BD_NAME), (char *)bd_name, (BD_NAME_LEN - 1));
        rem_name.status = BTM_SUCCESS;

        bta_dm_remname_cback(&rem_name);
    } else {
        /* get name of device */
        btm_status = BTM_ReadRemoteDeviceName (bta_dm_search_cb.peer_bdaddr,
                                               (tBTM_CMPL_CB *) bta_dm_remname_cback,
                                               BT_TRANSPORT_BR_EDR);
        if ( btm_status == BTM_BUSY ) {
            /* wait for next chance(notification of remote name discovery done) */
            APPL_TRACE_DEBUG("bta_dm_service_search_remname_cback: BTM_ReadRemoteDeviceName is busy");
        } else if ( btm_status != BTM_CMD_STARTED ) {
            /* if failed to start getting remote name then continue */
            APPL_TRACE_WARNING("bta_dm_service_search_remname_cback: BTM_ReadRemoteDeviceName returns 0x%02X", btm_status);

            rem_name.length = 0;
            rem_name.remote_bd_name[0] = 0;
            rem_name.status = btm_status;
            bta_dm_remname_cback(&rem_name);
        }
    }
}


/*******************************************************************************
**
** Function         bta_dm_remname_cback
**
** Description      Remote name complete call back from BTM
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_remname_cback (tBTM_REMOTE_DEV_NAME *p_remote_name)
{
    tBTA_DM_REM_NAME *p_msg;

    APPL_TRACE_DEBUG("bta_dm_remname_cback len = %d name=<%s>", p_remote_name->length,
                     p_remote_name->remote_bd_name);

    /* remote name discovery is done but it could be failed */
    bta_dm_search_cb.name_discover_done = TRUE;
    BCM_STRNCPY_S((char *)bta_dm_search_cb.peer_name, sizeof(BD_NAME), (char *)p_remote_name->remote_bd_name, (BD_NAME_LEN));
    bta_dm_search_cb.peer_name[BD_NAME_LEN] = 0;

    BTM_SecDeleteRmtNameNotifyCallback(&bta_dm_service_search_remname_cback);

#if BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE
    if (bta_dm_search_cb.transport == BT_TRANSPORT_LE ) {
        GAP_BleReadPeerPrefConnParams (bta_dm_search_cb.peer_bdaddr);
    }
#endif

    if ((p_msg = (tBTA_DM_REM_NAME *) osi_malloc(sizeof(tBTA_DM_REM_NAME))) != NULL) {
        bdcpy (p_msg->result.disc_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
        BCM_STRNCPY_S((char *)p_msg->result.disc_res.bd_name, sizeof(BD_NAME), (char *)p_remote_name->remote_bd_name, (BD_NAME_LEN));

        /* make sure the string is null terminated */
        p_msg->result.disc_res.bd_name[BD_NAME_LEN] = 0;

        p_msg->hdr.event = BTA_DM_REMT_NAME_EVT;
        bta_sys_sendmsg(p_msg);

    }
}

/*******************************************************************************
**
** Function         bta_dm_authorize_cback
**
** Description      cback requesting authorization
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
static UINT8 bta_dm_authorize_cback (BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name,
                                     UINT8 *service_name, UINT8 service_id, BOOLEAN is_originator)
{
    tBTA_DM_SEC sec_event;
    UINT8       index = 1;
    UNUSED(service_name);
    UNUSED(is_originator);

    bdcpy(sec_event.authorize.bd_addr, bd_addr);
    memcpy(sec_event.authorize.dev_class, dev_class, DEV_CLASS_LEN);

    BCM_STRNCPY_S((char *)sec_event.authorize.bd_name, sizeof(BD_NAME), (char *)bd_name, (BD_NAME_LEN - 1));

    /* make sure the string is null terminated */
    sec_event.authorize.bd_name[BD_NAME_LEN - 1] = 0;

#if ( defined(BTA_JV_INCLUDED) && BTA_JV_INCLUDED == TRUE )
    sec_event.authorize.service = service_id;
#endif

    while (index < BTA_MAX_SERVICE_ID) {
        /* get the BTA service id corresponding to BTM id */
        if (bta_service_id_to_btm_srv_id_lkup_tbl[index] == service_id) {
            sec_event.authorize.service = index;
            break;
        }
        index++;
    }


    /* if supported service callback otherwise not authorized */
    if (bta_dm_cb.p_sec_cback && (index < BTA_MAX_SERVICE_ID
#if ( defined(BTA_JV_INCLUDED) && BTA_JV_INCLUDED == TRUE )
                                  /* pass through JV service ID */
                                  || (service_id >= BTA_FIRST_JV_SERVICE_ID && service_id <= BTA_LAST_JV_SERVICE_ID)
#endif
                                 )) {
        bta_dm_cb.p_sec_cback(BTA_DM_AUTHORIZE_EVT, &sec_event);
        return BTM_CMD_STARTED;
    } else {
        return BTM_NOT_AUTHORIZED;
    }
}
 



/*******************************************************************************
**
** Function         bta_dm_pinname_cback
**
** Description      Callback requesting pin_key
**
** Returns          void
**
*******************************************************************************/
 static void bta_dm_pinname_cback (void *p_data)
{
    tBTM_REMOTE_DEV_NAME *p_result = (tBTM_REMOTE_DEV_NAME *)p_data;
    tBTA_DM_SEC           sec_event;
    UINT32                bytes_to_copy;
    tBTA_DM_SEC_EVT       event = bta_dm_cb.pin_evt;

    if (BTA_DM_SP_CFM_REQ_EVT == event) {
        /* Retrieved saved device class and bd_addr */
        bdcpy(sec_event.cfm_req.bd_addr, bta_dm_cb.pin_bd_addr);
        BTA_COPY_DEVICE_CLASS(sec_event.cfm_req.dev_class, bta_dm_cb.pin_dev_class);

        if (p_result && p_result->status == BTM_SUCCESS) {
            bytes_to_copy = (p_result->length < (BD_NAME_LEN - 1))
                            ? p_result->length : (BD_NAME_LEN - 1);
            memcpy(sec_event.cfm_req.bd_name, p_result->remote_bd_name, bytes_to_copy);
            sec_event.pin_req.bd_name[BD_NAME_LEN - 1] = 0;
        } else { /* No name found */
            sec_event.cfm_req.bd_name[0] = 0;
        }

        sec_event.key_notif.passkey    = bta_dm_cb.num_val; /* get PIN code numeric number */

        /* 1 additional event data fields for this event */
        sec_event.cfm_req.just_works = bta_dm_cb.just_works;
    } else {
        /* Retrieved saved device class and bd_addr */
        bdcpy(sec_event.pin_req.bd_addr, bta_dm_cb.pin_bd_addr);
        BTA_COPY_DEVICE_CLASS(sec_event.pin_req.dev_class, bta_dm_cb.pin_dev_class);

        if (p_result && p_result->status == BTM_SUCCESS) {
            bytes_to_copy = (p_result->length < (BD_NAME_LEN - 1))
                            ? p_result->length : (BD_NAME_LEN - 1);
            memcpy(sec_event.pin_req.bd_name, p_result->remote_bd_name, bytes_to_copy);
            sec_event.pin_req.bd_name[BD_NAME_LEN - 1] = 0;
        } else { /* No name found */
            sec_event.pin_req.bd_name[0] = 0;
        }

        event = bta_dm_cb.pin_evt;
        sec_event.key_notif.passkey    = bta_dm_cb.num_val; /* get PIN code numeric number */
    }

    if ( bta_dm_cb.p_sec_cback ) {
        bta_dm_cb.p_sec_cback(event, &sec_event);
    }
}

/*******************************************************************************
**
** Function         bta_dm_pin_cback
**
** Description      Callback requesting pin_key
**
** Returns          void
**
*******************************************************************************/
static UINT8 bta_dm_pin_cback (BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name,
                               BOOLEAN min_16_digit)
{
    tBTA_DM_SEC sec_event;

    if (!bta_dm_cb.p_sec_cback) {
        return BTM_NOT_AUTHORIZED;
    }

    /* If the device name is not known, save bdaddr and devclass and initiate a name request */
    if (bd_name[0] == 0) {
        bta_dm_cb.pin_evt = BTA_DM_PIN_REQ_EVT;
        bdcpy(bta_dm_cb.pin_bd_addr, bd_addr);
        BTA_COPY_DEVICE_CLASS(bta_dm_cb.pin_dev_class, dev_class);
        if ((BTM_ReadRemoteDeviceName(bd_addr, bta_dm_pinname_cback, BT_TRANSPORT_BR_EDR)) == BTM_CMD_STARTED) {
            return BTM_CMD_STARTED;
        }

        APPL_TRACE_WARNING(" bta_dm_pin_cback() -> Failed to start Remote Name Request  ");
    }

    bdcpy(sec_event.pin_req.bd_addr, bd_addr);
    BTA_COPY_DEVICE_CLASS(sec_event.pin_req.dev_class, dev_class);
    BCM_STRNCPY_S((char *)sec_event.pin_req.bd_name, sizeof(BD_NAME), (char *)bd_name, (BD_NAME_LEN - 1));
    sec_event.pin_req.bd_name[BD_NAME_LEN - 1] = 0;
    sec_event.pin_req.min_16_digit = min_16_digit;

    bta_dm_cb.p_sec_cback(BTA_DM_PIN_REQ_EVT, &sec_event);
    return BTM_CMD_STARTED;
}

/*******************************************************************************
**
** Function         bta_dm_new_link_key_cback
**
** Description      Callback from BTM to notify new link key
**
** Returns          void
**
*******************************************************************************/
static UINT8  bta_dm_new_link_key_cback(BD_ADDR bd_addr, DEV_CLASS dev_class,
                                        BD_NAME bd_name, LINK_KEY key, UINT8 key_type)
{
    tBTA_DM_SEC sec_event;
    tBTA_DM_AUTH_CMPL *p_auth_cmpl;
    UINT8             event;
    UNUSED(dev_class);

    memset (&sec_event, 0, sizeof(tBTA_DM_SEC));

    /* Not AMP Key type */
    if (key_type != HCI_LKEY_TYPE_AMP_WIFI && key_type != HCI_LKEY_TYPE_AMP_UWB) {
        event = BTA_DM_AUTH_CMPL_EVT;
        p_auth_cmpl = &sec_event.auth_cmpl;

        bdcpy(p_auth_cmpl->bd_addr, bd_addr);

        memcpy(p_auth_cmpl->bd_name, bd_name, (BD_NAME_LEN - 1));
        p_auth_cmpl->bd_name[BD_NAME_LEN - 1] = 0;

        p_auth_cmpl->key_present = TRUE;
        p_auth_cmpl->key_type = key_type;
        p_auth_cmpl->success = TRUE;

        memcpy(p_auth_cmpl->key, key, LINK_KEY_LEN);
        sec_event.auth_cmpl.fail_reason = HCI_SUCCESS;

#if BLE_INCLUDED == TRUE
        // Report the BR link key based on the BR/EDR address and type
        BTM_ReadDevInfo(bd_addr, &sec_event.auth_cmpl.dev_type, &sec_event.auth_cmpl.addr_type);
#endif
        if (bta_dm_cb.p_sec_cback) {
            bta_dm_cb.p_sec_cback(event, &sec_event);
        }
    } else {
        APPL_TRACE_WARNING("%s() Received AMP Key", __func__);
    }

    return BTM_CMD_STARTED;
}


/*******************************************************************************
**
** Function         bta_dm_authentication_complete_cback
**
** Description      Authentication complete callback from BTM
**
** Returns          void
**
*******************************************************************************/
static UINT8 bta_dm_authentication_complete_cback(BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name, int result)
{
    tBTA_DM_SEC sec_event;
    UNUSED(dev_class);

    if (result != BTM_SUCCESS) {
        memset(&sec_event, 0, sizeof(tBTA_DM_SEC));
        bdcpy(sec_event.auth_cmpl.bd_addr, bd_addr);

        memcpy(sec_event.auth_cmpl.bd_name, bd_name, (BD_NAME_LEN - 1));
        sec_event.auth_cmpl.bd_name[BD_NAME_LEN - 1] = 0;

#if BLE_INCLUDED == TRUE
        // Report the BR link key based on the BR/EDR address and type
        BTM_ReadDevInfo(bd_addr, &sec_event.auth_cmpl.dev_type, &sec_event.auth_cmpl.addr_type);
#endif
        sec_event.auth_cmpl.fail_reason = (UINT8)result;

        if (bta_dm_cb.p_sec_cback) {
            bta_dm_cb.p_sec_cback(BTA_DM_AUTH_CMPL_EVT, &sec_event);
        }

        bta_dm_remove_sec_dev_entry(bd_addr);
    }

    return BTM_SUCCESS;
}

#if (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE)
/*******************************************************************************
**
** Function         bta_dm_sp_cback
**
** Description      simple pairing callback from BTM
**
** Returns          void
**
*******************************************************************************/
static UINT8 bta_dm_sp_cback (tBTM_SP_EVT event, tBTM_SP_EVT_DATA *p_data)
{
    tBTM_STATUS status = BTM_CMD_STARTED;
    tBTA_DM_SEC sec_event;
    tBTA_DM_SEC_EVT pin_evt = BTA_DM_SP_KEY_NOTIF_EVT;

    APPL_TRACE_EVENT("bta_dm_sp_cback: %d", event);
    if (!bta_dm_cb.p_sec_cback) {
        return BTM_NOT_AUTHORIZED;
    }

    /* TODO_SP */
    switch (event) {
    case BTM_SP_IO_REQ_EVT:
#if (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE)
        /* translate auth_req */
        bta_dm_co_io_req(p_data->io_req.bd_addr, &p_data->io_req.io_cap,
                         &p_data->io_req.oob_data, &p_data->io_req.auth_req, p_data->io_req.is_orig);
#endif
#if BTM_OOB_INCLUDED == FALSE
        status = BTM_SUCCESS;
#endif

        APPL_TRACE_EVENT("io mitm: %d oob_data:%d", p_data->io_req.auth_req, p_data->io_req.oob_data);
        break;
    case BTM_SP_IO_RSP_EVT:
#if (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE)
        bta_dm_co_io_rsp(p_data->io_rsp.bd_addr, p_data->io_rsp.io_cap,
                         p_data->io_rsp.oob_data, p_data->io_rsp.auth_req );
#endif
        break;

    case BTM_SP_CFM_REQ_EVT:
        pin_evt = BTA_DM_SP_CFM_REQ_EVT;
        bta_dm_cb.just_works = sec_event.cfm_req.just_works = p_data->cfm_req.just_works;
        sec_event.cfm_req.loc_auth_req = p_data->cfm_req.loc_auth_req;
        sec_event.cfm_req.rmt_auth_req = p_data->cfm_req.rmt_auth_req;
        sec_event.cfm_req.loc_io_caps = p_data->cfm_req.loc_io_caps;
        sec_event.cfm_req.rmt_io_caps = p_data->cfm_req.rmt_io_caps;

        /* continue to next case */
#if (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE)
    /* Passkey entry mode, mobile device with output capability is very
        unlikely to receive key request, so skip this event */
    /*case BTM_SP_KEY_REQ_EVT: */
    case BTM_SP_KEY_NOTIF_EVT:
#endif
        if (BTM_SP_CFM_REQ_EVT == event) {
            /* Due to the switch case falling through below to BTM_SP_KEY_NOTIF_EVT,
               call remote name request using values from cfm_req */
            if (p_data->cfm_req.bd_name[0] == 0) {
                bta_dm_cb.pin_evt = pin_evt;
                bdcpy(bta_dm_cb.pin_bd_addr, p_data->cfm_req.bd_addr);
                BTA_COPY_DEVICE_CLASS(bta_dm_cb.pin_dev_class, p_data->cfm_req.dev_class);
                if ((BTM_ReadRemoteDeviceName(p_data->cfm_req.bd_addr, bta_dm_pinname_cback,
                                              BT_TRANSPORT_BR_EDR)) == BTM_CMD_STARTED) {
                    return BTM_CMD_STARTED;
                }
                APPL_TRACE_WARNING(" bta_dm_sp_cback() -> Failed to start Remote Name Request  ");
            } else {
                /* Due to the switch case falling through below to BTM_SP_KEY_NOTIF_EVT,
                   copy these values into key_notif from cfm_req */
                bdcpy(sec_event.key_notif.bd_addr, p_data->cfm_req.bd_addr);
                BTA_COPY_DEVICE_CLASS(sec_event.key_notif.dev_class, p_data->cfm_req.dev_class);
                BCM_STRNCPY_S((char *)sec_event.key_notif.bd_name, sizeof(BD_NAME),
                              (char *)p_data->cfm_req.bd_name, (BD_NAME_LEN - 1));
                sec_event.key_notif.bd_name[BD_NAME_LEN - 1] = 0;
            }
        }

        bta_dm_cb.num_val = sec_event.key_notif.passkey = p_data->key_notif.passkey;
        if (BTM_SP_KEY_NOTIF_EVT == event) {
            /* If the device name is not known, save bdaddr and devclass
               and initiate a name request with values from key_notif */
            if (p_data->key_notif.bd_name[0] == 0) {
                bta_dm_cb.pin_evt = pin_evt;
                bdcpy(bta_dm_cb.pin_bd_addr, p_data->key_notif.bd_addr);
                BTA_COPY_DEVICE_CLASS(bta_dm_cb.pin_dev_class, p_data->key_notif.dev_class);
                if ((BTM_ReadRemoteDeviceName(p_data->key_notif.bd_addr, bta_dm_pinname_cback,
                                              BT_TRANSPORT_BR_EDR)) == BTM_CMD_STARTED) {
                    return BTM_CMD_STARTED;
                }
                APPL_TRACE_WARNING(" bta_dm_sp_cback() -> Failed to start Remote Name Request  ");
            } else {
                bdcpy(sec_event.key_notif.bd_addr, p_data->key_notif.bd_addr);
                BTA_COPY_DEVICE_CLASS(sec_event.key_notif.dev_class, p_data->key_notif.dev_class);
                BCM_STRNCPY_S((char *)sec_event.key_notif.bd_name, sizeof(BD_NAME),
                              (char *)p_data->key_notif.bd_name, (BD_NAME_LEN - 1));
                sec_event.key_notif.bd_name[BD_NAME_LEN - 1] = 0;
            }
        }

        bta_dm_cb.p_sec_cback(pin_evt, &sec_event);

        break;

#if BTM_OOB_INCLUDED == TRUE
    case BTM_SP_LOC_OOB_EVT:
        bta_dm_co_loc_oob((BOOLEAN)(p_data->loc_oob.status == BTM_SUCCESS),
                          p_data->loc_oob.c, p_data->loc_oob.r);
        break;

    case BTM_SP_RMT_OOB_EVT:
        /* If the device name is not known, save bdaddr and devclass and initiate a name request */
        if (p_data->rmt_oob.bd_name[0] == 0) {
            bta_dm_cb.pin_evt = BTA_DM_SP_RMT_OOB_EVT;
            bdcpy(bta_dm_cb.pin_bd_addr, p_data->rmt_oob.bd_addr);
            BTA_COPY_DEVICE_CLASS(bta_dm_cb.pin_dev_class, p_data->rmt_oob.dev_class);
            if ((BTM_ReadRemoteDeviceName(p_data->rmt_oob.bd_addr, bta_dm_pinname_cback,
                                          BT_TRANSPORT_BR_EDR)) == BTM_CMD_STARTED) {
                return BTM_CMD_STARTED;
            }
            APPL_TRACE_WARNING(" bta_dm_sp_cback() -> Failed to start Remote Name Request  ");
        }

        bdcpy(sec_event.rmt_oob.bd_addr, p_data->rmt_oob.bd_addr);
        BTA_COPY_DEVICE_CLASS(sec_event.rmt_oob.dev_class, p_data->rmt_oob.dev_class);
        BCM_STRNCPY_S((char *)sec_event.rmt_oob.bd_name, sizeof(BD_NAME), (char *)p_data->rmt_oob.bd_name, (BD_NAME_LEN - 1));
        sec_event.rmt_oob.bd_name[BD_NAME_LEN - 1] = 0;

        bta_dm_cb.p_sec_cback(BTA_DM_SP_RMT_OOB_EVT, &sec_event);

        bta_dm_co_rmt_oob(p_data->rmt_oob.bd_addr);
        break;
#endif
    case BTM_SP_COMPLT_EVT:
        /* do not report this event - handled by link_key_callback or auth_complete_callback */
        break;

    case BTM_SP_KEYPRESS_EVT:
        memcpy(&sec_event.key_press, &p_data->key_press, sizeof(tBTM_SP_KEYPRESS));
        bta_dm_cb.p_sec_cback(BTA_DM_SP_KEYPRESS_EVT, &sec_event);
        break;

    case BTM_SP_UPGRADE_EVT:
        bta_dm_co_lk_upgrade(p_data->upgrade.bd_addr, &p_data->upgrade.upgrade );
        break;

    default:
        status = BTM_NOT_AUTHORIZED;
        break;
    }
    APPL_TRACE_EVENT("dm status: %d", status);
    return status;
}
#endif /* (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE) */

#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         bta_dm_local_name_cback
**
** Description      Callback from btm after local name is read
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_local_name_cback(UINT8 *p_name)
{
    tBTA_DM_SEC sec_event;
    UNUSED(p_name);

    sec_event.enable.status = BTA_SUCCESS;

    if (bta_dm_cb.p_sec_cback) {
        bta_dm_cb.p_sec_cback(BTA_DM_ENABLE_EVT, &sec_event);
    }

}

/*******************************************************************************
**
** Function         bta_dm_bl_change_cback
**
** Description      Callback from btm when acl connection goes up or down
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_bl_change_cback (tBTM_BL_EVENT_DATA *p_data)
{
    tBTA_DM_ACL_CHANGE *p_msg;

    if ((p_msg = (tBTA_DM_ACL_CHANGE *) osi_malloc(sizeof(tBTA_DM_ACL_CHANGE))) != NULL) {
        p_msg->event = p_data->event;
        p_msg->is_new = FALSE;

        switch (p_msg->event) {
        case BTM_BL_CONN_EVT:
            p_msg->is_new = TRUE;
            bdcpy(p_msg->bd_addr, p_data->conn.p_bda);
#if BLE_INCLUDED == TRUE
            p_msg->transport = p_data->conn.transport;
            p_msg->handle = p_data->conn.handle;
#endif
            break;
        case BTM_BL_DISCN_EVT:
            bdcpy(p_msg->bd_addr, p_data->discn.p_bda);
#if BLE_INCLUDED == TRUE
            p_msg->transport = p_data->discn.transport;
            p_msg->handle = p_data->discn.handle;
#endif
            break;
        case BTM_BL_UPDATE_EVT:
            p_msg->busy_level = p_data->update.busy_level;
            p_msg->busy_level_flags = p_data->update.busy_level_flags;
            break;
        case BTM_BL_ROLE_CHG_EVT:
            p_msg->new_role = p_data->role_chg.new_role;
            p_msg->hci_status = p_data->role_chg.hci_status;
            bdcpy(p_msg->bd_addr, p_data->role_chg.p_bda);
            break;
        case BTM_BL_COLLISION_EVT:
            bdcpy(p_msg->bd_addr, p_data->conn.p_bda);
            break;
        }

        p_msg->hdr.event = BTA_DM_ACL_CHANGE_EVT;
        bta_sys_sendmsg(p_msg);

    }

}

/*******************************************************************************
**
** Function         bta_dm_rs_cback
**
** Description      Receives the role switch complete event
**
** Returns
**
*******************************************************************************/
static void bta_dm_rs_cback (tBTM_ROLE_SWITCH_CMPL *p1)
{
    UNUSED(p1);
    APPL_TRACE_WARNING("bta_dm_rs_cback:%d", bta_dm_cb.rs_event);
    if (bta_dm_cb.rs_event == BTA_DM_API_SEARCH_EVT) {
        bta_dm_cb.search_msg.rs_res = BTA_DM_RS_OK; /* do not care about the result for now */
        bta_dm_cb.rs_event = 0;
        bta_dm_search_start((tBTA_DM_MSG *)&bta_dm_cb.search_msg);
    }
}

/*******************************************************************************
**
** Function         bta_dm_check_av
**
** Description      This function checks if AV is active
**                  if yes, make sure the AV link is master
**
** Returns          BOOLEAN - TRUE, if switch is in progress
**
*******************************************************************************/
static BOOLEAN bta_dm_check_av(UINT16 event)
{
    BOOLEAN avoid_roleswitch = FALSE;
    BOOLEAN switching = FALSE;
    UINT8 i;
    tBTA_DM_PEER_DEVICE *p_dev;

#if defined(BTA_DM_AVOID_A2DP_ROLESWITCH_ON_INQUIRY) && (BTA_DM_AVOID_A2DP_ROLESWITCH_ON_INQUIRY == TRUE)

    /* avoid role switch upon inquiry if a2dp is actively streaming as it
       introduces an audioglitch due to FW scheduling delays (unavoidable) */
    if (event == BTA_DM_API_SEARCH_EVT) {
        avoid_roleswitch = TRUE;
    }
#endif

    APPL_TRACE_WARNING("bta_dm_check_av:%d", bta_dm_cb.cur_av_count);
    if (bta_dm_cb.cur_av_count) {
        for (i = 0; i < bta_dm_cb.device_list.count; i++) {
            p_dev = &bta_dm_cb.device_list.peer_device[i];
            APPL_TRACE_WARNING("[%d]: state:%d, info:x%x, avoid_rs %d",
                               i, p_dev->conn_state, p_dev->info, avoid_roleswitch);
            if ((p_dev->conn_state == BTA_DM_CONNECTED) && (p_dev->info & BTA_DM_DI_AV_ACTIVE) &&
                    (avoid_roleswitch == FALSE)) {
                /* make master and take away the role switch policy */
                if (BTM_CMD_STARTED == BTM_SwitchRole (p_dev->peer_bdaddr, HCI_ROLE_MASTER, (tBTM_CMPL_CB *)bta_dm_rs_cback)) {
                    /* the role switch command is actually sent */
                    bta_dm_cb.rs_event = event;
                    switching = TRUE;
                }
                /* else either already master or can not switch for some reasons */
                bta_dm_policy_cback(BTA_SYS_PLCY_CLR, 0, HCI_ENABLE_MASTER_SLAVE_SWITCH, p_dev->peer_bdaddr);
                break;
            }
        }
    }
    return switching;
}

/*******************************************************************************
**
** Function         bta_dm_acl_change
**
** Description      Process BTA_DM_ACL_CHANGE_EVT
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_acl_change(tBTA_DM_MSG *p_data)
{

    UINT8 i;
    UINT8 *p;
    tBTA_DM_SEC conn;
    BOOLEAN is_new = p_data->acl_change.is_new;
    BD_ADDR_PTR     p_bda = p_data->acl_change.bd_addr;
    BOOLEAN         need_policy_change = FALSE;
    BOOLEAN         issue_unpair_cb = FALSE;

    tBTA_DM_PEER_DEVICE *p_dev;
    memset(&conn, 0, sizeof(tBTA_DM_SEC));

    switch (p_data->acl_change.event) {
    case BTM_BL_UPDATE_EVT:     /* busy level update */
        if ( bta_dm_cb.p_sec_cback ) {
            conn.busy_level.level = p_data->acl_change.busy_level;
            conn.busy_level.level_flags = p_data->acl_change.busy_level_flags;
            bta_dm_cb.p_sec_cback(BTA_DM_BUSY_LEVEL_EVT, &conn);
        }
        return;

    case BTM_BL_ROLE_CHG_EVT:   /* role change event */
        p_dev = bta_dm_find_peer_device(p_bda);
        if (p_dev) {
            APPL_TRACE_DEBUG("bta_dm_acl_change role chg info:x%x new_role:%d dev count:%d",
                             p_dev->info, p_data->acl_change.new_role, bta_dm_cb.device_list.count);
            if (p_dev->info & BTA_DM_DI_AV_ACTIVE) {
                /* there's AV activity on this link */
                if (p_data->acl_change.new_role == HCI_ROLE_SLAVE && bta_dm_cb.device_list.count > 1
                        && p_data->acl_change.hci_status == HCI_SUCCESS) {
                    /* more than one connections and the AV connection is role switched to slave
                     * switch it back to master and remove the switch policy */
                    BTM_SwitchRole(p_bda, BTM_ROLE_MASTER, NULL);
                    need_policy_change = TRUE;
                } else if (p_bta_dm_cfg->avoid_scatter && (p_data->acl_change.new_role == HCI_ROLE_MASTER)) {
                    /* if the link updated to be master include AV activities, remove the switch policy */
                    need_policy_change = TRUE;
                }

                if (need_policy_change) {
                    bta_dm_policy_cback(BTA_SYS_PLCY_CLR, 0, HCI_ENABLE_MASTER_SLAVE_SWITCH, p_dev->peer_bdaddr);
                }
            } else {
                /* there's AV no activity on this link and role switch happened
                 * check if AV is active
                 * if so, make sure the AV link is master */
                bta_dm_check_av(0);
            }
            bta_sys_notify_role_chg(p_data->acl_change.bd_addr, p_data->acl_change.new_role, p_data->acl_change.hci_status);
            bdcpy(conn.role_chg.bd_addr, p_bda);
            conn.role_chg.new_role = (UINT8) p_data->acl_change.new_role;
            if ( bta_dm_cb.p_sec_cback ) {
                bta_dm_cb.p_sec_cback(BTA_DM_ROLE_CHG_EVT, (tBTA_DM_SEC *)&conn);
            }
        }
        return;
    }

    /* Collision report from Stack: Notify profiles */
    if (p_data->acl_change.event == BTM_BL_COLLISION_EVT) {
        bta_sys_notify_collision (p_bda);
        return;
    }

    if (is_new) {
        for (i = 0; i < bta_dm_cb.device_list.count; i++) {
            if (!bdcmp( bta_dm_cb.device_list.peer_device[i].peer_bdaddr, p_bda)
#if BLE_INCLUDED == TRUE
                    && bta_dm_cb.device_list.peer_device[i].conn_handle == p_data->acl_change.handle
#endif
               ) {
                break;
            }

        }

        if (i == bta_dm_cb.device_list.count) {
            if (bta_dm_cb.device_list.count < BTA_DM_NUM_PEER_DEVICE) {
                bdcpy(bta_dm_cb.device_list.peer_device[bta_dm_cb.device_list.count].peer_bdaddr, p_bda);
                bta_dm_cb.device_list.peer_device[bta_dm_cb.device_list.count].link_policy = bta_dm_cb.cur_policy;
                bta_dm_cb.device_list.count++;
#if BLE_INCLUDED == TRUE
                bta_dm_cb.device_list.peer_device[i].conn_handle = p_data->acl_change.handle;
                if (p_data->acl_change.transport == BT_TRANSPORT_LE) {
                    bta_dm_cb.device_list.le_count++;
                }
#endif
            } else {
                APPL_TRACE_ERROR("%s max active connection reached, no resources", __func__);
                return;
            }
        }

        bta_dm_cb.device_list.peer_device[i].conn_state = BTA_DM_CONNECTED;
        bta_dm_cb.device_list.peer_device[i].pref_role = BTA_ANY_ROLE;
        bdcpy(conn.link_up.bd_addr, p_bda);
        bta_dm_cb.device_list.peer_device[i].info = BTA_DM_DI_NONE;
#if BLE_INCLUDED == TRUE
        conn.link_up.link_type = p_data->acl_change.transport;
        bta_dm_cb.device_list.peer_device[i].transport = p_data->acl_change.transport;
#endif

        if (((NULL != (p = BTM_ReadLocalFeatures ())) && HCI_SNIFF_SUB_RATE_SUPPORTED(p)) &&
                ((NULL != (p = BTM_ReadRemoteFeatures (p_bda))) && HCI_SNIFF_SUB_RATE_SUPPORTED(p))) {
            /* both local and remote devices support SSR */
            bta_dm_cb.device_list.peer_device[i].info = BTA_DM_DI_USE_SSR;
        }
        APPL_TRACE_DEBUG("%s info: 0x%x", __func__, bta_dm_cb.device_list.peer_device[i].info);

        if (bta_dm_cb.p_sec_cback) {
            bta_dm_cb.p_sec_cback(BTA_DM_LINK_UP_EVT, (tBTA_DM_SEC *)&conn);
        }
    } else {
        for (i = 0; i < bta_dm_cb.device_list.count; i++) {
            if (bdcmp( bta_dm_cb.device_list.peer_device[i].peer_bdaddr, p_bda)
#if BLE_INCLUDED == TRUE
                    || bta_dm_cb.device_list.peer_device[i].transport != p_data->acl_change.transport
#endif
               ) {
                continue;
            }

            if ( bta_dm_cb.device_list.peer_device[i].conn_state == BTA_DM_UNPAIRING ) {
                if (BTM_SecDeleteDevice(bta_dm_cb.device_list.peer_device[i].peer_bdaddr)) {
                    issue_unpair_cb = TRUE;
                }

                APPL_TRACE_DEBUG("%s: Unpairing: issue unpair CB = %d ", __FUNCTION__, issue_unpair_cb);
            }

            conn.link_down.is_removed = bta_dm_cb.device_list.peer_device[i].remove_dev_pending;

            for (; i < bta_dm_cb.device_list.count ; i++) {
                memcpy(&bta_dm_cb.device_list.peer_device[i], &bta_dm_cb.device_list.peer_device[i + 1], sizeof(bta_dm_cb.device_list.peer_device[i]));
            }
            break;
        }
        if (bta_dm_cb.device_list.count) {
            bta_dm_cb.device_list.count--;
        }
#if BLE_INCLUDED == TRUE
        if ((p_data->acl_change.transport == BT_TRANSPORT_LE) &&
                (bta_dm_cb.device_list.le_count)) {
            bta_dm_cb.device_list.le_count--;
        }
        conn.link_down.link_type = p_data->acl_change.transport;
#endif

        if (bta_dm_search_cb.wait_disc && !bdcmp(bta_dm_search_cb.peer_bdaddr, p_bda)) {
            bta_dm_search_cb.wait_disc = FALSE;

            if (bta_dm_search_cb.sdp_results) {
                APPL_TRACE_EVENT(" timer stopped  ");
                bta_sys_stop_timer(&bta_dm_search_cb.search_timer);
                bta_dm_discover_next_device();
            }

        }

        if (bta_dm_cb.disabling) {
            if (!BTM_GetNumAclLinks()) {
                bta_sys_stop_timer(&bta_dm_cb.disable_timer);
                bta_dm_cb.disable_timer.p_cback = (TIMER_CBACK *)&bta_dm_disable_conn_down_timer_cback;
                /*
                 * Start a timer to make sure that the profiles
                 * get the disconnect event.
                 */
                bta_sys_start_timer(&bta_dm_cb.disable_timer, 0, 1000);
            }
        }
        if (conn.link_down.is_removed) {
            BTM_SecDeleteDevice(p_bda);
#if (BLE_INCLUDED == TRUE && GATTC_INCLUDED == TRUE)
            /* need to remove all pending background connection */
            BTA_GATTC_CancelOpen(0, p_bda, FALSE);
            /* remove all cached GATT information */
            BTA_GATTC_Refresh(p_bda);
#endif
        }

        bdcpy(conn.link_down.bd_addr, p_bda);
        conn.link_down.status = (UINT8) btm_get_acl_disc_reason_code();
        if ( bta_dm_cb.p_sec_cback ) {
            bta_dm_cb.p_sec_cback(BTA_DM_LINK_DOWN_EVT, &conn);
            if ( issue_unpair_cb ) {
                bta_dm_cb.p_sec_cback(BTA_DM_DEV_UNPAIRED_EVT, &conn);
            }
        }
    }

    bta_dm_adjust_roles(TRUE);
}

/*******************************************************************************
**
** Function         bta_dm_disable_conn_down_timer_cback
**
** Description      Sends disable event to application
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_disable_conn_down_timer_cback (TIMER_LIST_ENT *p_tle)
{
    UNUSED(p_tle);
    tBTA_SYS_HW_MSG *sys_enable_event;
#if (BTM_SSR_INCLUDED == TRUE)
    /* disable the power managment module */
    bta_dm_disable_pm();
#endif  ///BTM_SSR_INCLUDED == TRUE
    /* register our callback to SYS HW manager */
    bta_sys_hw_register( BTA_SYS_HW_BLUETOOTH, bta_dm_sys_hw_cback );

    /* send a message to BTA SYS */
    if ((sys_enable_event = (tBTA_SYS_HW_MSG *) osi_malloc(sizeof(tBTA_SYS_HW_MSG))) != NULL) {
        sys_enable_event->hdr.event = BTA_SYS_API_DISABLE_EVT;
        sys_enable_event->hw_module = BTA_SYS_HW_BLUETOOTH;
        bta_sys_sendmsg(sys_enable_event);
    }

    bta_dm_cb.disabling = FALSE;

}

/*******************************************************************************
**
** Function         bta_dm_rm_cback
**
** Description      Role management callback from sys
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_rm_cback(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    UINT8 j;
    tBTA_PREF_ROLES role;
    tBTA_DM_PEER_DEVICE *p_dev;

    p_dev = bta_dm_find_peer_device(peer_addr);
    if ( status == BTA_SYS_CONN_OPEN) {
        if (p_dev) {
            /* Do not set to connected if we are in the middle of unpairing. When AV stream is
             * started it fakes out a SYS_CONN_OPEN to potentially trigger a role switch command.
             * But this should not be done if we are in the middle of unpairing.
             */
            if (p_dev->conn_state != BTA_DM_UNPAIRING) {
                p_dev->conn_state = BTA_DM_CONNECTED;
            }

            for (j = 1; j <= p_bta_dm_rm_cfg[0].app_id; j++) {
                if (((p_bta_dm_rm_cfg[j].app_id == app_id) || (p_bta_dm_rm_cfg[j].app_id == BTA_ALL_APP_ID))
                        && (p_bta_dm_rm_cfg[j].id == id)) {
                    role = p_bta_dm_rm_cfg[j].cfg;

                    if (role > p_dev->pref_role ) {
                        p_dev->pref_role = role;
                    }
                    break;
                }
            }
        }
    }

    if ((BTA_ID_AV == id) || (BTA_ID_AVK == id)) {
        if ( status == BTA_SYS_CONN_BUSY) {
            if (p_dev) {
                p_dev->info |= BTA_DM_DI_AV_ACTIVE;
            }
            /* AV calls bta_sys_conn_open with the A2DP stream count as app_id */
            if (BTA_ID_AV == id) {
#if (BTM_SSR_INCLUDED == TRUE)
                bta_dm_cb.cur_av_count = bta_dm_get_av_count();
#endif  ///BTM_SSR_INCLUDED == TRUE
            }
        } else if ( status == BTA_SYS_CONN_IDLE) {
            if (p_dev) {
                p_dev->info &= ~BTA_DM_DI_AV_ACTIVE;
            }

            /* get cur_av_count from connected services */
            if (BTA_ID_AV == id) {
#if (BTM_SSR_INCLUDED == TRUE)
                bta_dm_cb.cur_av_count = bta_dm_get_av_count();
#endif  ///BTM_SSR_INCLUDED == TRUE
            }
        }
        APPL_TRACE_WARNING("bta_dm_rm_cback:%d, status:%d", bta_dm_cb.cur_av_count, status);
    }

    /* Don't adjust roles for each busy/idle state transition to avoid
       excessive switch requests when individual profile busy/idle status
       changes */
    if ((status != BTA_SYS_CONN_BUSY) && (status != BTA_SYS_CONN_IDLE)) {
        bta_dm_adjust_roles(FALSE);
    }
}

/*******************************************************************************
**
** Function         bta_dm_delay_role_switch_cback
**
** Description      Callback from btm to delay a role switch
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_delay_role_switch_cback(TIMER_LIST_ENT *p_tle)
{
    UNUSED(p_tle);
    APPL_TRACE_EVENT("bta_dm_delay_role_switch_cback: initiating Delayed RS");
    bta_dm_adjust_roles (FALSE);
}

/*******************************************************************************
**
** Function         bta_dm_remove_sec_dev_entry
**
** Description      Removes device entry from Security device DB if ACL connection with
**                  remtoe device does not exist, else schedule for dev entry removal upon
                     ACL close
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
static void bta_dm_remove_sec_dev_entry(BD_ADDR remote_bd_addr)
{
    UINT16 index = 0;
    if ( BTM_IsAclConnectionUp(remote_bd_addr, BT_TRANSPORT_LE) ||
            BTM_IsAclConnectionUp(remote_bd_addr, BT_TRANSPORT_BR_EDR)) {
        APPL_TRACE_DEBUG("%s ACL is not down. Schedule for  Dev Removal when ACL closes",
                         __FUNCTION__);
        BTM_SecClearSecurityFlags (remote_bd_addr);
        for (index = 0; index < bta_dm_cb.device_list.count; index ++) {
            if (!bdcmp( bta_dm_cb.device_list.peer_device[index].peer_bdaddr, remote_bd_addr)) {
                break;
            }
        }
        if (index != bta_dm_cb.device_list.count) {
            bta_dm_cb.device_list.peer_device[index].remove_dev_pending = TRUE;
        } else {
            APPL_TRACE_ERROR(" %s Device does not exist in DB", __FUNCTION__);
        }
    } else {
        BTM_SecDeleteDevice (remote_bd_addr);
#if (BLE_INCLUDED == TRUE && GATTC_INCLUDED == TRUE)
        /* need to remove all pending background connection */
        BTA_GATTC_CancelOpen(0, remote_bd_addr, FALSE);
        /* remove all cached GATT information */
        BTA_GATTC_Refresh(remote_bd_addr);
#endif
    }
}
#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         bta_dm_adjust_roles
**
** Description      Adjust roles
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_adjust_roles(BOOLEAN delay_role_switch)
{

    UINT8 i;
    BOOLEAN set_master_role = FALSE;
#if BLE_INCLUDED == TRUE
    UINT8 br_count = bta_dm_cb.device_list.count - bta_dm_cb.device_list.le_count;
#else
    UINT8 br_count = bta_dm_cb.device_list.count;
#endif
    if (br_count) {

        /* the configuration is no scatternet
         * or AV connection exists and there are more than one ACL link */
        if ( (p_bta_dm_rm_cfg[0].cfg == BTA_DM_NO_SCATTERNET) ||
                (bta_dm_cb.cur_av_count && br_count > 1) ) {

            L2CA_SetDesireRole (HCI_ROLE_MASTER);
            set_master_role = TRUE;

        }

        for (i = 0; i < bta_dm_cb.device_list.count; i++) {
            if (bta_dm_cb.device_list.peer_device[i].conn_state == BTA_DM_CONNECTED
#if BLE_INCLUDED == TRUE
                    && bta_dm_cb.device_list.peer_device[i].transport == BT_TRANSPORT_BR_EDR
#endif
               ) {
                if (!set_master_role && (bta_dm_cb.device_list.peer_device[i].pref_role != BTA_ANY_ROLE)
                        && (p_bta_dm_rm_cfg[0].cfg == BTA_DM_PARTIAL_SCATTERNET)) {
                    L2CA_SetDesireRole (HCI_ROLE_MASTER);
                    set_master_role = TRUE;
                }

                if ((bta_dm_cb.device_list.peer_device[i].pref_role == BTA_MASTER_ROLE_ONLY)
                        || (br_count > 1)) {

                    /* Initiating immediate role switch with certain remote devices
                      has caused issues due to role  switch colliding with link encryption setup and
                      causing encryption (and in turn the link) to fail .  These device . Firmware
                      versions are stored in a blacklist and role switch with these devices are
                      delayed to avoid the collision with link encryption setup */

                    if (bta_dm_cb.device_list.peer_device[i].pref_role != BTA_SLAVE_ROLE_ONLY &&
                            delay_role_switch == FALSE) {
                        BTM_SwitchRole (bta_dm_cb.device_list.peer_device[i].peer_bdaddr,
                                        HCI_ROLE_MASTER, NULL);
                    } else {
                        bta_dm_cb.switch_delay_timer.p_cback =
                            (TIMER_CBACK *)&bta_dm_delay_role_switch_cback;
                        bta_sys_start_timer(&bta_dm_cb.switch_delay_timer, 0, 500);
                    }
                }

            }
        }


        if (!set_master_role) {

            L2CA_SetDesireRole (L2CAP_DESIRED_LINK_ROLE);

        }

    } else {
        L2CA_SetDesireRole (L2CAP_DESIRED_LINK_ROLE);
    }


}

/*******************************************************************************
**
** Function         bta_dm_get_remname
**
** Description      Returns a pointer to the remote name stored in the DM control
**                  block if it exists, or from the BTM memory.
**
** Returns          char * - Pointer to the remote device name
*******************************************************************************/
#if (SDP_INCLUDED == TRUE || SMP_INCLUDED == TRUE)
static char *bta_dm_get_remname(void)
{
    char *p_name = (char *)bta_dm_search_cb.peer_name;
    char *p_temp;

    /* If the name isn't already stored, try retrieving from BTM */
    if (*p_name == '\0')
        if ((p_temp = BTM_SecReadDevName(bta_dm_search_cb.peer_bdaddr)) != NULL) {
            p_name = p_temp;
        }

    return p_name;
}
#endif  ///SDP_INCLUDED == TRUE || SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_bond_cancel_complete_cback
**
** Description      Authentication complete callback from BTM
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
static void bta_dm_bond_cancel_complete_cback(tBTM_STATUS result)
{

    tBTA_DM_SEC sec_event;

    if (result == BTM_SUCCESS) {
        sec_event.bond_cancel_cmpl.result = BTA_SUCCESS;
    } else {
        sec_event.bond_cancel_cmpl.result = BTA_FAILURE;
    }

    if (bta_dm_cb.p_sec_cback) {
        bta_dm_cb.p_sec_cback(BTA_DM_BOND_CANCEL_CMPL_EVT, &sec_event);
    }
}
#endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_set_eir
**
** Description      This function creates EIR tagged data and writes it to controller.
**
** Returns          None
**
*******************************************************************************/
static void bta_dm_set_eir (char *local_name)
{
    BT_HDR   *p_buf;
    UINT8    *p;
    UINT8    *p_length;
#if (BTA_EIR_CANNED_UUID_LIST != TRUE)
    UINT8    *p_type;
    UINT8    max_num_uuid;
#if (BTA_EIR_SERVER_NUM_CUSTOM_UUID > 0)
    UINT8    custom_uuid_idx;
#endif  // BTA_EIR_SERVER_NUM_CUSTOM_UUID
#endif  // BTA_EIR_CANNED_UUID_LIST
#if (BTM_EIR_DEFAULT_FEC_REQUIRED == FALSE)
    UINT8    free_eir_length = HCI_EXT_INQ_RESPONSE_LEN;
#else  // BTM_EIR_DEFAULT_FEC_REQUIRED
    UINT8    free_eir_length = HCI_DM5_PACKET_SIZE;
#endif  // BTM_EIR_DEFAULT_FEC_REQUIRED
    UINT8    num_uuid;
    UINT8    data_type;
    UINT8    local_name_len;

    /* wait until complete to disable */
    if (bta_dm_cb.disable_timer.in_use) {
        return;
    }

#if ( BTA_EIR_CANNED_UUID_LIST != TRUE )
    /* wait until App is ready */
    if (bta_dm_cb.app_ready_timer.in_use) {
        return;
    }

    /* if local name is not provided, get it from controller */
    if ( local_name == NULL ) {
        if ( BTM_ReadLocalDeviceName( &local_name ) != BTM_SUCCESS ) {
            APPL_TRACE_ERROR("Fail to read local device name for EIR");
        }
    }
#endif  // BTA_EIR_CANNED_UUID_LIST

    /* Allocate a buffer to hold HCI command */
    if ((p_buf = (BT_HDR *)osi_malloc(BTM_CMD_BUF_SIZE)) == NULL) {
        APPL_TRACE_ERROR("bta_dm_set_eir couldn't allocate buffer");
        return;
    }
    p = (UINT8 *)p_buf + BTM_HCI_EIR_OFFSET;

    memset(p, 0x00, HCI_EXT_INQ_RESPONSE_LEN );

    APPL_TRACE_DEBUG("BTA is generating EIR");

    if ( local_name ) {
        local_name_len = strlen( local_name );
    } else {
        local_name_len = 0;
    }

    data_type = BTM_EIR_COMPLETE_LOCAL_NAME_TYPE;
    /* if local name is longer than minimum length of shortened name */
    /* check whether it needs to be shortened or not */
    if ( local_name_len > p_bta_dm_eir_cfg->bta_dm_eir_min_name_len ) {
        /* get number of UUID 16-bit list */
#if (BTA_EIR_CANNED_UUID_LIST == TRUE)
        num_uuid = p_bta_dm_eir_cfg->bta_dm_eir_uuid16_len / LEN_UUID_16;
#else  // BTA_EIR_CANNED_UUID_LIST
        max_num_uuid = (free_eir_length - 2) / LEN_UUID_16;
        data_type = BTM_GetEirSupportedServices( bta_dm_cb.eir_uuid, &p,
                    max_num_uuid, &num_uuid );
        p = (UINT8 *)p_buf + BTM_HCI_EIR_OFFSET; /* reset p */
#endif  // BTA_EIR_CANNED_UUID_LIST

        /* if UUID doesn't fit remaing space, shorten local name */
        if ( local_name_len > (free_eir_length - 4 - num_uuid * LEN_UUID_16)) {
            APPL_TRACE_WARNING("BTA EIR: local name is shortened");
            local_name_len = p_bta_dm_eir_cfg->bta_dm_eir_min_name_len;
            data_type = BTM_EIR_SHORTENED_LOCAL_NAME_TYPE;
        } else {
            data_type = BTM_EIR_COMPLETE_LOCAL_NAME_TYPE;
        }
    }

    UINT8_TO_STREAM(p, local_name_len + 1);
    UINT8_TO_STREAM(p, data_type);

    if (local_name != NULL) {
        memcpy(p, local_name, local_name_len);
        p += local_name_len;
    }
    free_eir_length -= local_name_len + 2;

#if (BTA_EIR_CANNED_UUID_LIST == TRUE)
    /* if UUID list is provided as static data in configuration */
    if (( p_bta_dm_eir_cfg->bta_dm_eir_uuid16_len > 0 )
            && (p_bta_dm_eir_cfg->bta_dm_eir_uuid16)) {
        if ( free_eir_length > LEN_UUID_16 + 2) {
            free_eir_length -= 2;

            if ( free_eir_length >= p_bta_dm_eir_cfg->bta_dm_eir_uuid16_len) {
                num_uuid = p_bta_dm_eir_cfg->bta_dm_eir_uuid16_len / LEN_UUID_16;
                data_type = BTM_EIR_COMPLETE_16BITS_UUID_TYPE;
            } else { /* not enough room for all UUIDs */
                APPL_TRACE_WARNING("BTA EIR: UUID 16-bit list is truncated");
                num_uuid = free_eir_length / LEN_UUID_16;
                data_type = BTM_EIR_MORE_16BITS_UUID_TYPE;
            }
            UINT8_TO_STREAM(p, num_uuid * LEN_UUID_16 + 1);
            UINT8_TO_STREAM(p, data_type);
            memcpy(p, p_bta_dm_eir_cfg->bta_dm_eir_uuid16, num_uuid * LEN_UUID_16 );
            p += num_uuid * LEN_UUID_16;
            free_eir_length -= num_uuid * LEN_UUID_16;
        }
    }
#else /* (BTA_EIR_CANNED_UUID_LIST == TRUE) */
    /* if UUID list is dynamic */
    if ( free_eir_length >= 2) {
        p_length = p++;
        p_type   = p++;
        num_uuid = 0;

        max_num_uuid = (free_eir_length - 2) / LEN_UUID_16;
        data_type = BTM_GetEirSupportedServices( bta_dm_cb.eir_uuid, &p, max_num_uuid, &num_uuid );

        if ( data_type == BTM_EIR_MORE_16BITS_UUID_TYPE ) {
            APPL_TRACE_WARNING("BTA EIR: UUID 16-bit list is truncated");
        }
#if (BTA_EIR_SERVER_NUM_CUSTOM_UUID > 0)
        else {
            for (custom_uuid_idx = 0; custom_uuid_idx < BTA_EIR_SERVER_NUM_CUSTOM_UUID; custom_uuid_idx++) {
                if (bta_dm_cb.custom_uuid[custom_uuid_idx].len == LEN_UUID_16) {
                    if ( num_uuid < max_num_uuid ) {
                        UINT16_TO_STREAM(p, bta_dm_cb.custom_uuid[custom_uuid_idx].uu.uuid16);
                        num_uuid++;
                    } else {
                        data_type = BTM_EIR_MORE_16BITS_UUID_TYPE;
                        APPL_TRACE_WARNING("BTA EIR: UUID 16-bit list is truncated");
                        break;
                    }
                }
            }
        }
#endif /* (BTA_EIR_SERVER_NUM_CUSTOM_UUID > 0) */

        UINT8_TO_STREAM(p_length, num_uuid * LEN_UUID_16 + 1);
        UINT8_TO_STREAM(p_type, data_type);
        free_eir_length -= num_uuid * LEN_UUID_16 + 2;
    }
#endif /* (BTA_EIR_CANNED_UUID_LIST == TRUE) */

#if ( BTA_EIR_CANNED_UUID_LIST != TRUE )&&(BTA_EIR_SERVER_NUM_CUSTOM_UUID > 0)
    /* Adding 32-bit UUID list */
    if ( free_eir_length >= 2) {
        p_length = p++;
        p_type   = p++;
        num_uuid = 0;
        data_type = BTM_EIR_COMPLETE_32BITS_UUID_TYPE;

        max_num_uuid = (free_eir_length - 2) / LEN_UUID_32;

        for (custom_uuid_idx = 0; custom_uuid_idx < BTA_EIR_SERVER_NUM_CUSTOM_UUID; custom_uuid_idx++) {
            if (bta_dm_cb.custom_uuid[custom_uuid_idx].len == LEN_UUID_32) {
                if ( num_uuid < max_num_uuid ) {
                    UINT32_TO_STREAM(p, bta_dm_cb.custom_uuid[custom_uuid_idx].uu.uuid32);
                    num_uuid++;
                } else {
                    data_type = BTM_EIR_MORE_32BITS_UUID_TYPE;
                    APPL_TRACE_WARNING("BTA EIR: UUID 32-bit list is truncated");
                    break;
                }
            }
        }

        UINT8_TO_STREAM(p_length, num_uuid * LEN_UUID_32 + 1);
        UINT8_TO_STREAM(p_type, data_type);
        free_eir_length -= num_uuid * LEN_UUID_32 + 2;
    }

    /* Adding 128-bit UUID list */
    if ( free_eir_length >= 2) {
        p_length = p++;
        p_type   = p++;
        num_uuid = 0;
        data_type = BTM_EIR_COMPLETE_128BITS_UUID_TYPE;

        max_num_uuid = (free_eir_length - 2) / LEN_UUID_128;

        for (custom_uuid_idx = 0; custom_uuid_idx < BTA_EIR_SERVER_NUM_CUSTOM_UUID; custom_uuid_idx++) {
            if (bta_dm_cb.custom_uuid[custom_uuid_idx].len == LEN_UUID_128) {
                if ( num_uuid < max_num_uuid ) {
                    ARRAY16_TO_STREAM(p, bta_dm_cb.custom_uuid[custom_uuid_idx].uu.uuid128);
                    num_uuid++;
                } else {
                    data_type = BTM_EIR_MORE_128BITS_UUID_TYPE;
                    APPL_TRACE_WARNING("BTA EIR: UUID 128-bit list is truncated");
                    break;
                }
            }
        }

        UINT8_TO_STREAM(p_length, num_uuid * LEN_UUID_128 + 1);
        UINT8_TO_STREAM(p_type, data_type);
        free_eir_length -= num_uuid * LEN_UUID_128 + 2;
    }
#endif /* ( BTA_EIR_CANNED_UUID_LIST != TRUE )&&(BTA_EIR_SERVER_NUM_CUSTOM_UUID > 0) */

    /* if Flags are provided in configuration */
    if (( p_bta_dm_eir_cfg->bta_dm_eir_flag_len > 0 )
            && ( p_bta_dm_eir_cfg->bta_dm_eir_flags )
            && ( free_eir_length >= p_bta_dm_eir_cfg->bta_dm_eir_flag_len + 2 )) {
        UINT8_TO_STREAM(p, p_bta_dm_eir_cfg->bta_dm_eir_flag_len + 1);
        UINT8_TO_STREAM(p, BTM_EIR_FLAGS_TYPE);
        memcpy(p, p_bta_dm_eir_cfg->bta_dm_eir_flags,
               p_bta_dm_eir_cfg->bta_dm_eir_flag_len);
        p += p_bta_dm_eir_cfg->bta_dm_eir_flag_len;
        free_eir_length -= p_bta_dm_eir_cfg->bta_dm_eir_flag_len + 2;
    }

    /* if Manufacturer Specific are provided in configuration */
    if (( p_bta_dm_eir_cfg->bta_dm_eir_manufac_spec_len > 0 )
            && ( p_bta_dm_eir_cfg->bta_dm_eir_manufac_spec )
            && ( free_eir_length >= p_bta_dm_eir_cfg->bta_dm_eir_manufac_spec_len + 2 )) {
        p_length = p;

        UINT8_TO_STREAM(p, p_bta_dm_eir_cfg->bta_dm_eir_manufac_spec_len + 1);
        UINT8_TO_STREAM(p, BTM_EIR_MANUFACTURER_SPECIFIC_TYPE);
        memcpy(p, p_bta_dm_eir_cfg->bta_dm_eir_manufac_spec,
               p_bta_dm_eir_cfg->bta_dm_eir_manufac_spec_len);
        p += p_bta_dm_eir_cfg->bta_dm_eir_manufac_spec_len;
        free_eir_length -= p_bta_dm_eir_cfg->bta_dm_eir_manufac_spec_len + 2;

    } else {
        p_length = NULL;
    }

    /* if Inquiry Tx Resp Power compiled */
    if ((p_bta_dm_eir_cfg->bta_dm_eir_inq_tx_power) &&
            (free_eir_length >= 3)) {
        UINT8_TO_STREAM(p, 2);      /* Length field */
        UINT8_TO_STREAM(p, BTM_EIR_TX_POWER_LEVEL_TYPE);
        UINT8_TO_STREAM(p, *(p_bta_dm_eir_cfg->bta_dm_eir_inq_tx_power));
        free_eir_length -= 3;
    }

    if ( free_eir_length ) {
        UINT8_TO_STREAM(p, 0);    /* terminator of significant part */
    }

    BTM_WriteEIR( p_buf );

}

/*******************************************************************************
**
** Function         bta_dm_eir_search_services
**
** Description      This function searches services in received EIR
**
** Returns          None
**
*******************************************************************************/
#if (SDP_INCLUDED == TRUE)
static void bta_dm_eir_search_services( tBTM_INQ_RESULTS  *p_result,
                                        tBTA_SERVICE_MASK *p_services_to_search,
                                        tBTA_SERVICE_MASK *p_services_found)
{
    tBTA_SERVICE_MASK       service_index = 0;
    tBTM_EIR_SEARCH_RESULT  result;

    APPL_TRACE_DEBUG("BTA searching services in EIR of BDA:0x%02X%02X%02X%02X%02X%02X",
                     p_result->remote_bd_addr[0], p_result->remote_bd_addr[1],
                     p_result->remote_bd_addr[2], p_result->remote_bd_addr[3],
                     p_result->remote_bd_addr[4], p_result->remote_bd_addr[5]);

    APPL_TRACE_DEBUG("    with services_to_search=0x%08X", *p_services_to_search);

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
    /* always do GATT based service discovery by SDP instead of from EIR    */
    /* if GATT based service is also to be put in EIR, need to modify this  */
    while (service_index < (BTA_MAX_SERVICE_ID - 1))
#else
    while (service_index < BTA_MAX_SERVICE_ID)
#endif
    {
        if ( *p_services_to_search
                & (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(service_index))) {
            result = BTM_HasInquiryEirService( p_result,
                                               bta_service_id_to_uuid_lkup_tbl[service_index] );

            /* Searching for HSP v1.2 only device */
            if ((result != BTM_EIR_FOUND) &&
                    (bta_service_id_to_uuid_lkup_tbl[service_index] == UUID_SERVCLASS_HEADSET)) {
                result = BTM_HasInquiryEirService (p_result, UUID_SERVCLASS_HEADSET_HS);
            }

            if ( result == BTM_EIR_FOUND ) {
                /* If Plug and Play service record, need to check to see if Broadcom stack */
                /* However, EIR data doesn't have EXT_BRCM_VERSION so just skip it */
                if ( bta_service_id_to_uuid_lkup_tbl[service_index]
                        != UUID_SERVCLASS_PNP_INFORMATION ) {

                    *p_services_found |=
                        (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(service_index));
                    /* remove the service from services to be searched  */
                    *p_services_to_search &=
                        (tBTA_SERVICE_MASK)(~(BTA_SERVICE_ID_TO_SERVICE_MASK(service_index)));
                }
            } else if ( result == BTM_EIR_NOT_FOUND ) {
                /* remove the service from services to be searched  */
                *p_services_to_search &=
                    (tBTA_SERVICE_MASK)(~(BTA_SERVICE_ID_TO_SERVICE_MASK(service_index)));
            }
        }

        service_index++;
    }

    APPL_TRACE_ERROR("BTA EIR search result, services_to_search=0x%08X, services_found=0x%08X",
                     *p_services_to_search, *p_services_found);
}
#endif  ///SDP_INCLUDED == TRUE

#if (BTA_EIR_CANNED_UUID_LIST != TRUE)
/*******************************************************************************
**
** Function         bta_dm_eir_update_uuid
**
** Description      This function adds or removes service UUID in EIR database.
**
** Returns          None
**
*******************************************************************************/
void bta_dm_eir_update_uuid(UINT16 uuid16, BOOLEAN adding)
{
    /* if this UUID is not advertised in EIR */
    if ( !BTM_HasEirService( p_bta_dm_eir_cfg->uuid_mask, uuid16 )) {
        return;
    }

    if ( adding ) {
        APPL_TRACE_EVENT("Adding UUID=0x%04X into EIR", uuid16);

        BTM_AddEirService( bta_dm_cb.eir_uuid, uuid16 );
    } else {
        APPL_TRACE_EVENT("Removing UUID=0x%04X from EIR", uuid16);

        BTM_RemoveEirService( bta_dm_cb.eir_uuid, uuid16 );
    }

    bta_dm_set_eir (NULL);

    APPL_TRACE_EVENT("bta_dm_eir_update_uuid UUID bit mask=0x%08X %08X",
                     bta_dm_cb.eir_uuid[1], bta_dm_cb.eir_uuid[0] );
}
#endif

/*******************************************************************************
**
** Function         bta_dm_enable_test_mode
**
** Description      enable test mode
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_enable_test_mode(tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    BTM_EnableTestMode();
}

/*******************************************************************************
**
** Function         bta_dm_disable_test_mode
**
** Description      disable test mode
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_disable_test_mode(tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    BTM_DeviceReset(NULL);
}

/*******************************************************************************
**
** Function         bta_dm_execute_callback
**
** Description      Just execute a generic call back in the context of the BTU/BTA tack
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_execute_callback(tBTA_DM_MSG *p_data)
{
    /* sanity check */
    if (p_data->exec_cback.p_exec_cback == NULL) {
        return;
    }

    p_data->exec_cback.p_exec_cback(p_data->exec_cback.p_param);
}

/*******************************************************************************
**
** Function         bta_dm_encrypt_cback
**
** Description      link encryption complete callback.
**
** Returns         None
**
*******************************************************************************/
void bta_dm_encrypt_cback(BD_ADDR bd_addr, tBT_TRANSPORT transport, void *p_ref_data, tBTM_STATUS result)
{
    tBTA_STATUS   bta_status = BTA_SUCCESS;
    tBTA_DM_ENCRYPT_CBACK *p_callback = NULL;
    UINT8   i ;
    UNUSED(p_ref_data);

    for (i = 0; i < bta_dm_cb.device_list.count; i++) {
        if (bdcmp( bta_dm_cb.device_list.peer_device[i].peer_bdaddr, bd_addr) == 0 &&
                bta_dm_cb.device_list.peer_device[i].conn_state == BTA_DM_CONNECTED) {
            break;
        }
    }

    if (i < bta_dm_cb.device_list.count) {
        p_callback = bta_dm_cb.device_list.peer_device[i].p_encrypt_cback;
        bta_dm_cb.device_list.peer_device[i].p_encrypt_cback = NULL;
    }

    switch (result) {
    case BTM_SUCCESS:
        break;
    case BTM_WRONG_MODE:
        bta_status = BTA_WRONG_MODE;
        break;
    case BTM_NO_RESOURCES:
        bta_status = BTA_NO_RESOURCES;
        break;
    case BTM_BUSY:
        bta_status = BTA_BUSY;
        break;
    default:
        bta_status = BTA_FAILURE;
        break;
    }

    APPL_TRACE_DEBUG("bta_dm_encrypt_cback status =%d p_callback=%p", bta_status, p_callback);

    if (p_callback) {
        (*p_callback)(bd_addr, transport, bta_status);
    }
}

/*******************************************************************************
**
** Function         bta_dm_set_encryption
**
** Description      This function to encrypt the link
**
** Returns          None
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void bta_dm_set_encryption (tBTA_DM_MSG *p_data)
{
    UINT8 i ;

    APPL_TRACE_DEBUG("bta_dm_set_encryption\n"); //todo
    if (!p_data->set_encryption.p_callback) {
        APPL_TRACE_ERROR("bta_dm_set_encryption callback is not provided\n");
        return;
    }
    for (i = 0; i < bta_dm_cb.device_list.count; i++) {
        if (bdcmp( bta_dm_cb.device_list.peer_device[i].peer_bdaddr, p_data->set_encryption.bd_addr) == 0 &&
                bta_dm_cb.device_list.peer_device[i].conn_state == BTA_DM_CONNECTED) {
            break;
        }
    }
    if (i < bta_dm_cb.device_list.count) {
        if (bta_dm_cb.device_list.peer_device[i].p_encrypt_cback) {
            APPL_TRACE_ERROR("earlier enc was not done for same device\n");
            (*p_data->set_encryption.p_callback)(p_data->set_encryption.bd_addr,
                                                 p_data->set_encryption.transport,
                                                 BTA_BUSY);
            return;
        }

        if (BTM_SetEncryption(p_data->set_encryption.bd_addr, p_data->set_encryption.transport,
                              bta_dm_encrypt_cback, &p_data->set_encryption.sec_act)
                == BTM_CMD_STARTED) {
            bta_dm_cb.device_list.peer_device[i].p_encrypt_cback = p_data->set_encryption.p_callback;
        }
    }
}
#endif  ///SMP_INCLUDED == TRUE

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_dm_observe_results_cb
**
** Description      Callback for BLE Observe result
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_observe_results_cb (tBTM_INQ_RESULTS *p_inq, UINT8 *p_eir)
{
    tBTA_DM_SEARCH     result;
    tBTM_INQ_INFO      *p_inq_info;
    APPL_TRACE_DEBUG("bta_dm_observe_results_cb")

    bdcpy(result.inq_res.bd_addr, p_inq->remote_bd_addr);
    result.inq_res.rssi = p_inq->rssi;
    result.inq_res.ble_addr_type    = p_inq->ble_addr_type;
    result.inq_res.inq_result_type  = p_inq->inq_result_type;
    result.inq_res.device_type      = p_inq->device_type;
    result.inq_res.flag             = p_inq->flag;
    result.inq_res.adv_data_len     = p_inq->adv_data_len;
    result.inq_res.scan_rsp_len     = p_inq->scan_rsp_len;

    /* application will parse EIR to find out remote device name */
    result.inq_res.p_eir = p_eir;

    if ((p_inq_info = BTM_InqDbRead(p_inq->remote_bd_addr)) != NULL) {
        /* initialize remt_name_not_required to FALSE so that we get the name by default */
        result.inq_res.remt_name_not_required = FALSE;
    }

    if (bta_dm_search_cb.p_scan_cback) {
        bta_dm_search_cb.p_scan_cback(BTA_DM_INQ_RES_EVT, &result);
    }

    if (p_inq_info) {
        /* application indicates if it knows the remote name, inside the callback
         copy that to the inquiry data base*/
        if (result.inq_res.remt_name_not_required) {
            p_inq_info->appl_knows_rem_name = TRUE;
        }
    }
}

/*******************************************************************************
**
** Function         bta_dm_observe_cmpl_cb
**
** Description      Callback for BLE Observe complete
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_observe_cmpl_cb (void *p_result)
{
    tBTA_DM_SEARCH  data;

    APPL_TRACE_DEBUG("bta_dm_observe_cmpl_cb");

    data.inq_cmpl.num_resps = ((tBTM_INQUIRY_CMPL *)p_result)->num_resp;
    if (bta_dm_search_cb.p_scan_cback) {
        bta_dm_search_cb.p_scan_cback(BTA_DM_INQ_CMPL_EVT, &data);
    }
}

#if (SMP_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_dm_ble_smp_cback
**
** Description      Callback for BLE SMP
**
**
** Returns          void
**
*******************************************************************************/
static UINT8 bta_dm_ble_smp_cback (tBTM_LE_EVT event, BD_ADDR bda, tBTM_LE_EVT_DATA *p_data)
{
    tBTM_STATUS status = BTM_SUCCESS;
    tBTA_DM_SEC sec_event;
    char *p_name = NULL;

    if (!bta_dm_cb.p_sec_cback) {
        return BTM_NOT_AUTHORIZED;
    }

    memset(&sec_event, 0, sizeof(tBTA_DM_SEC));
    switch (event) {
    case BTM_LE_IO_REQ_EVT:
        // #if (BTM_LOCAL_IO_CAPS != BTM_IO_CAP_NONE)

        bta_dm_co_ble_io_req(bda,
                             &p_data->io_req.io_cap,
                             &p_data->io_req.oob_data,
                             &p_data->io_req.auth_req,
                             &p_data->io_req.max_key_size,
                             &p_data->io_req.init_keys,
                             &p_data->io_req.resp_keys);
        // #endif
#if BTM_OOB_INCLUDED == FALSE
        status = BTM_SUCCESS;
#endif
        APPL_TRACE_EVENT("io mitm: %d oob_data:%d\n", p_data->io_req.auth_req, p_data->io_req.oob_data);

        break;

    case BTM_LE_SEC_REQUEST_EVT:
        bdcpy(sec_event.ble_req.bd_addr, bda);
        p_name = BTM_SecReadDevName(bda);
        if (p_name != NULL) {
            BCM_STRNCPY_S((char *)sec_event.ble_req.bd_name,
                          sizeof(BD_NAME), p_name, (BD_NAME_LEN));
        } else {
            sec_event.ble_req.bd_name[0] = 0;
        }
        sec_event.ble_req.bd_name[BD_NAME_LEN] = 0;
        bta_dm_cb.p_sec_cback(BTA_DM_BLE_SEC_REQ_EVT, &sec_event);
        break;

    case BTM_LE_KEY_NOTIF_EVT:
        bdcpy(sec_event.key_notif.bd_addr, bda);
        p_name = BTM_SecReadDevName(bda);
        if (p_name != NULL) {
            BCM_STRNCPY_S((char *)sec_event.key_notif.bd_name,
                          sizeof(BD_NAME), p_name, (BD_NAME_LEN));
        } else {
            sec_event.key_notif.bd_name[0] = 0;
        }
        sec_event.ble_req.bd_name[BD_NAME_LEN] = 0;
        sec_event.key_notif.passkey = p_data->key_notif;
        bta_dm_cb.p_sec_cback(BTA_DM_BLE_PASSKEY_NOTIF_EVT, &sec_event);
        break;

    case BTM_LE_KEY_REQ_EVT:
        bdcpy(sec_event.ble_req.bd_addr, bda);
        bta_dm_cb.p_sec_cback(BTA_DM_BLE_PASSKEY_REQ_EVT, &sec_event);
        break;

    case BTM_LE_OOB_REQ_EVT:
        bdcpy(sec_event.ble_req.bd_addr, bda);
        bta_dm_cb.p_sec_cback(BTA_DM_BLE_OOB_REQ_EVT, &sec_event);
        break;

    case BTM_LE_NC_REQ_EVT:
        bdcpy(sec_event.key_notif.bd_addr, bda);
        BCM_STRNCPY_S((char *)sec_event.key_notif.bd_name, sizeof(BD_NAME), bta_dm_get_remname(), (BD_NAME_LEN));
        sec_event.ble_req.bd_name[BD_NAME_LEN] = 0;
        sec_event.key_notif.passkey = p_data->key_notif;
        bta_dm_cb.p_sec_cback(BTA_DM_BLE_NC_REQ_EVT, &sec_event);
        break;

    case BTM_LE_KEY_EVT:
        bdcpy(sec_event.ble_key.bd_addr, bda);
        sec_event.ble_key.key_type = p_data->key.key_type;
        sec_event.ble_key.p_key_value = p_data->key.p_key_value;
        bta_dm_cb.p_sec_cback(BTA_DM_BLE_KEY_EVT, &sec_event);
        break;

    case BTM_LE_COMPLT_EVT:
        bdcpy(sec_event.auth_cmpl.bd_addr, bda);
#if BLE_INCLUDED == TRUE
        BTM_ReadDevInfo(bda, &sec_event.auth_cmpl.dev_type, &sec_event.auth_cmpl.addr_type);
#endif
        p_name = BTM_SecReadDevName(bda);
        if (p_name != NULL) {
            BCM_STRNCPY_S((char *)sec_event.auth_cmpl.bd_name,
                          sizeof(BD_NAME), p_name, (BD_NAME_LEN));
        } else {
            sec_event.auth_cmpl.bd_name[0] = 0;
        }
        if (p_data->complt.reason != 0) {
            sec_event.auth_cmpl.fail_reason = BTA_DM_AUTH_CONVERT_SMP_CODE(((UINT8)p_data->complt.reason));
            /* delete this device entry from Sec Dev DB */
            bta_dm_remove_sec_dev_entry (bda);
        } else {
            sec_event.auth_cmpl.success = TRUE;
            if (!p_data->complt.smp_over_br) {
                
            }
        }

        if (bta_dm_cb.p_sec_cback) {
            //bta_dm_cb.p_sec_cback(BTA_DM_AUTH_CMPL_EVT, &sec_event);
            bta_dm_cb.p_sec_cback(BTA_DM_BLE_AUTH_CMPL_EVT, &sec_event);
        }

        break;

    default:
        status = BTM_NOT_AUTHORIZED;
        break;
    }
    return status;
}

/*******************************************************************************
**
** Function         bta_dm_ble_id_key_cback
**
** Description      Callback for BLE local ID keys
**
**
** Returns          void
**
*******************************************************************************/
static void bta_dm_ble_id_key_cback (UINT8 key_type, tBTM_BLE_LOCAL_KEYS *p_key)
{
    UINT8   evt;
    tBTA_DM_SEC dm_key;

    switch (key_type) {
    case BTM_BLE_KEY_TYPE_ID:
    case BTM_BLE_KEY_TYPE_ER:
        if (bta_dm_cb.p_sec_cback) {
            memcpy(&dm_key.ble_id_keys, p_key, sizeof(tBTM_BLE_LOCAL_KEYS));

            evt = (key_type == BTM_BLE_KEY_TYPE_ID) ? BTA_DM_BLE_LOCAL_IR_EVT : \
                  BTA_DM_BLE_LOCAL_ER_EVT;
            bta_dm_cb.p_sec_cback(evt, &dm_key);
        }
        break;

    default:
        APPL_TRACE_DEBUG("Unknown key type %d", key_type);
        break;
    }
    return;

}

/*******************************************************************************
**
** Function         bta_dm_add_blekey
**
** Description      This function adds an BLE Key to an security database entry.
**                  This function shall only be called AFTER BTA_DmAddBleDevice has been called.
**                  It is normally called during host startup to restore all required information
**                  stored in the NVRAM.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_add_blekey (tBTA_DM_MSG *p_data)
{
    if (!BTM_SecAddBleKey (p_data->add_ble_key.bd_addr,
                           (tBTM_LE_KEY_VALUE *)&p_data->add_ble_key.blekey,
                           p_data->add_ble_key.key_type)) {
        APPL_TRACE_ERROR ("BTA_DM: Error adding BLE Key for device %08x%04x",
                          (p_data->add_ble_key.bd_addr[0] << 24) + (p_data->add_ble_key.bd_addr[1] << 16) + \
                          (p_data->add_ble_key.bd_addr[2] << 8) + p_data->add_ble_key.bd_addr[3],
                          (p_data->add_ble_key.bd_addr[4] << 8) + p_data->add_ble_key.bd_addr[5]);
    }
}

/*******************************************************************************
**
** Function         bta_dm_add_ble_device
**
** Description      This function adds an BLE device to an security database entry.
**                  It is normally called during host startup to restore all required information
**                  stored in the NVRAM.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_add_ble_device (tBTA_DM_MSG *p_data)
{
    if (!BTM_SecAddBleDevice (p_data->add_ble_device.bd_addr, NULL,
                              p_data->add_ble_device.dev_type  ,
                              p_data->add_ble_device.addr_type)) {
        APPL_TRACE_ERROR ("BTA_DM: Error adding BLE Device for device %08x%04x",
                          (p_data->add_ble_device.bd_addr[0] << 24) + (p_data->add_ble_device.bd_addr[1] << 16) + \
                          (p_data->add_ble_device.bd_addr[2] << 8) + p_data->add_ble_device.bd_addr[3],
                          (p_data->add_ble_device.bd_addr[4] << 8) + p_data->add_ble_device.bd_addr[5]);
    }
}

/*******************************************************************************
**
** Function         bta_dm_add_ble_device
**
** Description      This function adds an BLE device to an security database entry.
**                  It is normally called during host startup to restore all required information
**                  stored in the NVRAM.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_passkey_reply (tBTA_DM_MSG *p_data)
{
    if (p_data->pin_reply.accept) {
        BTM_BlePasskeyReply(p_data->ble_passkey_reply.bd_addr, BTM_SUCCESS, p_data->ble_passkey_reply.passkey);
    } else {
        BTM_BlePasskeyReply(p_data->ble_passkey_reply.bd_addr, BTM_NOT_AUTHORIZED, p_data->ble_passkey_reply.passkey);
    }

}

/*******************************************************************************
**
** Function         bta_dm_ble_confirm_reply
**
** Description      This is response to SM numeric comparison request submitted
**                  to application.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_confirm_reply (tBTA_DM_MSG *p_data)
{
    if (p_data->confirm.accept) {
        BTM_BleConfirmReply(p_data->confirm.bd_addr, BTM_SUCCESS);
    } else {
        BTM_BleConfirmReply(p_data->ble_passkey_reply.bd_addr, BTM_NOT_AUTHORIZED);
    }
}

/*******************************************************************************
**
** Function         bta_dm_security_grant
**
** Description      This function grant SMP security request access.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_security_grant (tBTA_DM_MSG *p_data)
{
    BTM_SecurityGrant(p_data->ble_sec_grant.bd_addr, p_data->ble_sec_grant.res);
}
#endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         bta_dm_ble_set_bg_conn_type
**
** Description      This function set the BLE background connection type
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_bg_conn_type (tBTA_DM_MSG *p_data)
{
    BTM_BleSetBgConnType(p_data->ble_set_bd_conn_type.bg_conn_type,
                         p_data->ble_set_bd_conn_type.p_select_cback);
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_conn_params
**
** Description      This function set the preferred connection parameters.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_conn_params (tBTA_DM_MSG *p_data)
{
    BTM_BleSetPrefConnParams(p_data->ble_set_conn_params.peer_bda,
                             p_data->ble_set_conn_params.conn_int_min,
                             p_data->ble_set_conn_params.conn_int_max,
                             p_data->ble_set_conn_params.slave_latency,
                             p_data->ble_set_conn_params.supervision_tout);
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_conn_scan_params
**
** Description      This function sets BLE scan parameters.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_scan_params(tBTA_DM_MSG *p_data)
{
    BTM_BleSetScanParams(p_data->ble_set_scan_params.client_if,
                         p_data->ble_set_scan_params.scan_int,
                         p_data->ble_set_scan_params.scan_window,
                         p_data->ble_set_scan_params.scan_mode,
                         p_data->ble_set_scan_params.scan_param_setup_cback);
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_scan_fil_params
**
** Description      This function sets BLE scan filter & parameters.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_scan_fil_params(tBTA_DM_MSG *p_data)
{
    BTM_BleSetScanFilterParams (p_data->ble_set_scan_fil_params.client_if,
                                p_data->ble_set_scan_fil_params.scan_int,
                                p_data->ble_set_scan_fil_params.scan_window,
                                p_data->ble_set_scan_fil_params.scan_mode,
                                p_data->ble_set_scan_fil_params.addr_type_own,
                                p_data->ble_set_scan_fil_params.scan_filter_policy,
                                p_data->ble_set_scan_fil_params.scan_param_setup_cback);
}


/*******************************************************************************
**
** Function         bta_dm_ble_set_conn_scan_params
**
** Description      This function set the preferred connection scan parameters.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_conn_scan_params (tBTA_DM_MSG *p_data)
{
    BTM_BleSetConnScanParams(p_data->ble_set_conn_scan_params.scan_int,
                             p_data->ble_set_conn_scan_params.scan_window);
}
/*******************************************************************************
**
** Function         bta_dm_ble_update_conn_params
**
** Description      This function update LE connection parameters.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_update_conn_params (tBTA_DM_MSG *p_data)
{
    if (!L2CA_UpdateBleConnParams(p_data->ble_update_conn_params.bd_addr,
                                  p_data->ble_update_conn_params.min_int,
                                  p_data->ble_update_conn_params.max_int,
                                  p_data->ble_update_conn_params.latency,
                                  p_data->ble_update_conn_params.timeout)) {
        APPL_TRACE_ERROR("Update connection parameters failed!");
    }
}
/*******************************************************************************
**
** Function         bta_dm_ble_disconnect
**
** Description      This function disconnect the ble connection.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_disconnect (tBTA_DM_MSG *p_data)
{
    L2CA_RemoveFixedChnl(L2CAP_ATT_CID, p_data->ble_disconnect.remote_bda);
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_rand_address
**
** Description      This function set the LE random address for the device.
**
** Parameters:      rand_addr:the random address whitch should be setting
** Explanation:     This function added by Yulong at 2016/9/9
*******************************************************************************/
void bta_dm_ble_set_rand_address(tBTA_DM_MSG *p_data)
{
    BOOLEAN set_flag = false;
    if (p_data->set_addr.addr_type != BLE_ADDR_RANDOM) {
        APPL_TRACE_ERROR("Invalid random adress type = %d\n", p_data->set_addr.addr_type);
        return;
    }
    //send the setting random address to BTM layer
    if ((set_flag = BTM_BleSetRandAddress(p_data->set_addr.address) != TRUE)){
        APPL_TRACE_ERROR("%s,set random address fail.", __func__);
    }

}

/*******************************************************************************
**
** Function         bta_dm_ble_stop_advertising
**
** Description      This function stop the BLE avdertising for the device.
**
** Parameters:      void
** Explanation:     This function added by Yulong at 2016/10/19
*******************************************************************************/
void bta_dm_ble_stop_advertising(tBTA_DM_MSG *p_data)
{
    if (p_data->hdr.event != BTA_DM_API_BLE_STOP_ADV_EVT) {
        APPL_TRACE_ERROR("Invalid BTA event,cann't stop the BLE adverting\n");
    }

    btm_ble_stop_adv();
}



#if BLE_PRIVACY_SPT == TRUE
/*******************************************************************************
**
** Function         bta_dm_ble_config_local_privacy
**
** Description      This function set the local device LE privacy settings.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_config_local_privacy (tBTA_DM_MSG *p_data)
{
    BTM_BleConfigPrivacy (p_data->ble_local_privacy.privacy_enable, p_data->ble_local_privacy.set_local_privacy_cback);
}
#endif

/*******************************************************************************
**
** Function         bta_dm_ble_observe
**
** Description      This function set the preferred connection scan parameters.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_observe (tBTA_DM_MSG *p_data)
{
    tBTM_STATUS status;
    if (p_data->ble_observe.start) {
        /*Save the  callback to be called when a scan results are available */
        bta_dm_search_cb.p_scan_cback = p_data->ble_observe.p_cback;

        if ((status = BTM_BleObserve(TRUE, p_data->ble_observe.duration,
                                     bta_dm_observe_results_cb, bta_dm_observe_cmpl_cb)) != BTM_CMD_STARTED) {
            APPL_TRACE_WARNING(" %s start observe failed. status=0x%x\n", __FUNCTION__, status);
        }

        if (p_data->ble_observe.p_start_scan_cback) {
            status = (status == BTM_CMD_STARTED ? BTA_SUCCESS : BTA_FAILURE);
            p_data->ble_observe.p_start_scan_cback(status);
        }
    } else {
        bta_dm_search_cb.p_scan_cback = NULL;
        status = BTM_BleObserve(FALSE, 0, NULL, NULL);

        if (status != BTM_CMD_STARTED){
            APPL_TRACE_WARNING(" %s stop observe failed, status=0x%x\n", __FUNCTION__, status);
        }

        if (p_data->ble_observe.p_stop_scan_cback) {
            status = (status == BTM_CMD_STARTED ? BTA_SUCCESS : BTA_FAILURE);
            p_data->ble_observe.p_stop_scan_cback(status);
        }
    }
}
/*******************************************************************************
**
** Function         bta_dm_ble_set_adv_params
**
** Description      This function set the adv parameters.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_adv_params (tBTA_DM_MSG *p_data)
{
    BTM_BleSetAdvParams(p_data->ble_set_adv_params.adv_int_min,
                        p_data->ble_set_adv_params.adv_int_max,
                        p_data->ble_set_adv_params.p_dir_bda,
                        BTA_DM_BLE_ADV_CHNL_MAP);
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_adv_params_all
**
** Description      This function is called to set all of the advertising parameters.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
void bta_dm_ble_set_adv_params_all  (tBTA_DM_MSG *p_data)
{
    tBTA_STATUS status = BTA_FAILURE;

    if (BTM_BleSetAdvParamsStartAdv(p_data->ble_set_adv_params_all.adv_int_min,
                                p_data->ble_set_adv_params_all.adv_int_max,
                                p_data->ble_set_adv_params_all.adv_type,
                                p_data->ble_set_adv_params_all.addr_type_own,
                                p_data->ble_set_adv_params_all.p_dir_bda,
                                p_data->ble_set_adv_params_all.channel_map,
                                p_data->ble_set_adv_params_all.adv_filter_policy) == BTM_SUCCESS) {
        status = BTA_SUCCESS;
    }

    if (p_data->ble_set_adv_params_all.p_start_adv_cback) {
        (*p_data->ble_set_adv_params_all.p_start_adv_cback)(status);
    }
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_adv_config
**
** Description      This function set the customized ADV data configuration
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_adv_config (tBTA_DM_MSG *p_data)
{
    tBTA_STATUS status = BTA_FAILURE;

    if (BTM_BleWriteAdvData(p_data->ble_set_adv_data.data_mask,
                            (tBTM_BLE_ADV_DATA *)p_data->ble_set_adv_data.p_adv_cfg) == BTM_SUCCESS) {
        status = BTA_SUCCESS;
    }

    if (p_data->ble_set_adv_data.p_adv_data_cback) {
        (*p_data->ble_set_adv_data.p_adv_data_cback)(status);
    }
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_adv_config_raw
**
** Description      This function set the customized ADV data configuration
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_adv_config_raw (tBTA_DM_MSG *p_data)
{
    tBTA_STATUS status = BTA_FAILURE;

    if (BTM_BleWriteAdvDataRaw(p_data->ble_set_adv_data_raw.p_raw_adv,
                               p_data->ble_set_adv_data_raw.raw_adv_len) == BTM_SUCCESS) {
        status = BTA_SUCCESS;
    }

    if (p_data->ble_set_adv_data_raw.p_adv_data_cback) {
        (*p_data->ble_set_adv_data_raw.p_adv_data_cback)(status);
    }
}


/*******************************************************************************
**
** Function         bta_dm_ble_set_scan_rsp
**
** Description      This function set the customized ADV scan resp. configuration
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_scan_rsp (tBTA_DM_MSG *p_data)
{
    tBTA_STATUS status = BTA_FAILURE;

    if (BTM_BleWriteScanRsp(p_data->ble_set_adv_data.data_mask,
                            (tBTM_BLE_ADV_DATA *)p_data->ble_set_adv_data.p_adv_cfg) == BTM_SUCCESS) {
        status = BTA_SUCCESS;
    }

    if (p_data->ble_set_adv_data.p_adv_data_cback) {
        (*p_data->ble_set_adv_data.p_adv_data_cback)(status);
    }
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_scan_rsp_raw
**
** Description      This function set the raw scan response data
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_set_scan_rsp_raw (tBTA_DM_MSG *p_data)
{
    tBTA_STATUS status = BTA_FAILURE;

    if (BTM_BleWriteScanRspRaw(p_data->ble_set_adv_data_raw.p_raw_adv,
                               p_data->ble_set_adv_data_raw.raw_adv_len) == BTM_SUCCESS) {
        status = BTA_SUCCESS;
    }

    if (p_data->ble_set_adv_data_raw.p_adv_data_cback) {
        (*p_data->ble_set_adv_data_raw.p_adv_data_cback)(status);
    }
}

/*******************************************************************************
**
** Function         bta_dm_ble_set_data_length
**
** Description      This function set the maximum transmission packet size
**
** Parameters
**
*******************************************************************************/
void bta_dm_ble_set_data_length(tBTA_DM_MSG *p_data)
{
    tACL_CONN *p_acl_cb = btm_bda_to_acl(p_data->ble_set_data_length.remote_bda, BT_TRANSPORT_LE);
     if (p_acl_cb == NULL) {
         APPL_TRACE_ERROR("%s error: Invalid connection remote_bda.", __func__);
         return;
     } else {
         p_acl_cb->p_set_pkt_data_cback = p_data->ble_set_data_length.p_set_pkt_data_cback;
     }
     UINT8 status = BTM_SetBleDataLength(p_data->ble_set_data_length.remote_bda,
                                         p_data->ble_set_data_length.tx_data_length);
     if (status != BTM_SUCCESS) {
        APPL_TRACE_ERROR("%s failed\n", __FUNCTION__);
        if (p_data->ble_set_data_length.p_set_pkt_data_cback) {
            if (p_acl_cb->data_length_params.tx_len == 0){
                uint16_t length = controller_get_interface()->get_acl_data_size_ble();
                p_acl_cb->data_length_params.rx_len = length;
                p_acl_cb->data_length_params.tx_len = length;
            }
            (*p_data->ble_set_data_length.p_set_pkt_data_cback)(status, &p_acl_cb->data_length_params);
        }
    }
}

/*******************************************************************************
**
** Function         bta_dm_ble_broadcast
**
** Description      Starts or stops LE broadcasts
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_broadcast (tBTA_DM_MSG *p_data)
{
    tBTM_STATUS status = 0;
    BOOLEAN start = p_data->ble_observe.start;

    status = BTM_BleBroadcast(start);

    if (p_data->ble_observe.p_stop_adv_cback){
        if (status != BTM_SUCCESS){
            APPL_TRACE_WARNING("%s, %s, status=0x%x\n", __func__,\
                    (start == TRUE) ? "start adv failed" : "stop adv failed", status);
        }
        status = (status == BTM_SUCCESS ? BTA_SUCCESS : BTA_FAILURE);
        p_data->ble_observe.p_stop_adv_cback(status);
    }

}

/*******************************************************************************
**
** Function         bta_dm_ble_multi_adv_enb
**
** Description      This function enables a single advertising instance
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_multi_adv_enb(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;

    bta_dm_cb.p_multi_adv_cback = p_data->ble_multi_adv_enb.p_cback;
    if (BTM_BleMaxMultiAdvInstanceCount() > 0 && NULL != p_data->ble_multi_adv_enb.p_ref) {
        btm_status = BTM_BleEnableAdvInstance((tBTM_BLE_ADV_PARAMS *)
                                              p_data->ble_multi_adv_enb.p_params,
                                              p_data->ble_multi_adv_enb.p_cback,
                                              p_data->ble_multi_adv_enb.p_ref);
    }

    if (BTM_CMD_STARTED != btm_status) {
        bta_dm_cb.p_multi_adv_cback(BTA_BLE_MULTI_ADV_ENB_EVT, 0xFF,
                                    p_data->ble_multi_adv_enb.p_ref, BTA_FAILURE);
    }
}
/*******************************************************************************
**
** Function         bta_dm_ble_multi_adv_param_upd
**
** Description      This function updates multiple advertising instance parameters
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_multi_adv_upd_param(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;
    void *p_ref = NULL;

    if (BTM_BleMaxMultiAdvInstanceCount() > 0 && p_data->ble_multi_adv_param.inst_id > 0
            && p_data->ble_multi_adv_param.inst_id < BTM_BleMaxMultiAdvInstanceCount()) {
        btm_status = BTM_BleUpdateAdvInstParam(p_data->ble_multi_adv_param.inst_id,
                                               (tBTM_BLE_ADV_PARAMS *)p_data->ble_multi_adv_param.p_params);
    }

    if (BTM_CMD_STARTED != btm_status) {
        p_ref = btm_ble_multi_adv_get_ref(p_data->ble_multi_adv_param.inst_id);
        bta_dm_cb.p_multi_adv_cback(BTA_BLE_MULTI_ADV_PARAM_EVT,
                                    p_data->ble_multi_adv_param.inst_id, p_ref, BTA_FAILURE);
    }
}
/*******************************************************************************
**
** Function         bta_dm_ble_multi_adv_data
**
** Description      This function write multiple advertising instance adv data
**                  or scan response data
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_multi_adv_data(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;
    void *p_ref = NULL;

    if (BTM_BleMaxMultiAdvInstanceCount() > 0 && p_data->ble_multi_adv_data.inst_id > 0
            && p_data->ble_multi_adv_data.inst_id < BTM_BleMaxMultiAdvInstanceCount()) {
        btm_status = BTM_BleCfgAdvInstData(p_data->ble_multi_adv_data.inst_id,
                                           p_data->ble_multi_adv_data.is_scan_rsp,
                                           p_data->ble_multi_adv_data.data_mask,
                                           (tBTM_BLE_ADV_DATA *)p_data->ble_multi_adv_data.p_data);
    }

    if (BTM_CMD_STARTED != btm_status) {
        p_ref = btm_ble_multi_adv_get_ref(p_data->ble_multi_adv_data.inst_id);
        bta_dm_cb.p_multi_adv_cback(BTA_BLE_MULTI_ADV_DATA_EVT,
                                    p_data->ble_multi_adv_data.inst_id, p_ref, BTA_FAILURE);
    }

}
/*******************************************************************************
**
** Function         btm_dm_ble_multi_adv_disable
**
** Description      This function disable a single adv instance
**
** Parameters:
**
*******************************************************************************/
void btm_dm_ble_multi_adv_disable(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;
    void *p_ref = NULL;

    if (BTM_BleMaxMultiAdvInstanceCount() > 0 && p_data->ble_multi_adv_disable.inst_id > 0
            && p_data->ble_multi_adv_disable.inst_id < BTM_BleMaxMultiAdvInstanceCount()) {
        btm_status = BTM_BleDisableAdvInstance(p_data->ble_multi_adv_disable.inst_id);
    }

    if (BTM_CMD_STARTED != btm_status) {
        p_ref = btm_ble_multi_adv_get_ref(p_data->ble_multi_adv_disable.inst_id);
        bta_dm_cb.p_multi_adv_cback(BTA_BLE_MULTI_ADV_DISABLE_EVT,
                                    p_data->ble_multi_adv_disable.inst_id, p_ref, BTA_FAILURE);
    }
}

/*******************************************************************************
**
** Function         bta_dm_ble_setup_storage
**
** Description      This function configures up the storage parameters for ADV batch scanning
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_setup_storage (tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 != cmn_ble_vsc_cb.tot_scan_results_strg) {
        btm_status = BTM_BleSetStorageConfig(p_data->ble_set_storage.batch_scan_full_max,
                                             p_data->ble_set_storage.batch_scan_trunc_max,
                                             p_data->ble_set_storage.batch_scan_notify_threshold,
                                             p_data->ble_set_storage.p_setup_cback,
                                             p_data->ble_set_storage.p_thres_cback,
                                             p_data->ble_set_storage.p_read_rep_cback,
                                             p_data->ble_set_storage.ref_value);
    }

    if (BTM_CMD_STARTED != btm_status)
        bta_ble_scan_setup_cb(BTM_BLE_BATCH_SCAN_CFG_STRG_EVT, p_data->ble_set_storage.ref_value,
                              btm_status);
}

/*******************************************************************************
**
** Function         bta_dm_ble_enable_batch_scan
**
** Description      This function sets up the parameters and enables batch scan
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_enable_batch_scan (tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 != cmn_ble_vsc_cb.tot_scan_results_strg) {
        btm_status = BTM_BleEnableBatchScan(p_data->ble_enable_scan.scan_mode,
                                            p_data->ble_enable_scan.scan_int,
                                            p_data->ble_enable_scan.scan_window,
                                            p_data->ble_enable_scan.discard_rule,
                                            p_data->ble_enable_scan.addr_type,
                                            p_data->ble_enable_scan.ref_value);
    }

    if (BTM_CMD_STARTED != btm_status)
        bta_ble_scan_setup_cb(BTM_BLE_BATCH_SCAN_ENABLE_EVT, p_data->ble_enable_scan.ref_value,
                              btm_status);
}

/*******************************************************************************
**
** Function         bta_dm_ble_disable_batch_scan
**
** Description      This function disables the batch scan
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_disable_batch_scan (tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);
    tBTM_STATUS btm_status = 0;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 != cmn_ble_vsc_cb.tot_scan_results_strg) {
        btm_status = BTM_BleDisableBatchScan(p_data->ble_disable_scan.ref_value);
    }

    if (BTM_CMD_STARTED != btm_status)
        bta_ble_scan_setup_cb(BTM_BLE_BATCH_SCAN_DISABLE_EVT, p_data->ble_enable_scan.ref_value,
                              btm_status);
}

/*******************************************************************************
**
** Function         bta_dm_ble_read_scan_reports
**
** Description      This function reads the batch scan reports
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_read_scan_reports(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 != cmn_ble_vsc_cb.tot_scan_results_strg) {
        btm_status = BTM_BleReadScanReports(p_data->ble_read_reports.scan_type,
                                            p_data->ble_read_reports.ref_value);
    }

    if (BTM_CMD_STARTED != btm_status)
        bta_ble_scan_setup_cb(BTM_BLE_BATCH_SCAN_READ_REPTS_EVT, p_data->ble_enable_scan.ref_value,
                              btm_status);
}

/*******************************************************************************
**
** Function         bta_dm_ble_track_advertiser
**
** Description      This function tracks the specific advertiser
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_track_advertiser(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;
    BD_ADDR bda;
    memset(&bda, 0 , sizeof(BD_ADDR));
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;
    tBTA_DM_BLE_TRACK_ADV_DATA track_adv_data;

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 != cmn_ble_vsc_cb.tot_scan_results_strg) {
        btm_status = BTM_BleTrackAdvertiser((tBTM_BLE_TRACK_ADV_CBACK *)
                                            p_data->ble_track_advert.p_track_adv_cback,
                                            p_data->ble_track_advert.ref_value);
    }

    if (BTM_CMD_STARTED != btm_status) {
        memset(&track_adv_data, 0, sizeof(tBTA_DM_BLE_TRACK_ADV_DATA));
        track_adv_data.advertiser_info_present = NO_ADV_INFO_PRESENT; /* Indicates failure */
        track_adv_data.client_if = (UINT8)p_data->ble_track_advert.ref_value;
        p_data->ble_track_advert.p_track_adv_cback(&track_adv_data);
    }
}

/*******************************************************************************
**
** Function         bta_ble_scan_setup_cb
**
** Description      Handle the setup callback from BTM layer and forward it to app layer
**
** Parameters:
**
*******************************************************************************/
void bta_ble_scan_setup_cb(tBTM_BLE_BATCH_SCAN_EVT evt, tBTM_BLE_REF_VALUE ref_value,
                           tBTM_STATUS status)
{
    tBTA_BLE_BATCH_SCAN_EVT bta_evt = 0;

    APPL_TRACE_DEBUG("bta_ble_scan_setup_cb : evt: %d, ref_value: %d, status:%d", evt,
                     ref_value, status);

    switch (evt) {
    case BTM_BLE_BATCH_SCAN_ENABLE_EVT:
        bta_evt = BTA_BLE_BATCH_SCAN_ENB_EVT;
        break;
    case BTM_BLE_BATCH_SCAN_CFG_STRG_EVT:
        bta_evt = BTA_BLE_BATCH_SCAN_CFG_STRG_EVT;
        break;
    case BTM_BLE_BATCH_SCAN_DISABLE_EVT:
        bta_evt = BTA_BLE_BATCH_SCAN_DIS_EVT;
        break;
    case BTM_BLE_BATCH_SCAN_PARAM_EVT:
        bta_evt = BTA_BLE_BATCH_SCAN_PARAM_EVT;
        break;
    default:
        break;
    }

    if (NULL != bta_dm_cb.p_setup_cback) {
        bta_dm_cb.p_setup_cback(bta_evt, ref_value, status);
    }
}


#if BLE_ANDROID_CONTROLLER_SCAN_FILTER == TRUE
/*******************************************************************************
**
** Function         bta_ble_scan_pf_cmpl
**
** Description      ADV payload filtering operation complete callback
**
**
** Returns         TRUE if handled, otherwise FALSE.
**
*******************************************************************************/
static void bta_ble_scan_cfg_cmpl(tBTM_BLE_PF_ACTION action, tBTM_BLE_SCAN_COND_OP cfg_op,
                                  tBTM_BLE_PF_AVBL_SPACE avbl_space, tBTM_STATUS status,
                                  tBTM_BLE_REF_VALUE ref_value)
{
    tBTA_STATUS st = (status == BTM_SUCCESS) ? BTA_SUCCESS : BTA_FAILURE;

    APPL_TRACE_DEBUG("bta_ble_scan_cfg_cmpl: %d, %d, %d, %d", action, cfg_op, avbl_space, status);

    if (bta_dm_cb.p_scan_filt_cfg_cback) {
        bta_dm_cb.p_scan_filt_cfg_cback(action, cfg_op, avbl_space, st, ref_value);
    }
}

/*******************************************************************************
**
** Function         bta_dm_cfg_filter_cond
**
** Description      This function configure adv payload filtering condition
**
** Parameters:
**
*******************************************************************************/
void bta_dm_cfg_filter_cond (tBTA_DM_MSG *p_data)
{
    tBTM_STATUS st = BTM_MODE_UNSUPPORTED;
    tBTA_STATUS status = BTA_FAILURE;

    tBTM_BLE_VSC_CB cmn_vsc_cb;

    APPL_TRACE_DEBUG("bta_dm_cfg_filter_cond");
    BTM_BleGetVendorCapabilities(&cmn_vsc_cb);
    if (0 != cmn_vsc_cb.filter_support) {
        if ((st = BTM_BleCfgFilterCondition(p_data->ble_cfg_filter_cond.action,
                                            p_data->ble_cfg_filter_cond.cond_type,
                                            (tBTM_BLE_PF_FILT_INDEX)p_data->ble_cfg_filter_cond.filt_index,
                                            (tBTM_BLE_PF_COND_PARAM *)p_data->ble_cfg_filter_cond.p_cond_param,
                                            bta_ble_scan_cfg_cmpl, p_data->ble_cfg_filter_cond.ref_value))
                == BTM_CMD_STARTED) {
            bta_dm_cb.p_scan_filt_cfg_cback = p_data->ble_cfg_filter_cond.p_filt_cfg_cback;
            return;
        }
    }

    if (p_data->ble_cfg_filter_cond.p_filt_cfg_cback)
        p_data->ble_cfg_filter_cond.p_filt_cfg_cback(BTA_DM_BLE_PF_CONFIG_EVT,
                p_data->ble_cfg_filter_cond.cond_type, 0, status,
                p_data->ble_cfg_filter_cond.ref_value);
    return;
}

/*******************************************************************************
**
** Function         bta_dm_enable_scan_filter
**
** Description      This function enable/disable adv payload filtering condition
**
** Parameters:
**
*******************************************************************************/
void bta_dm_enable_scan_filter(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS st = BTM_MODE_UNSUPPORTED;
    tBTA_STATUS status = BTA_FAILURE;

    tBTM_BLE_VSC_CB cmn_vsc_cb;
    APPL_TRACE_DEBUG("bta_dm_enable_scan_filter");
    BTM_BleGetVendorCapabilities(&cmn_vsc_cb);

    if (0 != cmn_vsc_cb.filter_support) {
        if ((st = BTM_BleEnableDisableFilterFeature(p_data->ble_enable_scan_filt.action,
                  p_data->ble_enable_scan_filt.p_filt_status_cback,
                  (tBTM_BLE_REF_VALUE)p_data->ble_enable_scan_filt.ref_value)) == BTM_CMD_STARTED) {
            bta_dm_cb.p_scan_filt_status_cback = p_data->ble_enable_scan_filt.p_filt_status_cback;
        }
        return;
    }

    if (p_data->ble_enable_scan_filt.p_filt_status_cback)
        p_data->ble_enable_scan_filt.p_filt_status_cback (BTA_DM_BLE_PF_ENABLE_EVT,
                p_data->ble_enable_scan_filt.ref_value, status);

}

/*******************************************************************************
**
** Function         bta_dm_scan_filter_param_setup
**
** Description      This function sets up scan filter params
**
** Parameters:
**
*******************************************************************************/
void bta_dm_scan_filter_param_setup (tBTA_DM_MSG *p_data)
{
    tBTM_STATUS st = BTM_MODE_UNSUPPORTED;
    tBTA_STATUS status = BTA_FAILURE;

    tBTM_BLE_VSC_CB cmn_vsc_cb;

    APPL_TRACE_DEBUG("bta_dm_scan_filter_param_setup");
    BTM_BleGetVendorCapabilities(&cmn_vsc_cb);
    if (0 != cmn_vsc_cb.filter_support) {
        if ((st = BTM_BleAdvFilterParamSetup(p_data->ble_scan_filt_param_setup.action,
                                             p_data->ble_scan_filt_param_setup.filt_index,
                                             (tBTM_BLE_PF_FILT_PARAMS *)&p_data->ble_scan_filt_param_setup.filt_params,
                                             p_data->ble_scan_filt_param_setup.p_target,
                                             p_data->ble_scan_filt_param_setup.p_filt_param_cback,
                                             p_data->ble_scan_filt_param_setup.ref_value)) == BTM_CMD_STARTED) {
            bta_dm_cb.p_scan_filt_param_cback = p_data->ble_scan_filt_param_setup.p_filt_param_cback;
            return;
        }
    }

    if (p_data->ble_scan_filt_param_setup.p_filt_param_cback)
        p_data->ble_scan_filt_param_setup.p_filt_param_cback (BTA_DM_BLE_PF_ENABLE_EVT, 0,
                p_data->ble_scan_filt_param_setup.ref_value, status);

    return;
}
#endif

/*******************************************************************************
**
** Function         bta_ble_enable_scan_cmpl
**
** Description      ADV payload filtering enable / disable complete callback
**
**
** Returns          None
**
*******************************************************************************/
static void bta_ble_energy_info_cmpl(tBTM_BLE_TX_TIME_MS tx_time,
                                     tBTM_BLE_RX_TIME_MS rx_time,
                                     tBTM_BLE_IDLE_TIME_MS idle_time,
                                     tBTM_BLE_ENERGY_USED  energy_used,
                                     tBTM_STATUS status)
{
    tBTA_STATUS st = (status == BTM_SUCCESS) ? BTA_SUCCESS : BTA_FAILURE;
    tBTA_DM_CONTRL_STATE ctrl_state = 0;
#if ((defined BTA_GATT_INCLUDED) &&  (BTA_GATT_INCLUDED == TRUE) && SDP_INCLUDED == TRUE)
    if (BTA_SUCCESS == st) {
        ctrl_state = bta_dm_pm_obtain_controller_state();
    }
#endif  
    if (bta_dm_cb.p_energy_info_cback) {
        bta_dm_cb.p_energy_info_cback(tx_time, rx_time, idle_time, energy_used, ctrl_state, st);
    }
}

/*******************************************************************************
**
** Function         bta_dm_ble_get_energy_info
**
** Description      This function obtains the energy info
**
** Parameters:
**
*******************************************************************************/
void bta_dm_ble_get_energy_info(tBTA_DM_MSG *p_data)
{
    tBTM_STATUS btm_status = 0;

    bta_dm_cb.p_energy_info_cback = p_data->ble_energy_info.p_energy_info_cback;
    btm_status = BTM_BleGetEnergyInfo(bta_ble_energy_info_cmpl);
    if (BTM_CMD_STARTED != btm_status) {
        bta_ble_energy_info_cmpl(0, 0, 0, 0, btm_status);
    }
}

#if ((defined BTA_GATT_INCLUDED) &&  (BTA_GATT_INCLUDED == TRUE) && SDP_INCLUDED == TRUE)
#ifndef BTA_DM_GATT_CLOSE_DELAY_TOUT
#define BTA_DM_GATT_CLOSE_DELAY_TOUT    1000
#endif

/*******************************************************************************
**
** Function         bta_dm_gattc_register
**
** Description      Register with GATTC in DM if BLE is needed.
**
**
** Returns          void
**
*******************************************************************************/
#if (GATTC_INCLUDED == TRUE)
static void bta_dm_gattc_register(void)
{
    tBT_UUID                app_uuid = {LEN_UUID_128, {0}};

    if (bta_dm_search_cb.client_if == BTA_GATTS_INVALID_IF) {
        memset (&app_uuid.uu.uuid128, 0x87, LEN_UUID_128);
        BTA_GATTC_AppRegister(&app_uuid, bta_dm_gattc_callback);
    }
}
#endif /* GATTC_INCLUDED == TRUE */
/*******************************************************************************
**
** Function         btm_dm_start_disc_gatt_services
**
** Description      This function starts a GATT service search request.
**
** Parameters:
**
*******************************************************************************/
static void btm_dm_start_disc_gatt_services (UINT16 conn_id)
{
    tBT_UUID    *p_uuid = bta_dm_search_cb.p_srvc_uuid +
                          bta_dm_search_cb.num_uuid - bta_dm_search_cb.uuid_to_search;

    p_uuid = bta_dm_search_cb.p_srvc_uuid +
             bta_dm_search_cb.num_uuid - bta_dm_search_cb.uuid_to_search;

    /* always search for all services */
    BTA_GATTC_ServiceSearchRequest(conn_id, p_uuid);
}

/*******************************************************************************
**
** Function         bta_dm_gatt_disc_result
**
** Description      This function process the GATT service search result.
**
** Parameters:
**
*******************************************************************************/
#if (GATTC_INCLUDED == TRUE)
static void bta_dm_gatt_disc_result(tBTA_GATT_ID service_id)
{
    tBTA_DM_SEARCH   result;

    /*
        * This logic will not work for gatt case.  We are checking against the bluetooth profiles here
        * just copy the GATTID in raw data field and send it across.
        */


    if ( bta_dm_search_cb.ble_raw_used + sizeof(tBTA_GATT_ID) < bta_dm_search_cb.ble_raw_size ) {
        APPL_TRACE_DEBUG("ADDING BLE SERVICE uuid=0x%x, ble_ptr = %p, ble_raw_used = 0x%x",
                         service_id.uuid.uu.uuid16, bta_dm_search_cb.p_ble_rawdata, bta_dm_search_cb.ble_raw_used);

        if (bta_dm_search_cb.p_ble_rawdata) {
            memcpy((bta_dm_search_cb.p_ble_rawdata + bta_dm_search_cb.ble_raw_used), &service_id,
                   sizeof(service_id) );

            bta_dm_search_cb.ble_raw_used += sizeof(service_id);
        } else {
            APPL_TRACE_ERROR("p_ble_rawdata is NULL");
        }

    } else {
        APPL_TRACE_ERROR("%s out of room to accomodate more service ids ble_raw_size = %d ble_raw_used = %d", __FUNCTION__, bta_dm_search_cb.ble_raw_size, bta_dm_search_cb.ble_raw_used );
    }

    LOG_INFO("%s service_id_uuid_len=%d ", __func__, service_id.uuid.len);
    if ( bta_dm_search_cb.state != BTA_DM_SEARCH_IDLE) {

        /* send result back to app now, one by one */
        bdcpy (result.disc_ble_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
        BCM_STRNCPY_S((char *)result.disc_ble_res.bd_name, sizeof(BD_NAME), bta_dm_get_remname(), (BD_NAME_LEN - 1));
        result.disc_ble_res.bd_name[BD_NAME_LEN] = 0;
        memcpy(&result.disc_ble_res.service, &service_id.uuid, sizeof(tBT_UUID));

        bta_dm_search_cb.p_search_cback(BTA_DM_DISC_BLE_RES_EVT, &result);
    }
}
#endif /* #if (GATTC_INCLUDED == TRUE) */
/*******************************************************************************
**
** Function         bta_dm_gatt_disc_complete
**
** Description      This function process the GATT service search complete.
**
** Parameters:
**
*******************************************************************************/
static void bta_dm_gatt_disc_complete(UINT16 conn_id, tBTA_GATT_STATUS status)
{
    tBTA_DM_MSG *p_msg;

    APPL_TRACE_DEBUG("bta_dm_gatt_disc_complete conn_id = %d", conn_id);

    if (bta_dm_search_cb.uuid_to_search > 0) {
        bta_dm_search_cb.uuid_to_search --;
    }

    if (status == BTA_GATT_OK && bta_dm_search_cb.uuid_to_search > 0) {
        btm_dm_start_disc_gatt_services(conn_id);
    } else {
        bta_dm_search_cb.uuid_to_search = 0;

        /* no more services to be discovered */
        if ((p_msg = (tBTA_DM_MSG *) osi_malloc(sizeof(tBTA_DM_MSG))) != NULL) {
            p_msg->hdr.event = BTA_DM_DISCOVERY_RESULT_EVT;
            p_msg->disc_result.result.disc_res.result = (status == BTA_GATT_OK) ? BTA_SUCCESS : BTA_FAILURE;
            APPL_TRACE_DEBUG("%s service found: 0x%08x", __FUNCTION__,
                             bta_dm_search_cb.services_found);
            p_msg->disc_result.result.disc_res.services = bta_dm_search_cb.services_found;
            p_msg->disc_result.result.disc_res.num_uuids = 0;
            p_msg->disc_result.result.disc_res.p_uuid_list = NULL;
            bdcpy (p_msg->disc_result.result.disc_res.bd_addr, bta_dm_search_cb.peer_bdaddr);
            BCM_STRNCPY_S((char *)p_msg->disc_result.result.disc_res.bd_name, sizeof(BD_NAME),
                          bta_dm_get_remname(), (BD_NAME_LEN - 1));

            /* make sure the string is terminated */
            p_msg->disc_result.result.disc_res.bd_name[BD_NAME_LEN - 1] = 0;

            p_msg->disc_result.result.disc_res.device_type |= BT_DEVICE_TYPE_BLE;
            if ( bta_dm_search_cb.ble_raw_used > 0 ) {
                p_msg->disc_result.result.disc_res.p_raw_data = osi_malloc(bta_dm_search_cb.ble_raw_used);

                memcpy( p_msg->disc_result.result.disc_res.p_raw_data,
                        bta_dm_search_cb.p_ble_rawdata,
                        bta_dm_search_cb.ble_raw_used );

                p_msg->disc_result.result.disc_res.raw_data_size = bta_dm_search_cb.ble_raw_used;
            } else {
                p_msg->disc_result.result.disc_res.p_raw_data = NULL;
                bta_dm_search_cb.p_ble_rawdata = 0;
            }

            bta_sys_sendmsg(p_msg);
        }

        if (conn_id != BTA_GATT_INVALID_CONN_ID) {
            /* start a GATT channel close delay timer */
            bta_sys_start_timer(&bta_dm_search_cb.gatt_close_timer, BTA_DM_DISC_CLOSE_TOUT_EVT,
                                BTA_DM_GATT_CLOSE_DELAY_TOUT);
            bdcpy(bta_dm_search_cb.pending_close_bda, bta_dm_search_cb.peer_bdaddr);
        }
        bta_dm_search_cb.gatt_disc_active = FALSE;
    }
}

/*******************************************************************************
**
** Function         bta_dm_close_gatt_conn
**
** Description      This function close the GATT connection after delay timeout.
**
** Parameters:
**
*******************************************************************************/
#if (GATTC_INCLUDED == TRUE)
void bta_dm_close_gatt_conn(tBTA_DM_MSG *p_data)
{
    UNUSED(p_data);

    if (bta_dm_search_cb.conn_id != BTA_GATT_INVALID_CONN_ID) {
        BTA_GATTC_Close(bta_dm_search_cb.conn_id);
    }

    memset(bta_dm_search_cb.pending_close_bda, 0, BD_ADDR_LEN);
    bta_dm_search_cb.conn_id = BTA_GATT_INVALID_CONN_ID;
}
#endif /* #if (GATTC_INCLUDED == TRUE) */
/*******************************************************************************
**
** Function         btm_dm_start_gatt_discovery
**
** Description      This is GATT initiate the service search by open a GATT connection
**                  first.
**
** Parameters:
**
*******************************************************************************/
#if (GATTC_INCLUDED == TRUE)
void btm_dm_start_gatt_discovery (BD_ADDR bd_addr)
{
    bta_dm_search_cb.gatt_disc_active = TRUE;

    /* connection is already open */
    if (bdcmp(bta_dm_search_cb.pending_close_bda, bd_addr) == 0 &&
            bta_dm_search_cb.conn_id != BTA_GATT_INVALID_CONN_ID) {
        memset(bta_dm_search_cb.pending_close_bda, 0, BD_ADDR_LEN);
        bta_sys_stop_timer(&bta_dm_search_cb.gatt_close_timer);
        btm_dm_start_disc_gatt_services(bta_dm_search_cb.conn_id);
    } else {
        BTA_GATTC_Open(bta_dm_search_cb.client_if, bd_addr, TRUE, BTA_GATT_TRANSPORT_LE);
    }
}
#endif /* #if (GATTC_INCLUDED == TRUE) */
/*******************************************************************************
**
** Function         bta_dm_cancel_gatt_discovery
**
** Description      This is GATT cancel the GATT service search.
**
** Parameters:
**
*******************************************************************************/
#if (GATTC_INCLUDED == TRUE)
static void bta_dm_cancel_gatt_discovery(BD_ADDR bd_addr)
{
    if (bta_dm_search_cb.conn_id == BTA_GATT_INVALID_CONN_ID) {
        BTA_GATTC_CancelOpen(bta_dm_search_cb.client_if, bd_addr, TRUE);
    }

    bta_dm_gatt_disc_complete(bta_dm_search_cb.conn_id, (tBTA_GATT_STATUS) BTA_GATT_ERROR);
}
#endif /* #if (GATTC_INCLUDED == TRUE) */
/*******************************************************************************
**
** Function         bta_dm_proc_open_evt
**
** Description      process BTA_GATTC_OPEN_EVT in DM.
**
** Parameters:
**
*******************************************************************************/
void bta_dm_proc_open_evt(tBTA_GATTC_OPEN *p_data)
{
    UINT8           *p1;
    UINT8           *p2;

    p1 = bta_dm_search_cb.peer_bdaddr;
    p2 = p_data->remote_bda;

    APPL_TRACE_DEBUG("DM Search state= %d search_cb.peer_dbaddr: [%08x%04x] connected_bda= [%08x%04x] ",
                     bta_dm_search_cb.state,
                     ((p1[0]) << 24) + ((p1[1]) << 16) + ((p1[2]) << 8) + (p1[3]),
                     ((p1[4]) << 8) + p1[5],
                     ((p2[0]) << 24) + ((p2[1]) << 16) + ((p2[2]) << 8) + (p2[3]),
                     ((p2[4]) << 8) + p2[5]);

    APPL_TRACE_DEBUG("BTA_GATTC_OPEN_EVT conn_id = %d client_if=%d status = %d" ,
                     p_data->conn_id,
                     p_data->client_if,
                     p_data->status);

    bta_dm_search_cb.conn_id = p_data->conn_id;

    if (p_data->status == BTA_GATT_OK) {
        btm_dm_start_disc_gatt_services(p_data->conn_id);
    } else {
        bta_dm_gatt_disc_complete(BTA_GATT_INVALID_CONN_ID, p_data->status);
    }
}

/*******************************************************************************
**
** Function         bta_dm_gattc_callback
**
** Description      This is GATT client callback function used in DM.
**
** Parameters:
**
*******************************************************************************/
#if (GATTC_INCLUDED == TRUE)
static void bta_dm_gattc_callback(tBTA_GATTC_EVT event, tBTA_GATTC *p_data)
{
    APPL_TRACE_DEBUG("bta_dm_gattc_callback event = %d", event);

    switch (event) {
    case BTA_GATTC_REG_EVT:
        APPL_TRACE_DEBUG("BTA_GATTC_REG_EVT client_if = %d",  p_data->reg_oper.client_if);
        if (p_data->reg_oper.status == BTA_GATT_OK) {
            bta_dm_search_cb.client_if = p_data->reg_oper.client_if;
        } else {
            bta_dm_search_cb.client_if = BTA_GATTS_INVALID_IF;
        }
        break;

    case BTA_GATTC_OPEN_EVT:
        bta_dm_proc_open_evt(&p_data->open);
        break;

    case BTA_GATTC_SEARCH_RES_EVT:
        bta_dm_gatt_disc_result(p_data->srvc_res.service_uuid);
        break;

    case BTA_GATTC_SEARCH_CMPL_EVT:
        if ( bta_dm_search_cb.state != BTA_DM_SEARCH_IDLE) {
            bta_dm_gatt_disc_complete(p_data->search_cmpl.conn_id, p_data->search_cmpl.status);
        }
        break;

    case BTA_GATTC_CLOSE_EVT:
        APPL_TRACE_DEBUG("BTA_GATTC_CLOSE_EVT reason = %d", p_data->close.reason);
        /* in case of disconnect before search is completed */
        if ( (bta_dm_search_cb.state != BTA_DM_SEARCH_IDLE) &&
                (bta_dm_search_cb.state != BTA_DM_SEARCH_ACTIVE) &&
                !memcmp(p_data->close.remote_bda, bta_dm_search_cb.peer_bdaddr, BD_ADDR_LEN)) {
            bta_dm_gatt_disc_complete((UINT16)BTA_GATT_INVALID_CONN_ID,  (tBTA_GATT_STATUS) BTA_GATT_ERROR);
        }
        break;

    default:
        break;
    }
}
#endif /* #if (GATTC_INCLUDED == TRUE) */
#endif /* BTA_GATT_INCLUDED */

#if BLE_VND_INCLUDED == TRUE
/*******************************************************************************
**
** Function         bta_dm_ctrl_features_rd_cmpl_cback
**
** Description      callback to handle controller feature read complete
**
** Parameters:
**
*******************************************************************************/
static void bta_dm_ctrl_features_rd_cmpl_cback(tBTM_STATUS result)
{
    APPL_TRACE_DEBUG("%s  status = %d ", __FUNCTION__, result);
    if (result == BTM_SUCCESS) {
        if (bta_dm_cb.p_sec_cback) {
            bta_dm_cb.p_sec_cback(BTA_DM_LE_FEATURES_READ, NULL);
        }
    } else {
        APPL_TRACE_ERROR("%s Ctrl BLE feature read failed: status :%d", __FUNCTION__, result);
    }

}
#endif /* BLE_VND_INCLUDED */

#endif  /* BLE_INCLUDED */
