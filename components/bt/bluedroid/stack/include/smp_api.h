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
 *  This file contains the SMP API function external definitions.
 *
 ******************************************************************************/
#ifndef SMP_API_H
#define SMP_API_H

#include "bt_target.h"

#define SMP_PIN_CODE_LEN_MAX    PIN_CODE_LEN
#define SMP_PIN_CODE_LEN_MIN    6

#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
/* SMP command code */
#define SMP_OPCODE_PAIRING_REQ            0x01
#define SMP_OPCODE_PAIRING_RSP            0x02
#define SMP_OPCODE_CONFIRM                0x03
#define SMP_OPCODE_RAND                   0x04
#define SMP_OPCODE_PAIRING_FAILED         0x05
#define SMP_OPCODE_ENCRYPT_INFO           0x06
#define SMP_OPCODE_MASTER_ID              0x07
#define SMP_OPCODE_IDENTITY_INFO          0x08
#define SMP_OPCODE_ID_ADDR                0x09
#define SMP_OPCODE_SIGN_INFO              0x0A
#define SMP_OPCODE_SEC_REQ                0x0B
#define SMP_OPCODE_PAIR_PUBLIC_KEY        0x0C
#define SMP_OPCODE_PAIR_DHKEY_CHECK       0x0D
#define SMP_OPCODE_PAIR_KEYPR_NOTIF       0x0E
#define SMP_OPCODE_MAX                    SMP_OPCODE_PAIR_KEYPR_NOTIF
#define SMP_OPCODE_MIN                    SMP_OPCODE_PAIRING_REQ
#define SMP_OPCODE_PAIR_COMMITM           0x0F
#endif

/* SMP event type */
#define SMP_IO_CAP_REQ_EVT      1       /* IO capability request event */
#define SMP_SEC_REQUEST_EVT     2       /* SMP pairing request */
#define SMP_PASSKEY_NOTIF_EVT   3       /* passkey notification event */
#define SMP_PASSKEY_REQ_EVT     4       /* passkey request event */
#define SMP_OOB_REQ_EVT         5       /* OOB request event */
#define SMP_NC_REQ_EVT          6       /* Numeric Comparison request event */
#define SMP_COMPLT_EVT          7       /* SMP complete event */
#define SMP_PEER_KEYPR_NOT_EVT  8       /* Peer keypress notification received event */
#define SMP_SC_OOB_REQ_EVT      9       /* SC OOB request event (both local and peer OOB data */
/* can be expected in response) */
#define SMP_SC_LOC_OOB_DATA_UP_EVT  10  /* SC OOB local data set is created */
/* (as result of SMP_CrLocScOobData(...)) */
#define SMP_BR_KEYS_REQ_EVT     12      /* SMP over BR keys request event */
typedef UINT8   tSMP_EVT;


/* pairing failure reason code */
#define SMP_PASSKEY_ENTRY_FAIL      0x01
#define SMP_OOB_FAIL                0x02
#define SMP_PAIR_AUTH_FAIL          0x03
#define SMP_CONFIRM_VALUE_ERR       0x04
#define SMP_PAIR_NOT_SUPPORT        0x05
#define SMP_ENC_KEY_SIZE            0x06
#define SMP_INVALID_CMD             0x07
#define SMP_PAIR_FAIL_UNKNOWN       0x08
#define SMP_REPEATED_ATTEMPTS       0x09
#define SMP_INVALID_PARAMETERS      0x0A
#define SMP_DHKEY_CHK_FAIL          0x0B
#define SMP_NUMERIC_COMPAR_FAIL     0x0C
#define SMP_BR_PARING_IN_PROGR      0x0D
#define SMP_XTRANS_DERIVE_NOT_ALLOW 0x0E
#define SMP_MAX_FAIL_RSN_PER_SPEC   SMP_XTRANS_DERIVE_NOT_ALLOW

/* self defined error code */
#define SMP_PAIR_INTERNAL_ERR       (SMP_MAX_FAIL_RSN_PER_SPEC + 0x01) /* 0x0E */

/* 0x0F unknown IO capability, unable to decide association model */
#define SMP_UNKNOWN_IO_CAP          (SMP_MAX_FAIL_RSN_PER_SPEC + 0x02) /* 0x0F */

#define SMP_INIT_FAIL               (SMP_MAX_FAIL_RSN_PER_SPEC + 0x03) /* 0x10 */
#define SMP_CONFIRM_FAIL            (SMP_MAX_FAIL_RSN_PER_SPEC + 0x04) /* 0x11 */
#define SMP_BUSY                    (SMP_MAX_FAIL_RSN_PER_SPEC + 0x05) /* 0x12 */
#define SMP_ENC_FAIL                (SMP_MAX_FAIL_RSN_PER_SPEC + 0x06) /* 0x13 */
#define SMP_STARTED                 (SMP_MAX_FAIL_RSN_PER_SPEC + 0x07) /* 0x14 */
#define SMP_RSP_TIMEOUT             (SMP_MAX_FAIL_RSN_PER_SPEC + 0x08) /* 0x15 */
#define SMP_DIV_NOT_AVAIL           (SMP_MAX_FAIL_RSN_PER_SPEC + 0x09) /* 0x16 */

/* 0x17 unspecified failed reason */
#define SMP_FAIL                    (SMP_MAX_FAIL_RSN_PER_SPEC + 0x0A) /* 0x17 */

#define SMP_CONN_TOUT               (SMP_MAX_FAIL_RSN_PER_SPEC + 0x0B)
#define SMP_SUCCESS                 0

typedef UINT8 tSMP_STATUS;


/* Device IO capability */
#define SMP_IO_CAP_OUT      BTM_IO_CAP_OUT   /* DisplayOnly */
#define SMP_IO_CAP_IO       BTM_IO_CAP_IO   /* DisplayYesNo */
#define SMP_IO_CAP_IN       BTM_IO_CAP_IN   /* KeyboardOnly */
#define SMP_IO_CAP_NONE     BTM_IO_CAP_NONE   /* NoInputNoOutput */
#define SMP_IO_CAP_KBDISP   BTM_IO_CAP_KBDISP   /* Keyboard Display */
#define SMP_IO_CAP_MAX      BTM_IO_CAP_MAX
typedef UINT8  tSMP_IO_CAP;

#ifndef SMP_DEFAULT_IO_CAPS
#define SMP_DEFAULT_IO_CAPS     SMP_IO_CAP_KBDISP
#endif

/* OOB data present or not */
enum {
    SMP_OOB_NONE,
    SMP_OOB_PRESENT,
    SMP_OOB_UNKNOWN
};
typedef UINT8  tSMP_OOB_FLAG;

/* type of OOB data required from application */
enum {
    SMP_OOB_INVALID_TYPE,
    SMP_OOB_PEER,
    SMP_OOB_LOCAL,
    SMP_OOB_BOTH
};
typedef UINT8   tSMP_OOB_DATA_TYPE;

#define SMP_AUTH_NO_BOND        0x00
#define SMP_AUTH_GEN_BOND       0x01 //todo sdh change GEN_BOND to BOND

/* SMP Authentication requirement */
#define SMP_AUTH_YN_BIT         (1 << 2)
#define SMP_SC_SUPPORT_BIT      (1 << 3)
#define SMP_KP_SUPPORT_BIT      (1 << 4)

#define SMP_AUTH_MASK    (SMP_AUTH_GEN_BOND|SMP_AUTH_YN_BIT|SMP_SC_SUPPORT_BIT|SMP_KP_SUPPORT_BIT)

#define SMP_AUTH_BOND           SMP_AUTH_GEN_BOND

/* no MITM, No Bonding, encryption only */
#define SMP_AUTH_NB_ENC_ONLY    0x00 //(SMP_AUTH_MASK | BTM_AUTH_SP_NO)

/* MITM, No Bonding, Use IO Capability to determine authentication procedure */
#define SMP_AUTH_NB_IOCAP       (SMP_AUTH_NO_BOND | SMP_AUTH_YN_BIT)

/* No MITM, General Bonding, Encryption only */
#define SMP_AUTH_GB_ENC_ONLY    (SMP_AUTH_GEN_BOND )

/* MITM, General Bonding, Use IO Capability to determine authentication procedure */
#define SMP_AUTH_GB_IOCAP       (SMP_AUTH_GEN_BOND | SMP_AUTH_YN_BIT)

/* Secure Connections, no MITM, no Bonding */
#define SMP_AUTH_SC_ENC_ONLY    (SMP_SC_SUPPORT_BIT)

/* Secure Connections, no MITM, Bonding */
#define SMP_AUTH_SC_GB          (SMP_SC_SUPPORT_BIT | SMP_AUTH_GEN_BOND)

/* Secure Connections, MITM, no Bonding */
#define SMP_AUTH_SC_MITM_NB     (SMP_SC_SUPPORT_BIT | SMP_AUTH_YN_BIT | SMP_AUTH_NO_BOND)

/* Secure Connections, MITM, Bonding */
#define SMP_AUTH_SC_MITM_GB     (SMP_SC_SUPPORT_BIT | SMP_AUTH_YN_BIT | SMP_AUTH_GEN_BOND)

/* All AuthReq RFU bits are set to 1 - NOTE: reserved bit in Bonding_Flags is not set */
#define SMP_AUTH_ALL_RFU_SET    0xF8

typedef UINT8 tSMP_AUTH_REQ;

#define SMP_SEC_NONE                 0
#define SMP_SEC_UNAUTHENTICATE      (1 << 0)
#define SMP_SEC_AUTHENTICATED       (1 << 2)
typedef UINT8 tSMP_SEC_LEVEL;

/* Maximum Encryption Key Size range */
#define SMP_ENCR_KEY_SIZE_MIN       7
#define SMP_ENCR_KEY_SIZE_MAX       16

/* SMP key types */
#define SMP_SEC_KEY_TYPE_ENC                (1 << 0)    /* encryption key */
#define SMP_SEC_KEY_TYPE_ID                 (1 << 1)    /* identity key */
#define SMP_SEC_KEY_TYPE_CSRK               (1 << 2)    /* slave CSRK */
#define SMP_SEC_KEY_TYPE_LK                 (1 << 3)    /* BR/EDR link key */
typedef UINT8 tSMP_KEYS;

#define SMP_BR_SEC_DEFAULT_KEY   (SMP_SEC_KEY_TYPE_ENC | SMP_SEC_KEY_TYPE_ID | \
                                  SMP_SEC_KEY_TYPE_CSRK)

/* default security key distribution value */
#define SMP_SEC_DEFAULT_KEY      (SMP_SEC_KEY_TYPE_ENC | SMP_SEC_KEY_TYPE_ID | \
                                  SMP_SEC_KEY_TYPE_CSRK | SMP_SEC_KEY_TYPE_LK)

#define SMP_SC_KEY_STARTED      0   /* passkey entry started */
#define SMP_SC_KEY_ENTERED      1   /* passkey digit entered */
#define SMP_SC_KEY_ERASED       2   /* passkey digit erased */
#define SMP_SC_KEY_CLEARED      3   /* passkey cleared */
#define SMP_SC_KEY_COMPLT       4   /* passkey entry completed */
#define SMP_SC_KEY_OUT_OF_RANGE 5   /* out of range */
typedef UINT8 tSMP_SC_KEY_TYPE;

/* data type for BTM_SP_IO_REQ_EVT */
typedef struct {
    tSMP_IO_CAP     io_cap;         /* local IO capabilities */
    tSMP_OOB_FLAG   oob_data;       /* OOB data present (locally) for the peer device */
    tSMP_AUTH_REQ   auth_req;       /* Authentication required (for local device) */
    UINT8           max_key_size;   /* max encryption key size */
    tSMP_KEYS       init_keys;      /* initiator keys to be distributed */
    tSMP_KEYS       resp_keys;      /* responder keys */
} tSMP_IO_REQ;

typedef struct {
    tSMP_STATUS reason;
    tSMP_SEC_LEVEL sec_level;
    BOOLEAN is_pair_cancel;
    BOOLEAN smp_over_br;
} tSMP_CMPL;

typedef struct {
    BT_OCTET32  x;
    BT_OCTET32  y;
} tSMP_PUBLIC_KEY;

/* the data associated with the info sent to the peer via OOB interface */
typedef struct {
    BOOLEAN         present;
    BT_OCTET16      randomizer;
    BT_OCTET16      commitment;

    tBLE_BD_ADDR    addr_sent_to;
    BT_OCTET32      private_key_used;   /* is used to calculate: */
    /* publ_key_used = P-256(private_key_used, curve_p256.G) - send it to the */
    /* other side */
    /* dhkey = P-256(private_key_used, publ key rcvd from the other side) */
    tSMP_PUBLIC_KEY publ_key_used; /* P-256(private_key_used, curve_p256.G) */
} tSMP_LOC_OOB_DATA;

/* the data associated with the info received from the peer via OOB interface */
typedef struct {
    BOOLEAN         present;
    BT_OCTET16      randomizer;
    BT_OCTET16      commitment;
    tBLE_BD_ADDR    addr_rcvd_from;
} tSMP_PEER_OOB_DATA;

typedef struct {
    tSMP_LOC_OOB_DATA   loc_oob_data;
    tSMP_PEER_OOB_DATA  peer_oob_data;
} tSMP_SC_OOB_DATA;


typedef union {
    UINT32          passkey;
    tSMP_IO_REQ     io_req;     /* IO request */
    tSMP_CMPL       cmplt;
    tSMP_OOB_DATA_TYPE  req_oob_type;
    tSMP_LOC_OOB_DATA   loc_oob_data;
} tSMP_EVT_DATA;


/* AES Encryption output */
typedef struct {
    UINT8   status;
    UINT8   param_len;
    UINT16  opcode;
    UINT8   param_buf[BT_OCTET16_LEN];
} tSMP_ENC;

/* Security Manager events - Called by the stack when Security Manager related events occur.*/
typedef UINT8 (tSMP_CALLBACK) (tSMP_EVT event, BD_ADDR bd_addr, tSMP_EVT_DATA *p_data);

/* callback function for CMAC algorithm
*/
typedef void (tCMAC_CMPL_CBACK)(UINT8 *p_mac, UINT16 tlen, UINT32 sign_counter);

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/* API of SMP */

/*******************************************************************************
**
** Function         SMP_Init
**
** Description      This function initializes the SMP unit.
**
** Returns          void
**
*******************************************************************************/
extern void SMP_Init(void);

/*******************************************************************************
**
** Function         SMP_SetTraceLevel
**
** Description      This function sets the trace level for SMP.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
extern UINT8 SMP_SetTraceLevel (UINT8 new_level);

/*******************************************************************************
**
** Function         SMP_Register
**
** Description      This function register for the SMP service callback.
**
** Returns          void
**
*******************************************************************************/
extern BOOLEAN SMP_Register (tSMP_CALLBACK *p_cback);

/*******************************************************************************
**
** Function         SMP_Pair
**
** Description      This function is called to start a SMP pairing.
**
** Returns          SMP_STARTED if bond started, else otherwise exception.
**
*******************************************************************************/
extern tSMP_STATUS SMP_Pair (BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         SMP_BR_PairWith
**
** Description      This function is called to start a SMP pairing over BR/EDR.
**
** Returns          SMP_STARTED if pairing started, otherwise reason for failure.
**
*******************************************************************************/
extern tSMP_STATUS SMP_BR_PairWith (BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         SMP_PairCancel
**
** Description      This function is called to cancel a SMP pairing.
**
** Returns          TRUE - pairing cancelled
**
*******************************************************************************/
extern  BOOLEAN SMP_PairCancel (BD_ADDR bd_addr);

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
extern void SMP_SecurityGrant(BD_ADDR bd_addr, UINT8 res);

/*******************************************************************************
**
** Function         SMP_PasskeyReply
**
** Description      This function is called after Security Manager submitted
**                  Passkey request to the application.
**
** Parameters:      bd_addr      - Address of the device for which PIN was requested
**                  res          - result of the operation SMP_SUCCESS if success
**                  passkey      - numeric value in the range of
**                  BTM_MIN_PASSKEY_VAL(0) - BTM_MAX_PASSKEY_VAL(999999(0xF423F)).
**
*******************************************************************************/
extern void SMP_PasskeyReply (BD_ADDR bd_addr, UINT8 res, UINT32 passkey);

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
extern void SMP_ConfirmReply (BD_ADDR bd_addr, UINT8 res);

/*******************************************************************************
**
** Function         SMP_OobDataReply
**
** Description      This function is called to provide the OOB data for
**                  SMP in response to SMP_OOB_REQ_EVT
**
** Parameters:      bd_addr     - Address of the peer device
**                  res         - result of the operation SMP_SUCCESS if success
**                  p_data      - SM Randomizer  C.
**
*******************************************************************************/
extern void SMP_OobDataReply(BD_ADDR bd_addr, tSMP_STATUS res, UINT8 len,
                             UINT8 *p_data);

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
extern void SMP_SecureConnectionOobDataReply(UINT8 *p_data);

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
**                  p_out               - pointer to the encrypted outputs
**
**  Returns         Boolean - TRUE: encryption is successful
*******************************************************************************/
extern BOOLEAN SMP_Encrypt (UINT8 *key, UINT8 key_len,
                            UINT8 *plain_text, UINT8 pt_len,
                            tSMP_ENC *p_out);

/*******************************************************************************
**
** Function         SMP_KeypressNotification
**
** Description      This function is called to notify SM about Keypress Notification.
**
** Parameters:      bd_addr      - Address of the device to send keypress
**                                 notification to
**                  value        - keypress notification parameter value
**
*******************************************************************************/
extern void SMP_KeypressNotification (BD_ADDR bd_addr, UINT8 value);

/*******************************************************************************
**
** Function         SMP_CreateLocalSecureConnectionsOobData
**
** Description      This function is called to start creation of local SC OOB
**                  data set (tSMP_LOC_OOB_DATA).
**
** Parameters:      bd_addr      - Address of the device to send OOB data block
**                                 to.
**
**  Returns         Boolean - TRUE: creation of local SC OOB data set started.
*******************************************************************************/
extern BOOLEAN SMP_CreateLocalSecureConnectionsOobData (
    tBLE_BD_ADDR *addr_to_send_to);

#ifdef __cplusplus
}
#endif
#endif /* SMP_API_H */
