/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_timer.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "esp_private/twai_interface.h"
#include "esp_private/twai_utils.h"
#include "twai_private.h"
#include "hal/twai_periph.h"
#include "hal/twai_hal.h"
#if SOC_HAS(TWAI_FD)
#include "hal/twaifd_ll.h"
#endif

static void _twai_rcc_clock_ctrl(uint8_t ctrlr_id, bool enable)
{
    PERIPH_RCC_ATOMIC() {
        twai_ll_enable_bus_clock(ctrlr_id, enable);
        twai_ll_reset_register(ctrlr_id);
    }
    PERIPH_RCC_ATOMIC() {
        twai_ll_enable_clock(ctrlr_id, enable);
    }
}
static void _twai_rcc_clock_sel(uint8_t ctrlr_id, twai_clock_source_t clock)
{
    PERIPH_RCC_ATOMIC() {
        twai_ll_set_clock_source(ctrlr_id, clock);
    }
}

#define TWAI_IDLE_EVENT_BIT BIT0    //event used for tx_wait_all_done

typedef struct {
    struct twai_node_base api_base;
    int ctrlr_id;
    uint64_t gpio_reserved;
    twai_hal_context_t *hal;
    intr_handle_t intr_hdl;
    intr_handle_t timer_intr_hdl;
    QueueHandle_t tx_mount_queue;
    EventGroupHandle_t event_group;
    twai_clock_source_t curr_clk_src;
    uint32_t src_freq_hz;
    uint32_t timestamp_freq_hz;
    uint32_t valid_fd_timing;
    twai_event_callbacks_t cbs;
    void *user_data;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif

    _Atomic twai_error_state_t state;
    twai_node_record_t history;

    atomic_bool hw_busy;
    atomic_bool rx_isr;
    const twai_frame_t *p_curr_tx;
    twai_hal_frame_t rcv_buff;
} twai_onchip_ctx_t;

typedef struct twai_platform_s {
    _lock_t ctrlr_mutex;
    _lock_t intr_mutex;
    twai_onchip_ctx_t *nodes[SOC_TWAI_CONTROLLER_NUM];
} twai_platform_t;
static twai_platform_t s_platform;

static int _ctrlr_acquire(twai_onchip_ctx_t *node)
{
    int ctrlr_id = -1;
    _lock_acquire(&s_platform.ctrlr_mutex);
    // Check if there is a controller available for use
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        if (s_platform.nodes[i] == NULL) {
            // Assign to node object to the controller slot
            s_platform.nodes[i] = node;
            ctrlr_id = i;
            break;
        }
    }
    _lock_release(&s_platform.ctrlr_mutex);

    // Return the controller index or -1
    return ctrlr_id;
}

static void _ctrlr_release(int ctrlr_id)
{
    _lock_acquire(&s_platform.ctrlr_mutex);
    assert(s_platform.nodes[ctrlr_id]);
    // Clear the node object from the controller slot
    s_platform.nodes[ctrlr_id] = NULL;
    _lock_release(&s_platform.ctrlr_mutex);
}

static esp_err_t _node_config_io(twai_onchip_ctx_t *node, const twai_onchip_node_config_t *node_config)
{
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(node_config->io_cfg.tx) || (node_config->flags.enable_listen_only && (node_config->io_cfg.tx == -1)), ESP_ERR_INVALID_ARG, TAG, "Invalid tx gpio num");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(node_config->io_cfg.rx), ESP_ERR_INVALID_ARG, TAG, "Invalid rx gpio num");
    ESP_RETURN_ON_FALSE(!(GPIO_IS_VALID_OUTPUT_GPIO(node_config->io_cfg.quanta_clk_out) && (twai_periph_signals[node->ctrlr_id].clk_out_sig < 0)), ESP_ERR_NOT_SUPPORTED, TAG, "quanta_clk_out gpio is not supported");
    ESP_RETURN_ON_FALSE(!(GPIO_IS_VALID_OUTPUT_GPIO(node_config->io_cfg.bus_off_indicator) && (twai_periph_signals[node->ctrlr_id].bus_off_sig < 0)), ESP_ERR_NOT_SUPPORTED, TAG, "bus_off_indicator gpio is not supported");

    uint64_t reserve_mask = 0;
    // Set RX pin
    gpio_set_pull_mode(node_config->io_cfg.rx, GPIO_PULLUP_ONLY);    // pullup to avoid noise if no connection to transceiver
    gpio_matrix_input(node_config->io_cfg.rx, twai_periph_signals[node->ctrlr_id].rx_sig, false);

    // Set TX pin
    if (node_config->io_cfg.tx != -1) { // listen only node is able to not have TX pin
        reserve_mask |= BIT64(node_config->io_cfg.tx);
        gpio_matrix_output(node_config->io_cfg.tx, twai_periph_signals[node->ctrlr_id].tx_sig, false, false);
    }

    //Configure output clock pin (Optional)
    if (GPIO_IS_VALID_OUTPUT_GPIO(node_config->io_cfg.quanta_clk_out)) {
        reserve_mask |= BIT64(node_config->io_cfg.quanta_clk_out);
        gpio_matrix_output(node_config->io_cfg.quanta_clk_out, twai_periph_signals[node->ctrlr_id].clk_out_sig, false, false);
    }

    //Configure bus status pin (Optional)
    if (GPIO_IS_VALID_OUTPUT_GPIO(node_config->io_cfg.bus_off_indicator)) {
        reserve_mask |= BIT64(node_config->io_cfg.bus_off_indicator);
        gpio_matrix_output(node_config->io_cfg.bus_off_indicator, twai_periph_signals[node->ctrlr_id].bus_off_sig, false, false);
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
    esp_rom_gpio_connect_in_signal(GPIO_MATRIX_CONST_ONE_INPUT, twai_periph_signals[node->ctrlr_id].rx_sig, false);
    esp_gpio_revoke(node->gpio_reserved);
    for (; node->gpio_reserved > 0;) {
        uint8_t pos = __builtin_ctzll(node->gpio_reserved);
        node->gpio_reserved &= ~(1ULL << pos);
        gpio_output_disable(pos);
    }
}

static void _node_start_trans(twai_onchip_ctx_t *node)
{
    const twai_frame_t *frame = node->p_curr_tx;
    twai_hal_context_t *hal = node->hal;

    twai_hal_frame_t hal_buf = {};
    twai_hal_trans_desc_t hal_trans = {
        .frame = {
            .header = (twai_frame_header_t *) &frame->header,
            .buffer = frame->buffer,
            .buffer_len = frame->buffer_len,
        },
        .config = {
            .retry_cnt = hal->retry_cnt,
            .loopback = hal->enable_loopback,
        },
    };
    twai_hal_format_frame(&hal_trans, &hal_buf);
    //TODO: utilize all txt buffers
    twai_hal_set_tx_buffer_and_transmit(hal, &hal_buf, 0);
}

static void _node_isr_main(void *arg)
{
    BaseType_t do_yield = pdFALSE;
    twai_onchip_ctx_t *twai_ctx = arg;
    uint32_t events = twai_hal_get_events(twai_ctx->hal);    //Get the events that triggered the interrupt

#if TWAI_LL_HAS_RX_FRAME_ISSUE || TWAI_LL_HAS_RX_FIFO_ISSUE
    // Errata workaround: Reset the peripheral on detection of this errata condition.
    // Note that if a frame is being sent on the bus during the reset, the message will be lost.
    if (events & TWAI_HAL_EVENT_NEED_PERIPH_RESET) {
        ESP_EARLY_LOGD(TAG, "Triggered peripheral reset");
        twai_hal_prepare_for_reset(twai_ctx->hal);
        PERIPH_RCC_ATOMIC() {
            twai_ll_reset_register(twai_ctx->ctrlr_id);
        }
        twai_hal_recover_from_reset(twai_ctx->hal);
    }
#endif

    // deal BUS ERR event
    if (events & (TWAI_HAL_EVENT_BUS_ERR | TWAI_HAL_EVENT_ARB_LOST)) {
        twai_error_event_data_t e_data = {
            .err_flags = twai_hal_get_err_flags(twai_ctx->hal),
        };
        twai_ctx->history.bus_err_num ++;
        if (twai_ctx->cbs.on_error) {
            do_yield |= twai_ctx->cbs.on_error(&twai_ctx->api_base, &e_data, twai_ctx->user_data);
        }
    }

    // deal FSM event
    if (events & (TWAI_HAL_EVENT_ERROR_ACTIVE | TWAI_HAL_EVENT_ERROR_WARNING | TWAI_HAL_EVENT_ERROR_PASSIVE | TWAI_HAL_EVENT_BUS_OFF)) {
        twai_error_state_t curr_sta = (events & TWAI_HAL_EVENT_BUS_OFF) ? TWAI_ERROR_BUS_OFF : \
                                      (events & TWAI_HAL_EVENT_ERROR_PASSIVE) ? TWAI_ERROR_PASSIVE : \
                                      (events & TWAI_HAL_EVENT_ERROR_WARNING) ? TWAI_ERROR_WARNING : TWAI_ERROR_ACTIVE;
        twai_state_change_event_data_t e_data = {
            .old_sta = twai_ctx->state,
            .new_sta = curr_sta,
        };
        atomic_store(&twai_ctx->state, e_data.new_sta);
        if (twai_ctx->cbs.on_state_change) {
            do_yield |= twai_ctx->cbs.on_state_change(&twai_ctx->api_base, &e_data, twai_ctx->user_data);
        }
        // node recover from busoff, restart remain tx transaction
        if ((e_data.old_sta == TWAI_ERROR_BUS_OFF) && (e_data.new_sta == TWAI_ERROR_ACTIVE)) {
            if (xQueueReceiveFromISR(twai_ctx->tx_mount_queue, &twai_ctx->p_curr_tx, &do_yield)) {
                atomic_store(&twai_ctx->hw_busy, true);
                _node_start_trans(twai_ctx);
            } else {
                atomic_store(&twai_ctx->hw_busy, false);
                xEventGroupSetBitsFromISR(twai_ctx->event_group, TWAI_IDLE_EVENT_BIT, &do_yield);
            }
        }
    }

    // deal RX event, then TX later, TODO: DIG-620
    if (events & TWAI_HAL_EVENT_RX_BUFF_FRAME) {
        while (twai_hal_get_rx_msg_count(twai_ctx->hal)) {
            if (twai_hal_read_rx_fifo(twai_ctx->hal, &twai_ctx->rcv_buff)) {
#if !SOC_HAS(TWAI_FD)
                // the legacy hardware filter don't support split frame format std/ext in filter, check in software
                if (!twai_hal_soft_filter_check_msg(twai_ctx->hal, &twai_ctx->rcv_buff)) {
                    continue;   // soft filter to check if id type match the filter config
                }
#endif
                if (twai_ctx->cbs.on_rx_done) {
                    atomic_store(&twai_ctx->rx_isr, true);  //todo: using compare_exchange to avoid intr from 2nd core
                    twai_rx_done_event_data_t rx_ev = {};
                    do_yield |= twai_ctx->cbs.on_rx_done(&twai_ctx->api_base, &rx_ev, twai_ctx->user_data);
                    atomic_store(&twai_ctx->rx_isr, false);
                }
            } else {    // failed to read from RX fifo because message is overrun
#if !TWAI_LL_SUPPORT(RX_STATUS)
                twai_hal_clear_rx_fifo_overrun(twai_ctx->hal);
                break;
#endif
            }
        }
    }

    // deal TX event
    if (events & TWAI_HAL_EVENT_TX_BUFF_FREE) {
        if (twai_ctx->cbs.on_tx_done) {
            twai_tx_done_event_data_t tx_ev = {
                .is_tx_success = (events & TWAI_HAL_EVENT_TX_SUCCESS),  // find 'on_error_cb' if not success
                .done_tx_frame = twai_ctx->p_curr_tx,
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
            if (atomic_load(&twai_ctx->state) != TWAI_ERROR_BUS_OFF) {
                // only when node is not in busoff here, means tx is finished
                xEventGroupSetBitsFromISR(twai_ctx->event_group, TWAI_IDLE_EVENT_BIT, &do_yield);
            }
        }
    }

    if (do_yield) {
        portYIELD_FROM_ISR();
    }
}

static void _node_destroy(twai_onchip_ctx_t *twai_ctx)
{
#ifdef CONFIG_PM_ENABLE
    if (twai_ctx->pm_lock) {
        esp_pm_lock_delete(twai_ctx->pm_lock);
    }
#endif
    if (twai_ctx->intr_hdl) {
        esp_intr_free(twai_ctx->intr_hdl);
    }
    if (twai_ctx->timer_intr_hdl) {
        esp_intr_free(twai_ctx->timer_intr_hdl);
    }
    if (twai_ctx->tx_mount_queue) {
        vQueueDeleteWithCaps(twai_ctx->tx_mount_queue);
    }
    if (twai_ctx->event_group) {
        vEventGroupDeleteWithCaps(twai_ctx->event_group);
    }
    if (twai_ctx->ctrlr_id != -1) {
        _ctrlr_release(twai_ctx->ctrlr_id);
    }
    free(twai_ctx);
}

static esp_err_t _node_delete(twai_node_handle_t node)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "delete node must when node stopped");

    _node_release_io(twai_ctx);
    twai_hal_deinit(twai_ctx->hal);
    _twai_rcc_clock_ctrl(twai_ctx->ctrlr_id, false);
    // curr_clk_src must not NULL as we already set to Default in twai_new_node_onchip
    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src(twai_ctx->curr_clk_src, false), TAG, "disable clock source failed");
    _node_destroy(twai_ctx);
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

static esp_err_t _node_check_timing_valid(twai_onchip_ctx_t *twai_ctx, const twai_timing_advanced_config_t *timing)
{
    if (timing) {
        ESP_RETURN_ON_FALSE(twai_hal_check_brp_validation(twai_ctx->hal, timing->brp), ESP_ERR_INVALID_ARG, TAG, "invalid brp");
        ESP_RETURN_ON_FALSE((timing->tseg_1 >= TWAI_LL_TSEG1_MIN) && (timing->tseg_1 <= TWAI_LL_TSEG1_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid tseg1");
        ESP_RETURN_ON_FALSE((timing->tseg_2 >= TWAI_LL_TSEG2_MIN) && (timing->tseg_2 <= TWAI_LL_TSEG2_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid tseg_2");
        ESP_RETURN_ON_FALSE((timing->sjw >= 1) && (timing->sjw <= TWAI_LL_SJW_MAX), ESP_ERR_INVALID_ARG, TAG, "invalid swj");
    }
    return ESP_OK;
}

static esp_err_t _node_set_bit_timing(twai_node_handle_t node, const twai_timing_advanced_config_t *timing, const twai_timing_advanced_config_t *timing_fd)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "config timing must when node stopped");
    ESP_RETURN_ON_ERROR(_node_check_timing_valid(twai_ctx, timing), TAG, "invalid param");
    ESP_RETURN_ON_ERROR(_node_check_timing_valid(twai_ctx, timing_fd), TAG, "invalid fd param");

    if (timing) {
        twai_hal_configure_timing(twai_ctx->hal, timing);
    }
#if SOC_HAS(TWAI_FD)
    if (timing_fd) {
        twai_ctx->valid_fd_timing = true;
        twai_hal_configure_timing_fd(twai_ctx->hal, timing_fd);
    }
#endif
    return ESP_OK;
}

static esp_err_t _node_calc_set_bit_timing(twai_node_handle_t node, const twai_timing_basic_config_t *timing, const twai_timing_basic_config_t *timing_fd)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(timing->bitrate, ESP_ERR_INVALID_ARG, TAG, "classic timing config is required");
#if !SOC_HAS(TWAI_FD)
    ESP_RETURN_ON_FALSE((!timing_fd->bitrate) || (timing_fd->bitrate == timing->bitrate), ESP_ERR_INVALID_ARG, TAG, "FD stage bitrate is not supported");
#endif

    twai_timing_constraint_t hw_const = {
        .brp_min = TWAI_LL_BRP_MIN,
        .brp_max = TWAI_LL_BRP_MAX,
        .tseg1_min = TWAI_LL_TSEG1_MIN,
        .tseg1_max = TWAI_LL_TSEG1_MAX,
        .tseg2_min = TWAI_LL_TSEG2_MIN,
        .tseg2_max = TWAI_LL_TSEG2_MAX,
        .sjw_max = TWAI_LL_SJW_MAX,
    };

    twai_timing_advanced_config_t timing_adv = {}, *timing_fd_ptr = NULL;
    uint32_t real_baud = twai_node_timing_calc_param(twai_ctx->src_freq_hz, timing, &hw_const, &timing_adv);
    ESP_LOGD(TAG, "timing: src %ld brp %ld prop %d seg1 %d seg2 %d sjw %d ssp %d", twai_ctx->src_freq_hz, timing_adv.brp, timing_adv.prop_seg, timing_adv.tseg_1, timing_adv.tseg_2, timing_adv.sjw, timing_adv.ssp_offset);
    ESP_RETURN_ON_FALSE(real_baud, ESP_ERR_INVALID_ARG, TAG, "bitrate can't achieve!");
    if (timing->bitrate != real_baud) {
        ESP_LOGW(TAG, "bitrate precision loss, adjust from %ld to %ld", timing->bitrate, real_baud);
    }
#if SOC_HAS(TWAI_FD)
    twai_timing_advanced_config_t timing_adv_fd = {};
    if (timing_fd->bitrate) {
        real_baud = twai_node_timing_calc_param(twai_ctx->src_freq_hz, timing_fd, &hw_const, &timing_adv_fd);
        ESP_LOGD(TAG, "timing_fd: src %ld brp %ld prop %d seg1 %d seg2 %d sjw %d ssp %d", twai_ctx->src_freq_hz, timing_adv_fd.brp, timing_adv_fd.prop_seg, timing_adv_fd.tseg_1, timing_adv_fd.tseg_2, timing_adv_fd.sjw, timing_adv_fd.ssp_offset);
        ESP_RETURN_ON_FALSE(real_baud, ESP_ERR_INVALID_ARG, TAG, "bitrate can't achieve!");
        if (timing_fd->bitrate != real_baud) {
            ESP_LOGW(TAG, "bitrate precision loss, adjust from %ld to %ld", timing_fd->bitrate, real_baud);
        }
        timing_fd_ptr = &timing_adv_fd;
    }
#endif
    ESP_RETURN_ON_ERROR(_node_set_bit_timing(node, &timing_adv, timing_fd_ptr), TAG, "invalid timing param, bitrate can't achieve!");
    return ESP_OK;
}

//convert microseconds to timestamp units
__attribute__((always_inline))
static inline uint64_t _time_us_to_timestamp(uint64_t time_us, uint32_t resolution)
{
    if (resolution > 1000000) {
        return time_us * (resolution / 1000000);
    } else if (resolution > 0) {
        return time_us / (1000000 / resolution);
    }
    return 0;
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
    twai_hal_start(twai_ctx->hal);
#if TWAI_LL_SUPPORT(TIMESTAMP)
    if (twai_ctx->timestamp_freq_hz) {
        twai_hal_timer_start_with(twai_ctx->hal, _time_us_to_timestamp(esp_timer_get_time(), twai_ctx->timestamp_freq_hz));
        ESP_RETURN_ON_ERROR(esp_intr_enable(twai_ctx->timer_intr_hdl), TAG, "enable timer interrupt failed");
    }
#endif
    twai_error_state_t hw_state = twai_hal_get_err_state(twai_ctx->hal);
    atomic_store(&twai_ctx->state, hw_state);
    // continuing the transaction if there be
    if (atomic_load(&twai_ctx->hw_busy) && hw_state != TWAI_ERROR_BUS_OFF) {
        _node_start_trans(twai_ctx);
    }
    ESP_RETURN_ON_ERROR(esp_intr_enable(twai_ctx->intr_hdl), TAG, "enable interrupt failed");
    return ESP_OK;
}

static esp_err_t _node_disable(twai_node_handle_t node)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) != TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "node already disabled");

#if TWAI_LL_SUPPORT(TIMESTAMP)
    if (twai_ctx->timestamp_freq_hz) {
        twai_hal_timer_stop(twai_ctx->hal);
        ESP_RETURN_ON_ERROR(esp_intr_disable(twai_ctx->timer_intr_hdl), TAG, "disable timer interrupt failed");
    }
#endif
    ESP_RETURN_ON_ERROR(esp_intr_disable(twai_ctx->intr_hdl), TAG, "disable interrupt failed");
    atomic_store(&twai_ctx->state, TWAI_ERROR_BUS_OFF);
    twai_hal_stop(twai_ctx->hal);
#if TWAI_LL_HAS_RX_FRAME_ISSUE
    // when `disable` happens during hardware busy, the next RX frame is corrupted, a HW reset can fix it
    if (twai_hal_is_hw_busy(twai_ctx->hal)) {
        twai_hal_backup_config(twai_ctx->hal);
        PERIPH_RCC_ATOMIC() {
            twai_ll_reset_register(twai_ctx->ctrlr_id);
        }
        twai_hal_restore_config(twai_ctx->hal);
    }
#endif

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
    ESP_RETURN_ON_FALSE(mask_cfg->num_of_ids <= 1, ESP_ERR_INVALID_ARG, TAG, "Invalid num_of_ids");
    uint32_t id = mask_cfg->num_of_ids ? mask_cfg->id_list[0] : mask_cfg->id;
    bool full_close = (mask_cfg->mask == UINT32_MAX) && (id == UINT32_MAX);
    ESP_RETURN_ON_FALSE(full_close || mask_cfg->dual_filter || mask_cfg->is_ext || !((mask_cfg->mask | id) & ~TWAI_STD_ID_MASK), ESP_ERR_INVALID_ARG, TAG, "std_id only (is_ext=0) but valid id/mask larger than 11 bits");
#if SOC_HAS(TWAI_FD)
    // FD targets don't support Dual filter
    ESP_RETURN_ON_FALSE(!mask_cfg->dual_filter, ESP_ERR_NOT_SUPPORTED, TAG, "The target don't support Dual Filter");
#endif
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "filter config must do when node stopped");

    twai_hal_configure_mask_filter(twai_ctx->hal, filter_id, mask_cfg);
    return ESP_OK;
}

#if SOC_TWAI_RANGE_FILTER_NUM
static esp_err_t _node_config_range_filter(twai_node_handle_t node, uint8_t filter_id, const twai_range_filter_config_t *range_cfg)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(filter_id < SOC_TWAI_RANGE_FILTER_NUM, ESP_ERR_INVALID_ARG, TAG, "Invalid range filter id %d", filter_id);
    ESP_RETURN_ON_FALSE((range_cfg->range_low > range_cfg->range_high) || range_cfg->is_ext || !(range_cfg->range_high & ~TWAI_STD_ID_MASK), \
                        ESP_ERR_INVALID_ARG, TAG, "std_id only (is_ext=0) but valid low/high id larger than 11 bits");
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "filter config must do when node stopped");

    twai_hal_configure_range_filter(twai_ctx->hal, filter_id, range_cfg);
    return ESP_OK;
}
#endif

static esp_err_t _node_recover(twai_node_handle_t node)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "node not in bus off");

    // After recover command, the hardware require 128 consecutive occurrences of 11 recessive bits received, so that it can be active again!
    // Checking `twai_node_status_t::state` Or waiting `on_state_change` callback can know if recover is finish
    twai_hal_start_bus_recovery(twai_ctx->hal);
    twai_ctx->history.bus_err_num = 0;
    return ESP_OK;
}

static esp_err_t _node_get_status(twai_node_handle_t node, twai_node_status_t *status_ret, twai_node_record_t *record_ret)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);

    if (status_ret) {
        status_ret->state = atomic_load(&twai_ctx->state);
        status_ret->tx_error_count = twai_hal_get_tec(twai_ctx->hal);
        status_ret->rx_error_count = twai_hal_get_rec(twai_ctx->hal);
        status_ret->tx_queue_remaining = uxQueueSpacesAvailable(twai_ctx->tx_mount_queue);
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
        ESP_RETURN_ON_FALSE_ISR(frame->header.dlc == twaifd_len2dlc(frame->buffer_len), ESP_ERR_INVALID_ARG, TAG, "unmatched dlc(%i) and buffer_len(%i)", frame->header.dlc, twaifd_len2dlc(frame->buffer_len));
    }
#if !SOC_HAS(TWAI_FD)
    ESP_RETURN_ON_FALSE_ISR(!frame->header.fdf || frame->buffer_len <= TWAI_FRAME_MAX_LEN, ESP_ERR_INVALID_ARG, TAG, "fdf flag or buffer_len not supported");
#endif
    ESP_RETURN_ON_FALSE_ISR((frame->header.dlc <= TWAIFD_FRAME_MAX_DLC) && \
                            (frame->buffer_len <= (frame->header.fdf ? TWAIFD_FRAME_MAX_LEN : TWAI_FRAME_MAX_LEN)), ESP_ERR_INVALID_ARG, TAG, "illegal transfer length (buffer_len %ld)", frame->buffer_len);
    ESP_RETURN_ON_FALSE_ISR((!frame->header.brs) || (twai_ctx->valid_fd_timing), ESP_ERR_INVALID_ARG, TAG, "brs can't be used without config data_timing");
    ESP_RETURN_ON_FALSE_ISR(!twai_ctx->hal->enable_listen_only, ESP_ERR_NOT_SUPPORTED, TAG, "node is config as listen only");
    ESP_RETURN_ON_FALSE_ISR(atomic_load(&twai_ctx->state) != TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "node is bus off");
    TickType_t ticks_to_wait = (timeout == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout);

    xEventGroupClearBits(twai_ctx->event_group, TWAI_IDLE_EVENT_BIT); //going to send, clear the idle event
    bool false_var = false;
    if (atomic_compare_exchange_strong(&twai_ctx->hw_busy, &false_var, true)) {
        twai_ctx->p_curr_tx = frame;
        _node_start_trans(twai_ctx);
    } else {
        // Hardware busy, need to queue the frame
        BaseType_t is_isr_context = xPortInIsrContext();
        BaseType_t yield_required = pdFALSE;

        if (is_isr_context) {
            // In ISR context - use ISR-safe queue operations
            ESP_RETURN_ON_FALSE_ISR(xQueueSendFromISR(twai_ctx->tx_mount_queue, &frame, &yield_required), ESP_ERR_TIMEOUT, TAG, "tx queue full");
        } else {
            // In task context - use normal queue operations
            ESP_RETURN_ON_FALSE(xQueueSend(twai_ctx->tx_mount_queue, &frame, ticks_to_wait), ESP_ERR_TIMEOUT, TAG, "tx queue full");
        }

        // Second chance check for hardware availability
        false_var = false;
        if (atomic_compare_exchange_strong(&twai_ctx->hw_busy, &false_var, true)) {
            BaseType_t dequeue_result;
            if (is_isr_context) {
                dequeue_result = xQueueReceiveFromISR(twai_ctx->tx_mount_queue, &twai_ctx->p_curr_tx, &yield_required);
            } else {
                dequeue_result = xQueueReceive(twai_ctx->tx_mount_queue, &twai_ctx->p_curr_tx, 0);
            }

            if (dequeue_result == pdTRUE) {
                _node_start_trans(twai_ctx);
            } else {
                assert(false && "should always get frame at this moment");
            }
        }

        // Handle ISR yield if required
        if (is_isr_context && yield_required) {
            portYIELD_FROM_ISR();
        }
    }
    return ESP_OK;
}

static esp_err_t _node_wait_tx_all_done(twai_node_handle_t node, int timeout)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    TickType_t ticks_to_wait = (timeout == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) != TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "node is bus off");

    // either hw_busy or tx_mount_queue is not empty, means tx is not finished
    // otherwise, hardware is idle, return immediately
    if (atomic_load(&twai_ctx->hw_busy) || uxQueueMessagesWaiting(twai_ctx->tx_mount_queue)) {
        //wait for idle event bit but without clear it, every tasks block here can be waked up
        if (TWAI_IDLE_EVENT_BIT != xEventGroupWaitBits(twai_ctx->event_group, TWAI_IDLE_EVENT_BIT, pdFALSE, pdFALSE, ticks_to_wait)) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

static esp_err_t _node_parse_rx(twai_node_handle_t node, twai_frame_t *rx_frame)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE_ISR(atomic_load(&twai_ctx->rx_isr), ESP_ERR_INVALID_STATE, TAG, "rx can only called in `rx_done` callback");
    assert(xPortInIsrContext() && "should always in rx_done callback");

    twai_hal_parse_frame(twai_ctx->hal, &twai_ctx->rcv_buff, &rx_frame->header, rx_frame->buffer, rx_frame->buffer_len);
    if (twai_ctx->timestamp_freq_hz && !rx_frame->header.timestamp) {
        // if timestamp not updated by hardware, use the esp_timer timestamp to calculate the timestamp
        rx_frame->header.timestamp = _time_us_to_timestamp(esp_timer_get_time(), twai_ctx->timestamp_freq_hz);
    }
    return ESP_OK;
}

/* --------------------------------- Public --------------------------------- */
esp_err_t twai_new_node_onchip(const twai_onchip_node_config_t *node_config, twai_node_handle_t *node_ret)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((node_config->tx_queue_depth > 0) || node_config->flags.enable_listen_only, ESP_ERR_INVALID_ARG, TAG, "tx_queue_depth at least 1");
    ESP_RETURN_ON_FALSE(!node_config->intr_priority || (BIT(node_config->intr_priority) & ESP_INTR_FLAG_LOWMED), ESP_ERR_INVALID_ARG, TAG, "Invalid intr_priority level");

    // Allocate TWAI node from internal memory because it contains atomic variable
    twai_onchip_ctx_t *node = heap_caps_calloc(1, sizeof(twai_onchip_ctx_t) + twai_hal_get_mem_requirment(), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    ESP_RETURN_ON_FALSE(node, ESP_ERR_NO_MEM, TAG, "No mem");
    node->ctrlr_id = -1;
    // Acquire controller
    int ctrlr_id = _ctrlr_acquire(node);
    ESP_GOTO_ON_FALSE(ctrlr_id != -1, ESP_ERR_NOT_FOUND, err, TAG, "Controller not available");
    node->ctrlr_id = ctrlr_id;
    node->hal = (twai_hal_context_t *)(node + 1);   //hal context is place at end of driver context
    node->curr_clk_src = node_config->clk_src ? node_config->clk_src : TWAI_CLK_SRC_DEFAULT;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz(node->curr_clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &node->src_freq_hz), err, TAG, "get clock source frequency failed");

    // state is in bus_off before enabled
    atomic_store(&node->state, TWAI_ERROR_BUS_OFF);
    node->tx_mount_queue = xQueueCreateWithCaps(node_config->tx_queue_depth, sizeof(twai_frame_t *), TWAI_MALLOC_CAPS);
    node->event_group = xEventGroupCreateWithCaps(TWAI_MALLOC_CAPS);
    ESP_GOTO_ON_FALSE((node->tx_mount_queue && node->event_group) || node_config->flags.enable_listen_only, ESP_ERR_NO_MEM, err, TAG, "no_mem");
    uint32_t intr_flags = TWAI_INTR_ALLOC_FLAGS;
    intr_flags |= (node_config->intr_priority > 0) ? BIT(node_config->intr_priority) : ESP_INTR_FLAG_LOWMED;
    _lock_acquire(&s_platform.intr_mutex);  // lock to prevent twai_intr and timer_intr registered to different cpu then triggered at the same time
    ESP_GOTO_ON_ERROR(esp_intr_alloc(twai_periph_signals[ctrlr_id].irq_id, intr_flags, _node_isr_main, (void *)node, &node->intr_hdl),
                      err, TAG, "Alloc interrupt failed");

    if (node_config->timestamp_resolution_hz) {
#if TWAI_LL_SUPPORT(TIMESTAMP)
        ESP_GOTO_ON_FALSE((node_config->timestamp_resolution_hz >= (node->src_freq_hz / TWAI_LL_TIMER_DIV_MAX)) && (node_config->timestamp_resolution_hz <= node->src_freq_hz), \
                          ESP_ERR_INVALID_ARG, err, TAG, "Timestamp resolution range [%d, %d]", node->src_freq_hz / TWAI_LL_TIMER_DIV_MAX, node->src_freq_hz);
        uint32_t real_timer_freq = node->src_freq_hz / (node->src_freq_hz / node_config->timestamp_resolution_hz);
        if (real_timer_freq != node_config->timestamp_resolution_hz) {
            ESP_LOGW(TAG, "timestamp resolution loss, adjust to %dHz", real_timer_freq);
        }
        // deal timer interrupt in same `_node_isr_main` handler and check timer event first
        // to avoid race condition if two hardware interrupts are triggered at the same time
        ESP_GOTO_ON_ERROR(esp_intr_alloc(twai_periph_signals[ctrlr_id].timer_irq_id, intr_flags, _node_isr_main, (void *)node, &node->timer_intr_hdl),
                          err, TAG, "Alloc timer interrupt failed");
#else
        ESP_GOTO_ON_FALSE(node_config->timestamp_resolution_hz <= 1000000, ESP_ERR_INVALID_ARG, err, TAG, "Timestamp resolution is at most 1MHz");
#endif
        node->timestamp_freq_hz = node_config->timestamp_resolution_hz;
    }
    _lock_release(&s_platform.intr_mutex);

#if CONFIG_PM_ENABLE
#if TWAI_LL_SUPPORT(APB_CLK)
    // DFS can change APB frequency. So add lock to prevent sleep and APB freq from changing
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, twai_periph_signals[ctrlr_id].module_name, &node->pm_lock), err, TAG, "init power manager failed");
#else // XTAL
    // XTAL freq can be closed in light sleep, so we need to create a lock to prevent light sleep
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, twai_periph_signals[ctrlr_id].module_name, &node->pm_lock), err, TAG, "init power manager failed");
#endif //TWAI_LL_SUPPORT(APB_CLK)
#endif //CONFIG_PM_ENABLE

    // Set clock source, enable bus clock and reset controller
    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src(node->curr_clk_src, true), TAG, "enable clock source failed");
    ESP_LOGD(TAG, "set clock source to %d, freq: %ld Hz", node->curr_clk_src, node->src_freq_hz);
    _twai_rcc_clock_sel(node->ctrlr_id, node->curr_clk_src);
    _twai_rcc_clock_ctrl(ctrlr_id, true);

    // Initialize HAL and configure register defaults.
    twai_hal_config_t hal_config = {
        .controller_id = node->ctrlr_id,
        .intr_mask = TWAI_LL_DRIVER_INTERRUPTS,
        .clock_source_hz = node->src_freq_hz,
        .timer_freq = node->timestamp_freq_hz,
        .retry_cnt = node_config->fail_retry_cnt,
        .no_receive_rtr = node_config->flags.no_receive_rtr,
        .enable_listen_only = node_config->flags.enable_listen_only,
        .enable_self_test = node_config->flags.enable_self_test,
        .enable_loopback = node_config->flags.enable_loopback,
    };
    ESP_GOTO_ON_FALSE(twai_hal_init(node->hal, &hal_config), ESP_ERR_INVALID_STATE, err, TAG, "hardware not in reset state");
    // Configure bus timing
    ESP_GOTO_ON_ERROR(_node_calc_set_bit_timing(&node->api_base, &node_config->bit_timing, &node_config->data_timing), err, TAG, "bitrate error");
    // Configure GPIO
    ESP_GOTO_ON_ERROR(_node_config_io(node, node_config), err, TAG, "gpio config failed");

    node->api_base.enable = _node_enable;
    node->api_base.disable = _node_disable;
    node->api_base.del = _node_delete;
    node->api_base.recover = _node_recover;
    node->api_base.config_mask_filter = _node_config_mask_filter;
#if SOC_TWAI_RANGE_FILTER_NUM
    node->api_base.config_range_filter = _node_config_range_filter;
#endif
    node->api_base.reconfig_timing = _node_set_bit_timing;
    node->api_base.register_cbs = _node_register_callbacks;
    node->api_base.transmit = _node_queue_tx;
    node->api_base.transmit_wait_done = _node_wait_tx_all_done;
    node->api_base.receive_isr = _node_parse_rx;
    node->api_base.get_info = _node_get_status;

    *node_ret = &node->api_base;
    return ESP_OK;
err:
    if (node) {
        _lock_release(&s_platform.intr_mutex);
        _node_destroy(node);
    }
    return ret;
}
