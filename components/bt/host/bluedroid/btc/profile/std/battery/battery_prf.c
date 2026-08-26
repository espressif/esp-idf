/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

//#include "bluedroid_test.h"
#include "bta/bta_api.h"
#include "bta/bta_gatt_api.h"
#include "device/controller.h"

#include "gatt_int.h"
#include "common/bt_trace.h"
#include "stack/btm_api.h"
#include "stack/bt_types.h"
#include "dis_api.h"

#if BLE_INCLUDED == true

#define BA_MAX_CHAR_NUM     1
#define BA_MAX_ATTR_NUM     (BA_MAX_CHAR_NUM * 5 + 1)
/*max 3 descriptors, 1 desclaration and 1 value*/

#ifndef BATTER_LEVEL_PROP
#define BATTER_LEVEL_PROP   (GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY)
#endif

#ifndef BATTER_LEVEL_PERM
#define BATTER_LEVEL_PERM   (GATT_PERM_READ)
#endif

#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]
esp_gatts_if_t server_if;

tBATTERY_CB         battery_cb;
tGATT_CHAR_PROP     prop = GATT_CHAR_PROP_BIT_READ;
tBA_REG_INFO        ba_reg_info;
UINT8               attr_handle_bit = 0x00;

extern tDIS_CB      dis_cb;
esp_bt_uuid_t       bas_uuid = {LEN_UUID_16, {UUID_SERVCLASS_BATTERY}};
/******************************************************************************
** Function      bas_gatts_callback
**
** Description   battery service register callback function
*******************************************************************************/
static void bas_gatts_callback(esp_gatts_evt_t event, tBTA_GATTS *p_data)
{
    switch (event) {
    case ESP_GATTS_REG_EVT: {
        esp_gatt_status_t  status = p_data->reg_oper.status;
        server_if = p_data->reg_oper.server_if;
        BTC_TRACE_ERROR("BAS register completed: event=%d, status=%d, server_if=%d\n",
                  event, status, server_if);

        UINT8 app_id = 0xff;
        bas_init(server_if, app_id);

        tDIS_ATTR_MASK mask = 0x01ff;
        DIS_Init(server_if, mask);
    }
    break;

    /*connect callback*/
    case ESP_GATTS_CONNECT_EVT: {
        BTC_TRACE_ERROR("\ndevice is connected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n",
                  BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                  p_data->conn.reason, p_data->conn.conn_id);
        /*return whether the remote device is currently connected*/
        int is_connected = BTA_DmGetConnectionState(p_data->conn.remote_bda);
        BTC_TRACE_ERROR("is_connected=%d\n", is_connected);
    }
    break;

    /*create service callback*/
    case ESP_GATTS_CREATE_EVT: {
        BTC_TRACE_ERROR("create service:server_if=%d,service_id=0x%x,service_uuid=0x%x\n",
                  p_data->create.server_if, p_data->create.service_id,
                  p_data->create.uuid.uu.uuid16);
        UINT16 service_uuid = p_data->create.uuid.uu.uuid16;
        UINT16 service_id = p_data->create.service_id;
        if (service_uuid == 0x180f) {
            tBT_UUID   uuid = {LEN_UUID_16, {GATT_UUID_BATTERY_LEVEL}};
            bas_AddChar(service_id, &uuid);
        }
        if (service_uuid == 0x180a) {
            dis_cb.service_handle = service_id;
            dis_cb.max_handle = service_id + DIS_MAX_ATTR_NUM;
            dis_AddChar(service_id);
        }

    }
    break;

    case ESP_GATTS_ADD_CHAR_EVT: {
        BTC_TRACE_ERROR("create characteristic:server_if=%d,service_id=0x%x,char_uuid=0x%x\n",
                  p_data->add_result.server_if, p_data->add_result.service_id,
                  p_data->add_result.char_uuid.uu.uuid16);
        UINT16 char_uuid = p_data->add_result.char_uuid.uu.uuid16;
        UINT16 service_id = p_data->add_result.service_id;
        UINT16 uuid_len = p_data->add_result.char_uuid.len;

        if (uuid_len == ESP_UUID_LEN_16) {
            if (char_uuid == GATT_UUID_BATTERY_LEVEL) {
                bas_AddCharDescr(service_id, p_data->add_result.attr_id);
            }

            switch (char_uuid) {
            case GATT_UUID_SYSTEM_ID:
                dis_cb.dis_attr[0].handle = service_id; break;
            case GATT_UUID_MODEL_NUMBER_STR:
                dis_cb.dis_attr[1].handle = service_id; break;
            case GATT_UUID_SERIAL_NUMBER_STR:
                dis_cb.dis_attr[2].handle = service_id; break;
            case GATT_UUID_FW_VERSION_STR:
                dis_cb.dis_attr[3].handle = service_id; break;
            case GATT_UUID_HW_VERSION_STR:
                dis_cb.dis_attr[4].handle = service_id; break;
            case GATT_UUID_SW_VERSION_STR:
                dis_cb.dis_attr[5].handle = service_id; break;
            case GATT_UUID_MANU_NAME:
                dis_cb.dis_attr[6].handle = service_id; break;
            case GATT_UUID_IEEE_DATA:
                dis_cb.dis_attr[7].handle = service_id; break;
            case GATT_UUID_PNP_ID:
                dis_cb.dis_attr[8].handle = service_id; break;
            }
            default:
                break;
        }
    }
    break;

    case ESP_GATTS_ADD_CHAR_DESCR_EVT: {

        BTC_TRACE_ERROR("create descriptor:server_if=%d,service_id=0x%x,attr_id=0x%x,char_uuid=0x%x\n",
                  p_data->add_result.server_if, p_data->add_result.service_id,
                  p_data->add_result.attr_id, p_data->add_result.char_uuid.uu.uuid16);
        bas_AddCharDescr(p_data->add_result.service_id, p_data->add_result.attr_id);
    }
    break;

    case ESP_GATTS_START_EVT: {
        BTC_TRACE_ERROR("start service:server_if=%d,service_id=0x%x\n", p_data->srvc_oper.server_if,
                  p_data->srvc_oper.service_id);
        bas_service_cmpl(p_data->srvc_oper.service_id, p_data->srvc_oper.status);

        /*start advertising*/
        //if(p_data->srvc_oper.status == GATT_SUCCESS)
        // BTA_GATTS_Listen(server_if, true, NULL);
        //    BTA_GATTC_Broadcast(client_if, true);       //non-connectable
    }
    break;

    case ESP_GATTS_READ_EVT: {
        UINT32 trans_id = p_data->req_data.trans_id;
        UINT16 conn_id = p_data->req_data.conn_id;
        UINT16 handle = p_data->req_data.p_data->read_req.handle;
        bool   is_long = p_data->req_data.p_data->read_req.is_long;
        BTC_TRACE_ERROR("read request:event=0x%x,handle=0x%x,trans_id=0x%x,conn_id=0x%x\n",
                  event, handle, trans_id, conn_id);

        if (dis_valid_handle_range(handle)) {
            tGATT_VALUE p_value;
            p_value.handle = handle;
            p_value.conn_id = conn_id;
            p_value.offset = p_data->req_data.p_data->read_req.offset;
            dis_s_read_attr_value(p_data->req_data.p_data, &p_value, trans_id, conn_id);
        } else {
            bas_s_read_attr_value(p_data->req_data.p_data, trans_id, conn_id);
        }
    }
    break;

    case ESP_GATTS_WRITE_EVT: {

        UINT32 trans_id = p_data->req_data.trans_id;
        UINT16 conn_id = p_data->req_data.conn_id;
        UINT16 handle = p_data->req_data.p_data->write_req.handle;
        BTC_TRACE_ERROR("write request:event=0x%x,handle=0x%x,trans_id=0x%x,conn_id=0x%x\n",
                  event, handle, trans_id, conn_id);
        bas_s_write_attr_value(p_data->req_data.p_data, trans_id, conn_id,
                               p_data->req_data.remote_bda);
    }
    break;

    case ESP_GATTS_EXEC_WRITE_EVT: {
        UINT32 trans_id = p_data->req_data.trans_id;
        UINT16 conn_id = p_data->req_data.conn_id;
        UINT8  exec_write = p_data->req_data.p_data->exec_write;
        BTC_TRACE_ERROR("execute write request:event=0x%x,exce_write=0x%x,trans_id=0x%x,conn_id=0x%x\n",
                  event, exec_write, trans_id, conn_id);
    }
    break;

    case ESP_GATTS_MTU_EVT: {
        UINT32 trans_id = p_data->req_data.trans_id;
        UINT16 conn_id = p_data->req_data.conn_id;
        UINT16 mtu = p_data->req_data.p_data->mtu;
        BTC_TRACE_ERROR("exchange mtu request:event=0x%x,mtu=0x%x,trans_id=0x%x,conn_id=0x%x\n",
                  event, mtu, trans_id, conn_id);
    }
    break;

    case ESP_GATTS_CFM_EVT: {

        UINT32 trans_id = p_data->req_data.trans_id;
        UINT16 conn_id = p_data->req_data.conn_id;
        BTC_TRACE_ERROR("configue request:trans_id=0x%x,conn_id=0x%x\n",
                  trans_id, conn_id);
    }
    break;

    default:
        BTC_TRACE_ERROR("unsettled event: %d\n", event);
        break;
    }

}
/******************************************************************************
** Function      bas_callback
**
** Description   battery service callback for client request
*******************************************************************************/
static void bas_callback(UINT32 trans_id, UINT16 conn_id, UINT8 app_id,
                         UINT8 event, tBA_WRITE_DATA *p_data)
{
    tBA_RSP_DATA p_rsp;
    tGATT_STATUS st = ESP_GATT_OK;
    switch (event) {
    case BA_READ_LEVEL_REQ : {
        BTC_TRACE_ERROR("read battery level\n");
        p_rsp.ba_level = 60;     //battery level
        Battery_Rsp(trans_id, conn_id, app_id, st, event, &p_rsp);
    }
    break;

    case BA_READ_PRE_FMT_REQ : {
        BTC_TRACE_ERROR("read presentation format\n");
    }
    break;

    case BA_READ_CLT_CFG_REQ : {
        BTC_TRACE_ERROR("read client characteristic configuration request\n");
        p_rsp.clt_cfg = 0x0001;   //notification
        Battery_Rsp(trans_id, conn_id, app_id, st, event, &p_rsp);
    }
    break;

    case BA_READ_RPT_REF_REQ : {
        BTC_TRACE_ERROR("read report reference descriptor\n");
    }
    break;

    /*battery level notify*/
    case BA_WRITE_CLT_CFG_REQ : {
        BTC_TRACE_ERROR("write client characteristic configuration request\n");
        Battery_Rsp(trans_id, conn_id, app_id, st, event, NULL);

        int battery_level = 50;
        Battery_Notify(conn_id, app_id, p_data->remote_bda, battery_level);
    }
    break;

    default:
        break;
    }

    return;
}
/*****************************************************************************
** Function      bas_s_read_attr_value
**
** Description   it will be called when client sends a read request
******************************************************************************/
void bas_s_read_attr_value(tGATTS_DATA *p_data, UINT32 trans_id, UINT16 conn_id)
{

    tBA_INST            *p_inst = &battery_cb.battery_inst[0];
    UINT8               i;
    esp_gatt_status_t   st = ESP_GATT_NOT_FOUND;
    UINT16              handle = p_data->read_req.handle;


    for (i = 0; i < BA_MAX_INT_NUM; i ++, p_inst ++) {
        // read battery level
        if (handle == p_inst->ba_level_hdl ||
                handle == p_inst->clt_cfg_hdl ||
                handle == p_inst->rpt_ref_hdl ||
                handle == p_inst->pres_fmt_hdl) {
            if (p_data->read_req.is_long) {
                st = ESP_GATT_NOT_LONG;
            }

            if (p_inst->p_cback) {
                if (handle == p_inst->ba_level_hdl) {
                    p_inst->pending_evt = BA_READ_LEVEL_REQ;
                }
                if (handle == p_inst->clt_cfg_hdl) {
                    p_inst->pending_evt = BA_READ_CLT_CFG_REQ;
                }
                if (handle == p_inst->pres_fmt_hdl) {
                    p_inst->pending_evt = BA_READ_PRE_FMT_REQ;
                }
                if (handle == p_inst->rpt_ref_hdl) {
                    p_inst->pending_evt = BA_READ_RPT_REF_REQ ;
                }

                // p_inst->pending_clcb_idx = clcb_idx;
                p_inst->pending_handle = handle;
                //act = SRVC_ACT_PENDING;
                (*p_inst->p_cback)(trans_id, conn_id, p_inst->app_id, p_inst->pending_evt, NULL);
            } else { /* application is not registered */
                st = ESP_GATT_ERR_UNLIKELY;
            }
            break;
        }
        /* else attribute not found */
    }
}

/*****************************************************************************
** Function      bas_s_write_attr_value
**
** Description   it will be called when client sends a write request
******************************************************************************/
void bas_s_write_attr_value(tGATTS_DATA *p_data, UINT32 trans_id, UINT16 conn_id, BD_ADDR bd_addr)
{
    tBA_WRITE_DATA   cfg;
    UINT8            *p = p_data->write_req.value;
    tBA_INST         *p_inst = &battery_cb.battery_inst[0];
    UINT8             i;
    esp_gatt_status_t st = ESP_GATT_NOT_FOUND;
    UINT16            handle = p_data->write_req.handle;


    for (i = 0; i < BA_MAX_INT_NUM; i ++, p_inst ++) {
        if (handle == p_inst->clt_cfg_hdl) {
            memcpy(cfg.remote_bda, bd_addr, BD_ADDR_LEN);
            STREAM_TO_UINT16(cfg.clt_cfg, p);

            if (p_inst->p_cback) {
                p_inst->pending_evt = BA_WRITE_CLT_CFG_REQ;
                p_inst->pending_handle = handle;
                cfg.need_rsp = p_data->write_req.need_rsp;
                (*p_inst->p_cback)(trans_id, conn_id, p_inst->app_id, p_inst->pending_evt, &cfg);
            } else { /* all other handle is not writable */
                st = ESP_GATT_WRITE_NOT_PERMIT;
            }
            break;
        }

    }
}
/***************************************************************
**
** Function     bas_register
**
** Description  register app for battery service
**
****************************************************************/
void bas_register(void)
{
    esp_ble_gatts_app_register(&bas_uuid, bas_gatts_callback);

}
/***************************************************************
**
** Function     bas_init
**
** Description  register battery service
**
****************************************************************/
void bas_init(tBTA_GATTS_IF gatt_if, UINT16 app_id)
{

    tBA_INST    *p_inst;

    ba_reg_info.is_pri = true;
    ba_reg_info.ba_level_descr = BA_LEVEL_NOTIFY;
    ba_reg_info.transport = GATT_TRANSPORT_LE;
    ba_reg_info.p_cback = bas_callback;
    if (battery_cb.inst_id == BA_MAX_INT_NUM) {
        GATT_TRACE_ERROR("MAX battery service has been reached\n");
        return;
    }

    p_inst = &battery_cb.battery_inst[battery_cb.inst_id];

    BTC_TRACE_ERROR("create battery service\n");
    BTC_TRACE_ERROR("inst_id=%d\n", battery_cb.inst_id);
    esp_ble_gatts_create_srvc (gatt_if, &bas_uuid, battery_cb.inst_id ,
                               BA_MAX_ATTR_NUM, ba_reg_info.is_pri);

    battery_cb.inst_id ++;

    p_inst->app_id  =   app_id;
    p_inst->p_cback =   ba_reg_info.p_cback;

}

/***************************************************************
**
** Function     bas_AddChar
**
** Description  add characteristic for battery service
**
****************************************************************/
void bas_AddChar(UINT16 service_id, tBT_UUID *char_uuid)
{
    if (ba_reg_info.ba_level_descr & BA_LEVEL_NOTIFY) {
        prop |= GATT_CHAR_PROP_BIT_NOTIFY;
    }
    attr_handle_bit = 0x01;
    esp_ble_gatts_add_char(service_id, char_uuid, BATTER_LEVEL_PERM, prop);

}

/***************************************************************
**
** Function     bas_AddCharDescr
**
** Description  add descriptor for battery service if needed
**
****************************************************************/
void bas_AddCharDescr(UINT16 service_id, UINT16 attr_id)
{
    tBT_UUID uuid;
    uuid.len = LEN_UUID_16;

    battery_cb.inst_id --;
    tBA_INST *p_inst = &battery_cb.battery_inst[battery_cb.inst_id++];
    /*store the attribute handles*/
    if (attr_handle_bit == 0x01) {
        p_inst->ba_level_hdl = attr_id;
    } else if (attr_handle_bit == 0x02) {
        p_inst->clt_cfg_hdl = attr_id;
    } else if (attr_handle_bit == 0x04) {
        p_inst->pres_fmt_hdl = attr_id;
    } else if (attr_handle_bit == 0x08) {
        p_inst->rpt_ref_hdl = attr_id;
    }


    if (ba_reg_info.ba_level_descr != 0) {
        if (ba_reg_info.ba_level_descr & BA_LEVEL_NOTIFY) {
            uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            ba_reg_info.ba_level_descr &= 0xfe;
            attr_handle_bit = 0x02;
            esp_ble_gatts_add_char_descr(service_id, (GATT_PERM_READ | GATT_PERM_WRITE), &uuid);
            return;
        }

        /* need presentation format descriptor? */
        if (ba_reg_info.ba_level_descr & BA_LEVEL_PRE_FMT) {
            uuid.uu.uuid16 = GATT_UUID_CHAR_PRESENT_FORMAT;
            esp_ble_gatts_add_char_descr(service_id, GATT_PERM_READ, &uuid);
            ba_reg_info.ba_level_descr &= 0xfd;
            attr_handle_bit = 0x04;
            return;
        }
        /* need report reference format descriptor? */
        if (ba_reg_info.ba_level_descr & BA_LEVEL_RPT_REF) {
            uuid.uu.uuid16 = GATT_UUID_RPT_REF_DESCR;
            ba_reg_info.ba_level_descr &= 0xfb;
            esp_ble_gatts_add_char_descr(service_id, GATT_PERM_READ, &uuid);
            attr_handle_bit = 0x08;
            return;
        }
    }

    else {
        esp_ble_gatts_start_srvc(service_id);
    }

}

/***************************************************************
**
** Function     bas_service_cmpl
**
** Description  create battery service complete
**
****************************************************************/
void bas_service_cmpl(UINT16 service_id, esp_gatt_status_t status)
{
    if (status != ESP_GATT_OK) {
        battery_cb.inst_id --;
        esp_ble_gatts_dele_srvc(service_id);
    }

}
/*******************************************************************************
**
** Function         Battery_Rsp
**
** Description      Respond to a battery service request
**
*******************************************************************************/
void Battery_Rsp (UINT32 trans_id, UINT16 conn_id, UINT8 app_id,
                  esp_gatt_status_t st, UINT8 event, tBA_RSP_DATA *p_rsp)
{
    tBA_INST *p_inst = &battery_cb.battery_inst[0];
    tGATTS_RSP  rsp;
    UINT8   *pp;

    UINT8   i = 0;
    while (i < BA_MAX_INT_NUM) {
        if (p_inst->app_id == app_id && p_inst->ba_level_hdl != 0) {
            break;
        }
        i ++;
    }

    if (i == BA_MAX_INT_NUM) {
        return;
    }

    memset(&rsp, 0, sizeof(tGATTS_RSP));

    if (p_inst->pending_evt == event) {
        switch (event) {
        case BA_READ_CLT_CFG_REQ:
            rsp.attr_value.handle = p_inst->pending_handle;
            rsp.attr_value.len = 2;
            pp = rsp.attr_value.value;
            UINT16_TO_STREAM(pp, p_rsp->clt_cfg);
            esp_ble_gatts_send_rsp(conn_id, trans_id, st, &rsp);
            //srvc_sr_rsp(p_inst->pending_clcb_idx, st, &rsp);
            break;

        case BA_READ_LEVEL_REQ:
            rsp.attr_value.handle = p_inst->pending_handle;
            rsp.attr_value.len = 1;
            pp = rsp.attr_value.value;
            UINT8_TO_STREAM(pp, p_rsp->ba_level);
            esp_ble_gatts_send_rsp(conn_id, trans_id, st, &rsp);
            //srvc_sr_rsp(p_inst->pending_clcb_idx, st, &rsp);
            break;

        case BA_WRITE_CLT_CFG_REQ:
            esp_ble_gatts_send_rsp(conn_id, trans_id, st, NULL);
            //srvc_sr_rsp(p_inst->pending_clcb_idx, st, NULL);
            break;

        case BA_READ_RPT_REF_REQ:
            rsp.attr_value.handle = p_inst->pending_handle;
            rsp.attr_value.len = 2;
            pp = rsp.attr_value.value;
            UINT8_TO_STREAM(pp, p_rsp->rpt_ref.rpt_id);
            UINT8_TO_STREAM(pp, p_rsp->rpt_ref.rpt_type);
            esp_ble_gatts_send_rsp(conn_id, trans_id, st, &rsp);
            //srvc_sr_rsp(p_inst->pending_clcb_idx, st, &rsp);
            break;

        default:
            break;
        }
        // p_inst->pending_clcb_idx = 0;
        p_inst->pending_evt = 0;
        p_inst->pending_handle = 0;
    }
    return;
}
/*******************************************************************************
**
** Function         Battery_Notify
**
** Description      Send battery level notification
**
*******************************************************************************/
void Battery_Notify (UINT16 conn_id, UINT8 app_id, BD_ADDR remote_bda, UINT8 battery_level)
{
    tBA_INST *p_inst = &battery_cb.battery_inst[0];
    UINT8    i = 0;

    while (i < BA_MAX_INT_NUM) {
        if (p_inst->app_id == app_id && p_inst->ba_level_hdl != 0) {
            break;
        }
        i ++;
    }

    if (i == BA_MAX_INT_NUM || p_inst->clt_cfg_hdl == 0) {
        return;
    }
    esp_ble_gatts_hdl_val_indica(conn_id, p_inst->ba_level_hdl, 1, &battery_level, false);
    //srvc_sr_notify(remote_bda, p_inst->ba_level_hdl, 1, &battery_level);

}
#endif
