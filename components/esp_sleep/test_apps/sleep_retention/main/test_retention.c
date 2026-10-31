/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "unity.h"
#include "esp_private/sleep_sys_periph.h"
#include "esp_private/sleep_retention.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"

#if SOC_CPU_HAS_PMA && CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
#include <stdio.h>
#include "riscv/csr.h"
#include "esp_cpu.h"
#include "esp_ipc.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#if CONFIG_IDF_TARGET_ESP32C5
#include "hal/efuse_hal.h"
#include "soc/chip_revision.h"
#endif
#endif

ESP_LOG_ATTR_TAG(TAG, "retention");

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
TEST_CASE("retention: can go to retention", "[retention]")
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    sleep_cpu_configure(true);

    TEST_ASSERT_EQUAL_INT32(true, peripheral_domain_pd_allowed());
    sleep_retention_dump_modules(stdout);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Going to sleep...");
    esp_light_sleep_start();

    ESP_LOGI(TAG, "After wakeup");
    TEST_ASSERT_EQUAL_INT32(true, peripheral_domain_pd_allowed());
    sleep_retention_dump_modules(stdout);

    sleep_cpu_configure(false);
}
#endif

#if SOC_CPU_HAS_PMA && CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
#if SOC_CPU_PMP_EXTENDED_REGIONS && !CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#define PMP_ENTRY_NUM   32
#else
#define PMP_ENTRY_NUM   16
#endif
#define PMA_ENTRY_NUM   16

typedef struct {
    uint32_t pmpaddr[PMP_ENTRY_NUM];
    uint32_t pmpcfg[PMP_ENTRY_NUM / 4];
    uint32_t pmaaddr[PMA_ENTRY_NUM];
    uint32_t pmacfg[PMA_ENTRY_NUM];
} pmp_pma_regs_t;

#define READ_PMPADDR(i) (r->pmpaddr[i] = RV_READ_CSR(CSR_PMPADDR0 + (i)))
#define READ_PMPCFG(i)  (r->pmpcfg[i] = RV_READ_CSR(CSR_PMPCFG0 + (i)))
#define READ_PMAADDR(i) (r->pmaaddr[i] = RV_READ_CSR(CSR_PMAADDR(i)))
#define READ_PMACFG(i)  (r->pmacfg[i] = RV_READ_CSR(CSR_PMACFG(i)))

static void read_pmp_pma(void *arg)
{
    pmp_pma_regs_t *r = (pmp_pma_regs_t *)arg;
    READ_PMPADDR(0);
    READ_PMPADDR(1);
    READ_PMPADDR(2);
    READ_PMPADDR(3);
    READ_PMPADDR(4);
    READ_PMPADDR(5);
    READ_PMPADDR(6);
    READ_PMPADDR(7);
    READ_PMPADDR(8);
    READ_PMPADDR(9);
    READ_PMPADDR(10);
    READ_PMPADDR(11);
    READ_PMPADDR(12);
    READ_PMPADDR(13);
    READ_PMPADDR(14);
    READ_PMPADDR(15);
    READ_PMPCFG(0);
    READ_PMPCFG(1);
    READ_PMPCFG(2);
    READ_PMPCFG(3);
#if PMP_ENTRY_NUM > 16
    READ_PMPADDR(16);
    READ_PMPADDR(17);
    READ_PMPADDR(18);
    READ_PMPADDR(19);
    READ_PMPADDR(20);
    READ_PMPADDR(21);
    READ_PMPADDR(22);
    READ_PMPADDR(23);
    READ_PMPADDR(24);
    READ_PMPADDR(25);
    READ_PMPADDR(26);
    READ_PMPADDR(27);
    READ_PMPADDR(28);
    READ_PMPADDR(29);
    READ_PMPADDR(30);
    READ_PMPADDR(31);
    READ_PMPCFG(4);
    READ_PMPCFG(5);
    READ_PMPCFG(6);
    READ_PMPCFG(7);
#endif
    READ_PMAADDR(0);
    READ_PMAADDR(1);
    READ_PMAADDR(2);
    READ_PMAADDR(3);
    READ_PMAADDR(4);
    READ_PMAADDR(5);
    READ_PMAADDR(6);
    READ_PMAADDR(7);
    READ_PMAADDR(8);
    READ_PMAADDR(9);
    READ_PMAADDR(10);
    READ_PMAADDR(11);
    READ_PMAADDR(12);
    READ_PMAADDR(13);
    READ_PMAADDR(14);
    READ_PMAADDR(15);
    READ_PMACFG(0);
    READ_PMACFG(1);
    READ_PMACFG(2);
    READ_PMACFG(3);
    READ_PMACFG(4);
    READ_PMACFG(5);
    READ_PMACFG(6);
    READ_PMACFG(7);
    READ_PMACFG(8);
    READ_PMACFG(9);
    READ_PMACFG(10);
    READ_PMACFG(11);
    READ_PMACFG(12);
    READ_PMACFG(13);
    READ_PMACFG(14);
    READ_PMACFG(15);
}

static pmp_pma_regs_t s_boot_regs[portNUM_PROCESSORS];

static void read_pmp_pma_all_cores(pmp_pma_regs_t regs[portNUM_PROCESSORS])
{
    int core = esp_cpu_get_core_id();
    read_pmp_pma(&regs[core]);
#if !CONFIG_FREERTOS_UNICORE
    int other_core = !core;
    ESP_ERROR_CHECK(esp_ipc_call_blocking(other_core, read_pmp_pma, &regs[other_core]));
#endif
}

void save_boot_pmp_pma(void)
{
    read_pmp_pma_all_cores(s_boot_regs);
}

static void check_pmp_pma_restored(bool keep_top_on)
{
    pmp_pma_regs_t after[portNUM_PROCESSORS] = {};
    esp_sleep_context_t sleep_ctx = {};

    TEST_ESP_OK(sleep_cpu_configure(true));
    TEST_ASSERT_TRUE(cpu_domain_pd_allowed());
    if (keep_top_on) {
        TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_TOP, ESP_PD_OPTION_ON));
    }
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(100 * 1000));

    esp_sleep_set_sleep_context(&sleep_ctx);
    TEST_ESP_OK(esp_light_sleep_start());
    read_pmp_pma_all_cores(after);
    esp_sleep_set_sleep_context(NULL);

    if (keep_top_on) {
        TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_TOP, ESP_PD_OPTION_AUTO));
    }
    TEST_ESP_OK(sleep_cpu_configure(false));

    TEST_ASSERT_EQUAL(ESP_OK, sleep_ctx.sleep_request_result);
    TEST_ASSERT_EQUAL_HEX32(PMU_SLEEP_PD_CPU, sleep_ctx.sleep_flags & PMU_SLEEP_PD_CPU);
    TEST_ASSERT_EQUAL_HEX32(keep_top_on ? 0 : PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);

    for (int core = 0; core < portNUM_PROCESSORS; core++) {
        char msg[32];
        snprintf(msg, sizeof(msg), "core %d pmpaddr", core);
        TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(s_boot_regs[core].pmpaddr, after[core].pmpaddr, PMP_ENTRY_NUM, msg);
        snprintf(msg, sizeof(msg), "core %d pmpcfg", core);
        TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(s_boot_regs[core].pmpcfg, after[core].pmpcfg, PMP_ENTRY_NUM / 4, msg);
        snprintf(msg, sizeof(msg), "core %d pmaaddr", core);
        TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(s_boot_regs[core].pmaaddr, after[core].pmaaddr, PMA_ENTRY_NUM, msg);
        snprintf(msg, sizeof(msg), "core %d pmacfg", core);
        TEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(s_boot_regs[core].pmacfg, after[core].pmacfg, PMA_ENTRY_NUM, msg);
    }
}

TEST_CASE("retention: PMP and PMA registers are restored after CPU power-down", "[retention]")
{
    check_pmp_pma_restored(true);
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
TEST_CASE("retention: PMP and PMA registers are restored after TOP power-down", "[retention]")
{
#if CONFIG_IDF_TARGET_ESP32C5
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102)) {
        TEST_IGNORE_MESSAGE("ESP32-C5 below v1.2 cannot power down the TOP domain");
    }
#endif
    check_pmp_pma_restored(false);
}
#endif
#endif
