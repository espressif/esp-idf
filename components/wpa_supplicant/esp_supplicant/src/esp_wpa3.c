/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef CONFIG_WPA3_SAE

#include "common/sae.h"
#include "common/ieee802_11_defs.h"
#include "esp_wifi_driver.h"
#include "rsn_supp/wpa.h"
#include "ap/hostapd.h"
#include "ap/ieee802_11.h"
#include "ap/sta_info.h"
#include "esp_wpa3_i.h"
#include "endian.h"
#include "esp_hostap.h"
#include <inttypes.h>
#include "common/defs.h"

#ifdef CONFIG_SAE_H2E
static struct sae_pt *g_sae_pt;
#endif /* CONFIG_SAE_H2E */
static struct sae_data g_sae_data;
static struct wpabuf *g_sae_token = NULL;
static struct wpabuf *g_sae_commit = NULL;
static struct wpabuf *g_sae_confirm = NULL;
int g_allowed_groups[] = { IANA_SECP256R1, 0 };

static esp_err_t wpa3_build_sae_commit(u8 *bssid, size_t *sae_msg_len)
{
    int default_group = IANA_SECP256R1;
    u32 len = 0;
    u8 own_addr[ETH_ALEN];
    const u8 *pw = (const u8 *)esp_wifi_sta_get_prof_password_internal();
    char sae_pwd_id[SAE_H2E_IDENTIFIER_LEN + 1] = {0};
    bool valid_pwd_id = false;
#ifdef CONFIG_SAE_H2E
    uint8_t sae_pwe = esp_wifi_get_config_sae_pwe_h2e_internal(WIFI_IF_STA);
    const u8 *rsnxe = NULL;
    uint8_t use_pt = 0;
    struct wifi_ssid *ssid = esp_wifi_sta_get_prof_ssid_internal();
    u8 rsnxe_capa = 0;

    if (wpa_key_mgmt_sae_ext_key(gWpaSm.key_mgmt)) {
        use_pt = 1;
    }

#ifdef CONFIG_WPA3_COMPAT
    if (esp_wifi_is_wpa3_compatible_mode_enabled(WIFI_IF_STA)) {
        rsnxe = esp_wifi_sta_get_ie((u8*)bssid, WFA_RSNXE_OVERRIDE_OUI_TYPE);
        if (rsnxe) {
            rsnxe_capa = rsnxe[2 + 4];
        }
    }
#endif
    if (!rsnxe) {
        rsnxe = esp_wifi_sta_get_ie((u8*)bssid, WLAN_EID_RSNX);
        if (rsnxe) {
            rsnxe_capa = rsnxe[2];
        }
    }
#endif /* CONFIG_SAE_H2E */

#ifdef CONFIG_SAE_PK
    bool use_pk = false;
    uint8_t sae_pk_mode = esp_wifi_sta_get_config_sae_pk_internal();
    if ((rsnxe_capa & BIT(WLAN_RSNX_CAPAB_SAE_PK)) &&
            sae_pk_mode != WPA3_SAE_PK_MODE_DISABLED &&
            ((pw && sae_pk_valid_password((const char *)pw)))) {
        use_pt = 1;
        use_pk = true;
    }

    if (sae_pk_mode == WPA3_SAE_PK_MODE_ONLY && !use_pk) {
        wpa_printf(MSG_DEBUG,
                   "SAE: Cannot use PK with the selected AP");
        return ESP_FAIL;
    }
#endif /* CONFIG_SAE_PK */
#ifdef CONFIG_SAE_H2E
    if (use_pt || sae_pwe == SAE_PWE_HASH_TO_ELEMENT ||
            sae_pwe == SAE_PWE_BOTH) {
        use_pt = !!(rsnxe_capa & BIT(WLAN_RSNX_CAPAB_SAE_H2E));

        if ((sae_pwe == SAE_PWE_HASH_TO_ELEMENT ||
                wpa_key_mgmt_sae_ext_key(gWpaSm.key_mgmt)
#ifdef CONFIG_SAE_PK
                || (use_pk && sae_pk_mode == WPA3_SAE_PK_MODE_ONLY)
#endif /* CONFIG_SAE_PK */
            ) && !use_pt) {
            wpa_printf(MSG_DEBUG,
                       "SAE: Cannot use H2E with the selected AP");
            return ESP_FAIL;
        }
    }

    if (use_pt != 0) {
        memcpy(sae_pwd_id, esp_wifi_sta_get_sae_identifier_internal(), SAE_H2E_IDENTIFIER_LEN);
        if (os_strlen(sae_pwd_id) > 0) {
            valid_pwd_id = true;
        }
    }

    if (use_pt && !g_sae_pt) {
        g_sae_pt = sae_derive_pt(g_allowed_groups, ssid->ssid, ssid->len, pw, strlen((const char *)pw), valid_pwd_id ? sae_pwd_id : NULL);
    }
#endif /* CONFIG_SAE_H2E */

    if (wpa_sta_cur_pmksa_matches_akm()) {
        wpa_printf(MSG_INFO, "wpa3: Skip SAE and use cached PMK instead");
        *sae_msg_len = 0;
        return ESP_FAIL;
    }

    if (g_sae_commit) {
        wpabuf_free(g_sae_commit);
        g_sae_commit = NULL;
    }

    if (g_sae_token) {
        len = wpabuf_len(g_sae_token);
        goto reuse_data;
    }

    memset(&g_sae_data, 0, sizeof(g_sae_data));
    if (sae_set_group(&g_sae_data, default_group)) {
        wpa_printf(MSG_ERROR, "wpa3: could not set SAE group %d", default_group);
        return ESP_FAIL;
    }

    g_sae_data.akmp = gWpaSm.key_mgmt;

    esp_wifi_get_macaddr_internal(WIFI_IF_STA, own_addr);
    if (!bssid) {
        wpa_printf(MSG_ERROR, "wpa3: cannot prepare SAE commit with no BSSID!");
        return ESP_FAIL;
    }

#ifdef CONFIG_SAE_H2E
    if (use_pt &&
            sae_prepare_commit_pt(&g_sae_data, g_sae_pt,
                                  own_addr, bssid, NULL, NULL) < 0) {
        wpa_printf(MSG_ERROR, "wpa3: failed to prepare SAE commit!");
        return ESP_FAIL;
    }
#endif /* CONFIG_SAE_H2E */
    if (
#ifdef CONFIG_SAE_H2E
        !use_pt &&
#endif /* CONFIG_SAE_H2E */
        sae_prepare_commit(own_addr, bssid, pw,
                           strlen((const char *)pw),
                           &g_sae_data) < 0) {
        wpa_printf(MSG_ERROR, "wpa3: failed to prepare SAE commit!");
        return ESP_FAIL;
    }

#ifdef CONFIG_SAE_PK
    if (g_sae_data.tmp && use_pt && use_pk) {
        g_sae_data.pk = 1;
        os_memcpy(g_sae_data.tmp->own_addr, own_addr, ETH_ALEN);
        os_memcpy(g_sae_data.tmp->peer_addr, bssid, ETH_ALEN);
        sae_pk_set_password(&g_sae_data, (const char*) pw);
    }
#endif

reuse_data:
    len += SAE_COMMIT_MAX_LEN;
    g_sae_commit = wpabuf_alloc(len);
    if (!g_sae_commit) {
        wpa_printf(MSG_ERROR, "wpa3: failed to allocate buffer for commit msg");
        return ESP_FAIL;
    }

    if (sae_write_commit(&g_sae_data, g_sae_commit, g_sae_token, valid_pwd_id ? sae_pwd_id : NULL) != ESP_OK) {
        wpa_printf(MSG_ERROR, "wpa3: failed to write SAE commit msg");
        wpabuf_free(g_sae_commit);
        g_sae_commit = NULL;
        return ESP_FAIL;
    }

    if (g_sae_token) {
        wpabuf_free(g_sae_token);
        g_sae_token = NULL;
    }
    g_sae_data.state = SAE_COMMITTED;

    return ESP_OK;
}

static esp_err_t wpa3_build_sae_confirm(void)
{
    if (g_sae_data.state != SAE_COMMITTED) {
        return ESP_FAIL;
    }

    if (g_sae_confirm) {
        wpabuf_free(g_sae_confirm);
        g_sae_confirm = NULL;
    }

    g_sae_confirm = wpabuf_alloc(SAE_COMMIT_MAX_LEN);
    if (!g_sae_confirm) {
        wpa_printf(MSG_ERROR, "wpa3: failed to allocate buffer for confirm msg");
        return ESP_FAIL;
    }

    if (sae_write_confirm(&g_sae_data, g_sae_confirm) != ESP_OK) {
        wpa_printf(MSG_ERROR, "wpa3: failed to write SAE confirm msg");
        wpabuf_free(g_sae_confirm);
        g_sae_confirm = NULL;
        return ESP_FAIL;
    }
    g_sae_data.state = SAE_CONFIRMED;

    return ESP_OK;
}

void esp_wpa3_free_sae_data(void)
{
    if (g_sae_commit) {
        wpabuf_free(g_sae_commit);
        g_sae_commit = NULL;
    }

    if (g_sae_confirm) {
        wpabuf_free(g_sae_confirm);
        g_sae_confirm = NULL;
    }
    sae_clear_data(&g_sae_data);
#ifdef CONFIG_SAE_H2E
    if (g_sae_pt) {
        sae_deinit_pt(g_sae_pt);
        g_sae_pt = NULL;
    }
#endif /* CONFIG_SAE_H2E */
}

static u8 *wpa3_build_sae_msg(u8 *bssid, u32 sae_msg_type, size_t *sae_msg_len)
{
    u8 *buf = NULL;

    switch (sae_msg_type) {
    case SAE_MSG_COMMIT:
        /* Do not go for SAE when WPS is ongoing */
        if (esp_wifi_get_wps_status_internal() != WPS_STATUS_DISABLE) {
            *sae_msg_len = 0;
            return NULL;
        }
        if (ESP_OK != wpa3_build_sae_commit(bssid, sae_msg_len)) {
            return NULL;
        }
        *sae_msg_len = wpabuf_len(g_sae_commit);
        buf = wpabuf_mhead_u8(g_sae_commit);
        break;
    case SAE_MSG_CONFIRM:
        if (ESP_OK != wpa3_build_sae_confirm()) {
            return NULL;
        }
        *sae_msg_len = wpabuf_len(g_sae_confirm);
        buf = wpabuf_mhead_u8(g_sae_confirm);
        break;
    default:
        break;
    }

    return buf;
}

static int wpa3_sae_is_group_enabled(int group)
{
    int *groups = NULL;
    int default_groups[] = { 19, 0 };
    int i;

    if (!groups) {
        groups = default_groups;
    }

    for (i = 0; groups[i] > 0; i++) {
        if (groups[i] == group) {
            return 1;
        }
    }

    return 0;
}

static int wpa3_check_sae_rejected_groups(const struct wpabuf *groups)
{
    size_t i, count, len;
    const u8 *pos;

    if (!groups) {
        return 0;
    }

    pos = wpabuf_head(groups);
    len = wpabuf_len(groups);
    if (len & 1) {
        wpa_printf(MSG_DEBUG,
                   "SAE: Invalid length of the Rejected Groups element payload: %zu",
                   len);
        return 1;
    }
    count = len / 2;
    for (i = 0; i < count; i++) {
        int enabled;
        u16 group;

        group = WPA_GET_LE16(pos);
        pos += 2;
        enabled = wpa3_sae_is_group_enabled(group);
        wpa_printf(MSG_DEBUG, "SAE: Rejected group %u is %s",
                   group, enabled ? "enabled" : "disabled");
        if (enabled) {
            return 1;
        }
    }

    return 0;
}

static int wpa3_parse_sae_commit(u8 *buf, u32 len, u16 status)
{
    int ret;

    if (g_sae_data.state != SAE_COMMITTED) {
        wpa_printf(MSG_DEBUG, "wpa3: Discarding commit frame received in state %d", g_sae_data.state);
        return ESP_ERR_WIFI_DISCARD;
    }

    if (status == WLAN_STATUS_ANTI_CLOGGING_TOKEN_REQ) {
        if (g_sae_token) {
            wpabuf_free(g_sae_token);
        }
        if (g_sae_data.h2e) {
            if ((buf[2] != WLAN_EID_EXTENSION) ||
                    (buf[3] == 0) ||
                    (buf[3] > len - 4) ||
                    (buf[4] != WLAN_EID_EXT_ANTI_CLOGGING_TOKEN)) {
                wpa_printf(MSG_ERROR, "Invalid SAE anti-clogging token container header");
                return ESP_FAIL;
            }
            if (len < 5) {
                wpa_printf(MSG_ERROR, "Invalid SAE anti-clogging token length");
                return ESP_FAIL;
            }
            g_sae_token = wpabuf_alloc_copy(buf + 5, len - 5);
        } else {
            if (len < 2) {
                wpa_printf(MSG_ERROR, "Invalid SAE anti-clogging token length");
                return ESP_FAIL;
            }
            g_sae_token = wpabuf_alloc_copy(buf + 2, len - 2);
        }
        return ESP_OK;
    }

    ret = sae_parse_commit(&g_sae_data, buf, len, NULL, 0, g_allowed_groups,
                           (status == WLAN_STATUS_SAE_HASH_TO_ELEMENT || status == WLAN_STATUS_SAE_PK));
    if (ret == SAE_SILENTLY_DISCARD) {
        wpa_printf(MSG_DEBUG, "wpa3: Discarding commit frame due to reflection attack");
        return ESP_ERR_WIFI_DISCARD;
    } else if (ret) {
        wpa_printf(MSG_ERROR, "wpa3: could not parse commit(%d)", ret);
        return ret;
    }
    if (g_sae_data.tmp && wpa3_check_sae_rejected_groups(g_sae_data.tmp->peer_rejected_groups)) {
        return -1;
    }

    ret = sae_process_commit(&g_sae_data);
    if (ret) {
        wpa_printf(MSG_ERROR, "wpa3: could not process commit(%d)", ret);
        return ret;
    }

    return ESP_OK;
}

static int wpa3_parse_sae_confirm(u8 *buf, u32 len)
{
    if (g_sae_data.state != SAE_CONFIRMED) {
        wpa_printf(MSG_ERROR, "wpa3: failed to parse SAE commit in state(%d)!",
                   g_sae_data.state);
        return ESP_FAIL;
    }

    if (sae_check_confirm(&g_sae_data, buf, len) != ESP_OK) {
        wpa_printf(MSG_ERROR, "wpa3: failed to parse SAE confirm");
        return ESP_FAIL;
    }
    g_sae_data.state = SAE_ACCEPTED;

    wpa_set_pmk(g_sae_data.pmk, g_sae_data.pmk_len, g_sae_data.pmkid, true);

    return ESP_OK;
}

static int wpa3_parse_sae_msg(u8 *buf, size_t len, u32 sae_msg_type, u16 status)
{
    int ret = ESP_OK;

    switch (sae_msg_type) {
    case SAE_MSG_COMMIT:
        ret = wpa3_parse_sae_commit(buf, len, status);
        break;
    case SAE_MSG_CONFIRM:
        ret = wpa3_parse_sae_confirm(buf, len);
        esp_wpa3_free_sae_data();
        break;
    default:
        wpa_printf(MSG_ERROR, "wpa3: Invalid SAE msg type(%" PRId32 ")!", sae_msg_type);
        ret = ESP_FAIL;
        break;
    }

    return ret;
}

void esp_wifi_register_wpa3_cb(struct wpa_funcs *wpa_cb)
{
    wpa_cb->wpa3_build_sae_msg = wpa3_build_sae_msg;
    wpa_cb->wpa3_parse_sae_msg = wpa3_parse_sae_msg;
}

void esp_wifi_unregister_wpa3_cb(void)
{
    extern struct wpa_funcs *wpa_cb;

    wpa_cb->wpa3_build_sae_msg = NULL;
    wpa_cb->wpa3_parse_sae_msg = NULL;

}
#endif /* CONFIG_WPA3_SAE */

#ifdef CONFIG_SAE

static TaskHandle_t g_wpa3_hostap_task_hdl = NULL;
static QueueHandle_t g_wpa3_hostap_evt_queue = NULL;
SemaphoreHandle_t g_wpa3_hostap_auth_api_lock = NULL;

int wpa3_hostap_post_evt(uint32_t evt_id, uint32_t data)
{
    wpa3_hostap_auth_event_t evt;

    evt.id = evt_id;
    evt.data = data;

    if (g_wpa3_hostap_auth_api_lock) {
        WPA3_HOSTAP_AUTH_API_LOCK();
        if (g_wpa3_hostap_evt_queue == NULL) {
            WPA3_HOSTAP_AUTH_API_UNLOCK();
            wpa_printf(MSG_DEBUG, "hostap evt queue NULL");
            return ESP_FAIL;
        }
    } else {
        wpa_printf(MSG_DEBUG, "g_wpa3_hostap_auth_api_lock not found");
        return ESP_FAIL;
    }
    if (evt.id == SIG_WPA3_RX_CONFIRM || evt.id == SIG_TASK_DEL) {
        /* prioritising confirm for completing handshake for committed sta */
        if (os_queue_send_to_front(g_wpa3_hostap_evt_queue, &evt, 0) != pdPASS) {
            WPA3_HOSTAP_AUTH_API_UNLOCK();
            wpa_printf(MSG_DEBUG, "failed to add msg to queue front");
            return ESP_FAIL;
        }
    } else {
        if (os_queue_send(g_wpa3_hostap_evt_queue, &evt, 0) != pdPASS) {
            WPA3_HOSTAP_AUTH_API_UNLOCK();
            wpa_printf(MSG_DEBUG, "failed to send msg to queue");
            return ESP_FAIL;
        }
    }
    if (evt_id != SIG_TASK_DEL) {
        /* For SIG_TASK_DEL, WPA3_HOSTAP_AUTH_API_UNLOCK will be after clean up of hostapd_data */
        WPA3_HOSTAP_AUTH_API_UNLOCK();
    }
    return ESP_OK;
}

static void wpa3_process_rx_commit(wpa3_hostap_auth_event_t *evt)
{
    struct hostapd_sae_commit_queue *frm;
    struct hostapd_data *hapd = (struct hostapd_data *)esp_wifi_get_hostap_private_internal();
    struct sta_info *sta = NULL;
    int ret;
    frm = dl_list_first(&hapd->sae_commit_queue,
                        struct hostapd_sae_commit_queue, list);
    if (!frm) {
        return;
    }

    dl_list_del(&frm->list);
    wpa_printf(MSG_DEBUG, "SAE: Process next available message from queue");

    sta = ap_get_sta(hapd, frm->bssid);
    if (!sta) {
        sta = ap_sta_add(hapd, frm->bssid);
        if (!sta) {
            wpa_printf(MSG_DEBUG, "ap_sta_add() failed");
            ret = WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA;
            if (esp_send_sae_auth_reply(hapd, frm->bssid, frm->bssid, WLAN_AUTH_SAE,
                                        frm->auth_transaction, ret, NULL,
                                        0) != 0) {
                wpa_printf(MSG_INFO, "esp_send_sae_auth_reply: send failed");
            }
            goto free;
        }
    }

    if (sta->lock && os_semphr_take(sta->lock, 0)) {
        sta->sae_commit_processing = true;
        ret = handle_auth_sae(hapd, sta, frm->msg, frm->len, frm->bssid, frm->auth_transaction, frm->status);

        if (sta->remove_pending) {
            ap_free_sta(hapd, sta);
            goto free;
        }
        sta->sae_commit_processing = false;
        os_semphr_give(sta->lock);
        uint16_t aid = 0;
        if (ret != WLAN_STATUS_SUCCESS &&
                ret != WLAN_STATUS_ANTI_CLOGGING_TOKEN_REQ) {
            esp_wifi_ap_get_sta_aid(frm->bssid, &aid);
            if (aid == 0) {
                esp_wifi_ap_deauth_internal(frm->bssid, ret);
            }
        }
    }

free:
    os_free(frm);
}

static void wpa3_process_rx_confirm(wpa3_hostap_auth_event_t *evt)
{
    struct hostapd_data *hapd = (struct hostapd_data *)esp_wifi_get_hostap_private_internal();
    struct sta_info *sta = NULL;
    int ret = WLAN_STATUS_SUCCESS;
    struct sae_hostap_confirm_data *frm = (struct sae_hostap_confirm_data *)evt->data;
    if (!frm) {
        return;
    }
    sta = ap_get_sta(hapd, frm->bssid);
    if (!sta) {
        os_free(frm);
        return;
    }

    if (sta->lock && os_semphr_take(sta->lock, 0)) {
        ret = handle_auth_sae(hapd, sta, frm->msg, frm->len, frm->bssid, frm->auth_transaction, frm->status);

        if (sta->remove_pending) {
            ap_free_sta(hapd, sta);
            goto done;
        }
        if (ret == WLAN_STATUS_SUCCESS) {
            if (sta->sae_data && esp_send_sae_auth_reply(hapd, sta->addr, frm->bssid, WLAN_AUTH_SAE, 2,
                                                         WLAN_STATUS_SUCCESS, wpabuf_head(sta->sae_data), wpabuf_len(sta->sae_data)) != ESP_OK) {
                ap_free_sta(hapd, sta);
                goto done;
            }
            if (esp_wifi_ap_notify_node_sae_auth_done(frm->bssid) != true) {
                ap_free_sta(hapd, sta);
                goto done;
            }
        }
        os_semphr_give(sta->lock);
        if (ret != WLAN_STATUS_SUCCESS) {
            uint16_t aid = 0;
            esp_wifi_ap_get_sta_aid(frm->bssid, &aid);
            if (aid == 0) {
                esp_wifi_ap_deauth_internal(frm->bssid, ret);
            } else {
                if (sta && sta->sae_data) {
                    wpabuf_free(sta->sae_data);
                    sta->sae_data = NULL;
                }
            }
        }
    }
done:
    os_free(frm);
}

static void esp_wpa3_hostap_task(void *pvParameters)
{
    wpa3_hostap_auth_event_t evt;
    bool task_del = false;

    while (1) {
        if (os_queue_recv(g_wpa3_hostap_evt_queue, &evt, portMAX_DELAY) == pdTRUE) {
            switch (evt.id) {
            case SIG_WPA3_RX_COMMIT: {
                wpa3_process_rx_commit(&evt);
                break;
            }
            case SIG_WPA3_RX_CONFIRM: {
                wpa3_process_rx_confirm(&evt);
                break;
            }
            case SIG_TASK_DEL:
                task_del = true;
                break;
            default:
                break;
            }

            if (task_del) {
                break;
            }
        }
    }
    uint32_t items_in_queue = os_queue_msg_waiting(g_wpa3_hostap_evt_queue);
    while (items_in_queue--) {
        /* Free events posted to queue */
        os_queue_recv(g_wpa3_hostap_evt_queue, &evt, portMAX_DELAY);
        if (evt.id == SIG_WPA3_RX_CONFIRM) {
            os_free((void *)evt.data);
        }
    }
    os_queue_delete(g_wpa3_hostap_evt_queue);
    g_wpa3_hostap_evt_queue = NULL;

    if (g_wpa3_hostap_auth_api_lock) {
        WPA3_HOSTAP_AUTH_API_UNLOCK();
    }
    /* At this point, task is deleted*/
    os_task_delete(NULL);
}

int wpa3_hostap_auth_init(void *data)
{
    if (g_wpa3_hostap_evt_queue) {
        wpa_printf(MSG_ERROR, "esp_wpa3_hostap_task has already been initialised");
        return ESP_OK;
    }

    if (g_wpa3_hostap_auth_api_lock == NULL) {
        g_wpa3_hostap_auth_api_lock = os_semphr_create(1, 1);
        if (!g_wpa3_hostap_auth_api_lock) {
            wpa_printf(MSG_ERROR, "wpa3_hostap_auth_init: failed to create WPA3 hostap auth API lock");
            return ESP_FAIL;
        }
    }

    g_wpa3_hostap_evt_queue =  os_queue_create(10, sizeof(wpa3_hostap_auth_event_t));
    if (!g_wpa3_hostap_evt_queue) {
        wpa_printf(MSG_ERROR, "wpa3_hostap_auth_init: failed to create queue");
        return ESP_FAIL;
    }

    if (os_task_create(esp_wpa3_hostap_task, "esp_wpa3_hostap_task",
                       WPA3_HOSTAP_HANDLE_AUTH_TASK_STACK_SIZE, NULL,
                       WPA3_HOSTAP_HANDLE_AUTH_TASK_PRIORITY,
                       &g_wpa3_hostap_task_hdl) != pdPASS) {
        wpa_printf(MSG_ERROR, "wpa3_hostap_auth_init: failed to create task");
        os_queue_delete(g_wpa3_hostap_evt_queue);
        g_wpa3_hostap_evt_queue = NULL;
        return ESP_FAIL;
    }

    return ESP_OK;
}

bool wpa3_hostap_auth_deinit(void)
{
    if (wpa3_hostap_post_evt(SIG_TASK_DEL, 0) != 0) {
        wpa_printf(MSG_DEBUG, "failed to send task delete event");
        return false;
    } else {
        return true;
    }
}

static int wpa3_hostap_handle_auth(u8 *buf, size_t len, u32 auth_transaction, u16 status, u8 *bssid)
{
    struct hostapd_data *hapd = (struct hostapd_data *)esp_wifi_get_hostap_private_internal();
    if (!hapd) {
        return ESP_FAIL;
    }
    struct sta_info *sta = ap_get_sta(hapd, bssid);
    if (auth_transaction == SAE_MSG_COMMIT) {
        if (sta && sta->sae_commit_processing) {
            /* Ignore commit msg as we are already processing commit msg for this station */
            return ESP_OK;
        }
        return auth_sae_queue(hapd, buf, len, bssid, status, auth_transaction);
    }

    if (sta && auth_transaction == SAE_MSG_CONFIRM) {
        struct sae_hostap_confirm_data *frm = os_malloc(sizeof(struct sae_hostap_confirm_data) + len);
        if (!frm) {
            wpa_printf(MSG_ERROR, "failed to allocate memory for confirm event");
            return ESP_FAIL;
        }
        frm->len = len;
        os_memcpy(frm->bssid, bssid, ETH_ALEN);
        frm->auth_transaction = auth_transaction;
        frm->status = status;
        os_memcpy(frm->msg, buf, len);
        if (wpa3_hostap_post_evt(SIG_WPA3_RX_CONFIRM, (u32)frm) != 0) {
            wpa_printf(MSG_ERROR, "failed to queue confirm build event");
            os_free(frm);
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

int esp_send_sae_auth_reply(struct hostapd_data *hapd,
                            const u8 *dst, const u8 *bssid,
                            u16 auth_alg, u16 auth_transaction, u16 resp,
                            const u8 *ies, size_t ies_len)
{
    int status = ESP_FAIL;
    /* Calculate total frame data length (auth_alg + transaction + resp + IEs) */
    size_t data_len = ies_len + 3 * sizeof(uint16_t);

    wifi_mgmt_frm_req_t *req = os_zalloc(sizeof(*req) + data_len);
    if (!req) {
        wpa_printf(MSG_ERROR, "Failed to allocate SAE authentication reply");
        return status;
    }

    /* Populate the frame data */
    ((uint16_t *)req->data)[0] = htole16(auth_alg); /* Authentication algorithm */
    ((uint16_t *)req->data)[1] = htole16(auth_transaction); /* Transaction number */
    ((uint16_t *)req->data)[2] = htole16(resp); /* Response code */

    if (ies && ies_len) {
        os_memcpy(&((uint16_t *)req->data)[3], ies, ies_len);
    }

    req->ifx = WIFI_IF_AP;
    req->subtype = (WLAN_FC_STYPE_AUTH << 4);
    req->data_len = data_len;
    os_memcpy(req->da, bssid, ETH_ALEN);

    if (esp_wifi_send_mgmt_frm_internal(req) != 0) {
        wpa_printf(MSG_INFO, "%s: SAE authentication reply send failed", __func__);
    } else {
        status = ESP_OK;
    }

    os_free(req);

    return status;
}

void esp_wifi_register_wpa3_ap_cb(struct wpa_funcs *wpa_cb)
{
    wpa_cb->wpa3_hostap_handle_auth = wpa3_hostap_handle_auth;
}

#endif /* CONFIG_SAE */
