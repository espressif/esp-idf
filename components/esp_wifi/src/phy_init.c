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
#include "esp_efuse.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "phy.h"
#include "phy_init_data.h"
#include "esp_coexist_internal.h"
#include "driver/periph_ctrl.h"
#include "esp_private/wifi.h"
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#include "esp32/rom/rtc.h"
#include "esp32/rom/crc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/ets_sys.h"
#include "esp32s2/rom/rtc.h"
#include "esp32s2/rom/crc.h"
#endif

#if CONFIG_IDF_TARGET_ESP32
extern wifi_mac_time_update_cb_t s_wifi_mac_time_update_cb;
#endif

static const char* TAG = "phy_init";

static _lock_t s_phy_access_lock;

/* Indicate PHY is calibrated or not */
static bool s_is_phy_calibrated = false;

/* Reference count of enabling PHY */
static uint8_t s_phy_access_ref = 0;

#if CONFIG_IDF_TARGET_ESP32
/* time stamp updated when the PHY/RF is turned on */
static int64_t s_phy_rf_en_ts = 0;
#endif

/* PHY spinlock for libphy.a */
static DRAM_ATTR portMUX_TYPE s_phy_int_mux = portMUX_INITIALIZER_UNLOCKED;

/* Memory to store PHY digital registers */
static uint32_t* s_phy_digital_regs_mem = NULL;

#if CONFIG_ESP32_SUPPORT_MULTIPLE_PHY_INIT_DATA_BIN
/* The following static variables are only used by Wi-Fi tasks, so they can be handled without lock */
static phy_init_data_type_t s_phy_init_data_type = 0;

static phy_init_data_type_t s_current_apply_phy_init_data = 0;

static char s_phy_current_country[PHY_COUNTRY_CODE_LEN] = {0};

/* Whether it is a new bin */
static bool s_multiple_phy_init_data_bin = false;

/* PHY init data type array */
static char* s_phy_type[ESP_PHY_INIT_DATA_TYPE_NUMBER] = {"DEFAULT", "SRRC", "FCC", "CE", "NCC", "KCC", "MIC", "IC", 
    "ACMA", "ANATEL", "ISED", "WPC", "OFCA", "IFETEL", "RCM"};

/* Country and PHY init data type map */
static phy_country_to_bin_type_t s_country_code_map_type_table[] = {
    {"AT",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"AU",  ESP_PHY_INIT_DATA_TYPE_ACMA},
    {"BE",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"BG",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"BR",  ESP_PHY_INIT_DATA_TYPE_ANATEL},
    {"CA",  ESP_PHY_INIT_DATA_TYPE_ISED},
    {"CH",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"CN",  ESP_PHY_INIT_DATA_TYPE_SRRC},     
    {"CY",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"CZ",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"DE",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"DK",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"EE",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"ES",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"FI",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"FR",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"GB",  ESP_PHY_INIT_DATA_TYPE_CE},   
    {"GR",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"HK",  ESP_PHY_INIT_DATA_TYPE_OFCA},
    {"HR",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"HU",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"IE",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"IN",  ESP_PHY_INIT_DATA_TYPE_WPC},
    {"IS",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"IT",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"JP",  ESP_PHY_INIT_DATA_TYPE_MIC}, 
    {"KR",  ESP_PHY_INIT_DATA_TYPE_KCC},
    {"LI",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"LT",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"LU",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"LV",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"MT",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"MX",  ESP_PHY_INIT_DATA_TYPE_IFETEL},
    {"NL",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"NO",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"NZ",  ESP_PHY_INIT_DATA_TYPE_RCM},
    {"PL",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"PT",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"RO",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"SE",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"SI",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"SK",  ESP_PHY_INIT_DATA_TYPE_CE},
    {"TW",  ESP_PHY_INIT_DATA_TYPE_NCC},
    {"US",  ESP_PHY_INIT_DATA_TYPE_FCC},
};
#endif
uint32_t IRAM_ATTR phy_enter_critical(void)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&s_phy_int_mux);

    } else {
        portENTER_CRITICAL(&s_phy_int_mux);
    }
    // Interrupt level will be stored in current tcb, so always return zero.
    return 0;
}

void IRAM_ATTR phy_exit_critical(uint32_t level)
{
    // Param level don't need any more, ignore it.
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&s_phy_int_mux);
    } else {
        portEXIT_CRITICAL(&s_phy_int_mux);
    }
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

IRAM_ATTR void esp_phy_common_clock_enable(void)
{
    wifi_bt_common_module_enable();
}

IRAM_ATTR void esp_phy_common_clock_disable(void)
{
    wifi_bt_common_module_disable();
}

static inline void phy_digital_regs_store(void)
{
    if (s_phy_digital_regs_mem == NULL) {
        s_phy_digital_regs_mem = (uint32_t *)malloc(SOC_PHY_DIG_REGS_MEM_SIZE);
    }

    if (s_phy_digital_regs_mem != NULL) {
        phy_dig_reg_backup(true, s_phy_digital_regs_mem);
    }
}

static inline void phy_digital_regs_load(void)
{
    if (s_phy_digital_regs_mem != NULL) {
        phy_dig_reg_backup(false, s_phy_digital_regs_mem);
    }
}

bool esp_phy_is_initialized(void)
{
    return s_is_phy_calibrated;
}

void esp_phy_enable(void)
{
    _lock_acquire(&s_phy_access_lock);

    if (s_phy_access_ref == 0) {
#if CONFIG_IDF_TARGET_ESP32
        // Update time stamp
        s_phy_rf_en_ts = esp_timer_get_time();
        // Update WiFi MAC time before WiFi/BT common clock is enabled
        phy_update_wifi_mac_time(false, s_phy_rf_en_ts);
#endif
        esp_phy_common_clock_enable();

        if (s_is_phy_calibrated == false) {
            esp_phy_load_cal_and_init();
            s_is_phy_calibrated = true;
        }
        else {
            phy_wakeup_init();
            phy_digital_regs_load();
        }

#if CONFIG_IDF_TARGET_ESP32
        coex_bt_high_prio();
#endif
    }
    s_phy_access_ref++;

    _lock_release(&s_phy_access_lock);
}

void esp_phy_disable(void)
{
    _lock_acquire(&s_phy_access_lock);

    s_phy_access_ref--;
    if (s_phy_access_ref == 0) {
        phy_digital_regs_store();
        // Disable PHY and RF.
        phy_close_rf();
#if CONFIG_IDF_TARGET_ESP32
        // Update WiFi MAC time before disalbe WiFi/BT common peripheral clock
        phy_update_wifi_mac_time(true, esp_timer_get_time());
#endif
        // Disable WiFi/BT common peripheral clock. Do not disable clock for hardware RNG
        esp_phy_common_clock_disable();
    }

    _lock_release(&s_phy_access_lock);
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
    // read phy data from flash
    esp_err_t err = esp_partition_read(partition, 0, init_data_store, init_data_store_length);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to read PHY data partition (0x%x)", err);
        free(init_data_store);
        return NULL;
    }
    // verify data
    if (memcmp(init_data_store, PHY_INIT_MAGIC, sizeof(phy_init_magic_pre)) != 0 ||
        memcmp(init_data_store + init_data_store_length - sizeof(phy_init_magic_post),
                PHY_INIT_MAGIC, sizeof(phy_init_magic_post)) != 0) {
#ifndef CONFIG_ESP32_PHY_DEFAULT_INIT_IF_INVALID
        ESP_LOGE(TAG, "failed to validate PHY data partition");
        free(init_data_store);
        return NULL;
#else
        ESP_LOGE(TAG, "failed to validate PHY data partition, restoring default data into flash...");

        memcpy(init_data_store,
               PHY_INIT_MAGIC, sizeof(phy_init_magic_pre));
        memcpy(init_data_store + sizeof(phy_init_magic_pre),
               &phy_init_data, sizeof(phy_init_data));
        memcpy(init_data_store + sizeof(phy_init_magic_pre) + sizeof(phy_init_data),
               PHY_INIT_MAGIC, sizeof(phy_init_magic_post));

        assert(memcmp(init_data_store, PHY_INIT_MAGIC, sizeof(phy_init_magic_pre)) == 0);
        assert(memcmp(init_data_store + init_data_store_length - sizeof(phy_init_magic_post),
                      PHY_INIT_MAGIC, sizeof(phy_init_magic_post)) == 0);

        // write default data
        err = esp_partition_write(partition, 0, init_data_store, init_data_store_length);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed to write default PHY data partition (0x%x)", err);
            free(init_data_store);
            return NULL;
        }
#endif // CONFIG_ESP32_PHY_DEFAULT_INIT_IF_INVALID
    }
#if CONFIG_ESP32_SUPPORT_MULTIPLE_PHY_INIT_DATA_BIN    
    if ((*(init_data_store + (sizeof(phy_init_magic_pre) + PHY_SUPPORT_MULTIPLE_BIN_OFFSET)))) {
        s_multiple_phy_init_data_bin = true;
        ESP_LOGI(TAG, "Support multiple PHY init data bins");
    } else {
        ESP_LOGW(TAG, "Does not support multiple PHY init data bins");
    }
#endif
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
// TODO: fix the esp_phy_reduce_tx_power unused warning for esp32s2 - IDF-759
static void __attribute((unused)) esp_phy_reduce_tx_power(esp_phy_init_data_t* init_data)
{
    uint8_t i;

    for(i = 0; i < PHY_TX_POWER_NUM; i++) {
        // LOWEST_PHY_TX_POWER is the lowest tx power
        init_data->params[PHY_TX_POWER_OFFSET+i] = PHY_TX_POWER_LOWEST;
    }
}
#endif

void esp_phy_load_cal_and_init(void)
{
    char * phy_version = get_phy_version_str();
    ESP_LOGI(TAG, "phy_version %s", phy_version);

#if CONFIG_IDF_TARGET_ESP32S2
    phy_eco_version_sel(esp_efuse_get_chip_ver());
#endif

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
    esp_err_t ret = register_chipv7_phy(init_data, cal_data, calibration_mode);
    if (ret == ESP_CAL_DATA_CHECK_FAIL) {
        ESP_LOGW(TAG, "saving new calibration data because of checksum failure, mode(%d)", calibration_mode);
    }

    if ((calibration_mode != PHY_RF_CAL_NONE && err != ESP_OK) ||
            (calibration_mode != PHY_RF_CAL_FULL && ret == ESP_CAL_DATA_CHECK_FAIL)) {
        err = esp_phy_store_cal_data_to_nvs(cal_data);
    } else {
        err = ESP_OK;
    }
#else
    register_chipv7_phy(init_data, cal_data, PHY_RF_CAL_FULL);
#endif

#if CONFIG_ESP32_REDUCE_PHY_TX_POWER
    esp_phy_release_init_data(phy_init_data);
    free(init_data);
#else
    esp_phy_release_init_data(init_data);
#endif

    free(cal_data); // PHY maintains a copy of calibration data, so we can free this
}

#if CONFIG_ESP32_SUPPORT_MULTIPLE_PHY_INIT_DATA_BIN
static esp_err_t phy_crc_check_init_data(uint8_t* init_data, const uint8_t* checksum, size_t init_data_length)
{   
    uint32_t crc_data = 0;
    crc_data = crc32_le(crc_data, init_data, init_data_length);
    uint32_t crc_size_conversion = htonl(crc_data);

    if (crc_size_conversion != *(uint32_t*)(checksum)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

static uint8_t phy_find_bin_type_according_country(const char* country)
{
    uint32_t i = 0;
    uint8_t phy_init_data_type = 0;

    for (i = 0; i < sizeof(s_country_code_map_type_table)/sizeof(phy_country_to_bin_type_t); i++) 
    {
        if (!memcmp(country, s_country_code_map_type_table[i].cc, sizeof(s_phy_current_country))) {
            phy_init_data_type = s_country_code_map_type_table[i].type;
            ESP_LOGD(TAG, "Current country is %c%c, PHY init data type is %s\n", s_country_code_map_type_table[i].cc[0],
                    s_country_code_map_type_table[i].cc[1], s_phy_type[s_country_code_map_type_table[i].type]);
            break;
        }
    }

    if (i == sizeof(s_country_code_map_type_table)/sizeof(phy_country_to_bin_type_t)) {
        phy_init_data_type = ESP_PHY_INIT_DATA_TYPE_DEFAULT;
        ESP_LOGW(TAG, "Use the default certification code beacuse %c%c doesn't have a certificate", country[0], country[1]);
    }

    return phy_init_data_type;
}

static esp_err_t phy_find_bin_data_according_type(uint8_t* out_init_data_store,
        const phy_control_info_data_t* init_data_control_info,
        const uint8_t* init_data_multiple,
        phy_init_data_type_t init_data_type)
{
      int i = 0;
      for (i = 0; i < init_data_control_info->number; i++) {
          if (init_data_type == *(init_data_multiple + (i * sizeof(esp_phy_init_data_t)) + PHY_INIT_DATA_TYPE_OFFSET)) {
              memcpy(out_init_data_store + sizeof(phy_init_magic_pre),
                      init_data_multiple + (i * sizeof(esp_phy_init_data_t)), sizeof(esp_phy_init_data_t));
              break;
          }
      }

      if (i == init_data_control_info->number) {
          return ESP_FAIL;
      }
      return ESP_OK;
}

static esp_err_t phy_get_multiple_init_data(const esp_partition_t* partition,
        uint8_t* init_data_store,
        size_t init_data_store_length,
        phy_init_data_type_t init_data_type)
{
    phy_control_info_data_t* init_data_control_info = (phy_control_info_data_t*) malloc(sizeof(phy_control_info_data_t));
    if (init_data_control_info == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for PHY init data control info");
        return ESP_FAIL;
    }
    
    esp_err_t err = esp_partition_read(partition, init_data_store_length, init_data_control_info, sizeof(phy_control_info_data_t));
    if (err != ESP_OK) {
        free(init_data_control_info);
        ESP_LOGE(TAG, "failed to read PHY control info data partition (0x%x)", err);
        return ESP_FAIL;
    }

    if ((init_data_control_info->check_algorithm) == PHY_CRC_ALGORITHM) {
        err =  phy_crc_check_init_data(init_data_control_info->multiple_bin_checksum, init_data_control_info->control_info_checksum,
                sizeof(phy_control_info_data_t) - sizeof(init_data_control_info->control_info_checksum));
        if (err != ESP_OK) {
            free(init_data_control_info);
            ESP_LOGE(TAG, "PHY init data control info check error");
            return ESP_FAIL;
        }
    } else {
        free(init_data_control_info);
        ESP_LOGE(TAG, "Check algorithm not CRC, PHY init data update failed");
        return ESP_FAIL;
    }

    uint8_t* init_data_multiple = (uint8_t*) malloc(sizeof(esp_phy_init_data_t) * init_data_control_info->number);
    if (init_data_multiple == NULL) {
        free(init_data_control_info);
        ESP_LOGE(TAG, "failed to allocate memory for PHY init data multiple bin");
        return ESP_FAIL;
    } 
    
    err = esp_partition_read(partition, init_data_store_length + sizeof(phy_control_info_data_t),
            init_data_multiple, sizeof(esp_phy_init_data_t) * init_data_control_info->number);
    if (err != ESP_OK) {
        free(init_data_multiple);
        free(init_data_control_info);
        ESP_LOGE(TAG, "failed to read PHY init data multiple bin partition (0x%x)", err);
        return ESP_FAIL;
    }
    
    if ((init_data_control_info->check_algorithm) == PHY_CRC_ALGORITHM) {
        err = phy_crc_check_init_data(init_data_multiple, init_data_control_info->multiple_bin_checksum, 
                sizeof(esp_phy_init_data_t) * init_data_control_info->number);
        if (err != ESP_OK) {
            free(init_data_multiple);
            free(init_data_control_info);
            ESP_LOGE(TAG, "PHY init data multiple bin check error");
            return ESP_FAIL;
        }
    } else {
        free(init_data_multiple);
        free(init_data_control_info);
        ESP_LOGE(TAG, "Check algorithm not CRC, PHY init data update failed");
        return ESP_FAIL;
    }

    err = phy_find_bin_data_according_type(init_data_store, init_data_control_info, init_data_multiple, init_data_type);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "%s has not been certified, use DEFAULT PHY init data", s_phy_type[init_data_type]);
        s_phy_init_data_type = ESP_PHY_INIT_DATA_TYPE_DEFAULT;
    } else {
        s_phy_init_data_type = init_data_type;
    }

    free(init_data_multiple);
    free(init_data_control_info);
    return ESP_OK;
}

esp_err_t esp_phy_update_init_data(phy_init_data_type_t init_data_type)
{
    const esp_partition_t* partition = esp_partition_find_first(
          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_PHY, NULL);
    if (partition == NULL) {
        ESP_LOGE(TAG, "Updated country code PHY data partition not found");
        return ESP_FAIL;
    }
    size_t init_data_store_length = sizeof(phy_init_magic_pre) +
        sizeof(esp_phy_init_data_t) + sizeof(phy_init_magic_post);
    uint8_t* init_data_store = (uint8_t*) malloc(init_data_store_length);
    if (init_data_store == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for updated country code PHY init data");
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = esp_partition_read(partition, 0, init_data_store, init_data_store_length);
    if (err != ESP_OK) {
        free(init_data_store);
        ESP_LOGE(TAG, "failed to read updated country code PHY data partition (0x%x)", err);
        return ESP_FAIL;
    }
    if (memcmp(init_data_store, PHY_INIT_MAGIC, sizeof(phy_init_magic_pre)) != 0 ||
            memcmp(init_data_store + init_data_store_length - sizeof(phy_init_magic_post),
                PHY_INIT_MAGIC, sizeof(phy_init_magic_post)) != 0) {
        free(init_data_store);
        ESP_LOGE(TAG, "failed to validate updated country code PHY data partition");
        return ESP_FAIL;
    }

    //find init data bin according init data type
    if (init_data_type != ESP_PHY_INIT_DATA_TYPE_DEFAULT) {
        err = phy_get_multiple_init_data(partition, init_data_store, init_data_store_length, init_data_type);
        if (err != ESP_OK) {
            free(init_data_store);
#if CONFIG_ESP32_PHY_INIT_DATA_ERROR
            abort();
#else
            return ESP_FAIL;
#endif
        }
    } else {
        s_phy_init_data_type = ESP_PHY_INIT_DATA_TYPE_DEFAULT;
    }

    if (s_current_apply_phy_init_data != s_phy_init_data_type) {
        err = esp_phy_apply_phy_init_data(init_data_store + sizeof(phy_init_magic_pre));
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "PHY init data failed to load");
            free(init_data_store);
            return ESP_FAIL;
        }

        ESP_LOGI(TAG, "PHY init data type updated from %s to %s",
                s_phy_type[s_current_apply_phy_init_data], s_phy_type[s_phy_init_data_type]);
        s_current_apply_phy_init_data = s_phy_init_data_type;
    }

    free(init_data_store);
    return ESP_OK;
}
#endif

esp_err_t esp_phy_update_country_info(const char *country)
{
#if CONFIG_ESP32_SUPPORT_MULTIPLE_PHY_INIT_DATA_BIN
    uint8_t phy_init_data_type_map = 0;

    if (!s_multiple_phy_init_data_bin) {
        ESP_LOGD(TAG, "Does not support multiple PHY init data bins");
        return ESP_FAIL;
    }

   //if country equal s_phy_current_country, return;
    if (!memcmp(country, s_phy_current_country, sizeof(s_phy_current_country))) {
        return ESP_OK;
    }

    memcpy(s_phy_current_country, country, sizeof(s_phy_current_country));

    phy_init_data_type_map = phy_find_bin_type_according_country(country);
    if (phy_init_data_type_map == s_phy_init_data_type) {
        return ESP_OK;
    }
    
    esp_err_t err =  esp_phy_update_init_data(phy_init_data_type_map);
    if (err != ESP_OK) {
        return err;
    }
#endif 
    return ESP_OK;
}
