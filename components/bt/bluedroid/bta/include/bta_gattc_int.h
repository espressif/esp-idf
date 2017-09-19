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
 *  This is the private file for the file transfer client (FTC).
 *
 ******************************************************************************/
#ifndef BTA_GATTC_INT_H
#define BTA_GATTC_INT_H

#include "bt_target.h"
#include "bta_sys.h"
#include "bta_gatt_api.h"
#include "bta_gattc_ci.h"
#include "bta_gattc_co.h"
#include "fixed_queue.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/
enum {
    BTA_GATTC_API_OPEN_EVT   = BTA_SYS_EVT_START(BTA_ID_GATTC),
    BTA_GATTC_INT_OPEN_FAIL_EVT,
    BTA_GATTC_API_CANCEL_OPEN_EVT,
    BTA_GATTC_INT_CANCEL_OPEN_OK_EVT,

    BTA_GATTC_API_READ_EVT,
    BTA_GATTC_API_WRITE_EVT,
    BTA_GATTC_API_EXEC_EVT,
    BTA_GATTC_API_CFG_MTU_EVT,

    BTA_GATTC_API_CLOSE_EVT,

    BTA_GATTC_API_SEARCH_EVT,
    BTA_GATTC_API_CONFIRM_EVT,
    BTA_GATTC_API_READ_MULTI_EVT,
    BTA_GATTC_API_REFRESH_EVT,

    BTA_GATTC_INT_CONN_EVT,
    BTA_GATTC_INT_DISCOVER_EVT,
    BTA_GATTC_DISCOVER_CMPL_EVT,
    BTA_GATTC_OP_CMPL_EVT,
    BTA_GATTC_INT_DISCONN_EVT,

    BTA_GATTC_INT_START_IF_EVT,
    BTA_GATTC_API_REG_EVT,
    BTA_GATTC_API_DEREG_EVT,
    BTA_GATTC_API_LISTEN_EVT,
    BTA_GATTC_API_BROADCAST_EVT,
    BTA_GATTC_API_DISABLE_EVT,
    BTA_GATTC_ENC_CMPL_EVT
};
typedef UINT16 tBTA_GATTC_INT_EVT;

#define BTA_GATTC_SERVICE_CHANGED_LEN    4

/* max client application GATTC can support */
#ifndef     BTA_GATTC_CL_MAX
#define     BTA_GATTC_CL_MAX    3 // 32
#endif

/* max known devices GATTC can support */
#ifndef     BTA_GATTC_KNOWN_SR_MAX
#if (GATT_MAX_PHY_CHANNEL > 3)
    #define     BTA_GATTC_KNOWN_SR_MAX    GATT_MAX_PHY_CHANNEL
#else
    #define     BTA_GATTC_KNOWN_SR_MAX    3 // The origin value is 10
#endif
#endif

#define BTA_GATTC_CONN_MAX      GATT_MAX_PHY_CHANNEL

#ifndef BTA_GATTC_CLCB_MAX
#define BTA_GATTC_CLCB_MAX      GATT_CL_MAX_LCB
#endif

#define BTA_GATTC_WRITE_PREPARE          GATT_WRITE_PREPARE
#define BTA_GATTC_INVALID_HANDLE         0

/* internal strucutre for GATTC register API  */
typedef struct {
    BT_HDR                  hdr;
    tBT_UUID                app_uuid;
    tBTA_GATTC_CBACK        *p_cback;
} tBTA_GATTC_API_REG;

typedef struct {
    BT_HDR                  hdr;
    tBTA_GATTC_IF           client_if;
} tBTA_GATTC_INT_START_IF;

typedef tBTA_GATTC_INT_START_IF tBTA_GATTC_API_DEREG;
typedef tBTA_GATTC_INT_START_IF tBTA_GATTC_INT_DEREG;

typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 remote_bda;
    tBTA_GATTC_IF           client_if;
    BOOLEAN                 is_direct;
    tBTA_TRANSPORT          transport;
} tBTA_GATTC_API_OPEN;

typedef tBTA_GATTC_API_OPEN tBTA_GATTC_API_CANCEL_OPEN;

typedef struct {
    BT_HDR                  hdr;
    tBTA_GATT_AUTH_REQ      auth_req;
    UINT16                  handle;
    tBTA_GATTC_EVT          cmpl_evt;
} tBTA_GATTC_API_READ;

typedef struct {
    BT_HDR                  hdr;
    tBTA_GATT_AUTH_REQ      auth_req;
    UINT16                  handle;
    tBTA_GATTC_EVT          cmpl_evt;
    tBTA_GATTC_WRITE_TYPE   write_type;
    UINT16                  offset;
    UINT16                  len;
    UINT8                   *p_value;
} tBTA_GATTC_API_WRITE;

typedef struct {
    BT_HDR                  hdr;
    BOOLEAN                 is_execute;
} tBTA_GATTC_API_EXEC;

typedef struct {
    BT_HDR                  hdr;
    UINT16                  handle;
} tBTA_GATTC_API_CONFIRM;

typedef tGATT_CL_COMPLETE tBTA_GATTC_CMPL;

typedef struct {
    BT_HDR                  hdr;
    UINT8                   op_code;
    tGATT_STATUS            status;
    tBTA_GATTC_CMPL         *p_cmpl;
} tBTA_GATTC_OP_CMPL;

typedef struct {
    BT_HDR              hdr;
    tBT_UUID            *p_srvc_uuid;
} tBTA_GATTC_API_SEARCH;

typedef struct {
    BT_HDR                  hdr;
    tBTA_GATT_AUTH_REQ      auth_req;
    UINT8                   num_attr;
    UINT16                  handles[GATT_MAX_READ_MULTI_HANDLES];
    tBTA_GATTC_EVT          cmpl_evt;
}tBTA_GATTC_API_READ_MULTI;

typedef struct {
    BT_HDR                  hdr;
    BD_ADDR_PTR             remote_bda;
    tBTA_GATTC_IF           client_if;
    BOOLEAN                 start;
} tBTA_GATTC_API_LISTEN;


typedef struct {
    BT_HDR              hdr;
} tBTA_GATTC_API_CFG_MTU;

typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 remote_bda;
    tBTA_GATTC_IF           client_if;
    UINT8                   role;
    tBT_TRANSPORT           transport;
    tGATT_DISCONN_REASON    reason;
} tBTA_GATTC_INT_CONN;

typedef struct {
    BT_HDR                  hdr;
    BD_ADDR                 remote_bda;
    tBTA_GATTC_IF           client_if;
} tBTA_GATTC_ENC_CMPL;

typedef union {
    BT_HDR                      hdr;
    tBTA_GATTC_API_REG          api_reg;
    tBTA_GATTC_API_DEREG        api_dereg;
    tBTA_GATTC_API_OPEN         api_conn;
    tBTA_GATTC_API_CANCEL_OPEN  api_cancel_conn;
    tBTA_GATTC_API_READ         api_read;
    tBTA_GATTC_API_SEARCH       api_search;
    tBTA_GATTC_API_WRITE        api_write;
    tBTA_GATTC_API_CONFIRM      api_confirm;
    tBTA_GATTC_API_EXEC         api_exec;
    tBTA_GATTC_API_READ_MULTI   api_read_multi;
    tBTA_GATTC_API_CFG_MTU      api_mtu;
    tBTA_GATTC_OP_CMPL          op_cmpl;
    tBTA_GATTC_INT_CONN         int_conn;
    tBTA_GATTC_ENC_CMPL         enc_cmpl;

    tBTA_GATTC_INT_START_IF     int_start_if;
    tBTA_GATTC_INT_DEREG        int_dereg;
    /* if peripheral role is supported */
    tBTA_GATTC_API_LISTEN       api_listen;

} tBTA_GATTC_DATA;


/* GATT server cache on the client */
typedef struct {
    tBT_UUID            uuid;
    UINT16              s_handle;
    UINT16              e_handle;
    // this field is set only for characteristic
    UINT16              char_decl_handle;
    BOOLEAN             is_primary;
    tBTA_GATT_CHAR_PROP property;
} tBTA_GATTC_ATTR_REC;


#define BTA_GATTC_MAX_CACHE_CHAR    40
#define BTA_GATTC_ATTR_LIST_SIZE    (BTA_GATTC_MAX_CACHE_CHAR * sizeof(tBTA_GATTC_ATTR_REC))

#ifndef BTA_GATTC_CACHE_SRVR_SIZE
#define BTA_GATTC_CACHE_SRVR_SIZE   600
#endif

enum {
    BTA_GATTC_IDLE_ST = 0,      /* Idle  */
    BTA_GATTC_W4_CONN_ST,       /* Wait for connection -  (optional) */
    BTA_GATTC_CONN_ST,          /* connected state */
    BTA_GATTC_DISCOVER_ST       /* discover is in progress */
};
typedef UINT8 tBTA_GATTC_STATE;

typedef struct {
    BOOLEAN             in_use;
    BD_ADDR             server_bda;
    BOOLEAN             connected;

#define BTA_GATTC_SERV_IDLE     0
#define BTA_GATTC_SERV_LOAD     1
#define BTA_GATTC_SERV_SAVE     2
#define BTA_GATTC_SERV_DISC     3
#define BTA_GATTC_SERV_DISC_ACT 4

    UINT8               state;

    list_t              *p_srvc_cache;  /* list of tBTA_GATTC_SERVICE */
    UINT8               update_count;   /* indication received */
    UINT8               num_clcb;       /* number of associated CLCB */


    tBTA_GATTC_ATTR_REC *p_srvc_list;
    UINT8               cur_srvc_idx;
    UINT8               cur_char_idx;
    UINT8               next_avail_idx;
    UINT8               total_srvc;
    UINT8               total_char;
    UINT16              total_attr;
    UINT8               srvc_hdl_chg;   /* service handle change indication pending */
    UINT16              attr_index;     /* cahce NV saving/loading attribute index */

    UINT16              mtu;
} tBTA_GATTC_SERV;

#ifndef BTA_GATTC_NOTIF_REG_MAX
#define BTA_GATTC_NOTIF_REG_MAX     7//15
#endif

typedef struct {
    BOOLEAN             in_use;
    BD_ADDR             remote_bda;
    UINT16              handle;
}tBTA_GATTC_NOTIF_REG;

typedef struct {
    tBTA_GATTC_CBACK        *p_cback;
    BOOLEAN                 in_use;
    tBTA_GATTC_IF           client_if;      /* client interface with BTE stack for this application */
    UINT8                   num_clcb;       /* number of associated CLCB */
    BOOLEAN                 dereg_pending;
    tBT_UUID                app_uuid;
    tBTA_GATTC_NOTIF_REG    notif_reg[BTA_GATTC_NOTIF_REG_MAX];
} tBTA_GATTC_RCB;

/* client channel is a mapping between a BTA client(cl_id) and a remote BD address */
typedef struct {
    UINT16              bta_conn_id;    /* client channel ID, unique for clcb */
    BD_ADDR             bda;
    tBTA_TRANSPORT      transport;      /* channel transport */
    tBTA_GATTC_RCB      *p_rcb;         /* pointer to the registration CB */
    tBTA_GATTC_SERV     *p_srcb;    /* server cache CB */
    tBTA_GATTC_DATA     *p_q_cmd;   /* command in queue waiting for execution */

#define BTA_GATTC_NO_SCHEDULE       0
#define BTA_GATTC_DISC_WAITING      0x01
#define BTA_GATTC_REQ_WAITING       0x10

    UINT8               auto_update; /* auto update is waiting */
    BOOLEAN             disc_active;
    BOOLEAN             in_use;
    tBTA_GATTC_STATE    state;
    tBTA_GATT_STATUS    status;
    UINT16              reason;
} tBTA_GATTC_CLCB;

/* back ground connection tracking information */
#if GATT_MAX_APPS <= 8
typedef UINT8 tBTA_GATTC_CIF_MASK ;
#elif GATT_MAX_APPS <= 16
typedef UINT16 tBTA_GATTC_CIF_MASK;
#elif GATT_MAX_APPS <= 32
typedef UINT32 tBTA_GATTC_CIF_MASK;
#endif

typedef struct {
    BOOLEAN                 in_use;
    BD_ADDR                 remote_bda;
    tBTA_GATTC_CIF_MASK     cif_mask;
    tBTA_GATTC_CIF_MASK     cif_adv_mask;

} tBTA_GATTC_BG_TCK;

typedef struct {
    BOOLEAN             in_use;
    BD_ADDR             remote_bda;
    TIMER_LIST_ENT      service_change_ccc_timer;           /* wait for discovering remote device's service change ccc handle */
    BOOLEAN             ccc_timer_used;                     /* service_change_ccc_timer started */
    BOOLEAN             service_change_ccc_written;         /* has written remote device's service change ccc */
} tBTA_GATTC_CONN;

enum {
    BTA_GATTC_STATE_DISABLED,
    BTA_GATTC_STATE_ENABLING,
    BTA_GATTC_STATE_ENABLED,
    BTA_GATTC_STATE_DISABLING
};

typedef struct {
    UINT8             state;

    tBTA_GATTC_CONN     conn_track[BTA_GATTC_CONN_MAX];
    tBTA_GATTC_BG_TCK   bg_track[BTA_GATTC_KNOWN_SR_MAX];
    tBTA_GATTC_RCB      cl_rcb[BTA_GATTC_CL_MAX];

    tBTA_GATTC_CLCB     clcb[BTA_GATTC_CLCB_MAX];
    tBTA_GATTC_SERV     known_server[BTA_GATTC_KNOWN_SR_MAX];
}tBTA_GATTC_CB;

typedef enum {
    SERVICE_CHANGE_CCC_WRITTEN_SUCCESS = 0,
    SERVICE_CHANGE_CACHE_NOT_FOUND,
    SERVICE_CHANGE_SERVICE_NOT_FOUND,
    SERVICE_CHANGE_CHAR_NOT_FOUND,
    SERVICE_CHANGE_CCC_NOT_FOUND,
    SERVICE_CHANGE_WRITE_CCC_FAILED
}tBTA_GATTC_FIND_SERVICE_CB;


/*****************************************************************************
**  Global data
*****************************************************************************/

/* GATTC control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_GATTC_CB  bta_gattc_cb;
#else
extern tBTA_GATTC_CB *bta_gattc_cb_ptr;
#define bta_gattc_cb (*bta_gattc_cb_ptr)
#endif

/*****************************************************************************
**  Function prototypes
*****************************************************************************/
extern BOOLEAN bta_gattc_hdl_event(BT_HDR *p_msg);
extern BOOLEAN bta_gattc_sm_execute(tBTA_GATTC_CLCB *p_clcb, UINT16 event, tBTA_GATTC_DATA *p_data);

/* function processed outside SM */
extern void bta_gattc_disable(tBTA_GATTC_CB *p_cb);
extern void bta_gattc_register(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_start_if(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_process_api_open (tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg);
extern void bta_gattc_process_api_open_cancel (tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg);
extern void bta_gattc_deregister(tBTA_GATTC_CB *p_cb, tBTA_GATTC_RCB  *p_clreg);
extern void bta_gattc_process_enc_cmpl(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg);

/* function within state machine */
extern void bta_gattc_open(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_open_fail(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_open_error(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);

extern void bta_gattc_cancel_open(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_cancel_open_ok(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_cancel_open_error(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);

extern void bta_gattc_conn(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_conncback(tBTA_GATTC_RCB *p_rcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_disconncback(tBTA_GATTC_RCB *p_rcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_close(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_close_fail(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_disc_close(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);

extern void bta_gattc_start_discover(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_disc_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_read(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_write(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_op_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_q_cmd(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_search(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_fail(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_confirm(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_execute(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_read_multi(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_ci_open(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_ci_close(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_ignore_op_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
extern void bta_gattc_restart_discover(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_msg);
extern void bta_gattc_init_bk_conn(tBTA_GATTC_API_OPEN *p_data, tBTA_GATTC_RCB *p_clreg);
extern void bta_gattc_cancel_bk_conn(tBTA_GATTC_API_CANCEL_OPEN *p_data);
extern void bta_gattc_send_open_cback( tBTA_GATTC_RCB *p_clreg, tBTA_GATT_STATUS status,
                                       BD_ADDR remote_bda, UINT16 conn_id, tBTA_TRANSPORT transport,  UINT16 mtu);
extern void bta_gattc_send_connect_cback( tBTA_GATTC_RCB *p_clreg, tBTA_GATT_STATUS status,
                                BD_ADDR remote_bda, UINT16 conn_id);
extern void bta_gattc_send_disconnect_cback( tBTA_GATTC_RCB *p_clreg, tBTA_GATT_STATUS status,
                                BD_ADDR remote_bda, UINT16 conn_id);
extern void bta_gattc_process_api_refresh(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg);
extern void bta_gattc_cfg_mtu(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);
#if BLE_INCLUDED == TRUE
extern void bta_gattc_listen(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg);
extern void bta_gattc_broadcast(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg);
#endif
/* utility functions */
extern tBTA_GATTC_CLCB *bta_gattc_find_clcb_by_cif (UINT8 client_if, BD_ADDR remote_bda, tBTA_TRANSPORT transport);
extern tBTA_GATTC_CLCB *bta_gattc_find_clcb_by_conn_id (UINT16 conn_id);
extern tBTA_GATTC_CLCB *bta_gattc_clcb_alloc(tBTA_GATTC_IF client_if, BD_ADDR remote_bda, tBTA_TRANSPORT transport);
extern void bta_gattc_clcb_dealloc(tBTA_GATTC_CLCB *p_clcb);
extern tBTA_GATTC_CLCB *bta_gattc_find_alloc_clcb(tBTA_GATTC_IF client_if, BD_ADDR remote_bda, tBTA_TRANSPORT transport);
extern tBTA_GATTC_RCB *bta_gattc_cl_get_regcb(UINT8 client_if);
extern tBTA_GATTC_SERV *bta_gattc_find_srcb(BD_ADDR bda);
extern tBTA_GATTC_SERV *bta_gattc_srcb_alloc(BD_ADDR bda);
extern tBTA_GATTC_SERV *bta_gattc_find_scb_by_cid (UINT16 conn_id);
extern tBTA_GATTC_CLCB *bta_gattc_find_int_conn_clcb(tBTA_GATTC_DATA *p_msg);
extern tBTA_GATTC_CLCB *bta_gattc_find_int_disconn_clcb(tBTA_GATTC_DATA *p_msg);

extern BOOLEAN bta_gattc_enqueue(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data);

extern BOOLEAN bta_gattc_uuid_compare (const tBT_UUID *p_src, const tBT_UUID *p_tar, BOOLEAN is_precise);
extern BOOLEAN bta_gattc_check_notif_registry(tBTA_GATTC_RCB  *p_clreg, tBTA_GATTC_SERV *p_srcb, tBTA_GATTC_NOTIFY  *p_notify);
extern BOOLEAN bta_gattc_mark_bg_conn (tBTA_GATTC_IF client_if,  BD_ADDR_PTR remote_bda, BOOLEAN add, BOOLEAN is_listen);
extern BOOLEAN bta_gattc_check_bg_conn (tBTA_GATTC_IF client_if,  BD_ADDR remote_bda, UINT8 role);
extern UINT8 bta_gattc_num_reg_app(void);
extern void bta_gattc_clear_notif_registration(tBTA_GATTC_SERV *p_srcb, UINT16 conn_id, UINT16 start_handle, UINT16 end_handle);
extern tBTA_GATTC_SERV * bta_gattc_find_srvr_cache(BD_ADDR bda);

/* discovery functions */
extern void bta_gattc_disc_res_cback (UINT16 conn_id, tGATT_DISC_TYPE disc_type, tGATT_DISC_RES *p_data);
extern void bta_gattc_disc_cmpl_cback (UINT16 conn_id, tGATT_DISC_TYPE disc_type, tGATT_STATUS status);
extern tBTA_GATT_STATUS bta_gattc_discover_procedure(UINT16 conn_id, tBTA_GATTC_SERV *p_server_cb, UINT8 disc_type);
extern tBTA_GATT_STATUS bta_gattc_discover_pri_service(UINT16 conn_id, tBTA_GATTC_SERV *p_server_cb, UINT8 disc_type);
extern void bta_gattc_search_service(tBTA_GATTC_CLCB *p_clcb, tBT_UUID *p_uuid);
extern const list_t* bta_gattc_get_services(UINT16 conn_id);
extern const tBTA_GATTC_SERVICE* bta_gattc_get_service_for_handle(UINT16 conn_id, UINT16 handle);
tBTA_GATTC_CHARACTERISTIC*  bta_gattc_get_characteristic_srcb(tBTA_GATTC_SERV *p_srcb, UINT16 handle);
extern tBTA_GATTC_CHARACTERISTIC* bta_gattc_get_characteristic(UINT16 conn_id, UINT16 handle);
extern tBTA_GATTC_DESCRIPTOR* bta_gattc_get_descriptor(UINT16 conn_id, UINT16 handle);
extern void bta_gattc_get_db_size_handle(UINT16 conn_id, UINT16 start_handle, UINT16 end_handle, int *count);
extern void bta_gattc_get_db_size_with_type_handle(UINT16 conn_id, bt_gatt_db_attribute_type_t type,
                                                   UINT16 start_handle, UINT16 end_handle, UINT16 char_handle, int *count);
extern void bta_gattc_get_service_with_uuid(UINT16 conn_id, tBT_UUID *svc_uuid,
                                            btgatt_db_element_t **svc_db,
                                            int *count);

extern void bta_gattc_get_db_with_opration(UINT16 conn_id,
                                                      bt_gatt_get_db_op_t op,
                                                      UINT16 char_handle,
                                                      tBT_UUID *incl_uuid,
                                                      tBT_UUID *char_uuid,
                                                      tBT_UUID *descr_uuid,
                                                      UINT16 start_handle, UINT16 end_handle,
                                                      btgatt_db_element_t **char_db,
                                                      int *count);

extern void bta_gattc_get_gatt_db(UINT16 conn_id, UINT16 start_handle, UINT16 end_handle, btgatt_db_element_t **db, int *count);

extern tBTA_GATT_STATUS bta_gattc_init_cache(tBTA_GATTC_SERV *p_srvc_cb);
extern void bta_gattc_rebuild_cache(tBTA_GATTC_SERV *p_srcv, UINT16 num_attr, tBTA_GATTC_NV_ATTR *attr);
extern void bta_gattc_cache_save(tBTA_GATTC_SERV *p_srvc_cb, UINT16 conn_id);
extern void bta_gattc_reset_discover_st(tBTA_GATTC_SERV *p_srcb, tBTA_GATT_STATUS status);

extern tBTA_GATTC_CONN *bta_gattc_conn_alloc(BD_ADDR remote_bda);
extern tBTA_GATTC_CONN *bta_gattc_conn_find(BD_ADDR remote_bda);
extern tBTA_GATTC_CONN *bta_gattc_conn_find_alloc(BD_ADDR remote_bda);
extern BOOLEAN bta_gattc_conn_dealloc(BD_ADDR remote_bda);

extern bool bta_gattc_cache_load(tBTA_GATTC_CLCB *p_clcb);
extern void bta_gattc_cache_reset(BD_ADDR server_bda);

#endif /* BTA_GATTC_INT_H */
