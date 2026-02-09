Wi-Fi Vendor Features
===================================

:link_to_translation:`zh_CN:[中文]`

.. only:: SOC_WIFI_MESH_SUPPORT

    ESP-WIFI-MESH
    -------------------------

    For details, see the :doc:`ESP-WIFI-MESH <../esp-wifi-mesh>`.


Wi-Fi 80211 Packet Send
---------------------------

The :cpp:func:`esp_wifi_80211_tx()` API can be used to:

 - Send the beacon, probe request, probe response, and action frame.
 - Send the non-QoS data frame.

It cannot be used for sending encrypted or QoS frames.

Preconditions of Using :cpp:func:`esp_wifi_80211_tx()`
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

 - The Wi-Fi mode is station, or AP, or station/AP.
 - Either ``esp_wifi_set_promiscuous(true)``, or :cpp:func:`esp_wifi_start()`, or both of these APIs return :c:macro:`ESP_OK`. This is because Wi-Fi hardware must be initialized before :cpp:func:`esp_wifi_80211_tx()` is called. In {IDF_TARGET_NAME}, both ``esp_wifi_set_promiscuous(true)`` and :cpp:func:`esp_wifi_start()` can trigger the initialization of Wi-Fi hardware.
 - The parameters of :cpp:func:`esp_wifi_80211_tx()` are hereby correctly provided.

Data Rate
+++++++++++++++++++++++++++++++++++++++++++++++

 - The default data rate is 1 Mbps.
 - Can set any rate through :cpp:func:`esp_wifi_config_80211_tx_rate()` API.
 - Can set any bandwidth through :cpp:func:`esp_wifi_set_bandwidth()` API.

Side-Effects to Avoid in Different Scenarios
+++++++++++++++++++++++++++++++++++++++++++++++++++++

Theoretically, if the side-effects the API imposes on the Wi-Fi driver or other stations/APs are not considered, a raw 802.11 packet can be sent over the air with any destination MAC, any source MAC, any BSSID, or any other types of packet. However, robust or useful applications should avoid such side-effects. The table below provides some tips and recommendations on how to avoid the side-effects of :cpp:func:`esp_wifi_80211_tx()` in different scenarios.

.. list-table::
   :header-rows: 1
   :widths: 10 50

   * - Scenario
     - Description
   * - No Wi-Fi connection
     - In this scenario, no Wi-Fi connection is set up, so there are no side-effects on the Wi-Fi driver. If ``en_sys_seq==true``, the Wi-Fi driver is responsible for the sequence control. If ``en_sys_seq==false``, the application needs to ensure that the buffer has the correct sequence.

       Theoretically, the MAC address can be any address. However, this may impact other stations/APs with the same MAC/BSSID.

       Side-effect example#1 The application calls :cpp:func:`esp_wifi_80211_tx()` to send a beacon with BSSID == mac_x in AP mode, but the mac_x is not the MAC of the AP interface. Moreover, there is another AP, e.g., “other-AP”, whose BSSID is mac_x. If this happens, an “unexpected behavior” may occur, because the stations which connect to the “other-AP” cannot figure out whether the beacon is from the “other-AP” or the :cpp:func:`esp_wifi_80211_tx()`.

       To avoid the above-mentioned side-effects, it is recommended that:

       - If :cpp:func:`esp_wifi_80211_tx` is called in station mode, the first MAC should be a multicast MAC or the exact target-device’s MAC, while the second MAC should be that of the station interface.

       - If :cpp:func:`esp_wifi_80211_tx` is called in AP mode, the first MAC should be a multicast MAC or the exact target-device’s MAC, while the second MAC should be that of the AP interface.

       The recommendations above are only for avoiding side-effects and can be ignored when there are good reasons.

   * - Have Wi-Fi connection
     - When the Wi-Fi connection is already set up, and the sequence is controlled by the application, the latter may impact the sequence control of the Wi-Fi connection as a whole. So, the ``en_sys_seq`` need to be true, otherwise ``ESP_ERR_INVALID_ARG`` is returned.

       The MAC-address recommendations in the “No Wi-Fi connection” scenario also apply to this scenario.

       If the Wi-Fi mode is station mode, the MAC address1 is the MAC of AP to which the station is connected, and the MAC address2 is the MAC of station interface, it is said that the packet is sent from the station to AP. Otherwise, if the Wi-Fi is in AP mode, the MAC address1 is the MAC of the station that connects to this AP, and the MAC address2 is the MAC of AP interface, it is said that the packet is sent from the AP to station. To avoid conflicting with Wi-Fi connections, the following checks are applied:

       - If the packet type is data and is sent from the station to AP, the ToDS bit in IEEE 80211 frame control should be 1 and the FromDS bit should be 0. Otherwise, the packet will be discarded by Wi-Fi driver.

       - If the packet type is data and is sent from the AP to station, the ToDS bit in IEEE 80211 frame control should be 0 and the FromDS bit should be 1. Otherwise, the packet will be discarded by Wi-Fi driver.

       - If the packet is sent from station to AP or from AP to station, the Power Management, More Data, and Re-Transmission bits should be 0. Otherwise, the packet will be discarded by Wi-Fi driver.

       ``ESP_ERR_INVALID_ARG`` is returned if any check fails.


Wi-Fi Vendor IE Configuration
-----------------------------------

By default, all Wi-Fi management frames are processed by the Wi-Fi driver, and the application can ignore them. However, some applications may have to handle the beacon, probe request, probe response, and other management frames. For example, if you insert some vendor-specific IE into the management frames, it is only the management frames which contain this vendor-specific IE that will be processed. In {IDF_TARGET_NAME}, :cpp:func:`esp_wifi_set_vendor_ie()` and :cpp:func:`esp_wifi_set_vendor_ie_cb()` are responsible for this kind of tasks.


.. only:: SOC_WIFI_CSI_SUPPORT

    Wi-Fi Channel State Information
    ------------------------------------

    .. only:: esp32 or esp32s2 or esp32c3 or esp32s3

        Channel state information (CSI) refers to the channel information of a Wi-Fi connection. In {IDF_TARGET_NAME}, this information consists of channel frequency responses of sub-carriers and is estimated when packets are received from the transmitter. Each channel frequency response of sub-carrier is recorded by two bytes of signed characters. The first one is imaginary part and the second one is real part. There are up to three fields of channel frequency responses according to the type of received packet. They are legacy long training field (LLTF), high throughput LTF (HT-LTF), and space time block code HT-LTF (STBC-HT-LTF). For different types of packets which are received on channels with different state, the sub-carrier index and total bytes of signed characters of CSI are shown in the following table.

        +-------------+--------------------+-----------------------------------------+--------------------------------------------------------+----------------------------------------------------------+
        | channel     | secondary channel  |                   none                  |                           below                        |                            above                         |
        +-------------+--------------------+-------------+---------------------------+----------+---------------------------------------------+----------+-----------------------------------------------+
        | packet      | signal mode        |   non HT    |            HT             |  non HT  |                      HT                     |  non HT  |                       HT                      |
        +             +--------------------+-------------+---------------------------+----------+-----------------+---------------------------+----------+-------------------+---------------------------+
        | information | channel bandwidth  |    20 MHz   |           20 MHz          |   20 MHz |      20 MHz     |            40 MHz         |   20 MHz |       20 MHz      |            40 MHz         |
        +             +--------------------+-------------+-------------+-------------+----------+----------+------+-------------+-------------+----------+----------+--------+-------------+-------------+
        |             | STBC               |  non STBC   |  non STBC   |     STBC    | non STBC | non STBC | STBC |  non STBC   |     STBC    | non STBC | non STBC |  STBC  |  non STBC   |     STBC    |
        +-------------+--------------------+-------------+-------------+-------------+----------+----------+------+-------------+-------------+----------+----------+--------+-------------+-------------+
        | sub-carrier | LLTF               | 0~31, -32~-1| 0~31, -32~-1| 0~31, -32~-1|   0~63   |   0~63   | 0~63 |     0~63    |     0~63    |  -64~-1  |  -64~-1  | -64~-1 |    -64~-1   |    -64~-1   |
        +             +--------------------+-------------+-------------+-------------+----------+----------+------+-------------+-------------+----------+----------+--------+-------------+-------------+
        | index       | HT-LTF             |      -      | 0~31, -32~-1| 0~31, -32~-1|     -    |   0~63   | 0~62 | 0~63, -64~-1| 0~60, -60~-1|     -    |  -64~-1  | -62~-1 | 0~63, -64~-1| 0~60, -60~-1|
        +             +--------------------+-------------+-------------+-------------+----------+----------+------+-------------+-------------+----------+----------+--------+-------------+-------------+
        |             | STBC-HT-LTF        |      -      |      -      | 0~31, -32~-1|     -    |     -    | 0~62 |       -     | 0~60, -60~-1|     -    |     -    | -62~-1 |       -     | 0~60, -60~-1|
        +-------------+--------------------+-------------+-------------+-------------+----------+----------+------+-------------+-------------+----------+----------+--------+-------------+-------------+
        | total bytes                      |     128     |     256     |     384     |    128   |    256   | 380  |      384    |      612    |    128   |    256   |   376  |      384    |      612    |
        +----------------------------------+-------------+-------------+-------------+----------+----------+------+-------------+-------------+----------+----------+--------+-------------+-------------+

        All of the information in the table can be found in the structure wifi_csi_info_t.

            - Secondary channel refers to secondary_channel field of rx_ctrl field.
            - Signal mode of packet refers to sig_mode field of rx_ctrl field.
            - Channel bandwidth refers to cwb field of rx_ctrl field.
            - STBC refers to stbc field of rx_ctrl field.
            - Total bytes refers to len field.
            - The CSI data corresponding to each Long Training Field (LTF) type is stored in a buffer starting from the buf field. Each item is stored as two bytes: imaginary part followed by real part. The order of each item is the same as the sub-carrier in the table. The order of LTF is: LLTF, HT-LTF, STBC-HT-LTF. However, all 3 LTFs may not be present, depending on the channel and packet information (see above).
            - If first_word_invalid field of :cpp:type:`wifi_csi_info_t` is true, it means that the first four bytes of CSI data is invalid due to a hardware limitation in {IDF_TARGET_NAME}.
            - More information like RSSI, noise floor of RF, receiving time and antenna is in the rx_ctrl field.

        When imaginary part and real part data of sub-carrier are used, please refer to the table below.

        +-----------------+-------------------+------------------------------+--------------------------+
        | PHY standard    | Sub-carrier range | Pilot sub-carrier            | Sub-carrier (total/data) |
        +=================+===================+==============================+==========================+
        | 802.11a/g       | -26 to +26        | -21, -7, +7, +21             | 52 total, 48 usable      |
        +-----------------+-------------------+------------------------------+--------------------------+
        | 802.11n, 20 MHz | -28 to +28        | -21, -7, +7, +21             | 56 total, 52 usable      |
        +-----------------+-------------------+------------------------------+--------------------------+
        | 802.11n, 40 MHz | -57 to +57        | -53, -25, -11, +11, +25, +53 | 114 total, 108 usable    |
        +-----------------+-------------------+------------------------------+--------------------------+

        .. note::

            - For STBC packet, CSI is provided for every space-time stream without CSD (cyclic shift delay). As each cyclic shift on the additional chains shall be -200 ns, only the CSD angle of first space-time stream is recorded in sub-carrier 0 of HT-LTF and STBC-HT-LTF for there is no channel frequency response in sub-carrier 0. CSD[10:0] is 11 bits, ranging from -pi to pi.

            - If LLTF, HT-LTF, or STBC-HT-LTF is not enabled by calling API :cpp:func:`esp_wifi_set_csi_config()`, the total bytes of CSI data will be fewer than that in the table. For example, if LLTF and HT-LTF is not enabled and STBC-HT-LTF is enabled, when a packet is received with the condition above/HT/40MHz/STBC, the total bytes of CSI data is 244 ((61 + 60) * 2 + 2 = 244. The result is aligned to four bytes, and the last two bytes are invalid).

    .. only:: esp32c5

        Channel state information (CSI) refers to the channel information of a Wi-Fi connection. In {IDF_TARGET_NAME}, this information consists of channel frequency responses of sub-carriers and is estimated when packets are received from the transmitter. Each channel frequency response of sub-carrier is recorded by two bytes of signed characters. The first one is imaginary part and the second one is real part. Except for the IEEE 802.11g mode, all other modes have two LTF sequences (LLTF + HT/VHT/HE-LTF). {IDF_TARGET_NAME} can determine whether to include LLTF or HT/VHT/HE-LTF through ``acquire_csi_force_lltf`` field of :cpp:struct:`wifi_csi_acquire_config_t`. For different types of packets which are received on channels with different state, the sub-carrier index and total bytes of signed characters of CSI are shown in the following table.

        +-------------+-------------------+--------------------------------------------+-----------------------------------+--------------------------------------------------------------------------+--------------------------------------------------------------------------+
        | channel     | secondary channel |                                           none                                 |                                  below                                   |                                  above                                   |
        +-------------+-------------------+--------------+-----------------------------+-----------------------------------+--------------+-----------------------------------------------------------+--------------+-----------------------------------------------------------+
        | packet      | signal mode       |    non HT    |              HT             |                 HE                |    non HT    |                           HT                              |    non HT    |                           HT                              |
        |             +-------------------+--------------+-----------------------------+-----------------------------------+--------------+-----------------------------+-----------------------------+--------------+-----------------------------+-----------------------------+
        | information | channel bandwidth |    20 MHz    |            20 MHz           |               20 MHz              |    20 MHz    |            20 MHz           |            40 MHz           |    20 MHz    |             20 MHz          |           40 MHz            |
        |             +-------------------+--------------+--------------+--------------+---------------+-------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
        |             | STBC              |   non STBC   |   non STBC   |     STBC     |    non STBC   |       STBC        |   non STBC   |   non STBC   |     STBC     |   none STBC  |     STBC     |   non STBC   |   non STBC   |     STBC     |   non STBC   |     STBC     |
        +-------------+-------------------+--------------+--------------+--------------+---------------+-------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
        | sub-carrier | LLTF              | 0~26, -26~-1 |      —       |      —       |       —       |         —         |     0~52     |      —       |      —       |      —       |      —       |    -53~-1    |       —      |      —       |      —       |      —       |
        |             +-------------------+--------------+--------------+--------------+---------------+-------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
        | index       | HT-LTF (HT-LTF1)  |      —       | 0~28, -28~-1 | 0~28, -28~-1 |       —       |         —         |      —       |     0~56     |     0~56     | 0~58, -58~-1 | 0~58, -58~-1 |       —      |    -57~-1    |    -57~-1    | 0~58, -58~-1 | 0~58, -58~-1 |
        |             +-------------------+--------------+--------------+--------------+---------------+-------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
        |             | HT-LTF2           |      —       |      —       | 0~28, -28~-1 |       —       |         —         |      —       |      —       |     0~56     |      —       | 0~58, -58~-1 |       —      |       —      |    -57~-1    |      —       | 0~58, -58~-1 |
        |             +-------------------+--------------+--------------+--------------+---------------+-------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
        |             | HE-LTF (HE-LTF1)  |      —       |      —       |      —       | 0~122, -122~1 | Determined by     |      —       |      —       |      —       |      —       |      —       |       —      |       —      |      —       |      —       |      —       |
        |             +-------------------+--------------+--------------+--------------+---------------+                   +--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
        |             | HE-LTF2           |      —       |      —       |      —       |       —       | wifi_csi_config_t |      —       |      —       |      —       |      —       |      —       |       —      |       —      |      —       |      —       |      —       |
        +-------------+-------------------+--------------+--------------+--------------+---------------+-------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
        | total bytes                     |     106      |     114      |     228      |      490      |        490        |     106      |     114      |     228      |     234      |     468      |      106     |      114     |     228      |     234      |     468      |
        +---------------------------------+--------------+--------------+--------------+---------------+-------------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+--------------+

        .. note::

            - In HT/VHT/HE modes, there are two LTF sequences: LLTF + HT/VHT/HE-LTF. If the ``acquire_csi_force_lltf`` field of :cpp:struct:`wifi_csi_acquire_config_t` is set to false, the CSI data will only contain HT/VHT/HE-LTF (as shown in the table above); otherwise, the CSI data will only contain LLTF. The sub-carrier index and total bytes for LLTF in HT/VHT/HE modes are the same as those for LLTF in non-HT modes.
            - In VHT mode, the sub-carrier index and total bytes are the same as those in HT mode.

        All of the information in the table can be found in the structure :cpp:type:`wifi_csi_info_t`.

            - Secondary channel refers to ``second`` field of ``rx_ctrl`` field.
            - signal mode of packet refers to  ``cur_bb_format`` field of ``rx_ctrl`` field.
            - total bytes refers to ``len`` field
            - The CSI data corresponding to each Long Training Field (LTF) type is stored in a buffer starting from the buf field. Each item is stored as two bytes: imaginary part followed by real part. The order of each item is the same as the sub-carrier in the table.
            - If ``first_word_invalid`` of :cpp:type:`wifi_csi_info_t` is true, it means that the first four bytes of CSI data is invalid due to a hardware limitation in {IDF_TARGET_NAME}.
            - If ``rx_channel_estimate_info_vld`` of ``rx_ctrl`` field is 1, indicates that the CSI data is valid; otherwise, the CSI data is invalid.
            - More information like RSSI, noise floor of RF, receiving time and antenna is in the ``rx_ctrl`` field.

        For STBC packets, the subcarrier indices of HE-LTF1 and HE-LTF2 are determined by ``acquire_csi_he_stbc_mode`` field of :cpp:type:`wifi_csi_config_t`. Please refer to the table below for details.

        +---------------------+----------------------+----------------------+
        | acquire_csi_he_stbc |         HE-LTF1      |         HE-LTF2      |
        +---------------------+----------------------+----------------------+
        |         0           |    -122~-1, 0~122    |           —          |
        +---------------------+----------------------+----------------------+
        |         1           |           —          |    -122~-1, 0~122    |
        +---------------------+----------------------+----------------------+
        |         2           | Sample evenly among the HE-LTF1 and HE-LTF2 |
        +---------------------+----------------------+----------------------+

        When imaginary part and real part data of sub-carrier are used, please refer to the table below.

        +-----------------------+-------------------+------------------------------------------+------------------------------+
        | PHY standard          | Sub-carrier range |           Invalid sub-carrier            |   Sub-carrier (total/data)   |
        +=======================+===================+==========================================+==============================+
        | 802.11a/g             |  -26 to +26       |                    0                     |     53 total, 52 usable      |
        +-----------------------+-------------------+------------------------------------------+------------------------------+
        | 802.11n, 20 MHz       |  -28 to +28       |                    0                     |     57 total, 56 usable      |
        +-----------------------+-------------------+------------------------------------------+------------------------------+
        | 802.11n, 40 MHz       |  -58 to +58       |                -1, 0, 1                  |     117 total, 114 usable    |
        +-----------------------+-------------------+------------------------------------------+------------------------------+
        | 802.11ac, 20 MHz      |  -28 to +28       |                    0                     |     57 total, 56 usable      |
        +-----------------------+-------------------+------------------------------------------+------------------------------+
        | 802.11ax, 20 MHz (SU) |  -122 to + 122    |                -1, 0, 1                  |     245 total, 242 usable    |
        +-----------------------+-------------------+------------------------------------------+------------------------------+

        .. note::

            - When the PHY is 802.11ax, please refer to the protocol for sub-carrier range and invalid sub-carrier for MU packets.

        .. note::

            - For STBC packet, CSI is provided for every space-time stream without CSD (cyclic shift delay). As each cyclic shift on the additional chains shall be -200 ns, only the CSD angle of first space-time stream is recorded in sub-carrier 0 of HT-LTF1 (HE-LTF1) and HT-LTF2 (HE-LTF2) for there is no channel frequency response in sub-carrier 0. CSD[10:0] is 11 bits, ranging from -pi to pi.


    Wi-Fi Channel State Information Configure
    -------------------------------------------

    To use Wi-Fi CSI, the following steps need to be done.

        - Select Wi-Fi CSI in menuconfig. Go to ``Menuconfig`` > ``Components config`` > ``Wi-Fi`` > ``Wi-Fi CSI (Channel State Information)``.
        - Set CSI receiving callback function by calling API :cpp:func:`esp_wifi_set_csi_rx_cb()`.
        - Configure CSI by calling API :cpp:func:`esp_wifi_set_csi_config()`.
        - Enable CSI by calling API :cpp:func:`esp_wifi_set_csi()`.

    The CSI receiving callback function runs from Wi-Fi task. So, do not do lengthy operations in the callback function. Instead, post necessary data to a queue and handle it from a lower priority task. Because station does not receive any packet when it is disconnected and only receives packets from AP when it is connected, it is suggested to enable sniffer mode to receive more CSI data by calling :cpp:func:`esp_wifi_set_promiscuous()`.


Wi-Fi Multiple Antennas
------------------------

Please refer to the :doc:`PHY <../../api-guides/phy>`.
