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

#include "gki.h"
#include "bt_types.h"
#include "hcimsgs.h"
#include "btu.h"
#include "l2c_int.h"
#include "btm_api.h"
#include "btm_int.h"
//#include "bt_utils.h"
#include "controller.h"
#include "osi.h"
#include "hci_layer.h"
#include "bt_trace.h"

#include "thread.h"

// TODO(zachoverflow): remove this horrible hack
#include "btu.h"
extern fixed_queue_t *btu_hci_msg_queue;

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
static void btu_hcif_conn_pkt_type_change_evt (void);
static void btu_hcif_qos_violation_evt (UINT8 *p);
static void btu_hcif_page_scan_mode_change_evt (void);
static void btu_hcif_page_scan_rep_mode_chng_evt (void);
static void btu_hcif_esco_connection_comp_evt(UINT8 *p);
static void btu_hcif_esco_connection_chg_evt(UINT8 *p);

/* Simple Pairing Events */
static void btu_hcif_host_support_evt (UINT8 *p);
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_io_cap_request_evt (UINT8 *p);
static void btu_hcif_io_cap_response_evt (UINT8 *p);
static void btu_hcif_user_conf_request_evt (UINT8 *p);
static void btu_hcif_user_passkey_request_evt (UINT8 *p);
static void btu_hcif_user_passkey_notif_evt (UINT8 *p);
static void btu_hcif_keypress_notif_evt (UINT8 *p);
#endif  ///SMP_INCLUDED == TRUE
#if BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE
static void btu_hcif_rem_oob_request_evt (UINT8 *p);
#endif
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_simple_pair_complete_evt (UINT8 *p);
#endif  ///SMP_INCLUDED == TRUE
#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
static void btu_hcif_enhanced_flush_complete_evt (void);
#endif

#if (BTM_SSR_INCLUDED == TRUE)
static void btu_hcif_ssr_evt (UINT8 *p, UINT16 evt_len);
#endif /* BTM_SSR_INCLUDED == TRUE */

#if BLE_INCLUDED == TRUE
static void btu_ble_ll_conn_complete_evt (UINT8 *p, UINT16 evt_len);
static void btu_ble_process_adv_pkt (UINT8 *p);
static void btu_ble_read_remote_feat_evt (UINT8 *p);
static void btu_ble_ll_conn_param_upd_evt (UINT8 *p, UINT16 evt_len);
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
        btu_hcif_conn_pkt_type_change_evt ();
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
#if (BTM_SSR_INCLUDED == TRUE)
    case HCI_SNIFF_SUB_RATE_EVT:
        btu_hcif_ssr_evt (p, hci_evt_len);
        break;
#endif  /* BTM_SSR_INCLUDED == TRUE */
    case HCI_RMT_HOST_SUP_FEAT_NOTIFY_EVT:
        btu_hcif_host_support_evt (p);
        break;
#if (SMP_INCLUDED == TRUE)
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
#endif  ///SMP_INCLUDED == TRUE
#if BTM_OOB_INCLUDED == TRUE && SMP_INCLUDED == TRUE
    case HCI_REMOTE_OOB_DATA_REQUEST_EVT:
        btu_hcif_rem_oob_request_evt (p);
        break;
#endif
#if (SMP_INCLUDED == TRUE)
    case HCI_SIMPLE_PAIRING_COMPLETE_EVT:
        btu_hcif_simple_pair_complete_evt (p);
        break;
    case HCI_USER_PASSKEY_NOTIFY_EVT:
        btu_hcif_user_passkey_notif_evt (p);
        break;
    case HCI_KEYPRESS_NOTIFY_EVT:
        btu_hcif_keypress_notif_evt (p);
        break;
#endif  ///SMP_INCLUDED == TRUE
#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
    case HCI_ENHANCED_FLUSH_COMPLETE_EVT:
        btu_hcif_enhanced_flush_complete_evt ();
        break;
#endif

#if (BLE_INCLUDED == TRUE)
    case HCI_BLE_EVENT:
        STREAM_TO_UINT8  (ble_sub_code, p);

        HCI_TRACE_DEBUG("BLE HCI(id=%d) event = 0x%02x)", hci_evt_code,  ble_sub_code);

        switch (ble_sub_code) {
        case HCI_BLE_ADV_PKT_RPT_EVT: /* result of inquiry */
            btu_ble_process_adv_pkt(p);
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
        return;
    }

    uint16_t opcode;
    uint8_t *stream = p_buf->data + p_buf->offset;
    void *vsc_callback = NULL;

    STREAM_TO_UINT16(opcode, stream);

    // Eww...horrible hackery here
    /* If command was a VSC, then extract command_complete callback */
    if ((opcode & HCI_GRP_VENDOR_SPECIFIC) == HCI_GRP_VENDOR_SPECIFIC
#if BLE_INCLUDED == TRUE
            || (opcode == HCI_BLE_RAND)
            || (opcode == HCI_BLE_ENCRYPT)
#endif
       ) {
        vsc_callback = *((void **)(p_buf + 1));
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
#if SMP_INCLUDED == TRUE
    UINT8       enc_mode;
#endif  ///SMP_INCLUDED == TRUE
#if BTM_SCO_INCLUDED == TRUE
    tBTM_ESCO_DATA  esco_data;
#endif

    STREAM_TO_UINT8    (status, p);
    STREAM_TO_UINT16   (handle, p);
    STREAM_TO_BDADDR   (bda, p);
    STREAM_TO_UINT8    (link_type, p);
#if (SMP_INCLUDED == TRUE)
    STREAM_TO_UINT8    (enc_mode, p);
#endif  ///SMP_INCLUDED == TRUE
    handle = HCID_GET_HANDLE (handle);

    if (link_type == HCI_LINK_TYPE_ACL) {
#if (SMP_INCLUDED == TRUE)
        btm_sec_connected (bda, handle, status, enc_mode);
#endif  ///SMP_INCLUDED == TRUE
        l2c_link_hci_conn_comp (status, handle, bda);
    }
#if BTM_SCO_INCLUDED == TRUE
    else {
        memset(&esco_data, 0, sizeof(tBTM_ESCO_DATA));
        /* esco_data.link_type = HCI_LINK_TYPE_SCO; already zero */
        memcpy (esco_data.bd_addr, bda, BD_ADDR_LEN);
        btm_sco_connected (status, bda, handle, &esco_data);
    }
#endif /* BTM_SCO_INCLUDED */
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

#if BTM_SCO_INCLUDED == TRUE
    /* If L2CAP doesn't know about it, send it to SCO */
    if (!l2c_link_hci_disc_comp (handle, reason)) {
        btm_sco_removed (handle, reason);
    }
#else
    l2c_link_hci_disc_comp (handle, reason);
#endif /* BTM_SCO_INCLUDED */
#if (SMP_INCLUDED == TRUE)
    /* Notify security manager */
    btm_sec_disconnected (handle, reason);
#endif  ///SMP_INCLUDED == TRUE
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

#if (BLE_INCLUDED == TRUE)
    /* BLE Commands sComplete*/
    case HCI_BLE_ADD_WHITE_LIST:
        btm_ble_add_2_white_list_complete(*p);
        break;

    case HCI_BLE_CLEAR_WHITE_LIST:
        btm_ble_clear_white_list_complete(p, evt_len);
        break;

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
    case HCI_BLE_SET_ADDR_RESOLUTION_ENABLE:
    case HCI_BLE_SET_RAND_PRIV_ADDR_TIMOUT:
        break;
#endif
#endif /* (BLE_INCLUDED == TRUE) */

    default:
        if ((opcode & HCI_GRP_VENDOR_SPECIFIC) == HCI_GRP_VENDOR_SPECIFIC) {
            btm_vsc_complete (p, opcode, evt_len, (tBTM_CMPL_CB *)p_cplt_cback);
        }
        break;
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

    GKI_freebuf(hack->response);
    osi_free(event);
}

static void btu_hcif_command_complete_evt(BT_HDR *response, void *context)
{
    BT_HDR *event = osi_calloc(sizeof(BT_HDR) + sizeof(command_complete_hack_t));
    command_complete_hack_t *hack = (command_complete_hack_t *)&event->data[0];

    LOG_DEBUG("btu_hcif_command_complete_evt\n");

    hack->callback = btu_hcif_command_complete_evt_on_task;
    hack->response = response;
    hack->context = context;

    event->event = BTU_POST_TO_TASK_NO_GOOD_HORRIBLE_HACK;

    fixed_queue_enqueue(btu_hci_msg_queue, event);
    // ke_event_set(KE_EVENT_BTU_TASK_THREAD);
    btu_task_post(SIG_BTU_WORK, TASK_POST_BLOCKING);
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
            case HCI_BLE_CREATE_LL_CONN:
                btm_ble_create_ll_conn_complete(status);
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

    GKI_freebuf(hack->command);
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

    fixed_queue_enqueue(btu_hci_msg_queue, event);
    //ke_event_set(KE_EVENT_BTU_TASK_THREAD);
    btu_task_post(SIG_BTU_WORK, TASK_POST_BLOCKING);
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
    /*?? No SCO for now */
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
#if (BTM_SSR_INCLUDED == TRUE)
static void btu_hcif_ssr_evt (UINT8 *p, UINT16 evt_len)
{
    btm_pm_proc_ssr_evt(p, evt_len);
}
#endif

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
    BD_ADDR  bda;

    STREAM_TO_BDADDR (bda, p);

    /* Tell L2CAP that there was a PIN code request,  */
    /* it may need to stretch timeouts                */
    l2c_pin_code_request (bda);

    btm_sec_pin_code_request (bda);
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
static void btu_hcif_conn_pkt_type_change_evt (void)
{
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
#if (SMP_INCLUDED == TRUE)
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
#endif  ///SMP_INCLUDED == TRUE


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
** Function         btu_hcif_simple_pair_complete_evt
**
** Description      Process event HCI_SIMPLE_PAIRING_COMPLETE_EVT
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
static void btu_hcif_simple_pair_complete_evt (UINT8 *p)
{
    btm_simple_pair_complete(p);
}
#endif  ///SMP_INCLUDED == TRUE
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

static void btu_ble_process_adv_pkt (UINT8 *p)
{
    HCI_TRACE_DEBUG("btu_ble_process_adv_pkt\n");

    btm_ble_process_adv_pkt(p);
}

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

