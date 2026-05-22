/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/apm_defs.h"

#include "hal/apm_hal.h"
#include "hal/apm_ll.h"
#include "hal/apm_types.h"

#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"

#include "riscv/csr.h"
#include "riscv/rv_utils.h"

#include "test_pms_priv.h"
#include "test_pms_params.h"

#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"

#include "unity.h"

#define TEST_VAL              0x0FACADE0

extern volatile bool pms_lp_excp_flag;
extern volatile uint32_t pms_lp_excp_addr;
extern volatile bool pms_dma_excp_flag;

/***************************** HP/HP2LP PERI_PMS & LP/LP2HP PERI_PMS *****************************/

typedef enum {
    HP_PERI0,
    HP_PERI1,
    CPU_PERI,
    LP_PERI
} test_pms_peri_grp_t;

typedef struct {
    apm_master_id_t master_id;
    apm_ctrl_module_t ctrl_mod;
    const uint32_t *test_reg;
    uint32_t test_reg_num;
    test_pms_peri_grp_t test_peri_grp;
} test_pms_periph_cfg_t;

static const uint32_t test_cpu_peri_reg[] = {
    [APM_SLAVE_TRACE0]    = TRACE_DATE_REG(0),
    [APM_SLAVE_TRACE1]    = TRACE_DATE_REG(1),
    [APM_SLAVE_CACHE]     = CACHE_DATE_REG,
};

static const uint32_t test_hp_peri0_reg[] = {
    [APM_SLAVE_HP_CRYPTO]          = SHA_DATE_REG,
    [APM_SLAVE_HP_RMT]             = RMT_DATE_REG,
    [APM_SLAVE_HP_BITSCRAMBLER]    = BITSCRAMBLER_VERSION_REG,
};

static const uint32_t test_hp_peri1_reg[] = {
    [APM_SLAVE_HP_TG0]       = TIMG_NTIMERS_DATE_REG(0),
    [APM_SLAVE_HP_I2C0]      = I2C_DATE_REG(0),
    [APM_SLAVE_HP_I2S0]      = I2S_DATE_REG(0),
    [APM_SLAVE_HP_PCNT]      = PCNT_DATE_REG,
    [APM_SLAVE_HP_UART2]     = UART_DATE_REG(2),
    [APM_SLAVE_HP_PARLIO]    = PARL_IO_VERSION_REG,
    [APM_SLAVE_HP_GPSPI2]    = SPI_DATE_REG(2),
    [APM_SLAVE_HP_LEDC]      = LEDC_DATE_REG,
    [APM_SLAVE_HP_ADC]       = ADC_CTRL_DATE_REG,
    [APM_SLAVE_HP_GPIO]      = GPIO_DATE_REG,
    [APM_SLAVE_HP_IOMUX]     = IO_MUX_DATE_REG,
    [APM_SLAVE_HP_SYS_REG]   = HP_SYSTEM_VER_DATE_REG,
};

static const uint32_t test_lp_peri_reg[] = {
    [APM_SLAVE_LP_TIMER]      = LP_TIMER_DATE_REG,
    [APM_SLAVE_LP_ANAPERI]    = LP_ANALOG_PERI_DATE_REG,
    [APM_SLAVE_LP_WDT]        = LP_WDT_DATE_REG,
    [APM_SLAVE_LP_UART]       = LP_UART_ID_REG,
    [APM_SLAVE_LP_I2C]        = LP_I2C_DATE_REG,
    [APM_SLAVE_LP_I2CMST]     = LP_I2C_ANA_MST_DATE_REG,
    [APM_SLAVE_LP_IOMUX]      = LP_IOMUX_VER_DATE_REG,
    [APM_SLAVE_LP_HUK]        = HUK_DATE_REG,
    [APM_SLAVE_LP_TRNG]       = RNG_DATE_REG,
};

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

IRAM_ATTR static void hp_cpu_peri_addr_rw(uint32_t peri_addr, bool should_panic)
{
    RV_WRITE_CSR(uscratch, 0x00);

    reg_write(peri_addr, TEST_VAL);
    test_delay_ms(10);

    if (should_panic) {
        TEST_ASSERT_EQUAL_HEX32(peri_addr, RV_READ_CSR(uscratch));
    } else {
        TEST_ASSERT_EQUAL_HEX32(0x00, RV_READ_CSR(uscratch));
    }

    RV_WRITE_CSR(uscratch, 0x00);

    volatile uint32_t val = reg_read(peri_addr);
    (void)val;
    test_delay_ms(10);

    if (should_panic) {
        TEST_ASSERT_EQUAL_HEX32(peri_addr, RV_READ_CSR(uscratch));
    } else {
        TEST_ASSERT_EQUAL_HEX32(0x00, RV_READ_CSR(uscratch));
    }

    RV_WRITE_CSR(uscratch, 0x00);
}

#if CONFIG_ULP_COPROC_ENABLED
IRAM_ATTR static void lp_cpu_peri_addr_rw(uint32_t peri_addr, bool should_panic)
{
    bool is_lp_peri = (peri_addr >= SOC_LP_PERIPH_LOW && peri_addr < SOC_LP_PERIPH_HIGH);

    SEND_MSG(MSG_SLAVE_CLEAR);
    test_delay_ms(5);

    if (is_lp_peri) {
        pms_lp_excp_flag = false;
        pms_lp_excp_addr = UINT32_MAX;
    } else {
        SEND_EXCP(0);
    }
    SEND_ADDR(peri_addr);
    SEND_DATA(TEST_VAL);
    SEND_MSG(MSG_SLAVE_WRITE);
    test_delay_ms(10);

    if (should_panic) {
        if (is_lp_peri) {
            TEST_ASSERT_TRUE(pms_lp_excp_flag);
            TEST_ASSERT_EQUAL_HEX32(peri_addr, pms_lp_excp_addr);
        } else {
            TEST_ASSERT(RECV_EXCP() == MCAUSE_STORE_ACCESS_FAULT);
            TEST_ASSERT_EQUAL_HEX32(peri_addr, RECV_ADDR());
            esp_rom_printf("[PERI_ACC] Store access fault at 0x%08x\n", peri_addr);
        }
    } else {
        if (is_lp_peri) {
            TEST_ASSERT_FALSE(pms_lp_excp_flag);
        } else {
            TEST_ASSERT(RECV_EXCP() == 0);
        }
    }

    SEND_MSG(MSG_SLAVE_CLEAR);
    test_delay_ms(5);

    if (is_lp_peri) {
        pms_lp_excp_flag = false;
        pms_lp_excp_addr = UINT32_MAX;
    } else {
        SEND_EXCP(0);
    }
    SEND_ADDR(peri_addr);
    SEND_MSG(MSG_SLAVE_READ);
    test_delay_ms(10);

    if (should_panic) {
        if (is_lp_peri) {
            TEST_ASSERT_TRUE(pms_lp_excp_flag);
            TEST_ASSERT_EQUAL_HEX32(peri_addr, pms_lp_excp_addr);
        } else {
            TEST_ASSERT(RECV_EXCP() == MCAUSE_LOAD_ACCESS_FAULT);
            TEST_ASSERT_EQUAL_HEX32(peri_addr, RECV_ADDR());
            esp_rom_printf("[PERI_ACC] Load access fault at 0x%08x\n", peri_addr);
        }
    } else {
        if (is_lp_peri) {
            TEST_ASSERT_FALSE(pms_lp_excp_flag);
        } else {
            TEST_ASSERT(RECV_EXCP() == 0);
        }
    }
}
#endif

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

static const apm_hal_pms_regn_cfg_t test_hp_peri_regn_cfg[] = {
    /* cpu_peri */
    APM_HAL_PMS_REGION_ENTRY(0, REG_TRACE_BASE(0),       REG_TRACE_BASE(1)),
    APM_HAL_PMS_REGION_ENTRY(1, DR_REG_CACHE_BASE,       ALIGN_TO_NEXT_4KB(DR_REG_CACHE_BASE)),
    /* hp_peri */
    APM_HAL_PMS_REGION_ENTRY(2, DR_REG_SHA_BASE,         DR_REG_RSA_BASE),
    APM_HAL_PMS_REGION_ENTRY(3, DR_REG_RMT_BASE,         DR_REG_BITSCRAMBLER_BASE),
    APM_HAL_PMS_REGION_ENTRY(4, DR_REG_PCNT_BASE,        DR_REG_UART0_BASE),
    APM_HAL_PMS_REGION_ENTRY(5, DR_REG_PARIO_BASE,       DR_REG_SPI2_BASE),
    APM_HAL_PMS_REGION_ENTRY(6, DR_REG_GPIO_BASE,        DR_REG_IO_MUX_BASE),
    APM_HAL_PMS_REGION_ENTRY(7, DR_REG_IO_MUX_BASE,      DR_REG_SYSTIMER_BASE),
};

static const uint32_t test_hp_peri_regn_addr[] = {
    TRACE_DATE_REG(0), CACHE_DATE_REG, SHA_DATE_REG, RMT_DATE_REG,
    PCNT_DATE_REG, PARL_IO_VERSION_REG, GPIO_DATE_REG, IO_MUX_DATE_REG,
};

static const apm_hal_pms_regn_cfg_t test_lp_peri_regn_cfg[] = {
    /* lp_peri */
    APM_HAL_PMS_REGION_ENTRY(0, DR_REG_LP_TIMER_BASE,       DR_REG_LP_ANALOG_PERI_BASE),
    APM_HAL_PMS_REGION_ENTRY(1, DR_REG_LP_ANALOG_PERI_BASE, DR_REG_LP_HUK_BASE),
    APM_HAL_PMS_REGION_ENTRY(2, DR_REG_LP_HUK_BASE,         DR_REG_PMU_BASE),
    APM_HAL_PMS_REGION_ENTRY(3, DR_REG_LP_WDT_BASE,         DR_REG_LP_MB_BASE),
    APM_HAL_PMS_REGION_ENTRY(4, DR_REG_LP_UART_BASE,        DR_REG_LP_I2C_BASE),
    APM_HAL_PMS_REGION_ENTRY(5, DR_REG_LP_I2C_BASE,         DR_REG_LP_SPI_BASE),
    APM_HAL_PMS_REGION_ENTRY(6, DR_REG_LP_I2C_ANA_MST_BASE,    DR_REG_LP_I2S_BASE),
    APM_HAL_PMS_REGION_ENTRY(7, DR_REG_LP_TRNG_BASE,        DR_REG_LP_ADC_BASE),
};

static const uint32_t test_lp_peri_regn_addr[] = {
    LP_TIMER_DATE_REG, LP_ANALOG_PERI_DATE_REG, HUK_DATE_REG, LP_WDT_DATE_REG,
    LP_UART_ID_REG, LP_I2C_DATE_REG, LP_I2C_ANA_MST_DATE_REG, RNG_DATE_REG,
};

static void test_hp_cpu_to_peri_access(const test_pms_periph_cfg_t *cfg)
{
    apm_hal_pms_hp_peri_cfg_t hp_peri_cfg = {
        .cpu_peri = UINT32_MAX,
        .hp_peri0 = UINT32_MAX,
        .hp_peri1 = UINT64_MAX,
    };
    const uint32_t lp_peri_cfg = UINT32_MAX;

    for (uint32_t mode = APM_SEC_MODE_TEE; mode <= APM_SEC_MODE_REE; mode++) {
        apm_hal_hp_peri_pms_set_hpcpu_access(0, (apm_security_mode_t)mode, &hp_peri_cfg);
        apm_hal_hp2lp_peri_pms_set_hpcpu_access(0, (apm_security_mode_t)mode, lp_peri_cfg);

        for (uint32_t sid = 0; sid < cfg->test_reg_num; sid++) {
            uint32_t peri_addr = cfg->test_reg[sid];
            if (peri_addr == 0) {
                continue;
            }
            bool is_switch_req = (mode == APM_SEC_MODE_REE);

            if (is_switch_req) {
                switch_priv_mode();
            }
            hp_cpu_peri_addr_rw(peri_addr, false);
            if (is_switch_req) {
                switch_priv_mode();
            }

            switch (cfg->ctrl_mod) {
            case APM_CTRL_HP_PERI_PMS:
                switch (cfg->test_peri_grp) {
                case HP_PERI0:
                    hp_peri_cfg.hp_peri0 &= ~BIT(sid);
                    break;
                case HP_PERI1:
                    hp_peri_cfg.hp_peri1 &= ~BIT64(sid);
                    break;
                case CPU_PERI:
                    hp_peri_cfg.cpu_peri &= ~BIT(sid);
                    break;
                default:
                    TEST_ASSERT_MESSAGE(false, "Invalid group");
                    return;
                }
                apm_hal_hp_peri_pms_set_hpcpu_access(0, (apm_security_mode_t)mode, &hp_peri_cfg);
                break;
            case APM_CTRL_HP2LP_PERI_PMS:
                apm_hal_hp2lp_peri_pms_set_hpcpu_access(0, (apm_security_mode_t)mode, lp_peri_cfg & ~BIT(sid));
                break;
            default:
                break;
            }
            memset(&hp_peri_cfg, 0xFF, sizeof(hp_peri_cfg));

            if (is_switch_req) {
                switch_priv_mode();
            }
            hp_cpu_peri_addr_rw(peri_addr, true);
            if (is_switch_req) {
                switch_priv_mode();
            }
        }
    }
}

static void test_hp_cpu_to_peri_access_regn(const test_pms_periph_cfg_t *cfg)
{
    const apm_hal_pms_regn_cfg_t *regn_cfg;
    if (cfg->test_peri_grp == LP_PERI) {
        regn_cfg = test_lp_peri_regn_cfg;
    } else {
        regn_cfg = test_hp_peri_regn_cfg;
    }

    apm_hal_lp_peri_pms_set_region_cfg(cfg->test_reg_num, regn_cfg);

    for (uint32_t mode = APM_SEC_MODE_TEE; mode <= APM_SEC_MODE_REE; mode++) {
        bool is_switch_req = (mode == APM_SEC_MODE_REE);
        for (uint32_t rid = 0; rid < cfg->test_reg_num; rid++) {
            uint32_t peri_addr = cfg->test_reg[rid];

            apm_hal_lp_peri_pms_set_master_region_attr(cfg->master_id, (apm_security_mode_t)mode, UINT32_MAX);

            if (is_switch_req) {
                switch_priv_mode();
            }
            hp_cpu_peri_addr_rw(peri_addr, false);
            if (is_switch_req) {
                switch_priv_mode();
            }

            apm_hal_lp_peri_pms_set_master_region_attr(cfg->master_id, (apm_security_mode_t)mode, UINT32_MAX & ~BIT(rid));

            if (is_switch_req) {
                switch_priv_mode();
            }
            hp_cpu_peri_addr_rw(peri_addr, true);
            if (is_switch_req) {
                switch_priv_mode();
            }
        }
    }
}

#if CONFIG_ULP_COPROC_ENABLED
static void test_lp_cpu_to_peri_access(const test_pms_periph_cfg_t *cfg)
{
    apm_hal_pms_hp_peri_cfg_t hp_peri_cfg = {
        .cpu_peri = UINT32_MAX,
        .hp_peri0 = UINT32_MAX,
        .hp_peri1 = UINT64_MAX,
    };
    const uint32_t lp_peri_cfg = UINT32_MAX;

    for (uint32_t sid = 0; sid < cfg->test_reg_num; sid++) {
        uint32_t peri_addr = cfg->test_reg[sid];
        if (peri_addr == 0) {
            continue;
        }

        apm_hal_lp_peri_pms_set_lpcpu_access(lp_peri_cfg);
        apm_hal_lp2hp_peri_pms_set_lpcpu_access(&hp_peri_cfg);
        lp_cpu_peri_addr_rw(peri_addr, false);

        switch (cfg->ctrl_mod) {
        case APM_CTRL_LP_PERI_PMS:
            apm_hal_lp_peri_pms_set_lpcpu_access(lp_peri_cfg & ~BIT(sid));
            break;
        case APM_CTRL_LP2HP_PERI_PMS:
            switch (cfg->test_peri_grp) {
            case HP_PERI0:
                hp_peri_cfg.hp_peri0 &= ~BIT(sid);
                break;
            case HP_PERI1:
                hp_peri_cfg.hp_peri1 &= ~BIT64(sid);
                break;
            case CPU_PERI:
                hp_peri_cfg.cpu_peri &= ~BIT(sid);
                break;
            default:
                TEST_ASSERT_MESSAGE(false, "Invalid group");
                return;
            }
            apm_hal_lp2hp_peri_pms_set_lpcpu_access(&hp_peri_cfg);
            break;
        default:
            break;
        }
        memset(&hp_peri_cfg, 0xFF, sizeof(hp_peri_cfg));

        lp_cpu_peri_addr_rw(peri_addr, true);
    }
}

static void test_lp_cpu_to_peri_access_regn(const test_pms_periph_cfg_t *cfg)
{
    const apm_hal_pms_regn_cfg_t *regn_cfg;
    if (cfg->test_peri_grp == LP_PERI) {
        regn_cfg = test_lp_peri_regn_cfg;
    } else {
        regn_cfg = test_hp_peri_regn_cfg;
    }

    apm_hal_lp_peri_pms_set_region_cfg(cfg->test_reg_num, regn_cfg);

    for (uint32_t rid = 0; rid < cfg->test_reg_num; rid++) {
        uint32_t peri_addr = cfg->test_reg[rid];

        apm_hal_lp_peri_pms_set_master_region_attr(cfg->master_id, APM_SEC_MODE_TEE, UINT32_MAX);
        lp_cpu_peri_addr_rw(peri_addr, false);

        apm_hal_lp_peri_pms_set_master_region_attr(cfg->master_id, APM_SEC_MODE_TEE, UINT32_MAX & ~BIT(rid));
        lp_cpu_peri_addr_rw(peri_addr, true);
    }
}
#endif

void test_sys_pms_master_hp_cpu_slave_hp_peri(void)
{
    const test_pms_periph_cfg_t cfgs[] = {
        {
            .master_id     = APM_MASTER_HPCPU0,
            .ctrl_mod      = APM_CTRL_HP_PERI_PMS,
            .test_reg      = test_hp_peri0_reg,
            .test_reg_num  = sizeof(test_hp_peri0_reg) / sizeof(test_hp_peri0_reg[0]),
            .test_peri_grp = HP_PERI0,
        },
        {
            .master_id     = APM_MASTER_HPCPU0,
            .ctrl_mod      = APM_CTRL_HP_PERI_PMS,
            .test_reg      = test_hp_peri1_reg,
            .test_reg_num  = sizeof(test_hp_peri1_reg) / sizeof(test_hp_peri1_reg[0]),
            .test_peri_grp = HP_PERI1,
        },
        {
            .master_id    = APM_MASTER_HPCPU0,
            .ctrl_mod     = APM_CTRL_HP_PERI_PMS,
            .test_reg     = test_cpu_peri_reg,
            .test_reg_num = sizeof(test_cpu_peri_reg) / sizeof(test_cpu_peri_reg[0]),
            .test_peri_grp = CPU_PERI,
        },
    };

    test_pms_ctrl_reset_all();
    set_test_vector_table();

    for (int i = 0; i < sizeof(cfgs) / sizeof(cfgs[0]); i++) {
        test_hp_cpu_to_peri_access(&cfgs[i]);
    }

    restore_default_vector_table();
}

void test_sys_pms_master_hp_cpu_slave_lp_peri(void)
{
    const test_pms_periph_cfg_t cfg = {
        .master_id    = APM_MASTER_HPCPU0,
        .ctrl_mod     = APM_CTRL_HP2LP_PERI_PMS,
        .test_reg     = test_lp_peri_reg,
        .test_reg_num = sizeof(test_lp_peri_reg) / sizeof(test_lp_peri_reg[0]),
    };

    test_pms_ctrl_reset_all();
    set_test_vector_table();

    test_hp_cpu_to_peri_access(&cfg);

    restore_default_vector_table();
}

void test_sys_pms_master_hp_cpu_peri_regn_access(void)
{
    const test_pms_periph_cfg_t hp_peri_regn_cfg = {
        .master_id     = APM_MASTER_HPCPU0,
        .test_reg      = test_hp_peri_regn_addr,
        .test_reg_num  = sizeof(test_hp_peri_regn_addr) / sizeof(test_hp_peri_regn_addr[0]),
    };

    const test_pms_periph_cfg_t lp_peri_regn_cfg = {
        .master_id     = APM_MASTER_HPCPU0,
        .test_reg      = test_lp_peri_regn_addr,
        .test_reg_num  = sizeof(test_lp_peri_regn_addr) / sizeof(test_lp_peri_regn_addr[0]),
        .test_peri_grp = LP_PERI,
    };

    test_pms_ctrl_reset_all();
    set_test_vector_table();

    test_hp_cpu_to_peri_access_regn(&hp_peri_regn_cfg);
    test_hp_cpu_to_peri_access_regn(&lp_peri_regn_cfg);

    restore_default_vector_table();
}

#if CONFIG_ULP_COPROC_ENABLED
void test_sys_pms_master_lp_cpu_slave_hp_peri(void)
{
    const test_pms_periph_cfg_t cfgs[] = {
        {
            .master_id     = APM_MASTER_LPCPU,
            .ctrl_mod      = APM_CTRL_LP2HP_PERI_PMS,
            .test_reg      = test_hp_peri0_reg,
            .test_reg_num  = sizeof(test_hp_peri0_reg) / sizeof(test_hp_peri0_reg[0]),
            .test_peri_grp = HP_PERI0,
        },
        {
            .master_id    = APM_MASTER_LPCPU,
            .ctrl_mod     = APM_CTRL_LP2HP_PERI_PMS,
            .test_reg     = test_hp_peri1_reg,
            .test_reg_num = sizeof(test_hp_peri1_reg) / sizeof(test_hp_peri1_reg[0]),
            .test_peri_grp = HP_PERI1,
        },
        {
            .master_id     = APM_MASTER_LPCPU,
            .ctrl_mod      = APM_CTRL_LP2HP_PERI_PMS,
            .test_reg      = test_cpu_peri_reg,
            .test_reg_num  = sizeof(test_cpu_peri_reg) / sizeof(test_cpu_peri_reg[0]),
            .test_peri_grp = CPU_PERI,
        },
    };

    test_pms_ctrl_reset_all();
    test_boot_lp_cpu();
    set_test_vector_table();

    for (int i = 0; i < sizeof(cfgs) / sizeof(cfgs[0]); i++) {
        test_lp_cpu_to_peri_access(&cfgs[i]);
    }

    restore_default_vector_table();
    test_stop_lp_cpu();
}

void test_sys_pms_master_lp_cpu_slave_lp_peri(void)
{
    const test_pms_periph_cfg_t cfg = {
        .master_id    = APM_MASTER_LPCPU,
        .ctrl_mod     = APM_CTRL_LP_PERI_PMS,
        .test_reg     = test_lp_peri_reg,
        .test_reg_num = sizeof(test_lp_peri_reg) / sizeof(test_lp_peri_reg[0]),
    };

    test_pms_ctrl_reset_all();
    test_pms_enable_intr(APM_CTRL_LP_PERI_PMS);
    test_boot_lp_cpu();

    set_test_vector_table();
    test_lp_cpu_to_peri_access(&cfg);
    restore_default_vector_table();

    test_stop_lp_cpu();
}

void test_sys_pms_master_lp_cpu_peri_regn_access(void)
{
    const test_pms_periph_cfg_t hp_peri_regn_cfg = {
        .master_id     = APM_MASTER_LPCPU,
        .test_reg      = test_hp_peri_regn_addr,
        .test_reg_num  = sizeof(test_hp_peri_regn_addr) / sizeof(test_hp_peri_regn_addr[0]),
    };

    const test_pms_periph_cfg_t lp_peri_regn_cfg = {
        .master_id     = APM_MASTER_LPCPU,
        .test_reg      = test_lp_peri_regn_addr,
        .test_reg_num  = sizeof(test_lp_peri_regn_addr) / sizeof(test_lp_peri_regn_addr[0]),
        .test_peri_grp = LP_PERI,
    };

    test_pms_ctrl_reset_all();
    test_pms_enable_intr(APM_CTRL_LP_PERI_PMS);
    test_boot_lp_cpu();

    set_test_vector_table();
    test_lp_cpu_to_peri_access_regn(&hp_peri_regn_cfg);
    test_lp_cpu_to_peri_access_regn(&lp_peri_regn_cfg);
    restore_default_vector_table();

    test_stop_lp_cpu();
}
#endif

/***************************** DMA_PMS  *****************************/

typedef struct {
    apm_master_dma_id_t master_id;
    uint32_t mem_start_addr;
    uint32_t mem_end_addr;
    uint32_t regn_count;
    uint32_t regn_sz;
    bool is_ext_mem;
} test_dma_pms_mem_cfg_t;

#define GDMA_TEST_REGN_RESV_NUM(is_ext_mem) \
    ((is_ext_mem) ? GDMA_EXTMEM_TEST_REGN_RESV_NUM : GDMA_HPMEM_TEST_REGN_RESV_NUM)

static void test_dma_pms_mem_setup(const test_dma_pms_mem_cfg_t *cfg, uint32_t test_start, uint32_t test_end)
{
    apm_hal_pms_regn_cfg_t regn_cfgs[GDMA_HPMEM_TEST_REGN_NUM] = {0};

    regn_cfgs[0] = (apm_hal_pms_regn_cfg_t)APM_HAL_PMS_REGION_ENTRY(0, cfg->mem_start_addr, test_start);
    regn_cfgs[1] = (apm_hal_pms_regn_cfg_t)APM_HAL_PMS_REGION_ENTRY(1, test_end, cfg->mem_end_addr);

    const uint32_t test_regn_start = GDMA_TEST_REGN_RESV_NUM(cfg->is_ext_mem);
    if (cfg->is_ext_mem) {
        regn_cfgs[2] = (apm_hal_pms_regn_cfg_t)APM_HAL_PMS_REGION_ENTRY(2, SOC_IRAM_LOW, SOC_DRAM_HIGH);
    }

    for (uint32_t idx = test_regn_start; idx < cfg->regn_count; idx++) {
        uint32_t regn_start = test_start + (idx - test_regn_start) * cfg->regn_sz;
        regn_cfgs[idx] = (apm_hal_pms_regn_cfg_t)APM_HAL_PMS_REGION_ENTRY(idx, regn_start, regn_start + cfg->regn_sz);
    }

    apm_hal_dma_pms_set_region_cfg(GDMA_HPMEM_TEST_REGN_NUM, regn_cfgs);
}

static void gdma_hpmem_addr_rw(uint8_t *src, uint8_t *dest, size_t size, bool should_panic)
{
    pms_dma_excp_flag = false;
    memset(src, 0xA5, size);
    esp_cache_msync(src, size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_UNALIGNED);

    test_gdma_init();
    test_gdma_m2m_transfer(src, dest, size);

    test_delay_ms(10);
    if (should_panic) {
        TEST_ASSERT(pms_dma_excp_flag);
    } else {
        test_gdma_wait_done();
        esp_cache_msync(dest, size, ESP_CACHE_MSYNC_FLAG_DIR_M2C);
        TEST_ASSERT_FALSE(pms_dma_excp_flag);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(src, dest, size);
    }

    test_gdma_deinit();
    pms_dma_excp_flag = false;
}

static void test_gdma_to_xmem_access(const test_dma_pms_mem_cfg_t *cfg, uint32_t test_start_addr)
{
    uint8_t *test_dma_buf = heap_caps_aligned_alloc(cfg->regn_sz, cfg->regn_sz, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(test_dma_buf);
    memset(test_dma_buf, 0x5A, cfg->regn_sz);

    const uint32_t test_regn_start = GDMA_TEST_REGN_RESV_NUM(cfg->is_ext_mem);
    const uint32_t resv_regn = BIT(test_regn_start) - 1;

    for (uint32_t regn_idx = test_regn_start; regn_idx < cfg->regn_count; regn_idx++) {
        uint8_t *regn_start = (uint8_t *)(test_start_addr + (regn_idx - test_regn_start) * cfg->regn_sz);

        apm_hal_dma_pms_set_master_region_attr(cfg->master_id, resv_regn, resv_regn | BIT(regn_idx));
        gdma_hpmem_addr_rw(regn_start, test_dma_buf, cfg->regn_sz, true);
        gdma_hpmem_addr_rw(test_dma_buf, regn_start, cfg->regn_sz, false);

        apm_hal_dma_pms_set_master_region_attr(cfg->master_id, resv_regn | BIT(regn_idx), resv_regn);
        gdma_hpmem_addr_rw(regn_start, test_dma_buf, cfg->regn_sz, false);
        gdma_hpmem_addr_rw(test_dma_buf, regn_start, cfg->regn_sz, true);

        apm_hal_dma_pms_set_master_region_attr(cfg->master_id, UINT32_MAX, UINT32_MAX);
    }

    free(test_dma_buf);
}

void test_sys_pms_master_gdma_slave_hp_mem(void)
{
    test_pms_ctrl_reset_all();
    test_pms_enable_intr(APM_CTRL_DMA_PMS);

    const uint32_t total_sz = (GDMA_HPMEM_TEST_REGN_NUM - GDMA_HPMEM_TEST_REGN_RESV_NUM) * GDMA_HPMEM_TEST_REGN_SIZE;
    test_dma_pms_mem_cfg_t cfg = {
        .master_id      = APM_MASTER_DMA_AHB_PDMA_DUMMY,
        .mem_start_addr = SOC_IRAM_LOW,
        .mem_end_addr   = SOC_DRAM_HIGH,
        .regn_count     = GDMA_HPMEM_TEST_REGN_NUM,
        .regn_sz        = GDMA_HPMEM_TEST_REGN_SIZE,
    };

    uint8_t *test_hpmem_buf = heap_caps_aligned_alloc(cfg.regn_sz, total_sz, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(test_hpmem_buf);

    uint32_t test_start_addr = (uint32_t)test_hpmem_buf;
    uint32_t test_end_addr   = test_start_addr + total_sz;
    test_dma_pms_mem_setup(&cfg, test_start_addr, test_end_addr);

    test_gdma_to_xmem_access(&cfg, test_start_addr);
    free(test_hpmem_buf);
}

#if CONFIG_SPIRAM
void test_sys_pms_master_gdma_slave_ext_mem(void)
{
    test_pms_ctrl_reset_all();
    test_pms_enable_intr(APM_CTRL_DMA_PMS);

    const uint32_t total_sz = (GDMA_HPMEM_TEST_REGN_NUM - GDMA_EXTMEM_TEST_REGN_RESV_NUM) * GDMA_HPMEM_TEST_REGN_SIZE;
    test_dma_pms_mem_cfg_t cfg = {
        .master_id      = APM_MASTER_DMA_AHB_PDMA_DUMMY,
        .mem_start_addr = SOC_EXTRAM_LOW,
        .mem_end_addr   = SOC_EXTRAM_HIGH,
        .regn_count     = GDMA_HPMEM_TEST_REGN_NUM,
        .regn_sz        = GDMA_HPMEM_TEST_REGN_SIZE,
        .is_ext_mem     = true,
    };

    uint8_t *test_extmem_buf = heap_caps_aligned_alloc(cfg.regn_sz, total_sz, MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(test_extmem_buf);

    uint32_t test_start_addr = (uint32_t)test_extmem_buf;
    uint32_t test_end_addr   = test_start_addr + total_sz;
    test_dma_pms_mem_setup(&cfg, test_start_addr, test_end_addr);

    test_gdma_to_xmem_access(&cfg, test_start_addr);
    free(test_extmem_buf);
}
#endif
