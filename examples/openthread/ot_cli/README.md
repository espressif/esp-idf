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

## Example1: TCP/UDP server and client

### Step 1 Configure the project

```bash
idf.py menuconfig
```

Enable the operation: Example Configuration -> Enable custom command in ot-cli -> Enable openthread tcp/udp socket

### Step 2 Build, Flash, and Run
You need to prepare two ESP devices each connected to a 15.4 RCP and flashed with this example.

```
idf.py -p PORT flash monitor
```

### Step 3 Set up network

On the first device, run:
```bash
> extaddr 166e0a0000000001
Done
> dataset channel 17
Done
> dataset panid 0xface
Done
> dataset extpanid 000db80000000000
Done
> dataset networkname GRL
Done
> dataset masterkey 00112233445566778899aabbccddeeff
Done
> dataset meshlocalprefix FD00:0DB8:0000:0000::
Done
> dataset pskc 00000000000000000000000000000000
Done
> dataset activetimestamp 1
Done
> dataset commit active
Done
> ifconfig up
Done
> thread start
Done

 # After some seconds

> state
leader
Done
```

Now the first device has formed a Thread network, on the second device run:

```bash
> extaddr 166e0a0000000002
Done
> dataset channel 17
Done
> dataset panid 0xface
Done
> dataset extpanid 000db80000000000
Done
> dataset networkname GRL
Done
> dataset masterkey 00112233445566778899aabbccddeeff
Done
> dataset meshlocalprefix FD00:0DB8:0000:0000::
Done
> dataset pskc 00000000000000000000000000000000
Done
> dataset activetimestamp 1
Done
> dataset commit active
Done
> ifconfig up
Done
> thread start
Done

 # After some seconds

> state
router  # child is also a valid state
Done
```

Now the second device has joined the Thread network and acting as a router (or a child).

### Step 4 Set up tcp/udp socket server and client

In leader device, run this command in command line shell.

```bash
# for setup a tcp server
> tcpsockserver

# for setup an udp server
> udpsockserver
```

Then run this command to get the leader IPv6 address.

```bash
> ipaddr
fd00:db8:0:0:0:ff:fe00:fc00
fd00:db8:0:0:0:ff:fe00:ac00
fd00:db8:0:0:284a:cb4a:cb3b:2a42
fe80:0:0:0:146e:a00:0:1
```

In router device, run this command to set up a socket client in command line shell.

```bash
# for setup a tcp client
> tcpsockclient fd00:db8:0:0:284a:cb4a:cb3b:2a42

# for setup an udp client
> udpsockclient fd00:db8:0:0:284a:cb4a:cb3b:2a42
```

You will get

```bash
# in leader device

> tcpsockserver
I(173437) OPENTHREAD:[INFO]-CLI-----: execute command: tcpsockserver
> I (173437) ot_secket: Socket created
I (173437) ot_secket: Socket bound, port 12345
I (173457) ot_secket: Socket listening, timeout is 30 seconds
I(175007) OPENTHREAD:[INFO]-MLE-----: Send Advertisement (ff02:0:0:0:0:0:0:1)

......

I (182187) ot_secket: Received 28 bytes from client:
I (182187) ot_secket: This message is from client
I (182187) ot_secket: Socket accepted ip address: FD00:DB8::498:DDB:EC7:49DC
I (182189) ot_secket: Socket server is closed.

# in router device

> tcpsockclient fd00:db8:0:0:284a:cb4a:cb3b:2a42
I(37731) OPENTHREAD:[INFO]-CLI-----: execute command: tcpsockclient fd00:db8:0:0:284a:cb4a:cb3b:2a42
> I (37741) ot_secket: Socket created, connecting to b80d00fd:0:4acb4a28:422a3bcb:12345
I(37751) OPENTHREAD:[INFO]-ARP-----: Sending address query for fd00:db8:0:0:284a:cb4a:cb3b:2a42

......

I (38171) ot_secket: Successfully connected

......

I (38321) ot_secket: Received 28 bytes from fd00:db8:0:0:284a:cb4a:cb3b:2a42
I (38321) ot_secket: This message is from server
I (38323) ot_secket: Socket client is closed.
```

