/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/
#include "soc/soc_caps.h"
#include "modem/i2c_ana_mst_reg.h"
#include "soc/apb_saradc_reg.h"
#include "soc/pmu_reg.h"
#include "soc/pcr_reg.h"
#include "esp_check.h"

#include "modem/modem_syscon_reg.h"
#include "modem/modem_lpcon_reg.h"

#include "esp_private/esp_pau.h"
#include "esp_private/sleep_modem.h"
#include "esp_private/sleep_retention.h"
#include "soc/gpio_reg.h"

#if SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY

#define FECOEX_SET_FREQ_SET_FREQ_REG    (0x600C001c)
#define FECOEX_SET_FREQ_EN_SW           (BIT(19))
#define FECOEX_FREQ_CTRL1_REG           (0x600C0020)
#define FECOEX_SET_FREQ_RESETN          (BIT(18))
#define FECOEX_SET_FREQ_SET_FREQ_ST_REG (0x600c0028)
#define FECOEX_SET_FREQ_DONE            (BIT(8))
#define FECOEX_BT_CHAN_FORCE_START      (2)
#define FECOEX_BT_CHAN_FORCE_END        (3)
#define FECOEX_BT_CHAN_FORCE_MASK       (0x3)

#define PCR_SDM_INV_PHASE_CONF_REG (0x60094184)
#define PCR_CLK_SDM_EN    (BIT(10))

ESP_LOG_ATTR_TAG(TAG, "sleep_phy");
typedef struct {
    #define DESC_IDX_I2C_MST_ENA (0)
    #define DESC_IDX_I2C_MST_DIS (1)
    #define DESC_MODEM_SYSCON_CLK_EN (2)
    #define DESC_MODEM_SYSCON_CLK_DIS (3)
    void *regdma_desc[DESC_MODEM_SYSCON_CLK_DIS + 1];
} sleep_phy_link_context_t;

#define SYSCON_FE_CLOCK_MSK             (MODEM_SYSCON_CLK_FE_APB_EN|MODEM_SYSCON_CLK_FE_32M_EN|MODEM_SYSCON_CLK_FE_SDM_EN|MODEM_SYSCON_CLK_FE_ADC_EN|MODEM_SYSCON_CLK_FE_16M_EN|MODEM_SYSCON_CLK_FE_TXLOGAIN_EN)
esp_err_t sleep_phy_retention_init(void *args)
{
    #define PHY_ENTRY() (BIT(SOC_PM_PAU_REGDMA_LINK_IDX_PHY))
    static sleep_retention_entries_config_t phy_modem_config[] = {
        /* Open modem clock for PHY */
        [0] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x00),      MODEM_LPCON_CLK_CONF_REG,         MODEM_LPCON_CLK_I2C_MST_EN, MODEM_LPCON_CLK_I2C_MST_EN_M, 1, 0), .owner = PHY_ENTRY() }, /* I2C MST enable */
        [1] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x01),      MODEM_SYSCON_CLK_CONF1_REG,       SYSCON_FE_CLOCK_MSK,                              SYSCON_FE_CLOCK_MSK,        1, 0), .owner = PHY_ENTRY() }, /* FE clock */
        [2] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x02),      FECOEX_FREQ_CTRL1_REG,           0,    FECOEX_SET_FREQ_RESETN,                         1, 0), .owner = PHY_ENTRY() },
        [3] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x03),      PCR_SDM_INV_PHASE_CONF_REG,       PCR_CLK_SDM_EN,                              PCR_CLK_SDM_EN,                  1, 0), .owner = PHY_ENTRY() }, /* PCR */
        [4] =  { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PHY_LINK(0x04), I2C_ANA_MST_I2C0_CTRL1_REG, I2C_ANA_MST_I2C0_CTRL1_REG, 3, 0, 0), .owner = PHY_ENTRY() },

        /* PMU or software to trigger enable RF PHY */
        [5] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x05), I2C_ANA_MST_ANA_CONF0_REG,        0,                         BIT(6),        1, 0), .owner = PHY_ENTRY() }, /* BBPLL calibration enable */
        [6] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x06), PMU_RF_PWC_REG,                   0xfc000000,                0xfc000000, 1, 0), .owner = PHY_ENTRY() },
        [7] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x07), I2C_ANA_MST_ANA_CONF0_REG,        BIT(6),                    BIT(6),        1, 0), .owner = PHY_ENTRY() }, /* BBPLL calibration enable */
        [8] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x08), PCR_TSENS_CLK_CONF_REG,           PCR_TSENS_CLK_EN,       PCR_TSENS_CLK_EN, 1, 0), .owner = PHY_ENTRY() },
        [9] =  { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x09), APB_SARADC_APB_TSENS_CTRL_REG,    APB_SARADC_TSENS_PU,       APB_SARADC_TSENS_PU, 1, 0), .owner = PHY_ENTRY() },
        [10] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0a), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 1, 0), .owner = PHY_ENTRY() },
        [11] = { .config = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x0b), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_I2C_MST_BURST_DONE,    I2C_ANA_MST_I2C_MST_BURST_DONE,        1, 0), .owner = PHY_ENTRY() },
        [12] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0c), FECOEX_FREQ_CTRL1_REG,            FECOEX_SET_FREQ_RESETN,    FECOEX_SET_FREQ_RESETN,                         1, 0), .owner = PHY_ENTRY() },
        [13] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0d), FECOEX_SET_FREQ_SET_FREQ_ST_REG,  FECOEX_BT_CHAN_FORCE_START,FECOEX_BT_CHAN_FORCE_MASK,    1, 0), .owner = PHY_ENTRY() },
        [14] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0e), FECOEX_SET_FREQ_SET_FREQ_REG,     FECOEX_SET_FREQ_EN_SW,     FECOEX_SET_FREQ_EN_SW,    1, 0), .owner = PHY_ENTRY() },
        [15] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x0f), FECOEX_SET_FREQ_SET_FREQ_REG,     0,                         FECOEX_SET_FREQ_EN_SW,    1, 0), .owner = PHY_ENTRY() },
        [16] = { .config = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x10), FECOEX_SET_FREQ_SET_FREQ_ST_REG,  FECOEX_SET_FREQ_DONE,      FECOEX_SET_FREQ_DONE,      1, 0), .owner = PHY_ENTRY() },
        [17] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x11), FECOEX_SET_FREQ_SET_FREQ_ST_REG,  FECOEX_BT_CHAN_FORCE_END,  FECOEX_BT_CHAN_FORCE_MASK,    1, 0), .owner = PHY_ENTRY() },

        /* PMU or software to trigger disable RF PHY */
        [18] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x12), FECOEX_SET_FREQ_SET_FREQ_REG,     0,                         FECOEX_SET_FREQ_EN_SW,    0, 1), .owner = PHY_ENTRY() },
        /* PMU or software to trigger disable RF PHY */
        [19] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x13), I2C_ANA_MST_I2C_BURST_CONF_REG,   0,                         0xffffffff, 0, 1), .owner = PHY_ENTRY() },
        [20] = { .config = REGDMA_LINK_WAIT_INIT (REGDMA_PHY_LINK(0x14), I2C_ANA_MST_I2C_BURST_STATUS_REG, I2C_ANA_MST_I2C_MST_BURST_DONE,    I2C_ANA_MST_I2C_MST_BURST_DONE,        0, 1), .owner = PHY_ENTRY() },
        [21] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x15), APB_SARADC_APB_TSENS_CTRL_REG,    0,                         APB_SARADC_TSENS_PU,0, 1), .owner = PHY_ENTRY() },
        [22] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x16), PCR_TSENS_CLK_CONF_REG,           0,                         PCR_TSENS_CLK_EN,                  0, 1), .owner = PHY_ENTRY() }, /* PCR */
        [23] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x17), PMU_RF_PWC_REG,                   0x8c000000,       0xfc000000, 0, 1), .owner = PHY_ENTRY() },

        [24] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x18), PCR_SDM_INV_PHASE_CONF_REG,       0,                         PCR_CLK_SDM_EN,                  0, 1), .owner = PHY_ENTRY() }, /* PCR */
        [25] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x19), MODEM_SYSCON_CLK_CONF1_REG,       0,                         MODEM_SYSCON_CLK_FE_TXLOGAIN_EN, 0, 1), .owner = PHY_ENTRY() }, /* FE clock */
        [26] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1a), MODEM_LPCON_CLK_CONF_REG,         0,                         MODEM_LPCON_CLK_I2C_MST_EN_M,    0, 1), .owner = PHY_ENTRY() }, /* I2C MST disable */
        [27] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1b), PMU_SLP_WAKEUP_CNTL7_REG,         0x200000,                  0xffff0000, 1, 0), .owner = PHY_ENTRY() },
        [28] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_PHY_LINK(0x1c), PMU_SLP_WAKEUP_CNTL7_REG,         0x9730000,                 0xffff0000, 0, 1), .owner = PHY_ENTRY() },
    };
    extern uint32_t phy_ana_i2c_master_burst_rf_onoff(bool on);
    phy_modem_config[0x0a].config.write_wait.value  = phy_ana_i2c_master_burst_rf_onoff(true);
    phy_modem_config[0x13].config.write_wait.value = phy_ana_i2c_master_burst_rf_onoff(false);
    esp_err_t err = sleep_retention_entries_create(phy_modem_config, ARRAY_SIZE(phy_modem_config), 7, SLEEP_RETENTION_MODULE_MODEM_PHY);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to init modem phy link");
    return err;
}

esp_err_t sleep_phy_link_init(void **link_context)
{
    esp_err_t err = ESP_OK;

#if SOC_PM_PAU_REGDMA_LINK_IDX_PHY
    sleep_retention_module_init_param_t init_param = { .cbs = { .create = { .handle = sleep_phy_retention_init, .arg = NULL } } };
    err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_MODEM_PHY, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_MODEM_PHY);
        if (err == ESP_OK) {
            const int id_array[] = { REGDMA_PHY_LINK(0x00), REGDMA_PHY_LINK(0x1a), /* I2C MST CLK entries */
                                     REGDMA_PHY_LINK(0x01), REGDMA_PHY_LINK(0x19), /* FE modem clock entries */
                                    };
            static DRAM_ATTR sleep_phy_link_context_t phy_link_context;
            for (int i = 0; (err == ESP_OK) && (i < ARRAY_SIZE(phy_link_context.regdma_desc)); i++) {
                void *desc = sleep_retention_find_link_by_id(id_array[i]);
                if (desc) {
                    phy_link_context.regdma_desc[i] = desc;
                } else {
                    err = ESP_ERR_NOT_FOUND;
                    ESP_LOGE(TAG, "failed to find modem phy link: %x", id_array[i]);
                }
            }
            if (err == ESP_OK) {
                *link_context = (void *)&phy_link_context;
            }
        } else {
            ESP_LOGE(TAG, "failed to allocate modem phy link");
        }
    }
    if (err != ESP_OK) {
        sleep_phy_link_deinit(NULL);
    }
#endif
    return err;
}

void IRAM_ATTR sleep_phy_link_config(void *link_context, uint32_t flags)
{
#if SOC_PM_PAU_REGDMA_LINK_IDX_PHY
    sleep_phy_link_context_t *phy_link_context = (sleep_phy_link_context_t *)link_context;

    if (flags & SLEEP_MODEM_SKIP_I2C_MST_CLK_RETENTION) {
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_ENA], true, true);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_DIS], true, true);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_MODEM_SYSCON_CLK_EN], true, true);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_MODEM_SYSCON_CLK_DIS], true, true);
    } else {
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_ENA], true, false);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_IDX_I2C_MST_DIS], false, true);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_MODEM_SYSCON_CLK_EN], true, false);
        regdma_link_set_skip_flag(phy_link_context->regdma_desc[DESC_MODEM_SYSCON_CLK_DIS], false, true);
    }
#endif // SOC_PM_PAU_REGDMA_LINK_IDX_PHY
}

esp_err_t sleep_phy_link_deinit(void *link_head)
{
#if SOC_PM_PAU_REGDMA_LINK_IDX_PHY
    sleep_retention_module_free(SLEEP_RETENTION_MODULE_MODEM_PHY);
    sleep_retention_module_deinit(SLEEP_RETENTION_MODULE_MODEM_PHY);
#endif
    return ESP_OK;
}
#endif /* SOC_PM_SUPPORT_REGDMA_TRIGGERED_PHY */
