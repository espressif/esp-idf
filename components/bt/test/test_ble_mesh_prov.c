/*
 Tests for the BLE Mesh Client API and timer implementation
*/

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/ringbuf.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_api.h"

#define TAG "ble_mesh_test"

static SemaphoreHandle_t ble_mesh_test_sem;
static int error_code = -1;

static esp_ble_mesh_model_pub_t gen_onoff_pub = {
    .msg = NET_BUF_SIMPLE(2 + 1),
    .update = NULL,
};

static esp_ble_mesh_model_op_t gen_onoff_op[] = {
        { ESP_BLE_MESH_MODEL_OP_2(0x82, 0x01), 0, 0},
        { ESP_BLE_MESH_MODEL_OP_2(0x82, 0x02), 2, 0},
        { ESP_BLE_MESH_MODEL_OP_2(0x82, 0x03), 2, 0},
        /* Each model operation struct array must use this terminator 
         * as the end tag of the operation uint. */
        ESP_BLE_MESH_MODEL_OP_END,
    };

/* Define the SIG light onoff model */
static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_op,
    &gen_onoff_pub, NULL),
};

/* Define the vendor light model operation */
static esp_ble_mesh_model_op_t light_op[] = {
    {ESP_BLE_MESH_MODEL_OP_3(0x01, 0x0D01), 1, 0}, //BRIGHINESS_GET
    {ESP_BLE_MESH_MODEL_OP_3(0x10, 0x0D10), 1, 0}, //BRIGHINESS_GET_RESULT
    {ESP_BLE_MESH_MODEL_OP_3(0x08, 0x0D08), 1, 0}, //BRIGHINESS_SET
    /* Each model operation struct array must use this terminator 
     * as the end tag of the operation uint. */
    ESP_BLE_MESH_MODEL_OP_END,
};

/* Define the vendor light model, the componey id is 0x01, and the model id is 0x7802 */
static esp_ble_mesh_model_t light_model[] = {
    ESP_BLE_MESH_VENDOR_MODEL(0x01, 0x7802, light_op, NULL, NULL),
};

/* There are two element will register to the stack, one is the sig onoff model, the
   other is vendor light model */
static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEM(0, root_models, ESP_BLE_MESH_MODEL_NONE),
    ESP_BLE_MESH_ELEM(0, ESP_BLE_MESH_MODEL_NONE, light_model),
};

static const esp_ble_mesh_comp_t comp = {
    .cid = 0,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

static uint8_t dev_uuid[16] = { 0xdd, 0xdd };

/* Disable OOB security for SILabs Android app */
static esp_ble_mesh_prov_t prov = {
         .uuid = dev_uuid,
#if 0
    .output_size = 4,
    .output_actions = ESP_BLE_MESH_DISPLAY_NUMBER,
    .input_actions = ESP_BLE_MESH_PUSH,
    .input_size = 4,
#else
    .output_size = 0,
    .output_actions = 0,
#endif
};

static char *esp_ble_mesh_prov_event_to_str(esp_ble_mesh_prov_cb_event_t event)
{
    switch(event) {
        case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
            return "ESP_BLE_MESH_PROV_REGISTER_COMP_EVT";
        case ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT:
            return "ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT";
        case ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT:
            return "ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT";
        case ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT:
            return "ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT";
        case ESP_BLE_MESH_NODE_PROV_INPUT_EVT:
            return "ESP_BLE_MESH_NODE_PROV_INPUT_EVT";
        case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:
            return "ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT";
        case ESP_BLE_MESH_NODE_PROV_RESET_EVT:
            return "ESP_BLE_MESH_NODE_PROV_RESET_EVT";
            //case ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT:
            //    return "ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT";
            //case ESP_BLE_MESH_MODEL_OPERATION_EVT:
            //    return "ESP_BLE_MESH_MODEL_OPERATION_EVT";
        default:
            return "Invalid mesh provision event.";
    }

    return NULL;
}

static void esp_mesh_prov_cb(esp_ble_mesh_prov_cb_event_t event, 
                      esp_ble_mesh_prov_cb_param_t *param)
{
    ESP_LOGE(TAG, "%s, event = %s", __func__, esp_ble_mesh_prov_event_to_str(event));
    switch(event) {
        case ESP_BLE_MESH_PROVISIONER_DEV_ADD_COMP_EVT:
            error_code = param->provisioner_dev_add_comp.err_code;
            xSemaphoreGive(ble_mesh_test_sem);
            break;
        case ESP_BLE_MESH_PROVISIONER_DEV_DEL_COMP_EVT:
            error_code = param->provisioner_dev_del_comp.err_code;
            xSemaphoreGive(ble_mesh_test_sem);
            break;
        case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT:
            error_code = param->set_node_name_comp.err_code;
            xSemaphoreGive(ble_mesh_test_sem);
            break;
        case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT:
            error_code = param->add_local_app_key_comp.err_code;
            xSemaphoreGive(ble_mesh_test_sem);
            break;
        default:
            break;
    }
    return;
}

static int ble_mesh_init(void)
{
    int err = 0;

    // Register the provisioning callback function
    esp_ble_mesh_register_prov_callback(esp_mesh_prov_cb);
    memcpy(dev_uuid, esp_bt_dev_get_address(), 6);

    err = esp_ble_mesh_init(&prov, &comp);
    if (err) {
        ESP_LOGE(TAG, "Initializing mesh failed (err %d)\n", err);
        return err;
    }

    esp_ble_mesh_node_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);

    ESP_LOGI(TAG, "Mesh initialized\n");

    return err;
}

static esp_err_t bluetooth_init(void)
{
    esp_err_t ret;

    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed\n", __func__);
        return ret;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
        return ret;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluetooth failed\n", __func__);
        return ret;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s enable bluetooth failed\n", __func__);
        return ret;
    }

    return ret;
}

static esp_err_t test_main(void)
{
    int err;

    ESP_LOGI(TAG, "Initializing...");

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return err;
    }

    //esp_ble_gap_config_local_privacy(true);

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)\n", err);
        return err;
    }
    
    return 0;
}

extern const struct bt_mesh_comp *dev_comp;

TEST_CASE("ble_mesh_provisioner_cb", "[ble_mesh]")
{
    esp_bd_addr_t addr;
    esp_err_t ret = test_main();
    ble_mesh_test_sem = xSemaphoreCreateCounting(1, 0); //Max 1, initial 0
    memcpy(dev_uuid, esp_bt_dev_get_address(), ESP_BD_ADDR_LEN);
    memcpy(addr, esp_bt_dev_get_address(), ESP_BD_ADDR_LEN);
    // Test the device add api
    TEST_ASSERT((esp_ble_mesh_provisioner_add_unprov_dev(addr, BLE_ADDR_TYPE_PUBLIC, dev_uuid, 
                                   ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT, false)) == ESP_OK);
    xSemaphoreTake(ble_mesh_test_sem, portMAX_DELAY);
    // Should callback 0 when device add complete
    TEST_ASSERT(error_code == 0);
    // Test the device delete api
    TEST_ASSERT((esp_ble_mesh_provisioner_delete_dev(addr, BLE_ADDR_TYPE_PUBLIC, dev_uuid)) == ESP_OK);
    xSemaphoreTake(ble_mesh_test_sem, portMAX_DELAY);
    // Should callback 0 when device delete complete
    TEST_ASSERT(error_code == 0);
    char *node_name = "esp_ble_mesh";
    TEST_ASSERT((esp_ble_mesh_provisioner_set_node_name(0, node_name)) == ESP_OK);
     xSemaphoreTake(ble_mesh_test_sem, portMAX_DELAY);
    // Should callback 0 when set node name complete
    TEST_ASSERT(error_code == 0);
    // Get the node name after set
    char *node_name_get = esp_ble_mesh_provisioner_get_node_name(0);
    // The node name should be equal with that set at the begining.
    TEST_ASSERT(strcmp(node_name_get, "esp_ble_mesh") == 0);
    int index = esp_ble_mesh_provisioner_get_node_index("esp_ble_mesh");
    TEST_ASSERT(index == 0);
    const uint8_t app_key[16] = {0x55};
    uint16_t net_idx = 0;
    uint16_t app_idx = 0;
    TEST_ASSERT((esp_ble_mesh_provisioner_add_local_app_key(app_key, net_idx, app_idx)) == ESP_OK);
    xSemaphoreTake(ble_mesh_test_sem, portMAX_DELAY);
    // Should callback 0 when local app key add complete
    TEST_ASSERT(error_code == 0);
    
}

