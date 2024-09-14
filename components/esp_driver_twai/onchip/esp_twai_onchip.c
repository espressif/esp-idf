/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/twai_periph.h"
#include "soc/io_mux_reg.h"
#include "hal/twai_hal.h"
#include "freertos/FreeRTOS.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_heap_caps.h"
#include "esp_intr_alloc.h"
#include "esp_clk_tree.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "esp_private/twai_interface.h"
#include "esp_private/twai_priv.h"
#include "esp_private/gpio.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_private/periph_ctrl.h"

static const char *TAG = "twai";

#ifdef CONFIG_TWAI_ISR_INTO_IRAM
#define TWAI_MALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define TWAI_MALLOC_CAPS    MALLOC_CAP_DEFAULT
#endif  //CONFIG_TWAI_ISR_INTO_IRAM

#if !SOC_RCC_IS_INDEPENDENT
#define TWAI_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define TWAI_RCC_ATOMIC()
#endif

#if SOC_PERIPH_CLK_CTRL_SHARED
#define TWAI_PERI_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define TWAI_PERI_ATOMIC()
#endif

#define DRIVER_DEFAULT_INTERRUPTS   (TWAIFD_LL_INTR_TX_DONE |\
                                     TWAIFD_LL_INTR_TX_SUCCESS |\
                                     TWAIFD_LL_INTR_RX_NOT_EMPTY |\
                                     TWAIFD_LL_INTR_RX_FULL |\
                                     TWAIFD_LL_INTR_ERR_WARN |\
                                     TWAIFD_LL_INTR_BUS_ERR |\
                                     TWAIFD_LL_INTR_FSM_CHANGE |\
                                     TWAIFD_LL_INTR_ARBI_LOST |\
                                     TWAIFD_LL_INTR_DATA_OVERRUN)

static void _twai_rcc_clock_ctrl(uint8_t ctrlr_id, bool enable)
{
    TWAI_RCC_ATOMIC() {
        twaifd_ll_enable_bus_clock(ctrlr_id, enable);
        twaifd_ll_reset_register(ctrlr_id);
    }
    TWAI_PERI_ATOMIC() {
        twaifd_ll_enable_clock(ctrlr_id, enable);
    }
}

typedef struct {
    struct twai_node_base api_base;
    int ctrlr_id;
    uint64_t gpio_reserved;
    twai_hal_context_t hal;
    intr_handle_t intr_hdl;
    QueueHandle_t tx_mount_queue;
    twai_clock_source_t curr_clk_src;
    uint32_t valid_fd_timing;
    twai_event_callbacks_t cbs;
    void *user_data;
    esp_pm_lock_handle_t pm_lock;

    _Atomic twai_error_state_t state;
    uint16_t tx_error_count;
    uint16_t rx_error_count;
    twai_node_record_t history;

    atomic_bool hw_busy;
    atomic_bool rx_isr;
    const twai_frame_t *p_curr_tx;
    twai_hal_frame_t rcv_buff;
} twai_onchip_ctx_t;

typedef struct twai_platform_s {
    _lock_t mutex;
    twai_onchip_ctx_t *nodes[SOC_TWAI_CONTROLLER_NUM];
} twai_platform_t;
static twai_platform_t s_platform;

static int _ctrlr_acquire(twai_onchip_ctx_t *node)
{
    int ctrlr_id = -1;
    _lock_acquire(&s_platform.mutex);
    // Check if there is a controller available for use
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        if (s_platform.nodes[i] == NULL) {
            // Assign to node object to the controller slot
            s_platform.nodes[i] = node;
            ctrlr_id = i;
            break;
        }
    }
    _lock_release(&s_platform.mutex);

    // Return the controller index or -1
    return ctrlr_id;
}

static void _ctrlr_release(int ctrlr_id)
{
    _lock_acquire(&s_platform.mutex);
    assert(s_platform.nodes[ctrlr_id]);
    // Clear the node object from the controller slot
    s_platform.nodes[ctrlr_id] = NULL;
    _lock_release(&s_platform.mutex);
}

static esp_err_t _node_config_io(twai_onchip_ctx_t *node, const twai_onchip_node_config_t *node_config)
{
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(node_config->io_cfg.tx), ESP_ERR_INVALID_ARG, TAG, "Invalid tx gpio num");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(node_config->io_cfg.rx), ESP_ERR_INVALID_ARG, TAG, "Invalid rx gpio num");
    uint64_t reserve_mask = BIT64(node_config->io_cfg.tx);

    // Set RX pin
    gpio_input_enable(node_config->io_cfg.rx);
    gpio_set_pull_mode(node_config->io_cfg.rx, GPIO_PULLUP_ONLY);    // pullup to avoid noise if no connection to transceiver
    gpio_func_sel(node_config->io_cfg.rx, PIN_FUNC_GPIO);
    esp_rom_gpio_connect_in_signal(node_config->io_cfg.rx, twai_controller_periph_signals.controllers[node->ctrlr_id].rx_sig, false);

    // Set TX pin
    gpio_func_sel(node_config->io_cfg.tx, PIN_FUNC_GPIO);
    esp_rom_gpio_connect_out_signal(node_config->io_cfg.tx, twai_controller_periph_signals.controllers[node->ctrlr_id].tx_sig, false, false);

    //Configure output clock pin (Optional)
    if (GPIO_IS_VALID_OUTPUT_GPIO(node_config->io_cfg.quanta_clk_out)) {
        reserve_mask |= BIT64(node_config->io_cfg.quanta_clk_out);
        gpio_func_sel(node_config->io_cfg.quanta_clk_out, PIN_FUNC_GPIO);
        esp_rom_gpio_connect_out_signal(node_config->io_cfg.quanta_clk_out, twai_controller_periph_signals.controllers[node->ctrlr_id].clk_out_sig, false, false);
    }

    //Configure bus status pin (Optional)
    if (GPIO_IS_VALID_OUTPUT_GPIO(node_config->io_cfg.bus_off_indicator)) {
        reserve_mask |= BIT64(node_config->io_cfg.bus_off_indicator);
        gpio_func_sel(node_config->io_cfg.bus_off_indicator, PIN_FUNC_GPIO);
        esp_rom_gpio_connect_out_signal(node_config->io_cfg.bus_off_indicator, twai_controller_periph_signals.controllers[node->ctrlr_id].bus_off_sig, false, false);
    }

    node->gpio_reserved = reserve_mask;
    uint64_t busy_mask = esp_gpio_reserve(reserve_mask);
    uint64_t conflict_mask = busy_mask & reserve_mask;
    for (; conflict_mask > 0;) {
        uint8_t pos = __builtin_ctzll(conflict_mask);
        conflict_mask &= ~(1ULL << pos);
        ESP_LOGW(TAG, "GPIO %d is not usable, maybe used by others", pos);
    }
    return ESP_OK;
}

static void _node_release_io(twai_onchip_ctx_t *node)
{
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, twai_controller_periph_signals.controllers[node->ctrlr_id].rx_sig, false);
    esp_gpio_revoke(node->gpio_reserved);
    for (; node->gpio_reserved > 0;) {
        uint8_t pos = __builtin_ctzll(node->gpio_reserved);
        node->gpio_reserved &= ~(1ULL << pos);
        gpio_output_disable(pos);
    }
}

static void _node_start_trans(twai_onchip_ctx_t *node)
{
    const twai_frame_header_t *format = &node->p_curr_tx->header;
    twai_hal_context_t *hal = &node->hal;
    twai_hal_frame_t tx_frame = {};

    int final_dlc = (format->dlc) ? format->dlc : twaifd_len2dlc(node->p_curr_tx->buffer_len);
    int data_len = (format->dlc) ? twaifd_dlc2len(format->dlc) : node->p_curr_tx->buffer_len;
    twaifd_ll_format_frame_header(format, final_dlc, &tx_frame);
    twaifd_ll_format_frame_data(node->p_curr_tx->buffer, data_len, &tx_frame);
    //TODO: utilize all txt buffers
    twaifd_ll_mount_tx_buffer(hal->dev, &tx_frame, 0);
    twaifd_ll_set_tx_cmd(hal->dev, 0, TWAIFD_LL_TX_CMD_READY);
}

static void _node_isr_main(void *arg)
{
    BaseType_t do_yield = pdFALSE;
    twai_onchip_ctx_t *twai_ctx = arg;
    uint32_t int_stat = twaifd_ll_get_intr_status(twai_ctx->hal.dev);
    twai_ctx->tx_error_count = twaifd_ll_get_tec(twai_ctx->hal.dev);
    twai_ctx->rx_error_count = twaifd_ll_get_rec(twai_ctx->hal.dev);
    twaifd_ll_clr_intr_status(twai_ctx->hal.dev, int_stat);

    // deal ERR event
    if (int_stat & (TWAIFD_LL_INTR_BUS_ERR | TWAIFD_LL_INTR_ARBI_LOST)) {
        uint32_t err_reason = twaifd_ll_get_err_reason_code(twai_ctx->hal.dev);
        twai_error_event_data_t e_data = {0};
        e_data.err_type.arb_lost = !!(int_stat & TWAIFD_LL_INTR_ARBI_LOST);
        e_data.err_type.bit_err = (err_reason == TWAIFD_LL_ERR_BIT_ERR);
        e_data.err_type.form_err = (err_reason == TWAIFD_LL_ERR_FRM_ERR);
        e_data.err_type.stuff_err = (err_reason == TWAIFD_LL_ERR_STUF_ERR);

        twai_ctx->history.bus_err_num ++;
        if (twai_ctx->cbs.on_error) {
            do_yield |= twai_ctx->cbs.on_error(&twai_ctx->api_base, &e_data, twai_ctx->user_data);
        }
    }

    // deal FSM event
    if (int_stat & (TWAIFD_LL_INTR_FSM_CHANGE | TWAIFD_LL_INTR_ERR_WARN)) {
        twai_state_change_event_data_t e_data = {.old_sta = twai_ctx->state};
        e_data.new_sta = twaifd_ll_get_fault_state(twai_ctx->hal.dev);
        uint32_t limit = twaifd_ll_get_err_warn_limit(twai_ctx->hal.dev);
        if ((e_data.new_sta == TWAI_ERROR_ACTIVE) && ((twai_ctx->tx_error_count >= limit) || (twai_ctx->rx_error_count >= limit))) {
            e_data.new_sta = TWAI_ERROR_WARNING;
        }
        atomic_store(&twai_ctx->state, e_data.new_sta);
        if (twai_ctx->cbs.on_state_change) {
            do_yield |= twai_ctx->cbs.on_state_change(&twai_ctx->api_base, &e_data, twai_ctx->user_data);
        }
    }

    // deal TX event
    if (int_stat & (TWAIFD_LL_INTR_TX_DONE | TWAIFD_LL_INTR_TX_SUCCESS)) {
        // only call tx_done_cb when tx without error, otherwise on_error_cb should triggered if it is registered
        if (twai_ctx->cbs.on_tx_done && (int_stat & TWAIFD_LL_INTR_TX_DONE)) {
            twai_tx_done_event_data_t tx_ev = {
                .done_trans_tx = (twai_frame_t *)twai_ctx->p_curr_tx,
            };
            do_yield |= twai_ctx->cbs.on_tx_done(&twai_ctx->api_base, &tx_ev, twai_ctx->user_data);
        }
        // start a new TX
        if ((atomic_load(&twai_ctx->state) != TWAI_ERROR_BUS_OFF) && xQueueReceiveFromISR(twai_ctx->tx_mount_queue, &twai_ctx->p_curr_tx, &do_yield)) {
            // Sanity check, must in `hw_busy` here, otherwise logic bug is somewhere
            assert(twai_ctx->hw_busy);
            _node_start_trans(twai_ctx);
        } else {
            atomic_store(&twai_ctx->hw_busy, false);
        }
    }

    // deal RX event
    if (int_stat & TWAIFD_LL_INTR_RX_NOT_EMPTY) {
        while (twaifd_ll_get_rx_frame_count(twai_ctx->hal.dev)) {
            twaifd_ll_get_rx_frame(twai_ctx->hal.dev, &twai_ctx->rcv_buff);
            if (twai_ctx->cbs.on_rx_done) {
                atomic_store(&twai_ctx->rx_isr, true);
                const twai_rx_done_event_data_t rx_ev = {
                };
                do_yield |= twai_ctx->cbs.on_rx_done(&twai_ctx->api_base, &rx_ev, twai_ctx->user_data);
                atomic_store(&twai_ctx->rx_isr, false);
            }
        }
        // rx flag can only cleared after read to empty
        twaifd_ll_clr_intr_status(twai_ctx->hal.dev, TWAIFD_LL_INTR_RX_NOT_EMPTY);
    }

    if (do_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t _node_delete(twai_node_handle_t node)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "delete node must when node stopped");

    _node_release_io(twai_ctx);
    _ctrlr_release(twai_ctx->ctrlr_id);
    _twai_rcc_clock_ctrl(twai_ctx->ctrlr_id, false);
#if CONFIG_PM_ENABLE
    if (twai_ctx->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_delete(twai_ctx->pm_lock), TAG, "delete power manager failed");
    }
#endif //CONFIG_PM_ENABLE
    esp_intr_free(twai_ctx->intr_hdl);
    vQueueDeleteWithCaps(twai_ctx->tx_mount_queue);
    free(twai_ctx);
    return ESP_OK;
}

static esp_err_t _node_register_callbacks(twai_node_handle_t node, const twai_event_callbacks_t *cbs, void *user_data)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "register callback must when node stopped");
#if CONFIG_TWAI_ISR_CACHE_SAFE
    ESP_RETURN_ON_FALSE(!cbs->on_tx_done || esp_ptr_in_iram(cbs->on_tx_done), ESP_ERR_INVALID_ARG, TAG, "on_tx_done callback not in IRAM");
    ESP_RETURN_ON_FALSE(!cbs->on_rx_done || esp_ptr_in_iram(cbs->on_rx_done), ESP_ERR_INVALID_ARG, TAG, "on_rx_done callback not in IRAM");
    ESP_RETURN_ON_FALSE(!cbs->on_state_change || esp_ptr_in_iram(cbs->on_state_change), ESP_ERR_INVALID_ARG, TAG, "on_state_change callback not in IRAM");
    ESP_RETURN_ON_FALSE(!cbs->on_error || esp_ptr_in_iram(cbs->on_error), ESP_ERR_INVALID_ARG, TAG, "on_error callback not in IRAM");
#endif
    memcpy(&twai_ctx->cbs, cbs, sizeof(twai_event_callbacks_t));
    twai_ctx->user_data = user_data;
    return ESP_OK;
}

static esp_err_t _node_check_timing_valid(twai_onchip_ctx_t *twai_ctx, const twai_timing_advanced_config_t *timing, uint32_t source_freq)
{
    if (!timing) {
        return ESP_OK;
    }
    ESP_RETURN_ON_FALSE(!timing->quanta_resolution_hz, ESP_ERR_INVALID_ARG, TAG, "quanta_resolution_hz is not supported");  //TODO: IDF-12725
    ESP_RETURN_ON_FALSE((timing->brp >= SOC_TWAI_BRP_MIN) && (timing->brp <= SOC_TWAI_BRP_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid brp");
    ESP_RETURN_ON_FALSE((timing->tseg_1 >= TWAI_LL_TSEG1_MIN) && (timing->tseg_1 <= TWAI_LL_TSEG1_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid tseg1");
    ESP_RETURN_ON_FALSE((timing->tseg_2 >= TWAI_LL_TSEG2_MIN) && (timing->tseg_2 <= TWAI_LL_TSEG2_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid tseg_2");
    ESP_RETURN_ON_FALSE((timing->sjw >= 1) && (timing->sjw <= TWAI_LL_SJW_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid swj");
    return ESP_OK;
}

static esp_err_t _node_set_bit_timing(twai_node_handle_t node, const twai_timing_advanced_config_t *timing, const twai_timing_advanced_config_t *timing_fd)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    twai_clock_source_t new_clock_src = twai_ctx->curr_clk_src;
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "config timing must when node stopped");
    if (timing && timing_fd) {
        ESP_RETURN_ON_FALSE(timing->clk_src == timing_fd->clk_src, ESP_ERR_INVALID_ARG, TAG, "clk_src of 2 configs must same");
        new_clock_src = timing->clk_src ? timing->clk_src : TWAI_CLK_SRC_DEFAULT;
    } else {
        if (timing) {
            new_clock_src = timing->clk_src ? timing->clk_src : TWAI_CLK_SRC_DEFAULT;
        } else {
            ESP_RETURN_ON_FALSE(!timing_fd->clk_src || (timing_fd->clk_src == TWAI_CLK_SRC_DEFAULT), ESP_ERR_INVALID_ARG, TAG, "don't change clk_src in single config");
        }
    }
    uint32_t source_freq = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(new_clock_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &source_freq), TAG, "clock src error, can't get freq");
    ESP_RETURN_ON_ERROR(_node_check_timing_valid(twai_ctx, timing, source_freq), TAG, "invalid param");
    ESP_RETURN_ON_ERROR(_node_check_timing_valid(twai_ctx, timing_fd, source_freq), TAG, "invalid fd param");

    int ssp_offset = 0;
    if (timing) {
        twaifd_ll_set_nominal_bitrate(twai_ctx->hal.dev, timing);
        if (timing->ssp_offset) {
            ssp_offset = timing->ssp_offset;
        }
    }
#if SOC_TWAI_SUPPORT_FD
    if (timing_fd) {
        twai_ctx->valid_fd_timing = true;
        twaifd_ll_set_fd_bitrate(twai_ctx->hal.dev, timing_fd);
        if (timing_fd->ssp_offset) {
            // prefer to config ssp by fd param
            ssp_offset = timing_fd->ssp_offset;
        }
    }
#endif

    // config ssp, the hardware offset_val TWAIFD_LL_SSP_SRC_MEAS_OFFSET measured by clock_src freq
    twaifd_ll_config_secondary_sample_point(twai_ctx->hal.dev, (ssp_offset ? TWAIFD_LL_SSP_SRC_MEAS_OFFSET : TWAIFD_LL_SSP_SRC_NO_SSP), ssp_offset);
    if (new_clock_src != twai_ctx->curr_clk_src) {
        twai_ctx->curr_clk_src = new_clock_src;
        TWAI_PERI_ATOMIC() {
            twaifd_ll_set_clock_source(twai_ctx->ctrlr_id, new_clock_src);
        }
    }
    return ESP_OK;
}

static esp_err_t _node_calc_set_bit_timing(twai_node_handle_t node, twai_clock_source_t clk_src, const twai_timing_basic_config_t *timing, const twai_timing_basic_config_t *timing_fd)
{
    ESP_RETURN_ON_FALSE(timing->bitrate, ESP_ERR_INVALID_ARG, TAG, "classic timing config is required");
#if !SOC_TWAI_SUPPORT_FD
    ESP_RETURN_ON_FALSE((!timing_fd->bitrate) || (timing_fd->bitrate == timing->bitrate), ESP_ERR_INVALID_ARG, TAG, "FD stage bitrate is not supported");
#endif
    twai_clock_source_t root_clock_src = clk_src ? clk_src : TWAI_CLK_SRC_DEFAULT;
    uint32_t source_freq = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(root_clock_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &source_freq), TAG, "can't get clock source freq");

    twai_timing_advanced_config_t timing_adv = { .clk_src = root_clock_src, };
    twai_timing_advanced_config_t *fd_cfg_ptr = NULL, timing_adv_fd = { .clk_src = root_clock_src, };
    twai_timing_constraint_t hw_const = {
        .brp_min = SOC_TWAI_BRP_MIN,
        .brp_max = SOC_TWAI_BRP_MAX,
        .tseg1_min = TWAI_LL_TSEG1_MIN,
        .tseg1_max = TWAI_LL_TSEG1_MAX,
        .tseg2_min = TWAI_LL_TSEG2_MIN,
        .tseg2_max = TWAI_LL_TSEG2_MAX,
        .sjw_max = TWAI_LL_SJW_MAX,
    };
    uint32_t real_baud = twai_node_timing_calc_param(source_freq, timing, &hw_const, &timing_adv);
    ESP_LOGD(TAG, "norm src %ld, param %ld %d %d %d %d %d", source_freq, timing_adv.brp, timing_adv.prop_seg, timing_adv.tseg_1, timing_adv.tseg_2, timing_adv.sjw, timing_adv.ssp_offset);
    ESP_RETURN_ON_FALSE(real_baud, ESP_ERR_INVALID_ARG, TAG, "bitrate can't achieve!");
    if (timing->bitrate != real_baud) {
        ESP_LOGW(TAG, "bitrate precision loss, adjust from %ld to %ld", timing->bitrate, real_baud);
    }
#if SOC_TWAI_SUPPORT_FD
    if (timing_fd->bitrate) {
        real_baud = twai_node_timing_calc_param(source_freq, timing_fd, &hw_const, &timing_adv_fd);
        ESP_LOGD(TAG, "fd src %ld, param %ld %d %d %d %d %d", source_freq, timing_adv_fd.brp, timing_adv_fd.prop_seg, timing_adv_fd.tseg_1, timing_adv_fd.tseg_2, timing_adv_fd.sjw, timing_adv_fd.ssp_offset);
        ESP_RETURN_ON_FALSE(real_baud, ESP_ERR_INVALID_ARG, TAG, "bitrate can't achieve!");
        if (timing_fd->bitrate != real_baud) {
            ESP_LOGW(TAG, "bitrate precision loss, adjust from %ld to %ld", timing_fd->bitrate, real_baud);
        }
        fd_cfg_ptr = &timing_adv_fd;
    }
#endif
    ESP_RETURN_ON_ERROR(_node_set_bit_timing(node, &timing_adv, fd_cfg_ptr), TAG, "invalid timing param");
    return ESP_OK;
}

/* -------------------------------------------------- Node Control -------------------------------------------------- */

static esp_err_t _node_enable(twai_node_handle_t node)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "node already enabled");

#if CONFIG_PM_ENABLE
    //Acquire pm_lock until _node_disable for potential receive
    if (twai_ctx->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_acquire(twai_ctx->pm_lock), TAG, "acquire power manager failed");
    }
#endif //CONFIG_PM_ENABLE
    twaifd_ll_enable_hw(twai_ctx->hal.dev, true);
    twaifd_ll_waiting_state_change(twai_ctx->hal.dev);  // waiting hardware ready before resume transaction

    // continuing the transaction if there be
    if (atomic_load(&twai_ctx->hw_busy) && twaifd_ll_get_fault_state(twai_ctx->hal.dev) != TWAI_ERROR_BUS_OFF) {
        _node_start_trans(twai_ctx);
    }
    // once interrupt enabled, state_change_intr will fire at the moment and update the `twai_ctx->state`
    ESP_RETURN_ON_ERROR(esp_intr_enable(twai_ctx->intr_hdl), TAG, "enable interrupt failed");
    return ESP_OK;
}

static esp_err_t _node_disable(twai_node_handle_t node)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) != TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "node already disabled");

    esp_intr_disable(twai_ctx->intr_hdl);
    atomic_store(&twai_ctx->state, TWAI_ERROR_BUS_OFF);
    twaifd_ll_enable_hw(twai_ctx->hal.dev, false);

#if CONFIG_PM_ENABLE
    if (twai_ctx->pm_lock) {
        ESP_RETURN_ON_ERROR(esp_pm_lock_release(twai_ctx->pm_lock), TAG, "release power manager failed");
    }
#endif //CONFIG_PM_ENABLE
    return ESP_OK;
}

static esp_err_t _node_config_mask_filter(twai_node_handle_t node, uint8_t filter_id, const twai_mask_filter_config_t *mask_cfg)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(filter_id < SOC_TWAI_MASK_FILTER_NUM, ESP_ERR_INVALID_ARG, TAG, "Invalid mask filter id %d", filter_id);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "config filter must when node stopped");

    bool full_open = (mask_cfg->mask == 0) && (mask_cfg->id == 0);
    bool full_close = (mask_cfg->mask == UINT32_MAX) && (mask_cfg->id == UINT32_MAX);
    bool cc_ext = full_open || (!full_close && mask_cfg->is_ext && !mask_cfg->no_classic);
    bool fd_ext = full_open || (!full_close && mask_cfg->is_ext && !mask_cfg->no_fd);
    bool cc_std = full_open || (!full_close && !mask_cfg->is_ext && !mask_cfg->no_classic);
    bool fd_std = full_open || (!full_close && !mask_cfg->is_ext && !mask_cfg->no_fd);
    twaifd_ll_filter_enable_basic_ext(twai_ctx->hal.dev, filter_id, false, cc_ext);
    twaifd_ll_filter_enable_fd_ext(twai_ctx->hal.dev, filter_id, false, fd_ext);
    twaifd_ll_filter_enable_basic_std(twai_ctx->hal.dev, filter_id, false, cc_std);
    twaifd_ll_filter_enable_fd_std(twai_ctx->hal.dev, filter_id, false, fd_std);
    twaifd_ll_filter_set_id_mask(twai_ctx->hal.dev, filter_id, mask_cfg->is_ext, mask_cfg->id, mask_cfg->mask);
    return ESP_OK;
}

#if SOC_TWAI_RANGE_FILTER_NUM
static esp_err_t _node_config_range_filter(twai_node_handle_t node, uint8_t filter_id, const twai_range_filter_config_t *range_cfg)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(filter_id < SOC_TWAI_RANGE_FILTER_NUM, ESP_ERR_INVALID_ARG, TAG, "Invalid range filter id %d", filter_id);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "config filter must when node stopped");

    bool full_open = (range_cfg->range_high == 0) && (range_cfg->range_low == 0);
    bool full_close = (range_cfg->range_high == UINT32_MAX) && (range_cfg->range_low == UINT32_MAX);
    bool cc_ext = full_open || (!full_close && range_cfg->is_ext && !range_cfg->no_classic);
    bool fd_ext = full_open || (!full_close && range_cfg->is_ext && !range_cfg->no_fd);
    bool cc_std = full_open || (!full_close && !range_cfg->is_ext && !range_cfg->no_classic);
    bool fd_std = full_open || (!full_close && !range_cfg->is_ext && !range_cfg->no_fd);
    twaifd_ll_filter_enable_basic_ext(twai_ctx->hal.dev, filter_id, true, cc_ext);
    twaifd_ll_filter_enable_fd_ext(twai_ctx->hal.dev, filter_id, true, fd_ext);
    twaifd_ll_filter_enable_basic_std(twai_ctx->hal.dev, filter_id, true, cc_std);
    twaifd_ll_filter_enable_fd_std(twai_ctx->hal.dev, filter_id, true, fd_std);
    twaifd_ll_filter_set_range(twai_ctx->hal.dev, 0, range_cfg->is_ext, range_cfg->range_high, range_cfg->range_low);
    return ESP_OK;
}
#endif

static esp_err_t _node_recover(twai_node_handle_t node)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "node not in bus off");

    // After recover command, the hardware require 128 consecutive occurrences of 11 recessive bits received, so that it can be active again!
    // Checking `twai_node_status_t::state` Or waiting `on_state_change` callback can know if recover is finish
    twaifd_ll_set_operate_cmd(twai_ctx->hal.dev, TWAIFD_LL_HW_CMD_RST_ERR_CNT);
    twai_ctx->history.bus_err_num = 0;
    return ESP_OK;
}

static esp_err_t _node_get_status(twai_node_handle_t node, twai_node_status_t *status_ret, twai_node_record_t *record_ret)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);

    if (status_ret) {
        status_ret->state = atomic_load(&twai_ctx->state);
        status_ret->tx_error_count = twai_ctx->tx_error_count;
        status_ret->rx_error_count = twai_ctx->rx_error_count;
    }
    if (record_ret) {
        *record_ret = twai_ctx->history;
    }
    return ESP_OK;
}

/* ----------------------------------------------- Node Communication ----------------------------------------------- */

static esp_err_t _node_queue_tx(twai_node_handle_t node, const twai_frame_t *frame, int timeout)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    if (frame->header.dlc && frame->buffer_len) {
        ESP_RETURN_ON_FALSE(frame->header.dlc == twaifd_len2dlc(frame->buffer_len), ESP_ERR_INVALID_ARG, TAG, "unmatched dlc and buffer_len");
    }
    ESP_RETURN_ON_FALSE(frame->buffer_len <= (frame->header.fdf ? TWAIFD_FRAME_MAX_LEN : TWAI_FRAME_MAX_LEN), ESP_ERR_INVALID_ARG, TAG, "illegal transfer length (buffer_len %ld)", frame->buffer_len);
    ESP_RETURN_ON_FALSE((!frame->header.brs) || (twai_ctx->valid_fd_timing), ESP_ERR_INVALID_ARG, TAG, "brs can't be used without config data_timing");
    ESP_RETURN_ON_FALSE(!twai_ctx->hal.enable_listen_only, ESP_ERR_NOT_SUPPORTED, TAG, "node is config as listen only");
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) != TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "node is bus off");
    TickType_t ticks_to_wait = (timeout == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout);

    bool false_var = false;
    if (atomic_compare_exchange_strong(&twai_ctx->hw_busy, &false_var, true)) {
        twai_ctx->p_curr_tx = frame;
        _node_start_trans(twai_ctx);
    } else {
        //options in following steps (in_queue->2nd_check->pop_queue) should exec ASAP
        //within about 50us (minimum time for one msg), to ensure data safe
        ESP_RETURN_ON_FALSE(xQueueSend(twai_ctx->tx_mount_queue, &frame, ticks_to_wait), ESP_ERR_TIMEOUT, TAG, "tx queue full");
        false_var = false;
        if (atomic_compare_exchange_strong(&twai_ctx->hw_busy, &false_var, true)) {
            if (xQueueReceive(twai_ctx->tx_mount_queue, &twai_ctx->p_curr_tx, 0) != pdTRUE) {
                assert(false && "should always get frame at this moment");
            }
            _node_start_trans(twai_ctx);
        }
    }
    return ESP_OK;
}

static esp_err_t _node_parse_rx(twai_node_handle_t node, twai_frame_header_t *header, uint8_t *rx_buffer, size_t buf_len, size_t *ret_len)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE_ISR(atomic_load(&twai_ctx->rx_isr), ESP_ERR_INVALID_STATE, TAG, "rx can only called in `rx_done` callback");

    twaifd_ll_parse_frame_header(&twai_ctx->rcv_buff, header);
    uint32_t frame_data_len = twaifd_dlc2len(header->dlc);
    uint8_t final_len = (frame_data_len < buf_len) ? frame_data_len : buf_len;
    twaifd_ll_parse_frame_data(&twai_ctx->rcv_buff, rx_buffer, final_len);
    if (ret_len) {
        *ret_len = frame_data_len;
    }
    return ESP_OK;
}

/* --------------------------------- Public --------------------------------- */
esp_err_t twai_new_node_onchip(const twai_onchip_node_config_t *node_config, twai_node_handle_t *node_ret)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(node_config->tx_queue_depth > 0, ESP_ERR_INVALID_ARG, TAG, "tx_queue_depth at least 1");
    ESP_RETURN_ON_FALSE(!node_config->intr_priority || (BIT(node_config->intr_priority) & ESP_INTR_FLAG_LOWMED), ESP_ERR_INVALID_ARG, TAG, "Invalid intr_priority level");

    // Allocate TWAI node object memory
    twai_onchip_ctx_t *node = heap_caps_calloc(1, sizeof(twai_onchip_ctx_t), TWAI_MALLOC_CAPS);
    ESP_RETURN_ON_FALSE(node, ESP_ERR_NO_MEM, TAG, "No mem");
    // Acquire controller
    int ctrlr_id = _ctrlr_acquire(node);
    ESP_GOTO_ON_FALSE(ctrlr_id != -1, ESP_ERR_NOT_FOUND, ctrlr_err, TAG, "Controller not available");
    node->ctrlr_id = ctrlr_id;

    // state is in bus_off before enabled
    atomic_store(&node->state, TWAI_ERROR_BUS_OFF);
    node->tx_mount_queue = xQueueCreateWithCaps(node_config->tx_queue_depth, sizeof(twai_frame_t *), TWAI_MALLOC_CAPS);
    ESP_GOTO_ON_FALSE(node->tx_mount_queue, ESP_ERR_NO_MEM, create_err, TAG, "no_mem");
    uint32_t intr_flags = node_config->intr_priority ? BIT(node_config->intr_priority) | ESP_INTR_FLAG_INTRDISABLED : ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_INTRDISABLED;
#if CONFIG_TWAI_ISR_CACHE_SAFE
    intr_flags |= ESP_INTR_FLAG_IRAM;
#endif
    ESP_GOTO_ON_ERROR(esp_intr_alloc(twai_controller_periph_signals.controllers[ctrlr_id].irq_id, intr_flags, _node_isr_main, (void *)node, &node->intr_hdl),
                      create_err, TAG, "Alloc interrupt failed");

    // Enable bus clock and reset controller
    _twai_rcc_clock_ctrl(ctrlr_id, true);
    // Initialize HAL and configure register defaults.
    twai_hal_config_t hal_config = {
        .controller_id = node->ctrlr_id,
        .intr_mask = DRIVER_DEFAULT_INTERRUPTS,
        .enable_listen_only = node_config->flags.enable_listen_only,
    };
    ESP_GOTO_ON_FALSE(twai_hal_init(&node->hal, &hal_config), ESP_ERR_INVALID_STATE, config_err, TAG, "hardware not in reset state");
    twaifd_ll_set_mode(node->hal.dev, node_config->flags.enable_listen_only, node_config->flags.enable_self_test, node_config->flags.enable_loopback);
    twaifd_ll_set_tx_retrans_limit(node->hal.dev, node_config->fail_retry_cnt);
    twaifd_ll_filter_block_rtr(node->hal.dev, node_config->flags.no_receive_rtr);
    twaifd_ll_enable_filter_mode(node->hal.dev, true);  // each filter still has independent enable control
    twaifd_ll_enable_fd_mode(node->hal.dev, true);      // fd frame still controlled by `header.fdf`

    // Configure bus timing
    ESP_GOTO_ON_ERROR(_node_calc_set_bit_timing(&node->api_base, node_config->clk_src, &node_config->bit_timing, &node_config->data_timing), config_err, TAG, "bitrate error");

    // Configure GPIO
    ESP_GOTO_ON_ERROR(_node_config_io(node, node_config), config_err, TAG, "gpio config failed");
#if CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, twai_controller_periph_signals.controllers[ctrlr_id].module_name, &node->pm_lock), config_err, TAG, "init power manager failed");
#endif //CONFIG_PM_ENABLE

    node->api_base.enable = _node_enable;
    node->api_base.disable = _node_disable;
    node->api_base.del = _node_delete;
    node->api_base.recover = _node_recover;
    node->api_base.config_mask_filter = _node_config_mask_filter;
    node->api_base.config_range_filter = _node_config_range_filter;
    node->api_base.timing_reconfig = _node_set_bit_timing;
    node->api_base.register_cbs = _node_register_callbacks;
    node->api_base.transmit = _node_queue_tx;
    node->api_base.receive_isr = _node_parse_rx;
    node->api_base.get_info = _node_get_status;

    *node_ret = &node->api_base;
    return ESP_OK;

config_err:
    if (node->intr_hdl) {
        esp_intr_free(node->intr_hdl);
    }
    if (node->tx_mount_queue) {
        vQueueDeleteWithCaps(node->tx_mount_queue);
    }
create_err:
    _ctrlr_release(ctrlr_id);
ctrlr_err:
    free(node);
    return ret;
}
