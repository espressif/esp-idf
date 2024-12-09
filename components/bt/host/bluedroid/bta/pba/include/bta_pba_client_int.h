/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "osi/list.h"
#include "common/bt_target.h"
#include "stack/sdp_api.h"
#include "stack/obex_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta/bta_pba_client_api.h"

#if BTA_PBA_CLIENT_INCLUDED

#define PBA_CLIENT_MAX_CONNECTION       2

#define PBA_CLIENT_MAX_MTU              L2CAP_MTU_SIZE              /* RFCOMM is base on L2CAP, its MTU will smaller than this */
#define PBA_CLIENT_MIN_MTU              255

enum {
    /* these events are handled by the state machine */
    BTA_PBA_CLIENT_API_OPEN_EVT = BTA_SYS_EVT_START(BTA_ID_PBC),
    BTA_PBA_CLIENT_API_CLOSE_EVT,
    BTA_PBA_CLIENT_API_REQ_EVT,
    BTA_PBA_CLIENT_DISC_RES_EVT,
    BTA_PBA_CLIENT_AUTHENTICATE_EVT,
    BTA_PBA_CLIENT_CONNECT_EVT,
    BTA_PBA_CLIENT_RESPONSE_EVT,
    BTA_PBA_CLIENT_RESPONSE_FINAL_EVT,
    BTA_PBA_CLIENT_GOEP_CONNECT_EVT,
    BTA_PBA_CLIENT_GOEP_DISCONNECT_EVT,

    /* these events are handled outside of the state machine */
    BTA_PBA_CLIENT_API_ENABLE_EVT,
    BTA_PBA_CLIENT_API_DISABLE_EVT,
    BTA_PBA_CLIENT_API_REGISTER_EVT,
    BTA_PBA_CLIENT_API_DEREGISTER_EVT,
};

typedef enum {
    BTA_PBA_CLIENT_OP_PULL_PHONE_BOOK,
    BTA_PBA_CLIENT_OP_SET_PHONE_BOOK,
    BTA_PBA_CLIENT_OP_PULL_VCARD_LISTING,
    BTA_PBA_CLIENT_OP_PULL_VCARD_ENTRY,
} tBTA_PBA_CLIENT_OP;

typedef struct {
    BT_HDR                      hdr;
    tBTA_PBA_CLIENT_CBACK      *p_cback;
} tBTA_PBA_CLIENT_API_ENABLE;

typedef struct {
    BT_HDR                      hdr;
    char                        name[BTA_SERVICE_NAME_LEN + 1];
} tBTA_PBA_CLIENT_API_REGISTER;

typedef struct {
    BT_HDR                      hdr;
    BD_ADDR                     bd_addr;
    tBTA_SEC                    sec_mask;
    UINT16                      mtu;
    UINT32                      supported_feat;
} tBTA_PBA_CLIENT_API_OPEN;

typedef struct {
    BT_HDR                      hdr;
    UINT8                       operation;
    UINT8                       flags;
    char                       *name;
    UINT16                      app_param_len;
    UINT8                      *app_param;
} tBTA_PBA_CLIENT_API_REQ;

typedef struct {
    BT_HDR                      hdr;
    UINT16                      status;
} tBTA_PBA_CLIENT_DISC_RESULT;

typedef struct {
    BT_HDR                      hdr;
    UINT16                      our_mtu;
    UINT16                      peer_mtu;
} tBTA_PBA_CLIENT_GOEP_CONNECT;

typedef struct {
    BT_HDR                      hdr;
    UINT16                      reason;
} tBTA_PBA_CLIENT_GOEP_DISCONNECT;

typedef struct {
    BT_HDR                      hdr;
    BT_HDR                     *pkt;
    UINT8                       opcode;
    BOOLEAN                     srm_en;
    BOOLEAN                     srm_wait;
} tBTA_PBA_CLIENT_GOEP_RESPONSE;

typedef union {
    BT_HDR                          hdr;
    tBTA_PBA_CLIENT_API_ENABLE      api_enable;
    tBTA_PBA_CLIENT_API_REGISTER    api_register;
    tBTA_PBA_CLIENT_API_OPEN        api_open;
    tBTA_PBA_CLIENT_API_REQ         api_req;
    tBTA_PBA_CLIENT_DISC_RESULT     disc_result;
    tBTA_PBA_CLIENT_GOEP_CONNECT    goep_connect;
    tBTA_PBA_CLIENT_GOEP_DISCONNECT goep_disconnect;
    tBTA_PBA_CLIENT_GOEP_RESPONSE   goep_response;
} tBTA_PBA_CLIENT_DATA;

typedef struct {
    BD_ADDR                     bd_addr;                    /* peer BD address */
    tSDP_DISCOVERY_DB          *p_disc_db;                  /* pointer to discovery database */
    tBTA_SEC                    sec_mask;                   /* security mask */
    UINT16                      peer_version;               /* peer profile version */
    UINT16                      peer_l2cap_psm;             /* peer l2cap psm */
    UINT8                       peer_rfcomm_scn;            /* peer rfcomm scn */
    UINT8                       peer_supported_repo;        /* peer supported repositories */
    UINT32                      peer_supported_feat;        /* peer supported features */
    UINT32                      our_supported_feat;         /* we supported features */
    BOOLEAN                     send_supported_feat;        /* whether we should send supported features in connect request */
    UINT16                      goep_handle;                /* goep connection handle */
    UINT32                      goep_cid;                   /* goep connection id */
    UINT16                      max_rx;                     /* max rx bytes */
    UINT16                      max_tx;                     /* max tx bytes */
    BOOLEAN                     authenticate;               /* whether we are authenticated */
    tBTA_PBA_CLIENT_OP          operation;                  /* ongoing or last operations */
    UINT8                       state;                      /* main state machine */
    UINT8                       allocated;                  /* index + 1 if allocated, otherwise 0 */
} tBTA_PBA_CLIENT_CCB;

typedef struct {
    tBTA_PBA_CLIENT_CCB         ccb[PBA_CLIENT_MAX_CONNECTION];     /* connection control block */
    tBTA_PBA_CLIENT_CBACK      *p_cback;                            /* message callback to upper */
    UINT32                      sdp_handle;                         /* sdp record handle */
    UINT8                       trace_level;                        /* debug trace level */
} tBTA_PBA_CLIENT_CB;

#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_PBA_CLIENT_CB bta_pba_client_cb;
#else
extern tBTA_PBA_CLIENT_CB *bta_pba_client_cb_ptr;
#define bta_pba_client_cb (*bta_pba_client_cb_ptr)
#endif

extern void bta_pba_client_api_enable(tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_api_disable(tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_api_register(tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_api_deregister(tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_api_open(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_api_close(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_api_req(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_do_connect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_authenticate(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_force_disconnect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);

extern void bta_pba_client_connect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_response(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
extern void bta_pba_client_response_final(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
void bta_pba_client_goep_connect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
void bta_pba_client_goep_disconnect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
void bta_pba_client_free_response(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);
void bta_pba_client_free_sdp_db(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);

extern void bta_pba_client_create_record(const char *p_service_name);
extern void bta_pba_client_del_record(void);
extern BOOLEAN bta_pba_client_sdp_find_attr(tBTA_PBA_CLIENT_CCB *p_ccb);
extern BOOLEAN bta_pba_client_do_disc(tBTA_PBA_CLIENT_CCB *p_ccb);
extern void bta_pba_client_free_db(tBTA_PBA_CLIENT_CCB *p_ccb);

extern void bta_pba_client_sm_execute(tBTA_PBA_CLIENT_CCB *p_ccb, UINT16 event, tBTA_PBA_CLIENT_DATA *p_data);
extern BOOLEAN bta_pba_client_hdl_event(BT_HDR *p_msg);

#endif
