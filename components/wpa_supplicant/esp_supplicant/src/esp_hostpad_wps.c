/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
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
#include "utils/eloop.h"

extern struct wps_sm *gWpsSm;
extern void *s_wps_api_lock;
extern void *s_wps_api_sem;
extern bool s_wps_enabled;

static int wps_reg_eloop_post_block(uint32_t sig, void *arg);

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

static int wifi_ap_wps_enable_internal(const esp_wps_config_t *config)
{
    struct wps_sm *sm = gWpsSm;
    wifi_mode_t mode = WIFI_MODE_NULL;

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "wps enable: wifi not started cannot enable wpsreg");
        return ESP_ERR_WIFI_STATE;
    }

    if (esp_wifi_get_mode(&mode) != ESP_OK) {
        wpa_printf(MSG_ERROR, "wps enable: unable to get current wifi mode");
        return ESP_FAIL;
    }

    if (mode != WIFI_MODE_AP && mode != WIFI_MODE_APSTA) {
        wpa_printf(MSG_ERROR, "wps enable: mode=%d does not include AP", mode);
        return ESP_ERR_WIFI_MODE;
    }

    if (esp_wifi_ap_get_prof_authmode_internal() == WIFI_AUTH_OPEN) {
        wpa_printf(MSG_ERROR, "wps enable: wpsreg not supported when authmode is open");
        return ESP_ERR_WIFI_MODE;
    }

    if (s_wps_enabled) {
        if (sm && os_memcmp(sm->identity, WSC_ID_ENROLLEE, sm->identity_len) == 0) {
            wpa_printf(MSG_ERROR, "wps enable: wps enrollee already enabled cannot enable wpsreg");
            return ESP_ERR_WIFI_MODE;
        } else {
            wpa_printf(MSG_DEBUG, "wps enable: already enabled");
            return ESP_OK;
        }
    }

    if (config->wps_type == WPS_TYPE_DISABLE) {
        wpa_printf(MSG_ERROR, "wps enable: invalid wps type");
        return ESP_ERR_WIFI_WPS_TYPE;
    }

    wpa_printf(MSG_DEBUG, "Set factory information.");
    if (wps_set_factory_info(config) != ESP_OK) {
        return ESP_FAIL;
    }

    if (wps_set_type(config->wps_type) != ESP_OK) {
        goto _err;
    }

    if (wps_set_status(WPS_STATUS_DISABLE) != ESP_OK) {
        goto _err;
    }

    if (wifi_ap_wps_init(config) != ESP_OK) {
        goto _err;
    }

    wpa_printf(MSG_INFO, "wifi_wps_enable");
    s_wps_enabled = true;
    return ESP_OK;

_err:
    wpa_printf(MSG_ERROR, "failure in wifi_wps_enable");
    wps_set_type(WPS_TYPE_DISABLE);
    wps_set_status(WPS_STATUS_DISABLE);

    return ESP_FAIL;
}

int esp_wifi_ap_wps_enable(const esp_wps_config_t *config)
{
    int ret = ESP_OK;

    API_MUTEX_TAKE();
    ret = wps_reg_eloop_post_block(SIG_WPS_REG_ENABLE, (void *) config);
    API_MUTEX_GIVE();
    return ret;
}

int wifi_ap_wps_disable_internal(void)
{
    struct wps_sm *sm = gWpsSm;

    if (sm && os_memcmp(sm->identity, WSC_ID_ENROLLEE, sm->identity_len) == 0) {
        return ESP_ERR_WIFI_MODE;
    }

    if (!s_wps_enabled) {
        wpa_printf(MSG_DEBUG, "wps disable: already disabled");
        return ESP_OK;
    }

    wpa_printf(MSG_INFO, "wifi_wps_disable");
    if (wps_set_type(WPS_TYPE_DISABLE) != ESP_OK) {
        goto _err;
    }

    if (wps_set_status(WPS_STATUS_DISABLE) != ESP_OK) {
        goto _err;
    }

    if (wifi_ap_wps_deinit() != ESP_OK) {
        goto _err;
    }

    s_wps_enabled = false;
    return ESP_OK;

_err:
    wpa_printf(MSG_ERROR, "wps disable: failed to disable wps");
    return ESP_FAIL;
}

int esp_wifi_ap_wps_disable(void)
{
    int ret = ESP_FAIL;
    API_MUTEX_TAKE();
    ret = wps_reg_eloop_post_block(SIG_WPS_REG_DISABLE, NULL);
    API_MUTEX_GIVE();
    return ret;
}

static int wifi_ap_wps_start_internal(const unsigned char *pin)
{
    wifi_mode_t mode = WIFI_MODE_NULL;

    esp_wifi_get_mode(&mode);
    if (mode != WIFI_MODE_AP && mode != WIFI_MODE_APSTA) {
        wpa_printf(MSG_ERROR, "wps start: mode=%d does not include AP", mode);
        return ESP_ERR_WIFI_MODE;
    }

    if (!s_wps_enabled) {
        wpa_printf(MSG_ERROR, "wps start: wps not enabled");
        API_MUTEX_GIVE();
        return ESP_ERR_WIFI_WPS_SM;
    }

    if (wps_get_type() == WPS_TYPE_DISABLE ||
            (wps_get_status() != WPS_STATUS_DISABLE &&
             wps_get_status() != WPS_STATUS_SCANNING)) {
        wpa_printf(MSG_ERROR, "wps start: wps_get_type=%d wps_get_status=%d",
                   wps_get_type(), wps_get_status());
        return ESP_ERR_WIFI_WPS_TYPE;
    }

    if (esp_wifi_get_user_init_flag_internal() == 0) {
        wpa_printf(MSG_ERROR, "wps start: esp_wifi_get_user_init_flag_internal=%d",
                   esp_wifi_get_user_init_flag_internal());
        return ESP_ERR_WIFI_STATE;
    }

    if (!pin) {
        pin = gWpsSm->wps->dev_password;
    }

    /* TODO ideally SoftAP mode should also do a single scan in PBC mode
     * however softAP scanning is not available at the moment */
    if (wps_set_status(WPS_STATUS_PENDING) != ESP_OK) {
        return ESP_FAIL;
    }
    if (wps_get_type() == WPS_TYPE_PBC) {
        if (hostapd_wps_button_pushed(hostapd_get_hapd_data(), NULL) != ESP_OK) {
            return ESP_FAIL;
        }
    } else if (wps_get_type() == WPS_TYPE_PIN) {
        if (hostapd_wps_add_pin(hostapd_get_hapd_data(), pin) != ESP_OK) {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

int esp_wifi_ap_wps_start(const unsigned char *pin)
{
    int ret = ESP_FAIL;
    API_MUTEX_TAKE();
    ret = wps_reg_eloop_post_block(SIG_WPS_REG_START, (void *)pin);
    API_MUTEX_GIVE();
    return ret;
}

static void wps_reg_eloop_handler(void *eloop_ctx, void *user_ctx)
{
    int ret = ESP_FAIL;
    enum wps_reg_sig_type *sig = (enum wps_reg_sig_type *) eloop_ctx;
    wps_ioctl_param_t *param = (wps_ioctl_param_t *) user_ctx;

    switch (*sig) {
    case SIG_WPS_REG_ENABLE:
        esp_wps_config_t *config = (esp_wps_config_t *)param->arg;
        ret = wifi_ap_wps_enable_internal(config);
        break;
    case SIG_WPS_REG_START:
        unsigned char *pin = (unsigned char *)param->arg;
        ret = wifi_ap_wps_start_internal((const unsigned char *)pin);
        break;
    case SIG_WPS_REG_DISABLE:
        ret = wifi_ap_wps_disable_internal();
        break;
    default:
        wpa_printf(MSG_WARNING, "%s(): invalid signal type=%d", __func__, *sig);
        ret = ESP_FAIL;
        break;
    }

    param->ret = ret;
    os_semphr_give(s_wps_api_sem);
}

static int wps_reg_eloop_post_block(uint32_t sig, void *arg)
{
    int ret = ESP_FAIL;
    wps_ioctl_param_t param;
    param.ret = ESP_FAIL;
    param.arg = arg;

    if (s_wps_api_sem == NULL) {
        s_wps_api_sem = os_semphr_create(1, 0);
        if (s_wps_api_sem == NULL) {
            wpa_printf(MSG_ERROR, "%s(): failed to create WPA API semaphore", __func__);
            return ESP_ERR_NO_MEM;
        }
    }

    eloop_register_timeout(0, 0, wps_reg_eloop_handler, (void *)&sig, (void *)&param);

    if (TRUE == os_semphr_take(s_wps_api_sem, OS_BLOCK)) {
        ret = param.ret;
    } else {
        ret = ESP_FAIL;
    }

    return ret;
}
