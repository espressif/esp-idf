// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _PROFILE_SYS_H__
#define _PROFILE_SYS_H__

#include "bt_types.h"
#include "prf_defs.h"

enum
{
	PRF_ID_SYS,
	PRF_ID_CONN,
	PRF_ID_HIDD_LE,
	PRF_ID_HIDH_LE,
	PRF_ID_DISS_LE,
	PRF_ID_DISC_LE,
	PRF_ID_AIRSYNC_LE,
	PRF_ID_ANCC_LE,
	PRF_ID_BUT_LE,

	PRF_ID_MAX
};

typedef UINT8 tBT_PRF_SYS_CONN_STATUS;


/* disable function type */
typedef void (tBT_PRF_SYS_DISABLE)(void);
/* event handler function type */
typedef BOOLEAN (tBT_PRF_SYS_EVT_HDLR)(prf_hdr_evt_t *p_msg);

/* conn callback for role / low power manager*/
typedef void (tBT_PRF_SYS_CONN_CBACK)(tBT_PRF_SYS_CONN_STATUS status,
									UINT8 id, UINT8 app_id, BD_ADDR peer_addr);

/* Calculate start of event enumeration; id is top 8 bits of event */
#define BT_PRF_SYS_EVT_START(id)       ((id) << 8)



/* registration structure */
typedef struct
{
    tBT_PRF_SYS_EVT_HDLR   *evt_hdlr;
    tBT_PRF_SYS_DISABLE    *disable;
} tBT_PRF_SYS_REG;

/* system manager control block */
typedef struct
{
    tBT_PRF_SYS_REG            *reg[PRF_ID_MAX];       /* registration structures */
    BOOLEAN                 	is_reg[PRF_ID_MAX];     /* registration structures */
    tBT_PRF_SYS_CONN_CBACK     *prm_cb;                 /* role management callback registered by DM */
    tBT_PRF_SYS_CONN_CBACK     *ppm_cb;                 /* low power management callback registered by DM */
    tBT_PRF_SYS_CONN_CBACK     *p_policy_cb;            /* link policy change callback registered by DM */

} tBT_PRF_SYS_CB;


extern tBT_PRF_SYS_CB bt_prf_sys_cb;


extern void bt_prf_sys_init(void);
extern void bt_prf_sys_free(void);
extern void bt_prf_sys_event(prf_hdr_evt_t *p_msg);

extern void bt_prf_sys_sendmsg(void *p_msg);


extern void bt_prf_sys_register(uint8_t id, const tBT_PRF_SYS_REG *p_reg);
extern void bt_prf_sys_deregister(uint8_t id);
extern BOOLEAN bt_prf_sys_is_register(uint8_t id);

extern void bt_prf_sys_idle(uint8_t id, uint8_t app_id, BD_ADDR peer_addr);
extern void bt_prf_sys_busy(uint8_t id, uint8_t app_id, BD_ADDR peer_addr);


#endif	///_PROFILE_SYS_H__
