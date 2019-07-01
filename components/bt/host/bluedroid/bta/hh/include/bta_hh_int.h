/******************************************************************************
 *
 *  Copyright (C) 2005-2012 Broadcom Corporation
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
 *  This file contains BTA HID Host internal definitions
 *
 ******************************************************************************/

#ifndef BTA_HH_INT_H
#define BTA_HH_INT_H

#include "bta/bta_sys.h"
#include "bta/utl.h"
#include "bta/bta_hh_api.h"

//#if BTA_HH_LE_INCLUDED == TRUE
#include "bta/bta_gatt_api.h"
//#endif
#if defined(BTA_HH_INCLUDED) && (BTA_HH_INCLUDED == TRUE)

/* can be moved to bta/bta_api.h */
#define BTA_HH_MAX_RPT_CHARS    8

#if (BTA_GATT_INCLUDED == FALSE || BLE_INCLUDED == FALSE)
#undef BTA_HH_LE_INCLUDED
#define BTA_HH_LE_INCLUDED      FALSE
#endif

/* state machine events, these events are handled by the state machine */
enum {
    BTA_HH_API_OPEN_EVT     = BTA_SYS_EVT_START(BTA_ID_HH),
    BTA_HH_API_CLOSE_EVT,
    BTA_HH_INT_OPEN_EVT,
    BTA_HH_INT_CLOSE_EVT,
    BTA_HH_INT_DATA_EVT,
    BTA_HH_INT_CTRL_DATA,
    BTA_HH_INT_HANDSK_EVT,
    BTA_HH_SDP_CMPL_EVT,
    BTA_HH_API_WRITE_DEV_EVT,
    BTA_HH_API_GET_DSCP_EVT,
    BTA_HH_API_MAINT_DEV_EVT,
    BTA_HH_OPEN_CMPL_EVT,
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    BTA_HH_GATT_CLOSE_EVT,
    BTA_HH_GATT_OPEN_EVT,
    BTA_HH_START_ENC_EVT,
    BTA_HH_ENC_CMPL_EVT,
    BTA_HH_GATT_READ_CHAR_CMPL_EVT,
    BTA_HH_GATT_WRITE_CHAR_CMPL_EVT,
    BTA_HH_GATT_READ_DESCR_CMPL_EVT,
    BTA_HH_GATT_WRITE_DESCR_CMPL_EVT,
    BTA_HH_API_SCPP_UPDATE_EVT,
    BTA_HH_GATT_ENC_CMPL_EVT,
#endif

    /* not handled by execute state machine */
    BTA_HH_API_ENABLE_EVT,
    BTA_HH_API_DISABLE_EVT,
    BTA_HH_DISC_CMPL_EVT
};
typedef UINT16 tBTA_HH_INT_EVT;         /* HID host internal events */

#define BTA_HH_INVALID_EVT      (BTA_HH_DISC_CMPL_EVT + 1)

/* event used to map between BTE event and BTA event */
#define BTA_HH_FST_TRANS_CB_EVT         BTA_HH_GET_RPT_EVT
#define BTA_HH_FST_BTE_TRANS_EVT        HID_TRANS_GET_REPORT

/* sub event code used for device maintainence API call */
#define BTA_HH_ADD_DEV          0
#define BTA_HH_REMOVE_DEV       1

/* state machine states */
enum {
    BTA_HH_NULL_ST,
    BTA_HH_IDLE_ST,
    BTA_HH_W4_CONN_ST,
    BTA_HH_CONN_ST
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    , BTA_HH_W4_SEC
#endif
    , BTA_HH_INVALID_ST   /* Used to check invalid states before executing SM function */

};
typedef UINT8 tBTA_HH_STATE;

/* data structure used to send a command/data to HID device */
typedef struct {
    BT_HDR           hdr;
    UINT8            t_type;
    UINT8            param;
    UINT8            rpt_id;
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    UINT8            srvc_id;
#endif
    UINT16           data;
    BT_HDR           *p_data;
} tBTA_HH_CMD_DATA;

/* data type for BTA_HH_API_ENABLE_EVT */
typedef struct {
    BT_HDR              hdr;
    UINT8               sec_mask;
    UINT8               service_name[BTA_SERVICE_NAME_LEN + 1];
    tBTA_HH_CBACK   *p_cback;
} tBTA_HH_API_ENABLE;

typedef struct {
    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    UINT8           sec_mask;
    tBTA_HH_PROTO_MODE  mode;
} tBTA_HH_API_CONN;

/* internal event data from BTE HID callback */
typedef struct {
    BT_HDR          hdr;
    BD_ADDR         addr;
    UINT32          data;
    BT_HDR          *p_data;
} tBTA_HH_CBACK_DATA;

typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bda;
    UINT16              attr_mask;
    UINT16              sub_event;
    UINT8               sub_class;
    UINT8               app_id;
    tBTA_HH_DEV_DSCP_INFO      dscp_info;
} tBTA_HH_MAINT_DEV;

#if BTA_HH_LE_INCLUDED == TRUE
typedef struct {
    BT_HDR              hdr;
    UINT16              conn_id;
    tBTA_GATT_REASON    reason;         /* disconnect reason code, not useful when connect event is reported */

} tBTA_HH_LE_CLOSE;

typedef struct {
    BT_HDR              hdr;
    UINT16              scan_int;
    UINT16              scan_win;
} tBTA_HH_SCPP_UPDATE;
#endif
/* union of all event data types */
typedef union {
    BT_HDR                   hdr;
    tBTA_HH_API_ENABLE       api_enable;
    tBTA_HH_API_CONN         api_conn;
    tBTA_HH_CMD_DATA         api_sndcmd;
    tBTA_HH_CBACK_DATA       hid_cback;
    tBTA_HH_STATUS           status;
    tBTA_HH_MAINT_DEV        api_maintdev;
#if BTA_HH_LE_INCLUDED == TRUE
    tBTA_HH_LE_CLOSE         le_close;
    tBTA_GATTC_OPEN          le_open;
    tBTA_HH_SCPP_UPDATE      le_scpp_update;
    tBTA_GATTC_ENC_CMPL_CB   le_enc_cmpl;
#endif
} tBTA_HH_DATA;

#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
typedef struct {
    UINT8                   index;
    BOOLEAN                 in_use;
    UINT8                   inst_id;    /* share service instance ID and report instance ID, as
                                           hi 4 for service instance ID, low 4 as charatceristic instance ID */
    tBTA_HH_RPT_TYPE        rpt_type;
    UINT16                  uuid;
    UINT8                   prop;
    UINT8                   rpt_id;
    BOOLEAN                 client_cfg_exist;
    UINT16                  client_cfg_value;
} tBTA_HH_LE_RPT;

#ifndef BTA_HH_LE_RPT_MAX
#define BTA_HH_LE_RPT_MAX       20
#endif

typedef struct {
    BOOLEAN                 in_use;
    tBTA_HH_LE_RPT          report[BTA_HH_LE_RPT_MAX];

#define BTA_HH_LE_PROTO_MODE_BIT        0x01
#define BTA_HH_LE_CP_BIT                0x02
    UINT8                   option_char; /* control point char exisit or not */

    BOOLEAN                 expl_incl_srvc;
    UINT8                   incl_srvc_inst; /* assuming only one included service : battery service */
    UINT8                   cur_expl_char_idx; /* currently discovering service index */
    UINT8                   *rpt_map;
    UINT16                  ext_rpt_ref;
    tBTA_HH_DEV_DESCR       descriptor;

} tBTA_HH_LE_HID_SRVC;

#ifndef BTA_HH_LE_HID_SRVC_MAX
#define BTA_HH_LE_HID_SRVC_MAX      1
#endif

/* convert a HID handle to the LE CB index */
#define BTA_HH_GET_LE_CB_IDX(x)         (((x) >> 4) - 1)
/* convert a GATT connection ID to HID device handle, it is the hi 4 bits of a UINT8 */
#define BTA_HH_GET_LE_DEV_HDL(x)        (UINT8)(((x)  + 1) << 4)
/* check to see if th edevice handle is a LE device handle */
#define BTA_HH_IS_LE_DEV_HDL(x)        ((x) & 0xf0)
#define BTA_HH_IS_LE_DEV_HDL_VALID(x)  (((x)>>4) <= BTA_HH_LE_MAX_KNOWN)
#endif

/* device control block */
typedef struct {
    tBTA_HH_DEV_DSCP_INFO  dscp_info;      /* report descriptor and DI information */
    BD_ADDR             addr;           /* BD-Addr of the HID device */
    UINT16              attr_mask;      /* attribute mask */
    UINT16              w4_evt;         /* W4_handshake event name */
    UINT8               index;          /* index number referenced to handle index */
    UINT8               sub_class;      /* Cod sub class */
    UINT8               sec_mask;       /* security mask */
    UINT8               app_id;         /* application ID for this connection */
    UINT8               hid_handle;     /* device handle : low 4 bits for regular HID: HID_HOST_MAX_DEVICES can not exceed 15;
                                                            high 4 bits for LE HID: GATT_MAX_PHY_CHANNEL can not exceed 15 */
    BOOLEAN             vp;             /* virtually unplug flag */
    BOOLEAN             in_use;         /* control block currently in use */
    BOOLEAN             incoming_conn;  /* is incoming connection? */
    UINT8               incoming_hid_handle;  /* temporary handle for incoming connection? */
    BOOLEAN             opened;         /* TRUE if device successfully opened HID connection */
    tBTA_HH_PROTO_MODE  mode;           /* protocol mode */
    tBTA_HH_STATE       state;          /* CB state */

#if (BTA_HH_LE_INCLUDED == TRUE)
#define BTA_HH_LE_DISC_NONE     0x00
#define BTA_HH_LE_DISC_HIDS     0x01
#define BTA_HH_LE_DISC_DIS      0x02
#define BTA_HH_LE_DISC_SCPS     0x04

    UINT8               disc_active;
    tBTA_HH_STATUS      status;
    tBTA_GATT_REASON    reason;
    BOOLEAN             is_le_device;
    tBTA_HH_LE_HID_SRVC hid_srvc[BTA_HH_LE_HID_SRVC_MAX];
    UINT16              conn_id;
    BOOLEAN             in_bg_conn;
    UINT8               total_srvc;
    UINT8               clt_cfg_idx;
    UINT8               cur_srvc_index; /* currently discovering service index */
    BOOLEAN             scps_supported;

#define BTA_HH_LE_SCPS_NOTIFY_NONE    0
#define BTA_HH_LE_SCPS_NOTIFY_SPT  0x01
#define BTA_HH_LE_SCPS_NOTIFY_ENB  0x02
    UINT8               scps_notify;   /* scan refresh supported/notification enabled */
#endif

    BOOLEAN             security_pending;
} tBTA_HH_DEV_CB;

/* key board parsing control block */
typedef struct {
    BOOLEAN             mod_key[4]; /* ctrl, shift(upper), Alt, GUI */
    BOOLEAN             num_lock;
    BOOLEAN             caps_lock;
    UINT8               last_report[BTA_HH_MAX_RPT_CHARS];
} tBTA_HH_KB_CB;

/******************************************************************************
** Main Control Block
*******************************************************************************/
typedef struct {
    tBTA_HH_KB_CB           kb_cb;                  /* key board control block,
                                                       suppose BTA will connect
                                                       to only one keyboard at
                                                        the same time */
    tBTA_HH_DEV_CB          kdev[BTA_HH_MAX_DEVICE]; /* device control block */
    tBTA_HH_DEV_CB         *p_cur;              /* current device control
                                                       block idx, used in sdp */
    UINT8                   cb_index[BTA_HH_MAX_KNOWN]; /* maintain a CB index
                                                        map to dev handle */
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    UINT8                   le_cb_index[BTA_HH_MAX_DEVICE]; /* maintain a CB index map to LE dev handle */
    tBTA_GATTC_IF           gatt_if;
#endif
    tBTA_HH_CBACK       *p_cback;               /* Application callbacks */
    tSDP_DISCOVERY_DB      *p_disc_db;
    UINT8                   trace_level;            /* tracing level */
    UINT8                   cnt_num;                /* connected device number */
    BOOLEAN                 w4_disable;             /* w4 disable flag */
}
tBTA_HH_CB;

#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_HH_CB  bta_hh_cb;
#else
extern tBTA_HH_CB *bta_hh_cb_ptr;
#define bta_hh_cb (*bta_hh_cb_ptr)
#endif

/* from bta_hh_cfg.c */
extern tBTA_HH_CFG *p_bta_hh_cfg;

/*****************************************************************************
**  Function prototypes
*****************************************************************************/
extern BOOLEAN bta_hh_hdl_event(BT_HDR *p_msg);
extern void bta_hh_sm_execute(tBTA_HH_DEV_CB *p_cb, UINT16 event,
                              tBTA_HH_DATA *p_data);

/* action functions */
extern void bta_hh_api_disc_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_open_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_close_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_data_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_ctrl_dat_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_start_sdp(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_sdp_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_write_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_get_dscp_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_handsk_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_maint_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_open_cmpl_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_open_failure(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);

/* utility functions */
extern UINT8  bta_hh_find_cb(BD_ADDR bda);
extern void bta_hh_parse_keybd_rpt(tBTA_HH_BOOT_RPT *p_kb_data,
                                   UINT8 *p_report, UINT16 report_len);
extern void bta_hh_parse_mice_rpt(tBTA_HH_BOOT_RPT *p_kb_data,
                                  UINT8 *p_report, UINT16 report_len);
extern BOOLEAN bta_hh_tod_spt(tBTA_HH_DEV_CB *p_cb, UINT8 sub_class);
extern void bta_hh_clean_up_kdev(tBTA_HH_DEV_CB *p_cb);

extern void bta_hh_add_device_to_list(tBTA_HH_DEV_CB *p_cb, UINT8 handle,
                                      UINT16 attr_mask,
                                      tHID_DEV_DSCP_INFO *p_dscp_info,
                                      UINT8 sub_class, UINT16 max_latency, UINT16 min_tout, UINT8 app_id);
extern void bta_hh_update_di_info(tBTA_HH_DEV_CB *p_cb, UINT16 vendor_id, UINT16 product_id,
                                  UINT16 version, UINT8 flag);
extern void bta_hh_cleanup_disable(tBTA_HH_STATUS status);

extern UINT8 bta_hh_dev_handle_to_cb_idx(UINT8 dev_handle);

/* action functions used outside state machine */
extern void bta_hh_api_enable(tBTA_HH_DATA *p_data);
extern void bta_hh_api_disable(void);
extern void bta_hh_disc_cmpl(void);

extern tBTA_HH_STATUS bta_hh_read_ssr_param(BD_ADDR bd_addr, UINT16 *p_max_ssr_lat, UINT16 *p_min_ssr_tout);

/* functions for LE HID */
extern void bta_hh_le_enable(void);
extern BOOLEAN bta_hh_le_is_hh_gatt_if(tBTA_GATTC_IF client_if);
extern void bta_hh_le_deregister(void);
extern BOOLEAN bta_hh_is_le_device(tBTA_HH_DEV_CB *p_cb, BD_ADDR remote_bda);
extern void bta_hh_le_open_conn(tBTA_HH_DEV_CB *p_cb, BD_ADDR remote_bda);
extern void bta_hh_le_api_disc_act(tBTA_HH_DEV_CB *p_cb);
extern void bta_hh_le_get_dscp_act(tBTA_HH_DEV_CB *p_cb);
extern void bta_hh_le_write_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern UINT8 bta_hh_le_add_device(tBTA_HH_DEV_CB *p_cb, tBTA_HH_MAINT_DEV *p_dev_info);
extern void bta_hh_le_remove_dev_bg_conn(tBTA_HH_DEV_CB *p_cb);
extern void bta_hh_le_open_fail(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_gatt_open(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_gatt_close(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_start_security(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_start_srvc_discovery(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_w4_le_read_char_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_le_read_char_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_w4_le_read_descr_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_le_read_descr_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_w4_le_write_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_le_write_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_le_write_char_descr_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_start_security(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_security_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_le_update_scpp(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);
extern void bta_hh_le_notify_enc_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);
extern void bta_hh_ci_load_rpt (tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_buf);

#if BTA_HH_DEBUG
extern void bta_hh_trace_dev_db(void);
#endif

#endif  ///defined(BTA_HH_INCLUDED) && (BTA_HH_INCLUDED == TRUE)
#endif

