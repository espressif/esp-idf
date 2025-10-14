/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_COEXIST_H__
#define __ESP_COEXIST_H__

#include <stdbool.h>
#include "esp_err.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXTERNAL_COEXIST_WIRE_1 0
#define EXTERNAL_COEXIST_WIRE_2 1
#define EXTERNAL_COEXIST_WIRE_3 2
#define EXTERNAL_COEXIST_WIRE_4 3
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
    EXTERN_COEX_WIRE_1 = EXTERNAL_COEXIST_WIRE_1,
    EXTERN_COEX_WIRE_2 = EXTERNAL_COEXIST_WIRE_2,
    EXTERN_COEX_WIRE_3 = EXTERNAL_COEXIST_WIRE_3,
    EXTERN_COEX_WIRE_4 = EXTERNAL_COEXIST_WIRE_4,
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

#if CONFIG_EXTERNAL_COEX_ENABLE
/**
 * @brief external coex gpio pti
 */
typedef struct {
    union {
        uint32_t in_pin0 __attribute__((deprecated("Use 'request' instead")));
        gpio_num_t request; /**< request gpio signal from follower to leader */
    };
    union {
        uint32_t in_pin1 __attribute__((deprecated("Use 'priority' instead")));
        gpio_num_t priority; /**< request gpio signal priority from follower to leader */
    };
    union {
        uint32_t out_pin0 __attribute__((deprecated("Use 'grant' instead")));
        gpio_num_t grant; /**< grant gpio signal from leader to follower */
    };
    union {
        uint32_t out_pin1 __attribute__((deprecated("Use 'tx_line' instead")));
        gpio_num_t tx_line; /**< tx_line gpio signal from leader to follower, indicates whether the leader's WiFi is transmitting or not*/
    };
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
 * @brief external coex role
 */
typedef enum {
    EXTERNAL_COEX_LEADER_ROLE = 0,
    EXTERNAL_COEX_FOLLOWER_ROLE = 2,
    EXTERNAL_COEX_UNKNOWN_ROLE,
} esp_extern_coex_work_mode_t;

/**
 * @brief external coex advance setup
 */
typedef struct {
    esp_extern_coex_work_mode_t work_mode;
    uint8_t delay_us;
    bool is_high_valid;
} esp_external_coex_advance_t;
#endif

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
 *  more smooth while wifi is running something.
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
 * @brief Configure work mode, the default work mode is leader role.
 * @param work_mode : work mode.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_external_coex_set_work_mode(esp_extern_coex_work_mode_t work_mode);

/**
 * @brief Setup gpio pin and corresponding pti level, start external coex,
 *        the default work mode is leader role, the default output grant validate pin is high,
 *        and the default delay output grant value is zero.
 * @param wire_type : to select the whole external coex gpio number.
 * @param gpio_pin : gpio pin number to choose.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_enable_extern_coex_gpio_pin(external_coex_wire_t wire_type,
                     esp_external_coex_gpio_set_t gpio_pin);

/**
 * @brief Disable external coex.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_disable_extern_coex_gpio_pin(void);

#if SOC_EXTERNAL_COEX_ADVANCE
/**
 * @brief Configure leader work mode, gpio pin correspondly and finally enable external coex,
 *        demand not to call the legacy function of `esp_enable_extern_coex_gpio_pin` any more.
 * @param wire_type : to select the whole external coex gpio number.
 * @param request : request gpio pin number to select.
 * @param priority : priority gpio pin number to select.
 * @param grant : grant gpio pin number to select.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_external_coex_leader_role_set_gpio_pin(external_coex_wire_t wire_type, uint32_t request, uint32_t priority,
                uint32_t grant) __attribute__((deprecated("Please use esp_external_coex_set_work_mode and esp_enable_extern_coex_gpio_pin instead")));

/**
 * @brief Configure follower work mode, gpio pin correspondly and finally enable external coex,
 *        demand not to call the legacy function of `esp_enable_extern_coex_gpio_pin` any more.
 * @param wire_type : to select the whole external coex gpio number.
 * @param request : request gpio pin number to select.
 * @param priority : priority gpio pin number to select.
 * @param grant : grant gpio pin number to select.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_external_coex_follower_role_set_gpio_pin(external_coex_wire_t wire_type, uint32_t request, uint32_t priority,
                uint32_t grant) __attribute__((deprecated("Please use esp_external_coex_set_work_mode and esp_enable_extern_coex_gpio_pin instead")));

/**
 * @brief Configure output grant signal latency in delay microseconds only for leader role of external coex,
 *        demand to call this function before `esp_external_coex_leader_role_set_gpio_pin`,
 *        if users want to setup output delay value.
 * @param delay_us : to setup how many microseconds the output signal performs latency.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_external_coex_set_grant_delay(uint8_t delay_us);

/**
 * @brief Configure output grant signal is high validate or not only for leader role of external coex,
 *        demand to call this function before `esp_external_coex_leader_role_set_gpio_pin`,
 *        if users want to setup output grant validate pin value.
 * @param is_high_valid : to select true means the output grant signal validate is high, other - validate is low.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_external_coex_set_validate_high(bool is_high_valid);
#endif /* SOC_EXTERNAL_COEX_ADVANCE */
#endif /* CONFIG_EXTERNAL_COEX_ENABLE */

#if CONFIG_ESP_COEX_SW_COEXIST_ENABLE && CONFIG_SOC_IEEE802154_SUPPORTED
/**
 * @brief Enable Wi-Fi and 802.15.4 coexistence.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_coex_wifi_i154_enable(void);
#endif

#if CONFIG_ESP_COEX_GPIO_DEBUG
/**
 * @brief Enable coexist GPIO debug.
 *        To fully enable this feature, make sure functions in rom_funcs are out of ROM.
 * @return : ESP_OK - success, other - failed
 */
esp_err_t esp_coexist_debug_init(void);
#endif

#ifdef __cplusplus
}
#endif


#endif /* __ESP_COEXIST_H__ */
