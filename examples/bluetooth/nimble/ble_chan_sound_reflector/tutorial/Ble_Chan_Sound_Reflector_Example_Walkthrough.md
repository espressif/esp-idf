# BLE Channel Sounding Reflector Example Walkthrough

## Introduction

In this tutorial, the ble_chan_sound_reflector example code is reviewed. The example shows how a BLE Peripheral acts as a Channel Sounding (CS) **reflector**: it advertises the Ranging Service, accepts a connection from a CS initiator, enables the reflector role on its controller, and serves the Ranging Service so the initiator can collect its half of the ranging data.

The striking thing about this example is how little application code a reflector needs. Once the role is enabled and the Ranging Service is registered, the controller performs the CS procedures and the Ranging Service implementation buffers and serves the resulting data on its own. There is no per-procedure application callback to write.

**Note:** Channel Sounding is performed entirely by the controller. No currently available ESP SoC has a Channel Sounding capable controller, so this example runs the NimBLE host only (`CONFIG_BT_CONTROLLER_DISABLED=y`) and requires an external CS-capable controller attached over HCI UART. See the [README](../README.md) for the wiring.

## Includes

This example is located in the examples folder of the ESP-IDF under the [ble_chan_sound_reflector/main](../main). The [main.c](../main/main.c) file located in the main folder contains all the functionality that we are going to review. The header files contained in [main.c](../main/main.c) are:

```c
#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/ras/ble_svc_ras.h"
#include "ble_chan_reflector.h"
#include "host/ble_cs.h"
```

* `host/ble_cs.h`: Channel Sounding APIs. The reflector uses `ble_cs_reflector_setup()` and `ble_cs_read_local_supp_cap()`.
* `services/ras/ble_svc_ras.h`: the Ranging Service. The reflector uses the **RRSP** (Ranging Responder, i.e. GATT server) half of it.

## Main Entry Point

`app_main()` initialises NVS and the NimBLE port, configures the host, registers the GATT services and starts the host task:

```c
void
app_main(void)
{
    ...
    ret = nimble_port_init();
    ...
    ble_hs_cfg.reset_cb = ble_chan_sound_refl_on_reset;
    ble_hs_cfg.sync_cb = ble_chan_sound_refl_on_sync;
#if CONFIG_BT_NIMBLE_RAS_SERVICE
    ble_hs_cfg.gatts_register_cb = ble_svc_ras_register_cb;
#endif
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
    ...
    /* RAP 6.2 SHALL: LE Secure Connections pairing (unconditional) */
    ble_hs_cfg.sm_sc = 1;

    rc = gatt_svr_init();
    assert(rc == 0);

    rc = ble_svc_gap_device_name_set("NIMBLE_CS_REFLECTOR");
    assert(rc == 0);

    ble_store_config_init();
    nimble_port_freertos_init(ble_chan_sound_refl_host_task);
}
```

Two assignments are specific to Channel Sounding:

* `ble_hs_cfg.gatts_register_cb = ble_svc_ras_register_cb` — a diagnostic hook. It logs each service, characteristic and descriptor handle at DEBUG level as the GATT database is built, which is the quickest way to confirm the Ranging Service registered as expected. It is not required for the service to work: the service captures its own value handles through the `.val_handle` pointers in its characteristic definition table, which NimBLE fills in during registration whether or not this callback is installed.
* `ble_hs_cfg.sm_sc = 1` — LE Secure Connections is set unconditionally, not behind a Kconfig option, because the Ranging Profile requires an encrypted LE Secure Connections link.

The pairing behaviour around those is configurable in `Example Configuration`: `EXAMPLE_BONDING` stores keys so a previously bonded initiator can re-encrypt without pairing again, and `EXAMPLE_RESOLVE_PEER_ADDR` distributes IRKs so the peer's resolvable private address can be resolved afterwards. Neither is required by CS itself.

## Registering the Ranging Service

The GATT server setup in [gatt_svr.c](../main/gatt_svr.c) is a three-line affair:

```c
int
gatt_svr_init(void)
{
#if CONFIG_BT_NIMBLE_GAP_SERVICE
    ble_svc_gap_init();
#endif
#if MYNEWT_VAL(BLE_GATTS)
    ble_svc_gatt_init();
#endif
#if MYNEWT_VAL(BLE_GATTS) && CONFIG_BT_NIMBLE_RAS_SERVICE
    ble_svc_ras_init();
#endif
    return 0;
}
```

`ble_svc_ras_init()` registers the whole Ranging Service — all six characteristics and their descriptors. The example defines no characteristics and no access callbacks of its own; every read, write and subscription from the initiator is handled inside the service.

## Host Sync Callback

When the host syncs with the controller, `ble_chan_sound_refl_on_sync()` runs three steps in order:

```c
static void
ble_chan_sound_refl_on_sync(void)
{
    ...
    /* Feature bit 47 is LE Channel Sounding Host Support.  The controller
     * refuses every CS command until the host has set it, so this has to happen
     * before anything else CS related.
     */
    rc = ble_gap_set_host_feat(47, 0x01);
    ...
}
```

**First, host feature bit 47** (LE Channel Sounding Host Support). The controller rejects every CS command until the host has declared it, so nothing CS related can precede this.

**Second, a capability read**, which is useful both as a sanity check that the attached controller really does support CS and as a log of what it can do:

```c
struct ble_cs_local_supp_cap cap;

rc = ble_cs_read_local_supp_cap(&cap);
if (rc != 0) {
    MODLOG_DFLT(ERROR, "LE CS Read Local Supported Capabilities FAILED; rc=%d\n", rc);
} else {
    MODLOG_DFLT(INFO, "CS local caps: num_antennas=%u max_antenna_paths=%u "
                "roles=0x%02x modes=0x%02x ...", ...);
}
```

If this command fails, the attached controller is not CS capable and nothing further will work — which makes it the first thing to check in the log when bringing up new hardware.

**Third, the device address.** `ble_hs_util_ensure_addr(0)` makes sure an identity exists and `ble_hs_id_infer_auto(1, &own_addr_type)` selects a random address, as the profile requires. The resulting type is cached in `s_own_addr_type` because advertising is restarted from several places later.

## Advertising

The example advertises the Ranging Service UUID (0x185B) so that a CS initiator scanning for it can find this device. There are two build variants, selected by `CONFIG_EXAMPLE_EXTENDED_ADV`.

The legacy path builds the advertisement from `struct ble_hs_adv_fields`:

```c
fields.uuids16 = (ble_uuid16_t[]) {
    BLE_UUID16_INIT(BLE_SVC_RAS_RANGING_SERVICE_VAL)
};
fields.num_uuids16 = 1;
fields.uuids16_is_complete = 1;
```

The extended path uses the extended advertising API with a raw AD payload, but deliberately keeps transmitting **legacy** PDUs:

```c
/* Advertise with legacy ADV_IND PDUs for the widest initiator
 * compatibility, so the connection is created on the 1M PHY.
 * legacy_pdu forces both PHYs to 1M; connectable legacy also
 * requires scannable (ADV_IND). */
params.legacy_pdu = 1;
params.scannable = 1;
```

So the two variants differ in which API is used, not in what goes on the air. The connection is created on the 1M PHY either way; the initiator upgrades it to 2M after connecting.

## Enabling the Reflector Role

This is the one piece of CS-specific application code in the example, and its placement is the important part:

```c
case BLE_GAP_EVENT_CONNECT:
    ...
    /* Enable the reflector role immediately, before pairing. LE CS Set
     * Default Settings is a local command and needs no encryption; the
     * initiator may send LL_CS_CAPABILITIES_REQ or LL_CS_CONFIG_REQ as
     * soon as the link is up, and the controller rejects both until
     * Role_Enable has been applied. */
    {
        struct ble_cs_reflector_setup_params params;
#if CONFIG_BT_NIMBLE_RAS_SERVICE
        params.cb = ble_svc_ras_cs_event_handler;
#else
        params.cb = NULL;
#endif
        params.cb_arg = NULL;
        params.conn_handle = event->connect.conn_handle;
        rc = ble_cs_reflector_setup(&params);
        ...
    }
    return 0;
```

Two things to take away:

1. **It happens on connect, not after encryption.** It is tempting to defer CS setup until the link is encrypted, by symmetry with the initiator's discovery. That would be wrong: the initiator can start CS capability and configuration exchanges as soon as the link is up, and the controller rejects them until the reflector role has been enabled. The command itself is local and needs no encryption.
2. **The callback is the Ranging Service handler.** `ble_svc_ras_cs_event_handler` receives the CS events from the controller and buffers each completed procedure's step data, ready to serve when the initiator asks for it. `conn_handle` must be set — the service keys its per-connection state on it.

From this point the example has nothing further to do per procedure. The controller runs the CS procedures and the Ranging Service answers the initiator's RAS traffic — feature reads, CCCD writes, `GET_RD` and `ACK_RD` control point commands, and the segmented ranging data transfers — without any application involvement.

## Other GAP Events

`BLE_GAP_EVENT_ENC_CHANGE` enforces the link security the Ranging Profile requires:

```c
if (event->enc_change.status != 0 || !desc.sec_state.encrypted ||
    !desc.sec_state.sc) {
    MODLOG_DFLT(ERROR, "encryption failed/missing or not LE Secure "
                "Connections (status=%d enc=%d sc=%d) "
                "- terminating conn=%d\n", ...);
    return ble_gap_terminate(event->enc_change.conn_handle,
                             BLE_ERR_AUTH_FAIL);
}
```

Both bits are checked, not just `encrypted`. Setting `ble_hs_cfg.sm_sc = 1` only advertises Secure Connections support in the pairing AuthReq; a peer that leaves that bit clear still completes LE Legacy Pairing and still ends up with an encrypted link, which the profile does not permit. The initiator applies the same gate.

The remaining handlers are bookkeeping. `BLE_GAP_EVENT_SUBSCRIBE` and `BLE_GAP_EVENT_NOTIFY_TX` log the initiator's CCCD writes and the outgoing data transfers at DEBUG level, which is the quickest way to confirm which ranging mode a run actually negotiated.

`BLE_GAP_EVENT_ADV_COMPLETE` has one subtlety worth noting:

```c
/* reason == 0 means the set terminated because a connection was
 * created; a CONNECT event always follows and restarts advertising
 * itself if that connection failed.  Restarting here instead races
 * the controller while it is still setting the connection up, which
 * it rejects with BLE_ERR_CONN_REJ_RESOURCES. */
if (event->adv_complete.reason == 0) {
    return 0;
}
```

Restarting advertising on every completion looks harmless but races the controller during connection setup. Advertising is restarted only on a real timeout, or from the connect and disconnect handlers.

## Disconnect Cleanup

```c
case BLE_GAP_EVENT_DISCONNECT:
    ...
#if CONFIG_BT_NIMBLE_RAS_SERVICE
    ble_svc_ras_on_disconnect(event->disconnect.conn.conn_handle);
#endif

#if MYNEWT_VAL(BLE_STATIC_TO_DYNAMIC)
    /* In a reflector-only setup the stack never frees the CS context
     * (see ble_cs.h); release it here so no CS RAM is held while
     * advertising.  The next connection re-allocates it inside
     * ble_cs_reflector_setup(). */
    ble_cs_state_deinit();
#endif
    ...
```

`ble_svc_ras_on_disconnect()` releases the service's per-connection ranging buffers. The `ble_cs_state_deinit()` call is only needed in a reflector-only setup, where nothing else in the stack owns the CS context; it keeps the device from holding CS RAM for the whole time it sits advertising.

## Conclusion

This example demonstrates:

1. Enabling the Channel Sounding host feature (bit 47) and verifying controller support with a local capability read.
2. Registering the Ranging Service as a GATT server, including the registration callback it needs.
3. Advertising the Ranging Service UUID so a CS initiator can find and connect.
4. Enabling the CS reflector role at connect time — before pairing — and routing CS events into the Ranging Service.
5. Releasing per-connection ranging and CS state on disconnect.

The initiator side of this exchange is covered in the [BLE Channel Sounding Initiator Example Walkthrough](../../ble_chan_sound_initiator/tutorial/Ble_Chan_Sound_Initiator_Example_Walkthrough.md).
