| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# WiFi NVS Config Example

(See the `README.md` file in the upper-level 'examples' directory for more information about examples.)

This example demonstrates how to configure Wi-Fi settings in NVS directly using a CSV file and utilize the Wi-Fi functionality of the ESP Wi-Fi driver.

## How to Use the Example

### 1) Configuration

Open the CSV file `nvs_station_data.csv` or `nvs_ap_data.csv`.

- Set the Wi-Fi configuration keys as explained below. These values are written into the NVS partition and read by the Wi-Fi driver during `esp_wifi_init()`.
- If needed, adjust or set other options as per your requirements. The AP and station keys along with their encoding types are listed [here](#key-configuration).

**Note 1:** To set the device in SoftAP mode, enable the `CONFIG_ESP_WIFI_SOFTAP_SUPPORT` flag in `menuconfig`.

**Note 2:** Setting these values is optional. Defaults are applied when `esp_wifi_init()` is called.

### 2) Generate the Binary File Using the NVS Partition Generator Utility

- Check the size of the NVS partition in the `partition_example.csv` file provided in this example.
- Generate a `.bin` file of the specified size using the following command:

**Usage:**

```bash
python nvs_partition_gen.py generate [-h] [--version {1,2}] [--outdir OUTDIR] input output size
```

- Sample command:

```bash
python nvs_partition_gen.py generate nvs_station_data.csv sample_nvs.bin 0x6000
```

- Please update the NVS Partition Generator to the latest version to avoid encoding type errors.
- For more information, see the [NVS Partition Generator Utility Documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_partition_gen.html).

### 3) Flash the NVS Binary File to the ESP Device

- Check the offset of the NVS partition in the `partition_example.csv` file.
- After flashing the build, flash the `sample_nvs.bin` file to the ESP device at the specified offset using `esptool.py`.
- Refer to the [Esptool Documentation](https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/index.html) for detailed usage.

### 4) Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output.

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

### 5) Monitor the Output

- Use `idf.py monitor` to view the console output of the example.

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html)

## Key Configuration

### Station Keys


  | Key Name                 | Encoding Type | Mandatory/Optional  | Description / Meaning                              |
  |--------------------------|---------------|---------------------|----------------------------------------------------|
  | sta.ssid                 | blob_sz_fill  | Mandatory           | SSID (Service Set Identifier) for station. (Refer parameter `uint8_t ssid[32]` from `struct wifi_sta_config_t`)      |
  | sta.pswd                 | blob_fill     | Mandatory           | Password for connecting to the Wi-Fi network. (Refer parameter `uint8_t password[64]` from `struct wifi_sta_config_t`)      |
  | bssid.set                | u8            | Optional            | Whether the BSSID is set or not. (Refer parameter `bool bssid_set` from `struct wifi_sta_config_t`)                  |
  | sta.bssid                | blob          | Optional            | BSSID (Basic Service Set Identifier) of the network. (Refer parameter `uint8_t bssid[6]` from `struct wifi_sta_config_t`) |
  | sta.lis_intval           | u16           | Optional            | Listen interval (Refer parameter `uint16_t listen_interval` from `struct wifi_sta_config_t`)           |
  | sta.phym                 | u8            | Optional            | Physical mode e.g., 802.11a/b/g/n/ac/ax. (Refer `esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap)`)            |
  | sta.phybw                | u8            | Optional            | Physical bandwidth (e.g., 20MHz, 40MHz) (Refer `esp_err_t esp_wifi_set_bandwidth(wifi_interface_t ifx, wifi_bandwidths_t* bw)`)           |
  | sta.sort_method          | u8            | Optional            | Sorting method for APs e.g. enum wifi_sort_method_t (Refer parameter `wifi_sort_method_t sort_method` from `struct wifi_sta_config_t`)|
  | sta.minrssi              | i8            | Optional            | Minimum RSSI for the station to connect. (Refer `esp_err_t esp_wifi_set_rssi_threshold()` OR `int8_t rssi` from `wifi_scan_threshold_t threshold`)       |
  | sta.minauth              | u8            | Optional            | Minimum authentication level required for APs. (Refer `wifi_auth_mode_t authmode` from `wifi_scan_threshold_t threshold`)  |
  | sta.pmf_r                | u8            | Optional            | Whether PMF is required. (Refer `wifi_pmf_config_t pmf_cfg` parameter from `struct wifi_sta_config_t`)                          |
  | sta.btm_e                | u8            | Optional            | Whether BTM is enabled. (Refer parameter `uint32_t btm_enabled` from `struct wifi_sta_config_t`)                           |
  | sta.rrm_e                | u8            | Optional            | Whether RRM is enabled. (Refer parameter `uint32_t rm_enabled` from `struct wifi_sta_config_t`)                           |
  | sta.mbo_e                | u8            | Optional            | Whether MBO (Multi-band Operation) is enabled. (Refer parameter `uint32_t mbo_enabled` from `struct wifi_sta_config_t`)     |
  | sta.phym5g               | u8            | Optional            | 5 GHz physical mode. (Only for chips supporting 5GHz bandwidth. (Refer `esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap)`)  |
  | sta.phybw5g              | u8            | Optional            | 5 GHz bandwidth setting.(Only for chips supporting 5GHz bandwidth. (Refer `esp_err_t esp_wifi_set_bandwidths(wifi_interface_t ifx, wifi_bandwidths_t* bw)` |
  | sta.ft                   | u8            | Optional            | Whether fast transition is enabled. (Refer parameter `uint32_t ft_enabled` from `struct wifi_sta_config_t`)               |
  | sta.owe                  | u8            | Optional            | Whether OWE (Opportunistic Wireless Encryption) is enabled. (Refer parameter `uint32_t owe_enabled` from `struct wifi_sta_config_t`)  |
  | sta.trans_d              | u8            | Optional            | Transition disabled setting. (Refer parameter `uint32_t transition_disable` from `struct wifi_sta_config_t`)                    |
  | sta.sae_h2e              | u8            | Optional            | SAE PWE method (e.g. enum wifi_sae_pwe_method_t). (Refer parameter `wifi_sae_pwe_method_t sae_pwe_h2e` from `struct wifi_sta_config_t`) |
  | sta.sae_pk_mode          | u8            | Optional            | SAE-pk mode setting (e.g. enum wifi_sae_pk_mode_t) (Refer parameter `wifi_sae_pk_mode_t sae_pk_mode` from `struct wifi_sta_config_t`)         |
  | sta.bss_retry            | u8            | Optional            | BSS retry counter. (Refer parameter `uint8_t failure_retry_cnt` from `struct wifi_sta_config_t`)                               |
  | sta.sae_h2e_id           | blob          | Optional            | Password identifier for H2E. This must be a null-terminated string (Refer parameter `uint8_t sae_h2e_identifier[SAE_H2E_IDENTIFIER_LEN]` from `struct wifi_sta_config_t`)          |
  | sta.rssi_5g_adj          | u8            | Optional            | RSSI adjustment for 5 GHz. (Refer `uint8_t rssi_5g_adjustment` from `wifi_scan_threshold_t threshold`)      |


### AP Keys

  | Key Name                 | Encoding Type | Mandatory/Optional  | Description / Meaning                              |
  |--------------------------|---------------|---------------------|----------------------------------------------------|
  | ap.ssid                  | blob_sz_fill  | Mandatory           | SSID (Service Set Identifier) for the AP prefixed with the length of the SSID. (Refer parameter `uint8_t ssid[32]` from `struct wifi_ap_config_t`)        |
  | ap.passwd                | blob_fill     | Mandatory           | Password for the AP's Wi-Fi network. (Refer parameter `uint8_t password[64]` from `struct wifi_ap_config_t`)              |
  | ap.chan                  | u8            | Mandatory           | Channel number on which the AP is broadcasting. (Refer parameter `uint8_t channel` from `struct wifi_ap_config_t`)   |
  | ap.authmode              | u8            | Mandatory           | Authentication mode for the AP e.g. enum wifi_auth_mode_t. (Refer parameter `wifi_auth_mode_t authmode` from `struct wifi_ap_config_t`)  |
  | ap.hidden                | u8            | Optional            | Whether the AP is hidden (1 for hidden, 0 for visible). (Refer parameter `uint8_t ssid_hidden` from `struct wifi_ap_config_t`) |
  | ap.max.conn              | u8            | Optional            | Maximum number of connections the AP supports. It is different for different chips.(Please refer parameter `uint8_t max_connection` from `struct wifi_ap_config_t` and [AP Basic Configurations](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#ap-basic-configuration) for more details).    |
  | bcn.interval             | u8            | Optional            | Beacon interval time for the AP. (Refer parameter `uint16_t beacon_interval` from `struct wifi_ap_config_t`))                 |
  | ap.pmf_r                 | u8            | Optional            | Whether PMF is required on the AP. (Refer parameter `wifi_pmf_config_t pmf_cfg` from `struct wifi_ap_config_t`))               |
  | ap.p_cipher              | u8            | Optional            | Pairwise Cipher suite used by the AP e.g. enum wifi_cipher_type_t (Refer parameter `wifi_cipher_type_t pairwise_cipher` from `struct wifi_ap_config_t`)  |
  | ap.ftm_r                 | u8            | Optional            | Whether FTM (Fine Timing Measurement) responder is enabled. (Refer parameter `bool ftm_responder` from `struct wifi_ap_config_t`)  |
  | ap.sae_h2e               | u8            | Optional            | Config method for SAE e.g. enum `wifi_sae_pwe_method_t` (Refer parameter `wifi_sae_pwe_method_t sae_pwe_h2e` from `struct wifi_ap_config_t`) |
  | ap.csa_count             | u8            | Optional            | Channel Switch Announcement count for the AP. (Refer parameter `uint8_t csa_count` from `struct wifi_ap_config_t`)    |
  | ap.dtim_period           | u8            | Optional            | DTIM period for the AP. (Refer parameter `uint8_t dtim_period` from `struct wifi_ap_config_t`)                           |
  | ap.phym5g                | u8            | Optional            | 5 GHz physical mode. (Refer `esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap)`)                               |
  | ap.phybw5g               | u8            | Optional            | 5 GHz bandwidth setting. (Refer `esp_err_t esp_wifi_set_bandwidths(wifi_interface_t ifx, wifi_bandwidth_t bw)`)                           |

### Generic Keys

  | Key Name                 | Encoding Type | Mandatory/Optional  | Description / Meaning                              |
  |--------------------------|---------------|---------------------|----------------------------------------------------|
  | opmode                   | u8            | Mandatory           | Operating mode (e.g. enum `wifi_mode_t` OR refer `esp_err_t esp_wifi_set_mode(wifi_mode_t mode)`)      |
  | country                  | blob          | Optional            | Operating country (e.g. enum `wifi_country_t` OR refer `esp_err_t esp_wifi_set_country_code(const char *country, bool ieee80211d_enabled)`)      |
  | band_mode                | u8            | Optional            | Operating band (e.g. enum `wifi_band_t` or refer [esp_err_t esp_wifi_set_band_mode(wifi_band_mode_t band_mode)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html#_CPPv422esp_wifi_set_band_mode16wifi_band_mode_t))      |

These keys can be configured by setting the desired values in `nvs_station_data.csv` or `nvs_ap_data.csv`.

For more details on station and AP configurations, refer to the following:

- [Station Basic Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#station-basic-configuration)
- [AP Basic Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/wifi.html#ap-basic-configuration)

Also review the limitations for values of each key in the [ESP-IDF API Reference Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html).


## Example Output
Note that the output, in particular the order of the output, may vary depending on the environment.

Console output if station connects to AP successfully:
```
I (599) wifi: wifi driver task: 3ffc08b4, prio:23, stack:3584, core=0
I (599) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (599) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (629) wifi: wifi firmware version: 2d94f02
I (629) wifi: config NVS flash: enabled
I (629) wifi: config nano formatting: disabled
I (629) wifi: Init dynamic tx buffer num: 32
I (629) wifi: Init data frame dynamic rx buffer num: 32
I (639) wifi: Init management frame dynamic rx buffer num: 32
I (639) wifi: Init management short buffer num: 32
I (649) wifi: Init static rx buffer size: 1600
I (649) wifi: Init static rx buffer num: 10
I (659) wifi: Init dynamic rx buffer num: 32
I (759) phy: phy_version: 4180, cb3948e, Sep 12 2019, 16:39:13, 0, 0
I (769) wifi: mode : sta (30:ae:a4:d9:bc:c4)
I (769) wifi station: wifi_init_sta finished.
I (889) wifi: new:<6,0>, old:<1,0>, ap:<255,255>, sta:<6,0>, prof:1
I (889) wifi: state: init -> auth (b0)
I (899) wifi: state: auth -> assoc (0)
I (909) wifi: state: assoc -> run (10)
I (939) wifi: connected with #!/bin/test, aid = 1, channel 6, BW20, bssid = ac:9e:17:7e:31:40
I (939) wifi: security type: 3, phy: bgn, rssi: -68
I (949) wifi: pm start, type: 1

I (1029) wifi: AP's beacon interval = 102400 us, DTIM period = 3
I (2089) esp_netif_handlers: sta ip: 192.168.77.89, mask: 255.255.255.0, gw: 192.168.77.1
I (2089) wifi station: got ip:192.168.77.89
I (2089) wifi station: connected to ap SSID:myssid password:mypassword
```

Console output if the station failed to connect to AP:
```
I (599) wifi: wifi driver task: 3ffc08b4, prio:23, stack:3584, core=0
I (599) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (599) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (629) wifi: wifi firmware version: 2d94f02
I (629) wifi: config NVS flash: enabled
I (629) wifi: config nano formatting: disabled
I (629) wifi: Init dynamic tx buffer num: 32
I (629) wifi: Init data frame dynamic rx buffer num: 32
I (639) wifi: Init management frame dynamic rx buffer num: 32
I (639) wifi: Init management short buffer num: 32
I (649) wifi: Init static rx buffer size: 1600
I (649) wifi: Init static rx buffer num: 10
I (659) wifi: Init dynamic rx buffer num: 32
I (759) phy: phy_version: 4180, cb3948e, Sep 12 2019, 16:39:13, 0, 0
I (759) wifi: mode : sta (30:ae:a4:d9:bc:c4)
I (769) wifi station: wifi_init_sta finished.
I (889) wifi: new:<6,0>, old:<1,0>, ap:<255,255>, sta:<6,0>, prof:1
I (889) wifi: state: init -> auth (b0)
I (1889) wifi: state: auth -> init (200)
I (1889) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (1889) wifi station: retry to connect to the AP
I (1899) wifi station: connect to the AP fail
I (3949) wifi station: retry to connect to the AP
I (3949) wifi station: connect to the AP fail
I (4069) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (4069) wifi: state: init -> auth (b0)
I (5069) wifi: state: auth -> init (200)
I (5069) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (5069) wifi station: retry to connect to the AP
I (5069) wifi station: connect to the AP fail
I (7129) wifi station: retry to connect to the AP
I (7129) wifi station: connect to the AP fail
I (7249) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (7249) wifi: state: init -> auth (b0)
I (8249) wifi: state: auth -> init (200)
I (8249) wifi: new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (8249) wifi station: retry to connect to the AP
I (8249) wifi station: connect to the AP fail
I (10299) wifi station: connect to the AP fail
I (10299) wifi station: Failed to connect to SSID:myssid, password:mypassword
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
