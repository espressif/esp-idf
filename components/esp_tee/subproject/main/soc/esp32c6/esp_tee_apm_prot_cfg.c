/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_tee.h"
#include "esp_tee_intr.h"

#include "hal/apm_hal.h"
#include "soc/soc.h"
#include "soc/spi_mem_reg.h"
#include "soc/efuse_reg.h"

extern void tee_apm_violation_isr(void *arg);

static const char *TAG = "esp_tee_apm_prot_cfg";

/* NOTE: Figuring out the eFuse protection range based on where the TEE secure storage key is stored */
#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
#if CONFIG_SECURE_TEE_SEC_STG_KEY_EFUSE_BLK > 9
#error "TEE: eFuse protection region for APM out of range! (see CONFIG_SECURE_TEE_SEC_STG_KEY_EFUSE_BLK)"
#endif
#define LP_APM_EFUSE_REG_START \
    (EFUSE_RD_KEY0_DATA0_REG + (((CONFIG_SECURE_TEE_SEC_STG_KEY_EFUSE_BLK) - 4) * 0x20))

#define LP_APM_EFUSE_REG_END \
    (EFUSE_RD_KEY1_DATA0_REG + (((CONFIG_SECURE_TEE_SEC_STG_KEY_EFUSE_BLK) - 4) * 0x20))
#elif CONFIG_SECURE_TEE_SEC_STG_MODE_DEVELOPMENT
#define LP_APM_EFUSE_REG_START EFUSE_RD_KEY5_DATA0_REG
#if CONFIG_SECURE_TEE_TEST_MODE
#define LP_APM_EFUSE_REG_END   EFUSE_RD_SYS_PART2_DATA0_REG
#else
#define LP_APM_EFUSE_REG_END   LP_APM_EFUSE_REG_START
#endif
#endif

/*----------------HP APM Configuration-----------------------*/

/* HP APM Range and Filter configuration. */
apm_ctrl_region_config_data_t hp_apm_pms_data[] = {
    /* Region0: LP memory region access. (RWX)*/
    {
        .regn_num = 0,
        .regn_start_addr = SOC_RTC_IRAM_LOW,
        .regn_end_addr   = SOC_RTC_IRAM_HIGH,
        .regn_pms        = 0x7,
        .filter_enable   = 1,
    },
    /* Peripherals region access.(RW)*/
    /* Region1: Next 2 entries for MMU peripheral protection. */
    {
        .regn_num = 1,
        .regn_start_addr = SOC_PERIPHERAL_LOW,
        .regn_end_addr   = (DR_REG_INTERRUPT_MATRIX_BASE - 0x4),
        .regn_pms        = 0x6,
        .filter_enable   = 1,
    },
    /* Region4: Interrupt Matrix protection. */
    {
        .regn_num = 2,
        .regn_start_addr = DR_REG_ATOMIC_BASE,
        .regn_end_addr   = (DR_REG_AES_BASE - 0x4),
        .regn_pms        = 0x6,
        .filter_enable   = 1,
    },
    /* Region5: Peripherals region access. (RW)*/
    {
        .regn_num = 3,
        .regn_start_addr = DR_REG_RSA_BASE,
        .regn_end_addr   = (DR_REG_TEE_BASE - 0x4),
        .regn_pms        = 0x6,
        .filter_enable   = 1,
    },
    /* Region6: Peripherals region access. (RW)*/
    {
        .regn_num = 4,
        .regn_start_addr = DR_REG_MISC_BASE,
        .regn_end_addr   = (DR_REG_PMU_BASE - 0x04),
        .regn_pms        = 0x6,
        .filter_enable   = 1,
    },
    /* Region7: Peripherals region access. (RW)*/
    {
        .regn_num = 5,
        .regn_start_addr = DR_REG_OPT_DEBUG_BASE,
        .regn_end_addr   = 0x600D0000, //PWDET_CONF_REG,
        .regn_pms        = 0x6,
        .filter_enable   = 1,
    },
    /* Region8: IRAM region access. (RW)*/
    {
        .regn_num = 6,
        .regn_start_addr = SOC_NS_IRAM_START,
        .regn_end_addr   = SOC_IRAM_HIGH,
        .regn_pms        = 0x6,
        .filter_enable   = 1,
    },
};

/* NOTE: Following are the master IDs for setting the security mode and access through APM:
 * +---------+-------------+
 * |   Bit   |   Source    |
 * +---------+-------------+
 * | 0       | HP CPU      |
 * | 1       | LP CPU      |
 * | 2       | reserved    |
 * | 3       | SDIO_SLV    |
 * | 4       | reserved    |
 * | 5       | MEM_MONITOR |
 * | 6       | TRACE       |
 * | 7~15    | reserved    |
 * | 16      | SPI2        |
 * | 17      | Dummy-1     |
 * | 18      | UHCI        |
 * | 19      | I2S         |
 * | 20      | Dummy-4     |
 * | 21      | Dummy-5     |
 * | 22      | AES         |
 * | 23      | SHA         |
 * | 24      | ADC         |
 * | 25      | PARLIO      |
 * | 26~31   | Dummy-10~15 |
 * +---------+-------------+
 */

/* HP APM configuration for the Masters. */
apm_ctrl_secure_mode_config_t hp_apm_sec_mode_data = {
    .apm_ctrl   = HP_APM_CTRL,
    .apm_m_cnt  = HP_APM_MAX_ACCESS_PATH,
    .sec_mode   = APM_LL_SECURE_MODE_REE0,
    /* Except crypto DMA (AES and SHA) and HP CPU, all other masters will be switch to REE0 mode - refer above note */
    .master_ids = 0xFF3FFFFE,
    .pms_data   = hp_apm_pms_data,
};

/*----------------HP APM TEE Configuration-----------------------*/

/* HP APM Range and Filter configuration. */
apm_ctrl_region_config_data_t hp_apm_pms_data_tee[] = {
    /* Region9: TEE All access enable. (RW)*/
    {
        .regn_num = 7,
        .regn_start_addr = 0x0,
        .regn_end_addr   = ~0x0,
        .regn_pms        = 0x7,
        .filter_enable   = 1,
    },
};

apm_ctrl_secure_mode_config_t hp_apm_sec_mode_data_tee = {
    .apm_ctrl   = HP_APM_CTRL,
    .apm_m_cnt  = HP_APM_MAX_ACCESS_PATH,
    .sec_mode   = APM_LL_SECURE_MODE_TEE,
    /* Crypto DMA will be switch to TEE mode - refer above note*/
    .master_ids = 0xC00001,
    .pms_data   = hp_apm_pms_data_tee,
};

/*----------------LP APM0 Configuration-----------------------*/

/* LP APM0 Range and Filter configuration. */
apm_ctrl_region_config_data_t lp_apm0_pms_data[] = {
    /* Region0: LP memory. (RWX) */
    {
        .regn_num = 0,
        .regn_start_addr = SOC_RTC_IRAM_LOW,
        .regn_end_addr   = SOC_RTC_IRAM_HIGH,
        .regn_pms        = 0x7,
        .filter_enable   = 1,
    },
};

/* LP APM0 configuration for the Masters. */
apm_ctrl_secure_mode_config_t lp_apm0_sec_mode_data = {
    .apm_ctrl   = LP_APM0_CTRL,
    .apm_m_cnt  = LP_APM0_MAX_ACCESS_PATH,
    .sec_mode   = APM_LL_SECURE_MODE_REE0,
    .master_ids = 0x2, //Only LP_CPU here.
    .pms_data   = lp_apm0_pms_data,
};

/*----------------LP APM Configuration-----------------------*/

/* LP APM Range and Filter configuration. */
apm_ctrl_region_config_data_t lp_apm_pms_data[] = {
    /* Region0: LP memory. (RWX) */
    {
        .regn_num = 0,
        .regn_start_addr = SOC_RTC_IRAM_LOW,
        .regn_end_addr   = SOC_RTC_IRAM_HIGH,
        .regn_pms        = 0x7,
        .filter_enable   = 1,
    },
    /* Region1: LP Peri 1. (RW) */
    {
        .regn_num = 1,
        .regn_start_addr = DR_REG_PMU_BASE,
        .regn_end_addr   = (LP_APM_EFUSE_REG_START - 0x04),
        .regn_pms        = 0x6,
        .filter_enable   = 1,
    },
    /* Region2: LP Peri 2. (RW) */
    {
        .regn_num = 2,
        .regn_start_addr = LP_APM_EFUSE_REG_END,
        .regn_end_addr   = (DR_REG_TRACE_BASE - 0x04),
        .regn_pms        = 0x6,
        .filter_enable   = 1,
    },
};

/* LP APM configuration for the Masters. */
apm_ctrl_secure_mode_config_t lp_apm_sec_mode_data = {
    .apm_ctrl   = LP_APM_CTRL,
    .apm_m_cnt  = LP_APM_MAX_ACCESS_PATH,
    .sec_mode   = APM_LL_SECURE_MODE_REE0,
    .master_ids = 0x3, //Only HP_CPU & LP_CPU here.
    .pms_data   = lp_apm_pms_data,
};

/* LP APM Range and Filter configuration. */
apm_ctrl_region_config_data_t lp_apm_pms_data_tee[] = {
    /* Region3: TEE All access enable. (RW)*/
    {
        .regn_num = 3,
        .regn_start_addr = 0x0,
        .regn_end_addr   = ~0x0,
        .regn_pms        = 0x7,
        .filter_enable   = 1,
    },
};

apm_ctrl_secure_mode_config_t lp_apm_sec_mode_data_tee = {
    .apm_ctrl   = LP_APM_CTRL,
    .apm_m_cnt  = LP_APM_MAX_ACCESS_PATH,
    .sec_mode   = APM_LL_SECURE_MODE_TEE,
    /* HP CPU and LP CPU will be switch to TEE mode */
    .master_ids = 0x00003,
    .pms_data   = lp_apm_pms_data_tee,
};

/*---------------- TEE APM API-----------------------*/

void esp_tee_apm_int_enable(apm_ctrl_secure_mode_config_t *sec_mode_data)
{
    for (int i = 0; i < sec_mode_data->apm_m_cnt; i++) {
        apm_ctrl_path_t *apm_excp_data = calloc(1, sizeof(apm_ctrl_path_t));
        assert(apm_excp_data != NULL);

        apm_excp_data->apm_ctrl = sec_mode_data->apm_ctrl;
        apm_excp_data->apm_m_path = i;

        int intr_src_num = apm_hal_apm_ctrl_get_int_src_num(apm_excp_data);

        struct vector_desc_t apm_vd = {0};
        apm_vd.source = intr_src_num;
        apm_vd.isr = tee_apm_violation_isr;
        apm_vd.arg = (void *)apm_excp_data;

        /* Register interrupt handler with TEE. */
        esp_tee_intr_register((void *)&apm_vd);

        /* Enable APM Ctrl intewrrupt for access path(M[0:n]) */
        apm_hal_apm_ctrl_exception_clear(apm_excp_data);
        apm_hal_apm_ctrl_interrupt_enable(apm_excp_data, true);
    }
}

void esp_tee_configure_apm_protection(void)
{
    /* Disable all control filter first to have full access of address rage. */
    apm_hal_apm_ctrl_filter_enable_all(false);

    /* LP APM0 configuration. */
    lp_apm0_sec_mode_data.regn_count = sizeof(lp_apm0_pms_data) / sizeof(apm_ctrl_region_config_data_t);
    apm_hal_apm_ctrl_master_sec_mode_config(&lp_apm0_sec_mode_data);

    /* LP APM0 interrupt configuration. */
    esp_tee_apm_int_enable(&lp_apm0_sec_mode_data);
    ESP_LOGD(TAG, "[REE0] LP_APM0 configured");

    /* LP APM TEE configuration. */
    lp_apm_sec_mode_data_tee.regn_count = sizeof(lp_apm_pms_data_tee) / sizeof(apm_ctrl_region_config_data_t);
    apm_hal_apm_ctrl_master_sec_mode_config(&lp_apm_sec_mode_data_tee);
    ESP_LOGD(TAG, "[TEE] LP_APM configured");

    /* LP APM configuration. */
    lp_apm_sec_mode_data.regn_count = sizeof(lp_apm_pms_data) / sizeof(apm_ctrl_region_config_data_t);
    apm_hal_apm_ctrl_master_sec_mode_config(&lp_apm_sec_mode_data);
    /* LP APM interrupt configuration. */
    esp_tee_apm_int_enable(&lp_apm_sec_mode_data);
    ESP_LOGD(TAG, "[REE0] LP_APM configured");

    /* HP APM TEE configuration. */
    hp_apm_sec_mode_data_tee.regn_count = sizeof(hp_apm_pms_data_tee) / sizeof(apm_ctrl_region_config_data_t);
    apm_hal_apm_ctrl_master_sec_mode_config(&hp_apm_sec_mode_data_tee);
    ESP_LOGD(TAG, "[TEE] HP_APM configured");

    /* HP APM configuration. */
    hp_apm_sec_mode_data.regn_count = sizeof(hp_apm_pms_data) / sizeof(apm_ctrl_region_config_data_t);
    apm_hal_apm_ctrl_master_sec_mode_config(&hp_apm_sec_mode_data);
    /* HP APM interrupt configuration. */
    esp_tee_apm_int_enable(&hp_apm_sec_mode_data);
    ESP_LOGD(TAG, "[REE0] HP_APM configured");
}
