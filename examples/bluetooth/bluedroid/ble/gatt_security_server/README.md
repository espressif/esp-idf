ESP-IDF Gatt Security Server Demo
========================

This is the demo of APIs to connect to and encrypt with peer devices.

To test this demo, you can run [gatt_security_client_demo](../gatt_security_client), which starts scanning, connects to and starts encryption with `gatt_security_server_demo` automatically.

There are some important points for this demo:  
1.`esp_ble_gap_set_security_param` should be used to set the security parameters in the initial stage;  
2.`esp_ble_set_encryption` should be used to start encryption with peer device. If the peer device initiates the encryption, `esp_ble_gap_security_rsp` should be used to send security response to the peer device when `ESP_GAP_BLE_SEC_REQ_EVT` is received.  
3.The `gatt_security_client_demo` will receive a `ESP_GAP_BLE_AUTH_CMPL_EVT` once the encryption procedure has completed.  

Please check the [tutorial](tutorial/Gatt_Security_Server_Example_Walkthrough.md) for more information about this example.

