| Supported Targets | ESP32 | ESP32-S31 |
| ----------------- | ----- | --------- |

# Bluetooth Classic OPP Server Example

This example shows how to use the Bluetooth **Object Push Profile (OPP)** server APIs to accept inbound connections and receive objects over OBEX. Object body data is received through the OPP VFS (`read` / `close`).

The peer can be any OPP client, for example:

- Another ESP32 or ESP32-S31 running the `bt_opp_client` example
- A smartphone sharing a contact or file over Bluetooth
- A computer running an OPP client such as BlueZ `obexctl`

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 and ESP32-S31 development boards, e.g. ESP32-DevKitC. To operate the example, keep this board discoverable and push an object from any OPP client to the advertised local device name.

### Configure the project

1. Open the project configuration menu:

```bash
idf.py menuconfig
```

2. Enable Classic Bluetooth and OPP Server:

`Component config --> Bluetooth --> Bluedroid Options --> OPP --> OPP Server`

OPP Server is already enabled in `sdkconfig.defaults`. OPP Client is disabled.

3. Optional example options under `OPP Example Configuration`:

- `Local device name`: advertised Classic Bluetooth name (default `ESP_OPP_SERVER`).
- `Auto-accept incoming objects`: if enabled (default), every incoming object is accepted automatically. If disabled, only the first object on an OBEX connection needs `esp_opp_server_accept()`; later objects on the same connection are auto-authorized.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Description

After startup, the example initializes the OPP server and starts listening. The board is connectable and generally discoverable. An inbound client can find this device by the configured local name and push one or more objects on the same OBEX session.

On `ESP_OPP_SERVER_INCOMING_OBJECT_EVT`, if `needs_accept` is true the example calls `esp_opp_server_accept()` and waits for `ESP_OPP_SERVER_ACCEPT_EVT` for the VFS fd; otherwise it uses `incoming.fd` directly. It then `read(fd)` until EOF (`0`) and `close(fd)`. Accept is dispatched to a work queue so the Bluetooth callback path is not blocked. Until the first accept or reject, the PUT is held with no OBEX response; if neither happens within about 30 seconds the stack auto-rejects with Forbidden.

Pairing uses PIN `1234`. SSP confirmation is accepted automatically.

## Example Output

After the server starts listening:

```
I (xxx) OPP_SERVER: Local device name: ESP_OPP_SERVER address: xx:xx:xx:xx:xx:xx
I (xxx) OPP_SERVER: ESP_OPP_SERVER_INIT_EVT status:0
I (xxx) OPP_SERVER: ESP_OPP_SERVER_START_EVT status:0 scn:1
```

When a client connects and pushes an object:

```
I (xxx) OPP_SERVER: ESP_OPP_SERVER_CONNECTION_STATE_EVT handle:1 state:1 status:0 bda:xx:xx:xx:xx:xx:xx
I (xxx) OPP_SERVER: ESP_OPP_SERVER_INCOMING_OBJECT_EVT handle:1 name:esp_opp_1.vcf type:text/x-vcard len:76 fd:3 needs_accept:0
I (xxx) OPP_SERVER: RX done fd=3 total=76
I (xxx) OPP_SERVER: ESP_OPP_SERVER_TRANSFER_COMPLETE_EVT handle:1 status:0 transferred:76
```

When the client disconnects, `ESP_OPP_SERVER_CONNECTION_STATE_EVT` reports a disconnected state.

## Troubleshooting

- Confirm this board stays discoverable and that the client is targeting the configured `Local device name`.
- If `Auto-accept incoming objects` is disabled, the first object on a connection must be accepted before body data is delivered.
