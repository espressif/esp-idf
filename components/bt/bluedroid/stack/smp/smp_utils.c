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
 *  This file contains functions for the SMP L2CAP utility functions
 *
 ******************************************************************************/
#include "bt_target.h"

#if SMP_INCLUDED == TRUE

#include "bt_types.h"
//#include "bt_utils.h"
#include <string.h>
//#include <ctype.h>
#include "hcidefs.h"
#include "btm_ble_api.h"
#include "l2c_api.h"
#include "l2c_int.h"
#include "smp_int.h"
#include "controller.h"
#include "btm_int.h"

#define SMP_PAIRING_REQ_SIZE    7
#define SMP_CONFIRM_CMD_SIZE    (BT_OCTET16_LEN + 1)
#define SMP_RAND_CMD_SIZE       (BT_OCTET16_LEN + 1)
#define SMP_INIT_CMD_SIZE       (BT_OCTET16_LEN + 1)
#define SMP_ENC_INFO_SIZE       (BT_OCTET16_LEN + 1)
#define SMP_MASTER_ID_SIZE      (BT_OCTET8_LEN + 2 + 1)
#define SMP_ID_INFO_SIZE        (BT_OCTET16_LEN + 1)
#define SMP_ID_ADDR_SIZE        (BD_ADDR_LEN + 1 + 1)
#define SMP_SIGN_INFO_SIZE      (BT_OCTET16_LEN + 1)
#define SMP_PAIR_FAIL_SIZE      2
#define SMP_SECURITY_REQUEST_SIZE  2
#define SMP_PAIR_PUBL_KEY_SIZE  (1 /* opcode */ + (2*BT_OCTET32_LEN))
#define SMP_PAIR_COMMITM_SIZE           (1 /* opcode */ + BT_OCTET16_LEN /*Commitment*/)
#define SMP_PAIR_DHKEY_CHECK_SIZE       (1 /* opcode */ + BT_OCTET16_LEN /*DHKey Check*/)
#define SMP_PAIR_KEYPR_NOTIF_SIZE       (1 /* opcode */ + 1 /*Notif Type*/)

/* SMP command sizes per spec */
static const UINT8 smp_cmd_size_per_spec[] = {
    0,
    SMP_PAIRING_REQ_SIZE,       /* 0x01: pairing request */
    SMP_PAIRING_REQ_SIZE,       /* 0x02: pairing response */
    SMP_CONFIRM_CMD_SIZE,       /* 0x03: pairing confirm */
    SMP_RAND_CMD_SIZE,          /* 0x04: pairing random */
    SMP_PAIR_FAIL_SIZE,         /* 0x05: pairing failed */
    SMP_ENC_INFO_SIZE,          /* 0x06: encryption information */
    SMP_MASTER_ID_SIZE,         /* 0x07: master identification */
    SMP_ID_INFO_SIZE,           /* 0x08: identity information */
    SMP_ID_ADDR_SIZE,           /* 0x09: identity address information */
    SMP_SIGN_INFO_SIZE,         /* 0x0A: signing information */
    SMP_SECURITY_REQUEST_SIZE,  /* 0x0B: security request */
    SMP_PAIR_PUBL_KEY_SIZE,     /* 0x0C: pairing public key */
    SMP_PAIR_DHKEY_CHECK_SIZE,  /* 0x0D: pairing dhkey check */
    SMP_PAIR_KEYPR_NOTIF_SIZE,  /* 0x0E: pairing keypress notification */
    SMP_PAIR_COMMITM_SIZE       /* 0x0F: pairing commitment */
};

static BOOLEAN smp_parameter_unconditionally_valid(tSMP_CB *p_cb);
static BOOLEAN smp_parameter_unconditionally_invalid(tSMP_CB *p_cb);

/* type for SMP command length validation functions */
typedef BOOLEAN (*tSMP_CMD_LEN_VALID)(tSMP_CB *p_cb);

static BOOLEAN smp_command_has_valid_fixed_length(tSMP_CB *p_cb);

static const tSMP_CMD_LEN_VALID smp_cmd_len_is_valid[] = {
    smp_parameter_unconditionally_invalid,
    smp_command_has_valid_fixed_length, /* 0x01: pairing request */
    smp_command_has_valid_fixed_length, /* 0x02: pairing response */
    smp_command_has_valid_fixed_length, /* 0x03: pairing confirm */
    smp_command_has_valid_fixed_length, /* 0x04: pairing random */
    smp_command_has_valid_fixed_length, /* 0x05: pairing failed */
    smp_command_has_valid_fixed_length, /* 0x06: encryption information */
    smp_command_has_valid_fixed_length, /* 0x07: master identification */
    smp_command_has_valid_fixed_length, /* 0x08: identity information */
    smp_command_has_valid_fixed_length, /* 0x09: identity address information */
    smp_command_has_valid_fixed_length, /* 0x0A: signing information */
    smp_command_has_valid_fixed_length, /* 0x0B: security request */
    smp_command_has_valid_fixed_length, /* 0x0C: pairing public key */
    smp_command_has_valid_fixed_length, /* 0x0D: pairing dhkey check */
    smp_command_has_valid_fixed_length, /* 0x0E: pairing keypress notification */
    smp_command_has_valid_fixed_length  /* 0x0F: pairing commitment */
};

/* type for SMP command parameter ranges validation functions */
typedef BOOLEAN (*tSMP_CMD_PARAM_RANGES_VALID)(tSMP_CB *p_cb);

static BOOLEAN smp_pairing_request_response_parameters_are_valid(tSMP_CB *p_cb);
static BOOLEAN smp_pairing_keypress_notification_is_valid(tSMP_CB *p_cb);

static const tSMP_CMD_PARAM_RANGES_VALID smp_cmd_param_ranges_are_valid[] = {
    smp_parameter_unconditionally_invalid,
    smp_pairing_request_response_parameters_are_valid, /* 0x01: pairing request */
    smp_pairing_request_response_parameters_are_valid, /* 0x02: pairing response */
    smp_parameter_unconditionally_valid, /* 0x03: pairing confirm */
    smp_parameter_unconditionally_valid, /* 0x04: pairing random */
    smp_parameter_unconditionally_valid, /* 0x05: pairing failed */
    smp_parameter_unconditionally_valid, /* 0x06: encryption information */
    smp_parameter_unconditionally_valid, /* 0x07: master identification */
    smp_parameter_unconditionally_valid, /* 0x08: identity information */
    smp_parameter_unconditionally_valid, /* 0x09: identity address information */
    smp_parameter_unconditionally_valid, /* 0x0A: signing information */
    smp_parameter_unconditionally_valid, /* 0x0B: security request */
    smp_parameter_unconditionally_valid, /* 0x0C: pairing public key */
    smp_parameter_unconditionally_valid, /* 0x0D: pairing dhkey check */
    smp_pairing_keypress_notification_is_valid, /* 0x0E: pairing keypress notification */
    smp_parameter_unconditionally_valid /* 0x0F: pairing commitment */
};

/* type for action functions */
typedef BT_HDR *(*tSMP_CMD_ACT)(UINT8 cmd_code, tSMP_CB *p_cb);

static BT_HDR *smp_build_pairing_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_confirm_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_rand_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_pairing_fail(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_identity_info_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_encrypt_info_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_security_request(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_signing_info_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_master_id_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_id_addr_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_pair_public_key_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_pairing_commitment_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_pair_dhkey_check_cmd(UINT8 cmd_code, tSMP_CB *p_cb);
static BT_HDR *smp_build_pairing_keypress_notification_cmd(UINT8 cmd_code, tSMP_CB *p_cb);

static const tSMP_CMD_ACT smp_cmd_build_act[] = {
    NULL,
    smp_build_pairing_cmd,          /* 0x01: pairing request */
    smp_build_pairing_cmd,          /* 0x02: pairing response */
    smp_build_confirm_cmd,          /* 0x03: pairing confirm */
    smp_build_rand_cmd,             /* 0x04: pairing random */
    smp_build_pairing_fail,         /* 0x05: pairing failure */
    smp_build_encrypt_info_cmd,     /* 0x06: encryption information */
    smp_build_master_id_cmd,        /* 0x07: master identification */
    smp_build_identity_info_cmd,    /* 0x08: identity information */
    smp_build_id_addr_cmd,          /* 0x09: identity address information */
    smp_build_signing_info_cmd,     /* 0x0A: signing information */
    smp_build_security_request,     /* 0x0B: security request */
    smp_build_pair_public_key_cmd,  /* 0x0C: pairing public key */
    smp_build_pair_dhkey_check_cmd, /* 0x0D: pairing DHKey check */
    smp_build_pairing_keypress_notification_cmd, /* 0x0E: pairing keypress notification */
    smp_build_pairing_commitment_cmd /* 0x0F: pairing commitment */
};

static const UINT8 smp_association_table[2][SMP_IO_CAP_MAX][SMP_IO_CAP_MAX] = {
    /* display only */    /* Display Yes/No */   /* keyboard only */
    /* No Input/Output */ /* keyboard display */

    /* initiator */
    /* model = tbl[peer_io_caps][loc_io_caps] */
    /* Display Only */
    {   {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY
        },

        /* Display Yes/No */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY
        },

        /* Keyboard only */
        {
            SMP_MODEL_KEY_NOTIF, SMP_MODEL_KEY_NOTIF, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF
        },

        /* No Input No Output */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY
        },

        /* keyboard display */
        {
            SMP_MODEL_KEY_NOTIF, SMP_MODEL_KEY_NOTIF, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF
        }
    },

    /* responder */
    /* model = tbl[loc_io_caps][peer_io_caps] */
    /* Display Only */
    {   {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF
        },

        /* Display Yes/No */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF
        },

        /* keyboard only */
        {
            SMP_MODEL_PASSKEY, SMP_MODEL_PASSKEY, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY
        },

        /* No Input No Output */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY
        },

        /* keyboard display */
        {
            SMP_MODEL_PASSKEY, SMP_MODEL_PASSKEY, SMP_MODEL_KEY_NOTIF,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY
        }
    }
};

static const UINT8 smp_association_table_sc[2][SMP_IO_CAP_MAX][SMP_IO_CAP_MAX] = {
    /* display only */    /* Display Yes/No */   /* keyboard only */
    /* No InputOutput */  /* keyboard display */

    /* initiator */
    /* model = tbl[peer_io_caps][loc_io_caps] */

    /* Display Only */
    {   {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_ENT
        },

        /* Display Yes/No */
        {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP
        },

        /* keyboard only */
        {
            SMP_MODEL_SEC_CONN_PASSKEY_DISP, SMP_MODEL_SEC_CONN_PASSKEY_DISP, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_DISP
        },

        /* No Input No Output */
        {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS
        },

        /* keyboard display */
        {
            SMP_MODEL_SEC_CONN_PASSKEY_DISP, SMP_MODEL_SEC_CONN_NUM_COMP, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP
        }
    },

    /* responder */
    /* model = tbl[loc_io_caps][peer_io_caps] */

    /* Display Only */
    {   {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_DISP,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_DISP
        },

        /* Display Yes/No */
        {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP, SMP_MODEL_SEC_CONN_PASSKEY_DISP,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP
        },

        /* keyboard only */
        {
            SMP_MODEL_SEC_CONN_PASSKEY_ENT, SMP_MODEL_SEC_CONN_PASSKEY_ENT, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_ENT
        },

        /* No Input No Output */
        {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS
        },

        /* keyboard display */
        {
            SMP_MODEL_SEC_CONN_PASSKEY_ENT, SMP_MODEL_SEC_CONN_NUM_COMP, SMP_MODEL_SEC_CONN_PASSKEY_DISP,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP
        }
    }
};

static tSMP_ASSO_MODEL smp_select_legacy_association_model(tSMP_CB *p_cb);
static tSMP_ASSO_MODEL smp_select_association_model_secure_connections(tSMP_CB *p_cb);

/*******************************************************************************
**
** Function         smp_send_msg_to_L2CAP
**
** Description      Send message to L2CAP.
**
*******************************************************************************/
BOOLEAN  smp_send_msg_to_L2CAP(BD_ADDR rem_bda, BT_HDR *p_toL2CAP)
{
    UINT16 l2cap_ret;
    UINT16 fixed_cid = L2CAP_SMP_CID;

    if (smp_cb.smp_over_br) {
        fixed_cid = L2CAP_SMP_BR_CID;
    }

    SMP_TRACE_EVENT("%s", __FUNCTION__);
    smp_cb.total_tx_unacked += 1;

    if ((l2cap_ret = L2CA_SendFixedChnlData (fixed_cid, rem_bda, p_toL2CAP)) == L2CAP_DW_FAILED) {
        smp_cb.total_tx_unacked -= 1;
        SMP_TRACE_ERROR("SMP   failed to pass msg:0x%0x to L2CAP",
                        *((UINT8 *)(p_toL2CAP + 1) + p_toL2CAP->offset));
        return FALSE;
    } else {
        return TRUE;
    }
}

/*******************************************************************************
**
** Function         smp_send_cmd
**
** Description      send a SMP command on L2CAP channel.
**
*******************************************************************************/
BOOLEAN smp_send_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR *p_buf;
    BOOLEAN sent = FALSE;
    UINT8 failure = SMP_PAIR_INTERNAL_ERR;
    SMP_TRACE_EVENT("smp_send_cmd on l2cap cmd_code=0x%x\n", cmd_code);
    if ( cmd_code <= (SMP_OPCODE_MAX + 1 /* for SMP_OPCODE_PAIR_COMMITM */) &&
            smp_cmd_build_act[cmd_code] != NULL) {
        p_buf = (*smp_cmd_build_act[cmd_code])(cmd_code, p_cb);

        if (p_buf != NULL &&
                smp_send_msg_to_L2CAP(p_cb->pairing_bda, p_buf)) {
            sent = TRUE;

            btu_stop_timer (&p_cb->rsp_timer_ent);
            btu_start_timer (&p_cb->rsp_timer_ent, BTU_TTYPE_SMP_PAIRING_CMD,
                             SMP_WAIT_FOR_RSP_TOUT);
        }
    }

    if (!sent) {
        if (p_cb->smp_over_br) {
            smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &failure);
        } else {
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &failure);
        }
    }
    return sent;
}

/*******************************************************************************
**
** Function         smp_rsp_timeout
**
** Description      Called when SMP wait for SMP command response timer expires
**
** Returns          void
**
*******************************************************************************/
void smp_rsp_timeout(TIMER_LIST_ENT *p_tle)
{
    tSMP_CB   *p_cb = &smp_cb;
    UINT8 failure = SMP_RSP_TIMEOUT;
    UNUSED(p_tle);

    SMP_TRACE_EVENT("%s state:%d br_state:%d", __FUNCTION__, p_cb->state, p_cb->br_state);

    if (p_cb->smp_over_br) {
        smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &failure);
    } else {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &failure);
    }
}

/*******************************************************************************
**
** Function         smp_build_pairing_req_cmd
**
** Description      Build pairing request command.
**
*******************************************************************************/
BT_HDR *smp_build_pairing_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;

    SMP_TRACE_EVENT("smp_build_pairing_cmd");
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_PAIRING_REQ_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, cmd_code);
        UINT8_TO_STREAM (p, p_cb->local_io_capability);
        UINT8_TO_STREAM (p, p_cb->loc_oob_flag);
        UINT8_TO_STREAM (p, p_cb->loc_auth_req);
        UINT8_TO_STREAM (p, p_cb->loc_enc_size);
        UINT8_TO_STREAM (p, p_cb->local_i_key);
        UINT8_TO_STREAM (p, p_cb->local_r_key);

        p_buf->offset = L2CAP_MIN_OFFSET;
        /* 1B ERR_RSP op code + 1B cmd_op_code + 2B handle + 1B status */
        p_buf->len = SMP_PAIRING_REQ_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_confirm_cmd
**
** Description      Build confirm request command.
**
*******************************************************************************/
static BT_HDR *smp_build_confirm_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("smp_build_confirm_cmd\n");
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_CONFIRM_CMD_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_CONFIRM);
        ARRAY_TO_STREAM (p, p_cb->confirm, BT_OCTET16_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_CONFIRM_CMD_SIZE;
    }

    return p_buf;
}
/*******************************************************************************
**
** Function         smp_build_rand_cmd
**
** Description      Build Random command.
**
*******************************************************************************/
static BT_HDR *smp_build_rand_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("%s\n", __func__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_RAND_CMD_SIZE + L2CAP_MIN_OFFSET))
            != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_RAND);
        ARRAY_TO_STREAM (p, p_cb->rand, BT_OCTET16_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_RAND_CMD_SIZE;
    }

    return p_buf;
}
/*******************************************************************************
**
** Function         smp_build_encrypt_info_cmd
**
** Description      Build security information command.
**
*******************************************************************************/
static BT_HDR *smp_build_encrypt_info_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("smp_build_encrypt_info_cmd\n");
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_ENC_INFO_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_ENCRYPT_INFO);
        ARRAY_TO_STREAM (p, p_cb->ltk, BT_OCTET16_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_ENC_INFO_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_master_id_cmd
**
** Description      Build security information command.
**
*******************************************************************************/
static BT_HDR *smp_build_master_id_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("%s\n", __func__);

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_MASTER_ID_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_MASTER_ID);
        UINT16_TO_STREAM (p, p_cb->ediv);
        ARRAY_TO_STREAM (p, p_cb->enc_rand, BT_OCTET8_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_MASTER_ID_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_identity_info_cmd
**
** Description      Build identity information command.
**
*******************************************************************************/
static BT_HDR *smp_build_identity_info_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    BT_OCTET16  irk;
    UNUSED(cmd_code);
    UNUSED(p_cb);

    SMP_TRACE_EVENT("smp_build_identity_info_cmd\n");
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_ID_INFO_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        BTM_GetDeviceIDRoot(irk);

        UINT8_TO_STREAM (p, SMP_OPCODE_IDENTITY_INFO);
        ARRAY_TO_STREAM (p,  irk, BT_OCTET16_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_ID_INFO_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_id_addr_cmd
**
** Description      Build identity address information command.
**
*******************************************************************************/
static BT_HDR *smp_build_id_addr_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR *p_buf = NULL;
    UINT8 *p;

    UNUSED(cmd_code);
    UNUSED(p_cb);
    SMP_TRACE_EVENT("smp_build_id_addr_cmd\n");
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_ID_ADDR_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_ID_ADDR);
        UINT8_TO_STREAM (p, 0);
        BDADDR_TO_STREAM (p, controller_get_interface()->get_address()->address);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_ID_ADDR_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_signing_info_cmd
**
** Description      Build signing information command.
**
*******************************************************************************/
static BT_HDR *smp_build_signing_info_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("smp_build_signing_info_cmd\n");
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_SIGN_INFO_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_SIGN_INFO);
        ARRAY_TO_STREAM (p, p_cb->csrk, BT_OCTET16_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_SIGN_INFO_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_pairing_fail
**
** Description      Build Pairing Fail command.
**
*******************************************************************************/
static BT_HDR *smp_build_pairing_fail(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("%s\n", __func__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_PAIR_FAIL_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_PAIRING_FAILED);
        UINT8_TO_STREAM (p, p_cb->failure);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_PAIR_FAIL_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_security_request
**
** Description      Build security request command.
**
*******************************************************************************/
static BT_HDR *smp_build_security_request(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("%s\n", __func__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + 2 + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_SEC_REQ);
        UINT8_TO_STREAM (p,  p_cb->loc_auth_req);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_SECURITY_REQUEST_SIZE;

        SMP_TRACE_EVENT("opcode=%d auth_req=0x%x", SMP_OPCODE_SEC_REQ,  p_cb->loc_auth_req );
    }

    return p_buf;

}

/*******************************************************************************
**
** Function         smp_build_pair_public_key_cmd
**
** Description      Build pairing public key command.
**
*******************************************************************************/
static BT_HDR *smp_build_pair_public_key_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR  *p_buf = NULL ;
    UINT8   *p;
    UINT8   publ_key[2 * BT_OCTET32_LEN];
    UINT8   *p_publ_key = publ_key;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("%s\n", __FUNCTION__);

    memcpy(p_publ_key, p_cb->loc_publ_key.x, BT_OCTET32_LEN);
    memcpy(p_publ_key + BT_OCTET32_LEN, p_cb->loc_publ_key.y, BT_OCTET32_LEN);

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) +
                                      SMP_PAIR_PUBL_KEY_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_PAIR_PUBLIC_KEY);
        ARRAY_TO_STREAM (p, p_publ_key, 2 * BT_OCTET32_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_PAIR_PUBL_KEY_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_pairing_commitment_cmd
**
** Description      Build pairing commitment command.
**
*******************************************************************************/
static BT_HDR *smp_build_pairing_commitment_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR *p_buf = NULL;
    UINT8 *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("%s\n", __func__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + SMP_PAIR_COMMITM_SIZE + L2CAP_MIN_OFFSET))
            != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_CONFIRM);
        ARRAY_TO_STREAM (p, p_cb->commitment, BT_OCTET16_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_PAIR_COMMITM_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_pair_dhkey_check_cmd
**
** Description      Build pairing DHKey check command.
**
*******************************************************************************/
static BT_HDR *smp_build_pair_dhkey_check_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR *p_buf = NULL;
    UINT8 *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("%s\n", __FUNCTION__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) +
                                      SMP_PAIR_DHKEY_CHECK_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_PAIR_DHKEY_CHECK);
        ARRAY_TO_STREAM (p, p_cb->dhkey_check, BT_OCTET16_LEN);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_PAIR_DHKEY_CHECK_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_build_pairing_keypress_notification_cmd
**
** Description      Build keypress notification command.
**
*******************************************************************************/
static BT_HDR *smp_build_pairing_keypress_notification_cmd(UINT8 cmd_code, tSMP_CB *p_cb)
{
    BT_HDR      *p_buf = NULL ;
    UINT8       *p;
    UNUSED(cmd_code);

    SMP_TRACE_EVENT("%s\n", __FUNCTION__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR)\
                                      + SMP_PAIR_KEYPR_NOTIF_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_PAIR_KEYPR_NOTIF);
        UINT8_TO_STREAM (p, p_cb->local_keypress_notification);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_PAIR_KEYPR_NOTIF_SIZE;
    }

    return p_buf;
}

/*******************************************************************************
**
** Function         smp_convert_string_to_tk
**
** Description      This function is called to convert a 6 to 16 digits numeric
**                  character string into SMP TK.
**
**
** Returns          void
**
*******************************************************************************/
void smp_convert_string_to_tk(BT_OCTET16 tk, UINT32 passkey)
{
    UINT8   *p = tk;
    tSMP_KEY    key;
    SMP_TRACE_EVENT("smp_convert_string_to_tk\n");
    UINT32_TO_STREAM(p, passkey);

    key.key_type    = SMP_KEY_TYPE_TK;
    key.p_data      = tk;

    smp_sm_event(&smp_cb, SMP_KEY_READY_EVT, &key);
}

/*******************************************************************************
**
** Function         smp_mask_enc_key
**
** Description      This function is called to mask off the encryption key based
**                  on the maximum encryption key size.
**
**
** Returns          void
**
*******************************************************************************/
void smp_mask_enc_key(UINT8 loc_enc_size, UINT8 *p_data)
{
    SMP_TRACE_EVENT("smp_mask_enc_key\n");
    if (loc_enc_size < BT_OCTET16_LEN) {
        for (; loc_enc_size < BT_OCTET16_LEN; loc_enc_size ++) {
            * (p_data + loc_enc_size) = 0;
        }
    }
    return;
}

/*******************************************************************************
**
** Function         smp_xor_128
**
** Description      utility function to do an biteise exclusive-OR of two bit
**                  strings of the length of BT_OCTET16_LEN.
**
** Returns          void
**
*******************************************************************************/
void smp_xor_128(BT_OCTET16 a, BT_OCTET16 b)
{
    UINT8 i, *aa = a, *bb = b;

    SMP_TRACE_EVENT("smp_xor_128\n");
    for (i = 0; i < BT_OCTET16_LEN; i++) {
        aa[i] = aa[i] ^ bb[i];
    }
}

/*******************************************************************************
**
** Function         smp_cb_cleanup
**
** Description      Clean up SMP control block
**
** Returns          void
**
*******************************************************************************/
void smp_cb_cleanup(tSMP_CB   *p_cb)
{
    tSMP_CALLBACK   *p_callback = p_cb->p_callback;
    UINT8           trace_level = p_cb->trace_level;

    SMP_TRACE_EVENT("smp_cb_cleanup\n");

    memset(p_cb, 0, sizeof(tSMP_CB));
    p_cb->p_callback = p_callback;
    p_cb->trace_level = trace_level;
}

/*******************************************************************************
**
** Function         smp_remove_fixed_channel
**
** Description      This function is called to remove the fixed channel
**
** Returns          void
**
*******************************************************************************/
void smp_remove_fixed_channel(tSMP_CB *p_cb)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    if (p_cb->smp_over_br) {
        L2CA_RemoveFixedChnl (L2CAP_SMP_BR_CID, p_cb->pairing_bda);
    } else {
        L2CA_RemoveFixedChnl (L2CAP_SMP_CID, p_cb->pairing_bda);
    }
}

/*******************************************************************************
**
** Function         smp_reset_control_value
**
** Description      This function is called to reset the control block value when
**                  pairing procedure finished.
**
**
** Returns          void
**
*******************************************************************************/
void smp_reset_control_value(tSMP_CB *p_cb)
{
    SMP_TRACE_EVENT("smp_reset_control_value\n");
    btu_stop_timer (&p_cb->rsp_timer_ent);
    p_cb->flags = 0;
    /* set the link idle timer to drop the link when pairing is done
       usually service discovery will follow authentication complete, to avoid
       racing condition for a link down/up, set link idle timer to be
       SMP_LINK_TOUT_MIN to guarantee SMP key exchange */
    L2CA_SetIdleTimeoutByBdAddr(p_cb->pairing_bda, SMP_LINK_TOUT_MIN, BT_TRANSPORT_LE);

    /* We can tell L2CAP to remove the fixed channel (if it has one) */
    smp_remove_fixed_channel(p_cb);
    smp_cb_cleanup(p_cb);
}

/*******************************************************************************
**
** Function         smp_proc_pairing_cmpl
**
** Description      This function is called to process pairing complete
**
**
** Returns          void
**
*******************************************************************************/
void smp_proc_pairing_cmpl(tSMP_CB *p_cb)
{
    tSMP_EVT_DATA   evt_data = {0};
    tSMP_CALLBACK   *p_callback = p_cb->p_callback;
    BD_ADDR         pairing_bda;

    SMP_TRACE_DEBUG ("smp_proc_pairing_cmpl \n");

    evt_data.cmplt.reason = p_cb->status;
    evt_data.cmplt.smp_over_br = p_cb->smp_over_br;

    if (p_cb->status == SMP_SUCCESS) {
        evt_data.cmplt.sec_level = p_cb->sec_level;
    }

    evt_data.cmplt.is_pair_cancel  = FALSE;

    if (p_cb->is_pair_cancel) {
        evt_data.cmplt.is_pair_cancel = TRUE;
    }


    SMP_TRACE_DEBUG ("send SMP_COMPLT_EVT reason=0x%0x sec_level=0x%0x\n",
                     evt_data.cmplt.reason,
                     evt_data.cmplt.sec_level );

    memcpy (pairing_bda, p_cb->pairing_bda, BD_ADDR_LEN);

    if (p_cb->role == HCI_ROLE_SLAVE) {
        L2CA_EnableUpdateBleConnParams(p_cb->pairing_bda, TRUE);
    }
    smp_reset_control_value(p_cb);

    if (p_callback) {
        (*p_callback) (SMP_COMPLT_EVT, pairing_bda, &evt_data);
    }
}

/*******************************************************************************
**
** Function         smp_command_has_invalid_parameters
**
** Description      Checks if the received SMP command has invalid parameters i.e.
**                  if the command length is valid and the command parameters are
**                  inside specified range.
**                  It returns TRUE if the command has invalid parameters.
**
** Returns          TRUE if the command has invalid parameters, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN smp_command_has_invalid_parameters(tSMP_CB *p_cb)
{
    UINT8 cmd_code = p_cb->rcvd_cmd_code;

    SMP_TRACE_DEBUG("%s for cmd code 0x%02x\n", __func__, cmd_code);

    if ((cmd_code > (SMP_OPCODE_MAX + 1 /* for SMP_OPCODE_PAIR_COMMITM */)) ||
            (cmd_code < SMP_OPCODE_MIN)) {
        SMP_TRACE_WARNING("Somehow received command with the RESERVED code 0x%02x\n", cmd_code);
        return TRUE;
    }

    if (!(*smp_cmd_len_is_valid[cmd_code])(p_cb)) {
        return TRUE;
    }

    if (!(*smp_cmd_param_ranges_are_valid[cmd_code])(p_cb)) {
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         smp_command_has_valid_fixed_length
**
** Description      Checks if the received command size is equal to the size
**                  according to specs.
**
** Returns          TRUE if the command size is as expected, FALSE otherwise.
**
** Note             The command is expected to have fixed length.
*******************************************************************************/
BOOLEAN smp_command_has_valid_fixed_length(tSMP_CB *p_cb)
{
    UINT8   cmd_code = p_cb->rcvd_cmd_code;

    SMP_TRACE_DEBUG("%s for cmd code 0x%02x\n", __func__, cmd_code);

    if (p_cb->rcvd_cmd_len != smp_cmd_size_per_spec[cmd_code]) {
        SMP_TRACE_WARNING("Rcvd from the peer cmd 0x%02x with invalid length\
            0x%02x (per spec the length is 0x%02x).\n",
                          cmd_code, p_cb->rcvd_cmd_len, smp_cmd_size_per_spec[cmd_code]);
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         smp_pairing_request_response_parameters_are_valid
**
** Description      Validates parameter ranges in the received SMP command
**                  pairing request or pairing response.
**                  The parameters to validate:
**                  IO capability,
**                  OOB data flag,
**                  Bonding_flags in AuthReq
**                  Maximum encryption key size.
**                  Returns FALSE if at least one of these parameters is out of range.
**
*******************************************************************************/
BOOLEAN smp_pairing_request_response_parameters_are_valid(tSMP_CB *p_cb)
{
    UINT8   io_caps = p_cb->peer_io_caps;
    UINT8   oob_flag = p_cb->peer_oob_flag;
    UINT8   bond_flag = p_cb->peer_auth_req & 0x03; //0x03 is gen bond with appropriate mask
    UINT8   enc_size = p_cb->peer_enc_size;

    SMP_TRACE_DEBUG("%s for cmd code 0x%02x\n", __func__, p_cb->rcvd_cmd_code);

    if (io_caps >= BTM_IO_CAP_MAX) {
        SMP_TRACE_WARNING("Rcvd from the peer cmd 0x%02x with IO Capabilty \
            value (0x%02x) out of range).\n",
                          p_cb->rcvd_cmd_code, io_caps);
        return FALSE;
    }

    if (!((oob_flag == SMP_OOB_NONE) || (oob_flag == SMP_OOB_PRESENT))) {
        SMP_TRACE_WARNING("Rcvd from the peer cmd 0x%02x with OOB data flag value \
            (0x%02x) out of range).\n",
                          p_cb->rcvd_cmd_code, oob_flag);
        return FALSE;
    }

    if (!((bond_flag == SMP_AUTH_NO_BOND) || (bond_flag == SMP_AUTH_BOND))) {
        SMP_TRACE_WARNING("Rcvd from the peer cmd 0x%02x with Bonding_Flags value (0x%02x)\
                           out of range).\n",
                          p_cb->rcvd_cmd_code, bond_flag);
        return FALSE;
    }

    if ((enc_size < SMP_ENCR_KEY_SIZE_MIN) || (enc_size > SMP_ENCR_KEY_SIZE_MAX)) {
        SMP_TRACE_WARNING("Rcvd from the peer cmd 0x%02x with Maximum Encryption \
            Key value (0x%02x) out of range).\n",
                          p_cb->rcvd_cmd_code, enc_size);
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         smp_pairing_keypress_notification_is_valid
**
** Description      Validates Notification Type parameter range in the received SMP command
**                  pairing keypress notification.
**                  Returns FALSE if this parameter is out of range.
**
*******************************************************************************/
BOOLEAN smp_pairing_keypress_notification_is_valid(tSMP_CB *p_cb)
{
    tBTM_SP_KEY_TYPE keypress_notification = p_cb->peer_keypress_notification;

    SMP_TRACE_DEBUG("%s for cmd code 0x%02x\n", __func__, p_cb->rcvd_cmd_code);

    if (keypress_notification >= BTM_SP_KEY_OUT_OF_RANGE) {
        SMP_TRACE_WARNING("Rcvd from the peer cmd 0x%02x with Pairing Keypress \
            Notification value (0x%02x) out of range).\n",
                          p_cb->rcvd_cmd_code, keypress_notification);
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         smp_parameter_unconditionally_valid
**
** Description      Always returns TRUE.
**
*******************************************************************************/
BOOLEAN smp_parameter_unconditionally_valid(tSMP_CB *p_cb)
{
    return TRUE;
}

/*******************************************************************************
**
** Function         smp_parameter_unconditionally_invalid
**
** Description      Always returns FALSE.
**
*******************************************************************************/
BOOLEAN smp_parameter_unconditionally_invalid(tSMP_CB *p_cb)
{
    return FALSE;
}

/*******************************************************************************
**
** Function         smp_reject_unexpected_pairing_command
**
** Description      send pairing failure to an unexpected pairing command during
**                  an active pairing process.
**
** Returns          void
**
*******************************************************************************/
void smp_reject_unexpected_pairing_command(BD_ADDR bd_addr)
{
    BT_HDR *p_buf;
    UINT8   *p;

    SMP_TRACE_DEBUG ("%s\n", __FUNCTION__);

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + \
                                      SMP_PAIR_FAIL_SIZE + L2CAP_MIN_OFFSET)) != NULL) {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, SMP_OPCODE_PAIRING_FAILED);
        UINT8_TO_STREAM (p, SMP_PAIR_NOT_SUPPORT);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = SMP_PAIR_FAIL_SIZE;

        smp_send_msg_to_L2CAP(bd_addr, p_buf);
    }
}

/*******************************************************************************
** Function         smp_select_association_model
**
** Description      This function selects association model to use for STK
**                  generation. Selection is based on both sides' io capability,
**                  oob data flag and authentication request.
**
** Note             If Secure Connections Only mode is required locally then we
**                  come to this point only if both sides support Secure Connections
**                  mode, i.e. if p_cb->secure_connections_only_mode_required = TRUE then we come
**                  to this point only if
**                      (p_cb->peer_auth_req & SMP_SC_SUPPORT_BIT) ==
**                      (p_cb->loc_auth_req & SMP_SC_SUPPORT_BIT) ==
**                      SMP_SC_SUPPORT_BIT
**
*******************************************************************************/
tSMP_ASSO_MODEL smp_select_association_model(tSMP_CB *p_cb)
{
    tSMP_ASSO_MODEL model = SMP_MODEL_OUT_OF_RANGE;
    p_cb->le_secure_connections_mode_is_used = FALSE;

    SMP_TRACE_EVENT("%s\n", __FUNCTION__);
    SMP_TRACE_DEBUG("%s p_cb->peer_io_caps = %d p_cb->local_io_capability = %d\n",
                    __FUNCTION__, p_cb->peer_io_caps, p_cb->local_io_capability);
    SMP_TRACE_DEBUG("%s p_cb->peer_oob_flag = %d p_cb->loc_oob_flag = %d\n",
                    __FUNCTION__, p_cb->peer_oob_flag, p_cb->loc_oob_flag);
    SMP_TRACE_DEBUG("%s p_cb->peer_auth_req = 0x%02x p_cb->loc_auth_req = 0x%02x\n",
                    __FUNCTION__, p_cb->peer_auth_req, p_cb->loc_auth_req);
    SMP_TRACE_DEBUG("%s p_cb->secure_connections_only_mode_required = %s\n",
                    __FUNCTION__, p_cb->secure_connections_only_mode_required ?
                    "TRUE" : "FALSE");

    if ((p_cb->peer_auth_req & SMP_SC_SUPPORT_BIT) && (p_cb->loc_auth_req & SMP_SC_SUPPORT_BIT)) {
        p_cb->le_secure_connections_mode_is_used = TRUE;
    }

    SMP_TRACE_DEBUG("use_sc_process = %d\n", p_cb->le_secure_connections_mode_is_used);

    if (p_cb->le_secure_connections_mode_is_used) {
        model = smp_select_association_model_secure_connections(p_cb);
    } else {
        model = smp_select_legacy_association_model(p_cb);
    }
    return model;
}

/*******************************************************************************
** Function         smp_select_legacy_association_model
**
** Description      This function is called to select association mode if at least
**                  one side doesn't support secure connections.
**
*******************************************************************************/
tSMP_ASSO_MODEL smp_select_legacy_association_model(tSMP_CB *p_cb)
{
    tSMP_ASSO_MODEL model = SMP_MODEL_OUT_OF_RANGE;

    SMP_TRACE_DEBUG("%s\n", __func__);
    /* if OOB data is present on both devices, then use OOB association model */
    if (p_cb->peer_oob_flag == SMP_OOB_PRESENT && p_cb->loc_oob_flag == SMP_OOB_PRESENT) {
        return SMP_MODEL_OOB;
    }

    /* else if neither device requires MITM, then use Just Works association model */
    if (SMP_NO_MITM_REQUIRED (p_cb->peer_auth_req) && SMP_NO_MITM_REQUIRED(p_cb->loc_auth_req)) {
        return SMP_MODEL_ENCRYPTION_ONLY;
    }

    /* otherwise use IO capability to select association model */
    if (p_cb->peer_io_caps < SMP_IO_CAP_MAX && p_cb->local_io_capability < SMP_IO_CAP_MAX) {
        if (p_cb->role == HCI_ROLE_MASTER) {
            model = smp_association_table[p_cb->role][p_cb->peer_io_caps]
                    [p_cb->local_io_capability];
        } else {
            model = smp_association_table[p_cb->role][p_cb->local_io_capability]
                    [p_cb->peer_io_caps];
        }
    }

    return model;
}

/*******************************************************************************
** Function         smp_select_association_model_secure_connections
**
** Description      This function is called to select association mode if both
**                  sides support secure connections.
**
*******************************************************************************/
tSMP_ASSO_MODEL smp_select_association_model_secure_connections(tSMP_CB *p_cb)
{
    tSMP_ASSO_MODEL model = SMP_MODEL_OUT_OF_RANGE;

    SMP_TRACE_DEBUG("%s\n", __func__);
    /* if OOB data is present on at least one device, then use OOB association model */
    if (p_cb->peer_oob_flag == SMP_OOB_PRESENT || p_cb->loc_oob_flag == SMP_OOB_PRESENT) {
        return SMP_MODEL_SEC_CONN_OOB;
    }

    /* else if neither device requires MITM, then use Just Works association model */
    if (SMP_NO_MITM_REQUIRED (p_cb->peer_auth_req) && SMP_NO_MITM_REQUIRED(p_cb->loc_auth_req)) {
        return SMP_MODEL_SEC_CONN_JUSTWORKS;
    }

    /* otherwise use IO capability to select association model */
    if (p_cb->peer_io_caps < SMP_IO_CAP_MAX && p_cb->local_io_capability < SMP_IO_CAP_MAX) {
        if (p_cb->role == HCI_ROLE_MASTER) {
            model = smp_association_table_sc[p_cb->role][p_cb->peer_io_caps]
                    [p_cb->local_io_capability];
        } else {
            model = smp_association_table_sc[p_cb->role][p_cb->local_io_capability]
                    [p_cb->peer_io_caps];
        }
    }

    return model;
}

/*******************************************************************************
** Function         smp_reverse_array
**
** Description      This function reverses array bytes
**
*******************************************************************************/
void smp_reverse_array(UINT8 *arr, UINT8 len)
{
    UINT8 i = 0, tmp;

    SMP_TRACE_DEBUG("smp_reverse_array\n");

    for (i = 0; i < len / 2; i ++) {
        tmp = arr[i];
        arr[i] = arr[len - 1 - i];
        arr[len - 1 - i] = tmp;
    }
}

/*******************************************************************************
** Function         smp_calculate_random_input
**
** Description      This function returns random input value to be used in commitment
**                  calculation for SC passkey entry association mode
**                  (if bit["round"] in "random" array == 1 then returns 0x81
**                   else returns 0x80).
**
** Returns          ri value
**
*******************************************************************************/
UINT8 smp_calculate_random_input(UINT8 *random, UINT8 round)
{
    UINT8 i = round / 8;
    UINT8 j = round % 8;
    UINT8 ri;

    SMP_TRACE_DEBUG("random: 0x%02x, round: %d, i: %d, j: %d\n", random[i], round, i, j);
    ri = ((random[i] >> j) & 1) | 0x80;
    SMP_TRACE_DEBUG("%s ri=0x%02x\n", __func__, ri);
    return ri;
}

/*******************************************************************************
** Function         smp_collect_local_io_capabilities
**
** Description      This function puts into IOcap array local device
**                  IOCapability, OOB data, AuthReq.
**
** Returns          void
**
*******************************************************************************/
void smp_collect_local_io_capabilities(UINT8 *iocap, tSMP_CB *p_cb)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    iocap[0] = p_cb->local_io_capability;
    iocap[1] = p_cb->loc_oob_flag;
    iocap[2] = p_cb->loc_auth_req;
}

/*******************************************************************************
** Function         smp_collect_peer_io_capabilities
**
** Description      This function puts into IOcap array peer device
**                  IOCapability, OOB data, AuthReq.
**
** Returns          void
**
*******************************************************************************/
void smp_collect_peer_io_capabilities(UINT8 *iocap, tSMP_CB *p_cb)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    iocap[0] = p_cb->peer_io_caps;
    iocap[1] = p_cb->peer_oob_flag;
    iocap[2] = p_cb->peer_auth_req;
}

/*******************************************************************************
** Function         smp_collect_local_ble_address
**
** Description      This function puts into le_addr array local device le address:
**                  le_addr[0-5] = local BD ADDR,
**                  le_addr[6] = local le address type (PUBLIC/RANDOM).
**
** Returns          void
**
*******************************************************************************/
void smp_collect_local_ble_address(UINT8 *le_addr, tSMP_CB *p_cb)
{
    tBLE_ADDR_TYPE  addr_type = 0;
    BD_ADDR         bda;
    UINT8           *p = le_addr;

    SMP_TRACE_DEBUG("%s\n", __func__);

    BTM_ReadConnectionAddr( p_cb->pairing_bda, bda, &addr_type);
    BDADDR_TO_STREAM(p, bda);
    UINT8_TO_STREAM(p, addr_type);
}

/*******************************************************************************
** Function         smp_collect_peer_ble_address
**
** Description      This function puts into le_addr array peer device le address:
**                  le_addr[0-5] = peer BD ADDR,
**                  le_addr[6] = peer le address type (PUBLIC/RANDOM).
**
** Returns          void
**
*******************************************************************************/
void smp_collect_peer_ble_address(UINT8 *le_addr, tSMP_CB *p_cb)
{
    tBLE_ADDR_TYPE  addr_type = 0;
    BD_ADDR         bda;
    UINT8           *p = le_addr;

    SMP_TRACE_DEBUG("%s\n", __func__);

    if (!BTM_ReadRemoteConnectionAddr(p_cb->pairing_bda, bda, &addr_type)) {
        SMP_TRACE_ERROR("can not collect peer le addr information for unknown device\n");
        return;
    }

    BDADDR_TO_STREAM(p, bda);
    UINT8_TO_STREAM(p, addr_type);
}

/*******************************************************************************
** Function         smp_check_commitment
**
** Description      This function compares peer commitment values:
**                  - expected (i.e. calculated locally),
**                  - received from the peer.
**
** Returns          TRUE  if the values are the same
**                  FALSE otherwise
**
*******************************************************************************/
BOOLEAN smp_check_commitment(tSMP_CB *p_cb)
{
    BT_OCTET16 expected;

    SMP_TRACE_DEBUG("%s\n", __func__);

    smp_calculate_peer_commitment(p_cb, expected);
    print128(expected, (const UINT8 *)"calculated peer commitment");
    print128(p_cb->remote_commitment, (const UINT8 *)"received peer commitment");

    if (memcmp(p_cb->remote_commitment, expected, BT_OCTET16_LEN)) {
        SMP_TRACE_WARNING("Commitment check fails\n");
        return FALSE;
    }

    SMP_TRACE_DEBUG("Commitment check succeeds\n");
    return TRUE;
}

/*******************************************************************************
**
** Function         smp_save_secure_connections_long_term_key
**
** Description      The function saves SC LTK as BLE key for future use as local
**                  and/or peer key.
**
** Returns          void
**
*******************************************************************************/
void smp_save_secure_connections_long_term_key(tSMP_CB *p_cb)
{
    tBTM_LE_LENC_KEYS   lle_key;
    tBTM_LE_PENC_KEYS   ple_key;

    SMP_TRACE_DEBUG("%s-Save LTK as local LTK key\n", __func__);
    memcpy(lle_key.ltk, p_cb->ltk, BT_OCTET16_LEN);
    lle_key.div = 0;
    lle_key.key_size = p_cb->loc_enc_size;
    lle_key.sec_level = p_cb->sec_level;
    btm_sec_save_le_key(p_cb->pairing_bda, BTM_LE_KEY_LENC, (tBTM_LE_KEY_VALUE *)&lle_key, TRUE);

    SMP_TRACE_DEBUG("%s-Save LTK as peer LTK key\n", __func__);
    ple_key.ediv = 0;
    memset(ple_key.rand, 0, BT_OCTET8_LEN);
    memcpy(ple_key.ltk, p_cb->ltk, BT_OCTET16_LEN);
    ple_key.sec_level = p_cb->sec_level;
    ple_key.key_size  = p_cb->loc_enc_size;
    btm_sec_save_le_key(p_cb->pairing_bda, BTM_LE_KEY_PENC, (tBTM_LE_KEY_VALUE *)&ple_key, TRUE);
}

/*******************************************************************************
**
** Function         smp_calculate_f5_mackey_and_long_term_key
**
** Description      The function calculates MacKey and LTK and saves them in CB.
**                  To calculate MacKey and LTK it calls smp_calc_f5(...).
**                  MacKey is used in dhkey calculation, LTK is used to encrypt
**                  the link.
**
** Returns          FALSE if out of resources, TRUE otherwise.
**
*******************************************************************************/
BOOLEAN smp_calculate_f5_mackey_and_long_term_key(tSMP_CB *p_cb)
{
    UINT8 a[7];
    UINT8 b[7];
    UINT8 *p_na;
    UINT8 *p_nb;

    SMP_TRACE_DEBUG("%s\n", __func__);

    if (p_cb->role == HCI_ROLE_MASTER) {
        smp_collect_local_ble_address(a, p_cb);
        smp_collect_peer_ble_address(b, p_cb);
        p_na = p_cb->rand;
        p_nb = p_cb->rrand;
    } else {
        smp_collect_local_ble_address(b, p_cb);
        smp_collect_peer_ble_address(a, p_cb);
        p_na = p_cb->rrand;
        p_nb = p_cb->rand;
    }

    if (!smp_calculate_f5(p_cb->dhkey, p_na, p_nb, a, b, p_cb->mac_key, p_cb->ltk)) {
        SMP_TRACE_ERROR("%s failed\n", __func__);
        return FALSE;
    }

    SMP_TRACE_EVENT ("%s is completed\n", __func__);
    return TRUE;
}

/*******************************************************************************
**
** Function         smp_request_oob_data
**
** Description      Requests application to provide OOB data.
**
** Returns          TRUE - OOB data has to be provided by application
**                  FALSE - otherwise (unexpected)
**
*******************************************************************************/
BOOLEAN smp_request_oob_data(tSMP_CB *p_cb)
{
    tSMP_OOB_DATA_TYPE req_oob_type = SMP_OOB_INVALID_TYPE;

    SMP_TRACE_DEBUG("%s\n", __func__);

    if (p_cb->peer_oob_flag == SMP_OOB_PRESENT && p_cb->loc_oob_flag == SMP_OOB_PRESENT) {
        /* both local and peer rcvd data OOB */
        req_oob_type = SMP_OOB_BOTH;
    } else if (p_cb->peer_oob_flag == SMP_OOB_PRESENT) {
        /* peer rcvd OOB local data, local didn't receive OOB peer data */
        req_oob_type = SMP_OOB_LOCAL;
    } else if (p_cb->loc_oob_flag == SMP_OOB_PRESENT) {
        req_oob_type = SMP_OOB_PEER;
    }

    SMP_TRACE_DEBUG("req_oob_type = %d\n", req_oob_type);

    if (req_oob_type == SMP_OOB_INVALID_TYPE) {
        return FALSE;
    }

    p_cb->req_oob_type = req_oob_type;
    p_cb->cb_evt = SMP_SC_OOB_REQ_EVT;
    smp_sm_event(p_cb, SMP_TK_REQ_EVT, &req_oob_type);

    return TRUE;
}


#endif

