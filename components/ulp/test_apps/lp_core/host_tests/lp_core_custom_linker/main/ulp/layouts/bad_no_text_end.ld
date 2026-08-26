/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 *
 * NEGATIVE: omits LP_CORE_TEXT_END(). Built with CONFIG_ULP_LP_CORE_MEMPROT=y,
 * where lp_core_pmp.c needs the RX/RW boundary for the PMP split.
 * Must fail with the MEMPROT ASSERT.
 */
PROVIDE(_custom_linker_marker = 0xCAFEBABE);

MEMORY
{
    lp_ram(RWX) : ORIGIN = LP_CORE_USER_MEMORY_REGION_START, LENGTH = LP_CORE_USER_MEMORY_REGION_END - LP_CORE_USER_MEMORY_REGION_START
}

SECTIONS
{
    LP_CORE_TEXT_START(lp_ram)

    .text ALIGN(4):   { *(.text) *(.text*) } > lp_ram
    .rodata ALIGN(4): { *(.rodata) *(.rodata*) } > lp_ram

    /* Intentionally missing: LP_CORE_TEXT_END() */
    LP_CORE_DATA_START()

    .data ALIGN(4): { *(.data) *(.data*) *(.sdata) *(.sdata*) } > lp_ram
    .bss ALIGN(4):  { *(.bss) *(.bss*) *(.sbss) *(.sbss*) PROVIDE(end = .); } > lp_ram

    .custom_state ALIGN(4): { KEEP(*(.custom_state)) } > lp_ram

    LP_CORE_DATA_END()
}
