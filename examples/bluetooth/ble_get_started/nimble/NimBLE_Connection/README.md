| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# NimBLE Connection Example

## Overview

This example is extended from NimBLE Beacon Example, and further introduces

1. How to advertise as a connectable peripheral device
2. How to capture GAP events and handle them
3. How to update connection parameters

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

1. Initialize LED, NVS flash, NimBLE host stack and GAP service; configure NimBLE host stack and start NimBLE host task thread; wait for NimBLE host stack to sync with BLE controller
2. Set advertisement and scan response data, then configure advertising parameters and start advertising
3. On connect event
    1. Turn on the LED on the dev board
    2. Print out connection descriptions
    3. Update connection parameters
4. On connection update event
    1. Print out connection descriptions
5. On disconnect event
    1. Turn off the LED on the dev board
    2. Print out connection descriptions

### Entry Point & On Stack Sync

Please refer to the NimBLE Beacon Example for details.

### Start Advertising

There're some slight differences in this example when compared to NimBLE Beacon Example. First, in this example we are constructing a connectable peripheral, so connection mode is set to connectable, that is 

``` C
static void start_advertising(void) {
    ...

    /* Set non-connetable and general discoverable mode to be a beacon */
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ...
}
```

Also, to demonstrate advertising parameters settings, advertising interval parameters are modified to 500ms, and shown in scan response. Please note that the unit of advertising interval is 0.625ms.

``` C
static void start_advertising(void) {
    ...

    /* Set advertising interval */
    rsp_fields.adv_itvl = BLE_GAP_ADV_ITVL_MS(500);
    rsp_fields.adv_itvl_is_present = 1;

    ...

    /* Set advertising interval */
    adv_params.itvl_min = BLE_GAP_ADV_ITVL_MS(500);
    adv_params.itvl_max = BLE_GAP_ADV_ITVL_MS(510);

    ...
}
```

And finally, when calling the advertising start API, a callback function `gap_event_handler` is passed as argument to receive GAP events. We'll talk about it in the next section.

``` C
static void start_advertising(void) {
    ...

    /* Start advertising */
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params,
                           gap_event_handler, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to start advertising, error code: %d", rc);
        return;
    }
    ESP_LOGI(TAG, "advertising started!");
}
```

### On GAP Events

To keep it simple, we're interested in 3 GAP events at the moment

- `BLE_GAP_EVENT_CONNECT` - Connect event
- `BLE_GAP_EVENT_DISCONNECT` - Disconnect event
- `BLE_GAP_EVENT_CONN_UPDATE` - Connection update event

#### Connect Event

When the device is connected to a peer device or a connection failed, a connect event will be passed to `gap_event_handler` by NimBLE host stack. We'll first check the connection status

- If succeeded
    - Get connection descriptor by connection handle and print out
    - Turn on the LED
    - Try to update connection parameters
- If failed
    - Re-start advertising

``` C
/* Connect event */
static int gap_event_handler(struct ble_gap_event *event, void *arg) {
    /* Local variables */
    int rc = 0;
    struct ble_gap_conn_desc desc;

    /* Handle different GAP event */
    switch (event->type) {

    /* Connect event */
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        ESP_LOGI(TAG, "connection %s; status=%d",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);

        /* Connection succeeded */
        if (event->connect.status == 0) {
            /* Check connection handle */
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            if (rc != 0) {
                ESP_LOGE(TAG,
                         "failed to find connection by handle, error code: %d",
                         rc);
                return rc;
            }

            /* Print connection descriptor and turn on the LED */
            print_conn_desc(&desc);
            led_on();

            /* Try to update connection parameters */
            struct ble_gap_upd_params params = {.itvl_min = desc.conn_itvl,
                                                .itvl_max = desc.conn_itvl,
                                                .latency = 3,
                                                .supervision_timeout =
                                                    desc.supervision_timeout};
            rc = ble_gap_update_params(event->connect.conn_handle, &params);
            if (rc != 0) {
                ESP_LOGE(
                    TAG,
                    "failed to update connection parameters, error code: %d",
                    rc);
                return rc;
            }
        }
        /* Connection failed, restart advertising */
        else {
            start_advertising();
        }
        return rc;

    ...
    }
}
```

#### Disconnect Event

On disconnect event, we simply 

1. Print out disconnect reason and connection descriptor
2. Turn off the LED
3. Re-start advertising

``` C
static int gap_event_handler(struct ble_gap_event *event, void *arg) {
    ...

    /* Disconnect event */
    case BLE_GAP_EVENT_DISCONNECT:
        /* A connection was terminated, print connection descriptor */
        ESP_LOGI(TAG, "disconnected from peer; reason=%d",
                 event->disconnect.reason);

        /* Turn off the LED */
        led_off();

        /* Restart advertising */
        start_advertising();
        return rc;

    ...
}
```

#### Connection Update Event

On connection update event, the operation is also very simple

1. Print out connection status
2. Get connection descriptor by connection handle and print out

``` C
static int gap_event_handler(struct ble_gap_event *event, void *arg) {
    ...

    /* Connection parameters update event */
    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        ESP_LOGI(TAG, "connection updated; status=%d",
                    event->conn_update.status);

        /* Print connection descriptor */
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        if (rc != 0) {
            ESP_LOGE(TAG, "failed to find connection by handle, error code: %d",
                        rc);
            return rc;
        }
        print_conn_desc(&desc);
        return rc;
        
    ...
}
```

## Observation

If everything goes well, except for what we have seen in NimBLE Beacon example, you should be able to see LED turned on when device is connected, and see LED turned off on disconnection.

## Troubleshooting

For any technical queries, please file an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

