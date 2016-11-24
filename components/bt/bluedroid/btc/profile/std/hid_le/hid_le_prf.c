#include <string.h>
// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "hid_le_prf.h"
#include "prf_defs.h"

#if (HIDD_LE_PROFILE_CFG)
hidd_le_env_t hidd_le_env;

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)


// HID Information characteristic value
static const uint8_t hidInfo[HID_INFORMATION_LEN] = {
    LO_UINT16(0x0111), HI_UINT16(0x0111),             // bcdHID (USB HID version)
    0x00,                                             // bCountryCode
    HID_KBD_FLAGS                                     // Flags
};

// HID Report Map characteristic value
// Keyboard report descriptor (using format for Boot interface descriptor)
static const uint8_t hidReportMap[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x02,  // Usage (Mouse)
    0xA1, 0x01,  // Collection (Application)
    0x85, 0x01,  // Report Id (1)
    0x09, 0x01,  //   Usage (Pointer)
    0xA1, 0x00,  //   Collection (Physical)
    0x05, 0x09,  //     Usage Page (Buttons)
    0x19, 0x01,  //     Usage Minimum (01) - Button 1
    0x29, 0x03,  //     Usage Maximum (03) - Button 3
    0x15, 0x00,  //     Logical Minimum (0)
    0x25, 0x01,  //     Logical Maximum (1)
    0x75, 0x01,  //     Report Size (1)
    0x95, 0x03,  //     Report Count (3)
    0x81, 0x02,  //     Input (Data, Variable, Absolute) - Button states
    0x75, 0x05,  //     Report Size (5)
    0x95, 0x01,  //     Report Count (1)
    0x81, 0x01,  //     Input (Constant) - Padding or Reserved bits
    0x05, 0x01,  //     Usage Page (Generic Desktop)
    0x09, 0x30,  //     Usage (X)
    0x09, 0x31,  //     Usage (Y)
    0x09, 0x38,  //     Usage (Wheel)
    0x15, 0x81,  //     Logical Minimum (-127)
    0x25, 0x7F,  //     Logical Maximum (127)
    0x75, 0x08,  //     Report Size (8)
    0x95, 0x03,  //     Report Count (3)
    0x81, 0x06,  //     Input (Data, Variable, Relative) - X & Y coordinate
    0xC0,        //   End Collection
    0xC0,        // End Collection

    0x05, 0x01,  // Usage Pg (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection: (Application)
    0x85, 0x02,  // Report Id (2)
    //
    0x05, 0x07,  //   Usage Pg (Key Codes)
    0x19, 0xE0,  //   Usage Min (224)
    0x29, 0xE7,  //   Usage Max (231)
    0x15, 0x00,  //   Log Min (0)
    0x25, 0x01,  //   Log Max (1)
    //
    //   Modifier byte
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x08,  //   Report Count (8)
    0x81, 0x02,  //   Input: (Data, Variable, Absolute)
    //
    //   Reserved byte
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x08,  //   Report Size (8)
    0x81, 0x01,  //   Input: (Constant)
    //
    //   LED report
    0x95, 0x05,  //   Report Count (5)
    0x75, 0x01,  //   Report Size (1)
    0x05, 0x08,  //   Usage Pg (LEDs)
    0x19, 0x01,  //   Usage Min (1)
    0x29, 0x05,  //   Usage Max (5)
    0x91, 0x02,  //   Output: (Data, Variable, Absolute)
    //
    //   LED report padding
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x03,  //   Report Size (3)
    0x91, 0x01,  //   Output: (Constant)
    //
    //   Key arrays (6 bytes)
    0x95, 0x06,  //   Report Count (6)
    0x75, 0x08,  //   Report Size (8)
    0x15, 0x00,  //   Log Min (0)
    0x25, 0x65,  //   Log Max (101)
    0x05, 0x07,  //   Usage Pg (Key Codes)
    0x19, 0x00,  //   Usage Min (0)
    0x29, 0x65,  //   Usage Max (101)
    0x81, 0x00,  //   Input: (Data, Array)
    //
    0xC0,        // End Collection
    //
    0x05, 0x0C,   // Usage Pg (Consumer Devices)
    0x09, 0x01,   // Usage (Consumer Control)
    0xA1, 0x01,   // Collection (Application)
    0x85, 0x03,   // Report Id (3)
    0x09, 0x02,   //   Usage (Numeric Key Pad)
    0xA1, 0x02,   //   Collection (Logical)
    0x05, 0x09,   //     Usage Pg (Button)
    0x19, 0x01,   //     Usage Min (Button 1)
    0x29, 0x0A,   //     Usage Max (Button 10)
    0x15, 0x01,   //     Logical Min (1)
    0x25, 0x0A,   //     Logical Max (10)
    0x75, 0x04,   //     Report Size (4)
    0x95, 0x01,   //     Report Count (1)
    0x81, 0x00,   //     Input (Data, Ary, Abs)
    0xC0,         //   End Collection
    0x05, 0x0C,   //   Usage Pg (Consumer Devices)
    0x09, 0x86,   //   Usage (Channel)
    0x15, 0xFF,   //   Logical Min (-1)
    0x25, 0x01,   //   Logical Max (1)
    0x75, 0x02,   //   Report Size (2)
    0x95, 0x01,   //   Report Count (1)
    0x81, 0x46,   //   Input (Data, Var, Rel, Null)
    0x09, 0xE9,   //   Usage (Volume Up)
    0x09, 0xEA,   //   Usage (Volume Down)
    0x15, 0x00,   //   Logical Min (0)
    0x75, 0x01,   //   Report Size (1)
    0x95, 0x02,   //   Report Count (2)
    0x81, 0x02,   //   Input (Data, Var, Abs)
    0x09, 0xE2,   //   Usage (Mute)
    0x09, 0x30,   //   Usage (Power)
    0x09, 0x83,   //   Usage (Recall Last)
    0x09, 0x81,   //   Usage (Assign Selection)
    0x09, 0xB0,   //   Usage (Play)
    0x09, 0xB1,   //   Usage (Pause)
    0x09, 0xB2,   //   Usage (Record)
    0x09, 0xB3,   //   Usage (Fast Forward)
    0x09, 0xB4,   //   Usage (Rewind)
    0x09, 0xB5,   //   Usage (Scan Next)
    0x09, 0xB6,   //   Usage (Scan Prev)
    0x09, 0xB7,   //   Usage (Stop)
    0x15, 0x01,   //   Logical Min (1)
    0x25, 0x0C,   //   Logical Max (12)
    0x75, 0x04,   //   Report Size (4)
    0x95, 0x01,   //   Report Count (1)
    0x81, 0x00,   //   Input (Data, Ary, Abs)
    0x09, 0x80,   //   Usage (Selection)
    0xA1, 0x02,   //   Collection (Logical)
    0x05, 0x09,   //     Usage Pg (Button)
    0x19, 0x01,   //     Usage Min (Button 1)
    0x29, 0x03,   //     Usage Max (Button 3)
    0x15, 0x01,   //     Logical Min (1)
    0x25, 0x03,   //     Logical Max (3)
    0x75, 0x02,   //     Report Size (2)
    0x81, 0x00,   //     Input (Data, Ary, Abs)
    0xC0,         //   End Collection
    0x81, 0x03,   //   Input (Const, Var, Abs)
    0xC0         // End Collection
};

// HID report map length
uint8_t hidReportMapLen = sizeof(hidReportMap);

uint8_t hidProtocolMode = HID_PROTOCOL_MODE_REPORT;

// HID report mapping table
static hidRptMap_t  hidRptMap[HID_NUM_REPORTS];


esp_bt_uuid_t char_info_uuid = {LEN_UUID_16, {CHAR_HID_INFO_UUID}};
esp_bt_uuid_t char_ctnl_pt_uuid = {LEN_UUID_16, {CHAR_HID_CTNL_PT_UUID}};
esp_bt_uuid_t char_report_map_uuid = {LEN_UUID_16, {CHAR_REPORT_MAP_UUID}};
esp_bt_uuid_t char_report_uuid = {LEN_UUID_16, {CHAR_REPORT_UUID}};
esp_bt_uuid_t char_proto_mode_uuid = {LEN_UUID_16, {CHAR_PROTOCOL_MODE_UUID}};
esp_bt_uuid_t char_kb_in_report_uuid = {LEN_UUID_16, {CHAR_BOOT_KB_IN_REPORT_UUID}};
esp_bt_uuid_t char_kb_out_report_uuid = {LEN_UUID_16, {CHAR_BOOT_KB_OUT_REPORT_UUID}};
esp_bt_uuid_t char_mouse_in_report_uuid = {LEN_UUID_16, {CHAR_BOOT_MOUSE_IN_REPORT_UUID}};



/// Full HID device Database Description - Used to add attributes into the database
const  char_desc_t hids_char_db[HIDD_LE_CHAR_MAX] = {
    // HID Information Characteristic Value
    [HIDD_LE_INFO_CHAR]                          = {
        &char_info_uuid,
        GATT_PERM_READ,
        GATT_CHAR_PROP_BIT_READ
    },

    // HID Control Point Characteristic Value
    [HIDD_LE_CTNL_PT_CHAR]                 = {
        &char_ctnl_pt_uuid,
        GATT_PERM_WRITE,
        GATT_CHAR_PROP_BIT_WRITE_NR
    },

    // Report Map Characteristic Value
    [HIDD_LE_REPORT_MAP_CHAR]               = {
        &char_report_map_uuid,
        GATT_PERM_READ,
        GATT_CHAR_PROP_BIT_READ
    },
    // Report Characteristic Value
    [HIDD_LE_REPORT_CHAR]                   = {
        &char_report_uuid,
        (GATT_PERM_READ | GATT_PERM_WRITE),
        (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE | GATT_CHAR_PROP_BIT_WRITE_NR)
    },
    // Protocol Mode Characteristic Declaration
    [HIDD_LE_PROTO_MODE_CHAR]                 = {
        &char_proto_mode_uuid,
        GATT_PERM_READ,
        GATT_CHAR_PROP_BIT_READ,
    },

    // Boot Keyboard Input Report Characteristic Value
    [HIDD_LE_BOOT_KB_IN_REPORT_CHAR]           = {
        &char_kb_in_report_uuid,
        GATT_PERM_READ,
        (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY),

    },

    // Boot Keyboard Output Report Characteristic Value
    [HIDD_LE_BOOT_KB_OUT_REPORT_CHAR]          = {
        &char_kb_out_report_uuid,
        GATT_PERM_READ,
        (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE | GATT_CHAR_PROP_BIT_WRITE_NR)
    },

    // Boot Mouse Input Report Characteristic Value
    [HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR]        = {
        &char_mouse_in_report_uuid,
        GATT_PERM_READ,
        (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY),
    },
};

static void hidd_add_characterisitc(const char_desc_t *char_desc);

/*****************************************************************************
**  Constants
*****************************************************************************/
static void hidd_le_profile_cb(esp_gatts_evt_t event,  esp_gatts_t *p_data);

/*******************************************************************************
**
** Function         hidd_add_characterisitc
**
** Description      the callback function after the hid device profile has been register to the BTA manager module
**
** Returns          NULL
**
*******************************************************************************/
static void hidd_add_characterisitc(const char_desc_t *char_desc)
{
    uint16_t service_id;
    if (char_desc == NULL) {
        LOG_ERROR("Invalid hid characteristic\n");
        return;
    }
    //check the hid device serivce has been register to the data base or not
    if (!hidd_le_env.enabled) {
        LOG_ERROR("The hid device didn't register yet\n");
        return;
    }
    //get the service id from the env whitch has been register
    service_id = hidd_le_env.hidd_clcb.cur_srvc_id;
    if (char_desc->char_uuid != 0x00) {
        // start added the charact to the data base
        esp_ble_gatts_add_char (service_id,
                                char_desc->char_uuid,
                                char_desc->perm,
                                char_desc->prop);
    }
}


/*******************************************************************************
**
** Function         hidd_le_profile_cb
**
** Description      the callback function after the hid device profile has been register to the BTA manager module
**
** Returns          NULL
**
*******************************************************************************/
static void hidd_le_profile_cb(esp_gatts_evt_t event, esp_gatts_t *p_data)
{
    esp_gatts_rsp_t rsp;
    esp_bt_uuid_t uuid = {LEN_UUID_16, {ATT_SVC_HID}};
    static uint8_t hid_char_idx;
    hidd_clcb_t *p_clcb = NULL;
    uint8_t app_id = 0xff;

    switch (event) {
    case ESP_GATTS_REG_EVT:
        //check the register of the hid device profile has been succeess or not
        if (p_data->reg_oper.status != ESP_GATT_OK) {
            LOG_ERROR("hidd profile register failed\n");
        }
        hidd_le_env.hidd_inst.app_id = app_id;
        //save the gatt interface in the hid device ENV
        hidd_le_env.gatt_if = p_data->reg_oper.server_if;
        //set the env flag to enable
        hidd_le_env.enabled = true;
        //create the hid device service to the service data base.
        if (p_data->reg_oper.uuid.uu.uuid16 == ATT_SVC_HID) {
            hidd_le_create_service(true);
        }
        break;
    case ESP_GATTS_CREATE_EVT:
        if (p_data->create.uuid.uu.uuid16 == ATT_SVC_HID) {
            ///store the service id to the env
            hidd_le_env.hidd_clcb.cur_srvc_id = p_data->create.service_id;
            //start the button service after created
            esp_ble_gatts_start_srvc(p_data->create.service_id);
            hid_char_idx = HIDD_LE_INFO_CHAR;
            //added the info character to the data base.
            hidd_add_characterisitc(&hids_char_db[hid_char_idx]);
            hid_char_idx++;
        }

        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:

        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        //save the charateristic handle to the env
        hidd_le_env.hidd_inst.att_tbl[hid_char_idx - 1] = p_data->add_result.attr_id;
        LOG_ERROR("hanlder = %x, p_data->add_result.char_uuid.uu.uuid16 = %x\n", p_data->add_result.attr_id,
                  p_data->add_result.char_uuid.uu.uuid16);
        LOG_ERROR("hid_char_idx=%x\n", hid_char_idx);
        if (hid_char_idx <= HIDD_LE_CHAR_MAX) { //added the characteristic until the index overflow

            if ((p_data->add_result.char_uuid.uu.uuid16 == CHAR_BOOT_KB_IN_REPORT_UUID) ||
                    (p_data->add_result.char_uuid.uu.uuid16 == CHAR_BOOT_MOUSE_IN_REPORT_UUID)) {
                // add the gattc config descriptor to the notify charateristic
                //tBTA_GATT_PERM perm = (GATT_PERM_WRITE|GATT_PERM_WRITE);
                uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
                LOG_ERROR("p_data->add_result.char_uuid.uu.uuid16 = %x\n",
                          p_data->add_result.char_uuid.uu.uuid16);
                esp_ble_gatts_add_char_descr (hidd_le_env.hidd_clcb.cur_srvc_id,
                                              GATT_PERM_WRITE,
                                              &uuid);


                break;
            }
            hidd_add_characterisitc(&hids_char_db[hid_char_idx]);
        }
        hid_char_idx++;
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        if (p_data->add_result.char_uuid.uu.uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG) {
            uuid.uu.uuid16 = GATT_UUID_RPT_REF_DESCR;
            BTA_GATTS_AddCharDescriptor (hidd_le_env.hidd_clcb.cur_srvc_id,
                                         GATT_PERM_READ,
                                         &uuid);
            LOG_ERROR("p_data->add_result.char_uuid.uu.uuid16 = %x\n",
                      p_data->add_result.char_uuid.uu.uuid16);
        }
        if (p_data->add_result.char_uuid.uu.uuid16 == GATT_UUID_RPT_REF_DESCR) {
            if (hid_char_idx < HIDD_LE_CHAR_MAX) {
                hidd_add_characterisitc(&hids_char_db[hid_char_idx]);
                hid_char_idx++;
            }
        }
        break;
    case ESP_GATTS_READ_EVT: {
        LOG_ERROR("Hidd profile  BTA_GATTS_READ_EVT\n");
        UINT32 trans_id = p_data->req_data.trans_id;
        UINT16 conn_id = p_data->req_data.conn_id;
        UINT16 handle = p_data->req_data.p_data->read_req.handle;
        bool   is_long = p_data->req_data.p_data->read_req.is_long;
        LOG_ERROR("read request:event=0x%x,handle=0x%x,trans_id=0x%x,conn_id=0x%x\n",
                  event, handle, trans_id, conn_id);

        hidd_read_attr_value(p_data->req_data.p_data, trans_id);
    }
    break;
    case ESP_GATTS_WRITE_EVT:
        esp_ble_gatts_send_rsp (p_data->req_data.conn_id, p_data->req_data.trans_id,
                                p_data->req_data.status, NULL);
        break;
    case ESP_GATTS_CONNECT_EVT:
        p_clcb = &hidd_le_env.hidd_clcb;

        if (!p_clcb->in_use) {
            p_clcb->in_use = TRUE;
            p_clcb->conn_id = p_data->conn.conn_id;;
            LOG_ERROR("hidd->conn_id = %x\n", p_data->conn.conn_id);
            p_clcb->connected = TRUE;
            memcpy(p_clcb->remote_bda, p_data->conn.remote_bda, BD_ADDR_LEN);
        }
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        p_clcb = &hidd_le_env.hidd_clcb;
        //set the connection flag to true
        p_clcb->connected = false;
        p_clcb->in_use = TRUE;
        memset(p_clcb->remote_bda, 0, BD_ADDR_LEN);
        break;
    case ESP_GATTS_START_EVT:
        break;
    case ESP_GATTS_CONGEST_EVT:
        if (hidd_le_env.hidd_clcb.connected && (hidd_le_env.hidd_clcb.conn_id == p_data->conn.conn_id)) {
            //set the connection channal congested flag to true
            hidd_le_env.hidd_clcb.congest =  p_data->congest.congested;
        }
        break;
    default:
        break;
    }
}

/*******************************************************************************
**
** Function         hidd_le_create_service
**
** Description      Create a Service for the hid device profile
**
** Parameters       is_primary: this service is the primary service or not,true is the primary service
**                              false is not the primary service
**                  p_service_uuid: service UUID.
**
** Returns          NULL
**
*******************************************************************************/
void hidd_le_create_service(BOOLEAN is_primary)
{
    esp_gatts_if_t server_if ;
    esp_bt_uuid_t  uuid = {LEN_UUID_16, {ATT_SVC_HID}};
    //the number of the hid device attributes in the hid service.
    UINT16 num_handle = HIDD_LE_IDX_NB;
    UINT8 inst = 0x00;
    server_if = hidd_le_env.gatt_if;
    hidd_le_env.inst_id = inst;
    //start create the hid device service
    esp_ble_gatts_create_srvc (server_if, &uuid, inst, num_handle, is_primary);
}


/*****************************************************************************
** Function      hidd_read_attr_value
**
** Description   it will be called when client sends a read request
******************************************************************************/
void hidd_read_attr_value(tGATTS_DATA *p_data, uint32_t trans_id)
{
    hidd_inst_t     *p_inst = &hidd_le_env.hidd_inst;
    uint8_t      i;
    uint8_t      status = ESP_GATT_OK;
    uint8_t      app_id = hidd_le_env.hidd_inst.app_id;

    esp_gatt_status_t st = ESP_GATT_NOT_FOUND;
    uint16_t     handle = p_data->read_req.handle;
    uint16_t     conn_id = hidd_le_env.hidd_clcb.conn_id;

    if (handle == p_inst->att_tbl[HIDD_LE_INFO_CHAR]) {
        //read hid device info evt
        p_inst->pending_evt = HIDD_LE_READ_INFO_EVT;

    } else if (handle == p_inst->att_tbl[HIDD_LE_CTNL_PT_CHAR]) {
        //read hid device contol point evt
        p_inst->pending_evt = HIDD_LE_READ_CTNL_PT_EVT;
    } else if (handle == p_inst->att_tbl[HIDD_LE_REPORT_MAP_CHAR]) {
        //read hid device report map value evt
        p_inst->pending_evt = HIDD_LE_READ_REPORT_MAP_EVT;
    } else if (handle == p_inst->att_tbl[HIDD_LE_REPORT_CHAR]) {
        //read hid device report evt
        p_inst->pending_evt = HIDD_LE_READ_REPORT_EVT;
    } else if (handle == p_inst->att_tbl[HIDD_LE_PROTO_MODE_CHAR]) {
        //read hid device mode evt
        p_inst->pending_evt = HIDD_LE_READ_PROTO_MODE_EVT;
    } else if (handle == p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR]) {
        //read hid boot keyboard in report evt
        p_inst->pending_evt = HIDD_LE_BOOT_KB_IN_REPORT_EVT;
    } else if (handle == p_inst->att_tbl[HIDD_LE_BOOT_KB_OUT_REPORT_CHAR]) {
        //read hid boot keyboard out report evt
        p_inst->pending_evt = HIDD_LE_BOOT_KB_OUT_REPORT_EVT;
    } else if (handle == p_inst->att_tbl[HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR]) {
        //read hid device boot mouse in report evt
        p_inst->pending_evt = HIDD_LE_BOOT_MOUSE_IN_REPORT_EVT;
    }

    //start build the rsp message
    hidd_rsp(trans_id, conn_id, app_id, status, p_inst->pending_evt, p_data);
}

/*******************************************************************************
**
** Function         hidd_rsp
**
** Description      Respond to a hid device service request
**
*******************************************************************************/
void hidd_rsp (uint32_t trans_id, uint16_t conn_id, uint8_t app_id,
               esp_gatt_status_t status, uint8_t event, tGATTS_DATA *p_rsp)
{
    hidd_inst_t *p_inst = &hidd_le_env.hidd_inst;
    tGATTS_RSP  rsp;
    uint8_t   *pp;
    LOG_ERROR("conn_id = %x, trans_id = %x, event = %x\n",
              conn_id, trans_id, event);

    if (p_inst->app_id == app_id) {
        return ;
    }

    memset(&rsp, 0, sizeof(tGATTS_RSP));

    if (p_inst->pending_evt == event) {
        switch (event) {
        case HIDD_LE_READ_INFO_EVT:
            LOG_ERROR(" p_inst->att_tbl[HIDD_LE_INFO_CHAR] = %x\n",
                      p_inst->att_tbl[HIDD_LE_INFO_CHAR]);
            rsp.attr_value.handle = p_inst->att_tbl[HIDD_LE_INFO_CHAR];
            rsp.attr_value.len = HID_INFORMATION_LEN;
            //copy the infomation value to the att value to sent to the peer device
            memcpy(rsp.attr_value.value, hidInfo, HID_INFORMATION_LEN);
            //start send the rsp to the peer device
            esp_ble_gatts_send_rsp(conn_id, trans_id, status, &rsp);
            break;

        case HIDD_LE_READ_CTNL_PT_EVT:
            LOG_ERROR(" p_inst->att_tbl[HIDD_LE_CTNL_PT_CHAR] = %x\n",
                      p_inst->att_tbl[HIDD_LE_CTNL_PT_CHAR]);
            rsp.attr_value.handle = p_inst->att_tbl[HIDD_LE_CTNL_PT_CHAR];
            rsp.attr_value.len = 0;
            //start send the rsp to the peer device
            esp_ble_gatts_send_rsp(conn_id, trans_id, status, &rsp);
            break;

        case HIDD_LE_READ_REPORT_MAP_EVT:
            LOG_ERROR("p_inst->att_tbl[HIDD_LE_REPORT_MAP_CHAR] = %x\n",
                      p_inst->att_tbl[HIDD_LE_REPORT_MAP_CHAR]);
            rsp.attr_value.handle = p_inst->att_tbl[HIDD_LE_REPORT_MAP_CHAR];
            rsp.attr_value.len = hidReportMapLen;
            //copy the infomation value to the att value to sent to the peer device
            memcpy(rsp.attr_value.value, hidReportMap, hidReportMapLen);
            //start send the rsp to the peer device
            esp_ble_gatts_send_rsp(conn_id, trans_id, status, &rsp);
            break;
        case HIDD_LE_READ_REPORT_EVT:
            LOG_ERROR("p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR] = %x\n",
                      p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR]);
            rsp.attr_value.handle = p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR];
            rsp.attr_value.len = 0;

            esp_ble_gatts_send_rsp(conn_id, trans_id, status, &rsp);
            break;
        case HIDD_LE_READ_PROTO_MODE_EVT:
            LOG_ERROR("p_inst->att_tbl[HIDD_LE_PROTO_MODE_CHAR] = %x\n",
                      p_inst->att_tbl[HIDD_LE_PROTO_MODE_CHAR]);
            rsp.attr_value.handle = p_inst->att_tbl[HIDD_LE_PROTO_MODE_CHAR];
            rsp.attr_value.len = 1;
            pp = rsp.attr_value.value;
            //copy the infomation value to the att value to sent to the peer device
            memcpy(rsp.attr_value.value, &hidProtocolMode, rsp.attr_value.len);
            esp_ble_gatts_send_rsp(conn_id, trans_id, status, &rsp);
            break;
        case HIDD_LE_BOOT_KB_IN_REPORT_EVT:
            LOG_ERROR("p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR] = %x\n",
                      p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR]);
            rsp.attr_value.handle = p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR];
            rsp.attr_value.len = 0;

            esp_ble_gatts_send_rsp(conn_id, trans_id, status, &rsp);
            break;
        case HIDD_LE_BOOT_KB_OUT_REPORT_EVT:
            LOG_ERROR("p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR] = %x\n",
                      p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR]);
            rsp.attr_value.handle = p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR];
            rsp.attr_value.len = 0;

            esp_ble_gatts_send_rsp(conn_id, trans_id, status, &rsp);
            break;
        case HIDD_LE_BOOT_MOUSE_IN_REPORT_EVT:
            LOG_ERROR("p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR] = %x\n",
                      p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR]);
            rsp.attr_value.handle = p_inst->att_tbl[HIDD_LE_BOOT_KB_IN_REPORT_CHAR];
            rsp.attr_value.len = 0;

            esp_ble_gatts_send_rsp(conn_id, trans_id, status, &rsp);
            break;
        default:
            break;
        }
        // p_inst->pending_clcb_idx = 0;
        p_inst->pending_evt = 0;
        p_inst->pending_hal = 0;
    }
    return;
}


/*******************************************************************************
**
** Function         hidd_le_init
**
** Description      Initializa the GATT Service for button profiles.
** Returns          NULL
*******************************************************************************/
esp_gatt_status_t hidd_le_init (void)
{
    tBT_UUID app_uuid = {LEN_UUID_16, {ATT_SVC_HID}};

    if (hidd_le_env.enabled) {
        LOG_ERROR("hid device svc already initaliezd\n");
        return ESP_GATT_ERROR;
    } else {
        memset(&hidd_le_env, 0, sizeof(hidd_le_env_t));
    }


    /*
    register the hid deivce profile to the BTA_GATTS module*/
    esp_ble_gatts_app_register(&app_uuid, hidd_le_profile_cb);

    hidd_le_env.enabled = TRUE;

    return ESP_GATT_OK;
}


#endif  ///HIDD_LE_PROFILE_CFG


