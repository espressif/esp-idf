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

#include <string.h>
#include "device/interop.h"
#include "common/bt_target.h"
#include "btm_int.h"
#include "stack/l2c_api.h"
#include "smp_int.h"
#include "p_256_ecc_pp.h"
//#include "utils/include/bt_utils.h"

#if SMP_INCLUDED == TRUE
const UINT8 smp_association_table[2][SMP_IO_CAP_MAX][SMP_IO_CAP_MAX] = {
    /* initiator */
    {   {SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY,   SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY}, /* Display Only */
        {SMP_MODEL_ENCRYPTION_ONLY,  SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY}, /* SMP_CAP_IO = 1 */
        {SMP_MODEL_KEY_NOTIF, SMP_MODEL_KEY_NOTIF,  SMP_MODEL_PASSKEY,   SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF}, /* keyboard only */
        {SMP_MODEL_ENCRYPTION_ONLY,  SMP_MODEL_ENCRYPTION_ONLY,   SMP_MODEL_ENCRYPTION_ONLY,  SMP_MODEL_ENCRYPTION_ONLY,    SMP_MODEL_ENCRYPTION_ONLY},/* No Input No Output */
        {SMP_MODEL_KEY_NOTIF, SMP_MODEL_KEY_NOTIF,  SMP_MODEL_PASSKEY,   SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF}
    }, /* keyboard display */
    /* responder */
    {   {SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY,   SMP_MODEL_KEY_NOTIF, SMP_MODEL_ENCRYPTION_ONLY,    SMP_MODEL_KEY_NOTIF}, /* Display Only */
        {SMP_MODEL_ENCRYPTION_ONLY,  SMP_MODEL_ENCRYPTION_ONLY,   SMP_MODEL_KEY_NOTIF,   SMP_MODEL_ENCRYPTION_ONLY,    SMP_MODEL_KEY_NOTIF}, /* SMP_CAP_IO = 1 */
        {SMP_MODEL_PASSKEY,   SMP_MODEL_PASSKEY,    SMP_MODEL_PASSKEY,   SMP_MODEL_ENCRYPTION_ONLY,    SMP_MODEL_PASSKEY}, /* keyboard only */
        {SMP_MODEL_ENCRYPTION_ONLY,  SMP_MODEL_ENCRYPTION_ONLY,   SMP_MODEL_ENCRYPTION_ONLY,  SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY},/* No Input No Output */
        {SMP_MODEL_PASSKEY,   SMP_MODEL_PASSKEY,    SMP_MODEL_KEY_NOTIF, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY}
    } /* keyboard display */
    /* display only */    /*SMP_CAP_IO = 1 */  /* keyboard only */   /* No InputOutput */  /* keyboard display */
};

#define SMP_KEY_DIST_TYPE_MAX       4
const tSMP_ACT smp_distribute_act [] = {
#if (BLE_INCLUDED == TRUE)
    smp_generate_ltk,
    smp_send_id_info,
    smp_generate_csrk,
    smp_set_derive_link_key
#else
    NULL,
    NULL,
    NULL,
    NULL
#endif  ///BLE_INCLUDED == TRUE
};

extern UINT8 bta_dm_co_ble_get_accept_auth_enable(void);
extern UINT8 bta_dm_co_ble_get_auth_req(void);

static bool lmp_version_below(BD_ADDR bda, uint8_t version)
{
    tACL_CONN *acl = btm_bda_to_acl(bda, BT_TRANSPORT_LE);
    if (acl == NULL || acl->lmp_version == 0) {
        SMP_TRACE_WARNING("%s cannot retrieve LMP version...", __func__);
        return false;
    }
    SMP_TRACE_DEBUG("%s LMP version %d < %d", __func__, acl->lmp_version, version);
    return acl->lmp_version < version;
}

/*******************************************************************************
** Function         smp_update_key_mask
** Description      This function updates the key mask for sending or receiving.
*******************************************************************************/
static void smp_update_key_mask (tSMP_CB *p_cb, UINT8 key_type, BOOLEAN recv)
{
    SMP_TRACE_DEBUG("%s before update role=%d recv=%d local_i_key = %02x, local_r_key = %02x\n",
                    __func__, p_cb->role, recv, p_cb->local_i_key, p_cb->local_r_key);

    if (((p_cb->le_secure_connections_mode_is_used) ||
            (p_cb->smp_over_br)) &&
            ((key_type == SMP_SEC_KEY_TYPE_ENC) || (key_type == SMP_SEC_KEY_TYPE_LK))) {
        /* in LE SC mode LTK, CSRK and BR/EDR LK are derived locally instead of
        ** being exchanged with the peer */
        p_cb->local_i_key &= ~key_type;
        p_cb->local_r_key &= ~key_type;
    } else if (p_cb->role == HCI_ROLE_SLAVE) {
        if (recv) {
            p_cb->local_i_key &= ~key_type;
        } else {
            p_cb->local_r_key &= ~key_type;
        }
    } else {
        if (recv) {
            p_cb->local_r_key &= ~key_type;
        } else {
            p_cb->local_i_key &= ~key_type;
        }
    }

    SMP_TRACE_DEBUG("updated local_i_key = %02x, local_r_key = %02x\n", p_cb->local_i_key,
                    p_cb->local_r_key);
}

/*******************************************************************************
** Function     smp_send_app_cback
** Description  notifies application about the events the application is interested in
*******************************************************************************/
void smp_send_app_cback(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    tSMP_EVT_DATA   cb_data;
    tSMP_STATUS callback_rc;
    SMP_TRACE_DEBUG("%s p_cb->cb_evt=%d\n", __func__, p_cb->cb_evt);
    if (p_cb->p_callback && p_cb->cb_evt != 0) {
        switch (p_cb->cb_evt) {
        case SMP_IO_CAP_REQ_EVT:
            cb_data.io_req.auth_req = p_cb->peer_auth_req;
            cb_data.io_req.oob_data = SMP_OOB_NONE;
            cb_data.io_req.io_cap = SMP_DEFAULT_IO_CAPS;
            cb_data.io_req.max_key_size = SMP_MAX_ENC_KEY_SIZE;
            cb_data.io_req.init_keys = p_cb->local_i_key ;
            cb_data.io_req.resp_keys = p_cb->local_r_key ;
            SMP_TRACE_DEBUG ( "io_cap = %d", cb_data.io_req.io_cap);
            break;

        case SMP_NC_REQ_EVT:
            cb_data.passkey = p_data->passkey;
            break;
        case SMP_SC_OOB_REQ_EVT:
            cb_data.req_oob_type = p_data->req_oob_type;
            break;
        case SMP_SC_LOC_OOB_DATA_UP_EVT:
            cb_data.loc_oob_data = p_cb->sc_oob_data.loc_oob_data;
            break;

        case SMP_BR_KEYS_REQ_EVT:
            cb_data.io_req.auth_req = 0;
            cb_data.io_req.oob_data = SMP_OOB_NONE;
            cb_data.io_req.io_cap = 0;
            cb_data.io_req.max_key_size = SMP_MAX_ENC_KEY_SIZE;
            cb_data.io_req.init_keys = SMP_BR_SEC_DEFAULT_KEY;
            cb_data.io_req.resp_keys = SMP_BR_SEC_DEFAULT_KEY;
            break;

        default:
            break;
        }

        callback_rc = (*p_cb->p_callback)(p_cb->cb_evt, p_cb->pairing_bda, &cb_data);

        SMP_TRACE_DEBUG("callback_rc=%d  p_cb->cb_evt=%d\n", callback_rc, p_cb->cb_evt );

        if (callback_rc == SMP_SUCCESS) {
            switch (p_cb->cb_evt) {
            case SMP_IO_CAP_REQ_EVT:
                p_cb->loc_auth_req   = cb_data.io_req.auth_req;
                p_cb->local_io_capability  = cb_data.io_req.io_cap;
                p_cb->loc_oob_flag = cb_data.io_req.oob_data;
                p_cb->loc_enc_size = cb_data.io_req.max_key_size;
                p_cb->local_i_key = cb_data.io_req.init_keys;
                p_cb->local_r_key = cb_data.io_req.resp_keys;

                if (!(p_cb->loc_auth_req & SMP_AUTH_BOND)) {
                    SMP_TRACE_WARNING ("Non bonding: No keys will be exchanged");
                    p_cb->local_i_key = 0;
                    p_cb->local_r_key = 0;
                }

                SMP_TRACE_DEBUG ("rcvd auth_req: 0x%02x, io_cap: %d \
                        loc_oob_flag: %d loc_enc_size: %d,"
                                    "local_i_key: 0x%02x, local_r_key: 0x%02x\n",
                                    p_cb->loc_auth_req, p_cb->local_io_capability, p_cb->loc_oob_flag,
                                    p_cb->loc_enc_size, p_cb->local_i_key, p_cb->local_r_key);

                p_cb->secure_connections_only_mode_required =
                    (btm_cb.security_mode == BTM_SEC_MODE_SC) ? TRUE : FALSE;

                if (p_cb->secure_connections_only_mode_required) {
                    p_cb->loc_auth_req |= SMP_SC_SUPPORT_BIT;
                }

                if (!(p_cb->loc_auth_req & SMP_SC_SUPPORT_BIT)
                        || lmp_version_below(p_cb->pairing_bda, HCI_PROTO_VERSION_4_2)
                        || interop_match(INTEROP_DISABLE_LE_SECURE_CONNECTIONS,
                                         (const bt_bdaddr_t *)&p_cb->pairing_bda)) {
                    p_cb->loc_auth_req &= ~SMP_KP_SUPPORT_BIT;
                    p_cb->local_i_key &= ~SMP_SEC_KEY_TYPE_LK;
                    p_cb->local_r_key &= ~SMP_SEC_KEY_TYPE_LK;
                }

                SMP_TRACE_DEBUG("set auth_req: 0x%02x, local_i_key: 0x%02x, local_r_key: 0x%02x\n",
                                  p_cb->loc_auth_req, p_cb->local_i_key, p_cb->local_r_key);

                smp_sm_event(p_cb, SMP_IO_RSP_EVT, NULL);
                break;
#if (CLASSIC_BT_INCLUDED == TRUE)
            case SMP_BR_KEYS_REQ_EVT:
                p_cb->loc_enc_size = cb_data.io_req.max_key_size;
                p_cb->local_i_key = cb_data.io_req.init_keys;
                p_cb->local_r_key = cb_data.io_req.resp_keys;

                p_cb->local_i_key &= ~SMP_SEC_KEY_TYPE_LK;
                p_cb->local_r_key &= ~SMP_SEC_KEY_TYPE_LK;

                SMP_TRACE_WARNING ( "for SMP over BR max_key_size: 0x%02x,\
                        local_i_key: 0x%02x, local_r_key: 0x%02x\n",
                                    p_cb->loc_enc_size, p_cb->local_i_key, p_cb->local_r_key);

                smp_br_state_machine_event(p_cb, SMP_BR_KEYS_RSP_EVT, NULL);
                break;
#endif  ///CLASSIC_BT_INCLUDED == TRUE
            }
        }
    }

    if (!p_cb->cb_evt && p_cb->discard_sec_req) {
        p_cb->discard_sec_req = FALSE;
        smp_sm_event(p_cb, SMP_DISCARD_SEC_REQ_EVT, NULL);
    }

    SMP_TRACE_DEBUG("%s return\n", __func__);
}

/*******************************************************************************
** Function     smp_send_pair_fail
** Description  pairing failure to peer device if needed.
*******************************************************************************/
void smp_send_pair_fail(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    p_cb->status = *(UINT8 *)p_data;
    p_cb->failure = *(UINT8 *)p_data;

    SMP_TRACE_DEBUG("%s status=%d failure=%d ", __func__, p_cb->status, p_cb->failure);

    if (p_cb->status <= SMP_MAX_FAIL_RSN_PER_SPEC && p_cb->status != SMP_SUCCESS) {
        smp_send_cmd(SMP_OPCODE_PAIRING_FAILED, p_cb);
        p_cb->wait_for_authorization_complete = TRUE;
    }
}

/*******************************************************************************
** Function     smp_send_pair_req
** Description  actions related to sending pairing request
*******************************************************************************/
void smp_send_pair_req(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

#if (BLE_INCLUDED == TRUE)
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (p_cb->pairing_bda);
    /* erase all keys when master sends pairing req*/
    if (p_dev_rec) {
        btm_sec_clear_ble_keys(p_dev_rec);
    }
#endif  ///BLE_INCLUDED == TRUE
    /* do not manipulate the key, let app decide,
       leave out to BTM to mandate key distribution for bonding case */
    smp_send_cmd(SMP_OPCODE_PAIRING_REQ, p_cb);
}

/*******************************************************************************
** Function     smp_send_pair_rsp
** Description  actions related to sending pairing response
*******************************************************************************/
void smp_send_pair_rsp(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

#if (BLE_INCLUDED == TRUE)
    p_cb->local_i_key &= p_cb->peer_i_key;
    p_cb->local_r_key &= p_cb->peer_r_key;

    if (smp_send_cmd (SMP_OPCODE_PAIRING_RSP, p_cb)) {
        if (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_OOB) {
            smp_use_oob_private_key(p_cb, NULL);
        } else {
            smp_decide_association_model(p_cb, NULL);
        }
    }
#endif  ///BLE_INCLUDED == TRUE
}

/*******************************************************************************
** Function     smp_send_confirm
** Description  send confirmation to the peer
*******************************************************************************/
void smp_send_confirm(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_send_cmd(SMP_OPCODE_CONFIRM, p_cb);
}

#if 0 //Unused
/*******************************************************************************
** Function     smp_send_init
** Description  process pairing initializer to slave device
*******************************************************************************/
void smp_send_init(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_send_cmd(SMP_OPCODE_INIT, p_cb);
}
#endif

/*******************************************************************************
** Function     smp_send_rand
** Description  send pairing random to the peer
*******************************************************************************/
void smp_send_rand(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_send_cmd(SMP_OPCODE_RAND, p_cb);
}

/*******************************************************************************
** Function     smp_send_pair_public_key
** Description  send pairing public key command to the peer
*******************************************************************************/
void smp_send_pair_public_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_send_cmd(SMP_OPCODE_PAIR_PUBLIC_KEY, p_cb);
}

/*******************************************************************************
** Function     SMP_SEND_COMMITMENT
** Description send commitment command to the peer
*******************************************************************************/
void smp_send_commitment(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s", __func__);
    smp_send_cmd(SMP_OPCODE_PAIR_COMMITM, p_cb);
}

/*******************************************************************************
** Function     smp_send_dhkey_check
** Description send DHKey Check command to the peer
*******************************************************************************/
void smp_send_dhkey_check(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s", __func__);
    smp_send_cmd(SMP_OPCODE_PAIR_DHKEY_CHECK, p_cb);
}

/*******************************************************************************
** Function     smp_send_keypress_notification
** Description send Keypress Notification command to the peer
*******************************************************************************/
void smp_send_keypress_notification(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    p_cb->local_keypress_notification = *(UINT8 *) p_data;
    smp_send_cmd(SMP_OPCODE_PAIR_KEYPR_NOTIF, p_cb);
}

/*******************************************************************************
** Function     smp_send_enc_info
** Description  send encryption information command.
*******************************************************************************/
void smp_send_enc_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    tBTM_LE_LENC_KEYS   le_key;

    SMP_TRACE_DEBUG("%s p_cb->loc_enc_size = %d\n", __func__, p_cb->loc_enc_size);
    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_ENC, FALSE);

    smp_send_cmd(SMP_OPCODE_ENCRYPT_INFO, p_cb);
    smp_send_cmd(SMP_OPCODE_MASTER_ID, p_cb);

    /* save the DIV and key size information when acting as slave device */
    memcpy(le_key.ltk, p_cb->ltk, BT_OCTET16_LEN);
    le_key.div =  p_cb->div;
    le_key.key_size = p_cb->loc_enc_size;
    le_key.sec_level = p_cb->sec_level;

#if (BLE_INCLUDED == TRUE)
    if ((p_cb->peer_auth_req & SMP_AUTH_BOND) && (p_cb->loc_auth_req & SMP_AUTH_BOND)) {
        btm_sec_save_le_key(p_cb->pairing_bda, BTM_LE_KEY_LENC,
                            (tBTM_LE_KEY_VALUE *)&le_key, TRUE);
    }

    SMP_TRACE_DEBUG ("%s\n", __func__);

    smp_key_distribution(p_cb, NULL);
#endif  ///BLE_INCLUDED == TRUE
}

/*******************************************************************************
** Function     smp_send_id_info
** Description  send ID information command.
*******************************************************************************/
void smp_send_id_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_ID, FALSE);

    smp_send_cmd(SMP_OPCODE_IDENTITY_INFO, p_cb);
    smp_send_cmd(SMP_OPCODE_ID_ADDR, p_cb);

#if (BLE_INCLUDED == TRUE)
    tBTM_LE_KEY_VALUE   le_key;
    if ((p_cb->peer_auth_req & SMP_AUTH_BOND) && (p_cb->loc_auth_req & SMP_AUTH_BOND)) {
        btm_sec_save_le_key(p_cb->pairing_bda, BTM_LE_KEY_LID,
                            &le_key, TRUE);
    }
#endif  ///BLE_INCLUDED == TRUE

    smp_key_distribution_by_transport(p_cb, NULL);
}

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
** Function     smp_send_csrk_info
** Description  send CSRK command.
*******************************************************************************/
void smp_send_csrk_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    tBTM_LE_LCSRK_KEYS  key;
    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_CSRK, FALSE);

    if (smp_send_cmd(SMP_OPCODE_SIGN_INFO, p_cb)) {
        key.div = p_cb->div;
        key.sec_level = p_cb->sec_level;
        key.counter = 0; /* initialize the local counter */
        memcpy (key.csrk, p_cb->csrk, BT_OCTET16_LEN);
        btm_sec_save_le_key(p_cb->pairing_bda, BTM_LE_KEY_LCSRK, (tBTM_LE_KEY_VALUE *)&key, TRUE);
    }

    smp_key_distribution_by_transport(p_cb, NULL);
}

/*******************************************************************************
** Function     smp_send_ltk_reply
** Description  send LTK reply
*******************************************************************************/
void smp_send_ltk_reply(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s", __func__);
    /* send stk as LTK response */
    btm_ble_ltk_request_reply(p_cb->pairing_bda, TRUE, p_data->key.p_data);
}

/*******************************************************************************
** Function     smp_proc_sec_req
** Description  process security request.
*******************************************************************************/
void smp_proc_sec_req(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    tBTM_LE_AUTH_REQ auth_req = *(tBTM_LE_AUTH_REQ *)p_data;
    tBTM_BLE_SEC_REQ_ACT sec_req_act;
    UINT8 reason;

    SMP_TRACE_DEBUG("%s auth_req=0x%x", __func__, auth_req);

    p_cb->cb_evt = 0;
    btm_ble_link_sec_check(p_cb->pairing_bda, auth_req,  &sec_req_act);

    SMP_TRACE_DEBUG("%s sec_req_act=0x%x", __func__, sec_req_act);

    switch (sec_req_act) {
    case  BTM_BLE_SEC_REQ_ACT_ENCRYPT:
        SMP_TRACE_DEBUG("%s BTM_BLE_SEC_REQ_ACT_ENCRYPT", __func__);
        smp_sm_event(p_cb, SMP_ENC_REQ_EVT, NULL);
        break;

    case BTM_BLE_SEC_REQ_ACT_PAIR:
        p_cb->secure_connections_only_mode_required =
            (btm_cb.security_mode == BTM_SEC_MODE_SC) ? TRUE : FALSE;

        /* respond to non SC pairing request as failure in SC only mode */
        if (p_cb->secure_connections_only_mode_required &&
                (auth_req & SMP_SC_SUPPORT_BIT) == 0) {
            reason = SMP_PAIR_AUTH_FAIL;
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        } else {
            /* initialize local i/r key to be default keys */
            p_cb->peer_auth_req = auth_req;
            p_cb->local_r_key = p_cb->local_i_key = SMP_SEC_DEFAULT_KEY ;
            p_cb->cb_evt = SMP_SEC_REQUEST_EVT;
        }
        break;

    case BTM_BLE_SEC_REQ_ACT_DISCARD:
        p_cb->discard_sec_req = TRUE;
        break;

    default:
        /* do nothing */
        break;
    }
}
#endif  ///BLE_INCLUDED == TRUE

/*******************************************************************************
** Function     smp_proc_sec_grant
** Description  process security grant.
*******************************************************************************/
void smp_proc_sec_grant(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 res = *(UINT8 *)p_data;
    SMP_TRACE_DEBUG("%s", __func__);
    if (res != SMP_SUCCESS) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, p_data);
    } else { /*otherwise, start pairing */
        /* send IO request callback */
        p_cb->cb_evt = SMP_IO_CAP_REQ_EVT;
    }
}

/*******************************************************************************
** Function     smp_proc_pair_fail
** Description  process pairing failure from peer device
*******************************************************************************/
void smp_proc_pair_fail(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s", __func__);
    p_cb->status = *(UINT8 *)p_data;
}

/*******************************************************************************
** Function     smp_get_auth_mode
** Description  Get the SMP pairing auth mode
*******************************************************************************/
uint16_t smp_get_auth_mode (tSMP_ASSO_MODEL model)
{
    SMP_TRACE_DEBUG("%s model %d", __func__, model);
    uint16_t auth = 0;
    if (model == SMP_MODEL_ENCRYPTION_ONLY || model == SMP_MODEL_SEC_CONN_JUSTWORKS) {
        //No MITM
        if(model == SMP_MODEL_SEC_CONN_JUSTWORKS) {
            //SC SMP_SC_SUPPORT_BIT
            auth |=  SMP_SC_SUPPORT_BIT;
        }
    } else if (model <= SMP_MODEL_KEY_NOTIF) {
        //NO SC, MITM
        auth |= SMP_AUTH_YN_BIT;
    } else if (model <= SMP_MODEL_SEC_CONN_OOB) {
        //SC, MITM
        auth |= SMP_SC_SUPPORT_BIT;
        auth |= SMP_AUTH_YN_BIT;
    } else {
        auth = 0;
    }
    return auth;
}

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
** Function     smp_proc_pair_cmd
** Description  Process the SMP pairing request/response from peer device
*******************************************************************************/
void smp_proc_pair_cmd(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   *p = (UINT8 *)p_data;
    UINT8   reason = SMP_ENC_KEY_SIZE;
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (p_cb->pairing_bda);

    SMP_TRACE_DEBUG("%s\n", __func__);
    /* erase all keys if it is slave proc pairing req*/
    if (p_dev_rec && (p_cb->role == HCI_ROLE_SLAVE)) {
        btm_sec_clear_ble_keys(p_dev_rec);
    }

    p_cb->flags |= SMP_PAIR_FLAG_ENC_AFTER_PAIR;

    STREAM_TO_UINT8(p_cb->peer_io_caps, p);
    STREAM_TO_UINT8(p_cb->peer_oob_flag, p);
    STREAM_TO_UINT8(p_cb->peer_auth_req, p);
    STREAM_TO_UINT8(p_cb->peer_enc_size, p);
    STREAM_TO_UINT8(p_cb->peer_i_key, p);
    STREAM_TO_UINT8(p_cb->peer_r_key, p);

    if (smp_command_has_invalid_parameters(p_cb)) {
        reason = SMP_INVALID_PARAMETERS;
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }
    p_cb->accept_specified_sec_auth = bta_dm_co_ble_get_accept_auth_enable();
    p_cb->origin_loc_auth_req = bta_dm_co_ble_get_auth_req();
    if (p_cb->role == HCI_ROLE_SLAVE) {
        if (!(p_cb->flags & SMP_PAIR_FLAGS_WE_STARTED_DD)) {
            /* peer (master) started pairing sending Pairing Request */
            p_cb->local_i_key = p_cb->peer_i_key;
            p_cb->local_r_key = p_cb->peer_r_key;

            p_cb->cb_evt = SMP_SEC_REQUEST_EVT;
        } else { /* update local i/r key according to pairing request */
            /* pairing started with this side (slave) sending Security Request */
            p_cb->local_i_key &= p_cb->peer_i_key;
            p_cb->local_r_key &= p_cb->peer_r_key;
            p_cb->selected_association_model = smp_select_association_model(p_cb);

            if (p_cb->secure_connections_only_mode_required &&
                    (!(p_cb->le_secure_connections_mode_is_used) ||
                     (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_JUSTWORKS))) {
                SMP_TRACE_ERROR("%s pairing failed - slave requires secure connection only mode",
                                __func__);
                reason = SMP_PAIR_AUTH_FAIL;
                smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
                return;
            }
            uint16_t auth = smp_get_auth_mode(p_cb->selected_association_model);
            if(p_cb->peer_auth_req & p_cb->loc_auth_req & SMP_AUTH_GEN_BOND) {
                auth |= SMP_AUTH_GEN_BOND;
            }
            p_cb->auth_mode = auth;
            if (p_cb->accept_specified_sec_auth) {
                if ((auth & p_cb->origin_loc_auth_req) != p_cb->origin_loc_auth_req ) {
                    SMP_TRACE_ERROR("%s pairing failed - slave requires auth is 0x%x but peer auth is 0x%x local auth is 0x%x",
                                    __func__, p_cb->origin_loc_auth_req, p_cb->peer_auth_req, p_cb->loc_auth_req);
                    if (BTM_IsAclConnectionUp(p_cb->pairing_bda, BT_TRANSPORT_LE)) {
                        btm_remove_acl (p_cb->pairing_bda, BT_TRANSPORT_LE);
                    }
                    reason = SMP_PAIR_AUTH_FAIL;
                    smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
                    return;
                }
            }

            if (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_OOB && p_cb->loc_oob_flag == SMP_OOB_PRESENT) {
                if (smp_request_oob_data(p_cb)) {
                    return;
                }
            } else {
                smp_send_pair_rsp(p_cb, NULL);
            }
        }
    } else { /* Master receives pairing response */
        p_cb->selected_association_model = smp_select_association_model(p_cb);

        if (p_cb->secure_connections_only_mode_required &&
                (!(p_cb->le_secure_connections_mode_is_used) ||
                 (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_JUSTWORKS))) {
            SMP_TRACE_ERROR ("Master requires secure connection only mode \
                but it can't be provided -> Master fails pairing");
            reason = SMP_PAIR_AUTH_FAIL;
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
            return;
        }

        uint16_t auth = smp_get_auth_mode(p_cb->selected_association_model);
        if(p_cb->peer_auth_req & p_cb->loc_auth_req & SMP_AUTH_GEN_BOND) {
            auth |= SMP_AUTH_GEN_BOND;
        }
        p_cb->auth_mode = auth;
        if (p_cb->accept_specified_sec_auth) {
            if ((auth & p_cb->origin_loc_auth_req) != p_cb->origin_loc_auth_req ) {
                SMP_TRACE_ERROR("%s pairing failed - master requires auth is 0x%x but peer auth is 0x%x local auth is 0x%x",
                                    __func__, p_cb->origin_loc_auth_req, p_cb->peer_auth_req, p_cb->loc_auth_req);
                if (BTM_IsAclConnectionUp(p_cb->pairing_bda, BT_TRANSPORT_LE)) {
                    btm_remove_acl (p_cb->pairing_bda, BT_TRANSPORT_LE);
                }
                reason = SMP_PAIR_AUTH_FAIL;
                smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
                return;
            }
        }

        /* Only if peer oob data present, then should request peer oob data */
        if (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_OOB && p_cb->loc_oob_flag == SMP_OOB_PRESENT) {
            if (smp_request_oob_data(p_cb)) {
                return;
            }
        } else {
            smp_decide_association_model(p_cb, NULL);
        }
    }
}
#endif  ///BLE_INCLUDED == TRUE

/*******************************************************************************
** Function     smp_proc_confirm
** Description  process pairing confirm from peer device
*******************************************************************************/
void smp_proc_confirm(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 *p = (UINT8 *)p_data;
    UINT8 reason = SMP_INVALID_PARAMETERS;

    SMP_TRACE_DEBUG("%s\n", __func__);

    if (smp_command_has_invalid_parameters(p_cb)) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    if (p != NULL) {
        /* save the SConfirm for comparison later */
        STREAM_TO_ARRAY(p_cb->rconfirm, p, BT_OCTET16_LEN);
    }

    p_cb->flags |= SMP_PAIR_FLAGS_CMD_CONFIRM;
}

#if 0 //Unused
/*******************************************************************************
** Function     smp_proc_init
** Description  process pairing initializer from peer device
*******************************************************************************/
void smp_proc_init(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 *p = (UINT8 *)p_data;
    UINT8 reason = SMP_INVALID_PARAMETERS;

    SMP_TRACE_DEBUG("%s", __func__);

    if (smp_command_has_invalid_parameters(p_cb)) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    /* save the SRand for comparison */
    STREAM_TO_ARRAY(p_cb->rrand, p, BT_OCTET16_LEN);
}
#endif

/*******************************************************************************
** Function     smp_proc_rand
** Description  process pairing random (nonce) from peer device
*******************************************************************************/
void smp_proc_rand(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 *p = (UINT8 *)p_data;
    UINT8 reason = SMP_INVALID_PARAMETERS;

    SMP_TRACE_DEBUG("%s\n", __func__);

    if (smp_command_has_invalid_parameters(p_cb)) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    /* save the SRand for comparison */
    STREAM_TO_ARRAY(p_cb->rrand, p, BT_OCTET16_LEN);
}

/*******************************************************************************
** Function     smp_process_pairing_public_key
** Description  process pairing public key command from the peer device
**              - saves the peer public key;
**              - sets the flag indicating that the peer public key is received;
**              - calls smp_wait_for_both_public_keys(...).
**
*******************************************************************************/
void smp_process_pairing_public_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 *p = (UINT8 *)p_data;
    UINT8 reason = SMP_INVALID_PARAMETERS;

    SMP_TRACE_DEBUG("%s", __func__);

    if (smp_command_has_invalid_parameters(p_cb)) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    STREAM_TO_ARRAY(p_cb->peer_publ_key.x, p, BT_OCTET32_LEN);
    STREAM_TO_ARRAY(p_cb->peer_publ_key.y, p, BT_OCTET32_LEN);

    /* Check if the peer device's and own public key are not same. If they are same then
     * return pairing fail. This check is needed to avoid 'Impersonation in Passkey entry
     * protocol' vulnerability (CVE-2020-26558).*/
    if ((memcmp(p_cb->loc_publ_key.x, p_cb->peer_publ_key.x, sizeof(BT_OCTET32)) == 0)) {
        p_cb->status = SMP_PAIR_AUTH_FAIL;
        p_cb->failure = SMP_PAIR_AUTH_FAIL;
        reason = SMP_PAIR_AUTH_FAIL;
        SMP_TRACE_ERROR("%s, Peer and own device cannot have same public key.", __func__);
        smp_sm_event(p_cb, SMP_PAIRING_FAILED_EVT, &reason);
        return ;
    }
    /* In order to prevent the x and y coordinates of the public key from being modified,
       we need to check whether the x and y coordinates are on the given elliptic curve. */
    if (!ECC_CheckPointIsInElliCur_P256((Point *)&p_cb->peer_publ_key)) {
        SMP_TRACE_ERROR("%s, Invalid Public key.", __func__);
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
    }
    p_cb->flags |= SMP_PAIR_FLAG_HAVE_PEER_PUBL_KEY;

    smp_wait_for_both_public_keys(p_cb, NULL);
}

/*******************************************************************************
** Function     smp_process_pairing_commitment
** Description  process pairing commitment from peer device
*******************************************************************************/
void smp_process_pairing_commitment(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 *p = (UINT8 *)p_data;
    UINT8 reason = SMP_INVALID_PARAMETERS;

    SMP_TRACE_DEBUG("%s", __func__);

    if (smp_command_has_invalid_parameters(p_cb)) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    p_cb->flags |= SMP_PAIR_FLAG_HAVE_PEER_COMM;

    if (p != NULL) {
        STREAM_TO_ARRAY(p_cb->remote_commitment, p, BT_OCTET16_LEN);
    }
}

/*******************************************************************************
** Function     smp_process_dhkey_check
** Description  process DHKey Check from peer device
*******************************************************************************/
void smp_process_dhkey_check(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 *p = (UINT8 *)p_data;
    UINT8 reason = SMP_INVALID_PARAMETERS;

    SMP_TRACE_DEBUG("%s", __func__);

    if (smp_command_has_invalid_parameters(p_cb)) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    if (p != NULL) {
        STREAM_TO_ARRAY(p_cb->remote_dhkey_check, p, BT_OCTET16_LEN);
    }

    p_cb->flags |= SMP_PAIR_FLAG_HAVE_PEER_DHK_CHK;
}

/*******************************************************************************
** Function     smp_process_keypress_notification
** Description  process pairing keypress notification from peer device
*******************************************************************************/
void smp_process_keypress_notification(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 *p = (UINT8 *)p_data;
    UINT8 reason = SMP_INVALID_PARAMETERS;

    SMP_TRACE_DEBUG("%s", __func__);
    p_cb->status = *(UINT8 *)p_data;

    if (smp_command_has_invalid_parameters(p_cb)) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    if (p != NULL) {
        STREAM_TO_UINT8(p_cb->peer_keypress_notification, p);
    } else {
        p_cb->peer_keypress_notification = BTM_SP_KEY_OUT_OF_RANGE;
    }
    p_cb->cb_evt = SMP_PEER_KEYPR_NOT_EVT;
}

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
** Function     smp_br_process_pairing_command
** Description  Process the SMP pairing request/response from peer device via
**              BR/EDR transport.
*******************************************************************************/
void smp_br_process_pairing_command(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   *p = (UINT8 *)p_data;
    UINT8   reason = SMP_ENC_KEY_SIZE;
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (p_cb->pairing_bda);

    SMP_TRACE_DEBUG("%s", __func__);
    /* rejecting BR pairing request over non-SC BR link */
    if (!p_dev_rec->new_encryption_key_is_p256 && p_cb->role == HCI_ROLE_SLAVE) {
        reason = SMP_XTRANS_DERIVE_NOT_ALLOW;
        smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &reason);
        return;
    }

#if (BLE_INCLUDED == TRUE)
    /* erase all keys if it is slave proc pairing req*/
    if (p_dev_rec && (p_cb->role == HCI_ROLE_SLAVE)) {
        btm_sec_clear_ble_keys(p_dev_rec);
    }
#endif  ///BLE_INCLUDED == TRUE

    p_cb->flags |= SMP_PAIR_FLAG_ENC_AFTER_PAIR;

    STREAM_TO_UINT8(p_cb->peer_io_caps, p);
    STREAM_TO_UINT8(p_cb->peer_oob_flag, p);
    STREAM_TO_UINT8(p_cb->peer_auth_req, p);
    STREAM_TO_UINT8(p_cb->peer_enc_size, p);
    STREAM_TO_UINT8(p_cb->peer_i_key, p);
    STREAM_TO_UINT8(p_cb->peer_r_key, p);

    if (smp_command_has_invalid_parameters(p_cb)) {
        reason = SMP_INVALID_PARAMETERS;
        smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &reason);
        return;
    }

    /* peer (master) started pairing sending Pairing Request */
    /* or being master device always use received i/r key as keys to distribute */
    p_cb->local_i_key = p_cb->peer_i_key;
    p_cb->local_r_key = p_cb->peer_r_key;

    if (p_cb->role == HCI_ROLE_SLAVE) {
        p_dev_rec->new_encryption_key_is_p256 = FALSE;
        /* shortcut to skip Security Grant step */
        p_cb->cb_evt = SMP_BR_KEYS_REQ_EVT;
    } else { /* Master receives pairing response */
        SMP_TRACE_DEBUG("%s master rcvs valid PAIRING RESPONSE."
                        " Supposed to move to key distribution phase. ", __func__);
    }

    /* auth_req received via BR/EDR SM channel is set to 0,
       but everything derived/exchanged has to be saved */
    p_cb->peer_auth_req |= SMP_AUTH_BOND;
    p_cb->loc_auth_req |= SMP_AUTH_BOND;
}

/*******************************************************************************
** Function     smp_br_process_security_grant
** Description  process security grant in case of pairing over BR/EDR transport.
*******************************************************************************/
void smp_br_process_security_grant(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 res = *(UINT8 *)p_data;
    SMP_TRACE_DEBUG("%s", __func__);
    if (res != SMP_SUCCESS) {
        smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, p_data);
    } else { /*otherwise, start pairing */
        /* send IO request callback */
        p_cb->cb_evt = SMP_BR_KEYS_REQ_EVT;
    }
}

/*******************************************************************************
** Function     smp_br_check_authorization_request
** Description  sets the SMP kes to be derived/distribute over BR/EDR transport
**              before starting the distribution/derivation
*******************************************************************************/
void smp_br_check_authorization_request(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 reason = SMP_SUCCESS;

    SMP_TRACE_DEBUG("%s rcvs i_keys=0x%x r_keys=0x%x "
                    "(i-initiator r-responder)", __FUNCTION__, p_cb->local_i_key,
                    p_cb->local_r_key);

    /* In LE SC mode LK field is ignored when BR/EDR transport is used */
    p_cb->local_i_key &= ~SMP_SEC_KEY_TYPE_LK;
    p_cb->local_r_key &= ~SMP_SEC_KEY_TYPE_LK;

    /* In LE SC mode only IRK, IAI, CSRK are exchanged with the peer.
    ** Set local_r_key on master to expect only these keys. */
    if (p_cb->role == HCI_ROLE_MASTER) {
        p_cb->local_r_key &= (SMP_SEC_KEY_TYPE_ID | SMP_SEC_KEY_TYPE_CSRK);
    }

    SMP_TRACE_DEBUG("%s rcvs upgrades: i_keys=0x%x r_keys=0x%x "
                    "(i-initiator r-responder)", __FUNCTION__, p_cb->local_i_key,
                    p_cb->local_r_key);

    if (/*((p_cb->peer_auth_req & SMP_AUTH_BOND) ||
            (p_cb->loc_auth_req & SMP_AUTH_BOND)) &&*/
        (p_cb->local_i_key || p_cb->local_r_key)) {
        smp_br_state_machine_event(p_cb, SMP_BR_BOND_REQ_EVT, NULL);

        /* if no peer key is expected, start master key distribution */
        if (p_cb->role == HCI_ROLE_MASTER && p_cb->local_r_key == 0) {
            smp_key_distribution_by_transport(p_cb, NULL);
        }
    } else {
        smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &reason);
    }
}

/*******************************************************************************
** Function     smp_br_select_next_key
** Description  selects the next key to derive/send when BR/EDR transport is
**              used.
*******************************************************************************/
void smp_br_select_next_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   reason = SMP_SUCCESS;
    SMP_TRACE_DEBUG("%s role=%d (0-master) r_keys=0x%x i_keys=0x%x",
                    __func__, p_cb->role, p_cb->local_r_key, p_cb->local_i_key);

    if (p_cb->role == HCI_ROLE_SLAVE ||
            (!p_cb->local_r_key && p_cb->role == HCI_ROLE_MASTER)) {
        smp_key_pick_key(p_cb, p_data);
    }

    if (!p_cb->local_i_key && !p_cb->local_r_key) {
        /* state check to prevent re-entrance */
        if (smp_get_br_state() == SMP_BR_STATE_BOND_PENDING) {
            if (p_cb->total_tx_unacked == 0) {
                smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &reason);
            } else {
                p_cb->wait_for_authorization_complete = TRUE;
            }
        }
    }
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
** Function     smp_proc_enc_info
** Description  process encryption information from peer device
*******************************************************************************/
void smp_proc_enc_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   *p = (UINT8 *)p_data;

    SMP_TRACE_DEBUG("%s\n", __func__);
    STREAM_TO_ARRAY(p_cb->ltk, p, BT_OCTET16_LEN);

    smp_key_distribution(p_cb, NULL);
}
#endif  ///BLE_INCLUDED == TRUE

/*******************************************************************************
** Function     smp_proc_master_id
** Description  process master ID from slave device
*******************************************************************************/
void smp_proc_master_id(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   *p = (UINT8 *)p_data;
    tBTM_LE_PENC_KEYS   le_key;

    SMP_TRACE_DEBUG("%s\np_cb->peer_auth_req = %d,p_cb->loc_auth_req= %d\n", __func__,
                    p_cb->peer_auth_req, p_cb->loc_auth_req);
    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_ENC, TRUE);

    STREAM_TO_UINT16(le_key.ediv, p);
    STREAM_TO_ARRAY(le_key.rand, p, BT_OCTET8_LEN );

    /* store the encryption keys from peer device */
    memcpy(le_key.ltk, p_cb->ltk, BT_OCTET16_LEN);
    le_key.sec_level = p_cb->sec_level;
    le_key.key_size  = p_cb->loc_enc_size;

#if (BLE_INCLUDED == TRUE)
    if ((p_cb->peer_auth_req & SMP_AUTH_BOND) && (p_cb->loc_auth_req & SMP_AUTH_BOND)) {
        btm_sec_save_le_key(p_cb->pairing_bda,
                            BTM_LE_KEY_PENC,
                            (tBTM_LE_KEY_VALUE *)&le_key, TRUE);
    }

    smp_key_distribution(p_cb, NULL);
#endif  ///BLE_INCLUDED == TRUE
}

/*******************************************************************************
** Function     smp_proc_enc_info
** Description  process identity information from peer device
*******************************************************************************/
void smp_proc_id_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   *p = (UINT8 *)p_data;

    SMP_TRACE_DEBUG("%s", __func__);
    STREAM_TO_ARRAY (p_cb->tk, p, BT_OCTET16_LEN);   /* reuse TK for IRK */
    smp_key_distribution_by_transport(p_cb, NULL);
}

/*******************************************************************************
** Function     smp_proc_id_addr
** Description  process identity address from peer device
*******************************************************************************/
void smp_proc_id_addr(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   *p = (UINT8 *)p_data;
    tBTM_LE_PID_KEYS    pid_key;

    SMP_TRACE_DEBUG("%s", __func__);
    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_ID, TRUE);

    STREAM_TO_UINT8(pid_key.addr_type, p);
    STREAM_TO_BDADDR(pid_key.static_addr, p);
    memcpy(pid_key.irk, p_cb->tk, BT_OCTET16_LEN);

    /* to use as BD_ADDR for lk derived from ltk */
    p_cb->id_addr_rcvd = TRUE;
    p_cb->id_addr_type = pid_key.addr_type;
    memcpy(p_cb->id_addr, pid_key.static_addr, BD_ADDR_LEN);

#if (BLE_INCLUDED == TRUE)
    /* store the ID key from peer device */
    if ((p_cb->peer_auth_req & SMP_AUTH_BOND) && (p_cb->loc_auth_req & SMP_AUTH_BOND)) {
        btm_sec_save_le_key(p_cb->pairing_bda, BTM_LE_KEY_PID,
                            (tBTM_LE_KEY_VALUE *)&pid_key, TRUE);
    }
#endif  ///BLE_INCLUDED == TRUE

    smp_key_distribution_by_transport(p_cb, NULL);
}

/*******************************************************************************
** Function     smp_proc_srk_info
** Description  process security information from peer device
*******************************************************************************/
void smp_proc_srk_info(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
#if (BLE_INCLUDED == TRUE)
    tBTM_LE_PCSRK_KEYS   le_key;

    SMP_TRACE_DEBUG("%s", __func__);
    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_CSRK, TRUE);

    /* save CSRK to security record */
    le_key.sec_level = p_cb->sec_level;
    memcpy (le_key.csrk, p_data, BT_OCTET16_LEN);   /* get peer CSRK */
    le_key.counter = 0; /* initialize the peer counter */

    if ((p_cb->peer_auth_req & SMP_AUTH_BOND) && (p_cb->loc_auth_req & SMP_AUTH_BOND)) {
        btm_sec_save_le_key(p_cb->pairing_bda,
                            BTM_LE_KEY_PCSRK,
                            (tBTM_LE_KEY_VALUE *)&le_key, TRUE);
    }

#endif  ///BLE_INCLUDED == TRUE
    smp_key_distribution_by_transport(p_cb, NULL);
}

/*******************************************************************************
** Function     smp_proc_compare
** Description  process compare value
*******************************************************************************/
void smp_proc_compare(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   reason;

    SMP_TRACE_DEBUG("%s\n", __func__);
    if (!memcmp(p_cb->rconfirm, p_data->key.p_data, BT_OCTET16_LEN)) {
        /* compare the max encryption key size, and save the smaller one for the link */
        if ( p_cb->peer_enc_size < p_cb->loc_enc_size) {
            p_cb->loc_enc_size = p_cb->peer_enc_size;
        }

        if (p_cb->role == HCI_ROLE_SLAVE) {
            smp_sm_event(p_cb, SMP_RAND_EVT, NULL);
        } else {
            /* master device always use received i/r key as keys to distribute */
            p_cb->local_i_key = p_cb->peer_i_key;
            p_cb->local_r_key = p_cb->peer_r_key;

            smp_sm_event(p_cb, SMP_ENC_REQ_EVT, NULL);
        }

    } else {
        reason = p_cb->failure = SMP_CONFIRM_VALUE_ERR;
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
    }
}

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
** Function     smp_proc_sl_key
** Description  process key ready events.
*******************************************************************************/
void smp_proc_sl_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 key_type = p_data->key.key_type;

    SMP_TRACE_DEBUG("%s\n", __func__);
    if (key_type == SMP_KEY_TYPE_TK) {
        smp_generate_srand_mrand_confirm(p_cb, NULL);
    } else if (key_type == SMP_KEY_TYPE_CFM) {
        smp_set_state(SMP_STATE_WAIT_CONFIRM);

        if (p_cb->flags & SMP_PAIR_FLAGS_CMD_CONFIRM) {
            smp_sm_event(p_cb, SMP_CONFIRM_EVT, NULL);
        }
    }
}

/*******************************************************************************
** Function     smp_start_enc
** Description  start encryption
*******************************************************************************/
void smp_start_enc(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    tBTM_STATUS cmd;
    UINT8 reason = SMP_ENC_FAIL;

    SMP_TRACE_DEBUG("%s\n", __func__);
    if (p_data != NULL) {
        cmd = btm_ble_start_encrypt(p_cb->pairing_bda, TRUE, p_data->key.p_data);
    } else {
        cmd = btm_ble_start_encrypt(p_cb->pairing_bda, FALSE, NULL);
    }

    if (cmd != BTM_CMD_STARTED && cmd != BTM_BUSY) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
    }
}
#endif  ///BLE_INCLUDED == TRUE

/*******************************************************************************
** Function     smp_proc_discard
** Description   processing for discard security request
*******************************************************************************/
void smp_proc_discard(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    if (!(p_cb->flags & SMP_PAIR_FLAGS_WE_STARTED_DD)) {
        smp_reset_control_value(p_cb);
    }
}

/*******************************************************************************
** Function     smp_enc_cmpl
** Description   encryption success
*******************************************************************************/
void smp_enc_cmpl(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 enc_enable = *(UINT8 *)p_data;
    UINT8 reason = enc_enable ? SMP_SUCCESS : SMP_ENC_FAIL;

    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
}

/*******************************************************************************
** Function     smp_check_auth_req
** Description  check authentication request
*******************************************************************************/
void smp_check_auth_req(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 enc_enable = *(UINT8 *)p_data;
    UINT8 reason = enc_enable ? SMP_SUCCESS : SMP_ENC_FAIL;

    SMP_TRACE_DEBUG("%s rcvs enc_enable=%d i_keys=0x%x r_keys=0x%x "
                    "(i-initiator r-responder)\n",
                    __func__, enc_enable, p_cb->local_i_key, p_cb->local_r_key);
    if (enc_enable == 1) {
        if (p_cb->le_secure_connections_mode_is_used) {
            /* In LE SC mode LTK is used instead of STK and has to be always saved */
            p_cb->local_i_key |= SMP_SEC_KEY_TYPE_ENC;
            p_cb->local_r_key |= SMP_SEC_KEY_TYPE_ENC;

            /* In LE SC mode LK is derived from LTK only if both sides request it */
            if (!(p_cb->local_i_key & SMP_SEC_KEY_TYPE_LK) ||
                    !(p_cb->local_r_key & SMP_SEC_KEY_TYPE_LK)) {
                p_cb->local_i_key &= ~SMP_SEC_KEY_TYPE_LK;
                p_cb->local_r_key &= ~SMP_SEC_KEY_TYPE_LK;
            }

            /* In LE SC mode only IRK, IAI, CSRK are exchanged with the peer.
            ** Set local_r_key on master to expect only these keys.
            */
            if (p_cb->role == HCI_ROLE_MASTER) {
                p_cb->local_r_key &= (SMP_SEC_KEY_TYPE_ID | SMP_SEC_KEY_TYPE_CSRK);
            }
        } else {
            /* in legacy mode derivation of BR/EDR LK is not supported */
            p_cb->local_i_key &= ~SMP_SEC_KEY_TYPE_LK;
            p_cb->local_r_key &= ~SMP_SEC_KEY_TYPE_LK;
        }
        SMP_TRACE_DEBUG("%s rcvs upgrades: i_keys=0x%x r_keys=0x%x "
                        "(i-initiator r-responder)\n",
                        __func__, p_cb->local_i_key, p_cb->local_r_key);

        if (/*((p_cb->peer_auth_req & SMP_AUTH_BOND) ||
             (p_cb->loc_auth_req & SMP_AUTH_BOND)) &&*/
            (p_cb->local_i_key || p_cb->local_r_key)) {
            smp_sm_event(p_cb, SMP_BOND_REQ_EVT, NULL);
        } else {
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        }
    } else if (enc_enable == 0) {
        /* if failed for encryption after pairing, send callback */
        if (p_cb->flags & SMP_PAIR_FLAG_ENC_AFTER_PAIR) {
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        }
        /* if enc failed for old security information */
        /* if master device, clean up and abck to idle; slave device do nothing */
        else if (p_cb->role == HCI_ROLE_MASTER) {
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        }
    }
}

/*******************************************************************************
** Function     smp_key_pick_key
** Description  Pick a key distribution function based on the key mask.
*******************************************************************************/
void smp_key_pick_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   key_to_dist = (p_cb->role == HCI_ROLE_SLAVE) ? p_cb->local_r_key : p_cb->local_i_key;
    UINT8   i = 0;

    SMP_TRACE_DEBUG("%s key_to_dist=0x%x\n", __func__, key_to_dist);
    while (i < SMP_KEY_DIST_TYPE_MAX) {
        SMP_TRACE_DEBUG("key to send = %02x, i = %d\n",  key_to_dist, i);

        if (key_to_dist & (1 << i) && smp_distribute_act[i] != NULL) {
            SMP_TRACE_DEBUG("smp_distribute_act[%d]\n", i);
            (* smp_distribute_act[i])(p_cb, p_data);
            break;
        }
        i ++;
    }
}

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
** Function     smp_key_distribution
** Description  start key distribution if required.
*******************************************************************************/
void smp_key_distribution(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   reason = SMP_SUCCESS;
    SMP_TRACE_DEBUG("\n%s role=%d (0-master) r_keys=0x%x i_keys=0x%x\n",
                    __func__, p_cb->role, p_cb->local_r_key, p_cb->local_i_key);

    if (p_cb->role == HCI_ROLE_SLAVE ||
            (!p_cb->local_r_key && p_cb->role == HCI_ROLE_MASTER)) {
        smp_key_pick_key(p_cb, p_data);
    }

    if (!p_cb->local_i_key && !p_cb->local_r_key) {
        /* state check to prevent re-entrant */
        if (smp_get_state() == SMP_STATE_BOND_PENDING) {
            if (p_cb->derive_lk) {
                smp_derive_link_key_from_long_term_key(p_cb, NULL);
                p_cb->derive_lk = FALSE;
            }

            if (p_cb->total_tx_unacked == 0) {
                smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
            } else {
                p_cb->wait_for_authorization_complete = TRUE;
            }
        }
    }
}

/*******************************************************************************
** Function         smp_decide_association_model
** Description      This function is called to select assoc model to be used for
**                  STK generation and to start STK generation process.
**
*******************************************************************************/
void smp_decide_association_model(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   failure = SMP_UNKNOWN_IO_CAP;
    UINT8 int_evt = 0;
    tSMP_KEY key;
    tSMP_INT_DATA   *p = NULL;

    SMP_TRACE_DEBUG("%s Association Model = %d\n", __func__, p_cb->selected_association_model);

    switch (p_cb->selected_association_model) {
    case SMP_MODEL_ENCRYPTION_ONLY:  /* TK = 0, go calculate Confirm */
        p_cb->sec_level = SMP_SEC_UNAUTHENTICATE;
        SMP_TRACE_EVENT ("p_cb->sec_level =%d (SMP_SEC_UNAUTHENTICATE) \n", p_cb->sec_level );

        key.key_type = SMP_KEY_TYPE_TK;
        key.p_data = p_cb->tk;
        p = (tSMP_INT_DATA *)&key;

        memset(p_cb->tk, 0, BT_OCTET16_LEN);
        /* TK, ready  */
        int_evt = SMP_KEY_READY_EVT;
        break;

    case SMP_MODEL_PASSKEY:
        p_cb->sec_level = SMP_SEC_AUTHENTICATED;
        SMP_TRACE_EVENT ("p_cb->sec_level =%d (SMP_SEC_AUTHENTICATED) \n", p_cb->sec_level );

        p_cb->cb_evt = SMP_PASSKEY_REQ_EVT;
        int_evt = SMP_TK_REQ_EVT;
        break;

    case SMP_MODEL_OOB:
        SMP_TRACE_ERROR ("Association Model = SMP_MODEL_OOB\n");
        p_cb->sec_level = SMP_SEC_AUTHENTICATED;
        SMP_TRACE_EVENT ("p_cb->sec_level =%d (SMP_SEC_AUTHENTICATED) \n", p_cb->sec_level );

        p_cb->cb_evt = SMP_OOB_REQ_EVT;
        int_evt = SMP_TK_REQ_EVT;
        break;

    case SMP_MODEL_KEY_NOTIF:
        p_cb->sec_level = SMP_SEC_AUTHENTICATED;
        SMP_TRACE_DEBUG("Need to generate Passkey\n");

        /* generate passkey and notify application */
        smp_generate_passkey(p_cb, NULL);
        break;

    case SMP_MODEL_SEC_CONN_JUSTWORKS:
    case SMP_MODEL_SEC_CONN_NUM_COMP:
    case SMP_MODEL_SEC_CONN_PASSKEY_ENT:
    case SMP_MODEL_SEC_CONN_PASSKEY_DISP:
    case SMP_MODEL_SEC_CONN_OOB:
        int_evt = SMP_PUBL_KEY_EXCH_REQ_EVT;
        break;

    case SMP_MODEL_OUT_OF_RANGE:
        SMP_TRACE_ERROR("Association Model = SMP_MODEL_OUT_OF_RANGE (failed)\n");
        p = (tSMP_INT_DATA *)&failure;
        int_evt = SMP_AUTH_CMPL_EVT;
        break;

    default:
        SMP_TRACE_ERROR("Association Model = %d (SOMETHING IS WRONG WITH THE CODE)\n",
                        p_cb->selected_association_model);
        p = (tSMP_INT_DATA *)&failure;
        int_evt = SMP_AUTH_CMPL_EVT;
    }

    SMP_TRACE_EVENT ("sec_level=%d \n", p_cb->sec_level );
    if (int_evt) {
        smp_sm_event(p_cb, int_evt, p);
    }
}

/*******************************************************************************
** Function     smp_process_io_response
** Description  process IO response for a slave device.
*******************************************************************************/
void smp_process_io_response(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    uint8_t reason = SMP_PAIR_AUTH_FAIL;
    SMP_TRACE_DEBUG("%s\n", __func__);
    if (p_cb->flags & SMP_PAIR_FLAGS_WE_STARTED_DD) {
        /* pairing started by local (slave) Security Request */
        smp_set_state(SMP_STATE_SEC_REQ_PENDING);
        smp_send_cmd(SMP_OPCODE_SEC_REQ, p_cb);
    } else { /* plan to send pairing respond */
        /* pairing started by peer (master) Pairing Request */
        p_cb->selected_association_model = smp_select_association_model(p_cb);

        if (p_cb->secure_connections_only_mode_required &&
                (!(p_cb->le_secure_connections_mode_is_used) ||
                 (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_JUSTWORKS))) {
            SMP_TRACE_ERROR ("Slave requires secure connection only mode \
                              but it can't be provided -> Slave fails pairing\n");
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
            return;
        }
        uint16_t auth = smp_get_auth_mode(p_cb->selected_association_model);
        if(p_cb->peer_auth_req & p_cb->loc_auth_req & SMP_AUTH_GEN_BOND) {
            auth |= SMP_AUTH_GEN_BOND;
        }
        p_cb->auth_mode = auth;
        if (p_cb->accept_specified_sec_auth) {
            if ((auth & p_cb->origin_loc_auth_req) != p_cb->origin_loc_auth_req ) {
                SMP_TRACE_ERROR("pairing failed - slave requires auth is 0x%x but peer auth is 0x%x local auth is 0x%x",
                                    p_cb->origin_loc_auth_req, p_cb->peer_auth_req, p_cb->loc_auth_req);
                if (BTM_IsAclConnectionUp(p_cb->pairing_bda, BT_TRANSPORT_LE)) {
                    btm_remove_acl (p_cb->pairing_bda, BT_TRANSPORT_LE);
                }
                reason = SMP_PAIR_AUTH_FAIL;
                smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
                return;
            }
        }

        if (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_OOB && p_cb->loc_oob_flag == SMP_OOB_PRESENT) {
            if (smp_request_oob_data(p_cb)) {
                return;
            }
        }
        smp_send_pair_rsp(p_cb, NULL);
    }
}
#endif  ///BLE_INCLUDED == TRUE

/*******************************************************************************
** Function     smp_br_process_slave_keys_response
** Description  process application keys response for a slave device
**              (BR/EDR transport).
*******************************************************************************/
void smp_br_process_slave_keys_response(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    smp_br_send_pair_response(p_cb, NULL);
}

/*******************************************************************************
** Function     smp_br_send_pair_response
** Description  actions related to sending pairing response over BR/EDR transport.
*******************************************************************************/
void smp_br_send_pair_response(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    p_cb->local_i_key &= p_cb->peer_i_key;
    p_cb->local_r_key &= p_cb->peer_r_key;

    smp_send_cmd (SMP_OPCODE_PAIRING_RSP, p_cb);
}

/*******************************************************************************
** Function         smp_pairing_cmpl
** Description      This function is called to send the pairing complete callback
**                  and remove the connection if needed.
*******************************************************************************/
void smp_pairing_cmpl(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    if (p_cb->total_tx_unacked == 0) {
        /* process the pairing complete */
        smp_proc_pairing_cmpl(p_cb);
    }
}

/*******************************************************************************
** Function         smp_pair_terminate
** Description      This function is called to send the pairing complete callback
**                  and remove the connection if needed.
*******************************************************************************/
void smp_pair_terminate(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    p_cb->status = SMP_CONN_TOUT;
    smp_proc_pairing_cmpl(p_cb);
}

/*******************************************************************************
** Function         smp_idle_terminate
** Description      This function calledin idle state to determine to send authentication
**                  complete or not.
*******************************************************************************/
void smp_idle_terminate(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    if (p_cb->flags & SMP_PAIR_FLAGS_WE_STARTED_DD) {
        SMP_TRACE_DEBUG("Pairing terminated at IDLE state.\n");
        p_cb->status = SMP_FAIL;
        smp_proc_pairing_cmpl(p_cb);
    }
}

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
** Function     smp_fast_conn_param
** Description  apply default connection parameter for pairing process
*******************************************************************************/
void smp_fast_conn_param(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    if(p_cb->role == BTM_ROLE_MASTER) {
#if (BT_MULTI_CONNECTION_ENBALE == FALSE)
        L2CA_EnableUpdateBleConnParams(p_cb->pairing_bda, FALSE);
#endif
    } else {
#if (SMP_SLAVE_CON_PARAMS_UPD_ENABLE == TRUE)
        tBTM_SEC_DEV_REC    *p_rec = btm_find_dev (p_cb->pairing_bda);
        if(p_rec && p_rec->ble.skip_update_conn_param) {
            //do nothing
            return;
        }
        /* Disable L2CAP connection parameter updates while bonding since
        some peripherals are not able to revert to fast connection parameters
        during the start of service discovery. Connection parameter updates
        get enabled again once service discovery completes. */
        #if (BT_MULTI_CONNECTION_ENBALE == FALSE)
        L2CA_EnableUpdateBleConnParams(p_cb->pairing_bda, FALSE);
        #endif
#endif
    }
}

/*******************************************************************************
** Function     smp_both_have_public_keys
** Description  The function is called when both local and peer public keys are
**              saved.
**              Actions:
**              - invokes DHKey computation;
**              - on slave side invokes sending local public key to the peer.
**              - invokes SC phase 1 process.
*******************************************************************************/
void smp_both_have_public_keys(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    /* invokes DHKey computation */
    smp_compute_dhkey(p_cb);

    /* on slave side invokes sending local public key to the peer */
    if (p_cb->role == HCI_ROLE_SLAVE) {
        smp_send_pair_public_key(p_cb, NULL);
    }

    smp_sm_event(p_cb, SMP_SC_DHKEY_CMPLT_EVT, NULL);
}

/*******************************************************************************
** Function     smp_start_secure_connection_phase1
** Description  The function starts Secure Connection phase1 i.e. invokes initialization of Secure Connection
**              phase 1 parameters and starts building/sending to the peer
**              messages appropriate for the role and association model.
*******************************************************************************/
void smp_start_secure_connection_phase1(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    if (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_JUSTWORKS) {
        p_cb->sec_level = SMP_SEC_UNAUTHENTICATE;
        SMP_TRACE_EVENT ("p_cb->sec_level =%d (SMP_SEC_UNAUTHENTICATE)\n ", p_cb->sec_level );
    } else {
        p_cb->sec_level = SMP_SEC_AUTHENTICATED;
        SMP_TRACE_EVENT ("p_cb->sec_level =%d (SMP_SEC_AUTHENTICATED)\n ", p_cb->sec_level );
    }

    switch (p_cb->selected_association_model) {
    case SMP_MODEL_SEC_CONN_JUSTWORKS:
    case SMP_MODEL_SEC_CONN_NUM_COMP:
        memset(p_cb->local_random, 0, BT_OCTET16_LEN);
        smp_start_nonce_generation(p_cb);
        break;
    case SMP_MODEL_SEC_CONN_PASSKEY_ENT:
        /* user has to provide passkey */
        p_cb->cb_evt = SMP_PASSKEY_REQ_EVT;
        smp_sm_event(p_cb, SMP_TK_REQ_EVT, NULL);
        break;
    case SMP_MODEL_SEC_CONN_PASSKEY_DISP:
        /* passkey has to be provided to user */
        SMP_TRACE_DEBUG("Need to generate SC Passkey\n");
        smp_generate_passkey(p_cb, NULL);
        break;
    case SMP_MODEL_SEC_CONN_OOB:
        /* use the available OOB information */
        smp_process_secure_connection_oob_data(p_cb, NULL);
        break;
    default:
        SMP_TRACE_ERROR ("Association Model = %d is not used in LE SC\n",
                         p_cb->selected_association_model);
        break;
    }
}

/*******************************************************************************
** Function     smp_process_local_nonce
** Description  The function processes new local nonce.
**
** Note         It is supposed to be called in SC phase1.
*******************************************************************************/
void smp_process_local_nonce(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    switch (p_cb->selected_association_model) {
    case SMP_MODEL_SEC_CONN_JUSTWORKS:
    case SMP_MODEL_SEC_CONN_NUM_COMP:
        if (p_cb->role == HCI_ROLE_SLAVE) {
            /* slave calculates and sends local commitment */
            smp_calculate_local_commitment(p_cb);
            smp_send_commitment(p_cb, NULL);
            /* Ensure the connection is still active */
            if (smp_get_state() == SMP_STATE_IDLE) return;
            /* slave has to wait for peer nonce */
            smp_set_state(SMP_STATE_WAIT_NONCE);
        } else { /* i.e. master */
            if (p_cb->flags & SMP_PAIR_FLAG_HAVE_PEER_COMM) {
                /* slave commitment is already received, send local nonce, wait for remote nonce*/
                SMP_TRACE_DEBUG("master in assoc mode = %d \
                    already rcvd slave commitment - race condition\n",
                                p_cb->selected_association_model);
                p_cb->flags &= ~SMP_PAIR_FLAG_HAVE_PEER_COMM;
                smp_send_rand(p_cb, NULL);
                /* Ensure the connection is still active */
                if (smp_get_state() == SMP_STATE_IDLE) return;
                smp_set_state(SMP_STATE_WAIT_NONCE);
            }
        }
        break;
    case SMP_MODEL_SEC_CONN_PASSKEY_ENT:
    case SMP_MODEL_SEC_CONN_PASSKEY_DISP:
        smp_calculate_local_commitment(p_cb);

        if (p_cb->role == HCI_ROLE_MASTER) {
            smp_send_commitment(p_cb, NULL);
        } else { /* slave */
            if (p_cb->flags & SMP_PAIR_FLAG_HAVE_PEER_COMM) {
                /* master commitment is already received */
                smp_send_commitment(p_cb, NULL);
                /* Ensure the connection is still active */
                if (smp_get_state() == SMP_STATE_IDLE) return;
                smp_set_state(SMP_STATE_WAIT_NONCE);
            }
        }
        break;
    case SMP_MODEL_SEC_CONN_OOB:
        if (p_cb->role == HCI_ROLE_MASTER) {
            smp_send_rand(p_cb, NULL);
            /* Ensure the connection is still active */
            if (smp_get_state() == SMP_STATE_IDLE) return;
        }

        smp_set_state(SMP_STATE_WAIT_NONCE);
        break;
    default:
        SMP_TRACE_ERROR ("Association Model = %d is not used in LE SC\n",
                         p_cb->selected_association_model);
        break;
    }
}

/*******************************************************************************
** Function     smp_process_peer_nonce
** Description  The function processes newly received and saved in CB peer nonce.
**              The actions depend on the selected association model and the role.
**
** Note         It is supposed to be called in SC phase1.
*******************************************************************************/
void smp_process_peer_nonce(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   reason;

    SMP_TRACE_DEBUG("%s start \n", __func__);

    switch (p_cb->selected_association_model) {
    case SMP_MODEL_SEC_CONN_JUSTWORKS:
    case SMP_MODEL_SEC_CONN_NUM_COMP:
        /* in these models only master receives commitment */
        if (p_cb->role == HCI_ROLE_MASTER) {
            if (!smp_check_commitment(p_cb)) {
                reason = p_cb->failure = SMP_CONFIRM_VALUE_ERR;
                smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
                break;
            }
        } else {
            /* slave sends local nonce */
            smp_send_rand(p_cb, NULL);
        }

        if (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_JUSTWORKS) {
            /* go directly to phase 2 */
            smp_sm_event(p_cb, SMP_SC_PHASE1_CMPLT_EVT, NULL);
        } else { /* numeric comparison */
            smp_set_state(SMP_STATE_WAIT_NONCE);
            smp_sm_event(p_cb, SMP_SC_CALC_NC_EVT, NULL);
        }
        break;
    case SMP_MODEL_SEC_CONN_PASSKEY_ENT:
    case SMP_MODEL_SEC_CONN_PASSKEY_DISP:
        if (!smp_check_commitment(p_cb)) {
            reason = p_cb->failure = SMP_CONFIRM_VALUE_ERR;
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
            break;
        }

        if (p_cb->role == HCI_ROLE_SLAVE) {
            smp_send_rand(p_cb, NULL);
        }

        if (++p_cb->round < 20) {
            smp_set_state(SMP_STATE_SEC_CONN_PHS1_START);
            p_cb->flags &= ~SMP_PAIR_FLAG_HAVE_PEER_COMM;
            smp_start_nonce_generation(p_cb);
            break;
        }

        smp_sm_event(p_cb, SMP_SC_PHASE1_CMPLT_EVT, NULL);
        break;
    case SMP_MODEL_SEC_CONN_OOB:
        if (p_cb->role == HCI_ROLE_SLAVE) {
            smp_send_rand(p_cb, NULL);
        }

        smp_sm_event(p_cb, SMP_SC_PHASE1_CMPLT_EVT, NULL);
        break;
    default:
        SMP_TRACE_ERROR ("Association Model = %d is not used in LE SC\n",
                         p_cb->selected_association_model);
        break;
    }

    SMP_TRACE_DEBUG("%s end\n ", __FUNCTION__);
}
#endif  ///BLE_INCLUDED == TRUE

/*******************************************************************************
** Function     smp_match_dhkey_checks
** Description  checks if the calculated peer DHKey Check value is the same as
**              received from the peer DHKey check value.
*******************************************************************************/
void smp_match_dhkey_checks(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 reason = SMP_DHKEY_CHK_FAIL;

    SMP_TRACE_DEBUG("%s\n", __func__);

    if (memcmp(p_data->key.p_data, p_cb->remote_dhkey_check, BT_OCTET16_LEN)) {
        SMP_TRACE_WARNING ("dhkey checks do no match\n");
        p_cb->failure = reason;
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    SMP_TRACE_EVENT ("dhkey checks match\n");

    /* compare the max encryption key size, and save the smaller one for the link */
    if (p_cb->peer_enc_size < p_cb->loc_enc_size) {
        p_cb->loc_enc_size = p_cb->peer_enc_size;
    }

    if (p_cb->role == HCI_ROLE_SLAVE) {
        smp_sm_event(p_cb, SMP_PAIR_DHKEY_CHCK_EVT, NULL);
    } else {
        /* master device always use received i/r key as keys to distribute */
        p_cb->local_i_key = p_cb->peer_i_key;
        p_cb->local_r_key = p_cb->peer_r_key;
        smp_sm_event(p_cb, SMP_ENC_REQ_EVT, NULL);
    }
}

/*******************************************************************************
** Function     smp_move_to_secure_connections_phase2
** Description  Signal State Machine to start SC phase 2 initialization (to
**              compute local DHKey Check value).
**
** Note         SM is supposed to be in the state SMP_STATE_SEC_CONN_PHS2_START.
*******************************************************************************/
void smp_move_to_secure_connections_phase2(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_sm_event(p_cb, SMP_SC_PHASE1_CMPLT_EVT, NULL);
}

/*******************************************************************************
** Function     smp_phase_2_dhkey_checks_are_present
** Description  generates event if dhkey check from the peer is already received.
**
** Note         It is supposed to be used on slave to prevent race condition.
**              It is supposed to be called after slave dhkey check is calculated.
*******************************************************************************/
void smp_phase_2_dhkey_checks_are_present(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    if (p_cb->flags & SMP_PAIR_FLAG_HAVE_PEER_DHK_CHK) {
        smp_sm_event(p_cb, SMP_SC_2_DHCK_CHKS_PRES_EVT, NULL);
    }
}

/*******************************************************************************
** Function     smp_wait_for_both_public_keys
** Description  generates SMP_BOTH_PUBL_KEYS_RCVD_EVT event when both local and master
**              public keys are available.
**
** Note         on the slave it is used to prevent race condition.
**
*******************************************************************************/
void smp_wait_for_both_public_keys(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    if ((p_cb->flags & SMP_PAIR_FLAG_HAVE_PEER_PUBL_KEY) &&
            (p_cb->flags & SMP_PAIR_FLAG_HAVE_LOCAL_PUBL_KEY)) {
        if ((p_cb->role == HCI_ROLE_SLAVE) &&
                ((p_cb->req_oob_type == SMP_OOB_LOCAL) || (p_cb->req_oob_type == SMP_OOB_BOTH))) {
            smp_set_state(SMP_STATE_PUBLIC_KEY_EXCH);
        }
        smp_sm_event(p_cb, SMP_BOTH_PUBL_KEYS_RCVD_EVT, NULL);
    }
}

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
** Function     smp_start_passkey_verification
** Description  Starts SC passkey entry verification.
*******************************************************************************/
void smp_start_passkey_verification(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8 *p = NULL;

    SMP_TRACE_DEBUG("%s\n", __func__);
    p = p_cb->local_random;
    UINT32_TO_STREAM(p, p_data->passkey);

    p = p_cb->peer_random;
    UINT32_TO_STREAM(p, p_data->passkey);

    p_cb->round = 0;
    smp_start_nonce_generation(p_cb);
}

/*******************************************************************************
** Function     smp_process_secure_connection_oob_data
** Description  Processes local/peer SC OOB data received from somewhere.
*******************************************************************************/
void smp_process_secure_connection_oob_data(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    tSMP_SC_OOB_DATA *p_sc_oob_data = &p_cb->sc_oob_data;
    if (p_sc_oob_data->loc_oob_data.present) {
        memcpy(p_cb->local_random, p_sc_oob_data->loc_oob_data.randomizer,
               sizeof(p_cb->local_random));
    } else {
        SMP_TRACE_EVENT ("local OOB randomizer is absent\n");
        memset(p_cb->local_random, 0, sizeof (p_cb->local_random));
    }

    if (!p_sc_oob_data->peer_oob_data.present) {
        SMP_TRACE_EVENT ("peer OOB data is absent\n");
        memset(p_cb->peer_random, 0, sizeof (p_cb->peer_random));
    } else {
        memcpy(p_cb->peer_random, p_sc_oob_data->peer_oob_data.randomizer,
               sizeof(p_cb->peer_random));
        memcpy(p_cb->remote_commitment, p_sc_oob_data->peer_oob_data.commitment,
               sizeof(p_cb->remote_commitment));

        UINT8 reason = SMP_CONFIRM_VALUE_ERR;
        /* check commitment */
        if (!smp_check_commitment(p_cb)) {
            p_cb->failure = reason;
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
            return;
        }

        if (p_cb->peer_oob_flag != SMP_OOB_PRESENT) {
            /* the peer doesn't have local randomiser */
            SMP_TRACE_EVENT ("peer didn't receive local OOB data, set local randomizer to 0\n");
            memset(p_cb->local_random, 0, sizeof (p_cb->local_random));
        }
    }

    print128(p_cb->local_random, (const UINT8 *)"local OOB randomizer");
    print128(p_cb->peer_random, (const UINT8 *)"peer OOB randomizer");
    smp_start_nonce_generation(p_cb);
}

/*******************************************************************************
** Function     smp_set_local_oob_keys
** Description  Saves calculated private/public keys in sc_oob_data.loc_oob_data,
**              starts nonce generation
**              (to be saved in sc_oob_data.loc_oob_data.randomizer).
*******************************************************************************/
void smp_set_local_oob_keys(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    memcpy(p_cb->sc_oob_data.loc_oob_data.private_key_used, p_cb->private_key,
           BT_OCTET32_LEN);
    p_cb->sc_oob_data.loc_oob_data.publ_key_used = p_cb->loc_publ_key;
    smp_start_nonce_generation(p_cb);
}

/*******************************************************************************
** Function     smp_set_local_oob_random_commitment
** Description  Saves calculated randomizer and commitment in sc_oob_data.loc_oob_data,
**              passes sc_oob_data.loc_oob_data up for safekeeping.
*******************************************************************************/
void smp_set_local_oob_random_commitment(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    memcpy(p_cb->sc_oob_data.loc_oob_data.randomizer, p_cb->rand,
           BT_OCTET16_LEN);

    smp_calculate_f4(p_cb->sc_oob_data.loc_oob_data.publ_key_used.x,
                     p_cb->sc_oob_data.loc_oob_data.publ_key_used.x,
                     p_cb->sc_oob_data.loc_oob_data.randomizer, 0,
                     p_cb->sc_oob_data.loc_oob_data.commitment);

    p_cb->sc_oob_data.loc_oob_data.present = true;
#if SMP_DEBUG == TRUE
    UINT8   *p_print = NULL;
    SMP_TRACE_DEBUG("local SC OOB data set:\n");
    p_print = (UINT8 *) &p_cb->sc_oob_data.loc_oob_data.addr_sent_to;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"addr_sent_to",
                                         sizeof(tBLE_BD_ADDR));
    p_print = (UINT8 *) &p_cb->sc_oob_data.loc_oob_data.private_key_used;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"private_key_used",
                                         BT_OCTET32_LEN);
    p_print = (UINT8 *) &p_cb->sc_oob_data.loc_oob_data.publ_key_used.x;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"publ_key_used.x",
                                         BT_OCTET32_LEN);
    p_print = (UINT8 *) &p_cb->sc_oob_data.loc_oob_data.publ_key_used.y;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"publ_key_used.y",
                                         BT_OCTET32_LEN);
    p_print = (UINT8 *) &p_cb->sc_oob_data.loc_oob_data.randomizer;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"randomizer",
                                         BT_OCTET16_LEN);
    p_print = (UINT8 *) &p_cb->sc_oob_data.loc_oob_data.commitment;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *) "commitment",
                                         BT_OCTET16_LEN);
    SMP_TRACE_DEBUG("");
#endif

    /* pass created OOB data up */
    p_cb->cb_evt = SMP_SC_LOC_OOB_DATA_UP_EVT;
    smp_send_app_cback(p_cb, NULL);

    // Store the data for later use when we are paired with
    smp_save_local_oob_data(p_cb);

    smp_cb_cleanup(p_cb);
}

/*******************************************************************************
**
** Function         smp_link_encrypted
**
** Description      This function is called when link is encrypted and notified to
**                  slave device. Proceed to to send LTK, DIV and ER to master if
**                  bonding the devices.
**
**
** Returns          void
**
*******************************************************************************/
void smp_link_encrypted(BD_ADDR bda, UINT8 encr_enable)
{
    tSMP_CB *p_cb = &smp_cb;
    tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_dev (bda);
    SMP_TRACE_DEBUG("%s encr_enable=%d\n", __func__, encr_enable);

    if (memcmp(&smp_cb.pairing_bda[0], bda, BD_ADDR_LEN) == 0) {
        /* encryption completed with STK, remember the key size now, could be overwrite
        *  when key exchange happens                                        */
        if (p_cb->loc_enc_size != 0 && encr_enable) {
            /* update the link encryption key size if a SMP pairing just performed */
            btm_ble_update_sec_key_size(bda, p_cb->loc_enc_size);
        }

        smp_sm_event(&smp_cb, SMP_ENCRYPTED_EVT, &encr_enable);
    } else if (p_dev_rec && !p_dev_rec->role_master && !p_dev_rec->enc_init_by_we ){
        /*
        if enc_init_by_we is false, it means that client initiates encryption before slave calls esp_ble_set_encryption()
        we need initiate pairing_bda and p_cb->role then encryption, for example iPhones
        */
        memcpy(&smp_cb.pairing_bda[0], bda, BD_ADDR_LEN);
        p_cb->state = SMP_STATE_ENCRYPTION_PENDING;
        p_cb->role = HCI_ROLE_SLAVE;
        p_dev_rec->enc_init_by_we = FALSE;
        smp_sm_event(&smp_cb, SMP_ENCRYPTED_EVT, &encr_enable);
    } else if (p_dev_rec && p_dev_rec->role_master && p_dev_rec->enc_init_by_we){
        memcpy(&smp_cb.pairing_bda[0], bda, BD_ADDR_LEN);
        p_cb->state = SMP_STATE_ENCRYPTION_PENDING;
        p_cb->role = HCI_ROLE_MASTER;
        p_dev_rec->enc_init_by_we = FALSE;
        smp_sm_event(&smp_cb, SMP_ENCRYPTED_EVT, &encr_enable);
    }
}

/*******************************************************************************
**
** Function         smp_proc_ltk_request
**
** Description      This function is called when LTK request is received from
**                  controller.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN smp_proc_ltk_request(BD_ADDR bda)
{
    SMP_TRACE_DEBUG("%s state = %d\n",  __func__, smp_cb.state);
    BOOLEAN match = FALSE;

    if (!memcmp(bda, smp_cb.pairing_bda, BD_ADDR_LEN)) {
        match = TRUE;
    } else {
        BD_ADDR dummy_bda = {0};
        tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(bda);
        if (p_dev_rec != NULL &&
                0 == memcmp(p_dev_rec->ble.pseudo_addr, smp_cb.pairing_bda, BD_ADDR_LEN) &&
                0 != memcmp(p_dev_rec->ble.pseudo_addr, dummy_bda, BD_ADDR_LEN)) {
            match = TRUE;
        }
    }

    if (match && smp_cb.state == SMP_STATE_ENCRYPTION_PENDING) {
        smp_sm_event(&smp_cb, SMP_ENC_REQ_EVT, NULL);
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         smp_process_secure_connection_long_term_key
**
** Description      This function is called to process SC LTK.
**                  SC LTK is calculated and used instead of STK.
**                  Here SC LTK is saved in BLE DB.
**
** Returns          void
**
*******************************************************************************/
void smp_process_secure_connection_long_term_key(void)
{
    tSMP_CB     *p_cb = &smp_cb;

    SMP_TRACE_DEBUG("%s\n", __func__);
    smp_save_secure_connections_long_term_key(p_cb);

    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_ENC, FALSE);
    smp_key_distribution(p_cb, NULL);
}

/*******************************************************************************
**
** Function         smp_set_derive_link_key
**
** Description      This function is called to set flag that indicates that
**                  BR/EDR LK has to be derived from LTK after all keys are
**                  distributed.
**
** Returns          void
**
*******************************************************************************/
void smp_set_derive_link_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG ("%s\n", __func__);
    p_cb->derive_lk = TRUE;
    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_LK, FALSE);
    smp_key_distribution(p_cb, NULL);
}

/*******************************************************************************
**
** Function         smp_derive_link_key_from_long_term_key
**
** Description      This function is called to derive BR/EDR LK from LTK.
**
** Returns          void
**
*******************************************************************************/
void smp_derive_link_key_from_long_term_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    tSMP_STATUS status = SMP_PAIR_FAIL_UNKNOWN;

    SMP_TRACE_DEBUG("%s\n", __func__);
    if (!smp_calculate_link_key_from_long_term_key(p_cb)) {
        SMP_TRACE_ERROR("%s failed\n", __FUNCTION__);
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
        return;
    }
}
#endif  ///BLE_INCLUDED == TRUE

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         smp_br_process_link_key
**
** Description      This function is called to process BR/EDR LK:
**                  - to derive SMP LTK from BR/EDR LK;
*8                  - to save SMP LTK.
**
** Returns          void
**
*******************************************************************************/
void smp_br_process_link_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    tSMP_STATUS status = SMP_PAIR_FAIL_UNKNOWN;

    SMP_TRACE_DEBUG("%s\n", __func__);
    if (!smp_calculate_long_term_key_from_link_key(p_cb)) {
        SMP_TRACE_ERROR ("%s failed\n", __FUNCTION__);
        smp_sm_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &status);
        return;
    }

    SMP_TRACE_DEBUG("%s: LTK derivation from LK successfully completed\n", __FUNCTION__);
    smp_save_secure_connections_long_term_key(p_cb);
    smp_update_key_mask (p_cb, SMP_SEC_KEY_TYPE_ENC, FALSE);
    smp_br_select_next_key(p_cb, NULL);
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE

/*******************************************************************************
** Function     smp_key_distribution_by_transport
** Description  depending on the transport used at the moment calls either
**              smp_key_distribution(...) or smp_br_key_distribution(...).
*******************************************************************************/
void smp_key_distribution_by_transport(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);
    if (p_cb->smp_over_br) {
#if (CLASSIC_BT_INCLUDED == TRUE)
        smp_br_select_next_key(p_cb, NULL);
#endif  ///CLASSIC_BT_INCLUDED == TRUE
    } else {
#if (BLE_INCLUDED == TRUE)
        smp_key_distribution(p_cb, NULL);
#endif  ///BLE_INCLUDED == TRUE
    }
}

/*******************************************************************************
** Function         smp_br_pairing_complete
** Description      This function is called to send the pairing complete callback
**                  and remove the connection if needed.
*******************************************************************************/
void smp_br_pairing_complete(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG("%s\n", __func__);

    if (p_cb->total_tx_unacked == 0) {
        /* process the pairing complete */
        smp_proc_pairing_cmpl(p_cb);
    }
}

#endif
