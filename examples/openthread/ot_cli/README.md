# OpenThread command line example 

## Overview

This example demonstrates a [basic OpenThread command line](https://github.com/openthread/openthread/blob/master/src/cli/README.md).

## How to use example

### Hardware connection

To run this example, it's used to use an DevKit C board and connect PIN4 and PIN5 to the UART TX and RX port of another 15.4 capable radio co-processor ([RCP](https://openthread.io/platforms/co-processor?hl=en))

### Configure the project

```
idf.py menuconfig
```

The example can run with the default configuration.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

Now you'll get an interactive OpenThread command line shell.

## Example Output

```bash
I (1540) OPENTHREAD: [INFO]-PLAT----: RCP reset: RESET_POWER_ON
I (1610) OPENTHREAD: [NOTE]-PLAT----: RCP API Version: 3
I (1840) OPENTHREAD: [INFO]-CORE----: Non-volatile: Read NetworkInfo {rloc:0x7404, extaddr:aee4a5cc7ed1ad88, role:Child, mode:0x0f, version:2, keyseq:0x0, ...
I (1850) OPENTHREAD: [INFO]-CORE----: Non-volatile: ... pid:0x1161bcdc, mlecntr:0x5a17, maccntr:0x5a91, mliid:33158c466ab576d4}
I (1900) OPENTHREAD: [INFO]-CORE----: Non-volatile: Read ParentInfo {extaddr:36505631b12ea5e3, version:2}
I (1900) OPENTHREAD: [INFO]-CORE----: Notifier: StateChanged (0x1007c300) [KeySeqCntr NetData Channel PanId NetName ExtPanId MstrKey ActDset] 

> ifconfig up
I (11320) OPENTHREAD: [INFO]-CLI-----: execute command: ifconfig up
Done
I (11340) OPENTHREAD: [INFO]-CORE----: Notifier: StateChanged (0x01001009) [Ip6+ LLAddr Ip6Mult+ NetifState] 
> dataset init new
I (105650) OPENTHREAD: [INFO]-CLI-----: execute command: dataset init new
Done

> dataset
I (107460) OPENTHREAD: [INFO]-CLI-----: execute command: dataset
Active Timestamp: 1
Channel: 14
Channel Mask: 0x07fff800
Ext PAN ID: d9d69bf6535735ec
Mesh Local Prefix: fd73:192f:f27:2a5c::/64
Master Key: 7ad0ec87abbd8c41f07d004922b480bf
Network Name: OpenThread-a5fe
PAN ID: 0xa5fe
PSKc: ef028c933febdeb226f6681cc780272a
Security Policy: 672, onrcb
Done

> dataset commit active
I (134350) OPENTHREAD: [INFO]-CLI-----: execute command: dataset commit active
I (134350) OPENTHREAD: [INFO]-MESH-CP-: Active dataset set
Done
I (134380) OPENTHREAD: [INFO]-CORE----: Notifier: StateChanged (0x101fc110) [MLAddr KeySeqCntr Channel PanId NetName ExtPanId MstrKey PSKc SecPolicy ...
I (134390) OPENTHREAD: [INFO]-CORE----: Notifier: StateChanged (0x101fc110) ... ActDset] 

>thread start                                                         
I (177250) OPENTHREAD: [INFO]-CLI-----: execute command: thread start
I (177250) OPENTHREAD: [NOTE]-MLE-----: Role Disabled -> Detached
I (177280) OPENTHREAD: [INFO]-CORE----: Non-volatile: Read NetworkInfo {rloc:0x7404, extaddr:aee4a5cc7ed1ad88, role:Child, mode:0x0f, version:2, keyseq:0x0, ...
I (177290) OPENTHREAD: [INFO]-CORE----: Non-volatile: ... pid:0x1161bcdc, mlecntr:0x5a17, maccntr:0x5a91, mliid:33158c466ab576d4}            
I (194054) OPENTHREAD: [INFO]-CORE----: Non-volatile: Saved NetworkInfo {rloc:0x7404, extaddr:aee4a5cc7ed1ad88, role:Child, mode:0x0f, version:2, keyseq:0x0, ...
I (194064) OPENTHREAD: [INFO]-CORE----: Non-volatile: ... pid:0x1161bcdc, mlecntr:0x5e00, maccntr:0x5e79, mliid:33158c466ab576d4}            
I (194074) OPENTHREAD: [INFO]-MLE-----: Send Child Update Request to parent (fe80:0:0:0:3450:5631:b12e:a5e3)                                 
Done                                                                  

# After some seconds

> state
leader
Done
```

