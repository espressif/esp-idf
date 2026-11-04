Wi-Fi
=====

:link_to_translation:`en:[English]`

企业级 Wi-Fi
------------

阶段 2 认证
~~~~~~~~~~~

``esp_eap_client_set_phase2_method()`` 同时用于设置 EAP-TTLS 和 EAP-PEAP 的阶段 2 方法，包括隧道内的 EAP-GTC（``ESP_EAP_PHASE2_GTC``）。

当 ``type`` 不是 ``esp_eap_phase2_types`` 的取值时，``esp_eap_client_set_phase2_method()`` 返回 ``ESP_ERR_INVALID_ARG``。对于使用片上 Wi-Fi 的芯片，``esp_eap_client_set_ttls_phase2_method()`` 会转发到该函数，并返回同样的错误。此前，超出取值范围的参数会被保存为 ``auth=MSCHAPV2``，且调用返回 ``ESP_OK``。

已移除的函数
~~~~~~~~~~~~

``esp_wpa2.h`` 此前已经删除。``esp_wpa2_api_port.c`` 中的包装实现现在也已删除，因此下列符号不再由 ``libwpa_supplicant.a`` 提供。自行声明这些函数原型的代码会在链接阶段失败。请改用 ``esp_eap_client.h`` 中的对应接口：

- ``esp_wifi_sta_wpa2_ent_enable()``：改用 ``esp_wifi_sta_enterprise_enable()``
- ``esp_wifi_sta_wpa2_ent_disable()``：改用 ``esp_wifi_sta_enterprise_disable()``
- ``esp_wifi_sta_wpa2_ent_set_identity()`` / ``esp_wifi_sta_wpa2_ent_clear_identity()``：改用 ``esp_eap_client_set_identity()`` / ``esp_eap_client_clear_identity()``
- ``esp_wifi_sta_wpa2_ent_set_username()`` / ``esp_wifi_sta_wpa2_ent_clear_username()``：改用 ``esp_eap_client_set_username()`` / ``esp_eap_client_clear_username()``
- ``esp_wifi_sta_wpa2_ent_set_password()`` / ``esp_wifi_sta_wpa2_ent_clear_password()``：改用 ``esp_eap_client_set_password()`` / ``esp_eap_client_clear_password()``
- ``esp_wifi_sta_wpa2_ent_set_new_password()`` / ``esp_wifi_sta_wpa2_ent_clear_new_password()``：改用 ``esp_eap_client_set_new_password()`` / ``esp_eap_client_clear_new_password()``
- ``esp_wifi_sta_wpa2_ent_set_ca_cert()`` / ``esp_wifi_sta_wpa2_ent_clear_ca_cert()``：改用 ``esp_eap_client_set_ca_cert()`` / ``esp_eap_client_clear_ca_cert()``
- ``esp_wifi_sta_wpa2_ent_set_cert_key()`` / ``esp_wifi_sta_wpa2_ent_clear_cert_key()``：改用 ``esp_eap_client_set_certificate_and_key()`` / ``esp_eap_client_clear_certificate_and_key()``
- ``esp_wifi_sta_wpa2_ent_set_disable_time_check()`` / ``esp_wifi_sta_wpa2_ent_get_disable_time_check()``：改用 ``esp_eap_client_set_disable_time_check()`` / ``esp_eap_client_get_disable_time_check()``
- ``esp_wifi_sta_wpa2_ent_set_ttls_phase2_method()``：改用 ``esp_eap_client_set_phase2_method()``
- ``esp_wifi_sta_wpa2_set_suiteb_192bit_certification()``：改用 ``esp_eap_client_set_suiteb_192bit_certification()``
- ``esp_wifi_sta_wpa2_ent_set_pac_file()``：改用 ``esp_eap_client_set_pac_file()``
- ``esp_wifi_sta_wpa2_ent_set_fast_phase1_params()``：改用 ``esp_eap_client_set_fast_params()``
- ``esp_wifi_sta_wpa2_use_default_cert_bundle()``：改用 ``esp_eap_client_use_default_cert_bundle()``
