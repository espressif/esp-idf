/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/interrupts.h"

#include "soc/apm_defs.h"
#include "hal/apm_hal.h"
#include "hal/apm_types.h"

#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_rom_sys.h"

#include "unity.h"

/***************************** Utility - APM interrupts  *****************************/

#define MAX_APM_CTRL_NUM  (4)
#define MAX_APM_PATH_NUM  (5)

static intr_handle_t s_apm_intr_hdl[MAX_APM_CTRL_NUM][MAX_APM_PATH_NUM] = { 0 };

static void test_apm_ctrl_free_all_intr(void);

/***************************** Utility - APM HAL  *****************************/

void apm_hal_enable_region_filter_all(apm_ctrl_module_t ctrl_mod, bool enable)
{
    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        REG_WRITE(HP_APM_REGION_FILTER_EN_REG, enable ? UINT32_MAX : 0);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        REG_WRITE(LP_APM0_REGION_FILTER_EN_REG, enable ? UINT32_MAX : 0);
        break;
#endif
    case APM_CTRL_LP_APM:
        REG_WRITE(LP_APM_REGION_FILTER_EN_REG, enable ? UINT32_MAX : 0);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        REG_WRITE(CPU_APM_REGION_FILTER_EN_REG, enable ? UINT32_MAX : 0);
        break;
#endif
    default:
        break;
    }
}

/********************************** APM Reset *************************************/

static void test_hp_apm_ctrl_reset(void)
{
    apm_hal_set_master_sec_mode_all(APM_SEC_MODE_TEE);
    apm_ll_hp_apm_enable_ctrl_filter_all(false);

    for (int i = 0; i < APM_CTRL_HP_APM_REGION_NUM; i++) {
        apm_ll_hp_apm_set_region_start_addr(i, 0x00U);
        apm_ll_hp_apm_set_region_end_addr(i, UINT32_MAX);
        apm_ll_hp_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE0, APM_PERM_NONE);
        apm_ll_hp_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE1, APM_PERM_NONE);
        apm_ll_hp_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE2, APM_PERM_NONE);
    }

    apm_hal_ctrl_info_t ctrl_info = {
        .ctrl_mod = APM_CTRL_HP_APM,
    };
    for (int i = 0; i < APM_CTRL_HP_APM_PATH_NUM; i++) {
        ctrl_info.path = (apm_ctrl_access_path_t)i;
        apm_hal_enable_intr(&ctrl_info, false);
        apm_hal_clear_exception_status(&ctrl_info);
    }
}

#if SOC_APM_LP_APM0_SUPPORTED
static void test_lp_apm0_ctrl_reset(void)
{
    apm_hal_set_master_sec_mode_all(APM_SEC_MODE_TEE);
    apm_ll_lp_apm0_enable_ctrl_filter_all(false);

    for (int i = 0; i < APM_CTRL_LP_APM0_REGION_NUM; i++) {
        apm_ll_lp_apm0_set_region_start_addr(i, 0x00U);
        apm_ll_lp_apm0_set_region_end_addr(i, UINT32_MAX);
        apm_ll_lp_apm0_set_sec_mode_region_attr(i, APM_SEC_MODE_REE0, APM_PERM_NONE);
        apm_ll_lp_apm0_set_sec_mode_region_attr(i, APM_SEC_MODE_REE1, APM_PERM_NONE);
        apm_ll_lp_apm0_set_sec_mode_region_attr(i, APM_SEC_MODE_REE2, APM_PERM_NONE);
    }

    apm_hal_ctrl_info_t ctrl_info = {
        .ctrl_mod = APM_CTRL_LP_APM0,
    };
    for (int i = 0; i < APM_CTRL_LP_APM0_PATH_NUM; i++) {
        ctrl_info.path = (apm_ctrl_access_path_t)i;
        apm_hal_enable_intr(&ctrl_info, false);
        apm_hal_clear_exception_status(&ctrl_info);
    }
}
#endif

static void test_lp_apm_ctrl_reset(void)
{
    apm_hal_set_master_sec_mode_all(APM_SEC_MODE_TEE);
    apm_ll_lp_apm_enable_ctrl_filter_all(false);

    for (int i = 0; i < APM_CTRL_LP_APM_REGION_NUM; i++) {
        apm_ll_lp_apm_set_region_start_addr(i, 0x00U);
        apm_ll_lp_apm_set_region_end_addr(i, UINT32_MAX);
        apm_ll_lp_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE0, APM_PERM_NONE);
        apm_ll_lp_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE1, APM_PERM_NONE);
        apm_ll_lp_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE2, APM_PERM_NONE);
    }

    apm_hal_ctrl_info_t ctrl_info = {
        .ctrl_mod = APM_CTRL_LP_APM,
    };
    for (int i = 0; i < APM_CTRL_LP_APM_PATH_NUM; i++) {
        ctrl_info.path = (apm_ctrl_access_path_t)i;
        apm_hal_enable_intr(&ctrl_info, false);
        apm_hal_clear_exception_status(&ctrl_info);
    }
}

#if SOC_APM_CPU_APM_SUPPORTED
static void test_cpu_apm_ctrl_reset(void)
{
    apm_hal_set_master_sec_mode_all(APM_SEC_MODE_TEE);
    apm_ll_cpu_apm_enable_ctrl_filter_all(false);

    for (int i = 0; i < APM_CTRL_CPU_APM_REGION_NUM; i++) {
        apm_ll_cpu_apm_set_region_start_addr(i, 0x00U);
        apm_ll_cpu_apm_set_region_end_addr(i, UINT32_MAX);
        apm_ll_cpu_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE0, APM_PERM_NONE);
        apm_ll_cpu_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE1, APM_PERM_NONE);
        apm_ll_cpu_apm_set_sec_mode_region_attr(i, APM_SEC_MODE_REE2, APM_PERM_NONE);
    }

    apm_hal_ctrl_info_t ctrl_info = {
        .ctrl_mod = APM_CTRL_CPU_APM,
    };
    for (int i = 0; i < APM_CTRL_CPU_APM_PATH_NUM; i++) {
        ctrl_info.path = (apm_ctrl_access_path_t)i;
        apm_hal_enable_intr(&ctrl_info, false);
        apm_hal_clear_exception_status(&ctrl_info);
    }
}
#endif

#if SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL
static void test_tee_peri_access_reset(void)
{
    for (uint32_t mode = APM_SEC_MODE_REE0; mode <= APM_SEC_MODE_REE2; mode++) {
        apm_hal_tee_set_peri_access_all(APM_TEE_CTRL_HP, (apm_security_mode_t)mode, APM_PERM_R | APM_PERM_W);
        apm_hal_tee_set_peri_access_all(APM_TEE_CTRL_LP, (apm_security_mode_t)mode, APM_PERM_R | APM_PERM_W);
    }
}
#endif

void test_apm_ctrl_reset_all(void)
{
    test_hp_apm_ctrl_reset();
    test_lp_apm_ctrl_reset();
#if SOC_APM_LP_APM0_SUPPORTED
    test_lp_apm0_ctrl_reset();
#endif
#if SOC_APM_CPU_APM_SUPPORTED
    test_cpu_apm_ctrl_reset();
#endif
#if SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL
    test_tee_peri_access_reset();
#endif
    test_apm_ctrl_free_all_intr();
}

/********************************** APM violation helper functions *************************************/

static const char *const excp_mid_strs[] = {
    [APM_MASTER_HPCORE]        = "HPCORE",
#if SOC_LP_CORE_SUPPORTED
    [APM_MASTER_LPCORE]        = "LPCORE",
#endif /* SOC_LP_CORE_SUPPORTED */
    [APM_MASTER_REGDMA]        = "REGDMA",
#if SOC_SDIO_SLAVE_SUPPORTED
    [APM_MASTER_SDIOSLV]       = "SDIOSLV",
#endif /* SOC_SDIO_SLAVE_SUPPORTED */
    [APM_MASTER_MODEM]         = "MODEM",
    [APM_MASTER_MEM_MON]       = "MEM_MONITOR",
    [APM_MASTER_TRACE]         = "TRACE",
#if SOC_SPIRAM_SUPPORTED
    [APM_MASTER_PSRAM_MEM_MON] = "PSRAM_MEM_MONITOR",
#endif /* SOC_SPIRAM_SUPPORTED */
#if SOC_GPSPI_SUPPORTED
    [APM_MASTER_GDMA_GPSPI]    = "GDMA_GPSPI",
#endif /* SOC_GPSPI_SUPPORTED */
#if SOC_UHCI_SUPPORTED
    [APM_MASTER_GDMA_UHCI]     = "GDMA_UHCI",
#endif /* SOC_UHCI_SUPPORTED */
    [APM_MASTER_GDMA_I2S]      = "GDMA_I2S",
#if SOC_AES_SUPPORTED
    [APM_MASTER_GDMA_AES]      = "GDMA_AES",
#endif /* SOC_AES_SUPPORTED */
    [APM_MASTER_GDMA_SHA]      = "GDMA_SHA",
    [APM_MASTER_GDMA_ADC]      = "GDMA_ADC",
#if SOC_PARLIO_SUPPORTED
    [APM_MASTER_GDMA_PARLIO]   = "GDMA_PARLIO",
#endif /* SOC_PARLIO_SUPPORTED */
    [26]                       = "GDMA_DUMMYX",
};

static const char *apm_excp_mid_to_str(uint8_t mid)
{
    if (mid < sizeof(excp_mid_strs) / sizeof(excp_mid_strs[0]) && excp_mid_strs[mid]) {
        return excp_mid_strs[mid];
    }
    return "Unknown";
}

static const char *apm_excp_type_to_str(uint8_t type)
{
    char *excp_type = "Unknown";

    if (type & BIT(0)) {
        excp_type = "Authority exception";
    } else if (type & BIT(1)) {
        excp_type = "Space exception";
    }

    return excp_type;
}

static const char *apm_excp_ctrl_to_str(apm_ctrl_module_t ctrl_mod)
{
    char *excp_ctrl = NULL;

    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        excp_ctrl = "HP_APM";
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        excp_ctrl = "LP_APM0";
        break;
#endif
    case APM_CTRL_LP_APM:
        excp_ctrl = "LP_APM";
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        excp_ctrl = "CPU_APM";
        break;
#endif
    default:
        excp_ctrl = "Unknown";
        break;
    }

    return excp_ctrl;
}

static const char *apm_excp_mode_to_str(uint8_t mode)
{
    char *excp_mode = NULL;

    switch (mode) {
    case APM_SEC_MODE_TEE:
    case APM_SEC_MODE_REE0:
        excp_mode = "REE0";
        break;
    case APM_SEC_MODE_REE1:
        excp_mode = "REE1";
        break;
    case APM_SEC_MODE_REE2:
        excp_mode = "REE2";
        break;
    default:
        excp_mode = "Unknown";
        break;
    }

    return excp_mode;
}

/********************************** APM exception check *************************************/

#define ETS_HP_APM_INTR_SOURCE(n)  (ETS_HP_APM_M0_INTR_SOURCE + n)
#if SOC_APM_LP_APM0_SUPPORTED
#define ETS_LP_APM0_INTR_SOURCE(n) (ETS_LP_APM0_INTR_SOURCE + n)
#endif
#define ETS_LP_APM_INTR_SOURCE(n)  (ETS_LP_APM_M0_INTR_SOURCE + n)
#if SOC_APM_CPU_APM_SUPPORTED
#define ETS_CPU_APM_INTR_SOURCE(n) (ETS_CPU_APM_M0_INTR_SOURCE + n)
#endif

volatile bool apm_master_excp_flag[APM_MASTER_MAX];

/* interrupt handler, clears the interrupt */
IRAM_ATTR void apm_ctrl_intr_cb(void *arg)
{
    apm_hal_ctrl_info_t *ctrl_info = (apm_hal_ctrl_info_t *)arg;
    apm_ctrl_exception_info_t excp_info = {};

    apm_hal_get_exception_info(ctrl_info, &excp_info);
    apm_hal_clear_exception_status(ctrl_info);

    apm_master_excp_flag[excp_info.id] = true;

    esp_rom_printf("APM violation: %s\n\r", apm_excp_type_to_str(excp_info.type));
    esp_rom_printf("Access addr: 0x%08x | Mode: %s\n\r", excp_info.addr, apm_excp_mode_to_str(excp_info.mode));
    esp_rom_printf("Module: %s | Path: %d\n\r", apm_excp_ctrl_to_str(ctrl_info->ctrl_mod), ctrl_info->path);
    esp_rom_printf("Master: %s | Region: %d\n\r", apm_excp_mid_to_str(excp_info.id), (excp_info.regn == 0) ? 0 : (__builtin_ffs(excp_info.regn) - 1));
}

void test_apm_ctrl_enable_intr(apm_ctrl_module_t ctrl_mod, apm_ctrl_access_path_t path)
{
    apm_hal_ctrl_info_t *ctrl_info = calloc(1, sizeof(apm_hal_ctrl_info_t));
    TEST_ASSERT_NOT_NULL(ctrl_info);

    ctrl_info->ctrl_mod = ctrl_mod;
    ctrl_info->path = path;

    int intr_src = -1;
    switch (ctrl_mod) {
    case APM_CTRL_HP_APM:
        intr_src = ETS_HP_APM_INTR_SOURCE(path);
        break;
#if SOC_APM_LP_APM0_SUPPORTED
    case APM_CTRL_LP_APM0:
        intr_src = ETS_LP_APM0_INTR_SOURCE(path);
        break;
#endif
    case APM_CTRL_LP_APM:
        intr_src = ETS_LP_APM_INTR_SOURCE(path);
        break;
#if SOC_APM_CPU_APM_SUPPORTED
    case APM_CTRL_CPU_APM:
        intr_src = ETS_CPU_APM_INTR_SOURCE(path);
        break;
#endif
    default:
        TEST_ASSERT_MESSAGE(false, "Invalid APM module");
        return;
    }

    uint32_t flags = ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL3;
    intr_handle_t *handle = &s_apm_intr_hdl[ctrl_mod][path];
    TEST_ESP_OK(esp_intr_alloc(intr_src, flags, &apm_ctrl_intr_cb, ctrl_info, handle));

    apm_hal_enable_intr(ctrl_info, true);
}

static void test_apm_ctrl_free_all_intr(void)
{
    for (int ctrl_mod = 0; ctrl_mod < MAX_APM_CTRL_NUM; ctrl_mod++) {
        for (int path = 0; path < MAX_APM_PATH_NUM; path++) {
            if (s_apm_intr_hdl[ctrl_mod][path]) {
                TEST_ESP_OK(esp_intr_free(s_apm_intr_hdl[ctrl_mod][path]));
                s_apm_intr_hdl[ctrl_mod][path] = NULL;
            }
        }
    }
}
