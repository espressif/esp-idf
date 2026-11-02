/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
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
#include "common/bt_target.h"

#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)

#include "common/bt_defs.h"
#include <string.h>
#include "gap_int.h"
#include "stack/gap_api.h"
#include "stack/gattdefs.h"
#include "stack/gatt_api.h"
#include "btm_int.h"
#include "stack/sdpdefs.h"

#define GAP_MAX_NUM_INC_SVR       0
#define GAP_MAX_ATTR_NUM          (2 * GAP_MAX_CHAR_NUM + GAP_MAX_NUM_INC_SVR + 1)

static void gap_ble_s_attr_request_cback (UINT16 conn_id, UINT32 trans_id, tGATTS_REQ_TYPE op_code, tGATTS_DATA *p_data);

static const tGATT_CBACK gap_cback = {
    NULL,
    NULL,
    NULL,
    NULL,
    gap_ble_s_attr_request_cback,
    NULL,
    NULL
};



/*******************************************************************************
**   GAP Attributes Database Request callback
*******************************************************************************/
tGATT_STATUS gap_read_attr_value (UINT16 handle, tGATT_VALUE *p_value, BOOLEAN is_long)
{
    tGAP_ATTR   *p_db_attr = gap_cb.gap_attr;
    UINT8       *p = p_value->value, i;
    UINT16      offset = p_value->offset;
    UINT8       *p_dev_name = NULL;

    for (i = 0; i < GAP_MAX_CHAR_NUM; i ++, p_db_attr ++) {
        if (handle == p_db_attr->handle) {
            if (p_db_attr->uuid != GATT_UUID_GAP_DEVICE_NAME &&
                    is_long == TRUE) {
                return GATT_NOT_LONG;
            }

            switch (p_db_attr->uuid) {
            case GATT_UUID_GAP_DEVICE_NAME:
                BTM_ReadLocalDeviceName((char **)&p_dev_name, BT_DEVICE_TYPE_BLE);
                if (strlen ((char *)p_dev_name) > GATT_MAX_ATTR_LEN) {
                    p_value->len = GATT_MAX_ATTR_LEN;
                } else {
                    p_value->len = (UINT16)strlen ((char *)p_dev_name);
                }

                if (offset > p_value->len) {
                    return GATT_INVALID_OFFSET;
                } else {
                    p_value->len -= offset;
                    p_dev_name += offset;
                    ARRAY_TO_STREAM(p, p_dev_name, p_value->len);
                    GAP_TRACE_EVENT("GATT_UUID_GAP_DEVICE_NAME len=0x%04x", p_value->len);
                }
                break;

            case GATT_UUID_GAP_ICON:
                UINT16_TO_STREAM(p, p_db_attr->attr_value.icon);
                p_value->len = 2;
                break;

            case GATT_UUID_GAP_PREF_CONN_PARAM:
                UINT16_TO_STREAM(p, p_db_attr->attr_value.conn_param.int_min); /* int_min */
                UINT16_TO_STREAM(p, p_db_attr->attr_value.conn_param.int_max); /* int_max */
                UINT16_TO_STREAM(p, p_db_attr->attr_value.conn_param.latency); /* latency */
                UINT16_TO_STREAM(p, p_db_attr->attr_value.conn_param.sp_tout);  /* sp_tout */
                p_value->len = 8;
                break;

            /* address resolution */
            case GATT_UUID_GAP_CENTRAL_ADDR_RESOL:
                UINT8_TO_STREAM(p, p_db_attr->attr_value.addr_resolution);
                p_value->len = 1;
                break;
#if (BT_GATTS_SECURITY_LEVELS_CHAR == TRUE)
            case GATT_UUID_GAP_GATT_SECURITY_LEVELS:
                UINT16_TO_STREAM(p, p_db_attr->attr_value.security_level);
                p_value->len = 2;
                break;
#endif // (BT_GATTS_SECURITY_LEVELS_CHAR == TRUE)
#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
            case GATT_UUID_GAP_KEY_MATERIAL:
                ARRAY_TO_STREAM(p, p_db_attr->attr_value.key_material.session_key, GAP_KEY_MATERIAL_SESSION_KEY_SIZE);
                ARRAY_TO_STREAM(p, p_db_attr->attr_value.key_material.iv, GAP_KEY_MATERIAL_IV_SIZE);
                p_value->len = GAP_KEY_MATERIAL_SIZE;
                break;
#endif // (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
            }
            return GATT_SUCCESS;
        }
    }
    return GATT_NOT_FOUND;
}

/*******************************************************************************
**   GAP Attributes Database Read/Read Blob Request process
*******************************************************************************/
tGATT_STATUS gap_proc_read (tGATTS_REQ_TYPE type, tGATT_READ_REQ *p_data, tGATTS_RSP *p_rsp)
{
    tGATT_STATUS    status = GATT_NO_RESOURCES;
    UNUSED(type);

    if (p_data->is_long) {
        p_rsp->attr_value.offset = p_data->offset;
    }

    p_rsp->attr_value.handle = p_data->handle;

    status = gap_read_attr_value(p_data->handle, &p_rsp->attr_value, p_data->is_long);

    return status;
}

/******************************************************************************
**
** Function         gap_proc_write_req
**
** Description      GAP ATT server process a write request.
**
** Returns          void.
**
*******************************************************************************/
UINT8 gap_proc_write_req( tGATTS_REQ_TYPE type, tGATT_WRITE_REQ *p_data)
{
    tGAP_ATTR   *p_db_attr = gap_cb.gap_attr;
    UINT8   i;
    UNUSED(type);

    for (i = 0; i < GAP_MAX_CHAR_NUM; i ++, p_db_attr ++) {
        if (p_data->handle == p_db_attr->handle) {
            switch (p_db_attr->uuid) {
                #if (GATTS_DEVICE_NAME_WRITABLE == TRUE)
                case GATT_UUID_GAP_DEVICE_NAME: {
                    if (p_data->len > BD_NAME_LEN) {
                        return GATT_INVALID_ATTR_LEN;
                    }
                    UINT8 *p_val = p_data->value;
                    p_val[p_data->len] = '\0';
                    BTM_SetLocalDeviceName((char *)p_val, BT_DEVICE_TYPE_BLE);
                    return GATT_SUCCESS;
                }
                #endif
                #if (GATTS_APPEARANCE_WRITABLE == TRUE)
                case GATT_UUID_GAP_ICON: {
                    UINT8 *p_val = p_data->value;
                    if (p_data->len != sizeof(UINT16)) {
                        return GATT_INVALID_ATTR_LEN;
                    }
                    STREAM_TO_UINT16(p_db_attr->attr_value.icon, p_val);
                    return GATT_SUCCESS;
                }
                #endif
                default:
                    break;
            }
            return GATT_WRITE_NOT_PERMIT;
        }
    }

    return GATT_NOT_FOUND;
}

/******************************************************************************
**
** Function         gap_ble_s_attr_request_cback
**
** Description      GAP ATT server attribute access request callback.
**
** Returns          void.
**
*******************************************************************************/
void gap_ble_s_attr_request_cback (UINT16 conn_id, UINT32 trans_id,
                                   tGATTS_REQ_TYPE type, tGATTS_DATA *p_data)
{
    UINT8       status = GATT_INVALID_PDU;
    tGATTS_RSP  rsp_msg;
    BOOLEAN     ignore = FALSE;

    GAP_TRACE_EVENT("gap_ble_s_attr_request_cback : recv type (0x%02x)", type);

    memset(&rsp_msg, 0, sizeof(tGATTS_RSP));

    switch (type) {
    case GATTS_REQ_TYPE_READ:
        status = gap_proc_read(type, &p_data->read_req, &rsp_msg);
        break;

    case GATTS_REQ_TYPE_WRITE:
        if (!p_data->write_req.need_rsp) {
            ignore = TRUE;
        }

        status = gap_proc_write_req(type, &p_data->write_req);
        break;

    case GATTS_REQ_TYPE_WRITE_EXEC:
        ignore = TRUE;
        GAP_TRACE_EVENT("ignore GATTS_REQ_TYPE_WRITE_EXEC");
        break;

    case GATTS_REQ_TYPE_MTU:
        GAP_TRACE_EVENT("Get MTU exchange new mtu size: %d", p_data->mtu);
        ignore = TRUE;
        break;

    default:
        GAP_TRACE_EVENT("Unknown/unexpected LE GAP ATT request: 0x%02x", type);
        break;
    }

    if (!ignore) {
        GATTS_SendRsp (conn_id, trans_id, status, &rsp_msg);
    }
}

/*******************************************************************************
**
** Function         btm_ble_att_db_init
**
** Description      GAP ATT database initialization.
**
** Returns          void.
**
*******************************************************************************/
void gap_attr_db_init(void)
{
    tBT_UUID        app_uuid = {LEN_UUID_128, {0}};
    tBT_UUID        uuid     = {LEN_UUID_16, {UUID_SERVCLASS_GAP_SERVER}};
    UINT16          service_handle;
    tGAP_ATTR       *p_db_attr = &gap_cb.gap_attr[0];
    tGATT_STATUS    status;

    /* Fill our internal UUID with a fixed pattern 0x82 */
    memset (&app_uuid.uu.uuid128, 0x82, LEN_UUID_128);
    memset(gap_cb.gap_attr, 0, sizeof(tGAP_ATTR) *GAP_MAX_CHAR_NUM);

    gap_cb.gatt_if = GATT_Register(&app_uuid, &gap_cback);

    GATT_StartIf(gap_cb.gatt_if);

    /* Create a GAP service */
    service_handle = GATTS_CreateService (gap_cb.gatt_if, &uuid, 0, GAP_MAX_ATTR_NUM, TRUE);

    GAP_TRACE_EVENT ("gap_attr_db_init service_handle = %d", service_handle);

    /* add Device Name Characteristic
    */
    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = p_db_attr->uuid = GATT_UUID_GAP_DEVICE_NAME;
    p_db_attr->handle = GATTS_AddCharacteristic(service_handle, &uuid,
                        #if (GATTS_DEVICE_NAME_WRITABLE == TRUE)
                        GATT_PERM_READ | GATT_PERM_WRITE,
                        GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE_NR,
                        #else
                        GATT_PERM_READ, GATT_CHAR_PROP_BIT_READ,
                        #endif
                        NULL, NULL);
    p_db_attr ++;

    /* add Icon characteristic
    */
    uuid.uu.uuid16   = p_db_attr->uuid = GATT_UUID_GAP_ICON;
    p_db_attr->handle = GATTS_AddCharacteristic(service_handle, &uuid,
                        #if (GATTS_APPEARANCE_WRITABLE == TRUE)
                        GATT_PERM_READ | GATT_PERM_WRITE,
                        GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE_NR,
                        #else
                        GATT_PERM_READ, GATT_CHAR_PROP_BIT_READ,
                        #endif
                        NULL, NULL);
    p_db_attr ++;

#if ((defined BTM_PERIPHERAL_ENABLED) && (BTM_PERIPHERAL_ENABLED == TRUE))
    /* Only needed for peripheral testing */
    /* add preferred connection parameter characteristic
    */
    uuid.uu.uuid16 = p_db_attr->uuid = GATT_UUID_GAP_PREF_CONN_PARAM;
    p_db_attr->attr_value.conn_param.int_max = GAP_PREFER_CONN_INT_MAX; /* 6 */
    p_db_attr->attr_value.conn_param.int_min = GAP_PREFER_CONN_INT_MIN; /* 0 */
    p_db_attr->attr_value.conn_param.latency = GAP_PREFER_CONN_LATENCY; /* 0 */
    p_db_attr->attr_value.conn_param.sp_tout = GAP_PREFER_CONN_SP_TOUT; /* 2000 */
    p_db_attr->handle = GATTS_AddCharacteristic(service_handle,
                        &uuid,
                        GATT_PERM_READ,
                        GATT_CHAR_PROP_BIT_READ,
                        NULL, NULL);
    p_db_attr ++;
#endif

    /* add Central address resolution Characteristic */
    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = p_db_attr->uuid = GATT_UUID_GAP_CENTRAL_ADDR_RESOL;
    p_db_attr->handle = GATTS_AddCharacteristic(service_handle, &uuid,
                        GATT_PERM_READ, GATT_CHAR_PROP_BIT_READ,
                        NULL, NULL);
    p_db_attr->attr_value.addr_resolution = 0;
    p_db_attr++;

#if (BT_GATTS_SECURITY_LEVELS_CHAR == TRUE)
    /* Add LE Security Levels Characteristic */
    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = p_db_attr->uuid = GATT_UUID_GAP_GATT_SECURITY_LEVELS;
    p_db_attr->handle = GATTS_AddCharacteristic(service_handle, &uuid,
                        GATT_PERM_READ, GATT_CHAR_PROP_BIT_READ,
                        NULL, NULL);
    p_db_attr->attr_value.security_level = 0x0101;
    p_db_attr++;
#endif // (BT_GATTS_SECURITY_LEVELS_CHAR == TRUE)

#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
    /* Add Encrypted Data Key Material Characteristic
     * Per Bluetooth spec: readable only when authenticated and authorized,
     * requires encrypted link to read.
     */
    uuid.len = LEN_UUID_16;
    uuid.uu.uuid16 = p_db_attr->uuid = GATT_UUID_GAP_KEY_MATERIAL;
    p_db_attr->handle = GATTS_AddCharacteristic(service_handle, &uuid,
                        GATT_PERM_READ_ENCRYPTED, GATT_CHAR_PROP_BIT_READ,
                        NULL, NULL);
    memset(&p_db_attr->attr_value.key_material, 0, sizeof(tGAP_BLE_KEY_MATERIAL));
    p_db_attr++;
#endif // (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)

    /* start service now */
    memset (&app_uuid.uu.uuid128, 0x81, LEN_UUID_128);

    status = GATTS_StartService(gap_cb.gatt_if, service_handle, GAP_TRANSPORT_SUPPORTED );
#if (CONFIG_BT_STACK_NO_LOG)
    (void) status;
#endif

    GAP_TRACE_EVENT ("GAP App gatt_if: %d  s_hdl = %d start_status=%d",
                     gap_cb.gatt_if, service_handle, status);



}

/*******************************************************************************
**
** Function         GAP_BleAttrDBUpdate
**
** Description      GAP ATT database update.
**
** Returns          void.
**
*******************************************************************************/
void GAP_BleAttrDBUpdate(UINT16 attr_uuid, tGAP_BLE_ATTR_VALUE *p_value)
{
    tGAP_ATTR  *p_db_attr = gap_cb.gap_attr;
    UINT8       i = 0;

    GAP_TRACE_EVENT("GAP_BleAttrDBUpdate attr_uuid=0x%04x\n", attr_uuid);

    if (p_value == NULL) {
        GAP_TRACE_ERROR("GAP_BleAttrDBUpdate: NULL pointer parameter");
        return;
    }

    for (i = 0; i < GAP_MAX_CHAR_NUM; i ++, p_db_attr ++) {
        if (p_db_attr->uuid == attr_uuid) {
            GAP_TRACE_EVENT("Found attr_uuid=0x%04x\n", attr_uuid);

            switch (attr_uuid) {
            case GATT_UUID_GAP_ICON:
                p_db_attr->attr_value.icon  =  p_value->icon;
                break;

            case GATT_UUID_GAP_PREF_CONN_PARAM:
                memcpy((void *)&p_db_attr->attr_value.conn_param,
                       (const void *)&p_value->conn_param, sizeof(tGAP_BLE_PREF_PARAM));
                break;

            case GATT_UUID_GAP_DEVICE_NAME:
                BTM_SetLocalDeviceName((char *)p_value->p_dev_name, BT_DEVICE_TYPE_BLE);
                break;

            case GATT_UUID_GAP_CENTRAL_ADDR_RESOL:
                p_db_attr->attr_value.addr_resolution = p_value->addr_resolution;
                break;

#if (BT_GATTS_SECURITY_LEVELS_CHAR == TRUE)
            case GATT_UUID_GAP_GATT_SECURITY_LEVELS:
                p_db_attr->attr_value.security_level = p_value->security_level;
                break;
#endif // (BT_GATTS_SECURITY_LEVELS_CHAR == TRUE)

#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
            case GATT_UUID_GAP_KEY_MATERIAL:
                memcpy(&p_db_attr->attr_value.key_material, &p_value->key_material,
                       sizeof(tGAP_BLE_KEY_MATERIAL));
                break;
#endif // (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)

            }
            break;
        }
    }

    return;
}

#endif  /* BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE*/
