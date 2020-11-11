// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_BLUFI_API_H__
#define __ESP_BLUFI_API_H__

#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_err.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_BLUFI_EVENT_INIT_FINISH = 0,                     /*<! When BLUFI init complete, this event happen */
    ESP_BLUFI_EVENT_DEINIT_FINISH,                       /*<! When BLUFI deinit complete, this event happen */
    ESP_BLUFI_EVENT_SET_WIFI_OPMODE,                     /*<! When Phone set ESP32 wifi operation mode(AP/STA/AP_STA), this event happen */
    ESP_BLUFI_EVENT_BLE_CONNECT,                         /*<! When Phone connect to ESP32 with BLE, this event happen */
    ESP_BLUFI_EVENT_BLE_DISCONNECT,                      /*<! When Phone disconnect with BLE, this event happen */
    ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP,                   /*<! When Phone request ESP32's STA connect to AP, this event happen */
    ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP,              /*<! When Phone request ESP32's STA disconnect from AP, this event happen */
    ESP_BLUFI_EVENT_GET_WIFI_STATUS,                     /*<! When Phone get ESP32 wifi status, this event happen */
    ESP_BLUFI_EVENT_DEAUTHENTICATE_STA,                  /*<! When Phone deauthenticate sta from SOFTAP, this event happen */
    /* recv data */
    ESP_BLUFI_EVENT_RECV_STA_BSSID,                      /*<! When Phone send STA BSSID to ESP32 to connect, this event happen */
    ESP_BLUFI_EVENT_RECV_STA_SSID,                       /*<! When Phone send STA SSID to ESP32 to connect, this event happen */
    ESP_BLUFI_EVENT_RECV_STA_PASSWD,                     /*<! When Phone send STA PASSWORD to ESP32 to connect, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_SSID,                    /*<! When Phone send SOFTAP SSID to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD,                  /*<! When Phone send SOFTAP PASSWORD to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM,            /*<! When Phone send SOFTAP max connection number to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE,               /*<! When Phone send SOFTAP authentication mode to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL,                 /*<! When Phone send SOFTAP channel to ESP32 to start SOFTAP, this event happen */
    ESP_BLUFI_EVENT_RECV_USERNAME,                       /*<! When Phone send username to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_CA_CERT,                        /*<! When Phone send CA certificate to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_CLIENT_CERT,                    /*<! When Phone send Client certificate to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_SERVER_CERT,                    /*<! When Phone send Server certificate to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY,                /*<! When Phone send Client Private key to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY,                /*<! When Phone send Server Private key to ESP32, this event happen */
    ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE,           /*<! When Phone send Disconnect key to ESP32, this event happen */
    ESP_BLUFI_EVENT_GET_WIFI_LIST,                       /*<! When Phone send get wifi list command to ESP32, this event happen */
    ESP_BLUFI_EVENT_REPORT_ERROR,                        /*<! When Blufi report error, this event happen */
    ESP_BLUFI_EVENT_RECV_CUSTOM_DATA,                    /*<! When Phone send custom data to ESP32, this event happen */
} esp_blufi_cb_event_t;

/// BLUFI config status
typedef enum {
    ESP_BLUFI_STA_CONN_SUCCESS = 0x00,
    ESP_BLUFI_STA_CONN_FAIL    = 0x01,
} esp_blufi_sta_conn_state_t;

/// BLUFI init status
typedef enum {
    ESP_BLUFI_INIT_OK = 0,
    ESP_BLUFI_INIT_FAILED,
} esp_blufi_init_state_t;

/// BLUFI deinit status
typedef enum {
    ESP_BLUFI_DEINIT_OK = 0,
    ESP_BLUFI_DEINIT_FAILED,
} esp_blufi_deinit_state_t;

typedef enum {
    ESP_BLUFI_SEQUENCE_ERROR = 0,
    ESP_BLUFI_CHECKSUM_ERROR,
    ESP_BLUFI_DECRYPT_ERROR,
    ESP_BLUFI_ENCRYPT_ERROR,
    ESP_BLUFI_INIT_SECURITY_ERROR,
    ESP_BLUFI_DH_MALLOC_ERROR,
    ESP_BLUFI_DH_PARAM_ERROR,
    ESP_BLUFI_READ_PARAM_ERROR,
    ESP_BLUFI_MAKE_PUBLIC_ERROR,
    ESP_BLUFI_DATA_FORMAT_ERROR,
} esp_blufi_error_state_t;

/**
 * @brief BLUFI  extra information structure
 */
typedef struct {
    //station
    uint8_t sta_bssid[6];           /*!< BSSID of station interface */
    bool sta_bssid_set;             /*!< is BSSID of station interface set */
    uint8_t *sta_ssid;              /*!< SSID of station interface */
    int sta_ssid_len;               /*!< length of SSID of station interface */
    uint8_t *sta_passwd;            /*!< password of station interface */
    int sta_passwd_len;             /*!< length of password of station interface */
    uint8_t *softap_ssid;           /*!< SSID of softap interface */
    int softap_ssid_len;            /*!< length of SSID of softap interface */
    uint8_t *softap_passwd;         /*!< password of station interface */
    int softap_passwd_len;          /*!< length of password of station interface */
    uint8_t softap_authmode;        /*!< authentication mode of softap interface */
    bool softap_authmode_set;       /*!< is authentication mode of softap interface set */
    uint8_t softap_max_conn_num;    /*!< max connection number of softap interface */
    bool softap_max_conn_num_set;   /*!< is max connection number of softap interface set */
    uint8_t softap_channel;         /*!< channel of softap interface */
    bool softap_channel_set;        /*!< is channel of softap interface set */
} esp_blufi_extra_info_t;

/** @brief Description of an WiFi AP */
typedef struct {
    uint8_t ssid[33];                     /**< SSID of AP */
    int8_t  rssi;                         /**< signal strength of AP */
} esp_blufi_ap_record_t;

/**
 * @brief BLUFI callback parameters union
 */
typedef union {
    /**
	 * @brief ESP_BLUFI_EVENT_INIT_FINISH
	 */
    struct blufi_init_finish_evt_param {
        esp_blufi_init_state_t state;				/*!< Initial status */
    } init_finish;									/*!< Blufi callback param of ESP_BLUFI_EVENT_INIT_FINISH */

    /**
	 * @brief ESP_BLUFI_EVENT_DEINIT_FINISH
	 */
    struct blufi_deinit_finish_evt_param {
        esp_blufi_deinit_state_t state;				/*!< De-initial status */
    } deinit_finish;								/*!< Blufi callback param of ESP_BLUFI_EVENT_DEINIT_FINISH */

    /**
     * @brief ESP_BLUFI_EVENT_SET_WIFI_MODE
     */
    struct blufi_set_wifi_mode_evt_param {
        wifi_mode_t op_mode;                        /*!< Wifi operation mode */
    } wifi_mode;									/*!< Blufi callback param of ESP_BLUFI_EVENT_INIT_FINISH */

    /**
     * @brief ESP_BLUFI_EVENT_CONNECT
	 */
    struct blufi_connect_evt_param {
        esp_bd_addr_t remote_bda;                   /*!< Blufi Remote bluetooth device address */
        uint8_t    server_if;                       /*!< server interface */
        uint16_t   conn_id;                         /*!< Connection id */
    } connect;									    /*!< Blufi callback param of ESP_BLUFI_EVENT_CONNECT */

    /**
     * @brief ESP_BLUFI_EVENT_DISCONNECT
	 */
    struct blufi_disconnect_evt_param {
        esp_bd_addr_t remote_bda;                   /*!< Blufi Remote bluetooth device address */
    } disconnect;									/*!< Blufi callback param of ESP_BLUFI_EVENT_DISCONNECT */

    /* ESP_BLUFI_EVENT_REQ_WIFI_CONNECT */          /* No callback param */
    /* ESP_BLUFI_EVENT_REQ_WIFI_DISCONNECT */       /* No callback param */

    /**
     * @brief ESP_BLUFI_EVENT_RECV_STA_BSSID
     */
    struct blufi_recv_sta_bssid_evt_param {
        uint8_t bssid[6];                           /*!< BSSID */
    } sta_bssid;                                    /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_STA_BSSID */

    /**
     * @brief ESP_BLUFI_EVENT_RECV_STA_SSID
     */
    struct blufi_recv_sta_ssid_evt_param {
        uint8_t *ssid;                              /*!< SSID */
        int ssid_len;                               /*!< SSID length */
    } sta_ssid;                                     /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_STA_SSID */

    /**
     * @brief
     * ESP_BLUFI_EVENT_RECV_STA_PASSWD
     */
    struct blufi_recv_sta_passwd_evt_param {
        uint8_t *passwd;                            /*!< Password */
        int passwd_len;                             /*!< Password Length */
    } sta_passwd;                                   /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_STA_PASSWD */

    /**
     * @brief ESP_BLUFI_EVENT_RECV_SOFTAP_SSID
     */
    struct blufi_recv_softap_ssid_evt_param {
        uint8_t *ssid;                              /*!< SSID */
        int ssid_len;                               /*!< SSID length */
    } softap_ssid;                                  /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_SOFTAP_SSID */

    /**
     * @brief
     * ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD
     */
    struct blufi_recv_softap_passwd_evt_param {
        uint8_t *passwd;                            /*!< Password */
        int passwd_len;                             /*!< Password Length */
    } softap_passwd;                                /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD */

    /**
     * @brief ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM
     */
    struct blufi_recv_softap_max_conn_num_evt_param {
        int max_conn_num;                           /*!< SSID */
    } softap_max_conn_num;                          /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM */

    /**
     * @brief
     * ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE
     */
    struct blufi_recv_softap_auth_mode_evt_param {
        wifi_auth_mode_t auth_mode;                 /*!< Authentication mode */
    } softap_auth_mode;                             /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE */

    /**
     * @brief
     * ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL
     */
    struct blufi_recv_softap_channel_evt_param {
        uint8_t channel;                            /*!< Authentication mode */
    } softap_channel;                             /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL */

    /**
     * @brief ESP_BLUFI_EVENT_RECV_USERNAME
     */
    struct blufi_recv_username_evt_param {
        uint8_t *name;                              /*!< Username point */
        int name_len;                               /*!< Username length */
    } username;                                     /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_USERNAME*/

    /**
     * @brief ESP_BLUFI_EVENT_RECV_CA_CERT
     */
    struct blufi_recv_ca_evt_param {
        uint8_t *cert;                              /*!< CA certificate point */
        int cert_len;                               /*!< CA certificate length */
    } ca;                                           /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_CA_CERT */

    /**
     * ESP_BLUFI_EVENT_RECV_CLIENT_CERT
     */
    struct blufi_recv_client_cert_evt_param {
        uint8_t *cert;                              /*!< Client certificate point */
        int cert_len;                               /*!< Client certificate length */
    } client_cert;                                  /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_CLIENT_CERT */

    /**
     * ESP_BLUFI_EVENT_RECV_SERVER_CERT
     */
    struct blufi_recv_server_cert_evt_param {
        uint8_t *cert;                              /*!< Client certificate point */
        int cert_len;                               /*!< Client certificate length */
    } server_cert;                                  /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_SERVER_CERT */

    /**
     * ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY
     */
    struct blufi_recv_client_pkey_evt_param {
        uint8_t *pkey;                              /*!< Client Private Key point, if Client certificate not contain Key */
        int pkey_len;                               /*!< Client Private key length */
    } client_pkey;                                  /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY */
    /**
     * ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY
     */
    struct blufi_recv_server_pkey_evt_param {
        uint8_t *pkey;                              /*!< Client Private Key point, if Client certificate not contain Key */
        int pkey_len;                               /*!< Client Private key length */
    } server_pkey;                                  /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY */
    /**
     * @brief
     * ESP_BLUFI_EVENT_REPORT_ERROR
     */
    struct blufi_get_error_evt_param {
        esp_blufi_error_state_t state;              /*!< Blufi error state */
    } report_error;                                 /*!< Blufi callback param of ESP_BLUFI_EVENT_REPORT_ERROR */
    /**
     * @brief
     * ESP_BLUFI_EVENT_RECV_CUSTOM_DATA
     */
    struct blufi_recv_custom_data_evt_param {
        uint8_t *data;                              /*!< Custom data */
        uint32_t data_len;                          /*!< Custom data Length */
    } custom_data;                                  /*!< Blufi callback param of ESP_BLUFI_EVENT_RECV_CUSTOM_DATA */
} esp_blufi_cb_param_t;

/**
 * @brief BLUFI event callback function type
 * @param event : Event type
 * @param param : Point to callback parameter, currently is union type
 */
typedef void (* esp_blufi_event_cb_t)(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param);

/* security function declare */

/**
 * @brief BLUFI negotiate data handler
 * @param data : data from phone
 * @param len  : length of data from phone
 * @param output_data : data want to send to phone
 * @param output_len : length of data want to send to phone
 */
typedef void (*esp_blufi_negotiate_data_handler_t)(uint8_t *data, int len, uint8_t **output_data, int *output_len, bool *need_free);

/**
 * @brief BLUFI  encrypt the data after negotiate a share key
 * @param iv8  : initial vector(8bit), normally, blufi core will input packet sequence number
 * @param crypt_data : plain text and encrypted data, the encrypt function must support autochthonous encrypt
 * @param crypt_len  : length of plain text
 * @return  Nonnegative number is encrypted length, if error, return negative number;
 */
typedef int (* esp_blufi_encrypt_func_t)(uint8_t iv8, uint8_t *crypt_data, int cyprt_len);

/**
 * @brief BLUFI  decrypt the data after negotiate a share key
 * @param iv8  : initial vector(8bit), normally, blufi core will input packet sequence number
 * @param crypt_data : encrypted data and plain text, the encrypt function must support autochthonous decrypt
 * @param crypt_len  : length of encrypted text
 * @return  Nonnegative number is decrypted length, if error, return negative number;
 */
typedef int (* esp_blufi_decrypt_func_t)(uint8_t iv8, uint8_t *crypt_data, int crypt_len);

/**
 * @brief BLUFI  checksum
 * @param iv8  : initial vector(8bit), normally, blufi core will input packet sequence number
 * @param data : data need to checksum
 * @param len  : length of data
 */
typedef uint16_t (*esp_blufi_checksum_func_t)(uint8_t iv8, uint8_t *data, int len);

/**
 * @brief BLUFI  callback functions type
 */
typedef struct {
    esp_blufi_event_cb_t                event_cb;                   /*!< BLUFI event callback */
    esp_blufi_negotiate_data_handler_t  negotiate_data_handler;     /*!< BLUFI negotiate data function for negotiate share key */
    esp_blufi_encrypt_func_t            encrypt_func;               /*!< BLUFI encrypt data function with share key generated by negotiate_data_handler */
    esp_blufi_decrypt_func_t            decrypt_func;               /*!< BLUFI decrypt data function with share key generated by negotiate_data_handler */
    esp_blufi_checksum_func_t           checksum_func;              /*!< BLUFI check sum function (FCS) */
} esp_blufi_callbacks_t;

/**
 *
 * @brief           This function is called to receive blufi callback event
 *
 * @param[in]       callbacks: callback functions
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_register_callbacks(esp_blufi_callbacks_t *callbacks);

/**
 *
 * @brief           This function is called to initialize blufi_profile
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_profile_init(void);

/**
 *
 * @brief           This function is called to de-initialize blufi_profile
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_profile_deinit(void);

/**
 *
 * @brief           This function is called to send wifi connection report
 * @param opmode :  wifi opmode
 * @param sta_conn_state   : station is already in connection or not
 * @param softap_conn_num  : softap connection number
 * @param extra_info       : extra information, such as sta_ssid, softap_ssid and etc.
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_send_wifi_conn_report(wifi_mode_t opmode, esp_blufi_sta_conn_state_t sta_conn_state, uint8_t softap_conn_num, esp_blufi_extra_info_t *extra_info);

/**
 *
 * @brief           This function is called to send wifi list
 * @param apCount :  wifi list count
 * @param list   : wifi list
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_send_wifi_list(uint16_t apCount, esp_blufi_ap_record_t *list);

/**
 *
 * @brief           Get BLUFI profile version
 *
 * @return          Most 8bit significant is Great version, Least 8bit is Sub version
 *
 */
uint16_t esp_blufi_get_version(void);

/**
 * @brief           Close a connection  a remote device.
 *
 * @param[in]       gatts_if: GATT server access interface
 * @param[in]       conn_id: connection ID to be closed.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_blufi_close(esp_gatt_if_t gatts_if, uint16_t conn_id);

/**
 *
 * @brief           This function is called to send blufi error information
 * @param state :  error state
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_send_error_info(esp_blufi_error_state_t state);
/**
 *
 * @brief           This function is called to custom data
 * @param data :  custom data value
 * @param data_len :  the length of custom data
 *
 * @return          ESP_OK - success, other - failed
 *
 */
esp_err_t esp_blufi_send_custom_data(uint8_t *data, uint32_t data_len);
#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLUFI_API_ */
