# 1. Introduction
## 1.1 Demo Function

This demo is used for fast provisioning networks. It takes no more than 60 seconds to provisioning 100 devices in this demo. 

This demo must be used with the EspBleMesh app. For details about how to use the EspBleMesh app, please click [here](EspBleMesh.md).

## 1.2 Node Composition

This demo has only one element, where the following five Models are implemented:

- The **Configuration Server** Model is used to represent a mesh network configuration of a device.
- The **Configuration Client** Model is used to represent an element that can control and monitor the configuration of a node.
- The **Generic OnOff Server** Model implements the node's Onoff state.
- The **Vendor Server** Model implements the node's `fast_prov_server` state.
- The **Vendor Client** Model is used to control the `fast_prov_server` state, which defines the fast provisioning behavior of a node.


## 2. Code Analysis

Code initialization part reference [Initializing Bluetooth and BLE Mesh](../../../wifi_coexist/tutorial/BLE_Mesh_WiFi_Coexist_Example_Walkthrough.md).

### 2.1  Data Structure

This section introduces the `example_fast_prov_server_t` strut for this demo, and its variables in groups.

```
typedef struct {
    esp_ble_mesh_model_t *model;    /* Fast Prov Server Model pointer */
    ATOMIC_DEFINE(srv_flags, SRV_MAX_FLAGS);

    bool     primary_role;  /* Indicate if the device is a Primary Provisioner */
    uint8_t  max_node_num;  /* The maximum number of devices can be provisioned by the Provisioner */
    uint8_t  prov_node_cnt; /* Number of self-provisioned nodes */
    uint16_t app_idx;       /* AppKey index of the application key added by other Provisioner */
    uint16_t top_address;   /* Address of the device(e.g. phone) which triggers fast provisioning */

    esp_ble_mesh_msg_ctx_t ctx; /* the context stored for sending fast prov status message */
    struct fast_prov_info_set *set_info;    /* Used to store received fast prov info set context */

    uint16_t node_addr_cnt;     /* Number of node address shall be received */
    uint16_t unicast_min;       /* Minimum unicast address can be send to other nodes */
    uint16_t unicast_max;       /* Maximum unicast address can be send to other nodes */
    uint16_t unicast_cur;       /* Current unicast address can be assigned */
    uint16_t unicast_step;      /* Unicast address change step */
    uint8_t  flags;             /* Flags state */
    uint32_t iv_index;          /* Iv_index state */
    uint16_t net_idx;           /* Netkey index state */
    uint16_t group_addr;        /* Subscribed group address */
    uint16_t prim_prov_addr;    /* Unicast address of Primary Provisioner */
    uint8_t  match_val[16];     /* Match value to be compared with unprovisioned device UUID */
    uint8_t  match_len;         /* Length of match value to be compared */

    uint8_t  pend_act;          /* Pending action to be performed */
    uint8_t  state;             /* Fast prov state -> 0: idle, 1: pend, 2: active */

    struct k_delayed_work disable_fast_prov_timer;  /* Used to disable fast provisioning */
    struct k_delayed_work gatt_proxy_enable_timer;  /* Used to Mesh GATT Proxy functionality */
} __attribute__((packed)) example_fast_prov_server_t;
```


#### 2.1.1 Provisioner Role and State

Different provisioners have different behaviors and it’s helpful to understand the concepts of different roles so you can better understand the codes.

In the struct, there are three variables that are related to roles and states, which are described in the following table:

| Variable Name        |Description               |
| ---------------------|------------------------- |
| `primary_role`      | Provisioner identity |
| `state`      | Fast provisioner state (0: idle, 1: pend, 2: active) |
| `srv_flags`  | Flags (`DISABLE_FAST_PROV_START`,`GATT_PROXY_ENABLE_START`,`RELAY_PROXY_DISABLED`,`SRV_MAX_FLAGS`) |

Among which, there are four roles in this demo (`primary_role`):

* Phone - Top Provisioner
* The device that has been provisioned by Phone - Primary Provisioner
* Devices that have been provisioned and changed to the role of a provisioner - Temporary Provisioner
* Devices that have been provisioned but not changed to the role of a provisioner - Node


#### 2.1.2 Provisioner Address Management

The provisioner address management is used to assign a unicast address to each node, so as to prevent address conflicts by allocating address in an equally manner. Each provisioner has its own address range and a maximum number of the nodes it can provisioned. The provisioner will allocate a subset of its address range to the nodes it has provisioned.

Example: A top provisioner's address range is 0 to 100 and the maximum number of nodes it can provisioned is 5. The provisioner address management will assign subsets of address range to these 5 nodes, which are 1 to 20, 21 to 40, 41 to 60, 61 to 80 and 81 to 100.

The variables that are related to the address management are described in the following table:

| Variable Name        |Description               |
| ----------------------|------------------------- |
| `unicast_min`      | Minimum unicast address can be allocated to other nodes |
| `unicast_max`      | Maximum unicast address can be allocated to other nodes |
| `unicast_cur`      | Current unicast address |
| `unicast_step`     | Unicast address change step Offset|

#### 2.1.3 Provisioner Cache Data

The cache data is required, so a node can change its role to become a provisioner. During this process, the `esp_ble_mesh_set_fast_prov_info` and `esp_ble_mesh_set_fast_prov_action` APIs are called.

The node's cache data, which are described in the following table, is sent by the provisioner. 

| Variable Name        |Description               |
| ----------------------|------------------------- |
| `flags`       |Flags state|
| `iv_index`    |Iv_index state|
| `net_idx`     |Netkey index state |
| `group_addr`  |Subscribed group address |
| `prim_prov_addr`  |Unicast address of Primary Provisioner |
| `match_val[16]`  |Match value to be compared with unprovisioned device UUID |
| `match_len`   | Length of match value to be compared |
| `max_node_num`   | The maximum number of devices can be provisioned by the Provisioner |
| `prov_node_cnt`   | Number of self-provisioned nodes |
| `app_idx`   |  AppKey index of the application key added by other Provisioner |
| `top_address`   | Address of the device(e.g. phone) which triggers fast provisioning |


#### 2.1.4 Provisioner Timer

There are two timers in this demo, which are:

1. `gatt_proxy_enable_timer` is used to enable Mesh GATT Proxy functionality.
	* The timer starts or resets and starts when a Temporary Provisioner provisions an unprovisioned device.
	* The Temporary Provisioner will send a message (Address information) to the Primary Provisioner.
2. `disable_fast_prov_timer` is used to disable the provisioning capabilities.
	* The node starts the timer when it receives a **Generic OnOff Get/Set/Set Unack** message sent by the EspBleMesh app. The group address should be used if you want to disable the provisioning capabilities of all nodes. 

The variables that are related to these two timers are described below:

| Variable Name        |Description               |
| ----------------------|------------------------- |
| `disable_fast_prov_timer`       |Used to disable fast provisioning|
| `gatt_proxy_enable_timer`       |Used to enable Mesh GATT Proxy functionality|

### 2.2  Model Definition

#### 2.2.1 Vendor Server Model

The **Vendor Server** Model implements the node's `fast_prov_server` state, which has been covered in the previous section. 

```c
example_fast_prov_server_t fast_prov_server = {
    .primary_role  = false,
    .max_node_num  = 6,
    .prov_node_cnt = 0x0,
    .unicast_min   = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .unicast_max   = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .unicast_cur   = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .unicast_step  = 0x0,
    .flags         = 0x0,
    .iv_index      = 0x0,
    .net_idx       = ESP_BLE_MESH_KEY_UNUSED,
    .app_idx       = ESP_BLE_MESH_KEY_UNUSED,
    .group_addr    = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .prim_prov_addr = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .match_len     = 0x0,
    .pend_act      = FAST_PROV_ACT_NONE,
    .state         = STATE_IDLE,
};
```

The `fast_prov_srv_op` is used to register the minimum length of messages. For example, the minimum length of the `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET` message is registered as 3 octets. 

```c
static esp_ble_mesh_model_op_t fast_prov_srv_op[] = {
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,      3,  NULL },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD,   16, NULL },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR,     2,  NULL },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET, 0,  NULL },
    ESP_BLE_MESH_MODEL_OP_END,
};

```
The `example_fast_prov_server_init` function is used to register the callback function triggered when the timers timeout, and initializes the Model-related variables in the data struct.

```c
err = example_fast_prov_server_init(&vnd_models[0]);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Failed to initialize fast prov server model", __func__);
    return err;  
}
```

The `fast_prov_server` struct represents the Vendor server's states. The `CID_ESP` and `ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_SRV` constants, which consist of the vendor server Model's Model id `ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_SRV`, are used to identity the Vendor server Model. 


```c
static esp_ble_mesh_model_t vnd_models[] = {
    ESP_BLE_MESH_VENDOR_MODEL(CID_ESP, ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_SRV,
    fast_prov_srv_op, NULL, &fast_prov_server),
};
static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, vnd_models),
};
```


#### 2.2.2 Vendor Client Model

The **Vendor Client** Model is used to control the `fast_prov_server` state, which defines the fast provisioning behavior of a node.

The `fast_prov_cli_op_pair` struct is used to register the corresponding message acknowledgements.

```c
static const esp_ble_mesh_client_op_pair_t fast_prov_cli_op_pair[] = {
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,      ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS      },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD,   ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS   },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR,     ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK    },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET, ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS },
};
```

Example: The **Vendor Client** Model sends message with an opcode of `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET`, which requires the **Vendor Server** Model to respond with a message with an opcode of `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS`. After that, the **Vendor Client** Model times out if it receives no corresponding acknowledgement.

```c
static const esp_ble_mesh_client_op_pair_t fast_prov_cli_op_pair[] = {
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,      ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS      },
};
```
Note that you can also use the code below if you don't want the **Vendor Client** Model to wait for acknowledgement from the server Model, which means the client Model will never time out.

```c
static const esp_ble_mesh_client_op_pair_t fast_prov_cli_op_pair[] = {
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,      NULL      },
};
```

The `esp_ble_mesh_client_model_init` API is used to register the callback function triggered when the timers timeout, and initializes the Model-related variables in the data struct.

```c
err = esp_ble_mesh_client_model_init(&vnd_models[1]);
if (err != ESP_OK) {
    ESP_LOGE(TAG, "%s: Failed to initialize fast prov client Model", __func__);
    return err;
}
```

The `CID_ESP` and `ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI` constants, which consist of the vendor client Model's Model id `ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI`, are used to identity the Vendor client Model. 

```c

esp_ble_mesh_client_t fast_prov_client = {
    .op_pair_size = ARRAY_SIZE(fast_prov_cli_op_pair),
    .op_pair = fast_prov_cli_op_pair,
};

static esp_ble_mesh_model_op_t fast_prov_cli_op[] = {
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS,    1, NULL },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS, 2, NULL },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK,  0, NULL },
    ESP_BLE_MESH_MODEL_OP_END,
};

static esp_ble_mesh_model_t vnd_models[] = {
    ESP_BLE_MESH_VENDOR_MODEL(CID_ESP, ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI,
    fast_prov_cli_op, NULL, &fast_prov_client),
};
static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, vnd_models),
};

```

## 2.3 Message Opcode

"Opcode-send" represents the message that the client sends to the server.

"Opcode-ack" represents the message that the server sends to the client.

* INFO_SET

|Meaning | Opcode-send   | Opcode-ack   |                  
| -----| ------------- | -------------|
|Opcode| `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET` | `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS`    |
|Function| This message contains all the information as a Provisioner |Checks each field of the Provisioner information and set the corresponding flag bit. The returned status is variable.|
|Parameter|structfast_prov_info_set|status_bit_mask, status_ctx_flag, status_unicast, status_net_idx, status_group, status_pri_prov, status_match, status_action|


* NODE_ADDR

|Meaning | Opcode-send   | Opcode-ack   |                  
| -----| ------------- | -------------|
|Opcode| `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR`  | `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK`  |
|Function| Temporary Provisioner reports the address of the node it has provisioned. |Used to check if the message was sent successfully. |
|Parameter| Address array    |NA   |

* ADDR_GET

|Meaning | Opcode-send   | Opcode-ack   |                  
| -----| ------------- | -------------|
|Opcode| `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET` | `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS`  |
|Function|Top Provisioner gets the address of all nodes obtained from Primary Provisioner. | Returns the address of all nodes, but does not contain its own.     |
|Parameter|NA    |Address array    |

* NET_KEY_ADD

|Meaning | Opcode-send   | Opcode-ack   |                  
| -----| ------------- | -------------|
|Opcode | `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD`   | `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS`    |
|Function| Reserved for later use   | Reserved for later use     |
|Parameter| NA   | NA     |


### 2.4 Callback Function
#### 2.4.1 The Callback function for the Vendor Server Model

```c
    esp_ble_mesh_register_custom_model_callback(example_ble_mesh_custom_model_cb);
    esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb);
```

1. The callback function will be triggered when the **Vendor Server** Model: 
	* Receives a message that indicates the Onoff state of the client Model; or
	* Calls any APIs that send messages.

2. The events that this callback function handle:

* Generic Onoff Server Model

| Event Name    | Opcode      |Description                                 |
| ------------- | ------------|------------------------------------------- |
| ESP_BLE_MESH_MODEL_OPERATION_EVT|ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET | This event is triggered when the **Generic Onoff Server** model receives the `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET` message |
| ESP_BLE_MESH_MODEL_OPERATION_EVT|ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK| This event is triggered when the **Generic Onoff Server** model receives the  `ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK` message. |

* Vendor Server Model

| Event Name    | Opcode      |Description                                 |
| ------------- | ------------|------------------------------------------- |
| ESP_BLE_MESH_MODEL_OPERATION_EVT | ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET  | This event is triggered when the **Vendor Server** model receives the `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET` message.|
| ESP_BLE_MESH_MODEL_OPERATION_EVT | ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR    | This event is triggered when the **Vendor Server** model receives the `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR` message.|
| ESP_BLE_MESH_MODEL_OPERATION_EVT | ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET    | This event is triggered when the **Vendor Server** model receives the `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET` message.|

* The **Configuration Client** Model

| Event Name    | Opcode      |Description                                 |
| ------------- | ------------|------------------------------------------- |
|ESP_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT| NA| This event is triggered when the `esp_ble_mesh_set_fast_prov_info` API is called.  |
|ESP_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT| NA| This event is triggered when the `esp_ble_mesh_set_fast_prov_action` API is called. |
|ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT|ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD|This event is triggered when the **Configuration Server** model receives and further triggers an API calling to send `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET` message. |
|ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT|ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD|This event is triggered when the API `example_send_config_appkey_add` times out.|

#### 2.4.2 The Vendor Client Model

```c
    esp_ble_mesh_register_custom_model_callback(example_ble_mesh_custom_model_cb);
```

1. The callback function will be triggered when the **Vendor Client** model: 
	* Receives any message sent by the vendor server Model; or
	* Calls any APIs that send messages.

2. The events that this callback function handle:

| Event Name    | Opcode      |Description                                 |
| ------------- | ------------|------------------------------------------- |
| ESP_BLE_MESH_MODEL_OPERATION_EVT   | ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS  | This event is triggered when the **Vendor Client** model receives the `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS` message.|
| ESP_BLE_MESH_MODEL_OPERATION_EVT   | ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS | This event is triggered when the **Vendor Client** model receives the `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS` message.|
| ESP_BLE_MESH_MODEL_OPERATION_EVT   | ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK  | This event is triggered when the **Vendor Client** model receives the `ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK` message |
| ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT     | client_send_timeout.opcode    | This event is triggered when the API `esp_ble_mesh_client_model_send_msg` times out.|

### 2.5 Message Sending
#### 2.5.1 The Vendor Client sends messages

The Vendor Client Model calls the `esp_ble_mesh_client_model_send_msg` API to send messages to the Vendor Server Model.

| Parameter Name        |Description               |
| ----------------------|------------------------- |
| `model`       | The pointer to the client Model struct  |
| `ctx.net_idx` | The NetKey Index of the subnet through which the message is sent |
| `ctx.app_idx` | The AppKey Index for the message encryption |
| `ctx.addr`    | The address of the destination nodes |
| `ctx.send_ttl`| The TTL State, which determines how many times a message can be relayed|
| `opcode`      | The message opcode  |
| `msg->len`    | The length of the `msg->data`|
| `msg->data`   | The pointer to sent data|
| `msg_timeout` | The maximum duration (4000 ms by default) that the Model waits for an acknowledgment.  |
|`true`         | True: an acknowledgement is required; False: no acknowledgement is required |

```c
esp_ble_mesh_msg_ctx_t ctx = {
    .net_idx  = info->net_idx,
    .app_idx  = info->app_idx,
    .addr     = info->dst, 
    .send_ttl = 0,
 };
 err = esp_ble_mesh_client_model_send_msg(model, &ctx,
        ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,
        msg->len, msg->data, info->timeout, true, info->role);
```

#### 2.5.2 The Vendor Server sends messages

The **Vendor Server** Model has to bind its Appkey before calling the `esp_ble_mesh_server_model_send_msg` API to send a message.

```c
esp_ble_mesh_server_model_send_msg(model, ctx, ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS,
                                   msg->len ,msg->data );
```
The **Vendor Server** Model calls the `esp_ble_mesh_model_publish` API to publish messages. Only the Models that have subscribed to this destination address receive the published messages.

```c
esp_err_t esp_ble_mesh_model_publish(esp_ble_mesh_model_t *model, uint32_t opcode,
                                     uint16_t length, uint8_t *data,
                                     esp_ble_mesh_dev_role_t device_role);
```
