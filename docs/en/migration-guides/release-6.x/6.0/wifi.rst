Wi-Fi
=====

This document outlines the Wi-Fi related breaking changes in release v6.0.

Removed Functions and Types
---------------------------

The following functions and types have been removed.

- **DPP:**
    - The DPP event callback ``esp_supp_dpp_event_cb_t`` and event enum ``esp_supp_dpp_event_t`` have been removed. Please use Wi-Fi events directly (e.g. ``WIFI_EVENT_DPP_URI_READY``, ``WIFI_EVENT_DPP_CFG_RECVD``, ``WIFI_EVENT_DPP_FAILED``).

- **RRM:** The function ``esp_rrm_send_neighbor_rep_request`` has been removed. Use ``esp_rrm_send_neighbor_report_request`` instead.

- **Antenna Configuration:**
    - The functions ``esp_wifi_set_ant_gpio``, ``esp_wifi_get_ant_gpio``, ``esp_wifi_set_ant``, and ``esp_wifi_get_ant`` have been removed. Please use the equivalent functions from ``esp_phy`` component: ``esp_phy_set_ant_gpio``, ``esp_phy_get_ant_gpio``, ``esp_phy_set_ant``, and ``esp_phy_get_ant``.

- **ESP-NOW:** The function ``esp_wifi_config_espnow_rate`` has been removed. Use ``esp_now_set_peer_rate_config`` instead.

Removed Variables, Macros and Enum Values
-----------------------------------------

- **Authentication Modes:** The authentication modes ``WIFI_AUTH_WPA3_EXT_PSK`` and ``WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE`` have been removed. Use ``WIFI_AUTH_WPA3_PSK`` instead.

- **Neighbor Report Event:** In the ``wifi_event_neighbor_report_t`` structure, the ``report`` field has been removed. Please use the ``n_report`` field instead, which is a flexible array member.

- **Wi-Fi Interface:** The header file ``components/esp_wifi/include/esp_interface.h`` has been removed. The ``wifi_interface_t`` enum is now defined in :project_file:`components/esp_wifi/include/esp_wifi_types_generic.h`. The enum values (e.g. ``WIFI_IF_STA``, ``WIFI_IF_AP``) are no longer defined based on ``ESP_IF_WIFI_STA``, ``ESP_IF_WIFI_AP``, etc.

- **Wi-Fi Interface Macros:** The macros ``ESP_IF_WIFI_STA`` and ``ESP_IF_WIFI_AP`` have been removed. Please use the enum values ``WIFI_IF_STA`` and ``WIFI_IF_AP`` directly.

- **Wi-Fi Disconnection Reasons:** The disconnection reason codes ``WIFI_REASON_ASSOC_EXPIRE``, ``WIFI_REASON_NOT_AUTHED``, and ``WIFI_REASON_NOT_ASSOCED`` have been removed. Please use ``WIFI_REASON_AUTH_EXPIRE``, ``WIFI_REASON_CLASS2_FRAME_FROM_NONAUTH_STA``, and ``WIFI_REASON_CLASS3_FRAME_FROM_NONASSOC_STA`` respectively.

- **NAN:**
    - Field ``svc_info`` from structures ``wifi_nan_publish_cfg_t``, ``wifi_nan_subscribe_cfg_t``, ``wifi_nan_followup_params_t``, ``wifi_event_ndp_indication_t``, ``wifi_event_ndp_confirm_t`` has been removed. Please use the equivalent fields ``ssi`` and ``ssi_len`` instead.
    - Field ``peer_svc_info`` from structure ``wifi_event_nan_receive_t`` has been removed. Please use the equivalent fields ``ssi`` and ``ssi_len`` instead.

- **FTM:** Field ``use_get_report_api`` from structure ``wifi_ftm_initiator_cfg_t`` and field ``ftm_report_data`` from structure ``wifi_event_ftm_report_t`` has been removed. Please use ``esp_wifi_ftm_get_report`` to fetch raw FTM report data instead.

Modified Functions and Types
----------------------------

The following Wi-Fi functions have been modified.

- **DPP:** The function ``esp_supp_dpp_init`` no longer accepts a callback and should be called as ``esp_supp_dpp_init(void)``.

- **WPS:** The function ``esp_wifi_wps_start`` no longer accepts a ``timeout_ms`` argument. It should now be called as ``esp_wifi_wps_start(void)``.

- **NAN:**
    - NAN API's are modified so that they can be shared between two separate NAN modes of operation called NAN Synchronization (NAN-Sync) and NAN Unsynchronized Discovery (NAN-USD) roles:
        - ``esp_wifi_nan_sync_start`` and ``esp_wifi_nan_sync_stop`` replace the previous ``esp_wifi_nan_start`` / ``esp_wifi_nan_stop`` pair.
        - Functions starting with prefix ``esp_wifi_nan_*`` are shared by both NAN-Sync and NAN-USD modes (for example ``esp_wifi_nan_publish_service`` and ``esp_wifi_nan_subscribe_service``).
        - Use ``WIFI_NAN_SYNC_CONFIG_DEFAULT()`` instead of the former ``WIFI_NAN_CONFIG_DEFAULT()`` macro when initialising synchronized NAN configuration.
        - The structure ``wifi_nan_config_t`` has been renamed to ``wifi_nan_sync_config_t``; update declarations such as ``wifi_config_t.nan`` to use the new type.
        - NAN datapath APIs and associated structures are valid for NAN Synchronization (NAN-Sync) clusters only.
    - Wi-Fi events ``WIFI_EVENT_NAN_STARTED`` / ``WIFI_EVENT_NAN_STOPPED`` have been renamed to ``WIFI_EVENT_NAN_SYNC_STARTED`` / ``WIFI_EVENT_NAN_SYNC_STOPPED``.
    - The function argument ``ndp_resp_needed`` of ``esp_wifi_nan_publish_service`` has been moved to structure ``wifi_nan_publish_cfg_t``.
    - The structure ``wifi_nan_wfa_ssi_t`` now declares its ``proto`` field as ``uint8_t`` (previously ``wifi_nan_svc_proto_t``). Update code that stores pointers to this field or relies on its size to treat it as a single octet.
    - The structures ``wifi_nan_publish_cfg_t`` and ``wifi_nan_subscribe_cfg_t`` now include a ``usd_discovery_flag`` bitfield together with corresponding ``wifi_nan_usd_config_t`` members. Add designated initialisers for these fields when constructing the structures to avoid accidental field shifts.

- **Off-Channel Operations:**
    - The structure :cpp:struct:`wifi_action_tx_req_t` now includes a ``bssid`` field that is placed in Address3 of the transmitted action frame. Initialise this field explicitly; leaving it all zeros retains the previous behaviour where the Wi-Fi driver inserts the broadcast address, so frames do not carry unintended BSSID values.
    - The structure :cpp:struct:`wifi_roc_req_t` now includes an ``allow_broadcast`` flag that controls whether broadcast and multicast action frames are passed to the ``rx_cb`` callback during remain-on-channel operations. Existing applications should review the default ``false`` value and enable it when discovery of broadcast peers is required. Frames whose Address3/BSSID is already broadcast continue to reach the callback even when ``allow_broadcast`` remains ``false``.
