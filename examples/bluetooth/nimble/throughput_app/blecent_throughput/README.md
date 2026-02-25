| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Throughput blecent Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`blecent_throughput` demonstrates client side implementation required for NimBLE throughput example. It connects to `bleprph_throughput` based on name string `nimble_prph`. It has interactive CLI support to start READ/WRITE/NOTIFY GATT operation for user specified time.

It performs read operations on the peripheral's `THRPT_LONG_CHR_READ_WRITE` characteristic (510 bytes), write operations on `THRPT_CHR_READ_WRITE` (509-byte payload), and subscribes to `THRPT_CHR_NOTIFY` for notifications. If the user does not specify a throughput test method within 30 seconds (`BLE_RX_TIMEOUT`), the program defaults to READ operations for 60 seconds.

`blecent_throughput` uses the `NimBLE` as BLE host.

### Procedure to Use This Demo Example

* `idf.py menuconfig` and configure the parameters as needed (connection related parameters in Example Configuration).
* The default connection interval is set to 7.5ms (6 units) for maximum throughput. This can be changed via menuconfig or at runtime.
* To test throughput on 1M PHY, make sure to disable the Example Configuration -> Enable Extended Adv flag in menuconfig on both sides.
* To test on 2M PHY or Coded PHY (S2/S8), you must enable the Example Configuration -> Enable Extended Adv flag on both sides.
* `bleprph_throughput` example needs to be run along with this client side example.
* After the connection link is established between the two devices, the user is given a window of `YES_NO_PARAM` (5 seconds) to customize connection parameters. If the user has configured parameters from menuconfig, this step can be skipped by either waiting it out or entering `Insert No`.
* Enter `Insert Yes` to customize connection parameters. Enter `MTU` and other connection parameters as directed on console instructions, e.g. `MTU 512` for MTU and `conn 6 6 0 500 12 24` for connection parameters in sequence of `min conn_itvl`, `max conn_itvl`, `latency`, `supervision timeout`, `min conn_evt_len` and `max_conn_evt_len`.
* The user will be presented with throughput test related console prints. The prints may appear like below

```
  ====================================================================================
 |                 Steps to test nimble throughput                                   |
 |                                                                                   |
 |  1. Enter throughput [--Type] [--Test time] [--Phy]                               |
 |  Type: read/write/notify.                                                         |
 |  Test time: Enter value in seconds.                                               |
 |  Phy mode: Enter value in 0 for 1M, 1 for 2M ,2 for Coded S2,                     |
 |              3 for Coded S8.                                                      |
 |                                                                                   |
 |  e.g. throughput read 600 3                                                       |
 |                                                                                   |
 |  ** Enter 'throughput read 60 0' for reading char for 60 seconds  on 1M phy       |
 |  OR 'throughput write 60 1' for writing to char for 60 seconds  on 2M phy         |
 |  OR 'throughput notify 60 2' for notifications (for 60 seconds on s=2 coded phy)**|
 |                                                                                   |
 =====================================================================================


```
* If the user fails to enter any values within 30 seconds, the app falls to the default behavior of READ for 60 seconds on 1M PHY.
* Read and write throughput numbers are displayed on the `blecent_throughput` console. For notification throughput, refer to the `bleprph_throughput` console, as the peripheral is the one sending notifications. Below is sample output:

```
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
I (1123) CLI: BLE CLI registered
I (1123) blecent_throughput: BLE Host Task Started

 ===============================================================================================
 |                         Steps to test nimble throughput                                       |
 |                                                                                               |
 |  1. Print 'help' to gain overview of commands                                                 |
 |                                                                                               |
I (1163) blecent_throughput: Do you want to configure connection params ?
I (1183) blecent_throughput: If yes then enter in this format: `Insert Yes`

 ===============================================================================================
Throughput demo >> GAP procedure initiated: discovery; own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1 duration=forever
I (6213) blecent_throughput: Event DISC
I (6213) blecent_throughput: connect; fields.num_uuids128 =0
I (6223) blecent_throughput: central connect to `nimble_prph` success
GAP procedure initiated: connect; peer_addr_type=1 peer_addr=14:58:11:c1:d0:2d scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
E (36243) blecent_throughput: Error: Connection failed; status = 13
GAP procedure initiated: discovery; own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1 duration=forever
I (36293) blecent_throughput: Event DISC
I (36293) blecent_throughput: connect; fields.num_uuids128 =0
I (36293) blecent_throughput: Name = nimble_prph
ל�j6�^n����
I (36303) blecent_throughput: central connect to `nimble_prph` success
GAP procedure initiated: connect; peer_addr_type=0 peer_addr=84:0d:8e:e6:83:de scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (36393) blecent_throughput: Connection established
GATT procedure initiated: exchange mtu
GAP procedure initiated: connection parameter update; conn_handle=0 itvl_min=6 itvl_max=6 latency=0 supervision_timeout=500 min_ce_len=8 max_ce_len=768
GATT procedure initiated: discover all services
I (36783) blecent_throughput: mtu update event; conn_handle = 0 cid = 4 mtu = 512
GATT procedure initiated: discover all characteristics; start_handle=1 end_handle=5
GATT procedure initiated: discover all characteristics; start_handle=6 end_handle=9
GATT procedure initiated: discover all characteristics; start_handle=10 end_handle=65535
GATT procedure initiated: discover all descriptors; chr_val_handle=8 end_handle=9
GATT procedure initiated: discover all descriptors; chr_val_handle=14 end_handle=15
GATT procedure initiated: discover all descriptors; chr_val_handle=17 end_handle=65535
I (36933) blecent_throughput: Service discovery complete; status=0 conn_handle=0

I (36933) blecent_throughput: Format for throughput demo:: throughput read 100
  ====================================================================================
 |                 Steps to test nimble throughput                                   |
 |                                                                                   |
 |  1. Enter throughput [--Type] [--Test time] [--Phy]                               |
 |  Type: read/write/notify.                                                         |
 |  Test time: Enter value in seconds.                                               |
 |  Phy mode: Enter value in 0 for 1M, 1 for 2M ,2 for Coded S2,                     |
 |              3 for Coded S8.                                                      |
 |                                                                                   |
 |  e.g. throughput read 600 3                                                       |
 |                                                                                   |
 |  ** Enter 'throughput read 60 0' for reading char for 60 seconds  on 1M phy       |
 |  OR 'throughput write 60 1' for writing to char for 60 seconds  on 2M phy         |
 |  OR 'throughput notify 60 2' for notifications (for 60 seconds on s=2 coded phy)**|
 |                                                                                   |
 =====================================================================================


Throughput demo >> throughput read 10
I (55333) Throughput demo handler: throughput read 10
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
Throughput demo >> GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Scope

This demo example demonstrates stable GATT read/write and notify operations at high throughput. The READ and WRITE GATT operations require an ACK from the peer, so this central app ensures that the next GATT operation is performed after completion of the previous one. For write operations, `ble_gattc_write_no_rsp_flat` is used (Write Without Response) which does not require an ATT-level ACK, allowing higher write throughput. If the mbuf pool is exhausted (`BLE_HS_ENOMEM`), the app yields briefly and retries.

For notifications (`bleprph_throughput` sends), the peripheral uses pipelined notification sending with a configurable pipeline depth, allowing the controller to pack multiple PDUs per connection event. Both sides are configured with `CONFIG_BT_NIMBLE_MSYS_1_BLOCK_COUNT=50` to provide sufficient buffer space for high-throughput operations.

## Example output

The `Procedure to use this demo example` section above covers initial console outputs. In this section we provide final console output depicting throughput numbers

```
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17
GATT procedure initiated: read; att_handle=17

****************************************************************
I (65363) blecent_throughput: Application Read throughput = 106800 bps, Read op counter = 268

****************************************************************

```

