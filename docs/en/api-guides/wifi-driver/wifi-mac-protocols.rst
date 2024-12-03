Wi-Fi MAC Protocols
==========================

:link_to_translation:`zh_CN:[中文]`

Wi-Fi HT20/40
--------------

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3 or esp32c5 or esp32c6

    {IDF_TARGET_NAME} supports Wi-Fi bandwidth HT20 or HT40 and does not support HT20/40 coexist. :cpp:func:`esp_wifi_set_bandwidth()` can be used to change the default bandwidth of station or AP. The default bandwidth for {IDF_TARGET_NAME} station and AP is HT40.

    In station mode, the actual bandwidth is firstly negotiated during the Wi-Fi connection. It is HT40 only if both the station and the connected AP support HT40, otherwise it is HT20. If the bandwidth of connected AP is changes, the actual bandwidth is negotiated again without Wi-Fi disconnecting.

    Similarly, in AP mode, the actual bandwidth is negotiated between AP and the stations that connect to the AP. It is HT40 if the AP and one of the stations support HT40, otherwise it is HT20.

    In station/AP coexist mode, the station/AP can configure HT20/40 separately. If both station and AP are negotiated to HT40, the HT40 channel should be the channel of station because the station always has higher priority than AP in {IDF_TARGET_NAME}. For example, the configured bandwidth of AP is HT40, the configured primary channel is 6, and the configured secondary channel is 10. The station is connected to an router whose primary channel is 6 and secondary channel is 2, then the actual channel of AP is changed to primary 6 and secondary 2 automatically.

    Theoretically, the HT40 can gain better throughput because the maximum raw physical (PHY) data rate for HT40 is 150 Mbps while it is 72 Mbps for HT20. However, if the device is used in some special environment, e.g., there are too many other Wi-Fi devices around the {IDF_TARGET_NAME} device, the performance of HT40 may be degraded. So if the applications need to support same or similar scenarios, it is recommended that the bandwidth is always configured to HT20.

.. only:: esp32c5

    .. note::

        When operating in the 2.4 GHz + 5 GHz band mode (``WIFI_BAND_MODE_AUTO``), can use the function :cpp:func:`esp_wifi_set_bandwidths()` to configure the bandwidth for the 2.4 GHz and 5 GHz bands separately.


.. only:: esp32c2

    {IDF_TARGET_NAME} supports Wi-Fi bandwidth HT20 and does not support Wi-Fi bandwidth HT40 or HT20/40 coexist.

Wi-Fi QoS
-------------------------

{IDF_TARGET_NAME} supports all the mandatory features required in WFA Wi-Fi QoS Certification.

Four ACs (Access Category) are defined in Wi-Fi specification, and each AC has its own priority to access the Wi-Fi channel. Moreover, a map rule is defined to map the QoS priority of other protocol, e.g., 802.11D or TCP/IP precedence is mapped to Wi-Fi AC.

The table below describes how the IP Precedences are mapped to Wi-Fi ACs in {IDF_TARGET_NAME}. It also indicates whether the AMPDU is supported for this AC. The table is sorted from high to low priority. That is to say, the AC_VO has the highest priority.

+------------------+------------------------+-----------------+
| IP Precedence    | Wi-Fi AC               |  Support AMPDU? |
+==================+========================+=================+
| 6, 7             | AC_VO (Voice)          |  No             |
+------------------+------------------------+-----------------+
| 4, 5             | AC_VI (Video)          |  Yes            |
+------------------+------------------------+-----------------+
| 3, 0             | AC_BE (Best Effort)    |  Yes            |
+------------------+------------------------+-----------------+
| 1, 2             | AC_BK (Background)     |  Yes            |
+------------------+------------------------+-----------------+

The application can make use of the QoS feature by configuring the IP precedence via socket option IP_TOS. Here is an example to make the socket to use VI queue::

    const int ip_precedence_vi = 4;
    const int ip_precedence_offset = 5;
    int priority = (ip_precedence_vi << ip_precedence_offset);
    setsockopt(socket_id, IPPROTO_IP, IP_TOS, &priority, sizeof(priority));

Theoretically, the higher priority AC has better performance than the lower priority AC. However, it is not always true. Here are some suggestions about how to use the Wi-Fi QoS:

 - Some really important application traffic can be put into the AC_VO queue. But avoid using the AC_VO queue for heavy traffic, as it may impact the management frames which also use this queue. Eventually, it is worth noting that the AC_VO queue does not support AMPDU, and its performance with heavy traffic is no better than other queues.
 - Avoid using more than two precedences supported by different AMPDUs, e.g., when socket A uses precedence 0, socket B uses precedence 1, and socket C uses precedence 2. This can be a bad design because it may need much more memory. To be specific, the Wi-Fi driver may generate a Block Ack session for each precedence and it needs more memory if the Block Ack session is set up.


Wi-Fi Aggregate MAC Protocol Data Unit (AMPDU)
----------------------------------------------------

{IDF_TARGET_NAME} supports both receiving and transmitting AMPDU, and the AMPDU can greatly improve the Wi-Fi throughput.

Generally, the AMPDU should be enabled. Disabling AMPDU is usually for debugging purposes.


Wi-Fi AMSDU
-------------------------

.. only:: not SOC_SPIRAM_SUPPORTED

    {IDF_TARGET_NAME} supports receiving AMSDU.

.. only:: SOC_SPIRAM_SUPPORTED

    {IDF_TARGET_NAME} supports receiving and transmitting AMSDU. AMSDU TX is disabled by default, since enable AMSDU TX need more memory. Select :ref:`CONFIG_ESP_WIFI_AMSDU_TX_ENABLED` to enable AMSDU Tx feature, it depends on :ref:`CONFIG_SPIRAM`.

Wi-Fi Fragment
-------------------------

.. only:: not SOC_WIFI_TXOP_SUPPORT

    {IDF_TARGET_NAME} supports Wi-Fi receiving fragment, but does not support Wi-Fi transmitting fragment.

.. only:: SOC_WIFI_TXOP_SUPPORT

    {IDF_TARGET_NAME} supports Wi-Fi receiving and transmitting fragment.
