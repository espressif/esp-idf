Wi-Fi Driver
=============

Important Notes
----------------

- This document describes the implementation of only the **latest** IDF release. Backward compatibility with older versions of ESP-IDF is not guaranteed.
- This document describes the features which have already been implemented in the **latest** IDF release. For  features that are now in developing/testing status, we also provide brief descriptions, while indicating the release versions in which these features will be eventually implemented.
- If you find anything wrong/ambiguous/hard to understand or inconsistent with the implementation, feel free to let us know about it on our IDF GitHub page.


ESP32 Wi-Fi Feature List
-------------------------
- Supports Station-only mode, SoftAP-only mode, Station/SoftAP-coexistence mode
- Supports IEEE-802.11B, IEEE-802.11G, IEEE802.11N and APIs to configure the protocol mode
- Supports WPA/WPA2/WPA2-Enterprise and WPS 
- Supports AMPDU, HT40, QoS and other key features
- Supports Modem-sleep
- Supports an Espressif-specific protocol which, in turn, supports up to **1 km** of data traffic
- Up to 20 MBit/sec TCP throughput and 30 MBit/sec UDP throughput over the air
- Supports Sniffer
- Support set fast_crypto algorithm and normal algorithm switch which used in wifi connect
- Support both fast scan and all channel scan feature

How To Write a Wi-Fi Application
----------------------------------

Preparation
+++++++++++
Generally, the most effective way to begin your own Wi-Fi application is to select an example which is similar to your own application, and port the useful part into your project. It is not a MUST but it is strongly recommended that you take some time to read this article first, especially if you want to program a robust Wi-Fi application. This article is supplementary to the Wi-Fi APIs/Examples. It describes the principles of using the Wi-Fi APIs, the limitations of the current Wi-Fi API implementation, and the most common pitfalls in using Wi-Fi. This article also reveals some design details of the Wi-Fi driver. We recommend that you become familiar at least with the following sections: <`ESP32 Wi-Fi API Error Code`_>, <`ESP32 Wi-Fi Programming Model`_>, and <`ESP32 Wi-Fi Event Description`_>.

Setting Wi-Fi Compile-time Options
++++++++++++++++++++++++++++++++++++
Refer to <`Wi-Fi Menuconfig`_>

Init Wi-Fi
+++++++++++
Refer to <`ESP32 Wi-Fi Station General Scenario`_>, <`ESP32 Wi-Fi soft-AP General Scenario`_>.

Start/Connect Wi-Fi
++++++++++++++++++++
Refer to <`ESP32 Wi-Fi Station General Scenario`_>, <`ESP32 Wi-Fi soft-AP General Scenario`_>.

Event-Handling
++++++++++++++
Generally, it is easy to write code in "sunny-day" scenarios, such as <`SYSTEM_EVENT_STA_START`_>, <`SYSTEM_EVENT_STA_CONNECTED`_> etc. The hard part is to write routines in "rainy-day" scenarios, such as <`SYSTEM_EVENT_STA_DISCONNECTED`_> etc. Good handling of "rainy-day" scenarios is fundamental to robust Wi-Fi applications. Refer to <`ESP32 Wi-Fi Event Description`_>, <`ESP32 Wi-Fi Station General Scenario`_>, <`ESP32 Wi-Fi soft-AP General Scenario`_>

Write Error-Recovery Routines Correctly at All Times 
++++++++++++++++++++++++++++++++++++++++++++++++++++
Just like the handling of "rainy-day" scenarios, a good error-recovery routine is also fundamental to robust Wi-Fi applications. Refer to <`ESP32 Wi-Fi API Error Code`_>


ESP32 Wi-Fi API Error Code
---------------------------
All of the ESP32 Wi-Fi APIs have well-defined return values, namely, the error code. The error code can be categorized into:
 - No errors, e.g. ESP_ERR_WIFI_OK means that the API returns successfully
 - Recoverable errors, such as ESP_ERR_WIFI_NO_MEM, etc.
 - Non-recoverable, non-critical errors 
 - Non-recoverable, critical errors 

Whether the error is critical or not depends on the API and the application scenario, and it is defined by the API user.

**The primary principle to write a robust application with Wi-Fi API is to always check the error code and write the error-handling code.** Generally, the error-handling code can be used:

 - for recoverable errors, in which case you can write a recoverable-error code. For example, when esp_wifi_start returns ESP_ERR_WIFI_NO_MEM, the recoverable-error code vTaskDelay can be called, in order to get a microseconds' delay for another try.
 - for non-recoverable, yet non-critical, errors, in which case printing the error code is a good method for error handling.
 - for non-recoverable, critical errors, in which case "assert" may be a good method for error handling. For example, if esp_wifi_set_mode returns ESP_ERR_WIFI_NOT_INIT, it means that the Wi-Fi driver is not initialized by esp_wifi_init successfully. You can detect this kind of error very quickly in the application development phase.

In esp_err.h, ESP_ERROR_CHECK checks the return values. It is a rather commonplace error-handling code and can be used
as the default error-handling code in the application development phase. However, we strongly recommend that the API user writes their own error-handling code.

ESP32 Wi-Fi Programming Model
------------------------------
The ESP32 Wi-Fi programming model is depicted as follows::

                            default handler              user handler
   -----------               -------------               -------------
  |           |   event     |             | callback or |             |
  |   TCPIP   | --------->  |    event    | ----------> | application |
  |   stack   |             |     task    |  event      |    task     |
   -----------               -------------               -------------
                                  /|\                          |
                                   |                           |
                            event  |                           |
                                   |                           |
                                   |                           |
                              -------------                    |
                             |             |                   |
                             | Wi-Fi Driver|/__________________|
                             |             |\     API call
                             |             |
                              -------------

The Wi-Fi driver can be considered a black box that knows nothing about high-layer code, such as the
TCPIP stack, application task, event task, etc. All the Wi-Fi driver can do is receive API calls from the high layer,
or post an event-queue to a specified queue which is initialized by API esp_wifi_init().

The event task is a daemon task which receives events from the Wi-Fi driver or from other subsystems, such
as the TCPIP stack. The event task will call the default callback function upon receiving the event. For example,
upon receiving SYSTEM_EVENT_STA_CONNECTED, it will call tcpip_adapter_start() to start the DHCP
client in its default handler.

An application can register its own event callback function by using API esp_event_init. Then, the application callback
function will be called after the default callback. Also, if the application does not want to execute the callback
in the event task, it needs to post the relevant event to the application task in the application callback function.

The application task (code) generally mixes all these things together: it calls APIs to initialize the system/Wi-Fi and
handle the events when necessary.

ESP32 Wi-Fi Event Description
------------------------------------
SYSTEM_EVENT_WIFI_READY
++++++++++++++++++++++++++++++++++++
The Wi-Fi driver will never generate this event, which, as a result, can be ignored by the application event callback. This event may be removed in future releases.

SYSTEM_EVENT_SCAN_DONE
++++++++++++++++++++++++++++++++++++
The scan-done event is triggered by esp_wifi_scan_start() and will arise in the following scenarios:

  - The scan is completed, e.g., the target AP is found successfully, or all channels have been scanned.
  - The scan is stopped by esp_wifi_scan_stop().
  - The esp_wifi_scan_start() is called before the scan is completed. A new scan will override the current scan and a scan-done event will be generated.  

The scan-done event will not arise in the following scenarios:

  - It is a blocked scan.
  - The scan is caused by esp_wifi_connect().

Upon receiving this event, the event task does nothing. The application event callback needs to call esp_wifi_scan_get_ap_num() and esp_wifi_scan_get_ap_records() to fetch the scanned AP list and trigger the Wi-Fi driver to free the internal memory which is allocated during the scan **(do not forget to do this)**! 
Refer to 'ESP32 Wi-Fi Scan' for a more detailed description.

SYSTEM_EVENT_STA_START
++++++++++++++++++++++++++++++++++++
If esp_wifi_start() returns ESP_OK and the current Wi-Fi mode is Station or SoftAP+Station, then this event will arise. Upon receiving this event, the event task will initialize the LwIP network interface (netif). Generally, the application event callback needs to call esp_wifi_connect() to connect to the configured AP.

SYSTEM_EVENT_STA_STOP
++++++++++++++++++++++++++++++++++++
If esp_wifi_stop() returns ESP_OK and the current Wi-Fi mode is Station or SoftAP+Station, then this event will arise. Upon receiving this event, the event task will release the station's IP address, stop the DHCP client, remove TCP/UDP-related connections and clear the LwIP station netif, etc. The application event callback generally does not need to do anything.

SYSTEM_EVENT_STA_CONNECTED
++++++++++++++++++++++++++++++++++++
If esp_wifi_connect() returns ESP_OK and the station successfully connects to the target AP, the connection event will arise. Upon receiving this event, the event task starts the DHCP client and begins the DHCP process of getting the IP address. Then, the Wi-Fi driver is ready for sending and receiving data. This moment is good for beginning the application work, provided that the application does not depend on LwIP, namely the IP address. However, if the application is LwIP-based, then you need to wait until the *got ip* event comes in.
  
SYSTEM_EVENT_STA_DISCONNECTED
++++++++++++++++++++++++++++++++++++
This event can be generated in the following scenarios:

  - When esp_wifi_disconnect(), or esp_wifi_stop(), or esp_wifi_deinit(), or esp_wifi_restart() is called and the station is already connected to the AP.
  - When esp_wifi_connect() is called, but the Wi-Fi driver fails to set up a connection with the AP due to certain reasons, e.g. the scan fails to find the target AP, authentication times out, etc.
  - When the Wi-Fi connection is disrupted because of specific reasons, e.g., the station continuously loses N beacons, the AP kicks off the station, the AP's authentication mode is changed, etc. 

Upon receiving this event, the event task will shut down the station's LwIP netif and notify the LwIP task to clear the UDP/TCP connections which cause the wrong status to all sockets. **For socket-based applications, the application callback needs to close all sockets and re-create them, if necessary, upon receiving this event.**

Now, let us consider the following scenario:

 - The application creates a TCP connection to maintain the application-level keep-alive data that is sent out every 60 seconds.
 - Due to certain reasons, the Wi-Fi connection is cut off, and the <`SYSTEM_EVENT_STA_DISCONNECTED`_> is raised. According to the current implementation, **all TCP connections will be removed and the keep-alive socket will be in a wrong status**. However, since the application designer believes that the network layer should NOT care about this error at the Wi-Fi layer, the application does not close the socket.
 - Five seconds later, the Wi-Fi connection is restored because esp_wifi_connect() is called in the application event callback function.
 - Sixty seconds later, when the application sends out data with the keep-alive socket, the socket returns an error and the application closes the socket and re-creates it when necessary.

Generally, if the application has a correct error-handling code, upon receiving <`SYSTEM_EVENT_STA_DISCONNECTED`_> the socket can quickly detect the failure without having to wait for 55 seconds. For applications similar to the keep-alive example, we suggest that you close all sockets, once the <`SYSTEM_EVENT_STA_DISCONNECTED`_> is received, and that you restart the application when SYSTEM_EVENT_STA_CONNECTED arises.

Ideally, the application sockets and the network layer should not be affected, since the Wi-Fi connection only fails temporarily and recovers very quickly. In future IDF releases, we are going to provide a more robust solution for handling events that disrupt Wi-Fi connection, as ESP32's Wi-Fi functionality continuously improves.


SYSTEM_EVENT_STA_AUTHMODE_CHANGE
++++++++++++++++++++++++++++++++++++
This event arises when the AP to which the station is connected changes its authentication mode, e.g., from no auth to WPA. Upon receiving this event, the event task will do nothing. Generally, the application event callback does not need to handle this either.

SYSTEM_EVENT_STA_GOT_IP
++++++++++++++++++++++++++++++++++++
SYSTEM_EVENT_AP_STA_GOT_IP6
++++++++++++++++++++++++++++++++++++
This event arises when the DHCP client successfully gets the IP address from the DHCP server. The event means that everything is ready and the application can begin its tasks (e.g., creating sockets).

The IP may be changed because of the following reasons:

  - The DHCP client fails to renew/rebind the IP address, and the station's IP is reset to 0.
  - The DHCP client rebinds to a different address.
  - The static-configured IP address is changed.

The socket is based on the IP address, which means that, if the IP changes, all sockets relating to this IP will become abnormal. Upon receiving this event, the application needs to close all sockets and recreate the application when the IP changes to a valid one.

SYSTEM_EVENT_AP_START
++++++++++++++++++++++++++++++++++++
Similar to <`SYSTEM_EVENT_STA_START`_>.

SYSTEM_EVENT_AP_STOP
++++++++++++++++++++++++++++++++++++
Similar to <`SYSTEM_EVENT_STA_STOP`_>.

SYSTEM_EVENT_AP_STACONNECTED
++++++++++++++++++++++++++++++++++++
Every time a station is connected to ESP32 SoftAP, the <`SYSTEM_EVENT_AP_STACONNECTED`_> will arise. Upon receiving this event, the event task will do nothing, and the application callback can also ignore it. However, you may want to do something, for example, to get the info of the connected STA, etc.

SYSTEM_EVENT_AP_STADISCONNECTED
++++++++++++++++++++++++++++++++++++
This event can happen in the following scenarios:

  - The application calls esp_wifi_disconnect(), or esp_wifi_deauth_sta(), to manually disconnect the station.
  - The Wi-Fi driver kicks off the station, e.g. because the SoftAP has not received any packets in the past five minutes, etc.
  - The station kicks off the SoftAP.

When this event happens, the event task will do nothing, but the application event callback needs to do something, e.g., close the socket which is related to this station, etc.

SYSTEM_EVENT_AP_PROBEREQRECVED
++++++++++++++++++++++++++++++++++++
Currently, the ESP32 implementation will never generate this event. It may be removed in future releases.

ESP32 Wi-Fi Station General Scenario
---------------------------------------
Below is a "big scenario" which describes some small scenarios in Station mode::

    ---------           ---------           ---------           ---------          ---------
   |  Main   |         |   app   |         |  Event  |         |  LwIP   |        |  Wi-Fi  |
   |  task   |         |   task  |         |   task  |         |  task   |        |  task   |
    ---------           ---------           ---------           ---------          ---------
        |                   |                   |                   |                  |         ---
        |              1.1> create/init LwIP    |                   |                  |          |
        |---------------------------------------------------------->|                  |          |
        |     1.2> create/init event            |                   |                  |       
        |-------------------------------------->|                   |                  |      1. Init Phase
        |                   |    1.3> create/init Wi-Fi             |                  |        
        |----------------------------------------------------------------------------->|          |
        |  1.4> create app task                 |                   |                  |          |
        |------------------>|                   |                   |                  |          |
        |                   |                   |                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |         
        |                   |    2> configure Wi-Fi                 |                  |      2. Configure Phase
        |----------------------------------------------------------------------------->|         
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |         ---
        |                   |         3.1> start Wi-Fi              |                  |          |
        |----------------------------------------------------------------------------->|     
        |                   |                   |     3.2 > SYSTEM_EVENT_STA_START     |     3. Start Phase
        |                   |                   |<-------------------------------------|          
        |              3.3> SYSTEM_EVENT_STA_START                  |                  |          |
        |                   |<------------------|                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |          |
        |                   |               4.1> connect wifi       |                  |          |
        |                   |--------------------------------------------------------->|     4. Connect Phase
        |                   |                   |   4.2> SYSTEM_EVENT_STA_CONNECTED    |          |
        |                   |                   |<-------------------------------------|          |
        |              4.3> SYSTEM_EVENT_STA_CONNECTED              |                  |          |
        |                   |<------------------|                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |          |
        |                   |               5.1> start DHCP client  |                  |          |
        |                   |                   |------------------>|                  |          |
        |                   |               5.2> SYSTEM_EVENT_STA_GOT_IP               |          |
        |                   |                   |<------------------|                  |          
        |                 5.3> SYSTEM_EVENT_STA_GOT_IP              |                  |     5. Got IP Phase
        |                   |<------------------|                   |                  |          
        |                   |-----              |                   |                  |          |
        |                   |     | 5.4> socket related init        |                  |          |
        |                   |<----              |                   |                  |          |
        |                   |                   |                   |                  |         ---
        |                   |                   |   6.1> SYSTEM_EVENT_STA_DISCONNECTED |          |
        |                   |                   |<-------------------------------------|     
        |              6.2> SYSTEM_EVENT_STA_DISCONNECTED           |                  |     6. Disconnect Phase     
        |                   |<------------------|                   |                  |     
        |                   |-----              |                   |                  |          |
        |                   |     | 6.3> disconnect handling        |                  |          |
        |                   |<----              |                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |               7.1> SYSTEM_EVENT_STA_GOT_IP               |          |
        |                   |                   |<------------------|                  |          |
        |                 7.2> SYSTEM_EVENT_STA_GOT_IP              |                  |          
        |                   |<------------------|                   |                  |    7. IP change phase
        |                   |-----              |                   |                  |          
        |                   |     | 7.3> socket error handling      |                  |          |
        |                   |<----              |                   |                  |          |
        |                   |                   |                   |                  |         ---
        |                   |           8.1> disconnect Wi-Fi       |                  |          |
        |                   |--------------------------------------------------------->|          
        |                   |           8.2> stop Wi-Fi             |                  |    8. Deinit phase      
        |                   |--------------------------------------------------------->|          
        |                   |           8.3> deinit Wi-Fi           |                  |          |
        |                   |--------------------------------------------------------->|          |
        |                   |                   |                   |                  |         ---  
  

1. Wi-Fi/LwIP Init Phase
++++++++++++++++++++++++++++++
 - s1.1: The main task calls tcpip_adapter_init() to create an LwIP core task and initialize LwIP-related work.

 - s1.2: The main task calls esp_event_loop_init() to create a system Event task and initialize an application event's callback function. In the scenario above, the application event's callback function does nothing but relaying the event to the application task. 

 - s1.3: The main task calls esp_wifi_init() to create the Wi-Fi driver task and initialize the Wi-Fi driver.

 - s1.4: The main task calls OS API to create the application task.

Step 1.1~1.4 is a recommended sequence that initializes a Wi-Fi-/LwIP-based application. However, it is **NOT** a must-follow sequence, which means that you can create the application task in step 1.1 and put all other initializations in the application task. Moreover, you may not want to create the application task in the initialization phase if the application task depends on the sockets. Rather, you can defer the task creation until the IP is obtained.

2. Wi-Fi Configuration Phase
+++++++++++++++++++++++++++++++
Once the Wi-Fi driver is initialized, you can start configuring the Wi-Fi driver. In this scenario, the mode is Station, so you may need to call esp_wifi_set_mode(WIFI_MODE_STA) to configure the Wi-Fi mode as Station. You can call other esp_wifi_set_xxx APIs to configure more settings, such as the protocol mode, country code, bandwidth, etc. Refer to <`ESP32 Wi-Fi Configuration`_>.

Generally, we configure the Wi-Fi driver before setting up the Wi-Fi connection, but this is **NOT** mandatory, which means that you can configure the Wi-Fi connection anytime, provided that the Wi-Fi driver is initialized successfully. However, if the configuration does not need to change after the Wi-Fi connection is set up, you should configure the Wi-Fi driver at this stage, because the configuration APIs (such as esp_wifi_set_protocol) will cause the Wi-Fi to reconnect, which may not be desirable.

If the Wi-Fi NVS flash is enabled by menuconfig, all Wi-Fi configuration in this phase, or later phases, will be stored into flash. When the board powers on/reboots, you do not need to configure the Wi-Fi driver from scratch. You only need to call esp_wifi_get_xxx APIs to fetch the configuration stored in flash previously. You can also configure the Wi-Fi driver if the previous configuration is not what you want.

3. Wi-Fi Start Phase
++++++++++++++++++++++++++++++++
 - s3.1: Call esp_wifi_start to start the Wi-Fi driver.
 - s3.2: The Wi-Fi driver posts <`SYSTEM_EVENT_STA_START`_> to the event task; then, the event task will do some common things and will call the application event callback function.
 - s3.3: The application event callback function relays the <`SYSTEM_EVENT_STA_START`_> to the application task. We recommend that you call esp_wifi_connect(). However, you can also call esp_wifi_connect() in other phrases after the <`SYSTEM_EVENT_STA_START`_> arises.
 
4. Wi-Fi Connect Phase
+++++++++++++++++++++++++++++++++
 - s4.1: Once esp_wifi_connect() is called, the Wi-Fi driver will start the internal scan/connection process.

 - s4.2: If the internal scan/connection process is successful, the <`SYSTEM_EVENT_STA_CONNECTED`_> will be generated. In the event task, it starts the DHCP client, which will finally trigger the DHCP process. 

 - s4.3: In the above-mentioned scenario, the application event callback will relay the event to the application task. Generally, the application needs to do nothing, and you can do whatever you want, e.g., print a log, etc.

In step 4.2, the Wi-Fi connection may fail because, for example, the password is wrong, the AP is not found, etc. In a case like this, <`SYSTEM_EVENT_STA_DISCONNECTED`_> will arise and the reason for such a failure will be provided. For handling events that disrupt Wi-Fi connection, please refer to phase 6.

5. Wi-Fi 'Got IP' Phase
+++++++++++++++++++++++++++++++++

 - s5.1: Once the DHCP client is initialized in step 4.2, the *got IP* phase will begin.
 - s5.2: If the IP address is successfully received from the DHCP server, then <`SYSTEM_EVENT_STA_GOT_IP`_> will arise and the event task will perform common handling.
 - s5.3: In the application event callback, <`SYSTEM_EVENT_STA_GOT_IP`_> is relayed to the application task. For LwIP-based applications, this event is very special and means that everything is ready for the application to begin its tasks, e.g. creating the TCP/UDP socket, etc. A very common mistake is to initialize the socket before <`SYSTEM_EVENT_STA_GOT_IP`_> is received. **DO NOT start the socket-related work before the IP is received.**

6. Wi-Fi Disconnect Phase
+++++++++++++++++++++++++++++++++
 - s6.1: When the Wi-Fi connection is disrupted, e.g. because the AP is powered off, the RSSI is poor, etc., <`SYSTEM_EVENT_STA_DISCONNECTED`_> will arise. This event may also arise in phase 3. Here, the event task will notify the LwIP task to clear/remove all UDP/TCP connections. Then, all application sockets will be in a wrong status. In other words, no socket can work properly when this event happens.
 - s6.2: In the scenario described above, the application event callback function relays <`SYSTEM_EVENT_STA_DISCONNECTED`_> to the application task. We recommend that esp_wifi_connect() be called to reconnect the Wi-Fi, close all sockets and re-create them if necessary. Refer to <`SYSTEM_EVENT_STA_DISCONNECTED`_>.
       
7. Wi-Fi IP Change Phase
++++++++++++++++++++++++++++++++++

 - s7.1: If the IP address is changed, the <`SYSTEM_EVENT_STA_GOT_IP`_> will arise.
 - s7.2: **This event is important to the application. When it occurs, the timing is good for closing all created sockets and recreating them.**


8. Wi-Fi Deinit Phase
++++++++++++++++++++++++++++

 - s8.1: Call esp_wifi_disconnect() to disconnect the Wi-Fi connectivity.
 - s8.2: Call esp_wifi_stop() to stop the Wi-Fi driver.
 - s8.3: Call esp_wifi_deinit() to unload the Wi-Fi driver.


ESP32 Wi-Fi soft-AP General Scenario
---------------------------------------------
Below is a "big scenario" which describes some small scenarios in Soft-AP mode::

    ---------           ---------           ---------           ---------          ---------
   |  Main   |         |   app   |         |  Event  |         |  LwIP   |        |  Wi-Fi  |
   |  task   |         |   task  |         |   task  |         |  task   |        |  task   |
    ---------           ---------           ---------           ---------          ---------
        |                   |                   |                   |                  |
        |                   |                   |                   |                  |         ---
        |              1.1> create/init LwIP    |                   |                  |          |
        |---------------------------------------------------------->|                  |          |
        |     1.2> create/init event            |                   |                  |          
        |-------------------------------------->|                   |                  |     1. Init Phase
        |                   |    1.3> create/init Wi-Fi             |                  |          
        |----------------------------------------------------------------------------->|          |
        |  1.4> create app task                 |                   |                  |          |
        |------------------>|                   |                   |                  |          |
        |                   |                   |                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |          
        |                   |    2> configure Wi-Fi                 |                  |     2. Configure Phase
        |----------------------------------------------------------------------------->|          
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |         3.1> start Wi-Fi              |                  |          
        |----------------------------------------------------------------------------->|     3. Start Phase
        |                   |                   |     3.2 > SYSTEM_EVENT_AP_START      |          
        |                   |                   |<-------------------------------------|          |
        |              3.3> SYSTEM_EVENT_AP_START                   |                  |          |
        |                   |<------------------|                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |          
        |                   |                   |   4.1> SYSTEM_EVENT_AP_STACONNECTED  |     4. Connect Phase      
        |                   |                   |<-------------------------------------|          
        |              4.2> SYSTEM_EVENT_AP_STACONNECTED            |                  |          |
        |                   |<------------------|                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |          |
        |                   |                   |   5.1> SYSTEM_EVENT_STA_DISCONNECTED |          
        |                   |                   |<-------------------------------------|     5. Disconnect Phase
        |              5.2> SYSTEM_EVENT_STA_DISCONNECTED           |                  |          
        |                   |<------------------|                   |                  |          |
        |                   |-----              |                   |                  |          |
        |                   |     | 5.3> disconnect handling        |                  |          |
        |                   |<----              |                   |                  |         ---
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |          |
        |                   |                   |                   |                  |          |
        |                   |           6.1> disconnect Wi-Fi       |                  |          |
        |                   |--------------------------------------------------------->|          
        |                   |           6.2> stop Wi-Fi             |                  |    6. Deinit phase      
        |                   |--------------------------------------------------------->|          
        |                   |           6.3> deinit Wi-Fi           |                  |          |
        |                   |--------------------------------------------------------->|          |
        |                   |                   |                   |                  |         ---  
        |                   |                   |                   |                  |         
  

ESP32 Wi-Fi Scan
------------------------

Currently, the esp_wifi_scan_start() API is supported only in Station or Station+SoftAP mode.

Scan Type 
+++++++++++++++++++++++++

+------------------+--------------------------------------------------------------+
| Mode             | Description                                                  |    
+==================+==============================================================+
| Active Scan      | Scan by sending a probe request.                             |
|                  | The default scan is an active scan.                          |    
|                  |                                                              |    
+------------------+--------------------------------------------------------------+
| Passive Scan     | No probe request is sent out. Just switch to the specific    |
|                  | channel and wait for a beacon.                               |    
|                  | Application can enable it via the scan_type field of         |
|                  | wifi_scan_config_t.                                          |    
|                  |                                                              |    
+------------------+--------------------------------------------------------------+
| Foreground Scan  | This scan is applicable when there is no Wi-Fi connection    |
|                  | in Station mode. Foreground or background scanning is        |
|                  | controlled by the Wi-Fi driver and cannot be configured by   |    
|                  | the application.                                             |    
+------------------+--------------------------------------------------------------+
| Background Scan  | This scan is applicable when there is a Wi-Fi connection in  |
|                  | Station mode or in Station+SoftAP mode.                      |    
|                  | Whether it is a foreground scan or background scan depends on|
|                  | the Wi-Fi driver and cannot be configured by the application.|    
|                  |                                                              |    
+------------------+--------------------------------------------------------------+
| All-Channel Scan | It scans all of the channels.                                |    
|                  | If the channel field of wifi_scan_config_t is set to 0, it is|
|                  | an all-channel scan.                                         |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| Specific Channel | It scans specific channels only.                             |
|     Scan         | If the channel field of wifi_scan_config_t set to 1, it is a |
|                  | specific-channel scan.                                       |    
|                  |                                                              |
+------------------+--------------------------------------------------------------+

The scan modes in above table can be combined arbitrarily, so we totally have 8 different scans:
 - All-Channel Background Active Scan
 - All-Channel Background Passive Scan
 - All-Channel Foreground Active Scan
 - All-Channel Foreground Passive Scan
 - Specific-Channel Background Active Scan
 - Specific-Channel Background Passive Scan
 - Specific-Channel Foreground Active Scan
 - Specific-Channel Foreground Passive Scan

Scan Configuration
+++++++++++++++++++++++++++++++++++++++

The scan type and other scan attributes are configured by esp_wifi_scan_start. The table below provides a detailed description of wifi_scan_config_t.

+------------------+--------------------------------------------------------------+
| Field            | Description                                                  |
+==================+==============================================================+
| ssid             | If the SSID is not NULL, it is only the AP with the same     |
|                  | SSID that can be scanned.                                    |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| bssid            | If the BSSID is not NULL, it is only the AP with the same    |
|                  | BSSID that can be scanned.                                   |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| channel          | If "channel" is 0, there will be an all-channel scan;        |
|                  | otherwise, there will be a specific-channel scan.            |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| show_hidden      | If "show_hidden" is 0, the scan ignores the AP with a hidden |
|                  | SSID; otherwise, the scan considers the hidden AP a normal   |
|                  | one.                                                         |
+------------------+--------------------------------------------------------------+
| scan_type        | If "scan_type" is WIFI_SCAN_TYPE_ACTIVE, the scan is         |
|                  | "active"; otherwise, it is a "passive" one.                  |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| scan_time        | This field is used to control how long the scan dwells on    |
|                  | each channel.                                                |
|                  |                                                              |
|                  | For passive scans, scan_time.passive designates the dwell    |
|                  | time for each channel.                                       |
|                  |                                                              |
|                  | For active scans, dwell times for each channel are listed    |
|                  | in the table below. Here, min is short for scan              |
|                  | time.active.min and max is short for scan_time.active.max.   |
|                  |                                                              |
|                  | - min=0, max=0: scan dwells on each channel for 120 ms.      |
|                  | - min>0, max=0: scan dwells on each channel for 120 ms.      |
|                  | - min=0, max>0: scan dwells on each channel for ``max`` ms.  |
|                  | - min>0, max>0: the minimum time the scan dwells on each     |
|                  |   channel is ``min`` ms. If no AP is found during this time  |
|                  |   frame, the scan switches to the next channel. Otherwise,   |
|                  |   the scan dwells on the channel for ``max`` ms.             |
|                  |                                                              |
|                  | If you want to improve the performance of the                |
|                  | the scan, you can try to modify these two parameters.        |
|                  |                                                              |
+------------------+--------------------------------------------------------------+

Scan All APs In All Channels(foreground)
+++++++++++++++++++++++++++++++++++++++++++

Scenario::

    ---------           ---------           --------- 
   |  app    |         |  event  |         |   Wi-Fi |
   |  task   |         |   task  |         |   task  |
    ---------           ---------           --------- 
        |                   |                   |
        |                   |                   |
        |      1.1> Configure country code      |
        |-------------------------------------->|
        |      1.2> Scan configuration          |
        |-------------------------------------->|
        |                   |                   |
        |                   |                   |
        |                   |                   |----
        |                   |                   |    | 2.1> Scan channel 1
        |                   |                   |<---
        |                   |                   |----
        |                   |                   |    | 2.2> Scan channel 2
        |                   |                   |<---
        |                   |                   |
        |                   |                   |      ....  ...
        |                   |                   |
        |                   |                   |----
        |                   |                   |    | 2.x> Scan channel N
        |                   |                   |<---
        |                   |                   |
        |           3.1 SYSTEM_EVENT_SCAN_DONE  |
        |                   |<------------------|
        |  3.2 SYSTEM_EVENT_SCAN_DONE           |
        |<------------------|                   |
        |                   |                   |


The scenario above describes an all-channel, foreground scan. The foreground scan can only occur in Station mode where the station does not connect to any AP. Whether it is a foreground or background scan is totally determined by the Wi-Fi driver, and cannot be configured by the application. 

Detailed scenario description:

Scan Configuration Phase
**************************

 - s1.1: Call esp_wifi_set_country() to set the country code. For China/Japan, the channel value ranges from 1 to 14; for the USA, it ranges from 1 to 11; and for Europe, it ranges from 1 to 13. The default country is China. 
 - s1.2: Call esp_wifi_scan_start() to configure the scan. To do so, you can refer to <`Scan Configuration`_>. Since this is an all-channel scan, just set the SSID/BSSID/channel to 0.


Wi-Fi Driver's Internal Scan Phase
**************************************

 - s2.1: The Wi-Fi driver switches to channel 1, in case the scan type is WIFI_SCAN_TYPE_ACTIVE, and broadcasts a probe request. Otherwise, the Wi-Fi will wait for a beacon from the APs. The Wi-Fi driver will stay in channel 1 for some time. The dwell time is configured in min/max time, with default value being 120 ms.
 - s2.2: The Wi-Fi driver switches to channel 2 and performs the same operation as in step 2.1.
 - s2.3: The Wi-Fi driver scans the last channel N, where N is determined by the country code which is configured in step 1.1.

Scan-Done Event Handling Phase
*********************************

 - s3.1: When all channels are scanned, <`SYSTEM_EVENT_SCAN_DONE`_> will arise.
 - s3.2: The application's event callback function notifies the application task that <`SYSTEM_EVENT_SCAN_DONE`_> is received. esp_wifi_scan_get_ap_num() is called to get the number of APs that have been found in this scan. Then, it allocates enough entries and calls esp_wifi_scan_get_ap_records() to get the AP records. Please note that the AP records in the Wi-Fi driver will be freed, once esp_wifi_scan_get_ap_records() is called. Do not call esp_wifi_scan_get_ap_records() twice for a single scan-done event. If esp_wifi_scan_get_ap_records() is not called when the scan-done event occurs, the AP records allocated by the Wi-Fi driver will not be freed. So, make sure you call esp_wifi_scan_get_ap_records(), yet only once.

Scan All APs on All Channels(background)
++++++++++++++++++++++++++++++++++++++++
Scenario::

    ---------           ---------           --------- 
   |  app    |         |  event  |         |   Wi-Fi |
   |  task   |         |   task  |         |   task  |
    ---------           ---------           --------- 
        |                   |                   |
        |                   |                   |
        |      1.1> Configure country code      |
        |-------------------------------------->|
        |      1.2> Scan configuration          |
        |-------------------------------------->|
        |                   |                   |
        |                   |                   |
        |                   |                   |----
        |                   |                   |    | 2.1> Scan channel 1
        |                   |                   |<---
        |                   |                   |----
        |                   |                   |    | 2.2> Back to home channel H
        |                   |                   |<---
        |                   |                   |----
        |                   |                   |    | 2.3> Scan channel 2
        |                   |                   |<---
        |                   |                   |----
        |                   |                   |    | 2.4> Back to home channel H
        |                   |                   |<---
        |                   |                   |
        |                   |                   |      ....  ...
        |                   |                   |
        |                   |                   |----
        |                   |                   |    | 2.x-1> Scan channel N
        |                   |                   |<---
        |                   |                   |----
        |                   |                   |    | 2.x> Back to home channel H
        |                   |                   |<---
        |                   |                   |
        |           3.1 SYSTEM_EVENT_SCAN_DONE  |
        |                   |<------------------|
        |  3.2 SYSTEM_EVENT_SCAN_DONE           |
        |<------------------|                   |
        |                   |                   |

The scenario above is an all-channel background scan. Compared to `Scan All APs In All Channels(foreground)`_ , the difference in the all-channel background scan is that the Wi-Fi driver will scan the back-to-home channel for 30 ms before it switches to the next channel to give the Wi-Fi connection a chance to transmit/receive data.

Scan for a Specific AP in All Channels
+++++++++++++++++++++++++++++++++++++++
Scenario::

    ---------           ---------           --------- 
   |  app    |         |  event  |         |   Wi-Fi |
   |  task   |         |   task  |         |   task  |
    ---------           ---------           --------- 
        |                   |                   |
        |                   |                   |
        |      1.1> Configure country code      |
        |-------------------------------------->|
        |      1.2> Scan configuration          |
        |-------------------------------------->|
        |                   |                   |
        |                   |                   |
        |                   |                   |----
        |                   |                   |    | 2.1> Scan channel C1
        |                   |                   |<---
        |                   |                   |----
        |                   |                   |    | 2.2> Scan channel C2
        |                   |                   |<---
        |                   |                   |
        |                   |                   | ...
        |                   |                   |
        |                   |                   |----
        |                   |                   |    | 2.x> Scan channel CN, or the AP is found
        |                   |                   |<---
        |                   |                   |
        |           3.1 SYSTEM_EVENT_SCAN_DONE  |
        |                   |<------------------|
        |  3.2 SYSTEM_EVENT_SCAN_DONE           |
        |<------------------|                   |
        |                   |                   |

This scan is similar to `Scan All APs In All Channels(foreground)`_. The differences are:

 - s1.1: In step 1.2, the target AP will be configured to SSID/BSSID.
 - s2.1~s2.N: Each time the Wi-Fi driver scans an AP, it will check whether it is a target AP or not. If it is a target AP, then the scan-done event will arise and scanning will end; otherwise, the scan will continue. Please note that the first scanned channel may not be channel 1, because the Wi-Fi driver optimizes the scanning sequence.

If there are more than one APs which match the target AP info, for example, if we happen to scan two APs whose SSID is "ap", then only the first AP will be returned. However, if the first AP is not the one you want, e.g., if its password is wrong, then the Wi-Fi driver will detect a four-way handshake failure and try to scan the next AP. If two APs have the same SSID, BSSID and password, then the Wi-Fi driver will choose the first one to connect to.

You can scan a specific AP, or all of them, in any given channel. These two scenarios are very similar.

Scan in Wi-Fi Connect
+++++++++++++++++++++++++

When esp_wifi_connect() is called, then the Wi-Fi driver will try to scan the configured AP first. The scan in "Wi-Fi Connect" is the same as `Scan for a Specific AP In All Channels`_, except that no scan-done event will be generated when the scan is completed. If the target AP is found, then the Wi-Fi driver will start the Wi-Fi connection; otherwise, <`SYSTEM_EVENT_STA_DISCONNECTED`_> will be generated. Refer to `Scan for a Specific AP in All Channels`_

Scan In Blocked Mode
++++++++++++++++++++

If the block parameter of esp_wifi_scan_start() is true, then the scan is a blocked one, and the application task will be blocked until the scan is done. The blocked scan is similar to an unblocked one, except that no scan-done event will arise when the blocked scan is completed.

Parallel Scan
+++++++++++++
Two application tasks may call esp_wifi_scan() at the same time, or the same application task calls esp_wifi_scan_start() before it gets a scan-done event. Both scenarios can happen. **However, in IDF2.1, the Wi-Fi driver does not support parallel scans adequately. As a result, a parallel scan should be avoided.** The parallel scan will be enhanced in future releases, as the ESP32's Wi-Fi functionality improves continuously.


ESP32 Wi-Fi Station Connecting Scenario
----------------------------------------
Generally, the application does not need to care about the connecting process. Below is a brief introduction to the process for those who are really interested.

Scenario::

    ---------           ---------           --------- 
   |  Event  |         |   Wi-Fi |         |  AP     |
   |  task   |         |   task  |         |         |
    ---------           ---------           --------- 
        |                   |                   |
        |                   |                   |       ---
        |                   |----               |        |
        |                   |    | 1.1> Scan    |        
        |                   |<---               |      Scan phase
        | 1.2> SYSTEM_EVENT_STA_DISCONNECTED    |      
        |<------------------|                   |        |
        |                   |                   |       ---
        |                   |                   |        |
        |                    2.1> Auth Request  |        |
        |                   |------------------>|        |
        | 2.2> SYSTEM_EVENT_STA_DISCONNECTED    |       
        |<------------------|                   |      Auth phase
        |                    2.3> Auth Response |       
        |                   |<------------------|        |
        | 2.4> SYSTEM_EVENT_STA_DISCONNECTED    |        |
        |<------------------|                   |       ---
        |                   |                   |        |
        |                   | 3.1 Assoc Request |        |
        |                   |------------------>|        |
        | 3.2> SYSTEM_EVENT_STA_DISCONNECTED    |        
        |<------------------|                   |     Assoc phase
        |                    3.3 Assoc Response |       
        |                   |<------------------|        |
        | 3.4> SYSTEM_EVENT_STA_DISCONNECTED    |        |
        |<------------------|                   |        |
        |                   |                   |       ---  
        |                   |                   |        |
        |                   | 4.1> 1/4 EAPOL    |        |
        |                   |<------------------|        |
        | 4.2> SYSTEM_EVENT_STA_DISCONNECTED    |        |
        |<------------------|                   |        |
        |                   | 4.3> 2/4 EAPOL    |        |
        |                   |------------------>|        |
        | 4.4> SYSTEM_EVENT_STA_DISCONNECTED    |        
        |<------------------|                   |     4-way handshake phase
        |                   | 4.5> 3/4 EAPOL    |        
        |                   |<------------------|        |
        | 4.6> SYSTEM_EVENT_STA_DISCONNECTED    |        |
        |<------------------|                   |        |
        |                   | 4.7> 4/4 EAPOL    |        |
        |                   |------------------>|        |
        | 4.8> SYSTEM_EVENT_STA_DISCONNECTED    |        |
        |<------------------|                   |        |
        |                   |                   |        |
        | 4.9> SYSTEM_EVENT_STA_DISCONNECTED    |        |
        |<------------------|                   |       ---  
        |                   |                   | 

Scan Phase
+++++++++++++++++++++

 - s1.1, The Wi-Fi driver begins scanning in "Wi-Fi Connect". Refer to <`Scan in Wi-Fi Connect`_> for more details.
 - s1.2, If the scan fails to find the target AP, <`SYSTEM_EVENT_STA_DISCONNECTED`_> will arise and the reason-code will be WIFI_REASON_NO_AP_FOUND. Refer to <`Wi-Fi Reason Code`_>. 

Auth Phase
+++++++++++++++++++++

 - s2.1, The authentication request packet is sent and the auth timer is enabled.
 - s2.2, If the authentication response packet is not received before the authentication timer times out, <`SYSTEM_EVENT_STA_DISCONNECTED`_> will arise and the reason-code will be WIFI_REASON_AUTH_EXPIRE. Refer to <`Wi-Fi Reason Code`_>. 
 - s2.3, The auth-response packet is received and the auth-timer is stopped.
 - s2.4, The AP rejects authentication in the response and <`SYSTEM_EVENT_STA_DISCONNECTED`_> arises, while the reason-code is WIFI_REASON_AUTH_FAIL or the reasons specified by the soft-AP. Refer to <`Wi-Fi Reason Code`_>.

Association Phase
+++++++++++++++++++++

 - s3.1, The association request is sent and the association timer is enabled.
 - s3.2, If the association response is not received before the association timer times out, <`SYSTEM_EVENT_STA_DISCONNECTED`_> will arise and the reason-code will be WIFI_REASON_ASSOC_EXPIRE. Refer to <`Wi-Fi Reason Code`_>.
 - s3.3, The association response is received and the association timer is stopped.
 - s3.4, The AP rejects the association in the response and <`SYSTEM_EVENT_STA_DISCONNECTED`_> arises, while the reason-code is the one specified in the association response. Refer to <`Wi-Fi Reason Code`_>. 


Four-way Handshake Phase
++++++++++++++++++++++++++

 - s4.1, The four-way handshake is sent out and the association timer is enabled.
 - s4.2, If the association response is not received before the association timer times out, <`SYSTEM_EVENT_STA_DISCONNECTED`_> will arise and the reason-code will be WIFI_REASON_ASSOC_EXPIRE. Refer to <`Wi-Fi Reason Code`_>. 
 - s4.3, The association response is received and the association timer is stopped.
 - s4.4, The AP rejects the association in the response and <`SYSTEM_EVENT_STA_DISCONNECTED`_> arises and the reason-code will be the one specified in the association response. Refer to <`Wi-Fi Reason Code`_>. 


Wi-Fi Reason Code
+++++++++++++++++++++

The table below shows the reason-code defined in ESP32. The first column is the macro name defined in esp_wifi_types.h. The common prefix *WIFI_REASON* is removed, which means that *UNSPECIFIED* actually stands for *WIFI_REASON_UNSPECIFIED* and so on. The second column is the value of the reason. The third column is the standard value to which this reason is mapped in section 8.4.1.7 of ieee802.11-2012. (For more information, refer to the standard mentioned above.) The last column is a description of the reason.

+---------------------------+-------+---------+-------------------------------------------------------------+
| Reason code               | ESP32 |Mapped To| Description                                                 |
|                           | value |Standard |                                                             | 
|                           |       |Value    |                                                             | 
+===========================+=======+=========+=============================================================+
| UNSPECIFIED               |   1   |    1    | Generally, it means an internal failure, e.g., the memory   |
|                           |       |         | runs out, the internal TX fails, or the reason is received  |
|                           |       |         | from the remote side, etc.                                  |
+---------------------------+-------+---------+-------------------------------------------------------------+
| AUTH_EXPIRE               |   2   |    2    | The previous authentication is no longer valid.             |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - auth is timed out                                        |
|                           |       |         |  - the reason is received from the soft-AP.                 |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 SoftAP, this reason is reported when:         |
|                           |       |         |                                                             |
|                           |       |         |  - the soft-AP has not received any packets from the station|
|                           |       |         |    in the past five minutes.                                |
|                           |       |         |  - the soft-AP is stopped by calling esp_wifi_stop().       |
|                           |       |         |  - the station is de-authed by calling esp_wifi_deauth_sta()|
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| AUTH_LEAVE                |   3   |    3    | De-authenticated, because the sending STA is                |
|                           |       |         | leaving (or has left).                                      |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| ASSOC_EXPIRE              |   4   |    4    | Disassociated due to inactivity.                            |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Soft-AP, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - the soft-AP has not received any packets from the        |
|                           |       |         |    station in the past five minutes.                        |
|                           |       |         |  - the soft-AP is stopped by calling esp_wifi_stop().       |
|                           |       |         |  - the station is de-authed by calling esp_wifi_deauth_sta()|
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| ASSOC_TOOMANY             |   5   |    5    | Disassociated, because the AP is unable to handle           |
|                           |       |         | all currently associated STAs at the same time.             |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Soft-AP, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - the stations associated with the soft-AP reach the       |
|                           |       |         |    maximum number that the soft-AP can support.             |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| NOT_AUTHED                |   6   |    6    | Class-2 frame received from a non-authenticated STA.        |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Soft-AP, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - the soft-AP receives a packet with data from a           |
|                           |       |         |    non-authenticated station.                               |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| NOT_ASSOCED               |   7   |    7    | Class-3 frame received from a non-associated STA.           |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Soft-AP, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - the soft-AP receives a packet with data from a           |
|                           |       |         |    non-associated station.                                  |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| ASSOC_LEAVE               |   8   |    8    | Disassociated, because the sending STA is leaving (or has   |
|                           |       |         | left) BSS.                                                  |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |  - the station is disconnected by esp_wifi_disconnect() and |
|                           |       |         |    other APIs.                                              |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| ASSOC_NOT_AUTHED          |   9   |    9    | STA requesting (re)association is not authenticated by the  |
|                           |       |         | responding STA.                                             |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Soft-AP, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - the soft-AP receives packets with data from an           |
|                           |       |         |    associated, yet not authenticated, station.              |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| DISASSOC_PWRCAP_BAD       |   10  |    10   | Disassociated, because the information in the Power         |
|                           |       |         | Capability element is unacceptable.                         |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| DISASSOC_SUPCHAN_BAD      |   11  |    11   | Disassociated, because the information in the Supported     |
|                           |       |         | Channels element is unacceptable.                           |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| IE_INVALID                |   13  |    13   | Invalid element, i.e. an element whose content does not meet|
|                           |       |         | the specifications of the Standard in Clause 8.             |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP                          |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Soft-AP, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - the soft-AP parses a wrong WPA or RSN IE.                |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| MIC_FAILURE               |   14  |    14   | Message integrity code (MIC) failure.                       |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| 4WAY_HANDSHAKE_TIMEOUT    |   15  |    15   | Four-way handshake times out. For legacy reasons, in ESP32  |
|                           |       |         | this reason-code is replaced with                           |
|                           |       |         | WIFI_REASON_HANDSHAKE_TIMEOUT.                              |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - the handshake times out                                  |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| GROUP_KEY_UPDATE_TIMEOUT  |   16  |    16   | Group-Key Handshake times out.                              |
|                           |       |         |                                                             | 
|                           |       |         | For the ESP32 station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| IE_IN_4WAY_DIFFERS        |   17  |    17   | The element in the four-way handshake is different from the |
|                           |       |         | (Re-)Association Request/Probe and Response/Beacon frame.   |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |  - the station finds that the four-way handshake IE differs |
|                           |       |         |    from the IE in the (Re-)Association Request/Probe and    |
|                           |       |         |    Response/Beacon frame.                                   |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| GROUP_CIPHER_INVALID      |   18  |    18   | Invalid group cipher.                                       |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| PAIRWISE_CIPHER_INVALID   |   19  |    19   | Invalid pairwise cipher.                                    |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| AKMP_INVALID              |   20  |    20   | Invalid AKMP.                                               |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| UNSUPP_RSN_IE_VERSION     |   21  |    21   | Unsupported RSNE version.                                   |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| INVALID_RSN_IE_CAP        |   22  |    22   | Invalid RSNE capabilities.                                  |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| 802_1X_AUTH_FAILED        |   23  |    23   | IEEE 802.1X. authentication failed.                         |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             |
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 soft-AP, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - 802.1 x authentication fails.                            |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| CIPHER_SUITE_REJECTED     |   24  |    24   | Cipher suite rejected due to security policies.             |
|                           |       |         |                                                             |
|                           |       |         | For the ESP32 Station, this reason is reported when:        |
|                           |       |         |                                                             | 
|                           |       |         |  - it is received from the soft-AP.                         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| BEACON_TIMEOUT            |  200  |reserved | Espressif-specific Wi-Fi reason-code: when the station      |
|                           |       |         | loses N beacons continuously, it will disrupt the connection| 
|                           |       |         | and report this reason.                                     |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| NO_AP_FOUND               |  201  |reserved | Espressif-specific Wi-Fi reason-code: when the station      |
|                           |       |         | fails to scan the target AP, this reason code will be       |
|                           |       |         | reported.                                                   |
+---------------------------+-------+---------+-------------------------------------------------------------+
| AUTH_FAIL                 |  202  |reserved | Espressif-specific Wi-Fi reason-code: the                   |
|                           |       |         | authentication fails, but not because of a timeout.         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+
| ASSOC_FAIL                |  203  |reserved | Espressif-specific Wi-Fi reason-code: the association       |
|                           |       |         | fails, but not because of ASSOC_EXPIRE or ASSOC_TOOMANY.    |
|                           |       |         |                                                             | 
+---------------------------+-------+---------+-------------------------------------------------------------+
| HANDSHAKE_TIMEOUT         |  204  |reserved | Espressif-specific Wi-Fi reason-code: the                   |
|                           |       |         | handshake fails for the same reason as that in              |
|                           |       |         | WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT.                         |
|                           |       |         |                                                             |
+---------------------------+-------+---------+-------------------------------------------------------------+

ESP32 Wi-Fi Configuration
---------------------------

All configurations will be stored into flash when the Wi-Fi NVS is enabled; otherwise, refer to <`Wi-Fi NVS Flash`_>.

Wi-Fi Mode
+++++++++++++++++++++++++
Call esp_wifi_set_mode() to set the Wi-Fi mode.

+------------------+--------------------------------------------------------------+
| Mode             | Description                                                  |
+==================+==============================================================+
| WIFI_MODE_NULL   | NULL mode: in this mode, the internal data struct is not     |
|                  | allocated to the station and the soft-AP, while both the     |
|                  | station and soft-AP interfaces are not initialized for       |
|                  | RX/TX Wi-Fi data. Generally, this mode is used for Sniffer,  |
|                  | or when you only want to stop both the STA and the AP        |
|                  | without calling esp_wifi_deinit() to unload the whole Wi-Fi  | 
|                  | driver.                                                      |
+------------------+--------------------------------------------------------------+
| WIFI_MODE_STA    | Station mode: in this mode, esp_wifi_start() will init the   |
|                  | internal station data, while the station's interface is ready|
|                  | for the RX and TX Wi-Fi data. After esp_wifi_connect() is    |
|                  | called, the STA will connect to the target AP.               |
+------------------+--------------------------------------------------------------+
| WIFI_MODE_AP     | Soft-AP mode: in this mode, esp_wifi_start() will init the   |
|                  | internal soft-AP data, while the soft-AP's interface is ready|
|                  | for RX/TX Wi-Fi data. Then, the Wi-Fi driver starts broad-   |
|                  | casting beacons, and the soft-AP is ready to get connected   |
|                  | to other stations.                                           |
+------------------+--------------------------------------------------------------+
| WIFI_MODE_APSTA  | Station-AP coexistence mode: in this mode, esp_wifi_start()  |
|                  | will simultaneously init both the station and the soft-AP.   |
|                  | This is done in station mode and soft-AP mode. Please note   |
|                  | that the channel of the external AP, which the ESP32 Station |
|                  | is connected to, has higher priority over the ESP32 Soft-AP  |
|                  | channel. Refer to `Wi-Fi Channel Management`_.               |
+------------------+--------------------------------------------------------------+

Station Basic Configuration
+++++++++++++++++++++++++++++++++++++

API esp_wifi_set_config() can be used to configure the station. The table below describes the fields in detail.

+------------------+--------------------------------------------------------------+
| Field            | Description                                                  |
+==================+==============================================================+
| ssid             | This is the SSID of the target AP, to which the station wants|
|                  | to connect to.                                               |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| password         | Password of the target AP                                    |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| bssid_set        | If bssid_set is 0, the station connects to the AP whose SSID |
|                  | is the same as the field "ssid", while the field "bssid"     |
|                  | is ignored. In all other cases, the station connects to      |
|                  | the AP whose SSID is the same as the "ssid" field, while its |
|                  | BSSID is the same the "bssid" field .                        |
+------------------+--------------------------------------------------------------+
| bssid            | This is valid only when bssid_set is 1; see field            |
|                  | "bssid_set".                                                 |
+------------------+--------------------------------------------------------------+
| channel          | If the channel is 0, the station scans the channel 1~N to    |
|                  | search for the target AP; otherwise, the station starts by   |
|                  | scanning the channel whose value is the same as that of the  |
|                  | "channel" field, and then scans others to find the target AP.|
|                  | If you do not know which channel the target AP is running on,|
|                  | set it to 0.                                                 |
+------------------+--------------------------------------------------------------+


Soft-AP Basic Configuration
+++++++++++++++++++++++++++++++++++++

API esp_wifi_set_config() can be used to configure the soft-AP. The table below describes the fields in detail.

+------------------+--------------------------------------------------------------+
| Field            | Description                                                  |
+==================+==============================================================+
| ssid             | SSID of soft-AP; if the ssid[0] is 0xFF and ssid[1] is 0xFF, |
|                  | the soft-AP defaults the SSID to ESP_aabbcc, where "aabbcc"  |
|                  | is the last three bytes of the soft-AP MAC.                  |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| password         | Password of soft-AP; if the auth mode is WIFI_AUTH_OPEN,     |
|                  | this field will be ignored.                                  |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| ssid_len         | Length of SSID; if ssid_len is 0, check the SSID until there |
|                  | is a termination character. If ssid_len > 32, change it to   |
|                  | 32; otherwise, set the SSID length according to ssid_len.    |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| channel          | Channel of soft-AP; if the channel is out of range, the Wi-Fi|
|                  | driver defaults the channel to channel 1. So, please make    |
|                  | sure the channel is within the required range.               |
|                  | For more details, refer to <`Channel Range`_>.               |
+------------------+--------------------------------------------------------------+
| authmode         | Auth mode of ESP32 soft-AP; currently, ESP32 Wi-Fi does not  |
|                  | support AUTH_WEP. If the authmode is an invalid value,       |
|                  | soft-AP defaults the value to WIFI_AUTH_OPEN.                |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| ssid_hidden      | If ssid_hidden is 1, soft-AP does not broadcast the SSID;    |
|                  | otherwise, it does broadcast the SSID.                       |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| max_connection   | Currently, ESP32 Wi-Fi supports up to 10 Wi-Fi connections.  |
|                  | If max_connection > 10, soft-AP defaults the value to 10.    |
|                  |                                                              |
+------------------+--------------------------------------------------------------+
| beacon_interval  | Beacon interval; the value is 100 ~ 60000 ms, with default   |
|                  | value being 100 ms. If the value is out of range,            |
|                  | soft-AP defaults it to 100 ms.                               |
+------------------+--------------------------------------------------------------+

Wi-Fi Protocol Mode
+++++++++++++++++++++++++

Currently, the IDF supports the following protocol modes:

+--------------------+------------------------------------------------------------+
| Protocol Mode      | Description                                                |
+====================+============================================================+
| 802.11 B           | Call esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B) to set  |
|                    | the station/soft-AP to 802.11B-only mode.                  |
|                    |                                                            |
+--------------------+------------------------------------------------------------+
| 802.11 BG          | Call esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_    |
|                    | PROTOCOL_11G) to set the station/soft-AP to 802.11BG mode. |
|                    |                                                            |
+--------------------+------------------------------------------------------------+
| 802.11 BGN         | Call esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|         |
|                    | WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N) to set the station/   |
|                    | soft-AP to BGN mode.                                       |
|                    |                                                            |
+--------------------+------------------------------------------------------------+
| 802.11 BGNLR       | Call esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|         |
|                    | WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR)      |
|                    | to set the station/soft-AP to BGN and the                  |
|                    | Espressif-specific mode.                                   |
+--------------------+------------------------------------------------------------+
| 802.11 LR          | Call esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_LR) to set   |
|                    | the station/soft-AP only to the Espressif-specific mode.   |
|                    |                                                            |
|                    | **This mode is an Espressif-patented mode which can achieve|
|                    | a one-kilometer line of sight range. Please, make sure both|
|                    | the station and the soft-AP are connected to an            |
|                    | ESP32 device**                                             |
+--------------------+------------------------------------------------------------+

Wi-Fi Channel Management
+++++++++++++++++++++++++

Channel Range
*************************

Call esp_wifi_set_country() to set the country code which limits the channel range.

+------------------+---------------------+
| Country          | Channel Range       |
+==================+=====================+
| China            | 1,2,3 ... 14        |
+------------------+---------------------+
| Japan            | 1,2,3 ... 14        |
+------------------+---------------------+
| USA              | 1,2,3 ... 11        |
+------------------+---------------------+
| Europe           | 1,2,3 ... 13        |
+------------------+---------------------+

Home Channel
*************************

In soft-AP mode, the home channel is defined as that of the soft-AP channel. In Station mode, the home channel is defined as the channel of the AP to which the station is connected. In Station+SoftAP mode, the home channel of soft-AP and station must be the same. If the home channels of Station and Soft-AP are different, the station's home channel is always in priority. Take the following as an example: at the beginning, the soft-AP is on channel 6, then the station connects to an AP whose channel is 9. Since the station's home channel has a higher priority, the soft-AP needs to switch its channel from 6 to 9 to make sure that both station and soft-AP have the same home channel.


Wi-Fi Vendor IE Configuration
+++++++++++++++++++++++++++++++++++

By default, all Wi-Fi management frames are processed by the Wi-Fi driver, and the application does not need to care about them. Some applications, however, may have to handle the beacon, probe request, probe response and other management frames. For example, if you insert some vendor-specific IE into the management frames, it is only the management frames which contain this vendor-specific IE that will be processed. In ESP32, esp_wifi_set_vendor_ie() and esp_wifi_set_vendor_ie_cb() are responsible for this kind of tasks.

ESP32 Wi-Fi Power-saving Mode
-----------------------------------
Currently, ESP32 Wi-Fi supports the Modem-sleep mode which refers to WMM (Wi-Fi Multi Media) power-saving mode in the IEEE 802.11 protocol. If the Modem-sleep mode is enabled and the Wi-Fi enters a sleep state, then, RF, PHY and BB are turned off in order to reduce power consumption. Modem-sleep mode works in Station-only mode and the station must be connected to the AP first.

Call esp_wifi_set_ps(WIFI_PS_MODEM) to enable Modem-sleep mode after calling esp_wifi_init(). About 10 seconds after the station connects to the AP, Modem-sleep will start. When the station disconnects from the AP, Modem-sleep will stop.

ESP32 Wi-Fi Connect Crypto
-----------------------------------
Now ESP32 have two group crypto functions can be used when do wifi connect, one is the original functions, the other is optimized by ESP hardware:
1. Original functions which is the source code used in the folder components/wpa_supplicant/src/crypto function;
2. The optimized functions is in the folder components/wpa_supplicant/src/fast_crypto, these function used the hardware crypto to make it faster than origin one, the type of function's name add `fast_` to distinguish with the original one. For example, the API aes_wrap() is used to encrypt frame information when do 4 way handshake, the fast_aes_wrap() has the same result but can be faster.

Two groups of crypto function can be used when register in the wpa_crypto_funcs_t, wpa2_crypto_funcs_t and wps_crypto_funcs_t structure, also we have given the recommend functions to register in the 
fast_crypto_ops.c, you can register the function as the way you need, however what should make action is that the crypto_hash_xxx function and crypto_cipher_xxx function need to register with the same function to operation. For example, if you register crypto_hash_init() function to initialize the esp_crypto_hash structure, you need use the crypto_hash_update() and crypto_hash_finish() function to finish the operation, rather than fast_crypto_hash_update() or fast_crypto_hash_finish(). 

ESP32 Wi-Fi Throughput
-----------------------------------

The table below shows the best throughput results we got in Espressif's lab and in a shield box.

+----------------------+-----------------+-----------------+
| Type/Throughput      | Air In Lab      | Shield-box      |
+======================+=================+=================+
| Raw 802.11 Packet RX |   N/A           | **130 MBit/sec**|
+----------------------+-----------------+-----------------+
| Raw 802.11 Packet TX |   N/A           | **130 MBit/sec**|
+----------------------+-----------------+-----------------+
| UDP RX               |   30 MBit/sec   | 80 MBit/sec     |
+----------------------+-----------------+-----------------+
| UDP TX               |   30 MBit/sec   | 80 MBit/sec     |
+----------------------+-----------------+-----------------+
| TCP RX               |   20 MBit/sec   | 25 MBit/sec     |
+----------------------+-----------------+-----------------+
| TCP TX               |   20 MBit/sec   | 25 MBit/sec     |
+----------------------+-----------------+-----------------+

The throughput result heavily depends on hardware and software configurations, such as CPU frequency, memory configuration, or whether the CPU is running in dual-core mode, etc. The table below shows the configurations with which we got the above-mentioned throughput results. In ESP32 IDF, the default configuration is based on "very conservative" calculations, so if you want to get the best throughput result, the first thing you need to do is to adjust the relevant configurations.

+------------------+---------------------+------------------------------+
| Type             | Value               | How to configure             |
+==================+=====================+==============================+
| CPU Core Mode    | Dual Core           | Menuconfig                   |
+------------------+---------------------+------------------------------+
| CPU Frequency    | 240 MHz             | Menuconfig                   |
+------------------+---------------------+------------------------------+
| Static RX Buffer | 15                  | Menuconfig                   |
+------------------+---------------------+------------------------------+
| Dynamic RX Buffer| Unlimited           | Menuconfig                   |
+------------------+---------------------+------------------------------+
| Dynamic TX Buffer| Unlimited           | Menuconfig                   |
+------------------+---------------------+------------------------------+
| TCP RX Window    | 12*1460 Bytes       | Release 2.1/2.0 and earlier: |
|                  |                     | TCP_WND_DEFAULT in lwipopts.h|
|                  |                     |                              |
|                  |                     | After the 2.1 Release:       |
|                  |                     | Menuconfig                   |
+------------------+---------------------+------------------------------+
| TCP TX Window    | 12*1460 Bytes       | Release 2.1/2.0 and earlier: |
|                  |                     | TCP_SND_BUF_DEFAULT in       |
|                  |                     | lwipopts.h                   |
|                  |                     |                              |
|                  |                     | After the 2.1 Release:       |
|                  |                     | Menuconfig                   |
+------------------+---------------------+------------------------------+
| TCP RX MBOX      | 12                  | Release 2.1/2.0 and earlier: |
|                  |                     | DEFAULT_TCP_RECVMBOX_SIZE in |
|                  |                     | lwipopts.h                   |
|                  |                     |                              |
|                  |                     | After the 2.1 Release:       |
|                  |                     | Menuconfig                   |
+------------------+---------------------+------------------------------+
| RX BA Window     | 9~16                | Release 2.1/2.0 and earlier: |
|                  |                     | not configurable             |
|                  |                     |                              |
|                  |                     | After the 2.1 Release:       |
|                  |                     | Menuconfig                   |
+------------------+---------------------+------------------------------+
| TX BA Window     | 9~16                | Release 2.1/2.0 and earlier: |
|                  |                     | not configurable             |
|                  |                     |                              |
|                  |                     | After the 2.1 Release:       |
|                  |                     | Menuconfig                   |
+------------------+---------------------+------------------------------+

Once you adjust the configurations, you can then run your own test code to test the performance. You can also run the iperf example to test the performance. However, the iperf example is not provided in release 2.1 and earlier ones, but will be so in the upcoming release. Those who really care about the performance should seek support from Espressif directly, so that we can provide them with the iperf version bin for their testing.

If you decide to modify some of the configurations in order to gain better throughput for your application, please consider the memory usage very carefully. For a  more detailed description, refer to <`Wi-Fi Buffer Usage`_> and <`Wi-Fi Buffer Configure`_>.

Wi-Fi 80211 Packet Send
---------------------------

**Important notes: The API esp_wifi_80211_tx is not available in IDF 2.1, but will be so in the upcoming release.**

The esp_wifi_80211_tx API can be used to:

 - Send the beacon, probe request, probe response, action frame.
 - Send the QoS and non-QoS data frame.

It cannot be used for sending cryptographic frames.

Parameters of esp_wifi_80211_tx
+++++++++++++++++++++++++++++++++++++++++++

+-----------------------------+---------------------------------------------------+
| Parameter                   | Description                                       |
+=============================+===================================================+
| ifx                         | Wi-Fi interface ID: if the Wi-Fi mode is Station, |
|                             | the ifx should be WIFI_IF_STA. If the Wi-Fi       |
|                             | mode is SoftAP, the ifx should be WIFI_IF_AP. If  |
|                             | the Wi-Fi mode is Station+SoftAP, the ifx should  |
|                             | be WIFI_IF_STA or WIFI_IF_AP.                     |
|                             | If the ifx is wrong, the API returns              |
|                             | ESP_ERR_WIFI_IF.                                  |
+-----------------------------+---------------------------------------------------+
| buffer                      | Raw 802.11 buffer. For building the correct       |
|                             | buffer, refer to the following sections:          |
|                             |                                                   |
|                             | If the buffer is wrong, or violates the Wi-Fi     |
|                             | driver's restrictions, the API returns            |
|                             | ESP_ERR_WIFI_ARG or results in unexpected         |
|                             | behavior. **Please read the following section     |
|                             | carefully to make sure you understand the         |
|                             | restrictions on encapsulating the buffer.**       |
|                             |                                                   |
+-----------------------------+---------------------------------------------------+
| len                         | The length must be <= 1500; otherwise, the API    |
|                             | will return ESP_ERR_WIFI_ARG.                     |
|                             |                                                   |
+-----------------------------+---------------------------------------------------+
| en_sys_seq                  | If en_sys_seq is true, it means that the Wi-Fi    |
|                             | driver will rewrite the sequence number in the    |
|                             | buffer; otherwise, it will not rewrite the        |
|                             | sequence number.                                  |
|                             | Generally, if esp_wifi_80211_tx is called before  |
|                             | the Wi-Fi connection has been set up, both        |
|                             | en_sys_seq==true and en_sys_seq==false are fine.  |
|                             | However, if the API is called after the Wi-Fi     |
|                             | connection has been set up, en_sys_seq should be  |
|                             | true. For more details, read the following section|
|                             | about the sequence configuration.                 |
+-----------------------------+---------------------------------------------------+

Preconditions of Using esp_wifi_80211_tx
++++++++++++++++++++++++++++++++++++++++++++

 - The Wi-Fi mode is Station, or SoftAP, or Station+SoftAP.
 - Either esp_wifi_set_promiscuous(true), or esp_wifi_start(), or both of these APIs return ESP_ERR_WIFI_OK. This is because we need to make sure that Wi-Fi hardware is initialized before esp_wifi_80211_tx() is called. In ESP32, both esp_wifi_set_promiscuous(true) and esp_wifi_start() can trigger the initialization of Wi-Fi hardware.
 - The parameters of esp_wifi_80211_tx are hereby correctly provided.

Side-Effects to Avoid in Different Scenarios
+++++++++++++++++++++++++++++++++++++++++++++++++++++

Theoretically, if we do not consider the side-effects the API imposes on the Wi-Fi driver or other stations/soft-APs, we can send a raw 802.11 packet over the air, with any destination MAC, any source MAC, any BSSID, or any other type of packet. However,robust/useful applications should avoid such side-effects. The table below provides some tips/recommendations on how to avoid the side-effects of esp_wifi_80211_tx in different scenarios.

+-----------------------------+---------------------------------------------------+
| Scenario                    | Description                                       |
+=============================+===================================================+
| NO_CONN_MGMT                | In this scenario, no Wi-Fi connection is set up,  |
|                             | so there are no side-effects on the Wi-Fi driver. |
|  - No Wi-Fi connection      | If en_sys_seq==true, the Wi-Fi driver is          |
|  - Can send management frame| responsible for the sequence control. If          |
|                             | en_sys_seq==false, the application needs to ensure|
|                             | that the buffer has the correct sequence.         |
|                             |                                                   |
|                             | Theoretically, the MAC address can be any address.|
|                             | However, this may impact other stations/soft-APs  |
|                             | with the same MAC/BSSID.                          |
|                             |                                                   |
|                             | Side-effect example#1                             |
|                             | The application calls esp_wifi_80211_tx to send   | 
|                             | a beacon with BSSID == mac_x in SoftAP mode, but  |
|                             | the mac_x is not the MAC of the SoftAP interface. |
|                             | Moreover, there is another soft-AP, say           |
|                             | "other-AP", whose bssid is mac_x. If this         |
|                             | happens, an "unexpected behavior" may occur,      |
|                             | because the stations which connect to the         |
|                             | "other-AP" cannot figure out whether the beacon is|
|                             | from the "other-AP" or the esp_wifi_80211_tx.     |
|                             |                                                   |
|                             | To avoid the above-mentioned side-effects, we     |
|                             | recommend that:                                   |
|                             |                                                   |
|                             |  - If esp_wifi_80211_tx is called in Station mode,|
|                             |    the first MAC should be a multicast MAC or the |
|                             |    exact target-device's MAC, while the second MAC|
|                             |    should be that of the station interface.       |
|                             |  - If esp_wifi_80211_tx is called in SoftAP mode, |
|                             |    the first MAC should be a multicast MAC or the |
|                             |    exact target-device's MAC, while the second MAC|
|                             |    should be that of the soft-AP interface.       |
|                             |                                                   |
|                             | The recommendations above are only for avoiding   |
|                             | side-effects and can be ignored when there are    |
|                             | good reasons for doing this.                      |
+-----------------------------+---------------------------------------------------+
| NO_CONN_NON_QOS             | Same as in NO_CONN_MGMT                           |
|                             |                                                   |
|  - No Wi-Fi connection      |                                                   |
|  - Can send non-QoS frame   |                                                   |
|                             |                                                   |
+-----------------------------+---------------------------------------------------+
| NO_CONN_QOS                 | Same as in NO_CONN_MGMT                           |
|                             |                                                   |
|  - No Wi-Fi connection      |                                                   |
|  - Can send QoS frame       |                                                   |
|                             |                                                   |
+-----------------------------+---------------------------------------------------+
| CONN_MGMT                   | When the Wi-Fi connection is already set up, and  |
|                             | the sequence is controlled by the application, the|
|  - Have Wi-Fi connection    | latter may impact the sequence control of the     |
|  - Send management frame    | Wi-Fi connection, as a whole. So, the             |
|    only                     | recommendation is that en_sys_seq be true.        |
|                             | The MAC-address recommendations in the            |
|                             | NO_CONN_MGMT scenario also apply to the CONN_MGMT |
|                             | scenario.                                         |
+-----------------------------+---------------------------------------------------+
| CONN_NON_QOS                | Generally, we should use a socket API, instead of |
|                             | this one, in order to send the data frame when    |
|  - Have Wi-Fi connection    | the Wi-Fi connection is already set up.           |
|  - Can send non-QoS frame   |                                                   |
|                             | However, if you have any special reasons for using|
|                             | this particular API, then en_sys_seq must be true;|
|                             | otherwise, you  may impact the internal sequence  |
|                             | control of the Wi-Fi connection described         |
|                             | in CONN_MGMT.                                     |
|                             | The MAC-address recommendations in the            |
|                             | NO_CONN_MGMT scenario also apply to the CONN_MGMT |
|                             | scenario.                                         |
+-----------------------------+---------------------------------------------------+
| CONN_QOS                    | Same as in CONN_NON_QOS                           |
|                             |                                                   |
|  - Have Wi-Fi connection    |                                                   |
|  - Can send non-QoS frame   |                                                   |
|                             |                                                   |
+-----------------------------+---------------------------------------------------+


Wi-Fi Sniffer Mode
---------------------------

The Wi-Fi sniffer mode can be enabled by esp_wifi_set_promiscuous(). If the sniffer mode is enabled, the following packets **can** be dumped to the application:

 - 802.11 Management frame
 - 802.11 Data frame, including MPDU, AMPDU, AMSDU, etc.
 - 802.11 MIMO frame, for MIMO frame, the sniffer only dumps the length of the frame.

The following packets will **NOT** be dumped to the application:

 - 802.11 Control frame
 - 802.11 error frame, such as the frame with a CRC error, etc.

For frames that the sniffer **can** dump, the application can additionally decide which specific type of packets can be filtered to the application by using esp_wifi_set_promiscuous_filter(). By default, it will filter all 802.11 data and management frames to the application.

The Wi-Fi sniffer mode can be enabled in the Wi-Fi mode of WIFI_MODE_NULL, or WIFI_MODE_STA, or WIFI_MODE_AP, or WIFI_MODE_APSTA. In other words, the sniffer mode is active when the station is connected to the soft-AP, or when the soft-AP has a Wi-Fi connection. Please note that the sniffer has a **great impact** on the throughput of the station or soft-AP Wi-Fi connection. Generally, we should **NOT** enable the sniffer, when the station/soft-AP Wi-Fi connection experiences heavy traffic unless we have special reasons.

Another noteworthy issue about the sniffer is the callback wifi_promiscuous_cb_t. The callback will be called directly in the Wi-Fi driver task, so if the application has a lot of work to do for each filtered packet, the recommendation is to post an event to the application task in the callback and defer the real work to the application task.

Wi-Fi Buffer Usage
--------------------------

This section is only about the dynamic buffer configuration.

Why Buffer Configuration Is Important
+++++++++++++++++++++++++++++++++++++++

In order to get a robust, high-performance system, we need to consider the memory usage/configuration very carefully, because:
 - the available memory in ESP32 is limited.
 - currently, the default type of buffer in LwIP and Wi-Fi drivers is "dynamic", **which means that both the LwIP and Wi-Fi share memory with the application**. Programmers should always keep this in mind; otherwise, they will face a memory issue, such as "running out of heap memory".
 - it is very dangerous to run out of heap memory, as this will cause ESP32 an "undefined behavior". Thus, enough heap memory should be reserved for the application, so that it never runs out of it.
 - the Wi-Fi throughput heavily depends on memory-related configurations, such as the TCP window size, Wi-Fi RX/TX dynamic buffer number, etc. Refer to <`ESP32 Wi-Fi Throughput`_>.
 - the peak heap memory that the ESP32 LwIP/Wi-Fi may consume depends on a number of factors, such as the maximum TCP/UDP connections that the application may have, etc.
 - the total memory that the application requires is also an important factor when considering memory configuration.

Due to these reasons, there is not a good-for-all application configuration. Rather, we have to consider memory configurations separately for every different application.

Dynamic vs. Static Buffer
++++++++++++++++++++++++++++++

The default type of buffer in LwIP and Wi-Fi drivers is "dynamic". Most of the time the dynamic buffer can significantly save memory. However, it makes the application programming a little more difficult, because in this case the application needs to consider memory usage in LwIP/Wi-Fi.


Peak LwIP Dynamic Buffer
++++++++++++++++++++++++++++++

The default type of LwIP buffer is "dynamic", and this section considers the dynamic buffer only.
The peak heap memory that LwIP consumes is the **theoretically-maximum memory** that the LwIP driver consumes. Generally, the peak heap memory that the LwIP consumes depends on:

 - the memory required to create a UDP connection: lwip_udp_conn
 - the memory required to create a TCP connection: lwip_tcp_conn
 - the number of UDP connections that the application has: lwip_udp_con_num
 - the number of TCP connections that the application has: lwip_tcp_con_num
 - the TCP TX window size: lwip_tcp_tx_win_size
 - the TCP RX window size: lwip_tcp_rx_win_size

**So, the peak heap memory that the LwIP consumes can be calculated with the following formula:**
  lwip_dynamic_peek_memory =  (lwip_udp_con_num * lwip_udp_conn)  + (lwip_tcp_con_num * (lwip_tcp_tx_win_size + lwip_tcp_rx_win_size + lwip_tcp_conn))

Some TCP-based applications need only one TCP connection. However, they may choose to close this TCP connection and create a new one when an error (such as a sending failure) occurs. This may result in multiple TCP connections existing in the system simultaneously, because it may take a long time for a TCP connection to close, according to the TCP state machine (refer to RFC793).

Peak Wi-Fi Dynamic Buffer
++++++++++++++++++++++++++++++

The Wi-Fi driver supports several types of buffer (refer to `Wi-Fi Buffer Configure`_). However, this section is about the usage of the dynamic Wi-Fi buffer only.
The peak heap memory that Wi-Fi consumes is the **theoretically-maximum memory** that the Wi-Fi driver consumes. Generally, the peak memory depends on:

 - the number of dynamic rx buffers that are configured: wifi_rx_dynamic_buf_num
 - the number of dynamic tx buffers that are configured: wifi_tx_dynamic_buf_num
 - the maximum packet size that the Wi-Fi driver can receive: wifi_rx_pkt_size_max
 - the maximum packet size that the Wi-Fi driver can send: wifi_tx_pkt_size_max

So, the peak memory that the Wi-Fi driver consumes can be calculated with the following formula:
  wifi_dynamic_peek_memory = (wifi_rx_dynamic_buf_num * wifi_rx_pkt_size_max) + (wifi_tx_dynamic_buf_num * wifi_tx_pkt_size_max) 
  
Generally, we do not need to care about the dynamic tx long buffers and dynamic tx long long buffers, because they are management frames which only have a small impact on the system. 




Wi-Fi Menuconfig
-----------------------

Wi-Fi Buffer Configure
+++++++++++++++++++++++

If you are going to modify the default number or type of buffer, it would be helpful to also have an overview of how the buffer is allocated/freed in the data path. The following diagram shows this process in the TX direction::

     -------------              -------------              -------------
    | Application |            |    LwIP     |            |    Wi-Fi    |
    |    task     | ---------> |    task     | ---------> |    task     |
     -------------              -------------              -------------
      1> User data               2> Pbuf                    3> Dynamic (Static) TX Buffer

Description:
 - The application allocates the data which needs to be sent out.
 - The application calls TCPIP-/Socket-related APIs to send the user data. These APIs will allocate a PBUF used in LwIP, and make a copy of the user data.
 - When LwIP calls a Wi-Fi API to send the PBUF, the Wi-Fi API will allocate a "Dynamic Tx Buffer" or "Static Tx Buffer", make a copy of the LwIP PBUF, and finally send the data.

The following diagram shows how buffer is allocated/freed in the RX direction::

     -------------              -------------              -------------              ------------- 
    | Application |            |    LwIP     |            |    Wi-Fi    |            |    Wi-Fi    |
    |    Task     | <--------- |    task     | <--------- |    task     | <--------- |  Interrupt  |
     -------------              -------------              -------------              -------------
      4> User data               3> Pbuf                    2> Dynamic RX Buffer       1> Static RX Buffer

Description:

 - The  Wi-Fi hardware receives a packet over the air and puts the packet content to the "Static Rx Buffer", which is also called "RX DMA Buffer".
 - The Wi-Fi driver allocates a "Dynamic Rx Buffer", makes a copy of the "Static Rx Buffer", and returns the "Static Rx Buffer" to hardware.
 - The Wi-Fi driver delivers the packet to the upper-layer (LwIP), and allocates a PBUF for holding the "Dynamic Rx Buffer".
 - The application receives data from LwIP.

The diagram shows the configuration of the Wi-Fi internal buffer.

+------------------+------------+------------+--------------+---------------------------------------+
| Buffer Type      | Alloc Type | Default    | Configurable | Description                           |
+==================+============+============+==============+=======================================+
| Static RX Buffer | Static     | 10  *      | Yes          | This is a kind of DMA memory. It is   |
| (Hardware RX     |            | 1600 Bytes |              | initialized in esp_wifi_init() and    |
| Buffer)          |            |            |              | freed in esp_wifi_deinit(). The       |
|                  |            |            |              | 'Static Rx Buffer' forms the hardware |
|                  |            |            |              | receiving list. Upon receiving a frame|
|                  |            |            |              | over the air, hardware writes the     |
|                  |            |            |              | frame into the buffer and raises an   |
|                  |            |            |              | interrupt to the CPU. Then, the Wi-Fi |
|                  |            |            |              | driver reads the content from the     |
|                  |            |            |              | buffer and returns the buffer back to |
|                  |            |            |              | the list.                             |
+------------------+------------+------------+--------------+---------------------------------------+
| Dynamic RX Buffer| Dynamic    | 32         | Yes          | The buffer length is variable and it  |
|                  |            |            |              | depends on the received frames'       |
|                  |            |            |              | length. When the Wi-Fi driver receives| 
|                  |            |            |              | a frame from the 'Hardware Rx Buffer',|
|                  |            |            |              | the 'Dynamic Rx Buffer' needs to be   |
|                  |            |            |              | allocated from the heap. The number of|
|                  |            |            |              | the Dynamic Rx Buffer, configured in  |
|                  |            |            |              | the menuconfig, is used to limit the  |
|                  |            |            |              | total un-freed Dynamic Rx Buffer      |
|                  |            |            |              | number.                               |
+------------------+------------+------------+--------------+---------------------------------------+
| Dynamic TX Buffer| Dynamic    | 32         | Yes          | This is a kind of DMA memory. It is   |
|                  |            |            |              | allocated to the heap. When the upper-|
|                  |            |            |              | layer (LwIP) sends packets to the     |
|                  |            |            |              | Wi-Fi driver, it firstly allocates a  |
|                  |            |            |              | 'Dynamic TX Buffer' and makes a copy  |
|                  |            |            |              | of the upper-layer buffer.            |
|                  |            |            |              |                                       |
|                  |            |            |              | The Dynamic and Static TX Buffers are |
|                  |            |            |              | mutually exclusive.                   |
+------------------+------------+------------+--------------+---------------------------------------+
| Static TX Buffer | Static     | 32 *       | Yes          | This is a kind of DMA memory. It is   |
|                  |            | 1600Bytes  |              | initialized in esp_wifi_init() and    |
|                  |            |            |              | freed in esp_wifi_deinit().           |
|                  |            |            |              | When the upper-layer (LwIP) sends     |
|                  |            |            |              | packets to the Wi-Fi driver, it       | 
|                  |            |            |              | firstly allocates a 'Static TX Buffer'|
|                  |            |            |              | and makes a copy of the upper-layer   |
|                  |            |            |              | buffer.                               |
|                  |            |            |              | The Dynamic and Static TX Buffer are  |
|                  |            |            |              | mutually exclusive.                   |
+------------------+------------+------------+--------------+---------------------------------------+
| Management Short | Static     | 16         | NO           | Wi-Fi driver's internal buffer        |
|      Buffer      |            |            |              |                                       |
+------------------+------------+------------+--------------+---------------------------------------+
| Management Long  | Dynamic    | 32         | NO           | Wi-Fi driver's internal buffer        |
|      Buffer      |            |            |              |                                       |
+------------------+------------+------------+--------------+---------------------------------------+
| Mgmt Long Long   | Dynamic    | 32         | NO           | Wi-Fi driver's internal buffer        |
|      Buffer      |            |            |              |                                       |
+------------------+------------+------------+--------------+---------------------------------------+



Wi-Fi NVS Flash
+++++++++++++++++++++
If the Wi-Fi NVS flash is enabled, all Wi-Fi configurations set via the Wi-Fi APIs will be stored into flash, and the Wi-Fi driver will start up with these configurations next time it powers on/reboots. However, the application can choose to disable the Wi-Fi NVS flash if it does not need to store the configurations into persistent memory, or has its own persistent storage, or simply due to debugging reasons, etc.

Wi-Fi AMPDU
+++++++++++++++++++++
Generally, the AMPDU should be enabled, because it can greatly improve the Wi-Fi throughput. Disabling AMPDU is usually for debugging purposes. It may be removed from future releases.
