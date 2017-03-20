# Wifi Performance Examples

Some simple codes help to test the wifi performance.

Including TCP/UDP TX/RX throughput.

#tcp_perf

Using tcp.

This example is used to test tcp throughput and delay time.

First you should set menuconfig.

You can set esp32 will be use as AP/STA, client/sever, sender/receiver in menuconfig. Also some config such as SSID, PASSWORD, SEVER_IP can be set in menuconfig.

Open AP, then open STA, when they make a connect, they will send/receive data.You will see the calc result in com output. Make sure that your set can let them connect.

Explaining more in [main.c](tcp_perf/main/main.c).




See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.
