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
 *  This file contains functions for BLE device control utilities, and LE
 *  security functions.
 *
 ******************************************************************************/
#include "common/bt_target.h"

#include <string.h>

#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "btm_int.h"
#include "stack/btm_ble_api.h"
#include "stack/smp_api.h"
#include "l2c_int.h"
#include "stack/gap_api.h"
//#include "bt_utils.h"
#include "device/controller.h"

//#define LOG_TAG "bt_btm_ble"
//#include "osi/include/log.h"
#if BLE_INCLUDED == TRUE
extern void BTM_UpdateAddrInfor(uint8_t addr_type, BD_ADDR bda);
#if SMP_INCLUDED == TRUE
// The temp variable to pass parameter between functions when in the connected event callback.
static BOOLEAN temp_enhanced = FALSE;
extern BOOLEAN aes_cipher_msg_auth_code(BT_OCTET16 key, UINT8 *input, UINT16 length,
                                        UINT16 tlen, UINT8 *p_signature);
extern void smp_link_encrypted(BD_ADDR bda, UINT8 encr_enable);
extern BOOLEAN smp_proc_ltk_request(BD_ADDR bda);
#endif
extern void gatt_notify_enc_cmpl(BD_ADDR bd_addr);
/*******************************************************************************/
/* External Function to be called by other modules                             */
/*******************************************************************************/
/********************************************************
**
** Function         BTM_SecAddBleDevice
**
** Description      Add/modify device.  This function will be normally called
**                  during host startup to restore all required information
**                  for a LE device stored in the NVRAM.
**
** Parameters:      bd_addr          - BD address of the peer
**                  bd_name          - Name of the peer device.  NULL if unknown.
**                  dev_type         - Remote device's device type.
**                  addr_type        - LE device address type.
**                  auth_mode        - auth mode
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
BOOLEAN BTM_SecAddBleDevice (BD_ADDR bd_addr, BD_NAME bd_name, tBT_DEVICE_TYPE dev_type,
                             tBLE_ADDR_TYPE addr_type, UINT32 auth_mode)
{
    tBTM_SEC_DEV_REC  *p_dev_rec;
    tBTM_INQ_INFO      *p_info = NULL;

    BTM_TRACE_DEBUG ("BTM_SecAddBleDevice dev_type=0x%x", dev_type);
    p_dev_rec = btm_find_dev (bd_addr);

    if (!p_dev_rec) {
        BTM_TRACE_DEBUG("Add a new device");

        /* There is no device record, allocate one.
         * If we can not find an empty spot for this one, let it fail. */
        if (list_length(btm_cb.p_sec_dev_rec_list) < BTM_SEC_MAX_DEVICE_RECORDS) {
	    p_dev_rec = (tBTM_SEC_DEV_REC *)osi_malloc(sizeof(tBTM_SEC_DEV_REC));
	    if(p_dev_rec) {
		list_append(btm_cb.p_sec_dev_rec_list, p_dev_rec);
                BTM_TRACE_DEBUG ("allocate a new dev rec idx=0x%x\n", list_length(btm_cb.p_sec_dev_rec_list));

                /* Mark this record as in use and initialize */
                memset (p_dev_rec, 0, sizeof (tBTM_SEC_DEV_REC));
                p_dev_rec->sec_flags = BTM_SEC_IN_USE;
                memcpy (p_dev_rec->bd_addr, bd_addr, BD_ADDR_LEN);
                p_dev_rec->hci_handle = BTM_GetHCIConnHandle (bd_addr, BT_TRANSPORT_BR_EDR);
                p_dev_rec->ble_hci_handle = BTM_GetHCIConnHandle (bd_addr, BT_TRANSPORT_LE);

                /* update conn params, use default value for background connection params */
                p_dev_rec->conn_params.min_conn_int     =
                    p_dev_rec->conn_params.max_conn_int     =
                        p_dev_rec->conn_params.supervision_tout =
                            p_dev_rec->conn_params.slave_latency    = BTM_BLE_CONN_PARAM_UNDEF;

                BTM_TRACE_DEBUG ("hci_handl=0x%x ",  p_dev_rec->ble_hci_handle );
            }
        }

        if (!p_dev_rec) {
            return (FALSE);
        }
    } else {
        BTM_TRACE_DEBUG("Device already exist");
    }

    memset(p_dev_rec->sec_bd_name, 0, sizeof(tBTM_BD_NAME));

    if (bd_name && bd_name[0]) {
        p_dev_rec->sec_flags |= BTM_SEC_NAME_KNOWN;
        BCM_STRNCPY_S ((char *)p_dev_rec->sec_bd_name, (char *)bd_name, BTM_MAX_REM_BD_NAME_LEN);
    }
    p_dev_rec->device_type |= dev_type;
    p_dev_rec->ble.ble_addr_type = addr_type;
    p_dev_rec->ble.auth_mode = auth_mode;

    memcpy (p_dev_rec->ble.pseudo_addr, bd_addr, BD_ADDR_LEN);
    /* sync up with the Inq Data base*/
    p_info = BTM_InqDbRead(bd_addr);
    if (p_info) {
        p_info->results.ble_addr_type = p_dev_rec->ble.ble_addr_type ;
        p_info->results.device_type = p_dev_rec->device_type;
        BTM_TRACE_DEBUG ("InqDb  device_type =0x%x  addr_type=0x%x",
                         p_info->results.device_type, p_info->results.ble_addr_type);
    }
    return (TRUE);
}
#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         BTM_SecAddBleKey
**
** Description      Add/modify LE device information.  This function will be
**                  normally called during host startup to restore all required
**                  information stored in the NVRAM.
**
** Parameters:      bd_addr          - BD address of the peer
**                  p_le_key         - LE key values.
**                  key_type         - LE SMP key type.
*
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
#if SMP_INCLUDED == TRUE
BOOLEAN BTM_SecAddBleKey (BD_ADDR bd_addr, tBTM_LE_KEY_VALUE *p_le_key, tBTM_LE_KEY_TYPE key_type)
{
    tBTM_SEC_DEV_REC  *p_dev_rec;
    BTM_TRACE_DEBUG ("BTM_SecAddBleKey");
    p_dev_rec = btm_find_dev (bd_addr);
    if (!p_dev_rec || !p_le_key ||
            (key_type != BTM_LE_KEY_PENC && key_type != BTM_LE_KEY_PID &&
             key_type != BTM_LE_KEY_PCSRK && key_type != BTM_LE_KEY_LENC &&
             key_type != BTM_LE_KEY_LCSRK && key_type != BTM_LE_KEY_LID)) {
        BTM_TRACE_WARNING ("BTM_SecAddBleKey()  Wrong Type, or No Device record \
                        for bdaddr: %08x%04x, Type: %d",
                           (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                           (bd_addr[4] << 8) + bd_addr[5], key_type);
        return (FALSE);
    }

    BTM_TRACE_DEBUG ("BTM_SecAddLeKey()  BDA: %08x%04x, Type: 0x%02x",
                     (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                     (bd_addr[4] << 8) + bd_addr[5], key_type);

    btm_sec_save_le_key (bd_addr, key_type, p_le_key, FALSE);

#if (BLE_PRIVACY_SPT == TRUE)
    if (key_type == BTM_LE_KEY_PID || key_type == BTM_LE_KEY_LID) {
        btm_ble_resolving_list_load_dev (p_dev_rec);
    }
#endif


    return (TRUE);
}
#endif


/*******************************************************************************
**
** Function         BTM_BleLoadLocalKeys
**
** Description      Local local identity key, encryption root or sign counter.
**
** Parameters:      key_type: type of key, can be BTM_BLE_KEY_TYPE_ID, BTM_BLE_KEY_TYPE_ER
**                            or BTM_BLE_KEY_TYPE_COUNTER.
**                  p_key: pointer to the key.
*
** Returns          non2.
**
*******************************************************************************/
void BTM_BleLoadLocalKeys(UINT8 key_type, tBTM_BLE_LOCAL_KEYS *p_key)
{
    tBTM_DEVCB *p_devcb = &btm_cb.devcb;
    BTM_TRACE_DEBUG ("%s", __func__);
    if (p_key != NULL) {
        switch (key_type) {
        case BTM_BLE_KEY_TYPE_ID:
            memcpy(&p_devcb->id_keys, &p_key->id_keys, sizeof(tBTM_BLE_LOCAL_ID_KEYS));
            break;

        case BTM_BLE_KEY_TYPE_ER:
            memcpy(p_devcb->ble_encryption_key_value, p_key->er, sizeof(BT_OCTET16));
            break;

        default:
            BTM_TRACE_ERROR("unknown local key type: %d", key_type);
            break;
        }
    }
}

/*******************************************************************************
**
** Function         BTM_GetDeviceEncRoot
**
** Description      This function is called to read the local device encryption
**                  root.
**
** Returns          void
**                  the local device ER is copied into ble_encr_key_value
**
*******************************************************************************/
void BTM_GetDeviceEncRoot (BT_OCTET16 ble_encr_key_value)
{
    BTM_TRACE_DEBUG ("%s", __func__);
    memcpy (ble_encr_key_value, btm_cb.devcb.ble_encryption_key_value, BT_OCTET16_LEN);
}

/*******************************************************************************
**
** Function         BTM_GetDeviceIDRoot
**
** Description      This function is called to read the local device identity
**                  root.
**
** Returns          void
**                  the local device IR is copied into irk
**
*******************************************************************************/
void BTM_GetDeviceIDRoot (BT_OCTET16 irk)
{
    BTM_TRACE_DEBUG ("BTM_GetDeviceIDRoot ");

    memcpy (irk, btm_cb.devcb.id_keys.irk, BT_OCTET16_LEN);
}

/*******************************************************************************
**
** Function         BTM_GetDeviceDHK
**
** Description      This function is called to read the local device DHK.
**
** Returns          void
**                  the local device DHK is copied into dhk
**
*******************************************************************************/
void BTM_GetDeviceDHK (BT_OCTET16 dhk)
{
    BTM_TRACE_DEBUG ("BTM_GetDeviceDHK");
    memcpy (dhk, btm_cb.devcb.id_keys.dhk, BT_OCTET16_LEN);
}

/*******************************************************************************
**
** Function         BTM_ReadConnectionAddr
**
** Description      This function is called to get the local device address information
**                  .
**
** Returns          void
**
*******************************************************************************/
void BTM_ReadConnectionAddr (BD_ADDR remote_bda, BD_ADDR local_conn_addr, tBLE_ADDR_TYPE *p_addr_type)
{
    tACL_CONN       *p_acl = btm_bda_to_acl(remote_bda, BT_TRANSPORT_LE);

    if (p_acl == NULL) {
        BTM_TRACE_ERROR("No connection exist!");
        return;
    }
    memcpy(local_conn_addr, p_acl->conn_addr, BD_ADDR_LEN);
    * p_addr_type = p_acl->conn_addr_type;

    BTM_TRACE_DEBUG ("BTM_ReadConnectionAddr address type: %d addr: 0x%02x",
                     p_acl->conn_addr_type, p_acl->conn_addr[0]);
}

/*******************************************************************************
**
** Function         BTM_IsBleConnection
**
** Description      This function is called to check if the connection handle
**                  for an LE link
**
** Returns          TRUE if connection is LE link, otherwise FALSE.
**
*******************************************************************************/
BOOLEAN BTM_IsBleConnection (UINT16 conn_handle)
{
#if (BLE_INCLUDED == TRUE)
    tACL_CONN            *p;

    BTM_TRACE_API ("BTM_IsBleConnection: conn_handle: %d", conn_handle);

    p = btm_handle_to_acl(conn_handle);
    if (!p) {
        return FALSE;
    }
    return (p->transport == BT_TRANSPORT_LE);
#else
    return FALSE;
#endif
}

/*******************************************************************************
**
** Function         BTM_ReadRemoteConnectionAddr
**
** Description      This function is read the remote device address currently used
**
** Parameters     pseudo_addr: pseudo random address available
**                conn_addr:connection address used
**                p_addr_type : BD Address type, Public or Random of the address used
**
** Returns          BOOLEAN , TRUE if connection to remote device exists, else FALSE
**
*******************************************************************************/
BOOLEAN BTM_ReadRemoteConnectionAddr(BD_ADDR pseudo_addr, BD_ADDR conn_addr,
                                     tBLE_ADDR_TYPE *p_addr_type)
{
    BOOLEAN         st = TRUE;
#if (BLE_PRIVACY_SPT == TRUE)
    tACL_CONN       *p = btm_bda_to_acl (pseudo_addr, BT_TRANSPORT_LE);

    if (p == NULL) {
        BTM_TRACE_ERROR("BTM_ReadRemoteConnectionAddr can not find connection"
                        " with matching address");
        return FALSE;
    }

    memcpy(conn_addr, p->active_remote_addr, BD_ADDR_LEN);
    *p_addr_type = p->active_remote_addr_type;
#else
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(pseudo_addr);

    memcpy(conn_addr, pseudo_addr, BD_ADDR_LEN);
    if (p_dev_rec != NULL) {
        *p_addr_type = p_dev_rec->ble.ble_addr_type;
    }
#endif
    return st;

}
/*******************************************************************************
**
** Function         BTM_SecurityGrant
**
** Description      This function is called to grant security process.
**
** Parameters       bd_addr - peer device bd address.
**                  res     - result of the operation BTM_SUCCESS if success.
**                            Otherwise, BTM_REPEATED_ATTEMPTS is too many attempts.
**
** Returns          None
**
*******************************************************************************/
void BTM_SecurityGrant(BD_ADDR bd_addr, UINT8 res)
{
#if SMP_INCLUDED == TRUE
    tSMP_STATUS res_smp = (res == BTM_SUCCESS) ? SMP_SUCCESS : SMP_REPEATED_ATTEMPTS;
    BTM_TRACE_DEBUG ("BTM_SecurityGrant");
    SMP_SecurityGrant(bd_addr, res_smp);
#endif
}

/*******************************************************************************
**
** Function         BTM_BlePasskeyReply
**
** Description      This function is called after Security Manager submitted
**                  passkey request to the application.
**
** Parameters:      bd_addr      - Address of the device for which passkey was requested
**                  res          - result of the operation BTM_SUCCESS if success
**                  key_len      - length in bytes of the Passkey
**                  p_passkey        - pointer to array with the passkey
**                  trusted_mask - bitwise OR of trusted services (array of UINT32)
**
*******************************************************************************/
void BTM_BlePasskeyReply (BD_ADDR bd_addr, UINT8 res, UINT32 passkey)
{
#if SMP_INCLUDED == TRUE
    tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_dev (bd_addr);
    tSMP_STATUS      res_smp = (res == BTM_SUCCESS) ? SMP_SUCCESS : SMP_PASSKEY_ENTRY_FAIL;

    if (p_dev_rec == NULL) {
        BTM_TRACE_ERROR("Passkey reply to Unknown device");
        return;
    }

    p_dev_rec->sec_flags   |= BTM_SEC_LE_AUTHENTICATED;
    BTM_TRACE_DEBUG ("BTM_BlePasskeyReply");
    SMP_PasskeyReply(bd_addr, res_smp, passkey);
#endif
}

/*******************************************************************************
**
** Function         BTM_BleSetStaticPasskey
**
** Description      This function is called to set static passkey
**
**
** Parameters:      add          - set static passkey when add is TRUE
**                                 clear static passkey when add is FALSE
**                  passkey      - static passkey
**
**
*******************************************************************************/
void BTM_BleSetStaticPasskey(BOOLEAN add, UINT32 passkey)
{
#if SMP_INCLUDED == TRUE
    SMP_SetStaticPasskey(add, passkey);
#endif
}

/*******************************************************************************
**
** Function         BTM_BleConfirmReply
**
** Description      This function is called after Security Manager submitted
**                  numeric comparison request to the application.
**
** Parameters:      bd_addr      - Address of the device with which numeric
**                                 comparison was requested
**                  res          - comparison result BTM_SUCCESS if success
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void BTM_BleConfirmReply (BD_ADDR bd_addr, UINT8 res)
{
    tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_dev (bd_addr);
    tSMP_STATUS      res_smp = (res == BTM_SUCCESS) ? SMP_SUCCESS : SMP_PASSKEY_ENTRY_FAIL;

    if (p_dev_rec == NULL) {
        BTM_TRACE_ERROR("Passkey reply to Unknown device");
        return;
    }

    p_dev_rec->sec_flags   |= BTM_SEC_LE_AUTHENTICATED;
    BTM_TRACE_DEBUG ("%s\n", __func__);
    SMP_ConfirmReply(bd_addr, res_smp);
}
#endif  ///SMP_INCLUDED == TRUE

/*******************************************************************************
**
** Function         BTM_BleOobDataReply
**
** Description      This function is called to provide the OOB data for
**                  SMP in response to BTM_LE_OOB_REQ_EVT
**
** Parameters:      bd_addr     - Address of the peer device
**                  res         - result of the operation SMP_SUCCESS if success
**                  p_data      - simple pairing Randomizer  C.
**
*******************************************************************************/
void BTM_BleOobDataReply(BD_ADDR bd_addr, UINT8 res, UINT8 len, UINT8 *p_data)
{
#if SMP_INCLUDED == TRUE
    tSMP_STATUS res_smp = (res == BTM_SUCCESS) ? SMP_SUCCESS : SMP_OOB_FAIL;
    tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_dev (bd_addr);

    BTM_TRACE_DEBUG ("BTM_BleOobDataReply");

    if (p_dev_rec == NULL) {
        BTM_TRACE_ERROR("BTM_BleOobDataReply() to Unknown device");
        return;
    }

    p_dev_rec->sec_flags |= BTM_SEC_LE_AUTHENTICATED;
    SMP_OobDataReply(bd_addr, res_smp, len, p_data);
#endif
}

/*******************************************************************************
**
** Function         BTM_BleSecureConnectionOobDataReply
**
** Description      This function is called to provide the OOB data for
**                  SMP in response to BTM_LE_SC_OOB_REQ_EVT when secure connection
**
** Parameters:      bd_addr     - Address of the peer device
**                  p_c         - pointer to Confirmation
**                  p_r         - pointer to Randomizer
**
*******************************************************************************/
void BTM_BleSecureConnectionOobDataReply(BD_ADDR bd_addr, UINT8 *p_c, UINT8 *p_r)
{
#if SMP_INCLUDED == TRUE
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (bd_addr);

    BTM_TRACE_DEBUG ("%s", __func__);

    if (p_dev_rec == NULL) {
        BTM_TRACE_ERROR("%s Unknown device", __func__);
        return;
    }

    p_dev_rec->sec_flags |= BTM_SEC_LE_AUTHENTICATED;

    tSMP_SC_OOB_DATA oob;
    memset(&oob, 0, sizeof(tSMP_SC_OOB_DATA));

    oob.peer_oob_data.present = true;
    memcpy(&oob.peer_oob_data.commitment, p_c, BT_OCTET16_LEN);
    memcpy(&oob.peer_oob_data.randomizer, p_r, BT_OCTET16_LEN);
    oob.peer_oob_data.addr_rcvd_from.type = p_dev_rec->ble.ble_addr_type;
    memcpy(oob.peer_oob_data.addr_rcvd_from.bda, bd_addr, BD_ADDR_LEN);

    SMP_SecureConnectionOobDataReply((UINT8 *)&oob);
#endif
}

/*******************************************************************************
**
** Function         BTM_BleSecureConnectionCreateOobData
**
** Description      This function is called to create the OOB data for
**                  SMP when secure connection
**
*******************************************************************************/
void BTM_BleSecureConnectionCreateOobData(void)
{
#if SMP_INCLUDED == TRUE
    BTM_TRACE_DEBUG ("%s", __func__);

    SMP_CreateLocalSecureConnectionsOobData();
#endif
}

/******************************************************************************
**
** Function         BTM_BleSetConnScanParams
**
** Description      Set scan parameter used in BLE connection request
**
** Parameters:      scan_interval: scan interval
**                  scan_window: scan window
**
** Returns          void
**
*******************************************************************************/
void BTM_BleSetConnScanParams (UINT32 scan_interval, UINT32 scan_window)
{
#if SMP_INCLUDED == TRUE
    tBTM_BLE_CB *p_ble_cb = &btm_cb.ble_ctr_cb;
    BOOLEAN     new_param = FALSE;

    if (BTM_BLE_ISVALID_PARAM(scan_interval, BTM_BLE_SCAN_INT_MIN, BTM_BLE_SCAN_INT_MAX) &&
            BTM_BLE_ISVALID_PARAM(scan_window, BTM_BLE_SCAN_WIN_MIN, BTM_BLE_SCAN_WIN_MAX)) {
        if (p_ble_cb->scan_int != scan_interval) {
            p_ble_cb->scan_int = scan_interval;
            new_param = TRUE;
        }

        if (p_ble_cb->scan_win != scan_window) {
            p_ble_cb->scan_win = scan_window;
            new_param = TRUE;
        }

        if (new_param && p_ble_cb->conn_state == BLE_BG_CONN) {
            btm_ble_suspend_bg_conn();
        }
    } else {
        BTM_TRACE_ERROR("Illegal Connection Scan Parameters");
    }
#endif
}

/********************************************************
**
** Function         BTM_BleSetPrefConnParams
**
** Description      Set a peripheral's preferred connection parameters
**
** Parameters:      bd_addr          - BD address of the peripheral
**                  scan_interval: scan interval
**                  scan_window: scan window
**                  min_conn_int     - minimum preferred connection interval
**                  max_conn_int     - maximum preferred connection interval
**                  slave_latency    - preferred slave latency
**                  supervision_tout - preferred supervision timeout
**
** Returns          void
**
*******************************************************************************/
void BTM_BleSetPrefConnParams (BD_ADDR bd_addr,
                               UINT16 min_conn_int, UINT16 max_conn_int,
                               UINT16 slave_latency, UINT16 supervision_tout)
{
    tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_or_alloc_dev (bd_addr);

    BTM_TRACE_API ("BTM_BleSetPrefConnParams min: %u  max: %u  latency: %u  \
                    tout: %u",
                   min_conn_int, max_conn_int, slave_latency, supervision_tout);

    if (BTM_BLE_ISVALID_PARAM(min_conn_int, BTM_BLE_CONN_INT_MIN, BTM_BLE_CONN_INT_MAX) &&
            BTM_BLE_ISVALID_PARAM(max_conn_int, BTM_BLE_CONN_INT_MIN, BTM_BLE_CONN_INT_MAX) &&
            BTM_BLE_ISVALID_PARAM(supervision_tout, BTM_BLE_CONN_SUP_TOUT_MIN, BTM_BLE_CONN_SUP_TOUT_MAX) &&
            (slave_latency <= BTM_BLE_CONN_LATENCY_MAX || slave_latency == BTM_BLE_CONN_PARAM_UNDEF)) {
        if (p_dev_rec) {
            /* expect conn int and stout and slave latency to be updated all together */
            if (min_conn_int != BTM_BLE_CONN_PARAM_UNDEF || max_conn_int != BTM_BLE_CONN_PARAM_UNDEF) {
                if (min_conn_int != BTM_BLE_CONN_PARAM_UNDEF) {
                    p_dev_rec->conn_params.min_conn_int = min_conn_int;
                } else {
                    p_dev_rec->conn_params.min_conn_int = max_conn_int;
                }

                if (max_conn_int != BTM_BLE_CONN_PARAM_UNDEF) {
                    p_dev_rec->conn_params.max_conn_int = max_conn_int;
                } else {
                    p_dev_rec->conn_params.max_conn_int = min_conn_int;
                }

                if (slave_latency != BTM_BLE_CONN_PARAM_UNDEF) {
                    p_dev_rec->conn_params.slave_latency = slave_latency;
                } else {
                    p_dev_rec->conn_params.slave_latency = BTM_BLE_CONN_SLAVE_LATENCY_DEF;
                }

                if (supervision_tout != BTM_BLE_CONN_PARAM_UNDEF) {
                    p_dev_rec->conn_params.supervision_tout = supervision_tout;
                } else {
                    p_dev_rec->conn_params.supervision_tout = BTM_BLE_CONN_TIMEOUT_DEF;
                }

            }

        } else {
            BTM_TRACE_ERROR("Unknown Device, setting rejected");
        }
    } else {
        BTM_TRACE_ERROR("Illegal Connection Parameters");
    }
}

/*******************************************************************************
**
** Function         BTM_ReadDevInfo
**
** Description      This function is called to read the device/address type
**                  of BD address.
**
** Parameter        remote_bda: remote device address
**                  p_dev_type: output parameter to read the device type.
**                  p_addr_type: output parameter to read the address type.
**
*******************************************************************************/
void BTM_ReadDevInfo (BD_ADDR remote_bda, tBT_DEVICE_TYPE *p_dev_type, tBLE_ADDR_TYPE *p_addr_type)
{
    tBTM_SEC_DEV_REC  *p_dev_rec = btm_find_dev (remote_bda);
    tBTM_INQ_INFO     *p_inq_info = BTM_InqDbRead(remote_bda);
    tBLE_ADDR_TYPE    temp_addr_type = (*p_addr_type);

    *p_addr_type = BLE_ADDR_PUBLIC;

    if (!p_dev_rec) {
        *p_dev_type = BT_DEVICE_TYPE_BREDR;
        /* Check with the BT manager if details about remote device are known */
        if (p_inq_info != NULL) {
            *p_dev_type = p_inq_info->results.device_type ;
            *p_addr_type = p_inq_info->results.ble_addr_type;
        } else {
            if(temp_addr_type <= BLE_ADDR_TYPE_MAX) {
                *p_addr_type = temp_addr_type;
            } else {
                /* unknown device, assume BR/EDR */
                BTM_TRACE_DEBUG ("btm_find_dev_type - unknown device, BR/EDR assumed");
            }
        }
    } else { /* there is a security device record existing */
        /* new inquiry result, overwrite device type in security device record */
        if (p_inq_info) {
            p_dev_rec->device_type          = p_inq_info->results.device_type;
            p_dev_rec->ble.ble_addr_type    = p_inq_info->results.ble_addr_type;
        }
        if (memcmp(p_dev_rec->bd_addr, remote_bda, BD_ADDR_LEN) == 0 &&
                memcmp(p_dev_rec->ble.pseudo_addr, remote_bda, BD_ADDR_LEN) == 0) {
            *p_dev_type = p_dev_rec->device_type;
            *p_addr_type = p_dev_rec->ble.ble_addr_type;
        } else if (memcmp(p_dev_rec->ble.pseudo_addr, remote_bda, BD_ADDR_LEN) == 0) {
            *p_dev_type = BT_DEVICE_TYPE_BLE;
            *p_addr_type = p_dev_rec->ble.ble_addr_type;
        } else { /* matching static address only */
            *p_dev_type = BT_DEVICE_TYPE_BREDR;
            *p_addr_type = BLE_ADDR_PUBLIC;
        }

    }

    BTM_TRACE_DEBUG ("btm_find_dev_type - device_type = %d addr_type = %d", *p_dev_type , *p_addr_type);
}
#endif  ///BLE_INCLUDED == TRUE

/*******************************************************************************
**
** Function         BTM_ReadConnectedTransportAddress
**
** Description      This function is called to read the paired device/address type of other device paired
**                  corresponding to the BD_address
**
** Parameter        remote_bda: remote device address, carry out the transport address
**                  transport: active transport
**
** Return           TRUE if an active link is identified; FALSE otherwise
**
*******************************************************************************/
BOOLEAN BTM_ReadConnectedTransportAddress(BD_ADDR remote_bda, tBT_TRANSPORT transport)
{
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(remote_bda);

    /* if no device can be located, return */
    if (p_dev_rec == NULL) {
        memset(remote_bda, 0, BD_ADDR_LEN);
        return FALSE;
    }

    if (transport == BT_TRANSPORT_BR_EDR) {
        if (btm_bda_to_acl(p_dev_rec->bd_addr, transport) != NULL) {
            memcpy(remote_bda, p_dev_rec->bd_addr, BD_ADDR_LEN);
            return TRUE;
        } else if (p_dev_rec->device_type & BT_DEVICE_TYPE_BREDR) {
            memcpy(remote_bda, p_dev_rec->bd_addr, BD_ADDR_LEN);
        } else {
            memset(remote_bda, 0, BD_ADDR_LEN);
        }
        return FALSE;
    }
#if (BLE_INCLUDED == TRUE)
    if (transport == BT_TRANSPORT_LE) {
        memcpy(remote_bda, p_dev_rec->ble.pseudo_addr, BD_ADDR_LEN);
        if (btm_bda_to_acl(p_dev_rec->ble.pseudo_addr, transport) != NULL) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
#endif  ///BLE_INCLUDED == TRUE
    return FALSE;
}

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTM_BleReceiverTest
**
** Description      This function is called to start the LE Receiver test
**
** Parameter       rx_freq - Frequency Range
**               p_cmd_cmpl_cback - Command Complete callback
**
*******************************************************************************/
void BTM_BleReceiverTest(UINT8 rx_freq, tBTM_CMPL_CB *p_cmd_cmpl_cback)
{
    btm_cb.devcb.p_le_test_cmd_cmpl_cb = p_cmd_cmpl_cback;

    if (btsnd_hcic_ble_receiver_test(rx_freq) == FALSE) {
        BTM_TRACE_ERROR("%s: Unable to Trigger LE receiver test", __FUNCTION__);
    }
}

/*******************************************************************************
**
** Function         BTM_BleTransmitterTest
**
** Description      This function is called to start the LE Transmitter test
**
** Parameter       tx_freq - Frequency Range
**                       test_data_len - Length in bytes of payload data in each packet
**                       packet_payload - Pattern to use in the payload
**                       p_cmd_cmpl_cback - Command Complete callback
**
*******************************************************************************/
void BTM_BleTransmitterTest(UINT8 tx_freq, UINT8 test_data_len,
                            UINT8 packet_payload, tBTM_CMPL_CB *p_cmd_cmpl_cback)
{
    btm_cb.devcb.p_le_test_cmd_cmpl_cb = p_cmd_cmpl_cback;
    if (btsnd_hcic_ble_transmitter_test(tx_freq, test_data_len, packet_payload) == FALSE) {
        BTM_TRACE_ERROR("%s: Unable to Trigger LE transmitter test", __FUNCTION__);
    }
}

/*******************************************************************************
**
** Function         BTM_BleTestEnd
**
** Description      This function is called to stop the in-progress TX or RX test
**
** Parameter       p_cmd_cmpl_cback - Command complete callback
**
*******************************************************************************/
void BTM_BleTestEnd(tBTM_CMPL_CB *p_cmd_cmpl_cback)
{
    btm_cb.devcb.p_le_test_cmd_cmpl_cb = p_cmd_cmpl_cback;

    if (btsnd_hcic_ble_test_end() == FALSE) {
        BTM_TRACE_ERROR("%s: Unable to End the LE TX/RX test", __FUNCTION__);
    }
}

/*******************************************************************************
** Internal Functions
*******************************************************************************/
void btm_ble_test_command_complete(UINT8 *p)
{
    tBTM_CMPL_CB   *p_cb = btm_cb.devcb.p_le_test_cmd_cmpl_cb;

    btm_cb.devcb.p_le_test_cmd_cmpl_cb = NULL;

    if (p_cb) {
        (*p_cb)(p);
    }
}


#if (BLE_50_FEATURE_SUPPORT == TRUE)
/*******************************************************************************
**
** Function         BTM_BleEnhancedReceiverTest
**
** Description      This function is called to start the LE Enhanced Receiver test
**
** Parameter       rx_freq - Frequency Range
**                 phy - The type of phy that receives data
**                 modulation_index - modulation index
**                 p_cmd_cmpl_cback - Command Complete callback
**
*******************************************************************************/
void BTM_BleEnhancedReceiverTest(UINT8 rx_freq, UINT8 phy, UINT8 modulation_index, tBTM_CMPL_CB *p_cmd_cmpl_cback)
{
    btm_cb.devcb.p_le_test_cmd_cmpl_cb = p_cmd_cmpl_cback;

    if (btsnd_hcic_ble_enhand_rx_test(rx_freq, phy, modulation_index) == FALSE) {
        BTM_TRACE_ERROR("%s: Unable to Trigger LE enhanced receiver test", __FUNCTION__);
    }
}

/*******************************************************************************
**
** Function         BTM_BleEnhancedTransmitterTest
**
** Description      This function is called to start the LE Enhanced Transmitter test
**
** Parameter       tx_freq - Frequency Range
**                 test_data_len - Length in bytes of payload data in each packet
**                 packet_payload - Pattern to use in the payload
**                 phy - The type of phy that sends data
**                 p_cmd_cmpl_cback - Command Complete callback
**
*******************************************************************************/
void BTM_BleEnhancedTransmitterTest(UINT8 tx_freq, UINT8 test_data_len,
                            UINT8 packet_payload, UINT8 phy, tBTM_CMPL_CB *p_cmd_cmpl_cback)
{
    btm_cb.devcb.p_le_test_cmd_cmpl_cb = p_cmd_cmpl_cback;
    if (btsnd_hcic_ble_enhand_tx_test(tx_freq, test_data_len, packet_payload, phy) == FALSE) {
        BTM_TRACE_ERROR("%s: Unable to Trigger LE enhanced transmitter test", __FUNCTION__);
    }
}
#endif // BLE_50_FEATURE_SUPPORT

/*******************************************************************************
**
** Function         BTM_UseLeLink
**
** Description      This function is to select the underneath physical link to use.
**
** Returns          TRUE to use LE, FALSE use BR/EDR.
**
*******************************************************************************/
BOOLEAN BTM_UseLeLink (BD_ADDR bd_addr)
{
    tACL_CONN         *p;
    tBT_DEVICE_TYPE     dev_type;
    tBLE_ADDR_TYPE      addr_type = 0;
    BOOLEAN             use_le = FALSE;

    if ((p = btm_bda_to_acl(bd_addr, BT_TRANSPORT_BR_EDR)) != NULL) {
        return use_le;
    } else if ((p = btm_bda_to_acl(bd_addr, BT_TRANSPORT_LE)) != NULL) {
        use_le = TRUE;
    } else {
        BTM_ReadDevInfo(bd_addr, &dev_type, &addr_type);
        use_le = (dev_type == BT_DEVICE_TYPE_BLE);
    }
    return use_le;
}


/*******************************************************************************
**
** Function         BTM_SetBleDataLength
**
** Description      This function is to set maximum BLE transmission packet size
**
** Returns          BTM_SUCCESS if success; otherwise failed.
**
*******************************************************************************/
tBTM_STATUS BTM_SetBleDataLength(BD_ADDR bd_addr, UINT16 tx_pdu_length)
{
    tACL_CONN *p_acl = btm_bda_to_acl(bd_addr, BT_TRANSPORT_LE);

    BTM_TRACE_DEBUG("%s: tx_pdu_length =%d", __FUNCTION__, tx_pdu_length);

    if (!controller_get_interface()->supports_ble_packet_extension()) {
        BTM_TRACE_ERROR("%s failed, request not supported", __FUNCTION__);
        return BTM_CONTROL_LE_DATA_LEN_UNSUPPORTED;
    }

    if (p_acl != NULL) {
        if (!HCI_LE_DATA_LEN_EXT_SUPPORTED(p_acl->peer_le_features)) {
            BTM_TRACE_ERROR("%s failed, peer does not support request", __FUNCTION__);
            return BTM_PEER_LE_DATA_LEN_UNSUPPORTED;
        }

        if (tx_pdu_length > BTM_BLE_DATA_SIZE_MAX) {
            tx_pdu_length =  BTM_BLE_DATA_SIZE_MAX;
        } else if (tx_pdu_length < BTM_BLE_DATA_SIZE_MIN) {
            tx_pdu_length =  BTM_BLE_DATA_SIZE_MIN;
        }

        /* always set the TxTime to be max, as controller does not care for now */
        btsnd_hcic_ble_set_data_length(p_acl->hci_handle, tx_pdu_length,
                                       BTM_BLE_DATA_TX_TIME_MAX);

        return BTM_SUCCESS;
    } else {
        BTM_TRACE_ERROR("%s: Wrong mode: no LE link exist or LE not supported", __FUNCTION__);
        return BTM_WRONG_MODE;
    }
}

#if (SMP_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         btm_ble_determine_security_act
**
** Description      This function checks the security of current LE link
**                  and returns the appropriate action that needs to be
**                  taken to achieve the required security.
**
** Parameter        is_originator - True if outgoing connection
**                  bdaddr: remote device address
**                  security_required: Security required for the service.
**
** Returns          The appropriate security action required.
**
*******************************************************************************/
tBTM_SEC_ACTION btm_ble_determine_security_act(BOOLEAN is_originator, BD_ADDR bdaddr, UINT16 security_required)
{
    tBTM_LE_AUTH_REQ auth_req = 0x00;

    if (is_originator)
    {
        if ((security_required & BTM_SEC_OUT_FLAGS) == 0 &&
                (security_required & BTM_SEC_OUT_MITM) == 0)
        {
            BTM_TRACE_DEBUG ("%s No security required for outgoing connection", __func__);
            return BTM_SEC_OK;
        }

        if (security_required & BTM_SEC_OUT_MITM) {
            auth_req |= BTM_LE_AUTH_REQ_MITM;
        }
    }
    else
    {
        if ((security_required & BTM_SEC_IN_FLAGS) == 0&& (security_required & BTM_SEC_IN_MITM) == 0)
        {
            BTM_TRACE_DEBUG ("%s No security required for incoming connection", __func__);
            return BTM_SEC_OK;
        }

        if (security_required & BTM_SEC_IN_MITM) {
            auth_req |= BTM_LE_AUTH_REQ_MITM;
        }
    }

    tBTM_BLE_SEC_REQ_ACT ble_sec_act = BTM_BLE_SEC_REQ_ACT_NONE;
    btm_ble_link_sec_check(bdaddr, auth_req, &ble_sec_act);

    BTM_TRACE_DEBUG ("%s ble_sec_act %d", __func__ , ble_sec_act);

    if (ble_sec_act == BTM_BLE_SEC_REQ_ACT_DISCARD) {
        return BTM_SEC_ENC_PENDING;
    }

    if (ble_sec_act == BTM_BLE_SEC_REQ_ACT_NONE) {
        return BTM_SEC_OK;
    }

    UINT8 sec_flag = 0;
    BTM_GetSecurityFlagsByTransport(bdaddr, &sec_flag, BT_TRANSPORT_LE);

    BOOLEAN is_link_encrypted = FALSE;
    BOOLEAN is_key_mitm = FALSE;
    if (sec_flag & (BTM_SEC_FLAG_ENCRYPTED| BTM_SEC_FLAG_LKEY_KNOWN))
    {
        if (sec_flag & BTM_SEC_FLAG_ENCRYPTED) {
            is_link_encrypted = TRUE;
        }

        if (sec_flag & BTM_SEC_FLAG_LKEY_AUTHED) {
            is_key_mitm = TRUE;
        }
    }

    if (auth_req & BTM_LE_AUTH_REQ_MITM)
    {
        if (!is_key_mitm)
        {
            return BTM_SEC_ENCRYPT_MITM;
        } else {
            if (is_link_encrypted) {
                return BTM_SEC_OK;
            } else {
                return BTM_SEC_ENCRYPT;
            }
        }
    } else {
        if (is_link_encrypted) {
            return BTM_SEC_OK;
        } else {
            return BTM_SEC_ENCRYPT_NO_MITM;
        }
    }

    return BTM_SEC_OK;
}

/*******************************************************************************
**
** Function         btm_ble_start_sec_check
**
** Description      This function is to check and set the security required for
**                  LE link for LE COC.
**
** Parameter        bdaddr: remote device address.
**                  psm : PSM of the LE COC service.
**                  is_originator: TRUE if outgoing connection.
**                  p_callback : Pointer to the callback function.
**                  p_ref_data : Pointer to be returned along with the callback.
**
** Returns          TRUE if link already meets the required security; otherwise FALSE.
**
*******************************************************************************/
BOOLEAN btm_ble_start_sec_check(BD_ADDR bd_addr, UINT16 psm, BOOLEAN is_originator,
                            tBTM_SEC_CALLBACK *p_callback, void *p_ref_data)
{
    /* Find the service record for the PSM */
    tBTM_SEC_SERV_REC *p_serv_rec = btm_sec_find_first_serv (is_originator, psm);

    /* If there is no application registered with this PSM do not allow connection */
    if (!p_serv_rec)
    {
        BTM_TRACE_WARNING ("%s PSM: %d no application registered", __func__, psm);
        (*p_callback) (bd_addr, BT_TRANSPORT_LE, p_ref_data, BTM_MODE_UNSUPPORTED);
        return FALSE;
    }

    tBTM_SEC_ACTION sec_act = btm_ble_determine_security_act(is_originator,
                                  bd_addr, p_serv_rec->security_flags);

    tBTM_BLE_SEC_ACT ble_sec_act = BTM_BLE_SEC_NONE;
    BOOLEAN status = FALSE;

    switch (sec_act)
    {
        case BTM_SEC_OK:
            BTM_TRACE_DEBUG ("%s Security met", __func__);
            p_callback(bd_addr, BT_TRANSPORT_LE, p_ref_data, BTM_SUCCESS);
            status = TRUE;
            break;

        case BTM_SEC_ENCRYPT:
            BTM_TRACE_DEBUG ("%s Encryption needs to be done", __func__);
            ble_sec_act = BTM_BLE_SEC_ENCRYPT;
            break;

        case BTM_SEC_ENCRYPT_MITM:
            BTM_TRACE_DEBUG ("%s Pairing with MITM needs to be done", __func__);
            ble_sec_act = BTM_BLE_SEC_ENCRYPT_MITM;
            break;

        case BTM_SEC_ENCRYPT_NO_MITM:
            BTM_TRACE_DEBUG ("%s Pairing with No MITM needs to be done", __func__);
            ble_sec_act = BTM_BLE_SEC_ENCRYPT_NO_MITM;
            break;

        case BTM_SEC_ENC_PENDING:
            BTM_TRACE_DEBUG ("%s Ecryption pending", __func__);
            break;
    }

    if (ble_sec_act == BTM_BLE_SEC_NONE) {
        return status;
    }

    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bd_addr, BT_TRANSPORT_LE);
    p_lcb->sec_act = sec_act;
    BTM_SetEncryption(bd_addr, BT_TRANSPORT_LE, p_callback, p_ref_data);

    return FALSE;
}


/*******************************************************************************
**
** Function         btm_ble_rand_enc_complete
**
** Description      This function is the callback functions for HCI_Rand command
**                  and HCI_Encrypt command is completed.
**                  This message is received from the HCI.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_rand_enc_complete (UINT8 *p, UINT16 op_code, tBTM_RAND_ENC_CB *p_enc_cplt_cback)
{
    tBTM_RAND_ENC   params;
    UINT8           *p_dest = params.param_buf;

    BTM_TRACE_DEBUG ("btm_ble_rand_enc_complete");

    memset(&params, 0, sizeof(tBTM_RAND_ENC));

    /* If there was a callback address for vcs complete, call it */
    if (p_enc_cplt_cback && p) {
        /* Pass parameters to the callback function */
        STREAM_TO_UINT8(params.status, p); /* command status */

        if (params.status == HCI_SUCCESS) {
            params.opcode = op_code;

            if (op_code == HCI_BLE_RAND) {
                params.param_len = BT_OCTET8_LEN;
            } else {
                params.param_len = BT_OCTET16_LEN;
            }

            memcpy(p_dest, p, params.param_len);  /* Fetch return info from HCI event message */
        }
        if (p_enc_cplt_cback) {
            (*p_enc_cplt_cback)(&params);    /* Call the Encryption complete callback function */
        }
    }
}
#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         btm_ble_get_enc_key_type
**
** Description      This function is to increment local sign counter
** Returns         None
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void btm_ble_increment_sign_ctr(BD_ADDR bd_addr, BOOLEAN is_local )
{
    tBTM_SEC_DEV_REC *p_dev_rec;

    BTM_TRACE_DEBUG ("btm_ble_increment_sign_ctr is_local=%d", is_local);

    if ((p_dev_rec = btm_find_dev (bd_addr)) != NULL) {
        if (is_local) {
            p_dev_rec->ble.keys.local_counter++;
        } else {
            p_dev_rec->ble.keys.counter++;
        }
        BTM_TRACE_DEBUG ("is_local=%d local sign counter=%d peer sign counter=%d",
                         is_local,
                         p_dev_rec->ble.keys.local_counter,
                         p_dev_rec->ble.keys.counter);
    }
}
#endif  ///SMP_INCLUDED == TRUE
#endif  ///BLE_INCLUDED == TRUE

/*******************************************************************************
**
** Function         btm_ble_get_enc_key_type
**
** Description      This function is to get the BLE key type that has been exchanged
**                  in between local device and peer device.
**
** Returns          p_key_type: output parameter to carry the key type value.
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
#if (BLE_INCLUDED == TRUE)
BOOLEAN btm_ble_get_enc_key_type(BD_ADDR bd_addr, UINT8 *p_key_types)
{
    tBTM_SEC_DEV_REC *p_dev_rec;

    BTM_TRACE_DEBUG ("btm_ble_get_enc_key_type");

    if ((p_dev_rec = btm_find_dev (bd_addr)) != NULL) {
        *p_key_types = p_dev_rec->ble.key_type;
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         btm_get_local_div
**
** Description      This function is called to read the local DIV
**
** Returns          TRUE - if a valid DIV is available
*******************************************************************************/
BOOLEAN btm_get_local_div (BD_ADDR bd_addr, UINT16 *p_div)
{
    tBTM_SEC_DEV_REC   *p_dev_rec;
    BOOLEAN            status = FALSE;
    BTM_TRACE_DEBUG ("btm_get_local_div");

    BTM_TRACE_DEBUG("bd_addr:%02x-%02x-%02x-%02x-%02x-%02x",
                    bd_addr[0], bd_addr[1],
                    bd_addr[2], bd_addr[3],
                    bd_addr[4], bd_addr[5]);

    *p_div = 0;
    p_dev_rec = btm_find_dev (bd_addr);

    if (p_dev_rec && p_dev_rec->ble.keys.div) {
        status = TRUE;
        *p_div = p_dev_rec->ble.keys.div;
    }
    BTM_TRACE_DEBUG ("btm_get_local_div status=%d (1-OK) DIV=0x%x", status, *p_div);
    return status;
}

/*******************************************************************************
**
** Function         btm_sec_save_le_key
**
** Description      This function is called by the SMP to update
**                  an  BLE key.  SMP is internal, whereas all the keys shall
**                  be sent to the application.  The function is also called
**                  when application passes ble key stored in NVRAM to the btm_sec.
**                  pass_to_application parameter is false in this case.
**
** Returns          void
**
*******************************************************************************/
void btm_sec_save_le_key(BD_ADDR bd_addr, tBTM_LE_KEY_TYPE key_type, tBTM_LE_KEY_VALUE *p_keys,
                         BOOLEAN pass_to_application)
{
    tBTM_SEC_DEV_REC *p_rec;
    tBTM_LE_EVT_DATA    cb_data;
    UINT8 i;

    BTM_TRACE_DEBUG ("btm_sec_save_le_key key_type=0x%x pass_to_application=%d", key_type, pass_to_application);
    /* Store the updated key in the device database */

    BTM_TRACE_DEBUG("bd_addr:%02x-%02x-%02x-%02x-%02x-%02x",
                    bd_addr[0], bd_addr[1],
                    bd_addr[2], bd_addr[3],
                    bd_addr[4], bd_addr[5]);

    if ((p_rec = btm_find_dev (bd_addr)) != NULL && (p_keys || key_type == BTM_LE_KEY_LID)) {
        btm_ble_init_pseudo_addr (p_rec, bd_addr);

        switch (key_type) {
        case BTM_LE_KEY_PENC:
            memcpy(p_rec->ble.keys.pltk, p_keys->penc_key.ltk, BT_OCTET16_LEN);
            memcpy(p_rec->ble.keys.rand, p_keys->penc_key.rand, BT_OCTET8_LEN);
            p_rec->ble.keys.sec_level = p_keys->penc_key.sec_level;
            p_rec->ble.keys.ediv = p_keys->penc_key.ediv;
            p_rec->ble.keys.key_size = p_keys->penc_key.key_size;
            p_rec->ble.key_type |= BTM_LE_KEY_PENC;
            p_rec->sec_flags |= BTM_SEC_LE_LINK_KEY_KNOWN;
            if (p_keys->penc_key.sec_level == SMP_SEC_AUTHENTICATED) {
                p_rec->sec_flags |= BTM_SEC_LE_LINK_KEY_AUTHED;
            } else {
                p_rec->sec_flags &= ~BTM_SEC_LE_LINK_KEY_AUTHED;
            }
            BTM_TRACE_DEBUG("BTM_LE_KEY_PENC key_type=0x%x sec_flags=0x%x sec_leve=0x%x",
                            p_rec->ble.key_type,
                            p_rec->sec_flags,
                            p_rec->ble.keys.sec_level);
            break;

        case BTM_LE_KEY_PID:
            for (i = 0; i < BT_OCTET16_LEN; i++) {
                p_rec->ble.keys.irk[i] = p_keys->pid_key.irk[i];
            }

            //memcpy( p_rec->ble.keys.irk, p_keys->pid_key, BT_OCTET16_LEN); todo will crash the system
            memcpy(p_rec->ble.static_addr, p_keys->pid_key.static_addr, BD_ADDR_LEN);
            p_rec->ble.static_addr_type = p_keys->pid_key.addr_type;
            p_rec->ble.key_type |= BTM_LE_KEY_PID;
            BTM_TRACE_DEBUG("BTM_LE_KEY_PID key_type=0x%x save peer IRK",  p_rec->ble.key_type);
            /* update device record address as static address */
            memcpy(p_rec->bd_addr, p_keys->pid_key.static_addr, BD_ADDR_LEN);
            /* combine DUMO device security record if needed */
            btm_consolidate_dev(p_rec);
            break;

        case BTM_LE_KEY_PCSRK:
            memcpy(p_rec->ble.keys.pcsrk, p_keys->pcsrk_key.csrk, BT_OCTET16_LEN);
            p_rec->ble.keys.srk_sec_level = p_keys->pcsrk_key.sec_level;
            p_rec->ble.keys.counter  = p_keys->pcsrk_key.counter;
            p_rec->ble.key_type |= BTM_LE_KEY_PCSRK;
            p_rec->sec_flags |=  BTM_SEC_LE_LINK_KEY_KNOWN;
            if ( p_keys->pcsrk_key.sec_level == SMP_SEC_AUTHENTICATED) {
                p_rec->sec_flags |= BTM_SEC_LE_LINK_KEY_AUTHED;
            } else {
                p_rec->sec_flags &= ~BTM_SEC_LE_LINK_KEY_AUTHED;
            }

            BTM_TRACE_DEBUG("BTM_LE_KEY_PCSRK key_type=0x%x sec_flags=0x%x sec_level=0x%x peer_counter=%d",
                            p_rec->ble.key_type,
                            p_rec->sec_flags,
                            p_rec->ble.keys.srk_sec_level,
                            p_rec->ble.keys.counter );
            break;

        case BTM_LE_KEY_LENC:
            memcpy(p_rec->ble.keys.lltk, p_keys->lenc_key.ltk, BT_OCTET16_LEN);
            p_rec->ble.keys.div = p_keys->lenc_key.div; /* update DIV */
            p_rec->ble.keys.sec_level = p_keys->lenc_key.sec_level;
            p_rec->ble.keys.key_size = p_keys->lenc_key.key_size;
            p_rec->ble.key_type |= BTM_LE_KEY_LENC;

            /* Set that link key is known since this shares field with BTM_SEC_FLAG_LKEY_KNOWN flag in stack/btm_api.h*/
            p_rec->sec_flags |=  BTM_SEC_LE_LINK_KEY_KNOWN;
            if ( p_keys->lenc_key.sec_level == SMP_SEC_AUTHENTICATED) {
                p_rec->sec_flags |= BTM_SEC_LE_LINK_KEY_AUTHED;
            } else {
                p_rec->sec_flags &= ~BTM_SEC_LE_LINK_KEY_AUTHED;
            }

            BTM_TRACE_DEBUG("BTM_LE_KEY_LENC key_type=0x%x DIV=0x%x key_size=0x%x sec_level=0x%x",
                            p_rec->ble.key_type,
                            p_rec->ble.keys.div,
                            p_rec->ble.keys.key_size,
                            p_rec->ble.keys.sec_level );
            break;

        case BTM_LE_KEY_LCSRK:/* local CSRK has been delivered */
            memcpy (p_rec->ble.keys.lcsrk, p_keys->lcsrk_key.csrk, BT_OCTET16_LEN);
            p_rec->ble.keys.div = p_keys->lcsrk_key.div; /* update DIV */
            p_rec->ble.keys.local_csrk_sec_level = p_keys->lcsrk_key.sec_level;
            p_rec->ble.keys.local_counter  = p_keys->lcsrk_key.counter;
            p_rec->ble.key_type |= BTM_LE_KEY_LCSRK;
            BTM_TRACE_DEBUG("BTM_LE_KEY_LCSRK key_type=0x%x DIV=0x%x scrk_sec_level=0x%x local_counter=%d",
                            p_rec->ble.key_type,
                            p_rec->ble.keys.div,
                            p_rec->ble.keys.local_csrk_sec_level,
                            p_rec->ble.keys.local_counter );
            break;

        case BTM_LE_KEY_LID:
            p_rec->ble.key_type |= BTM_LE_KEY_LID;
            break;
        default:
            BTM_TRACE_WARNING("btm_sec_save_le_key (Bad key_type 0x%02x)", key_type);
            return;
        }

        BTM_TRACE_DEBUG ("BLE key type 0x%02x updated for BDA: %08x%04x (btm_sec_save_le_key)", key_type,
                         (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                         (bd_addr[4] << 8) + bd_addr[5]);

        /* Notify the application that one of the BLE keys has been updated
           If link key is in progress, it will get sent later.*/
        if (pass_to_application && btm_cb.api.p_le_callback) {
            cb_data.key.p_key_value = p_keys;
            cb_data.key.key_type = key_type;

            (*btm_cb.api.p_le_callback) (BTM_LE_KEY_EVT, bd_addr, &cb_data);
        }
        return;
    }

    BTM_TRACE_WARNING ("BLE key type 0x%02x called for Unknown BDA or type: %08x%04x !! (btm_sec_save_le_key)", key_type,
                       (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                       (bd_addr[4] << 8) + bd_addr[5]);

    if (p_rec) {
        BTM_TRACE_DEBUG ("sec_flags=0x%x", p_rec->sec_flags);
    }
}

/*******************************************************************************
**
** Function         btm_ble_update_sec_key_size
**
** Description      update the current lin kencryption key size
**
** Returns          void
**
*******************************************************************************/
void btm_ble_update_sec_key_size(BD_ADDR bd_addr, UINT8 enc_key_size)
{
    tBTM_SEC_DEV_REC *p_rec;

    BTM_TRACE_DEBUG("btm_ble_update_sec_key_size enc_key_size = %d", enc_key_size);

    if ((p_rec = btm_find_dev (bd_addr)) != NULL ) {
        p_rec->enc_key_size = enc_key_size;
    }
}


/*******************************************************************************
**
** Function         btm_ble_read_sec_key_size
**
** Description      update the current lin kencryption key size
**
** Returns          void
**
*******************************************************************************/
UINT8 btm_ble_read_sec_key_size(BD_ADDR bd_addr)
{
    tBTM_SEC_DEV_REC *p_rec;

    if ((p_rec = btm_find_dev (bd_addr)) != NULL ) {
        return p_rec->enc_key_size;
    } else {
        return 0;
    }
    return 0;
}

/*******************************************************************************
**
** Function         btm_ble_link_sec_check
**
** Description      Check BLE link security level match.
**
** Returns          TRUE: check is OK and the *p_sec_req_act contain the action
**
*******************************************************************************/
void btm_ble_link_sec_check(BD_ADDR bd_addr, tBTM_LE_AUTH_REQ auth_req, tBTM_BLE_SEC_REQ_ACT *p_sec_req_act)
{
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (bd_addr);
    UINT8 req_sec_level = BTM_LE_SEC_NONE, cur_sec_level = BTM_LE_SEC_NONE;

    BTM_TRACE_DEBUG ("btm_ble_link_sec_check auth_req =0x%x", auth_req);

    if (p_dev_rec == NULL) {
        BTM_TRACE_ERROR ("btm_ble_link_sec_check received for unknown device");
        return;
    }

    if (p_dev_rec->sec_state == BTM_SEC_STATE_ENCRYPTING ||
            p_dev_rec->sec_state == BTM_SEC_STATE_AUTHENTICATING) {
        /* race condition: discard the security request while master is encrypting the link */
        *p_sec_req_act = BTM_BLE_SEC_REQ_ACT_DISCARD;
    } else {
        req_sec_level = BTM_LE_SEC_UNAUTHENTICATE;
        if (auth_req & BTM_LE_AUTH_REQ_MITM) {
            req_sec_level = BTM_LE_SEC_AUTHENTICATED;
        }

        BTM_TRACE_DEBUG ("dev_rec sec_flags=0x%x", p_dev_rec->sec_flags);

        /* currently encrypted  */
        if (p_dev_rec->sec_flags & BTM_SEC_LE_ENCRYPTED) {
            if (p_dev_rec->sec_flags & BTM_SEC_LE_AUTHENTICATED) {
                cur_sec_level = BTM_LE_SEC_AUTHENTICATED;
            } else {
                cur_sec_level = BTM_LE_SEC_UNAUTHENTICATE;
            }
        } else { /* unencrypted link */
            /* if bonded, get the key security level */
            if (p_dev_rec->ble.key_type & BTM_LE_KEY_PENC) {
                cur_sec_level = p_dev_rec->ble.keys.sec_level;
            } else {
                cur_sec_level = BTM_LE_SEC_NONE;
            }
        }

        if (cur_sec_level >= req_sec_level) {
            /* To avoid re-encryption on an encrypted link for an equal condition encryption */
            *p_sec_req_act = BTM_BLE_SEC_REQ_ACT_ENCRYPT;
        } else {
            *p_sec_req_act = BTM_BLE_SEC_REQ_ACT_PAIR; /* start the pariring process to upgrade the keys*/
        }
    }

    BTM_TRACE_DEBUG("cur_sec_level=%d req_sec_level=%d sec_req_act=%d",
                    cur_sec_level,
                    req_sec_level,
                    *p_sec_req_act);


}
#endif  ///BLE_INCLUDED == TRUE
#endif  ///SMP_INCLUDED == TRUE

#if (BLE_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         btm_ble_set_encryption
**
** Description      This function is called to ensure that LE connection is
**                  encrypted.  Should be called only on an open connection.
**                  Typically only needed for connections that first want to
**                  bring up unencrypted links, then later encrypt them.
**
** Returns          void
**                  the local device ER is copied into er
**
*******************************************************************************/
tBTM_STATUS btm_ble_set_encryption (BD_ADDR bd_addr, void *p_ref_data, UINT8 link_role)
{
    tBTM_STATUS         cmd = BTM_NO_RESOURCES;
#if (SMP_INCLUDED == TRUE)
    tBTM_BLE_SEC_ACT    sec_act = *(tBTM_BLE_SEC_ACT *)p_ref_data ;
    tBTM_SEC_DEV_REC    *p_rec = btm_find_dev (bd_addr);
    tBTM_BLE_SEC_REQ_ACT sec_req_act;
    tBTM_LE_AUTH_REQ    auth_req;

    if (p_rec == NULL) {
        BTM_TRACE_WARNING ("btm_ble_set_encryption (NULL device record!! sec_act=0x%x", sec_act);
        return (BTM_WRONG_MODE);
    }

    BTM_TRACE_DEBUG ("btm_ble_set_encryption sec_act=0x%x role_master=%d", sec_act, p_rec->role_master);

    if (sec_act == BTM_BLE_SEC_ENCRYPT_MITM) {
        p_rec->security_required |= BTM_SEC_IN_MITM;
    }

    switch (sec_act) {
    case BTM_BLE_SEC_ENCRYPT:
        if (link_role == BTM_ROLE_MASTER && (p_rec->ble.key_type & BTM_LE_KEY_PENC)) {
            /* start link layer encryption using the security info stored */
            cmd = btm_ble_start_encrypt(bd_addr, FALSE, NULL);
            break;
        }
    /* if salve role then fall through to call SMP_Pair below which will send a
       sec_request to request the master to encrypt the link */
    case BTM_BLE_SEC_ENCRYPT_NO_MITM:
    case BTM_BLE_SEC_ENCRYPT_MITM:
        if ((link_role == BTM_ROLE_MASTER) && (sec_act != BTM_BLE_SEC_ENCRYPT)) {
            auth_req = (sec_act == BTM_BLE_SEC_ENCRYPT_NO_MITM)
                       ? SMP_AUTH_GEN_BOND : (SMP_AUTH_GEN_BOND | SMP_AUTH_YN_BIT);
            btm_ble_link_sec_check (bd_addr, auth_req, &sec_req_act);

            if (sec_req_act == BTM_BLE_SEC_REQ_ACT_ENCRYPT) {
                cmd = btm_ble_start_encrypt(bd_addr, FALSE, NULL);
                break;
            }
        }
#if (SMP_SLAVE_CON_PARAMS_UPD_ENABLE == TRUE)
        // already have encrypted information, do not need to update connection parameters
        if(link_role == BTM_ROLE_SLAVE && (p_rec->ble.key_type & BTM_LE_KEY_PENC)) {
            p_rec->ble.skip_update_conn_param = true;
        } else {
            p_rec->ble.skip_update_conn_param = false;
        }
#endif
        if (SMP_Pair(bd_addr) == SMP_STARTED) {
            cmd = BTM_CMD_STARTED;
            p_rec->sec_state = BTM_SEC_STATE_AUTHENTICATING;
        }
        break;

    default:
        cmd = BTM_WRONG_MODE;
        break;
    }
#endif  ///SMP_INCLUDED == TRUE
    return cmd;
}

/*******************************************************************************
**
** Function         btm_ble_ltk_request
**
** Description      This function is called when encryption request is received
**                  on a slave device.
**
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void btm_ble_ltk_request(UINT16 handle, UINT8 rand[8], UINT16 ediv)
{
    tBTM_CB *p_cb = &btm_cb;
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev_by_handle (handle);
    BT_OCTET16 dummy_stk = {0};

    BTM_TRACE_DEBUG ("btm_ble_ltk_request");

    p_cb->ediv = ediv;

    memcpy(p_cb->enc_rand, rand, BT_OCTET8_LEN);

    if (p_dev_rec != NULL) {
        if (!smp_proc_ltk_request(p_dev_rec->bd_addr)) {
            btm_ble_ltk_request_reply(p_dev_rec->bd_addr, FALSE, dummy_stk);
        }
    }

}
#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         btm_ble_start_encrypt
**
** Description      This function is called to start LE encryption.
**
**
** Returns          BTM_SUCCESS if encryption was started successfully
**
*******************************************************************************/
tBTM_STATUS btm_ble_start_encrypt(BD_ADDR bda, BOOLEAN use_stk, BT_OCTET16 stk)
{
#if (SMP_INCLUDED == TRUE)
    tBTM_CB *p_cb = &btm_cb;
    tBTM_SEC_DEV_REC    *p_rec = btm_find_dev (bda);
    BT_OCTET8    dummy_rand = {0};
#endif  ///SMP_INCLUDED == TRUE

    tBTM_STATUS  rt = BTM_NO_RESOURCES;
#if (SMP_INCLUDED == TRUE)
    BTM_TRACE_DEBUG ("btm_ble_start_encrypt");

    if (!p_rec ) {
        BTM_TRACE_ERROR("Link is not active, can not encrypt!");
        return BTM_WRONG_MODE;
    }

    if (p_rec->sec_state == BTM_SEC_STATE_ENCRYPTING) {
        BTM_TRACE_WARNING("Link Encryption is active, Busy!");
        return BTM_BUSY;
    }

    p_cb->enc_handle = p_rec->ble_hci_handle;

    if (use_stk) {
        if (btsnd_hcic_ble_start_enc(p_rec->ble_hci_handle, dummy_rand, 0, stk)) {
            rt = BTM_CMD_STARTED;
        }
    } else if (p_rec->ble.key_type & BTM_LE_KEY_PENC) {
        if (btsnd_hcic_ble_start_enc(p_rec->ble_hci_handle, p_rec->ble.keys.rand,
                                     p_rec->ble.keys.ediv, p_rec->ble.keys.pltk)) {
            rt = BTM_CMD_STARTED;
        }
    } else {
        BTM_TRACE_ERROR("No key available to encrypt the link");
    }
    if (rt == BTM_CMD_STARTED) {
        if (p_rec->sec_state == BTM_SEC_STATE_IDLE) {
            p_rec->sec_state = BTM_SEC_STATE_ENCRYPTING;
        }
    }
#endif  ///SMP_INCLUDED == TRUE
    return rt;
}

/*******************************************************************************
**
** Function         btm_ble_link_encrypted
**
** Description      This function is called when LE link encryption status is changed.
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void btm_ble_link_encrypted(BD_ADDR bd_addr, UINT8 encr_enable)
{
    tBTM_SEC_DEV_REC    *p_dev_rec = btm_find_dev (bd_addr);
    BOOLEAN             enc_cback;

    if (!p_dev_rec) {
        BTM_TRACE_WARNING ("btm_ble_link_encrypted (No Device Found!) encr_enable=%d", encr_enable);
        return;
    }

    BTM_TRACE_DEBUG ("btm_ble_link_encrypted encr_enable=%d", encr_enable);

    enc_cback = (p_dev_rec->sec_state == BTM_SEC_STATE_ENCRYPTING);

    smp_link_encrypted(bd_addr, encr_enable);

    BTM_TRACE_DEBUG(" p_dev_rec->sec_flags=0x%x", p_dev_rec->sec_flags);

    if (encr_enable && p_dev_rec->enc_key_size == 0) {
        p_dev_rec->enc_key_size = p_dev_rec->ble.keys.key_size;
    }

    p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;
    if (p_dev_rec->p_callback && enc_cback) {
        if (encr_enable) {
            btm_sec_dev_rec_cback_event(p_dev_rec, BTM_SUCCESS, TRUE);
        } else if (p_dev_rec->role_master) {
            btm_sec_dev_rec_cback_event(p_dev_rec, BTM_ERR_PROCESSING, TRUE);
        }

    }
    /* to notify GATT to send data if any request is pending */
    gatt_notify_enc_cmpl(p_dev_rec->ble.pseudo_addr);
}
#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         btm_ble_ltk_request_reply
**
** Description      This function is called to send a LTK request reply on a slave
**                  device.
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
void btm_ble_ltk_request_reply(BD_ADDR bda,  BOOLEAN use_stk, BT_OCTET16 stk)
{
    tBTM_SEC_DEV_REC    *p_rec = btm_find_dev (bda);
    tBTM_CB *p_cb = &btm_cb;

    if (p_rec == NULL) {
        BTM_TRACE_ERROR("btm_ble_ltk_request_reply received for unknown device");
        return;
    }

    BTM_TRACE_DEBUG ("btm_ble_ltk_request_reply");
    p_cb->enc_handle = p_rec->ble_hci_handle;
    p_cb->key_size = p_rec->ble.keys.key_size;

    BTM_TRACE_DEBUG("key size = %d", p_rec->ble.keys.key_size);
    if (use_stk) {
        btsnd_hcic_ble_ltk_req_reply(btm_cb.enc_handle, stk);
    } else { /* calculate LTK using peer device  */
        if (p_rec->ble.key_type & BTM_LE_KEY_LENC) {
            btsnd_hcic_ble_ltk_req_reply(btm_cb.enc_handle, p_rec->ble.keys.lltk);
        } else {
            btsnd_hcic_ble_ltk_req_neg_reply(btm_cb.enc_handle);
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_io_capabilities_req
**
** Description      This function is called to handle SMP get IO capability request.
**
** Returns          void
**
*******************************************************************************/
UINT8 btm_ble_io_capabilities_req(tBTM_SEC_DEV_REC *p_dev_rec, tBTM_LE_IO_REQ *p_data)
{
    UINT8           callback_rc = BTM_SUCCESS;
    BTM_TRACE_DEBUG ("btm_ble_io_capabilities_req");
    if (btm_cb.api.p_le_callback) {
        /* the callback function implementation may change the IO capability... */
        callback_rc = (*btm_cb.api.p_le_callback) (BTM_LE_IO_REQ_EVT, p_dev_rec->bd_addr, (tBTM_LE_EVT_DATA *)p_data);
    }
#if BTM_OOB_INCLUDED == TRUE
    if ((callback_rc == BTM_SUCCESS) || (BTM_OOB_UNKNOWN != p_data->oob_data))
#else
    if (callback_rc == BTM_SUCCESS)
#endif
    {
#if BTM_BLE_CONFORMANCE_TESTING == TRUE
        if (btm_cb.devcb.keep_rfu_in_auth_req) {
            BTM_TRACE_DEBUG ("btm_ble_io_capabilities_req keep_rfu_in_auth_req = %u",
                             btm_cb.devcb.keep_rfu_in_auth_req);
            p_data->auth_req &= BTM_LE_AUTH_REQ_MASK_KEEP_RFU;
            btm_cb.devcb.keep_rfu_in_auth_req = FALSE;
        } else {
            /* default */
            p_data->auth_req &= BTM_LE_AUTH_REQ_MASK;
        }
#else
        p_data->auth_req &= BTM_LE_AUTH_REQ_MASK;
#endif

        BTM_TRACE_DEBUG ("btm_ble_io_capabilities_req 1: p_dev_rec->security_required = %d auth_req:%d",
                         p_dev_rec->security_required, p_data->auth_req);
        BTM_TRACE_DEBUG ("btm_ble_io_capabilities_req 2: i_keys=0x%x r_keys=0x%x (bit 0-LTK 1-IRK 2-CSRK)",
                         p_data->init_keys,
                         p_data->resp_keys);

        /* if authentication requires MITM protection, put on the mask */
        if (p_dev_rec->security_required & BTM_SEC_IN_MITM) {
            p_data->auth_req |= BTM_LE_AUTH_REQ_MITM;
        }

        if (!(p_data->auth_req & SMP_AUTH_BOND)) {
            BTM_TRACE_DEBUG("Non bonding: No keys should be exchanged");
            p_data->init_keys = 0;
            p_data->resp_keys = 0;
        }

        BTM_TRACE_DEBUG ("btm_ble_io_capabilities_req 3: auth_req:%d\n", p_data->auth_req);
        BTM_TRACE_DEBUG ("btm_ble_io_capabilities_req 4: i_keys=0x%x r_keys=0x%x\n",
                         p_data->init_keys,
                         p_data->resp_keys);

        BTM_TRACE_DEBUG ("btm_ble_io_capabilities_req 5: p_data->io_cap = %d auth_req:%d\n",
                         p_data->io_cap, p_data->auth_req);

        /* remove MITM protection requirement if IO cap does not allow it */
        if ((p_data->io_cap == BTM_IO_CAP_NONE) && p_data->oob_data == SMP_OOB_NONE) {
            p_data->auth_req &= ~BTM_LE_AUTH_REQ_MITM;
        }

        if (!(p_data->auth_req & SMP_SC_SUPPORT_BIT)) {
            /* if Secure Connections are not supported then remove LK derivation,
            ** and keypress notifications.
            */
            BTM_TRACE_DEBUG("%s-SC not supported -> No LK derivation, no keypress notifications",
                            __func__);
            p_data->auth_req &= ~SMP_KP_SUPPORT_BIT;
            p_data->init_keys &= ~SMP_SEC_KEY_TYPE_LK;
            p_data->resp_keys &= ~SMP_SEC_KEY_TYPE_LK;
        }

        BTM_TRACE_DEBUG ("btm_ble_io_capabilities_req 6: IO_CAP:%d oob_data:%d auth_req:0x%02x\n",
                         p_data->io_cap, p_data->oob_data, p_data->auth_req);
    }
    return callback_rc;
}
#endif  ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         btm_ble_br_keys_req
**
** Description      This function is called to handle SMP request for keys sent
**                  over BR/EDR.
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
UINT8 btm_ble_br_keys_req(tBTM_SEC_DEV_REC *p_dev_rec, tBTM_LE_IO_REQ *p_data)
{
    UINT8           callback_rc = BTM_SUCCESS;
    BTM_TRACE_DEBUG ("%s\n", __func__);
    if (btm_cb.api.p_le_callback) {
        /* the callback function implementation may change the IO capability... */
        callback_rc = (*btm_cb.api.p_le_callback) (BTM_LE_IO_REQ_EVT, p_dev_rec->bd_addr,
                      (tBTM_LE_EVT_DATA *)p_data);
    }
    return callback_rc;
}
#endif  ///SMP_INCLUDED


#if (BLE_PRIVACY_SPT == TRUE )
/*******************************************************************************
**
** Function         btm_ble_resolve_random_addr_on_conn_cmpl
**
** Description      resolve random address complete on connection complete event.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_resolve_random_addr_on_conn_cmpl(void *p_rec, void *p_data)
{
    UINT8   *p = (UINT8 *)p_data;
    tBTM_SEC_DEV_REC    *match_rec = (tBTM_SEC_DEV_REC *) p_rec;
    UINT8       role, bda_type;
    UINT16      handle;
    BD_ADDR     bda, local_rpa, peer_rpa;
    UINT16      conn_interval, conn_latency, conn_timeout;
    BOOLEAN     match = FALSE;

    ++p;
    STREAM_TO_UINT16   (handle, p);
    STREAM_TO_UINT8    (role, p);
    STREAM_TO_UINT8    (bda_type, p);
    STREAM_TO_BDADDR   (bda, p);
    // if the enhanced is true, means the connection is enhanced connect,
    // so the packet should include the local Resolvable Private Address and Peer Resolvable Private Address
    if(temp_enhanced) {
        STREAM_TO_BDADDR(local_rpa, p);
        STREAM_TO_BDADDR(peer_rpa, p);
    }
    STREAM_TO_UINT16   (conn_interval, p);
    STREAM_TO_UINT16   (conn_latency, p);
    STREAM_TO_UINT16   (conn_timeout, p);

    handle = HCID_GET_HANDLE (handle);
    BTM_TRACE_EVENT ("%s\n", __func__);

    if (match_rec) {
        BTM_TRACE_DEBUG("%s matched and resolved random address", __func__);
        match = TRUE;
        match_rec->ble.active_addr_type = BTM_BLE_ADDR_RRA;
        memcpy(match_rec->ble.cur_rand_addr, bda, BD_ADDR_LEN);
        if (!btm_ble_init_pseudo_addr (match_rec, bda)) {
            /* assign the original address to be the current report address */
            memcpy(bda, match_rec->ble.pseudo_addr, BD_ADDR_LEN);
        } else {
            memcpy(bda, match_rec->bd_addr, BD_ADDR_LEN);
        }
    } else {
        BTM_TRACE_DEBUG("%s unable to match and resolve random address", __func__);
    }

    btm_ble_connected(bda, handle, HCI_ENCRYPT_MODE_DISABLED, role, bda_type, match);

    l2cble_conn_comp (handle, role, bda, bda_type, conn_interval,
                      conn_latency, conn_timeout);

    return;
}
#endif

/*******************************************************************************
**
** Function         btm_ble_connected
**
** Description      This function is when a LE connection to the peer device is
**                  established
**
** Returns          void
**
*******************************************************************************/
void btm_ble_connected (UINT8 *bda, UINT16 handle, UINT8 enc_mode, UINT8 role,
                        tBLE_ADDR_TYPE addr_type, BOOLEAN addr_matched)
{
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (bda);
    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;
    UNUSED(addr_matched);

    BTM_TRACE_EVENT ("btm_ble_connected");

    /* Commenting out trace due to obf/compilation problems.
    */
#if (BT_USE_TRACES == TRUE)
    if (p_dev_rec) {
        BTM_TRACE_EVENT ("Security Manager: btm_ble_connected :  handle:%d  enc_mode:%d  bda:%x RName:%s",
                         handle,  enc_mode,
                         (bda[2] << 24) + (bda[3] << 16) + (bda[4] << 8) + bda[5],
                         p_dev_rec->sec_bd_name);

        BTM_TRACE_DEBUG ("btm_ble_connected sec_flags=0x%x", p_dev_rec->sec_flags);
    } else {
        BTM_TRACE_EVENT ("Security Manager: btm_ble_connected:   handle:%d  enc_mode:%d  bda:%x ",
                         handle,  enc_mode,
                         (bda[2] << 24) + (bda[3] << 16) + (bda[4] << 8) + bda[5]);
    }
#endif

    if (!p_dev_rec) {
        /* There is no device record for new connection.  Allocate one */
        if ((p_dev_rec = btm_sec_alloc_dev (bda)) == NULL) {
            return;
        }
    } else { /* Update the timestamp for this device */
        p_dev_rec->timestamp = btm_cb.dev_rec_count++;
    }

    /* update device information */
    p_dev_rec->device_type |= BT_DEVICE_TYPE_BLE;
    p_dev_rec->ble_hci_handle = handle;
    p_dev_rec->ble.ble_addr_type = addr_type;
    /* update pseudo address */
    memcpy(p_dev_rec->ble.pseudo_addr, bda, BD_ADDR_LEN);

    p_dev_rec->role_master = FALSE;
    if (role == HCI_ROLE_MASTER) {
        p_dev_rec->role_master = TRUE;
    }

#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    if (!addr_matched) {
        p_dev_rec->ble.active_addr_type = BTM_BLE_ADDR_PSEUDO;
    }

    if (p_dev_rec->ble.ble_addr_type == BLE_ADDR_RANDOM && !addr_matched) {
        memcpy(p_dev_rec->ble.cur_rand_addr, bda, BD_ADDR_LEN);
    }
#endif

    p_cb->inq_var.directed_conn = BTM_BLE_CONNECT_EVT;

    return;
}

/*****************************************************************************
**  Function        btm_ble_conn_complete
**
**  Description     LE connection complete.
**
******************************************************************************/
void btm_ble_conn_complete(UINT8 *p, UINT16 evt_len, BOOLEAN enhanced)
{
#if (BLE_PRIVACY_SPT == TRUE )
    UINT8       *p_data = p, peer_addr_type;
#endif  ///BLE_PRIVACY_SPT == TRUE
    UINT8       role, status, bda_type;
    UINT16      handle;
    BD_ADDR     bda;
    BD_ADDR     local_rpa, peer_rpa;
    UINT16      conn_interval, conn_latency, conn_timeout;
    BOOLEAN     match = FALSE;
    UNUSED(evt_len);
    STREAM_TO_UINT8   (status, p);
    STREAM_TO_UINT16   (handle, p);
    STREAM_TO_UINT8    (role, p);
    STREAM_TO_UINT8    (bda_type, p);
    STREAM_TO_BDADDR   (bda, p);
    BTM_TRACE_DEBUG("status = %d, handle = %d, role = %d, bda_type = %d",status,handle,role,bda_type);
    if (status == 0) {
        if (enhanced) {
            STREAM_TO_BDADDR   (local_rpa, p);
            STREAM_TO_BDADDR   (peer_rpa, p);
#if (CONTROLLER_RPA_LIST_ENABLE == TRUE)
            BD_ADDR dummy_bda = {0};
            /* For controller generates RPA, if resolving list contains no matching entry, it use identity address.
             * So we should update own addr type in Host */
            if (memcmp(local_rpa, dummy_bda, BD_ADDR_LEN)) {
                btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type |= (BLE_ADDR_TYPE_ID_BIT);
                BTM_UpdateAddrInfor(btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type, local_rpa);
            } else {
                btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type &= (~BLE_ADDR_TYPE_ID_BIT);
            }
#endif
        }
#if (BLE_PRIVACY_SPT == TRUE )
        peer_addr_type = bda_type;
        match = btm_identity_addr_to_random_pseudo (bda, &bda_type, FALSE);

        /* possibly receive connection complete with resolvable random on
           slave role while the device has been paired */

        /* It will cause that scanner doesn't send scan request to advertiser
        * which has sent IRK to us and we have stored the IRK in controller.
        * It is a hardware limitation. The preliminary solution is not to
        * send key to the controller, but to resolve the random address in host.
        * so we need send the real address information to controller to connect.
        * Once the connection is successful, resolve device address whether it is
        * slave or master*/

#if CONTROLLER_RPA_LIST_ENABLE
        if (!match && role == HCI_ROLE_SLAVE && bda_type != BLE_ADDR_PUBLIC && BTM_BLE_IS_RESOLVE_BDA(bda)) {
#else
        if (!match && bda_type != BLE_ADDR_PUBLIC && BTM_BLE_IS_RESOLVE_BDA(bda)) {
#endif
            // save the enhanced value to used in btm_ble_resolve_random_addr_on_conn_cmpl func.
            temp_enhanced = enhanced;
            btm_ble_resolve_random_addr(bda, btm_ble_resolve_random_addr_on_conn_cmpl, p_data);
            // set back the temp enhanced to default after used.
            temp_enhanced = FALSE;
        } else
#endif
        {
            STREAM_TO_UINT16   (conn_interval, p);
            STREAM_TO_UINT16   (conn_latency, p);
            STREAM_TO_UINT16   (conn_timeout, p);
            handle = HCID_GET_HANDLE (handle);

            btm_ble_connected(bda, handle, HCI_ENCRYPT_MODE_DISABLED, role, bda_type, match);
            l2cble_conn_comp (handle, role, bda, bda_type, conn_interval,
                              conn_latency, conn_timeout);

#if (BLE_PRIVACY_SPT == TRUE)
            if (enhanced) {
                btm_ble_refresh_local_resolvable_private_addr(bda, local_rpa);

                if (peer_addr_type & BLE_ADDR_TYPE_ID_BIT) {
                    btm_ble_refresh_peer_resolvable_private_addr(bda, peer_rpa, BLE_ADDR_RANDOM);
                }
            }
#endif
        }
    } else {
        role = HCI_ROLE_UNKNOWN;
        if (status != HCI_ERR_DIRECTED_ADVERTISING_TIMEOUT) {
            btm_ble_set_conn_st(BLE_CONN_IDLE);
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
            btm_ble_disable_resolving_list(BTM_BLE_RL_INIT, TRUE);
#endif
        } else {
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
            btm_cb.ble_ctr_cb.inq_var.adv_mode  = BTM_BLE_ADV_DISABLE;
            btm_ble_disable_resolving_list(BTM_BLE_RL_ADV, TRUE);
#endif
        }

    }

    BOOLEAN bg_con = btm_ble_update_mode_operation(role, bda, status);
    if (status != HCI_SUCCESS && !bg_con) {
        // notify connection failed
        l2c_link_hci_disc_comp (handle, status);
#if (SMP_INCLUDED == TRUE)
        /* Notify security manager */
        btm_sec_disconnected (handle, status);
#endif  ///SMP_INCLUDED == TRUE
    }
}



/*****************************************************************************
** Function btm_ble_create_ll_conn_complete
**
** Description LE connection complete.
**
******************************************************************************/
void btm_ble_create_ll_conn_complete (UINT8 status)
{
    if (status != HCI_SUCCESS) {
        btm_ble_set_conn_st(BLE_CONN_IDLE);
        btm_ble_update_mode_operation(HCI_ROLE_UNKNOWN, NULL, status);
        if(l2cb.is_ble_connecting) {
            /* see L2CA_CancelBleConnectReq() */
            tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(l2cb.ble_connecting_bda, BT_TRANSPORT_LE);
            /* Do not remove lcb if an LE link is already up as a peripheral */
            if (p_lcb != NULL &&
                    !(p_lcb->link_role == HCI_ROLE_SLAVE && BTM_LE_ACL_IS_CONNECTED(l2cb.ble_connecting_bda))) {
                p_lcb->disc_reason = L2CAP_CONN_CANCEL;
                l2cu_release_lcb (p_lcb);
            }
        }
    }
}

/*****************************************************************************
** Function btm_ble_create_conn_cancel_complete
**
** Description LE connection cancel complete.
**
******************************************************************************/
void btm_ble_create_conn_cancel_complete (UINT8 *p)
{
    UINT8       status;

    STREAM_TO_UINT8 (status, p);

    switch (status) {
    case HCI_SUCCESS:
        if (btm_ble_get_conn_st() == BLE_CONN_CANCEL) {
            btm_ble_set_conn_st (BLE_CONN_IDLE);
        }
        break;
    default:
        break;
    }
}

/*****************************************************************************
**  Function        btm_proc_smp_cback
**
**  Description     This function is the SMP callback handler.
**
******************************************************************************/
#if (SMP_INCLUDED == TRUE)
UINT8 btm_proc_smp_cback(tSMP_EVT event, BD_ADDR bd_addr, tSMP_EVT_DATA *p_data)
{
    tBTM_SEC_DEV_REC    *p_dev_rec = btm_find_dev (bd_addr);
    UINT8 res = 0;

    BTM_TRACE_DEBUG ("btm_proc_smp_cback event = %d", event);

    if (p_dev_rec != NULL) {
        switch (event) {
        case SMP_IO_CAP_REQ_EVT:
            btm_ble_io_capabilities_req(p_dev_rec, (tBTM_LE_IO_REQ *)&p_data->io_req);
            break;

        case SMP_BR_KEYS_REQ_EVT:
            btm_ble_br_keys_req(p_dev_rec, (tBTM_LE_IO_REQ *)&p_data->io_req);
            break;

        case SMP_PASSKEY_REQ_EVT:
        case SMP_PASSKEY_NOTIF_EVT:
        case SMP_OOB_REQ_EVT:
        case SMP_NC_REQ_EVT:
        case SMP_SC_OOB_REQ_EVT:
            /* fall through */
            p_dev_rec->sec_flags |= BTM_SEC_LE_AUTHENTICATED;

        case SMP_SEC_REQUEST_EVT:
            if (event == SMP_SEC_REQUEST_EVT && btm_cb.pairing_state != BTM_PAIR_STATE_IDLE) {
                BTM_TRACE_DEBUG("%s: Ignoring SMP Security request", __func__);
                break;
            }
            memcpy (btm_cb.pairing_bda, bd_addr, BD_ADDR_LEN);
            p_dev_rec->sec_state = BTM_SEC_STATE_AUTHENTICATING;
            btm_cb.pairing_flags |= BTM_PAIR_FLAGS_LE_ACTIVE;
        /* fall through */

        case SMP_COMPLT_EVT:
            if (btm_cb.api.p_le_callback) {
                /* the callback function implementation may change the IO capability... */
                BTM_TRACE_DEBUG ("btm_cb.api.p_le_callback=%p", btm_cb.api.p_le_callback );
                (*btm_cb.api.p_le_callback) (event, bd_addr, (tBTM_LE_EVT_DATA *)p_data);
            }

            if (event == SMP_COMPLT_EVT) {
                BTM_TRACE_DEBUG ("evt=SMP_COMPLT_EVT before update sec_level=0x%x sec_flags=0x%x", p_data->cmplt.sec_level , p_dev_rec->sec_flags );

                res = (p_data->cmplt.reason == SMP_SUCCESS) ? BTM_SUCCESS : BTM_ERR_PROCESSING;

                BTM_TRACE_DEBUG ("after update result=%d sec_level=0x%x sec_flags=0x%x",
                                 res, p_data->cmplt.sec_level , p_dev_rec->sec_flags );

                if (p_data->cmplt.is_pair_cancel && btm_cb.api.p_bond_cancel_cmpl_callback ) {
                    BTM_TRACE_DEBUG ("Pairing Cancel completed");
                    (*btm_cb.api.p_bond_cancel_cmpl_callback)(BTM_SUCCESS);
                }
#if BTM_BLE_CONFORMANCE_TESTING == TRUE
                if (res != BTM_SUCCESS) {
                    if (!btm_cb.devcb.no_disc_if_pair_fail && p_data->cmplt.reason != SMP_CONN_TOUT) {
                        BTM_TRACE_DEBUG ("Pairing failed - prepare to remove ACL");
                        l2cu_start_post_bond_timer(p_dev_rec->ble_hci_handle);
                    } else {
                        BTM_TRACE_DEBUG ("Pairing failed - Not Removing ACL");
                        p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;
                    }
                }
#else
                if (res != BTM_SUCCESS && p_data->cmplt.reason != SMP_CONN_TOUT) {
                    BTM_TRACE_DEBUG ("Pairing failed - prepare to remove ACL");
                    l2cu_start_post_bond_timer(p_dev_rec->ble_hci_handle);
                }
#endif

                BTM_TRACE_DEBUG ("btm_cb pairing_state=%x pairing_flags=%x",
                                 btm_cb.pairing_state,
                                 btm_cb.pairing_flags);
                BTM_TRACE_DEBUG ("btm_cb.pairing_bda %02x:%02x:%02x:%02x:%02x:%02x",
                                 btm_cb.pairing_bda[0], btm_cb.pairing_bda[1], btm_cb.pairing_bda[2],
                                 btm_cb.pairing_bda[3], btm_cb.pairing_bda[4], btm_cb.pairing_bda[5]);

                /* Reset btm state only if the callback address matches pairing address*/
                if (memcmp(bd_addr, btm_cb.pairing_bda, BD_ADDR_LEN) == 0) {
                    memset (btm_cb.pairing_bda, 0xff, BD_ADDR_LEN);
                    btm_cb.pairing_state = BTM_PAIR_STATE_IDLE;
                    btm_cb.pairing_flags = 0;
                }

                if (res == BTM_SUCCESS) {
                    p_dev_rec->sec_state = BTM_SEC_STATE_IDLE;
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
                    /* add all bonded device into resolving list if IRK is available*/
                    btm_ble_resolving_list_load_dev(p_dev_rec);
#endif
                }

                btm_sec_dev_rec_cback_event(p_dev_rec, res, TRUE);
            }
            break;

        default:
            BTM_TRACE_DEBUG ("unknown event = %d", event);
            break;


        }
    } else {
        if (event == SMP_SC_LOC_OOB_DATA_UP_EVT) {
            tBTM_LE_EVT_DATA evt_data;
            memcpy(&evt_data.local_oob_data, &p_data->loc_oob_data, sizeof(tSMP_LOC_OOB_DATA));
            if (btm_cb.api.p_le_callback) {
                (*btm_cb.api.p_le_callback)(event, bd_addr, &evt_data);
            }
        } else {
            BTM_TRACE_ERROR("btm_proc_smp_cback received for unknown device");
        }
    }
    return 0;
}
#endif   ///SMP_INCLUDED == TRUE


/*******************************************************************************
**
** Function         BTM_BleDataSignature
**
** Description      This function is called to sign the data using AES128 CMAC
**                  algorithm.
**
** Parameter        bd_addr: target device the data to be signed for.
**                  p_text: singing data
**                  len: length of the data to be signed.
**                  signature: output parameter where data signature is going to
**                             be stored.
**
** Returns          TRUE if signing successful, otherwise FALSE.
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
BOOLEAN BTM_BleDataSignature (BD_ADDR bd_addr, UINT8 *p_text, UINT16 len,
                              BLE_SIGNATURE signature)
{
    tBTM_SEC_DEV_REC *p_rec = btm_find_dev (bd_addr);

    BTM_TRACE_DEBUG ("%s", __func__);
#endif  ///SMP_INCLUDED == TRUE
    BOOLEAN ret = FALSE;
#if (SMP_INCLUDED == TRUE)
    if (p_rec == NULL) {
        BTM_TRACE_ERROR("%s-data signing can not be done from unknown device", __func__);
    } else {
        UINT8 *p_mac = (UINT8 *)signature;
        UINT8 *p_buf, *pp;
        if ((p_buf = (UINT8 *)osi_malloc((UINT16)(len + 4))) != NULL) {
            BTM_TRACE_DEBUG("%s-Start to generate Local CSRK", __func__);
            pp = p_buf;
            /* prepare plain text */
            if (p_text) {
                memcpy(p_buf, p_text, len);
                pp = (p_buf + len);
            }

            UINT32_TO_STREAM(pp, p_rec->ble.keys.local_counter);
            UINT32_TO_STREAM(p_mac, p_rec->ble.keys.local_counter);

            if ((ret = aes_cipher_msg_auth_code(p_rec->ble.keys.lcsrk, p_buf, (UINT16)(len + 4),
                                                BTM_CMAC_TLEN_SIZE, p_mac)) == TRUE) {
                btm_ble_increment_sign_ctr(bd_addr, TRUE);
            }

            BTM_TRACE_DEBUG("%s p_mac = %p", __func__, p_mac);
            BTM_TRACE_DEBUG("p_mac[0] = 0x%02x p_mac[1] = 0x%02x p_mac[2] = 0x%02x p_mac[3] = 0x%02x",
                            *p_mac, *(p_mac + 1), *(p_mac + 2), *(p_mac + 3));
            BTM_TRACE_DEBUG("p_mac[4] = 0x%02x p_mac[5] = 0x%02x p_mac[6] = 0x%02x p_mac[7] = 0x%02x",
                            *(p_mac + 4), *(p_mac + 5), *(p_mac + 6), *(p_mac + 7));
            osi_free(p_buf);
        }
    }
    return ret;
}

/*******************************************************************************
**
** Function         BTM_BleVerifySignature
**
** Description      This function is called to verify the data signature
**
** Parameter        bd_addr: target device the data to be signed for.
**                  p_orig:  original data before signature.
**                  len: length of the signing data
**                  counter: counter used when doing data signing
**                  p_comp: signature to be compared against.

** Returns          TRUE if signature verified correctly; otherwise FALSE.
**
*******************************************************************************/
BOOLEAN BTM_BleVerifySignature (BD_ADDR bd_addr, UINT8 *p_orig, UINT16 len, UINT32 counter,
                                UINT8 *p_comp)
{
    BOOLEAN verified = FALSE;
    tBTM_SEC_DEV_REC *p_rec = btm_find_dev (bd_addr);
    UINT8 p_mac[BTM_CMAC_TLEN_SIZE];

    if (p_rec == NULL || (p_rec && !(p_rec->ble.key_type & BTM_LE_KEY_PCSRK))) {
        BTM_TRACE_ERROR("can not verify signature for unknown device");
    } else if (counter < p_rec->ble.keys.counter) {
        BTM_TRACE_ERROR("signature received with out dated sign counter");
    } else if (p_orig == NULL) {
        BTM_TRACE_ERROR("No signature to verify");
    } else {
        BTM_TRACE_DEBUG ("%s rcv_cnt=%d >= expected_cnt=%d", __func__, counter,
                         p_rec->ble.keys.counter);

        if (aes_cipher_msg_auth_code(p_rec->ble.keys.pcsrk, p_orig, len, BTM_CMAC_TLEN_SIZE, p_mac)) {
            if (memcmp(p_mac, p_comp, BTM_CMAC_TLEN_SIZE) == 0) {
                btm_ble_increment_sign_ctr(bd_addr, FALSE);
                verified = TRUE;
            }
        }
    }
    return verified;
}
#endif  /* SMP_INCLUDED */


/*******************************************************************************
**
** Function         BTM_GetLeSecurityState
**
** Description      This function is called to get security mode 1 flags and
**                  encryption key size for LE peer.
**
** Returns          BOOLEAN TRUE if LE device is found, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN BTM_GetLeSecurityState (BD_ADDR bd_addr, UINT8 *p_le_dev_sec_flags, UINT8 *p_le_key_size)
{
#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
    tBTM_SEC_DEV_REC *p_dev_rec;
    UINT16 dev_rec_sec_flags;
#endif

    *p_le_dev_sec_flags = 0;
    *p_le_key_size = 0;

#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
    if ((p_dev_rec = btm_find_dev (bd_addr)) == NULL) {
        BTM_TRACE_ERROR ("%s fails", __func__);
        return (FALSE);
    }

    if (p_dev_rec->ble_hci_handle == BTM_SEC_INVALID_HANDLE) {
        BTM_TRACE_ERROR ("%s-this is not LE device", __func__);
        return (FALSE);
    }

    dev_rec_sec_flags = p_dev_rec->sec_flags;

    if (dev_rec_sec_flags & BTM_SEC_LE_ENCRYPTED) {
        /* link is encrypted with LTK or STK */
        *p_le_key_size = p_dev_rec->enc_key_size;
        *p_le_dev_sec_flags |= BTM_SEC_LE_LINK_ENCRYPTED;

        *p_le_dev_sec_flags |= (dev_rec_sec_flags & BTM_SEC_LE_AUTHENTICATED)
                               ? BTM_SEC_LE_LINK_PAIRED_WITH_MITM      /* set auth LTK flag */
                               : BTM_SEC_LE_LINK_PAIRED_WITHOUT_MITM;  /* set unauth LTK flag */
    } else if (p_dev_rec->ble.key_type & BTM_LE_KEY_PENC) {
        /* link is unencrypted, still LTK is available */
        *p_le_key_size = p_dev_rec->ble.keys.key_size;

        *p_le_dev_sec_flags |= (dev_rec_sec_flags & BTM_SEC_LE_LINK_KEY_AUTHED)
                               ? BTM_SEC_LE_LINK_PAIRED_WITH_MITM      /* set auth LTK flag */
                               : BTM_SEC_LE_LINK_PAIRED_WITHOUT_MITM;  /* set unauth LTK flag */
    }

    BTM_TRACE_DEBUG ("%s - le_dev_sec_flags: 0x%02x, le_key_size: %d",
                     __func__, *p_le_dev_sec_flags, *p_le_key_size);

    return TRUE;
#else
    return FALSE;
#endif
}

/*******************************************************************************
**
** Function         BTM_BleSecurityProcedureIsRunning
**
** Description      This function indicates if LE security procedure is
**                  currently running with the peer.
**
** Returns          BOOLEAN TRUE if security procedure is running, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN BTM_BleSecurityProcedureIsRunning(BD_ADDR bd_addr)
{
#if (BLE_INCLUDED == TRUE)
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (bd_addr);

    if (p_dev_rec == NULL) {
        BTM_TRACE_ERROR ("%s device with BDA: %08x%04x is not found",
                         __func__, (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                         (bd_addr[4] << 8) + bd_addr[5]);
        return FALSE;
    }

    return (p_dev_rec->sec_state == BTM_SEC_STATE_ENCRYPTING ||
            p_dev_rec->sec_state == BTM_SEC_STATE_AUTHENTICATING);
#else
    return FALSE;
#endif
}

/*******************************************************************************
**
** Function         BTM_BleGetSupportedKeySize
**
** Description      This function gets the maximum encryption key size in bytes
**                  the local device can support.
**                  record.
**
** Returns          the key size or 0 if the size can't be retrieved.
**
*******************************************************************************/
extern UINT8 BTM_BleGetSupportedKeySize (BD_ADDR bd_addr)
{
#ifndef L2CAP_LE_COC_INCLUDED
#define L2CAP_LE_COC_INCLUDED   FALSE
#endif
#if ((BLE_INCLUDED == TRUE) && (L2CAP_LE_COC_INCLUDED == TRUE))
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (bd_addr);
    tBTM_LE_IO_REQ dev_io_cfg;
    UINT8 callback_rc;

    if (!p_dev_rec) {
        BTM_TRACE_ERROR ("%s device with BDA: %08x%04x is not found",
                         __func__, (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                         (bd_addr[4] << 8) + bd_addr[5]);
        return 0;
    }

    if (btm_cb.api.p_le_callback == NULL) {
        BTM_TRACE_ERROR ("%s can't access supported key size", __func__);
        return 0;
    }

    callback_rc = (*btm_cb.api.p_le_callback) (BTM_LE_IO_REQ_EVT, p_dev_rec->bd_addr,
                  (tBTM_LE_EVT_DATA *) &dev_io_cfg);

    if (callback_rc != BTM_SUCCESS) {
        BTM_TRACE_ERROR ("%s can't access supported key size", __func__);
        return 0;
    }

    BTM_TRACE_DEBUG ("%s device supports key size = %d", __func__, dev_io_cfg.max_key_size);
    return (dev_io_cfg.max_key_size);
#else
    return 0;
#endif
}

/*******************************************************************************
**  Utility functions for LE device IR/ER generation
*******************************************************************************/
/*******************************************************************************
**
** Function         btm_notify_new_key
**
** Description      This function is to notify application new keys have been
**                  generated.
**
** Returns          void
**
*******************************************************************************/
#if (SMP_INCLUDED == TRUE)
static void btm_notify_new_key(UINT8 key_type)
{
    tBTM_BLE_LOCAL_KEYS *p_locak_keys = NULL;

    BTM_TRACE_DEBUG ("btm_notify_new_key key_type=%d", key_type);

    if (btm_cb.api.p_le_key_callback) {
        switch (key_type) {
        case BTM_BLE_KEY_TYPE_ID:
            BTM_TRACE_DEBUG ("BTM_BLE_KEY_TYPE_ID");
            p_locak_keys = (tBTM_BLE_LOCAL_KEYS *)&btm_cb.devcb.id_keys;
            break;

        case BTM_BLE_KEY_TYPE_ER:
            BTM_TRACE_DEBUG ("BTM_BLE_KEY_TYPE_ER");
            p_locak_keys = (tBTM_BLE_LOCAL_KEYS *)&btm_cb.devcb.ble_encryption_key_value;
            break;

        default:
            BTM_TRACE_ERROR("unknown key type: %d", key_type);
            break;
        }
        if (p_locak_keys != NULL) {
            (*btm_cb.api.p_le_key_callback) (key_type, p_locak_keys);
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_process_er2
**
** Description      This function is called when ER is generated, store it in
**                  local control block.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_process_er2(tBTM_RAND_ENC *p)
{
    BTM_TRACE_DEBUG ("btm_ble_process_er2");

    if (p && p->opcode == HCI_BLE_RAND) {
        memcpy(&btm_cb.devcb.ble_encryption_key_value[8], p->param_buf, BT_OCTET8_LEN);
        btm_notify_new_key(BTM_BLE_KEY_TYPE_ER);
    } else {
        BTM_TRACE_ERROR("Generating ER2 exception.");
        memset(&btm_cb.devcb.ble_encryption_key_value, 0, sizeof(BT_OCTET16));
    }
}

/*******************************************************************************
**
** Function         btm_ble_process_er
**
** Description      This function is called when ER is generated, store it in
**                  local control block.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_process_er(tBTM_RAND_ENC *p)
{
    BTM_TRACE_DEBUG ("btm_ble_process_er");

    if (p && p->opcode == HCI_BLE_RAND) {
        memcpy(&btm_cb.devcb.ble_encryption_key_value[0], p->param_buf, BT_OCTET8_LEN);

        if (!btsnd_hcic_ble_rand((void *)btm_ble_process_er2)) {
            memset(&btm_cb.devcb.ble_encryption_key_value, 0, sizeof(BT_OCTET16));
            BTM_TRACE_ERROR("Generating ER2 failed.");
        }
    } else {
        BTM_TRACE_ERROR("Generating ER1 exception.");
    }
}

/*******************************************************************************
**
** Function         btm_ble_process_irk
**
** Description      This function is called when IRK is generated, store it in
**                  local control block.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_process_irk(tSMP_ENC *p)
{
    BTM_TRACE_DEBUG ("btm_ble_process_irk");
    if (p && p->opcode == HCI_BLE_ENCRYPT) {
        memcpy(btm_cb.devcb.id_keys.irk, p->param_buf, BT_OCTET16_LEN);
        btm_notify_new_key(BTM_BLE_KEY_TYPE_ID);

#if (CONTROLLER_RPA_LIST_ENABLE == TRUE)
        btm_ble_add_default_entry_to_resolving_list();
#endif

#if (BLE_PRIVACY_SPT == TRUE) && (CONTROLLER_RPA_LIST_ENABLE == FALSE)
        /* if privacy is enabled, new RPA should be calculated */
        if (btm_cb.ble_ctr_cb.privacy_mode != BTM_PRIVACY_NONE) {
            btm_gen_resolvable_private_addr((void *)btm_gen_resolve_paddr_low);
        }
#endif
    } else {
        BTM_TRACE_ERROR("Generating IRK exception.");
    }

    /* proceed generate ER */
    if (!btsnd_hcic_ble_rand((void *)btm_ble_process_er)) {
        BTM_TRACE_ERROR("Generating ER failed.");
    }
}


/*******************************************************************************
**
** Function         btm_ble_process_dhk
**
** Description      This function is called when DHK is calculated, store it in
**                  local control block, and proceed to generate ER, a 128-bits
**                  random number.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_process_dhk(tSMP_ENC *p)
{
    UINT8 btm_ble_irk_pt = 0x01;
    tSMP_ENC output;

    BTM_TRACE_DEBUG ("btm_ble_process_dhk");

    if (p && p->opcode == HCI_BLE_ENCRYPT) {
        memcpy(btm_cb.devcb.id_keys.dhk, p->param_buf, BT_OCTET16_LEN);
        BTM_TRACE_DEBUG("BLE DHK generated.");

        /* IRK = D1(IR, 1) */
        if (!SMP_Encrypt(btm_cb.devcb.id_keys.ir, BT_OCTET16_LEN, &btm_ble_irk_pt,
                         1,   &output)) {
            /* reset all identity root related key */
            memset(&btm_cb.devcb.id_keys, 0, sizeof(tBTM_BLE_LOCAL_ID_KEYS));
        } else {
            btm_ble_process_irk(&output);
        }
    } else {
        /* reset all identity root related key */
        memset(&btm_cb.devcb.id_keys, 0, sizeof(tBTM_BLE_LOCAL_ID_KEYS));
    }
}

/*******************************************************************************
**
** Function         btm_ble_process_ir2
**
** Description      This function is called when IR is generated, proceed to calculate
**                  DHK = Eir({0x03, 0, 0 ...})
**
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_process_ir2(tBTM_RAND_ENC *p)
{
    UINT8 btm_ble_dhk_pt = 0x03;
    tSMP_ENC output;

    BTM_TRACE_DEBUG ("btm_ble_process_ir2");

    if (p && p->opcode == HCI_BLE_RAND) {
        /* remembering in control block */
        memcpy(&btm_cb.devcb.id_keys.ir[8], p->param_buf, BT_OCTET8_LEN);
        /* generate DHK= Eir({0x03, 0x00, 0x00 ...}) */


        SMP_Encrypt(btm_cb.devcb.id_keys.ir, BT_OCTET16_LEN, &btm_ble_dhk_pt,
                    1, &output);
        btm_ble_process_dhk(&output);

        BTM_TRACE_DEBUG("BLE IR generated.");
    } else {
        memset(&btm_cb.devcb.id_keys, 0, sizeof(tBTM_BLE_LOCAL_ID_KEYS));
    }
}

/*******************************************************************************
**
** Function         btm_ble_process_ir
**
** Description      This function is called when IR is generated, proceed to calculate
**                  DHK = Eir({0x02, 0, 0 ...})
**
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_process_ir(tBTM_RAND_ENC *p)
{
    BTM_TRACE_DEBUG ("btm_ble_process_ir");

    if (p && p->opcode == HCI_BLE_RAND) {
        /* remembering in control block */
        memcpy(btm_cb.devcb.id_keys.ir, p->param_buf, BT_OCTET8_LEN);

        if (!btsnd_hcic_ble_rand((void *)btm_ble_process_ir2)) {
            BTM_TRACE_ERROR("Generating IR2 failed.");
            memset(&btm_cb.devcb.id_keys, 0, sizeof(tBTM_BLE_LOCAL_ID_KEYS));
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_reset_id
**
** Description      This function is called to reset LE device identity.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_reset_id( void )
{
    BTM_TRACE_DEBUG ("btm_ble_reset_id");

    /* regenrate Identity Root*/
    if (!btsnd_hcic_ble_rand((void *)btm_ble_process_ir)) {
        BTM_TRACE_DEBUG("Generating IR failed.");
    }
}
#endif  ///SMP_INCLUDED == TRUE

#if BTM_BLE_CONFORMANCE_TESTING == TRUE
/*******************************************************************************
**
** Function         btm_ble_set_no_disc_if_pair_fail
**
** Description      This function indicates that whether no disconnect of the ACL
**                  should be used if pairing failed
**
** Returns          void
**
*******************************************************************************/
void btm_ble_set_no_disc_if_pair_fail(BOOLEAN disable_disc )
{
    BTM_TRACE_DEBUG ("btm_ble_set_disc_enable_if_pair_fail disable_disc=%d", disable_disc);
    btm_cb.devcb.no_disc_if_pair_fail = disable_disc;
}

/*******************************************************************************
**
** Function         btm_ble_set_test_mac_value
**
** Description      This function set test MAC value
**
** Returns          void
**
*******************************************************************************/
void btm_ble_set_test_mac_value(BOOLEAN enable, UINT8 *p_test_mac_val )
{
    BTM_TRACE_DEBUG ("btm_ble_set_test_mac_value enable=%d", enable);
    btm_cb.devcb.enable_test_mac_val = enable;
    memcpy(btm_cb.devcb.test_mac, p_test_mac_val, BT_OCTET8_LEN);
}

/*******************************************************************************
**
** Function         btm_ble_set_test_local_sign_cntr_value
**
** Description      This function set test local sign counter value
**
** Returns          void
**
*******************************************************************************/
void btm_ble_set_test_local_sign_cntr_value(BOOLEAN enable, UINT32 test_local_sign_cntr )
{
    BTM_TRACE_DEBUG ("btm_ble_set_test_local_sign_cntr_value enable=%d local_sign_cntr=%d",
                     enable, test_local_sign_cntr);
    btm_cb.devcb.enable_test_local_sign_cntr = enable;
    btm_cb.devcb.test_local_sign_cntr =  test_local_sign_cntr;
}

/*******************************************************************************
**
** Function         btm_set_random_address
**
** Description      This function set a random address to local controller.
**
** Returns          void
**
*******************************************************************************/
void btm_set_random_address(BD_ADDR random_bda)
{
    tBTM_LE_RANDOM_CB *p_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    BOOLEAN     adv_mode = btm_cb.ble_ctr_cb.inq_var.adv_mode ;

    BTM_TRACE_DEBUG ("btm_set_random_address");

    if (adv_mode  == BTM_BLE_ADV_ENABLE) {
        btsnd_hcic_ble_set_adv_enable (BTM_BLE_ADV_DISABLE);
    }

    memcpy(p_cb->private_addr, random_bda, BD_ADDR_LEN);
    btsnd_hcic_ble_set_random_addr(p_cb->private_addr);

    if (adv_mode  == BTM_BLE_ADV_ENABLE) {
        btsnd_hcic_ble_set_adv_enable (BTM_BLE_ADV_ENABLE);
    }


}

/*******************************************************************************
**
** Function         btm_ble_set_keep_rfu_in_auth_req
**
** Description      This function indicates if RFU bits have to be kept as is
**                  (by default they have to be set to 0 by the sender).
**
** Returns          void
**
*******************************************************************************/
void btm_ble_set_keep_rfu_in_auth_req(BOOLEAN keep_rfu)
{
    BTM_TRACE_DEBUG ("btm_ble_set_keep_rfu_in_auth_req keep_rfus=%d", keep_rfu);
    btm_cb.devcb.keep_rfu_in_auth_req = keep_rfu;
}

#endif /* BTM_BLE_CONFORMANCE_TESTING */

/*******************************************************************************
**
** Function         btm_get_current_conn_params
**
** Description      This function is called to get current connection parameters
**                  information of the device
**
** Returns          TRUE if the information is geted, else FALSE
**
*******************************************************************************/

BOOLEAN btm_get_current_conn_params(BD_ADDR bda, UINT16 *interval, UINT16 *latency, UINT16 *timeout)
{
    if( (interval == NULL) || (latency == NULL) || (timeout == NULL) ) {
        BTM_TRACE_ERROR("%s invalid parameters ", __func__);
        return FALSE;
    }

    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(bda, BT_TRANSPORT_LE);
    if(p_lcb != NULL) {
         (*interval) = p_lcb->current_used_conn_interval;
         (*latency) = p_lcb->current_used_conn_latency;
         (*timeout) = p_lcb->current_used_conn_timeout;
        return TRUE;
    }
    BTM_TRACE_WARNING("%s Device is not connected", __func__);

    return FALSE;
}

uint8_t btm_ble_adv_active_count(void)
{
    uint8_t count = 0;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;

    if (p_cb->state & BTM_BLE_ADVERTISING) {
        count++;
    }

    return count;
}

uint8_t btm_ble_scan_active_count(void)
{
    uint8_t count = 0;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;

    if (p_cb->state & BTM_BLE_SCANNING) {
        count++;
    }

    return count;
}

#if (SMP_INCLUDED == TRUE)
uint8_t btm_ble_sec_dev_active_count(void)
{
    tBTM_SEC_DEV_REC *p_dev_rec = NULL;
    list_node_t *p_node = NULL;
    uint8_t count = 0;

    /* First look for the non-paired devices for the oldest entry */
    for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
        p_dev_rec = list_node(p_node);
        if (p_dev_rec && (p_dev_rec->sec_flags & BTM_SEC_IN_USE) && (p_dev_rec->ble.key_type != BTM_LE_KEY_NONE)) {
            count++;
        }
    }

    return count;
}
#endif

#if (BLE_PRIVACY_SPT == TRUE)
uint8_t btm_ble_privacy_is_enabled(void)
{
    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;
    return (p_cb->privacy_mode != BTM_PRIVACY_NONE);
}
#endif

#endif /* BLE_INCLUDED */
