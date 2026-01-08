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

#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
// BLE Channel Sounding parameter validation macros per BLE spec
#define BTM_BLE_CS_MAX_STEPS_REPORTED               0xA0  // Range: 0x00 to 0xA0 (0 to 160)
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)

#if (BLE_FEAT_CTE_EN == TRUE)
// BLE CTE parameter validation macros per BLE spec
#define BTM_BLE_CTE_MAX_SAMPLE_COUNT                0x52  // Maximum sample count per BLE spec: 0x52 (82)
#endif // (BLE_FEAT_CTE_EN == TRUE)


/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_inquiry_comp_evt (UINT8 *p);
static void btu_hcif_inquiry_result_evt (UINT8 *p);
static void btu_hcif_inquiry_rssi_result_evt (UINT8 *p);
static void btu_hcif_extended_inquiry_result_evt (UINT8 *p);

static void btu_hcif_connection_comp_evt (UINT8 *p);
static void btu_hcif_connection_request_evt (UINT8 *p);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_disconnection_comp_evt (UINT8 *p);
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_authentication_comp_evt (UINT8 *p);
#endif  ///SMP_INCLUDED == TRUE
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_rmt_name_request_comp_evt (UINT8 *p, UINT16 evt_len);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_encryption_change_evt (UINT8 *p);
#endif  ///SMP_INCLUDED == TRUE
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_read_rmt_features_comp_evt (UINT8 *p);
static void btu_hcif_read_rmt_ext_features_comp_evt (UINT8 *p);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_read_rmt_version_comp_evt (UINT8 *p);
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_qos_setup_comp_evt (UINT8 *p);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_command_complete_evt (BT_HDR *response, void *context);
static void btu_hcif_command_status_evt (uint8_t status, BT_HDR *command, void *context);
static void btu_hcif_hardware_error_evt (UINT8 *p);
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_flush_occured_evt (void);
static void btu_hcif_role_change_evt (UINT8 *p);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_num_compl_data_pkts_evt (UINT8 *p);

#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_mode_change_evt (UINT8 *p);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

#if (SMP_INCLUDED == TRUE)
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_pin_code_request_evt (UINT8 *p);
static void btu_hcif_link_key_request_evt (UINT8 *p);
static void btu_hcif_link_key_notification_evt (UINT8 *p);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
#endif  ///SMP_INCLUDED == TRUE
#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

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
#if (CLASSIC_BT_INCLUDED == TRUE)
static void btu_hcif_link_supv_to_changed_evt (UINT8 *p);
#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
static void btu_hcif_enhanced_flush_complete_evt (void);
#endif

static void btu_hcif_ssr_evt (UINT8 *p, UINT16 evt_len);
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

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
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
static void btu_ble_ext_adv_report_evt(UINT8 *p, UINT16 evt_len);
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
static void btu_ble_periodic_adv_sync_establish_evt(UINT8 *p, bool v2_evt);
static void btu_ble_periodic_adv_report_evt(UINT8 *p, UINT8 evt_len, bool v2_evt);
static void btu_ble_periodic_adv_sync_lost_evt(UINT8 *p);
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
static void btu_ble_scan_timeout_evt(UINT8 *p);
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
#if (BLE_50_EXTEND_ADV_EN == TRUE)
static void btu_ble_adv_set_terminate_evt(UINT8 *p);
static void btu_ble_scan_req_received_evt(UINT8 *p);
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)
static void btu_ble_channel_select_alg_evt(UINT8 *p);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
static void btu_ble_periodic_adv_sync_trans_recv(UINT8 *p);
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

#if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
void btu_ble_create_big_cmd_status(UINT8 status);
static void btu_ble_big_create_complete_evt(UINT8 *p);
void btu_ble_big_terminate_cmd_status(UINT8 status);
static void btu_ble_big_terminate_complete_evt(UINT8 *p);
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
static void btu_ble_create_big_sync_cmd_status(UINT8 status);
static void btu_ble_big_sync_establish_evt(UINT8 *p);
static void btu_ble_big_sync_lost_evt(UINT8 *p);
static void btu_ble_biginfo_adv_report_evt(UINT8 *p);
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
static void btu_ble_create_cis_cmd_status(UINT8 status);
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
static void btu_ble_accept_cis_req_cmd_status(UINT8 status);
static void btu_ble_cis_request_evt(UINT8 *p);
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
static void btu_ble_cis_established_evt(UINT8 *p, bool v2_evt);
static void btu_ble_cis_disconnected(UINT16 handle, UINT8 reason);
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)

#endif // #if (BLE_FEAT_ISO_EN == TRUE)


#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
static void btu_ble_cte_connless_iq_report_evt(UINT8 *p);
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
static void btu_ble_cte_conn_iq_report_evt(UINT8 *p);
static void btu_ble_cte_req_failed_evt(UINT8 *p);
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
#endif // #if (BLE_FEAT_CTE_EN == TRUE)

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
static void btu_ble_path_loss_threshold_evt(UINT8 *p);
static void btu_ble_transmit_power_report_evt(UINT8 *p);
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)

#if (BLE_FEAT_CONN_SUBRATING == TRUE)
static void btu_ble_subrate_change_evt(UINT8 *p);
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)

#if (BT_BLE_FEAT_PAWR_EN == TRUE)
static void btu_ble_pa_subevt_data_request_evt(UINT8 *p);
static void btu_ble_pa_response_report_evt(UINT8 *p);
#endif // (BT_BLE_FEAT_PAWR_EN == TRUE)

#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
static void btu_ble_cs_read_remote_supp_caps_evt(UINT8 *p);
static void btu_ble_cs_read_remote_fae_tab_evt(UINT8 *p);
static void btu_ble_cs_security_enable_cmpl_evt(UINT8 *p);
static void btu_ble_cs_config_cmpl_evt(UINT8 *p);
static void btu_ble_cs_proc_enable_cmpl_evt(UINT8 *p);
static void btu_ble_cs_subevt_result_evt(UINT8 *p);
static void btu_ble_cs_subevt_result_continue_evt(UINT8 *p);
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)

#if (BLE_42_ADV_EN == TRUE)
extern osi_sem_t adv_enable_sem;
extern osi_sem_t adv_data_sem;
extern osi_sem_t adv_param_sem;
extern uint8_t adv_enable_status;
extern uint8_t adv_data_status;
extern uint8_t adv_param_status;
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
extern osi_sem_t scan_enable_sem;
extern osi_sem_t scan_param_sem;
extern uint8_t scan_enable_status;
extern uint8_t scan_param_status;
#endif // #if (BLE_42_SCAN_EN == TRUE)
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
#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_DISCONNECTION_COMP_EVT:
        btu_hcif_disconnection_comp_evt (p);
        break;
    case HCI_AUTHENTICATION_COMP_EVT:
#if (SMP_INCLUDED == TRUE)
        btu_hcif_authentication_comp_evt (p);
#endif  ///SMP_INCLUDED == TRUE
        break;
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_RMT_NAME_REQUEST_COMP_EVT:
        btu_hcif_rmt_name_request_comp_evt (p, hci_evt_len);
        break;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
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
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_READ_RMT_FEATURES_COMP_EVT:
        btu_hcif_read_rmt_features_comp_evt (p);
        break;
    case HCI_READ_RMT_EXT_FEATURES_COMP_EVT:
        btu_hcif_read_rmt_ext_features_comp_evt (p);
        break;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_READ_RMT_VERSION_COMP_EVT:
        btu_hcif_read_rmt_version_comp_evt (p);
        break;
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_QOS_SETUP_COMP_EVT:
        btu_hcif_qos_setup_comp_evt (p);
        break;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
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
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_FLUSH_OCCURED_EVT:
        btu_hcif_flush_occured_evt ();
        break;
    case HCI_ROLE_CHANGE_EVT:
        btu_hcif_role_change_evt (p);
        break;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_NUM_COMPL_DATA_PKTS_EVT:
        btu_hcif_num_compl_data_pkts_evt (p);
        break;
 #if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_MODE_CHANGE_EVT:
        btu_hcif_mode_change_evt (p);
        break;
#endif //  #if (CLASSIC_BT_INCLUDED == TRUE)
#if (SMP_INCLUDED == TRUE)
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_PIN_CODE_REQUEST_EVT:
        btu_hcif_pin_code_request_evt (p);
        break;
    case HCI_LINK_KEY_REQUEST_EVT:
        btu_hcif_link_key_request_evt (p);
        break;
    case HCI_LINK_KEY_NOTIFICATION_EVT:
        btu_hcif_link_key_notification_evt (p);
        break;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
#endif  ///SMP_INCLUDED == TRUE
#if (CLASSIC_BT_INCLUDED == TRUE)
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
    case HCI_LINK_SUPER_TOUT_CHANGED_EVT:
        btu_hcif_link_supv_to_changed_evt (p);
        break;
#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
    case HCI_ENHANCED_FLUSH_COMPLETE_EVT:
        btu_hcif_enhanced_flush_complete_evt ();
        break;
#endif
#endif /* (CLASSIC_BT_INCLUDED == TRUE) */

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
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
        case HCI_BLE_EXT_ADV_REPORT_EVT:
            //HCI_TRACE_ERROR("%s, HCI_BLE_EXT_ADV_REPORT_EVT.", __func__);
            btu_ble_ext_adv_report_evt(p, hci_evt_len);
            break;
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
        case HCI_BLE_PERIOD_ADV_SYNC_ESTAB_EVT:
            btu_ble_periodic_adv_sync_establish_evt(p, false);
            break;
        case HCI_BLE_PERIOD_ADV_SYNC_ESTAB_EVT_V2:
            btu_ble_periodic_adv_sync_establish_evt(p, true);
            break;
        case HCI_BLE_PERIOD_ADV_REPORT_EVT:
            btu_ble_periodic_adv_report_evt(p, hci_evt_len, false);
            break;
        case HCI_BLE_PERIOD_ADV_REPORT_EVT_V2:
            btu_ble_periodic_adv_report_evt(p, hci_evt_len, true);
            break;
        case HCI_BLE_PERIOD_ADV_SYNC_LOST_EVT:
            btu_ble_periodic_adv_sync_lost_evt(p);
            break;
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
        case HCI_BLE_SCAN_TIMEOUT_EVT:
            btu_ble_scan_timeout_evt(p);
            break;
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
#if (BLE_50_EXTEND_ADV_EN == TRUE)
        case HCI_BLE_ADV_SET_TERMINATED_EVT:
            btu_ble_adv_set_terminate_evt(p);
            break;
        case HCI_BLE_SCAN_REQ_RECEIVED_EVT:
            btu_ble_scan_req_received_evt(p);
            break;
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)
        case HCI_BLE_CHANNEL_SELECT_ALG:
            btu_ble_channel_select_alg_evt(p);
            break;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
        case HCI_BLE_PERIOD_ADV_SYNC_TRANS_RECV_EVT:
            btu_ble_periodic_adv_sync_trans_recv(p);
            break;
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
        case HCI_BLE_CIS_ESTABLISHED_V1_EVT:
            btu_ble_cis_established_evt(p, false);
            break;
        case HCI_BLE_CIS_ESTABLISHED_V2_EVT:
            btu_ble_cis_established_evt(p, true);
            break;
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
        case HCI_BLE_CIS_REQUEST_EVT:
            btu_ble_cis_request_evt(p);
            break;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
        case HCI_BLE_BIG_CREATE_COMPLETE_EVT:
            btu_ble_big_create_complete_evt(p);
            break;
        case HCI_BLE_BIG_TERMINATE_COMPLETE_EVT:
            btu_ble_big_terminate_complete_evt(p);
            break;
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
        case HCI_BLE_BIG_SYNC_ESTABLISHED_EVT:
            btu_ble_big_sync_establish_evt(p);
            break;
        case HCI_BLE_BIG_SYNC_LOST_EVT:
            btu_ble_big_sync_lost_evt(p);
            break;
        case HCI_BLE_BIGINFO_ADV_REPORT_EVT:
            btu_ble_biginfo_adv_report_evt(p);
            break;
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
#endif // #if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
        case HCI_BLE_CONNLESS_IQ_REPORT_EVT:
            btu_ble_cte_connless_iq_report_evt(p);
            break;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
        case HCI_BLE_CONN_IQ_REPORT_EVT:
            btu_ble_cte_conn_iq_report_evt(p);
            break;
        case HCI_BLE_CTE_REQUEST_FAILED_EVT:
            btu_ble_cte_req_failed_evt(p);
            break;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

#endif // #if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
        case HCI_BLE_PATH_LOSS_THRESHOLD_EVT:
            btu_ble_path_loss_threshold_evt(p);
            break;
        case HCI_BLE_TRANS_POWER_REPORTING_EVT:
            btu_ble_transmit_power_report_evt(p);
            break;
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BLE_FEAT_CONN_SUBRATING == TRUE)
        case HCI_BLE_SUBRATE_CHANGE_EVT:
            btu_ble_subrate_change_evt(p);
            break;
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
        case HCI_BLE_PA_SUBEVT_DATA_REQUEST_EVT:
            btu_ble_pa_subevt_data_request_evt(p);
            break;
        case HCI_BLE_PA_RESPONSE_REPORT_EVT:
            btu_ble_pa_response_report_evt(p);
            break;
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
        case HCI_BLE_CS_READ_REMOTE_SUPP_CAPS_CMPL_EVT:
            btu_ble_cs_read_remote_supp_caps_evt(p);
            break;
        case HCI_BLE_CS_READ_REMOTE_FAE_TAB_CMPL_EVT:
            btu_ble_cs_read_remote_fae_tab_evt(p);
            break;
        case HCI_BLE_CS_SECURITY_ENABLE_CMPL_EVT:
            btu_ble_cs_security_enable_cmpl_evt(p);
            break;
        case HCI_BLE_CS_CONFIG_CMPL_EVT:
            btu_ble_cs_config_cmpl_evt(p);
            break;
        case HCI_BLE_CS_PROC_ENABLE_CMPL_EVT:
            btu_ble_cs_proc_enable_cmpl_evt(p);
            break;
        case HCI_BLE_CS_SUBEVENT_RESULT_EVT:
            btu_ble_cs_subevt_result_evt(p);
            break;
        case HCI_BLE_CS_SUBEVENT_RESULT_CONTINUE_EVT:
            btu_ble_cs_subevt_result_continue_evt(p);
            break;
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
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
#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

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
    BOOLEAN dev_find = FALSE;

    ++p;
    STREAM_TO_UINT16 (handle, p);
    STREAM_TO_UINT8  (reason, p);

    handle = HCID_GET_HANDLE (handle);

    dev_find = btm_acl_disconnected(handle, reason);

#if (BLE_FEAT_ISO_CIG_EN == TRUE)
    // Not find device. it is iso handle
    if (!dev_find) {
        btu_ble_cis_disconnected(handle, reason);
    }
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)

    HCI_TRACE_WARNING("hcif disc complete: hdl 0x%x, rsn 0x%x dev_find %d", handle, reason, dev_find);

    UNUSED(dev_find);
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

#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

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
#if (CLASSIC_BT_INCLUDED == TRUE)
    btm_acl_encrypt_change (handle, status, encr_enable);
#endif // (CLASSIC_BT_INCLUDED == TRUE)
    btm_sec_encrypt_change (handle, status, encr_enable);
}
#endif  ///SMP_INCLUDED == TRUE
#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

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

#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

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
#if (BLE_INCLUDED == TRUE)
    uint8_t status;
#endif // (BLE_INCLUDED == TRUE)
    switch (opcode) {
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_INQUIRY_CANCEL:
        /* Tell inquiry processing that we are done */
        btm_process_cancel_complete(HCI_SUCCESS, BTM_BR_INQUIRY_MASK);
        break;
    case HCI_SET_EVENT_FILTER:
        btm_event_filter_complete (p);
        break;

    case HCI_DELETE_STORED_LINK_KEY:
        btm_delete_stored_link_key_complete (p, evt_len);
        break;

    case HCI_GET_LINK_QUALITY:
#if (CLASSIC_BT_INCLUDED == TRUE)
        btm_read_link_quality_complete (p);
#endif // (CLASSIC_BT_INCLUDED == TRUE)
        break;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_READ_LOCAL_NAME:
#if (CLASSIC_BT_INCLUDED == TRUE)
        btm_read_local_name_complete (p, evt_len);
#endif // (CLASSIC_BT_INCLUDED == TRUE)
        break;
    case HCI_READ_RSSI:
        btm_read_rssi_complete (p, evt_len);
        break;
    case HCI_BLE_READ_CHNL_MAP:
#if BLE_INCLUDED == TRUE
        btm_read_channel_map_complete (p);
#endif // #if BLE_INCLUDED == TRUE
        break;
    case HCI_READ_TRANSMIT_POWER_LEVEL:
        break;
#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_CREATE_CONNECTION_CANCEL:
        btm_create_conn_cancel_complete(p, evt_len);
        break;
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_READ_LOCAL_OOB_DATA:
#if BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE
        btm_read_local_oob_complete(p, evt_len);
#endif
        break;

#if (CLASSIC_BT_INCLUDED == TRUE)
    case HCI_READ_INQ_TX_POWER_LEVEL:
        btm_read_linq_tx_power_complete (p);
        break;
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
        break;
#if (BLE_42_ADV_EN == TRUE)
    case HCI_BLE_WRITE_ADV_ENABLE:
        btm_ble_write_adv_enable_complete(p);
        break;
#endif // #if (BLE_42_ADV_EN == TRUE)
    case HCI_BLE_CREATE_LL_CONN:
        btm_ble_create_ll_conn_complete(*p);
        break;

    case HCI_BLE_TRANSMITTER_TEST:
    case HCI_BLE_RECEIVER_TEST:
#if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))
    case HCI_BLE_TEST_END:
        btm_ble_test_command_complete(p);
        break;
#endif // #if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))
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
#if (BLE_50_EXTEND_ADV_EN == TRUE)
    case HCI_BLE_SET_EXT_ADV_PARAM:
    case HCI_BLE_SET_EXT_ADV_DATA:
    case HCI_BLE_SET_EXT_SCAN_RSP_DATA:
    case HCI_BLE_SET_EXT_ADV_ENABLE: {
        STREAM_TO_UINT8  (status, p);
        HCI_TRACE_EVENT("%s opcode 0x%x status 0x%x", __func__, opcode, status);
	    break;
    }
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)
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
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_50_DTM_TEST_EN == TRUE)
    case HCI_BLE_ENH_RX_TEST:
    case HCI_BLE_ENH_TX_TEST:
        btm_ble_test_command_complete(p);
        break;
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)

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
#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    case HCI_BLE_BIG_TERMINATE_SYNC:
        UINT16 big_handle;
        STREAM_TO_UINT8(status, p);
        STREAM_TO_UINT8(big_handle, p);
        btm_ble_big_sync_terminate_complete(status, big_handle);
        break;
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    case HCI_BLE_ISO_SET_DATA_PATH:
    case HCI_BLE_ISO_REMOVE_DATA_PATH:
        UINT16 conn_handle;
        STREAM_TO_UINT8(status, p);
        STREAM_TO_UINT16(conn_handle, p);
        btm_ble_iso_data_path_update_complete(opcode, status, conn_handle);
        break;
    case HCI_BLE_ISO_READ_TX_SYNC:
        btm_ble_iso_read_iso_tx_sync_complete(p);
        break;
    case HCI_BLE_ISO_READ_ISO_LINK_QUALITY:
        btm_ble_iso_read_iso_link_quality_complete(p);
        break;

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
    case HCI_BLE_ISO_SET_CIG_PARAMS:
    case HCI_BLE_ISO_SET_CIG_PARAMS_TEST:
        btm_ble_iso_set_cig_params_complete(p);
        break;
    case HCI_BLE_ISO_REMOVE_CIG:
        // do nothing
        break;
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
    case HCI_BLE_ISO_ACCEPT_CIS_REQ:
    case HCI_BLE_ISO_REJECT_CIS_REQ:
        // do nothing
        break;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#endif // #if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_CTE_EN == TRUE)
    case HCI_BLE_READ_ANT_INFOR:
        btm_ble_cte_read_ant_infor_complete(p);
        break;
#endif // #if (BLE_FEAT_CTE_EN == TRUE)

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
    case HCI_BLE_ENH_READ_TRANS_POWER_LEVEL:
        btm_enh_read_trans_pwr_level_cmpl_evt(p);
        break;
#endif //#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
    case HCI_BLE_CS_READ_LOCAL_SUPP_CAPS:
        btm_ble_cs_read_local_supp_caps_cmpl_evt(p);
        break;
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
#endif /* (BLE_INCLUDED == TRUE) */

    default: {
        if ((opcode & HCI_GRP_VENDOR_SPECIFIC) == HCI_GRP_VENDOR_SPECIFIC) {
            btm_vsc_complete (p, opcode, evt_len, (tBTM_CMPL_CB *)p_cplt_cback);
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

    if (*stream != HCI_SUCCESS) {
        HCI_TRACE_WARNING("opcode=0x%04x, status= %02x: %s", opcode, *stream, hci_status_code_to_string(*stream));
    }

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
#if (BLE_42_ADV_EN == TRUE)
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
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
        case HCI_BLE_WRITE_SCAN_PARAMS:
            scan_param_status = *stream;
            osi_sem_give(&scan_param_sem);
            break;
        case HCI_BLE_WRITE_SCAN_ENABLE:
            scan_enable_status = *stream;
            osi_sem_give(&scan_enable_sem);
            break;
#endif // #if (BLE_42_SCAN_EN == TRUE)
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
        HCI_TRACE_WARNING("opcode=0x%04x, status= %02x: %s", opcode, status, hci_status_code_to_string(status));
    }
    BD_ADDR         bd_addr;
    UINT16          handle;
#if BTM_SCO_INCLUDED == TRUE
    tBTM_ESCO_DATA  esco_data;
#endif

    switch (opcode) {
#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_50_EXTEND_SYNC_EN == TRUE)
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
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)
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
#if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
    case HCI_BLE_CREATE_BIG:
    case HCI_BLE_CREATE_BIG_TEST:
        btu_ble_create_big_cmd_status(status);
        break;
    case HCI_BLE_TERMINATE_BIG:
        btu_ble_big_terminate_cmd_status(status);
        break;
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    case HCI_BLE_BIG_CREATE_SYNC:
        btu_ble_create_big_sync_cmd_status(status);
        break;
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
    case HCI_BLE_ISO_CREATE_CIS:
        btu_ble_create_cis_cmd_status(status);
        break;
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
    case HCI_BLE_ISO_ACCEPT_CIS_REQ:
        btu_ble_accept_cis_req_cmd_status(status);
        break;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#endif // #if (BLE_FEAT_ISO_EN == TRUE)
#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
    case HCI_BLE_READ_REMOTE_TRANS_POWER_LEVEL:
        btm_read_remote_trans_pwr_level_cmpl(status);
        break;
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#if (BLE_FEAT_CONN_SUBRATING == TRUE)
    case HCI_BLE_SUBRATE_REQUEST:
        btm_subrate_req_cmd_status(status);
        break;
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)
#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
    case HCI_BLE_CS_READ_REMOTE_SUPP_CAPS:
        btm_ble_cs_read_remote_supp_caps_cmd_status(status);
        break;
    case HCI_BLE_CS_SECURITY_ENABLE:
        btm_ble_cs_security_enable_cmd_status(status);
        break;
    case HCI_BLE_CS_READ_REMOTE_FAE_TABLE:
        btm_ble_cs_read_remote_fae_table_cmd_status(status);
        break;
    case HCI_BLE_CS_CREATE_CONFIG:
        btm_ble_cs_update_config_cmd_status(status, true);
        break;
    case HCI_BLE_CS_REMOVE_CONFIG:
        btm_ble_cs_update_config_cmd_status(status, false);
        break;
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
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
            #if (CLASSIC_BT_INCLUDED == TRUE)
                /* Tell qos setup that we are done */
                btm_qos_setup_complete(status, 0, NULL);
            #endif // (CLASSIC_BT_INCLUDED == TRUE)
                break;

            case HCI_SWITCH_ROLE:
                /* Tell BTM that the command failed */
                /* read bd addr out of stored command */
            #if (CLASSIC_BT_INCLUDED == TRUE)
                if (p_cmd != NULL) {
                    p_cmd++;
                    STREAM_TO_BDADDR (bd_addr, p_cmd);
                    btm_acl_role_changed(status, bd_addr, BTM_ROLE_UNDEFINED);
                } else {
                    btm_acl_role_changed(status, NULL, BTM_ROLE_UNDEFINED);
                }
                l2c_link_role_changed (NULL, BTM_ROLE_UNDEFINED, HCI_ERR_COMMAND_DISALLOWED);
            #endif // (CLASSIC_BT_INCLUDED == TRUE)
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
    (void)metadata;

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

#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)

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
#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
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
#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
#endif  ///SMP_INCLUDED == TRUE

#if (CLASSIC_BT_INCLUDED == TRUE)
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

#if (CLASSIC_BT_INCLUDED == TRUE)
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
#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
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
#if (BLE_50_EXTEND_SCAN_EN == TRUE)
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
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)
#endif

#if (BLE_50_EXTEND_SCAN_EN == TRUE)
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
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
static void btu_ble_periodic_adv_sync_establish_evt(UINT8 *p, bool v2_evt)
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
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    if (v2_evt) {
        STREAM_TO_UINT8(sync_estab.num_subevt, p);
        STREAM_TO_UINT8(sync_estab.subevt_interval, p);
        STREAM_TO_UINT8(sync_estab.rsp_slot_delay, p);
        STREAM_TO_UINT8(sync_estab.rsp_slot_spacing, p);
    }
#endif // (BT_BLE_FEAT_PAWR_EN == TRUE)

    btm_ble_periodic_adv_sync_establish_evt(&sync_estab);
}

static void btu_ble_periodic_adv_report_evt(UINT8 *p, UINT8 evt_len, bool v2_evt)
{
    tBTM_PERIOD_ADV_REPORT adv_report = {0};
    /* This parameter is intended to be used in a future feature. */
    UINT8 unused = 0;
    UINT8 min_len = MIN_BLE_PERIODIC_ADV_REPORT_LEN;

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
    STREAM_TO_UINT8(adv_report.cte_type, p);
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    if (v2_evt) {
        STREAM_TO_UINT16(adv_report.periodic_evt_cnt, p);
        STREAM_TO_UINT8(adv_report.subevt, p);
        min_len += 3;
    }
#endif // (BT_BLE_FEAT_PAWR_EN == TRUE)
    STREAM_TO_UINT8(adv_report.data_status, p);
    STREAM_TO_UINT8(adv_report.data_length, p);

    if ((evt_len - min_len) != adv_report.data_length) {
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
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)

#if (BLE_50_EXTEND_SCAN_EN == TRUE)
static void btu_ble_scan_timeout_evt(UINT8 *p)
{
    UNUSED(p);

    btm_ble_scan_timeout_evt();
}
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)

#if (BLE_50_EXTEND_ADV_EN == TRUE)
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
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

static void btu_ble_channel_select_alg_evt(UINT8 *p)
{
    tBTM_BLE_CHANNEL_SEL_ALG chan_sel_alg = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT16(chan_sel_alg.conn_handle, p);
    STREAM_TO_UINT8(chan_sel_alg.channel_sel_alg, p);

    btm_ble_channel_select_algorithm_evt(&chan_sel_alg);
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

#if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
static void btu_ble_create_cis_cmd_status(UINT8 status)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (status != HCI_SUCCESS) {
        status = (BTM_HCI_ERROR | status);
    }
    cb_params.status = status;
    btm_ble_create_cis_cmd_status(&cb_params);
}
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
static void btu_ble_accept_cis_req_cmd_status(UINT8 status)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (status != HCI_SUCCESS) {
        status = (BTM_HCI_ERROR | status);
    }
    cb_params.status = status;

    btm_ble_accept_cis_req_cmd_status(&cb_params);
}
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_EN == TRUE)
static void btu_ble_cis_disconnected(UINT16 handle, UINT8 reason)
{
    tBTM_BLE_CIS_DISCON_CMPL cis_disconnected_evt = {0};
    cis_disconnected_evt.cis_handle = handle;
    cis_disconnected_evt.reason = reason;
    btm_ble_cis_disconnected_evt(&cis_disconnected_evt);
}

static void btu_ble_cis_established_evt(UINT8 *p, bool v2_evt)
{
    HCI_TRACE_DEBUG("%s", __func__);
    tBTM_BLE_CIS_ESTABLISHED_CMPL cis_estab_evt = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(cis_estab_evt.status, p);
    STREAM_TO_UINT16(cis_estab_evt.conn_handle, p);
    STREAM_TO_UINT24(cis_estab_evt.cig_sync_delay, p);
    STREAM_TO_UINT24(cis_estab_evt.cis_sync_delay, p);
    STREAM_TO_UINT24(cis_estab_evt.trans_lat_c_to_p, p);
    STREAM_TO_UINT24(cis_estab_evt.trans_lat_p_to_c, p);
    STREAM_TO_UINT8(cis_estab_evt.phy_c_to_p, p);
    STREAM_TO_UINT8(cis_estab_evt.phy_p_to_c, p);
    STREAM_TO_UINT8(cis_estab_evt.nse, p);
    STREAM_TO_UINT8(cis_estab_evt.bn_c_to_p, p);
    STREAM_TO_UINT8(cis_estab_evt.bn_p_to_c, p);
    STREAM_TO_UINT8(cis_estab_evt.ft_c_to_p, p);
    STREAM_TO_UINT8(cis_estab_evt.ft_p_to_c, p);
    STREAM_TO_UINT16(cis_estab_evt.max_pdu_c_to_p, p);
    STREAM_TO_UINT16(cis_estab_evt.max_pdu_p_to_c, p);
    STREAM_TO_UINT16(cis_estab_evt.iso_interval, p);
#if (BLE_FEAT_ISO_60_EN == TRUE)
    if (v2_evt) {
        STREAM_TO_UINT24(cis_estab_evt.sub_interval, p);
        STREAM_TO_UINT16(cis_estab_evt.max_sdu_c_to_p, p);
        STREAM_TO_UINT16(cis_estab_evt.max_sdu_p_to_c, p);
        STREAM_TO_UINT24(cis_estab_evt.sdu_int_c_to_p, p);
        STREAM_TO_UINT24(cis_estab_evt.sdu_int_p_to_c, p);
        STREAM_TO_UINT8(cis_estab_evt.framing, p);
    }
#endif // #if (BLE_FEAT_ISO_60_EN == TRUE)

    btm_ble_cis_established_evt(&cis_estab_evt);
}
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
static void btu_ble_cis_request_evt(UINT8 *p)
{
    HCI_TRACE_DEBUG("%s", __func__);
    tBTM_BLE_CIS_REQUEST_CMPL cis_req_evt = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT16(cis_req_evt.acl_handle, p);
    STREAM_TO_UINT16(cis_req_evt.cis_handle, p);
    STREAM_TO_UINT8(cis_req_evt.cig_id, p);
    STREAM_TO_UINT8(cis_req_evt.cis_id, p);


    btm_ble_cis_request_evt(&cis_req_evt);
}
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
void btu_ble_create_big_cmd_status(UINT8 status)
{
    if (status != HCI_SUCCESS) {
        tBTM_BLE_BIG_CREATE_CMPL big_cmpl = {0};
        big_cmpl.status = status;
        btm_ble_big_create_cmpl_evt(&big_cmpl);
    }
}

static void btu_ble_big_create_complete_evt(UINT8 *p)
{
    HCI_TRACE_DEBUG("%s", __func__);
    tBTM_BLE_BIG_CREATE_CMPL big_cmpl = {0};
    UINT8 num_bis;

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(big_cmpl.status, p);
    STREAM_TO_UINT8(big_cmpl.big_handle, p);
    STREAM_TO_UINT24(big_cmpl.big_sync_delay, p);
    STREAM_TO_UINT24(big_cmpl.transport_latency, p);
    STREAM_TO_UINT8(big_cmpl.phy, p);
    STREAM_TO_UINT8(big_cmpl.nse, p);
    STREAM_TO_UINT8(big_cmpl.bn, p);
    STREAM_TO_UINT8(big_cmpl.pto, p);
    STREAM_TO_UINT8(big_cmpl.irc, p);
    STREAM_TO_UINT16(big_cmpl.max_pdu, p);
    STREAM_TO_UINT16(big_cmpl.iso_interval, p);
    STREAM_TO_UINT8(num_bis, p);

    // Validate num_bis to prevent buffer overflow
    if (num_bis > BLE_ISO_BIS_MAX_COUNT) {
        HCI_TRACE_ERROR("%s, num_bis %d exceeds BLE_ISO_BIS_MAX_COUNT %d", __func__, num_bis, BLE_ISO_BIS_MAX_COUNT);
        num_bis = BLE_ISO_BIS_MAX_COUNT;
    }
    big_cmpl.num_bis = num_bis;

    for (uint8_t i = 0; i < num_bis; i++)
    {
        STREAM_TO_UINT16(big_cmpl.bis_handle[i], p);
        // only 12 bits meaningful
        big_cmpl.bis_handle[i] = (big_cmpl.bis_handle[i] & 0x0FFF);
    }

    btm_ble_big_create_cmpl_evt(&big_cmpl);
}

void btu_ble_big_terminate_cmd_status(UINT8 status)
{
    if (status != HCI_SUCCESS) {
        tBTM_BLE_BIG_TERMINATE_CMPL big_term = {0};
        big_term.status = (status | BTM_HCI_ERROR);
        btm_ble_big_terminate_cmpl_evt(&big_term);
    }
}

static void btu_ble_big_terminate_complete_evt(UINT8 *p)
{
    tBTM_BLE_BIG_TERMINATE_CMPL big_term = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(big_term.big_handle, p);
    STREAM_TO_UINT8(big_term.reason, p);
    big_term.status = HCI_SUCCESS;

    btm_ble_big_terminate_cmpl_evt(&big_term);
}
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
void btu_ble_create_big_sync_cmd_status(UINT8 status)
{
    if (status != HCI_SUCCESS) {
        tBTM_BLE_BIG_SYNC_ESTAB_CMPL big_estb = {0};
        big_estb.status = status;
        btm_ble_big_sync_estab_evt(&big_estb);
    }
}

static void btu_ble_big_sync_establish_evt(UINT8 *p)
{
    tBTM_BLE_BIG_SYNC_ESTAB_CMPL big_estb = {0};
    UINT8 num_bis;

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(big_estb.status, p);
    STREAM_TO_UINT8(big_estb.big_handle, p);
    STREAM_TO_UINT24(big_estb.transport_latency_big, p);
    STREAM_TO_UINT8(big_estb.nse, p);
    STREAM_TO_UINT8(big_estb.bn, p);
    STREAM_TO_UINT8(big_estb.pto, p);
    STREAM_TO_UINT8(big_estb.irc, p);
    STREAM_TO_UINT16(big_estb.max_pdu, p);
    STREAM_TO_UINT16(big_estb.iso_interval, p);
    STREAM_TO_UINT8(num_bis, p);

    // Validate num_bis to prevent buffer overflow
    if (num_bis > BLE_ISO_BIS_MAX_COUNT) {
        HCI_TRACE_ERROR("%s, num_bis %d exceeds BLE_ISO_BIS_MAX_COUNT %d", __func__, num_bis, BLE_ISO_BIS_MAX_COUNT);
        num_bis = BLE_ISO_BIS_MAX_COUNT;
    }
    big_estb.num_bis = num_bis;

    for (uint8_t i = 0; i < num_bis; i++)
    {
        STREAM_TO_UINT16(big_estb.bis_handle[i], p);
    }

    btm_ble_big_sync_estab_evt(&big_estb);
}

static void btu_ble_big_sync_lost_evt(UINT8 *p)
{
    tBTM_BLE_BIG_SYNC_LOST_EVT big_sync_lost = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(big_sync_lost.big_handle, p);
    STREAM_TO_UINT8(big_sync_lost.reason, p);

    btm_ble_big_sync_lost_evt(&big_sync_lost);
}

static void btu_ble_biginfo_adv_report_evt(UINT8 *p)
{
    tBTM_BLE_BIGINFO_ADV_REPORT_EVT biginfo_report = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT16(biginfo_report.sync_handle, p);
    STREAM_TO_UINT8(biginfo_report.num_bis, p);
    STREAM_TO_UINT8(biginfo_report.nse, p);
    STREAM_TO_UINT16(biginfo_report.iso_interval, p);
    STREAM_TO_UINT8(biginfo_report.bn, p);
    STREAM_TO_UINT8(biginfo_report.pto, p);
    STREAM_TO_UINT8(biginfo_report.irc, p);
    STREAM_TO_UINT16(biginfo_report.max_pdu, p);
    STREAM_TO_UINT24(biginfo_report.sdu_interval, p);
    STREAM_TO_UINT16(biginfo_report.max_sdu, p);
    STREAM_TO_UINT8(biginfo_report.phy, p);
    STREAM_TO_UINT8(biginfo_report.framing, p);
    STREAM_TO_UINT8(biginfo_report.encryption, p);

    btm_ble_biginfo_adv_report_evt(&biginfo_report);
}
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
#endif // #if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
static void btu_ble_cte_connless_iq_report_evt(UINT8 *p)
{
    tBTM_BLE_CTE_CONNLESS_IQ_REPORT_EVT connless_iq_rpt = {0};
    UINT8 sample_count;

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT16(connless_iq_rpt.sync_handle, p);
    STREAM_TO_UINT8(connless_iq_rpt.channel_idx, p);
    STREAM_TO_UINT16(connless_iq_rpt.rssi, p);
    STREAM_TO_UINT8(connless_iq_rpt.rssi_ant_id, p);
    STREAM_TO_UINT8(connless_iq_rpt.cte_type, p);
    STREAM_TO_UINT8(connless_iq_rpt.slot_dur, p);
    STREAM_TO_UINT8(connless_iq_rpt.pkt_status, p);
    STREAM_TO_UINT16(connless_iq_rpt.periodic_evt_counter, p);
    STREAM_TO_UINT8(sample_count, p);

    // Validate sample_count to prevent buffer overflow
    if (sample_count > BTM_BLE_CTE_MAX_SAMPLE_COUNT) {
        HCI_TRACE_ERROR("%s, sample_count %d exceeds maximum %d", __func__, sample_count, BTM_BLE_CTE_MAX_SAMPLE_COUNT);
        sample_count = BTM_BLE_CTE_MAX_SAMPLE_COUNT;
    }
    connless_iq_rpt.sample_count = sample_count;

    for (uint8_t i = 0; i < sample_count; i++)
    {
        STREAM_TO_UINT8(connless_iq_rpt.i_sample[i], p);
    }

    for (uint8_t i = 0; i < sample_count; i++)
    {
        STREAM_TO_UINT8(connless_iq_rpt.q_sample[i], p);
    }

    btm_ble_connless_iq_report_evt(&connless_iq_rpt);
}
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
static void btu_ble_cte_conn_iq_report_evt(UINT8 *p)
{
    tBTM_BLE_CTE_CONN_IQ_REPORT_EVT conn_iq_rpt = {0};
    UINT8 sample_count;

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT16(conn_iq_rpt.conn_handle, p);
    STREAM_TO_UINT8(conn_iq_rpt.rx_phy, p);
    STREAM_TO_UINT8(conn_iq_rpt.data_channel_idx, p);
    STREAM_TO_UINT16(conn_iq_rpt.rssi, p);
    STREAM_TO_UINT8(conn_iq_rpt.rssi_ant_id, p);
    STREAM_TO_UINT8(conn_iq_rpt.cte_type, p);
    STREAM_TO_UINT8(conn_iq_rpt.slot_dur, p);
    STREAM_TO_UINT8(conn_iq_rpt.pkt_status, p);
    STREAM_TO_UINT16(conn_iq_rpt.conn_evt_counter, p);
    STREAM_TO_UINT8(sample_count, p);

    // Validate sample_count to prevent buffer overflow
    if (sample_count > BTM_BLE_CTE_MAX_SAMPLE_COUNT) {
        HCI_TRACE_ERROR("%s, sample_count %d exceeds maximum %d", __func__, sample_count, BTM_BLE_CTE_MAX_SAMPLE_COUNT);
        sample_count = BTM_BLE_CTE_MAX_SAMPLE_COUNT;
    }
    conn_iq_rpt.sample_count = sample_count;

    for (uint8_t i = 0; i < sample_count; i++)
    {
        STREAM_TO_UINT8(conn_iq_rpt.i_sample[i], p);
    }

    for (uint8_t i = 0; i < sample_count; i++)
    {
        STREAM_TO_UINT8(conn_iq_rpt.q_sample[i], p);
    }

    btm_ble_conn_iq_report_evt(&conn_iq_rpt);
}

static void btu_ble_cte_req_failed_evt(UINT8 *p)
{
    tBTM_BLE_CTE_REQ_FAILED_EVT cte_req_failed = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(cte_req_failed.status, p);
    STREAM_TO_UINT16(cte_req_failed.conn_handle, p);

    btm_ble_cte_req_failed_evt(&cte_req_failed);
}
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

#endif // #if (BLE_FEAT_CTE_EN == TRUE)

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
static void btu_ble_path_loss_threshold_evt(UINT8 *p)
{
    tBTM_BLE_PATH_LOSS_THRESHOLD_EVT path_loss_thres_evt = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT16(path_loss_thres_evt.conn_handle, p);
    STREAM_TO_UINT8(path_loss_thres_evt.cur_path_loss, p);
    STREAM_TO_UINT8(path_loss_thres_evt.zone_entered, p);

    btm_ble_path_loss_threshold_evt(&path_loss_thres_evt);
}

static void btu_ble_transmit_power_report_evt(UINT8 *p)
{
    tBTM_BLE_TRANS_POWER_REPORT_EVT trans_pwr_report_evt = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(trans_pwr_report_evt.status, p);
    STREAM_TO_UINT16(trans_pwr_report_evt.conn_handle, p);
    STREAM_TO_UINT8(trans_pwr_report_evt.reason, p);
    STREAM_TO_UINT8(trans_pwr_report_evt.phy, p);
    STREAM_TO_UINT8(trans_pwr_report_evt.tx_power_level, p);
    STREAM_TO_UINT8(trans_pwr_report_evt.tx_power_level_flag, p);
    STREAM_TO_UINT8(trans_pwr_report_evt.delta, p);

    btm_ble_transmit_power_report_evt(&trans_pwr_report_evt);
}
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)

#if (BLE_FEAT_CONN_SUBRATING == TRUE)
static void btu_ble_subrate_change_evt(UINT8 *p)
{
    tBTM_BLE_SUBRATE_CHANGE_EVT subrate_change_evt = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(subrate_change_evt.status, p);
    STREAM_TO_UINT16(subrate_change_evt.conn_handle, p);
    STREAM_TO_UINT16(subrate_change_evt.subrate_factor, p);
    STREAM_TO_UINT16(subrate_change_evt.peripheral_latency, p);
    STREAM_TO_UINT16(subrate_change_evt.continuation_number, p);
    STREAM_TO_UINT16(subrate_change_evt.supervision_timeout, p);

    btm_ble_subrate_change_evt(&subrate_change_evt);
}
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)

#if (BT_BLE_FEAT_PAWR_EN == TRUE)
static void btu_ble_pa_subevt_data_request_evt(UINT8 *p)
{
    tBTM_BLE_PA_SUBEVT_DATA_REQ_EVT pa_subevt_req_evt = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(pa_subevt_req_evt.adv_handle, p);
    STREAM_TO_UINT8(pa_subevt_req_evt.subevt_start, p);
    STREAM_TO_UINT8(pa_subevt_req_evt.subevt_data_count, p);

    btm_ble_pa_subevt_data_req_evt(&pa_subevt_req_evt);
}

static void btu_ble_pa_response_report_evt(UINT8 *p)
{
    tBTM_BLE_PA_RSP_REPORT_EVT pa_rsp_rpt_evt = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(pa_rsp_rpt_evt.adv_handle, p);
    STREAM_TO_UINT8(pa_rsp_rpt_evt.subevt, p);
    STREAM_TO_UINT8(pa_rsp_rpt_evt.tx_status, p);
    STREAM_TO_UINT8(pa_rsp_rpt_evt.num_rsp, p);

    // num_rsp is UINT8, range 0x00 to 0xFF (0 to 255), no need to validate
    if (pa_rsp_rpt_evt.num_rsp) {
        pa_rsp_rpt_evt.rsp_data_info = osi_malloc(pa_rsp_rpt_evt.num_rsp * sizeof(tBTM_BLE_PA_RSP_DATA_INFO));
        if (pa_rsp_rpt_evt.rsp_data_info)
        {
            for (UINT8 i = 0; i < pa_rsp_rpt_evt.num_rsp; i++)
            {
                STREAM_TO_UINT8(pa_rsp_rpt_evt.rsp_data_info[i].tx_power, p);
                STREAM_TO_UINT8(pa_rsp_rpt_evt.rsp_data_info[i].rssi, p);
                STREAM_TO_UINT8(pa_rsp_rpt_evt.rsp_data_info[i].cte_type, p);
                STREAM_TO_UINT8(pa_rsp_rpt_evt.rsp_data_info[i].rsp_slot, p);
                STREAM_TO_UINT8(pa_rsp_rpt_evt.rsp_data_info[i].data_status, p);
                STREAM_TO_UINT8(pa_rsp_rpt_evt.rsp_data_info[i].data_len, p);
                // data_len is UINT8, range 0x00 to 0xFF (0 to 255), no need to validate
                if (pa_rsp_rpt_evt.rsp_data_info[i].data_len) {
                    pa_rsp_rpt_evt.rsp_data_info[i].data = osi_malloc(pa_rsp_rpt_evt.rsp_data_info[i].data_len);
                    if (pa_rsp_rpt_evt.rsp_data_info[i].data) {
                        STREAM_TO_ARRAY(pa_rsp_rpt_evt.rsp_data_info[i].data, p, pa_rsp_rpt_evt.rsp_data_info[i].data_len);
                    } else {
                        HCI_TRACE_ERROR("%s, no enough memory for data_len %d at index %d", __func__, pa_rsp_rpt_evt.rsp_data_info[i].data_len, i);
                        pa_rsp_rpt_evt.rsp_data_info[i].data_len = 0;
                    }
                } else {
                    pa_rsp_rpt_evt.rsp_data_info[i].data = NULL;
                }
            }
        } else {
            HCI_TRACE_ERROR("%s, no memory for rsp_data_info", __func__);
        }
    }

    btm_ble_pa_rsp_rpt_evt(&pa_rsp_rpt_evt);

    if (pa_rsp_rpt_evt.rsp_data_info)
    {
        for (UINT8 i = 0; i < pa_rsp_rpt_evt.num_rsp; i++)
        {
            if (pa_rsp_rpt_evt.rsp_data_info[i].data) {
                osi_free(pa_rsp_rpt_evt.rsp_data_info[i].data);
            }
        }
        osi_free(pa_rsp_rpt_evt.rsp_data_info);
    }
}
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)

#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
static void btu_ble_cs_read_remote_supp_caps_evt(UINT8 *p)
{
    tBTM_BLE_CS_READ_REMOTE_SUPP_CAPS_CMPL_EVT cs_read_remote_supp_caps = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(cs_read_remote_supp_caps.status, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.conn_handle, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.num_config_supported, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.max_consecutive_proc_supported, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.num_ant_supported, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.max_ant_paths_supported, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.roles_supported, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.modes_supported, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.rtt_capability, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.rtt_aa_only_n, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.rtt_sounding_n, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.rtt_random_payload_n, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.NADM_sounding_capability, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.NADM_random_capability, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.cs_sync_phys_supported, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.subfeatures_supported, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.T_IP1_times_supported, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.T_IP2_times_supported, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.T_FCS_times_supported, p);
    STREAM_TO_UINT16(cs_read_remote_supp_caps.T_PM_times_supported, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.T_SW_times_supported, p);
    STREAM_TO_UINT8(cs_read_remote_supp_caps.TX_SNR_capability, p);

    btm_ble_cs_read_remote_supp_caps_cmpl_evt(&cs_read_remote_supp_caps);
}

static void btu_ble_cs_read_remote_fae_tab_evt(UINT8 *p)
{
    tBTM_BLE_CS_READ_REMOTE_FAE_TAB_CMPL_EVT cs_read_remote_fae_tab = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(cs_read_remote_fae_tab.status, p);
    STREAM_TO_UINT16(cs_read_remote_fae_tab.conn_handle, p);
    STREAM_TO_ARRAY(cs_read_remote_fae_tab.remote_fae_table, p, 72);

    btm_ble_cs_read_remote_fae_tab_cmpl_evt(&cs_read_remote_fae_tab);
}

static void btu_ble_cs_security_enable_cmpl_evt(UINT8 *p)
{
    tBTM_BLE_CS_SEC_ENABLE_CMPL_EVT cs_security_enable = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }
    STREAM_TO_UINT8(cs_security_enable.status, p);
    STREAM_TO_UINT16(cs_security_enable.conn_handle, p);

    btm_ble_cs_securuty_enable_cmpl_evt(&cs_security_enable);
}

static void btu_ble_cs_config_cmpl_evt(UINT8 *p)
{
    tBTM_BLE_CS_CONFIG_CMPL_EVT config_cmpl = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }
    STREAM_TO_UINT8(config_cmpl.status, p);
    STREAM_TO_UINT16(config_cmpl.conn_handle, p);
    STREAM_TO_UINT8(config_cmpl.config_id, p);
    STREAM_TO_UINT8(config_cmpl.action, p);;
    STREAM_TO_UINT8(config_cmpl.main_mode_type, p);
    STREAM_TO_UINT8(config_cmpl.sub_mode_type, p);
    STREAM_TO_UINT8(config_cmpl.min_main_mode_steps, p);
    STREAM_TO_UINT8(config_cmpl.max_main_mode_steps, p);
    STREAM_TO_UINT8(config_cmpl.main_mode_repetition, p);
    STREAM_TO_UINT8(config_cmpl.mode_0_steps, p);
    STREAM_TO_UINT8(config_cmpl.role, p);
    STREAM_TO_UINT8(config_cmpl.rtt_type, p);
    STREAM_TO_UINT8(config_cmpl.cs_sync_phy, p);
    STREAM_TO_ARRAY(config_cmpl.channel_map, p, 10);
    STREAM_TO_UINT8(config_cmpl.channel_map_repetition, p);
    STREAM_TO_UINT8(config_cmpl.channel_selection_type, p);
    STREAM_TO_UINT8(config_cmpl.ch3c_shape, p);
    STREAM_TO_UINT8(config_cmpl.ch3c_jump, p);
    STREAM_TO_UINT8(config_cmpl.reserved, p);
    STREAM_TO_UINT8(config_cmpl.t_ip1_time, p);
    STREAM_TO_UINT8(config_cmpl.t_ip2_time, p);
    STREAM_TO_UINT8(config_cmpl.t_fcs_time, p);
    STREAM_TO_UINT8(config_cmpl.t_pm_time, p);

    btm_ble_cs_config_cmpl_evt(&config_cmpl);
}

static void btu_ble_cs_proc_enable_cmpl_evt(UINT8 *p)
{
    tBTM_BLE_CS_PROC_ENABLE_CMPL_EVT proc_en = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT8(proc_en.status, p);
    STREAM_TO_UINT16(proc_en.conn_handle, p);
    STREAM_TO_UINT8(proc_en.config_id, p);
    STREAM_TO_UINT8(proc_en.state, p);
    STREAM_TO_UINT8(proc_en.tone_ant_config_select, p);
    STREAM_TO_UINT8(proc_en.select_tx_power, p);
    STREAM_TO_UINT24(proc_en.subevent_Len, p);
    STREAM_TO_UINT8(proc_en.subevents_per_event, p);
    STREAM_TO_UINT16(proc_en.subevent_interval, p);
    STREAM_TO_UINT16(proc_en.event_interval, p);
    STREAM_TO_UINT16(proc_en.procedure_interval, p);
    STREAM_TO_UINT16(proc_en.procedure_count, p);
    STREAM_TO_UINT16(proc_en.max_procedure_len, p);

    btm_ble_cs_proc_enable_cmpl_evt(&proc_en);
}

static void btu_ble_cs_subevt_result_evt(UINT8 *p)
{
    tBTM_BLE_CS_SUBEVT_RESULT_CMPL_EVT subevt_result = {0};
    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }
    STREAM_TO_UINT16(subevt_result.conn_handle, p);
    STREAM_TO_UINT8(subevt_result.config_id, p);
    STREAM_TO_UINT16(subevt_result.start_acl_conn_event_counter, p);
    STREAM_TO_UINT16(subevt_result.procedure_counter, p);
    STREAM_TO_UINT16(subevt_result.frequency_compensation, p);
    STREAM_TO_UINT8(subevt_result.reference_power_level, p);
    STREAM_TO_UINT8(subevt_result.procedure_done_status, p);
    STREAM_TO_UINT8(subevt_result.subevent_done_status, p);
    STREAM_TO_UINT8(subevt_result.abort_reason, p);
    STREAM_TO_UINT8(subevt_result.num_ant_paths, p);
    UINT8 num_steps_reported;
    STREAM_TO_UINT8(num_steps_reported, p);

    // Validate num_steps_reported per BLE spec: Range 0x00 to 0xA0 (0 to 160)
    if (num_steps_reported > BTM_BLE_CS_MAX_STEPS_REPORTED) {
        HCI_TRACE_ERROR("%s, num_steps_reported %d exceeds maximum %d", __func__, num_steps_reported, BTM_BLE_CS_MAX_STEPS_REPORTED);
        num_steps_reported = BTM_BLE_CS_MAX_STEPS_REPORTED;
    }
    subevt_result.num_steps_reported = num_steps_reported;

    subevt_result.step_info = osi_malloc(subevt_result.num_steps_reported * sizeof(tBTM_BLE_CS_STEP_INFO));
    if (subevt_result.step_info) {
        for (uint8_t i = 0; i < subevt_result.num_steps_reported; i++)
        {
            STREAM_TO_UINT8(subevt_result.step_info[i].step_mode, p);
            STREAM_TO_UINT8(subevt_result.step_info[i].step_channel, p);
            STREAM_TO_UINT8(subevt_result.step_info[i].step_data_len, p);
            // step_data_len is UINT8, range 0x00 to 0xFF (0 to 255), no need to validate
            subevt_result.step_info[i].data = osi_malloc(subevt_result.step_info[i].step_data_len);
            if (subevt_result.step_info[i].data) {
                STREAM_TO_ARRAY(subevt_result.step_info[i].data, p, subevt_result.step_info[i].step_data_len);
            } else if (subevt_result.step_info[i].step_data_len) {
                HCI_TRACE_ERROR("%s, no memory.", __func__);
            }
        }
    }

    btm_ble_cs_subevt_result_evt(&subevt_result);

    if (subevt_result.step_info)
    {
        for (UINT8 i = 0; i < subevt_result.num_steps_reported; i++)
        {
            if (subevt_result.step_info[i].data) {
                osi_free(subevt_result.step_info[i].data);
            }
        }
        osi_free(subevt_result.step_info);
    }

}

static void btu_ble_cs_subevt_result_continue_evt(UINT8 *p)
{
    tBTM_BLE_CS_SUBEVT_RESULT_CONTINUE_EVT subevt_continue_result = {0};

    if (!p) {
        HCI_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    STREAM_TO_UINT16(subevt_continue_result.conn_handle, p);
    STREAM_TO_UINT8(subevt_continue_result.config_id, p);
    STREAM_TO_UINT8(subevt_continue_result.proc_done_status, p);
    STREAM_TO_UINT8(subevt_continue_result.subevt_done_status, p);
    STREAM_TO_UINT8(subevt_continue_result.abort_reason, p);
    STREAM_TO_UINT8(subevt_continue_result.num_ant_paths, p);
    UINT8 num_steps_reported;
    STREAM_TO_UINT8(num_steps_reported, p);

    // Validate num_steps_reported per BLE spec: Range 0x00 to 0xA0 (0 to 160)
    if (num_steps_reported > BTM_BLE_CS_MAX_STEPS_REPORTED) {
        HCI_TRACE_ERROR("%s, num_steps_reported %d exceeds maximum %d", __func__, num_steps_reported, BTM_BLE_CS_MAX_STEPS_REPORTED);
        num_steps_reported = BTM_BLE_CS_MAX_STEPS_REPORTED;
    }
    subevt_continue_result.num_steps_reported = num_steps_reported;

    subevt_continue_result.step_info = osi_malloc(subevt_continue_result.num_steps_reported * sizeof(tBTM_BLE_CS_STEP_INFO));
    if (subevt_continue_result.step_info) {
        for (uint8_t i = 0; i < subevt_continue_result.num_steps_reported; i++) {
            STREAM_TO_UINT8(subevt_continue_result.step_info[i].step_mode, p);
            STREAM_TO_UINT8(subevt_continue_result.step_info[i].step_channel, p);
            STREAM_TO_UINT8(subevt_continue_result.step_info[i].step_data_len, p);
            // step_data_len is UINT8, range 0x00 to 0xFF (0 to 255), no need to validate
            subevt_continue_result.step_info[i].data = osi_malloc(subevt_continue_result.step_info[i].step_data_len);
            if (subevt_continue_result.step_info[i].data) {
                STREAM_TO_ARRAY(subevt_continue_result.step_info[i].data, p, subevt_continue_result.step_info[i].step_data_len);
            } else if (subevt_continue_result.step_info[i].step_data_len) {
                HCI_TRACE_ERROR("%s, no memory.", __func__);
            }
        }
    }

    btm_ble_cs_subevt_continue_result_evt(&subevt_continue_result);

    if (subevt_continue_result.step_info)
    {
        for (UINT8 i = 0; i < subevt_continue_result.num_steps_reported; i++)
        {
            if (subevt_continue_result.step_info[i].data) {
                osi_free(subevt_continue_result.step_info[i].data);
            }
        }
        osi_free(subevt_continue_result.step_info);
    }

}
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)

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
