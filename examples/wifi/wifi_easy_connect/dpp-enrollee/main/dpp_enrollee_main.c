/* DPP Enrollee Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_dpp.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "qrcode.h"

#ifdef CONFIG_ESP_DPP_LISTEN_CHANNEL_LIST
#define EXAMPLE_DPP_LISTEN_CHANNEL_LIST     CONFIG_ESP_DPP_LISTEN_CHANNEL_LIST
#else
#define EXAMPLE_DPP_LISTEN_CHANNEL_LIST     "6"
#endif

#ifdef CONFIG_ESP_DPP_BOOTSTRAPPING_KEY
#define EXAMPLE_DPP_BOOTSTRAPPING_KEY   CONFIG_ESP_DPP_BOOTSTRAPPING_KEY
#else
#define EXAMPLE_DPP_BOOTSTRAPPING_KEY   0
#endif

#ifdef CONFIG_ESP_DPP_DEVICE_INFO
#define EXAMPLE_DPP_DEVICE_INFO      CONFIG_ESP_DPP_DEVICE_INFO
#else
#define EXAMPLE_DPP_DEVICE_INFO      0
#endif

#define CURVE_SEC256R1_PKEY_HEX_DIGITS     64

static const char *TAG = "wifi dpp-enrollee";

/* Multi-config state */
static esp_dpp_config_data_t *s_dpp_configs = NULL;
static int s_config_count = 0;
static int s_active_config_index = 0;
static int s_last_attempted_config_index = -1;
static int s_connect_retry_count = 0;

#define WIFI_MAX_RETRY_NUM   3
#define DPP_AUTH_RETRY_MAX   3

#define IS_DPP_AKM(akm) \
    ((akm) == ESP_DPP_AKM_DPP || (akm) == ESP_DPP_AKM_SAE_DPP || (akm) == ESP_DPP_AKM_PSK_SAE_DPP)

/* Flag to track if bootstrap URI generation was successful. */
static bool s_bootstrap_ready = false;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_dpp_event_group;

#define DPP_CONNECTED_BIT    BIT0
#define DPP_CONNECT_FAIL_BIT BIT1
#define DPP_AUTH_FAIL_BIT    BIT2

static void cleanup_dpp_configs(void)
{
    if (s_dpp_configs) {
        free(s_dpp_configs);
        s_dpp_configs = NULL;
    }
    s_config_count = 0;
    s_active_config_index = 0;
    s_last_attempted_config_index = -1;
    s_connect_retry_count = 0;
}


static void dpp_enrollee_retry_dpp_auth_or_fail(void)
{
    esp_err_t err;

    if (s_connect_retry_count >= DPP_AUTH_RETRY_MAX) {
        xEventGroupSetBits(s_dpp_event_group, DPP_AUTH_FAIL_BIT);
        return;
    }

    ESP_LOGI(TAG, "Retrying DPP Auth/Listen... (%d/%d)",
             s_connect_retry_count + 1, DPP_AUTH_RETRY_MAX);
    err = esp_supp_dpp_start_listen();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to restart DPP listen: %s", esp_err_to_name(err));
        xEventGroupSetBits(s_dpp_event_group, DPP_AUTH_FAIL_BIT);
        return;
    }
    s_connect_retry_count++;
}

static esp_err_t apply_config_and_connect(esp_dpp_config_data_t *config_row)
{
    wifi_config_t wifi_cfg = {0};
    esp_err_t err;

    if (!config_row) {
        return ESP_ERR_INVALID_ARG;
    }

    s_last_attempted_config_index = s_active_config_index;

    size_t ssid_len = config_row->ssid_len;
    if (ssid_len > MAX_SSID_LEN) {
        ESP_LOGW(TAG, "ssid_len %u exceeds MAX_SSID_LEN, clamping",
                 (unsigned)config_row->ssid_len);
        ssid_len = MAX_SSID_LEN;
    }
    if (ssid_len > sizeof(wifi_cfg.sta.ssid)) {
        ssid_len = sizeof(wifi_cfg.sta.ssid);
    }
    if (ssid_len) {
        memcpy(wifi_cfg.sta.ssid, config_row->ssid, ssid_len);
    }

    /* Auth threshold so a weaker same-SSID AP does not win the default RSSI sort */
    switch ((esp_dpp_akm_t)config_row->akm) {
    case ESP_DPP_AKM_DPP:
        wifi_cfg.sta.threshold.authmode = WIFI_AUTH_DPP;
        break;
    case ESP_DPP_AKM_SAE_DPP:
    case ESP_DPP_AKM_SAE:
        wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA3_PSK;
        break;
    case ESP_DPP_AKM_PSK_SAE_DPP:
    case ESP_DPP_AKM_PSK_SAE:
        wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_WPA3_PSK;
        break;
    case ESP_DPP_AKM_PSK:
    default:
        wifi_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
        break;
    }

    if (IS_DPP_AKM(config_row->akm)) {
        err = esp_supp_dpp_set_config(config_row);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_supp_dpp_set_config failed: %s", esp_err_to_name(err));
            return err;
        }
    } else {
        err = esp_supp_dpp_set_config(NULL);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_supp_dpp_set_config(NULL) failed: %s", esp_err_to_name(err));
            return err;
        }
    }

    size_t pass_len = config_row->password_len;
    if (pass_len > MAX_PASSPHRASE_LEN) {
        ESP_LOGW(TAG, "password_len %u exceeds MAX_PASSPHRASE_LEN, clamping",
                 (unsigned)config_row->password_len);
        pass_len = MAX_PASSPHRASE_LEN;
    }
    if (pass_len > sizeof(wifi_cfg.sta.password)) {
        pass_len = sizeof(wifi_cfg.sta.password);
    }
    if (pass_len) {
        memcpy(wifi_cfg.sta.password, config_row->password, pass_len);
    }

    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config failed: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_connect();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_connect failed: %s", esp_err_to_name(err));
    }
    return err;
}

static void advance_to_next_config_or_fail(void)
{
    s_active_config_index++;
    if (s_active_config_index < s_config_count) {
        esp_dpp_config_data_t *config_row = &s_dpp_configs[s_active_config_index];

        s_connect_retry_count = 0;
        ESP_LOGI(TAG, "Next config %d/%d - SSID len=%u: %.*s",
                 s_active_config_index + 1, s_config_count,
                 (unsigned)config_row->ssid_len,
                 (int)config_row->ssid_len, (const char *)config_row->ssid);
        if (apply_config_and_connect(config_row) != ESP_OK) {
            advance_to_next_config_or_fail();
        }
    } else {
        ESP_LOGE(TAG, "No more configs left");
        xEventGroupSetBits(s_dpp_event_group, DPP_CONNECT_FAIL_BIT);
    }
}

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
        case WIFI_EVENT_STA_START:
            cleanup_dpp_configs();
            ESP_ERROR_CHECK(esp_supp_dpp_start_listen());
            ESP_LOGI(TAG, "Started listening for DPP Authentication");
            break;
        case WIFI_EVENT_STA_DISCONNECTED: {
            if (s_config_count > 0) {
                if (s_connect_retry_count < WIFI_MAX_RETRY_NUM) {
                    ESP_LOGI(TAG, "Disconnect event, retry %d/%d to connect to the AP",
                             s_connect_retry_count + 1, WIFI_MAX_RETRY_NUM);
                    if (esp_wifi_connect() == ESP_OK) {
                        s_connect_retry_count++;
                    } else {
                        advance_to_next_config_or_fail();
                    }
                } else {
                    ESP_LOGD(TAG, "Retries exhausted on current config");
                    advance_to_next_config_or_fail();
                }
            }
            break;
        }
        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Successfully connected to the AP");
            break;
        case WIFI_EVENT_DPP_URI_READY: {
            wifi_event_dpp_uri_ready_t *uri_data = event_data;
            if (uri_data != NULL) {
                esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();

                ESP_LOGI(TAG, "Scan below QR Code to configure the enrollee:");
                esp_qrcode_generate(&cfg, (const char *)uri_data->uri);
                s_bootstrap_ready = true;
            }
            break;
        }
        case WIFI_EVENT_DPP_CFG_RECVD: {
            wifi_event_dpp_config_received_t *ev = event_data;

            cleanup_dpp_configs();
            if (ev->total_conf > 0) {
                s_config_count = ev->total_conf;
                s_dpp_configs = malloc(s_config_count * sizeof(esp_dpp_config_data_t));
                if (s_dpp_configs) {
                    memcpy(s_dpp_configs, ev->configs, s_config_count * sizeof(esp_dpp_config_data_t));
                    s_active_config_index = 0;
                    s_connect_retry_count = 0;

                    ESP_LOGI(TAG, "DPP Config Received: %d objects", s_config_count);
                    if (apply_config_and_connect(&s_dpp_configs[0]) != ESP_OK) {
                        /* First config failed at the application stage; try the
                         * remaining ones (consistent with the STA_DISCONNECTED
                         * retry-exhausted path) before declaring failure. */
                        advance_to_next_config_or_fail();
                    }
                } else {
                    ESP_LOGE(TAG, "Failed to allocate memory for %d DPP configs", s_config_count);
                    s_config_count = 0;
                    xEventGroupSetBits(s_dpp_event_group, DPP_CONNECT_FAIL_BIT);
                }
            }
            break;
        }
        case WIFI_EVENT_DPP_FAILED: {
            if (s_config_count == 0) {
                wifi_event_dpp_failed_t *dpp_failure = event_data;
                esp_err_t reason = (dpp_failure != NULL) ? (esp_err_t)dpp_failure->failure_reason : ESP_ERR_DPP_FAILURE;

                if (s_bootstrap_ready) {
                    ESP_LOGI(TAG, "DPP Authentication failed (%s), retrying...", esp_err_to_name(reason));
                    dpp_enrollee_retry_dpp_auth_or_fail();
                } else {
                    ESP_LOGE(TAG, "DPP Bootstrap generation failed (%s)", esp_err_to_name(reason));
                    xEventGroupSetBits(s_dpp_event_group, DPP_AUTH_FAIL_BIT);
                }
            } else {
                /*
                 * DPP failure occurred after we already have a configuration list.
                 * This is typically a Network Introduction protocol failure.
                 * Trigger a disconnect to drive the retry/fallback state machine
                 * in the STA_DISCONNECTED handler.
                 */
                ESP_LOGE(TAG, "DPP post-provisioning failure; triggering retry");
                esp_wifi_disconnect();
            }
            break;
        }
        default:
            break;
        }
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_connect_retry_count = 0;
        xEventGroupSetBits(s_dpp_event_group, DPP_CONNECTED_BIT);
    }
}

esp_err_t dpp_enrollee_bootstrap(void)
{
    esp_err_t ret;
    size_t pkey_len = strlen(EXAMPLE_DPP_BOOTSTRAPPING_KEY);
    char *key = NULL;

    if (pkey_len) {
        /* Currently only NIST P-256 curve is supported, add prefix/postfix accordingly */
        char prefix[] = "30310201010420";
        char postfix[] = "a00a06082a8648ce3d030107";

        if (pkey_len != CURVE_SEC256R1_PKEY_HEX_DIGITS) {
            ESP_LOGI(TAG, "Invalid key length! Private key needs to be 32 bytes (or 64 hex digits) long");
            return ESP_FAIL;
        }

        key = malloc(sizeof(prefix) + pkey_len + sizeof(postfix));
        if (!key) {
            ESP_LOGI(TAG, "Failed to allocate for bootstrapping key");
            return ESP_ERR_NO_MEM;
        }
        sprintf(key, "%s%s%s", prefix, EXAMPLE_DPP_BOOTSTRAPPING_KEY, postfix);
    }

    /* Currently only supported method is QR Code */
    ret = esp_supp_dpp_bootstrap_gen(EXAMPLE_DPP_LISTEN_CHANNEL_LIST, DPP_BOOTSTRAP_QR_CODE,
                                     key, EXAMPLE_DPP_DEVICE_INFO);

    if (key)
        free(key);

    return ret;
}

void dpp_enrollee_init(void)
{
    s_dpp_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_supp_dpp_init());
    ESP_ERROR_CHECK(dpp_enrollee_bootstrap());
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_dpp_event_group,
                                           DPP_CONNECTED_BIT | DPP_CONNECT_FAIL_BIT | DPP_AUTH_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & DPP_CONNECTED_BIT) {
        ESP_LOGI(TAG, "DPP Provisioning and Connection successful");
    } else if (bits & DPP_CONNECT_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect after exhausting all configurations");
    } else if (bits & DPP_AUTH_FAIL_BIT) {
        ESP_LOGI(TAG, "DPP Authentication failed");

    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    uint8_t diag_ssid_len = 0;
    uint8_t diag_ssid[32]; // MAX_SSID_LEN
    bool diag_password_set = false;
    bool diag_have_last_config = false;

    if (bits & DPP_CONNECT_FAIL_BIT) {
        int last_idx = s_last_attempted_config_index;

        if (last_idx < 0 && s_config_count > 0) {
            last_idx = s_active_config_index >= s_config_count ? s_config_count - 1 : s_active_config_index;
        }
        if (s_dpp_configs != NULL && last_idx >= 0 && last_idx < s_config_count) {
            const esp_dpp_config_data_t *row = &s_dpp_configs[last_idx];

            diag_ssid_len = row->ssid_len;
            if (diag_ssid_len > 32) {
                diag_ssid_len = 32;
            }
            memcpy(diag_ssid, row->ssid, diag_ssid_len);
            diag_password_set = row->password_len > 0;
            diag_have_last_config = true;
        }
    }

    if (bits & DPP_CONNECT_FAIL_BIT && diag_have_last_config) {
        ESP_LOGI(TAG, "Last attempted config -> SSID len=%u: %.*s, password: %s",
                 (unsigned)diag_ssid_len,
                 (int)diag_ssid_len,
                 (const char *)diag_ssid,
                 diag_password_set ? "(set)" : "(not set)");
    }

    esp_supp_dpp_deinit();
    s_bootstrap_ready = false;
    cleanup_dpp_configs();

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_dpp_event_group);
}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    dpp_enrollee_init();
}
