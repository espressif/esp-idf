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
 *  This file contains internally used SMP definitions
 *
 ******************************************************************************/
#ifndef  SMP_INT_H
#define  SMP_INT_H

// #if (SMP_INCLUDED == TRUE)

#include "stack/btu.h"
#include "stack/btm_ble_api.h"
#include "stack/btm_api.h"
#include "stack/smp_api.h"

#define SMP_MODEL_ENCRYPTION_ONLY  0   /* Legacy mode, Just Works model */
#define SMP_MODEL_PASSKEY       1   /* Legacy mode, Passkey Entry model, this side inputs the key */
#define SMP_MODEL_OOB           2   /* Legacy mode, OOB model */
#define SMP_MODEL_KEY_NOTIF     3   /* Legacy mode, Passkey Entry model, this side displays the key */
#define SMP_MODEL_SEC_CONN_JUSTWORKS  4  /* Secure Connections mode, Just Works model */
#define SMP_MODEL_SEC_CONN_NUM_COMP   5  /* Secure Connections mode, Numeric Comparison model */
#define SMP_MODEL_SEC_CONN_PASSKEY_ENT 6 /* Secure Connections mode, Passkey Entry model, */
/* this side inputs the key */
#define SMP_MODEL_SEC_CONN_PASSKEY_DISP 7   /* Secure Connections mode, Passkey Entry model, */
/* this side displays the key */
#define SMP_MODEL_SEC_CONN_OOB  8   /* Secure Connections mode, OOB model */
#define SMP_MODEL_OUT_OF_RANGE  9
typedef UINT8   tSMP_ASSO_MODEL;


#ifndef SMP_MAX_CONN
#define SMP_MAX_CONN    2
#endif

#define SMP_WAIT_FOR_RSP_TOUT           30

#define SMP_OPCODE_INIT                   0x04

/* SMP events */
#define SMP_PAIRING_REQ_EVT             SMP_OPCODE_PAIRING_REQ
#define SMP_PAIRING_RSP_EVT             SMP_OPCODE_PAIRING_RSP
#define SMP_CONFIRM_EVT                 SMP_OPCODE_CONFIRM
#define SMP_RAND_EVT                    SMP_OPCODE_RAND
#define SMP_PAIRING_FAILED_EVT          SMP_OPCODE_PAIRING_FAILED
#define SMP_ENCRPTION_INFO_EVT          SMP_OPCODE_ENCRYPT_INFO
#define SMP_MASTER_ID_EVT               SMP_OPCODE_MASTER_ID
#define SMP_ID_INFO_EVT                 SMP_OPCODE_IDENTITY_INFO
#define SMP_ID_ADDR_EVT                 SMP_OPCODE_ID_ADDR
#define SMP_SIGN_INFO_EVT               SMP_OPCODE_SIGN_INFO
#define SMP_SECURITY_REQ_EVT            SMP_OPCODE_SEC_REQ

#define SMP_PAIR_PUBLIC_KEY_EVT         SMP_OPCODE_PAIR_PUBLIC_KEY
#define SMP_PAIR_KEYPRESS_NOTIFICATION_EVT SMP_OPCODE_PAIR_KEYPR_NOTIF

#define SMP_PAIR_COMMITM_EVT            SMP_OPCODE_PAIR_COMMITM

#define SMP_SELF_DEF_EVT                (SMP_PAIR_COMMITM_EVT + 1)
#define SMP_KEY_READY_EVT               (SMP_SELF_DEF_EVT)
#define SMP_ENCRYPTED_EVT               (SMP_SELF_DEF_EVT + 1)
#define SMP_L2CAP_CONN_EVT              (SMP_SELF_DEF_EVT + 2)
#define SMP_L2CAP_DISCONN_EVT           (SMP_SELF_DEF_EVT + 3)
#define SMP_IO_RSP_EVT                  (SMP_SELF_DEF_EVT + 4)
#define SMP_API_SEC_GRANT_EVT           (SMP_SELF_DEF_EVT + 5)
#define SMP_TK_REQ_EVT                  (SMP_SELF_DEF_EVT + 6)
#define SMP_AUTH_CMPL_EVT               (SMP_SELF_DEF_EVT + 7)
#define SMP_ENC_REQ_EVT                 (SMP_SELF_DEF_EVT + 8)
#define SMP_BOND_REQ_EVT                (SMP_SELF_DEF_EVT + 9)
#define SMP_DISCARD_SEC_REQ_EVT         (SMP_SELF_DEF_EVT + 10)

#define SMP_PAIR_DHKEY_CHCK_EVT         SMP_OPCODE_PAIR_DHKEY_CHECK

#define SMP_PUBL_KEY_EXCH_REQ_EVT       (SMP_SELF_DEF_EVT + 11) /* request to start public */
/* key exchange */

#define SMP_LOC_PUBL_KEY_CRTD_EVT       (SMP_SELF_DEF_EVT + 12) /* local public key created */

#define SMP_BOTH_PUBL_KEYS_RCVD_EVT     (SMP_SELF_DEF_EVT + 13) /* both local and peer public */
/* keys are saved in cb */

#define SMP_SC_DHKEY_CMPLT_EVT          (SMP_SELF_DEF_EVT + 14) /* DHKey computation is completed,*/
/* time to start SC phase1 */

#define SMP_HAVE_LOC_NONCE_EVT          (SMP_SELF_DEF_EVT + 15) /* new local nonce is generated */
/*and saved in p_cb->rand */

#define SMP_SC_PHASE1_CMPLT_EVT         (SMP_SELF_DEF_EVT + 16) /* time to start SC phase2 */

#define SMP_SC_CALC_NC_EVT              (SMP_SELF_DEF_EVT + 17) /* request to calculate number */
/* for user check. Used only in the */
/* numeric compare protocol */

/* Request to display the number for user check to the user.*/
/* Used only in the numeric compare protocol */
#define SMP_SC_DSPL_NC_EVT              (SMP_SELF_DEF_EVT + 18)

#define SMP_SC_NC_OK_EVT                (SMP_SELF_DEF_EVT + 19) /* user confirms 'OK' numeric */
/*comparison request */

/* both local and peer DHKey Checks are already present - it is used on slave to prevent race condition */
#define SMP_SC_2_DHCK_CHKS_PRES_EVT     (SMP_SELF_DEF_EVT + 20)

/* same meaning as SMP_KEY_READY_EVT to separate between SC and legacy actions */
#define SMP_SC_KEY_READY_EVT            (SMP_SELF_DEF_EVT + 21)
#define SMP_KEYPRESS_NOTIFICATION_EVENT (SMP_SELF_DEF_EVT + 22)

#define SMP_SC_OOB_DATA_EVT             (SMP_SELF_DEF_EVT + 23) /* SC OOB data from some */
/* repository is provided */

#define SMP_CR_LOC_SC_OOB_DATA_EVT      (SMP_SELF_DEF_EVT + 24)
#define SMP_MAX_EVT                      SMP_CR_LOC_SC_OOB_DATA_EVT

typedef UINT8 tSMP_EVENT;

/* Assumption it's only using the low 8 bits, if bigger than that, need to expand it to 16 bits */
#define SMP_SEC_KEY_MASK                    0x00ff

#define SMP_PASSKEY_MASK    0xfff00000

/* SMP pairing state */
enum {
    SMP_STATE_IDLE,
    SMP_STATE_WAIT_APP_RSP,
    SMP_STATE_SEC_REQ_PENDING,
    SMP_STATE_PAIR_REQ_RSP,
    SMP_STATE_WAIT_CONFIRM,
    SMP_STATE_CONFIRM,
    SMP_STATE_RAND,
    SMP_STATE_PUBLIC_KEY_EXCH,
    SMP_STATE_SEC_CONN_PHS1_START,
    SMP_STATE_WAIT_COMMITMENT,
    SMP_STATE_WAIT_NONCE,
    SMP_STATE_SEC_CONN_PHS2_START,
    SMP_STATE_WAIT_DHK_CHECK,
    SMP_STATE_DHK_CHECK,
    SMP_STATE_ENCRYPTION_PENDING,
    SMP_STATE_BOND_PENDING,
    SMP_STATE_CREATE_LOCAL_SEC_CONN_OOB_DATA,
    SMP_STATE_MAX
};
typedef UINT8 tSMP_STATE;

/* SMP over BR/EDR events */
#define SMP_BR_PAIRING_REQ_EVT              SMP_OPCODE_PAIRING_REQ
#define SMP_BR_PAIRING_RSP_EVT              SMP_OPCODE_PAIRING_RSP
#define SMP_BR_CONFIRM_EVT                  SMP_OPCODE_CONFIRM    /* not expected over BR/EDR */
#define SMP_BR_RAND_EVT                     SMP_OPCODE_RAND       /* not expected over BR/EDR */
#define SMP_BR_PAIRING_FAILED_EVT           SMP_OPCODE_PAIRING_FAILED
#define SMP_BR_ENCRPTION_INFO_EVT           SMP_OPCODE_ENCRYPT_INFO /* not expected over BR/EDR */
#define SMP_BR_MASTER_ID_EVT                SMP_OPCODE_MASTER_ID    /* not expected over BR/EDR */
#define SMP_BR_ID_INFO_EVT                  SMP_OPCODE_IDENTITY_INFO
#define SMP_BR_ID_ADDR_EVT                  SMP_OPCODE_ID_ADDR
#define SMP_BR_SIGN_INFO_EVT                SMP_OPCODE_SIGN_INFO
#define SMP_BR_SECURITY_REQ_EVT          SMP_OPCODE_SEC_REQ          /* not expected over BR/EDR */
#define SMP_BR_PAIR_PUBLIC_KEY_EVT       SMP_OPCODE_PAIR_PUBLIC_KEY  /* not expected over BR/EDR */
#define SMP_BR_PAIR_DHKEY_CHCK_EVT       SMP_OPCODE_PAIR_DHKEY_CHECK /* not expected over BR/EDR */
#define SMP_BR_PAIR_KEYPR_NOTIF_EVT      SMP_OPCODE_PAIR_KEYPR_NOTIF /* not expected over BR/EDR */
#define SMP_BR_SELF_DEF_EVT              SMP_BR_PAIR_KEYPR_NOTIF_EVT
#define SMP_BR_KEY_READY_EVT                (SMP_BR_SELF_DEF_EVT + 1)
#define SMP_BR_ENCRYPTED_EVT                (SMP_BR_SELF_DEF_EVT + 2)
#define SMP_BR_L2CAP_CONN_EVT               (SMP_BR_SELF_DEF_EVT + 3)
#define SMP_BR_L2CAP_DISCONN_EVT            (SMP_BR_SELF_DEF_EVT + 4)
#define SMP_BR_KEYS_RSP_EVT                 (SMP_BR_SELF_DEF_EVT + 5)
#define SMP_BR_API_SEC_GRANT_EVT            (SMP_BR_SELF_DEF_EVT + 6)
#define SMP_BR_TK_REQ_EVT                   (SMP_BR_SELF_DEF_EVT + 7)
#define SMP_BR_AUTH_CMPL_EVT                (SMP_BR_SELF_DEF_EVT + 8)
#define SMP_BR_ENC_REQ_EVT                  (SMP_BR_SELF_DEF_EVT + 9)
#define SMP_BR_BOND_REQ_EVT                 (SMP_BR_SELF_DEF_EVT + 10)
#define SMP_BR_DISCARD_SEC_REQ_EVT          (SMP_BR_SELF_DEF_EVT + 11)
#define SMP_BR_MAX_EVT                      (SMP_BR_SELF_DEF_EVT + 12)
typedef UINT8 tSMP_BR_EVENT;

/* SMP over BR/EDR pairing states */
enum {
    SMP_BR_STATE_IDLE = SMP_STATE_IDLE,
    SMP_BR_STATE_WAIT_APP_RSP,
    SMP_BR_STATE_PAIR_REQ_RSP,
    SMP_BR_STATE_BOND_PENDING,
    SMP_BR_STATE_MAX
};
typedef UINT8 tSMP_BR_STATE;

/* random and encrption activity state */
enum {
    SMP_GEN_COMPARE = 1,
    SMP_GEN_CONFIRM,

    SMP_GEN_DIV_LTK,
    SMP_GEN_DIV_CSRK,
    SMP_GEN_RAND_V,
    SMP_GEN_TK,
    SMP_GEN_SRAND_MRAND,
    SMP_GEN_SRAND_MRAND_CONT,
    SMP_GENERATE_PRIVATE_KEY_0_7,
    SMP_GENERATE_PRIVATE_KEY_8_15,
    SMP_GENERATE_PRIVATE_KEY_16_23,
    SMP_GENERATE_PRIVATE_KEY_24_31,
    SMP_GEN_NONCE_0_7,
    SMP_GEN_NONCE_8_15
};

enum {
    SMP_KEY_TYPE_TK,
    SMP_KEY_TYPE_CFM,
    SMP_KEY_TYPE_CMP,
    SMP_KEY_TYPE_PEER_DHK_CHCK,
    SMP_KEY_TYPE_STK,
    SMP_KEY_TYPE_LTK
};
typedef struct {
    UINT8   key_type;
    UINT8  *p_data;
} tSMP_KEY;

typedef union {
    UINT8       *p_data;    /* UINT8 type data pointer */
    tSMP_KEY    key;
    UINT16      reason;
    UINT32      passkey;
    tSMP_OOB_DATA_TYPE  req_oob_type;
} tSMP_INT_DATA;

/* internal status mask */
#define SMP_PAIR_FLAGS_WE_STARTED_DD           (1)
#define SMP_PAIR_FLAGS_PEER_STARTED_DD         (1 << 1)
#define SMP_PAIR_FLAGS_CMD_CONFIRM             (1 << SMP_OPCODE_CONFIRM) /* 1 << 3 */
#define SMP_PAIR_FLAG_ENC_AFTER_PAIR           (1 << 4)
#define SMP_PAIR_FLAG_HAVE_PEER_DHK_CHK   (1 << 5) /* used on slave to resolve race condition */
#define SMP_PAIR_FLAG_HAVE_PEER_PUBL_KEY  (1 << 6) /* used on slave to resolve race condition */
#define SMP_PAIR_FLAG_HAVE_PEER_COMM      (1 << 7) /* used to resolve race condition */
#define SMP_PAIR_FLAG_HAVE_LOCAL_PUBL_KEY (1 << 8) /* used on slave to resolve race condition */

/* check if authentication requirement need MITM protection */
#define SMP_NO_MITM_REQUIRED(x)  (((x) & SMP_AUTH_YN_BIT) == 0)

#define SMP_ENCRYT_KEY_SIZE                16
#define SMP_ENCRYT_DATA_SIZE               16
#define SMP_ECNCRPYT_STATUS                HCI_SUCCESS

typedef struct {
    BD_ADDR bd_addr;
    BT_HDR *p_copy;
} tSMP_REQ_Q_ENTRY;

/* SMP control block */
typedef struct {
    tSMP_CALLBACK   *p_callback;
    TIMER_LIST_ENT  rsp_timer_ent;
    UINT8           trace_level;
    BD_ADDR         pairing_bda;
    tSMP_STATE      state;
    BOOLEAN         derive_lk;
    BOOLEAN         id_addr_rcvd;
    tBLE_ADDR_TYPE  id_addr_type;
    BD_ADDR         id_addr;
    BOOLEAN         smp_over_br;
    tSMP_BR_STATE   br_state;           /* if SMP over BR/ERD has priority over SMP */
    UINT8           failure;
    UINT8           status;
    UINT8           role;
    UINT16          flags;
    UINT8           cb_evt;
    tSMP_SEC_LEVEL  sec_level;
    BOOLEAN         connect_initialized;
    BT_OCTET16      confirm;
    BT_OCTET16      rconfirm;
    BT_OCTET16      rrand;                      /* for SC this is peer nonce */
    BT_OCTET16      rand;                       /* for SC this is local nonce */
    BT_OCTET32      private_key;
    BT_OCTET32      dhkey;
    BT_OCTET16      commitment;
    BT_OCTET16      remote_commitment;
    BT_OCTET16      local_random;               /* local randomizer - passkey or OOB randomizer */
    BT_OCTET16      peer_random;                /* peer randomizer - passkey or OOB randomizer */
    BT_OCTET16      dhkey_check;
    BT_OCTET16      remote_dhkey_check;
    tSMP_PUBLIC_KEY loc_publ_key;
    tSMP_PUBLIC_KEY peer_publ_key;
    tSMP_OOB_DATA_TYPE  req_oob_type;
    tSMP_SC_OOB_DATA    sc_oob_data;
    tSMP_IO_CAP     peer_io_caps;
    tSMP_IO_CAP     local_io_capability;
    tSMP_OOB_FLAG   peer_oob_flag;
    tSMP_OOB_FLAG   loc_oob_flag;
    tSMP_AUTH_REQ   peer_auth_req;
    tSMP_AUTH_REQ   loc_auth_req;
    tSMP_AUTH_REQ   auth_mode;
    BOOLEAN         secure_connections_only_mode_required;/* TRUE if locally SM is required to operate */
    /* either in Secure Connections mode or not at all */
    tSMP_ASSO_MODEL selected_association_model;
    BOOLEAN         le_secure_connections_mode_is_used;
    BOOLEAN le_sc_kp_notif_is_used;
    tSMP_SC_KEY_TYPE local_keypress_notification;
    tSMP_SC_KEY_TYPE peer_keypress_notification;
    UINT8           round;       /* authentication stage 1 round for passkey association model */
    UINT32          number_to_display;
    BT_OCTET16      mac_key;
    UINT8           peer_enc_size;
    UINT8           loc_enc_size;
    UINT8           peer_i_key;
    UINT8           peer_r_key;
    UINT8           local_i_key;
    UINT8           local_r_key;

    BT_OCTET16      tk;
    BT_OCTET16      ltk;
    UINT16          div;
    BT_OCTET16      csrk;  /* storage for local CSRK */
    UINT16          ediv;
    BT_OCTET8       enc_rand;
    UINT8           rand_enc_proc_state;
    UINT8           addr_type;
    BD_ADDR         local_bda;
    BOOLEAN         is_pair_cancel;
    BOOLEAN         discard_sec_req;
    UINT8           rcvd_cmd_code;
    UINT8           rcvd_cmd_len;
    UINT16          total_tx_unacked;
    BOOLEAN         wait_for_authorization_complete;
    BOOLEAN         use_static_passkey;
    UINT32          static_passkey;
    BOOLEAN         accept_specified_sec_auth;
    tSMP_AUTH_REQ   origin_loc_auth_req;
} tSMP_CB;

/* Server Action functions are of this type */
typedef void (*tSMP_ACT)(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);


#ifdef __cplusplus
extern "C"
{
#endif

#if SMP_DYNAMIC_MEMORY == FALSE
extern tSMP_CB  smp_cb;
#else
extern tSMP_CB *smp_cb_ptr;
#define smp_cb (*smp_cb_ptr)
#endif

#ifdef __cplusplus
}
#endif

/* Functions provided by att_main.c */
extern void smp_init (void);

/* smp main */
extern void smp_sm_event(tSMP_CB *p_cb, tSMP_EVENT event, void *p_data);

extern void smp_proc_sec_request(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void  smp_set_fail_nc (BOOLEAN enable);
extern void  smp_set_fail_conf (BOOLEAN enable);
extern void  smp_set_passk_entry_fail(BOOLEAN enable);
extern void  smp_set_oob_fail(BOOLEAN enable);
extern void  smp_set_peer_sc_notif(BOOLEAN enable);
extern void smp_aes_cmac_rfc4493_chk (UINT8 *key, UINT8 *msg, UINT8 msg_len,
                                      UINT8 mac_len, UINT8 *mac);
extern void smp_f4_calc_chk (UINT8 *U, UINT8 *V, UINT8 *X, UINT8 *Z, UINT8 *mac);
extern void smp_g2_calc_chk (UINT8 *U, UINT8 *V, UINT8 *X, UINT8 *Y);
extern void smp_h6_calc_chk (UINT8 *key, UINT8 *key_id, UINT8 *mac);
extern void smp_f5_key_calc_chk (UINT8 *w, UINT8 *mac);
extern void smp_f5_mackey_or_ltk_calc_chk(UINT8 *t, UINT8 *counter,
        UINT8 *key_id, UINT8 *n1,
        UINT8 *n2, UINT8 *a1, UINT8 *a2,
        UINT8 *length, UINT8 *mac);
extern void smp_f5_calc_chk (UINT8 *w, UINT8 *n1, UINT8 *n2, UINT8 *a1, UINT8 *a2,
                             UINT8 *mac_key, UINT8 *ltk);
extern void smp_f6_calc_chk (UINT8 *w, UINT8 *n1, UINT8 *n2, UINT8 *r,
                             UINT8 *iocap, UINT8 *a1, UINT8 *a2, UINT8 *mac);
/* smp_main */
extern void         smp_sm_event(tSMP_CB *p_cb, tSMP_EVENT event, void *p_data);
extern tSMP_STATE   smp_get_state(void);
extern void         smp_set_state(tSMP_STATE state);

/* smp_br_main */
extern void smp_br_state_machine_event(tSMP_CB *p_cb, tSMP_BR_EVENT event, void *p_data);
extern tSMP_BR_STATE    smp_get_br_state(void);
extern void             smp_set_br_state(tSMP_BR_STATE state);


/* smp_act.c */
extern void smp_send_pair_req(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_confirm(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_pair_fail(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_rand(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_pair_public_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_commitment(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_dhkey_check(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_keypress_notification(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_pair_fail(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_confirm(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_rand(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_pairing_public_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_enc_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_master_id(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_id_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_id_addr(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_sec_grant(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_sec_req(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_sl_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_start_enc(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_enc_cmpl(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_discard(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_pairing_cmpl(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_decide_association_model(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_app_cback(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_compare(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_check_auth_req(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_io_response(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_id_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_enc_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_csrk_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_ltk_reply(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_pair_cmd(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_pair_terminate(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_idle_terminate(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_send_pair_rsp(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_key_distribution(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_proc_srk_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_generate_csrk(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_fast_conn_param(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_key_pick_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_both_have_public_keys(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_start_secure_connection_phase1(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_local_nonce(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_pairing_commitment(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_peer_nonce(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_dhkey_check(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_match_dhkey_checks(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_keypress_notification(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_move_to_secure_connections_phase2(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_phase_2_dhkey_checks_are_present(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_wait_for_both_public_keys(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_start_passkey_verification(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_secure_connection_oob_data(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_process_secure_connection_long_term_key(void);
extern void smp_set_local_oob_keys(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_set_local_oob_random_commitment(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_set_derive_link_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_derive_link_key_from_long_term_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_br_process_pairing_command(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_br_process_security_grant(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_br_process_slave_keys_response(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_br_send_pair_response(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_br_check_authorization_request(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_br_select_next_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_br_process_link_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_key_distribution_by_transport(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_br_pairing_complete(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);

/* smp_l2c */
extern void smp_l2cap_if_init (void);
extern void smp_data_ind (BD_ADDR bd_addr, BT_HDR *p_buf);

/* smp_util.c */
extern BOOLEAN smp_send_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
extern void smp_cb_cleanup(tSMP_CB *p_cb);
extern void smp_reset_control_value(tSMP_CB *p_cb);
extern void smp_proc_pairing_cmpl(tSMP_CB *p_cb);
extern void smp_convert_string_to_tk(BT_OCTET16 tk, UINT32 passkey);
extern void smp_mask_enc_key(UINT8 loc_enc_size, UINT8 *p_data);
extern void smp_rsp_timeout(TIMER_LIST_ENT *p_tle);
extern void smp_xor_128(BT_OCTET16 a, const BT_OCTET16 b);
extern BOOLEAN smp_encrypt_data (UINT8 *key, UINT8 key_len,
                                 UINT8 *plain_text, UINT8 pt_len,
                                 tSMP_ENC *p_out);
extern BOOLEAN smp_command_has_invalid_parameters(tSMP_CB *p_cb);
extern void smp_reject_unexpected_pairing_command(BD_ADDR bd_addr);
extern tSMP_ASSO_MODEL smp_select_association_model(tSMP_CB *p_cb);
extern void smp_reverse_array(UINT8 *arr, UINT8 len);
extern UINT8 smp_calculate_random_input(UINT8 *random, UINT8 round);
extern void smp_collect_local_io_capabilities(UINT8 *iocap, tSMP_CB *p_cb);
extern void smp_collect_peer_io_capabilities(UINT8 *iocap, tSMP_CB *p_cb);
extern void smp_collect_local_ble_address(UINT8 *le_addr, tSMP_CB *p_cb);
extern void smp_collect_peer_ble_address(UINT8 *le_addr, tSMP_CB *p_cb);
extern BOOLEAN smp_check_commitment(tSMP_CB *p_cb);
extern void smp_save_secure_connections_long_term_key(tSMP_CB *p_cb);
extern BOOLEAN smp_calculate_f5_mackey_and_long_term_key(tSMP_CB *p_cb);
extern void smp_remove_fixed_channel(tSMP_CB *p_cb);
extern BOOLEAN smp_request_oob_data(tSMP_CB *p_cb);

/* smp_keys.c */
extern void smp_generate_srand_mrand_confirm (tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_generate_compare (tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_generate_stk (tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_generate_ltk(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_generate_passkey (tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_generate_rand_cont(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_create_private_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_use_oob_private_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_compute_dhkey(tSMP_CB *p_cb);
extern void smp_calculate_local_commitment(tSMP_CB *p_cb);
extern void smp_calculate_peer_commitment(tSMP_CB *p_cb, BT_OCTET16 output_buf);
extern void smp_calculate_numeric_comparison_display_number(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_calculate_local_dhkey_check(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_calculate_peer_dhkey_check(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
extern void smp_start_nonce_generation(tSMP_CB *p_cb);
extern BOOLEAN smp_calculate_link_key_from_long_term_key(tSMP_CB *p_cb);
extern BOOLEAN smp_calculate_long_term_key_from_link_key(tSMP_CB *p_cb);
extern void smp_calculate_f4(UINT8 *u, UINT8 *v, UINT8 *x, UINT8 z, UINT8 *c);
extern UINT32 smp_calculate_g2(UINT8 *u, UINT8 *v, UINT8 *x, UINT8 *y);
extern BOOLEAN smp_calculate_f5(UINT8 *w, UINT8 *n1, UINT8 *n2, UINT8 *a1, UINT8 *a2,
                                UINT8 *mac_key, UINT8 *ltk);
extern BOOLEAN smp_calculate_f5_mackey_or_long_term_key(UINT8 *t, UINT8 *counter,
        UINT8 *key_id, UINT8 *n1, UINT8 *n2, UINT8 *a1,
        UINT8 *a2, UINT8 *length, UINT8 *mac);
extern BOOLEAN smp_calculate_f5_key(UINT8 *w, UINT8 *t);
extern BOOLEAN smp_calculate_f6(UINT8 *w, UINT8 *n1, UINT8 *n2, UINT8 *r, UINT8 *iocap,
                                UINT8 *a1, UINT8 *a2, UINT8 *f3);
extern BOOLEAN smp_calculate_h6(UINT8 *w, UINT8 *keyid, UINT8 *h2);
#if SMP_DEBUG == TRUE
extern void smp_debug_print_nbyte_little_endian (UINT8 *p, const UINT8 *key_name,
        UINT8 len);
#endif

/* smp_cmac.c */
extern BOOLEAN aes_cipher_msg_auth_code(BT_OCTET16 key, UINT8 *input, UINT16 length,
                                        UINT16 tlen, UINT8 *p_signature);
extern void print128(BT_OCTET16 x, const UINT8 *key_name);

// #endif  ///BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE

#endif /* SMP_INT_H */
