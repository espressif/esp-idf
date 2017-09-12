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
 *  this file contains the L2CAP API definitions
 *
 ******************************************************************************/
#ifndef L2C_API_H
#define L2C_API_H

#include <stdbool.h>

#include "bt_target.h"
#include "l2cdefs.h"
#include "hcidefs.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

/* Define the minimum offset that L2CAP needs in a buffer. This is made up of
** HCI type(1), len(2), handle(2), L2CAP len(2) and CID(2) => 9
*/
#define L2CAP_MIN_OFFSET    13     /* plus control(2), SDU length(2) */

/* Minimum offset for broadcast needs another two bytes for the PSM */
#define L2CAP_BCST_MIN_OFFSET       11

/* ping result codes */
#define L2CAP_PING_RESULT_OK        0       /* Ping reply received OK     */
#define L2CAP_PING_RESULT_NO_LINK   1       /* Link could not be setup    */
#define L2CAP_PING_RESULT_NO_RESP   2       /* Remote L2CAP did not reply */

/* result code for L2CA_DataWrite() */
#define L2CAP_DW_FAILED        FALSE
#define L2CAP_DW_SUCCESS       TRUE
#define L2CAP_DW_CONGESTED     2

/* Values for priority parameter to L2CA_SetAclPriority */
#define L2CAP_PRIORITY_NORMAL       0
#define L2CAP_PRIORITY_HIGH         1

/* Values for priority parameter to L2CA_SetTxPriority */
#define L2CAP_CHNL_PRIORITY_HIGH    0
#define L2CAP_CHNL_PRIORITY_MEDIUM  1
#define L2CAP_CHNL_PRIORITY_LOW     2

typedef UINT8 tL2CAP_CHNL_PRIORITY;

/* Values for Tx/Rx data rate parameter to L2CA_SetChnlDataRate */
#define L2CAP_CHNL_DATA_RATE_HIGH       3
#define L2CAP_CHNL_DATA_RATE_MEDIUM     2
#define L2CAP_CHNL_DATA_RATE_LOW        1
#define L2CAP_CHNL_DATA_RATE_NO_TRAFFIC 0

typedef UINT8 tL2CAP_CHNL_DATA_RATE;

/* Data Packet Flags  (bits 2-15 are reserved) */
/* layer specific 14-15 bits are used for FCR SAR */
#define L2CAP_FLUSHABLE_MASK        0x0003
#define L2CAP_FLUSHABLE_CH_BASED    0x0000
#define L2CAP_FLUSHABLE_PKT         0x0001
#define L2CAP_NON_FLUSHABLE_PKT     0x0002


/* L2CA_FlushChannel num_to_flush definitions */
#define L2CAP_FLUSH_CHANS_ALL       0xffff
#define L2CAP_FLUSH_CHANS_GET       0x0000


/* special CID for Multi-AV for reporting congestion */
#define L2CAP_MULTI_AV_CID          0

/* length of the HCI header block */
/* HCI header(4) + SNK count(1) + FCR bits(1) + AV data length(2) */
#define L2CAP_MULTI_AV_HCI_HDR_LEN  8

/* length of padding for 4 bytes align */
#define L2CAP_MULTI_AV_PADDING_LEN  2

/* length of the HCI header block with padding for FCR */
/* HCI header(4) + SNK count(1) + FCR bits(1) + AV data length(2) + padding(2) */
#define L2CAP_MULTI_AV_HCI_HDR_LEN_WITH_PADDING 10

/* length of the L2CAP header block */
/* HCI header(4) + L2CAP header(4) + padding(4) or control word(2) + FCS(2) */
#define L2CAP_MULTI_AV_L2C_HDR_LEN  12

/* definition used for L2CA_SetDesireRole */
#define L2CAP_ROLE_SLAVE            HCI_ROLE_SLAVE
#define L2CAP_ROLE_MASTER           HCI_ROLE_MASTER
#define L2CAP_ROLE_ALLOW_SWITCH     0x80    /* set this bit to allow switch at create conn */
#define L2CAP_ROLE_DISALLOW_SWITCH  0x40    /* set this bit to disallow switch at create conn */
#define L2CAP_ROLE_CHECK_SWITCH     0xC0


/* Values for 'allowed_modes' field passed in structure tL2CAP_ERTM_INFO
*/
#define L2CAP_FCR_CHAN_OPT_BASIC    (1 << L2CAP_FCR_BASIC_MODE)
#define L2CAP_FCR_CHAN_OPT_ERTM     (1 << L2CAP_FCR_ERTM_MODE)
#define L2CAP_FCR_CHAN_OPT_STREAM   (1 << L2CAP_FCR_STREAM_MODE)

#define L2CAP_FCR_CHAN_OPT_ALL_MASK (L2CAP_FCR_CHAN_OPT_BASIC | L2CAP_FCR_CHAN_OPT_ERTM | L2CAP_FCR_CHAN_OPT_STREAM)

/* Validity check for PSM.  PSM values must be odd.  Also, all PSM values must
** be assigned such that the least significant bit of the most sigificant
** octet equals zero.
*/
#define L2C_INVALID_PSM(psm)    (((psm) & 0x0101) != 0x0001)
#define L2C_IS_VALID_PSM(psm)   (((psm) & 0x0101) == 0x0001)

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

typedef struct {
#define L2CAP_FCR_BASIC_MODE    0x00
#define L2CAP_FCR_ERTM_MODE     0x03
#define L2CAP_FCR_STREAM_MODE   0x04

    UINT8  mode;

    UINT8  tx_win_sz;
    UINT8  max_transmit;
    UINT16 rtrans_tout;
    UINT16 mon_tout;
    UINT16 mps;
} tL2CAP_FCR_OPTS;

/* Define a structure to hold the configuration parameters. Since the
** parameters are optional, for each parameter there is a boolean to
** use to signify its presence or absence.
*/
typedef struct {
    UINT16      result;                 /* Only used in confirm messages */
    BOOLEAN     mtu_present;
    UINT16      mtu;
    BOOLEAN     qos_present;
    FLOW_SPEC   qos;
    BOOLEAN     flush_to_present;
    UINT16      flush_to;
    BOOLEAN     fcr_present;
    tL2CAP_FCR_OPTS fcr;
    BOOLEAN     fcs_present;            /* Optionally bypasses FCS checks */
    UINT8       fcs;                    /* '0' if desire is to bypass FCS, otherwise '1' */
    BOOLEAN               ext_flow_spec_present;
    tHCI_EXT_FLOW_SPEC    ext_flow_spec;
    UINT16      flags;                  /* bit 0: 0-no continuation, 1-continuation */
} tL2CAP_CFG_INFO;

/* L2CAP channel configured field bitmap */
#define L2CAP_CH_CFG_MASK_MTU           0x0001
#define L2CAP_CH_CFG_MASK_QOS           0x0002
#define L2CAP_CH_CFG_MASK_FLUSH_TO      0x0004
#define L2CAP_CH_CFG_MASK_FCR           0x0008
#define L2CAP_CH_CFG_MASK_FCS           0x0010
#define L2CAP_CH_CFG_MASK_EXT_FLOW_SPEC 0x0020

typedef UINT16 tL2CAP_CH_CFG_BITS;

/*********************************
**  Callback Functions Prototypes
**********************************/

/* Connection indication callback prototype. Parameters are
**              BD Address of remote
**              Local CID assigned to the connection
**              PSM that the remote wants to connect to
**              Identifier that the remote sent
*/
typedef void (tL2CA_CONNECT_IND_CB) (BD_ADDR, UINT16, UINT16, UINT8);


/* Connection confirmation callback prototype. Parameters are
**              Local CID
**              Result - 0 = connected, non-zero means failure reason
*/
typedef void (tL2CA_CONNECT_CFM_CB) (UINT16, UINT16);


/* Connection pending callback prototype. Parameters are
**              Local CID
*/
typedef void (tL2CA_CONNECT_PND_CB) (UINT16);


/* Configuration indication callback prototype. Parameters are
**              Local CID assigned to the connection
**              Pointer to configuration info
*/
typedef void (tL2CA_CONFIG_IND_CB) (UINT16, tL2CAP_CFG_INFO *);


/* Configuration confirm callback prototype. Parameters are
**              Local CID assigned to the connection
**              Pointer to configuration info
*/
typedef void (tL2CA_CONFIG_CFM_CB) (UINT16, tL2CAP_CFG_INFO *);


/* Disconnect indication callback prototype. Parameters are
**              Local CID
**              Boolean whether upper layer should ack this
*/
typedef void (tL2CA_DISCONNECT_IND_CB) (UINT16, BOOLEAN);


/* Disconnect confirm callback prototype. Parameters are
**              Local CID
**              Result
*/
typedef void (tL2CA_DISCONNECT_CFM_CB) (UINT16, UINT16);


/* QOS Violation indication callback prototype. Parameters are
**              BD Address of violating device
*/
typedef void (tL2CA_QOS_VIOLATION_IND_CB) (BD_ADDR);


/* Data received indication callback prototype. Parameters are
**              Local CID
**              Address of buffer
*/
typedef void (tL2CA_DATA_IND_CB) (UINT16, BT_HDR *);


/* Echo response callback prototype. Note that this is not included in the
** registration information, but is passed to L2CAP as part of the API to
** actually send an echo request. Parameters are
**              Result
*/
typedef void (tL2CA_ECHO_RSP_CB) (UINT16);


/* Callback function prototype to pass broadcom specific echo response  */
/* to the upper layer                                                   */
typedef void (tL2CA_ECHO_DATA_CB) (BD_ADDR, UINT16, UINT8 *);


/* Congestion status callback protype. This callback is optional. If
** an application tries to send data when the transmit queue is full,
** the data will anyways be dropped. The parameter is:
**              Local CID
**              TRUE if congested, FALSE if uncongested
*/
typedef void (tL2CA_CONGESTION_STATUS_CB) (UINT16, BOOLEAN);

/* Callback prototype for number of packets completed events.
** This callback notifies the application when Number of Completed Packets
** event has been received.
** This callback is originally designed for 3DG devices.
** The parameter is:
**          peer BD_ADDR
*/
typedef void (tL2CA_NOCP_CB) (BD_ADDR);

/* Transmit complete callback protype. This callback is optional. If
** set, L2CAP will call it when packets are sent or flushed. If the
** count is 0xFFFF, it means all packets are sent for that CID (eRTM
** mode only). The parameters are:
**              Local CID
**              Number of SDUs sent or dropped
*/
typedef void (tL2CA_TX_COMPLETE_CB) (UINT16, UINT16);

/* Define the structure that applications use to register with
** L2CAP. This structure includes callback functions. All functions
** MUST be provided, with the exception of the "connect pending"
** callback and "congestion status" callback.
*/
typedef struct {
    tL2CA_CONNECT_IND_CB        *pL2CA_ConnectInd_Cb;
    tL2CA_CONNECT_CFM_CB        *pL2CA_ConnectCfm_Cb;
    tL2CA_CONNECT_PND_CB        *pL2CA_ConnectPnd_Cb;
    tL2CA_CONFIG_IND_CB         *pL2CA_ConfigInd_Cb;
    tL2CA_CONFIG_CFM_CB         *pL2CA_ConfigCfm_Cb;
    tL2CA_DISCONNECT_IND_CB     *pL2CA_DisconnectInd_Cb;
    tL2CA_DISCONNECT_CFM_CB     *pL2CA_DisconnectCfm_Cb;
    tL2CA_QOS_VIOLATION_IND_CB  *pL2CA_QoSViolationInd_Cb;
    tL2CA_DATA_IND_CB           *pL2CA_DataInd_Cb;
    tL2CA_CONGESTION_STATUS_CB  *pL2CA_CongestionStatus_Cb;
    tL2CA_TX_COMPLETE_CB        *pL2CA_TxComplete_Cb;

} tL2CAP_APPL_INFO;

/* Define the structure that applications use to create or accept
** connections with enhanced retransmission mode.
*/
typedef struct {
    UINT8       preferred_mode;
    UINT8       allowed_modes;
    UINT16      user_rx_buf_size;
    UINT16      user_tx_buf_size;
    UINT16      fcr_rx_buf_size;
    UINT16      fcr_tx_buf_size;

} tL2CAP_ERTM_INFO;

#define L2CA_REGISTER(a,b,c)        L2CA_Register(a,(tL2CAP_APPL_INFO *)b)
#define L2CA_DEREGISTER(a)          L2CA_Deregister(a)
#define L2CA_CONNECT_REQ(a,b,c,d)   L2CA_ErtmConnectReq(a,b,c)
#define L2CA_CONNECT_RSP(a,b,c,d,e,f,g) L2CA_ErtmConnectRsp(a,b,c,d,e,f)
#define L2CA_CONFIG_REQ(a,b)        L2CA_ConfigReq(a,b)
#define L2CA_CONFIG_RSP(a,b)        L2CA_ConfigRsp(a,b)
#define L2CA_DISCONNECT_REQ(a)      L2CA_DisconnectReq(a)
#define L2CA_DISCONNECT_RSP(a)      L2CA_DisconnectRsp(a)
#define L2CA_DATA_WRITE(a, b)       L2CA_DataWrite(a, b)

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         L2CA_Register
**
** Description      Other layers call this function to register for L2CAP
**                  services.
**
** Returns          PSM to use or zero if error. Typically, the PSM returned
**                  is the same as was passed in, but for an outgoing-only
**                  connection to a dynamic PSM, a "virtual" PSM is returned
**                  and should be used in the calls to L2CA_ConnectReq() and
**                  BTM_SetSecurityLevel().
**
*******************************************************************************/
extern UINT16 L2CA_Register (UINT16 psm, tL2CAP_APPL_INFO *p_cb_info);

/*******************************************************************************
**
** Function         L2CA_Deregister
**
** Description      Other layers call this function to deregister for L2CAP
**                  services.
**
** Returns          void
**
*******************************************************************************/
extern void L2CA_Deregister (UINT16 psm);

/*******************************************************************************
**
** Function         L2CA_AllocatePSM
**
** Description      Other layers call this function to find an unused PSM for L2CAP
**                  services.
**
** Returns          PSM to use.
**
*******************************************************************************/
extern UINT16 L2CA_AllocatePSM(void);

/*******************************************************************************
**
** Function         L2CA_ConnectReq
**
** Description      Higher layers call this function to create an L2CAP connection.
**                  Note that the connection is not established at this time, but
**                  connection establishment gets started. The callback function
**                  will be invoked when connection establishes or fails.
**
** Returns          the CID of the connection, or 0 if it failed to start
**
*******************************************************************************/
extern UINT16 L2CA_ConnectReq (UINT16 psm, BD_ADDR p_bd_addr);

/*******************************************************************************
**
** Function         L2CA_ConnectRsp
**
** Description      Higher layers call this function to accept an incoming
**                  L2CAP connection, for which they had gotten an connect
**                  indication callback.
**
** Returns          TRUE for success, FALSE for failure
**
*******************************************************************************/
extern BOOLEAN L2CA_ConnectRsp (BD_ADDR p_bd_addr, UINT8 id, UINT16 lcid,
                                UINT16 result, UINT16 status);

/*******************************************************************************
**
** Function         L2CA_ErtmConnectReq
**
** Description      Higher layers call this function to create an L2CAP connection
**                  that needs to use Enhanced Retransmission Mode.
**                  Note that the connection is not established at this time, but
**                  connection establishment gets started. The callback function
**                  will be invoked when connection establishes or fails.
**
** Returns          the CID of the connection, or 0 if it failed to start
**
*******************************************************************************/
extern UINT16 L2CA_ErtmConnectReq (UINT16 psm, BD_ADDR p_bd_addr,
                                   tL2CAP_ERTM_INFO *p_ertm_info);

// This function sets the callback routines for the L2CAP connection referred to by
// |local_cid|. The callback routines can only be modified for outgoing connections
// established by |L2CA_ConnectReq| or accepted incoming connections. |callbacks|
// must not be NULL. This function returns true if the callbacks could be updated,
// false if not (e.g. |local_cid| was not found).
bool L2CA_SetConnectionCallbacks(uint16_t local_cid, const tL2CAP_APPL_INFO *callbacks);

/*******************************************************************************
**
** Function         L2CA_ErtmConnectRsp
**
** Description      Higher layers call this function to accept an incoming
**                  L2CAP connection, for which they had gotten an connect
**                  indication callback, and for which the higher layer wants
**                  to use Enhanced Retransmission Mode.
**
** Returns          TRUE for success, FALSE for failure
**
*******************************************************************************/
extern BOOLEAN  L2CA_ErtmConnectRsp (BD_ADDR p_bd_addr, UINT8 id, UINT16 lcid,
                                     UINT16 result, UINT16 status,
                                     tL2CAP_ERTM_INFO *p_ertm_info);

/*******************************************************************************
**
** Function         L2CA_ConfigReq
**
** Description      Higher layers call this function to send configuration.
**
** Returns          TRUE if configuration sent, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_ConfigReq (UINT16 cid, tL2CAP_CFG_INFO *p_cfg);

/*******************************************************************************
**
** Function         L2CA_ConfigRsp
**
** Description      Higher layers call this function to send a configuration
**                  response.
**
** Returns          TRUE if configuration response sent, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_ConfigRsp (UINT16 cid, tL2CAP_CFG_INFO *p_cfg);

/*******************************************************************************
**
** Function         L2CA_DisconnectReq
**
** Description      Higher layers call this function to disconnect a channel.
**
** Returns          TRUE if disconnect sent, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_DisconnectReq (UINT16 cid);

/*******************************************************************************
**
** Function         L2CA_DisconnectRsp
**
** Description      Higher layers call this function to acknowledge the
**                  disconnection of a channel.
**
** Returns          void
**
*******************************************************************************/
extern BOOLEAN L2CA_DisconnectRsp (UINT16 cid);
#endif  ///CLASSIC_BT_INCLUDED == TRUE


/*******************************************************************************
**
** Function         L2CA_DataWrite
**
** Description      Higher layers call this function to write data.
**
** Returns          L2CAP_DW_SUCCESS, if data accepted, else FALSE
**                  L2CAP_DW_CONGESTED, if data accepted and the channel is congested
**                  L2CAP_DW_FAILED, if error
**
*******************************************************************************/
extern UINT8 L2CA_DataWrite (UINT16 cid, BT_HDR *p_data);

#if (CLASSIC_BT_INCLUDED == TRUE)

/*******************************************************************************
**
** Function         L2CA_Ping
**
** Description      Higher layers call this function to send an echo request.
**
** Returns          TRUE if echo request sent, else FALSE.
**
*******************************************************************************/
extern BOOLEAN L2CA_Ping (BD_ADDR p_bd_addr, tL2CA_ECHO_RSP_CB *p_cb);

/*******************************************************************************
**
** Function         L2CA_Echo
**
** Description      Higher layers call this function to send an echo request
**                  with application-specific data.
**
** Returns          TRUE if echo request sent, else FALSE.
**
*******************************************************************************/
extern BOOLEAN  L2CA_Echo (BD_ADDR p_bd_addr, BT_HDR *p_data, tL2CA_ECHO_DATA_CB *p_callback);
#endif  ///CLASSIC_BT_INCLUDED == TRUE


// Given a local channel identifier, |lcid|, this function returns the bound remote
// channel identifier, |rcid|, and the ACL link handle, |handle|. If |lcid| is not
// known or is invalid, this function returns false and does not modify the values
// pointed at by |rcid| and |handle|. |rcid| and |handle| may be NULL.
bool L2CA_GetIdentifiers(uint16_t lcid, uint16_t *rcid, uint16_t *handle);

/*******************************************************************************
**
** Function         L2CA_SetIdleTimeout
**
** Description      Higher layers call this function to set the idle timeout for
**                  a connection, or for all future connections. The "idle timeout"
**                  is the amount of time that a connection can remain up with
**                  no L2CAP channels on it. A timeout of zero means that the
**                  connection will be torn down immediately when the last channel
**                  is removed. A timeout of 0xFFFF means no timeout. Values are
**                  in seconds.
**
** Returns          TRUE if command succeeded, FALSE if failed
**
*******************************************************************************/
extern BOOLEAN L2CA_SetIdleTimeout (UINT16 cid, UINT16 timeout,
                                    BOOLEAN is_global);


/*******************************************************************************
**
** Function         L2CA_SetIdleTimeoutByBdAddr
**
** Description      Higher layers call this function to set the idle timeout for
**                  a connection. The "idle timeout" is the amount of time that
**                  a connection can remain up with no L2CAP channels on it.
**                  A timeout of zero means that the connection will be torn
**                  down immediately when the last channel is removed.
**                  A timeout of 0xFFFF means no timeout. Values are in seconds.
**                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY,
**                  then the idle timeouts for all active l2cap links will be
**                  changed.
**
** Returns          TRUE if command succeeded, FALSE if failed
**
** NOTE             This timeout applies to all logical channels active on the
**                  ACL link.
*******************************************************************************/
extern BOOLEAN L2CA_SetIdleTimeoutByBdAddr(BD_ADDR bd_addr, UINT16 timeout,
        tBT_TRANSPORT transport);


/*******************************************************************************
**
** Function         L2CA_SetTraceLevel
**
** Description      This function sets the trace level for L2CAP. If called with
**                  a value of 0xFF, it simply reads the current trace level.
**
** Returns          the new (current) trace level
**
*******************************************************************************/
extern UINT8 L2CA_SetTraceLevel (UINT8 trace_level);


/*******************************************************************************
**
** Function     L2CA_SetDesireRole
**
** Description  This function sets the desire role for L2CAP.
**              If the new role is L2CAP_ROLE_ALLOW_SWITCH, allow switch on
**              HciCreateConnection.
**              If the new role is L2CAP_ROLE_DISALLOW_SWITCH, do not allow switch on
**              HciCreateConnection.
**
**              If the new role is a valid role (HCI_ROLE_MASTER or HCI_ROLE_SLAVE),
**              the desire role is set to the new value. Otherwise, it is not changed.
**
** Returns      the new (current) role
**
*******************************************************************************/
extern UINT8 L2CA_SetDesireRole (UINT8 new_role);
#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function     L2CA_LocalLoopbackReq
**
** Description  This function sets up a CID for local loopback
**
** Returns      CID of 0 if none.
**
*******************************************************************************/
extern UINT16 L2CA_LocalLoopbackReq (UINT16 psm, UINT16 handle, BD_ADDR p_bd_addr);

/*******************************************************************************
**
** Function     L2CA_FlushChannel
**
** Description  This function flushes none, some or all buffers queued up
**              for xmission for a particular CID. If called with
**              L2CAP_FLUSH_CHANS_GET (0), it simply returns the number
**              of buffers queued for that CID L2CAP_FLUSH_CHANS_ALL (0xffff)
**              flushes all buffers.  All other values specifies the maximum
**              buffers to flush.
**
** Returns      Number of buffers left queued for that CID
**
*******************************************************************************/
extern UINT16   L2CA_FlushChannel (UINT16 lcid, UINT16 num_to_flush);


/*******************************************************************************
**
** Function         L2CA_SetAclPriority
**
** Description      Sets the transmission priority for an ACL channel.
**                  (For initial implementation only two values are valid.
**                  L2CAP_PRIORITY_NORMAL and L2CAP_PRIORITY_HIGH).
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_SetAclPriority (BD_ADDR bd_addr, UINT8 priority);

/*******************************************************************************
**
** Function         L2CA_FlowControl
**
** Description      Higher layers call this function to flow control a channel.
**
**                  data_enabled - TRUE data flows, FALSE data is stopped
**
** Returns          TRUE if valid channel, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_FlowControl (UINT16 cid, BOOLEAN data_enabled);

/*******************************************************************************
**
** Function         L2CA_SendTestSFrame
**
** Description      Higher layers call this function to send a test S-frame.
**
** Returns          TRUE if valid Channel, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_SendTestSFrame (UINT16 cid, UINT8 sup_type,
                                    UINT8 back_track);

/*******************************************************************************
**
** Function         L2CA_SetTxPriority
**
** Description      Sets the transmission priority for a channel. (FCR Mode)
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_SetTxPriority (UINT16 cid, tL2CAP_CHNL_PRIORITY priority);

/*******************************************************************************
**
** Function         L2CA_RegForNoCPEvt
**
** Description      Register callback for Number of Completed Packets event.
**
** Input Param      p_cb - callback for Number of completed packets event
**                  p_bda - BT address of remote device
**
** Returns
**
*******************************************************************************/
extern BOOLEAN L2CA_RegForNoCPEvt(tL2CA_NOCP_CB *p_cb, BD_ADDR p_bda);

/*******************************************************************************
**
** Function         L2CA_SetChnlDataRate
**
** Description      Sets the tx/rx data rate for a channel.
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_SetChnlDataRate (UINT16 cid, tL2CAP_CHNL_DATA_RATE tx, tL2CAP_CHNL_DATA_RATE rx);

typedef void (tL2CA_RESERVE_CMPL_CBACK) (void);

/*******************************************************************************
**
** Function         L2CA_SetFlushTimeout
**
** Description      This function set the automatic flush time out in Baseband
**                  for ACL-U packets.
**                  BdAddr : the remote BD address of ACL link. If it is BT_DB_ANY
**                           then the flush time out will be applied to all ACL link.
**                  FlushTimeout: flush time out in ms
**                           0x0000 : No automatic flush
**                           L2CAP_NO_RETRANSMISSION : No retransmission
**                           0x0002 - 0xFFFE : flush time out, if (flush_tout*8)+3/5)
**                                    <= HCI_MAX_AUTO_FLUSH_TOUT (in 625us slot).
**                                    Otherwise, return FALSE.
**                           L2CAP_NO_AUTOMATIC_FLUSH : No automatic flush
**
** Returns          TRUE if command succeeded, FALSE if failed
**
** NOTE             This flush timeout applies to all logical channels active on the
**                  ACL link.
*******************************************************************************/
extern BOOLEAN L2CA_SetFlushTimeout (BD_ADDR bd_addr, UINT16 flush_tout);
#endif  ///CLASSIC_BT_INCLUDED == TRUE

/*******************************************************************************
**
** Function         L2CA_DataWriteEx
**
** Description      Higher layers call this function to write data with extended
**                  flags.
**                  flags : L2CAP_FLUSHABLE_CH_BASED
**                          L2CAP_FLUSHABLE_PKT
**                          L2CAP_NON_FLUSHABLE_PKT
**
** Returns          L2CAP_DW_SUCCESS, if data accepted, else FALSE
**                  L2CAP_DW_CONGESTED, if data accepted and the channel is congested
**                  L2CAP_DW_FAILED, if error
**
*******************************************************************************/
extern UINT8 L2CA_DataWriteEx (UINT16 cid, BT_HDR *p_data, UINT16 flags);

/*******************************************************************************
**
** Function         L2CA_SetChnlFlushability
**
** Description      Higher layers call this function to set a channels
**                  flushability flags
**
** Returns          TRUE if CID found, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_SetChnlFlushability (UINT16 cid, BOOLEAN is_flushable);

/*******************************************************************************
**
**  Function         L2CA_GetPeerFeatures
**
**  Description      Get a peers features and fixed channel map
**
**  Parameters:      BD address of the peer
**                   Pointers to features and channel mask storage area
**
**  Return value:    TRUE if peer is connected
**
*******************************************************************************/
extern BOOLEAN L2CA_GetPeerFeatures (BD_ADDR bd_addr, UINT32 *p_ext_feat, UINT8 *p_chnl_mask);

/*******************************************************************************
**
**  Function         L2CA_GetBDAddrbyHandle
**
**  Description      Get BD address for the given HCI handle
**
**  Parameters:      HCI handle
**                   BD address of the peer
**
**  Return value:    TRUE if found lcb for the given handle, FALSE otherwise
**
*******************************************************************************/
extern BOOLEAN L2CA_GetBDAddrbyHandle (UINT16 handle, BD_ADDR bd_addr);

#if (CLASSIC_BT_INCLUDED == TRUE)

/*******************************************************************************
**
**  Function         L2CA_GetChnlFcrMode
**
**  Description      Get the channel FCR mode
**
**  Parameters:      Local CID
**
**  Return value:    Channel mode
**
*******************************************************************************/
extern UINT8 L2CA_GetChnlFcrMode (UINT16 lcid);
#endif  ///CLASSIC_BT_INCLUDED == TRUE


/*******************************************************************************
**
**                      UCD callback prototypes
**
*******************************************************************************/

/* UCD discovery. Parameters are
**      BD Address of remote
**      Data Type
**      Data
*/
#define L2CAP_UCD_INFO_TYPE_RECEPTION   0x01
#define L2CAP_UCD_INFO_TYPE_MTU         0x02

typedef void (tL2CA_UCD_DISCOVER_CB) (BD_ADDR, UINT8, UINT32);

/* UCD data received. Parameters are
**      BD Address of remote
**      Pointer to buffer with data
*/
typedef void (tL2CA_UCD_DATA_CB) (BD_ADDR, BT_HDR *);

/* Congestion status callback protype. This callback is optional. If
** an application tries to send data when the transmit queue is full,
** the data will anyways be dropped. The parameter is:
**              remote BD_ADDR
**              TRUE if congested, FALSE if uncongested
*/
typedef void (tL2CA_UCD_CONGESTION_STATUS_CB) (BD_ADDR, BOOLEAN);

/* UCD registration info (the callback addresses and PSM)
*/
typedef struct {
    tL2CA_UCD_DISCOVER_CB           *pL2CA_UCD_Discover_Cb;
    tL2CA_UCD_DATA_CB               *pL2CA_UCD_Data_Cb;
    tL2CA_UCD_CONGESTION_STATUS_CB  *pL2CA_UCD_Congestion_Status_Cb;
} tL2CAP_UCD_CB_INFO;

/*******************************************************************************
**
**  Function        L2CA_UcdRegister
**
**  Description     Register PSM on UCD.
**
**  Parameters:     tL2CAP_UCD_CB_INFO
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
extern BOOLEAN L2CA_UcdRegister ( UINT16 psm, tL2CAP_UCD_CB_INFO *p_cb_info );

/*******************************************************************************
**
**  Function        L2CA_UcdDeregister
**
**  Description     Deregister PSM on UCD.
**
**  Parameters:     PSM
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
extern BOOLEAN L2CA_UcdDeregister ( UINT16 psm );

/*******************************************************************************
**
**  Function        L2CA_UcdDiscover
**
**  Description     Discover UCD of remote device.
**
**  Parameters:     PSM
**                  BD_ADDR of remote device
**                  info_type : L2CAP_UCD_INFO_TYPE_RECEPTION
**                              L2CAP_UCD_INFO_TYPE_MTU
**
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
extern BOOLEAN L2CA_UcdDiscover ( UINT16 psm, BD_ADDR rem_bda, UINT8 info_type );

/*******************************************************************************
**
**  Function        L2CA_UcdDataWrite
**
**  Description     Send UCD to remote device
**
**  Parameters:     PSM
**                  BD Address of remote
**                  Pointer to buffer of type BT_HDR
**                  flags : L2CAP_FLUSHABLE_CH_BASED
**                          L2CAP_FLUSHABLE_PKT
**                          L2CAP_NON_FLUSHABLE_PKT
**
** Return value     L2CAP_DW_SUCCESS, if data accepted
**                  L2CAP_DW_FAILED,  if error
**
*******************************************************************************/
extern UINT16 L2CA_UcdDataWrite (UINT16 psm, BD_ADDR rem_bda, BT_HDR *p_buf, UINT16 flags);

/*******************************************************************************
**
**  Function        L2CA_UcdSetIdleTimeout
**
**  Description     Set UCD Idle timeout.
**
**  Parameters:     BD Addr
**                  Timeout in second
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
extern BOOLEAN L2CA_UcdSetIdleTimeout ( BD_ADDR rem_bda, UINT16 timeout );

/*******************************************************************************
**
** Function         L2CA_UCDSetTxPriority
**
** Description      Sets the transmission priority for a connectionless channel.
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
extern BOOLEAN L2CA_UCDSetTxPriority ( BD_ADDR rem_bda, tL2CAP_CHNL_PRIORITY priority );


/*******************************************************************************
**
**                      Fixed Channel callback prototypes
**
*******************************************************************************/

/* Fixed channel connected and disconnected. Parameters are
**      channel
**      BD Address of remote
**      TRUE if channel is connected, FALSE if disconnected
**      Reason for connection failure
**      transport : physical transport, BR/EDR or LE
*/
typedef void (tL2CA_FIXED_CHNL_CB) (UINT16, BD_ADDR, BOOLEAN, UINT16, tBT_TRANSPORT);

/* Signalling data received. Parameters are
**      channel
**      BD Address of remote
**      Pointer to buffer with data
*/
typedef void (tL2CA_FIXED_DATA_CB) (UINT16, BD_ADDR, BT_HDR *);

/* Congestion status callback protype. This callback is optional. If
** an application tries to send data when the transmit queue is full,
** the data will anyways be dropped. The parameter is:
**      remote BD_ADDR
**      TRUE if congested, FALSE if uncongested
*/
typedef void (tL2CA_FIXED_CONGESTION_STATUS_CB) (BD_ADDR, BOOLEAN);

/* Fixed channel registration info (the callback addresses and channel config)
*/
typedef struct {
    tL2CA_FIXED_CHNL_CB    *pL2CA_FixedConn_Cb;
    tL2CA_FIXED_DATA_CB    *pL2CA_FixedData_Cb;
    tL2CA_FIXED_CONGESTION_STATUS_CB *pL2CA_FixedCong_Cb;
    tL2CAP_FCR_OPTS         fixed_chnl_opts;

    UINT16                  default_idle_tout;
    tL2CA_TX_COMPLETE_CB    *pL2CA_FixedTxComplete_Cb; /* fixed channel tx complete callback */
} tL2CAP_FIXED_CHNL_REG;


#if (L2CAP_NUM_FIXED_CHNLS > 0)
/*******************************************************************************
**
**  Function        L2CA_RegisterFixedChannel
**
**  Description     Register a fixed channel.
**
**  Parameters:     Fixed Channel #
**                  Channel Callbacks and config
**
**  Return value:   TRUE if registered OK
**
*******************************************************************************/
extern BOOLEAN  L2CA_RegisterFixedChannel (UINT16 fixed_cid, tL2CAP_FIXED_CHNL_REG *p_freg);

/*******************************************************************************
**
**  Function        L2CA_ConnectFixedChnl
**
**  Description     Connect an fixed signalling channel to a remote device.
**
**  Parameters:     Fixed CID
**                  BD Address of remote
**
**  Return value:   TRUE if connection started
**
*******************************************************************************/
extern BOOLEAN L2CA_ConnectFixedChnl (UINT16 fixed_cid, BD_ADDR bd_addr);

/*******************************************************************************
**
**  Function        L2CA_SendFixedChnlData
**
**  Description     Write data on a fixed signalling channel.
**
**  Parameters:     Fixed CID
**                  BD Address of remote
**                  Pointer to buffer of type BT_HDR
**
** Return value     L2CAP_DW_SUCCESS, if data accepted
**                  L2CAP_DW_FAILED,  if error
**
*******************************************************************************/
extern UINT16 L2CA_SendFixedChnlData (UINT16 fixed_cid, BD_ADDR rem_bda, BT_HDR *p_buf);

/*******************************************************************************
**
**  Function        L2CA_RemoveFixedChnl
**
**  Description     Remove a fixed channel to a remote device.
**
**  Parameters:     Fixed CID
**                  BD Address of remote
**                  Idle timeout to use (or 0xFFFF if don't care)
**
**  Return value:   TRUE if channel removed
**
*******************************************************************************/
extern BOOLEAN L2CA_RemoveFixedChnl (UINT16 fixed_cid, BD_ADDR rem_bda);

/*******************************************************************************
**
** Function         L2CA_SetFixedChannelTout
**
** Description      Higher layers call this function to set the idle timeout for
**                  a fixed channel. The "idle timeout" is the amount of time that
**                  a connection can remain up with no L2CAP channels on it.
**                  A timeout of zero means that the connection will be torn
**                  down immediately when the last channel is removed.
**                  A timeout of 0xFFFF means no timeout. Values are in seconds.
**                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY,
**                  then the idle timeouts for all active l2cap links will be
**                  changed.
**
** Returns          TRUE if command succeeded, FALSE if failed
**
*******************************************************************************/
extern BOOLEAN L2CA_SetFixedChannelTout (BD_ADDR rem_bda, UINT16 fixed_cid, UINT16 idle_tout);

#endif /* (L2CAP_NUM_FIXED_CHNLS > 0) */

#if (CLASSIC_BT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function     L2CA_GetCurrentConfig
**
** Description  This function returns configurations of L2CAP channel
**              pp_our_cfg : pointer of our saved configuration options
**              p_our_cfg_bits : valid config in bitmap
**              pp_peer_cfg: pointer of peer's saved configuration options
**              p_peer_cfg_bits : valid config in bitmap
**
** Returns      TRUE if successful
**
*******************************************************************************/
extern BOOLEAN L2CA_GetCurrentConfig (UINT16 lcid,
                                      tL2CAP_CFG_INFO **pp_our_cfg,  tL2CAP_CH_CFG_BITS *p_our_cfg_bits,
                                      tL2CAP_CFG_INFO **pp_peer_cfg, tL2CAP_CH_CFG_BITS *p_peer_cfg_bits);
#endif  ///CLASSIC_BT_INCLUDED == TRUE


#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
**
**  Function        L2CA_CancelBleConnectReq
**
**  Description     Cancel a pending connection attempt to a BLE device.
**
**  Parameters:     BD Address of remote
**
**  Return value:   TRUE if connection was cancelled
**
*******************************************************************************/
extern BOOLEAN L2CA_CancelBleConnectReq (BD_ADDR rem_bda);

/*******************************************************************************
**
**  Function        L2CA_UpdateBleConnParams
**
**  Description     Update BLE connection parameters.
**
**  Parameters:     BD Address of remote
**
**  Return value:   TRUE if update started
**
*******************************************************************************/
extern BOOLEAN L2CA_UpdateBleConnParams (BD_ADDR rem_bdRa, UINT16 min_int,
        UINT16 max_int, UINT16 latency, UINT16 timeout);

/*******************************************************************************
**
**  Function        L2CA_EnableUpdateBleConnParams
**
**  Description     Update BLE connection parameters.
**
**  Parameters:     BD Address of remote
**                  enable flag
**
**  Return value:   TRUE if update started
**
*******************************************************************************/
extern BOOLEAN L2CA_EnableUpdateBleConnParams (BD_ADDR rem_bda, BOOLEAN enable);

/*******************************************************************************
**
** Function         L2CA_GetBleConnRole
**
** Description      This function returns the connection role.
**
** Returns          link role.
**
*******************************************************************************/
extern UINT8 L2CA_GetBleConnRole (BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         L2CA_GetDisconnectReason
**
** Description      This function returns the disconnect reason code.
**
**  Parameters:     BD Address of remote
**                         Physical transport for the L2CAP connection (BR/EDR or LE)
**
** Returns          disconnect reason
**
*******************************************************************************/
extern UINT16 L2CA_GetDisconnectReason (BD_ADDR remote_bda, tBT_TRANSPORT transport);

#endif /* (BLE_INCLUDED == TRUE) */

#ifdef __cplusplus
}
#endif

#endif  /* L2C_API_H */
