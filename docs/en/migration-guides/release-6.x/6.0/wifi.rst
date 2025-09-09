Wi-Fi
=====

This document outlines the Wi-Fi related breaking changes in release v6.0.

Removed Functions and Types
---------------------------

The following functions and types have been removed.

- **WPA2 Enterprise:** The header file :project_file:`components/wpa_supplicant/esp_supplicant/include/esp_wpa2.h` has been removed. Please use the APIs from :project_file:`components/wpa_supplicant/esp_supplicant/include/esp_eap_client.h` instead. The following functions have been removed:
    - ``esp_wifi_sta_wpa2_ent_enable``, use ``esp_eap_client_enable`` instead.
    - ``esp_wifi_sta_wpa2_ent_disable``, use ``esp_eap_client_disable`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_identity``, use ``esp_eap_client_set_identity`` instead.
    - ``esp_wifi_sta_wpa2_ent_clear_identity``, use ``esp_eap_client_clear_identity`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_username``, use ``esp_eap_client_set_username`` instead.
    - ``esp_wifi_sta_wpa2_ent_clear_username``, use ``esp_eap_client_clear_username`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_password``, use ``esp_eap_client_set_password`` instead.
    - ``esp_wifi_sta_wpa2_ent_clear_password``, use ``esp_eap_client_clear_password`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_new_password``, use ``esp_eap_client_set_new_password`` instead.
    - ``esp_wifi_sta_wpa2_ent_clear_new_password``, use ``esp_eap_client_clear_new_password`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_ca_cert``, use ``esp_eap_client_set_ca_cert`` instead.
    - ``esp_wifi_sta_wpa2_ent_clear_ca_cert``, use ``esp_eap_client_clear_ca_cert`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_cert_key``, use ``esp_eap_client_set_certificate_and_key`` instead.
    - ``esp_wifi_sta_wpa2_ent_clear_cert_key``, use ``esp_eap_client_clear_certificate_and_key`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_disable_time_check``, use ``esp_eap_client_set_disable_time_check`` instead.
    - ``esp_wifi_sta_wpa2_ent_get_disable_time_check``, use ``esp_eap_client_get_disable_time_check`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_ttls_phase2_method``, use ``esp_eap_client_set_ttls_phase2_method`` instead.
    - ``esp_wifi_sta_wpa2_set_suiteb_192bit_certification``, use ``esp_eap_client_set_suiteb_192bit_certification`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_pac_file``, use ``esp_eap_client_set_pac_file`` instead.
    - ``esp_wifi_sta_wpa2_ent_set_fast_phase1_params``, use ``esp_eap_client_set_fast_params`` instead.
    - ``esp_wifi_sta_wpa2_use_default_cert_bundle``, use ``esp_eap_client_use_default_cert_bundle`` instead.

- **DPP:**
    - The DPP event callback ``esp_supp_dpp_event_cb_t`` and event enum ``esp_supp_dpp_event_t`` have been removed. Please use Wi-Fi events directly (e.g. ``WIFI_EVENT_DPP_URI_READY``, ``WIFI_EVENT_DPP_CFG_RECVD``, ``WIFI_EVENT_DPP_FAILED``).
    - The function ``esp_supp_dpp_init`` no longer accepts a callback and should be called as ``esp_supp_dpp_init(void)``.

- **RRM:** The function ``esp_rrm_send_neighbor_rep_request`` has been removed. Use ``esp_rrm_send_neighbor_report_request`` instead.

- **Antenna Configuration:**
    - The functions ``esp_wifi_set_ant_gpio``, ``esp_wifi_get_ant_gpio``, ``esp_wifi_set_ant``, and ``esp_wifi_get_ant`` have been removed. Please use the equivalent functions from ``esp_phy`` component: ``esp_phy_set_ant_gpio``, ``esp_phy_get_ant_gpio``, ``esp_phy_set_ant``, and ``esp_phy_get_ant``.

- **ESP-NOW:** The function ``esp_wifi_config_espnow_rate`` has been removed. Use ``esp_now_set_peer_rate_config`` instead.

- **WPS:** The function ``esp_wifi_wps_start`` no longer accepts a ``timeout_ms`` argument. It should now be called as ``esp_wifi_wps_start(void)``.

Removed Macros and Enum Values
-------------------------------

- **Authentication Modes:** The authentication modes ``WIFI_AUTH_WPA3_EXT_PSK`` and ``WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE`` have been removed. Use ``WIFI_AUTH_WPA3_PSK`` instead.

- **Neighbor Report Event:** In the ``wifi_event_neighbor_report_t`` structure, the ``report`` field has been removed. Please use the ``n_report`` field instead, which is a flexible array member.

- **Wi-Fi Interface:** The header file :project_file:`components/esp_wifi/include/esp_interface.h` has been removed. The ``wifi_interface_t`` enum is now defined in :project_file:`components/esp_wifi/include/esp_wifi_types_generic.h`. The enum values (e.g. ``WIFI_IF_STA``, ``WIFI_IF_AP``) are no longer defined based on ``ESP_IF_WIFI_STA``, ``ESP_IF_WIFI_AP``, etc.
