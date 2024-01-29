/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/cache_reg.h"
#include "soc/gpio_reg.h"
#include "soc/hp_system_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/interrupt_core0_reg.h"
#include "soc/interrupt_core1_reg.h"
#include "soc/pau_reg.h"
#include "soc/regdma.h"
#include "soc/system_periph_retention.h"
#include "soc/spi_mem_reg.h"
#include "soc/systimer_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/timer_periph.h"
#include "soc/uart_reg.h"

/* Interrupt Matrix Registers Context */
#define N_REGS_INTR_CORE0()    (((INTERRUPT_CORE0_CLOCK_GATE_REG - DR_REG_INTERRUPT_CORE0_BASE) / 4) + 1)
#define N_REGS_INTR_CORE1()    (((INTERRUPT_CORE1_CLOCK_GATE_REG - DR_REG_INTERRUPT_CORE1_BASE) / 4) + 1)
const regdma_entries_config_t intr_matrix_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0x00), DR_REG_INTERRUPT_CORE0_BASE, DR_REG_INTERRUPT_CORE0_BASE, N_REGS_INTR_CORE0(), 0, 0), .owner = ENTRY(0) },  /* intr matrix */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0x01), DR_REG_INTERRUPT_CORE1_BASE, DR_REG_INTERRUPT_CORE1_BASE, N_REGS_INTR_CORE1(), 0, 0), .owner = ENTRY(0) }   /* intr matrix */
};
_Static_assert(ARRAY_SIZE(intr_matrix_regs_retention) == INT_MTX_RETENTION_LINK_LEN, "Inconsistent INT_MTX retention link length definitions");

/* L2 Cache Registers Context */
#define N_REGS_L2_CACHE()      (((CACHE_L2_CACHE_DATA_MEM_POWER_CTRL_REG - CACHE_L2_CACHE_CTRL_REG) / 4) + 1)
const regdma_entries_config_t l2_cache_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_HPSYS_LINK(0), CACHE_L2_CACHE_CTRL_REG, CACHE_L2_CACHE_CTRL_REG, N_REGS_L2_CACHE(), 0, 0), .owner = ENTRY(0) }  /* hp system */
};
_Static_assert(ARRAY_SIZE(l2_cache_regs_retention) == HP_SYSTEM_RETENTION_LINK_LEN, "Inconsistent L2 CACHE retention link length definitions");

/* HP System Registers Context */
#define N_REGS_HP_SYSTEM()      (((HP_SYSTEM_AHB2AXI_BRESP_ERR_INT_ENA_REG - DR_REG_HP_SYS_BASE) / 4) + 1)
const regdma_entries_config_t hp_system_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_HPSYS_LINK(0), DR_REG_HP_SYS_BASE, DR_REG_HP_SYS_BASE, N_REGS_HP_SYSTEM(), 0, 0), .owner = ENTRY(0) }  /* hp system */
};
_Static_assert(ARRAY_SIZE(hp_system_regs_retention) == HP_SYSTEM_RETENTION_LINK_LEN, "Inconsistent HP_SYSTEM retention link length definitions");

/* UART0 Registers Context */
#define N_REGS_UART()   (((UART_CLK_CONF_REG(0) - REG_UART_BASE(0)) / 4) + 1)
const regdma_entries_config_t uart_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_UART_LINK(0x00), REG_UART_BASE(0),       REG_UART_BASE(0), N_REGS_UART(),     0, 0), .owner = ENTRY(0) }, /* uart */
    /* Note: uart register should set update reg to make the configuration take effect */
    [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_UART_LINK(0x01), UART_REG_UPDATE_REG(0), UART_REG_UPDATE,  UART_REG_UPDATE_M, 1, 0), .owner = ENTRY(0) },
    [2] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_UART_LINK(0x02), UART_REG_UPDATE_REG(0), 0x0,              UART_REG_UPDATE_M, 1, 0), .owner = ENTRY(0) }
};
_Static_assert(ARRAY_SIZE(uart_regs_retention) == UART_RETENTION_LINK_LEN, "Inconsistent UART retention link length definitions");

/* Timergroup Registers Context */
#define N_REGS_TG0()     (((TIMG_REGCLK_REG(0) - REG_TIMG_BASE(0)) / 4) + 1)
const regdma_entries_config_t tg_regs_retention[] = {
    /*Timer group0 backup. T0_wdt should get of write project firstly. wdt used by RTOS.*/
    [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x00), TIMG_WDTWPROTECT_REG(0), TIMG_WDT_WKEY_VALUE,     TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) }, /* TG0 */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TIMG_LINK(0x01), REG_TIMG_BASE(0),        REG_TIMG_BASE(0),        N_REGS_TG0(),              0, 0), .owner = ENTRY(0) },
    [2] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x02), TIMG_WDTWPROTECT_REG(0), TIMG_WDT_WKEY_VALUE,     TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) },
    [3] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x03), TIMG_WDTCONFIG0_REG(0),  TIMG_WDT_CONF_UPDATE_EN, TIMG_WDT_CONF_UPDATE_EN_M, 1, 0), .owner = ENTRY(0) },
    [4] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x04), TIMG_T0UPDATE_REG(0),    TIMG_T0_UPDATE,          TIMG_T0_UPDATE_M,          0, 1), .owner = ENTRY(0) },
    [5] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_TIMG_LINK(0x05), TIMG_T0UPDATE_REG(0),    0x0,                     TIMG_T0_UPDATE_M,          0, 1), .owner = ENTRY(0) },
    [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TIMG_LINK(0x06), TIMG_T0LO_REG(0),        TIMG_T0LOADLO_REG(0),    2,                         0, 0), .owner = ENTRY(0) },
    [7] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x07), TIMG_T0LOAD_REG(0),      0x1,                     TIMG_T0_LOAD_M,            1, 0), .owner = ENTRY(0) },
};
_Static_assert(ARRAY_SIZE(tg_regs_retention) == TIMG_RETENTION_LINK_LEN, "Inconsistent Timergroup retention link length definitions");

/* IO MUX Registers Context */
#define N_REGS_IOMUX_0()    (((PERIPHS_IO_MUX_U_PAD_GPIO56 - REG_IO_MUX_BASE) / 4) + 1)
#define N_REGS_IOMUX_1()    (((GPIO_ZERO_DET1_FILTER_CNT_REG - DR_REG_GPIO_BASE) / 4) + 1)
const regdma_entries_config_t iomux_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x00), REG_IO_MUX_BASE,         REG_IO_MUX_BASE,            N_REGS_IOMUX_0(), 0, 0), .owner = ENTRY(0) }, /* io_mux */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x01), DR_REG_GPIO_BASE,        DR_REG_GPIO_BASE,           N_REGS_IOMUX_1(), 0, 0), .owner = ENTRY(0) },
};
_Static_assert(ARRAY_SIZE(iomux_regs_retention) == IOMUX_RETENTION_LINK_LEN, "Inconsistent IOMUX retention link length definitions");

/* Memory SPI Registers Context */
#define N_REGS_SPI1_MEM_0()     (((SPI1_MEM_C_SUS_STATUS_REG - DR_REG_FLASH_SPI1_BASE) / 4) + 1)
#define N_REGS_SPI1_MEM_1()     (((SPI1_MEM_C_DDR_REG - SPI1_MEM_C_INT_ENA_REG) / 4) + 1)
#define N_REGS_SPI1_MEM_2()     (1)
#define N_REGS_SPI1_MEM_3()     (1)
#define N_REGS_SPI0_MEM_0()     ((SPI_MEM_C_SMEM_DDR_REG - DR_REG_FLASH_SPI0_BASE) / 4 + 1)
#define N_REGS_SPI0_MEM_1()     (((SPI_MEM_C_SMEM_AC_REG - SPI_MEM_C_FMEM__PMS0_ATTR_REG) / 4) + 1)
#define N_REGS_SPI0_MEM_2()     (1)
#define N_REGS_SPI0_MEM_3()     (((SPI_MEM_C_DATE_REG - SPI_MEM_C_MMU_POWER_CTRL_REG) / 4) + 1)
const regdma_entries_config_t spimem_regs_retention[] = {
    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x00), DR_REG_FLASH_SPI1_BASE,            DR_REG_FLASH_SPI1_BASE,      N_REGS_SPI1_MEM_0(), 0, 0), .owner = ENTRY(0) }, /* spi1_mem */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x01), SPI1_MEM_C_INT_ENA_REG,            SPI1_MEM_C_INT_ENA_REG,      N_REGS_SPI1_MEM_1(), 0, 0), .owner = ENTRY(0) },
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x02), SPI1_MEM_C_TIMING_CALI_REG,        SPI1_MEM_C_TIMING_CALI_REG,  N_REGS_SPI1_MEM_2(), 0, 0), .owner = ENTRY(0) },
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x03), SPI1_MEM_C_CLOCK_GATE_REG,         SPI1_MEM_C_CLOCK_GATE_REG,   N_REGS_SPI1_MEM_3(), 0, 0), .owner = ENTRY(0) },

    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x04), DR_REG_FLASH_SPI0_BASE,            DR_REG_FLASH_SPI0_BASE,          N_REGS_SPI0_MEM_0(), 0, 0), .owner = ENTRY(0) }, /* spi0_mem */
    [5] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x05), SPI_MEM_C_FMEM__PMS0_ATTR_REG,     SPI_MEM_C_FMEM__PMS0_ATTR_REG,   N_REGS_SPI0_MEM_1(), 0, 0), .owner = ENTRY(0) },
    [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x06), SPI_MEM_C_CLOCK_GATE_REG,          SPI_MEM_C_CLOCK_GATE_REG,        N_REGS_SPI0_MEM_2(), 0, 0), .owner = ENTRY(0) },
    [7] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x07), SPI_MEM_C_MMU_POWER_CTRL_REG,      SPI_MEM_C_MMU_POWER_CTRL_REG,    N_REGS_SPI0_MEM_3(), 0, 0), .owner = ENTRY(0) }
};
_Static_assert(ARRAY_SIZE(spimem_regs_retention) == SPIMEM_RETENTION_LINK_LEN, "Inconsistent SPI Mem retention link length definitions");

/* Systimer Registers Context */
#define N_REGS_SYSTIMER_0()     (((SYSTIMER_TARGET2_CONF_REG - SYSTIMER_TARGET0_HI_REG) / 4) + 1)
const regdma_entries_config_t systimer_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x00), SYSTIMER_UNIT0_OP_REG,       SYSTIMER_TIMER_UNIT0_UPDATE_M,    SYSTIMER_TIMER_UNIT0_UPDATE_M,    0, 1), .owner = ENTRY(0) }, /* Systimer */
    [1]  = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_SYSTIMER_LINK(0x01), SYSTIMER_UNIT0_OP_REG,       SYSTIMER_TIMER_UNIT0_VALUE_VALID, SYSTIMER_TIMER_UNIT0_VALUE_VALID, 0, 1), .owner = ENTRY(0) },
    [2]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x02), SYSTIMER_UNIT0_VALUE_HI_REG, SYSTIMER_UNIT0_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) },
    [3]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x03), SYSTIMER_UNIT0_LOAD_REG,     SYSTIMER_TIMER_UNIT0_LOAD_M,      SYSTIMER_TIMER_UNIT0_LOAD_M,      1, 0), .owner = ENTRY(0) },
    [4]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x04), SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_UPDATE_M,    SYSTIMER_TIMER_UNIT1_UPDATE_M,    0, 1), .owner = ENTRY(0) },
    [5]  = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_SYSTIMER_LINK(0x05), SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_VALUE_VALID, SYSTIMER_TIMER_UNIT1_VALUE_VALID, 0, 1), .owner = ENTRY(0) },
    [6]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x06), SYSTIMER_UNIT1_VALUE_HI_REG, SYSTIMER_UNIT1_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) },
    [7]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x07), SYSTIMER_UNIT1_LOAD_REG,     SYSTIMER_TIMER_UNIT1_LOAD_M,      SYSTIMER_TIMER_UNIT1_LOAD_M,      1, 0), .owner = ENTRY(0) },
    [8]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x08), SYSTIMER_TARGET0_HI_REG,     SYSTIMER_TARGET0_HI_REG,          N_REGS_SYSTIMER_0(),              0, 0), .owner = ENTRY(0) }, /* Systimer target value & period */
    [9]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x09), SYSTIMER_COMP0_LOAD_REG,     SYSTIMER_TIMER_COMP0_LOAD,        SYSTIMER_TIMER_COMP0_LOAD,        1, 0), .owner = ENTRY(0) },
    [10] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0a), SYSTIMER_COMP1_LOAD_REG,     SYSTIMER_TIMER_COMP1_LOAD,        SYSTIMER_TIMER_COMP1_LOAD,        1, 0), .owner = ENTRY(0) },
    [11] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0b), SYSTIMER_COMP2_LOAD_REG,     SYSTIMER_TIMER_COMP2_LOAD,        SYSTIMER_TIMER_COMP2_LOAD,        1, 0), .owner = ENTRY(0) },
    [12] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0c), SYSTIMER_TARGET0_CONF_REG,   0,                                SYSTIMER_TARGET0_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [13] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0d), SYSTIMER_TARGET0_CONF_REG,   SYSTIMER_TARGET0_PERIOD_MODE_M,   SYSTIMER_TARGET0_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [14] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0e), SYSTIMER_TARGET1_CONF_REG,   0,                                SYSTIMER_TARGET1_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [15] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0f), SYSTIMER_TARGET1_CONF_REG,   SYSTIMER_TARGET1_PERIOD_MODE_M,   SYSTIMER_TARGET1_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [16] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x10), SYSTIMER_TARGET2_CONF_REG,   0,                                SYSTIMER_TARGET2_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [17] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x11), SYSTIMER_CONF_REG,           SYSTIMER_CONF_REG,                1,                                0, 0), .owner = ENTRY(0) }, /* Systimer work enable */
    [18] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x12), SYSTIMER_INT_ENA_REG,        SYSTIMER_INT_ENA_REG,             1,                                0, 0), .owner = ENTRY(0) }  /* Systimer intr enable */
};
_Static_assert(ARRAY_SIZE(systimer_regs_retention) == SYSTIMER_RETENTION_LINK_LEN, "Inconsistent Systimer retention link length definitions");

/* PAU Registers Context */
#define N_REGS_PAU()    (((PAU_INT_ENA_REG - DR_REG_PAU_BASE) / 4) + 1)
const regdma_entries_config_t pau_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PAU_LINK(0x0),  DR_REG_PAU_BASE,               DR_REG_PAU_BASE,    N_REGS_PAU(),       0, 0), .owner = ENTRY(0) },  /* pau */
};
_Static_assert(ARRAY_SIZE(pau_regs_retention) == HP_SYSTEM_RETENTION_LINK_LEN, "Inconsistent PAU retention link length definitions");
