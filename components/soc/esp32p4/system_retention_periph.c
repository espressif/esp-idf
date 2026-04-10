/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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
#include "soc/uart_reg.h"
#include "esp32p4/rom/cache.h"
#include "soc/pvt_reg.h"

/* Interrupt Matrix Registers Context */
#define N_REGS_INTR_CORE0()    (((INTERRUPT_CORE0_CLOCK_GATE_REG - DR_REG_INTERRUPT_CORE0_BASE) / 4) + 1)
#define N_REGS_INTR_CORE1()    (((INTERRUPT_CORE1_CLOCK_GATE_REG - DR_REG_INTERRUPT_CORE1_BASE) / 4) + 1)
const regdma_entries_config_t intr_matrix_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0x00), DR_REG_INTERRUPT_CORE0_BASE, DR_REG_INTERRUPT_CORE0_BASE, N_REGS_INTR_CORE0(), 0, 0), .owner = ENTRY(0) },  /* intr matrix */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0x01), DR_REG_INTERRUPT_CORE1_BASE, DR_REG_INTERRUPT_CORE1_BASE, N_REGS_INTR_CORE1(), 0, 0), .owner = ENTRY(0) }   /* intr matrix */
};
_Static_assert(ARRAY_SIZE(intr_matrix_regs_retention) == INT_MTX_RETENTION_LINK_LEN, "Inconsistent INT_MTX retention link length definitions");

/* L1 Cache Registers Context */
/*  CACHE_L1_ICACHE_CTRL_REG & CACHE_L1_DCACHE_CTRL_REG & CACHE_L1_BYPASS_CACHE_CONF_REG &
    CACHE_L1_CACHE_ACS_FAIL_CTRL_REG & CACHE_L1_CACHE_ACS_FAIL_INT_ENA_REG*/
#define L1_CACHE_RETENTION_REGS_CNT     (5)
#define L1_CACHE_RETENTION_REGS_BASE    (CACHE_L1_ICACHE_CTRL_REG)
static const uint32_t l1_cache_regs_map[4] = {0x7, 0x0, 0xc000000, 0x0};
/* L2 Cache Registers Context */
/*  CACHE_L2_CACHE_CTRL_REG & CACHE_L2_BYPASS_CACHE_CONF_REG &
    CACHE_L2_CACHE_CACHESIZE_CONF_REG & CACHE_L2_CACHE_BLOCKSIZE_CONF_REG &
    CACHE_L2_CACHE_ACS_FAIL_CTRL_REG & CACHE_L2_CACHE_ACS_FAIL_INT_ENA_REG */
#define L2_CACHE_RETENTION_REGS_CNT     (6)
#define L2_CACHE_RETENTION_REGS_BASE    (CACHE_L2_CACHE_CTRL_REG)
#define L1_CACHE_ACS_FAIL_INR_CLR       (CACHE_L1_ICACHE0_FAIL_INT_CLR | CACHE_L1_ICACHE1_FAIL_INT_CLR | CACHE_L1_ICACHE2_FAIL_INT_CLR | CACHE_L1_ICACHE3_FAIL_INT_CLR |CACHE_L1_DCACHE_FAIL_INT_CLR)
#define L1_CACHE_ACS_FAIL_INR_CLR_M     (CACHE_L1_ICACHE0_FAIL_INT_CLR_M | CACHE_L1_ICACHE1_FAIL_INT_CLR_M | CACHE_L1_ICACHE2_FAIL_INT_CLR_M | CACHE_L1_ICACHE3_FAIL_INT_CLR_M |CACHE_L1_DCACHE_FAIL_INT_CLR_M)

static const uint32_t l2_cache_regs_map[4] = {0xc000000f, 0x0, 0x0, 0x0};
const regdma_entries_config_t cache_regs_retention[] = {
    // Clear the cache error status, since the auto clock gating added to the cache after version v3, it may falsely report cache acs fail when the module is reset.
    [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x00), CACHE_L1_CACHE_ACS_FAIL_INT_CLR_REG,  L1_CACHE_ACS_FAIL_INR_CLR,      L1_CACHE_ACS_FAIL_INR_CLR_M,    1, 0), .owner = ENTRY(0) },
    [1] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x01), CACHE_L2_CACHE_ACS_FAIL_INT_CLR_REG,  CACHE_L2_CACHE_FAIL_INT_CLR,    CACHE_L2_CACHE_FAIL_INT_CLR_M,  1, 0), .owner = ENTRY(0) },
    [2] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_CACHE_LINK(0x02), L1_CACHE_RETENTION_REGS_BASE, L1_CACHE_RETENTION_REGS_BASE, \
                                            L1_CACHE_RETENTION_REGS_CNT, 0, 0,              \
                                            l1_cache_regs_map[0], l1_cache_regs_map[1],     \
                                            l1_cache_regs_map[2], l1_cache_regs_map[3]),    \
        .owner = ENTRY(0)
    },
    [3] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_CACHE_LINK(0x03),                                    \
                                            L2_CACHE_RETENTION_REGS_BASE, L2_CACHE_RETENTION_REGS_BASE, \
                                            L2_CACHE_RETENTION_REGS_CNT, 0, 0,                          \
                                            l2_cache_regs_map[0], l2_cache_regs_map[1],                 \
                                            l2_cache_regs_map[2], l2_cache_regs_map[3]),                \
        .owner = ENTRY(0)
    },
    // Invalidate L1 Cache
    [4] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x04), CACHE_SYNC_ADDR_REG, 0,                       CACHE_SYNC_ADDR_M,      1, 0), .owner = ENTRY(0) },
    [5] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x05), CACHE_SYNC_SIZE_REG, 0,                       CACHE_SYNC_SIZE_M,      1, 0), .owner = ENTRY(0) },
    [6] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x06), CACHE_SYNC_MAP_REG,  CACHE_MAP_L1_CACHE_MASK, CACHE_SYNC_MAP_M,       1, 0), .owner = ENTRY(0) },
    [7] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x07), CACHE_SYNC_CTRL_REG, 0,                       CACHE_SYNC_RGID_M,      1, 0), .owner = ENTRY(0) },
    [8] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x08), CACHE_SYNC_CTRL_REG, CACHE_INVALIDATE_ENA,    CACHE_INVALIDATE_ENA_M, 1, 0), .owner = ENTRY(0) },
    [9] = { .config = REGDMA_LINK_WAIT_INIT(REGDMA_CACHE_LINK(0x09),  CACHE_SYNC_CTRL_REG, CACHE_SYNC_DONE,         CACHE_SYNC_DONE_M,      1, 0), .owner = ENTRY(0) },
    // Reset L2 CACHE SYNC
    [10] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x10), CACHE_L2_CACHE_SYNC_RST_CTRL_REG, CACHE_L2_CACHE_SYNC_RST,    CACHE_L2_CACHE_SYNC_RST_M, 1, 0), .owner = ENTRY(0) },
    [11] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_CACHE_LINK(0x11), CACHE_L2_CACHE_SYNC_RST_CTRL_REG, 0,    CACHE_L2_CACHE_SYNC_RST_M, 1, 0), .owner = ENTRY(0) },

};
_Static_assert(ARRAY_SIZE(cache_regs_retention) == CACHE_RETENTION_LINK_LEN, "Inconsistent L2 CACHE retention link length definitions");

/* HP System Registers Context */
#define N_REGS_HP_SYSTEM()      (((HP_SYSTEM_AHB2AXI_BRESP_ERR_INT_ENA_REG - DR_REG_HP_SYS_BASE) / 4) + 1)
const regdma_entries_config_t hp_system_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_HPSYS_LINK(2), DR_REG_HP_SYS_BASE, DR_REG_HP_SYS_BASE, N_REGS_HP_SYSTEM(), 0, 0), .owner = ENTRY(0) }  /* hp system */
};
_Static_assert(ARRAY_SIZE(hp_system_regs_retention) == HP_SYSTEM_RETENTION_LINK_LEN, "Inconsistent HP_SYSTEM retention link length definitions");

/* IO MUX Registers Context */
#define N_REGS_IOMUX()      (((IO_MUX_GPIO54_REG - REG_IO_MUX_BASE) / 4) + 1)
#define N_REGS_MSPI_IOMUX() (((IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG - IOMUX_MSPI_PIN_CLK_EN0_REG) / 4) + 1)
#define N_REGS_GPIO_PINx()  (((GPIO_PIN56_REG - GPIO_PIN0_REG) / 4) + 1)
#define N_REGS_GPIO_FUNCx() (((GPIO_FUNC56_OUT_SEL_CFG_REG - GPIO_FUNC1_IN_SEL_CFG_REG) / 4) + 1)

#define GPIO_RETENTION_REGS_CNT0 6
#define GPIO_RETENTION_REGS_CNT1 9
#define GPIO_RETENTION_REGS_BASE0 (GPIO_OUT_REG)
#define GPIO_RETENTION_REGS_BASE1 (GPIO_CLOCK_GATE_REG)
static const uint32_t gpio_regs_map0[4] = {0x90489, 0x0, 0x0, 0x0};
static const uint32_t gpio_regs_map1[4] = {0x1, 0x6fa000, 0x0, 0x0};

const regdma_entries_config_t iomux_regs_retention[] = {
    /* IO_MUX */
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x00), REG_IO_MUX_BASE,             REG_IO_MUX_BASE,            N_REGS_IOMUX(), 0, 0), .owner = ENTRY(0) },
    /* MSPI IOMUX */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x01), IOMUX_MSPI_PIN_FLASH_CS_PIN0_REG,  IOMUX_MSPI_PIN_FLASH_CS_PIN0_REG, N_REGS_MSPI_IOMUX(), 0, 0), .owner = ENTRY(0) },
    /* GPIO_OUT_REG / GPIO_OUT1_REG / GPIO_ENABLE_REG / GPIO_ENABLE1_REG / GPIO_STATUS_REG / GPIO_STATUS1_REG*/
    [2] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_IOMUX_LINK(0x02), GPIO_RETENTION_REGS_BASE0, GPIO_RETENTION_REGS_BASE0, GPIO_RETENTION_REGS_CNT0, 0, 0, \
                                            gpio_regs_map0[0], gpio_regs_map0[1], gpio_regs_map0[2], gpio_regs_map0[3]), .owner = ENTRY(0)
    },
    /* GPIO_PIN0_REG ~ GPIO_PIN56_REG*/
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x03), GPIO_PIN0_REG,            GPIO_PIN0_REG,           N_REGS_GPIO_PINx(), 0, 0), .owner = ENTRY(0) },
    /* GPIO_FUNC1_IN_SEL_CFG_REG ~ GPIO_FUNC255_IN_SEL_CFG_REG & GPIO_FUNC0_OUT_SEL_CFG_REG ~ GPIO_FUNC56_OUT_SEL_CFG_REG */
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x04), GPIO_FUNC1_IN_SEL_CFG_REG, GPIO_FUNC1_IN_SEL_CFG_REG, N_REGS_GPIO_FUNCx(), 0, 0), .owner = ENTRY(0) },

    [5] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_IOMUX_LINK(0x05), GPIO_RETENTION_REGS_BASE1, GPIO_RETENTION_REGS_BASE1, GPIO_RETENTION_REGS_CNT1, 0, 0, \
                                            gpio_regs_map1[0], gpio_regs_map1[1], gpio_regs_map1[2], gpio_regs_map1[3]), .owner = ENTRY(0)
    },
};
_Static_assert(ARRAY_SIZE(iomux_regs_retention) == IOMUX_RETENTION_LINK_LEN, "Inconsistent IOMUX retention link length definitions");

/* Memory SPI Registers Context */
#define N_REGS_SPI1_C_MEM_0()     (((SPI1_MEM_C_SUS_STATUS_REG - DR_REG_FLASH_SPI1_BASE) / 4) + 1)
#define N_REGS_SPI1_C_MEM_1()     (((SPI1_MEM_C_DDR_REG - SPI1_MEM_C_INT_ENA_REG) / 4) + 1)
#define N_REGS_SPI1_C_MEM_2()     (1)
#define N_REGS_SPI1_C_MEM_3()     (1)
#define N_REGS_SPI0_C_MEM_0()     ((SPI_MEM_C_SMEM_DDR_REG - DR_REG_FLASH_SPI0_BASE) / 4 + 1)
#define N_REGS_SPI0_C_MEM_1()     (((SPI_MEM_C_SMEM_AC_REG - SPI_MEM_C_FMEM__PMS0_ATTR_REG) / 4) + 1)
#define N_REGS_SPI0_C_MEM_2()     (1)
#define N_REGS_SPI0_C_MEM_3()     (((SPI_MEM_C_DPA_CTRL_REG - SPI_MEM_C_MMU_POWER_CTRL_REG) / 4) + 1)
#if (CONFIG_ESP_REV_MIN_FULL == 300) // Workaround for rev3.0 MSPI Power UP issue only.
#define FLASH_SPIMEM_RETENTION_ENTRY     (ENTRY(0) | REGDMA_SW_TRIGGER_ENTRY)
#else
#define FLASH_SPIMEM_RETENTION_ENTRY     ENTRY(0)
#endif
const regdma_entries_config_t flash_spimem_regs_retention[] = {
    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x00), DR_REG_FLASH_SPI1_BASE,            DR_REG_FLASH_SPI1_BASE,      N_REGS_SPI1_C_MEM_0(), 0, 0), .owner = FLASH_SPIMEM_RETENTION_ENTRY }, /* spi1_mem */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x01), SPI1_MEM_C_INT_ENA_REG,            SPI1_MEM_C_INT_ENA_REG,      N_REGS_SPI1_C_MEM_1(), 0, 0), .owner = FLASH_SPIMEM_RETENTION_ENTRY },
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x02), SPI1_MEM_C_TIMING_CALI_REG,        SPI1_MEM_C_TIMING_CALI_REG,  N_REGS_SPI1_C_MEM_2(), 0, 0), .owner = FLASH_SPIMEM_RETENTION_ENTRY },

    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x04), DR_REG_FLASH_SPI0_BASE,            DR_REG_FLASH_SPI0_BASE,          N_REGS_SPI0_C_MEM_0(), 0, 0), .owner = FLASH_SPIMEM_RETENTION_ENTRY }, /* spi0_mem */
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x05), SPI_MEM_C_FMEM__PMS0_ATTR_REG,     SPI_MEM_C_FMEM__PMS0_ATTR_REG,   N_REGS_SPI0_C_MEM_1(), 0, 0), .owner = FLASH_SPIMEM_RETENTION_ENTRY },
    [5] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x06), SPI_MEM_C_CLOCK_GATE_REG,          SPI_MEM_C_CLOCK_GATE_REG,        N_REGS_SPI0_C_MEM_2(), 0, 0), .owner = FLASH_SPIMEM_RETENTION_ENTRY },
    [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x07), SPI_MEM_C_MMU_POWER_CTRL_REG,      SPI_MEM_C_MMU_POWER_CTRL_REG,    N_REGS_SPI0_C_MEM_3(), 0, 0), .owner = FLASH_SPIMEM_RETENTION_ENTRY },
    [7] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SPIMEM_LINK(0x08), SPI_MEM_C_TIMING_CALI_REG,         SPI_MEM_C_TIMING_CALI_UPDATE,    SPI_MEM_C_TIMING_CALI_UPDATE_M, 1, 0), .owner = FLASH_SPIMEM_RETENTION_ENTRY },
};
_Static_assert(ARRAY_SIZE(flash_spimem_regs_retention) == SPIMEM_FLASH_RETENTION_LINK_LEN, "Inconsistent Flash SPI Mem retention link length definitions");

#if CONFIG_SPIRAM
#define N_REGS_SPI0_S_MEM_0()     (((SPI_MEM_S_USER2_REG - SPI_MEM_S_CTRL_REG) / 4) + 1)
#define N_REGS_SPI0_S_MEM_1()     (((SPI_MEM_S_FSM_REG - SPI_MEM_S_MISC_REG) / 4) + 1)
#define N_REGS_SPI0_S_MEM_2()     (((SPI_MEM_S_SMEM_DDR_REG - SPI_MEM_S_INT_ENA_REG) / 4) + 1)
#define N_REGS_SPI0_S_MEM_3()     (((SPI_MEM_S_SMEM_DOUT_HEX_MODE_REG - SPI_MEM_S_ECC_CTRL_REG) / 4) + 1)
#define N_REGS_SPI0_S_MEM_4()     (((SPI_MEM_S_DPA_CTRL_REG - SPI_MEM_S_MMU_POWER_CTRL_REG) / 4) + 1)
#define N_REGS_SPI1_S_MEM_0()     ((SPI1_MEM_S_CACHE_FCTRL_REG - SPI1_MEM_S_CMD_REG) / 4 + 1)
#define N_REGS_SPI1_S_MEM_1()     (((SPI1_MEM_S_SUS_STATUS_REG - SPI1_MEM_S_FLASH_WAITI_CTRL_REG) / 4) + 1)
#define N_REGS_SPI1_S_MEM_2()     (((SPI1_MEM_S_DDR_REG - SPI1_MEM_S_INT_ENA_REG) / 4) + 1)
#define N_REGS_SPI1_S_MEM_3()     (1)
#define N_REGS_SPI1_S_MEM_4()     (1)

const regdma_entries_config_t psram_spimem_regs_retention[SPIMEM_PSRAM_RETENTION_LINK_LEN] = {
    [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SPIMEM_LINK(0x08),          HP_SYS_CLKRST_PERI_CLK_CTRL00_REG, HP_SYS_CLKRST_REG_PSRAM_CORE_CLK_EN, HP_SYS_CLKRST_REG_PSRAM_CORE_CLK_EN_M, 1, 0), .owner = ENTRY(0) }, // Enable PSRAM mspi core clock on restore
    [1] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SPIMEM_LINK(0x09),          HP_SYS_CLKRST_SOC_CLK_CTRL0_REG,   HP_SYS_CLKRST_REG_PSRAM_SYS_CLK_EN,  HP_SYS_CLKRST_REG_PSRAM_SYS_CLK_EN_M,  1, 0), .owner = ENTRY(0) }, // Enable PSRAM mspi core clock on restore
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x0a),     SPI_MEM_S_CTRL_REG,                SPI_MEM_S_CTRL_REG,                  N_REGS_SPI0_S_MEM_0(), 0, 0), .owner = ENTRY(0) }, /* spi0_mem */
    [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x0b),     SPI_MEM_S_MISC_REG,                SPI_MEM_S_MISC_REG,                  N_REGS_SPI0_S_MEM_1(), 0, 0), .owner = ENTRY(0) },
    [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x0c),     SPI_MEM_S_INT_ENA_REG,             SPI_MEM_S_INT_ENA_REG,               N_REGS_SPI0_S_MEM_2(), 0, 0), .owner = ENTRY(0) },
    [5] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x0d),     SPI_MEM_S_ECC_CTRL_REG,            SPI_MEM_S_ECC_CTRL_REG,              N_REGS_SPI0_S_MEM_3(), 0, 0), .owner = ENTRY(0) },
    [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x0e),     SPI_MEM_S_MMU_POWER_CTRL_REG,      SPI_MEM_S_MMU_POWER_CTRL_REG,        N_REGS_SPI0_S_MEM_4(), 0, 0), .owner = ENTRY(0) },
    [7] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x0f),     SPI1_MEM_S_CMD_REG,                SPI1_MEM_S_CMD_REG,                  N_REGS_SPI1_S_MEM_0(), 0, 0), .owner = ENTRY(0) }, /* spi1_mem */
    [8] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x10),     SPI1_MEM_S_FLASH_WAITI_CTRL_REG,   SPI1_MEM_S_FLASH_WAITI_CTRL_REG,     N_REGS_SPI1_S_MEM_1(), 0, 0), .owner = ENTRY(0) },
    [9] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x11),     SPI1_MEM_S_INT_ENA_REG,            SPI1_MEM_S_INT_ENA_REG,              N_REGS_SPI1_S_MEM_2(), 0, 0), .owner = ENTRY(0) },
    [10] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x12),    SPI1_MEM_S_TIMING_CALI_REG,        SPI1_MEM_S_TIMING_CALI_REG,          N_REGS_SPI1_S_MEM_3(), 0, 0), .owner = ENTRY(0) },
    [11] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x13),    SPI1_MEM_S_CLOCK_GATE_REG,         SPI1_MEM_S_CLOCK_GATE_REG,           N_REGS_SPI1_S_MEM_4(), 0, 0), .owner = ENTRY(0) },
    [12] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SPIMEM_LINK(0x14),         HP_SYS_CLKRST_PERI_CLK_CTRL00_REG, 0,                                   HP_SYS_CLKRST_REG_PSRAM_CORE_CLK_EN_M, 0, 1), .owner = ENTRY(0) }, // Disable PSRAM mspi core clock on backup
    [13] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SPIMEM_LINK(0x15),         HP_SYS_CLKRST_SOC_CLK_CTRL0_REG,   0,                                   HP_SYS_CLKRST_REG_PSRAM_SYS_CLK_EN_M,  0, 1), .owner = ENTRY(0) }, // Disable PSRAM mspi core clock on backup
};

_Static_assert(ARRAY_SIZE(psram_spimem_regs_retention) == SPIMEM_PSRAM_RETENTION_LINK_LEN, "Inconsistent PSRAM SPI Mem retention link length definitions");
#endif

/* Systimer Registers Context */
#define N_REGS_SYSTIMER_0()     (((SYSTIMER_TARGET2_CONF_REG - SYSTIMER_TARGET0_HI_REG) / 4) + 1)
const regdma_entries_config_t systimer_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x00), SYSTIMER_UNIT0_OP_REG,       SYSTIMER_TIMER_UNIT0_UPDATE_M,    SYSTIMER_TIMER_UNIT0_UPDATE_M,    0, 1), .owner = ENTRY(0) },      /* Systimer */
    [1]  = { .config = REGDMA_LINK_WAIT_INIT(REGDMA_SYSTIMER_LINK(0x01), SYSTIMER_UNIT0_OP_REG,       SYSTIMER_TIMER_UNIT0_VALUE_VALID, SYSTIMER_TIMER_UNIT0_VALUE_VALID, 0, 1), .owner = ENTRY(0) },
    [2]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x02), SYSTIMER_UNIT0_VALUE_HI_REG, SYSTIMER_UNIT0_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) },
    [3]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x03), SYSTIMER_UNIT0_LOAD_REG,     SYSTIMER_TIMER_UNIT0_LOAD_M,      SYSTIMER_TIMER_UNIT0_LOAD_M,      1, 0), .owner = ENTRY(0) },
    [4]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x04), SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_UPDATE_M,    SYSTIMER_TIMER_UNIT1_UPDATE_M,    0, 1), .owner = ENTRY(0) },
    [5]  = { .config = REGDMA_LINK_WAIT_INIT(REGDMA_SYSTIMER_LINK(0x05), SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_VALUE_VALID, SYSTIMER_TIMER_UNIT1_VALUE_VALID, 0, 1), .owner = ENTRY(0) },
    [6]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x06), SYSTIMER_UNIT1_VALUE_HI_REG, SYSTIMER_UNIT1_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) },
    [7]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x07), SYSTIMER_UNIT1_LOAD_REG,     SYSTIMER_TIMER_UNIT1_LOAD_M,      SYSTIMER_TIMER_UNIT1_LOAD_M,      1, 0), .owner = ENTRY(0) },
    [8]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x08), SYSTIMER_TARGET0_HI_REG,     SYSTIMER_TARGET0_HI_REG,          N_REGS_SYSTIMER_0(),              0, 0), .owner = ENTRY(0) }, /* Systimer target value & period */
    [9]  = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x09), SYSTIMER_COMP0_LOAD_REG,     SYSTIMER_TIMER_COMP0_LOAD,        SYSTIMER_TIMER_COMP0_LOAD,        1, 0), .owner = ENTRY(0) },
    [10] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x0a), SYSTIMER_COMP1_LOAD_REG,     SYSTIMER_TIMER_COMP1_LOAD,        SYSTIMER_TIMER_COMP1_LOAD,        1, 0), .owner = ENTRY(0) },
    [11] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x0b), SYSTIMER_COMP2_LOAD_REG,     SYSTIMER_TIMER_COMP2_LOAD,        SYSTIMER_TIMER_COMP2_LOAD,        1, 0), .owner = ENTRY(0) },
    [12] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x0c), SYSTIMER_TARGET0_CONF_REG,   0,                                SYSTIMER_TARGET0_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [13] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x0d), SYSTIMER_TARGET0_CONF_REG,   SYSTIMER_TARGET0_PERIOD_MODE_M,   SYSTIMER_TARGET0_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [14] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x0e), SYSTIMER_TARGET1_CONF_REG,   0,                                SYSTIMER_TARGET1_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [15] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x0f), SYSTIMER_TARGET1_CONF_REG,   SYSTIMER_TARGET1_PERIOD_MODE_M,   SYSTIMER_TARGET1_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
    [16] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SYSTIMER_LINK(0x10), SYSTIMER_TARGET2_CONF_REG,   0,                                SYSTIMER_TARGET2_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) },
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

/* PVT Registers Context */
#define N_REGS_PVT    (((PVT_COMB_PD_SITE3_UNIT0_VT1_CONF2_REG - DR_REG_PVT_MONITOR_BASE) / 4) + 1)
const regdma_entries_config_t pvt_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_PVT_LINK(0x00), DR_REG_PVT_MONITOR_BASE, DR_REG_PVT_MONITOR_BASE, N_REGS_PVT, 0, 0), .owner = ENTRY(0)},
};
_Static_assert(ARRAY_SIZE(pvt_regs_retention) == PVT_RETENTION_LINK_LEN, "Inconsistent PVT retention link length definitions");
