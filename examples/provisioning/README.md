# Provisioning Application Examples

These consist of the following examples :

* softap_prov
    Provisioning involves Wi-Fi station configuration via an HTTP server running on the device, which is initially configured to be in SoftAP mode. After provisioning, device runs in Wi-Fi station mode only and connects to the AP whose credentials were provided during provisioning.

* ble_prov
    Provisioning involves Wi-Fi station configuration via BLE service endpoints running on the device initially. After provisioning, BLE is turned off and device runs in Wi-Fi station mode, connecting to the AP whose credentials were provided during provisioning.

* console_prov
    Provisioning involves Wi-Fi station configuration via UART console. This is intended for debugging protocomm and provisioning related features.

* custom_config
    Similar to softap_prov examples, but allows for configuration of custom (device-local) information during provisioning. This is intended as an example for implementing custom provisioning schemes.

A python based utility is provided under "$IDF_PATH/tools/esp_prov" for testing the examples over a host.

Refer to the README.md files in each example directory for more information.
