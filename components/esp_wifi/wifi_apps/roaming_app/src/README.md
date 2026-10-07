**Introduction**

The advanced Wi-Fi roaming app has been written with the intention of simplifying the process of developing applications that will function in a network environment that allows for roaming between the service areas of multiple compatible APs. It gathers basic approaches about how different roaming mechanisms and APIs can be integrated for an efficient solution to roaming and bundles into an easy to use yet highly configurable module.

For runtime flow and configuration details, see [ROAMING_FLOW.md](ROAMING_FLOW.md).

**How to use:**

To enable the roaming app in the menuconfig, please navigate to Component Settings --> Wi-Fi and then enable “Advanced support for Wi-Fi Roaming”


**Configuring the advanced Wi-Fi roaming app :**

After enabling the roaming app in the menuconfig, this roaming app can be configured to best suit your application requirements and the network environment. The configurations are classified into Roaming Triggers (better understood as “Under what conditions to roam?”), Roaming methods (better understood as “How to roam?”), and then some additional configurations such as scanning parameters, backoff times, periodic neighbor report requests, blacklisting, etc.


**Roaming Triggers: (Roaming Module Settings --> Roaming Triggers)**

There are broadly two different roaming triggers you can choose from:

1) Low RSSI triggered roaming :

If enabled, in this method the roaming app sets a Wi-Fi Threshold (configured by “Wi-Fi RSSI threshold to trigger roaming”), which when reached by the connection to the current AP, calls the common roam method decision (`decide_and_roam`: 11v → 11k → legacy) without an upfront discovery scan. A full discovery scan is started only if legacy is selected and no candidate is already known.

Every time the threshold is reached, a new threshold needs to be set at an even lower RSSI threshold since if we fail to find a better AP the first time the RSSI threshold is reached there will be no further attempts to find better APs, leading to possible disconnection as the only avenue for finding a better AP. The offset by which the next RSSI threshold can be set is decided by “Offset by which to reset the RSSI Threshold after attempt to roam” which defaults to 5.

Also please note that if the AP we connect to, upon connecting, itself has a worse RSSI than the threshold set in the configuration, the new RSSI threshold is set to the current RSSI - offset. Additionally, the RSSI threshold gets reset to the configured value if the RSSI later improves above the configured threshold. The periodic connected-AP RSSI log only reports RSSI/BSSID/channel and does not change the low-RSSI trigger.

2) Periodic Scan based roaming:

Unlike Low RSSI triggered roaming, which is a bit reactive to the changing network environment, periodic monitoring (enabled by “Conduct periodic scans to check if a better AP is available”) allows for a more active approach. When the connected RSSI is below the configured threshold, the app first runs a discovery scan. Only if a better AP is found does it call the same common `decide_and_roam` decision (11v → 11k → legacy), with that candidate available for preferred-candidate BTM and for legacy directed reconnect (no second scan). You can also decide the threshold after which you want to begin this monitoring with a default of -50 dBm. (Configured by “Threshold at which to begin periodic scanning for a better AP”)

The intervals at which this periodic check will take place can also be configured through “Time intervals at which station will initiate a scan”. This defaults to 30 seconds. The RSSI difference between a candidate AP and the current AP, which can be considered as acceptable to treat an AP as better after a discovery scan, can also be configured as the “RSSI difference b/w current AP and a candidate AP to initiate roaming”

**Please note that at least one of the two Roaming Triggers needs to be enabled.**


**Roaming Methods**

Currently 3 methods of roaming are supported by the roaming app. (Roaming App Settings --> Roaming Methods)

Priority while connected: **802.11v BTM > 802.11k assisted > legacy**.

Both triggers share `decide_and_roam` for method selection. Low RSSI enters that decision directly. Periodic monitoring runs a discovery scan first and only then enters `decide_and_roam` if a better AP was found. **802.11v and 802.11k do not require a full discovery scan on the low-RSSI path.** Legacy directed-reconnects when a candidate is already known; otherwise it runs a discovery scan. Fallback is only to methods that are still enabled; if none remain usable, roaming triggers are suppressed until the next association.

1) Network Assisted Roaming:

Enabled by “Support Network Assisted Roaming using 802.11v”, this method primarily uses the BSS transition Management mechanisms outlined in IEEE 802.11v. It sends BSS Transition Management Queries using the cached neighbor list from periodic Neighbor Report requests (if enabled). Depending on the current radio environment and vendor implementation on the side of the AP, this could then lead to BSS Transition Management Requests and corresponding BSS Transition Management responses which could lead to a seamless transition from one AP to another. For a better understanding of the mechanisms involved and the general implementation please look up the IEEE 802.11v specification and upstream wpa_supplicant’s implementation.

Please note that for this to work as expected, the APs should be setup in a network where they are aware of each other. If the network is expected to preserve IP configuration across a roaming event, `Skip IP renew after roaming` can also be enabled. This is relevant only when the roam keeps the same IP state, such as 802.11r or BTM-based roaming.

2) 802.11k assisted roaming:

Enabled by “Support 802.11k assisted roaming using neighbor reports”. When selected, the station uses Neighbor Report information to scan only AP-advertised channels and look for a better AP. On success it uses a directed reconnect. If NR channels are already cached (for example from periodic RRM) and still within the configured neighbor-report channel cache expiry window, the on-demand Neighbor Report Request can be skipped; after that window the cache is treated as stale and a fresh report is required (same idea as the scan results expiry window). Neighbor Report Request retries are configured separately from BTM retries. On 11k failure the station retries NR while attempts remain; only after retries are exhausted does it fall through to legacy (if enabled).

3) Legacy Roaming approach.

Enabled by “Support Legacy roaming approach”, this is designed for scenarios where there is no support for 802.11v/802.11k amongst the APs of the network, or after higher-priority methods have been exhausted. If a better AP candidate is already known (for example from a periodic discovery scan), the station directed-reconnects to it without scanning again. Otherwise it runs a discovery scan, and upon determining a better AP forcibly disconnects and attempts to connect to that AP.

Please note that if BTM is enabled together with 11k and/or legacy, “Retry count after which to switch to 11k/legacy roaming” controls how many BTM attempts are made before falling through. 11k has its own independent retry count for Neighbor Report Requests.

**Please note that at least one of the roaming methods needs to be selected.**


**Scan configuration**

The scan configuration allows for configuring the parameters for the scans that are conducted in various scenarios during the functioning of the roaming app. Please note that all scans conducted by the roaming app are active scans.

The minimum and maximum active scanning duration for each channel in milliseconds can be configured through “Minimum duration of active scan time for a station” & “Maximum duration of active scan time for a station”. If connected, the “Home channel dwell time between scanning consecutive channels” configuration decides for how long the station will return to the home channel for Tx of various frames in the buffer, and Rx of frames buffered by the AP.

Additionally, if channels of operation of the APs that the application is designed to work with are known, you can provide this information as a comma-separated list of channels. (configured using “Preferred channel list for scanning”) (for e.g. 1,6,9,11) Only those channels will be scanned, but the list is treated as a filter and not as an ordered scan sequence. Keeping in mind that network discovery/scanning is the process that takes up most of the time in roaming as well as connecting to an AP, providing such a list could significantly increase the efficiency of the roaming app and process.

This module can trigger scans due to several reasons. The configuration “Scan results expiry window” decides the duration for which different modules will use the most recent scan results instead of triggering a new scan of their own.


**Backoff Time:**

Successive roaming attempts by multiple roaming triggers simultaneously could lead to performance degradation, hence a backoff time can be configured (“Default time to wait between subsequent roaming attempts”). A roaming attempt can be made using any method only once the configured backoff time has passed since the last roaming attempt.


**Periodic Neighbor Report Requests:**

Neighbor Report requests are a part of the IEEE 802.11k specification, and hence the intended network would need to support and be setup in a way to support its mechanisms. Periodic neighbor report requests provide vital information about the network and other APs in the vicinity that are candidate APs of the same network. This can be enabled using “Send Periodic Neighbor Report requests for updating the internal list” when 802.11v and/or 802.11k roaming is enabled. The cached list is used by BTM queries and to warm the 11k NR channel cache. The frequency of these requests is controlled by “Time interval between Periodic Neighbor report Requests”. There can also be a RSSI threshold (“Threshold for sending periodic neighbor report requests”) after which you wish to consider sending these requests however this is set by default to -20.


**Blacklist and security behavior:**

`Enable BSSID blacklisting` allows the app to skip specific BSSIDs during candidate selection. Entries can be added manually through `esp_wifi_blacklist_add()` and `esp_wifi_blacklist_remove()`.

If `Enable automatic BSSID blacklisting` is enabled, a BSSID is blacklisted after repeated connection or authentication failures. Blacklisted entries are removed again after `Blacklist timeout (in seconds)`.

The roaming logic also honors AP-advertised transition disable policy while selecting candidates and prevents roaming to candidates that do not meet the authmode threshold set by `wifi_config_t.sta`.


**Notes :**

1) Advanced roaming support is disabled by default.

2) When enabling the advanced roaming support, it is expected that the BSSID to connect to is not specifically set by the application. This would defeat the purpose of roaming between different APs of the network. The roaming app may temporarily set BSSID/channel hints for a directed reconnect, but these hints are cleared again so normal roaming can continue.

3) For roaming to work as expected, the APs between which the station is expected to roam must have the same or compatible authmode. These include :

   Open <--> OWE

   PSK based authmodes <--> PSK based authmodes (Does not include WEP)

   `WAPI_PSK` <--> legacy WPA/WPA2 PSK authmodes

   Enterprise <--> Enterprise.

   Also note that this module would also prevent roaming to an AP that does not clear the authmode threshold set by wifi_config_t.sta.

4) As a general guideline for configuring the values of the roaming app it would be helpful to understand the trade-off between roaming and general performance of the overall application. Roaming includes processes such as scanning, disconnecting, connecting, etc which take up time away from other tasks of application. Deciding the several configurations in this module needs to be done while prioritising between being connected to the best AP, seamless transfer between APs and performance based on the requirements of the end application. For example, setting the thresholds of processes outlined above to a lower value or reducing the intervals between the aforementioned periodic tasks would make the roaming more aggressive while interrupting the general operation of the whole application more frequently.
