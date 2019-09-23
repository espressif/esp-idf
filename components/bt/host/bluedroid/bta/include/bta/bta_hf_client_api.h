/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
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

/******************************************************************************
 *
 *  This is the public interface file for the handsfree (HF role) subsystem
 *
 ******************************************************************************/
#ifndef BTA_HF_CLIENT_API_H
#define BTA_HF_CLIENT_API_H

#include "bta_api.h"
#include "bta_hfp_defs.h"

#if (BTA_HF_INCLUDED == TRUE)
/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* HFP peer (AG) features*/
#define BTA_HF_CLIENT_PEER_FEAT_3WAY   0x00000001  /* Three-way calling */
#define BTA_HF_CLIENT_PEER_FEAT_ECNR   0x00000002  /* Echo cancellation and/or noise reduction */
#define BTA_HF_CLIENT_PEER_FEAT_VREC   0x00000004  /* Voice recognition */
#define BTA_HF_CLIENT_PEER_INBAND      0x00000008  /* In-band ring tone */
#define BTA_HF_CLIENT_PEER_VTAG        0x00000010  /* Attach a phone number to a voice tag */
#define BTA_HF_CLIENT_PEER_REJECT      0x00000020  /* Ability to reject incoming call */
#define BTA_HF_CLIENT_PEER_ECS         0x00000040  /* Enhanced Call Status */
#define BTA_HF_CLIENT_PEER_ECC         0x00000080  /* Enhanced Call Control */
#define BTA_HF_CLIENT_PEER_EXTERR      0x00000100  /* Extended error codes */
#define BTA_HF_CLIENT_PEER_CODEC       0x00000200  /* Codec Negotiation */

typedef UINT16 tBTA_HF_CLIENT_PEER_FEAT;

/* HFP HF features */
#define BTA_HF_CLIENT_FEAT_ECNR        0x00000001  /* Echo cancellation and/or noise reduction */
#define BTA_HF_CLIENT_FEAT_3WAY        0x00000002  /* Call waiting and three-way calling */
#define BTA_HF_CLIENT_FEAT_CLI         0x00000004  /* Caller ID presentation capability */
#define BTA_HF_CLIENT_FEAT_VREC        0x00000008  /* Voice recognition activation */
#define BTA_HF_CLIENT_FEAT_VOL         0x00000010  /* Remote volume control */
#define BTA_HF_CLIENT_FEAT_ECS         0x00000020  /* Enhanced Call Status */
#define BTA_HF_CLIENT_FEAT_ECC         0x00000040  /* Enhanced Call Control */
#define BTA_HF_CLIENT_FEAT_CODEC       0x00000080  /* Codec Negotiation */

/* HFP HF extended call handling - masks not related to any spec */
#define BTA_HF_CLIENT_CHLD_REL          0x00000001  /* 0  Release waiting call or held calls */
#define BTA_HF_CLIENT_CHLD_REL_ACC      0x00000002  /* 1  Release active calls and accept other (waiting or held) cal */
#define BTA_HF_CLIENT_CHLD_REL_X        0x00000004  /* 1x Release x call*/
#define BTA_HF_CLIENT_CHLD_HOLD_ACC     0x00000008  /* 2  Active calls on hold and accept other call */
#define BTA_HF_CLIENT_CHLD_PRIV_X       0x00000010  /* 2x Active multiparty call on hold except call x */
#define BTA_HF_CLIENT_CHLD_MERGE        0x00000020  /* 3  Add held call to multiparty */
#define BTA_HF_CLIENT_CHLD_MERGE_DETACH 0x00000040  /* 4  Add held call to multiparty */

typedef UINT16 tBTA_HF_CLIENT_CHLD_FEAT;

/* HFP AG errors ot OK sent to HF Unit */
#define BTA_HF_CLIENT_AT_RESULT_OK            0
#define BTA_HF_CLIENT_AT_RESULT_ERROR         1
#define BTA_HF_CLIENT_AT_RESULT_NO_CARRIER    2
#define BTA_HF_CLIENT_AT_RESULT_BUSY          3
#define BTA_HF_CLIENT_AT_RESULT_NO_ANSWER     4
#define BTA_HF_CLIENT_AT_RESULT_DELAY         5
#define BTA_HF_CLIENT_AT_RESULT_BLACKLISTED   6
#define BTA_HF_CLIENT_AT_RESULT_CME           7

typedef UINT8 tBTA_HF_CLIENT_AT_RESULT_TYPE;

/* HF Client callback events */
#define BTA_HF_CLIENT_ENABLE_EVT            0  /* HF Client enabled */
#define BTA_HF_CLIENT_REGISTER_EVT          1  /* HF Client registered */
#define BTA_HF_CLIENT_OPEN_EVT              2  /* HF Client connection open */
#define BTA_HF_CLIENT_CLOSE_EVT             3  /* HF Client connection closed */
#define BTA_HF_CLIENT_CONN_EVT              4  /* Service level connection opened */
#define BTA_HF_CLIENT_AUDIO_OPEN_EVT        5  /* Audio connection open */
#define BTA_HF_CLIENT_AUDIO_MSBC_OPEN_EVT   6  /* Audio connection with mSBC codec open */
#define BTA_HF_CLIENT_AUDIO_CLOSE_EVT       7  /* Audio connection closed */
#define BTA_HF_CLIENT_SPK_EVT               8  /* Speaker volume changed */
#define BTA_HF_CLIENT_MIC_EVT               9  /* Microphone volume changed */
#define BTA_HF_CLIENT_IND_EVT               10 /* Indicator */
#define BTA_HF_CLIENT_VOICE_REC_EVT         11 /* AG changed voice recognition setting */
#define BTA_HF_CLIENT_OPERATOR_NAME_EVT     12 /* Operator name acquired */
#define BTA_HF_CLIENT_CLIP_EVT              13 /* Calling line identification event */
#define BTA_HF_CLIENT_CCWA_EVT              14 /* Call waiting notification */
#define BTA_HF_CLIENT_AT_RESULT_EVT         15 /* Call waiting notification */
#define BTA_HF_CLIENT_CLCC_EVT              16 /* current call event */
#define BTA_HF_CLIENT_CNUM_EVT              17 /* subscriber information event */
#define BTA_HF_CLIENT_BTRH_EVT              18 /* bluetooth response and hold event */
#define BTA_HF_CLIENT_BSIR_EVT              19 /* in-band ring tone setting changed event */
#define BTA_HF_CLIENT_BINP_EVT              20 /* binp number event */
#define BTA_HF_CLIENT_RING_INDICATION       21 /* HF Client ring indication */
#define BTA_HF_CLIENT_DISABLE_EVT           30 /* HF Client disabled */

typedef UINT8 tBTA_HF_CLIENT_EVT;

/* HF Client open status */
#define BTA_HF_CLIENT_SUCCESS          0 /* Connection successfully opened */
#define BTA_HF_CLIENT_FAIL_SDP         1 /* Open failed due to SDP */
#define BTA_HF_CLIENT_FAIL_RFCOMM      2 /* Open failed due to RFCOMM */
#define BTA_HF_CLIENT_FAIL_RESOURCES   3 /* out of resources failure  */

typedef UINT8 tBTA_HF_CLIENT_STATUS;

/* indicator constants HFP 1.1 and later */
#define BTA_HF_CLIENT_IND_CALL             0   /* position of call indicator */
#define BTA_HF_CLIENT_IND_CALLSETUP        1   /* position of callsetup indicator */
#define BTA_HF_CLIENT_IND_SERVICE          2   /* position of service indicator */
/* indicator constants HFP 1.5 and later */
#define BTA_HF_CLIENT_IND_SIGNAL           3   /* position of signal strength indicator */
#define BTA_HF_CLIENT_IND_ROAM             4   /* position of roaming indicator */
#define BTA_HF_CLIENT_IND_BATTCH           5   /* position of battery charge indicator */
#define BTA_HF_CLIENT_IND_CALLHELD         6   /* position of callheld indicator */
#define BTA_HF_CLIENT_IND_BEARER           7   /* position of bearer indicator */
typedef UINT8 tBTA_HF_CLIENT_IND_TYPE;

/* AT commands */
#define BTA_HF_CLIENT_AT_CMD_VTS    0
#define BTA_HF_CLIENT_AT_CMD_BTRH   1
#define BTA_HF_CLIENT_AT_CMD_CHUP   2
#define BTA_HF_CLIENT_AT_CMD_CHLD   3
#define BTA_HF_CLIENT_AT_CMD_BCC    4
#define BTA_HF_CLIENT_AT_CMD_CNUM   5
#define BTA_HF_CLIENT_AT_CMD_ATA    6
#define BTA_HF_CLIENT_AT_CMD_COPS   7
#define BTA_HF_CLIENT_AT_CMD_ATD    8
#define BTA_HF_CLIENT_AT_CMD_VGM    9
#define BTA_HF_CLIENT_AT_CMD_VGS    10
#define BTA_HF_CLIENT_AT_CMD_BVRA   11
#define BTA_HF_CLIENT_AT_CMD_CLCC   12
#define BTA_HF_CLIENT_AT_CMD_BINP   13
#define BTA_HF_CLIENT_AT_CMD_BLDN   14
#define BTA_HF_CLIENT_AT_CMD_NREC   15

typedef UINT8 tBTA_HF_CLIENT_AT_CMD_TYPE;

/* data associated with most non-AT events */
/* placeholder, if not needed should be removed*/
typedef struct {
} tBTA_HF_CLIENT_HDR;

/* data associated with BTA_HF_CLIENT_REGISTER_EVT */
typedef struct {
    tBTA_HF_CLIENT_HDR      hdr;
    UINT16                  handle;
    tBTA_HF_CLIENT_STATUS   status;
} tBTA_HF_CLIENT_REGISTER;

/* data associated with BTA_HF_CLIENT_OPEN_EVT */
typedef struct {
    tBTA_HF_CLIENT_HDR      hdr;
    BD_ADDR                 bd_addr;
    tBTA_HF_CLIENT_STATUS   status;
} tBTA_HF_CLIENT_OPEN;

/* data associated with BTA_HF_CLIENT_CONN_EVT */
typedef struct {
    tBTA_HF_CLIENT_HDR         hdr;
    tBTA_HF_CLIENT_PEER_FEAT   peer_feat;
    tBTA_HF_CLIENT_CHLD_FEAT   chld_feat;
} tBTA_HF_CLIENT_CONN;

/* data associated with BTA_HF_CLIENT_IND_EVT event */
typedef struct {
    tBTA_HF_CLIENT_HDR         hdr;
    tBTA_HF_CLIENT_IND_TYPE    type;
    UINT16                     value;
} tBTA_HF_CLIENT_IND;

/* data associated with BTA_HF_CLIENT_OPERATOR_NAME_EVT */
#define BTA_HF_CLIENT_OPERATOR_NAME_LEN 16
typedef struct {
    char                       name[BTA_HF_CLIENT_OPERATOR_NAME_LEN + 1];
} tBTA_HF_CLIENT_OPERATOR_NAME;

/* data associated with BTA_HF_CLIENT_CLIP_EVT  and BTA_HF_CLIENT_CCWA_EVT*/
#define BTA_HF_CLIENT_NUMBER_LEN 32
typedef struct {
    char                       number[BTA_HF_CLIENT_NUMBER_LEN + 1];
} tBTA_HF_CLIENT_NUMBER;

/* data associated with BTA_HF_CLIENT_AT_RESULT_EVT event */
typedef struct {
    tBTA_HF_CLIENT_AT_RESULT_TYPE    type;
    UINT16                           cme;
} tBTA_HF_CLIENT_AT_RESULT;

/* data associated with BTA_HF_CLIENT_CLCC_EVT event */
typedef struct {
    UINT32                     idx;
    BOOLEAN                    inc;
    UINT8                      status;
    BOOLEAN                    mpty;
    BOOLEAN                    number_present;
    char                       number[BTA_HF_CLIENT_NUMBER_LEN + 1];
} tBTA_HF_CLIENT_CLCC;

/* data associated with BTA_HF_CLIENT_CNUM_EVT event */
typedef struct {
    UINT16                     service;
    char                       number[BTA_HF_CLIENT_NUMBER_LEN + 1];
} tBTA_HF_CLIENT_CNUM;

/* data associated with other events */
typedef struct {
    UINT16                     value;
} tBTA_HF_CLIENT_VAL;

/* union of data associated with AG callback */
typedef union {
    tBTA_HF_CLIENT_HDR              hdr;
    tBTA_HF_CLIENT_REGISTER         reg;
    tBTA_HF_CLIENT_OPEN             open;
    tBTA_HF_CLIENT_CONN             conn;
    tBTA_HF_CLIENT_IND              ind;
    tBTA_HF_CLIENT_VAL              val;
    tBTA_HF_CLIENT_OPERATOR_NAME    operator;
    tBTA_HF_CLIENT_NUMBER           number;
    tBTA_HF_CLIENT_AT_RESULT        result;
    tBTA_HF_CLIENT_CLCC             clcc;
    tBTA_HF_CLIENT_CNUM             cnum;
} tBTA_HF_CLIENT;

typedef UINT32 tBTA_HF_CLIENT_FEAT;

/* HF Client callback */
typedef void (tBTA_HF_CLIENT_CBACK)(tBTA_HF_CLIENT_EVT event, void *p_data);

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         BTA_HfClientEnable
**
** Description      Enable the HF CLient service. When the enable
**                  operation is complete the callback function will be
**                  called with a BTA_HF_CLIENT_ENABLE_EVT. This function must
**                  be called before other function in the HF CLient API are
**                  called.
**
** Returns          BTA_SUCCESS if OK, BTA_FAILURE otherwise.
**
*******************************************************************************/
tBTA_STATUS BTA_HfClientEnable(tBTA_HF_CLIENT_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_HfClientDisable
**
** Description      Disable the HF Client service
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientDisable(void);

/*******************************************************************************
**
** Function         BTA_HfClientRegister
**
** Description      Register an HF Client service.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientRegister(tBTA_SEC sec_mask, tBTA_HF_CLIENT_FEAT features,
                          char *p_service_name);

/*******************************************************************************
**
** Function         BTA_HfClientDeregister
**
** Description      Deregister an HF Client service.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientDeregister(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_HfClientOpen
**
** Description      Opens a connection to an audio gateway.
**                  When connection is open callback function is called
**                  with a BTA_HF_CLIENT_OPEN_EVT. Only the data connection is
**                  opened. The audio connection is not opened.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientOpen(UINT16 handle, BD_ADDR bd_addr, tBTA_SEC sec_mask);

/*******************************************************************************
**
** Function         BTA_HfClientClose
**
** Description      Close the current connection to an audio gateway.
**                  Any current audio connection will also be closed
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientClose(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_HfCllientAudioOpen
**
** Description      Opens an audio connection to the currently connected
**                 audio gateway
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientAudioOpen(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_HfClientAudioClose
**
** Description      Close the currently active audio connection to an audio
**                  gateway. The data connection remains open
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientAudioClose(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_HfClientSendAT
**
** Description      send AT command
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientSendAT(UINT16 handle, tBTA_HF_CLIENT_AT_CMD_TYPE at, UINT32 val1, UINT32 val2, const char *str);

#if (BTM_SCO_HCI_INCLUDED == TRUE )
void BTA_HfClientCiData(void);
#endif /*#if (BTM_SCO_HCI_INCLUDED == TRUE ) */

int BTA_HfClientGetCbDataSize(tBTA_HF_CLIENT_EVT event);

#ifdef __cplusplus
}
#endif
#endif /* #if (BTA_HF_INCLUDED == TRUE) */
#endif /* BTA_HF_CLIENT_API_H */
