/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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
 *  This file contains functions for BLE address management.
 *
 ******************************************************************************/

#include <string.h>

#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "btm_int.h"
#include "stack/gap_api.h"
#include "device/controller.h"

#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE)
#include "btm_ble_int.h"
#include "stack/smp_api.h"


/*******************************************************************************
**
** Function         btm_gen_resolve_paddr_cmpl
**
** Description      This is callback functioin when resolvable private address
**                  generation is complete.
**
** Returns          void
**
*******************************************************************************/
static void btm_gen_resolve_paddr_cmpl(tSMP_ENC *p)
{
    tBTM_LE_RANDOM_CB *p_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    BTM_TRACE_EVENT ("btm_gen_resolve_paddr_cmpl");

    if (p) {
        /* set hash to be LSB of rpAddress */
        p_cb->private_addr[5] = p->param_buf[0];
        p_cb->private_addr[4] = p->param_buf[1];
        p_cb->private_addr[3] = p->param_buf[2];

        /* set it to controller */
        btm_ble_set_random_addr(p_cb->private_addr);

        p_cb->exist_addr_bit |= BTM_BLE_GAP_ADDR_BIT_RESOLVABLE;
        memcpy(p_cb->resolvale_addr, p_cb->private_addr, BD_ADDR_LEN);
        if (p_cb->set_local_privacy_cback){
            (*p_cb->set_local_privacy_cback)(BTM_SET_PRIVACY_SUCCESS);
            p_cb->set_local_privacy_cback = NULL;
        }

        /* start a periodical timer to refresh random addr */
        btu_stop_timer_oneshot(&p_cb->raddr_timer_ent);
#if (BTM_BLE_CONFORMANCE_TESTING == TRUE)
        btu_start_timer_oneshot(&p_cb->raddr_timer_ent, BTU_TTYPE_BLE_RANDOM_ADDR,
                                btm_cb.ble_ctr_cb.rpa_tout);
#else
        btu_start_timer_oneshot(&p_cb->raddr_timer_ent, BTU_TTYPE_BLE_RANDOM_ADDR,
                                BTM_BLE_PRIVATE_ADDR_INT);
#endif
    } else {
        /* random address set failure */
        BTM_TRACE_DEBUG("set random address failed");
        if (p_cb->set_local_privacy_cback){
            (*p_cb->set_local_privacy_cback)(BTM_SET_PRIVACY_FAIL);
            p_cb->set_local_privacy_cback = NULL;
        }
    }
}
/*******************************************************************************
**
** Function         btm_gen_resolve_paddr_low
**
** Description      This function is called when random address has generate the
**                  random number base for low 3 byte bd address.
**
** Returns          void
**
*******************************************************************************/
void btm_gen_resolve_paddr_low(tBTM_RAND_ENC *p)
{
#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
    tBTM_LE_RANDOM_CB *p_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tSMP_ENC    output;

    BTM_TRACE_EVENT ("btm_gen_resolve_paddr_low");
    if (p) {
        p->param_buf[2] &= (~BLE_RESOLVE_ADDR_MASK);
        p->param_buf[2] |= BLE_RESOLVE_ADDR_MSB;

        p_cb->private_addr[2] = p->param_buf[0];
        p_cb->private_addr[1] = p->param_buf[1];
        p_cb->private_addr[0] = p->param_buf[2];

        /* encrypt with ur IRK */
        if (!SMP_Encrypt(btm_cb.devcb.id_keys.irk, BT_OCTET16_LEN, p->param_buf, 3, &output)) {
            btm_gen_resolve_paddr_cmpl(NULL);
        } else {
            btm_gen_resolve_paddr_cmpl(&output);
        }
    }
#endif
}
/*******************************************************************************
**
** Function         btm_gen_resolvable_private_addr
**
** Description      This function generate a resolvable private address.
**
** Returns          void
**
*******************************************************************************/
void btm_gen_resolvable_private_addr (void *p_cmd_cplt_cback)
{
    BTM_TRACE_EVENT ("btm_gen_resolvable_private_addr");
    /* generate 3B rand as BD LSB, SRK with it, get BD MSB */
    if (!btsnd_hcic_ble_rand((void *)p_cmd_cplt_cback)) {
        btm_gen_resolve_paddr_cmpl(NULL);
    }
}
/*******************************************************************************
**
** Function         btm_gen_non_resolve_paddr_cmpl
**
** Description      This is the callback function when non-resolvable private
**                  function is generated and write to controller.
**
** Returns          void
**
*******************************************************************************/
static void btm_gen_non_resolve_paddr_cmpl(tBTM_RAND_ENC *p)
{
    tBTM_LE_RANDOM_CB *p_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tBTM_BLE_ADDR_CBACK *p_cback = p_cb->p_generate_cback;
    void    *p_data = p_cb->p;
    UINT8   *pp;
    BD_ADDR     static_random;

    BTM_TRACE_EVENT ("btm_gen_non_resolve_paddr_cmpl");

    p_cb->p_generate_cback = NULL;
    if (p) {

        pp = p->param_buf;
        STREAM_TO_BDADDR(static_random, pp);
        /* mask off the 2 MSB */
        static_random[0] &= BLE_STATIC_PRIVATE_MSB_MASK;

        /* report complete */
        if (p_cback) {
            (* p_cback)(static_random, p_data);
        }
    } else {
        BTM_TRACE_DEBUG("btm_gen_non_resolvable_private_addr failed");
        if (p_cback) {
            (* p_cback)(NULL, p_data);
        }
    }
}
/*******************************************************************************
**
** Function         btm_gen_non_resolvable_private_addr
**
** Description      This function generate a non-resolvable private address.
**
**
** Returns          void
**
*******************************************************************************/
void btm_gen_non_resolvable_private_addr (tBTM_BLE_ADDR_CBACK *p_cback, void *p)
{
    tBTM_LE_RANDOM_CB   *p_mgnt_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;

    BTM_TRACE_EVENT ("btm_gen_non_resolvable_private_addr");

    if (p_mgnt_cb->p_generate_cback != NULL) {
        return;
    }

    p_mgnt_cb->p_generate_cback = p_cback;
    p_mgnt_cb->p                = p;
    if (!btsnd_hcic_ble_rand((void *)btm_gen_non_resolve_paddr_cmpl)) {
        btm_gen_non_resolve_paddr_cmpl(NULL);
    }

}

/*******************************************************************************
**  Utility functions for Random address resolving
*******************************************************************************/
/*******************************************************************************
**
** Function         btm_ble_resolve_address_cmpl
**
** Description      This function sends the random address resolving complete
**                  callback.
**
** Returns          None.
**
*******************************************************************************/
#if SMP_INCLUDED == TRUE
static void btm_ble_resolve_address_cmpl(void)
{
    tBTM_LE_RANDOM_CB   *p_mgnt_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;

    BTM_TRACE_EVENT ("btm_ble_resolve_address_cmpl p_mgnt_cb->p_dev_rec = 0x%08x", (uint32_t)p_mgnt_cb->p_dev_rec);

    p_mgnt_cb->busy = FALSE;

    (* p_mgnt_cb->p_resolve_cback)(p_mgnt_cb->p_dev_rec, p_mgnt_cb->p);
}
/*******************************************************************************
**
** Function         btm_ble_proc_resolve_x
**
** Description      This function compares the X with random address 3 MSO bytes
**                  to find a match, if not match, continue for next record.
**
** Returns          None.
**
*******************************************************************************/
static BOOLEAN btm_ble_proc_resolve_x(tSMP_ENC *p)
{
    tBTM_LE_RANDOM_CB   *p_mgnt_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    UINT8    comp[3];
    BTM_TRACE_EVENT ("btm_ble_proc_resolve_x");
    /* compare the hash with 3 LSB of bd address */
    comp[0] = p_mgnt_cb->random_bda[5];
    comp[1] = p_mgnt_cb->random_bda[4];
    comp[2] = p_mgnt_cb->random_bda[3];

    if (p) {
        if (!memcmp(p->param_buf, &comp[0], 3)) {
            /* match is found */
            BTM_TRACE_EVENT ("match is found");
            btm_ble_resolve_address_cmpl();
            return TRUE;
        }
    }

    return FALSE;
}
#endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         btm_ble_init_pseudo_addr
**
** Description      This function is used to initialize pseudo address.
**                  If pseudo address is not available, use dummy address
**
** Returns          TRUE is updated; FALSE otherwise.
**
*******************************************************************************/
BOOLEAN btm_ble_init_pseudo_addr (tBTM_SEC_DEV_REC *p_dev_rec, BD_ADDR new_pseudo_addr)
{
#if (SMP_INCLUDED == TRUE)
    BD_ADDR dummy_bda = {0};

    if (memcmp(p_dev_rec->ble.pseudo_addr, dummy_bda, BD_ADDR_LEN) == 0) {
        memcpy(p_dev_rec->ble.pseudo_addr, new_pseudo_addr, BD_ADDR_LEN);
        return TRUE;
    }
#endif  ///SMP_INCLUDED == TRUE
    return FALSE;
}

/*******************************************************************************
**
** Function         btm_ble_addr_resolvable
**
** Description      This function checks if a RPA is resolvable by the device key.
**
** Returns          TRUE is resolvable; FALSE otherwise.
**
*******************************************************************************/
BOOLEAN btm_ble_addr_resolvable (BD_ADDR rpa, tBTM_SEC_DEV_REC *p_dev_rec)
{
    BOOLEAN rt = FALSE;
#if (SMP_INCLUDED == TRUE)
    if (!BTM_BLE_IS_RESOLVE_BDA(rpa)) {
        return rt;
    }

    UINT8 rand[3];
    tSMP_ENC output;
    if ((p_dev_rec->device_type & BT_DEVICE_TYPE_BLE) &&
            (p_dev_rec->ble.key_type & BTM_LE_KEY_PID)) {
        BTM_TRACE_DEBUG("%s try to resolve", __func__);
        /* use the 3 MSB of bd address as prand */
        rand[0] = rpa[2];
        rand[1] = rpa[1];
        rand[2] = rpa[0];

        /* generate X = E irk(R0, R1, R2) and R is random address 3 LSO */
        SMP_Encrypt(p_dev_rec->ble.keys.irk, BT_OCTET16_LEN,
                    &rand[0], 3, &output);

        rand[0] = rpa[5];
        rand[1] = rpa[4];
        rand[2] = rpa[3];

        if (!memcmp(output.param_buf, &rand[0], 3)) {
            btm_ble_init_pseudo_addr (p_dev_rec, rpa);
            rt = TRUE;
        }
    }
#endif  ///SMP_INCLUDED == TRUE
    return rt;
}

#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         btm_ble_match_random_bda
**
** Description      This function match the random address to the appointed device
**                  record, starting from calculating IRK. If record index exceed
**                  the maximum record number, matching failed and send callback.
**
** Returns          None.
**
*******************************************************************************/
static BOOLEAN btm_ble_match_random_bda(tBTM_SEC_DEV_REC *p_dev_rec)
{
    /* use the 3 MSB of bd address as prand */

    tBTM_LE_RANDOM_CB *p_mgnt_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    UINT8 rand[3];
    rand[0] = p_mgnt_cb->random_bda[2];
    rand[1] = p_mgnt_cb->random_bda[1];
    rand[2] = p_mgnt_cb->random_bda[0];

    BTM_TRACE_EVENT("%s p_dev_rec = 0x%08x", __func__, (uint32_t)p_dev_rec);

    {
        tSMP_ENC output;

        BTM_TRACE_DEBUG("sec_flags = %02x device_type = %d", p_dev_rec->sec_flags,
                        p_dev_rec->device_type);

        if ((p_dev_rec->device_type & BT_DEVICE_TYPE_BLE) &&
                (p_dev_rec->ble.key_type & BTM_LE_KEY_PID)) {
            /* generate X = E irk(R0, R1, R2) and R is random address 3 LSO */
            SMP_Encrypt(p_dev_rec->ble.keys.irk, BT_OCTET16_LEN,
                        &rand[0], 3, &output);
            return btm_ble_proc_resolve_x(&output);
        } else {
            // not completed
            return FALSE;
        }
    }
}
#endif ///BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         btm_ble_resolve_random_addr
**
** Description      This function is called to resolve a random address.
**
** Returns          pointer to the security record of the device whom a random
**                  address is matched to.
**
*******************************************************************************/
void btm_ble_resolve_random_addr(BD_ADDR random_bda, tBTM_BLE_RESOLVE_CBACK *p_cback, void *p)
{
#if (SMP_INCLUDED == TRUE)
    if (btm_cb.addr_res_en == FALSE) {
        return;
    }

    tBTM_LE_RANDOM_CB   *p_mgnt_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    list_node_t         *p_node    = NULL;
    tBTM_SEC_DEV_REC *p_dev_rec    = NULL;

    BTM_TRACE_EVENT ("btm_ble_resolve_random_addr");
    if ( !p_mgnt_cb->busy) {
        p_mgnt_cb->p = p;
        p_mgnt_cb->busy = TRUE;
        p_mgnt_cb->p_dev_rec = NULL;
        p_mgnt_cb->p_resolve_cback = p_cback;
        memcpy(p_mgnt_cb->random_bda, random_bda, BD_ADDR_LEN);
        /* start to resolve random address */
        /* check for next security record */
        for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
            p_dev_rec = list_node(p_node);
            p_mgnt_cb->p_dev_rec = p_dev_rec;
            if (btm_ble_match_random_bda(p_dev_rec)) {
                break;
            }
            p_mgnt_cb->p_dev_rec = NULL;
        }
        btm_ble_resolve_address_cmpl();
    } else {
        (*p_cback)(NULL, p);
    }
#endif

}


/*******************************************************************************
**  address mapping between pseudo address and real connection address
*******************************************************************************/
/*******************************************************************************
**
** Function         btm_find_dev_by_identity_addr
**
** Description      find the security record whose LE static address is matching
**
*******************************************************************************/
tBTM_SEC_DEV_REC *btm_find_dev_by_identity_addr(BD_ADDR bd_addr, UINT8 addr_type)
{
#if BLE_PRIVACY_SPT == TRUE
    tBTM_SEC_DEV_REC *p_dev_rec = NULL;
    list_node_t *p_node         = NULL;
    tSecDevContext context;
    context.type                = SEC_DEV_ID_ADDR;
    context.context.p_bd_addr   = bd_addr;
    context.free_check          = FALSE;
    p_node = list_foreach(btm_cb.p_sec_dev_rec_list, btm_find_sec_dev_in_list, &context);
    if (p_node) {
	p_dev_rec = list_node(p_node);
        if ((p_dev_rec->ble.static_addr_type & (~BLE_ADDR_TYPE_ID_BIT)) !=
                (addr_type & (~BLE_ADDR_TYPE_ID_BIT))) {
            BTM_TRACE_WARNING("%s find pseudo->random match with diff addr type: %d vs %d",
                              __func__, p_dev_rec->ble.static_addr_type, addr_type);
	}
    }
    return p_dev_rec;
#endif
    return NULL;
}

/*******************************************************************************
**
** Function         btm_identity_addr_to_random_pseudo
**
** Description      This function map a static BD address to a pseudo random address
**                  in security database.
**
*******************************************************************************/
BOOLEAN btm_identity_addr_to_random_pseudo(BD_ADDR bd_addr, UINT8 *p_addr_type, BOOLEAN refresh)
{
#if BLE_PRIVACY_SPT == TRUE
    if (btm_cb.addr_res_en == FALSE) {
        return TRUE;
    }

    tBTM_SEC_DEV_REC    *p_dev_rec = btm_find_dev_by_identity_addr(bd_addr, *p_addr_type);

    BTM_TRACE_EVENT ("%s", __func__);
    /* evt reported on static address, map static address to random pseudo */
    if (p_dev_rec != NULL) {
        /* if RPA offloading is supported, or 4.2 controller, do RPA refresh */
        if (refresh && controller_get_interface()->get_ble_resolving_list_max_size() != 0) {
            btm_ble_read_resolving_list_entry(p_dev_rec);
        }

        /* assign the original address to be the current report address */
        if (!btm_ble_init_pseudo_addr (p_dev_rec, bd_addr)) {
            memcpy(bd_addr, p_dev_rec->ble.pseudo_addr, BD_ADDR_LEN);
        }

        *p_addr_type = p_dev_rec->ble.ble_addr_type;
        return TRUE;
    }
#endif
    return FALSE;
}

/*******************************************************************************
**
** Function         btm_random_pseudo_to_identity_addr
**
** Description      This function map a random pseudo address to a public address
**                  random_pseudo is input and output parameter
**
*******************************************************************************/
BOOLEAN btm_random_pseudo_to_identity_addr(BD_ADDR random_pseudo, UINT8 *p_static_addr_type)
{
#if BLE_PRIVACY_SPT == TRUE
    if (btm_cb.addr_res_en == FALSE) {
        return TRUE;
    }

    tBTM_SEC_DEV_REC    *p_dev_rec = btm_find_dev (random_pseudo);

    if (p_dev_rec != NULL) {
        if (p_dev_rec->ble.in_controller_list & BTM_RESOLVING_LIST_BIT) {
            * p_static_addr_type = p_dev_rec->ble.static_addr_type;
            memcpy(random_pseudo, p_dev_rec->ble.static_addr, BD_ADDR_LEN);
            if (controller_get_interface()->supports_ble_privacy() && p_dev_rec->ble.ble_addr_type != BLE_ADDR_PUBLIC) {
                *p_static_addr_type |= BLE_ADDR_TYPE_ID_BIT;
            }
            return TRUE;
        }
    }
#endif
    return FALSE;
}

/*******************************************************************************
**
** Function         btm_ble_refresh_peer_resolvable_private_addr
**
** Description      This function refresh the currently used resolvable remote private address into security
**                  database and set active connection address.
**
*******************************************************************************/
void btm_ble_refresh_peer_resolvable_private_addr(BD_ADDR pseudo_bda, BD_ADDR rpa,
        UINT8 rra_type)
{
#if BLE_PRIVACY_SPT == TRUE
    UINT8 rra_dummy = FALSE;
    BD_ADDR dummy_bda = {0};

    if (memcmp(dummy_bda, rpa, BD_ADDR_LEN) == 0) {
        rra_dummy = TRUE;
    }

    /* update security record here, in adv event or connection complete process */
    tBTM_SEC_DEV_REC *p_sec_rec = btm_find_dev(pseudo_bda);
    if (p_sec_rec != NULL) {
        memcpy(p_sec_rec->ble.cur_rand_addr, rpa, BD_ADDR_LEN);

        /* unknown, if dummy address, set to static */
        if (rra_type == BTM_BLE_ADDR_PSEUDO) {
            p_sec_rec->ble.active_addr_type = rra_dummy ? BTM_BLE_ADDR_STATIC : BTM_BLE_ADDR_RRA;
        } else {
            p_sec_rec->ble.active_addr_type = rra_type;
        }
    } else {
        BTM_TRACE_ERROR("No matching known device in record");
        return;
    }

    BTM_TRACE_DEBUG("%s: active_addr_type: %d ",
                    __func__, p_sec_rec->ble.active_addr_type);

    /* connection refresh remote address */
    tACL_CONN *p_acl = btm_bda_to_acl(p_sec_rec->bd_addr, BT_TRANSPORT_LE);
    if (p_acl == NULL) {
        p_acl = btm_bda_to_acl(p_sec_rec->ble.pseudo_addr, BT_TRANSPORT_LE);
    }

    if (p_acl != NULL) {
        if (rra_type == BTM_BLE_ADDR_PSEUDO) {
            /* use static address, resolvable_private_addr is empty */
            if (rra_dummy) {
                p_acl->active_remote_addr_type = p_sec_rec->ble.static_addr_type;
                memcpy(p_acl->active_remote_addr, p_sec_rec->ble.static_addr, BD_ADDR_LEN);
            } else {
                p_acl->active_remote_addr_type = BLE_ADDR_RANDOM;
                memcpy(p_acl->active_remote_addr, rpa, BD_ADDR_LEN);
            }
        } else {
            p_acl->active_remote_addr_type = rra_type;
            memcpy(p_acl->active_remote_addr, rpa, BD_ADDR_LEN);
        }

        BTM_TRACE_DEBUG("p_acl->active_remote_addr_type: %d ", p_acl->active_remote_addr_type);
        BTM_TRACE_DEBUG("%s conn_addr: %02x:%02x:%02x:%02x:%02x:%02x",
                        __func__, p_acl->active_remote_addr[0], p_acl->active_remote_addr[1],
                        p_acl->active_remote_addr[2], p_acl->active_remote_addr[3],
                        p_acl->active_remote_addr[4], p_acl->active_remote_addr[5]);
    }
#endif
}

/*******************************************************************************
**
** Function         btm_ble_refresh_local_resolvable_private_addr
**
** Description      This function refresh the currently used resolvable private address for the
**                  active link to the remote device
**
*******************************************************************************/
void btm_ble_refresh_local_resolvable_private_addr(BD_ADDR pseudo_addr,
        BD_ADDR local_rpa)
{
#if BLE_PRIVACY_SPT == TRUE
    tACL_CONN *p = btm_bda_to_acl(pseudo_addr, BT_TRANSPORT_LE);
    BD_ADDR     dummy_bda = {0};

    if (p != NULL) {
        if (btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type >= BLE_ADDR_RANDOM) {
            p->conn_addr_type = BLE_ADDR_RANDOM;
            if (memcmp(local_rpa, dummy_bda, BD_ADDR_LEN)) {
                memcpy(p->conn_addr, local_rpa, BD_ADDR_LEN);
            } else {
                memcpy(p->conn_addr, btm_cb.ble_ctr_cb.addr_mgnt_cb.private_addr, BD_ADDR_LEN);
            }
        } else {
            p->conn_addr_type = BLE_ADDR_PUBLIC;
            memcpy(p->conn_addr, &controller_get_interface()->get_address()->address, BD_ADDR_LEN);
        }
    }
#endif
}
#endif
