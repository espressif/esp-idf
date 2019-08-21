/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
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

#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta/bta_hf_client_api.h"
#include "bta_hf_client_at.h"

#if (BTA_HF_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/
#define HFP_VERSION_1_1         0x0101
#define HFP_VERSION_1_5         0x0105
#define HFP_VERSION_1_6         0x0106

/* RFCOMM MTU SIZE */
#define BTA_HF_CLIENT_MTU       256

/* profile role for connection */
#define BTA_HF_CLIENT_ACP       0       /* accepted connection */
#define BTA_HF_CLIENT_INT       1       /* initiating connection */

/* Timer to wait for retry in case of collision */
#ifndef BTA_HF_CLIENT_COLLISION_TIMER
#define BTA_HF_CLIENT_COLLISION_TIMER  2411
#endif

enum {
    /* these events are handled by the state machine */
    BTA_HF_CLIENT_API_REGISTER_EVT = BTA_SYS_EVT_START(BTA_ID_HS),
    BTA_HF_CLIENT_API_DEREGISTER_EVT,
    BTA_HF_CLIENT_API_OPEN_EVT,
    BTA_HF_CLIENT_API_CLOSE_EVT,
    BTA_HF_CLIENT_API_AUDIO_OPEN_EVT,
    BTA_HF_CLIENT_API_AUDIO_CLOSE_EVT,
    BTA_HF_CLIENT_RFC_OPEN_EVT,
    BTA_HF_CLIENT_RFC_CLOSE_EVT,
    BTA_HF_CLIENT_RFC_SRV_CLOSE_EVT,
    BTA_HF_CLIENT_RFC_DATA_EVT,
    BTA_HF_CLIENT_DISC_ACP_RES_EVT,
    BTA_HF_CLIENT_DISC_INT_RES_EVT,
    BTA_HF_CLIENT_DISC_OK_EVT,
    BTA_HF_CLIENT_DISC_FAIL_EVT,
    BTA_HF_CLIENT_SCO_OPEN_EVT,
    BTA_HF_CLIENT_SCO_CLOSE_EVT,
    BTA_HF_CLIENT_SEND_AT_CMD_EVT,
#if (BTM_SCO_HCI_INCLUDED == TRUE )
    BTA_HF_CLIENT_CI_SCO_DATA_EVT,
#endif /* (BTM_SCO_HCI_INCLUDED == TRUE ) */
    BTA_HF_CLIENT_MAX_EVT,

    /* these events are handled outside of the state machine */
    BTA_HF_CLIENT_API_ENABLE_EVT,
    BTA_HF_CLIENT_API_DISABLE_EVT
};

/*****************************************************************************
**  Data types
*****************************************************************************/

/* data type for BTA_HF_CLIENT_API_ENABLE_EVT */
typedef struct {
    BT_HDR                     hdr;
    tBTA_HF_CLIENT_CBACK      *p_cback;
} tBTA_HF_CLIENT_API_ENABLE;

/* data type for BTA_HF_CLIENT_API_REGISTER_EVT */
typedef struct {
    BT_HDR                     hdr;
    tBTA_HF_CLIENT_CBACK      *p_cback;
    tBTA_SEC                   sec_mask;
    tBTA_HF_CLIENT_FEAT        features;
    char                       name[BTA_SERVICE_NAME_LEN + 1];
} tBTA_HF_CLIENT_API_REGISTER;

/* data type for BTA_HF_CLIENT_API_OPEN_EVT */
typedef struct {
    BT_HDR              hdr;
    BD_ADDR             bd_addr;
    tBTA_SEC            sec_mask;
} tBTA_HF_CLIENT_API_OPEN;

/* data type for BTA_HF_CLIENT_DISC_RESULT_EVT */
typedef struct {
    BT_HDR          hdr;
    UINT16          status;
} tBTA_HF_CLIENT_DISC_RESULT;

/* data type for RFCOMM events */
typedef struct {
    BT_HDR          hdr;
    UINT16          port_handle;
} tBTA_HF_CLIENT_RFC;

/* generic purpose data type for other events */
typedef struct {
    BT_HDR          hdr;
    BOOLEAN         bool_val;
    UINT8           uint8_val;
    UINT32          uint32_val1;
    UINT32          uint32_val2;
    char            str[BTA_HF_CLIENT_NUMBER_LEN + 1];
} tBTA_HF_CLIENT_DATA_VAL;

/* union of all event datatypes */
typedef union {
    BT_HDR                         hdr;
    tBTA_HF_CLIENT_API_ENABLE      api_enable;
    tBTA_HF_CLIENT_API_REGISTER    api_register;
    tBTA_HF_CLIENT_API_OPEN        api_open;
    tBTA_HF_CLIENT_DISC_RESULT     disc_result;
    tBTA_HF_CLIENT_RFC             rfc;
    tBTA_HF_CLIENT_DATA_VAL        val;

} tBTA_HF_CLIENT_DATA;

/* type for each service control block */
typedef struct {
    UINT16              serv_handle;    /* RFCOMM server handle */
    BD_ADDR             peer_addr;      /* peer bd address */
    tSDP_DISCOVERY_DB   *p_disc_db;     /* pointer to discovery database */
    UINT16              conn_handle;    /* RFCOMM handle of connected service */
    tBTA_SEC            serv_sec_mask;  /* server security mask */
    tBTA_SEC            cli_sec_mask;   /* client security mask */
    tBTA_HF_CLIENT_FEAT        features;       /* features registered by application */
    tBTA_HF_CLIENT_PEER_FEAT   peer_features;  /* peer device features */
    tBTA_HF_CLIENT_CHLD_FEAT   chld_features;  /* call handling features */
    UINT16              peer_version;   /* profile version of peer device */
    UINT8               peer_scn;       /* peer scn */
    UINT8               role;           /* initiator/acceptor role */
    UINT16              sco_idx;        /* SCO handle */
    UINT8               sco_state;      /* SCO state variable */
    BOOLEAN             sco_close_rfc;   /* TRUE if also close RFCOMM after SCO */
    BOOLEAN             retry_with_sco_only;
    BOOLEAN             deregister;     /* TRUE if service shutting down */
    BOOLEAN             svc_conn;       /* set to TRUE when service level connection is up */
    BOOLEAN             send_at_reply;  /* set to TRUE to notify framework about AT results */
    tBTA_HF_CLIENT_AT_CB at_cb;         /* AT Parser control block */
    UINT8               state;          /* state machine state */
    tBTM_SCO_CODEC_TYPE negotiated_codec; /* negotiated codec */
    TIMER_LIST_ENT      colli_timer;    /* Collision timer */
    BOOLEAN             colli_tmr_on;   /* TRUE if collision timer is active */

    UINT16              in_pkt_len;
    UINT16              out_pkt_len;
    UINT8               link_type;      /* BTM_LINK_TYPE_SCO or BTM_LINK_TYPE_ESCO */
    UINT8               tx_interval;
    UINT8               retrans_window;
    UINT8               air_mode;
} tBTA_HF_CLIENT_SCB;

/* sco states */
enum {
    BTA_HF_CLIENT_SCO_SHUTDOWN_ST,  /* no listening, no connection */
    BTA_HF_CLIENT_SCO_LISTEN_ST,    /* listening */
    BTA_HF_CLIENT_SCO_OPENING_ST,   /* connection opening */
    BTA_HF_CLIENT_SCO_OPEN_CL_ST,   /* opening connection being closed */
    BTA_HF_CLIENT_SCO_OPEN_ST,      /* open */
    BTA_HF_CLIENT_SCO_CLOSING_ST,   /* closing */
    BTA_HF_CLIENT_SCO_CLOSE_OP_ST,  /* closing sco being opened */
    BTA_HF_CLIENT_SCO_SHUTTING_ST   /* sco shutting down */
};

/* type for AG control block */
typedef struct {
    tBTA_HF_CLIENT_SCB         scb;             /* service control block */
    UINT32                     sdp_handle;
    UINT8                      scn;
    tBTA_HF_CLIENT_CBACK       *p_cback;        /* application callback */
    BOOLEAN                    msbc_enabled;
} tBTA_HF_CLIENT_CB;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* control block declaration */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_HF_CLIENT_CB bta_hf_client_cb;
#else
extern tBTA_HF_CLIENT_CB *bta_hf_client_cb_ptr;
#define bta_hf_client_cb (*bta_hf_client_cb_ptr)
#endif

/*****************************************************************************
**  Function prototypes
*****************************************************************************/

/* main functions */
extern void bta_hf_client_scb_init(void);
extern void bta_hf_client_scb_disable(void);
extern BOOLEAN bta_hf_client_hdl_event(BT_HDR *p_msg);
extern void bta_hf_client_sm_execute(UINT16 event,
                                     tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_slc_seq(BOOLEAN error);
extern void bta_hf_client_collision_cback (tBTA_SYS_CONN_STATUS status, UINT8 id,
        UINT8 app_id, BD_ADDR peer_addr);
extern void bta_hf_client_resume_open (void);

/* SDP functions */
extern BOOLEAN bta_hf_client_add_record(char *p_service_name,
                                        UINT8 scn, tBTA_HF_CLIENT_FEAT features,
                                        UINT32 sdp_handle);
extern void bta_hf_client_create_record(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_del_record(tBTA_HF_CLIENT_DATA *p_data);
extern BOOLEAN bta_hf_client_sdp_find_attr(void);
extern void bta_hf_client_do_disc(void);
extern void bta_hf_client_free_db(tBTA_HF_CLIENT_DATA *p_data);

/* RFCOMM functions */
extern void bta_hf_client_setup_port(UINT16 handle);
extern void bta_hf_client_start_server(void);
extern void bta_hf_client_close_server(void);
extern void bta_hf_client_rfc_do_open(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_rfc_do_close(tBTA_HF_CLIENT_DATA *p_data);

/* SCO functions */
extern void bta_hf_client_sco_listen(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_sco_shutdown(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_sco_conn_open(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_sco_conn_close(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_sco_open(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_sco_close(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_cback_sco(UINT8 event);

/* AT command functions */
extern void bta_hf_client_at_parse(char *buf, unsigned int len);
extern void bta_hf_client_send_at_brsf(void);
extern void bta_hf_client_send_at_bac(void);
extern void bta_hf_client_send_at_cind(BOOLEAN status);
extern void bta_hf_client_send_at_cmer(BOOLEAN activate);
extern void bta_hf_client_send_at_chld(char cmd, UINT32 idx);
extern void bta_hf_client_send_at_clip(BOOLEAN activate);
extern void bta_hf_client_send_at_ccwa(BOOLEAN activate);
extern void bta_hf_client_send_at_cmee(BOOLEAN activate);
extern void bta_hf_client_send_at_cops(BOOLEAN query);
extern void bta_hf_client_send_at_clcc(void);
extern void bta_hf_client_send_at_bvra(BOOLEAN enable);
extern void bta_hf_client_send_at_vgs(UINT32 volume);
extern void bta_hf_client_send_at_vgm(UINT32 volume);
extern void bta_hf_client_send_at_atd(char *number, UINT32 memory);
extern void bta_hf_client_send_at_bldn(void);
extern void bta_hf_client_send_at_ata(void);
extern void bta_hf_client_send_at_chup(void);
extern void bta_hf_client_send_at_btrh(BOOLEAN query, UINT32 val);
extern void bta_hf_client_send_at_vts(char code);
extern void bta_hf_client_send_at_bcc(void);
extern void bta_hf_client_send_at_bcs(UINT32 codec);
extern void bta_hf_client_send_at_cnum(void);
extern void bta_hf_client_send_at_nrec(void);
extern void bta_hf_client_send_at_binp(UINT32 action);
extern void bta_hf_client_send_at_bia(void);

/* Action functions */
extern void bta_hf_client_register(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_deregister(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_start_dereg(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_start_close(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_start_open(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_rfc_acp_open(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_rfc_open(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_rfc_fail(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_disc_fail(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_open_fail(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_rfc_close(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_disc_acp_res(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_rfc_data(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_disc_int_res(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_svc_conn_open(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_ind(tBTA_HF_CLIENT_IND_TYPE type, UINT16 value);
extern void bta_hf_client_evt_val(tBTA_HF_CLIENT_EVT type, UINT16 value);
extern void bta_hf_client_operator_name(char *name);
extern void bta_hf_client_clip(char *number);
extern void bta_hf_client_ccwa(char *number);
extern void bta_hf_client_at_result(tBTA_HF_CLIENT_AT_RESULT_TYPE type, UINT16 cme);
extern void bta_hf_client_clcc(UINT32 idx, BOOLEAN incoming, UINT8 status, BOOLEAN mpty, char *number);
extern void bta_hf_client_cnum(char *number, UINT16 service);
extern void bta_hf_client_binp(char *number);

/* Commands handling functions */
extern void bta_hf_client_dial(tBTA_HF_CLIENT_DATA *p_data);
extern void bta_hf_client_send_at_cmd(tBTA_HF_CLIENT_DATA *p_data);
#if (BTM_SCO_HCI_INCLUDED == TRUE )
extern void bta_hf_client_ci_sco_data(tBTA_HF_CLIENT_DATA *p_data);
#endif /* (BTM_SCO_HCI_INCLUDED == TRUE ) */
#endif /* #if (BTA_HF_INCLUDED == TRUE) */
