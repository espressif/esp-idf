/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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
#include "freertos/FreeRTOS.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/critical_section.h"
#include "esp_sleep.h"
#include "hal/efuse_hal.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "esp_private/regi2c_ctrl.h"
#if CONFIG_IDF_TARGET_ESP32H2
#include "soc/rtc.h"
#endif // CONFIG_IDF_TARGET_ESP32H2

// Please define the frequently called modules in the low bit,
// which will improve the execution efficiency
typedef enum {
    MODEM_CLOCK_MODEM_ADC_COMMON_FE,
    MODEM_CLOCK_MODEM_PRIVATE_FE,
    MODEM_CLOCK_COEXIST,
    MODEM_CLOCK_I2C_MASTER,
#if SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
    MODEM_CLOCK_WIFI_APB,
    MODEM_CLOCK_WIFI_BB_44M,
#endif
#if SOC_WIFI_SUPPORTED
    MODEM_CLOCK_WIFI_MAC,
    MODEM_CLOCK_WIFI_BB,
#endif
    MODEM_CLOCK_ETM,
#if SOC_BT_SUPPORTED
    MODEM_CLOCK_BLE_MAC,
#endif
#if SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
    MODEM_CLOCK_BT_I154_COMMON_BB,
#endif
#if SOC_IEEE802154_SUPPORTED
    MODEM_CLOCK_802154_MAC,
#endif
    MODEM_CLOCK_DATADUMP,
    MODEM_CLOCK_DEVICE_MAX
} modem_clock_device_t;

#define MODEM_STATUS_IDLE           (0)
#define MODEM_STATUS_WIFI_INITED    (0x1UL)

typedef struct modem_clock_context {
    modem_clock_hal_context_t *hal;
    portMUX_TYPE              lock;
    struct {
        int16_t     refs;
        uint16_t    reserved;   /* reserved for 4 bytes aligned */
        void (*configure)(struct modem_clock_context *, bool);
    } dev[MODEM_CLOCK_DEVICE_MAX];
    /* the low-power clock source for each module */
    modem_clock_lpclk_src_t lpclk_src[PERIPH_MODEM_MODULE_NUM];
#if SOC_WIFI_SUPPORTED
    uint32_t modem_status;
#endif
} modem_clock_context_t;


#if SOC_WIFI_SUPPORTED
static void IRAM_ATTR modem_clock_wifi_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable || !(ctx->modem_status & MODEM_STATUS_WIFI_INITED)) {
#if !SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
        modem_syscon_ll_enable_wifi_apb_clock(ctx->hal->syscon_dev, enable);
#endif
        modem_syscon_ll_enable_wifi_mac_clock(ctx->hal->syscon_dev, enable);
    }
}

static void IRAM_ATTR modem_clock_wifi_bb_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable || !(ctx->modem_status & MODEM_STATUS_WIFI_INITED)) {
        modem_syscon_ll_clk_wifibb_configure(ctx->hal->syscon_dev, enable);
    }
}
#endif // SOC_WIFI_SUPPORTED

#if SOC_BT_SUPPORTED
static void IRAM_ATTR modem_clock_ble_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_bt_mac_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_modem_sec_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_ble_timer_clock(ctx->hal->syscon_dev, enable);
}
#endif // SOC_BT_SUPPORTED

#if SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
static void IRAM_ATTR modem_clock_wifi_apb_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable || !(ctx->modem_status & MODEM_STATUS_WIFI_INITED)) {
        modem_syscon_ll_enable_wifi_apb_clock(ctx->hal->syscon_dev, enable);
    }
}

static void IRAM_ATTR modem_clock_wifi_bb_44m_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable || !(ctx->modem_status & MODEM_STATUS_WIFI_INITED)) {
        modem_syscon_ll_enable_wifibb_44m_clock(ctx->hal->syscon_dev, enable);
    }
}
#endif

#if SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
static void IRAM_ATTR modem_clock_ble_i154_bb_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_bt_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_bt_clock(ctx->hal->syscon_dev, enable);
}

#endif // SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED

#if SOC_IEEE802154_SUPPORTED
static void IRAM_ATTR modem_clock_ieee802154_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_ieee802154_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_ieee802154_mac_clock(ctx->hal->syscon_dev, enable);
}
#endif // SOC_IEEE802154_SUPPORTED

static void IRAM_ATTR modem_clock_coex_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_lpcon_ll_enable_coex_clock(ctx->hal->lpcon_dev, enable);
}

static void IRAM_ATTR modem_clock_modem_adc_common_fe_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_clock_hal_enable_modem_common_fe_clock(ctx->hal, enable);
}

static void IRAM_ATTR modem_clock_modem_private_fe_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_clock_hal_enable_modem_private_fe_clock(ctx->hal, enable);
}

static void IRAM_ATTR modem_clock_i2c_master_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable) {
        ANALOG_CLOCK_ENABLE();
    } else {
        ANALOG_CLOCK_DISABLE();
    }
}

static void IRAM_ATTR modem_clock_etm_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_etm_clock(ctx->hal->syscon_dev, enable);
}

static void IRAM_ATTR modem_clock_data_dump_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_data_dump_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_data_dump_mux_clock(ctx->hal->syscon_dev, enable);
}

modem_clock_context_t * __attribute__((weak)) IRAM_ATTR MODEM_CLOCK_instance(void)
{
    /* It should be explicitly defined in the internal RAM */
    static DRAM_ATTR modem_clock_hal_context_t modem_clock_hal = { .syscon_dev = NULL, .lpcon_dev = NULL };
    static DRAM_ATTR modem_clock_context_t modem_clock_context = {
        .hal = &modem_clock_hal, .lock = portMUX_INITIALIZER_UNLOCKED,
        .dev = {
            [MODEM_CLOCK_MODEM_ADC_COMMON_FE]   = { .refs = 0, .configure = modem_clock_modem_adc_common_fe_configure },
            [MODEM_CLOCK_MODEM_PRIVATE_FE]      = { .refs = 0, .configure = modem_clock_modem_private_fe_configure },
            [MODEM_CLOCK_COEXIST]               = { .refs = 0, .configure = modem_clock_coex_configure },
            [MODEM_CLOCK_I2C_MASTER]            = { .refs = 0, .configure = modem_clock_i2c_master_configure },
#if SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
            [MODEM_CLOCK_WIFI_APB]              = { .refs = 0, .configure = modem_clock_wifi_apb_configure },
            [MODEM_CLOCK_WIFI_BB_44M]           = { .refs = 0, .configure = modem_clock_wifi_bb_44m_configure },
#endif
#if SOC_WIFI_SUPPORTED
            [MODEM_CLOCK_WIFI_MAC]              = { .refs = 0, .configure = modem_clock_wifi_mac_configure },
            [MODEM_CLOCK_WIFI_BB]               = { .refs = 0, .configure = modem_clock_wifi_bb_configure },
#endif
            [MODEM_CLOCK_ETM]                   = { .refs = 0, .configure = modem_clock_etm_configure },
#if SOC_BT_SUPPORTED
            [MODEM_CLOCK_BLE_MAC]               = { .refs = 0, .configure = modem_clock_ble_mac_configure },
#endif
#if SOC_IEEE802154_SUPPORTED || SOC_BT_SUPPORTED
            [MODEM_CLOCK_BT_I154_COMMON_BB]       = { .refs = 0, .configure = modem_clock_ble_i154_bb_configure },
#endif
#if SOC_IEEE802154_SUPPORTED
            [MODEM_CLOCK_802154_MAC]            = { .refs = 0, .configure = modem_clock_ieee802154_mac_configure },
#endif
            [MODEM_CLOCK_DATADUMP]              = { .refs = 0, .configure = modem_clock_data_dump_configure }
        },
        .lpclk_src = { [0 ... PERIPH_MODEM_MODULE_NUM - 1] = MODEM_CLOCK_LPCLK_SRC_INVALID }
#if SOC_WIFI_SUPPORTED
        ,
        .modem_status = MODEM_STATUS_IDLE
#endif
    };
    if (modem_clock_hal.syscon_dev == NULL || modem_clock_hal.lpcon_dev == NULL) {
        modem_clock_hal.syscon_dev = &MODEM_SYSCON;
        modem_clock_hal.lpcon_dev = &MODEM_LPCON;
#if SOC_CLOCK_TREE_MANAGEMENT_SUPPORTED
        ESP_ERROR_CHECK(esp_clk_tree_enable_src(SOC_MOD_CLK_MODEM_APB, true));
#endif
    }
    return &modem_clock_context;
}

#if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND
esp_err_t modem_clock_domain_clk_gate_enable(modem_clock_domain_t domain, pmu_hp_icg_modem_mode_t mode)
{
    if (domain >= MODEM_CLOCK_DOMAIN_MAX || domain < MODEM_CLOCK_DOMAIN_MODEM_APB) {
        return ESP_ERR_INVALID_ARG;
    }
    if (mode > PMU_HP_ICG_MODEM_CODE_ACTIVE || mode < PMU_HP_ICG_MODEM_CODE_SLEEP) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_os_enter_critical_safe(&MODEM_CLOCK_instance()->lock);
    uint32_t code = modem_clock_hal_get_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain);
    modem_clock_hal_set_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain, (code & ~BIT(mode)));
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);
    return ESP_OK;
}

esp_err_t modem_clock_domain_clk_gate_disable(modem_clock_domain_t domain, pmu_hp_icg_modem_mode_t mode)
{
    if (domain >= MODEM_CLOCK_DOMAIN_MAX || domain < MODEM_CLOCK_DOMAIN_MODEM_APB) {
        return ESP_ERR_INVALID_ARG;
    }
    if (mode > PMU_HP_ICG_MODEM_CODE_ACTIVE || mode < PMU_HP_ICG_MODEM_CODE_SLEEP) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_os_enter_critical_safe(&MODEM_CLOCK_instance()->lock);
    uint32_t code = modem_clock_hal_get_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain);
    modem_clock_hal_set_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain, (code | BIT(mode)));
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);
    return ESP_OK;
}
#endif // #if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND

static void IRAM_ATTR modem_clock_device_enable(modem_clock_context_t *ctx, uint32_t dev_map)
{
    esp_os_enter_critical_safe(&ctx->lock);
    for (int i = 0; dev_map; dev_map >>= 1, i++) {
        if (dev_map & BIT(0)) {
            ctx->dev[i].refs++;
            (*ctx->dev[i].configure)(ctx, true);
        }
    }
    esp_os_exit_critical_safe(&ctx->lock);
}

static void IRAM_ATTR modem_clock_device_disable(modem_clock_context_t *ctx, uint32_t dev_map)
{
    int16_t refs = 0;
    esp_os_enter_critical_safe(&ctx->lock);
    for (int i = 0; dev_map; dev_map >>= 1, i++) {
        if (dev_map & BIT(0)) {
            refs = --ctx->dev[i].refs;
            if (refs == 0) {
                (*ctx->dev[i].configure)(ctx, false);
            }
        }
    }
    esp_os_exit_critical_safe(&ctx->lock);
    assert(refs >= 0);
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
#if SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
#define WIFI_CLOCK_DEPS         (BIT(MODEM_CLOCK_WIFI_MAC) | BIT(MODEM_CLOCK_WIFI_APB) | BIT(MODEM_CLOCK_WIFI_BB) | BIT(MODEM_CLOCK_WIFI_BB_44M) | BIT(MODEM_CLOCK_COEXIST))
#else
#define WIFI_CLOCK_DEPS         (BIT(MODEM_CLOCK_WIFI_MAC) | BIT(MODEM_CLOCK_WIFI_BB) | BIT(MODEM_CLOCK_COEXIST))
#endif
#define BLE_CLOCK_DEPS          (BIT(MODEM_CLOCK_BLE_MAC) | BIT(MODEM_CLOCK_BT_I154_COMMON_BB) | BIT(MODEM_CLOCK_ETM) | BIT(MODEM_CLOCK_COEXIST))
#define IEEE802154_CLOCK_DEPS   (BIT(MODEM_CLOCK_802154_MAC) | BIT(MODEM_CLOCK_BT_I154_COMMON_BB) | BIT(MODEM_CLOCK_ETM) | BIT(MODEM_CLOCK_COEXIST))
#define COEXIST_CLOCK_DEPS      (BIT(MODEM_CLOCK_COEXIST))
#define PHY_CLOCK_DEPS          (BIT(MODEM_CLOCK_I2C_MASTER) | BIT(MODEM_CLOCK_MODEM_ADC_COMMON_FE) | BIT(MODEM_CLOCK_MODEM_PRIVATE_FE))
#define I2C_ANA_MST_CLOCK_DEPS  (BIT(MODEM_CLOCK_I2C_MASTER))
#define MODEM_ETM_CLOCK_DEPS    (BIT(MODEM_CLOCK_ETM))
#define MODEM_ADC_COMMON_FE_CLOCK_DEPS  (BIT(MODEM_CLOCK_MODEM_ADC_COMMON_FE))
#if SOC_PHY_CALIBRATION_CLOCK_IS_INDEPENDENT
#define PHY_CALIBRATION_WIFI_CLOCK_DEPS     (BIT(MODEM_CLOCK_WIFI_MAC) | BIT(MODEM_CLOCK_WIFI_APB) | BIT(MODEM_CLOCK_WIFI_BB) | BIT(MODEM_CLOCK_WIFI_BB_44M))
#define PHY_CALIBRATION_BT_I154_CLOCK_DEPS (BIT(MODEM_CLOCK_WIFI_APB) | BIT(MODEM_CLOCK_WIFI_BB_44M) | BIT(MODEM_CLOCK_BT_I154_COMMON_BB))
#else
#define PHY_CALIBRATION_WIFI_CLOCK_DEPS     (BIT(MODEM_CLOCK_WIFI_MAC) | BIT(MODEM_CLOCK_WIFI_BB))
#define PHY_CALIBRATION_BT_I154_CLOCK_DEPS (BIT(MODEM_CLOCK_BT_I154_COMMON_BB))
#endif
#ifndef SOC_WIFI_SUPPORTED
#undef  PHY_CALIBRATION_WIFI_CLOCK_DEPS
#define PHY_CALIBRATION_WIFI_CLOCK_DEPS     0
#endif
#if !defined(SOC_BT_SUPPORTED) && !defined(SOC_IEEE802154_SUPPORTED)
#undef  PHY_CALIBRATION_BT_I154_CLOCK_DEPS
#define PHY_CALIBRATION_BT_I154_CLOCK_DEPS 0
#endif
#define PHY_CALIBRATION_CLOCK_DEPS          (PHY_CALIBRATION_WIFI_CLOCK_DEPS | PHY_CALIBRATION_BT_I154_CLOCK_DEPS)

static IRAM_ATTR uint32_t modem_clock_get_module_deps(shared_periph_module_t module)
{
    uint32_t deps = 0;
    switch (module) {
        case PERIPH_ANA_I2C_MASTER_MODULE:      deps = I2C_ANA_MST_CLOCK_DEPS;          break;
        case PERIPH_PHY_MODULE:                 deps = PHY_CLOCK_DEPS;                  break;
        case PERIPH_MODEM_ADC_COMMON_FE_MODULE: deps = MODEM_ADC_COMMON_FE_CLOCK_DEPS;  break;
#if SOC_WIFI_SUPPORTED || SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
        case PERIPH_COEX_MODULE:                deps = COEXIST_CLOCK_DEPS;              break;
#endif
#if SOC_WIFI_SUPPORTED
        case PERIPH_WIFI_MODULE:                deps = WIFI_CLOCK_DEPS;                 break;
#endif
#if SOC_BT_SUPPORTED
        case PERIPH_BT_MODULE:                  deps = BLE_CLOCK_DEPS;                  break;
#endif
        case PERIPH_PHY_CALIBRATION_MODULE:     deps = PHY_CALIBRATION_CLOCK_DEPS;      break;
#if SOC_IEEE802154_SUPPORTED
        case PERIPH_IEEE802154_MODULE:          deps = IEEE802154_CLOCK_DEPS;           break;
#endif
#if SOC_BT_SUPPORTED || SOC_IEEE802154_SUPPORTED
        case PERIPH_MODEM_ETM_MODULE:           deps = MODEM_ETM_CLOCK_DEPS;            break;
#endif
        default:
            assert(0);
    }
    return deps;
}

/* the ICG code's bit 0, 1 and 2 indicates the ICG state
 * of pmu SLEEP, MODEM and ACTIVE mode respectively */
#define ICG_NOGATING_ACTIVE (BIT(PMU_HP_ICG_MODEM_CODE_ACTIVE))
#define ICG_NOGATING_SLEEP  (BIT(PMU_HP_ICG_MODEM_CODE_SLEEP))
#define ICG_NOGATING_MODEM  (BIT(PMU_HP_ICG_MODEM_CODE_MODEM))

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
static const DRAM_ATTR uint32_t initial_gating_mode[MODEM_CLOCK_DOMAIN_MAX] = {
    [MODEM_CLOCK_DOMAIN_MODEM_APB]      = ICG_NOGATING_ACTIVE | ICG_NOGATING_MODEM,
    [MODEM_CLOCK_DOMAIN_MODEM_PERIPH]   = ICG_NOGATING_ACTIVE,
    [MODEM_CLOCK_DOMAIN_WIFI]           = ICG_NOGATING_ACTIVE | ICG_NOGATING_MODEM,
    [MODEM_CLOCK_DOMAIN_BT]             = ICG_NOGATING_ACTIVE,
    [MODEM_CLOCK_DOMAIN_MODEM_FE]       = ICG_NOGATING_ACTIVE | ICG_NOGATING_MODEM,
    [MODEM_CLOCK_DOMAIN_IEEE802154]     = ICG_NOGATING_ACTIVE,
    [MODEM_CLOCK_DOMAIN_LP_APB]         = ICG_NOGATING_ACTIVE | ICG_NOGATING_MODEM,
    [MODEM_CLOCK_DOMAIN_I2C_MASTER]     = ICG_NOGATING_ACTIVE | ICG_NOGATING_MODEM,
    [MODEM_CLOCK_DOMAIN_COEX]           = ICG_NOGATING_ACTIVE | ICG_NOGATING_MODEM,
    [MODEM_CLOCK_DOMAIN_WIFIPWR]        = ICG_NOGATING_ACTIVE | ICG_NOGATING_MODEM,
};
#endif

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
static IRAM_ATTR void modem_clock_module_icg_map_init_all(void)
{
    esp_os_enter_critical_safe(&MODEM_CLOCK_instance()->lock);
    for (int domain = 0; domain < MODEM_CLOCK_DOMAIN_MAX; domain++) {
        uint32_t code = modem_clock_hal_get_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain);
        modem_clock_hal_set_clock_domain_icg_bitmap(MODEM_CLOCK_instance()->hal, domain, initial_gating_mode[domain] | code);
    }
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);
}
#endif

void IRAM_ATTR modem_clock_module_enable(shared_periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
#if SOC_PM_SUPPORT_PMU_MODEM_STATE
    modem_clock_module_icg_map_init_all();
#endif
    uint32_t deps = modem_clock_get_module_deps(module);
    modem_clock_device_enable(MODEM_CLOCK_instance(), deps);
}

void IRAM_ATTR modem_clock_module_disable(shared_periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
    uint32_t deps = modem_clock_get_module_deps(module);
    modem_clock_device_disable(MODEM_CLOCK_instance(), deps);
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

void modem_clock_select_lp_clock_source(shared_periph_module_t module, modem_clock_lpclk_src_t src, uint32_t divider)
{
    assert(IS_MODEM_MODULE(module));
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
    {
#if CONFIG_IDF_TARGET_ESP32H2
        bool rc_clk_en = true;
        bool selected = (src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ||
                        (src == MODEM_CLOCK_LPCLK_SRC_RC_SLOW);
        if (selected) {
            rc_clk_en = clk_ll_rc32k_is_enabled();
            if (!rc_clk_en) {
                rtc_clk_rc32k_enable(true);
            }
            modem_clock_hal_select_ble_rtc_timer_lpclk_source(MODEM_CLOCK_instance()->hal, MODEM_CLOCK_LPCLK_SRC_RC32K);
        }
#endif // CONFIG_IDF_TARGET_ESP32H2
        modem_clock_hal_deselect_all_ble_rtc_timer_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_select_ble_rtc_timer_lpclk_source(MODEM_CLOCK_instance()->hal, src);
        modem_clock_hal_set_ble_rtc_timer_divisor_value(MODEM_CLOCK_instance()->hal, divider);
        modem_clock_hal_enable_ble_rtc_timer_clock(MODEM_CLOCK_instance()->hal, true);
#if CONFIG_IDF_TARGET_ESP32H2
        if (!rc_clk_en) {
            extern void r_esp_ble_rtc_ticks_delay(uint32_t ticks);
            r_esp_ble_rtc_ticks_delay(2);
            rtc_clk_rc32k_enable(false);
        }
#endif // CONFIG_IDF_TARGET_ESP32H2
#if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND
        if (efuse_hal_chip_revision() != 0) {
            if (src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) {
                esp_sleep_clock_config(ESP_SLEEP_CLOCK_BT_USE_WIFI_PWR_CLK, ESP_SLEEP_CLOCK_OPTION_UNGATE);
                modem_clock_hal_enable_wifipwr_clock(MODEM_CLOCK_instance()->hal, true);
                modem_clock_domain_clk_gate_disable(MODEM_CLOCK_DOMAIN_WIFIPWR, PMU_HP_ICG_MODEM_CODE_SLEEP);
            }
        }
#endif
        break;
    }
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
    modem_clock_lpclk_src_t last_src = MODEM_CLOCK_instance()->lpclk_src[module - PERIPH_MODEM_MODULE_MIN];
#endif
    MODEM_CLOCK_instance()->lpclk_src[module - PERIPH_MODEM_MODULE_MIN] = src;
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);

#if SOC_LIGHT_SLEEP_SUPPORTED
    /* The power domain of the low-power clock source required by the modem
     * module remains powered on during sleep */
    esp_sleep_pd_domain_t pd_domain = (esp_sleep_pd_domain_t) (
              (last_src == MODEM_CLOCK_LPCLK_SRC_RC_FAST)  ? ESP_PD_DOMAIN_RC_FAST  :
              (last_src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ? ESP_PD_DOMAIN_XTAL    :
#if !CONFIG_ESP_CLK_RC32K_NOT_TO_USE
              (last_src == MODEM_CLOCK_LPCLK_SRC_RC32K)     ? ESP_PD_DOMAIN_RC32K   :
#endif
              (last_src == MODEM_CLOCK_LPCLK_SRC_XTAL32K)   ? ESP_PD_DOMAIN_XTAL32K :
              ESP_PD_DOMAIN_MAX);
    esp_sleep_pd_domain_t pu_domain = (esp_sleep_pd_domain_t) (
              (src == MODEM_CLOCK_LPCLK_SRC_RC_FAST)  ? ESP_PD_DOMAIN_RC_FAST  :
              (src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ? ESP_PD_DOMAIN_XTAL    :
#if !CONFIG_ESP_CLK_RC32K_NOT_TO_USE
              (src == MODEM_CLOCK_LPCLK_SRC_RC32K)     ? ESP_PD_DOMAIN_RC32K   :
#endif
              (src == MODEM_CLOCK_LPCLK_SRC_XTAL32K)   ? ESP_PD_DOMAIN_XTAL32K :
              ESP_PD_DOMAIN_MAX);
    esp_sleep_pd_config(pd_domain, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(pu_domain, ESP_PD_OPTION_ON);
#endif
}

void modem_clock_deselect_lp_clock_source(shared_periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
    esp_os_enter_critical_safe(&MODEM_CLOCK_instance()->lock);
#if SOC_LIGHT_SLEEP_SUPPORTED
    modem_clock_lpclk_src_t last_src = MODEM_CLOCK_instance()->lpclk_src[module - PERIPH_MODEM_MODULE_MIN];
#endif
    MODEM_CLOCK_instance()->lpclk_src[module - PERIPH_MODEM_MODULE_MIN] = MODEM_CLOCK_LPCLK_SRC_INVALID;
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
#if SOC_BLE_USE_WIFI_PWR_CLK_WORKAROUND && SOC_LIGHT_SLEEP_SUPPORTED  // TODO: [ESP32C5] IDF-8643
        if (efuse_hal_chip_revision() != 0) {
            if (last_src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) {
                esp_sleep_clock_config(ESP_SLEEP_CLOCK_BT_USE_WIFI_PWR_CLK, ESP_SLEEP_CLOCK_OPTION_GATE);
                modem_clock_hal_enable_wifipwr_clock(MODEM_CLOCK_instance()->hal, false);
                modem_clock_domain_clk_gate_enable(MODEM_CLOCK_DOMAIN_WIFIPWR, PMU_HP_ICG_MODEM_CODE_SLEEP);
            }
        }
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
    esp_os_exit_critical_safe(&MODEM_CLOCK_instance()->lock);

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_pd_domain_t pd_domain = (esp_sleep_pd_domain_t) (
              (last_src == MODEM_CLOCK_LPCLK_SRC_RC_FAST)  ? ESP_PD_DOMAIN_RC_FAST  :
              (last_src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ? ESP_PD_DOMAIN_XTAL    :
#if !CONFIG_ESP_CLK_RC32K_NOT_TO_USE
              (last_src == MODEM_CLOCK_LPCLK_SRC_RC32K)     ? ESP_PD_DOMAIN_RC32K   :
#endif
              (last_src == MODEM_CLOCK_LPCLK_SRC_XTAL32K)   ? ESP_PD_DOMAIN_XTAL32K :
              ESP_PD_DOMAIN_MAX);
    esp_sleep_pd_config(pd_domain, ESP_PD_OPTION_OFF);
#endif
}
