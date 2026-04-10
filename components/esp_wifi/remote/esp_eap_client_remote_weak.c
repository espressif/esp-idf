/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// This file is auto-generated
#include "esp_eap_client_remote_api.h"
#include "esp_log.h"

#define WEAK __attribute__((weak))
#define LOG_UNSUPPORTED_AND_RETURN(ret) ESP_LOGW("esp_eap_client_remote_weak", "%s unsupported", __func__); \
         return ret;
#define LOG_UNSUPPORTED_VOID() ESP_LOGW("esp_eap_client_remote_weak", "%s unsupported", __func__);

WEAK esp_err_t esp_wifi_remote_sta_enterprise_enable(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_wifi_remote_sta_enterprise_disable(void)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_eap_client_remote_set_identity(const unsigned char *identity, int len)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK void esp_eap_client_remote_clear_identity(void)
{
    LOG_UNSUPPORTED_VOID();
}

WEAK esp_err_t esp_eap_client_remote_set_username(const unsigned char *username, int len)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK void esp_eap_client_remote_clear_username(void)
{
    LOG_UNSUPPORTED_VOID();
}

WEAK esp_err_t esp_eap_client_remote_set_password(const unsigned char *password, int len)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK void esp_eap_client_remote_clear_password(void)
{
    LOG_UNSUPPORTED_VOID();
}

WEAK esp_err_t esp_eap_client_remote_set_new_password(const unsigned char *new_password, int len)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK void esp_eap_client_remote_clear_new_password(void)
{
    LOG_UNSUPPORTED_VOID();
}

WEAK esp_err_t esp_eap_client_remote_set_ca_cert(const unsigned char *ca_cert, int ca_cert_len)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK void esp_eap_client_remote_clear_ca_cert(void)
{
    LOG_UNSUPPORTED_VOID();
}

WEAK esp_err_t esp_eap_client_remote_set_certificate_and_key(const unsigned char *client_cert, int client_cert_len, const unsigned char *private_key, int private_key_len, const unsigned char *private_key_password, int private_key_passwd_len)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK void esp_eap_client_remote_clear_certificate_and_key(void)
{
    LOG_UNSUPPORTED_VOID();
}

WEAK esp_err_t esp_eap_client_remote_set_disable_time_check(_Bool disable)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_eap_client_remote_get_disable_time_check(_Bool *disable)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_eap_client_remote_set_ttls_phase2_method(esp_eap_ttls_phase2_types type)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_eap_client_remote_set_suiteb_192bit_certification(_Bool enable)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_eap_client_remote_set_pac_file(const unsigned char *pac_file, int pac_file_len)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_eap_client_remote_set_fast_params(esp_eap_fast_config config)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_eap_client_remote_use_default_cert_bundle(_Bool use_default_bundle)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK void esp_wifi_remote_set_okc_support(_Bool enable)
{
    LOG_UNSUPPORTED_VOID();
}

WEAK esp_err_t esp_eap_client_remote_set_domain_name(const char *domain_name)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}

WEAK esp_err_t esp_eap_client_remote_set_eap_methods(esp_eap_method_t methods)
{
    LOG_UNSUPPORTED_AND_RETURN(ESP_ERR_NOT_SUPPORTED);
}
