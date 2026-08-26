/******************************************************************************
 *
 *  Copyright (C) 1999-2013 Broadcom Corporation
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
//#include "bt_utils.h"
//#include "stack/gatt_api.h"

#define LOG_TAG "bt_srvc"
//#include "osi/include/log.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"

#include "bta/bta_api.h"
#include "bta/bta_gatt_api.h"
#include "device/controller.h"

#include "gatt_int.h"
#include "common/bt_trace.h"
#include "stack/btm_api.h"
#include "stack/bt_types.h"
#include "dis_api.h"

#if BLE_INCLUDED == TRUE

#define UINT64_TO_STREAM(p, u64) {*(p)++ = (UINT8)(u64);       *(p)++ = (UINT8)((u64) >> 8);*(p)++ = (UINT8)((u64) >> 16); *(p)++ = (UINT8)((u64) >> 24); \
                                    *(p)++ = (UINT8)((u64) >> 32); *(p)++ = (UINT8)((u64) >> 40);*(p)++ = (UINT8)((u64) >> 48); *(p)++ = (UINT8)((u64) >> 56);}

#define STREAM_TO_UINT64(u64, p) {u64 = (((UINT64)(*(p))) + ((((UINT64)(*((p) + 1)))) << 8) + ((((UINT64)(*((p) + 2)))) << 16) + ((((UINT64)(*((p) + 3)))) << 24) \
                                  + ((((UINT64)(*((p) + 4)))) << 32) + ((((UINT64)(*((p) + 5)))) << 40) + ((((UINT64)(*((p) + 6)))) << 48) + ((((UINT64)(*((p) + 7)))) << 56)); (p) += 8;}

esp_bt_uuid_t           uuid = {LEN_UUID_16, {UUID_SERVCLASS_DEVICE_INFO}};
UINT16                  i = 0;
tDIS_ATTR_MASK          dis_mask;
static const UINT16     dis_attr_uuid[DIS_MAX_CHAR_NUM] = {
    GATT_UUID_SYSTEM_ID,
    GATT_UUID_MODEL_NUMBER_STR,
    GATT_UUID_SERIAL_NUMBER_STR,
    GATT_UUID_FW_VERSION_STR,
    GATT_UUID_HW_VERSION_STR,
    GATT_UUID_SW_VERSION_STR,
    GATT_UUID_MANU_NAME,
    GATT_UUID_IEEE_DATA,
    GATT_UUID_PNP_ID
};

tDIS_CB dis_cb;

static tDIS_ATTR_MASK dis_uuid_to_attr(UINT16 uuid)
{
    switch (uuid) {
    case GATT_UUID_SYSTEM_ID:
        return DIS_ATTR_SYS_ID_BIT;
    case GATT_UUID_MODEL_NUMBER_STR:
        return DIS_ATTR_MODEL_NUM_BIT;
    case GATT_UUID_SERIAL_NUMBER_STR:
        return DIS_ATTR_SERIAL_NUM_BIT;
    case GATT_UUID_FW_VERSION_STR:
        return DIS_ATTR_FW_NUM_BIT;
    case GATT_UUID_HW_VERSION_STR:
        return DIS_ATTR_HW_NUM_BIT;
    case GATT_UUID_SW_VERSION_STR:
        return DIS_ATTR_SW_NUM_BIT;
    case GATT_UUID_MANU_NAME:
        return DIS_ATTR_MANU_NAME_BIT;
    case GATT_UUID_IEEE_DATA:
        return DIS_ATTR_IEEE_DATA_BIT;
    case GATT_UUID_PNP_ID:
        return DIS_ATTR_PNP_ID_BIT;
    default:
        return 0;
    };
}

/*******************************************************************************
**   dis_valid_handle_range
**
**   validate a handle to be a DIS attribute handle or not.
*******************************************************************************/
BOOLEAN dis_valid_handle_range(UINT16 handle)
{
    if (handle >= dis_cb.service_handle && handle <= dis_cb.max_handle) {
        return TRUE;
    } else {
        return FALSE;
    }
}
/*******************************************************************************
**   dis_write_attr_value
**
**   Process write DIS attribute request.
*******************************************************************************/
UINT8 dis_write_attr_value(tGATT_WRITE_REQ *p_data, esp_gatt_status_t *p_status)
{
    UNUSED(p_data);

    *p_status = GATT_WRITE_NOT_PERMIT;
    return ESP_GATT_OK;
}
/*******************************************************************************
**   DIS Attributes Database Server Request callback
*******************************************************************************/

/*******************************************************************************
**   dis_s_read_attr_value
**
**   Process read DIS attribute request.
*******************************************************************************/
void dis_s_read_attr_value (tGATTS_DATA *p_data, tGATT_VALUE *p_value, UINT32 trans_id, UINT16 conn_id)
{
    tDIS_DB_ENTRY   *p_db_attr = dis_cb.dis_attr;
    UINT8           *p = p_value->value, i, *pp;
    UINT16          offset = p_data->read_req.offset;
    tGATT_STATUS    st = ESP_GATT_NOT_FOUND;
    UINT16          handle = p_data->read_req.handle;
    bool            is_long = p_data->read_req.is_long;

    for (i = 0; i < DIS_MAX_CHAR_NUM; i ++, p_db_attr ++) {
        if (handle == p_db_attr->handle) {
            if ((p_db_attr->uuid == GATT_UUID_PNP_ID || p_db_attr->uuid == GATT_UUID_SYSTEM_ID) &&
                    is_long == TRUE) {
                st = ESP_GATT_NOT_LONG;
                break;
            }
            st = ESP_GATT_NOT_FOUND;

            switch (p_db_attr->uuid) {
            case GATT_UUID_MANU_NAME:
            case GATT_UUID_MODEL_NUMBER_STR:
            case GATT_UUID_SERIAL_NUMBER_STR:
            case GATT_UUID_FW_VERSION_STR:
            case GATT_UUID_HW_VERSION_STR:
            case GATT_UUID_SW_VERSION_STR:
            case GATT_UUID_IEEE_DATA:
                pp = dis_cb.dis_value.data_string[p_db_attr->uuid - GATT_UUID_MODEL_NUMBER_STR];
                if (pp != NULL) {
                    if (strlen ((char *)pp) > GATT_MAX_ATTR_LEN) {
                        p_value->len = GATT_MAX_ATTR_LEN;
                    } else {
                        p_value->len = (UINT16)strlen ((char *)pp);
                    }
                } else {
                    p_value->len = 0;
                }

                if (offset > p_value->len) {
                    st = ESP_GATT_INVALID_OFFSET;
                    break;
                } else {
                    p_value->len -= offset;
                    pp += offset;
                    ARRAY_TO_STREAM(p, pp, p_value->len);
                    GATT_TRACE_EVENT("GATT_UUID_MANU_NAME len=0x%04x", p_value->len);
                }
                break;


            case GATT_UUID_SYSTEM_ID:
                UINT64_TO_STREAM(p, dis_cb.dis_value.system_id); /* int_min */
                p_value->len = DIS_SYSTEM_ID_SIZE;
                break;

            case  GATT_UUID_PNP_ID:
                UINT8_TO_STREAM(p, dis_cb.dis_value.pnp_id.vendor_id_src);
                UINT16_TO_STREAM(p, dis_cb.dis_value.pnp_id.vendor_id);
                UINT16_TO_STREAM(p, dis_cb.dis_value.pnp_id.product_id);
                UINT16_TO_STREAM(p, dis_cb.dis_value.pnp_id.product_version);
                p_value->len = DIS_PNP_ID_SIZE;
                break;

            }
            break;
        }
    }
    tGATTS_RSP       rsp;
    rsp.attr_value = *p_value;
    esp_ble_gatts_send_rsp(conn_id, trans_id, st, &rsp);

}


/*******************************************************************************
**
** Function         DIS_Init
**
** Description      Initialize the Device Information Service Server.
**
*******************************************************************************/
void DIS_Init (esp_gatts_if_t gatt_if, tDIS_ATTR_MASK dis_attr_mask)
{

    tGATT_STATUS      status;
    dis_mask = dis_attr_mask;
    if (dis_cb.enabled) {
        GATT_TRACE_ERROR("DIS already initalized");
        return;
    }

    memset(&dis_cb, 0, sizeof(tDIS_CB));

    esp_ble_gatts_create_srvc (gatt_if , &uuid, 0, DIS_MAX_ATTR_NUM, TRUE);

}
/*******************************************************************************
**
** Function         dis_AddChar
**
** Description      add characteristic for dis
**
*******************************************************************************/

void dis_AddChar(UINT16 service_id)
{
    //dis_cb.service_handle = service_id;
    //dis_cb.max_handle = service_id + DIS_MAX_ATTR_NUM;
    tDIS_DB_ENTRY        *p_db_attr = &dis_cb.dis_attr[0];
    while (dis_mask != 0 && i < DIS_MAX_CHAR_NUM) {
        uuid.uu.uuid16 = p_db_attr->uuid = dis_attr_uuid[i];
        esp_ble_gatts_add_char(dis_cb.service_handle, &uuid, GATT_PERM_READ,
                               GATT_CHAR_PROP_BIT_READ);
        p_db_attr ++;
        i ++;
        dis_mask >>= 1;
    }
    /*start service*/
    esp_ble_gatts_start_srvc(dis_cb.service_handle);
    dis_cb.enabled = TRUE;
}
/*******************************************************************************
**
** Function         DIS_SrUpdate
**
** Description      Update the DIS server attribute values
**
*******************************************************************************/
tDIS_STATUS DIS_SrUpdate(tDIS_ATTR_BIT dis_attr_bit, tDIS_ATTR *p_info)
{
    UINT8           i = 1;
    tDIS_STATUS     st = DIS_SUCCESS;

    if (dis_attr_bit & DIS_ATTR_SYS_ID_BIT) {
        dis_cb.dis_value.system_id = p_info->system_id;
    } else if (dis_attr_bit & DIS_ATTR_PNP_ID_BIT) {
        dis_cb.dis_value.pnp_id.vendor_id         = p_info->pnp_id.vendor_id;
        dis_cb.dis_value.pnp_id.vendor_id_src     = p_info->pnp_id.vendor_id_src;
        dis_cb.dis_value.pnp_id.product_id        = p_info->pnp_id.product_id;
        dis_cb.dis_value.pnp_id.product_version   = p_info->pnp_id.product_version;
    } else {
        st = DIS_ILLEGAL_PARAM;

        while (dis_attr_bit && i < (DIS_MAX_CHAR_NUM - 1 )) {
            if (dis_attr_bit & (UINT16)(1 << i)) {
                if (dis_cb.dis_value.data_string[i - 1] != NULL) {
                    osi_free(dis_cb.dis_value.data_string[i - 1]);
                }
                /* coverity[OVERRUN-STATIC] False-positive : when i = 8, (1 << i) == DIS_ATTR_PNP_ID_BIT, and it will never come down here
                CID 49902: Out-of-bounds read (OVERRUN_STATIC)
                Overrunning static array "dis_cb.dis_value.data_string", with 7 elements, at position 7 with index variable "i".
                */
                if ((dis_cb.dis_value.data_string[i - 1] = (UINT8 *)osi_malloc((UINT16)(p_info->data_str.len + 1))) != NULL) {

                    memcpy(dis_cb.dis_value.data_string[i - 1], p_info->data_str.p_data, p_info->data_str.len);
                    dis_cb.dis_value.data_string[i - 1][p_info->data_str.len] = 0; /* make sure null terminate */
                    st = DIS_SUCCESS;
                } else {
                    st = DIS_NO_RESOURCES;
                }

                break;
            }
            i ++;
        }
    }
    return st;
}
#endif  /* BLE_INCLUDED */
