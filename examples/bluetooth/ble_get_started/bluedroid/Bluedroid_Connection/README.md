| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Bluedroid Connection Example

## Overview

This example is extended from Bluedroid Beacon Example, and further introduces

1. How to advertise as a connectable peripheral device
2. How to capture GAP events and handle them
3. How to update connection parameters

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

1. Initialize NVS flash, Bluedroid host stack, GAP and GATT service; configure Bluedroid Host stack
2. Set advertisement and scan response data, then configure advertising parameters and start advertising
3. On connect event
    1. Print out connection descriptions
    2. Update connection parameters
4. On connection update event
    1. Print out connection descriptions
5. On disconnect event
    1. Print out connection descriptions

### Entry Point

Please refer to the Bluedroid Beacon Example for details.

### Start Advertising

Please refer to the Bluedroid Beacon Example for details.

### On GAP & GATT Events

To keep it simple, we're interested in 1 GAP event and 3 GATT events at the moment

- `ESP_GATTS_REG_EVT` - GATT service registered event
- `ESP_GATTS_CONNECT_EVT` - GATT connection event
- `ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT` - Connection update event
- `ESP_GATTS_DISCONNECT_EVT` - GATT disconnection event

#### Register GATT service

In `app_main`, we register `gatts_event_handler` function as the callback function for all GATT services by calling `esp_ble_gatts_register_callback`. Then, we need to register a GATT server application with application ID 0 by calling `esp_ble_gatts_app_register`

``` C
void app_main(void)
{
    ...
    ret = esp_ble_gap_register_callback(esp_gap_cb);
    if (ret) {
        ESP_LOGE(CONN_TAG, "%s gap register failed, error code = %x", __func__, ret);
        return;
    }

    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(CONN_TAG, "%s gatts register failed, error code = %x", __func__, ret);
        return;
    }
    ...
}
```
#### Connect Event

When the device is connected to a peer device, a connect event will be passed to `gatts_event_handler` by Bluedroid host stack. Once connection has been established, we will call `esp_ble_gap_update_conn_params` to update connection parameters.
``` C
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    ...
    case ESP_GATTS_CONNECT_EVT:
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x20;
        conn_params.min_int = 0x10;
        conn_params.timeout = 400;
        ESP_LOGI(CONN_TAG, "Connected, conn_id %u, remote "ESP_BD_ADDR_STR"",
                param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda));
        esp_ble_gap_update_conn_params(&conn_params);
        break;
    ...
}
```

#### Connection Update Event

When connection parameters are updated, an `ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT` will be passed to `esp_gap_cb` by Bluedroid host stack. 
``` C
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ...
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(CONN_TAG, "Connection params update, status %d, conn_int %d, latency %d, timeout %d",
                 param->update_conn_params.status,
                 param->update_conn_params.conn_int,
                 param->update_conn_params.latency,
                 param->update_conn_params.timeout);
        break;
    ...
}
```
#### Disconnect Event

On `ESP_GATTS_DISCONNECT_EVT`, we simply 

1. Print out disconnect reason and connection descriptor
2. Re-start advertising

``` C
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    ...
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(CONN_TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
             ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    ...
}
```
## Observation

If everything goes well, except for what we have seen in Bluedroid Beacon example, you should be able to connect to and disconnect from the device successfully.

## Troubleshooting

For any technical queries, please file an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.