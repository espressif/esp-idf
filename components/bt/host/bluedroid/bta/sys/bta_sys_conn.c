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
 *  Routes connection status callbacks from various sub systems to DM
 *
 ******************************************************************************/

#include <stddef.h>
#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta_sys_int.h"
#include "bta/utl.h"

#if (CLASSIC_BT_INCLUDED == TRUE)

/*******************************************************************************
**
** Function         bta_sys_rm_register
**
** Description      Called by BTA DM to register role management callbacks
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_rm_register(tBTA_SYS_CONN_CBACK *p_cback)
{
    bta_sys_cb.prm_cb = p_cback;
}


/*******************************************************************************
**
** Function         bta_sys_policy_register
**
** Description      Called by BTA DM to register link policy change callbacks
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_policy_register(tBTA_SYS_CONN_CBACK *p_cback)
{
    bta_sys_cb.p_policy_cb = p_cback;
}

/*******************************************************************************
**
** Function         bta_sys_role_chg_register
**
** Description      Called by BTA AV to register role change callbacks
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_role_chg_register(tBTA_SYS_CONN_CBACK *p_cback)
{
    bta_sys_cb.p_role_cb = p_cback;
}
/*******************************************************************************
**
** Function         bta_sys_ssr_cfg_register
**
** Description      Called by BTA DM to register SSR configuration callback
**
**
** Returns          void
**
*******************************************************************************/
#if (BTM_SSR_INCLUDED == TRUE)
void bta_sys_ssr_cfg_register(tBTA_SYS_SSR_CFG_CBACK *p_cback)
{
    bta_sys_cb.p_ssr_cb = p_cback;
}
#endif
/*******************************************************************************
**
** Function         bta_sys_role_chg_register
**
** Description      Called by BTA AV to register role change callbacks
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_notify_role_chg(BD_ADDR_PTR p_bda, UINT8 new_role, UINT8 hci_status)
{
    if (bta_sys_cb.p_role_cb) {
        bta_sys_cb.p_role_cb(BTA_SYS_ROLE_CHANGE, new_role, hci_status, p_bda);
    }
}

/*******************************************************************************
**
** Function         bta_sys_collision_register
**
** Description      Called by any BTA module to register for collision event.
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_collision_register(UINT8 bta_id, tBTA_SYS_CONN_CBACK *p_cback)
{
    UINT8 index;

    for (index = 0; index < MAX_COLLISION_REG; index++) {
        if ((bta_sys_cb.colli_reg.id[index] == bta_id) ||
                (bta_sys_cb.colli_reg.id[index] == 0)) {
            bta_sys_cb.colli_reg.id[index] = bta_id;
            bta_sys_cb.colli_reg.p_coll_cback[index] = p_cback;
            return;
        }
    }
}

/*******************************************************************************
**
** Function         bta_sys_notify_collision
**
** Description      Called by BTA DM to notify collision event.
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_notify_collision (BD_ADDR_PTR p_bda)
{
    UINT8 index;

    for (index = 0; index < MAX_COLLISION_REG; index++) {
        if ((bta_sys_cb.colli_reg.id[index] != 0) &&
                (bta_sys_cb.colli_reg.p_coll_cback[index] != NULL)) {
            bta_sys_cb.colli_reg.p_coll_cback[index] (0, BTA_ID_SYS, 0, p_bda);
        }
    }
}

/*******************************************************************************
**
** Function         bta_sys_sco_register
**
** Description      Called by BTA AV to register sco connection change callbacks
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_sco_register(tBTA_SYS_CONN_CBACK *p_cback)
{
    bta_sys_cb.p_sco_cb = p_cback;
}

/*******************************************************************************
**
** Function         bta_sys_pm_register
**
** Description      Called by BTA DM to register power management callbacks
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_pm_register(tBTA_SYS_CONN_CBACK *p_cback)
{
    bta_sys_cb.ppm_cb = p_cback;
}

/*******************************************************************************
**
** Function         bta_sys_conn_open
**
** Description      Called by BTA subsystems when a connection is made to
**                  the service
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_conn_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    if (bta_sys_cb.prm_cb) {

        bta_sys_cb.prm_cb(BTA_SYS_CONN_OPEN, id, app_id, peer_addr);

    }

    if (bta_sys_cb.ppm_cb) {

        bta_sys_cb.ppm_cb(BTA_SYS_CONN_OPEN, id, app_id, peer_addr);

    }
}



/*******************************************************************************
**
** Function         bta_sys_conn_close
**
** Description      Called by BTA subsystems when a connection to the service
**                  is closed
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_conn_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    if (bta_sys_cb.prm_cb) {

        bta_sys_cb.prm_cb(BTA_SYS_CONN_CLOSE, id, app_id, peer_addr);

    }

    if (bta_sys_cb.ppm_cb) {

        bta_sys_cb.ppm_cb(BTA_SYS_CONN_CLOSE, id, app_id, peer_addr);

    }
}


/*******************************************************************************
**
** Function         bta_sys_app_open
**
** Description      Called by BTA subsystems when application initiates connection
**                  to a peer device
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_app_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    if (bta_sys_cb.ppm_cb) {
        bta_sys_cb.ppm_cb(BTA_SYS_APP_OPEN, id, app_id, peer_addr);
    }
}



/*******************************************************************************
**
** Function         bta_sys_app_close
**
** Description      Called by BTA subsystems when application initiates close
**                  of connection to peer device
**
** Returns          void
**
*******************************************************************************/
void bta_sys_app_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    if (bta_sys_cb.ppm_cb) {
        bta_sys_cb.ppm_cb(BTA_SYS_APP_CLOSE, id, app_id, peer_addr);
    }
}


/*******************************************************************************
**
** Function         bta_sys_sco_open
**
** Description      Called by BTA subsystems when sco connection for that service
**                  is open
**
** Returns          void
**
*******************************************************************************/
void bta_sys_sco_open(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    /* AG triggers p_sco_cb by bta_sys_sco_use. */
    if ((id != BTA_ID_AG) && (bta_sys_cb.p_sco_cb)) {
        /* without querying BTM_GetNumScoLinks() */
        bta_sys_cb.p_sco_cb(BTA_SYS_SCO_OPEN, 1, app_id, peer_addr);
    }

    if (bta_sys_cb.ppm_cb) {
        bta_sys_cb.ppm_cb(BTA_SYS_SCO_OPEN, id, app_id, peer_addr);
    }
}

/*******************************************************************************
**
** Function         bta_sys_sco_close
**
** Description      Called by BTA subsystems when sco connection for that service
**                  is closed
**
** Returns          void
**
*******************************************************************************/
void bta_sys_sco_close(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    UINT8 num_sco_links;

    if ((id != BTA_ID_AG) && (bta_sys_cb.p_sco_cb)) {
        num_sco_links = BTM_GetNumScoLinks();
        bta_sys_cb.p_sco_cb(BTA_SYS_SCO_CLOSE, num_sco_links, app_id, peer_addr);
    }

    if (bta_sys_cb.ppm_cb) {
        bta_sys_cb.ppm_cb(BTA_SYS_SCO_CLOSE, id, app_id, peer_addr);
    }
}

/*******************************************************************************
**
** Function         bta_sys_sco_use
**
** Description      Called by BTA subsystems when that service needs to use sco.
**
**
** Returns          void
**
*******************************************************************************/
void bta_sys_sco_use(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    UNUSED(id);

    /* AV streaming need to be suspended before SCO is connected. */
    if (bta_sys_cb.p_sco_cb) {
        /* without querying BTM_GetNumScoLinks() */
        bta_sys_cb.p_sco_cb(BTA_SYS_SCO_OPEN, 1, app_id, peer_addr);
    }
}

/*******************************************************************************
**
** Function         bta_sys_sco_unuse
**
** Description      Called by BTA subsystems when sco connection for that service
**                  is no longer needed.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_sco_unuse(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    UINT8 num_sco_links;
    UNUSED(id);

    if ((bta_sys_cb.p_sco_cb)) {
        num_sco_links = BTM_GetNumScoLinks();
        bta_sys_cb.p_sco_cb(BTA_SYS_SCO_CLOSE, num_sco_links, app_id, peer_addr);
    }
}
/*******************************************************************************
**
** Function         bta_sys_chg_ssr_config
**
** Description      Called by BTA subsystems to indicate that the given app SSR setting
**                  need to be changed.
**
** Returns          void
**
*******************************************************************************/
#if (BTM_SSR_INCLUDED == TRUE)
void bta_sys_chg_ssr_config (UINT8 id, UINT8 app_id, UINT16 max_latency, UINT16 min_tout)
{
    if (bta_sys_cb.p_ssr_cb) {
        bta_sys_cb.p_ssr_cb(id, app_id, max_latency, min_tout);
    }
}
#endif
/*******************************************************************************
**
** Function         bta_sys_set_policy
**
** Description      Called by BTA subsystems to indicate that the given link
**                  policy to peer device should be set
**
** Returns          void
**
*******************************************************************************/
void bta_sys_set_policy (UINT8 id, UINT8 policy, BD_ADDR peer_addr)
{
    if (bta_sys_cb.p_policy_cb) {
        bta_sys_cb.p_policy_cb(BTA_SYS_PLCY_SET, id, policy, peer_addr);
    }
}

/*******************************************************************************
**
** Function         bta_sys_clear_policy
**
** Description      Called by BTA subsystems to indicate that the given link
**                  policy to peer device should be clear
**
** Returns          void
**
*******************************************************************************/
void bta_sys_clear_policy (UINT8 id, UINT8 policy, BD_ADDR peer_addr)
{
    if (bta_sys_cb.p_policy_cb) {
        bta_sys_cb.p_policy_cb(BTA_SYS_PLCY_CLR, id, policy, peer_addr);
    }
}

/*******************************************************************************
**
** Function         bta_sys_set_default_policy
**
** Description      Called by BTA subsystems to indicate that the given default
**                  link policy should be set
**
** Returns          void
**
*******************************************************************************/
void bta_sys_set_default_policy (UINT8 id, UINT8 policy)
{
    if (bta_sys_cb.p_policy_cb) {
        bta_sys_cb.p_policy_cb(BTA_SYS_PLCY_DEF_SET, id, policy, NULL);
    }
}

/*******************************************************************************
**
** Function         bta_sys_clear_default_policy
**
** Description      Called by BTA subsystems to indicate that the given default
**                  link policy should be clear
**
** Returns          void
**
*******************************************************************************/
void bta_sys_clear_default_policy (UINT8 id, UINT8 policy)
{
    if (bta_sys_cb.p_policy_cb) {
        bta_sys_cb.p_policy_cb(BTA_SYS_PLCY_DEF_CLR, id, policy, NULL);
    }
}

/*******************************************************************************
**
** Function         bta_sys_idle
**
** Description      Called by BTA subsystems to indicate that the connection to
**                  peer device is idle
**
** Returns          void
**
*******************************************************************************/
void bta_sys_idle(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{

    if (bta_sys_cb.prm_cb) {

        bta_sys_cb.prm_cb(BTA_SYS_CONN_IDLE, id, app_id, peer_addr);

    }

    if (bta_sys_cb.ppm_cb) {

        bta_sys_cb.ppm_cb(BTA_SYS_CONN_IDLE, id, app_id, peer_addr);
    }
}

/*******************************************************************************
**
** Function         bta_sys_busy
**
** Description      Called by BTA subsystems to indicate that the connection to
**                  peer device is busy
**
** Returns          void
**
*******************************************************************************/
void bta_sys_busy(UINT8 id, UINT8 app_id, BD_ADDR peer_addr)
{
    if (bta_sys_cb.prm_cb) {

        bta_sys_cb.prm_cb(BTA_SYS_CONN_BUSY, id, app_id, peer_addr);

    }

    if (bta_sys_cb.ppm_cb) {

        bta_sys_cb.ppm_cb(BTA_SYS_CONN_BUSY, id, app_id, peer_addr);

    }
}

#if (BTA_EIR_CANNED_UUID_LIST != TRUE)
/*******************************************************************************
**
** Function         bta_sys_eir_register
**
** Description      Called by BTA DM to register EIR utility function that can be
**                  used by the other BTA modules to add/remove UUID.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_eir_register(tBTA_SYS_EIR_CBACK *p_cback)
{
    bta_sys_cb.eir_cb = p_cback;
}

/*******************************************************************************
**
** Function         bta_sys_add_uuid
**
** Description      Called by BTA subsystems to indicate to DM that new service
**                  class UUID is added.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_add_uuid(UINT16 uuid16)
{
    tBT_UUID uuid;
    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = uuid16;

    if (bta_sys_cb.eir_cb) {
        bta_sys_cb.eir_cb(uuid, TRUE);
    }
}


/*******************************************************************************
**
** Function         bta_sys_add_uuid_32
**
** Description      Called by BTA subsystems to indicate to DM that new service
**                  class UUID is added.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_add_uuid_32(UINT32 uuid32)
{
    tBT_UUID uuid;
    uuid.len = LEN_UUID_32;
    uuid.uu.uuid32 = uuid32;

    if (bta_sys_cb.eir_cb) {
        bta_sys_cb.eir_cb(uuid, TRUE);
    }
}

/*******************************************************************************
**
** Function         bta_sys_add_uuid_128
**
** Description      Called by BTA subsystems to indicate to DM that new service
**                  class UUID is added.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_add_uuid_128(UINT8 *uuid128)
{
    tBT_UUID uuid;
    uuid.len = LEN_UUID_128;
    memcpy(&uuid.uu.uuid128, uuid128, LEN_UUID_128);

    if (bta_sys_cb.eir_cb) {
        bta_sys_cb.eir_cb(uuid, TRUE);
    }
}

/*******************************************************************************
**
** Function         bta_sys_remove_uuid
**
** Description      Called by BTA subsystems to indicate to DM that the service
**                  class UUID is removed.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_remove_uuid(UINT16 uuid16)
{
    tBT_UUID uuid;
    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = uuid16;

    if (bta_sys_cb.eir_cb) {
        bta_sys_cb.eir_cb(uuid, FALSE);
    }
}

/*******************************************************************************
**
** Function         bta_sys_remove_uuid_32
**
** Description      Called by BTA subsystems to indicate to DM that the service
**                  class UUID is removed.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_remove_uuid_32(UINT32 uuid32)
{
    tBT_UUID uuid;
    uuid.len = LEN_UUID_32;
    uuid.uu.uuid32 = uuid32;

    if (bta_sys_cb.eir_cb) {
        bta_sys_cb.eir_cb(uuid, FALSE);
    }
}

/*******************************************************************************
**
** Function         bta_sys_remove_uuid_128
**
** Description      Called by BTA subsystems to indicate to DM that the service
**                  class UUID is removed.
**
** Returns          void
**
*******************************************************************************/
void bta_sys_remove_uuid_128(UINT8 *uuid128)
{
    tBT_UUID uuid;
    uuid.len = LEN_UUID_128;
    memcpy(&uuid.uu.uuid128, uuid128, LEN_UUID_128);

    if (bta_sys_cb.eir_cb) {
        bta_sys_cb.eir_cb(uuid, FALSE);
    }
}

#endif

/*******************************************************************************
**
** Function         bta_sys_vs_hdl
**
** Description      Called by BTA subsystems to execute a VS event handler function
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_sys_vs_hdl(UINT16 evt, void *p)
{
    if (bta_sys_cb.p_vs_evt_hdlr) {
        return (*bta_sys_cb.p_vs_evt_hdlr)(evt, p);
    }

    return FALSE;
}

#endif // #if (CLASSIC_BT_INCLUDED == TRUE)
