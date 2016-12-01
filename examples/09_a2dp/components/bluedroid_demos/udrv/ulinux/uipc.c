/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

/*****************************************************************************
 *
 *  Filename:      uipc.c
 *
 *  Description:   UIPC implementation for bluedroid
 *
 *****************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include "uipc.h"
#include "esp_system.h"
#include "EspAudio.h"
#include "EspAudioCom.h"
#include "bt_trace.h"

#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
/*****************************************************************************
**  Constants & Macros
******************************************************************************/



const char *dump_uipc_event(tUIPC_EVENT event)
{
    return NULL;
}

/*******************************************************************************
**
** Function         UIPC_Init
**
** Description      Initialize UIPC module
**
** Returns          void
**
*******************************************************************************/
void UIPC_Init(void *dummy)
{
    LOG_ERROR("UIPC_Init: Free memory: %d bytes\n", system_get_free_heap_size());
    // EspAudio_Init();

    {
        int volumn;
        // TODO: review the stream param config parameter here
        EspAudioPlayerStreamCfg(StreamSampleRate_44k, 2, StreamBitLen_16BIT);
        EspAudio_SetupStream("stream.pcm", InputSrcType_Stream);
        EspAudio_GetVolume(&volumn);
        LOG_ERROR("UIPC: Vol: %d\n", volumn);
        EspAudio_SetVolume(99);
    }
    return;
}

/*******************************************************************************
**
** Function         UIPC_Open
**
** Description      Open UIPC interface
**
** Returns          void
**
*******************************************************************************/
BOOLEAN UIPC_Open(tUIPC_CH_ID ch_id, tUIPC_RCV_CBACK *p_cback)
{
    LOG_ERROR("UIPC_Open\n");

    //if (ch_id == UIPC_CH_ID_AV_AUDIO) {
    // }

    /*
    if (p_cback) {
        p_cback(ch_id, UIPC_OPEN_EVT);
    }
    */
    return TRUE;
}

/*******************************************************************************
**
** Function         UIPC_Close
**
** Description      Close UIPC interface
**
** Returns          void
**
*******************************************************************************/
void UIPC_Close(tUIPC_CH_ID ch_id)
{
    return;
}

/*******************************************************************************
**
** Function         UIPC_SendBuf
**
** Description      Called to transmit a message over UIPC.
**                  Message buffer will be freed by UIPC_SendBuf.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN UIPC_SendBuf(tUIPC_CH_ID ch_id, BT_HDR *p_msg)
{
    return TRUE;
}

/*******************************************************************************
**
** Function         UIPC_Send
**
** Description      Called to transmit a message over UIPC.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN UIPC_Send(tUIPC_CH_ID ch_id, UINT16 msg_evt, UINT8 *p_buf, UINT16 msglen)
{
    if (ch_id == UIPC_CH_ID_AV_AUDIO) {
        uint32_t t_now = system_get_time();
        printf("t: %u, l %d\n", t_now, msglen);
        EspAudioPlayerStreamWrite(p_buf, msglen);
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         UIPC_Read
**
** Description      Called to read a message from UIPC.
**
** Returns          void
**
*******************************************************************************/
UINT32 UIPC_Read(tUIPC_CH_ID ch_id, UINT16 *p_msg_evt, UINT8 *p_buf, UINT32 len)
{
    return 0;
}


/*******************************************************************************
**
** Function         UIPC_Ioctl
**
** Description      Called to control UIPC.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN UIPC_Ioctl(tUIPC_CH_ID ch_id, UINT32 request, void *param)
{
    return TRUE;
}

