ESP-IDF GATT SECURITY SERVICE demo
========================

This is the demo for user to use ESP BLE security API to connection with peer device & communication.
1.Should used the esp_ble_gap_set_security_param API to set the security parameter to the BLE stack in the init stage;
2.Used the esp_ble_set_encryption API to start encryption with peer device, if the peer device take the initiative encryption, should used the esp_ble_gap_security_rsp API to sent response to peer device when receive the ESP_GAP_BLE_SEC_REQ_EVT.
3.It will receive the ESP_GAP_BLE_AUTH_CMPL_EVT event when encryption finish will peer device.


