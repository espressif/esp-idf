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

#include "rom/ets_sys.h"
#include "rom/rtc.h"
#include "soc/rtc.h"
#include "soc/dport_reg.h"

#include "esp_err.h"
#include "esp_phy_init.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "phy.h"
#include "phy_init_data.h"
#include "esp_coexist.h"
#include "driver/periph_ctrl.h"

static const char* TAG = "phy_init";

/* Count value to indicate if there is peripheral that has initialized PHY and RF */
static int s_phy_rf_init_count = 0;

static _lock_t s_phy_rf_init_lock;

esp_err_t esp_phy_rf_init(const esp_phy_init_data_t* init_data,
        esp_phy_calibration_mode_t mode, esp_phy_calibration_data_t* calibration_data)
{
    assert((s_phy_rf_init_count <= 1) && (s_phy_rf_init_count >= 0));

    _lock_acquire(&s_phy_rf_init_lock);
    if (s_phy_rf_init_count == 0) {
        // Enable WiFi/BT common peripheral clock
        periph_module_enable(PERIPH_WIFI_BT_COMMON_MODULE);
        ESP_LOGV(TAG, "register_chipv7_phy, init_data=%p, cal_data=%p, mode=%d",
                init_data, calibration_data, mode);
        phy_set_wifi_mode_only(0);
        register_chipv7_phy(init_data, calibration_data, mode);
        coex_bt_high_prio();
    } else {
#if CONFIG_SW_COEXIST_ENABLE
        coex_init();
#endif
    }
    s_phy_rf_init_count++;
    _lock_release(&s_phy_rf_init_lock);
    return ESP_OK;
}

esp_err_t esp_phy_rf_deinit(void)
{
    assert((s_phy_rf_init_count <= 2) && (s_phy_rf_init_count >= 1));

    _lock_acquire(&s_phy_rf_init_lock);
    if (s_phy_rf_init_count == 1) {
        // Disable PHY and RF.
        phy_close_rf();
        // Disable WiFi/BT common peripheral clock. Do not disable clock for hardware RNG
        periph_module_disable(PERIPH_WIFI_BT_COMMON_MODULE);
    } else {
#if CONFIG_SW_COEXIST_ENABLE
        coex_deinit();
#endif
    }
    s_phy_rf_init_count--;
    _lock_release(&s_phy_rf_init_lock);
    return ESP_OK;
}

// PHY init data handling functions
#if CONFIG_ESP32_PHY_INIT_DATA_IN_PARTITION
#include "esp_partition.h"

const esp_phy_init_data_t* esp_phy_get_init_data()
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

const esp_phy_init_data_t* esp_phy_get_init_data()
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

static esp_err_t load_cal_data_from_nvs_handle(nvs_handle handle,
        esp_phy_calibration_data_t* out_cal_data);

static esp_err_t store_cal_data_to_nvs_handle(nvs_handle handle,
        const esp_phy_calibration_data_t* cal_data);

esp_err_t esp_phy_load_cal_data_from_nvs(esp_phy_calibration_data_t* out_cal_data)
{
    nvs_handle handle;
    esp_err_t err = nvs_open(PHY_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_INITIALIZED) {
        ESP_LOGE(TAG, "%s: NVS has not been initialized. "
                "Call nvs_flash_init before starting WiFi/BT.", __func__);
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
    nvs_handle handle;
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

static esp_err_t load_cal_data_from_nvs_handle(nvs_handle handle,
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

static esp_err_t store_cal_data_to_nvs_handle(nvs_handle handle,
        const esp_phy_calibration_data_t* cal_data)
{
    esp_err_t err;
    uint32_t cal_format_version = phy_get_rf_cal_version() & (~BIT(16));
    ESP_LOGV(TAG, "phy_get_rf_cal_version: %d\n", cal_format_version);
    err = nvs_set_u32(handle, PHY_CAL_VERSION_KEY, cal_format_version);
    if (err != ESP_OK) {
        return err;
    }
    uint8_t sta_mac[6];
    esp_efuse_mac_get_default(sta_mac);
    err = nvs_set_blob(handle, PHY_CAL_MAC_KEY, sta_mac, sizeof(sta_mac));
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_set_blob(handle, PHY_CAL_DATA_KEY, cal_data, sizeof(*cal_data));
    return err;
}

void esp_phy_load_cal_and_init(void)
{
    esp_phy_calibration_data_t* cal_data =
            (esp_phy_calibration_data_t*) calloc(sizeof(esp_phy_calibration_data_t), 1);
    if (cal_data == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for RF calibration data");
        abort();
    }

    const esp_phy_init_data_t* init_data = esp_phy_get_init_data();
    if (init_data == NULL) {
        ESP_LOGE(TAG, "failed to obtain PHY init data");
        abort();
    }

#ifdef CONFIG_ESP32_PHY_CALIBRATION_AND_DATA_STORAGE
    esp_phy_calibration_mode_t calibration_mode = PHY_RF_CAL_PARTIAL;
    if (rtc_get_reset_reason(0) == DEEPSLEEP_RESET) {
        calibration_mode = PHY_RF_CAL_NONE;
    }
    esp_err_t err = esp_phy_load_cal_data_from_nvs(cal_data);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to load RF calibration data (0x%x), falling back to full calibration", err);
        calibration_mode = PHY_RF_CAL_FULL;
    }

    esp_phy_rf_init(init_data, calibration_mode, cal_data);

    if (calibration_mode != PHY_RF_CAL_NONE && err != ESP_OK) {
        err = esp_phy_store_cal_data_to_nvs(cal_data);
    } else {
        err = ESP_OK;
    }
#else
    esp_phy_rf_init(init_data, PHY_RF_CAL_FULL, cal_data);
#endif

    esp_phy_release_init_data(init_data);

    free(cal_data); // PHY maintains a copy of calibration data, so we can free this
}

