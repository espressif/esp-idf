/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "modem/modem_clock_impl.h"
#include "esp_private/regi2c_ctrl.h"

/* Clock dependency definitions */
#define WIFI_CLOCK_DEPS                     ( MODEM_CLOCKS( WIFI_MAC, WIFI_APB, WIFI_BB, WIFI_BB_44M, COEXIST ) )
#define BLE_CLOCK_DEPS                      ( MODEM_CLOCKS( BLE_MAC, BT_I154_COMMON_BB, ETM, COEXIST ) )
#define IEEE802154_CLOCK_DEPS               ( MODEM_CLOCKS( 802154_MAC, BT_I154_COMMON_BB, ETM, COEXIST ) )
#define COEXIST_CLOCK_DEPS                  ( MODEM_CLOCKS( COEXIST ) )
#define I2C_ANA_MST_CLOCK_DEPS              ( MODEM_CLOCKS( I2C_MASTER ) )
#define PHY_CLOCK_DEPS                      ( MODEM_CLOCKS( MODEM_ADC_COMMON_FE, MODEM_PRIVATE_FE ) | I2C_ANA_MST_CLOCK_DEPS )
#define MODEM_ETM_CLOCK_DEPS                ( MODEM_CLOCKS( ETM ) )
#define MODEM_ADC_COMMON_FE_CLOCK_DEPS      ( MODEM_CLOCKS( MODEM_ADC_COMMON_FE ) )
#define PHY_CALIBRATION_WIFI_CLOCK_DEPS     ( MODEM_CLOCKS( WIFI_APB, WIFI_BB, WIFI_BB_44M ) )
#define PHY_CALIBRATION_BT_I154_CLOCK_DEPS  ( MODEM_CLOCKS( WIFI_APB, WIFI_BB_44M, BT_I154_COMMON_BB ) )
#define PHY_CALIBRATION_CLOCK_DEPS          ( PHY_CALIBRATION_WIFI_CLOCK_DEPS | PHY_CALIBRATION_BT_I154_CLOCK_DEPS )

uint32_t IRAM_ATTR modem_clock_get_module_deps(shared_periph_module_t module)
{
    uint32_t deps = 0;
    switch (module) {
        case PERIPH_ANA_I2C_MASTER_MODULE:      deps = I2C_ANA_MST_CLOCK_DEPS;          break;
        case PERIPH_PHY_MODULE:                 deps = PHY_CLOCK_DEPS;                  break;
        case PERIPH_MODEM_ADC_COMMON_FE_MODULE: deps = MODEM_ADC_COMMON_FE_CLOCK_DEPS;  break;
        case PERIPH_COEX_MODULE:                deps = COEXIST_CLOCK_DEPS;              break;
        case PERIPH_WIFI_MODULE:                deps = WIFI_CLOCK_DEPS;                 break;
        case PERIPH_BT_MODULE:                  deps = BLE_CLOCK_DEPS;                  break;
        case PERIPH_PHY_CALIBRATION_MODULE:     deps = PHY_CALIBRATION_CLOCK_DEPS;      break;
        case PERIPH_IEEE802154_MODULE:          deps = IEEE802154_CLOCK_DEPS;           break;
        case PERIPH_MODEM_ETM_MODULE:           deps = MODEM_ETM_CLOCK_DEPS;            break;
        default:
            assert(0);
    }
    return deps;
}

static void IRAM_ATTR modem_clock_wifi_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable || !(ctx->modem_status & MODEM_STATUS_WIFI_INITED)) {
        modem_syscon_ll_enable_wifi_mac_clock(ctx->hal->syscon_dev, enable);
    }
}

static void IRAM_ATTR modem_clock_wifi_bb_configure(modem_clock_context_t *ctx, bool enable)
{
    if (enable || !(ctx->modem_status & MODEM_STATUS_WIFI_INITED)) {
        modem_syscon_ll_clk_wifibb_configure(ctx->hal->syscon_dev, enable);
    }
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_wifi_mac_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_wifi_mac_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_wifi_bb_check_enable(modem_clock_context_t *ctx)
{
    return modem_syscon_ll_wifibb_clock_is_enabled(ctx->hal->syscon_dev) ? ESP_OK : ESP_FAIL;
}
#endif

static void IRAM_ATTR modem_clock_ble_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_bt_mac_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_modem_sec_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_ble_timer_clock(ctx->hal->syscon_dev, enable);
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_ble_mac_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_bt_mac_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_modem_sec_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_ble_timer_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}
#endif

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

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_wifi_apb_check_enable(modem_clock_context_t *ctx)
{
    return modem_syscon_ll_wifi_apb_clock_is_enabled(ctx->hal->syscon_dev) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_wifi_bb_44m_check_enable(modem_clock_context_t *ctx)
{
    return modem_syscon_ll_wifibb_44m_clock_is_enabled(ctx->hal->syscon_dev) ? ESP_OK : ESP_FAIL;
}
#endif

static void IRAM_ATTR modem_clock_ble_i154_bb_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_bt_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_bt_clock(ctx->hal->syscon_dev, enable);
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_ble_i154_bb_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_bt_apb_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_bt_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}
#endif

static void IRAM_ATTR modem_clock_ieee802154_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_ieee802154_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_ieee802154_mac_clock(ctx->hal->syscon_dev, enable);
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_ieee802154_mac_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_ieee802154_apb_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_ieee802154_mac_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}
#endif

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

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_coex_check_enable(modem_clock_context_t *ctx)
{
    return modem_lpcon_ll_coex_clock_is_enabled(ctx->hal->lpcon_dev) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_modem_adc_common_fe_check_enable(modem_clock_context_t *ctx)
{
    return modem_clock_hal_modem_common_fe_clock_is_enabled(ctx->hal) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_modem_private_fe_check_enable(modem_clock_context_t *ctx)
{
    return modem_clock_hal_modem_private_fe_clock_is_enabled(ctx->hal) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_i2c_master_check_enable(modem_clock_context_t *ctx)
{
    return ANALOG_CLOCK_IS_ENABLED() ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_etm_check_enable(modem_clock_context_t *ctx)
{
    return modem_syscon_ll_etm_clock_is_enabled(ctx->hal->syscon_dev) ? ESP_OK : ESP_FAIL;
}

static esp_err_t IRAM_ATTR modem_clock_data_dump_check_enable(modem_clock_context_t *ctx)
{
    bool all_clock_enabled = true;
    all_clock_enabled &= modem_syscon_ll_data_dump_clock_is_enabled(ctx->hal->syscon_dev);
    all_clock_enabled &= modem_syscon_ll_data_dump_mux_clock_is_enabled(ctx->hal->syscon_dev);
    return all_clock_enabled ? ESP_OK : ESP_FAIL;
}
#endif

static void IRAM_ATTR modem_clock_configure_impl(modem_clock_context_t *ctx, int dev_id, bool enable, wrapper_t wrapper)
{
    assert(MODEM_CLOCK_DEVICE_MIN <= dev_id && dev_id < MODEM_CLOCK_DEVICE_MAX);

    void (*action)(struct modem_clock_context *, bool) =
        ( (dev_id == MODEM_CLOCK_MODEM_ADC_COMMON_FE)   ? modem_clock_modem_adc_common_fe_configure
        : (dev_id == MODEM_CLOCK_MODEM_PRIVATE_FE)      ? modem_clock_modem_private_fe_configure
        : (dev_id == MODEM_CLOCK_COEXIST)               ? modem_clock_coex_configure
        : (dev_id == MODEM_CLOCK_I2C_MASTER)            ? modem_clock_i2c_master_configure
        : (dev_id == MODEM_CLOCK_WIFI_APB)              ? modem_clock_wifi_apb_configure
        : (dev_id == MODEM_CLOCK_WIFI_BB_44M)           ? modem_clock_wifi_bb_44m_configure
        : (dev_id == MODEM_CLOCK_WIFI_MAC)              ? modem_clock_wifi_mac_configure
        : (dev_id == MODEM_CLOCK_WIFI_BB)               ? modem_clock_wifi_bb_configure
        : (dev_id == MODEM_CLOCK_ETM)                   ? modem_clock_etm_configure
        : (dev_id == MODEM_CLOCK_BLE_MAC)               ? modem_clock_ble_mac_configure
        : (dev_id == MODEM_CLOCK_BT_I154_COMMON_BB)     ? modem_clock_ble_i154_bb_configure
        : (dev_id == MODEM_CLOCK_802154_MAC)            ? modem_clock_ieee802154_mac_configure
        : (dev_id == MODEM_CLOCK_DATADUMP)              ? modem_clock_data_dump_configure
        : NULL);
    assert(action != NULL);
    (*wrapper)(ctx, enable, action, &ctx->dev->refs[dev_id].count, &ctx->dev->refs[dev_id].flags);
}

#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
static esp_err_t IRAM_ATTR modem_clock_check_impl(modem_clock_context_t *ctx, int dev_id)
{
    assert(MODEM_CLOCK_DEVICE_MIN <= dev_id && dev_id < MODEM_CLOCK_DEVICE_MAX);

    esp_err_t (*check_action)(struct modem_clock_context *) =
        ( (dev_id == MODEM_CLOCK_MODEM_ADC_COMMON_FE)   ? modem_clock_modem_adc_common_fe_check_enable
        : (dev_id == MODEM_CLOCK_MODEM_PRIVATE_FE)      ? modem_clock_modem_private_fe_check_enable
        : (dev_id == MODEM_CLOCK_COEXIST)               ? modem_clock_coex_check_enable
        : (dev_id == MODEM_CLOCK_I2C_MASTER)            ? modem_clock_i2c_master_check_enable
        : (dev_id == MODEM_CLOCK_WIFI_APB)              ? modem_clock_wifi_apb_check_enable
        : (dev_id == MODEM_CLOCK_WIFI_BB_44M)           ? modem_clock_wifi_bb_44m_check_enable
        : (dev_id == MODEM_CLOCK_WIFI_MAC)              ? modem_clock_wifi_mac_check_enable
        : (dev_id == MODEM_CLOCK_WIFI_BB)               ? modem_clock_wifi_bb_check_enable
        : (dev_id == MODEM_CLOCK_ETM)                   ? modem_clock_etm_check_enable
        : (dev_id == MODEM_CLOCK_BLE_MAC)               ? modem_clock_ble_mac_check_enable
        : (dev_id == MODEM_CLOCK_BT_I154_COMMON_BB)     ? modem_clock_ble_i154_bb_check_enable
        : (dev_id == MODEM_CLOCK_802154_MAC)            ? modem_clock_ieee802154_mac_check_enable
        : (dev_id == MODEM_CLOCK_DATADUMP)              ? modem_clock_data_dump_check_enable
        : NULL);
    assert(check_action != NULL);
    return (*check_action)(ctx);
}
#endif

IRAM_ATTR modem_clock_device_context_t *modem_clock_device_context(void)
{
    static DRAM_ATTR modem_clock_device_context_t dev = {
        .refs = { [0 ... MODEM_CLOCK_DEVICE_MAX - 1] = { .count = 0, .flags = 0 | REFS_FL_WITH_REFCNT } },
        .configure = modem_clock_configure_impl,
#if CONFIG_ESP_MODEM_CLOCK_ENABLE_CHECKING
        .check = modem_clock_check_impl,
#endif
    };
    dev.refs[MODEM_CLOCK_I2C_MASTER].flags &= ~REFS_FL_WITH_REFCNT;
    return &dev;
}

IRAM_ATTR uint8_t *modem_clock_domain_icg_config(void)
{
    static DRAM_ATTR uint8_t icg_config_default[MODEM_CLOCK_DOMAIN_MAX] = {
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
    return icg_config_default;
}
