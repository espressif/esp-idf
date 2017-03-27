#include "sdkconfig.h"

#define ULP_BIN_MAGIC		0x00706c75
#define HEADER_SIZE			12
MEMORY
{
    ram(RW) : ORIGIN = 0, LENGTH = CONFIG_ULP_COPROC_RESERVE_MEM
}

SECTIONS
{
    .text : AT(HEADER_SIZE)
    {
        *(.text)
    } >ram
    .data :
    {
        . = ALIGN(4);
        *(.data)
    } >ram
    .bss :
    {
        . = ALIGN(4);
        *(.bss)
    } >ram
    
    .header : AT(0)
    {
        LONG(ULP_BIN_MAGIC)
        SHORT(LOADADDR(.text)) 
        SHORT(SIZEOF(.text))
        SHORT(SIZEOF(.data))
        SHORT(SIZEOF(.bss))
    }
}
