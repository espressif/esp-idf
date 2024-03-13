ESP-NOW
=======

:link_to_translation:`zh_CN:[中文]`

Overview
--------

ESP-NOW is a kind of connectionless Wi-Fi communication protocol that is defined by Espressif. In ESP-NOW, application data is encapsulated in a vendor-specific action frame and then transmitted from one Wi-Fi device to another without connection.

CTR with CBC-MAC Protocol (CCMP) is used to protect the action frame for security. ESP-NOW is widely used in smart light, remote controlling, sensor, etc.

Frame Format
------------

ESP-NOW uses a vendor-specific action frame to transmit ESP-NOW data. The default ESP-NOW bit rate is 1 Mbps. The format of the vendor-specific action frame is as follows:

.. highlight:: none

::

    ------------------------------------------------------------------------------------------------------------
    | MAC Header | Category Code | Organization Identifier | Random Values | Vendor Specific Content |   FCS   |
    ------------------------------------------------------------------------------------------------------------
      24 bytes         1 byte              3 bytes               4 bytes             7-257 bytes        4 bytes

- Category Code: The Category Code field is set to the value (127) indicating the vendor-specific category.
- Organization Identifier: The Organization Identifier contains a unique identifier (0x18fe34), which is the first three bytes of MAC address applied by Espressif.
- Random Value: The Random Value filed is used to prevents relay attacks.
- Vendor Specific Content: The Vendor Specific Content contains vendor-specific fields as follows:

.. highlight:: none

::

    -------------------------------------------------------------------------------
    | Element ID | Length | Organization Identifier | Type | Version |    Body    |
    -------------------------------------------------------------------------------
        1 byte     1 byte            3 bytes         1 byte   1 byte   0-250 bytes

- Element ID: The Element ID field is set to the value (221), indicating the vendor-specific element.
- Length: The length is the total length of Organization Identifier, Type, Version and Body.
- Organization Identifier: The Organization Identifier contains a unique identifier (0x18fe34), which is the first three bytes of MAC address applied by Espressif.
- Type: The Type field is set to the value (4) indicating ESP-NOW.
- Version: The Version field is set to the version of ESP-NOW.
- Body: The Body contains the ESP-NOW data.

As ESP-NOW is connectionless, the MAC header is a little different from that of standard frames. The FromDS and ToDS bits of FrameControl field are both 0. The first address field is set to the destination address. The second address field is set to the source address. The third address field is set to broadcast address (0xff:0xff:0xff:0xff:0xff:0xff).

Security
--------

ESP-NOW uses the CCMP method, which is described in IEEE Std. 802.11-2012, to protect the vendor-specific action frame. The Wi-Fi device maintains a Primary Master Key (PMK) and several Local Master Keys (LMKs, each paired device has one LMK). The lengths of both PMK and LMK are 16 bytes.

    * PMK is used to encrypt LMK with the AES-128 algorithm. Call :cpp:func:`esp_now_set_pmk()` to set PMK. If PMK is not set, a default PMK will be used.
    * LMK of the paired device is used to encrypt the vendor-specific action frame with the CCMP method. If the LMK of the paired device is not set, the vendor-specific action frame will not be encrypted.

Encrypting multicast vendor-specific action frame is not supported.

Initialization and Deinitialization
------------------------------------

Call :cpp:func:`esp_now_init()` to initialize ESP-NOW and :cpp:func:`esp_now_deinit()` to de-initialize ESP-NOW. ESP-NOW data must be transmitted after Wi-Fi is started, so it is recommended to start Wi-Fi before initializing ESP-NOW and stop Wi-Fi after de-initializing ESP-NOW.

When :cpp:func:`esp_now_deinit()` is called, all of the information of paired devices are deleted.

Add Paired Device
-----------------

Call :cpp:func:`esp_now_add_peer()` to add the device to the paired device list before you send data to this device. If security is enabled, the LMK must be set. You can send ESP-NOW data via both the Station and the SoftAP interface. Make sure that the interface is enabled before sending ESP-NOW data.

.. only:: esp32c2

    The maximum number of paired devices is 20, and the paired encryption devices are no more than 4, the default is 2. If you want to change the number of paired encryption devices, set :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM` in the Wi-Fi component configuration menu.

.. only:: esp32 or esp32s2 or esp32s3 or esp32c3 or esp32c6

    The maximum number of paired devices is 20, and the paired encryption devices are no more than 17, the default is 7. If you want to change the number of paired encryption devices, set :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM` in the Wi-Fi component configuration menu.

A device with a broadcast MAC address must be added before sending broadcast data. The range of the channel of paired devices is from 0 to 14. If the channel is set to 0, data will be sent on the current channel. Otherwise, the channel must be set as the channel that the local device is on.

Send ESP-NOW Data
-----------------

Call :cpp:func:`esp_now_send()` to send ESP-NOW data and :cpp:func:`esp_now_register_send_cb()` to register sending callback function. It will return `ESP_NOW_SEND_SUCCESS` in sending callback function if the data is received successfully on the MAC layer. Otherwise, it will return `ESP_NOW_SEND_FAIL`. Several reasons can lead to ESP-NOW fails to send data. For example, the destination device does not exist; the channels of the devices are not the same; the action frame is lost when transmitting on the air, etc. It is not guaranteed that application layer can receive the data. If necessary, send back ack data when receiving ESP-NOW data. If receiving ack data timeouts, retransmit the ESP-NOW data. A sequence number can also be assigned to ESP-NOW data to drop the duplicate data.

If there is a lot of ESP-NOW data to send, call :cpp:func:`esp_now_send()` to send less than or equal to 250 bytes of data once a time. Note that too short interval between sending two ESP-NOW data may lead to disorder of sending callback function. So, it is recommended that sending the next ESP-NOW data after the sending callback function of the previous sending has returned. The sending callback function runs from a high-priority Wi-Fi task. So, do not do lengthy operations in the callback function. Instead, post the necessary data to a queue and handle it from a lower priority task.

Receiving ESP-NOW Data
----------------------

Call :cpp:func:`esp_now_register_recv_cb()` to register receiving callback function.  Call the receiving callback function when receiving ESP-NOW. The receiving callback function also runs from the Wi-Fi task. So, do not do lengthy operations in the callback function.
Instead, post the necessary data to a queue and handle it from a lower priority task.

Config ESP-NOW Rate
-------------------

.. only:: esp32 or esp32s2 or esp32s3 or esp32c2 or esp32c3

    Call :cpp:func:`esp_wifi_config_espnow_rate()` to config ESP-NOW rate of specified interface. Make sure that the interface is enabled before config rate. This API should be called after :cpp:func:`esp_wifi_start()`.

.. only:: esp32c6

    Call :cpp:func:`esp_now_set_peer_rate_config()` to configure ESP-NOW rate of each peer. Make sure that the peer is added before configuring the rate. This API should be called after :cpp:func:`esp_wifi_start()` and :cpp:func:`esp_now_add_peer()`.

    .. note::

        :cpp:func:`esp_wifi_config_espnow_rate()` is deprecated, please use cpp::func:`esp_now_set_peer_rate_config()` instead.

Config ESP-NOW Power-saving Parameter
--------------------------------------------

Sleep is supported only when {IDF_TARGET_NAME} is configured as station.

Call :cpp:func:`esp_now_set_wake_window()` to configure Window for ESP-NOW RX at sleep. The default value is the maximum, which allowing RX all the time.

If Power-saving is needed for ESP-NOW, call :cpp:func:`esp_wifi_connectionless_module_set_wake_interval()` to configure Interval as well.

.. only:: SOC_WIFI_SUPPORTED

    Please refer to :ref:`connectionless module power save <connectionless-module-power-save>` to get more detail.

Application Examples
--------------------

* Example of sending and receiving ESP-NOW data between two devices: :example:`wifi/espnow`.

* For more application examples of how to use ESP-NOW, please visit `ESP-NOW <https://github.com/espressif/esp-now>`_ repository.

API Reference
-------------

.. include-build-file:: inc/esp_now.inc
