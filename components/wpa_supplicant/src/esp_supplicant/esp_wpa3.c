// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef CONFIG_WPA3_SAE

#include "common/sae.h"
#include "common/ieee802_11_defs.h"
#include "esp_wifi_driver.h"
#include "rsn_supp/wpa.h"

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
    const u8 *pw;

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

    esp_wifi_get_macaddr_internal(WIFI_IF_STA, own_addr);
    if (!bssid) {
        wpa_printf(MSG_ERROR, "wpa3: cannot prepare SAE commit with no BSSID!");
        return ESP_FAIL;
    }

    pw = (const u8 *)esp_wifi_sta_get_prof_password_internal();
    if (sae_prepare_commit(own_addr, bssid, pw, strlen((const char *)pw), NULL, &g_sae_data) < 0) {
        wpa_printf(MSG_ERROR, "wpa3: failed to prepare SAE commit!");
        return ESP_FAIL;
    }

reuse_data:
    len += SAE_COMMIT_MAX_LEN;
    g_sae_commit = wpabuf_alloc(len);
    if (!g_sae_commit) {
        wpa_printf(MSG_ERROR, "wpa3: failed to allocate buffer for commit msg");
        return ESP_FAIL;
    }

    if (sae_write_commit(&g_sae_data, g_sae_commit, g_sae_token, NULL) != ESP_OK) {
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
    if (g_sae_data.state != SAE_COMMITTED)
        return ESP_FAIL;

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
}

static u8 *wpa3_build_sae_msg(u8 *bssid, u32 sae_msg_type, u32 *sae_msg_len)
{
    u8 *buf = NULL;

    switch (sae_msg_type) {
        case SAE_MSG_COMMIT:
            /* Do not go for SAE when WPS is ongoing */
            if (esp_wifi_get_wps_status_internal() != WPS_STATUS_DISABLE) {
                *sae_msg_len = 0;
                return NULL;
            }
            if (ESP_OK != wpa3_build_sae_commit(bssid, sae_msg_len))
                return NULL;
            *sae_msg_len = (u32)wpabuf_len(g_sae_commit);
            buf = wpabuf_mhead_u8(g_sae_commit);
            break;
        case SAE_MSG_CONFIRM:
            if (ESP_OK != wpa3_build_sae_confirm())
                return NULL;
            *sae_msg_len = (u32)wpabuf_len(g_sae_confirm);
            buf = wpabuf_mhead_u8(g_sae_confirm);
            break;
        default:
            break;
    }

    return buf;
}

static int wpa3_parse_sae_commit(u8 *buf, u32 len, u16 status)
{
    int ret;

    if (g_sae_data.state != SAE_COMMITTED) {
        wpa_printf(MSG_ERROR, "wpa3: failed to parse SAE commit in state(%d)!",
                   g_sae_data.state);
        return ESP_FAIL;
    }

    if (status == WLAN_STATUS_ANTI_CLOGGING_TOKEN_REQ) {
        if (g_sae_token)
            wpabuf_free(g_sae_token);
        g_sae_token = wpabuf_alloc_copy(buf + 2, len - 2);
        return ESP_OK;
    }

    ret = sae_parse_commit(&g_sae_data, buf, len, NULL, 0, g_allowed_groups);
    if (ret) {
        wpa_printf(MSG_ERROR, "wpa3: could not parse commit(%d)", ret);
        return ret;
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
        wpa_printf(MSG_ERROR, "wpa3: failed to parse SAE confirm in state(%d)!",
                   g_sae_data.state);
        return ESP_FAIL;
    }

    if (sae_check_confirm(&g_sae_data, buf, len) != ESP_OK) {
        wpa_printf(MSG_ERROR, "wpa3: failed to parse SAE confirm");
        return ESP_FAIL;
    }
    g_sae_data.state = SAE_ACCEPTED;

    wpa_set_pmk(g_sae_data.pmk, g_sae_data.pmkid, true);

    return ESP_OK;
}

static int wpa3_parse_sae_msg(u8 *buf, u32 len, u32 sae_msg_type, u16 status)
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
            wpa_printf(MSG_ERROR, "wpa3: Invalid SAE msg type(%d)!", sae_msg_type);
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

#endif /* CONFIG_WPA3_SAE */
