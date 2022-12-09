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
 *  This is the implementation of the API for the handsfree (HF role)
 *  subsystem of BTA
 *
 ******************************************************************************/

#include <string.h>
#include "bta/bta_hf_client_api.h"
#include "bta_hf_client_int.h"
#include "osi/allocator.h"

#if (BTA_HF_INCLUDED == TRUE)
/*****************************************************************************
**  Constants and data types
*****************************************************************************/
static const tBTA_SYS_REG bta_hf_client_reg = {
    bta_hf_client_hdl_event,
    BTA_HfClientDisable
};

static const uint8_t bta_hf_client_cb_data_size[] = {
    0,                                  // #define BTA_HF_CLIENT_ENABLE_EVT            0
    sizeof(tBTA_HF_CLIENT_REGISTER),    // #define BTA_HF_CLIENT_REGISTER_EVT          1
    sizeof(tBTA_HF_CLIENT_OPEN),        // #define BTA_HF_CLIENT_OPEN_EVT              2
    0,                                  // #define BTA_HF_CLIENT_CLOSE_EVT             3
    sizeof(tBTA_HF_CLIENT_CONN),        // #define BTA_HF_CLIENT_CONN_EVT              4
    sizeof(tBTA_HF_CLIENT_HDR),         // #define BTA_HF_CLIENT_AUDIO_OPEN_EVT        5
    sizeof(tBTA_HF_CLIENT_HDR),         //#define BTA_HF_CLIENT_AUDIO_MSBC_OPEN_EVT   6 
    sizeof(tBTA_HF_CLIENT_HDR),         // #define BTA_HF_CLIENT_AUDIO_CLOSE_EVT       7
    sizeof(tBTA_HF_CLIENT_VAL),         // #define BTA_HF_CLIENT_SPK_EVT               8
    sizeof(tBTA_HF_CLIENT_VAL),         // #define BTA_HF_CLIENT_MIC_EVT               9
    sizeof(tBTA_HF_CLIENT_IND),         //#define BTA_HF_CLIENT_IND_EVT               10
    sizeof(tBTA_HF_CLIENT_VAL),         // #define BTA_HF_CLIENT_VOICE_REC_EVT         11
    sizeof(tBTA_HF_CLIENT_OPERATOR_NAME),    // #define BTA_HF_CLIENT_OPERATOR_NAME_EVT     12
    sizeof(tBTA_HF_CLIENT_NUMBER),           // #define BTA_HF_CLIENT_CLIP_EVT              13
    sizeof(tBTA_HF_CLIENT_NUMBER),      // #define BTA_HF_CLIENT_CCWA_EVT              14
    sizeof(tBTA_HF_CLIENT_AT_RESULT),   // #define BTA_HF_CLIENT_AT_RESULT_EVT         15
    sizeof(tBTA_HF_CLIENT_CLCC),        // #define BTA_HF_CLIENT_CLCC_EVT              16
    sizeof(tBTA_HF_CLIENT_CNUM),        //#define BTA_HF_CLIENT_CNUM_EVT              17
    sizeof(tBTA_HF_CLIENT_VAL),         // #define BTA_HF_CLIENT_BTRH_EVT              18
    sizeof(tBTA_HF_CLIENT_VAL),         // #define BTA_HF_CLIENT_BSIR_EVT              19
    sizeof(tBTA_HF_CLIENT_NUMBER),      // #define BTA_HF_CLIENT_BINP_EVT              20
    sizeof(tBTA_HF_CLIENT_VAL),         // #define BTA_HF_CLIENT_RING_INDICATION       21
    0,                                  // #define BTA_HF_CLIENT_DISABLE_EVT           30
};
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
tBTA_STATUS BTA_HfClientEnable(tBTA_HF_CLIENT_CBACK *p_cback)
{
    tBTA_HF_CLIENT_API_ENABLE  *p_buf;

    if (bta_sys_is_register (BTA_ID_HS)) {
        APPL_TRACE_ERROR("BTA HF Client is already enabled, ignoring ...");
        return BTA_FAILURE;
    }

    /* register with BTA system manager */
    bta_sys_register(BTA_ID_HS, &bta_hf_client_reg);

    if ((p_buf = (tBTA_HF_CLIENT_API_ENABLE *) osi_malloc(sizeof(tBTA_HF_CLIENT_API_ENABLE))) != NULL) {
        p_buf->hdr.event = BTA_HF_CLIENT_API_ENABLE_EVT;
        p_buf->p_cback = p_cback;
        bta_sys_sendmsg(p_buf);
    }

    return BTA_SUCCESS;
}

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
void BTA_HfClientDisable(void)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HF_CLIENT_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

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
                          char *p_service_name)
{
    tBTA_HF_CLIENT_API_REGISTER    *p_buf;

    if ((p_buf = (tBTA_HF_CLIENT_API_REGISTER *) osi_malloc(sizeof(tBTA_HF_CLIENT_API_REGISTER))) != NULL) {
        p_buf->hdr.event = BTA_HF_CLIENT_API_REGISTER_EVT;
        p_buf->features = features;
        p_buf->sec_mask = sec_mask;
        if (p_service_name) {
            BCM_STRNCPY_S(p_buf->name, p_service_name, BTA_SERVICE_NAME_LEN);
            p_buf->name[BTA_SERVICE_NAME_LEN] = 0;
        } else {
            p_buf->name[0] = '\0';
        }
        bta_sys_sendmsg(p_buf);
    }
}

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
void BTA_HfClientDeregister(UINT16 handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HF_CLIENT_API_DEREGISTER_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_HfClientOpen
**
** Description      Opens a connection to an audio gateway.
**                  When connection is open callback function is called
**                  with a BTA_AG_OPEN_EVT. Only the data connection is
**                  opened. The audio connection is not opened.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HfClientOpen(UINT16 handle, BD_ADDR bd_addr, tBTA_SEC sec_mask)
{
    tBTA_HF_CLIENT_API_OPEN  *p_buf;

    if ((p_buf = (tBTA_HF_CLIENT_API_OPEN *) osi_malloc(sizeof(tBTA_HF_CLIENT_API_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_HF_CLIENT_API_OPEN_EVT;
        p_buf->hdr.layer_specific = handle;
        bdcpy(p_buf->bd_addr, bd_addr);
        p_buf->sec_mask = sec_mask;
        bta_sys_sendmsg(p_buf);
    }
}

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
void BTA_HfClientClose(UINT16 handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HF_CLIENT_API_CLOSE_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

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
void BTA_HfClientAudioOpen(UINT16 handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HF_CLIENT_API_AUDIO_OPEN_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

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
void BTA_HfClientAudioClose(UINT16 handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HF_CLIENT_API_AUDIO_CLOSE_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

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
void BTA_HfClientSendAT(UINT16 handle, tBTA_HF_CLIENT_AT_CMD_TYPE at, UINT32 val1, UINT32 val2, const char *str)
{
    tBTA_HF_CLIENT_DATA_VAL  *p_buf;

    if ((p_buf = (tBTA_HF_CLIENT_DATA_VAL *) osi_malloc(sizeof(tBTA_HF_CLIENT_DATA_VAL))) != NULL) {
        p_buf->hdr.event = BTA_HF_CLIENT_SEND_AT_CMD_EVT;
        p_buf->uint8_val = at;
        p_buf->uint32_val1 = val1;
        p_buf->uint32_val2 = val2;

        if (str) {
            strlcpy(p_buf->str, str, BTA_HF_CLIENT_NUMBER_LEN + 1);
            p_buf->str[BTA_HF_CLIENT_NUMBER_LEN] = '\0';
        } else {
            p_buf->str[0] = '\0';
        }

        p_buf->hdr.layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}
#if (BTM_SCO_HCI_INCLUDED == TRUE )
void BTA_HfClientCiData(void)
{
    BT_HDR *p_buf;
    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HF_CLIENT_CI_SCO_DATA_EVT;
        bta_sys_sendmsg(p_buf);
    }
}
#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE ) */

int BTA_HfClientGetCbDataSize(tBTA_HF_CLIENT_EVT event)
{
    return bta_hf_client_cb_data_size[event];
}
#endif /* #if (BTA_HF_INCLUDED == TRUE) */
