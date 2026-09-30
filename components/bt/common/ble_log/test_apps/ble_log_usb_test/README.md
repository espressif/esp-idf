# BLE Log USB CDC Test App

## Overview

USB OTG device app for the BLE Log TinyUSB CDC-ACM transport
(`CONFIG_BLE_LOG_PRPH_USB=y`). It serves three measurement roles and one
regression role, selected by Kconfig:

- **smoke** (default, no mode option set): one small record every 100 ms, to
  check enumeration and a live byte stream end to end.
- **`CONFIG_BLE_LOG_USB_TEST_PERF_MODE`**: CPU-bound back-to-back writes for
  host-measured throughput. The host counts frames, bytes, per-source SN
  continuity and payload checksums; the device also prints its accepted and
  rejected record counts.
- **`CONFIG_BLE_LOG_USB_TEST_RAW_PIPE_MODE`**: the same TinyUSB CDC device, but
  a task writes fixed 640-byte blocks straight into the CDC TX FIFO with no
  BLE Log runtime. Comparing raw-pipe with perf attributes the gap between the
  transport path and the raw pipe.
- **`CONFIG_BLE_LOG_USB_TEST_LIFECYCLE_MODE`**: deterministic regression for
  the dispatcher teardown handshake, no host and no USB traffic needed. See
  below.

The USB console and the log share one CDC port; the ESP console stays on the
UART. `ble_log_usb_test/CMakeLists.txt` documents the target set.

## Lifecycle Check

The USB runtime dispatches from a dedicated task that owns a transport between
the queue receive and the recycle, so deinit must not tear the pool or the
peripheral down while a dispatch call is in flight.

The mode forces those windows instead of racing for them, with one check per
window.

The first check parks the dispatch task inside the dispatch call while it owns
the only queued transport and the runtime queue is empty; deinit then runs from
a second task. It fails if deinit completes while that call is still parked, or
if the parked call does not resume after it is released.

The second check parks the task after it published the stop ack and before it
suspends itself - the window in which deinit has the ack but the handle is not
safe to delete yet. It fails if deinit completes inside that window. Deleting a
still-running task there makes the kernel's terminated-task cleanup lose the
TCB and stack, so deinit waits for the suspended state, not just for the ack.

The single sealing write is sized to fill one transport exactly, so no
partially filled transport is left behind for the periodic flush to seal, and
the check additionally fails loudly if it ever reaches the first periodic
boundary. A failing run prints `TEST FAIL:` and aborts; a passing run prints
`TEST PASS:` once per check.

## Build, Flash, Run

```bash
cd components/bt/common/ble_log/test_apps/ble_log_usb_test
idf.py set-target <chip>
idf.py -p <PORT> build flash monitor
```

For a mode other than smoke, set the mode option in `sdkconfig.defaults` (or
`sdkconfig`), for example through a local overlay:

```bash
echo CONFIG_BLE_LOG_USB_TEST_PERF_MODE=y > sdkconfig.perf
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.perf" set-target esp32s3 build flash monitor
```

The CDC buffer options must satisfy the component's configuration checks:
`CONFIG_TINYUSB_CDC_TX_BUFSIZE` and `CONFIG_TINYUSB_CDC_EP_BUFSIZE` must each
**equal** the matching `CONFIG_BLE_LOG_USB_CDC_*` value, and
`CONFIG_TINYUSB_CDC_RX_BUFSIZE` must be at least
`CONFIG_TINYUSB_CDC_EP_BUFSIZE`. See the component README for the measured
per-speed buffer guidance.

## Supported Targets

| Supported Targets | ESP32-S2 | ESP32-S3 | ESP32-P4 | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- |

CI builds are temporarily disabled until BLE Log test runners are available.
