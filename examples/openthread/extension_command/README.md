# Openthread Extension Commands

The ESP OpenThread examples provide a series of extension commands in addition to the standard [OpenThread CLI](https://github.com/openthread/openthread/blob/main/src/cli/README.md).
The extension commands are available in the following examples:
* [ot_cli](../ot_cli)
* [ot_br](../ot_br)

## Enabling the extension commands

To enable OpenThread extension commands, the following Kconfig option needs to be enabled:
`OpenThread Extension CLI` -> `Enable Espressif's extended features`.

## Commands

* [iperf](#iperf)
* [tcpsockclient](#tcpsockclient)
* [tcpsockserver](#tcpsockserver)
* [udpsockclient](#udpsockclient)
* [udpsockserver](#udpsockserver)
* [wifi](#wifi)

### iperf

Iperf is a tool for performing TCP or UDP throughput on the Thread network.

For running iperf, you need to have two Thread devices on the same network.

* General Options

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

* Typical usage

For measuring the TCP throughput, first create an iperf service on one node:
```bash
> iperf -V -s -t 20 -i 3 -p 5001
Done
```

Then create an iperf client connecting to the service on another node. Here we use `fdde:ad00:beef:0:a7c6:6311:9c8c:271b` as the example service address.

```bash
> iperf -V -c fdde:ad00:beef:0:a7c6:6311:9c8c:271b -t 20 -i 1 -p 5001 -l 85
Done
        Interval Bandwidth
   0-   1 sec       0.05 Mbits/sec
   1-   2 sec       0.05 Mbits/sec
   2-   3 sec       0.05 Mbits/sec
   3-   4 sec       0.05 Mbits/sec
   4-   5 sec       0.05 Mbits/sec
...
   19-   20 sec       0.05 Mbits/sec
   0-   20 sec       0.05 Mbits/sec
```

For measuring the UDP throughput, first create an iperf service similarly:

```bash
> iperf -V -u -s -t 20 -i 3 -p 5001
Done
```

Then create an iperf client:

```bash
> iperf -V -u -c fdde:ad00:beef:0:a7c6:6311:9c8c:271b -t 20 -i 1 -p 5001 -l 85
Done
```

### tcpsockserver

Used for creating a tcp server.

```bash
> tcpsockserver
Done
I (1310225) ot_socket: Socket created
I (1310225) ot_socket: Socket bound, port 12345
I (1310225) ot_socket: Socket listening, timeout is 30 seconds
```

### tcpsockclient

Used for creating a tcp client.

```bash
> tcpsockclient fdde:ad00:beef:0:a7c6:6311:9c8c:271b
Done
ot_socket: Socket created, connecting to fdde:ad00:beef:0:a7c6:6311:9c8c:271b:12345
ot_socket: Successfully connected
...
```

### udpsockserver

Used for creating a udp server.

```bash
> udpsockserver
Done
I (1310225) ot_socket: Socket created
I (1310225) ot_socket: Socket bound, port 12345
I (1310225) ot_socket: Socket listening, timeout is 30 seconds
```

### udpsockclient

Used for creating a udp client. Note that the client shall be connected to the same Thread network as the server.

```bash
> udpsockclient fdde:ad00:beef:0:a7c6:6311:9c8c:271b
Done
ot_socket: Socket created, connecting to fdde:ad00:beef:0:a7c6:6311:9c8c:271b:12345
ot_socket: Successfully connected
...
```
