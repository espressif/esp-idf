# BLE Log Console

BLE Log Console was originally provided in the ESP-IDF repository as the host-side companion tool for the BLE Log feature. It collects BLE log data from ESP chip firmware, then parses, displays, and saves the logs on a PC. The tool works with the BLE Log firmware configuration in ESP-IDF and receives log frames over transports such as UART or SPI Bridge, helping developers inspect the runtime state, loss statistics, and raw log data of BLE components including Controller, Host, and Mesh.

To allow releases, dependency management, documentation, and feature development to move independently from the ESP-IDF main repository, BLE Log Console has been migrated out of ESP-IDF and is now maintained in a standalone repository.

New repository: [esp-ble-tools/components/ble_log_console](https://github.com/espressif/esp-ble-tools/components/ble_log_console)

## Current Directory

This directory is kept only as a migration notice. It no longer contains the BLE Log Console source code, build scripts, tests, or runtime dependencies.

Previous location:

```text
tools/bt/ble_log_console
```

Future development, issue tracking, releases, and documentation for BLE Log Console should use the standalone repository.

## Relationship With ESP-IDF

ESP-IDF still provides the firmware-side configuration and implementation for BLE Log. Users can continue to enable BLE Log through ESP-IDF menuconfig, then use BLE Log Console from the standalone repository to capture and analyze logs on a PC.

Common firmware configuration example:

```text
CONFIG_BT_LOG_CRITICAL_ONLY=y
CONFIG_BLE_LOG_PRPH_UART_DMA=y
CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE=3000000
```

For SPI Bridge and other transport-specific setup, as well as complete usage instructions, refer to the standalone repository documentation.
