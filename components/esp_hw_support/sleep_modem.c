/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/soc_caps.h"
#include "esp_private/pm_impl.h"
#include "esp_private/sleep_modem.h"
#include "esp_private/sleep_retention.h"
#include "sdkconfig.h"

#if SOC_PM_MODEM_RETENTION_BY_REGDMA
#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"
#include "soc/i2c_ana_mst_reg.h"
#include "esp_pau.h"
#endif

#if SOC_PM_SUPPORT_PMU_MODEM_STATE
#include "soc/pmu_reg.h"
#include "esp_private/esp_pau.h"
#include "esp_private/esp_pmu.h"
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

#define PMU_RF_PWR_REG                  (0x600b0154)
#define SARADC_TSENS_REG                (0x6000e058)
#define SARADC_TSENS_PU                 (BIT(22))
#define FECOEX_SET_FREQ_SET_CHAN_REG    (0x600a00c0)
#define FECOEX_SET_CHAN_EN              (BIT(14))
#define FECOEX_SET_FREQ_SET_CHAN_ST_REG (0x600a00cc)
#define FECOEX_SET_CHAN_DONE            (BIT(8))
#define FECOEX_AGC_CONF_REG             (0x600a7030)
#define FECOEX_AGC_DIS                  (BIT(29))
#define WDEVTXQ_BLOCK                   (0x600A4ca8)
#define WDEV_RXBLOCK                    (BIT(12))
#define MODEM_FE_DATA_BASE              (0x600a0400)
#define MODEM_FE_CTRL_BASE              (0x600a0800)

#define I2C_BURST_VAL(host, start, end) (((host) << 31) | ((end) << 22) | ((start) << 16))

typedef struct {
    struct {
        uint8_t start, end; /* the start and end index of phy i2c master command memory */
        uint8_t host_id;    /* phy i2c master host id */
    } config[2];
} phy_i2c_master_command_attribute_t;

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
    phy_i2c_master_command_attribute_t cmd;

    /* get RF on or off configuration info of i2c master command memory */
    extern void phy_i2c_master_mem_cfg(phy_i2c_master_command_attribute_t *);
    phy_i2c_master_mem_cfg(&cmd);

    ESP_LOGD(TAG, "Modem link i2c master configuration: (%d,%d,%d), (%d,%d,%d)", cmd.config[0].host_id, cmd.config[0].start,
            cmd.config[0].end, cmd.config[1].host_id, cmd.config[1].start, cmd.config[1].end);

    static regdma_link_config_t wifi_modem_config[] = {
        [0] = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEM_FE_LINK(0), MODEM_FE_DATA_BASE, MODEM_FE_DATA_BASE, 41, 0, 0),
        [1] = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEM_FE_LINK(1), MODEM_FE_CTRL_BASE, MODEM_FE_CTRL_BASE, 87, 0, 0),

        [2]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x00), MODEM_LPCON_CLK_CONF_REG,         MODEM_LPCON_CLK_I2C_MST_EN,        MODEM_LPCON_CLK_I2C_MST_EN_M,       1, 0), /* I2C MST enable */
        [3]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x01), MODEM_LPCON_I2C_MST_CLK_CONF_REG, MODEM_LPCON_CLK_I2C_MST_SEL_160M,  MODEM_LPCON_CLK_I2C_MST_SEL_160M_M, 1, 0), /* I2C MST sel 160m enable */

        /* PMU or software to trigger enable RF PHY */
        [4]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x02), I2C_ANA_MST_ANA_CONF0_REG,        0x8,                       0xc,        1, 0), /* BBPLL calibration enable */
        [5]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x03), PMU_RF_PWR_REG,                   0xf0000000,                0xf0000000, 1, 0),
        [6]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x04), SARADC_TSENS_REG,                 SARADC_TSENS_PU,           0x400000,   1, 0),
        [7]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x05), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 1, 0),
        [8]  = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x06), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_BURST_DONE,    0x1,        1, 0),
        [9]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x07), FECOEX_SET_FREQ_SET_CHAN_REG,     FECOEX_SET_CHAN_EN,        0x4000,     1, 0),
        [10] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x08), FECOEX_SET_FREQ_SET_CHAN_REG,     0,                         0x4000,     1, 0),
        [11] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x09), FECOEX_SET_FREQ_SET_CHAN_ST_REG,  FECOEX_SET_CHAN_DONE,      0x100,      1, 0),
        [12] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0a), MODEM_SYSCON_WIFI_BB_CFG_REG,     BIT(1),                    0x2,        1, 0),
        [13] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0b), FECOEX_AGC_CONF_REG,              0,                         0x20000000, 1, 0),

        /* PMU to trigger enable RXBLOCK */
        [14] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0c), WDEVTXQ_BLOCK,                    0,                         0x1000,     1, 0),

        /* PMU or software to trigger disable RF PHY */
        [15] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0d), FECOEX_AGC_CONF_REG,              FECOEX_AGC_DIS,            0x20000000, 0, 1),
        [16] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0e), MODEM_SYSCON_WIFI_BB_CFG_REG,     0,                         0x2,        0, 1),
        [17] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0f), FECOEX_SET_FREQ_SET_CHAN_REG,     0,                         0x4000,     0, 1),
        [18] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x10), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 0, 1),
        [19] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x11), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_BURST_DONE,    0x1,        0, 1),
        [20] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x12), SARADC_TSENS_REG,                 0,                         0x400000,   0, 1),
        [21] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x13), PMU_RF_PWR_REG,                   0,                         0xf0000000, 0, 1),
        [22] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x14), I2C_ANA_MST_ANA_CONF0_REG,        0x4,                       0xc,        0, 1), /* BBPLL calibration disable */

        [23] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x15), MODEM_LPCON_CLK_CONF_REG,         0,                         MODEM_LPCON_CLK_I2C_MST_EN_M,       0, 1), /* I2C MST disable */

        /* PMU to trigger disable RXBLOCK */
        [24] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x17), WDEVTXQ_BLOCK,                    0,                         0x6000,     0, 1),
        [25] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x18), WDEVTXQ_BLOCK,                    WDEV_RXBLOCK,              0x1000,     0, 1),
        [26] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x19), WDEVTXQ_BLOCK,                    0,                         0x6000,     0, 1),

        [27] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1a), PMU_SLP_WAKEUP_CNTL7_REG,         0x200000,                  0xffff0000, 1, 0),
        [28] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1b), PMU_SLP_WAKEUP_CNTL7_REG,         0x9730000,                 0xffff0000, 0, 1)
    };
    wifi_modem_config[7].write_wait.value  = I2C_BURST_VAL(cmd.config[1].host_id, cmd.config[1].start, cmd.config[1].end);
    wifi_modem_config[18].write_wait.value = I2C_BURST_VAL(cmd.config[0].host_id, cmd.config[0].start, cmd.config[0].end);

    void *link = NULL;
    if (s_sleep_modem.wifi.phy_link == NULL) {
        for (int i = ARRAY_SIZE(wifi_modem_config) - 1; (err == ESP_OK) && (i >= 0); i--) {
            void *next = regdma_link_init_safe(&wifi_modem_config[i], false, 0, link);
            if (next) {
                link = next;
            } else {
                regdma_link_destroy(link, 0);
                err = ESP_ERR_NO_MEM;
            }
        }
        if (err == ESP_OK) {
            pau_regdma_set_modem_link_addr(link);
            s_sleep_modem.wifi.phy_link = link;
            s_sleep_modem.wifi.flags = 0;
        }
    }
    return err;
}

__attribute__((unused)) void sleep_modem_wifi_modem_state_deinit(void)
{
    if (s_sleep_modem.wifi.phy_link) {
        regdma_link_destroy(s_sleep_modem.wifi.phy_link, 0);
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
    const uint32_t modules = sleep_retention_get_modules();
    const uint32_t mask_wifi = (const uint32_t) (SLEEP_RETENTION_MODULE_WIFI_MAC |
                                                 SLEEP_RETENTION_MODULE_WIFI_BB);
    const uint32_t mask_ble = (const uint32_t) (SLEEP_RETENTION_MODULE_BLE_MAC |
                                                SLEEP_RETENTION_MODULE_BT_BB);
    const uint32_t mask_154 = (const uint32_t) (SLEEP_RETENTION_MODULE_802154_MAC |
                                                SLEEP_RETENTION_MODULE_BT_BB);
    return (((modules & mask_wifi) == mask_wifi) ||
            ((modules & mask_ble)  == mask_ble) ||
            ((modules & mask_154)  == mask_154));
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
