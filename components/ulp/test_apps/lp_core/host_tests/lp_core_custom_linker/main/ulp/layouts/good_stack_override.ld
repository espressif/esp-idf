/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 *
 * POSITIVE: overrides __stack_top and __stack_size to values that differ from
 * the base defaults (default __stack_top is the window end, default
 * __stack_size is all the free space above the data). The stack top is pulled
 * 0x40 below the window end and the size is pinned to 0x200, so the test can
 * confirm the layout's assignments win over the base PROVIDE() defaults.
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

    LP_CORE_TEXT_END()
    LP_CORE_DATA_START()

    .data ALIGN(4): { *(.data) *(.data*) *(.sdata) *(.sdata*) } > lp_ram
    .bss ALIGN(4):  { *(.bss) *(.bss*) *(.sbss) *(.sbss*) PROVIDE(end = .); } > lp_ram

    .custom_state ALIGN(4): { KEEP(*(.custom_state)) } > lp_ram

    LP_CORE_DATA_END()

    /* Distinct, valid overrides: reserve 0x40 at the top of the window and cap
       the stack at 0x200 instead of taking all the remaining space. */
    __stack_top  = LP_CORE_USER_MEMORY_REGION_END - 0x40;
    __stack_size = 0x200;
}
