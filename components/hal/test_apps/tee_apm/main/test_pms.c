/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "unity.h"
#include "test_pms.h"

#include "sdkconfig.h"

/**
 * Test default access behavior for TEE mode
 *
 * Verifies the expected default access policy for TEE mode as defined by APM design.
 * Typically, masters in TEE mode should have full access to memory regions not explicitly
 * covered by an APM region entry.
 *
 * On ESP32-C6 and ESP32-H2, a known hardware bug causes TEE-mode masters to be denied access
 * to such regions, incorrectly triggering APM exceptions. This test validates that behavior.
 *
 * The test allocates a region in HP_MEM without an APM region entry and performs GDMA
 * read/write operations in TEE mode. It confirms:
 * - On affected SoCs (e.g., C6, H2): APM exceptions are expected.
 * - On unaffected SoCs: GDMA access should succeed without any APM violations.
 */
TEST_CASE("Test TEE mode default access", "[SYS_APM]")
{
    test_tee_mode_default_access();
}

/**
 * Test HP-CPU access to CPU_PERI
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Configures multiple APM regions covering the CPU_PERI address space. For each region
 * and security mode (TEE, REE0, REE1, REE2), tests HP-CPU read/write access with varying
 * permissions: R-only (read passes, write fails) and W-only (write passes, read fail).
 * Verifies enforcement of APM access control by asserting expected APM violations.
 */
TEST_CASE("Test HP_CPU -> CPU_PERI access", "[SYS_APM]")
{
    test_sys_apm_master_hp_cpu_slave_cpu_peri();
}

/**
 * Test HP-CPU access to HP_PERI
 *
 * Configures multiple APM regions covering the HP_PERI address space. For each region
 * and security mode (TEE, REE0, REE1, REE2), tests HP-CPU read/write access with varying
 * permissions: R-only (read passes, write fails) and W-only (write passes, read fail).
 * Verifies enforcement of APM access control by asserting expected APM violations.
 */
TEST_CASE("Test HP_CPU -> HP_PERI access", "[SYS_APM]")
{
    test_sys_apm_master_hp_cpu_slave_hp_peri();
}

/**
 * Test HP-CPU access to LP_PERI
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Configures multiple APM regions covering the LP_PERI address space. For each region
 * and security mode (TEE, REE0, REE1, REE2), tests HP-CPU read/write access with varying
 * permissions: R-only (read passes, write fails) and W-only (write passes, read fail).
 * Verifies enforcement of APM access control by asserting expected APM violations.
 */
TEST_CASE("Test HP_CPU -> LP_PERI access", "[SYS_APM]")
{
    test_sys_apm_master_hp_cpu_slave_lp_peri();
}

#if SOC_APM_CPU_APM_SUPPORTED
/**
 * Test HP-CPU access to HP_MEM
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Divides a contiguous HP_MEM block into multiple APM regions and configures
 * their boundaries. For each region and security mode (TEE, REE0, REE1, REE2), tests
 * HP-CPU read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read and execute fail). Verifies enforcement of APM access control by
 * asserting expected APM violations.
 */
TEST_CASE("Test HP_CPU -> HP_MEM access", "[SYS_APM]")
{
    test_sys_apm_master_hp_cpu_slave_hpmem();
}
#endif /* SOC_APM_CPU_APM_SUPPORTED */

#if SOC_RTC_MEM_SUPPORTED
/**
 * Test HP-CPU access to LP_MEM
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Divides a contiguous LP_MEM block into multiple APM regions and configures
 * their boundaries. For each region and security mode (TEE, REE0, REE1, REE2), tests
 * HP-CPU read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read and execute fail). Verifies enforcement of APM access control by
 * asserting expected APM violations.
 */
TEST_CASE("Test HP_CPU -> LP_MEM access", "[SYS_APM]")
{
    test_sys_apm_master_hp_cpu_slave_lpmem();
}
#endif /* SOC_RTC_MEM_SUPPORTED */

/**
 * Test GDMA access to HP_MEM
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Divides a contiguous HP_MEM block into multiple APM regions and configures
 * their boundaries. For each region and security mode (TEE, REE0, REE1, REE2), tests
 * GDMA read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read fails). Verifies enforcement of APM access control by
 * asserting expected APM violations.
 */
TEST_CASE("Test GDMA -> HP_MEM access", "[SYS_APM]")
{
    test_sys_apm_master_gdma_slave_hpmem();
}

#if CONFIG_SPIRAM
/**
 * Test GDMA access to EXT_MEM (SPIRAM)
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Divides a contiguous EXT_MEM block into multiple APM regions and configures
 * their boundaries. For each region and security mode (TEE, REE0, REE1, REE2), tests
 * GDMA read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read fails). Verifies enforcement of APM access control by
 * asserting expected APM violations.
 */
TEST_CASE("Test GDMA -> EXT_MEM access", "[SYS_APM]")
{
    test_sys_apm_master_gdma_slave_extmem();
}
#endif /* CONFIG_SPIRAM */

#if CONFIG_ULP_COPROC_ENABLED
/**
 * Test LP-CPU access to LP_PERI
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Configures multiple APM regions covering the LP_PERI address space. For each region
 * and security mode (TEE, REE0, REE1, REE2), tests LP-CPU read/write access with varying
 * permissions: R-only (read passes, write fails) and W-only (write passes, read fail).
 * Verifies enforcement of APM access control by asserting expected APM violations.
 */
TEST_CASE("Test LP_CPU -> LP_PERI access", "[SYS_APM]")
{
    test_sys_apm_master_lp_cpu_slave_lp_peri();
}

/**
 * Test LP-CPU access to LP_MEM
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Divides a contiguous LP_MEM block into multiple APM regions and configures
 * their boundaries. For each region and security mode (TEE, REE0, REE1, REE2), tests
 * LP-CPU read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read and execute fail). Verifies enforcement of APM access control by
 * asserting expected APM violations.
 */
TEST_CASE("Test LP_CPU -> LP_MEM access", "[SYS_APM]")
{
    test_sys_apm_master_lp_cpu_slave_lpmem();
}

/**
 * Test LP-CPU access to HP_MEM
 *
 * SYS_APM uses region-based access control with start/end address boundaries and per-mode permissions.
 * Divides a contiguous HP_MEM block into multiple APM regions and configures
 * their boundaries. For each region and security mode (TEE, REE0, REE1, REE2), tests
 * LP-CPU read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read and execute fail). Verifies enforcement of APM access control by
 * asserting expected APM violations.
 *
 * Also verifies that LP-CPU can forcibly access HP_MEM regardless of APM permission settings.
 */
TEST_CASE("Test LP_CPU -> HP_MEM access", "[SYS_APM]")
{
    test_sys_apm_master_lp_cpu_slave_hpmem();
}
#endif /* CONFIG_ULP_COPROC_ENABLED */

#if SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL
/**
 * Test HP-CPU access to HP_PERI using PERI_APM
 *
 * PERI_APM uses per-peripheral based access control with per-mode permissions.
 * For each peripheral and security mode (TEE, REE0, REE1, REE2), tests HP-CPU
 * read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read fail). Verifies enforcement of access control
 * by asserting expected load/store access faults.
 */
TEST_CASE("Test HP_CPU -> HP_PERI access", "[PERI_APM]")
{
    test_peri_apm_master_hp_cpu_slave_hp_peri();
}

/**
 * Test HP-CPU access to LP_PERI using PERI_APM
 *
 * PERI_APM uses per-peripheral based access control with per-mode permissions.
 * For each peripheral and security mode (TEE, REE0, REE1, REE2), tests HP-CPU
 * read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read fail). Verifies enforcement of access control
 * by asserting expected load/store access faults.
 */
TEST_CASE("Test HP_CPU -> LP_PERI access", "[PERI_APM]")
{
    test_peri_apm_master_hp_cpu_slave_lp_peri();
}

#if CONFIG_ULP_COPROC_ENABLED
/**
 * Test LP-CPU access to HP_PERI using PERI_APM
 *
 * PERI_APM uses per-peripheral based access control with per-mode permissions.
 * For each peripheral and security mode (TEE, REE0, REE1, REE2), tests LP-CPU
 * read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read fail). Verifies enforcement of access control
 * by asserting expected load/store access faults.
 */
TEST_CASE("Test LP_CPU -> HP_PERI access", "[PERI_APM]")
{
    test_peri_apm_master_lp_cpu_slave_hp_peri();
}

/**
 * Test LP-CPU access to LP_PERI using PERI_APM
 *
 * PERI_APM uses per-peripheral based access control with per-mode permissions.
 * For each peripheral and security mode (TEE, REE0, REE1, REE2), tests LP-CPU
 * read/write access with varying permissions: R-only (read passes, write fails) and
 * W-only (write passes, read fail). Verifies enforcement of access control
 * by asserting expected load/store access faults.
 */
TEST_CASE("Test LP_CPU -> LP_PERI access", "[PERI_APM]")
{
    test_peri_apm_master_lp_cpu_slave_lp_peri();
}
#endif /* CONFIG_ULP_COPROC_ENABLED */
#endif /* SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL */
