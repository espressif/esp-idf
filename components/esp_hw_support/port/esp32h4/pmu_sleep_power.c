/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_check.h"
#include "esp_macros.h"
#include "esp_pmu.h"

#include "soc/pmu_reg.h"

#include "esp_private/esp_regdma.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/startup_internal.h"

#include "pmu_param.h"

ESP_LOG_ATTR_TAG(TAG, "sleep_power");

#if SOC_PM_MODEM_LOCK_CLK_WORKAROUND

static esp_err_t sleep_power_system_retention_init(void *arg)
{
    const static sleep_retention_entries_config_t power_regs_retention[] = {
        /* During the modem-to-active transition, the MODEM lock keeps the xtalx2, bbpll xpd status unchanged.
        * Therefore, if xtalx2, bbpll is disabled in modem state, we need to set xtalx2, bbpll tie-high to enable it in active state. */
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(0),   PMU_IMM_HP_CK_POWER_REG,   PMU_TIE_HIGH_XTALX2    | PMU_TIE_HIGH_GLOBAL_XTALX2_ICG,  PMU_TIE_HIGH_XTALX2_M    | PMU_TIE_HIGH_GLOBAL_XTALX2_ICG_M,  1,  0),  .owner = ENTRY(2) },
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(1),   PMU_IMM_HP_CK_POWER_REG,   PMU_TIE_HIGH_XPD_BBPLL | PMU_TIE_HIGH_XPD_BBPLL_I2C,      PMU_TIE_HIGH_XPD_BBPLL_M | PMU_TIE_HIGH_XPD_BBPLL_I2C_M,      1,  0),  .owner = ENTRY(2) },
    };
    esp_err_t err = sleep_retention_entries_create(power_regs_retention, ARRAY_SIZE(power_regs_retention), REGDMA_LINK_PRI_POWER, SLEEP_RETENTION_MODULE_POWER);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for clock power retention");
    ESP_LOGI(TAG, "Clock power sleep retention initialization");
    return ESP_OK;
}

#endif

#define SLEEP_POWER_ANA_WAIT_LINK_BASE          10
#define SLEEP_POWER_ANA_WAIT_LINK_ID(_idx)      REGDMA_POWER_LINK((SLEEP_POWER_ANA_WAIT_LINK_BASE + (_idx)))

typedef struct {
    void *regdma_desc[ANALOG_WAIT_CTRL_NUM];
} pmu_sleep_power_ana_wait_context_t;

static esp_err_t sleep_power_analog_wait_ctrl_init(void *arg)
{
    const static sleep_retention_entries_config_t analog_wait_ctrl_retention[] = {
        /* Sleep to Modem retention has passed hp analog wait duration, so Modem to Sleep retention can reduce this time.
         * So here Sleep to Modem retention set the analog wait duration to about 1.6us, and Modem to Sleep retention set the analog wait duration to origin value. */
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(10),   PMU_SLP_WAKEUP_CNTL7_REG,   (0x20 << PMU_ANA_WAIT_TARGET_S),   PMU_ANA_WAIT_TARGET_M,  1,  0),  .owner = ENTRY(1) },
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(11),   PMU_SLP_WAKEUP_CNTL7_REG,   (PMU_HP_ANALOG_WAIT_TARGET_CYCLES << PMU_ANA_WAIT_TARGET_S),  PMU_ANA_WAIT_TARGET_M,  0,  1),  .owner = ENTRY(1) },
        [2] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_POWER_LINK(12),   PMU_SLP_WAKEUP_CNTL7_REG,   (PMU_HP_ANALOG_WAIT_TARGET_CYCLES << PMU_ANA_WAIT_TARGET_S),  PMU_ANA_WAIT_TARGET_M,  1,  0),  .owner = ENTRY(2) },
    };
    esp_err_t err = sleep_retention_entries_create(analog_wait_ctrl_retention, ARRAY_SIZE(analog_wait_ctrl_retention), REGDMA_LINK_PRI_POWER, SLEEP_RETENTION_MODULE_POWER);

    pmu_sleep_power_ana_wait_context_t *ana_wait_context = (pmu_sleep_power_ana_wait_context_t *)arg;
    for (int i = 0; i < ANALOG_WAIT_CTRL_NUM; i++) {
        if (!ana_wait_context->regdma_desc[i]) {
            void *head = sleep_retention_find_link_by_id(SLEEP_POWER_ANA_WAIT_LINK_ID(i));
            if (head) {
                ana_wait_context->regdma_desc[i] = head;
            }
        }
    }
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for analog wait ctrl retention");
    return ESP_OK;
}

static esp_err_t sleep_power_retention_init(void *arg)
{
#if SOC_PM_MODEM_LOCK_CLK_WORKAROUND
    ESP_RETURN_ON_ERROR(sleep_power_system_retention_init(arg), TAG, "system retention init failed");
#endif
    ESP_RETURN_ON_ERROR(sleep_power_analog_wait_ctrl_init(arg), TAG, "analog wait ctrl retention init failed");
    return ESP_OK;
}

static esp_err_t sleep_power_retention_deinit(void *arg)
{
    pmu_sleep_power_ana_wait_context_t *ana_wait_ctx = (pmu_sleep_power_ana_wait_context_t *)arg;
    for (int i = 0; i < ANALOG_WAIT_CTRL_NUM; i++) {
        ana_wait_ctx->regdma_desc[i] = NULL;
    }
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(sleep_power_startup_init, SECONDARY, BIT(0), 108)
{
    static DRAM_ATTR pmu_sleep_power_ana_wait_context_t ana_wait_ctx;

    sleep_retention_module_init_param_t init_param = {
        .cbs = {
            .create  = { .handle = sleep_power_retention_init,   .arg = &ana_wait_ctx },
            .destroy = { .handle = sleep_power_retention_deinit, .arg = &ana_wait_ctx },
        },
        .attribute = SLEEP_RETENTION_MODULE_ATTR_PASSIVE | SLEEP_RETENTION_MODULE_ATTR_ATTACH
    };

    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_POWER, &init_param);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to init power retention module, err=%d", err);
    } else {
        PMU_instance()->priv = &ana_wait_ctx;
    }

    return ESP_OK;
}

void pmu_sleep_power_analog_wait_config(void *ana_wait_context, const uint16_t analog_wait[ANALOG_WAIT_CTRL_NUM])
{
    if (!ana_wait_context) {
        return;
    }

    pmu_sleep_power_ana_wait_context_t *ana_wait_ctx = (pmu_sleep_power_ana_wait_context_t *)ana_wait_context;
    for (int i = 0; i < ANALOG_WAIT_CTRL_NUM; i++) {
        if (!ana_wait_ctx->regdma_desc[i]) {
            continue;
        }
        regdma_link_set_write_wait_content(ana_wait_ctx->regdma_desc[i], (uint32_t)analog_wait[i] << PMU_ANA_WAIT_TARGET_S, PMU_ANA_WAIT_TARGET_M);
    }
}
