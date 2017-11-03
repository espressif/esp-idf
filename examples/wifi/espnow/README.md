# ESPNOW Example

This example shows how to use ESPNOW of wifi. Example does the following steps:

1. Start WiFi.

2. Initialize ESPNOW.

3. Register ESPNOW sending or receiving callback function.

4. Add ESPNOW peer information.

5. Send and receive ESPNOW data. 

In order to get the MAC address of the other device, firstly send broadcast ESPNOW data to each other with 'state' set as 0. When receiving 
broadcast ESPNOW data with 'state' as 0, add the device from which the data comes to the peer list. Then start sending broadcast ESPNOW 
data with 'state' set as 1. When receiving broadcast ESPNOW data with 'state' as 1, compare the local magic number with that in the data. 
If the local one is bigger than that one, stop sending broadcast ESPNOW data and start sending unicast ESPNOW data. If receive unicast 
ESPNOW data, also stop sending broadcast ESPNOW data. That is what happens in this example. It shows how to send/receive broadcast/unicast 
ESPNOW data. In practice, if the MAC address of the other device is known, it's not required to send/receive broadcast ESPNOW data first, 
just add the device to the peer list and send/receive unicast ESPNOW data.

There are a lot of "extras" on top of ESPNOW data, such as type, state, sequence number, CRC and magic in this example. These "extras" are 
not required to use ESPNOW. They are only used to make this example to run correctly. However, it is recommended that users add some "extras" 
to make ESPNOW data more safe and more reliable.

*Note:* The two devices can be set as either station or softap or station+softap mode. If the receiving device is in station mode only
and it connects to an AP, modem sleep should be disabled.

More info in the code [espnow_example_main.c](./main/espnow_example_main.c).
