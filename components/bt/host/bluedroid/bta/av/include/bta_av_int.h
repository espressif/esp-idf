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
 *  This is the private interface file for the BTA advanced audio/video.
 *
 ******************************************************************************/
#ifndef BTA_AV_INT_H
#define BTA_AV_INT_H

#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta/bta_av_api.h"
#include "stack/avdt_api.h"
#include "bta/bta_av_co.h"
#include "osi/list.h"

#if (BTA_AV_INCLUDED == TRUE)

/*****************************************************************************
**  Constants
*****************************************************************************/

enum {
    /* these events are handled by the AV main state machine */
    BTA_AV_API_DISABLE_EVT = BTA_SYS_EVT_START(BTA_ID_AV),
    BTA_AV_API_REMOTE_CMD_EVT,
    BTA_AV_API_VENDOR_CMD_EVT,
    BTA_AV_API_VENDOR_RSP_EVT,
    BTA_AV_API_META_RSP_EVT,
    BTA_AV_API_RC_CLOSE_EVT,
    BTA_AV_AVRC_OPEN_EVT,
    BTA_AV_AVRC_MSG_EVT,
    BTA_AV_AVRC_NONE_EVT,

    /* these events are handled by the AV stream state machine */
    BTA_AV_API_OPEN_EVT,
    BTA_AV_API_CLOSE_EVT,
    BTA_AV_AP_START_EVT,        /* the following 3 events must be in the same order as the *API_*EVT */
    BTA_AV_AP_STOP_EVT,
    BTA_AV_AP_SET_DELAY_VALUE_EVT,
    BTA_AV_API_RECONFIG_EVT,
    BTA_AV_API_PROTECT_REQ_EVT,
    BTA_AV_API_PROTECT_RSP_EVT,
    BTA_AV_API_RC_OPEN_EVT,
    BTA_AV_SRC_DATA_READY_EVT,
    BTA_AV_CI_SETCONFIG_OK_EVT,
    BTA_AV_CI_SETCONFIG_FAIL_EVT,
    BTA_AV_SDP_DISC_OK_EVT,
    BTA_AV_SDP_DISC_FAIL_EVT,
    BTA_AV_STR_DISC_OK_EVT,
    BTA_AV_STR_DISC_FAIL_EVT,
    BTA_AV_STR_GETCAP_OK_EVT,
    BTA_AV_STR_GETCAP_FAIL_EVT,
    BTA_AV_STR_OPEN_OK_EVT,
    BTA_AV_STR_OPEN_FAIL_EVT,
    BTA_AV_STR_START_OK_EVT,
    BTA_AV_STR_START_FAIL_EVT,
    BTA_AV_STR_CLOSE_EVT,
    BTA_AV_STR_CONFIG_IND_EVT,
    BTA_AV_STR_SECURITY_IND_EVT,
    BTA_AV_STR_SECURITY_CFM_EVT,
    BTA_AV_STR_WRITE_CFM_EVT,
    BTA_AV_STR_SUSPEND_CFM_EVT,
    BTA_AV_STR_RECONFIG_CFM_EVT,
    BTA_AV_AVRC_TIMER_EVT,
    BTA_AV_AVDT_CONNECT_EVT,
    BTA_AV_AVDT_DISCONNECT_EVT,
    BTA_AV_ROLE_CHANGE_EVT,
    BTA_AV_AVDT_DELAY_RPT_EVT,
    BTA_AV_ACP_CONNECT_EVT,
#if BTA_AV_CA_INCLUDED
    /* these events are handled by the Cover Art Client state machine */
    BTA_AV_API_CA_OPEN_EVT,
    BTA_AV_API_CA_CLOSE_EVT,
    BTA_AV_API_CA_GET_EVT,
    BTA_AV_CA_RESPONSE_EVT,
    BTA_AV_CA_RESPONSE_FINAL_EVT,
    BTA_AV_CA_GOEP_CONNECT_EVT,
    BTA_AV_CA_GOEP_DISCONNECT_EVT,
#endif
    /* these events are handled outside of the state machine */
    BTA_AV_API_ENABLE_EVT,
    BTA_AV_API_REGISTER_EVT,
    BTA_AV_API_DEREGISTER_EVT,
    BTA_AV_API_DISCONNECT_EVT,
    BTA_AV_CI_SRC_DATA_READY_EVT,
    BTA_AV_SIG_CHG_EVT,
    BTA_AV_SIG_TIMER_EVT,
    BTA_AV_SDP_AVRC_DISC_EVT,
    BTA_AV_AVRC_CLOSE_EVT,
    BTA_AV_CONN_CHG_EVT,
    BTA_AV_DEREG_COMP_EVT,
#if (BTA_AV_SINK_INCLUDED == TRUE)
    BTA_AV_API_SINK_ENABLE_EVT,
    // add
    BTA_AV_API_GET_DELAY_VALUE_EVT,
#endif
#if (AVDT_REPORTING == TRUE)
    BTA_AV_AVDT_RPT_CONN_EVT,
#endif
    BTA_AV_API_START_EVT,       /* the following 3 events must be in the same order as the *AP_*EVT */
    BTA_AV_API_STOP_EVT,
    BTA_AV_API_SET_DELAY_VALUE_EVT,
};

/* events for AV control block state machine */
#define BTA_AV_FIRST_SM_EVT     BTA_AV_API_DISABLE_EVT
#define BTA_AV_LAST_SM_EVT      BTA_AV_AVRC_NONE_EVT

#if BTA_AV_CA_INCLUDED
/* events for AVRC Cover Art state machine */
#define BTA_AV_CA_FIRST_SM_EVT     BTA_AV_API_CA_OPEN_EVT
#define BTA_AV_CA_LAST_SM_EVT      BTA_AV_CA_GOEP_DISCONNECT_EVT
#endif

/* events for AV stream control block state machine */
#define BTA_AV_FIRST_SSM_EVT    BTA_AV_API_OPEN_EVT

/* events that do not go through state machine */
#define BTA_AV_FIRST_NSM_EVT    BTA_AV_API_ENABLE_EVT
#define BTA_AV_LAST_NSM_EVT     BTA_AV_API_SET_DELAY_VALUE_EVT

/* API events passed to both SSMs (by bta_av_api_to_ssm) */
#define BTA_AV_FIRST_A2S_API_EVT    BTA_AV_API_START_EVT
#define BTA_AV_FIRST_A2S_SSM_EVT    BTA_AV_AP_START_EVT

#define BTA_AV_LAST_EVT             BTA_AV_API_SET_DELAY_VALUE_EVT

/* maximum number of SEPS in stream discovery results */
#define BTA_AV_NUM_SEPS         32

/* initialization value for AVRC handle */
#define BTA_AV_RC_HANDLE_NONE   0xFF

/* size of database for service discovery */
#define BTA_AV_DISC_BUF_SIZE        1000

/* offset of media type in codec info byte array */
#define BTA_AV_MEDIA_TYPE_IDX       1

/* maximum length of AVDTP security data */
#define BTA_AV_SECURITY_MAX_LEN     400

/* check number of buffers queued at L2CAP when this amount of buffers are queued to L2CAP */
#define BTA_AV_QUEUE_DATA_CHK_NUM   L2CAP_HIGH_PRI_MIN_XMIT_QUOTA

/* the number of ACL links with AVDT */
#define BTA_AV_NUM_LINKS            AVDT_NUM_LINKS

#define BTA_AV_CO_ID_TO_BE_STREAM(p, u32) {*(p)++ = (UINT8)((u32) >> 16); *(p)++ = (UINT8)((u32) >> 8); *(p)++ = (UINT8)(u32); }
#define BTA_AV_BE_STREAM_TO_CO_ID(u32, p) {u32 = (((UINT32)(*((p) + 2))) + (((UINT32)(*((p) + 1))) << 8) + (((UINT32)(*(p))) << 16)); (p) += 3;}

/* these bits are defined for bta_av_cb.multi_av */
#define BTA_AV_MULTI_AV_SUPPORTED   0x01
#define BTA_AV_MULTI_AV_IN_USE      0x02

#define TSEP_TO_SYS_ID(x) ((x) == AVDT_TSEP_SRC ? BTA_ID_AV : BTA_ID_AVK)

/*****************************************************************************
**  Data types
*****************************************************************************/
/* data type for BTA_AV_API_ENABLE_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_AV_CBACK       *p_cback;
    tBTA_AV_FEAT        features;
    tBTA_SEC            sec_mask;
} tBTA_AV_API_ENABLE;

/* data type for BTA_AV_API_REG_EVT */
typedef struct {
    BT_HDR              hdr;
    char                p_service_name[BTA_SERVICE_NAME_LEN + 1];
    UINT8               app_id;
    UINT8               tsep; // local SEP type
    tBTA_AV_DATA_CBACK  *p_app_data_cback;
    tBTA_AV_CO_FUNCTS   *bta_av_cos;
    tBTA_AVRC_CO_FUNCTS *bta_avrc_cos;
} tBTA_AV_API_REG;


enum {
    BTA_AV_RS_NONE,     /* straight API call */
    BTA_AV_RS_OK,       /* the role switch result - successful */
    BTA_AV_RS_FAIL,     /* the role switch result - failed */
    BTA_AV_RS_DONE      /* the role switch is done - continue */
};
typedef UINT8 tBTA_AV_RS_RES;
/* data type for BTA_AV_API_OPEN_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    BOOLEAN             use_rc;
    tBTA_SEC            sec_mask;
    tBTA_AV_RS_RES      switch_res;
    UINT16              uuid;  /* uuid of initiator */
} tBTA_AV_API_OPEN;

/* data type for BTA_AV_API_STOP_EVT */
typedef struct {
    BT_HDR              hdr;
    BOOLEAN             suspend;
    BOOLEAN             flush;
} tBTA_AV_API_STOP;

/* data type for BTA_AV_API_DISCONNECT_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
} tBTA_AV_API_DISCNT;

/* data type for BTA_AV_API_PROTECT_REQ_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT8               *p_data;
    UINT16              len;
} tBTA_AV_API_PROTECT_REQ;

/* data type for BTA_AV_API_PROTECT_RSP_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT8               *p_data;
    UINT16              len;
    UINT8               error_code;
} tBTA_AV_API_PROTECT_RSP;

/* data type for BTA_AV_API_REMOTE_CMD_EVT */
typedef struct {
    BT_HDR              hdr;
    tAVRC_MSG_PASS      msg;
    UINT8               label;
} tBTA_AV_API_REMOTE_CMD;

/* data type for BTA_AV_API_VENDOR_CMD_EVT and RSP */
typedef struct {
    BT_HDR              hdr;
    tAVRC_MSG_VENDOR    msg;
    UINT8               label;
} tBTA_AV_API_VENDOR;

/* data type for BTA_AV_API_RC_OPEN_EVT */
typedef struct {
    BT_HDR              hdr;
} tBTA_AV_API_OPEN_RC;

/* data type for BTA_AV_API_RC_CLOSE_EVT */
typedef struct {
    BT_HDR              hdr;
} tBTA_AV_API_CLOSE_RC;

/* data type for BTA_AV_API_META_RSP_EVT */
typedef struct {
    BT_HDR              hdr;
    BOOLEAN             is_rsp;
    UINT8               label;
    tBTA_AV_CODE        rsp_code;
    BT_HDR              *p_pkt;
} tBTA_AV_API_META_RSP;


/* data type for BTA_AV_API_RECONFIG_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT8               codec_info[AVDT_CODEC_SIZE];    /* codec configuration */
    BOOLEAN             suspend;
    UINT8               sep_info_idx;
    UINT8               num_protect;
    UINT8               p_protect_info[0];
} tBTA_AV_API_RCFG;

/* data type for BTA_AV_CI_SETCONFIG_OK_EVT and BTA_AV_CI_SETCONFIG_FAIL_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_AV_HNDL        hndl;
    UINT8               err_code;
    UINT8               category;
    BOOLEAN             recfg_needed;
    UINT8               avdt_handle;  /* local sep type for which this stream will be set up */
    UINT8               num_seid;
    UINT8               p_seid[0];
} tBTA_AV_CI_SETCONFIG;

/* data type for all stream events from AVDTP */
typedef struct {
    BT_HDR              hdr;
    tAVDT_CFG           cfg;        /* configuration/capabilities parameters */
    tAVDT_CTRL          msg;        /* AVDTP callback message parameters */
    BD_ADDR             bd_addr;    /* bd address */
    UINT8               handle;
    UINT8               avdt_event;
    BOOLEAN             initiator;  /* TRUE, if local device initiates the SUSPEND */
} tBTA_AV_STR_MSG;

/* data type for BTA_AV_AVRC_MSG_EVT */
typedef struct {
    BT_HDR              hdr;
    tAVRC_MSG           msg;
    UINT8               handle;
    UINT8               label;
    UINT8               opcode;
} tBTA_AV_RC_MSG;

/* data type for BTA_AV_AVRC_OPEN_EVT, BTA_AV_AVRC_CLOSE_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             peer_addr;
    UINT8               handle;
} tBTA_AV_RC_CONN_CHG;

/* data type for BTA_AV_CONN_CHG_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             peer_addr;
    BOOLEAN             is_up;
} tBTA_AV_CONN_CHG;

/* data type for BTA_AV_ROLE_CHANGE_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT8               new_role;
    UINT8               hci_status;
} tBTA_AV_ROLE_RES;

/* data type for BTA_AV_SDP_DISC_OK_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT16              avdt_version;   /* AVDTP protocol version */
} tBTA_AV_SDP_RES;

/* type for SEP control block */
typedef struct {
    UINT8               av_handle;         /* AVDTP handle */
    tBTA_AV_CODEC       codec_type;        /* codec type */
    UINT8               tsep;              /* SEP type of local SEP */
    tBTA_AV_DATA_CBACK  *p_app_data_cback; /* Application callback for media packets */
} tBTA_AV_SEP;

/* data type for BTA_AV_API_SET_DELAY_VALUE_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT16              delay_value;
} tBTA_AV_API_SET_DELAY_VALUE;

/* data type for BTA_AV_API_GET_DELAY_VALUE_EVT */
typedef struct {
    BT_HDR              hdr;
} tBTA_AV_API_GET_DELAY_VALUE;

#if BTA_AV_CA_INCLUDED

/* data type for BTA_AV_API_CA_OPEN_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT16              mtu;
} tBTA_AV_API_CA_OPEN;

/* data type for BTA_AV_API_CA_CLOSE_EVT */
typedef struct {
    BT_HDR              hdr;
} tBTA_AV_API_CA_CLOSE;

/* data type for BTA_AV_API_CA_GET_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_AV_GET_TYPE    type;
    UINT8               image_handle[7];
    /* Image descriptor used in get image function */
    UINT16              image_descriptor_len;
    UINT8               *image_descriptor;
} tBTA_AV_API_CA_GET;

/* data type for BTA_AV_CA_RESPONSE_EVT and BTA_AV_CA_RESPONSE_FINAL_EVT */
typedef struct {
    BT_HDR              hdr;
    BT_HDR              *pkt;
    UINT8               opcode;
    BOOLEAN             srm_en;
    BOOLEAN             srm_wait;
} tBTA_AV_CA_RESPONSE;

/* data type for BTA_AV_CA_CONNECT_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT16              max_rx;
} tBTA_AV_CA_CONNECT;

/* data type for BTA_AV_CA_DISCONNECT_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT16              reason;
} tBTA_AV_CA_DISCONNECT;

#endif /* BTA_AV_CA_INCLUDED */

/* initiator/acceptor role for adaption */
#define BTA_AV_ROLE_AD_INT          0x00       /* initiator */
#define BTA_AV_ROLE_AD_ACP          0x01       /* acceptor */

/* initiator/acceptor signaling roles */
#define BTA_AV_ROLE_START_ACP       0x00
#define BTA_AV_ROLE_START_INT       0x10    /* do not change this value */

#define BTA_AV_ROLE_SUSPEND         0x20    /* suspending on start */
#define BTA_AV_ROLE_SUSPEND_OPT     0x40    /* Suspend on Start option is set */

/* union of all event datatypes */
typedef union {
    BT_HDR                  hdr;
    tBTA_AV_API_ENABLE      api_enable;
    tBTA_AV_API_REG         api_reg;
    tBTA_AV_API_OPEN        api_open;
    tBTA_AV_API_STOP        api_stop;
    tBTA_AV_API_DISCNT      api_discnt;
    tBTA_AV_API_PROTECT_REQ api_protect_req;
    tBTA_AV_API_PROTECT_RSP api_protect_rsp;
    tBTA_AV_API_REMOTE_CMD  api_remote_cmd;
    tBTA_AV_API_VENDOR      api_vendor;
    tBTA_AV_API_RCFG        api_reconfig;
    tBTA_AV_CI_SETCONFIG    ci_setconfig;
    tBTA_AV_STR_MSG         str_msg;
    tBTA_AV_RC_MSG          rc_msg;
    tBTA_AV_RC_CONN_CHG     rc_conn_chg;
    tBTA_AV_CONN_CHG        conn_chg;
    tBTA_AV_ROLE_RES        role_res;
    tBTA_AV_SDP_RES         sdp_res;
    tBTA_AV_API_META_RSP    api_meta_rsp;
    tBTA_AV_API_SET_DELAY_VALUE api_set_delay_vlaue;
    tBTA_AV_API_GET_DELAY_VALUE api_get_delay_value;
#if BTA_AV_CA_INCLUDED
    tBTA_AV_API_CA_OPEN     api_ca_open;
    tBTA_AV_API_CA_CLOSE    api_ca_close;
    tBTA_AV_API_CA_GET      api_ca_get;
    tBTA_AV_CA_RESPONSE     ca_response;
    tBTA_AV_CA_CONNECT      ca_connect;
    tBTA_AV_CA_DISCONNECT   ca_disconnect;
#endif
} tBTA_AV_DATA;

typedef void (tBTA_AV_VDP_DATA_ACT)(void *p_scb);

typedef struct {
    tBTA_AV_VDP_DATA_ACT    *p_act;
    UINT8                   *p_frame;
    UINT16                  buf_size;
    UINT32                  len;
    UINT32                  offset;
    UINT32                  timestamp;
} tBTA_AV_VF_INFO;

typedef union {
    tBTA_AV_VF_INFO     vdp;            /* used for video channels only */
    tBTA_AV_API_OPEN    open;           /* used only before open and role switch
                                           is needed on another AV channel */
} tBTA_AV_Q_INFO;

#define BTA_AV_Q_TAG_OPEN               0x01 /* after API_OPEN, before STR_OPENED */
#define BTA_AV_Q_TAG_START              0x02 /* before start sending media packets */
#define BTA_AV_Q_TAG_STREAM             0x03 /* during streaming */

#define BTA_AV_WAIT_ACP_CAPS_ON         0x01 /* retrieving the peer capabilities */
#define BTA_AV_WAIT_ACP_CAPS_STARTED    0x02 /* started while retrieving peer capabilities */
#define BTA_AV_WAIT_ROLE_SW_RES_OPEN    0x04 /* waiting for role switch result after API_OPEN, before STR_OPENED */
#define BTA_AV_WAIT_ROLE_SW_RES_START   0x08 /* waiting for role switch result before streaming */
#define BTA_AV_WAIT_ROLE_SW_STARTED     0x10 /* started while waiting for role switch result */
#define BTA_AV_WAIT_ROLE_SW_RETRY       0x20 /* set when retry on timeout */
#define BTA_AV_WAIT_CHECK_RC            0x40 /* set when the timer is used by role switch */
#define BTA_AV_WAIT_ROLE_SW_FAILED      0x80 /* role switch failed */

#define BTA_AV_WAIT_ROLE_SW_BITS        (BTA_AV_WAIT_ROLE_SW_RES_OPEN|BTA_AV_WAIT_ROLE_SW_RES_START|BTA_AV_WAIT_ROLE_SW_STARTED|BTA_AV_WAIT_ROLE_SW_RETRY)

/* Bitmap for collision, coll_mask */
#define BTA_AV_COLL_INC_TMR             0x01 /* Timer is running for incoming L2C connection */
#define BTA_AV_COLL_API_CALLED          0x02 /* API open was called while incoming timer is running */

/* type for AV stream control block */
typedef struct {
    const tBTA_AV_ACT   *p_act_tbl;     /* the action table for stream state machine */
    const tBTA_AV_CO_FUNCTS *p_cos;     /* the associated callout functions */
    tSDP_DISCOVERY_DB   *p_disc_db;     /* pointer to discovery database */
    tBTA_AV_SEP         seps[BTA_AV_MAX_SEPS];
    tAVDT_CFG           *p_cap;         /* buffer used for get capabilities */
    list_t              *a2d_list;      /* used for audio channels only */
    tBTA_AV_Q_INFO      q_info;
    tAVDT_SEP_INFO      sep_info[BTA_AV_NUM_SEPS];      /* stream discovery results */
    tAVDT_CFG           cfg;            /* local SEP configuration */
    TIMER_LIST_ENT      timer;          /* delay timer for AVRC CT */
    BD_ADDR             peer_addr;      /* peer BD address */
    UINT16              l2c_cid;        /* L2CAP channel ID */
    UINT16              stream_mtu;     /* MTU of stream */
    UINT16              avdt_version;   /* the avdt version of peer device */
    tBTA_SEC            sec_mask;       /* security mask */
    tBTA_AV_CODEC       codec_type;     /* codec type */
    UINT8               media_type;     /* Media type */
    BOOLEAN             cong;           /* TRUE if AVDTP congested */
    tBTA_AV_STATUS      open_status;    /* open failure status */
    tBTA_AV_CHNL        chnl;           /* the channel: audio/video */
    tBTA_AV_HNDL        hndl;           /* the handle: ((hdi + 1)|chnl) */
    UINT16              cur_psc_mask;   /* Protocol service capabilities mask for current connection */
    UINT8               avdt_handle;    /* AVDTP handle */
    UINT8               hdi;            /* the index to SCB[] */
    UINT8               num_seps;       /* number of seps returned by stream discovery */
    UINT8               num_disc_snks;  /* number of discovered snks */
    UINT8               num_disc_srcs;  /* number of discovered srcs */
    UINT8               sep_info_idx;   /* current index into sep_info */
    UINT8               sep_idx;        /* current index into local seps[] */
    UINT8               rcfg_idx;       /* reconfig requested index into sep_info */
    UINT8               state;          /* state machine state */
    UINT8               avdt_label;     /* AVDTP label */
    UINT8               app_id;         /* application id */
    UINT8               num_recfg;      /* number of reconfigure sent */
    UINT8               role;
    UINT8               l2c_bufs;       /* the number of buffers queued to L2CAP */
    UINT8               rc_handle;      /* connected AVRCP handle */
    BOOLEAN             use_rc;         /* TRUE if AVRCP is allowed */
    BOOLEAN             started;        /* TRUE if stream started */
    UINT8               co_started;     /* non-zero, if stream started from call-out perspective */
    BOOLEAN             recfg_sup;      /* TRUE if the first attempt to reconfigure the stream was successful, else False if command fails */
    BOOLEAN             suspend_sup;    /* TRUE if Suspend stream is supported, else FALSE if suspend command fails */
    BOOLEAN             deregistring;   /* TRUE if deregistering */
    BOOLEAN             sco_suspend;    /* TRUE if SUSPEND is issued automatically for SCO */
    UINT8               coll_mask;      /* Mask to check incoming and outgoing collision */
    tBTA_AV_API_OPEN    open_api;       /* Saved OPEN api message */
    UINT8               wait;           /* set 0x1, when getting Caps as ACP, set 0x2, when started */
    UINT8               q_tag;          /* identify the associated q_info union member */
    BOOLEAN             no_rtp_hdr;     /* TRUE if add no RTP header*/
    UINT8               disc_rsn;       /* disconnection reason */
    UINT16              uuid_int;       /*intended UUID of Initiator to connect to */
} tBTA_AV_SCB;

#define BTA_AV_RC_ROLE_MASK     0x10
#define BTA_AV_RC_ROLE_INT      0x00
#define BTA_AV_RC_ROLE_ACP      0x10

#define BTA_AV_RC_CONN_MASK     0x20

#define BTA_AV_CA_IMG_HDL_UTF16_LEN     16      /* Cover Art image handle in utf-16 format, fixed to 16 */

#define BTA_AV_CA_SRM_DISABLE       0x00
#define BTA_AV_CA_SRM_ENABLE_REQ    0x01
#define BTA_AV_CA_SRM_WAIT          0x02
#define BTA_AV_CA_SRM_ENABLE        0x03

/* type for AV RCP control block */
/* index to this control block is the rc handle */
typedef struct {
    UINT8   status;
    UINT8   handle;
    UINT8   shdl;   /* stream handle (hdi + 1) */
    UINT8   lidx;   /* (index+1) to LCB */
    tBTA_AV_FEAT        peer_features;  /* peer features mask */
    UINT16              peer_ct_features;
    UINT16              peer_tg_features;
#if BTA_AV_CA_INCLUDED
    UINT16              cover_art_l2cap_psm;    /* OBEX over L2CAP PSM */
    UINT16              cover_art_goep_hdl;     /* Cover Art client GOEP connection handle */
    UINT8               cover_art_state;        /* Cover Art client state machine */
    UINT32              cover_art_cid;          /* Cover Art client connection id */
    UINT16              cover_art_max_tx;       /* max packet length peer device can receive */
    UINT16              cover_art_max_rx;       /* max packet length we can receive */
#endif
} tBTA_AV_RCB;
#define BTA_AV_NUM_RCB      (BTA_AV_NUM_STRS  + 2)

enum {
    BTA_AV_LCB_FREE,
    BTA_AV_LCB_FIND
};

/* type for AV ACL Link control block */
typedef struct {
    BD_ADDR             addr;           /* peer BD address */
    UINT8               conn_msk;       /* handle mask of connected stream handle */
    UINT8               lidx;           /* index + 1 */
} tBTA_AV_LCB;

/* type for stream state machine action functions */
typedef void (*tBTA_AV_SACT)(tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);


/* type for AV control block */
typedef struct {
    tBTA_AV_SCB         *p_scb[BTA_AV_NUM_STRS];    /* stream control block */
    tSDP_DISCOVERY_DB   *p_disc_db;     /* pointer to discovery database */
    tBTA_AV_CBACK       *p_cback;       /* application callback function */
    tBTA_AV_RCB         rcb[BTA_AV_NUM_RCB];  /* RCB control block */
    tBTA_AV_LCB         lcb[BTA_AV_NUM_LINKS + 1]; /* link control block */
    TIMER_LIST_ENT      sig_tmr;        /* link timer */
    TIMER_LIST_ENT      acp_sig_tmr;    /* timer to monitor signalling when accepting */
    UINT32              sdp_a2d_handle; /* SDP record handle for audio src */
#if (BTA_AV_SINK_INCLUDED == TRUE)
    UINT32              sdp_a2d_snk_handle; /* SDP record handle for audio snk */
#endif
    UINT32              sdp_vdp_handle; /* SDP record handle for video src */
    tBTA_AV_FEAT        features;       /* features mask */
    tBTA_SEC            sec_mask;       /* security mask */
    tBTA_AV_HNDL        handle;         /* the handle for SDP activity */
    tBTA_AVRC_CO_FUNCTS *p_rc_cos;      /* AVRCP call-out functions */
    BOOLEAN             disabling;      /* TRUE if api disabled called */
    UINT8               disc;           /* (hdi+1) or (rc_handle|BTA_AV_CHNL_MSK) if p_disc_db is in use */
    UINT8               state;          /* state machine state */
    UINT8               conn_rc;        /* handle mask of connected RCP channels */
    UINT8               conn_audio;     /* handle mask of connected audio channels */
    UINT8               conn_video;     /* handle mask of connected video channels */
    UINT8               conn_lcb;       /* index mask of used LCBs */
    UINT8               audio_open_cnt; /* number of connected audio channels */
    UINT8               reg_audio;      /* handle mask of registered audio channels */
    UINT8               reg_video;      /* handle mask of registered video channels */
    UINT8               rc_acp_handle;
    UINT8               rc_acp_idx;     /* (index + 1) to RCB */
    UINT8               rs_idx;         /* (index + 1) to SCB for the one waiting for RS on open */
    BOOLEAN             sco_occupied;   /* TRUE if SCO is being used or call is in progress */
    UINT8               audio_streams;  /* handle mask of streaming audio channels */
    UINT8               video_streams;  /* handle mask of streaming video channels */
} tBTA_AV_CB;

/* type for dealing with SBC data frames and codec capabilities functions */
typedef int (tBTA_AV_SBC_ACT)(void *p_src, void *p_dst,
                              UINT32 src_samples, UINT32 dst_samples,
                              UINT32 *p_ret);

/* type for AV up sample control block */
typedef struct {
    INT32               cur_pos;    /* current position */
    UINT32              src_sps;    /* samples per second (source audio data) */
    UINT32              dst_sps;    /* samples per second (converted audio data) */
    tBTA_AV_SBC_ACT     *p_act;     /* the action function to do the conversion */
    UINT16              bits;       /* number of bits per pcm sample */
    UINT16              n_channels; /* number of channels (i.e. mono(1), stereo(2)...) */
    INT16               worker1;
    INT16               worker2;
    UINT8               div;
} tBTA_AV_SBC_UPS_CB;

/*****************************************************************************
**  Global data
*****************************************************************************/
/* control block declaration up sample */
#if BTA_DYNAMIC_MEMORY == TRUE
extern tBTA_AV_SBC_UPS_CB *bta_av_sbc_ups_cb_ptr;
#define bta_av_sbc_ups_cb (*bta_av_sbc_ups_cb_ptr)
#endif

/* control block declaration */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_AV_CB bta_av_cb;
#else
extern tBTA_AV_CB *bta_av_cb_ptr;
#define bta_av_cb (*bta_av_cb_ptr)
#endif

/* config struct */
extern tBTA_AV_CFG *p_bta_av_cfg;

extern const tBTA_AV_SACT bta_av_a2d_action[];
extern const tBTA_AV_SACT bta_av_vdp_action[];
extern tAVDT_CTRL_CBACK *const bta_av_dt_cback[];
extern void bta_av_stream_data_cback(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp, UINT8 m_pt);

/*****************************************************************************
**  Function prototypes
*****************************************************************************/
/* utility functions */
extern tBTA_AV_SCB *bta_av_hndl_to_scb(UINT16 handle);
extern BOOLEAN bta_av_chk_start(tBTA_AV_SCB *p_scb);
extern void bta_av_restore_switch (void);
extern UINT16 bta_av_chk_mtu(tBTA_AV_SCB *p_scb, UINT16 mtu);
extern void bta_av_conn_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data);
extern UINT8 bta_av_rc_create(tBTA_AV_CB *p_cb, UINT8 role, UINT8 shdl, UINT8 lidx);
extern void bta_av_stream_chg(tBTA_AV_SCB *p_scb, BOOLEAN started);
extern BOOLEAN bta_av_is_scb_opening (tBTA_AV_SCB *p_scb);
extern BOOLEAN bta_av_is_scb_incoming (tBTA_AV_SCB *p_scb);
extern void bta_av_set_scb_sst_init (tBTA_AV_SCB *p_scb);
extern BOOLEAN bta_av_is_scb_init (tBTA_AV_SCB *p_scb);
extern void bta_av_set_scb_sst_incoming (tBTA_AV_SCB *p_scb);
extern tBTA_AV_LCB *bta_av_find_lcb(BD_ADDR addr, UINT8 op);


/* debug functions */
extern char *bta_av_evt_code(UINT16 evt_code);
extern char *bta_av_action_code(UINT16 action_code);

/* main functions */
extern void bta_av_api_deregister(tBTA_AV_DATA *p_data);
extern void bta_av_dup_audio_buf(tBTA_AV_SCB *p_scb, BT_HDR *p_buf);
extern void bta_av_sm_execute(tBTA_AV_CB *p_cb, UINT16 event, tBTA_AV_DATA *p_data);
extern void bta_av_ssm_execute(tBTA_AV_SCB *p_scb, UINT16 event, tBTA_AV_DATA *p_data);
extern BOOLEAN bta_av_hdl_event(BT_HDR *p_msg);
extern BOOLEAN bta_av_switch_if_needed(tBTA_AV_SCB *p_scb);
extern BOOLEAN bta_av_link_role_ok(tBTA_AV_SCB *p_scb, UINT8 bits);
extern BOOLEAN bta_av_is_rcfg_sst(tBTA_AV_SCB *p_scb);

/* nsm action functions */
extern void bta_av_api_disconnect(tBTA_AV_DATA *p_data);
extern void bta_av_sig_chg(tBTA_AV_DATA *p_data);
extern void bta_av_sig_timer(tBTA_AV_DATA *p_data);
extern void bta_av_rc_disc_done(tBTA_AV_DATA *p_data);
extern void bta_av_rc_closed(tBTA_AV_DATA *p_data);
extern void bta_av_rc_disc(UINT8 disc);
extern void bta_av_conn_chg(tBTA_AV_DATA *p_data);
extern void bta_av_dereg_comp(tBTA_AV_DATA *p_data);

/* sm action functions */
extern void bta_av_disable (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_opened (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_remote_cmd (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_vendor_cmd (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_vendor_rsp (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_msg (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_close (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_meta_rsp (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_free_rsp (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);
extern void bta_av_rc_free_msg (tBTA_AV_CB *p_cb, tBTA_AV_DATA *p_data);

extern tBTA_AV_RCB *bta_av_get_rcb_by_shdl(UINT8 shdl);
extern void bta_av_del_rc(tBTA_AV_RCB *p_rcb);

/* ssm action functions */
extern void bta_av_do_disc_a2d (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_cleanup (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_free_sdb (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_config_ind (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_disconnect_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_rsp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_setconfig_rsp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_str_opened (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_ind (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_do_close (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_connect_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_sdp_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_disc_results (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_disc_res_as_acp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_open_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_getcap_results (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_setconfig_rej (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_discover_req (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_conn_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_do_start (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_str_stopped (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_reconfig (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_data_path (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_start_ok (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_start_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_str_closed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_clr_cong (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_suspend_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_str_ok (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_failed (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_connect (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_discntd (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_suspend_cont (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_cfm (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rcfg_open (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_security_rej (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_open_rc (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_chk_2nd_start (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_save_caps (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rej_conn (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_rej_conn (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_set_use_rc (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_cco_close (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_switch_role (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_role_res (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_delay_co (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_open_at_inc (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_open_fail_sdp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_set_delay_value (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);

/* ssm action functions - vdp specific */
extern void bta_av_do_disc_vdp (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_vdp_str_opened (tBTA_AV_SCB *p_scb, tBTA_AV_DATA *p_data);
extern void bta_av_reg_vdp (tAVDT_CS *p_cs, char *p_service_name, void *p_data);

#if BTA_AV_CA_INCLUDED
extern void bta_av_ca_api_open(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);
extern void bta_av_ca_api_close(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);
extern void bta_av_ca_api_get(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);
extern void bta_av_ca_response(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);
extern void bta_av_ca_response_final(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);
extern void bta_av_ca_goep_connect(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);
extern void bta_av_ca_goep_disconnect(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);
extern void bta_av_ca_force_disconnect(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);
extern void bta_av_ca_sm_execute(tBTA_AV_RCB *p_rcb, UINT16 event, tBTA_AV_DATA *p_data);
extern void bta_av_ca_reset(tBTA_AV_RCB *p_rcb);
#endif

#endif  ///BTA_AV_INCLUDED == TRUE

#endif /* BTA_AV_INT_H */
