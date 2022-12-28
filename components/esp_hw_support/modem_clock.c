/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdlib.h>
#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "soc/soc.h"
#include "freertos/FreeRTOS.h"
#include "hal/clk_gate_ll.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_sleep.h"

// Please define the frequently called modules in the low bit,
// which will improve the execution efficiency
typedef enum {
    MODEM_CLOCK_FE         = BIT(0),
    MODEM_CLOCK_COEXIST    = BIT(1),
    MODEM_CLOCK_I2C_MASTER = BIT(2),
    MODEM_CLOCK_WIFI_MAC   = BIT(3),
    MODEM_CLOCK_WIFI_BB    = BIT(4),
    MODEM_CLOCK_ETM        = BIT(5),
    MODEM_CLOCK_BLE_MAC    = BIT(6),
    MODEM_CLOCK_BLE_BB     = BIT(7),
    MODEM_CLOCK_802154_MAC = BIT(8),
    MODEM_CLOCK_DATADUMP   = BIT(9),
    MODEM_CLOCK_DEVICE_MAX = 10
} modem_clock_device_t;


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
} modem_clock_context_t;


static void IRAM_ATTR modem_clock_wifi_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_wifi_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifi_mac_clock(ctx->hal->syscon_dev, enable);
}

static void IRAM_ATTR modem_clock_wifi_bb_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_wifibb_160x1_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifibb_80x1_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifibb_40x1_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifibb_80x_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifibb_40x_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifibb_80m_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifibb_44m_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifibb_40m_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_wifibb_22m_clock(ctx->hal->syscon_dev, enable);
}

static void IRAM_ATTR modem_clock_ble_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_etm_clock(ctx->hal->syscon_dev, enable);
    modem_syscom_ll_enable_modem_sec_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_ble_timer_clock(ctx->hal->syscon_dev, enable);
}

static void IRAM_ATTR modem_clock_ble_bb_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_bt_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_bt_clock(ctx->hal->syscon_dev, enable);
}

static void IRAM_ATTR modem_clock_ieee802154_mac_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_ieee802154_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_ieee802154_mac_clock(ctx->hal->syscon_dev, enable);
}

static void IRAM_ATTR modem_clock_coex_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_lpcon_ll_enable_coex_clock(ctx->hal->lpcon_dev, enable);
}

static void IRAM_ATTR modem_clock_fe_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_syscon_ll_enable_fe_apb_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_fe_cal_160m_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_fe_160m_clock(ctx->hal->syscon_dev, enable);
    modem_syscon_ll_enable_fe_80m_clock(ctx->hal->syscon_dev, enable);
}

static void IRAM_ATTR modem_clock_i2c_master_configure(modem_clock_context_t *ctx, bool enable)
{
    modem_lpcon_ll_enable_i2c_master_clock(ctx->hal->lpcon_dev, enable);
    modem_lpcon_ll_enable_i2c_master_160m_clock(ctx->hal->lpcon_dev, enable);
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
    static DRAM_ATTR modem_clock_hal_context_t modem_clock_hal = { .syscon_dev = &MODEM_SYSCON, .lpcon_dev = &MODEM_LPCON };
    static DRAM_ATTR modem_clock_context_t modem_clock_context = {
        .hal = &modem_clock_hal, .lock = portMUX_INITIALIZER_UNLOCKED,
        .dev = {
            { .refs = 0, .configure = modem_clock_fe_configure             },
            { .refs = 0, .configure = modem_clock_coex_configure           },
            { .refs = 0, .configure = modem_clock_i2c_master_configure     },
            { .refs = 0, .configure = modem_clock_wifi_mac_configure       },
            { .refs = 0, .configure = modem_clock_wifi_bb_configure        },
            { .refs = 0, .configure = modem_clock_etm_configure            },
            { .refs = 0, .configure = modem_clock_ble_mac_configure        },
            { .refs = 0, .configure = modem_clock_ble_bb_configure         },
            { .refs = 0, .configure = modem_clock_ieee802154_mac_configure },
            { .refs = 0, .configure = modem_clock_data_dump_configure      }
        },
        .lpclk_src = { [0 ... PERIPH_MODEM_MODULE_NUM - 1] = MODEM_CLOCK_LPCLK_SRC_INVALID }
    };
    return &modem_clock_context;
}


// TODO: IDF-5351: move to esp_pmu.h after support pmu driver
#define PMU_SLEEP   0
#define PMU_MODEM   1
#define PMU_ACTIVE  2
#define SLEEP_MODE  BIT(PMU_SLEEP)
#define MODEM_MODE  BIT(PMU_MODEM)
#define ACTIVE_MODE BIT(PMU_ACTIVE)

static void IRAM_ATTR modem_clock_domain_power_state_icg_map_init(modem_clock_context_t *ctx)
{
    /* the ICG code's bit 0, 1 and 2 indicates the ICG state
     * of pmu SLEEP, MODEM and ACTIVE mode respectively */
    const uint32_t code[MODEM_CLOCK_DOMAIN_MAX] = {
        [MODEM_CLOCK_DOMAIN_MODEM_APB]      = MODEM_MODE | ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_MODEM_PERIPH]   = ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_WIFI]           = MODEM_MODE | ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_BT]             = MODEM_MODE | ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_FE]             = MODEM_MODE | ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_IEEE802154]     = MODEM_MODE | ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_LP_APB]         = MODEM_MODE | ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_I2C_MASTER]     = MODEM_MODE | ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_COEX]           = MODEM_MODE | ACTIVE_MODE,
        [MODEM_CLOCK_DOMAIN_WIFIPWR]        = MODEM_MODE | ACTIVE_MODE,
    };
    for (modem_clock_domain_t domain = MODEM_CLOCK_DOMAIN_MODEM_APB; domain < MODEM_CLOCK_DOMAIN_MAX; domain++) {
        modem_clock_hal_set_clock_domain_icg_bitmap(ctx->hal, domain, code[domain]);
    }
}


#include "soc/pmu_reg.h"
void modem_clock_domain_pmu_state_icg_map_init(void)
{
    // Set modem clock ICG code map, should implement with pmu driver // TODO: IDF-5351
    REG_SET_FIELD(PMU_HP_SLEEP_ICG_MODEM_REG, PMU_HP_SLEEP_DIG_ICG_MODEM_CODE, PMU_SLEEP);
    REG_SET_FIELD(PMU_HP_MODEM_ICG_MODEM_REG, PMU_HP_MODEM_DIG_ICG_MODEM_CODE, PMU_MODEM);
    REG_SET_FIELD(PMU_HP_ACTIVE_ICG_MODEM_REG, PMU_HP_ACTIVE_DIG_ICG_MODEM_CODE, PMU_ACTIVE);
    REG_SET_BIT(PMU_IMM_MODEM_ICG_REG, PMU_UPDATE_DIG_ICG_MODEM_EN);
    REG_SET_BIT(PMU_IMM_SLEEP_SYSCLK_REG, PMU_UPDATE_DIG_ICG_SWITCH);

    modem_clock_domain_power_state_icg_map_init(MODEM_CLOCK_instance());
}

static void IRAM_ATTR modem_clock_device_enable(modem_clock_context_t *ctx, uint32_t dev_map)
{
    int16_t refs = 0;
    portENTER_CRITICAL_SAFE(&ctx->lock);
    for (int i = 0; dev_map; dev_map >>= 1, i++) {
        if (dev_map & BIT(0)) {
            refs = ctx->dev[i].refs++;
            if (refs == 0) {
                (*ctx->dev[i].configure)(ctx, true);
            }
        }
    }
    portEXIT_CRITICAL_SAFE(&ctx->lock);
    assert(refs >= 0);
}

static void IRAM_ATTR modem_clock_device_disable(modem_clock_context_t *ctx, uint32_t dev_map)
{
    int16_t refs = 0;
    portENTER_CRITICAL_SAFE(&ctx->lock);
    for (int i = 0; dev_map; dev_map >>= 1, i++) {
        if (dev_map & BIT(0)) {
            refs = --ctx->dev[i].refs;
            if (refs == 0) {
                (*ctx->dev[i].configure)(ctx, false);
            }
        }
    }
    portEXIT_CRITICAL_SAFE(&ctx->lock);
    assert(refs >= 0);
}

#define WIFI_CLOCK_DEPS       (MODEM_CLOCK_WIFI_MAC | MODEM_CLOCK_FE | MODEM_CLOCK_WIFI_BB | MODEM_CLOCK_COEXIST)
#define BLE_CLOCK_DEPS        (MODEM_CLOCK_BLE_MAC | MODEM_CLOCK_FE | MODEM_CLOCK_BLE_BB | MODEM_CLOCK_ETM | MODEM_CLOCK_COEXIST)
#define IEEE802154_CLOCK_DEPS (MODEM_CLOCK_802154_MAC | MODEM_CLOCK_FE | MODEM_CLOCK_BLE_BB | MODEM_CLOCK_ETM | MODEM_CLOCK_COEXIST)
#define COEXIST_CLOCK_DEPS    (MODEM_CLOCK_COEXIST)
#define PHY_CLOCK_DEPS        (MODEM_CLOCK_I2C_MASTER)

void IRAM_ATTR modem_clock_module_enable(periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
    const int deps = (module == PERIPH_WIFI_MODULE)         ? WIFI_CLOCK_DEPS       \
                   : (module == PERIPH_BT_MODULE)           ? BLE_CLOCK_DEPS        \
                   : (module == PERIPH_IEEE802154_MODULE)   ? IEEE802154_CLOCK_DEPS \
                   : (module == PERIPH_COEX_MODULE)         ? COEXIST_CLOCK_DEPS    \
                   : (module == PERIPH_PHY_MODULE)          ? PHY_CLOCK_DEPS        \
                   : 0;
    modem_clock_device_enable(MODEM_CLOCK_instance(), deps);
}

void IRAM_ATTR modem_clock_module_disable(periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
    const int deps = (module == PERIPH_WIFI_MODULE)         ? WIFI_CLOCK_DEPS       \
                   : (module == PERIPH_BT_MODULE)           ? BLE_CLOCK_DEPS        \
                   : (module == PERIPH_IEEE802154_MODULE)   ? IEEE802154_CLOCK_DEPS \
                   : (module == PERIPH_COEX_MODULE)         ? COEXIST_CLOCK_DEPS    \
                   : (module == PERIPH_PHY_MODULE)          ? PHY_CLOCK_DEPS        \
                   : 0;
    modem_clock_device_disable(MODEM_CLOCK_instance(), deps);
}

void modem_clock_select_lp_clock_source(periph_module_t module, modem_clock_lpclk_src_t src, uint32_t divider)
{
    assert(IS_MODEM_MODULE(module));
    portENTER_CRITICAL_SAFE(&MODEM_CLOCK_instance()->lock);
    switch (module)
    {
    case PERIPH_WIFI_MODULE:
        modem_clock_hal_deselect_all_wifi_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_select_wifi_lpclk_source(MODEM_CLOCK_instance()->hal, src);
        modem_lpcon_ll_set_wifi_lpclk_divisor_value(MODEM_CLOCK_instance()->hal->lpcon_dev, divider);
        modem_lpcon_ll_enable_wifipwr_clock(MODEM_CLOCK_instance()->hal->lpcon_dev, true);
        break;
    case PERIPH_BT_MODULE:
        modem_clock_hal_deselect_all_lp_timer_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_select_lp_timer_lpclk_source(MODEM_CLOCK_instance()->hal, src);
        modem_lpcon_ll_set_lp_timer_divisor_value(MODEM_CLOCK_instance()->hal->lpcon_dev, divider);
        modem_lpcon_ll_enable_lp_timer_clock(MODEM_CLOCK_instance()->hal->lpcon_dev, true);
        break;
    case PERIPH_COEX_MODULE:
        modem_clock_hal_deselect_all_coex_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_clock_hal_select_coex_lpclk_source(MODEM_CLOCK_instance()->hal, src);
        modem_lpcon_ll_set_coex_lpclk_divisor_value(MODEM_CLOCK_instance()->hal->lpcon_dev, divider);
        // modem_lpcon_ll_enable_coex_clock(MODEM_CLOCK_instance()->hal->lpcon_dev, true); // TODO: IDF-5727
        break;
    default:
        break;
    }
    modem_clock_lpclk_src_t last_src = MODEM_CLOCK_instance()->lpclk_src[module - PERIPH_MODEM_MODULE_MIN];
    MODEM_CLOCK_instance()->lpclk_src[module - PERIPH_MODEM_MODULE_MIN] = src;
    portEXIT_CRITICAL_SAFE(&MODEM_CLOCK_instance()->lock);

    /* The power domain of the low-power clock source required by the modem
     * module remains powered on during sleep */
    esp_sleep_pd_domain_t pd_domain = (esp_sleep_pd_domain_t) ( \
              (last_src == MODEM_CLOCK_LPCLK_SRC_RC_FAST)  ? ESP_PD_DOMAIN_RC_FAST  \
            : (last_src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ? ESP_PD_DOMAIN_XTAL    \
            : (last_src == MODEM_CLOCK_LPCLK_SRC_RC32K)     ? ESP_PD_DOMAIN_RC32K   \
            : (last_src == MODEM_CLOCK_LPCLK_SRC_XTAL32K)   ? ESP_PD_DOMAIN_XTAL32K \
            : ESP_PD_DOMAIN_MAX);
    esp_sleep_pd_domain_t pu_domain = (esp_sleep_pd_domain_t) ( \
              (src == MODEM_CLOCK_LPCLK_SRC_RC_FAST)  ? ESP_PD_DOMAIN_RC_FAST  \
            : (src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ? ESP_PD_DOMAIN_XTAL    \
            : (src == MODEM_CLOCK_LPCLK_SRC_RC32K)     ? ESP_PD_DOMAIN_RC32K   \
            : (src == MODEM_CLOCK_LPCLK_SRC_XTAL32K)   ? ESP_PD_DOMAIN_XTAL32K \
            : ESP_PD_DOMAIN_MAX);
    esp_sleep_pd_config(pd_domain, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(pu_domain, ESP_PD_OPTION_ON);
}

void modem_clock_deselect_lp_clock_source(periph_module_t module)
{
    assert(IS_MODEM_MODULE(module));
    portENTER_CRITICAL_SAFE(&MODEM_CLOCK_instance()->lock);
    switch (module)
    {
    case PERIPH_WIFI_MODULE:
        modem_clock_hal_deselect_all_wifi_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_lpcon_ll_enable_wifipwr_clock(MODEM_CLOCK_instance()->hal->lpcon_dev, false);
        break;
    case PERIPH_BT_MODULE:
        modem_clock_hal_deselect_all_lp_timer_lpclk_source(MODEM_CLOCK_instance()->hal);
        modem_lpcon_ll_enable_lp_timer_clock(MODEM_CLOCK_instance()->hal->lpcon_dev, false);
        break;
    case PERIPH_COEX_MODULE:
        modem_clock_hal_deselect_all_coex_lpclk_source(MODEM_CLOCK_instance()->hal);
        // modem_lpcon_ll_enable_coex_clock(MODEM_CLOCK_instance()->hal->lpcon_dev, false); // TODO: IDF-5727
        break;
    default:
        break;
    }
    modem_clock_lpclk_src_t last_src = MODEM_CLOCK_instance()->lpclk_src[module - PERIPH_MODEM_MODULE_MIN];
    MODEM_CLOCK_instance()->lpclk_src[module - PERIPH_MODEM_MODULE_MIN] = MODEM_CLOCK_LPCLK_SRC_INVALID;
    portEXIT_CRITICAL_SAFE(&MODEM_CLOCK_instance()->lock);

    esp_sleep_pd_domain_t pd_domain = (esp_sleep_pd_domain_t) ( \
              (last_src == MODEM_CLOCK_LPCLK_SRC_RC_FAST)  ? ESP_PD_DOMAIN_RC_FAST  \
            : (last_src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) ? ESP_PD_DOMAIN_XTAL    \
            : (last_src == MODEM_CLOCK_LPCLK_SRC_RC32K)     ? ESP_PD_DOMAIN_RC32K   \
            : (last_src == MODEM_CLOCK_LPCLK_SRC_XTAL32K)   ? ESP_PD_DOMAIN_XTAL32K \
            : ESP_PD_DOMAIN_MAX);
    esp_sleep_pd_config(pd_domain, ESP_PD_OPTION_OFF);
}
