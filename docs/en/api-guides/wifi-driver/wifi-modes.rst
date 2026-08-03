Wi-Fi Modes
===================================

:link_to_translation:`zh_CN:[中文]`

.. _wifi-ap-general-scenario:

{IDF_TARGET_NAME} Wi-Fi AP General Scenario
---------------------------------------------

Below is a "big scenario" which describes some small scenarios in AP mode:

 .. seqdiag::
    :caption: Sample Wi-Fi Event Scenarios in AP Mode
    :align: center

    seqdiag sample-scenarios-soft-ap-mode {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 140;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        MAIN_TASK  [label = "Main\ntask"];
        APP_TASK   [label = "App\ntask"];
        EVENT_TASK [label = "Event\ntask"];
        LwIP_TASK  [label = "LwIP\ntask"];
        WIFI_TASK  [label = "Wi-Fi\ntask"];

        === 1. Init Phase ===
        MAIN_TASK  ->  LwIP_TASK   [label="1.1> Create / init LwIP"];
        MAIN_TASK  ->  EVENT_TASK  [label="1.2> Create / init event"];
        MAIN_TASK  ->  WIFI_TASK   [label="1.3> Create / init Wi-Fi"];
        MAIN_TASK  ->  APP_TASK    [label="1.4> Create app task"];
        === 2. Configure Phase ===
        MAIN_TASK  ->  WIFI_TASK   [label="2> Configure Wi-Fi"];
        === 3. Start Phase ===
        MAIN_TASK  ->  WIFI_TASK   [label="3.1> Start Wi-Fi"];
        EVENT_TASK <-  WIFI_TASK   [label="3.2> WIFI_EVENT_AP_START"];
        APP_TASK   <-  EVENT_TASK  [label="3.3> WIFI_EVENT_AP_START"];
        === 4. Connect Phase ===
        EVENT_TASK <-  WIFI_TASK   [label="4.1> WIFI_EVENT_AP_STACONNECTED"];
        APP_TASK   <- EVENT_TASK   [label="4.2> WIFI_EVENT_AP_STACONNECTED"];
        === 5. Disconnect Phase ===
        EVENT_TASK <-  WIFI_TASK   [label="5.1> WIFI_EVENT_AP_STADISCONNECTED"];
        APP_TASK   <-  EVENT_TASK  [label="5.2> WIFI_EVENT_AP_STADISCONNECTED"];
        APP_TASK   ->  APP_TASK    [label="5.3> disconnect handling"];
        === 6. Deinit Phase ===
        APP_TASK   ->  WIFI_TASK   [label="6.1> Disconnect Wi-Fi"];
        APP_TASK   ->  WIFI_TASK   [label="6.2> Stop Wi-Fi"];
        APP_TASK   ->  WIFI_TASK   [label="6.3> Deinit Wi-Fi"];
    }


Wi-Fi Sniffer Mode
---------------------------

The Wi-Fi sniffer mode can be enabled by :cpp:func:`esp_wifi_set_promiscuous`. If the sniffer mode is enabled, the following packets **can** be dumped to the application:

 - 802.11 Management frame.
 - 802.11 Data frame, including MPDU, AMPDU, and AMSDU.
 - 802.11 MIMO frame, for MIMO frame, the sniffer only dumps the length of the frame.
 - 802.11 Control frame.
 - 802.11 CRC error frame.

The following packets will **NOT** be dumped to the application:

 - Other 802.11 error frames.

For frames that the sniffer **can** dump, the application can additionally decide which specific type of packets can be filtered to the application by using :cpp:func:`esp_wifi_set_promiscuous_filter()` and :cpp:func:`esp_wifi_set_promiscuous_ctrl_filter()`. By default, it will filter all 802.11 data and management frames to the application. If you want to filter the 802.11 control frames, the filter parameter in :cpp:func:`esp_wifi_set_promiscuous_filter()` should include `WIFI_PROMIS_FILTER_MASK_CTRL` type, and if you want to differentiate control frames further, then call :cpp:func:`esp_wifi_set_promiscuous_ctrl_filter()`.

The Wi-Fi sniffer mode can be enabled in the Wi-Fi mode of :cpp:enumerator:`WIFI_MODE_NULL`, :cpp:enumerator:`WIFI_MODE_STA`, :cpp:enumerator:`WIFI_MODE_AP`, or :cpp:enumerator:`WIFI_MODE_APSTA`. In other words, the sniffer mode is active when the station is connected to the AP, or when the AP has a Wi-Fi connection. Please note that the sniffer has a **great impact** on the throughput of the station or AP Wi-Fi connection. Generally, the sniffer should be enabled **only if** the station/AP Wi-Fi connection does not experience heavy traffic.

Another noteworthy issue about the sniffer is the callback :cpp:type:`wifi_promiscuous_cb_t`. The callback will be called directly in the Wi-Fi driver task, so if the application has a lot of work to do for each filtered packet, the recommendation is to post an event to the application task in the callback and defer the real work to the application task.


.. only:: SOC_WIFI_NAN_SUPPORT

    Wi-Fi Aware\ :sup:`TM` (NAN)
    ----------------------------

    Wi-Fi Aware\ :sup:`TM` or NAN (Neighbor Awareness Networking) is a protocol that allows Wi-Fi devices to discover services in their proximity. NAN uses direct device-to-device communication and does not require any Internet or AP connection.

    Multiple NAN devices in the vicinity will form a NAN cluster which allows them to communicate with each other. NAN devices in a cluster synchronise their clocks and listen to each other periodically on Channel 6. Devices can advertise (Publish) or seek for (Subscribe) services within their NAN Cluster using Service Discovery protocols. Matching of services is done by service name and optionally matching filters. Once a Subscriber gets a match with a Publisher, it can either send a message (Follow-up) or establish a datapath (NDP) with the Publisher. After NDP is setup both devices will obtain an IPv6 address and can use it for communication.

    By default the NAN datapath is unencrypted. NAN Security (encrypted NDP) can be enabled per service using passphrase/PMK-based (NCS-SK-128) or pairing-based (NCS-PK-PASN-128) credentials. See :doc:`Wi-Fi Security <../wifi-security>` for details.

    .. note::

        The NAN datapath security based on shared passphrase/PMK (``WIFI_NAN_CSID_NCS_SK_128``) is available with stand-alone ESP-IDF and requires :ref:`CONFIG_ESP_WIFI_NAN_SECURITY`.

        NAN Pairing (``WIFI_NAN_CSID_NCS_PK_PASN_128``) requires :ref:`CONFIG_ESP_WIFI_NAN_PAIRING` and the **Wi-Fi Aware** external component, which is **not** bundled with stand-alone ESP-IDF. Add it to your project from the `ESP Component Registry <https://components.espressif.com/components/espressif/wifi_aware/>`__ by listing it in your project's ``idf_component.yml``:

        .. code-block:: yaml

            dependencies:
              espressif/wifi_aware: "^0.0.1"

    NAN operates in standalone mode, which means co-existence with STA or AP interface is not supported. NAN uses a separate interface for Discovery and Datapath, which is other than that used for STA and AP.

    Refer to ESP-IDF examples :idf_file:`examples/wifi/wifi_aware/nan_publisher/README.md` and :idf_file:`examples/wifi/wifi_aware/nan_subscriber/README.md` to setup a NAN Publisher and Subscriber.


    Unsynchronized Service Discovery (USD)
    --------------------------------------------

    Unsynchronized Service Discovery (USD) is a mechanism for devices to discover the services that have been made discoverable on new devices that enter the RF environment, without requiring synchronization between the devices.

    USD uses Service Info field in the SDEA of a Publish and Follow-up message to convey the service specific information.

    Refer to ESP-IDF examples :idf_file:`examples/wifi/wifi_aware/usd_publisher/README.md` and :idf_file:`examples/wifi/wifi_aware/usd_subscriber/README.md` to setup a NAN-USD Publisher and Subscriber.
