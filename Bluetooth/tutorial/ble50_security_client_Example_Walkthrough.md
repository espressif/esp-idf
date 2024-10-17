# BLE50 GATT Security Client Example Walkthrough

## Introduction

* This document presents a review of the GATT Security Client code example for the ESP32C3 or BLE5.0 supported Chip. 
* The GATT Client is capable of scanning for nearby devices and once it has found a device of interest, it requests a secure connection. The GATT client behaves as a central device that initiates a connection to a peripheral by sending a *Aux Connect Pairing Request* as specified by the [Bluetooth Core Specification Version 5.0](https://www.bluetooth.com/specifications/bluetooth-core-specification). 
* The peripheral device is normally a GATT Server that exposes Services and Characteristics. The peripheral replies with a *Aux Connect Pairing Response* followed by authentication and exchange of keys. If the bonding process is also executed, the Long Term Keys are stored for subsequent connections. Finally an encrypted channel is established which can support protection against Man-In-The-Middle (MITM) attacks depending on the security configuration. 
* The code is implemented using an Application Profile that upon registration, allows to set the local privacy configuration as events are triggered during the life time of the program. 

This document only includes a description of the security aspects of the BLE5.0 Security GATT Client implementation, for the more infor about extended scan , periodic scan please refer to [Periodic_Sync_Example Walkthrough] (../../peroidic_sync/tutorial/Periodic_Sync_Example_Walkthrough.md).

##include

```c
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
```
These `includes` are required for the FreeRTOS and underlying system components to run, including the logging functionality and a library to store data in non-volatile flash memory. We are interested in `“bt.h”`, `“esp_bt_main.h”`, `"esp_gap_ble_api.h"` and `“esp_gattc_api.h”`, which expose the BLE APIs required to implement this example.

* `bt.h`: configures the BT controller and VHCI from the host side.
* `esp_bt_main.h`: initializes and enables the Bluedroid stack.
* `esp_gap_ble_api.h`: implements the GAP configuration, such as advertising and connection parameters.
* `esp_gattc_api.h`: implements the GATT Client configuration, such as connecting to peripherals and searching for services.

## Configuring Local Privacy of the Security Client

The example registers one Application Profile defined as:

```c
#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0
```

The registration takes place in the ``app_main()`` function by using the ``esp_ble_gattc_app_register()`` function:

```c
…
ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
if (ret){
	ESP_LOGE(GATTC_TAG, "%s gattc app register error, error code = %x", __func__, ret);
}
…
```

The Application Profile registration triggers an ``ESP_GATTC_REG_EVT`` event which is managed by the ``esp_gattc_cb()`` callback function and forwarded to the Profile A event handler ``gattc_profile_event_handler()``. Here, the event is used to configure the local privacy of the slave device by using the ``esp_ble_gap_config_local_privacy()`` function.

```c
case ESP_GATTC_REG_EVT:
    ESP_LOGI(GATTC_TAG, "REG_EVT");
    esp_ble_gap_config_local_privacy(true);
    break;
```

This function is a Bluedroid API call for configuring default privacy settings on the local device. Once the privacy is set, an ``ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT`` is triggered which is used to set scan parameters and start scanning for nearby peripherals:

```c
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
         if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS){
       ESP_LOGE(GATTC_TAG, "config local privacy failed, error code =%x", param->local_priva
cy_cmpl.status);
            break;
        }
        esp_err_t scan_ret = esp_ble_gap_set_ext_scan_params(&ext_scan_params);
        if (scan_ret){
            ESP_LOGE(GATTC_TAG, "set extend scan params error, error code = %x", scan_ret);
        }
        break;
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT: {
        if (param->set_ext_scan_params.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "extend scan parameters set failed, error status = %x", param->se
t_ext_scan_params.status);
            break;
        }
        //the unit of the duration is second
        esp_ble_gap_start_ext_scan(EXT_SCAN_DURATION, EXT_SCAN_PERIOD);
        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        if (param->ext_scan_start.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "scan start failed, error status = %x", param->scan_start_cmpl.st
atus);

```

## Configuring and Bonding to a Slave Device

The rest of the configuration for the GATT Client is performed normally in the same way as the regular GATT Client example. That is, the client finds a device of interest and opens a connection. At this point the GATT client, which is usually the central, initiates the pairing process by sending a Pairing Request to the peripheral device. This request should be acknowledged with a Pairing Response. The Pairing process is implemented automatically by the stack and no extra user configuration is needed. However, depending on the I/O capabilities of both devices, a passkey might be generated on the ESP32 which is presented to the user with the ``ESP_GAP_BLE_PASSKEY_NOTIF_EVT``:

```c
 case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  
 ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
 ///show the passkey number to the user to input it in the peer device.
      ESP_LOGE(GATTS_TABLE_TAG, "The passkey Notify number:%d", param->ble_security.key_notif.passkey);
      break;

```
The combination of input and output capabilities that determine which algorithm is used are:

|                        | Display Only   | Display Yes/No | Keyboard Only  | No Input No Output | Keyboard Display|
| :--                    | :------------- | :------------- | :------------- | :----------------- | :-------------- |
| **Display Only**       | Just Works     | Just Works     | Passkey Entry  | Just Works         | Passkey Entry   |
| **Display Yes/No**     | Just Works     | Just Works     | Passkey Entry  | Just Works         | Passkey Entry   |
| **Keyboard Only**      | Passkey Entry  | Passkey Entry  | Passkey Entry  | Just Works         | Passkey Entry   |
| **No Input No Output** | Just Works     | Just Works     | Just Works     | Just Works         | Just Works      |
| **Keyboard Display**   | Passkey Entry  | Passkey Entry  | Passkey Entry  | Just Works         | Passkey Entry   |

In the Just Works method, the Temporary Key is set to 0. This is a practical way to authenticate devices when no display or keyboards are attached to them, so that there is no way to show or enter a passkey. However, if the ESP32 GATT Client has an LCD, it can present the passkey generated locally so that the user can input it on the other peer device, or if the GATT Client has a keyboard, it can input the passkey generated by the other peer device. Additionally, a numeric comparison can be performed if both devices have a display and yes/no confirm buttons and LE Secure Connections are used, that way an independently generated passkey is displayed on both devices and the user manually checks that both 6-digit confirmation values match.

## Exchanging Keys

When the client connects to a remote device and the pairing is done successfully, The initiator and responder keys as decided during pairing req / rsp are exchanged. For each key exchange message, an ``ESP_GAP_BLE_KEY_EVT`` event is triggered which can be used to print the type of key received:

```c
case ESP_GAP_BLE_KEY_EVT:
    //shows the ble key info share with peer device to the user.
    ESP_LOGI(GATTS_TABLE_TAG, "key type = %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
    break;       
```

When the keys are exchanged successfully, the pairing process is completed and encryption of payload data can be started using the AES-128 engine. This triggers an ``ESP_GAP_BLE_AUTH_CMPL_EVT`` event which is used to print information:

```c
case ESP_GAP_BLE_AUTH_CMPL_EVT: {
    esp_bd_addr_t bd_addr;
    memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
    ESP_LOGI(GATTS_TABLE_TAG, "remote BD_ADDR: %08x%04x",\
    (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3], (bd_addr[4] << 8) + bd_addr[5]);
    ESP_LOGI(GATTS_TABLE_TAG, "address type = %d", param->ble_security.auth_cmpl.addr_type);
    ESP_LOGI(GATTS_TABLE_TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
    break;
```

## Conclusion

In this document, a review of the security aspects of the BLE5.0 GATT Client has been realized. BLE security encompasses Pairing, Bonding and Encryption. In order to establish a secure link between a central and a peripheral device, the local privacy of the GATT client is set, which allows the BLE stack to set necessary security parameters automatically without the need of additional user configuration. The combination of features and capabilities of the peer devices results in the selection of the appropriate pairing method which the BLE stack then executes. Immediately, the required keys are generated and exchanged and the encryption of subsequent messages is started using the AES-128 engine. These steps trigger different events that are managed by the GATT and GAP event handlers which can be used to print useful information such as the types of keys exchanged and the pairing status. The rest of the security GATT client functionality such as registering for notifications of characteristics is implemented in the same way as in [Periodic Sync Example Walkthrough](../../peroidic_sync/tutorial/Periodic_Sync_Example_Walkthrough.md).
