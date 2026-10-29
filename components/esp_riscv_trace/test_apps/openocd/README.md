| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S31 |
| ----------------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# RISC-V Trace OpenOCD Test App

This app tests the RISC-V trace encoder with OpenOCD attached over
USB-Serial-JTAG. The console runs on an external UART.

The firmware runs a branchy workload on every traced core and reads console
commands.

- `panic` calls `abort()`.
- `api_start` prints the result of `esp_riscv_trace_start()` on core 0.
- `status` prints `esp_riscv_trace_get_status()` for every traced core.

## Test Configurations

- `ocd`: OpenOCD support with start and stop control. Autostart is off.
- `ocd_autostart`: autostart capture without core dump. OpenOCD can only read
  the trace.

## Tests

`test_riscv_trace_openocd_start_stop` runs a loop capture started and stopped
by `esp riscv_trace` commands, and checks the refusals while it runs, `dump`
after the stop, a timed capture, and that the encoder stays stopped. The
written `.esptrace` files are parsed on the host.

`test_riscv_trace_openocd_panic` triggers a panic during an OpenOCD loop
capture, with and without a duration. The driver finalizes the snapshot before
the debugger halt, and `stop` writes the panic capture at once without running
firmware code. It also checks that the session is released, so a new capture
works after a reset.

`test_riscv_trace_openocd_autostart_panic` checks that OpenOCD refuses to start
a capture and to dump a running one under autostart. After a panic it dumps the
autostart capture left by the panic, which gives the trace without core dump.

The tests need an OpenOCD build with the `esp riscv_trace` commands, so CI
skips them for now. To run them locally, put that OpenOCD first in `PATH` and
set `OPENOCD_SCRIPTS` to its `tcl` directory.
