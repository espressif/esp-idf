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
    twai_timing_basic_config_t bit_timing;  /**< Timing configuration for classic twai and FD arbitration stage */
    twai_timing_basic_config_t data_timing; /**< Optional, timing configuration for FD data stage */
    uint32_t timestamp_resolution_hz;       /**< Timebase frequency (in Hz), used for recording the timestamp of RX frame, set 0 to disable the timestamp feature */
    int8_t fail_retry_cnt;                  /**< Hardware retry limit if failed, range [-1:15], -1 for re-trans forever */
    uint32_t tx_queue_depth;                /**< Depth of the transmit queue */
    int intr_priority;                      /**< Interrupt priority, [0:3] */
    struct {
        uint32_t enable_self_test: 1;       /**< Transmission does not require acknowledgment. Use this mode for self testing */
        uint32_t enable_loopback: 1;        /**< The TWAI controller receives back frames that it sends out, but does not acknowledge them */
        uint32_t enable_listen_only: 1;     /**< No transmissions or acknowledgements. The controller only monitors the bus without participating */
        uint32_t no_receive_rtr: 1;         /**< Don't receive remote frames */
    } flags;                                /**< Misc configuration flags */
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

/**
 * @brief Helper function to configure a dual 16-bit acceptance filter.
 * @note  For 29bits Extended IDs, ONLY high 16bits id/mask is used for each filter.
 *
 * @param id1     First full 11/29 bits ID to filter.
 * @param mask1   Mask for first ID.
 * @param id2     Second full 11/29 bits ID to filter.
 * @param mask2   Mask for second ID.
 * @param is_ext  True if using Extended (29-bit) IDs, false for Standard (11-bit) IDs.
 * @return twai_mask_filter_config_t   A filled filter configuration structure for dual filtering.
 */
static inline twai_mask_filter_config_t twai_make_dual_filter(uint32_t id1, uint32_t mask1, uint32_t id2, uint32_t mask2, bool is_ext)
{
    /**
     * Dual filter is a special mode in hardware,
     * which allow split general 32bits filter register to 2 unit of 16 bits id/mask pairs then using as 2 filter
     * below code do bit shifting to reach hardware requirement in this mode, for detail please refer to `Dual Filter Mode` of TRM
     *
     *  31            16 15             0
     *  ▼              ▼ ▼              ▼
     *  xxxxxxxxxxxxxxxx xxxxxxxxxxxxxxxx
     *  ◄─11 bits─►      ◄─11 bits─►
     *  std filter 2     std filter 1
     *  ─────────────────────────────────
     *  ◄───16─bits────► ◄───16─bits────►
     *    ext filter 2     etx filter 1
     */
    twai_mask_filter_config_t dual_cfg = {
        .id = is_ext ? (((id1 & TWAI_EXT_ID_MASK) >> 13) << 16) | ((id2 & TWAI_EXT_ID_MASK) >> 13) : \
        ((id1 & TWAI_STD_ID_MASK) << 21) | ((id2 & TWAI_STD_ID_MASK) << 5),
        .mask = is_ext ? (((mask1 & TWAI_EXT_ID_MASK) >> 13) << 16) | ((mask2 & TWAI_EXT_ID_MASK) >> 13) : \
        ((mask1 & TWAI_STD_ID_MASK) << 21) | ((mask2 & TWAI_STD_ID_MASK) << 5),
        .is_ext = is_ext,
        .no_classic = false,
        .no_fd = false,
        .dual_filter = true,
    };
    if ((id1 & mask1 & id2 & mask2) == 0xffffffff) {
        dual_cfg.id = 0xffffffff;   // recover the 'disable' code
        dual_cfg.mask = 0xffffffff;
    }
    return dual_cfg;
}

#ifdef __cplusplus
}
#endif
