/******************************************************************************
 *
 *  Copyright (C) 2004-2012 Broadcom Corporation
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
 *  This file contains functions for processing AT commands and results.
 *
 ******************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "common/bt_target.h"
#include "common/bt_trace.h"
#include "stack/bt_types.h"
#include "bta/bta_ag_api.h"
#include "bta_ag_at.h"
#include "bta_ag_int.h"
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "osi/allocator.h"
#include "stack/port_api.h"
#include "bta/utl.h"


#if (BTA_AG_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

/* ring timeout */
#define BTA_AG_RING_TOUT        5000
#define BTA_AG_CMD_MAX_VAL      32767  /* Maximum value is signed 16-bit value */

/* Invalid Chld command */
#define BTA_AG_INVALID_CHLD        255

/* clip type constants */
#define BTA_AG_CLIP_TYPE_MIN        128
#define BTA_AG_CLIP_TYPE_MAX        175
#define BTA_AG_CLIP_TYPE_DEFAULT    129
#define BTA_AG_CLIP_TYPE_VOIP       255

/*******************************************
*              HSP callback 
********************************************/
/* callback event lookup table for HSP */
const tBTA_AG_EVT bta_ag_hsp_cb_evt[] =
{
    BTA_AG_AT_CKPD_EVT,     /* BTA_AG_HS_CMD_CKPD */
    BTA_AG_SPK_EVT,         /* BTA_AG_HS_CMD_VGS */
    BTA_AG_MIC_EVT          /* BTA_AG_HS_CMD_VGM */
};
/* HSP AT commands matches bta_ag_hsp_cmd[] */
enum
{
    BTA_AG_HS_CMD_CKPD,
    BTA_AG_HS_CMD_VGS,
    BTA_AG_HS_CMD_VGM
};
/* HSP AT command interpreter table */
const tBTA_AG_AT_CMD bta_ag_hsp_cmd[] =
{
    {"+CKPD",   BTA_AG_AT_SET,                      BTA_AG_AT_INT, 200, 200},
    {"+VGS",    BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,  15},
    {"+VGM",    BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,  15},
    {"",        BTA_AG_AT_NONE,                     BTA_AG_AT_STR,   0,   0}
};

/*******************************************
*              HFP callback 
********************************************/
/* callback event lookup table for HFP  (Indexed by command) */
const tBTA_AG_EVT bta_ag_hfp_cb_evt[] =
{
    BTA_AG_AT_A_EVT,        /* BTA_AG_HF_CMD_A */
    BTA_AG_AT_D_EVT,        /* BTA_AG_HF_CMD_D */
    BTA_AG_SPK_EVT,         /* BTA_AG_HF_CMD_VGS */
    BTA_AG_MIC_EVT,         /* BTA_AG_HF_CMD_VGM */
    0,                      /* BTA_AG_HF_CMD_CCWA */
    BTA_AG_AT_CHLD_EVT,     /* BTA_AG_HF_CMD_CHLD */
    BTA_AG_AT_CHUP_EVT,     /* BTA_AG_HF_CMD_CHUP */
    BTA_AG_AT_CIND_EVT,     /* BTA_AG_HF_CMD_CIND */
    0,                      /* BTA_AG_HF_CMD_CLIP */
    0,                      /* BTA_AG_HF_CMD_CMER */
    BTA_AG_AT_VTS_EVT,      /* BTA_AG_HF_CMD_VTS */
    BTA_AG_AT_BINP_EVT,     /* BTA_AG_HF_CMD_BINP */
    BTA_AG_AT_BLDN_EVT,     /* BTA_AG_HF_CMD_BLDN */
    BTA_AG_AT_BVRA_EVT,     /* BTA_AG_HF_CMD_BVRA */
    0,                      /* BTA_AG_HF_CMD_BRSF */
    BTA_AG_AT_NREC_EVT,     /* BTA_AG_HF_CMD_NREC */
    BTA_AG_AT_CNUM_EVT,     /* BTA_AG_HF_CMD_CNUM */
    BTA_AG_AT_BTRH_EVT,     /* BTA_AG_HF_CMD_BTRH */
    BTA_AG_AT_CLCC_EVT,     /* BTA_AG_HF_CMD_CLCC */
    BTA_AG_AT_COPS_EVT,     /* BTA_AG_HF_CMD_COPS */
    0,                      /* BTA_AG_HF_CMD_CMEE */
    0,                      /* BTA_AG_HF_CMD_BIA */
    BTA_AG_AT_CBC_EVT,      /* BTA_AG_HF_CMD_CBC */
    0,                      /* BTA_AG_HF_CMD_BCC */
    BTA_AG_AT_BCS_EVT,      /* BTA_AG_HF_CMD_BCS */
    BTA_AG_AT_BAC_EVT       /* BTA_AG_HF_CMD_BAC */
};

/* HFP AT commands matches bta_ag_hfp_cmd[] */
enum
{
    BTA_AG_HF_CMD_A,
    BTA_AG_HF_CMD_D,
    BTA_AG_HF_CMD_VGS,
    BTA_AG_HF_CMD_VGM,
    BTA_AG_HF_CMD_CCWA,
    BTA_AG_HF_CMD_CHLD,
    BTA_AG_HF_CMD_CHUP,
    BTA_AG_HF_CMD_CIND,
    BTA_AG_HF_CMD_CLIP,
    BTA_AG_HF_CMD_CMER,
    BTA_AG_HF_CMD_VTS,
    BTA_AG_HF_CMD_BINP,
    BTA_AG_HF_CMD_BLDN,
    BTA_AG_HF_CMD_BVRA,
    BTA_AG_HF_CMD_BRSF,
    BTA_AG_HF_CMD_NREC,
    BTA_AG_HF_CMD_CNUM,
    BTA_AG_HF_CMD_BTRH,
    BTA_AG_HF_CMD_CLCC,
    BTA_AG_HF_CMD_COPS,
    BTA_AG_HF_CMD_CMEE,
    BTA_AG_HF_CMD_BIA,
    BTA_AG_HF_CMD_CBC,
    BTA_AG_HF_CMD_BCC,
    BTA_AG_HF_CMD_BCS,
    BTA_AG_HF_CMD_BAC
};

/* HFP AT command interpreter table */
const tBTA_AG_AT_CMD bta_ag_hfp_cmd[] =
{
    {"A",       BTA_AG_AT_NONE,                     BTA_AG_AT_STR,   0,   0},
    {"D",       (BTA_AG_AT_NONE | BTA_AG_AT_FREE),  BTA_AG_AT_STR,   0,   0},
    {"+VGS",    BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,  15},
    {"+VGM",    BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,  15},
    {"+CCWA",   BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,   1},
    /* Consider CHLD as str to take care of indexes for ECC */
    {"+CHLD",   (BTA_AG_AT_SET | BTA_AG_AT_TEST),   BTA_AG_AT_STR,   0,   4},
    {"+CHUP",   BTA_AG_AT_NONE,                     BTA_AG_AT_STR,   0,   0},
    {"+CIND",   (BTA_AG_AT_READ | BTA_AG_AT_TEST),  BTA_AG_AT_STR,   0,   0},
    {"+CLIP",   BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,   1},
    {"+CMER",   BTA_AG_AT_SET,                      BTA_AG_AT_STR,   0,   0},
    {"+VTS",    BTA_AG_AT_SET,                      BTA_AG_AT_STR,   0,   0},
    {"+BINP",   BTA_AG_AT_SET,                      BTA_AG_AT_INT,   1,   1},
    {"+BLDN",   BTA_AG_AT_NONE,                     BTA_AG_AT_STR,   0,   0},
    {"+BVRA",   BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,   1},
    {"+BRSF",   BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,   BTA_AG_CMD_MAX_VAL},
    {"+NREC",   BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,   0},
    {"+CNUM",   BTA_AG_AT_NONE,                     BTA_AG_AT_STR,   0,   0},
    {"+BTRH",   (BTA_AG_AT_READ | BTA_AG_AT_SET),   BTA_AG_AT_INT,   0,   2},
    {"+CLCC",   BTA_AG_AT_NONE,                     BTA_AG_AT_STR,   0,   0},
    {"+COPS",   (BTA_AG_AT_READ | BTA_AG_AT_SET),   BTA_AG_AT_STR,   0,   0},
    {"+CMEE",   BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,   1},
    {"+BIA",    BTA_AG_AT_SET,                      BTA_AG_AT_STR,   0,   20},
    {"+CBC",    BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,   100},
    {"+BCC",    BTA_AG_AT_NONE,                     BTA_AG_AT_STR,   0,   0},
    {"+BCS",    BTA_AG_AT_SET,                      BTA_AG_AT_INT,   0,   BTA_AG_CMD_MAX_VAL},
    {"+BAC",    BTA_AG_AT_SET,                      BTA_AG_AT_STR,   0,   0},
    {"",        BTA_AG_AT_NONE,                     BTA_AG_AT_STR,   0,   0}
};

/*******************************************
*              AT Result 
********************************************/
const tBTA_AG_AT_CMD *bta_ag_at_tbl[BTA_AG_NUM_IDX] =
{
    bta_ag_hsp_cmd,
    bta_ag_hfp_cmd
};

/* AT result code argument types */
enum
{
    BTA_AG_RES_FMT_NONE,       /* no argument */
    BTA_AG_RES_FMT_INT,        /* integer argument */
    BTA_AG_RES_FMT_STR         /* string argument */
};

#if defined(BTA_AG_MULTI_RESULT_INCLUDED) && (BTA_AG_MULTI_RESULT_INCLUDED == TRUE)
#define BTA_AG_AT_MULTI_LEN            2
#define AT_SET_RES_CB(res_cb, c, p, i) {res_cb.code = c; res_cb.p_arg = p; res_cb.int_arg = i;}

/* type for AT result code block */
typedef struct
{
    UINT8 code;
    char *p_arg;
    INT16 int_arg;
} tBTA_AG_RESULT_CB;

/* type for multiple AT result codes block */
typedef struct
{
    UINT8 num_result;
    tBTA_AG_RESULT_CB res_cb[BTA_AG_AT_MULTI_LEN];
} tBTA_AG_MULTI_RESULT_CB;
#endif

/* AT result code table element */
typedef struct
{
    const char  *p_res;         /* AT result string */
    UINT8       fmt;            /* whether argument is int or string */
} tBTA_AG_RESULT;

/* AT result code constant table  (Indexed by result code) */
const tBTA_AG_RESULT bta_ag_result_tbl[] =
{
    {"OK",      BTA_AG_RES_FMT_NONE},
    {"ERROR",   BTA_AG_RES_FMT_NONE},
    {"RING",    BTA_AG_RES_FMT_NONE},
    {"+VGS: ",  BTA_AG_RES_FMT_INT},
    {"+VGM: ",  BTA_AG_RES_FMT_INT},
    {"+CCWA: ", BTA_AG_RES_FMT_STR},
    {"+CHLD: ", BTA_AG_RES_FMT_STR},
    {"+CIND: ", BTA_AG_RES_FMT_STR},
    {"+CLIP: ", BTA_AG_RES_FMT_STR},
    {"+CIEV: ", BTA_AG_RES_FMT_STR},
    {"+BINP: ", BTA_AG_RES_FMT_STR},
    {"+BVRA: ", BTA_AG_RES_FMT_INT},
    {"+BRSF: ", BTA_AG_RES_FMT_INT},
    {"+BSIR: ", BTA_AG_RES_FMT_INT},
    {"+CNUM: ", BTA_AG_RES_FMT_STR},
    {"+BTRH: ", BTA_AG_RES_FMT_INT},
    {"+CLCC: ", BTA_AG_RES_FMT_STR},
    {"+COPS: ", BTA_AG_RES_FMT_STR},
    {"+CME ERROR: ", BTA_AG_RES_FMT_INT},
    {"+BCS: ",  BTA_AG_RES_FMT_INT},
    {"",        BTA_AG_RES_FMT_STR}
};

/* AT result codes, matches bta_ag_result_tbl[] */
enum
{
    BTA_AG_RES_OK,
    BTA_AG_RES_ERROR,
    BTA_AG_RES_RING,
    BTA_AG_RES_VGS,
    BTA_AG_RES_VGM,
    BTA_AG_RES_CCWA,
    BTA_AG_RES_CHLD,
    BTA_AG_RES_CIND,
    BTA_AG_RES_CLIP,
    BTA_AG_RES_CIEV,
    BTA_AG_RES_BINP,
    BTA_AG_RES_BVRA,
    BTA_AG_RES_BRSF,
    BTA_AG_RES_BSIR,
    BTA_AG_RES_CNUM,
    BTA_AG_RES_BTRH,
    BTA_AG_RES_CLCC,
    BTA_AG_RES_COPS,
    BTA_AG_RES_CMEE,
    BTA_AG_RES_BCS,
    BTA_AG_RES_UNAT
};

/* translation of API result code values to internal values */
const UINT8 bta_ag_trans_result[] =
{
    BTA_AG_RES_VGS,     /* BTA_AG_SPK_RES */
    BTA_AG_RES_VGM,     /* BTA_AG_MIC_RES */
    BTA_AG_RES_BSIR,    /* BTA_AG_INBAND_RING_RES */
    BTA_AG_RES_CIND,    /* BTA_AG_CIND_RES */
    BTA_AG_RES_BINP,    /* BTA_AG_BINP_RES */
    BTA_AG_RES_CIEV,    /* BTA_AG_IND_RES */
    BTA_AG_RES_BVRA,    /* BTA_AG_BVRA_RES */
    BTA_AG_RES_CNUM,    /* BTA_AG_CNUM_RES */
    BTA_AG_RES_BTRH,    /* BTA_AG_BTRH_RES */
    BTA_AG_RES_CLCC,    /* BTA_AG_CLCC_RES */
    BTA_AG_RES_COPS,    /* BTA_AG_COPS_RES */
    0,                  /* BTA_AG_IN_CALL_RES */
    0,                  /* BTA_AG_IN_CALL_CONN_RES */
    BTA_AG_RES_CCWA,    /* BTA_AG_CALL_WAIT_RES */
    0,                  /* BTA_AG_OUT_CALL_ORIG_RES */
    0,                  /* BTA_AG_OUT_CALL_ALERT_RES */
    0,                  /* BTA_AG_OUT_CALL_CONN_RES */
    0,                  /* BTA_AG_CALL_CANCEL_RES */
    0,                  /* BTA_AG_END_CALL_RES */
    0,                  /* BTA_AG_IN_CALL_HELD_RES */
    BTA_AG_RES_UNAT     /* BTA_AG_UNAT_RES */
};

/* callsetup indicator value lookup table */
const UINT8 bta_ag_callsetup_ind_tbl[] =
{
    0,                          /* BTA_AG_SPK_RES */
    0,                          /* BTA_AG_MIC_RES */
    0,                          /* BTA_AG_INBAND_RING_RES */
    0,                          /* BTA_AG_CIND_RES */
    0,                          /* BTA_AG_BINP_RES */
    0,                          /* BTA_AG_IND_RES */
    0,                          /* BTA_AG_BVRA_RES */
    0,                          /* BTA_AG_CNUM_RES */
    0,                          /* BTA_AG_BTRH_RES */
    0,                          /* BTA_AG_CLCC_RES */
    0,                          /* BTA_AG_COPS_RES */
    BTA_AG_CALLSETUP_INCOMING,  /* BTA_AG_IN_CALL_RES */
    BTA_AG_CALLSETUP_NONE,      /* BTA_AG_IN_CALL_CONN_RES */
    BTA_AG_CALLSETUP_INCOMING,  /* BTA_AG_CALL_WAIT_RES */
    BTA_AG_CALLSETUP_OUTGOING,  /* BTA_AG_OUT_CALL_ORIG_RES */
    BTA_AG_CALLSETUP_ALERTING,  /* BTA_AG_OUT_CALL_ALERT_RES */
    BTA_AG_CALLSETUP_NONE,      /* BTA_AG_OUT_CALL_CONN_RES */
    BTA_AG_CALLSETUP_NONE,      /* BTA_AG_CALL_CANCEL_RES */
    BTA_AG_CALLSETUP_NONE,      /* BTA_AG_END_CALL_RES */
    BTA_AG_CALLSETUP_NONE       /* BTA_AG_IN_CALL_HELD_RES */
};

#if defined(BTA_HSP_RESULT_REPLACE_COLON) && (BTA_HSP_RESULT_REPLACE_COLON == TRUE)
#define COLON_IDX_4_VGSVGM    4
#endif

/*******************************************
*              Funcitons Result 
********************************************/
/*******************************************************************************
**
** Function         bta_ag_send_result
**
** Description      Send an AT result code.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_send_result(tBTA_AG_SCB *p_scb, UINT8 code, char *p_arg, INT16 int_arg)
{
    char    buf[BTA_AG_AT_MAX_LEN + 16];
    char    *p = buf;
    UINT16  len;
#if (BTIF_TRACE_DEBUG == TRUE)
    memset(buf, NULL, sizeof(buf));
#endif
    /* init with \r\n */
    *p++ = '\r';
    *p++ = '\n';

    /* copy result code string */
    BCM_STRCPY_S(p, bta_ag_result_tbl[code].p_res);
#if defined(BTA_HSP_RESULT_REPLACE_COLON) && (BTA_HSP_RESULT_REPLACE_COLON == TRUE)
    if(p_scb->conn_service == BTA_AG_HSP) {
        /* If HSP then ":"symbol should be changed as "=" for HSP compatibility */
        switch(code) {
            case BTA_AG_RES_VGS:
            case BTA_AG_RES_VGM:
            {
                if(*(p+COLON_IDX_4_VGSVGM) == ':')
                {
                    #if defined(BTA_AG_RESULT_DEBUG) && (BTA_AG_RESULT_DEBUG == TRUE)
                    APPL_TRACE_DEBUG("[HSP] ':'symbol is changed as '=' for HSP compatibility");
                    #endif
                    *(p+COLON_IDX_4_VGSVGM) = '=';
                }
                break;
            }
        }
    }
#endif
    p += strlen(bta_ag_result_tbl[code].p_res);

    /* copy argument if any */
    if (bta_ag_result_tbl[code].fmt == BTA_AG_RES_FMT_INT) {
        p += utl_itoa((UINT16) int_arg, p);
    } else if (bta_ag_result_tbl[code].fmt == BTA_AG_RES_FMT_STR) {
        BCM_STRCPY_S(p, p_arg);
        p += strlen(p_arg);
    }
    /* finish with \r\n */
    *p++ = '\r';
    *p++ = '\n';
    APPL_TRACE_DEBUG("bta_ag_send_result: %s", buf);
    /* send to RFCOMM */
    PORT_WriteData(p_scb->conn_handle, buf, (UINT16) (p - buf), &len);
}

#if defined(BTA_AG_MULTI_RESULT_INCLUDED) && (BTA_AG_MULTI_RESULT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_ag_send_multi_result
**
** Description      Send multiple AT result codes.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_send_multi_result(tBTA_AG_SCB *p_scb, tBTA_AG_MULTI_RESULT_CB *m_res_cb)
{
    char    buf[BTA_AG_AT_MAX_LEN * BTA_AG_AT_MULTI_LEN + 16];
    char    *p = buf;
    UINT16  len;
    UINT8   res_idx = 0;

    if((!m_res_cb) || (m_res_cb->num_result == 0) || (m_res_cb->num_result > BTA_AG_AT_MULTI_LEN)) {
        APPL_TRACE_DEBUG("m_res_cb is NULL or num_result is out of range.");
        return;
    }

#if defined(BTA_AG_RESULT_DEBUG) && (BTA_AG_RESULT_DEBUG == TRUE)
    memset(buf, NULL, sizeof(buf));
#endif

    while(res_idx < m_res_cb->num_result) {
        /* init with \r\n */
        *p++ = '\r';
        *p++ = '\n';

        /* copy result code string */
        BCM_STRCPY_S(p, bta_ag_result_tbl[m_res_cb->res_cb[res_idx].code].p_res);
        p += strlen(bta_ag_result_tbl[m_res_cb->res_cb[res_idx].code].p_res);

        /* copy argument if any */
        if (bta_ag_result_tbl[m_res_cb->res_cb[res_idx].code].fmt == BTA_AG_RES_FMT_INT) {
            p += utl_itoa((UINT16) m_res_cb->res_cb[res_idx].int_arg, p);
        } else if (bta_ag_result_tbl[m_res_cb->res_cb[res_idx].code].fmt == BTA_AG_RES_FMT_STR) {
            BCM_STRCPY_S(p, m_res_cb->res_cb[res_idx].p_arg);
            p += strlen(m_res_cb->res_cb[res_idx].p_arg);
        }
        /* finish with \r\n */
        *p++ = '\r';
        *p++ = '\n';
        res_idx++;
    }
#if defined(BTA_AG_RESULT_DEBUG) && (BTA_AG_RESULT_DEBUG == TRUE)
    APPL_TRACE_DEBUG("send_result: %s", buf);
#endif
    /* send to RFCOMM */
    PORT_WriteData(p_scb->conn_handle, buf, (UINT16) (p - buf), &len);
}
#endif /* #if defined(BTA_AG_MULTI_RESULT_INCLUDED) && (BTA_AG_MULTI_RESULT_INCLUDED == TRUE) */

/*******************************************************************************
**
** Function         bta_ag_send_ok
**
** Description      Send an OK result code.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_send_ok(tBTA_AG_SCB *p_scb)
{
    bta_ag_send_result(p_scb, BTA_AG_RES_OK, NULL, 0);
}

/*******************************************************************************
**
** Function         bta_ag_send_error
**
** Description      Send an ERROR result code.
**                      errcode - used to send verbose errocode
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_send_error(tBTA_AG_SCB *p_scb, INT16 errcode)
{
    /* If HFP and extended audio gateway error codes are enabled */
    if (p_scb->conn_service == BTA_AG_HFP && p_scb->cmee_enabled) {
        bta_ag_send_result(p_scb, BTA_AG_RES_CMEE, NULL, errcode);
    } else {
        bta_ag_send_result(p_scb, BTA_AG_RES_ERROR, NULL, 0);
    }
}

/*******************************************************************************
**
** Function         bta_ag_send_ind
**
** Description      Send an indicator CIEV result code.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_send_ind(tBTA_AG_SCB *p_scb, UINT16 id, UINT16 value, BOOLEAN on_demand)
{
    char    str[12];
    char    *p = str;
    /* If the indicator is masked out, just return */
    /* Mandatory indicators can not be masked out. */
    if ((p_scb->bia_masked_out & ((UINT32)1 << id)) &&
        ((id != BTA_AG_IND_CALL) && (id != BTA_AG_IND_CALLSETUP) && (id != BTA_AG_IND_CALLHELD)))
        return;

    /* Ensure we do not send duplicate indicators if not requested by app */
    /* If it was requested by app, transmit CIEV even if it is duplicate. */
    if (id == BTA_AG_IND_CALL) {
        if ((value == p_scb->call_ind) && (on_demand == FALSE)) {
            return;
        }
        p_scb->call_ind = (UINT8)value;
    }
    if ((id == BTA_AG_IND_CALLSETUP) && (on_demand == FALSE)) {
        if (value == p_scb->callsetup_ind) {
            return;
        }
        p_scb->callsetup_ind = (UINT8)value;
    }
    if ((id == BTA_AG_IND_SERVICE) && (on_demand == FALSE)) {
        if (value == p_scb->service_ind) {
            return;
        }
        p_scb->service_ind = (UINT8)value;
    }
    if ((id == BTA_AG_IND_SIGNAL) && (on_demand == FALSE)) {
        if (value == p_scb->signal_ind) {
            return;
        }
        p_scb->signal_ind = (UINT8)value;
    }
    if ((id == BTA_AG_IND_ROAM) && (on_demand == FALSE)) {
        if (value == p_scb->roam_ind) {
            return;            
        }
        p_scb->roam_ind = (UINT8)value;
    }
    if ((id == BTA_AG_IND_BATTCHG) && (on_demand == FALSE)) {
        if (value == p_scb->battchg_ind) {
            return;
        }
        p_scb->battchg_ind = (UINT8)value;
    }
    if ((id == BTA_AG_IND_CALLHELD) && (on_demand == FALSE)) {
        /* call swap could result in sending callheld=1 multiple times */
        if ((value != 1) && (value == p_scb->callheld_ind)) {
            return;
        }
        p_scb->callheld_ind = (UINT8)value;
    }
    if (p_scb->cmer_enabled) {
        p += utl_itoa(id, p);
        *p++ = ',';
        utl_itoa(value, p);
        bta_ag_send_result(p_scb, BTA_AG_RES_CIEV, str, 0);
    }
}

/*******************************************************************************
**
** Function         bta_ag_parse_cmer
**
** Description      Parse AT+CMER parameter string.
**
**
** Returns          TRUE if parsed ok, FALSE otherwise.
**
*******************************************************************************/
static BOOLEAN bta_ag_parse_cmer(char *p_s, BOOLEAN *p_enabled)
{
    INT16   n[4] = {-1, -1, -1, -1};
    int     i;
    char    *p;

    for (i = 0; i < 4; i++) {
        /* skip to comma delimiter */
        for (p = p_s; *p != ',' && *p != 0; p++);
        /* get integer value */
        *p = 0;
        n[i] = utl_str2int(p_s);
        p_s = p + 1;
        if (p_s == 0) {
            break;
        }
    }
    /* process values */
    if (n[0] < 0 || n[3] < 0) {
        return FALSE;
    }
    if ((n[0] == 3) && ((n[3] == 1) || (n[3] == 0))) {
        *p_enabled = (BOOLEAN) n[3];
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         bta_ag_parse_chld
**
** Description      Parse AT+CHLD parameter string.
**
**
** Returns          Returns idx (1-7), 0 if ECC not enabled or BTA_AG_INVALID_CHLD
                    if idx doesn't exist/1st character of argument is not a digit
**
*******************************************************************************/
static UINT8 bta_ag_parse_chld(tBTA_AG_SCB *p_scb, char *p_s)
{
    UINT8   retval = 0;
    INT16   idx = -1;
    UNUSED(p_scb);

    if (!isdigit(p_s[0])) {
        return BTA_AG_INVALID_CHLD;
    }

    if (p_s[1] != 0) {
        /* p_idxstr++;  point to beginning of call number */
        idx = utl_str2int(&p_s[1]);
        if (idx != -1 && idx < 255) {
            retval = (UINT8)idx;
        } else {
            retval = BTA_AG_INVALID_CHLD;
        }
    }
    return(retval);
}

#if (BTM_WBS_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_ag_parse_bac
**
** Description      Parse AT+BAC parameter string.
**
** Returns          Returns bitmap of supported codecs.
**
*******************************************************************************/
static tBTA_AG_PEER_CODEC bta_ag_parse_bac(tBTA_AG_SCB *p_scb, char *p_s)
{
    tBTA_AG_PEER_CODEC  retval = BTA_AG_CODEC_NONE;
    UINT16  uuid_codec;
    BOOLEAN cont = FALSE;       /* Continue processing */
    char *p;

    while (p_s) {
        /* skip to comma delimiter */
        for (p = p_s; *p != ',' && *p != 0; p++);

        /* get integre value */
        if (*p != 0) {
            *p = 0;
            cont = TRUE;
        } else {
            cont = FALSE;
        }
        uuid_codec = utl_str2int(p_s);
        switch(uuid_codec) {
            case UUID_CODEC_CVSD:   
                retval |= BTA_AG_CODEC_CVSD;     
                break;
            
            case UUID_CODEC_MSBC:   
                retval |= BTA_AG_CODEC_MSBC;     
                break;
            
            default:
                APPL_TRACE_ERROR("Unknown Codec UUID(%d) received", uuid_codec);
                return BTA_AG_CODEC_NONE;
        }
        if (cont) {
            p_s = p + 1;
        }
        else {
            break;
        }
    }
    return (retval);
}
#endif /* #if (BTM_WBS_INCLUDED == TRUE ) */

/*******************************************************************************
**
** Function         bta_ag_process_unat_res
**
** Description      Process the unat response data and remove extra carriage return
**                  and line feed
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_process_unat_res(char *unat_result)
{
    UINT8   str_leng;
    UINT8   i = 0;
    UINT8   j = 0;
    UINT8   pairs_of_nl_cr;
    char    trim_data[BTA_AG_AT_MAX_LEN];

    str_leng = strlen(unat_result);
    /* If no extra CR and LF, just return */
    if(str_leng < 4) {
        return;
    }

    /* Remove the carriage return and left feed */
    while(unat_result[0] =='\r' && unat_result[1] =='\n'
        && unat_result[str_leng-2] =='\r' && unat_result[str_leng-1] =='\n') {
        pairs_of_nl_cr = 1;
        for (i=0;i<(str_leng-4*pairs_of_nl_cr);i++) {
            trim_data[j++] = unat_result[i+pairs_of_nl_cr*2];
        }
        /* Add EOF */
        trim_data[j] = '\0';
        str_leng = str_leng - 4;
        BCM_STRNCPY_S(unat_result, trim_data, BTA_AG_AT_MAX_LEN);
        i=0;
        j=0;
        if(str_leng <4) {
            return;
        }
    }
    return;
}

/*******************************************************************************
**
** Function         bta_ag_inband_enabled
**
** Description      Determine whether in-band ring can be used.
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_ag_inband_enabled(tBTA_AG_SCB *p_scb)
{
    /* if feature is enabled and no other scbs connected */
    if (p_scb->inband_enabled && !bta_ag_other_scb_open(p_scb)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*******************************************************************************
**
** Function         bta_ag_send_call_inds
**
** Description      Send call and callsetup indicators.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_send_call_inds(tBTA_AG_SCB *p_scb, tBTA_AG_RES result)
{
    UINT8 call = p_scb->call_ind;
    UINT8 callsetup;
    /* set new call and callsetup values based on BTA_AgResult */
    callsetup = bta_ag_callsetup_ind_tbl[result];

    if (result == BTA_AG_END_CALL_RES) {
        call = BTA_AG_CALL_INACTIVE;
    } else if (result == BTA_AG_IN_CALL_CONN_RES || result == BTA_AG_OUT_CALL_CONN_RES
             || result == BTA_AG_IN_CALL_HELD_RES) {
        call = BTA_AG_CALL_ACTIVE;
    } else {
        call = p_scb->call_ind;
    }
    /* Send indicator function tracks if the values have actually changed */
    bta_ag_send_ind(p_scb, BTA_AG_IND_CALL, call, FALSE);
    bta_ag_send_ind(p_scb, BTA_AG_IND_CALLSETUP, callsetup, FALSE);
}

/*******************************************************************************
**
** Function         bta_ag_at_hsp_cback
**
** Description      AT command processing callback for HSP.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_at_hsp_cback(tBTA_AG_SCB *p_scb, UINT16 cmd, UINT8 arg_type,
                                char *p_arg, INT16 int_arg)
{
    tBTA_AG_VAL val;
    APPL_TRACE_DEBUG("AT cmd:%d arg_type:%d arg:%d arg:%s", cmd, arg_type, int_arg, p_arg);

    /* send OK */
    bta_ag_send_ok(p_scb);
    val.hdr.handle = bta_ag_scb_to_idx(p_scb);
    val.hdr.app_id = p_scb->app_id;
    val.num = (UINT16) int_arg;
    BCM_STRNCPY_S(val.str, p_arg, BTA_AG_AT_MAX_LEN);
    val.str[BTA_AG_AT_MAX_LEN] = 0;
    /* call callback with event */
    (*bta_ag_cb.p_cback)(bta_ag_hsp_cb_evt[cmd], (tBTA_AG *) &val);
}

/*******************************************************************************
**
** Function         bta_ag_at_hfp_cback
**
** Description      AT command processing callback for HFP.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_at_hfp_cback(tBTA_AG_SCB *p_scb, UINT16 cmd, UINT8 arg_type,
                                char *p_arg, INT16 int_arg)
{
    tBTA_AG_VAL     val;
    tBTA_AG_EVT     event;
    tBTA_AG_SCB     *ag_scb;
    UINT32          i, ind_id;
    UINT32          bia_masked_out;
#if (BTM_WBS_INCLUDED == TRUE)
    tBTA_AG_PEER_CODEC  codec_type, codec_sent;
#endif
    if (p_arg == NULL) {
        APPL_TRACE_ERROR("%s: p_arg is null, send error and return", __func__);
        bta_ag_send_error(p_scb, BTA_AG_ERR_INV_CHAR_IN_TSTR);
        return;
    }
    APPL_TRACE_DEBUG("HFP AT cmd:%d arg_type:%d arg:%d arg:%s", cmd, arg_type, int_arg, p_arg);

    val.hdr.handle = bta_ag_scb_to_idx(p_scb);
    val.hdr.app_id = p_scb->app_id;
    val.num = int_arg;
    bdcpy(val.bd_addr, p_scb->peer_addr);
    BCM_STRNCPY_S(val.str, p_arg, BTA_AG_AT_MAX_LEN);
    val.str[BTA_AG_AT_MAX_LEN] = 0;
    event = bta_ag_hfp_cb_evt[cmd];

    switch (cmd)
    {
        case BTA_AG_HF_CMD_A:
        case BTA_AG_HF_CMD_VGS:
        case BTA_AG_HF_CMD_VGM:
        case BTA_AG_HF_CMD_CHUP:
        case BTA_AG_HF_CMD_CBC:
            /* send OK */
            bta_ag_send_ok(p_scb);
            break;

        case BTA_AG_HF_CMD_BLDN:
            /* Do not send OK, App will send error or OK depending on last-dial-umber enabled or not */
            break;

        case BTA_AG_HF_CMD_D:
        {
            /* Do not send OK for Dial cmds Let application decide whether to send OK or ERROR*/
            /* if mem dial cmd, make sure string contains only digits */
            if(p_arg[0] == '>') {
                if(!utl_isintstr(p_arg+1)) {
                    event = 0;
                    bta_ag_send_error(p_scb, BTA_AG_ERR_INV_CHAR_IN_DSTR);
                }
            } else if (p_arg[0] == 'V') {
                /* ATDV : Dial VoIP Call */ 
                /* We do not check string. Code will be added later if needed. */
                if(!((p_scb->peer_features & BTA_AG_PEER_FEAT_VOIP) && (p_scb->features & BTA_AG_FEAT_VOIP))) {
                    event = 0;
                    bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
                }
            } else {
            /* If dial cmd, make sure string contains only dial digits
            ** Dial digits are 0-9, A-C, *, #, + */
                if(!utl_isdialstr(p_arg)) {
                    event = 0;
                    bta_ag_send_error(p_scb, BTA_AG_ERR_INV_CHAR_IN_DSTR);
                }
            }
            break;
        }

        case BTA_AG_HF_CMD_CCWA:
            p_scb->ccwa_enabled = (BOOLEAN) int_arg; /* store setting */
            bta_ag_send_ok(p_scb); /* send OK */
            break;

        case BTA_AG_HF_CMD_CHLD:
        {
            if (arg_type == BTA_AG_AT_TEST) {
                /* don't call callback */
                event = 0;
                /* send CHLD string */
                /* Form string based on supported 1.5 feature */
                if ((p_scb->peer_version >= HFP_VERSION_1_5) &&
                    (p_scb->features & BTA_AG_FEAT_ECC) &&
                    (p_scb->peer_features & BTA_AG_PEER_FEAT_ECC)) {
                    bta_ag_send_result(p_scb, BTA_AG_RES_CHLD, p_bta_ag_cfg->chld_val_ecc, 0);
                    } else {
                    bta_ag_send_result(p_scb, BTA_AG_RES_CHLD, p_bta_ag_cfg->chld_val, 0);
                    }
                /* send OK */
                bta_ag_send_ok(p_scb);
                /* if service level conn. not already open, now it's open */
                bta_ag_svc_conn_open(p_scb, NULL);
            } else {
                val.idx = bta_ag_parse_chld(p_scb, val.str);
                if (val.idx == BTA_AG_INVALID_CHLD) {
                    event = 0;
                    bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
                    break;
                }
                if(val.idx && !((p_scb->features & BTA_AG_FEAT_ECC) && (p_scb->peer_features & BTA_AG_PEER_FEAT_ECC))) {
                    /* we do not support ECC, but HF is sending us a CHLD with call index*/
                    event = 0;
                    bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);

                } else {
                    /* If it is swap between calls, set call held indicator to 3(out of valid 0-2)
                    ** Application will set it back to 1
                    ** callheld indicator will be sent across to the peer. */
                    if(val.str[0] == '2') {
                        for (i = 0, ag_scb = &bta_ag_cb.scb[0]; i < BTA_AG_NUM_SCB; i++, ag_scb++) {
                            if (ag_scb->in_use) {
                                if((ag_scb->call_ind == BTA_AG_CALL_ACTIVE)
                                    && (ag_scb->callsetup_ind == BTA_AG_CALLSETUP_NONE)) {
                                    ag_scb->callheld_ind = BTA_AG_CALLHELD_NOACTIVE + 1;
                                    }
                            }
                        }
                    }
                }
            /* Do not send OK. Let app decide after parsing the val str */
            /* bta_ag_send_ok(p_scb); */
            }
            break;
        }

        case BTA_AG_HF_CMD_CIND:
            if (arg_type == BTA_AG_AT_TEST) {
                /* don't call callback */
                event = 0;
                /* send CIND string, send OK */
                bta_ag_send_result(p_scb, BTA_AG_RES_CIND, p_bta_ag_cfg->cind_info, 0);
                bta_ag_send_ok(p_scb);
            }
            break;

        case BTA_AG_HF_CMD_CLIP:
            /* store setting, send OK */
            p_scb->clip_enabled = (BOOLEAN) int_arg;
            bta_ag_send_ok(p_scb);
            break;

        case BTA_AG_HF_CMD_CMER:
            /* if parsed ok store setting, send OK */
            if (bta_ag_parse_cmer(p_arg, &p_scb->cmer_enabled)) {
                bta_ag_send_ok(p_scb);
                /* if service level conn. not already open and our features and
                ** peer features do not have 3-way, service level conn. now open
                */
                if (!p_scb->svc_conn && !((p_scb->features & BTA_AG_FEAT_3WAY) && (p_scb->peer_features & BTA_AG_PEER_FEAT_3WAY))) {
                    bta_ag_svc_conn_open(p_scb, NULL);
                }
            } else {
                bta_ag_send_error(p_scb, BTA_AG_ERR_INV_CHAR_IN_TSTR);
            }
            break;

        case BTA_AG_HF_CMD_VTS:
            /* check argument */
            if (strlen(p_arg) == 1) {
                bta_ag_send_ok(p_scb);
            } else {
                event = 0;
                bta_ag_send_error(p_scb, BTA_AG_ERR_INV_CHAR_IN_TSTR);
            }
            break;

        case BTA_AG_HF_CMD_BINP:
            /* if feature not set don't call callback, send ERROR */
            if (!(p_scb->features & BTA_AG_FEAT_VTAG)) {
                event = 0;
                bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
            }
            break;

        case BTA_AG_HF_CMD_BVRA:
            /* if feature not supported don't call callback, send ERROR. App will send OK */
            if (!(p_scb->features & BTA_AG_FEAT_VREC)) {
                event = 0;
                bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
            } else {
                bta_ag_send_ok(p_scb);
            }
            break;

        case BTA_AG_HF_CMD_BRSF:
        {
            /* store peer features */
            p_scb->peer_features = (UINT16) int_arg;
            /* send BRSF, send OK */
            bta_ag_send_result(p_scb, BTA_AG_RES_BRSF, NULL, (INT16) (p_scb->features & BTA_AG_BSRF_FEAT_SPEC));
            bta_ag_send_ok(p_scb);
            break;
        }

        case BTA_AG_HF_CMD_NREC:
            /* if feature send OK, else don't call callback, send ERROR */
            if (p_scb->features & BTA_AG_FEAT_ECNR) {
                bta_ag_send_ok(p_scb);
            } else {
                event = 0;
                bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
            }
            break;

        case BTA_AG_HF_CMD_BTRH:
            /* if feature send BTRH, send OK:, else don't call callback, send ERROR */
            if (p_scb->features & BTA_AG_FEAT_BTRH) {
                /* If set command; send response and notify app */
                if (arg_type == BTA_AG_AT_SET) {
                    for (i = 0, ag_scb = &bta_ag_cb.scb[0]; i < BTA_AG_NUM_SCB; i++, ag_scb++) {
                        if (ag_scb->in_use) {
                            bta_ag_send_result(ag_scb, BTA_AG_RES_BTRH, NULL, int_arg);
                        }
                    }
                    bta_ag_send_ok(p_scb);
                } else {
                    /* Read Command */
                    val.num = BTA_AG_BTRH_READ;
                }
            } else {
                event = 0;
                bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
            }
            break;

        case BTA_AG_HF_CMD_COPS:
            if (arg_type == BTA_AG_AT_SET) {
                /* don't call callback */
                event = 0;
                /* send OK */
                bta_ag_send_ok(p_scb);
            }
            break;

        case BTA_AG_HF_CMD_CMEE:
            if (p_scb->features & BTA_AG_FEAT_EXTERR) {
                /* store setting */
                p_scb->cmee_enabled = (BOOLEAN) int_arg;
                /* send OK */
                bta_ag_send_ok(p_scb);
            } else {
                bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
            }
            /* don't call callback */
            event = 0;
            break;

        case BTA_AG_HF_CMD_BIA:
        {
            /* don't call callback */
            event = 0;
            bia_masked_out = p_scb->bia_masked_out;
            /* Parse the indicator mask */
            for (i = 0, ind_id = 1; (val.str[i] != 0) && (ind_id <= 20); i++, ind_id++) {
                if (val.str[i] == ',')
                    continue;
                if (val.str[i] == '0')
                    bia_masked_out |= ((UINT32)1 << ind_id);
                else if (val.str[i] == '1')
                    bia_masked_out &= ~((UINT32)1 << ind_id);
                else
                    break;
                i++;
                if ( (val.str[i] != 0) && (val.str[i] != ',') )
                    break;
            }
            if (val.str[i] == 0) {
                p_scb->bia_masked_out = bia_masked_out;
                bta_ag_send_ok (p_scb);
            } else {
                bta_ag_send_error (p_scb, BTA_AG_ERR_INVALID_INDEX);
            }
            break;
        }

        case BTA_AG_HF_CMD_CNUM:
            if(!(p_scb->features & BTA_AG_FEAT_ECS)) {
                event = 0;
                bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
            }
            break;
        
        case BTA_AG_HF_CMD_CLCC:
            if(!(p_scb->features & BTA_AG_FEAT_ECS)) {
                event = 0;
                bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
            }
            break;

#if (BTM_WBS_INCLUDED == TRUE)
        case BTA_AG_HF_CMD_BAC:
        {
            bta_ag_send_ok(p_scb);
            /* store available codecs from the peer */
            if((p_scb->peer_features & BTA_AG_PEER_FEAT_CODEC) && (p_scb->features & BTA_AG_FEAT_CODEC)) {
                p_scb->peer_codecs = bta_ag_parse_bac(p_scb, p_arg);
                p_scb->codec_updated = TRUE;

                if (p_scb->peer_codecs & BTA_AG_CODEC_MSBC) {
                    p_scb->sco_codec = UUID_CODEC_MSBC;
                    APPL_TRACE_DEBUG("Received AT+BAC, updating sco codec to MSBC");
                } else {
                    p_scb->sco_codec = UUID_CODEC_CVSD;
                    APPL_TRACE_DEBUG("Received AT+BAC, updating sco codec to CVSD");
                }
                /* The above logic sets the stack preferred codec based on local and peer codec
                capabilities. This can be overridden by the application depending on its preference
                using the bta_ag_setcodec API. We send the peer_codecs to the application. */
                val.num = p_scb->peer_codecs;
                /* Received BAC while in codec negotiation. */
                if ((bta_ag_cb.sco.state == BTA_AG_SCO_CODEC_ST) && (bta_ag_cb.sco.p_curr_scb == p_scb)) {
                    bta_ag_codec_negotiate (p_scb);
                }
            } else {
                p_scb->peer_codecs = BTA_AG_CODEC_NONE;
                APPL_TRACE_ERROR("Unexpected CMD:AT+BAC, Codec Negotiation is not supported");
            }
            break;
        }

        case BTA_AG_HF_CMD_BCS:
        {
            bta_ag_send_ok(p_scb);
            /* stop cn timer */
            bta_sys_stop_timer(&p_scb->cn_timer);

            switch(int_arg) {
                case UUID_CODEC_CVSD:   
                    codec_type = BTA_AG_CODEC_CVSD;     
                    break;
                
                case UUID_CODEC_MSBC:   
                    codec_type = BTA_AG_CODEC_MSBC;
                    break;
                
                default:
                    APPL_TRACE_ERROR("Unknown codec_uuid %d", int_arg);
                    codec_type = 0xFFFF;
                    break;
            }

            if (p_scb->codec_fallback) {
                codec_sent = BTA_AG_CODEC_CVSD;
            } else {
                codec_sent = p_scb->sco_codec;
            }

            if(codec_type == codec_sent) {
                bta_ag_sco_codec_nego(p_scb, TRUE);
            } else {
                bta_ag_sco_codec_nego(p_scb, FALSE);
            }
            /* send final codec info to callback */
            val.num = codec_sent;
            break;
        }

        case BTA_AG_HF_CMD_BCC:
        {
            bta_ag_send_ok(p_scb);
            bta_ag_sco_open(p_scb, NULL);
            break;
        }
#endif
        default:
            bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
            break;
    }
    /* call callback */
    if (event != 0) {
        (*bta_ag_cb.p_cback)(event, (tBTA_AG *) &val);
    }
}

/*******************************************************************************
**
** Function         bta_ag_at_err_cback
**
** Description      AT command parser error callback.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_at_err_cback(tBTA_AG_SCB *p_scb, BOOLEAN unknown, char *p_arg)
{
    tBTA_AG_VAL     val;

    if(unknown && (!strlen(p_arg))) {
        APPL_TRACE_DEBUG("Empty AT cmd string received");
        bta_ag_send_ok(p_scb);
        return;
    }

    /* if unknown AT command and configured to pass these to app */
    if (unknown && (p_scb->features & BTA_AG_FEAT_UNAT)) {
        val.hdr.handle = bta_ag_scb_to_idx(p_scb);
        val.hdr.app_id = p_scb->app_id;
        val.num = 0;
        BCM_STRNCPY_S(val.str, p_arg, BTA_AG_AT_MAX_LEN);
        val.str[BTA_AG_AT_MAX_LEN] = 0;
        (*bta_ag_cb.p_cback)(BTA_AG_AT_UNAT_EVT, (tBTA_AG *) &val);
    } else {
        bta_ag_send_error(p_scb, BTA_AG_ERR_OP_NOT_SUPPORTED);
    }
}

/*******************************************************************************
**
** Function         bta_ag_hsp_result
**
** Description      Handle API result for HSP connections.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_hsp_result(tBTA_AG_SCB *p_scb, tBTA_AG_API_RESULT *p_result)
{
    UINT8 code = bta_ag_trans_result[p_result->result];

    APPL_TRACE_DEBUG("bta_ag_hsp_result : res = %d", p_result->result);

    switch(p_result->result) {
        case BTA_AG_SPK_RES:
        case BTA_AG_MIC_RES:
            bta_ag_send_result(p_scb, code, NULL, p_result->data.num);
            break;

        case BTA_AG_IN_CALL_RES:
        {
            /* tell sys to stop av if any */
            bta_sys_sco_use(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
            /* if sco already opened or no inband ring send ring now */
            if (bta_ag_sco_is_open(p_scb) || !bta_ag_inband_enabled(p_scb) ||
                (p_scb->features & BTA_AG_FEAT_NOSCO)) {
                bta_ag_send_ring(p_scb, (tBTA_AG_DATA *) p_result);
            } else {
                /* else open sco, send ring after sco opened */
                /* HSPv1.2: AG shall not send RING if using in-band ring tone. */
                if (p_scb->hsp_version >= HSP_VERSION_1_2) {
                    p_scb->post_sco = BTA_AG_POST_SCO_NONE;
                } else {
                    p_scb->post_sco = BTA_AG_POST_SCO_RING;
                }
                bta_ag_sco_open(p_scb, (tBTA_AG_DATA *) p_result);
            }
            break;
        }

        case BTA_AG_IN_CALL_CONN_RES:
        case BTA_AG_OUT_CALL_ORIG_RES:
        {
            /* if incoming call connected stop ring timer */
            if (p_result->result == BTA_AG_IN_CALL_CONN_RES) {
                bta_sys_stop_timer(&p_scb->act_timer);
            }

            if (!(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                /* if audio connected to this scb open sco */
                if (p_result->data.audio_handle == bta_ag_scb_to_idx(p_scb)) {
                    bta_ag_sco_open(p_scb, (tBTA_AG_DATA *) p_result);
                } else if (p_result->data.audio_handle == BTA_AG_HANDLE_NONE) {
                    /* else if no audio at call close sco */
                    bta_ag_sco_close(p_scb, (tBTA_AG_DATA *) p_result);
                }
            }
            break;
        }

        case BTA_AG_END_CALL_RES:
        {
            /* stop ring timer */
            bta_sys_stop_timer(&p_scb->act_timer);
            /* close sco */
            if ((bta_ag_sco_is_open(p_scb) || bta_ag_sco_is_opening(p_scb)) && !(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                bta_ag_sco_close(p_scb, (tBTA_AG_DATA *) p_result);
            } else {
                /* if av got suspended by this call, let it resume. */
                bta_sys_sco_unuse(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
            }
            break;
        }

        case BTA_AG_INBAND_RING_RES:
            p_scb->inband_enabled = p_result->data.state;
            APPL_TRACE_DEBUG("inband_enabled set to %d", p_scb->inband_enabled);
            break;

        case BTA_AG_UNAT_RES:
        {
            if (p_result->data.ok_flag != BTA_AG_OK_ERROR) {
                if (p_result->data.str[0] != 0) {
                    bta_ag_send_result(p_scb, code, p_result->data.str, 0);
                }
                if (p_result->data.ok_flag == BTA_AG_OK_DONE) {
                    bta_ag_send_ok(p_scb);
                }
            } else {
                bta_ag_send_error(p_scb, BTA_AG_ERR_INV_CHAR_IN_TSTR);
            }
            break;
        }
        default:
            break; /* ignore all others */
    }
}

/*******************************************************************************
**
** Function         bta_ag_hfp_result
**
** Description      Handle API result for HFP connections.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_hfp_result(tBTA_AG_SCB *p_scb, tBTA_AG_API_RESULT *p_result)
{
    UINT8 code = bta_ag_trans_result[p_result->result];

    APPL_TRACE_DEBUG("bta_ag_hfp_result : res = %d", p_result->result);

    switch(p_result->result)
    {
        case BTA_AG_SPK_RES:
        case BTA_AG_MIC_RES:
            bta_ag_send_result(p_scb, code, NULL, p_result->data.num);
            break;

        case BTA_AG_IN_CALL_RES:
        {
            /* tell sys to stop av if any */
            bta_sys_sco_use(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
            /* store caller id string.
             * append type info at the end.
             * make sure a valid type info is passed.
             * otherwise add 129 as default type */
            if ((p_result->data.num < BTA_AG_CLIP_TYPE_MIN) || (p_result->data.num > BTA_AG_CLIP_TYPE_MAX)) {
                if (p_result->data.num != BTA_AG_CLIP_TYPE_VOIP) {
                    p_result->data.num = BTA_AG_CLIP_TYPE_DEFAULT;
                }
            }
            APPL_TRACE_DEBUG("CLIP type :%d", p_result->data.num);
            p_scb->clip[0] = 0;
            if (p_result->data.str[0] != 0) {
                snprintf(p_scb->clip, sizeof(p_scb->clip), "%s,%d", p_result->data.str, p_result->data.num);
            }
            /* send callsetup indicator */
            if (p_scb->post_sco == BTA_AG_POST_SCO_CALL_END) {
                /* Need to sent 2 callsetup IND's(Call End and Incoming call) after SCO close. */
                p_scb->post_sco = BTA_AG_POST_SCO_CALL_END_INCALL;
            } else {
                bta_ag_send_call_inds(p_scb, p_result->result);
                /* if sco already opened or no inband ring send ring now */
                if (bta_ag_sco_is_open(p_scb) || !bta_ag_inband_enabled(p_scb) ||
                    (p_scb->features & BTA_AG_FEAT_NOSCO)) {
                    bta_ag_send_ring(p_scb, (tBTA_AG_DATA *) p_result);
                } else {
                    /* else open sco, send ring after sco opened */
                    p_scb->post_sco = BTA_AG_POST_SCO_RING;
                    bta_ag_sco_open(p_scb, (tBTA_AG_DATA *) p_result);
                }
            }
            break;
        }

        case BTA_AG_IN_CALL_CONN_RES:
        {
            /* stop ring timer */
            bta_sys_stop_timer(&p_scb->act_timer);
            /* if sco not opened and we need to open it, send indicators first
            ** then open sco. */
            bta_ag_send_call_inds(p_scb, p_result->result);

            if (!(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                if (p_result->data.audio_handle == bta_ag_scb_to_idx(p_scb)) {
                    bta_ag_sco_open(p_scb, (tBTA_AG_DATA *) p_result);
                } else if ((p_result->data.audio_handle == BTA_AG_HANDLE_NONE) && bta_ag_sco_is_open(p_scb)) {
                    bta_ag_sco_close(p_scb, (tBTA_AG_DATA *) p_result);
                }
            }
            break;
        }

        case BTA_AG_IN_CALL_HELD_RES:
            /* stop ring timer */
            bta_sys_stop_timer(&p_scb->act_timer);
            bta_ag_send_call_inds(p_scb, p_result->result);
            break;

        case BTA_AG_OUT_CALL_ORIG_RES:
            bta_ag_send_call_inds(p_scb, p_result->result);
            if (p_result->data.audio_handle == bta_ag_scb_to_idx(p_scb) && !(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                bta_ag_sco_open(p_scb, (tBTA_AG_DATA *) p_result);
            }
            break;

        case BTA_AG_OUT_CALL_ALERT_RES:
            /* send indicators */
            bta_ag_send_call_inds(p_scb, p_result->result);
            if (p_result->data.audio_handle == bta_ag_scb_to_idx(p_scb) && !(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                bta_ag_sco_open(p_scb, (tBTA_AG_DATA *) p_result);
            }
            break;

        case BTA_AG_MULTI_CALL_RES:
            /* open SCO at SLC for this three way call */
            APPL_TRACE_DEBUG("Headset Connected in three way call");
            if (!(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                if (p_result->data.audio_handle == bta_ag_scb_to_idx(p_scb)) {
                    bta_ag_sco_open(p_scb, (tBTA_AG_DATA *) p_result);
                } else if (p_result->data.audio_handle == BTA_AG_HANDLE_NONE) {
                    bta_ag_sco_close(p_scb, (tBTA_AG_DATA *) p_result);
                }
            }
            break;

        case BTA_AG_OUT_CALL_CONN_RES:
            /* send indicators */
            bta_ag_send_call_inds(p_scb, p_result->result);
            /* open or close sco */
            if (!(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                if (p_result->data.audio_handle == bta_ag_scb_to_idx(p_scb)) {
                    bta_ag_sco_open(p_scb, (tBTA_AG_DATA *) p_result);
                } else if (p_result->data.audio_handle == BTA_AG_HANDLE_NONE) {
                    bta_ag_sco_close(p_scb, (tBTA_AG_DATA *) p_result);
                }
            }
            break;

        case BTA_AG_CALL_CANCEL_RES:
            /* send indicators */
            bta_ag_send_call_inds(p_scb, p_result->result);
            break;

        case BTA_AG_END_CALL_RES:
            /* stop ring timer */
            bta_sys_stop_timer(&p_scb->act_timer);
            /* if sco open, close sco then send indicator values */
            if ((bta_ag_sco_is_open(p_scb) || bta_ag_sco_is_opening(p_scb)) && !(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                p_scb->post_sco = BTA_AG_POST_SCO_CALL_END;
                bta_ag_sco_close(p_scb, (tBTA_AG_DATA *) p_result);
            } else if (p_scb->post_sco == BTA_AG_POST_SCO_CALL_END_INCALL) {
                /* sco closing for outgoing call because of incoming call */
                /* Send only callsetup end indicator after sco close */
                p_scb->post_sco = BTA_AG_POST_SCO_CALL_END;
            } else {
                bta_ag_send_call_inds(p_scb, p_result->result);
                /* if av got suspended by this call, let it resume. */
                bta_sys_sco_unuse(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
            }
            break;

        case BTA_AG_INBAND_RING_RES:
            p_scb->inband_enabled = p_result->data.state;
            APPL_TRACE_DEBUG("inband_enabled set to %d", p_scb->inband_enabled);
            bta_ag_send_result(p_scb, code, NULL, p_result->data.state);
            break;

        case BTA_AG_CIND_RES:
        {
            /* store local values */
            p_scb->call_ind = p_result->data.str[0] - '0';
            p_scb->callsetup_ind = p_result->data.str[2] - '0';
            p_scb->service_ind = p_result->data.str[4] - '0';
            p_scb->signal_ind = p_result->data.str[6] - '0';
            p_scb->roam_ind = p_result->data.str[8] - '0';
            p_scb->battchg_ind = p_result->data.str[10] - '0';
            p_scb->callheld_ind = p_result->data.str[12] - '0';
            
            APPL_TRACE_DEBUG("cind call:%d callsetup:%d", p_scb->call_ind, p_scb->callsetup_ind);
            bta_ag_send_result(p_scb, code, p_result->data.str, 0);
            bta_ag_send_ok(p_scb);
            break;
        }

        case BTA_AG_BINP_RES: // Not supported yet
        case BTA_AG_CNUM_RES:
        case BTA_AG_CLCC_RES:
        case BTA_AG_COPS_RES:
        {
            if (p_result->data.ok_flag != BTA_AG_OK_ERROR) {
                if (p_result->data.str[0] != 0) {
                   bta_ag_send_result(p_scb, code, p_result->data.str, 0);
                   bta_ag_send_ok(p_scb);
                }
                if (p_result->data.ok_flag == BTA_AG_OK_DONE) {
                    bta_ag_send_ok(p_scb);
                }
            } else {
                bta_ag_send_error(p_scb, p_result->data.errcode);
            }
            break;
        }

        case BTA_AG_UNAT_RES:
            if (p_result->data.ok_flag != BTA_AG_OK_ERROR) {
                if (p_result->data.str[0] != 0) {
                    bta_ag_process_unat_res(p_result->data.str);
                    APPL_TRACE_DEBUG("BTA_AG_RES :%s",p_result->data.str);
                    bta_ag_send_result(p_scb, code, p_result->data.str, 0);
                }

                if (p_result->data.ok_flag == BTA_AG_OK_DONE) {
                    bta_ag_send_ok(p_scb);
                }
            } else {
                bta_ag_send_error(p_scb, p_result->data.errcode);
            }
            break;

        case BTA_AG_CALL_WAIT_RES:
            if (p_scb->ccwa_enabled) {
                bta_ag_send_result(p_scb, code, p_result->data.str, 0);
            }
            bta_ag_send_call_inds(p_scb, p_result->result);
            break;

        case BTA_AG_IND_RES:
            bta_ag_send_ind(p_scb, p_result->data.ind.type, p_result->data.ind.value, FALSE);
            break;

        case BTA_AG_BVRA_RES:
            bta_ag_send_result(p_scb, code, NULL, p_result->data.state);
            if (p_result->data.ok_flag!= BTA_AG_OK_ERROR)
            {
                bta_ag_send_ok(p_scb);
            } else {
                bta_ag_send_error(p_scb, p_result->data.errcode);
            }
            break;

        case BTA_AG_BTRH_RES: // Not supported yet
            if (p_result->data.ok_flag != BTA_AG_OK_ERROR) {
                /* Don't respond to read if not in response & hold state */
                if (p_result->data.num != BTA_AG_BTRH_NO_RESP) {
                    bta_ag_send_result(p_scb, code, NULL, p_result->data.num);
                }
                /* In case of a response to a read request we need to send OK */
                if (p_result->data.ok_flag == BTA_AG_OK_DONE) {
                    bta_ag_send_ok(p_scb);
                }
            } else {
                bta_ag_send_error(p_scb, p_result->data.errcode);
            }
            break;

       default:
            break;
    }
}


/*******************************************************************************
**
** Function         bta_ag_result
**
** Description      Handle API result.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_result(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    if (p_scb->conn_service == BTA_AG_HSP) {
        bta_ag_hsp_result(p_scb, &p_data->api_result);
    } else {
        bta_ag_hfp_result(p_scb, &p_data->api_result);
    }
}

/*******************************************************************************
**
** Function         bta_ag_send_bcs
**
** Description      Send +BCS AT command to peer.
**
** Returns          void
**
*******************************************************************************/
#if (BTM_WBS_INCLUDED == TRUE )
void bta_ag_send_bcs(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UINT16 codec_uuid;

    if (p_scb->codec_fallback) {
        codec_uuid = UUID_CODEC_CVSD;
    } else {
        switch(p_scb->sco_codec) {
            case BTA_AG_CODEC_NONE:
                codec_uuid = UUID_CODEC_CVSD;
                break;
            
            case BTA_AG_CODEC_CVSD:
                codec_uuid = UUID_CODEC_CVSD;
                break;
            
            case BTA_AG_CODEC_MSBC:
                codec_uuid = UUID_CODEC_MSBC;
                break;
            
            default:
                APPL_TRACE_ERROR("bta_ag_send_bcs: unknown codec %d, use CVSD", p_scb->sco_codec);
                codec_uuid = UUID_CODEC_CVSD;
                break;
        }
    }
    /* send +BCS */
    APPL_TRACE_DEBUG("send +BCS codec is %d", codec_uuid);
    bta_ag_send_result(p_scb, BTA_AG_RES_BCS, NULL, codec_uuid);
}
#endif

/*******************************************************************************
**
** Function         bta_ag_send_ring
**
** Description      Send RING result code to peer.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_send_ring(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UNUSED(p_data);

#if defined(BTA_AG_MULTI_RESULT_INCLUDED) && (BTA_AG_MULTI_RESULT_INCLUDED == TRUE)
    tBTA_AG_MULTI_RESULT_CB m_res_cb;
    if (p_scb->conn_service == BTA_AG_HFP && p_scb->clip_enabled && p_scb->clip[0] != 0) {
        memset(&m_res_cb, NULL, sizeof(tBTA_AG_MULTI_RESULT_CB));
        m_res_cb.num_result = 2;
        AT_SET_RES_CB(m_res_cb.res_cb[0], BTA_AG_RES_RING, NULL, 0)
        AT_SET_RES_CB(m_res_cb.res_cb[1], BTA_AG_RES_CLIP, p_scb->clip, 0)
        bta_ag_send_multi_result(p_scb, &m_res_cb);
    } else {
        /* send RING ONLY */
        bta_ag_send_result(p_scb, BTA_AG_RES_RING, NULL, 0);
    }
#else
    /* send RING */
    bta_ag_send_result(p_scb, BTA_AG_RES_RING, NULL, 0);
    /* if HFP and clip enabled and clip data send CLIP */
    if (p_scb->conn_service == BTA_AG_HFP && p_scb->clip_enabled && p_scb->clip[0] != 0) {
        bta_ag_send_result(p_scb, BTA_AG_RES_CLIP, p_scb->clip, 0);
    }
#endif
    /* restart ring timer */
    bta_sys_start_timer(&p_scb->act_timer, BTA_AG_RING_TOUT_EVT, BTA_AG_RING_TOUT);
}

#endif /* #if (BTA_AG_INCLUDED == TRUE) */