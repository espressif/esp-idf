/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"

#include "soc/apm_defs.h"
#include "hal/apm_hal.h"
#include "hal/apm_types.h"

#include "rom/cache.h"
#include "esp_rom_sys.h"
#include "esp_memory_utils.h"
#include "esp_heap_caps.h"

#include "test_pms_priv.h"
#include "test_pms_params.h"

#include "unity.h"

#define TEST_VAL              0x0FACADE0

#if SOC_APM_SUPPORT_CTRL_CFG_LOCK
#define APM_REGION_ENTRY(path, id, start, end, perm) \
    APM_HAL_REGION_ENTRY(path, id, start, end, perm, false)
#else
#define APM_REGION_ENTRY(path, id, start, end, perm) \
    APM_HAL_REGION_ENTRY(path, id, start, end, perm)
#endif

extern bool apm_master_excp_flag[APM_MASTER_MAX];

/***************************** APM setup for memory *****************************/

static void test_mem_apm_setup(test_sys_apm_mem_cfg_t *cfg, uint32_t test_start_addr, uint32_t test_end_addr)
{
    apm_hal_set_region_start_addr(cfg->ctrl_mod, 0, cfg->mem_start_addr);
    apm_hal_set_region_end_addr(cfg->ctrl_mod, 0, test_start_addr - 1);

    apm_hal_set_region_start_addr(cfg->ctrl_mod, 1, test_end_addr);
    apm_hal_set_region_end_addr(cfg->ctrl_mod, 1, cfg->mem_end_addr);

    for (uint32_t regn_idx = 0; regn_idx < cfg->regn_count; regn_idx++) {
        if (regn_idx > 1) {
            uint32_t regn_start = test_start_addr + (regn_idx - 2) * cfg->regn_sz;
            uint32_t regn_end   = regn_start + cfg->regn_sz - 1;
            apm_hal_set_region_start_addr(cfg->ctrl_mod, regn_idx, regn_start);
            apm_hal_set_region_end_addr(cfg->ctrl_mod, regn_idx, regn_end);
        }
        apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, APM_SEC_MODE_REE0, APM_PERM_ALL);
        apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, APM_SEC_MODE_REE1, APM_PERM_ALL);
        apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, APM_SEC_MODE_REE2, APM_PERM_ALL);
    }

    test_apm_ctrl_enable_intr(cfg->ctrl_mod, cfg->path);
    apm_hal_enable_region_filter_all(cfg->ctrl_mod, true);
    apm_hal_enable_ctrl_filter(cfg->ctrl_mod, cfg->path, true);
}

/***************************** GDMA: Utility *****************************/

static void gdma_xmem_addr_rw(uint8_t *src, uint8_t *dest, size_t size, uint32_t test_attr)
{
    apm_master_excp_flag[TEST_GDMA_APM_MASTER_ID] = false;
    memset(src, TEST_VAL, size);

    test_gdma_init();
#if CONFIG_SPIRAM
    if (esp_ptr_external_ram(src)) {
        Cache_WriteBack_Addr((uint32_t)src, size);
    }
#endif
    test_gdma_m2m_transfer(src, dest, size);
#if CONFIG_SPIRAM
    if (esp_ptr_external_ram(dest)) {
        Cache_Invalidate_Addr((uint32_t)dest, size);
    }
#endif

    if (!test_attr) {
        test_delay_ms(10);
        TEST_ASSERT(apm_master_excp_flag[TEST_GDMA_APM_MASTER_ID]);
        test_gdma_wait_done();
    } else {
        test_gdma_wait_done();
        test_delay_ms(10);
        TEST_ASSERT_FALSE(apm_master_excp_flag[TEST_GDMA_APM_MASTER_ID]);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(src, dest, size);
    }

    test_gdma_deinit();
    apm_master_excp_flag[TEST_GDMA_APM_MASTER_ID] = false;
}

static void test_gdma_to_xmem_access(const test_sys_apm_mem_cfg_t *cfg, uint32_t test_start_addr)
{
    uint8_t *test_dma_buf = heap_caps_aligned_alloc(cfg->regn_sz, cfg->regn_sz, MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(test_dma_buf);
    memset(test_dma_buf, 0x5A, cfg->regn_sz);

    for (uint32_t mode = APM_SEC_MODE_TEE; mode <= APM_SEC_MODE_REE2; mode++) {
        apm_ll_hp_tee_set_master_sec_mode(TEST_GDMA_APM_MASTER_ID, (apm_security_mode_t)mode);

        for (uint32_t regn_idx = 2; regn_idx < cfg->regn_count; regn_idx++) {
            uint8_t *regn_start = (uint8_t *)(test_start_addr + (regn_idx - 2) * cfg->regn_sz);
            uint32_t pms_attr = APM_PERM_NONE;

            if (mode == APM_SEC_MODE_TEE) {
                pms_attr = APM_PERM_R | APM_PERM_W;
                gdma_xmem_addr_rw(regn_start, test_dma_buf, cfg->regn_sz, pms_attr & APM_PERM_R);
                gdma_xmem_addr_rw(test_dma_buf, regn_start, cfg->regn_sz, pms_attr & APM_PERM_W);
                continue;
            }

            pms_attr = APM_PERM_W;
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_NONE);
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, pms_attr);
            gdma_xmem_addr_rw(regn_start, test_dma_buf, cfg->regn_sz, pms_attr & APM_PERM_R);
            gdma_xmem_addr_rw(test_dma_buf, regn_start, cfg->regn_sz, pms_attr & APM_PERM_W);

            pms_attr = APM_PERM_R;
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_NONE);
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, pms_attr);
            gdma_xmem_addr_rw(regn_start, test_dma_buf, cfg->regn_sz, pms_attr & APM_PERM_R);
            gdma_xmem_addr_rw(test_dma_buf, regn_start, cfg->regn_sz, pms_attr & APM_PERM_W);

            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_ALL);
        }
    }

    free(test_dma_buf);
}

/***************************** GDMA: HP_MEM *****************************/

void test_sys_apm_master_gdma_slave_hpmem(void)
{
    test_apm_ctrl_reset_all();

    const uint32_t total_sz = (GDMA_HPMEM_TEST_REGN_NUM - 2) * 0x1000;
    test_sys_apm_mem_cfg_t cfg = {
        .ctrl_mod = GDMA_HPMEM_APM_CTRL,
        .path = GDMA_HPMEM_APM_PATH,
        .mem_start_addr = SOC_IRAM_LOW,
        .mem_end_addr = SOC_DRAM_HIGH - 1,
        .regn_count = GDMA_HPMEM_TEST_REGN_NUM,
        .regn_sz = total_sz / (GDMA_HPMEM_TEST_REGN_NUM - 2),
    };

    uint8_t *test_hpmem_buf = heap_caps_aligned_alloc(cfg.regn_sz, total_sz, MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(test_hpmem_buf);
    memset(test_hpmem_buf, 0xA5, total_sz);

    uint32_t test_start_addr = (uint32_t)test_hpmem_buf;
    uint32_t test_end_addr   = test_start_addr + total_sz;
    test_mem_apm_setup(&cfg, test_start_addr, test_end_addr);

    test_gdma_to_xmem_access(&cfg, test_start_addr);
    free(test_hpmem_buf);
}

/***************************** GDMA: EXT_MEM *****************************/

#if CONFIG_SPIRAM
void test_sys_apm_master_gdma_slave_extmem(void)
{
    test_apm_ctrl_reset_all();

    const uint32_t total_sz = (GDMA_EXTMEM_TEST_REGN_NUM - 2) * 0x1000;
    test_sys_apm_mem_cfg_t cfg = {
        .ctrl_mod = GDMA_EXTMEM_APM_CTRL,
        .path = GDMA_EXTMEM_APM_PATH,
        .mem_start_addr = SOC_EXTRAM_DATA_LOW,
        .mem_end_addr = SOC_EXTRAM_DATA_HIGH - 1,
        .regn_count = GDMA_EXTMEM_TEST_REGN_NUM,
        .regn_sz = total_sz / (GDMA_EXTMEM_TEST_REGN_NUM - 2),
    };

    uint8_t *test_extmem_buf = heap_caps_aligned_alloc(cfg.regn_sz, total_sz, MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(test_extmem_buf);
    memset(test_extmem_buf, 0xA5, total_sz);

    uint32_t test_start_addr = (uint32_t)test_extmem_buf;
    uint32_t test_end_addr   = test_start_addr + total_sz;
    test_mem_apm_setup(&cfg, test_start_addr, test_end_addr);

    test_gdma_to_xmem_access(&cfg, test_start_addr);
    free(test_extmem_buf);
}
#endif

/***************************** HP_CPU: Peripherals (Utility) *****************************/

static void hp_cpu_peri_addr_rw(uint32_t peri_addr, uint32_t attr)
{
    apm_master_excp_flag[APM_MASTER_HPCORE] = false;

    REG_WRITE(peri_addr, TEST_VAL);
    test_delay_ms(10);

    bool can_write = (attr & APM_PERM_W);
    TEST_ASSERT(apm_master_excp_flag[APM_MASTER_HPCORE] != can_write);

    apm_master_excp_flag[APM_MASTER_HPCORE] = false;

    volatile uint32_t val = REG_READ(peri_addr);
    (void)val;
    test_delay_ms(10);

    bool can_read = (attr & APM_PERM_R);
    TEST_ASSERT(apm_master_excp_flag[APM_MASTER_HPCORE] != can_read);

    apm_master_excp_flag[APM_MASTER_HPCORE] = false;
}

#if CONFIG_ULP_COPROC_ENABLED
static void lp_cpu_peri_addr_rw(uint32_t peri_addr, uint32_t attr)
{
    apm_master_excp_flag[APM_MASTER_LPCORE] = false;
    test_reset_lp_cpu();

    SEND_MSG(MSG_SLAVE_WRITE);
    SEND_ADDR(peri_addr);
    test_delay_ms(10);

    bool can_write = (attr & APM_PERM_W);
    TEST_ASSERT(apm_master_excp_flag[APM_MASTER_LPCORE] != can_write);

    apm_master_excp_flag[APM_MASTER_LPCORE] = false;

    SEND_MSG(MSG_SLAVE_READ);
    SEND_ADDR(peri_addr);
    test_delay_ms(10);

    bool can_read = (attr & APM_PERM_R);
    TEST_ASSERT(apm_master_excp_flag[APM_MASTER_LPCORE] != can_read);

    apm_master_excp_flag[APM_MASTER_LPCORE] = false;
}
#endif

static void test_cpu_to_peri_access(const test_sys_apm_periph_cfg_t *cfg)
{
    typedef void (*peri_access_func_t)(uint32_t peri_addr, uint32_t attr);
    peri_access_func_t cpu_peri_rw = NULL;

    switch (cfg->master_id) {
    case APM_MASTER_HPCORE:
        cpu_peri_rw = hp_cpu_peri_addr_rw;
        break;
#if CONFIG_ULP_COPROC_ENABLED
    case APM_MASTER_LPCORE:
        cpu_peri_rw = lp_cpu_peri_addr_rw;
        break;
#endif
    default:
        TEST_ASSERT_MESSAGE(false, "Unsupported master");
        return;
    }

    for (uint32_t mode = APM_SEC_MODE_TEE; mode <= APM_SEC_MODE_REE2; mode++) {
        apm_hal_set_master_sec_mode(BIT(cfg->master_id), (apm_security_mode_t)mode);

        for (uint32_t regn_idx = 0; regn_idx < cfg->test_addr_num; regn_idx++) {
            if (BIT(regn_idx) & cfg->test_addr_resv_mask) {
                continue;
            }

            uint32_t pms_attr = APM_PERM_NONE;
            if (mode == APM_SEC_MODE_TEE) {
                pms_attr = APM_PERM_R | APM_PERM_W;
                cpu_peri_rw(cfg->test_addr[regn_idx], pms_attr);
                continue;
            }

            pms_attr = APM_PERM_W;
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_NONE);
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, pms_attr);
            if (cfg->master_id == APM_MASTER_HPCORE) {
                test_m2u_switch();
            }
            cpu_peri_rw(cfg->test_addr[regn_idx], pms_attr);
            if (cfg->master_id == APM_MASTER_HPCORE) {
                test_u2m_switch();
            }

            pms_attr = APM_PERM_R;
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_NONE);
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, pms_attr);
            if (cfg->master_id == APM_MASTER_HPCORE) {
                test_m2u_switch();
            }
            cpu_peri_rw(cfg->test_addr[regn_idx], pms_attr);
            if (cfg->master_id == APM_MASTER_HPCORE) {
                test_u2m_switch();
            }

            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_R | APM_PERM_W);
        }
    }
}

/***************************** HP_CPU: Peripherals (HP_PERI) *****************************/

static apm_hal_ctrl_region_cfg_t test_hp_peri_regn_cfg[] = {
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 0, TEST_HP_PERI_REGION0_START, TEST_HP_PERI_REGION0_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 1, TEST_HP_PERI_REGION1_START, TEST_HP_PERI_REGION1_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 2, TEST_HP_PERI_REGION2_START, TEST_HP_PERI_REGION2_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 3, TEST_HP_PERI_REGION3_START, TEST_HP_PERI_REGION3_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 4, TEST_HP_PERI_REGION4_START, TEST_HP_PERI_REGION4_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 5, TEST_HP_PERI_REGION5_START, TEST_HP_PERI_REGION5_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 6, TEST_HP_PERI_REGION6_START, TEST_HP_PERI_REGION6_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 7, TEST_HP_PERI_REGION7_START, TEST_HP_PERI_REGION7_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 8, TEST_HP_PERI_REGION8_START, TEST_HP_PERI_REGION8_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 9, TEST_HP_PERI_REGION9_START, TEST_HP_PERI_REGION9_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 10, TEST_HP_PERI_REGION10_START, TEST_HP_PERI_REGION10_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 11, TEST_HP_PERI_REGION11_START, TEST_HP_PERI_REGION11_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 12, TEST_HP_PERI_REGION12_START, TEST_HP_PERI_REGION12_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 13, TEST_HP_PERI_REGION13_START, TEST_HP_PERI_REGION13_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 14, TEST_HP_PERI_REGION14_START, TEST_HP_PERI_REGION14_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_HPPERI_APM_PATH, 15, TEST_HP_PERI_REGION15_START, TEST_HP_PERI_REGION15_END, APM_PERM_R | APM_PERM_W),
};

static uint32_t test_hp_peri_regn_addr[] = {
    TEST_HP_PERI_ADDR0, TEST_HP_PERI_ADDR1, TEST_HP_PERI_ADDR2, TEST_HP_PERI_ADDR3,
    TEST_HP_PERI_ADDR4, TEST_HP_PERI_ADDR5, TEST_HP_PERI_ADDR6, TEST_HP_PERI_ADDR7,
    TEST_HP_PERI_ADDR8, TEST_HP_PERI_ADDR9, TEST_HP_PERI_ADDR10, TEST_HP_PERI_ADDR11,
    TEST_HP_PERI_ADDR12, TEST_HP_PERI_ADDR13, TEST_HP_PERI_ADDR14, TEST_HP_PERI_ADDR15
};

void test_sys_apm_master_hp_cpu_slave_hp_peri(void)
{
    test_apm_ctrl_reset_all();

    apm_ctrl_module_t ctrl_mod = HP_CPU_HPPERI_APM_CTRL;
    apm_ctrl_access_path_t path = HP_CPU_HPPERI_APM_PATH;

    uint32_t test_regn_num = sizeof(test_hp_peri_regn_cfg) / sizeof(test_hp_peri_regn_cfg[0]);
    for (uint32_t regn_idx = 0; regn_idx < test_regn_num; regn_idx++) {
        for (uint32_t mode = APM_SEC_MODE_REE0; mode <= APM_SEC_MODE_REE2; mode++) {
            apm_hal_set_region_filter_cfg(ctrl_mod, (apm_security_mode_t)mode, &test_hp_peri_regn_cfg[regn_idx]);
        }
    }

    test_apm_ctrl_enable_intr(ctrl_mod, path);
    apm_hal_enable_region_filter_all(ctrl_mod, true);
    apm_hal_enable_ctrl_filter(ctrl_mod, path, true);

    test_sys_apm_periph_cfg_t cfg = {
        .master_id = APM_MASTER_HPCORE,
        .ctrl_mod = ctrl_mod,
        .test_addr = test_hp_peri_regn_addr,
        .test_addr_num = test_regn_num,
        .test_addr_resv_mask = TEST_HP_PERI_RESV_MASK,
    };

    set_test_vector_table();
    test_cpu_to_peri_access(&cfg);
    restore_default_vector_table();
}

/***************************** HP_CPU: Peripherals (CPU_PERI) *****************************/

static apm_hal_ctrl_region_cfg_t test_cpu_peri_regn_cfg[] = {
    APM_REGION_ENTRY(HP_CPU_CPUPERI_APM_PATH, 0, TEST_CPU_PERI_REGION0_START, TEST_CPU_PERI_REGION0_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_CPUPERI_APM_PATH, 1, TEST_CPU_PERI_REGION1_START, TEST_CPU_PERI_REGION1_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_CPUPERI_APM_PATH, 2, TEST_CPU_PERI_REGION2_START, TEST_CPU_PERI_REGION2_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_CPUPERI_APM_PATH, 3, TEST_CPU_PERI_REGION3_START, TEST_CPU_PERI_REGION3_END, APM_PERM_R | APM_PERM_W),
};

static uint32_t test_cpu_peri_regn_addr[] = {
    TEST_CPU_PERI_ADDR0, TEST_CPU_PERI_ADDR1, TEST_CPU_PERI_ADDR2, TEST_CPU_PERI_ADDR3,
};

void test_sys_apm_master_hp_cpu_slave_cpu_peri(void)
{
    test_apm_ctrl_reset_all();

    apm_ctrl_module_t ctrl_mod = HP_CPU_CPUPERI_APM_CTRL;
    apm_ctrl_access_path_t path = HP_CPU_CPUPERI_APM_PATH;

    uint32_t test_regn_num = sizeof(test_cpu_peri_regn_cfg) / sizeof(test_cpu_peri_regn_cfg[0]);
    for (uint32_t regn_idx = 0; regn_idx < test_regn_num; regn_idx++) {
        for (uint32_t mode = APM_SEC_MODE_REE0; mode <= APM_SEC_MODE_REE2; mode++) {
            apm_hal_set_region_filter_cfg(ctrl_mod, (apm_security_mode_t)mode, &test_cpu_peri_regn_cfg[regn_idx]);
        }
    }

    test_apm_ctrl_enable_intr(ctrl_mod, path);
    apm_hal_enable_region_filter_all(ctrl_mod, true);
    apm_hal_enable_ctrl_filter(ctrl_mod, path, true);

    test_sys_apm_periph_cfg_t cfg = {
        .master_id = APM_MASTER_HPCORE,
        .ctrl_mod = ctrl_mod,
        .test_addr = test_cpu_peri_regn_addr,
        .test_addr_num = test_regn_num,
    };

    set_test_vector_table();
    test_cpu_to_peri_access(&cfg);
    restore_default_vector_table();
}

/***************************** HP_CPU: Peripherals (LP_PERI) *****************************/

static apm_hal_ctrl_region_cfg_t test_lp_peri_regn_cfg[] = {
    APM_REGION_ENTRY(HP_CPU_LPPERI_APM_PATH, 0, TEST_LP_PERI_REGION0_START, TEST_LP_PERI_REGION0_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_LPPERI_APM_PATH, 1, TEST_LP_PERI_REGION1_START, TEST_LP_PERI_REGION1_END, APM_PERM_R | APM_PERM_W),
#if APM_CTRL_LP_APM_REGION_NUM > 2
    APM_REGION_ENTRY(HP_CPU_LPPERI_APM_PATH, 2, TEST_LP_PERI_REGION2_START, TEST_LP_PERI_REGION2_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_LPPERI_APM_PATH, 3, TEST_LP_PERI_REGION3_START, TEST_LP_PERI_REGION3_END, APM_PERM_R | APM_PERM_W),
#endif
#if APM_CTRL_LP_APM_REGION_NUM > 4
    APM_REGION_ENTRY(HP_CPU_LPPERI_APM_PATH, 4, TEST_LP_PERI_REGION4_START, TEST_LP_PERI_REGION4_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_LPPERI_APM_PATH, 5, TEST_LP_PERI_REGION5_START, TEST_LP_PERI_REGION5_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_LPPERI_APM_PATH, 6, TEST_LP_PERI_REGION6_START, TEST_LP_PERI_REGION6_END, APM_PERM_R | APM_PERM_W),
    APM_REGION_ENTRY(HP_CPU_LPPERI_APM_PATH, 7, TEST_LP_PERI_REGION7_START, TEST_LP_PERI_REGION7_END, APM_PERM_R | APM_PERM_W),
#endif
};

static uint32_t test_lp_peri_regn_addr[] = {
    TEST_LP_PERI_ADDR0, TEST_LP_PERI_ADDR1,
#if APM_CTRL_LP_APM_REGION_NUM > 2
    TEST_LP_PERI_ADDR2, TEST_LP_PERI_ADDR3,
#endif
#if APM_CTRL_LP_APM_REGION_NUM > 4
    TEST_LP_PERI_ADDR4, TEST_LP_PERI_ADDR5, TEST_LP_PERI_ADDR6, TEST_LP_PERI_ADDR7,
#endif
};

void test_sys_apm_master_hp_cpu_slave_lp_peri(void)
{
    test_apm_ctrl_reset_all();
    apm_ctrl_module_t ctrl_mod = HP_CPU_LPPERI_APM_CTRL;
    apm_ctrl_access_path_t path = HP_CPU_LPPERI_APM_PATH;

    uint32_t test_regn_num = sizeof(test_lp_peri_regn_cfg) / sizeof(test_lp_peri_regn_cfg[0]);
    for (uint32_t regn_idx = 0; regn_idx < test_regn_num; regn_idx++) {
        for (uint32_t mode = APM_SEC_MODE_REE0; mode <= APM_SEC_MODE_REE2; mode++) {
            apm_hal_set_region_filter_cfg(ctrl_mod, (apm_security_mode_t)mode, &test_lp_peri_regn_cfg[regn_idx]);
        }
    }

    test_apm_ctrl_enable_intr(ctrl_mod, path);
    apm_hal_enable_region_filter_all(ctrl_mod, true);
    apm_hal_enable_ctrl_filter(ctrl_mod, path, true);

    test_sys_apm_periph_cfg_t cfg = {
        .master_id = APM_MASTER_HPCORE,
        .ctrl_mod = ctrl_mod,
        .test_addr = test_lp_peri_regn_addr,
        .test_addr_num = test_regn_num,
        .test_addr_resv_mask = TEST_LP_PERI_RESV_MASK,
    };

    set_test_vector_table();
    test_cpu_to_peri_access(&cfg);
    restore_default_vector_table();
}

/***************************** HP_CPU: LP_MEM *****************************/

static void write_dummy_code_to_mem(uint32_t start_addr, size_t size)
{
    const uint16_t nop_ins = 0x0001;
    const uint16_t retw_ins = 0x8082;
    uint16_t *addr = (uint16_t *)start_addr;

    size_t num_nops = (size / sizeof(uint16_t)) - 1;
    for (size_t i = 0; i < num_nops; i++) {
        addr[i] = nop_ins;
    }

    if (size >= sizeof(uint16_t)) {
        addr[num_nops] = retw_ins;
    }
    __asm__ volatile("fence");
}

static void hp_cpu_xmem_addr_rw(uint32_t mem_addr, size_t size, uint32_t attr)
{
    apm_master_excp_flag[APM_MASTER_HPCORE] = false;
#if SOC_RTC_MEM_SUPPORTED
    ADJUST_LP_MEM_ADDR(mem_addr);
#endif

    volatile uint32_t *addr = (volatile uint32_t *)mem_addr;
    uint32_t num_words = size / sizeof(uint32_t);
    uint32_t step = (num_words / 16) ? (num_words / 16) : 1;

    for (uint32_t offs = 0; offs < num_words; offs += step) {
        addr[offs] = TEST_VAL;
        asm volatile("nop");
        test_delay_ms(10);

        bool can_write = (attr & APM_PERM_W);
        TEST_ASSERT(apm_master_excp_flag[APM_MASTER_HPCORE] != can_write);

        apm_master_excp_flag[APM_MASTER_HPCORE] = false;

        uint32_t val = addr[offs];
        (void)val;
        asm volatile("fence");
        test_delay_ms(10);

        bool can_read = (attr & APM_PERM_R);
        TEST_ASSERT(apm_master_excp_flag[APM_MASTER_HPCORE] != can_read);

        apm_master_excp_flag[APM_MASTER_HPCORE] = false;
    }
}

static void hp_cpu_xmem_addr_x(uint32_t mem_addr, size_t size, uint32_t attr)
{
    apm_master_excp_flag[APM_MASTER_HPCORE] = false;
#if SOC_RTC_MEM_SUPPORTED
    ADJUST_LP_MEM_ADDR(mem_addr);
#endif
    write_dummy_code_to_mem(mem_addr, size);

    void (*func_ptr)(void);
    func_ptr = (void(*)(void))(mem_addr);
    func_ptr();
    test_delay_ms(10);

    bool can_execute = attr & APM_PERM_X;
    TEST_ASSERT(apm_master_excp_flag[APM_MASTER_HPCORE] != can_execute);

    apm_master_excp_flag[APM_MASTER_HPCORE] = false;
}

#if CONFIG_ULP_COPROC_ENABLED
static void lp_cpu_xmem_addr_rw(uint32_t mem_addr, size_t size, uint32_t attr)
{
    apm_master_excp_flag[APM_MASTER_LPCORE] = false;
    ADJUST_LP_MEM_ADDR(mem_addr);
    test_reset_lp_cpu();

    SEND_MSG(MSG_SLAVE_WRITE);
    SEND_ADDR(mem_addr);
    SEND_SIZE(sizeof(uint32_t));
    test_delay_ms(10);

    bool can_write = (attr & APM_PERM_W);
    TEST_ASSERT(apm_master_excp_flag[APM_MASTER_LPCORE] != can_write);

    apm_master_excp_flag[APM_MASTER_LPCORE] = false;

    SEND_MSG(MSG_SLAVE_READ);
    SEND_ADDR(mem_addr);
    test_delay_ms(10);

    bool can_read = (attr & APM_PERM_R);
    TEST_ASSERT(apm_master_excp_flag[APM_MASTER_LPCORE] != can_read);

    apm_master_excp_flag[APM_MASTER_LPCORE] = false;
}

static void lp_cpu_xmem_addr_x(uint32_t mem_addr, size_t size, uint32_t attr)
{
    apm_master_excp_flag[APM_MASTER_LPCORE] = false;
    ADJUST_LP_MEM_ADDR(mem_addr);
    write_dummy_code_to_mem(mem_addr, size);
    test_reset_lp_cpu();

    SEND_MSG(MSG_SLAVE_EXEC);
    SEND_ADDR(mem_addr);
    test_delay_ms(10);

    bool can_execute = (attr & APM_PERM_X);
    TEST_ASSERT(apm_master_excp_flag[APM_MASTER_LPCORE] != can_execute);

    apm_master_excp_flag[APM_MASTER_LPCORE] = false;
}

static void test_lp_cpu_to_hpmem_force_access(const test_sys_apm_mem_cfg_t *cfg, uint32_t test_start_addr)
{
    apm_ll_lp_tee_enable_force_hp_mem_access(true);

    for (uint32_t mode = APM_SEC_MODE_REE0; mode <= APM_SEC_MODE_REE2; mode++) {
        apm_hal_set_master_sec_mode(BIT(cfg->master_id), (apm_security_mode_t)mode);

        for (uint32_t regn_idx = 2; regn_idx < cfg->regn_count; regn_idx++) {
            uint32_t regn_start = test_start_addr + (regn_idx - 2) * cfg->regn_sz;

            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_NONE);
            lp_cpu_xmem_addr_rw(regn_start, cfg->regn_sz, APM_PERM_R | APM_PERM_W);
            lp_cpu_xmem_addr_x(regn_start, cfg->regn_sz, APM_PERM_X);
        }
    }

    apm_ll_lp_tee_enable_force_hp_mem_access(false);
}
#endif

static void test_cpu_to_xmem_access(test_sys_apm_mem_cfg_t *cfg, uint32_t test_start_addr)
{
    typedef void (*mem_access_func_t)(uint32_t mem_addr, size_t size, uint32_t attr);
    mem_access_func_t cpu_mem_rw = NULL, cpu_mem_x = NULL;

    switch (cfg->master_id) {
    case APM_MASTER_HPCORE:
        cpu_mem_rw = hp_cpu_xmem_addr_rw;
        cpu_mem_x  = hp_cpu_xmem_addr_x;
        break;
#if CONFIG_ULP_COPROC_ENABLED
    case APM_MASTER_LPCORE:
        cpu_mem_rw = lp_cpu_xmem_addr_rw;
        cpu_mem_x  = lp_cpu_xmem_addr_x;
        break;
#endif
    default:
        TEST_ASSERT_MESSAGE(false, "Unsupported master");
        return;
    }

    for (uint32_t mode = APM_SEC_MODE_TEE; mode <= APM_SEC_MODE_REE2; mode++) {
        apm_hal_set_master_sec_mode(BIT(cfg->master_id), (apm_security_mode_t)mode);

        for (uint32_t regn_idx = 2; regn_idx < cfg->regn_count; regn_idx++) {
            uint32_t regn_start = test_start_addr + (regn_idx - 2) * cfg->regn_sz;

            uint32_t pms_attr = APM_PERM_NONE;
            if (mode == APM_SEC_MODE_TEE) {
                pms_attr = APM_PERM_ALL;
                cpu_mem_rw(regn_start, cfg->regn_sz, pms_attr);
                cpu_mem_x(regn_start, cfg->regn_sz, pms_attr);
                continue;
            }

            pms_attr = APM_PERM_W;
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_NONE);
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, pms_attr);
            if (cfg->master_id == APM_MASTER_HPCORE) {
                test_m2u_switch();
            }
            cpu_mem_rw(regn_start, cfg->regn_sz, pms_attr);
            cpu_mem_x(regn_start, cfg->regn_sz, pms_attr);
            if (cfg->master_id == APM_MASTER_HPCORE) {
                test_u2m_switch();
            }

            pms_attr = APM_PERM_R;
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_NONE);
            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, pms_attr);
            if (cfg->master_id == APM_MASTER_HPCORE) {
                test_m2u_switch();
            }
            cpu_mem_rw(regn_start, cfg->regn_sz, pms_attr);
            if (cfg->master_id == APM_MASTER_HPCORE) {
                test_u2m_switch();
            }

            apm_hal_set_sec_mode_region_attr(cfg->ctrl_mod, regn_idx, mode, APM_PERM_ALL);
        }
    }
}

#if SOC_RTC_MEM_SUPPORTED
void test_sys_apm_master_hp_cpu_slave_lpmem(void)
{
    test_apm_ctrl_reset_all();

    const uint32_t total_sz = (HP_CPU_LPMEM_TEST_REGN_NUM - 2) * 0x100;
    test_sys_apm_mem_cfg_t cfg = {
        .master_id = APM_MASTER_HPCORE,
        .ctrl_mod = HP_CPU_LPMEM_APM_CTRL,
        .path = HP_CPU_LPMEM_APM_PATH,
        .mem_start_addr = SOC_RTC_IRAM_LOW,
        .mem_end_addr   = SOC_RTC_DRAM_HIGH - 1,
        .regn_count = HP_CPU_LPMEM_TEST_REGN_NUM,
        .regn_sz = total_sz / (HP_CPU_LPMEM_TEST_REGN_NUM - 2),
    };

    uint32_t test_start_addr = cfg.mem_start_addr + (cfg.mem_end_addr - cfg.mem_start_addr) / 4 + 1;
    uint32_t test_end_addr = test_start_addr + total_sz;
    test_mem_apm_setup(&cfg, test_start_addr, test_end_addr);

    set_test_vector_table();
    test_cpu_to_xmem_access(&cfg, test_start_addr);
    restore_default_vector_table();
}
#endif

#if SOC_APM_CPU_APM_SUPPORTED
void test_sys_apm_master_hp_cpu_slave_hpmem(void)
{
    test_apm_ctrl_reset_all();

    const uint32_t total_sz = (HP_CPU_HPMEM_TEST_REGN_NUM - 2) * 0x1000;
    test_sys_apm_mem_cfg_t cfg = {
        .master_id = APM_MASTER_HPCORE,
        .ctrl_mod = HP_CPU_HPMEM_APM_CTRL,
        .path = HP_CPU_HPMEM_I_APM_PATH,
        .mem_start_addr = SOC_IRAM_LOW,
        .mem_end_addr   = SOC_DRAM_HIGH - 1,
        .regn_count = HP_CPU_HPMEM_TEST_REGN_NUM,
        .regn_sz = total_sz / (HP_CPU_HPMEM_TEST_REGN_NUM - 2),
    };

    uint8_t *test_hpmem_buf = heap_caps_aligned_alloc(cfg.regn_sz, total_sz, MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(test_hpmem_buf);
    memset(test_hpmem_buf, 0xA5, total_sz);

    uint32_t test_start_addr = (uint32_t)test_hpmem_buf;
    uint32_t test_end_addr   = test_start_addr + total_sz;
    test_mem_apm_setup(&cfg, test_start_addr, test_end_addr);

    test_apm_ctrl_enable_intr(cfg.ctrl_mod, HP_CPU_HPMEM_D_APM_PATH);
    apm_hal_enable_ctrl_filter(cfg.ctrl_mod, HP_CPU_HPMEM_D_APM_PATH, true);

    set_test_vector_table();
    test_cpu_to_xmem_access(&cfg, test_start_addr);
    restore_default_vector_table();

    free(test_hpmem_buf);
}
#endif

/***************************** LP_CPU: Peripherals (LP_PERI) *****************************/

#if CONFIG_ULP_COPROC_ENABLED
void test_sys_apm_master_lp_cpu_slave_lp_peri(void)
{
    test_apm_ctrl_reset_all();

    apm_ctrl_module_t ctrl_mod = LP_CPU_LPPERI_APM_CTRL;
    apm_ctrl_access_path_t path = LP_CPU_LPPERI_APM_PATH;

    uint32_t test_regn_num = sizeof(test_lp_peri_regn_cfg) / sizeof(test_lp_peri_regn_cfg[0]);
    for (uint32_t regn_idx = 0; regn_idx < test_regn_num; regn_idx++) {
        for (uint32_t mode = APM_SEC_MODE_REE0; mode <= APM_SEC_MODE_REE2; mode++) {
            apm_hal_set_region_filter_cfg(ctrl_mod, (apm_security_mode_t)mode, &test_lp_peri_regn_cfg[regn_idx]);
        }
    }

    test_apm_ctrl_enable_intr(ctrl_mod, path);
    apm_hal_enable_region_filter_all(ctrl_mod, true);
    apm_hal_enable_ctrl_filter(ctrl_mod, path, true);

    test_sys_apm_periph_cfg_t cfg = {
        .master_id = APM_MASTER_LPCORE,
        .ctrl_mod = ctrl_mod,
        .test_addr = test_lp_peri_regn_addr,
        .test_addr_num = test_regn_num,
        .test_addr_resv_mask = TEST_LP_PERI_RESV_MASK,
    };

    test_boot_lp_cpu();

    set_test_vector_table();
    test_cpu_to_peri_access(&cfg);
    restore_default_vector_table();

    test_stop_lp_cpu();
}

void test_sys_apm_master_lp_cpu_slave_lpmem(void)
{
    test_apm_ctrl_reset_all();

    const uint32_t total_sz = (HP_CPU_LPMEM_TEST_REGN_NUM - 2) * 0x100;
    test_sys_apm_mem_cfg_t cfg = {
        .master_id = APM_MASTER_LPCORE,
        .ctrl_mod = LP_CPU_LPMEM_APM_CTRL,
        .path = LP_CPU_LPMEM_APM_PATH,
        .mem_start_addr = SOC_RTC_IRAM_LOW + 0x20000000,
        .mem_end_addr = SOC_RTC_DRAM_HIGH + 0x20000000 - 1,
        .regn_count = LP_CPU_LPMEM_TEST_REGN_NUM,
        .regn_sz = total_sz / (LP_CPU_LPMEM_TEST_REGN_NUM - 2),
    };

    uint32_t test_start_addr = cfg.mem_start_addr + (cfg.mem_end_addr - cfg.mem_start_addr) / 4 + 1;
    uint32_t test_end_addr   = test_start_addr + total_sz;
    test_mem_apm_setup(&cfg, test_start_addr, test_end_addr);

    test_switch_lp_mem_speed(false);
    test_boot_lp_cpu();

    set_test_vector_table();
    test_cpu_to_xmem_access(&cfg, test_start_addr);
    restore_default_vector_table();

    test_stop_lp_cpu();
    test_switch_lp_mem_speed(true);
}

void test_sys_apm_master_lp_cpu_slave_hpmem(void)
{
    test_apm_ctrl_reset_all();

    const uint32_t total_sz = (LP_CPU_HPMEM_TEST_REGN_NUM - 2) * 0x1000;
    test_sys_apm_mem_cfg_t cfg = {
        .master_id = APM_MASTER_LPCORE,
        .ctrl_mod = LP_CPU_HPMEM_APM_CTRL,
        .path = LP_CPU_HPMEM_APM_PATH,
        .mem_start_addr = SOC_IRAM_LOW,
        .mem_end_addr = SOC_DRAM_HIGH - 1,
        .regn_count = LP_CPU_HPMEM_TEST_REGN_NUM,
        .regn_sz = total_sz / (LP_CPU_HPMEM_TEST_REGN_NUM - 2),
    };

    uint8_t *test_hpmem_buf = heap_caps_aligned_alloc(cfg.regn_sz, total_sz, MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(test_hpmem_buf);
    memset(test_hpmem_buf, 0xA5, total_sz);

    uint32_t test_start_addr = (uint32_t)test_hpmem_buf;
    uint32_t test_end_addr   = test_start_addr + total_sz;
    test_mem_apm_setup(&cfg, test_start_addr, test_end_addr);

    set_test_vector_table();

    test_boot_lp_cpu();
    test_lp_cpu_to_hpmem_force_access(&cfg, test_start_addr);
    test_cpu_to_xmem_access(&cfg, test_start_addr);
    test_stop_lp_cpu();

    restore_default_vector_table();
    free(test_hpmem_buf);
}
#endif

/***************************** TEE mode default access *****************************/

static void setup_apm_regn_cfg(apm_ctrl_module_t ctrl_mod, uint32_t regn_num, uint32_t start_addr, uint32_t end_addr)
{
    apm_hal_set_region_start_addr(ctrl_mod, regn_num, start_addr);
    apm_hal_set_region_end_addr(ctrl_mod, regn_num, end_addr);
    apm_hal_enable_region_filter(ctrl_mod, regn_num, true);
}

static void test_tee_mode_apm_setup(test_sys_apm_mem_cfg_t *cfg, uint32_t test_start_addr, uint32_t test_end_addr)
{
    setup_apm_regn_cfg(cfg->ctrl_mod, 0, cfg->mem_start_addr, test_start_addr - 1);
    setup_apm_regn_cfg(cfg->ctrl_mod, 1, test_end_addr, cfg->mem_end_addr);

    test_apm_ctrl_enable_intr(cfg->ctrl_mod, cfg->path);
    apm_hal_enable_ctrl_filter(cfg->ctrl_mod, cfg->path, true);
}

void test_tee_mode_default_access(void)
{
    test_apm_ctrl_reset_all();

    const uint32_t total_sz = 0x1000;
    test_sys_apm_mem_cfg_t cfg = {
        .ctrl_mod = GDMA_HPMEM_APM_CTRL,
        .path = GDMA_HPMEM_APM_PATH,
        .mem_start_addr = SOC_IRAM_LOW,
        .mem_end_addr = SOC_DRAM_HIGH - 1,
    };

    uint8_t *test_hpmem_buf = heap_caps_aligned_alloc(total_sz, total_sz, MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(test_hpmem_buf);
    memset(test_hpmem_buf, 0xA5, total_sz);

    uint8_t *test_dma_buf = heap_caps_aligned_alloc(total_sz, total_sz, MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(test_dma_buf);
    memset(test_dma_buf, 0xA5, total_sz);

    test_tee_mode_apm_setup(&cfg, (uint32_t)test_hpmem_buf, (uint32_t)test_hpmem_buf + total_sz);
    apm_ll_hp_tee_set_master_sec_mode(TEST_GDMA_APM_MASTER_ID, APM_SEC_MODE_TEE);

    uint32_t pms_attr = APM_PERM_R | APM_PERM_W;
#if SOC_APM_CTRL_TEE_MODE_ACCESS_BUG
    pms_attr = APM_PERM_NONE;
#endif

    gdma_xmem_addr_rw(test_hpmem_buf, test_dma_buf, total_sz, pms_attr & APM_PERM_R);
    gdma_xmem_addr_rw(test_dma_buf, test_hpmem_buf, total_sz, pms_attr & APM_PERM_W);

    free(test_dma_buf);
    free(test_hpmem_buf);
}
