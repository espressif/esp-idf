/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_modem_clock.h"
#include "modem/modem_clock_impl.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_sleep.h"
#include "hal/efuse_hal.h"
#include "hal/clk_tree_ll.h"
#if SOC_CLOCK_TREE_MANAGEMENT_SUPPORTED
#include "esp_private/esp_clk_tree_common.h"
#endif

modem_clock_context_t * __attribute__((weak)) IRAM_ATTR MODEM_CLOCK_instance(void)
{
    /* It should be explicitly defined in the internal RAM */
    static DRAM_ATTR modem_clock_hal_context_t modem_clock_hal = { .syscon_dev = NULL, .lpcon_dev = NULL };
    static DRAM_ATTR modem_clock_context_t modem_clock_context = {
        .hal = &modem_clock_hal, .lock = SPINLOCK_INITIALIZER,
        .dev = NULL,
#if SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG
        .icg_config = NULL,
#endif
#if SOC_WIFI_SUPPORTED
        .modem_status = MODEM_STATUS_IDLE,
#endif
        .lpclk_src = { [0 ... MODEM_LPCLK_SRC_MODULE_MAX - 1] = MODEM_CLOCK_LPCLK_SRC_INVALID }
    };
    if (modem_clock_context.dev == NULL) {
        modem_clock_context.dev = modem_clock_device_context();
    }
#if SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG
    if (modem_clock_context.icg_config == NULL) {
        modem_clock_context.icg_config = modem_clock_domain_icg_config();
    }
#endif
    if (modem_clock_hal.syscon_dev == NULL || modem_clock_hal.lpcon_dev == NULL) {
        modem_clock_hal.syscon_dev = &MODEM_SYSCON;
        modem_clock_hal.lpcon_dev = &MODEM_LPCON;
#if SOC_CLOCK_TREE_MANAGEMENT_SUPPORTED
        ESP_ERROR_CHECK(esp_clk_tree_enable_src(SOC_MOD_CLK_MODEM_APB, true));
#endif
    }
    return &modem_clock_context;
}

static IRAM_ATTR void config_wrapper(struct modem_clock_context *ctx, bool enable, void (*action)(struct modem_clock_context *, bool),
                                    int16_t *refcnt, uint16_t *flags)
{
    int16_t refs = 0;
    bool with_refcnt = (*flags) & REFS_FL_WITH_REFCNT;
    if (enable) {
        refs = with_refcnt ? (*refcnt)++ : refs;
    } else {
        refs = with_refcnt ? --(*refcnt) : refs;
    }
    if (refs == 0 || !with_refcnt) {
        (*action)(ctx, enable);
    }
    assert(refs >= 0);
}

static IRAM_ATTR void modem_clock_device_control(modem_clock_context_t *ctx, uint32_t dev_map, bool enable)
{
    esp_os_enter_critical_safe(&ctx->lock);
    for (int i = 0; dev_map; dev_map >>= 1, i++) {
        if (dev_map & BIT(0)) {
            ctx->dev->configure(ctx, i, enable, config_wrapper);
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
            if (enable && ctx->dev->check) {
                ESP_ERROR_CHECK((*ctx->dev->check)(ctx, i));
            }
#endif
        }
    }
    esp_os_exit_critical_safe(&ctx->lock);
}

void IRAM_ATTR modem_clock_module_mac_reset(shared_periph_module_t module)
{
    __attribute__((unused)) modem_clock_context_t *ctx = MODEM_CLOCK_instance();
    esp_os_enter_critical_safe(&ctx->lock);
    switch (module)
    {
#if SOC_WIFI_SUPPORTED
        case PERIPH_WIFI_MODULE:
            modem_syscon_ll_reset_wifimac(ctx->hal->syscon_dev);
            break;
#endif
#if SOC_BT_SUPPORTED
        case PERIPH_BT_MODULE:
            modem_syscon_ll_reset_btmac(ctx->hal->syscon_dev);
            modem_syscon_ll_reset_btmac_apb(ctx->hal->syscon_dev);
            modem_syscon_ll_reset_ble_timer(ctx->hal->syscon_dev);
            modem_syscon_ll_reset_modem_sec(ctx->hal->syscon_dev);
            break;
#endif
#if SOC_IEEE802154_SUPPORTED
        case PERIPH_IEEE802154_MODULE:
            modem_syscon_ll_reset_zbmac(ctx->hal->syscon_dev);
            modem_syscon_ll_reset_zbmac_apb(ctx->hal->syscon_dev);
            break;
#endif
        default:
            assert(0);
    }
    esp_os_exit_critical_safe(&ctx->lock);
}

#if SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG
static IRAM_ATTR void modem_clock_module_icg_map_init_all(void)
{
    esp_os_enter_critical_safe(&MODEM_CLOCK_instance()->lock);
    for (int domain = 0; domain < MODEM_CLOCK_DOMAIN_MAX; domain++) {
        uint32_t code = modem_clock_hal_get_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain);
        modem_clock_hal_set_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain, MODEM_CLOCK_instance()->icg_config[domain] | code);
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        assert((modem_clock_hal_get_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain) & code) == code);
#endif
    }
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);
}
#endif

void IRAM_ATTR modem_clock_module_enable(shared_periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
#if SOC_PM_SUPPORT_MODEM_CLOCK_DOMAIN_ICG
    modem_clock_module_icg_map_init_all();
#endif
    uint32_t deps = modem_clock_get_module_deps(module);
    modem_clock_device_control(MODEM_CLOCK_instance(), deps, true);
}

void IRAM_ATTR modem_clock_module_disable(shared_periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
    uint32_t deps = modem_clock_get_module_deps(module);
    modem_clock_device_control(MODEM_CLOCK_instance(), deps, false);
}

uint32_t IRAM_ATTR modem_clock_module_bits_get(shared_periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
    uint32_t val = 0;
    switch (module)
    {
#if SOC_WIFI_SUPPORTED
        case PERIPH_WIFI_MODULE:
#endif
#if SOC_BT_SUPPORTED
        case PERIPH_BT_MODULE:
#endif
#if SOC_IEEE802154_SUPPORTED
        case PERIPH_IEEE802154_MODULE:
#endif
        case PERIPH_PHY_MODULE:
            val = modem_syscon_ll_clk_conf1_get(MODEM_CLOCK_instance()->hal->syscon_dev);
        default:
            break;
    }
    return val;
}

#if SOC_WIFI_SUPPORTED
void modem_clock_configure_wifi_status(bool inited)
{
    esp_os_enter_critical_safe(&MODEM_CLOCK_instance()->lock);
    if (inited)
        MODEM_CLOCK_instance()->modem_status |= MODEM_STATUS_WIFI_INITED;
    else
        MODEM_CLOCK_instance()->modem_status &= ~MODEM_STATUS_WIFI_INITED;
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);
}
#endif

/* Map module to lpclk_src array index (only WIFI, BT, COEX support LP clock) */
static IRAM_ATTR modem_lpclk_src_module_t modem_clock_lpclk_src_module(shared_periph_module_t module)
{
#if SOC_WIFI_SUPPORTED
    if (module == PERIPH_WIFI_MODULE) {
        return MODEM_LPCLK_SRC_MODULE_WIFI;
    }
#endif
#if SOC_BT_SUPPORTED
    if (module == PERIPH_BT_MODULE) {
        return MODEM_LPCLK_SRC_MODULE_BT;
    }
#endif
    if (module == PERIPH_COEX_MODULE) {
        return MODEM_LPCLK_SRC_MODULE_COEX;
    }
    return MODEM_LPCLK_SRC_MODULE_MAX;  /* Invalid module */
}

void modem_clock_deselect_all_module_lp_clock_source(void)
{
#if SOC_WIFI_SUPPORTED
    modem_clock_hal_deselect_all_wifi_lpclk_source(MODEM_CLOCK_instance()->hal);
#endif
#if SOC_BT_SUPPORTED
    modem_clock_hal_deselect_all_ble_rtc_timer_lpclk_source(MODEM_CLOCK_instance()->hal);
#endif
    modem_clock_hal_deselect_all_coex_lpclk_source(MODEM_CLOCK_instance()->hal);
}

#if SOC_LIGHT_SLEEP_SUPPORTED
static inline esp_sleep_pd_domain_t modem_clock_lpclk_src_power_domain(modem_clock_lpclk_src_t src)
{
    return (esp_sleep_pd_domain_t) (
              (src == MODEM_CLOCK_LPCLK_SRC_RC_FAST)   ? ESP_PD_DOMAIN_RC_FAST :
              (src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ? ESP_PD_DOMAIN_XTAL    :
#if !CONFIG_ESP_CLK_RC32K_NOT_TO_USE
              (src == MODEM_CLOCK_LPCLK_SRC_RC32K)     ? ESP_PD_DOMAIN_RC32K   :
#endif
              (src == MODEM_CLOCK_LPCLK_SRC_XTAL32K)   ? ESP_PD_DOMAIN_XTAL32K :
              ESP_PD_DOMAIN_MAX);
}
#endif

void modem_clock_select_lp_clock_source(shared_periph_module_t module, modem_clock_lpclk_src_t src, uint32_t divider)
{
    assert(IS_MODEM_MODULE(module));

    modem_lpclk_src_module_t lpclk_src_module = modem_clock_lpclk_src_module(module);
    if (lpclk_src_module >= MODEM_LPCLK_SRC_MODULE_MAX) {
        return;
    }

    esp_os_enter_critical_safe(&MODEM_CLOCK_instance()->lock);
    switch (module)
    {
#if SOC_WIFI_SUPPORTED
    case PERIPH_WIFI_MODULE:
        modem_clock_hal_deselect_all_wifi_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_select_wifi_lpclk_source(MODEM_CLOCK_instance()->hal, src);
        modem_lpcon_ll_set_wifi_lpclk_divisor_value(MODEM_CLOCK_instance()->hal->lpcon_dev, divider);
        modem_clock_hal_enable_wifipwr_clock(MODEM_CLOCK_instance()->hal, true);
        break;
#endif // SOC_WIFI_SUPPORTED

#if SOC_BT_SUPPORTED
    case PERIPH_BT_MODULE:
#if SOC_MODEM_CLOCK_BLE_RTC_TIMER_WORKAROUND
        modem_clock_select_ble_rtc_timer_clk_workaround(MODEM_CLOCK_instance(), true, src);
#endif
        modem_clock_hal_deselect_all_ble_rtc_timer_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_select_ble_rtc_timer_lpclk_source(MODEM_CLOCK_instance()->hal, src);
        modem_clock_hal_set_ble_rtc_timer_divisor_value(MODEM_CLOCK_instance()->hal, divider);
        modem_clock_hal_enable_ble_rtc_timer_clock(MODEM_CLOCK_instance()->hal, true);
#if SOC_MODEM_CLOCK_BLE_RTC_TIMER_WORKAROUND
        modem_clock_select_ble_rtc_timer_clk_workaround(MODEM_CLOCK_instance(), false, src);
#endif
#if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND
        modem_clock_bt_wifipwr_clk_workaround(MODEM_CLOCK_instance(), true, src);
#endif
        break;
#endif // SOC_BT_SUPPORTED

    case PERIPH_COEX_MODULE:
        modem_clock_hal_deselect_all_coex_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_select_coex_lpclk_source(MODEM_CLOCK_instance()->hal, src);
        modem_lpcon_ll_set_coex_lpclk_divisor_value(MODEM_CLOCK_instance()->hal->lpcon_dev, divider);
        // modem_lpcon_ll_enable_coex_clock(MODEM_CLOCK_instance()->hal->lpcon_dev, true); // TODO: IDF-14964
        break;
    default:
        break;
    }
#if SOC_LIGHT_SLEEP_SUPPORTED
    modem_clock_lpclk_src_t last_src = MODEM_CLOCK_instance()->lpclk_src[lpclk_src_module];
#endif
    MODEM_CLOCK_instance()->lpclk_src[lpclk_src_module] = src;
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);

#if SOC_LIGHT_SLEEP_SUPPORTED
    /* The power domain of the low-power clock source required by the modem
     * module remains powered on during sleep */
    esp_sleep_pd_config(modem_clock_lpclk_src_power_domain(last_src), ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(modem_clock_lpclk_src_power_domain(src), ESP_PD_OPTION_ON);
#endif
}

void modem_clock_deselect_lp_clock_source(shared_periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));

    modem_lpclk_src_module_t lpclk_src_module = modem_clock_lpclk_src_module(module);
    if (lpclk_src_module >= MODEM_LPCLK_SRC_MODULE_MAX) {
        return;
    }

    esp_os_enter_critical_safe(&MODEM_CLOCK_instance()->lock);
#if SOC_LIGHT_SLEEP_SUPPORTED
    modem_clock_lpclk_src_t last_src = MODEM_CLOCK_instance()->lpclk_src[lpclk_src_module];
#endif
    switch (module)
    {
#if SOC_WIFI_SUPPORTED
    case PERIPH_WIFI_MODULE:
        modem_clock_hal_deselect_all_wifi_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_enable_wifipwr_clock(MODEM_CLOCK_instance()->hal, false);
        break;
#endif // SOC_WIFI_SUPPORTED

#if SOC_BT_SUPPORTED
    case PERIPH_BT_MODULE:
        modem_clock_hal_deselect_all_ble_rtc_timer_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_enable_ble_rtc_timer_clock(MODEM_CLOCK_instance()->hal, false);
#if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND
        modem_clock_bt_wifipwr_clk_workaround(MODEM_CLOCK_instance(), false, last_src);
#endif
        break;
#endif // SOC_BT_SUPPORTED
    case PERIPH_COEX_MODULE:
        modem_clock_hal_deselect_all_coex_lpclk_source(MODEM_CLOCK_instance()->hal);
        // modem_lpcon_ll_enable_coex_clock(MODEM_CLOCK_instance()->hal->lpcon_dev, false); // TODO: IDF-14964
        break;
    default:
        break;
    }
    MODEM_CLOCK_instance()->lpclk_src[lpclk_src_module] = MODEM_CLOCK_LPCLK_SRC_INVALID;
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_pd_config(modem_clock_lpclk_src_power_domain(last_src), ESP_PD_OPTION_OFF);
#endif
}
