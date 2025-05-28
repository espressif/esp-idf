| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Bluedroid Beacon Example

## Overview

This is a pretty simple example, aiming to introduce

1. How to initialize Bluedroid stack
2. How to configure advertisement and scan response data
3. How to start advertising as a non-connectable beacon

It uses ESP32's Bluetooth controller and Bluedroid host stack.

To test this demo, any BLE scanner application can be used.

## Try It Yourself

### Set Target

Before project configuration and build, be sure to set the correct chip target using:

``` shell
idf.py set-target <chip_name>
```

For example, if you're using ESP32, then input

``` Shell
idf.py set-target esp32
```

### Build and Flash

Run the following command to build, flash and monitor the project.

``` Shell
idf.py -p <PORT> flash monitor
```

For example, if the corresponding serial port is `/dev/ttyACM0`, then it goes

``` Shell
idf.py -p /dev/ttyACM0 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Code Explained

### Overview

1. Initialize NVS flash, Bluedroid host stack and GAP service; configure Bluedroid host stack and start Bluedroid host task thread
2. Set advertisement and scan response data, then configure advertising parameters and start advertising

### Entry Point

`app_main` in `main.c` is the entry point of all ESP32 applications. In general, application initialization should be done here.

First, call `nvs_flash_init`, `esp_bt_controller_init` and `esp_bt_controller_enable` functions to initialize NVS flash as well as the BT controller.

``` C
void app_main(void) {
        esp_err_t ret;

    //initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
        return;
    }
    ...
}
```

Then, call `esp_bluedroid_init` and `esp_bluedroid_enable` function to initialize Bluedroid host stack. 

``` C
void app_main(void) {
    ...

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(DEMO_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
        return;
    }

    ...
}
```

After that, call `esp_ble_gap_register_callback` to register `esp_gap_cb` function as GAP service callback function. From then on all GAP events will be handled by `esp_gap_cb` function.

``` C
void app_main(void) {
    ...
    
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "gap register error, error code = %x", ret);
        return;
    }

    ...
}
```

### Start Advertising

As a beacon device, we're going to start advertising and send scan response if a scan request is received. To make it happen, we need to set advertisement and scan response data before advertising starts. So the following are what we do:

1. Initialize advertisement and scan response fields structs `adv_raw_data` and `scan_rsp_raw_data`, as well as advertising parameters struct `adv_params`.
2. Set advertising parameters based on your requirements.
    1. advertising interval is set to 20 ms.
    2. advertising PDU is set to `ADV_SCAN_IND`.
    3. advertising address type is public address.
    4. advertising channel is set to all channels. Channels 37, 38 and 39 will all be used for advertising.
3. Set advertising raw data and scan response raw data.

``` C
static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,  // 20ms
    .adv_int_max = 0x20,  // 20ms
    .adv_type = ADV_TYPE_SCAN_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

//configure raw data for advertising packet
static uint8_t adv_raw_data[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x11, ESP_BLE_AD_TYPE_NAME_CMPL, 'B', 'l', 'u', 'e', 'd', 'r', 'o', 'i', 'd', '_', 'B', 'e', 'a', 'c', 'o', 'n',
    0x02, ESP_BLE_AD_TYPE_TX_PWR, 0x09,
    0x03, ESP_BLE_AD_TYPE_APPEARANCE, 0x00,0x02,
    0x02, ESP_BLE_AD_TYPE_LE_ROLE, 0x00,
};

static uint8_t scan_rsp_raw_data[] = {
    0x08, ESP_BLE_AD_TYPE_LE_DEV_ADDR, 0x46, 0xF5, 0x06, 0xBD, 0xF5, 0xF0, 0x00,
    0x11, ESP_BLE_AD_TYPE_URI, 0x17, 0x2F, 0x2F, 0x65, 0x73, 0x70, 0x72, 0x65, 0x73, 0x73, 0x69, 0x66, 0x2E, 0x63, 0x6F, 0x6D,
};

```
4. Config advertising raw data with `esp_ble_gap_config_adv_data_raw`. Set device address in response raw data and call `esp_ble_gap_config_scan_rsp_data_raw` to configure advertising raw data.
    1. Since `AdvData` in advertisement packet **should not be longer than 31 bytes**, additional information must be placed in scan response packet
    2. We put the official website link of espressif into URI field
    3. Take care of endianness of device address

``` C
void app_main(void) {
    ...
    
    adv_config_done |= ADV_CONFIG_FLAG;
    adv_config_done |= SCAN_RSP_CONFIG_FLAG;
    ret = esp_ble_gap_config_adv_data_raw(adv_raw_data, sizeof(adv_raw_data));
    if (ret) {
        ESP_LOGE(DEMO_TAG, "config adv data failed, error code = %x", ret);
        return;
    }

    ret = esp_ble_gap_get_local_used_addr(local_addr, &local_addr_type);
    if (ret) {
        ESP_LOGE(DEMO_TAG, "get local used address failed, error code = %x", ret);
        return;
    }

    scan_rsp_raw_data[2] = local_addr[5];
    scan_rsp_raw_data[3] = local_addr[4];
    scan_rsp_raw_data[4] = local_addr[3];
    scan_rsp_raw_data[5] = local_addr[2];
    scan_rsp_raw_data[6] = local_addr[1];
    scan_rsp_raw_data[7] = local_addr[0];
    ret = esp_ble_gap_config_scan_rsp_data_raw(scan_rsp_raw_data, sizeof(scan_rsp_raw_data));
    if (ret) {
        ESP_LOGE(DEMO_TAG, "config scan rsp data failed, error code = %x", ret);
    }
}
```

4. When both advertising raw data and scan response raw data are successfully set, start advertising by calling `esp_ble_gap_start_advertising`

``` C
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ...
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Advertising data raw set, status %d", param->adv_data_raw_cmpl.status);
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        ESP_LOGI(DEMO_TAG, "Scan response data raw set, status %d", param->scan_rsp_data_raw_cmpl.status);
        adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
        if (adv_config_done == 0) {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
}
```

5. If advertising has been successfully enabled, you should receive `ESP_GAP_BLE_ADV_START_COMPLETE_EVT` GAP event.
``` C
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ...
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(DEMO_TAG, "Advertising start failed, status %d", param->adv_start_cmpl.status);
            break;
        }
        ESP_LOGI(DEMO_TAG, "Advertising start successfully");
        break;
}
```

### Observation

If everything goes well, you should be able to see `Bluedroid_Beacon` on a BLE scanner device, broadcasting a lot of information including an URI of "https://espressif.com" (The official website of espressif), which is exactly what we expect.

## Troubleshooting

For any technical queries, please file an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
