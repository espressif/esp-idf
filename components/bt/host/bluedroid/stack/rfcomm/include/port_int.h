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

/*****************************************************************************
 *
 *  This file contains definitions internal to the PORT unit
 *
 *****************************************************************************/

#ifndef PORT_INT_H
#define PORT_INT_H

#include "common/bt_target.h"
#include "stack/rfcdefs.h"
#include "stack/port_api.h"
#include "osi/fixed_queue.h"
#include "common/bt_defs.h"

/* Local events passed when application event is sent from the api to PORT */
/* ???*/
#define PORT_EVENT_OPEN         (1  | BT_EVT_TO_BTU_SP_EVT)
#define PORT_EVENT_CONTROL      (2  | BT_EVT_TO_BTU_SP_EVT)
#define PORT_EVENT_SET_STATE    (3  | BT_EVT_TO_BTU_SP_EVT)
#define PORT_EVENT_SET_CALLBACK (5  | BT_EVT_TO_BTU_SP_EVT)
#define PORT_EVENT_WRITE        (6  | BT_EVT_TO_BTU_SP_EVT)
#define PORT_EVENT_PURGE        (7  | BT_EVT_TO_BTU_SP_EVT)
#define PORT_EVENT_SEND_ERROR   (8  | BT_EVT_TO_BTU_SP_EVT)
#define PORT_EVENT_FLOW_CONTROL (9  | BT_EVT_TO_BTU_SP_EVT)

/*
** Flow control configuration values for the mux
*/
#define PORT_FC_UNDEFINED       0   /* mux flow control mechanism not defined yet */
#define PORT_FC_TS710           1   /* use TS 07.10 flow control  */
#define PORT_FC_CREDIT          2   /* use RFCOMM credit based flow control */

/*
** Define Port Data Transfere control block
*/
typedef struct {
    fixed_queue_t *queue;         /* Queue of buffers waiting to be sent */
    BOOLEAN  peer_fc;       /* TRUE if flow control is set based on peer's request */
    BOOLEAN  user_fc;       /* TRUE if flow control is set based on user's request  */
    UINT32   queue_size;    /* Number of data bytes in the queue */
    tPORT_CALLBACK *p_callback;  /* Address of the callback function */
} tPORT_DATA;

/*
** Port control structure used to pass modem info
*/
typedef struct {
#define MODEM_SIGNAL_DTRDSR        0x01
#define MODEM_SIGNAL_RTSCTS        0x02
#define MODEM_SIGNAL_RI            0x04
#define MODEM_SIGNAL_DCD           0x08

    UINT8   modem_signal;       /* [DTR/DSR | RTS/CTS | RI | DCD ] */

    UINT8   break_signal;       /* 0-3 s in steps of 200 ms */

    UINT8   discard_buffers;    /* 0 - do not discard, 1 - discard */

#define RFCOMM_CTRL_BREAK_ASAP      0
#define RFCOMM_CTRL_BREAK_IN_SEQ    1

    UINT8   break_signal_seq;   /* as soon as possible | in sequence (default) */

    BOOLEAN fc;                 /* TRUE when the device is unable to accept frames */
} tPORT_CTRL;


/*
** RFCOMM multiplexer Control Block
*/
typedef struct {
    TIMER_LIST_ENT tle;       /* Timer list entry */
    fixed_queue_t  *cmd_q;          /* Queue for command messages on this mux */
    UINT8     port_inx[RFCOMM_MAX_DLCI + 1];  /* Array for quick access to  */
    /* tPORT based on dlci        */
    BD_ADDR   bd_addr;        /* BD ADDR of the peer if initiator */
    UINT16    lcid;           /* Local cid used for this channel */
    UINT16    peer_l2cap_mtu; /* Max frame that can be sent to peer L2CAP */
    UINT8     state;          /* Current multiplexer channel state */
    UINT8     is_initiator;   /* TRUE if this side sends SABME (dlci=0) */
    BOOLEAN   local_cfg_sent;
    BOOLEAN   peer_cfg_rcvd;
    BOOLEAN   restart_required; /* TRUE if has to restart channel after disc */
    BOOLEAN   peer_ready;      /* True if other side can accept frames */
    UINT8     flow;            /* flow control mechanism for this mux */
    BOOLEAN   l2cap_congested; /* TRUE if L2CAP is congested */
    BOOLEAN   is_disc_initiator; /* TRUE if initiated disc of port */
    UINT16    pending_lcid;    /* store LCID for incoming connection while connecting */
    UINT8     pending_id;      /* store l2cap ID for incoming connection while connecting */
} tRFC_MCB;


/*
** RFCOMM Port Connection Control Block
*/
struct t_rfc_port {
#define RFC_PORT_STATE_IDLE          0
#define RFC_PORT_STATE_WAIT_START    1
#define RFC_PORT_STATE_OPENING       2
#define RFC_PORT_STATE_OPENED        3
#define RFC_PORT_STATE_CLOSING       4

    UINT8     state;          /* Current state of the connection */

#define RFC_RSP_PN                  0x01
#define RFC_RSP_RPN_REPLY           0x02
#define RFC_RSP_RPN                 0x04
#define RFC_RSP_MSC                 0x08
#define RFC_RSP_RLS                 0x10

    UINT8    expected_rsp;

    tRFC_MCB *p_mcb;

    TIMER_LIST_ENT tle;       /* Timer list entry */
};
typedef struct t_rfc_port tRFC_PORT;


/*
** Define control block containing information about PORT connection
*/
struct t_port_info {
    UINT8   inx;            /* Index of this control block in the port_info array */
    BOOLEAN in_use;         /* True when structure is allocated */

#define PORT_STATE_CLOSED   0
#define PORT_STATE_OPENING  1
#define PORT_STATE_OPENED   2
#define PORT_STATE_CLOSING  3

    UINT8   state;          /* State of the application */

    UINT8   scn;            /* Service channel number */
    UINT16  uuid;           /* Service UUID */

    BD_ADDR bd_addr;        /* BD ADDR of the device for the multiplexer channel */
    BOOLEAN is_server;      /* TRUE if the server application */
    UINT8   dlci;           /* DLCI of the connection */

    UINT8   error;          /* Last error detected */

    UINT8   line_status;    /* Line status as reported by peer */

    UINT8   default_signal_state; /* Initial signal state depending on uuid */

    UINT16  mtu;            /* Max MTU that port can receive */
    UINT16  peer_mtu;       /* Max MTU that port can send */

    tPORT_DATA tx;          /* Control block for data from app to peer */
    tPORT_DATA rx;          /* Control block for data from peer to app */

    tPORT_STATE user_port_pars;  /* Port parameters for user connection */
    tPORT_STATE peer_port_pars;  /* Port parameters for user connection */

    tPORT_CTRL  local_ctrl;
    tPORT_CTRL  peer_ctrl;

#define PORT_CTRL_REQ_SENT          0x01
#define PORT_CTRL_REQ_CONFIRMED     0x02
#define PORT_CTRL_IND_RECEIVED      0x04
#define PORT_CTRL_IND_RESPONDED     0x08

    UINT8       port_ctrl;                  /* Modem Status Command  */

    BOOLEAN     rx_flag_ev_pending;         /* RXFLAG Character is received */

    tRFC_PORT   rfc;                        /* RFCOMM port control block */

    UINT32      ev_mask;                    /* Event mask for the callback */
    tPORT_CALLBACK      *p_callback;        /* Pointer to users callback function */
    tPORT_CALLBACK      *p_mgmt_callback;   /* Callback function to receive connection up/down */
    tPORT_DATA_CALLBACK *p_data_callback;   /* Callback function to receive data indications */
    tPORT_DATA_CO_CALLBACK *p_data_co_callback;   /* Callback function with callouts and flowctrl */
    UINT16      credit_tx;                  /* Flow control credits for tx path */
    UINT16      credit_rx;                  /* Flow control credits for rx path, this is */
    /* number of buffers peer is allowed to sent */
    UINT16      credit_rx_max;              /* Max number of credits we will allow this guy to sent */
    UINT16      credit_rx_low;              /* Number of credits when we send credit update */
    UINT16      rx_buf_critical;            /* port receive queue critical watermark level */
    BOOLEAN     keep_port_handle;           /* TRUE if port is not deallocated when closing */
    /* it is set to TRUE for server when allocating port */
    UINT16      keep_mtu;                   /* Max MTU that port can receive by server */
};
typedef struct t_port_info tPORT;


/* Define the PORT/RFCOMM control structure
*/
typedef struct {
    tPORT        port[MAX_RFC_PORTS];            /* Port info pool */
    tRFC_MCB     rfc_mcb[MAX_BD_CONNECTIONS];    /* RFCOMM bd_connections pool */
} tPORT_CB;

#ifdef __cplusplus
extern "C" {
#endif

/*
** Functions provided by the port_utils.c
*/
extern tPORT    *port_allocate_port (UINT8 dlci, BD_ADDR bd_addr);
extern void     port_set_defaults (tPORT *p_port);
extern void     port_select_mtu (tPORT *p_port);
extern void     port_release_port (tPORT *p_port);
extern tPORT    *port_find_mcb_dlci_port (tRFC_MCB *p_mcb, UINT8 dlci);
extern tRFC_MCB *port_find_mcb (BD_ADDR bd_addr);
extern tPORT    *port_find_dlci_port (UINT8 dlci);
extern tPORT    *port_find_port (UINT8 dlci, BD_ADDR bd_addr);
extern UINT32   port_get_signal_changes (tPORT *p_port, UINT8 old_signals, UINT8 signal);
extern UINT32   port_flow_control_user (tPORT *p_port);
extern void     port_flow_control_peer(tPORT *p_port, BOOLEAN enable, UINT16 count);

/*
** Functions provided by the port_rfc.c
*/
extern int  port_open_continue (tPORT *p_port);
extern void port_start_port_open (tPORT *p_port);
extern void port_start_par_neg (tPORT *p_port);
extern void port_start_control (tPORT *p_port);
extern void port_start_close (tPORT *p_port);
extern void port_rfc_closed (tPORT *p_port, UINT8 res);

#ifdef __cplusplus
}
#endif

#endif
