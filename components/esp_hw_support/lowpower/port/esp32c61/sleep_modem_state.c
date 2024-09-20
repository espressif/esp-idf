/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/soc_caps.h"
#include "soc/i2c_ana_mst_reg.h"
#include "soc/pmu_reg.h"

#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"

#include "esp_private/esp_pau.h"
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

#define WDEVTXQ_BLOCK                   (0x600a4ca8)
#define WDEV_RXBLOCK                    (BIT(12))

esp_err_t sleep_modem_state_phy_link_init(void **link_head)
{
    esp_err_t err = ESP_OK;

#if SOC_PM_PAU_REGDMA_LINK_WIFIMAC
    static regdma_link_config_t wifi_modem_config[] = {
        [0]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x00), MODEM_LPCON_CLK_CONF_REG,         MODEM_LPCON_CLK_I2C_MST_EN,        MODEM_LPCON_CLK_I2C_MST_EN_M,       1, 0), /* I2C MST enable */

        /* PMU or software to trigger enable RF PHY */
        [1]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x01), I2C_ANA_MST_ANA_CONF0_REG,        0x8,                       0xc,        1, 0), /* BBPLL calibration enable */
        [2]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x02), PMU_RF_PWR_REG,                   0xf0000000,                0xf0000000, 1, 0),
        [3]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x03), SARADC_TSENS_REG,                 SARADC_TSENS_PU,           0x400000,   1, 0),

        [4]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x04), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 1, 0),
        [5]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x05), PMU_DATE_REG,                     ~I2C_ANA_MST_BURST_DONE,   0x1,        1, 0),
        [6]  = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x06), PMU_DATE_REG,                     ~I2C_ANA_MST_BURST_DONE,   0x1,        1, 0),
        [7]  = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x07), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_BURST_DONE,    0x1,        1, 0),

        [8]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x08), FECOEX_SET_FREQ_SET_CHAN_REG,     FECOEX_SET_CHAN_EN,        0x20000,    1, 0),
        [9]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x09), FECOEX_SET_FREQ_SET_CHAN_REG,     0,                         0x20000,    1, 0),
        [10] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0a), PMU_DATE_REG,                     ~FECOEX_SET_CHAN_DONE,     0x100,      1, 0),
        [11] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x0b), PMU_DATE_REG,                     ~FECOEX_SET_CHAN_DONE,     0x100,      1, 0),
        [12] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x0c), FECOEX_SET_FREQ_SET_CHAN_ST_REG,  FECOEX_SET_CHAN_DONE,      0x100,      1, 0),
        [13] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0d), MODEM_SYSCON_WIFI_BB_CFG_REG,     BIT(1),                    0x2,        1, 0),
        [14] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0e), FECOEX_AGC_CONF_REG,              0,                         0x20000000, 1, 0),

        /* PMU to trigger enable RXBLOCK */
        [15] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0f), WDEVTXQ_BLOCK,                    0,                         0x1000,     1, 0),

        /* PMU or software to trigger disable RF PHY */
        [16] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x10), FECOEX_AGC_CONF_REG,              FECOEX_AGC_DIS,            0x20000000, 0, 1),
        [17] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x11), MODEM_SYSCON_WIFI_BB_CFG_REG,     0,                         0x2,        0, 1),
        [18] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x12), FECOEX_SET_FREQ_SET_CHAN_REG,     0,                         0x4000,     0, 1),
        [19] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x13), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 0, 1),
        [20] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x14), PMU_DATE_REG,                     ~I2C_ANA_MST_BURST_DONE,   0x1,        0, 1),
        [21] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x15), PMU_DATE_REG,                     ~I2C_ANA_MST_BURST_DONE,   0x1,        0, 1),
        [22] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x16), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_BURST_DONE,    0x1,        0, 1),
        [23] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x17), SARADC_TSENS_REG,                 0,                         0x400000,   0, 1),
        [24] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x18), PMU_RF_PWR_REG,                   0,                         0xf0000000, 0, 1),
        [25] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x19), I2C_ANA_MST_ANA_CONF0_REG,        0x4,                       0xc,        0, 1), /* BBPLL calibration disable */

        [26] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1a), MODEM_LPCON_CLK_CONF_REG,         0,                         MODEM_LPCON_CLK_I2C_MST_EN_M,       0, 1), /* I2C MST disable */

        /* PMU to trigger disable RXBLOCK */
        [27] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1b), PMU_DATE_REG,                     ~0,                        0x6000,     0, 1),
        [28] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x1c), PMU_DATE_REG,                     ~0,                        0x6000,     0, 1),
        [29] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x1d), WDEVTXQ_BLOCK,                    0,                         0x6000,     0, 1),
        [30] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1e), WDEVTXQ_BLOCK,                    WDEV_RXBLOCK,              0x1000,     0, 1),
        [31] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1f), PMU_DATE_REG,                     ~0,                        0x6000,     0, 1),
        [32] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x20), PMU_DATE_REG,                     ~0,                        0x6000,     0, 1),
        [33] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x21), WDEVTXQ_BLOCK,                    0,                         0x6000,     0, 1),

        [34] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x22), PMU_SLP_WAKEUP_CNTL7_REG,         0x200000,                  0xffff0000, 1, 0),
        [35] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x23), PMU_SLP_WAKEUP_CNTL7_REG,         0x9730000,                 0xffff0000, 0, 1)
    };
    extern uint32_t phy_ana_i2c_master_burst_rf_onoff(bool on);
    wifi_modem_config[4].write_wait.value  = phy_ana_i2c_master_burst_rf_onoff(true);
    wifi_modem_config[19].write_wait.value = phy_ana_i2c_master_burst_rf_onoff(false);

    void *link = NULL;
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
        *link_head = link;
    }
#endif
    return err;
}

esp_err_t sleep_modem_state_phy_link_deinit(void *link_head)
{
#if SOC_PM_PAU_REGDMA_LINK_WIFIMAC
    regdma_link_destroy(link_head, 0);
#endif
    return ESP_OK;
}

#endif /* SOC_PM_SUPPORT_PMU_MODEM_STATE */
