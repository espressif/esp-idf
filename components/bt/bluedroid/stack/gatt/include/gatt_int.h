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

#ifndef  GATT_INT_H
#define  GATT_INT_H

#include "bt_target.h"
#include "bt_trace.h"
#include "gatt_api.h"
#include "btm_ble_api.h"
#include "btu.h"
#include "fixed_queue.h"

#include <string.h>


#define GATT_CREATE_CONN_ID(tcb_idx, gatt_if)  ((UINT16) ((((UINT8)(tcb_idx) ) << 8) | ((UINT8) (gatt_if))))
#define GATT_GET_TCB_IDX(conn_id)  ((UINT8) (((UINT16) (conn_id)) >> 8))
#define GATT_GET_GATT_IF(conn_id)  ((tGATT_IF)((UINT8) (conn_id)))

#define GATT_GET_SR_REG_PTR(index) (&gatt_cb.sr_reg[(UINT8) (index)]);
#define GATT_TRANS_ID_MAX          0x0fffffff      /* 4 MSB is reserved */
#define GATT_RSP_BY_APP            0x00
#define GATT_RSP_BY_STACK          0x01
#define GATT_RSP_DEFAULT           GATT_RSP_BY_APP          //need to rsp by the app.

/* security action for GATT write and read request */
#define GATT_SEC_NONE              0
#define GATT_SEC_OK                1
#define GATT_SEC_SIGN_DATA         2   /* compute the signature for the write cmd */
#define GATT_SEC_ENCRYPT           3    /* encrypt the link with current key */
#define GATT_SEC_ENCRYPT_NO_MITM   4    /* unauthenticated encryption or better */
#define GATT_SEC_ENCRYPT_MITM      5    /* authenticated encryption */
#define GATT_SEC_ENC_PENDING       6   /* wait for link encryption pending */
typedef UINT8 tGATT_SEC_ACTION;


#define GATT_ATTR_OP_SPT_MTU               (0x00000001 << 0)
#define GATT_ATTR_OP_SPT_FIND_INFO         (0x00000001 << 1)
#define GATT_ATTR_OP_SPT_FIND_BY_TYPE      (0x00000001 << 2)
#define GATT_ATTR_OP_SPT_READ_BY_TYPE      (0x00000001 << 3)
#define GATT_ATTR_OP_SPT_READ              (0x00000001 << 4)
#define GATT_ATTR_OP_SPT_MULT_READ         (0x00000001 << 5)
#define GATT_ATTR_OP_SPT_READ_BLOB         (0x00000001 << 6)
#define GATT_ATTR_OP_SPT_READ_BY_GRP_TYPE  (0x00000001 << 7)
#define GATT_ATTR_OP_SPT_WRITE             (0x00000001 << 8)
#define GATT_ATTR_OP_SPT_WRITE_CMD         (0x00000001 << 9)
#define GATT_ATTR_OP_SPT_PREP_WRITE        (0x00000001 << 10)
#define GATT_ATTR_OP_SPT_EXE_WRITE         (0x00000001 << 11)
#define GATT_ATTR_OP_SPT_HDL_VALUE_CONF    (0x00000001 << 12)
#define GATT_ATTR_OP_SP_SIGN_WRITE         (0x00000001 << 13)

#define GATT_INDEX_INVALID      0xff

#define GATT_PENDING_REQ_NONE   0


#define GATT_WRITE_CMD_MASK     0xc0  /*0x1100-0000*/
#define GATT_AUTH_SIGN_MASK     0x80  /*0x1000-0000*/
#define GATT_AUTH_SIGN_LEN      12

#define GATT_HDR_SIZE           3 /* 1B opcode + 2B handle */

/* wait for ATT cmd response timeout value */
#define GATT_WAIT_FOR_RSP_TOUT       30
#define GATT_WAIT_FOR_DISC_RSP_TOUT  5
#define GATT_REQ_RETRY_LIMIT         2
#define GATT_WAIT_FOR_IND_ACK_TOUT   5

/* characteristic descriptor type */
#define GATT_DESCR_EXT_DSCPTOR   1    /* Characteristic Extended Properties */
#define GATT_DESCR_USER_DSCPTOR  2    /* Characteristic User Description    */
#define GATT_DESCR_CLT_CONFIG    3    /* Client Characteristic Configuration */
#define GATT_DESCR_SVR_CONFIG    4    /* Server Characteristic Configuration */
#define GATT_DESCR_PRES_FORMAT   5    /* Characteristic Presentation Format */
#define GATT_DESCR_AGGR_FORMAT   6    /* Characteristic Aggregate Format */
#define GATT_DESCR_VALID_RANGE   7    /* Characteristic Valid Range */
#define GATT_DESCR_UNKNOWN       0xff

#define GATT_SEC_FLAG_LKEY_UNAUTHED     BTM_SEC_FLAG_LKEY_KNOWN
#define GATT_SEC_FLAG_LKEY_AUTHED       BTM_SEC_FLAG_LKEY_AUTHED
#define GATT_SEC_FLAG_ENCRYPTED         BTM_SEC_FLAG_ENCRYPTED
typedef UINT8 tGATT_SEC_FLAG;

/* Find Information Response Type
*/
#define GATT_INFO_TYPE_PAIR_16      0x01
#define GATT_INFO_TYPE_PAIR_128     0x02

/*  GATT client FIND_TYPE_VALUE_Request data */
typedef struct {
    tBT_UUID        uuid;           /* type of attribute to be found */
    UINT16          s_handle;       /* starting handle */
    UINT16          e_handle;       /* ending handle */
    UINT16          value_len;      /* length of the attribute value */
    UINT8           value[GATT_MAX_MTU_SIZE];       /* pointer to the attribute value to be found */
} tGATT_FIND_TYPE_VALUE;

/* client request message to ATT protocol
*/
typedef union {
    tGATT_READ_BY_TYPE      browse;     /* read by type request */
    tGATT_FIND_TYPE_VALUE   find_type_value;/* find by type value */
    tGATT_READ_MULTI        read_multi;   /* read multiple request */
    tGATT_READ_PARTIAL      read_blob;    /* read blob */
    tGATT_VALUE             attr_value;   /* write request */
    /* prepare write */
    /* write blob */
    UINT16                  handle;        /* read,  handle value confirmation */
    UINT16                  mtu;
    tGATT_EXEC_FLAG         exec_write;    /* execute write */
} tGATT_CL_MSG;

/* error response strucutre */
typedef struct {
    UINT16  handle;
    UINT8   cmd_code;
    UINT8   reason;
} tGATT_ERROR;

/* Execute write response structure */
typedef struct {
    UINT8   op_code;
}__attribute__((packed)) tGATT_EXEC_WRITE_RSP;

/* Write request response structure */
typedef struct {
    UINT8   op_code;
}__attribute__((packed)) tGATT_WRITE_REQ_RSP;

/* server response message to ATT protocol
*/
typedef union {
    /* data type            member          event   */
    tGATT_VALUE             attr_value;     /* READ, HANDLE_VALUE_IND, PREPARE_WRITE */
    /* READ_BLOB, READ_BY_TYPE */
    tGATT_ERROR             error;          /* ERROR_RSP */
    UINT16                  handle;         /* WRITE, WRITE_BLOB */
    UINT16                  mtu;            /* exchange MTU request */
} tGATT_SR_MSG;

/* Characteristic declaration attribute value
*/
typedef struct {
    tGATT_CHAR_PROP             property;
    UINT16                      char_val_handle;
} tGATT_CHAR_DECL;

/* attribute value maintained in the server database
*/
typedef union {
    tBT_UUID                uuid;               /* service declaration */
    tGATT_CHAR_DECL         char_decl;          /* characteristic declaration */
    tGATT_INCL_SRVC         incl_handle;        /* included service */
    tGATT_ATTR_VAL          attr_val;
} tGATT_ATTR_VALUE;

/* Attribute UUID type
*/
#define GATT_ATTR_UUID_TYPE_16      0
#define GATT_ATTR_UUID_TYPE_128     1
#define GATT_ATTR_UUID_TYPE_32      2
typedef UINT8   tGATT_ATTR_UUID_TYPE;

/* 16 bits UUID Attribute in server database
*/
typedef struct {
    void                    *p_next;  /* pointer to the next attribute, either tGATT_ATTR16 or tGATT_ATTR128 */
    tGATT_ATTR_VALUE        *p_value;
    tGATT_ATTR_UUID_TYPE    uuid_type;
    tGATT_PERM              permission;
    tGATTS_ATTR_CONTROL     control;
    tGATT_ATTR_MASK         mask;
    UINT16                  handle;
    UINT16                  uuid;
} tGATT_ATTR16;

/* 32 bits UUID Attribute in server database
*/
typedef struct {
    void                    *p_next;  /* pointer to the next attribute, either tGATT_ATTR16, tGATT_ATTR32 or tGATT_ATTR128 */
    tGATT_ATTR_VALUE        *p_value;
    tGATT_ATTR_UUID_TYPE    uuid_type;
    tGATT_PERM              permission;
    tGATTS_ATTR_CONTROL     control;
    tGATT_ATTR_MASK         mask;
    UINT16                  handle;
    UINT32                  uuid;
} tGATT_ATTR32;


/* 128 bits UUID Attribute in server database
*/
typedef struct {
    void                    *p_next;  /* pointer to the next attribute, either tGATT_ATTR16 or tGATT_ATTR128 */
    tGATT_ATTR_VALUE        *p_value;
    tGATT_ATTR_UUID_TYPE    uuid_type;
    tGATT_PERM              permission;
    tGATTS_ATTR_CONTROL     control;
    tGATT_ATTR_MASK         mask;
    UINT16                  handle;
    UINT8                   uuid[LEN_UUID_128];
} tGATT_ATTR128;

/* Service Database definition
*/
typedef struct {
    void            *p_attr_list;       /* pointer to the first attribute, either tGATT_ATTR16 or tGATT_ATTR128 */
    UINT8           *p_free_mem;        /* Pointer to free memory       */
    fixed_queue_t   *svc_buffer;         /* buffer queue used for service database */
    UINT32          mem_free;           /* Memory still available       */
    UINT16          end_handle;         /* Last handle number           */
    UINT16          next_handle;        /* Next usable handle value     */
} tGATT_SVC_DB;

/* Data Structure used for GATT server                                        */
/* A GATT registration record consists of a handle, and 1 or more attributes  */
/* A service registration information record consists of beginning and ending */
/* attribute handle, service UUID and a set of GATT server callback.          */
typedef struct {
    tGATT_SVC_DB    *p_db;              /* pointer to the service database */
    tBT_UUID        app_uuid;           /* applicatino UUID */
    UINT32          sdp_handle;         /* primamry service SDP handle */
    UINT16          service_instance;   /* service instance number */
    UINT16          type;               /* service type UUID, primary or secondary */
    UINT16          s_hdl;              /* service starting handle */
    UINT16          e_hdl;              /* service ending handle */
    tGATT_IF        gatt_if;            /* this service is belong to which application */
    BOOLEAN         in_use;
} tGATT_SR_REG;

#define GATT_LISTEN_TO_ALL  0xff
#define GATT_LISTEN_TO_NONE 0

/* Data Structure used for GATT server */
/* An GATT registration record consists of a handle, and 1 or more attributes */
/* A service registration information record consists of beginning and ending */
/* attribute handle, service UUID and a set of GATT server callback.          */

typedef struct {
    tBT_UUID     app_uuid128;
    tGATT_CBACK  app_cb;
    tGATT_IF     gatt_if; /* one based */
    BOOLEAN      in_use;
    UINT8        listening; /* if adv for all has been enabled */
} tGATT_REG;




/* command queue for each connection */
typedef struct {
    BT_HDR      *p_cmd;
    UINT16      clcb_idx;
    UINT8       op_code;
    BOOLEAN     to_send;
} tGATT_CMD_Q;


#if GATT_MAX_SR_PROFILES <= 8
typedef UINT8 tGATT_APP_MASK;
#elif GATT_MAX_SR_PROFILES <= 16
typedef UINT16 tGATT_APP_MASK;
#elif GATT_MAX_SR_PROFILES <= 32
typedef UINT32 tGATT_APP_MASK;
#endif

/* command details for each connection */
typedef struct {
    BT_HDR          *p_rsp_msg;
    UINT32           trans_id;
    tGATT_READ_MULTI multi_req;
    fixed_queue_t    *multi_rsp_q;
    UINT16           handle;
    UINT8            op_code;
    UINT8            status;
    UINT8            cback_cnt[GATT_MAX_APPS];
} tGATT_SR_CMD;

#define     GATT_CH_CLOSE               0
#define     GATT_CH_CLOSING             1
#define     GATT_CH_CONN                2
#define     GATT_CH_CFG                 3
#define     GATT_CH_OPEN                4

typedef UINT8 tGATT_CH_STATE;

#define GATT_GATT_START_HANDLE  1
#define GATT_GAP_START_HANDLE   20
#define GATT_APP_START_HANDLE   40

typedef struct hdl_cfg {
    UINT16               gatt_start_hdl;
    UINT16               gap_start_hdl;
    UINT16               app_start_hdl;
} tGATT_HDL_CFG;

typedef struct hdl_list_elem {
    struct              hdl_list_elem *p_next;
    struct              hdl_list_elem *p_prev;
    tGATTS_HNDL_RANGE   asgn_range; /* assigned handle range */
    tGATT_SVC_DB        svc_db;
    BOOLEAN             in_use;
} tGATT_HDL_LIST_ELEM;

typedef struct {
    tGATT_HDL_LIST_ELEM  *p_first;
    tGATT_HDL_LIST_ELEM  *p_last;
    UINT16               count;
} tGATT_HDL_LIST_INFO;


typedef struct srv_list_elem {
    struct              srv_list_elem *p_next;
    struct              srv_list_elem *p_prev;
    UINT16              s_hdl;
    UINT8               i_sreg;
    BOOLEAN             in_use;
    BOOLEAN             is_primary;
} tGATT_SRV_LIST_ELEM;


typedef struct {
    tGATT_SRV_LIST_ELEM  *p_last_primary;
    tGATT_SRV_LIST_ELEM  *p_first;
    tGATT_SRV_LIST_ELEM  *p_last;
    UINT16               count;
} tGATT_SRV_LIST_INFO;

/* prepare write queue data */
typedef struct{
    //len: length of value
    tGATT_ATTR16  *p_attr;
    UINT16 len;
    UINT8 op_code;
    UINT16 handle;
    UINT16 offset;
    UINT8 value[2];
}__attribute__((packed)) tGATT_PREPARE_WRITE_QUEUE_DATA;

/* structure to store prepare write packts information */
typedef struct{
    //only store prepare write packets which need
    //to be responded by stack (not by application)
    fixed_queue_t *queue;

    //store the total number of prepare write packets
    //including that should be responded by stack or by application
    UINT16 total_num;

    //store application error code for prepare write,
    //invalid offset && invalid length
    UINT8 error_code_app;
}tGATT_PREPARE_WRITE_RECORD;

typedef struct {
    fixed_queue_t    *pending_enc_clcb;   /* pending encryption channel q */
    tGATT_SEC_ACTION sec_act;
    BD_ADDR         peer_bda;
    tBT_TRANSPORT   transport;
    UINT32          trans_id;

    UINT16          att_lcid;           /* L2CAP channel ID for ATT */
    UINT16          payload_size;

    tGATT_CH_STATE  ch_state;
    UINT8           ch_flags;

    tGATT_IF        app_hold_link[GATT_MAX_APPS];

    /* server needs */
    /* server response data */
#if (GATTS_INCLUDED == TRUE)
    tGATT_SR_CMD    sr_cmd;
#endif  ///GATTS_INCLUDED == TRUE
    UINT16          indicate_handle;
    fixed_queue_t   *pending_ind_q;

    TIMER_LIST_ENT  conf_timer_ent;     /* peer confirm to indication timer */

    UINT8           prep_cnt[GATT_MAX_APPS];
    UINT8           ind_count;

    tGATT_CMD_Q     cl_cmd_q[GATT_CL_MAX_LCB];
    TIMER_LIST_ENT  ind_ack_timer_ent;    /* local app confirm to indication timer */
    UINT8           pending_cl_req;
    UINT8           next_slot_inq;    /* index of next available slot in queue */

    BOOLEAN         in_use;
    UINT8           tcb_idx;
    tGATT_PREPARE_WRITE_RECORD prepare_write_record;    /* prepare write packets record */
} tGATT_TCB;


/* logic channel */
typedef struct {
    UINT16                  next_disc_start_hdl;   /* starting handle for the next inc srvv discovery */
    tGATT_DISC_RES          result;
    BOOLEAN                 wait_for_read_rsp;
} tGATT_READ_INC_UUID128;
typedef struct {
    tGATT_TCB               *p_tcb;         /* associated TCB of this CLCB */
    tGATT_REG               *p_reg;        /* owner of this CLCB */
    UINT8                   sccb_idx;
    UINT8                   *p_attr_buf;    /* attribute buffer for read multiple, prepare write */
    tBT_UUID                uuid;
    UINT16                  conn_id;        /* connection handle */
    UINT16                  clcb_idx;
    UINT16                  s_handle;       /* starting handle of the active request */
    UINT16                  e_handle;       /* ending handle of the active request */
    UINT16                  counter;        /* used as offset, attribute length, num of prepare write */
    UINT16                  start_offset;
    tGATT_AUTH_REQ          auth_req;       /* authentication requirement */
    UINT8                   operation;      /* one logic channel can have one operation active */
    UINT8                   op_subtype;     /* operation subtype */
    UINT8                   status;         /* operation status */
    BOOLEAN                 first_read_blob_after_read;
    tGATT_READ_INC_UUID128  read_uuid128;
    BOOLEAN                 in_use;
    TIMER_LIST_ENT          rsp_timer_ent;  /* peer response timer */
    UINT8                   retry_count;

} tGATT_CLCB;

typedef struct {
    tGATT_CLCB  *p_clcb;
} tGATT_PENDING_ENC_CLCB;


#define GATT_SIGN_WRITE             1
#define GATT_VERIFY_SIGN_DATA       2

typedef struct {
    BT_HDR      hdr;
    tGATT_CLCB  *p_clcb;
} tGATT_SIGN_WRITE_OP;

typedef struct {
    BT_HDR      hdr;
    tGATT_TCB   *p_tcb;
    BT_HDR      *p_data;

} tGATT_VERIFY_SIGN_OP;


typedef struct {
    UINT16                  clcb_idx;
    BOOLEAN                 in_use;
} tGATT_SCCB;

typedef struct {
    UINT16      handle;
    UINT16      uuid;
    UINT32      service_change;
} tGATT_SVC_CHG;

typedef struct {
    tGATT_IF        gatt_if[GATT_MAX_APPS];
    tGATT_IF        listen_gif[GATT_MAX_APPS];
    BD_ADDR         remote_bda;
    BOOLEAN         in_use;
} tGATT_BG_CONN_DEV;

#define GATT_SVC_CHANGED_CONNECTING        1   /* wait for connection */
#define GATT_SVC_CHANGED_SERVICE           2   /* GATT service discovery */
#define GATT_SVC_CHANGED_CHARACTERISTIC    3   /* service change char discovery */
#define GATT_SVC_CHANGED_DESCRIPTOR        4   /* service change CCC discoery */
#define GATT_SVC_CHANGED_CONFIGURE_CCCD    5   /* config CCC */

typedef struct {
    UINT16  conn_id;
    BOOLEAN in_use;
    BOOLEAN connected;
    BD_ADDR bda;
    tBT_TRANSPORT   transport;

    /* GATT service change CCC related variables */
    UINT8       ccc_stage;
    UINT8       ccc_result;
    UINT16      s_handle;
    UINT16      e_handle;
} tGATT_PROFILE_CLCB;

typedef struct {
    tGATT_TCB           tcb[GATT_MAX_PHY_CHANNEL];
    fixed_queue_t       *sign_op_queue;

    tGATT_SR_REG        sr_reg[GATT_MAX_SR_PROFILES];
    UINT16              next_handle;    /* next available handle */
    tGATT_SVC_CHG       gattp_attr;     /* GATT profile attribute service change */
    tGATT_IF            gatt_if;
#if (GATTS_INCLUDED == TRUE)    
    tGATT_HDL_LIST_INFO hdl_list_info;
    tGATT_HDL_LIST_ELEM hdl_list[GATT_MAX_SR_PROFILES];
    tGATT_SRV_LIST_INFO srv_list_info;
    tGATT_SRV_LIST_ELEM srv_list[GATT_MAX_SR_PROFILES];
#endif  ///GATTS_INCLUDED == TRUE
    fixed_queue_t       *srv_chg_clt_q;   /* service change clients queue */
    fixed_queue_t       *pending_new_srv_start_q; /* pending new service start queue */
    tGATT_REG           cl_rcb[GATT_MAX_APPS];
    tGATT_CLCB          clcb[GATT_CL_MAX_LCB];  /* connection link control block*/
    tGATT_SCCB          sccb[GATT_MAX_SCCB];    /* sign complete callback function GATT_MAX_SCCB <= GATT_CL_MAX_LCB */
    UINT8               trace_level;
    UINT16              def_mtu_size;

#if GATT_CONFORMANCE_TESTING == TRUE
    BOOLEAN             enable_err_rsp;
    UINT8               req_op_code;
    UINT8               err_status;
    UINT16              handle;
#endif
#if (GATTS_INCLUDED == TRUE)
    tGATT_PROFILE_CLCB  profile_clcb[GATT_MAX_APPS];
#endif  ///GATTS_INCLUDED == TRUE
    UINT16              handle_of_h_r;          /* Handle of the handles reused characteristic value */

    tGATT_APPL_INFO       cb_info;



    tGATT_HDL_CFG           hdl_cfg;
    tGATT_BG_CONN_DEV       bgconn_dev[GATT_MAX_BG_CONN_DEV];

} tGATT_CB;

typedef struct{
    UINT16 local_mtu;
} tGATT_DEFAULT;

#define GATT_SIZE_OF_SRV_CHG_HNDL_RANGE 4

#ifdef __cplusplus
extern "C" {
#endif

extern tGATT_DEFAULT gatt_default;

/* Global GATT data */
#if GATT_DYNAMIC_MEMORY == FALSE
extern tGATT_CB  gatt_cb;
#else
extern tGATT_CB *gatt_cb_ptr;
#define gatt_cb (*gatt_cb_ptr)
#endif

#if GATT_CONFORMANCE_TESTING == TRUE
extern void gatt_set_err_rsp(BOOLEAN enable, UINT8 req_op_code, UINT8 err_status);
#endif

#ifdef __cplusplus
}
#endif

/* internal functions */
extern void gatt_init (void);
extern void gatt_free(void);

/* from gatt_main.c */
extern BOOLEAN gatt_disconnect (tGATT_TCB *p_tcb);
extern BOOLEAN gatt_act_connect (tGATT_REG *p_reg, BD_ADDR bd_addr, tBT_TRANSPORT transport);
extern BOOLEAN gatt_connect (BD_ADDR rem_bda,  tGATT_TCB *p_tcb, tBT_TRANSPORT transport);
extern void gatt_data_process (tGATT_TCB *p_tcb, BT_HDR *p_buf);
extern void gatt_update_app_use_link_flag ( tGATT_IF gatt_if, tGATT_TCB *p_tcb, BOOLEAN is_add, BOOLEAN check_acl_link);

extern void gatt_profile_db_init(void);
extern void gatt_set_ch_state(tGATT_TCB *p_tcb, tGATT_CH_STATE ch_state);
extern tGATT_CH_STATE gatt_get_ch_state(tGATT_TCB *p_tcb);
extern void gatt_init_srv_chg(void);
extern void gatt_proc_srv_chg (void);
extern void gatt_send_srv_chg_ind (BD_ADDR peer_bda);
extern void gatt_chk_srv_chg(tGATTS_SRV_CHG *p_srv_chg_clt);
extern void gatt_add_a_bonded_dev_for_srv_chg (BD_ADDR bda);

/* from gatt_attr.c */
extern UINT16 gatt_profile_find_conn_id_by_bd_addr(BD_ADDR bda);


/* Functions provided by att_protocol.c */
extern tGATT_STATUS attp_send_cl_msg (tGATT_TCB *p_tcb, UINT16 clcb_idx, UINT8 op_code, tGATT_CL_MSG *p_msg);
extern BT_HDR *attp_build_sr_msg(tGATT_TCB *p_tcb, UINT8 op_code, tGATT_SR_MSG *p_msg);
extern tGATT_STATUS attp_send_sr_msg (tGATT_TCB *p_tcb, BT_HDR *p_msg);
extern tGATT_STATUS attp_send_msg_to_l2cap(tGATT_TCB *p_tcb, BT_HDR *p_toL2CAP);

/* utility functions */
extern UINT8 *gatt_dbg_op_name(UINT8 op_code);
#if (SDP_INCLUDED == TRUE)
extern UINT32 gatt_add_sdp_record (tBT_UUID *p_uuid, UINT16 start_hdl, UINT16 end_hdl);
#endif  ///SDP_INCLUDED == TRUE
extern BOOLEAN gatt_parse_uuid_from_cmd(tBT_UUID *p_uuid, UINT16 len, UINT8 **p_data);
extern UINT8 gatt_build_uuid_to_stream(UINT8 **p_dst, tBT_UUID uuid);
extern BOOLEAN gatt_uuid_compare(tBT_UUID src, tBT_UUID tar);
extern void gatt_convert_uuid32_to_uuid128(UINT8 uuid_128[LEN_UUID_128], UINT32 uuid_32);
extern void gatt_sr_get_sec_info(BD_ADDR rem_bda, tBT_TRANSPORT transport, UINT8 *p_sec_flag, UINT8 *p_key_size);
extern void gatt_start_rsp_timer(UINT16 clcb_idx);
extern void gatt_start_conf_timer(tGATT_TCB    *p_tcb);
extern void gatt_rsp_timeout(TIMER_LIST_ENT *p_tle);
extern void gatt_ind_ack_timeout(TIMER_LIST_ENT *p_tle);
extern void gatt_start_ind_ack_timer(tGATT_TCB *p_tcb);
extern tGATT_STATUS gatt_send_error_rsp(tGATT_TCB *p_tcb, UINT8 err_code, UINT8 op_code, UINT16 handle, BOOLEAN deq);
extern void gatt_dbg_display_uuid(tBT_UUID bt_uuid);
extern tGATT_PENDING_ENC_CLCB *gatt_add_pending_enc_channel_clcb(tGATT_TCB *p_tcb, tGATT_CLCB *p_clcb );

extern tGATTS_PENDING_NEW_SRV_START *gatt_sr_is_new_srv_chg(tBT_UUID *p_app_uuid128, tBT_UUID *p_svc_uuid, UINT16 svc_inst);

extern BOOLEAN gatt_is_srv_chg_ind_pending (tGATT_TCB *p_tcb);
extern tGATTS_SRV_CHG *gatt_is_bda_in_the_srv_chg_clt_list (BD_ADDR bda);

extern BOOLEAN gatt_find_the_connected_bda(UINT8 start_idx, BD_ADDR bda, UINT8 *p_found_idx, tBT_TRANSPORT *p_transport);
extern void gatt_set_srv_chg(void);
extern void gatt_delete_dev_from_srv_chg_clt_list(BD_ADDR bd_addr);
extern tGATT_VALUE *gatt_add_pending_ind(tGATT_TCB  *p_tcb, tGATT_VALUE *p_ind);
extern tGATTS_PENDING_NEW_SRV_START *gatt_add_pending_new_srv_start( tGATTS_HNDL_RANGE *p_new_srv_start);
extern void gatt_free_srvc_db_buffer_app_id(tBT_UUID *p_app_id);
extern BOOLEAN gatt_update_listen_mode(void);
extern BOOLEAN gatt_cl_send_next_cmd_inq(tGATT_TCB *p_tcb);

/* reserved handle list */
extern tGATT_HDL_LIST_ELEM *gatt_find_hdl_buffer_by_app_id (tBT_UUID *p_app_uuid128, tBT_UUID *p_svc_uuid, UINT16 svc_inst);
extern tGATT_HDL_LIST_ELEM *gatt_find_hdl_buffer_by_handle(UINT16 handle);
extern tGATT_HDL_LIST_ELEM *gatt_find_hdl_buffer_by_attr_handle(UINT16 attr_handle);
extern tGATT_HDL_LIST_ELEM *gatt_alloc_hdl_buffer(void);
extern void gatt_free_hdl_buffer(tGATT_HDL_LIST_ELEM *p);
extern void gatt_free_attr_value_buffer(tGATT_HDL_LIST_ELEM *p);
extern BOOLEAN gatt_is_last_attribute(tGATT_SRV_LIST_INFO *p_list, tGATT_SRV_LIST_ELEM *p_start, tBT_UUID value);
extern void gatt_update_last_pri_srv_info(tGATT_SRV_LIST_INFO *p_list);
extern BOOLEAN gatt_add_a_srv_to_list(tGATT_SRV_LIST_INFO *p_list, tGATT_SRV_LIST_ELEM *p_new);
extern BOOLEAN gatt_remove_a_srv_from_list(tGATT_SRV_LIST_INFO *p_list, tGATT_SRV_LIST_ELEM *p_remove);
extern BOOLEAN gatt_add_an_item_to_list(tGATT_HDL_LIST_INFO *p_list, tGATT_HDL_LIST_ELEM *p_new);
extern BOOLEAN gatt_remove_an_item_from_list(tGATT_HDL_LIST_INFO *p_list, tGATT_HDL_LIST_ELEM *p_remove);
extern tGATTS_SRV_CHG *gatt_add_srv_chg_clt(tGATTS_SRV_CHG *p_srv_chg);

/* for background connection */
extern BOOLEAN gatt_update_auto_connect_dev (tGATT_IF gatt_if, BOOLEAN add, BD_ADDR bd_addr, BOOLEAN is_initiator);
extern BOOLEAN gatt_is_bg_dev_for_app(tGATT_BG_CONN_DEV *p_dev, tGATT_IF gatt_if);
extern BOOLEAN gatt_remove_bg_dev_for_app(tGATT_IF gatt_if, BD_ADDR bd_addr);
extern UINT8 gatt_get_num_apps_for_bg_dev(BD_ADDR bd_addr);
extern BOOLEAN gatt_find_app_for_bg_dev(BD_ADDR bd_addr, tGATT_IF *p_gatt_if);
extern tGATT_BG_CONN_DEV *gatt_find_bg_dev(BD_ADDR remote_bda);
extern void gatt_deregister_bgdev_list(tGATT_IF gatt_if);
extern void gatt_reset_bgdev_list(void);

/* server function */
extern UINT8 gatt_sr_find_i_rcb_by_handle(UINT16 handle);
extern UINT8 gatt_sr_find_i_rcb_by_app_id(tBT_UUID *p_app_uuid128, tBT_UUID *p_svc_uuid, UINT16 svc_inst);
extern UINT8 gatt_sr_alloc_rcb(tGATT_HDL_LIST_ELEM *p_list);
extern tGATT_STATUS gatt_sr_process_app_rsp (tGATT_TCB *p_tcb, tGATT_IF gatt_if, UINT32 trans_id, UINT8 op_code, tGATT_STATUS status, tGATTS_RSP *p_msg);
extern void gatt_server_handle_client_req (tGATT_TCB *p_tcb, UINT8 op_code,
        UINT16 len, UINT8 *p_data);
extern void gatt_sr_send_req_callback(UINT16 conn_id,  UINT32 trans_id,
                                      UINT8 op_code, tGATTS_DATA *p_req_data);
extern UINT32 gatt_sr_enqueue_cmd (tGATT_TCB *p_tcb, UINT8 op_code, UINT16 handle);
extern BOOLEAN gatt_cancel_open(tGATT_IF gatt_if, BD_ADDR bda);

/*   */

extern tGATT_REG *gatt_get_regcb (tGATT_IF gatt_if);
extern BOOLEAN gatt_is_clcb_allocated (UINT16 conn_id);
extern tGATT_CLCB *gatt_clcb_alloc (UINT16 conn_id);
extern void gatt_clcb_dealloc (tGATT_CLCB *p_clcb);

extern void gatt_sr_copy_prep_cnt_to_cback_cnt(tGATT_TCB *p_tcb );
extern BOOLEAN gatt_sr_is_cback_cnt_zero(tGATT_TCB *p_tcb );
extern BOOLEAN gatt_sr_is_prep_cnt_zero(tGATT_TCB *p_tcb );
extern void gatt_sr_reset_cback_cnt(tGATT_TCB *p_tcb );
extern void gatt_sr_reset_prep_cnt(tGATT_TCB *p_tcb );
extern void gatt_sr_update_cback_cnt(tGATT_TCB *p_tcb, tGATT_IF gatt_if, BOOLEAN is_inc, BOOLEAN is_reset_first);
extern void gatt_sr_update_prep_cnt(tGATT_TCB *p_tcb, tGATT_IF gatt_if, BOOLEAN is_inc, BOOLEAN is_reset_first);

extern BOOLEAN gatt_find_app_hold_link(tGATT_TCB *p_tcb, UINT8 start_idx, UINT8 *p_found_idx, tGATT_IF *p_gatt_if);
extern UINT8 gatt_num_apps_hold_link(tGATT_TCB *p_tcb);
extern UINT8 gatt_num_clcb_by_bd_addr(BD_ADDR bda);
extern tGATT_TCB *gatt_find_tcb_by_cid(UINT16 lcid);
extern tGATT_TCB *gatt_allocate_tcb_by_bdaddr(BD_ADDR bda, tBT_TRANSPORT transport);
extern tGATT_TCB *gatt_get_tcb_by_idx(UINT8 tcb_idx);
extern tGATT_TCB *gatt_find_tcb_by_addr(BD_ADDR bda, tBT_TRANSPORT transport);
extern BOOLEAN gatt_send_ble_burst_data (BD_ADDR remote_bda,  BT_HDR *p_buf);

/* GATT client functions */
extern void gatt_dequeue_sr_cmd (tGATT_TCB *p_tcb);
extern UINT8 gatt_send_write_msg(tGATT_TCB *p_tcb, UINT16 clcb_idx, UINT8 op_code, UINT16 handle,
                                 UINT16 len, UINT16 offset, UINT8 *p_data);
extern void gatt_cleanup_upon_disc(BD_ADDR bda, UINT16 reason, tBT_TRANSPORT transport);
extern void gatt_end_operation(tGATT_CLCB *p_clcb, tGATT_STATUS status, void *p_data);

extern void gatt_act_discovery(tGATT_CLCB *p_clcb);
extern void gatt_act_read(tGATT_CLCB *p_clcb, UINT16 offset);
extern void gatt_act_write(tGATT_CLCB *p_clcb, UINT8 sec_act);
extern UINT8 gatt_act_send_browse(tGATT_TCB *p_tcb, UINT16 index, UINT8 op, UINT16 s_handle, UINT16 e_handle,
                                  tBT_UUID uuid);
extern tGATT_CLCB *gatt_cmd_dequeue(tGATT_TCB *p_tcb, UINT8 *p_opcode);
extern BOOLEAN gatt_cmd_enq(tGATT_TCB *p_tcb, UINT16 clcb_idx, BOOLEAN to_send, UINT8 op_code, BT_HDR *p_buf);
extern void gatt_client_handle_server_rsp (tGATT_TCB *p_tcb, UINT8 op_code,
        UINT16 len, UINT8 *p_data);
extern void gatt_send_queue_write_cancel (tGATT_TCB *p_tcb, tGATT_CLCB *p_clcb, tGATT_EXEC_FLAG flag);

/* gatt_auth.c */
extern BOOLEAN gatt_security_check_start(tGATT_CLCB *p_clcb);
extern void gatt_verify_signature(tGATT_TCB *p_tcb, BT_HDR *p_buf);
extern tGATT_SEC_ACTION gatt_determine_sec_act(tGATT_CLCB *p_clcb );
extern tGATT_STATUS gatt_get_link_encrypt_status(tGATT_TCB *p_tcb);
extern tGATT_SEC_ACTION gatt_get_sec_act(tGATT_TCB *p_tcb);
extern void gatt_set_sec_act(tGATT_TCB *p_tcb, tGATT_SEC_ACTION sec_act);

/* gatt_db.c */
extern BOOLEAN gatts_init_service_db (tGATT_SVC_DB *p_db, tBT_UUID *p_service, BOOLEAN is_pri, UINT16 s_hdl, UINT16 num_handle);
extern UINT16 gatts_add_included_service (tGATT_SVC_DB *p_db, UINT16 s_handle, UINT16 e_handle, tBT_UUID service);
extern UINT16 gatts_add_characteristic (tGATT_SVC_DB *p_db, tGATT_PERM perm,
                                                        tGATT_CHAR_PROP property,
                                                        tBT_UUID *p_char_uuid, tGATT_ATTR_VAL *attr_val, 
                                                        tGATTS_ATTR_CONTROL *control);
extern UINT16 gatts_add_char_descr (tGATT_SVC_DB *p_db, tGATT_PERM perm, 
                                         tBT_UUID *p_dscp_uuid, tGATT_ATTR_VAL *attr_val,
                                         tGATTS_ATTR_CONTROL *control);

extern tGATT_STATUS gatts_set_attribute_value(tGATT_SVC_DB *p_db, UINT16 attr_handle, 
                                    UINT16 length, UINT8 *value);

extern tGATT_STATUS gatts_get_attribute_value(tGATT_SVC_DB *p_db, UINT16 attr_handle, 
                                    UINT16 *length, UINT8 **value);
extern BOOLEAN gatts_is_auto_response(UINT16 attr_handle);
extern tGATT_STATUS gatts_db_read_attr_value_by_type (tGATT_TCB *p_tcb, tGATT_SVC_DB *p_db, UINT8 op_code, BT_HDR *p_rsp, UINT16 s_handle,
        UINT16 e_handle, tBT_UUID type, UINT16 *p_len, tGATT_SEC_FLAG sec_flag, UINT8 key_size, UINT32 trans_id, UINT16 *p_cur_handle);
extern tGATT_STATUS gatts_read_attr_value_by_handle(tGATT_TCB *p_tcb, tGATT_SVC_DB *p_db, UINT8 op_code, UINT16 handle, UINT16 offset,
        UINT8 *p_value, UINT16 *p_len, UINT16 mtu, tGATT_SEC_FLAG sec_flag, UINT8 key_size, UINT32 trans_id);
extern tGATT_STATUS gatts_write_attr_value_by_handle(tGATT_SVC_DB *p_db,
                                                            UINT16 handle, UINT16 offset,
                                                            UINT8 *p_value, UINT16 len);
extern tGATT_STATUS gatts_write_attr_perm_check (tGATT_SVC_DB *p_db, UINT8 op_code, UINT16 handle, UINT16 offset, UINT8 *p_data,
        UINT16 len, tGATT_SEC_FLAG sec_flag, UINT8 key_size);
extern tGATT_STATUS gatts_read_attr_perm_check(tGATT_SVC_DB *p_db, BOOLEAN is_long, UINT16 handle, tGATT_SEC_FLAG sec_flag, UINT8 key_size);
extern void gatts_update_srv_list_elem(UINT8 i_sreg, UINT16 handle, BOOLEAN is_primary);
extern tBT_UUID *gatts_get_service_uuid (tGATT_SVC_DB *p_db);

extern void gatt_reset_bgdev_list(void);
extern uint16_t gatt_get_local_mtu(void);
extern void gatt_set_local_mtu(uint16_t mtu);
#endif
