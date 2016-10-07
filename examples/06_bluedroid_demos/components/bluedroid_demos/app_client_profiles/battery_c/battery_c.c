/***************************************************************
* *
* * This file is for client to execute battery-related operation
* *
***************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "bta_api.h"
#include "bta_gatt_api.h"
#include "controller.h"
     
#include "gatt_int.h"
#include "bt_trace.h"
#include "btm_api.h"
#include "bt_types.h"
#include "gattc_profile.h"

#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
tBTA_GATTC_IF client_if;

tBT_UUID            bas_uuid = {LEN_UUID_16, {UUID_SERVCLASS_BATTERY}};

uint16_t get_uuid16(tBT_UUID* p_uuid)
{
    if(p_uuid->len == LEN_UUID_16)
    {
        return p_uuid->uu.uuid16;
    }
    else if(p_uuid->len == LEN_UUID_128)
    {
        UINT16 u16;
        UINT8 *p = &p_uuid->uu.uuid128[LEN_UUID_128 - 4];
        STREAM_TO_UINT16(u16, p);
        return u16;
    }
    else
    {
        return (UINT16)p_uuid->uu.uuid32;
    }
}

/*fill a GATT ID structure*/
void bta_le_fill_16bits_gatt_id(UINT8 inst_id, UINT16 uuid, tBTA_GATT_ID* p_output)
{
    p_output->inst_id = inst_id;
    p_output->uuid.len = LEN_UUID_16;
    p_output->uuid.uu.uuid16 = uuid;
}

/*fill a service ID structure with a 16 bits service UUID*/
void bta_le_fill_16bits_srvc_id(bool is_pri, UINT8 inst_id, UINT16 srvc_uuid, tBTA_GATT_SRVC_ID* p_output)
{
    memset((void *)p_output, 0, sizeof(tBTA_GATT_SRVC_ID));
    p_output->is_primary = is_pri;
    bta_le_fill_16bits_gatt_id(inst_id, srvc_uuid, &p_output->id);
}

/*fill a char ID structure with a 16 bits char UUID*/
void bta_le_fill_16bits_char_id(UINT8 inst_id, UINT16 char_uuid, tBTA_GATT_ID* p_output)
{
    memset((void *)p_output, 0, sizeof(tBTA_GATT_ID));
    bta_le_fill_16bits_gatt_id(inst_id, char_uuid, p_output);
}

/******************************************************************************
** Function      bas_gattc_callback
**
** Description   battery service register callback function
*******************************************************************************/
static void bas_gattc_callback(tBTA_GATTC_EVT event, tBTA_GATTC* p_data)
{
    switch (event)
    {   
        case BTA_GATTC_REG_EVT:
        {  
            tBTA_GATT_STATUS  status = p_data->reg_oper.status;
            client_if = p_data->reg_oper.client_if;
            LOG_ERROR("BAS register completed: event=%d, status=%d, client_if=%d\n", 
                event, status, client_if);

        }
        break;

        /*connect callback*/
        case BTA_GATTC_OPEN_EVT:
        {
            
            LOG_ERROR("\n%s:device is connected "BT_BD_ADDR_STR", client_if=%d, status=%d, connect_id=%d\n", 
                             __FUNCTION__, BT_BD_ADDR_HEX(p_data->open.remote_bda), p_data->open.client_if,
                             p_data->open.status, p_data->open.conn_id);
            /*return whether the remote device is currently connected*/
            int is_connected = BTA_DmGetConnectionState(p_data->open.remote_bda);
            LOG_ERROR("is_connected=%d\n",is_connected);
            /*get the energy info of the controller*/

            /*read battery level*/
            int conn_id = p_data->open.conn_id;

            /*discover service*/
            BTA_GATTC_ServiceSearchRequest(conn_id, NULL);

        }
        break;

        case BTA_GATTC_SEARCH_RES_EVT:
        {
           // tBTA_GATTC_SRVC_RES service_result;
            LOG_ERROR("find the service,uuid=0x%x, is_primary=%d\n",
                get_uuid16(&p_data->srvc_res.service_uuid.id.uuid), 
                p_data->srvc_res.service_uuid.is_primary);
        }
        break;

        case BTA_GATTC_SEARCH_CMPL_EVT:
        {
            LOG_ERROR("search service complete, conn_id=%d,status=%d\n", p_data->search_cmpl.conn_id,
                p_data->search_cmpl.status);

            /*get first characteristic of battey service*/
            LOG_ERROR("get first characteristic of battery service\n");
            tBTA_GATT_STATUS    status;
            tBTA_GATT_SRVC_ID   battery_srvc_id;
            tBTA_GATTC_CHAR_ID  out_char_id;
            tGATT_CHAR_PROP     out_char_prop;
            bta_le_fill_16bits_srvc_id(TRUE, 0, UUID_SERVCLASS_BATTERY, &battery_srvc_id);
            status = BTA_GATTC_GetFirstChar(p_data->search_cmpl.conn_id, &battery_srvc_id, NULL, 
                &out_char_id, &out_char_prop);
            if(status == 0)
            {
                LOG_ERROR("the first char:srvc_id=0x%x,char_id=0x%x, property = %d\n", 
                    get_uuid16(&out_char_id.srvc_id.id.uuid), get_uuid16(&out_char_id.char_id.uuid),
                    out_char_prop);
                /*read battery level*/
                tBTA_GATTC_CHAR_ID battery_char_id;
                bta_le_fill_16bits_srvc_id(TRUE, 0, UUID_SERVCLASS_BATTERY, &battery_char_id.srvc_id);
                bta_le_fill_16bits_char_id(0, GATT_UUID_BATTERY_LEVEL, &battery_char_id.char_id);
            
                BTA_GATTC_ReadCharacteristic(p_data->search_cmpl.conn_id, &battery_char_id, 
                    BTA_GATT_AUTH_REQ_NONE); 
            }
        }
        break;

        case BTA_GATTC_READ_CHAR_EVT:
        {
            
            LOG_ERROR("\nread characteristic:connect_id=%d, status=%d\n", 
                p_data->read.conn_id, p_data->read.status);
            LOG_ERROR("srvc_id=0x%x,char_id=0x%x,descr_type=0x%x\n", 
                get_uuid16(&p_data->read.srvc_id.id.uuid), 
                get_uuid16(&p_data->read.char_id.uuid), 
                get_uuid16(&p_data->read.descr_type.uuid));
            if(get_uuid16(&p_data->read.descr_type.uuid) != GATT_UUID_CHAR_AGG_FORMAT
                && p_data->read.p_value->unformat.len > 0
                && p_data->read.p_value->unformat.p_value != NULL)
            {
                LOG_ERROR("read the value: len=%d, value=%d\n", p_data->read.p_value->unformat.len,
                    *(p_data->read.p_value->unformat.p_value));
            }
        }
        break;

        default:
        LOG_ERROR("unsettled event: %d\n", event);
    }

}

/***************************************************************
**
** Function     bac_register
**
** Description  register app for battery service
**
****************************************************************/
void bac_register(void)
{
    BTA_GATTC_AppRegister(&bas_uuid, bas_gattc_callback);    
    
}

