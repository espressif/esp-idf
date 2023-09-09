# 1. Introduction
## 1.1 Demo Function

This demo completes the following functions:

1. Provisioning an unprovisioned device and change it to a node.
2. Binding the provisioner's Appkey to its own models.
3. Sending messages to the node about the Appkey and the fast provisioning information.
4. Getting the addresses of all the nodes in the fast provisioning network. 
5. Controlling the nodes by their group address.

**Note: The demo's functionality is similar to that of the EspBleMesh app.**

## 1.2 Node Composition

This demo has only one element, in which the following four models are implemented:

- The **Configuration Server** model is used to represent a mesh network configuration of a device.
- The **Configuration Client** model is used to represent an element that can control and monitor the configuration of a node.
- The **Generic OnOff Client** model controls a Generic OnOff Server via messages defined by the **Generic OnOff** model (turning on and off the lights in this demo).
- The **Vendor Client** model is used to control the `fast_prov_server` state, which defines the fast provisioning behavior of a node.

**Note: For detailed information about these models, please refer to other BLE Mesh demos.**

## 2. Code Analysis

Code initialization part reference [Initializing Bluetooth and BLE Mesh](../../../wifi_coexist/tutorial/BLE_Mesh_WiFi_Coexist_Example_Walkthrough.md).

### 2.1  Data Structure

`example_prov_info_t` is used to define the keys, the address range can be assigned by a node, and the maximum number of nodes supported by the mesh network.

| Name        |Description               |
| ----------------------|------------------------- |
| `net_idx`      | Netkey index value  |
| `app_idx`      | AppKey index value  |
| `app_key[16]`  | Appkey, which is used throughout the network |
| `node_addr_cnt`| The maximum number of nodes supported in the mesh network，which serves the same purpose of the `Fast provisioning count` parameter in the EspBleMesh app|
| `unicast_min` | Minimum unicast address to be assigned to the nodes in the mesh network |
| `unicast_max` | Maximum unicast address to be assigned to the nodes in the mesh network |
| `group_addr`| The group address, which is used to control the on/off state of all nodes in the mesh network, that is said, turning on and off the lights in this demo|
| `match_val[16]`| The value used by the Fast Provisioning Provisioner to filter the devices to be provisioned  |
| `match_len` | The maximum length of `match_val[16]` |
| `max_node_num`    | The maximum number of nodes can be provisioned by the client |

### 2.2  Code Flow

The events and APIs in this section are presented in the same order with code execution.

### 2.2.1 Initialization

#### 2.2.1.1 Set the UUID Filter

The `esp_ble_mesh_provisioner_set_dev_uuid_match` API is called by the provisioner to set the part of the device UUID to be compared before starting to provision. 

```
/**
 * @brief         This function is called by Provisioner to set the part of the device UUID
 *                to be compared before starting to provision.
 *
 * @param[in]     match_val: Value to be compared with the part of the device UUID.
 * @param[in]     match_len: Length of the compared match value.
 * @param[in]     offset: Offset of the device UUID to be compared (based on zero).
 * @param[in]     prov_after_match: Flag used to indicate whether provisioner should start to provision
 *                                  the device immediately if the part of the UUID matches.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_dev_uuid_match(const uint8_t *match_val, uint8_t match_len,
        uint8_t offset, bool prov_after_match);
```

```c
err = esp_ble_mesh_provisioner_set_dev_uuid_match(match, 0x02, 0x00, false);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Failed to set matching device UUID", __func__);
    return ESP_FAIL;
}
```



#### 2.2.1.2 Add local Appkey

The provisioner has no Appkey right after it has been initialized. Therefore, you have to add a local Appkey for the provisioner by calling the `esp_ble_mesh_provisioner_add_local_app_key`. 

```c
err = esp_ble_mesh_provisioner_add_local_app_key(prov_info.app_key, prov_info.net_idx, prov_info.app_idx);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Failed to add local application key", __func__);
    return ESP_FAIL;
}
```
Please check the return value of the API calling and the return value of `ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT`, and make sure that the Appkey has been added to this provisioner.

#### 2.2.1.3 Bind Appkey to local model

To control the server model, the client model uses messages to control the server model and these message must be encrypted by the Appkey. To that end, users must bind the Appkey of the provisioner to its local models, which are the **Generic OnOff Client** model and the **Vendor Client** model, by calling the `esp_ble_mesh_provisioner_add_local_app_key` api. 

```c
prov_info.app_idx = param->provisioner_add_app_key_comp.app_idx;
err = esp_ble_mesh_provisioner_bind_app_key_to_local_model(PROV_OWN_ADDR, prov_info.app_idx,
                                              ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI, ESP_BLE_MESH_CID_NVAL);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Failed to bind AppKey with OnOff Client Model", __func__);
    return;
}
err = esp_ble_mesh_provisioner_bind_app_key_to_local_model(PROV_OWN_ADDR, prov_info.app_idx,
                                            ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI, CID_ESP);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Failed to bind AppKey with Fast Prov Client Model", __func__);
    return;
}
```
Please check the return value of the API calling and the return value of the `ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT` event, and make sure that the Appkey has been binded to the local models.


### 2.2.2 Provisioning a device

The unprovisioned devices continuously send the **Unprovisioned Device** beacon, which contains the value of its UUID. 

* If the UUID matched, a `ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT` event will be triggered, which will add the unprovisioned device information to the queue of to-be-provisioned devices.

	```c
	err = esp_ble_mesh_provisioner_add_unprov_dev(&add_dev, flag);
	if (err != ESP_OK) {
   		ESP_LOGE(TAG, "%s: Failed to start provisioning a device", __func__);
    	return;
	}

	if (!reprov) {
   		if (prov_info.max_node_num) {
        	prov_info.max_node_num--;
	}
	}
	```
* If not, this device will be ignored.

After that, all the devices in the queue will be provisioned automatically.

### 2.2.3 Sending cache data

Appkey is among the cache required for this node to become a provisioner.

When the provisioning completes, an `ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT` event will be triggered, which will add the Appkey to the node's **Config Server** model by calling the `esp_ble_mesh_config_client_set_state` API:

```c
common.opcode       = ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD;
common.model        = model;
common.ctx.net_idx  = info->net_idx;
common.ctx.app_idx  = 0x0000; /* not used for config messages */
common.ctx.addr     = info->dst;
common.ctx.send_ttl = 0;
common.msg_timeout  = info->timeout;

return esp_ble_mesh_config_client_set_state(&common, &set);
```

* If API calling succeeds, an `ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT` event will be triggered, which sends other cache information (`example_fast_prov_info_set_t`) to the node's **Vendor Server** model by calling the `example_send_fast_prov_info_set` function;
	* If API calling (`example_send_fast_prov_info_set`) succeeded, a message with an opcode of `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET` will be sent, whose acknowledgement (with an opcode of `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS`) will further trigger an `ESP_BLE_MESH_MODEL_OPERATION_EVT` event  
		```c
		err = example_send_fast_prov_info_set(fast_prov_client.model, &info, &set);
		if (err != ESP_OK) {
    		ESP_LOGE(TAG, "%s: Failed to set Fast Prov Info Set message", __func__);
    	return;
		}
		```
	* If API calling (`example_send_fast_prov_info_set`) times out, an `ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT` event will be triggered.  
* If API calling times out, an `ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT` event is triggered.

After that, this node has the ability to provisioning other nodes as a provisioner, and further controls other nodes.

**Note: The message with an opcode of `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET` contains the group address of all the nodes. When a node receives this message, it will automatically subscribe the Onoff Server model of this address.**

### 2.2.4 Controlling the node

When the `ESP_BLE_MESH_MODEL_OPERATION_EVT` event is triggered, the provisioner starts a timer.  

```c
        ESP_LOG_BUFFER_HEX("fast prov info status", data, len);
#if !defined(CONFIG_BLE_MESH_FAST_PROV)
        prim_prov_addr = ctx->addr;
        k_delayed_work_init(&get_all_node_addr_timer, example_get_all_node_addr);
        k_delayed_work_submit(&get_all_node_addr_timer, GET_ALL_NODE_ADDR_TIMEOUT);
#endif
        break;
```
After the timers times out, the provisioner starts to get the addresses of all nodes in the mesh network by calling the `example_send_fast_prov_all_node_addr_get` function, which sends a message with an opcode of `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET`.

```c
err = example_send_fast_prov_all_node_addr_get(model, &info);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Failed to send Fast Prov Node Address Get message", __func__);
    return;
}
```

After that, the provisioner will receive an acknowledgement, which is a message with an opcode of `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS`, which triggers the `ESP_BLE_MESH_MODEL_OPERATION_EVT` event.

Then, the provisioner is able to turn on all the nodes (which are lights in this demo) by calling the `example_send_generic_onoff_set` function using the group address.

```c
example_msg_common_info_t info = {
    .net_idx = node->net_idx,
    .app_idx = node->app_idx,
    .dst = node->group_addr,
    .timeout = 0,
};
err = example_send_generic_onoff_set(cli_model, &info, LED_ON, 0x00, false);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Failed to send Generic OnOff Set Unack message", __func__);
    return ESP_FAIL;
}
```
