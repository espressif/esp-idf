# GATT Security Client Example Walkthrough



## Introduction

This documents presents a review of the GATT Security Client code example for the ESP32. The GATT Client is capable of scanning for nearby devices and once it has found a device of interest, it requests a secure connection. The GATT client behaves as a master device that initiates a connection to a slave by sending a *Pairing Request* as specified by the [Bluetooth Core Specification Version 4.2](https://www.bluetooth.com/specifications/bluetooth-core-specification). The remote slave device is normally a GATT Server that exposes Services and Characteristics. The slave replies with a *Pairing Response* followed by authentication and exchange of keys. If the bonding process is also executed, the Long Term Keys are stored for subsequent connections. Finally an encrypted channel is established which can support protection against Man-In-The-Middle (MITM) attacks depending on the security configuration. The code is implemented using an Application Profile that upon registration, allows to set the local privacy configuration as events are triggered during the lifetime of the program. 

This document only includes a description of the security aspects of the GATT Client implementation, for a review of how to define the functionality of the GATT client such as scanning parameters and opening connections please refer to **GATT Client Example Walkthrough**.

## Configuring Local Privacy of the Security Client

The example registers one Application Profile defined as:

```
#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0
```

The registration takes place in the ``app_main()`` function by using the ``esp_ble_gattc_app_register()`` function:

```
…
ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret){
        ESP_LOGE(GATTC_TAG, "%s gattc app register error, error code = %x\n", __func__, ret);
    }
…
```

The Application Profile registration triggers an ``ESP_GATTC_REG_EVT`` event which is managed by the ``esp_gattc_cb()`` callback function and forwarded to the Profile A event handler ``gattc_profile_event_handler()``. Here, the event is used to configure the local privacy of the slave device by using the ``esp_ble_gap_config_local_privacy()`` function.

```
case ESP_GATTC_REG_EVT:
        ESP_LOGI(GATTC_TAG, "REG_EVT");
        esp_ble_gap_config_local_privacy(true);
        break;
```

This function is a Bluedroid API call for configuring default privacy settings on the local device. Once the privacy is set, an ``ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT`` is triggered which is used to set scan parameters and start scanning for nearby peripherals:

```
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS){
            ESP_LOGE(GATTC_TAG, "config local privacy failed, error code =%x", param->local_privacy_cmpl.status);
            break;
        }
        esp_err_t scan_ret = esp_ble_gap_set_scan_params(&ble_scan_params);
        if (scan_ret){
            ESP_LOGE(GATTC_TAG, "set scan params error, error code = %x", scan_ret);
        }
        break;
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        //the unit of the duration is second
        uint32_t duration = 30;
        esp_ble_gap_start_scanning(duration);
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "scan start failed, error status = %x", param->scan_start_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "Scan start success");
        break;
```

## Configuring and Bonding to a Slave Device

The rest of the configuration for the GATT Client is performed normally in the same way as the regular GATT Client example. That is, the client finds a device of interest and opens a connection. At this point the GATT client, which is usually the master, initiates the pairing process by sending a Pairing Request to the slave device. This request should be acknowledged with a Pairing Response. The Pairing process is implemented automatically by the stack and no extra user configuration is needed. However, depending on the I/O capabilities of both devices, a passkey might be generated on the ESP32 which is presented to the user with the ``ESP_GAP_BLE_PASSKEY_NOTIF_EVT``:

```
 case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
        ///show the passkey number to the user to input it in the peer deivce.
        ESP_LOGE(GATTS_TABLE_TAG, "The passkey Notify number:%d", param->ble_security.key_notif.passkey);
        break;

```
The combination of input and output capabilities that determine which algorithm is used are:

  | Display Only | Display Yes/No|Keyboard Only|No Input No Output|Keyboard Display|
--------------------|------------------|-----------------------|--------------------|------------------|-----------------------|
Display Only | Just Works| Just Works|Passkey Entry|Just Works|Passkey Entry|
Display Yes/No |Just Works|Just Works|Passkey Entry|Just Works|Passkey Entry|
Keyboard Only |Passkey Entry |Passkey Entry|Passkey Entry|Just Works|Passkey Entry|
No Input No Output| Just Works|Just Works|Just Works|Just Works|Just Works|
Keyboard Display|Passkey Entry|Passkey Entry|Passkey Entry|Just Works|Passkey Entry|

In the Just Works method, the Temporary Key is set to 0. This is a practical way to authenticate devices when no display or keyboards are attached to them, so that there is no way to show or enter a passkey. However, if the ESP32 GATT Client has an LCD, it can present the passkey generated locally so that the user can input it on the other peer device, or if the GATT Client has a keyboard, it can input the passkey generated by the other peer device. Additionally, a numeric comparison can be performed if both devices have a display and yes/no confirm buttons and LE Secure Connections are used, that way an independently generated passkey is displayed on both devices and the user manually checks that both 6-digit confirmation values match.

## Exchanging Keys

When the client connects to a remote device and the pairing is done successfully, the initiator and responder keys are exchanged. For each key exchange message, an ``ESP_GAP_BLE_KEY_EVT`` event is triggered which can be used to print the type of key received:

```
case ESP_GAP_BLE_KEY_EVT:
        //shows the ble key info share with peer device to the user.
        ESP_LOGI(GATTS_TABLE_TAG, "key type = %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
        break;       
```

When the keys are exchanged successfully, the pairing process is completed and encryption of payload data can be started using the AES-128 engine. This triggers an ``ESP_GAP_BLE_AUTH_CMPL_EVT`` event which is used to print information:

```
case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        esp_bd_addr_t bd_addr;
        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        ESP_LOGI(GATTS_TABLE_TAG, "remote BD_ADDR: %08x%04x",\
                (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                (bd_addr[4] << 8) + bd_addr[5]);
        ESP_LOGI(GATTS_TABLE_TAG, "address type = %d", param->ble_security.auth_cmpl.addr_type);
        ESP_LOGI(GATTS_TABLE_TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
        break;
```

## Conclusion

In this document, a review of the security aspects of the GATT Client has been realized. BLE security encompasses Pairing, Bonding and Encryption. In order to establish a secure link between a master and a slave device, the local privacy of the GATT client is set, which allows the BLE stack to set necessary security parameters automatically without the need of additional user configuration. The combination of features and capabilities of the peer devices results in the selection of the appropriate pairing method which the BLE stack then executes. Immediately, the required keys are generated and exchanged and the encryption of subsequent messages is started using the AES-128 engine. These steps trigger different events that are managed by the GATT and GAP event handlers which can be used to print useful information such as the types of keys exchanged and the pairing status. The rest of the security GATT client functionality such as registering for notifications of characteristics is implemented in the same way as in **GATT Client Example Walkthrough**.