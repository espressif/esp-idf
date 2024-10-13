**Introduction**

The advanced Wi-Fi roaming app has been written with the intention of simplifying the process of developing applications that will function in a network environment that allows for roaming between the service areas of multiple compatible APs.  It gathers basic approaches about how different roaming mechanisms and APIs can be integrated for an efficient solution to roaming and bundles into an easy to user yet highly configurable module.

**How to use:**

To enable the roaming app in the menuconfig, please navigate to Component Settings --> Wi-Fi and then enable “Advanced support for Wi-Fi Roaming”


**Configuring the advanced Wi-Fi roaming app :**

After enabling the roaming app in the menuconfig, this roaming app can be configured to best suit your application requirements and the network environment. The configurations are classified into Roaming Triggers (better understood as “Under what conditions to roam?”), Roaming methods (better understood as “How to Roam’), and then some additional configurations such as scanning parameters, backoff times, periodic neighbor report requests etc.


**Roaming Triggers: (Roaming Module Settings --> Roaming Triggers)**

There are broadly two different Roaming triggers you can choose from:

1) Low RSSI triggered roaming :

If enabled, in this method the roaming app sets a Wi-Fi Threshold (configured by “Wi-Fi RSSI threshold to trigger roaming”), which when reached by the connection to the current AP, triggers a check for a better AP and if found will trigger roaming.

Every time the threshold is reached, a new threshold needs to be set at an even lower rssi threshold since  if we fail to find a better AP the first time the RSSI threshold is reached there will be no further attempts to find better APs leading to possible disconnection as the only avenue for finding a better AP. The offset by which the next RSSI threshold can be set is decided by “Wi-Fi RSSI threshold to trigger roaming” which defaults to 5.

Also please note that if the AP we connect to, upon connecting itself has a worse RSSI than the threshold set in the configuration, the new RSSI threshold is set to the current RSSI - offset. Additionally, the RSSI threshold gets reset to the configured value if the RSSI ever goes below the configured threshold by the offset amount.

2) Periodic Scan based roaming:

Unlike Low RSSI triggered roaming, which is a bit reactive to the changing network environment, periodic scanning-based roaming (enabled by “Conduct periodic scans to check if a better AP is available”) allows for a more active approach to ensuring that you are connected to the best AP in the network. You can also decide the threshold after which you want to conduct the periodic scans with a default of –20dbm. (Configured by “Threshold at which to begin periodic scanning for a better AP”)

The intervals at which this periodic scan will take place can also be configured through “Time intervals at which station will initiate a scan”. This defaults to 30 seconds.The RSSI difference between a candidate AP and the current AP, which can be considered as acceptable to initiate roaming can also be configured as the “RSSI difference b/w current AP and a candidate AP to initiate roaming”

**Please note that at least one of the two Roaming Triggers needs to be enabled.**


**Roaming Methods**

Currently 2 methods of roaming are supported by the roaming app. (Roaming App Settings --> Roaming Methods)

1) Network Assisted Roaming:

Enabled by “Support Network Assisted Roaming using 802.11kv”, this method primarily uses the BSS transition Management mechanisms outlined in IEEE 802.11v. It uses Candidates received from neighbor report requests (if enabled, explained later.) and scanning results to Send BSS transition Management Queries to the AP it is currently associated to. Depending on the current radio environment and vendor implementation on the side of the AP, this could then lead to BSS Transition Management Requests and corresponding BSS Transition Management responses which could lead to a seamless transition from one AP to another. For a better understanding of the mechanisms involved and the general implementation please look up the IEEE 802.11v specification and upstream wpa_supplicant’s implementation.

Please note that for this to work as expected, the APs should support 802.11k & 802.11v and be setup in a network where they are aware of each other.

2) Legacy Roaming approach.

Enabled by “Support Legacy roaming approach”, this is designed for scenarios where there is no support for 802.11v amongst the APs of the network. In this scenario, upon determining the better AP to connect to, the station forcibly will disconnect and attempt to connect to the AP with the better AP.

Please note that if both approaches are enabled, there is an additional configuration (“Retry count after which to switch to legacy roaming”) that allows developers to decide how many times should the station attempt to roam using the Network Assisted roaming methods before switching to using the legacy approach.

**Please note that at least one of the two roaming methods needs to be selected.**


**Scan configuration**

The scan configuration allows for configuring the parameters for the scans that are conducted in various scenarios during the functioning of the roaming app. Please note that all scans conducted by the roaming app are active scans.

The minimum and maximum active scanning duration for each channel in milliseconds can be configured through “Minimum duration of active scan time for a station” & “Maximum duration of active scan time for a station”. If connected, the “Home channel dwell time between scanning consecutive channels” configuration decides for how long the station will return to the home channel for Tx of various frames in the buffer, and Rx of frames buffered by the AP.

Additionally, if channels of operation of the APs that the application designed will work with is known, you can provide this information as an ordered list of comma-separated channels. (configured using “Preferred channel list for scanning”) (for e.g. 1,6,9,11) Only those channels will be scanned in the order mentioned. Keeping in mind that network discovery/scanning is the process that takes up most of the time in roaming as well connecting to an AP, providing such a list could significantly increase the efficiency of the roaming app and process.

This module can trigger scans due to several reasons. The configuration “Scan results expiry window” decides the duration for which different modules will use the most recent scan results instead of triggering a new scan of their own.


**Backoff Time:**

Successive roaming attempts by multiple roaming triggers simultaneously could lead to performance degradation, hence a backoff time can be configured (“Default time to wait between subsequent roaming attempts”). A roaming attempt can be made using any method only once the configured backoff time has passed since the last roaming attempt.


**Periodic Neighbor Report Requests:**

Neighbor Report requests are a part of the IEEE 802.11k specification, and hence the intended network would need to support and be setup in a way to support its mechanisms. Periodic neighbor report requests provide vital information about the network and other APs in the vicinity that are candidate APs of the same network. This can be enabled using “Send Periodic Neighbor Report requests for updating the internal list”. The frequency of these requests is controlled by “Time interval between Periodic Neighbor report Requests”. There can also be a RSSI threshold (“Threshold for sending periodic neighbor report requests”) after which you wish to consider sending these requests however this is set by default to -20.


**Notes :**

1) Advanced roaming support is disabled by default.

2) When enabling the advanced roaming support , it is expected that the bssid to connect to is not specifically set by the application. This would defeat the purpose of roaming between different APs of the network. Hence if the BSSID is set (in  wifi_config_t.sta) , it will be unset at the first disconnection/connection.

3) For roaming to work as expected, the APs between which the station is expected to roam must have the same or compatible authmode.  These include :

   Open <--> OWE

   PSK based authmodes  <--> PSK based authmodes (Does not include WEP)

   Enterprise <--> Enterprise.

   Also note that this module would also prevent roaming to an AP that does not clear the authmode threshold set by wifi_config_t.sta.

4) As a general guideline for configuring the values of the roaming app it would be helpful to understand the trade-off between roaming and general performance of the overall application. Roaming includes processes such as scanning, disconnecting, connecting, etc which take up time away from other tasks of application. Deciding the several configurations in this module needs to be done while prioritising between being connected to the best AP, seamless transfer between APs and performance based on the requirements of the end application. For example, setting the thresholds of processes outlined above to a lower value or reducing the intervals between the aforementioned periodic tasks would make the roaming more aggressive while interrupting the general operation of the whole application more frequently.
