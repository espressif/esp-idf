| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Throughput bleprph Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`bleprph_throughput` demonstrates server side implementation required for NimBLE throughput example. It has characteristics supporting READ, WRITE and NOTIFY (`PTS_LONG_CHR_READ_WRITE`,`PTS_CHR_READ_WRITE`,`PTS_CHR_NOTIFY`). The data of 500 Bytes (`READ_THROUGHPUT_PAYLOAD`) and 400 Bytes (`WRITE_THROUGHPUT_PAYLOAD`) is transferred for throughput GATT read and write operations respectively.

`bleprph_throughput` uses the `nimble` component as BLE host.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (573) BTDM_INIT: BT controller compile version [b73c48e]
I (573) system_api: Base MAC address is not set
I (573) system_api: read default base MAC address from EFUSE
I (663) phy: phy_version: 4500, 0cd6843, Sep 17 2020, 15:37:07, 0, 0
I (903) bleprph_throughput: BLE Host Task Started
I (933) bleprph_throughput: Device Address:
I (933) bleprph_throughput: 3c:71:bf:99:38:7a
GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (1403) bleprph_throughput: connection established; status = 0
I (1433) bleprph_throughput: mtu update event; conn_handle = 0 mtu = 512

```

In a separate terminal flash and run corresponding `blecent_throughput` for full fledged throughput demo.

If at the `blecent_throughput` side `notify` is selected as GATT operation, then below console output can be observed:

```
I (23853) bleprph_throughput: subscribe event; cur_notify=1; value handle; val_handle = 14
GATT procedure initiated: notify; att_handle=14
GATT procedure initiated: notify; att_handle=14
GATT procedure initiated: notify; att_handle=14
GATT procedure initiated: notify; att_handle=14
.
.
.

*********************************
I (83943) bleprph_throughput: Notify throughput = 160466 bps, count = 2407

*********************************
I (83943) bleprph_throughput:  Notification test completed for stipulated time of 60 sec

```

> Here, bps is bits per second; count is number of Notifications successfully sent.

## Example scope

This demo example along with `blecent_throughput` tries to demonstrate stable implementation of GATT operations like read/write and notify. For `bleprph_throughput` app, notifications are sent almost continuously for stipulated period of time. The almost part is because we use counting semaphore (~100) to mimic continuous notifications.  Here one needs to understand that notifications are sent in `os_mbufs` packets and there can always be chance of them getting full because of continuous operation, so one may need to allocate higher number of mbufs through menuconfig, whenever there is `os_mbuf` memory exhaustion, app provides delay so NimBLE host stack can breathe and free `mbuf chains`.

