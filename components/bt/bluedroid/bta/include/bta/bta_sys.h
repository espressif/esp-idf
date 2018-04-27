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
 *  This is the public interface file for the BTA system manager.
 *
 ******************************************************************************/
#ifndef BTA_SYS_H
#define BTA_SYS_H

#include "common/bt_target.h"
#include "common/bt_defs.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* vendor specific event handler function type */
typedef BOOLEAN (tBTA_SYS_VS_EVT_HDLR)(UINT16 evt, void *p);

/* event handler function type */
typedef BOOLEAN (tBTA_SYS_EVT_HDLR)(BT_HDR *p_msg);

/* disable function type */
typedef void (tBTA_SYS_DISABLE)(void);


/* HW modules */
enum {
    BTA_SYS_HW_BLUETOOTH,
    BTA_SYS_HW_RT,

    BTA_SYS_MAX_HW_MODULES
};

typedef UINT16 tBTA_SYS_HW_MODULE;

#ifndef BTA_DM_NUM_JV_ID
#define BTA_DM_NUM_JV_ID    2
#endif

/* SW sub-systems */
#define BTA_ID_SYS          0            /* system manager */
/* BLUETOOTH PART - from 0 to BTA_ID_BLUETOOTH_MAX */
#define BTA_ID_DM           1            /* device manager */
#define BTA_ID_DM_SEARCH    2            /* device manager search */
#define BTA_ID_DM_SEC       3            /* device manager security */
#define BTA_ID_DG           4            /* data gateway */
#define BTA_ID_AG           5            /* audio gateway */
#define BTA_ID_OPC          6            /* object push client */
#define BTA_ID_OPS          7            /* object push server */
#define BTA_ID_FTS          8            /* file transfer server */
#define BTA_ID_CT           9            /* cordless telephony terminal */
#define BTA_ID_FTC          10           /* file transfer client */
#define BTA_ID_SS           11           /* synchronization server */
#define BTA_ID_PR           12           /* Printer client */
#define BTA_ID_BIC          13           /* Basic Imaging Client */
#define BTA_ID_PAN          14           /* Personal Area Networking */
#define BTA_ID_BIS          15           /* Basic Imaging Server */
#define BTA_ID_ACC          16           /* Advanced Camera Client */
#define BTA_ID_SC           17           /* SIM Card Access server */
#define BTA_ID_AV           18           /* Advanced audio/video */
#define BTA_ID_AVK          19           /* Audio/video sink */
#define BTA_ID_HD           20           /* HID Device */
#define BTA_ID_CG           21           /* Cordless Gateway */
#define BTA_ID_BP           22           /* Basic Printing Client */
#define BTA_ID_HH           23           /* Human Interface Device Host */
#define BTA_ID_PBS          24           /* Phone Book Access Server */
#define BTA_ID_PBC          25           /* Phone Book Access Client */
#define BTA_ID_JV           26           /* Java */
#define BTA_ID_HS           27           /* Headset */
#define BTA_ID_MSE          28           /* Message Server Equipment */
#define BTA_ID_MCE          29           /* Message Client Equipment */
#define BTA_ID_HL           30           /* Health Device Profile*/
#define BTA_ID_GATTC        31           /* GATT Client */
#define BTA_ID_GATTS        32           /* GATT Client */
#define BTA_ID_SDP          33           /* SDP Client */
#define BTA_ID_BLUETOOTH_MAX   34        /* last BT profile */

/* GENERIC */
#define BTA_ID_PRM          38
#define BTA_ID_SYSTEM       39           /* platform-specific */
#define BTA_ID_SWRAP        40           /* Insight script wrapper */
#define BTA_ID_MIP          41           /* Multicase Individual Polling */
#define BTA_ID_RT           42           /* Audio Routing module: This module is always on. */


/* JV */
#define BTA_ID_JV1          44           /* JV1 */
#define BTA_ID_JV2          45           /* JV2 */

#define BTA_ID_MAX          (44 + BTA_DM_NUM_JV_ID)

typedef UINT8 tBTA_SYS_ID;


#define BTA_SYS_CONN_OPEN           0x00
#define BTA_SYS_CONN_CLOSE          0x01
#define BTA_SYS_APP_OPEN            0x02
#define BTA_SYS_APP_CLOSE           0x03
#define BTA_SYS_SCO_OPEN            0x04
#define BTA_SYS_SCO_CLOSE           0x05
#define BTA_SYS_CONN_IDLE           0x06
#define BTA_SYS_CONN_BUSY           0x07

/* for link policy */
#define BTA_SYS_PLCY_SET            0x10 /* set the link policy to the given addr */
#define BTA_SYS_PLCY_CLR            0x11 /* clear the link policy to the given addr */
#define BTA_SYS_PLCY_DEF_SET        0x12 /* set the default link policy */
#define BTA_SYS_PLCY_DEF_CLR        0x13 /* clear the default link policy */
#define BTA_SYS_ROLE_CHANGE         0x14 /* role change */

typedef UINT8 tBTA_SYS_CONN_STATUS;

/* Bitmask of sys features */
#define BTA_SYS_FEAT_PCM2           0x0001
#define BTA_SYS_FEAT_PCM2_MASTER    0x0002

/* tBTA_PREF_ROLES */
typedef UINT8 tBTA_SYS_PREF_ROLES;

/* conn callback for role / low power manager*/
typedef void (tBTA_SYS_CONN_CBACK)(tBTA_SYS_CONN_STATUS status, UINT8 id, UINT8 app_id, BD_ADDR peer_addr);

/* conn callback for role / low power manager*/
typedef void (tBTA_SYS_SSR_CFG_CBACK)(UINT8 id, UINT8 app_id, UINT16 latency, UINT16 tout);

#if (BTA_EIR_CANNED_UUID_LIST != TRUE)
/* eir callback for adding/removeing UUID */
typedef void (tBTA_SYS_EIR_CBACK)(UINT16 uuid16, BOOLEAN adding);
#endif

/* registration structure */
typedef struct {
    tBTA_SYS_EVT_HDLR   *evt_hdlr;
    tBTA_SYS_DISABLE    *disable;
} tBTA_SYS_REG;

/* data type to send events to BTA SYS HW manager */
typedef struct {
    BT_HDR                hdr;
    tBTA_SYS_HW_MODULE   hw_module;
} tBTA_SYS_HW_MSG;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* trace level */
extern UINT8 appl_trace_level;

/*****************************************************************************
**  Macros
*****************************************************************************/

/* Calculate start of event enumeration; id is top 8 bits of event */
#define BTA_SYS_EVT_START(id)       ((id) << 8)

/*****************************************************************************
**  events for BTA SYS HW manager
*****************************************************************************/

/* events sent to SYS HW manager - must be kept synchronized with tables in bta_sys_main.c */
enum {
    /* device manager local device API events */
    BTA_SYS_API_ENABLE_EVT = BTA_SYS_EVT_START(BTA_ID_SYS),
    BTA_SYS_EVT_ENABLED_EVT,
    BTA_SYS_EVT_STACK_ENABLED_EVT,
    BTA_SYS_API_DISABLE_EVT,
    BTA_SYS_EVT_DISABLED_EVT,
    BTA_SYS_ERROR_EVT,

    BTA_SYS_MAX_EVT
};



/* SYS HW status events - returned by SYS HW manager to other modules. */
enum {
    BTA_SYS_HW_OFF_EVT,
    BTA_SYS_HW_ON_EVT,
    BTA_SYS_HW_STARTING_EVT,
    BTA_SYS_HW_STOPPING_EVT,
    BTA_SYS_HW_ERROR_EVT

};
typedef UINT8 tBTA_SYS_HW_EVT;

/* HW enable callback type */
typedef void (tBTA_SYS_HW_CBACK)(tBTA_SYS_HW_EVT status);

/*****************************************************************************
**  Function declarations
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern void bta_sys_init(void);
extern void bta_sys_free(void);
extern void bta_sys_event(BT_HDR *p_msg);
extern void bta_sys_set_trace_level(UINT8 level);
extern void bta_sys_register(UINT8 id, const tBTA_SYS_REG *p_reg);
extern void bta_sys_deregister(UINT8 id);
extern BOOLEAN bta_sys_is_register(UINT8 id);
extern UINT16 bta_sys_get_sys_features(void);
extern void bta_sys_sendmsg(void *p_msg);
extern void bta_sys_start_timer(TIMER_LIST_ENT *p_tle, UINT16 type, INT32 timeout_ms);
extern void bta_sys_stop_timer(TIMER_LIST_ENT *p_tle);
extern void bta_sys_free_timer(TIMER_LIST_ENT *p_tle);
extern void bta_sys_disable(tBTA_SYS_HW_MODULE module);
extern UINT32 bta_sys_get_remaining_ticks(TIMER_LIST_ENT *p_target_tle);

extern void bta_sys_hw_register( tBTA_SYS_HW_MODULE module, tBTA_SYS_HW_CBACK *cback);
extern void bta_sys_hw_unregister( tBTA_SYS_HW_MODULE module );


extern void bta_sys_rm_register(tBTA_SYS_CONN_CBACK *p_cback);
extern void bta_sys_pm_register(tBTA_SYS_CONN_CBACK *p_cback);

extern void bta_sys_policy_register(tBTA_SYS_CONN_CBACK *p_cback);
extern void bta_sys_sco_register(tBTA_SYS_CONN_CBACK *p_cback);


extern void bta_sys_conn_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_conn_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_app_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_app_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_sco_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_sco_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_sco_use(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_sco_unuse(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_idle(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bta_sys_busy(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);

#if (BTM_SSR_INCLUDED == TRUE)
extern void bta_sys_ssr_cfg_register(tBTA_SYS_SSR_CFG_CBACK *p_cback);
extern void bta_sys_chg_ssr_config (UINT8 id, UINT8 app_id, UINT16 max_latency, UINT16 min_tout);
#endif

extern void bta_sys_role_chg_register(tBTA_SYS_CONN_CBACK *p_cback);
extern void bta_sys_notify_role_chg(BD_ADDR_PTR p_bda, UINT8 new_role, UINT8 hci_status);
extern void bta_sys_collision_register(UINT8 bta_id, tBTA_SYS_CONN_CBACK *p_cback);
extern void bta_sys_notify_collision (BD_ADDR_PTR p_bda);

#if (BTA_EIR_CANNED_UUID_LIST != TRUE)
extern void bta_sys_eir_register(tBTA_SYS_EIR_CBACK *p_cback);
extern void bta_sys_add_uuid(UINT16 uuid16);
extern void bta_sys_remove_uuid(UINT16 uuid16);
#else
#define bta_sys_eir_register(ut)
#define bta_sys_add_uuid(ut)
#define bta_sys_remove_uuid(ut)
#endif

extern void bta_sys_set_policy (UINT8 id, UINT8 policy, BD_ADDR peer_addr);
extern void bta_sys_clear_policy (UINT8 id, UINT8 policy, BD_ADDR peer_addr);
extern void bta_sys_set_default_policy (UINT8 id, UINT8 policy);
extern void bta_sys_clear_default_policy (UINT8 id, UINT8 policy);
extern BOOLEAN bta_sys_vs_hdl(UINT16 evt, void *p);

#ifdef __cplusplus
}
#endif

#endif /* BTA_SYS_H */
