# Bluedroid host Examples for BLE_50

Note: To use examples in this directory, you need to have BLE enabled in configuration and Bluedroid selected as the host stack.

# Example Layout

The examples are grouped into subdirectories by category. Each category directory contains one or more example projects:
* `ble50_security_client` Demonstrates how to use the ESP BLE5.0 security APIs to securely connect with a peer device and encrypt data exchanges. See the [ble50_security_client](./ble50_security_client/README.md) about example.
* `ble50_security_server` Demonstrates how to securely connect with a peer device and use encryption for data exchange using ESP32-C2, ESP32-C3, ESP32-C6, ESP32-H2, and ESP32-S3 targets. See the [ble50_security_server](./ble50_security_server/README.md) about example.
* `multi-adv` Demonstrates how to support both legacy and extended advertisement for all phy on various ESP32 chips, including ESP32-C2, ESP32-C3, ESP32-C6, ESP32-H2, and ESP32-S3. See the [multi-adv](./multi-adv/README.md) about example.
* `peroidic_adv` Demonstrates how to support periodic advertisement, allowing a scanner to sync with an advertiser, and supports extended adv with 2M phy in connectable mode on ESP32-C2, ESP32-C3, ESP32-C6, ESP32-H2, and ESP32-S3 targets. See the [peroidic_adv](./peroidic_adv/README.md) about example.
* `peroidic_sync` Demonstrates how to conduct a periodic extended scan for extended advertisements on various ESP32 chips, providing instructions for project configuration, building, and flashing, as well as troubleshooting tips. See the [peroidic_sync](./peroidic_sync/README.md) about example.

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.