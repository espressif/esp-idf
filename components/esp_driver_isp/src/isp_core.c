/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "esp_clk_tree.h"
#include "driver/isp_core.h"
#include "driver/isp_bf.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/mipi_csi_share_hw_ctrl.h"
#include "hal/hal_utils.h"
#include "hal/color_hal.h"
#include "soc/mipi_csi_bridge_struct.h"
#include "soc/isp_periph.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_private/isp_private.h"

#define ISP_DIV_ROUND_UP(x, y)    (((x) + (y) - 1) / (y))

typedef struct isp_platform_t {
    _lock_t         mutex;
    isp_processor_t *processors[SOC_ISP_NUMS];
} isp_platform_t;

static const char *TAG = "ISP";
static isp_platform_t s_platform;

static esp_err_t s_isp_claim_processor(isp_processor_t *proc)
{
    assert(proc);

    _lock_acquire(&s_platform.mutex);
    bool found = false;
    for (int i = 0; i < SOC_ISP_NUMS; i ++) {
        found = !s_platform.processors[i];
        if (found) {
            s_platform.processors[i] = proc;
            proc->proc_id = i;
            PERIPH_RCC_ATOMIC() {
                isp_ll_enable_module_clock(proc->hal.hw, true);
                isp_ll_reset_module_clock(proc->hal.hw);
            }
            break;
        }
    }
    _lock_release(&s_platform.mutex);

    if (!found) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

static esp_err_t s_isp_declaim_processor(isp_processor_t *proc)
{
    assert(proc);

    _lock_acquire(&s_platform.mutex);
    s_platform.processors[proc->proc_id] = NULL;
    PERIPH_RCC_ATOMIC() {
        isp_ll_enable_module_clock(proc->hal.hw, false);
    }
    _lock_release(&s_platform.mutex);

    return ESP_OK;
}

esp_err_t esp_isp_new_processor(const esp_isp_processor_cfg_t *proc_config, isp_proc_handle_t *ret_proc)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(proc_config && ret_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc_config->input_data_source != ISP_INPUT_DATA_SOURCE_DWGDMA, ESP_ERR_NOT_SUPPORTED, TAG, "input source not supported yet");
    if (proc_config->flags.bypass_isp) {
        ESP_RETURN_ON_FALSE(proc_config->input_data_color_type == proc_config->output_data_color_type, ESP_ERR_INVALID_ARG, TAG, "isp is bypassed, input and output data color type should be same");
    }

    isp_processor_t *proc = heap_caps_calloc(1, sizeof(isp_processor_t), ISP_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_NO_MEM, TAG, "no mem");

    //claim a processor, then do assignment
    ESP_GOTO_ON_ERROR(s_isp_claim_processor(proc), err, TAG, "no available isp processor");
#if SOC_ISP_SHARE_CSI_BRG
    ESP_GOTO_ON_ERROR(mipi_csi_brg_claim(MIPI_CSI_BRG_USER_SHARE, &proc->csi_brg_id), err, TAG, "csi bridge is in use already");
#endif

    isp_clk_src_t clk_src = !proc_config->clk_src ? ISP_CLK_SRC_DEFAULT : proc_config->clk_src;
    uint32_t clk_src_freq_hz = 0;
    ESP_GOTO_ON_ERROR(esp_clk_tree_src_get_freq_hz(clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz), err, TAG, "clock source setting fail");
    ESP_GOTO_ON_FALSE((proc_config->clk_hz > 0 && proc_config->clk_hz <= clk_src_freq_hz), ESP_ERR_INVALID_ARG, err, TAG, "clk hz not supported");

    proc->intr_priority = (proc_config->intr_priority > 0 && proc_config->intr_priority <= 3) ? BIT(proc_config->intr_priority) : ESP_INTR_FLAG_LOWMED;

    uint32_t out_clk_freq_hz = 0;
    hal_utils_clk_div_t clk_div = {};
    hal_utils_clk_info_t clk_info = {
        .src_freq_hz = clk_src_freq_hz,
        .exp_freq_hz = proc_config->clk_hz,
        .max_integ = ISP_LL_TX_MAX_CLK_INT_DIV,
        .min_integ = 1,
        .round_opt = HAL_DIV_ROUND,
    };
    out_clk_freq_hz = hal_utils_calc_clk_div_integer(&clk_info, &clk_div.integer);
    if (out_clk_freq_hz != proc_config->clk_hz) {
        ESP_LOGW(TAG, "precision loss, real output frequency: %"PRIu32"Hz", out_clk_freq_hz);
    }
    isp_hal_init(&proc->hal, proc->proc_id);
    ESP_GOTO_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)clk_src, true), err, TAG, "clock source enable failed");
    PERIPH_RCC_ATOMIC() {
        isp_ll_select_clk_source(proc->hal.hw, clk_src);
        isp_ll_set_clock_div(proc->hal.hw, &clk_div);
    }
    proc->clk_src = clk_src;
    proc->isp_fsm = ISP_FSM_INIT;
    proc->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;

    //Input & Output color format
    color_space_pixel_format_t in_color_format = {
        .color_type_id = proc_config->input_data_color_type,
    };
    color_space_pixel_format_t out_color_format = {
        .color_type_id = proc_config->output_data_color_type,
    };
    int in_bits_per_pixel = color_hal_pixel_format_get_bit_depth(in_color_format);

    if (!proc_config->flags.bypass_isp) {
        bool valid_format = false;
        valid_format = isp_ll_set_input_data_color_format(proc->hal.hw, in_color_format);
        ESP_GOTO_ON_FALSE(valid_format, ESP_ERR_INVALID_ARG, err, TAG, "invalid input color space config");

        valid_format = false;
        valid_format = isp_ll_set_output_data_color_format(proc->hal.hw, out_color_format);
        ESP_GOTO_ON_FALSE(valid_format, ESP_ERR_INVALID_ARG, err, TAG, "invalid output color space config");
    }

    isp_ll_clk_enable(proc->hal.hw, true);
    isp_ll_set_input_data_source(proc->hal.hw, proc_config->input_data_source);
    isp_ll_enable_line_start_packet_exist(proc->hal.hw, proc_config->has_line_start_packet);
    isp_ll_enable_line_end_packet_exist(proc->hal.hw, proc_config->has_line_end_packet);
    if (proc_config->flags.bypass_isp) {
        /**
         * When ISP bypass, input module is still working, input Hsize needs to be re-calculated
         * according to input bits per pixel and IDI32.
         * Hsize now stands for the number of 32-bit in one line.
         */
        isp_ll_set_intput_data_h_pixel_num(proc->hal.hw, ISP_DIV_ROUND_UP(proc_config->h_res * in_bits_per_pixel, 32));
    } else {
        isp_ll_set_intput_data_h_pixel_num(proc->hal.hw, proc_config->h_res);
    }
    isp_ll_set_intput_data_v_row_num(proc->hal.hw, proc_config->v_res);
    isp_ll_set_bayer_mode(proc->hal.hw, proc_config->bayer_order);
    isp_ll_yuv_set_std(proc->hal.hw, proc_config->yuv_std);
    if (out_color_format.color_space == COLOR_SPACE_YUV) {
        isp_ll_yuv_set_range(proc->hal.hw, proc_config->yuv_range);
    }

    if (out_color_format.color_space == COLOR_SPACE_RGB && proc_config->input_data_source == ISP_INPUT_DATA_SOURCE_DVP) {
        isp_ll_color_enable(proc->hal.hw, true); // workaround for DIG-474
    }
    if (proc_config->flags.byte_swap_en) {
        isp_ll_set_byte_swap(proc->hal.hw, true);
    }

    proc->in_color_format = in_color_format;
    proc->out_color_format = out_color_format;
    proc->h_res = proc_config->h_res;
    proc->v_res = proc_config->v_res;
    proc->bayer_order = proc_config->bayer_order;
    proc->bypass_isp = proc_config->flags.bypass_isp;

    *ret_proc = proc;

    return ESP_OK;

err:
    free(proc);

    return ret;
}

esp_err_t esp_isp_del_processor(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->isp_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "processor isn't in init state");

    //declaim first, then do free
    ESP_RETURN_ON_ERROR(s_isp_declaim_processor(proc), TAG, "declaim processor fail");
#if SOC_ISP_SHARE_CSI_BRG
    ESP_RETURN_ON_ERROR(mipi_csi_brg_declaim(proc->csi_brg_id), TAG, "declaim csi bridge fail");
#endif
    ESP_RETURN_ON_ERROR(esp_clk_tree_enable_src((soc_module_clk_t)(proc->clk_src), false), TAG, "clock source disable failed");
    free(proc);

    return ESP_OK;
}

esp_err_t esp_isp_register_event_callbacks(isp_proc_handle_t proc, const esp_isp_evt_cbs_t *cbs, void *user_data)
{
    ESP_RETURN_ON_FALSE(proc && cbs, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(proc->isp_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "processor isn't in the init state");

#if CONFIG_ISP_ISR_IRAM_SAFE
    if (cbs->on_sharpen_frame_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_sharpen_frame_done), ESP_ERR_INVALID_ARG, TAG, "on_sharpen_frame_done callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif
    proc->cbs.on_sharpen_frame_done = cbs->on_sharpen_frame_done;
    proc->user_data = user_data;

    if (cbs->on_sharpen_frame_done) {
        ESP_RETURN_ON_ERROR(esp_isp_register_isr(proc, ISP_SUBMODULE_SHARPEN), TAG, "fail to register ISR");
    } else {
        ESP_RETURN_ON_ERROR(esp_isp_deregister_isr(proc, ISP_SUBMODULE_SHARPEN), TAG, "fail to deregister ISR");
    }

    return ESP_OK;
}

esp_err_t esp_isp_enable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->isp_fsm == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "processor isn't in init state");
    ESP_RETURN_ON_FALSE(proc->bypass_isp == false, ESP_ERR_INVALID_STATE, TAG, "processor is configured to be bypassed");

    isp_ll_enable(proc->hal.hw, true);
    proc->isp_fsm = ISP_FSM_ENABLE;

    return ESP_OK;
}

esp_err_t esp_isp_disable(isp_proc_handle_t proc)
{
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(proc->isp_fsm == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "processor isn't in enable state");

    isp_ll_enable(proc->hal.hw, false);
    proc->isp_fsm = ISP_FSM_INIT;

    return ESP_OK;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
static void IRAM_ATTR s_isp_isr_dispatcher(void *arg)
{
    isp_processor_t *proc = (isp_processor_t *)arg;
    bool need_yield = false;

    //Check and clear hw events
    uint32_t af_events = isp_hal_check_clear_intr_event(&proc->hal, ISP_LL_EVENT_AF_MASK);
    uint32_t awb_events = isp_hal_check_clear_intr_event(&proc->hal, ISP_LL_EVENT_AWB_MASK);
    uint32_t ae_events = isp_hal_check_clear_intr_event(&proc->hal, ISP_LL_EVENT_AE_MASK);
    uint32_t sharp_events = isp_hal_check_clear_intr_event(&proc->hal, ISP_LL_EVENT_SHARP_MASK);
    uint32_t hist_events = isp_hal_check_clear_intr_event(&proc->hal, ISP_LL_EVENT_HIST_MASK);

    bool do_dispatch = false;
    //Deal with hw events
    if (af_events) {
        portENTER_CRITICAL_ISR(&proc->spinlock);
        do_dispatch = proc->isr_users.af_isr_added;
        portEXIT_CRITICAL_ISR(&proc->spinlock);

        if (do_dispatch) {
            need_yield |= esp_isp_af_isr(proc, af_events);
        }
        do_dispatch = false;
    }
    if (awb_events) {
        portENTER_CRITICAL_ISR(&proc->spinlock);
        do_dispatch = proc->isr_users.awb_isr_added;
        portEXIT_CRITICAL_ISR(&proc->spinlock);

        if (do_dispatch) {
            need_yield |= esp_isp_awb_isr(proc, awb_events);
        }
        do_dispatch = false;
    }
    if (ae_events) {
        portENTER_CRITICAL_ISR(&proc->spinlock);
        do_dispatch = proc->isr_users.ae_isr_added;
        portEXIT_CRITICAL_ISR(&proc->spinlock);

        if (do_dispatch) {
            need_yield |= esp_isp_ae_isr(proc, ae_events);
        }
        do_dispatch = false;
    }
    if (sharp_events) {
        portENTER_CRITICAL_ISR(&proc->spinlock);
        do_dispatch = proc->isr_users.sharp_isr_added;
        portEXIT_CRITICAL_ISR(&proc->spinlock);

        if (do_dispatch) {
            need_yield |= esp_isp_sharpen_isr(proc, sharp_events);
        }
        do_dispatch = false;
    }
    if (hist_events) {
        portENTER_CRITICAL_ISR(&proc->spinlock);
        do_dispatch = proc->isr_users.hist_isr_added;
        portEXIT_CRITICAL_ISR(&proc->spinlock);

        if (do_dispatch) {
            need_yield |= esp_isp_hist_isr(proc, hist_events);
        }
        do_dispatch = false;
    }
    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t esp_isp_register_isr(isp_proc_handle_t proc, isp_submodule_t submodule)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool do_alloc = false;
    portENTER_CRITICAL(&proc->spinlock);
    proc->isr_ref_counts++;
    if (proc->isr_ref_counts == 1) {
        assert(!proc->intr_hdl);
        do_alloc = true;
    }

    switch (submodule) {
    case ISP_SUBMODULE_AF:
        proc->isr_users.af_isr_added = true;
        break;
    case ISP_SUBMODULE_AWB:
        proc->isr_users.awb_isr_added = true;
        break;
    case ISP_SUBMODULE_AE:
        proc->isr_users.ae_isr_added = true;
        break;
    case ISP_SUBMODULE_SHARPEN:
        proc->isr_users.sharp_isr_added = true;
        break;
    case ISP_SUBMODULE_HIST:
        proc->isr_users.hist_isr_added = true;
        break;
    default:
        assert(false);
    }
    portEXIT_CRITICAL(&proc->spinlock);

    if (do_alloc) {

        uint32_t intr_st_reg_addr = isp_ll_get_intr_status_reg_addr(proc->hal.hw);
        uint32_t intr_st_mask = ISP_LL_EVENT_AF_MASK | ISP_LL_EVENT_AE_MASK | ISP_LL_EVENT_AWB_MASK | ISP_LL_EVENT_HIST_MASK;
        ret = esp_intr_alloc_intrstatus(isp_hw_info.instances[proc->proc_id].irq, ISP_INTR_ALLOC_FLAGS | proc->intr_priority, intr_st_reg_addr, intr_st_mask,
                                        s_isp_isr_dispatcher, (void *)proc, &proc->intr_hdl);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "no intr source");
            return ret;
        }
        esp_intr_enable(proc->intr_hdl);
    }

    return ESP_OK;
}

esp_err_t esp_isp_deregister_isr(isp_proc_handle_t proc, isp_submodule_t submodule)
{
    esp_err_t ret = ESP_FAIL;
    ESP_RETURN_ON_FALSE(proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool do_free = false;
    portENTER_CRITICAL(&proc->spinlock);
    proc->isr_ref_counts--;
    assert(proc->isr_ref_counts >= 0);
    if (proc->isr_ref_counts == 0) {
        assert(proc->intr_hdl);
        do_free = true;
    }

    switch (submodule) {
    case ISP_SUBMODULE_AF:
        proc->isr_users.af_isr_added = false;
        break;
    case ISP_SUBMODULE_AWB:
        proc->isr_users.awb_isr_added = false;
        break;
    case ISP_SUBMODULE_AE:
        proc->isr_users.ae_isr_added = false;
        break;
    case ISP_SUBMODULE_SHARPEN:
        proc->isr_users.sharp_isr_added = false;
        break;
    case ISP_SUBMODULE_HIST:
        proc->isr_users.hist_isr_added = false;
        break;
    default:
        assert(false);
    }
    portEXIT_CRITICAL(&proc->spinlock);

    if (do_free) {
        esp_intr_disable(proc->intr_hdl);
        ret = esp_intr_free(proc->intr_hdl);
        if (ret != ESP_OK) {
            return ret;
        }
    }

    return ESP_OK;
}
