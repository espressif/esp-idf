| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# NimBLE Security Example

## Overview

This example is extended from NimBLE GATT Server Example, and further introduces

1. How to set random non-resolvable private address for device
2. How to ask for connection encryption from peripheral side on characteristic access
3. How to bond with peer device using a random generated 6-digit passkey


To test this demo, install *nRF Connect for Mobile* on your phone. 

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

The following is additional content compared to the NimBLE GATT Server example.

1. Initialization procedure is generally similar to NimBLE GATT Server Example, but we'll initialize random number generator in the beginning, and in `nimble_host_config_init` we will configure security manager to enable related features
2. On stack sync, a random non-resolvable private address is generated and set as the device address
3. Characteristics' permission modified to require connection encryption when accessing
4. 3 more GAP event branches added in `gap_event_handler` to handle encryption related events

### Entry Point

In `nimble_host_config_init` function, we're going to enable some security manager features, including

- Bonding
- Man-in-the-middle protection
- Key distribution

Also, we're going to set the IO capability to `BLE_HS_IO_DISPLAY_ONLY`, since it's possible to print out the passkey to serial output.

``` C
static void nimble_host_config_init(void) {
    ...

    /* Security manager configuration */
    ble_hs_cfg.sm_io_cap = BLE_HS_IO_DISPLAY_ONLY;
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_mitm = 1;
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;

    ...
}
```

### GATT Server Updates

For heart rate characteristic and LED characteristic, `BLE_GATT_CHR_F_READ_ENC` and `BLE_GATT_CHR_F_WRITE_ENC` flag are added respectively to require connection encryption when GATT client tries to access the characteristic. Thanks to NimBLE host stack, the connection encryption will be initiated automatically by adding these flags.

However, heart rate characteristic is also indicatable, and NimBLE host stack does not offer an implementation for indication access to require connection encryption, so we need to do it ourselves. For GATT server, we simply check connection security status by calling an external function `is_connection_encrypted` in `send_heart_rate_indication` function to determine if the indication should be sent. This external function is defined in GAP layer, and we'll talk about it in *GAP Event Handler Updates* section.

``` C
void send_heart_rate_indication(void) {
    /* Check if connection handle is initialized */
    if (!heart_rate_chr_conn_handle_inited) {
        return;
    }

    /* Check indication and security status */
    if (heart_rate_ind_status &&
        is_connection_encrypted(heart_rate_chr_conn_handle)) {
        ble_gatts_indicate(heart_rate_chr_conn_handle,
                           heart_rate_chr_val_handle);
    }
}
```

### Random Address

In the following function, we can generate a random non-resolvable private address and set as the device address. We will call it in `adv_init` function before ensuring address availability.

``` C
static void set_random_addr(void) {
    /* Local variables */
    int rc = 0;
    ble_addr_t addr;

    /* Generate new non-resolvable private address */
    rc = ble_hs_id_gen_rnd(0, &addr);
    assert(rc == 0);

    /* Set address */
    rc = ble_hs_id_set_rnd(addr.val);
    assert(rc == 0);
}
```

### Check Connection Encryption Status

By connection handle, we can fetch connection descriptor from NimBLE host stack, and there's a flag indicating connection encryption status, check the following codes

``` C
bool is_connection_encrypted(uint16_t conn_handle) {
    /* Local variables */
    int rc = 0;
    struct ble_gap_conn_desc desc;

    /* Print connection descriptor */
    rc = ble_gap_conn_find(conn_handle, &desc);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to find connection by handle, error code: %d",
                 rc);
        return false;
    }

    return desc.sec_state.encrypted;
}
```

### GAP Event Handler Updates

3 more GAP event branches are added in `gap_event_handler`, which are

- `BLE_GAP_EVENT_ENC_CHANGE` - Encryption change event
- `BLE_GAP_EVENT_REPEAT_PAIRING` - Repeat pairing event
- `BLE_GAP_EVENT_PASSKEY_ACTION` - Passkey action event

On encryption change event, we're going to print the encryption change status to output.

``` C
/* Encryption change event */
case BLE_GAP_EVENT_ENC_CHANGE:
    /* Encryption has been enabled or disabled for this connection. */
    if (event->enc_change.status == 0) {
        ESP_LOGI(TAG, "connection encrypted!");
    } else {
        ESP_LOGE(TAG, "connection encryption failed, status: %d",
                    event->enc_change.status);
    }
    return rc;
```

On repeat pairing event, to make it simple, we will just delete the old bond and repeat pairing.

``` C
/* Repeat pairing event */
case BLE_GAP_EVENT_REPEAT_PAIRING:
    /* Delete the old bond */
    rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to find connection, error code %d", rc);
        return rc;
    }
    ble_store_util_delete_peer(&desc.peer_id_addr);

    /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
        * continue with pairing operation */
    ESP_LOGI(TAG, "repairing...");
    return BLE_GAP_REPEAT_PAIRING_RETRY;
```

On passkey action event, a random 6-digit passkey is generated, and you are supposed to enter the same passkey on pairing. If the input is consistent with the generated passkey, you should be able to bond with the device.

``` C
/* Passkey action event */
case BLE_GAP_EVENT_PASSKEY_ACTION:
    /* Display action */
    if (event->passkey.params.action == BLE_SM_IOACT_DISP) {
        /* Generate passkey */
        struct ble_sm_io pkey = {0};
        pkey.action = event->passkey.params.action;
        pkey.passkey = 100000 + esp_random() % 900000;
        ESP_LOGI(TAG, "enter passkey %" PRIu32 " on the peer side",
                    pkey.passkey);
        rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);
        if (rc != 0) {
            ESP_LOGE(TAG,
                        "failed to inject security manager io, error code: %d",
                        rc);
            return rc;
        }
    }
    return rc;
```

## Observation

If everything goes well, pairing will be required when you try to access any of the characteristics, that is, read or indicate heart rate characteristic, or write LED characteristic.

## Troubleshooting

For any technical queries, please file an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
