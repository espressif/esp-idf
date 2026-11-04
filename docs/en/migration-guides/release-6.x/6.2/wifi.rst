Wi-Fi
=====

:link_to_translation:`zh_CN:[中文]`

Wi-Fi Enterprise
----------------

Phase 2 Authentication
~~~~~~~~~~~~~~~~~~~~~~

``esp_eap_client_set_phase2_method()`` sets the phase 2 method for both EAP-TTLS and EAP-PEAP, including tunneled EAP-GTC (``ESP_EAP_PHASE2_GTC``).

``esp_eap_client_set_phase2_method()`` returns ``ESP_ERR_INVALID_ARG`` when ``type`` is not a value of ``esp_eap_phase2_types``. On devices with on-chip Wi-Fi, ``esp_eap_client_set_ttls_phase2_method()`` forwards to that function and returns the same error. Previously, an out-of-range value was stored as ``auth=MSCHAPV2`` and the call returned ``ESP_OK``.

Removed Functions
~~~~~~~~~~~~~~~~~

``esp_wpa2.h`` was already removed. The wrapper implementations in ``esp_wpa2_api_port.c`` are removed as well, so these symbols are no longer in ``libwpa_supplicant.a``. Code that declared the prototypes itself fails at link time. Call the replacement from ``esp_eap_client.h``:

- ``esp_wifi_sta_wpa2_ent_enable()``: use ``esp_wifi_sta_enterprise_enable()``
- ``esp_wifi_sta_wpa2_ent_disable()``: use ``esp_wifi_sta_enterprise_disable()``
- ``esp_wifi_sta_wpa2_ent_set_identity()`` / ``esp_wifi_sta_wpa2_ent_clear_identity()``: use ``esp_eap_client_set_identity()`` / ``esp_eap_client_clear_identity()``
- ``esp_wifi_sta_wpa2_ent_set_username()`` / ``esp_wifi_sta_wpa2_ent_clear_username()``: use ``esp_eap_client_set_username()`` / ``esp_eap_client_clear_username()``
- ``esp_wifi_sta_wpa2_ent_set_password()`` / ``esp_wifi_sta_wpa2_ent_clear_password()``: use ``esp_eap_client_set_password()`` / ``esp_eap_client_clear_password()``
- ``esp_wifi_sta_wpa2_ent_set_new_password()`` / ``esp_wifi_sta_wpa2_ent_clear_new_password()``: use ``esp_eap_client_set_new_password()`` / ``esp_eap_client_clear_new_password()``
- ``esp_wifi_sta_wpa2_ent_set_ca_cert()`` / ``esp_wifi_sta_wpa2_ent_clear_ca_cert()``: use ``esp_eap_client_set_ca_cert()`` / ``esp_eap_client_clear_ca_cert()``
- ``esp_wifi_sta_wpa2_ent_set_cert_key()`` / ``esp_wifi_sta_wpa2_ent_clear_cert_key()``: use ``esp_eap_client_set_certificate_and_key()`` / ``esp_eap_client_clear_certificate_and_key()``
- ``esp_wifi_sta_wpa2_ent_set_disable_time_check()`` / ``esp_wifi_sta_wpa2_ent_get_disable_time_check()``: use ``esp_eap_client_set_disable_time_check()`` / ``esp_eap_client_get_disable_time_check()``
- ``esp_wifi_sta_wpa2_ent_set_ttls_phase2_method()``: use ``esp_eap_client_set_phase2_method()``
- ``esp_wifi_sta_wpa2_set_suiteb_192bit_certification()``: use ``esp_eap_client_set_suiteb_192bit_certification()``
- ``esp_wifi_sta_wpa2_ent_set_pac_file()``: use ``esp_eap_client_set_pac_file()``
- ``esp_wifi_sta_wpa2_ent_set_fast_phase1_params()``: use ``esp_eap_client_set_fast_params()``
- ``esp_wifi_sta_wpa2_use_default_cert_bundle()``: use ``esp_eap_client_use_default_cert_bundle()``
