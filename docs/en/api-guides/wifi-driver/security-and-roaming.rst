Security and Roaming
============================

:link_to_translation:`zh_CN:[中文]`


Supported Wi-Fi Security Modes
--------------------------------

{IDF_TARGET_NAME} supports the following Wi-Fi security modes. For detailed configuration and usage, refer to :doc:`Wi-Fi Security <../wifi-security>`.

- Open
- WEP
- WPA-Personal
- WPA2-Personal
- WPA3-Personal
- Wi-Fi Enterprise


Wi-Fi Easy Connect™ (DPP)
--------------------------

Wi-Fi Easy Connect\ :sup:`TM` (or Device Provisioning Protocol) is a secure and standardized provisioning protocol for configuring Wi-Fi devices. More information can be found in :doc:`esp_dpp <../../api-reference/network/esp_dpp>`.


WPS Enrollee
-------------------------

{IDF_TARGET_NAME} supports WPS enrollee feature in Wi-Fi mode :cpp:enumerator:`WIFI_MODE_STA` or :cpp:enumerator:`WIFI_MODE_APSTA`. Currently, {IDF_TARGET_NAME} supports WPS enrollee type PBC and PIN.


Wireless Network Management
----------------------------

Wireless Network Management allows client devices to exchange information about the network topology, including information related to RF environment. This makes each client network-aware, facilitating overall improvement in the performance of the wireless network. It is part of 802.11v specification. It also enables the client to support Network assisted Roaming.
- Network assisted Roaming: Enables WLAN to send messages to associated clients, resulting clients to associate with APs with better link metrics. This is useful for both load balancing and in directing poorly connected clients.

Current implementation of 802.11v includes support for BSS transition management frames.

Radio Resource Measurement
---------------------------

Radio Resource Measurement (802.11k) is intended to improve the way traffic is distributed within a network. In a WLAN, each device normally connects to the access point (AP) that provides the strongest signal. Depending on the number and geographic locations of the subscribers, this arrangement can sometimes lead to excessive demand on one AP and underutilization of others, resulting in degradation of overall network performance. In a network conforming to 802.11k, if the AP having the strongest signal is loaded to its full capacity, a wireless device can be moved to one of the underutilized APs. Even though the signal may be weaker, the overall throughput is greater because more efficient use is made of the network resources.

Current implementation of 802.11k includes support for beacon measurement report, link measurement report, and neighbor request.

Refer ESP-IDF example :idf_file:`examples/wifi/roaming/README.md` to set up and use these APIs. Example code only demonstrates how these APIs can be used, and the application should define its own algorithm and cases as required.

Fast BSS Transition
---------------------------

Fast BSS transition (802.11r FT), is a standard to permit continuous connectivity aboard wireless devices in motion, with fast and secure client transitions from one Basic Service Set (abbreviated BSS, and also known as a base station or more colloquially, an access point) to another performed in a nearly seamless manner **avoiding 802.11i 4 way handshake**. 802.11r specifies transitions between access points by redefining the security key negotiation protocol, allowing both the negotiation and requests for wireless resources to occur in parallel. The key derived from the server to be cached in the wireless network, so that a reasonable number of future connections can be based on the cached key, avoiding the 802.1X process.


{IDF_TARGET_NAME} station supports FT for WPA2-PSK networks. Do note that {IDF_TARGET_NAME} station only support FT over the air protocol only.

A config option :ref:`CONFIG_ESP_WIFI_11R_SUPPORT` and configuration parameter :cpp:type:`ft_enabled` in :cpp:type:`wifi_sta_config_t` is provided to enable 802.11r support for station. Refer ESP-IDF example :idf_file:`examples/wifi/roaming/README.md` for further details.

.. only:: SOC_WIFI_FTM_SUPPORT

    Wi-Fi Location
    -------------------------------

    Wi-Fi Location will improve the accuracy of a device's location data beyond the Access Point, which will enable creation of new and feature-rich applications and services such as geo-fencing, network management, and navigation. One of the protocols used to determine the device location with respect to the Access Point is Fine Timing Measurement which calculates Time-of-Flight of a Wi-Fi frame.

    Fine Timing Measurement (FTM)
    +++++++++++++++++++++++++++++

    FTM is used to measure Wi-Fi Round Trip Time (Wi-Fi RTT) which is the time a Wi-Fi signal takes to travel from a device to another device and back again. Using Wi-Fi RTT, the distance between the devices can be calculated with a simple formula of `RTT * c / 2`, where c is the speed of light.

    FTM uses timestamps given by Wi-Fi interface hardware at the time of arrival or departure of frames exchanged between a pair of devices. One entity called FTM Initiator (mostly a station device) discovers the FTM Responder (can be a station or an Access Point) and negotiates to start an FTM procedure. The procedure uses multiple Action frames sent in bursts and its ACK's to gather the timestamps data. FTM Initiator gathers the data in the end to calculate an average Round-Trip-Time.

    {IDF_TARGET_NAME} supports FTM in below configuration:

    - {IDF_TARGET_NAME} as FTM Initiator in station mode.
    - {IDF_TARGET_NAME} as FTM Responder in AP mode.

.. only:: esp32c6

   {IDF_TARGET_NAME} ECO1 and older versions do not support FTM Initiator mode.

.. attention::

    Distance measurement using RTT is not accurate, and factors such as RF interference, multi-path travel, antenna orientation, and lack of calibration increase these inaccuracies. For better results, it is suggested to perform FTM between two ESP32 chip series devices as station and AP.

    Refer to ESP-IDF example :idf_file:`examples/wifi/ftm/README.md` for steps on how to set up and perform FTM.
