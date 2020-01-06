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
#include "esp_wifi_driver.h"
#include "rsn_supp/wpa.h"

static struct sae_data g_sae_data;

int g_allowed_groups[] = { IANA_SECP256R1, 0 };

static struct wpabuf *wpa3_build_sae_commit(u8 *bssid)
{
	int default_group = IANA_SECP256R1;
        struct wpabuf *buf;
	u8 own_addr[ETH_ALEN];
	const u8 *pw;
	memset(&g_sae_data, 0, sizeof(g_sae_data));

	if (sae_set_group(&g_sae_data, default_group)) {
		wpa_printf(MSG_ERROR, "wpa3: could not set SAE group %d", default_group);
		return NULL;
	}

	esp_wifi_get_macaddr_internal(WIFI_IF_STA, own_addr);
	if (!bssid) {
		wpa_printf(MSG_ERROR, "wpa3: cannot prepare SAE commit with no BSSID!");
		return NULL;
	}

	pw = (const u8 *)esp_wifi_sta_get_prof_password_internal();
	if (sae_prepare_commit(own_addr, bssid, pw, strlen((const char *)pw), NULL, &g_sae_data) < 0) {
		wpa_printf(MSG_ERROR, "wpa3: failed to prepare SAE commit!");
		return NULL;
	}

	buf = wpabuf_alloc(SAE_COMMIT_MAX_LEN);
	if (!buf) {
		wpa_printf(MSG_ERROR, "wpa3: failed to allocate buffer for commit msg");
		return NULL;
	}

	if (sae_write_commit(&g_sae_data, buf, NULL, NULL) != ESP_OK) {
		wpa_printf(MSG_ERROR, "wpa3: failed to write SAE commit msg");
		wpabuf_free(buf);
		return NULL;
	}
	g_sae_data.state = SAE_COMMITTED;

	return buf;
}

static struct wpabuf *wpa3_build_sae_confirm(void)
{
        struct wpabuf *buf;

	if (g_sae_data.state != SAE_COMMITTED)
		return NULL;

	buf = wpabuf_alloc(SAE_COMMIT_MAX_LEN);
	if (!buf) {
		wpa_printf(MSG_ERROR, "wpa3: failed to allocate buffer for confirm msg");
		return NULL;
	}

	if (sae_write_confirm(&g_sae_data, buf) != ESP_OK) {
		wpa_printf(MSG_ERROR, "wpa3: failed to write SAE confirm msg");
		wpabuf_free(buf);
		return NULL;
	}
	g_sae_data.state = SAE_CONFIRMED;

	return buf;
}

static u8 *wpa3_build_sae_msg(u8 *bssid, u32 sae_msg_type, u32 *sae_msg_len)
{
        struct wpabuf *buf = NULL;

	switch (sae_msg_type) {
		case SAE_MSG_COMMIT:
			buf = wpa3_build_sae_commit(bssid);
			break;
		case SAE_MSG_CONFIRM:
			buf = wpa3_build_sae_confirm();
			break;
		default:
			break;
	}

	if (buf) {
		*sae_msg_len = (u32)wpabuf_len(buf);
		return wpabuf_mhead_u8(buf);
	} else
		return NULL;
}

static int wpa3_parse_sae_commit(u8 *buf, u32 len)
{
	int ret;

	if (g_sae_data.state != SAE_COMMITTED) {
		wpa_printf(MSG_ERROR, "wpa3: failed to parse SAE commit in state(%d)!",
			   g_sae_data.state);
		return ESP_FAIL;
	}

	ret = sae_parse_commit(&g_sae_data, buf, len, NULL, 0, g_allowed_groups);
	if (ret) {
		wpa_printf(MSG_ERROR, "wpa3: could not parse commit(%d)", ret);
		return ESP_FAIL;
	}

	ret = sae_process_commit(&g_sae_data);
	if (ret) {
		wpa_printf(MSG_ERROR, "wpa3: could not process commit(%d)", ret);
		return ESP_FAIL;
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

	wpa_set_pmk(g_sae_data.pmk);
	memcpy(esp_wifi_sta_get_ap_info_prof_pmk_internal(), g_sae_data.pmk, PMK_LEN);

	return ESP_OK;
}

static int wpa3_parse_sae_msg(u8 *buf, u32 len, u32 sae_msg_type)
{
	int ret = ESP_OK;

	switch (sae_msg_type) {
		case SAE_MSG_COMMIT:
			ret = wpa3_parse_sae_commit(buf, len);
			break;
		case SAE_MSG_CONFIRM:
			ret = wpa3_parse_sae_confirm(buf, len);
			break;
		default:
			wpa_printf(MSG_ERROR, "wpa3: Invalid SAE msg type(%d)!",
				   sae_msg_type);
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
