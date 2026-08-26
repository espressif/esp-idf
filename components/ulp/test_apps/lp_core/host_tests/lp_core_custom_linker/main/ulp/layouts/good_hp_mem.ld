/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 *
 * POSITIVE: custom layout under CONFIG_ULP_COPROC_RUN_FROM_HP_MEM. Keeps the
 * reset vector in LP RAM and routes the application to hp_ram, which is
 * declared by the base part. __stack_top defaults to the top of the window.
 */
PROVIDE(_custom_linker_marker = 0xCAFEBABE);

MEMORY
{
    lp_ram(RWX) : ORIGIN = LP_CORE_USER_MEMORY_REGION_START, LENGTH = LP_CORE_USER_MEMORY_REGION_END - LP_CORE_USER_MEMORY_REGION_START
}

SECTIONS
{
    LP_CORE_TEXT_START(lp_ram)

    . = ORIGIN(hp_ram);

    .text ALIGN(4):   { *(.text) *(.text*) } > hp_ram
    .rodata ALIGN(4): { *(.rodata) *(.rodata*) } > hp_ram

    LP_CORE_TEXT_END()
    LP_CORE_DATA_START()

    .data ALIGN(4): { *(.data) *(.data*) *(.sdata) *(.sdata*) } > hp_ram
    .bss ALIGN(4):  { *(.bss) *(.bss*) *(.sbss) *(.sbss*) PROVIDE(end = .); } > hp_ram

    .custom_state ALIGN(4): { KEEP(*(.custom_state)) } > hp_ram

    LP_CORE_DATA_END()
}
