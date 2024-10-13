/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/** Simplified memory map for the bootloader.
 *  Make sure the bootloader can load into main memory without overwriting itself.
 *
 *  ESP32-H2 ROM static data usage is as follows:
 *  - 0x4083ba78 - 0x4084d380: Shared buffers, used in UART/USB/SPI download mode only
 *  - 0x4084d380 - 0x4084f380: PRO CPU stack, can be reclaimed as heap after RTOS startup
 *  - 0x4084f380 - 0x4084fee0: ROM .bss and .data used in startup code or nonos/early boot (can be freed when IDF runs)
 *  - 0x4084fee0 - 0x40850000: ROM .bss and .data used in startup code and when IDF runs (cannot be freed)
 *
 *  The 2nd stage bootloader can take space up to the end of ROM shared
 *  buffers area (0x4084d380).
 */

/* We consider 0x3fcdc710 to be the last usable address for 2nd stage bootloader stack overhead, dram_seg,
 * and work out iram_seg and iram_loader_seg addresses from there, backwards.
 */

/* These lengths can be adjusted, if necessary: */
bootloader_usable_dram_end = 0x4084cfd0;
bootloader_stack_overhead = 0x2000; /* For safety margin between bootloader data section and startup stacks */
bootloader_dram_seg_len = 0x5000;
bootloader_iram_loader_seg_len = 0x7000;
bootloader_iram_seg_len = 0x2500;

/* Start of the lower region is determined by region size and the end of the higher region */
bootloader_dram_seg_end = bootloader_usable_dram_end - bootloader_stack_overhead;
bootloader_dram_seg_start = bootloader_dram_seg_end - bootloader_dram_seg_len;
bootloader_iram_loader_seg_start = bootloader_dram_seg_start - bootloader_iram_loader_seg_len;
bootloader_iram_seg_start = bootloader_iram_loader_seg_start - bootloader_iram_seg_len;

MEMORY
{
  iram_seg (RWX) :                  org = bootloader_iram_seg_start, len = bootloader_iram_seg_len
  iram_loader_seg (RWX) :           org = bootloader_iram_loader_seg_start, len = bootloader_iram_loader_seg_len
  dram_seg (RW) :                   org = bootloader_dram_seg_start, len = bootloader_dram_seg_len
}

/* The app may use RAM for static allocations up to the start of iram_loader_seg.
 * If you have changed something above and this assert fails:
 * 1. Check what the new value of bootloader_iram_loader_seg start is.
 * 2. Update the value in this assert.
 * 3. Update SRAM_DRAM_END in components/esp_system/ld/esp32h2/memory.ld.in to the same value.
 */
ASSERT(bootloader_iram_loader_seg_start == 0x4083EFD0, "bootloader_iram_loader_seg_start inconsistent with SRAM_DRAM_END");

/* Default entry point: */
ENTRY(call_start_cpu0);

SECTIONS
{

  .iram_loader.text :
  {
    . = ALIGN (16);
    _loader_text_start = ABSOLUTE(.);
    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.iram1 .iram1.*) /* catch stray IRAM_ATTR */
    *liblog.a:(.literal .text .literal.* .text.*)
    /* we use either libgcc or compiler-rt, so put similar entries for them here */
    *libgcc.a:(.literal .text .literal.* .text.*)
    *libclang_rt.builtins.a:(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_clock_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_common_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_flash.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_random.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_random*.*(.literal.bootloader_random_disable .text.bootloader_random_disable)
    *libbootloader_support.a:bootloader_efuse.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_utility.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_sha.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_console_loader.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_panic.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:bootloader_soc.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:esp_image_format.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_encrypt.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_encryption_secure_features.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:flash_partitions.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot_secure_features.*(.literal .text .literal.* .text.*)
    *libbootloader_support.a:secure_boot_signatures_bootloader.*(.literal .text .literal.* .text.*)
    *libmicro-ecc.a:*.*(.literal .text .literal.* .text.*)
    *libspi_flash.a:*.*(.literal .text .literal.* .text.*)
    *libhal.a:wdt_hal_iram.*(.literal .text .literal.* .text.*)
    *libhal.a:mmu_hal.*(.literal .text .literal.* .text.*)
    *libhal.a:cache_hal.*(.literal .text .literal.* .text.*)
    *libhal.a:efuse_hal.*(.literal .text .literal.* .text.*)
    *libesp_hw_support.a:rtc_clk.*(.literal .text .literal.* .text.*)
    *libesp_hw_support.a:rtc_time.*(.literal .text .literal.* .text.*)
    *libesp_hw_support.a:regi2c_ctrl.*(.literal .text .literal.* .text.*)
    *libefuse.a:*.*(.literal .text .literal.* .text.*)
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _loader_text_end = ABSOLUTE(.);
  } > iram_loader_seg

  .iram.text :
  {
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
  } > iram_seg


  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _dram_start = ABSOLUTE(.);
    _bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } > dram_seg

  .dram0.bootdesc : ALIGN(0x10)
  {
    _data_start = ABSOLUTE(.);
    *(.data_bootloader_desc .data_bootloader_desc.*)               /* Should be the first.  Bootloader version info.        DO NOT PUT ANYTHING BEFORE IT! */
  } > dram_seg

  .dram0.data :
  {
    *(.dram1 .dram1.*) /* catch stray DRAM_ATTR */
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    _data_end = ABSOLUTE(.);
  } > dram_seg

  .dram0.rodata :
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    *(.sdata2 .sdata2.* .srodata .srodata.*)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    *(.eh_frame_hdr)
    *(.eh_frame)
    . = (. + 3) & ~ 3;
    /* C++ constructor and destructor tables, properly ordered: */
    __init_array_start = ABSOLUTE(.);
    KEEP (*crtbegin.*(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.*(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    _dram_end = ABSOLUTE(.);
  } > dram_seg

  .iram.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.literal .text .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.iram .iram.*) /* catch stray IRAM_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)

    /** CPU will try to prefetch up to 16 bytes of
      * of instructions. This means that any configuration (e.g. MMU, PMS) must allow
      * safe access to up to 16 bytes after the last real instruction, add
      * dummy bytes to ensure this
      */
    . += 16;

    _text_end = ABSOLUTE(.);
    _etext = .;
  } > iram_seg

  .riscv.attributes 0: { *(.riscv.attributes) }

  /* DWARF 1 */
  .debug              0 : { *(.debug) }
  .line               0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo      0 : { *(.debug_srcinfo) }
  .debug_sfnames      0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges      0 : { *(.debug_aranges) }
  .debug_pubnames     0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info         0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev       0 : { *(.debug_abbrev) }
  .debug_line         0 : { *(.debug_line) }
  .debug_frame        0 : { *(.debug_frame) }
  .debug_str          0 : { *(.debug_str) }
  .debug_loc          0 : { *(.debug_loc) }
  .debug_macinfo      0 : { *(.debug_macinfo) }
  .debug_pubtypes     0 : { *(.debug_pubtypes) }
  /* DWARF 3 */
  .debug_ranges       0 : { *(.debug_ranges) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames    0 : { *(.debug_weaknames) }
  .debug_funcnames    0 : { *(.debug_funcnames) }
  .debug_typenames    0 : { *(.debug_typenames) }
  .debug_varnames     0 : { *(.debug_varnames) }
  /* GNU DWARF 2 extensions */
  .debug_gnu_pubnames 0 : { *(.debug_gnu_pubnames) }
  .debug_gnu_pubtypes 0 : { *(.debug_gnu_pubtypes) }
  /* DWARF 4 */
  .debug_types        0 : { *(.debug_types) }
  /* DWARF 5 */
  .debug_addr         0 : { *(.debug_addr) }
  .debug_line_str     0 : { *(.debug_line_str) }
  .debug_loclists     0 : { *(.debug_loclists) }
  .debug_macro        0 : { *(.debug_macro) }
  .debug_names        0 : { *(.debug_names) }
  .debug_rnglists     0 : { *(.debug_rnglists) }
  .debug_str_offsets  0 : { *(.debug_str_offsets) }

  .comment 0 : { *(.comment) }
  .note.GNU-stack 0: { *(.note.GNU-stack) }

  /**
   * Discarding .rela.* sections results in the following mapping:
   * .rela.text.* -> .text.*
   * .rela.data.* -> .data.*
   * And so forth...
   */
  /DISCARD/ : { *(.rela.*) }

}

/**
 *  Appendix: Memory Usage of ROM bootloader
 *
 * 0x4083ba78 ------------------> _dram0_0_start
 *            |               |
 *            |               |
 *            |               |   1. Large buffers that are only used in certain boot modes, see shared_buffers.h
 *            |               |
 *            |               |
 * 0x4084d380 ------------------> __stack_sentry
 *            |               |
 *            |               |   2. Startup pro cpu stack (freed when IDF app is running)
 *            |               |
 * 0x4084f380 ------------------> __stack (pro cpu)
 *            |               |
 *            |               |
 *            |               |   3. Shared memory only used in startup code or nonos/early boot*
 *            |               |      (can be freed when IDF runs)
 *            |               |
 *            |               |
 * 0x4084fee0 ------------------> _dram0_rtos_reserved_start
 *            |               |
 *            |               |
 *            |               |   4. Shared memory used in startup code and when IDF runs
 *            |               |
 *            |               |
 * 0x4084ffc0 ------------------> _dram0_rtos_reserved_end
 *            |               |
 * 0x4084ffc8 ------------------> _data_start_interface
 *            |               |
 *            |               |   5. End of DRAM is the 'interface' data with constant addresses (ECO compatible)
 *            |               |
 * 0x40850000 ------------------> _data_end_interface
 */
