ESP-NOW
=======

Overview
--------

ESP-NOW is a kind of connectionless WiFi communication protocol which is defined by Espressif. In ESP-NOW, application data is 
encapsulated in vendor-specific action frame and then transmitted from one WiFi device to another without connection. 
CTR with CBC-MAC Protocol(CCMP) is used to protect the action frame for security. ESP-NOW is widely used in smart light, remote 
controlling, sensor, etc.

Frame Format
------------

ESP-NOW uses vendor-specific action frame to transmit ESP-NOW data. The format of vendor-specific action frame is as follows:

.. highlight:: none

::

    ----------------------------------------------------------------------------------------
    | MAC Header | Category Code | Organization Identifier | Vendor Specific Content | FCS |
    ----------------------------------------------------------------------------------------
                       1 byte              3 bytes                  7~255 bytes

- Category Code: The Category field is set to the value(127) indicating the vendor-specific category.
- Organization Identifier: The Organization Identifier contains a unique identifier(0x18fe34) which is the first three bytes 
                           of MAC address applied by Espressif.
- Vendor Specific Content: The Vendor Specific Content contains vendor-specific field as follows:

.. highlight:: none

::

    -------------------------------------------------------------------------------
    | Element ID | Length | Organization Identifier | Type | Version |    Body    |
    -------------------------------------------------------------------------------
        1 byte     1 byte            3 bytes         1 byte   1 byte   0~250 bytes
    
- Element ID: The Element ID field is set to the value(221) indicating the vendor-specific element.
- Length: The length is the total length of Organization Identifier, Type, Version and Body.
- Organization Identifier: The Organization Identifier contains a unique identifier(0x18fe34) which is the first three bytes 
                           of MAC address applied by Espressif.
- Type: The Type field is set to the value(4) indicating ESP-NOW.
- Version: The Version field is set to the version of ESP-NOW.
- Body: The Body contains the ESP-NOW data.

As ESP-NOW is connectionless, the MAC header is a little different from that of standard frames. The FromDS and ToDS bits of 
FrameControl field are both 0. The first address field is set to the destination address. The second address field is set to 
the source address. The third address field is set to broadcast address(0xff:0xff:0xff:0xff:0xff:0xff).

Security
--------

ESP-NOW use CCMP method which can be referenced in IEEE Std. 802.11-2012 to protect the vendor-specific action frame. The WiFi 
device maintains a Primary Master Key(PMK) and several Local Master Keys(LMK). The lengths of them are 16 bytes. PMK is used 
to encrypt LMK with AES-128 algorithm. Call ``esp_now_set_pmk()`` to set PMK. If PMK is not set, a default PMK will be used. 
If LMK of the paired device is set, it will be used to encrypt the vendor-specific action frame with CCMP method. The maximum 
number of different LMKs is six. Do not support encrypting multicast vendor-specific action frame. 

Initialization and De-initialization
------------------------------------

Call ``esp_now_init()`` to initialize ESP-NOW and ``esp_now_deinit()`` to de-initialize ESP-NOW. ESP-NOW data must be transmitted
after WiFi is started, so it is recommended to start WiFi before initializing ESP-NOW and stop WiFi after de-initializing ESP-NOW.
When ``esp_now_deinit()`` is called, all of the information of paired devices will be deleted.

Add Paired Device
-----------------

Before sending data to other device, call ``esp_now_add_peer()`` to add it to the paired device list first. The maximum number of 
paired devices is twenty. If security is enabled, the LMK must be set. ESP-NOW data can be sent from station or softap interface. 
Make sure that the interface is enabled before sending ESP-NOW data. A device with broadcast MAC address must be added before 
sending broadcast data. The range of the channel of paired device is from 0 to 14. If the channel is set to 0, data will be sent 
on the current channel. Otherwise, the channel must be set as the channel that the local device is on.

Send ESP-NOW Data
-----------------

Call ``esp_now_send()`` to send ESP-NOW data and ``esp_now_register_send_cb`` to register sending callback function. It will return 
`ESP_NOW_SEND_SUCCESS` in sending callback function if the data is received successfully on MAC layer. Otherwise, it will return 
`ESP_NOW_SEND_FAIL`. There are several reasons failing to send ESP-NOW data, for example, the destination device doesn't exist, the 
channels of the devices are not the same, the action frame is lost when transmiting on the air, etc. It is not guaranteed that 
application layer can receive the data. If necessary, send back ack data when receiving ESP-NOW data. If receiving ack data timeout 
happens, retransmit the ESP-NOW data. A sequence number can also be assigned to ESP-NOW data to drop the duplicated data.

If there is a lot of ESP-NOW data to send, call ``esp_now_send()`` to send less than or equal to 250 bytes of data once a time. 
Note that too short interval between sending two ESP-NOW datas may lead to disorder of sending callback function. So, it is 
recommended that sending the next ESP-NOW data after the sending callback function of previous sending has returned. The sending 
callback function runs from a high-priority WiFi task. So, do not do lengthy operations in the callback function. Instead, post 
necessary data to a queue and handle it from a lower priority task.

Receiving ESP-NOW Data
----------------------

Call ``esp_now_register_recv_cb`` to register receiving callback function. When receiving ESP-NOW data, receiving callback function 
is called. The receiving callback function also runs from WiFi task. So, do not do lengthy operations in the callback function. 
Instead, post necessary data to a queue and handle it from a lower priority task.

API Reference
-------------

.. include:: /_build/inc/esp_now.inc
