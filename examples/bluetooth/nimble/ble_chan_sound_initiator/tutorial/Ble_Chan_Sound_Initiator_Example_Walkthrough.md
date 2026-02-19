# BLE Channel Sounding Initiator Example Walkthrough

## Introduction

In this tutorial, the ble_chan_sound_initiator example code is reviewed. The example shows how a BLE Central acts as a Channel Sounding (CS) **initiator**: it scans for a peer advertising the Ranging Service, pairs with LE Secure Connections, discovers the Ranging Service (RAS) on that peer, starts free-running CS procedures, and collects both halves of the ranging data — the local CS step data reported by its own controller over HCI, and the peer's step data delivered over the RAS protocol.

**Note:** Channel Sounding is performed entirely by the controller. No currently available ESP SoC has a Channel Sounding capable controller, so this example runs the NimBLE host only (`CONFIG_BT_CONTROLLER_DISABLED=y`) and requires an external CS-capable controller attached over HCI UART. See the [README](../README.md) for the wiring.

Distance estimation is intentionally **out of scope**. The example hands a complete, per-procedure data set to the application and stops there; converting that into a distance is the job of an application-supplied estimation module.

## Includes

This example is located in the examples folder of the ESP-IDF under the [ble_chan_sound_initiator/main](../main). The [main.c](../main/main.c) file located in the main folder contains all the functionality that we are going to review. The header files contained in [main.c](../main/main.c) are:

```c
#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_cs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/ras/ble_svc_ras.h"
#include "ble_chan_initiator.h"
#include "esp_nimble_cfg.h"
#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
#include "esp_nimble_mem.h"
#endif
```

* `host/ble_cs.h`: Channel Sounding APIs — capability and configuration commands, and `ble_cs_initiator_procedure_start()`.
* `services/ras/ble_svc_ras.h`: the Ranging Service. The initiator uses the **RREQ** (Ranging Requester, i.e. GATT client) half of it, declared as the `ble_svc_ras_rreq_*` family.
* `ble_chan_initiator.h`: example-local declarations; it pulls in `esp_central.h` for the shared `peer` service-discovery cache.

## Main Entry Point

`app_main()` brings up NVS and the NimBLE port, installs the host callbacks, and starts the host task:

```c
void
app_main(void)
{
    ...
    ret = nimble_port_init();
    ...
    ble_hs_cfg.reset_cb = ble_chan_sound_init_on_reset;
    ble_hs_cfg.sync_cb = ble_chan_sound_init_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* RAP 6.2 SHALL: LE Secure Connections pairing */
    ble_hs_cfg.sm_sc = 1;

    rc = peer_init(MYNEWT_VAL(BLE_MAX_CONNECTIONS), 64, 64, 64);
    assert(rc == 0);

    rc = ble_svc_gap_device_name_set("NIMBLE_CS_INITIATOR");
    assert(rc == 0);

    ble_store_config_init();
    nimble_port_freertos_init(ble_chan_sound_init_host_task);
}
```

`ble_hs_cfg.sm_sc = 1` is not optional here. The Ranging Profile requires an encrypted LE Secure Connections link before any ranging data is exchanged, and the example enforces that later by terminating any connection that fails to encrypt.

## Host Sync Callback

Once the host has synced with the controller, `ble_chan_sound_init_on_sync()` runs:

```c
static void
ble_chan_sound_init_on_sync(void)
{
    int rc;

    /* Feature bit 47 is LE Channel Sounding Host Support.  The controller
     * refuses every CS command until the host has set it, so this has to happen
     * before anything else CS related.
     */
    rc = ble_gap_set_host_feat(47, 0x01);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "Failed to enable Channel Sounding host feature; rc=%d\n", rc);
    }

    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);

    ble_chan_sound_init_scan();
}
```

Setting **host feature bit 47** (LE Channel Sounding Host Support) is the first thing that has to happen. Until the host declares it, the controller rejects every CS command, so this precedes scanning and everything else CS related.

## Scanning and Connection

`ble_chan_sound_init_scan()` starts a passive, duplicate-filtered scan with `ble_gap_disc()`. Every advertising report is passed to `ble_chan_sound_init_connect_if_interesting()`, which parses the AD structures and connects only to a peer advertising the Ranging Service UUID:

```c
for (int i = 0; i < fields.num_uuids16; i++) {
    if (ble_uuid_u16(&fields.uuids16[i].u) == BLE_SVC_RAS_RANGING_SERVICE_VAL) {
        found = 1;
        break;
    }
}
if (!found) {
    return;
}
```

The example builds in two variants selected by `CONFIG_EXAMPLE_EXTENDED_ADV`: the extended-scanning path reads the report out of `struct ble_gap_ext_disc_desc`, the legacy path out of `struct ble_gap_disc_desc`. Both feed the same UUID filter.

Addresses follow the profile requirement to use a random device address: `ble_hs_id_infer_auto(1, &own_addr_type)` is called before both the scan and the connect.

## Connection Setup: PHY and Security

On `BLE_GAP_EVENT_CONNECT` the example resets its per-connection state, then does two things before anything CS related:

```c
/* Connections from legacy ADV_IND start on the 1M PHY. Request
 * an upgrade to 2M explicitly for higher throughput on the RAS
 * real-time stream.  Non-fatal: if the peer declines we stay on
 * 1M rather than drop the link. */
rc = ble_gap_set_prefered_le_phy(event->connect.conn_handle,
                                 BLE_GAP_LE_PHY_2M_MASK,
                                 BLE_GAP_LE_PHY_2M_MASK, 0);

rc = ble_gap_security_initiate(event->connect.conn_handle);
```

The 2M PHY request is a throughput optimisation and is deliberately non-fatal. Security is not: a failure to start pairing terminates the link.

## Encryption Gate and Service Discovery

`BLE_GAP_EVENT_ENC_CHANGE` is where the example decides whether the link is usable at all:

```c
if (event->enc_change.status != 0 || !desc.sec_state.encrypted ||
    !desc.sec_state.sc) {
    MODLOG_DFLT(ERROR, "encryption failed/missing or not LE Secure "
                "Connections (status=%d enc=%d sc=%d) "
                "- terminating conn=%d\n", ...);
    return ble_gap_terminate(event->enc_change.conn_handle,
                             BLE_ERR_AUTH_FAIL);
}

if (disc_initiated) {
    return 0;
}
disc_initiated = true;

ble_gattc_exchange_mtu(event->enc_change.conn_handle,
                       exchange_func, NULL);
rc = peer_disc_all(event->enc_change.conn_handle,
                   ble_chan_sound_init_on_disc_complete, NULL);
```

Discovery is gated on security rather than on connection, because the RAS characteristics require an encrypted link. The `sc` bit is checked as well as `encrypted`: `ble_hs_cfg.sm_sc = 1` only advertises Secure Connections support in the pairing AuthReq, so a peer that does not set that bit still completes LE Legacy Pairing and still produces an encrypted link. Testing `encrypted` alone would let such a link through, which the Ranging Profile does not permit. The `disc_initiated` flag makes this run once per connection — a re-encryption on a bonded reconnect raises `ENC_CHANGE` again and must not restart discovery.

## Building the RAS Handle Set

`ble_chan_sound_init_on_disc_complete()` turns the discovered attribute database into the handle set that the RREQ client needs. Two helper macros look up each characteristic value handle and each Client Characteristic Configuration descriptor:

```c
FILL_CHR_HANDLE(features,       BLE_SVC_RAS_CHR_UUID_FEATURES_VAL);
FILL_CHR_HANDLE(od_rd,          BLE_SVC_RAS_CHR_UUID_ONDEMAND_RD_VAL);
FILL_CHR_HANDLE(rt_rd,          BLE_SVC_RAS_CHR_UUID_REALTIME_RD_VAL);
FILL_CHR_HANDLE(cp,             BLE_SVC_RAS_CHR_UUID_CP_VAL);
FILL_CHR_HANDLE(rd_ready,       BLE_SVC_RAS_CHR_UUID_RD_READY_VAL);
FILL_CHR_HANDLE(rd_overwritten, BLE_SVC_RAS_CHR_UUID_RD_OVERWRITTEN_VAL);

FILL_CCC_HANDLE(od_rd_ccc,          BLE_SVC_RAS_CHR_UUID_ONDEMAND_RD_VAL);
FILL_CCC_HANDLE(rt_rd_ccc,          BLE_SVC_RAS_CHR_UUID_REALTIME_RD_VAL);
...
```

The six RAS characteristics are:

| Characteristic | Role |
| --- | --- |
| RAS Features | Read once; advertises which optional features the server supports |
| Real-time Ranging Data | Server pushes ranging data as it is produced |
| On-demand Ranging Data | Server sends ranging data when asked for it |
| RAS Control Point | Client commands: `GET_RD`, `ACK_RD`, abort |
| Ranging Data Ready | Server tells the client a procedure's data is available |
| Ranging Data Overwritten | Server tells the client a buffered procedure was discarded |

Only `cp`, `rd_ready` and `features` are treated as mandatory; a peer missing any of them is dropped. The handle set is then given to the RREQ client and the callbacks are registered:

```c
rc = ble_svc_ras_rreq_alloc(conn_handle, &handles);
...
ble_svc_ras_rreq_set_rd_ready_cb(conn_handle, ble_chan_sound_init_on_rd_ready);
ble_svc_ras_rreq_set_rd_overwritten_cb(conn_handle, ble_chan_sound_init_on_rd_overwritten);
ble_svc_ras_rreq_set_data_complete_cb(conn_handle, ble_chan_sound_init_on_data_complete);
ble_svc_ras_rreq_set_local_cs_cb(conn_handle, ble_chan_sound_init_on_local_cs_event);

rc = ble_svc_ras_rreq_read_features(conn_handle, ble_chan_sound_init_on_features_read);
```

## Choosing the Ranging Data Mode

The features read decides how the peer's ranging data will be transported:

```c
s_use_realtime = CS_INIT_PREFER_REALTIME &&
                 !!(features & BLE_SVC_RAS_FEAT_REALTIME_RD_BIT);
int rc = ble_svc_ras_rreq_set_ranging_mode(conn_handle, s_use_realtime);
...
ble_svc_ras_rreq_set_indicate(conn_handle, CS_INIT_USE_INDICATE);
rc = ble_svc_ras_rreq_subscribe(conn_handle, ble_chan_sound_init_on_subscribe_complete);
```

* **Real-time** mode has the server push each procedure's data as it is produced, with no round trip. It is optional and has to be advertised in the features bitmap.
* **On-demand** mode is mandatory for every RAS server, so it is the fallback.

Two compile-time switches at the top of `main.c` select the test scenario:

```c
/* ---- Ranging data transfer mode (change these two to switch test scenarios) ----
 * CS_INIT_PREFER_REALTIME : 1 = real-time push,  0 = on-demand GET_RD
 * CS_INIT_USE_INDICATE    : 1 = data RD indicate, 0 = notify  (RT and OD)
 * ----------------------------------------------------------------------------- */
#define CS_INIT_PREFER_REALTIME 1
#define CS_INIT_USE_INDICATE    1
```

Note that `CS_INIT_PREFER_REALTIME` is only a *preference*: if the peer does not advertise the real-time bit, the example falls back to on-demand regardless.

## Starting Free-Running CS Procedures

Once every RAS CCCD is subscribed, the CS procedure is started:

```c
struct ble_cs_initiator_procedure_start_params param;
memset(&param, 0, sizeof(param));
param.conn_handle = conn_handle;
param.cb = ble_svc_ras_rreq_cs_event_handler;
param.cb_arg = NULL;
int rc = ble_cs_initiator_procedure_start(&param);
```

Two details matter here:

1. **The callback is the RREQ handler, not an example function.** CS events are delivered into the Ranging Service, which looks up the per-connection RREQ context by connection handle and forwards the event to whatever the example registered with `ble_svc_ras_rreq_set_local_cs_cb()`. Note that it forwards exactly three event types — `SUBEVENT_RESULT`, `SUBEVENT_RESULT_CONTINUE` and `CS_PROCEDURE_COMPLETE`; any other CS event is dropped and never reaches the application.
2. **Procedures are free-running.** Procedure Enable is sent once with `max_procedure_count = 0`, so the controllers repeat procedures until the link drops. The example never re-arms anything per procedure; it only counts completions.

In real-time mode an extra timer is armed:

```c
if (s_use_realtime) {
    int rt_rc = ble_svc_ras_rreq_rt_cs_started(conn_handle);
    ...
}
```

This is the profile's "no data" watchdog: it surfaces a stalled real-time stream as a timeout instead of hanging silently. It is re-armed after every completed transfer.

## Collecting Local CS Step Data

The local half of the data arrives as HCI subevent results. `ble_chan_sound_init_on_local_cs_event()` accumulates them into one buffer per procedure:

```c
case BLE_CS_EVENT_SUBEVENT_RESULT: {
    int32_t pc =
        BLE_SVC_RAS_RANGING_COUNTER(event->subev_result.procedure_counter);

    if (pc != local_steps_proc_counter) {
        local_steps_len = 0;
        local_steps_num = 0;
        local_steps_proc_counter = pc;
    }
    ...
    local_steps_append(event->subev_result.steps,
                       event->subev_result.num_steps_reported);
    break;
}

case BLE_CS_EVENT_SUBEVENT_RESULT_CONTINUE:
    local_steps_append(event->subev_result_continue.steps,
                       event->subev_result_continue.num_steps_reported);
    break;

case BLE_CS_EVENT_CS_PROCEDURE_COMPLETE:
    MODLOG_DFLT(INFO, "[CS] Procedure %d complete status=%d "
                "(local: %d bytes, %d steps, nap=%d)\n", ...);
    break;
```

A change in procedure counter resets the buffer, so each procedure starts clean. `RESULT` carries the first batch of steps for a subevent and `RESULT_CONTINUE` carries the rest when they do not fit in one HCI event; both append through the same path.

Steps arrive in a packed, variable-length HCI layout — `mode(1) + channel(1) + data_len(1) + data[data_len]` per step — so `cs_step_data_total_len()` walks the entries to find the byte length rather than assuming a fixed stride.

The buffer itself follows the NimBLE `BLE_STATIC_TO_DYNAMIC` idiom: a static `.bss` array when the option is off, and a heap allocation made lazily on the first subevent of a procedure and released on disconnect when it is on, so that idle and scanning periods do not hold CS-specific RAM.

## Receiving the Peer's Ranging Data

In **on-demand** mode the peer announces each procedure and the example asks for it:

```c
static void
ble_chan_sound_init_on_rd_ready(uint16_t conn_handle, uint16_t ranging_counter)
{
    ...
    int rc = ble_svc_ras_rreq_get_ranging_data(conn_handle, ranging_counter,
                                                NULL);
    if (rc == BLE_HS_EBUSY) {
        /* Another transfer is still in flight.  With free-running
         * procedures a new RD Ready can arrive before the previous
         * transfer's ACK completes; skip this counter - the next
         * procedure's RD Ready follows shortly. */
        return;
    }
    ...
}
```

The library then runs the segmented transfer and the `ACK_RD` handshake internally. In **real-time** mode no `GET_RD` is sent at all — segments arrive unsolicited — and an `RD Ready` from a misbehaving peer is ignored.

Either way, a finished procedure surfaces in one place:

```c
static void
ble_chan_sound_init_on_data_complete(uint16_t conn_handle, uint16_t ranging_counter,
                     const uint8_t *data, uint16_t data_len, int status)
{
    if (status == 0) {
        ...
        /* Both local HCI step data (local_steps / local_steps_len) and
         * the peer's RAS ranging data (`data` / `data_len`) are now
         * available for this procedure.  Distance estimation is
         * intentionally out of scope for this example and is expected
         * to be provided by an application-supplied module; plug it in
         * here when integrating. */
        cs_procedures_done++;
    } else {
        /* Free-running stream: this procedure's data is lost, but the
         * next procedure arrives on its own - just log and move on. */
    }
}
```

This is the integration point. At this moment the application holds a matched pair — the local step data and the peer's step data for the same procedure counter — which is exactly what a distance-estimation module needs. The example prints a transfer summary instead:

```
I (5048) NimBLE: ============================================
I (5048) NimBLE:  CHANNEL SOUNDING DATA TRANSFER SUMMARY
I (5058) NimBLE:  Procedure counter : 0
I (5058) NimBLE:  Peer (server) data: 1307 bytes received OK
I (5068) NimBLE:  Local (initiator) : 1569 bytes, 135 steps (nap=1)
I (5068) NimBLE: ============================================
```

Because the stream is free-running, errors are deliberately non-fatal throughout: a failed transfer logs and drops that one procedure, and the next one arrives on its own.

## Disconnect Cleanup

```c
case BLE_GAP_EVENT_DISCONNECT:
    ble_svc_ras_rreq_free(event->disconnect.conn.conn_handle);
    peer_delete(event->disconnect.conn.conn_handle);
    local_steps_buf_deinit();
    ble_chan_sound_init_scan();
    return 0;
```

The RREQ state, the peer discovery cache and the step buffer are all released, and the example returns to scanning.

## Conclusion

This example demonstrates:

1. Enabling the Channel Sounding host feature (bit 47) before any CS operation.
2. Filtering advertisements on the Ranging Service UUID and enforcing an encrypted LE Secure Connections link before ranging.
3. Discovering the Ranging Service and driving it through the RREQ client API.
4. Selecting real-time or on-demand ranging data transport from the peer's advertised features.
5. Running free-running CS procedures with a single Procedure Enable.
6. Accumulating local HCI step data per procedure and pairing it with the peer's ranging data, ready for an application-supplied distance-estimation module.

The peer side of this exchange is covered in the [BLE Channel Sounding Reflector Example Walkthrough](../../ble_chan_sound_reflector/tutorial/Ble_Chan_Sound_Reflector_Example_Walkthrough.md).
