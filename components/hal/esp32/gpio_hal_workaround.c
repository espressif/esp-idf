/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for GPIO (common part)
//
#include "esp_attr.h"
#include "soc/soc.h"
#include "hal/gpio_hal.h"
#include "hal/rtc_io_ll.h"
#include "soc/soc_caps.h"

#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
typedef struct gpio_slp_mode_cfg {
    volatile uint32_t fun_pu;
    volatile uint32_t fun_pd;
} gpio_slp_mode_cfg_t;

static void gpio_hal_sleep_mode_setup_wrapper(
        gpio_hal_context_t *hal,
        uint32_t gpio_num,
        void (*opt)(gpio_hal_context_t *, uint32_t, void *)
    )
{
    static DRAM_ATTR gpio_slp_mode_cfg_t gpio_cfg;
    if (opt) {
        (*opt)(hal, gpio_num, (void *)&gpio_cfg);
    }
}

/**
 * @brief GPIO pu/pd information backup function
 * @param hal gpio hal
 * @param  gpio_num gpio num
 * @param args pointer for bitmap to backup GPIO pu/pd information
 */
static void gpio_hal_fun_pupd_backup(gpio_hal_context_t *hal, uint32_t gpio_num, void *args)
{
    /* On ESP32, setting SLP_PU, SLP_PD couldn't change GPIO status
     * from FUN_PU, FUN_PD to SLP_PU, SLP_PD at sleep.
     * On the ESP32S2, it does.
     * The following code emulates ESP32S2`s behavior:
     */
    gpio_slp_mode_cfg_t *pcfg = (gpio_slp_mode_cfg_t *)args;

    int rtcio_num = rtc_io_num_map[gpio_num];
    if (rtcio_num >= 0 && gpio_ll_sleep_sel_is_enabled(hal->dev, gpio_num)) {
        if (rtcio_ll_is_pullup_enabled(rtcio_num)) {
            pcfg->fun_pu |= BIT(rtcio_num);
        } else {
            pcfg->fun_pu &= ~BIT(rtcio_num);
        }
        if (rtcio_ll_is_pulldown_enabled(rtcio_num)) {
            pcfg->fun_pd |= BIT(rtcio_num);
        } else {
            pcfg->fun_pd &= ~BIT(rtcio_num);
        }

        if (gpio_ll_sleep_pullup_is_enabled(hal->dev, gpio_num)) {
            rtcio_ll_pullup_enable(rtcio_num);
        } else {
            rtcio_ll_pullup_disable(rtcio_num);
        }
        if (gpio_ll_sleep_pulldown_is_enabled(hal->dev, gpio_num)) {
            rtcio_ll_pulldown_enable(rtcio_num);
        } else {
            rtcio_ll_pulldown_disable(rtcio_num);
        }
    }
}

/**
 * @brief GPIO pu/pd information backup function
 * @param hal gpio hal
 * @param  gpio_num gpio num
 * @param args pointer for bitmap to restore GPIO pu/pd information
 */
static void gpio_hal_fun_pupd_restore(gpio_hal_context_t *hal, uint32_t gpio_num, void *args)
{
    /* On ESP32, setting SLP_PU, SLP_PD couldn't change GPIO status
     * from SLP_PU, SLP_PD to FUN_PU, FUN_PD when it wakes up.
     * On the ESP32S2, it does.
     * The following code emulates ESP32S2`s behavior:
     */
    gpio_slp_mode_cfg_t *pcfg = (gpio_slp_mode_cfg_t *)args;

    int rtcio_num = rtc_io_num_map[gpio_num];
    if (rtcio_num >= 0 && gpio_ll_sleep_sel_is_enabled(hal->dev, gpio_num)) {
        if (pcfg->fun_pu & BIT(rtcio_num)) {
            rtcio_ll_pullup_enable(rtcio_num);
        } else {
            rtcio_ll_pullup_disable(rtcio_num);
        }
        if (pcfg->fun_pd & BIT(rtcio_num)) {
            rtcio_ll_pulldown_enable(rtcio_num);
        } else {
            rtcio_ll_pulldown_disable(rtcio_num);
        }
    }
}

void gpio_hal_sleep_pupd_config_apply(gpio_hal_context_t *hal, uint32_t gpio_num)
{
    gpio_hal_sleep_mode_setup_wrapper(hal, gpio_num, gpio_hal_fun_pupd_backup);
}

void gpio_hal_sleep_pupd_config_unapply(gpio_hal_context_t *hal, uint32_t gpio_num)
{
    gpio_hal_sleep_mode_setup_wrapper(hal, gpio_num, gpio_hal_fun_pupd_restore);
}
#endif
