| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- | -------- |

# IEEE802.15.4 Debug Component

This component is used to enable several debug features, including:
- Consolidate the RX buffer statistics for IEEE802.15.4
- Consolidate the TX / RX statistics for IEEE802.15.4
- Print the recorded events for IEEE802.15.4

The use of this component is demonstrated in the `ieee802154_cli` example below, but can be similarly implemented in examples (e.g. Zigbee / Thread) for other protocol stacks. 

## How to use component 

In addition to the necessary configurations described in the `ieee802154_cli` example, some additional steps are required for configuring the board to enable the IEEE802.15.4 Debug component.

This component should be added as a dependency in `idf_component.yml`:
```
dependencies:
  cmd_ieee802154_debug:
    path: ${IDF_PATH}/examples/ieee802154/components/cmd_ieee802154_debug
```

The `register_ieee802154_debug_cmd()` function should be called upon initialization to register the commands to be used in the cli. To enable this function, use:

```
idf.py menuconfig
```

This component can be enabled through the menuconfig:

```
Component config → IEEE 802.15.4 → IEEE802154 Enable → Enable IEEE802154 Debug
```

The commands can be independently enabled / disabled using:

```
Enable IEEE802154 Debug → Rx buffer statistic
Enable IEEE802154 Debug → Record the information with IEEE802154 state and event
Enable IEEE802154 Debug → Enable record tx/rx packets information for debugging
```

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

Now you'll get an IEEE802.15.4 command line shell.

## IEEE802.15.4 Command List

In addition to the commands available in the `ieee802154_cli` example, enabling this component provides the following new commands: 

- [record](#record)
- [rxbufstat](#rxbufstat)
- [txrxstat](#txrxstat)

### record
#### record -p
Print the recorded IEEE802154 state/event/cmd etc.

```bash
ieee802154> record -p
W (130811) ieee802154: Print the record event, current event index: 14
W (130811) ieee802154: index  0: event: 0x1000,     RX_SFD_DONE, state:        RX, timestamp: 90951226 
...
W (130921) ieee802154: index 13: event: 0x   2,         RX_DONE, state:        RX, timestamp: 109182378 
W (130931) ieee802154: index 14: event: 0x   0,    Multi events, state:   DISABLE, timestamp: 0 
...
W (131061) ieee802154: index 29: event: 0x   0,    Multi events, state:   DISABLE, timestamp: 0 
W (131071) ieee802154: Print the record event done.
W (131071) ieee802154: Print the record state, current state index: 9
W (131081) ieee802154: index  0: line:  794, state:      IDLE, timestamp: 114737
W (131091) ieee802154: index  1: line:  354, state:        RX, timestamp: 90578575
...
W (131131) ieee802154: index  7: line:  354, state:        RX, timestamp: 109215261
W (131141) ieee802154: index  8: line: 1038, state:     SLEEP, timestamp: 112782354
W (131141) ieee802154: index  9: line:     , state:   DISABLE, timestamp: 0
W (131151) ieee802154: Print the record state done.
W (131161) ieee802154: Print the record cmd, current cmd index: 8
W (131161) ieee802154: index  0: line:  352, cmd:        rx, timestamp: 90578559
...
W (131211) ieee802154: index  7: line:  204, cmd:      stop, timestamp: 112782331
W (131221) ieee802154: index  8: line:     , cmd:          , timestamp: 0
W (131221) ieee802154: index  9: line:     , cmd:          , timestamp: 0
W (131231) ieee802154: Print the record cmd done.
W (131241) ieee802154: Print the record abort, current abort index: 0
W (131241) ieee802154: index  0: rx abort:    0,            RSVD, timestamp: 0
...
W (131301) ieee802154: index  9: rx abort:    0,            RSVD, timestamp: 0
W (131311) ieee802154: Print the record abort done.

```

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

### txrxstat
#### txrxstat -p
Print a summary table of rx buffer statistics.

```bash
ieee802154> txrxstat -p                                                                                                                                                                                                                                                           
W (115381) ieee802154: +--------------------+-----------------------------------+--------------------------------------------------+                                                                                                                                              
W (115381) ieee802154: |                    |Done:     0                   0.00%|Success:                 0                   0.00%|                                                                                                                                              
W (115391) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115401) ieee802154: |                    |                                   |tx_direct_num:           0                   0.00%|                                                                                                                                              
W (115411) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115431) ieee802154: |                    |                                   |tx_deferred_num:         0                   0.00%|                                                                                                                                              
W (115451) ieee802154: +                    +-----------------------------------+--------------------------------------------------+                                                                                                                                              
W (115461) ieee802154: |                    |                                   |rx_ack_coex_break:       0                   0.00%|                                                                                                                                              
W (115471) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115481) ieee802154: |                    |                                   |rx_ack_timeout:          0                   0.00%|                                                                                                                                              
W (115491) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115501) ieee802154: |TX:  0              |Abort     0                   0.00%|tx_coex_break:           0                   0.00%|                                                                                                                                              
W (115511) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115531) ieee802154: |                    |                                   |tx_security_error:       0                   0.00%|                                                                                                                                              
W (115541) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115551) ieee802154: |                    |                                   |cca_failed:              0                   0.00%|                                                                                                                                              
W (115561) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115571) ieee802154: |                    |                                   |cca_busy:                0                   0.00%|                                                                                                                                              
W (115581) ieee802154: +--------------------+-----------------------------------+--------------------------------------------------+                                                                                                                                              
W (115591) ieee802154: |                    |Done:     6                        |Success:                 6                        |                                                                                                                                              
W (115611) ieee802154: +                    +-----------------------------------+--------------------------------------------------+                                                                                                                                              
W (115621) ieee802154: |                    |                                   |tx_ack_coex_break:       0                        |                                                                                                                                              
W (115631) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115641) ieee802154: |                    |                                   |sfd_timeout:             14                       |                                                                                                                                              
W (115651) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115661) ieee802154: |                    |                                   |crc_error:               1                        |                                                                                                                                              
W (115671) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115691) ieee802154: |RX                  |Abort     17                       |filter_fail:             0                        |                                                                                                                                              
W (115701) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115711) ieee802154: |                    |                                   |no_rss:                  0                        |                                                                                                                                              
W (115721) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115731) ieee802154: |                    |                                   |rx_coex_break:           0                        |                                                                                                                                              
W (115741) ieee802154: +                    +                                   +--------------------------------------------------+                                                                                                                                              
W (115751) ieee802154: |                    |                                   |rx_restart:              2                        |                                                                                                                                              
W (115771) ieee802154: +                    +                                   +--------------------------------------------------+     
W (115781) ieee802154: |                    |                                   |ed_abort:                0                        |     
W (115791) ieee802154: +--------------------+-----------------------------------+--------------------------------------------------+ 
```

#### txrxstat -c
Clear the rx buffer statistics.

```bash
> txrxstat -c
I (7971) i154cmd: clear the txrx statistics 
```