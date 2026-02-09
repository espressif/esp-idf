Overview
==========

:link_to_translation:`zh_CN:[中文]`

{IDF_TARGET_NAME} Wi-Fi Feature List
------------------------------------

The following features are supported:

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3

    - 4 virtual Wi-Fi interfaces, which are STA, AP, Sniffer and reserved
    - Station-only mode, AP-only mode, station/AP-coexistence mode
    - IEEE 802.11b, IEEE 802.11g, IEEE 802.11n, and APIs to configure the protocol mode
    - WPA/WPA2/WPA3/WPA2-Enterprise/WPA3-Enterprise/WAPI/WPS and DPP
    - AMSDU, AMPDU, HT40, QoS, and other key features
    - Modem-sleep
    - The Espressif-specific ESP-NOW protocol and Long Range mode, which supports up to **1 km** of data traffic
    - Up to 20 MBit/s TCP throughput and 30 MBit/s UDP throughput over the air
    - Sniffer
    - Both fast scan and all-channel scan
    - Multiple antennas
    - Channel state information


.. only:: esp32c2

    - 3 virtual Wi-Fi interfaces, which are STA, AP and Sniffer
    - Station-only mode, AP-only mode, station/AP-coexistence mode
    - IEEE 802.11b, IEEE 802.11g, IEEE 802.11n, and APIs to configure the protocol mode
    - WPA/WPA2/WPA3/WPA2-Enterprise/WPA3-Enterprise/WPS and DPP
    - AMPDU, QoS, and other key features
    - Modem-sleep
    - Up to 20 MBit/s TCP throughput and 30 MBit/s UDP throughput over the air
    - Sniffer
    - Both fast scan and all-channel scan
    - Multiple antennas


.. only:: esp32c6

    - 4 virtual Wi-Fi interfaces, which are STA, AP, Sniffer and reserved
    - Station-only mode, AP-only mode, station/AP-coexistence mode
    - IEEE 802.11b, IEEE 802.11g, IEEE 802.11n, IEEE 802.11ax, and APIs to configure the protocol mode
    - WPA/WPA2/WPA3/WPA2-Enterprise/WPA3-Enterprise/WAPI/WPS and DPP
    - AMSDU, AMPDU, HT40, QoS, and other key features
    - Modem-sleep
    - The Espressif-specific ESP-NOW protocol and Long Range mode, which supports up to **1 km** of data traffic
    - Up to 20 MBit/s TCP throughput and 30 MBit/s UDP throughput over the air
    - Sniffer
    - Both fast scan and all-channel scan
    - Multiple antennas
    - Channel state information
    - Individual TWT and Broadcast TWT
    - Downlink MU-MIMO
    - OFDMA
    - BSS Color


.. only:: esp32c5

    - 4 virtual Wi-Fi interfaces, which are STA, AP, Sniffer and reserved
    - Station-only mode, AP-only mode, station/AP-coexistence mode
    - IEEE 802.11b, IEEE 802.11g, IEEE 802.11n, IEEE 802.11a, IEEE 802.11ac, IEEE 802.11ax, and APIs to configure the protocol mode
    - WPA/WPA2/WPA3/WPA2-Enterprise/WPA3-Enterprise/WAPI/WPS and DPP
    - AMSDU, AMPDU, HT40, QoS, and other key features
    - Modem-sleep
    - The Espressif-specific ESP-NOW protocol and Long Range mode (only supported on 2.4 GHz band), which supports up to **1 km** of data traffic
    - Up to 20 MBit/s TCP throughput and 30 MBit/s UDP throughput over the air
    - Sniffer
    - Both fast scan and all-channel scan
    - Multiple antennas
    - Channel state information
    - Individual TWT and Broadcast TWT
    - Downlink MU-MIMO
    - OFDMA
    - BSS Color
.. only:: SOC_WIFI_NAN_SUPPORT

    - Wi-Fi Aware (NAN)


How To Write a Wi-Fi Application
----------------------------------

Preparation
+++++++++++

Generally, the most effective way to begin your own Wi-Fi application is to select an example which is similar to your own application, and port the useful part into your project. It is not a MUST, but it is strongly recommended that you take some time to read this article first, especially if you want to program a robust Wi-Fi application.

This article is supplementary to the Wi-Fi APIs/Examples. It describes the principles of using the Wi-Fi APIs, the limitations of the current Wi-Fi API implementation, and the most common pitfalls in using Wi-Fi. This article also reveals some design details of the Wi-Fi driver. We recommend you to select an :example:`example <wifi>`.

- :example:`wifi/getting_started/station` demonstrates how to use the station functionality to connect to an AP.

- :example:`wifi/getting_started/softAP` demonstrates how to use the SoftAP functionality to configure {IDF_TARGET_NAME} as an AP.

- :example:`wifi/scan` demonstrates how to scan for available APs, configure the scan settings, and display the scan results.

- :example:`wifi/fast_scan` demonstrates how to perform fast and all channel scans for nearby APs, set thresholds for signal strength and authentication modes, and connect to the best fitting AP based on signal strength and authentication mode.

- :example:`wifi/wps` demonstrates how to use the WPS enrollee feature to simplify the process of connecting to a Wi-Fi router, with options for PIN or PBC modes.

- :example:`wifi/wps_softap_registrar` demonstrates how to use the WPS registrar feature on SoftAP mode, simplifying the process of connecting to a Wi-Fi SoftAP from a station.

- :example:`wifi/smart_config` demonstrates how to use the smartconfig feature to connect to a target AP using the ESPTOUCH app.

- :example:`wifi/power_save` demonstrates how to use the power save mode in station mode.

- :example:`wifi/softap_sta` demonstrates how to configure {IDF_TARGET_NAME} to function as both an AP and a station simultaneously, effectively enabling it to act as a Wi-Fi NAT router.

- :example:`wifi/iperf` demonstrates how to implement the protocol used by the iPerf performance measurement tool, allowing for performance measurement between two chips or between a single chip and a computer running the iPerf tool, with specific instructions for testing station/soft-AP TCP/UDP RX/TX throughput.

- :example:`wifi/roaming/roaming_app` demonstrates how to use the Wi-Fi Roaming App functionality to efficiently roam between compatible APs.

- :example:`wifi/roaming/roaming_11kvr` demonstrates how to implement roaming using 11k and 11v APIs.

.. only:: SOC_WIFI_HE_SUPPORT

    - :example:`wifi/itwt` demonstrates how to use the iTWT feature, which only works in station mode and under different power save modes, with commands for setup, teardown, and suspend, and also shows the difference in current consumption when iTWT is enabled or disabled.

Setting Wi-Fi Compile-time Options
++++++++++++++++++++++++++++++++++++

Refer to :ref:`wifi-menuconfig`.

Init Wi-Fi
+++++++++++

Refer to :ref:`wifi-station-general-scenario` and :ref:`wifi-ap-general-scenario`.

Start/Connect Wi-Fi
++++++++++++++++++++

Refer to :ref:`wifi-station-general-scenario` and :ref:`wifi-ap-general-scenario`.

Event-Handling
++++++++++++++

Generally, it is easy to write code in "sunny-day" scenarios, such as :ref:`wifi-event-sta-start` and :ref:`wifi-event-sta-connected`. The hard part is to write routines in "rainy-day" scenarios, such as :ref:`wifi-event-sta-disconnected`. Good handling of "rainy-day" scenarios is fundamental to robust Wi-Fi applications. Refer to :ref:`wifi-event-description`, :ref:`wifi-station-general-scenario`, and :ref:`wifi-ap-general-scenario`. See also the :doc:`overview of the Event Loop Library in ESP-IDF <../../api-reference/system/esp_event>`.

Write Error-Recovery Routines Correctly at All Times
++++++++++++++++++++++++++++++++++++++++++++++++++++

Just like the handling of "rainy-day" scenarios, a good error-recovery routine is also fundamental to robust Wi-Fi applications. Refer to :ref:`wifi-api-error-code`.


.. _wifi-api-error-code:

{IDF_TARGET_NAME} Wi-Fi API Error Code
--------------------------------------

All of the {IDF_TARGET_NAME} Wi-Fi APIs have well-defined return values, namely, the error code. The error code can be categorized into:

 - No errors, e.g., :c:macro:`ESP_OK` means that the API returns successfully.
 - Recoverable errors, such as :c:macro:`ESP_ERR_NO_MEM`.
 - Non-recoverable, non-critical errors.
 - Non-recoverable, critical errors.

Whether the error is critical or not depends on the API and the application scenario, and it is defined by the API user.

**The primary principle to write a robust application with Wi-Fi API is to always check the error code and write the error-handling code.** Generally, the error-handling code can be used:

 - For recoverable errors, in which case you can write a recoverable-error code. For example, when :cpp:func:`esp_wifi_start()` returns :c:macro:`ESP_ERR_NO_MEM`, the recoverable-error code vTaskDelay can be called in order to get a microseconds' delay for another try.
 - For non-recoverable, yet non-critical errors, in which case printing the error code is a good method for error handling.
 - For non-recoverable and also critical errors, in which case "assert" may be a good method for error handling. For example, if :cpp:func:`esp_wifi_set_mode()` returns ``ESP_ERR_WIFI_NOT_INIT``, it means that the Wi-Fi driver is not initialized by :cpp:func:`esp_wifi_init()` successfully. You can detect this kind of error very quickly in the application development phase.

In :component_file:`esp_common/include/esp_err.h`, ``ESP_ERROR_CHECK`` checks the return values. It is a rather commonplace error-handling code and can be used as the default error-handling code in the application development phase. However, it is strongly recommended that API users write their own error-handling code.


{IDF_TARGET_NAME} Wi-Fi API Parameter Initialization
----------------------------------------------------

When initializing struct parameters for the API, one of two approaches should be followed:

- Explicitly set all fields of the parameter.
- Use get API to get current configuration first, then set application specific fields.

Initializing or getting the entire structure is very important, because most of the time the value 0 indicates that the default value is used. More fields may be added to the struct in the future and initializing these to zero ensures the application will still work correctly after ESP-IDF is updated to a new release.


.. _wifi-programming-model:

{IDF_TARGET_NAME} Wi-Fi Programming Model
-----------------------------------------

The {IDF_TARGET_NAME} Wi-Fi programming model is depicted as follows:

.. blockdiag::
    :caption: Wi-Fi Programming Model
    :align: center

    blockdiag wifi-programming-model {

        # global attributes
        node_height = 60;
        node_width = 100;
        span_width = 100;
        span_height = 60;
        default_shape = roundedbox;
        default_group_color = none;

        # node labels
        TCP_STACK [label="TCP\n stack", fontsize=12];
        EVNT_TASK [label="Event\n task", fontsize=12];
        APPL_TASK [label="Application\n task", width = 120, fontsize=12];
        WIFI_DRV  [label="Wi-Fi\n Driver", width = 120, fontsize=12];
        KNOT [shape=none];

        # node connections + labels
        TCP_STACK -> EVNT_TASK [label=event];
        EVNT_TASK -> APPL_TASK [label="callback\n or event"];

        # arrange nodes vertically
        group {
           label = "default handler";
           orientation = portrait;
           EVNT_TASK <- WIFI_DRV [label=event];
        }

        # intermediate node
        group {
            label = "user handler";
            orientation = portrait;
            APPL_TASK -- KNOT;
        }
        WIFI_DRV <- KNOT [label="API\n call"];
    }


The Wi-Fi driver can be considered a black box that knows nothing about high-layer code, such as the TCP/IP stack, application task, and event task. The application task (code) generally calls :doc:`Wi-Fi driver APIs <../../api-reference/network/esp_wifi>` to initialize Wi-Fi and handles Wi-Fi events when necessary. Wi-Fi driver receives API calls, handles them, and posts events to the application.

Wi-Fi event handling is based on the :doc:`esp_event library <../../api-reference/system/esp_event>`. Events are sent by the Wi-Fi driver to the :ref:`default event loop <esp-event-default-loops>`. Application may handle these events in callbacks registered using :cpp:func:`esp_event_handler_register()`. Wi-Fi events are also handled by :doc:`esp_netif component <../../api-reference/network/esp_netif>` to provide a set of default behaviors. For example, when Wi-Fi station connects to an AP, esp_netif will automatically start the DHCP client by default.


.. _wifi-event-description:

{IDF_TARGET_NAME} Wi-Fi Event Description
-----------------------------------------

WIFI_EVENT_WIFI_READY
++++++++++++++++++++++++++++++++++++

The Wi-Fi driver will never generate this event, which, as a result, can be ignored by the application event callback. This event may be removed in future releases.

.. _wifi-event-scan-done:

WIFI_EVENT_SCAN_DONE
++++++++++++++++++++++++++++++++++++

The scan-done event is triggered by :cpp:func:`esp_wifi_scan_start()` and will arise in the following scenarios:

  - The scan is completed, e.g., the target AP is found successfully, or all channels have been scanned.
  - The scan is stopped by :cpp:func:`esp_wifi_scan_stop()`.
  - The :cpp:func:`esp_wifi_scan_start()` is called before the scan is completed. A new scan will override the current scan and a scan-done event will be generated.

The scan-done event will not arise in the following scenarios:

  - It is a blocked scan.
  - The scan is caused by :cpp:func:`esp_wifi_connect()`.

Upon receiving this event, the event task does nothing. The application event callback needs to call :cpp:func:`esp_wifi_scan_get_ap_num()` and :cpp:func:`esp_wifi_scan_get_ap_records()` to fetch the scanned AP list and trigger the Wi-Fi driver to free the internal memory which is allocated during the scan **(do not forget to do this!)**.
Refer to :ref:`wifi-scan` for a more detailed description.

.. _wifi-event-sta-start:

WIFI_EVENT_STA_START
++++++++++++++++++++++++++++++++++++

If :cpp:func:`esp_wifi_start()` returns :c:macro:`ESP_OK` and the current Wi-Fi mode is station or station/AP, then this event will arise. Upon receiving this event, the event task will initialize the LwIP network interface (netif). Generally, the application event callback needs to call :cpp:func:`esp_wifi_connect()` to connect to the configured AP.

WIFI_EVENT_STA_STOP
++++++++++++++++++++++++++++++++++++

If :cpp:func:`esp_wifi_stop()` returns :c:macro:`ESP_OK` and the current Wi-Fi mode is station or station/AP, then this event will arise. Upon receiving this event, the event task will release the station's IP address, stop the DHCP client, remove TCP/UDP-related connections, and clear the LwIP station netif, etc. The application event callback generally does not need to do anything.

.. _wifi-event-sta-connected:

WIFI_EVENT_STA_CONNECTED
++++++++++++++++++++++++++++++++++++

If :cpp:func:`esp_wifi_connect()` returns :c:macro:`ESP_OK` and the station successfully connects to the target AP, the connection event will arise. Upon receiving this event, the event task starts the DHCP client and begins the DHCP process of getting the IP address. Then, the Wi-Fi driver is ready for sending and receiving data. This moment is good for beginning the application work, provided that the application does not depend on LwIP, namely the IP address. However, if the application is LwIP-based, then you need to wait until the *got ip* event comes in.

.. _wifi-event-sta-disconnected:

WIFI_EVENT_STA_DISCONNECTED
++++++++++++++++++++++++++++++++++++

This event can be generated in the following scenarios:

  - When :cpp:func:`esp_wifi_disconnect()` or :cpp:func:`esp_wifi_stop()` is called and the station is already connected to the AP.
  - When :cpp:func:`esp_wifi_connect()` is called, but the Wi-Fi driver fails to set up a connection with the AP due to certain reasons, e.g., the scan fails to find the target AP or the authentication times out. If there are more than one AP with the same SSID, the disconnected event will be raised after the station fails to connect all of the found APs.
  - When the Wi-Fi connection is disrupted because of specific reasons, e.g., the station continuously loses N beacons, the AP kicks off the station, or the AP's authentication mode is changed.

Upon receiving this event, the default behaviors of the event task are:

- Shutting down the station's LwIP netif.
- Notifying the LwIP task to clear the UDP/TCP connections which cause the wrong status to all sockets. For socket-based applications, the application callback can choose to close all sockets and re-create them, if necessary, upon receiving this event.

The most common event handle code for this event in application is to call :cpp:func:`esp_wifi_connect()` to reconnect the Wi-Fi. However, if the event is raised because :cpp:func:`esp_wifi_disconnect()` is called, the application should not call :cpp:func:`esp_wifi_connect()` to reconnect. It is the application's responsibility to distinguish whether the event is caused by :cpp:func:`esp_wifi_disconnect()` or other reasons. Sometimes a better reconnection strategy is required. Refer to :ref:`wifi-reconnect` and :ref:`scan-when-wifi-is-connecting`.

Another thing that deserves attention is that the default behavior of LwIP is to abort all TCP socket connections on receiving the disconnect. In most cases, it is not a problem. However, for some special applications, this may not be what they want. Consider the following scenarios:

- The application creates a TCP connection to maintain the application-level keep-alive data that is sent out every 60 seconds.
- Due to certain reasons, the Wi-Fi connection is cut off, and the `WIFI_EVENT_STA_DISCONNECTED`_ is raised. According to the current implementation, all TCP connections will be removed and the keep-alive socket will be in a wrong status. However, since the application designer believes that the network layer should **ignore** this error at the Wi-Fi layer, the application does not close the socket.
- Five seconds later, the Wi-Fi connection is restored because :cpp:func:`esp_wifi_connect()` is called in the application event callback function. **Moreover, the station connects to the same AP and gets the same IPV4 address as before**.
- Sixty seconds later, when the application sends out data with the keep-alive socket, the socket returns an error and the application closes the socket and re-creates it when necessary.

In above scenarios, ideally, the application sockets and the network layer should not be affected, since the Wi-Fi connection only fails temporarily and recovers very quickly.

.. _ip-event-sta-got-ip:

IP_EVENT_STA_GOT_IP
++++++++++++++++++++++++++++++++++++

This event arises when the DHCP client successfully gets the IPV4 address from the DHCP server, or when the IPV4 address is changed. The event means that everything is ready and the application can begin its tasks (e.g., creating sockets).

The IPV4 may be changed because of the following reasons:

  - The DHCP client fails to renew/rebind the IPV4 address, and the station's IPV4 is reset to 0.
  - The DHCP client rebinds to a different address.
  - The static-configured IPV4 address is changed.

Whether the IPV4 address is changed or not is indicated by the field ``ip_change`` of ``ip_event_got_ip_t``.

The socket is based on the IPV4 address, which means that, if the IPV4 changes, all sockets relating to this IPV4 will become abnormal. Upon receiving this event, the application needs to close all sockets and recreate the application when the IPV4 changes to a valid one.

IP_EVENT_GOT_IP6
++++++++++++++++++++++++++++++++++++

This event arises when the IPV6 SLAAC support auto-configures an address for the {IDF_TARGET_NAME}, or when this address changes. The event means that everything is ready and the application can begin its tasks, e.g., creating sockets.

IP_EVENT_STA_LOST_IP
++++++++++++++++++++++++++++++++++++

This event arises when the IPV4 address becomes invalid.

IP_EVENT_STA_LOST_IP does not arise immediately after the Wi-Fi disconnects. Instead, it starts an IPV4 address lost timer (configurable via :ref:`CONFIG_ESP_NETIF_LOST_IP_TIMER_ENABLE` and :ref:`CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL`). If the IPV4 address is got before the timer expires, IP_EVENT_STA_LOST_IP does not happen. Otherwise, the event arises when the IPV4 address lost timer expires.

Generally, the application can ignore this event, because it is just a debug event to inform that the IPV4 address is lost.

WIFI_EVENT_AP_START
++++++++++++++++++++++++++++++++++++

Similar to `WIFI_EVENT_STA_START`_.

WIFI_EVENT_AP_STOP
++++++++++++++++++++++++++++++++++++

Similar to `WIFI_EVENT_STA_STOP`_.

WIFI_EVENT_AP_STACONNECTED
++++++++++++++++++++++++++++++++++++

Every time a station is connected to {IDF_TARGET_NAME} AP, the `WIFI_EVENT_AP_STACONNECTED`_ will arise. Upon receiving this event, the event task will do nothing, and the application callback can also ignore it. However, you may want to do something, for example, to get the info of the connected STA.

WIFI_EVENT_AP_STADISCONNECTED
++++++++++++++++++++++++++++++++++++

This event can happen in the following scenarios:

  - The application calls :cpp:func:`esp_wifi_disconnect()`, or :cpp:func:`esp_wifi_deauth_sta()`, to manually disconnect the station.
  - The Wi-Fi driver kicks off the station, e.g., because the AP has not received any packets in the past five minutes. The time can be modified by :cpp:func:`esp_wifi_set_inactive_time()`.
  - The station kicks off the AP.

When this event happens, the event task will do nothing, but the application event callback needs to do something, e.g., close the socket which is related to this station.

WIFI_EVENT_AP_PROBEREQRECVED
++++++++++++++++++++++++++++++++++++

This event is disabled by default. The application can enable it via API :cpp:func:`esp_wifi_set_event_mask()`.
When this event is enabled, it will be raised each time the AP receives a probe request.

.. _wifi-event-sta-beacon-timeout:

WIFI_EVENT_STA_BEACON_TIMEOUT
++++++++++++++++++++++++++++++++++++

If the station does not receive the beacon of the connected AP within the inactive time, the beacon timeout happens, the :ref:`wifi-event-sta-beacon-timeout` will arise. The application can set inactive time via API :cpp:func:`esp_wifi_set_inactive_time()`.

.. _wifi-event-connectionless-module-wake-interval-start:

WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

The :ref:`wifi-event-connectionless-module-wake-interval-start` will arise at the start of connectionless module `Interval`. See :ref:`connectionless module power save <connectionless-module-power-save>`.



{IDF_TARGET_MAX_CONN_STA_NUM:default="15", esp32c2="4", esp32c3="10", esp32c6="10"}

{IDF_TARGET_SUB_MAX_NUM_FROM_KEYS:default="2", esp32c3="7", esp32c6="7"}

.. _wifi-configuration:

{IDF_TARGET_NAME} Wi-Fi Configuration
-------------------------------------

All configurations will be stored into flash when the Wi-Fi NVS is enabled; otherwise, refer to :ref:`wifi-nvs-flash`.

Wi-Fi Mode
+++++++++++++++++++++++++
Call :cpp:func:`esp_wifi_set_mode()` to set the Wi-Fi mode.

.. list-table::
   :header-rows: 1
   :widths: 15 50

   * - Mode
     - Description
   * - ``WIFI_MODE_NULL``
     - NULL mode: in this mode, the internal data struct is not allocated to the station and the AP, while both the station and AP interfaces are not initialized for RX/TX Wi-Fi data. Generally, this mode is used for Sniffer, or when you only want to stop both the station and the AP without calling :cpp:func:`esp_wifi_deinit()` to unload the whole Wi-Fi driver.
   * - ``WIFI_MODE_STA``
     - Station mode: in this mode, :cpp:func:`esp_wifi_start()` will init the internal station data, while the station’s interface is ready for the RX and TX Wi-Fi data. After :cpp:func:`esp_wifi_connect()`, the station will connect to the target AP.
   * - ``WIFI_MODE_AP``
     - AP mode: in this mode, :cpp:func:`esp_wifi_start()` will init the internal AP data, while the AP’s interface is ready for RX/TX Wi-Fi data. Then, the Wi-Fi driver starts broad-casting beacons, and the AP is ready to get connected to other stations.
   * - ``WIFI_MODE_APSTA``
     - Station/AP coexistence mode: in this mode, :cpp:func:`esp_wifi_start()` will simultaneously initialize both the station and the AP. This is done in station mode and AP mode. Please note that the channel of the external AP, which the ESP station is connected to, has higher priority over the ESP AP channel.


.. only:: esp32c5

    Wi-Fi Band Mode Configuration
    ++++++++++++++++++++++++++++++

    The Wi-Fi band mode used by {IDF_TARGET_NAME} can be set via the function :cpp:func:`esp_wifi_set_band_mode()`.

    .. list-table::
        :header-rows: 1
        :widths: 15 50

        * - Mode
          - Description
        * - ``WIFI_BAND_MODE_2G_ONLY``
          - **2.4 GHz band mode**: The device operates only on 2.4 GHz band channels.
        * - ``WIFI_BAND_MODE_5G_ONLY``
          - **5 GHz band mode**: The device operates only on 5 GHz band channels.
        * - ``WIFI_BAND_MODE_AUTO``
          - **2.4 GHz + 5 GHz auto mode**: The device automatically selects either the 2.4 GHz or 5 GHz band based on the connected AP or SoftAP configuration.

    .. note::
        - ``WIFI_BAND_MODE_AUTO`` does not mean simultaneous dual-band support; it only allows automatic band selection.

        When operating in ``WIFI_BAND_MODE_AUTO`` mode, protocols and bandwidth can be configured separately for the 2.4 GHz and 5 GHz bands:

        - Use the function :cpp:func:`esp_wifi_set_protocols()` to set the supported protocol types for each band (e.g., 802.11b/g/n/ac/ax);

        - Use the function :cpp:func:`esp_wifi_set_bandwidths()` to set the bandwidth for each band (e.g., 20 MHz, 40 MHz).

.. only:: esp32c5

    AP Choose
    +++++++++++++++++++++++++

    When the device scans multiple APs with the same SSID, {IDF_TARGET_NAME} selects the most suitable AP to connect to based on signal strength (RSSI) and band information. The default policy usually prefers the AP with higher RSSI; however, in environments where 2.4 GHz and 5 GHz coexist, this can cause the device to favor the 2.4 GHz band, ignoring the performance benefits of the 5 GHz band.

    To address this, ESP-IDF provides the field :cpp:member:`rssi_5g_adjustment` in the :cpp:type:`wifi_scan_threshold_t` structure to optimize the priority of selecting 5 GHz APs.

    .. list-table::
      :header-rows: 1
      :widths: 30 70

      * - Field
        - Description
      * - ``rssi_5g_adjustment``
        - Used to adjust priority between 2.4 GHz and 5 GHz APs with the same SSID. The default value is ``10``, meaning when the 5 GHz AP's RSSI is within 10 dB lower than the 2.4 GHz AP, the 5 GHz AP will be preferred. Properly setting this parameter helps the device prioritize 5 GHz networks that offer better bandwidth and interference resistance when signal strengths are close.

    Example:

    Suppose the device scans the following two APs with the SSID "MyWiFi":

    - 2.4 GHz AP: RSSI = -60 dBm
    - 5 GHz AP: RSSI = -68 dBm

    Since ``rssi_5g_adjustment = 10`` (default) and ``-68 > -60 - 10`` holds true, the device will prioritize connecting to the 5 GHz AP.

    .. note::

        This parameter only takes effect when scanning results contain APs with the same SSID on both 2.4 GHz and 5 GHz bands. Its purpose is to avoid always connecting to a 2.4 GHz network with slightly stronger signal but poorer performance.


.. _station-basic-configuration:

Station Basic Configuration
+++++++++++++++++++++++++++++++++++++++++++++++

API :cpp:func:`esp_wifi_set_config()` can be used to configure the station. And the configuration will be stored in NVS. The table below describes the fields in detail.

.. list-table::
   :header-rows: 1
   :widths: 15 50

   * - Field
     - Description
   * - ssid
     - This is the SSID of the target AP, to which the station wants to connect.
   * - password
     - Password of the target AP.
   * - scan_method
     - For ``WIFI_FAST_SCAN`` scan, the scan ends when the first matched AP is found. For ``WIFI_ALL_CHANNEL_SCAN``, the scan finds all matched APs on all channels. The default scan is ``WIFI_FAST_SCAN``.
   * - bssid_set
     - If bssid_set is 0, the station connects to the AP whose SSID is the same as the field “ssid”, while the field “bssid” is ignored. In all other cases, the station connects to the AP whose SSID is the same as the “ssid” field, while its BSSID is the same the “bssid” field .
   * - bssid
     - This is valid only when bssid_set is 1; see field “bssid_set”.
   * - channel
     - If the channel is 0, the station scans the channel 1 ~ N to search for the target AP; otherwise, the station starts by scanning the channel whose value is the same as that of the “channel” field, and then scans the channel 1 ~ N but skip the specific channel to find the target AP. For example, if the channel is 3, the scan order will be 3, 1, 2, 4,..., N. If you do not know which channel the target AP is running on, set it to 0.
   * - sort_method
     - This field is only for ``WIFI_ALL_CHANNEL_SCAN``.

       If the sort_method is ``WIFI_CONNECT_AP_BY_SIGNAL``, all matched APs are sorted by signal, and the AP with the best signal will be connected firstly. For example, the station wants to connect an AP whose SSID is “apxx”. If the scan finds two APs whose SSID equals to “apxx”, and the first AP’s signal is -90 dBm while the second AP’s signal is -30 dBm, the station connects the second AP firstly, and it would not connect the first one unless it fails to connect the second one.

       If the sort_method is ``WIFI_CONNECT_AP_BY_SECURITY``, all matched APs are sorted by security. For example, the station wants to connect an AP whose SSID is “apxx”. If the scan finds two APs whose SSID is “apxx”, and the security of the first found AP is open while the second one is WPA2, the station connects to the second AP firstly, and it would not connect the first one unless it fails to connect the second one.
   * - threshold
     - The threshold is used to filter the found AP. If the RSSI or security mode is less than the configured threshold, the AP will be discarded.

       If the RSSI is set to 0, it means the default threshold is used, and the default RSSI threshold is -127 dBm. If the authmode threshold is set to 0, it means the default threshold is used, and the default authmode threshold is open.


.. attention::

    WEP/WPA security modes are deprecated in IEEE 802.11-2016 specifications and are recommended not to be used. These modes can be rejected using authmode threshold by setting threshold as WPA2 by threshold.authmode as ``WIFI_AUTH_WPA2_PSK``.

AP Basic Configuration
+++++++++++++++++++++++++++++++++++++

API :cpp:func:`esp_wifi_set_config()` can be used to configure the AP. And the configuration will be stored in NVS. The table below describes the fields in detail.

.. only:: esp32 or esp32s2 or esp32s3 or esp32c3 or esp32c5 or esp32c6

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - Field
        - Description
      * - ssid
        - SSID of AP; if the ssid[0] is 0xFF and ssid[1] is 0xFF, the AP defaults the SSID to ``ESP_aabbcc``, where “aabbcc” is the last three bytes of the AP MAC.
      * - password
        - Password of AP; if the auth mode is ``WIFI_AUTH_OPEN``, this field will be ignored.
      * - ssid_len
        - Length of SSID; if ssid_len is 0, check the SSID until there is a termination character. If ssid_len > 32, change it to 32; otherwise, set the SSID length according to ssid_len.
      * - channel
        - Channel of AP; if the channel is out of range, the Wi-Fi driver will return error. So, please make sure the channel is within the required range. For more details, refer to `Wi-Fi Country Code`_.
      * - authmode
        - Auth mode of ESP AP; currently, ESP AP does not support AUTH_WEP. If the authmode is an invalid value, AP defaults the value to ``WIFI_AUTH_OPEN``.
      * - ssid_hidden
        - If ssid_hidden is 1, AP does not broadcast the SSID; otherwise, it does broadcast the SSID.
      * - max_connection
        - The max number of stations allowed to connect in, the default value is 10. ESP Wi-Fi supports up to {IDF_TARGET_MAX_CONN_STA_NUM} (``ESP_WIFI_MAX_CONN_NUM``) Wi-Fi connections. Please note that ESP AP and ESP-NOW share the same encryption hardware keys, so the max_connection parameter will be affected by the :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM`. The total number of encryption hardware keys is 17, if :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM` <= {IDF_TARGET_SUB_MAX_NUM_FROM_KEYS}, the max_connection can be set up to {IDF_TARGET_MAX_CONN_STA_NUM}, otherwise the max_connection can be set up to (17 - :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM`).
      * - beacon_interval
        - Beacon interval; the value is 100 ~ 60000 ms, with default value being 100 ms. If the value is out of range, AP defaults it to 100 ms.


.. only:: esp32c2

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - Field
        - Description
      * - ssid
        - SSID of AP; if the ssid[0] is 0xFF and ssid[1] is 0xFF, the AP defaults the SSID to ``ESP_aabbcc``, where “aabbcc” is the last three bytes of the AP MAC.
      * - password
        - Password of AP; if the auth mode is ``WIFI_AUTH_OPEN``, this field will be ignored.
      * - ssid_len
        - Length of SSID; if ssid_len is 0, check the SSID until there is a termination character. If ssid_len > 32, change it to 32; otherwise, set the SSID length according to ssid_len.
      * - channel
        - Channel of AP; if the channel is out of range, the Wi-Fi driver defaults to channel 1. So, please make sure the channel is within the required range. For more details, refer to `Wi-Fi Country Code`_.
      * - authmode
        - Auth mode of ESP AP; currently, ESP AP does not support AUTH_WEP. If the authmode is an invalid value, AP defaults the value to ``WIFI_AUTH_OPEN``.
      * - ssid_hidden
        - If ssid_hidden is 1, AP does not broadcast the SSID; otherwise, it does broadcast the SSID.
      * - max_connection
        - The max number of stations allowed to connect in, the default value is 2. ESP Wi-Fi supports up to {IDF_TARGET_MAX_CONN_STA_NUM} (``ESP_WIFI_MAX_CONN_NUM``) Wi-Fi connections. Please note that ESP AP and ESP-NOW share the same encryption hardware keys, so the max_connection parameter will be affected by the :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM`. The total number of encryption hardware keys is {IDF_TARGET_MAX_CONN_STA_NUM}, the max_connection can be set up to ({IDF_TARGET_MAX_CONN_STA_NUM} - :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM`).
      * - beacon_interval
        - Beacon interval; the value is 100 ~ 60000 ms, with default value being 100 ms. If the value is out of range, AP defaults it to 100 ms.


Wi-Fi Protocol Mode
+++++++++++++++++++++++++

Currently, the ESP-IDF supports the following protocol modes:

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - Protocol Mode
        - Description
      * - 802.11b
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B)`` to set the station/AP to 802.11b-only mode.
      * - 802.11bg
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)`` to set the station/AP to 802.11bg mode.
      * - 802.11g
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)`` and esp_wifi_config_11b_rate(ifx, true) to set the station/AP to 802.11g mode.
      * - 802.11bgn
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B| WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` to set the station/ AP to BGN mode.
      * - 802.11gn
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` and esp_wifi_config_11b_rate(ifx, true) to set the station/AP to 802.11gn mode.
      * - 802.11 BGNLR
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B| WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR)`` to set the station/AP to BGN and the LR mode.
      * - 802.11 LR
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_LR)`` to set the station/AP only to the LR mode.

          **This mode is an Espressif-patented mode which can achieve a one-kilometer line of sight range. Please make sure both the station and the AP are connected to an ESP device.**

.. only:: esp32c6

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - Protocol Mode
        - Description
      * - 802.11b
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B)`` to set the station/AP to 802.11b-only mode.
      * - 802.11bg
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)`` to set the station/AP to 802.11bg mode.
      * - 802.11g
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)`` and ``esp_wifi_config_11b_rate(ifx, true)`` to set the station/AP to 802.11g mode.
      * - 802.11bgn
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B| WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` to set the station/ AP to BGN mode.
      * - 802.11gn
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` and esp_wifi_config_11b_rate(ifx, true) to set the station/AP to 802.11gn mode.
      * - 802.11 BGNLR
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B| WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR)`` to set the station/AP to BGN and the LR mode.
      * - 802.11bgnax
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B| WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_11AX)`` to set the station/ AP to 802.11bgnax mode.
      * - 802.11 BGNAXLR
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B| WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_11AX|WIFI_PROTOCOL_LR)`` to set the station/ AP to 802.11bgnax and LR mode.
      * - 802.11 LR
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_LR)`` to set the station/AP only to the LR mode.

          **This mode is an Espressif-patented mode which can achieve a one-kilometer line of sight range. Please make sure both the station and the AP are connected to an ESP device.**


.. only:: esp32c5

    - **2.4 GHz band**: Supports 802.11b, 802.11bg, 802.11bgn, 802.11bgnax, and Espressif's proprietary LR mode.
    - **5 GHz band**: Supports 802.11a, 802.11an, 802.11anac, and 802.11anacax.

    {IDF_TARGET_NAME} supports configuring Wi-Fi protocol modes for the 2.4 GHz and 5 GHz bands separately. It is recommended to use :cpp:func:`esp_wifi_set_protocols()` for this purpose. The legacy API :cpp:func:`esp_wifi_set_protocol()` is also supported.

    **Recommended Usage**

    Use the new API :cpp:func:`esp_wifi_set_protocols()` to configure each band independently:

    .. code-block:: c

        // Set 2.4 GHz to use 802.11bgnax, and 5 GHz to use 802.11anacax
        wifi_protocols_t protocols = {
            .ghz_2g = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX,
            .ghz_5g = WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AC | WIFI_PROTOCOL_11AX,
        };
        esp_wifi_set_protocols(WIFI_IF_STA, &protocols);

    **Legacy Usage**

    Use the legacy API :cpp:func:`esp_wifi_set_protocol()` to configure the protocol mode for 2.4 GHz band or 5 GHz band:

    .. code-block:: c

        // Set band mode to 2.4 GHz band
        esp_wifi_set_band_mode(WIFI_BAND_MODE_2G_ONLY);

        // Set protocol of station to 802.11bgnax
        uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX;
        esp_wifi_set_protocol(WIFI_IF_STA, protocol);

    .. note::

        - The new API `esp_wifi_set_protocols()` allows configuring both bands simultaneously and is recommended for use on {IDF_TARGET_NAME}.
        - The function ``esp_wifi_set_protocol()`` is suitable for single-band scenarios, such as when ``WIFI_BAND_MODE_2G_ONLY`` or ``WIFI_BAND_MODE_5G_ONLY`` is used. It only takes effect on the currently connected band. For example, if the interface is operating on the 5 GHz band, any configuration for the 2.4 GHz band will be ignored.
        - If the configuration includes unsupported protocol combinations, the function will return an error.
        - To enable Espressif's proprietary LR mode, make sure to include `WIFI_PROTOCOL_LR` in the 2.4 GHz protocol configuration.


.. only:: esp32c2

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - Protocol Mode
        - Description
      * - 802.11b
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B)`` to set the station/AP to 802.11b-only mode.
      * - 802.11bg
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)`` to set the station/AP to 802.11bg mode.
      * - 802.11g
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G) and esp_wifi_config_11b_rate(ifx, true)`` to set the station/AP to 802.11g mode.
      * - 802.11bgn
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B| WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` to set the station/ AP to BGN mode.
      * - 802.11gn
        - Call ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` and ``esp_wifi_config_11b_rate(ifx, true)`` to set the station/AP to 802.11gn mode.


Wi-Fi Bandwidth Mode
++++++++++++++++++++++

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3

    {IDF_TARGET_NAME} currently supports 20 MHz and 40 MHz bandwidth modes, which are used in combination with protocol modes. Common combinations include:

    - **HT20**: 802.11n with 20 MHz bandwidth
    - **HT40**: 802.11n with 40 MHz bandwidth

    .. note::

        - The 40 MHz bandwidth mode is only supported in 802.11n mode.

    Applications can use the :cpp:func:`esp_wifi_set_bandwidth()` API to set the bandwidth mode of the current interface.

    Example:

    .. code-block:: c

        // Set STA interface protocol to 802.11bgn
        uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N;
        esp_wifi_set_protocol(WIFI_IF_STA, protocol);

        // Set STA interface bandwidth to 40 MHz
        esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW40);

.. only:: esp32c6

    {IDF_TARGET_NAME} currently supports 20 MHz and 40 MHz bandwidth modes, which are used in combination with protocol modes. Common combinations include:

    - **HT20**: 802.11n with 20 MHz bandwidth
    - **HT40**: 802.11n with 40 MHz bandwidth
    - **HE20**: 802.11ax with 20 MHz bandwidth

    .. note::

        - The 40 MHz bandwidth mode is only supported in 802.11n mode.

    Applications can use the :cpp:func:`esp_wifi_set_bandwidth()` API to set the bandwidth mode of the current interface.

    Example:

    .. code-block:: c

        // Set protocol of station to 802.11bgn
        uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N;
        esp_wifi_set_protocol(WIFI_IF_STA, protocol);

        // Set bandwidth of station to 40 MHz
        esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW40);

.. only:: esp32c5

    {IDF_TARGET_NAME} currently supports two bandwidth modes, 20 MHz and 40 MHz, which are used in combination with protocol modes. Common combinations include:

    - **HT20**: 802.11n/11an, 20 MHz bandwidth
    - **HT40**: 802.11n/11an, 40 MHz bandwidth
    - **HE20**: 802.11ax, 20 MHz bandwidth

    .. note::

        - The 40 MHz bandwidth mode is only supported under 802.11n (2.4 GHz) or 802.11an (5 GHz) modes.

    Applications can use the :cpp:func:`esp_wifi_set_bandwidths()` API to set independent bandwidths for 2.4 GHz and 5 GHz bands.

    Example:

    .. code-block:: c

        // Set 2.4 GHz to use 802.11bgnax, and 5 GHz to use 802.11an
        wifi_protocols_t protocols = {
            .ghz_2g = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX,
            .ghz_5g = WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11N,
        };
        esp_wifi_set_protocols(WIFI_IF_STA, &protocols);

        // Set bandwidth to 20 MHz for 2.4 GHz band and 40 MHz for 5 GHz band
        wifi_bandwidths_t bw = {
            .ghz_2g = WIFI_BW20,
            .ghz_5g = WIFI_BW40
        };
        esp_wifi_set_bandwidths(WIFI_IF_STA, &bw);

    .. note::

        - When `.ghz_2g` is set to 0, only the 5 GHz bandwidth is updated, and the 2.4 GHz bandwidth remains unchanged.
        - When `.ghz_5g` is set to 0, only the 2.4 GHz bandwidth is updated, and the 5 GHz bandwidth remains unchanged.

    **Legacy Usage**

    Use the legacy API :cpp:func:`esp_wifi_set_bandwidth()` to configure the bandwidth of the 2.4 GHz or 5 GHz band:

    .. code-block:: c

          // Set band mode to 5 GHz band
          esp_wifi_set_band_mode(WIFI_BAND_MODE_5G_ONLY);

          // Set protocol of the station interface to 802.11an
          uint8_t protocol = WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11N;
          esp_wifi_set_protocol(WIFI_IF_STA, protocol);

          // Set bandwidth of the station interface to 40 MHz
          esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW40);

    .. note::

        - The new API :cpp:func:`esp_wifi_set_bandwidths()` can configure both 2.4 GHz and 5 GHz bandwidths simultaneously, and is recommended for use on {IDF_TARGET_NAME}.
        - ``esp_wifi_set_bandwidth()`` is suitable for single-band scenarios, such as ``WIFI_BAND_MODE_2G_ONLY`` or ``WIFI_BAND_MODE_5G_ONLY`` modes. It only affects the currently connected band. For example, if the interface is on the 5 GHz band, any 2.4 GHz bandwidth settings will be ignored.
        - If the configured bandwidth is not supported on the current band, the function will return an error.

.. only:: esp32c2

    {IDF_TARGET_NAME} currently supports only 20 MHz bandwidth mode.

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3 or esp32c5 or esp32c6

    Long Range (LR)
    +++++++++++++++++++++++++

    Long Range (LR) mode is an Espressif-patented Wi-Fi mode which can achieve a one-kilometer line of sight range. Compared to the traditional 802.11b mode, it has better reception sensitivity, stronger anti-interference ability, and longer transmission distance.

    LR Compatibility
    *************************

    Since LR is an Espressif-unique Wi-Fi mode operating on the 2.4 GHz band, only ESP32-series chips (excluding the ESP32-C2) support LR data transmission and reception. To ensure compatibility, the ESP32 devices should NOT use LR data rates when connected to non-LR-capable devices. This can be enforced by configuring the appropriate Wi-Fi mode:

    - If the negotiated mode supports LR, ESP32 devices may transmit data at LR rates.
    - Otherwise, they must default to traditional Wi-Fi data rates.

    The following table depicts the Wi-Fi mode negotiation on 2.4 GHz band:

    .. only:: esp32 or esp32s2 or esp32c3 or esp32s3

        +-------+-----+----+---+-------+------+-----+----+
        |AP\STA | BGN | BG | B | BGNLR | BGLR | BLR | LR |
        +=======+=====+====+===+=======+======+=====+====+
        | BGN   | BGN | BG | B | BGN   | BG   | B   | -  |
        +-------+-----+----+---+-------+------+-----+----+
        | BG    | BG  | BG | B | BG    | BG   | B   | -  |
        +-------+-----+----+---+-------+------+-----+----+
        | B     | B   | B  | B | B     | B    | B   | -  |
        +-------+-----+----+---+-------+------+-----+----+
        | BGNLR | -   | -  | - | BGNLR | BGLR | BLR | LR |
        +-------+-----+----+---+-------+------+-----+----+
        | BGLR  | -   | -  | - | BGLR  | BGLR | BLR | LR |
        +-------+-----+----+---+-------+------+-----+----+
        | BLR   | -   | -  | - | BLR   | BLR  | BLR | LR |
        +-------+-----+----+---+-------+------+-----+----+
        | LR    | -   | -  | - | LR    | LR   | LR  | LR |
        +-------+-----+----+---+-------+------+-----+----+

    .. only:: esp32c5 or esp32c6

        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | AP\STA  | BGNAX | BGN | BG | B | BGNAXLR | BGNLR | BGLR | BLR | LR |
        +=========+=======+=====+====+===+=========+=======+======+=====+====+
        | BGNAX   | BGAX  | BGN | BG | B | BGAX    | BGN   | BG   | B   | -  |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BGN     | BGN   | BGN | BG | B | BGN     | BGN   | BG   | B   | -  |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BG      | BG    | BG  | BG | B | BG      | BG    | BG   | B   | -  |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | B       | B     | B   | B  | B | B       | B     | B    | B   | -  |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BGNAXLR | -     | -   | -  | - | BGAXLR  | BGNLR | BGLR | BLR | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BGNLR   | -     | -   | -  | - | BGNLR   | BGNLR | BGLR | BLR | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BGLR    | -     | -   | -  | - | BGLR    | BGLR  | BGLR | BLR | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BLR     | -     | -   | -  | - | BLR     | BLR   | BLR  | BLR | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | LR      | -     | -   | -  | - | LR      | LR    | LR   | LR  | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+

    In the above table, the row is the Wi-Fi mode of AP and the column is the Wi-Fi mode of station. The "-" indicates Wi-Fi mode of the AP and station are not compatible.

    According to the table, the following conclusions can be drawn:

    - For LR-enabled AP of {IDF_TARGET_NAME}, it is incompatible with traditional 802.11 mode, because the beacon is sent in LR mode.
    - For LR-enabled station of {IDF_TARGET_NAME} whose mode is NOT LR-only mode, it is compatible with traditional 802.11 mode.
    - If both station and AP are ESP32 series chips devices (except ESP32-C2) and both of them have enabled LR mode, the negotiated mode supports LR.

    If the negotiated Wi-Fi mode supports both traditional 802.11 mode and LR mode, it is the Wi-Fi driver's responsibility to automatically select the best data rate in different Wi-Fi modes and the application can ignore it.

    LR Impacts to Traditional Wi-Fi Device
    ***************************************

    The data transmission in LR rate has no impacts on the traditional Wi-Fi device because:

    - The CCA and backoff process in LR mode are consistent with 802.11 specification.
    - The traditional Wi-Fi device can detect the LR signal via CCA and do backoff.

    In other words, the transmission impact in LR mode is similar to that in 802.11b mode.

    LR Transmission Distance
    *************************

    The reception sensitivity gain of LR is about 4 dB larger than that of the traditional 802.11b mode. Theoretically, the transmission distance is about 2 to 2.5 times the distance of 11B.

    LR Throughput
    *************************

    The LR rate has very limited throughput, because the raw PHY data rate LR is 1/2 Mbps and 1/4 Mbps.

    When to Use LR
    *************************

    The general conditions for using LR are:

    - Both the AP and station are Espressif devices.
    - Long distance Wi-Fi connection and data transmission is required.
    - Data throughput requirements are very small, such as remote device control.


.. only:: esp32c5

    Dynamic Frequency Selection (DFS)
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++

    In the 5 GHz Wi-Fi band, certain channels (e.g., channels 52–144) share spectrum with critical systems such as weather radars. To avoid interference with these systems, Wi-Fi devices must perform specific detection and channel switching mechanisms before operating on these channels—this is known as **Dynamic Frequency Selection (DFS)**.

    Enabling DFS allows devices to access more 5 GHz channels, thereby increasing network capacity and reducing interference. This is especially beneficial in high-density deployments or applications that require high bandwidth. The range of DFS channels may vary by country or region; see :component_file:`esp_wifi/regulatory/esp_wifi_regulatory.txt` for details.

    {IDF_TARGET_NAME} supports DFS channels in the 5 GHz band, but only supports **passive radar detection**.

    .. list-table::
      :header-rows: 1
      :widths: 20 60

      * - Type
        - Description
      * - Passive Radar Detection Supported
        - During scanning, the device can listen to DFS channels, detect access points (APs) operating on DFS channels, and connect to them. If the AP detects radar signals and switches to another channel using Channel Switch Announcement (CSA), {IDF_TARGET_NAME} will follow the AP to the new channel.
      * - Active Radar Detection Not Supported
        - {IDF_TARGET_NAME} **does not support** active radar detection and therefore cannot operate as a DFS AP in SoftAP mode.

    .. note::

        - In STA mode, {IDF_TARGET_NAME} can connect to APs operating on DFS channels, provided those channels are detected during scanning.
        - In SoftAP mode, {IDF_TARGET_NAME} is not allowed to operate on DFS channels to comply with regulatory requirements.
        - In STA+SoftAP coexistence mode:

          1. If the STA connects to an AP on a DFS channel, the SoftAP is allowed to switch to the same DFS channel using CSA (Channel Switch Announcement).
          2. When the STA disconnects, the SoftAP will switch back to a non-DFS channel via CSA to remain compliant with regulations.

..

.. _wifi-country-code:

Wi-Fi Country Code
++++++++++++++++++++++++++++

.. only:: esp32 or esp32c2 or esp32s2 or esp32c3 or esp32s3 or esp32c6

    Call :cpp:func:`esp_wifi_set_country()` to set the country info. The table below describes the fields in detail. Please consult local 2.4 GHz RF operating regulations before configuring these fields.

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - Field
        - Description
      * - cc[3]
        - Country code string. This attribute identifies the country or noncountry entity in which the station/AP is operating. If it is a country, the first two octets of this string is the two-character country info as described in the document ISO/IEC3166-1. The third octet is one of the following:

          - an ASCII space character, which means the regulations under which the station/AP is operating encompass all environments for the current frequency band in the country.
          - an ASCII ‘O’ character, which means the regulations under which the station/AP is operating are for an outdoor environment only.
          - an ASCII ‘I’ character, which means the regulations under which the station/AP is operating are for an indoor environment only.
          - an ASCII ‘X’ character, which means the station/AP is operating under a noncountry entity. The first two octets of the noncountry entity is two ASCII ‘XX’ characters.
          - the binary representation of the Operating Class table number currently in use. Refer to Annex E of IEEE Std 802.11-2020.

      * - schan
        - Start channel. It is the minimum channel number of the regulations under which the station/AP can operate.
      * - nchan
        - Total number of channels as per the regulations. For example, if the schan=1, nchan=13, then the station/AP can send data from channel 1 to 13.
      * - policy
        - Country policy. This field controls which country info will be used if the configured country info is in conflict with the connected AP’s. For more details on related policies, see the following section.


    The default country info is::

        wifi_country_t config = {
            .cc = "01",
            .schan = 1,
            .nchan = 11,
          .policy = WIFI_COUNTRY_POLICY_AUTO,
        };

    If the Wi-Fi Mode is station/AP coexist mode, they share the same configured country info. Sometimes, the country info of AP, to which the station is connected, is different from the country info of configured. For example, the configured station has country info::

        wifi_country_t config = {
            .cc = "JP",
            .schan = 1,
            .nchan = 14,
            .policy = WIFI_COUNTRY_POLICY_AUTO,
        };

    but the connected AP has country info::

        wifi_country_t config = {
            .cc = "CN",
            .schan = 1,
            .nchan = 13,
        ;

    then country info of connected AP's is used.

    The following table depicts which country info is used in different Wi-Fi modes and different country policies, and it also describes the impact on active scan.

    .. list-table::
      :header-rows: 1
      :widths: 15 15 35

      * - Wi-Fi Mode
        - Policy
        - Description
      * - Station
        - WIFI_COUNTRY_POLICY_AUTO
        - If the connected AP has country IE in its beacon, the country info equals to the country info in beacon. Otherwise, use the default country info.

          For scan:

            Use active scan from 1 to 11 and use passive scan from 12 to 14.

          Always keep in mind that if an AP with hidden SSID and station is set to a passive scan channel, the passive scan will not find it. In other words, if the application hopes to find the AP with hidden SSID in every channel, the policy of country info should be configured to WIFI_COUNTRY_POLICY_MANUAL.

      * - Station
        - WIFI_COUNTRY_POLICY_MANUAL
        - Always use the configured country info.

          For scan:

            Use active scan from schan to schan+nchan-1.

      * - AP
        - WIFI_COUNTRY_POLICY_AUTO
        - Always use the configured country info.

      * - AP
        - WIFI_COUNTRY_POLICY_MANUAL
        - Always use the configured country info.

      * - Station/AP-coexistence
        - WIFI_COUNTRY_POLICY_AUTO
        - Station: Same as station mode with policy WIFI_COUNTRY_POLICY_AUTO.
          AP: If the station does not connect to any external AP, the AP uses the configured country info. If the station connects to an external AP, the AP has the same country info as the station.

      * - Station/AP-coexistence
        - WIFI_COUNTRY_POLICY_MANUAL
        - Station: Same as station mode with policy WIFI_COUNTRY_POLICY_MANUAL.
          AP: Same as AP mode with policy WIFI_COUNTRY_POLICY_MANUAL.

.. only:: esp32c5

    The :cpp:func:`esp_wifi_set_country()` function is used to set the country/region information. The table below details the meaning of each field. Before configuring these fields, please refer to local 2.4 GHz and 5 GHz RF regulations.

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - Field
        - Description
      * - cc[3]
        - Country/region code string identifying the country or region of the station/AP, or a non-country entity. If it is a country/region, the first two bytes comply with the ISO/IEC 3166-1 two-letter country code standard. The third byte has the following meanings:

          - Space character (ASCII): The country/region allows usage of the respective frequency band in all environments.
          - Character `'O'`: Usage allowed only in outdoor environments.
          - Character `'I'`: Usage allowed only in indoor environments.
          - Character `'X'`: Non-country entity, in which case the first two characters should be `'XX'`.
          - Binary operational class number, referring to IEEE Std 802.11-2020 Appendix E.

      * - schan
        - Start channel number, indicating the minimum channel allowed in the 2.4 GHz band for the configured country/region.
      * - nchan
        - Number of channels. Defines the total number of allowed channels in the 2.4 GHz band. For example, if `schan = 1` and `nchan = 13`, then channels 1 through 13 are allowed.
      * - policy
        - Country/region policy. When the configured country/region conflicts with that of the connected AP, this field determines which information to use. Details are explained below.
      * - wifi_5g_channel_mask
        - Bitmask indicating allowed 5 GHz channels for the station/AP. The mapping between channel numbers and bits can be found in :cpp:enum:`wifi_5g_channel_bit_t`.

    A default configuration example::

        wifi_country_t config = {
            .cc = "01",
            .schan = 1,
            .nchan = 11,
            .wifi_5g_channel_mask = 0xfe,
            .policy = WIFI_COUNTRY_POLICY_AUTO,
        };

    When Wi-Fi is in station/AP coexistence mode, both use the same country/region information. Sometimes, the connected AP's country/region information may differ from the station's preset. For example:

    Station configuration::

        wifi_country_t config = {
            .cc = "JP",
            .schan = 1,
            .nchan = 14,
            .wifi_5g_channel_mask = 0xfe,
            .policy = WIFI_COUNTRY_POLICY_AUTO,
        };

    Connected AP configuration::

        wifi_country_t config = {
            .cc = "CN",
            .schan = 1,
            .nchan = 13,
        };

    In this case, the connected AP's country/region information will be used.

    The following table explains which country/region information is used under different Wi-Fi modes and policies, as well as differences in scanning behavior:

    .. list-table::
      :header-rows: 1
      :widths: 15 15 35

      * - Wi-Fi Mode
        - Policy
        - Description
      * - Station mode
        - WIFI_COUNTRY_POLICY_AUTO
        - If the connected AP's beacon contains country/region information IE, that information is used; otherwise, the default configuration is used.

          Scanning behavior:

            - 2.4 GHz band: active scanning on channels 1–11, passive scanning on channels 12–14;
            - 5 GHz band: active scanning on non-DFS channels, passive scanning on DFS channels.

          Note: If an AP with a hidden SSID is on a passive scan channel, scanning may not discover that AP. To discover hidden SSIDs on all channels, use `WIFI_COUNTRY_POLICY_MANUAL`.

      * - Station mode
        - WIFI_COUNTRY_POLICY_MANUAL
        - Always use the configured country/region information.

          Scanning behavior:

            - 2.4 GHz band: scan channels from `schan` to `schan + nchan - 1`;
            - 5 GHz band: scan channels supported as indicated by `wifi_5g_channel_mask`.

      * - AP mode
        - WIFI_COUNTRY_POLICY_AUTO
        - Always use the configured country/region information.
      * - AP mode
        - WIFI_COUNTRY_POLICY_MANUAL
        - Always use the configured country/region information.
      * - Station/AP coexistence mode
        - WIFI_COUNTRY_POLICY_AUTO
        - Same behavior as station mode with `WIFI_COUNTRY_POLICY_AUTO`.

          If the station is not connected to any AP, the AP uses the configured country/region information;
          if the station is connected to an external AP, the AP uses the country/region information obtained by the station.

      * - Station/AP coexistence mode
        - WIFI_COUNTRY_POLICY_MANUAL
        - Same behavior as station mode with `WIFI_COUNTRY_POLICY_MANUAL`. The AP always uses the configured country/region information.


Home Channel
*************************

In AP mode, the home channel is defined as the AP channel. In station mode, home channel is defined as the channel of AP which the station is connected to. In station/AP-coexistence mode, the home channel of AP and station must be the same, and if they are different, the station's home channel is always in priority. For example, assume that the AP is on channel 6, and the station connects to an AP whose channel is 9. Since the station's home channel has higher priority, the AP needs to switch its channel from 6 to 9 to make sure that it has the same home channel as the station. While switching channel, the {IDF_TARGET_NAME} in AP mode will notify the connected stations about the channel migration using a Channel Switch Announcement (CSA). Station that supports channel switching will transit without disconnecting and reconnecting to the AP.


.. _wifi-menuconfig:

Wi-Fi Menuconfig
-----------------

.. _wifi-buffer-configure:

Wi-Fi Buffer Configure
+++++++++++++++++++++++

If you are going to modify the default number or type of buffer, it would be helpful to also have an overview of how the buffer is allocated/freed in the data path. The following diagram shows this process in the TX direction:

.. blockdiag::
    :caption: TX Buffer Allocation
    :align: center

    blockdiag buffer_allocation_tx {

        # global attributes
        node_height = 60;
        node_width = 100;
        span_width = 50;
        span_height = 20;
        default_shape = roundedbox;

        # labels of diagram nodes
        APPL_TASK [label="Application\n task", fontsize=12];
        LwIP_TASK [label="LwIP\n task", fontsize=12];
        WIFI_TASK [label="Wi-Fi\n task", fontsize=12];

        # labels of description nodes
        APPL_DESC [label="1> User data", width=120, height=25, shape=note, color=yellow];
        LwIP_DESC [label="2> Pbuf", width=120, height=25, shape=note, color=yellow];
        WIFI_DESC [label="3> Dynamic (Static)\n TX Buffer", width=150, height=40, shape=note, color=yellow];

        # node connections
        APPL_TASK -> LwIP_TASK -> WIFI_TASK
        APPL_DESC -> LwIP_DESC -> WIFI_DESC [style=none]
    }


Description:

 - The application allocates the data which needs to be sent out.
 - The application calls TCPIP-/Socket-related APIs to send the user data. These APIs will allocate a PBUF used in LwIP, and make a copy of the user data.
 - When LwIP calls a Wi-Fi API to send the PBUF, the Wi-Fi API will allocate a "Dynamic Tx Buffer" or "Static Tx Buffer", make a copy of the LwIP PBUF, and finally send the data.

The following diagram shows how buffer is allocated/freed in the RX direction:

.. blockdiag::
    :caption: RX Buffer Allocation
    :align: center

    blockdiag buffer_allocation_rx {

        # global attributes
        node_height = 60;
        node_width = 100;
        span_width = 40;
        span_height = 20;
        default_shape = roundedbox;

        # labels of diagram nodes
        APPL_TASK [label="Application\n task", fontsize=12];
        LwIP_TASK [label="LwIP\n task", fontsize=12];
        WIFI_TASK [label="Wi-Fi\n task", fontsize=12];
        WIFI_INTR [label="Wi-Fi\n interrupt", fontsize=12];

        # labels of description nodes
        APPL_DESC [label="4> User\n Data Buffer", height=40, shape=note, color=yellow];
        LwIP_DESC [label="3> Pbuf", height=40, shape=note, color=yellow];
        WIFI_DESC [label="2> Dynamic\n RX Buffer", height=40, shape=note, color=yellow];
        INTR_DESC [label="1> Static\n RX Buffer", height=40, shape=note, color=yellow];

        # node connections
        APPL_TASK <- LwIP_TASK <- WIFI_TASK <- WIFI_INTR
        APPL_DESC <- LwIP_DESC <- WIFI_DESC <- INTR_DESC [style=none]
    }

Description:

 - The Wi-Fi hardware receives a packet over the air and puts the packet content to the "Static Rx Buffer", which is also called "RX DMA Buffer".
 - The Wi-Fi driver allocates a "Dynamic Rx Buffer", makes a copy of the "Static Rx Buffer", and returns the "Static Rx Buffer" to hardware.
 - The Wi-Fi driver delivers the packet to the upper-layer (LwIP), and allocates a PBUF for holding the "Dynamic Rx Buffer".
 - The application receives data from LwIP.

The diagram shows the configuration of the Wi-Fi internal buffer.

.. list-table::
   :header-rows: 1
   :widths: 10 10 10 10 25

   * - Buffer Type
     - Alloc Type
     - Default
     - Configurable
     - Description
   * - Static RX Buffer (Hardware RX Buffer)
     - Static
     - 10 * 1600 Bytes
     - Yes
     - This is a kind of DMA memory. It is initialized in :cpp:func:`esp_wifi_init()` and freed in :cpp:func:`esp_wifi_deinit()`. The ‘Static Rx Buffer’ forms the hardware receiving list. Upon receiving a frame over the air, hardware writes the frame into the buffer and raises an interrupt to the CPU. Then, the Wi-Fi driver reads the content from the buffer and returns the buffer back to the list.

       If needs be, the application can reduce the memory statically allocated by Wi-Fi. It can reduce this value from 10 to 6 to save 6400 Bytes of memory. It is not recommended to reduce the configuration to a value less than 6 unless the AMPDU feature is disabled.
   * - Dynamic RX Buffer
     - Dynamic
     - 32
     - Yes
     - The buffer length is variable and it depends on the received frames’ length. When the Wi-Fi driver receives a frame from the ‘Hardware Rx Buffer’, the ‘Dynamic Rx Buffer’ needs to be allocated from the heap. The number of the Dynamic Rx Buffer, configured in the menuconfig, is used to limit the total un-freed Dynamic Rx Buffer number.
   * - Dynamic TX Buffer
     - Dynamic
     - 32
     - Yes
     - This is a kind of DMA memory. It is allocated to the heap. When the upper-layer (LwIP) sends packets to the Wi-Fi driver, it firstly allocates a ‘Dynamic TX Buffer’ and makes a copy of the upper-layer buffer.

       The Dynamic and Static TX Buffers are mutually exclusive.
   * - Static TX Buffer
     - Static
     - 16 * 1600Bytes
     - Yes
     - This is a kind of DMA memory. It is initialized in :cpp:func:`esp_wifi_init()` and freed in :cpp:func:`esp_wifi_deinit()`. When the upper-layer (LwIP) sends packets to the Wi-Fi driver, it firstly allocates a ‘Static TX Buffer’ and makes a copy of the upper-layer buffer.

       The Dynamic and Static TX Buffer are mutually exclusive.

       The TX buffer must be a DMA buffer. For this reason, if PSRAM is enabled, the TX buffer must be static.
   * - Management Short Buffer
     - Dynamic
     - 8
     - NO
     - Wi-Fi driver’s internal buffer.
   * - Management Long Buffer
     - Dynamic
     - 32
     - NO
     - Wi-Fi driver’s internal buffer.
   * - Management Long Long Buffer
     - Dynamic
     - 32
     - NO
     - Wi-Fi driver’s internal buffer.

.. _wifi-nvs-flash:

Wi-Fi NVS Flash
+++++++++++++++++++++

If the Wi-Fi NVS flash is enabled, all Wi-Fi configurations set via the Wi-Fi APIs will be stored into flash, and the Wi-Fi driver will start up with these configurations the next time it powers on/reboots. However, the application can choose to disable the Wi-Fi NVS flash if it does not need to store the configurations into persistent memory, or has its own persistent storage, or simply due to debugging reasons, etc.


Troubleshooting
----------------

Please refer to a separate document with :doc:`../wireshark-user-guide`.

.. toctree::
    :hidden:

    ../wireshark-user-guide
