/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 *
 * POSITIVE: multi-region layout. Splits the window into custom-named regions
 * using only LP_CORE_USER_MEMORY_REGION_START / _END. __stack_top defaults to
 * the top of the window (the data region reaches it).
 */
PROVIDE(_custom_linker_marker = 0xCAFEBABE);

_fixed_addr = LP_CORE_USER_MEMORY_REGION_START + 0x700;

MEMORY
{
    code(RWX)  : ORIGIN = LP_CORE_USER_MEMORY_REGION_START, LENGTH = _fixed_addr - LP_CORE_USER_MEMORY_REGION_START
    fixed(RWX) : ORIGIN = _fixed_addr,                      LENGTH = 0x100
    data(RW)   : ORIGIN = _fixed_addr + 0x100,              LENGTH = LP_CORE_USER_MEMORY_REGION_END - (_fixed_addr + 0x100)
}

SECTIONS
{
    LP_CORE_TEXT_START(code)

    .text ALIGN(4):   { *(.text) *(.text*) } > code
    .rodata ALIGN(4): { *(.rodata) *(.rodata*) } > code

    LP_CORE_TEXT_END()

    . = ORIGIN(fixed);
    .fixed ALIGN(4): { KEEP(*(.fixed_region .fixed_region.*)) } > fixed

    . = ORIGIN(data);
    LP_CORE_DATA_START()

    .data ALIGN(4): { *(.data) *(.data*) *(.sdata) *(.sdata*) } > data
    .bss ALIGN(4):  { *(.bss) *(.bss*) *(.sbss) *(.sbss*) PROVIDE(end = .); } > data

    .custom_state ALIGN(4): { KEEP(*(.custom_state)) } > data

    LP_CORE_DATA_END()
}
