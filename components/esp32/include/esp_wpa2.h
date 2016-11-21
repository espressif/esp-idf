#ifndef ESP_WPA2_H
#define ESP_WPA2_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Set wpa2 enterprise authentication.
  *
  * @attention wpa2 enterprise authentication can only be used when ESP8266 station is enabled.
  *            wpa2 enterprise authentication can only support PEAP-MSCHAPv2 and TTLS-MSCHAPv2 method.
  *
  * @param  enable
  *    - 1: enable wpa2 enterprise authentication;
  *    - 0: disable wpa2 enterprise authentication
  *
  * @return  0: succeed
            -1: fail
  */
esp_err_t esp_wifi_sta_set_wpa2_enterprise_auth(int enable);

/**
  * @brief  Set username for PEAP/TTLS method.
  *
  * @param  username: point to address where stores the username;
  *         len: length of username, limited to 1~127
  *
  * @return  0: succeed
  *         -1: fail(len <= 0 or len >= 128)
  *         -2: fail(internal memory malloc fail)
  */
esp_err_t esp_wifi_sta_set_enterprise_username(unsigned char *username, int len);

/**
  * @brief  Set password for PEAP/TTLS method..
  *
  * @param  password: point to address where stores the password;
  *         len: length of password
  *
  * @return  0: succeed
  *         -1: fail(len <= 0)
  *         -2: fail(internal memory malloc fail)
  */
esp_err_t esp_wifi_sta_set_enterprise_password(unsigned char *password, int len);

/**
  * @brief  Set CA certificate for PEAP/TTLS method.
  *
  * @param  ca_cert: point to address where stores the CA certificate;
  *         len: length of ca_cert
  *
  * @return 0: succeed
  */
esp_err_t esp_wifi_sta_set_enterprise_ca_cert(unsigned char *ca_cert, int len);

/**
  * @brief  Clear username for PEAP/TTLS method.
  *
  * @param  null
  *
  * @return  null
  */
void esp_wifi_sta_clear_enterprise_username(void);

/**
  * @brief  Clear password for PEAP/TTLS method..
  *
  * @param  null
  *
  * @return  null
  */
void esp_wifi_sta_clear_enterprise_password(void);

/**
  * @brief  Clear CA certificate for PEAP/TTLS method.
  *
  * @param  null
  *
  * @return  null
  */
void esp_wifi_sta_clear_enterprise_ca_cert(void);

/**
  * @brief  Set client certificate and key.
  *
  * @param  client_cert: point to address where stores the client certificate;
  *         client_cert_len: length of client certificate;
  *         private_key: point to address where stores the private key;
  *         private_key_len: length of private key;
  *         private_key_password: point to address where stores the private key password;
  *         private_key_password_len: length of private key password;
  *
  * @return 0: succeed
  */
esp_err_t esp_wifi_sta_set_enterprise_cert_key(unsigned char *client_cert, int client_cert_len, unsigned char *private_key, int private_key_len, unsigned char *private_key_passwd, int private_key_passwd_len);

/**
  * @brief  Clear client certificate and key.
  *
  * @param  null
  *
  * @return  null
  */
void esp_wifi_sta_clear_enterprise_cert_key(void);

#ifdef __cplusplus
}
#endif
#endif
