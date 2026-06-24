/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc.h"
#include "ld.common"
#if CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
#include "ld.hp_mem_defs"
#endif

#if CONFIG_ESP_ROM_HAS_LP_ROM
/* With LP-ROM memory layout is different due to LP ROM stack/data */
/* For P4 ECO5 we also reserve some RTC MEM at the first for MSPI workaround */
#define ULP_MEM_START_ADDRESS SOC_RTC_DRAM_LOW + RESERVE_RTC_MEM + MSPI_WORKAROUND_SIZE
#else
#define ULP_MEM_START_ADDRESS (SOC_RTC_DRAM_LOW)
#endif

#define ALIGN_DOWN(SIZE, AL)   (SIZE & ~(AL - 1))
/* Ensure the end where the shared memory starts is aligned to 8 bytes
   if updating this also update the same in ulp_lp_core_memory_shared.c
 */
#define ALIGNED_COPROC_MEM ALIGN_DOWN(CONFIG_ULP_COPROC_RESERVE_MEM, 0x8)

ENTRY(reset_vector)

MEMORY
{
    /* First 128 bytes for exception/interrupt vectors */
    vector_table(RX) :   ORIGIN = ULP_MEM_START_ADDRESS, LENGTH = 0x80
    lp_ram(RWX) :        ORIGIN = ULP_MEM_START_ADDRESS + 0x80, LENGTH = ALIGNED_COPROC_MEM - 0x80 - CONFIG_ULP_SHARED_MEM
    shared_mem_ram(RW) : ORIGIN = ULP_MEM_START_ADDRESS + ALIGNED_COPROC_MEM - CONFIG_ULP_SHARED_MEM, LENGTH = CONFIG_ULP_SHARED_MEM
#if CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
    hp_ram(RWX) :        ORIGIN = ULP_HP_MEM_START, LENGTH = ULP_HP_MEM_SIZE
#endif
}

#if CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
REGION_ALIAS("default_app_seg", hp_ram);
#else
REGION_ALIAS("default_app_seg", lp_ram);
#endif

SECTIONS
{
    .vector.text :
    {
        /* Exception/interrupt vectors */
        __mtvec_base = .;
        KEEP (*(.init.vector .init.vector.*))
    } > vector_table

    . = ORIGIN(lp_ram);

    /* Interrupt/exception handlers stay in LP RAM (reachable on wakeup before HP SRAM is up). */
    .rtc_text ALIGN(4):
    {
        _lp_text_start = .;
        *(.text.vectors) /* Default reset vector must link to offset 0x80 */
        *(.text.handlers)
        *(.text.handlers.*)
    } > lp_ram

#if CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
    /* End of LP-resident code (handlers only); align to 128 bytes. */
    . = ALIGN(128);
    _lp_text_end = .;
    . = ORIGIN(hp_ram);
#endif

    .text ALIGN(4):
    {
        *(.text)
        *(.text*)
    } > default_app_seg

    .rodata ALIGN(4):
    {
        *(.rodata)
        *(.rodata*)
    } > default_app_seg

    /* 128-byte alignment required for PMP TOR granularity (SOC_CPU_PMP_REGION_GRANULARITY) */
    . = ALIGN(128);
#if !CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
    _lp_text_end = .;
#endif
    _lp_data_start = .;

    .data ALIGN(4):
    {
        _data_start = .;
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)
        _data_end = .;
    } > default_app_seg

    .bss ALIGN(4) :
    {
        _bss_start = .;
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        PROVIDE(end = .);
        _bss_end = .;
    } > default_app_seg

    __stack_top = ORIGIN(lp_ram) + LENGTH(lp_ram);

    . = ORIGIN(shared_mem_ram);
    .shared_mem (ALIGN(4)) :
    {
        KEEP(*(.shared_mem))
    } > shared_mem_ram
}
