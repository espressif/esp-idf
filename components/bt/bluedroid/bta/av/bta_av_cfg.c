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

#include "bt_target.h"
#include "bta_api.h"
#include "bta_av_int.h"

#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)

#ifndef BTA_AV_RC_PASS_RSP_CODE
#define BTA_AV_RC_PASS_RSP_CODE     BTA_AV_RSP_NOT_IMPL
#endif

const UINT32  bta_av_meta_caps_co_ids[] = {
    AVRC_CO_METADATA,
    AVRC_CO_BROADCOM
};

/* AVRCP cupported categories */
#define BTA_AV_RC_SUPF_CT       (AVRC_SUPF_CT_CAT2)

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
#define BTA_AV_RC_SUPF_TG       (AVRC_SUPF_TG_CAT1) /* TODO: | AVRC_SUPF_TG_APP_SETTINGS) */
#else
#define BTA_AV_RC_SUPF_TG       (AVRC_SUPF_TG_CAT1)
#endif

/*
 * If the number of event IDs is changed in this array, BTA_AV_ NUM_RC_EVT_IDS   also needs to be changed.
 */
const UINT8  bta_av_meta_caps_evt_ids[] = {
    AVRC_EVT_PLAY_STATUS_CHANGE,
    AVRC_EVT_TRACK_CHANGE,
    AVRC_EVT_PLAY_POS_CHANGED,
    /* TODO: Add support for these events
    AVRC_EVT_APP_SETTING_CHANGE,
    */
};
#ifndef BTA_AV_NUM_RC_EVT_IDS
#define BTA_AV_NUM_RC_EVT_IDS   (sizeof(bta_av_meta_caps_evt_ids) / sizeof(bta_av_meta_caps_evt_ids[0]))
#endif /* BTA_AV_NUM_RC_EVT_IDS */

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
    BTA_AV_RC_SUPF_CT,      /* AVRCP controller categories */
    BTA_AV_RC_SUPF_TG,      /* AVRCP target categories */
    672,                    /* AVDTP signaling channel MTU at L2CAP */
    BTA_AV_MAX_A2DP_MTU,    /* AVDTP audio transport channel MTU at L2CAP */
    bta_av_audio_flush_to,  /* AVDTP audio transport channel flush timeout */
    6,                      /* AVDTP audio channel max data queue size */
    BTA_AV_MAX_VDP_MTU,     /* AVDTP video transport channel MTU at L2CAP */
    600,                    /* AVDTP video transport channel flush timeout */
    FALSE,                   /* TRUE, to accept AVRC 1.3 group nevigation command */
    2,                      /* company id count in p_meta_co_ids */
    BTA_AV_NUM_RC_EVT_IDS, /* event id count in p_meta_evt_ids */
    BTA_AV_RC_PASS_RSP_CODE,/* the default response code for pass through commands */
    bta_av_meta_caps_co_ids,/* the metadata Get Capabilities response for company id */
    bta_av_meta_caps_evt_ids,/* the the metadata Get Capabilities response for event id */
    NULL,                   /* the action function table for VDP stream */
    NULL,                   /* action function to register VDP */
    {0},                    /* Default AVRCP controller name */
    {0},                    /* Default AVRCP target name */
};

tBTA_AV_CFG *p_bta_av_cfg = (tBTA_AV_CFG *) &bta_av_cfg;

const UINT16 bta_av_rc_id[] = {
    0x0000, /* bit mask: 0=SELECT, 1=UP, 2=DOWN, 3=LEFT,
                         4=RIGHT, 5=RIGHT_UP, 6=RIGHT_DOWN, 7=LEFT_UP,
                         8=LEFT_DOWN, 9=ROOT_MENU, 10=SETUP_MENU, 11=CONT_MENU,
                         12=FAV_MENU, 13=EXIT */

    0,      /* not used */

    0x0000, /* bit mask: 0=0, 1=1, 2=2, 3=3,
                         4=4, 5=5, 6=6, 7=7,
                         8=8, 9=9, 10=DOT, 11=ENTER,
                         12=CLEAR */

    0x0000, /* bit mask: 0=CHAN_UP, 1=CHAN_DOWN, 2=PREV_CHAN, 3=SOUND_SEL,
                         4=INPUT_SEL, 5=DISP_INFO, 6=HELP, 7=PAGE_UP,
                         8=PAGE_DOWN */

#if (BTA_AV_RC_PASS_RSP_CODE == BTA_AV_RSP_INTERIM)
    /* btui_app provides an example of how to leave the decision of rejecting a command or not
     * based on which media player is currently addressed (this is only applicable for AVRCP 1.4 or later)
     * If the decision is per player for a particular rc_id, the related bit is clear (not set) */
    0x0070, /* bit mask: 0=POWER, 1=VOL_UP, 2=VOL_DOWN, 3=MUTE,
                         4=PLAY, 5=STOP, 6=PAUSE, 7=RECORD,
                         8=REWIND, 9=FAST_FOR, 10=EJECT, 11=FORWARD,
                         12=BACKWARD */
#else
#if (defined BTA_AVRCP_FF_RW_SUPPORT) && (BTA_AVRCP_FF_RW_SUPPORT == TRUE)
    0x1b70, /* bit mask: 0=POWER, 1=VOL_UP, 2=VOL_DOWN, 3=MUTE,
                         4=PLAY, 5=STOP, 6=PAUSE, 7=RECORD,
                         8=REWIND, 9=FAST_FOR, 10=EJECT, 11=FORWARD,
                         12=BACKWARD */
#else
    0x1870, /* bit mask: 0=POWER, 1=VOL_UP, 2=VOL_DOWN, 3=MUTE,
                         4=PLAY, 5=STOP, 6=PAUSE, 7=RECORD,
                         8=REWIND, 9=FAST_FOR, 10=EJECT, 11=FORWARD,
                         12=BACKWARD */
#endif
#endif

    0x0000, /* bit mask: 0=ANGLE, 1=SUBPICT */

    0,      /* not used */

    0x0000  /* bit mask: 0=not used, 1=F1, 2=F2, 3=F3,
                         4=F4, 5=F5 */
};

#if (BTA_AV_RC_PASS_RSP_CODE == BTA_AV_RSP_INTERIM)
const UINT16 bta_av_rc_id_ac[] = {
    0x0000, /* bit mask: 0=SELECT, 1=UP, 2=DOWN, 3=LEFT,
                         4=RIGHT, 5=RIGHT_UP, 6=RIGHT_DOWN, 7=LEFT_UP,
                         8=LEFT_DOWN, 9=ROOT_MENU, 10=SETUP_MENU, 11=CONT_MENU,
                         12=FAV_MENU, 13=EXIT */

    0,      /* not used */

    0x0000, /* bit mask: 0=0, 1=1, 2=2, 3=3,
                         4=4, 5=5, 6=6, 7=7,
                         8=8, 9=9, 10=DOT, 11=ENTER,
                         12=CLEAR */

    0x0000, /* bit mask: 0=CHAN_UP, 1=CHAN_DOWN, 2=PREV_CHAN, 3=SOUND_SEL,
                         4=INPUT_SEL, 5=DISP_INFO, 6=HELP, 7=PAGE_UP,
                         8=PAGE_DOWN */

    /* btui_app provides an example of how to leave the decision of rejecting a command or not
     * based on which media player is currently addressed (this is only applicable for AVRCP 1.4 or later)
     * If the decision is per player for a particular rc_id, the related bit is set */
    0x1800, /* bit mask: 0=POWER, 1=VOL_UP, 2=VOL_DOWN, 3=MUTE,
                         4=PLAY, 5=STOP, 6=PAUSE, 7=RECORD,
                         8=REWIND, 9=FAST_FOR, 10=EJECT, 11=FORWARD,
                         12=BACKWARD */

    0x0000, /* bit mask: 0=ANGLE, 1=SUBPICT */

    0,      /* not used */

    0x0000  /* bit mask: 0=not used, 1=F1, 2=F2, 3=F3,
                         4=F4, 5=F5 */
};
UINT16 *p_bta_av_rc_id_ac = (UINT16 *) bta_av_rc_id_ac;
#else
UINT16 *p_bta_av_rc_id_ac = NULL;
#endif

UINT16 *p_bta_av_rc_id = (UINT16 *) bta_av_rc_id;

#endif /* if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE) */
