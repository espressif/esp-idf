# Wifi Performance Examples

Some simple codes help to test the wifi performance.

Including TCP/UDP TX/RX throughput.

# tcp_perf

This example is used to test tcp throughput and delay time. First you should set options in menuconfig.

You can set esp32 as AP/STA, client/sever, sender/receiver. Make sure that STAcan connect to AP with your configuration in menuconfig.

So the tcp_perf can be used in following scenes:

* esp32 to Router (using esp32 as STA)
* esp32 to Wifi adaptor (using esp32 as AP)
* esp32 to esp32 (using one of them as AP, the other STA)

After connection established, TCP sever and TCP client can send data to each other. The result of throughput will show by COM.

Explaining more in [main.c](./tcp_perf/main/main.c).

# udp_perf

Similar with tcp_perf.

There are some points need to notice.

* A packet will be send from client to sever.So the sever can konw the ip&port of client.

* To easy use this example, it's better to use udp sever as recviver.


# More

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.
