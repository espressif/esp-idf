/****************************************************************************
*
* This file is for gatt client. It can scan ble device, connect one device, 
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "bta_api.h"
#include "bta_gatt_api.h"
#include "controller.h"
               
#include "bt_trace.h"
#include "btm_api.h"
#include "bt_types.h"
#include "gattc_profile.h"

#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

tBTA_GATTC_IF client_if;
BD_ADDR obj_addr;
static unsigned char BASE_UUID[16] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
                  
typedef struct {
    uint8_t uu[16];
} bt_uuid_t;

int uuidType(unsigned char* p_uuid)
{
    int i = 0;
    int match = 0;
    int all_zero = 1;
                
    for(i = 0; i != 16; ++i)
    {
        if (i == 12 || i == 13)
            continue;
                              
        if (p_uuid[i] == BASE_UUID[i])
            ++match;
                                        
        if (p_uuid[i] != 0)
            all_zero = 0;
    }
    if (all_zero)
        return 0;
    if (match == 12)
        return LEN_UUID_32;
    if (match == 14)
        return LEN_UUID_16;
    return LEN_UUID_128;
}

static void btif_to_bta_uuid(tBT_UUID *p_dest, bt_uuid_t *p_src)
{
    char *p_byte = (char*)p_src;
   
    int i = 0;
            
    p_dest->len = uuidType(p_src->uu);
                  
    switch (p_dest->len)
    {
        case LEN_UUID_16:
            p_dest->uu.uuid16 = (p_src->uu[13] << 8) + p_src->uu[12];
            break;
                                    
        case LEN_UUID_32:
            p_dest->uu.uuid32  = (p_src->uu[13] <<  8) + p_src->uu[12];
            p_dest->uu.uuid32 += (p_src->uu[15] << 24) + (p_src->uu[14] << 16);
            break;
                                                   
        case LEN_UUID_128:
            for(i = 0; i != 16; ++i)
                p_dest->uu.uuid128[i] = p_byte[i];
                break;
                                                            
        default:         
        LOG_ERROR("%s: Unknown UUID length %d!", __FUNCTION__, p_dest->len);
        break;
    }
}
/*
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

//fill a GATT ID structure
void bta_le_fill_16bits_gatt_id(UINT8 inst_id, UINT16 uuid, tBTA_GATT_ID* p_output)
{
    p_output->inst_id = inst_id;
    p_output->uuid.len = LEN_UUID_16;
    p_output->uuid.uu.uuid16 = uuid;
}

//fill a service ID structure with a 16 bits service UUID
void bta_le_fill_16bits_srvc_id(bool is_pri, UINT8 inst_id, UINT16 srvc_uuid, tBTA_GATT_SRVC_ID* p_output)
{
    memset((void *)p_output, 0, sizeof(tBTA_GATT_SRVC_ID));
    p_output->is_primary = is_pri;
    bta_le_fill_16bits_gatt_id(inst_id, srvc_uuid, &p_output->id);
}

//fill a char ID structure with a 16 bits char UUID
void bta_le_fill_16bits_char_id(UINT8 inst_id, UINT16 char_uuid, tBTA_GATT_ID* p_output)
{
    memset((void *)p_output, 0, sizeof(tBTA_GATT_ID));
    bta_le_fill_16bits_gatt_id(inst_id, char_uuid, p_output);
}
*/
/*get remote name*/
static bool check_remote_name(tBTA_DM_INQ_RES* result, uint8_t* rmt_name, uint8_t* rmt_name_len)
{
    uint8_t *p_rmt_name = NULL;
    uint8_t remote_name_len = 0;
    
    if (result->p_eir) {
        p_rmt_name = BTM_CheckEirData(result->p_eir,
            BTM_EIR_COMPLETE_LOCAL_NAME_TYPE,
            &remote_name_len);
        if (!p_rmt_name)
            p_rmt_name = BTM_CheckEirData(result->p_eir,
                BTM_EIR_SHORTENED_LOCAL_NAME_TYPE,
                &remote_name_len);
        if (p_rmt_name) {
            if (remote_name_len > BD_NAME_LEN)
                remote_name_len = BD_NAME_LEN;
            if (rmt_name && rmt_name_len) {
                memcpy(rmt_name, p_rmt_name, remote_name_len);
                *(rmt_name + remote_name_len) = 0;
                *rmt_name_len = remote_name_len;
                }
            return true;
        }
    }
    return false;
}

/************************************************************************************
* * Function        bta_scan_recult_callback
* *
* * Description     scan result.it will be called when device scaned a peer device
* *
* * Return          NULL
**************************************************************************************/
static void bta_scan_result_callback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* p_data)
{
    uint8_t len;
    BD_ADDR bd_addr;
    char dev_name[32];
    tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;
    //char obj_name[] = "Find Me";
    char obj_name[] = "SimpleBLEPeripheral";
    uint8_t dev_name_len;

    switch (event)
    {
        case BTA_DM_INQ_RES_EVT:
        {
            LOG_ERROR("scan result: event=%d, "BT_BD_ADDR_STR", device_type=%d\n",
                event, BT_BD_ADDR_HEX(p_data->inq_res.bd_addr), p_data->inq_res.device_type); 
            
            bdcpy(bd_addr, p_data->inq_res.bd_addr);
            if (p_data->inq_res.p_eir)
            {
                if (BTM_CheckEirData(p_data->inq_res.p_eir, BTM_EIR_COMPLETE_LOCAL_NAME_TYPE, &len))
                {
                    p_data->inq_res.remt_name_not_required = TRUE;
                }
            }

            if(check_remote_name(&(p_data->inq_res), dev_name, &dev_name_len))
            {
                LOG_ERROR("scan device name len=%d, name = %s\n", dev_name_len, dev_name);
            }

            if(strcmp(dev_name, obj_name) == 0)
            {
                bdcpy(obj_addr, bd_addr);
                LOG_ERROR("find the device, obj_addr="BT_BD_ADDR_STR"\n", BT_BD_ADDR_HEX(obj_addr));
//                BTA_GATTC_Open(client_if, obj_addr, true, transport);
            }
        }
        break;

        case BTA_DM_INQ_CMPL_EVT:
        {
            LOG_ERROR("%s-BLE observe complete. Num Resp %d\n", __FUNCTION__, p_data->inq_cmpl.num_resps);

            LOG_ERROR("connect the device "BT_BD_ADDR_STR", client_if=%d\n", 
                BT_BD_ADDR_HEX(obj_addr), client_if); 

/* scan complete, start connect*/
            BTA_GATTC_Open(client_if, obj_addr, true, transport); 
        }
        break;

        default:
        LOG_ERROR("%s : unknown event 0x%x", __FUNCTION__, event);
    }
}

/************************************************************************************
* * Function        bta_scan_param_setup_cback
* *
* * Description     set scan param callback.it will be called after setting scan parameter
* *
* * Return          NULL
**************************************************************************************/
static void bta_scan_param_setup_cback(tGATT_IF c_client_if, tBTM_STATUS status)
{
    client_if = c_client_if;
    LOG_ERROR("\nset scan params complete: status=%d, client_if=%d\n", status, client_if);
    /*start scan*/
    BTA_DmBleObserve(true, 8, bta_scan_result_callback);
}

/************************************************************************************
* * Function        bta_gattc_callback
* *
* * Description     app register callback
* *
* * Return          NULL
**************************************************************************************/
static void bta_gattc_callback(tBTA_GATTC_EVT event, tBTA_GATTC* p_data)
{
    switch (event)
    {   
        case BTA_GATTC_REG_EVT:
        {  
            tBTA_GATT_STATUS  status = p_data->reg_oper.status;
            client_if = p_data->reg_oper.client_if;
            LOG_ERROR("%s:register complete: event=%d, status=%d, client_if=%d\n", __FUNCTION__, event, status, client_if);
            UINT8 scan_interval = 0x50;
            UINT8 scan_window = 0x30;
            tBLE_SCAN_MODE scan_mode = BTM_BLE_SCAN_MODE_ACTI;
            
            bac_register();
            /*register complete,set scan parameter*/
            BTA_DmSetBleScanParams(client_if, scan_interval, scan_window, scan_mode,
                bta_scan_param_setup_cback);
  
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
      //      BTA_GATTC_ServiceSearchRequest(conn_id, NULL);

        }
        break;
/*
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

            //get first characteristic of battey service
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
                //read battery level
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
*/
        default:
        LOG_ERROR("%s:unknown event: %d\n", __FUNCTION__, event);
    }

}

/************************************************************************************
* * Function        ble_client_appRegister
* *
* * Description     app register function
* *
* * Return          NULL
**************************************************************************************/
void ble_client_appRegister(void)
{
    
    bt_uuid_t uuid;
    tBT_UUID t_uuid;
    memcpy(&uuid, BASE_UUID, sizeof(bt_uuid_t));
    btif_to_bta_uuid(&t_uuid, &uuid);

    LOG_ERROR("register application\n");
    BTA_GATTC_AppRegister(&t_uuid, bta_gattc_callback);

    /*battery service register*/
//    bac_register();

}

void gattc_client_test(void)
{
    BTM_SetTraceLevel(BT_TRACE_LEVEL_DEBUG);

    ble_client_appRegister();
}
