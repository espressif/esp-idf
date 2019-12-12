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

/******************************************************************************
 *
 *  This is the implementation of the API for the audio gateway (AG)
 *  subsystem of BTA, Broadcom's Bluetooth application layer for mobile
 *  phones.
 *
 ******************************************************************************/

#include <string.h>
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_ag_api.h"
#include "bta_ag_int.h"
#include "osi/allocator.h"

#if (BTA_AG_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_ag_reg =
{
    bta_ag_hdl_event,
    BTA_AgDisable
};

/*******************************************************************************
**
** Function         BTA_AgEnable
**
** Description      Enable the audio gateway service. When the enable
**                  operation is complete the callback function will be
**                  called with a BTA_AG_ENABLE_EVT. This function must
**                  be called before other function in the AG API are
**                  called.
**
** Returns          BTA_SUCCESS if OK, BTA_FAILURE otherwise.
**
*******************************************************************************/
tBTA_STATUS BTA_AgEnable(tBTA_AG_PARSE_MODE parse_mode, tBTA_AG_CBACK *p_cback)
{
    tBTA_AG_API_ENABLE  *p_buf;
    UINT8       idx;

    /* Error if AG is already enabled, or AG is in the middle of disabling. */
    for (idx = 0; idx < BTA_AG_NUM_SCB; idx++) {
        if (bta_ag_cb.scb[idx].in_use) {
            APPL_TRACE_ERROR ("BTA_AgEnable: FAILED, AG already enabled.");
            return BTA_FAILURE;
        }
    }
    /* register with BTA system manager */
    bta_sys_register(BTA_ID_AG, &bta_ag_reg);

    if ((p_buf = (tBTA_AG_API_ENABLE *) osi_malloc(sizeof(tBTA_AG_API_ENABLE))) != NULL) {
        p_buf->hdr.event = BTA_AG_API_ENABLE_EVT;
        p_buf->parse_mode = parse_mode;
        p_buf->p_cback = p_cback;
        bta_sys_sendmsg(p_buf);
    }
    return BTA_SUCCESS;
}

/*******************************************************************************
**
** Function         BTA_AgDisable
**
** Description      Disable the audio gateway service
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgDisable(void)
{
    BT_HDR  *p_buf;
    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_AG_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AgRegister
**
** Description      Register an Audio Gateway service.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgRegister(tBTA_SERVICE_MASK services, tBTA_SEC sec_mask,tBTA_AG_FEAT features,
                  char * p_service_names[], UINT8 app_id)
{
    tBTA_AG_API_REGISTER    *p_buf;
    int                     i;

    if ((p_buf = (tBTA_AG_API_REGISTER *) osi_malloc(sizeof(tBTA_AG_API_REGISTER))) != NULL) {
        p_buf->hdr.event = BTA_AG_API_REGISTER_EVT;
        p_buf->features = features;
        p_buf->sec_mask = sec_mask;
        p_buf->services = services;
        p_buf->app_id = app_id;
        for (i = 0; i < BTA_AG_NUM_IDX; i++) {
            if(p_service_names[i]) {
                BCM_STRNCPY_S(p_buf->p_name[i], BTA_SERVICE_NAME_LEN+1, p_service_names[i], BTA_SERVICE_NAME_LEN);
                p_buf->p_name[i][BTA_SERVICE_NAME_LEN] = 0;
            } else {
                p_buf->p_name[i][0] = 0;
            }
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AgDeregister
**
** Description      Deregister an audio gateway service.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgDeregister(UINT16 handle)
{
    BT_HDR  *p_buf;
    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_AG_API_DEREGISTER_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AgOpen
**
** Description      Opens a connection to a headset or hands-free device.
**                  When connection is open callback function is called
**                  with a BTA_AG_OPEN_EVT. Only the data connection is
**                  opened. The audio connection is not opened.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgOpen(UINT16 handle, BD_ADDR bd_addr, tBTA_SEC sec_mask, tBTA_SERVICE_MASK services)
{
    tBTA_AG_API_OPEN  *p_buf;

    if ((p_buf = (tBTA_AG_API_OPEN *) osi_malloc(sizeof(tBTA_AG_API_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_AG_API_OPEN_EVT;
        p_buf->hdr.layer_specific = handle;
        bdcpy(p_buf->bd_addr, bd_addr);
        p_buf->services = services;
        p_buf->sec_mask = sec_mask;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AgClose
**
** Description      Close the current connection to a headset or a handsfree
**                  Any current audio connection will also be closed.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgClose(UINT16 handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_AG_API_CLOSE_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AgAudioOpen
**
** Description      Opens an audio connection to the currently connected
**                  headset or handsfree.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgAudioOpen(UINT16 handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_AG_API_AUDIO_OPEN_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AgAudioClose
**
** Description      Close the currently active audio connection to a headset
**                  or handsfree. The data connection remains open
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgAudioClose(UINT16 handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_AG_API_AUDIO_CLOSE_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}


/*******************************************************************************
**
** Function         BTA_AgResult
**
** Description      Send an AT result code to a headset or hands-free device.
**                  This function is only used when the AG parse mode is set
**                  to BTA_AG_PARSE.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgResult(UINT16 handle, tBTA_AG_RES result, tBTA_AG_RES_DATA *p_data)
{
    tBTA_AG_API_RESULT  *p_buf;

    // printf("BTA_AgReslut: %d\n",result);
    
    if ((p_buf = (tBTA_AG_API_RESULT *) osi_malloc(sizeof(tBTA_AG_API_RESULT))) != NULL) {
        p_buf->hdr.event = BTA_AG_API_RESULT_EVT;
        p_buf->hdr.layer_specific = handle;
        p_buf->result = result;
        if(p_data) {
            memcpy(&p_buf->data, p_data, sizeof(p_buf->data));
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AgSetCodec
**
** Description      Specify the codec type to be used for the subsequent
**                  audio connection.
**
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AgSetCodec(UINT16 handle, tBTA_AG_PEER_CODEC codec)
{
    tBTA_AG_API_SETCODEC    *p_buf;

    if ((p_buf = (tBTA_AG_API_SETCODEC *) osi_malloc(sizeof(tBTA_AG_API_SETCODEC))) != NULL) {
        p_buf->hdr.event = BTA_AG_API_SETCODEC_EVT;
        p_buf->hdr.layer_specific = handle;
        p_buf->codec = codec;
        bta_sys_sendmsg(p_buf);
    }
}

#if (BTM_SCO_HCI_INCLUDED == TRUE )
/************************************************************************************************
 * Function        BTA_AgCiData
 * 
 * Description      Trigger the lower-layer to fetch and send audio data. This function is only
 *                  only used in the case that Voice Over HCI is enabled. Precondition is that
 *                  the HFP audio connection is connected. After this function is called, lower
 *                  layer will invoke esp_hf_client_outgoing_data_cb_t to fetch data
 *
 ***********************************************************************************************/
void BTA_AgCiData(void)
{
    BT_HDR *p_buf;
    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_AG_CI_SCO_DATA_EVT;
        bta_sys_sendmsg(p_buf);
    }
}
#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE ) */

#endif /* #if (BTA_AG_INCLUDED == TRUE)*/