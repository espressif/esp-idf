# Wifi Performance Examples

Some simple codes help to test the wifi performance.

Including TCP/UDP TX/RX throughput.

# tcp_perf

This example is used to test tcp throughput and delay time. First you should set options in menuconfig.

Steps:

* Step1: Set a AP and a STA with same SSID & PASSWORD. You can set one esp32 as AP and another esp32 as STA, also you can use Router or wifi adaptor instead one of the them.

* Step2: Set a tcp client and a sever. Make sure the client has correct sever ip & port so they can get connected. It's okay if you creat a tcp sever & client using PC since one of the wifi device is't esp32.

* Step3: Set a sender and a receiver. Set one of them sending data and the other receiving.

* Step4: Save your settings and make bin file downloading to flash.

* Step5: Start test. 

Here are some things that might help you do the test easily.

* You'd better turn on the AP before the STA.
* The tcp sever should be built before the tcp client.
* If you use a esp32 as AP, you'd better use it as tcp sever also.
* Once the tcp connection crashed, esp32 should be restarted to rebuild tcp connection.

After connection established, TCP sever and TCP client can send data to each other. The result of throughput will show by COM.

Explaining more in [main.c](./tcp_perf/main/main.c).

# udp_perf

This example is similar to tcp_perf. Also the steps is similar to tcp_perf.

There's a obvious difference between udp_perf and tcp perf:

Before formal sending & receiving, a packet will be send from client to sever. So the sever can konw the ip&port of client. Maybe it's easily to use if you set the udp sever as receiver.

Explaining more in [main.c](./udp_perf/main/main.c).

# More

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.
