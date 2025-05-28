/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "esp_private/twai_interface.h"
#include "esp_private/twai_utils.h"
#include "twai_private.h"

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

#if SOC_TWAI_SUPPORT_FD
#include "hal/twaifd_ll.h"
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
static void _twai_rcc_clock_sel(uint8_t ctrlr_id, twai_clock_source_t clock)
{
    TWAI_PERI_ATOMIC() {
        twaifd_ll_set_clock_source(ctrlr_id, clock);
    }
}
#else
#include "hal/twai_ll.h"
static void _twai_rcc_clock_ctrl(uint8_t ctrlr_id, bool enable)
{
    TWAI_RCC_ATOMIC() {
        twai_ll_enable_bus_clock(ctrlr_id, enable);
        twai_ll_reset_register(ctrlr_id);
    }
    TWAI_PERI_ATOMIC() {
        twai_ll_enable_clock(ctrlr_id, enable);
    }
}
static void _twai_rcc_clock_sel(uint8_t ctrlr_id, twai_clock_source_t clock)
{
    TWAI_PERI_ATOMIC() {
        twai_ll_set_clock_source(ctrlr_id, clock);
    }
}
#endif //SOC_TWAI_SUPPORT_FD

typedef struct {
    struct twai_node_base api_base;
    int ctrlr_id;
    uint64_t gpio_reserved;
    twai_hal_context_t *hal;
    intr_handle_t intr_hdl;
    QueueHandle_t tx_mount_queue;
    twai_clock_source_t curr_clk_src;
    uint32_t valid_fd_timing;
    twai_event_callbacks_t cbs;
    void *user_data;
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_handle_t pm_lock;
#endif

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

#if CONFIG_IDF_TARGET_ESP32 // only esp32 have this errata, TODO: IDF-13002 check errata runtime
    if (events & TWAI_HAL_EVENT_NEED_PERIPH_RESET) {
        twai_hal_prepare_for_reset(twai_ctx->hal);
        TWAI_RCC_ATOMIC() {
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
    }

    // deal RX event, then TX later, TODO: DIG-620
    if (events & TWAI_HAL_EVENT_RX_BUFF_FRAME) {
        while (twai_hal_get_rx_msg_count(twai_ctx->hal)) {
            if (twai_hal_read_rx_fifo(twai_ctx->hal, &twai_ctx->rcv_buff)) {
#if !SOC_TWAI_SUPPORT_FD
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
#if !SOC_TWAI_SUPPORTS_RX_STATUS
                twai_hal_clear_rx_fifo_overrun(twai_ctx->hal);
                break;
#endif
            }
        }
    }

    // deal TX event
    if (events & TWAI_HAL_EVENT_TX_BUFF_FREE) {
        // only call tx_done_cb when tx without error, otherwise on_error_cb should triggered if it is registered
        if (twai_ctx->cbs.on_tx_done && (events & TWAI_HAL_EVENT_TX_SUCCESS)) {
            twai_tx_done_event_data_t tx_ev = {
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
    if (twai_ctx->tx_mount_queue) {
        vQueueDeleteWithCaps(twai_ctx->tx_mount_queue);
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

static esp_err_t _node_check_timing_valid(twai_onchip_ctx_t *twai_ctx, const twai_timing_advanced_config_t *timing, uint32_t source_freq)
{
    if (!timing) {
        return ESP_OK;
    }
    ESP_RETURN_ON_FALSE(!timing->quanta_resolution_hz, ESP_ERR_INVALID_ARG, TAG, "quanta_resolution_hz is not supported");  //TODO: IDF-12725
    ESP_RETURN_ON_FALSE(twai_hal_check_brp_validation(twai_ctx->hal, timing->brp), ESP_ERR_INVALID_ARG, TAG, "invalid brp");
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
            ESP_RETURN_ON_FALSE(!twai_ctx->valid_fd_timing || !timing->clk_src || (timing->clk_src == TWAI_CLK_SRC_DEFAULT), ESP_ERR_INVALID_ARG, TAG, "don't change clk_src in single config");
            new_clock_src = timing->clk_src ? timing->clk_src : TWAI_CLK_SRC_DEFAULT;
        } else {
            ESP_RETURN_ON_FALSE(!timing_fd->clk_src || (timing_fd->clk_src == TWAI_CLK_SRC_DEFAULT), ESP_ERR_INVALID_ARG, TAG, "don't change clk_src in single config");
        }
    }
    uint32_t source_freq = 0;
    ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(new_clock_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_APPROX, &source_freq), TAG, "clock src error, can't get freq");
    ESP_RETURN_ON_ERROR(_node_check_timing_valid(twai_ctx, timing, source_freq), TAG, "invalid param");
    ESP_RETURN_ON_ERROR(_node_check_timing_valid(twai_ctx, timing_fd, source_freq), TAG, "invalid fd param");

    if (timing) {
        twai_hal_configure_timing(twai_ctx->hal, timing);
    }
#if SOC_TWAI_SUPPORT_FD
    if (timing_fd) {
        twai_ctx->valid_fd_timing = true;
        twai_hal_configure_timing_fd(twai_ctx->hal, timing_fd);
    }
#endif

    if (new_clock_src != twai_ctx->curr_clk_src) {
        // TODO: IDF-13144
        ESP_ERROR_CHECK(esp_clk_tree_enable_src((soc_module_clk_t)(new_clock_src), true));
        twai_ctx->curr_clk_src = new_clock_src;
        _twai_rcc_clock_sel(twai_ctx->ctrlr_id, new_clock_src);
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
    twai_timing_advanced_config_t *fd_cfg_ptr = NULL;
    twai_timing_constraint_t hw_const = {
        .brp_min = TWAI_LL_BRP_MIN,
        .brp_max = TWAI_LL_BRP_MAX,
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
    twai_timing_advanced_config_t timing_adv_fd = { .clk_src = root_clock_src, };
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
    ESP_RETURN_ON_ERROR(_node_set_bit_timing(node, &timing_adv, fd_cfg_ptr), TAG, "invalid timing param, bitrate can't achieve!");
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
    twai_hal_start(twai_ctx->hal);

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

    ESP_RETURN_ON_ERROR(esp_intr_disable(twai_ctx->intr_hdl), TAG, "disable interrupt failed");
    atomic_store(&twai_ctx->state, TWAI_ERROR_BUS_OFF);
    twai_hal_stop(twai_ctx->hal);
#if CONFIG_IDF_TARGET_ESP32 // only esp32 have this errata, TODO: IDF-13002 check errata runtime
    // when `disable` happens during hardware busy, the next RX frame is corrupted, a HW reset can fix it
    if (twai_hal_is_hw_busy(twai_ctx->hal)) {
        twai_hal_backup_config(twai_ctx->hal);
        TWAI_RCC_ATOMIC() {
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
#if SOC_TWAI_SUPPORT_FD
    // FD targets don't support Dual filter
    ESP_RETURN_ON_FALSE(!mask_cfg->dual_filter, ESP_ERR_NOT_SUPPORTED, TAG, "The target don't support Dual Filter");
#endif
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "config filter must when node stopped");

    twai_hal_configure_filter(twai_ctx->hal, filter_id, mask_cfg);
    return ESP_OK;
}

#if SOC_TWAI_RANGE_FILTER_NUM
static esp_err_t _node_config_range_filter(twai_node_handle_t node, uint8_t filter_id, const twai_range_filter_config_t *range_cfg)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE(filter_id < SOC_TWAI_RANGE_FILTER_NUM, ESP_ERR_INVALID_ARG, TAG, "Invalid range filter id %d", filter_id);
    ESP_RETURN_ON_FALSE(atomic_load(&twai_ctx->state) == TWAI_ERROR_BUS_OFF, ESP_ERR_INVALID_STATE, TAG, "config filter must when node stopped");

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
#if !SOC_TWAI_SUPPORT_FD
    ESP_RETURN_ON_FALSE(!frame->header.fdf || frame->buffer_len <= TWAI_FRAME_MAX_LEN, ESP_ERR_INVALID_ARG, TAG, "fdf flag or buffer_len not supported");
#endif
    ESP_RETURN_ON_FALSE(frame->buffer_len <= (frame->header.fdf ? TWAIFD_FRAME_MAX_LEN : TWAI_FRAME_MAX_LEN), ESP_ERR_INVALID_ARG, TAG, "illegal transfer length (buffer_len %ld)", frame->buffer_len);
    ESP_RETURN_ON_FALSE((!frame->header.brs) || (twai_ctx->valid_fd_timing), ESP_ERR_INVALID_ARG, TAG, "brs can't be used without config data_timing");
    ESP_RETURN_ON_FALSE(!twai_ctx->hal->enable_listen_only, ESP_ERR_NOT_SUPPORTED, TAG, "node is config as listen only");
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

static esp_err_t _node_parse_rx(twai_node_handle_t node, twai_frame_t *rx_frame)
{
    twai_onchip_ctx_t *twai_ctx = __containerof(node, twai_onchip_ctx_t, api_base);
    ESP_RETURN_ON_FALSE_ISR(atomic_load(&twai_ctx->rx_isr), ESP_ERR_INVALID_STATE, TAG, "rx can only called in `rx_done` callback");

    twai_hal_parse_frame(&twai_ctx->rcv_buff, &rx_frame->header, rx_frame->buffer, rx_frame->buffer_len);
    return ESP_OK;
}

/* --------------------------------- Public --------------------------------- */
esp_err_t twai_new_node_onchip(const twai_onchip_node_config_t *node_config, twai_node_handle_t *node_ret)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(node_config->tx_queue_depth > 0, ESP_ERR_INVALID_ARG, TAG, "tx_queue_depth at least 1");
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

    // state is in bus_off before enabled
    atomic_store(&node->state, TWAI_ERROR_BUS_OFF);
    node->tx_mount_queue = xQueueCreateWithCaps(node_config->tx_queue_depth, sizeof(twai_frame_t *), TWAI_MALLOC_CAPS);
    ESP_GOTO_ON_FALSE(node->tx_mount_queue, ESP_ERR_NO_MEM, err, TAG, "no_mem");
    uint32_t intr_flags = TWAI_INTR_ALLOC_FLAGS;
    intr_flags |= (node_config->intr_priority > 0) ? BIT(node_config->intr_priority) : ESP_INTR_FLAG_LOWMED;
    ESP_GOTO_ON_ERROR(esp_intr_alloc(twai_controller_periph_signals.controllers[ctrlr_id].irq_id, intr_flags, _node_isr_main, (void *)node, &node->intr_hdl),
                      err, TAG, "Alloc interrupt failed");

    // Enable bus clock and reset controller
    _twai_rcc_clock_ctrl(ctrlr_id, true);
    // Initialize HAL and configure register defaults.
    twai_hal_config_t hal_config = {
        .controller_id = node->ctrlr_id,
        .intr_mask = TWAI_LL_DRIVER_INTERRUPTS,
        .retry_cnt = node_config->fail_retry_cnt,
        .no_receive_rtr = node_config->flags.no_receive_rtr,
        .enable_listen_only = node_config->flags.enable_listen_only,
        .enable_self_test = node_config->flags.enable_self_test,
        .enable_loopback = node_config->flags.enable_loopback,
    };
    ESP_GOTO_ON_FALSE(twai_hal_init(node->hal, &hal_config), ESP_ERR_INVALID_STATE, err, TAG, "hardware not in reset state");
    // Configure bus timing
    ESP_GOTO_ON_ERROR(_node_calc_set_bit_timing(&node->api_base, node_config->clk_src, &node_config->bit_timing, &node_config->data_timing), err, TAG, "bitrate error");
    // Configure GPIO
    ESP_GOTO_ON_ERROR(_node_config_io(node, node_config), err, TAG, "gpio config failed");
#if CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, twai_controller_periph_signals.controllers[ctrlr_id].module_name, &node->pm_lock), err, TAG, "init power manager failed");
#endif //CONFIG_PM_ENABLE

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
    node->api_base.receive_isr = _node_parse_rx;
    node->api_base.get_info = _node_get_status;

    *node_ret = &node->api_base;
    return ESP_OK;

err:
    if (node) {
        _node_destroy(node);
    }
    return ret;
}
