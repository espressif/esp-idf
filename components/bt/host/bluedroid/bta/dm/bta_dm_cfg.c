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
 *  This file contains compile-time configurable constants for the device
 *  manager.
 *
 ******************************************************************************/

#include <stddef.h>
#include "common/bt_target.h"
#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta_dm_int.h"
#include "bta/bta_jv_api.h"
#include "bta/bta_gap_bt_co.h"

#ifndef BTA_DM_LINK_POLICY_SETTINGS
#define BTA_DM_LINK_POLICY_SETTINGS    (HCI_ENABLE_MASTER_SLAVE_SWITCH | HCI_ENABLE_HOLD_MODE | HCI_ENABLE_SNIFF_MODE | HCI_ENABLE_PARK_MODE)
#endif

/* page timeout in 625uS */
#ifndef BTA_DM_PAGE_TIMEOUT
#define BTA_DM_PAGE_TIMEOUT    8192
#endif

/* link supervision timeout in 625uS (5 secs) */
#ifndef BTA_DM_LINK_TIMEOUT
#define BTA_DM_LINK_TIMEOUT    8000
#endif

/* TRUE to avoid scatternet when av is streaming (be the master) */
#ifndef BTA_DM_AVOID_SCATTER_A2DP
#define BTA_DM_AVOID_SCATTER_A2DP    TRUE
#endif

/* For Insight, PM cfg lookup tables are runtime configurable (to allow tweaking of params for power consumption measurements) */
#ifndef BTE_SIM_APP
#define tBTA_DM_PM_TYPE_QUALIFIER   const
#else
#define tBTA_DM_PM_TYPE_QUALIFIER
#endif


const tBTA_DM_CFG bta_dm_cfg = {
    /* mobile phone COD */
    BTA_DM_COD,
    /* link policy settings */
    BTA_DM_LINK_POLICY_SETTINGS,
    /* page timeout in 625uS */
    BTA_DM_PAGE_TIMEOUT,
    /* link supervision timeout in 625uS*/
    BTA_DM_LINK_TIMEOUT,
    /* TRUE to avoid scatternet when av is streaming (be the master) */
    BTA_DM_AVOID_SCATTER_A2DP
};

#ifndef BTA_DM_SCATTERNET
/* By default, allow partial scatternet */
#define BTA_DM_SCATTERNET BTA_DM_PARTIAL_SCATTERNET
#endif

#ifndef BTA_HH_ROLE
/* By default, do not specify HH role (backward compatibility) */
#define BTA_HH_ROLE BTA_ANY_ROLE
#endif

#ifndef BTA_AV_ROLE
/* By default, AV role (backward BTA_MASTER_ROLE_PREF) */
#define BTA_AV_ROLE BTA_MASTER_ROLE_PREF
#endif

#ifndef BTA_PANU_ROLE
/* By default, AV role (backward BTA_MASTER_ROLE_PREF) */
#define BTA_PANU_ROLE BTA_SLAVE_ROLE_ONLY
#endif
#define BTA_DM_NUM_RM_ENTRY    6

/* appids for PAN used by insight sample application
   these have to be same as defined in btui_int.h */
#define BTUI_PAN_ID_PANU         0
#define BTUI_PAN_ID_NAP          1
#define BTUI_PAN_ID_GN           2

/* First element is always for SYS:
   app_id = # of entries table, cfg is
   device scatternet support */
const tBTA_DM_RM bta_dm_rm_cfg[] = {
    {BTA_ID_SYS, BTA_DM_NUM_RM_ENTRY, BTA_DM_SCATTERNET},
    {BTA_ID_PAN, BTUI_PAN_ID_NAP, BTA_ANY_ROLE},
    {BTA_ID_PAN, BTUI_PAN_ID_GN, BTA_ANY_ROLE},
    {BTA_ID_PAN, BTA_APP_ID_PAN_MULTI, BTA_MASTER_ROLE_ONLY},
    {BTA_ID_PAN, BTUI_PAN_ID_PANU, BTA_PANU_ROLE},
    {BTA_ID_HH,  BTA_ALL_APP_ID, BTA_HH_ROLE},
    {BTA_ID_AV,  BTA_ALL_APP_ID, BTA_AV_ROLE}
};


tBTA_DM_CFG *const p_bta_dm_cfg = (tBTA_DM_CFG *) &bta_dm_cfg;

tBTA_DM_RM *const p_bta_dm_rm_cfg = (tBTA_DM_RM *) &bta_dm_rm_cfg;

#if BLE_INCLUDED == TRUE
#  define BTA_DM_NUM_PM_ENTRY         10  /* number of entries in bta_dm_pm_cfg except the first */
#  define BTA_DM_NUM_PM_SPEC          10  /* number of entries in bta_dm_pm_spec */
#else
#  define BTA_DM_NUM_PM_ENTRY         8  /* number of entries in bta_dm_pm_cfg except the first */
#  define BTA_DM_NUM_PM_SPEC          8  /* number of entries in bta_dm_pm_spec */
#endif

#if (BTA_DM_PM_INCLUDED == TRUE)

tBTA_DM_PM_TYPE_QUALIFIER tBTA_DM_PM_CFG bta_dm_pm_cfg[BTA_DM_NUM_PM_ENTRY + 1] = {
    {BTA_ID_SYS, BTA_DM_NUM_PM_ENTRY, 0},  /* reserved: specifies length of this table. */
    {BTA_ID_AG,  BTA_ALL_APP_ID,      0},  /* ag uses first spec table for app id 0 */
    {BTA_ID_AV,  BTA_ALL_APP_ID,      1},  /* av spec table */
    {BTA_ID_JV,  BTA_APP_ID_1,        2},  /* app BTA_JV_PM_ID_1, reuse ftc spec table */
    {BTA_ID_JV,  BTA_ALL_APP_ID,      3},  /* reuse fts spec table */
    {BTA_ID_HS,  BTA_ALL_APP_ID,      4},  /* HS spec table */
    {BTA_ID_AVK, BTA_ALL_APP_ID,      5},  /* avk spec table */
    {BTA_ID_HD,  BTA_ALL_APP_ID,      6},  /* hd spec table */
    {BTA_ID_HH,  BTA_ALL_APP_ID,      7}   /* hh spec table */
#if BLE_INCLUDED == TRUE
    , {BTA_ID_GATTC,  BTA_ALL_APP_ID,   8} /* gattc spec table */
    , {BTA_ID_GATTS,  BTA_ALL_APP_ID,   9} /* gatts spec table */
#endif
};

#define BTA_DM_PM_SPEC_TO_OFFSET     (197) /* timeout offset to avoid conflict with other bluedroid host */
tBTA_DM_PM_TYPE_QUALIFIER tBTA_DM_PM_SPEC bta_dm_pm_spec[BTA_DM_NUM_PM_SPEC] = {
    /* AG : 0 */
    {
        (BTA_DM_PM_SNIFF | BTA_DM_PM_PARK),                           /* allow park & sniff */
#if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR2),                                              /* the SSR entry */
#endif
        {
            {{BTA_DM_PM_SNIFF_AG_OPEN_IDX,  7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* conn open sniff  */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},   /* conn close  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},   /* app open */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},   /* app close */
            {{BTA_DM_PM_SNIFF_SCO_OPEN_IDX, 7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* sco open, active */
            {{BTA_DM_PM_SNIFF_SCO_CLOSE_IDX,  7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* sco close sniff  */
            {{BTA_DM_PM_SNIFF_AG_IDLE_IDX,  7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* idle */
            {{BTA_DM_PM_ACTIVE,    0},   {BTA_DM_PM_NO_ACTION, 0}},   /* busy */
            {{BTA_DM_PM_RETRY,  7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}}    /* mode change retry */
        }
    },

    /* AV : 1 */
    {
        (BTA_DM_PM_SNIFF),                                             /* allow sniff */
#if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR2),                                              /* the SSR entry */
#endif
        {
            {{BTA_DM_PM_SNIFF_A2DP_IDX, 7000 + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}}, /* conn open  sniff */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn close  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app open */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app close */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco open  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco close   */
            {{BTA_DM_PM_SNIFF_A2DP_IDX, 7000 + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}}, /* idle */
            {{BTA_DM_PM_ACTIVE,    0},   {BTA_DM_PM_NO_ACTION, 0}},    /* busy */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}}     /* mode change retry */
        }
    },

    /* FTC, OPC, JV : 2 */
    {
        (BTA_DM_PM_SNIFF),                                             /* allow sniff */
  #if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR2),                                              /* the SSR entry */
  #endif
        {
            {{BTA_DM_PM_SNIFF_JV_IDX, BTA_FTC_OPS_IDLE_TO_SNIFF_DELAY_MS + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}},    /* conn open  active */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn close  */
            {{BTA_DM_PM_ACTIVE,    0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app open */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app close */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco open  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco close   */
            {{BTA_DM_PM_SNIFF_JV_IDX, BTA_FTC_OPS_IDLE_TO_SNIFF_DELAY_MS + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}},    /* idle */
            {{BTA_DM_PM_ACTIVE,    0},   {BTA_DM_PM_NO_ACTION, 0}},    /* busy */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}}     /* mode change retry */
        }
    },

    /* FTS, PBS, OPS, MSE, BTA_JV_PM_ID_1 : 3 */
    {
        (BTA_DM_PM_SNIFF),                                             /* allow sniff */
  #if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR2),                                              /* the SSR entry */
  #endif
        {
            {{BTA_DM_PM_SNIFF_JV_IDX, BTA_FTS_OPS_IDLE_TO_SNIFF_DELAY_MS + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}},    /* conn open  active */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn close  */
            {{BTA_DM_PM_ACTIVE,    0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app open */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app close */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco open  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco close   */
            {{BTA_DM_PM_SNIFF_JV_IDX, BTA_FTS_OPS_IDLE_TO_SNIFF_DELAY_MS + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}}, /* idle */
            {{BTA_DM_PM_ACTIVE,    0},   {BTA_DM_PM_NO_ACTION, 0}},    /* busy */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}}     /* mode change retry */
        }
    },

    /* HS : 4 */
    {
        (BTA_DM_PM_SNIFF | BTA_DM_PM_PARK),                           /* allow park & sniff */
#if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR2),                                              /* the SSR entry */
#endif
        {
            {{BTA_DM_PM_SNIFF,  7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* conn open sniff  */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},   /* conn close  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},   /* app open */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},   /* app close */
            {{BTA_DM_PM_SNIFF3, 7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* sco open, active */
            {{BTA_DM_PM_SNIFF,  7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* sco close sniff  */
            {{BTA_DM_PM_SNIFF,  7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* idle */
            {{BTA_DM_PM_ACTIVE,    0},   {BTA_DM_PM_NO_ACTION, 0}},   /* busy */
            {{BTA_DM_PM_RETRY,  7000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}}    /* mode change retry */
        }
    },

    /* AVK : 5 */
    {
        (BTA_DM_PM_SNIFF),                                             /* allow sniff */
#if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR2),                                              /* the SSR entry */
#endif
        {
            {{BTA_DM_PM_SNIFF,  3000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn open  sniff */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn close  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app open */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app close */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco open  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco close   */
            {{BTA_DM_PM_SNIFF_AVK_IDLE_IDX, 3000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},    /* idle */
            {{BTA_DM_PM_ACTIVE,    0},   {BTA_DM_PM_NO_ACTION, 0}},    /* busy */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}}     /* mode change retry */
        }
    },

    /* HD : 6 */
    {
        (BTA_DM_PM_SNIFF | BTA_DM_PM_PARK),                            /* allow park & sniff */
#if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR3),                                              /* the SSR entry */
#endif
        {
            {{BTA_DM_PM_SNIFF_HD_ACTIVE_IDX, 5000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* conn open sniff */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn close */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app open */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app close */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco open  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco close */
            {{BTA_DM_PM_SNIFF_HD_IDLE_IDX,   5000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}},   /* idle */
            {{BTA_DM_PM_SNIFF_HD_ACTIVE_IDX, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* busy */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}}     /* mode change retry */
        }
    },

    /* HH : 7 */
    {
        (BTA_DM_PM_SNIFF | BTA_DM_PM_PARK),                            /* allow park & sniff */
#if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR1),                                              /* the SSR entry */
#endif
        {
            {{BTA_DM_PM_SNIFF_HH_OPEN_IDX, BTA_DM_PM_HH_OPEN_DELAY + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}},   /* conn open  sniff */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn close  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app open */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app close */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco open  */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco close, used for HH suspend   */
            {{BTA_DM_PM_SNIFF_HH_IDLE_IDX, BTA_DM_PM_HH_IDLE_DELAY + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}},   /* idle */
            {{BTA_DM_PM_SNIFF_HH_ACTIVE_IDX, BTA_DM_PM_HH_ACTIVE_DELAY + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}},   /* busy */
            {{BTA_DM_PM_NO_ACTION, 0},   {BTA_DM_PM_NO_ACTION, 0}}     /* mode change retry */
        }
    }

#if BLE_INCLUDED == TRUE
    /* GATTC : 8 */
    , {
        (BTA_DM_PM_SNIFF | BTA_DM_PM_PARK),                           /* allow park & sniff */
#if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR2),                                              /* the SSR entry */
#endif
        {
            {{BTA_DM_PM_SNIFF_A2DP_IDX, 10000 + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}}, /* conn open  active */
            {{BTA_DM_PM_NO_PREF,    0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn close  */
            {{BTA_DM_PM_ACTIVE,     0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app open */
            {{BTA_DM_PM_NO_ACTION,  0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app close */
            {{BTA_DM_PM_NO_ACTION,  0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco open  */
            {{BTA_DM_PM_NO_ACTION,  0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco close   */
            {{BTA_DM_PM_SNIFF_A2DP_IDX, 10000 + BTA_DM_PM_SPEC_TO_OFFSET}, {BTA_DM_PM_NO_ACTION, 0}}, /* idle */
            {{BTA_DM_PM_ACTIVE,     0},   {BTA_DM_PM_NO_ACTION, 0}},    /* busy */
#if defined(AMP_INCLUDED) && (AMP_INCLUDED == TRUE)
            {{BTA_DM_PM_NO_ACTION,  0},   {BTA_DM_PM_NO_ACTION, 0}},   /* amp */
#endif
            {{BTA_DM_PM_RETRY,   5000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}}    /* mode change retry */
        }
    }
    /* GATTS : 9 */
    , {
        (BTA_DM_PM_SNIFF | BTA_DM_PM_PARK),                           /* allow park & sniff */
#if (BTM_SSR_INCLUDED == TRUE)
        (BTA_DM_PM_SSR2),                                              /* the SSR entry */
#endif
        {
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn open  active */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* conn close  */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app open */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* app close */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco open  */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* sco close   */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* idle */
            {{BTA_DM_PM_NO_PREF,   0},   {BTA_DM_PM_NO_ACTION, 0}},    /* busy */
#if defined(AMP_INCLUDED) && (AMP_INCLUDED == TRUE)
            {{BTA_DM_PM_NO_PREF, 0},   {BTA_DM_PM_NO_ACTION, 0}},   /* amp */
#endif
            {{BTA_DM_PM_RETRY,  5000 + BTA_DM_PM_SPEC_TO_OFFSET},   {BTA_DM_PM_NO_ACTION, 0}}    /* mode change retry */
        }
    }

#endif

#ifdef BTE_SIM_APP      /* For Insight builds only */
    /* Entries at the end of the pm_spec table are user-defined (runtime configurable),
       for power consumption experiments.
       Insight finds the first user-defined entry by looking for the first BTA_DM_PM_NO_PREF.
       The number of user_defined specs is defined by BTA_SWRAP_UD_PM_SPEC_COUNT */
    ,
    {BTA_DM_PM_NO_PREF},               /* pm_spec USER_DEFINED_0 */
    {BTA_DM_PM_NO_PREF}                /* pm_spec USER_DEFINED_1 */
#endif  /* BTE_SIM_APP */
};

/* Please refer to the SNIFF table definitions in bta/bta_api.h.
 *
 * Adding to or Modifying the Table
 * Additional sniff parameter entries can be added for BTA_DM_PM_SNIFF5 - BTA_DM_PM_SNIFF7.
 * Overrides of additional table entries can be specified in bdroid_buildcfg.h.  If additional
 * sniff parameter entries are added or an override of an existing entry is specified in
 * bdroid_buildcfg.h then the BTA_DM_PM_*_IDX defines in bta/bta_api.h will need to be match the new
 * ordering.
 *
 * Table Ordering
 * Sniff Table entries must be ordered from highest latency (biggest interval) to lowest latency.
 * If there is a conflict among the connected services the setting with the lowest latency will
 * be selected.
 */
tBTA_DM_PM_TYPE_QUALIFIER tBTM_PM_PWR_MD bta_dm_pm_md[] = {
    /*
     * More sniff parameter entries can be added for
     * BTA_DM_PM_SNIFF3 - BTA_DM_PM_SNIFF7, if needed. When entries are added or
     * removed, BTA_DM_PM_PARK_IDX needs to be updated to reflect the actual index
     * BTA_DM_PM_PARK_IDX is defined in bta/bta_api.h and can be override by the
     * bdroid_buildcfg.h settings.
     * The SNIFF table entries must be in the order from highest latency (biggest
     * interval) to lowest latency. If there's a conflict among the connected
     * services, the setting with lowest latency wins.
     */
    /* sniff modes: max interval, min interval, attempt, timeout */
    {BTA_DM_PM_SNIFF_MAX, BTA_DM_PM_SNIFF_MIN, BTA_DM_PM_SNIFF_ATTEMPT, BTA_DM_PM_SNIFF_TIMEOUT, BTM_PM_MD_SNIFF}, /* for BTA_DM_PM_SNIFF - A2DP */
    {BTA_DM_PM_SNIFF1_MAX, BTA_DM_PM_SNIFF1_MIN, BTA_DM_PM_SNIFF1_ATTEMPT, BTA_DM_PM_SNIFF1_TIMEOUT, BTM_PM_MD_SNIFF}, /* for BTA_DM_PM_SNIFF1 */
    {BTA_DM_PM_SNIFF2_MAX, BTA_DM_PM_SNIFF2_MIN, BTA_DM_PM_SNIFF2_ATTEMPT, BTA_DM_PM_SNIFF2_TIMEOUT, BTM_PM_MD_SNIFF}, /* for BTA_DM_PM_SNIFF2 */
    {BTA_DM_PM_SNIFF3_MAX, BTA_DM_PM_SNIFF3_MIN, BTA_DM_PM_SNIFF3_ATTEMPT, BTA_DM_PM_SNIFF3_TIMEOUT, BTM_PM_MD_SNIFF}, /* for BTA_DM_PM_SNIFF3- SCO open */
    {BTA_DM_PM_SNIFF4_MAX, BTA_DM_PM_SNIFF4_MIN, BTA_DM_PM_SNIFF4_ATTEMPT, BTA_DM_PM_SNIFF4_TIMEOUT, BTM_PM_MD_SNIFF}, /* for BTA_DM_PM_SNIFF4 */
    {BTA_DM_PM_SNIFF5_MAX, BTA_DM_PM_SNIFF5_MIN, BTA_DM_PM_SNIFF5_ATTEMPT, BTA_DM_PM_SNIFF5_TIMEOUT, BTM_PM_MD_SNIFF}, /* for BTA_DM_PM_SNIFF5 */
    {BTA_DM_PM_PARK_MAX, BTA_DM_PM_PARK_MIN, BTA_DM_PM_PARK_ATTEMPT, BTA_DM_PM_PARK_TIMEOUT, BTM_PM_MD_PARK}

#ifdef BTE_SIM_APP      /* For Insight builds only */
    /* Entries at the end of the bta_dm_pm_md table are user-defined (runtime configurable),
       for power consumption experiments.
       Insight finds the first user-defined entry by looking for the first 'max=0'.
       The number of user_defined specs is defined by BTA_SWRAP_UD_PM_DM_COUNT */
    ,
    {0},           /* CONN_OPEN/SCO_CLOSE power mode settings for pm_spec USER_DEFINED_0 */
    {0},           /* SCO_OPEN power mode settings for pm_spec USER_DEFINED_0 */

    {0},           /* CONN_OPEN/SCO_CLOSE power mode settings for pm_spec USER_DEFINED_1 */
    {0}            /* SCO_OPEN power mode settings for pm_spec USER_DEFINED_1 */
#endif  /* BTE_SIM_APP */
};

/* 0=max_lat -> no SSR */
/* the smaller of the SSR max latency wins.
 * the entries in this table must be from highest latency (biggest interval) to lowest latency */
#if (BTM_SSR_INCLUDED == TRUE)
tBTA_DM_SSR_SPEC bta_dm_ssr_spec[] = {
    /*max_lat, min_rmt_to, min_loc_to*/
    {0,      0, 0},     /* BTA_DM_PM_SSR0 - do not use SSR */
    {0,      0, 2},     /* BTA_DM_PM_SSR1 - HH, can NOT share entry with any other profile,
                           setting default max latency and min remote timeout as 0,
                           and always read individual device preference from HH module */
    {1200,   2, 2},     /* BTA_DM_PM_SSR2 - others (as long as sniff is allowed)*/
    {360,  160, 1600}   /* BTA_DM_PM_SSR3 - HD */
};

tBTA_DM_SSR_SPEC *const p_bta_dm_ssr_spec = (tBTA_DM_SSR_SPEC *) &bta_dm_ssr_spec;
#endif

tBTA_DM_PM_CFG *const p_bta_dm_pm_cfg = (tBTA_DM_PM_CFG *) &bta_dm_pm_cfg;
tBTA_DM_PM_SPEC *const p_bta_dm_pm_spec = (tBTA_DM_PM_SPEC *) &bta_dm_pm_spec;
tBTM_PM_PWR_MD *const p_bta_dm_pm_md = (tBTM_PM_PWR_MD *) &bta_dm_pm_md;

#endif /* #if (BTA_DM_PM_INCLUDED == TRUE) */

/* The performance impact of EIR packet size
**
** When BTM_EIR_DEFAULT_FEC_REQUIRED is TRUE,
** 1 to 17 bytes,    DM1 is used and most robust.
** 18 to 121 bytes,  DM3 is used but impacts inquiry scan time with large number
**                    of devices.(almost double with 150 users)
** 122 to 224 bytes, DM5 is used but cause quite big performance loss even with
**                    small number of users. so it is not recommended.
** 225 to 240 bytes, DH5 is used without FEC but it not recommended.
**                    (same reason of DM5)
**
** When BTM_EIR_DEFAULT_FEC_REQUIRED is FALSE,
** 1 to 27 bytes,    DH1 is used but only robust at short range.
** 28 to 183 bytes,  DH3 is used but only robust at short range and impacts inquiry
**                    scan time with large number of devices.
** 184 to 240 bytes, DH5 is used but it not recommended.
*/

#if (BTA_EIR_CANNED_UUID_LIST == TRUE)
/* for example */
const UINT8 bta_dm_eir_uuid16_list[] = {    0x08, 0x11, /* Headset */
                                            0x1E, 0x11, /* Handsfree */
                                            0x0E, 0x11, /* AV Remote Control */
                                            0x0B, 0x11, /* Audio Sink */
                                       };
#endif  // BTA_EIR_CANNED_UUID_LIST

/* Extended Inquiry Response */
tBTA_DM_EIR_CONF bta_dm_eir_cfg = {
    BTM_EIR_DEFAULT_FEC_REQUIRED, /* FEC required */
    TRUE,  /* Included local name */
    50,    /* minimum length of local name when it is shortened */
    /* if length of local name is longer than this and EIR has not enough */
    /* room for all UUID list then local name is shortened to this length */
    TRUE, /* Included UUIDs */
#if (BTA_EIR_CANNED_UUID_LIST == TRUE)
    8,
    (UINT8 *)bta_dm_eir_uuid16_list,
#else // BTA_EIR_CANNED_UUID_LIST
    {   /* mask of UUID list in EIR */
        0xFFFFFFFF, /* LSB is the first UUID of the first 32 UUIDs in BTM_EIR_UUID_LKUP_TBL */
        0xFFFFFFFF  /* LSB is the first UUID of the next 32 UUIDs in BTM_EIR_UUID_LKUP_TBL */
        /* BTM_EIR_UUID_LKUP_TBL can be overridden */
    },
#endif  // BTA_EIR_CANNED_UUID_LIST
    FALSE,  /* Not included TX power*/
    3,      /* Inquiry TX power         */
    0,      /* flags for EIR */
    0,      /* length of manufacturer specific in bytes */
    NULL,   /* manufacturer specific */
    0,      /* length of URL in bytes */
    NULL,   /* URL */
#if (BTC_GAP_BT_INCLUDED == TRUE)
    (tBTA_DM_CONFIG_EIR_CBACK *)btc_gap_bt_config_eir_cmpl_callback    /* callback */
#else
    NULL
#endif /* #if (BTC_GAP_BT_INCLUDED == TRUE) */
};
tBTA_DM_EIR_CONF *p_bta_dm_eir_cfg = (tBTA_DM_EIR_CONF *) &bta_dm_eir_cfg;
