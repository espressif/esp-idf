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
 *  This file contains security manager protocol utility functions
 *
 ******************************************************************************/
#include "bt_target.h"

#if SMP_INCLUDED == TRUE
#if SMP_DEBUG == TRUE
#include <stdio.h>
#endif
#include <string.h>
//#include "bt_utils.h"
#include "btm_ble_api.h"
#include "smp_int.h"
#include "btm_int.h"
#include "btm_ble_int.h"
#include "hcimsgs.h"
#include "aes.h"
#include "p_256_ecc_pp.h"
#include "controller.h"

#ifndef SMP_MAX_ENC_REPEAT
#define SMP_MAX_ENC_REPEAT  3
#endif

static void smp_rand_back(tBTM_RAND_ENC *p);
static void smp_generate_confirm(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
static void smp_generate_ltk_cont(tSMP_CB *p_cb, tSMP_INT_DATA *p_data);
static void smp_generate_y(tSMP_CB *p_cb, tSMP_INT_DATA *p);
static void smp_generate_rand_vector (tSMP_CB *p_cb, tSMP_INT_DATA *p);
static void smp_process_stk(tSMP_CB *p_cb, tSMP_ENC *p);
static void smp_calculate_comfirm_cont(tSMP_CB *p_cb, tSMP_ENC *p);
static void smp_process_confirm(tSMP_CB *p_cb, tSMP_ENC *p);
static void smp_process_compare(tSMP_CB *p_cb, tSMP_ENC *p);
static void smp_process_ediv(tSMP_CB *p_cb, tSMP_ENC *p);
static BOOLEAN smp_calculate_legacy_short_term_key(tSMP_CB *p_cb, tSMP_ENC *output);
static void smp_continue_private_key_creation(tSMP_CB *p_cb, tBTM_RAND_ENC *p);
static void smp_process_private_key(tSMP_CB *p_cb);
static void smp_finish_nonce_generation(tSMP_CB *p_cb);
static void smp_process_new_nonce(tSMP_CB *p_cb);

static const tSMP_ACT smp_encrypt_action[] = {
    smp_generate_compare,           /* SMP_GEN_COMPARE */
    smp_generate_confirm,          /* SMP_GEN_CONFIRM*/
    smp_generate_stk,               /* SMP_GEN_STK*/
    smp_generate_ltk_cont,          /* SMP_GEN_LTK */
    smp_generate_ltk,               /* SMP_GEN_DIV_LTK */
    smp_generate_rand_vector,        /* SMP_GEN_RAND_V */
    smp_generate_y,                  /* SMP_GEN_EDIV */
    smp_generate_passkey,           /* SMP_GEN_TK */
    smp_generate_srand_mrand_confirm, /* SMP_GEN_SRAND_MRAND */
    smp_generate_rand_cont         /* SMP_GEN_SRAND_MRAND_CONT */
};

#define SMP_PASSKEY_MASK    0xfff00000

void smp_debug_print_nbyte_little_endian(UINT8 *p, const UINT8 *key_name, UINT8 len)
{
#if SMP_DEBUG == TRUE
    int     ind, x;
    int     col_count = 32;
    int     row_count;
    UINT8   p_buf[512];

    SMP_TRACE_WARNING("%s(LSB ~ MSB):\n", key_name);
    memset(p_buf, 0, sizeof(p_buf));
    row_count = len % col_count ? len / col_count + 1 : len / col_count;

    ind = 0;
    for (int row = 0; row <  row_count; row++) {
        for (int column = 0, x = 0; (ind < len) && (column < col_count); column++, ind++) {
            x += sprintf((char *)&p_buf[x], "%02x ", p[ind]);
        }
        SMP_TRACE_WARNING("  [%03d]: %s", row * col_count, p_buf);
    }
#endif
}

void smp_debug_print_nbyte_big_endian (UINT8 *p, const UINT8 *key_name, UINT8 len)
{
#if SMP_DEBUG == TRUE
    UINT8  p_buf[512];

    SMP_TRACE_WARNING("%s(MSB ~ LSB):", key_name);
    memset(p_buf, 0, sizeof(p_buf));
    nrows = len % ncols ? len / ncols + 1 : len / ncols;

    int ind = 0;
    int  ncols = 32; /* num entries in one line */
    int  nrows;      /* num lines */
    int  x;

    for (int row = 0; row <  nrows; row++) {
        for (int col = 0, x = 0; (ind < len) && (col < ncols); col++, ind++) {
            x += sprintf ((char *)&p_buf[len - x - 1], "%02x ", p[ind]);
        }
        SMP_TRACE_WARNING("[%03d]: %s", row * ncols, p_buf);
    }
#endif
}

/*******************************************************************************
**
** Function         smp_encrypt_data
**
** Description      This function is called to encrypt data.
**                  It uses AES-128 encryption algorithm.
**                  Plain_text is encrypted using key, the result is at p_out.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN smp_encrypt_data (UINT8 *key, UINT8 key_len,
                          UINT8 *plain_text, UINT8 pt_len,
                          tSMP_ENC *p_out)
{
    aes_context ctx;
    UINT8 *p_start = NULL;
    UINT8 *p = NULL;
    UINT8 *p_rev_data = NULL;    /* input data in big endilan format */
    UINT8 *p_rev_key = NULL;     /* input key in big endilan format */
    UINT8 *p_rev_output = NULL;  /* encrypted output in big endilan format */

    SMP_TRACE_DEBUG ("%s\n", __func__);
    if ( (p_out == NULL ) || (key_len != SMP_ENCRYT_KEY_SIZE) ) {
        SMP_TRACE_ERROR ("%s failed\n", __func__);
        return FALSE;
    }

    if ((p_start = (UINT8 *)GKI_getbuf((SMP_ENCRYT_DATA_SIZE * 4))) == NULL) {
        SMP_TRACE_ERROR ("%s failed unable to allocate buffer\n", __func__);
        return FALSE;
    }

    if (pt_len > SMP_ENCRYT_DATA_SIZE) {
        pt_len = SMP_ENCRYT_DATA_SIZE;
    }

    memset(p_start, 0, SMP_ENCRYT_DATA_SIZE * 4);
    p = p_start;
    ARRAY_TO_STREAM (p, plain_text, pt_len); /* byte 0 to byte 15 */
    p_rev_data = p = p_start + SMP_ENCRYT_DATA_SIZE; /* start at byte 16 */
    REVERSE_ARRAY_TO_STREAM (p, p_start, SMP_ENCRYT_DATA_SIZE);  /* byte 16 to byte 31 */
    p_rev_key = p; /* start at byte 32 */
    REVERSE_ARRAY_TO_STREAM (p, key, SMP_ENCRYT_KEY_SIZE); /* byte 32 to byte 47 */

#if SMP_DEBUG == TRUE && SMP_DEBUG_VERBOSE == TRUE
    smp_debug_print_nbyte_little_endian(key, (const UINT8 *)"Key", SMP_ENCRYT_KEY_SIZE);
    smp_debug_print_nbyte_little_endian(p_start, (const UINT8 *)"Plain text", SMP_ENCRYT_DATA_SIZE);
#endif
    p_rev_output = p;
    aes_set_key(p_rev_key, SMP_ENCRYT_KEY_SIZE, &ctx);
    bluedroid_aes_encrypt(p_rev_data, p, &ctx);  /* outputs in byte 48 to byte 63 */

    p = p_out->param_buf;
    REVERSE_ARRAY_TO_STREAM (p, p_rev_output, SMP_ENCRYT_DATA_SIZE);
#if SMP_DEBUG == TRUE && SMP_DEBUG_VERBOSE == TRUE
    smp_debug_print_nbyte_little_endian(p_out->param_buf, (const UINT8 *)"Encrypted text", SMP_ENCRYT_KEY_SIZE);
#endif

    p_out->param_len = SMP_ENCRYT_KEY_SIZE;
    p_out->status = HCI_SUCCESS;
    p_out->opcode =  HCI_BLE_ENCRYPT;

    GKI_freebuf(p_start);

    return TRUE;
}

/*******************************************************************************
**
** Function         smp_generate_passkey
**
** Description      This function is called to generate passkey.
**
** Returns          void
**
*******************************************************************************/
void smp_generate_passkey(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    SMP_TRACE_DEBUG ("%s", __func__);
    p_cb->rand_enc_proc_state = SMP_GEN_TK;

    /* generate MRand or SRand */
    if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
        smp_rand_back(NULL);
    }
}

/*******************************************************************************
**
** Function         smp_proc_passkey
**
** Description      This function is called to process a passkey.
**
** Returns          void
**
*******************************************************************************/
void smp_proc_passkey(tSMP_CB *p_cb , tBTM_RAND_ENC *p)
{
    UINT8   *tt = p_cb->tk;
    tSMP_KEY    key;
    UINT32  passkey; /* 19655 test number; */
    UINT8 *pp = p->param_buf;

    SMP_TRACE_DEBUG ("%s", __func__);
    STREAM_TO_UINT32(passkey, pp);
    passkey &= ~SMP_PASSKEY_MASK;

    /* truncate by maximum value */
    while (passkey > BTM_MAX_PASSKEY_VAL) {
        passkey >>= 1;
    }

    /* save the TK */
    memset(p_cb->tk, 0, BT_OCTET16_LEN);
    UINT32_TO_STREAM(tt, passkey);

    key.key_type = SMP_KEY_TYPE_TK;
    key.p_data  = p_cb->tk;

    if (p_cb->p_callback) {
        (*p_cb->p_callback)(SMP_PASSKEY_NOTIF_EVT, p_cb->pairing_bda, (tSMP_EVT_DATA *)&passkey);
    }

    if (p_cb->selected_association_model == SMP_MODEL_SEC_CONN_PASSKEY_DISP) {
        smp_sm_event(&smp_cb, SMP_KEY_READY_EVT, &passkey);
    } else {
        smp_sm_event(p_cb, SMP_KEY_READY_EVT, (tSMP_INT_DATA *)&key);
    }
}

/*******************************************************************************
**
** Function         smp_generate_stk
**
** Description      This function is called to generate STK calculated by running
**                  AES with the TK value as key and a concatenation of the random
**                  values.
**
** Returns          void
**
*******************************************************************************/
void smp_generate_stk(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    tSMP_ENC output;
    tSMP_STATUS status = SMP_PAIR_FAIL_UNKNOWN;

    SMP_TRACE_DEBUG ("%s\n", __func__);

    if (p_cb->le_secure_connections_mode_is_used) {
        SMP_TRACE_WARNING ("FOR LE SC LTK IS USED INSTEAD OF STK");
        output.param_len = SMP_ENCRYT_KEY_SIZE;
        output.status = HCI_SUCCESS;
        output.opcode =  HCI_BLE_ENCRYPT;
        memcpy(output.param_buf, p_cb->ltk, SMP_ENCRYT_DATA_SIZE);
    } else if (!smp_calculate_legacy_short_term_key(p_cb, &output)) {
        SMP_TRACE_ERROR("%s failed", __func__);
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
        return;
    }

    smp_process_stk(p_cb, &output);
}

/*******************************************************************************
**
** Function         smp_generate_srand_mrand_confirm
**
** Description      This function is called to start the second pairing phase by
**                  start generating random number.
**
**
** Returns          void
**
*******************************************************************************/
void smp_generate_srand_mrand_confirm(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    SMP_TRACE_DEBUG ("%s\n", __func__);
    p_cb->rand_enc_proc_state = SMP_GEN_SRAND_MRAND;
    /* generate MRand or SRand */
    if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
        smp_rand_back(NULL);
    }
}

/*******************************************************************************
**
** Function         smp_generate_rand_cont
**
** Description      This function is called to generate another 64 bits random for
**                  MRand or Srand.
**
** Returns          void
**
*******************************************************************************/
void smp_generate_rand_cont(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    SMP_TRACE_DEBUG ("%s\n", __func__);
    p_cb->rand_enc_proc_state = SMP_GEN_SRAND_MRAND_CONT;
    /* generate 64 MSB of MRand or SRand */
    if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
        smp_rand_back(NULL);
    }
}

/*******************************************************************************
**
** Function         smp_generate_ltk
**
** Description      This function is called:
**                  - in legacy pairing - to calculate LTK, starting with DIV
**                    generation;
**                  - in LE Secure Connections pairing over LE transport - to process LTK
**                    already generated to encrypt LE link;
**                  - in LE Secure Connections pairing over BR/EDR transport - to start
**                    BR/EDR Link Key processing.
**
** Returns          void
**
*******************************************************************************/
void smp_generate_ltk(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    BOOLEAN div_status;
    SMP_TRACE_DEBUG ("%s\n", __FUNCTION__);
    if (smp_get_br_state() == SMP_BR_STATE_BOND_PENDING) {
        smp_br_process_link_key(p_cb, NULL);
        return;
    } else if (p_cb->le_secure_connections_mode_is_used) {
        smp_process_secure_connection_long_term_key();
        return;
    }

    div_status = btm_get_local_div(p_cb->pairing_bda, &p_cb->div);

    if (div_status) {
        smp_generate_ltk_cont(p_cb, NULL);
    } else {
        SMP_TRACE_DEBUG ("Generate DIV for LTK\n");
        p_cb->rand_enc_proc_state = SMP_GEN_DIV_LTK;
        /* generate MRand or SRand */
        if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
            smp_rand_back(NULL);
        }
    }
}

/*******************************************************************************
**
** Function         smp_compute_csrk
**
** Description      This function is called to calculate CSRK
**
**
** Returns          void
**
*******************************************************************************/
void smp_compute_csrk(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    BT_OCTET16  er;
    UINT8       buffer[4]; /* for (r || DIV)  r=1*/
    UINT16      r = 1;
    UINT8       *p = buffer;
    tSMP_ENC    output;
    tSMP_STATUS   status = SMP_PAIR_FAIL_UNKNOWN;

    SMP_TRACE_DEBUG ("smp_compute_csrk div=%x\n", p_cb->div);
    BTM_GetDeviceEncRoot(er);
    /* CSRK = d1(ER, DIV, 1) */
    UINT16_TO_STREAM(p, p_cb->div);
    UINT16_TO_STREAM(p, r);

    if (!SMP_Encrypt(er, BT_OCTET16_LEN, buffer, 4, &output)) {
        SMP_TRACE_ERROR("smp_generate_csrk failed\n");
        if (p_cb->smp_over_br) {
            smp_br_state_machine_event(p_cb, SMP_BR_AUTH_CMPL_EVT, &status);
        } else {
            smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
        }
    } else {
        memcpy((void *)p_cb->csrk, output.param_buf, BT_OCTET16_LEN);
        smp_send_csrk_info(p_cb, NULL);
    }
}

/*******************************************************************************
**
** Function         smp_generate_csrk
**
** Description      This function is called to calculate CSRK, starting with DIV
**                  generation.
**
**
** Returns          void
**
*******************************************************************************/
void smp_generate_csrk(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    BOOLEAN     div_status;

    SMP_TRACE_DEBUG ("smp_generate_csrk");

    div_status = btm_get_local_div(p_cb->pairing_bda, &p_cb->div);
    if (div_status) {
        smp_compute_csrk(p_cb, NULL);
    } else {
        SMP_TRACE_DEBUG ("Generate DIV for CSRK");
        p_cb->rand_enc_proc_state = SMP_GEN_DIV_CSRK;
        if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
            smp_rand_back(NULL);
        }
    }
}

/*******************************************************************************
** Function         smp_concatenate_peer
**                  add pairing command sent from local device into p1.
*******************************************************************************/
void smp_concatenate_local( tSMP_CB *p_cb, UINT8 **p_data, UINT8 op_code)
{
    UINT8   *p = *p_data;

    SMP_TRACE_DEBUG ("%s\n", __func__);
    UINT8_TO_STREAM(p, op_code);
    UINT8_TO_STREAM(p, p_cb->local_io_capability);
    UINT8_TO_STREAM(p, p_cb->loc_oob_flag);
    UINT8_TO_STREAM(p, p_cb->loc_auth_req);
    UINT8_TO_STREAM(p, p_cb->loc_enc_size);
    UINT8_TO_STREAM(p, p_cb->local_i_key);
    UINT8_TO_STREAM(p, p_cb->local_r_key);

    *p_data = p;
}

/*******************************************************************************
** Function         smp_concatenate_peer
**                  add pairing command received from peer device into p1.
*******************************************************************************/
void smp_concatenate_peer( tSMP_CB *p_cb, UINT8 **p_data, UINT8 op_code)
{
    UINT8   *p = *p_data;

    SMP_TRACE_DEBUG ("smp_concatenate_peer \n");
    UINT8_TO_STREAM(p, op_code);
    UINT8_TO_STREAM(p, p_cb->peer_io_caps);
    UINT8_TO_STREAM(p, p_cb->peer_oob_flag);
    UINT8_TO_STREAM(p, p_cb->peer_auth_req);
    UINT8_TO_STREAM(p, p_cb->peer_enc_size);
    UINT8_TO_STREAM(p, p_cb->peer_i_key);
    UINT8_TO_STREAM(p, p_cb->peer_r_key);

    *p_data = p;
}

/*******************************************************************************
**
** Function         smp_gen_p1_4_confirm
**
** Description      Generate Confirm/Compare Step1:
**                  p1 = pres || preq || rat' || iat'
**
** Returns          void
**
*******************************************************************************/
void smp_gen_p1_4_confirm( tSMP_CB *p_cb, BT_OCTET16 p1)
{
    UINT8 *p = (UINT8 *)p1;
    tBLE_ADDR_TYPE    addr_type = 0;
    BD_ADDR           remote_bda;

    SMP_TRACE_DEBUG ("smp_gen_p1_4_confirm\n");

    if (!BTM_ReadRemoteConnectionAddr(p_cb->pairing_bda, remote_bda, &addr_type)) {
        SMP_TRACE_ERROR("can not generate confirm for unknown device\n");
        return;
    }

    BTM_ReadConnectionAddr( p_cb->pairing_bda, p_cb->local_bda, &p_cb->addr_type);

    if (p_cb->role == HCI_ROLE_MASTER) {
        /* LSB : rat': initiator's(local) address type */
        UINT8_TO_STREAM(p, p_cb->addr_type);
        /* LSB : iat': responder's address type */
        UINT8_TO_STREAM(p, addr_type);
        /* concatinate preq */
        smp_concatenate_local(p_cb, &p, SMP_OPCODE_PAIRING_REQ);
        /* concatinate pres */
        smp_concatenate_peer(p_cb, &p, SMP_OPCODE_PAIRING_RSP);
    } else {
        /* LSB : iat': initiator's address type */
        UINT8_TO_STREAM(p, addr_type);
        /* LSB : rat': responder's(local) address type */
        UINT8_TO_STREAM(p, p_cb->addr_type);
        /* concatinate preq */
        smp_concatenate_peer(p_cb, &p, SMP_OPCODE_PAIRING_REQ);
        /* concatinate pres */
        smp_concatenate_local(p_cb, &p, SMP_OPCODE_PAIRING_RSP);
    }
#if SMP_DEBUG == TRUE
    SMP_TRACE_DEBUG("p1 = pres || preq || rat' || iat'\n");
    smp_debug_print_nbyte_little_endian ((UINT8 *)p1, (const UINT8 *)"P1", 16);
#endif
}

/*******************************************************************************
**
** Function         smp_gen_p2_4_confirm
**
** Description      Generate Confirm/Compare Step2:
**                  p2 = padding || ia || ra
**
** Returns          void
**
*******************************************************************************/
void smp_gen_p2_4_confirm( tSMP_CB *p_cb, BT_OCTET16 p2)
{
    UINT8       *p = (UINT8 *)p2;
    BD_ADDR     remote_bda;
    tBLE_ADDR_TYPE  addr_type = 0;
    SMP_TRACE_DEBUG ("smp_gen_p2_4_confirm\n");
    if (!BTM_ReadRemoteConnectionAddr(p_cb->pairing_bda, remote_bda, &addr_type)) {
        SMP_TRACE_ERROR("can not generate confirm p2 for unknown device\n");
        return;
    }

    SMP_TRACE_DEBUG ("smp_gen_p2_4_confirm\n");

    memset(p, 0, sizeof(BT_OCTET16));

    if (p_cb->role == HCI_ROLE_MASTER) {
        /* LSB ra */
        BDADDR_TO_STREAM(p, remote_bda);
        /* ia */
        BDADDR_TO_STREAM(p, p_cb->local_bda);
    } else {
        /* LSB ra */
        BDADDR_TO_STREAM(p, p_cb->local_bda);
        /* ia */
        BDADDR_TO_STREAM(p, remote_bda);
    }
#if SMP_DEBUG == TRUE
    SMP_TRACE_DEBUG("p2 = padding || ia || ra");
    smp_debug_print_nbyte_little_endian(p2, (const UINT8 *)"p2", 16);
#endif
}

/*******************************************************************************
**
** Function         smp_calculate_comfirm
**
** Description      This function is called to calculate Confirm value.
**
** Returns          void
**
*******************************************************************************/
void smp_calculate_comfirm (tSMP_CB *p_cb, BT_OCTET16 rand, BD_ADDR bda)
{
    UNUSED(bda);

    BT_OCTET16      p1;
    tSMP_ENC       output;
    tSMP_STATUS     status = SMP_PAIR_FAIL_UNKNOWN;

    SMP_TRACE_DEBUG ("smp_calculate_comfirm \n");
    /* generate p1 = pres || preq || rat' || iat' */
    smp_gen_p1_4_confirm(p_cb, p1);

    /* p1 = rand XOR p1 */
    smp_xor_128(p1, rand);

    smp_debug_print_nbyte_little_endian ((UINT8 *)p1, (const UINT8 *)"P1' = r XOR p1", 16);

    /* calculate e(k, r XOR p1), where k = TK */
    if (!SMP_Encrypt(p_cb->tk, BT_OCTET16_LEN, p1, BT_OCTET16_LEN, &output)) {
        SMP_TRACE_ERROR("smp_generate_csrk failed");
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
    } else {
        smp_calculate_comfirm_cont(p_cb, &output);
    }
}

/*******************************************************************************
**
** Function         smp_calculate_comfirm_cont
**
** Description      This function is called when SConfirm/MConfirm is generated
**                  proceed to send the Confirm request/response to peer device.
**
** Returns          void
**
*******************************************************************************/
static void smp_calculate_comfirm_cont(tSMP_CB *p_cb, tSMP_ENC *p)
{
    BT_OCTET16    p2;
    tSMP_ENC      output;
    tSMP_STATUS     status = SMP_PAIR_FAIL_UNKNOWN;

    SMP_TRACE_DEBUG ("smp_calculate_comfirm_cont \n");
#if SMP_DEBUG == TRUE
    SMP_TRACE_DEBUG("Confirm step 1 p1' = e(k, r XOR p1)  Generated\n");
    smp_debug_print_nbyte_little_endian (p->param_buf, (const UINT8 *)"C1", 16);
#endif

    smp_gen_p2_4_confirm(p_cb, p2);

    /* calculate p2 = (p1' XOR p2) */
    smp_xor_128(p2, p->param_buf);
    smp_debug_print_nbyte_little_endian ((UINT8 *)p2, (const UINT8 *)"p2' = C1 xor p2", 16);

    /* calculate: Confirm = E(k, p1' XOR p2) */
    if (!SMP_Encrypt(p_cb->tk, BT_OCTET16_LEN, p2, BT_OCTET16_LEN, &output)) {
        SMP_TRACE_ERROR("smp_calculate_comfirm_cont failed\n");
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
    } else {
        SMP_TRACE_DEBUG("p_cb->rand_enc_proc_state=%d\n", p_cb->rand_enc_proc_state);
        switch (p_cb->rand_enc_proc_state) {
        case SMP_GEN_CONFIRM:
            smp_process_confirm(p_cb, &output);
            break;

        case SMP_GEN_COMPARE:
            smp_process_compare(p_cb, &output);
            break;
        }
    }
}

/*******************************************************************************
**
** Function         smp_generate_confirm
**
** Description      This function is called when a 48 bits random number is generated
**                  as SRand or MRand, continue to calculate Sconfirm or MConfirm.
**
** Returns          void
**
*******************************************************************************/
static void smp_generate_confirm(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    SMP_TRACE_DEBUG ("%s\n", __func__);
    p_cb->rand_enc_proc_state = SMP_GEN_CONFIRM;
    smp_debug_print_nbyte_little_endian ((UINT8 *)p_cb->rand,  (const UINT8 *)"local rand", 16);
    smp_calculate_comfirm(p_cb, p_cb->rand, p_cb->pairing_bda);
}

/*******************************************************************************
**
** Function         smp_generate_compare
**
** Description      This function is called to generate SConfirm for Slave device,
**                  or MSlave for Master device. This function can be also used for
**                  generating Compare number for confirm value check.
**
** Returns          void
**
*******************************************************************************/
void smp_generate_compare (tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    SMP_TRACE_DEBUG ("smp_generate_compare \n");
    p_cb->rand_enc_proc_state = SMP_GEN_COMPARE;
    smp_debug_print_nbyte_little_endian ((UINT8 *)p_cb->rrand,  (const UINT8 *)"peer rand", 16);
    smp_calculate_comfirm(p_cb, p_cb->rrand, p_cb->local_bda);
}

/*******************************************************************************
**
** Function         smp_process_confirm
**
** Description      This function is called when SConfirm/MConfirm is generated
**                  proceed to send the Confirm request/response to peer device.
**
** Returns          void
**
*******************************************************************************/
static void smp_process_confirm(tSMP_CB *p_cb, tSMP_ENC *p)
{
    tSMP_KEY    key;

    SMP_TRACE_DEBUG ("%s\n", __FUNCTION__);
    memcpy(p_cb->confirm, p->param_buf, BT_OCTET16_LEN);

#if (SMP_DEBUG == TRUE)
    SMP_TRACE_DEBUG("Confirm  Generated");
    smp_debug_print_nbyte_little_endian ((UINT8 *)p_cb->confirm,  (const UINT8 *)"Confirm", 16);
#endif

    key.key_type = SMP_KEY_TYPE_CFM;
    key.p_data = p->param_buf;

    smp_sm_event(p_cb, SMP_KEY_READY_EVT, &key);

}

/*******************************************************************************
**
** Function         smp_process_compare
**
** Description      This function is called when Compare is generated using the
**                  RRand and local BDA, TK information.
**
** Returns          void
**
*******************************************************************************/
static void smp_process_compare(tSMP_CB *p_cb, tSMP_ENC *p)
{
    tSMP_KEY    key;

    SMP_TRACE_DEBUG ("smp_process_compare \n");
#if (SMP_DEBUG == TRUE)
    SMP_TRACE_DEBUG("Compare Generated\n");
    smp_debug_print_nbyte_little_endian (p->param_buf,  (const UINT8 *)"Compare", 16);
#endif
    key.key_type = SMP_KEY_TYPE_CMP;
    key.p_data   = p->param_buf;
    //smp_set_state(SMP_STATE_CONFIRM);
    smp_sm_event(p_cb, SMP_KEY_READY_EVT, &key);
}

/*******************************************************************************
**
** Function         smp_process_stk
**
** Description      This function is called when STK is generated
**                  proceed to send the encrypt the link using STK.
**
** Returns          void
**
*******************************************************************************/
static void smp_process_stk(tSMP_CB *p_cb, tSMP_ENC *p)
{
    tSMP_KEY    key;

    SMP_TRACE_DEBUG ("smp_process_stk ");
#if (SMP_DEBUG == TRUE)
    SMP_TRACE_ERROR("STK Generated");
#endif
    smp_mask_enc_key(p_cb->loc_enc_size, p->param_buf);

    key.key_type = SMP_KEY_TYPE_STK;
    key.p_data   = p->param_buf;

    smp_sm_event(p_cb, SMP_KEY_READY_EVT, &key);
}

/*******************************************************************************
**
** Function         smp_generate_ltk_cont
**
** Description      This function is to calculate LTK = d1(ER, DIV, 0)= e(ER, DIV)
**
** Returns          void
**
*******************************************************************************/
static void smp_generate_ltk_cont(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UNUSED(p_data);

    BT_OCTET16  er;
    tSMP_ENC    output;
    tSMP_STATUS     status = SMP_PAIR_FAIL_UNKNOWN;

    SMP_TRACE_DEBUG ("%s\n", __func__);
    BTM_GetDeviceEncRoot(er);

    /* LTK = d1(ER, DIV, 0)= e(ER, DIV)*/
    if (!SMP_Encrypt(er, BT_OCTET16_LEN, (UINT8 *)&p_cb->div,
                     sizeof(UINT16), &output)) {
        SMP_TRACE_ERROR("%s failed\n", __func__);
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
    } else {
        /* mask the LTK */
        smp_mask_enc_key(p_cb->loc_enc_size, output.param_buf);
        memcpy((void *)p_cb->ltk, output.param_buf, BT_OCTET16_LEN);
        smp_generate_rand_vector(p_cb, NULL);
    }
}

/*******************************************************************************
**
** Function         smp_generate_y
**
** Description      This function is to proceed generate Y = E(DHK, Rand)
**
** Returns          void
**
*******************************************************************************/
static void smp_generate_y(tSMP_CB *p_cb, tSMP_INT_DATA *p)
{
    UNUSED(p);

    BT_OCTET16  dhk;
    tSMP_ENC   output;
    tSMP_STATUS     status = SMP_PAIR_FAIL_UNKNOWN;


    SMP_TRACE_DEBUG ("smp_generate_y \n");
    BTM_GetDeviceDHK(dhk);

    if (!SMP_Encrypt(dhk, BT_OCTET16_LEN, p_cb->enc_rand,
                     BT_OCTET8_LEN, &output)) {
        SMP_TRACE_ERROR("smp_generate_y failed");
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
    } else {
        smp_process_ediv(p_cb, &output);
    }
}

/*******************************************************************************
**
** Function         smp_generate_rand_vector
**
** Description      This function is called when LTK is generated, send state machine
**                  event to SMP.
**
** Returns          void
**
*******************************************************************************/
static void smp_generate_rand_vector (tSMP_CB *p_cb, tSMP_INT_DATA *p)
{
    UNUSED(p);

    /* generate EDIV and rand now */
    /* generate random vector */
    SMP_TRACE_DEBUG ("smp_generate_rand_vector\n");
    p_cb->rand_enc_proc_state = SMP_GEN_RAND_V;
    if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
        smp_rand_back(NULL);
    }
}

/*******************************************************************************
**
** Function         smp_process_ediv
**
** Description      This function is to calculate EDIV = Y xor DIV
**
** Returns          void
**
*******************************************************************************/
static void smp_process_ediv(tSMP_CB *p_cb, tSMP_ENC *p)
{
    tSMP_KEY    key;
    UINT8 *pp = p->param_buf;
    UINT16  y;

    SMP_TRACE_DEBUG ("smp_process_ediv ");
    STREAM_TO_UINT16(y, pp);

    /* EDIV = Y xor DIV */
    p_cb->ediv = p_cb->div ^ y;
    /* send LTK ready */
    SMP_TRACE_DEBUG("LTK ready");
    key.key_type = SMP_KEY_TYPE_LTK;
    key.p_data   = p->param_buf;

    smp_sm_event(p_cb, SMP_KEY_READY_EVT, &key);
}

/*******************************************************************************
**
** Function         smp_calculate_legacy_short_term_key
**
** Description      The function calculates legacy STK.
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
*******************************************************************************/
BOOLEAN smp_calculate_legacy_short_term_key(tSMP_CB *p_cb, tSMP_ENC *output)
{
    BT_OCTET16 ptext;
    UINT8 *p = ptext;

    SMP_TRACE_DEBUG ("%s\n", __func__);
    memset(p, 0, BT_OCTET16_LEN);
    if (p_cb->role == HCI_ROLE_MASTER) {
        memcpy(p, p_cb->rand, BT_OCTET8_LEN);
        memcpy(&p[BT_OCTET8_LEN], p_cb->rrand, BT_OCTET8_LEN);
    } else {
        memcpy(p, p_cb->rrand, BT_OCTET8_LEN);
        memcpy(&p[BT_OCTET8_LEN], p_cb->rand, BT_OCTET8_LEN);
    }

    BOOLEAN encrypted;
    /* generate STK = Etk(rand|rrand)*/
    encrypted = SMP_Encrypt( p_cb->tk, BT_OCTET16_LEN, ptext, BT_OCTET16_LEN, output);
    if (!encrypted) {
        SMP_TRACE_ERROR("%s failed\n", __func__);
    }
    return encrypted;
}

/*******************************************************************************
**
** Function         smp_create_private_key
**
** Description      This function is called to create private key used to
**                  calculate public key and DHKey.
**                  The function starts private key creation requesting controller
**                  to generate [0-7] octets of private key.
**
** Returns          void
**
*******************************************************************************/
void smp_create_private_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG ("%s", __FUNCTION__);
    p_cb->rand_enc_proc_state = SMP_GENERATE_PRIVATE_KEY_0_7;
    if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
        smp_rand_back(NULL);
    }
}

/*******************************************************************************
**
** Function         smp_use_oob_private_key
**
** Description      This function is called
**                  - to save the secret key used to calculate the public key used
**                    in calculations of commitment sent OOB to a peer
**                  - to use this secret key to recalculate the public key and
**                    start the process of sending this public key to the peer
**                  if secret/public keys have to be reused.
**                  If the keys aren't supposed to be reused, continue from the
**                  point from which request for OOB data was issued.
**
** Returns          void
**
*******************************************************************************/
void smp_use_oob_private_key(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG ("%s req_oob_type: %d, role: %d\n",
                     __func__, p_cb->req_oob_type, p_cb->role);

    switch (p_cb->req_oob_type) {
    case SMP_OOB_BOTH:
    case SMP_OOB_LOCAL:
        SMP_TRACE_DEBUG("%s restore secret key\n", __func__);
        memcpy(p_cb->private_key, p_cb->sc_oob_data.loc_oob_data.private_key_used, BT_OCTET32_LEN);
        smp_process_private_key(p_cb);
        break;
    default:
        SMP_TRACE_DEBUG("%s create secret key anew\n", __func__);
        smp_set_state(SMP_STATE_PAIR_REQ_RSP);
        smp_decide_association_model(p_cb, NULL);
        break;
    }
}

/*******************************************************************************
**
** Function         smp_continue_private_key_creation
**
** Description      This function is used to continue private key creation.
**
** Returns          void
**
*******************************************************************************/
void smp_continue_private_key_creation (tSMP_CB *p_cb, tBTM_RAND_ENC *p)
{
    UINT8   state = p_cb->rand_enc_proc_state & ~0x80;
    SMP_TRACE_DEBUG ("%s state=0x%x\n", __func__, state);

    switch (state) {
    case SMP_GENERATE_PRIVATE_KEY_0_7:
        memcpy((void *)p_cb->private_key, p->param_buf, p->param_len);
        p_cb->rand_enc_proc_state = SMP_GENERATE_PRIVATE_KEY_8_15;
        if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
            smp_rand_back(NULL);
        }
        break;

    case SMP_GENERATE_PRIVATE_KEY_8_15:
        memcpy((void *)&p_cb->private_key[8], p->param_buf, p->param_len);
        p_cb->rand_enc_proc_state = SMP_GENERATE_PRIVATE_KEY_16_23;
        if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
            smp_rand_back(NULL);
        }
        break;

    case SMP_GENERATE_PRIVATE_KEY_16_23:
        memcpy((void *)&p_cb->private_key[16], p->param_buf, p->param_len);
        p_cb->rand_enc_proc_state = SMP_GENERATE_PRIVATE_KEY_24_31;
        if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
            smp_rand_back(NULL);
        }
        break;

    case SMP_GENERATE_PRIVATE_KEY_24_31:
        memcpy((void *)&p_cb->private_key[24], p->param_buf, p->param_len);
        smp_process_private_key (p_cb);
        break;

    default:
        break;
    }

    return;
}

/*******************************************************************************
**
** Function         smp_process_private_key
**
** Description      This function processes private key.
**                  It calculates public key and notifies SM that private key /
**                  public key pair is created.
**
** Returns          void
**
*******************************************************************************/
void smp_process_private_key(tSMP_CB *p_cb)
{
    Point       public_key;
    BT_OCTET32  private_key;

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);

    memcpy(private_key, p_cb->private_key, BT_OCTET32_LEN);
    ECC_PointMult(&public_key, &(curve_p256.G), (DWORD *) private_key, KEY_LENGTH_DWORDS_P256);
    memcpy(p_cb->loc_publ_key.x, public_key.x, BT_OCTET32_LEN);
    memcpy(p_cb->loc_publ_key.y, public_key.y, BT_OCTET32_LEN);

    smp_debug_print_nbyte_little_endian (p_cb->private_key, (const UINT8 *)"private",
                                         BT_OCTET32_LEN);
    smp_debug_print_nbyte_little_endian (p_cb->loc_publ_key.x, (const UINT8 *)"local public(x)",
                                         BT_OCTET32_LEN);
    smp_debug_print_nbyte_little_endian (p_cb->loc_publ_key.y, (const UINT8 *)"local public(y)",
                                         BT_OCTET32_LEN);
    p_cb->flags |= SMP_PAIR_FLAG_HAVE_LOCAL_PUBL_KEY;
    smp_sm_event(p_cb, SMP_LOC_PUBL_KEY_CRTD_EVT, NULL);
}

/*******************************************************************************
**
** Function         smp_compute_dhkey
**
** Description      The function:
**                  - calculates a new public key using as input local private
**                    key and peer public key;
**                  - saves the new public key x-coordinate as DHKey.
**
** Returns          void
**
*******************************************************************************/
void smp_compute_dhkey (tSMP_CB *p_cb)
{
    Point       peer_publ_key, new_publ_key;
    BT_OCTET32  private_key;

    SMP_TRACE_DEBUG ("%s\n", __FUNCTION__);

    memcpy(private_key, p_cb->private_key, BT_OCTET32_LEN);
    memcpy(peer_publ_key.x, p_cb->peer_publ_key.x, BT_OCTET32_LEN);
    memcpy(peer_publ_key.y, p_cb->peer_publ_key.y, BT_OCTET32_LEN);

    ECC_PointMult(&new_publ_key, &peer_publ_key, (DWORD *) private_key, KEY_LENGTH_DWORDS_P256);

    memcpy(p_cb->dhkey, new_publ_key.x, BT_OCTET32_LEN);

    smp_debug_print_nbyte_little_endian (p_cb->dhkey, (const UINT8 *)"Old DHKey",
                                         BT_OCTET32_LEN);

    smp_debug_print_nbyte_little_endian (p_cb->private_key, (const UINT8 *)"private",
                                         BT_OCTET32_LEN);
    smp_debug_print_nbyte_little_endian (p_cb->peer_publ_key.x, (const UINT8 *)"rem public(x)",
                                         BT_OCTET32_LEN);
    smp_debug_print_nbyte_little_endian (p_cb->peer_publ_key.y, (const UINT8 *)"rem public(y)",
                                         BT_OCTET32_LEN);
    smp_debug_print_nbyte_little_endian (p_cb->dhkey, (const UINT8 *)"Reverted DHKey",
                                         BT_OCTET32_LEN);
}

/*******************************************************************************
**
** Function         smp_calculate_local_commitment
**
** Description      The function calculates and saves local commmitment in CB.
**
** Returns          void
**
*******************************************************************************/
void smp_calculate_local_commitment(tSMP_CB *p_cb)
{
    UINT8 random_input;

    SMP_TRACE_DEBUG("%s\n", __FUNCTION__);

    switch (p_cb->selected_association_model) {
    case SMP_MODEL_SEC_CONN_JUSTWORKS:
    case SMP_MODEL_SEC_CONN_NUM_COMP:
        if (p_cb->role  == HCI_ROLE_MASTER)
            SMP_TRACE_WARNING ("local commitment calc on master is not expected \
                                    for Just Works/Numeric Comparison models\n");
        smp_calculate_f4(p_cb->loc_publ_key.x, p_cb->peer_publ_key.x, p_cb->rand, 0,
                         p_cb->commitment);
        break;
    case SMP_MODEL_SEC_CONN_PASSKEY_ENT:
    case SMP_MODEL_SEC_CONN_PASSKEY_DISP:
        random_input = smp_calculate_random_input(p_cb->local_random, p_cb->round);
        smp_calculate_f4(p_cb->loc_publ_key.x, p_cb->peer_publ_key.x, p_cb->rand,
                         random_input, p_cb->commitment);
        break;
    case SMP_MODEL_SEC_CONN_OOB:
        SMP_TRACE_WARNING ("local commitment calc is expected for OOB model BEFORE pairing\n");
        smp_calculate_f4(p_cb->loc_publ_key.x, p_cb->loc_publ_key.x, p_cb->local_random, 0,
                         p_cb->commitment);
        break;
    default:
        SMP_TRACE_ERROR("Association Model = %d is not used in LE SC\n",
                        p_cb->selected_association_model);
        return;
    }

    SMP_TRACE_EVENT ("local commitment calculation is completed");
}

/*******************************************************************************
**
** Function         smp_calculate_peer_commitment
**
** Description      The function calculates and saves peer commmitment at the
**                  provided output buffer.
**
** Returns          void
**
*******************************************************************************/
void smp_calculate_peer_commitment(tSMP_CB *p_cb, BT_OCTET16 output_buf)
{
    UINT8 ri;

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);

    switch (p_cb->selected_association_model) {
    case SMP_MODEL_SEC_CONN_JUSTWORKS:
    case SMP_MODEL_SEC_CONN_NUM_COMP:
        if (p_cb->role  == HCI_ROLE_SLAVE)
            SMP_TRACE_WARNING ("peer commitment calc on slave is not expected \
                for Just Works/Numeric Comparison models\n");
        smp_calculate_f4(p_cb->peer_publ_key.x, p_cb->loc_publ_key.x, p_cb->rrand, 0,
                         output_buf);
        break;
    case SMP_MODEL_SEC_CONN_PASSKEY_ENT:
    case SMP_MODEL_SEC_CONN_PASSKEY_DISP:
        ri = smp_calculate_random_input(p_cb->peer_random, p_cb->round);
        smp_calculate_f4(p_cb->peer_publ_key.x, p_cb->loc_publ_key.x, p_cb->rrand, ri,
                         output_buf);
        break;
    case SMP_MODEL_SEC_CONN_OOB:
        smp_calculate_f4(p_cb->peer_publ_key.x, p_cb->peer_publ_key.x, p_cb->peer_random, 0,
                         output_buf);
        break;
    default:
        SMP_TRACE_ERROR("Association Model = %d is not used in LE SC\n",
                        p_cb->selected_association_model);
        return;
    }

    SMP_TRACE_EVENT ("peer commitment calculation is completed\n");
}

/*******************************************************************************
**
** Function         smp_calculate_f4
**
** Description      The function calculates
**                  C = f4(U, V, X, Z) = AES-CMAC (U||V||Z)
**                                               X
**                  where
**                  input:  U is 256 bit,
**                          V is 256 bit,
**                          X is 128 bit,
**                          Z is 8 bit,
**                  output: C is 128 bit.
**
** Returns          void
**
** Note             The LSB is the first octet, the MSB is the last octet of
**                  the AES-CMAC input/output stream.
**
*******************************************************************************/
void smp_calculate_f4(UINT8 *u, UINT8 *v, UINT8 *x, UINT8 z, UINT8 *c)
{
    UINT8   msg_len = BT_OCTET32_LEN /* U size */ + BT_OCTET32_LEN /* V size */ + 1 /* Z size */;
    UINT8   msg[BT_OCTET32_LEN + BT_OCTET32_LEN + 1];
    UINT8   key[BT_OCTET16_LEN];
    UINT8   cmac[BT_OCTET16_LEN];
    UINT8   *p = NULL;
#if SMP_DEBUG == TRUE
    UINT8   *p_prnt = NULL;
#endif

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);

#if SMP_DEBUG == TRUE
    p_prnt = u;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"U", BT_OCTET32_LEN);
    p_prnt = v;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"V", BT_OCTET32_LEN);
    p_prnt = x;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"X", BT_OCTET16_LEN);
    p_prnt = &z;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"Z", 1);
#endif

    p = msg;
    UINT8_TO_STREAM(p, z);
    ARRAY_TO_STREAM(p, v, BT_OCTET32_LEN);
    ARRAY_TO_STREAM(p, u, BT_OCTET32_LEN);
#if SMP_DEBUG == TRUE
    p_prnt = msg;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"M", msg_len);
#endif

    p = key;
    ARRAY_TO_STREAM(p, x, BT_OCTET16_LEN);
#if SMP_DEBUG == TRUE
    p_prnt = key;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"K", BT_OCTET16_LEN);
#endif

    aes_cipher_msg_auth_code(key, msg, msg_len, BT_OCTET16_LEN, cmac);
#if SMP_DEBUG == TRUE
    p_prnt = cmac;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"AES_CMAC", BT_OCTET16_LEN);
#endif

    p = c;
    ARRAY_TO_STREAM(p, cmac, BT_OCTET16_LEN);
}

/*******************************************************************************
**
** Function         smp_calculate_numeric_comparison_display_number
**
** Description      The function calculates and saves number to display in numeric
**                  comparison association mode.
**
** Returns          void
**
*******************************************************************************/
void smp_calculate_numeric_comparison_display_number(tSMP_CB *p_cb,
        tSMP_INT_DATA *p_data)
{
    SMP_TRACE_DEBUG ("%s", __func__);

    if (p_cb->role == HCI_ROLE_MASTER) {
        p_cb->number_to_display =
            smp_calculate_g2(p_cb->loc_publ_key.x, p_cb->peer_publ_key.x, p_cb->rand,
                             p_cb->rrand);
    } else {
        p_cb->number_to_display =
            smp_calculate_g2(p_cb->peer_publ_key.x, p_cb->loc_publ_key.x, p_cb->rrand,
                             p_cb->rand);
    }

    if (p_cb->number_to_display >= (BTM_MAX_PASSKEY_VAL + 1)) {
        UINT8 reason;
        reason = p_cb->failure = SMP_PAIR_FAIL_UNKNOWN;
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &reason);
        return;
    }

    SMP_TRACE_EVENT("Number to display in numeric comparison = %d", p_cb->number_to_display);
    p_cb->cb_evt = SMP_NC_REQ_EVT;
    smp_sm_event(p_cb, SMP_SC_DSPL_NC_EVT, &p_cb->number_to_display);
    return;
}

/*******************************************************************************
**
** Function         smp_calculate_g2
**
** Description      The function calculates
**                  g2(U, V, X, Y) = AES-CMAC (U||V||Y) mod 2**32 mod 10**6
**                                           X
**                  and
**                  Vres = g2(U, V, X, Y) mod 10**6
**                  where
**                  input:  U     is 256 bit,
**                          V     is 256 bit,
**                          X     is 128 bit,
**                          Y     is 128 bit,
**
** Returns          Vres.
**                  Expected value has to be in the range [0 - 999999] i.e. [0 - 0xF423F].
**                  Vres = 1000000 means that the calculation fails.
**
** Note             The LSB is the first octet, the MSB is the last octet of
**                  the AES-CMAC input/output stream.
**
*******************************************************************************/
UINT32 smp_calculate_g2(UINT8 *u, UINT8 *v, UINT8 *x, UINT8 *y)
{
    UINT8   msg_len = BT_OCTET32_LEN /* U size */ + BT_OCTET32_LEN /* V size */
                      + BT_OCTET16_LEN /* Y size */;
    UINT8   msg[BT_OCTET32_LEN + BT_OCTET32_LEN + BT_OCTET16_LEN];
    UINT8   key[BT_OCTET16_LEN];
    UINT8   cmac[BT_OCTET16_LEN];
    UINT8   *p = NULL;
    UINT32  vres;
#if SMP_DEBUG == TRUE
    UINT8   *p_prnt = NULL;
#endif

    SMP_TRACE_DEBUG ("%s\n", __FUNCTION__);

    p = msg;
    ARRAY_TO_STREAM(p, y, BT_OCTET16_LEN);
    ARRAY_TO_STREAM(p, v, BT_OCTET32_LEN);
    ARRAY_TO_STREAM(p, u, BT_OCTET32_LEN);
#if SMP_DEBUG == TRUE
    p_prnt = u;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"U", BT_OCTET32_LEN);
    p_prnt = v;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"V", BT_OCTET32_LEN);
    p_prnt = x;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"X", BT_OCTET16_LEN);
    p_prnt = y;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"Y", BT_OCTET16_LEN);
#endif

    p = key;
    ARRAY_TO_STREAM(p, x, BT_OCTET16_LEN);
#if SMP_DEBUG == TRUE
    p_prnt = key;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"K", BT_OCTET16_LEN);
#endif

    if (!aes_cipher_msg_auth_code(key, msg, msg_len, BT_OCTET16_LEN, cmac)) {
        SMP_TRACE_ERROR("%s failed", __FUNCTION__);
        return (BTM_MAX_PASSKEY_VAL + 1);
    }

#if SMP_DEBUG == TRUE
    p_prnt = cmac;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"AES-CMAC", BT_OCTET16_LEN);
#endif

    /* vres = cmac mod 2**32 mod 10**6 */
    p = &cmac[0];
    STREAM_TO_UINT32(vres, p);
#if SMP_DEBUG == TRUE
    p_prnt = (UINT8 *) &vres;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"cmac mod 2**32", 4);
#endif

    while (vres > BTM_MAX_PASSKEY_VAL) {
        vres -= (BTM_MAX_PASSKEY_VAL + 1);
    }
#if SMP_DEBUG == TRUE
    p_prnt = (UINT8 *) &vres;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"cmac mod 2**32 mod 10**6", 4);
#endif

    SMP_TRACE_ERROR("Value for numeric comparison = %d", vres);
    return vres;
}

/*******************************************************************************
**
** Function         smp_calculate_f5
**
** Description      The function provides two AES-CMAC that are supposed to be used as
**                  - MacKey (MacKey is used in pairing DHKey check calculation);
**                  - LTK (LTK is used to ecrypt the link after completion of Phase 2
**                    and on reconnection, to derive BR/EDR LK).
**                  The function inputs are W, N1, N2, A1, A2.
**                  F5 rules:
**                  - the value used as key in MacKey/LTK (T) is calculated
**                    (function smp_calculate_f5_key(...));
**                    The formula is:
**                          T = AES-CMAC    (W)
**                                      salt
**                    where salt is internal parameter of smp_calculate_f5_key(...).
**                  - MacKey and LTK are calculated as AES-MAC values received with the
**                    key T calculated in the previous step and the plaintext message
**                    built from the external parameters N1, N2, A1, A2 and the internal
**                    parameters counter, keyID, length.
**                    The function smp_calculate_f5_mackey_or_long_term_key(...) is used in the
**                    calculations.
**                    The same formula is used in calculation of MacKey and LTK and the
**                    same parameter values except the value of the internal parameter
**                    counter:
**                    - in MacKey calculations the value is 0;
**                    - in LTK calculations the value is 1.
**                      MacKey  = AES-CMAC (Counter=0||keyID||N1||N2||A1||A2||Length=256)
**                                        T
**                      LTK     = AES-CMAC (Counter=1||keyID||N1||N2||A1||A2||Length=256)
**                                        T
**                  The parameters are
**                  input:
**                          W       is 256 bits,
**                          N1      is 128 bits,
**                          N2      is 128 bits,
**                          A1 is 56 bit,
**                          A2 is 56 bit.
**                  internal:
**                          Counter is 8 bits,  its value is 0 for MacKey,
**                                                          1 for LTK;
**                          KeyId   is 32 bits, its value is
**                                              0x62746c65 (MSB~LSB);
**                          Length  is 16 bits, its value is 0x0100
**                                              (MSB~LSB).
**                  output:
**                          MacKey  is 128 bits;
**                          LTK     is 128 bits
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
** Note             The LSB is the first octet, the MSB is the last octet of
**                  the AES-CMAC input/output stream.
**
*******************************************************************************/
BOOLEAN smp_calculate_f5(UINT8 *w, UINT8 *n1, UINT8 *n2, UINT8 *a1, UINT8 *a2,
                         UINT8 *mac_key, UINT8 *ltk)
{
    BT_OCTET16  t;    /* AES-CMAC output in smp_calculate_f5_key(...), key in */
    /* smp_calculate_f5_mackey_or_long_term_key(...) */
#if SMP_DEBUG == TRUE
    UINT8   *p_prnt = NULL;
#endif
    /* internal parameters: */

    /*
        counter is 0 for MacKey,
                is 1 for LTK
    */
    UINT8   counter_mac_key[1]  = {0};
    UINT8   counter_ltk[1]      = {1};
    /*
        keyID   62746c65
    */
    UINT8   key_id[4] = {0x65, 0x6c, 0x74, 0x62};
    /*
        length  0100
    */
    UINT8   length[2] = {0x00, 0x01};

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);
#if SMP_DEBUG == TRUE
    p_prnt = w;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"W", BT_OCTET32_LEN);
    p_prnt = n1;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"N1", BT_OCTET16_LEN);
    p_prnt = n2;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"N2", BT_OCTET16_LEN);
    p_prnt = a1;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"A1", 7);
    p_prnt = a2;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *) "A2", 7);
#endif

    if (!smp_calculate_f5_key(w, t)) {
        SMP_TRACE_ERROR("%s failed to calc T", __FUNCTION__);
        return FALSE;
    }
#if SMP_DEBUG == TRUE
    p_prnt = t;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"T", BT_OCTET16_LEN);
#endif

    if (!smp_calculate_f5_mackey_or_long_term_key(t, counter_mac_key, key_id, n1, n2, a1, a2,
            length, mac_key)) {
        SMP_TRACE_ERROR("%s failed to calc MacKey", __FUNCTION__);
        return FALSE;
    }
#if SMP_DEBUG == TRUE
    p_prnt = mac_key;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"MacKey", BT_OCTET16_LEN);
#endif

    if (!smp_calculate_f5_mackey_or_long_term_key(t, counter_ltk, key_id, n1, n2, a1, a2,
            length, ltk)) {
        SMP_TRACE_ERROR("%s failed to calc LTK", __FUNCTION__);
        return FALSE;
    }
#if SMP_DEBUG == TRUE
    p_prnt = ltk;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"LTK", BT_OCTET16_LEN);
#endif

    return TRUE;
}

/*******************************************************************************
**
** Function         smp_calculate_f5_mackey_or_long_term_key
**
** Description      The function calculates the value of MacKey or LTK by the rules
**                  defined for f5 function.
**                  At the moment exactly the same formula is used to calculate
**                  LTK and MacKey.
**                  The difference is the value of input parameter Counter:
**                  - in MacKey calculations the value is 0;
**                  - in LTK calculations the value is 1.
**                  The formula:
**                  mac = AES-CMAC (Counter||keyID||N1||N2||A1||A2||Length)
**                                T
**                  where
**                  input:      T       is 256 bits;
**                              Counter is 8 bits, its value is 0 for MacKey,
**                                                              1 for LTK;
**                              keyID   is 32 bits, its value is 0x62746c65;
**                              N1      is 128 bits;
**                              N2      is 128 bits;
**                              A1      is 56 bits;
**                              A2      is 56 bits;
**                              Length  is 16 bits, its value is 0x0100
**                  output:     LTK     is 128 bit.
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
** Note             The LSB is the first octet, the MSB is the last octet of
**                  the AES-CMAC input/output stream.
**
*******************************************************************************/
BOOLEAN smp_calculate_f5_mackey_or_long_term_key(UINT8 *t, UINT8 *counter,
        UINT8 *key_id, UINT8 *n1, UINT8 *n2, UINT8 *a1, UINT8 *a2,
        UINT8 *length, UINT8 *mac)
{
    UINT8   *p = NULL;
    UINT8   cmac[BT_OCTET16_LEN];
    UINT8   key[BT_OCTET16_LEN];
    UINT8   msg_len = 1 /* Counter size */ + 4 /* keyID size */ +
                      BT_OCTET16_LEN /* N1 size */ + BT_OCTET16_LEN /* N2 size */ +
                      7 /* A1 size*/ + 7 /* A2 size*/ + 2 /* Length size */;
    UINT8   msg[1 + 4 + BT_OCTET16_LEN + BT_OCTET16_LEN + 7 + 7 + 2];
    BOOLEAN ret = TRUE;
#if SMP_DEBUG == TRUE
    UINT8   *p_prnt = NULL;
#endif

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);
#if SMP_DEBUG == TRUE
    p_prnt = t;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"T", BT_OCTET16_LEN);
    p_prnt = counter;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"Counter", 1);
    p_prnt = key_id;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"KeyID", 4);
    p_prnt = n1;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"N1", BT_OCTET16_LEN);
    p_prnt = n2;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"N2", BT_OCTET16_LEN);
    p_prnt = a1;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"A1", 7);
    p_prnt = a2;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"A2", 7);
    p_prnt = length;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"Length", 2);
#endif

    p = key;
    ARRAY_TO_STREAM(p, t, BT_OCTET16_LEN);
#if SMP_DEBUG == TRUE
    p_prnt = key;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"K", BT_OCTET16_LEN);
#endif
    p = msg;
    ARRAY_TO_STREAM(p, length, 2);
    ARRAY_TO_STREAM(p, a2, 7);
    ARRAY_TO_STREAM(p, a1, 7);
    ARRAY_TO_STREAM(p, n2, BT_OCTET16_LEN);
    ARRAY_TO_STREAM(p, n1, BT_OCTET16_LEN);
    ARRAY_TO_STREAM(p, key_id, 4);
    ARRAY_TO_STREAM(p, counter, 1);
#if SMP_DEBUG == TRUE
    p_prnt = msg;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"M", msg_len);
#endif

    if (!aes_cipher_msg_auth_code(key, msg, msg_len, BT_OCTET16_LEN, cmac)) {
        SMP_TRACE_ERROR("%s failed", __FUNCTION__);
        ret = FALSE;
    }

#if SMP_DEBUG == TRUE
    p_prnt = cmac;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"AES-CMAC", BT_OCTET16_LEN);
#endif

    p = mac;
    ARRAY_TO_STREAM(p, cmac, BT_OCTET16_LEN);
    return ret;
}

/*******************************************************************************
**
** Function         smp_calculate_f5_key
**
** Description      The function calculates key T used in calculation of
**                  MacKey and LTK (f5 output is defined as MacKey || LTK).
**                  T = AES-CMAC    (W)
**                              salt
**                  where
**                  Internal:   salt    is 128 bit.
**                  input:      W       is 256 bit.
**                  Output:     T       is 128 bit.
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
** Note             The LSB is the first octet, the MSB is the last octet of
**                  the AES-CMAC input/output stream.
**
*******************************************************************************/
BOOLEAN smp_calculate_f5_key(UINT8 *w, UINT8 *t)
{
    UINT8 *p = NULL;
    /* Please see 2.2.7 LE Secure Connections Key Generation Function f5 */
    /*
        salt:   6C88 8391 AAF5 A538 6037 0BDB 5A60 83BE
    */
    BT_OCTET16  salt = {
        0xBE, 0x83, 0x60, 0x5A, 0xDB, 0x0B, 0x37, 0x60,
        0x38, 0xA5, 0xF5, 0xAA, 0x91, 0x83, 0x88, 0x6C
    };
#if SMP_DEBUG == TRUE
    UINT8   *p_prnt = NULL;
#endif

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);
#if SMP_DEBUG == TRUE
    p_prnt = salt;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"salt", BT_OCTET16_LEN);
    p_prnt = w;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"W", BT_OCTET32_LEN);
#endif

    BT_OCTET16 key;
    BT_OCTET32 msg;

    p = key;
    ARRAY_TO_STREAM(p, salt, BT_OCTET16_LEN);
    p = msg;
    ARRAY_TO_STREAM(p, w, BT_OCTET32_LEN);
#if SMP_DEBUG == TRUE
    p_prnt = key;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"K", BT_OCTET16_LEN);
    p_prnt = msg;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"M", BT_OCTET32_LEN);
#endif

    BT_OCTET16 cmac;
    BOOLEAN ret = TRUE;
    if (!aes_cipher_msg_auth_code(key, msg, BT_OCTET32_LEN, BT_OCTET16_LEN, cmac)) {
        SMP_TRACE_ERROR("%s failed", __FUNCTION__);
        ret = FALSE;
    }

#if SMP_DEBUG == TRUE
    p_prnt = cmac;
    smp_debug_print_nbyte_little_endian (p_prnt, (const UINT8 *)"AES-CMAC", BT_OCTET16_LEN);
#endif

    p = t;
    ARRAY_TO_STREAM(p, cmac, BT_OCTET16_LEN);
    return ret;
}

/*******************************************************************************
**
** Function         smp_calculate_local_dhkey_check
**
** Description      The function calculates and saves local device DHKey check
**                  value in CB.
**                  Before doing this it calls smp_calculate_f5_mackey_and_long_term_key(...).
**                  to calculate MacKey and LTK.
**                  MacKey is used in dhkey calculation.
**
** Returns          void
**
*******************************************************************************/
void smp_calculate_local_dhkey_check(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8   iocap[3], a[7], b[7];

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);

    smp_calculate_f5_mackey_and_long_term_key(p_cb);

    smp_collect_local_io_capabilities(iocap, p_cb);

    smp_collect_local_ble_address(a, p_cb);
    smp_collect_peer_ble_address(b, p_cb);
    smp_calculate_f6(p_cb->mac_key, p_cb->rand, p_cb->rrand, p_cb->peer_random, iocap, a, b,
                     p_cb->dhkey_check);

    SMP_TRACE_EVENT ("local DHKey check calculation is completed");
}

/*******************************************************************************
**
** Function         smp_calculate_peer_dhkey_check
**
** Description      The function calculates peer device DHKey check value.
**
** Returns          void
**
*******************************************************************************/
void smp_calculate_peer_dhkey_check(tSMP_CB *p_cb, tSMP_INT_DATA *p_data)
{
    UINT8       iocap[3], a[7], b[7];
    BT_OCTET16  param_buf;
    BOOLEAN     ret;
    tSMP_KEY    key;
    tSMP_STATUS status = SMP_PAIR_FAIL_UNKNOWN;

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);

    smp_collect_peer_io_capabilities(iocap, p_cb);

    smp_collect_local_ble_address(a, p_cb);
    smp_collect_peer_ble_address(b, p_cb);
    ret = smp_calculate_f6(p_cb->mac_key, p_cb->rrand, p_cb->rand, p_cb->local_random, iocap,
                           b, a, param_buf);

    if (ret) {
        SMP_TRACE_EVENT ("peer DHKey check calculation is completed");
#if (SMP_DEBUG == TRUE)
        smp_debug_print_nbyte_little_endian (param_buf, (const UINT8 *)"peer DHKey check",
                                             BT_OCTET16_LEN);
#endif
        key.key_type = SMP_KEY_TYPE_PEER_DHK_CHCK;
        key.p_data   = param_buf;
        smp_sm_event(p_cb, SMP_SC_KEY_READY_EVT, &key);
    } else {
        SMP_TRACE_EVENT ("peer DHKey check calculation failed");
        smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &status);
    }
}

/*******************************************************************************
**
** Function         smp_calculate_f6
**
** Description      The function calculates
**                  C = f6(W, N1, N2, R, IOcap, A1, A2) = AES-CMAC (N1||N2||R||IOcap||A1||A2)
**                                                                W
**                  where
**                  input:  W is 128 bit,
**                          N1 is 128 bit,
**                          N2 is 128 bit,
**                          R is 128 bit,
**                          IOcap is 24 bit,
**                          A1 is 56 bit,
**                          A2 is 56 bit,
**                  output: C is 128 bit.
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
** Note             The LSB is the first octet, the MSB is the last octet of
**                  the AES-CMAC input/output stream.
**
*******************************************************************************/
BOOLEAN smp_calculate_f6(UINT8 *w, UINT8 *n1, UINT8 *n2, UINT8 *r, UINT8 *iocap, UINT8 *a1,
                         UINT8 *a2, UINT8 *c)
{
    UINT8   *p = NULL;
    UINT8   msg_len = BT_OCTET16_LEN /* N1 size */ + BT_OCTET16_LEN /* N2 size */ +
                      BT_OCTET16_LEN /* R size */ + 3 /* IOcap size */ + 7 /* A1 size*/
                      + 7 /* A2 size*/;
    UINT8   msg[BT_OCTET16_LEN + BT_OCTET16_LEN + BT_OCTET16_LEN + 3 + 7 + 7];
#if SMP_DEBUG == TRUE
    UINT8   *p_print = NULL;
#endif

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);
#if SMP_DEBUG == TRUE
    p_print = w;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"W", BT_OCTET16_LEN);
    p_print = n1;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"N1", BT_OCTET16_LEN);
    p_print = n2;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"N2", BT_OCTET16_LEN);
    p_print = r;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"R", BT_OCTET16_LEN);
    p_print = iocap;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"IOcap", 3);
    p_print = a1;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"A1", 7);
    p_print = a2;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"A2", 7);
#endif

    UINT8 cmac[BT_OCTET16_LEN];
    UINT8 key[BT_OCTET16_LEN];

    p = key;
    ARRAY_TO_STREAM(p, w, BT_OCTET16_LEN);
#if SMP_DEBUG == TRUE
    p_print = key;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"K", BT_OCTET16_LEN);
#endif

    p = msg;
    ARRAY_TO_STREAM(p, a2, 7);
    ARRAY_TO_STREAM(p, a1, 7);
    ARRAY_TO_STREAM(p, iocap, 3);
    ARRAY_TO_STREAM(p, r, BT_OCTET16_LEN);
    ARRAY_TO_STREAM(p, n2, BT_OCTET16_LEN);
    ARRAY_TO_STREAM(p, n1, BT_OCTET16_LEN);
#if SMP_DEBUG == TRUE
    p_print = msg;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"M", msg_len);
#endif

    BOOLEAN ret = TRUE;
    if (!aes_cipher_msg_auth_code(key, msg, msg_len, BT_OCTET16_LEN, cmac)) {
        SMP_TRACE_ERROR("%s failed", __FUNCTION__);
        ret = FALSE;
    }

#if SMP_DEBUG == TRUE
    p_print = cmac;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"AES-CMAC", BT_OCTET16_LEN);
#endif

    p = c;
    ARRAY_TO_STREAM(p, cmac, BT_OCTET16_LEN);
    return ret;
}

/*******************************************************************************
**
** Function         smp_calculate_link_key_from_long_term_key
**
** Description      The function calculates and saves BR/EDR link key derived from
**                  LE SC LTK.
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
*******************************************************************************/
BOOLEAN smp_calculate_link_key_from_long_term_key(tSMP_CB *p_cb)
{
    tBTM_SEC_DEV_REC *p_dev_rec;
    BD_ADDR bda_for_lk;
    tBLE_ADDR_TYPE conn_addr_type;

    SMP_TRACE_DEBUG ("%s", __func__);

    if (p_cb->id_addr_rcvd && p_cb->id_addr_type == BLE_ADDR_PUBLIC) {
        SMP_TRACE_DEBUG ("Use rcvd identity address as BD_ADDR of LK rcvd identity address");
        memcpy(bda_for_lk, p_cb->id_addr, BD_ADDR_LEN);
    } else if ((BTM_ReadRemoteConnectionAddr(p_cb->pairing_bda, bda_for_lk, &conn_addr_type)) &&
               conn_addr_type == BLE_ADDR_PUBLIC) {
        SMP_TRACE_DEBUG ("Use rcvd connection address as BD_ADDR of LK");
    } else {
        SMP_TRACE_WARNING ("Don't have peer public address to associate with LK");
        return FALSE;
    }

    if ((p_dev_rec = btm_find_dev (p_cb->pairing_bda)) == NULL) {
        SMP_TRACE_ERROR("%s failed to find Security Record", __func__);
        return FALSE;
    }

    BT_OCTET16 intermediate_link_key;
    BOOLEAN ret = TRUE;

    ret = smp_calculate_h6(p_cb->ltk, (UINT8 *)"1pmt" /* reversed "tmp1" */, intermediate_link_key);
    if (!ret) {
        SMP_TRACE_ERROR("%s failed to derive intermediate_link_key", __func__);
        return ret;
    }

    BT_OCTET16 link_key;
    ret = smp_calculate_h6(intermediate_link_key, (UINT8 *) "rbel" /* reversed "lebr" */, link_key);
    if (!ret) {
        SMP_TRACE_ERROR("%s failed", __func__);
    } else {
        UINT8 link_key_type;
        if (btm_cb.security_mode == BTM_SEC_MODE_SC) {
            /* Secure Connections Only Mode */
            link_key_type = BTM_LKEY_TYPE_AUTH_COMB_P_256;
        } else if (controller_get_interface()->supports_secure_connections()) {
            /* both transports are SC capable */
            if (p_cb->sec_level == SMP_SEC_AUTHENTICATED) {
                link_key_type = BTM_LKEY_TYPE_AUTH_COMB_P_256;
            } else {
                link_key_type = BTM_LKEY_TYPE_UNAUTH_COMB_P_256;
            }
        } else if (btm_cb.security_mode == BTM_SEC_MODE_SP) {
            /* BR/EDR transport is SSP capable */
            if (p_cb->sec_level == SMP_SEC_AUTHENTICATED) {
                link_key_type = BTM_LKEY_TYPE_AUTH_COMB;
            } else {
                link_key_type = BTM_LKEY_TYPE_UNAUTH_COMB;
            }
        } else {
            SMP_TRACE_ERROR ("%s failed to update link_key. Sec Mode = %d, sm4 = 0x%02x",
                             __func__, btm_cb.security_mode, p_dev_rec->sm4);
            return FALSE;
        }

        link_key_type += BTM_LTK_DERIVED_LKEY_OFFSET;

        UINT8 *p;
        BT_OCTET16 notif_link_key;
        p = notif_link_key;
        ARRAY16_TO_STREAM(p, link_key);

        btm_sec_link_key_notification (bda_for_lk, notif_link_key, link_key_type);

        SMP_TRACE_EVENT ("%s is completed", __func__);
    }

    return ret;
}

/*******************************************************************************
**
** Function         smp_calculate_long_term_key_from_link_key
**
** Description      The function calculates and saves SC LTK derived from BR/EDR
**                  link key.
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
*******************************************************************************/
BOOLEAN smp_calculate_long_term_key_from_link_key(tSMP_CB *p_cb)
{
    BOOLEAN ret = TRUE;
    tBTM_SEC_DEV_REC *p_dev_rec;
    UINT8 rev_link_key[16];

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);

    if ((p_dev_rec = btm_find_dev (p_cb->pairing_bda)) == NULL) {
        SMP_TRACE_ERROR("%s failed to find Security Record", __FUNCTION__);
        return FALSE;
    }

    UINT8 br_link_key_type;
    if ((br_link_key_type = BTM_SecGetDeviceLinkKeyType (p_cb->pairing_bda))
            == BTM_LKEY_TYPE_IGNORE) {
        SMP_TRACE_ERROR("%s failed to retrieve BR link type", __FUNCTION__);
        return FALSE;
    }

    if ((br_link_key_type != BTM_LKEY_TYPE_AUTH_COMB_P_256) &&
            (br_link_key_type != BTM_LKEY_TYPE_UNAUTH_COMB_P_256)) {
        SMP_TRACE_ERROR("%s LE SC LTK can't be derived from LK %d",
                        __FUNCTION__, br_link_key_type);
        return FALSE;
    }

    UINT8 *p1;
    UINT8 *p2;
    p1 = rev_link_key;
    p2 = p_dev_rec->link_key;
    REVERSE_ARRAY_TO_STREAM(p1, p2, 16);

    BT_OCTET16 intermediate_long_term_key;
    /* "tmp2" obtained from the spec */
    ret = smp_calculate_h6(rev_link_key, (UINT8 *) "2pmt" /* reversed "tmp2" */,
                           intermediate_long_term_key);

    if (!ret) {
        SMP_TRACE_ERROR("%s failed to derive intermediate_long_term_key", __FUNCTION__);
        return ret;
    }

    /* "brle" obtained from the spec */
    ret = smp_calculate_h6(intermediate_long_term_key, (UINT8 *) "elrb" /* reversed "brle" */,
                           p_cb->ltk);

    if (!ret) {
        SMP_TRACE_ERROR("%s failed", __FUNCTION__);
    } else {
        p_cb->sec_level = (br_link_key_type == BTM_LKEY_TYPE_AUTH_COMB_P_256)
                          ? SMP_SEC_AUTHENTICATED : SMP_SEC_UNAUTHENTICATE;
        SMP_TRACE_EVENT ("%s is completed", __FUNCTION__);
    }

    return ret;
}

/*******************************************************************************
**
** Function         smp_calculate_h6
**
** Description      The function calculates
**                  C = h6(W, KeyID) = AES-CMAC (KeyID)
**                                             W
**                  where
**                  input:  W is 128 bit,
**                          KeyId is 32 bit,
**                  output: C is 128 bit.
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
** Note             The LSB is the first octet, the MSB is the last octet of
**                  the AES-CMAC input/output stream.
**
*******************************************************************************/
BOOLEAN smp_calculate_h6(UINT8 *w, UINT8 *keyid, UINT8 *c)
{
#if SMP_DEBUG == TRUE
    UINT8   *p_print = NULL;
#endif

    SMP_TRACE_DEBUG ("%s", __FUNCTION__);
#if SMP_DEBUG == TRUE
    p_print = w;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"W", BT_OCTET16_LEN);
    p_print = keyid;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"keyID", 4);
#endif

    UINT8 *p = NULL;
    UINT8 key[BT_OCTET16_LEN];

    p = key;
    ARRAY_TO_STREAM(p, w, BT_OCTET16_LEN);

#if SMP_DEBUG == TRUE
    p_print = key;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"K", BT_OCTET16_LEN);
#endif

    UINT8 msg_len = 4 /* KeyID size */;
    UINT8 msg[4];

    p = msg;
    ARRAY_TO_STREAM(p, keyid, 4);

#if SMP_DEBUG == TRUE
    p_print = msg;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *) "M", msg_len);
#endif

    BOOLEAN ret = TRUE;
    UINT8 cmac[BT_OCTET16_LEN];
    if (!aes_cipher_msg_auth_code(key, msg, msg_len, BT_OCTET16_LEN, cmac)) {
        SMP_TRACE_ERROR("%s failed", __FUNCTION__);
        ret = FALSE;
    }

#if SMP_DEBUG == TRUE
    p_print = cmac;
    smp_debug_print_nbyte_little_endian (p_print, (const UINT8 *)"AES-CMAC", BT_OCTET16_LEN);
#endif

    p = c;
    ARRAY_TO_STREAM(p, cmac, BT_OCTET16_LEN);
    return ret;
}

/*******************************************************************************
**
** Function         smp_start_nonce_generation
**
** Description      This function starts nonce generation.
**
** Returns          void
**
*******************************************************************************/
void smp_start_nonce_generation(tSMP_CB *p_cb)
{
    SMP_TRACE_DEBUG("%s", __FUNCTION__);
    p_cb->rand_enc_proc_state = SMP_GEN_NONCE_0_7;
    if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
        smp_rand_back(NULL);
    }
}

/*******************************************************************************
**
** Function         smp_finish_nonce_generation
**
** Description      This function finishes nonce generation.
**
** Returns          void
**
*******************************************************************************/
void smp_finish_nonce_generation(tSMP_CB *p_cb)
{
    SMP_TRACE_DEBUG("%s", __FUNCTION__);
    p_cb->rand_enc_proc_state = SMP_GEN_NONCE_8_15;
    if (!btsnd_hcic_ble_rand((void *)smp_rand_back)) {
        smp_rand_back(NULL);
    }
}

/*******************************************************************************
**
** Function         smp_process_new_nonce
**
** Description      This function notifies SM that it has new nonce.
**
** Returns          void
**
*******************************************************************************/
void smp_process_new_nonce(tSMP_CB *p_cb)
{
    SMP_TRACE_DEBUG ("%s round %d", __FUNCTION__, p_cb->round);
    smp_sm_event(p_cb, SMP_HAVE_LOC_NONCE_EVT, NULL);
}

/*******************************************************************************
**
** Function         smp_rand_back
**
** Description      This function is to process the rand command finished,
**                  process the random/encrypted number for further action.
**
** Returns          void
**
*******************************************************************************/
static void smp_rand_back(tBTM_RAND_ENC *p)
{
    tSMP_CB *p_cb = &smp_cb;
    UINT8   *pp = p->param_buf;
    UINT8   failure = SMP_PAIR_FAIL_UNKNOWN;
    UINT8   state = p_cb->rand_enc_proc_state & ~0x80;

    SMP_TRACE_DEBUG ("%s state=0x%x", __FUNCTION__, state);
    if (p && p->status == HCI_SUCCESS) {
        switch (state) {
        case SMP_GEN_SRAND_MRAND:
            memcpy((void *)p_cb->rand, p->param_buf, p->param_len);
            smp_generate_rand_cont(p_cb, NULL);
            break;

        case SMP_GEN_SRAND_MRAND_CONT:
            memcpy((void *)&p_cb->rand[8], p->param_buf, p->param_len);
            smp_generate_confirm(p_cb, NULL);
            break;

        case SMP_GEN_DIV_LTK:
            STREAM_TO_UINT16(p_cb->div, pp);
            smp_generate_ltk_cont(p_cb, NULL);
            break;

        case SMP_GEN_DIV_CSRK:
            STREAM_TO_UINT16(p_cb->div, pp);
            smp_compute_csrk(p_cb, NULL);
            break;

        case SMP_GEN_TK:
            smp_proc_passkey(p_cb, p);
            break;

        case SMP_GEN_RAND_V:
            memcpy(p_cb->enc_rand, p->param_buf, BT_OCTET8_LEN);
            smp_generate_y(p_cb, NULL);
            break;

        case SMP_GENERATE_PRIVATE_KEY_0_7:
        case SMP_GENERATE_PRIVATE_KEY_8_15:
        case SMP_GENERATE_PRIVATE_KEY_16_23:
        case SMP_GENERATE_PRIVATE_KEY_24_31:
            smp_continue_private_key_creation(p_cb, p);
            break;

        case SMP_GEN_NONCE_0_7:
            memcpy((void *)p_cb->rand, p->param_buf, p->param_len);
            smp_finish_nonce_generation(p_cb);
            break;

        case SMP_GEN_NONCE_8_15:
            memcpy((void *)&p_cb->rand[8], p->param_buf, p->param_len);
            smp_process_new_nonce(p_cb);
            break;
        }

        return;
    }

    SMP_TRACE_ERROR("%s key generation failed: (%d)", __FUNCTION__, p_cb->rand_enc_proc_state);
    smp_sm_event(p_cb, SMP_AUTH_CMPL_EVT, &failure);
}

#endif

