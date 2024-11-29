| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- | -------- |

# IEEE802.15.4 RX Buffer Statistics Component

This component is used to consolidate the RX buffer statistics for IEEE802.15.4. The use of this component is demonstrated in the `ieee802154_cli` example, but can be similarly implemented for other stack examples (e.g. Zigbee / Thread). 

## How to use component 

In addition to the necessary configurations described in the `ieee802154_cli` example, some additional steps are required for configuring the board to enable the RX buffer statistics component.

```
idf.py menuconfig
```

This component can be enabled through the menuconfig:

```
Component config → IEEE 802.15.4 → IEEE802154 Enable → Enable IEEE802154 Debug → Rx buffer statistic
```

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

Now you'll get an IEEE802.15.4 command line shell.

## IEEE802.15.4 Command List

In addition to the commands available in the `ieee802154_cli` example, enabling this component provides a new command: 

- [rxbufstat](#rxbufstat)

### rxbufstat
#### rxbufstat -p
Print a summary table of rx buffer statistics.

```bash
> rxbufstat -p
W (913011) ieee802154: +-------------------------+-------------------------+
W (913011) ieee802154: |      rx buff total size:|20                       |
W (913021) ieee802154: |      buffer alloc times:|80                       |
W (913021) ieee802154: +-------------------------+-------------------------+
W (913031) ieee802154: |   0%    ~  10%     used:|80                100.00%|
W (913031) ieee802154: |  10%    ~  20%     used:|0                   0.00%|
W (913041) ieee802154: |  20%    ~  30%     used:|0                   0.00%|
W (913051) ieee802154: |  30%    ~  40%     used:|0                   0.00%|
W (913051) ieee802154: |  40%    ~  50%     used:|0                   0.00%|
W (913061) ieee802154: |  50%    ~  60%     used:|0                   0.00%|
W (913061) ieee802154: |  60%    ~  70%     used:|0                   0.00%|
W (913081) ieee802154: |  70%    ~  80%     used:|0                   0.00%|
W (913091) ieee802154: |  80%    ~  90%     used:|0                   0.00%|
W (913091) ieee802154: |  90%    ~ 100%     used:|0                   0.00%|
W (913101) ieee802154: |               full used:|0                   0.00%|
W (913101) ieee802154: +-------------------------+-------------------------+
```

#### rxbufstat -c
Clear the rx buffer statistics.

```bash
> rxbufstat -c
I (7971) i154cmd: clear the rx buffer statistics 
```