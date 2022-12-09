
# ESP BLE Mesh Node demo

## 1. Introduction

ESP BLE Mesh is built on top of Zephyr BLE Mesh stack. ESP BLE Mesh nodes support:

* Network provisioning
* Node control, including the use of PB\_GATT and PB\_ADV bearers
* Encryption
* Node features of Proxy, Relay, Low power and Friend

This demo has only one element, in which the following two models are implemented:

- **Configuration Server model**: The role of this model is mainly to configure Provisioner device’s AppKey and set up its relay function, TTL size, subscription, etc.
- **Generic OnOff Server model**: This model implements the most basic function of turning the lights on and off.

## 2. Code Analysis

### 2.1 Folder Structure

The folder `ble_mesh_node` contains the following files and subfolders:

```
$ tree examples/bluetooth/esp_ble_mesh/ble_mesh/ble_mesh_node
├── Makefile    /* Compiling parameters for the demo */
├── README.md   /* Quick start guide */
├── build
├── main        /* Stores the `.c` and `.h` application code files for this demo */
├── sdkconfig      /* Current parameters of `idf.py menuconfig` */
├── sdkconfig.defaults   /* Default parameters of `idf.py menuconfig` */
├── sdkconfig.old      /* Previously saved parameters of `idf.py menuconfig` */
└── tutorial         /* More in-depth information about the demo */
```

The contents of the `main` subfolder are as follows:

```
main
├── Kconfig.projbuild
├── ble_mesh_demo_main.c   /* main application codes, more info below */
├── board.c                /* Codes for implementation
├── board.h                   of the RGB LED driver */
└── component.mk
```

- `ble_mesh_demo_main.c`: contains the following main application codes, which are needed to implement the BLE Mesh demo
  - Initialize Bluetooth Controller stack and Host stack (bluedroid)
  - Initialize BLE Mesh stack
  - Register the callback function of BLE Mesh provision and BLE Mesh model
  - Implement and initialize BLE Mesh element
  - Implement and initialize BLE Mesh Configuration Server model and Generic OnOff Server model
  - Function as BLE Mesh Configuration Server Model Get Opcode and BLE Mesh Configuration Server Model Set Opcode
  - Declare and define the RGB LED structure.

### 2.2 Code Analysis of BLE Mesh Node demo

For better understanding of the demo implementation, this section provides a detailed analysis of the codes in the file `ble_mesh_demo_main.c`.

#### 2.2.1 Initializing and Enabling BLE Mesh

When ESP32 system initialization is completed, `app_main` is called. The code block below demonstrates the implementation of the functions in `app_main`.

```c
void app_main(void)
{
    int err;

    ESP_LOGI(TAG, "Initializing...");

    board_init();

    err = bluetooth_init();
    
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    /* Initializes the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
    }
}
```

In particular, the code includes:

- `err = bluetooth_init()`: initialization related to the Bluetooth protocol stack (including Controller and Host)
- `err = ble_mesh_init()`: initialization related to BLE Mesh

Further, the code for initialization of the BLE Mesh protocol stack is introduced, together with the description of the required actions to initialize BLE Mesh.

```c
static esp_err_t ble_mesh_init(void)
{
    int err = 0;

    memcpy(dev_uuid + 2, esp_bt_dev_get_address(), BLE_MESH_ADDR_LEN);

    // See comment 1
     esp_ble_mesh_register_prov_callback(esp_ble_mesh_prov_cb);
    esp_ble_mesh_register_custom_model_callback(esp_ble_mesh_model_cb);

    err = esp_ble_mesh_init(&provision, &composition);
    if (err) {
        ESP_LOGE(TAG, "Initializing mesh failed (err %d)", err);
        return err;
    }

    esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT);

    ESP_LOGI(TAG, "BLE Mesh Node initialized");

    board_led_operation(LED_G, LED_ON);

    return err;
}
```

The code includes the following:

- `esp_ble_mesh_register_prov_callback(esp_ble_mesh_prov_cb)`: registers the provisioning callback function in the BLE Mesh stack. This callback function gets executed during the BLE Mesh network configuration process. It allows the BLE Mesh stack to generate events and notify the application layer about important network configuration processes. This callback function mainly implements the following events:
  - `ESP_BLE_MESH_PROVISION_REG_EVT`: Generated when the BLE Mesh initialization process is completed after calling the API function `esp_ble_mesh_init`. It returns the initialization status of the BLE Mesh application.
  - `ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT`: Generated when a Provisioner and an unprovisioned device establish a link.
  - `ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT`: Generated to notify the application layer that a link has been broken after BLE Mesh bottom-layer protocol sends or receives the message `The Link Broken`.
  - `ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT`: Received by the application layer if during the configuration process `output_actions` is set as `ESP_BLE_MESH_DISPLAY_NUMBER`, and the target peer `input_actions` is set as `ESP_BLE_MESH_ENTER_NUMBER`.
  - `ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT`: Received by the application layer if during the configuration process `output_actions` is set as `ESP_BLE_MESH_DISPLAY_STRING`, and the target peer `input_actions` is set as `ESP_BLE_MESH_ENTER_STRING`.
  - `ESP_BLE_MESH_NODE_PROV_INPUT_EVT`: Received by the application layer if during the configuration process `input_actions` is set as anything but `ESP_BLE_MESH_NO_INPUT`.
  - `ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT`: Received by the application layer when the provisioning is completed.
  - `ESP_BLE_MESH_NODE_PROV_RESET_EVT`: Received by the application layer when the network reset is completed.

- `esp_ble_mesh_register_custom_model_callback(esp_ble_mesh_model_cb)`: registers the model operation callback function. This callback function is used when the target peer operates the model state of the source peer after BLE Mesh has completed network configuration. This callback function mainly implements the following events:
	- `ESP_BLE_MESH_MODEL_OPERATION_EVT`: Can be triggered by the two scenarios below:
		- Server model receives `Get Status` or `Set Status` from Client model.
		- Client model receives `Status state` from Server model.
	- `ESP_BLE_MESH_MODEL_SEND_COMP_EVT`: Generated after the Server model sends `Status state` by calling the API function `esp_ble_mesh_server_model_send_msg`.
	- `ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT`: Generated after the application has completed calling the API `esp_ble_mesh_model_publish_msg` to publish messages
	- `ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT`: Generated when the Client model calls the API function `esp_ble_mesh_client_model_send_msg`, but fails to receive ACK from the target peer due to timeout
	- `ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT`: Generated after the application sets up the publish function to regularly send messages to the target peer.

- `esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT)`: enables the Advertising and Scan functions when the BLE Mesh initialization is completed. It makes the devices visible to Provisioners for network provisioning.
- `board_led_operation(LED_G, LED_ON)`: initializes the RGB LED.

At this point, initialization and enabling of BLE Mesh as a node port is completed, which means a Provisioner can identify devices for network provisioning and data transmission.

#### 2.2.2 Implementation of BLE Mesh Element Structure

The section above shows how to initialize BLE Mesh as a node port. You may still have the following questions:

- What else needs to be done before initialization?
- How to add an element and a model to ESP BLE Mesh stack?
- How to choose a different encryption approach?
- How to declare the features of Proxy, Relay, Low Power and Friend?

This section provides the answers to these questions.

First of all, before calling the API `esp_ble_mesh_init` to initialize BLE Mesh, an element and a model need to be declared and defined.

The code block below shows the declaration of an element structure.

```c

/*!< Abstraction that describes a BLE Mesh Element.
    This structure is associated with bt_mesh_elem in mesh_access.h */
typedef struct {
    /* Element Address, it is assigned during provisioning. */
    uint16_t element_addr;

    /* Location Descriptor (GATT Bluetooth Namespace Descriptors) */
    const uint16_t location;

    /* Model count */
    const uint8_t sig_model_count;
    const uint8_t vnd_model_count;

    /* Models */
    esp_ble_mesh_model_t *sig_models;
    esp_ble_mesh_model_t *vnd_models;
} esp_ble_mesh_elem_t;
```

The next code block shows the definition of an element structure, which only requires to call the macro `ESP_BLE_MESH_ELEMENT`.

```c
static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, ESP_BLE_MESH_MODEL_NONE),
};
```
Another code block provides the codes needed to implement the macro `ESP_BLE_MESH_ELEMENT`.

```c
#define ESP_BLE_MESH_ELEMENT(_loc, _mods, _vnd_mods)    \
{                                                       \
    .location         = (_loc),                         \
    .sig_model_count  = ARRAY_SIZE(_mods),              \
    .sig_models       = (_mods),                        \
    .vnd_model_count  = ARRAY_SIZE(_vnd_mods),          \
    .vnd_models       = (_vnd_mods),                    \
}

```

The variables of the element structure are as follows:

- `addr`: stores the element primary address, used by Mesh Stack during the configuration process. It can be ignored for the higher level applications.
- `loc`: location descriptor defined by SIG. For this demo, set its value to `0`.
- `model_count`: number of SIG models supported in this element.
- `vnd_model_count`: number of the Vendor model supported in this element.
- `models`: pointer to the SIG Models that have already been defined.
- `vnd_models`: pointer to the Vendor Model that has already been defined.

<table><tr><td bgcolor=orange> Note: the SIG Model count and the Vendor Model count work separately. For example, if two SIG Models and one Vendor model are supported in an element, the variables would be model_count = 2, vnd_model_count = 1. </td></tr></table>


If a defined element does not support the Vendor model, the third parameter (the last one) of the macro `ESP_BLE_MESH_ELEMENT` should be set to `ESP_BLE_MESH_MODEL_NODE`. Likewise, if the SIG Model is not supported, the second parameter should be set to `ESP_BLE_MESH_MODEL_NODE`. </td></tr></table>

#### 2.2.3 Implementation of BLE Mesh Model Structure

The preceding section has introduced the specific ways to implement and define an element by passing specific model pointers to it. This section explains how to implement and define a Model structure, which is shown in the code blocks below.

```c
/** Abstraction that describes a Mesh Model instance.
 *  This structure is associated with bt_mesh_model in mesh_access.h
 */
struct esp_ble_mesh_model {
    /* Model ID */
    union {
        const uint16_t model_id;
        struct {
            uint16_t company_id;
            uint16_t model_id;
        } vnd;
    };

    /* The Element to which this Model belongs */
    esp_ble_mesh_elem_t *element;

    /* Model Publication */
    esp_ble_mesh_model_pub_t *const pub;

    /* AppKey List */
    uint16_t keys[CONFIG_BLE_MESH_MODEL_KEY_COUNT];

    /* Subscription List (group or virtual addresses) */
    uint16_t groups[CONFIG_BLE_MESH_MODEL_GROUP_COUNT];

    /* Model operation context */
    esp_ble_mesh_model_op_t *op;

    /* Model-specific user data */
    void *user_data;
};
```

The block above shows a specific implementation of the model structure. Although this structure has many variables, only the following four ones are used for applications:

- `id` and `vnd`: union variables, defining the SIG Model and the Vendor Model respectively.
- `op`: structure with a set of variables for the Model Operation, declaring the opcode that corresponds to Get, Set, or Status State, as well as the minimum value lengths that are supported in this module.
- `pub`: structure that needs to be defined if the Model structure supports the Publish function.
- `user_data`: optional variable for storing the application layer data.

The other structures and variables (keys, group, element) get their values through the BLE Mesh stack during the initialization or configuration stages. You are not required to initialize them.

The next code block presents the definition of the model structure, and the `root_models[]` array. This array is used for indicating the number of the existing model structures. A model is implemented by using a macro.

```c

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV, onoff_op,
    &onoff_pub, &led_state[0]),
};
```

Different models require different macros. The existing types of models and their respective macros needed for implementation are given in the table below.

|  | Model Name | Macro Required for its Definition |
| --------------- | ---- | ----------------------------- |
| **SIG Models Implemented in ESP32 BLE Mesh Stack** | Configuration Server Model | `ESP_BLE_MESH_MODEL_CFG_SRV` |
| | Configuration Client Model | `ESP_BLE_MESH_MODEL_CFG_CLI` |
| | Generic OnOff Client Model | `ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI` |
| | Generic Level Client Model | `ESP_BLE_MESH_MODEL_GEN_LEVEL_CLI` |
| | Generic Default Transition Time Client Model | `ESP_BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI` |
| | Generic Power OnOff Client Model | `ESP_BLE_MESH_MODEL_GEN_POWER_ONOFF_CLI` |
| | Generic Power Level Client Model | `ESP_BLE_MESH_MODEL_GEN_POWER_LEVEL_CLI` |
| | Generic Battery Client Model | `ESP_BLE_MESH_MODEL_GEN_BATTERY_CLI` |
| | Generic Location Client Model | `ESP_BLE_MESH_MODEL_GEN_LOCATION_CLI` |
| | Generic Property Client Model | `ESP_BLE_MESH_MODEL_GEN_PROPERTY_CLI` |
| | Light Lightness Client Model | `ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI` |
| | Light CTL Client Model | `ESP_BLE_MESH_MODEL_LIGHT_CTL_CLI` |
| | Light HSL Client Model | `ESP_BLE_MESH_MODEL_LIGHT_HSL_CLI` |
| | Sensor Client Model | `ESP_BLE_MESH_MODEL_SENSOR_CLI` |
| | Scene Client Model | `ESP_BLE_MESH_MODEL_SCENE_CLI` |
| **SIG Models Not Implemented in ESP32 BLE Mesh Stack** | - | `ESP_BLE_MESH_SIG_MODEL` |
| **Vendor Models** | - | `ESP_BLE_MESH_VENDOR_MODEL` |

Another important structure in a model is `esp_ble_mesh_model_op_t *op` pointers. These structures point to the operation structure that defines the Model state. Generally, there are two types of models in BLE Mesh:

- Server Model:
	- Consists of one or multiple states that can exist across different elements
	- Defines the messages sent/received by the model, along with the element's behavior.
		- Example：On/Off switch --- Indicates the On/Off status.
- Client Model:
	- Defines the messages used by the client to request, change or use the relevant state of the server.
		- Example：On/Off switch --- Indicates the On or Off message sent by the Client.

Operation structure defines the state value supported by a model. A specific operation structure is given below.

The following code block shows the declaration of the Model operation structure.

```c
/*!< Model operation context.
    This structure is associated with bt_mesh_model_op in mesh_access.h */
typedef struct {
    const uint32_t    opcode;   /* Opcode encoded with the ESP_BLE_MESH_MODEL_OP_* macro */
    const size_t      min_len;  /* Minimum required message length */
    esp_ble_mesh_cb_t param_cb; /* The callback is only used for the BLE Mesh stack, not for the app layer. */
} esp_ble_mesh_model_op_t;
```

There are three variables in the declaration of the operation structure:

- `opcode`: opcode corresponding to a state. As specified in BLE Mesh, the SIG Model opcode should be 1~2 bytes, and the Vendor Model opcode should be 3 bytes.
- `min_len`: min length of the messages received by the state. For example, OnOff Get state is 0 bytes, and OnOff Set State is 2 bytes.
- `param_cb`: used for the BLE Mesh protocol only. Applications need to set its value to `0`.

The block below shows the operation structure array defined by the OnOff Server in this demo.

```c
static esp_ble_mesh_model_op_t onoff_op[] = {
    { ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET, 0, 0},
    { ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET, 2, 0},
    { ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, 2, 0},
    /* Each model operation struct array must use this terminator
     * as the end tag of the operation uint. */
    ESP_BLE_MESH_MODEL_OP_END,
};
```

It presents the opcodes, corresponding to the three states, defined in this demo: OnOff Get, OnOff Set, and OnOff Set Unack.

<table><tr><td bgcolor=orange> Note: for the Server Model, the corresponding request state of the Client Model needs to be written in the operation definition. For example, if the Generic OnOff Server Model is to be implemented in this demo, the following three states requested by the Generic OnOff Client Model need to be written in the operation definition: OnOff Get, OnOff Set, and OnOff Set Unack. Likewise, for the Client Model, the corresponding State of the messages received by the Server needs to be written in the operation definition. </td></tr></table>

#### 2.2.4. Encryption and Authentication of BLE Mesh

In the project developing process, different security levels are required. BLE Mesh offers various kinds of encryption and authentication features which can be split into two categories - Input and Output.

The classification of the Input features is given in the table below. 

| Feature Name | Action Supported |
| ------------ | ------------------ |
| `ESP_BLE_MESH_NO_INPUT` | Input is not supported by device's IO |
| `ESP_BLE_MESH_PUSH`  | PUSH |
| `ESP_BLE_MESH_TWIST` | TWIST |
| `ESP_BLE_MESH_ENTER_NUMBER` | ENTER NUMBER |
| `ESP_BLE_MESH_ENTER_STRING` | ENTER STRING |

The classification of the Output features is given in the table below.

| Feature Name | Action Supported |
| ------------ | ------------------ |
| `ESP_BLE_MESH_NO_OUTPUT ` | Output is not supported by device's IO |
| `ESP_BLE_MESH_BLINK `  | BLINK |
| `ESP_BLE_MESH_BEEP ` | BEEP |
| `ESP_BLE_MESH_VIBRATE ` | VIBRATE |
| `ESP_BLE_MESH_DISPLAY_NUMBER ` | DISPLAY NUMBER |
| `ESP_BLE_MESH_DISPLAY_STRING ` | DISPLAY STRING |

The above Input and Output categories in the declaration of the structure `esp_ble_mesh_prov_t` can be defined by the following four variables:

- `output_size`
- `output_actions`
- `input_actions`
- `input_size`

These variables should be set to `0` for this demo, as it uses the most basic authentication features.

## 3. Configuration of BLE Mesh Menuconfig

To be functional across different applications, the BLE Mesh menuconfig is specifically designed to offer a variety of configuration options, which can be helpful in tailoring your own configuration.

The list of configuration options in BLE Mesh menuconfig is stored in `Component config`  ---> `[]Bluetooth Mesh support` and can be accessed with the command `idf.py menuconfig`. This configuration option list is shown below.

```
—— Bluetooth Mesh support
[*]   Support for BLE Mesh Node
[ ]   Support for BLE Mesh Provisioner
[*]   Provisioning support using the advertising bearer (PB-ADV)
[*]   net buffer pool usage
[*]   Provisioning support using GATT (PB-GATT)
[*]   GATT Proxy Service
(60)    Node Identity advertising timeout
(1)   Maximum number of filter entries per Proxy Client
[*]   Perform self-tests
[*]   Test the IV Update Procedure
(1)   Maximum number of mesh subnets per network
(1)   Maximum number of application keys per network
(1)   Maximum number of application keys per model
(1)   Maximum number of group address subscriptions per model
(1)   Maximum number of Label UUIDs used for virtual Addresses
(10)  Maximum capacity of the replay protection list
(10)  Network message cache size
(20)  Number of advertising buffers
(6)   Maximum number of simultaneous outgoing segmented messages
(1)   Maximum number of simultaneous incoming segmented messages
(384) Maximum incoming Upper Transport Access PDU length
[*]   Relay support
[ ]   Support for Low Power features
[ ]   Support for acting as a Friend Node
[*]   Support for Configuration Client Model
[*]   Support for Health Client Model
[ ]   Support for Generic OnOff Client Model
[ ]   Support for Generic Level Client Model
[ ]   Support for Generic Default Transition Time Client Model
[ ]   Support for Generic Power OnOff Client Model
[ ]   Support for Generic Power Level Client Model
[ ]   Support for Generic Battery Client Model
[ ]   Support for Generic Location Client Model
[ ]   Support for Generic Property Client Model
[ ]   Support for Sensor Client Model
[ ]   Support for Scene Client Model
[ ]   Support for Light Lightness Client Model
[ ]   Support for Light CTL Client Model
[ ]   Support for Light HSL Client Model
[ ]   Enable Bluetooth Mesh shell
```

The detailed information about the roles and functions of these options is presented below.

- **Support for BLE Mesh Node**: Indicates if the role of a node is supported. There are two roles in BLE Mesh: a Provisioner and a node. In this demo only a node is supported.
- **Support for BLE Mesh Provisioner**: Indicates if the role of a Provisioner is supported.
- **Provisioning support using the advertising bearer (PB-ADV)**: Indicates if the bearer PB-ADV is supported for network provisioning. In BLE Mesh，the bearers PB-ADV and PB-GATT are supported for network provisioning. This demo supports both of them.
- **net buffer pool usage**: When enabled, BLE Mesh monitors the usage of the Adv buffer.
- **Provisioning support using GATT (PB-GATT)**: Indicates if the PB-GATT bearer is supported for network provisioning.
- **GATT Proxy Service**: Indicates if the GATT proxy service is supported.
	- **Node Identity advertising timeout**: Indicates the time (in seconds) after which advertising stops. This value gets assigned by BLE Mesh protocol stack when using the GATT proxy service.
- **Maximum number of filter entries per Proxy Client**: Used to configure the maximum number of filtered addresses. To reduce the number of Network PDUs exchanges between a Proxy Client and a Proxy Server, a proxy filter can be used. The output filter of the network interface instantiated by the Proxy Server can be configured by the Proxy Client. This allows the Proxy Client to explicitly request to receive only mesh messages with certain destination addresses. For example, a Proxy Client that is subscribed to a group address may want to only receive packets addressed to the unicast address of one of its elements and to that group address. Thus, the Proxy Client has full control over the packets it receives using the Proxy protocol.
- **Perform self-tests**:
- **Test the IV Update Procedure**:
- **Maximum number of mesh subnets per network**: Indicates the maximum number of the subnets supported in a BLE Mesh network.
- **Maximum number of application keys per network**: Indicates the maximum number of AppKeys supported in a BLE Mesh network.
- **Maximum number of application keys per model**: Indicates the maximum number of AppKeys bound in each Model.
- **Maximum number of group address subscriptions per model**: Indicates the maximum number of group address subscriptions supported in each model in BLE Mesh.
- **Maximum number of Label UUIDs used for Virtual Addresses**: Indicates the maximum number of Label UUIDs supported in BLE Mesh.
- **Maximum capacity of the replay protection list**: Indicates what the name suggests.
- **Network message cache size**: Configures the size of the cache, which is used to store the forwarded messages to avoid multiple forwarding in BLE Mesh.
- **Number of advertising buffers**: Indicates what the name suggests.
- **Maximum number of simultaneous outgoing segmented messages**: Indicates what the name suggests.
- **Maximum number of simultaneous incoming segmented messages**: Indicates what the name suggests.
- **Maximum incoming Upper Transport Access PDU length**: Indicates that the access layer can receive the maximum length of a complete packet.
- **Relay support**: Indicates if the Relay feature is supported.
- **Support for Low Power features**: Indicates if the Low Power features are supported.
- **Support for acting as a Friend Node**: Indicates if the Friend feature is supported.
- **Support for Configuration Client Model**: Indicates if the Configuration Client model is supported.
- **Support for Health Client Model**: Indicates if the given model is supported.
- **Support for Generic OnOff Client Model**: Indicates if the given model is supported.
- **Support for Generic Level Client Model**: Indicates if the given model is supported.
- **Support for Generic Default Transition Time Client Model**: Indicates if the given model is supported.
- **Support for Generic Power Onoff Client Model**: Indicates if the given model is supported.
- **Support for Generic Power Level Client Model**: Indicates if the given model is supported.
- **Support for Generic Battery Client Model**: Indicates if the given model is supported.
- **Support for Generic Location Client Model**: Indicates if the given model is supported.
- **Support for Generic Property Client Model**: Indicates if the given model is supported.
- **Support for Sensor Client Model**: Indicates if the given model is supported.
- **Support for Scene Client Model**: Indicates if the given model is supported.
- **Support for Light Lightness Client Model**: Indicates if the given model is supported.
- **Support for Light CTL Client Model**: Indicates if the given model is supported.
- **Support for Light HSL Client Model**: Indicates if the given model is supported.
- **Enable Bluetooth Mesh shell**:
