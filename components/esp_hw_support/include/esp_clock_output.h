/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX || SOC_GPIO_CLOCKOUT_BY_IO_MUX
typedef struct esp_clock_output_mapping *esp_clock_output_mapping_handle_t;

/**
 * @brief Start output specified clock signal to specified GPIO, will also
 *        initialize the clkout_mapping_ret_hdl.
 *
 * @param[in]   clk_src  The clock signal source to be mapped to GPIOs
 * @param[in]   gpio_num GPIO number to be mapped soc_root_clk signal source
 * @param[out]  clkout_mapping_ret_hdl Clock output controll handler
 * @return
 *      - ESP_OK: Output specified clock signal to specified GPIO successfully
 *      - ESP_ERR_INVALID_ARG: Specified GPIO not supported to output internal clock
 *                             or specified GPIO is already mapped to other internal clock source.
  *     - ESP_FAIL: There are no clock out signals that can be allocated.
 */
esp_err_t esp_clock_output_start(soc_clkout_sig_id_t clk_sig, gpio_num_t gpio_num, esp_clock_output_mapping_handle_t *clkout_mapping_ret_hdl);

/**
 * @brief Stop clock signal to GPIO outputting
 * @param[in]  clkout_mapping_hdl Clock output mapping controll handle
 * @return
 *     - ESP_OK: Disable the clock output on GPIO successfully
 *     - ESP_ERR_INVALID_STATE  The clock in handle is already in the disabled state
 */
esp_err_t esp_clock_output_stop(esp_clock_output_mapping_handle_t clkout_mapping_hdl);
#endif // SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX || SOC_GPIO_CLOCKOUT_BY_IO_MUX

#ifdef __cplusplus
}
#endif
