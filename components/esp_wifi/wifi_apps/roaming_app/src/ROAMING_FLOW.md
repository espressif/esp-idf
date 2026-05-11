# Roaming App Flow

## Scope

This document describes the runtime control flow of the advanced Wi-Fi roaming app and identifies the configuration options that influence each stage. Each diagram focuses on one stage of the app, and the table below it lists the related configuration options.

## Terminology

- `AP`: Access Point.
- `BSSID`: MAC address of one AP radio.
- `Candidate AP`: A scanned AP that is still under consideration for roaming.
- `BTM`: Network-assisted roaming using IEEE 802.11v BSS Transition Management.
- `RRM / Neighbor Report`: Information about neighboring APs obtained using IEEE 802.11k.
- `Legacy roaming`: A directed reconnect initiated by the roaming app.
- `Blacklist`: A temporary deny-list of BSSIDs that the app should avoid.

## Initialization Preconditions

The roaming app starts only if both of the following are true:

- At least one roaming trigger is enabled.
- At least one roaming method is enabled.

If either condition is not satisfied, `roam_init_app()` returns without starting roaming support.

## 1. High-Level Flow

The following diagram summarizes the complete roaming flow before the later sections break it down by stage.

```mermaid
flowchart TD
    A["roam_init_app()"] --> B{"Triggers and methods enabled?"}
    B -- "No" --> X["Return without starting roaming support"]
    B -- "Yes" --> C["Wait for station connection"]
    C --> D["On connection, apply roaming configuration<br/>and start enabled monitors"]

    D --> E{"Runtime event"}
    E -- "Low RSSI or periodic scan" --> F["Find the best candidate AP"]
    E -- "Periodic RRM timer" --> G["Refresh cached neighbor list for BTM"]
    G --> E
    E -- "Disconnect event" --> H{"Reconnect allowed?"}
    H -- "No" --> I["No action"]
    H -- "Yes" --> J{"After disconnect handling,<br/>is the disconnected BSSID blacklisted now?"}
    J -- "No" --> K["Use default reconnect path"]
    J -- "Yes" --> F

    F --> L{"Reuse scan cache?"}
    L -- "Yes" --> M["Evaluate scanned APs"]
    L -- "No" --> N["Run a new scan and evaluate APs"]
    M --> O{"Candidate selected?"}
    N --> O

    O -- "No, still connected" --> P["Stay on current AP"]
    O -- "No, disconnected recovery" --> Q["Schedule retry with a fresh scan"]
    O -- "Yes" --> R{"How to roam?"}

    R -- "BTM query can be used" --> S["Send BTM query and wait for AP or network action"]
    R -- "Directed reconnect is used" --> T["Start reconnect to the selected AP"]
    R -- "No method available" --> U["No roam action"]

    P --> E
    Q --> E
    S --> E
    T --> E
    K --> E
```

## 2. Initialization and Monitoring

```mermaid
flowchart TD
    A["roam_init_app()"] --> B{"At least one trigger enabled?"}
    B -- "No" --> X["Return without starting roaming support"]
    B -- "Yes" --> C{"At least one roaming method enabled?"}
    C -- "No" --> X
    C -- "Yes" --> D["Load default configuration"]
    D --> E["Register Wi-Fi event handlers"]
    E --> F["Wait for station connection"]

    F --> G["On connection: read AP state,<br/>refresh scan filters, apply runtime config,<br/>and arm enabled monitors"]

    G --> H{"Runtime event"}
    H -- "Low RSSI" --> I["Candidate discovery"]
    H -- "Periodic scan timer" --> I
    H -- "Periodic RRM timer" --> J["Send neighbor report request"]
    J --> K["Update cached BTM neighbor list"]
    K --> H
    H -- "Disconnect event" --> L{"Reconnect allowed?"}
    L -- "No" --> M["No action"]
    L -- "Yes" --> N{"After disconnect handling,<br/>is the disconnected BSSID blacklisted now?"}
    N -- "Yes" --> I
    N -- "No" --> O["Default reconnect"]
```

### Configuration used in this stage

| Area | Configuration |
| --- | --- |
| Low RSSI trigger | `ESP_WIFI_ROAMING_LOW_RSSI_ROAMING`, `ESP_WIFI_ROAMING_LOW_RSSI_THRESHOLD`, `ESP_WIFI_ROAMING_LOW_RSSI_OFFSET` |
| Periodic scan trigger | `ESP_WIFI_ROAMING_PERIODIC_SCAN_MONITOR`, `ESP_WIFI_ROAMING_PERIODIC_SCAN_THRESHOLD`, `ESP_WIFI_ROAMING_SCAN_MONITOR_INTERVAL`, `ESP_WIFI_ROAMING_SCAN_ROAM_RSSI_DIFF` |
| Periodic RRM | `ESP_WIFI_ROAMING_PERIODIC_RRM_MONITORING`, `ESP_WIFI_ROAMING_RRM_MONITOR_TIME`, `ESP_WIFI_ROAMING_RRM_MONITOR_THRESHOLD` |
| Blacklist handling | `ESP_WIFI_ROAMING_BSSID_BLACKLIST`, `ESP_WIFI_ROAMING_AUTO_BLACKLISTING`, `ESP_WIFI_ROAMING_MAX_CONN_FAILURES`, `ESP_WIFI_ROAMING_BLACKLIST_TIMEOUT` |
| Runtime overrides | `roam_set_config_params()`, `scan_filter_ssid`, `scan_filter_bssid` |

## 3. Candidate Discovery and Selection

```mermaid
flowchart TD
    A["determine_best_ap(threshold)"] --> A1{"Can the app act on a roam candidate now?"}
    A1 -- "No" --> O["Stay on current AP"]
    A1 -- "Yes" --> B{"Recent scan cache still valid?"}
    B -- "Yes" --> C["Reuse cached scan results"]
    B -- "No" --> D["Run a new scan"]
    C --> E["Evaluate scanned APs"]
    D --> E

    E --> F{"Station connected?"}
    F -- "Yes" --> G["Set baseline RSSI from current connection state"]
    F -- "No" --> H["Use disconnected recovery rules"]
    G --> I["Skip blacklisted BSSIDs"]
    H --> I

    I --> J["Apply security policy"]
    J --> K["Compare RSSI delta with threshold"]
    K --> L{"Candidate selected?"}

    L -- "Yes" --> M{"Still able to act on this candidate?"}
    M -- "Yes" --> Q["Post roam event"]
    M -- "No" --> O
    L -- "No" --> N{"Station connected?"}
    N -- "Yes" --> O["Stay on current AP"]
    N -- "No" --> P["Schedule retry and invalidate scan cache"]
```

### Configuration used in this stage

| Area | Configuration |
| --- | --- |
| Scan timing | `ESP_WIFI_ROAMING_SCAN_MIN_SCAN_TIME`, `ESP_WIFI_ROAMING_SCAN_MAX_SCAN_TIME`, `ESP_WIFI_ROAMING_HOME_CHANNEL_DWELL_TIME` |
| Scan filter and scope | `ESP_WIFI_ROAMING_SCAN_CHAN_LIST`, `scan_filter_ssid`, `scan_filter_bssid` |
| Scan cache reuse | `ESP_WIFI_ROAMING_SCAN_EXPIRY_WINDOW` |
| Blacklist filter | `ESP_WIFI_ROAMING_BSSID_BLACKLIST`, `ESP_WIFI_ROAMING_BLACKLIST_TIMEOUT`, `ESP_WIFI_ROAMING_MAX_CANDIDATES` |
| Security policy | `wifi_config_t.sta.threshold.authmode`, `wifi_config_t.sta.owe_enabled`, `wifi_config_t.sta.pmf_cfg.required`, `wifi_config_t.sta.sae_pwe_h2e`, `wifi_config_t.sta.sae_pk_mode` |
| Periodic scan threshold input | `ESP_WIFI_ROAMING_SCAN_ROAM_RSSI_DIFF` |

## 4. Roam Execution

```mermaid
flowchart TD
    A["roaming_app_trigger_roam(candidate)"] --> B{"Station connected?"}
    B -- "No" --> C["Start reconnect to selected AP"]
    B -- "Yes" --> D{"Inside backoff window?"}
    D -- "Yes" --> X["Ignore request"]
    D -- "No" --> E{"BTM enabled and supported by current AP?"}

    E -- "Yes" --> F{"BTM retries remaining?"}
    F -- "Yes" --> G{"Submit BTM query?"}
    G -- "Yes" --> H["Send BTM query with selected candidate<br/>and cached neighbor list"]
    H --> I["Wait for AP or network action"]
    G -- "No" --> J{"Legacy roaming enabled?"}
    F -- "No" --> J

    E -- "No" --> J
    J -- "Yes" --> K["Start directed reconnect to selected AP"]
    J -- "No" --> L["No roam action"]
```

### Configuration used in this stage

| Area | Configuration |
| --- | --- |
| Backoff | `ESP_WIFI_ROAMING_BACKOFF_TIME` |
| BTM path | `ESP_WIFI_ROAMING_NETWORK_ASSISTED_ROAM`, `ESP_WIFI_NETWORK_ASSISTED_ROAMING_RETRY_COUNT` |
| Legacy path | `ESP_WIFI_ROAMING_LEGACY_ROAMING` |
| Post-roam IP behavior | `ESP_WIFI_ROAMING_IP_RENEW_SKIP` |

## 5. Configuration Summary by Functional Area

| Functional area | Configuration |
| --- | --- |
| Low RSSI roaming | `ESP_WIFI_ROAMING_LOW_RSSI_ROAMING`, `ESP_WIFI_ROAMING_LOW_RSSI_THRESHOLD`, `ESP_WIFI_ROAMING_LOW_RSSI_OFFSET` |
| Periodic scan roaming | `ESP_WIFI_ROAMING_PERIODIC_SCAN_MONITOR`, `ESP_WIFI_ROAMING_PERIODIC_SCAN_THRESHOLD`, `ESP_WIFI_ROAMING_SCAN_MONITOR_INTERVAL`, `ESP_WIFI_ROAMING_SCAN_ROAM_RSSI_DIFF` |
| Network-assisted roaming | `ESP_WIFI_ROAMING_NETWORK_ASSISTED_ROAM`, `ESP_WIFI_NETWORK_ASSISTED_ROAMING_RETRY_COUNT` |
| Post-roam IP behavior | `ESP_WIFI_ROAMING_IP_RENEW_SKIP` |
| Legacy roaming | `ESP_WIFI_ROAMING_LEGACY_ROAMING` |
| Periodic RRM / neighbor reports | `ESP_WIFI_ROAMING_PERIODIC_RRM_MONITORING`, `ESP_WIFI_ROAMING_RRM_MONITOR_TIME`, `ESP_WIFI_ROAMING_RRM_MONITOR_THRESHOLD` |
| Scan behavior | `ESP_WIFI_ROAMING_SCAN_MIN_SCAN_TIME`, `ESP_WIFI_ROAMING_SCAN_MAX_SCAN_TIME`, `ESP_WIFI_ROAMING_HOME_CHANNEL_DWELL_TIME`, `ESP_WIFI_ROAMING_SCAN_CHAN_LIST`, `ESP_WIFI_ROAMING_SCAN_EXPIRY_WINDOW` |
| Blacklist behavior | `ESP_WIFI_ROAMING_BSSID_BLACKLIST`, `ESP_WIFI_ROAMING_AUTO_BLACKLISTING`, `ESP_WIFI_ROAMING_MAX_CONN_FAILURES`, `ESP_WIFI_ROAMING_BLACKLIST_TIMEOUT`, `ESP_WIFI_ROAMING_MAX_CANDIDATES` |
| Runtime scan filters | `roam_set_config_params()`, `scan_filter_ssid`, `scan_filter_bssid` |
| Candidate security policy | `wifi_config_t.sta.threshold.authmode`, `wifi_config_t.sta.owe_enabled`, `wifi_config_t.sta.pmf_cfg.required`, `wifi_config_t.sta.sae_pwe_h2e`, `wifi_config_t.sta.sae_pk_mode` |

## 6. Behavioral Notes

- A periodic RRM event does not directly trigger roaming. Its purpose is to refresh the cached neighbor list used by the BTM path.
- While connected, candidate ranking uses the stronger of two values: the RSSI from the current connection and the RSSI for the same BSSID in the latest scan results.
- After a disconnect, the app checks whether the disconnected BSSID is blacklisted. This covers both BSSIDs that were already blacklisted and BSSIDs that become blacklisted because the latest failure reached the configured threshold.
- After a disconnect, the app does not compare other APs against the failed AP's scan RSSI. This lets it choose any valid AP that is not blacklisted.
- Candidate security checks allow roaming only within supported compatibility groups: Open/OWE, Personal, and Enterprise. Transition-disable policy and station security requirements are still enforced.
- A BTM query is asynchronous. After the query is sent, the app waits for AP or network action. It does not immediately force legacy roaming in the same step.
- The reconnect paths start a connection attempt. Final success or failure is reported later through the normal Wi-Fi connection and disconnection events.
- If the station is already disconnected, the app can still reconnect to a selected AP even when legacy roaming is disabled.
