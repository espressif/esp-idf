/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_eap_client.h"

esp_err_t esp_wifi_sta_wpa2_ent_enable(void)
{
    return esp_wifi_sta_enterprise_enable();
}

esp_err_t esp_wifi_sta_wpa2_ent_disable(void)
{
    return esp_wifi_sta_enterprise_disable();
}

esp_err_t esp_wifi_sta_wpa2_ent_set_identity(const unsigned char *identity, int len)
{
    return esp_eap_client_set_identity(identity, len);
}

void esp_wifi_sta_wpa2_ent_clear_identity(void)
{
    esp_eap_client_clear_identity();
}

esp_err_t esp_wifi_sta_wpa2_ent_set_username(const unsigned char *username, int len)
{
    return esp_eap_client_set_username(username, len);
}

void esp_wifi_sta_wpa2_ent_clear_username(void)
{
    esp_eap_client_clear_username();
}

esp_err_t esp_wifi_sta_wpa2_ent_set_password(const unsigned char *password, int len)
{
    return esp_eap_client_set_password(password, len);
}

void esp_wifi_sta_wpa2_ent_clear_password(void)
{
    esp_eap_client_clear_password();
}

esp_err_t esp_wifi_sta_wpa2_ent_set_new_password(const unsigned char *new_password, int len)
{
    return esp_eap_client_set_new_password(new_password, len);
}

void esp_wifi_sta_wpa2_ent_clear_new_password(void)
{
    esp_eap_client_clear_new_password();
}

esp_err_t esp_wifi_sta_wpa2_ent_set_ca_cert(const unsigned char *ca_cert, int ca_cert_len)
{
    return esp_eap_client_set_ca_cert(ca_cert, ca_cert_len);
}

void esp_wifi_sta_wpa2_ent_clear_ca_cert(void)
{
    esp_eap_client_clear_ca_cert();
}

esp_err_t esp_wifi_sta_wpa2_ent_set_cert_key(const unsigned char *client_cert, int client_cert_len,
                                             const unsigned char *private_key, int private_key_len,
                                             const unsigned char *private_key_passwd, int private_key_passwd_len)
{
    return esp_eap_client_set_certificate_and_key(client_cert, client_cert_len,
                                                  private_key, private_key_len, private_key_passwd, private_key_passwd_len);
}

void esp_wifi_sta_wpa2_ent_clear_cert_key(void)
{
    esp_eap_client_clear_certificate_and_key();
}

esp_err_t esp_wifi_sta_wpa2_ent_set_disable_time_check(bool disable)
{
    return esp_eap_client_set_disable_time_check(disable);
}

esp_err_t esp_wifi_sta_wpa2_ent_get_disable_time_check(bool *disable)
{
    return esp_eap_client_get_disable_time_check(disable);
}

esp_err_t esp_wifi_sta_wpa2_ent_set_ttls_phase2_method(esp_eap_ttls_phase2_types type)
{
    return esp_eap_client_set_ttls_phase2_method(type);
}

esp_err_t esp_wifi_sta_wpa2_set_suiteb_192bit_certification(bool enable)
{
    return esp_eap_client_set_suiteb_192bit_certification(enable);
}

esp_err_t esp_wifi_sta_wpa2_ent_set_pac_file(const unsigned char *pac_file, int pac_file_len)
{
    return esp_eap_client_set_pac_file(pac_file, pac_file_len);
}

esp_err_t esp_wifi_sta_wpa2_ent_set_fast_phase1_params(esp_eap_fast_config config)
{
    return esp_eap_client_set_fast_params(config);
}

esp_err_t esp_wifi_sta_wpa2_use_default_cert_bundle(bool use_default_bundle)
{
    return esp_eap_client_use_default_cert_bundle(use_default_bundle);
}
