/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// This file is auto-generated
#include "esp_eap_client.h"
#include "esp_eap_client_remote_api.h"

esp_err_t esp_wifi_sta_enterprise_enable(void)
{
    return esp_wifi_remote_sta_enterprise_enable();
}

esp_err_t esp_wifi_sta_enterprise_disable(void)
{
    return esp_wifi_remote_sta_enterprise_disable();
}

esp_err_t esp_eap_client_set_identity(const unsigned char *identity, int len)
{
    return esp_eap_client_remote_set_identity(identity, len);
}

void esp_eap_client_clear_identity(void)
{
    esp_eap_client_remote_clear_identity();
}

esp_err_t esp_eap_client_set_username(const unsigned char *username, int len)
{
    return esp_eap_client_remote_set_username(username, len);
}

void esp_eap_client_clear_username(void)
{
    esp_eap_client_remote_clear_username();
}

esp_err_t esp_eap_client_set_password(const unsigned char *password, int len)
{
    return esp_eap_client_remote_set_password(password, len);
}

void esp_eap_client_clear_password(void)
{
    esp_eap_client_remote_clear_password();
}

esp_err_t esp_eap_client_set_new_password(const unsigned char *new_password, int len)
{
    return esp_eap_client_remote_set_new_password(new_password, len);
}

void esp_eap_client_clear_new_password(void)
{
    esp_eap_client_remote_clear_new_password();
}

esp_err_t esp_eap_client_set_ca_cert(const unsigned char *ca_cert, int ca_cert_len)
{
    return esp_eap_client_remote_set_ca_cert(ca_cert, ca_cert_len);
}

void esp_eap_client_clear_ca_cert(void)
{
    esp_eap_client_remote_clear_ca_cert();
}

esp_err_t esp_eap_client_set_certificate_and_key(const unsigned char *client_cert, int client_cert_len, const unsigned char *private_key, int private_key_len, const unsigned char *private_key_password, int private_key_passwd_len)
{
    return esp_eap_client_remote_set_certificate_and_key(client_cert, client_cert_len, private_key, private_key_len, private_key_password, private_key_passwd_len);
}

void esp_eap_client_clear_certificate_and_key(void)
{
    esp_eap_client_remote_clear_certificate_and_key();
}

esp_err_t esp_eap_client_set_disable_time_check(_Bool disable)
{
    return esp_eap_client_remote_set_disable_time_check(disable);
}

esp_err_t esp_eap_client_get_disable_time_check(_Bool *disable)
{
    return esp_eap_client_remote_get_disable_time_check(disable);
}

esp_err_t esp_eap_client_set_ttls_phase2_method(esp_eap_ttls_phase2_types type)
{
    return esp_eap_client_remote_set_ttls_phase2_method(type);
}

esp_err_t esp_eap_client_set_suiteb_192bit_certification(_Bool enable)
{
    return esp_eap_client_remote_set_suiteb_192bit_certification(enable);
}

esp_err_t esp_eap_client_set_pac_file(const unsigned char *pac_file, int pac_file_len)
{
    return esp_eap_client_remote_set_pac_file(pac_file, pac_file_len);
}

esp_err_t esp_eap_client_set_fast_params(esp_eap_fast_config config)
{
    return esp_eap_client_remote_set_fast_params(config);
}

esp_err_t esp_eap_client_use_default_cert_bundle(_Bool use_default_bundle)
{
    return esp_eap_client_remote_use_default_cert_bundle(use_default_bundle);
}

void esp_wifi_set_okc_support(_Bool enable)
{
    esp_wifi_remote_set_okc_support(enable);
}

esp_err_t esp_eap_client_set_domain_name(const char *domain_name)
{
    return esp_eap_client_remote_set_domain_name(domain_name);
}

esp_err_t esp_eap_client_set_eap_methods(esp_eap_method_t methods)
{
    return esp_eap_client_remote_set_eap_methods(methods);
}
