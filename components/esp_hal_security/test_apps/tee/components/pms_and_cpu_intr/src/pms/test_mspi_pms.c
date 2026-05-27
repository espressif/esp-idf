/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#include "hal/mspi_pms_ll.h"

#include "esp_attr.h"
#include "esp_partition.h"
#include "esp_rom_spiflash.h"
#include "esp_flash_partitions.h"

#include "test_pms.h"
#include "test_pms_priv.h"
#include "test_pms_params.h"   /* may define SOC_MSPI_PMS_SUPPORT_ONLY_TEE_MODE */

#if !SOC_MSPI_PMS_SUPPORT_ONLY_TEE_MODE
#include "esp_cpu.h"
#include "riscv/rv_utils.h"
#endif

#include "unity.h"

#define FLASH_SECTOR_SIZE               (0x1000)
#define GET_PART_TEST_ADDR(addr, size)  (addr + size - FLASH_SECTOR_SIZE)

typedef struct {
    uint32_t addr;
    uint32_t size;
    mspi_pms_attr_t attr;
    uint32_t test_addr;
} test_mspi_pms_regn_cfg_t;

extern volatile bool pms_mspi_intr_flag;

#if !SOC_MSPI_PMS_SUPPORT_ONLY_TEE_MODE
IRAM_ATTR static void switch_priv_mode(void)
{
    int mode = esp_cpu_get_curr_privilege_level();
    if (mode == PRV_M) {
        rv_utils_intr_global_disable();
        test_m2u_switch();
    } else {
        test_u2m_switch();
        rv_utils_intr_global_enable();
    }
}
#endif

static void test_flash_mspi_regn_setup(const test_mspi_pms_regn_cfg_t *cfg, size_t regn_num)
{
    for (uint32_t idx = 0; idx < regn_num; idx++) {
        mspi_ll_pms_set_region_addr(MSPI_PMS_MEM_FLASH, idx, cfg[idx].addr);
        mspi_ll_pms_set_region_size(MSPI_PMS_MEM_FLASH, idx, cfg[idx].size);
        mspi_ll_pms_set_region_attr(MSPI_PMS_MEM_FLASH, idx, MSPI_PMS_MODE_TEE, cfg[idx].attr);
#if !SOC_MSPI_PMS_SUPPORT_ONLY_TEE_MODE
        mspi_ll_pms_set_region_attr(MSPI_PMS_MEM_FLASH, idx, MSPI_PMS_MODE_REE, cfg[idx].attr);
#endif
    }
}

static void hp_cpu_flash_addr_rw(mspi_pms_mode_t mode, uint32_t flash_addr, uint32_t attr)
{
    static uint32_t rbuf[4] = {0};
    static const uint32_t wbuf[4] = {0xAA, 0xBB, 0xCC, 0xDD};

#if !SOC_MSPI_PMS_SUPPORT_ONLY_TEE_MODE
    bool is_ree = (mode == MSPI_PMS_MODE_REE);
    if (is_ree) {
        switch_priv_mode();
    }
#else
    (void)mode;
#endif

    pms_mspi_intr_flag = false;
    esp_rom_spiflash_read(flash_addr, rbuf, sizeof(rbuf));
    test_delay_ms(10);
    bool can_read = (attr & MSPI_PMS_ATTR_RD);
    TEST_ASSERT(pms_mspi_intr_flag != can_read);

    pms_mspi_intr_flag = false;
    esp_rom_spiflash_write(flash_addr, wbuf, sizeof(wbuf));
    test_delay_ms(10);
    bool can_write = (attr & MSPI_PMS_ATTR_WR);
    TEST_ASSERT(pms_mspi_intr_flag != can_write);

    pms_mspi_intr_flag = false;

#if !SOC_MSPI_PMS_SUPPORT_ONLY_TEE_MODE
    if (is_ree) {
        switch_priv_mode();
    }
#endif
}

static void test_flash_mspi_mode_access(const test_mspi_pms_regn_cfg_t *cfg, size_t regn_num, mspi_pms_mode_t mode)
{
    static const mspi_pms_attr_t test_attrs[] = {
        MSPI_PMS_ATTR_RD,
        MSPI_PMS_ATTR_WR,
    };

    for (size_t regn = 0; regn < regn_num; regn++) {
        for (size_t i = 0; i < sizeof(test_attrs) / sizeof(test_attrs[0]); i++) {
            mspi_ll_pms_set_region_attr(MSPI_PMS_MEM_FLASH, regn, mode, test_attrs[i]);
            hp_cpu_flash_addr_rw(mode, cfg[regn].test_addr, test_attrs[i]);
        }
        /* Restore full access before moving on */
        mspi_ll_pms_set_region_attr(MSPI_PMS_MEM_FLASH, regn, mode, MSPI_PMS_ATTR_RD | MSPI_PMS_ATTR_WR);
    }
}

static void test_hp_cpu_to_flash_mspi_access(const test_mspi_pms_regn_cfg_t *cfg, size_t regn_num)
{
    test_flash_mspi_mode_access(cfg, regn_num, MSPI_PMS_MODE_TEE);
    /*
     * NOTE: The MSPI flash/PSRAM PMS configuration registers have no lock bit and
     * were observed to be writable from U-mode (REE). The PMS block therefore
     * cannot protect its own configuration - REE could simply widen a region and
     * bypass the flash/PSRAM protection. The MSPI peripheral register space must be
     * guarded by SYS_APM (deny REE access to the MSPI peripheral) so that REE cannot
     * reconfigure the PMS. Hence there is no REE-writes-PMS-config test case here:
     * that integrity is an APM responsibility, not an MSPI PMS one.
     *
     * TODO: Need to check this ESP32-P4
     */
#if !SOC_MSPI_PMS_SUPPORT_ONLY_TEE_MODE
    test_flash_mspi_mode_access(cfg, regn_num, MSPI_PMS_MODE_REE);
#endif
}

void test_sys_pms_master_hp_cpu_slave_flash_mspi(void)
{
    const esp_partition_t *nvs_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    TEST_ASSERT_NOT_NULL(nvs_part);

    const esp_partition_t *factory_part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, NULL);
    TEST_ASSERT_NOT_NULL(factory_part);

    const esp_partition_t *custom_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "custom");
    TEST_ASSERT_NOT_NULL(custom_part);

    const test_mspi_pms_regn_cfg_t regn_cfg[MSPI_LL_PMS_REGION_NUM] = {
        [0] = {
            .addr = 0x00,
            .size = nvs_part->address,
            .attr = MSPI_PMS_ATTR_NONE,
            .test_addr = CONFIG_PARTITION_TABLE_OFFSET + ESP_PARTITION_TABLE_MAX_LEN,
        },
        [1] = {
            .addr = nvs_part->address,
            .size = nvs_part->size,
            .attr = MSPI_PMS_ATTR_RD | MSPI_PMS_ATTR_WR,
            .test_addr = GET_PART_TEST_ADDR(nvs_part->address, nvs_part->size)
        },
        [2] = {
            .addr = factory_part->address,
            .size = factory_part->size,
            .attr = MSPI_PMS_ATTR_RD | MSPI_PMS_ATTR_WR,
            .test_addr = GET_PART_TEST_ADDR(factory_part->address, factory_part->size)
        },
        [3] = {
            .addr = custom_part->address,
            .size = custom_part->size,
            .attr = MSPI_PMS_ATTR_RD | MSPI_PMS_ATTR_WR,
            .test_addr = GET_PART_TEST_ADDR(custom_part->address, custom_part->size)
        },
    };

    test_flash_mspi_regn_setup(regn_cfg, MSPI_LL_PMS_REGION_NUM);
    test_pms_mspi_enable_intr();

    set_test_vector_table();
    test_hp_cpu_to_flash_mspi_access(regn_cfg, MSPI_LL_PMS_REGION_NUM);
    restore_default_vector_table();

    test_pms_mspi_disable_intr();
}
