/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/reg_base.h"

#define HP_SYSTEM_CORE_ERR_RESP_DIS_REG     (DR_REG_HP_SYS_BASE + 0x1a4)

/* Clock related */
#define DR_REG_LP_CLKRST_BASE               (DR_REG_LPAON_BASE + 0x1000)
#define LP_CLKRST_HPCPU_RESET_CTRL0_REG     (DR_REG_LP_CLKRST_BASE + 0x14)
#define LP_CLKRST_HPCORE0_STAT_VECTOR_SEL   (1 << 15)

#define HP_SYS_CLKRST_HP_RST_EN0_REG        (DR_REG_HP_SYS_CLKRST_BASE + 0xc0)
#define HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI    (1 << 22)
#define HP_SYS_CLKRST_REG_RST_EN_MSPI_APB    (1 << 24)


/* SPIMEM related */
#define DR_REG_FLASH_SPI0_BASE              (DR_REG_HPPERIPH0_BASE + 0x8C000)
#define SPI_MEM_C_CACHE_FCTRL_REG           (DR_REG_FLASH_SPI0_BASE + 0x3c)
#define SPI_MEM_C_CLOSE_AXI_INF_EN          (1 << 31)
#define SPI_MEM_C_AXI_REQ_EN                (1 << 0)

#define SPI_MEM_C_MMU_ITEM_INDEX_REG        (DR_REG_FLASH_SPI0_BASE + 0x380)
#define SPI_MEM_C_MMU_ITEM_CONTENT_REG      (DR_REG_FLASH_SPI0_BASE + 0x37c)

.macro REG_SET_BIT addr, value
    li a0, \addr
    li a1, \value
    lw a2, (a0)
    or a2, a2, a1
    sw a2, (a0)
.endm

.macro REG_CLR_BIT addr, value
    li a0, \addr
    /* Since all our parameters will be constants, we can pre-calculate it at assemble time */
    li a1, ~\value
    lw a2, (a0)
    and a2, a2, a1
    sw a2, (a0)
.endm

.macro REG_WRITE addr, value
    li a0, \addr
    li a1, \value
    sw a1, (a0)
.endm

.macro REG_READ addr
    li  a0, \addr
    lw  a1, (a0)
.endm

.macro DELAY_US us
    li      t3, (40 * \us)     /* CPU @40MHz after reset */
    csrr    t0, cycle
    add     t1, t0, t3
1:  csrr    t2, cycle
    blt     t2, t1, 1b
.endm

/**
 * @brief Workaround for MSPI issues on ESP32-P4 revision 3
 *
 * This function implements a workaround for MSPI-related issues on ESP32-P4 revision 3.
 * It performs 2 flash dummy reads to stabilize the MSPI functionality before jumping to
 * ROM code after deepsleep wakeup.
 */
.global p4_rev3_mspi_workaround
.section .p4_rev3_mspi_workaround.rtc_text,"ax"

p4_rev3_mspi_workaround:
    # Recover the reset vector to HP ROM
    REG_SET_BIT LP_CLKRST_HPCPU_RESET_CTRL0_REG, LP_CLKRST_HPCORE0_STAT_VECTOR_SEL

    # Clear the bit to close AXI interface and then set the AXI request enable bit
    REG_CLR_BIT SPI_MEM_C_CACHE_FCTRL_REG, SPI_MEM_C_CLOSE_AXI_INF_EN
    REG_SET_BIT SPI_MEM_C_CACHE_FCTRL_REG, SPI_MEM_C_AXI_REQ_EN

    # Set 1 mspi mmu entry for axi addr to flash addr
    REG_WRITE SPI_MEM_C_MMU_ITEM_INDEX_REG, 0
    REG_WRITE SPI_MEM_C_MMU_ITEM_CONTENT_REG, 0x1000

    # Disable cpu get error response
    REG_WRITE HP_SYSTEM_CORE_ERR_RESP_DIS_REG, 0x7

    # Perform dummy reads
    REG_READ 0x80000000
    # Perform dummy reads again
    REG_READ 0x80000080

    # Delay 1us to wait MSPI read transmission done
    DELAY_US 1

    # Enable cpu get error response
    REG_WRITE HP_SYSTEM_CORE_ERR_RESP_DIS_REG, 0

    # Reset MSPI AXI and APB interfaces
    REG_SET_BIT HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI
    REG_SET_BIT HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_APB
    REG_CLR_BIT HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI
    REG_CLR_BIT HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_APB

    # Jump to HP ROM first stage boot code
    li  a5, 0x4fc00000
    jr  a5
