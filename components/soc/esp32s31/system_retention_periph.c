/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#include "soc/cache_reg.h"
#include "soc/gpio_reg.h"
#include "soc/hp_system_reg.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/iomux_mspi_pin_reg.h"
#include "soc/interrupt_core0_reg.h"
#include "soc/interrupt_core1_reg.h"
#include "soc/pau_reg.h"
#include "soc/regdma.h"
#include "soc/system_periph_retention.h"
#include "soc/spi_mem_reg.h"
#include "soc/spi_mem_s_reg.h"
#include "soc/spi1_mem_s_reg.h"
#include "soc/systimer_reg.h"
#include "soc/timer_group_reg.h"
// #include "soc/timer_periph.h"
#include "soc/uart_reg.h"
#include "esp32s31/rom/cache.h"
#include "soc/hp_apm_reg.h"
#include "soc/cpu_apm_reg.h"
#include "soc/hp_mem_apm_reg.h"
#include "soc/tee_reg.h"
#include "soc/hp_peri0_pms_reg.h"
#include "soc/hp_peri1_pms_reg.h"

/* Interrupt Matrix Registers Context */
#define N_REGS_INTR_CORE0()    (((INTERRUPT_CORE0_SIG_IDX_ASSERT_IN_SEC_REG - DR_REG_INTERRUPT_CORE0_BASE) / 4) + 1)
#define N_REGS_INTR_CORE1()    (((INTERRUPT_CORE1_SIG_IDX_ASSERT_IN_SEC_REG - DR_REG_INTERRUPT_CORE1_BASE) / 4) + 1)
const regdma_entries_config_t intr_matrix_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0x00), DR_REG_INTERRUPT_CORE0_BASE, DR_REG_INTERRUPT_CORE0_BASE, N_REGS_INTR_CORE0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },  /* intr matrix */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0x01), DR_REG_INTERRUPT_CORE1_BASE, DR_REG_INTERRUPT_CORE1_BASE, N_REGS_INTR_CORE1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }   /* intr matrix */
};
_Static_assert(ARRAY_SIZE(intr_matrix_regs_retention) == INT_MTX_RETENTION_LINK_LEN, "Inconsistent INT_MTX retention link length definitions");

#if SOC_APM_SUPPORTED
/* TEE APM Registers Context */
#define N_REGS_TEE_0()    (((TEE_M87_MODE_CTRL_REG                  - TEE_M0_MODE_CTRL_REG)                / 4) + 1)
#define N_REGS_TEE_1()    (((TEE_SUPERVISOR_PRIV_SEL_REG            - TEE_SUPERVISOR_PRIV_SEL_REG)         / 4) + 1)
#define N_REGS_APM_0()    (((HP_APM_FUNC_CTRL_REG                   - HP_APM_REGION_FILTER_EN_REG)         / 4) + 1)
#define N_REGS_APM_1()    (((HP_APM_BUS_ERR_CONF_REG                - HP_APM_INT_EN_REG)                   / 4) + 1)
#define N_REGS_APM_2()    (((CPU_APM_FUNC_CTRL_REG                  - CPU_APM_REGION_FILTER_EN_REG)        / 4) + 1)
#define N_REGS_APM_3()    (((CPU_APM_BUS_ERR_CONF_REG               - CPU_APM_INT_EN_REG)                  / 4) + 1)
#define N_REGS_APM_4()    (((HP_MEM_APM_FUNC_CTRL_REG               - HP_MEM_APM_REGION_FILTER_EN_REG)     / 4) + 1)
#define N_REGS_APM_5()    (((HP_MEM_APM_BUS_ERR_CONF_REG            - HP_MEM_APM_INT_EN_REG)               / 4) + 1)

#define N_REGS_PMS0_0()   (((HP_PERI0_PMS_AXI_PERF_MON_CTRL_REG     - HP_PERI0_PMS_TRACE0_CTRL_REG)        / 4) + 1)
#define N_REGS_PMS0_1()   (((HP_PERI0_PMS_INT_EN_REG                - HP_PERI0_PMS_INT_EN_REG)             / 4) + 1)
#define N_REGS_PMS0_2()   (((HP_PERI0_PMS_CLOCK_GATE_REG            - HP_PERI0_PMS_BUS_ERR_CONF_REG)       / 4) + 1)
#define N_REGS_PMS1_0()   (((HP_PERI1_PMS_HP_PERI1_PMS_CTRL_REG     - HP_PERI1_PMS_HP_USBOTG_PHY_CTRL_REG) / 4) + 1)
#define N_REGS_PMS1_1()   (((HP_PERI1_PMS_INT_EN_REG                - HP_PERI1_PMS_INT_EN_REG)             / 4) + 1)
#define N_REGS_PMS1_2()   (((HP_PERI1_PMS_CLOCK_GATE_REG            - HP_PERI1_PMS_BUS_ERR_CONF_REG)       / 4) + 1)

const regdma_entries_config_t tee_apm_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x00), TEE_M0_MODE_CTRL_REG,                TEE_M0_MODE_CTRL_REG,                  N_REGS_TEE_0(),  0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* tee */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x01), TEE_SUPERVISOR_PRIV_SEL_REG,         TEE_SUPERVISOR_PRIV_SEL_REG,           N_REGS_TEE_1(),  0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x02), HP_APM_REGION_FILTER_EN_REG,         HP_APM_REGION_FILTER_EN_REG,           N_REGS_APM_0(),  0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x03), HP_APM_INT_EN_REG,                   HP_APM_INT_EN_REG,                     N_REGS_APM_1(),  0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x04), CPU_APM_REGION_FILTER_EN_REG,        CPU_APM_REGION_FILTER_EN_REG,          N_REGS_APM_2(),  0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [5] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x05), CPU_APM_INT_EN_REG,                  CPU_APM_INT_EN_REG,                    N_REGS_APM_3(),  0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x06), HP_MEM_APM_REGION_FILTER_EN_REG,     HP_MEM_APM_REGION_FILTER_EN_REG,       N_REGS_APM_4(),  0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [7] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x07), HP_MEM_APM_INT_EN_REG,               HP_MEM_APM_INT_EN_REG,                 N_REGS_APM_5(),  0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [8] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x08), HP_PERI0_PMS_TRACE0_CTRL_REG,        HP_PERI0_PMS_TRACE0_CTRL_REG,          N_REGS_PMS0_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [9] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x09), HP_PERI0_PMS_INT_EN_REG,             HP_PERI0_PMS_INT_EN_REG,               N_REGS_PMS0_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [10] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x0a), HP_PERI0_PMS_BUS_ERR_CONF_REG,       HP_PERI0_PMS_BUS_ERR_CONF_REG,         N_REGS_PMS0_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [11] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x0b), HP_PERI1_PMS_HP_USBOTG_PHY_CTRL_REG, HP_PERI1_PMS_HP_USBOTG_PHY_CTRL_REG,   N_REGS_PMS1_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [12] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x0c), HP_PERI1_PMS_INT_EN_REG,             HP_PERI1_PMS_INT_EN_REG,               N_REGS_PMS1_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [13] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0x0d), HP_PERI1_PMS_BUS_ERR_CONF_REG,       HP_PERI1_PMS_BUS_ERR_CONF_REG,         N_REGS_PMS1_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
};
_Static_assert(ARRAY_SIZE(tee_apm_regs_retention) == TEE_APM_RETENTION_LINK_LEN, "Inconsistent HP_SYSTEM retention link length definitions");
#endif

/* HP System Registers Context */
#define N_REGS_HP_SYSTEM()      (((HP_SYSTEM_GMAC1_PAD_BIST_INT_ENA_REG - DR_REG_HP_SYS_BASE) / 4) + 1)
const regdma_entries_config_t hp_system_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_HPSYS_LINK(0), DR_REG_HP_SYS_BASE, DR_REG_HP_SYS_BASE, N_REGS_HP_SYSTEM(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* hp system */
};
_Static_assert(ARRAY_SIZE(hp_system_regs_retention) == HP_SYSTEM_RETENTION_LINK_LEN, "Inconsistent HP_SYSTEM retention link length definitions");

/* IO MUX Registers Context */
#define N_REGS_IOMUX()      (((IO_MUX_GPIO62_REG - REG_IO_MUX_BASE) / 4) + 1)
#define N_REGS_MSPI_IOMUX() (((IOMUX_MSPI_PIN_PSRAM_CS_PIN0_REG - IOMUX_MSPI_PIN_CLK_EN0_REG) / 4) + 1)
#define N_REGS_GPIO_PINx()  (((GPIO_PIN62_REG - GPIO_PIN0_REG) / 4) + 1)
#define N_REGS_GPIO_FUNC_IN() (((GPIO_FUNC255_IN_SEL_CFG_REG - GPIO_FUNC0_IN_SEL_CFG_REG) / 4) + 1)
#define N_REGS_GPIO_FUNC_OUT() (((GPIO_FUNC62_OUT_SEL_CFG_REG - GPIO_FUNC0_OUT_SEL_CFG_REG) / 4) + 1)

const regdma_entries_config_t iomux_regs_retention[] = {
    /* IO_MUX */
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x00), REG_IO_MUX_BASE,            REG_IO_MUX_BASE,            N_REGS_IOMUX(),         0, 0), .owner = ENTRY(0) | ENTRY(2) },
    /* MSPI IOMUX */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x01), IOMUX_MSPI_PIN_PSRAM_CS_PIN0_REG,  IOMUX_MSPI_PIN_PSRAM_CS_PIN0_REG, N_REGS_MSPI_IOMUX(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    /* GPIO_PIN0_REG ~ GPIO_PIN62_REG*/
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x02), GPIO_PIN0_REG,              GPIO_PIN0_REG,              N_REGS_GPIO_PINx(),     0, 0), .owner = ENTRY(0) | ENTRY(2) },
    /* GPIO_FUNC0_IN_SEL_CFG_REG ~ GPIO_FUNC255_IN_SEL_CFG_REG & GPIO_FUNC0_OUT_SEL_CFG_REG ~ GPIO_FUNC0_IN_SEL_CFG_REG */
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x03), GPIO_FUNC0_IN_SEL_CFG_REG,  GPIO_FUNC0_IN_SEL_CFG_REG,  N_REGS_GPIO_FUNC_IN(),  0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x04), GPIO_FUNC0_OUT_SEL_CFG_REG, GPIO_FUNC0_OUT_SEL_CFG_REG, N_REGS_GPIO_FUNC_OUT(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
};
_Static_assert(ARRAY_SIZE(iomux_regs_retention) == IOMUX_RETENTION_LINK_LEN, "Inconsistent IOMUX retention link length definitions");

/* Memory SPI Registers Context */
#define N_REGS_SPI1_C_MEM_0()     (((SPI1_MEM_C_INT_ENA_REG             - DR_REG_FLASH_SPI1_BASE)       / 4) + 1)
#define N_REGS_SPI1_C_MEM_1()     (((SPI1_MEM_C_TIMING_CALI_REG         - SPI1_MEM_C_TIMING_CALI_REG)   / 4) + 1)
#define N_REGS_SPI0_C_MEM_0()     (((SPI_MEM_C_DDR_REG                  - DR_REG_FLASH_SPI0_BASE)       / 4) + 1)
#define N_REGS_SPI0_C_MEM_1()     (((SPI_MEM_C_DOUT_MODE_REG            - SPI_FMEM_C_PMS0_ATTR_REG)     / 4) + 1)
#define N_REGS_SPI0_C_MEM_2()     (((SPI_MEM_C_NAND_FLASH_SPI_SEQ31_REG - SPI_MEM_C_CLOCK_GATE_REG)     / 4) + 1)
#define N_REGS_SPI0_C_MEM_3()     (((SPI_MEM_C_REGISTERRND_ECO_LOW_REG  - SPI_MEM_C_MMU_POWER_CTRL_REG) / 4) + 1)

const regdma_entries_config_t flash_spimem_regs_retention[] = {
    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x00), DR_REG_FLASH_SPI1_BASE,            DR_REG_FLASH_SPI1_BASE,          N_REGS_SPI1_C_MEM_0(),          0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* spi1_mem */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x01), SPI1_MEM_C_INT_ENA_REG,            SPI1_MEM_C_INT_ENA_REG,          N_REGS_SPI1_C_MEM_1(),          0, 0), .owner = ENTRY(0) | ENTRY(2) },

    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x02), DR_REG_FLASH_SPI0_BASE,            DR_REG_FLASH_SPI0_BASE,          N_REGS_SPI0_C_MEM_0(),          0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* spi0_mem */
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x03), SPI_FMEM_C_PMS0_ATTR_REG,          SPI_FMEM_C_PMS0_ATTR_REG,        N_REGS_SPI0_C_MEM_1(),          0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x04), SPI_MEM_C_CLOCK_GATE_REG,          SPI_MEM_C_CLOCK_GATE_REG,        N_REGS_SPI0_C_MEM_2(),          0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [5] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x05), SPI_MEM_C_MMU_POWER_CTRL_REG,      SPI_MEM_C_MMU_POWER_CTRL_REG,    N_REGS_SPI0_C_MEM_3(),          0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [6] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SPIMEM_LINK(0x06),      SPI_MEM_C_TIMING_CALI_REG,         SPI_MEM_C_TIMING_CALI_UPDATE,    SPI_MEM_C_TIMING_CALI_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
};
_Static_assert(ARRAY_SIZE(flash_spimem_regs_retention) == SPIMEM_FLASH_RETENTION_LINK_LEN, "Inconsistent Flash SPI Mem retention link length definitions");

#if CONFIG_SPIRAM
#define N_REGS_SPI0_S_MEM_0()     (((SPI_MEM_S_INT_ENA_REG      - SPI_MEM_S_CTRL_REG)           / 4) + 1)
#define N_REGS_SPI0_S_MEM_1()     (((SPI_MEM_S_CLOCK_GATE_REG   - SPI_MEM_S_DDR_REG)            / 4) + 1)
#define N_REGS_SPI1_S_MEM_0()     (((SPI1_MEM_S_INT_ENA_REG     - SPI1_MEM_S_CMD_REG)           / 4) + 1)
#define N_REGS_SPI1_S_MEM_1()     (((SPI1_MEM_S_DDR_REG         - SPI1_MEM_S_DDR_REG)           / 4) + 1)
#define N_REGS_SPI1_S_MEM_2()     (((SPI1_MEM_S_TIMING_CALI_REG - SPI1_MEM_S_TIMING_CALI_REG)   / 4) + 1)

const regdma_entries_config_t psram_spimem_regs_retention[SPIMEM_PSRAM_RETENTION_LINK_LEN] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x00),     SPI_MEM_S_CTRL_REG,              SPI_MEM_S_CTRL_REG,              N_REGS_SPI0_S_MEM_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* spi0_mem */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x01),     SPI_MEM_S_DDR_REG,               SPI_MEM_S_DDR_REG,               N_REGS_SPI0_S_MEM_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x02),     SPI1_MEM_S_CMD_REG,              SPI1_MEM_S_CMD_REG,              N_REGS_SPI1_S_MEM_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x03),     SPI1_MEM_S_DDR_REG,              SPI1_MEM_S_DDR_REG,              N_REGS_SPI1_S_MEM_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x04),     SPI1_MEM_S_TIMING_CALI_REG,      SPI1_MEM_S_TIMING_CALI_REG,      N_REGS_SPI1_S_MEM_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
};

_Static_assert(ARRAY_SIZE(psram_spimem_regs_retention) == SPIMEM_PSRAM_RETENTION_LINK_LEN, "Inconsistent PSRAM SPI Mem retention link length definitions");
#endif

/* Systimer Registers Context */
#define N_REGS_SYSTIMER_0()     (((SYSTIMER_TARGET2_CONF_REG - SYSTIMER_TARGET0_HI_REG) / 4) + 1)
const regdma_entries_config_t systimer_regs_retention[] = {
    // cnt value
    [0]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x00),       SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_UPDATE_M,    SYSTIMER_TIMER_UNIT1_UPDATE_M,    0, 1), .owner = ENTRY(0) | ENTRY(2) },
    [1]  = { .config = REGDMA_LINK_WAIT_INIT(REGDMA_SYSTIMER_LINK(0x01),        SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_VALUE_VALID, SYSTIMER_TIMER_UNIT1_VALUE_VALID, 0, 1), .owner = ENTRY(0) | ENTRY(2) },
    [2]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x02),  SYSTIMER_UNIT1_VALUE_HI_REG, SYSTIMER_UNIT1_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x03),       SYSTIMER_UNIT1_LOAD_REG,     SYSTIMER_TIMER_UNIT1_LOAD_M,      SYSTIMER_TIMER_UNIT1_LOAD_M,      1, 0), .owner = ENTRY(0) | ENTRY(2) },

    // target value
    [4]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x04),  SYSTIMER_TARGET0_HI_REG,     SYSTIMER_TARGET0_HI_REG,          N_REGS_SYSTIMER_0(),              0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* Systimer target value & period */
    [5]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x05),       SYSTIMER_COMP0_LOAD_REG,     SYSTIMER_TIMER_COMP0_LOAD,        SYSTIMER_TIMER_COMP0_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [6]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x06),       SYSTIMER_COMP1_LOAD_REG,     SYSTIMER_TIMER_COMP1_LOAD,        SYSTIMER_TIMER_COMP1_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [7]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x07),       SYSTIMER_COMP2_LOAD_REG,     SYSTIMER_TIMER_COMP2_LOAD,        SYSTIMER_TIMER_COMP2_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },

    // conf & int value
    [8]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x08),  SYSTIMER_CONF_REG,           SYSTIMER_CONF_REG,                1,                                0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* Systimer work enable */
    [9]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x09),  SYSTIMER_INT_ENA_REG,        SYSTIMER_INT_ENA_REG,             1,                                0, 0), .owner = ENTRY(0) | ENTRY(2) },  /* Systimer intr enable */
};
_Static_assert(ARRAY_SIZE(systimer_regs_retention) == SYSTIMER_RETENTION_LINK_LEN, "Inconsistent Systimer retention link length definitions");
