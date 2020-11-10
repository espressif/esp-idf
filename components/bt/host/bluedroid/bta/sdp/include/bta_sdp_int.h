

/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
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
 *  This is the private interface file for the BTA SDP I/F
 *
 ******************************************************************************/
#ifndef BTA_SDP_INT_H
#define BTA_SDP_INT_H

#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta/bta_sdp_api.h"

#if (SDP_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

enum {
    /* these events are handled by the state machine */
    BTA_SDP_API_ENABLE_EVT = BTA_SYS_EVT_START(BTA_ID_SDP),
    BTA_SDP_API_SEARCH_EVT,
    BTA_SDP_API_CREATE_RECORD_USER_EVT,
    BTA_SDP_API_REMOVE_RECORD_USER_EVT,
    BTA_SDP_MAX_INT_EVT
};

enum {
    BTA_SDP_ACTIVE_NONE = 0,
    BTA_SDP_ACTIVE_YES       /* waiting for SDP result */
};



/* data type for BTA_SDP_API_ENABLE_EVT */
typedef struct {
    BT_HDR             hdr;
    tBTA_SDP_DM_CBACK  *p_cback;
} tBTA_SDP_API_ENABLE;

/* data type for BTA_SDP_API_SEARCH_EVT */
typedef struct {
    BT_HDR          hdr;
    BD_ADDR         bd_addr;
    tSDP_UUID       uuid;
} tBTA_SDP_API_SEARCH;

/* data type for BTA_SDP_API_SEARCH_EVT */
typedef struct {
    BT_HDR          hdr;
    void           *user_data;
} tBTA_SDP_API_RECORD_USER;

/* union of all data types */
typedef union {
    /* event buffer header */
    BT_HDR                                 hdr;
    tBTA_SDP_API_ENABLE                    enable;
    tBTA_SDP_API_SEARCH                    get_search;
    tBTA_SDP_API_RECORD_USER               record;
} tBTA_SDP_MSG;

/* SDP control block */
typedef struct {
    UINT8              sdp_active;  /* see BTA_SDP_SDP_ACT_* */
    BD_ADDR            remote_addr;
    tBTA_SDP_DM_CBACK  *p_dm_cback;
} tBTA_SDP_CB;


/* SDP control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_SDP_CB bta_sdp_cb;
#else
extern tBTA_SDP_CB *bta_sdp_cb_ptr;
#define bta_sdp_cb (*bta_sdp_cb_ptr)
#endif

/* config struct */
extern tBTA_SDP_CFG *p_bta_sdp_cfg;

extern BOOLEAN bta_sdp_sm_execute(BT_HDR *p_msg);

extern void bta_sdp_enable (tBTA_SDP_MSG *p_data);
extern void bta_sdp_search (tBTA_SDP_MSG *p_data);
extern void bta_sdp_create_record(tBTA_SDP_MSG *p_data);
extern void bta_sdp_remove_record(tBTA_SDP_MSG *p_data);

#endif  ///SDP_INCLUDED == TRUE

#endif /* BTA_SDP_INT_H */
