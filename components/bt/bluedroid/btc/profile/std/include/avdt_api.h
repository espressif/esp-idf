/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
 *  This interface file contains the interface to the Audio Video
 *  Distribution Transport Protocol (AVDTP).
 *
 ******************************************************************************/
#ifndef AVDT_API_H
#define AVDT_API_H

#include "bt_types.h"
#include "bt_target.h"

/*****************************************************************************
**  Constants
*****************************************************************************/
#ifndef AVDT_VERSION
#define AVDT_VERSION                0x0102
#endif
#define AVDT_VERSION_SYNC           0x0103

/* API function return value result codes. */
#define AVDT_SUCCESS                0       /* Function successful */
#define AVDT_BAD_PARAMS             1       /* Invalid parameters */
#define AVDT_NO_RESOURCES           2       /* Not enough resources */
#define AVDT_BAD_HANDLE             3       /* Bad handle */
#define AVDT_BUSY                   4       /* A procedure is already in progress */
#define AVDT_WRITE_FAIL             5       /* Write failed */

/* The index to access the codec type in codec_info[]. */
#define AVDT_CODEC_TYPE_INDEX       2

/* The size in bytes of a Adaptation Layer header. */
#define AVDT_AL_HDR_SIZE         3

/* The size in bytes of a media packet header. */
#define AVDT_MEDIA_HDR_SIZE         12

/* AVDTP 7.5.3 Adaptation Layer Fragmentation
 * original length of the un-fragmented transport packet should be specified by
 * two bytes length field of Adaptation Layer Header  */
#define AVDT_MAX_MEDIA_SIZE         (0xFFFF - AVDT_MEDIA_HDR_SIZE)

/* The handle is used when reporting MULTI_AV specific events */
#define AVDT_MULTI_AV_HANDLE        0xFF

/* The number of bytes needed by the protocol stack for the protocol headers
** of a media packet.  This is the size of the media packet header, the
** L2CAP packet header and HCI header.
*/
#define AVDT_MEDIA_OFFSET           23

/* The marker bit is used by the application to mark significant events such
** as frame boundaries in the data stream.  This constant is used to check or
** set the marker bit in the m_pt parameter of an AVDT_WriteReq()
** or AVDT_DATA_IND_EVT.
*/
#define AVDT_MARKER_SET             0x80

/* SEP Type.  This indicates the stream endpoint type. */
#define AVDT_TSEP_SRC               0       /* Source SEP */
#define AVDT_TSEP_SNK               1       /* Sink SEP */

/* initiator/acceptor role for adaption */
#define AVDT_INT                0       /* initiator */
#define AVDT_ACP                1       /* acceptor */

/* Media Type.  This indicates the media type of the stream endpoint. */
#define AVDT_MEDIA_AUDIO            0       /* Audio SEP */
#define AVDT_MEDIA_VIDEO            1       /* Video SEP */
#define AVDT_MEDIA_MULTI            2       /* Multimedia SEP */

/* for reporting packets */
#define AVDT_RTCP_PT_SR         200     /* the packet type - SR (Sender Report) */
#define AVDT_RTCP_PT_RR         201     /* the packet type - RR (Receiver Report) */
#define AVDT_RTCP_PT_SDES       202     /* the packet type - SDES (Source Description) */
typedef UINT8 AVDT_REPORT_TYPE;

#define AVDT_RTCP_SDES_CNAME    1       /* SDES item CNAME */
#ifndef AVDT_MAX_CNAME_SIZE
#define AVDT_MAX_CNAME_SIZE     28
#endif

/* Protocol service capabilities.  This indicates the protocol service
** capabilities of a stream endpoint.  This value is a mask.
** Multiple values can be combined with a bitwise OR.
*/
#define AVDT_PSC_TRANS              (1<<1)  /* Media transport */
#define AVDT_PSC_REPORT             (1<<2)  /* Reporting */
#define AVDT_PSC_RECOV              (1<<3)  /* Recovery */
#define AVDT_PSC_HDRCMP             (1<<5)  /* Header compression */
#define AVDT_PSC_MUX                (1<<6)  /* Multiplexing */
#define AVDT_PSC_DELAY_RPT          (1<<8)  /* Delay Report */

/* Recovery type.  This indicates the recovery type. */
#define AVDT_RECOV_RFC2733          1       /* RFC2733 recovery */

/* Header compression capabilities.  This indicates the header compression
** capabilities.  This value is a mask.  Multiple values can be combined
** with a bitwise OR.
*/
#define AVDT_HDRCMP_MEDIA           (1<<5)  /* Available for media packets */
#define AVDT_HDRCMP_RECOV           (1<<6)  /* Available for recovery packets */
#define AVDT_HDRCMP_BACKCH          (1<<7)  /* Back channel supported */

/* Multiplexing capabilities mask. */
#define AVDT_MUX_FRAG               (1<<7)  /* Allow Adaptation Layer Fragmentation */

/* Application service category. This indicates the application
** service category.
*/
#define AVDT_ASC_PROTECT            4       /* Content protection */
#define AVDT_ASC_CODEC              7       /* Codec */

/* Error codes.  The following are error codes defined in the AVDTP and GAVDP
** specifications.  These error codes communicate protocol errors between
** AVDTP and the application.  More detailed descriptions of the error codes
** and their appropriate use can be found in the AVDTP and GAVDP specifications.
** These error codes are unrelated to the result values returned by the
** AVDTP API functions.
*/
#define AVDT_ERR_HEADER             0x01    /* Bad packet header format */
#define AVDT_ERR_LENGTH             0x11    /* Bad packet length */
#define AVDT_ERR_SEID               0x12    /* Invalid SEID */
#define AVDT_ERR_IN_USE             0x13    /* The SEP is in use */
#define AVDT_ERR_NOT_IN_USE         0x14    /* The SEP is not in use */
#define AVDT_ERR_CATEGORY           0x17    /* Bad service category */
#define AVDT_ERR_PAYLOAD            0x18    /* Bad payload format */
#define AVDT_ERR_NSC                0x19    /* Requested command not supported */
#define AVDT_ERR_INVALID_CAP        0x1A    /* Reconfigure attempted invalid capabilities */
#define AVDT_ERR_RECOV_TYPE         0x22    /* Requested recovery type not defined */
#define AVDT_ERR_MEDIA_TRANS        0x23    /* Media transport capability not correct */
#define AVDT_ERR_RECOV_FMT          0x25    /* Recovery service capability not correct */
#define AVDT_ERR_ROHC_FMT           0x26    /* Header compression service capability not correct */
#define AVDT_ERR_CP_FMT             0x27    /* Content protection service capability not correct */
#define AVDT_ERR_MUX_FMT            0x28    /* Multiplexing service capability not correct */
#define AVDT_ERR_UNSUP_CFG          0x29    /* Configuration not supported */
#define AVDT_ERR_BAD_STATE          0x31    /* Message cannot be processed in this state */
#define AVDT_ERR_REPORT_FMT         0x65    /* Report service capability not correct */
#define AVDT_ERR_SERVICE            0x80    /* Invalid service category */
#define AVDT_ERR_RESOURCE           0x81    /* Insufficient resources */
#define AVDT_ERR_INVALID_MCT        0xC1    /* Invalid Media Codec Type */
#define AVDT_ERR_UNSUP_MCT          0xC2    /* Unsupported Media Codec Type */
#define AVDT_ERR_INVALID_LEVEL      0xC3    /* Invalid Level */
#define AVDT_ERR_UNSUP_LEVEL        0xC4    /* Unsupported Level */
#define AVDT_ERR_INVALID_CP         0xE0    /* Invalid Content Protection Type */
#define AVDT_ERR_INVALID_FORMAT     0xE1    /* Invalid Content Protection format */

/* Additional error codes.  This indicates error codes used by AVDTP
** in addition to the ones defined in the specifications.
*/
#define AVDT_ERR_CONNECT            0x07    /* Connection failed. */
#define AVDT_ERR_TIMEOUT            0x08    /* Response timeout. */

/* Control callback events. */
#define AVDT_DISCOVER_CFM_EVT       0       /* Discover confirm */
#define AVDT_GETCAP_CFM_EVT         1       /* Get capabilities confirm */
#define AVDT_OPEN_CFM_EVT           2       /* Open confirm */
#define AVDT_OPEN_IND_EVT           3       /* Open indication */
#define AVDT_CONFIG_IND_EVT         4       /* Configuration indication */
#define AVDT_START_CFM_EVT          5       /* Start confirm */
#define AVDT_START_IND_EVT          6       /* Start indication */
#define AVDT_SUSPEND_CFM_EVT        7       /* Suspend confirm */
#define AVDT_SUSPEND_IND_EVT        8       /* Suspend indication */
#define AVDT_CLOSE_CFM_EVT          9       /* Close confirm */
#define AVDT_CLOSE_IND_EVT          10      /* Close indication */
#define AVDT_RECONFIG_CFM_EVT       11      /* Reconfiguration confirm */
#define AVDT_RECONFIG_IND_EVT       12      /* Reconfiguration indication */
#define AVDT_SECURITY_CFM_EVT       13      /* Security confirm */
#define AVDT_SECURITY_IND_EVT       14      /* Security indication */
#define AVDT_WRITE_CFM_EVT          15      /* Write confirm */
#define AVDT_CONNECT_IND_EVT        16      /* Signaling channel connected */
#define AVDT_DISCONNECT_IND_EVT     17      /* Signaling channel disconnected */
#define AVDT_REPORT_CONN_EVT        18      /* Reporting channel connected */
#define AVDT_REPORT_DISCONN_EVT     19      /* Reporting channel disconnected */
#define AVDT_DELAY_REPORT_EVT       20      /* Delay report received */
#define AVDT_DELAY_REPORT_CFM_EVT   21      /* Delay report response received */

#define AVDT_MAX_EVT                (AVDT_DELAY_REPORT_CFM_EVT)

/* PSM for AVDT */
#define AVDT_PSM                    0x0019

/* Nonsupported protocol command messages.  This value is used in tAVDT_CS */
#define AVDT_NSC_SUSPEND            0x01    /* Suspend command not supported */
#define AVDT_NSC_RECONFIG           0x02    /* Reconfigure command not supported */
#define AVDT_NSC_SECURITY           0x04    /* Security command not supported */

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

typedef struct {
    UINT32  ntp_sec;        /* NTP time: seconds relative to 0h UTC on 1 January 1900 */
    UINT32  ntp_frac;       /* NTP time: the fractional part */
    UINT32  rtp_time;       /* timestamp in RTP header */
    UINT32  pkt_count;      /* sender's packet count: since starting transmission
                             * up until the time this SR packet was generated. */
    UINT32  octet_count;    /* sender's octet count: same comment */
} tAVDT_SENDER_INFO;

typedef struct {
    UINT8   frag_lost;      /* fraction lost since last RR */
    UINT32  packet_lost;    /* cumulative number of packets lost since the beginning */
    UINT32  seq_num_rcvd;   /* extended highest sequence number received */
    UINT32  jitter;         /* interarrival jitter */
    UINT32  lsr;            /* last SR timestamp */
    UINT32  dlsr;           /* delay since last SR */
} tAVDT_REPORT_BLK;

typedef union {
    tAVDT_SENDER_INFO   sr;
    tAVDT_REPORT_BLK    rr;
    UINT8               cname[AVDT_MAX_CNAME_SIZE + 1];
} tAVDT_REPORT_DATA;

/* This structure contains parameters which are set at registration. */
typedef struct {
    UINT16      ctrl_mtu;   /* L2CAP MTU of the AVDTP signaling channel */
    UINT8       ret_tout;   /* AVDTP signaling retransmission timeout */
    UINT8       sig_tout;   /* AVDTP signaling message timeout */
    UINT8       idle_tout;  /* AVDTP idle signaling channel timeout */
    UINT8       sec_mask;   /* Security mask for BTM_SetSecurityLevel() */
} tAVDT_REG;

/* This structure contains the SEP information.  This information is
** transferred during the discovery procedure.
*/
typedef struct {
    BOOLEAN     in_use;         /* TRUE if stream is currently in use */
    UINT8       seid;           /* Stream endpoint identifier */
    UINT8       media_type;     /* Media type */
    UINT8       tsep;           /* SEP type */
} tAVDT_SEP_INFO;

/* This structure contains the SEP configuration. */
typedef struct {
    UINT8   codec_info[AVDT_CODEC_SIZE];        /* Codec capabilities array */
    UINT8   protect_info[AVDT_PROTECT_SIZE];    /* Content protection capabilities */
    UINT8   num_codec;                          /* Number of media codec information elements */
    UINT8   num_protect;                        /* Number of content protection information elements */
    UINT16  psc_mask;                           /* Protocol service capabilities mask */
    UINT8   recov_type;                         /* Recovery type */
    UINT8   recov_mrws;                         /* Maximum recovery window size */
    UINT8   recov_mnmp;                         /* Recovery maximum number of media packets */
    UINT8   hdrcmp_mask;                        /* Header compression capabilities */
#if AVDT_MULTIPLEXING == TRUE
    UINT8   mux_mask;                           /* Multiplexing capabilities. AVDT_MUX_XXX bits can be combined with a bitwise OR */
    UINT8   mux_tsid_media;                     /* TSID for media transport session */
    UINT8   mux_tcid_media;                     /* TCID for media transport session */
    UINT8   mux_tsid_report;                    /* TSID for reporting transport session */
    UINT8   mux_tcid_report;                    /* TCID for reporting transport session */
    UINT8   mux_tsid_recov;                     /* TSID for recovery transport session */
    UINT8   mux_tcid_recov;                     /* TCID for recovery transport session */
#endif
} tAVDT_CFG;

/* Header structure for callback event parameters. */
typedef struct {
    UINT8           err_code;           /* Zero if operation succeeded; nonzero if operation failed */
    UINT8           err_param;          /* Error parameter included for some events */
    UINT8           label;              /* Transaction label */
    UINT8           seid;               /* For internal use only */
    UINT8           sig_id;             /* For internal use only */
    UINT8           ccb_idx;            /* For internal use only */
} tAVDT_EVT_HDR;

/* This data structure is associated with the AVDT_GETCAP_CFM_EVT,
** AVDT_RECONFIG_IND_EVT, and AVDT_RECONFIG_CFM_EVT.
*/
typedef struct {
    tAVDT_EVT_HDR   hdr;                /* Event header */
    tAVDT_CFG       *p_cfg;             /* Pointer to configuration for this SEP */
} tAVDT_CONFIG;

/* This data structure is associated with the AVDT_CONFIG_IND_EVT. */
typedef struct {
    tAVDT_EVT_HDR   hdr;                /* Event header */
    tAVDT_CFG       *p_cfg;             /* Pointer to configuration for this SEP */
    UINT8           int_seid;           /* Stream endpoint ID of stream initiating the operation */
} tAVDT_SETCONFIG;

/* This data structure is associated with the AVDT_OPEN_IND_EVT and AVDT_OPEN_CFM_EVT. */
typedef struct {
    tAVDT_EVT_HDR   hdr;                /* Event header */
    UINT16          peer_mtu;           /* Transport channel L2CAP MTU of the peer */
    UINT16          lcid;               /* L2CAP LCID for media channel */
} tAVDT_OPEN;

/* This data structure is associated with the AVDT_SECURITY_IND_EVT
** and AVDT_SECURITY_CFM_EVT.
*/
typedef struct {
    tAVDT_EVT_HDR   hdr;                /* Event header */
    UINT8           *p_data;            /* Pointer to security data */
    UINT16          len;                /* Length in bytes of the security data */
} tAVDT_SECURITY;

/* This data structure is associated with the AVDT_DISCOVER_CFM_EVT. */
typedef struct {
    tAVDT_EVT_HDR   hdr;                /* Event header */
    tAVDT_SEP_INFO  *p_sep_info;        /* Pointer to SEP information */
    UINT8           num_seps;           /* Number of stream endpoints */
} tAVDT_DISCOVER;

/* This data structure is associated with the AVDT_DELAY_REPORT_EVT. */
typedef struct {
    tAVDT_EVT_HDR   hdr;                /* Event header */
    UINT16          delay;              /* Delay value */
} tAVDT_DELAY_RPT;

/* Union of all control callback event data structures */
typedef union {
    tAVDT_EVT_HDR       hdr;
    tAVDT_DISCOVER      discover_cfm;
    tAVDT_CONFIG        getcap_cfm;
    tAVDT_OPEN          open_cfm;
    tAVDT_OPEN          open_ind;
    tAVDT_SETCONFIG     config_ind;
    tAVDT_EVT_HDR       start_cfm;
    tAVDT_EVT_HDR       suspend_cfm;
    tAVDT_EVT_HDR       close_cfm;
    tAVDT_CONFIG        reconfig_cfm;
    tAVDT_CONFIG        reconfig_ind;
    tAVDT_SECURITY      security_cfm;
    tAVDT_SECURITY      security_ind;
    tAVDT_EVT_HDR       connect_ind;
    tAVDT_EVT_HDR       disconnect_ind;
    tAVDT_EVT_HDR       report_conn;
    tAVDT_DELAY_RPT     delay_rpt_cmd;
} tAVDT_CTRL;

/* This is the control callback function.  This function passes control events
** to the application.  This function is required for all registered stream
** endpoints and for the AVDT_DiscoverReq() and AVDT_GetCapReq() functions.
**
*/
typedef void (tAVDT_CTRL_CBACK)(UINT8 handle, BD_ADDR bd_addr, UINT8 event,
                                tAVDT_CTRL *p_data);

/* This is the data callback function.  It is executed when AVDTP has a media
** packet ready for the application.  This function is required for SNK
** endpoints and not applicable for SRC endpoints.
*/
typedef void (tAVDT_DATA_CBACK)(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp,
                                UINT8 m_pt);

#if AVDT_MULTIPLEXING == TRUE
/* This is the second version of the data callback function. This version uses
** application buffer assigned by AVDT_SetMediaBuf. Caller can assign different
** buffer during callback or can leave the current buffer for further using.
** This callback is called when AVDTP has a media packet ready for the application.
** This function is required for SNK endpoints and not applicable for SRC endpoints.
*/
typedef void (tAVDT_MEDIA_CBACK)(UINT8 handle, UINT8 *p_payload, UINT32 payload_len,
                                 UINT32 time_stamp, UINT16 seq_num, UINT8 m_pt, UINT8 marker);
#endif

#if AVDT_REPORTING == TRUE
/* This is the report callback function.  It is executed when AVDTP has a reporting
** packet ready for the application.  This function is required for streams
** created with AVDT_PSC_REPORT.
*/
typedef void (tAVDT_REPORT_CBACK)(UINT8 handle, AVDT_REPORT_TYPE type,
                                  tAVDT_REPORT_DATA *p_data);
#endif

typedef UINT16 (tAVDT_GETCAP_REQ) (BD_ADDR bd_addr, UINT8 seid, tAVDT_CFG *p_cfg, tAVDT_CTRL_CBACK *p_cback);

/* This structure contains information required when a stream is created.
** It is passed to the AVDT_CreateStream() function.
*/
typedef struct {
    tAVDT_CFG           cfg;            /* SEP configuration */
    tAVDT_CTRL_CBACK    *p_ctrl_cback;  /* Control callback function */
    tAVDT_DATA_CBACK    *p_data_cback;  /* Data callback function */
#if AVDT_MULTIPLEXING == TRUE
    tAVDT_MEDIA_CBACK   *p_media_cback; /* Media callback function. It will be called only if p_data_cback is NULL */
#endif
#if AVDT_REPORTING == TRUE
    tAVDT_REPORT_CBACK  *p_report_cback;/* Report callback function. */
#endif
    UINT16              mtu;            /* The L2CAP MTU of the transport channel */
    UINT16              flush_to;       /* The L2CAP flush timeout of the transport channel */
    UINT8               tsep;           /* SEP type */
    UINT8               media_type;     /* Media type */
    UINT16              nsc_mask;       /* Nonsupported protocol command messages */
} tAVDT_CS;

/* AVDT data option mask is used in the write request */
#define AVDT_DATA_OPT_NONE      0x00         /* No option still add RTP header */
#define AVDT_DATA_OPT_NO_RTP   (0x01 << 0)   /* Skip adding RTP header */

typedef UINT8 tAVDT_DATA_OPT_MASK;



/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         AVDT_Register
**
** Description      This is the system level registration function for the
**                  AVDTP protocol.  This function initializes AVDTP and
**                  prepares the protocol stack for its use.  This function
**                  must be called once by the system or platform using AVDTP
**                  before the other functions of the API an be used.
**
**
** Returns          void
**
*******************************************************************************/
extern void AVDT_Register(tAVDT_REG *p_reg, tAVDT_CTRL_CBACK *p_cback);

/*******************************************************************************
**
** Function         AVDT_Deregister
**
** Description      This function is called to deregister use AVDTP protocol.
**                  It is called when AVDTP is no longer being used by any
**                  application in the system.  Before this function can be
**                  called, all streams must be removed with AVDT_RemoveStream().
**
**
** Returns          void
**
*******************************************************************************/
extern void AVDT_Deregister(void);


/*******************************************************************************
**
** Function         AVDT_SINK_Activate
**
** Description      Activate SEP of A2DP Sink. In Use parameter is adjusted.
**                  In Use will be made false in case of activation. A2DP SRC
**                  will receive in_use as false and can open A2DP Sink
**                  connection
**
** Returns          void
**
*******************************************************************************/
extern void AVDT_SINK_Activate(void);

/*******************************************************************************
**
** Function         AVDT_SINK_Deactivate
**
** Description      Deactivate SEP of A2DP Sink. In Use parameter is adjusted.
**                  In Use will be made TRUE in case of activation. A2DP SRC
**                  will receive in_use as true and will not open A2DP Sink
**                  connection
**
** Returns          void.
**
*******************************************************************************/
extern void AVDT_SINK_Deactivate(void);

/*******************************************************************************
**
** Function         AVDT_AbortReq
**
** Description      Trigger Abort request to pass AVDTP Abort related mandatory
**                  PTS Test case.
**
** Returns          void.
**
*******************************************************************************/
extern void AVDT_AbortReq(UINT8 handle);

/*******************************************************************************
**
** Function         AVDT_CreateStream
**
** Description      Create a stream endpoint.  After a stream endpoint is
**                  created an application can initiate a connection between
**                  this endpoint and an endpoint on a peer device.  In
**                  addition, a peer device can discover, get the capabilities,
**                  and connect to this endpoint.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_CreateStream(UINT8 *p_handle, tAVDT_CS *p_cs);

/*******************************************************************************
**
** Function         AVDT_RemoveStream
**
** Description      Remove a stream endpoint.  This function is called when
**                  the application is no longer using a stream endpoint.
**                  If this function is called when the endpoint is connected
**                  the connection is closed and then the stream endpoint
**                  is removed.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_RemoveStream(UINT8 handle);

/*******************************************************************************
**
** Function         AVDT_DiscoverReq
**
** Description      This function initiates a connection to the AVDTP service
**                  on the peer device, if not already present, and discovers
**                  the stream endpoints on the peer device.  (Please note
**                  that AVDTP discovery is unrelated to SDP discovery).
**                  This function can be called at any time regardless of whether
**                  there is an AVDTP connection to the peer device.
**
**                  When discovery is complete, an AVDT_DISCOVER_CFM_EVT
**                  is sent to the application via its callback function.
**                  The application must not call AVDT_GetCapReq() or
**                  AVDT_DiscoverReq() again to the same device until
**                  discovery is complete.
**
**                  The memory addressed by sep_info is allocated by the
**                  application.  This memory is written to by AVDTP as part
**                  of the discovery procedure.  This memory must remain
**                  accessible until the application receives the
**                  AVDT_DISCOVER_CFM_EVT.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_DiscoverReq(BD_ADDR bd_addr, tAVDT_SEP_INFO *p_sep_info,
                               UINT8 max_seps, tAVDT_CTRL_CBACK *p_cback);


/*******************************************************************************
**
** Function         AVDT_GetCapReq
**
** Description      This function initiates a connection to the AVDTP service
**                  on the peer device, if not already present, and gets the
**                  capabilities of a stream endpoint on the peer device.
**                  This function can be called at any time regardless of
**                  whether there is an AVDTP connection to the peer device.
**
**                  When the procedure is complete, an AVDT_GETCAP_CFM_EVT is
**                  sent to the application via its callback function.  The
**                  application must not call AVDT_GetCapReq() or
**                  AVDT_DiscoverReq() again until the procedure is complete.
**
**                  The memory pointed to by p_cfg is allocated by the
**                  application.  This memory is written to by AVDTP as part
**                  of the get capabilities procedure.  This memory must
**                  remain accessible until the application receives
**                  the AVDT_GETCAP_CFM_EVT.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_GetCapReq(BD_ADDR bd_addr, UINT8 seid, tAVDT_CFG *p_cfg,
                             tAVDT_CTRL_CBACK *p_cback);

/*******************************************************************************
**
** Function         AVDT_GetAllCapReq
**
** Description      This function initiates a connection to the AVDTP service
**                  on the peer device, if not already present, and gets the
**                  capabilities of a stream endpoint on the peer device.
**                  This function can be called at any time regardless of
**                  whether there is an AVDTP connection to the peer device.
**
**                  When the procedure is complete, an AVDT_GETCAP_CFM_EVT is
**                  sent to the application via its callback function.  The
**                  application must not call AVDT_GetCapReq() or
**                  AVDT_DiscoverReq() again until the procedure is complete.
**
**                  The memory pointed to by p_cfg is allocated by the
**                  application.  This memory is written to by AVDTP as part
**                  of the get capabilities procedure.  This memory must
**                  remain accessible until the application receives
**                  the AVDT_GETCAP_CFM_EVT.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_GetAllCapReq(BD_ADDR bd_addr, UINT8 seid, tAVDT_CFG *p_cfg,
                                tAVDT_CTRL_CBACK *p_cback);

/*******************************************************************************
**
** Function         AVDT_DelayReport
**
** Description      This functions sends a Delay Report to the peer device
**                  that is associated with a particular SEID.
**                  This function is called by SNK device.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_DelayReport(UINT8 handle, UINT8 seid, UINT16 delay);

/*******************************************************************************
**
** Function         AVDT_OpenReq
**
** Description      This function initiates a connection to the AVDTP service
**                  on the peer device, if not already present, and connects
**                  to a stream endpoint on a peer device.  When the connection
**                  is completed, an AVDT_OPEN_CFM_EVT is sent to the
**                  application via the control callback function for this handle.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_OpenReq(UINT8 handle, BD_ADDR bd_addr, UINT8 seid,
                           tAVDT_CFG *p_cfg);


/*******************************************************************************
**
** Function         AVDT_ConfigRsp
**
** Description      Respond to a configure request from the peer device.  This
**                  function must be called if the application receives an
**                  AVDT_CONFIG_IND_EVT through its control callback.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_ConfigRsp(UINT8 handle, UINT8 label, UINT8 error_code,
                             UINT8 category);

/*******************************************************************************
**
** Function         AVDT_StartReq
**
** Description      Start one or more stream endpoints.  This initiates the
**                  transfer of media packets for the streams.  All stream
**                  endpoints must previously be opened.  When the streams
**                  are started, an AVDT_START_CFM_EVT is sent to the
**                  application via the control callback function for each stream.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_StartReq(UINT8 *p_handles, UINT8 num_handles);

/*******************************************************************************
**
** Function         AVDT_SuspendReq
**
** Description      Suspend one or more stream endpoints. This suspends the
**                  transfer of media packets for the streams.  All stream
**                  endpoints must previously be open and started.  When the
**                  streams are suspended, an AVDT_SUSPEND_CFM_EVT is sent to
**                  the application via the control callback function for
**                  each stream.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_SuspendReq(UINT8 *p_handles, UINT8 num_handles);

/*******************************************************************************
**
** Function         AVDT_CloseReq
**
** Description      Close a stream endpoint.  This stops the transfer of media
**                  packets and closes the transport channel associated with
**                  this stream endpoint.  When the stream is closed, an
**                  AVDT_CLOSE_CFM_EVT is sent to the application via the
**                  control callback function for this handle.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_CloseReq(UINT8 handle);

/*******************************************************************************
**
** Function         AVDT_ReconfigReq
**
** Description      Reconfigure a stream endpoint.  This allows the application
**                  to change the codec or content protection capabilities of
**                  a stream endpoint after it has been opened.  This function
**                  can only be called if the stream is opened but not started
**                  or if the stream has been suspended.  When the procedure
**                  is completed, an AVDT_RECONFIG_CFM_EVT is sent to the
**                  application via the control callback function for this handle.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_ReconfigReq(UINT8 handle, tAVDT_CFG *p_cfg);

/*******************************************************************************
**
** Function         AVDT_ReconfigRsp
**
** Description      Respond to a reconfigure request from the peer device.
**                  This function must be called if the application receives
**                  an AVDT_RECONFIG_IND_EVT through its control callback.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_ReconfigRsp(UINT8 handle, UINT8 label, UINT8 error_code,
                               UINT8 category);

/*******************************************************************************
**
** Function         AVDT_SecurityReq
**
** Description      Send a security request to the peer device.  When the
**                  security procedure is completed, an AVDT_SECURITY_CFM_EVT
**                  is sent to the application via the control callback function
**                  for this handle.  (Please note that AVDTP security procedures
**                  are unrelated to Bluetooth link level security.)
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_SecurityReq(UINT8 handle, UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         AVDT_SecurityRsp
**
** Description      Respond to a security request from the peer device.
**                  This function must be called if the application receives
**                  an AVDT_SECURITY_IND_EVT through its control callback.
**                  (Please note that AVDTP security procedures are unrelated
**                  to Bluetooth link level security.)
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_SecurityRsp(UINT8 handle, UINT8 label, UINT8 error_code,
                               UINT8 *p_data, UINT16 len);

/*******************************************************************************
**
** Function         AVDT_WriteReq
**
** Description      Send a media packet to the peer device.  The stream must
**                  be started before this function is called.  Also, this
**                  function can only be called if the stream is a SRC.
**
**                  When AVDTP has sent the media packet and is ready for the
**                  next packet, an AVDT_WRITE_CFM_EVT is sent to the
**                  application via the control callback.  The application must
**                  wait for the AVDT_WRITE_CFM_EVT before it makes the next
**                  call to AVDT_WriteReq().  If the applications calls
**                  AVDT_WriteReq() before it receives the event the packet
**                  will not be sent.  The application may make its first call
**                  to AVDT_WriteReq() after it receives an AVDT_START_CFM_EVT
**                  or AVDT_START_IND_EVT.
**
**                  The application passes the packet using the BT_HDR structure.
**                  This structure is described in section 2.1.  The offset
**                  field must be equal to or greater than AVDT_MEDIA_OFFSET.
**                  This allows enough space in the buffer for the L2CAP and
**                  AVDTP headers.
**
**                  The memory pointed to by p_pkt must be a GKI buffer
**                  allocated by the application.  This buffer will be freed
**                  by the protocol stack; the application must not free
**                  this buffer.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_WriteReq(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp,
                            UINT8 m_pt);
/*******************************************************************************
**
** Function         AVDT_WriteReqOpt
**
** Description      Send a media packet to the peer device.  The stream must
**                  be started before this function is called.  Also, this
**                  function can only be called if the stream is a SRC
**
**                  When AVDTP has sent the media packet and is ready for the
**                  next packet, an AVDT_WRITE_CFM_EVT is sent to the
**                  application via the control callback.  The application must
**                  wait for the AVDT_WRITE_CFM_EVT before it makes the next
**                  call to AVDT_WriteReq().  If the applications calls
**                  AVDT_WriteReq() before it receives the event the packet
**                  will not be sent.  The application may make its first call
**                  to AVDT_WriteReq() after it receives an AVDT_START_CFM_EVT
**                  or AVDT_START_IND_EVT.
**
**                  The application passes the packet using the BT_HDR structure
**                  This structure is described in section 2.1.  The offset
**                  field must be equal to or greater than AVDT_MEDIA_OFFSET
**                  (if NO_RTP is specified, L2CAP_MIN_OFFSET can be used)
**                  This allows enough space in the buffer for the L2CAP and
**                  AVDTP headers.
**
**                  The memory pointed to by p_pkt must be a GKI buffer
**                  allocated by the application.  This buffer will be freed
**                  by the protocol stack; the application must not free
**                  this buffer.
**
**                  The opt parameter allows passing specific options like:
**                  - NO_RTP : do not add the RTP header to buffer
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_WriteReqOpt(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp,
                               UINT8 m_pt, tAVDT_DATA_OPT_MASK opt);

/*******************************************************************************
**
** Function         AVDT_ConnectReq
**
** Description      This function initiates an AVDTP signaling connection
**                  to the peer device.  When the connection is completed, an
**                  AVDT_CONNECT_IND_EVT is sent to the application via its
**                  control callback function.  If the connection attempt fails
**                  an AVDT_DISCONNECT_IND_EVT is sent.  The security mask
**                  parameter overrides the outgoing security mask set in
**                  AVDT_Register().
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_ConnectReq(BD_ADDR bd_addr, UINT8 sec_mask,
                              tAVDT_CTRL_CBACK *p_cback);

/*******************************************************************************
**
** Function         AVDT_DisconnectReq
**
** Description      This function disconnect an AVDTP signaling connection
**                  to the peer device.  When disconnected an
**                  AVDT_DISCONNECT_IND_EVT is sent to the application via its
**                  control callback function.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_DisconnectReq(BD_ADDR bd_addr, tAVDT_CTRL_CBACK *p_cback);

/*******************************************************************************
**
** Function         AVDT_GetL2CapChannel
**
** Description      Get the L2CAP CID used by the handle.
**
** Returns          CID if successful, otherwise 0.
**
*******************************************************************************/
extern UINT16 AVDT_GetL2CapChannel(UINT8 handle);

/*******************************************************************************
**
** Function         AVDT_GetSignalChannel
**
** Description      Get the L2CAP CID used by the signal channel of the given handle.
**
** Returns          CID if successful, otherwise 0.
**
*******************************************************************************/
extern UINT16 AVDT_GetSignalChannel(UINT8 handle, BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         AVDT_WriteDataReq
**
** Description      Send a media packet to the peer device.  The stream must
**                  be started before this function is called.  Also, this
**                  function can only be called if the stream is a SRC.
**
**                  When AVDTP has sent the media packet and is ready for the
**                  next packet, an AVDT_WRITE_CFM_EVT is sent to the
**                  application via the control callback.  The application must
**                  wait for the AVDT_WRITE_CFM_EVT before it makes the next
**                  call to AVDT_WriteDataReq().  If the applications calls
**                  AVDT_WriteDataReq() before it receives the event the packet
**                  will not be sent.  The application may make its first call
**                  to AVDT_WriteDataReq() after it receives an
**                  AVDT_START_CFM_EVT or AVDT_START_IND_EVT.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_WriteDataReq(UINT8 handle, UINT8 *p_data, UINT32 data_len,
                                UINT32 time_stamp, UINT8 m_pt, UINT8 marker);

/*******************************************************************************
**
** Function         AVDT_SetMediaBuf
**
** Description      Assigns buffer for media packets or forbids using of assigned
**                  buffer if argument p_buf is NULL. This function can only
**                  be called if the stream is a SNK.
**
**                  AVDTP uses this buffer to reassemble fragmented media packets.
**                  When AVDTP receives a complete media packet, it calls the
**                  p_media_cback assigned by AVDT_CreateStream().
**                  This function can be called during callback to assign a
**                  different buffer for next media packet or can leave the current
**                  buffer for next packet.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_SetMediaBuf(UINT8 handle, UINT8 *p_buf, UINT32 buf_len);

/*******************************************************************************
**
** Function         AVDT_SendReport
**
** Description
**
**
**
** Returns
**
*******************************************************************************/
extern UINT16 AVDT_SendReport(UINT8 handle, AVDT_REPORT_TYPE type,
                              tAVDT_REPORT_DATA *p_data);

/******************************************************************************
**
** Function         AVDT_SetTraceLevel
**
** Description      Sets the trace level for AVDT. If 0xff is passed, the
**                  current trace level is returned.
**
**                  Input Parameters:
**                      new_level:  The level to set the AVDT tracing to:
**                      0xff-returns the current setting.
**                      0-turns off tracing.
**                      >= 1-Errors.
**                      >= 2-Warnings.
**                      >= 3-APIs.
**                      >= 4-Events.
**                      >= 5-Debug.
**
** Returns          The new trace level or current trace level if
**                  the input parameter is 0xff.
**
******************************************************************************/
extern UINT8 AVDT_SetTraceLevel (UINT8 new_level);

#ifdef __cplusplus
}
#endif


#endif /* AVDT_API_H */
