/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdint.h>

#include "soc/soc_caps.h"
#include "hal/clk_tree_ll.h"
#include "esp_clk_tree.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_check.h"
#include "i2s_private.h"

#define TAG "i2s_apll"

_Static_assert(I2S_DIR_RX == BIT(0) && I2S_DIR_TX == BIT(1),
               "i2s_dir_t values must remain usable as APLL user-mask bits");

static bool i2s_apll_candidate_in_range(uint64_t candidate, uint64_t low, uint64_t high_exclusive)
{
    return candidate >= low && candidate < high_exclusive && candidate <= UINT32_MAX;
}

static bool i2s_apll_divider_is_valid(uint32_t source_hz, uint32_t mclk_hz)
{
    /* Keep the lower-bound tolerance consistent with the mode clock checks.
     * The realized APLL frequency can be a few Hz below the ideal 2 * MCLK target.
     * A ratio below 2 only becomes the integer divider 2 when its fractional part reaches
     * the carry threshold of hal_utils_calc_clk_div_frac_accurate(); otherwise the integer
     * part becomes 1, below the minimum divider of 2. */
    uint64_t carry_threshold_hz = (uint64_t)mclk_hz * 2 -
                                  (uint64_t)(mclk_hz / (I2S_LL_CLK_FRAC_DIV_AB_MAX - 1)) * 2;
    return mclk_hz &&
           (uint64_t)source_hz * 100 > (uint64_t)mclk_hz * 199 &&
           (uint64_t)source_hz >= carry_threshold_hz &&
           (uint64_t)source_hz < (uint64_t)mclk_hz * I2S_LL_CLK_FRAC_DIV_N_MAX;
}

/* Select the lowest APLL frequency that keeps integer division for the deciding user
 * and stays within the divider range of the other user, if any. */
static esp_err_t i2s_apll_plan_minimal(uint32_t principal_mclk, uint32_t other_mclk, uint32_t *target_hz)
{
    uint64_t low = MAX((uint64_t)CLK_LL_APLL_MIN_HZ, (uint64_t)principal_mclk * 2);
    uint64_t high_exclusive = MIN((uint64_t)CLK_LL_APLL_MAX_HZ + 1,
                                  (uint64_t)principal_mclk * I2S_LL_CLK_FRAC_DIV_N_MAX);
    if (other_mclk) {
        low = MAX(low, (uint64_t)other_mclk * 2);
        high_exclusive = MIN(high_exclusive, (uint64_t)other_mclk * I2S_LL_CLK_FRAC_DIV_N_MAX);
    }
    uint64_t candidate = ((low + principal_mclk - 1) / principal_mclk) * principal_mclk;
    ESP_RETURN_ON_FALSE(i2s_apll_candidate_in_range(candidate, low, high_exclusive), ESP_ERR_INVALID_ARG, TAG,
                        "no adjustable APLL frequency allows integer division for MCLK %"PRIu32" Hz "
                        "with the paired MCLK %"PRIu32" Hz", principal_mclk, other_mclk);
    *target_hz = (uint32_t)candidate;
    return ESP_OK;
}

/* The internal master decides the shared frequency. Without one, the earliest remaining user decides it. */
static bool i2s_apll_is_principal(i2s_chan_handle_t handle, i2s_chan_handle_t peer)
{
    if (handle->role != peer->role) {
        return handle->role == I2S_ROLE_MASTER;
    }
    return handle->controller->apll.first_user == (uint8_t)handle->dir;
}

/* Scale the shared APLL with the tuned MCLK so the deciding user keeps its current integer divider. */
static bool i2s_apll_try_keep_divider(i2s_chan_handle_t handle, uint32_t mclk_freq_hz, uint32_t other_mclk,
                                      uint32_t *target_hz)
{
    uint32_t curr_target_hz = handle->controller->apll.target_hz;
    uint32_t curr_mclk_hz = handle->curr_mclk_hz;
    if (!curr_target_hz || !curr_mclk_hz) {
        return false;
    }
    uint32_t divider = (curr_target_hz + curr_mclk_hz / 2) / curr_mclk_hz;
    uint64_t candidate = (uint64_t)divider * mclk_freq_hz;
    if (divider < 2 || divider >= I2S_LL_CLK_FRAC_DIV_N_MAX ||
            candidate < CLK_LL_APLL_MIN_HZ || candidate > CLK_LL_APLL_MAX_HZ ||
            (other_mclk && !i2s_apll_divider_is_valid((uint32_t)candidate, other_mclk))) {
        return false;
    }
    *target_hz = (uint32_t)candidate;
    return true;
}

/* Select the ideal shared APLL frequency for a logical user request.
 * The other user only changes APLL when its divider can't fit the current frequency. */
static esp_err_t i2s_apll_select_target(i2s_chan_handle_t handle, uint32_t mclk_freq_hz,
                                        const i2s_apll_operation_t *operation, uint32_t *target_hz)
{
    const i2s_apll_state_t *apll = &handle->controller->apll;
    uint8_t user_bit = (uint8_t)handle->dir;
    bool existing_user = (apll->user_mask & user_bit) != 0;
    uint32_t peer_mclk = 0;

    if (apll->user_mask & ~user_bit) {
        i2s_chan_handle_t peer = i2s_channel_get_peer(handle);
        peer_mclk = peer ? peer->target_mclk_hz : 0;
        ESP_RETURN_ON_FALSE(peer_mclk, ESP_ERR_INVALID_STATE, TAG, "paired adjustable APLL target MCLK is unavailable");
        if (!i2s_apll_is_principal(handle, peer)) {
            if (i2s_apll_divider_is_valid(apll->actual_hz, mclk_freq_hz)) {
                *target_hz = apll->target_hz;
                return ESP_OK;
            }
            return i2s_apll_plan_minimal(peer_mclk, mclk_freq_hz, target_hz);
        }
        if (!existing_user &&
                i2s_apll_divider_is_valid(apll->actual_hz, mclk_freq_hz) &&
                i2s_apll_divider_is_valid(apll->actual_hz, peer_mclk) &&
                apll->target_hz % mclk_freq_hz == 0) {
            *target_hz = apll->target_hz;
            return ESP_OK;
        }
    }
    if (existing_user && operation->keep_divider &&
            i2s_apll_try_keep_divider(handle, mclk_freq_hz, peer_mclk, target_hz)) {
        return ESP_OK;
    }
    return i2s_apll_plan_minimal(mclk_freq_hz, peer_mclk, target_hz);
}

/* A running or stopping user still uses the shared APLL, so the APLL frequency can't change */
static bool i2s_apll_user_is_active(i2s_chan_handle_t handle)
{
    return handle->state == I2S_CHAN_STATE_RUNNING || handle->state == I2S_CHAN_STATE_STOPPING;
}

/* Refresh one existing logical user's divider after the shared APLL changes.
 * The caller must validate the source/MCLK divider range before updating hardware. */
static esp_err_t i2s_apll_refresh_adjustable_user(i2s_chan_handle_t handle, uint32_t source_hz)
{
    hal_utils_clk_div_t mclk_div = {};
    i2s_hal_calc_mclk_precise_division(source_hz, handle->target_mclk_hz, &mclk_div);
    uint64_t divider = (uint64_t)mclk_div.integer * mclk_div.denominator + mclk_div.numerator;
    ESP_RETURN_ON_FALSE(divider && mclk_div.denominator, ESP_ERR_INVALID_ARG, TAG,
                        "invalid adjustable APLL divider");

    PERIPH_RCC_ATOMIC() {
        if (handle->dir == I2S_DIR_TX) {
            i2s_ll_tx_set_mclk(handle->controller->hal.dev, &mclk_div);
#if SOC_I2S_HW_VERSION_2
            i2s_ll_tx_update(handle->controller->hal.dev);
#endif
        } else {
            i2s_ll_rx_set_mclk(handle->controller->hal.dev, &mclk_div);
#if SOC_I2S_HW_VERSION_2
            i2s_ll_rx_update(handle->controller->hal.dev);
#endif
        }
    }
    handle->sclk_hz = source_hz;
    handle->origin_mclk_hz = (uint32_t)(((uint64_t)source_hz * mclk_div.denominator) / divider);
    handle->curr_mclk_hz = handle->origin_mclk_hz;
    return ESP_OK;
}

bool i2s_apll_is_controller_managed(i2s_chan_handle_t handle)
{
    return handle->mode == I2S_COMM_MODE_STD
#if SOC_I2S_SUPPORTS_TDM
           || handle->mode == I2S_COMM_MODE_TDM
#endif
           ;
}

/* Acquire the single controller-owned adjustable APLL reference before frequency planning. */
esp_err_t i2s_apll_controller_operation_begin(i2s_chan_handle_t handle, i2s_apll_operation_t *operation)
{
    ESP_RETURN_ON_FALSE(handle && operation, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    i2s_controller_t *controller = handle->controller;
    i2s_apll_state_t *apll = &controller->apll;
    uint8_t user_bit = (uint8_t)handle->dir;
    bool existing_user = (apll->user_mask & user_bit) != 0;

    *operation = (i2s_apll_operation_t) {};
    operation->mode = I2S_APLL_OP_CONTROLLER_ADJUSTABLE;

    if (apll->user_mask && !existing_user) {
        ESP_RETURN_ON_FALSE(controller->full_duplex, ESP_ERR_INVALID_STATE, TAG,
                            "adjustable APLL is already owned by an unrelated simplex channel");
    }

    if (!apll->acquired) {
        esp_clk_tree_src_config_t resource_config = {
            .flags.adjustable = true,
        };
        ESP_RETURN_ON_ERROR(esp_clk_tree_acquire_src(SOC_MOD_CLK_APLL, &resource_config),
                            TAG, "adjustable APLL acquire failed");
        apll->acquired = true;
        apll->valid_user_mask = 0;
        operation->acquired_in_transaction = true;
    }
    return ESP_OK;
}

/* Join/retarget controller-managed adjustable APLL and keep all logical users coherent. */
static esp_err_t i2s_apll_resolve_adjustable_freq(i2s_chan_handle_t handle, uint32_t mclk_freq_hz,
                                                  i2s_apll_operation_t *operation,
                                                  uint32_t *real_freq_hz)
{
    ESP_RETURN_ON_FALSE(operation->mode == I2S_APLL_OP_CONTROLLER_ADJUSTABLE,
                        ESP_ERR_INVALID_STATE, TAG, "adjustable APLL operation was not prepared");
    esp_err_t ret = ESP_OK;
    i2s_controller_t *controller = handle->controller;
    i2s_apll_state_t *apll = &controller->apll;
    uint8_t user_bit = (uint8_t)handle->dir;

    if (operation->acquired_in_transaction) {
        esp_clk_tree_apll_info_t info = {};
        ESP_RETURN_ON_ERROR(esp_clk_tree_apll_get_info(&info), TAG, "failed to query acquired APLL");
        if (info.config_valid) {
            operation->target_hz = info.frequency_hz;
            operation->frequency_hz = info.frequency_hz;
            ESP_RETURN_ON_FALSE(i2s_apll_divider_is_valid(info.frequency_hz, mclk_freq_hz),
                                ESP_ERR_INVALID_ARG, TAG,
                                "preconfigured APLL frequency can't provide the requested MCLK");
            *real_freq_hz = info.frequency_hz;
            return ESP_OK;
        }
    }

    uint32_t target_hz = 0;
    ESP_RETURN_ON_ERROR(i2s_apll_select_target(handle, mclk_freq_hz, operation, &target_hz),
                        TAG, "adjustable APLL planning failed");
    uint8_t other_users = apll->user_mask & ~user_bit;
    if (other_users && target_hz != apll->target_hz && i2s_apll_user_is_active(i2s_channel_get_peer(handle))) {
        /* Keep the current APLL for the running peer if it can still provide the requested MCLK */
        ESP_RETURN_ON_FALSE(i2s_apll_divider_is_valid(apll->actual_hz, mclk_freq_hz), ESP_ERR_INVALID_STATE, TAG,
                            "can't retarget adjustable APLL while the paired channel is running or stopping");
        target_hz = apll->target_hz;
        if (target_hz % mclk_freq_hz) {
            ESP_LOGW(TAG, "%s MCLK %"PRIu32" Hz can't be divided from APLL %"PRIu32" Hz by an integer while "
                     "the paired channel is running, which may increase clock jitter",
                     handle->dir == I2S_DIR_TX ? "TX" : "RX", mclk_freq_hz, apll->actual_hz);
        }
    }
    operation->target_hz = target_hz;

    uint32_t old_actual_hz = apll->actual_hz;
    uint32_t actual_hz = old_actual_hz;
    if (!actual_hz || target_hz != apll->target_hz) {
        ESP_RETURN_ON_ERROR(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, target_hz, &actual_hz),
                            TAG, "adjustable APLL frequency update failed");
    }
    operation->frequency_changed = old_actual_hz && actual_hz != old_actual_hz;
    operation->frequency_hz = actual_hz;

    uint8_t prospective_users = apll->user_mask | user_bit;
    if (prospective_users & I2S_DIR_TX) {
        uint32_t tx_mclk = handle->dir == I2S_DIR_TX ? mclk_freq_hz :
                           controller->tx_chan->target_mclk_hz;
        ESP_GOTO_ON_FALSE(i2s_apll_divider_is_valid(actual_hz, tx_mclk), ESP_ERR_INVALID_ARG,
                          invalid, TAG, "TX divider is invalid at actual APLL frequency %"PRIu32" Hz", actual_hz);
    }
    if (prospective_users & I2S_DIR_RX) {
        uint32_t rx_mclk = handle->dir == I2S_DIR_RX ? mclk_freq_hz :
                           controller->rx_chan->target_mclk_hz;
        ESP_GOTO_ON_FALSE(i2s_apll_divider_is_valid(actual_hz, rx_mclk), ESP_ERR_INVALID_ARG,
                          invalid, TAG, "RX divider is invalid at actual APLL frequency %"PRIu32" Hz", actual_hz);
    }

    /* APLL frequency changed */
    if (actual_hz != old_actual_hz) {
        apll->target_hz = target_hz;
        apll->actual_hz = actual_hz;
        apll->valid_user_mask = 0;
        if ((apll->user_mask & I2S_DIR_TX) && handle->dir != I2S_DIR_TX) {
            ESP_GOTO_ON_ERROR(i2s_apll_refresh_adjustable_user(controller->tx_chan, actual_hz),
                              invalid, TAG, "failed to refresh TX after APLL update");
            apll->valid_user_mask |= I2S_DIR_TX;
        }
        if ((apll->user_mask & I2S_DIR_RX) && handle->dir != I2S_DIR_RX) {
            ESP_GOTO_ON_ERROR(i2s_apll_refresh_adjustable_user(controller->rx_chan, actual_hz),
                              invalid, TAG, "failed to refresh RX after APLL update");
            apll->valid_user_mask |= I2S_DIR_RX;
        }
    }

    *real_freq_hz = actual_hz;
    return ESP_OK;

invalid:
    if (actual_hz != old_actual_hz) {
        apll->target_hz = target_hz;
        apll->actual_hz = actual_hz;
        apll->valid_user_mask = 0;
    }
    return ret;
}

/* Resolve the usable fixed or per-channel adjustable APLL frequency. */
static esp_err_t i2s_apll_resolve_fixed_freq(i2s_chan_handle_t handle, uint32_t mclk_freq_hz,
                                             bool clk_src_adjustable, i2s_apll_operation_t *operation,
                                             uint32_t *real_freq_hz)
{
    uint32_t mclk_div = CLK_LL_APLL_MIN_HZ / mclk_freq_hz + 1;
    /* apll_freq = mclk * div
     * The div should be at least 2 according to the hardware specification */
    mclk_div = mclk_div < 2 ? 2 : mclk_div;
    uint64_t expected_freq_hz = (uint64_t)mclk_freq_hz * mclk_div;
    ESP_RETURN_ON_FALSE(expected_freq_hz <= CLK_LL_APLL_MAX_HZ, ESP_ERR_INVALID_ARG, TAG,
                        "required APLL frequency exceeds its maximum value");
    operation->mode = clk_src_adjustable ? I2S_APLL_OP_CHANNEL_ADJUSTABLE :
                      I2S_APLL_OP_CHANNEL_FIXED;

    bool apll_in_use = handle->state != I2S_CHAN_STATE_REGISTER &&
                       handle->clk_src == I2S_CLK_SRC_APLL;
    if (apll_in_use) {
        ESP_RETURN_ON_FALSE(handle->apll_adjustable == clk_src_adjustable,
                            ESP_ERR_INVALID_STATE, TAG, "can't change active APLL access mode");
        if (!handle->apll_adjustable) {
            *real_freq_hz = handle->sclk_hz;
            operation->frequency_hz = *real_freq_hz;
            return ESP_OK;
        }
    } else {
        esp_clk_tree_apll_info_t apll_info = {};
        ESP_RETURN_ON_ERROR(esp_clk_tree_apll_get_info(&apll_info), TAG, "failed to get APLL information");
        if (apll_info.config_valid) {
            *real_freq_hz = apll_info.frequency_hz;
            operation->frequency_hz = *real_freq_hz;
            return ESP_OK;
        }
    }

    esp_err_t ret = esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, expected_freq_hz, real_freq_hz);
    if (ret == ESP_OK && apll_in_use && *real_freq_hz != handle->sclk_hz) {
        operation->frequency_changed = true;
    }
    if (ret == ESP_OK) {
        operation->frequency_hz = *real_freq_hz;
    }
    ESP_LOGD(TAG, "APLL expected frequency is %"PRIu64" Hz, real frequency is %"PRIu32" Hz",
             expected_freq_hz, *real_freq_hz);
    return ret;
}

esp_err_t i2s_apll_resolve_freq(i2s_chan_handle_t handle, bool clk_src_adjustable,
                                uint32_t mclk_freq_hz, i2s_apll_operation_t *operation,
                                uint32_t *real_freq_hz)
{
    ESP_RETURN_ON_FALSE(mclk_freq_hz, ESP_ERR_INVALID_ARG, TAG, "invalid MCLK frequency");

    if (clk_src_adjustable && i2s_apll_is_controller_managed(handle)) {
        return i2s_apll_resolve_adjustable_freq(handle, mclk_freq_hz, operation, real_freq_hz);
    }
    return i2s_apll_resolve_fixed_freq(handle, mclk_freq_hz, clk_src_adjustable, operation, real_freq_hz);
}

void i2s_apll_operation_commit(i2s_chan_handle_t handle, i2s_apll_operation_t *operation)
{
    switch (operation->mode) {
    case I2S_APLL_OP_CONTROLLER_ADJUSTABLE: {
        i2s_apll_state_t *apll = &handle->controller->apll;
        if (!apll->user_mask) {
            apll->first_user = (uint8_t)handle->dir;
        }
        apll->user_mask |= (uint8_t)handle->dir;
        apll->valid_user_mask |= (uint8_t)handle->dir;
        apll->target_hz = operation->target_hz;
        apll->actual_hz = operation->frequency_hz;
        handle->apll_adjustable = true;
        handle->sclk_hz = operation->frequency_hz;
        break;
    }
    case I2S_APLL_OP_CHANNEL_FIXED:
    case I2S_APLL_OP_CHANNEL_ADJUSTABLE:
        handle->apll_adjustable = operation->mode == I2S_APLL_OP_CHANNEL_ADJUSTABLE;
        handle->sclk_hz = operation->frequency_hz;
        break;
    case I2S_APLL_OP_NONE:
        break;
    }
    *operation = (i2s_apll_operation_t) {};
}

void i2s_apll_operation_abort(i2s_chan_handle_t handle, i2s_apll_operation_t *operation)
{
    switch (operation->mode) {
    case I2S_APLL_OP_CONTROLLER_ADJUSTABLE: {
        i2s_apll_state_t *apll = &handle->controller->apll;
        if (operation->acquired_in_transaction && apll->user_mask == 0) {
            /* Release an uncommitted first acquisition after downstream configuration fails. */
            esp_err_t release_ret = esp_clk_tree_release_src(SOC_MOD_CLK_APLL);
            if (release_ret == ESP_OK) {
                *apll = (i2s_apll_state_t) {};
            } else {
                ESP_LOGE(TAG, "failed to release adjustable APLL after configuration failure");
            }
        } else if (operation->frequency_changed) {
            /* The hardware frequency cannot be rolled back safely. Refreshed peers remain valid,
             * while the failed channel stays excluded from valid_user_mask until reconfigured. */
            ESP_LOGE(TAG, "APLL changed to %"PRIu32" Hz but channel configuration failed; "
                     "reconfigure this channel before enable", operation->frequency_hz);
        }
        break;
    }
    case I2S_APLL_OP_CHANNEL_FIXED:
    case I2S_APLL_OP_CHANNEL_ADJUSTABLE:
        if (operation->frequency_changed) {
            /* Keep per-channel metadata aligned with a frequency that cannot be rolled back safely. */
            handle->apll_adjustable = operation->mode == I2S_APLL_OP_CHANNEL_ADJUSTABLE;
            handle->sclk_hz = operation->frequency_hz;
            ESP_LOGW(TAG, "APLL frequency changed to %"PRIu32" Hz, but I2S clock configuration failed; "
                     "reconfigure the I2S clock", operation->frequency_hz);
        }
        break;
    case I2S_APLL_OP_NONE:
        break;
    }
    *operation = (i2s_apll_operation_t) {};
}

esp_err_t i2s_apll_remove_logical_user(i2s_chan_handle_t handle)
{
    i2s_apll_state_t *apll = &handle->controller->apll;
    uint8_t user_bit = (uint8_t)handle->dir;
    if (!(apll->user_mask & user_bit)) {
        return ESP_OK;
    }

    if (apll->user_mask == user_bit) {
        if (apll->acquired) {
            ESP_RETURN_ON_ERROR(esp_clk_tree_release_src(SOC_MOD_CLK_APLL), TAG,
                                "failed to release adjustable APLL");
        }
        *apll = (i2s_apll_state_t) {};
        handle->apll_adjustable = false;
        return ESP_OK;
    }
    apll->user_mask &= ~user_bit;
    apll->valid_user_mask &= ~user_bit;
    apll->first_user = apll->user_mask;
    handle->apll_adjustable = false;
    return ESP_OK;
}

esp_err_t i2s_apll_release_controller(i2s_controller_t *controller)
{
    if (!controller->apll.acquired) {
        return ESP_OK;
    }
    esp_err_t ret = esp_clk_tree_release_src(SOC_MOD_CLK_APLL);
    if (ret != ESP_OK) {
        return ret;
    }
    controller->apll = (i2s_apll_state_t) {};
    return ESP_OK;
}

esp_err_t i2s_apll_check_enable(i2s_chan_handle_t handle)
{
    i2s_apll_state_t *apll = &handle->controller->apll;
    ESP_RETURN_ON_FALSE(!(apll->user_mask & (uint8_t)handle->dir) ||
                        (apll->valid_user_mask & (uint8_t)handle->dir),
                        ESP_ERR_INVALID_STATE, TAG,
                        "adjustable APLL hardware state is invalid; reconfigure the channel before enable");
    return ESP_OK;
}
