/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/uart_periph.h"
#include "soc/gpio_periph.h"
#include "soc/soc.h"
#include "esp_gdbstub_common.h"
#include "sdkconfig.h"

#define UART_NUM CONFIG_ESP_CONSOLE_UART_NUM

#define GDBSTUB_MEM_REGION_COUNT 9

#define UART_REG_FIELD_LEN 0x84

typedef struct {
    intptr_t lower;
    intptr_t upper;
} mem_bound_t;

static const mem_bound_t mem_region_table [GDBSTUB_MEM_REGION_COUNT] =
{
    {SOC_DROM_LOW, SOC_DROM_HIGH},
    {SOC_IROM_LOW, SOC_IROM_HIGH},
    {SOC_IRAM_LOW, SOC_IRAM_HIGH},
    {SOC_DRAM_LOW, SOC_DRAM_HIGH},
    {SOC_IROM_MASK_LOW, SOC_IROM_MASK_HIGH},
    {SOC_DROM_MASK_LOW, SOC_DROM_MASK_HIGH},
    // We shouldn't read the uart registers since it will disturb the debugging via UART,
    // so skip UART part of the peripheral registers.
    {DR_REG_UART_BASE + UART_REG_FIELD_LEN, SOC_PERIPHERAL_HIGH},
    {SOC_DEBUG_LOW, SOC_DEBUG_HIGH},
};

static inline bool check_inside_valid_region(intptr_t addr)
{
    for (size_t i = 0; i < GDBSTUB_MEM_REGION_COUNT; i++) {
        if (addr >= mem_region_table[i].lower && addr < mem_region_table[i].upper) {
            return true;
        }
    }

    return false;
}

void esp_gdbstub_target_init()
{
}

//assume UART gdbstub channel

int esp_gdbstub_getchar()
{
    while (REG_GET_FIELD(UART_STATUS_REG(UART_NUM), UART_RXFIFO_CNT) == 0) {
        ;
    }
    return REG_READ(UART_FIFO_AHB_REG(UART_NUM));
}

void esp_gdbstub_putchar(int c)
{
    while (REG_GET_FIELD(UART_STATUS_REG(UART_NUM), UART_TXFIFO_CNT) >= 126) {
        ;
    }
    REG_WRITE(UART_FIFO_AHB_REG(UART_NUM), c);
}

void esp_gdbstub_flush()
{
    //not needed for uart
}

int esp_gdbstub_readmem(intptr_t addr)
{
    if (!check_inside_valid_region(addr)) {
        /* see esp_cpu_configure_region_protection */
        return -1;
    }
    uint32_t val_aligned = *(uint32_t *)(addr & (~3));
    uint32_t shift = (addr & 3) * 8;
    return (val_aligned >> shift) & 0xff;
}

int esp_gdbstub_writemem(unsigned int addr, unsigned char data)
{
    if (!check_inside_valid_region(addr)) {
        /* see esp_cpu_configure_region_protection */
        return -1;
    }

    /* 'addr' may be pointing at the memory which does not allow for
     * byte access, such as IRAM.
     * Perform a word-aligned read-modify-write, instead of writing
     * the byte directly.
     */
    unsigned *addr_aligned = (unsigned *)(addr & (~3));
    const uint32_t bit_offset = (addr & 0x3) * 8;
    const uint32_t mask = ~(0xff << bit_offset);
    *addr_aligned = (*addr_aligned & mask) | (data << bit_offset);
    return 0;
}
