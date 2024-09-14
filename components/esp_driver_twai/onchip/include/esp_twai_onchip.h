/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_twai_types.h"
#include "hal/gpio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TWAI on-chip node initialization configuration structure
 */
typedef struct {
    struct {
        gpio_num_t tx;                      /**< GPIO pin for twai TX */
        gpio_num_t rx;                      /**< GPIO pin for twai RX */
        gpio_num_t quanta_clk_out;          /**< GPIO pin for quanta clock output, Set -1 to not use */
        gpio_num_t bus_off_indicator;       /**< GPIO pin for bus-off indicator, Set -1 to not use */
    } io_cfg;                               /**< I/O configuration */
    twai_clock_source_t clk_src;            /**< Optional, clock source, remain 0 to using TWAI_CLK_SRC_DEFAULT by default */
    twai_timing_basic_config_t bit_timing; /**< Timing configuration for classic twai and FD arbitration stage */
    twai_timing_basic_config_t data_timing;/**< Optional, timing configuration for FD data stage */
    int8_t fail_retry_cnt;                  /**< Hardware retry limit if failed, range [-1:15], -1 for re-trans forever */
    uint32_t tx_queue_depth;                /**< Depth of the transmit queue */
    int intr_priority;                      /**< Interrupt priority, [0:3] */
    struct {
        uint32_t enable_self_test: 1;       /**< Transmission does not require acknowledgment. Use this mode for self testing */
        uint32_t enable_loopback: 1;        /**< The TWAI controller receive back frames what it send out */
        uint32_t enable_listen_only: 1;     /**< The TWAI controller will not influence the bus (No transmissions or acknowledgments) but can receive messages */
        uint32_t no_receive_rtr: 1;         /**< Don't receive remote frames */
    } flags;
} twai_onchip_node_config_t;

/**
 * @brief Allocate a TWAI hardware node by specific init config structure
 *        To delete/free the TWAI, call `twai_node_delete()`
 *
 * @param[in]  node_config   Init config structure
 * @param[out] node_ret      Return driver handle
 *
 * @return  ESP_OK                  Allocate success
 *          ESP_ERR_NO_MEM          No enough free memory
 *          ESP_ERR_NOT_FOUND       No free hardware controller
 *          ESP_ERR_INVALID_ARG     Config argument not available
 *          ESP_ERR_INVALID_STATE   State error, including hardware state error and driver state error
 *          ESP_FAIL                Other reasons
 */
esp_err_t twai_new_node_onchip(const twai_onchip_node_config_t *node_config, twai_node_handle_t *node_ret);

#ifdef __cplusplus
}
#endif
