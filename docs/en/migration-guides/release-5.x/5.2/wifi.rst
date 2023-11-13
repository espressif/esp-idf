WiFi
====

:link_to_translation:`zh_CN:[中文]`


WiFi Enterprise Security
------------------------

APIs defined in `esp_wpa2.h` have been deprecated. Please use newer APIs from `esp_eap_client.h`.


WiFi Disconnect Reason Codes
----------------------------

For the event WIFI_EVENT_STA_DISCONNECTED, the original reason code WIFI_REASON_NO_AP_FOUND has been split as follows:

- REASON_NO_AP_FOUND(original and still used in some scenarios)
- REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD
- REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD
- REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY

For details, please refer to :ref:`esp_wifi_reason_code`.
