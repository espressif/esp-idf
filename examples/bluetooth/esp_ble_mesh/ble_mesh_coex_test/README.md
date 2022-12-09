| Supported Targets | ESP32 |
| ----------------- | ----- |

# Example of BLE Mesh and TCP Server/Client Coexistence

This example introduces how to test the basic functions of `BLE Mesh data interface` and `TCP Server/Client Coexistence`. `BLE Mesh data interface` is GAP scanning and advertising.

There are two working modes here:

 * In automatic mode, the program coordinates three development boards working through a synchronization mechanism.

 * In manual mode, you will work with three development boards via commands


## Test Preparation

* Before running the test, you need to prepare a router and three ESP32 development boards. This Example of BLE Mesh and TCP Server/Client Coexistence has the following three items, and any of the three development boards is for running one specific item.

  * ble_dev : Run only the BLE  program.
  * coex_dev: Run BLE  and Wi-Fi program.
  * wifi_dev: Run only the Wi-Fi program.

``Note: If you want better performance in BLE and WiFi coexistence, you should use a development board with PSRAM that could run a coexistence program. Such as ESP32_LyraT, ESP32-WROVER-B and etc.`` 

* The following structure shows the parameters you need to configure. And usually, there are two methods for configuration, i.e. configuring during initialization or configuring with the command `env`.

```c
coex_test_env_t test_env = {
#if defined(CONFIG_EXAMPLE_MANAUL)
    .ap_ssid = CONFIG_EXAMPLE_WIFI_SSID,
    .ap_password = CONFIG_EXAMPLE_WIFI_PASSWORD,
#endif
#if defined(CONFIG_EXAMPLE_COEX_ROLE)
    .ap_ssid = CONFIG_EXAMPLE_WIFI_SSID,
    .ap_password = CONFIG_EXAMPLE_WIFI_PASSWORD,
#endif
    .test_port = "8080",
    .server_ip = "192.168.3.32",
    .duration = "120000",
    .is_start = false,
};
```


## Run Test Case Manually
Configure to Manual Mode via `Example Configuration  --->run mode (manual) `

The meaning of the numeric argument of the command `run_tc` is as follows:

| id | case name | description |
|:-:|:-|:-|
| 0 | wifi_tcp_tx_throughput| Test the case of Wi-Fi tcp tx, which will create a tcp client that will continuously send data to the tcp server. |
| 1 |wifi_tcp_rx_throughput| Test the case of Wi-Fi tcp rx, which will create a tcp server that will continuously receive data from the tcp client.  |
| 2 | ble_adv  | Test the case of BLE advertising. |
| 3 | ble_scan| Test the case of BLE Scan.|


###  Case 1: tcp tx + scan  
1. wifi_dev: run_tc -w 1
2. coex_dev: env -s -k server_ip -v 192.168.3.34       run_tc -w 0 -b 3
3. ble_dev : run_tc -b 2	


### Case 2: tcp rx + scan
1.  coex_dev:  run_tc -w 1 -b 3 
2.  wifi_dev:  env -s -k server_ip -v 192.168.3.34     run_tc -w 0
3.  ble_dev :  run_tc -b 2

### Case 3: tcp tx + adv
1.  wifi_dev:  run_tc -w 1
2.  coex_dev:  env -s -k server_ip -v 192.168.3.13     run_tc -w 0 -b 2 
3.  ble_dev :  run_tc -b 3


### Case 4: tcp rx + adv
1. coex_dev:  run_tc -w 1 -b 2
2. wifi_dev:  env -s -k server_ip -v 192.168.3.34       run_tc -w 0
3. ble_dev :  run_tc -b 3

## Run Test Case By Automation
Configure to Automatic Mode via `Example Configuration  --->run mode (auto) `

### Coexistence device configuration
1. Select a development board as coexistence role by `Example Configuration  --->select role (run device as coex role) `
2. Select a test case by `Example Configuration  --->select case `. 
* There are four types of cases:
    * TCP TX and BLE ADV:  The TCP client will be created on the coexistence device, and bluetooth will start advertising when the Wi-Fi is running tx throughput program.
    * TCP RX and BLE ADV:  The TCP server will be created on the coexistence device, and bluetooth will start advertising when the Wi-Fi is running rx throughput program.
    * TCP TX and BLE SCAN: The TCP client will be created on the coexistence device, and bluetooth will start scanning when the Wi-Fi is running tx throughput program.
    * TCP RX and BLE SCAN: The TCP server will be created on the coexistence device, and bluetooth will start scanning when the Wi-Fi is running rx throughput program.

### Bluetooth device configuration
1. Select a development board as bluetooth role by `select role (run device as bluetooth role) `

### Wi-Fi device configuration
1. Select a development board as bluetooth role by `select role (run device as wifi role) `


## Coexistence Configuration
In theory, the performance of BLE and Wi-Fi coexistence will drop to half of the performance in BLE Only mode or Wi-Fi Only mode.

* ESP32 working frequency:
    * Component config  --->  ESP32-specific  --->  CPU frequency (240 MHz) 

* ESP32 external PSRAM
    * Component config  --->  ESP32-specific  --->  Support for external, SPI-connected RAM
    * Devices that do not support PSRAM cannot open this option!

* ESP32 coexistence mode
    * Component config  --->  Wi-Fi  --->   WiFi/Bluetooth coexistence performance preference (Balance)
