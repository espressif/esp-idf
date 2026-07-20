| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# RISC-V Trace Basic Test App

This app validates the basic RISC-V trace encoder driver flow on targets that
support the trace peripheral. It is intentionally small: the tests focus on
startup configuration, start/stop sequencing, trace-buffer capture, status
reporting, and basic filter programming.

## Test Configurations

Pytest runs the app with two configurations:

- `default`: uses the driver-allocated internal trace buffer.
- `psram`: enables PSRAM and selects `CONFIG_ESP_RISCV_TRACE_BUFFER_IN_EXTERNAL`
  so the driver allocates the trace buffer from external RAM. This configuration
  also enables CPU stall-on-FIFO-full to avoid losing coverage when the external
  memory target is slower than internal RAM.

Both configurations inherit `sdkconfig.defaults`, which enables the driver,
sets packet resynchronization, and configures the trace buffer size.

## Unity Test Cases

- `RISC-V trace loop capture`
  - Starts tracing on the configured core mask.
  - Runs a branch-heavy workload to fill and wrap the loop buffer.
  - Stops tracing, checks the encoder status, and decodes the wrapped buffer
    from surviving resynchronization anchors.

- `RISC-V trace address-window filter`
  - Captures a baseline trace with filtering disabled.
  - Programs an instruction-address window filter around one workload function.
  - Confirms the filtered capture is smaller and that sync packets land inside
    the selected address window.

## Running

The pytest entry points run all Unity cases for each selected configuration:

```bash
idf-ci build run -t esp32p4 -p components/esp_riscv_trace/test_apps/basic
pytest --target esp32p4 components/esp_riscv_trace/test_apps/basic
```

For manual hardware validation, build and flash the app with the desired
configuration and run all Unity cases from the device menu.
