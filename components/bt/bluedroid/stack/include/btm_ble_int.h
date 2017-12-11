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
 *  this file contains the main Bluetooth Manager (BTM) internal
 *  definitions.
 *
 ******************************************************************************/

#ifndef BTM_BLE_INT_H
#define BTM_BLE_INT_H

#include "bt_target.h"
#include "fixed_queue.h"
#include "hcidefs.h"
#include "btm_ble_api.h"
#include "btm_int.h"

#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
#include "smp_api.h"
#endif


/* scanning enable status */
#define BTM_BLE_SCAN_ENABLE      0x01
#define BTM_BLE_SCAN_DISABLE     0x00

/* advertising enable status */
#define BTM_BLE_ADV_ENABLE     0x01
#define BTM_BLE_ADV_DISABLE    0x00

/* use the high 4 bits unused by inquiry mode */
#define BTM_BLE_SELECT_SCAN     0x20
#define BTM_BLE_NAME_REQUEST    0x40
#define BTM_BLE_OBSERVE         0x80

#define BTM_BLE_MAX_WL_ENTRY        1
#define BTM_BLE_AD_DATA_LEN         31

#define BTM_BLE_ENC_MASK    0x03

#define BTM_BLE_DUPLICATE_ENABLE        1
#define BTM_BLE_DUPLICATE_DISABLE       0

#define BTM_BLE_GAP_DISC_SCAN_INT      18         /* Interval(scan_int) = 11.25 ms= 0x0010 * 0.625 ms */
#define BTM_BLE_GAP_DISC_SCAN_WIN      18         /* scan_window = 11.25 ms= 0x0010 * 0.625 ms */
#define BTM_BLE_GAP_ADV_INT            512        /* Tgap(gen_disc) = 1.28 s= 512 * 0.625 ms */
#define BTM_BLE_GAP_LIM_TOUT           180        /* Tgap(lim_timeout) = 180s max */
#define BTM_BLE_LOW_LATENCY_SCAN_INT   8000       /* Interval(scan_int) = 5s= 8000 * 0.625 ms */
#define BTM_BLE_LOW_LATENCY_SCAN_WIN   8000       /* scan_window = 5s= 8000 * 0.625 ms */


#define BTM_BLE_GAP_ADV_FAST_INT_1         48         /* TGAP(adv_fast_interval1) = 30(used) ~ 60 ms  = 48 *0.625 */
#define BTM_BLE_GAP_ADV_FAST_INT_2         160         /* TGAP(adv_fast_interval2) = 100(used) ~ 150 ms = 160 * 0.625 ms */
#define BTM_BLE_GAP_ADV_SLOW_INT           2048         /* Tgap(adv_slow_interval) = 1.28 s= 512 * 0.625 ms */
#define BTM_BLE_GAP_ADV_DIR_MAX_INT        800         /* Tgap(dir_conn_adv_int_max) = 500 ms = 800 * 0.625 ms */
#define BTM_BLE_GAP_ADV_DIR_MIN_INT        400         /* Tgap(dir_conn_adv_int_min) = 250 ms = 400 * 0.625 ms */

#define BTM_BLE_GAP_FAST_ADV_TOUT          30

#define BTM_BLE_SEC_REQ_ACT_NONE           0
#define BTM_BLE_SEC_REQ_ACT_ENCRYPT        1 /* encrypt the link using current key or key refresh */
#define BTM_BLE_SEC_REQ_ACT_PAIR           2
#define BTM_BLE_SEC_REQ_ACT_DISCARD        3 /* discard the sec request while encryption is started but not completed */
typedef UINT8   tBTM_BLE_SEC_REQ_ACT;

#define BLE_STATIC_PRIVATE_MSB_MASK          0x3f
#define BLE_RESOLVE_ADDR_MSB                 0x40   /*  most significant bit, bit7, bit6 is 01 to be resolvable random */
#define BLE_RESOLVE_ADDR_MASK                0xc0   /* bit 6, and bit7 */
#define BTM_BLE_IS_RESOLVE_BDA(x)           ((x[0] & BLE_RESOLVE_ADDR_MASK) == BLE_RESOLVE_ADDR_MSB)

/* LE scan activity bit mask, continue with LE inquiry bits */
#define BTM_LE_SELECT_CONN_ACTIVE      0x0040     /* selection connection is in progress */
#define BTM_LE_OBSERVE_ACTIVE          0x0080     /* observe is in progress */
#define BTM_LE_DISCOVER_ACTIVE         0x0100     /* scan is in progress */

/* BLE scan activity mask checking */
#define BTM_BLE_IS_SCAN_ACTIVE(x)   ((x) & BTM_BLE_SCAN_ACTIVE_MASK)
#define BTM_BLE_IS_INQ_ACTIVE(x)   ((x) & BTM_BLE_INQUIRY_MASK)
#define BTM_BLE_IS_OBS_ACTIVE(x)   ((x) & BTM_LE_OBSERVE_ACTIVE)
#define BTM_BLE_IS_DISCO_ACTIVE(x)   ((x) & BTM_LE_DISCOVER_ACTIVE)
#define BTM_BLE_IS_SEL_CONN_ACTIVE(x)   ((x) & BTM_LE_SELECT_CONN_ACTIVE)

/* BLE ADDR type ID bit */
#define BLE_ADDR_TYPE_ID_BIT 0x02

#define BTM_VSC_CHIP_CAPABILITY_L_VERSION 55
#define BTM_VSC_CHIP_CAPABILITY_M_VERSION 95

typedef enum {
    BTM_BLE_SCANNING,
    BTM_BLE_SCAN_PENDING,
    BTM_BLE_STOP_SCAN,
    BTM_BLE_ADVERTISING,
    BTM_BLE_ADV_PENDING,
    BTM_BLE_STOP_ADV,
}tBTM_BLE_GAP_STATE;

typedef struct {
    UINT16              data_mask;
    UINT8               *p_flags;
    UINT8               ad_data[BTM_BLE_AD_DATA_LEN];
    UINT8               *p_pad;
} tBTM_BLE_LOCAL_ADV_DATA;

typedef struct {
    UINT32          inq_count;          /* Used for determining if a response has already been      */
    /* received for the current inquiry operation. (We do not   */
    /* want to flood the caller with multiple responses from    */
    /* the same device.                                         */
    BOOLEAN         scan_rsp;
    tBLE_BD_ADDR    le_bda;
} tINQ_LE_BDADDR;

#define BTM_BLE_ADV_DATA_LEN_MAX        31
#define BTM_BLE_CACHE_ADV_DATA_MAX      62

#define BTM_BLE_ISVALID_PARAM(x, min, max)  (((x) >= (min) && (x) <= (max)) || ((x) == BTM_BLE_CONN_PARAM_UNDEF))

#define BTM_BLE_PRIVATE_ADDR_INT    900  /* 15 minutes minimum for random address refreshing */

typedef struct {
    UINT16 discoverable_mode;
    UINT16 connectable_mode;
    BOOLEAN scan_params_set;
    UINT32 scan_window;
    UINT32 scan_interval;
    UINT8 scan_type; /* current scan type: active or passive */
    UINT8 scan_duplicate_filter; /* duplicate filter enabled for scan */
    UINT16 adv_interval_min;
    UINT16 adv_interval_max;
    tBTM_BLE_AFP afp; /* advertising filter policy */
    tBTM_BLE_SFP sfp; /* scanning filter policy */

    tBLE_ADDR_TYPE adv_addr_type;
    UINT8 evt_type;
    UINT8 adv_mode;
    tBLE_BD_ADDR direct_bda;
    tBTM_BLE_EVT directed_conn;
    BOOLEAN fast_adv_on;
    TIMER_LIST_ENT fast_adv_timer;

    UINT8 adv_len;
    UINT8 adv_data_cache[BTM_BLE_CACHE_ADV_DATA_MAX];

    /* inquiry BD addr database */
    UINT8 num_bd_entries;
    UINT8 max_bd_entries;
    tBTM_BLE_LOCAL_ADV_DATA adv_data;
    tBTM_BLE_ADV_CHNL_MAP adv_chnl_map;

    TIMER_LIST_ENT inq_timer_ent;
    BOOLEAN scan_rsp;
    tBTM_BLE_GAP_STATE state; /* Current state that the inquiry process is in */
    INT8 tx_power;
} tBTM_BLE_INQ_CB;


/* random address resolving complete callback */
typedef void (tBTM_BLE_RESOLVE_CBACK) (void *match_rec, void *p);

typedef void (tBTM_BLE_ADDR_CBACK) (BD_ADDR_PTR static_random, void *p);

/* random address management control block */
typedef struct {
    tBLE_ADDR_TYPE              own_addr_type;         /* local device LE address type */
    BD_ADDR                     private_addr;
    BD_ADDR                     random_bda;
    BOOLEAN                     busy;
    UINT16                       index;
    tBTM_BLE_RESOLVE_CBACK      *p_resolve_cback;
    tBTM_BLE_ADDR_CBACK         *p_generate_cback;
    void                        *p;
    TIMER_LIST_ENT              raddr_timer_ent;
    tBTM_SET_LOCAL_PRIVACY_CBACK *set_local_privacy_cback;
} tBTM_LE_RANDOM_CB;

#define BTM_BLE_MAX_BG_CONN_DEV_NUM    10

typedef struct {
    UINT16              min_conn_int;
    UINT16              max_conn_int;
    UINT16              slave_latency;
    UINT16              supervision_tout;

} tBTM_LE_CONN_PRAMS;


typedef struct {
    BD_ADDR     bd_addr;
    UINT8       attr;
    BOOLEAN     is_connected;
    BOOLEAN     in_use;
} tBTM_LE_BG_CONN_DEV;

/* white list using state as a bit mask */
#define BTM_BLE_WL_IDLE         0
#define BTM_BLE_WL_INIT         1
#define BTM_BLE_WL_SCAN         2
#define BTM_BLE_WL_ADV          4
typedef UINT8 tBTM_BLE_WL_STATE;

/* resolving list using state as a bit mask */
#define BTM_BLE_RL_IDLE         0
#define BTM_BLE_RL_INIT         1
#define BTM_BLE_RL_SCAN         2
#define BTM_BLE_RL_ADV          4
typedef UINT8 tBTM_BLE_RL_STATE;

/* BLE connection state */
#define BLE_CONN_IDLE    0
#define BLE_DIR_CONN     1
#define BLE_BG_CONN      2
#define BLE_CONN_CANCEL  3
typedef UINT8 tBTM_BLE_CONN_ST;

typedef struct {
    void    *p_param;
} tBTM_BLE_CONN_REQ;

/* LE state request */
#define BTM_BLE_STATE_INVALID               0
#define BTM_BLE_STATE_CONN_ADV              1
#define BTM_BLE_STATE_INIT                  2
#define BTM_BLE_STATE_MASTER                3
#define BTM_BLE_STATE_SLAVE                 4
#define BTM_BLE_STATE_LO_DUTY_DIR_ADV       5
#define BTM_BLE_STATE_HI_DUTY_DIR_ADV       6
#define BTM_BLE_STATE_NON_CONN_ADV          7
#define BTM_BLE_STATE_PASSIVE_SCAN          8
#define BTM_BLE_STATE_ACTIVE_SCAN           9
#define BTM_BLE_STATE_SCAN_ADV              10
#define BTM_BLE_STATE_MAX                   11
typedef UINT8 tBTM_BLE_STATE;

#define BTM_BLE_STATE_CONN_ADV_BIT          0x0001
#define BTM_BLE_STATE_INIT_BIT              0x0002
#define BTM_BLE_STATE_MASTER_BIT            0x0004
#define BTM_BLE_STATE_SLAVE_BIT             0x0008
#define BTM_BLE_STATE_LO_DUTY_DIR_ADV_BIT   0x0010
#define BTM_BLE_STATE_HI_DUTY_DIR_ADV_BIT   0x0020
#define BTM_BLE_STATE_NON_CONN_ADV_BIT      0x0040
#define BTM_BLE_STATE_PASSIVE_SCAN_BIT      0x0080
#define BTM_BLE_STATE_ACTIVE_SCAN_BIT       0x0100
#define BTM_BLE_STATE_SCAN_ADV_BIT          0x0200
typedef UINT16 tBTM_BLE_STATE_MASK;

#define BTM_BLE_STATE_ALL_MASK              0x03ff
#define BTM_BLE_STATE_ALL_ADV_MASK          (BTM_BLE_STATE_CONN_ADV_BIT|BTM_BLE_STATE_LO_DUTY_DIR_ADV_BIT|BTM_BLE_STATE_HI_DUTY_DIR_ADV_BIT|BTM_BLE_STATE_SCAN_ADV_BIT)
#define BTM_BLE_STATE_ALL_SCAN_MASK         (BTM_BLE_STATE_PASSIVE_SCAN_BIT|BTM_BLE_STATE_ACTIVE_SCAN_BIT)
#define BTM_BLE_STATE_ALL_CONN_MASK         (BTM_BLE_STATE_MASTER_BIT|BTM_BLE_STATE_SLAVE_BIT)

#ifndef BTM_LE_RESOLVING_LIST_MAX
#define BTM_LE_RESOLVING_LIST_MAX     0x20
#endif

typedef struct {
    BD_ADDR         *resolve_q_random_pseudo;
    UINT8           *resolve_q_action;
    UINT8           q_next;
    UINT8           q_pending;
} tBTM_BLE_RESOLVE_Q;

typedef struct {
    BOOLEAN     in_use;
    BOOLEAN     to_add;
    BD_ADDR     bd_addr;
    UINT8       attr;
} tBTM_BLE_WL_OP;

/* BLE privacy mode */
#define BTM_PRIVACY_NONE    0              /* BLE no privacy */
#define BTM_PRIVACY_1_1     1              /* BLE privacy 1.1, do not support privacy 1.0 */
#define BTM_PRIVACY_1_2     2              /* BLE privacy 1.2 */
#define BTM_PRIVACY_MIXED   3              /* BLE privacy mixed mode, broadcom propietary mode */
typedef UINT8 tBTM_PRIVACY_MODE;

/* data length change event callback */
typedef void (tBTM_DATA_LENGTH_CHANGE_CBACK) (UINT16 max_tx_length, UINT16 max_rx_length);

/* Define BLE Device Management control structure
*/
typedef struct {
    UINT16 scan_activity;         /* LE scan activity mask */

    /*****************************************************
    **      BLE Inquiry
    *****************************************************/
    tBTM_BLE_INQ_CB inq_var;

    /* observer callback and timer */
    tBTM_INQ_RESULTS_CB *p_obs_results_cb;
    tBTM_CMPL_CB *p_obs_cmpl_cb;
    TIMER_LIST_ENT obs_timer_ent;

    /* scan callback and timer */
    tBTM_INQ_RESULTS_CB *p_scan_results_cb;
    tBTM_CMPL_CB *p_scan_cmpl_cb;
    TIMER_LIST_ENT scan_timer_ent;

    /* background connection procedure cb value */
    tBTM_BLE_CONN_TYPE bg_conn_type;
    UINT32 scan_int;
    UINT32 scan_win;
    tBTM_BLE_SEL_CBACK *p_select_cback;

    /* white list information */
    UINT8 white_list_avail_size;
    tBTM_ADD_WHITELIST_CBACK *add_wl_cb;
    tBTM_BLE_WL_STATE wl_state;

    fixed_queue_t *conn_pending_q;
    tBTM_BLE_CONN_ST conn_state;

    /* random address management control block */
    tBTM_LE_RANDOM_CB addr_mgnt_cb;

    BOOLEAN enabled;

#if BLE_PRIVACY_SPT == TRUE
    BOOLEAN mixed_mode; /* privacy 1.2 mixed mode is on or not */
    tBTM_PRIVACY_MODE privacy_mode; /* privacy mode */
    UINT8 resolving_list_avail_size; /* resolving list available size */
    tBTM_BLE_RESOLVE_Q resolving_list_pend_q; /* Resolving list queue */
    tBTM_BLE_RL_STATE suspended_rl_state; /* Suspended resolving list state */
    UINT8 *irk_list_mask; /* IRK list availability mask, up to max entry bits */
    tBTM_BLE_RL_STATE rl_state; /* Resolving list state */
#endif

    tBTM_BLE_WL_OP wl_op_q[BTM_BLE_MAX_BG_CONN_DEV_NUM];

    /* current BLE link state */
    tBTM_BLE_STATE_MASK cur_states; /* bit mask of tBTM_BLE_STATE */
    UINT8 link_count[2]; /* total link count master and slave*/
} tBTM_BLE_CB;

#ifdef __cplusplus
extern "C" {
#endif

void btm_ble_timeout(TIMER_LIST_ENT *p_tle);
void btm_ble_process_adv_pkt (UINT8 *p);
void btm_ble_proc_scan_rsp_rpt (UINT8 *p);
tBTM_STATUS btm_ble_read_remote_name(BD_ADDR remote_bda, tBTM_INQ_INFO *p_cur, tBTM_CMPL_CB *p_cb);
BOOLEAN btm_ble_cancel_remote_name(BD_ADDR remote_bda);

tBTM_STATUS btm_ble_set_discoverability(UINT16 combined_mode);
tBTM_STATUS btm_ble_set_connectability(UINT16 combined_mode);
tBTM_STATUS btm_ble_start_inquiry (UINT8 mode, UINT8   duration);
void btm_ble_stop_scan(void);
void btm_clear_all_pending_le_entry(void);

BOOLEAN btm_ble_send_extended_scan_params(UINT8 scan_type, UINT32 scan_int,
        UINT32 scan_win, UINT8 addr_type_own,
        UINT8 scan_filter_policy);
void btm_ble_stop_inquiry(void);
void btm_ble_init (void);
void btm_ble_free (void);
void btm_ble_connected (UINT8 *bda, UINT16 handle, UINT8 enc_mode, UINT8 role, tBLE_ADDR_TYPE addr_type, BOOLEAN addr_matched);
void btm_ble_read_remote_features_complete(UINT8 *p);
void btm_ble_write_adv_enable_complete(UINT8 *p);
void btm_ble_conn_complete(UINT8 *p, UINT16 evt_len, BOOLEAN enhanced);
void btm_read_ble_local_supported_states_complete(UINT8 *p, UINT16 evt_len);
tBTM_BLE_CONN_ST btm_ble_get_conn_st(void);
void btm_ble_set_conn_st(tBTM_BLE_CONN_ST new_st);
UINT8 *btm_ble_build_adv_data(tBTM_BLE_AD_MASK *p_data_mask, UINT8 **p_dst,
                              tBTM_BLE_ADV_DATA *p_data);
tBTM_STATUS btm_ble_start_adv(void);
tBTM_STATUS btm_ble_stop_adv(void);
tBTM_STATUS btm_ble_start_scan(void);
void btm_ble_create_ll_conn_complete (UINT8 status);

/* LE security function from btm_sec.c */
#if SMP_INCLUDED == TRUE
void btm_ble_link_sec_check(BD_ADDR bd_addr, tBTM_LE_AUTH_REQ auth_req, tBTM_BLE_SEC_REQ_ACT *p_sec_req_act);
void btm_ble_ltk_request_reply(BD_ADDR bda,  BOOLEAN use_stk, BT_OCTET16 stk);
UINT8 btm_proc_smp_cback(tSMP_EVT event, BD_ADDR bd_addr, tSMP_EVT_DATA *p_data);
tBTM_STATUS btm_ble_set_encryption (BD_ADDR bd_addr, void *p_ref_data, UINT8 link_role);
void btm_ble_ltk_request(UINT16 handle, UINT8 rand[8], UINT16 ediv);
tBTM_STATUS btm_ble_start_encrypt(BD_ADDR bda, BOOLEAN use_stk, BT_OCTET16 stk);
void btm_ble_link_encrypted(BD_ADDR bd_addr, UINT8 encr_enable);
#endif

/* LE device management functions */
void btm_ble_reset_id( void );

/* security related functions */
void btm_ble_increment_sign_ctr(BD_ADDR bd_addr, BOOLEAN is_local );
BOOLEAN btm_get_local_div (BD_ADDR bd_addr, UINT16 *p_div);
BOOLEAN btm_ble_get_enc_key_type(BD_ADDR bd_addr, UINT8 *p_key_types);

void btm_ble_test_command_complete(UINT8 *p);
void btm_ble_rand_enc_complete (UINT8 *p, UINT16 op_code, tBTM_RAND_ENC_CB *p_enc_cplt_cback);

void btm_sec_save_le_key(BD_ADDR bd_addr, tBTM_LE_KEY_TYPE key_type, tBTM_LE_KEY_VALUE *p_keys, BOOLEAN pass_to_application);
void btm_ble_update_sec_key_size(BD_ADDR bd_addr, UINT8 enc_key_size);
UINT8 btm_ble_read_sec_key_size(BD_ADDR bd_addr);

/* white list function */
BOOLEAN btm_update_dev_to_white_list(BOOLEAN to_add, BD_ADDR bd_addr, tBTM_ADD_WHITELIST_CBACK *add_wl_cb);
void btm_update_scanner_filter_policy(tBTM_BLE_SFP scan_policy);
void btm_update_adv_filter_policy(tBTM_BLE_AFP adv_policy);
void btm_ble_clear_white_list (void);
void btm_read_white_list_size_complete(UINT8 *p, UINT16 evt_len);
void btm_ble_add_2_white_list_complete(UINT8 status);
void btm_ble_remove_from_white_list_complete(UINT8 *p, UINT16 evt_len);
void btm_ble_clear_white_list_complete(UINT8 *p, UINT16 evt_len);
void btm_ble_white_list_init(UINT8 white_list_size);

/* background connection function */
BOOLEAN btm_ble_suspend_bg_conn(void);
BOOLEAN btm_ble_resume_bg_conn(void);
void btm_ble_initiate_select_conn(BD_ADDR bda);
BOOLEAN btm_ble_start_auto_conn(BOOLEAN start);
BOOLEAN btm_ble_start_select_conn(BOOLEAN start, tBTM_BLE_SEL_CBACK   *p_select_cback);
BOOLEAN btm_ble_renew_bg_conn_params(BOOLEAN add, BD_ADDR bd_addr);
void btm_write_dir_conn_wl(BD_ADDR target_addr);
void btm_ble_update_mode_operation(UINT8 link_role, BD_ADDR bda, UINT8 status);
BOOLEAN btm_execute_wl_dev_operation(void);
void btm_ble_update_link_topology_mask(UINT8 role, BOOLEAN increase);

/* direct connection utility */
BOOLEAN btm_send_pending_direct_conn(void);
void btm_ble_enqueue_direct_conn_req(void *p_param);

/* BLE address management */
void btm_gen_resolvable_private_addr (void *p_cmd_cplt_cback);
void btm_gen_non_resolvable_private_addr (tBTM_BLE_ADDR_CBACK *p_cback, void *p);
void btm_ble_resolve_random_addr(BD_ADDR random_bda, tBTM_BLE_RESOLVE_CBACK *p_cback, void *p);
void btm_gen_resolve_paddr_low(tBTM_RAND_ENC *p);

/*  privacy function */
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
/* BLE address mapping with CS feature */
BOOLEAN btm_identity_addr_to_random_pseudo(BD_ADDR bd_addr, UINT8 *p_addr_type, BOOLEAN refresh);
BOOLEAN btm_random_pseudo_to_identity_addr(BD_ADDR random_pseudo, UINT8 *p_static_addr_type);
void btm_ble_refresh_peer_resolvable_private_addr(BD_ADDR pseudo_bda, BD_ADDR rra, UINT8 rra_type);
void btm_ble_refresh_local_resolvable_private_addr(BD_ADDR pseudo_addr, BD_ADDR local_rpa);
void btm_ble_read_resolving_list_entry_complete(UINT8 *p, UINT16 evt_len) ;
void btm_ble_remove_resolving_list_entry_complete(UINT8 *p, UINT16 evt_len);
void btm_ble_add_resolving_list_entry_complete(UINT8 *p, UINT16 evt_len);
void btm_ble_clear_resolving_list_complete(UINT8 *p, UINT16 evt_len);
void btm_read_ble_resolving_list_size_complete (UINT8 *p, UINT16 evt_len);
void btm_ble_enable_resolving_list(UINT8);
BOOLEAN btm_ble_disable_resolving_list(UINT8 rl_mask, BOOLEAN to_resume);
void btm_ble_enable_resolving_list_for_platform (UINT8 rl_mask);
void btm_ble_resolving_list_init(UINT8 max_irk_list_sz);
void btm_ble_resolving_list_cleanup(void);
#endif

void btm_ble_multi_adv_configure_rpa (tBTM_BLE_MULTI_ADV_INST *p_inst);
void btm_ble_multi_adv_init(void);
void *btm_ble_multi_adv_get_ref(UINT8 inst_id);
void btm_ble_multi_adv_cleanup(void);
void btm_ble_multi_adv_reenable(UINT8 inst_id);
void btm_ble_multi_adv_enb_privacy(BOOLEAN enable);
char btm_ble_map_adv_tx_power(int tx_power_index);
void btm_ble_batchscan_init(void);
void btm_ble_batchscan_cleanup(void);
void btm_ble_adv_filter_init(void);
void btm_ble_adv_filter_cleanup(void);
BOOLEAN btm_ble_topology_check(tBTM_BLE_STATE_MASK request);
BOOLEAN btm_ble_clear_topology_mask(tBTM_BLE_STATE_MASK request_state);
BOOLEAN btm_ble_set_topology_mask(tBTM_BLE_STATE_MASK request_state);

#if BTM_BLE_CONFORMANCE_TESTING == TRUE
void btm_ble_set_no_disc_if_pair_fail (BOOLEAN disble_disc);
void btm_ble_set_test_mac_value (BOOLEAN enable, UINT8 *p_test_mac_val);
void btm_ble_set_test_local_sign_cntr_value(BOOLEAN enable, UINT32 test_local_sign_cntr);
void btm_set_random_address(BD_ADDR random_bda);
void btm_ble_set_keep_rfu_in_auth_req(BOOLEAN keep_rfu);
#endif

/*
#ifdef __cplusplus
}
#endif
*/
#endif
