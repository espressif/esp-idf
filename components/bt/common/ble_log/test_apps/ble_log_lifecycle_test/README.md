| Supported Targets |
| ----------------- |

# Controller / BLE Log lifecycle regression

This standalone app runs 20 init/enable/disable/deinit cycles. It supports
ESP32-C2, C5, C6, C61, H2, H21, H4 and S31. On dual-core controllers it alternates
`cfg.btdm.task_run_cpu` between CPU0 and CPU1; on the other targets the controller
runs on CPU0. The test uses the memory peripheral with an infinite buffer wait;
no log receiver or extra wiring is required.

The test stops recycling transport buffers, fills the shared pool, and sends an
HCI Reset. A test-only linker wrapper intercepts one log callback from the real
controller task and writes a marker through the production LL writer, preserving
the original callback flags. Other callbacks are suppressed while this injection
is armed so the command sender can continue. The test requires the controller
task to remain blocked inside that call for at least 20 ms before disabling it.
It verifies that log admission remains closed and queued transport data still
exists after disable. Recycling is then restored to drain queued data, and deinit
must succeed. Production builds contain neither wrapper nor injection.

Build from this directory after exporting this worktree's IDF environment;
replace `esp32h2` and the serial port with your target:

```sh
idf.py -B build -D IDF_TARGET=esp32h2 build
idf.py -B build -p /dev/ttyUSB2 flash monitor
```

Success ends with `LIFECYCLE ALL PASS (20 rounds)`. Each round prints the runtime
controller CPU, original callback flags and `disable_us`. This time includes
controller disable, not just log cancellation. The app keeps the default
FreeRTOS tick rate; the cancellation wait can take one tick before the caller
checks completion again.

## Negative control

```sh
idf.py -B build-negative -D IDF_TARGET=esp32h2 -D LOG_STOP_NEGATIVE_CONTROL=ON build
```

This variant wraps `ble_log_pause()` to skip cancellation, reproducing task
deletion while its log call is parked. It is expected to panic during LBM deinit,
with `Timed out waiting for BLE Log writers`. It is a deliberately broken test
image, not an alternate product configuration.

## Normal completion control

```sh
idf.py -B build-recycle -D IDF_TARGET=esp32h2 -D LOG_STOP_RECYCLE_CONTROL=ON build
```

This variant recycles buffers and confirms the parked log call returns normally
before controller disable. The write completes without cancellation; disable
then follows the same production path as the base test.

These cases check real task scheduling and controller teardown with an emulated
stalled transport. They do not exercise physical UART DMA or RF traffic stress.
