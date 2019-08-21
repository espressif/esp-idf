/******************************************************************************
 *
 *  Copyright (C) 2008-2012 Broadcom Corporation
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
 *  This file contains the implementation of the SMP interface used by
 *  applications that can run over an SMP.
 *
 ******************************************************************************/
#include <string.h>

#include "common/bt_target.h"
//#include "bt_utils.h"
#if SMP_INCLUDED == TRUE
#include "smp_int.h"
#include "stack/smp_api.h"
#include "stack/l2cdefs.h"
#include "l2c_int.h"
#include "btm_int.h"
#include "stack/hcimsgs.h"

#include "stack/btu.h"
#include "p_256_ecc_pp.h"
#include "osi/allocator.h"

/*******************************************************************************
**
** Function         SMP_Init
**
** Description      This function initializes the SMP unit.
**
** Returns          void
**
*******************************************************************************/
void SMP_Init(void)
{
#if SMP_DYNAMIC_MEMORY
    smp_cb_ptr = (tSMP_CB *)osi_malloc(sizeof(tSMP_CB));
    curve_ptr = (elliptic_curve_t *)osi_malloc(sizeof(elliptic_curve_t));
    curve_p256_ptr = (elliptic_curve_t *)osi_malloc(sizeof(elliptic_curve_t));
#endif
    memset(&smp_cb, 0, sizeof(tSMP_CB));
    memset(&curve, 0, sizeof(elliptic_curve_t));
    memset(&curve_p256, 0, sizeof(elliptic_curve_t));

#if defined(SMP_INITIAL_TRACE_LEVEL)
    smp_cb.trace_level = SMP_INITIAL_TRACE_LEVEL;
#else
    smp_cb.trace_level = BT_TRACE_LEVEL_NONE;    /* No traces */
#endif
    SMP_TRACE_EVENT ("%s", __FUNCTION__);

    smp_l2cap_if_init();
    /* initialization of P-256 parameters */
    p_256_init_curve(KEY_LENGTH_DWORDS_P256);
}

void SMP_Free(void)
{
    memset(&smp_cb, 0, sizeof(tSMP_CB));
#if SMP_DYNAMIC_MEMORY
    FREE_AND_RESET(smp_cb_ptr);
    FREE_AND_RESET(curve_ptr);
    FREE_AND_RESET(curve_p256_ptr);
#endif /* #if SMP_DYNAMIC_MEMORY */
}


/*******************************************************************************
**
** Function         SMP_SetTraceLevel
**
** Description      This function sets the trace level for SMP.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
**                  Input Parameters:
**                      level:  The level to set the GATT tracing to:
**                      0xff-returns the current setting.
**                      0-turns off tracing.
**                      >= 1-Errors.
**                      >= 2-Warnings.
**                      >= 3-APIs.
**                      >= 4-Events.
**                      >= 5-Debug.
**
** Returns          The new or current trace level
**
*******************************************************************************/
extern UINT8 SMP_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF) {
        smp_cb.trace_level = new_level;
    }

    return (smp_cb.trace_level);
}


/*******************************************************************************
**
** Function         SMP_Register
**
** Description      This function register for the SMP services callback.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN SMP_Register (tSMP_CALLBACK *p_cback)
{
    SMP_TRACE_EVENT ("SMP_Register state=%d", smp_cb.state);

    if (smp_cb.p_callback != NULL) {
        SMP_TRACE_ERROR ("SMP_Register: duplicate registration, overwrite it");
    }
    smp_cb.p_callback = p_cback;

    return (TRUE);

}

/*******************************************************************************
**
** Function         SMP_Pair
**
** Description      This function call to perform a SMP pairing with peer device.
**                  Device support one SMP pairing at one time.
**
** Parameters       bd_addr - peer device bd address.
**
** Returns          None
**
*******************************************************************************/
tSMP_STATUS SMP_Pair (BD_ADDR bd_addr)
{
    tSMP_CB   *p_cb = &smp_cb;
    UINT8     status = SMP_PAIR_INTERNAL_ERR;

    SMP_TRACE_EVENT ("%s state=%d br_state=%d flag=0x%x \n",
                     __FUNCTION__, p_cb->state, p_cb->br_state, p_cb->flags);
    if (p_cb->state != SMP_STATE_IDLE || p_cb->flags & SMP_PAIR_FLAGS_WE_STARTED_DD ||
            p_cb->smp_over_br) {
        /* pending security on going, reject this one */
        return SMP_BUSY;
    } else {
        p_cb->flags = SMP_PAIR_FLAGS_WE_STARTED_DD;

        memcpy (p_cb->pairing_bda, bd_addr, BD_ADDR_LEN);

        if (!L2CA_ConnectFixedChnl (L2CAP_SMP_CID, bd_addr, BLE_ADDR_UNKNOWN_TYPE)) {
            SMP_TRACE_ERROR("%s: L2C connect fixed channel failed.\n", __FUNCTION__);
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
            return status;
        }

        return SMP_STARTED;
    }
}

/*******************************************************************************
**
** Function         SMP_BR_PairWith
**
** Description      This function is called to start a SMP pairing over BR/EDR.
**                  Device support one SMP pairing at one time.
**
** Parameters       bd_addr - peer device bd address.
**
** Returns          SMP_STARTED if pairing started, otherwise reason for failure.
**
*******************************************************************************/
#if (CLASSIC_BT_INCLUDED == TRUE)
tSMP_STATUS SMP_BR_PairWith (BD_ADDR bd_addr)
{
    tSMP_CB   *p_cb = &smp_cb;
    UINT8     status = SMP_PAIR_INTERNAL_ERR;

    SMP_TRACE_EVENT ("%s state=%d br_state=%d flag=0x%x ",
                     __func__, p_cb->state, p_cb->br_state, p_cb->flags);

    if (p_cb->state != SMP_STATE_IDLE ||
            p_cb->smp_over_br ||
            p_cb->flags & SMP_PAIR_FLAGS_WE_STARTED_DD) {
        /* pending security on going, reject this one */
        return SMP_BUSY;
    }

    p_cb->role = HCI_ROLE_MASTER;
    p_cb->flags = SMP_PAIR_FLAGS_WE_STARTED_DD;
    p_cb->smp_over_br = TRUE;

    memcpy (p_cb->pairing_bda, bd_addr, BD_ADDR_LEN);

    if (!L2CA_ConnectFixedChnl (L2CAP_SMP_BR_CID, bd_addr, BLE_ADDR_UNKNOWN_TYPE)) {
        SMP_TRACE_ERROR("%s: L2C connect fixed channel failed.", __FUNCTION__);
        smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &status);
        return status;
    }

    return SMP_STARTED;
}
#endif  ///CLASSIC_BT_INCLUDED == TRUE

/*******************************************************************************
**
** Function         SMP_PairCancel
**
** Description      This function call to cancel a SMP pairing with peer device.
**
** Parameters       bd_addr - peer device bd address.
**
** Returns          TRUE - Pairining is cancelled
**
*******************************************************************************/
BOOLEAN SMP_PairCancel (BD_ADDR bd_addr)
{
    tSMP_CB   *p_cb = &smp_cb;
    UINT8     err_code = SMP_PAIR_FAIL_UNKNOWN;
    BOOLEAN   status = FALSE;

    BTM_TRACE_EVENT ("SMP_CancelPair state=%d flag=0x%x ", p_cb->state, p_cb->flags);
    if ( (p_cb->state != SMP_STATE_IDLE)  &&
            (!memcmp (p_cb->pairing_bda, bd_addr, BD_ADDR_LEN)) ) {
        p_cb->is_pair_cancel = TRUE;
        SMP_TRACE_DEBUG("Cancel Pairing: set fail reason Unknown");
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &err_code);
        status = TRUE;
    }

    return status;
}
/*******************************************************************************
**
** Function         SMP_SecurityGrant
**
** Description      This function is called to grant security process.
**
** Parameters       bd_addr - peer device bd address.
**                  res     - result of the operation SMP_SUCCESS if success.
**                            Otherwise, SMP_REPEATED_ATTEMPTS is too many attempts.
**
** Returns          None
**
*******************************************************************************/
void SMP_SecurityGrant(BD_ADDR bd_addr, UINT8 res)
{
    SMP_TRACE_EVENT ("SMP_SecurityGrant ");

#if (CLASSIC_BT_INCLUDED == TRUE)
    if (smp_cb.smp_over_br) {
        if (smp_cb.br_state != SMP_BR_STATE_WAIT_APP_RSP ||
                smp_cb.cb_evt != SMP_SEC_REQUEST_EVT ||
                memcmp (smp_cb.pairing_bda, bd_addr, BD_ADDR_LEN)) {
            return;
        }

        /* clear the SMP_SEC_REQUEST_EVT event after get grant */
        /* avoid generating duplicate pair request */
        smp_cb.cb_evt = 0;
        smp_br_state_machine_event(&smp_cb, SMP_BR_API_SEC_GRANT_EVT, &res);
        return;
    }
#endif  ///CLASSIC_BT_INCLUDED == TRUE

    if (smp_cb.state != SMP_STATE_WAIT_APP_RSP ||
            smp_cb.cb_evt != SMP_SEC_REQUEST_EVT ||
            memcmp (smp_cb.pairing_bda, bd_addr, BD_ADDR_LEN)) {
        return;
    }
    /* clear the SMP_SEC_REQUEST_EVT event after get grant */
    /* avoid generate duplicate pair request */
    smp_cb.cb_evt = 0;
    smp_sm_event(&smp_cb, SMP_API_SEC_GRANT_EVT, &res);
}

/*******************************************************************************
**
** Function         SMP_PasskeyReply
**
** Description      This function is called after Security Manager submitted
**                  passkey request to the application.
**
** Parameters:      bd_addr      - Address of the device for which passkey was requested
**                  res          - result of the operation SMP_SUCCESS if success
**                  passkey - numeric value in the range of
**                  BTM_MIN_PASSKEY_VAL(0) - BTM_MAX_PASSKEY_VAL(999999(0xF423F)).
**
*******************************************************************************/
void SMP_PasskeyReply (BD_ADDR bd_addr, UINT8 res, UINT32 passkey)
{
    tSMP_CB *p_cb = & smp_cb;
    UINT8   failure = SMP_PASSKEY_ENTRY_FAIL;

    SMP_TRACE_EVENT ("SMP_PasskeyReply: Key: %d  Result:%d",
                     passkey, res);

    /* If timeout already expired or has been canceled, ignore the reply */
    if (p_cb->cb_evt != SMP_PASSKEY_REQ_EVT) {
        SMP_TRACE_WARNING ("SMP_PasskeyReply() - Wrong State: %d", p_cb->state);
        return;
    }

    if (memcmp (bd_addr, p_cb->pairing_bda, BD_ADDR_LEN) != 0) {
        SMP_TRACE_ERROR ("SMP_PasskeyReply() - Wrong BD Addr");
        return;
    }

    if (btm_find_dev (bd_addr) == NULL) {
        SMP_TRACE_ERROR ("SMP_PasskeyReply() - no dev CB");
        return;
    }

    if (passkey > BTM_MAX_PASSKEY_VAL || res != SMP_SUCCESS) {
        SMP_TRACE_WARNING ("SMP_PasskeyReply() - Wrong key len: %d or passkey entry fail", passkey);
        /* send pairing failure */
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &failure);

    } else if (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_PASSKEY_ENT) {
        smp_sm_event(&smp_cb, SMP_SC_KEY_READY_EVT, &passkey);
    } else {
        smp_convert_string_to_tk(p_cb->tk, passkey);
    }

    return;
}

/*******************************************************************************
**
** Function         SMP_SetStaticPasskey
**
** Description      This function is called to set static passkey
**
**
** Parameters:      add          - set static passkey when add is TRUE
**                                 clear static passkey when add is FALSE
**                  passkey      - static passkey
**
**
*******************************************************************************/
void SMP_SetStaticPasskey (BOOLEAN add, UINT32 passkey)
{
    SMP_TRACE_DEBUG("static passkey %6d", passkey);
    tSMP_CB *p_cb = & smp_cb;
    if(add) {
        p_cb->static_passkey = passkey;
        p_cb->use_static_passkey = true;
    } else {
        p_cb->static_passkey = 0;
        p_cb->use_static_passkey = false;
    }
}

/*******************************************************************************
**
** Function         SMP_ConfirmReply
**
** Description      This function is called after Security Manager submitted
**                  numeric comparison request to the application.
**
** Parameters:      bd_addr      - Address of the device with which numeric
**                                 comparison was requested
**                  res          - comparison result SMP_SUCCESS if success
**
*******************************************************************************/
void SMP_ConfirmReply (BD_ADDR bd_addr, UINT8 res)
{
    tSMP_CB *p_cb = & smp_cb;
    UINT8   failure = SMP_NUMERIC_COMPAR_FAIL;

    SMP_TRACE_EVENT ("%s: Result:%d", __FUNCTION__, res);

    /* If timeout already expired or has been canceled, ignore the reply */
    if (p_cb->cb_evt != SMP_NC_REQ_EVT) {
        SMP_TRACE_WARNING ("%s() - Wrong State: %d", __FUNCTION__, p_cb->state);
        return;
    }

    if (memcmp (bd_addr, p_cb->pairing_bda, BD_ADDR_LEN) != 0) {
        SMP_TRACE_ERROR ("%s() - Wrong BD Addr", __FUNCTION__);
        return;
    }

    if (btm_find_dev (bd_addr) == NULL) {
        SMP_TRACE_ERROR ("%s() - no dev CB", __FUNCTION__);
        return;
    }

    if (res != SMP_SUCCESS) {
        SMP_TRACE_WARNING ("%s() - Numeric Comparison fails", __FUNCTION__);
        /* send pairing failure */
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &failure);
    } else {
        smp_sm_event(p_cb, SMP_SC_NC_OK_EVT, NULL);
    }
}

/*******************************************************************************
**
** Function         SMP_OobDataReply
**
** Description      This function is called to provide the OOB data for
**                  SMP in response to SMP_OOB_REQ_EVT
**
** Parameters:      bd_addr     - Address of the peer device
**                  res         - result of the operation SMP_SUCCESS if success
**                  p_data      - simple pairing Randomizer  C.
**
*******************************************************************************/
void SMP_OobDataReply(BD_ADDR bd_addr, tSMP_STATUS res, UINT8 len, UINT8 *p_data)
{
    tSMP_CB *p_cb = & smp_cb;
    UINT8   failure = SMP_OOB_FAIL;
    tSMP_KEY        key;

    SMP_TRACE_EVENT ("%s State: %d  res:%d", __FUNCTION__, smp_cb.state, res);

    /* If timeout already expired or has been canceled, ignore the reply */
    if (p_cb->state != SMP_STATE_WAIT_APP_RSP || p_cb->cb_evt != SMP_OOB_REQ_EVT) {
        return;
    }

    if (res != SMP_SUCCESS || len == 0 || !p_data) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &failure);
    } else {
        if (len > BT_OCTET16_LEN) {
            len = BT_OCTET16_LEN;
        }

        memcpy(p_cb->tk, p_data, len);

        key.key_type    = SMP_KEY_TYPE_TK;
        key.p_data      = p_cb->tk;

        smp_sm_event(&smp_cb, SMP_KEY_READY_EVT, &key);
    }
}

/*******************************************************************************
**
** Function         SMP_SecureConnectionOobDataReply
**
** Description      This function is called to provide the SC OOB data for
**                  SMP in response to SMP_SC_OOB_REQ_EVT
**
** Parameters:      p_data      - pointer to the data
**
*******************************************************************************/
void SMP_SecureConnectionOobDataReply(UINT8 *p_data)
{
    tSMP_CB  *p_cb = &smp_cb;

    UINT8  failure = SMP_OOB_FAIL;
    tSMP_SC_OOB_DATA  *p_oob = (tSMP_SC_OOB_DATA *) p_data;
    if (!p_oob) {
        SMP_TRACE_ERROR("%s received no data", __FUNCTION__);
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &failure);
        return;
    }

    SMP_TRACE_EVENT ("%s req_oob_type: %d, loc_oob_data.present: %d, "
                     "peer_oob_data.present: %d",
                     __FUNCTION__, p_cb->req_oob_type, p_oob->loc_oob_data.present,
                     p_oob->peer_oob_data.present);

    if (p_cb->state != SMP_STATE_WAIT_APP_RSP || p_cb->cb_evt != SMP_SC_OOB_REQ_EVT) {
        return;
    }

    BOOLEAN  data_missing = FALSE;
    switch (p_cb->req_oob_type) {
    case SMP_OOB_PEER:
        if (!p_oob->peer_oob_data.present) {
            data_missing = TRUE;
        }
        break;
    case SMP_OOB_LOCAL:
        if (!p_oob->loc_oob_data.present) {
            data_missing = TRUE;
        }
        break;
    case SMP_OOB_BOTH:
        if (!p_oob->loc_oob_data.present || !p_oob->peer_oob_data.present) {
            data_missing = TRUE;
        }
        break;
    default:
        SMP_TRACE_EVENT ("Unexpected OOB data type requested. Fail OOB");
        data_missing = TRUE;
        break;
    }

    if (data_missing) {
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &failure);
        return;
    }

    p_cb->sc_oob_data = *p_oob;

    smp_sm_event(&smp_cb, SMP_SC_OOB_DATA_EVT, p_data);
}

/*******************************************************************************
**
** Function         SMP_Encrypt
**
** Description      This function is called to encrypt the data with the specified
**                  key
**
** Parameters:      key                 - Pointer to key key[0] conatins the MSB
**                  key_len             - key length
**                  plain_text          - Pointer to data to be encrypted
**                                        plain_text[0] conatins the MSB
**                  pt_len              - plain text length
**                  p_out                - output of the encrypted texts
**
**  Returns         Boolean - request is successful
*******************************************************************************/
BOOLEAN SMP_Encrypt (UINT8 *key, UINT8 key_len,
                     UINT8 *plain_text, UINT8 pt_len,
                     tSMP_ENC *p_out)

{
    BOOLEAN status = FALSE;
    status = smp_encrypt_data(key, key_len, plain_text, pt_len, p_out);
    return status;
}

/*******************************************************************************
**
** Function         SMP_KeypressNotification
**
** Description      This function is called to notify Security Manager about Keypress Notification.
**
** Parameters:     bd_addr      Address of the device to send keypress notification to
**                 value        Keypress notification parameter value
**
*******************************************************************************/
void SMP_KeypressNotification (BD_ADDR bd_addr, UINT8 value)
{
    tSMP_CB   *p_cb = &smp_cb;

    SMP_TRACE_EVENT ("%s: Value: %d", __FUNCTION__, value);

    if (memcmp (bd_addr, p_cb->pairing_bda, BD_ADDR_LEN) != 0) {
        SMP_TRACE_ERROR ("%s() - Wrong BD Addr", __FUNCTION__);
        return;
    }

    if (btm_find_dev (bd_addr) == NULL) {
        SMP_TRACE_ERROR ("%s() - no dev CB", __FUNCTION__);
        return;
    }

    /* Keypress Notification is used by a device with KeyboardOnly IO capabilities */
    /* during the passkey entry protocol */
    if (p_cb->local_io_capability != SMP_IO_CAP_IN) {
        SMP_TRACE_ERROR ("%s() - wrong local IO capabilities %d",
                         __FUNCTION__, p_cb->local_io_capability);
        return;
    }

    if (p_cb->selected_association_model != SMP_MODEL_SEC_CONN_PASSKEY_ENT) {
        SMP_TRACE_ERROR ("%s() - wrong protocol %d", __FUNCTION__,
                         p_cb->selected_association_model);
        return;
    }

    smp_sm_event(p_cb, SMP_KEYPRESS_NOTIFICATION_EVENT, &value);
}

/*******************************************************************************
**
** Function         SMP_CreateLocalSecureConnectionsOobData
**
** Description      This function is called to start creation of local SC OOB
**                  data set (tSMP_LOC_OOB_DATA).
**
** Parameters:      bd_addr      - Address of the device to send OOB data block to
**
**  Returns         Boolean - TRUE: creation of local SC OOB data set started.
*******************************************************************************/
BOOLEAN SMP_CreateLocalSecureConnectionsOobData (tBLE_BD_ADDR *addr_to_send_to)
{
    tSMP_CB *p_cb = &smp_cb;
#if (!CONFIG_BT_STACK_NO_LOG)
    UINT8   *bd_addr;
#endif

    if (addr_to_send_to == NULL) {
        SMP_TRACE_ERROR ("%s addr_to_send_to is not provided", __FUNCTION__);
        return FALSE;
    }

#if (!CONFIG_BT_STACK_NO_LOG)
    bd_addr = addr_to_send_to->bda;
#endif

    SMP_TRACE_EVENT ("%s addr type: %u,  BDA: %08x%04x,  state: %u, br_state: %u",
                     __FUNCTION__, addr_to_send_to->type,
                     (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                     (bd_addr[4] << 8) + bd_addr[5],
                     p_cb->state,
                     p_cb->br_state);

    if ((p_cb->state != SMP_STATE_IDLE) || (p_cb->smp_over_br)) {
        SMP_TRACE_WARNING ("%s creation of local OOB data set "\
                           "starts only in IDLE state", __FUNCTION__);
        return FALSE;
    }

    p_cb->sc_oob_data.loc_oob_data.addr_sent_to = *addr_to_send_to;
    smp_sm_event(p_cb, SMP_CR_LOC_SC_OOB_DATA_EVT, NULL);

    return TRUE;
}

#endif /* SMP_INCLUDED */
