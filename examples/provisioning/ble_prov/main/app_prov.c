/* BLE based Provisioning Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_bt.h>

#include <protocomm.h>
#include <protocomm_ble.h>
#include <protocomm_security0.h>
#include <protocomm_security1.h>
#include <wifi_provisioning/wifi_config.h>

#include "app_prov.h"

static const char *TAG = "app_prov";
static const char *ssid_prefix = "PROV_";

/* Handlers for wifi_config provisioning endpoint */
extern wifi_prov_config_handlers_t wifi_prov_handlers;

/**
 * @brief   Data relevant to provisioning application
 */
struct app_prov_data {
    protocomm_t *pc;                      /*!< Protocomm handler */
    int security;                         /*!< Type of security to use with protocomm */
    const protocomm_security_pop_t *pop;  /*!< Pointer to proof of possession */
    esp_timer_handle_t timer;             /*!< Handle to timer */

    /* State of WiFi Station */
    wifi_prov_sta_state_t wifi_state;

    /* Code for WiFi station disconnection (if disconnected) */
    wifi_prov_sta_fail_reason_t wifi_disconnect_reason;
};

/* Pointer to provisioning application data */
static struct app_prov_data *g_prov;

static esp_err_t app_prov_start_service(void)
{
    /* Create new protocomm instance */
    g_prov->pc = protocomm_new();
    if (g_prov->pc == NULL) {
        ESP_LOGE(TAG, "Failed to create new protocomm instance");
        return ESP_FAIL;
    }

    /* Endpoint UUIDs */
    protocomm_ble_name_uuid_t nu_lookup_table[] = {
        {"prov-session", 0xFF51},
        {"prov-config",  0xFF52},
        {"proto-ver",    0xFF53},
    };

    /* Config for protocomm_ble_start() */
    protocomm_ble_config_t config = {
        .service_uuid = {
            /* LSB <---------------------------------------
             * ---------------------------------------> MSB */
            0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
            0x00, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
        },
        .nu_lookup_count = sizeof(nu_lookup_table)/sizeof(nu_lookup_table[0]),
        .nu_lookup = nu_lookup_table
    };
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(config.device_name, sizeof(config.device_name), "%s%02X%02X%02X",
             ssid_prefix, eth_mac[3], eth_mac[4], eth_mac[5]);

    /* Release BT memory, as we need only BLE */
    esp_err_t err = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (err) {
        ESP_LOGE(TAG, "bt_controller_mem_release failed %d", err);
        if (err != ESP_ERR_INVALID_STATE) {
            return err;
        }
    }

    /* Start protocomm layer on top of BLE */
    if (protocomm_ble_start(g_prov->pc, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start BLE provisioning");
        return ESP_FAIL;
    }

    /* Set protocomm version verification endpoint for protocol */
    protocomm_set_version(g_prov->pc, "proto-ver", "V0.1");

    /* Set protocomm security type for endpoint */
    if (g_prov->security == 0) {
        protocomm_set_security(g_prov->pc, "prov-session", &protocomm_security0, NULL);
    } else if (g_prov->security == 1) {
        protocomm_set_security(g_prov->pc, "prov-session", &protocomm_security1, g_prov->pop);
    }

    /* Add endpoint for provisioning to set wifi station config */
    if (protocomm_add_endpoint(g_prov->pc, "prov-config",
                               wifi_prov_config_data_handler,
                               (void *) &wifi_prov_handlers) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set provisioning endpoint");
        protocomm_ble_stop(g_prov->pc);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Provisioning started with BLE devname : %s", config.device_name);
    return ESP_OK;
}

static void app_prov_stop_service(void)
{
    /* Remove provisioning endpoint */
    protocomm_remove_endpoint(g_prov->pc, "prov-config");
    /* Unset provisioning security */
    protocomm_unset_security(g_prov->pc, "prov-session");
    /* Unset provisioning version endpoint */
    protocomm_unset_version(g_prov->pc, "proto-ver");
    /* Stop protocomm ble service */
    protocomm_ble_stop(g_prov->pc);
    /* Delete protocomm instance */
    protocomm_delete(g_prov->pc);

    /* Release memory used by BT stack */
    esp_bt_mem_release(ESP_BT_MODE_BTDM);
}

/* Task spawned by timer callback */
static void stop_prov_task(void * arg)
{
    ESP_LOGI(TAG, "Stopping provisioning");
    app_prov_stop_service();

    /* Timer not needed anymore */
    esp_timer_handle_t timer = g_prov->timer;
    esp_timer_delete(timer);
    g_prov->timer = NULL;

    /* Free provisioning process data */
    free(g_prov);
    g_prov = NULL;
    ESP_LOGI(TAG, "Provisioning stopped");

    vTaskDelete(NULL);
}

/* Callback to be invoked by timer */
static void _stop_prov_cb(void * arg)
{
    xTaskCreate(&stop_prov_task, "stop_prov", 2048, NULL, tskIDLE_PRIORITY, NULL);
}

/* Event handler for starting/stopping provisioning.
 * To be called from within the context of the main
 * event handler.
 */
esp_err_t app_prov_event_handler(void *ctx, system_event_t *event)
{
    /* For accessing reason codes in case of disconnection */
    system_event_info_t *info = &event->event_info;

    /* If pointer to provisioning application data is NULL
     * then provisioning is not running, therefore return without
     * error */
    if (!g_prov) {
        return ESP_OK;
    }

    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "STA Start");
        /* Once configuration is received through protocomm,
         * device is started as station. Once station starts,
         * wait for connection to establish with configured
         * host SSID and password */
        g_prov->wifi_state = WIFI_PROV_STA_CONNECTING;
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "STA Got IP");
        /* Station got IP. That means configuraion is successful.
         * Schedule timer to stop provisioning app after 30 seconds. */
        g_prov->wifi_state = WIFI_PROV_STA_CONNECTED;
        if (g_prov && g_prov->timer) {
            esp_timer_start_once(g_prov->timer, 30000*1000U);
        }
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGE(TAG, "STA Disconnected");
        /* Station couldn't connect to configured host SSID */
        g_prov->wifi_state = WIFI_PROV_STA_DISCONNECTED;
        ESP_LOGE(TAG, "Disconnect reason : %d", info->disconnected.reason);

        /* Set code corresponding to the reason for disconnection */
        switch (info->disconnected.reason) {
        case WIFI_REASON_AUTH_EXPIRE:
        case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
        case WIFI_REASON_BEACON_TIMEOUT:
        case WIFI_REASON_AUTH_FAIL:
        case WIFI_REASON_ASSOC_FAIL:
        case WIFI_REASON_HANDSHAKE_TIMEOUT:
            ESP_LOGI(TAG, "STA Auth Error");
            g_prov->wifi_disconnect_reason = WIFI_PROV_STA_AUTH_ERROR;
            break;
        case WIFI_REASON_NO_AP_FOUND:
            ESP_LOGI(TAG, "STA AP Not found");
            g_prov->wifi_disconnect_reason = WIFI_PROV_STA_AP_NOT_FOUND;
            break;
        default:
            /* If none of the expected reasons,
             * retry connecting to host SSID */
            g_prov->wifi_state = WIFI_PROV_STA_CONNECTING;
            esp_wifi_connect();
        }
        break;

    default:
        break;
    }
    return ESP_OK;
}

esp_err_t app_prov_get_wifi_state(wifi_prov_sta_state_t* state)
{
    if (g_prov == NULL || state == NULL) {
        return ESP_FAIL;
    }

    *state = g_prov->wifi_state;
    return ESP_OK;
}

esp_err_t app_prov_get_wifi_disconnect_reason(wifi_prov_sta_fail_reason_t* reason)
{
    if (g_prov == NULL || reason == NULL) {
        return ESP_FAIL;
    }

    if (g_prov->wifi_state != WIFI_PROV_STA_DISCONNECTED) {
        return ESP_FAIL;
    }

    *reason = g_prov->wifi_disconnect_reason;
    return ESP_OK;
}

esp_err_t app_prov_is_provisioned(bool *provisioned)
{
    *provisioned = false;

#ifdef CONFIG_RESET_PROVISIONED
    nvs_flash_erase();
#endif

    if (nvs_flash_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NVS");
        return ESP_FAIL;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&cfg) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init wifi");
        return ESP_FAIL;
    }

    /* Get WiFi Station configuration */
    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }

    if (strlen((const char*) wifi_cfg.sta.ssid)) {
        *provisioned = true;
        ESP_LOGI(TAG, "Found ssid %s",     (const char*) wifi_cfg.sta.ssid);
        ESP_LOGI(TAG, "Found password %s", (const char*) wifi_cfg.sta.password);
    }
    return ESP_OK;
}

esp_err_t app_prov_configure_sta(wifi_config_t *wifi_cfg)
{
    /* Initialize WiFi with default config */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&cfg) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init WiFi");
        return ESP_FAIL;
    }
    /* Configure WiFi as station */
    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi mode");
        return ESP_FAIL;
    }
    /* Configure WiFi station with host credentials
     * provided during provisioning */
    if (esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_cfg) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi configuration");
        return ESP_FAIL;
    }
    /* Start WiFi */
    if (esp_wifi_start() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi configuration");
        return ESP_FAIL;
    }
    /* Connect to AP */
    if (esp_wifi_connect() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect WiFi");
        return ESP_FAIL;
    }

    if (g_prov) {
        /* Reset wifi station state for provisioning app */
        g_prov->wifi_state = WIFI_PROV_STA_CONNECTING;
    }
    return ESP_OK;
}

esp_err_t app_prov_start_ble_provisioning(int security, const protocomm_security_pop_t *pop)
{
    /* If provisioning app data present,
     * means provisioning app is already running */
    if (g_prov) {
        ESP_LOGI(TAG, "Invalid provisioning state");
        return ESP_FAIL;
    }

    /* Allocate memory for provisioning app data */
    g_prov = (struct app_prov_data *) calloc(1, sizeof(struct app_prov_data));
    if (!g_prov) {
        ESP_LOGI(TAG, "Unable to allocate prov data");
        return ESP_ERR_NO_MEM;
    }

    /* Initialise app data */
    g_prov->pop = pop;
    g_prov->security = security;

    /* Create timer object as a member of app data */
    esp_timer_create_args_t timer_conf = {
        .callback = _stop_prov_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "stop_ble_tm"
    };
    esp_err_t err = esp_timer_create(&timer_conf, &g_prov->timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create timer");
        return err;
    }

    /* Start provisioning service through BLE */
    err = app_prov_start_service();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Provisioning failed to start");
        return err;
    }

    ESP_LOGI(TAG, "BLE Provisioning started");
    return ESP_OK;
}
