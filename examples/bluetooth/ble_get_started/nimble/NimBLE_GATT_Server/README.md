| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# NimBLE GATT Server Example

## Overview

This example is extended from NimBLE Connection Example, and further introduces

1. How to implement a GATT server using GATT services table
2. How to handle characteristic access requests
    1. Write access demonstrated by LED control
    2. Read and indicate access demonstrated by heart rate measurement(mocked)

To test this demo, install *nRF Connect for Mobile* on your phone. 

Please refer to [BLE Introduction](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/ble/get-started/ble-introduction.html#:~:text=%E4%BE%8B%E7%A8%8B%E5%AE%9E%E8%B7%B5)
for detailed example introduction and code explanation.

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

1. Initialization
    1. Initialize LED, NVS flash, NimBLE host stack, GAP service
    2. Initialize GATT service and add services to registration queue
    3. Configure NimBLE host stack and start NimBLE host task thread, GATT services will be registered automatically when NimBLE host stack started
    4. Start heart rate update task thread
2. Wait for NimBLE host stack to sync with BLE controller, and start advertising; wait for connection event to come
3. After connection established, wait for GATT characteristics access events to come
    1. On write LED event, turn on or off the LED accordingly
    2. On read heart rate event, send out current heart rate measurement value
    3. On indicate heart rate event, enable heart rate indication

### Entry Point

In this example, we call GATT `gatt_svr_init` function to initialize GATT server in `app_main` before NimBLE host configuration. This is a custom function defined in `gatt_svc.c`, and basically we just call GATT service initialization API and add services to registration queue.

And there's another code added in `nimble_host_config_init`, which is 

``` C
static void nimble_host_config_init(void) {
    ...

    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;

    ...
}
```

That is GATT server register callback function. In this case it will only print out some registration information when services, characteristics or descriptors are registered.

Then, after NimBLE host task thread is created, we'll create another task defined in `heart_rate_task` to update heart rate measurement mock value and send indication if enabled.

### GAP Service Updates

`gap_event_handler` is updated with LED control removed, and more event handling branches, when compared to NimBLE Connection Example, including

- `BLE_GAP_EVENT_ADV_COMPLETE` - Advertising complete event
- `BLE_GAP_EVENT_NOTIFY_TX` - Notificate event
- `BLE_GAP_EVENT_SUBSCRIBE` - Subscribe event
- `BLE_GAP_EVENT_MTU` - MTU update event

`BLE_GAP_EVENT_ADV_COMPLETE` and `BLE_GAP_EVENT_MTU` events are actually not involved in this example, but we still put them down there for reference. `BLE_GAP_EVENT_NOTIFY_TX` and `BLE_GAP_EVENT_SUBSCRIBE` events will be discussed in the next section.

### GATT Services Table

GATT services are defined in `ble_gatt_svc_def` struct array, with a variable name `gatt_svr_svcs` in this demo. We'll call it as the GATT services table in the following content.

``` C
/* Heart rate service */
static const ble_uuid16_t heart_rate_svc_uuid = BLE_UUID16_INIT(0x180D);

static uint8_t heart_rate_chr_val[2] = {0};
static uint16_t heart_rate_chr_val_handle;
static const ble_uuid16_t heart_rate_chr_uuid = BLE_UUID16_INIT(0x2A37);

static uint16_t heart_rate_chr_conn_handle = 0;
static bool heart_rate_chr_conn_handle_inited = false;
static bool heart_rate_ind_status = false;

/* Automation IO service */
static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);
static uint16_t led_chr_val_handle;
static const ble_uuid128_t led_chr_uuid =
    BLE_UUID128_INIT(0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 0xde, 0xef,
                     0x12, 0x12, 0x25, 0x15, 0x00, 0x00);

/* GATT services table */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    /* Heart rate service */
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = &heart_rate_svc_uuid.u,
     .characteristics =
         (struct ble_gatt_chr_def[]){
             {/* Heart rate characteristic */
              .uuid = &heart_rate_chr_uuid.u,
              .access_cb = heart_rate_chr_access,
              .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_INDICATE,
              .val_handle = &heart_rate_chr_val_handle},
             {
                 0, /* No more characteristics in this service. */
             }}},

    /* Automation IO service */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &auto_io_svc_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){/* LED characteristic */
                                        {.uuid = &led_chr_uuid.u,
                                         .access_cb = led_chr_access,
                                         .flags = BLE_GATT_CHR_F_WRITE,
                                         .val_handle = &led_chr_val_handle},
                                        {0}},
    },
    
    {
        0, /* No more services. */
    },
};
```

In this table, there are two GATT primary services defined

- Heart rate service with a UUID of `0x180D`
- Automation IO service with a UUID of `0x1815`

#### Automation IO Service

Under automation IO service, there's a LED characteristic, with a vendor-specific UUID and write only permission.

The characteristic is binded with `led_chr_access` callback function, in which the write access event is captured. The LED will be turned on or off according to the write value, quite straight-forward.

``` C
static int led_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
    /* Local variables */
    int rc;

    /* Handle access events */
    /* Note: LED characteristic is write only */
    switch (ctxt->op) {

    /* Write characteristic event */
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        /* Verify connection handle */
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            ESP_LOGI(TAG, "characteristic write; conn_handle=%d attr_handle=%d",
                     conn_handle, attr_handle);
        } else {
            ESP_LOGI(TAG,
                     "characteristic write by nimble stack; attr_handle=%d",
                     attr_handle);
        }

        /* Verify attribute handle */
        if (attr_handle == led_chr_val_handle) {
            /* Verify access buffer length */
            if (ctxt->om->om_len == 1) {
                /* Turn the LED on or off according to the operation bit */
                if (ctxt->om->om_data[0]) {
                    led_on();
                    ESP_LOGI(TAG, "led turned on!");
                } else {
                    led_off();
                    ESP_LOGI(TAG, "led turned off!");
                }
            } else {
                goto error;
            }
            return rc;
        }
        goto error;

    /* Unknown event */
    default:
        goto error;
    }

error:
    ESP_LOGE(TAG,
             "unexpected access operation to led characteristic, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
}
```

#### Heart Rate Service

Under heart rate service, there's a heart rate measurement characteristic, with a UUID of `0x2A37` and read + indicate access permission.

The characteristic is binded with `heart_rate_chr_access` callback function, in which the read access event is captured. It should be mentioned that in SIG definition, heart rate measurement is a multi-byte data structure, with the first byte indicating the flags

- Bit 0: Heart rate value type
    - 0: Heart rate value is `uint8_t` type
    - 1: Heart rate value is `uint16_t` type
- Bit 1: Sensor contact status
- Bit 2: Sensor contact supported
- Bit 3: Energy expended status
- Bit 4: RR-interval status
- Bit 5-7: Reserved

and the rest of bytes are data fields. In this case, we use `uint8_t` type and disable other features, making the characteristic value a 2-byte array. So when characteristic read event arrives, we'll get the latest heart rate value and send it back to peer device.

``` C
static int heart_rate_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                 struct ble_gatt_access_ctxt *ctxt, void *arg) {
    /* Local variables */
    int rc;

    /* Handle access events */
    /* Note: Heart rate characteristic is read only */
    switch (ctxt->op) {

    /* Read characteristic event */
    case BLE_GATT_ACCESS_OP_READ_CHR:
        /* Verify connection handle */
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            ESP_LOGI(TAG, "characteristic read; conn_handle=%d attr_handle=%d",
                     conn_handle, attr_handle);
        } else {
            ESP_LOGI(TAG, "characteristic read by nimble stack; attr_handle=%d",
                     attr_handle);
        }

        /* Verify attribute handle */
        if (attr_handle == heart_rate_chr_val_handle) {
            /* Update access buffer value */
            heart_rate_chr_val[1] = get_heart_rate();
            rc = os_mbuf_append(ctxt->om, &heart_rate_chr_val,
                                sizeof(heart_rate_chr_val));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        goto error;

    /* Unknown event */
    default:
        goto error;
    }

error:
    ESP_LOGE(
        TAG,
        "unexpected access operation to heart rate characteristic, opcode: %d",
        ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
}
```

Indicate access, however, is a bit more complicated. As mentioned in *GAP Service Updates*, we'll handle another 2 events namely `BLE_GAP_EVENT_NOTIFY_TX` and `BLE_GAP_EVENT_SUBSCRIBE` in `gap_event_handler`. In this case, if peer device wants to enable heart rate measurement indication, it will send a subscribe request to the local device, and the request will be captured as a subscribe event in `gap_event_handler`. But from the perspective of software layering, the event should be handled in GATT server, so we just pass the event to GATT server by calling `gatt_svr_subscribe_cb`, as demonstrated in the demo

``` C
static int gap_event_handler(struct ble_gap_event *event, void *arg) {
    ...

    /* Subscribe event */
    case BLE_GAP_EVENT_SUBSCRIBE:
        /* Print subscription info to log */
        ESP_LOGI(TAG,
                    "subscribe event; conn_handle=%d attr_handle=%d "
                    "reason=%d prevn=%d curn=%d previ=%d curi=%d",
                    event->subscribe.conn_handle, event->subscribe.attr_handle,
                    event->subscribe.reason, event->subscribe.prev_notify,
                    event->subscribe.cur_notify, event->subscribe.prev_indicate,
                    event->subscribe.cur_indicate);

        /* GATT subscribe event callback */
        gatt_svr_subscribe_cb(event);
        return rc;
    
    ...
}
```

Then we'll check connection handle and attribute handle, if the attribute handle matches `heart_rate_chr_val_chandle`, `heart_rate_chr_conn_handle` and `heart_rate_ind_status` will be updated together. 

``` C
void gatt_svr_subscribe_cb(struct ble_gap_event *event) {
    /* Check connection handle */
    if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle);
    } else {
        ESP_LOGI(TAG, "subscribe by nimble stack; attr_handle=%d",
                 event->subscribe.attr_handle);
    }

    /* Check attribute handle */
    if (event->subscribe.attr_handle == heart_rate_chr_val_handle) {
        /* Update heart rate subscription status */
        heart_rate_chr_conn_handle = event->subscribe.conn_handle;
        heart_rate_chr_conn_handle_inited = true;
        heart_rate_ind_status = event->subscribe.cur_indicate;
    }
}
```

Notice that heart rate measurement incation is handled in `heart_rate_task` by calling `send_heart_rate_indication` function periodically. Actually, this function will check heart rate indication status and send indication accordingly. In this way, heart rate indication is implemented.

``` C
void send_heart_rate_indication(void) {
    if (heart_rate_ind_status && heart_rate_chr_conn_handle_inited) {
        ble_gatts_indicate(heart_rate_chr_conn_handle,
                           heart_rate_chr_val_handle);
        ESP_LOGI(TAG, "heart rate indication sent!");
    }
}

static void heart_rate_task(void *param) {
    /* Task entry log */
    ESP_LOGI(TAG, "heart rate task has been started!");

    /* Loop forever */
    while (1) {
        /* Update heart rate value every 1 second */
        update_heart_rate();
        ESP_LOGI(TAG, "heart rate updated to %d", get_heart_rate());

        /* Send heart rate indication if enabled */
        send_heart_rate_indication();

        /* Sleep */
        vTaskDelay(HEART_RATE_TASK_PERIOD);
    }

    /* Clean up at exit */
    vTaskDelete(NULL);
}
```

## Observation

If everything goes well, you should be able to see 4 services when connected to ESP32, including

- Generic Access
- Generic Attribute
- Heart Rate
- Automation IO

Click on Automation IO Service, you should be able to see LED characteristic. Click on upload button, you should be able to write `ON` or `OFF` value. Send it to the device, LED will be turned on or off following your instruction.

Click on Heart Rate Service, you should be able to see Heart Rate Measurement characteristic. Click on download button, you should be able to see the latest heart rate measurement mock value, and it should be consistent with what is shown on serial output. Click on subscribe button, you should be able to see the heart rate measurement mock value updated every second.

## Troubleshooting

For any technical queries, please file an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
