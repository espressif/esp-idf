/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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
 *  This file contains functions that interface with the HCI transport. On
 *  the receive side, it routes events to the appropriate handler, e.g.
 *  L2CAP, ScoMgr. On the transmit side, it manages the command
 *  transmission.
 *
 ******************************************************************************/

//#define LOG_TAG "bt_btu_hcif"

//#include <stdlib.h>
#include <string.h>

#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "l2c_int.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include "stack/acl_hci_link_interface.h"
//#include "bt_utils.h"
#include "device/controller.h"
#include "osi/osi.h"
#include "hci/hci_layer.h"
#include "common/bt_trace.h"

#include "osi/thread.h"
#include "osi/pkt_queue.h"
//#include "osi/mutex.h"
// TODO(zachoverflow): remove this horrible hack
#include "stack/btu.h"

extern void btm_process_cancel_complete(UINT8 status, UINT8 mode);
extern void btm_ble_test_command_complete(UINT8 *p);

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
static void btu_hcif_inquiry_comp_evt (UINT8 *p);
static void btu_hcif_inquiry_result_evt (UINT8 *p);
static void btu_hcif_inquiry_rssi_result_evt (UINT8 *p);
static void btu_hcif_extended_inquiry_result_evt (UINT8 *p);

static void btu_hcif_connection_comp_evt (UINT8 *p);
static void btu_hcif_connection_request_evt (UINT8 *p);
static void btu_hcif_disconnection_comp_evt (UINT8 *p);
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_authentication_comp_evt (UINT8 *p);
#endif  ///SMP_INCLUDED == TRUE
static void btu_hcif_rmt_name_request_comp_evt (UINT8 *p, UINT16 evt_len);
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_encryption_change_evt (UINT8 *p);
#endif  ///SMP_INCLUDED == TRUE
static void btu_hcif_read_rmt_features_comp_evt (UINT8 *p);
static void btu_hcif_read_rmt_ext_features_comp_evt (UINT8 *p);
static void btu_hcif_read_rmt_version_comp_evt (UINT8 *p);
static void btu_hcif_qos_setup_comp_evt (UINT8 *p);
static void btu_hcif_command_complete_evt (BT_HDR *response, void *context);
static void btu_hcif_command_status_evt (uint8_t status, BT_HDR *command, void *context);
static void btu_hcif_hardware_error_evt (UINT8 *p);
static void btu_hcif_flush_occured_evt (void);
static void btu_hcif_role_change_evt (UINT8 *p);
static void btu_hcif_num_compl_data_pkts_evt (UINT8 *p);
static void btu_hcif_mode_change_evt (UINT8 *p);
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_pin_code_request_evt (UINT8 *p);
static void btu_hcif_link_key_request_evt (UINT8 *p);
static void btu_hcif_link_key_notification_evt (UINT8 *p);
#endif  ///SMP_INCLUDED == TRUE
static void btu_hcif_loopback_command_evt (void);
static void btu_hcif_data_buf_overflow_evt (void);
static void btu_hcif_max_slots_changed_evt (void);
static void btu_hcif_read_clock_off_comp_evt (UINT8 *p);
static void btu_hcif_conn_pkt_type_change_evt (UINT8 *p);
static void btu_hcif_qos_violation_evt (UINT8 *p);
static void btu_hcif_page_scan_mode_change_evt (void);
static void btu_hcif_page_scan_rep_mode_chng_evt (void);
static void btu_hcif_esco_connection_comp_evt(UINT8 *p);
static void btu_hcif_esco_connection_chg_evt(UINT8 *p);

static void btu_hcif_host_support_evt (UINT8 *p);
/* Simple Pairing Events */
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_io_cap_request_evt (UINT8 *p);
static void btu_hcif_io_cap_response_evt (UINT8 *p);
static void btu_hcif_user_conf_request_evt (UINT8 *p);
static void btu_hcif_user_passkey_request_evt (UINT8 *p);
static void btu_hcif_simple_pair_complete_evt (UINT8 *p);
static void btu_hcif_user_passkey_notif_evt (UINT8 *p);
static void btu_hcif_keypress_notif_evt (UINT8 *p);
#endif  /* (CLASSIC_BT_INCLUDED == TRUE) */
#if BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE
static void btu_hcif_rem_oob_request_evt (UINT8 *p);
#endif
static void btu_hcif_link_supv_to_changed_evt (UINT8 *p);
#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
static void btu_hcif_enhanced_flush_complete_evt (void);
#endif

static void btu_hcif_ssr_evt (UINT8 *p, UINT16 evt_len);

#if BLE_INCLUDED == TRUE
static void btu_ble_ll_conn_complete_evt (UINT8 *p, UINT16 evt_len);
static void btu_ble_read_remote_feat_evt (UINT8 *p);
static void btu_ble_ll_conn_param_upd_evt (UINT8 *p, UINT16 evt_len);
static void btu_ble_ll_get_conn_param_format_err_from_contoller (UINT8 status, UINT16 handle);
#if (SMP_INCLUDED == TRUE)
static void btu_ble_proc_ltk_req (UINT8 *p);
static void btu_hcif_encryption_key_refresh_cmpl_evt (UINT8 *p);
#endif  ///SMP_INCLUDED == TRUE
static void btu_ble_data_length_change_evt (UINT8 *p, UINT16 evt_len);
#if (BLE_LLT_INCLUDED == TRUE)
static void btu_ble_rc_param_req_evt(UINT8 *p);
#endif
//#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
static void btu_ble_proc_enhanced_conn_cmpl (UINT8 *p, UINT16 evt_len);
//#endif
#if (BLE_50_FEATURE_SUPPORT == TRUE)
static void btu_ble_phy_update_complete_evt(UINT8 *p);
static void btu_ble_ext_adv_report_evt(UINT8 *p, UINT16 evt_len);
static void btu_ble_periodic_adv_sync_establish_evt(UINT8 *p);
static void btu_ble_periodic_adv_report_evt(UINT8 *p, UINT8 evt_len);
static void btu_ble_periodic_adv_sync_lost_evt(UINT8 *p);
static void btu_ble_scan_timeout_evt(UINT8 *p);
static void btu_ble_adv_set_terminate_evt(UINT8 *p);
static void btu_ble_scan_req_received_evt(UINT8 *p);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
static void btu_ble_periodic_adv_sync_trans_recv(UINT8 *p);
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

extern osi_sem_t adv_enable_sem;
extern osi_sem_t adv_data_sem;
extern osi_sem_t adv_param_sem;
extern osi_sem_t scan_enable_sem;
extern osi_sem_t scan_param_sem;
extern uint8_t adv_enable_status;
extern uint8_t adv_data_status;
extern uint8_t adv_param_status;
extern uint8_t scan_enable_status;
extern uint8_t scan_param_status;

#endif

/*******************************************************************************
**
** Function         btu_hcif_process_event
**
** Description      This function is called when an event is received from
**                  the Host Controller.
**
** Returns          void
**
*******************************************************************************/
void btu_hcif_process_event (UNUSED_ATTR UINT8 controller_id, BT_HDR *p_msg)
{
    UINT8   *p = (UINT8 *)(p_msg + 1) + p_msg->offset;
    UINT8   hci_evt_code, hci_evt_len;
#if BLE_INCLUDED == TRUE
    UINT8   ble_sub_code;
#endif
    STREAM_TO_UINT8  (hci_evt_code, p);
    STREAM_TO_UINT8  (hci_evt_len, p);

    switch (hci_evt_code) {
    case HCI_INQUIRY_COMP_EVT:
        btu_hcif_inquiry_comp_evt (p);
        break;
    case HCI_INQUIRY_RESULT_EVT:
        btu_hcif_inquiry_result_evt (p);
        break;
    case HCI_INQUIRY_RSSI_RESULT_EVT:
        btu_hcif_inquiry_rssi_result_evt (p);
        break;
    case HCI_EXTENDED_INQUIRY_RESULT_EVT:
        btu_hcif_extended_inquiry_result_evt (p);
        break;
    case HCI_CONNECTION_COMP_EVT:
        btu_hcif_connection_comp_evt (p);
        break;
    case HCI_CONNECTION_REQUEST_EVT:
        btu_hcif_connection_request_evt (p);
        break;
    case HCI_DISCONNECTION_COMP_EVT:
        btu_hcif_disconnection_comp_evt (p);
        break;
    case HCI_AUTHENTICATION_COMP_EVT:
#if (SMP_INCLUDED == TRUE)
        btu_hcif_authentication_comp_evt (p);
#endif  ///SMP_INCLUDED == TRUE
        break;
    case HCI_RMT_NAME_REQUEST_COMP_EVT:
        btu_hcif_rmt_name_request_comp_evt (p, hci_evt_len);
        break;
    case HCI_ENCRYPTION_CHANGE_EVT:
#if (SMP_INCLUDED == TRUE)
        btu_hcif_encryption_change_evt (p);
#endif  ///SMP_INCLUDED == TRUE
        break;
#if BLE_INCLUDED == TRUE
    case HCI_ENCRYPTION_KEY_REFRESH_COMP_EVT:
#if (SMP_INCLUDED == TRUE)
        btu_hcif_encryption_key_refresh_cmpl_evt(p);
#endif  ///SMP_INCLUDED == TRUE
        break;
#endif
    case HCI_READ_RMT_FEATURES_COMP_EVT:
        btu_hcif_read_rmt_features_comp_evt (p);
        break;
    case HCI_READ_RMT_EXT_FEATURES_COMP_EVT:
        btu_hcif_read_rmt_ext_features_comp_evt (p);
        break;
    case HCI_READ_RMT_VERSION_COMP_EVT:
        btu_hcif_read_rmt_version_comp_evt (p);
        break;
    case HCI_QOS_SETUP_COMP_EVT:
        btu_hcif_qos_setup_comp_evt (p);
        break;
    case HCI_COMMAND_COMPLETE_EVT:
        //HCI_TRACE_ERROR("%s should not have received a command complete event. "
        //      "Someone didn't go through the hci transmit_command function.", __func__);
        break;
    case HCI_COMMAND_STATUS_EVT:
        //HCI_TRACE_ERROR("%s should not have received a command status event. "
        //      "Someone didn't go through the hci transmit_command function.", __func__);
        break;
    case HCI_HARDWARE_ERROR_EVT:
        btu_hcif_hardware_error_evt (p);
        break;
    case HCI_FLUSH_OCCURED_EVT:
        btu_hcif_flush_occured_evt ();
        break;
    case HCI_ROLE_CHANGE_EVT:
        btu_hcif_role_change_evt (p);
        break;
    case HCI_NUM_COMPL_DATA_PKTS_EVT:
        btu_hcif_num_compl_data_pkts_evt (p);
        break;
    case HCI_MODE_CHANGE_EVT:
        btu_hcif_mode_change_evt (p);
        break;
#if (SMP_INCLUDED == TRUE)
    case HCI_PIN_CODE_REQUEST_EVT:
        btu_hcif_pin_code_request_evt (p);
        break;
    case HCI_LINK_KEY_REQUEST_EVT:
        btu_hcif_link_key_request_evt (p);
        break;
    case HCI_LINK_KEY_NOTIFICATION_EVT:
        btu_hcif_link_key_notification_evt (p);
        break;
#endif  ///SMP_INCLUDED == TRUE
    case HCI_LOOPBACK_COMMAND_EVT:
        btu_hcif_loopback_command_evt ();
        break;
    case HCI_DATA_BUF_OVERFLOW_EVT:
        btu_hcif_data_buf_overflow_evt ();
        break;
    case HCI_MAX_SLOTS_CHANGED_EVT:
        btu_hcif_max_slots_changed_evt ();
        break;
    case HCI_READ_CLOCK_OFF_COMP_EVT:
        btu_hcif_read_clock_off_comp_evt (p);
        break;
    case HCI_CONN_PKT_TYPE_CHANGE_EVT:
        btu_hcif_conn_pkt_type_change_evt (p);
        break;
    case HCI_QOS_VIOLATION_EVT:
        btu_hcif_qos_violation_evt (p);
        break;
    case HCI_PAGE_SCAN_MODE_CHANGE_EVT:
        btu_hcif_page_scan_mode_change_evt ();
        break;
    case HCI_PAGE_SCAN_REP_MODE_CHNG_EVT:
        btu_hcif_page_scan_rep_mode_chng_evt ();
        break;
    case HCI_ESCO_CONNECTION_COMP_EVT:
        btu_hcif_esco_connection_comp_evt (p);
        break;
    case HCI_ESCO_CONNECTION_CHANGED_EVT:
        btu_hcif_esco_connection_chg_evt (p);
        break;
    case HCI_SNIFF_SUB_RATE_EVT:
        btu_hcif_ssr_evt (p, hci_evt_len);
        break;
    case HCI_RMT_HOST_SUP_FEAT_NOTIFY_EVT:
        btu_hcif_host_support_evt (p);
        break;
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_IO_CAPABILITY_REQUEST_EVT:
        btu_hcif_io_cap_request_evt (p);
        break;
    case HCI_IO_CAPABILITY_RESPONSE_EVT:
        btu_hcif_io_cap_response_evt (p);
        break;
    case HCI_USER_CONFIRMATION_REQUEST_EVT:
        btu_hcif_user_conf_request_evt (p);
        break;
    case HCI_USER_PASSKEY_REQUEST_EVT:
        btu_hcif_user_passkey_request_evt (p);
        break;
#endif /* (CLASSIC_BT_INCLUDED == TRUE) */
#if BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE
    case HCI_REMOTE_OOB_DATA_REQUEST_EVT:
        btu_hcif_rem_oob_request_evt (p);
        break;
#endif
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_SIMPLE_PAIRING_COMPLETE_EVT:
        btu_hcif_simple_pair_complete_evt (p);
        break;
    case HCI_USER_PASSKEY_NOTIFY_EVT:
        btu_hcif_user_passkey_notif_evt (p);
        break;
    case HCI_KEYPRESS_NOTIFY_EVT:
        btu_hcif_keypress_notif_evt (p);
        break;
#endif /* (CLASSIC_BT_INCLUDED == TRUE) */
    case HCI_LINK_SUPER_TOUT_CHANGED_EVT:
        btu_hcif_link_supv_to_changed_evt (p);
        break;
#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
    case HCI_ENHANCED_FLUSH_COMPLETE_EVT:
        btu_hcif_enhanced_flush_complete_evt ();
        break;
#endif

#if (BLE_INCLUDED == TRUE)
    case HCI_BLE_EVENT:
        STREAM_TO_UINT8  (ble_sub_code, p);
        hci_evt_len--;
        HCI_TRACE_DEBUG("BLE HCI(id=%d) event = 0x%02x)", hci_evt_code,  ble_sub_code);

        switch (ble_sub_code) {
        case HCI_BLE_ADV_PKT_RPT_EVT: /* result of inquiry */
        case HCI_BLE_ADV_DISCARD_REPORT_EVT:
        case HCI_BLE_DIRECT_ADV_EVT:
            // These three events are directed to another specialized processing path
            HCI_TRACE_ERROR("Unexpected HCI BLE event = 0x%02x", ble_sub_code);
            break;
        case HCI_BLE_CONN_COMPLETE_EVT:
            btu_ble_ll_conn_complete_evt(p, hci_evt_len);
            break;
        case HCI_BLE_LL_CONN_PARAM_UPD_EVT:
            btu_ble_ll_conn_param_upd_evt(p, hci_evt_len);
            break;
        case HCI_BLE_READ_REMOTE_FEAT_CMPL_EVT:
            btu_ble_read_remote_feat_evt(p);
            break;
        case HCI_BLE_LTK_REQ_EVT: /* received only at slave device */
#if (SMP_INCLUDED == TRUE)
            btu_ble_proc_ltk_req(p);
#endif  ///SMP_INCLUDED == TRUE
            break;
//#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
        case HCI_BLE_ENHANCED_CONN_COMPLETE_EVT:
            btu_ble_proc_enhanced_conn_cmpl(p, hci_evt_len);
            break;
//#endif
#if (BLE_LLT_INCLUDED == TRUE)
        case HCI_BLE_RC_PARAM_REQ_EVT:
            btu_ble_rc_param_req_evt(p);
            break;
#endif
        case HCI_BLE_DATA_LENGTH_CHANGE_EVT:
            btu_ble_data_length_change_evt(p, hci_evt_len);
            break;
#if (BLE_50_FEATURE_SUPPORT == TRUE)
        case HCI_BLE_PHY_UPDATE_COMPLETE_EVT:
            btu_ble_phy_update_complete_evt(p);
            break;
        case HCI_BLE_EXT_ADV_REPORT_EVT:
            //HCI_TRACE_ERROR("%s, HCI_BLE_EXT_ADV_REPORT_EVT.", __func__);
            btu_ble_ext_adv_report_evt(p, hci_evt_len);
            break;
        case HCI_BLE_PERIOD_ADV_SYNC_ESTAB_EVT:
            btu_ble_periodic_adv_sync_establish_evt(p);
            break;
        case HCI_BLE_PERIOD_ADV_REPORT_EVT:
            btu_ble_periodic_adv_report_evt(p, hci_evt_len);
            break;
        case HCI_BLE_PERIOD_ADV_SYNC_LOST_EVT:
            btu_ble_periodic_adv_sync_lost_evt(p);
            break;
        case HCI_BLE_SCAN_TIMEOUT_EVT:
            btu_ble_scan_timeout_evt(p);
            break;
        case HCI_BLE_ADV_SET_TERMINATED_EVT:
            btu_ble_adv_set_terminate_evt(p);
            break;
        case HCI_BLE_SCAN_REQ_RECEIVED_EVT:
            btu_ble_scan_req_received_evt(p);
            break;
        case HCI_BLE_CHANNEL_SELECT_ALG:
            break;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
        case HCI_BLE_PERIOD_ADV_SYNC_TRANS_RECV_EVT:
            btu_ble_periodic_adv_sync_trans_recv(p);
            break;
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
        }
        break;
#endif /* BLE_INCLUDED */
    case HCI_VENDOR_SPECIFIC_EVT:
        btm_vendor_specific_evt (p, hci_evt_len);
        break;
    }
}


/*******************************************************************************
**
** Function         btu_hcif_send_cmd
**
** Description      This function is called to send commands to the Host Controller.
**
** Returns          void
**
*******************************************************************************/
void btu_hcif_send_cmd (UNUSED_ATTR UINT8 controller_id, BT_HDR *p_buf)
{
    if (!p_buf) {
        HCI_TRACE_ERROR("%s p_buf is NULL", __func__);
        return;
    }

    uint16_t opcode;
    uint8_t *stream = p_buf->data + p_buf->offset;
    void *vsc_callback = NULL;

    STREAM_TO_UINT16(opcode, stream);

    assert (p_buf->layer_specific == HCI_CMD_BUF_TYPE_METADATA);
    hci_cmd_metadata_t *metadata = HCI_GET_CMD_METAMSG(p_buf);
    metadata->command_complete_cb = btu_hcif_command_complete_evt;
    metadata->command_status_cb = btu_hcif_command_status_evt;
    metadata->opcode = opcode;

    vsc_callback = metadata->context;
    /* If command is not a VSC, then the context field should be empty */
    if ((opcode & HCI_GRP_VENDOR_SPECIFIC) != HCI_GRP_VENDOR_SPECIFIC
#if BLE_INCLUDED == TRUE
            && (opcode != HCI_BLE_RAND)
            && (opcode != HCI_BLE_ENCRYPT)
#endif
        ) {
        assert (vsc_callback == NULL);
    }

    hci_layer_get_interface()->transmit_command(
        p_buf,
        btu_hcif_command_complete_evt,
        btu_hcif_command_status_evt,
        vsc_callback);

#if (defined(HCILP_INCLUDED) && HCILP_INCLUDED == TRUE)
    btu_check_bt_sleep ();
#endif
}

#if (BLE_50_FEATURE_SUPPORT == TRUE)
UINT8 btu_hcif_send_cmd_sync (UINT8 controller_id, BT_HDR *p_buf)
{
    if (!p_buf) {
        HCI_TRACE_ERROR("%s p_buf is NULL", __func__);
        return HCI_ERR_ILLEGAL_PARAMETER_FMT;
    }
    BlE_SYNC *sync_info =  btsnd_hcic_ble_get_sync_info();
    if((sync_info ==  NULL) || (sync_info->sync_sem == NULL)) {
        HCI_TRACE_ERROR("%s sync_info error", __func__);
        return HCI_ERR_ILLEGAL_PARAMETER_FMT;
    }
    uint16_t opcode;
    uint8_t *stream = p_buf->data + p_buf->offset;
    void *vsc_callback = NULL;

    STREAM_TO_UINT16(opcode, stream);

    sync_info->opcode = opcode;

    assert (p_buf->layer_specific == HCI_CMD_BUF_TYPE_METADATA);
    hci_cmd_metadata_t *metadata = HCI_GET_CMD_METAMSG(p_buf);
    metadata->command_complete_cb = btu_hcif_command_complete_evt;
    metadata->command_status_cb = btu_hcif_command_status_evt;
    metadata->command_free_cb = NULL;
    metadata->opcode = opcode;

    vsc_callback = metadata->context;
    /* If command is not a VSC, then the context field should be empty */
    if ((opcode & HCI_GRP_VENDOR_SPECIFIC) != HCI_GRP_VENDOR_SPECIFIC
#if BLE_INCLUDED == TRUE
            && (opcode != HCI_BLE_RAND)
            && (opcode != HCI_BLE_ENCRYPT)
#endif
        ) {
        assert (vsc_callback == NULL);
    }

    hci_layer_get_interface()->transmit_command(
        p_buf,
        btu_hcif_command_complete_evt,
        btu_hcif_command_status_evt,
        vsc_callback);

    osi_sem_take(&sync_info->sync_sem, OSI_SEM_MAX_TIMEOUT);

#if (defined(HCILP_INCLUDED) && HCILP_INCLUDED == TRUE)
    btu_check_bt_sleep ();
#endif
    return btsnd_hcic_ble_get_status();
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)


/*******************************************************************************
**
** Function         btu_hcif_send_host_rdy_for_data
**
** Description      This function is called to check if it can send commands
**                  to the Host Controller. It may be passed the address of
**                  a packet to send.
**
** Returns          void
**
*******************************************************************************/
void btu_hcif_send_host_rdy_for_data(void)
{
    UINT16      num_pkts[MAX_L2CAP_LINKS + 4];      /* 3 SCO connections */
    UINT16      handles[MAX_L2CAP_LINKS + 4];
    UINT8       num_ents;

    /* Get the L2CAP numbers */
    num_ents = l2c_link_pkts_rcvd (num_pkts, handles);

    /* Get the SCO numbers */
    /* No SCO for now ?? */

    if (num_ents) {
        btsnd_hcic_host_num_xmitted_pkts (num_ents, handles, num_pkts);
    }
}

/*******************************************************************************
**
** Function         btu_hcif_inquiry_comp_evt
**
** Description      Process event HCI_INQUIRY_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_inquiry_comp_evt (UINT8 *p)
{
    UINT8   status;

    STREAM_TO_UINT8    (status, p);

    /* Tell inquiry processing that we are done */
    btm_process_inq_complete(status, BTM_BR_INQUIRY_MASK);
}


/*******************************************************************************
**
** Function         btu_hcif_inquiry_result_evt
**
** Description      Process event HCI_INQUIRY_RESULT_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_inquiry_result_evt (UINT8 *p)
{
    /* Store results in the cache */
    btm_process_inq_results (p, BTM_INQ_RESULT_STANDARD);
}

/*******************************************************************************
**
** Function         btu_hcif_inquiry_rssi_result_evt
**
** Description      Process event HCI_INQUIRY_RSSI_RESULT_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_inquiry_rssi_result_evt (UINT8 *p)
{
    /* Store results in the cache */
    btm_process_inq_results (p, BTM_INQ_RESULT_WITH_RSSI);
}

/*******************************************************************************
**
** Function         btu_hcif_extended_inquiry_result_evt
**
** Description      Process event HCI_EXTENDED_INQUIRY_RESULT_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_extended_inquiry_result_evt (UINT8 *p)
{
    /* Store results in the cache */
    btm_process_inq_results (p, BTM_INQ_RESULT_EXTENDED);
}

/*******************************************************************************
**
** Function         btu_hcif_connection_comp_evt
**
** Description      Process event HCI_CONNECTION_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_connection_comp_evt (UINT8 *p)
{
    UINT8       status;
    UINT16      handle;
    BD_ADDR     bda;
    UINT8       link_type;
    UINT8       enc_mode;

    STREAM_TO_UINT8    (status, p);
    STREAM_TO_UINT16   (handle, p);
    STREAM_TO_BDADDR   (bda, p);
    STREAM_TO_UINT8    (link_type, p);
    STREAM_TO_UINT8    (enc_mode, p);

    handle = HCID_GET_HANDLE (handle);

    btm_acl_connected(bda, handle, link_type, enc_mode, status);

    HCI_TRACE_WARNING("hcif conn complete: hdl 0x%x, st 0x%x", handle, status);
}


/*******************************************************************************
**
** Function         btu_hcif_connection_request_evt
**
** Description      Process event HCI_CONNECTION_REQUEST_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_connection_request_evt (UINT8 *p)
{
    BD_ADDR     bda;
    DEV_CLASS   dc;
    UINT8       link_type;

    STREAM_TO_BDADDR   (bda, p);
    STREAM_TO_DEVCLASS (dc, p);
    STREAM_TO_UINT8    (link_type, p);
    /* Pass request to security manager to check connect filters before */
    /* passing request to l2cap */
    if (link_type == HCI_LINK_TYPE_ACL) {
#if (SMP_INCLUDED == TRUE)
        btm_sec_conn_req (bda, dc);
#endif  ///SMP_INCLUDED == TRUE
    }
#if BTM_SCO_INCLUDED == TRUE
    else {
        btm_sco_conn_req (bda, dc, link_type);
    }
#endif /* BTM_SCO_INCLUDED */
}


/*******************************************************************************
**
** Function         btu_hcif_disconnection_comp_evt
**
** Description      Process event HCI_DISCONNECTION_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_disconnection_comp_evt (UINT8 *p)
{
    UINT16  handle;
    UINT8   reason;

    ++p;
    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT8  (reason, p);

    handle = HCID_GET_HANDLE (handle);

    btm_acl_disconnected(handle, reason);

    HCI_TRACE_WARNING("hcif disc complete: hdl 0x%x, rsn 0x%x", handle, reason);
}

/*******************************************************************************
**
** Function         btu_hcif_authentication_comp_evt
**
** Description      Process event HCI_AUTHENTICATION_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_authentication_comp_evt (UINT8 *p)
{
    UINT8   status;
    UINT16  handle;

    STREAM_TO_UINT8  (status, p);
    STREAM_TO_UINT16 (handle, p);

    btm_sec_auth_complete (handle, status);
}
#endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         btu_hcif_rmt_name_request_comp_evt
**
** Description      Process event HCI_RMT_NAME_REQUEST_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_rmt_name_request_comp_evt (UINT8 *p, UINT16 evt_len)
{
    UINT8   status;
    BD_ADDR bd_addr;

    STREAM_TO_UINT8 (status, p);
    STREAM_TO_BDADDR (bd_addr, p);

    evt_len -= (1 + BD_ADDR_LEN);

    btm_process_remote_name (bd_addr, p, evt_len, status);
#if (SMP_INCLUDED == TRUE)
    btm_sec_rmt_name_request_complete (bd_addr, p, status);
#endif  ///SMP_INCLUDED == TRUE
}


/*******************************************************************************
**
** Function         btu_hcif_encryption_change_evt
**
** Description      Process event HCI_ENCRYPTION_CHANGE_EVT
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_encryption_change_evt (UINT8 *p)
{
    UINT8   status;
    UINT16  handle;
    UINT8   encr_enable;

    STREAM_TO_UINT8  (status, p);
    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT8  (encr_enable, p);

    btm_acl_encrypt_change (handle, status, encr_enable);
    btm_sec_encrypt_change (handle, status, encr_enable);
}
#endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         btu_hcif_read_rmt_features_comp_evt
**
** Description      Process event HCI_READ_RMT_FEATURES_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_read_rmt_features_comp_evt (UINT8 *p)
{
    btm_read_remote_features_complete(p);
}

/*******************************************************************************
**
** Function         btu_hcif_read_rmt_ext_features_comp_evt
**
** Description      Process event HCI_READ_RMT_EXT_FEATURES_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_read_rmt_ext_features_comp_evt (UINT8 *p)
{
    UINT8 *p_cur = p;
    UINT8 status;
    UINT16 handle;

    STREAM_TO_UINT8 (status, p_cur);

    if (status == HCI_SUCCESS) {
        btm_read_remote_ext_features_complete(p);
    } else {
        STREAM_TO_UINT16 (handle, p_cur);
        btm_read_remote_ext_features_failed(status, handle);
    }
}

/*******************************************************************************
**
** Function         btu_hcif_read_rmt_version_comp_evt
**
** Description      Process event HCI_READ_RMT_VERSION_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_read_rmt_version_comp_evt (UINT8 *p)
{
    btm_read_remote_version_complete (p);
}


/*******************************************************************************
**
** Function         btu_hcif_qos_setup_comp_evt
**
** Description      Process event HCI_QOS_SETUP_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_qos_setup_comp_evt (UINT8 *p)
{
    UINT8 status;
    UINT16 handle;
    FLOW_SPEC flow;

    STREAM_TO_UINT8 (status, p);
    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT8 (flow.qos_flags, p);
    STREAM_TO_UINT8 (flow.service_type, p);
    STREAM_TO_UINT32 (flow.token_rate, p);
    STREAM_TO_UINT32 (flow.peak_bandwidth, p);
    STREAM_TO_UINT32 (flow.latency, p);
    STREAM_TO_UINT32 (flow.delay_variation, p);

    btm_qos_setup_complete(status, handle, &flow);
}


/*******************************************************************************
**
** Function         btu_hcif_esco_connection_comp_evt
**
** Description      Process event HCI_ESCO_CONNECTION_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_esco_connection_comp_evt (UINT8 *p)
{
#if BTM_SCO_INCLUDED == TRUE
    tBTM_ESCO_DATA  data;
    UINT16          handle;
    BD_ADDR         bda;
    UINT8           status;

    STREAM_TO_UINT8 (status, p);
    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_BDADDR (bda, p);

    STREAM_TO_UINT8 (data.link_type, p);
    STREAM_TO_UINT8 (data.tx_interval, p);
    STREAM_TO_UINT8 (data.retrans_window, p);
    STREAM_TO_UINT16 (data.rx_pkt_len, p);
    STREAM_TO_UINT16 (data.tx_pkt_len, p);
    STREAM_TO_UINT8 (data.air_mode, p);

    memcpy (data.bd_addr, bda, BD_ADDR_LEN);
    btm_sco_connected (status, bda, handle, &data);
#endif
}


/*******************************************************************************
**
** Function         btu_hcif_esco_connection_chg_evt
**
** Description      Process event HCI_ESCO_CONNECTION_CHANGED_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_esco_connection_chg_evt (UINT8 *p)
{
#if BTM_SCO_INCLUDED == TRUE
    UINT16  handle;
    UINT16  tx_pkt_len;
    UINT16  rx_pkt_len;
    UINT8   status;
    UINT8   tx_interval;
    UINT8   retrans_window;

    STREAM_TO_UINT8 (status, p);
    STREAM_TO_UINT16 (handle, p);

    STREAM_TO_UINT8 (tx_interval, p);
    STREAM_TO_UINT8 (retrans_window, p);
    STREAM_TO_UINT16 (rx_pkt_len, p);
    STREAM_TO_UINT16 (tx_pkt_len, p);

    btm_esco_proc_conn_chg (status, handle, tx_interval, retrans_window,
                            rx_pkt_len, tx_pkt_len);
#endif
}

/*******************************************************************************
**
** Function         btu_hcif_hdl_command_complete
**
** Description      Handle command complete event
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_hdl_command_complete (UINT16 opcode, UINT8 *p, UINT16 evt_len,
        void *p_cplt_cback)
{
    uint8_t status;
    switch (opcode) {
    case HCI_INQUIRY_CANCEL:
        /* Tell inquiry processing that we are done */
        btm_process_cancel_complete(HCI_SUCCESS, BTM_BR_INQUIRY_MASK);
        break;
    case HCI_SET_EVENT_FILTER:
        btm_event_filter_complete (p);
        break;

    case HCI_DELETE_STORED_LINK_KEY:
        btm_delete_stored_link_key_complete (p);
        break;

    case HCI_READ_LOCAL_NAME:
        btm_read_local_name_complete (p, evt_len);
        break;

    case HCI_GET_LINK_QUALITY:
        btm_read_link_quality_complete (p);
        break;

    case HCI_READ_RSSI:
        btm_read_rssi_complete (p);
        break;

    case HCI_READ_TRANSMIT_POWER_LEVEL:
        btm_read_tx_power_complete(p, FALSE);
        break;

    case HCI_CREATE_CONNECTION_CANCEL:
        btm_create_conn_cancel_complete(p);
        break;

    case HCI_READ_LOCAL_OOB_DATA:
#if BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE
        btm_read_local_oob_complete(p);
#endif
        break;


    case HCI_READ_INQ_TX_POWER_LEVEL:
        btm_read_linq_tx_power_complete (p);
        break;
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_SET_AFH_CHANNELS:
        btm_set_afh_channels_complete(p);
        break;
    case HCI_WRITE_PAGE_TOUT:
        btm_set_page_timeout_complete(p);
        break;
#if (ENC_KEY_SIZE_CTRL_MODE == ENC_KEY_SIZE_CTRL_MODE_STD)
    case HCI_SET_MIN_ENC_KEY_SIZE:
        btm_set_min_enc_key_size_complete(p);
        break;
#endif
#if (ENC_KEY_SIZE_CTRL_MODE == ENC_KEY_SIZE_CTRL_MODE_VSC)
    case HCI_VENDOR_BT_SET_MIN_ENC_KEY_SIZE:
        btm_set_min_enc_key_size_complete(p);
        break;
#endif
#endif

#if (BLE_INCLUDED == TRUE)
    /* BLE Commands Complete*/
    case HCI_BLE_SET_HOST_CHNL_CLASS:
        btm_ble_set_channels_complete(p);
        break;
    case HCI_BLE_ADD_WHITE_LIST:
        btm_ble_add_2_white_list_complete(*p);
        break;

    case HCI_BLE_CLEAR_WHITE_LIST:
        btm_ble_clear_white_list_complete(p, evt_len);
        break;
    case HCI_BLE_WRITE_ADV_PARAMS: {
        STREAM_TO_UINT8  (status, p);
        if(status != HCI_SUCCESS) {
            HCI_TRACE_ERROR("hci write adv params error 0x%x", status);
        }
        break;
    }
    case HCI_BLE_RC_PARAM_REQ_REPLY: {
        STREAM_TO_UINT8  (status, p);
        if(status != HCI_SUCCESS) {
            HCI_TRACE_ERROR("hci connection params reply command error 0x%x", status);
        }
        break;
    }
    case HCI_BLE_RC_PARAM_REQ_NEG_REPLY: {
        STREAM_TO_UINT8  (status, p);
        if(status != HCI_SUCCESS) {
            HCI_TRACE_ERROR("hci connection params neg reply command error %x", status);
        }
        break;
    }
    case HCI_BLE_REMOVE_WHITE_LIST:
        btm_ble_remove_from_white_list_complete(p, evt_len);
        break;

    case HCI_BLE_RAND:
    case HCI_BLE_ENCRYPT:
#if (SMP_INCLUDED == TRUE)
        btm_ble_rand_enc_complete (p, opcode, (tBTM_RAND_ENC_CB *)p_cplt_cback);
#endif  ///SMP_INCLUDED == TRUE
        break;

    case HCI_BLE_READ_ADV_CHNL_TX_POWER:
        btm_read_tx_power_complete(p, TRUE);
        break;

    case HCI_BLE_WRITE_ADV_ENABLE:
        btm_ble_write_adv_enable_complete(p);
        break;

    case HCI_BLE_CREATE_LL_CONN:
        btm_ble_create_ll_conn_complete(*p);
        break;

    case HCI_BLE_TRANSMITTER_TEST:
    case HCI_BLE_RECEIVER_TEST:
    case HCI_BLE_TEST_END:
        btm_ble_test_command_complete(p);
        break;
    case HCI_BLE_CREATE_CONN_CANCEL:
        btm_ble_create_conn_cancel_complete(p);
        break;

#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    case HCI_BLE_ADD_DEV_RESOLVING_LIST:
        btm_ble_add_resolving_list_entry_complete(p, evt_len);
        break;

    case HCI_BLE_RM_DEV_RESOLVING_LIST:
        btm_ble_remove_resolving_list_entry_complete(p, evt_len);
        break;

    case HCI_BLE_CLEAR_RESOLVING_LIST:
        btm_ble_clear_resolving_list_complete(p, evt_len);
        break;

    case HCI_BLE_READ_RESOLVABLE_ADDR_PEER:
        btm_ble_read_resolving_list_entry_complete(p, evt_len);
        break;

    case HCI_BLE_READ_RESOLVABLE_ADDR_LOCAL:
        break;
    case HCI_BLE_SET_ADDR_RESOLUTION_ENABLE:
        btm_ble_set_addr_resolution_enable_complete(p, evt_len);
        break;
    case HCI_BLE_SET_RAND_PRIV_ADDR_TIMOUT:
        btm_ble_set_rpa_timeout_complete(p, evt_len);
        break;
    case HCI_BLE_SET_PRIVACY_MODE:
        btm_ble_set_privacy_mode_complete(p, evt_len);
        break;
#endif // #if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    case HCI_BLE_SET_EXT_ADV_PARAM:
    case HCI_BLE_SET_EXT_ADV_DATA:
    case HCI_BLE_SET_EXT_SCAN_RSP_DATA:
    case HCI_BLE_SET_EXT_ADV_ENABLE: {
        STREAM_TO_UINT8  (status, p);
        HCI_TRACE_EVENT("%s opcode 0x%x status 0x%x", __func__, opcode, status);
	    break;
    }
    case HCI_BLE_READ_PHY: {
        uint16_t conn_handle;
        uint8_t tx_phy;
        uint8_t rx_phy;
        STREAM_TO_UINT8(status, p);
        STREAM_TO_UINT16(conn_handle, p);
        STREAM_TO_UINT8(tx_phy, p);
        STREAM_TO_UINT8(rx_phy, p);
        btm_read_phy_callback(status, conn_handle, tx_phy, rx_phy);
        break;
    }
    case HCI_BLE_ENH_RX_TEST:
    case HCI_BLE_ENH_TX_TEST:
        btm_ble_test_command_complete(p);
        break;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    case HCI_BLE_SET_PERIOD_ADV_RECV_ENABLE:
    case HCI_BLE_SET_DEFAULT_PAST_PARAMS:
        break;
    case HCI_BLE_PERIOD_ADV_SYNC_TRANS:
    case HCI_BLE_PERIOD_ADV_SET_INFO_TRANS:
    case HCI_BLE_SET_PAST_PARAMS: {
        UINT16 conn_handle;
        STREAM_TO_UINT8(status, p);
        STREAM_TO_UINT16(conn_handle, p);
        btm_ble_periodic_adv_sync_trans_complete(opcode, status, conn_handle);
        break;
    }
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#endif /* (BLE_INCLUDED == TRUE) */

    default: {
        if ((opcode & HCI_GRP_VENDOR_SPECIFIC) == HCI_GRP_VENDOR_SPECIFIC) {
            btm_vsc_complete (p, opcode, evt_len, (tBTM_CMPL_CB *)p_cplt_cback);
        }
        STREAM_TO_UINT8  (status, p);
        if(status != HCI_SUCCESS) {
            HCI_TRACE_ERROR("CC evt: op=0x%x, status=0x%x", opcode, status);
        }
        break;
    }
    }
}

/*******************************************************************************
**
** Function         btu_hcif_command_complete_evt
**
** Description      Process event HCI_COMMAND_COMPLETE_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_command_complete_evt_on_task(BT_HDR *event)
{
    command_complete_hack_t *hack = (command_complete_hack_t *)&event->data[0];

    command_opcode_t opcode;
    uint8_t *stream = hack->response->data + hack->response->offset + 3; // 2 to skip the event headers, 1 to skip the command credits
    STREAM_TO_UINT16(opcode, stream);

    btu_hcif_hdl_command_complete(
        opcode,
        stream,
        hack->response->len - 5, // 3 for the command complete headers, 2 for the event headers
        hack->context);

    osi_free(hack->response);
    osi_free(event);
}

static void btu_hcif_command_complete_evt(BT_HDR *response, void *context)
{
#if (BLE_INCLUDED == TRUE)
    command_opcode_t opcode;
    uint8_t *stream = response->data + response->offset + 3;
    STREAM_TO_UINT16(opcode, stream);
    switch (opcode) {
        case HCI_BLE_WRITE_ADV_DATA:
            adv_data_status = *stream;
            osi_sem_give(&adv_data_sem);
            break;
        case HCI_BLE_WRITE_SCAN_RSP_DATA:
            adv_data_status = *stream;
            osi_sem_give(&adv_data_sem);
            break;
        case HCI_BLE_WRITE_ADV_ENABLE: {
            adv_enable_status = *stream;
            osi_sem_give(&adv_enable_sem);
            break;
        }
        case HCI_BLE_WRITE_ADV_PARAMS:
            adv_param_status = *stream;
            osi_sem_give(&adv_param_sem);
            break;
        case HCI_BLE_WRITE_SCAN_PARAMS:
            scan_param_status = *stream;
            osi_sem_give(&scan_param_sem);
            break;
        case HCI_BLE_WRITE_SCAN_ENABLE:
            scan_enable_status = *stream;
            osi_sem_give(&scan_enable_sem);
            break;
        default:
            break;
    }
#endif
    BT_HDR *event = osi_calloc(sizeof(BT_HDR) + sizeof(command_complete_hack_t));
    command_complete_hack_t *hack = (command_complete_hack_t *)&event->data[0];
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    UINT8 status = 0;
    stream = response->data + response->offset + 3 + 2; // 2 to skip the event headers, 1 to skip the command credits, 2 to opcode.
    STREAM_TO_UINT8(status, stream);
    btsnd_hci_ble_set_status(status);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
    HCI_TRACE_DEBUG("btu_hcif_command_complete_evt\n");

    hack->callback = btu_hcif_command_complete_evt_on_task;
    hack->response = response;
    hack->context = context;

    event->event = BTU_POST_TO_TASK_NO_GOOD_HORRIBLE_HACK;

    if (btu_task_post(SIG_BTU_HCI_MSG, event, OSI_THREAD_MAX_TIMEOUT) == false) {
        osi_free(event);
    }
}


/*******************************************************************************
**
** Function         btu_hcif_hdl_command_status
**
** Description      Handle a command status event
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_hdl_command_status (UINT16 opcode, UINT8 status, UINT8 *p_cmd,
        void *p_vsc_status_cback)
{
    if (status != HCI_SUCCESS){
        HCI_TRACE_WARNING("%s,opcode:0x%04x,status:0x%02x", __func__, opcode,status);
    }
    BD_ADDR         bd_addr;
    UINT16          handle;
#if BTM_SCO_INCLUDED == TRUE
    tBTM_ESCO_DATA  esco_data;
#endif

    switch (opcode) {
    case HCI_EXIT_SNIFF_MODE:
    case HCI_EXIT_PARK_MODE:
#if BTM_SCO_WAKE_PARKED_LINK == TRUE
        if (status != HCI_SUCCESS) {
            /* Allow SCO initiation to continue if waiting for change mode event */
            if (p_cmd != NULL) {
                p_cmd++;    /* bypass length field */
                STREAM_TO_UINT16 (handle, p_cmd);
                btm_sco_chk_pend_unpark (status, handle);
            }
        }
#endif
    /* Case Falls Through */

    case HCI_HOLD_MODE:
    case HCI_SNIFF_MODE:
    case HCI_PARK_MODE:
        btm_pm_proc_cmd_status(status);
        break;
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    case HCI_BLE_PERIOD_ADV_CREATE_SYNC:
    {
        uint8_t btm_status = BTM_SUCCESS;
        if(status != HCI_SUCCESS) {
            HCI_TRACE_ERROR("CS evt: LE PA CreateSync status=0x%x", status);
            btm_status = BTM_ILLEGAL_VALUE;
        }
        btm_create_sync_callback(btm_status);
        break;
    }
    case HCI_BLE_SET_PHY:
    {
        uint8_t btm_status = BTM_SUCCESS;
        if(status != HCI_SUCCESS) {
            HCI_TRACE_ERROR("CS evt: LE SetPhy status=0x%x", status);
            btm_status = BTM_ILLEGAL_VALUE;
        }
        btm_set_phy_callback(btm_status);
        break;
    }
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
    default:
        /* If command failed to start, we may need to tell BTM */
        if (status != HCI_SUCCESS) {
            switch (opcode) {
            case HCI_INQUIRY:
                /* Tell inquiry processing that we are done */
                btm_process_inq_complete(status, BTM_BR_INQUIRY_MASK);
                break;

            case HCI_RMT_NAME_REQUEST:
                /* Tell inquiry processing that we are done */
                btm_process_remote_name (NULL, NULL, 0, status);
#if (SMP_INCLUDED == TRUE)
                btm_sec_rmt_name_request_complete (NULL, NULL, status);
#endif  ///SMP_INCLUDED == TRUE
                break;

            case HCI_QOS_SETUP_COMP_EVT:
                /* Tell qos setup that we are done */
                btm_qos_setup_complete(status, 0, NULL);
                break;

            case HCI_SWITCH_ROLE:
                /* Tell BTM that the command failed */
                /* read bd addr out of stored command */
                if (p_cmd != NULL) {
                    p_cmd++;
                    STREAM_TO_BDADDR (bd_addr, p_cmd);
                    btm_acl_role_changed(status, bd_addr, BTM_ROLE_UNDEFINED);
                } else {
                    btm_acl_role_changed(status, NULL, BTM_ROLE_UNDEFINED);
                }
                l2c_link_role_changed (NULL, BTM_ROLE_UNDEFINED, HCI_ERR_COMMAND_DISALLOWED);
                break;

            case HCI_CREATE_CONNECTION:
                /* read bd addr out of stored command */
                if (p_cmd != NULL) {
                    p_cmd++;
                    STREAM_TO_BDADDR (bd_addr, p_cmd);
#if (SMP_INCLUDED == TRUE)
                    btm_sec_connected (bd_addr, HCI_INVALID_HANDLE, status, 0);
#endif  ///SMP_INCLUDED == TRUE
                    l2c_link_hci_conn_comp (status, HCI_INVALID_HANDLE, bd_addr);
                }
                break;

            case HCI_READ_RMT_EXT_FEATURES:
                if (p_cmd != NULL) {
                    p_cmd++; /* skip command length */
                    STREAM_TO_UINT16 (handle, p_cmd);
                } else {
                    handle = HCI_INVALID_HANDLE;
                }

                btm_read_remote_ext_features_failed(status, handle);
                break;

            case HCI_AUTHENTICATION_REQUESTED:
#if (SMP_INCLUDED == TRUE)
                /* Device refused to start authentication.  That should be treated as authentication failure. */
                btm_sec_auth_complete (BTM_INVALID_HCI_HANDLE, status);
#endif  ///SMP_INCLUDED == TRUE
                break;

            case HCI_SET_CONN_ENCRYPTION:
#if (SMP_INCLUDED == TRUE)
                /* Device refused to start encryption.  That should be treated as encryption failure. */
                btm_sec_encrypt_change (BTM_INVALID_HCI_HANDLE, status, FALSE);
#endif  ///SMP_INCLUDED == TRUE
                break;

#if BLE_INCLUDED == TRUE
#if (BLE_50_FEATURE_SUPPORT == TRUE)
            case HCI_BLE_EXT_CREATE_CONN:
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
            case HCI_BLE_CREATE_LL_CONN:
                btm_ble_create_ll_conn_complete(status);
                break;
            case HCI_BLE_UPD_LL_CONN_PARAMS:
                if (p_cmd != NULL){
                    p_cmd++;
                    STREAM_TO_UINT16 (handle, p_cmd);
                    btu_ble_ll_get_conn_param_format_err_from_contoller(status, handle);
                }
                break;
#endif

#if BTM_SCO_INCLUDED == TRUE
            case HCI_SETUP_ESCO_CONNECTION:
                /* read handle out of stored command */
                if (p_cmd != NULL) {
                    p_cmd++;
                    STREAM_TO_UINT16 (handle, p_cmd);

                    /* Determine if initial connection failed or is a change of setup */
                    if (btm_is_sco_active(handle)) {
                        btm_esco_proc_conn_chg (status, handle, 0, 0, 0, 0);
                    } else {
                        btm_sco_connected (status, NULL, handle, &esco_data);
                    }
                }
                break;
#endif

            /* This is commented out until an upper layer cares about returning event
            #if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
                        case HCI_ENHANCED_FLUSH:
                            break;
            #endif
            */
            default:
                if ((opcode & HCI_GRP_VENDOR_SPECIFIC) == HCI_GRP_VENDOR_SPECIFIC) {
                    btm_vsc_complete (&status, opcode, 1, (tBTM_CMPL_CB *)p_vsc_status_cback);
                }
                break;
            }

        } else {
            if ((opcode & HCI_GRP_VENDOR_SPECIFIC) == HCI_GRP_VENDOR_SPECIFIC) {
                btm_vsc_complete (&status, opcode, 1, (tBTM_CMPL_CB *)p_vsc_status_cback);
            }
        }
    }
}

/*******************************************************************************
**
** Function         btu_hcif_command_status_evt
**
** Description      Process event HCI_COMMAND_STATUS_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_command_status_evt_on_task(BT_HDR *event)
{
    command_status_hack_t *hack = (command_status_hack_t *)&event->data[0];

    command_opcode_t opcode;
    uint8_t *stream = hack->command->data + hack->command->offset;
    STREAM_TO_UINT16(opcode, stream);

    btu_hcif_hdl_command_status(
        opcode,
        hack->status,
        stream,
        hack->context);

    // check the HCI command integrity: opcode
    hci_cmd_metadata_t *metadata = HCI_GET_CMD_METAMSG(hack->command);
    assert(metadata->opcode == opcode);

    HCI_FREE_CMD_BUF(hack->command);
    osi_free(event);
}

static void btu_hcif_command_status_evt(uint8_t status, BT_HDR *command, void *context)
{
    BT_HDR *event = osi_calloc(sizeof(BT_HDR) + sizeof(command_status_hack_t));
    command_status_hack_t *hack = (command_status_hack_t *)&event->data[0];

    hack->callback = btu_hcif_command_status_evt_on_task;
    hack->status = status;
    hack->command = command;
    hack->context = context;

    event->event = BTU_POST_TO_TASK_NO_GOOD_HORRIBLE_HACK;
    if (btu_task_post(SIG_BTU_HCI_MSG, event, OSI_THREAD_MAX_TIMEOUT) == false) {
        osi_free(event);
    }
}

/*******************************************************************************
**
** Function         btu_hcif_hardware_error_evt
**
** Description      Process event HCI_HARDWARE_ERROR_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_hardware_error_evt (UINT8 *p)
{
    HCI_TRACE_ERROR("Ctlr H/w error event - code:0x%x\n", *p);

    /* If anyone wants device status notifications, give him one. */
    btm_report_device_status (BTM_DEV_STATUS_DOWN);

    /* Reset the controller */
    if (BTM_IsDeviceUp()) {
        BTM_DeviceReset (NULL);
    }
}


/*******************************************************************************
**
** Function         btu_hcif_flush_occured_evt
**
** Description      Process event HCI_FLUSH_OCCURED_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_flush_occured_evt (void)
{
}


/*******************************************************************************
**
** Function         btu_hcif_role_change_evt
**
** Description      Process event HCI_ROLE_CHANGE_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_role_change_evt (UINT8 *p)
{
    UINT8       status;
    BD_ADDR     bda;
    UINT8       role;

    STREAM_TO_UINT8 (status, p);
    STREAM_TO_BDADDR (bda, p);
    STREAM_TO_UINT8  (role, p);

    l2c_link_role_changed (bda, role, status);
    btm_acl_role_changed(status, bda, role);
}


/*******************************************************************************
**
** Function         btu_hcif_num_compl_data_pkts_evt
**
** Description      Process event HCI_NUM_COMPL_DATA_PKTS_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_num_compl_data_pkts_evt (UINT8 *p)
{
    /* Process for L2CAP and SCO */
    l2c_link_process_num_completed_pkts (p);

    /* Send on to SCO */
#if (BTM_SCO_HCI_INCLUDED == TRUE) && (BTM_SCO_INCLUDED == TRUE)
    btm_sco_process_num_completed_pkts (p);
#endif
}

/*******************************************************************************
**
** Function         btu_hcif_mode_change_evt
**
** Description      Process event HCI_MODE_CHANGE_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_mode_change_evt (UINT8 *p)
{
    UINT8       status;
    UINT16      handle;
    UINT8       current_mode;
    UINT16      interval;

    STREAM_TO_UINT8 (status, p);

    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT8 (current_mode, p);
    STREAM_TO_UINT16 (interval, p);
#if BTM_SCO_WAKE_PARKED_LINK == TRUE
    btm_sco_chk_pend_unpark (status, handle);
#endif
    btm_pm_proc_mode_change (status, handle, current_mode, interval);
    HCI_TRACE_WARNING("hcif mode change: hdl 0x%x, mode %d, intv %d, status 0x%x",
                    handle, current_mode, interval, status);

    /*
    #if (HID_DEV_INCLUDED == TRUE) && (HID_DEV_PM_INCLUDED == TRUE)
        hidd_pm_proc_mode_change( status, current_mode, interval ) ;
    #endif
    */
}

/*******************************************************************************
**
** Function         btu_hcif_ssr_evt
**
** Description      Process event HCI_SNIFF_SUB_RATE_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_ssr_evt (UINT8 *p, UINT16 evt_len)
{
#if (BTM_SSR_INCLUDED == TRUE)
    btm_pm_proc_ssr_evt(p, evt_len);
#endif

    UINT8       status;
    UINT16      handle;
    UINT16      max_tx_lat;
    UINT16      max_rx_lat;

    STREAM_TO_UINT8 (status, p);
    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT16 (max_tx_lat, p);
    STREAM_TO_UINT16 (max_rx_lat, p);

    UNUSED(status);
    UNUSED(handle);
    UNUSED(max_tx_lat);
    UNUSED(max_rx_lat);

    HCI_TRACE_WARNING("hcif ssr evt: st 0x%x, hdl 0x%x, tx_lat %d rx_lat %d", status, handle, max_tx_lat, max_rx_lat);
}

/*******************************************************************************
**
** Function         btu_hcif_pin_code_request_evt
**
** Description      Process event HCI_PIN_CODE_REQUEST_EVT
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_pin_code_request_evt (UINT8 *p)
{
#if (CLASSIC_BT_INCLUDED == TRUE)
    BD_ADDR  bda;

    STREAM_TO_BDADDR (bda, p);

    /* Tell L2CAP that there was a PIN code request,  */
    /* it may need to stretch timeouts                */
    l2c_pin_code_request (bda);

    btm_sec_pin_code_request (bda);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
}


/*******************************************************************************
**
** Function         btu_hcif_link_key_request_evt
**
** Description      Process event HCI_LINK_KEY_REQUEST_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_link_key_request_evt (UINT8 *p)
{
    BD_ADDR  bda;

    STREAM_TO_BDADDR (bda, p);
    btm_sec_link_key_request (bda);
}

/*******************************************************************************
**
** Function         btu_hcif_link_key_notification_evt
**
** Description      Process event HCI_LINK_KEY_NOTIFICATION_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_link_key_notification_evt (UINT8 *p)
{
    BD_ADDR  bda;
    LINK_KEY key;
    UINT8    key_type;

    STREAM_TO_BDADDR (bda, p);
    STREAM_TO_ARRAY16 (key, p);
    STREAM_TO_UINT8 (key_type, p);

    btm_sec_link_key_notification (bda, key, key_type);
}
#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         btu_hcif_loopback_command_evt
**
** Description      Process event HCI_LOOPBACK_COMMAND_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_loopback_command_evt (void)
{
}


/*******************************************************************************
**
** Function         btu_hcif_data_buf_overflow_evt
**
** Description      Process event HCI_DATA_BUF_OVERFLOW_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_data_buf_overflow_evt (void)
{
}


/*******************************************************************************
**
** Function         btu_hcif_max_slots_changed_evt
**
** Description      Process event HCI_MAX_SLOTS_CHANGED_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_max_slots_changed_evt (void)
{
}


/*******************************************************************************
**
** Function         btu_hcif_read_clock_off_comp_evt
**
** Description      Process event HCI_READ_CLOCK_OFF_COMP_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_read_clock_off_comp_evt (UINT8 *p)
{
    UINT8       status;
    UINT16      handle;
    UINT16      clock_offset;

    STREAM_TO_UINT8  (status, p);

    /* If failed to get clock offset just drop the result */
    if (status != HCI_SUCCESS) {
        return;
    }

    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT16 (clock_offset, p);

    handle = HCID_GET_HANDLE (handle);

    btm_process_clk_off_comp_evt (handle, clock_offset);
    btm_sec_update_clock_offset (handle, clock_offset);
}


/*******************************************************************************
**
** Function         btu_hcif_conn_pkt_type_change_evt
**
** Description      Process event HCI_CONN_PKT_TYPE_CHANGE_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_conn_pkt_type_change_evt (UINT8 *p)
{
    UINT8       status;
    UINT16      handle;
    UINT16      pkt_types;

    STREAM_TO_UINT8  (status, p);
    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT16 (pkt_types, p);

    handle = HCID_GET_HANDLE (handle);

    btm_acl_pkt_types_changed(status, handle, pkt_types);
}


/*******************************************************************************
**
** Function         btu_hcif_qos_violation_evt
**
** Description      Process event HCI_QOS_VIOLATION_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_qos_violation_evt (UINT8 *p)
{
    UINT16   handle;

    STREAM_TO_UINT16 (handle, p);

    handle = HCID_GET_HANDLE (handle);


    l2c_link_hci_qos_violation (handle);
}


/*******************************************************************************
**
** Function         btu_hcif_page_scan_mode_change_evt
**
** Description      Process event HCI_PAGE_SCAN_MODE_CHANGE_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_page_scan_mode_change_evt (void)
{
}


/*******************************************************************************
**
** Function         btu_hcif_page_scan_rep_mode_chng_evt
**
** Description      Process event HCI_PAGE_SCAN_REP_MODE_CHNG_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_page_scan_rep_mode_chng_evt (void)
{
}

/**********************************************
** Simple Pairing Events
***********************************************/

/*******************************************************************************
**
** Function         btu_hcif_host_support_evt
**
** Description      Process event HCI_RMT_HOST_SUP_FEAT_NOTIFY_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_host_support_evt (UINT8 *p)
{
    btm_sec_rmt_host_support_feat_evt(p);
}

/*******************************************************************************
**
** Function         btu_hcif_io_cap_request_evt
**
** Description      Process event HCI_IO_CAPABILITY_REQUEST_EVT
**
** Returns          void
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_io_cap_request_evt (UINT8 *p)
{
    btm_io_capabilities_req(p);
}


/*******************************************************************************
**
** Function         btu_hcif_io_cap_response_evt
**
** Description      Process event HCI_IO_CAPABILITY_RESPONSE_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_io_cap_response_evt (UINT8 *p)
{
    btm_io_capabilities_rsp(p);
}


/*******************************************************************************
**
** Function         btu_hcif_user_conf_request_evt
**
** Description      Process event HCI_USER_CONFIRMATION_REQUEST_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_user_conf_request_evt (UINT8 *p)
{
    btm_proc_sp_req_evt(BTM_SP_CFM_REQ_EVT, p);
}

/*******************************************************************************
**
** Function         btu_hcif_user_passkey_request_evt
**
** Description      Process event HCI_USER_PASSKEY_REQUEST_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_user_passkey_request_evt (UINT8 *p)
{
    btm_proc_sp_req_evt(BTM_SP_KEY_REQ_EVT, p);
}

/*******************************************************************************
**
** Function         btu_hcif_simple_pair_complete_evt
**
** Description      Process event HCI_SIMPLE_PAIRING_COMPLETE_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_simple_pair_complete_evt (UINT8 *p)
{
    btm_simple_pair_complete(p);
}

/*******************************************************************************
**
** Function         btu_hcif_user_passkey_notif_evt
**
** Description      Process event HCI_USER_PASSKEY_NOTIFY_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_user_passkey_notif_evt (UINT8 *p)
{
    btm_proc_sp_req_evt(BTM_SP_KEY_NOTIF_EVT, p);
}

/*******************************************************************************
**
** Function         btu_hcif_keypress_notif_evt
**
** Description      Process event HCI_KEYPRESS_NOTIFY_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_keypress_notif_evt (UINT8 *p)
{
    btm_keypress_notif_evt(p);
}
#endif /* (CLASSIC_BT_INCLUDED == TRUE) */

/*******************************************************************************
**
** Function         btu_hcif_rem_oob_request_evt
**
** Description      Process event HCI_REMOTE_OOB_DATA_REQUEST_EVT
**
** Returns          void
**
*******************************************************************************/
#if BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE
static void btu_hcif_rem_oob_request_evt (UINT8 *p)
{
    btm_rem_oob_req(p);
}
#endif

/*******************************************************************************
**
** Function         btu_hcif_link_supv_to_changed_evt
**
** Description      Process event HCI_LINK_SUPER_TOUT_CHANGED_EVT
**
** Returns          void
**
*******************************************************************************/
static void btu_hcif_link_supv_to_changed_evt (UINT8 *p)
{
    UINT16 handle;
    UINT16 supv_to;

    STREAM_TO_UINT16(handle, p);
    STREAM_TO_UINT16(supv_to, p);

    UNUSED(handle);
    UNUSED(supv_to);

    HCI_TRACE_WARNING("hcif link supv_to changed: hdl 0x%x, supv_to %d", handle, supv_to);
}

/*******************************************************************************
**
** Function         btu_hcif_enhanced_flush_complete_evt
**
** Description      Process event HCI_ENHANCED_FLUSH_COMPLETE_EVT
**
** Returns          void
**
*******************************************************************************/
#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
static void btu_hcif_enhanced_flush_complete_evt (void)
{
    /* This is empty until an upper layer cares about returning event */
}
#endif
/**********************************************
** End of Simple Pairing Events
***********************************************/


/**********************************************
** BLE Events
***********************************************/
#if (defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE)
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_encryption_key_refresh_cmpl_evt (UINT8 *p)
{
    UINT8   status;
    UINT8   enc_enable = 0;
    UINT16  handle;

    STREAM_TO_UINT8  (status, p);
    STREAM_TO_UINT16 (handle, p);

    if (status == HCI_SUCCESS) {
        enc_enable = 1;
    }

    btm_sec_encrypt_change (handle, status, enc_enable);
}
#endif  ///SMP_INCLUDED == TRUE

static void btu_ble_ll_conn_complete_evt ( UINT8 *p, UINT16 evt_len)
{
    btm_ble_conn_complete(p, evt_len, FALSE);
}
//#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
static void btu_ble_proc_enhanced_conn_cmpl( UINT8 *p, UINT16 evt_len)
{
    btm_ble_conn_complete(p, evt_len, TRUE);
}
//#endif
static void btu_ble_ll_conn_param_upd_evt (UINT8 *p, UINT16 evt_len)
{
    /* LE connection update has completed successfully as a master. */
    /* We can enable the update request if the result is a success. */
    /* extract the HCI handle first */
    UINT8   status;
    UINT16  handle, conn_interval, conn_latency, conn_timeout;
    STREAM_TO_UINT8  (status, p);
    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT16 (conn_interval, p);
    STREAM_TO_UINT16 (conn_latency, p);
    STREAM_TO_UINT16 (conn_timeout, p);

    l2cble_process_conn_update_evt(handle, status, conn_interval,
                                   conn_latency, conn_timeout);
}

static void btu_ble_ll_get_conn_param_format_err_from_contoller (UINT8 status, UINT16 handle)
{
    /* host send illegal connection parameters format, controller would send
       back HCI_ERR_ILLEGAL_PARAMETER_FMT */
    l2cble_get_conn_param_format_err_from_contoller(status, handle);

}

static void btu_ble_read_remote_feat_evt (UINT8 *p)
{
    btm_ble_read_remote_features_complete(p);
}

#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
static void btu_ble_proc_ltk_req (UINT8 *p)
{

    UINT16 ediv, handle;
    UINT8   *pp;

    STREAM_TO_UINT16(handle, p);
    pp = p + 8;
    STREAM_TO_UINT16(ediv, pp);

    btm_ble_ltk_request(handle, p, ediv);
    /* This is empty until an upper layer cares about returning event */
}
#endif  ///BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE


static void btu_ble_data_length_change_evt(UINT8 *p, UINT16 evt_len)
{
    UINT16 handle;
    UINT16 tx_data_len;
    UINT16 rx_data_len;

    if (!controller_get_interface()->supports_ble_packet_extension()) {
        HCI_TRACE_WARNING("%s, request not supported", __FUNCTION__);
        return;
    }

    STREAM_TO_UINT16(handle, p);
    STREAM_TO_UINT16(tx_data_len, p);
    p += 2; /* Skip the TxTimer */
    STREAM_TO_UINT16(rx_data_len, p);

    l2cble_process_data_length_change_event(handle, tx_data_len, rx_data_len);
}
#if (BLE_50_FEATURE_SUPPORT == TRUE)
static void btu_ble_phy_update_complete_evt(UINT8 *p)
{
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }
    tBTM_BLE_UPDATE_PHY update_phy = {0};
    STREAM_TO_UINT8(update_phy.status, p);
    STREAM_TO_UINT16(update_phy.conn_idx, p);
    STREAM_TO_UINT8(update_phy.tx_phy, p);
    STREAM_TO_UINT8(update_phy.rx_phy, p);

    btm_ble_update_phy_evt(&update_phy);
}

#if BLE_PRIVACY_SPT == TRUE
/*******************************************************************************
**
** Function         btm_ble_resolve_random_addr_adv_ext
**
** Description      resolve random address complete callback.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_resolve_random_addr_adv_ext(void *p_rec, void *p)
{
    tBTM_SEC_DEV_REC    *match_rec = (tBTM_SEC_DEV_REC *) p_rec;
    BD_ADDR     bda;
    UINT8       *pp = (UINT8 *)p+4; //jump to the location of bd addr
    if (match_rec) {
        // Assign the original address to be the current report address
        memcpy(bda, match_rec->ble.pseudo_addr, BD_ADDR_LEN);
        BDADDR_TO_STREAM(pp,bda);
    }
}
#endif

static void btu_ble_ext_adv_report_evt(UINT8 *p, UINT16 evt_len)
{
    tBTM_BLE_EXT_ADV_REPORT ext_adv_report = {0};
    UINT8 num_reports = {0};
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    UINT8 *pp = p;
#endif
    //UINT8 legacy_event_type = 0;
    UINT16 evt_type = 0;
    uint8_t addr_type;
    BD_ADDR bda;
    #if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    BOOLEAN             match = FALSE;
    #endif

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(num_reports, p);

    if (num_reports == 0) {
        HCI_TRACE_ERROR("%s, Invalid number reports is 0", __func__);
    }

    while (num_reports--) {
        STREAM_TO_UINT16(evt_type, p);
        ext_adv_report.event_type = evt_type & 0x1F;
        if(ext_adv_report.event_type & BTM_BLE_ADV_LEGACY_MASK) {
            ext_adv_report.data_status = BTM_BLE_EXT_ADV_DATA_COMPLETE;
        } else {
            switch(evt_type & BTM_BLE_ADV_DATA_STATUS_MASK) {
                case BTM_BLE_ADV_DATA_COMPLETE_MASK:
                    ext_adv_report.data_status = BTM_BLE_EXT_ADV_DATA_COMPLETE;
                    break;
                case BTM_BLE_ADV_DATA_INCOMPLETE_MASK:
                    ext_adv_report.data_status = BTM_BLE_EXT_ADV_DATA_INCOMPLETE;
                    break;
                case BTM_BLE_ADV_DATA_TRUNCATED_MASK:
                    ext_adv_report.data_status = BTM_BLE_EXT_ADV_DATA_TRUNCATED;
                    break;
            }
        }

        STREAM_TO_UINT8(addr_type, p);
        STREAM_TO_BDADDR(bda, p);
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
        if(addr_type != 0xFF) {
            match = btm_identity_addr_to_random_pseudo(bda, &addr_type, FALSE);
            if (!match && BTM_BLE_IS_RESOLVE_BDA(bda)) {
                btm_ble_resolve_random_addr(bda, btm_ble_resolve_random_addr_adv_ext, pp);
                //the BDADDR may be updated, so read it again
                p = p - sizeof(bda);
                STREAM_TO_BDADDR(bda, p);
            }
        }
#endif
        ext_adv_report.addr_type = addr_type;
        memcpy(ext_adv_report.addr, bda, 6);
        STREAM_TO_UINT8(ext_adv_report.primary_phy, p);
        STREAM_TO_UINT8(ext_adv_report.secondry_phy, p);
        STREAM_TO_UINT8(ext_adv_report.sid, p);
        STREAM_TO_UINT8(ext_adv_report.tx_power, p);
        STREAM_TO_UINT8(ext_adv_report.rssi, p);
        STREAM_TO_UINT16(ext_adv_report.per_adv_interval, p);
        STREAM_TO_UINT8(ext_adv_report.dir_addr_type, p);
        STREAM_TO_BDADDR(ext_adv_report.dir_addr, p);
        STREAM_TO_UINT8(ext_adv_report.adv_data_len, p);
        if (ext_adv_report.adv_data_len) {
            ext_adv_report.adv_data = p;
        } else {
            ext_adv_report.adv_data = NULL;
        }

        btm_ble_ext_adv_report_evt(&ext_adv_report);
        p += ext_adv_report.adv_data_len;
    }

}

static void btu_ble_periodic_adv_sync_establish_evt(UINT8 *p)
{
    tBTM_BLE_PERIOD_ADV_SYNC_ESTAB sync_estab = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(sync_estab.status, p);
    STREAM_TO_UINT16(sync_estab.sync_handle, p);
    STREAM_TO_UINT8(sync_estab.sid, p);
    STREAM_TO_UINT8(sync_estab.adv_addr_type, p);
    STREAM_TO_BDADDR(sync_estab.adv_addr, p);
    STREAM_TO_UINT8(sync_estab.adv_phy, p);
    STREAM_TO_UINT16(sync_estab.period_adv_interval, p);
    STREAM_TO_UINT8(sync_estab.adv_clk_accuracy, p);

    btm_ble_periodic_adv_sync_establish_evt(&sync_estab);
}

static void btu_ble_periodic_adv_report_evt(UINT8 *p, UINT8 evt_len)
{
    tBTM_PERIOD_ADV_REPORT adv_report = {0};
    /* This parameter is intended to be used in a future feature. */
    UINT8 unused = 0;

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    if (evt_len < MIN_BLE_PERIODIC_ADV_REPORT_LEN) {
        HCI_TRACE_ERROR("%s, Invalid params, the adv len is too short.", __func__);
        return;
    }

    STREAM_TO_UINT16(adv_report.sync_handle, p);
    STREAM_TO_UINT8(adv_report.tx_power, p);
    STREAM_TO_UINT8(adv_report.rssi, p);
    STREAM_TO_UINT8(unused, p);
    STREAM_TO_UINT8(adv_report.data_status, p);
    STREAM_TO_UINT8(adv_report.data_length, p);

    if ((evt_len - MIN_BLE_PERIODIC_ADV_REPORT_LEN) != adv_report.data_length) {
        HCI_TRACE_ERROR("%s, Invalid ev_len = %d is less than adv len = %d", __func__, evt_len, adv_report.data_length);
        return;
    }

    if (adv_report.data_length) {
        adv_report.data = p;
    } else {
        adv_report.data = NULL;
    }

    btm_ble_periodic_adv_report_evt(&adv_report);

    UNUSED(unused);
}

static void btu_ble_periodic_adv_sync_lost_evt(UINT8 *p)
{
    tBTM_BLE_PERIOD_ADV_SYNC_LOST sync_lost = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT16(sync_lost.sync_handle, p);

    btm_ble_periodic_adv_sync_lost_evt(&sync_lost);
}

static void btu_ble_scan_timeout_evt(UINT8 *p)
{
    UNUSED(p);

    btm_ble_scan_timeout_evt();
}

static void btu_ble_adv_set_terminate_evt(UINT8 *p)
{
    tBTM_BLE_ADV_TERMINAT adv_term = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(adv_term.status, p);
    STREAM_TO_UINT8(adv_term.adv_handle, p);
    STREAM_TO_UINT16(adv_term.conn_handle, p);
    STREAM_TO_UINT8(adv_term.completed_event, p);

    btm_ble_adv_set_terminated_evt(&adv_term);
}

static void btu_ble_scan_req_received_evt(UINT8 *p)
{
    tBTM_BLE_SCAN_REQ_RECEIVED req_received = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(req_received.adv_handle, p);
    STREAM_TO_UINT8(req_received.scan_addr_type, p);
    STREAM_TO_BDADDR(req_received.scan_addr, p);

    btm_ble_scan_req_received_evt(&req_received);
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
static void btu_ble_periodic_adv_sync_trans_recv(UINT8 *p)
{
    UINT16 conn_handle;
    tL2C_LCB *p_lcb = NULL;
    tBTM_BLE_PERIOD_ADV_SYNC_TRANS_RECV past_recv = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(past_recv.status, p);
    STREAM_TO_UINT16(conn_handle, p);
    STREAM_TO_UINT16(past_recv.service_data, p);
    STREAM_TO_UINT16(past_recv.sync_handle, p);
    STREAM_TO_UINT8(past_recv.adv_sid, p);
    STREAM_TO_UINT8(past_recv.adv_addr_type, p);
    STREAM_TO_BDADDR(past_recv.adv_addr, p);
    STREAM_TO_UINT8(past_recv.adv_phy, p);
    STREAM_TO_UINT16(past_recv.adv_interval, p);
    STREAM_TO_UINT8(past_recv.adv_clk_accuracy, p);

    p_lcb = l2cu_find_lcb_by_handle(conn_handle);
    if(p_lcb) {
       memcpy(past_recv.addr, p_lcb->remote_bd_addr, BD_ADDR_LEN);
    }

    btm_ble_periodic_adv_sync_trans_recv_evt(&past_recv);
}
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

/**********************************************
** End of BLE Events Handler
***********************************************/
#if (defined BLE_LLT_INCLUDED) && (BLE_LLT_INCLUDED == TRUE)
static void btu_ble_rc_param_req_evt(UINT8 *p)
{
    UINT16 handle;
    UINT16  int_min, int_max, latency, timeout;

    STREAM_TO_UINT16(handle, p);
    STREAM_TO_UINT16(int_min, p);
    STREAM_TO_UINT16(int_max, p);
    STREAM_TO_UINT16(latency, p);
    STREAM_TO_UINT16(timeout, p);

    l2cble_process_rc_param_request_evt(handle, int_min, int_max, latency, timeout);
}
#endif /* BLE_LLT_INCLUDED */
#endif /* BLE_INCLUDED */
