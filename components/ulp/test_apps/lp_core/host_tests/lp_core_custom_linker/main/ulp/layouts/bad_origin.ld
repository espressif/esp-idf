/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 *
 * NEGATIVE: the region starts at LP_CORE_USER_MEMORY_REGION_START + 0x180
 * instead of LP_CORE_USER_MEMORY_REGION_START, so LP_CORE_TEXT_START places the
 * reset vector at the wrong address and the reset-vector position ASSERT fires.
 */
PROVIDE(_custom_linker_marker = 0xCAFEBABE);

MEMORY
{
    lp_ram(RWX) : ORIGIN = LP_CORE_USER_MEMORY_REGION_START + 0x180, LENGTH = LP_CORE_USER_MEMORY_REGION_END - LP_CORE_USER_MEMORY_REGION_START - 0x180
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
}
