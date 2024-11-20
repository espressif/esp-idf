/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_event.h>
#include <esp_wifi.h>
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "esp_private/sleep_modem.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_check.h"
#include "esp_private/pm_impl.h"
#include "esp_private/esp_clk.h"
#include "esp_wpa.h"
#include "esp_netif.h"
#ifdef CONFIG_ESP_COEX_ENABLED
#include "private/esp_coexist_internal.h"
#endif
#ifdef CONFIG_ESP_PHY_ENABLED
#include "esp_phy_init.h"
#include "esp_private/phy.h"
#endif
#if __has_include("esp_psram.h")
#include "esp_psram.h"
#endif
#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
#include "apps_private/wifi_apps_private.h"
#endif
#ifdef CONFIG_ESP_WIFI_FTM_ENABLE
#include "esp_chip_info.h"
#endif

#if SOC_PM_MODEM_RETENTION_BY_REGDMA
#include "esp_private/sleep_retention.h"
#endif

#if CONFIG_ESP_WIFI_ENABLE_ROAMING_APP
#include "esp_roaming.h"
#endif

static bool s_wifi_inited = false;

#if (CONFIG_ESP_WIFI_RX_BA_WIN > CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM)
#error "WiFi configuration check: WARNING, WIFI_RX_BA_WIN should not be larger than WIFI_DYNAMIC_RX_BUFFER_NUM!"
#endif

#if (CONFIG_ESP_WIFI_RX_BA_WIN > (CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM << 1))
#error "WiFi configuration check: WARNING, WIFI_RX_BA_WIN should not be larger than double of the WIFI_STATIC_RX_BUFFER_NUM!"
#endif

ESP_EVENT_DEFINE_BASE(WIFI_EVENT);

extern uint8_t esp_wifi_get_user_init_flag_internal(void);
#ifdef CONFIG_PM_ENABLE
static esp_pm_lock_handle_t s_wifi_modem_sleep_lock;
#endif

#if CONFIG_IDF_TARGET_ESP32
/* Callback function to update WiFi MAC time */
wifi_mac_time_update_cb_t s_wifi_mac_time_update_cb = NULL;
#endif

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
# define WIFI_BEACON_MONITOR_CONFIG_DEFAULT(ena)   { \
    .enable = (ena), \
    .loss_timeout = CONFIG_ESP_WIFI_SLP_BEACON_LOST_TIMEOUT, \
    .loss_threshold = CONFIG_ESP_WIFI_SLP_BEACON_LOST_THRESHOLD, \
    .delta_intr_early = 0, \
    .delta_loss_timeout = 0, \
    .beacon_abort = 1, \
    .broadcast_wakeup = 1, \
    .tsf_time_sync_deviation = 5, \
    .modem_state_consecutive = 10, \
    .rf_ctrl_wait_cycle = 20 \
}
#else
# define WIFI_BEACON_MONITOR_CONFIG_DEFAULT(ena)   { \
    .enable = (ena), \
    .loss_timeout = CONFIG_ESP_WIFI_SLP_BEACON_LOST_TIMEOUT, \
    .loss_threshold = CONFIG_ESP_WIFI_SLP_BEACON_LOST_THRESHOLD, \
    .delta_intr_early = CONFIG_ESP_WIFI_SLP_PHY_ON_DELTA_EARLY_TIME, \
    .delta_loss_timeout = CONFIG_ESP_WIFI_SLP_PHY_OFF_DELTA_TIMEOUT_TIME \
}
#endif

static const char* TAG = "wifi_init";

static void __attribute__((constructor)) s_set_default_wifi_log_level(void)
{
    /* WiFi libraries aren't compiled to know CONFIG_LOG_DEFAULT_LEVEL,
       so set it at runtime startup. Done here not in esp_wifi_init() to allow
       the user to set the level again before esp_wifi_init() is called.
    */
    esp_log_level_set("wifi", CONFIG_LOG_DEFAULT_LEVEL);
    esp_log_level_set("mesh", CONFIG_LOG_DEFAULT_LEVEL);
    esp_log_level_set("smartconfig", CONFIG_LOG_DEFAULT_LEVEL);
    esp_log_level_set("ESPNOW", CONFIG_LOG_DEFAULT_LEVEL);
}

static void esp_wifi_set_log_level(void)
{
    wifi_log_level_t wifi_log_level = WIFI_LOG_INFO;
    /* set WiFi log level */
#if CONFIG_LOG_MAXIMUM_LEVEL == 0
    wifi_log_level = WIFI_LOG_NONE;
#elif CONFIG_LOG_MAXIMUM_LEVEL == 1
    wifi_log_level = WIFI_LOG_ERROR;
#elif CONFIG_LOG_MAXIMUM_LEVEL == 2
    wifi_log_level = WIFI_LOG_WARNING;
#elif CONFIG_LOG_MAXIMUM_LEVEL == 3
    wifi_log_level = WIFI_LOG_INFO;
#elif CONFIG_LOG_MAXIMUM_LEVEL == 4
    wifi_log_level = WIFI_LOG_DEBUG;
#elif CONFIG_LOG_MAXIMUM_LEVEL == 5
    wifi_log_level = WIFI_LOG_VERBOSE;
#endif
    esp_wifi_internal_set_log_level(wifi_log_level);
}

#if (CONFIG_FREERTOS_USE_TICKLESS_IDLE && SOC_PM_MODEM_RETENTION_BY_REGDMA)
static esp_err_t init_wifi_mac_sleep_retention(void *arg)
{
    int config_size;
    sleep_retention_entries_config_t *config = esp_wifi_internal_mac_retention_context_get(&config_size);
    esp_err_t err = sleep_retention_entries_create(config, config_size, 3, SLEEP_RETENTION_MODULE_WIFI_MAC);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for modem (%s) retention", "WiFi MAC");
    ESP_LOGD(TAG, "WiFi MAC sleep retention initialization");
    return ESP_OK;
}
#endif

#if CONFIG_MAC_BB_PD
static void esp_wifi_mac_pd_mem_init(void)
{
#if SOC_PM_MODEM_RETENTION_BY_REGDMA
    esp_err_t err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_WIFI_MAC);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to allocate sleep retention linked list for wifi mac retention");
    }
#endif
    esp_wifi_internal_set_mac_sleep(true);
}
static void esp_wifi_mac_pd_mem_deinit(void)
{
    esp_wifi_internal_set_mac_sleep(false);
#if SOC_PM_MODEM_RETENTION_BY_REGDMA
    esp_err_t err = sleep_retention_module_free(SLEEP_RETENTION_MODULE_WIFI_MAC);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to free sleep retention linked list for wifi mac retention");
    }
#endif
}
#endif

static esp_err_t wifi_deinit_internal(void)
{
    esp_err_t err = ESP_OK;

    if (esp_wifi_get_user_init_flag_internal()) {
        ESP_LOGE(TAG, "Wi-Fi not stop");
        return ESP_ERR_WIFI_NOT_STOPPED;
    }

    if (esp_wifi_internal_reg_rxcb(WIFI_IF_STA,  NULL) != ESP_OK ||
            esp_wifi_internal_reg_rxcb(WIFI_IF_AP,  NULL) != ESP_OK) {
        ESP_LOGW(TAG, "Failed to unregister Rx callbacks");
    }

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
    esp_nan_app_deinit();
#endif

#if CONFIG_MAC_BB_PD
    esp_wifi_mac_pd_mem_deinit();
    esp_mac_bb_pd_mem_deinit();
#endif

    esp_supplicant_deinit();

#if CONFIG_ESP_WIFI_ENABLE_ROAMING_APP
    deinit_roaming_app();
#endif

    err = esp_wifi_deinit_internal();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deinit Wi-Fi driver (0x%x)", err);
        return err;
    }
#ifdef CONFIG_PM_ENABLE
    if (s_wifi_modem_sleep_lock) {
        esp_pm_lock_delete(s_wifi_modem_sleep_lock);
        s_wifi_modem_sleep_lock = NULL;
    }
#endif
#ifdef CONFIG_ESP_PHY_ENABLED
    esp_wifi_power_domain_off();
#endif

#if CONFIG_ESP_WIFI_SLP_BEACON_LOST_OPT
    wifi_beacon_monitor_config_t monitor_config = WIFI_BEACON_MONITOR_CONFIG_DEFAULT(false);
    esp_wifi_beacon_monitor_configure(&monitor_config);
#endif

#if CONFIG_PM_ENABLE && CONFIG_ESP_WIFI_SLP_IRAM_OPT
    esp_pm_unregister_light_sleep_default_params_config_callback();
#endif
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#if SOC_WIFI_HW_TSF
    esp_pm_unregister_skip_light_sleep_callback(esp_wifi_internal_is_tsf_active);
    esp_pm_unregister_inform_out_light_sleep_overhead_callback(esp_wifi_internal_update_light_sleep_wake_ahead_time);
    esp_sleep_disable_wifi_wakeup();
# if CONFIG_ESP_WIFI_ENHANCED_LIGHT_SLEEP
    esp_sleep_disable_wifi_beacon_wakeup();
# endif
#endif /* SOC_WIFI_HW_TSF */
#if SOC_PM_MODEM_RETENTION_BY_REGDMA
    err = sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_WIFI_MAC);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "WiFi MAC sleep retention deinit failed");
    }
#endif /* SOC_PM_MODEM_RETENTION_BY_REGDMA */
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */
#if CONFIG_MAC_BB_PD
    esp_unregister_mac_bb_pd_callback(pm_mac_sleep);
    esp_unregister_mac_bb_pu_callback(pm_mac_wakeup);
#endif
#if CONFIG_ESP_WIFI_ENHANCED_LIGHT_SLEEP
    esp_wifi_internal_modem_state_configure(false);
    esp_pm_unregister_skip_light_sleep_callback(sleep_modem_wifi_modem_state_skip_light_sleep);
#endif
#ifdef CONFIG_ESP_PHY_ENABLED
    esp_phy_modem_deinit();
#endif
    s_wifi_inited = false;

    return err;
}

esp_err_t esp_wifi_deinit(void)
{
    if (s_wifi_inited == false) {
        return ESP_ERR_WIFI_NOT_INIT;
    }

    return wifi_deinit_internal();
}

static void esp_wifi_config_info(void)
{
#ifdef CONFIG_ESP_WIFI_RX_BA_WIN
    ESP_LOGI(TAG, "rx ba win: %d", CONFIG_ESP_WIFI_RX_BA_WIN);
#endif

#ifdef CONFIG_ESP_NETIF_TCPIP_LWIP
    ESP_LOGI(TAG, "accept mbox: %d", CONFIG_LWIP_TCP_ACCEPTMBOX_SIZE);
    ESP_LOGI(TAG, "tcpip mbox: %d", CONFIG_LWIP_TCPIP_RECVMBOX_SIZE);
    ESP_LOGI(TAG, "udp mbox: %d", CONFIG_LWIP_UDP_RECVMBOX_SIZE);
    ESP_LOGI(TAG, "tcp mbox: %d", CONFIG_LWIP_TCP_RECVMBOX_SIZE);
    ESP_LOGI(TAG, "tcp tx win: %d", CONFIG_LWIP_TCP_SND_BUF_DEFAULT);
    ESP_LOGI(TAG, "tcp rx win: %d", CONFIG_LWIP_TCP_WND_DEFAULT);
    ESP_LOGI(TAG, "tcp mss: %d", CONFIG_LWIP_TCP_MSS);

#ifdef CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP
    ESP_LOGI(TAG, "WiFi/LWIP prefer SPIRAM");
#endif

#ifdef CONFIG_ESP_WIFI_IRAM_OPT
    ESP_LOGI(TAG, "WiFi IRAM OP enabled");
#endif

#ifdef CONFIG_ESP_WIFI_RX_IRAM_OPT
    ESP_LOGI(TAG, "WiFi RX IRAM OP enabled");
#endif

#ifdef CONFIG_ESP_WIFI_SLP_IRAM_OPT
    ESP_LOGI(TAG, "WiFi SLP IRAM OP enabled");
#endif

#ifdef CONFIG_LWIP_IRAM_OPTIMIZATION
    ESP_LOGI(TAG, "LWIP IRAM OP enabled");
#endif

#else
    ESP_LOGI(TAG, "LWIP disabled");
#endif
}

#if CONFIG_SPIRAM
static esp_err_t esp_wifi_psram_check(const wifi_init_config_t *config)
{
#if CONFIG_SPIRAM_IGNORE_NOTFOUND
    if (!esp_psram_is_initialized()) {
        if (config->feature_caps & CONFIG_FEATURE_CACHE_TX_BUF_BIT) {
            ESP_LOGW(TAG, "WiFi cache TX buffers should be disabled when initialize SPIRAM failed");
        }
        if (config->tx_buf_type == 0) {
            ESP_LOGW(TAG, "TX buffers type should be changed from static to dynamic when initialize SPIRAM failed");
        }
#ifdef CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP
        ESP_LOGW(TAG, "WiFi/LWIP prefer SPIRAM should be disabled when initialize SPIRAM failed");
#endif
        if (config->amsdu_tx_enable) {
            ESP_LOGW(TAG, "WiFi AMSDU TX should be disabled when initialize SPIRAM failed");
        }
    }
#endif
    if ((config->feature_caps & CONFIG_FEATURE_CACHE_TX_BUF_BIT) && (WIFI_CACHE_TX_BUFFER_NUM == 0)) {
        ESP_LOGE(TAG, "Number of WiFi cache TX buffers should not equal 0 when enable SPIRAM");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}
#endif

esp_err_t esp_wifi_init(const wifi_init_config_t *config)
{
    if (s_wifi_inited) {
        return ESP_OK;
    }

    esp_err_t result = ESP_OK;
#ifdef CONFIG_SPIRAM
    result = esp_wifi_psram_check(config);
    if (result != ESP_OK) {
        return result;
    }
#endif

#if CONFIG_PM_ENABLE && CONFIG_ESP_WIFI_SLP_IRAM_OPT
    int min_freq_mhz = esp_pm_impl_get_cpu_freq(PM_MODE_LIGHT_SLEEP);
    int max_freq_mhz = esp_pm_impl_get_cpu_freq(PM_MODE_CPU_MAX);
    esp_wifi_internal_update_light_sleep_default_params(min_freq_mhz, max_freq_mhz);

    esp_pm_register_light_sleep_default_params_config_callback(esp_wifi_internal_update_light_sleep_default_params);

#endif

    uint32_t min_active_time_us = CONFIG_ESP_WIFI_SLP_DEFAULT_MIN_ACTIVE_TIME * 1000;
    esp_wifi_set_sleep_min_active_time(min_active_time_us);

    uint32_t keep_alive_time_us = CONFIG_ESP_WIFI_SLP_DEFAULT_MAX_ACTIVE_TIME * 1000 * 1000;
    esp_wifi_set_keep_alive_time(keep_alive_time_us);

    uint32_t wait_broadcast_data_time_us = CONFIG_ESP_WIFI_SLP_DEFAULT_WAIT_BROADCAST_DATA_TIME * 1000;
    esp_wifi_set_sleep_wait_broadcast_data_time(wait_broadcast_data_time_us);

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#if SOC_PM_MODEM_RETENTION_BY_REGDMA
    sleep_retention_module_init_param_t init_param = {
        .cbs     = { .create = { .handle = init_wifi_mac_sleep_retention, .arg = NULL } },
    };
    init_param.depends.bitmap[SLEEP_RETENTION_MODULE_WIFI_BB >> 5] |= BIT(SLEEP_RETENTION_MODULE_WIFI_BB % 32);
    init_param.depends.bitmap[SLEEP_RETENTION_MODULE_CLOCK_MODEM >> 5] |= BIT(SLEEP_RETENTION_MODULE_CLOCK_MODEM % 32);
    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_WIFI_MAC, &init_param);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "WiFi MAC sleep retention init failed");
    }
#endif

#if CONFIG_MAC_BB_PD
    if (esp_register_mac_bb_pd_callback(pm_mac_sleep) != ESP_OK
            || esp_register_mac_bb_pu_callback(pm_mac_wakeup) != ESP_OK) {

        esp_unregister_mac_bb_pd_callback(pm_mac_sleep);
        esp_unregister_mac_bb_pu_callback(pm_mac_wakeup);
        return ESP_ERR_INVALID_ARG;
    }
#endif

#if SOC_WIFI_HW_TSF
    esp_err_t ret = esp_pm_register_skip_light_sleep_callback(esp_wifi_internal_is_tsf_active);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register skip light sleep callback (0x%x)", ret);
#if CONFIG_MAC_BB_PD
        esp_unregister_mac_bb_pd_callback(pm_mac_sleep);
        esp_unregister_mac_bb_pu_callback(pm_mac_wakeup);
#endif
        return ret;
    }
    ret = esp_pm_register_inform_out_light_sleep_overhead_callback(esp_wifi_internal_update_light_sleep_wake_ahead_time);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register inform light sleep overhead callback (0x%x)", ret);
        return ret;
    }
    esp_sleep_enable_wifi_wakeup();
# if CONFIG_ESP_WIFI_ENHANCED_LIGHT_SLEEP
    esp_sleep_enable_wifi_beacon_wakeup();
# endif
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    coex_wifi_register_update_lpclk_callback(esp_wifi_update_tsf_tick_interval);
#endif
#endif /* SOC_WIFI_HW_TSF */
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */

#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    coex_init();
#endif
    esp_wifi_set_log_level();
#ifdef CONFIG_ESP_PHY_ENABLED
    esp_wifi_power_domain_on();
#endif
#ifdef CONFIG_ESP_WIFI_FTM_ENABLE
    esp_chip_info_t info = {0};
    esp_chip_info(&info);
    if (info.model == CHIP_ESP32C6 && info.revision <= 1) {
        ((wifi_init_config_t *)config)->feature_caps &= ~(CONFIG_FEATURE_FTM_INITIATOR_BIT);
    }
#endif
    result = esp_wifi_init_internal(config);
    if (result == ESP_OK) {
#if CONFIG_MAC_BB_PD
        esp_mac_bb_pd_mem_init();
        esp_wifi_mac_pd_mem_init();
#endif
#ifdef CONFIG_ESP_PHY_ENABLED
        esp_phy_modem_init();
#endif
#if CONFIG_ESP_WIFI_ENHANCED_LIGHT_SLEEP
        if (sleep_modem_wifi_modem_state_enabled()) {
            esp_pm_register_skip_light_sleep_callback(sleep_modem_wifi_modem_state_skip_light_sleep);
            esp_wifi_internal_modem_state_configure(true); /* require WiFi to enable automatically receives the beacon */
        }
#endif
#if CONFIG_IDF_TARGET_ESP32
        s_wifi_mac_time_update_cb = esp_wifi_internal_update_mac_time;
#endif

#ifdef CONFIG_PM_ENABLE
        if (s_wifi_modem_sleep_lock == NULL) {
            result = esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "wifi",
                                        &s_wifi_modem_sleep_lock);
            if (result != ESP_OK) {
                ESP_LOGE(TAG, "Failed to create pm lock (0x%x)", result);
                goto _deinit;
            }
        }
#endif

        result = esp_supplicant_init();
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "Failed to init supplicant (0x%x)", result);
            goto _deinit;
        }

#if CONFIG_ESP_WIFI_ENABLE_ROAMING_APP
        init_roaming_app();
#endif

    } else {
        goto _deinit;
    }
#if CONFIG_ESP_WIFI_SLP_BEACON_LOST_OPT
    wifi_beacon_monitor_config_t monitor_config = WIFI_BEACON_MONITOR_CONFIG_DEFAULT(true);
    esp_wifi_beacon_monitor_configure(&monitor_config);
#endif
    adc2_cal_include(); //This enables the ADC2 calibration constructor at start up.

    esp_wifi_config_info();

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
    esp_nan_app_init();
#endif

    s_wifi_inited = true;

    return result;

_deinit:
    ;
    esp_err_t deinit_ret = wifi_deinit_internal();
    if (deinit_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deinit Wi-Fi (0x%x)", deinit_ret);
    }

    return result;
}

esp_err_t esp_wifi_connect(void)
{
    esp_err_t ret = ESP_OK;
    ret = esp_wifi_connect_internal();

#if CONFIG_ESP_WIFI_ENABLE_ROAMING_APP
    roaming_app_enable_reconnect();
#endif
    return ret;
}

esp_err_t esp_wifi_disconnect(void)
{
    esp_err_t ret = ESP_OK;
    ret = esp_wifi_disconnect_internal();

#if CONFIG_ESP_WIFI_ENABLE_ROAMING_APP
    roaming_app_disable_reconnect();
#endif
    return ret;
}

#ifdef CONFIG_PM_ENABLE
void wifi_apb80m_request(void)
{
    assert(s_wifi_modem_sleep_lock);
    esp_pm_lock_acquire(s_wifi_modem_sleep_lock);
}

void wifi_apb80m_release(void)
{
    assert(s_wifi_modem_sleep_lock);
    esp_pm_lock_release(s_wifi_modem_sleep_lock);
}
#endif //CONFIG_PM_ENABLE

#ifndef CONFIG_ESP_WIFI_FTM_ENABLE
esp_err_t ieee80211_ftm_attach(void)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return ESP_OK;
}
#endif

#ifndef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
void net80211_softap_funcs_init(void)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
}

bool ieee80211_ap_try_sa_query(void *p)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return false;
}

bool ieee80211_ap_sa_query_timeout(void *p)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return false;
}

int add_mic_ie_bip(void *p)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return 0;
}

void ieee80211_free_beacon_eb(void)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
}

int ieee80211_pwrsave(void *p1, void *p2)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return 0;
}

void cnx_node_remove(void *p)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
}

int ieee80211_set_tim(void *p, int arg)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return 0;
}

bool ieee80211_is_bufferable_mmpdu(void *p)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return false;
}

void cnx_node_leave(void *p, uint8_t arg)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
}

void ieee80211_beacon_construct(void *p1, void *p2, void *p3, void *p4)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
}

void * ieee80211_assoc_resp_construct(void *p, int arg)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return NULL;
}

void * ieee80211_alloc_proberesp(void *p, int arg)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return NULL;
}

#endif

#ifndef CONFIG_ESP_WIFI_NAN_ENABLE

esp_err_t nan_start(void)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return ESP_OK;
}

esp_err_t nan_stop(void)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return ESP_OK;
}

int nan_input(void *p1, int p2, int p3)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
    return 0;
}

void nan_sm_handle_event(void *p1, int p2)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
}

#endif

#if CONFIG_IDF_TARGET_ESP32C2
#if CONFIG_ESP32C2_REV_MIN_FULL < 200
void esp32c2_eco4_rom_ptr_init(void)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
}
#endif
#endif

#ifndef CONFIG_ESP_WIFI_SLP_SAMPLE_BEACON_FEATURE
void pm_beacon_offset_funcs_init(void)
{
    /* Do not remove, stub to overwrite weak link in Wi-Fi Lib */
}
#endif
