/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 *
 * POSITIVE: esp-bist-style layout that takes ownership of the stack window.
 * It ends its data with a NOLOAD stack-overflow guard band, marks the data end
 * with _lp_data_end, and overrides both __stack_top and __stack_size (the size
 * being all the space left between the data end and the stack top). The
 * framework's bottom-bounds ASSERT confirms the stack does not reach the data.
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

    /* Stack-overflow guard band, kept out of the loadable image. */
    .stack_guard (NOLOAD):
    {
        . = ALIGN(4);
        . += 0x8;
        _stack_overflow_protection_start = .;
        . += 0x4;
        _stack_overflow_protection_end = .;
    } > lp_ram

    LP_CORE_DATA_END()

    /* Take ownership of the stack window: stack starts at the top of lp_ram and
       may grow down over all the remaining free space above the data end. */
    __stack_top  = ORIGIN(lp_ram) + LENGTH(lp_ram);
    __stack_size = __stack_top - _lp_data_end;
}
