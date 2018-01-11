# Gatt Security Server Example Walkthrough

## Introduction

In this document, a description of the security GATT Server BLE example for the ESP32 is presented. The security configuration enables a GATT Server acting as a slave device to bond with a master and establish an encrypted link between them. This functionality is defined by the [Bluetooth Specification version 4.2](https://www.bluetooth.com/specifications/bluetooth-core-specification) and implemented on the ESP-IDF BLE stack, specifically on the Security Manager Protocol (SMP) API.

BLE security involves three interrelated concepts: pairing, bonding and encryption. Pairing concerns with the exchange of security features and types of keys needed. In addition, the pairing procedure takes care of the generation and exchange of shared keys. The core specification defines the legacy pairing and Secure Connections pairing (introduced in Bluetooth 4.2), which are both supported by ESP32. Once the exchange of shared keys is completed, a temporary encrypted link is established to exchange short term and long term keys. Bonding refers to storing the exchanged keys for subsequent connections so that they do not have to be transmitted again. Finally, encryption pertains to the ciphering of plain text data using the AES-128 engine and the shared keys. Server attributes may also be defined to allow only encrypted write and read messages. At any point of the communication, a slave device can always ask to start encryption by issuing a security request to the other peer device, which returns a security response by calling an API.

This document only describes the security configuration. The rest of the GATT server functionalities, such as defining the service table, are explained in the GATT Server example walkthrough documentation. For a better understanding of this example workflow, it is recommended that the reader is familiar with the pairing feature exchange and key generation defined in the section 3.5 of the [Bluetooth Specification Version 4.2](https://www.bluetooth.com/specifications/bluetooth-core-specification) [Vol 3, Part H].

## Setting Security Parameters

The ESP32 requires a series of security parameters in order to define how the pairing request and response are going to be built. The Pairing Response packet built by the GATT Server includes fields such as the input/output capabilities, Secure Connections pairing, authenticated Man-In-The-Middle (MITM) protection or no security requirements (see Section 2.3.1 of the [Bluetooth Specification Version 4.2](https://www.bluetooth.com/specifications/bluetooth-core-specification) [Vol 3, Part H]). In this example, this procedure is done in the `app_main()` function. The pairing request is sent by the initiator which in this case is a remote GATT client. The ESP32 server implemented in this example receives this request and replies with a pairing response, which contains the same security parameters in order for both devices to agree on the resources available and the applicable pairing algorithm (*Just Works* or *Passkey Entry*). Both the pairing request and response commands have the following parameters:

* *IO Capability*: describes if the device has input/output capabilities such as a display or a keyboard.
* *OOB Flag*: describes if the device supports Out of Band passkey exchange, for example using NFC or Wi-Fi to exchange keys as TKs. Currently, the ESP32 does not support this feature.
* *Authorization Request*: indicates the requested security properties such as Bonding, Secure Connections (SC), MITM protection or none that will be present in the Pairing Request and Response packets.
* *Maximum Encryption Key Size*: maximum encryption key size in octets.
* *Initiator Key Distribution/Generation*: indicates which keys the initiator is requesting to distribute/generate or use during the Transport Specific Key Distribution phase. In the pairing request, these keys are requested, while in the pairing response, these keys are confirmed to be distributed.
* *Responder Key Distribution/Generation*: indicates which keys the initiator is requesting the responder to distribute/generate or use during the Transport Specific Key Distribution phase. In the pairing request, these keys are requested, while in the pairing response, these keys are confirmed to be distributed.

In code, these parameters are defined as follows:

* *IO Capability*:

	```c
	esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;//set the IO capability to No Input No Output
	```

	The possible values for *IO Capabilities* are:

	```c
	ESP_IO_CAP_OUT         0   /*!< DisplayOnly */
	ESP_IO_CAP_IO          1   /*!< DisplayYesNo */
	ESP_IO_CAP_IN          2   /*!< KeyboardOnly */
	ESP_IO_CAP_NONE        3   /*!< NoInputNoOutput */
	ESP_IO_CAP_KBDISP      4   /*!< Keyboard display */
	```

* *Authorization Request*:

	```c
	esp_ble_auth_req_t auth_req = ESP_LE_AUTH_BOND; //bonding with peer device after authentication
	```

	The possible values for *Authorization Request* are a combination of Bonding, MITM protection and Secure Connections requests:

	```c
	ESP_LE_AUTH_NO_BOND: No bonding.
	ESP_LE_AUTH_BOND: Bonding is performed.
	ESP_LE_AUTH_REQ_MITM: MITM Protection is enabled.
	ESP_LE_AUTH_REQ_SC_ONLY: Secure Connections without bonding enabled.
	ESP_LE_AUTH_REQ_SC_BOND: Secure Connections with bonding enabled.
	ESP_LE_AUTH_REQ_SC_MITM: Secure Connections with MITM Protection and no bonding enabled.
	ESP_LE_AUTH_REQ_SC_MITM_BOND: Secure Connections with MITM Protection and bonding enabled.
	```

* *Maximum Encryption Key Size*:

	```c
	uint8_t key_size = 16;      //the key size should be 7~16 bytes
	```

* *Initiator Key Distribution/Generation*:

	```c
	uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	```

	The initiator distributes the LTK and IRK keys by the setting the EncKey and IdKey masks.

* *Responder Key Distribution/Generation*:

	```c
	uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	```

	The responder distributes the LTK and IRK keys by the setting the *EncKey* and *IdKey* masks.

	Once defined, the parameters are set using the `esp_ble_gap_set_security_param()` function. This function sets the parameter type, the parameter value and the parameter length:

	```c
	esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
	```

	This information is enough for the BLE stack to perform the pairing process, including pairing confirmation and key generation. The procedure is invisible to the user and executed automatically by the stack.

## Connecting and Bonding to a Peer Device

The security parameters set previously are stored locally to be used later when the master device connects to the slave. Every time a remote device connects to the local GATT server, the connection event `ESP_GATTS_CONNECT_EVT` is triggered. This event is employed to perform the pairing and bonding process by invoking the `esp_ble_set_encryption()` function which takes as parameters the remote device address and the type of encryption that will be performed. The BLE stack then executes the actual pairing process in the background. In this example, the encryption includes MITM protection.

```c
case ESP_GATTS_CONNECT_EVT:
     //start security connect with peer device when receive the connect event sent by the master.  
    esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);  
    break;
```
The types of encryptions available are:

* `ESP_BLE_SEC_NONE`
* `ESP_BLE_SEC_ENCRYPT`
* `ESP_BLE_SEC_ENCRYPT_NO_MITM`
* `ESP_BLE_SEC_ENCRYPT_MITM`

The difference between `ESP_BLE_SEC_ENCRYPT` and `ESP_BLE_SEC_ENCRYPT_NO_MITM` lies in the fact that a previous connection might have a security level that needs to be upgraded, therefore requires to exchange keys again.

In this example, the I/O capabilities are set to *No Input No Output*, therefore the *Just Works* pairing method, which doesn't not require the generation of a random 6-digit passkey, is used (For details, please refer to the the table below). The user may modify the example to set the I/O capabilities to use other than *No Input No Output*. Therefore, depending on the I/O capabilities of the remote device, a passkey might be generated on the ESP32 which is presented to the user with the `ESP_GAP_BLE_PASSKEY_NOTIF_EVT`:

```c
case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:    
     ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
     ///show the passkey number to the user to input it in the peer deivce.
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


## Exchanging Keys

When the client connects to the server and the pairing is done successfully, the keys indicated by the `init_key` and `rsp_key` security parameters are exchanged. In this example the following keys are generated and distributed:

* Local device LTK
* Local device IRK
* Local device CSRK
* Peer device LTK
* Peer device IRK

Note that for this example only, the peer device CSRK is not exchanged. For each key exchange message, an `ESP_GAP_BLE_KEY_EVT` event is triggered, which can be used to print the type of key received:

```c
case ESP_GAP_BLE_KEY_EVT:
     //shows the ble key info share with peer device to the user.
     ESP_LOGI(GATTS_TABLE_TAG, "key type = %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
     break;
```

When the keys are exchanged successfully, the pairing process is done. This triggers an `ESP_GAP_BLE_AUTH_CMPL_EVT` event, which is used to print information such as remote device, address type and pair status:

```c
case ESP_GAP_BLE_AUTH_CMPL_EVT: {
     esp_bd_addr_t bd_addr;
     memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr,  
            sizeof(esp_bd_addr_t));
     ESP_LOGI(GATTS_TABLE_TAG, "remote BD_ADDR: %08x%04x",\
              (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) +   
              bd_addr[3],  
              (bd_addr[4] << 8) + bd_addr[5]);
     ESP_LOGI(GATTS_TABLE_TAG, "address type = %d",   
              param->ble_security.auth_cmpl.addr_type);  
              ESP_LOGI(GATTS_TABLE_TAG, "pair status = %s",  
              param->ble_security.auth_cmpl.success ? "success" : "fail");
     break;
}
```

## Security Permission for Attributes

When defining the attributes of the server, different permissions can be set to the write and read events. Attributes permissions can be:

* Permission to read
* Permission to read with encryption
* Permission to read with encryption and MITM protection
* Permission to write
* Permission to write with encryption
* Permission to write with encryption and MITM protection
* Permission to signed write
* Permission to signed write with MITM protection

These permissions are defined in the API as:

* `ESP_GATT_PERM_READ`
* `ESP_GATT_PERM_READ_ENCRYPTED`
* `ESP_GATT_PERM_READ_ENC_MITM`
* `ESP_GATT_PERM_WRITE`
* `ESP_GATT_PERM_WRITE_ENCRYPTED`
* `ESP_GATT_PERM_WRITE_ENC_MITM`
* `ESP_GATT_PERM_WRITE_SIGNED`
* `ESP_GATT_PERM_WRITE_SIGNED_MITM`

When creating the services table, each attribute can have permissions to read or write, with or without encryption. When an attribute has encrypted permissions and a peer device that does not have the required security clearance tries to read or write to that attribute, the local host sends an Insufficient Authorization Error. In the example, the following attributes are defined with permissions with encryption:

```c
…
// Body Sensor Location Characteristic Value
    [HRS_IDX_BOBY_SENSOR_LOC_VAL]   = {
    {ESP_GATT_AUTO_RSP},  
    {ESP_UUID_LEN_16,  
    (uint8_t *)&body_sensor_location_uuid,
    ESP_GATT_PERM_READ_ENCRYPTED,
    sizeof(uint8_t),  
    sizeof(body_sensor_loc_val),  
    (uint8_t *)body_sensor_loc_val}
    },
…
// Heart Rate Control Point Characteristic Value
    [HRS_IDX_HR_CTNL_PT_VAL]        = {
    {ESP_GATT_AUTO_RSP},  
    {ESP_UUID_LEN_16,  
    (uint8_t *)&heart_rate_ctrl_point,  
    ESP_GATT_PERM_WRITE_ENCRYPTED|ESP_GATT_PERM_READ_ENCRYPTED,  
    sizeof(uint8_t),  
    sizeof(heart_ctrl_point),   
    (uint8_t *)heart_ctrl_point}  
    },
…
```

## Security Requests

During the communication between a master and a slave device, the slave might request to start encryption at any moment by issuing a security request command. This command will trigger an `ESP_GAP_BLE_SEC_REQ_EVT` event on the master, which will reply a positive (true) security response to the peer device to accept the request or a negative (false) one to reject the request. In this example, this event is used to reply a start encryption response by using the `esp_ble_gap_security_rsp()` API call.

```c
case ESP_GAP_BLE_SEC_REQ_EVT:
     /* send the positive (true) security response to the peer device to accept the security request.
     If not accept the security request, should sent the security response with negative(false) accept value*/
     esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
     break;
```
## Conclusion

In this document, a review of the security aspects of the GATT Server has been realized. BLE security encompasses Pairing, Bonding and Encryption. In order to establish a secured link between a master and a slave device, security parameters that define the capabilities and features each device possess are set. The combination of features and capabilities of the peer devices results in the selection of the appropriate pairing method which the BLE stack then executes. Immediately, the required keys are generated and exchanged, and the encryption of subsequent messages is started using the AES-128 engine and these keys. These steps trigger different events that are managed by the GATT and GAP event handlers which can be used to print useful information such as the types of keys exchanged and the pairing status. In addition, attribute permissions are appointed to allow only encrypted read and write events when needed. The rest of the Security GATT server functionalities such as defining services and characteristics are implemented in the same way as presented in the GATT Server example.