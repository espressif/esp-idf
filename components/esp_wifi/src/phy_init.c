// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/lock.h>

#include "soc/rtc.h"
#include "soc/dport_reg.h"
#include "esp_err.h"
#include "esp_phy_init.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "phy.h"
#include "phy_init_data.h"
#include "esp_coexist_internal.h"
#include "driver/periph_ctrl.h"
#include "esp_private/wifi.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#include "esp32/rom/rtc.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/ets_sys.h"
#include "esp32s2beta/rom/rtc.h"
#endif

#if CONFIG_IDF_TARGET_ESP32
extern wifi_mac_time_update_cb_t s_wifi_mac_time_update_cb;
#endif

static const char* TAG = "phy_init";

static _lock_t s_phy_rf_init_lock;

/* Bit mask of modules needing to call phy_rf_init */
static uint32_t s_module_phy_rf_init = 0;

/* Whether modem sleep is turned on */
static volatile bool s_is_phy_rf_en = false;

/* Bit mask of modules needing to enter modem sleep mode */
static uint32_t s_modem_sleep_module_enter = 0;

/* Bit mask of modules which might use RF, system can enter modem
 * sleep mode only when all modules registered require to enter
 * modem sleep*/
static uint32_t s_modem_sleep_module_register = 0;

/* Whether modern sleep is turned on */
static volatile bool s_is_modem_sleep_en = false;

static _lock_t s_modem_sleep_lock;

#if CONFIG_IDF_TARGET_ESP32
/* time stamp updated when the PHY/RF is turned on */
static int64_t s_phy_rf_en_ts = 0;
#endif

uint32_t IRAM_ATTR phy_enter_critical(void)
{
    return portENTER_CRITICAL_NESTED();
}

void IRAM_ATTR phy_exit_critical(uint32_t level)
{
    portEXIT_CRITICAL_NESTED(level);
}

#if CONFIG_IDF_TARGET_ESP32
int64_t esp_phy_rf_get_on_ts(void)
{
    return s_phy_rf_en_ts;
}

static inline void phy_update_wifi_mac_time(bool en_clock_stopped, int64_t now)
{
    static uint32_t s_common_clock_disable_time = 0;

    if (en_clock_stopped) {
        s_common_clock_disable_time = (uint32_t)now;
    } else {
        if (s_common_clock_disable_time) {
            uint32_t diff = (uint64_t)now - s_common_clock_disable_time;

            if (s_wifi_mac_time_update_cb) {
                s_wifi_mac_time_update_cb(diff);
            }
            s_common_clock_disable_time = 0;
        }
    }
}
#endif

esp_err_t esp_phy_rf_init(const esp_phy_init_data_t* init_data, esp_phy_calibration_mode_t mode,
                          esp_phy_calibration_data_t* calibration_data, phy_rf_module_t module)
{
    /* 3 modules may call phy_init: Wi-Fi, BT, Modem Sleep */
    if (module >= PHY_MODULE_COUNT){
        ESP_LOGE(TAG, "%s, invalid module parameter(%d), should be smaller than \
                 module count(%d)", __func__, module, PHY_MODULE_COUNT);
        return ESP_ERR_INVALID_ARG;
    }

    _lock_acquire(&s_phy_rf_init_lock);
    uint32_t s_module_phy_rf_init_old = s_module_phy_rf_init;
    bool is_wifi_or_bt_enabled = !!(s_module_phy_rf_init_old & (BIT(PHY_BT_MODULE) | BIT(PHY_WIFI_MODULE)));
    esp_err_t status = ESP_OK;
    s_module_phy_rf_init |= BIT(module);

    if ((is_wifi_or_bt_enabled == false) && (module == PHY_MODEM_MODULE)){
        status = ESP_FAIL;
    }
    else if (s_is_phy_rf_en == true) {
    }
    else {
        /* If Wi-Fi, BT all disabled, modem sleep should not take effect;
         * If either Wi-Fi or BT is enabled, should allow modem sleep requires
         * to enter sleep;
         * If Wi-Fi, BT co-exist, it is disallowed that only one module
         * support modem sleep, E,g. BT support modem sleep but Wi-Fi not
         * support modem sleep;
         */
        if (is_wifi_or_bt_enabled == false){
            if ((module == PHY_BT_MODULE) || (module == PHY_WIFI_MODULE)){
                s_is_phy_rf_en = true;
            }
        }
        else {
            if (module == PHY_MODEM_MODULE){
                s_is_phy_rf_en = true;
            }
            else if ((module == PHY_BT_MODULE) || (module == PHY_WIFI_MODULE)){
                /* New module (BT or Wi-Fi) can init RF according to modem_sleep_exit */
            }
        }
        if (s_is_phy_rf_en == true){
#if CONFIG_IDF_TARGET_ESP32
            // Update time stamp
            s_phy_rf_en_ts = esp_timer_get_time();
            // Update WiFi MAC time before WiFi/BT common clock is enabled
            phy_update_wifi_mac_time(false, s_phy_rf_en_ts);
#endif
            // Enable WiFi/BT common peripheral clock
            periph_module_enable(PERIPH_WIFI_BT_COMMON_MODULE);
            phy_set_wifi_mode_only(0);

#if CONFIG_IDF_TARGET_ESP32S2BETA
            if (module == PHY_MODEM_MODULE) {
                phy_wakeup_init();
            }
            else
#endif
            if (ESP_CAL_DATA_CHECK_FAIL == register_chipv7_phy(init_data, calibration_data, mode)) {
                ESP_LOGW(TAG, "saving new calibration data because of checksum failure, mode(%d)", mode);
#ifdef CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE
                if (mode != PHY_RF_CAL_FULL) {
                    esp_phy_store_cal_data_to_nvs(calibration_data);
                }
#endif
            }

#if CONFIG_IDF_TARGET_ESP32
            coex_bt_high_prio();
#endif
        }
    }

#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    if ((module == PHY_BT_MODULE) || (module == PHY_WIFI_MODULE)){
        uint32_t phy_bt_wifi_mask = BIT(PHY_BT_MODULE) | BIT(PHY_WIFI_MODULE);
        if ((s_module_phy_rf_init & phy_bt_wifi_mask) == phy_bt_wifi_mask) { //both wifi & bt enabled
            coex_init();
            coex_preference_set(CONFIG_ESP32_WIFI_SW_COEXIST_PREFERENCE_VALUE);
            coex_resume();
        }
    }
#endif

    _lock_release(&s_phy_rf_init_lock);
    return status;
}

esp_err_t esp_phy_rf_deinit(phy_rf_module_t module)
{
    /* 3 modules may call phy_init: Wi-Fi, BT, Modem Sleep */
    if (module >= PHY_MODULE_COUNT){
        ESP_LOGE(TAG, "%s, invalid module parameter(%d), should be smaller than \
                 module count(%d)", __func__, module, PHY_MODULE_COUNT);
        return ESP_ERR_INVALID_ARG;
    }

    _lock_acquire(&s_phy_rf_init_lock);
    uint32_t s_module_phy_rf_init_old = s_module_phy_rf_init;
    uint32_t phy_bt_wifi_mask = BIT(PHY_BT_MODULE) | BIT(PHY_WIFI_MODULE);
    bool is_wifi_or_bt_enabled = !!(s_module_phy_rf_init_old & phy_bt_wifi_mask);
    bool is_both_wifi_bt_enabled = ((s_module_phy_rf_init_old & phy_bt_wifi_mask) == phy_bt_wifi_mask);
    s_module_phy_rf_init &= ~BIT(module);
    esp_err_t status = ESP_OK;

#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    if ((module == PHY_BT_MODULE) || (module == PHY_WIFI_MODULE)){
        if (is_both_wifi_bt_enabled == true) {
            coex_deinit();
        }
    }
#endif

    if ((is_wifi_or_bt_enabled == false) && (module == PHY_MODEM_MODULE)){
        /* Modem sleep should not take effect in this case */
        status = ESP_FAIL;
    }
    else if (s_is_phy_rf_en == false) {
        //do nothing
    }
    else {
        if (is_wifi_or_bt_enabled == false){
            if ((module == PHY_BT_MODULE) || (module == PHY_WIFI_MODULE)){
                s_is_phy_rf_en = false;
                ESP_LOGE(TAG, "%s, RF should not be in enabled state if both Wi-Fi and BT are disabled", __func__);
            }
        }
        else {
            if (module == PHY_MODEM_MODULE){
                s_is_phy_rf_en = false;
            }
            else if ((module == PHY_BT_MODULE) || (module == PHY_WIFI_MODULE)){
                s_is_phy_rf_en = is_both_wifi_bt_enabled ? true : false;
            }
        }

        if (s_is_phy_rf_en == false) {
            // Disable PHY and RF.
            phy_close_rf();
#if CONFIG_IDF_TARGET_ESP32
            // Update WiFi MAC time before disalbe WiFi/BT common peripheral clock
            phy_update_wifi_mac_time(true, esp_timer_get_time());
#endif
            // Disable WiFi/BT common peripheral clock. Do not disable clock for hardware RNG
            periph_module_disable(PERIPH_WIFI_BT_COMMON_MODULE);
        }
    }

    _lock_release(&s_phy_rf_init_lock);
    return status;
}



esp_err_t esp_modem_sleep_enter(modem_sleep_module_t module)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    uint32_t phy_bt_wifi_mask = BIT(PHY_BT_MODULE) | BIT(PHY_WIFI_MODULE);
#endif

    if (module >= MODEM_MODULE_COUNT){
        ESP_LOGE(TAG, "%s, invalid module parameter(%d), should be smaller than \
                 module count(%d)", __func__, module, MODEM_MODULE_COUNT);
        return ESP_ERR_INVALID_ARG;
    }
    else if (!(s_modem_sleep_module_register & BIT(module))){
        ESP_LOGW(TAG, "%s, module (%d) has not been registered", __func__, module);
        return ESP_ERR_INVALID_ARG;
    }
    else {
        _lock_acquire(&s_modem_sleep_lock);
        s_modem_sleep_module_enter |= BIT(module);
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
        _lock_acquire(&s_phy_rf_init_lock);
        if (((s_module_phy_rf_init & phy_bt_wifi_mask) == phy_bt_wifi_mask)  //both wifi & bt enabled
                && (s_modem_sleep_module_enter & (MODEM_BT_MASK | MODEM_WIFI_MASK)) != 0){
            coex_pause();
        }
        _lock_release(&s_phy_rf_init_lock);
#endif
        if (!s_is_modem_sleep_en && (s_modem_sleep_module_enter == s_modem_sleep_module_register)){
            esp_err_t status = esp_phy_rf_deinit(PHY_MODEM_MODULE);
            if (status == ESP_OK){
                s_is_modem_sleep_en = true;
            }
        }
        _lock_release(&s_modem_sleep_lock);
        return ESP_OK;
    }
}

esp_err_t esp_modem_sleep_exit(modem_sleep_module_t module)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    uint32_t phy_bt_wifi_mask = BIT(PHY_BT_MODULE) | BIT(PHY_WIFI_MODULE);
#endif

    if (module >= MODEM_MODULE_COUNT){
        ESP_LOGE(TAG, "%s, invalid module parameter(%d), should be smaller than \
                 module count(%d)", __func__, module, MODEM_MODULE_COUNT);
        return ESP_ERR_INVALID_ARG;
    }
    else if (!(s_modem_sleep_module_register & BIT(module))){
        ESP_LOGW(TAG, "%s, module (%d) has not been registered", __func__, module);
        return ESP_ERR_INVALID_ARG;
    }
    else {
        _lock_acquire(&s_modem_sleep_lock);
        s_modem_sleep_module_enter &= ~BIT(module);
        if (s_is_modem_sleep_en){
            esp_err_t status = esp_phy_rf_init(NULL,PHY_RF_CAL_NONE,NULL, PHY_MODEM_MODULE);
            if (status == ESP_OK){
                s_is_modem_sleep_en = false;
            }
        }
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
        _lock_acquire(&s_phy_rf_init_lock);
        if (((s_module_phy_rf_init & phy_bt_wifi_mask) == phy_bt_wifi_mask)  //both wifi & bt enabled
                && (s_modem_sleep_module_enter & (MODEM_BT_MASK | MODEM_WIFI_MASK)) == 0){
            coex_resume();
        }
        _lock_release(&s_phy_rf_init_lock);
#endif
        _lock_release(&s_modem_sleep_lock);
        return ESP_OK;
    }
    return ESP_OK;
}

esp_err_t esp_modem_sleep_register(modem_sleep_module_t module)
{
    if (module >= MODEM_MODULE_COUNT){
        ESP_LOGE(TAG, "%s, invalid module parameter(%d), should be smaller than \
                 module count(%d)", __func__, module, MODEM_MODULE_COUNT);
        return ESP_ERR_INVALID_ARG;
    }
    else if (s_modem_sleep_module_register & BIT(module)){
        ESP_LOGI(TAG, "%s, multiple registration of module (%d)", __func__, module);
        return ESP_OK;
    }
    else{
        _lock_acquire(&s_modem_sleep_lock);
        s_modem_sleep_module_register |= BIT(module);
        /* The module is set to enter modem sleep by default, otherwise will prevent
         * other modules from entering sleep mode if this module never call enter sleep function
         * in the future */
        s_modem_sleep_module_enter |= BIT(module);
        _lock_release(&s_modem_sleep_lock);
        return ESP_OK;
    }
}

esp_err_t esp_modem_sleep_deregister(modem_sleep_module_t module)
{
    if (module >= MODEM_MODULE_COUNT){
        ESP_LOGE(TAG, "%s, invalid module parameter(%d), should be smaller than \
                 module count(%d)", __func__, module, MODEM_MODULE_COUNT);
        return ESP_ERR_INVALID_ARG;
    }
    else if (!(s_modem_sleep_module_register & BIT(module))){
        ESP_LOGI(TAG, "%s, module (%d) has not been registered", __func__, module);
        return ESP_OK;
    }
    else{
        _lock_acquire(&s_modem_sleep_lock);
        s_modem_sleep_module_enter &= ~BIT(module);
        s_modem_sleep_module_register &= ~BIT(module);
        if (s_modem_sleep_module_register == 0){
            s_modem_sleep_module_enter = 0;
            /* Once all module are de-registered and current state
             * is modem sleep mode, we need to turn off modem sleep
             */
            if (s_is_modem_sleep_en == true){
               s_is_modem_sleep_en = false;
               esp_phy_rf_init(NULL,PHY_RF_CAL_NONE,NULL, PHY_MODEM_MODULE);
            }
        }
        _lock_release(&s_modem_sleep_lock);
        return ESP_OK;
    }
}


// PHY init data handling functions
#if CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION
#include "esp_partition.h"

const esp_phy_init_data_t* esp_phy_get_init_data(void)
{
    const esp_partition_t* partition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_PHY, NULL);
    if (partition == NULL) {
        ESP_LOGE(TAG, "PHY data partition not found");
        return NULL;
    }
    ESP_LOGD(TAG, "loading PHY init data from partition at offset 0x%x", partition->address);
    size_t init_data_store_length = sizeof(phy_init_magic_pre) +
            sizeof(esp_phy_init_data_t) + sizeof(phy_init_magic_post);
    uint8_t* init_data_store = (uint8_t*) malloc(init_data_store_length);
    if (init_data_store == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for PHY init data");
        return NULL;
    }
    esp_err_t err = esp_partition_read(partition, 0, init_data_store, init_data_store_length);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to read PHY data partition (0x%x)", err);
        return NULL;
    }
    if (memcmp(init_data_store, PHY_INIT_MAGIC, sizeof(phy_init_magic_pre)) != 0 ||
        memcmp(init_data_store + init_data_store_length - sizeof(phy_init_magic_post),
                PHY_INIT_MAGIC, sizeof(phy_init_magic_post)) != 0) {
        ESP_LOGE(TAG, "failed to validate PHY data partition");
        return NULL;
    }
    ESP_LOGD(TAG, "PHY data partition validated");
    return (const esp_phy_init_data_t*) (init_data_store + sizeof(phy_init_magic_pre));
}

void esp_phy_release_init_data(const esp_phy_init_data_t* init_data)
{
    free((uint8_t*) init_data - sizeof(phy_init_magic_pre));
}

#else // CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION

// phy_init_data.h will declare static 'phy_init_data' variable initialized with default init data

const esp_phy_init_data_t* esp_phy_get_init_data(void)
{
    ESP_LOGD(TAG, "loading PHY init data from application binary");
    return &phy_init_data;
}

void esp_phy_release_init_data(const esp_phy_init_data_t* init_data)
{
    // no-op
}
#endif // CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION


// PHY calibration data handling functions
static const char* PHY_NAMESPACE = "phy";
static const char* PHY_CAL_VERSION_KEY = "cal_version";
static const char* PHY_CAL_MAC_KEY = "cal_mac";
static const char* PHY_CAL_DATA_KEY = "cal_data";

static esp_err_t load_cal_data_from_nvs_handle(nvs_handle_t handle,
        esp_phy_calibration_data_t* out_cal_data);

static esp_err_t store_cal_data_to_nvs_handle(nvs_handle_t handle,
        const esp_phy_calibration_data_t* cal_data);

esp_err_t esp_phy_load_cal_data_from_nvs(esp_phy_calibration_data_t* out_cal_data)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(PHY_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_INITIALIZED) {
        ESP_LOGE(TAG, "%s: NVS has not been initialized. "
                "Call nvs_flash_init before starting WiFi/BT.", __func__);
        return err;
    } else if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to open NVS namespace (0x%x)", __func__, err);
        return err;
    }
    err = load_cal_data_from_nvs_handle(handle, out_cal_data);
    nvs_close(handle);
    return err;
}

esp_err_t esp_phy_store_cal_data_to_nvs(const esp_phy_calibration_data_t* cal_data)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(PHY_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to open NVS namespace (0x%x)", __func__, err);
        return err;
    }
    else {
        err = store_cal_data_to_nvs_handle(handle, cal_data);
        nvs_close(handle);
        return err;
    }
}

esp_err_t esp_phy_erase_cal_data_in_nvs(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(PHY_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: failed to open NVS phy namespace (0x%x)", __func__, err);
        return err;
    }
    else {
        err = nvs_erase_all(handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: failed to erase NVS phy namespace (0x%x)", __func__, err);
        }
        else {
            err = nvs_commit(handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: failed to commit NVS phy namespace (0x%x)", __func__, err);
            }
        }
    }
    nvs_close(handle);
    return err;
}

static esp_err_t load_cal_data_from_nvs_handle(nvs_handle_t handle,
        esp_phy_calibration_data_t* out_cal_data)
{
    esp_err_t err;
    uint32_t cal_data_version;
    err = nvs_get_u32(handle, PHY_CAL_VERSION_KEY, &cal_data_version);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to get cal_version (0x%x)", __func__, err);
        return err;
    }
    uint32_t cal_format_version = phy_get_rf_cal_version() & (~BIT(16));
    ESP_LOGV(TAG, "phy_get_rf_cal_version: %d\n", cal_format_version);
    if (cal_data_version != cal_format_version) {
        ESP_LOGD(TAG, "%s: expected calibration data format %d, found %d",
                __func__, cal_format_version, cal_data_version);
        return ESP_FAIL;
    }
    uint8_t cal_data_mac[6];
    size_t length = sizeof(cal_data_mac);
    err = nvs_get_blob(handle, PHY_CAL_MAC_KEY, cal_data_mac, &length);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to get cal_mac (0x%x)", __func__, err);
        return err;
    }
    if (length != sizeof(cal_data_mac)) {
        ESP_LOGD(TAG, "%s: invalid length of cal_mac (%d)", __func__, length);
        return ESP_ERR_INVALID_SIZE;
    }
    uint8_t sta_mac[6];
    esp_efuse_mac_get_default(sta_mac);
    if (memcmp(sta_mac, cal_data_mac, sizeof(sta_mac)) != 0) {
        ESP_LOGE(TAG, "%s: calibration data MAC check failed: expected " \
                MACSTR ", found " MACSTR,
                __func__, MAC2STR(sta_mac), MAC2STR(cal_data_mac));
        return ESP_FAIL;
    }
    length = sizeof(*out_cal_data);
    err = nvs_get_blob(handle, PHY_CAL_DATA_KEY, out_cal_data, &length);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: failed to get cal_data(0x%x)", __func__, err);
        return err;
    }
    if (length != sizeof(*out_cal_data)) {
        ESP_LOGD(TAG, "%s: invalid length of cal_data (%d)", __func__, length);
        return ESP_ERR_INVALID_SIZE;
    }
    return ESP_OK;
}

static esp_err_t store_cal_data_to_nvs_handle(nvs_handle_t handle,
        const esp_phy_calibration_data_t* cal_data)
{
    esp_err_t err;

    err = nvs_set_blob(handle, PHY_CAL_DATA_KEY, cal_data, sizeof(*cal_data));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store calibration data failed(0x%x)\n", __func__, err);
        return err;
    }

    uint8_t sta_mac[6];
    esp_efuse_mac_get_default(sta_mac);
    err = nvs_set_blob(handle, PHY_CAL_MAC_KEY, sta_mac, sizeof(sta_mac));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store calibration mac failed(0x%x)\n", __func__, err);
        return err;
    }

    uint32_t cal_format_version = phy_get_rf_cal_version() & (~BIT(16));
    ESP_LOGV(TAG, "phy_get_rf_cal_version: %d\n", cal_format_version);
    err = nvs_set_u32(handle, PHY_CAL_VERSION_KEY, cal_format_version);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store calibration version failed(0x%x)\n", __func__, err);
        return err;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store calibration nvs commit failed(0x%x)\n", __func__, err);
    }

    return err;
}

#if CONFIG_ESP32_REDUCE_PHY_TX_POWER
// TODO: fix the esp_phy_reduce_tx_power unused warning for esp32s2beta - IDF-759
static void __attribute((unused)) esp_phy_reduce_tx_power(esp_phy_init_data_t* init_data)
{
    uint8_t i;

    for(i = 0; i < PHY_TX_POWER_NUM; i++) {
        // LOWEST_PHY_TX_POWER is the lowest tx power
        init_data->params[PHY_TX_POWER_OFFSET+i] = PHY_TX_POWER_LOWEST;
    }
}
#endif

void esp_phy_load_cal_and_init(phy_rf_module_t module)
{
    esp_phy_calibration_data_t* cal_data =
            (esp_phy_calibration_data_t*) calloc(sizeof(esp_phy_calibration_data_t), 1);
    if (cal_data == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for RF calibration data");
        abort();
    }

#if CONFIG_ESP32_REDUCE_PHY_TX_POWER
    const esp_phy_init_data_t* phy_init_data = esp_phy_get_init_data();
    if (phy_init_data == NULL) {
        ESP_LOGE(TAG, "failed to obtain PHY init data");
        abort();
    }

    esp_phy_init_data_t* init_data = (esp_phy_init_data_t*) malloc(sizeof(esp_phy_init_data_t));
    if (init_data == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for phy init data");
        abort();
    }

    memcpy(init_data, phy_init_data, sizeof(esp_phy_init_data_t));
#if CONFIG_IDF_TARGET_ESP32
    // ToDo: remove once esp_reset_reason is supported on esp32s2
    if (esp_reset_reason() == ESP_RST_BROWNOUT) {
        esp_phy_reduce_tx_power(init_data);
    }
#endif
#else
    const esp_phy_init_data_t* init_data = esp_phy_get_init_data();
    if (init_data == NULL) {
        ESP_LOGE(TAG, "failed to obtain PHY init data");
        abort();
    }
#endif

#ifdef CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE
    esp_phy_calibration_mode_t calibration_mode = PHY_RF_CAL_PARTIAL;
    uint8_t sta_mac[6];
    if (rtc_get_reset_reason(0) == DEEPSLEEP_RESET) {
        calibration_mode = PHY_RF_CAL_NONE;
    }
    esp_err_t err = esp_phy_load_cal_data_from_nvs(cal_data);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to load RF calibration data (0x%x), falling back to full calibration", err);
        calibration_mode = PHY_RF_CAL_FULL;
    }

    esp_efuse_mac_get_default(sta_mac);
    memcpy(cal_data->mac, sta_mac, 6);
    esp_phy_rf_init(init_data, calibration_mode, cal_data, module);

    if (calibration_mode != PHY_RF_CAL_NONE && err != ESP_OK) {
        err = esp_phy_store_cal_data_to_nvs(cal_data);
    } else {
        err = ESP_OK;
    }
#else
    esp_phy_rf_init(init_data, PHY_RF_CAL_FULL, cal_data, module);
#endif

#if CONFIG_ESP32_REDUCE_PHY_TX_POWER
    esp_phy_release_init_data(phy_init_data);
    free(init_data);
#else
    esp_phy_release_init_data(init_data);
#endif

    free(cal_data); // PHY maintains a copy of calibration data, so we can free this
}

