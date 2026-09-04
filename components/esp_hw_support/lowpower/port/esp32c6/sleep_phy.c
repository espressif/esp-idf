/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_attr.h"

#include "soc/soc_caps.h"
#include "soc/pmu_reg.h"

#include "modem/i2c_ana_mst_reg.h"
#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"

#include "esp_private/esp_pau.h"
#include "esp_private/sleep_modem.h"
#include "esp_private/sleep_retention.h"

#if SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY

#define SARADC_TSENS_REG                (0x6000e058)
#define SARADC_TSENS_PU                 (BIT(22))
#define PMU_RF_PWR_REG                  (0x600b0154)

#define FECOEX_SET_FREQ_SET_CHAN_REG    (0x600a00c0)
#define FECOEX_SET_FREQ_RESETN          (BIT(30))
#define FECOEX_SET_CHAN_EN              (BIT(14))
#define FECOEX_SET_FREQ_SET_CHAN_ST_REG (0x600a00cc)
#define FECOEX_SET_CHAN_DONE            (BIT(8))
#define FECOEX_AGC_CONF_REG             (0x600a7030)
#define FECOEX_AGC_DIS                  (BIT(29))

#define WDEVTXQ_BLOCK                   (0x600A4ca8)
#define WDEV_RXBLOCK                    (BIT(12))
#define MODEM_FE_DATA_BASE              (0x600a0400)
#define MODEM_FE_CTRL_BASE              (0x600a0800)

typedef struct {
    void *link_head;
#define DESC_IDX_I2C_MST_ENA (0)
#define DESC_IDX_I2C_MST_SEL (1)
#define DESC_IDX_I2C_MST_DIS (2)
#define DESC_IDX_SKIP_WIFI               (3)
#define DESC_SKIP_WIFI_ENTRY_CNT         (8)
#define DESC_SKIP_WIFI_RESTORE_ENTRY_CNT (1)
    void *regdma_desc[DESC_IDX_I2C_MST_DIS + 1 + DESC_SKIP_WIFI_ENTRY_CNT];
} sleep_phy_link_context_t;

esp_err_t sleep_phy_link_init(void **link_context)
{
    esp_err_t err = ESP_OK;

#if SOC_PM_PAU_REGDMA_LINK_MODEM
    static regdma_link_config_t phy_modem_config[] = {
        [0] = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEM_FE_LINK(0), MODEM_FE_DATA_BASE, MODEM_FE_DATA_BASE, 41, 0, 0),
        [1] = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_MODEM_FE_LINK(1), MODEM_FE_CTRL_BASE, MODEM_FE_CTRL_BASE, 87, 0, 0),

        [2]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x00), MODEM_LPCON_CLK_CONF_REG,         MODEM_LPCON_CLK_I2C_MST_EN,        MODEM_LPCON_CLK_I2C_MST_EN_M,       1, 0), /* I2C MST enable */
        [3]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x01), MODEM_LPCON_I2C_MST_CLK_CONF_REG, MODEM_LPCON_CLK_I2C_MST_SEL_160M,  MODEM_LPCON_CLK_I2C_MST_SEL_160M_M, 1, 0), /* I2C MST sel 160m enable */

        /* Reset SET_FREQ fsm */
        [4]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x02), FECOEX_SET_FREQ_SET_CHAN_REG,     0,    FECOEX_SET_FREQ_RESETN,                         1, 0),
        /* PMU or software to trigger enable RF PHY */
        [5]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x03), I2C_ANA_MST_ANA_CONF0_REG,        0x8,                       0xc,        1, 0), /* BBPLL calibration enable */
        [6]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x04), PMU_RF_PWR_REG,                   0xf0000000,                0xf0000000, 1, 0),
        [7]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x05), SARADC_TSENS_REG,                 SARADC_TSENS_PU,           0x400000,   1, 0),
        [8]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x06), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 1, 0),
        [9]  = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x07), PMU_DATE_REG,                     ~I2C_ANA_MST_BURST_DONE,   0x1,        1, 0),
        [10] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x08), PMU_DATE_REG,                     ~I2C_ANA_MST_BURST_DONE,   0x1,        1, 0),
        [11] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x09), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_BURST_DONE,    0x1,        1, 0),
        [12] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0a), FECOEX_SET_FREQ_SET_CHAN_REG,     FECOEX_SET_FREQ_RESETN,    FECOEX_SET_FREQ_RESETN,          1, 0),
        [13] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0b), FECOEX_SET_FREQ_SET_CHAN_REG,     FECOEX_SET_CHAN_EN,        0x4000,     1, 0),
        [14] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0c), FECOEX_SET_FREQ_SET_CHAN_REG,     0,                         0x4000,     1, 0),
        [15] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0d), PMU_DATE_REG,                     ~FECOEX_SET_CHAN_DONE,     0x100,      1, 0),
        [16] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x0e), PMU_DATE_REG,                     ~FECOEX_SET_CHAN_DONE,     0x100,      1, 0),
        [17] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x0f), FECOEX_SET_FREQ_SET_CHAN_ST_REG,  FECOEX_SET_CHAN_DONE,      0x100,      1, 0),
        [18] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x10), MODEM_SYSCON_WIFI_BB_CFG_REG,     BIT(1),                    0x2,        1, 0),
        [19] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x11), FECOEX_AGC_CONF_REG,              0,                         0x20000000, 1, 0),

        /* PMU to trigger enable RXBLOCK */
        [20] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x12), WDEVTXQ_BLOCK,                    0,                         0x1000,     1, 0),

        /* PMU or software to trigger disable RF PHY */
        [21] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x13), FECOEX_AGC_CONF_REG,              FECOEX_AGC_DIS,            0x20000000, 0, 1),
        [22] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x14), MODEM_SYSCON_WIFI_BB_CFG_REG,     0,                         0x2,        0, 1),
        [23] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x15), FECOEX_SET_FREQ_SET_CHAN_REG,     0,                         0x4000,     0, 1),
        [24] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x16), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 0, 1),
        [25] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x17), PMU_DATE_REG,                     ~I2C_ANA_MST_BURST_DONE,   0x1,        0, 1),
        [26] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x18), PMU_DATE_REG,                     ~I2C_ANA_MST_BURST_DONE,   0x1,        0, 1),
        [27] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x19), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_BURST_DONE,    0x1,        0, 1),
        [28] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1a), SARADC_TSENS_REG,                 0,                         0x400000,   0, 1),
        [29] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1b), PMU_RF_PWR_REG,                   0,                         0xf0000000, 0, 1),
        [30] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1c), I2C_ANA_MST_ANA_CONF0_REG,        0x4,                       0xc,        0, 1), /* BBPLL calibration disable */

        [31] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1d), MODEM_LPCON_CLK_CONF_REG,         0,                         MODEM_LPCON_CLK_I2C_MST_EN_M,       0, 1), /* I2C MST disable */

        /* PMU to trigger disable RXBLOCK */
        [32] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1e), PMU_DATE_REG,                     ~0,                        0x6000,     0, 1),
        [33] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x1f), PMU_DATE_REG,                     ~0,                        0x6000,     0, 1),
        [34] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x20), WDEVTXQ_BLOCK,                    0,                         0x6000,     0, 1),
        [35] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x21), WDEVTXQ_BLOCK,                    WDEV_RXBLOCK,              0x1000,     0, 1),
        [36] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x22), PMU_DATE_REG,                     ~0,                        0x6000,     0, 1),
        [37] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x23), PMU_DATE_REG,                     ~0,                        0x6000,     0, 1),
        [38] = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x24), WDEVTXQ_BLOCK,                    0,                         0x6000,     0, 1),

        [39] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x25), PMU_SLP_WAKEUP_CNTL7_REG,         0x200000,                  0xffff0000, 1, 0),
        [40] = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x26), PMU_SLP_WAKEUP_CNTL7_REG,         0x9730000,                 0xffff0000, 0, 1)
    };
    extern uint32_t phy_ana_i2c_master_burst_rf_onoff(bool on);
    phy_modem_config[8].write_wait.value  = phy_ana_i2c_master_burst_rf_onoff(true);
    phy_modem_config[24].write_wait.value = phy_ana_i2c_master_burst_rf_onoff(false);

    void *link = NULL;
    for (int i = ARRAY_SIZE(phy_modem_config) - 1; (err == ESP_OK) && (i >= 0); i--) {
        void *next = regdma_link_init_safe(&phy_modem_config[i], false, 0, link);
        if (next) {
            link = next;
        } else {
            regdma_link_destroy(link, 0);
            err = ESP_ERR_NO_MEM;
        }
    }
    if (err == ESP_OK) {
        pau_regdma_set_modem_link_addr(link);

        const int id_array[] = { REGDMA_PHY_LINK(0x00), REGDMA_PHY_LINK(0x01), REGDMA_PHY_LINK(0x1d),                           /* I2C MST CLK entries */
                                 REGDMA_PHY_LINK(0x12), REGDMA_PHY_LINK(0x1e), REGDMA_PHY_LINK(0x1f), REGDMA_PHY_LINK(0x20),
                                 REGDMA_PHY_LINK(0x21), REGDMA_PHY_LINK(0x22), REGDMA_PHY_LINK(0x23), REGDMA_PHY_LINK(0x24)     /* WiFi Related entries */
                                };
        static DRAM_ATTR sleep_phy_link_context_t phy_link_context;

        for (int i = 0; (err == ESP_OK) && (i < ARRAY_SIZE(phy_link_context.regdma_desc)); i++) {
            void *desc = regdma_find_link_by_id(link, 0, id_array[i]);
            if (desc) {
                phy_link_context.regdma_desc[i] = desc;
            } else {
                err = ESP_ERR_NOT_FOUND;
            }
        }
        if (err == ESP_OK) {
            phy_link_context.link_head = link;
            *link_context = (void *)&phy_link_context;
        } else {
            regdma_link_destroy(link, 0);
        }
    }
#endif
    return err;
}

void IRAM_ATTR sleep_phy_link_config(void *link_context, uint32_t flags)
{
    sleep_phy_link_context_t *phy_link_context = (sleep_phy_link_context_t *)link_context;

    if (flags & SLEEP_MODEM_SKIP_I2C_MST_CLK_RETENTION) {
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_ENA], true, true);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_SEL], true, true);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_DIS], true, true);
    } else {
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_ENA], true, false);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_SEL], true, false);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_DIS], false, true);
    }

    if (flags & SLEEP_MODEM_SKIP_WIFI_RETENTION) {
        for (int i = 0; i < DESC_SKIP_WIFI_ENTRY_CNT; i++) {
            regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_SKIP_WIFI + i], true, true);
        }
    } else {
        for (int i = 0; i < DESC_SKIP_WIFI_ENTRY_CNT; i++) {
            if (i < DESC_SKIP_WIFI_RESTORE_ENTRY_CNT) {
                regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_SKIP_WIFI + i], true, false);
            } else {
                regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_SKIP_WIFI + i], false, true);
            }
        }
    }
}

esp_err_t sleep_phy_link_deinit(void *link_context)
{
#if SOC_PM_PAU_REGDMA_LINK_MODEM
    regdma_link_destroy(((sleep_phy_link_context_t *)link_context)->link_head, 0);
#endif
    return ESP_OK;
}
#endif /* SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY */
