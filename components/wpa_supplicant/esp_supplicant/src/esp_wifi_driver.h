/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_WIFI_DRIVER_H_
#define _ESP_WIFI_DRIVER_H_

#include "esp_err.h"
#include "esp_wifi.h"

#if CONFIG_NEWLIB_NANO_FORMAT
#define TASK_STACK_SIZE_ADD 0
#else
#define TASK_STACK_SIZE_ADD 512
#endif

#define WPA2_TASK_STACK_SIZE  (6144 + TASK_STACK_SIZE_ADD)
#define WPS_TASK_STACK_SIZE  (12288 + TASK_STACK_SIZE_ADD)

enum wpa_alg{
    WIFI_WPA_ALG_NONE   = 0,
    WIFI_WPA_ALG_WEP40  = 1,
    WIFI_WPA_ALG_TKIP   = 2,
    WIFI_WPA_ALG_CCMP   = 3,
    WIFI_WAPI_ALG_SMS4  = 4,
    WIFI_WPA_ALG_WEP104 = 5,
    WIFI_WPA_ALG_WEP    = 6,
    WIFI_WPA_ALG_IGTK   = 7,
    WIFI_WPA_ALG_PMK    = 8,
    WIFI_WPA_ALG_GCMP   = 9,
};

typedef enum {
    WIFI_APPIE_PROBEREQ = 0,
    WIFI_APPIE_ASSOC_REQ,
    WIFI_APPIE_ASSOC_RESP,
    WIFI_APPIE_WPA,
    WIFI_APPIE_RSN,
    WIFI_APPIE_WPS_PR,
    WIFI_APPIE_WPS_AR,
    WIFI_APPIE_MESH_QUICK,
    WIFI_APPIE_FREQ_ERROR,
    WIFI_APPIE_ESP_MANUFACTOR,
    WIFI_APPIE_COUNTRY,
    WIFI_APPIE_MAX,
} wifi_appie_t;

/* wifi_appie_t is in rom code and can't be changed anymore, use wifi_appie_ram_t for new app IEs */
typedef enum {
    WIFI_APPIE_RAM_BEACON = WIFI_APPIE_MAX,
    WIFI_APPIE_RAM_PROBE_RSP,
    WIFI_APPIE_RAM_STA_AUTH,
    WIFI_APPIE_RAM_AP_AUTH,
    WIFI_APPIE_RAM_MAX
} wifi_appie_ram_t;

enum {
    NONE_AUTH           = 0x01,
    WPA_AUTH_UNSPEC     = 0x02,
    WPA_AUTH_PSK        = 0x03,
    WPA2_AUTH_ENT       = 0x04,
    WPA2_AUTH_PSK       = 0x05,
    WPA_AUTH_CCKM       = 0x06,
    WPA2_AUTH_CCKM      = 0x07,
    WPA2_AUTH_PSK_SHA256= 0x08,
    WPA3_AUTH_PSK       = 0x09,
    WPA2_AUTH_ENT_SHA256= 0x0a,
    WAPI_AUTH_PSK       = 0x0b,
    WAPI_AUTH_CERT      = 0x0c,
    WPA2_AUTH_ENT_SHA384_SUITE_B = 0x0d,
    WPA2_AUTH_FT_PSK    = 0x0e,
    WPA3_AUTH_OWE       = 0x0f,
    WPA3_AUTH_PSK_EXT_KEY = 0x10,
    WPA2_AUTH_INVALID
};

typedef enum {
    WPA2_ENT_EAP_STATE_NOT_START,
    WPA2_ENT_EAP_STATE_IN_PROGRESS,
    WPA2_ENT_EAP_STATE_SUCCESS,
    WPA2_ENT_EAP_STATE_FAIL,
} wpa2_ent_eap_state_t;

struct wifi_appie {
    uint16_t   ie_len;
    uint8_t    ie_data[];
};

struct wifi_ssid {
    int len;
    uint8_t ssid[32];
};

struct wps_scan_ie {
    uint8_t    *bssid;
    uint8_t    chan;
    uint16_t   capinfo;
    uint8_t    *ssid;
    uint8_t    *wpa;
    uint8_t    *rsn;
    uint8_t    *wps;
};

typedef struct {
    int proto;
    int pairwise_cipher;
    int group_cipher;
    int key_mgmt;
    int capabilities;
    size_t num_pmkid;
    const u8 *pmkid;
    int mgmt_group_cipher;
    uint8_t rsnxe_capa;
} wifi_wpa_ie_t;

struct wpa_funcs {
    bool (*wpa_sta_init)(void);
    bool (*wpa_sta_deinit)(void);
    int (*wpa_sta_connect)(uint8_t *bssid);
    void (*wpa_sta_connected_cb)(uint8_t *bssid);
    void (*wpa_sta_disconnected_cb)(uint8_t reason_code);
    int (*wpa_sta_rx_eapol)(u8 *src_addr, u8 *buf, u32 len);
    bool (*wpa_sta_in_4way_handshake)(void);
    void *(*wpa_ap_init)(void);
    bool (*wpa_ap_deinit)(void *data);
    bool (*wpa_ap_join)(void **sm, u8 *bssid, u8 *wpa_ie, u8 wpa_ie_len, u8* rsnxe, u8 rsnxe_len, bool *pmf_enable, int subtype);
    bool (*wpa_ap_remove)(u8 *bssid);
    uint8_t *(*wpa_ap_get_wpa_ie)(uint8_t *len);
    bool (*wpa_ap_rx_eapol)(void *hapd_data, void *sm, u8 *data, size_t data_len);
    void (*wpa_ap_get_peer_spp_msg)(void *sm, bool *spp_cap, bool *spp_req);
    char *(*wpa_config_parse_string)(const char *value, size_t *len);
    int (*wpa_parse_wpa_ie)(const u8 *wpa_ie, size_t wpa_ie_len, wifi_wpa_ie_t *data);
    int (*wpa_config_bss)(u8 *bssid);
    int (*wpa_michael_mic_failure)(u16 is_unicast);
    uint8_t *(*wpa3_build_sae_msg)(uint8_t *bssid, uint32_t type, size_t *len);
    int (*wpa3_parse_sae_msg)(uint8_t *buf, size_t len, uint32_t type, uint16_t status);
    int (*wpa3_hostap_handle_auth)(uint8_t *buf, size_t len, uint32_t type, uint16_t status, uint8_t *bssid);
    int (*wpa_sta_rx_mgmt)(u8 type, u8 *frame, size_t len, u8 *sender, int8_t rssi, u8 channel, u64 current_tsf);
    void (*wpa_config_done)(void);
    uint8_t *(*owe_build_dhie)(uint16_t group);
    int (*owe_process_assoc_resp)(const u8 *rsn_ie, size_t rsn_len, const uint8_t *dh_ie, size_t dh_len);
    int (*wpa_sta_set_ap_rsnxe)(const u8 *rsnxe, size_t rsnxe_ie_len);
};

struct wpa2_funcs {
    int  (*wpa2_sm_rx_eapol)(u8 *src_addr, u8 *buf, u32 len, u8 *bssid);
    int  (*wpa2_start)(void);
    u8   (*wpa2_get_state)(void);
    int  (*wpa2_init)(void);
    void (*wpa2_deinit)(void);
};

struct wps_funcs {
    bool (*wps_parse_scan_result)(struct wps_scan_ie *scan);
    int  (*wifi_station_wps_start)(void);
    int  (*wps_sm_rx_eapol)(u8 *src_addr, u8 *buf, u32 len);
    int  (*wps_start_pending)(void);
};

typedef esp_err_t (*wifi_wpa2_fn_t)(void *);
typedef struct {
    wifi_wpa2_fn_t   fn;
    void    *param;
} wifi_wpa2_param_t;

#define IS_WPS_REGISTRAR(type) (((type)>WPS_TYPE_MAX)?(((type)<WPS_TYPE_MAX)?true:false):false)
#define IS_WPS_ENROLLEE(type)  (((type)>WPS_TYPE_DISABLE)?(((type)<WPS_TYPE_MAX)?true:false):false)

typedef enum wps_status {
    WPS_STATUS_DISABLE = 0,
    WPS_STATUS_SCANNING,
    WPS_STATUS_PENDING,
    WPS_STATUS_SUCCESS,
    WPS_STATUS_MAX,
} WPS_STATUS_t;

typedef void(*wifi_tx_cb_t)(void *);
typedef void(* eapol_txcb_t)(uint8_t *, size_t, bool);
typedef int (*wifi_ipc_fn_t)(void *);
typedef struct {
    wifi_ipc_fn_t fn;
    void *arg;
    uint32_t arg_size;
} wifi_ipc_config_t;

#define WPA_IGTK_MAX_LEN 32
typedef struct {
    uint8_t keyid[2];
    uint8_t pn[6];
    uint8_t igtk[WPA_IGTK_MAX_LEN];
} wifi_wpa_igtk_t;

typedef struct {
#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif
    wifi_interface_t ifx;
    uint8_t subtype;
    uint32_t data_len;
    uint8_t da[ETH_ALEN];
    uint8_t data[0];
} wifi_mgmt_frm_req_t;

enum key_flag {
    KEY_FLAG_MODIFY                 = BIT(0),
    KEY_FLAG_DEFAULT                = BIT(1),
    KEY_FLAG_RX                     = BIT(2),
    KEY_FLAG_TX                     = BIT(3),
    KEY_FLAG_GROUP                  = BIT(4),
    KEY_FLAG_PAIRWISE               = BIT(5),
    KEY_FLAG_PMK                    = BIT(6),
};

uint8_t *esp_wifi_ap_get_prof_pmk_internal(void);
struct wifi_ssid *esp_wifi_ap_get_prof_ap_ssid_internal(void);
uint8_t esp_wifi_ap_get_prof_authmode_internal(void);
uint8_t esp_wifi_sta_get_prof_authmode_internal(void);
uint8_t *esp_wifi_ap_get_prof_password_internal(void);
struct wifi_ssid *esp_wifi_sta_get_prof_ssid_internal(void);
uint8_t esp_wifi_sta_get_reset_param_internal(void);
uint8_t esp_wifi_sta_get_pairwise_cipher_internal(void);
uint8_t esp_wifi_sta_get_group_cipher_internal(void);
bool esp_wifi_sta_prof_is_wpa_internal(void);
int esp_wifi_get_macaddr_internal(uint8_t if_index, uint8_t *macaddr);
int esp_wifi_set_appie_internal(uint8_t type, uint8_t *ie, uint16_t len, uint8_t flag);
int esp_wifi_unset_appie_internal(uint8_t type);
struct wifi_appie *esp_wifi_get_appie_internal(uint8_t type);
void *esp_wifi_get_hostap_private_internal(void); //1
uint8_t *esp_wifi_sta_get_prof_password_internal(void);
void esp_wifi_deauthenticate_internal(u8 reason_code);
uint16_t esp_wifi_get_spp_attrubute_internal(uint8_t ifx);
bool esp_wifi_sta_is_running_internal(void);
bool esp_wifi_auth_done_internal(void);
int esp_wifi_set_ap_key_internal(int alg, const u8 *addr, int idx, u8 *key, size_t key_len);
int esp_wifi_set_sta_key_internal(int alg, u8 *addr, int key_idx, int set_tx,
                                  u8 *seq, size_t seq_len, u8 *key, size_t key_len, enum key_flag key_flag);
int  esp_wifi_get_sta_key_internal(uint8_t *ifx, int *alg, u8 *addr, int *key_idx,
                                   u8 *key, size_t key_len, enum key_flag key_flag);
bool esp_wifi_wpa_ptk_init_done_internal(uint8_t *mac);
uint8_t esp_wifi_sta_set_reset_param_internal(uint8_t reset_flag);
uint8_t esp_wifi_get_sta_gtk_index_internal(void);
int esp_wifi_register_tx_cb_internal(wifi_tx_cb_t fn, u8 id);
int esp_wifi_register_eapol_txdonecb_internal(eapol_txcb_t fn);
int esp_wifi_register_wpa_cb_internal(struct wpa_funcs *cb);
int esp_wifi_unregister_wpa_cb_internal(void);
int esp_wifi_get_assoc_bssid_internal(uint8_t *bssid);
bool esp_wifi_sta_is_ap_notify_completed_rsne_internal(void);
int esp_wifi_ap_deauth_internal(uint8_t *mac, uint32_t reason);
int esp_wifi_ipc_internal(wifi_ipc_config_t *cfg, bool sync);
int esp_wifi_register_wpa2_cb_internal(struct wpa2_funcs *cb);
int esp_wifi_unregister_wpa2_cb_internal(void);
bool esp_wifi_sta_prof_is_wpa2_internal(void);
bool esp_wifi_sta_prof_is_rsn_internal(void);
bool esp_wifi_sta_prof_is_wapi_internal(void);
esp_err_t esp_wifi_sta_wpa2_ent_disable_internal(wifi_wpa2_param_t *param);
esp_err_t esp_wifi_sta_wpa2_ent_enable_internal(wifi_wpa2_param_t *param);
esp_err_t esp_wifi_set_wpa2_ent_state_internal(wpa2_ent_eap_state_t state);
int esp_wifi_get_wps_type_internal(void);
int esp_wifi_set_wps_type_internal(uint32_t type);
int esp_wifi_get_wps_status_internal(void);
int esp_wifi_set_wps_status_internal(uint32_t status);
int esp_wifi_disarm_sta_connection_timer_internal(void);
bool esp_wifi_get_sniffer_internal(void);
int esp_wifi_set_wps_cb_internal(struct wps_funcs *wps_cb);
bool esp_wifi_enable_sta_privacy_internal(void);
uint8_t esp_wifi_get_user_init_flag_internal(void);
esp_err_t esp_wifi_internal_supplicant_header_md5_check(const char *md5);
int esp_wifi_sta_update_ap_info_internal(void);
uint8_t *esp_wifi_sta_get_ap_info_prof_pmk_internal(void);
esp_err_t esp_wifi_set_wps_start_flag_internal(bool start);
uint16_t esp_wifi_sta_pmf_enabled(void);
wifi_cipher_type_t esp_wifi_sta_get_mgmt_group_cipher(void);
int esp_wifi_set_igtk_internal(uint8_t if_index, const wifi_wpa_igtk_t *igtk);
esp_err_t esp_wifi_internal_issue_disconnect(uint8_t reason_code);
bool esp_wifi_skip_supp_pmkcaching(void);
bool esp_wifi_is_rm_enabled_internal(uint8_t if_index);
bool esp_wifi_is_btm_enabled_internal(uint8_t if_index);
esp_err_t esp_wifi_register_mgmt_frame_internal(uint32_t type, uint32_t subtype);
esp_err_t esp_wifi_send_mgmt_frm_internal(const wifi_mgmt_frm_req_t *req);
uint8_t esp_wifi_ap_get_prof_pairwise_cipher_internal(void);
esp_err_t esp_wifi_action_tx_req(uint8_t type, uint8_t channel,
                                 uint32_t wait_time_ms, const wifi_action_tx_req_t *req);
esp_err_t esp_wifi_remain_on_channel(uint8_t ifx, uint8_t type, uint8_t channel,
                                     uint32_t wait_time_ms, wifi_action_rx_cb_t rx_cb);
bool esp_wifi_is_mbo_enabled_internal(uint8_t if_index);
void esp_wifi_get_pmf_config_internal(wifi_pmf_config_t *pmf_cfg, uint8_t ifx);
bool esp_wifi_is_ft_enabled_internal(uint8_t if_index);
uint8_t esp_wifi_sta_get_config_sae_pk_internal(void);
void esp_wifi_sta_disable_sae_pk_internal(void);
void esp_wifi_sta_disable_wpa2_authmode_internal(void);
uint8_t esp_wifi_ap_get_max_sta_conn(void);
uint8_t esp_wifi_get_config_sae_pwe_h2e_internal(uint8_t ifx);
bool esp_wifi_ap_notify_node_sae_auth_done(uint8_t *mac);
bool esp_wifi_ap_is_sta_sae_reauth_node(uint8_t *mac);
uint8_t* esp_wifi_sta_get_sae_identifier_internal(void);
bool esp_wifi_eb_tx_status_success_internal(void *eb);
uint8_t* esp_wifi_sta_get_rsnxe(void);

#endif /* _ESP_WIFI_DRIVER_H_ */
