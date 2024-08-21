/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <cJSON.h>

#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <esp_timer.h>

#include <protocomm.h>
#include <protocomm_security0.h>
#include <protocomm_security1.h>
#include <protocomm_security2.h>

#include "wifi_provisioning_priv.h"

#define WIFI_PROV_MGR_VERSION      "v1.1"
#define WIFI_PROV_STORAGE_BIT       BIT0
#define WIFI_PROV_SETTING_BIT       BIT1
#define MAX_SCAN_RESULTS           CONFIG_WIFI_PROV_SCAN_MAX_ENTRIES

#define ACQUIRE_LOCK(mux)     assert(xSemaphoreTake(mux, portMAX_DELAY) == pdTRUE)
#define RELEASE_LOCK(mux)     assert(xSemaphoreGive(mux) == pdTRUE)

static const char *TAG = "wifi_prov_mgr";

ESP_EVENT_DEFINE_BASE(WIFI_PROV_EVENT);
ESP_EVENT_DEFINE_BASE(WIFI_PROV_MGR_PVT_EVENT);

typedef enum {
    WIFI_PROV_STATE_IDLE,
    WIFI_PROV_STATE_STARTING,
    WIFI_PROV_STATE_STARTED,
    WIFI_PROV_STATE_CRED_RECV,
    WIFI_PROV_STATE_FAIL,
    WIFI_PROV_STATE_SUCCESS,
    WIFI_PROV_STATE_STOPPING
} wifi_prov_mgr_state_t;

typedef enum {
    WIFI_PROV_MGR_STOP,
} wifi_prov_mgr_pvt_event_t;

/**
 * @brief  Structure for storing capabilities supported by
 *         the provisioning service
 */
struct wifi_prov_capabilities {
    /* Security 0 is used */
    bool no_sec;

    /* Proof of Possession is not required for establishing session */
    bool no_pop;

    /* Provisioning doesn't stop on it's own after receiving Wi-Fi credentials
     * instead application has to explicitly call wifi_prov_mgr_stop_provisioning() */
    bool no_auto_stop;
};

/**
 * @brief  Structure for storing miscellaneous information about
 *         provisioning service that will be conveyed to clients
 */
struct wifi_prov_info {
    const char *version;
    struct wifi_prov_capabilities capabilities;
};

/**
 * @brief  Context data for provisioning manager
 */
struct wifi_prov_mgr_ctx {
    /* Provisioning manager configuration */
    wifi_prov_mgr_config_t mgr_config;

    /* State of the provisioning service */
    wifi_prov_mgr_state_t prov_state;

    /* Provisioning scheme configuration */
    void *prov_scheme_config;

    /* Protocomm handle */
    protocomm_t *pc;

    /* Type of security to use with protocomm */
    int security;

    /* Pointer to security params */
    const void* protocomm_sec_params;

    /* Handle for Provisioning Auto Stop timer */
    esp_timer_handle_t autostop_timer;

    /* Handle for delayed Wi-Fi connection timer */
    esp_timer_handle_t wifi_connect_timer;

    /* Handle for delayed cleanup timer */
    esp_timer_handle_t cleanup_delay_timer;

    /* State of Wi-Fi Station */
    wifi_prov_sta_state_t wifi_state;

    /* Code for Wi-Fi station disconnection (if disconnected) */
    wifi_prov_sta_fail_reason_t wifi_disconnect_reason;

    /* Protocomm handlers for Wi-Fi configuration endpoint */
    wifi_prov_config_handlers_t *wifi_prov_handlers;

    /* Protocomm handlers for Wi-Fi scan endpoint */
    wifi_prov_scan_handlers_t *wifi_scan_handlers;

    /* Protocomm handlers for Wi-Fi ctrl endpoint */
    wifi_ctrl_handlers_t *wifi_ctrl_handlers;

    /* Count of used endpoint UUIDs */
    unsigned int endpoint_uuid_used;

    /* Provisioning service information */
    struct wifi_prov_info mgr_info;

    /* Application related information in JSON format */
    cJSON *app_info_json;

    /* Delay after which resources will be cleaned up asynchronously
     * upon execution of wifi_prov_mgr_stop_provisioning() */
    uint32_t cleanup_delay;

    /* Wi-Fi scan parameters and state variables */
    bool scanning;
    uint8_t channels_per_group;
    uint16_t curr_channel;
    uint16_t ap_list_len[14];   // 14 entries corresponding to each channel
    wifi_ap_record_t *ap_list[14];
    wifi_ap_record_t *ap_list_sorted[MAX_SCAN_RESULTS];
    wifi_scan_config_t scan_cfg;
};

/* Mutex to lock/unlock access to provisioning singleton
 * context data. This is allocated only once on first init
 * and never deleted as wifi_prov_mgr is a singleton */
static SemaphoreHandle_t prov_ctx_lock = NULL;

/* Pointer to provisioning context data */
static struct wifi_prov_mgr_ctx *prov_ctx;

/* This executes registered app_event_callback for a particular event
 *
 * NOTE : By the time this function returns, it is possible that
 * the manager got de-initialized due to a call to wifi_prov_mgr_deinit()
 * either inside the event callbacks or from another thread. Therefore
 * post execution of execute_event_cb(), the validity of prov_ctx must
 * always be checked. A cleaner way, to avoid this pitfall safely, would
 * be to limit the usage of this function to only public APIs, and that
 * too at the very end, just before returning.
 *
 * NOTE: This function should be called only after ensuring that the
 * context is valid and the control mutex is locked. */
static void execute_event_cb(wifi_prov_cb_event_t event_id, void *event_data, size_t event_data_size)
{
    ESP_LOGD(TAG, "execute_event_cb : %d", event_id);

    if (prov_ctx) {
        wifi_prov_cb_func_t app_cb = prov_ctx->mgr_config.app_event_handler.event_cb;
        void *app_data = prov_ctx->mgr_config.app_event_handler.user_data;

        wifi_prov_cb_func_t scheme_cb = prov_ctx->mgr_config.scheme_event_handler.event_cb;
        void *scheme_data = prov_ctx->mgr_config.scheme_event_handler.user_data;

        /* Release the mutex before executing the callbacks. This is done so that
         * wifi_prov_mgr_event_handler() doesn't stay blocked for the duration */
        RELEASE_LOCK(prov_ctx_lock);

        if (scheme_cb) {
            /* Call scheme specific event handler */
            scheme_cb(scheme_data, event_id, event_data);
        }

        if (app_cb) {
            /* Call application specific event handler */
            app_cb(app_data, event_id, event_data);
        }

        if (esp_event_post(WIFI_PROV_EVENT, event_id,
                           event_data, event_data_size,
                           portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post event %d to default event loop", event_id);
        }

        ACQUIRE_LOCK(prov_ctx_lock);
    }
}

esp_err_t wifi_prov_mgr_set_app_info(const char *label, const char *version,
                                     const char**capabilities, size_t total_capabilities)
{
    if (!label || !version || !capabilities) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_FAIL;
    ACQUIRE_LOCK(prov_ctx_lock);

    if (prov_ctx && prov_ctx->prov_state == WIFI_PROV_STATE_IDLE) {
        if (!prov_ctx->app_info_json) {
            prov_ctx->app_info_json = cJSON_CreateObject();
        }

        cJSON *new_entry_json = cJSON_CreateObject();
        cJSON *capabilities_json = cJSON_CreateArray();
        cJSON_AddItemToObject(prov_ctx->app_info_json, label, new_entry_json);

        /* Version ("ver") */
        cJSON_AddStringToObject(new_entry_json, "ver", version);

        /* List of capabilities ("cap") */
        cJSON_AddItemToObject(new_entry_json, "cap", capabilities_json);
        for (unsigned int i = 0; i < total_capabilities; i++) {
            if (capabilities[i]) {
                cJSON_AddItemToArray(capabilities_json, cJSON_CreateString(capabilities[i]));
            }
        }
        ret = ESP_OK;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }

    RELEASE_LOCK(prov_ctx_lock);
    return ret;
}

static cJSON* wifi_prov_get_info_json(void)
{
    cJSON *full_info_json = prov_ctx->app_info_json ?
                                cJSON_Duplicate(prov_ctx->app_info_json, 1) : cJSON_CreateObject();
    cJSON *prov_info_json = cJSON_CreateObject();
    cJSON *prov_capabilities = cJSON_CreateArray();

    /* Use label "prov" to indicate provisioning related information */
    cJSON_AddItemToObject(full_info_json, "prov", prov_info_json);

    /* Version field */
    cJSON_AddStringToObject(prov_info_json, "ver", prov_ctx->mgr_info.version);

    cJSON_AddNumberToObject(prov_info_json, "sec_ver", prov_ctx->security);
    /* Capabilities field */
    cJSON_AddItemToObject(prov_info_json, "cap", prov_capabilities);

    /* If Security / Proof of Possession is not used, indicate in capabilities */
    if (prov_ctx->mgr_info.capabilities.no_sec) {
        cJSON_AddItemToArray(prov_capabilities, cJSON_CreateString("no_sec"));
    } else if (prov_ctx->mgr_info.capabilities.no_pop) {
        cJSON_AddItemToArray(prov_capabilities, cJSON_CreateString("no_pop"));
    }

    /* Indicate capability for performing Wi-Fi scan */
    cJSON_AddItemToArray(prov_capabilities, cJSON_CreateString("wifi_scan"));
    return full_info_json;
}

/* Declare the internal event handler */
static void wifi_prov_mgr_event_handler_internal(void* arg, esp_event_base_t event_base,
                                                 int32_t event_id, void* event_data);

static esp_err_t wifi_prov_mgr_start_service(const char *service_name, const char *service_key)
{
    const wifi_prov_scheme_t *scheme = &prov_ctx->mgr_config.scheme;
    esp_err_t ret;

    /* Create new protocomm instance */
    prov_ctx->pc = protocomm_new();
    if (prov_ctx->pc == NULL) {
        ESP_LOGE(TAG, "Failed to create new protocomm instance");
        return ESP_FAIL;
    }

    ret = scheme->set_config_service(prov_ctx->prov_scheme_config, service_name, service_key);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure service");
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    /* Start provisioning */
    ret = scheme->prov_start(prov_ctx->pc, prov_ctx->prov_scheme_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start service");
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    /* Set version information / capabilities of provisioning service and application */
    cJSON *version_json = wifi_prov_get_info_json();
    char *version_str = cJSON_Print(version_json);
    ret = protocomm_set_version(prov_ctx->pc, "proto-ver", version_str);
    free(version_str);
    cJSON_Delete(version_json);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set version endpoint");
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    /* Set protocomm security type for endpoint */
    if (prov_ctx->security == 0) {
#ifdef CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0
        ret = protocomm_set_security(prov_ctx->pc, "prov-session",
                                     &protocomm_security0, NULL);
#else
        // Enable SECURITY_VERSION_0 in Protocomm configuration menu
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else if (prov_ctx->security == 1) {
#ifdef CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1
        ret = protocomm_set_security(prov_ctx->pc, "prov-session",
                                     &protocomm_security1, prov_ctx->protocomm_sec_params);
#else
        // Enable SECURITY_VERSION_1 in Protocomm configuration menu
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else if (prov_ctx->security == 2) {
#ifdef CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2
        ret = protocomm_set_security(prov_ctx->pc, "prov-session",
                                     &protocomm_security2, prov_ctx->protocomm_sec_params);
#else
        // Enable SECURITY_VERSION_2 in Protocomm configuration menu
        return ESP_ERR_NOT_SUPPORTED;
#endif
    } else {
        ESP_LOGE(TAG, "Unsupported protocomm security version %d", prov_ctx->security);
        ret = ESP_ERR_INVALID_ARG;
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set security endpoint");
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    prov_ctx->wifi_prov_handlers = malloc(sizeof(wifi_prov_config_handlers_t));
    ret = get_wifi_prov_handlers(prov_ctx->wifi_prov_handlers);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "Failed to allocate memory for provisioning handlers");
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ESP_ERR_NO_MEM;
    }

    /* Add protocomm endpoint for Wi-Fi station configuration */
    ret = protocomm_add_endpoint(prov_ctx->pc, "prov-config",
                                 wifi_prov_config_data_handler,
                                 prov_ctx->wifi_prov_handlers);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set provisioning endpoint");
        free(prov_ctx->wifi_prov_handlers);
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    prov_ctx->wifi_scan_handlers = malloc(sizeof(wifi_prov_scan_handlers_t));
    ret = get_wifi_scan_handlers(prov_ctx->wifi_scan_handlers);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "Failed to allocate memory for Wi-Fi scan handlers");
        free(prov_ctx->wifi_prov_handlers);
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ESP_ERR_NO_MEM;
    }

    /* Add endpoint for scanning Wi-Fi APs and sending scan list */
    ret = protocomm_add_endpoint(prov_ctx->pc, "prov-scan",
                                 wifi_prov_scan_handler,
                                 prov_ctx->wifi_scan_handlers);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi scan endpoint");
        free(prov_ctx->wifi_scan_handlers);
        free(prov_ctx->wifi_prov_handlers);
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    prov_ctx->wifi_ctrl_handlers = malloc(sizeof(wifi_ctrl_handlers_t));
    ret = get_wifi_ctrl_handlers(prov_ctx->wifi_ctrl_handlers);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to allocate memory for Wi-Fi ctrl handlers");
        free(prov_ctx->wifi_prov_handlers);
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ESP_ERR_NO_MEM;
    }

    /* Add endpoint for controlling state of Wi-Fi station */
    ret = protocomm_add_endpoint(prov_ctx->pc, "prov-ctrl",
                                 wifi_ctrl_handler,
                                 prov_ctx->wifi_ctrl_handlers);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi ctrl endpoint");
        free(prov_ctx->wifi_ctrl_handlers);
        free(prov_ctx->wifi_prov_handlers);
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    /* Register global event handler */
    ret = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                     wifi_prov_mgr_event_handler_internal, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register WiFi event handler");
        free(prov_ctx->wifi_scan_handlers);
        free(prov_ctx->wifi_ctrl_handlers);
        free(prov_ctx->wifi_prov_handlers);
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    ret = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                     wifi_prov_mgr_event_handler_internal, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register IP event handler");
        esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                     wifi_prov_mgr_event_handler_internal);
        free(prov_ctx->wifi_scan_handlers);
        free(prov_ctx->wifi_ctrl_handlers);
        free(prov_ctx->wifi_prov_handlers);
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ret;
    }

    ret = esp_event_handler_register(WIFI_PROV_MGR_PVT_EVENT, WIFI_PROV_MGR_STOP,
                                     wifi_prov_mgr_event_handler_internal, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register provisioning event handler");
        esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                     wifi_prov_mgr_event_handler_internal);
        esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                     wifi_prov_mgr_event_handler_internal);
        free(prov_ctx->wifi_scan_handlers);
        free(prov_ctx->wifi_ctrl_handlers);
        free(prov_ctx->wifi_prov_handlers);
        scheme->prov_stop(prov_ctx->pc);
        protocomm_delete(prov_ctx->pc);
        return ret;
    }


    ESP_LOGI(TAG, "Provisioning started with service name : %s ",
             service_name ? service_name : "<NULL>");
    return ESP_OK;
}

esp_err_t wifi_prov_mgr_endpoint_create(const char *ep_name)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ESP_FAIL;

    ACQUIRE_LOCK(prov_ctx_lock);
    if (prov_ctx &&
        prov_ctx->prov_state == WIFI_PROV_STATE_IDLE) {
        err = prov_ctx->mgr_config.scheme.set_config_endpoint(
                prov_ctx->prov_scheme_config, ep_name,
                prov_ctx->endpoint_uuid_used + 1);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create additional endpoint");
    } else {
        prov_ctx->endpoint_uuid_used++;
    }
    RELEASE_LOCK(prov_ctx_lock);
    return err;
}

esp_err_t wifi_prov_mgr_endpoint_register(const char *ep_name, protocomm_req_handler_t handler, void *user_ctx)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ESP_FAIL;

    ACQUIRE_LOCK(prov_ctx_lock);
    if (prov_ctx &&
        prov_ctx->prov_state > WIFI_PROV_STATE_STARTING &&
        prov_ctx->prov_state < WIFI_PROV_STATE_STOPPING) {
        err = protocomm_add_endpoint(prov_ctx->pc, ep_name, handler, user_ctx);
    }
    RELEASE_LOCK(prov_ctx_lock);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register handler for endpoint");
    }
    return err;
}

void wifi_prov_mgr_endpoint_unregister(const char *ep_name)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return;
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (prov_ctx &&
        prov_ctx->prov_state > WIFI_PROV_STATE_STARTING &&
        prov_ctx->prov_state < WIFI_PROV_STATE_STOPPING) {
        protocomm_remove_endpoint(prov_ctx->pc, ep_name);
    }
    RELEASE_LOCK(prov_ctx_lock);
}

static void prov_stop_and_notify(bool is_async)
{
    esp_event_handler_unregister(WIFI_PROV_MGR_PVT_EVENT, WIFI_PROV_MGR_STOP,
                                 wifi_prov_mgr_event_handler_internal);

    if (prov_ctx->cleanup_delay_timer) {
        esp_timer_stop(prov_ctx->cleanup_delay_timer);
        esp_timer_delete(prov_ctx->cleanup_delay_timer);
        prov_ctx->cleanup_delay_timer = NULL;
    }

    wifi_prov_cb_func_t app_cb = prov_ctx->mgr_config.app_event_handler.event_cb;
    void *app_data = prov_ctx->mgr_config.app_event_handler.user_data;

    wifi_prov_cb_func_t scheme_cb = prov_ctx->mgr_config.scheme_event_handler.event_cb;
    void *scheme_data = prov_ctx->mgr_config.scheme_event_handler.user_data;

    /* This delay is so that the client side app is notified first
     * and then the provisioning is stopped. Generally 1000ms is enough. */
    if (!is_async)
    {
        uint32_t cleanup_delay = prov_ctx->cleanup_delay > 100 ? prov_ctx->cleanup_delay : 100;
        vTaskDelay(cleanup_delay / portTICK_PERIOD_MS);
    }

    protocomm_remove_endpoint(prov_ctx->pc, "prov-ctrl");

    protocomm_remove_endpoint(prov_ctx->pc, "prov-scan");

    protocomm_remove_endpoint(prov_ctx->pc, "prov-config");

    protocomm_unset_security(prov_ctx->pc, "prov-session");

    protocomm_unset_version(prov_ctx->pc, "proto-ver");

    /* All the extra application added endpoints are also
     * removed automatically when prov_stop is called */
    prov_ctx->mgr_config.scheme.prov_stop(prov_ctx->pc);

    /* Delete protocomm instance */
    protocomm_delete(prov_ctx->pc);
    prov_ctx->pc = NULL;

    /* Free provisioning handlers */
    free(prov_ctx->wifi_prov_handlers->ctx);
    free(prov_ctx->wifi_prov_handlers);
    prov_ctx->wifi_prov_handlers = NULL;

    free(prov_ctx->wifi_scan_handlers->ctx);
    free(prov_ctx->wifi_scan_handlers);
    prov_ctx->wifi_scan_handlers = NULL;

    free(prov_ctx->wifi_ctrl_handlers);
    prov_ctx->wifi_ctrl_handlers = NULL;

    /* Switch device to Wi-Fi STA mode irrespective of
     * whether provisioning was completed or not */
    esp_wifi_set_mode(WIFI_MODE_STA);
    ESP_LOGI(TAG, "Provisioning stopped");

    if (is_async) {
        /* NOTE: While calling this API in an async fashion,
         * the context lock prov_ctx_lock has already been taken
         */
        prov_ctx->prov_state = WIFI_PROV_STATE_IDLE;

        ESP_LOGD(TAG, "execute_event_cb : %d", WIFI_PROV_END);
        if (scheme_cb) {
            scheme_cb(scheme_data, WIFI_PROV_END, NULL);
        }
        if (app_cb) {
            app_cb(app_data, WIFI_PROV_END, NULL);
        }
        if (esp_event_post(WIFI_PROV_EVENT, WIFI_PROV_END, NULL, 0, portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post event WIFI_PROV_END");
        }
    }
}

/* This will do one of these:
 * 1) if blocking is false, start a cleanup timer for stopping the provisioning service (returns true)
 * 2) if blocking is true, stop provisioning service immediately (returns true)
 * 3) if service was already in the process of termination, in blocking mode this will
 *    wait till the service is stopped (returns false)
 * 4) if service was not running, this will return immediately (returns false)
 *
 * NOTE: This function should be called only after ensuring that the context
 * is valid and the control mutex is locked
 *
 * NOTE: When blocking mode is selected, the event callbacks are not executed.
 * This help with de-initialization.
 */
static bool wifi_prov_mgr_stop_service(bool blocking)
{
    if (blocking) {
        /* Wait for any ongoing calls to wifi_prov_mgr_start_service() or
         * wifi_prov_mgr_stop_service() from another thread to finish */
        while (prov_ctx && (
            prov_ctx->prov_state == WIFI_PROV_STATE_STARTING ||
            prov_ctx->prov_state == WIFI_PROV_STATE_STOPPING)) {
            RELEASE_LOCK(prov_ctx_lock);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            ACQUIRE_LOCK(prov_ctx_lock);
        }
    } else {
        /* Wait for any ongoing call to wifi_prov_mgr_start_service()
         * from another thread to finish */
        while (prov_ctx &&
            prov_ctx->prov_state == WIFI_PROV_STATE_STARTING) {
            RELEASE_LOCK(prov_ctx_lock);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            ACQUIRE_LOCK(prov_ctx_lock);
        }

        if (prov_ctx && prov_ctx->prov_state == WIFI_PROV_STATE_STOPPING) {
            ESP_LOGD(TAG, "Provisioning is already stopping");
            return false;
        }
    }

    if (!prov_ctx || prov_ctx->prov_state == WIFI_PROV_STATE_IDLE) {
        ESP_LOGD(TAG, "Provisioning not running");
        return false;
    }

    /* Timers not needed anymore */
    if (prov_ctx->autostop_timer) {
        esp_timer_stop(prov_ctx->autostop_timer);
        esp_timer_delete(prov_ctx->autostop_timer);
        prov_ctx->autostop_timer = NULL;
    }

    if (prov_ctx->wifi_connect_timer) {
        esp_timer_stop(prov_ctx->wifi_connect_timer);
        esp_timer_delete(prov_ctx->wifi_connect_timer);
        prov_ctx->wifi_connect_timer = NULL;
    }

    ESP_LOGD(TAG, "Stopping provisioning");
    prov_ctx->prov_state = WIFI_PROV_STATE_STOPPING;

    /* Free proof of possession */
    if (prov_ctx->protocomm_sec_params) {
        if (prov_ctx->security == 1) {
            // In case of security 1 we keep an internal copy of "pop".
            // Hence free it at this point
            uint8_t *pop = (uint8_t *)((protocomm_security1_params_t *) prov_ctx->protocomm_sec_params)->data;
            free(pop);
        }
        prov_ctx->protocomm_sec_params = NULL;
    }

    /* Delete all scan results */
    for (uint16_t channel = 0; channel < 14; channel++) {
        free(prov_ctx->ap_list[channel]);
        prov_ctx->ap_list[channel] = NULL;
    }
    prov_ctx->scanning = false;
    for (uint8_t i = 0; i < MAX_SCAN_RESULTS; i++) {
        prov_ctx->ap_list_sorted[i] = NULL;
    }

    /* Remove event handler */
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                 wifi_prov_mgr_event_handler_internal);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                 wifi_prov_mgr_event_handler_internal);

    if (blocking) {
        /* Run the cleanup without launching a separate task. Also the
         * WIFI_PROV_END event is not emitted in this case */
        RELEASE_LOCK(prov_ctx_lock);
        prov_stop_and_notify(false);
        ACQUIRE_LOCK(prov_ctx_lock);
        prov_ctx->prov_state = WIFI_PROV_STATE_IDLE;
    } else {
        /* Launch cleanup timer to perform the cleanup asynchronously.
         * It is important to do this asynchronously because, there are
         * situations in which the transport level resources have to be
         * released - some duration after - returning from a call to
         * wifi_prov_mgr_stop_provisioning(), like when it is called
         * inside a protocomm handler */
        uint64_t cleanup_delay_ms = prov_ctx->cleanup_delay > 100 ? prov_ctx->cleanup_delay : 100;
        esp_timer_start_once(prov_ctx->cleanup_delay_timer, cleanup_delay_ms * 1000U);
        ESP_LOGD(TAG, "Provisioning scheduled for stopping");
    }
    return true;
}

/* Task spawned by timer callback */
static void stop_prov_timer_cb(void *arg)
{
    wifi_prov_mgr_stop_provisioning();
}

static void cleanup_delay_timer_cb(void *arg)
{
    esp_err_t ret = esp_event_post(WIFI_PROV_MGR_PVT_EVENT, WIFI_PROV_MGR_STOP, NULL, 0, pdMS_TO_TICKS(100));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to post WIFI_PROV_MGR_STOP event! %d %s", ret, esp_err_to_name(ret));
    }
}

esp_err_t wifi_prov_mgr_disable_auto_stop(uint32_t cleanup_delay)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_FAIL;
    ACQUIRE_LOCK(prov_ctx_lock);

    if (prov_ctx && prov_ctx->prov_state == WIFI_PROV_STATE_IDLE) {
        prov_ctx->mgr_info.capabilities.no_auto_stop = true;
        prov_ctx->cleanup_delay = cleanup_delay;
        ret = ESP_OK;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }

    RELEASE_LOCK(prov_ctx_lock);
    return ret;
}

/* Call this if provisioning is completed before the timeout occurs */
esp_err_t wifi_prov_mgr_done(void)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    bool auto_stop_enabled = false;
    ACQUIRE_LOCK(prov_ctx_lock);
    if (prov_ctx && !prov_ctx->mgr_info.capabilities.no_auto_stop) {
        auto_stop_enabled = true;
    }
    RELEASE_LOCK(prov_ctx_lock);

    /* Stop provisioning if auto stop is enabled */
    if (auto_stop_enabled) {
        wifi_prov_mgr_stop_provisioning();
    }
    return ESP_OK;
}

static esp_err_t update_wifi_scan_results(void)
{
    if (!prov_ctx->scanning) {
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGD(TAG, "Scan finished");

    esp_err_t ret = ESP_FAIL;
    uint16_t count = 0;
    uint16_t curr_channel = prov_ctx->curr_channel;

    if (prov_ctx->ap_list[curr_channel]) {
        free(prov_ctx->ap_list[curr_channel]);
        prov_ctx->ap_list[curr_channel] = NULL;
        prov_ctx->ap_list_len[curr_channel] = 0;
    }

    if (esp_wifi_scan_get_ap_num(&count) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get count of scanned APs");
        goto exit;
    }

    if (!count) {
        ESP_LOGD(TAG, "Scan result empty");
        ret = ESP_OK;
        goto exit;
    }

    uint16_t get_count = MIN(count, MAX_SCAN_RESULTS);
    prov_ctx->ap_list[curr_channel] = (wifi_ap_record_t *) calloc(get_count, sizeof(wifi_ap_record_t));
    if (!prov_ctx->ap_list[curr_channel]) {
        ESP_LOGE(TAG, "Failed to allocate memory for AP list");
        esp_wifi_clear_ap_list();
        goto exit;
    }
    if (esp_wifi_scan_get_ap_records(&get_count, prov_ctx->ap_list[curr_channel]) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get scanned AP records");
        goto exit;
    }
    prov_ctx->ap_list_len[curr_channel] = get_count;

    if (prov_ctx->channels_per_group) {
        ESP_LOGD(TAG, "Scan results for channel %d :", curr_channel);
    } else {
        ESP_LOGD(TAG, "Scan results :");
    }
    ESP_LOGD(TAG, "\tS.N. %-32s %-12s %s %s", "SSID", "BSSID", "RSSI", "AUTH");
    for (uint8_t i = 0; i < prov_ctx->ap_list_len[curr_channel]; i++) {
        ESP_LOGD(TAG, "\t[%2d] %-32s %02x%02x%02x%02x%02x%02x %4d %4d", i,
                 prov_ctx->ap_list[curr_channel][i].ssid,
                 prov_ctx->ap_list[curr_channel][i].bssid[0],
                 prov_ctx->ap_list[curr_channel][i].bssid[1],
                 prov_ctx->ap_list[curr_channel][i].bssid[2],
                 prov_ctx->ap_list[curr_channel][i].bssid[3],
                 prov_ctx->ap_list[curr_channel][i].bssid[4],
                 prov_ctx->ap_list[curr_channel][i].bssid[5],
                 prov_ctx->ap_list[curr_channel][i].rssi,
                 prov_ctx->ap_list[curr_channel][i].authmode);
    }

    /* Store results in sorted list */
    {
        int rc = get_count;
        int is = MAX_SCAN_RESULTS - rc - 1;
        while (rc > 0 && is >= 0) {
            if (prov_ctx->ap_list_sorted[is]) {
                if (prov_ctx->ap_list_sorted[is]->rssi > prov_ctx->ap_list[curr_channel][rc - 1].rssi) {
                    prov_ctx->ap_list_sorted[is + rc] = &prov_ctx->ap_list[curr_channel][rc - 1];
                    rc--;
                    continue;
                }
                prov_ctx->ap_list_sorted[is + rc] = prov_ctx->ap_list_sorted[is];
            }
            is--;
        }
        while (rc > 0) {
            prov_ctx->ap_list_sorted[rc - 1] = &prov_ctx->ap_list[curr_channel][rc - 1];
            rc--;
        }
    }

    ret = ESP_OK;
    exit:

    if (!prov_ctx->channels_per_group) {
        /* All channel scan was performed
         * so nothing more to do */
        prov_ctx->scanning = false;
        goto final;
    }

    curr_channel = prov_ctx->curr_channel = (prov_ctx->curr_channel + 1) % 14;
    if (ret != ESP_OK || curr_channel == 0) {
        prov_ctx->scanning = false;
        goto final;
    }

    if ((curr_channel % prov_ctx->channels_per_group) == 0) {
        vTaskDelay(120 / portTICK_PERIOD_MS);
    }

    ESP_LOGD(TAG, "Scan starting on channel %u...", curr_channel);
    prov_ctx->scan_cfg.channel = curr_channel;
    ret = esp_wifi_scan_start(&prov_ctx->scan_cfg, false);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start scan");
        prov_ctx->scanning = false;
        goto final;
    }
    ESP_LOGD(TAG, "Scan started");

    final:

    return ret;
}

static void wifi_prov_mgr_event_handler_internal(
    void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return;
    }

    ACQUIRE_LOCK(prov_ctx_lock);

    /* If pointer to provisioning application data is NULL
     * then provisioning manager is not running, therefore
     * return with error to allow the global handler to act */
    if (!prov_ctx) {
        RELEASE_LOCK(prov_ctx_lock);
        return;
    }

    /* If scan completed then update scan result */
    if (prov_ctx->prov_state == WIFI_PROV_STATE_STARTED &&
        event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_SCAN_DONE) {
        update_wifi_scan_results();
    }

    /* Only handle events when credential is received and
     * Wi-Fi STA is yet to complete trying the connection */
    if (prov_ctx->prov_state < WIFI_PROV_STATE_CRED_RECV) {
        RELEASE_LOCK(prov_ctx_lock);
        return;
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "STA Start");
        /* Once configuration is received through protocomm,
         * device is started as station. Once station starts,
         * wait for connection to establish with configured
         * host SSID and password */
        prov_ctx->wifi_state = WIFI_PROV_STA_CONNECTING;
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "STA Got IP");
        /* Station got IP. That means configuration is successful. */
        prov_ctx->wifi_state = WIFI_PROV_STA_CONNECTED;
        prov_ctx->prov_state = WIFI_PROV_STATE_SUCCESS;

        /* If auto stop is enabled (default), schedule timer to
         * stop provisioning after configured timeout. */
        if (!prov_ctx->mgr_info.capabilities.no_auto_stop) {
            ESP_LOGD(TAG, "Starting %d sec timer for stop_prov_timer_cb()",
                        CONFIG_WIFI_PROV_AUTOSTOP_TIMEOUT);
            esp_timer_start_once(prov_ctx->autostop_timer, CONFIG_WIFI_PROV_AUTOSTOP_TIMEOUT * 1000000U);
        }

        /* Execute user registered callback handler */
        execute_event_cb(WIFI_PROV_CRED_SUCCESS, NULL, 0);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGE(TAG, "STA Disconnected");
        /* Station couldn't connect to configured host SSID */
        prov_ctx->wifi_state = WIFI_PROV_STA_DISCONNECTED;

        wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*) event_data;
        ESP_LOGE(TAG, "Disconnect reason : %d", disconnected->reason);

        /* Set code corresponding to the reason for disconnection */
        switch (disconnected->reason) {
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        case WIFI_REASON_AUTH_FAIL:
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
        case WIFI_REASON_MIC_FAILURE:
            ESP_LOGE(TAG, "STA Auth Error");
            prov_ctx->wifi_disconnect_reason = WIFI_PROV_STA_AUTH_ERROR;
            break;
        case WIFI_REASON_NO_AP_FOUND:
            ESP_LOGE(TAG, "STA AP Not found");
            prov_ctx->wifi_disconnect_reason = WIFI_PROV_STA_AP_NOT_FOUND;
            break;
        default:
            /* If none of the expected reasons,
             * retry connecting to host SSID */
            prov_ctx->wifi_state = WIFI_PROV_STA_CONNECTING;
            esp_wifi_connect();
        }

        /* In case of disconnection, update state of service and
         * run the event handler with disconnection reason as data */
        if (prov_ctx->wifi_state == WIFI_PROV_STA_DISCONNECTED) {
            prov_ctx->prov_state = WIFI_PROV_STATE_FAIL;
            wifi_prov_sta_fail_reason_t reason = prov_ctx->wifi_disconnect_reason;
            /* Execute user registered callback handler */
            execute_event_cb(WIFI_PROV_CRED_FAIL, (void *)&reason, sizeof(reason));
        }
    } else if (event_base == WIFI_PROV_MGR_PVT_EVENT && event_id == WIFI_PROV_MGR_STOP) {
        prov_stop_and_notify(true);
    }

    RELEASE_LOCK(prov_ctx_lock);
}

esp_err_t wifi_prov_mgr_wifi_scan_start(bool blocking, bool passive,
                                        uint8_t group_channels, uint32_t period_ms)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    ACQUIRE_LOCK(prov_ctx_lock);

    if (!prov_ctx) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_ERR_INVALID_STATE;
    }

    if (prov_ctx->scanning) {
        ESP_LOGD(TAG, "Scan already running");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_OK;
    }

    /* Clear sorted list for new entries */
    for (uint8_t i = 0; i < MAX_SCAN_RESULTS; i++) {
        prov_ctx->ap_list_sorted[i] = NULL;
    }

    if (passive) {
        prov_ctx->scan_cfg.scan_type = WIFI_SCAN_TYPE_PASSIVE;
/* We do not recommend scan configuration modification in Wi-Fi and BT coexistence mode */
#if !CONFIG_BT_ENABLED
        prov_ctx->scan_cfg.scan_time.passive = period_ms;
#endif
    } else {
        prov_ctx->scan_cfg.scan_type = WIFI_SCAN_TYPE_ACTIVE;
/* We do not recommend scan configuration modification in Wi-Fi and BT coexistence mode */
#if !CONFIG_BT_ENABLED
        prov_ctx->scan_cfg.scan_time.active.min = period_ms;
        prov_ctx->scan_cfg.scan_time.active.max = period_ms;
#endif
    }
    prov_ctx->channels_per_group = group_channels;

    if (prov_ctx->channels_per_group) {
        ESP_LOGD(TAG, "Scan starting on channel 1...");
        prov_ctx->scan_cfg.channel = 1;
    } else {
        ESP_LOGD(TAG, "Scan starting...");
        prov_ctx->scan_cfg.channel = 0;
    }

    if (esp_wifi_scan_start(&prov_ctx->scan_cfg, false) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start scan");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Scan started");
    prov_ctx->scanning = true;
    prov_ctx->curr_channel = prov_ctx->scan_cfg.channel;
    RELEASE_LOCK(prov_ctx_lock);

    /* If scan is to be non-blocking, return immediately */
    if (!blocking) {
        return ESP_OK;
    }

    /* Loop till scan is complete */
    bool scanning = true;
    while (scanning) {
        ACQUIRE_LOCK(prov_ctx_lock);
        scanning = (prov_ctx && prov_ctx->scanning);
        RELEASE_LOCK(prov_ctx_lock);

        /* 120ms delay is  sufficient for Wi-Fi beacons to be sent */
        vTaskDelay(120 / portTICK_PERIOD_MS);
    }
    return ESP_OK;
}

bool wifi_prov_mgr_wifi_scan_finished(void)
{
    bool scan_finished = true;
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return scan_finished;
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (!prov_ctx) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        RELEASE_LOCK(prov_ctx_lock);
        return scan_finished;
    }

    scan_finished = !prov_ctx->scanning;
    RELEASE_LOCK(prov_ctx_lock);
    return scan_finished;
}

uint16_t wifi_prov_mgr_wifi_scan_result_count(void)
{
    uint16_t rval = 0;
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return rval;
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (!prov_ctx) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        RELEASE_LOCK(prov_ctx_lock);
        return rval;
    }

    while (rval < MAX_SCAN_RESULTS) {
        if (!prov_ctx->ap_list_sorted[rval]) {
            break;
        }
        rval++;
    }
    RELEASE_LOCK(prov_ctx_lock);
    return rval;
}

const wifi_ap_record_t *wifi_prov_mgr_wifi_scan_result(uint16_t index)
{
    const wifi_ap_record_t *rval = NULL;
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return rval;
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (!prov_ctx) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        RELEASE_LOCK(prov_ctx_lock);
        return rval;
    }

    if (index < MAX_SCAN_RESULTS) {
        rval = prov_ctx->ap_list_sorted[index];
    }
    RELEASE_LOCK(prov_ctx_lock);
    return rval;
}

esp_err_t wifi_prov_mgr_get_wifi_state(wifi_prov_sta_state_t *state)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (prov_ctx == NULL || state == NULL) {
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }

    *state = prov_ctx->wifi_state;
    RELEASE_LOCK(prov_ctx_lock);
    return ESP_OK;
}

esp_err_t wifi_prov_mgr_get_wifi_disconnect_reason(wifi_prov_sta_fail_reason_t *reason)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (prov_ctx == NULL || reason == NULL) {
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }

    if (prov_ctx->wifi_state != WIFI_PROV_STA_DISCONNECTED) {
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }

    *reason = prov_ctx->wifi_disconnect_reason;
    RELEASE_LOCK(prov_ctx_lock);
    return ESP_OK;
}

static void debug_print_wifi_credentials(wifi_sta_config_t sta, const char* pretext)
{
    size_t passlen = strlen((const char*) sta.password);
    ESP_LOGD(TAG, "%s Wi-Fi SSID     : %.*s", pretext,
             strnlen((const char *) sta.ssid, sizeof(sta.ssid)), (const char *) sta.ssid);

    if (passlen) {
        /* Mask password partially if longer than 3, else mask it completely */
        memset(sta.password + (passlen > 3), '*', passlen - 2*(passlen > 3));
        ESP_LOGD(TAG, "%s Wi-Fi Password : %s", pretext, (const char *) sta.password);
    }
}

esp_err_t wifi_prov_mgr_is_provisioned(bool *provisioned)
{
    if (!provisioned) {
        return ESP_ERR_INVALID_ARG;
    }

    *provisioned = false;

    /* Get Wi-Fi Station configuration */
    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }

    if (strlen((const char *) wifi_cfg.sta.ssid)) {
        *provisioned = true;
        debug_print_wifi_credentials(wifi_cfg.sta, "Found");
    }
    return ESP_OK;
}

bool wifi_prov_mgr_is_sm_idle(void)
{
    return (prov_ctx->prov_state == WIFI_PROV_STATE_IDLE);
}

static void wifi_connect_timer_cb(void *arg)
{
    if (esp_wifi_connect() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect Wi-Fi");
    }
}

esp_err_t wifi_prov_mgr_configure_sta(wifi_config_t *wifi_cfg)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (!prov_ctx) {
        ESP_LOGE(TAG, "Invalid state of Provisioning app");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }
    if (prov_ctx->prov_state >= WIFI_PROV_STATE_CRED_RECV) {
        ESP_LOGE(TAG, "Wi-Fi credentials already received by provisioning app");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }
    debug_print_wifi_credentials(wifi_cfg->sta, "Received");

    /* Configure Wi-Fi as both AP and/or Station */
    if (esp_wifi_set_mode(prov_ctx->mgr_config.scheme.wifi_mode) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi mode");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }

    /* Don't release mutex yet as it is possible that right after
     * esp_wifi_connect()  is called below, the related Wi-Fi event
     * happens even before manager state is updated in the next
     * few lines causing the internal event handler to miss */

    /* Set Wi-Fi storage again to flash to keep the newly
     * provided credentials on NVS */
    if (esp_wifi_set_storage(WIFI_STORAGE_FLASH) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set storage Wi-Fi");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }
    /* Configure Wi-Fi station with host credentials
     * provided during provisioning */
    if (esp_wifi_set_config(WIFI_IF_STA, wifi_cfg) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi configuration");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }
    /* Connect to AP after one second so that the response can
     * be sent to the client successfully, before a channel change happens*/
    if (esp_timer_start_once(prov_ctx->wifi_connect_timer, 1000 * 1000U) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi connect timer");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_FAIL;
    }

    /* Reset Wi-Fi station state for provisioning app */
    prov_ctx->wifi_state = WIFI_PROV_STA_CONNECTING;
    prov_ctx->prov_state = WIFI_PROV_STATE_CRED_RECV;
    /* Execute user registered callback handler */
    execute_event_cb(WIFI_PROV_CRED_RECV, (void *)&wifi_cfg->sta, sizeof(wifi_cfg->sta));
    RELEASE_LOCK(prov_ctx_lock);

    return ESP_OK;
}

esp_err_t wifi_prov_mgr_init(wifi_prov_mgr_config_t config)
{
    if (!prov_ctx_lock) {
       /* Create mutex if this is the first time init is being called.
        * This is created only once and never deleted because if some
        * other thread is trying to take this mutex while it is being
        * deleted from another thread then the reference may become
        * invalid and cause exception */
        prov_ctx_lock = xSemaphoreCreateMutex();
        if (!prov_ctx_lock) {
            ESP_LOGE(TAG, "Failed to create mutex");
            return ESP_ERR_NO_MEM;
        }
    }

    void *fn_ptrs[] = {
        config.scheme.prov_stop,
        config.scheme.prov_start,
        config.scheme.new_config,
        config.scheme.delete_config,
        config.scheme.set_config_service,
        config.scheme.set_config_endpoint
    };

    /* All function pointers in the scheme structure must be non-null */
    for (size_t i = 0; i < sizeof(fn_ptrs)/sizeof(fn_ptrs[0]); i++) {
        if (!fn_ptrs[i]) {
            return ESP_ERR_INVALID_ARG;
        }
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (prov_ctx) {
        ESP_LOGE(TAG, "Provisioning manager already initialized");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_ERR_INVALID_STATE;
    }

    /* Allocate memory for provisioning app data */
    prov_ctx = (struct wifi_prov_mgr_ctx *) calloc(1, sizeof(struct wifi_prov_mgr_ctx));
    if (!prov_ctx) {
        ESP_LOGE(TAG, "Error allocating memory for singleton instance");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_ERR_NO_MEM;
    }

    prov_ctx->mgr_config = config;
    prov_ctx->prov_state = WIFI_PROV_STATE_IDLE;
    prov_ctx->mgr_info.version = WIFI_PROV_MGR_VERSION;

    /* Allocate memory for provisioning scheme configuration */
    const wifi_prov_scheme_t *scheme = &prov_ctx->mgr_config.scheme;
    esp_err_t ret = ESP_OK;
    prov_ctx->prov_scheme_config = scheme->new_config();
    if (!prov_ctx->prov_scheme_config) {
        ESP_LOGE(TAG, "failed to allocate provisioning scheme configuration");
        ret = ESP_ERR_NO_MEM;
        goto exit;
    }

    ret = scheme->set_config_endpoint(prov_ctx->prov_scheme_config, "prov-ctrl", 0xFF4F);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to configure Wi-Fi state control endpoint");
        goto exit;
    }

    ret = scheme->set_config_endpoint(prov_ctx->prov_scheme_config, "prov-scan", 0xFF50);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to configure Wi-Fi scanning endpoint");
        goto exit;
    }

    ret = scheme->set_config_endpoint(prov_ctx->prov_scheme_config, "prov-session", 0xFF51);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to configure security endpoint");
        goto exit;
    }

    ret = scheme->set_config_endpoint(prov_ctx->prov_scheme_config, "prov-config", 0xFF52);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to configure Wi-Fi configuration endpoint");
        goto exit;
    }

    ret = scheme->set_config_endpoint(prov_ctx->prov_scheme_config, "proto-ver", 0xFF53);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to configure version endpoint");
        goto exit;
    }

    /* Application specific custom endpoints will be assigned
     * incremental UUIDs starting after this value */
    prov_ctx->endpoint_uuid_used = 0xFF53;

    /* This delay is so that the client side app is notified first
     * and then the provisioning is stopped. Default is 1000ms. */
    prov_ctx->cleanup_delay = 1000;

exit:
    if (ret != ESP_OK) {
        if (prov_ctx->prov_scheme_config) {
            config.scheme.delete_config(prov_ctx->prov_scheme_config);
        }
        free(prov_ctx);
    } else {
        execute_event_cb(WIFI_PROV_INIT, NULL, 0);
    }
    RELEASE_LOCK(prov_ctx_lock);
    return ret;
}

void wifi_prov_mgr_wait(void)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return;
    }

    while (1) {
        ACQUIRE_LOCK(prov_ctx_lock);
        if (prov_ctx &&
            prov_ctx->prov_state != WIFI_PROV_STATE_IDLE) {
            RELEASE_LOCK(prov_ctx_lock);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        break;
    }
    RELEASE_LOCK(prov_ctx_lock);
}

void wifi_prov_mgr_deinit(void)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return;
    }

    ACQUIRE_LOCK(prov_ctx_lock);

    /* This will do one of these:
     * 1) if found running, stop the provisioning service (returns true)
     * 2) if service was already in the process of termination, this will
     *    wait till the service is stopped (returns false)
     * 3) if service was not running, this will return immediately (returns false)
     */
    bool service_was_running = wifi_prov_mgr_stop_service(1);

     /* If service was not running, its also possible that manager
      * was not even initialized */
    if (!service_was_running && !prov_ctx) {
        ESP_LOGD(TAG, "Manager already de-initialized");
        RELEASE_LOCK(prov_ctx_lock);
        vSemaphoreDelete(prov_ctx_lock);
        prov_ctx_lock = NULL;
        return;
    }

    if (prov_ctx->app_info_json) {
        cJSON_Delete(prov_ctx->app_info_json);
    }

    if (prov_ctx->prov_scheme_config) {
        prov_ctx->mgr_config.scheme.delete_config(prov_ctx->prov_scheme_config);
    }

    /* Extract the callbacks to be called post deinit */
    wifi_prov_cb_func_t app_cb = prov_ctx->mgr_config.app_event_handler.event_cb;
    void *app_data = prov_ctx->mgr_config.app_event_handler.user_data;

    wifi_prov_cb_func_t scheme_cb = prov_ctx->mgr_config.scheme_event_handler.event_cb;
    void *scheme_data = prov_ctx->mgr_config.scheme_event_handler.user_data;

    /* Free manager context */
    free(prov_ctx);
    prov_ctx = NULL;
    RELEASE_LOCK(prov_ctx_lock);

    /* If a running service was also stopped during de-initialization
     * then WIFI_PROV_END event also needs to be emitted before deinit */
    if (service_was_running) {
        ESP_LOGD(TAG, "execute_event_cb : %d", WIFI_PROV_END);
        if (scheme_cb) {
            scheme_cb(scheme_data, WIFI_PROV_END, NULL);
        }
        if (app_cb) {
            app_cb(app_data, WIFI_PROV_END, NULL);
        }
        if (esp_event_post(WIFI_PROV_EVENT, WIFI_PROV_END, NULL, 0, portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post event WIFI_PROV_END");
        }
    }

    ESP_LOGD(TAG, "execute_event_cb : %d", WIFI_PROV_DEINIT);

    /* Execute deinit event callbacks */
    if (scheme_cb) {
        scheme_cb(scheme_data, WIFI_PROV_DEINIT, NULL);
    }
    if (app_cb) {
        app_cb(app_data, WIFI_PROV_DEINIT, NULL);
    }
    if (esp_event_post(WIFI_PROV_EVENT, WIFI_PROV_DEINIT, NULL, 0, portMAX_DELAY) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to post event WIFI_PROV_DEINIT");
    }

    vSemaphoreDelete(prov_ctx_lock);
    prov_ctx_lock = NULL;
}

esp_err_t wifi_prov_mgr_start_provisioning(wifi_prov_security_t security, const void *wifi_prov_sec_params,
                                           const char *service_name, const char *service_key)
{
    uint8_t restore_wifi_flag = 0;

    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ACQUIRE_LOCK(prov_ctx_lock);
    if (!prov_ctx) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_ERR_INVALID_STATE;
    }

    if (prov_ctx->prov_state != WIFI_PROV_STATE_IDLE) {
        ESP_LOGE(TAG, "Provisioning service already started");
        RELEASE_LOCK(prov_ctx_lock);
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_OK;
    /* Update state so that parallel call to wifi_prov_mgr_start_provisioning()
     * or wifi_prov_mgr_stop_provisioning() or wifi_prov_mgr_deinit() from another
     * thread doesn't interfere with this process */
    prov_ctx->prov_state = WIFI_PROV_STATE_STARTING;

    /* Start Wi-Fi in Station Mode.
     * This is necessary for scanning to work */
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi mode to STA");
        goto err;
    }
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi");
        goto err;
    }

    /* Change Wi-Fi storage to RAM temporarily and erase any old
     * credentials in RAM(i.e. without erasing the copy on NVS). Also
     * call disconnect to make sure device doesn't remain connected
     * to the AP whose credentials were present earlier */
    wifi_config_t wifi_cfg_empty, wifi_cfg_old;
    memset(&wifi_cfg_empty, 0, sizeof(wifi_config_t));
    esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg_old);
    ret = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi storage to RAM");
        goto err;
    }

    /* WiFi storage needs to be restored before exiting this API */
    restore_wifi_flag |= WIFI_PROV_STORAGE_BIT;
    /* Erase Wi-Fi credentials in RAM, when call disconnect and user code
     * receive WIFI_EVENT_STA_DISCONNECTED and maybe call esp_wifi_connect, at
     * this time Wi-Fi will have no configuration to connect */
    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg_empty);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set empty Wi-Fi credentials");
        goto err;
    }
    /* WiFi settings needs to be restored if provisioning error before exiting this API */
    restore_wifi_flag |= WIFI_PROV_SETTING_BIT;

    ret = esp_wifi_disconnect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disconnect");
        goto err;
    }

#ifdef CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0
    /* Initialize app data */
    if (security == WIFI_PROV_SECURITY_0) {
        prov_ctx->mgr_info.capabilities.no_sec = true;
    }
#endif
#ifdef CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1
    if (security == WIFI_PROV_SECURITY_1) {
        if (wifi_prov_sec_params) {
            static protocomm_security1_params_t sec1_params;
            // Generate internal copy of "pop", that shall be freed at the end
            char *pop = strdup(wifi_prov_sec_params);
            if (pop == NULL) {
                ESP_LOGE(TAG, "Failed to allocate memory for pop");
                ret = ESP_ERR_NO_MEM;
                goto err;
            }
            sec1_params.data = (const uint8_t *)pop;
            sec1_params.len = strlen(pop);
            prov_ctx->protocomm_sec_params = (const void *) &sec1_params;
        } else {
            prov_ctx->mgr_info.capabilities.no_pop = true;
        }
    }
#endif
#ifdef CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2
    if (security == WIFI_PROV_SECURITY_2) {
        if (wifi_prov_sec_params) {
            prov_ctx->protocomm_sec_params = wifi_prov_sec_params;
        }
    }
#endif
    prov_ctx->security = security;


    esp_timer_create_args_t wifi_connect_timer_conf = {
        .callback = wifi_connect_timer_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "wifi_prov_connect_tm"
    };
    ret = esp_timer_create(&wifi_connect_timer_conf, &prov_ctx->wifi_connect_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create Wi-Fi connect timer");
        goto err;
    }

    /* If auto stop on completion is enabled (default) create the stopping timer */
    if (!prov_ctx->mgr_info.capabilities.no_auto_stop) {
        /* Create timer object as a member of app data */
        esp_timer_create_args_t autostop_timer_conf = {
            .callback = stop_prov_timer_cb,
            .arg = NULL,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "wifi_prov_autostop_tm"
        };
        ret = esp_timer_create(&autostop_timer_conf, &prov_ctx->autostop_timer);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create auto-stop timer");
            esp_timer_delete(prov_ctx->wifi_connect_timer);
            goto err;
        }
    }

    esp_timer_create_args_t cleanup_delay_timer = {
        .callback = cleanup_delay_timer_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "cleanup_delay_tm"
    };
    ret = esp_timer_create(&cleanup_delay_timer, &prov_ctx->cleanup_delay_timer);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to create cleanup delay timer");
      esp_timer_delete(prov_ctx->wifi_connect_timer);
      esp_timer_delete(prov_ctx->autostop_timer);
      goto err;
    }


    /* System APIs for BLE / Wi-Fi will be called inside wifi_prov_mgr_start_service(),
     * which may trigger system level events. Hence, releasing the context lock will
     * ensure that wifi_prov_mgr_event_handler() doesn't block the global event_loop
     * handler when system events need to be handled */
    RELEASE_LOCK(prov_ctx_lock);

    /* Start provisioning service */
    ret = wifi_prov_mgr_start_service(service_name, service_key);
    if (ret != ESP_OK) {
        esp_timer_delete(prov_ctx->autostop_timer);
        esp_timer_delete(prov_ctx->wifi_connect_timer);
        esp_timer_delete(prov_ctx->cleanup_delay_timer);
    }
    ACQUIRE_LOCK(prov_ctx_lock);
    if (ret == ESP_OK) {
        prov_ctx->prov_state = WIFI_PROV_STATE_STARTED;
        /* Execute user registered callback handler */
        execute_event_cb(WIFI_PROV_START, NULL, 0);
        goto exit;
    }

err:
    prov_ctx->prov_state = WIFI_PROV_STATE_IDLE;
    if (restore_wifi_flag & WIFI_PROV_SETTING_BIT) {
        /* Restore current WiFi settings, since provisioning start has failed */
        esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg_old);
    }
exit:
    if (restore_wifi_flag & WIFI_PROV_STORAGE_BIT) {
        /* Restore WiFi storage back to FLASH */
        esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    }
    RELEASE_LOCK(prov_ctx_lock);
    return ret;
}

void wifi_prov_mgr_stop_provisioning(void)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return;
    }

    ACQUIRE_LOCK(prov_ctx_lock);

    /* Launches task for stopping the provisioning service. This will do one of these:
     * 1) start a task for stopping the provisioning service (returns true)
     * 2) if service was already in the process of termination, this will
     *    wait till the service is stopped (returns false)
     * 3) if service was not running, this will return immediately (returns false)
     */
    wifi_prov_mgr_stop_service(0);

    RELEASE_LOCK(prov_ctx_lock);
}

esp_err_t wifi_prov_mgr_reset_provisioning(void)
{
    esp_err_t ret = esp_wifi_restore();

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_restore fail, ret is %d", ret);
        ret = ESP_FAIL;
    }

    return ret;
}

esp_err_t wifi_prov_mgr_reset_sm_state_on_failure(void)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ACQUIRE_LOCK(prov_ctx_lock);

    esp_err_t err = ESP_OK;
    if (prov_ctx->prov_state != WIFI_PROV_STATE_FAIL) {
        ESP_LOGE(TAG, "Trying reset when not in failure state. Current state: %d", prov_ctx->prov_state);
        err = ESP_ERR_INVALID_STATE;
        goto exit;
    }

    wifi_config_t wifi_cfg = {0};

    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set wifi config, 0x%x", err);
        goto exit;
    }

    prov_ctx->prov_state = WIFI_PROV_STATE_STARTED;

exit:
    RELEASE_LOCK(prov_ctx_lock);
    return err;
}

esp_err_t wifi_prov_mgr_reset_sm_state_for_reprovision(void)
{
    if (!prov_ctx_lock) {
        ESP_LOGE(TAG, "Provisioning manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    ACQUIRE_LOCK(prov_ctx_lock);

    esp_err_t ret = ESP_OK;
    wifi_config_t wifi_cfg_empty = {0};
    uint8_t restore_wifi_flag = 0;

    if (!prov_ctx->mgr_info.capabilities.no_auto_stop) {
        ESP_LOGE(TAG, "Execute wifi_prov_mgr_disable_auto_stop() before calling this API");
        ret = ESP_ERR_INVALID_STATE;
        goto exit;
    }

    ret = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi storage to RAM");
        goto exit;
    }
    restore_wifi_flag |= WIFI_PROV_STORAGE_BIT;

    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg_empty);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set empty Wi-Fi credentials, 0x%x", ret);
        goto exit;
    }

    ret = esp_wifi_disconnect();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disconnect wifi, 0x%x", ret);
        goto exit;
    }

    prov_ctx->prov_state = WIFI_PROV_STATE_STARTED;
    execute_event_cb(WIFI_PROV_START, NULL, 0);

exit:
    if (restore_wifi_flag & WIFI_PROV_STORAGE_BIT) {
        esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    }
    RELEASE_LOCK(prov_ctx_lock);
    return ret;
}
