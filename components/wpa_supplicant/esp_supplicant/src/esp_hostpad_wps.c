/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils/common.h"

#include "rsn_supp/wpa.h"
#include "common/eapol_common.h"
#include "utils/wpa_debug.h"
#include "common/ieee802_11_defs.h"
#include "wps/wps_i.h"
#include "wps/wps_dev_attr.h"
#include "eap_peer/eap_defs.h"
#include "eap_peer/eap_common.h"
#include "esp_wifi_driver.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_private/wifi.h"
#include "esp_wps.h"
#include "esp_wps_i.h"
#include "eap_common/eap_wsc_common.h"
#include "esp_wpas_glue.h"
#include "ap/hostapd.h"
#include "ap/ap_config.h"
#include "ap/wps_hostapd.h"

extern struct wps_sm *gWpsSm;
extern void *s_wps_api_lock;
extern void *s_wps_api_sem;
extern bool s_wps_enabled;

static int wifi_ap_wps_init(const esp_wps_config_t *config)
{
    struct wps_sm *sm = NULL;
    uint8_t mac[ETH_ALEN];
    struct wps_config cfg = {0};
    struct hostapd_data *hapd = hostapd_get_hapd_data();

    if (!hapd || gWpsSm) {
        goto _out;
    }

    wpa_printf(MSG_DEBUG, "wifi wps init");

    gWpsSm = os_zalloc(sizeof(struct wps_sm));   /* alloc Wps_sm */
    if (!gWpsSm) {
        goto _out;
    }

    sm = gWpsSm;

    esp_wifi_get_macaddr_internal(WIFI_IF_AP, mac);
    os_memcpy(sm->ownaddr, mac, ETH_ALEN);

    sm->identity_len = WSC_ID_REGISTRAR_LEN;
    os_memcpy(sm->identity, WSC_ID_REGISTRAR, sm->identity_len);

    sm->wps_ctx = os_zalloc(sizeof(struct wps_context)); /* alloc wps_ctx */
    if (!sm->wps_ctx) {
        goto _err;
    }

    if (wps_dev_init() != 0) {
        goto _err;
    }

    cfg.registrar = 1;
    cfg.wps = sm->wps_ctx;

    os_memcpy((void *)cfg.pin, config->pin, 8);
    wps_init_cfg_pin(&cfg);
    os_memcpy(cfg.wps->uuid, sm->uuid, WPS_UUID_LEN);
    if ((sm->wps = wps_init(&cfg)) == NULL) {         /* alloc wps_data */
        goto _err;
    }

    hostapd_init_wps(hapd, sm->wps, sm->wps_ctx);

    /* Report PIN */
    if (wps_get_type() == WPS_TYPE_PIN) {
        wifi_event_sta_wps_er_pin_t evt;
        os_memcpy(evt.pin_code, sm->wps->dev_password, 8);
        esp_event_post(WIFI_EVENT, WIFI_EVENT_AP_WPS_RG_PIN, &evt, sizeof(evt), OS_BLOCK);
    }

    return ESP_OK;

_err:
    if (sm->dev) {
        wps_dev_deinit(sm->dev);
        sm->dev = NULL;
    }
    if (sm->wps_ctx) {
        os_free(sm->wps_ctx);
        sm->wps_ctx = NULL;
    }
    if (sm->wps) {
        wps_deinit(sm->wps);
        sm->wps = NULL;
    }
    os_free(gWpsSm);
    gWpsSm = NULL;
    return ESP_FAIL;
_out:
    return ESP_FAIL;
}

int wifi_ap_wps_deinit(void)
{
    struct wps_sm *sm = gWpsSm;

    if (gWpsSm == NULL) {
        return ESP_FAIL;
    }

    hostapd_deinit_wps(hostapd_get_hapd_data());
    if (sm->wps) {
        sm->wps->registrar = 0;
        wps_deinit(sm->wps);
        sm->wps = NULL;
    }
    if (sm->dev) {
        wps_dev_deinit(sm->dev);
        sm->dev = NULL;
    }

    if (sm->wps_ctx) {
        os_free(sm->wps_ctx);
        sm->wps_ctx = NULL;
    }

    os_free(gWpsSm);
    gWpsSm = NULL;

    return ESP_OK;
}

int wifi_ap_wps_enable_internal(const esp_wps_config_t *config)
{
    int ret = 0;

    wpa_printf(MSG_DEBUG, "ESP WPS crypto initialize!");
    if (config->wps_type == WPS_TYPE_DISABLE) {
        wpa_printf(MSG_ERROR, "wps enable: invalid wps type");
        return ESP_ERR_WIFI_WPS_TYPE;
    }

    wpa_printf(MSG_DEBUG, "Set factory information.");
    ret = wps_set_factory_info(config);
    if (ret != 0) {
        return ret;
    }

    wpa_printf(MSG_INFO, "wifi_wps_enable");

    wps_set_type(config->wps_type);
    wps_set_status(WPS_STATUS_DISABLE);

    ret = wifi_ap_wps_init(config);

    if (ret != 0) {
        wps_set_type(WPS_STATUS_DISABLE);
        wps_set_status(WPS_STATUS_DISABLE);
        return ESP_FAIL;
    }

    return ESP_OK;
}

int esp_wifi_ap_wps_enable(const esp_wps_config_t *config)
{
    int ret = ESP_OK;
    struct wps_sm *sm = gWpsSm;
    wifi_mode_t mode = WIFI_MODE_NULL;

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "wps enable: wifi not started cannot enable wpsreg");
        return ESP_ERR_WIFI_STATE;
    }

    ret = esp_wifi_get_mode(&mode);
    if (mode != WIFI_MODE_AP && mode != WIFI_MODE_APSTA) {
        wpa_printf(MSG_ERROR, "wps enable: mode=%d does not include AP", mode);
        return ESP_ERR_WIFI_MODE;
    }

    if (esp_wifi_ap_get_prof_authmode_internal() == WIFI_AUTH_OPEN) {
        wpa_printf(MSG_ERROR, "wps enable: wpsreg not supported when authmode is open");
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();
    if (s_wps_enabled) {
        if (sm && os_memcmp(sm->identity, WSC_ID_ENROLLEE, sm->identity_len) == 0) {
            wpa_printf(MSG_ERROR, "wps enable: wps enrollee already enabled cannot enable wpsreg");
            ret = ESP_ERR_WIFI_MODE;
        } else {
            wpa_printf(MSG_DEBUG, "wps enable: already enabled");
            ret = ESP_OK;
        }
        API_MUTEX_GIVE();
        return ret;
    }

    ret = wifi_ap_wps_enable_internal(config);
    s_wps_enabled = true;
    API_MUTEX_GIVE();
    return ret;
}

int esp_wifi_ap_wps_disable(void)
{
    int ret = 0;
    struct wps_sm *sm = gWpsSm;

    if (sm && os_memcmp(sm->identity, WSC_ID_ENROLLEE, sm->identity_len) == 0) {
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();

    if (!s_wps_enabled) {
        wpa_printf(MSG_DEBUG, "wps disable: already disabled");
        API_MUTEX_GIVE();
        return ESP_OK;
    }

    wpa_printf(MSG_INFO, "wifi_wps_disable");
    wps_set_type(WPS_TYPE_DISABLE);
    wps_set_status(WPS_STATUS_DISABLE);

    wifi_ap_wps_deinit();

    if (ESP_OK != ret) {
        wpa_printf(MSG_ERROR, "wps disable: failed to disable wps, ret=%d", ret);
    }

    s_wps_enabled = false;
    API_MUTEX_GIVE();
    return ESP_OK;
}

int esp_wifi_ap_wps_start(const unsigned char *pin)
{
    wifi_mode_t mode = WIFI_MODE_NULL;

    esp_wifi_get_mode(&mode);
    if (mode != WIFI_MODE_AP && mode != WIFI_MODE_APSTA) {
        wpa_printf(MSG_ERROR, "wps start: mode=%d does not include AP", mode);
        return ESP_ERR_WIFI_MODE;
    }

    API_MUTEX_TAKE();

    if (!s_wps_enabled) {
        wpa_printf(MSG_ERROR, "wps start: wps not enabled");
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_WPS_SM;
    }

    if (wps_get_type() == WPS_TYPE_DISABLE || (wps_get_status() != WPS_STATUS_DISABLE && wps_get_status() != WPS_STATUS_SCANNING)) {
        wpa_printf(MSG_ERROR, "wps start: wps_get_type=%d wps_get_status=%d", wps_get_type(), wps_get_status());
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_WPS_TYPE;
    }

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "wps start: esp_wifi_get_user_init_flag_internal=%d", esp_wifi_get_user_init_flag_internal());
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_STATE;
    }

    if (!pin) {
        pin = gWpsSm->wps->dev_password;
    }
    /* TODO ideally SoftAP mode should also do a single scan in PBC mode
     * however softAP scanning is not available at the moment */
    wps_set_status(WPS_STATUS_PENDING);
    if (wps_get_type() == WPS_TYPE_PBC) {
        hostapd_wps_button_pushed(hostapd_get_hapd_data(), NULL);
    } else if (wps_get_type() == WPS_TYPE_PIN) {
        hostapd_wps_add_pin(hostapd_get_hapd_data(), pin);
    }
    API_MUTEX_GIVE();
    return ESP_OK;
}
