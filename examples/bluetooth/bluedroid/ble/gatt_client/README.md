| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# ESP-IDF Gatt Client Example

This example shows how to use ESP APIs to create a GATT Client.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

To test this example, you first run the [gatt_server_demo](../gatt_server), which creates services and starts advertising. `Gatt_client_demo` will start scanning and connect to the `gatt_server_demo` automatically.

This example will enable gatt server's notification function once the connection is established and then the devices start exchanging data.

Please, check this [tutorial](tutorial/Gatt_Client_Example_Walkthrough.md) for more information about this example.

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-H2/ESP32-C2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

### Settings for UUID128

This example works with UUID16 as default. To change to UUID128, follow this steps:

1. Change the UIID16 to UUID128. You can change the UUID according to your needs.

```c
// Create a new UUID128 (using random values for this example)
static uint8_t gatts_xxx_uuid128[ESP_UUID_LEN_128] = {0x06, 0x18, 0x7a, 0xec, 0xbe, 0x11, 0x11, 0xea, 0x00, 0x16, 0x02, 0x42, 0x01, 0x13, 0x00, 0x04};
```
By adding this new UUID128, you can remove the `#define` macros with the old UUID16.

2. Change the structure to:

```c
static esp_bt_uuid_t xxx_uuid = {
    .len = ESP_UUID_LEN_128,
    .uuid = {.uuid128 = { 0 },},
};
```

3. Add the new UUID128 to the profile.

```c
// Copy the new UUID128 to the profile
memcpy(xxx_uuid.uuid.uuid128, gatts_xxx_uuid128, ESP_UUID_LEN_128);
```

4. Edit the `ESP_GATTC_SEARCH_RES_EVT` in order to filter the new UUID128.

```c
case ESP_GATTC_SEARCH_RES_EVT: {
    ESP_LOGI(GATTC_TAG, "SEARCH RES: conn_id = %x is primary service %d", p_data->search_res.conn_id, p_data->search_res.is_primary);
    ESP_LOGI(GATTC_TAG, "start handle %d end handle %d current handle value %d", p_data->search_res.start_handle, p_data->search_res.end_handle, p_data->search_res.srvc_id.inst_id);
    if (p_data->search_res.srvc_id.uuid.len == ESP_UUID_LEN_128) {
        if(memcmp(p_data->search_res.srvc_id.uuid.uuid.uuid128, gatts_xxx_uuid128, ESP_UUID_LEN_128) == 0){
            ESP_LOGI(GATTC_TAG, "service uuid128 found");
            get_server = true;
            gl_profile_tab[PROFILE_X_APP_ID].service_start_handle = p_data->search_res.start_handle;
            gl_profile_tab[PROFILE_X_APP_ID].service_end_handle = p_data->search_res.end_handle;
        } else {
            ESP_LOGE(GATTC_TAG, "service not found");
        }
    }
    break;
```

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (525) BTDM_INIT: BT controller compile version [1342a48]
I (525) system_api: Base MAC address is not set
I (525) system_api: read default base MAC address from EFUSE
I (535) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (945) GATTC_DEMO: REG_EVT
I (955) GATTC_DEMO: scan start success
I (1115) GATTC_DEMO: 08 ef 3b a7 04 41
I (1115) GATTC_DEMO: searched Adv Data Len 9, Scan Response Len 15
I (1115) GATTC_DEMO: searched Device Name Len 13
I (1125) GATTC_DEMO: LG CM2760(41)
I (1125) GATTC_DEMO:

I (1425) GATTC_DEMO: 08 ef 3b a7 04 41
I (1425) GATTC_DEMO: searched Adv Data Len 9, Scan Response Len 15
I (1425) GATTC_DEMO: searched Device Name Len 13
I (1435) GATTC_DEMO: LG CM2760(41)
I (1435) GATTC_DEMO:

I (1865) GATTC_DEMO: 38 68 a4 69 bb 7c
I (1865) GATTC_DEMO: searched Adv Data Len 31, Scan Response Len 14
I (1865) GATTC_DEMO: searched Device Name Len 0
I (1875) GATTC_DEMO:

I (2185) GATTC_DEMO: 38 68 a4 69 bb 7c
I (2185) GATTC_DEMO: searched Adv Data Len 31, Scan Response Len 14
I (2185) GATTC_DEMO: searched Device Name Len 0
I (2185) GATTC_DEMO:
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
