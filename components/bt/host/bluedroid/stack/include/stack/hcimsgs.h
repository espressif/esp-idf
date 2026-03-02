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

#ifndef HCIMSGS_H
#define HCIMSGS_H

#include <stddef.h>
#include "common/bt_target.h"
#include "stack/hcidefs.h"
#include "stack/bt_types.h"
#include "osi/pkt_queue.h"
#include "osi/allocator.h"

#define HCI_CMD_BUF_TYPE_METADATA       (0xa56e)

#define HCI_CMD_MSG_F_SRC_NOACK         (0x01)

typedef void (*hci_cmd_cmpl_cb)(BT_HDR *response, void *context);
typedef void (*hci_cmd_stat_cb)(uint8_t status, BT_HDR *command, void *context);
typedef void (*hci_cmd_free_cb)(pkt_linked_item_t *linked_pkt);

typedef struct {
    uint8_t flags_src;
    uint8_t flags_vnd; // used for downstream layer
    uint16_t opcode;
    hci_cmd_cmpl_cb command_complete_cb;
    hci_cmd_stat_cb command_status_cb;
    void *context;
    void *complete_future;
    hci_cmd_free_cb command_free_cb;
    BT_HDR command;
} hci_cmd_metadata_t;

#define HCI_CMD_METADATA_HDR_SIZE       (sizeof(hci_cmd_metadata_t))

#define HCI_CMD_LINKED_BUF_SIZE(paramlen)      (BT_PKT_LINKED_HDR_SIZE + HCI_CMD_METADATA_HDR_SIZE + HCIC_PREAMBLE_SIZE + (paramlen))

#define HCI_GET_CMD_METAMSG(cmd_ptr)  (hci_cmd_metadata_t *)((void *)(cmd_ptr) - offsetof(hci_cmd_metadata_t, command))
#define HCI_GET_CMD_LINKED_STRUCT(metadata_ptr) (pkt_linked_item_t *)((void *)(metadata_ptr) - offsetof(pkt_linked_item_t, data))

static inline BT_HDR *hci_get_cmd_buf(size_t param_len)
{
    pkt_linked_item_t *linked_pkt = osi_calloc(HCI_CMD_LINKED_BUF_SIZE(param_len));
    if (linked_pkt == NULL) {
        return NULL;
    }
    hci_cmd_metadata_t *metadata = (hci_cmd_metadata_t *)linked_pkt->data;
    BT_HDR *command = &metadata->command;

    command->layer_specific = HCI_CMD_BUF_TYPE_METADATA;
    command->len = HCIC_PREAMBLE_SIZE + param_len;
    command->offset = 0;

    return command;
}

static inline void hci_free_cmd_buf(BT_HDR *buf)
{
    assert(buf->layer_specific == HCI_CMD_BUF_TYPE_METADATA);
    hci_cmd_metadata_t *metadata = HCI_GET_CMD_METAMSG(buf);
    pkt_linked_item_t *linked_pkt = HCI_GET_CMD_LINKED_STRUCT(metadata);
    osi_free(linked_pkt);
}

#define HCI_GET_CMD_BUF(param_len)       hci_get_cmd_buf(param_len)
#define HCI_FREE_CMD_BUF(buf)            hci_free_cmd_buf(buf)

void bte_main_hci_send(BT_HDR *p_msg, UINT16 event);
void bte_main_lpm_allow_bt_device_sleep(void);

/* Message by message.... */

BOOLEAN btsnd_hcic_inquiry(const LAP inq_lap, UINT8 duration,
                           UINT8 response_cnt);

#define HCIC_PARAM_SIZE_INQUIRY 5


#define HCIC_INQ_INQ_LAP_OFF    0
#define HCIC_INQ_DUR_OFF        3
#define HCIC_INQ_RSP_CNT_OFF    4
/* Inquiry */

/* Inquiry Cancel */
BOOLEAN btsnd_hcic_inq_cancel(void);

#define HCIC_PARAM_SIZE_INQ_CANCEL   0

/* Periodic Inquiry Mode */
BOOLEAN btsnd_hcic_per_inq_mode(UINT16 max_period, UINT16 min_period,
                                const LAP inq_lap, UINT8 duration,
                                UINT8 response_cnt);

#define HCIC_PARAM_SIZE_PER_INQ_MODE    9

#define HCI_PER_INQ_MAX_INTRVL_OFF  0
#define HCI_PER_INQ_MIN_INTRVL_OFF  2
#define HCI_PER_INQ_INQ_LAP_OFF     4
#define HCI_PER_INQ_DURATION_OFF    7
#define HCI_PER_INQ_RSP_CNT_OFF     8
/* Periodic Inquiry Mode */

/* Exit Periodic Inquiry Mode */
BOOLEAN btsnd_hcic_exit_per_inq(void);

#define HCIC_PARAM_SIZE_EXIT_PER_INQ   0
/* Create Connection */
BOOLEAN btsnd_hcic_create_conn(BD_ADDR dest, UINT16 packet_types,
                               UINT8 page_scan_rep_mode,
                               UINT8 page_scan_mode,
                               UINT16 clock_offset,
                               UINT8 allow_switch);

#define HCIC_PARAM_SIZE_CREATE_CONN  13

#define HCIC_CR_CONN_BD_ADDR_OFF        0
#define HCIC_CR_CONN_PKT_TYPES_OFF      6
#define HCIC_CR_CONN_REP_MODE_OFF       8
#define HCIC_CR_CONN_PAGE_SCAN_MODE_OFF 9
#define HCIC_CR_CONN_CLK_OFF_OFF        10
#define HCIC_CR_CONN_ALLOW_SWITCH_OFF   12
/* Create Connection */

/* Disconnect */
BOOLEAN btsnd_hcic_disconnect(UINT16 handle, UINT8 reason);

#define HCIC_PARAM_SIZE_DISCONNECT 3

#define HCI_DISC_HANDLE_OFF             0
#define HCI_DISC_REASON_OFF             2
/* Disconnect */

#if BTM_SCO_INCLUDED == TRUE
/* Add SCO Connection */
BOOLEAN btsnd_hcic_add_SCO_conn (UINT16 handle, UINT16 packet_types);
#endif /* BTM_SCO_INCLUDED */

#define HCIC_PARAM_SIZE_ADD_SCO_CONN    4

#define HCI_ADD_SCO_HANDLE_OFF          0
#define HCI_ADD_SCO_PACKET_TYPES_OFF    2
/* Add SCO Connection */

/* Create Connection Cancel */
BOOLEAN btsnd_hcic_create_conn_cancel(BD_ADDR dest);

#define HCIC_PARAM_SIZE_CREATE_CONN_CANCEL  6

#define HCIC_CR_CONN_CANCEL_BD_ADDR_OFF     0
/* Create Connection Cancel */

/* Accept Connection Request */
BOOLEAN btsnd_hcic_accept_conn (BD_ADDR bd_addr, UINT8 role);

#define HCIC_PARAM_SIZE_ACCEPT_CONN     7

#define HCI_ACC_CONN_BD_ADDR_OFF        0
#define HCI_ACC_CONN_ROLE_OFF           6
/* Accept Connection Request */

/* Reject Connection Request */
BOOLEAN btsnd_hcic_reject_conn (BD_ADDR bd_addr, UINT8 reason);

#define HCIC_PARAM_SIZE_REJECT_CONN      7

#define HCI_REJ_CONN_BD_ADDR_OFF        0
#define HCI_REJ_CONN_REASON_OFF         6
/* Reject Connection Request */

/* Link Key Request Reply */
BOOLEAN btsnd_hcic_link_key_req_reply (BD_ADDR bd_addr,
                                       LINK_KEY link_key);

#define HCIC_PARAM_SIZE_LINK_KEY_REQ_REPLY   22

#define HCI_LINK_KEY_REPLY_BD_ADDR_OFF  0
#define HCI_LINK_KEY_REPLY_LINK_KEY_OFF 6
/* Link Key Request Reply  */

/* Link Key Request Neg Reply */
BOOLEAN btsnd_hcic_link_key_neg_reply (BD_ADDR bd_addr);

#define HCIC_PARAM_SIZE_LINK_KEY_NEG_REPLY   6

#define HCI_LINK_KEY_NEG_REP_BD_ADR_OFF 0
/* Link Key Request Neg Reply  */

/* PIN Code Request Reply */
BOOLEAN btsnd_hcic_pin_code_req_reply (BD_ADDR bd_addr,
                                       UINT8 pin_code_len,
                                       PIN_CODE pin_code);

#define HCIC_PARAM_SIZE_PIN_CODE_REQ_REPLY   23

#define HCI_PIN_CODE_REPLY_BD_ADDR_OFF  0
#define HCI_PIN_CODE_REPLY_PIN_LEN_OFF  6
#define HCI_PIN_CODE_REPLY_PIN_CODE_OFF 7
/* PIN Code Request Reply  */

/* Link Key Request Neg Reply */
BOOLEAN btsnd_hcic_pin_code_neg_reply (BD_ADDR bd_addr);

#define HCIC_PARAM_SIZE_PIN_CODE_NEG_REPLY   6

#define HCI_PIN_CODE_NEG_REP_BD_ADR_OFF 0
/* Link Key Request Neg Reply  */

/* Change Connection Type */
BOOLEAN btsnd_hcic_change_conn_type (UINT16 handle, UINT16 packet_types);

#define HCIC_PARAM_SIZE_CHANGE_CONN_TYPE     4

#define HCI_CHNG_PKT_TYPE_HANDLE_OFF    0
#define HCI_CHNG_PKT_TYPE_PKT_TYPE_OFF  2
/* Change Connection Type */

#define HCIC_PARAM_SIZE_CMD_HANDLE      2

#define HCI_CMD_HANDLE_HANDLE_OFF       0

BOOLEAN btsnd_hcic_auth_request (UINT16 handle);     /* Authentication Request */

/* Set Connection Encryption */
BOOLEAN btsnd_hcic_set_conn_encrypt (UINT16 handle, BOOLEAN enable);
#define HCIC_PARAM_SIZE_SET_CONN_ENCRYPT     3


#define HCI_SET_ENCRYPT_HANDLE_OFF      0
#define HCI_SET_ENCRYPT_ENABLE_OFF      2
/* Set Connection Encryption */

/* Remote Name Request */
BOOLEAN btsnd_hcic_rmt_name_req (BD_ADDR bd_addr,
                                 UINT8 page_scan_rep_mode,
                                 UINT8 page_scan_mode,
                                 UINT16 clock_offset);

#define HCIC_PARAM_SIZE_RMT_NAME_REQ   10

#define HCI_RMT_NAME_BD_ADDR_OFF        0
#define HCI_RMT_NAME_REP_MODE_OFF       6
#define HCI_RMT_NAME_PAGE_SCAN_MODE_OFF 7
#define HCI_RMT_NAME_CLK_OFF_OFF        8
/* Remote Name Request */

/* Remote Name Request Cancel */
BOOLEAN btsnd_hcic_rmt_name_req_cancel(BD_ADDR bd_addr);

#define HCIC_PARAM_SIZE_RMT_NAME_REQ_CANCEL   6

#define HCI_RMT_NAME_CANCEL_BD_ADDR_OFF       0
/* Remote Name Request Cancel */

BOOLEAN btsnd_hcic_rmt_features_req(UINT16 handle);      /* Remote Features Request */

/* Remote Extended Features */
BOOLEAN btsnd_hcic_rmt_ext_features(UINT16 handle, UINT8 page_num);

#define HCIC_PARAM_SIZE_RMT_EXT_FEATURES   3

#define HCI_RMT_EXT_FEATURES_HANDLE_OFF    0
#define HCI_RMT_EXT_FEATURES_PAGE_NUM_OFF  2
/* Remote Extended Features */


BOOLEAN btsnd_hcic_rmt_ver_req(UINT16 handle);           /* Remote Version Info Request */
BOOLEAN btsnd_hcic_read_rmt_clk_offset(UINT16 handle);   /* Remote Clock Offset */
BOOLEAN btsnd_hcic_read_lmp_handle(UINT16 handle);       /* Remote LMP Handle */

BOOLEAN btsnd_hcic_setup_esco_conn (UINT16 handle,
                                    UINT32 tx_bw, UINT32 rx_bw,
                                    UINT16 max_latency, UINT16 voice,
                                    UINT8 retrans_effort,
                                    UINT16 packet_types);
#define HCIC_PARAM_SIZE_SETUP_ESCO      17

#define HCI_SETUP_ESCO_HANDLE_OFF       0
#define HCI_SETUP_ESCO_TX_BW_OFF        2
#define HCI_SETUP_ESCO_RX_BW_OFF        6
#define HCI_SETUP_ESCO_MAX_LAT_OFF      10
#define HCI_SETUP_ESCO_VOICE_OFF        12
#define HCI_SETUP_ESCO_RETRAN_EFF_OFF   14
#define HCI_SETUP_ESCO_PKT_TYPES_OFF    15


BOOLEAN btsnd_hcic_accept_esco_conn (BD_ADDR bd_addr,
                                     UINT32 tx_bw, UINT32 rx_bw,
                                     UINT16 max_latency,
                                     UINT16 content_fmt,
                                     UINT8 retrans_effort,
                                     UINT16 packet_types);
#define HCIC_PARAM_SIZE_ACCEPT_ESCO     21

#define HCI_ACCEPT_ESCO_BDADDR_OFF      0
#define HCI_ACCEPT_ESCO_TX_BW_OFF       6
#define HCI_ACCEPT_ESCO_RX_BW_OFF       10
#define HCI_ACCEPT_ESCO_MAX_LAT_OFF     14
#define HCI_ACCEPT_ESCO_VOICE_OFF       16
#define HCI_ACCEPT_ESCO_RETRAN_EFF_OFF  18
#define HCI_ACCEPT_ESCO_PKT_TYPES_OFF   19


BOOLEAN btsnd_hcic_reject_esco_conn (BD_ADDR bd_addr, UINT8 reason);
#define HCIC_PARAM_SIZE_REJECT_ESCO     7

#define HCI_REJECT_ESCO_BDADDR_OFF      0
#define HCI_REJECT_ESCO_REASON_OFF      6

/* Hold Mode */
BOOLEAN btsnd_hcic_hold_mode(UINT16 handle, UINT16 max_hold_period,
                             UINT16 min_hold_period);

#define HCIC_PARAM_SIZE_HOLD_MODE       6

#define HCI_HOLD_MODE_HANDLE_OFF        0
#define HCI_HOLD_MODE_MAX_PER_OFF       2
#define HCI_HOLD_MODE_MIN_PER_OFF       4
/* Hold Mode */

/**
 * Sniff Mode
 * sniff_attempt should no more than 0xFF
 */
BOOLEAN btsnd_hcic_sniff_mode(UINT16 handle,
                              UINT16 max_sniff_period,
                              UINT16 min_sniff_period,
                              UINT16 sniff_attempt,
                              UINT16 sniff_timeout);

#define HCIC_PARAM_SIZE_SNIFF_MODE      10


#define HCI_SNIFF_MODE_HANDLE_OFF       0
#define HCI_SNIFF_MODE_MAX_PER_OFF      2
#define HCI_SNIFF_MODE_MIN_PER_OFF      4
#define HCI_SNIFF_MODE_ATTEMPT_OFF      6
#define HCI_SNIFF_MODE_TIMEOUT_OFF      8
/* Sniff Mode */

BOOLEAN btsnd_hcic_exit_sniff_mode(UINT16 handle);       /* Exit Sniff Mode */

/* Park Mode */
BOOLEAN btsnd_hcic_park_mode (UINT16 handle,
                              UINT16 beacon_max_interval,
                              UINT16 beacon_min_interval);

#define HCIC_PARAM_SIZE_PARK_MODE       6

#define HCI_PARK_MODE_HANDLE_OFF        0
#define HCI_PARK_MODE_MAX_PER_OFF       2
#define HCI_PARK_MODE_MIN_PER_OFF       4
/* Park Mode */

BOOLEAN btsnd_hcic_exit_park_mode(UINT16 handle);  /* Exit Park Mode */

/* QoS Setup */
BOOLEAN btsnd_hcic_qos_setup (UINT16 handle, UINT8 flags,
                              UINT8 service_type,
                              UINT32 token_rate, UINT32 peak,
                              UINT32 latency, UINT32 delay_var);

#define HCIC_PARAM_SIZE_QOS_SETUP       20

#define HCI_QOS_HANDLE_OFF              0
#define HCI_QOS_FLAGS_OFF               2
#define HCI_QOS_SERVICE_TYPE_OFF        3
#define HCI_QOS_TOKEN_RATE_OFF          4
#define HCI_QOS_PEAK_BANDWIDTH_OFF      8
#define HCI_QOS_LATENCY_OFF             12
#define HCI_QOS_DELAY_VAR_OFF           16
/* QoS Setup */

/* Switch Role Request */
BOOLEAN btsnd_hcic_switch_role (BD_ADDR bd_addr, UINT8 role);

#define HCIC_PARAM_SIZE_SWITCH_ROLE  7

#define HCI_SWITCH_BD_ADDR_OFF          0
#define HCI_SWITCH_ROLE_OFF             6
/* Switch Role Request */

/* Write Policy Settings */
BOOLEAN btsnd_hcic_write_policy_set(UINT16 handle, UINT16 settings);

#define HCIC_PARAM_SIZE_WRITE_POLICY_SET     4

#define HCI_WRITE_POLICY_HANDLE_OFF          0
#define HCI_WRITE_POLICY_SETTINGS_OFF        2
/* Write Policy Settings */

/* Write Default Policy Settings */
BOOLEAN btsnd_hcic_write_def_policy_set(UINT16 settings);

#define HCIC_PARAM_SIZE_WRITE_DEF_POLICY_SET     2

#define HCI_WRITE_DEF_POLICY_SETTINGS_OFF        0
/* Write Default Policy Settings */

/******************************************
**    Lisbon Features
*******************************************/
#if BTM_SSR_INCLUDED == TRUE
/* Sniff Subrating */
BOOLEAN btsnd_hcic_sniff_sub_rate(UINT16 handle, UINT16 max_lat,
                                  UINT16 min_remote_lat,
                                  UINT16 min_local_lat);

#define HCIC_PARAM_SIZE_SNIFF_SUB_RATE             8

#define HCI_SNIFF_SUB_RATE_HANDLE_OFF              0
#define HCI_SNIFF_SUB_RATE_MAX_LAT_OFF             2
#define HCI_SNIFF_SUB_RATE_MIN_REM_LAT_OFF         4
#define HCI_SNIFF_SUB_RATE_MIN_LOC_LAT_OFF         6
/* Sniff Subrating */

#else   /* BTM_SSR_INCLUDED == FALSE */

#define btsnd_hcic_sniff_sub_rate(handle, max_lat, min_remote_lat, min_local_lat) FALSE

#endif  /* BTM_SSR_INCLUDED */

/* Extended Inquiry Response */
void btsnd_hcic_write_ext_inquiry_response(BT_HDR *buffer, UINT8 fec_req);

#define HCIC_PARAM_SIZE_EXT_INQ_RESP        241

#define HCIC_EXT_INQ_RESP_FEC_OFF     0
#define HCIC_EXT_INQ_RESP_RESPONSE    1
/* IO Capabilities Response */
BOOLEAN btsnd_hcic_io_cap_req_reply (BD_ADDR bd_addr, UINT8 capability,
                                     UINT8 oob_present, UINT8 auth_req);

#define HCIC_PARAM_SIZE_IO_CAP_RESP     9

#define HCI_IO_CAP_BD_ADDR_OFF          0
#define HCI_IO_CAPABILITY_OFF           6
#define HCI_IO_CAP_OOB_DATA_OFF         7
#define HCI_IO_CAP_AUTH_REQ_OFF         8

/* IO Capabilities Req Neg Reply */
BOOLEAN btsnd_hcic_io_cap_req_neg_reply (BD_ADDR bd_addr, UINT8 err_code);

#define HCIC_PARAM_SIZE_IO_CAP_NEG_REPLY 7

#define HCI_IO_CAP_NR_BD_ADDR_OFF        0
#define HCI_IO_CAP_NR_ERR_CODE           6

/* Read Local OOB Data */
BOOLEAN btsnd_hcic_read_local_oob_data (void);

#define HCIC_PARAM_SIZE_R_LOCAL_OOB     0


BOOLEAN btsnd_hcic_user_conf_reply (BD_ADDR bd_addr, BOOLEAN is_yes);

#define HCIC_PARAM_SIZE_UCONF_REPLY     6

#define HCI_USER_CONF_BD_ADDR_OFF       0


BOOLEAN btsnd_hcic_user_passkey_reply (BD_ADDR bd_addr, UINT32 value);

#define HCIC_PARAM_SIZE_U_PKEY_REPLY    10

#define HCI_USER_PASSKEY_BD_ADDR_OFF    0
#define HCI_USER_PASSKEY_VALUE_OFF      6


BOOLEAN btsnd_hcic_user_passkey_neg_reply (BD_ADDR bd_addr);

#define HCIC_PARAM_SIZE_U_PKEY_NEG_REPLY 6

#define HCI_USER_PASSKEY_NEG_BD_ADDR_OFF 0

/* Remote OOB Data Request Reply */
BOOLEAN btsnd_hcic_rem_oob_reply (BD_ADDR bd_addr, UINT8 *p_c,
                                  UINT8 *p_r);

#define HCIC_PARAM_SIZE_REM_OOB_REPLY   38

#define HCI_REM_OOB_DATA_BD_ADDR_OFF    0
#define HCI_REM_OOB_DATA_C_OFF          6
#define HCI_REM_OOB_DATA_R_OFF          22

/* Remote OOB Data Request Negative Reply */
BOOLEAN btsnd_hcic_rem_oob_neg_reply (BD_ADDR bd_addr);

#define HCIC_PARAM_SIZE_REM_OOB_NEG_REPLY   6

#define HCI_REM_OOB_DATA_NEG_BD_ADDR_OFF    0

/* Read Tx Power Level */
BOOLEAN btsnd_hcic_read_inq_tx_power (void);

#define HCIC_PARAM_SIZE_R_TX_POWER      0

/* Read Default Erroneous Data Reporting */
BOOLEAN btsnd_hcic_read_default_erroneous_data_rpt (void);

#define HCIC_PARAM_SIZE_R_ERR_DATA_RPT      0

#if L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE
BOOLEAN btsnd_hcic_enhanced_flush (UINT16 handle, UINT8 packet_type);

#define HCIC_PARAM_SIZE_ENHANCED_FLUSH  3
#endif


BOOLEAN btsnd_hcic_send_keypress_notif (BD_ADDR bd_addr, UINT8 notif);

#define HCIC_PARAM_SIZE_SEND_KEYPRESS_NOTIF    7

#define HCI_SEND_KEYPRESS_NOTIF_BD_ADDR_OFF    0
#define HCI_SEND_KEYPRESS_NOTIF_NOTIF_OFF      6

/**** end of Simple Pairing Commands ****/

/* Store Current Settings */
#define MAX_FILT_COND   (sizeof (BD_ADDR) + 1)

BOOLEAN btsnd_hcic_set_event_filter(UINT8 filt_type,
                                    UINT8 filt_cond_type,
                                    UINT8 *filt_cond,
                                    UINT8 filt_cond_len);

#define HCIC_PARAM_SIZE_SET_EVT_FILTER  9

#define HCI_FILT_COND_FILT_TYPE_OFF     0
#define HCI_FILT_COND_COND_TYPE_OFF     1
#define HCI_FILT_COND_FILT_OFF          2
/* Set Event Filter */

/* Delete Stored Key */
BOOLEAN btsnd_hcic_delete_stored_key (BD_ADDR bd_addr, BOOLEAN delete_all_flag);

#define HCIC_PARAM_SIZE_DELETE_STORED_KEY        7

#define HCI_DELETE_KEY_BD_ADDR_OFF      0
#define HCI_DELETE_KEY_ALL_FLAG_OFF     6
/* Delete Stored Key */

/* Change Local Name */
BOOLEAN btsnd_hcic_change_name (const UINT8 *name);

#define HCIC_PARAM_SIZE_CHANGE_NAME     BD_NAME_LEN

#define HCI_CHANGE_NAME_NAME_OFF        0
/* Change Local Name */


#define HCIC_PARAM_SIZE_READ_CMD     0

#define HCIC_PARAM_SIZE_WRITE_PARAM1     1

#define HCIC_WRITE_PARAM1_PARAM_OFF      0

#define HCIC_PARAM_SIZE_WRITE_PARAM2     2

#define HCIC_WRITE_PARAM2_PARAM_OFF      0

#define HCIC_PARAM_SIZE_WRITE_PARAM3     3

#define HCIC_WRITE_PARAM3_PARAM_OFF      0

/* set AFH channels */
BOOLEAN btsnd_hcic_set_afh_channels (AFH_CHANNELS channels);
#define HCIC_PARAM_SIZE_SET_AFH_CHANNELS    10
BOOLEAN btsnd_hcic_ble_set_channels (BLE_CHANNELS channels);
#define HCIC_PARAM_SIZE_BLE_SET_CHANNELS    5

/* set minimum encryption key size */
BOOLEAN btsnd_hcic_set_min_enc_key_size (UINT8 size);
#define HCIC_PARAM_SIZE_SET_MIN_ENC_KEY_SIZE    1

BOOLEAN btsnd_hcic_write_pin_type(UINT8 type);                   /* Write PIN Type */
BOOLEAN btsnd_hcic_write_auto_accept(UINT8 flag);                /* Write Auto Accept */
BOOLEAN btsnd_hcic_read_name (void);                             /* Read Local Name */
BOOLEAN btsnd_hcic_write_page_tout(UINT16 timeout);              /* Write Page Timeout */
BOOLEAN btsnd_hcic_write_scan_enable(UINT8 flag);                /* Write Scan Enable */
BOOLEAN btsnd_hcic_write_pagescan_cfg(UINT16 interval,
                                      UINT16 window);            /* Write Page Scan Activity */

#define HCIC_PARAM_SIZE_WRITE_PAGESCAN_CFG  4

#define HCI_SCAN_CFG_INTERVAL_OFF       0
#define HCI_SCAN_CFG_WINDOW_OFF         2
/* Write Page Scan Activity */

/* Write Inquiry Scan Activity */
BOOLEAN btsnd_hcic_write_inqscan_cfg(UINT16 interval, UINT16 window);

#define HCIC_PARAM_SIZE_WRITE_INQSCAN_CFG    4

#define HCI_SCAN_CFG_INTERVAL_OFF       0
#define HCI_SCAN_CFG_WINDOW_OFF         2
/* Write Inquiry Scan Activity */

BOOLEAN btsnd_hcic_write_auth_enable(UINT8 flag);                 /* Write Authentication Enable */
BOOLEAN btsnd_hcic_write_dev_class(DEV_CLASS dev);                /* Write Class of Device */
BOOLEAN btsnd_hcic_write_voice_settings(UINT16 flags);            /* Write Voice Settings */

/* Host Controller to Host flow control */
#define HCI_HOST_FLOW_CTRL_OFF          0
#define HCI_HOST_FLOW_CTRL_ACL_ON       1
#define HCI_HOST_FLOW_CTRL_SCO_ON       2
#define HCI_HOST_FLOW_CTRL_BOTH_ON      3

#define  HCI_HOST_FLOW_CTRL_ADV_REPORT_OFF     0
#define  HCI_HOST_FLOW_CTRL_ADV_REPORT_ON      1


BOOLEAN btsnd_hcic_write_auto_flush_tout(UINT16 handle,
        UINT16 timeout);    /* Write Retransmit Timeout */

#define HCIC_PARAM_SIZE_WRITE_AUTO_FLUSH_TOUT    4

#define HCI_FLUSH_TOUT_HANDLE_OFF       0
#define HCI_FLUSH_TOUT_TOUT_OFF         2

BOOLEAN btsnd_hcic_read_tx_power(UINT16 handle, UINT8 type);     /* Read Tx Power */

#define HCIC_PARAM_SIZE_READ_TX_POWER    3

#define HCI_READ_TX_POWER_HANDLE_OFF    0
#define HCI_READ_TX_POWER_TYPE_OFF      2

/* Read transmit power level parameter */
#define HCI_READ_CURRENT                0x00
#define HCI_READ_MAXIMUM                0x01

BOOLEAN btsnd_hcic_host_num_xmitted_pkts (UINT8 num_handles,
        UINT16 *handle,
        UINT16 *num_pkts);         /* Set Host Buffer Size */

#define HCIC_PARAM_SIZE_NUM_PKTS_DONE_SIZE    sizeof(btmsg_hcic_num_pkts_done_t)

#define MAX_DATA_HANDLES        10

#define HCI_PKTS_DONE_NUM_HANDLES_OFF   0
#define HCI_PKTS_DONE_HANDLE_OFF        1
#define HCI_PKTS_DONE_NUM_PKTS_OFF      3

/* Write Link Supervision Timeout */
BOOLEAN btsnd_hcic_write_link_super_tout(UINT8 local_controller_id, UINT16 handle, UINT16 timeout);

#define HCIC_PARAM_SIZE_WRITE_LINK_SUPER_TOUT        4

#define HCI_LINK_SUPER_TOUT_HANDLE_OFF  0
#define HCI_LINK_SUPER_TOUT_TOUT_OFF    2
/* Write Link Supervision Timeout */

BOOLEAN btsnd_hcic_write_cur_iac_lap (UINT8 num_cur_iac,
                                      LAP *const iac_lap);   /* Write Current IAC LAP */

#define MAX_IAC_LAPS    0x40

#define HCI_WRITE_IAC_LAP_NUM_OFF       0
#define HCI_WRITE_IAC_LAP_LAP_OFF       1
/* Write Current IAC LAP */

BOOLEAN btsnd_hcic_get_link_quality (UINT16 handle);            /* Get Link Quality */
BOOLEAN btsnd_hcic_read_rssi (UINT16 handle);                   /* Read RSSI */
BOOLEAN btsnd_hcic_enable_test_mode (void);                     /* Enable Device Under Test Mode */
BOOLEAN btsnd_hcic_write_pagescan_type(UINT8 type);             /* Write Page Scan Type */
BOOLEAN btsnd_hcic_write_inqscan_type(UINT8 type);              /* Write Inquiry Scan Type */
BOOLEAN btsnd_hcic_write_inquiry_mode(UINT8 type);              /* Write Inquiry Mode */

#define HCI_DATA_HANDLE_MASK 0x0FFF

#define HCID_GET_HANDLE_EVENT(p)  (UINT16)((*((UINT8 *)((p) + 1) + p->offset) + \
                                           (*((UINT8 *)((p) + 1) + p->offset + 1) << 8)))

#define HCID_GET_HANDLE(u16) (UINT16)((u16) & HCI_DATA_HANDLE_MASK)

#define HCI_DATA_EVENT_MASK   3
#define HCI_DATA_EVENT_OFFSET 12
#define HCID_GET_EVENT(u16)   (UINT8)(((u16) >> HCI_DATA_EVENT_OFFSET) & HCI_DATA_EVENT_MASK)

#define HCI_DATA_BCAST_MASK   3
#define HCI_DATA_BCAST_OFFSET 10
#define HCID_GET_BCAST(u16)   (UINT8)(((u16) >> HCI_DATA_BCAST_OFFSET) & HCI_DATA_BCAST_MASK)

#define HCID_GET_ACL_LEN(p)     (UINT16)((*((UINT8 *)((p) + 1) + p->offset + 2) + \
                                         (*((UINT8 *)((p) + 1) + p->offset + 3) << 8)))

#define HCID_HEADER_SIZE      4

#define HCID_GET_SCO_LEN(p)  (*((UINT8 *)((p) + 1) + p->offset + 2))

void btsnd_hcic_vendor_spec_cmd (BT_HDR *buffer, UINT16 opcode,
                                 UINT8 len, UINT8 *p_data,
                                 void *p_cmd_cplt_cback);

#if (BLE_INCLUDED == TRUE)
/********************************************************************************
** BLE Commands
**      Note: "local_controller_id" is for transport, not counted in HCI message size
*********************************************************************************/
#define HCIC_BLE_RAND_DI_SIZE                   8
#define HCIC_BLE_ENCRYT_KEY_SIZE                16
#define HCIC_BLE_IRK_SIZE                       16

#define HCIC_PARAM_SIZE_SET_USED_FEAT_CMD       8
#define HCIC_PARAM_SIZE_WRITE_RANDOM_ADDR_CMD    6
#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS    15
#if (BLE_50_FEATURE_SUPPORT == TRUE)
#define HCIC_PARAM_SIZE_BLE_WRITE_EXT_ADV_PARAMS 25
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#define HCIC_PARAM_SIZE_BLE_WRITE_SCAN_RSP      31
#define HCIC_PARAM_SIZE_WRITE_ADV_ENABLE        1
#define HCIC_PARAM_SIZE_BLE_WRITE_SCAN_PARAM    7
#define HCIC_PARAM_SIZE_BLE_WRITE_SCAN_ENABLE   2
#define HCIC_PARAM_SIZE_BLE_CREATE_LL_CONN      25
#define HCIC_PARAM_SIZE_BLE_CREATE_CONN_CANCEL  0
#define HCIC_PARAM_SIZE_CLEAR_WHITE_LIST        0
#define HCIC_PARAM_SIZE_ADD_WHITE_LIST          7
#define HCIC_PARAM_SIZE_REMOVE_WHITE_LIST       7
#define HCIC_PARAM_SIZE_BLE_UPD_LL_CONN_PARAMS  14
#define HCIC_PARAM_SIZE_SET_HOST_CHNL_CLASS     5
#define HCIC_PARAM_SIZE_READ_CHNL_MAP         2
#define HCIC_PARAM_SIZE_BLE_READ_REMOTE_FEAT    2
#define HCIC_PARAM_SIZE_BLE_ENCRYPT             32
#define HCIC_PARAM_SIZE_BLE_RAND                0
#define HCIC_PARAM_SIZE_WRITE_LE_HOST_SUPPORTED 2

#define HCIC_BLE_RAND_DI_SIZE                   8
#define HCIC_BLE_ENCRYT_KEY_SIZE                16
#define HCIC_PARAM_SIZE_BLE_START_ENC           (4 + HCIC_BLE_RAND_DI_SIZE + HCIC_BLE_ENCRYT_KEY_SIZE)
#define HCIC_PARAM_SIZE_LTK_REQ_REPLY           (2 + HCIC_BLE_ENCRYT_KEY_SIZE)
#define HCIC_PARAM_SIZE_LTK_REQ_NEG_REPLY       2
#define HCIC_BLE_CHNL_MAP_SIZE                  5
#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA      31

#define HCIC_PARAM_SIZE_BLE_ADD_DEV_RESOLVING_LIST      (7 + HCIC_BLE_IRK_SIZE * 2)
#define HCIC_PARAM_SIZE_BLE_RM_DEV_RESOLVING_LIST       7
#define HCIC_PARAM_SIZE_BLE_CLEAR_RESOLVING_LIST        0
#define HCIC_PARAM_SIZE_BLE_READ_RESOLVING_LIST_SIZE    0
#define HCIC_PARAM_SIZE_BLE_READ_RESOLVABLE_ADDR_PEER   7
#define HCIC_PARAM_SIZE_BLE_READ_RESOLVABLE_ADDR_LOCAL  7
#define HCIC_PARAM_SIZE_BLE_SET_ADDR_RESOLUTION_ENABLE  1
#define HCIC_PARAM_SIZE_BLE_SET_RAND_PRIV_ADDR_TIMOUT   2
#define HCIC_PARAM_SIZE_BLE_SET_DATA_LENGTH             6
#define HCIC_PARAM_SIZE_BLE_WRITE_EXTENDED_SCAN_PARAM   11
#define HCIC_PARAM_SIZE_BLE_UPDATE_ADV_FLOW_CONTROL     2
#define HCIC_PARAM_SIZE_BLE_CLEAR_ADV                   0
#define HCIC_PARAM_SIZE_SET_PRIVACY_MODE                8
#define HCIC_PARAM_SIZE_BLE_SET_CSA_SUPPORT             1
#define HCIC_PARAM_SIZE_BLE_SET_VENDOR_EVT_MASK         4
#if (BLE_50_FEATURE_SUPPORT == TRUE)
#define HCIC_PARAM_SIZE_BLE_READ_PHY                   2
#define HCIC_PARAM_SIZE_BLE_SET_DEF_PHY                3
#define HCIC_PARAM_SIZE_BLE_SET_PHY                    7
#endif
#if (BLE_50_DTM_TEST_EN == TRUE)
#define HCIC_PARAM_SIZE_ENH_RX_TEST                    3
#define HCIC_PARAM_SIZE_ENH_TX_TEST                    4
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)
#if (BLE_50_FEATURE_SUPPORT == TRUE)
#define HCIC_PARAM_SIZE_EXT_RAND_ADDR                  7
#define HCIC_PARAM_SIZE_EXT_ADV_SET_PARAMS             25
#define HCIC_PARAM_SIZE_EXT_ADV_WRITE_DATA             251
#define HCIC_PARAM_SIZE_READ_MAX_ADV_SIZE              0
#define HCIC_PARAM_SIZE_NUM_SUPPORT_ADV_SET            0
#define HCIC_PARAM_SIZE_REMOVE_ADV_SET                 1
#define HCIC_PARAM_SIZE_CLEAR_ADV_SET                  0
#define HCIC_PARAM_SIZE_SET_PERIODIC_ADV_PARAMS        7
#define HCIC_PARAM_SIZE_WRITE_PERIODIC_ADV_DATA        252
#define HCIC_PARAM_SIZE_PERIODIC_ADV_ENABLE            2
#define HCIC_PARAM_SIZE_SET_EXT_SCAN_PARAMS            3
#define HCIC_PARAM_SIZE_EXT_SCAN_ENABLE                6
#define HCIC_PARAM_SIZE_EXT_CONN_CREATE_BASE           10
#define HCIC_PARAM_SIZE_PERIODIC_ADV_CREATE_SYNC       12
#define HCIC_PARAM_SIZE_PERIODIC_ADV_CREATE_SYNC_CANCEL 0
#define HCIC_PARAM_SIZE_PERIODIC_ADV_TERM_SYNC         2
#define HCIC_PARAM_SIZE_ADD_DEV_TO_PERIODIC_ADV_LIST   8
#define HCIC_PARAM_SIZE_RM_DEV_FROM_PERIODIC_ADV_LIST  8
#define HCIC_PARAM_SIZE_CLEAR_PERIODIC_ADV_LIST        0
#define HCIC_PARAM_SIZE_READ_PERIODIC_ADV_LIST         0
#define HCIC_PARAM_SIZE_READ_TRANS_POWER               0
#define HCIC_PARAM_SIZE_READ_RF_PATH_COMPENSATION      0
#define HCIC_PARAM_SIZE_WRITE_RF_PATH_COMPENSATION     4

BlE_SYNC *btsnd_hcic_ble_get_sync_info(void);
void btsnd_hcic_ble_sync_sem_init(void);
void btsnd_hcic_ble_sync_sem_deinit(void);

uint8_t btsnd_hcic_ble_get_status(void);

void btsnd_hci_ble_set_status(UINT8 hci_status);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

/* ULP HCI command */
BOOLEAN btsnd_hcic_ble_set_evt_mask (BT_EVENT_MASK event_mask);

BOOLEAN btsnd_hcic_ble_read_buffer_size (void);

BOOLEAN btsnd_hcic_ble_read_local_spt_feat (void);

BOOLEAN btsnd_hcic_ble_set_local_used_feat (UINT8 feat_set[8]);

BOOLEAN btsnd_hcic_ble_set_random_addr (BD_ADDR random_addr);

BOOLEAN btsnd_hcic_ble_write_adv_params (UINT16 adv_int_min, UINT16 adv_int_max,
        UINT8 adv_type, UINT8 addr_type_own,
        UINT8 addr_type_dir, BD_ADDR direct_bda,
        UINT8 channel_map, UINT8 adv_filter_policy);

BOOLEAN btsnd_hcic_ble_read_adv_chnl_tx_power (void);

BOOLEAN btsnd_hcic_ble_set_adv_data (UINT8 data_len, UINT8 *p_data);

BOOLEAN btsnd_hcic_ble_set_scan_rsp_data (UINT8 data_len, UINT8 *p_scan_rsp);

BOOLEAN btsnd_hcic_ble_set_adv_enable (UINT8 adv_enable);

BOOLEAN btsnd_hcic_ble_set_scan_params (UINT8 scan_type,
                                        UINT16 scan_int, UINT16 scan_win,
                                        UINT8 addr_type, UINT8 scan_filter_policy);

BOOLEAN btsnd_hcic_ble_set_scan_enable (UINT8 scan_enable, UINT8 duplicate);

BOOLEAN btsnd_hcic_ble_create_ll_conn (UINT16 scan_int, UINT16 scan_win,
                                       UINT8 init_filter_policy, UINT8 addr_type_peer, BD_ADDR bda_peer, UINT8 addr_type_own,
                                       UINT16 conn_int_min, UINT16 conn_int_max, UINT16 conn_latency, UINT16 conn_timeout,
                                       UINT16 min_ce_len, UINT16 max_ce_len);

BOOLEAN btsnd_hcic_ble_create_conn_cancel (void);

BOOLEAN btsnd_hcic_ble_read_white_list_size (void);

BOOLEAN btsnd_hcic_ble_clear_white_list (void);

BOOLEAN btsnd_hcic_ble_add_white_list (UINT8 addr_type, BD_ADDR bda);

BOOLEAN btsnd_hcic_ble_remove_from_white_list (UINT8 addr_type, BD_ADDR bda);

BOOLEAN btsnd_hcic_ble_upd_ll_conn_params (UINT16 handle, UINT16 conn_int_min, UINT16 conn_int_max,
        UINT16 conn_latency, UINT16 conn_timeout, UINT16 min_len, UINT16 max_len);

BOOLEAN btsnd_hcic_ble_set_host_chnl_class (UINT8 chnl_map[HCIC_BLE_CHNL_MAP_SIZE]);

BOOLEAN btsnd_hcic_ble_read_chnl_map (UINT16 handle);

BOOLEAN btsnd_hcic_ble_read_remote_feat ( UINT16 handle);

BOOLEAN btsnd_hcic_ble_encrypt (UINT8 *key, UINT8 key_len, UINT8 *plain_text, UINT8 pt_len, void *p_cmd_cplt_cback);

BOOLEAN btsnd_hcic_ble_rand (void *p_cmd_cplt_cback);

BOOLEAN btsnd_hcic_ble_start_enc ( UINT16 handle,
                                   UINT8 rand[HCIC_BLE_RAND_DI_SIZE],
                                   UINT16 ediv, UINT8 ltk[HCIC_BLE_ENCRYT_KEY_SIZE]);

BOOLEAN btsnd_hcic_ble_ltk_req_reply (UINT16 handle, UINT8 ltk[HCIC_BLE_ENCRYT_KEY_SIZE]);

BOOLEAN btsnd_hcic_ble_ltk_req_neg_reply (UINT16 handle);

BOOLEAN btsnd_hcic_ble_read_supported_states (void);

BOOLEAN btsnd_hcic_ble_write_host_supported (UINT8 le_host_spt, UINT8 simul_le_host_spt);

BOOLEAN btsnd_hcic_ble_read_host_supported (void);

BOOLEAN btsnd_hcic_ble_receiver_test(UINT8 rx_freq);

BOOLEAN btsnd_hcic_ble_transmitter_test(UINT8 tx_freq, UINT8 test_data_len,
                                        UINT8 payload);
BOOLEAN btsnd_hcic_ble_test_end(void);

#if (defined BLE_LLT_INCLUDED) && (BLE_LLT_INCLUDED == TRUE)

#define HCIC_PARAM_SIZE_BLE_RC_PARAM_REQ_REPLY           14
BOOLEAN btsnd_hcic_ble_rc_param_req_reply(UINT16 handle,
        UINT16 conn_int_min, UINT16 conn_int_max,
        UINT16 conn_latency, UINT16 conn_timeout,
        UINT16 min_ce_len, UINT16 max_ce_len);

#define HCIC_PARAM_SIZE_BLE_RC_PARAM_REQ_NEG_REPLY       3
BOOLEAN btsnd_hcic_ble_rc_param_req_neg_reply(UINT16 handle, UINT8 reason);

#endif /* BLE_LLT_INCLUDED */

BOOLEAN btsnd_hcic_ble_set_data_length(UINT16 conn_handle, UINT16 tx_octets,
                                       UINT16 tx_time);

BOOLEAN btsnd_hcic_ble_add_device_resolving_list (UINT8 addr_type_peer,
        BD_ADDR bda_peer,
        UINT8 irk_peer[HCIC_BLE_IRK_SIZE],
        UINT8 irk_local[HCIC_BLE_IRK_SIZE]);

BOOLEAN btsnd_hcic_ble_rm_device_resolving_list (UINT8 addr_type_peer,
        BD_ADDR bda_peer);

BOOLEAN btsnd_hcic_ble_clear_resolving_list (void);

BOOLEAN btsnd_hcic_ble_read_resolvable_addr_peer (UINT8 addr_type_peer,
        BD_ADDR bda_peer);

BOOLEAN btsnd_hcic_ble_read_resolvable_addr_local (UINT8 addr_type_peer,
        BD_ADDR bda_peer);

BOOLEAN btsnd_hcic_ble_set_addr_resolution_enable (UINT8 addr_resolution_enable);

BOOLEAN btsnd_hcic_ble_set_rand_priv_addr_timeout (UINT16 rpa_timout);

BOOLEAN btsnd_hcic_ble_clear_adv(void);

BOOLEAN btsnd_hcic_ble_set_privacy_mode(UINT8 addr_type, BD_ADDR addr, UINT8 privacy_mode);

BOOLEAN btsnd_hcic_ble_set_csa_support (UINT8 csa_select);

BOOLEAN btsnd_hcic_ble_set_vendor_evt_mask (UINT32 evt_mask);

#endif /* BLE_INCLUDED */

#if (BLE_50_FEATURE_SUPPORT == TRUE)
typedef struct {
    UINT8 scan_type;
    UINT16 scan_interval;
    UINT16 scan_window;
} tHCI_EXT_SCAN_PARAMS;

typedef struct {
    UINT16 scan_interval;
    UINT16 scan_window;
    UINT16 conn_interval_min;
    UINT16 conn_interval_max;
    UINT16 conn_latency;
    UINT16 sup_timeout;
    UINT16 min_ce_len;
    UINT16 max_ce_len;
} tHCI_ExtConnParams;

typedef struct {
#if (BT_BLE_FEAT_PAWR_EN == TRUE)
    UINT8 adv_handle;
    UINT8 subevent;
#endif // (BT_BLE_FEAT_PAWR_EN == TRUE)
    UINT8 filter_policy;
    UINT8 own_addr_type;
    UINT8 peer_addr_type;
    BD_ADDR peer_addr;
    UINT8 init_phy_mask;
    tHCI_ExtConnParams params[3];
} tHCI_CreatExtConn;
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
BOOLEAN btsnd_hcic_read_authenticated_payload_tout(UINT16 handle);

BOOLEAN btsnd_hcic_write_authenticated_payload_tout(UINT16 handle,
        UINT16 timeout);

BOOLEAN btsnd_hcic_ble_update_adv_report_flow_control (UINT16 num, BT_HDR *static_buf);
#if (BLE_50_FEATURE_SUPPORT == TRUE)
BOOLEAN btsnd_hcic_ble_read_phy(UINT16 conn_handle);

UINT8 btsnd_hcic_ble_set_prefered_default_phy(UINT8 all_phys,
                                                              UINT8 tx_phys,
                                                              UINT8 rx_phys);
BOOLEAN btsnd_hcic_ble_set_phy(UINT16 conn_handle,
                                           UINT8 all_phys, UINT8 tx_phys,
                                           UINT8 rx_phys, UINT16 phy_options);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_50_DTM_TEST_EN == TRUE)
UINT8 btsnd_hcic_ble_enhand_rx_test(UINT8 rx_channel, UINT8 phy,
                                                         UINT8 modulation_idx);

UINT8 btsnd_hcic_ble_enhand_tx_test(UINT8 tx_channel, UINT8 len,
                                                         UINT8 packect,
                                                         UINT8 phy);
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
UINT8 btsnd_hcic_ble_set_extend_rand_address(UINT8 adv_handle, BD_ADDR rand_addr);

UINT8 btsnd_hcic_ble_set_ext_adv_params(UINT8 adv_handle, UINT16 properties, UINT32 interval_min,
                                          UINT32 interval_max, UINT8 channel_map, UINT8 own_addr_type,
                                          UINT8 peer_addr_type, BD_ADDR peer_addr,
                                          UINT8 adv_filter_policy, INT8 adv_tx_power,
                                          UINT8 primary_adv_phy, UINT8 secondary_adv_max_skip,
                                          UINT8 secondary_adv_phy,
                                          UINT8 adv_sid, UINT8 scan_req_ntf_enable);

UINT8 btsnd_hcic_ble_set_ext_adv_data(UINT8 adv_handle,
                                                          UINT8 operation, UINT8 fragment_prefrence,
                                                          UINT8 data_len, UINT8 *p_data);

UINT8 btsnd_hcic_ble_set_ext_adv_scan_rsp_data(UINT8 adv_handle,
                                                          UINT8 operation, UINT8 fragment_prefrence,
                                                          UINT8 data_len, UINT8 *p_data);

UINT8 btsnd_hcic_ble_ext_adv_enable(UINT8 enable, UINT8 num_of_sets, UINT8 *adv_handle,
                                                       UINT16 *duration, UINT8 *max_adv_evt);

UINT8 btsnd_hcic_ble_read_max_adv_len(void);

UINT8 btsnd_hcic_ble_read_num_support_adv_set(void);

UINT8 btsnd_hcic_ble_remove_adv_set(UINT8 adv_handle);

UINT8 btsnd_hcic_ble_clear_adv_set(void);

UINT8 btsnd_hcic_ble_set_periodic_adv_params(UINT8 adv_handle,
                                                                     UINT16 interval_min,
                                                                     UINT16 interval_max,
                                                                     UINT16 propertics);

UINT8 btsnd_hcic_ble_set_periodic_adv_data(UINT8 adv_handle,
                                                                  UINT8 operation,
                                                                  UINT8 len,
                                                                  UINT8 *p_data);

UINT8 btsnd_hcic_ble_periodic_adv_enable(UINT8 enable, UINT8 adv_handle);

UINT8 btsnd_hcic_ble_set_ext_scan_params(UINT8 own_addr_type, UINT8 filter_policy,
                                                               UINT8 phy_mask, UINT8 phy_count,
                                                               tHCI_EXT_SCAN_PARAMS *params);

UINT8 btsnd_hcic_ble_ext_scan_enable(UINT8 enable, UINT8 filter_dups,
                                                         UINT16 duration, UINT16 period);


BOOLEAN btsnd_hcic_ble_create_ext_conn(tHCI_CreatExtConn *p_conn);

#if (BT_BLE_FEAT_PAWR_EN == TRUE)
BOOLEAN btsnd_hcic_ble_create_ext_conn_v2(tHCI_CreatExtConn *p_conn);
#endif // (BT_BLE_FEAT_PAWR_EN == TRUE)

BOOLEAN btsnd_hcic_ble_periodic_adv_create_sync(UINT8 filter_policy, UINT8 adv_sid,
                                                                       UINT8 adv_addr_type, BD_ADDR adv_addr,
                                                                       UINT16 sync_timeout, UINT8 sync_cte_type);

UINT8 btsnd_hcic_ble_periodic_adv_create_sync_cancel(void);


UINT8 btsnd_hcic_ble_periodic_adv_term_sync(UINT16 sync_handle);

UINT8 btsnd_hcic_ble_add_dev_to_periodic_adv_list(UINT8 adv_addr_type, BD_ADDR adv_addr,
                                                                             UINT8 adv_sid);
UINT8 btsnd_hcic_ble_rm_dev_from_periodic_adv_list(UINT8 adv_addr_type, BD_ADDR adv_addr,
                                                                             UINT8 adv_sid);

UINT8 btsnd_hcic_ble_clear_periodic_adv_list(void);

UINT8 btsnd_hcic_ble_read_periodic_adv_list_size(void);

UINT8 btsnd_hcic_ble_read_trans_power(void);

UINT8 btsnd_hcic_ble_read_rf_path_compensation(void);

UINT8 btsnd_hcic_ble_write_rf_path_compensation(UINT16 rf_tx_path, UINT16 rf_rx_path);

#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#define HCIC_PARAM_SIZE_WRITE_AUTHENT_PAYLOAD_TOUT  4

#define HCI__WRITE_AUTHENT_PAYLOAD_TOUT_HANDLE_OFF  0
#define HCI__WRITE_AUTHENT_PAYLOAD_TOUT_TOUT_OFF    2

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#define HCIC_PARAM_SIZE_PERIODIC_ADV_RECV_ENABLE                3
#define HCI_PERIODIC_ADV_RECV_REPORT_EN                         1
#define HCI_PERIODIC_ADV_RECV_DUP_FILTER_EN                     2
#define HCIC_PARAM_SIZE_PERIODIC_ADV_SYNC_TRANS                 6
#define HCIC_PARAM_SIZE_PERIODIC_ADV_SET_INFO_TRANS             5
#define HCIC_PARAM_SIZE_SET_PAST_PARAMS                         8
#define HCIC_PARAM_SIZE_SET_DEFAULT_PAST_PARAMS                 6

UINT8 btsnd_hcic_ble_set_periodic_adv_recv_enable(UINT16 sync_handle, UINT8 enable);

BOOLEAN btsnd_hcic_ble_periodic_adv_sync_trans(UINT16 conn_handle, UINT16 service_data, UINT16 sync_handle);

BOOLEAN btsnd_hcic_ble_periodic_adv_set_info_trans(UINT16 conn_handle, UINT16 service_data, UINT8 adv_handle);

BOOLEAN btsnd_hcic_ble_set_periodic_adv_sync_trans_params(UINT16 conn_handle, UINT8 mode, UINT16 skip, UINT16 sync_timeout, UINT8 cte_type);

UINT8 btsnd_hcic_ble_set_default_periodic_adv_sync_trans_params(UINT8 mode, UINT16 skip, UINT16 sync_timeout, UINT8 cte_type);
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

#if (BLE_FEAT_ISO_EN == TRUE)
#define HCIC_PARAM_SIZE_ISO_READ_TX_SYNC_PARAMS           2
#define HCIC_PARAM_SIZE_ISO_SET_CIG_PARAMS                15
#define HCIC_PARAM_SIZE_ISO_SET_CIG_TEST_PARAMS           15
#define HCIC_PARAM_SIZE_ISO_CREATE_CIS_PARAMS             1
#define HCIC_PARAM_SIZE_ISO_REMOVE_CIG_PARAMS             1
#define HCIC_PARAM_SIZE_ISO_ACCEPT_CIS_REQ_PARAMS         2
#define HCIC_PARAM_SIZE_ISO_REJECT_CIS_REQ_PARAMS         3
#define HCIC_PARAM_SIZE_ISO_READ_LINK_QUALITY_PARAMS      2
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#define HCIC_PARAM_SIZE_BIG_CREATE_PARAMS                 31
#define HCIC_PARAM_SIZE_BIG_CREATE_TEST_PARAMS            36
#define HCIC_PARAM_SIZE_BIG_TERMINATE_PARAMS              2
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
#define HCIC_PARAM_SIZE_BIG_SYNC_CREATE_PARAMS            55
#define HCIC_PARAM_SIZE_BIG_SYNC_TERMINATE_PARAMS         1
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
#define HCIC_PARAM_SIZE_ISO_SET_DATA_PATH_PARAMS          13
#define HCIC_PARAM_SIZE_ISO_REMOVE_DATA_PATH_PARAMS       3

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
struct ble_hci_le_cis_params {
    uint8_t cis_id;
    uint16_t max_sdu_c_to_p;
    uint16_t max_sdu_p_to_c;
    uint8_t phy_c_to_p;
    uint8_t phy_p_to_c;
    uint8_t rtn_c_to_p;
    uint8_t rtn_p_to_c;
} __attribute__((packed));
struct ble_hci_le_cis_params_test {
    uint8_t cis_id;
    uint8_t nse;
    uint16_t max_sdu_c_to_p;
    uint16_t max_sdu_p_to_c;
    uint16_t max_pdu_c_to_p;
    uint16_t max_pdu_p_to_c;
    uint8_t phy_c_to_p;
    uint8_t phy_p_to_c;
    uint8_t bn_c_to_p;
    uint8_t bn_p_to_c;
} __attribute__((packed));


struct ble_hci_cis_hdls {
    uint16_t cis_hdl;
    uint16_t acl_hdl;
}__attribute__((packed));

UINT8 btsnd_hcic_ble_iso_set_cig_params(uint8_t cig_id, uint32_t sdu_int_c_to_p, uint32_t sdu_int_p_to_c, uint8_t worse_case_SCA, uint8_t packing,
                                        uint8_t framing, uint16_t mtl_c_to_p, uint16_t mtl_p_to_c, uint8_t cis_cnt, struct ble_hci_le_cis_params *cis_params);
UINT8 btsnd_hcic_ble_iso_set_cig_params_test(uint8_t cig_id, uint32_t sdu_int_c_to_p, uint32_t sdu_int_p_to_c, uint8_t ft_c_to_p, uint8_t ft_p_to_c,
                                            uint16_t iso_interval, uint8_t worse_case_SCA, uint8_t packing, uint8_t framing, uint8_t cis_cnt,
                                            struct ble_hci_le_cis_params_test *cis_params_test);
UINT8 btsnd_hcic_ble_iso_create_cis(uint8_t cis_count, struct ble_hci_cis_hdls *cis_hdls);
UINT8 btsnd_hcic_ble_iso_remove_cig(uint8_t cig_id);
#endif // (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
UINT8 btsnd_hcic_ble_iso_accept_cis_req(uint16_t cis_handle);
UINT8 btsnd_hcic_ble_iso_reject_cis_req(uint16_t cis_handle, uint8_t reason);
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
UINT8 btsnd_hcic_ble_big_create(uint8_t big_handle, uint8_t adv_handle, uint8_t num_bis,
                                uint32_t sdu_interval, uint16_t max_sdu, uint16_t max_transport_latency,
                                uint8_t rtn, uint8_t phy, uint8_t packing, uint8_t framing,
                                uint8_t encryption, uint8_t *broadcast_code);

UINT8 btsnd_hcic_ble_big_create_test(uint8_t big_handle, uint8_t adv_handle, uint8_t num_bis,
                                    uint32_t sdu_interval, uint16_t iso_interval, uint8_t nse,
                                    uint16_t max_sdu, uint16_t max_pdu, uint8_t phy,
                                    uint8_t packing, uint8_t framing, uint8_t bn, uint8_t irc,
                                    uint8_t pto, uint8_t encryption, uint8_t *broadcast_code);

UINT8 btsnd_hcic_ble_big_terminate(uint8_t big_handle, uint8_t reason);
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
UINT8 btsnd_hcic_ble_big_sync_create(uint8_t big_handle, uint16_t sync_handle,
                                    uint8_t encryption, uint8_t *bc_code,
                                    uint8_t mse, uint16_t big_sync_timeout,
                                    uint8_t num_bis, uint8_t *bis);

UINT8 btsnd_hcic_ble_big_sync_terminate(uint8_t big_handle);
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)

UINT8 btsnd_hcic_ble_iso_set_data_path(uint16_t conn_handle, uint8_t data_path_dir, uint8_t data_path_id, uint8_t coding_fmt,
                                    uint16_t company_id, uint16_t vs_codec_id, uint32_t controller_delay, uint8_t codec_len,
                                    uint8_t *codec_cfg);
UINT8 btsnd_hcic_ble_iso_remove_data_path(uint16_t conn_handle, uint8_t data_path_dir);

UINT8 btsnd_hcic_ble_iso_read_tx_sync(uint16_t iso_hdl);

UINT8 btsnd_hcic_ble_iso_read_iso_link_quality(uint16_t iso_hdl);

#endif // #if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#define HCIC_PARAM_SIZE_SET_CONNLESS_CTE_TRANS_PARAMS    5
#define HCIC_PARAM_SIZE_SET_CONNLESS_CTE_TRANS_ENABLE    2
#define HCIC_PARAM_SIZE_SET_CONNLESS_IQ_SAMPLING_ENABLE  6
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
#define HCIC_PARAM_SIZE_SET_CONN_CTE_RECEIVE_PARAMS      5
#define HCIC_PARAM_SIZE_SET_CONN_CTE_TRANS_PARAMS        4
#define HCIC_PARAM_SIZE_CONN_CTE_REQ_ENABLE              7
#define HCIC_PARAM_SIZE_CONN_CTE_RSP_ENABLE              3
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
#define HCIC_PARAM_SIZE_READ_ANT_INFO                    0
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
UINT8 btsnd_hcic_ble_set_connless_cte_trans_params(uint8_t adv_hdl, uint8_t cte_len, uint8_t cte_type,
                                                uint8_t cte_cnt, uint8_t switching_pattern_len, uint8_t *antenna_ids);
UINT8 btsnd_hcic_ble_set_connless_cte_enable(uint8_t adv_hdl, uint8_t cte_en);
UINT8 btsnd_hcic_ble_set_connless_iq_sampling_enable(uint16_t sync_hdl, uint8_t sampling_en, uint8_t slot_dur,
                                                uint8_t max_sampled_ctes, uint8_t switching_pattern_len, uint8_t *antenna_ids);
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
UINT8 btsnd_hcic_ble_set_conn_cte_receive_params(uint16_t conn_hdl, uint8_t sampling_en, uint8_t slot_dur,
                                                uint8_t switching_pattern_len, uint8_t *antenna_ids);
UINT8 btsnd_hcic_ble_set_conn_cte_trans_params(uint16_t conn_hdl, uint8_t cte_type, uint8_t switching_pattern_len, uint8_t *antenna_ids);
UINT8 btsnd_hcic_ble_conn_cte_req_enable(uint16_t conn_hdl, uint8_t enable, uint16_t cte_req_int, uint8_t req_cte_len, uint8_t req_cte_type);
UINT8 btsnd_hcic_ble_conn_cte_rsp_enable(uint16_t conn_hdl, uint8_t enable);
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
UINT8 btsnd_hcic_ble_read_antenna_info(void);
#endif // #if (BLE_FEAT_CTE_EN == TRUE)

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
#define HCIC_PARAM_SIZE_ENH_READ_TRANS_PWR_LEVEL                    3
#define HCIC_PARAM_SIZE_READ_REMOTE_TRANS_PWR_LEVEL                 3
#define HCIC_PARAM_SIZE_SET_PATH_LOSS_REPORTING_PARAMS              8
#define HCIC_PARAM_SIZE_SET_PATH_LOSS_REPORTING_ENABLE              3
#define HCIC_PARAM_SIZE_SET_TRANS_PWR_REPORTING_ENABLE              4

UINT8 btsnd_hcic_ble_enh_read_trans_power_level(uint16_t conn_handle, uint8_t phy);
UINT8 btsnd_hcic_ble_read_remote_trans_power_level(uint16_t conn_handle, uint8_t phy);
UINT8 btsnd_hcic_ble_set_path_loss_rpt_params(uint16_t conn_handle, uint8_t high_threshold, uint8_t high_hysteresis,
                                            uint8_t low_threshold, uint8_t low_hysteresis, uint16_t min_time_spent);
UINT8 btsnd_hcic_ble_set_path_loss_rpt_enable(uint16_t conn_handle, uint8_t enable);
UINT8 btsnd_hcic_ble_set_trans_pwr_rpt_enable(uint16_t conn_handle, uint8_t local_enable, uint8_t remote_enable);
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)

#if (BLE_FEAT_CONN_SUBRATING == TRUE)
#define HCIC_PARAM_SIZE_SET_DEFAULT_SUBRATE_PARAMS_LEN              10
#define HCIC_PARAM_SIZE_SUBRATE_REQ_LENGTH_PARAMS_LEN               12
UINT8 btsnd_hcic_ble_set_default_subrate(UINT16 subrate_min, UINT16 subrate_max, UINT16 max_latency,
                                            UINT16 continuation_number, UINT16 supervision_timeout);

UINT8 btsnd_hcic_ble_subrate_request(UINT16 conn_handle, UINT16 subrate_min, UINT16 subrate_max, UINT16 max_latency,
                                        UINT16 continuation_number, UINT16 supervision_timeout);
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
#define HCIC_PARAM_SIZE_SET_HOST_FEATURE_PARAMS       2
#define HCIC_PARAM_SIZE_SET_HOST_FEATURE_PARAMS_V2    3
UINT8 btsnd_hcic_ble_set_host_feature(uint16_t bit_num, uint8_t bit_val);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BT_BLE_FEAT_PAWR_EN == TRUE)
typedef struct {
	uint8_t subevent;
	uint8_t response_slot_start;
	uint8_t response_slot_count;
    uint8_t subevent_data_len;
	uint8_t data[251];
} ble_subevent_params;

#define HCIC_PARAM_SIZE_SET_PA_SUBEVT_DATA_PARAMS_LEN               2
#define HCIC_PARAM_SIZE_SET_PA_RESPONSE_DATA_PARAMS_LEN             8
#define HCIC_PARAM_SIZE_SET_PA_SYNC_SUBEVT_PARAMS_LEN               5

#define HCIC_PARAM_SIZE_SET_PERIODIC_ADV_PARAMS_V2                  12
UINT8 btsnd_hcic_ble_set_periodic_adv_params_v2(UINT8 adv_handle, UINT16 interval_min, UINT16 interval_max,
                                                UINT16 propertics, UINT8 num_subevents, UINT8 subevent_interval,
                                                UINT8 rsp_slot_delay, UINT8 rsp_slot_spacing, UINT8 num_rsp_slots);
UINT8 btsnd_hcic_ble_set_periodic_adv_subevt_data(UINT8 adv_handle, UINT8 num_subevents_with_data, ble_subevent_params *subevent_params);
UINT8 btsnd_hcic_ble_set_periodic_adv_rsp_data(UINT16 sync_handle, UINT16 req_evt, UINT8 req_subevt, UINT8 rsp_subevt, UINT8 rsp_slot, UINT8 rsp_data_len, UINT8 *rsp_data);
UINT8 btsnd_hcic_ble_set_periodic_sync_subevt(UINT16 sync_handle, UINT16 periodic_adv_properties, UINT8 num_subevents_to_sync, UINT8 *subevt);
#endif // #if (BT_BLE_FEAT_PAWR_EN == TRUE)

#if (BT_BLE_FEAT_ADV_CODING_SELECTION == TRUE)
UINT8 btsnd_hcic_ble_set_ext_adv_params_v2(UINT8 adv_handle, UINT16 properties, UINT32 interval_min,
                                          UINT32 interval_max, UINT8 channel_map, UINT8 own_addr_type,
                                          UINT8 peer_addr_type, BD_ADDR peer_addr,
                                          UINT8 adv_filter_policy, INT8 adv_tx_power,
                                          UINT8 primary_adv_phy, UINT8 secondary_adv_max_skip,
                                          UINT8 secondary_adv_phy,
                                          UINT8 adv_sid, UINT8 scan_req_ntf_enable,
                                          UINT8 primary_adv_phy_options, UINT8 secondary_adv_phy_options);
#endif // (BT_BLE_FEAT_ADV_CODING_SELECTION == TRUE)

#if (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)
#define HCIC_PARAM_SIZE_READ_LOCAL_SUPP_CAPS_PARAMS_LEN         0
#define HCIC_PARAM_SIZE_READ_REMOTE_SUPP_CAPS_PARAMS_LEN        2
#define HCIC_PARAM_SIZE_WRITE_CACHE_REMOTE_SUPP_CAPS_PARAMS_LEN        30
#define HCIC_PARAM_SIZE_SECURITY_ENABLE_PARAMS_LEN              2
#define HCIC_PARAM_SIZE_SET_DEFAULT_SETTINGS_PARAMS_LEN         5
#define HCIC_PARAM_SIZE_READ_REMOTE_FAE_TAB_PARAMS_LEN          2
#define HCIC_PARAM_SIZE_WRITE_CACHED_REMOTE_FAE_TAB_PARAMS_LEN  74
#define HCIC_PARAM_SIZE_CREATE_CONFIG_PARAMS_LEN                28
#define HCIC_PARAM_SIZE_REMOVE_CONFIG_PARAMS_LEN                3
#define HCIC_PARAM_SIZE_SET_CHANNEL_CLASS_PARAMS_LEN            10
#define HCIC_PARAM_SIZE_SET_PROCEDURE_PARAMS_LEN                23
#define HCIC_PARAM_SIZE_SET_PROCEDURE_ENABLE_PARAMS_LEN         4

UINT8 btsnd_hcic_ble_cs_read_local_supported_caps(void);
UINT8 btsnd_hcic_ble_cs_read_remote_supported_capabilities(UINT16 conn_handle);
UINT8 btsnd_hcic_ble_cs_write_cached_remote_supported_capabilities(UINT16 conn_handle, UINT8 num_config_supported, UINT16 max_consecutive_proc_supported,
                                                                    UINT8 num_ant_supported, UINT8 max_ant_paths_supported, UINT8 roles_supported,
                                                                    UINT8 modes_supported, UINT8 rtt_capability, UINT8 rtt_aa_only_n,
                                                                    UINT8 rtt_sounding_n, UINT8 rtt_random_payload_n, UINT16 NADM_sounding_capability,
                                                                    UINT16 NADM_random_capability, UINT8  cs_sync_phys_supported, UINT16 subfeatures_supported,
                                                                    UINT16 T_IP1_times_supported, UINT16 T_IP2_times_supported, UINT16 T_FCS_times_supported,
                                                                    UINT16 T_PM_times_supported, UINT8 T_SW_times_supported, UINT8 TX_SNR_capability);

UINT8 btsnd_hcic_ble_cs_security_enable(UINT16 conn_handle);
UINT8 btsnd_hcic_ble_cs_set_default_settings(UINT16 conn_handle, UINT8 role_enable, UINT8 cs_sync_ant_selection, INT8 max_tx_power);
UINT8 btsnd_hcic_ble_cs_read_remote_fae_table(UINT16 conn_handle);
UINT8 btsnd_hcic_ble_cs_write_cached_remote_fae_table(UINT16 conn_handle, UINT8 *remote_fae_table);
UINT8 btsnd_hcic_ble_cs_create_config(UINT16 conn_handle, UINT8 config_id, UINT8 create_context,
                                UINT8 main_mode_type, UINT8 sub_mode_type, UINT8 min_main_mode_steps,
                                UINT8 max_main_mode_steps, UINT8 main_mode_repetition, UINT8 mode_0_steps,
                                UINT8 role, UINT8 rtt_type, UINT8 cs_sync_phy, UINT8 *channel_map,
                                UINT8 channel_map_repetition, UINT8 channel_selection_type, UINT8 ch3c_shape,
                                UINT8 ch3c_jump,UINT8 reserved);
UINT8 btsnd_hcic_ble_cs_remove_config(UINT16 conn_handle, UINT8 config_id);
UINT8 btsnd_hcic_ble_cs_set_channel_classification(UINT8 *channel_class);
UINT8 btsnd_hcic_ble_cs_set_procedure_params(UINT16 conn_handle, UINT8 config_id, UINT16 max_procedure_len,
                                UINT16 min_procedure_interval, UINT16 max_procedure_interval,
                                UINT16 max_procedure_count, UINT32 min_subevent_len,
                                UINT32 max_subevent_len, UINT8 tone_ant_config_selection,
                                UINT8 phy, UINT8 tx_power_delta, UINT8 preferred_peer_antenna,
                                UINT8 SNR_control_initiator, UINT8 SNR_control_reflector);
UINT8 btsnd_hcic_ble_cs_procedure_enable(UINT16 conn_handle, UINT8 config_id, UINT8 enable);
#endif // (BT_BLE_FEAT_CHANNEL_SOUNDING == TRUE)

#endif
