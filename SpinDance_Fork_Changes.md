# SpinDance ESP IDF v5.0 Changes
The following is an analysis of the changes to [ESP IDF](https://github.com/espressif/esp-idf) `v5.0` in this [SpinDance fork](https://github.com/spindance/esp-idf) on branch `spindance_changes_v5.0`. The purpose is to categorize the changes into features and describe their purpose and usage in the [SpinDance Embedded Starter Kit](https://github.com/spindance/spindance-embedded-reuse/).

A handy way to view the diff is via this [Github pull request link](https://github.com/espressif/esp-idf/compare/v5.0...spindance:esp-idf:spindance_changes_v5.0).

The original ESP repo and this SpinDance fork can be cloned and viewed separately via:
```
git clone git@github.com:espressif/esp-idf.git esp-idf
cd esp-idf
git checkout v5.0

cd ..
git clone git@github.com:spindance/esp-idf.git spindance-esp-idf
cd spindance-esp-idf
git checkout spindance_changes_v5.0
```

## Changed Files
```
components/esp_netif/include/lwip/esp_netif_net_stack.h  (was components/lwip/port/esp32/include/netif/wlanif.h)
components/lwip/port/esp32/netif/wlanif.c
components/protocomm/include/security/protocomm_security.h
components/protocomm/include/transports/protocomm_ble.h
components/protocomm/src/common/protocomm.c
components/protocomm/src/security/security1.c
components/protocomm/src/transports/protocomm_nimble.c
components/wifi_provisioning/include/wifi_provisioning/manager.h
components/wifi_provisioning/include/wifi_provisioning/wifi_config.h
components/wifi_provisioning/include/wifi_provisioning/wifi_scan.h
components/wifi_provisioning/proto/wifi_config.proto
components/wifi_provisioning/proto/wifi_scan.proto
components/wifi_provisioning/proto-c/wifi_config.pb-c.c
components/wifi_provisioning/proto-c/wifi_config.pb-c.h
components/wifi_provisioning/proto-c/wifi_scan.pb-c.c
components/wifi_provisioning/proto-c/wifi_scan.pb-c.h
components/wifi_provisioning/python/wifi_config_pb2.py
components/wifi_provisioning/python/wifi_constants_pb2.py
components/wifi_provisioning/python/wifi_scan_pb2.py
components/wifi_provisioning/src/manager.c
components/wifi_provisioning/src/wifi_config.c
components/wifi_provisioning/src/wifi_provisioning_priv.h
components/wifi_provisioning/src/wifi_scan.c
tools/idf_tools.py
```

## Feature List
The following is a list of the SpinDance Embedded Starter Kit features that the changes in this fork support:
- Network (NW) Metrics Reporting
- Protocomm BLE Connectivity Reporting
- Provisioning State in BLE Advertisement Data (AD)
- JWT Authorization for Protocomm WiFi Provisioning
- WPA2 Enterprise NW Support
- WiFi Provisioning Sequence Changes
- Python Version Change

## Feature Details
The features, their associated changes to ESP IDF and how the change is related to the Embedded Starter Kit are described in more detail below:
- **Network (NW) Metrics Reporting**
  - Added support for simple bytes in/bytes out tracking and reporting in ESP's LWIP implementation
  - Embedded Starter Kit accesses these values via its PAL and includes them in metrics reported to MQTT
  - Impacted ESP IDF files:
    - esp_netif_net_stack.h
    - wlanif.c
  - Commits:
    - [lwip/port/esp32: add metrics for wlan bytes in/out](https://github.com/spindance/esp-idf/commit/ef9b870d0dad29b4993815c323c57202f5c1700b)
- **Protocomm BLE Connectivity Reporting**
  - Added reporting of BLE peer connectivity state (connected, connected securely, not connected)
  - Embedded Starter Kit reports these states to analagous pubsub topic IDs in the pubsub namespace `WIFI_TOPIC_NS`
  - Note: We'd likely want to implement this for `Security2` if/when we migrate to ESP IDF v5. Used to display LEDs of different colors depending upon the state.
  - Impacted ESP IDF files:
    - protocomm_security.h
    - protocomm_ble.h
    - protocomm.c
    - security1.c
    - protocomm_nimble.c
  - Commits
    - [spindance_changes for wifi_provisioning](https://github.com/spindance/esp-idf/commit/56c743a69cf9dce0bf4ce4eab4048a2c1088fdee)
- **Provisioning State in BLE Advertisement Data (AD)**
  - Added ability to directly modify the manufacturer specific data in the BLE advertisement.
  - Embedded Starter Kit uses this to include the WiFi provisioning state in the advertisement
  - Note: It maybe possible that the ESP IDF provides a different way to modify the advertisement.
  - Impacted ESP IDF files:
    - protocomm_ble.h
    - protocomm_nimble.c
  - Commits
    - [spindance_changes for wifi_provisioning](https://github.com/spindance/esp-idf/commit/56c743a69cf9dce0bf4ce4eab4048a2c1088fdee)
- **JWT Authorization for Protocomm WiFi Provisioning**
  - Added an auth token property to the Protocomm protobuf messages, which is supplied to an also added optional authorization callback for validation prior to scanning for or configuring a WiFi access point.
  - Embedded Starter Kit registers an authorization handler that validates the token as a JWT. This feature is disabled in the WiFi configuration in `devkit`.
  - Note: This was added to validate device claiming.
  - Impacted ESP IDF files:
    - Protobuf definition files:
      - wifi_config.proto
      - wifi_scan.proto
      - wifi_config.pb-c.h
      - wifi_config.pb-c.c
      - wifi_scan.pb-c.c
      - wifi_scan.pb-c.h
      - wifi_config_pb2.py
      - wifi_constants_pb2.py
      - wifi_scan_pb2.py
    - manager.h
    - manager.c
    - wifi_config.h
    - wifi_config.c
    - wifi_scan.h
    - wifi_scan.c
  - Commits
    - [spindance_changes for wifi_provisioning](https://github.com/spindance/esp-idf/commit/56c743a69cf9dce0bf4ce4eab4048a2c1088fdee)
- **WPA2 Enterprise NW Support**
  - Existing internal IDF helper functions that provide information about scanned WiFi access points were made public by moving their declarations to a public IDF header file.
  - Embedded Starter Kit uses these functions to determine if the access point attempting to be configured is a WPA2 enterprise NW, and then enables or disables support for WPA2 enterprise.
  - Note: It's possible that a newer version of ESP IDF may sufficiently support WPA2 enterprise. _Also, it's possible that WPA2 enterprise NW support inside the Starter Kit's `wifi` component is not tested or fully functional._
  - Impacted ESP IDF files:
    - manager.h
    - wifi_provisioning_priv.h
  - Commits
    - [spindance_changes for wifi_provisioning](https://github.com/spindance/esp-idf/commit/56c743a69cf9dce0bf4ce4eab4048a2c1088fdee)
- **WiFi Provisioning Sequence Changes**
  - `WIFI_PROV_SCAN_STARTED` was added to the `wifi_prov_cb_event_t` enum and is reported via the `app_event_handler` in manager.c once a WiFi access point scan is started.
    - Embedded Starter kit wifi.c listens for `WIFI_PROV_SCAN_STARTED`, at which point it sets a boolean flag `_provisioning_in_progress` indicating WiFi is being used to scan for and configure an access points. It also disconnects wifi via `esp_wifi_disconnect()`, which happens to be one of the calls that is removed in manager.c `wifi_prov_mgr_start_provisioning()`, as discussed below.
  - Code was removed in manager.c `wifi_prov_mgr_start_provisioning()` to remove steps from the setup when WiFi provisioning is started.
    - Removed "Start Wi-Fi in Station Mode":
      - Removed calls: `esp_wifi_set_mode(WIFI_MODE_STA); esp_wifi_start();`
      - Embedded Starter Kit calls these two functions once, at startup, in `wifi_init()`. It's not clear if calling these again in `wifi_prov_mgr_start_provisioning()` is therefore necessary (apparently it is not), _nor is it clear if calling them again by re-including the removed calls would be problematic_.
    - Removed "Change Wi-Fi storage to RAM temporarily and erase any old credentials in RAM"
      - Removed calls: `esp_wifi_set_storage(WIFI_STORAGE_RAM); esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg_empty);`
      - Based upon the comments, this appears to be a sort of back door means of preventing the **application code** from attempting to auto-reconnect when it receives `WIFI_EVENT_STA_DISCONNECTED` due to the call to `esp_wifi_disconnect()` discussed below. Presumably the auto-reconnect cannot occur or won't succeed due to there being no stored access point credentials.
      - Embedded Starter Kit wifi.c appears to manage not attempting to reconnect during provisioning. It controls when it calls `esp_wifi_connect()` when it receives `WIFI_EVENT_STA_DISCONNECTED`, only calling it if `_provisioning_in_progress` is false and WiFi is not actively being disconnected from.
    - Removed "Disconnect to make sure device doesn't remain connected to the AP whose credentials were present earlier"
      - Removed call: `esp_wifi_disconnect();`
      - Embedded Starter Kit calls this function when it receives the `WIFI_PROV_SCAN_STARTED` event, as discussed above.
  - Impacted ESP IDF files:
    - manager.h
    - manager.c
  - Commits
    - [spindance_changes for wifi_provisioning](https://github.com/spindance/esp-idf/commit/56c743a69cf9dce0bf4ce4eab4048a2c1088fdee)
- **Python Version Change**
  - idf_tools.py was modified to use `python3` instead of the Mac default (`python2`)
  - Impacted ESP IDF files:
    - idf_tools.py
  - Commits
    - [spindance_changes for wifi_provisioning](https://github.com/spindance/esp-idf/commit/56c743a69cf9dce0bf4ce4eab4048a2c1088fdee)


## Feature / File Association
| File                                                                 | SK Feature(s) |
|:-------------------------------------------------------------------- | -------------- |
|components/lwip/port/esp32/include/esp_netif_net_stack.h              | NW Metrics Reporting |
|components/lwip/port/esp32/netif/wlanif.c                             | NW Metrics Reporting |
|components/protocomm/include/security/protocomm_security.h            | Protocomm BLE Connectivity Reporting |
|components/protocomm/include/transports/protocomm_ble.h               | Protocomm BLE Connectivity Reporting, Provisioning State in BLE AD |
|components/protocomm/src/common/protocomm.c                           | Protocomm BLE Connectivity Reporting |
|components/protocomm/src/security/security1.c                         | Protocomm BLE Connectivity Reporting |
|components/protocomm/src/transports/protocomm_nimble.c                | Protocomm BLE Connectivity Reporting, Provisioning State in BLE AD |
|components/wifi_provisioning/include/wifi_provisioning/manager.h      | WiFi Provisioning Sequence Changes, JWT Authorization for Protocomm WiFi provisioning, WPA2 Enterprise NW Support |
|components/wifi_provisioning/include/wifi_provisioning/wifi_config.h  | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/include/wifi_provisioning/wifi_scan.h    | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/proto/wifi_config.proto                  | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/proto/wifi_scan.proto                    | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/proto-c/wifi_config.pb-c.c               | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/proto-c/wifi_config.pb-c.h               | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/proto-c/wifi_scan.pb-c.c                 | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/proto-c/wifi_scan.pb-c.h                 | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/python/wifi_config_pb2.py                | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/python/wifi_constants_pb2.py             | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/python/wifi_scan_pb2.py                  | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/src/manager.c                            | JWT Authorization for Protocomm WiFi Provisioning, WiFi Provisioning Sequence Changes |
|components/wifi_provisioning/src/wifi_config.c                        | JWT Authorization for Protocomm WiFi Provisioning |
|components/wifi_provisioning/src/wifi_provisioning_priv.h             | WPA2 Enterprise NW Support |
|components/wifi_provisioning/src/wifi_scan.c                          | JWT Authorization for Protocomm WiFi Provisioning |
|tools/idf_tools.py                                                    | Python Version Change |
