/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"

#include "soc/lp_aon_reg.h"
#include "hal/apm_types.h"

/********* Panic Handler *********/

#define MCAUSE_INST_ACCESS_FAULT      1
#define MCAUSE_ILLEGAL_INSTRUCTION    2
#define MCAUSE_LOAD_ACCESS_FAULT      5
#define MCAUSE_STORE_ACCESS_FAULT     7

/********* HP_CPU - LP_CPU messaging *********/

#define SEND_MSG(msg)       REG_WRITE(LP_AON_STORE0_REG, msg)
#define RECV_MSG()          REG_READ(LP_AON_STORE0_REG)

#define SEND_ADDR(addr)     REG_WRITE(LP_AON_STORE7_REG, addr)
#define RECV_ADDR()         REG_READ(LP_AON_STORE7_REG)

#define SEND_SIZE(size)     REG_WRITE(LP_AON_STORE8_REG, size)
#define RECV_SIZE()         REG_READ(LP_AON_STORE8_REG)

#define SEND_EXCP(val)      REG_WRITE(LP_AON_STORE9_REG, val)
#define RECV_EXCP()         REG_READ(LP_AON_STORE9_REG)

#define MSG_SLAVE_READ      0x11221122
#define MSG_SLAVE_WRITE     0x33443344
#define MSG_SLAVE_EXEC      0x55665566
#define MSG_SLAVE_CLEAR     0x77887788

#if SOC_RTC_MEM_SUPPORTED
#define ADJUST_LP_MEM_ADDR(addr)                              \
    do {                                                      \
        if (((addr) >= (SOC_RTC_IRAM_LOW + 0x20000000))       \
            && ((addr) < (SOC_RTC_DRAM_HIGH + 0x20000000))) { \
            (addr) -= 0x20000000;                             \
        }                                                     \
    } while (0)
#endif

typedef struct {
    apm_master_id_t master_id;
    apm_ctrl_module_t ctrl_mod;
    const uint32_t *test_addr;
    uint32_t test_addr_num;
    uint32_t test_addr_resv_mask;
} test_sys_apm_periph_cfg_t;

typedef struct {
    apm_master_id_t master_id;
    apm_ctrl_module_t ctrl_mod;
    apm_ctrl_access_path_t path;
    uint32_t mem_start_addr;
    uint32_t mem_end_addr;
    uint32_t regn_count;
    uint32_t regn_sz;
} test_sys_apm_mem_cfg_t;

typedef struct {
    apm_master_id_t master_id;
    apm_tee_ctrl_module_t ctrl_mod;
    const uint32_t *test_reg;
    uint32_t test_reg_num;
    uint64_t test_reg_resv_mask;
} test_peri_apm_periph_cfg_t;

/* Utility functions */
void set_test_vector_table(void);
void restore_default_vector_table(void);
void test_m2u_switch(void);
void test_u2m_switch(void);

void apm_hal_enable_region_filter_all(apm_ctrl_module_t ctrl_mod, bool enable);
void test_apm_ctrl_reset_all(void);
void test_apm_ctrl_enable_intr(apm_ctrl_module_t ctrl_mod, apm_ctrl_access_path_t path);

void test_boot_lp_cpu(void);
void test_stop_lp_cpu(void);
void test_reset_lp_cpu(void);
void test_switch_lp_mem_speed(bool high_speed);

void test_gdma_init(void);
void test_gdma_deinit(void);
void test_gdma_m2m_transfer(uint8_t *src, uint8_t *dest, size_t size);
void test_gdma_wait_done(void);

void test_delay_ms(uint32_t ms);
