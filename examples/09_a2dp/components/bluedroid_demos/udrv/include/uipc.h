/******************************************************************************
 *
 *  Copyright (C) 2007-2012 Broadcom Corporation
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
#ifndef UIPC_H
#define UIPC_H

#include "bt_types.h"

#define UIPC_CH_ID_AV_CTRL  0
#define UIPC_CH_ID_AV_AUDIO 1
#define UIPC_CH_NUM         2

#define UIPC_CH_ID_ALL      3   /* used to address all the ch id at once */

#define DEFAULT_READ_POLL_TMO_MS 100

typedef UINT8 tUIPC_CH_ID;

/* Events generated */
typedef enum {
    UIPC_OPEN_EVT           = 0x0001,
    UIPC_CLOSE_EVT          = 0x0002,
    UIPC_RX_DATA_EVT        = 0x0004,
    UIPC_RX_DATA_READY_EVT  = 0x0008,
    UIPC_TX_DATA_READY_EVT  = 0x0010
} tUIPC_EVENT;

/*
 * UIPC IOCTL Requests
 */

#define UIPC_REQ_RX_FLUSH               1
#define UIPC_REG_CBACK                  2
#define UIPC_REG_REMOVE_ACTIVE_READSET  3
#define UIPC_SET_READ_POLL_TMO          4

typedef void (tUIPC_RCV_CBACK)(tUIPC_CH_ID ch_id, tUIPC_EVENT event); /* points to BT_HDR which describes event type and length of data; len contains the number of bytes of entire message (sizeof(BT_HDR) + offset + size of data) */

const char *dump_uipc_event(tUIPC_EVENT event);

/*******************************************************************************
**
** Function         UIPC_Init
**
** Description      Initialize UIPC module
**
** Returns          void
**
*******************************************************************************/
void UIPC_Init(void *);

/*******************************************************************************
**
** Function         UIPC_Open
**
** Description      Open UIPC interface
**
** Returns          void
**
*******************************************************************************/
BOOLEAN UIPC_Open(tUIPC_CH_ID ch_id, tUIPC_RCV_CBACK *p_cback);

/*******************************************************************************
**
** Function         UIPC_Close
**
** Description      Close UIPC interface
**
** Returns          void
**
*******************************************************************************/
void UIPC_Close(tUIPC_CH_ID ch_id);

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
BOOLEAN UIPC_SendBuf(tUIPC_CH_ID ch_id, BT_HDR *p_msg);

/*******************************************************************************
**
** Function         UIPC_Send
**
** Description      Called to transmit a message over UIPC.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN UIPC_Send(tUIPC_CH_ID ch_id, UINT16 msg_evt, UINT8 *p_buf, UINT16 msglen);

/*******************************************************************************
**
** Function         UIPC_Read
**
** Description      Called to read a message from UIPC.
**
** Returns          void
**
*******************************************************************************/
UINT32 UIPC_Read(tUIPC_CH_ID ch_id, UINT16 *p_msg_evt, UINT8 *p_buf, UINT32 len);

/*******************************************************************************
**
** Function         UIPC_Ioctl
**
** Description      Called to control UIPC.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN UIPC_Ioctl(tUIPC_CH_ID ch_id, UINT32 request, void *param);

#endif  /* UIPC_H */
