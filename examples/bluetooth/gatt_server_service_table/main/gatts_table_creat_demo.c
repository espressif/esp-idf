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


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "bt.h"
#include "bta_api.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_main.h"
#include "gatts_table_creat_demo.h"

#define GATTS_TABLE_TAG "GATTS_TABLE_DEMO"

#define HEART_PROFILE_NUM 			    1
#define HEART_PROFILE_APP_IDX 			0
#define ESP_HEART_RATE_APP_ID			0x55
#define SAMPLE_DEVICE_NAME              "ESP_HEART_RATE"
#define SAMPLE_MANUFACTURER_DATA_LEN    17
#define HEART_RATE_SVC_INST_ID	    	0

#define GATTS_DEMO_CHAR_VAL_LEN_MAX		0x40

uint8_t char1_str[] ={0x11,0x22,0x33};

uint16_t heart_rate_handle_table[HRS_IDX_NB];

esp_attr_value_t gatts_demo_char1_val = 
{
	.attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
	.attr_len		= sizeof(char1_str),
	.attr_value     = char1_str,
};


static uint8_t heart_rate_service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x18, 0x0D, 0x00, 0x00,
};


static esp_ble_adv_data_t heart_rate_adv_config = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x20,
    .max_interval = 0x40,
    .appearance = 0x00,
    .manufacturer_len = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data =  NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(heart_rate_service_uuid),
    .p_service_uuid = heart_rate_service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t heart_rate_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, 
					esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst heart_rate_profile_tab[HEART_PROFILE_NUM] = {
    [HEART_PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    
};

/*
 * HTPT PROFILE ATTRIBUTES
 ****************************************************************************************
 */


/*
 *  Heart Rate PROFILE ATTRIBUTES
 ****************************************************************************************
 */

/// Heart Rate Sensor Service
static const uint16_t heart_rate_svc = ESP_GATT_UUID_HEART_RATE_SVC;

#define CHAR_DECLARATION_SIZE   (sizeof(uint8_t))
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_notify = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_WRITE|ESP_GATT_CHAR_PROP_BIT_READ;

/// Heart Rate Sensor Service - Heart Rate Measurement Characteristic, notify
static const uint16_t heart_rate_meas_uuid = ESP_GATT_HEART_RATE_MEAS;
static const uint8_t heart_measurement_ccc[2] ={ 0x00, 0x00};


/// Heart Rate Sensor Service -Body Sensor Location characteristic, read
static const uint16_t body_sensor_location_uuid = ESP_GATT_BODY_SENSOR_LOCATION;
static const uint8_t body_sensor_loc_val[1] = {0x00};


/// Heart Rate Sensor Service - Heart Rate Control Point characteristic, write&read
static const uint16_t heart_rate_ctrl_point = ESP_GATT_HEART_RATE_CNTL_POINT;
static const uint8_t heart_ctrl_point[1] = {0x00};

/// Full HRS Database Description - Used to add attributes into the database
static const esp_gatts_attr_db_t heart_rate_gatt_db[HRS_IDX_NB] =
{
    // Heart Rate Service Declaration
    [HRS_IDX_SVC]                      	=  
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(heart_rate_svc), (uint8_t *)&heart_rate_svc}},

    // Heart Rate Measurement Characteristic Declaration
    [HRS_IDX_HR_MEAS_CHAR]            = 
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_notify}},
      
    // Heart Rate Measurement Characteristic Value
    [HRS_IDX_HR_MEAS_VAL]             	=   
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&heart_rate_meas_uuid, ESP_GATT_PERM_READ,
      HRPS_HT_MEAS_MAX_LEN,0, NULL}},

    // Heart Rate Measurement Characteristic - Client Characteristic Configuration Descriptor
    [HRS_IDX_HR_MEAS_NTF_CFG]     	=    
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
      sizeof(uint16_t),sizeof(heart_measurement_ccc), (uint8_t *)heart_measurement_ccc}},

    // Body Sensor Location Characteristic Declaration
    [HRS_IDX_BOBY_SENSOR_LOC_CHAR]  = 
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read}},

    // Body Sensor Location Characteristic Value
    [HRS_IDX_BOBY_SENSOR_LOC_VAL]   = 
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&body_sensor_location_uuid, ESP_GATT_PERM_READ,
      sizeof(uint8_t), sizeof(body_sensor_loc_val), (uint8_t *)body_sensor_loc_val}},

    // Heart Rate Control Point Characteristic Declaration
    [HRS_IDX_HR_CTNL_PT_CHAR]          = 
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write}},
                                         			
    // Heart Rate Control Point Characteristic Value
    [HRS_IDX_HR_CTNL_PT_VAL]             = 
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&heart_rate_ctrl_point, ESP_GATT_PERM_WRITE|ESP_GATT_PERM_READ,
      sizeof(uint8_t), sizeof(heart_ctrl_point), (uint8_t *)heart_ctrl_point}},  
};



static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ESP_LOGE(GATTS_TABLE_TAG, "GAP_EVT, event %d\n", event);

    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&heart_rate_adv_params);
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTS_TABLE_TAG, "Advertising start failed\n");
        }
        break;
    default:
        break;
    }
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, 
										   esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) 
{
    ESP_LOGE(GATTS_TABLE_TAG, "event = %x\n",event);
    switch (event) {
    	case ESP_GATTS_REG_EVT:
		ESP_LOGI(GATTS_TABLE_TAG, "%s %d\n", __func__, __LINE__);
        	esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME);
        	ESP_LOGI(GATTS_TABLE_TAG, "%s %d\n", __func__, __LINE__);
       	esp_ble_gap_config_adv_data(&heart_rate_adv_config);

        	ESP_LOGI(GATTS_TABLE_TAG, "%s %d\n", __func__, __LINE__);
		esp_ble_gatts_create_attr_tab(heart_rate_gatt_db, gatts_if, 
								HRS_IDX_NB, HEART_RATE_SVC_INST_ID);
       	break;
    	case ESP_GATTS_READ_EVT:
       
       	 break;
    	case ESP_GATTS_WRITE_EVT: 
      	 	break;
    	case ESP_GATTS_EXEC_WRITE_EVT:
		break;
    	case ESP_GATTS_MTU_EVT:
		break;
   	 case ESP_GATTS_CONF_EVT:
		break;
    	case ESP_GATTS_UNREG_EVT:
        	break;
    	case ESP_GATTS_DELETE_EVT:
        	break;
    	case ESP_GATTS_START_EVT:
        	break;
    	case ESP_GATTS_STOP_EVT:
        	break;
    	case ESP_GATTS_CONNECT_EVT:
        	break;
    	case ESP_GATTS_DISCONNECT_EVT:
		break;
    	case ESP_GATTS_OPEN_EVT:
		break;
    	case ESP_GATTS_CANCEL_OPEN_EVT:
		break;
    	case ESP_GATTS_CLOSE_EVT:
		break;
    	case ESP_GATTS_LISTEN_EVT:
		break;
    	case ESP_GATTS_CONGEST_EVT:
		break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
        ESP_LOGI(GATTS_TABLE_TAG, "The number handle =%x\n",param->add_attr_tab.num_handle);
        if (param->add_attr_tab.status != ESP_GATT_OK){
            ESP_LOGE(GATTS_TABLE_TAG, "Create attribute table failed, error code=0x%x", param->add_attr_tab.status);
        }
        else if (param->add_attr_tab.num_handle != HRS_IDX_NB){
            ESP_LOGE(GATTS_TABLE_TAG, "Create attribute table abnormally, num_handle (%d) \
                    doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, HRS_IDX_NB);
        }
        else {
            memcpy(heart_rate_handle_table, param->add_attr_tab.handles, sizeof(heart_rate_handle_table));
            esp_ble_gatts_start_service(heart_rate_handle_table[HRS_IDX_SVC]);
        }
        break;
                                      }

    default:
        break;
    }
}


static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, 
									esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(GATTS_TABLE_TAG, "EVT %d, gatts if %d\n", event, gatts_if);

    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            heart_rate_profile_tab[HEART_PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGI(GATTS_TABLE_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id, 
                    param->reg.status);
            return;
        }
    }
	
    do {
        int idx;
        for (idx = 0; idx < HEART_PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == heart_rate_profile_tab[idx].gatts_if) {
                if (heart_rate_profile_tab[idx].gatts_cb) {
                    heart_rate_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

void app_main()
{
    esp_err_t ret;

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed\n", __func__);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed\n", __func__);
        return;
    }

    ESP_LOGI(GATTS_TABLE_TAG, "%s init bluetooth\n", __func__);
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s init bluetooth failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTS_TABLE_TAG, "%s enable bluetooth failed\n", __func__);
        return;
    }

    esp_ble_gatts_register_callback(gatts_event_handler);
    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gatts_app_register(ESP_HEART_RATE_APP_ID);
    return;
}
