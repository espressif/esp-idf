
# Demo for ESP-IDF BLE Mesh Provisioner

## 1. Introduction

This Demo is used for the demonstration of BLE Device working as a Provisioner. Please go check the [Demo for ESP-IDF BLE Mesh Node](../ble_mesh_node/README.md) first, if you don't have much experience with BLE Mesh.

## 2. Code Flow

### 2.1 Initialization and Enabling

<center>
![](https://i.imgur.com/l3xoiba.png)

**Figure 1**

</center>

As shown in **Figure 1**, the following should be done for the initialization of BLE Mesh with `ble_mesh_init`.

- Line 207: register the `adv callback` function. When working as a provisioner, BLE Device must scan for to-be-provisioned device, and decide whether to provision this device or not.
- Line 209: add the device to the list of to-be-provisioned devices. Once a device is added to the list, the BLE Mesh protocol stack will automatically provision this device when the device is found.
- Line 217: call API `esp_ble_mesh_provisioner_prov_enable` to scan for the to-be-provisioned devices in the surrounding environment.

### 2.2 Set and Bind App Key

When working as a Provisioner that initiates the provisioning, BLE Device also has to configure parameters, including App key, TTL and proxy, after the provisioning, which are not required when working as a Node. Please note the configuration of App key is of vital importance. The Application can only send/receive data (set state, get state and publish) after the App key has been set and bound.

<center>
![](https://i.imgur.com/OLjmmW4.png)

**Figure 2**

</center>

As shown in **Figure 2**, after the provisioning is completed, API `esp_ble_mesh_provisioner_local_app_key_add` must be called (code line 98) to set and bind the App key. Then, the device can send/receive Model messages to/from peer devices.

<table><tr><td bgcolor=orange> Note: Set and bind App key is used to configure and bind App key through the Configuration Client Model. Therefore, Provisioner must also register the Configuration Client Model. </td></tr></table>

### 2.3 Register Configuration Client Model

The process to register a Configuration Client Model is similar to the process of registering other models, which can be seen below:

<center>
![](https://i.imgur.com/Z2hdRqY.png)

**Figure 3**

</center>

As shown in **Figure 3**, the following should be done to register a Configuration Client Model:

- Line 58: define a variable `cfg_client` of type `esp_ble_mesh_client_t`
- Line 62: use Marco `ESP_BLE_MESH_MODEL_CFG_CLI` to add this Configuration Client Model to array `root_models`

After the definition, the Configuration Client Model can be registered simply by passing the pointer of `element` to the BLE Mesh protocol stack.