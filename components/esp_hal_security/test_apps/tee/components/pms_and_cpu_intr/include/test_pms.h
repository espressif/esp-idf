/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"

/* Test-cases */
#if SOC_APM_CTRL_FILTER_SUPPORTED
void test_sys_apm_master_hp_cpu_slave_hp_peri(void);

void test_sys_apm_master_hp_cpu_slave_cpu_peri(void);

void test_sys_apm_master_gdma_slave_hpmem(void);

void test_sys_apm_master_hp_cpu_slave_lpmem(void);

void test_sys_apm_master_lp_cpu_slave_lp_peri(void);

void test_sys_apm_master_hp_cpu_slave_lp_peri(void);

void test_sys_apm_master_lp_cpu_slave_lpmem(void);

void test_sys_apm_master_lp_cpu_slave_hpmem(void);

void test_sys_apm_master_gdma_slave_extmem(void);

void test_sys_apm_master_hp_cpu_slave_hpmem(void);

void test_peri_apm_master_hp_cpu_slave_hp_peri(void);

void test_peri_apm_master_hp_cpu_slave_lp_peri(void);

void test_peri_apm_master_lp_cpu_slave_hp_peri(void);

void test_peri_apm_master_lp_cpu_slave_lp_peri(void);

void test_tee_mode_default_access(void);
#else
void test_sys_pms_master_hp_cpu_slave_hp_peri(void);

void test_sys_pms_master_hp_cpu_slave_lp_peri(void);

void test_sys_pms_master_hp_cpu_peri_regn_access(void);

void test_sys_pms_master_lp_cpu_slave_hp_peri(void);

void test_sys_pms_master_lp_cpu_slave_lp_peri(void);

void test_sys_pms_master_lp_cpu_peri_regn_access(void);

void test_sys_pms_master_gdma_slave_hp_mem(void);

void test_sys_pms_master_gdma_slave_ext_mem(void);
#endif
