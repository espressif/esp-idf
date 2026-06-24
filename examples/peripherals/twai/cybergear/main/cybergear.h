/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_twai_onchip.h"
#include "cybergear_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CyberGear motor config
 */
typedef struct {
    uint32_t motor_id;              /*!< Motor ID */
    uint32_t esp_node_id;           /*!< ESP Node ID */
} cybergear_config_t;

/**
 * @brief Create a CyberGear motor
 *
 * @param[in] twai_node The TWAI node handle
 * @param[in] motor_config The motor config
 * @param[out] ret_motor The motor handle
 * @return
 *      - ESP_OK: Create CyberGear motor successfully
 *      - ESP_ERR_INVALID_ARG: Create CyberGear motor failed because of invalid argument
 *      - ESP_FAIL: Create CyberGear motor failed because of other error
 */
esp_err_t cybergear_new_twai_motor(twai_node_handle_t twai_node, const cybergear_config_t *motor_config, cybergear_handle_t *ret_motor);

/**
 * @brief Delete a CyberGear motor
 *
 * @param[in] motor The motor handle
 * @return
 *      - ESP_OK: Delete CyberGear motor successfully
 *      - ESP_ERR_INVALID_ARG: Delete CyberGear motor failed because of invalid argument
 *      - ESP_FAIL: Delete CyberGear motor failed because of other error
 */
esp_err_t cybergear_del(cybergear_handle_t motor);

/**
 * @brief Enable the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @return
 *      - ESP_OK: Enable CyberGear motor successfully
 *      - ESP_ERR_INVALID_ARG: Enable CyberGear motor failed because of invalid argument
 *      - ESP_FAIL: Enable CyberGear motor failed because of other error
 */
esp_err_t cybergear_enable(cybergear_handle_t motor);

/**
 * @brief Disable the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] clear_fault Whether to clear the fault
 * @return
 *      - ESP_OK: Disable CyberGear motor successfully
 *      - ESP_ERR_INVALID_ARG: Disable CyberGear motor failed because of invalid argument
 *      - ESP_FAIL: Disable CyberGear motor failed because of other error
 */
esp_err_t cybergear_disable(cybergear_handle_t motor, bool clear_fault);

/**
 * @brief Set the mode of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] mode The mode to set
 * @return
 *      - ESP_OK: Set mode successfully
 *      - ESP_ERR_INVALID_ARG: Set mode failed because of invalid argument
 *      - ESP_FAIL: Set mode failed because of other error
 */
esp_err_t cybergear_set_mode(cybergear_handle_t motor, cybergear_mode_t mode);

/**
 * @brief Set the location of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] location The location to set
 * @return
 *      - ESP_OK: Set location successfully
 *      - ESP_ERR_INVALID_ARG: Set location failed because of invalid argument
 *      - ESP_FAIL: Set location failed because of other error
 */
esp_err_t cybergear_set_location(cybergear_handle_t motor, float location);

/**
 * @brief Set the speed of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] speed The speed to set
 * @return
 *      - ESP_OK: Set speed successfully
 *      - ESP_ERR_INVALID_ARG: Set speed failed because of invalid argument
 *      - ESP_FAIL: Set speed failed because of other error
 */
esp_err_t cybergear_set_speed(cybergear_handle_t motor, float speed);

/**
 * @brief Set the speed limit of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] speed_limit The speed limit to set
 * @return
 *      - ESP_OK: Set speed limit successfully
 *      - ESP_ERR_INVALID_ARG: Set speed limit failed because of invalid argument
 *      - ESP_FAIL: Set speed limit failed because of other error
 */
esp_err_t cybergear_set_speed_limit(cybergear_handle_t motor, float speed_limit);

/**
 * @brief Set the current of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] current The current to set
 * @return
 *      - ESP_OK: Set current successfully
 *      - ESP_ERR_INVALID_ARG: Set current failed because of invalid argument
 *      - ESP_FAIL: Set current failed because of other error
 */
esp_err_t cybergear_set_current(cybergear_handle_t motor, float current);

/**
 * @brief Set the current limit of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] current The current to set
 * @return
 *      - ESP_OK: Set current successfully
 *      - ESP_ERR_INVALID_ARG: Set current failed because of invalid argument
 *      - ESP_FAIL: Set current failed because of other error
 */
esp_err_t cybergear_set_current_limit(cybergear_handle_t motor, float current_limit);

/**
 * @brief Set the zero location of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @return
 *      - ESP_OK: Set zero location successfully
 *      - ESP_ERR_INVALID_ARG: Set zero location failed because of invalid argument
 *      - ESP_FAIL: Set zero location failed because of other error
 */
esp_err_t cybergear_reset_zero_location(cybergear_handle_t motor);

/**
 * @brief Set the motor ID of the CyberGear motor
 *
 * @note The motor ID can not be changed when the motor is running, please disable the motor and set motor id.
 *
 * @param[in] motor The motor handle
 * @param[in] new_motor_id The new motor ID to set
 * @return
 *      - ESP_OK: Set motor ID successfully
 *      - ESP_ERR_INVALID_ARG: Set motor ID failed because of invalid argument
 *      - ESP_FAIL: Set motor ID failed because of other error
 */
esp_err_t cybergear_set_motor_id(cybergear_handle_t motor, uint8_t new_motor_id);

/**
 * @brief Get the motor ID of the CyberGear motor
 *
 * @note After calling this function, you need to power off the motor and power it on again. Then it will broadcast its motor ID.
 *       This API will block until the motor ID is broadcasted.
 *
 * @param[in] motor The motor handle
 * @param[out] ret_motor_id The motor ID to get
 * @return
 *      - ESP_OK: Get motor ID successfully
 *      - ESP_ERR_INVALID_ARG: Get motor ID failed because of invalid argument
 *      - ESP_FAIL: Get motor ID failed because of other error
 */
esp_err_t cybergear_get_motor_id(cybergear_handle_t motor, uint8_t *ret_motor_id);

/**
 * @brief Get the parameter of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] para_index The parameter index to get
 * @return
 *      - ESP_OK: Get parameter successfully
 *      - ESP_ERR_INVALID_ARG: Get parameter failed because of invalid argument
 *      - ESP_FAIL: Get parameter failed because of other error
 */
esp_err_t cybergear_get_parameter(cybergear_handle_t motor, uint16_t para_index);

/**
 * @brief Motion control the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[in] motion_control The motion control to set
 * @return
 *      - ESP_OK: Motion control successfully
 *      - ESP_ERR_INVALID_ARG: Motion control failed because of invalid argument
 *      - ESP_FAIL: Motion control failed because of other error
 */
esp_err_t cybergear_control_motion(cybergear_handle_t motor, const cybergear_motion_control_t *motion_control);

/**
 * @brief Get the status of the CyberGear motor
 *
 * @param[in] motor The motor handle
 * @param[out] status The status to get
 * @return
 *      - ESP_OK: Get status successfully
 *      - ESP_ERR_INVALID_ARG: Get status failed because of invalid argument
 *      - ESP_FAIL: Get status failed because of other error
 */
esp_err_t cybergear_get_status(cybergear_handle_t motor, cybergear_status_t *status);

#ifdef __cplusplus
}
#endif
