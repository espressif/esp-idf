/* main.c - Application main entry point */

/*
 * Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//1. PB-ADV
//2. scan interver 40ms, window 20ms
//7. provisioned 5 devices ,and node all reset. (table)
//8. test count = 100 
//10. client timeout 1s

//3. PSRAM enable configuration, Bluedroid to PSRAM
//4. BLE adv report flow control supported ??
//5. ADV priorty  ??
//6. Max provision time = 30s,from link open  
//9. need wifi connect


#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "freertos/event_groups.h"

#include "ble_mesh_demo_init.h"
#include "esp_timer.h"
#include "board.h"
#include "esp_bt.h"

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "esp_coexist.h"

#define SD_STORE 0
#define TEST_COUNT_MAX 100
char file_path[] = "/sdcard/hello.txt";
// This example can use SDMMC and SPI peripherals to communicate with SD card.
// By default, SDMMC peripheral is used.
// To enable SPI mode, uncomment the following line:

#define USE_SPI_MODE

// When testing SD and SPI modes, keep in mind that once the card has been
// initialized in SPI mode, it can not be reinitialized in SD mode without
// toggling power to the card.

#ifdef USE_SPI_MODE
// Pin mapping when using SPI mode.
// With this mapping, SD card can be used both in SPI and 1-line SD mode.
// Note that a pull-up on CS line is required in SD mode.
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13
#endif //USE_SPI_MODE
FILE* f_test;

#define EXAMPLE_WIFI_SSID  "asus_ble_mesh_mi"
// #define EXAMPLE_WIFI_SSID  "asus_ble_mesh"
// #define EXAMPLE_WIFI_SSID  "tp_ble_mesh"

#define EXAMPLE_WIFI_PASS  "1qazxsw2"

#define CID_ESP             0x02E5
#define CID_NVAL            0xFFFF

#define PROV_OWN_ADDR       0x0001

#define MSG_SEND_TTL        3
#define MSG_SEND_REL        false
#define MSG_TIMEOUT         0
#define MSG_ROLE            ROLE_PROVISIONER

#define COMP_DATA_PAGE_0    0x00

#define APP_KEY_IDX         0x0000
#define APP_KEY_OCTET       0x12

static uint8_t dev_uuid[16];

typedef struct {
    uint8_t  uuid[16];
    uint16_t unicast;
    uint8_t  elem_num;
    uint8_t  onoff;
} esp_ble_mesh_node_info_t;

#define PROV_TIME_OUT    30000*1000   //30s
#define PROV_NEXT_DEV    3000*1000    //3s
#define PROV_MAX_TIMEOUT 120000*1000  //120s
#define PROV_MAX_COUNT 5
#define UUID_LEN       16
uint8_t match_dev_uuid[][16] = {
    {0xdd,0xdd,0x30,0xae,0xa4,0x10,0x29,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xdd,0xdd,0x30,0xae,0xa4,0x0f,0xa8,0x5e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // {0xdd,0xdd,0x30,0xae,0xa4,0x17,0xab,0xc2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xdd,0xdd,0x30,0xae,0xa4,0x1e,0x55,0xde,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xdd,0xdd,0x30,0xae,0xa4,0x17,0xa5,0x4e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xdd,0xdd,0x30,0xae,0xa4,0xd9,0xba,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

struct prov_resut_t{
    uint32_t t_start;
    uint32_t t_end;
    uint32_t one_time;
    uint64_t average;
}prov_resut = {
    .t_start = 0x0,
    .t_end   = 0x0,
    .average = 0x0,
    .one_time= 0x0,
};

struct prov_statistic_t{
    uint32_t all_count;
    uint32_t success_count;
    uint32_t failed_count;

    uint16_t prov_uuid[PROV_MAX_COUNT];
    uint8_t  prov_count;
    int8_t  last_count;
    esp_timer_handle_t prov_timer;
} prov_statistic = {
    .prov_count    = 0x0,
    .all_count     = 0x0,
    .success_count = 0x0,
    .failed_count  = 0x0,
    .last_count    = -1,
};

static void update_result(struct prov_statistic_t *result, bool all_count, bool success_count, bool failed_count)
{
    if(all_count == true){
        prov_resut.t_start = esp_timer_get_time() / 1000; //ms
        result->all_count += 1;
        return;
    }

    if(failed_count == true){
        result->failed_count += 1;
        return;
    }

    if(success_count == true){
        prov_resut.t_end = esp_timer_get_time() / 1000;
        prov_resut.one_time = prov_resut.t_end - prov_resut.t_start;
        prov_resut.average += prov_resut.one_time;
        result->success_count += 1;
        return;
    }
    ESP_LOGW(TAG, "update result not valid");
}


static esp_ble_mesh_node_info_t nodes[CONFIG_BLE_MESH_MAX_PROV_NODES] = {
    [0 ... (CONFIG_BLE_MESH_MAX_PROV_NODES - 1)] = {
        .unicast = ESP_BLE_MESH_ADDR_UNASSIGNED,
        .elem_num = 0,
        .onoff = LED_OFF,
    }
};

static struct esp_ble_mesh_key {
    uint16_t net_idx;
    uint16_t app_idx;
    uint8_t  app_key[16];
} prov_key;

static esp_ble_mesh_client_t config_client;
static esp_ble_mesh_client_t onoff_client;

static esp_ble_mesh_cfg_srv_t config_server = {
    .relay = ESP_BLE_MESH_RELAY_DISABLED,
    .beacon = ESP_BLE_MESH_BEACON_ENABLED,
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
    /* 3 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(4, 10),
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
};

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(NULL, &onoff_client),
};

static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, ESP_BLE_MESH_MODEL_NONE),
};

static esp_ble_mesh_comp_t composition = {
    .cid = CID_ESP,
    .elements = elements,
    .element_count = ARRAY_SIZE(elements),
};
uint8_t oob[]={0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0x10,0x11,0x12,0x13,0x14,0x15,0x16};
static esp_ble_mesh_prov_t provision = {
    .prov_uuid           = dev_uuid,
    .prov_unicast_addr   = PROV_OWN_ADDR,
    .prov_start_address  = 0x0005,
    .prov_attention      = 0x00,
    .prov_algorithm      = 0x00,
    .prov_pub_key_oob    = 0x00,
    .prov_static_oob_val =oob,
    .prov_static_oob_len = 16,
    .flags               = 0x00,
    .iv_index            = 0x00,
};

static esp_err_t example_ble_mesh_store_node_info(const uint8_t uuid[16], uint16_t unicast,
                                                  uint8_t elem_num, uint8_t onoff_state)
{
    int i;

    if (!uuid || !ESP_BLE_MESH_ADDR_IS_UNICAST(unicast)) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Judge if the device has been provisioned before */
    for (i = 0; i < ARRAY_SIZE(nodes); i++) {
        if (!memcmp(nodes[i].uuid, uuid, 16)) {
            // ESP_LOGW(TAG, "%s: reprovisioned device 0x%04x", __func__, unicast);
            nodes[i].unicast = unicast;
            nodes[i].elem_num = elem_num;
            nodes[i].onoff = onoff_state;
            return ESP_OK;
        }
    }

    for (i = 0; i < ARRAY_SIZE(nodes); i++) {
        if (nodes[i].unicast == ESP_BLE_MESH_ADDR_UNASSIGNED) {
            memcpy(nodes[i].uuid, uuid, 16);
            nodes[i].unicast = unicast;
            nodes[i].elem_num = elem_num;
            nodes[i].onoff = onoff_state;
            return ESP_OK;
        }
    }

    return ESP_FAIL;
}

static esp_ble_mesh_node_info_t *example_ble_mesh_get_node_info(uint16_t unicast)
{
    int i;

    if (!ESP_BLE_MESH_ADDR_IS_UNICAST(unicast)) {
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(nodes); i++) {
        if (nodes[i].unicast <= unicast &&
                nodes[i].unicast + nodes[i].elem_num > unicast) {
            return &nodes[i];
        }
    }

    return NULL;
}

static esp_err_t example_ble_mesh_set_msg_common(esp_ble_mesh_client_common_param_t *common,
                                                 esp_ble_mesh_node_info_t *node,
                                                 esp_ble_mesh_model_t *model, uint32_t opcode)
{
    if (!common || !node || !model) {
        return ESP_ERR_INVALID_ARG;
    }

    common->opcode = opcode;
    common->model = model;
    common->ctx.net_idx = prov_key.net_idx;
    common->ctx.app_idx = prov_key.app_idx;
    common->ctx.addr = node->unicast;
    common->ctx.send_ttl = MSG_SEND_TTL;
    common->ctx.send_rel = MSG_SEND_REL;
    common->msg_timeout = MSG_TIMEOUT;
    common->msg_role = MSG_ROLE;

    return ESP_OK;
}

static esp_err_t prov_complete(int node_idx, const esp_ble_mesh_octet16_t uuid,
                               uint16_t unicast, uint8_t elem_num, uint16_t net_idx)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_cfg_client_get_state_t get_state = {0};
    esp_ble_mesh_node_info_t *node = NULL;
    char name[10];
    int err;
    prov_statistic.prov_uuid[prov_statistic.prov_count] = unicast;
    prov_statistic.last_count = prov_statistic.prov_count;
    ESP_LOGI(TAG, "node index: 0x%x, unicast address: 0x%02x, element num: %d, netkey index: 0x%02x",
             node_idx, unicast, elem_num, net_idx);
    ESP_LOGI(TAG, "device uuid: %s", bt_hex(uuid, 16));
#if SD_STORE
    fprintf(f_test, "device uuid: %s", bt_hex(uuid, 16));
#endif
    sprintf(name, "%s%d", "NODE-", node_idx);
    err = esp_ble_mesh_provisioner_set_node_name(node_idx, name);
    if (err) {
        ESP_LOGE(TAG, "%s: Set node name failed", __func__);
        return ESP_FAIL;
    }

    err = example_ble_mesh_store_node_info(uuid, unicast, elem_num, LED_OFF);
    if (err) {
        ESP_LOGE(TAG, "%s: Store node info failed", __func__);
        return ESP_FAIL;
    }

    node = example_ble_mesh_get_node_info(unicast);
    if (!node) {
        ESP_LOGE(TAG, "%s: Get node info failed", __func__);
        return ESP_FAIL;
    }

    example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET);
    get_state.comp_data_get.page = COMP_DATA_PAGE_0;
    err = esp_ble_mesh_config_client_get_state(&common, &get_state);
    if (err) {
        ESP_LOGE(TAG, "%s: Send config comp data get failed", __func__);
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void prov_link_open(esp_ble_mesh_prov_bearer_t bearer)
{
    esp_err_t ret;
    ESP_LOGI(TAG, "%s link open", bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
#if SD_STORE
    fprintf(f_test,  "%s link open", bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
#endif
    gpio_set_level(GPIO_NUM_33,1);
    update_result(&prov_statistic, true, false, false);
    esp_timer_stop(prov_statistic.prov_timer);
    ret = esp_timer_start_once(prov_statistic.prov_timer, PROV_TIME_OUT );
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "timer start failed %d %x\n",__LINE__,ret);
    }
}

static void prov_link_close(esp_ble_mesh_prov_bearer_t bearer, uint8_t reason)
{
    ESP_LOGI(TAG, "%s link close, reason 0x%02x",
             bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT", reason);
    gpio_set_level(GPIO_NUM_33,0);


    gpio_set_level(GPIO_NUM_32, 0);  
    gpio_set_level(GPIO_NUM_22, 0);    

    gpio_set_level(GPIO_NUM_23, 0);   

    gpio_set_level(GPIO_NUM_14, 0);   
    gpio_set_level(GPIO_NUM_12, 0);   

    gpio_set_level(GPIO_NUM_18, 0);  
    // gpio_set_level(GPIO_NUM_17, 0);  
    gpio_set_level(GPIO_NUM_16, 0);   
}

static void recv_unprov_adv_pkt(uint8_t dev_uuid[16], uint8_t addr[BD_ADDR_LEN],
                                esp_ble_mesh_addr_type_t addr_type, uint16_t oob_info,
                                uint8_t adv_type, esp_ble_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_unprov_dev_add_t add_dev = {0};
    int err;

    /* Due to the API esp_ble_mesh_provisioner_set_dev_uuid_match, Provisioner will only
     * use this callback to report the devices, whose device UUID starts with 0xdd & 0xdd,
     * to the application layer.
     */

    ESP_LOGI(TAG, "address: %s, address type: %d, adv type: %d", bt_hex(addr, BD_ADDR_LEN), addr_type, adv_type);
    ESP_LOGI(TAG, "device uuid: %s", bt_hex(dev_uuid, 16));
    ESP_LOGI(TAG, "oob info: %d, bearer: %s", oob_info, (bearer & ESP_BLE_MESH_PROV_ADV) ? "PB-ADV" : "PB-GATT");

    memcpy(add_dev.addr, addr, BD_ADDR_LEN);
    add_dev.addr_type = (uint8_t)addr_type;
    memcpy(add_dev.uuid, dev_uuid, 16);
    add_dev.oob_info = oob_info;
    add_dev.bearer = (uint8_t)bearer;
    /* Note: If unprovisioned device adv packets have not been received, we should not add
             device with ADD_DEV_START_PROV_NOW_FLAG set. */
    err = esp_ble_mesh_provisioner_add_unprov_dev(&add_dev,
            ADD_DEV_RM_AFTER_PROV_FLAG | ADD_DEV_START_PROV_NOW_FLAG | ADD_DEV_FLUSHABLE_DEV_FLAG);
    if (err) {
        ESP_LOGE(TAG, "%s: Add unprovisioned device into queue failed", __func__);
    }

    return;
}

void send_all_node_reset(void)
{
    int err;

    for(int i=0; i < PROV_MAX_COUNT; i++){
        esp_ble_mesh_cfg_client_set_state_t set_state = {0};
        esp_ble_mesh_client_common_param_t common = {0};
        common.opcode = ESP_BLE_MESH_MODEL_OP_NODE_RESET;
        common.model = config_client.model;
        common.ctx.net_idx = prov_key.net_idx;
        common.ctx.app_idx = prov_key.app_idx;
        common.ctx.addr    = prov_statistic.prov_uuid[i];
        common.ctx.send_ttl = MSG_SEND_TTL;
        common.ctx.send_rel = MSG_SEND_REL;
        common.msg_timeout  = MSG_TIMEOUT;
        common.msg_role     = MSG_ROLE;

        set_state.app_key_add.net_idx = prov_key.net_idx;
        set_state.app_key_add.app_idx = prov_key.app_idx;
        memcpy(set_state.app_key_add.app_key, prov_key.app_key, 16);
        err = esp_ble_mesh_config_client_set_state(&common, &set_state);
        if (err) {
            ESP_LOGE(TAG, "%s: Config AppKey Add failed", __func__);
            return;
        }
    }
}

static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
                                             esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT, err_code %d", param->provisioner_prov_enable_comp.err_code);
        // int err = bt_mesh_config_scan_enable();
        // if (err) {
        //     ESP_LOGE(TAG, "config_scan_enable (err %d)", err);
        // }
        esp_coex_status_bit_set(ESP_COEX_ST_TYPE_BLE, ESP_COEX_BLE_ST_MESH_CONFIG);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT, err_code %d", param->provisioner_prov_disable_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT");
        recv_unprov_adv_pkt(param->provisioner_recv_unprov_adv_pkt.dev_uuid, param->provisioner_recv_unprov_adv_pkt.addr,
                            param->provisioner_recv_unprov_adv_pkt.addr_type, param->provisioner_recv_unprov_adv_pkt.oob_info,
                            param->provisioner_recv_unprov_adv_pkt.adv_type, param->provisioner_recv_unprov_adv_pkt.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT:
        prov_link_open(param->provisioner_prov_link_open.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT:
        prov_link_close(param->provisioner_prov_link_close.bearer, param->provisioner_prov_link_close.reason);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT:
        prov_complete(param->provisioner_prov_complete.node_idx, param->provisioner_prov_complete.device_uuid,
                      param->provisioner_prov_complete.unicast_addr, param->provisioner_prov_complete.element_num,
                      param->provisioner_prov_complete.netkey_idx);
        break;
    case ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT, err_code %d", param->provisioner_add_unprov_dev_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT, err_code %d", param->provisioner_set_dev_uuid_match_comp.err_code);
        if(param->provisioner_set_dev_uuid_match_comp.err_code != 0){
            if(prov_statistic.prov_count >= PROV_MAX_COUNT){
                prov_statistic.prov_count = 0;
            }
            esp_ble_mesh_provisioner_set_dev_uuid_match(match_dev_uuid[prov_statistic.prov_count], UUID_LEN, 0x0, true);
        }
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT: {
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT, err_code %d", param->provisioner_set_node_name_comp.err_code);
        if (param->provisioner_set_node_name_comp.err_code == ESP_OK) {
            const char *name = NULL;
            name = esp_ble_mesh_provisioner_get_node_name(param->provisioner_set_node_name_comp.node_index);
            if (!name) {
                ESP_LOGE(TAG, "Get node name failed");
                return;
            }
            ESP_LOGI(TAG, "Node %d name is: %s", param->provisioner_set_node_name_comp.node_index, name);
        }
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT: {
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT, err_code %d", param->provisioner_add_app_key_comp.err_code);
        if (param->provisioner_add_app_key_comp.err_code == ESP_OK) {
            esp_err_t err = 0;
            prov_key.app_idx = param->provisioner_add_app_key_comp.app_idx;
            err = esp_ble_mesh_provisioner_bind_app_key_to_local_model(PROV_OWN_ADDR, prov_key.app_idx,
                    ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI, CID_NVAL);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Provisioner bind local model appkey failed");
                return;
            }
        }
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT, err_code %d", param->provisioner_bind_app_key_to_model_comp.err_code);
        break;
    default:
        break;
    }

    return;
}

static void example_ble_mesh_config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                              esp_ble_mesh_cfg_client_cb_param_t *param)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_node_info_t *node = NULL;
    uint32_t opcode;
    uint16_t addr;
    int err;

    opcode = param->params->opcode;
    addr = param->params->ctx.addr;

    if(prov_statistic.last_count != prov_statistic.prov_count){
        ESP_LOGI(TAG, "cancle timeout event");
        return;
    }

    ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x, opcode: 0x%04x",
             __func__, param->error_code, event, param->params->ctx.addr, opcode);

    node = example_ble_mesh_get_node_info(addr);
    if (!node) {
        ESP_LOGE(TAG, "%s: Get node info failed", __func__);
        return;
    }
    
    if (param->error_code) {
        ESP_LOGE(TAG, "Send config client message failed, opcode 0x%04x", opcode);
        switch (opcode)
        {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:{
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND);
            set_state.model_app_bind.element_addr = node->unicast;
            set_state.model_app_bind.model_app_idx = prov_key.app_idx;
            set_state.model_app_bind.model_id = ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV;
            set_state.model_app_bind.company_id = CID_NVAL;
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Model App Bind failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }


        return;
    }

    switch (event) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET: {
            ESP_LOGI(TAG, "composition data %s", bt_hex(param->status_cb.comp_data_status.composition_data->data,
                     param->status_cb.comp_data_status.composition_data->len));
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD);
            set_state.app_key_add.net_idx = prov_key.net_idx;
            set_state.app_key_add.app_idx = prov_key.app_idx;
            memcpy(set_state.app_key_add.app_key, prov_key.app_key, 16);
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config AppKey Add failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD: {
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND);
            set_state.model_app_bind.element_addr = node->unicast;
            set_state.model_app_bind.model_app_idx = prov_key.app_idx;
            set_state.model_app_bind.model_id = ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV;
            set_state.model_app_bind.company_id = CID_NVAL;
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Model App Bind failed", __func__);
                return;
            }
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND: {
            esp_ble_mesh_generic_client_get_state_t get_state = {0};
            example_ble_mesh_set_msg_common(&common, node, onoff_client.model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET);
            err = esp_ble_mesh_generic_client_get_state(&common, &get_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Generic OnOff Get failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_STATUS:
            ESP_LOG_BUFFER_HEX("composition data %s", param->status_cb.comp_data_status.composition_data->data,
                               param->status_cb.comp_data_status.composition_data->len);
            break;
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_STATUS:
            break;
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET: {
            esp_ble_mesh_cfg_client_get_state_t get_state = {0};
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET);
            get_state.comp_data_get.page = COMP_DATA_PAGE_0;
            err = esp_ble_mesh_config_client_get_state(&common, &get_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Composition Data Get failed", __func__);
                return;
            }
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD: {
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD);
            set_state.app_key_add.net_idx = prov_key.net_idx;
            set_state.app_key_add.app_idx = prov_key.app_idx;
            memcpy(set_state.app_key_add.app_key, prov_key.app_key, 16);
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config AppKey Add failed", __func__);
                return;
            }
            break;
        }
        case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND: {
            esp_ble_mesh_cfg_client_set_state_t set_state = {0};
            example_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND);
            set_state.model_app_bind.element_addr = node->unicast;
            set_state.model_app_bind.model_app_idx = prov_key.app_idx;
            set_state.model_app_bind.model_id = ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV;
            set_state.model_app_bind.company_id = CID_NVAL;
            err = esp_ble_mesh_config_client_set_state(&common, &set_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Config Model App Bind failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    default:
        ESP_LOGE(TAG, "Not a config client status message event");
        break;
    }
}

static void example_ble_mesh_generic_client_cb(esp_ble_mesh_generic_client_cb_event_t event,
                                               esp_ble_mesh_generic_client_cb_param_t *param)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_node_info_t *node = NULL;
    uint32_t opcode;
    uint16_t addr;
    int err;

    opcode = param->params->opcode;
    addr = param->params->ctx.addr;

    ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x, opcode: 0x%04x",
             __func__, param->error_code, event, param->params->ctx.addr, opcode);

    if (param->error_code) {
        ESP_LOGE(TAG, "Send generic client message failed, opcode 0x%04x", opcode);
        return;
    }

    node = example_ble_mesh_get_node_info(addr);
    if (!node) {
        ESP_LOGE(TAG, "%s: Get node info failed", __func__);
        return;
    }

    switch (event) {
    case ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET: {
            node->onoff = param->status_cb.onoff_status.present_onoff;
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET onoff: 0x%02x", node->onoff);
#if SD_STORE
            fprintf(f_test,"ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET onoff: 0x%02x", node->onoff);
#endif
            esp_timer_stop(prov_statistic.prov_timer);
            int ret = esp_timer_start_once(prov_statistic.prov_timer, PROV_NEXT_DEV );
            if (ret != ESP_OK) {
                printf("timer start failed %d %x\n",__LINE__,ret);
            }
            update_result(&prov_statistic, false, true, false);
            ESP_LOGW(TAG, "all:%d,success: %d,time:%d " , prov_statistic.all_count,prov_statistic.success_count,prov_resut.one_time);
#if SD_STORE
            fprintf(f_test, "all:%d,success: %d, time:%d", prov_statistic.all_count,prov_statistic.success_count,prov_resut.one_time);
#endif
            if(prov_statistic.all_count % TEST_COUNT_MAX == 0){
                ESP_LOGW(TAG, "average: %lld" , prov_resut.average / prov_statistic.success_count);
#if SD_STORE
                fprintf(f_test,"average: %lld", prov_resut.average / prov_statistic.success_count);
#endif
                prov_resut.average = 0;
#if SD_STORE
                fclose(f_test);
#endif
                esp_restart();
            }
            printf("prov_count: %d \n",prov_statistic.prov_count );
            if(prov_statistic.prov_count == PROV_MAX_COUNT){
                send_all_node_reset();
            }

#if SD_STORE
            fclose(f_test);
            f_test = fopen(file_path, "a");
            if (f_test == NULL) {
                ESP_LOGE(TAG, "Failed to open file for writing");
                return;
            }
#endif
            break;
        }
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET:
            node->onoff = param->status_cb.onoff_status.present_onoff;
            ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET onoff: 0x%02x", node->onoff);
            break;
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT:
        break;
    case ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT:
        /* If failed to receive the responses, these messages will be resend */
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET: {
            esp_ble_mesh_generic_client_get_state_t get_state = {0};
            example_ble_mesh_set_msg_common(&common, node, onoff_client.model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET);
            err = esp_ble_mesh_generic_client_get_state(&common, &get_state);
            if (err) {
                ESP_LOGE(TAG, "%s: Generic OnOff Get failed", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    default:
        ESP_LOGE(TAG, "Not a generic client status message event");
        break;
    }
}

static EventGroupHandle_t wifi_event_group;

const int IPV4_GOTIP_BIT = BIT0;
const int IPV6_GOTIP_BIT = BIT1;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        /* enable ipv6 */
        tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, IPV4_GOTIP_BIT);
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, IPV4_GOTIP_BIT);
        xEventGroupClearBits(wifi_event_group, IPV6_GOTIP_BIT);
        break;
    case SYSTEM_EVENT_AP_STA_GOT_IP6:
        xEventGroupSetBits(wifi_event_group, IPV6_GOTIP_BIT);
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP6");

        char *ip6 = ip6addr_ntoa(&event->event_info.got_ip6.ip6_info.ip);
        ESP_LOGI(TAG, "IPv6: %s", ip6);
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void wait_for_ip()
{
    uint32_t bits = IPV4_GOTIP_BIT | IPV6_GOTIP_BIT ;

    ESP_LOGW(TAG, "Waiting for AP connection...");
    xEventGroupWaitBits(wifi_event_group, bits, false, true, portMAX_DELAY);
#if SD_STORE
    fprintf(f_test,"Connected to AP");
#endif
    ESP_LOGW(TAG, "Connected to AP");
}

static esp_err_t ble_mesh_init(void)
{

    esp_err_t err;

    prov_key.net_idx = ESP_BLE_MESH_KEY_PRIMARY;
    prov_key.app_idx = APP_KEY_IDX;
    memset(prov_key.app_key, APP_KEY_OCTET, sizeof(prov_key.app_key));

    esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb);
    esp_ble_mesh_register_config_client_callback(example_ble_mesh_config_client_cb);
    esp_ble_mesh_register_generic_client_callback(example_ble_mesh_generic_client_cb);


    err = esp_ble_mesh_init(&provision, &composition);
    if (err) {
        ESP_LOGE(TAG, "Initializing mesh failed (err %d)", err);
        return err;
    }
    
    esp_ble_mesh_provisioner_set_dev_uuid_match(match_dev_uuid[prov_statistic.prov_count], UUID_LEN, 0x0, true);

    esp_ble_mesh_provisioner_prov_enable(ESP_BLE_MESH_PROV_ADV );

    esp_ble_mesh_provisioner_add_local_app_key(prov_key.app_key, prov_key.net_idx, prov_key.app_idx);

    ESP_LOGI(TAG, "BLE Mesh Provisioner initialized");

    return err;
}



static void prov_timeout(void *arg)
{
    printf("%s \n", __func__);
#if SD_STORE
    fprintf(f_test,"%s \n", __func__);
    fclose(f_test);
    f_test = fopen(file_path, "a");
    if (f_test == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
#endif
    struct prov_statistic_t * result = (struct prov_statistic_t *)arg;
    update_result(result, false, false, true);
    result->prov_count += 1;
    result->last_count = -1;
    if(result->prov_count >= PROV_MAX_COUNT){
        result->prov_count = 0;
    }
    
    esp_ble_mesh_provisioner_set_dev_uuid_match(match_dev_uuid[result->prov_count], UUID_LEN, 0x0, true);
    esp_timer_stop(prov_statistic.prov_timer);
    int ret = esp_timer_start_once(prov_statistic.prov_timer, PROV_MAX_TIMEOUT );
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "timer start failed %d %x\n",__LINE__,ret);
    }
}

esp_err_t create_prov_timer( esp_timer_handle_t * handle)
{
    esp_err_t ret;
    esp_timer_create_args_t tca = {
       .callback = (esp_timer_cb_t)prov_timeout,
       .dispatch_method = ESP_TIMER_TASK,
       .name = "RELAY_TABLE",
    };
    tca.arg = &prov_statistic;

    ret = esp_timer_create(&tca, handle);
    if (ret != ESP_OK) {
        printf("timer create failed %d %x\n",__LINE__,ret);
        return ret;
    }
    // esp_timer_get_time();
    ret = esp_timer_start_once(*handle, PROV_TIME_OUT );
    if (ret != ESP_OK) {
        printf("timer start failed %d %x\n",__LINE__,ret);
        return ret;
    }
    return ret;
}

//gpio_set_level(GPIO_NUM_25,1);     0//timout = 1 
//gpio_set_level(GPIO_NUM_26,1);     1//scan
//gpio_set_level(GPIO_NUM_27,1);     2//adv
//gpio_set_level(GPIO_NUM_33,1);     3//link open --> link close


//gpio_set_level(GPIO_NUM_32,1);     4//link open --> link ack
//gpio_set_level(GPIO_NUM_22,1);     5//invite --> invite ack

//gpio_set_level(GPIO_NUM_23,1);     6//recv capabilities

//gpio_set_level(GPIO_NUM_14,1);     7//start --> start ack
//gpio_set_level(GPIO_NUM_12,1);     8//tx public key --> tx ack

//gpio_set_level(GPIO_NUM_18,1);     9//recv public key 
//gpio_set_level(GPIO_NUM_17,1);     10//tx confirmation  -->rx confirmation
//gpio_set_level(GPIO_NUM_16,1);     11//random + prov data



void gpio_test(void)
{
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_25,1);     //timout = 1
    vTaskDelay(200/portTICK_PERIOD_MS);
        gpio_set_level(GPIO_NUM_25,0);
    gpio_set_level(GPIO_NUM_22,1);      //scan
    vTaskDelay(200/portTICK_PERIOD_MS);
   gpio_set_level(GPIO_NUM_22,0);
vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_23,1);      //adv
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_23,0);
vTaskDelay(200/portTICK_PERIOD_MS);


vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_26,1);     //link open --> link close
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_26,0);
vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_27,1);     //link open --> link ack
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_27,0);
vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_33,1);     //invite --> invite ack
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_33,0);
vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_32,1);      //recv capabilities
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_32,0);
 vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_14,1);      //start --> start ack
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_14,0);
 vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_12,1);      //tx public key --> rx public key
    vTaskDelay(200/portTICK_PERIOD_MS);
   gpio_set_level(GPIO_NUM_12,0);
vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_18,1);      //tx confirmation  -->rx confirmation
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_18,0);
vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_17,1);     //random + prov data
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_17,0);
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_16,1);     //link close
    vTaskDelay(200/portTICK_PERIOD_MS);
    gpio_set_level(GPIO_NUM_16,0);
    vTaskDelay(200/portTICK_PERIOD_MS);
}

void IRAM_ATTR btdm_start_scan_trigger_gpio()
{
    gpio_set_level(GPIO_NUM_26, 0);
}

void IRAM_ATTR btdm_stop_scan_trigger_gpio()
{
    gpio_set_level(GPIO_NUM_26, 1);
}

void IRAM_ATTR btdm_start_adv_trigger_gpio() 
{
    gpio_set_level(GPIO_NUM_27, 0);
}

void IRAM_ATTR btdm_stop_adv_trigger_gpio()
{
    gpio_set_level(GPIO_NUM_27, 1);
}

int store_log(const char *format, ...)
{
    return 0;
}
#if SD_STORE
void sd_init(void)
{
    ESP_LOGI(TAG, "Initializing SD card");

#ifndef USE_SPI_MODE
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY);   // CMD, needed in 4- and 1- line modes
    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);    // D0, needed in 4- and 1-line modes
    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);    // D1, needed in 4-line mode only
    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY);   // D2, needed in 4-line mode only
    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY);   // D3, needed in 4- and 1-line modes

#else
    ESP_LOGI(TAG, "Using SPI peripheral");
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = PIN_NUM_MISO;
    slot_config.gpio_mosi = PIN_NUM_MOSI;
    slot_config.gpio_sck  = PIN_NUM_CLK;
    slot_config.gpio_cs   = PIN_NUM_CS;
#endif //USE_SPI_MODE
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
    ESP_LOGI(TAG, "Opening file");

    f_test = fopen(file_path, "a");
    if (f_test == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f_test, "prov test start \n");
}
#endif

void app_main(void)
{
    esp_err_t err;
    ESP_LOGI(TAG, "Initializing low distance...");
    board_init();
#if SD_STORE
    sd_init();
#endif
    // gpio_test();
    err = create_prov_timer(&prov_statistic.prov_timer);
    if (err) {
        ESP_LOGE(TAG, "create_prov_timer failed (err %d)", err);
        return;
    }

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    initialise_wifi();
    wait_for_ip();
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    ble_mesh_get_dev_uuid(dev_uuid);
    err = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV,ESP_PWR_LVL_P9);
    if (err) {
        ESP_LOGE(TAG, "tx_power_set failed (err %d)", err);
        return;
    }

    printf("power:%d\n", esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_ADV));
    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }
}
