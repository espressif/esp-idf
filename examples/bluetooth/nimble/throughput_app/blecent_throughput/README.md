| Supported Targets | ESP32 |
| ----------------- | ----- |

# Throughput blecent Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`blecent_throughput` demonstrates client side implementation required for NimBLE throughput example. It connects to `bleprph_throughput` based on name string `nimble_prph`. It has interactive CLI support to start READ/WRITE/NOTIFY GATT operation for user specified time.

It performs read operation on peripheral's `THRPT_LONG_CHR_READ_WRITE` characteristic, write operation on `THRPT_CHR_READ_WRITE` and subscribes to `THRPT_CHR_NOTIFY` characteristic for notifications. If user does not specify any throughput test method for 30 seconds (`BLE_RX_TIMEOUT`) then the program starts with default READ operations for 60 seconds.

`blecent_throughput` uses the `NimBLE` as BLE host.

### Procedure to use this demo example

* `idf.py menuconfig` and configure the parameters as needed (connection related parameters in example parameters).
* `bleprph_throughput` example needs to be run along with this client side example.
* After connection link is established between these two devices, user is given a window of `YES_NO_PARAM` (5 seconds) to customize connection parameters. If user has configured parameters from menuconfig, this step can be skipped by either waiting it out or entering `Insert no`.
* User needs to enter `Insert yes` to customize connection parameters. Enter `MTU` and other connection parameters as directed on console instructions e.g. `MTU 512` for MTU and `conn 6 120 0 500 0 0` for connection parameters in sequence of `min conn_itvl`, `max conn_itvl`, `latency`, `supervision timeout`, `min conn_evt_len` and `max_conn_evt_len`.
* User will be now presented with throughput test related console prints, this suggests application is now ready to be run throughput test for user defined time. The prints may appear like below

```
 ==================================================================
 |                 Steps to test nimble throughput                |
 |                                                                |
 |  1. Enter throughput [--Type] [--Test time]                    |
 |  Type: read/write/notify.                                      |
 |  Test time: Enter value in seconds.                            |
 |                                                                |
 |  e.g. throughput read 600                                      |
 |                                                                |
 |  ** Enter 'throughput read 60' for reading char for 60 seconds |
 |  OR 'throughput write 60' for writing to char for 60 seconds   |
 |  OR 'throughput notify 60' for notifications (for 60 seconds)**|
 |                                                                |
  =================================================================

```
* If user fail to enter any values for next 30 seconds, the app falls to default behavior of READ for 60 seconds mode.
* Read and write throughput numbers will be presented in `blecent_throughput` console output. For notification `bleprph_throughput` console shall be referred, as the peripheral is the one who is sending notifications. Below is the sample output of the app:

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
 ==================================================================
 |                 Steps to test nimble throughput                |
 |                                                                |
 |  1. Enter throughput [--Type] [--Test time]                    |
 |  Type: read/write/notify.                                      |
 |  Test time: Enter value in seconds.                            |
 |                                                                |
 |  e.g. throughput read 600                                      |
 |                                                                |
 |  ** Enter 'throughput read 60' for reading char for 60 seconds |
 |  OR 'throughput write 60' for writing to char for 60 seconds   |
 |  OR 'throughput notify 60' for notifications (for 60 seconds)**|
 |                                                                |
 =================================================================

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

## Example scope

This demo example tries to demonstrate stable implementation of GATT operations like read/write and notify. The READ and WRITE GATT operations require ACK from peer, so this central app makes sure that next GATT operation is performed after completion of previous one. In case of notification (`bleprph_throughput` sends) operation there is no waiting for ACK, however one needs to mind `os_mbufs` getting full, so one may need to allocate higher number of mbufs through menuconfig.

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

