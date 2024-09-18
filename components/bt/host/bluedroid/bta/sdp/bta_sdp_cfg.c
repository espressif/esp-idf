/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
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
 *  This file contains compile-time configurable constants for SDP Search
 ******************************************************************************/

#include "common/bt_target.h"
#include "bta/bta_api.h"
#include "bta/bta_sdp_api.h"

#if defined(BTA_SDP_INCLUDED) && (BTA_SDP_INCLUDED == TRUE)

#ifndef BTA_SDP_DB_SIZE
#define BTA_SDP_DB_SIZE  1500
#endif

#ifndef BTA_SDP_RAW_DATA_SIZE
#define BTA_SDP_RAW_DATA_SIZE 1024
#endif

#if BTA_DYNAMIC_MEMORY == FALSE
static UINT8 bta_sdp_raw_data[BTA_SDP_RAW_DATA_SIZE];
static UINT8 __attribute__ ((aligned(4))) bta_sdp_db_data[BTA_SDP_DB_SIZE];
#endif

/* SDP configuration structure */
tBTA_SDP_CFG bta_sdp_cfg = {
    BTA_SDP_RAW_DATA_SIZE,
    BTA_SDP_DB_SIZE,
#if BTA_DYNAMIC_MEMORY == FALSE
    bta_sdp_raw_data,
    (tSDP_DISCOVERY_DB *)bta_sdp_db_data /* The data buffer to keep SDP database */
#else
    NULL,
    NULL
#endif
};

tBTA_SDP_CFG *p_bta_sdp_cfg = (tBTA_SDP_CFG *) &bta_sdp_cfg;

#endif /* #if defined(BTA_SDP_INCLUDED) && (BTA_SDP_INCLUDED == TRUE) */
