/**
 ****************************************************************************************
 *
 * @file bt_prf_sys.h
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/10/12
 *
 *
 ****************************************************************************************
 */

#ifndef _PROFILE_SYS_H__
#define _PROFILE_SYS_H__

#include "bt_types.h"

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
typedef BOOLEAN (tBT_PRF_SYS_EVT_HDLR)(BT_HDR *p_msg);

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
extern void bt_prf_sys_event(BT_HDR *p_msg);

extern void bt_prf_sys_register(UINT8 id, const tBT_PRF_SYS_REG *p_reg);
extern void bt_prf_sys_deregister(UINT8 id);
extern BOOLEAN bt_prf_sys_is_register(UINT8 id);

extern void bt_prf_sys_idle(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);
extern void bt_prf_sys_busy(UINT8 id, UINT8 app_id, BD_ADDR peer_addr);


#endif	///_PROFILE_SYS_H__
