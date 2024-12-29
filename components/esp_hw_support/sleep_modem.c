/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_check.h"
#include "soc/soc_caps.h"
#include "esp_private/pm_impl.h"
#include "esp_private/sleep_modem.h"
#include "esp_private/sleep_retention.h"
#include "sdkconfig.h"

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
#include "esp_private/esp_pmu.h"
#endif
#if SOC_PM_MODEM_RETENTION_BY_REGDMA
#include "esp_pau.h"
#endif

static __attribute__((unused)) const char *TAG = "sleep_modem";

#if CONFIG_PM_SLP_DEFAULT_PARAMS_OPT
static void esp_pm_light_sleep_default_params_config(int min_freq_mhz, int max_freq_mhz);
#endif

#if SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD
static bool s_modem_sleep = false;
static uint8_t s_modem_prepare_ref = 0;
static _lock_t s_modem_prepare_lock;
#endif // SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD

#if CONFIG_MAC_BB_PD
#define MAC_BB_POWER_DOWN_CB_NO     (3)
#define MAC_BB_POWER_UP_CB_NO       (3)

static DRAM_ATTR mac_bb_power_down_cb_t s_mac_bb_power_down_cb[MAC_BB_POWER_DOWN_CB_NO];
static DRAM_ATTR mac_bb_power_up_cb_t   s_mac_bb_power_up_cb[MAC_BB_POWER_UP_CB_NO];

esp_err_t esp_register_mac_bb_pd_callback(mac_bb_power_down_cb_t cb)
{
    int index = MAC_BB_POWER_DOWN_CB_NO;
    for (int i = MAC_BB_POWER_DOWN_CB_NO - 1; i >= 0; i--) {
        if (s_mac_bb_power_down_cb[i] == cb) {
            return ESP_OK;
        }

        if (s_mac_bb_power_down_cb[i] == NULL) {
            index = i;
        }
    }

    if (index < MAC_BB_POWER_DOWN_CB_NO) {
        s_mac_bb_power_down_cb[index] = cb;
        return ESP_OK;
    }

    return ESP_ERR_NO_MEM;
}

esp_err_t esp_unregister_mac_bb_pd_callback(mac_bb_power_down_cb_t cb)
{
    for (int i = MAC_BB_POWER_DOWN_CB_NO - 1; i >= 0; i--) {
        if (s_mac_bb_power_down_cb[i] == cb) {
            s_mac_bb_power_down_cb[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

void IRAM_ATTR mac_bb_power_down_cb_execute(void)
{
    for (int i = 0; i < MAC_BB_POWER_DOWN_CB_NO; i++) {
        if (s_mac_bb_power_down_cb[i]) {
            s_mac_bb_power_down_cb[i]();
        }
    }
}

esp_err_t esp_register_mac_bb_pu_callback(mac_bb_power_up_cb_t cb)
{
    int index = MAC_BB_POWER_UP_CB_NO;
    for (int i = MAC_BB_POWER_UP_CB_NO - 1; i >= 0; i--) {
        if (s_mac_bb_power_up_cb[i] == cb) {
            return ESP_OK;
        }

        if (s_mac_bb_power_up_cb[i] == NULL) {
            index = i;
        }
    }

    if (index < MAC_BB_POWER_UP_CB_NO) {
        s_mac_bb_power_up_cb[index] = cb;
        return ESP_OK;
    }

    return ESP_ERR_NO_MEM;
}

esp_err_t esp_unregister_mac_bb_pu_callback(mac_bb_power_up_cb_t cb)
{
    for (int i = MAC_BB_POWER_UP_CB_NO - 1; i >= 0; i--) {
        if (s_mac_bb_power_up_cb[i] == cb) {
            s_mac_bb_power_up_cb[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

void IRAM_ATTR mac_bb_power_up_cb_execute(void)
{
    for (int i = 0; i < MAC_BB_POWER_UP_CB_NO; i++) {
        if (s_mac_bb_power_up_cb[i]) {
            s_mac_bb_power_up_cb[i]();
        }
    }
}

#endif ///CONFIG_MAC_BB_PD

#if SOC_PM_SUPPORT_PMU_MODEM_STATE

typedef struct sleep_modem_config {
    struct {
        void    *phy_link;
        union {
            struct {
                uint32_t modem_state_phy_done: 1;
                uint32_t reserved: 31;
            };
            uint32_t flags;
        };
    } wifi;
} sleep_modem_config_t;

static sleep_modem_config_t s_sleep_modem = { .wifi.phy_link = NULL, .wifi.flags = 0 };


esp_err_t sleep_modem_wifi_modem_state_init(void)
{
    esp_err_t err = ESP_OK;

    void *link = NULL;
    if (s_sleep_modem.wifi.phy_link == NULL) {
        err = sleep_modem_state_phy_link_init(&link);
        if (err == ESP_OK) {
            s_sleep_modem.wifi.phy_link = link;
            s_sleep_modem.wifi.flags = 0;
        }
    }
    return err;
}

__attribute__((unused)) void sleep_modem_wifi_modem_state_deinit(void)
{
    if (s_sleep_modem.wifi.phy_link) {
        sleep_modem_state_phy_link_deinit(s_sleep_modem.wifi.phy_link);
        s_sleep_modem.wifi.phy_link = NULL;
        s_sleep_modem.wifi.flags = 0;
    }
}

void IRAM_ATTR sleep_modem_wifi_do_phy_retention(bool restore)
{
    if (restore) {
        pau_regdma_trigger_modem_link_restore();
    } else {
        pau_regdma_trigger_modem_link_backup();
        s_sleep_modem.wifi.modem_state_phy_done = 1;
    }
}

inline __attribute__((always_inline)) bool sleep_modem_wifi_modem_state_enabled(void)
{
    return (s_sleep_modem.wifi.phy_link != NULL);
}

inline __attribute__((always_inline)) bool sleep_modem_wifi_modem_link_done(void)
{
    return (s_sleep_modem.wifi.modem_state_phy_done == 1);
}

#endif /* SOC_PM_SUPPORT_PMU_MODEM_STATE */

bool modem_domain_pd_allowed(void)
{
#if SOC_PM_MODEM_RETENTION_BY_REGDMA
    const sleep_retention_module_bitmap_t inited_modules = sleep_retention_get_inited_modules();
    const sleep_retention_module_bitmap_t created_modules = sleep_retention_get_created_modules();

    sleep_retention_module_bitmap_t mask = (sleep_retention_module_bitmap_t){ .bitmap = { 0 } };
#if SOC_WIFI_SUPPORTED
    mask.bitmap[SLEEP_RETENTION_MODULE_WIFI_MAC >> 5] |= BIT(SLEEP_RETENTION_MODULE_WIFI_MAC % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_WIFI_BB >> 5] |= BIT(SLEEP_RETENTION_MODULE_WIFI_BB % 32);
#endif
#if SOC_BT_SUPPORTED
    mask.bitmap[SLEEP_RETENTION_MODULE_BLE_MAC >> 5] |= BIT(SLEEP_RETENTION_MODULE_BLE_MAC % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_BT_BB >> 5] |= BIT(SLEEP_RETENTION_MODULE_BT_BB % 32);
#endif
#if SOC_IEEE802154_SUPPORTED
    mask.bitmap[SLEEP_RETENTION_MODULE_802154_MAC >> 5] |= BIT(SLEEP_RETENTION_MODULE_802154_MAC % 32);
    mask.bitmap[SLEEP_RETENTION_MODULE_BT_BB >> 5] |= BIT(SLEEP_RETENTION_MODULE_BT_BB % 32);
#endif

    const sleep_retention_module_bitmap_t modem_domain_inited_modules = sleep_retention_module_bitmap_and(inited_modules, mask);
    const sleep_retention_module_bitmap_t modem_domain_created_modules = sleep_retention_module_bitmap_and(created_modules, mask);
    return sleep_retention_module_bitmap_eq(modem_domain_inited_modules, modem_domain_created_modules);
#else
    return false; /* MODEM power domain is controlled by each module (WiFi, Bluetooth or 15.4) of modem */
#endif
}

uint32_t IRAM_ATTR sleep_modem_reject_triggers(void)
{
    uint32_t reject_triggers = 0;
#if SOC_PM_SUPPORT_PMU_MODEM_STATE
    reject_triggers = (s_sleep_modem.wifi.phy_link != NULL) ? BIT(16) : 0;
#endif
    return reject_triggers;
}

bool IRAM_ATTR sleep_modem_wifi_modem_state_skip_light_sleep(void)
{
    bool skip = false;
#if SOC_PM_SUPPORT_PMU_MODEM_STATE
    /* To block the system from entering sleep before modem link done. In light
     * sleep mode, the system may switch to modem state, which will cause
     * hardware to fail to enable RF */
    skip = sleep_modem_wifi_modem_state_enabled() && !sleep_modem_wifi_modem_link_done();
#endif
    return skip;
}

esp_err_t sleep_modem_configure(int max_freq_mhz, int min_freq_mhz, bool light_sleep_enable)
{
#if CONFIG_ESP_WIFI_ENHANCED_LIGHT_SLEEP
    extern int esp_wifi_internal_light_sleep_configure(bool);
    esp_wifi_internal_light_sleep_configure(light_sleep_enable);
#endif
#if CONFIG_PM_SLP_DEFAULT_PARAMS_OPT
    if (light_sleep_enable) {
        esp_pm_light_sleep_default_params_config(min_freq_mhz, max_freq_mhz);
    }
#endif
    return ESP_OK;
}

#define PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO 2

/* Inform peripherals of light sleep wakeup overhead time */
static inform_out_light_sleep_overhead_cb_t s_periph_inform_out_light_sleep_overhead_cb[PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO];

esp_err_t esp_pm_register_inform_out_light_sleep_overhead_callback(inform_out_light_sleep_overhead_cb_t cb)
{
    for (int i = 0; i < PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO; i++) {
        if (s_periph_inform_out_light_sleep_overhead_cb[i] == cb) {
            return ESP_OK;
        } else if (s_periph_inform_out_light_sleep_overhead_cb[i] == NULL) {
            s_periph_inform_out_light_sleep_overhead_cb[i] = cb;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_pm_unregister_inform_out_light_sleep_overhead_callback(inform_out_light_sleep_overhead_cb_t cb)
{
    for (int i = 0; i < PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO; i++) {
        if (s_periph_inform_out_light_sleep_overhead_cb[i] == cb) {
            s_periph_inform_out_light_sleep_overhead_cb[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

void periph_inform_out_light_sleep_overhead(uint32_t out_light_sleep_time)
{
    for (int i = 0; i < PERIPH_INFORM_OUT_LIGHT_SLEEP_OVERHEAD_NO; i++) {
        if (s_periph_inform_out_light_sleep_overhead_cb[i]) {
            s_periph_inform_out_light_sleep_overhead_cb[i](out_light_sleep_time);
        }
    }
}

static update_light_sleep_default_params_config_cb_t s_light_sleep_default_params_config_cb = NULL;

void esp_pm_register_light_sleep_default_params_config_callback(update_light_sleep_default_params_config_cb_t cb)
{
    if (s_light_sleep_default_params_config_cb == NULL) {
        s_light_sleep_default_params_config_cb = cb;
    }
}

void esp_pm_unregister_light_sleep_default_params_config_callback(void)
{
    if (s_light_sleep_default_params_config_cb) {
        s_light_sleep_default_params_config_cb = NULL;
    }
}

#if CONFIG_PM_SLP_DEFAULT_PARAMS_OPT
static void esp_pm_light_sleep_default_params_config(int min_freq_mhz, int max_freq_mhz)
{
    if (s_light_sleep_default_params_config_cb) {
        (*s_light_sleep_default_params_config_cb)(min_freq_mhz, max_freq_mhz);
    }
}
#endif

#if SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD
void sleep_modem_register_mac_bb_module_prepare_callback(mac_bb_power_down_cb_t pd_cb,
                                                         mac_bb_power_up_cb_t pu_cb)
{
    _lock_acquire(&s_modem_prepare_lock);
    if (s_modem_prepare_ref++ == 0) {
        esp_register_mac_bb_pd_callback(pd_cb);
        esp_register_mac_bb_pu_callback(pu_cb);
    }
    _lock_release(&s_modem_prepare_lock);
}

void sleep_modem_unregister_mac_bb_module_prepare_callback(mac_bb_power_down_cb_t pd_cb,
                                                           mac_bb_power_up_cb_t pu_cb)
{
    _lock_acquire(&s_modem_prepare_lock);
    assert(s_modem_prepare_ref);
    if (--s_modem_prepare_ref == 0) {
        esp_unregister_mac_bb_pd_callback(pd_cb);
        esp_unregister_mac_bb_pu_callback(pu_cb);
    }
    _lock_release(&s_modem_prepare_lock);

}

/**
 * @brief Switch root clock source to PLL do retention and switch back
 *
 * This function is used when Bluetooth/IEEE802154 module requires register backup/restore, this function
 * is called ONLY when SOC_PM_RETENTION_HAS_CLOCK_BUG is set.
 * @param backup true for backup, false for restore
 * @param cpu_freq_mhz cpu frequency to do retention
 * @param do_retention function for retention
 */
static void IRAM_ATTR rtc_clk_cpu_freq_to_pll_mhz_and_do_retention(bool backup, int cpu_freq_mhz, void (*do_retention)(bool))
{
#if SOC_PM_SUPPORT_PMU_MODEM_STATE
    if (pmu_sleep_pll_already_enabled()) {
        return;
    }
#endif
    rtc_cpu_freq_config_t config, pll_config;
    rtc_clk_cpu_freq_get_config(&config);

    rtc_clk_cpu_freq_mhz_to_config(cpu_freq_mhz, &pll_config);
    rtc_clk_cpu_freq_set_config(&pll_config);

    if (do_retention) {
        (*do_retention)(backup);
    }

    rtc_clk_cpu_freq_set_config(&config);
}

void IRAM_ATTR sleep_modem_mac_bb_power_down_prepare(void)
{
    if (s_modem_sleep == false) {
        rtc_clk_cpu_freq_to_pll_mhz_and_do_retention(true,
                                                     CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
                                                     sleep_retention_do_extra_retention);
        s_modem_sleep = true;
    }
}

void IRAM_ATTR sleep_modem_mac_bb_power_up_prepare(void)
{
    if (s_modem_sleep) {
        rtc_clk_cpu_freq_to_pll_mhz_and_do_retention(false,
                                                     CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
                                                     sleep_retention_do_extra_retention);
        s_modem_sleep = false;
    }
}
#endif /* SOC_PM_RETENTION_HAS_CLOCK_BUG && CONFIG_MAC_BB_PD */
