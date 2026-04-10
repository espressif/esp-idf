# BLE Channel Sounding Initiator Example Walkthrough

## Introduction

In this tutorial, the ble_chan_sound_initiator example code is reviewed. This example demonstrates how a BLE Central device can perform Channel Sounding (CS) procedures with a BLE Peripheral. The initiator scans for a device advertising the Ranging Service (0x185B), establishes a connection, performs service discovery for the RAS (Ranging and Sounding) service, and initiates the CS procedure to exchange ranging data.

**Note:** This example currently requires an external Bluetooth controller supporting BLE Channel Sounding functionality.

## Includes

This example is located in the examples folder of the ESP-IDF under the [ble_chan_sound_initiator/main](../main). The [main.c](../main/main.c) file located in the main folder contains all the functionality that we are going to review. The key header files are:

```c
#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_cs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/hid/ble_svc_hid.h"
#include "services/ras/ble_svc_ras.h"
#include "ble_chan_initiator.h"
```

* `ble_cs.h`: Defines Channel Sounding APIs for initiating CS procedures.
* `ble_svc_ras.h`: Defines the Ranging and Sounding (RAS) service APIs and structures.
* `ble_chan_initiator.h`: Contains forward declarations and definitions specific to this example.

## Main Entry Point

The `app_main()` function initializes NVS, the NimBLE stack, configures host callbacks, initializes peer tracking structures, sets the device name, and starts the NimBLE host task:

```c
void
app_main(void)
{
    ...
    ret = nimble_port_init();
    ...
    ble_hs_cfg.reset_cb = blecent_on_reset;
    ble_hs_cfg.sync_cb = blecent_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);

    rc = ble_svc_gap_device_name_set("nimble_ble_channel_sounding");
    assert(rc == 0);

    ble_store_config_init();
    nimble_port_freertos_init(blecent_host_task);
}
```

## Host Sync Callback

When the NimBLE host syncs with the controller, `blecent_on_sync()` is called. It sets the Channel Sounding host feature bit, ensures a valid identity address, and begins scanning:

```c
static void
blecent_on_sync(void)
{
    int rc;
    rc = ble_gap_set_host_feat(47, 0x01);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting host feature; rc=%d\n", rc);
    }
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    blecent_scan();
}
```

The `ble_gap_set_host_feat(47, 0x01)` call enables the Channel Sounding host feature (bit 47) as required by the BLE specification for CS support.

## Scanning and Connection

The `blecent_scan()` function performs passive scanning using `ble_gap_disc()`. When an advertisement is received, `blecent_connect_if_interesting()` checks if the advertising data contains the Ranging Service UUID (0x185B). If found, the initiator connects to the peripheral.

## Service Discovery and RAS Subscription

After connection and encryption setup (`BLE_GAP_EVENT_ENC_CHANGE`), the initiator performs GATT service discovery via `peer_disc_all()`. Upon discovery completion, `blecent_on_disc_complete()` locates the RAS service characteristics:

- **RAS Features** (0x2C14) - Read to determine supported features
- **RAS Control Point** (0x2C17) - For sending ranging commands
- **Ranging Data Ready** (0x2C18) - Notification of available ranging data
- **Ranging Data Overwritten** (0x2C19) - Notification when data is overwritten
- **On-demand Ranging Data** (0x2C16) - For receiving ranging data on demand

The initiator reads the RAS Features characteristic to determine if real-time ranging data is supported, then subscribes to the appropriate characteristics accordingly.

## Channel Sounding Procedure

Once all four required subscriptions are complete (`subscribe_all == 4`), the CS procedure is initiated:

```c
struct ble_cs_initiator_procedure_start_params param;
memset(&param, 0, sizeof param);
param.conn_handle = conn_handle;
param.cb = blecs_gap_event;
param.cb_arg = NULL;
ble_cs_initiator_procedure_start(&param);
```

CS subevent results are handled in the `blecs_gap_event()` callback, which processes:
- `BLE_CS_EVENT_CS_PROCEDURE_COMPLETE`: Procedure completion notification
- `BLE_CS_EVENT_SUBEVET_RESULT`: Individual subevent results containing CS step data
- `BLE_CS_EVENT_SUBEVET_RESULT_CONTINUE`: Continuation of subevent results

## Ranging Data Exchange

The initiator handles RAS Control Point and Data Ready notifications to exchange ranging data with the reflector:

1. When a **Ranging Data Ready** indication arrives, the initiator sends a `GET_RD` (Get Ranging Data) command via the RAS Control Point.
2. The reflector responds with ranging data segments through On-demand Ranging Data notifications.
3. When the data transfer is complete (`COMPLETE_RD_RSP`), the initiator sends an `ACK_RD` acknowledgment.

## Conclusion

This example demonstrates:
1. BLE Channel Sounding procedure initiation from a Central device.
2. RAS (Ranging and Sounding) service discovery and characteristic subscription.
3. Exchange of ranging data between initiator and reflector using the RAS protocol.
4. CS subevent result handling for processing step data.
