# 1. Introduction
## 1.1 Demo Function

1. This demo forwards the message sent by the nRF Mesh app.
2. The user enters the address of the destination node and use it to forwarded packet.
3. The types of the forwarded message include:
	* `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET`, 
	* `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET`,
	* `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK`.
4. The destination node reports its Onoff state with the `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS` message.

Example: The nRF Mesh app sends a `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET` message to the node that runs the `ble_mesh_client_model` project. Then this node sends a `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET` message to the destination node that runs the `onoff_models` project. The address of the destination node is entered by the user via the serial port.

## 1.1.1 What You Need

* 1 x Device that runs the `ble_mesh_client_model` project.
* 1 x Device that runs the `onoff_models` project.
* 1 x Phone that installs the nRF Mesh app for controlling these two devices

## 1.2 Node Composition

This demo has only one element, in which the following three Models are implemented:

- **Configuration Server Model** is mainly to represent a mesh network configuration, such as its AppKey, Relay State, TTL State, Subscription List State, etc.
- **Generic OnOff Client Model** controls a Generic OnOff Server via messages defined by the Generic OnOff Model, that is, turning on and off the lights.
- **Generic OnOff Server Model** implements the nodes' Onoff state.

## 1.3 Message Sequence

You can choose from the 4 message sequences described below:

1. Acknowledged Get
2. Acknowledged Set
3. Unacknowledged Set
4. Acknowledged Set with Periodic Publishing

![Packet interaction](images/message.png)

## 2. Code Analysis

Code initialization part reference [Initializing Bluetooth and BLE Mesh](../../../wifi_coexist/tutorial/BLE_Mesh_WiFi_Coexist_Example_Walkthrough.md)

### 2.1 Model Definition

#### 2.1.1 Generic OnOff Server Model

```c
//Allocating memory for publishing messages.
ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_srv_pub, 2 + 1, MSG_ROLE);
//Registering the minimum length of messages. For example, the minimum length of the ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET message is registered as 2 octets. 
static esp_ble_mesh_model_op_t onoff_op[] = {
    { ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET, 0, 0},
    { ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET, 2, 0},
    { ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, 2, 0},
    ESP_BLE_MESH_MODEL_OP_END,
};
//Registering the Generic Onoff Server Model.
static esp_ble_mesh_model_t root_models[] = {
    //onoff server's onoff state init
    ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, onoff_op,
    &onoff_srv_pub, &led_state[0]),
};
```
#### 2.1.2 Generic OnOff Client Model

```c
//Allocating memory for publishing messages.
ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_cli_pub, 2 + 1, MSG_ROLE);
//Registering the Generic Onoff Client Model.
static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(&onoff_cli_pub, &onoff_client),
};
```

### 2.2 Model Callback Function

#### 2.2.1 The Callback function for the Generic Onoff Client Model

```c
esp_ble_mesh_register_generic_client_callback(esp_ble_mesh_generic_cb);

```
1. The callback function will be triggered when the Client Model:

	* Receives a message that indicates the Onoff state of the Sever Model; Or
	* Calls any APIs that the Server Model send messages.

2. The events that the callback function handle:

| Event name    | Opcode      |Description                                 |
| ------------- | ------------|------------------------------------------- |
| ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT   | ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET    |The event triggered when the Generic Onoff Client Model receives acknowledgment after sending the `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET` message        |
| ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT   | ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET    | The event triggered when the Generic Onoff Client Model receives acknowledgment after sending the `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET` message  |
| ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT     | NA                         | The event triggered when the Generic Onoff Client Model receives publishing messages.    |
| ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT     | ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET    | The event triggered when API (that send messages) calling times out   |
  

#### 2.2.2 The Callback function for the Generic Onoff Server Model

```c
esp_ble_mesh_register_custom_model_callback(esp_ble_mesh_model_cb);

```
1. The callback function will be triggered when the Server Model:

	* Receives a message that indicates operating the Onoff state of the Server Model from the Generic Onoff Client Model; Or
	* Calls any APIs that the Server Model send messages.

2. The events of the callback function:

| Event Name                          | Opcode                                    | Description                                                                                                                                                                              |
|-------------------------------------|-------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ESP_BLE_MESH_MODEL_OPERATION_EVT    | ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET       | The event triggered when the Generic Onoff Server Model receives the ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET message that **gets** the Onoff state of the server from the Client Model       |
| ESP_BLE_MESH_MODEL_OPERATION_EVT    | ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET       | The event triggered when the Generic Onoff Server Model receives the ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET message that **sets** the Onoff state of the server from the Client Model       |
| ESP_BLE_MESH_MODEL_OPERATION_EVT    | ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK | The event triggered when the Generic Onoff Server Model receives the ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK message that **sets** the Onoff state of the server from the Client Model |
| ESP_BLE_MESH_MODEL_SEND_COMP_EVT    | NA                                        | The event triggered when the `esp_ble_mesh_server_model_send_msg` API calling completes                                                                                                  |
| ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT | NA                                        | The event triggered when the `esp_ble_mesh_model_publish` API calling completes                                                                                                          |


### 2.3 Model that Sends Message
#### 2.3.1 Message Control

The `esp_ble_mesh_set_msg_common` function is used to set the message controlling parameters. 

| Parameter Name        |Description               |
| ----------------------|------------------------- |
| `opcode`      | The message opcode  |
| `model`       | The pointer to the client Model struct  |
| `ctx.net_idx` | The NetKey Index of the subnet through which the message is sent |
| `ctx.app_idx` | The AppKey Index for message encryption |
| `ctx.addr`    | The address of the destination nodes |
| `ctx.send_ttl`| The TTL State, which determines how many times a message will be relayed |
| `msg_timeout` | The maximum time the Model will wait for an acknowledgement   |

> Note: 
> 
> Please check the `ESP_BLE_MESH_MODEL_SEND_COMP_EVT` event to see if the message is sent successfully.
> This event is just for sending sig Model and vendor Model messages, not for all Models. 

#### 2.3.2 The Generic Onoff Client sends message

The Generic Onoff Client Model calls the `esp_ble_mesh_generic_client_get_state` API to get the state of the server Model, such as the Onoff state.

```c
esp_ble_mesh_generic_client_get_state_t get_state = {0};
esp_ble_mesh_set_msg_common(&common, node, onoff_client.model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET);
err = esp_ble_mesh_generic_client_get_state(&common, &get_state);
if (err) {
    ESP_LOGE(TAG, "%s: Generic OnOff Get failed", __func__);
    return;
}
```

The Generic Onoff Client Model calls the `esp_ble_mesh_generic_client_set_state` API to set the state of the server Model, such as the Onoff state.

```c
esp_ble_mesh_generic_client_set_state_t set_state = {0};
esp_ble_mesh_set_msg_common(&common, &set_state, onoff_client.model,
                             ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET, remote_onoff);
err = esp_ble_mesh_generic_client_set_state(&common, &set_state);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Generic OnOff Set failed", __func__);
    return;
}
```

#### 2.3.3 The Generic Onoff Server sends message

The Generic Onoff Server Model has to bind its Appkey before calling the `esp_ble_mesh_server_model_send_msg` API to send a message.

```c
err = esp_ble_mesh_server_model_send_msg(model, ctx, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
       sizeof(send_data), &send_data);
```
The Generic Onoff Server Model calls the `esp_ble_mesh_model_publish` API to publish messages. Only the Models that have subscribed to this destination address receive the published messages.

```c
err = esp_ble_mesh_model_publish(model, ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS,
                                 sizeof(led->current), &led->current, ROLE_NODE);
```

### 2.4 Users Enter the Address of the Destination Node via Serial Port

Please connect your devices and enters the address of the destination node via the serial port.

Users can adjust the address of the destination node.


> Note:
> Please connect the pin 16 and pin 17 of the device that runs the `ble_mesh_client_model` project to the USB-to-UART tool.

```c
#define UART1_TX_PIN  GPIO_NUM_16
#define UART1_RX_PIN  GPIO_NUM_17
```

The `board_uart_task` task is used to receive commands sent via the serial port, among which, the`remote_addr` represents the address of destination node that the message is forwarded to. Please enters hexadecimal string, such as 5, for this parameter. The address will be converted to 0x05 automatically. 
	
```c
static void board_uart_task(void *p)
{   
    uint8_t *data = calloc(1, UART_BUF_SIZE);
    uint32_t input;
    
    while (1) { 
        int len = uart_read_bytes(MESH_UART_NUM, data, UART_BUF_SIZE, 100 / portTICK_PERIOD_MS);
        if (len > 0) {
            input = strtoul((const char *)data, NULL, 16);
            remote_addr = input & 0xFFFF;
            ESP_LOGI(TAG, "%s: input 0x%08x, remote_addr 0x%04x", __func__, input, remote_addr);
            memset(data, 0, UART_BUF_SIZE);
        }
    }
    
    vTaskDelete(NULL);
}
```


# 3. Timing Sequence Diagram

The steps for this demo:

1. The nRF Mesh App provisionings the unprovisioned devices into nodes;
2. The nRF Mesh App adds a Appkey to these nodes, and bind the Models of these nodes to this Appkey.
3. The nRF Mesh App sends a controlling message to the Generic Onoff Client Model. Then the Client Model forwards this message to the server Model of the other node. 

The timing sequence diagram of this demo is shown below：

![Packet interaction](images/picture5.png) <div align=center></div>

>Note:
>
>The node **only forwards the message after it receives the controlling message sent by the app**. That is said, the node will **not** forwards messages to the other nodes every time the user enters the address of the destination node through the serial port. 


# 4. The nRF Mesh App

![Packet interaction](images/app.png)

1. Scan the unprovisioned devices.
2. Identity the the capability of the unprovisioned devices.
3. Provisioning the unprovisioned devices.
4. Check if the Mesh node has been configured successful.
5. Configure the Models of the nodes.
6. Click on the Generic On Off Client option.
7. Bind the Generic On Off Client Model to the Appkey.
8. Check if the binding is successfully.
9. Bind the Generic On Off Server Model to the Appkey.
10. Send controlling messages.
