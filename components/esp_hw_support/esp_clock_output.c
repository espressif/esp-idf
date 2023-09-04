/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include "driver/gpio.h"
#include "esp_clock_output.h"
#include "esp_check.h"
#include "esp_rom_gpio.h"
#include "esp_private/clkout_channel.h"
#include "esp_private/startup_internal.h"
#include "hal/gpio_hal.h"
#include "soc/soc_caps.h"
#include "soc/io_mux_reg.h"

typedef struct gpio_clock_out_ctx {
    bool is_mapped;
    uint8_t ref_cnt;
    clock_out_channel_t clk_out;
    gpio_num_t mapped_io;
    soc_clkout_sig_id_t mapped_clock;
} gpio_clock_out_ctx_t;

static const char *TAG = "gpio_output_clk";

static portMUX_TYPE s_clkout_channel_lock = portMUX_INITIALIZER_UNLOCKED;

static gpio_clock_out_ctx_t s_pin_ctrl_clk_out[CLKOUT_CHANNEL_MAX] = {
    [0 ... CLKOUT_CHANNEL_MAX - 1] = {
        .is_mapped = false,
        .ref_cnt = 0,
    }
};

static gpio_clock_out_ctx_t* io_mux_pin_ctrl_clk_out_alloc(soc_clkout_sig_id_t clk_sig, gpio_num_t gpio_num)
{
    gpio_clock_out_ctx_t *allocated_clk_out = NULL;

#if SOC_GPIO_CLOCKOUT_BY_IO_MUX
    portENTER_CRITICAL(&s_clkout_channel_lock);
    if (!s_pin_ctrl_clk_out[IONUM_TO_CLKOUT(gpio_num)].is_mapped) {
        s_pin_ctrl_clk_out[IONUM_TO_CLKOUT(gpio_num)].is_mapped = true;
        s_pin_ctrl_clk_out[IONUM_TO_CLKOUT(gpio_num)].clk_out = IONUM_TO_CLKOUT(gpio_num);
        allocated_clk_out = &s_pin_ctrl_clk_out[IONUM_TO_CLKOUT(gpio_num)];
    } else if ((s_pin_ctrl_clk_out[IONUM_TO_CLKOUT(gpio_num)].mapped_io == gpio_num) &&
        (s_pin_ctrl_clk_out[IONUM_TO_CLKOUT(gpio_num)].mapped_clock == clk_sig)) {
        allocated_clk_out = &s_pin_ctrl_clk_out[IONUM_TO_CLKOUT(gpio_num)];
    }
    portEXIT_CRITICAL(&s_clkout_channel_lock);
#elif SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX
    for(uint32_t i = 0; i < CLKOUT_CHANNEL_MAX; i++) {
        portENTER_CRITICAL(&s_clkout_channel_lock);
        if (!s_pin_ctrl_clk_out[i].is_mapped) {
            s_pin_ctrl_clk_out[i].is_mapped = true;
            s_pin_ctrl_clk_out[i].clk_out = (clock_out_channel_t)i;
            allocated_clk_out = &s_pin_ctrl_clk_out[i];
            portEXIT_CRITICAL(&s_clkout_channel_lock);
            break;
        } else if ((s_pin_ctrl_clk_out[i].mapped_io == gpio_num) &&
            (s_pin_ctrl_clk_out[i].mapped_clock == clk_sig)) {
            allocated_clk_out = &s_pin_ctrl_clk_out[i];
            portEXIT_CRITICAL(&s_clkout_channel_lock);
            break;
        }
        portEXIT_CRITICAL(&s_clkout_channel_lock);
    }
#endif

    if (allocated_clk_out != NULL) {
        portENTER_CRITICAL(&s_clkout_channel_lock);
        allocated_clk_out->mapped_io = gpio_num;
        allocated_clk_out->mapped_clock = clk_sig;
        allocated_clk_out->ref_cnt++;
        portEXIT_CRITICAL(&s_clkout_channel_lock);
    }

    return allocated_clk_out;
}

static bool io_mux_pin_ctrl_clk_out_sig_try_free(gpio_clock_out_handle_t clk_out_hdl)
{
    bool do_free = false;
    portENTER_CRITICAL(&s_clkout_channel_lock);
    if (--clk_out_hdl->ref_cnt == 0) {
        do_free = true;
        clk_out_hdl->is_mapped = false;
    }
    portEXIT_CRITICAL(&s_clkout_channel_lock);
    return do_free;
}

esp_err_t esp_clock_output_start(soc_clkout_sig_id_t clk_sig, gpio_num_t gpio_num, gpio_clock_out_handle_t *clk_out_hdl)
{
    ESP_RETURN_ON_FALSE((clk_out_hdl != NULL), ESP_ERR_INVALID_ARG, TAG, "Clock out handle passed in is invalid");

    ESP_RETURN_ON_FALSE(IS_VALID_CLKOUT_IO(gpio_num), ESP_ERR_INVALID_ARG, TAG, "%s", "Output GPIO number error");

     gpio_clock_out_ctx_t* new_hdl= io_mux_pin_ctrl_clk_out_alloc(clk_sig, gpio_num);

#if SOC_GPIO_CLOCKOUT_BY_IO_MUX
    ESP_RETURN_ON_FALSE((new_hdl != NULL), ESP_ERR_INVALID_ARG, TAG, "Selected clock out IO is already mapped to other internal clock source");
#elif SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX
    ESP_RETURN_ON_FALSE((new_hdl != NULL), ESP_FAIL, TAG, "Maximum support for %d output clock signals, no available clock_out channel for assignment", CLKOUT_CHANNEL_MAX);
#endif

    if (new_hdl->ref_cnt == 1) {
        uint32_t clk_out_mask = (new_hdl->clk_out == CLKOUT_CHANNEL_1) ? CLK_OUT1 :
                                (new_hdl->clk_out == CLKOUT_CHANNEL_2) ? CLK_OUT2 :
                                (new_hdl->clk_out == CLKOUT_CHANNEL_3) ? CLK_OUT3 : 0;
        uint32_t clk_out_shift =    (new_hdl->clk_out == CLKOUT_CHANNEL_1) ? CLK_OUT1_S :
                                    (new_hdl->clk_out == CLKOUT_CHANNEL_2) ? CLK_OUT2_S :
                                    (new_hdl->clk_out == CLKOUT_CHANNEL_3) ? CLK_OUT3_S : 0;

        portENTER_CRITICAL(&s_clkout_channel_lock);
        gpio_ll_set_pin_ctrl(clk_sig, clk_out_mask, clk_out_shift);
        portEXIT_CRITICAL(&s_clkout_channel_lock);

#if SOC_GPIO_CLOCKOUT_BY_IO_MUX
        uint32_t clk_out_func = (new_hdl->clk_out == CLKOUT_CHANNEL_1) ? FUNC_CLK_OUT1 :
                                (new_hdl->clk_out == CLKOUT_CHANNEL_2) ? FUNC_CLK_OUT2 :
                                (new_hdl->clk_out == CLKOUT_CHANNEL_3) ? FUNC_CLK_OUT3 : 0;

        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], clk_out_func);
#elif SOC_GPIO_CLOCKOUT_BY_GPIO_MATRIX
        uint32_t gpio_clk_out_sig_idx = (new_hdl->clk_out == CLKOUT_CHANNEL_1) ? CLK_OUT_OUT1_IDX :
                                        (new_hdl->clk_out == CLKOUT_CHANNEL_2) ? CLK_OUT_OUT2_IDX :
                                        (new_hdl->clk_out == CLKOUT_CHANNEL_3) ? CLK_OUT_OUT3_IDX : SIG_GPIO_OUT_IDX;

        gpio_set_pull_mode(gpio_num, GPIO_FLOATING);
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_num], PIN_FUNC_GPIO);
        gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
        esp_rom_gpio_connect_out_signal(gpio_num, gpio_clk_out_sig_idx, false, false);
#endif
    }

    *clk_out_hdl = new_hdl;
    return ESP_OK;
}

esp_err_t esp_clock_output_stop(gpio_clock_out_handle_t clk_out_hdl)
{
    assert(clk_out_hdl != NULL);
    ESP_RETURN_ON_FALSE(clk_out_hdl->is_mapped, ESP_ERR_INVALID_STATE, TAG, "%s", "Clock outputting is already disabled");
    if (io_mux_pin_ctrl_clk_out_sig_try_free(clk_out_hdl)) {
        gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[clk_out_hdl->mapped_io], PIN_FUNC_GPIO);
        esp_rom_gpio_connect_out_signal(clk_out_hdl->mapped_io, SIG_GPIO_OUT_IDX, false, false);
        gpio_set_direction(clk_out_hdl->mapped_io, GPIO_MODE_DISABLE);
    }
    return ESP_OK;
}

#if CONFIG_IDF_TARGET_ESP32
// Due to a hardware bug, PIN_CTRL cannot select 0xf output, whereas 0xf is the default value.
__attribute__((constructor))
static void esp_clock_output_pin_ctrl_init(void)
{
    gpio_ll_set_pin_ctrl(0, CLK_OUT1, CLK_OUT1_S);
    gpio_ll_set_pin_ctrl(0, CLK_OUT2, CLK_OUT2_S);
    gpio_ll_set_pin_ctrl(0, CLK_OUT3, CLK_OUT3_S);
}
#endif
