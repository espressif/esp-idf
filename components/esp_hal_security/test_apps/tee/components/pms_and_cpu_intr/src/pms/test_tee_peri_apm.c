/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "soc/apm_defs.h"
#include "hal/apm_hal.h"
#include "hal/apm_types.h"

#include "esp_cpu.h"
#include "esp_bit_defs.h"
#include "esp_rom_sys.h"

#include "riscv/csr.h"
#include "riscv/rv_utils.h"

#include "test_pms_priv.h"
#include "test_pms_params.h"

#include "unity.h"

#define TEST_VAL              0x0FACADE0

/***************************** PERI_APM *****************************/

static const uint32_t test_peri_apm_hp_peri_reg[] = {
    [APM_TEE_HP_PERIPH_UART1]         = UART_DATE_REG(1),
    [APM_TEE_HP_PERIPH_I2C_EXT0]      = I2C_DATE_REG(0),
    [APM_TEE_HP_PERIPH_I2S]           = I2S_DATE_REG(0),
    [APM_TEE_HP_PERIPH_PWM]           = MCPWM_VERSION_REG(0),
    [APM_TEE_HP_PERIPH_TWAI0]         = TWAIFD_DATE_VER_REG(0),
    [APM_TEE_HP_PERIPH_GDMA]          = AHB_DMA_DATE_REG,
    [APM_TEE_HP_PERIPH_REGDMA]        = PAU_DATE_REG,
    [APM_TEE_HP_PERIPH_INTMTX]        = INTERRUPT_CORE0_INTERRUPT_DATE_REG,
    [APM_TEE_HP_PERIPH_APB_ADC]       = APB_SARADC_CTRL_DATE_REG,
    [APM_TEE_HP_PERIPH_TG0]           = TIMG_NTIMERS_DATE_REG(0),
    [APM_TEE_HP_PERIPH_PCNT]          = PCNT_DATE_REG,
    [APM_TEE_HP_PERIPH_IOMUX]         = IO_MUX_DATE_REG,
    [APM_TEE_HP_PERIPH_SYSTEM_REG]    = HP_SYSTEM_DATE_REG,
    [APM_TEE_HP_PERIPH_PCR_REG]       = PCR_DATE_REG,
    [APM_TEE_HP_PERIPH_MSPI]          = SPI_MEM_DATE_REG(0),
    [APM_TEE_HP_PERIPH_HP_APM]        = HP_APM_DATE_REG,
    [APM_TEE_HP_PERIPH_CPU_APM]       = CPU_APM_DATE_REG,
    [APM_TEE_HP_PERIPH_CRYPTO]        = SHA_DATE_REG,
    [APM_TEE_HP_PERIPH_CACHE_CFG]     = CACHE_DATE_REG,
    [APM_TEE_HP_PERIPH_SPI2]          = SPI_DATE_REG(0),
    [APM_TEE_HP_PERIPH_BIT_SCRAMBLER] = BITSCRAMBLER_VERSION_REG,
    [APM_TEE_HP_PERIPH_KEY_MANAGER]   = KEYMNG_DATE_REG,
    [APM_TEE_HP_PERIPH_SLC_HOST]      = SDIO_SLC_HOST_DATE_REG,
};

static const uint32_t test_peri_apm_lp_peri_reg[] = {
    [APM_TEE_LP_PERIPH_PMU]           = PMU_DATE_REG,
    [APM_TEE_LP_PERIPH_CLKRST]        = LP_CLKRST_DATE_REG,
    [APM_TEE_LP_PERIPH_LP_AON_CTRL]   = LP_AON_DATE_REG,
    [APM_TEE_LP_PERIPH_LP_WDT]        = LP_WDT_DATE_REG,
    [APM_TEE_LP_PERIPH_LP_PERI]       = LPPERI_DATE_REG,
    [APM_TEE_LP_PERIPH_LP_ANA_PERI]   = LP_ANA_DATE_REG,
    [APM_TEE_LP_PERIPH_UART]          = LP_UART_ID_REG,
    [APM_TEE_LP_PERIPH_I2C_ANA_MST]   = LP_I2C_ANA_MST_DATE_REG,
    [APM_TEE_LP_PERIPH_I2C_EXT]       = LP_I2C_DATE_REG,
    [APM_TEE_LP_PERIPH_HUK]           = HUK_DATE_REG,
    [APM_TEE_LP_PERIPH_LP_APM]        = LP_APM_DATE_REG,
};

#define TEST_LP_CPU_HP_PERI_RESV_MASK  (BIT64(APM_TEE_HP_PERIPH_CACHE_CFG) | \
                                        BIT64(APM_TEE_HP_PERIPH_HP_APM)    | \
                                        BIT64(APM_TEE_HP_PERIPH_CPU_APM))

#define TEST_LP_CPU_LP_PERI_RESV_MASK  (BIT64(APM_TEE_LP_PERIPH_PMU)          | \
                                        BIT64(APM_TEE_LP_PERIPH_LP_AON_CTRL)  | \
                                        BIT64(APM_TEE_LP_PERIPH_LP_PERI)      | \
                                        BIT64(APM_TEE_LP_PERIPH_LP_APM))

FORCE_INLINE_ATTR uint32_t reg_read(uint32_t addr)
{
    uint32_t value;
    __asm__ volatile(
        "lw   %0, 0(%1)\n"
        "fence\n"
        "nop\nnop\nnop\nnop\n"
        : "=r"(value)
        : "r"(addr)
        : "memory"
    );
    return value;
}

FORCE_INLINE_ATTR void reg_write(uint32_t addr, uint32_t value)
{
    __asm__ volatile(
        "sw   %1, 0(%0)\n"
        "fence\n"
        "nop\nnop\nnop\nnop\n"
        :
        : "r"(addr), "r"(value)
        : "memory"
    );
}

IRAM_ATTR static void hp_cpu_peri_addr_rw(uint32_t peri_addr, uint32_t attr)
{
    RV_WRITE_CSR(uscratch, 0x00);

    reg_write(peri_addr, TEST_VAL);
    test_delay_ms(10);

    bool can_write = (attr & APM_PERM_W);
    if (!can_write) {
        TEST_ASSERT_EQUAL_HEX32(peri_addr, RV_READ_CSR(uscratch));
    }

    RV_WRITE_CSR(uscratch, 0x00);

    volatile uint32_t val = reg_read(peri_addr);
    (void)val;
    test_delay_ms(10);

    bool can_read = (attr & APM_PERM_R);
    if (!can_read) {
        TEST_ASSERT_EQUAL_HEX32(peri_addr, RV_READ_CSR(uscratch));
    }

    RV_WRITE_CSR(uscratch, 0x00);
}

IRAM_ATTR static void lp_cpu_peri_addr_rw(uint32_t peri_addr, uint32_t attr)
{
    test_reset_lp_cpu();

    SEND_EXCP(0);
    SEND_MSG(MSG_SLAVE_WRITE);
    SEND_ADDR(peri_addr);
    test_delay_ms(10);

    bool can_write = (attr & APM_PERM_W);
    if (!can_write) {
        TEST_ASSERT(RECV_EXCP() == MCAUSE_STORE_ACCESS_FAULT);
        TEST_ASSERT_EQUAL_HEX32(peri_addr, RECV_ADDR());
        esp_rom_printf("[PERI_APM] Store access fault at 0x%08x\n", peri_addr);
    }

    SEND_EXCP(0);
    SEND_MSG(MSG_SLAVE_READ);
    SEND_ADDR(peri_addr);
    test_delay_ms(10);

    bool can_read = (attr & APM_PERM_R);
    if (!can_read) {
        TEST_ASSERT(RECV_EXCP() == MCAUSE_LOAD_ACCESS_FAULT);
        TEST_ASSERT_EQUAL_HEX32(peri_addr, RECV_ADDR());
        esp_rom_printf("[PERI_APM] Load access fault at 0x%08x\n", peri_addr);
    }

    SEND_EXCP(0);
}

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

static void test_cpu_to_peri_access(const test_peri_apm_periph_cfg_t *cfg)
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

        for (uint32_t peri_idx = 0; peri_idx < cfg->test_reg_num; peri_idx++) {
            uint32_t peri_addr = cfg->test_reg[peri_idx];
            if (peri_addr == 0 || (cfg->test_reg_resv_mask & BIT64(peri_idx))) {
                continue;
            }

            uint32_t pms_attr = APM_PERM_W;
            apm_hal_tee_set_peri_access(cfg->ctrl_mod, BIT64(peri_idx), (apm_security_mode_t)mode, APM_PERM_NONE);
            apm_hal_tee_set_peri_access(cfg->ctrl_mod, BIT64(peri_idx), (apm_security_mode_t)mode, pms_attr);

            bool is_switch_req = (cfg->master_id == APM_MASTER_HPCORE && mode != APM_SEC_MODE_TEE);

            if (is_switch_req) {
                switch_priv_mode();
            }
            cpu_peri_rw(peri_addr, pms_attr);
            if (is_switch_req) {
                switch_priv_mode();
            }

            pms_attr = APM_PERM_R;
            apm_hal_tee_set_peri_access(cfg->ctrl_mod, BIT64(peri_idx), (apm_security_mode_t)mode, APM_PERM_NONE);
            apm_hal_tee_set_peri_access(cfg->ctrl_mod, BIT64(peri_idx), (apm_security_mode_t)mode, pms_attr);

            if (is_switch_req) {
                switch_priv_mode();
            }
            cpu_peri_rw(peri_addr, pms_attr);
            if (is_switch_req) {
                switch_priv_mode();
            }

            apm_hal_tee_set_peri_access(cfg->ctrl_mod, BIT64(peri_idx), (apm_security_mode_t)mode, APM_PERM_R | APM_PERM_W);
        }
    }
}

void test_peri_apm_master_hp_cpu_slave_hp_peri(void)
{
    test_apm_ctrl_reset_all();

    test_peri_apm_periph_cfg_t cfg = {
        .master_id = APM_MASTER_HPCORE,
        .ctrl_mod = APM_TEE_CTRL_HP,
        .test_reg = test_peri_apm_hp_peri_reg,
        .test_reg_num = sizeof(test_peri_apm_hp_peri_reg) / sizeof(test_peri_apm_hp_peri_reg[0]),
    };

    set_test_vector_table();
    test_cpu_to_peri_access(&cfg);
    restore_default_vector_table();
}

void test_peri_apm_master_hp_cpu_slave_lp_peri(void)
{
    test_apm_ctrl_reset_all();

    test_peri_apm_periph_cfg_t cfg = {
        .master_id = APM_MASTER_HPCORE,
        .ctrl_mod = APM_TEE_CTRL_LP,
        .test_reg = test_peri_apm_lp_peri_reg,
        .test_reg_num = sizeof(test_peri_apm_lp_peri_reg) / sizeof(test_peri_apm_lp_peri_reg[0]),
    };

    test_boot_lp_cpu();

    set_test_vector_table();
    test_cpu_to_peri_access(&cfg);
    restore_default_vector_table();

    test_stop_lp_cpu();
}

#if CONFIG_ULP_COPROC_ENABLED
void test_peri_apm_master_lp_cpu_slave_hp_peri(void)
{
    test_apm_ctrl_reset_all();

    test_peri_apm_periph_cfg_t cfg = {
        .master_id = APM_MASTER_LPCORE,
        .ctrl_mod = APM_TEE_CTRL_HP,
        .test_reg = test_peri_apm_hp_peri_reg,
        .test_reg_num = sizeof(test_peri_apm_hp_peri_reg) / sizeof(test_peri_apm_hp_peri_reg[0]),
        .test_reg_resv_mask = TEST_LP_CPU_HP_PERI_RESV_MASK,
    };

    test_boot_lp_cpu();

    set_test_vector_table();
    test_cpu_to_peri_access(&cfg);
    restore_default_vector_table();

    test_stop_lp_cpu();
}

void test_peri_apm_master_lp_cpu_slave_lp_peri(void)
{
    test_apm_ctrl_reset_all();

    test_peri_apm_periph_cfg_t cfg = {
        .master_id = APM_MASTER_LPCORE,
        .ctrl_mod = APM_TEE_CTRL_LP,
        .test_reg = test_peri_apm_lp_peri_reg,
        .test_reg_num = sizeof(test_peri_apm_lp_peri_reg) / sizeof(test_peri_apm_lp_peri_reg[0]),
        .test_reg_resv_mask = TEST_LP_CPU_LP_PERI_RESV_MASK,
    };

    test_boot_lp_cpu();

    set_test_vector_table();
    test_cpu_to_peri_access(&cfg);
    restore_default_vector_table();

    test_stop_lp_cpu();
}
#endif
