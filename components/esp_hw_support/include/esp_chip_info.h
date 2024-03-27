/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Chip models
 */
typedef enum {
    CHIP_ESP32  = 1, //!< ESP32
    CHIP_ESP32S2 = 2, //!< ESP32-S2
    CHIP_ESP32S3 = 9, //!< ESP32-S3
    CHIP_ESP32C3 = 5, //!< ESP32-C3
    CHIP_ESP32C2 = 12, //!< ESP32-C2
    CHIP_ESP32C6 = 13, //!< ESP32-C6
    CHIP_ESP32H2 = 16, //!< ESP32-H2
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION  // TODO: IDF-9197
    CHIP_ESP32C5 = 17, //!< ESP32-C5 beta3 (MPW)
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
    CHIP_ESP32C5 = 23, //!< ESP32-C5 MP
#endif
    CHIP_ESP32P4 = 18, //!< ESP32-P4
    CHIP_ESP32C61= 20, //!< ESP32-C61
    CHIP_POSIX_LINUX = 999, //!< The code is running on POSIX/Linux simulator
} esp_chip_model_t;

/* Chip feature flags, used in esp_chip_info_t */
#define CHIP_FEATURE_EMB_FLASH      BIT(0)      //!< Chip has embedded flash memory
#define CHIP_FEATURE_WIFI_BGN       BIT(1)      //!< Chip has 2.4GHz WiFi
#define CHIP_FEATURE_BLE            BIT(4)      //!< Chip has Bluetooth LE
#define CHIP_FEATURE_BT             BIT(5)      //!< Chip has Bluetooth Classic
#define CHIP_FEATURE_IEEE802154     BIT(6)      //!< Chip has IEEE 802.15.4
#define CHIP_FEATURE_EMB_PSRAM      BIT(7)      //!< Chip has embedded psram

/**
 * @brief The structure represents information about the chip
 */
typedef struct {
    esp_chip_model_t model;  //!< chip model, one of esp_chip_model_t
    uint32_t features;       //!< bit mask of CHIP_FEATURE_x feature flags
    uint16_t revision;       //!< chip revision number (in format MXX; where M - wafer major version, XX - wafer minor version)
    uint8_t cores;           //!< number of CPU cores
} esp_chip_info_t;

/**
 * @brief Fill an esp_chip_info_t structure with information about the chip
 * @param[out] out_info structure to be filled
 */
void esp_chip_info(esp_chip_info_t* out_info);

#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
/**
 * @brief Cache lock bug exists or not
 *
 * @return
 *          - ture : bug exists
 *          - false : bug not exists
 */
bool soc_has_cache_lock_bug(void);
#endif

#ifdef __cplusplus
}
#endif
