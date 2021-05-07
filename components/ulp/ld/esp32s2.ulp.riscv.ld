#include "sdkconfig.h"

ENTRY(reset_vector)

MEMORY
{
    ram(RW) : ORIGIN = 0, LENGTH = CONFIG_ESP32S2_ULP_COPROC_RESERVE_MEM
}

SECTIONS
{
    . = ORIGIN(ram);
    .text :
    {
        *start.S.obj(.text.vectors) /* Default reset vector must link to offset 0x0 */
        *(.text)
        *(.text*)
    } >ram

    .rodata ALIGN(4):
    {
        *(.rodata)
        *(.rodata*)
    } > ram

    .data ALIGN(4):
    {
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)
    } > ram

    .bss ALIGN(4) :
    {
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
    } >ram

    __stack_top = ORIGIN(ram) + LENGTH(ram);
}
