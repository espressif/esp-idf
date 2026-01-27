/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "esp_twai.h"
#include "esp_private/twai_interface.h"
#include "esp_private/twai_utils.h"
#include "twai_private.h"

/**
 * @brief Calculate twai timing param by giving bitrate and hardware limit.
 *
 * +---------------------------------------------------+
 * |         bit time in time quanta (total_tq)        |
 * +--------------+----------+------------+------------+
 * |   sync_seg   | prop_seg | phase_seg1 | phase_seg2 |
 * +--------------+----------+------------+------------+
 * |      1       |        tseg1          |   tseg2    |
 * +--------------+----------+------------+------------+
 *                | tseg2/2  ^            ^
 *                          sjw      sample_point
 */
uint32_t twai_node_timing_calc_param(const uint32_t source_freq, const twai_timing_basic_config_t *in_param, const twai_timing_constraint_t *hw_limit, twai_timing_advanced_config_t *out_param)
{
    uint32_t total_div = (source_freq + in_param->bitrate / 2) / in_param->bitrate;
    uint32_t pre_div = hw_limit->brp_min;
    uint16_t tseg = 0;
    for (; pre_div <= hw_limit->brp_max; pre_div ++) {
        tseg = total_div / pre_div;
        if (total_div != tseg * pre_div) {
            continue;   // no integer tseg
        }
        if ((tseg <= (hw_limit->tseg1_max + hw_limit->tseg2_max + 1)) && (tseg >= (hw_limit->tseg1_min + hw_limit->tseg2_min))) {
            break;
        }
    }
    if (pre_div > hw_limit->brp_max) {  // no valid pre_div
        return 0;
    }

    uint16_t default_point = (in_param->bitrate >= 800000) ? 750 : ((in_param->bitrate >= 500000) ? 800 : 875);
    uint16_t sample_point = in_param->sp_permill ? in_param->sp_permill : default_point;  // default sample point based on bitrate if not configured
    uint16_t tseg_1 = (tseg * sample_point) / 1000 - 1;
    tseg_1 = MAX(hw_limit->tseg1_min, MIN(tseg_1, hw_limit->tseg1_max));
    uint16_t tseg_2 = tseg - tseg_1 - 1;
    tseg_2 = MAX(hw_limit->tseg2_min, MIN(tseg_2, hw_limit->tseg2_max));
    uint16_t prop = MAX(1, tseg_1 / 4); // prop_seg is usually shorter than tseg_1 and at least 1
    tseg_1 -= prop;

    out_param->brp = pre_div;
    out_param->prop_seg = prop;
    out_param->tseg_1 = tseg_1;
    out_param->tseg_2 = tseg_2;
    out_param->sjw = MAX(1, MIN(tseg_2 >> 1, hw_limit->sjw_max));
    out_param->ssp_offset = (tseg * in_param->ssp_permill) / 1000;  // ssp is optional, default 0 if not configured

    return source_freq / (pre_div * (prop + tseg_1 + tseg_2 + 1));
}

esp_err_t twai_node_enable(twai_node_handle_t node)
{
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null handle");
    ESP_RETURN_ON_FALSE(node->enable, ESP_ERR_NOT_SUPPORTED, TAG, "enable is not supported");

    return node->enable(node);
}

esp_err_t twai_node_disable(twai_node_handle_t node)
{
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null handle");
    ESP_RETURN_ON_FALSE(node->disable, ESP_ERR_NOT_SUPPORTED, TAG, "disable is not supported");

    return node->disable(node);
}

esp_err_t twai_node_delete(twai_node_handle_t node)
{
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null handle");
    ESP_RETURN_ON_FALSE(node->del, ESP_ERR_NOT_SUPPORTED, TAG, "delete is not supported");

    return node->del(node);
}

esp_err_t twai_node_config_mask_filter(twai_node_handle_t node, uint8_t filter_id, const twai_mask_filter_config_t *mask_cfg)
{
    ESP_RETURN_ON_FALSE(node && mask_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null");
    ESP_RETURN_ON_FALSE(node->config_mask_filter, ESP_ERR_NOT_SUPPORTED, TAG, "config_mask_filter is not supported");

    return node->config_mask_filter(node, filter_id, mask_cfg);
}

esp_err_t twai_node_config_range_filter(twai_node_handle_t node, uint8_t filter_id, const twai_range_filter_config_t *range_cfg)
{
    ESP_RETURN_ON_FALSE(node && range_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null");
    ESP_RETURN_ON_FALSE(node->config_range_filter, ESP_ERR_NOT_SUPPORTED, TAG, "config_range_filter is not supported");

    return node->config_range_filter(node, filter_id, range_cfg);
}

esp_err_t twai_node_reconfig_timing(twai_node_handle_t node, const twai_timing_advanced_config_t *bit_timing, const twai_timing_advanced_config_t *data_timing)
{
    ESP_RETURN_ON_FALSE(node && (bit_timing || data_timing), ESP_ERR_INVALID_ARG, TAG, "invalid argument: null");
    ESP_RETURN_ON_FALSE(node->reconfig_timing, ESP_ERR_NOT_SUPPORTED, TAG, "reconfig_timing is not supported");

    return node->reconfig_timing(node, bit_timing, data_timing);
}

esp_err_t twai_node_register_event_callbacks(twai_node_handle_t node, const twai_event_callbacks_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(node && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null");
    ESP_RETURN_ON_FALSE(node->register_cbs, ESP_ERR_NOT_SUPPORTED, TAG, "register_cbs is not supported");

    return node->register_cbs(node, cbs, user_data);
}

esp_err_t twai_node_recover(twai_node_handle_t node)
{
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null handle");
    ESP_RETURN_ON_FALSE(node->recover, ESP_ERR_NOT_SUPPORTED, TAG, "recover is not supported");

    return node->recover(node);
}

esp_err_t twai_node_get_info(twai_node_handle_t node, twai_node_status_t *status_ret, twai_node_record_t *statistics_ret)
{
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null handle");
    ESP_RETURN_ON_FALSE(node->get_info, ESP_ERR_NOT_SUPPORTED, TAG, "get_info is not supported");

    return node->get_info(node, status_ret, statistics_ret);
}

esp_err_t twai_node_transmit(twai_node_handle_t node, const twai_frame_t *frame, int timeout_ms)
{
    ESP_RETURN_ON_FALSE_ISR(node && frame, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null");
    ESP_RETURN_ON_FALSE_ISR(node->transmit, ESP_ERR_NOT_SUPPORTED, TAG, "transmit is not supported");

    return node->transmit(node, frame, timeout_ms);
}

esp_err_t twai_node_transmit_wait_all_done(twai_node_handle_t node, int timeout_ms)
{
    ESP_RETURN_ON_FALSE(node, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null handle");
    ESP_RETURN_ON_FALSE(node->transmit_wait_done, ESP_ERR_NOT_SUPPORTED, TAG, "transmit_wait_done is not supported");

    return node->transmit_wait_done(node, timeout_ms);
}

esp_err_t twai_node_receive_from_isr(twai_node_handle_t node, twai_frame_t *rx_frame)
{
    ESP_RETURN_ON_FALSE_ISR(node && rx_frame, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null");
    ESP_RETURN_ON_FALSE_ISR((rx_frame->buffer_len == 0) || esp_ptr_in_dram(rx_frame->buffer) || esp_ptr_external_ram(rx_frame->buffer), ESP_ERR_INVALID_ARG, TAG, "invalid 'rx_frame->buffer' pointer or buffer_len");
    ESP_RETURN_ON_FALSE_ISR(node->receive_isr, ESP_ERR_NOT_SUPPORTED, TAG, "receive is not supported");

    return node->receive_isr(node, rx_frame);
}

#if CONFIG_TWAI_ENABLE_DEBUG_LOG
__attribute__((constructor))
static void twai_override_default_log_level(void)
{
    esp_log_level_set(TAG, ESP_LOG_VERBOSE);
}
#endif
