/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_COEXIST_H__
#define __ESP_COEXIST_H__

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief coex prefer value
 */
typedef enum {
    ESP_COEX_PREFER_WIFI = 0,       /*!< Prefer to WiFi, WiFi will have more opportunity to use RF */
    ESP_COEX_PREFER_BT,             /*!< Prefer to bluetooth, bluetooth will have more opportunity to use RF */
    ESP_COEX_PREFER_BALANCE,        /*!< Do balance of WiFi and bluetooth */
    ESP_COEX_PREFER_NUM,            /*!< Prefer value numbers */
} esp_coex_prefer_t;

typedef enum {
    EXTERN_COEX_WIRE_1 = 0,
    EXTERN_COEX_WIRE_2,
    EXTERN_COEX_WIRE_3,
    EXTERN_COEX_WIRE_NUM,
} external_coex_wire_t;

/**
 * @brief coex status type
 */
typedef enum {
    ESP_COEX_ST_TYPE_WIFI = 0,
    ESP_COEX_ST_TYPE_BLE,
    ESP_COEX_ST_TYPE_BT,
} esp_coex_status_type_t;

/**
 * @brief external coex gpio pti
 */
typedef struct {
    int32_t in_pin0;
    int32_t in_pin1;
    int32_t out_pin0;
} esp_external_coex_gpio_set_t;

/**
 * @brief external coex pti level
 */
typedef enum {
    EXTERN_COEX_PTI_MID = 0,
    EXTERN_COEX_PTI_HIGH,
    EXTERN_COEX_PTI_NUM,
} esp_coex_pti_level_t;

/**
 * @brief external coex pti
 */
typedef struct {
    uint32_t in_pti1;
    uint32_t in_pti2;
    uint32_t in_pti3;
    uint32_t out_pti1;
    uint32_t out_pti2;
    uint32_t out_pti3;
} esp_external_coex_pti_set_t;

#define ESP_COEX_BLE_ST_MESH_CONFIG        0x08
#define ESP_COEX_BLE_ST_MESH_TRAFFIC       0x10
#define ESP_COEX_BLE_ST_MESH_STANDBY       0x20

#define ESP_COEX_BT_ST_A2DP_STREAMING      0x10
#define ESP_COEX_BT_ST_A2DP_PAUSED         0x20

/**
 * @brief Get software coexist version string
 *
 * @return : version string
 */
const char *esp_coex_version_get(void);

/**
 * @deprecated Use esp_coex_status_bit_set() and esp_coex_status_bit_clear() instead.
 *  Set coexist preference of performance
 *  For example, if prefer to bluetooth, then it will make A2DP(play audio via classic bt)
 *  more smooth while wifi is runnning something.
 *  If prefer to wifi, it will do similar things as prefer to bluetooth.
 *  Default, it prefer to balance.
 *
 *  @param prefer : the prefer enumeration value
 *  @return : ESP_OK - success, other - failed
 */
esp_err_t esp_coex_preference_set(esp_coex_prefer_t prefer);

/**
 * @brief Set coex schm status
 * @param type : WIFI/BLE/BT
 * @param status : WIFI/BLE/BT STATUS
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_coex_status_bit_set(esp_coex_status_type_t type, uint32_t status);

/**
 * @brief Clear coex schm status
 * @param type : WIFI/BLE/BT
 * @param status : WIFI/BLE/BT STATUS
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_coex_status_bit_clear(esp_coex_status_type_t type, uint32_t status);

#if CONFIG_EXTERNAL_COEX_ENABLE
/**
 * @brief Setup gpio pin and corresponding pti level, start external coex.
 * @param wire_type : to select the whole external coex gpio number.
 * @param gpio_pin : gpio pin number to choose.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_enable_extern_coex_gpio_pin(external_coex_wire_t wire_type,
                    esp_external_coex_gpio_set_t gpio_pin);

esp_err_t esp_disable_extern_coex_gpio_pin();
#endif

#ifdef __cplusplus
}
#endif


#endif /* __ESP_COEXIST_H__ */
