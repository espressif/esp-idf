/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_wifi_driver.h"
#include "esp_wps.h"

/* WPS message flag */
enum wps_msg_flag {
    WPS_MSG_FLAG_MORE = 0x01,
    WPS_MSG_FLAG_LEN = 0x02
};

#ifdef USE_WPS_TASK
enum wps_sig_type {
    SIG_WPS_ENABLE = 1,         //1
    SIG_WPS_DISABLE,            //2
    SIG_WPS_START,              //3
    SIG_WPS_RX,                 //4
    SIG_WPS_TIMER_TIMEOUT,      //5
    SIG_WPS_TIMER_MSG_TIMEOUT,  //6
    SIG_WPS_TIMER_SUCCESS_CB,   //7
    SIG_WPS_TIMER_SCAN,         //8
    SIG_WPS_TIMER_EAPOL_START,  //9
    SIG_WPS_NUM,                //10
};
#endif

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
    WPA_FAIL
};
#endif /* ESP_SUPPLICANT */

#define WPS_IGNORE_SEL_REG_MAX_CNT  4

#define WPS_MAX_DIS_AP_NUM  10

/* Bssid of the discard AP which is discarded for not select reg or other reason */
struct discard_ap_list_t {
    u8 bssid[6];
};

#ifndef MAX_PASSPHRASE_LEN
#define MAX_PASSPHRASE_LEN 64
#endif

#ifndef MAX_CRED_COUNT
#define MAX_CRED_COUNT 10
#endif

#define WPS_OUTBUF_SIZE 500
struct wps_sm {
    u8 state;
    struct wps_config *wps_cfg;
    struct wps_context *wps_ctx;
    struct wps_data *wps;
    char identity[32];
    u8 identity_len;
    u8 ownaddr[ETH_ALEN];
    u8 bssid[ETH_ALEN];
    u8 ssid[MAX_CRED_COUNT][SSID_MAX_LEN];
    u8 ssid_len[MAX_CRED_COUNT];
    char key[MAX_CRED_COUNT][MAX_PASSPHRASE_LEN];
    u8 key_len[MAX_CRED_COUNT];
    u8 ap_cred_cnt;
    struct wps_device_data *dev;
    u8 uuid[16];
    u8 current_identifier;
    bool is_wps_scan;
    u8 channel;
    u8 scan_cnt;
#ifdef USE_WPS_TASK
    u8 wps_sig_cnt[SIG_WPS_NUM];
#endif
    u8 discover_ssid_cnt;
    bool ignore_sel_reg;
    struct discard_ap_list_t dis_ap_list[WPS_MAX_DIS_AP_NUM];
    u8 discard_ap_cnt;
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
int wps_start_pending(void);
int wps_sm_rx_eapol(u8 *src_addr, u8 *buf, u32 len);

int wps_dev_deinit(struct wps_device_data *dev);
int wps_dev_init(void);
int wps_set_factory_info(const esp_wps_config_t *config);

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
