| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# BLE SPP peripheral example

  In Bluetooth classic (BR/EDR) systems, a Serial Port Profile (SPP) is an adopted profile defined by the Bluetooth Special Interest Group (SIG) used to emulate a serial port connection over a Bluetooth wireless connection. For BLE systems, an adopted SPP profile over BLE is not defined, thus emulation of a serial port must be implemented as a vendor-specific custom profile.

  This reference design consists of two Demos, the BLE SPP server and BLE SPP client that run on their respective endpoints. These devices connect and exchange data wirelessly with each other. This capability creates a virtual serial link over the air. Each byte input can be sent and received by both the server and client. The SPP server is implemented as the [spp_server](../spp_server) demo while the SPP client is implemented as the [spp_client](../spp_client) demo. Espressif designed the BLE SPP applications to use the UART transport layer but you could adapt this design to work with other serial protocols, such as SPI.

  This vendor-specific custom profile is implemented in [main.c](../spp_client/main/main.c) and [main.c](../spp_server/main/main.c).

## Using Examples

### Initialization

  Both the server and client will first initialize the UART and BLE. The server demo will set up the serial port service with standard GATT and GAP services in the attribute server. The client demo will scan the BLE broadcast over the air to find the SPP server.

### Event Processing

  The SPP server has two main event processing functions for BLE event:

```c
  static int ble_spp_server_gap_event(struct ble_gap_event *event, void *arg);
  static int  ble_svc_gatt_handler(uint16_t conn_handle, uint16_t attr_handle,struct ble_gatt_access_ctxt *ctxt, void *arg);
```

  The SPP client has one main event processing functions for BLE event:

```c
  esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t * param);
```

  These are some queues and tasks used by SPP application:

  Queues:

  * spp_uart_queue       - Uart data messages received from the Uart

  Tasks:

  * `ble_server_uart_task`            - process Uart

### Packet Structure

  After the Uart received data, the data will be posted to Uart task. Then, in the UART_DATA event, the raw data may be retrieved. The max length is 120 bytes every time.
  If you run the BLE SPP demo with two ESP32 chips, the MTU size will be exchanged for 200 bytes after the ble connection is established, so every packet can be send directly.
  If you only run the ble_spp_server demo, and it was connected by a phone, the MTU size may be less than 123 bytes. In such a case the data will be split into fragments and send in turn.
  In every packet, we add 4 bytes to indicate that this is a fragment packet. The first two bytes contain "##" if this is a fragment packet, the third byte is the total number of the packets, the fourth byte is the current number of this packet.
  The phone APP need to check the structure of the packet if it want to communicate with the ble_spp_server demo.

### Sending Data Wirelessly

  The client will be sending WriteNoRsp packets to the server. The server side sends data through notifications. When the Uart receives data, the Uart task places it in the buffer.

### Receiving Data Wirelessly

   The server will receive this data in the BLE_GATT_ACCESS_OP_WRITE_CHR event.

### GATT Server Attribute Table

  charactertistic|UUID|Permissions
  :-:|:-:|:-:
  SPP_DATA_RECV_CHAR|0xABF1|READ&WRITE_NR
  SPP_DATA_NOTIFY_CHAR|0xABF2|READ&NOTIFY
  SPP_COMMAND_CHAR|0xABF3|READ&WRITE_NR
  SPP_STATUS_CHAR|0xABF4|READ & NOTIFY

This example creates GATT server and advertises data, it then gets connected to a central device.

It takes input from user and performs notify GATT operations against the specified peer.

Note :

* To install the dependency packages needed, please refer to the top level [README file](../../../../README.md#running-test-python-script-pytest).
* Currently this Python utility is only supported on Linux (BLE communication is via BLuez + DBus).

## How to use example

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
I (464) NimBLE_SPP_BLE_PRPH: BLE Host Task Started
GAP procedure initiated: stop advertising.
Device Address: 7c:df:a1:40:3e:fa
GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
connection established; status=0 handle=1 our_ota_addr_type=0 our_ota_addr=7c:df:a1:40:3e:fa our_id_addr_type=0 our_id_addr=7c:df:a1:40:3e:fa peer_ota_addr_type=0 peer_ota_addr=7c:df:a1:c2:19:92 peer_id_addr_type=0 peer_id_addr=7c:df:a1:c2:19:92 conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (6924) NimBLE_SPP_BLE_PRPH: Data received in write event,conn_handle = 1,attr_handle = 11
1b5b41I
(10824) NimBLE_SPP_BLE_PRPH: Notification sent successfully

```
