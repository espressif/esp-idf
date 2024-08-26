| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# NimBLE Beacon Example

## Overview

This is a pretty simple example, aiming to introduce

1. How to initialize NimBLE stack
2. How to configure advertisement and scan response data
3. How to start advertising as a non-connectable beacon

It uses ESP32's Bluetooth controller and NimBLE host stack.

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

1. Initialize NVS flash, NimBLE host stack and GAP service; configure NimBLE host stack and start NimBLE host task thread
2. Wait for NimBLE host stack to sync with BLE controller
3. Set advertisement and scan response data, then configure advertising parameters and start advertising

### Entry Point

`app_main` in `main.c` is the entry point of all ESP32 applications. In general, application initialization should be done here.

First, call `nvs_flash_init` function to initialize NVS flash, which is the dependency for BLE module to store configurations.

``` C
void app_main(void) {
    /* Local variables */
    int rc;
    esp_err_t ret;

    /* NVS flash initialization */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nvs flash, error code: %d ", ret);
        return;
    }

    ...
}
```

Then, call `nimble_port_init` function to initialize NimBLE host stack. 

``` C
void app_main(void) {
    ...

    /* NimBLE host stack initialization */
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nimble stack, error code: %d ",
                 ret);
        return;
    }

    ...
}
```

After that, call `gap_init` defined in `gap.c`. We will initialize GAP service, set GAP device name and appearance in this function.

``` C
int gap_init(void) {
    /* Local variables */
    int rc = 0;

    /* Initialize GAP service */
    ble_svc_gap_init();

    /* Set GAP device name */
    rc = ble_svc_gap_device_name_set(DEVICE_NAME);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set device name to %s, error code: %d",
                 DEVICE_NAME, rc);
        return rc;
    }

    /* Set GAP device appearance */
    rc = ble_svc_gap_device_appearance_set(BLE_GAP_APPEARANCE_GENERIC_TAG);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set device appearance, error code: %d", rc);
        return rc;
    }
    return rc;
}

void app_main(void) {
    ...

    /* GAP service initialization */
    rc = gap_init();
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to initialize GAP service, error code: %d", rc);
        return;
    }

    ...
}
```

And we need to configure some callback functions for NimBLE host stack to call and store the configurations in `nimble_host_config_init` in `main.c`.

``` C
static void nimble_host_config_init(void) {
    /* Set host callbacks */
    ble_hs_cfg.reset_cb = on_stack_reset;
    ble_hs_cfg.sync_cb = on_stack_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Store host configuration */
    ble_store_config_init();
}

void app_main(void) {
    ...

    /* NimBLE host configuration initialization */
    nimble_host_config_init();

    ...
}
```

So far, initialization has been done. We can call `xTaskCreate` to create `nimble_host_task` thread, and let NimBLE host stack run in the background.

``` C
static void nimble_host_task(void *param) {
    /* Task entry log */
    ESP_LOGI(TAG, "nimble host task has been started!");

    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();

    /* Clean up at exit */
    vTaskDelete(NULL);
}

void app_main(void) {
    ...

    /* Start NimBLE host task thread and return */
    xTaskCreate(nimble_host_task, "NimBLE Host", 4*1024, NULL, 5, NULL);
    return;   
}
```

### On Stack Sync

Once NimBLE host stack is synced with BLE controller, `on_stack_sync` in `gap.c` will be called by NimBLE host stack, which has been configured in `nimble_host_config_init`.

In this function, we will call `adv_init` function to ask NimBLE host stack to check if device MAC address is available by `ble_hs_util_ensure_addr` and `ble_hs_id_infer_auto` functions. If so, we will copy the address and try to start advertising by calling `start_advertising` in the same source file.

``` C
static void on_stack_sync(void) {
    /* On stack sync, do advertising initialization */
    adv_init();
}

void adv_init(void) {
    /* Local variables */
    int rc = 0;
    char addr_str[12] = {0};

    /* Make sure we have proper BT identity address set */
    rc = ble_hs_util_ensure_addr(0);
    if (rc != 0) {
        ESP_LOGE(TAG, "device does not have any available bt address!");
        return;
    }

    /* Figure out BT address to use while advertising */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to infer address type, error code: %d", rc);
        return;
    }

    /* Copy device address to addr_val */
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to copy device address, error code: %d", rc);
        return;
    }
    format_addr(addr_str, addr_val);
    ESP_LOGI(TAG, "device address: %s", addr_str);

    /* Start advertising. */
    start_advertising();
}
```

### Start Advertising

As a beacon device, we're going to start advertising and send scan response if a scan request is received. To make it happen, we need to set advertisement and scan response data before advertising starts. So the following are what we do:

1. Initialize advertisement and scan response fields structs `adv_fields` and `rsp_fields`, as well as advertising parameters struct `adv_params`

``` C
static void start_advertising(void) {
    /* Local variables */
    int rc = 0;
    const char *name;
    struct ble_hs_adv_fields adv_fields = {0};
    struct ble_hs_adv_fields rsp_fields = {0};
    struct ble_gap_adv_params adv_params = {0};

    ...
}
```

2. Set advertising flags, device name, transmit power, appearance and LE role in `adv_fields`, and call `ble_gap_adv_set_fields`
    1. For adveritisng flags, `BLE_HS_ADV_F_DISC_GEN` means advertising is general discoverable, and `BLE_HS_ADV_F_BREDR_UNSUP` means BLE support only (BR/EDR refers to Bluetooth Classic)
    2. For appearance, it is used to tell scanner what does it look like; we use `BLE_GAP_APPEARANCE_GENERIC_TAG` here to make our device identified as a tag

``` C
static void start_advertising(void) {
    ...

    /* Set advertising flags */
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Set device name */
    name = ble_svc_gap_device_name();
    adv_fields.name = (uint8_t *)name;
    adv_fields.name_len = strlen(name);
    adv_fields.name_is_complete = 1;

    /* Set device tx power */
    adv_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    adv_fields.tx_pwr_lvl_is_present = 1;

    /* Set device appearance */
    adv_fields.appearance = BLE_GAP_APPEARANCE_GENERIC_TAG;
    adv_fields.appearance_is_present = 1;

    /* Set device LE role */
    adv_fields.le_role = BLE_GAP_LE_ROLE_PERIPHERAL;
    adv_fields.le_role_is_present = 1;

    /* Set advertiement fields */
    rc = ble_gap_adv_set_fields(&adv_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set advertising data, error code: %d", rc);
        return;
    }

    ...
}
```

3. Set device address and URI in `rsp_fields`, and call `ble_gap_adv_rsp_set_fields`
    1. Since `AdvData` in advertisement packet **should not be longer than 31 bytes**, additional information must be placed in scan response packet
    2. We put the official website link of espressif into URI field

``` C
static void start_advertising(void) {
    ...

    /* Set device address */
    rsp_fields.device_addr = addr_val;
    rsp_fields.device_addr_type = own_addr_type;
    rsp_fields.device_addr_is_present = 1;

    /* Set URI */
    rsp_fields.uri = esp_uri;
    rsp_fields.uri_len = sizeof(esp_uri);

    /* Set scan response fields */
    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set scan response data, error code: %d", rc);
        return;
    }

    ...
}
```

4. Set advertising mode and discoverable mode to non-connectable and general-discoverable respectively in `adv_params`, and finally, start advertising by calling `ble_gap_adv_start`

``` C
static void start_advertising(void) {
    ...

    /* Set non-connetable and general discoverable mode to be a beacon */
    adv_params.conn_mode = BLE_GAP_CONN_MODE_NON;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    /* Start advertising */
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params,
                           NULL, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to start advertising, error code: %d", rc);
        return;
    }
    ESP_LOGI(TAG, "advertising started!");
}
```

### Observation

If everything goes well, you should be able to see `NimBLE_Beacon` on a BLE scanner device, broadcasting a lot of information including an URI of "https://espressif.com" (The official website of espressif), which is exactly what we expect.

## Troubleshooting

For any technical queries, please file an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
