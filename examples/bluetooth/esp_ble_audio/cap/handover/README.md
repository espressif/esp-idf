| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# CAP Handover Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements a device with a **collocated CAP Initiator and CAP Commander** and moves
the audio back and forth between unicast and broadcast using the **CAP Handover procedures**
(CAP v1.0.1 §7.3.1.10 Unicast to Broadcast, §7.3.1.11 Broadcast to Unicast).

It connects to a CAP Acceptor exactly like [`../initiator/`](../initiator/) does — scan, connect,
pair, exchange MTU, discover CAS / sink ASEs / source ASEs — and additionally discovers the
Acceptor's **Broadcast Audio Scan Service (BASS)**, which the Commander writes into to tell the
Acceptor about the broadcast source. Once unicast audio is streaming, a timer alternates between
the two procedures every `EXAMPLE_HANDOVER_PERIOD_S`.

**Only the sink direction is handed over.** A broadcast Audio Stream is one-directional, so the
return (Acceptor to Initiator) stream is not part of the procedure: §7.3.1.10 applies to "all CISes
within the CIG that is carrying an Audio Stream from the Initiator to the Acceptor", and leaves the
reverse direction implementation specific.

The very same `bt_cap_stream` objects carry the audio before and after a handover — that is a
requirement of the CAP handover API, not a shortcut: the procedure validates that every broadcast
stream is one of the unicast group's streaming sink streams, and that *all* of them are handed over.
Consequently there is **no standalone broadcast source** in this example: it is created by the
unicast-to-broadcast procedure and deleted by the reverse one.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* A second board running [`../acceptor/`](../acceptor/) with **both** `EXAMPLE_UNICAST` and
  `EXAMPLE_BROADCAST` enabled (its `sdkconfig.defaults` already does), so it exposes ASCS and BASS
  at the same time
* The Acceptor must **not** be built with `EXAMPLE_SCAN_SELF` (its Kconfig already makes that
  mutually exclusive with `EXAMPLE_UNICAST`) — see *Periodic advertising sync* below

## Periodic advertising sync: PAST only

This example is a **collocated broadcaster**: the same device is the Broadcast Source and the
Commander that tells the Acceptor about it. It therefore never scans for its own broadcast, and it
hands the periodic advertising train over with **PAST** rather than asking the Acceptor to find it.

* The Add Source operation is written with `PA_Sync = 0x01` (*synchronize, PAST available*).
* The Acceptor answers by putting its Broadcast Receive State into `PA_Sync_State = 1`
  (*SyncInfo Request*).
* Seeing that state, this example sends **LE Periodic Advertising Set Info Transfer** — the
  variant for handing over one of *our own* advertising sets. (The other variant, Sync Transfer,
  applies when an Assistant relays a sync it holds to a third-party broadcaster; that is not this
  case.) The Source ID goes in the **high octet** of the service data, which is where the Acceptor
  reads it from.

There is deliberately **no self-scan fallback**: a real earbud is not asked to scan for a source
its phone already knows about. If the Acceptor were built to self-scan instead, it would sit at
"Waiting for BASE" forever.

## Configuration

```bash
idf.py menuconfig
```

Under **Example: CAP Handover**:

* `EXAMPLE_HANDOVER_PERIOD_S` (default 300) — seconds between switches. Set to 0 to stay on
  unicast after the initial setup.

> **Size note.** The Broadcast Assistant (BASS *client*) is genuinely used — it is how the
> Commander writes Add Source / Modify Source / Remove Source. The Scan Delegator (BASS
> *server*) is **not**: it is pulled in only because every branch of `BT_CAP_COMMANDER`'s
> `depends on` that avoids it requires some other client role we do not need either
> (`BT_VCP_VOL_CTLR`, `BT_MICP_MIC_CTLR`, `BT_TBS_CLIENT`, `BT_MCC`). CAP §7.3.1.8 only requires
> a Commander to act as Scan Delegator when it is **not collocated** with the Initiator **and**
> the stream is encrypted — neither holds here — so this is an over-constraint in the Kconfig
> rather than a real dependency.

### Security & Pairing

Just-Works pairing (LE Secure Connections, no MITM) with bonding, inherited from
`../../common_components/example_init/ble_audio_example_init.c`.

## Build & Flash

```bash
idf.py set-target esp32h4
idf.py -p PORT flash monitor
```

For the **NimBLE** host, layer the overlay:

```bash
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.esp32h4;sdkconfig.defaults.nimble" -p PORT flash monitor
```

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. `app_main` brings up NVS, the controller, the LE Audio common layer, the unicast role, the
   handover module and the TX pump, then starts scanning.
2. On finding CAS in a connectable advertisement: connect, pair, exchange MTU, GATT discovery.
3. Discover CAS, then the sink and source ASEs, then **BASS**.
4. **Sweep the Acceptor's Broadcast Receive States.** Discovery only subscribes to them, so the
   example reads each one once and removes any left over from an earlier run of ours (matched by
   Broadcast ID). Receive states live on the Acceptor and survive a reflash of *this* board, and
   BAP §6.5.4 forbids an Add Source that would duplicate the
   {address, SID, Broadcast ID} triple — without the sweep the first handover after a reboot
   fails with `0xFC` (Write Request Rejected). The sweep then continues into
   `cap_handover_unicast_setup_and_start()`.
5. A unicast group is created with one CIS per sink stream (the return direction shares the first
   CIS) and the streams start.
6. **Unicast to broadcast**, every `EXAMPLE_HANDOVER_PERIOD_S`:
   * the advertising set is brought up (extended advertising started, periodic advertising
     configured but *stopped* — the BASE does not exist yet);
   * the local public address and SID are registered with the audio stack, because the BASS Add
     Source operation carries them;
   * `esp_ble_audio_cap_handover_unicast_to_broadcast()` stops and releases the unicast group,
     creates the broadcast source, and has the Commander write Add Source to the Acceptor;
   * from the `created` callback the BASE is encoded and periodic advertising is started.
7. **Broadcast to unicast**, one period later: the Commander stops the reception on the Acceptor
   (using the Source ID learned from the Broadcast Receive State), the broadcast source is stopped
   and deleted, a new unicast group is created and the streams start again; advertising is stopped.
8. On ACL disconnect any broadcast source left over is deleted and scanning resumes.

## Expected Log

TAG: `CAP_HOV`.

```
I (xxx) CAP_HOV: CAP initiator unicast initialized
I (xxx) CAP_HOV: CAP initiator handover initialized (period 60 s)
I (xxx) CAP_HOV: Scanning for CAP Acceptor...
I (xxx) CAP_HOV: Found CAS in peer adv data!
I (xxx) CAP_HOV: Connected: handle ... role ... peer ...
I (xxx) CAP_HOV: Discover sources complete
I (xxx) CAP_HOV: Discovering BASS
I (xxx) CAP_HOV: BASS discovered (1 receive state(s))
I (xxx) CAP_HOV: Created unicast group
I (xxx) CAP_HOV: [SNK #0] Stream started
I (xxx) CAP_HOV: [SNK #1] Stream started
I (xxx) CAP_HOV: Unicast start completed

I (xxx) CAP_HOV: Handover: unicast -> broadcast (2 stream(s))
I (xxx) CAP_HOV: Advertising started, BASE pending (handle 0)
I (xxx) CAP_HOV: Periodic advertising started (handle 0)
I (xxx) CAP_HOV: Acceptor receive state: src_id 1 pa_sync 2
I (xxx) CAP_HOV: Handover to broadcast completed

I (xxx) CAP_HOV: Handover: broadcast -> unicast (2 stream(s))
I (xxx) CAP_HOV: Advertising stopped (handle 0)
I (xxx) CAP_HOV: Handover to unicast completed
```

Note the TX label follows the transport: `[SNK #0]` while the stream runs on a CIS, `[SRC #0]`
once the same stream object is bound to a broadcast source endpoint. Seeing `[SRC #0] TX: 6000
packets` during a broadcast period is the check that audio really is going out over the BIS.

On the acceptor side each switch shows the sink ASEs being released and a BASS Add Source arriving,
then the reverse.

### Timing

Each procedure is strictly sequential, so there is a silent gap at every switch. Measured on
ESP32-S31 with two sink streams, it is stable to within a few milliseconds:

| Direction | Duration |
|---|---|
| unicast to broadcast | ~1.69 s |
| broadcast to unicast | ~2.65 s |

## Diagnostics

Either side of every handover the example samples the ISO timing of each stream and logs it under
`[<phase>]`, where phase is `u2b-pre`, `u2b-post`, `b2u-pre` or `b2u-post`:

```
I (xxx) CAP_HOV: [u2b-pre][0] state 4 ts 66125612 us offset 0 us seq 6025
I (xxx) CAP_HOV: [u2b-pre][0] iso_interval 10000 us cig_sync_delay 2256 us cis_sync_delay 2256 us
I (xxx) CAP_HOV: [u2b-pre][0] c2p {ft 1 bn 1 latency 2256 us sdu_interval unknown (v1 event)}
I (xxx) CAP_HOV: [u2b-pre][0] p2c {ft 1 bn 0 latency 0 us sdu_interval unknown (v1 event)}
I (xxx) CAP_HOV: [u2b-pre][0] pd_pref [20000, 40000] us pd_range [20000, 40000] us
```

A BIS reports the same first line plus its own terms, and no QoS preference:

```
I (xxx) CAP_HOV: [u2b-post][0] state 4 (timestamp not read: transport just started)
I (xxx) CAP_HOV: [u2b-post][0] iso_interval 10000 us big_sync_delay 1974 us
I (xxx) CAP_HOV: [u2b-post][0] bis {pto 0 bn 1 latency 1974 us}
```

These are the terms CAP §7.3.1.10's rendering-point alignment would need. Reading them:

* `ts` is `TX_Time_Stamp`: the CIG reference point or BIG anchor point, taken from the
  controller's free-running clock. Both transports use the same clock, so the two can be
  subtracted directly.
* The **post** samples deliberately do not read `ts`. HCI answers *Command Disallowed* until a
  stream has sent its first SDU, and a transport that has just come up has not; the timestamp for
  the new transport shows up in the next **pre** sample instead.
* `sdu_interval` is reported as unknown because it is only carried by
  `HCI_LE_CIS_Established_V2`; the controller sends the v1 event. Use the QoS value instead.
* `latency` is the transport latency the controller actually achieved, not the maximum the preset
  asked for.
* `ft` is derived: the host stores `flush_timeout = FT x ISO_Interval`, so it is divided back out
  here — the alignment arithmetic wants FT itself.
* Only a CIS prints `pd_pref` / `pd_range`. Those come from the peer ASE's Codec Configured
  notification, and a broadcast source has no peer to state a preference, so the fields would
  only ever read zero.

## Peer Pairing

1. Flash [`../acceptor/`](../acceptor/) on one board with both roles enabled (default).
2. Flash this example on the other board.
3. Audio starts on CISes, then alternates with BISes every period.

## Notes

* The handover procedures in this stack are **sequential**: the unicast streams are stopped before
  the broadcast source is created (CAP allows this as the "Initiator does not have the resources to
  concurrently run both" path). There is therefore an audible gap at each switch. CAP §7.3.1.10 also
  recommends aligning the rendering points of the two streams, which only makes sense on the
  concurrent path and is not implemented.
* `Streaming_Audio_Contexts` and `CCID_List` are required by the specification to be the same on
  both sides of a handover. So is the **codec configuration**: a different sampling frequency,
  frame duration or frame size would make the Acceptor's decoder reconfigure exactly at the
  switching point. Both paths therefore take their configuration from one
  `HANDOVER_LC3_PRESET_DEFINE()` in `cap_handover.h`, and `handover_audio_config_check()` rejects
  the handover if they ever diverge. Only the channel allocation differs by design: unicast puts
  one channel in each stream, broadcast puts it in a per-BIS LTV.
* The **Source ID is assigned by the Acceptor**, not chosen here — the Add Source operation has no
  such field. It is learned from the Broadcast Receive State and then used for Modify/Remove
  Source. It keeps counting across a reflash of this board and only restarts when the *Acceptor*
  reboots, which is why the receive-state sweep in step 4 exists.

## Troubleshooting

| Symptom | Cause |
|---|---|
| First handover after reflashing this board fails with `err 252` | A receive state left on the Acceptor by the previous run. The sweep in step 4 clears it; if it was removed, this is what comes back. |
| Acceptor logs "Syncing without PAST", then waits for the BASE forever | The Add Source went out with `PA_Sync = 0x02`. Check that the advertising set is registered with `esp_ble_audio_bap_broadcast_adv_add()` before the handover — its address and SID are what make PAST available. |
| Broadcast period is silent, no `[SRC #x] TX:` line | The TX pump did not register for the broadcast streams. It decides with `bt_bap_ep_info.can_send`; deriving it from `dir` instead does not work, because a broadcast source endpoint reports `dir = SOURCE` while a unicast client endpoint reports the peer's direction. |
