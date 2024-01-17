/******************************************************************************
 *
 *  Copyright (C) 2006-2012 Broadcom Corporation
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
 *  This is the private interface file for the BTA Java I/F
 *
 ******************************************************************************/
#ifndef BTA_JV_INT_H
#define BTA_JV_INT_H

#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta/bta_jv_api.h"
#include "stack/rfcdefs.h"
#include "stack/port_api.h"
#include "stack/sdp_api.h"

#include "common/bt_target.h"
#if (defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

#define SPP_VERSION             0x0102

enum {
    /* these events are handled by the state machine */
    BTA_JV_API_ENABLE_EVT = BTA_SYS_EVT_START(BTA_ID_JV),
    BTA_JV_API_DISABLE_EVT,
    BTA_JV_API_GET_CHANNEL_EVT,
    BTA_JV_API_FREE_SCN_EVT,
    BTA_JV_API_START_DISCOVERY_EVT,
    BTA_JV_API_CREATE_RECORD_EVT,
    BTA_JV_API_DELETE_RECORD_EVT,
#if BTA_JV_L2CAP_INCLUDED
    BTA_JV_API_L2CAP_CONNECT_EVT,
    BTA_JV_API_L2CAP_CLOSE_EVT,
    BTA_JV_API_L2CAP_START_SERVER_EVT,
    BTA_JV_API_L2CAP_STOP_SERVER_EVT,
    BTA_JV_API_L2CAP_READ_EVT,
    BTA_JV_API_L2CAP_WRITE_EVT,
#endif /* BTA_JV_L2CAP_INCLUDED */
#if BTA_JV_RFCOMM_INCLUDED
    BTA_JV_API_RFCOMM_CONFIG_EVT,
    BTA_JV_API_RFCOMM_CONNECT_EVT,
    BTA_JV_API_RFCOMM_CLOSE_EVT,
    BTA_JV_API_RFCOMM_START_SERVER_EVT,
    BTA_JV_API_RFCOMM_STOP_SERVER_EVT,
    BTA_JV_API_RFCOMM_READ_EVT,
    BTA_JV_API_RFCOMM_WRITE_EVT,
    BTA_JV_API_RFCOMM_FLOW_CONTROL_EVT,
#endif /* BTA_JV_RFCOMM_INCLUDED */
    BTA_JV_API_SET_PM_PROFILE_EVT,
    BTA_JV_API_PM_STATE_CHANGE_EVT,
#if BTA_JV_L2CAP_INCLUDED
    BTA_JV_API_L2CAP_CONNECT_LE_EVT,
    BTA_JV_API_L2CAP_START_SERVER_LE_EVT,
    BTA_JV_API_L2CAP_STOP_SERVER_LE_EVT,
    BTA_JV_API_L2CAP_WRITE_FIXED_EVT,
    BTA_JV_API_L2CAP_CLOSE_FIXED_EVT,
#endif /* BTA_JV_L2CAP_INCLUDED */
    BTA_JV_MAX_INT_EVT
};

#ifndef BTA_JV_RFC_EV_MASK
#define BTA_JV_RFC_EV_MASK (PORT_EV_RXCHAR | PORT_EV_TXEMPTY | PORT_EV_FC | PORT_EV_FCS)
#endif

/* data type for BTA_JV_API_ENABLE_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_JV_DM_CBACK   *p_cback;
} tBTA_JV_API_ENABLE;

/* data type for BTA_JV_API_DISABLE_EVT */
typedef struct {
    BT_HDR          hdr;
    tBTA_JV_RFCOMM_CBACK   *p_cback;
} tBTA_JV_API_DISABLE;

/* data type for BTA_JV_API_START_DISCOVERY_EVT */
typedef struct {
    BT_HDR      hdr;
    BD_ADDR bd_addr;
    UINT16 num_uuid;
    tSDP_UUID uuid_list[BTA_JV_MAX_UUIDS];
    UINT16 num_attr;
    UINT16 attr_list[BTA_JV_MAX_ATTRS];
    void            *user_data;      /* piggyback caller's private data*/
} tBTA_JV_API_START_DISCOVERY;

enum {
    BTA_JV_PM_FREE_ST = 0, /* empty PM slot */
    BTA_JV_PM_IDLE_ST,
    BTA_JV_PM_BUSY_ST
};

/* BTA JV PM control block */
typedef struct {
    UINT32          handle;     /* The connection handle */
    UINT8           state;      /* state: see above enum */
    tBTA_JV_PM_ID   app_id;     /* JV app specific id indicating power table to use */
    BD_ADDR         peer_bd_addr;    /* Peer BD address */
} tBTA_JV_PM_CB;

enum {
    BTA_JV_ST_NONE = 0,
    BTA_JV_ST_CL_OPENING,
    BTA_JV_ST_CL_OPEN,
    BTA_JV_ST_CL_CLOSING,
    BTA_JV_ST_SR_LISTEN,
    BTA_JV_ST_SR_OPEN,
    BTA_JV_ST_SR_CLOSING
} ;
typedef UINT8  tBTA_JV_STATE;
#define BTA_JV_ST_CL_MAX    BTA_JV_ST_CL_CLOSING

#if BTA_JV_L2CAP_INCLUDED
/* JV L2CAP control block */
typedef struct {
    tBTA_JV_L2CAP_CBACK *p_cback;   /* the callback function */
    UINT16              psm;        /* the psm used for this server connection */
    tBTA_JV_STATE       state;      /* the state of this control block */
    tBTA_SERVICE_ID     sec_id;     /* service id */
    UINT32              handle;     /* the handle reported to java app (same as gap handle) */
    BOOLEAN             cong;       /* TRUE, if congested */
    tBTA_JV_PM_CB      *p_pm_cb;    /* ptr to pm control block, NULL: unused */
    void                *user_data; /* user data for callback from higher layers */
} tBTA_JV_L2C_CB;
#endif /* BTA_JV_L2CAP_INCLUDED */

#define BTA_JV_RFC_HDL_MASK         0xFF
#define BTA_JV_RFCOMM_MASK          0x80
#define BTA_JV_ALL_APP_ID           0xFF
#define BTA_JV_RFC_HDL_TO_SIDX(r)   (((r)&0xFF00) >> 8)
#define BTA_JV_RFC_H_S_TO_HDL(h, s) ((h)|(s<<8))

/* port control block */
typedef struct {
    UINT32              handle;     /* the rfcomm session handle at jv */
    UINT16              port_handle;/* port handle */
    tBTA_JV_STATE       state;      /* the state of this control block */
    UINT8               max_sess;   /* max sessions */
    void                *user_data; /* piggyback caller's private data*/
    BOOLEAN             cong;       /* TRUE, if congested */
    tBTA_JV_PM_CB       *p_pm_cb;   /* ptr to pm control block, NULL: unused */
} tBTA_JV_PCB;

#if BTA_JV_RFCOMM_INCLUDED
/* JV RFCOMM control block */
typedef struct {
    tBTA_JV_RFCOMM_CBACK *p_cback;  /* the callback function */
    UINT16              rfc_hdl[BTA_JV_MAX_RFC_SR_SESSION];
    tBTA_SERVICE_ID     sec_id;     /* service id */
    UINT8               handle;     /* index: the handle reported to java app */
    UINT8               scn;        /* the scn of the server */
    UINT8               max_sess;   /* max sessions */
    int                 curr_sess;   /* current sessions count*/
} tBTA_JV_RFC_CB;
#endif /* BTA_JV_RFCOMM_INCLUDED */

#if BTA_JV_L2CAP_INCLUDED
/* data type for BTA_JV_API_L2CAP_CONNECT_EVT & BTA_JV_API_L2CAP_CONNECT_LE_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_SEC            sec_mask;
    tBTA_JV_ROLE        role;
    union {
        UINT16          remote_psm;
        UINT16          remote_chan;
    };
    UINT16              rx_mtu;
    BD_ADDR             peer_bd_addr;
    INT32               has_cfg;
    tL2CAP_CFG_INFO     cfg;
    INT32               has_ertm_info;
    tL2CAP_ERTM_INFO    ertm_info;
    tBTA_JV_L2CAP_CBACK *p_cback;
    void                *user_data;
} tBTA_JV_API_L2CAP_CONNECT;

/* data type for BTA_JV_API_L2CAP_SERVER_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_SEC            sec_mask;
    tBTA_JV_ROLE        role;
    union {
        UINT16          local_psm;
        UINT16          local_chan;
    };
    UINT16              rx_mtu;
    INT32               has_cfg;
    tL2CAP_CFG_INFO     cfg;
    INT32               has_ertm_info;
    tL2CAP_ERTM_INFO    ertm_info;
    tBTA_JV_L2CAP_CBACK *p_cback;
    void                *user_data;
} tBTA_JV_API_L2CAP_SERVER;

/* data type for BTA_JV_API_L2CAP_CLOSE_EVT */
typedef struct {
    BT_HDR          hdr;
    UINT32          handle;
    tBTA_JV_L2C_CB  *p_cb;
    tBTA_JV_L2CAP_CBACK *p_cback;
    void            *user_data;
} tBTA_JV_API_L2CAP_CLOSE;

/* data type for BTA_JV_API_L2CAP_READ_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT32              handle;
    UINT32              req_id;
    tBTA_JV_L2CAP_CBACK *p_cback;
    UINT8              *p_data;
    UINT16              len;
    void                *user_data;
} tBTA_JV_API_L2CAP_READ;

/* data type for BTA_JV_API_L2CAP_WRITE_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT32              handle;
    UINT32              req_id;
    tBTA_JV_L2C_CB      *p_cb;
    UINT8               *p_data;
    UINT16              len;
    void                *user_data;
} tBTA_JV_API_L2CAP_WRITE;

/* data type for BTA_JV_API_L2CAP_WRITE_FIXED_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT16              channel;
    BD_ADDR             addr;
    UINT32              req_id;
    tBTA_JV_L2CAP_CBACK *p_cback;
    UINT8               *p_data;
    UINT16              len;
    void                *user_data;
} tBTA_JV_API_L2CAP_WRITE_FIXED;
#endif /* BTA_JV_L2CAP_INCLUDED */

#if BTA_JV_RFCOMM_INCLUDED
/* data type for BTA_JV_API_RFCOMM_CONFIG_EVT */
typedef struct {
    BT_HDR          hdr;
    BOOLEAN         enable_l2cap_ertm;
} tBTA_JV_API_RFCOMM_CONFIG;

/* data type for BTA_JV_API_RFCOMM_CONNECT_EVT */
typedef struct {
    BT_HDR          hdr;
    tBTA_SEC        sec_mask;
    tBTA_JV_ROLE    role;
    UINT8           remote_scn;
    BD_ADDR         peer_bd_addr;
    tBTA_JV_RFCOMM_CBACK *p_cback;
    void            *user_data;
} tBTA_JV_API_RFCOMM_CONNECT;

/* data type for BTA_JV_API_RFCOMM_SERVER_EVT */
typedef struct {
    BT_HDR          hdr;
    tBTA_SEC        sec_mask;
    tBTA_JV_ROLE    role;
    UINT8           local_scn;
    UINT8           max_session;
    UINT32          handle;
    tBTA_JV_RFCOMM_CBACK *p_cback;
    void            *user_data;
} tBTA_JV_API_RFCOMM_SERVER;

/* data type for BTA_JV_API_RFCOMM_READ_EVT */
typedef struct {
    BT_HDR          hdr;
    UINT32          handle;
    UINT32          req_id;
    UINT8           *p_data;
    UINT16          len;
    tBTA_JV_RFC_CB  *p_cb;
    tBTA_JV_PCB     *p_pcb;
} tBTA_JV_API_RFCOMM_READ;
#endif /* BTA_JV_RFCOMM_INCLUDED */

/* data type for BTA_JV_API_SET_PM_PROFILE_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT32              handle;
    tBTA_JV_PM_ID       app_id;
    tBTA_JV_CONN_STATE  init_st;
} tBTA_JV_API_SET_PM_PROFILE;

/* data type for BTA_JV_API_PM_STATE_CHANGE_EVT */
typedef struct {
    BT_HDR              hdr;
    tBTA_JV_PM_CB       *p_cb;
    tBTA_JV_CONN_STATE  state;
} tBTA_JV_API_PM_STATE_CHANGE;

#if BTA_JV_RFCOMM_INCLUDED
/* data type for BTA_JV_API_RFCOMM_WRITE_EVT */
typedef struct {
    BT_HDR          hdr;
    UINT32          handle;
    UINT32          req_id;
    UINT8           *p_data;
    int             len;
    tBTA_JV_RFC_CB  *p_cb;
    tBTA_JV_PCB     *p_pcb;
} tBTA_JV_API_RFCOMM_WRITE;

/* data type for BTA_JV_API_RFCOMM_FLOW_CONTROL_EVT */
typedef struct {
    BT_HDR      hdr;
    tBTA_JV_RFC_CB  *p_cb;
    tBTA_JV_PCB     *p_pcb;
    UINT16      credits_given;
} tBTA_JV_API_RFCOMM_FLOW_CONTROL;

/* data type for BTA_JV_API_RFCOMM_CLOSE_EVT */
typedef struct {
    BT_HDR          hdr;
    UINT32          handle;
    tBTA_JV_RFC_CB  *p_cb;
    tBTA_JV_PCB     *p_pcb;
    void            *user_data;
} tBTA_JV_API_RFCOMM_CLOSE;
#endif /* BTA_JV_RFCOMM_INCLUDED */

/* data type for BTA_JV_API_CREATE_RECORD_EVT */
typedef struct {
    BT_HDR      hdr;
#define ESP_SDP_SERVER_NAME_MAX (32)
    char name[ESP_SDP_SERVER_NAME_MAX + 1];
    INT32       channel;
    void        *user_data;
} tBTA_JV_API_CREATE_RECORD;

/* data type for BTA_JV_API_ADD_ATTRIBUTE_EVT */
typedef struct {
    BT_HDR      hdr;
    UINT32      handle;
    UINT16      attr_id;
    UINT8       *p_value;
    INT32       value_size;
} tBTA_JV_API_ADD_ATTRIBUTE;

/* data type for BTA_JV_API_FREE_SCN_EVT */
typedef struct {
    BT_HDR      hdr;
    INT32       type;       /* One of BTA_JV_CONN_TYPE_ */
    UINT16      scn;
    tBTA_JV_RFCOMM_CBACK *p_cback;
    void        *user_data;
} tBTA_JV_API_FREE_CHANNEL;

/* data type for BTA_JV_API_ALLOC_CHANNEL_EVT */
typedef struct {
    BT_HDR      hdr;
    INT32       type;       /* One of BTA_JV_CONN_TYPE_ */
    INT32       channel;    /* optionally request a specific channel */
    void        *user_data;
} tBTA_JV_API_ALLOC_CHANNEL;
/* union of all data types */
typedef union {
    /* GKI event buffer header */
    BT_HDR                          hdr;
    tBTA_JV_API_ENABLE              enable;
    tBTA_JV_API_DISABLE             disable;
    tBTA_JV_API_START_DISCOVERY     start_discovery;
    tBTA_JV_API_ALLOC_CHANNEL       alloc_channel;
    tBTA_JV_API_FREE_CHANNEL        free_channel;
    tBTA_JV_API_CREATE_RECORD       create_record;
    tBTA_JV_API_ADD_ATTRIBUTE       add_attr;
#if BTA_JV_L2CAP_INCLUDED
    tBTA_JV_API_L2CAP_CONNECT       l2cap_connect;
    tBTA_JV_API_L2CAP_READ          l2cap_read;
    tBTA_JV_API_L2CAP_WRITE         l2cap_write;
    tBTA_JV_API_L2CAP_CLOSE         l2cap_close;
    tBTA_JV_API_L2CAP_SERVER        l2cap_server;
    tBTA_JV_API_L2CAP_WRITE_FIXED   l2cap_write_fixed;
#endif /* BTA_JV_L2CAP_INCLUDED */
#if BTA_JV_RFCOMM_INCLUDED
    tBTA_JV_API_RFCOMM_CONFIG       rfcomm_config;
    tBTA_JV_API_RFCOMM_CONNECT      rfcomm_connect;
    tBTA_JV_API_RFCOMM_READ         rfcomm_read;
    tBTA_JV_API_RFCOMM_WRITE        rfcomm_write;
    tBTA_JV_API_RFCOMM_FLOW_CONTROL rfcomm_fc;
    tBTA_JV_API_RFCOMM_CLOSE        rfcomm_close;
    tBTA_JV_API_RFCOMM_SERVER       rfcomm_server;
#endif /* BTA_JV_RFCOMM_INCLUDED */
    tBTA_JV_API_SET_PM_PROFILE      set_pm;
    tBTA_JV_API_PM_STATE_CHANGE     change_pm_state;
} tBTA_JV_MSG;

/* JV control block */
typedef struct {
    /* the SDP handle reported to JV user is the (index + 1) to sdp_handle[].
     * if sdp_handle[i]==0, it's not used.
     * otherwise sdp_handle[i] is the stack SDP handle. */
    UINT32                  sdp_handle[BTA_JV_MAX_SDP_REC]; /* SDP records created */
    UINT8                   *p_sel_raw_data;/* the raw data of last service select */
    tBTA_JV_DM_CBACK        *p_dm_cback;
#if BTA_JV_L2CAP_INCLUDED
    tBTA_JV_L2C_CB          l2c_cb[BTA_JV_MAX_L2C_CONN];    /* index is GAP handle (index) */
#endif /* BTA_JV_L2CAP_INCLUDED */
#if BTA_JV_RFCOMM_INCLUDED
    tBTA_JV_RFC_CB          rfc_cb[BTA_JV_MAX_RFC_CONN];
#endif /* BTA_JV_RFCOMM_INCLUDED */
    tBTA_JV_PCB             port_cb[MAX_RFC_PORTS];         /* index of this array is
                                                               the port_handle, */
    UINT8                   sec_id[BTA_JV_NUM_SERVICE_ID];  /* service ID */
    BOOLEAN                 scn[BTA_JV_MAX_SCN];            /* SCN allocated by java */
    UINT16                  free_psm_list[BTA_JV_MAX_L2C_CONN];  /* PSMs freed by java
                                                                    (can be reused) */
    UINT8                   sdp_active;                     /* see BTA_JV_SDP_ACT_* */
    tSDP_UUID               uuid;                           /* current uuid of sdp discovery*/
    tBTA_JV_PM_CB           pm_cb[BTA_JV_PM_MAX_NUM];       /* PM on a per JV handle bases */
} tBTA_JV_CB;

enum {
    BTA_JV_SDP_ACT_NONE = 0,
    BTA_JV_SDP_ACT_YES,     /* waiting for SDP result */
    BTA_JV_SDP_ACT_CANCEL   /* waiting for cancel complete */
};

/* JV control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_JV_CB bta_jv_cb;
#else
extern tBTA_JV_CB *bta_jv_cb_ptr;
#define bta_jv_cb (*bta_jv_cb_ptr)
#endif

/* config struct */
extern tBTA_JV_CFG *p_bta_jv_cfg;

extern BOOLEAN bta_jv_sm_execute(BT_HDR *p_msg);

extern void bta_jv_enable (tBTA_JV_MSG *p_data);
extern void bta_jv_disable (tBTA_JV_MSG *p_data);
extern void bta_jv_get_channel_id (tBTA_JV_MSG *p_data);
extern void bta_jv_free_scn (tBTA_JV_MSG *p_data);
extern void bta_jv_start_discovery (tBTA_JV_MSG *p_data);
extern void bta_jv_create_record (tBTA_JV_MSG *p_data);
extern void bta_jv_delete_record (tBTA_JV_MSG *p_data);
#if BTA_JV_L2CAP_INCLUDED
extern void bta_jv_l2cap_connect (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_close (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_start_server (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_stop_server (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_read (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_write (tBTA_JV_MSG *p_data);
#endif /* BTA_JV_L2CAP_INCLUDED */
#if BTA_JV_RFCOMM_INCLUDED
extern void bta_jv_rfcomm_config (tBTA_JV_MSG *p_data);
extern void bta_jv_rfcomm_connect (tBTA_JV_MSG *p_data);
extern void bta_jv_rfcomm_close (tBTA_JV_MSG *p_data);
extern void bta_jv_rfcomm_start_server (tBTA_JV_MSG *p_data);
extern void bta_jv_rfcomm_stop_server (tBTA_JV_MSG *p_data);
extern void bta_jv_rfcomm_read (tBTA_JV_MSG *p_data);
extern void bta_jv_rfcomm_write (tBTA_JV_MSG *p_data);
extern void bta_jv_rfcomm_flow_control(tBTA_JV_MSG *p_data);
#endif /* BTA_JV_RFCOMM_INCLUDED */
extern void bta_jv_set_pm_profile (tBTA_JV_MSG *p_data);
extern void bta_jv_change_pm_state(tBTA_JV_MSG *p_data);
#if BTA_JV_L2CAP_INCLUDED
extern void bta_jv_l2cap_connect_le (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_start_server_le (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_stop_server_le (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_write_fixed (tBTA_JV_MSG *p_data);
extern void bta_jv_l2cap_close_fixed (tBTA_JV_MSG *p_data);
#endif /* BTA_JV_L2CAP_INCLUDED */

#endif  ///defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE
#endif /* BTA_JV_INT_H */
