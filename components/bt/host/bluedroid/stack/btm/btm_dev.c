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
 *  This file contains functions for the Bluetooth Device Manager
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>
#include <stddef.h>

#include "stack/bt_types.h"
#include "device/controller.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include "stack/hcidefs.h"
#include "stack/l2c_api.h"

static tBTM_SEC_DEV_REC *btm_find_oldest_dev (void);

/*******************************************************************************
**
** Function         BTM_SecAddDevice
**
** Description      Add/modify device.  This function will be normally called
**                  during host startup to restore all required information
**                  stored in the NVRAM.
**
** Parameters:      bd_addr          - BD address of the peer
**                  dev_class        - Device Class
**                  bd_name          - Name of the peer device.  NULL if unknown.
**                  features         - Remote device's features (up to 3 pages). NULL if not known
**                  trusted_mask     - Bitwise OR of services that do not
**                                     require authorization. (array of UINT32)
**                  link_key         - Connection link key. NULL if unknown.
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
BOOLEAN BTM_SecAddDevice (BD_ADDR bd_addr, DEV_CLASS dev_class, BD_NAME bd_name,
                          UINT8 *features, UINT32 trusted_mask[],
                          LINK_KEY link_key, UINT8 key_type, tBTM_IO_CAP io_cap,
                          UINT8 pin_length, UINT8 sc_support)
{
#if (SMP_INCLUDED == TRUE)
    tBTM_SEC_DEV_REC  *p_dev_rec;
    int               i, j;
    BOOLEAN           found = FALSE;

    BTM_TRACE_API("%s, link key type:%x\n", __FUNCTION__, key_type);
    p_dev_rec = btm_find_dev (bd_addr);
    if (!p_dev_rec) {
        /* There is no device record, allocate one.
         * If we can not find an empty spot for this one, let it fail. */
        if (list_length(btm_cb.p_sec_dev_rec_list) < BTM_SEC_MAX_DEVICE_RECORDS) {
	    p_dev_rec = (tBTM_SEC_DEV_REC *)osi_malloc(sizeof(tBTM_SEC_DEV_REC));
	    if(p_dev_rec) {
	        list_append(btm_cb.p_sec_dev_rec_list, p_dev_rec);
                /* Mark this record as in use and initialize */
                memset (p_dev_rec, 0, sizeof (tBTM_SEC_DEV_REC));
                p_dev_rec->sec_flags = BTM_SEC_IN_USE;
                memcpy (p_dev_rec->bd_addr, bd_addr, BD_ADDR_LEN);
                p_dev_rec->hci_handle = BTM_GetHCIConnHandle (bd_addr, BT_TRANSPORT_BR_EDR);
                p_dev_rec->ble_hci_handle = BTM_GetHCIConnHandle (bd_addr, BT_TRANSPORT_LE);
                p_dev_rec->enc_mode = BTM_ENC_MODE_UNKNOWN;

#if BLE_INCLUDED == TRUE
                /* use default value for background connection params */
                /* update conn params, use default value for background connection params */
                memset(&p_dev_rec->conn_params, 0xff, sizeof(tBTM_LE_CONN_PRAMS));
#endif
            }
        }

        if (!p_dev_rec) {
            return (FALSE);
        }
    }
    p_dev_rec->bond_type = BOND_TYPE_UNKNOWN;           /* Default value */
    p_dev_rec->timestamp = btm_cb.dev_rec_count++;
    p_dev_rec->remote_secure_connection_previous_state = sc_support;

    if (dev_class) {
        memcpy (p_dev_rec->dev_class, dev_class, DEV_CLASS_LEN);
    }

    memset(p_dev_rec->sec_bd_name, 0, sizeof(tBTM_BD_NAME));

    if (bd_name && bd_name[0]) {
        p_dev_rec->sec_flags |= BTM_SEC_NAME_KNOWN;
        BCM_STRNCPY_S ((char *)p_dev_rec->sec_bd_name, (char *)bd_name, BTM_MAX_REM_BD_NAME_LEN);
    }

    p_dev_rec->num_read_pages = 0;
    if (features) {
        memcpy (p_dev_rec->features, features, sizeof (p_dev_rec->features));
        for (i = HCI_EXT_FEATURES_PAGE_MAX; i >= 0; i--) {
            for (j = 0; j < HCI_FEATURE_BYTES_PER_PAGE; j++) {
                if (p_dev_rec->features[i][j] != 0) {
                    found = TRUE;
                    break;
                }
            }
            if (found) {
                p_dev_rec->num_read_pages = i + 1;
                break;
            }
        }
    } else {
        memset (p_dev_rec->features, 0, sizeof (p_dev_rec->features));
    }

    BTM_SEC_COPY_TRUSTED_DEVICE(trusted_mask, p_dev_rec->trusted_mask);

    if (link_key) {
        BTM_TRACE_EVENT ("BTM_SecAddDevice()  BDA: %02x:%02x:%02x:%02x:%02x:%02x\n",
                         bd_addr[0], bd_addr[1], bd_addr[2],
                         bd_addr[3], bd_addr[4], bd_addr[5]);
        p_dev_rec->sec_flags |= BTM_SEC_LINK_KEY_KNOWN;
        memcpy (p_dev_rec->link_key, link_key, LINK_KEY_LEN);
        p_dev_rec->link_key_type = key_type;
        p_dev_rec->pin_code_length = pin_length;

        if (pin_length >= 16 ||
                key_type == BTM_LKEY_TYPE_AUTH_COMB ||
                key_type == BTM_LKEY_TYPE_AUTH_COMB_P_256) {
            // Set the fiag if the link key was made by using either a 16 digit
            // pin or MITM.
            p_dev_rec->sec_flags |= BTM_SEC_16_DIGIT_PIN_AUTHED;
        }
    }

#if defined(BTIF_MIXED_MODE_INCLUDED) && (BTIF_MIXED_MODE_INCLUDED == TRUE)
    if (key_type  < BTM_MAX_PRE_SM4_LKEY_TYPE) {
        p_dev_rec->sm4 = BTM_SM4_KNOWN;
    } else {
        p_dev_rec->sm4 = BTM_SM4_TRUE;
    }
#endif

    p_dev_rec->rmt_io_caps = io_cap;
    p_dev_rec->device_type |= BT_DEVICE_TYPE_BREDR;
#endif  ///SMP_INCLUDED == TRUE
    return (TRUE);
}


/*******************************************************************************
**
** Function         BTM_SecDeleteDevice
**
** Description      Free resources associated with the device.
**
** Parameters:      bd_addr          - BD address of the peer
**                  transport        - BT_TRANSPORT_BR_EDR or BT_TRANSPORT_LE
**
** Returns          TRUE if removed OK, FALSE if not found or ACL link is active
**
*******************************************************************************/
BOOLEAN BTM_SecDeleteDevice (BD_ADDR bd_addr, tBT_TRANSPORT transport)
{
    tBTM_SEC_DEV_REC *p_dev_rec;

    if (BTM_IsAclConnectionUp(bd_addr, transport)) {
        BTM_TRACE_WARNING("%s FAILED: Cannot Delete when connection is active\n", __func__);
        return FALSE;
    }

    if ((p_dev_rec = btm_find_dev(bd_addr)) != NULL) {
        /* Tell controller to get rid of the link key, if it has one stored */
        BTM_DeleteStoredLinkKey (p_dev_rec->bd_addr, NULL);

        btm_sec_free_dev(p_dev_rec, transport);
    }

#if (BLE_SMP_ID_RESET_ENABLE == TRUE)
    /*
     * There are tracking risks associated with using a fixed or static IRK.
     * A best-practices approach, when all pairing and bonding records are deleted,
     * assign a new randomly-generated IRK.
     */
    if (list_is_empty(btm_cb.p_sec_dev_rec_list)) {
        btm_ble_reset_id();
    }
#endif

    return TRUE;
}

/*******************************************************************************
**
** Function         BTM_SecClearSecurityFlags
**
** Description      Reset the security flags (mark as not-paired) for a given
**                  remove device.
**
*******************************************************************************/
extern void BTM_SecClearSecurityFlags (BD_ADDR bd_addr)
{
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(bd_addr);
    if (p_dev_rec == NULL) {
        return;
    }

    p_dev_rec->sec_flags = 0;
    p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;
    p_dev_rec->sm4 = BTM_SM4_UNKNOWN;
}


/*******************************************************************************
**
** Function         BTM_SecReadDevName
**
** Description      Looks for the device name in the security database for the
**                  specified BD address.
**
** Returns          Pointer to the name or NULL
**
*******************************************************************************/
char *BTM_SecReadDevName (BD_ADDR bd_addr)
{
    char *p_name = NULL;
    tBTM_SEC_DEV_REC *p_srec;

    if ((p_srec = btm_find_dev(bd_addr)) != NULL) {
        p_name = (char *)p_srec->sec_bd_name;
    }

    return (p_name);
}


/*******************************************************************************
**
** Function         btm_find_sec_dev_in_list
**
** Description      Look for the record in the device database for the record
**                  with specified address
**
** Returns          Pointer to the record or NULL
**
*******************************************************************************/
BOOLEAN btm_find_sec_dev_in_list (void *p_node_data, void *context)
{
    tBTM_SEC_DEV_REC *p_sec_dev = (tBTM_SEC_DEV_REC *)p_node_data;
    BOOLEAN ret = TRUE;
    BOOLEAN dev_free = !(p_sec_dev->sec_flags & BTM_SEC_IN_USE);
    tSecDevContext *p_context = (tSecDevContext *)context;

    if (dev_free == p_context->free_check) {
        switch (p_context->type) {
            case SEC_DEV_BDA:
	        if (!memcmp(p_context->context.p_bd_addr, p_sec_dev->bd_addr, BD_ADDR_LEN)) {
		    ret = FALSE;
		}
	    break;
            case SEC_DEV_HDL:
	        if (p_context->context.handle == p_sec_dev->hci_handle
#if BLE_INCLUDED == TRUE
                    || (p_context->context.handle == p_sec_dev->ble_hci_handle)
#endif
		    ) {
		    ret = FALSE;
		}
	    break;
#if BLE_PRIVACY_SPT == TRUE
	    case SEC_DEV_ID_ADDR:
	        if (!memcmp(p_context->context.p_bd_addr, p_sec_dev->ble.static_addr, BD_ADDR_LEN)) {
		    ret = FALSE;
		}
	    break;
#endif //BLE_PRIVACY_SPT == TRUE
            case SEC_DEV_BTDM_BDA:
	       if (!memcmp(p_context->context.p_bd_addr, p_sec_dev->bd_addr, BD_ADDR_LEN)) {
	           ret = FALSE;
	       }
#if BLE_INCLUDED == TRUE
               // If a LE random address is looking for device record
               if (!memcmp(p_sec_dev->ble.pseudo_addr, p_context->context.p_bd_addr, BD_ADDR_LEN)) {
                   ret = FALSE;
               }

               if (btm_ble_addr_resolvable(p_context->context.p_bd_addr, p_sec_dev)) {
                   ret = FALSE;
               }
#endif
	    break;
	default:
	    break;
	}
    }
    return ret;
}

/*******************************************************************************
**
** Function         btm_sec_alloc_dev
**
** Description      Look for the record in the device database for the record
**                  with specified address
**
** Returns          Pointer to the record or NULL
**
*******************************************************************************/
tBTM_SEC_DEV_REC *btm_sec_alloc_dev (BD_ADDR bd_addr)
{
    tBTM_SEC_DEV_REC *p_dev_rec = NULL;
    tBTM_SEC_DEV_REC *p_dev_new_rec = NULL;
    tBTM_SEC_DEV_REC *p_dev_old_rec = NULL;
    tBTM_INQ_INFO    *p_inq_info;
    list_node_t      *p_node    = NULL;
    BOOLEAN           new_entry_found  = FALSE;
    BOOLEAN           old_entry_found  = FALSE;
    BOOLEAN           malloc_new_entry = FALSE;
    BTM_TRACE_EVENT ("btm_sec_alloc_dev\n");
    for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
        p_dev_old_rec = list_node(p_node);
        /* look for old entry which match the bd_addr and the BTM_SEC_IN_USE is cleared */
        if (!(p_dev_old_rec->sec_flags & BTM_SEC_IN_USE) &&
            (!memcmp (p_dev_old_rec->bd_addr, bd_addr, BD_ADDR_LEN))) {
            old_entry_found = TRUE;
            BTM_TRACE_EVENT ("btm_sec_alloc_dev old device found\n");
            break;
        }
    }
    for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
        p_dev_new_rec = list_node(p_node);
        /* find the first entry whose BTM_SEC_IN_USE is cleared */
        if (!(p_dev_new_rec->sec_flags & BTM_SEC_IN_USE)) {
            new_entry_found = TRUE;
            break;
        }
    }
    if (!new_entry_found) {
        /* We can not find new device. We need malloc a new one if p_sec_dev_rec_list is not full */
        if (list_length(btm_cb.p_sec_dev_rec_list) < BTM_SEC_MAX_DEVICE_RECORDS){
            p_dev_new_rec = (tBTM_SEC_DEV_REC *)osi_malloc(sizeof(tBTM_SEC_DEV_REC));
            if (p_dev_new_rec) {
                new_entry_found = TRUE;
                malloc_new_entry = TRUE;
            } else {
                return NULL;
            }
        }
    }
    if (!new_entry_found) {
        p_dev_rec = btm_find_oldest_dev();
    } else {
        /* if the old device entry not present go with new entry */
        if (old_entry_found) {
            p_dev_rec = p_dev_old_rec;
            if (malloc_new_entry) {
                osi_free(p_dev_new_rec);
            }
        } else {
            if (malloc_new_entry) {
                list_append(btm_cb.p_sec_dev_rec_list, p_dev_new_rec);
            }
            p_dev_rec = p_dev_new_rec;
        }
    }
    memset (p_dev_rec, 0, sizeof (tBTM_SEC_DEV_REC));

    p_dev_rec->bond_type = BOND_TYPE_UNKNOWN;           /* Default value */
    p_dev_rec->sec_flags = BTM_SEC_IN_USE;

    /* Check with the BT manager if details about remote device are known */
    /* outgoing connection */
    if ((p_inq_info = BTM_InqDbRead(bd_addr)) != NULL) {
        memcpy (p_dev_rec->dev_class, p_inq_info->results.dev_class, DEV_CLASS_LEN);

#if BLE_INCLUDED == TRUE
        p_dev_rec->device_type = p_inq_info->results.device_type;
        p_dev_rec->ble.ble_addr_type = p_inq_info->results.ble_addr_type;

        /* update conn params, use default value for background connection params */
        memset(&p_dev_rec->conn_params, 0xff, sizeof(tBTM_LE_CONN_PRAMS));
#endif
    } else {
#if BLE_INCLUDED == TRUE
        /* update conn params, use default value for background connection params */
        memset(&p_dev_rec->conn_params, 0xff, sizeof(tBTM_LE_CONN_PRAMS));
#endif

        if (!memcmp (bd_addr, btm_cb.connecting_bda, BD_ADDR_LEN)) {
            memcpy (p_dev_rec->dev_class, btm_cb.connecting_dc, DEV_CLASS_LEN);
        }
    }

    memcpy (p_dev_rec->bd_addr, bd_addr, BD_ADDR_LEN);

#if BLE_INCLUDED == TRUE
    p_dev_rec->ble_hci_handle = BTM_GetHCIConnHandle (bd_addr, BT_TRANSPORT_LE);
#endif
    p_dev_rec->hci_handle = BTM_GetHCIConnHandle (bd_addr, BT_TRANSPORT_BR_EDR);
    p_dev_rec->timestamp = btm_cb.dev_rec_count++;

    return (p_dev_rec);
}


/*******************************************************************************
**
** Function         btm_sec_free_dev
**
** Description      Mark device record as not used
**
*******************************************************************************/
void btm_sec_free_dev (tBTM_SEC_DEV_REC *p_dev_rec, tBT_TRANSPORT transport)
{
    if (transport == BT_TRANSPORT_BR_EDR) {
        memset(p_dev_rec->link_key, 0, LINK_KEY_LEN);
        p_dev_rec->sec_flags &= ~(BTM_SEC_AUTHORIZED | BTM_SEC_AUTHENTICATED
                                | BTM_SEC_ENCRYPTED | BTM_SEC_NAME_KNOWN
                                | BTM_SEC_LINK_KEY_KNOWN | BTM_SEC_LINK_KEY_AUTHED
                                | BTM_SEC_ROLE_SWITCHED | BTM_SEC_16_DIGIT_PIN_AUTHED);
    } else if (transport == BT_TRANSPORT_LE) {
        p_dev_rec->bond_type = BOND_TYPE_UNKNOWN;
        p_dev_rec->sec_flags &= ~(BTM_SEC_LE_AUTHENTICATED | BTM_SEC_LE_ENCRYPTED
                                | BTM_SEC_LE_NAME_KNOWN | BTM_SEC_LE_LINK_KEY_KNOWN
                                | BTM_SEC_LE_LINK_KEY_AUTHED | BTM_SEC_ROLE_SWITCHED);
#if BLE_INCLUDED == TRUE
        /* Clear out any saved BLE keys */
        btm_sec_clear_ble_keys (p_dev_rec);
#endif
    } else {
        p_dev_rec->bond_type = BOND_TYPE_UNKNOWN;
        memset(p_dev_rec->link_key, 0, LINK_KEY_LEN);
        p_dev_rec->sec_flags = 0;

#if BLE_INCLUDED == TRUE
        /* Clear out any saved BLE keys */
        btm_sec_clear_ble_keys (p_dev_rec);
#endif
    }
    /* No BLE keys and BT keys, clear the sec_flags */
    if(p_dev_rec->sec_flags == BTM_SEC_IN_USE) {
        p_dev_rec->sec_flags = 0;
    }
    list_remove(btm_cb.p_sec_dev_rec_list, p_dev_rec);
}

/*******************************************************************************
**
** Function         btm_dev_support_switch
**
** Description      This function is called by the L2CAP to check if remote
**                  device supports role switch
**
** Parameters:      bd_addr       - Address of the peer device
**
** Returns          TRUE if device is known and role switch is supported
**
*******************************************************************************/
BOOLEAN btm_dev_support_switch (BD_ADDR bd_addr)
{
    tBTM_SEC_DEV_REC  *p_dev_rec;
    UINT8   xx;
    BOOLEAN feature_empty = TRUE;

#if BTM_SCO_INCLUDED == TRUE
    /* Role switch is not allowed if a SCO is up */
    if (btm_is_sco_active_by_bdaddr(bd_addr)) {
        return (FALSE);
    }
#endif
    p_dev_rec = btm_find_dev (bd_addr);
    if (p_dev_rec && controller_get_interface()->supports_master_slave_role_switch()) {
        if (HCI_SWITCH_SUPPORTED(p_dev_rec->features[HCI_EXT_FEATURES_PAGE_0])) {
            BTM_TRACE_DEBUG("btm_dev_support_switch return TRUE (feature found)\n");
            return (TRUE);
        }

        /* If the feature field is all zero, we never received them */
        for (xx = 0 ; xx < BD_FEATURES_LEN ; xx++) {
            if (p_dev_rec->features[HCI_EXT_FEATURES_PAGE_0][xx] != 0x00) {
                feature_empty = FALSE; /* at least one is != 0 */
                break;
            }
        }

        /* If we don't know peer's capabilities, assume it supports Role-switch */
        if (feature_empty) {
            BTM_TRACE_DEBUG("btm_dev_support_switch return TRUE (feature empty)\n");
            return (TRUE);
        }
    }

    BTM_TRACE_DEBUG("btm_dev_support_switch return FALSE\n");
    return (FALSE);
}

/*******************************************************************************
**
** Function         btm_find_dev_by_handle
**
** Description      Look for the record in the device database for the record
**                  with specified handle
**
** Returns          Pointer to the record or NULL
**
*******************************************************************************/
tBTM_SEC_DEV_REC *btm_find_dev_by_handle (UINT16 handle)
{
    tBTM_SEC_DEV_REC *p_dev_rec = NULL;
    list_node_t *p_node         = NULL;
    tSecDevContext context;
    context.type                = SEC_DEV_HDL;
    context.context.handle      = handle;
    context.free_check          = FALSE;

    p_node = list_foreach(btm_cb.p_sec_dev_rec_list, btm_find_sec_dev_in_list, &context);
    if (p_node) {
        p_dev_rec = list_node(p_node);
    }
    return (p_dev_rec);
}
/*******************************************************************************
**
** Function         btm_find_dev
**
** Description      Look for the record in the device database for the record
**                  with specified BD address
**
** Returns          Pointer to the record or NULL
**
*******************************************************************************/
tBTM_SEC_DEV_REC *btm_find_dev(BD_ADDR bd_addr)
{
    if(bd_addr) {
        list_node_t *p_node        = NULL;
        tSecDevContext context;
        context.type               = SEC_DEV_BTDM_BDA;
        context.context.p_bd_addr  = bd_addr;
        context.free_check         = FALSE;
        p_node = list_foreach(btm_cb.p_sec_dev_rec_list, btm_find_sec_dev_in_list, &context);
	if (p_node) {
	    return(list_node(p_node));
	}
    }
    return (NULL);
}
/*******************************************************************************
**
** Function         btm_consolidate_dev
**
** Description      combine security records if identified as same peer
**
** Returns          none
**
*******************************************************************************/
void btm_consolidate_dev(tBTM_SEC_DEV_REC *p_target_rec)
{
#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
    tBTM_SEC_DEV_REC *p_dev_rec = NULL;
    tBTM_SEC_DEV_REC temp_rec = *p_target_rec;
    list_node_t *p_node = NULL;
    BTM_TRACE_DEBUG("%s\n", __func__);

    for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
        p_dev_rec = list_node(p_node);
        if (p_target_rec != p_dev_rec && p_dev_rec->sec_flags & BTM_SEC_IN_USE) {
            if (!memcmp (p_dev_rec->bd_addr, p_target_rec->bd_addr, BD_ADDR_LEN)) {
                memcpy(p_target_rec, p_dev_rec, sizeof(tBTM_SEC_DEV_REC));
                p_target_rec->ble = temp_rec.ble;
                p_target_rec->ble_hci_handle = temp_rec.ble_hci_handle;
                p_target_rec->enc_key_size = temp_rec.enc_key_size;
                p_target_rec->conn_params = temp_rec.conn_params;
                p_target_rec->device_type |= temp_rec.device_type;
                p_target_rec->sec_flags |= temp_rec.sec_flags;

                p_target_rec->new_encryption_key_is_p256 = temp_rec.new_encryption_key_is_p256;
                p_target_rec->no_smp_on_br = temp_rec.no_smp_on_br;
                p_target_rec->bond_type = temp_rec.bond_type;
                /* Remove the unused device from the list */
                list_remove(btm_cb.p_sec_dev_rec_list, p_dev_rec);
                break;
            }

            /* an RPA device entry is a duplicate of the target record */
            if (btm_ble_addr_resolvable(p_dev_rec->bd_addr, p_target_rec)) {
                if (memcmp(p_target_rec->ble.pseudo_addr, p_dev_rec->bd_addr, BD_ADDR_LEN) == 0) {
                    p_target_rec->ble.ble_addr_type = p_dev_rec->ble.ble_addr_type;
                    p_target_rec->device_type |= p_dev_rec->device_type;
                    /* Remove the unused device from the list */
                    list_remove(btm_cb.p_sec_dev_rec_list, p_dev_rec);
		}
                break;
            }
        }
    }
#endif
}

/*******************************************************************************
**
** Function         btm_find_or_alloc_dev
**
** Description      Look for the record in the device database for the record
**                  with specified BD address
**
** Returns          Pointer to the record or NULL
**
*******************************************************************************/
tBTM_SEC_DEV_REC *btm_find_or_alloc_dev (BD_ADDR bd_addr)
{
    tBTM_SEC_DEV_REC *p_dev_rec;
    BTM_TRACE_EVENT ("btm_find_or_alloc_dev\n");
    if ((p_dev_rec = btm_find_dev (bd_addr)) == NULL) {

        /* Allocate a new device record or reuse the oldest one */
        p_dev_rec = btm_sec_alloc_dev (bd_addr);
    }
    return (p_dev_rec);
}

/*******************************************************************************
**
** Function         btm_find_oldest_dev
**
** Description      Locates the oldest device in use. It first looks for
**                  the oldest non-paired device.  If all devices are paired it
**                  deletes the oldest paired device.
**
** Returns          Pointer to the record or NULL
**
*******************************************************************************/
tBTM_SEC_DEV_REC *btm_find_oldest_dev (void)
{
    tBTM_SEC_DEV_REC *p_dev_rec = NULL;
    tBTM_SEC_DEV_REC *p_oldest  = NULL;
    list_node_t *p_node              = NULL;
    UINT32 old_ts                    = 0xFFFFFFFF;

    /* First look for the non-paired devices for the oldest entry */
    for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
	p_dev_rec = list_node(p_node);
        if (((p_dev_rec->sec_flags & BTM_SEC_IN_USE) == 0)
                || ((p_dev_rec->sec_flags & (BTM_SEC_LINK_KEY_KNOWN | BTM_SEC_LE_LINK_KEY_KNOWN)) != 0)) {
            continue;    /* Device is paired so skip it */
        }

        if (p_dev_rec->timestamp < old_ts) {
            p_oldest = p_dev_rec;
            old_ts   = p_dev_rec->timestamp;
        }
    }

    if (old_ts != 0xFFFFFFFF) {
        return (p_oldest);
    }

    /* All devices are paired; find the oldest */
    for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
        if ((p_dev_rec->sec_flags & BTM_SEC_IN_USE) == 0) {
            continue;
        }

        if (p_dev_rec->timestamp < old_ts) {
            p_oldest = p_dev_rec;
            old_ts   = p_dev_rec->timestamp;
        }
    }
    return (p_oldest);
}
/*******************************************************************************
**
** Function         btm_get_bond_type_dev
**
** Description      Get the bond type for a device in the device database
**                  with specified BD address
**
** Returns          The device bond type if known, otherwise BOND_TYPE_UNKNOWN
**
*******************************************************************************/
tBTM_BOND_TYPE btm_get_bond_type_dev(BD_ADDR bd_addr)
{
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(bd_addr);

    if (p_dev_rec == NULL) {
        return BOND_TYPE_UNKNOWN;
    }

    return p_dev_rec->bond_type;
}

/*******************************************************************************
**
** Function         btm_set_bond_type_dev
**
** Description      Set the bond type for a device in the device database
**                  with specified BD address
**
** Returns          TRUE on success, otherwise FALSE
**
*******************************************************************************/
BOOLEAN btm_set_bond_type_dev(BD_ADDR bd_addr, tBTM_BOND_TYPE bond_type)
{
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(bd_addr);

    if (p_dev_rec == NULL) {
        return FALSE;
    }

    p_dev_rec->bond_type = bond_type;
    return TRUE;
}

/*******************************************************************************
**
** Function         btm_sec_dev_init
**
** Description      Create new linked list for dynamic allocation on sec_dev_rec
**
*******************************************************************************/
void btm_sec_dev_init(void)
{
    btm_cb.p_sec_dev_rec_list = list_new(osi_free_func);
}

/*******************************************************************************
**
** Function         btm_sec_dev_free
**
** Description      Delete sec_dev_rec list when btm_cb is being released
**
*******************************************************************************/
void btm_sec_dev_free(void)
{
    list_free(btm_cb.p_sec_dev_rec_list);
}
