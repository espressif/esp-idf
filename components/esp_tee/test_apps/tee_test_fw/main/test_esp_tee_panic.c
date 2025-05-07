/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/aes_reg.h"
#include "soc/hmac_reg.h"
#include "soc/ds_reg.h"
#include "soc/efuse_reg.h"
#include "soc/pcr_reg.h"
#include "soc/lp_analog_peri_reg.h"
#include "soc/lp_wdt_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/ext_mem_defs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "unity.h"
#include "esp_tee.h"
#include "secure_service_num.h"

#define ALIGN_DOWN_TO_MMU_PAGE_SIZE(addr)  ((addr) & ~((SOC_MMU_PAGE_SIZE) - 1))

extern uint32_t _iram_start;
extern uint32_t _instruction_reserved_start;

/* ---------------------------------------------------- Basic Exceptions for TEE tests ---------------------------------------------------- */

#if CONFIG_SECURE_TEE_SEC_STG_MODE_RELEASE
#define TEST_APM_EFUSE_PROT_REG \
    (EFUSE_RD_KEY0_DATA0_REG + (CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID * 0x20))
#else
#define TEST_APM_EFUSE_PROT_REG EFUSE_RD_KEY5_DATA0_REG
#endif

TEST_CASE("Test APM violation interrupt: eFuse", "[apm_violation]")
{
    uint32_t val = UINT32_MAX;
    val = REG_READ(TEST_APM_EFUSE_PROT_REG);
    TEST_ASSERT_EQUAL(0, val);
    TEST_FAIL_MESSAGE("APM violation interrupt should have been generated");
}

TEST_CASE("Test APM violation interrupt: MMU", "[apm_violation]")
{
    uint32_t val = UINT32_MAX;
    REG_WRITE(SPI_MEM_MMU_ITEM_INDEX_REG(0), SOC_MMU_ENTRY_NUM - 2);
    val = REG_READ(SPI_MEM_MMU_ITEM_CONTENT_REG(0));
    TEST_ASSERT_EQUAL(0, val);
    TEST_FAIL_MESSAGE("APM violation interrupt should have been generated");
}

TEST_CASE("Test APM violation interrupt: AES", "[apm_violation]")
{
    uint32_t val = UINT32_MAX;
    val = REG_READ(AES_KEY_2_REG);
    TEST_ASSERT_EQUAL(0, val);
    TEST_FAIL_MESSAGE("APM violation interrupt should have been generated");
}

TEST_CASE("Test APM violation interrupt: HMAC", "[apm_violation]")
{
    uint32_t val = UINT32_MAX;
    val = REG_READ(HMAC_SET_PARA_KEY_REG);
    TEST_ASSERT_EQUAL(0, val);
    TEST_FAIL_MESSAGE("APM violation interrupt should have been generated");
}

TEST_CASE("Test APM violation interrupt: DS", "[apm_violation]")
{
    uint32_t val = UINT32_MAX;
    val = REG_READ(DS_Z_MEM);
    TEST_ASSERT_EQUAL(0, val);
    TEST_FAIL_MESSAGE("APM violation interrupt should have been generated");
}

TEST_CASE("Test APM violation interrupt: SHA PCR", "[apm_violation]")
{
    uint32_t val = 0;
    REG_WRITE(PCR_SHA_CONF_REG, val);
    TEST_FAIL_MESSAGE("APM violation interrupt should have been generated");
}

TEST_CASE("Test APM violation interrupt: ECC PCR", "[apm_violation]")
{
    uint32_t val = 0;
    REG_WRITE(PCR_ECC_CONF_REG, val);
    TEST_FAIL_MESSAGE("APM violation interrupt should have been generated");
}

/* TEE IRAM: Reserved/Vector-table boundary */
TEST_CASE("Test TEE-TEE violation: IRAM (W1)", "[exception]")
{
    esp_tee_service_call(1, SS_ESP_TEE_TEST_IRAM_REG1_WRITE_VIOLATION);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* Illegal memory space: Write */
TEST_CASE("Test TEE-TEE violation: Reserved (W1)", "[exception]")
{
    esp_tee_service_call(1, SS_ESP_TEE_TEST_RESV_REG1_WRITE_VIOLATION);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* Illegal memory space: Execution */
TEST_CASE("Test TEE-TEE violation: Reserved (X1)", "[exception]")
{
    esp_tee_service_call(1, SS_ESP_TEE_TEST_RESV_REG1_EXEC_VIOLATION);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE IRAM: Vector table region */
TEST_CASE("Test TEE-TEE violation: IRAM (W2)", "[exception]")
{
    esp_tee_service_call(1, SS_ESP_TEE_TEST_IRAM_REG2_WRITE_VIOLATION);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE DRAM: Stack region */
TEST_CASE("Test TEE-TEE violation: DRAM (X1)", "[exception]")
{
    esp_tee_service_call(1, SS_ESP_TEE_TEST_DRAM_REG1_EXEC_VIOLATION);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE DRAM: Heap region */
TEST_CASE("Test TEE-TEE violation: DRAM (X2)", "[exception]")
{
    esp_tee_service_call(1, SS_ESP_TEE_TEST_DRAM_REG2_EXEC_VIOLATION);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* Illegal Instruction */
TEST_CASE("Test TEE-TEE violation: Illegal Instruction", "[exception]")
{
    esp_tee_service_call(1, SS_ESP_TEE_TEST_ILLEGAL_INSTRUCTION);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE DRAM -REE IRAM Boundary */
TEST_CASE("Test REE-TEE isolation: DRAM (R1)", "[exception]")
{
    uint32_t* val = (uint32_t *)(&_iram_start - 0x04);
    TEST_ASSERT_EQUAL(0, *val);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE DRAM -REE IRAM Boundary */
TEST_CASE("Test REE-TEE isolation: DRAM (W1)", "[exception]")
{
    *(uint32_t *)(&_iram_start - 0x04) = 0xbadc0de;
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE IRAM region */
TEST_CASE("Test REE-TEE isolation: IRAM (R1)", "[exception]")
{
    uint32_t *val = (uint32_t *)(&_iram_start - (CONFIG_SECURE_TEE_IRAM_SIZE + CONFIG_SECURE_TEE_DRAM_SIZE) + 0x04);
    TEST_ASSERT_EQUAL(0, *val);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE IRAM region */
TEST_CASE("Test REE-TEE isolation: IRAM (W1)", "[exception]")
{
    *(uint32_t *)(&_iram_start - (CONFIG_SECURE_TEE_IRAM_SIZE + CONFIG_SECURE_TEE_DRAM_SIZE) + 0x04) = 0xbadc0de;
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE IROM region */
TEST_CASE("Test REE-TEE isolation: IROM (R1)", "[exception]")
{
    uint32_t *val = (uint32_t *)(SOC_IROM_LOW + 0x04);
    TEST_ASSERT_EQUAL(0, *val);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE IROM region */
TEST_CASE("Test REE-TEE isolation: IROM (W1)", "[exception]")
{
    *(uint32_t *)(SOC_IROM_LOW + 0x04) = 0xbadc0de;
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE DROM - REE IROM boundary */
TEST_CASE("Test REE-TEE isolation: DROM (R1)", "[exception]")
{
    const uint32_t test_addr = ALIGN_DOWN_TO_MMU_PAGE_SIZE((uint32_t)&_instruction_reserved_start);
    uint32_t *val = (uint32_t *)(test_addr - 0x04);
    TEST_ASSERT_EQUAL(0, *val);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* TEE DROM - REE IROM boundary */
TEST_CASE("Test REE-TEE isolation: DROM (W1)", "[exception]")
{
    const uint32_t test_addr = ALIGN_DOWN_TO_MMU_PAGE_SIZE((uint32_t)&_instruction_reserved_start);
    *(uint32_t *)(test_addr - 0x04) = 0xbadc0de;
    TEST_FAIL_MESSAGE("Exception should have been generated");
}

/* SWDT/BOD Reset register */
TEST_CASE("Test REE-TEE isolation: SWDT/BOD (W)", "[exception]")
{
    REG_WRITE(LP_ANALOG_PERI_LP_ANA_FIB_ENABLE_REG, 0);
    TEST_FAIL_MESSAGE("Exception should have been generated");
}
