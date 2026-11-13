/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Layout of the U-mode programs that umode_elf_add_program() links as position-independent shared objects.
 *
 * Each loadable segment starts on a page of its own, so that umode_elf_load() can map each with its own
 * permissions: code is executable, rodata is read-only, and the relocated data is writable. The program is
 * linked at address 0, umode_elf_load() relocates it to its load address.
 *
 * Passing a linker script also keeps the toolchain's default script for applications (picolibc.ld) out. */

ENTRY(_start)

PHDRS
{
    dyninfo PT_LOAD;
    text    PT_LOAD;
    rodata  PT_LOAD;
    data    PT_LOAD;
    dynamic PT_DYNAMIC;
}

SECTIONS
{
    . = SIZEOF_HEADERS;
    /* Only read while loading */
    .hash      : { *(.hash) } :dyninfo
    .gnu.hash  : { *(.gnu.hash) } :dyninfo
    .dynsym    : { *(.dynsym) } :dyninfo
    .dynstr    : { *(.dynstr) } :dyninfo
    .rela.dyn  : { *(.rela.*) } :dyninfo

    . = ALIGN(0x1000);
    .text      : { *(.text .text.*) } :text

    . = ALIGN(0x1000);
    .rodata    : { *(.rodata .rodata.* .srodata .srodata.*) } :rodata

    /* Pointer tables in .data.rel.ro are relocated at load time, so they go to the writable segment */
    . = ALIGN(0x1000);
    .data.rel.ro : { *(.data.rel.ro .data.rel.ro.*) } :data
    .dynamic   : { *(.dynamic) } :data :dynamic
    .got       : { *(.got .got.*) } :data
    .data      : { *(.data .data.* .sdata .sdata.*) } :data
    .bss       : { *(.bss .bss.* .sbss .sbss.* COMMON) } :data

    /DISCARD/  : { *(.note.*) *(.eh_frame .eh_frame_hdr .interp) }
}
