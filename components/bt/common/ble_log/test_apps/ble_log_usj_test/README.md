# BLE Log USB Serial/JTAG test app

USB Serial/JTAG smoke app for `CONFIG_BLE_LOG_PRPH_USB_SERIAL_JTAG=y`.
The device stays the ROM USB Serial/JTAG identity (VID 0x303A PID 0x1001).
Open that CDC-ACM port for the binary log stream. Do not open the JTAG
interface. Do not scan for PID 0x10B1 or for the product string
`BLE-Log-Port`.

The app releases the console in `sdkconfig.defaults`
(`ESP_CONSOLE_UART_DEFAULT` and `ESP_CONSOLE_SECONDARY_NONE`). It does
not turn the console off through a Kconfig `select`.

## Modes

- smoke (default): one 8-byte record every 100 ms.
- `CONFIG_BLE_LOG_USJ_TEST_PERF_MODE`: CPU-bound back-to-back writes for
  a host-measured throughput. UART stays the console so loss warnings
  still print there.

Dispatcher teardown lives in `ble_log_usb_test` lifecycle mode. That
path is shared `COPY_FIFO` code, not USJ-specific.

## Build, flash, run

Flash may use the USJ ACM node or a UART-bridge COM port.

```bash
cd components/bt/common/ble_log/test_apps/ble_log_usj_test
idf.py set-target <chip>
idf.py -p <FLASH_PORT> build flash
```

Do not run `idf.py monitor` on the USJ ACM node. That node carries binary
BLE Log frames. The console is UART (`ESP_CONSOLE_UART_DEFAULT`).
`ESP_LOG`, init failures, perf summaries, and loss warnings print there.

On C3, C5, and C6 the onboard Type-C ACM node (`/dev/ttyACM*` on Linux)
is the log port, not the console. UART0 is on the header pins and needs
a USB-UART adapter if the board has no UART bridge.

```bash
idf.py -p <UART_PORT> monitor
```

`<UART_PORT>` is the UART console device. It is not the ROM USJ
`303a:1001` ACM node.

For perf mode:

```bash
echo CONFIG_BLE_LOG_USJ_TEST_PERF_MODE=y > sdkconfig.perf
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.perf" set-target esp32c5 build flash
```

## Supported targets

ESP32-C3, ESP32-C5, ESP32-C6, ESP32-C61, ESP32-H2, ESP32-H21, ESP32-H4,
ESP32-S3, ESP32-S31, ESP32-P4 (`SOC_USB_SERIAL_JTAG_SUPPORTED`).

CI builds are temporarily disabled until BLE Log test runners are available.
