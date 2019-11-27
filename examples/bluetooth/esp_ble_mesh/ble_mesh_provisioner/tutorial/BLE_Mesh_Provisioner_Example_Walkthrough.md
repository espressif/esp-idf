
# Demo for ESP BLE Mesh Provisioner

## 1. Introduction

This demo shows how a BLE Mesh device can function as a Provisioner. If you are new to BLE Mesh, please start by checking [Demo for ESP BLE Mesh Node](../../ble_mesh_node/README.md).

## 2. Code Flow

### 2.1 Initialization

The code block below shows the initialization of BLE Mesh.

```c
static int ble_mesh_init(void)
{
    uint8_t match[2] = {0xdd, 0xdd};
    int err = 0;

    prov_key.net_idx = ESP_BLE_MESH_KEY_PRIMARY;
    prov_key.app_idx = ESP_BLE_MESH_APP_IDX;
    memset(prov_key.app_key, APP_KEY_OCTET, sizeof(prov_key.app_key));

    memcpy(dev_uuid, esp_bt_dev_get_address(), BLE_MESH_ADDR_LEN);

    esp_ble_mesh_register_prov_callback(esp_ble_mesh_prov_cb);
    esp_ble_mesh_register_custom_model_callback(esp_ble_mesh_model_cb);
    esp_ble_mesh_register_config_client_callback(esp_ble_mesh_config_client_cb);
    esp_ble_mesh_register_generic_client_callback(esp_ble_mesh_generic_client_cb);

    esp_ble_mesh_provisioner_set_dev_uuid_match(match, sizeof(match), 0x0, false);

    err = esp_ble_mesh_init(&provision, &composition);
    if (err) {
        ESP_LOGE(TAG, "Initializing mesh failed (err %d)", err);
        return err;
    }

    esp_ble_mesh_provisioner_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT);

    esp_ble_mesh_provisioner_add_local_app_key(prov_key.app_key, prov_key.net_idx, prov_key.app_idx);

    ESP_LOGI(TAG, "Provisioner initialized");

    return err;
}
```

The following procedures are needed for the initialization with `ble_mesh_init`.

1. `???`: adds the device to the list of devices for provisioning. Once a device is added, BLE Mesh protocol stack automatically provisions this device.
2. `esp_ble_mesh_provisioner_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT)`: call this API function to scan for the unprovisioned devices in the surrounding environment.

### 2.2 Set and Bind AppKey

While working as a Provisioner, a BLE Mesh device also needs to configure such parameters as AppKey, TTL, and Proxy after provisioning is finished. It is not required if a BLE Mesh device works as a node. 

Please note that successful configuring of AppKeys is of vital importance. App can only send/receive data (set a state, get a state and publish) after an AppKey has been set and bound.

```c
static esp_err_t prov_complete(int node_idx, const esp_ble_mesh_octet16_t uuid,
                               uint16_t unicast, uint8_t elem_num, uint16_t net_idx)
{
    esp_ble_mesh_client_common_param_t common = {0};
    esp_ble_mesh_cfg_client_get_state_t get_state = {0};
    esp_ble_mesh_node_info_t *node = NULL;
    char name[10];
    int err;

    ESP_LOGI(TAG, "node index: 0x%x, unicast address: 0x%02x, element num: %d, netkey index: 0x%02x",
             node_idx, unicast, elem_num, net_idx);
    ESP_LOGI(TAG, "device uuid: %s", bt_hex(uuid, 16));

    sprintf(name, "%s%d", "NODE-", node_idx);
    err = esp_ble_mesh_provisioner_set_node_name(node_idx, name);
    if (err) {
        ESP_LOGE(TAG, "%s: Set node name failed", __func__);
        return ESP_FAIL;
    }

    err = esp_ble_mesh_store_node_info(uuid, unicast, elem_num, LED_OFF);
    if (err) {
        ESP_LOGE(TAG, "%s: Store node info failed", __func__);
        return ESP_FAIL;
    }

    node = esp_ble_mesh_get_node_info(unicast);
    if (!node) {
        ESP_LOGE(TAG, "%s: Get node info failed", __func__);
        return ESP_FAIL;
    }

    esp_ble_mesh_set_msg_common(&common, node, config_client.model, ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET);
    get_state.comp_data_get.page = COMPOSITION_DATA_PAGE_0;
    err = esp_ble_mesh_config_client_get_state(&common, &get_state);
    if (err) {
        ESP_LOGE(TAG, "%s: Send config comp data get failed", __func__);
        return ESP_FAIL;
    }

    return ESP_OK;
}
```

After provisioning is completed, the API function `esp_ble_mesh_provisioner_add_local_app_key` must be called to set and bind an AppKey. After that, the device can exchange model messages with peer devices.

<table><tr><td bgcolor=orange> Note: Set and bind AppKey is used to configure and bind an AppKey through the Configuration Client model. For this reason, a Provisioner must also register the Configuration Client model. </td></tr></table>

### 2.3 Register the Configuration Client Model

The process of registering the Configuration Client model is similar to registering other models, as can be seen below.

```c
static esp_ble_mesh_client_t config_client;
```
```c
static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(NULL, &onoff_client),
};
```

The procedures to register the Configuration Client model include:

1. `static esp_ble_mesh_client_t config_client`: defines a variable `config_client` of the type `esp_ble_mesh_client_t`
2. `ESP_BLE_MESH_MODEL_CFG_CLI(&config_client)`: use the macro `ESP_BLE_MESH_MODEL_CFG_CLI` to add the Configuration Client model to the `root_models` array.

After the definition is completed, the Configuration Client model can be registered simply by passing the pointer of `element` to the BLE Mesh protocol stack.
