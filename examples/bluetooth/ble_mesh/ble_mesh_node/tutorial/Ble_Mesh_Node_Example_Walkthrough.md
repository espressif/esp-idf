
# ESP-IDF BLE Mesh Node demo

## 1. Introduction

ESP32 BLE-Mesh is built upon Zephyr Mesh lib. It supports network provisioning and node control in BLE Mesh, including the use of PB\_GATT and PB\_ADV bearers, encryption, as well as the node features of Proxy, Relay, Low power and Friend. This demo has only one element, where the following two models are implemented:

- Configuration Server Model: The role of this model is mainly to configure the App key and set the relay function, set the TTL size, set subscription, etc. for the provisioner device.
- Generic OnOff Server Model: This model realizes the most basic function of turning on and off the light.

## 2. Code Analysis

### 2.1 Code File Structure

<center>

![](https://i.imgur.com/oYe8tR9.png)

**Figure 1**

![](https://i.imgur.com/eDH2lPY.png)

**Figure 2**

</center>

As presented in **Figure 1**, under the **ble\_mesh\_node** folder, are the following files and subfolders:

- **main**: stores all the `.c` and `.h` application codes for this demo;
- **Makefile**: serves as the compiling option to configure the demo;
- **README.md**: introduces the demo;  
- **sdkconfig.defaults**: configures the default parameters of `make menuconfig`.

Under the **main** subfolder in **Figure 1**, are the following files as demonstrated in **Figure 2**:

- `ble_mesh_demo_main.c`: serves as the main application codes to implement the BLE-Mesh demo, including:
  - Initialize Bluetooth Controller Stack and Host Stack (bluedroid);  
  - Initialize BLE Mesh Stack;
  - Register the callback function of BLE Mesh provision and BLE Mesh model;
  - Implement and initialize BLE Mesh Element;
  - Implement and initialize **BLE Mesh Configuration Server Model** and **Generic OnOff Server Model**;
  - Function as **BLE Mesh Configuration Server Model Get Opcode** and **BLE Mesh Configuration Server Model Set Opcode**;
  - Declare and define the RGB LED structure. 
- `board.c` and `board.h`: serve as the codes to implement the RGB LED driver.

### 2.2 Code Analysis of BLE Mesh Node demo

To help users better understand how to implement the demo, this section provides a detailed analysis of the codes in the file `ble_mesh_demo_main.c`.

#### 2.2.1 Initialization and Enabling of BLE Mesh 

<center>

![](https://i.imgur.com/xqDvf88.png)

**Figure 3**

</center>

**Figure 3** demonstrates the implementation of the related codes after `app_main` is called. When ESP32 system initialization is completed, it will call `app_main` that implements the following two functions:

- Line 343 - the initialization related to Bluetooth protocol stack (including Controller and Host)
- Line 352 - the initialization related to BLE Mesh 

The next part will introduce the initialization function of BLE Mesh protocol stack. Let's see what actions are required to initialize BLE Mesh.

<center>

![](https://i.imgur.com/Xw3fpAQ.png)

**Figure 4**

</center>
**Figure 4** displays the codes to implement the initialization of BLE Mesh protocol stack, with main focus on the functions below:

- Line 276 - registers the provisioning callback function to BLE Mesh Stack. This callback function executes during the BLE Mesh network configuration process, which allows the BLE Mesh Stack to notify the application layer about the important network configuration processes in the form of events. This callback function mainly implements the following events:
 - `ESP_BLE_MESH_PROVISION_REG_EVT`: The event returned after the BLE Mesh initialization is completed by calling `esp_ble_mesh_init` API. This is to notify whether the BLE Mesh application has been successfully initialized. 
 - `ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT`: The event returned when provisioner and unprovisioned device establish a link.
 - `ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT`: The event to notify the application layer that the link has been closed when BLE Mesh bottom-layer protocol sends or receives the message of **The Link Close**.
 - `ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT`: The event received by the application layer when `output_actions` is set as `ESP_BLE_MESH_DISPLAY_NUMBER`, and the target peer sets `input_actions` as `ESP_BLE_MESH_ENTER_NUMBER` during the configuration process.
 - `ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT`: The event received by the application layer when `output_actions` is set as `ESP_BLE_MESH_DISPLAY_STRING`, and the target peer sets  `input_actions` as `ESP_BLE_MESH_ENTER_STRING` during the configuration process.
 - `ESP_BLE_MESH_NODE_PROV_INPUT_EVT`: The event received by the application layer when `input_actions` is set as anything except `ESP_BLE_MESH_NO_INPUT` during the configuration process.
 - `ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT`: The event received by the application layer when the provisioning is completed.
 - `ESP_BLE_MESH_NODE_PROV_RESET_EVT`: The event received by the application layer when the network reset is done.

- Line 278 - registers the model operation callback function. This callback function is used when the target peer operates the model state of the source peer after the BLE Mesh has completed the network configuration. This callback function mainly implements the following events:
	- `ESP_BLE_MESH_MODEL_OPERATION_EVT`: The event triggered by the two scenarios below:
		- **Server Model** receives **Get Status** or **Set Status**  from **Client Model**; 
		- **Client Model** receives **Status State** from **Server Model**.
	- `ESP_BLE_MESH_MODEL_SEND_COMP_EVT`: The event returned after the Server Model sends **Status State** by calling `esp_ble_mesh_server_model_send_msg` API.
	- `ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT`: The event returned after the application has completed calling `esp_ble_mesh_model_publish_msg` API to publish messages.
	- `ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT`: The event returned when the Client Model calls `esp_ble_mesh_client_model_send_msg` API, but fails to receive ACK from the target peer due to timeout.
	- `ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT`: The event returned after the application sets the publish function to regularly publish messages to the target peer.
	
- Line 287 - enables the **Advertising** and **Scan** functions when the BLE Mesh initialization is completed, in order to make the devices visible to provisioners for network provisioning. 
- Line 291 - initializes the RGB LED.

At this point, the initialization and the enabling of the BLE Mesh as a Node port have completed, which means provisioner can identify the devices for network provisioning and data transmission.

#### 2.2.2 Implementation of BLE Mesh Element Structure

The above section has introduced how to initialize BLE Mesh as a Node port. You may still have the following questions: 

- What else needs to be done before initialization?
- How to add Element and Model to Stack?
- How do we opt for a different encryption approach?
- How to declare the features of Proxy, Relay, Low Power and Friend?

This section will answer those questions.
 
First, before calling `esp_ble_mesh_init` API to initialize the BLE Mesh, we have to declare and define Element and Model as demonstrated in the picture below, detailing the codes to define Element:

<center>

	<img src="https://i.imgur.com/fSs0iyy.png" width="%" height="%" />
	<img src="https://i.imgur.com/pNipHHc.png" width="%" height="%" />
	<img src="https://i.imgur.com/OkEyOab.png" width="%" height="%" />

**Figure 5**

</center>

The top picture in **Figure 5** shows the declaration of the Element structure, the middle shows the definition of the Element structure, and the bottom shows the Macro to be used when defining the Element.  The next part will introduce the variables of the Element structure:
 
- `addr`: the variable that stores the Element primary address, which will be used by Mesh Stack during the configuration process. For the higher level applications, it can be ignored.
- `loc`: the Location Descriptor defined by SIG. In this demo, set its value to 0.
- `model_count`: the number of the SIG Models supported in this Element.
- `vnd_model_count`: the number of the Vendor Model supported in this Element.
- `models`: the pointer to the defined of the defined SIG Models.
- `vnd_models`: the pointer to the defined of the defined Vendor Model.

<table><tr><td bgcolor=orange> Note: the SIG Model count and the Vendor Model count work separately. For example, if two SIG Models and one Vendor Model are supported in our Element, it would be model_count = 2, vnd_model_count = 1. </td></tr></table>

The middle picture in **Figure 5** presents the definition of the Element, which only requires to call the Marco `ESP_BLE_MESH_ELEMENT`. The bottom picture is about the codes to implement the Macro `ESP_BLE_MESH_ELEMENT`.
<table><tr><td bgcolor=orange> Note: when our defined Element only supports the SIG Model, which means the Vendor Model is not supported, the last parameter in the Macro `ESP_BLE_MESH_ELEMENT` should be set as `ESP_BLE_MESH_MODEL_NODE`. Likewise, if the SIG Model is not supported, the middle parameter should be set as `ESP_BLE_MESH_MODEL_NODE`. </td></tr></table>

#### 2.2.3 Implementation of BLE Mesh Model Structure

The above section has introduced the specific ways to implement and define the Element by passing the specific Model pointers to the above Element. In this section, we'll introduce how to implement and define the Model, which is demonstrated in the picture below:

<center>
	<img src="https://i.imgur.com/7oQpCWA.png" />
	<img src="https://i.imgur.com/4sxvONQ.png" />

**Figure 6**

</center>

First, let's take a look at the top picture in **Figure 6**, which shows the specific implementation of the Model structure. There are many variables in this structure, but only the following four variables will be used for applications:

- `id` and `vnd`: both as the union variables, of which `id` is used for defining the **SIG Model**, and `vnd` for the **Vendor Model**.
- `op`: the structure of the Model Operation to declare the opcode  that corresponds with **Get**, **Set**, or **Status State**, and the minimum value length that are supported in this module. 
- `pub`: the structure that has to be defined in case the Model supports the Publish function.
- `user_data`: an optional variable that is used to store the application layer data.

As for other structures and variables (keys, group, elem), the BLE Mesh Stack will pass the parameters to them during the initialization or the configuration process. Therefore, it is not necessary for users to initialize their values.

Let's move on to the bottom picture in **Figure 6**, which presents the definition of the Model, and defines the `root_models[]` array that is used to indicate the number of the Model we have. The Model is implemented by using Macros. Different Models require different Marcos. Generally, there are the following types of Models:

- SIG Model: for the SIG Model that hasn't been implemented in ESP32 BLE Mesh Stack, it has to be defined by using the Macro `ESP_BLE_MESH_SIG_MODEL`.
- Vendor Model: it needs to be defined by using the Marco `ESP_BLE_MESH_VENDOR_MODEL`.
- The Model that has been implemented in ESP32 BLE Mesh Stack: it needs to be defined by using a specific Macro. Currently, the following Models have been implemented in our protocol stack:
	- Configuration Server Model: it needs to be defined by using the Marco `ESP_BLE_MESH_MODEL_CFG_SRV`. A parameter, which is defined as struct `esp_ble_mesh_cfg_srv_t` needs to passed to this Marco.
	- Configuration Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_CFG_CLI`. A parameter, which is defined as struct `esp_ble_mesh_cfg_cli_t` needs to be passed to this Macro.
	- Generic OnOff Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI`. **All the below models take two parameters:** One is `cli_pub`, which is defined as struct `esp_ble_mesh_model_pub_t`, and the other is `cli_data`, which is defined as struct  `esp_ble_mesh_client_t`.
	- Generic Level Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_LEVEL_CLI`.
	- Generic Default Transition Time Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI`.
	- Generic Power OnOff Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_POWER_ONOFF_CLI`.
	- Generic Power Level Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_POWER_LEVEL_CLI`.
	- Generic Battery Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_BATTERY_CLI`.
	- Generic Location Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_LOCATION_CLI`.
	- Generic Location Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_LOCATION_CLI`.
	- Generic Property Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_GEN_PROPERTY_CLI`.
	- Light Lightness Client Model: it needs to be defined by using the Marco `ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI`.
	- Light CTL Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_LIGHT_CTL_CLI`.
	- Light HSL Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_LIGHT_HSL_CLI`.
	- Sensor Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_SENSOR_CLI`.
	- Scene Client Model: it needs to be defined by using the Macro `ESP_BLE_MESH_MODEL_SCENE_CLI`.

The above section has introduced how to declare and define the Model. Let's now focus on an important structure in the Model, i.e. `esp_ble_mesh_model_op_t *op` pointers, which point to the operation structure that defines the Model state. Generally, there are two types of Model in BLE Mesh:

- Server Model:
	- Consists of at least one or multiple states that may appear across different Elements
	- Defines the messages sent/received by the Model, along with the Element behavior accordingly.
		- Example：On/Off switch --- Indicates the On/Off status.
- Client Model:
	- Defines the messages used by the Client to request, change or use the relevant state of the server.
		- Example：On/Off switch --- Indicates the On or Off message sent by the Client.

Operation structure defines the state value supported by the Model.  Please find the specific operation structure below:
<center>
	<img src="https://i.imgur.com/XA5Y1FU.png" />
	<img src="https://i.imgur.com/Jwdqhh6.png" />

**Figure 7**
</center>

In **Figure 7**, the top picture shows the declaration of the Model operation structure, and the bottom picture shows the operation structure array defined by the OnOff Server in this Demo. There are three variables in the declaration of the operation structure:

- `opcode`: the opcode that corresponds with each State. As specified in BLE Mesh, the Opcode of the SIG Model should be 1~2 bytes, and the Opcode of the Vendor Model should be 3 bytes.
- `min_len`: the minimum length of the messages received by the State, for example, OnOff Get State is 0 byte, and OnOff Set State is 2 byte.
- `param_cb`: this is only for the BLE Mesh protocol. Applications just need to initialize its value to 0.

The bottom picture in **Figure 7** presents the Opcodes defined in this Demo that correspond with the three States, which are OnOff Get, OnOff Set and OnOff Set Unack.

<table><tr><td bgcolor=orange> Note: for the Server Model, the corresponding request State of the Client needs to be written in the operation definition. For example, in this Demo, if the Generic OnOff Server Model is to be implemented, the following three States requested by the Generic OnOff Client Model need to be written in the operation definition: OnOff Get, OnOff Set and OnOff Set Unack. Likewise, for the Client Model, the corresponding State of the messages received by the Server needs to be written in the operation definition. </td></tr></table>

#### 2.2.4. Encryption and Authentication of BLE Mesh

In the project developing process, different projects require different security levels. BLE Mesh offers various kinds of encryption and authentication. Generally speaking, there are two types, i.e. **Input** and **Output**, and these two types can be further classified into the following categories:

- **Input**:
	- `ESP_BLE_MESH_NO_INPUT`: indicates the device IO doesn't support the **Input** function.
	- `ESP_BLE_MESH_PUSH`: indicates **PUSH** is supported for the Input function.
	- `ESP_BLE_MESH_TWIST`: indicates **TWIST** is supported for the Input function.
	- `ESP_BLE_MESH_ENTER_NUMBER`: indicates **ENTER NUMBER** is supported for the Input function.
	- `ESP_BLE_MESH_ENTER_STRING`: indicates **ENTER STRING** is supported for the Input function.
- **Output**:
	- `ESP_BLE_MESH_NO_OUTPUT`: indicates the device IO doesn't support the **Output** function.
	- `ESP_BLE_MESH_BLINK`: indicates **BLINK** is supported for the Output function.
	- `ESP_BLE_MESH_BEEP`: indicates **BEEP** is supported for the Output function.
	- `ESP_BLE_MESH_VIBRATE`: indicates **VIBRATE** is supported for the Output function.
	- `ESP_BLE_MESH_DISPLAY_NUMBER`: indicates **DISPLAY NUMBER** is supported for the Output function.
	- `ESP_BLE_MESH_DISPLAY_STRING`: indicates **DISPLAY STRING** is supported for the Output function.

The above **Input** and **Output** categories in the declaration of the structure `esp_ble_mesh_prov_t` can be defined by the following four variables:

- `output_size`
- `output_actions`
- `input_actions`
- `input_size`

In this Demo, we opt for the simplest authentication, which is setting `output_size`, `output_actions`, `input_actions` and `input_size` to 0.

## 3. Configuration of BLE Mesh Menuconfig

To meet the various requirements in different applications, our BLE Mesh Menuconfig is specially designed with a variety of configuration options, which can help users quickly tailor their configuration. The configuration option list of ESP32 Menuconfig can be opened by using the command `make menuconfig`. The configuration option list of BLE Mesh is located in `Component config`  ---> `[]Bluetooth Mesh support`. Once they are opened, the following list will appear:
<center>
	<img src="https://i.imgur.com/jinm2QA.png" />
	<img src="https://i.imgur.com/e0jXrkn.png" />

**Figure 8**
</center>

**Figure 8** presents lots of configuration options. Please find the  part below detailing their roles and functions:

- **Support for BLE Mesh Node**: Indicates whether Node is supported. There are two roles in BLE Mesh, one is provisioner, and the other is Node. In this Demo, only Node is supported, which means Provisioner is not supported.
- **Support for BLE Mesh Provisioner**: Indicates whether Provisioner is supported.
- **Provisioning support using the advertising bearer (PB-ADV)**: Indicates whether **PB-ADV** bearer is supported for network provisioning. In BLE Mesh，two bearers are supported for network provisioning, i.e. **PB-ADV** and **PB-GATT**. In this Demo, both of them are supported.
- **net buffer pool usage**: Once it is enabled, BLE Mesh will summarize the usage of Adv buffer.
- **Provisioning support using GATT (PB-GATT)**: Indicates whether **PB-GATT** bearer is supported for network provisioning.
- **GATT Proxy Service**: indicates whether GATT proxy service is supported.
	- **Node Identity advertising timeout**: Indicates the time (unit: s) connected to the advertising sent by BLE Mesh protocol stack when using the GATT proxy service.
- **Maximum number of filter entries per Proxy Client**: It is used to configure the maximum number of the filtered address. In order to reduce the number of Network PDUs exchanged between a Proxy Client and a Proxy Server, a proxy filter can be used. The output filter of the network interface instantiated by the Proxy Server can be configured by the Proxy Client. This allows the Proxy Client to explicitly request to receive only mesh messages with certain destination addresses. For example, a Proxy Client that is subscribed to a group address may want to only receive packets addressed to the unicast address of one of its elements and to that group address. Thus, the Proxy Client has full control over the packets it receives using the Proxy protocol.

- **Maximum number of mesh subnets per network**: Indicates the maximum number of the subnets supported in BLE Mesh.
- **Maximum number of application keys per network**: Indicates the maximum number of the App keys supported in BLE Mesh.
- **Maximum number of application keys per model**: Indicates the maximum number of the App keys bound in each Model in BLE Mesh.
- **Maximum number of group address subscriptions per model**: Indicates the maximum number of the group address subscriptions supported in each Model in BLE Mesh.
- **Maximum number of Label UUIDs used for Virtual Addresses**: Indicates the maximum number of the Label UUID supported in BLE Mesh.
- **Maximum capacity of the replay protection list**: Indicates what 
- the name suggests.
- **Network message cache size**: Configures the size of the Cache, which is used to store the forwarded messages to avoid multiple forwarding in BLE Mesh.
- **Number of advertising buffers**: Indicates what the name suggests.
- **Maximum number of simultaneous outgoing segmented messages**: Indicates what the name suggests.
- **Maximum number of simultaneous incoming segmented messages**: Indicates what the name suggests.
- **Maximum incoming Upper Transport Access PDU length**: Indicates the access layer can receive the maximum length of a complete packet.
- **Relay support**: Indicates whether the Relay feature is supported.
- **Support for Low Power features**: Indicates whether the Low Power feature is supported.
- **Support for acting as a Friend Node**: Indicates whether the Friend feature is supported.
- **Support for Configuration Client Model**: Indicates whether the Configuration Client Model is supported.
- **Support for Health Client Model**: Indicates whether the Health Client Model is supported.
- **Support for Generic OnOff Client Model**: Indicates whether the Generic OnOff Client Model is supported.
- **Support for Generic Level Client Model**: Indicates whether the Generic Level Client Model is supported.
- **Support for Generic Default Transition Time Client Model**: Indicates whether the Generic Default Transition Time Client Model is supported.
- **Support for Generic Power Onoff Client Model**: Indicates whether the Generic Power Onoff Client Model is supported.
- **Support for Generic Power Level Client Model**: Indicates whether the Generic Power Level Client Model is supported.
- **Support for Generic Battery Client Model**: Indicates whether the Generic Battery Client Model is supported.
- **Support for Generic Location Client Model**: Indicates whether the Generic Location Client Model is supported.
- **Support for Generic Property Client Model**: Indicates whether the Generic Property Client Model is supported.
- **Support for Sensor Client Model**: Indicates whether the Sensor Client Model is supported.
- **Support for Scene Client Model**: Indicates whether the Scene Client Model is supported.
- **Support for Light Lightness Client Model**: Indicates whether the Light Lightness Client Model is supported.
- **Support for Light CTL Client Model**: Indicates whether the Light CTL Client Model is supported.
- **Support for Light HSL Client Model**: Indicates whether the Light HSL Client Model is supported.