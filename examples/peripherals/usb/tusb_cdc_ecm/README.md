# TinyUSB CDC-ECM Example

Demonstrates how to use the ESP-IDF Framework API to perform a ethernet connection via USB.
Afterwards two TCP Servers are started, which are listen on the Ports 3333 and 1234.
In the folder tcp_client, you can find a tcp client written in python to test the communication.
The client opens two sockets to address both ports. This shall demonstrate, that multiple ports can be served at once.
