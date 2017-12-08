// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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



/****************************************************************************
*
* This file is for ble spp client demo.
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "controller.h"
#include "driver/uart.h"

#include "esp_bt.h"
#include "nvs_flash.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_system.h"
#include "btc_main.h"
#include "esp_gatt_common_api.h"

#define GATTC_TAG                   "GATTC_SPP_DEMO"
#ifdef SUPPORT_NEW_GATTC_API
#define PROFILE_NUM                 1
#define PROFILE_APP_ID              0
#define BT_BD_ADDR_STR              "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)        addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]

#define SPP_SRV_INDEX               1
#define SPP_DATA_RECV_CHAR_INDEX    1
#define SPP_DATA_NTFY_CHAR_INDEX    2
#define SPP_CMD_CHAR_INDEX          3
#define SPP_STATUS_CHAR_INDEX       4
#define SPP_HEARTBEAT_CHAR_INDEX    5
#define SPP_NO_DESC                 0
#define SPP_IS_DESC                 1

//#define SUPPORT_HEARTBEAT
//#define DEBUG_MODE

typedef struct spp_ble_gattc_char_descr {
    int char_descr_index;
    uint16_t conn_id;
    esp_gatt_id_t descr_id;
    struct spp_ble_gattc_char_descr *next;
} spp_ble_gattc_char_descr_t;

typedef struct spp_ble_gattc_char {
    int srv_index;
    int index;
    uint16_t conn_id;
    esp_gatt_srvc_id_t srvc_id;
    esp_gatt_id_t char_id;
    esp_gatt_char_prop_t char_prop;
    spp_ble_gattc_char_descr_t * char_descr;
    struct spp_ble_gattc_char *next;
} spp_ble_gattc_char_t;

typedef struct {
    int32_t len;
    spp_ble_gattc_char_t * pfirst;
    spp_ble_gattc_char_t * plast;
} spp_ble_gattc_char_head;

typedef struct at_ble_gattc_srv {
    int index;
    uint16_t conn_id;
    esp_gatt_srvc_id_t srvc_id;
    spp_ble_gattc_char_head * gattc_srv_char_head;
    struct at_ble_gattc_srv *next;
} spp_ble_gattc_srv_t;

typedef struct {
    int32_t len;
    spp_ble_gattc_srv_t * pfirst;
    spp_ble_gattc_srv_t * plast;
} spp_ble_gattc_srv_head;

static spp_ble_gattc_srv_head pAtGattcSrvHead = {
    .len    = 0,
    .pfirst = NULL,
    .plast  = NULL
};

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    esp_bd_addr_t remote_bda;
};

///Declare static functions
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};


static const char device_name[] = "ESP_SPP_SERVER";
static bool is_connect = false;
static uint16_t spp_conn_id = 0;
static uint16_t spp_mtu_size = 23;
static uint16_t cmd = 0;
static uint16_t wr_descr_ccc_num = 0;
static uint16_t spp_gattc_if = 0xff;
static int need_notify_number  = 0;
static int char_descr_index_count = 0;
static char * notify_value_p = NULL;
static int notify_value_offset = 0;
static int notify_value_count = 0;
static spp_ble_gattc_srv_t * temp_srv_p = NULL;
static spp_ble_gattc_char_t * notify_char_pp = NULL;
static spp_ble_gattc_char_t notify_char_p[3] ;
static spp_ble_gattc_char_t * temp_gattc_char_p1 = NULL;
static spp_ble_gattc_char_descr_t * notify_char_descr_p[2] = {NULL,NULL};
static spp_ble_gattc_char_descr_t * temp_char_descr_p = NULL;
static spp_ble_gattc_char_head * pGattcCharHead = NULL;
static esp_ble_gap_cb_param_t scan_rst;
static xQueueHandle cmd_reg_queue = NULL;
static xQueueHandle cmd_read_queue = NULL;
QueueHandle_t spp_uart_queue = NULL;
#ifdef SUPPORT_HEARTBEAT
static uint8_t  heartbeat_s[9] = {'E','s','p','r','e','s','s','i','f'};
static xQueueHandle cmd_heartbeat_queue = NULL;
#endif

static void print_srv_node(void)
{
    temp_srv_p = pAtGattcSrvHead.pfirst;

    if(temp_srv_p == NULL){
        ESP_LOGE(GATTC_TAG, "temp_srv_p == NULL,%s L#%d\n",__func__,__LINE__);
    }
    temp_srv_p = pAtGattcSrvHead.pfirst;
    while(temp_srv_p != NULL){
        ESP_LOGI(GATTC_TAG,"+SRV:No.%d,UUID:0x%04x\n", temp_srv_p->index,temp_srv_p->srvc_id.id.uuid.uuid.uuid16);
        temp_srv_p = temp_srv_p->next;
    }
}

static void get_char_and_descr(void)
{
    pGattcCharHead = (spp_ble_gattc_char_head *)malloc(sizeof(spp_ble_gattc_char_head));
    if(pGattcCharHead == NULL){
        ESP_LOGE(GATTC_TAG, "malloc failed,%s L#%d\n",__func__,__LINE__);
        return;
    }

    pGattcCharHead->len = 0;
    pGattcCharHead->pfirst = NULL;
    pGattcCharHead->plast = NULL;
    pAtGattcSrvHead.pfirst->gattc_srv_char_head = pGattcCharHead;
    temp_srv_p = pAtGattcSrvHead.pfirst;

    esp_ble_gattc_get_characteristic(spp_gattc_if, temp_srv_p->conn_id, &temp_srv_p->srvc_id, NULL);
}

static void Characteristic_Rd(int32_t srv_index,int32_t char_index)
{
    spp_ble_gattc_char_t * pGattc_char_node = NULL;

    temp_srv_p = pAtGattcSrvHead.pfirst;
    if(temp_srv_p == NULL) {
        ESP_LOGE(GATTC_TAG, "temp_srv_p == NULL,%s L#%d\n",__func__,__LINE__);
        return ;
    }
    while(--srv_index) {
        temp_srv_p = temp_srv_p->next;
        if (temp_srv_p == NULL) {
            ESP_LOGE(GATTC_TAG, "temp_srv_p == NULL,%s L#%d\n",__func__,__LINE__);
            return ;
        }
    }
    pGattcCharHead = temp_srv_p->gattc_srv_char_head;
    if(pGattcCharHead == NULL){
        ESP_LOGE(GATTC_TAG, "pGattcCharHead == NULL,%s L#%d\n",__func__,__LINE__);
        return;
    }

    pGattc_char_node = pGattcCharHead->pfirst;
    if(pGattc_char_node == NULL) {
        ESP_LOGE(GATTC_TAG, "pGattc_char_node == NULL,%s L#%d\n",__func__,__LINE__);
        return ;
    }
    while(--char_index){
        pGattc_char_node = pGattc_char_node->next;
        if(pGattc_char_node == NULL){
            ESP_LOGE(GATTC_TAG, "pGattc_char_node == NULL,%s L#%d\n",__func__,__LINE__);
            return ;
        }
    }

    esp_ble_gattc_read_char(spp_gattc_if, pGattc_char_node->conn_id, &pGattc_char_node->srvc_id, &pGattc_char_node->char_id, ESP_GATT_AUTH_REQ_NONE);
}

static void  Characteristic_Wr(int32_t srv_index,int32_t char_index,char * s ,size_t length)
{
    future_t **future_p = NULL;
    int32_t len = 0;
    spp_ble_gattc_char_t * pGattc_char_node;
    uint8_t * spp_gattc_wr_buffer = NULL;

    if(length == 0) {
        return ;
    }
    len = length;

    temp_srv_p = pAtGattcSrvHead.pfirst;
    if(temp_srv_p == NULL) {
        ESP_LOGE(GATTC_TAG, "temp_srv_p == NULL,%s L#%d\n",__func__,__LINE__);
        return ;
    }
    while(--srv_index) {
        temp_srv_p = temp_srv_p->next;
        if (temp_srv_p == NULL) {
            ESP_LOGE(GATTC_TAG, "temp_srv_p == NULL,%s L#%d\n",__func__,__LINE__);
            return ;
        }
    }

    pGattcCharHead = temp_srv_p->gattc_srv_char_head;
    if(pGattcCharHead == NULL){
        ESP_LOGE(GATTC_TAG, "pGattcCharHead == NULL,%s L#%d\n",__func__,__LINE__);
        return ;
    }
    pGattc_char_node = pGattcCharHead->pfirst;
    if (pGattc_char_node == NULL) {
        ESP_LOGE(GATTC_TAG, "pGattc_char_node == NULL,%s L#%d\n",__func__,__LINE__);
        return ;
    }
    while(--char_index){
        pGattc_char_node = pGattc_char_node->next;
        if(pGattc_char_node == NULL){
            ESP_LOGE(GATTC_TAG, "pGattc_char_node == NULL,%s L#%d\n",__func__,__LINE__);
            return ;
        }
    }

    spp_gattc_wr_buffer = (uint8_t *)malloc(sizeof(uint8_t)*(len));
    if(spp_gattc_wr_buffer == NULL){
        ESP_LOGE(GATTC_TAG, "malloc failed,%s L#%d\n",__func__,__LINE__);
        return;
    }
    memcpy(spp_gattc_wr_buffer,s,len);

    if((pGattc_char_node->char_prop & (ESP_GATT_CHAR_PROP_BIT_WRITE_NR|ESP_GATT_CHAR_PROP_BIT_WRITE)) == 0){
        ESP_LOGE(GATTC_TAG,"char_prop do not allow write\n");
        free(spp_gattc_wr_buffer);
        return ;
    }

    future_p = btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE);
    *future_p = future_new();
    if (*future_p == NULL) {
        ESP_LOGE(GATTC_TAG,"%s failed\n", __func__);
        return;
    }
    esp_ble_gattc_write_char(spp_gattc_if, pGattc_char_node->conn_id, &pGattc_char_node->srvc_id, &pGattc_char_node->char_id, len,spp_gattc_wr_buffer, ESP_GATT_WRITE_TYPE_RSP, ESP_GATT_AUTH_REQ_NONE);
    free(spp_gattc_wr_buffer);
    if(future_await(*future_p) == FUTURE_FAIL) {
        ESP_LOGE(GATTC_TAG,"%s failed\n", __func__);
        return;
    }

    return ;
}

static void store_srv_info(esp_ble_gattc_cb_param_t * p_data)
{
    spp_ble_gattc_srv_t * pGattc_srv_node = NULL;

    pGattc_srv_node = (spp_ble_gattc_srv_t *)malloc(sizeof(spp_ble_gattc_srv_t));
    if (pGattc_srv_node == NULL) {
        ESP_LOGE(GATTC_TAG, "malloc failed,%s L#%d\n",__func__,__LINE__);
        return;
    }
    pGattc_srv_node->conn_id = p_data->search_res.conn_id;
    pGattc_srv_node->next = NULL;
    pGattc_srv_node->gattc_srv_char_head = NULL;
    pGattc_srv_node->index = pAtGattcSrvHead.len + 1;
    pGattc_srv_node->srvc_id.is_primary = p_data->search_res.srvc_id.is_primary;
    pGattc_srv_node->srvc_id.id.inst_id = p_data->search_res.srvc_id.id.inst_id;
    pGattc_srv_node->srvc_id.id.uuid.len = p_data->search_res.srvc_id.id.uuid.len;
    pGattc_srv_node->srvc_id.id.uuid.uuid.uuid16 = p_data->search_res.srvc_id.id.uuid.uuid.uuid16;

    if (pAtGattcSrvHead.len == 0) {
        pAtGattcSrvHead.len++;
        pAtGattcSrvHead.pfirst = pGattc_srv_node;
        pAtGattcSrvHead.plast = pGattc_srv_node;
    } else {
        pAtGattcSrvHead.len++;
        pAtGattcSrvHead.plast->next = pGattc_srv_node;
        pAtGattcSrvHead.plast = pGattc_srv_node;
    }
}

static void store_char_info(esp_ble_gattc_cb_param_t * p_data)
{
    spp_ble_gattc_char_t * pGattc_char_node = NULL;

    pGattc_char_node = (spp_ble_gattc_char_t *)malloc(sizeof(spp_ble_gattc_char_t));
    if(pGattc_char_node == NULL){
        ESP_LOGE(GATTC_TAG, "malloc failed,%s L#%d\n",__func__,__LINE__);
        return;
    }
    pGattc_char_node->srv_index = temp_srv_p->index;
    pGattc_char_node->char_descr = NULL;
    pGattc_char_node->conn_id = p_data->get_char.conn_id;
    pGattc_char_node->next = NULL;
    pGattc_char_node->index = pGattcCharHead->len + 1;
    pGattc_char_node->srvc_id.is_primary = p_data->get_char.srvc_id.is_primary;
    pGattc_char_node->srvc_id.id.inst_id = p_data->get_char.srvc_id.id.inst_id;
    pGattc_char_node->srvc_id.id.uuid.len = p_data->get_char.srvc_id.id.uuid.len;
    pGattc_char_node->srvc_id.id.uuid.uuid.uuid16 = p_data->get_char.srvc_id.id.uuid.uuid.uuid16;

    pGattc_char_node->char_id.inst_id = p_data->get_char.char_id.inst_id;
    pGattc_char_node->char_id.uuid.len = p_data->get_char.char_id.uuid.len;
    pGattc_char_node->char_prop = p_data->get_char.char_prop;
    pGattc_char_node->char_id.uuid.uuid.uuid16 = p_data->get_char.char_id.uuid.uuid.uuid16;

    temp_gattc_char_p1 = pGattc_char_node;

    if (pGattcCharHead->len == 0) {
        pGattcCharHead->len++;
        pGattcCharHead->pfirst = pGattc_char_node;
        pGattcCharHead->plast = pGattc_char_node;
    } else {
        pGattcCharHead->len++;
        pGattcCharHead->plast->next = pGattc_char_node;
        pGattcCharHead->plast = pGattc_char_node;
    }
    esp_ble_gattc_get_descriptor(spp_gattc_if, pGattc_char_node->conn_id, &pGattc_char_node->srvc_id, &pGattc_char_node->char_id, NULL);
}

static void store_desc_info(esp_ble_gattc_cb_param_t * p_data)
{
    spp_ble_gattc_char_descr_t * pGattc_char_node_descr = NULL;

    temp_char_descr_p = temp_gattc_char_p1->char_descr;
    pGattc_char_node_descr = (spp_ble_gattc_char_descr_t *)malloc(sizeof(spp_ble_gattc_char_descr_t));
    if(pGattc_char_node_descr == NULL){
        ESP_LOGE(GATTC_TAG, "malloc failed,%s L#%d\n",__func__,__LINE__);
        return;
    }
    pGattc_char_node_descr->conn_id = p_data->get_descr.conn_id;
    pGattc_char_node_descr->char_descr_index = ++char_descr_index_count;
    if (temp_char_descr_p == NULL)
        temp_gattc_char_p1->char_descr = pGattc_char_node_descr;
    else {
        while (temp_char_descr_p->next != NULL) {
            temp_char_descr_p = temp_char_descr_p->next;
        }
        temp_char_descr_p->next = pGattc_char_node_descr;
    }
    pGattc_char_node_descr->next = NULL;
    pGattc_char_node_descr->descr_id.inst_id = p_data->get_descr.descr_id.inst_id;
    pGattc_char_node_descr->descr_id.uuid.len = p_data->get_descr.descr_id.uuid.len;

    pGattc_char_node_descr->descr_id.uuid.uuid.uuid16 = p_data->get_descr.descr_id.uuid.uuid.uuid16;
    if((p_data->get_descr.descr_id.uuid.uuid.uuid16 == 0x2902)&&(need_notify_number<3)){
        memset(&(notify_char_p[need_notify_number]),0x0,sizeof(spp_ble_gattc_char_t));
        memcpy(&(notify_char_p[need_notify_number]),temp_gattc_char_p1,sizeof(spp_ble_gattc_char_t));
        notify_char_descr_p[need_notify_number] = pGattc_char_node_descr;
        ESP_LOGI(GATTC_TAG,"need_notify_number=%d\n",need_notify_number);
        ESP_LOGI(GATTC_TAG,"- srvc_id = 0x%04x, char_id = 0x%04x \n", notify_char_p[need_notify_number].srvc_id.id.uuid.uuid.uuid16, notify_char_p[need_notify_number].char_id.uuid.uuid.uuid16);
        need_notify_number++;
    }
    esp_ble_gattc_get_descriptor(spp_gattc_if, temp_gattc_char_p1->conn_id, &temp_gattc_char_p1->srvc_id, &temp_gattc_char_p1->char_id, &pGattc_char_node_descr->descr_id);
}

static void notify_event_handler(esp_ble_gattc_cb_param_t * p_data)
{
    uint8_t gattc_srv_index_for_port = 0;
    uint8_t gattc_char_index_for_port = 0;

    temp_srv_p = pAtGattcSrvHead.pfirst;
    while(temp_srv_p != NULL){
        if(temp_srv_p->srvc_id.id.uuid.uuid.uuid16 ==  p_data->notify.srvc_id.id.uuid.uuid.uuid16){
            gattc_srv_index_for_port = temp_srv_p->index;
            pGattcCharHead = temp_srv_p->gattc_srv_char_head;
            if(pGattcCharHead != NULL){
                notify_char_pp = pGattcCharHead->pfirst;
                while(notify_char_pp != NULL){
                    if(notify_char_pp->char_id.uuid.uuid.uuid16== p_data->notify.char_id.uuid.uuid.uuid16){
                        gattc_char_index_for_port = notify_char_pp->index;
                    }
                    notify_char_pp = notify_char_pp->next;
                }
            }
        }
        temp_srv_p = temp_srv_p->next;
    }
    if(p_data->notify.is_notify == true){
        ESP_LOGI(GATTC_TAG,"+NOTIFY:%d,%d,%d,",gattc_srv_index_for_port,gattc_char_index_for_port,p_data->notify.value_len);
    }else{
        ESP_LOGI(GATTC_TAG,"+INDICATE:%d,%d,%d,",gattc_srv_index_for_port,gattc_char_index_for_port,p_data->notify.value_len);
    }
    if((gattc_srv_index_for_port == SPP_SRV_INDEX)&&(gattc_char_index_for_port == SPP_DATA_NTFY_CHAR_INDEX)){
#ifdef SPP_DEBUG_MODE
        esp_log_buffer_char(GATTC_TAG, (char *)p_data->notify.value, p_data->notify.value_len);
#else
        if((p_data->notify.value[0] == '#')&&(p_data->notify.value[1] == '#')){
            if((++notify_value_count) != p_data->notify.value[3]){
                if(notify_value_p != NULL){
                    free(notify_value_p);
                }
                notify_value_count = 0;
                notify_value_p = NULL;
                notify_value_offset = 0;
                ESP_LOGE(GATTC_TAG,"notify value count is not continuous,%s\n",__func__);
                return;
            }
            if(p_data->notify.value[3] == 1){
                notify_value_p = (char *)malloc(((spp_mtu_size-7)*(p_data->notify.value[2]))*sizeof(char));
                if(notify_value_p == NULL){
                    ESP_LOGE(GATTC_TAG, "malloc failed,%s L#%d\n",__func__,__LINE__);
                    notify_value_count = 0;
                    return;
                }
                memcpy((notify_value_p + notify_value_offset),(p_data->notify.value + 4),(p_data->notify.value_len - 4));
                if(p_data->notify.value[2] == p_data->notify.value[3]){
                    uart_write_bytes(UART_NUM_0, (char *)(notify_value_p), (p_data->notify.value_len - 4 + notify_value_offset));
                    free(notify_value_p);
                    notify_value_p = NULL;
                    notify_value_offset = 0;
                    return;
                }
                notify_value_offset += (p_data->notify.value_len - 4);
            }else if(p_data->notify.value[3] <= p_data->notify.value[2]){
                memcpy((notify_value_p + notify_value_offset),(p_data->notify.value + 4),(p_data->notify.value_len - 4));
                if(p_data->notify.value[3] == p_data->notify.value[2]){
                    uart_write_bytes(UART_NUM_0, (char *)(notify_value_p), (p_data->notify.value_len - 4 + notify_value_offset));
                    free(notify_value_p);
                    notify_value_count = 0;
                    notify_value_p = NULL;
                    notify_value_offset = 0;
                    return;
                }
                notify_value_offset += (p_data->notify.value_len - 4);
            }
        }else{
            uart_write_bytes(UART_NUM_0, (char *)(p_data->notify.value), p_data->notify.value_len);
        }
#endif
    }else if((gattc_srv_index_for_port == SPP_SRV_INDEX)&&(gattc_char_index_for_port == SPP_STATUS_CHAR_INDEX)){
        esp_log_buffer_char(GATTC_TAG, (char *)p_data->notify.value, p_data->notify.value_len);
        //TODO:server notify status characteristic
    }else{
        esp_log_buffer_char(GATTC_TAG, (char *)p_data->notify.value, p_data->notify.value_len);
    }
}

static void read_event_handler(esp_ble_gattc_cb_param_t * p_data)
{
    uint8_t gattc_srv_index_for_port = 0;
    uint8_t gattc_char_index_for_port = 0;

    temp_srv_p = pAtGattcSrvHead.pfirst;
    while(temp_srv_p != NULL){
        if(temp_srv_p->srvc_id.id.uuid.uuid.uuid16 ==  p_data->notify.srvc_id.id.uuid.uuid.uuid16){
            gattc_srv_index_for_port = temp_srv_p->index;
            pGattcCharHead = temp_srv_p->gattc_srv_char_head;
            if(pGattcCharHead != NULL){
                notify_char_pp = pGattcCharHead->pfirst;
                while(notify_char_pp != NULL){
                    if(notify_char_pp->char_id.uuid.uuid.uuid16== p_data->notify.char_id.uuid.uuid.uuid16){
                        gattc_char_index_for_port = notify_char_pp->index;
                    }
                    notify_char_pp = notify_char_pp->next;
                }
            }
        }
        temp_srv_p = temp_srv_p->next;
    }

    if((gattc_srv_index_for_port == SPP_SRV_INDEX)&&(gattc_char_index_for_port == SPP_STATUS_CHAR_INDEX)){
        //TODO: read status characteristic
    }
}

static void printf_char_and_descr(uint8_t srv_index)
{
    spp_ble_gattc_char_t * temp_gattc_char_p = NULL;
    spp_ble_gattc_char_descr_t * temp_gattc_char_descr_p = NULL;

    temp_srv_p = pAtGattcSrvHead.pfirst;

    if(temp_srv_p == NULL){
        ESP_LOGE(GATTC_TAG, "srv = NULL\n");
    }
    while(--srv_index){
        temp_srv_p = temp_srv_p->next;
        if(temp_srv_p == NULL){
            ESP_LOGE(GATTC_TAG, "srv = NULL\n");
        }
    }

    temp_gattc_char_p = temp_srv_p->gattc_srv_char_head->pfirst;
    while(temp_gattc_char_p != NULL){
        ESP_LOGI(GATTC_TAG,"+CHAR:%d,%d,UUID:0x%04x",temp_srv_p->index, temp_gattc_char_p->index,temp_gattc_char_p->char_id.uuid.uuid.uuid16);

        temp_gattc_char_descr_p = temp_gattc_char_p->char_descr;
        while(temp_gattc_char_descr_p != NULL){
            ESP_LOGI(GATTC_TAG,"+DESC:%d,%d,%d,UUID:0x%04x",temp_srv_p->index, temp_gattc_char_p->index,temp_gattc_char_descr_p->char_descr_index,temp_gattc_char_descr_p->descr_id.uuid.uuid.uuid16);
            temp_gattc_char_descr_p = temp_gattc_char_descr_p->next;
        }
        temp_gattc_char_p = temp_gattc_char_p->next;
    }
    return ;
}

static void free_gattc_srv_db(void)
{
    spp_ble_gattc_srv_t * temp_service_p = NULL , * temp_service_pp = NULL;
    spp_ble_gattc_char_head * temp_srv_char_head_p = NULL;
    spp_ble_gattc_char_t * temp_gattc_char_node_p = NULL , *temp_gattc_char_node_pp =NULL;
    spp_ble_gattc_char_descr_t * temp_gattc_char_descr_node_p = NULL,* temp_gattc_char_descr_node_pp = NULL;

    temp_service_p = pAtGattcSrvHead.pfirst;
    while(temp_service_p != NULL){
        temp_srv_char_head_p = temp_service_p->gattc_srv_char_head;
        temp_gattc_char_node_p = temp_srv_char_head_p->pfirst;
        while(temp_gattc_char_node_p != NULL){
            temp_gattc_char_node_pp = temp_gattc_char_node_p;
            temp_gattc_char_descr_node_p = temp_gattc_char_node_pp->char_descr;
            while(temp_gattc_char_descr_node_p != NULL){
                temp_gattc_char_descr_node_pp = temp_gattc_char_descr_node_p;
                temp_gattc_char_descr_node_p = temp_gattc_char_descr_node_p->next;
                free(temp_gattc_char_descr_node_pp);
            }
            temp_gattc_char_node_p = temp_gattc_char_node_p->next;
            free(temp_gattc_char_node_pp);
        }
        free(temp_srv_char_head_p);

        temp_service_pp = temp_service_p;
        temp_service_p = temp_service_p->next;
        free(temp_service_pp);
    }
    pAtGattcSrvHead.len = 0;
    pAtGattcSrvHead.pfirst = NULL;
    pAtGattcSrvHead.plast = NULL;
    is_connect = false;
    spp_conn_id = 0;
    spp_mtu_size = 23;
    cmd = 0;
    wr_descr_ccc_num = 0;
    spp_gattc_if = 0xff;
    need_notify_number  = 0;
    char_descr_index_count = 0;
    notify_value_p = NULL;
    notify_value_offset = 0;
    notify_value_count = 0;
}

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;

    switch(event){
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        if(param->scan_param_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Scan param set failed");
            break;
        }
        //the unit of the duration is second
        uint32_t duration = 0xFFFF;
        ESP_LOGI(GATTC_TAG, "Enable Ble Scan:during time 0x%04X minutes.",duration);
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "Scan start failed");
            break;
        }
        ESP_LOGI(GATTC_TAG, "Scan start successed");
        break;
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "Scan stop failed");
            break;
        }
        ESP_LOGI(GATTC_TAG, "Scan stop successed");
        if (is_connect == false) {
            ESP_LOGI(GATTC_TAG, "Connect to the remote device.");
            esp_ble_gattc_open(gl_profile_tab[PROFILE_APP_ID].gattc_if, scan_rst.scan_rst.bda, true);
        }
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        switch (scan_result->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            esp_log_buffer_hex(GATTC_TAG, scan_result->scan_rst.bda, 6);
            ESP_LOGI(GATTC_TAG, "Searched Adv Data Len %d, Scan Response Len %d", scan_result->scan_rst.adv_data_len, scan_result->scan_rst.scan_rsp_len);
            adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
            ESP_LOGI(GATTC_TAG, "Searched Device Name Len %d", adv_name_len);
            esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
            ESP_LOGI(GATTC_TAG, "\n");
            if (adv_name != NULL) {
                if ( strncmp((char *)adv_name, device_name, adv_name_len) == 0) {
                    memcpy(&(scan_rst), scan_result, sizeof(esp_ble_gap_cb_param_t));
                    esp_ble_gap_stop_scanning();
                }
            }
            break;
        case ESP_GAP_SEARCH_INQ_CMPL_EVT:
            break;
        default:
            break;
        }
        break;
    }
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "Adv stop failed");
        }else {
            ESP_LOGI(GATTC_TAG, "Stop adv successfully");
        }
        break;
    default:
        break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    ESP_LOGI(GATTC_TAG, "EVT %d, gattc if %d", event, gattc_if);

    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
        } else {
            ESP_LOGI(GATTC_TAG, "Reg app failed, app_id %04x, status %d",param->reg.app_id,param->reg.status);
            return;
        }
    }
    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gattc_if == gl_profile_tab[idx].gattc_if) {
                if (gl_profile_tab[idx].gattc_cb) {
                    gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
                }
            }
        }
    } while (0);
}

static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGE(GATTC_TAG, "REG EVT, set scan params");
        esp_ble_gap_set_scan_params(&ble_scan_params);
        break;
    case ESP_GATTC_CONNECT_EVT:
        spp_gattc_if = gattc_if;
        is_connect = true;
        spp_conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTC_TAG, "ESP_GATTC_CONNECT_EVT: conn_id=%d, gatt_if = %d", spp_conn_id, gattc_if);
        ESP_LOGI(GATTC_TAG, "REMOTE BDA:");
        esp_log_buffer_hex(GATTC_TAG, gl_profile_tab[PROFILE_APP_ID].remote_bda, sizeof(esp_bd_addr_t));
        esp_ble_gattc_search_service(gattc_if, spp_conn_id, NULL);
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(GATTC_TAG, "disconnect");
        free_gattc_srv_db();
        esp_ble_gap_start_scanning(0xffff);
        break;
    case ESP_GATTC_SEARCH_RES_EVT:
        if((p_data->search_res.srvc_id.id.uuid.len == ESP_UUID_LEN_16)&&(p_data->search_res.srvc_id.id.uuid.uuid.uuid16 == 0x1800)){
            ESP_LOGI(GATTC_TAG, "uuid == 0x1800\n");
            break;
        }
        if((p_data->search_res.srvc_id.id.uuid.len == ESP_UUID_LEN_16)&&(p_data->search_res.srvc_id.id.uuid.uuid.uuid16 == 0x1801)){
            ESP_LOGI(GATTC_TAG, "uuid == 0x1801\n");
            break;
        }
        store_srv_info(p_data);
        break;
    case ESP_GATTC_SEARCH_CMPL_EVT:
        ESP_LOGI(GATTC_TAG, "SEARCH_CMPL: conn_id = %x, status %d", spp_conn_id, p_data->search_cmpl.status);
        esp_ble_gattc_send_mtu_req(gattc_if,spp_conn_id);
        break;
    case ESP_GATTC_GET_CHAR_EVT:
        if (p_data->get_char.status != ESP_GATT_OK) {
            printf_char_and_descr(SPP_SRV_INDEX);
            xQueueSend(cmd_reg_queue,&cmd,10/portTICK_PERIOD_MS);
            cmd++;
            break;
        }
        store_char_info(p_data);
        break;
    case ESP_GATTC_GET_DESCR_EVT:
        if (p_data->get_descr.status != ESP_GATT_OK) {
            char_descr_index_count = 0;
            esp_ble_gattc_get_characteristic(spp_gattc_if, temp_srv_p->conn_id, &temp_srv_p->srvc_id, &temp_gattc_char_p1->char_id);
            break;
        }
        store_desc_info(p_data);
        break;
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
        ESP_LOGI(GATTC_TAG,"\nwr_descr_cmd = %d-- srvc_id = 0x%04x, char_id = 0x%04x, descr_id = 0x%04x \n",wr_descr_ccc_num , notify_char_p[wr_descr_ccc_num].srvc_id.id.uuid.uuid.uuid16, notify_char_p[wr_descr_ccc_num].char_id.uuid.uuid.uuid16,notify_char_descr_p[wr_descr_ccc_num]->descr_id.uuid.uuid.uuid16);

        uint16_t notify_en = 1;
        esp_ble_gattc_write_char_descr(
                spp_gattc_if,
                notify_char_p[wr_descr_ccc_num].conn_id,
                &(notify_char_p[wr_descr_ccc_num].srvc_id),
                &(notify_char_p[wr_descr_ccc_num].char_id),
                &notify_char_descr_p[wr_descr_ccc_num]->descr_id,
                sizeof(notify_en),
                (uint8_t *)&notify_en,
                ESP_GATT_WRITE_TYPE_RSP,
                ESP_GATT_AUTH_REQ_NONE);

        wr_descr_ccc_num++;
        break;
    }
    case ESP_GATTC_NOTIFY_EVT:
        ESP_LOGI(GATTC_TAG,"ESP_GATTC_NOTIFY_EVT\n");
        notify_event_handler(p_data);
        break;
    case ESP_GATTC_READ_CHAR_EVT:
        ESP_LOGI(GATTC_TAG,"ESP_GATTC_READ_CHAR_EVT\n");
        read_event_handler(p_data);
        break;
    case ESP_GATTC_WRITE_CHAR_EVT:
        ESP_LOGI(GATTC_TAG,"ESP_GATTC_WRITE_CHAR_EVT:status=%d,srvc_uuid = 0x%04x, char_uuid = 0x%04x, descr_uuid=0x%04x \n",p_data->write.status , p_data->write.srvc_id.id.uuid.uuid.uuid16, p_data->write.char_id.uuid.uuid.uuid16, p_data->write.descr_id.uuid.uuid.uuid16);
        if(param->write.status == ESP_GATT_OK){
            if(param->write.char_id.uuid.uuid.uuid16 == 0xABF1){
                future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_SUCCESS);
            }
        }
        break;
    case ESP_GATTC_PREP_WRITE_EVT:
        break;
    case ESP_GATTC_EXEC_EVT:
        break;
    case ESP_GATTC_WRITE_DESCR_EVT:
        ESP_LOGI(GATTC_TAG,"ESP_GATTC_WRITE_DESCR_EVT: status =%d,srvc_uuid = 0x%04x, char_uuid = 0x%04x, descr_uuid=0x%04x \n",p_data->write.status,p_data->write.srvc_id.id.uuid.uuid.uuid16, p_data->write.char_id.uuid.uuid.uuid16, p_data->write.descr_id.uuid.uuid.uuid16);
        xQueueSend(cmd_reg_queue,&cmd,10/portTICK_PERIOD_MS);
        cmd++;
        break;
    case ESP_GATTC_CFG_MTU_EVT:
        if(p_data->cfg_mtu.status != ESP_OK){
            break;
        }
        ESP_LOGI(GATTC_TAG,"+MTU:%d\n", p_data->cfg_mtu.mtu);
        spp_mtu_size = p_data->cfg_mtu.mtu;
        print_srv_node();
        get_char_and_descr();
        break;
    case ESP_GATTC_SRVC_CHG_EVT:
        break;
    default:
        break;
    }
}

void spp_client_reg_task(void* arg)
{
    uint16_t cmd_id;
    for(;;) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        if(xQueueReceive(cmd_reg_queue, &cmd_id, portMAX_DELAY)) {
            if(0 == cmd_id){
                esp_ble_gattc_register_for_notify(spp_gattc_if, gl_profile_tab[PROFILE_APP_ID].remote_bda ,&(notify_char_p[cmd_id].srvc_id), &(notify_char_p[cmd_id].char_id));
                ESP_LOGI(GATTC_TAG,"No.%d-- srvc_id = 0x%04x, char_id = 0x%04x \n",cmd_id, notify_char_p[cmd_id].srvc_id.id.uuid.uuid.uuid16, notify_char_p[cmd_id].char_id.uuid.uuid.uuid16);
            }else if(1 == cmd_id){
                esp_ble_gattc_register_for_notify(spp_gattc_if, gl_profile_tab[PROFILE_APP_ID].remote_bda ,&(notify_char_p[cmd_id].srvc_id), &(notify_char_p[cmd_id].char_id));
                ESP_LOGI(GATTC_TAG,"No.%d-- srvc_id = 0x%04x, char_id = 0x%04x \n",cmd_id, notify_char_p[cmd_id].srvc_id.id.uuid.uuid.uuid16, notify_char_p[cmd_id].char_id.uuid.uuid.uuid16);
            }else if(2 == cmd_id){
                esp_ble_gattc_register_for_notify(spp_gattc_if, gl_profile_tab[PROFILE_APP_ID].remote_bda ,&(notify_char_p[cmd_id].srvc_id), &(notify_char_p[cmd_id].char_id));
                ESP_LOGI(GATTC_TAG,"No.%d-- srvc_id = 0x%04x, char_id = 0x%04x \n",cmd_id, notify_char_p[cmd_id].srvc_id.id.uuid.uuid.uuid16, notify_char_p[cmd_id].char_id.uuid.uuid.uuid16);
            }
#ifdef SUPPORT_HEARTBEAT
            else if(3 == cmd_id){
                xQueueSend(cmd_heartbeat_queue,&cmd,10/portTICK_PERIOD_MS);
            }
#endif
        }
    }
}

#ifdef SUPPORT_HEARTBEAT
void spp_heart_beat_task(void * arg)
{
    uint16_t cmd_id;

    for(;;) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        if(xQueueReceive(cmd_heartbeat_queue, &cmd_id, portMAX_DELAY)) {
            while(1){
                if(is_connect == true){
                    Characteristic_Wr(SPP_SRV_INDEX,SPP_HEARTBEAT_CHAR_INDEX,(char *)heartbeat_s,sizeof(heartbeat_s));
                    vTaskDelay(5000 / portTICK_PERIOD_MS);
                }else{
                    ESP_LOGI(GATTC_TAG,"disconnect\n");
                    break;
                }
            }
        }
    }
}
#endif

void spp_client_read_task(void * arg)
{
    uint16_t cmd_id;
    int32_t srv_index = SPP_SRV_INDEX, char_index = SPP_STATUS_CHAR_INDEX;

    for(;;) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        if(xQueueReceive(cmd_read_queue, &cmd_id, portMAX_DELAY)) {
            if(is_connect == true){
                Characteristic_Rd(srv_index,char_index);
            }else{
                ESP_LOGI(GATTC_TAG,"disconnect\n");
            }
        }
    }
}

void ble_client_appRegister(void)
{
    esp_err_t status;

    ESP_LOGI(GATTC_TAG, "register callback");

    //register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        ESP_LOGE(GATTC_TAG, "gap register error, error code = %x", status);
        return;
    }
    //register the callback function to the gattc module
    if ((status = esp_ble_gattc_register_callback(esp_gattc_cb)) != ESP_OK) {
        ESP_LOGE(GATTC_TAG, "gattc register error, error code = %x", status);
        return;
    }
    esp_ble_gattc_app_register(PROFILE_APP_ID);

    esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(200);
    if (local_mtu_ret){
        ESP_LOGE(GATTC_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    }

    cmd_reg_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(spp_client_reg_task, "spp_client_reg_task", 2048, NULL, 10, NULL);

#ifdef SUPPORT_HEARTBEAT
    cmd_heartbeat_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(spp_heart_beat_task, "spp_heart_beat_task", 2048, NULL, 10, NULL);
#endif

    cmd_read_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(spp_client_read_task, "spp_client_read_task", 2048, NULL, 10, NULL);
}

void uart_task(void *pvParameters)
{
    uart_event_t event;
    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(spp_uart_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            switch (event.type) {
            //Event of UART receving data
            case UART_DATA:
                if (event.size) {
                    uint8_t * temp = NULL;
                    temp = (uint8_t *)malloc(sizeof(uint8_t)*event.size);
                    if(temp == NULL){
                        ESP_LOGE(GATTC_TAG, "malloc failed,%s L#%d\n",__func__,__LINE__);
                        break;
                    }
                    memset(temp,0x0,event.size);
                    uart_read_bytes(UART_NUM_0,temp,event.size,portMAX_DELAY);
                    Characteristic_Wr(SPP_SRV_INDEX,SPP_DATA_RECV_CHAR_INDEX,(char *)temp,event.size);
                    free(temp);
                }
                break;
            default:
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

static void spp_uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS,
        .rx_flow_ctrl_thresh = 122,
    };

    //Set UART parameters
    uart_param_config(UART_NUM_0, &uart_config);
    //Set UART pins
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    //Install UART driver, and get the queue.
    uart_driver_install(UART_NUM_0, 4096, 8192, 10,&spp_uart_queue,0);
    xTaskCreate(uart_task, "uTask", 2048, (void*)UART_NUM_0, 8, NULL);
}
#endif

void app_main()
{
    esp_err_t ret;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    nvs_flash_init();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable controller failed\n", __func__);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable controller failed\n", __func__);
        return;
    }

    ESP_LOGI(GATTC_TAG, "%s init bluetooth\n", __func__);
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s init bluetooth failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(GATTC_TAG, "%s enable bluetooth failed\n", __func__);
        return;
    }

#ifdef SUPPORT_NEW_GATTC_API
    ble_client_appRegister();
    spp_uart_init();
#endif
}

