/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_check.h"

#include "soc/soc_caps.h"
#include "soc/i2c_ana_mst_reg.h"
#include "soc/pmu_reg.h"

#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"

#include "esp_private/sleep_modem.h"
#include "esp_private/sleep_retention.h"

#if SOC_PM_SUPPORT_PMU_MODEM_STATE

#define SARADC_TSENS_REG                (0x6000e058)
#define SARADC_TSENS_PU                 (BIT(22))
#define PMU_RF_PWR_REG                  (0x600b0158)

#define FECOEX_SET_FREQ_SET_CHAN_REG    (0x600a001c)
#define FECOEX_SET_CHAN_EN              (BIT(17))
#define FECOEX_SET_FREQ_SET_CHAN_ST_REG (0x600a0028)
#define FECOEX_SET_CHAN_DONE            (BIT(8))
#define FECOEX_AGC_CONF_REG             (0x600a7030)
#define FECOEX_AGC_DIS                  (BIT(29))

#define WDEVTXQ_BLOCK                   (0x600A4ca8)
#define WDEV_RXBLOCK                    (BIT(12))
#define MODEM_FE_DATA_BASE              (0x600a0400)
#define MODEM_FE_CTRL_BASE              (0x600a0800)

static __attribute__((unused)) const char *TAG = "sleep";

#if SOC_PM_PAU_REGDMA_LINK_IDX_WIFIMAC
static esp_err_t sleep_modem_state_phy_wifi_init(void *arg)
{
    #define WIFIMAC_ENTRY() (BIT(SOC_PM_PAU_REGDMA_LINK_IDX_WIFIMAC))

    static sleep_retention_entries_config_t wifi_modem_config[] = {
        [0]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x00), MODEM_LPCON_CLK_CONF_REG,         MODEM_LPCON_CLK_I2C_MST_EN, MODEM_LPCON_CLK_I2C_MST_EN_M, 1, 0), .owner = WIFIMAC_ENTRY() }, /* I2C MST enable */

        /* PMU or software to trigger enable RF PHY */
        [1]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x01), I2C_ANA_MST_ANA_CONF0_REG,        0x8,                       0xc,        1, 0), .owner = WIFIMAC_ENTRY() }, /* BBPLL calibration enable */
        [2]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x02), PMU_RF_PWR_REG,                   0xf3800000,                0xf3800000, 1, 0), .owner = WIFIMAC_ENTRY() },
        [3]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x03), SARADC_TSENS_REG,                 SARADC_TSENS_PU,           0x400000,   1, 0), .owner = WIFIMAC_ENTRY() },
        [4]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x04), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 1, 0), .owner = WIFIMAC_ENTRY() },
        [5]  = { .config = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x05), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_BURST_DONE,    0x1,        1, 0), .owner = WIFIMAC_ENTRY() },
        [6]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x06), FECOEX_SET_FREQ_SET_CHAN_REG,     FECOEX_SET_CHAN_EN,        0x20000,    1, 0), .owner = WIFIMAC_ENTRY() },
        [7]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x07), FECOEX_SET_FREQ_SET_CHAN_REG,     0,                         0x20000,    1, 0), .owner = WIFIMAC_ENTRY() },
        [8]  = { .config = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x08), FECOEX_SET_FREQ_SET_CHAN_ST_REG,  FECOEX_SET_CHAN_DONE,      0x100,      1, 0), .owner = WIFIMAC_ENTRY() },
        [9]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x09), MODEM_SYSCON_WIFI_BB_CFG_REG,     BIT(1),                    0x2,        1, 0), .owner = WIFIMAC_ENTRY() },
        [10] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0a), FECOEX_AGC_CONF_REG,              0,                         0x20000000, 1, 0), .owner = WIFIMAC_ENTRY() },

        /* PMU to trigger enable RXBLOCK */
        [11] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0b), WDEVTXQ_BLOCK,                    0,                         0x1000,     1, 0), .owner = WIFIMAC_ENTRY() },

        /* PMU or software to trigger disable RF PHY */
        [12] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0c), FECOEX_AGC_CONF_REG,              FECOEX_AGC_DIS,            0x20000000, 0, 1), .owner = WIFIMAC_ENTRY() },
        [13] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0d), MODEM_SYSCON_WIFI_BB_CFG_REG,     0,                         0x2,        0, 1), .owner = WIFIMAC_ENTRY() },
        [14] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0e), FECOEX_SET_FREQ_SET_CHAN_REG,     0,                         0x20000,    0, 1), .owner = WIFIMAC_ENTRY() },
        [15] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0f), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 1, 1), .owner = WIFIMAC_ENTRY() },
        [16] = { .config = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x10), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_BURST_DONE,    0x1,        1, 1), .owner = WIFIMAC_ENTRY() },
        [17] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x11), SARADC_TSENS_REG,                 0,                         0x400000,   0, 1), .owner = WIFIMAC_ENTRY() },
        [18] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x12), PMU_RF_PWR_REG,                   0,                         0xf3800000, 0, 1), .owner = WIFIMAC_ENTRY() },
        [19] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x13), I2C_ANA_MST_ANA_CONF0_REG,        0x4,                       0xc,        0, 1), .owner = WIFIMAC_ENTRY() }, /* BBPLL calibration disable */

        [20] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x14), MODEM_LPCON_CLK_CONF_REG,         0,                         MODEM_LPCON_CLK_I2C_MST_EN_M,       0, 1), .owner = WIFIMAC_ENTRY() }, /* I2C MST disable */

        /* PMU to trigger disable RXBLOCK */
        [21] = { .config = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x15), WDEVTXQ_BLOCK,                    0,                         0x6000,     0, 1), .owner = WIFIMAC_ENTRY() },
        [22] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x16), WDEVTXQ_BLOCK,                    WDEV_RXBLOCK,              0x1000,     0, 1), .owner = WIFIMAC_ENTRY() },
        [23] = { .config = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x17), WDEVTXQ_BLOCK,                    0,                         0x6000,     0, 1), .owner = WIFIMAC_ENTRY() },

        [24] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x18), PMU_SLP_WAKEUP_CNTL7_REG,         0x200000,                  0xffff0000, 1, 0), .owner = WIFIMAC_ENTRY() },
        [25] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x19), PMU_SLP_WAKEUP_CNTL7_REG,         0x9730000,                 0xffff0000, 0, 1), .owner = WIFIMAC_ENTRY() }
    };
    extern uint32_t phy_ana_i2c_master_burst_rf_onoff(bool on);
    wifi_modem_config[4].config.write_wait.value  = phy_ana_i2c_master_burst_rf_onoff(true);
    wifi_modem_config[15].config.write_wait.value = phy_ana_i2c_master_burst_rf_onoff(false);
    esp_err_t err = sleep_retention_entries_create(wifi_modem_config, ARRAY_SIZE(wifi_modem_config), 7, SLEEP_RETENTION_MODULE_MODEM_PHY);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate modem phy link for wifi modem state");
    return ESP_OK;
}
#endif

esp_err_t sleep_modem_state_phy_link_init(void **link_head)
{
    esp_err_t err = ESP_OK;

#if SOC_PM_PAU_REGDMA_LINK_IDX_WIFIMAC
    sleep_retention_module_init_param_t init_param = { .cbs = { .create = { .handle = sleep_modem_state_phy_wifi_init, .arg = NULL } } };
    err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_MODEM_PHY, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_MODEM_PHY);
        if (err == ESP_OK) {
            *link_head = sleep_retention_find_link_by_id(REGDMA_PHY_LINK(0x00));
        }
    }
#endif
    return err;
}

esp_err_t sleep_modem_state_phy_link_deinit(void *link_head)
{
    esp_err_t err = ESP_OK;
#if SOC_PM_PAU_REGDMA_LINK_IDX_WIFIMAC
    err = sleep_retention_module_free(SLEEP_RETENTION_MODULE_MODEM_PHY);
    if (err == ESP_OK) {
        sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_MODEM_PHY);
    }
#endif
    return err;
}

#endif /* SOC_PM_SUPPORT_PMU_MODEM_STATE */
