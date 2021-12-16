// Hardware crypto support Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_WPA2_H
#define _ESP_WPA2_H

#include <stdbool.h>

#include "esp_err.h"

typedef struct {
   int fast_provisioning;
   int fast_max_pac_list_len;
   bool fast_pac_format_binary;
} esp_eap_fast_config;

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Enable wpa2 enterprise authentication.
  *
  * @attention 1. wpa2 enterprise authentication can only be used when ESP32 station is enabled.
  * @attention 2. wpa2 enterprise authentication can only support TLS, PEAP-MSCHAPv2 and TTLS-MSCHAPv2 method.
  *
  * @return
  *    - ESP_OK: succeed.
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
esp_err_t esp_wifi_sta_wpa2_ent_enable(void);

/**
  * @brief  Disable wpa2 enterprise authentication.
  *
  * @attention 1. wpa2 enterprise authentication can only be used when ESP32 station is enabled.
  * @attention 2. wpa2 enterprise authentication can only support TLS, PEAP-MSCHAPv2 and TTLS-MSCHAPv2 method.
  *
  * @return
  *    - ESP_OK: succeed.
  */
esp_err_t esp_wifi_sta_wpa2_ent_disable(void);

/**
  * @brief  Set identity for PEAP/TTLS method.
  *
  * @attention The API only passes the parameter identity to the global pointer variable in wpa2 enterprise module.
  *
  * @param  identity: point to address where stores the identity;
  * @param  len: length of identity, limited to 1~127
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(len <= 0 or len >= 128)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
esp_err_t esp_wifi_sta_wpa2_ent_set_identity(const unsigned char *identity, int len);

/**
  * @brief  Clear identity for PEAP/TTLS method.
  */
void esp_wifi_sta_wpa2_ent_clear_identity(void);

/**
  * @brief  Set username for PEAP/TTLS method.
  *
  * @attention The API only passes the parameter username to the global pointer variable in wpa2 enterprise module.
  *
  * @param  username: point to address where stores the username;
  * @param  len: length of username, limited to 1~127
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(len <= 0 or len >= 128)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
esp_err_t esp_wifi_sta_wpa2_ent_set_username(const unsigned char *username, int len);

/**
  * @brief  Clear username for PEAP/TTLS method.
  */
void esp_wifi_sta_wpa2_ent_clear_username(void);

/**
  * @brief  Set password for PEAP/TTLS method..
  *
  * @attention The API only passes the parameter password to the global pointer variable in wpa2 enterprise module.
  *
  * @param  password: point to address where stores the password;
  * @param  len: length of password(len > 0)
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(len <= 0)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
esp_err_t esp_wifi_sta_wpa2_ent_set_password(const unsigned char *password, int len);

/**
  * @brief  Clear password for PEAP/TTLS method..
  */
void esp_wifi_sta_wpa2_ent_clear_password(void);

/**
  * @brief  Set new password for MSCHAPv2 method..
  *
  * @attention 1. The API only passes the parameter password to the global pointer variable in wpa2 enterprise module.
  * @attention 2. The new password is used to substitute the old password when eap-mschapv2 failure request message with error code ERROR_PASSWD_EXPIRED is received.
  *
  * @param  new_password: point to address where stores the password;
  * @param  len: length of password
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(len <= 0)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */

esp_err_t esp_wifi_sta_wpa2_ent_set_new_password(const unsigned char *new_password, int len);

/**
  * @brief  Clear new password for MSCHAPv2 method..
  */
void esp_wifi_sta_wpa2_ent_clear_new_password(void);

/**
  * @brief  Set CA certificate for PEAP/TTLS method.
  *
  * @attention 1. The API only passes the parameter ca_cert to the global pointer variable in wpa2 enterprise module.
  * @attention 2. The ca_cert should be zero terminated.
  *
  * @param  ca_cert: point to address where stores the CA certificate;
  * @param  ca_cert_len: length of ca_cert
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_sta_wpa2_ent_set_ca_cert(const unsigned char *ca_cert, int ca_cert_len);

/**
  * @brief  Clear CA certificate for PEAP/TTLS method.
  */
void esp_wifi_sta_wpa2_ent_clear_ca_cert(void);

/**
  * @brief  Set client certificate and key.
  *
  * @attention 1. The API only passes the parameter client_cert, private_key and private_key_passwd to the global pointer variable in wpa2 enterprise module.
  * @attention 2. The client_cert, private_key and private_key_passwd should be zero terminated.
  *
  * @param  client_cert: point to address where stores the client certificate;
  * @param  client_cert_len: length of client certificate;
  * @param  private_key: point to address where stores the private key;
  * @param  private_key_len: length of private key, limited to 1~2048;
  * @param  private_key_password: point to address where stores the private key password;
  * @param  private_key_password_len: length of private key password;
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_sta_wpa2_ent_set_cert_key(const unsigned char *client_cert, int client_cert_len, const unsigned char *private_key, int private_key_len, const unsigned char *private_key_passwd, int private_key_passwd_len);

/**
  * @brief  Clear client certificate and key.
  */
void esp_wifi_sta_wpa2_ent_clear_cert_key(void);

/**
  * @brief  Set wpa2 enterprise certs time check(disable or not).
  *
  * @param  true: disable wpa2 enterprise certs time check
  * @param  false: enable wpa2 enterprise certs time check
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_sta_wpa2_ent_set_disable_time_check(bool disable);

/**
  * @brief  Get wpa2 enterprise certs time check(disable or not).
  *
  * @param  disable: store disable value
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_sta_wpa2_ent_get_disable_time_check(bool *disable);

/**
  * @brief  Set client pac file
  *
  * @attention  1. For files read from the file system, length has to be decremented by 1 byte.
  * @attention  2. Disabling the WPA_MBEDTLS_CRYPTO config is required to use EAP-FAST.
  *
  * @param  pac_file: pointer to the pac file
  *         pac_file_len: length of the pac file
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
esp_err_t esp_wifi_sta_wpa2_ent_set_pac_file(const unsigned char *pac_file, int pac_file_len);

/**
  * @brief  Set Phase 1 parameters for EAP-FAST
  *
  * @attention  1. Disabling the WPA_MBEDTLS_CRYPTO config is required to use EAP-FAST.
  *
  * @param  config: eap fast phase 1 configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: fail(out of bound arguments)
  *    - ESP_ERR_NO_MEM: fail(internal memory malloc fail)
  */
esp_err_t esp_wifi_sta_wpa2_ent_set_fast_phase1_params(esp_eap_fast_config config);

#ifdef __cplusplus
}
#endif
#endif
