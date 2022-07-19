/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for GPIO (common part)
//
#include "esp_attr.h"
#include "soc/soc.h"
#include "hal/gpio_hal.h"
#include "soc/soc_caps.h"

#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
typedef struct gpio_slp_mode_cfg {
    volatile uint16_t fun_pu[((SOC_GPIO_PIN_COUNT-1) >> 4) + 1];
    volatile uint16_t fun_pd[((SOC_GPIO_PIN_COUNT-1) >> 4) + 1];
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
    /* On ESP32, setting SLP_PU, SLP_PD couldn`t change GPIO status
     * from FUN_PU, FUN_PD to SLP_PU, SLP_PD at sleep.
     * On the ESP32S2, it does.
     * The following code emulates ESP32S2`s behavior:
     */
    gpio_slp_mode_cfg_t *pcfg = (gpio_slp_mode_cfg_t *)args;

    if (gpio_ll_sleep_sel_is_enabled(hal->dev, gpio_num)) {
        /* Record fun_pu and fun_pd state in bitmap */
        if (gpio_ll_pullup_is_enabled(hal->dev, gpio_num)) {
            pcfg->fun_pu[gpio_num >> 4] |= BIT(gpio_num & 0xf);
        } else {
            pcfg->fun_pu[gpio_num >> 4] &= ~BIT(gpio_num & 0xf);
        }
        if (gpio_ll_pulldown_is_enabled(hal->dev, gpio_num)) {
            pcfg->fun_pd[gpio_num >> 4] |= BIT(gpio_num & 0xf);
        } else {
            pcfg->fun_pd[gpio_num >> 4] &= ~BIT(gpio_num & 0xf);
        }

        if (gpio_ll_sleep_pullup_is_enabled(hal->dev, gpio_num)) {
            gpio_ll_pullup_en(hal->dev, gpio_num);
        } else {
            gpio_ll_pullup_dis(hal->dev, gpio_num);
        }
        if (gpio_ll_sleep_pulldown_is_enabled(hal->dev, gpio_num)) {
            gpio_ll_pulldown_en(hal->dev, gpio_num);
        } else {
            gpio_ll_pulldown_dis(hal->dev, gpio_num);
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
    /* On ESP32, setting SLP_PU, SLP_PD couldn`t change GPIO status
     * from SLP_PU, SLP_PD to FUN_PU, FUN_PD when it wakes up.
     * On the ESP32S2, it does.
     * The following code emulates ESP32S2`s behavior:
     */
    gpio_slp_mode_cfg_t *pcfg = (gpio_slp_mode_cfg_t *)args;

    if (gpio_ll_sleep_sel_is_enabled(hal->dev, gpio_num)) {
        if (pcfg->fun_pu[gpio_num >> 4] & BIT(gpio_num & 0xf)) {
            gpio_ll_pullup_en(hal->dev, gpio_num);
        } else {
            gpio_ll_pullup_dis(hal->dev, gpio_num);
        }
        if (pcfg->fun_pd[gpio_num >> 4] & BIT(gpio_num & 0xf)) {
            gpio_ll_pulldown_en(hal->dev, gpio_num);
        } else {
            gpio_ll_pulldown_dis(hal->dev, gpio_num);
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
