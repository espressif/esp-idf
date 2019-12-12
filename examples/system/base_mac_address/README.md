# Base MAC Address

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Each network interface on the ESP32 (e.g., WiFi Station/AP, BT, Ethernet) must be assigned a unique Medium Access Control (MAC) address. Each interface's MAC address is [derived from a base MAC address](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/system.html#mac-address) that must be set before any network interface's initialization is called.

This example demonstrates the following:

1. How to retrieve a base MAC address from non-volatile memory
2. How to set the base MAC address
3. How to obtain the derived MAC addresses of each network interface

This example utilizes the [MAC Address API](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/system.html#mac-address). 

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board. However, if an external source of non-volatile memory is used to store the base MAC address (e.g. an I2C Serial EEPROM), the user should wire the connections to their chosen external storage as needed.

### Configure the project

```
idf.py menuconfig
```

* To select the storage source of the base MAC address, go to `Example Configuration > Storage location of the base MAC address` where the following options are available:
  * `eFuse BLK0` will cause this example to read the base MAC address from eFuse Block 0 and is selected by default. The `eFuse BLK0` MAC address is factory programmed into each ESP32.
  * `eFuse BLK3` will cause this example to read the base MAC address from words 1 & 2 of eFuse Block 3. `eFuse BLK3` allows users to use a custom eFuse, but must be burned into the ESP32 using the [espefuse tool](https://github.com/espressif/esptool/wiki/espefuse). Attempting to read `eFuse BLK3` without burning the eFuse will result in an error.
  * `Other external storage` will call a `external_storage_mac_get()` which will merely retrieve an arbitrary base MAC address that is set in software. Users should re-implement `external_storage_mac_get()` to access their chosen external storage (e.g. an I2C Serial EEPROM)

* If `eFuse BLK3` is chosen, the `Behavior when retrieving BLK3 eFuse fails` option may also be selected. When retrieving the base MAC address from `eFuse BLK3` without burning eFuse Block 3, the example can either abort or default to retrieving from eFuse Block 0.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Depending on the `Storage of the base MAC address` configuration option, the console should output one of the following logs to indicate the source of the base MAC address.

```
I (288) BASE_MAC: Base MAC Address read from EFUSE BLK0
...
I (288) BASE_MAC: Base MAC Address read from EFUSE BLK3
...
I (288) BASE_MAC: Base MAC Address read from external storage
```

The console will also output the retrieved 6 byte MAC address when it sets it as the system wide base MAC address.
```
I (298) BASE_MAC: Using "0xd8, 0xa0, 0x1d, 0x0, 0xb, 0x88" as base MAC address
```

The console will then output the derived MAC address of each network interface as such:
```
I (297) WIFI_STA MAC: 0x0, 0x11, 0x22, 0x33, 0x44, 0x55
I (307) SoftAP MAC: 0x0, 0x11, 0x22, 0x33, 0x44, 0x56
I (317) BT MAC: 0x0, 0x11, 0x22, 0x33, 0x44, 0x57
I (317) Ethernet MAC: 0x0, 0x11, 0x22, 0x33, 0x44, 0x58
```

## Troubleshooting

When attempting to retrieve the base MAC address from `eFuse BLK3` without first burning eFuse Block 3 will result in one of the following situations.

If the example has been configured to abort when retrieving from `eFuse BLK3` fails.

```
E (288) system_api: Base MAC address from BLK3 of EFUSE version error, version = 0
E (298) BASE_MAC: Failed to get base MAC address from EFUSE BLK3. (ESP_ERR_INVALID_VERSION)
E (308) BASE_MAC: Aborting
abort() was called at PC 0x400d237b on core 0
```

If not configured to abort, the example will default to `eFuse BLK0` and output the following instead.

```
E (288) system_api: Base MAC address from BLK3 of EFUSE version error, version = 0
E (298) BASE_MAC: Failed to get base MAC address from EFUSE BLK3. (ESP_ERR_INVALID_VERSION)
I (308) BASE_MAC: Defaulting to base MAC address in BLK0 of EFUSE
I (308) BASE_MAC: Base MAC Address read from EFUSE BLK0
```