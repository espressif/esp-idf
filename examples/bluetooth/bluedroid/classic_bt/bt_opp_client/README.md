| Supported Targets | ESP32 | ESP32-S31 |
| ----------------- | ----- | --------- |

# Bluetooth Classic OPP Client Example

This example shows how to use the Bluetooth **Object Push Profile (OPP)** client APIs to discover a remote OPP server by device name, connect, and push objects over OBEX. Object body data is sent through the OPP VFS (`write` / `close`).

The peer can be any discoverable OPP server, for example:

- Another ESP32 or ESP32-S31 running the `bt_opp_server` example
- A smartphone that accepts Bluetooth object push
- A computer running an OPP server such as BlueZ `obexd`

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 and ESP32-S31 development boards, e.g. ESP32-DevKitC. To operate the example, make the target OPP server discoverable and set its Bluetooth name to the value configured in this project.

### Configure the project

1. Open the project configuration menu:

```bash
idf.py menuconfig
```

2. Enable Classic Bluetooth and OPP Client:

`Component config --> Bluetooth --> Bluedroid Options --> OPP --> OPP Client`

OPP Client is already enabled in `sdkconfig.defaults`. OPP Server is disabled.

3. Set the remote OPP server name:

`OPP Example Configuration --> Target device name`

The default is `ESP_OPP_SERVER`. Change it to the advertised name of the peer you want to connect to. An empty name is rejected at runtime.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Description

After startup, the example initializes the OPP client and starts a general inquiry. It filters discovery results by the configured target name (from EIR or the Bluetooth device name). When a match is found, inquiry is cancelled and the client connects.

After the connection is established, the example opens three built-in vCards in sequence. For each object it calls `esp_opp_client_open_object()`. On `ESP_OPP_CLIENT_OPEN_EVT` it writes the full declared length to the callback fd, then `close(fd)` to finish the object (End-of-Body). When all objects have been sent, the client disconnects.

Pairing uses PIN `1234`. SSP confirmation is accepted automatically.

If the peer does not answer an OBEX CONNECT/PUT/DISCONNECT within about 30 seconds, the client aborts and disconnects. Application write stalls do not count toward that timeout.

## Example Output

After OPP client initialization, inquiry starts:

```
I (xxx) OPP_CLIENT: ESP_OPP_CLIENT_INIT_EVT status:0
I (xxx) OPP_CLIENT: Discovering peer name: ESP_OPP_SERVER
```

When the target is found and the connection succeeds:

```
I (xxx) OPP_CLIENT: Connecting to ESP_OPP_SERVER (xx:xx:xx:xx:xx:xx)
I (xxx) OPP_CLIENT: ESP_OPP_CLIENT_CONNECTION_STATE_EVT handle:1 state:1 status:0 bda:xx:xx:xx:xx:xx:xx
I (xxx) OPP_CLIENT: Sending object[1/3] name:esp_opp_1.vcf len:76
```

Each successful push is reported as:

```
I (xxx) OPP_CLIENT: ESP_OPP_CLIENT_TRANSFER_COMPLETE_EVT handle:1 status:0 transferred:76
```

After the last object, the client disconnects and reports `ESP_OPP_CLIENT_CONNECTION_STATE_EVT` with a disconnected state.

## Troubleshooting

- Confirm the peer is discoverable and that `Target device name` matches its advertised Bluetooth name exactly.
- If discovery finishes without a match, power-cycle the peer or start this example again so inquiry can run while the server is advertising.
