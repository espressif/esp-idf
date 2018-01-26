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
 *  This file contains the HID host main functions and state machine.
 *
 ******************************************************************************/

#include "bt_target.h"

#if defined(BTA_HH_INCLUDED) && (BTA_HH_INCLUDED == TRUE)

#include <string.h>

#include "bta_hh_api.h"
#include "bta_hh_int.h"

/*****************************************************************************
** Constants and types
*****************************************************************************/

/* state machine action enumeration list */
enum {
    BTA_HH_API_DISC_ACT,        /* HID host process API close action    */
    BTA_HH_OPEN_ACT,            /* HID host process BTA_HH_EVT_OPEN     */
    BTA_HH_CLOSE_ACT,           /* HID host process BTA_HH_EVT_CLOSE    */
    BTA_HH_DATA_ACT,            /* HID host receive data report         */
    BTA_HH_CTRL_DAT_ACT,
    BTA_HH_HANDSK_ACT,
    BTA_HH_START_SDP,           /* HID host inquery                     */
    BTA_HH_SDP_CMPL,
    BTA_HH_WRITE_DEV_ACT,
    BTA_HH_GET_DSCP_ACT,
    BTA_HH_MAINT_DEV_ACT,
    BTA_HH_OPEN_CMPL_ACT,
    BTA_HH_OPEN_FAILURE,
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    BTA_HH_GATT_CLOSE,
    BTA_HH_LE_OPEN_FAIL,
    BTA_HH_GATT_OPEN,
    BTA_HH_W4_LE_READ_CHAR,
    BTA_HH_LE_READ_CHAR,
    BTA_HH_W4_LE_READ_DESCR,
    BTA_HH_LE_READ_DESCR,
    BTA_HH_W4_LE_WRITE,
    BTA_HH_LE_WRITE,
    BTA_HH_WRITE_DESCR,
    BTA_HH_START_SEC,
    BTA_HH_SEC_CMPL,
    BTA_HH_LE_UPDATE_SCPP,
    BTA_HH_GATT_ENC_CMPL,
#endif
    BTA_HH_NUM_ACTIONS
};

#define BTA_HH_IGNORE       BTA_HH_NUM_ACTIONS

/* type for action functions */
typedef void (*tBTA_HH_ACTION)(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data);

/* action functions */
const tBTA_HH_ACTION bta_hh_action[] = {
    bta_hh_api_disc_act,
    bta_hh_open_act,
    bta_hh_close_act,
    bta_hh_data_act,
    bta_hh_ctrl_dat_act,
    bta_hh_handsk_act,
    bta_hh_start_sdp,
    bta_hh_sdp_cmpl,
    bta_hh_write_dev_act,
    bta_hh_get_dscp_act,
    bta_hh_maint_dev_act,
    bta_hh_open_cmpl_act,
    bta_hh_open_failure
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    , bta_hh_gatt_close
    , bta_hh_le_open_fail
    , bta_hh_gatt_open
    , bta_hh_w4_le_read_char_cmpl
    , bta_hh_le_read_char_cmpl
    , bta_hh_w4_le_read_descr_cmpl
    , bta_hh_le_read_descr_cmpl
    , bta_hh_w4_le_write_cmpl
    , bta_hh_le_write_cmpl
    , bta_hh_le_write_char_descr_cmpl
    , bta_hh_start_security
    , bta_hh_security_cmpl
    , bta_hh_le_update_scpp
    , bta_hh_le_notify_enc_cmpl
#endif
};

/* state table information */
#define BTA_HH_ACTION                   0       /* position of action */
#define BTA_HH_NEXT_STATE               1       /* position of next state */
#define BTA_HH_NUM_COLS                 2       /* number of columns */

/* state table for idle state */
const UINT8 bta_hh_st_idle[][BTA_HH_NUM_COLS] = {
    /* Event                          Action                    Next state */
    /* BTA_HH_API_OPEN_EVT      */    {BTA_HH_START_SDP,     BTA_HH_W4_CONN_ST },
    /* BTA_HH_API_CLOSE_EVT     */    {BTA_HH_IGNORE,        BTA_HH_IDLE_ST    },
    /* BTA_HH_INT_OPEN_EVT      */    {BTA_HH_OPEN_ACT,      BTA_HH_W4_CONN_ST },
    /* BTA_HH_INT_CLOSE_EVT     */    {BTA_HH_CLOSE_ACT,     BTA_HH_IDLE_ST    },
    /* BTA_HH_INT_DATA_EVT      */    {BTA_HH_IGNORE,        BTA_HH_IDLE_ST    },
    /* BTA_HH_INT_CTRL_DATA     */    {BTA_HH_IGNORE,        BTA_HH_IDLE_ST    },
    /* BTA_HH_INT_HANDSK_EVT    */    {BTA_HH_IGNORE,        BTA_HH_IDLE_ST    },
    /* BTA_HH_SDP_CMPL_EVT      */    {BTA_HH_IGNORE,        BTA_HH_IDLE_ST    },
    /* BTA_HH_API_WRITE_DEV_EVT */    {BTA_HH_IGNORE,        BTA_HH_IDLE_ST    },
    /* BTA_HH_API_GET_DSCP_EVT  */    {BTA_HH_IGNORE,        BTA_HH_IDLE_ST    },
    /* BTA_HH_API_MAINT_DEV_EVT */    {BTA_HH_MAINT_DEV_ACT, BTA_HH_IDLE_ST    },
    /* BTA_HH_OPEN_CMPL_EVT        */  {BTA_HH_OPEN_CMPL_ACT, BTA_HH_CONN_ST    }
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    /* BTA_HH_GATT_CLOSE_EVT    */   , {BTA_HH_IGNORE,         BTA_HH_IDLE_ST    }
    /* BTA_HH_GATT_OPEN_EVT    */    , {BTA_HH_GATT_OPEN,      BTA_HH_W4_CONN_ST }
    /* BTA_HH_START_ENC_EVT    */    , {BTA_HH_IGNORE,         BTA_HH_IDLE_ST    }
    /* BTA_HH_ENC_CMPL_EVT     */    , {BTA_HH_IGNORE,         BTA_HH_IDLE_ST    }
    /* READ_CHAR_CMPL_EVT */         , {BTA_HH_IGNORE,       BTA_HH_IDLE_ST  }
    /* BTA_HH_GATT_WRITE_CMPL_EVT*/    , {BTA_HH_IGNORE,       BTA_HH_IDLE_ST  }
    /* READ_DESCR_CMPL_EVT */        , {BTA_HH_IGNORE,           BTA_HH_IDLE_ST  }
    /* WRITE_DESCR_CMPL_EVT */       , {BTA_HH_IGNORE,           BTA_HH_IDLE_ST   }
    /* SCPP_UPDATE_EVT */            , {BTA_HH_IGNORE,           BTA_HH_IDLE_ST   }
    /* BTA_HH_GATT_ENC_CMPL_EVT */   , {BTA_HH_IGNORE,           BTA_HH_IDLE_ST   }
#endif

};


const UINT8 bta_hh_st_w4_conn[][BTA_HH_NUM_COLS] = {
    /* Event                          Action                 Next state */
    /* BTA_HH_API_OPEN_EVT      */    {BTA_HH_IGNORE,        BTA_HH_W4_CONN_ST },
    /* BTA_HH_API_CLOSE_EVT     */    {BTA_HH_IGNORE,        BTA_HH_IDLE_ST    },
    /* BTA_HH_INT_OPEN_EVT      */    {BTA_HH_OPEN_ACT,      BTA_HH_W4_CONN_ST },
    /* BTA_HH_INT_CLOSE_EVT     */    {BTA_HH_OPEN_FAILURE,  BTA_HH_IDLE_ST    },
    /* BTA_HH_INT_DATA_EVT      */    {BTA_HH_IGNORE,        BTA_HH_W4_CONN_ST },
    /* BTA_HH_INT_CTRL_DATA     */    {BTA_HH_IGNORE,        BTA_HH_W4_CONN_ST },
    /* BTA_HH_INT_HANDSK_EVT    */    {BTA_HH_IGNORE,        BTA_HH_W4_CONN_ST },
    /* BTA_HH_SDP_CMPL_EVT      */    {BTA_HH_SDP_CMPL,      BTA_HH_W4_CONN_ST },
    /* BTA_HH_API_WRITE_DEV_EVT */    {BTA_HH_WRITE_DEV_ACT, BTA_HH_W4_CONN_ST },
    /* BTA_HH_API_GET_DSCP_EVT  */    {BTA_HH_IGNORE,        BTA_HH_W4_CONN_ST },
    /* BTA_HH_API_MAINT_DEV_EVT */    {BTA_HH_MAINT_DEV_ACT, BTA_HH_IDLE_ST    },
    /* BTA_HH_OPEN_CMPL_EVT     */    {BTA_HH_OPEN_CMPL_ACT, BTA_HH_CONN_ST    }
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    /* BTA_HH_GATT_CLOSE_EVT    */   , {BTA_HH_LE_OPEN_FAIL,  BTA_HH_IDLE_ST    }
    /* BTA_HH_GATT_OPEN_EVT    */    , {BTA_HH_GATT_OPEN,     BTA_HH_W4_CONN_ST }
    /* BTA_HH_START_ENC_EVT    */    , {BTA_HH_START_SEC,     BTA_HH_W4_SEC     }
    /* BTA_HH_ENC_CMPL_EVT     */    , {BTA_HH_IGNORE,        BTA_HH_W4_CONN_ST }
    /* READ_CHAR_CMPL_EVT */        , {BTA_HH_W4_LE_READ_CHAR,    BTA_HH_W4_CONN_ST  }
    /* BTA_HH_GATT_WRITE_CMPL_EVT*/  , {BTA_HH_W4_LE_WRITE,    BTA_HH_W4_CONN_ST  }
    /* READ_DESCR_CMPL_EVT */        , {BTA_HH_W4_LE_READ_DESCR, BTA_HH_W4_CONN_ST  }
    /* WRITE_DESCR_CMPL_EVT */       , {BTA_HH_WRITE_DESCR,   BTA_HH_W4_CONN_ST   }
    /* SCPP_UPDATE_EVT */            , {BTA_HH_IGNORE,           BTA_HH_W4_CONN_ST   }
    /* BTA_HH_GATT_ENC_CMPL_EVT */   , {BTA_HH_IGNORE,        BTA_HH_W4_CONN_ST }
#endif
};


const UINT8 bta_hh_st_connected[][BTA_HH_NUM_COLS] = {
    /* Event                          Action                 Next state */
    /* BTA_HH_API_OPEN_EVT      */    {BTA_HH_IGNORE,        BTA_HH_CONN_ST    },
    /* BTA_HH_API_CLOSE_EVT     */    {BTA_HH_API_DISC_ACT,  BTA_HH_CONN_ST    },
    /* BTA_HH_INT_OPEN_EVT      */    {BTA_HH_OPEN_ACT,      BTA_HH_CONN_ST    },
    /* BTA_HH_INT_CLOSE_EVT     */    {BTA_HH_CLOSE_ACT,     BTA_HH_IDLE_ST    },
    /* BTA_HH_INT_DATA_EVT      */    {BTA_HH_DATA_ACT,      BTA_HH_CONN_ST    },
    /* BTA_HH_INT_CTRL_DATA     */    {BTA_HH_CTRL_DAT_ACT,  BTA_HH_CONN_ST    },
    /* BTA_HH_INT_HANDSK_EVT    */    {BTA_HH_HANDSK_ACT,    BTA_HH_CONN_ST    },
    /* BTA_HH_SDP_CMPL_EVT      */    {BTA_HH_IGNORE,         BTA_HH_CONN_ST       },
    /* BTA_HH_API_WRITE_DEV_EVT */    {BTA_HH_WRITE_DEV_ACT, BTA_HH_CONN_ST    },
    /* BTA_HH_API_GET_DSCP_EVT  */    {BTA_HH_GET_DSCP_ACT,  BTA_HH_CONN_ST    },
    /* BTA_HH_API_MAINT_DEV_EVT */    {BTA_HH_MAINT_DEV_ACT, BTA_HH_CONN_ST    },
    /* BTA_HH_OPEN_CMPL_EVT        */    {BTA_HH_IGNORE,         BTA_HH_CONN_ST    }
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    /* BTA_HH_GATT_CLOSE_EVT    */    , {BTA_HH_GATT_CLOSE,    BTA_HH_IDLE_ST    }
    /* BTA_HH_GATT_OPEN_EVT    */    , {BTA_HH_IGNORE,        BTA_HH_CONN_ST    }
    /* BTA_HH_START_ENC_EVT    */    , {BTA_HH_IGNORE,        BTA_HH_CONN_ST     }
    /* BTA_HH_ENC_CMPL_EVT     */    , {BTA_HH_IGNORE,        BTA_HH_CONN_ST     }
    /* READ_CHAR_CMPL_EVT */         , {BTA_HH_LE_READ_CHAR,  BTA_HH_CONN_ST     }
    /* WRITE_CHAR_CMPL_EVT*/         , {BTA_HH_LE_WRITE,      BTA_HH_CONN_ST     }
    /* READ_DESCR_CMPL_EVT */        , {BTA_HH_LE_READ_DESCR, BTA_HH_CONN_ST     }  /* do not currently read any descr when connection up */
    /* WRITE_DESCR_CMPL_EVT */       , {BTA_HH_WRITE_DESCR,   BTA_HH_CONN_ST     }  /* do not currently write any descr when connection up */
    /* SCPP_UPDATE_EVT */            , {BTA_HH_LE_UPDATE_SCPP,  BTA_HH_CONN_ST   }
    /* BTA_HH_GATT_ENC_CMPL_EVT */   , {BTA_HH_IGNORE,        BTA_HH_CONN_ST     }
#endif
};
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
const UINT8 bta_hh_st_w4_sec[][BTA_HH_NUM_COLS] = {
    /* Event                          Action                 Next state */
    /* BTA_HH_API_OPEN_EVT      */    {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_API_CLOSE_EVT     */    {BTA_HH_API_DISC_ACT,  BTA_HH_W4_SEC  },
    /* BTA_HH_INT_OPEN_EVT      */    {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_INT_CLOSE_EVT     */    {BTA_HH_OPEN_FAILURE,  BTA_HH_IDLE_ST },
    /* BTA_HH_INT_DATA_EVT      */    {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_INT_CTRL_DATA     */    {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_INT_HANDSK_EVT    */    {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_SDP_CMPL_EVT      */    {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_API_WRITE_DEV_EVT */    {BTA_HH_IGNORE  ,      BTA_HH_W4_SEC  },
    /* BTA_HH_API_GET_DSCP_EVT  */    {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_API_MAINT_DEV_EVT */    {BTA_HH_MAINT_DEV_ACT, BTA_HH_W4_SEC  },
    /* BTA_HH_OPEN_CMPL_EVT     */    {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_GATT_CLOSE_EVT    */    {BTA_HH_LE_OPEN_FAIL,  BTA_HH_IDLE_ST },
    /* BTA_HH_GATT_OPEN_EVT    */     {BTA_HH_IGNORE,        BTA_HH_W4_SEC  },
    /* BTA_HH_START_ENC_EVT    */     {BTA_HH_IGNORE,        BTA_HH_W4_SEC     },
    /* BTA_HH_ENC_CMPL_EVT     */     {BTA_HH_SEC_CMPL,      BTA_HH_W4_CONN_ST },
    /* READ_CHAR_CMPL_EVT */          {BTA_HH_IGNORE,        BTA_HH_W4_SEC     },
    /* BTA_HH_GATT_WRITE_CMPL_EVT*/   {BTA_HH_IGNORE,        BTA_HH_W4_SEC     },
    /* READ_DESCR_CMPL_EVT */         {BTA_HH_IGNORE,        BTA_HH_W4_SEC   },
    /* WRITE_DESCR_CMPL_EVT */        {BTA_HH_IGNORE,        BTA_HH_W4_SEC   }
    /* SCPP_UPDATE_EVT */            , {BTA_HH_IGNORE,        BTA_HH_W4_SEC   }
    /* BTA_HH_GATT_ENC_CMPL_EVT */   , {BTA_HH_GATT_ENC_CMPL, BTA_HH_W4_SEC   }
};
#endif

/* type for state table */
typedef const UINT8 (*tBTA_HH_ST_TBL)[BTA_HH_NUM_COLS];

/* state table */
const tBTA_HH_ST_TBL bta_hh_st_tbl[] = {
    bta_hh_st_idle,
    bta_hh_st_w4_conn,
    bta_hh_st_connected
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    , bta_hh_st_w4_sec
#endif
};

/*****************************************************************************
** Global data
*****************************************************************************/
#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_HH_CB  bta_hh_cb;
#else
tBTA_HH_CB  *bta_hh_cb_ptr;
#endif
/*****************************************************************************
** Static functions
*****************************************************************************/
#if BTA_HH_DEBUG == TRUE
static char *bta_hh_evt_code(tBTA_HH_INT_EVT evt_code);
static char *bta_hh_state_code(tBTA_HH_STATE state_code);
#endif

/*******************************************************************************
**
** Function         bta_hh_sm_execute
**
** Description      State machine event handling function for HID Host
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_sm_execute(tBTA_HH_DEV_CB *p_cb, UINT16 event, tBTA_HH_DATA *p_data)
{
    tBTA_HH_ST_TBL  state_table;
    UINT8           action;
    tBTA_HH         cback_data;
    tBTA_HH_EVT     cback_event = 0;
#if BTA_HH_DEBUG == TRUE
    tBTA_HH_STATE   in_state ;
    UINT16          debug_event = event;
#endif

    memset(&cback_data, 0, sizeof(tBTA_HH));

    /* handle exception, no valid control block was found */
    if (!p_cb) {
        /* BTA HH enabled already? otherwise ignore the event although it's bad*/
        if (bta_hh_cb.p_cback != NULL) {
            switch (event) {
            /* no control block available for new connection */
            case BTA_HH_API_OPEN_EVT:
                cback_event = BTA_HH_OPEN_EVT;
                /* build cback data */
                bdcpy(cback_data.conn.bda, ((tBTA_HH_API_CONN *)p_data)->bd_addr);
                cback_data.conn.status  = BTA_HH_ERR_DB_FULL;
                cback_data.conn.handle  = BTA_HH_INVALID_HANDLE;
                break;
            /* DB full, BTA_HhAddDev */
            case BTA_HH_API_MAINT_DEV_EVT:
                cback_event = p_data->api_maintdev.sub_event;

                if (p_data->api_maintdev.sub_event == BTA_HH_ADD_DEV_EVT) {
                    bdcpy(cback_data.dev_info.bda, p_data->api_maintdev.bda);
                    cback_data.dev_info.status    = BTA_HH_ERR_DB_FULL;
                    cback_data.dev_info.handle    = BTA_HH_INVALID_HANDLE;
                } else {
                    cback_data.dev_info.status    = BTA_HH_ERR_HDL;
                    cback_data.dev_info.handle    = (UINT8)p_data->api_maintdev.hdr.layer_specific;
                }
                break;
            case BTA_HH_API_WRITE_DEV_EVT:
                cback_event = (p_data->api_sndcmd.t_type - BTA_HH_FST_BTE_TRANS_EVT) +
                              BTA_HH_FST_TRANS_CB_EVT;
                if (p_data->api_sndcmd.p_data != NULL) {
                    osi_free(p_data->api_sndcmd.p_data);
                }
                if (p_data->api_sndcmd.t_type == HID_TRANS_SET_PROTOCOL ||
                        p_data->api_sndcmd.t_type == HID_TRANS_SET_REPORT ||
                        p_data->api_sndcmd.t_type == HID_TRANS_SET_IDLE) {
                    cback_data.dev_status.status = BTA_HH_ERR_HDL;
                    cback_data.dev_status.handle = (UINT8)p_data->api_sndcmd.hdr.layer_specific;
                } else if (p_data->api_sndcmd.t_type != HID_TRANS_DATA &&
                           p_data->api_sndcmd.t_type != HID_TRANS_CONTROL) {
                    cback_data.hs_data.handle = (UINT8)p_data->api_sndcmd.hdr.layer_specific;
                    cback_data.hs_data.status = BTA_HH_ERR_HDL;
                    /* hs_data.rsp_data will be all zero, which is not valid value */
                } else if (p_data->api_sndcmd.t_type == HID_TRANS_CONTROL &&
                           p_data->api_sndcmd.param == BTA_HH_CTRL_VIRTUAL_CABLE_UNPLUG) {
                    cback_data.status = BTA_HH_ERR_HDL;
                    cback_event = BTA_HH_VC_UNPLUG_EVT;
                } else {
                    cback_event = 0;
                }
                break;

            case BTA_HH_API_CLOSE_EVT:
                cback_event = BTA_HH_CLOSE_EVT;

                cback_data.dev_status.status = BTA_HH_ERR_HDL;
                cback_data.dev_status.handle = (UINT8)p_data->api_sndcmd.hdr.layer_specific;
                break;

            default:
                /* invalid handle, call bad API event */
                APPL_TRACE_ERROR("wrong device handle: [%d]", p_data->hdr.layer_specific);
                /* Free the callback buffer now */
                if (p_data != NULL && p_data->hid_cback.p_data != NULL) {
                    osi_free(p_data->hid_cback.p_data);
                    p_data->hid_cback.p_data = NULL;
                }
                break;
            }
            if (cback_event) {
                (* bta_hh_cb.p_cback)(cback_event, &cback_data);
            }
        }
    }
    /* corresponding CB is found, go to state machine */
    else {
#if BTA_HH_DEBUG == TRUE
        in_state = p_cb->state;
        APPL_TRACE_EVENT("bta_hh_sm_execute: State 0x%02x [%s], Event [%s]",
                         in_state, bta_hh_state_code(in_state),
                         bta_hh_evt_code(debug_event));
#endif

        if ((p_cb->state == BTA_HH_NULL_ST) || (p_cb->state >= BTA_HH_INVALID_ST)) {
            APPL_TRACE_ERROR("bta_hh_sm_execute: Invalid state State = 0x%x, Event = %d",
                             p_cb->state, event);
            return;
        }
        state_table = bta_hh_st_tbl[p_cb->state - 1];

        event &= 0xff;

        p_cb->state = state_table[event][BTA_HH_NEXT_STATE] ;

        if ((action = state_table[event][BTA_HH_ACTION]) != BTA_HH_IGNORE) {
            (*bta_hh_action[action])(p_cb, p_data);
        }

#if BTA_HH_DEBUG == TRUE
        if (in_state != p_cb->state) {
            APPL_TRACE_DEBUG("HH State Change: [%s] -> [%s] after Event [%s]",
                             bta_hh_state_code(in_state),
                             bta_hh_state_code(p_cb->state),
                             bta_hh_evt_code(debug_event));
        }
#endif
    }

    return;
}
/*******************************************************************************
**
** Function         bta_hh_hdl_event
**
** Description      HID host main event handling function.
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_hh_hdl_event(BT_HDR *p_msg)
{
    UINT8           index = BTA_HH_IDX_INVALID;
    tBTA_HH_DEV_CB *p_cb = NULL;

    switch (p_msg->event) {
    case BTA_HH_API_ENABLE_EVT:
        bta_hh_api_enable((tBTA_HH_DATA *) p_msg);
        break;

    case BTA_HH_API_DISABLE_EVT:
        bta_hh_api_disable();
        break;

    case BTA_HH_DISC_CMPL_EVT:          /* disable complete */
        bta_hh_disc_cmpl();
        break;

    default:
        /* all events processed in state machine need to find corresponding
            CB before proceed */
        if (p_msg->event == BTA_HH_API_OPEN_EVT) {
            index = bta_hh_find_cb(((tBTA_HH_API_CONN *)p_msg)->bd_addr);
        } else if (p_msg->event == BTA_HH_API_MAINT_DEV_EVT) {
            /* if add device */
            if (((tBTA_HH_MAINT_DEV *)p_msg)->sub_event == BTA_HH_ADD_DEV_EVT) {
                index = bta_hh_find_cb(((tBTA_HH_MAINT_DEV *)p_msg)->bda);
            } else { /* else remove device by handle */
                index = bta_hh_dev_handle_to_cb_idx((UINT8)p_msg->layer_specific);
// btla-specific ++
                /* If BT disable is done while the HID device is connected and Link_Key uses unauthenticated combination
                  * then we can get into a situation where remove_bonding is called with the index set to 0 (without getting
                  * cleaned up). Only when VIRTUAL_UNPLUG is called do we cleanup the index and make it MAX_KNOWN.
                  * So if REMOVE_DEVICE is called and in_use is FALSE then we should treat this as a NULL p_cb. Hence we
                  * force the index to be IDX_INVALID
                  */
                if ((index != BTA_HH_IDX_INVALID) &&
                        (bta_hh_cb.kdev[index].in_use == FALSE)) {
                    index = BTA_HH_IDX_INVALID;
                }
// btla-specific --
            }
        } else if (p_msg->event == BTA_HH_INT_OPEN_EVT) {
            index = bta_hh_find_cb(((tBTA_HH_CBACK_DATA *)p_msg)->addr);
        } else {
            index = bta_hh_dev_handle_to_cb_idx((UINT8)p_msg->layer_specific);
        }

        if (index != BTA_HH_IDX_INVALID) {
            p_cb = &bta_hh_cb.kdev[index];
        }

#if BTA_HH_DEBUG
        APPL_TRACE_DEBUG("bta_hh_hdl_event:: handle = %d dev_cb[%d] ", p_msg->layer_specific, index);
#endif
        bta_hh_sm_execute(p_cb, p_msg->event, (tBTA_HH_DATA *) p_msg);
    }
    return (TRUE);
}

/*****************************************************************************
**  Debug Functions
*****************************************************************************/
#if BTA_HH_DEBUG
/*******************************************************************************
**
** Function         bta_hh_evt_code
**
** Description
**
** Returns          void
**
*******************************************************************************/
static char *bta_hh_evt_code(tBTA_HH_INT_EVT evt_code)
{
    switch (evt_code) {
    case BTA_HH_API_DISABLE_EVT:
        return "BTA_HH_API_DISABLE_EVT";
    case BTA_HH_API_ENABLE_EVT:
        return "BTA_HH_API_ENABLE_EVT";
    case BTA_HH_API_OPEN_EVT:
        return "BTA_HH_API_OPEN_EVT";
    case BTA_HH_API_CLOSE_EVT:
        return "BTA_HH_API_CLOSE_EVT";
    case BTA_HH_INT_OPEN_EVT:
        return "BTA_HH_INT_OPEN_EVT";
    case BTA_HH_INT_CLOSE_EVT:
        return "BTA_HH_INT_CLOSE_EVT";
    case BTA_HH_INT_HANDSK_EVT:
        return "BTA_HH_INT_HANDSK_EVT";
    case BTA_HH_INT_DATA_EVT:
        return "BTA_HH_INT_DATA_EVT";
    case BTA_HH_INT_CTRL_DATA:
        return "BTA_HH_INT_CTRL_DATA";
    case BTA_HH_API_WRITE_DEV_EVT:
        return "BTA_HH_API_WRITE_DEV_EVT";
    case BTA_HH_SDP_CMPL_EVT:
        return "BTA_HH_SDP_CMPL_EVT";
    case BTA_HH_DISC_CMPL_EVT:
        return "BTA_HH_DISC_CMPL_EVT";
    case BTA_HH_API_MAINT_DEV_EVT:
        return "BTA_HH_API_MAINT_DEV_EVT";
    case BTA_HH_API_GET_DSCP_EVT:
        return "BTA_HH_API_GET_DSCP_EVT";
    case BTA_HH_OPEN_CMPL_EVT:
        return "BTA_HH_OPEN_CMPL_EVT";
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    case BTA_HH_GATT_CLOSE_EVT:
        return "BTA_HH_GATT_CLOSE_EVT";
    case BTA_HH_GATT_OPEN_EVT:
        return "BTA_HH_GATT_OPEN_EVT";
    case BTA_HH_START_ENC_EVT:
        return "BTA_HH_START_ENC_EVT";
    case BTA_HH_ENC_CMPL_EVT:
        return "BTA_HH_ENC_CMPL_EVT";
    case BTA_HH_GATT_READ_CHAR_CMPL_EVT:
        return "BTA_HH_GATT_READ_CHAR_CMPL_EVT";
    case BTA_HH_GATT_WRITE_CHAR_CMPL_EVT:
        return "BTA_HH_GATT_WRITE_CHAR_CMPL_EVT";
    case BTA_HH_GATT_READ_DESCR_CMPL_EVT:
        return "BTA_HH_GATT_READ_DESCR_CMPL_EVT";
    case BTA_HH_GATT_WRITE_DESCR_CMPL_EVT:
        return "BTA_HH_GATT_WRITE_DESCR_CMPL_EVT";
#endif
    default:
        return "unknown HID Host event code";
    }
}

/*******************************************************************************
**
** Function         bta_hh_state_code
**
** Description      get string representation of HID host state code.
**
** Returns          void
**
*******************************************************************************/
static char *bta_hh_state_code(tBTA_HH_STATE state_code)
{
    switch (state_code) {
    case BTA_HH_NULL_ST:
        return"BTA_HH_NULL_ST";
    case BTA_HH_IDLE_ST:
        return "BTA_HH_IDLE_ST";
    case BTA_HH_W4_CONN_ST:
        return "BTA_HH_W4_CONN_ST";
    case BTA_HH_CONN_ST:
        return "BTA_HH_CONN_ST";
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    case BTA_HH_W4_SEC:
        return "BTA_HH_W4_SEC";
#endif
    default:
        return "unknown HID Host state";
    }
}

#endif  /* Debug Functions */

#endif /* BTA_HH_INCLUDED */
