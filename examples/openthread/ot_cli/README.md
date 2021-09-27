| Supported Targets | ESP32-H2 |
| ----------------- | -------- |

# OpenThread Command Line Example

This example demonstrates an [OpenThread CLI](https://github.com/openthread/openthread/blob/master/src/cli/README.md), with some additional features such as TCP, UDP and Iperf.

## How to use example

### Hardware Required

To run this example, an ESP32-H2 board is required.

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

Now you'll get an OpenThread command line shell.

### Example Output

The `help` command will print all of the supported commands.
```bash
>  help
I(7058) OPENTHREAD:[INFO]-CLI-----: execute command: help
bbr
bufferinfo
ccathreshold
channel
child
childip
childmax
childsupervision
childtimeout
coap
contextreusedelay
counters
dataset
delaytimermin
diag
discover
dns
domainname
eidcache
eui64
extaddr
extpanid
factoryreset
...
```

## Set Up Network

To run this example, at least two ESP32-H2 boards flashed with this ot_cli example are required.

On the first device, run the following commands:
```bash
> factoryreset
... # the device will reboot

> dataset init new
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
Now the first device has formed a Thread network as a leader. Get some information which will be used in next steps:
```bash
> ipaddr
fdde:ad00:beef:0:0:ff:fe00:fc00
fdde:ad00:beef:0:0:ff:fe00:8000
fdde:ad00:beef:0:a7c6:6311:9c8c:271b
fe80:0:0:0:5c27:a723:7115:c8f8

# Get the Active Dataset
> dataset active -x
0e080000000000010000000300001835060004001fffe00208fe7bb701f5f1125d0708fd75cbde7c6647bd0510b3914792d44f45b6c7d76eb9306eec94030f4f70656e5468726561642d35383332010258320410e35c581af5029b054fc904a24c2b27700c0402a0fff8
```

On the second device, set the active dataset from leader, and start Thread interface:
```bash
> factoryreset
... # the device will reboot

> dataset set active 0e080000000000010000000300001835060004001fffe00208fe7bb701f5f1125d0708fd75cbde7c6647bd0510b3914792d44f45b6c7d76eb9306eec94030f4f70656e5468726561642d35383332010258320410e35c581af5029b054fc904a24c2b27700c0402a0fff8
> ifconfig up
Done
> thread start
Done

# After some seconds

> state
router  # child is also a valid state
Done
```
The second device has joined the Thread network as a router (or a child).

## TCP and UDP Example

On the leader device, start a TCP or UDP server:
```bash
> tcpsockserver
Done
I (1310225) ot_socket: Socket created
I (1310225) ot_socket: Socket bound, port 12345
I (1310225) ot_socket: Socket listening, timeout is 30 seconds
```
or (UDP Server)
```bash
> udpsockserver
Done
I (1339815) ot_socket: Socket created
I (1339815) ot_socket: Socket bound, port 54321
I (1339815) ot_socket: Waiting for data, timeout is 30 seconds
Done
```

On router device, start a TCP or UDP client (replace with the leader's IPv6 address):
```bash
> tcpsockclient fdde:ad00:beef:0:a7c6:6311:9c8c:271b
Done
ot_socket: Socket created, connecting to fdde:ad00:beef:0:a7c6:6311:9c8c:271b:12345
ot_socket: Successfully connected
...
```
or (UDP Client)
```bash
> udpsockclient fdde:ad00:beef:0:a7c6:6311:9c8c:271b
Done
ot_socket: Socket created, sending to fdde:ad00:beef:0:a7c6:6311:9c8c:271b:54321
ot_socket: Message sent
...
```

### Iperf Example

Print the iperf help:
```bash
iperf
---iperf parameter---
-s                  :     server mode, only receive
-u                  :     upd mode
-V                  :     use IPV6 address  
-c <addr>           :     client mode, only transmit
-i <interval>       :     seconds between periodic bandwidth reports
-t <time>           :     time in seconds to transmit for (default 10 secs)
-p <port>           :     server port to listen on/connect to
-l <len_send_buf>   :     the lenth of send buffer
---example---
create a tcp server :     iperf -s -i 3 -p 5001 -t 60 
create a udp client :     iperf -c <addr> -u -i 3 -t 60 -p 5001 -l 512
Done
```

On the leader device, start iperf TCP or UDP server:
```bash
> iperf -V -s -i 3 -p 5001 -t 20
i:3
dp:5001
sp:5001
t:20
Done
```
or (UDP Server)
```bash
> iperf -V -s -u -i 3 -p 5001 -t 20
i:3
dp:5001
sp:5001
t:20
Done
```

On the router device, start iperf TCP or UDP client:

```bash
> iperf -V -c fdde:ad00:beef:0:a7c6:6311:9c8c:271b -i 1 -t 5 -p 5001 -l 85
ip:fdde:ad00:beef:0:a7c6:6311:9c8c:271b
i:1
t:5
dp:5001
sp:5001
Done
        Interval Bandwidth
   0-   1 sec       0.05 Mbits/sec
   1-   2 sec       0.05 Mbits/sec
   2-   3 sec       0.05 Mbits/sec
   3-   4 sec       0.05 Mbits/sec
   4-   5 sec       0.05 Mbits/sec
   0-   5 sec       0.05 Mbits/sec
```
or (UDP Client)
```bash
> iperf -V -c fdde:ad00:beef:0:a7c6:6311:9c8c:271b -u -i 1 -t 5 -p 5001 -l 85
ip:fdde:ad00:beef:0:a7c6:6311:9c8c:271b
i:1
t:5
dp:5001
sp:5001
Done
   0-   1 sec       0.05 Mbits/sec
   1-   2 sec       0.05 Mbits/sec
   2-   3 sec       0.05 Mbits/sec
   3-   4 sec       0.05 Mbits/sec
   4-   5 sec       0.05 Mbits/sec
   0-   5 sec       0.05 Mbits/sec
```
