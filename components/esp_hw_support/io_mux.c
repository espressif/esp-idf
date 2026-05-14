/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/io_mux.h"
#include "esp_private/critical_section.h"
#include "esp_private/periph_ctrl.h"
#include "soc/clk_tree_defs.h"
#include "hal/gpio_ll.h"

#if GPIO_LL_CLK_SRC_SELECTABLE

DEFINE_CRIT_SECTION_LOCK_STATIC(s_io_mux_clk_spinlock);

static soc_module_clk_t s_io_mux_clk_src = 0;
static uint32_t s_io_mux_clk_ref_count = 0;

#endif // GPIO_LL_CLK_SRC_SELECTABLE

esp_err_t io_mux_acquire_clock_source(soc_module_clk_t clk_src)
{
#if GPIO_LL_CLK_SRC_SELECTABLE
    bool clk_conflict = false;
    bool need_update_hw = false;

    esp_os_enter_critical(&s_io_mux_clk_spinlock);
    if (s_io_mux_clk_ref_count > 0 && s_io_mux_clk_src != clk_src) {
        clk_conflict = true;
    } else {
        if (s_io_mux_clk_ref_count == 0) {
            s_io_mux_clk_src = clk_src;
            need_update_hw = true;
        }
        s_io_mux_clk_ref_count++;
    }
    esp_os_exit_critical(&s_io_mux_clk_spinlock);

    if (clk_conflict) {
        return ESP_ERR_INVALID_STATE;
    }

    if (need_update_hw) {
        PERIPH_RCC_ATOMIC() {
            gpio_ll_iomux_set_clk_src(clk_src);
        }
    }

    return ESP_OK;
#else
    (void)clk_src;
    return ESP_OK;
#endif
}

esp_err_t io_mux_release_clock_source(soc_module_clk_t clk_src)
{
#if GPIO_LL_CLK_SRC_SELECTABLE
    bool invalid = false;
    bool need_revert_hw = false;

    esp_os_enter_critical(&s_io_mux_clk_spinlock);
    if (s_io_mux_clk_ref_count == 0 || s_io_mux_clk_src != clk_src) {
        invalid = true;
    } else {
        s_io_mux_clk_ref_count--;
        if (s_io_mux_clk_ref_count == 0) {
            s_io_mux_clk_src = 0;
            need_revert_hw = true;
        }
    }
    esp_os_exit_critical(&s_io_mux_clk_spinlock);

    if (invalid) {
        return ESP_ERR_INVALID_STATE;
    }

    if (need_revert_hw) {
        PERIPH_RCC_ATOMIC() {
            gpio_ll_iomux_set_clk_src(SOC_MOD_CLK_XTAL);
        }
    }

    return ESP_OK;
#else
    (void)clk_src;
    return ESP_OK;
#endif
}
