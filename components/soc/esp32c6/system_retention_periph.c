/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/regdma.h"
#include "soc/system_periph_retention.h"
#include "soc/timer_periph.h"
#include "soc/uart_reg.h"
#include "soc/systimer_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/hp_system_reg.h"
#include "soc/tee_reg.h"
#include "soc/hp_apm_reg.h"
#include "soc/gpio_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/interrupt_matrix_reg.h"

/* Interrupt Matrix Registers Context */
#define N_REGS_INTR_MATRIX()    (((INTMTX_CORE0_CLOCK_GATE_REG - DR_REG_INTERRUPT_MATRIX_BASE) / 4) + 1)
const regdma_entries_config_t intr_matrix_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0), DR_REG_INTERRUPT_MATRIX_BASE, DR_REG_INTERRUPT_MATRIX_BASE, N_REGS_INTR_MATRIX(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* intr matrix */
};
_Static_assert(ARRAY_SIZE(intr_matrix_regs_retention) == INT_MTX_RETENTION_LINK_LEN, "Inconsistent INT_MTX retention link length definitions");

/* HP System Registers Context */
#define N_REGS_HP_SYSTEM()      (((HP_SYSTEM_MEM_TEST_CONF_REG - DR_REG_HP_SYSTEM_BASE) / 4) + 1)
const regdma_entries_config_t hp_system_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_HPSYS_LINK(0), DR_REG_HP_SYSTEM_BASE, DR_REG_HP_SYSTEM_BASE, N_REGS_HP_SYSTEM(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* hp system */
};
_Static_assert(ARRAY_SIZE(hp_system_regs_retention) == HP_SYSTEM_RETENTION_LINK_LEN, "Inconsistent HP_SYSTEM retention link length definitions");

/* TEE/APM Registers Context */
#define N_REGS_TEE()    (((TEE_CLOCK_GATE_REG - DR_REG_TEE_BASE) / 4) + 1)
#define N_REGS_APM()    (((HP_APM_CLOCK_GATE_REG - DR_REG_HP_APM_BASE) / 4) + 1)
const regdma_entries_config_t tee_apm_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0), DR_REG_HP_APM_BASE, DR_REG_HP_APM_BASE, N_REGS_APM(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* apm */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(1), DR_REG_TEE_BASE,    DR_REG_TEE_BASE,    N_REGS_TEE(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* tee */
};
const regdma_entries_config_t tee_apm_highpri_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TEEAPM_LINK(2), TEE_M4_MODE_CTRL_REG, 0x0, 0xffffffff, 1, 0), .owner = ENTRY(2) }
};
_Static_assert((ARRAY_SIZE(tee_apm_regs_retention) == TEE_APM_RETENTION_LINK_LEN) && (ARRAY_SIZE(tee_apm_highpri_regs_retention) == TEE_APM_HIGH_PRI_RETENTION_LINK_LEN), "Inconsistent TEE_APM retention link length definitions");

/* UART0 Registers Context */
#define N_REGS_UART()   (((UART_ID_REG(0) - UART_INT_RAW_REG(0)) / 4) + 1)
const regdma_entries_config_t uart_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_UART_LINK(0x00), UART_INT_RAW_REG(0),    UART_INT_RAW_REG(0), N_REGS_UART(),  0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* uart */
    /* Note: uart register should set update reg to make the configuration take effect */
    [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_UART_LINK(0x01), UART_REG_UPDATE_REG(0), UART_REG_UPDATE,  UART_REG_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [2] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_UART_LINK(0x02), UART_REG_UPDATE_REG(0), 0x0,              UART_REG_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) }
};
_Static_assert(ARRAY_SIZE(uart_regs_retention) == UART_RETENTION_LINK_LEN, "Inconsistent UART retention link length definitions");

/* Timergroup Registers Context */
#define N_REGS_TG()     (((TIMG_REGCLK_REG(0) - REG_TIMG_BASE(0)) / 4) + 1)
const regdma_entries_config_t tg_regs_retention[] = {
    /*Timer group0 backup. T0_wdt should get of write project firstly. wdt used by RTOS.*/
    [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x00), TIMG_WDTWPROTECT_REG(0), TIMG_WDT_WKEY_VALUE,     TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) }, /* TG0 */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TIMG_LINK(0x01), REG_TIMG_BASE(0),        REG_TIMG_BASE(0),        N_REGS_TG(),               0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [2] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x02), TIMG_WDTWPROTECT_REG(0), TIMG_WDT_WKEY_VALUE,     TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x03), TIMG_WDTCONFIG0_REG(0),  TIMG_WDT_CONF_UPDATE_EN, TIMG_WDT_CONF_UPDATE_EN_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [4] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x04), TIMG_T0UPDATE_REG(0),    TIMG_T0_UPDATE,          TIMG_T0_UPDATE_M,          0, 1), .owner = ENTRY(0) | ENTRY(2) },
    [5] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_TIMG_LINK(0x05), TIMG_T0UPDATE_REG(0),    0x0,                     TIMG_T0_UPDATE_M,          0, 1), .owner = ENTRY(0) | ENTRY(2) },
    [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TIMG_LINK(0x06), TIMG_T0LO_REG(0),        TIMG_T0LOADLO_REG(0),    2,                         0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [7] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x07), TIMG_T0LOAD_REG(0),      0x1,                     TIMG_T0_LOAD_M,            1, 0), .owner = ENTRY(0) | ENTRY(2) }
};
_Static_assert(ARRAY_SIZE(tg_regs_retention) == TIMG_RETENTION_LINK_LEN, "Inconsistent Timergroup retention link length definitions");

/* IO MUX Registers Context */
#define N_REGS_IOMUX_0()    (((PERIPHS_IO_MUX_SPID_U - REG_IO_MUX_BASE) / 4) + 1)
#define N_REGS_IOMUX_1()    (((GPIO_FUNC34_OUT_SEL_CFG_REG - GPIO_FUNC0_OUT_SEL_CFG_REG) / 4) + 1)
#define N_REGS_IOMUX_2()    (((GPIO_FUNC124_IN_SEL_CFG_REG - GPIO_STATUS_NEXT_REG) / 4) + 1)
#define N_REGS_IOMUX_3()    (((GPIO_PIN34_REG - DR_REG_GPIO_BASE) / 4) + 1)

const regdma_entries_config_t iomux_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x00), REG_IO_MUX_BASE,            REG_IO_MUX_BASE,            N_REGS_IOMUX_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* io_mux */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x01), GPIO_FUNC0_OUT_SEL_CFG_REG, GPIO_FUNC0_OUT_SEL_CFG_REG, N_REGS_IOMUX_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x02), GPIO_STATUS_NEXT_REG,       GPIO_STATUS_NEXT_REG,       N_REGS_IOMUX_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x03), DR_REG_GPIO_BASE,           DR_REG_GPIO_BASE,           N_REGS_IOMUX_3(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }
};
_Static_assert(ARRAY_SIZE(iomux_regs_retention) == IOMUX_RETENTION_LINK_LEN, "Inconsistent IOMUX retention link length definitions");

/* Mememory SPI Registers Context */
#define N_REGS_SPI1_MEM_0()     (((SPI_MEM_SPI_SMEM_DDR_REG(1) - REG_SPI_MEM_BASE(1)) / 4) + 1)
#define N_REGS_SPI1_MEM_1()     (((SPI_MEM_SPI_SMEM_AC_REG(1) - SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(1)) / 4) + 1)
#define N_REGS_SPI1_MEM_2()     (1)
#define N_REGS_SPI1_MEM_3()     (((SPI_MEM_DATE_REG(1) - SPI_MEM_MMU_POWER_CTRL_REG(1)) / 4) + 1)
#define N_REGS_SPI0_MEM_0()     (((SPI_MEM_SPI_SMEM_DDR_REG(0) - REG_SPI_MEM_BASE(0)) / 4) + 1)
#define N_REGS_SPI0_MEM_1()     (((SPI_MEM_SPI_SMEM_AC_REG(0) - SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(0)) / 4) + 1)
#define N_REGS_SPI0_MEM_2()     (1)
#define N_REGS_SPI0_MEM_3()     (((SPI_MEM_DATE_REG(0) - SPI_MEM_MMU_POWER_CTRL_REG(0)) / 4) + 1)
const regdma_entries_config_t spimem_regs_retention[] = {
    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x00), REG_SPI_MEM_BASE(1),               REG_SPI_MEM_BASE(1),               N_REGS_SPI1_MEM_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* spi1_mem */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x01), SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(1), SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(1), N_REGS_SPI1_MEM_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x02), SPI_MEM_CLOCK_GATE_REG(1),         SPI_MEM_CLOCK_GATE_REG(1),         N_REGS_SPI1_MEM_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x03), SPI_MEM_MMU_POWER_CTRL_REG(1),     SPI_MEM_MMU_POWER_CTRL_REG(1),     N_REGS_SPI1_MEM_3(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },

    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x04), REG_SPI_MEM_BASE(0),               REG_SPI_MEM_BASE(0),               N_REGS_SPI0_MEM_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* spi0_mem */
    [5] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x05), SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(0), SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(0), N_REGS_SPI0_MEM_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x06), SPI_MEM_CLOCK_GATE_REG(0),         SPI_MEM_CLOCK_GATE_REG(0),         N_REGS_SPI0_MEM_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [7] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x07), SPI_MEM_MMU_POWER_CTRL_REG(0),     SPI_MEM_MMU_POWER_CTRL_REG(0),     N_REGS_SPI0_MEM_3(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }
};
_Static_assert(ARRAY_SIZE(spimem_regs_retention) == SPIMEM_RETENTION_LINK_LEN, "Inconsistent SPI Mem retention link length definitions");

/* Systimer Registers Context */
#define N_REGS_SYSTIMER_0()     (((SYSTIMER_TARGET2_CONF_REG - SYSTIMER_TARGET0_HI_REG) / 4) + 1)
const regdma_entries_config_t systimer_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x00), SYSTIMER_UNIT0_OP_REG,       SYSTIMER_TIMER_UNIT0_UPDATE_M,    SYSTIMER_TIMER_UNIT0_UPDATE_M,    0, 1), .owner = ENTRY(0) | ENTRY(2) }, /* Systimer */
    [1]  = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_SYSTIMER_LINK(0x01), SYSTIMER_UNIT0_OP_REG,       SYSTIMER_TIMER_UNIT0_VALUE_VALID, SYSTIMER_TIMER_UNIT0_VALUE_VALID, 0, 1), .owner = ENTRY(0) | ENTRY(2) },
    [2]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x02), SYSTIMER_UNIT0_VALUE_HI_REG, SYSTIMER_UNIT0_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [3]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x03), SYSTIMER_UNIT0_LOAD_REG,     SYSTIMER_TIMER_UNIT0_LOAD_M,      SYSTIMER_TIMER_UNIT0_LOAD_M,      1, 0), .owner = ENTRY(0) | ENTRY(2) },

    [4]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x04), SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_UPDATE_M,    SYSTIMER_TIMER_UNIT1_UPDATE_M,    0, 1), .owner = ENTRY(0) | ENTRY(2) },
    [5]  = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_SYSTIMER_LINK(0x05), SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_VALUE_VALID, SYSTIMER_TIMER_UNIT1_VALUE_VALID, 0, 1), .owner = ENTRY(0) | ENTRY(2) },
    [6]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x06), SYSTIMER_UNIT1_VALUE_HI_REG, SYSTIMER_UNIT1_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) | ENTRY(2) },
    [7]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x07), SYSTIMER_UNIT1_LOAD_REG,     SYSTIMER_TIMER_UNIT1_LOAD_M,      SYSTIMER_TIMER_UNIT1_LOAD_M,      1, 0), .owner = ENTRY(0) | ENTRY(2) },

    [8]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x08), SYSTIMER_TARGET0_HI_REG,     SYSTIMER_TARGET0_HI_REG,          N_REGS_SYSTIMER_0(),              0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* Systimer target value & period */

    [9]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x09), SYSTIMER_COMP0_LOAD_REG,     SYSTIMER_TIMER_COMP0_LOAD,        SYSTIMER_TIMER_COMP0_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [10] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0a), SYSTIMER_COMP1_LOAD_REG,     SYSTIMER_TIMER_COMP1_LOAD,        SYSTIMER_TIMER_COMP1_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [11] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0b), SYSTIMER_COMP2_LOAD_REG,     SYSTIMER_TIMER_COMP2_LOAD,        SYSTIMER_TIMER_COMP2_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },

    [12] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0c), SYSTIMER_TARGET0_CONF_REG,   0,                                SYSTIMER_TARGET0_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [13] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0d), SYSTIMER_TARGET0_CONF_REG,   SYSTIMER_TARGET0_PERIOD_MODE_M,   SYSTIMER_TARGET0_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },

    [14] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0e), SYSTIMER_TARGET1_CONF_REG,   0,                                SYSTIMER_TARGET1_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },
    [15] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0f), SYSTIMER_TARGET1_CONF_REG,   SYSTIMER_TARGET1_PERIOD_MODE_M,   SYSTIMER_TARGET1_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },

    [16] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x10), SYSTIMER_TARGET2_CONF_REG,   0,                                SYSTIMER_TARGET2_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },

    [17] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x11), SYSTIMER_CONF_REG,           SYSTIMER_CONF_REG,                1,                                0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* Systimer work enable */
    [18] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x12), SYSTIMER_INT_ENA_REG,        SYSTIMER_INT_ENA_REG,             1,                                0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* Systimer intr enable */
};
_Static_assert(ARRAY_SIZE(systimer_regs_retention) == SYSTIMER_RETENTION_LINK_LEN, "Inconsistent Systimer retention link length definitions");
