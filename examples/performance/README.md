# Wifi Performance Examples

Some simple codes help to test the wifi performance.

Including TCP/UDP TX/RX throughput.

# tcp_perf

This example is used to test tcp throughput and delay time. 

Step1: Set options in `make menuconfig` like ssid, password, server ip and server port. And choose what the esp32 will work as.

* AP or STA. You can set one esp32 as AP and another esp32 as STA with same ssid & password, also you can use Router or wifi adapter instead of one of these.

* Client or server. Make sure the client has correct server ip & port so they can get connected. It's okay if you create a tcp server & client using PC since one of the wifi device is't esp32.

* Send or receive. Set one of them sending data and the other receiving.

Step2: Exit menuconfig, saving the new settings. Then build the app and flash it to the ESP32.

Step3: Start test. And here are some things that might help you do the test easily.

* You'd better turn on the AP before the STA.
* The tcp server should be started before the tcp client.
* If you use a esp32 as AP, you'd better use it as tcp server also.
* Once the tcp connection crashed, esp32 should be restarted to re-establish TCP connection.

Step4: View the result. After connection established, TCP server and TCP client can send data to each other. The result of throughput will be printed in the serial log.

See [main.c](./tcp_perf/main/main.c) for full details.

# udp_perf

This example is similar to tcp_perf. Also the steps is similar to tcp_perf.

There's a obvious difference between udp_perf and tcp perf:

Before formal sending & receiving, a packet will be send from client to server. So the server can know the ip&port of client. It is usually eaiser to set the UDP server as the receiver.

See [main.c](./udp_perf/main/main.c) for full details.

# More

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.
