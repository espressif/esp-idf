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

#include "access.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_ble_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_api.h"

#define TAG "ble_mesh_test"

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
        case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
            break;
        case ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT:
            break;
        case ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT:
            break;
        case ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT:
            
            break;
        case ESP_BLE_MESH_NODE_PROV_INPUT_EVT:
            break;
        case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:
            //prov_complete(param->prov_comp.net_idx,param->prov_comp.addr);
            break;
        case ESP_BLE_MESH_NODE_PROV_RESET_EVT:
            break;
        default:
            break;
    }
    return;
}

void esp_mesh_model_cb(esp_ble_mesh_model_cb_event_t event, 
                       esp_ble_mesh_model_cb_param_t *param)
{
    switch(event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT:
    break;
    case ESP_BLE_MESH_SERVER_MODEL_SEND_COMP_EVT:
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT:
        break;
    default:
        break;
    }
}

static int ble_mesh_init(void)
{
    int err = 0;

    // Register the provisioning callback function
    esp_ble_mesh_register_prov_callback(esp_mesh_prov_cb);
    // Register the model send callback function
    esp_ble_mesh_register_custom_model_callback(esp_mesh_model_cb);
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

TEST_CASE("ble_mesh_node_init", "[ble_mesh]")
{
    esp_err_t ret = test_main();
    TEST_ASSERT(ret == ESP_OK);
    /* We wait init finish 200ms here */
    vTaskDelay(200 / portTICK_PERIOD_MS);
    const struct bt_mesh_comp *dev_comp = bt_mesh_comp_get();
    /* We need to check if the corret model and element values are stored in
       the dev_comp pointer. */
    TEST_ASSERT(dev_comp);
    /* We are register the cid = 0 to the stack, so we should check if its value is correct */
    TEST_ASSERT(dev_comp->cid == 0);
    /* We are register 2 element, one have root_models and the other have light_model */
    TEST_ASSERT(dev_comp->elem_count == 2);
    /* Check the element pointer */
    TEST_ASSERT(dev_comp->elem);
    for (int i = 0; i < dev_comp->elem_count; i++) {
        struct bt_mesh_elem *elem = &dev_comp->elem[i];
        /* We are register 1 sig model in it and register 1 vendor model in it */
        TEST_ASSERT((elem->model_count == 1) || (elem->vnd_model_count == 1));

        for (int j = 0; j < elem->model_count; j++) {
            struct bt_mesh_model *models = &elem->models[j];
            TEST_ASSERT(models);
            /* The model id should equal the onoff server model id */
            TEST_ASSERT(models->id == ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV);
            /* The elem pointer in the models shouldn't NULL */
            TEST_ASSERT(models->elem);
            /* The models op pointer shouldn't NULL */
            TEST_ASSERT(models->op);
            TEST_ASSERT(models->op[0].opcode == ESP_BLE_MESH_MODEL_OP_2(0x82, 0x01));
            TEST_ASSERT(models->op[1].opcode == ESP_BLE_MESH_MODEL_OP_2(0x82, 0x02));
            TEST_ASSERT(models->op[2].opcode == ESP_BLE_MESH_MODEL_OP_2(0x82, 0x03));
            TEST_ASSERT(models->op[0].min_len == 0);
            TEST_ASSERT(models->op[1].min_len == 2);
            TEST_ASSERT(models->op[2].min_len == 2);
        }
        
        for (int k = 0; k < elem->vnd_model_count; k++) {
            struct bt_mesh_model *vnd_models = &elem->vnd_models[k];
            TEST_ASSERT(vnd_models);
            /* The vendor model company id should equal 0x01 */
            TEST_ASSERT(vnd_models->vnd.company == 0x01);
            /* The vendor model company id should equal 0 */
            TEST_ASSERT(vnd_models->vnd.id == 0x7802);
            /* The elem pointer in the models shouldn't NULL */
            TEST_ASSERT(vnd_models->elem);
            /* The models op pointer shouldn't NULL */
            TEST_ASSERT(vnd_models->op);
            TEST_ASSERT(vnd_models->op[0].opcode == ESP_BLE_MESH_MODEL_OP_3(0x01, 0x0D01));
            TEST_ASSERT(vnd_models->op[1].opcode == ESP_BLE_MESH_MODEL_OP_3(0x10, 0x0D10));
            TEST_ASSERT(vnd_models->op[2].opcode == ESP_BLE_MESH_MODEL_OP_3(0x08, 0x0D08));
            TEST_ASSERT(vnd_models->op[0].min_len == 1);
            TEST_ASSERT(vnd_models->op[1].min_len == 1);
            TEST_ASSERT(vnd_models->op[2].min_len == 1);
        }
    }
}
