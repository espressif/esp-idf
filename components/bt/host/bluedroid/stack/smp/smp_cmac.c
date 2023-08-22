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
 *  This file contains the implementation of the AES128 CMAC algorithm.
 *
 ******************************************************************************/

#include "common/bt_target.h"
#include "osi/allocator.h"

#if SMP_INCLUDED == TRUE
//    #include <stdio.h>
#include <string.h>

#include "stack/btm_ble_api.h"
#include "smp_int.h"
#include "stack/hcimsgs.h"

typedef struct {
    UINT8               *text;
    UINT16              len;
    UINT16              round;
} tCMAC_CB;

tCMAC_CB    cmac_cb;

/* Rb for AES-128 as block cipher, LSB as [0] */
const BT_OCTET16 const_Rb = {
    0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void print128(BT_OCTET16 x, const UINT8 *key_name)
{
#if SMP_DEBUG == TRUE && SMP_DEBUG_VERBOSE == TRUE
    UINT8  *p = (UINT8 *)x;
    UINT8  i;

    SMP_TRACE_WARNING("%s(MSB ~ LSB) = ", key_name);

    for (i = 0; i < 4; i ++) {
        SMP_TRACE_WARNING("%02x %02x %02x %02x",
                          p[BT_OCTET16_LEN - i * 4 - 1], p[BT_OCTET16_LEN - i * 4 - 2],
                          p[BT_OCTET16_LEN - i * 4 - 3], p[BT_OCTET16_LEN - i * 4 - 4]);
    }
#endif
}

/*******************************************************************************
**
** Function         padding
**
** Description      utility function to padding the given text to be a 128 bits
**                  data. The parameter dest is input and output parameter, it
**                  must point to a BT_OCTET16_LEN memory space; where include
**                  length bytes valid data.
**
** Returns          void
**
*******************************************************************************/
static void padding ( BT_OCTET16 dest, UINT8 length )
{
    UINT8   i, *p = dest;
    /* original last block */
    for ( i = length ; i < BT_OCTET16_LEN; i++ ) {
        p[BT_OCTET16_LEN - i - 1] = ( i == length ) ? 0x80 : 0;
    }
}
/*******************************************************************************
**
** Function         leftshift_onebit
**
** Description      utility function to left shift one bit for a 128 bits value.
**
** Returns          void
**
*******************************************************************************/
static void leftshift_onebit(UINT8 *input, UINT8 *output)
{
    UINT8   i, overflow = 0 , next_overflow = 0;
    SMP_TRACE_EVENT ("leftshift_onebit ");
    /* input[0] is LSB */
    for ( i = 0; i < BT_OCTET16_LEN ; i ++ ) {
        next_overflow = (input[i] & 0x80) ? 1 : 0;
        output[i] = (input[i] << 1) | overflow;
        overflow = next_overflow;
    }
    return;
}
/*******************************************************************************
**
** Function         cmac_aes_cleanup
**
** Description      clean up function for AES_CMAC algorithm.
**
** Returns          void
**
*******************************************************************************/
static void cmac_aes_cleanup(void)
{
    if (cmac_cb.text != NULL) {
        osi_free(cmac_cb.text);
    }
    memset(&cmac_cb, 0, sizeof(tCMAC_CB));
}

/*******************************************************************************
**
** Function         cmac_aes_k_calculate
**
** Description      This function is the calculation of block cipher using AES-128.
**
** Returns          void
**
*******************************************************************************/
static BOOLEAN cmac_aes_k_calculate(BT_OCTET16 key, UINT8 *p_signature, UINT16 tlen)
{
    tSMP_ENC output;
    UINT16    i = 1, err = 0;
    UINT8    x[16] = {0};
    UINT8   *p_mac;

    SMP_TRACE_EVENT ("cmac_aes_k_calculate ");

    while (i <= cmac_cb.round) {
        smp_xor_128(&cmac_cb.text[(cmac_cb.round - i)*BT_OCTET16_LEN], x); /* Mi' := Mi (+) X  */

        if (!SMP_Encrypt(key, BT_OCTET16_LEN, &cmac_cb.text[(cmac_cb.round - i)*BT_OCTET16_LEN], BT_OCTET16_LEN, &output)) {
            err = 1;
            break;
        }

        memcpy(x, output.param_buf, BT_OCTET16_LEN);
        i ++;
    }

    if (!err) {
        p_mac = output.param_buf + (BT_OCTET16_LEN - tlen);
        memcpy(p_signature, p_mac, tlen);

        SMP_TRACE_DEBUG("tlen = %d p_mac = %p", tlen, p_mac);
        SMP_TRACE_DEBUG("p_mac[0] = 0x%02x p_mac[1] = 0x%02x p_mac[2] = 0x%02x p_mac[3] = 0x%02x",
                        *p_mac, *(p_mac + 1), *(p_mac + 2), *(p_mac + 3));
        SMP_TRACE_DEBUG("p_mac[4] = 0x%02x p_mac[5] = 0x%02x p_mac[6] = 0x%02x p_mac[7] = 0x%02x",
                        *(p_mac + 4), *(p_mac + 5), *(p_mac + 6), *(p_mac + 7));

        return TRUE;

    } else {
        return FALSE;
    }
}
/*******************************************************************************
**
** Function         cmac_prepare_last_block
**
** Description      This function proceeed to prepare the last block of message
**                  Mn depending on the size of the message.
**
** Returns          void
**
*******************************************************************************/
static void cmac_prepare_last_block (BT_OCTET16 k1, BT_OCTET16 k2)
{
//    UINT8       x[16] = {0};
    BOOLEAN      flag;

    SMP_TRACE_EVENT ("cmac_prepare_last_block ");
    /* last block is a complete block set flag to 1 */
    flag = ((cmac_cb.len % BT_OCTET16_LEN) == 0 && cmac_cb.len != 0)  ? TRUE : FALSE;

    SMP_TRACE_DEBUG("flag = %d round = %d", flag, cmac_cb.round);

    if ( flag ) {
        /* last block is complete block */
        smp_xor_128(&cmac_cb.text[0], k1);
    } else { /* padding then xor with k2 */
        padding(&cmac_cb.text[0], (UINT8)(cmac_cb.len % 16));

        smp_xor_128(&cmac_cb.text[0], k2);
    }
}
/*******************************************************************************
**
** Function         cmac_subkey_cont
**
** Description      This is the callback function when CIPHk(0[128]) is completed.
**
** Returns          void
**
*******************************************************************************/
static void cmac_subkey_cont(tSMP_ENC *p)
{
    UINT8 k1[BT_OCTET16_LEN], k2[BT_OCTET16_LEN];
    UINT8 *pp = p->param_buf;
    SMP_TRACE_EVENT ("cmac_subkey_cont ");
    print128(pp, (const UINT8 *)"K1 before shift");

    /* If MSB(L) = 0, then K1 = L << 1 */
    if ( (pp[BT_OCTET16_LEN - 1] & 0x80) != 0 ) {
        /* Else K1 = ( L << 1 ) (+) Rb */
        leftshift_onebit(pp, k1);
        smp_xor_128(k1, const_Rb);
    } else {
        leftshift_onebit(pp, k1);
    }

    if ( (k1[BT_OCTET16_LEN - 1] & 0x80) != 0 ) {
        /* K2 =  (K1 << 1) (+) Rb */
        leftshift_onebit(k1, k2);
        smp_xor_128(k2, const_Rb);
    } else {
        /* If MSB(K1) = 0, then K2 = K1 << 1 */
        leftshift_onebit(k1, k2);
    }

    print128(k1, (const UINT8 *)"K1");
    print128(k2, (const UINT8 *)"K2");

    cmac_prepare_last_block (k1, k2);
}
/*******************************************************************************
**
** Function         cmac_generate_subkey
**
** Description      This is the function to generate the two subkeys.
**
** Parameters       key - CMAC key, expect SRK when used by SMP.
**
** Returns          void
**
*******************************************************************************/
static BOOLEAN cmac_generate_subkey(BT_OCTET16 key)
{
    BT_OCTET16 z = {0};
    BOOLEAN     ret = TRUE;
    tSMP_ENC output;
    SMP_TRACE_EVENT (" cmac_generate_subkey");

    if (SMP_Encrypt(key, BT_OCTET16_LEN, z, BT_OCTET16_LEN, &output)) {
        cmac_subkey_cont(&output);;
    } else {
        ret = FALSE;
    }

    return ret;
}
/*******************************************************************************
**
** Function         aes_cipher_msg_auth_code
**
** Description      This is the AES-CMAC Generation Function with tlen implemented.
**
** Parameters       key - CMAC key in little endian order, expect SRK when used by SMP.
**                  input - text to be signed in little endian byte order.
**                  length - length of the input in byte.
**                  tlen - lenth of mac desired
**                  p_signature - data pointer to where signed data to be stored, tlen long.
**
** Returns          FALSE if out of resources, TRUE in other cases.
**
*******************************************************************************/
BOOLEAN aes_cipher_msg_auth_code(BT_OCTET16 key, UINT8 *input, UINT16 length,
                                 UINT16 tlen, UINT8 *p_signature)
{
    UINT16  len, diff;
    UINT16  n = (length + BT_OCTET16_LEN - 1) / BT_OCTET16_LEN;       /* n is number of rounds */
    BOOLEAN ret = FALSE;

    SMP_TRACE_EVENT ("%s", __func__);

    if (n == 0) {
        n = 1;
    }
    len = n * BT_OCTET16_LEN;

    SMP_TRACE_DEBUG("AES128_CMAC started, allocate buffer size = %d", len);
    /* allocate a memory space of multiple of 16 bytes to hold text  */
    if ((cmac_cb.text = (UINT8 *)osi_malloc(len)) != NULL) {
        cmac_cb.round = n;

        memset(cmac_cb.text, 0, len);
        diff = len - length;

        if (input != NULL && length > 0) {
            memcpy(&cmac_cb.text[diff] , input, (int)length);
            cmac_cb.len = length;
        } else {
            cmac_cb.len = 0;
        }

        /* prepare calculation for subkey s and last block of data */
        if (cmac_generate_subkey(key)) {
            /* start calculation */
            ret = cmac_aes_k_calculate(key, p_signature, tlen);
        }
        /* clean up */
        cmac_aes_cleanup();
    } else {
        ret = FALSE;
        SMP_TRACE_ERROR("No resources");
    }

    return ret;
}

#if 0 /* testing code, sample data from spec */
void test_cmac_cback(UINT8 *p_mac, UINT16 tlen)
{
    SMP_TRACE_EVENT ("test_cmac_cback ");
    SMP_TRACE_ERROR("test_cmac_cback");
}

void test_cmac(void)
{
    SMP_TRACE_EVENT ("test_cmac ");
    UINT8 M[64] = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
        0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
        0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
        0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
        0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
        0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
        0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
    };

    UINT8 key[16] = {
        0x3c, 0x4f, 0xcf, 0x09, 0x88, 0x15, 0xf7, 0xab,
        0xa6, 0xd2, 0xae, 0x28, 0x16, 0x15, 0x7e, 0x2b
    };
    UINT8 i = 0, tmp;
    UINT16 len;

    len = 64;

    for (i = 0; i < len / 2; i ++) {
        tmp = M[i];
        M[i] = M[len - 1 - i];
        M[len - 1 - i] = tmp;
    }


    memset(&cmac_cb, 0, sizeof(tCMAC_CB));

    SMP_TRACE_WARNING("\n Example 1: len = %d\n", len);

    aes_cipher_msg_auth_code(key, M, len, 128, test_cmac_cback, 0);

}
#endif
#endif
