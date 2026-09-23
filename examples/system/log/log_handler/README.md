| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Log Message Handler Example

This example intercepts ordinary logs, writes selected messages to files in internal flash, and optionally keeps standard console output. See the [examples guide](../../../README.md) for general setup.

| Message | File output | Normal console output |
| --- | --- | --- |
| `SECURITY`, any enabled level | `security.log` only | No |
| `SENSOR`, warning or error | `sensor.log` and `warnings.log` | Yes |
| Other `SENSOR` messages | `sensor.log` | Yes |
| Other warnings and errors | `warnings.log` | Yes |
| Everything else | None | Yes |

The application installs its handler first. The separate `security_log` component installs a wrapper and saves the previous function and context:

```text
security_log -> application handler -> standard output
```

A matching `SECURITY` message returns `false` immediately. Other messages call the previous handler and return its result. Only `true` reaching the library permits standard output; there is no automatic handler list.

## Usage

### Hardware Required

A supported development board with at least 2 MB flash and a USB connection. No external storage is needed.

### Set Chip Target

From this example directory, select your board's chip, for example:

```sh
idf.py set-target esp32
```

### Configure the Project

No configuration is required. [sdkconfig.defaults](sdkconfig.defaults) selects Log V2, text output and a SPIFFS partition. To try V1, select it under **Component config > Log** in `idf.py menuconfig`. Binary logging is not supported by these printf-based file handlers.

Mounting formats the example storage partition if necessary. All three files are overwritten on every run.

### Build and Flash

```sh
idf.py build flash monitor
```

Exit the monitor with `Ctrl` + `]`.

## Example Output

Captured on ESP32 with Log V2; timestamps vary. Security messages are absent from normal logging, but the example deliberately displays their file afterwards for inspection.

```text
I (370) APP: Startup complete
I (370) SENSOR: Temperature: 25 C
W (370) WIFI: Connection lost, retry=1
E (370) SENSOR: Read failed: error=-1

--- /logs/warnings.log ---
Connection lost, retry=1
Read failed: error=-1

--- /logs/sensor.log ---
Temperature: 25 C
Read failed: error=-1

--- /logs/security.log ---
Audit started
Access attempt requires review
Access denied
Log handler example complete
```

## Example Breakdown

- `main/main.c` owns the warnings and sensor streams. It uses `va_copy()` to preserve arguments for the next file or handler.
- `components/security_log` owns its stream and previous-handler pair. It formats into a 256-byte buffer (longer text is truncated), writes once, and returns `false`.
- All sample messages use `ESP_LOGx`. V1 already includes the standard prefix in the format; V2 file output contains only the message text.

The example emits synchronously, removes wrappers in reverse order, then closes and dumps the files. A concurrent application must finish its producer calls before closing their streams: changing the handler does not wait for old calls. Files are flushed on close; rotation and write-error recovery are outside this example.

These rules apply to ordinary logs that pass level filtering. Early, DRAM and recursively emitted logs bypass the handlers. V2 also bypasses them in constrained contexts; V1 requires ordinary logs to be called from normal execution contexts.
