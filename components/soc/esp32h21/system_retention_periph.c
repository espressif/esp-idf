/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include "soc/regdma.h"
#include "soc/system_periph_retention.h"
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
#define N_REGS_INTR_MATRIX()    (((INTMTX_CORE0_CLOCK_GATE_REG - DR_REG_INTMTX_BASE) / 4) + 1)
const regdma_entries_config_t intr_matrix_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0), DR_REG_INTMTX_BASE, DR_REG_INTMTX_BASE, N_REGS_INTR_MATRIX(), 0, 0), .owner = ENTRY(0) }  /* intr matrix */
};
_Static_assert(ARRAY_SIZE(intr_matrix_regs_retention) == INT_MTX_RETENTION_LINK_LEN, "Inconsistent INT_MTX retention link length definitions");

/* HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG / HP_SYSTEM_SRAM_USAGE_CONF_REG
 * HP_SYSTEM_SEC_DPA_CONF_REG / HP_SYSTEM_CPU_PERI_TIMEOUT_CONF_REG
 * HP_SYSTEM_HP_PERI_TIMEOUT_CONF_REG /HP_SYSTEM_ROM_TABLE_LOCK_REG /HP_SYSTEM_ROM_TABLE_REG
 */
#define HP_SYSTEM_RETENTION_REGS_CNT 7
#define HP_SYSTEM_RETENTION_REGS_BASE (HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG)
static const uint32_t hp_system_regs_map[4] = {0x64f, 0x0, 0x0, 0x0};
const regdma_entries_config_t hp_system_regs_retention[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_HPSYS_LINK(0x00), \
                                            HP_SYSTEM_RETENTION_REGS_BASE, HP_SYSTEM_RETENTION_REGS_BASE, \
                                            HP_SYSTEM_RETENTION_REGS_CNT, 0, 0, \
                                            hp_system_regs_map[0], hp_system_regs_map[1], \
                                            hp_system_regs_map[2], hp_system_regs_map[3]), \
        .owner = ENTRY(0)
    }, \
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

/* IO MUX Registers Context */
#define N_REGS_IOMUX_0()    (SOC_GPIO_PIN_COUNT)
#define N_REGS_IOMUX_1()    (SOC_GPIO_PIN_COUNT)
#define N_REGS_IOMUX_2()    (SOC_GPIO_PIN_COUNT)

/*
 * GPIO FUNCx_IN_SEL_CFG selective backup (same mask layout as esp32h4, extended for FUNC118..124).
 */
#define GPIO_IN_SEL_CFG_RETENTION_REGS_CNT  91
#define GPIO_IN_SEL_CFG_RETENTION_MAP_BASE  GPIO_FUNC0_IN_SEL_CFG_REG
static const uint32_t gpio_in_sel_cfg_regs_map[4] = {0xF00BFFC1, 0x81FFE70F, 0xFF87E3FF, 0x1FFFFFFE};

/*
 * GPIO_OUT_REG / GPIO_ENABLE_REG / GPIO_STATUS_REG backup; restore via W1TS registers (popcount = 3).
 */
#define GPIO_RETENTION_REGS_CNT  3
#define GPIO_BACKUP_MAP_BASE  GPIO_OUT_REG
#define GPIO_RETENTION_MAP_BASE  GPIO_OUT_W1TS_REG
static const uint32_t gpio_regs_map[4] = {0x10001001, 0x0, 0x0, 0x0};

const regdma_entries_config_t iomux_regs_retention[] = {
    [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x00), IO_MUX_GPIO0_REG,           IO_MUX_GPIO0_REG,           N_REGS_IOMUX_0(), 0, 0), .owner = ENTRY(0) }, /* io_mux */
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x01), GPIO_FUNC0_OUT_SEL_CFG_REG, GPIO_FUNC0_OUT_SEL_CFG_REG, N_REGS_IOMUX_1(), 0, 0), .owner = ENTRY(0) },
    [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x02), GPIO_PIN0_REG,              GPIO_PIN0_REG,              N_REGS_IOMUX_2(), 0, 0), .owner = ENTRY(0) },
    [3] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_IOMUX_LINK(0x03), GPIO_IN_SEL_CFG_RETENTION_MAP_BASE, GPIO_IN_SEL_CFG_RETENTION_MAP_BASE, GPIO_IN_SEL_CFG_RETENTION_REGS_CNT, 0, 0,
                                            gpio_in_sel_cfg_regs_map[0], gpio_in_sel_cfg_regs_map[1], gpio_in_sel_cfg_regs_map[2], gpio_in_sel_cfg_regs_map[3]), .owner = ENTRY(0)
    },
    [4] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_IOMUX_LINK(0x04), GPIO_BACKUP_MAP_BASE,             GPIO_RETENTION_MAP_BASE,          GPIO_RETENTION_REGS_CNT,             0, 0,
                                            gpio_regs_map[0], gpio_regs_map[1], gpio_regs_map[2], gpio_regs_map[3]),                                               .owner = ENTRY(0)
    },
};
_Static_assert(ARRAY_SIZE(iomux_regs_retention) == IOMUX_RETENTION_LINK_LEN, "Inconsistent IOMUX retention link length definitions");

/* Memory SPI Registers Context */
/*
 * SPI1 addr_map @ SPI_MEM_CMD_REG(1), 20 words:
 *   SPI_MEM_CMD/ADDR/CTRL/CTRL1/CTRL2/CLOCK/USER/USER1/USER2/MOSI_DLEN/MISO_DLEN/RD_STATUS/
 *   MISC/CACHE_FCTRL/FLASH_WAITI_CTRL/FLASH_SUS_CTRL/FLASH_SUS_CMD/SUS_STATUS/
 *   FLASH_WAITI_CTRL1/INT_ENA
 */
#define SPI1_MEM_RETENTION_REGS_CNT_0     (20)
#define SPI1_MEM_RETENTION_MAP_BASE_0     SPI_MEM_CMD_REG(1)
static const uint32_t spi1_mem_regs_map_0[4] = {0x0000AFFF, 0x00010BC0, 0x00000000, 0x00000000};

/* SPI1 SPI_MEM_CLOCK_GATE_REG(1), 1 word */
#define SPI1_MEM_RETENTION_REGS_CNT_1     (1)
#define SPI1_MEM_RETENTION_MAP_BASE_1     SPI_MEM_CLOCK_GATE_REG(1)

/*
 * SPI0 addr_map @ SPI_MEM_CMD_REG(0), 56 words:
 *   SPI_MEM_CMD/ADDR/CTRL/CTRL1/CTRL2/CLOCK/USER/USER1/USER2/MISC/CACHE_FCTRL/SRAM_CMD/FSM/INT_ENA,
 *   SPI_FMEM_PMS0..3_ATTR/ADDR/SIZE, SPI_SMEM_PMS0..3_ATTR/ADDR/SIZE,
 *   SPI_MEM_PMS_REJECT/ECC_CTRL/SMEM_ECC_CTRL/AXI_ERR_RESP_EN,
 *   SPI_MEM_TIMING_CALI/DIN_MODE/DIN_NUM/DOUT_MODE,
 *   SPI_SMEM_TIMING_CALI/DIN_MODE/DIN_NUM/DOUT_MODE/SMEM_AC
 * (excludes SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG)
 */
#define SPI0_MEM_RETENTION_REGS_CNT_0     (56)
#define SPI0_MEM_RETENTION_MAP_BASE_0     SPI_MEM_CMD_REG(0)
static const uint32_t spi0_mem_regs_map_0[4] = {0x003FA9FF, 0x00010000, 0xA5FFFFFF, 0x000001FF};

/*
 * SPI0 addr_map @ SPI_MEM_CLOCK_GATE_REG(0), 10 words:
 *   SPI_MEM_CLOCK_GATE_REG,
 *   SPI_MEM_NAND_FLASH_SPI_SEQ0_REG,
 *   SPI_MEM_XTS_PLAIN_BASE_REG,
 *   SPI_MEM_XTS_LINESIZE/DESTINATION/PHYSICAL_ADDRESS/DATE,
 *   SPI_MEM_MMU_POWER_CTRL/DPA_CTRL/XTS_PSEUDO_ROUND_CONF
 */
#define SPI0_MEM_RETENTION_REGS_CNT_1     (10)
#define SPI0_MEM_RETENTION_MAP_BASE_1     SPI_MEM_CLOCK_GATE_REG(0)
static const uint32_t spi0_mem_regs_map_1[4] = {0x00000001, 0x00000001, 0x00870001, 0x0000000E};

const regdma_entries_config_t flash_spimem_regs_retention[] = {
    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_SPIMEM_LINK(0x00), SPI1_MEM_RETENTION_MAP_BASE_0, SPI1_MEM_RETENTION_MAP_BASE_0, SPI1_MEM_RETENTION_REGS_CNT_0, 0, 0,
                                            spi1_mem_regs_map_0[0], spi1_mem_regs_map_0[1], spi1_mem_regs_map_0[2], spi1_mem_regs_map_0[3]), .owner = ENTRY(0)
    },
    [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x01), SPI1_MEM_RETENTION_MAP_BASE_1,    SPI1_MEM_RETENTION_MAP_BASE_1,    SPI1_MEM_RETENTION_REGS_CNT_1, 0, 0), .owner = ENTRY(0) },
    /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
    [2] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_SPIMEM_LINK(0x02), SPI0_MEM_RETENTION_MAP_BASE_0, SPI0_MEM_RETENTION_MAP_BASE_0, SPI0_MEM_RETENTION_REGS_CNT_0, 0, 0,
                                            spi0_mem_regs_map_0[0], spi0_mem_regs_map_0[1], spi0_mem_regs_map_0[2], spi0_mem_regs_map_0[3]), .owner = ENTRY(0)
    },
    [3] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_SPIMEM_LINK(0x03), SPI0_MEM_RETENTION_MAP_BASE_1, SPI0_MEM_RETENTION_MAP_BASE_1, SPI0_MEM_RETENTION_REGS_CNT_1, 0, 0,
                                            spi0_mem_regs_map_1[0], spi0_mem_regs_map_1[1], spi0_mem_regs_map_1[2], spi0_mem_regs_map_1[3]), .owner = ENTRY(0)
    },    /* Note: spimem register should set update reg to make the configuration take effect */
    [4] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_SPIMEM_LINK(0x04), SPI_MEM_TIMING_CALI_REG(0),    SPI_MEM_TIMING_CALI_UPDATE,    SPI_MEM_TIMING_CALI_UPDATE_M, 1, 0), .owner = ENTRY(0) },
};
_Static_assert(ARRAY_SIZE(flash_spimem_regs_retention) == SPIMEM_RETENTION_LINK_LEN, "Inconsistent SPI Mem retention link length definitions");

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
