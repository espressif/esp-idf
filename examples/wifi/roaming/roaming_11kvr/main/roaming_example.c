/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wnm.h"
#include "esp_rrm.h"
#include "esp_mbo.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <sys/time.h>

/* Configuration */
#define EXAMPLE_WIFI_SSID CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_WIFI_PASSWORD CONFIG_EXAMPLE_WIFI_PASSWORD
#define EXAMPLE_WIFI_RSSI_THRESHOLD CONFIG_EXAMPLE_WIFI_RSSI_THRESHOLD

static bool g_neighbor_report_active;

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* esp netif object representing the WIFI station */
static esp_netif_t *sta_netif = NULL;

static const char *TAG = "roaming_example";

static inline uint32_t WPA_GET_LE32(const uint8_t *a)
{
	return ((uint32_t) a[3] << 24) | (a[2] << 16) | (a[1] << 8) | a[0];
}

static void event_handler(void* arg, esp_event_base_t event_base,
		int32_t event_id, void* event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
		wifi_event_sta_disconnected_t *disconn = event_data;
		ESP_LOGI(TAG, "station got disconnected reason=%d", disconn->reason);
		if (disconn->reason == WIFI_REASON_ROAMING) {
			ESP_LOGI(TAG, "station roaming, do nothing");
		} else {
			esp_wifi_connect();
		}
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
#if EXAMPLE_WIFI_RSSI_THRESHOLD
		if (EXAMPLE_WIFI_RSSI_THRESHOLD) {
			ESP_LOGI(TAG, "setting rssi threshold as %d", EXAMPLE_WIFI_RSSI_THRESHOLD);
			esp_wifi_set_rssi_threshold(EXAMPLE_WIFI_RSSI_THRESHOLD);
		}
#endif
		if (esp_rrm_is_rrm_supported_connection()) {
			ESP_LOGI(TAG,"RRM supported");
                        esp_rrm_send_neighbor_report_request();
                        g_neighbor_report_active = true;
		} else {
			ESP_LOGI(TAG,"RRM not supported");
		}
		if (esp_wnm_is_btm_supported_connection()) {
			ESP_LOGI(TAG,"BTM supported");
		} else {
			ESP_LOGI(TAG,"BTM not supported");
		}

	}
}

#ifndef WLAN_EID_MEASURE_REPORT
#define WLAN_EID_MEASURE_REPORT 39
#endif
#ifndef MEASURE_TYPE_LCI
#define MEASURE_TYPE_LCI 9
#endif
#ifndef MEASURE_TYPE_LOCATION_CIVIC
#define MEASURE_TYPE_LOCATION_CIVIC 11
#endif
#ifndef WLAN_EID_NEIGHBOR_REPORT
#define WLAN_EID_NEIGHBOR_REPORT 52
#endif
#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

#define MAX_LCI_CIVIC_LEN 256 * 2 + 1
#define MAX_NEIGHBOR_LEN 512
#if 1
static char * get_btm_neighbor_list(uint8_t *report, size_t report_len)
{
	size_t len = 0;
	const uint8_t *data;
	int ret = 0;

	char *lci = NULL;
	char *civic = NULL;
	/*
	 * Neighbor Report element (IEEE P802.11-REVmc/D5.0)
	 * BSSID[6]
	 * BSSID Information[4]
	 * Operating Class[1]
	 * Channel Number[1]
	 * PHY Type[1]
	 * Optional Subelements[variable]
	 */
#define NR_IE_MIN_LEN (ETH_ALEN + 4 + 1 + 1 + 1)

	if (!report || report_len == 0) {
		ESP_LOGI(TAG, "RRM neighbor report is not valid");
		return NULL;
	}

	char *buf = calloc(1, MAX_NEIGHBOR_LEN);
	if (!buf) {
		ESP_LOGE(TAG, "Memory allocation for neighbor list failed");
		goto cleanup;
	}
	data = report;

	while (report_len >= 2 + NR_IE_MIN_LEN) {
		const uint8_t *nr;
		lci = (char *)malloc(sizeof(char)*MAX_LCI_CIVIC_LEN);
		if (!lci) {
			ESP_LOGE(TAG, "Memory allocation for lci failed");
			goto cleanup;
		}
		civic = (char *)malloc(sizeof(char)*MAX_LCI_CIVIC_LEN);
		if (!civic) {
			ESP_LOGE(TAG, "Memory allocation for civic failed");
			goto cleanup;
		}
		uint8_t nr_len = data[1];
		const uint8_t *pos = data, *end;

		if (pos[0] != WLAN_EID_NEIGHBOR_REPORT ||
		    nr_len < NR_IE_MIN_LEN) {
			ESP_LOGI(TAG, "CTRL: Invalid Neighbor Report element: id=%u len=%u",
					data[0], nr_len);
			ret = -1;
			goto cleanup;
		}

		if (2U + nr_len > report_len) {
			ESP_LOGI(TAG, "CTRL: Invalid Neighbor Report element: id=%u len=%zu nr_len=%u",
					data[0], report_len, nr_len);
			ret = -1;
			goto cleanup;
		}
		pos += 2;
		end = pos + nr_len;

		nr = pos;
		pos += NR_IE_MIN_LEN;

		lci[0] = '\0';
		civic[0] = '\0';
		while (end - pos > 2) {
			uint8_t s_id, s_len;

			s_id = *pos++;
			s_len = *pos++;
			if (s_len > end - pos) {
				ret = -1;
				goto cleanup;
			}
			if (s_id == WLAN_EID_MEASURE_REPORT && s_len > 3) {
				/* Measurement Token[1] */
				/* Measurement Report Mode[1] */
				/* Measurement Type[1] */
				/* Measurement Report[variable] */
				switch (pos[2]) {
					case MEASURE_TYPE_LCI:
						if (lci[0])
							break;
						memcpy(lci, pos, s_len);
						break;
					case MEASURE_TYPE_LOCATION_CIVIC:
						if (civic[0])
							break;
						memcpy(civic, pos, s_len);
						break;
				}
			}

			pos += s_len;
		}

		ESP_LOGI(TAG, "RMM neighbor report bssid=" MACSTR
				" info=0x%" PRIx32 " op_class=%u chan=%u phy_type=%u%s%s%s%s",
				MAC2STR(nr), WPA_GET_LE32(nr + ETH_ALEN),
				nr[ETH_ALEN + 4], nr[ETH_ALEN + 5],
				nr[ETH_ALEN + 6],
				lci[0] ? " lci=" : "", lci,
				civic[0] ? " civic=" : "", civic);

		/* neighbor start */
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, " neighbor=");
		/* bssid */
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, MACSTR, MAC2STR(nr));
		/* , */
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, ",");
		/* bssid info */
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, "0x%04" PRIx32 "", WPA_GET_LE32(nr + ETH_ALEN));
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, ",");
		/* operating class */
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, "%u", nr[ETH_ALEN + 4]);
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, ",");
		/* channel number */
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, "%u", nr[ETH_ALEN + 5]);
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, ",");
		/* phy type */
		len += snprintf(buf + len, MAX_NEIGHBOR_LEN - len, "%u", nr[ETH_ALEN + 6]);
		/* optional elements, skip */

		data = end;
		report_len -= 2 + nr_len;

		if (lci) {
			free(lci);
			lci = NULL;
		}
		if (civic) {
			free(civic);
			civic = NULL;
		}
	}

cleanup:
	if (lci) {
		free(lci);
	}
	if (civic) {
		free(civic);
	}

	if (ret < 0) {
		free(buf);
		buf = NULL;
	}
	return buf;
}

#else

/* Sample API to create neighbor list */
char * get_tmp_neighbor_list(uint8_t *report, size_t report_len)
{
#define MAC1 "00:01:02:03:04:05"
#define MAC2 "00:02:03:04:05:06"

	char * buf = calloc(1, MAX_NEIGHBOR_LEN);
	size_t len = 0;
	char *pos;
	if (!buf)
		return NULL;

	pos = buf;
	/* create two temp neighbors */
	/* format for neighbor list : neighbor=11:22:33:44:55:66,0x0000,81,3,7,0301ff */

	/* neighbor1 start */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, " neighbor=");
	/* bssid */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, MAC1);
	/* , */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, ",");
	/* bssid info */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, "0x0000");
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, ",");
	/* operating class */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, "81");
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, ",");
	/* channel number */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, "6");
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, ",");
	/* phy type */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, "7");
	/* optional elements, skip */

	/* neighbor2 start */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, " neighbor=");
	/* bssid */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, MAC2);
	/* , */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, ",");
	/* bssid info */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, "0x0000");
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, ",");
	/* operating class */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, "81");
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, ",");
	/* channel number */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, "6");
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, ",");
	/* phy type */
	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, "7");
	/* optional elements, skip */

	len += snprintf(pos + len, MAX_NEIGHBOR_LEN - len, " ");

#undef MAC1
#undef MAC2
	return buf;
}
#endif
static void esp_neighbor_report_recv_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
	if (!g_neighbor_report_active) {
		ESP_LOGV(TAG,"Neighbor report received but not triggered by us");
	    return;
    }
    if (!event_data) {
        ESP_LOGE(TAG, "No event data received for neighbor report");
        return;
    }
    g_neighbor_report_active = false;
    uint8_t cand_list = 0;
    wifi_event_neighbor_report_t *neighbor_report_event = (wifi_event_neighbor_report_t*)event_data;
    uint8_t *pos = (uint8_t *)neighbor_report_event->report;
    char * neighbor_list = NULL;
    if (!pos) {
        ESP_LOGE(TAG, "Neighbor report is empty");
        return;
    }
    uint8_t report_len = neighbor_report_event->report_len;
    /* dump report info */
    ESP_LOGD(TAG, "rrm: neighbor report len=%d", report_len);
    ESP_LOG_BUFFER_HEXDUMP(TAG, pos, report_len, ESP_LOG_DEBUG);

    /* create neighbor list */
    neighbor_list = get_btm_neighbor_list(pos + 1, report_len - 1);
    /* In case neighbor list is not present issue a scan and get the list from that */
    if (!neighbor_list) {
        /* issue scan */
        wifi_scan_config_t params;
        memset(&params, 0, sizeof(wifi_scan_config_t));
        if (esp_wifi_scan_start(&params, true) < 0) {
		    goto cleanup;
	    }
	    /* cleanup from net802.11 */
        esp_wifi_clear_ap_list();
        cand_list = 1;
	}
	/* send AP btm query, this will cause STA to roam as well */
	esp_wnm_send_bss_transition_mgmt_query(REASON_FRAME_LOSS, neighbor_list, cand_list);
cleanup:
	if (neighbor_list)
		free(neighbor_list);

}

#if EXAMPLE_WIFI_RSSI_THRESHOLD
static void esp_bss_rssi_low_handler(void* arg, esp_event_base_t event_base,
		int32_t event_id, void* event_data)
{
	wifi_event_bss_rssi_low_t *event = event_data;

	ESP_LOGI(TAG, "%s:bss rssi is=%d", __func__, event->rssi);
	/* Lets check channel conditions */
	rrm_ctx++;
	if (esp_rrm_send_neighbor_report_request() < 0) {
		/* failed to send neighbor report request */
		ESP_LOGI(TAG, "failed to send neighbor report request");
		if (esp_wnm_send_bss_transition_mgmt_query(REASON_FRAME_LOSS, NULL, 0) < 0) {
			ESP_LOGI(TAG, "failed to send btm query");
		}
	} else {
		g_neighbor_report_active = true;
	}

}
#endif

#if 0
/* Example code to update channel preference in MBO */
static void clear_chan_preference()
{
	esp_mbo_update_non_pref_chan(NULL);
}

static void update_chan_preference(void)
{
	struct non_pref_chan_s *chans = malloc(sizeof(struct non_pref_chan_s) + 2 * sizeof(struct non_pref_chan));
	chans->non_pref_chan_num = 2;

	/* first */
	struct non_pref_chan *chan = &chans->chan[0];
	chan->reason = NON_PREF_CHAN_REASON_UNSPECIFIED;
	chan->oper_class = 0x51;
	chan->chan = 1;
	chan->preference = 0;

	/* second */
	chan = &chans->chan[1];
	chan->reason = NON_PREF_CHAN_REASON_UNSPECIFIED;
	chan->oper_class = 0x51;
	chan->chan = 11;
	chan->preference = 1;

	esp_mbo_update_non_pref_chan(chans);
	free(chans);
}
#endif

static void initialise_wifi(void)
{
	ESP_ERROR_CHECK(esp_netif_init());
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
	ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_NEIGHBOR_REP,
				&esp_neighbor_report_recv_handler, NULL));
#if EXAMPLE_WIFI_RSSI_THRESHOLD
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_BSS_RSSI_LOW,
				&esp_bss_rssi_low_handler, NULL));
#endif


	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	wifi_config_t wifi_config = {
		.sta = {
			.ssid = EXAMPLE_WIFI_SSID,
			.password = EXAMPLE_WIFI_PASSWORD,
			.rm_enabled =1,
			.btm_enabled =1,
			.mbo_enabled =1,
			.pmf_cfg.capable = 1,
			.ft_enabled =1,
		},
	};

	ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
	ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );
}

void app_main(void)
{
	ESP_ERROR_CHECK( nvs_flash_init() );
	initialise_wifi();
}
