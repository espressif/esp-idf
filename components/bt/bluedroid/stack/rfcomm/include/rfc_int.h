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
 *  This file contains definitions internal to the RFC unit
 *
 *****************************************************************************/

#ifndef RFC_INT_H
#define RFC_INT_H

#include "l2c_api.h"
#include "port_int.h"

/*
** Define RFCOMM result codes
*/
#define RFCOMM_SUCCESS          0
#define RFCOMM_ERROR            1
#define RFCOMM_LOW_RESOURCES    2
#define RFCOMM_TRY_LATER        3

#define RFCOMM_USER_ERR         111
#define RFCOMM_SECURITY_ERR     112

/*
** Define max and min RFCOMM MTU (N1)
*/
#define RFCOMM_MIN_MTU          23
#define RFCOMM_MAX_MTU          32767

extern void RFCOMM_StartReq (tRFC_MCB *p_mcb);
extern void RFCOMM_StartRsp (tRFC_MCB *p_mcb, UINT16 result);

extern void RFCOMM_DlcEstablishReq (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu);
extern void RFCOMM_DlcEstablishRsp (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT16 result);

extern void RFCOMM_DataReq (tRFC_MCB *p_mcb, UINT8 dlci, BT_HDR *p_buf);

extern void RFCOMM_DlcReleaseReq (tRFC_MCB *p_mcb, UINT8 dlci);

extern void RFCOMM_ParNegReq (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu);
extern void RFCOMM_ParNegRsp (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT8 cl, UINT8 k);

extern void RFCOMM_TestReq (UINT8 *p_data, UINT16 len);

#define RFCOMM_FLOW_STATE_DISABLE       0
#define RFCOMM_FLOW_STATE_ENABLE        1

extern void RFCOMM_FlowReq (tRFC_MCB *p_mcb, UINT8 dlci, UINT8 state);

extern void RFCOMM_PortNegReq (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_STATE *p_pars);
extern void RFCOMM_PortNegRsp (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_STATE *p_pars, UINT16 param_mask);

extern void RFCOMM_ControlReq (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_CTRL *p_pars);
extern void RFCOMM_ControlRsp (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_CTRL *p_pars);

extern void RFCOMM_LineStatusReq (tRFC_MCB *p_mcb, UINT8 dlci, UINT8 line_status);
/*
** Define logical struct used for sending and decoding MX frames
*/
typedef struct {
    UINT8   dlci;
    UINT8   type;
    UINT8   cr;
    UINT8   ea;
    UINT8   pf;
    UINT8   credit;

    union {
        struct {
            UINT8 dlci;
            UINT8 frame_type;
            UINT8 conv_layer;
            UINT8 priority;
            UINT8 t1;
            UINT16 mtu;
            UINT8 n2;
            UINT8 k;
        } pn;
        struct {
            UINT8   *p_data;
            UINT16  data_len;
        } test;
        struct {
            UINT8 dlci;
            UINT8 signals;
            UINT8 break_present;
            UINT8 break_duration;
        } msc;
        struct {
            UINT8 ea;
            UINT8 cr;
            UINT8 type;
        } nsc;
        struct {
            UINT8 dlci;
            UINT8 is_request;
            UINT8 baud_rate;
            UINT8 byte_size;
            UINT8 stop_bits;
            UINT8 parity;
            UINT8 parity_type;
            UINT8 fc_type;
            UINT8 xon_char;
            UINT8 xoff_char;
            UINT16 param_mask;
        } rpn;
        struct {
            UINT8 dlci;
            UINT8 line_status;
        } rls;
    } u;
} MX_FRAME;

#define LINE_STATUS_NO_ERROR       0x00
#define LINE_STATUS_OVERRUN        0x02  /* Receive Overrun Error   */
#define LINE_STATUS_RXPARITY       0x04  /* Receive Parity Error    */
#define LINE_STATUS_FRAME          0x08  /* Receive Framing error   */
#define LINE_STATUS_FAILED         0x10  /* Connection Failed       */

/*
** Define states and events for the RFC multiplexer state machine
*/
#define RFC_MX_STATE_IDLE           0
#define RFC_MX_STATE_WAIT_CONN_CNF  1
#define RFC_MX_STATE_CONFIGURE      2
#define RFC_MX_STATE_SABME_WAIT_UA  3
#define RFC_MX_STATE_WAIT_SABME     4
#define RFC_MX_STATE_CONNECTED      5
#define RFC_MX_STATE_DISC_WAIT_UA   6

/*
** Define port states
*/
#define RFC_STATE_CLOSED                0
#define RFC_STATE_SABME_WAIT_UA         1
#define RFC_STATE_ORIG_WAIT_SEC_CHECK   2
#define RFC_STATE_TERM_WAIT_SEC_CHECK   3
#define RFC_STATE_OPENED                4
#define RFC_STATE_DISC_WAIT_UA          5

/*
** Events that can be received by multiplexer as well as port state machines
*/
#define RFC_EVENT_SABME                 0
#define RFC_EVENT_UA                    1
#define RFC_EVENT_DM                    2
#define RFC_EVENT_DISC                  3
#define RFC_EVENT_UIH                   4
#define RFC_EVENT_TIMEOUT               5
#define RFC_EVENT_BAD_FRAME             50
/*
** Multiplexer events
*/
#define RFC_MX_EVENT_START_REQ          6
#define RFC_MX_EVENT_START_RSP          7
#define RFC_MX_EVENT_CLOSE_REQ          8
#define RFC_MX_EVENT_CONN_CNF           9
#define RFC_MX_EVENT_CONN_IND           10
#define RFC_MX_EVENT_CONF_CNF           11
#define RFC_MX_EVENT_CONF_IND           12
#define RFC_MX_EVENT_QOS_VIOLATION_IND  13
#define RFC_MX_EVENT_DISC_IND           14
#define RFC_MX_EVENT_TEST_CMD           15
#define RFC_MX_EVENT_TEST_RSP           16
#define RFC_MX_EVENT_FCON_CMD           17
#define RFC_MX_EVENT_FCOFF_CMD          18
#define RFC_MX_EVENT_NSC                19
#define RFC_MX_EVENT_NSC_RSP            20

/*
** Port events
*/
#define RFC_EVENT_OPEN                  9
#define RFC_EVENT_ESTABLISH_RSP         11
#define RFC_EVENT_CLOSE                 12
#define RFC_EVENT_CLEAR                 13
#define RFC_EVENT_DATA                  14
#define RFC_EVENT_SEC_COMPLETE          15

// btla-specific ++
#define RFC_T1_TIMEOUT                  20   /* seconds to wait for reply with Poll bit */
#define RFC_PORT_T1_TIMEOUT             60   /* seconds to wait for reply with Poll bit other than MX */
#define RFC_T2_TIMEOUT                  20   /* timeout to wait for Mx UIH */
// btla-specific --
#define RFC_DISC_TIMEOUT                3    /* If something goes wrong and we send DISC we should not wait for min */
#define RFC_CLOSE_TIMEOUT               10
#define RFCOMM_CONN_TIMEOUT            120   /* first connection to be established on Mx */


/* Define RFComm control block
*/
typedef struct {
    MX_FRAME  rx_frame;
    tL2CAP_APPL_INFO  reg_info;              /* L2CAP Registration info */
    tRFC_MCB *p_rfc_lcid_mcb[MAX_L2CAP_CHANNELS];     /* MCB based on the L2CAP's lcid */
    BOOLEAN   peer_rx_disabled;              /* If TRUE peer sent FCOFF */
    UINT8     last_mux;                      /* Last mux allocated */
    UINT8     last_port;                     /* Last port allocated */
} tRFCOMM_CB;

/* Main Control Block for the RFCOMM Layer (PORT and RFC) */
typedef struct {
    tRFCOMM_CB  rfc;
    tPORT_CB    port;
    UINT8       trace_level;
} tRFC_CB;


#if RFC_DYNAMIC_MEMORY == FALSE
extern tRFC_CB  rfc_cb;
#else
extern tRFC_CB *rfc_cb_ptr;
#define rfc_cb (*rfc_cb_ptr)
#endif

/* Timer running on the multiplexor channel while no DLCI connection is opened */
#define RFC_MCB_INIT_INACT_TIMER    60  /* in seconds */

/* Timer running on the multiplexor channel after last DLCI is released */
#define RFC_MCB_RELEASE_INACT_TIMER 2   /* in seconds */

/*
** Define RFCOMM frame processing errors
*/
#define RFCOMM_ERR_BAD_SABME        1
#define RFCOMM_ERR_BAD_UA           2
#define RFCOMM_ERR_BAD_DM           3
#define RFCOMM_ERR_BAD_DISC         4
#define RFCOMM_ERR_BAD_UIH          5

#ifdef RFCOMM_PRECALC_FCS

#define RFCOMM_SABME_FCS(p_data, cr, dlci) rfc_sabme_fcs[cr][dlci]
#define RFCOMM_UA_FCS(p_data, cr, dlci)    rfc_ua_fcs[cr][dlci]
#define RFCOMM_DM_FCS(p_data, cr, dlci)    rfc_dm_fcs[cr][dlci]
#define RFCOMM_DISC_FCS(p_data, cr, dlci)  rfc_disc_fcs[cr][dlci]
#define RFCOMM_UIH_FCS(p_data, dlci)       rfc_uih_fcs[dlci]

#else

extern  UINT8 rfc_calc_fcs (UINT16 len, UINT8 *p);

#define RFCOMM_SABME_FCS(p_data, cr, dlci) rfc_calc_fcs(3, p_data)
#define RFCOMM_UA_FCS(p_data, cr, dlci)    rfc_calc_fcs(3, p_data)
#define RFCOMM_DM_FCS(p_data, cr, dlci)    rfc_calc_fcs(3, p_data)
#define RFCOMM_DISC_FCS(p_data, cr, dlci)  rfc_calc_fcs(3, p_data)
#define RFCOMM_UIH_FCS(p_data, dlci)       rfc_calc_fcs(2, p_data)

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void rfc_mx_sm_execute (tRFC_MCB *p_mcb, UINT16 event, void *p_data);

/*
** Functions provided by the rfc_port_fsm.c
*/
extern void rfc_port_sm_execute (tPORT *p_port, UINT16 event, void *p_data);


extern void rfc_process_pn (tRFC_MCB *p_rfc_mcb, BOOLEAN is_command, MX_FRAME *p_frame);
extern void rfc_process_msc (tRFC_MCB *p_rfc_mcb, BOOLEAN is_command, MX_FRAME *p_frame);
extern void rfc_process_rpn (tRFC_MCB *p_rfc_mcb, BOOLEAN is_command, BOOLEAN is_request, MX_FRAME *p_frame);
extern void rfc_process_rls (tRFC_MCB *p_rfc_mcb, BOOLEAN is_command, MX_FRAME *p_frame);
extern void rfc_process_nsc (tRFC_MCB *p_rfc_mcb, MX_FRAME *p_frame);
extern void rfc_process_test_rsp (tRFC_MCB *p_rfc_mcb, BT_HDR *p_buf);
extern void rfc_process_fcon (tRFC_MCB *p_rfc_mcb, BOOLEAN is_command);
extern void rfc_process_fcoff (tRFC_MCB *p_rfc_mcb, BOOLEAN is_command);
extern void rfc_process_l2cap_congestion (tRFC_MCB *p_mcb, BOOLEAN is_congested);

/*
** Functions provided by the rfc_utils.c
*/
tRFC_MCB  *rfc_alloc_multiplexer_channel (BD_ADDR bd_addr, BOOLEAN is_initiator);
extern void      rfc_release_multiplexer_channel (tRFC_MCB *p_rfc_mcb);
extern void      rfc_timer_start (tRFC_MCB *p_rfc_mcb, UINT16 timeout);
extern void      rfc_timer_stop (tRFC_MCB *p_rfc_mcb);
extern void      rfc_timer_free (tRFC_MCB *p_rfc_mcb);
extern void      rfc_port_timer_start (tPORT *p_port, UINT16 tout);
extern void      rfc_port_timer_stop (tPORT *p_port);
extern void      rfc_port_timer_free (tPORT *p_port);

BOOLEAN   rfc_check_uih_fcs (UINT8 dlci, UINT8 received_fcs);
BOOLEAN   rfc_check_fcs (UINT16 len, UINT8 *p, UINT8 received_fcs);
tRFC_MCB  *rfc_find_lcid_mcb (UINT16 lcid);
extern void      rfc_save_lcid_mcb (tRFC_MCB *p_rfc_mcb, UINT16 lcid);
extern void      rfc_check_mcb_active (tRFC_MCB *p_mcb);
extern void      rfc_port_closed (tPORT *p_port);
extern void      rfc_sec_check_complete (BD_ADDR bd_addr, tBT_TRANSPORT transport, void *p_ref_data, UINT8 res);
extern void      rfc_inc_credit (tPORT *p_port, UINT8 credit);
extern void      rfc_dec_credit (tPORT *p_port);
extern void      rfc_check_send_cmd(tRFC_MCB *p_mcb, BT_HDR *p_buf);

/*
** Functions provided by the rfc_ts_frames.c
*/
extern void     rfc_send_sabme (tRFC_MCB *p_rfc_mcb, UINT8 dlci);
extern void     rfc_send_ua (tRFC_MCB *p_rfc_mcb, UINT8 dlci);
extern void     rfc_send_dm (tRFC_MCB *p_rfc_mcb, UINT8 dlci, BOOLEAN pf);
extern void     rfc_send_disc (tRFC_MCB *p_rfc_mcb, UINT8 dlci);
extern void     rfc_send_pn (tRFC_MCB *p_mcb, UINT8 dlci, BOOLEAN is_command, UINT16 mtu,
                             UINT8 cl, UINT8 k);
extern void     rfc_send_test (tRFC_MCB *p_rfc_mcb, BOOLEAN is_command, BT_HDR *p_buf);
extern void     rfc_send_msc (tRFC_MCB *p_mcb, UINT8 dlci, BOOLEAN is_command,
                              tPORT_CTRL *p_pars);
extern void     rfc_send_rls (tRFC_MCB *p_mcb, UINT8 dlci, BOOLEAN is_command, UINT8 status);
extern void     rfc_send_rpn (tRFC_MCB *p_mcb, UINT8 dlci, BOOLEAN is_command,
                              tPORT_STATE *p_pars, UINT16 mask);
extern void     rfc_send_fcon (tRFC_MCB *p_mcb, BOOLEAN is_command);
extern void     rfc_send_fcoff (tRFC_MCB *p_mcb, BOOLEAN is_command);
extern void     rfc_send_buf_uih (tRFC_MCB *p_rfc_mcb, UINT8 dlci, BT_HDR *p_buf);
extern void     rfc_send_credit(tRFC_MCB *p_mcb, UINT8 dlci, UINT8 credit);
extern void     rfc_process_mx_message (tRFC_MCB *p_rfc_mcb, BT_HDR *p_buf);
extern UINT8    rfc_parse_data (tRFC_MCB *p_rfc_mcb, MX_FRAME *p_frame, BT_HDR *p_buf);

/*
** Functions provided by the rfc_disp.c
*/



/* Call back functions from RFCOMM */
extern void rfcomm_l2cap_if_init (void);

extern void PORT_StartInd (tRFC_MCB *p_mcb);
extern void PORT_StartCnf (tRFC_MCB *p_mcb, UINT16 result);

extern void PORT_CloseInd (tRFC_MCB *p_mcb);
extern void Port_TimeOutCloseMux (tRFC_MCB *p_mcb);

extern void PORT_DlcEstablishInd (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu);
extern void PORT_DlcEstablishCnf (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT16 result);

extern void PORT_DataInd (tRFC_MCB *p_mcb, UINT8 dlci, BT_HDR *p_buf);

extern void PORT_DlcReleaseInd (tRFC_MCB *p_mcb, UINT8 dlci);

extern void PORT_ParNegInd (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT8 cl, UINT8 k);
extern void PORT_ParNegCnf (tRFC_MCB *p_mcb, UINT8 dlci, UINT16 mtu, UINT8 cl, UINT8 k);

extern void PORT_TestCnf (tRFC_MCB *p_mcb, UINT8 *p_data, UINT16 len);

extern void PORT_FlowInd (tRFC_MCB *p_mcb, UINT8 dlci, BOOLEAN fc);

extern void PORT_PortNegInd (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_STATE *p_pars, UINT16 param_mask);
extern void PORT_PortNegCnf (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_STATE *p_pars, UINT16 result);

extern void PORT_ControlInd (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_CTRL *p_pars);
extern void PORT_ControlCnf (tRFC_MCB *p_mcb, UINT8 dlci, tPORT_CTRL *p_pars);

extern void PORT_LineStatusInd (tRFC_MCB *p_mcb, UINT8 dlci, UINT8 line_status);

#ifdef __cplusplus
}
#endif

#endif
