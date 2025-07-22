/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/spi_mem_reg.h"
#include "soc/efuse_reg.h"
#include "soc/pcr_reg.h"

#include "soc/apm_defs.h"
#include "hal/apm_types.h"
#include "hal/apm_hal.h"

#include "esp_log.h"
#include "esp_tee.h"
#include "esp_tee_intr.h"

extern void tee_apm_violation_isr(void *arg);

static const char *TAG = "esp_tee_apm_prot_cfg";

/* NOTE: Figuring out the eFuse protection range based on where the TEE secure storage key is stored */
#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
#if CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID < 0
#error "TEE: eFuse protection region for APM out of range! (see CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID)"
#endif
#define LP_APM_EFUSE_REG_START \
    (EFUSE_RD_KEY0_DATA0_REG + (CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID * 0x20))

#define LP_APM_EFUSE_REG_END \
    (EFUSE_RD_KEY1_DATA0_REG + (CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID * 0x20))
#elif CONFIG_SECURE_TEE_SEC_STG_MODE_DEVELOPMENT
#define LP_APM_EFUSE_REG_START EFUSE_RD_KEY5_DATA0_REG
#if CONFIG_SECURE_TEE_TEST_MODE
#define LP_APM_EFUSE_REG_END   EFUSE_RD_SYS_PART2_DATA0_REG
#else
#define LP_APM_EFUSE_REG_END   LP_APM_EFUSE_REG_START
#endif
#endif

/* NOTE: Flash protection over the SPI1 controller */
#define HP_APM_SPI1_REG_START    DR_REG_SPI1_BASE
#if CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1
#define HP_APM_SPI1_REG_END      DR_REG_I2C_EXT_BASE
#else
#define HP_APM_SPI1_REG_END      HP_APM_SPI1_REG_START
#endif

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

#define APM_MASTERS_ALL               (0xFFFFFFFFU)
#define APM_MASTERS_HP_CPU            (BIT(APM_MASTER_HPCORE))
#define APM_MASTERS_LP_CPU            (BIT(APM_MASTER_LPCORE))
#define APM_MASTERS_GDMA_CRYPTO       (BIT(APM_MASTER_GDMA_AES) | BIT(APM_MASTER_GDMA_SHA))
#define APM_MASTERS_TEE               (APM_MASTERS_HP_CPU | APM_MASTERS_GDMA_CRYPTO)
#define APM_MASTERS_REE               (APM_MASTERS_ALL & ~(APM_MASTERS_TEE))

/*----------------------- TEE mode configuration -----------------------*/

/**
 * NOTE: Fixes APM filter behavior to allow unrestricted access for TEE mode.
 *
 * By default, TEE mode should have unrestricted access to the entire CPU address space.
 * However, it has been observed that when APM filters are enabled, TEE mode
 * accesses are incorrectly being filtered based on the region configurations and
 * access attributes set for REE[0..2] modes.
 */
static void enable_tee_mode_access(void)
{
    uint32_t regn_num = 0;
    uint32_t regn_start_addr = 0x00;
    uint32_t regn_end_addr = UINT32_MAX;

    /* HP_APM */
    apm_ll_hp_apm_set_region_start_addr(regn_num, regn_start_addr);
    apm_ll_hp_apm_set_region_end_addr(regn_num, regn_end_addr);
    apm_ll_hp_apm_enable_region_filter(regn_num, true);
    /* LP_APM */
    apm_ll_lp_apm_set_region_start_addr(regn_num, regn_start_addr);
    apm_ll_lp_apm_set_region_end_addr(regn_num, regn_end_addr);
    apm_ll_lp_apm_enable_region_filter(regn_num, true);
    /* LP_APM0 */
    apm_ll_lp_apm0_set_region_start_addr(regn_num, regn_start_addr);
    apm_ll_lp_apm0_set_region_end_addr(regn_num, regn_end_addr);
    apm_ll_lp_apm0_enable_region_filter(regn_num, true);
}

/*----------------------- REE0 mode configuration -----------------------*/

/*----------------------- HP_APM configuration -----------------------*/

/* HP_APM: REE0 mode accessible regions */
static apm_hal_ctrl_region_cfg_t hp_apm_regn_cfg_ree0[] = {
    /* Region 1: CPU peripherals (RW) */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M0, 1, DR_REG_TRACE_BASE, 0x600D0000, APM_PERM_R | APM_PERM_W),

    /* NOTE: Without this entry, the REE SRAM region becomes inaccessible to
     * the MODEM master, resulting in an APM violation during Wi-Fi initialization.
     */
    /* Region 2: REE SRAM region (RW) - for all other masters except LP_CORE */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M1, 2, SOC_NS_IRAM_START, SOC_IRAM_HIGH, APM_PERM_R | APM_PERM_W),
    /* Region 2: REE SRAM region (RW) - for all LP_CORE */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M2, 2, SOC_NS_IRAM_START, SOC_IRAM_HIGH, APM_PERM_R | APM_PERM_W),

    /* Region 3: Peripherals [Start - MMU] (RW) */
    /* Protected: MMU */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 3, SOC_PERIPHERAL_LOW, SPI_MEM_MMU_ITEM_CONTENT_REG(0), APM_PERM_R | APM_PERM_W),

    /* Region 4: Peripherals [MMU - SPI1] (RW) */
    /* Protected: SPI1 */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 4, SPI_MEM_MMU_POWER_CTRL_REG(0), HP_APM_SPI1_REG_START, APM_PERM_R | APM_PERM_W),

    /* Region 5: Peripherals [SPI1 - Interrupt Matrix] (RW) */
    /* Protected: Interrupt Matrix */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 5, HP_APM_SPI1_REG_END, DR_REG_INTMTX_BASE, APM_PERM_R | APM_PERM_W),

    /* Region 6/7: Peripherals [H/W Lock - HMAC] (RW) */
    /* Protected: AES, SHA, ECC, DS, HMAC */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 6, DR_REG_ATOMIC_BASE, DR_REG_AES_BASE, APM_PERM_R | APM_PERM_W),
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 7, DR_REG_RSA_BASE, DR_REG_ECC_MULT_BASE, APM_PERM_R | APM_PERM_W),

    /* Region 8/9/10: Peripherals [IO_MUX - TEE Controller & APM] (RW) */
    /* Protected: AES, SHA, ECC, DS and HMAC PCRs, APM, TEE Controller */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 8, DR_REG_IO_MUX_BASE, PCR_AES_CONF_REG, APM_PERM_R | APM_PERM_W),
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 9, PCR_RSA_CONF_REG, PCR_ECC_CONF_REG, APM_PERM_R | APM_PERM_W),
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 10, PCR_IOMUX_CONF_REG, DR_REG_TEE_BASE, APM_PERM_R | APM_PERM_W),

    /* Region 11: Peripherals [Miscellaneous - PMU] (RW) */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M3, 11, DR_REG_MISC_BASE, DR_REG_PMU_BASE, APM_PERM_R | APM_PERM_W),
};

/* HP_APM: REE0 mode masters' configuration */
static apm_hal_ctrl_sec_mode_cfg_t hp_apm_ctrl_sec_mode_cfg_ree0 =
    APM_HAL_SEC_MODE_CFG(APM_CTRL_HP_APM, APM_SEC_MODE_REE0, hp_apm_regn_cfg_ree0);

/*----------------------- LP_APM configuration -----------------------*/

static apm_hal_ctrl_region_cfg_t lp_apm_regn_cfg_ree0[] = {
    /* Region 1: RTC memory (RWX) */
    /* NOTE: LP_MEM gets automatically remapped to an offset of 0x20000000
     * when accessed in low-speed mode */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M0, 1, SOC_RTC_IRAM_LOW + 0x20000000, SOC_RTC_IRAM_HIGH + 0x20000000, APM_PERM_ALL),

    /* Region 2: LP Peripherals [PMU - eFuse BLK x] (RW) */
    /* Protected: eFuse BLK x */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M1, 2, DR_REG_PMU_BASE, LP_APM_EFUSE_REG_START, APM_PERM_R | APM_PERM_W),

    /* Region 3: LP Peripherals [eFuse - END] (RW) */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M1, 3, LP_APM_EFUSE_REG_END, DR_REG_TRACE_BASE, APM_PERM_R | APM_PERM_W),
};

/* LP_APM: REE0 mode masters' configuration */
static apm_hal_ctrl_sec_mode_cfg_t lp_apm_ctrl_sec_mode_cfg_ree0 =
    APM_HAL_SEC_MODE_CFG(APM_CTRL_LP_APM, APM_SEC_MODE_REE0, lp_apm_regn_cfg_ree0);

/*----------------------- LP_APM0 configuration -----------------------*/

/* LP_APM0: REE0 mode accessible regions */
static apm_hal_ctrl_region_cfg_t lp_apm0_regn_cfg_ree0[] = {
    /* Region 0: RTC memory (RWX) */
    APM_HAL_REGION_ENTRY(APM_CTRL_ACCESS_PATH_M0, 1, SOC_RTC_IRAM_LOW, SOC_RTC_IRAM_HIGH, APM_PERM_ALL),
};

/* LP_APM0: REE0 mode masters' configuration */
static apm_hal_ctrl_sec_mode_cfg_t lp_apm0_ctrl_sec_mode_cfg_ree0 =
    APM_HAL_SEC_MODE_CFG(APM_CTRL_LP_APM0, APM_SEC_MODE_REE0, lp_apm0_regn_cfg_ree0);

/*---------------- TEE APM API-----------------------*/

static void enable_apm_intr(apm_ctrl_module_t ctrl_mod, uint32_t path_count)
{
    for (uint32_t i = 0; i < path_count; i++) {
        apm_hal_ctrl_info_t *ctrl_info = calloc(1, sizeof(apm_hal_ctrl_info_t));
        assert(ctrl_info != NULL);

        ctrl_info->ctrl_mod = ctrl_mod;
        ctrl_info->path = i;

        int intr_src_num = apm_hal_get_intr_src_num(ctrl_info);

        struct vector_desc_t apm_vd = {0};
        apm_vd.source = intr_src_num;
        apm_vd.isr = tee_apm_violation_isr;
        apm_vd.arg = (void *)ctrl_info;

        /* Register interrupt handler with TEE. */
        esp_tee_intr_register((void *)&apm_vd);

        /* Enable APM Ctrl interrupt for access path(M[0:n]) */
        apm_hal_clear_exception_status(ctrl_info);
        apm_hal_enable_intr(ctrl_info, true);
    }
}

void esp_tee_configure_apm_protection(void)
{
    /* Disable all control filter first to have full access of address rage. */
    apm_hal_enable_ctrl_filter_all(false);

    /* Enable TEE mode access for all APM modules */
    enable_tee_mode_access();

    /* HP_APM REE0 configuration. */
    apm_hal_set_ctrl_sec_mode_cfg(&hp_apm_ctrl_sec_mode_cfg_ree0);
    /* HP_APM interrupt configuration. */
    enable_apm_intr(APM_CTRL_HP_APM, APM_CTRL_HP_APM_PATH_NUM);
    ESP_LOGD(TAG, "[HP_APM] Configured for REE0");

    /* LP_APM REE0 configuration. */
    apm_hal_set_ctrl_sec_mode_cfg(&lp_apm_ctrl_sec_mode_cfg_ree0);
    /* LP_APM interrupt configuration. */
    enable_apm_intr(APM_CTRL_LP_APM, APM_CTRL_LP_APM_PATH_NUM);
    ESP_LOGD(TAG, "[LP_APM] Configured for REE0");

    /* LP_APM0 REE0 configuration. */
    apm_hal_set_ctrl_sec_mode_cfg(&lp_apm0_ctrl_sec_mode_cfg_ree0);
    enable_apm_intr(APM_CTRL_LP_APM0, APM_CTRL_LP_APM0_PATH_NUM);
    ESP_LOGD(TAG, "[LP_APM0] Configured for REE0");

    /* Switch HP_CPU to TEE mode and rest of the masters to REE0 mode */
    apm_hal_set_master_sec_mode(APM_MASTERS_TEE, APM_SEC_MODE_TEE);
    apm_hal_set_master_sec_mode(APM_MASTERS_REE, APM_SEC_MODE_REE0);
}
