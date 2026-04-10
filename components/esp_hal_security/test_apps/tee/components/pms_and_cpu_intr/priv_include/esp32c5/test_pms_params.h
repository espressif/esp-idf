/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/reg_base.h"

#include "hal/apm_types.h"
#include "esp_bit_defs.h"

/*
 * NOTE: By default, TEE mode should have unrestricted access to the entire CPU address space.
 * However, for ESP32-C6 and ESP32-H2, it has been observed that when APM filters are enabled,
 * TEE mode accesses are incorrectly being filtered based on the region configurations and
 * access attributes set for REE[0..2] modes.
 *
 * This behaviour has been fixed on later SoCs.
 */
#define SOC_APM_CTRL_TEE_MODE_ACCESS_BUG   0

/********* GDMA APM master ID *********/
#define TEST_GDMA_APM_MASTER_ID         26    /* GDMA_DUMMY10 */

/********* APM controller and path definitions *********/
#define HP_CPU_CPUPERI_APM_CTRL         APM_CTRL_HP_APM
#define HP_CPU_CPUPERI_APM_PATH         APM_CTRL_ACCESS_PATH_M0
/* NOTE: ESP32-C5 has only 4 CPU peripherals */
#define TEST_HP_CPU_CPUPERI_REGN_NUM    4

#define GDMA_HPMEM_APM_CTRL             APM_CTRL_HP_APM
#define GDMA_HPMEM_APM_PATH             APM_CTRL_ACCESS_PATH_M1
#define GDMA_HPMEM_TEST_REGN_NUM        APM_CTRL_HP_APM_REGION_NUM

#define LP_CPU_HPMEM_APM_CTRL           APM_CTRL_HP_APM
#define LP_CPU_HPMEM_APM_PATH           APM_CTRL_ACCESS_PATH_M2
#define LP_CPU_HPMEM_TEST_REGN_NUM      APM_CTRL_HP_APM_REGION_NUM

#define HP_CPU_HPPERI_APM_CTRL          APM_CTRL_HP_APM
#define HP_CPU_HPPERI_APM_PATH          APM_CTRL_ACCESS_PATH_M3
#define HP_CPU_HPPERI_TEST_REGN_NUM     APM_CTRL_HP_APM_REGION_NUM

#define GDMA_EXTMEM_APM_CTRL            APM_CTRL_HP_APM
#define GDMA_EXTMEM_APM_PATH            APM_CTRL_ACCESS_PATH_M4
#define GDMA_EXTMEM_TEST_REGN_NUM       APM_CTRL_HP_APM_REGION_NUM

#define LP_CPU_LPMEM_APM_CTRL           APM_CTRL_LP_APM
#define LP_CPU_LPMEM_APM_PATH           APM_CTRL_ACCESS_PATH_M0
#define LP_CPU_LPMEM_TEST_REGN_NUM      APM_CTRL_LP_APM_REGION_NUM

#define HP_CPU_LPPERI_APM_CTRL          APM_CTRL_LP_APM
#define HP_CPU_LPPERI_APM_PATH          APM_CTRL_ACCESS_PATH_M1
#define HP_CPU_LPPERI_TEST_REGN_NUM     APM_CTRL_LP_APM_REGION_NUM

#define LP_CPU_LPPERI_APM_CTRL          APM_CTRL_LP_APM
#define LP_CPU_LPPERI_APM_PATH          APM_CTRL_ACCESS_PATH_M1
#define LP_CPU_LPPERI_TEST_REGN_NUM     APM_CTRL_LP_APM_REGION_NUM

#define HP_CPU_LPMEM_APM_CTRL           APM_CTRL_LP_APM0
#define HP_CPU_LPMEM_APM_PATH           APM_CTRL_ACCESS_PATH_M0
#define HP_CPU_LPMEM_TEST_REGN_NUM      APM_CTRL_LP_APM0_REGION_NUM

#define HP_CPU_HPMEM_APM_CTRL           APM_CTRL_CPU_APM
#define HP_CPU_HPMEM_I_APM_PATH         APM_CTRL_ACCESS_PATH_M0
#define HP_CPU_HPMEM_D_APM_PATH         APM_CTRL_ACCESS_PATH_M1
#define HP_CPU_HPMEM_TEST_REGN_NUM      APM_CTRL_CPU_APM_REGION_NUM

/********* APM peripheral space test regions *********/
#define ALIGN_TO_NEXT_4KB(addr)         (((addr) + 0x1000) & ~0xFFF)

/********* CPU_PERI (4 regions) *********/
#define TEST_CPU_PERI_REGION0_START     DR_REG_TRACE_BASE
#define TEST_CPU_PERI_REGION0_END       DR_REG_BUS_MONITOR_BASE
#define TEST_CPU_PERI_ADDR0             TEST_CPU_PERI_REGION0_START

#define TEST_CPU_PERI_REGION1_START     DR_REG_BUS_MONITOR_BASE
#define TEST_CPU_PERI_REGION1_END       DR_REG_INTPRI_BASE
#define TEST_CPU_PERI_ADDR1             TEST_CPU_PERI_REGION1_START

#define TEST_CPU_PERI_REGION2_START     DR_REG_INTPRI_BASE
#define TEST_CPU_PERI_REGION2_END       DR_REG_CACHE_BASE
#define TEST_CPU_PERI_ADDR2             TEST_CPU_PERI_REGION2_START

#define TEST_CPU_PERI_REGION3_START     DR_REG_CACHE_BASE
#define TEST_CPU_PERI_REGION3_END       ALIGN_TO_NEXT_4KB(DR_REG_CACHE_BASE)
#define TEST_CPU_PERI_ADDR3             TEST_CPU_PERI_REGION3_START

#define TEST_CPU_PERI_RESV_MASK         (0U)

/********* HP_PERI (16 regions) *********/
/* PERI0 */
/* 0: UART0 - I2C - Reserved (UART0 accessed) */
#define TEST_HP_PERI_REGION0_START      DR_REG_UART0_BASE
#define TEST_HP_PERI_REGION0_END        DR_REG_I2C_BASE
#define TEST_HP_PERI_ADDR0              TEST_HP_PERI_REGION0_START

#define TEST_HP_PERI_REGION1_START      DR_REG_I2C_BASE
#define TEST_HP_PERI_REGION1_END        DR_REG_TIMERG0_BASE
#define TEST_HP_PERI_ADDR1              TEST_HP_PERI_REGION1_START

#define TEST_HP_PERI_REGION2_START      DR_REG_TIMERG0_BASE
#define TEST_HP_PERI_REGION2_END        DR_REG_I2S_BASE
#define TEST_HP_PERI_ADDR2              TEST_HP_PERI_REGION2_START

#define TEST_HP_PERI_REGION3_START      DR_REG_I2S_BASE
#define TEST_HP_PERI_REGION3_END        DR_REG_INTMTX_BASE
#define TEST_HP_PERI_ADDR3              TEST_HP_PERI_REGION3_START

#define TEST_HP_PERI_REGION4_START      DR_REG_INTMTX_BASE
#define TEST_HP_PERI_REGION4_END        DR_REG_MCPWM_BASE
#define TEST_HP_PERI_ADDR4              TEST_HP_PERI_REGION4_START

#define TEST_HP_PERI_REGION5_START      DR_REG_MCPWM_BASE
#define TEST_HP_PERI_REGION5_END        DR_REG_SLCHOST_BASE
#define TEST_HP_PERI_ADDR5              TEST_HP_PERI_REGION5_START

#define TEST_HP_PERI_REGION6_START      DR_REG_SLCHOST_BASE
#define TEST_HP_PERI_REGION6_END        ALIGN_TO_NEXT_4KB(DR_REG_PSRAM_MEM_MONITOR_BASE)
#define TEST_HP_PERI_ADDR6              TEST_HP_PERI_REGION6_START

/* PERI1 */
#define TEST_HP_PERI_REGION7_START      DR_REG_AHB_DMA_BASE
#define TEST_HP_PERI_REGION7_END        ALIGN_TO_NEXT_4KB(DR_REG_BITSCRAMBLER_BASE)
#define TEST_HP_PERI_ADDR7              TEST_HP_PERI_REGION7_START

#define TEST_HP_PERI_REGION8_START      DR_REG_KEYMNG_BASE
#define TEST_HP_PERI_REGION8_END        DR_REG_RSA_BASE
#define TEST_HP_PERI_ADDR8              TEST_HP_PERI_REGION8_START

#define TEST_HP_PERI_REGION9_START      DR_REG_RSA_BASE
#define TEST_HP_PERI_REGION9_END        DR_REG_DS_BASE
#define TEST_HP_PERI_ADDR9              TEST_HP_PERI_REGION9_START

#define TEST_HP_PERI_REGION10_START     DR_REG_DS_BASE
#define TEST_HP_PERI_REGION10_END       ALIGN_TO_NEXT_4KB(DR_REG_ECDSA_BASE)
#define TEST_HP_PERI_ADDR10             TEST_HP_PERI_REGION10_START

/* PERI2 */
#define TEST_HP_PERI_REGION11_START     DR_REG_IO_MUX_BASE
#define TEST_HP_PERI_REGION11_END       DR_REG_MEM_MONITOR_BASE
#define TEST_HP_PERI_ADDR11             TEST_HP_PERI_REGION11_START

#define TEST_HP_PERI_REGION12_START     DR_REG_MEM_MONITOR_BASE
#define TEST_HP_PERI_REGION12_END       DR_REG_HP_SYSTEM_BASE
#define TEST_HP_PERI_ADDR12             TEST_HP_PERI_REGION12_START

#define TEST_HP_PERI_REGION13_START     DR_REG_HP_SYSTEM_BASE
#define TEST_HP_PERI_REGION13_END       DR_REG_TEE_BASE
#define TEST_HP_PERI_ADDR13             TEST_HP_PERI_REGION13_START

/* 14: TEE - CPU_APM (TEE and APM CTRL protected by PERI_APM) */
#define TEST_HP_PERI_REGION14_START     DR_REG_TEE_BASE
#define TEST_HP_PERI_REGION14_END       ALIGN_TO_NEXT_4KB(DR_REG_CPU_APM_BASE)
#define TEST_HP_PERI_ADDR14             TEST_HP_PERI_REGION14_START

#define TEST_HP_PERI_REGION15_START     DR_REG_MISC_BASE
#define TEST_HP_PERI_REGION15_END       DR_REG_PMU_BASE
#define TEST_HP_PERI_ADDR15             TEST_HP_PERI_REGION15_START

#define TEST_HP_PERI_RESV_MASK          BIT(0) | BIT(14)

/********* LP_PERI (8 regions) *********/
/* 0: PMU - Reserved (PMU_LP_CPU_PWR1_REG accessed) */
#define TEST_LP_PERI_REGION0_START      DR_REG_PMU_BASE
#define TEST_LP_PERI_REGION0_END        DR_REG_LP_CLKRST_BASE
#define TEST_LP_PERI_ADDR0              TEST_LP_PERI_REGION0_START

#define TEST_LP_PERI_REGION1_START      DR_REG_LP_CLKRST_BASE
#define TEST_LP_PERI_REGION1_END        DR_REG_LP_AON_BASE
#define TEST_LP_PERI_ADDR1              TEST_LP_PERI_REGION1_START

/* 2: LP_AON - Reserved (LP_AON_STOREn_REG accessed) */
#define TEST_LP_PERI_REGION2_START      DR_REG_LP_AON_BASE
#define TEST_LP_PERI_REGION2_END        DR_REG_LP_UART_BASE
#define TEST_LP_PERI_ADDR2              TEST_LP_PERI_REGION2_START

#define TEST_LP_PERI_REGION3_START      DR_REG_LP_UART_BASE
#define TEST_LP_PERI_REGION3_END        DR_REG_LP_WDT_BASE
#define TEST_LP_PERI_ADDR3              TEST_LP_PERI_REGION3_START

#define TEST_LP_PERI_REGION4_START      DR_REG_LP_WDT_BASE
#define TEST_LP_PERI_REGION4_END        DR_REG_LP_I2C_ANA_MST_BASE
#define TEST_LP_PERI_ADDR4              TEST_LP_PERI_REGION4_START

#define TEST_LP_PERI_REGION5_START      DR_REG_LP_I2C_ANA_MST_BASE
#define TEST_LP_PERI_REGION5_END        DR_REG_HUK_BASE
#define TEST_LP_PERI_ADDR5              TEST_LP_PERI_REGION5_START

#define TEST_LP_PERI_REGION6_START      DR_REG_HUK_BASE
#define TEST_LP_PERI_REGION6_END        DR_REG_LP_IO_MUX_BASE
#define TEST_LP_PERI_ADDR6              TEST_LP_PERI_REGION6_START

#define TEST_LP_PERI_REGION7_START      DR_REG_LP_IO_MUX_BASE
#define TEST_LP_PERI_REGION7_END        ALIGN_TO_NEXT_4KB(DR_REG_OTP_DEBUG_BASE)
#define TEST_LP_PERI_ADDR7              TEST_LP_PERI_REGION7_START

#define TEST_LP_PERI_RESV_MASK          BIT(0) | BIT(2)

/********* For PERI_APM *********/

#include "soc/reg_base.h"

/********* For PERI_APM *********/
#include "soc/uart_reg.h"
#include "soc/i2c_reg.h"
#include "soc/i2s_reg.h"
#include "soc/mcpwm_reg.h"
#include "soc/twaifd_reg.h"
#include "soc/ahb_dma_reg.h"
#include "soc/pau_reg.h"
#include "soc/interrupt_matrix_reg.h"
#include "soc/apb_saradc_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/pcnt_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/hp_system_reg.h"
#include "soc/pcr_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/hp_apm_reg.h"
#include "soc/cpu_apm_reg.h"
#include "soc/sha_reg.h"
#include "soc/cache_reg.h"
#include "soc/spi_reg.h"
#include "soc/bitscrambler_reg.h"
#include "soc/keymng_reg.h"
#include "soc/sdio_slc_host_reg.h"

#include "soc/efuse_reg.h"
#include "soc/pmu_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_aon_reg.h"
#include "soc/lp_wdt_reg.h"
#include "soc/lpperi_reg.h"
#include "soc/lp_analog_peri_reg.h"
#include "soc/lp_uart_reg.h"
#include "soc/lp_i2c_reg.h"
#include "soc/lp_i2c_ana_mst_reg.h"
#include "soc/huk_reg.h"
#include "soc/lp_apm_reg.h"
