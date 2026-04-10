
/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "esp_bt.h"

/* From Modem and Phy */
#include "esp_phy_init.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_private/esp_clk_tree_common.h"

#ifdef CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
#include "esp_private/pm_impl.h"
#include "esp_private/sleep_modem.h"
#include "esp_private/sleep_retention.h"
#endif

/*
 ***************************************************************************************************
 * Local Defined Macros
 ***************************************************************************************************
 */
#define BTDM_LOG_TAG "BTDM_SLEEP"

#define BTDM_RTC_DELAY_US_LIGHT_SLEEP (2000)
#define BTDM_RTC_DELAY_US_MODEM_SLEEP (1500)

/*
 ***************************************************************************************************
 * External Functions
 ***************************************************************************************************
 */
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
extern bool r_btdm_sleep_should_skip_light_sleep_check(void);
extern const sleep_retention_entries_config_t *r_btdm_mac_retention_link_get(uint8_t *size);
extern void r_btdm_sleep_wake_up_overhead_set(uint32_t overhead);
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
extern esp_err_t sleep_modem_bredr_mac_modem_state_init(void);
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
#if UC_BT_CTRL_BLE_IS_ENABLE
extern esp_err_t sleep_modem_ble_mac_modem_state_init(void);
#endif // UC_BT_CTRL_BLE_IS_ENABLEs
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE */

extern void r_btdm_sleep_set_sleep_cb(void *s_cb, void *w_cb, void *s_arg, void *w_arg,
                                      uint32_t us_to_enabled);

/*
 ***************************************************************************************************
 * Local Variables
 ***************************************************************************************************
 */
static bool s_bt_active = false;
static DRAM_ATTR modem_clock_lpclk_src_t s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_INVALID;

#ifdef CONFIG_PM_ENABLE
static DRAM_ATTR esp_pm_lock_handle_t s_pm_lock = NULL;
#endif // CONFIG_PM_ENABLE
// static modem_clock_lpclk_src_t s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_INVALID;
static uint32_t s_bt_lpclk_freq = 100000;

/*
 ***************************************************************************************************
 * Static Function Definitions
 ***************************************************************************************************
 */
static void
btdm_lp_rtc_slow_clk_select(uint8_t slow_clk_src)
{
    /* Select slow clock source for BT momdule */
    switch (slow_clk_src) {
        case MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL:
            ESP_LOGI(BTDM_LOG_TAG, "Using main XTAL as clock source");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (CONFIG_XTAL_FREQ * 1000000 / s_bt_lpclk_freq - 1));
            break;
        case MODEM_CLOCK_LPCLK_SRC_RC_SLOW:
            ESP_LOGW(BTDM_LOG_TAG, "Using 136 kHz RC as clock source, use with caution as it may not maintain ACL or Sync process due to low clock accuracy!");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (5 - 1));
            break;
        case MODEM_CLOCK_LPCLK_SRC_XTAL32K:
            ESP_LOGI(BTDM_LOG_TAG, "Using external 32.768 kHz XTAL as clock source");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
            break;
        case MODEM_CLOCK_LPCLK_SRC_RC32K:
            ESP_LOGI(BTDM_LOG_TAG, "Using 32 kHz RC as clock source, can only run legacy ADV or SCAN due to low clock accuracy!");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
            break;
        case MODEM_CLOCK_LPCLK_SRC_EXT32K:
            ESP_LOGI(BTDM_LOG_TAG, "Using 32 kHz oscillator as clock source, can only run legacy ADV or SCAN due to low clock accuracy!");
            modem_clock_select_lp_clock_source(PERIPH_BT_MODULE, slow_clk_src, (1 - 1));
            break;
        default:
    }
}

static void
btdm_lp_timer_clk_init(esp_btdm_controller_config_t *cfg)
{
    if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_INVALID) {
#if CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL
        s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL;
#else
#if CONFIG_RTC_CLK_SRC_INT_RC
        s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_RC_SLOW;
#elif CONFIG_RTC_CLK_SRC_EXT_CRYS
        uint32_t clk_freq = 0;

        if (rtc_clk_slow_src_get() == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
            if (!esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_XTAL32K, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &clk_freq)) {
                if (clk_freq > 32700 && clk_freq < 33800) {
                    s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_XTAL32K;
                } else {
                    ESP_LOGW(BTDM_LOG_TAG, "32.768kHz XTAL detection error, switch to main XTAL as Bluetooth sleep clock");
                    s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL;
                }
            } else {
                ESP_LOGW(BTDM_LOG_TAG, "32.768kHz XTAL detection error, switch to main XTAL as Bluetooth sleep clock");
                s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL;
            }
        } else {
            ESP_LOGW(BTDM_LOG_TAG, "32.768kHz XTAL not detected, fall back to main XTAL as Bluetooth sleep clock");
            s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL;
        }
#elif CONFIG_RTC_CLK_SRC_INT_RC32K
        s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_RC32K;
#elif CONFIG_RTC_CLK_SRC_EXT_OSC
        s_bt_lpclk_src = MODEM_CLOCK_LPCLK_SRC_EXT32K;
#else
        ESP_LOGE(BTDM_LOG_TAG, "Unsupported clock source");
        assert(0);
#endif
#endif /* CONFIG_BT_LE_LP_CLK_SRC_MAIN_XTAL */
    }

    // if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_MAIN_XTAL) {
    //     cfg->rtc_freq = s_bt_lpclk_freq;
    // } else if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_XTAL32K) {
    //     cfg->rtc_freq = 32768;
    // } else if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_RC_SLOW) {
    //     cfg->rtc_freq = esp_clk_tree_lp_slow_get_freq_hz(ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED) / 5;
    //     //TODO
    //     // cfg->ble_ll_sca = 3000;
    // } else if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_RC32K) {
    //     cfg->rtc_freq = 32000;
    // } else if (s_bt_lpclk_src == MODEM_CLOCK_LPCLK_SRC_EXT32K) {
    //     cfg->rtc_freq = 32000;
    // }
    btdm_lp_rtc_slow_clk_select(s_bt_lpclk_src);
}

static void
btdm_lp_timer_clk_deinit(void)
{
    modem_clock_deselect_lp_clock_source(PERIPH_BT_MODULE);
}

void
btdm_lp_sleep_cb(uint32_t enable_tick, void *arg)
{
    if (!s_bt_active) {
        return;
    }
    esp_phy_disable(PHY_MODEM_BT);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(s_pm_lock);
#endif // CONFIG_PM_ENABLE
    s_bt_active = false;
}

void
btdm_lp_wake_up_cb(void *arg)
{
    if (s_bt_active) {
        return;
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(s_pm_lock);
#endif // CONFIG_PM_ENABLE
    esp_phy_enable(PHY_MODEM_BT);
    s_bt_active = true;
}

#if UC_BT_CTRL_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE
static esp_err_t
btdm_lp_modem_retention_create(void)
{
    uint8_t size = 0;
    const sleep_retention_entries_config_t *btdm_mac_modem_config =
        r_btdm_mac_retention_link_get(&size);
    esp_err_t err = sleep_retention_entries_create(btdm_mac_modem_config, size, REGDMA_LINK_PRI_5,
                                                   SLEEP_RETENTION_MODULE_BLE_MAC);
    if (err == ESP_OK) {
        ESP_LOGI(BTDM_LOG_TAG, "Modem BTDM MAC retention initialization");
    }

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    // TODO: check the return value
    sleep_modem_bredr_mac_modem_state_init();
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE

#if UC_BT_CTRL_BLE_IS_ENABLE
    // TODO: check the return value
    sleep_modem_ble_mac_modem_state_init();
#endif // UC_BT_CTRL_BLE_IS_ENABLE
    return err;
}

static esp_err_t
btdm_lp_modem_state_init(void)
{
    sleep_retention_module_init_param_t init_param = {
        .cbs = {.create = {.handle = (void *)btdm_lp_modem_retention_create, .arg = NULL}},
        .depends = RETENTION_MODULE_BITMAP_INIT(BT_BB)};
    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_BLE_MAC, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_BLE_MAC);
    }
    return err;
}

static void
btdm_lp_modem_state_deinit(void)
{
    esp_err_t err = sleep_retention_module_free(SLEEP_RETENTION_MODULE_BLE_MAC);
    if (err == ESP_OK) {
        err = sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_BLE_MAC);
        assert(err == ESP_OK);
    }
}
#endif // UC_BT_CTRL_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE

/*
 ***************************************************************************************************
 * Public Function Definitions
 ***************************************************************************************************
 */
#include "modem/modem_syscon_reg.h"
void
btdm_lp_enable_clock(esp_btdm_controller_config_t *cfg)
{
    modem_clock_module_enable(PERIPH_BT_MODULE);
    modem_clock_module_mac_reset(PERIPH_BT_MODULE);
    // TODO: set the clock ion modem_clock_module_enable
    REG_WRITE(MODEM_SYSCON_CLK_CONF_POWER_ST_REG, 0XFFFFFFFF);
    btdm_lp_timer_clk_init(cfg);
}

void
btdm_lp_disable_clock(void)
{
    btdm_lp_timer_clk_deinit();
    modem_clock_module_disable(PERIPH_BT_MODULE);
}

int
btdm_lp_init(void)
{
#if UC_BT_CTRL_SLEEP_ENABLE
    ESP_LOGI(BTDM_LOG_TAG, "Bluetooth modem sleep is enabled");
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
    r_btdm_sleep_set_sleep_cb(btdm_lp_sleep_cb, btdm_lp_wake_up_cb, 0, 0,
                              BTDM_RTC_DELAY_US_LIGHT_SLEEP);
#else
    r_btdm_sleep_set_sleep_cb(btdm_lp_sleep_cb, btdm_lp_wake_up_cb, 0, 0,
                              BTDM_RTC_DELAY_US_MODEM_SLEEP);
#endif /* FREERTOS_USE_TICKLESS_IDLE */
#endif // UC_BT_CTRL_SLEEP_ENABLE

#ifdef CONFIG_PM_ENABLE
    esp_err_t rc = 0;

    rc = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "bt", &s_pm_lock);
    if (rc != ESP_OK) {
        return -1;
    }
#if UC_BT_CTRL_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE
    /* Create a new regdma link for BLE related register restoration */
    rc = btdm_lp_modem_state_init();
    assert(rc == 0);
    esp_sleep_enable_bt_wakeup();
    ESP_LOGW(BTDM_LOG_TAG, "Enable light sleep, the wake up source is BLE timer");

    rc = esp_pm_register_inform_out_light_sleep_overhead_callback(r_btdm_sleep_wake_up_overhead_set);
    if (rc != ESP_OK) {
        return -2;
    }
    // rc = esp_pm_register_skip_light_sleep_callback(r_btdm_sleep_should_skip_light_sleep_check);
    if (rc != ESP_OK) {
        return -3;
    }
#endif /* UC_BT_CTRL_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE */
#endif /* CONFIG_PM_ENABLE */

    return 0;
}

void
btdm_lp_deinit(void)
{
#if UC_BT_CTRL_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE
    // esp_pm_unregister_skip_light_sleep_callback(r_btdm_sleep_should_skip_light_sleep_check);
    esp_pm_unregister_inform_out_light_sleep_overhead_callback(r_btdm_sleep_wake_up_overhead_set);
    esp_sleep_disable_bt_wakeup();
    btdm_lp_modem_state_deinit();
#endif /* UC_BT_CTRL_SLEEP_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE */

#ifdef CONFIG_PM_ENABLE
    /* lock should be released first */
    if (s_pm_lock) {
        esp_pm_lock_delete(s_pm_lock);
        s_pm_lock = NULL;
    }
#endif // CONFIG_PM_ENABLE
}

void
btdm_lp_reset(bool enable_stage)
{
    // TODO: For enable_stage, need to disable the phy by default.
    if (enable_stage) {
        assert(!s_bt_active);

#if CONFIG_PM_ENABLE
        esp_pm_lock_acquire(s_pm_lock);
#endif // CONFIG_PM_ENABLE

        esp_phy_enable(PHY_MODEM_BT);
        // TODO: Need to be deleted.
        void phy_set_rfpll_xpd(bool en);
        phy_set_rfpll_xpd(0);
        esp_btbb_enable();
        s_bt_active = true;
    } else {
        if (s_bt_active) {
            esp_btbb_disable();
            esp_phy_disable(PHY_MODEM_BT);
#if CONFIG_PM_ENABLE
            esp_pm_lock_release(s_pm_lock);
#endif // CONFIG_PM_ENABLE
            s_bt_active = false;
        }
    }
}

void
btdm_lp_shutdown(void)
{
    modem_clock_module_mac_reset(PERIPH_BT_MODULE);
    if (s_bt_active) {
        // esp_phy_disable(PHY_MODEM_BT);
        s_bt_active = false;
    }
}
