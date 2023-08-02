/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "driver/mcpwm_prelude.h"

/**
 * @brief   svpwm inverter config struct type
 */
typedef struct inverter_config {
    mcpwm_timer_config_t timer_config;          // pwm timer and timing config
    mcpwm_operator_config_t operator_config;    // mcpwm operator config
    mcpwm_comparator_config_t compare_config;   // mcpwm comparator config
    int gen_gpios[3][2];                        // 6 GPIO pins for generator config
    mcpwm_dead_time_config_t dt_config;         // dead time config for positive pwm output
    mcpwm_dead_time_config_t inv_dt_config;     // dead time config for negative pwm output
} inverter_config_t;

// inverter handler type
typedef struct mcpwm_svpwm_ctx  *inverter_handle_t;

/**
 * @brief Config mcpwm as a inverter with corresponding config value
 *
 * @param config        config value for mcpwm peripheral
 * @param ret_inverter  return handler for corresponding mcpwm
 *
 * @return  - ESP_OK: Create invertor successfully
 *          - ESP_ERR_INVALID_ARG: NULL arguments
 *          - ESP_ERR_NO_MEM: no free memory
 */
esp_err_t svpwm_new_inverter(const inverter_config_t *config, inverter_handle_t *ret_inverter);

/**
 * @brief register update callbacks for a mcpwm peripheral
 *
 * @param handle    svpwm invertor handler
 * @param event     callbacks config
 * @param user_ctx  pointer to user data to be passed to callbacks
 *
 * @return  - ESP_OK: register callbacks successfully
 *          - ESP_ERR_INVALID_ARG: NULL arguments
 */
esp_err_t svpwm_inverter_register_cbs(inverter_handle_t handle, const mcpwm_timer_event_callbacks_t *event, void *user_ctx);

/**
 * @brief start/stop a svpwm invertor
 *
 * @param handle    svpwm invertor handler
 * @param command   see "mcpwm_timer_start_stop_cmd_t"
 *
 * @return  - ESP_OK: start inverter successfully
 *          - ESP_ERR_INVALID_ARG: NULL arguments
 */
esp_err_t svpwm_inverter_start(inverter_handle_t handle, mcpwm_timer_start_stop_cmd_t command);

/**
 * @brief set 3 channels pwm comparator value for invertor
 *
 * @param handle  svpwm invertor handler
 * @param u comparator value for channel UH and UL
 * @param v comparator value for channel VH and VL
 * @param w comparator value for channel WH and WL
 *
 * @return  - ESP_OK: set compare value successfully
 *          - ESP_ERR_INVALID_ARG: NULL arguments
 */
esp_err_t svpwm_inverter_set_duty(inverter_handle_t handle, uint16_t u, uint16_t v, uint16_t w);

/**
 * @brief free a svpwm invertor
 *
 * @param handle  svpwm invertor handler
 *
 * @return  - ESP_OK: free inverter successfully
 *          - ESP_ERR_INVALID_ARG: NULL arguments
 */
esp_err_t svpwm_del_inverter(inverter_handle_t handle);
