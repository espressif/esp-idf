/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_wifi_driver.h"
#include "esp_wps.h"
#include "wps/wps.h"
#include "wps/wps_attr_parse.h"

/* WPS message flag */
enum wps_msg_flag {
    WPS_MSG_FLAG_MORE = 0x01,
    WPS_MSG_FLAG_LEN = 0x02
};

enum wps_reg_sig_type {
    SIG_WPS_REG_ENABLE = 1,         //1
    SIG_WPS_REG_DISABLE,            //2
    SIG_WPS_REG_START,              //3
    SIG_WPS_REG_MAX,                //4
};

typedef struct {
    void *arg;
    int ret; /* return value */
} wps_ioctl_param_t;

#ifdef ESP_SUPPLICANT
enum wps_sm_state {
    WAIT_START,
    WPA_MESG,
    WPA_FINISH_PROCESS,
    WPA_FAIL
};
#endif /* ESP_SUPPLICANT */

/* Bssid of the discard AP which is discarded for not select reg or other reason */
struct discard_ap_list_t {
    u8 bssid[6];
};

struct wps_sm {
    u8 state;
    struct wps_config *wps_cfg;
    struct wps_context *wps_ctx;
    struct wps_data *wps;
    char identity[32];
    u8 identity_len;
    u8 ownaddr[ETH_ALEN];
    u8 bssid[ETH_ALEN];
    struct wps_credential creds[MAX_CRED_COUNT];
    u8 ap_cred_cnt;
    struct wps_device_data *dev;
    u8 uuid[WPS_UUID_LEN];
    u8 uuid_r[WPS_UUID_LEN];
    u8 current_identifier;
    bool is_wps_scan;
    u8 channel;
    u8 discover_ssid_cnt;
    bool wps_pbc_overlap;
    bool post_m8_recv;
};

#define API_MUTEX_TAKE() do {\
    if (!s_wps_api_lock) {\
        s_wps_api_lock = os_recursive_mutex_create();\
        if (!s_wps_api_lock) {\
            wpa_printf(MSG_ERROR, "wps api lock create failed");\
            return ESP_ERR_NO_MEM;\
        }\
    }\
    os_mutex_lock(s_wps_api_lock);\
} while(0)

#define API_MUTEX_GIVE() os_mutex_unlock(s_wps_api_lock)
#define DATA_MUTEX_TAKE() os_mutex_lock(s_wps_data_lock)
#define DATA_MUTEX_GIVE() os_mutex_unlock(s_wps_data_lock)

struct wps_sm *wps_sm_get(void);
int wps_station_wps_unregister_cb(void);

int wps_dev_deinit(struct wps_device_data *dev);
int wps_dev_init(void);
int wps_set_factory_info(const esp_wps_config_t *config);
struct wps_sm_funcs {
    void (*wps_sm_notify_deauth)(void);
};

struct wps_sm_funcs* wps_get_wps_sm_cb(void);

static inline int wps_get_type(void)
{
    return esp_wifi_get_wps_type_internal();
}

static inline int wps_set_type(uint32_t type)
{
    return esp_wifi_set_wps_type_internal(type);
}

static inline int wps_get_status(void)
{
    return esp_wifi_get_wps_status_internal();
}

static inline int wps_set_status(uint32_t status)
{
    return esp_wifi_set_wps_status_internal(status);
}

bool is_wps_enabled(void);
int wps_init_cfg_pin(struct wps_config *cfg);
void wifi_station_wps_eapol_start_handle(void *data, void *user_ctx);
int wifi_ap_wps_disable_internal(void);
