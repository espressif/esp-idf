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
 *  This file contains compile-time configurable constants for advanced
 *  audio/video
 *
 ******************************************************************************/

#include <stddef.h>

#include "common/bt_target.h"
#include "bta/bta_api.h"
#include "bta_av_int.h"

#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)

#ifndef BTA_AV_RC_PASS_RSP_CODE
#define BTA_AV_RC_PASS_RSP_CODE     BTA_AV_RSP_NOT_IMPL
#endif

const UINT32  bta_av_meta_caps_co_ids[] = {
    AVRC_CO_METADATA,
    AVRC_CO_BROADCOM
};

/* AVRCP supported categories */
#define BTA_AV_RC_SNK_SUPF_CT       (AVRC_SUPF_CT_CAT1)
#define BTA_AV_RC_SRC_SUPF_CT       (AVRC_SUPF_CT_CAT2)


/* Added to modify
**  1. flush timeout
**  2. Remove Group navigation support in SupportedFeatures
**  3. GetCapabilities supported event_ids list
**  4. GetCapabilities supported event_ids count
*/
/* Flushing partial avdtp packets can cause some headsets to disconnect the link
   if receiving partial a2dp frames */
const UINT16  bta_av_audio_flush_to[] = {
    0, /* 1 stream */
    0, /* 2 streams */
    0, /* 3 streams */
    0, /* 4 streams */
    0  /* 5 streams */
};     /* AVDTP audio transport channel flush timeout */

/* Note: Android doesnt support AVRC_SUPF_TG_GROUP_NAVI  */
/* Note: if AVRC_SUPF_TG_GROUP_NAVI is set, bta_av_cfg.avrc_group should be TRUE */
#if AVRC_METADATA_INCLUDED == TRUE
#define BTA_AV_RC_SNK_SUPF_TG       (AVRC_SUPF_TG_CAT2) /* TODO: | AVRC_SUPF_TG_APP_SETTINGS) */
#define BTA_AV_RC_SRC_SUPF_TG       (AVRC_SUPF_TG_CAT1) /* TODO: | AVRC_SUPF_TG_APP_SETTINGS) */
#else
#define BTA_AV_RC_SNK_SUPF_TG       (AVRC_SUPF_TG_CAT2)
#define BTA_AV_RC_SRC_SUPF_TG       (AVRC_SUPF_TG_CAT1)
#endif

/* the MTU for the AVRCP browsing channel */
#ifndef BTA_AV_MAX_RC_BR_MTU
#define BTA_AV_MAX_RC_BR_MTU      1008
#endif

const tBTA_AV_CFG bta_av_cfg = {
    AVRC_CO_BROADCOM,       /* AVRCP Company ID */
#if AVRC_METADATA_INCLUDED == TRUE
    512,                    /* AVRCP MTU at L2CAP for control channel */
#else
    48,                     /* AVRCP MTU at L2CAP for control channel */
#endif
    BTA_AV_MAX_RC_BR_MTU,   /* AVRCP MTU at L2CAP for browsing channel */
    BTA_AV_RC_SNK_SUPF_CT,  /* AVRCP controller categories as SNK */
    BTA_AV_RC_SNK_SUPF_TG,  /* AVRCP target categories as SNK */
    BTA_AV_RC_SRC_SUPF_CT,  /* AVRCP controller categories as SRC */
    BTA_AV_RC_SRC_SUPF_TG,  /* AVRCP target categories as SRC */
    672,                    /* AVDTP signaling channel MTU at L2CAP */
    BTA_AV_MAX_A2DP_MTU,    /* AVDTP audio transport channel MTU at L2CAP */
    bta_av_audio_flush_to,  /* AVDTP audio transport channel flush timeout */
    6,                      /* AVDTP audio channel max data queue size */
    BTA_AV_MAX_VDP_MTU,     /* AVDTP video transport channel MTU at L2CAP */
    600,                    /* AVDTP video transport channel flush timeout */
    FALSE,                   /* TRUE, to accept AVRC 1.3 group nevigation command */
    2,                      /* company id count in p_meta_co_ids */
    BTA_AV_RC_PASS_RSP_CODE,/* the default response code for pass through commands */
    bta_av_meta_caps_co_ids,/* the metadata Get Capabilities response for company id */
    NULL,                   /* the action function table for VDP stream */
    NULL,                   /* action function to register VDP */
    {0},                    /* Default AVRCP controller name */
    {0},                    /* Default AVRCP target name */
};

tBTA_AV_CFG *p_bta_av_cfg = (tBTA_AV_CFG *) &bta_av_cfg;

#endif /* if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE) */
